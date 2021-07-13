#!/usr/bin/env python3
PROLOG="""\
#IBM_PROLOG_BEGIN_TAG
#
# Copyright 2015,2018 IBM International Business Machines Corp.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#IBM_PROLOG_END_TAG
"""

"""
Generate most of the pyecmd code by parsing function headers out of the
C API header files.

@author Joachim Fenkes <fenkes@de.ibm.com>

Instructions:
 1. ./generate_pyecmd.py <all the header files you want to transform> > generated.py
 2. If some function is not as you'd expect, the lists at the beginning of this script
    allow some customization.
"""

import re, sys, os, textwrap

"""
Override the with of fixed input data functions from the default of 64
"""
special_input_widths = {
"putCfamRegister" : 32,
}

"""
Functions that return an uint32_t that is _not_ a return code and should be returned to the caller
"""
force_return_value_names = (
"flattenSize",
"simFusionRand32",
"hashString32"
)

"""
Functions to not export to Python
"""
non_exported_names = (
# manually wrapped by pyecmd
"loadDll",
"commandArgs",
"unloadDll",
"looperInit",
"existLooperNext",
"existLooperInit",
"looperNext",
"configLooperNext",
"configLooperInit",
"relatedTargets",
# aren't being converted correctly - FIXME
"parseTokens",
"queryHostMemInfoRanges",
# not necessary because there are Pythonic replacements
"queryErrorState",
"hexToUInt32",
"genB32FromHex",
"genB32FromHexLeft",
"genB32FromHexRight",
"decToUInt32",
)

"""
Functions with multiple overloaded definitions. Most of these can be handled by using the most
generic variant with parameter defaults, so we substitute modified prototypes. This doesn't work
for some; those have to be omitted via non_exported_names and implemented manually in base.py.
"""
overloaded_functions = {
"writeTarget":        "const ecmdChipTarget &i_target, ecmdTargetDisplayMode_t i_displayMode=ECMD_DISPLAY_TARGET_PLUGIN_MODE)",
"displayScomData":    "ecmdChipTarget &i_target, ecmdScomData &i_scomData, ecmdDataBuffer &i_data, const char *i_format, std::string *o_strData=NULL)",
# Patch o_ringFile to be i_ringFile so we generate correct Python wrapper code
"croFastLoadL2":      "ecmdChipTarget &i_target, croFastL2LoadModes &i_modes, ecmdMemoryEntryList &i_data, const char *i_ringFile=NULL)",
}

FUNC_GLOBAL = 0
FUNC_TARGETED = 1

def python_type(type):
    if   type[-2:] == "_t":           return "int"
    elif type == "bool":              return "bool"
    elif type == "char" \
      or type == "std::string":       return "str"
    elif type == "ecmdDataBuffer":    return "EcmdBitArray"
    elif type == "ecmdChipTarget":    return "Target"
    elif type[-4:] == "List":         return "list(" + python_type(type[0:-4]) + ")"
    elif type[-6:] == "Vector":       return "list(" + python_type(type[0:-6]) + ")"
    elif type[-3:] == "Map":
        parts = type[:-3].split("_", 1)
        return "dict(" + python_type(parts[0]) + ", " + python_type(parts[1]) + ")"
    else:                             return "ecmd." + type

class FunctionArgument(object):
    """
    Wrapper class for a single function argument, takes care of all type transformations
    """
    __slots__ = ("_type", "name", "def_value")

    def __init__(self, argstr):
        parts = argstr.split()
        self._type = parts[:-1]
        if "=" in parts[-1]:
            self.name, self.def_value = parts[-1].split("=")
        else:
            self.name = parts[-1]
            self.def_value = ""

    @property
    def type(self):
        return self._type[-1]

    @property
    def full_type(self):
        return " ".join(self._type)

    @property
    def name_with_def_value(self):
        result = self.name
        if self.def_value:
            result += "=" + ("ecmd." if re.match(r"[A-Z]+_", self.def_value) else "") + self.def_value
        return result

    @property
    def name_stripped(self):
        return re.sub(r"^(i|o|io)_", "", self.name)

    @property
    def is_input(self):
        return self.name[:2] == "i_"

    @property
    def is_output(self):
        return self.name[:2] == "o_"

    @property
    def is_inout(self):
        return self.name[:3] == "io_"

    @property
    def is_returned_out_arg(self):
        return self.type.endswith("_t") or self.full_type in ("std::string", "const char")

    @property
    def retval_construction(self):
        if   self.type == "std::string":       return '""'
        elif self.type == "std::stringVector": return "ecmd.stringVector()"
        elif self.type == "std::stringList":   return "ecmd.stringList()"
        elif self.type[-2:] == "_t":           return "0"
        elif self.type == "ecmdChipTarget":    return "Target()"
        else:                                  return "ecmd." + self.type.replace("std::string", "string") + "()"

    @property
    def retval_conversion(self):
        if   self.type == "ecmdDataBuffer":    return "base._from_ecmdDataBuffer(" + self.name + ")"
        elif self.type == "std::stringVector" \
          or self.type == "std::stringList":   return "list(" + self.name + ")"
        else:                                  return self.name

    @property
    def python_type(self):
        return python_type(self.type)

"""
Emit a wrapping Python function for an eCMD function, tailored towards the specific function
"""
def print_function(data, func_type):
    global cur_header

    (new_funcname, header, line1, comment, returntype, funcname, argstr) = data

    # indent target and buffer functions
    prefix = "" if func_type == FUNC_GLOBAL else "    "

    # if the header changes, print it
    if header != cur_header:
        cur_header = header
        print("")
        print(prefix + '# -------------------------------------------------------------------------')
        print(prefix + '# ' + header)
        print(prefix + '# -------------------------------------------------------------------------')

    # Pythonify and clean up the string of function arguments, then split into (type, name) tuples
    argstr = argstr.replace("NULL", "None").replace("true", "True").replace("false", "False").replace("void", "")
    argstr = re.sub(r"\).*$", "", argstr).strip(" \t\n()")

    args = argstr.split(",") if argstr else []
    args = [FunctionArgument(re.sub(r"(0)ULL$", r"\1", re.sub(r"[&*]|\[.*\]|ull$", "", arg.strip()))) for arg in args]

    # Remove the i_target parameter for target functions; we'll replace it by self
    if func_type == FUNC_TARGETED:
        if args[0].name != "i_target":
            raise ValueError("No i_target parameter in targeted function "+new_funcname)
        del args[0]

    calls_wrapped_method = returntype == "void"
    returns_wrapped_method = returntype not in ("uint32_t", "void") \
        or new_funcname in force_return_value_names

    # find output parameters and split them into those that get passed in by-ref and those that will be
    # returned by the SWIG function in a tuple.
    out_args_returned = []
    out_args_passed_in = []
    out_args = []
    if not returns_wrapped_method:
        for arg in args:
            if arg.is_output or (arg.is_inout and arg.is_returned_out_arg):
                out_args.append(arg)
                if arg.is_returned_out_arg:
                    out_args_returned.append(arg)
                else:
                    out_args_passed_in.append(arg)

    # build list of parameter names for the Python function definition; add self and remove default values
    definition_args = [arg.name_with_def_value for arg in args if not arg.is_output]
    if func_type == FUNC_TARGETED:
        definition_args = ["self"] + definition_args

    # build list of parameter names for the SWIG function invocation
    invocation_args = ["self"] if func_type == FUNC_TARGETED else []
    for arg in args:
        if arg in out_args_returned and not arg.is_inout:
            continue

        argname = arg.name
        if arg.is_output:
            pass # don't transform internally constructed by-ref output parameters
        elif arg.type == "ecmdDataBuffer":
            if new_funcname in special_input_widths:
                argname = argname + (", %d" % special_input_widths[new_funcname])
            argname = "base._to_ecmdDataBuffer(" + argname + ")"
        invocation_args.append(argname)

    # determine type of return values
    if returns_wrapped_method:
        returntypes = python_type(returntype)
    else:
        returntypes = ", ".join(arg.python_type for arg in out_args)
        if len(out_args) > 1:
            returntypes = "tuple(" + returntypes + ")"

    # construct return values
    return_values = [arg.retval_conversion for arg in out_args]

    # construct usage example, mainly for functions returning tuples
    example = "pyecmd." if func_type == FUNC_GLOBAL else "target." if func_type == FUNC_TARGETED else "buffer."
    example = example + new_funcname + "(" + ", ".join(arg.name_stripped for arg in args if not arg.is_output) + ")"
    if returns_wrapped_method:
        example = "result = " + example
    elif out_args:
        example = ", ".join(arg.name_stripped for arg in out_args) + " = " + example

    # miscellaneous
    has_retval = out_args or returns_wrapped_method

    # and now print the entire shebang
    print("")
    if len(out_args) > 1:
        print(prefix + "_" + new_funcname + "Retval = namedtuple(\"_" + new_funcname + "Retval\", \"" + " ".join(arg.name_stripped for arg in out_args) + "\")")
    print(prefix + "def " + new_funcname + "(" + ", ".join(definition_args) + "):")
    print(prefix + '    """')
    print(prefix + "    " + comment)
    print(prefix + "    C++ signature: " + line1)
    print(prefix + "    Example: " + example)
    if has_retval:
        print(prefix + "    @rtype: " + returntypes)
    print(prefix + '    """')

    for arg in out_args_passed_in:
        print(prefix + "    %s = %s" % (arg.name, arg.retval_construction))

    invocation = "ecmd." + funcname + "(" + ", ".join(invocation_args) + ")"
    if calls_wrapped_method:
        print(prefix + "    " + invocation)
    elif returns_wrapped_method:
        print(prefix + "    return " + invocation)
    else:
        if out_args_returned:
            print(prefix + "    " + ", ".join(["rc"] + [arg.name for arg in out_args_returned]) + " = " + invocation)
            print(prefix + "    base._rcwrap(rc)")
        else:
            print(prefix + "    base._rcwrap(" + invocation + ")")
        if len(out_args) > 1:
            print(prefix + "    return " + ("_" if func_type == FUNC_GLOBAL else "self._") + new_funcname + "Retval(" + ", ".join(return_values) + ")")
        elif out_args:
            print(prefix + "    return " + ", ".join(return_values))

def add_function(returntype, funcname, argstr, comment, header, full_prototype):
    # filter out #defines, destructors and operators
    if returntype == "#define" or funcname[0] == "~" or funcname[0:8] == "operator":
        return

    # transform function name
    if funcname[0:4] == "ecmd":
        new_funcname = funcname[4].lower() + funcname[5:]
    else:
        new_funcname = funcname

    if new_funcname in non_exported_names:
        return

    # export each function only once, warn if duplicate and not marked as "overloaded"
    if new_funcname in found_functions:
        if new_funcname not in overloaded_functions:
            sys.stderr.write("WARNING: %s looks overloaded without special handling!\n" % new_funcname)
        return

    # if overloaded, replace arguments
    if new_funcname in overloaded_functions:
        argstr = overloaded_functions[new_funcname]
        full_prototype = returntype + " " + funcname + " (" + argstr

    # add function to the right list
    found_functions.add(new_funcname)
    func_data = (new_funcname, header, full_prototype, comment, returntype, funcname, argstr)
    if re.search(r"ecmdChipTarget[\s&]+i_target", argstr):
        target_functions.append(func_data)
    else:
        global_functions.append(func_data)

list2python_re = re.compile(r"std::(List|Vector)\s*\<\s*(.*?)\s*>")
map2python_re = re.compile(r"std::map\s*\<\s*(.*?)\s*,\s*(.*?)\s*>")
def pythonize_types(argstr):
    argstr = argstr.replace("std::list", "std::List").replace("std::vector", "std::Vector")
    argstr = list2python_re.sub("\\2\\1", argstr)
    argstr = map2python_re.sub("\\1_\\2Map", argstr)
    return argstr

def parse_header(f):
    cur_header = ""
    comment = ""
    comment_re = re.compile(r"@name\s+(?P<header>.*?)(?:$|\n)|@brief\s+(?P<comment>.*?)(?:$|\n[\s*]*@)", re.S)
    master_re = re.compile(r"\/\*\s*(?P<comment>.*?)\s*\*\/|\n\s*(?P<returntype>[\w\s*:]+?)(?P<funcname>\w+)\s*\(\s*(?P<argstr>[^/]*?)\s*\)\s*;", re.S)
    # master_re splits out comment blocks and looks for function prototypes
    for m in master_re.finditer(f.read()):
        if m.group("comment"):
            # Comments are being searched for group headers and @brief descriptions
            m = comment_re.search(m.group("comment"))
            if m and m.group("header"):
                cur_header = re.sub(r"\s+", " ", m.group("header"))
            elif m and m.group("comment"):
                comment = re.sub(r"[\s*]+", " ", m.group("comment")).strip()
                if comment[-1].isalpha():
                    comment = comment + "."
        elif m.group("funcname"):
            returntype = m.group("returntype").strip()
            funcname   = m.group("funcname")
            argstr     = pythonize_types(re.sub("\s+", " ", re.sub("\s*=\s*", "=", m.group("argstr").replace("nullptr", "NULL"))))
            add_function(returntype, funcname, argstr.replace("*", ""), comment, cur_header, "%s %s (%s)" % (returntype, funcname, argstr))
            comment = ""

##### Main program #####
if __name__ == "__main__":
    global_functions = []
    target_functions = []
    found_functions = set()

    # parse input files
    for fname in sys.argv:
        with open(fname, "r") as f:
            parse_header(f)

    # header
    print(PROLOG + "\n'''\n" +
          "\n".join(textwrap.wrap("Autogenerated from " + ", ".join(os.path.basename(x) for x in sys.argv[1:]) + " -- modify at your own risk!")) +
          """

This module takes the hand-written code from pyecmd_base and extends it
with autogenerated wrappers for most of the eCmd functions.

@author: Joachim Fenkes <fenkes@de.ibm.com>
'''

import ecmd
from . import base
from collections import namedtuple""")

    # module scope functions
    cur_header = ""
    for data in global_functions:
        print_function(data, FUNC_GLOBAL)

    # target scope functions
    print("\n\nclass _Target(ecmd.ecmdChipTarget):")
    cur_header = ""
    for data in target_functions:
        print_function(data, FUNC_TARGETED)
