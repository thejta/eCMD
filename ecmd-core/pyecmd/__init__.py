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
pyecmd - a Pythonic wrapper around the eCMD Python API

Naming:
=======

* Functions and classes drop the ecmd prefix,
  e.g. ecmdDataBuffer becomes pyecmd.DataBuffer and
  ecmdQueryDllInfo() becomes pyecmd.queryDllInfo()
* ecmdChipTarget becomes pyecmd.Target for brevity
* Extension functions and constants are exposed through pyecmd without
  renaming, e.g. pyecmd.ECMD_SELECTED_TARGETS_LOOP or Target.croQuerySpy()

Function Results:
=================

* All bad eCMD return codes are internally converted to exceptions
  (EcmdError or a subclass of it)
* eCMD functions that have exactly one output parameter return just that.
* eCMD functions that have more than one output parameter return a tuple
  containing all output parameters in their original order

Data Buffers:
=============

* ecmdDataBuffer is replaced by the awesome bitstring module
  (http://pythonhosted.org/bitstring/), which is much more Pythonic.
  All functions returning data return a bitstring.BitArray.
* Input data can be supplied as a bitstring (Bits, BitArray, ...),
  or a string that can be fed into a bistring constructor like "0x1234".
  If the method in question takes a fixed amount of data (like putScom,
  putCfamRegister, ...), you can also simply supply an integer value.

Targets:
========

* All eCMD functions whose first parameter is a target become methods of Target.
* Initializing a target is easy, field states (UNUSED, VALID, WILDCARD)
  are handled by the Target class internally.
* Target looping is encapsulated in pyecmd.loopTargets() which
  will simply return a list of targets.

Init and Shutdown:
==================

* Initialization and shutdown is handled through a context handler class
  called pyecmd.Ecmd - do your work inside a with statement and you can
  be sure eCMD is properly shut down even in the face of an exception.

@author: Joachim Fenkes <fenkes@de.ibm.com>

Here's a simple and very basic program using pyecmd:

from __future__ import print_function # not needed on python3
import pyecmd, sys

with pyecmd.Ecmd(args=sys.argv):
    for target in pyecmd.loopTargets("pu", pyecmd.ECMD_SELECTED_TARGETS_LOOP):
        print("Working on target %s" % target)

        # Output TP probe settings
        root_ctrl1 = target.getScom(0x50011)
        print("Probe0 select: %d -- Probe1 select: %d" % (root_ctrl1[0:4].uint, root_ctrl1[4:8].uint))

        # drop fence 0
        root_ctrl0 = target.getScom(0x50010)
        root_ctrl0[0] = False
        target.putScom(0x50010, root_ctrl0)

        # reset FSI2PIB engine
        target.putCfamRegister(0x1006, 0)

"""

import ecmd
from .base import *
from .base import _rcwrap, _bufwrap, _from_ecmdDataBuffer, _to_ecmdDataBuffer
from .generated import *
from .generated import _Target
from .constants import *
from .ecmdbitstring import *

class EcmdWrapperError(Exception):
    pass

def _map_attr_with_state(obj, name):
    # Global method to avoid implicit recursive __getattr__ call
    if name in Target._attrs_with_state:
        state = super(ecmd.ecmdChipTarget, obj).__getattribute__(name + "State")
        if state == ecmd.ECMD_TARGET_FIELD_WILDCARD:
            return "*"
        elif state == ecmd.ECMD_TARGET_FIELD_UNUSED:
            return None
    return super(ecmd.ecmdChipTarget, obj).__getattribute__(name)

class Target(_Target):
    """
    Extends the base ecmdChipTarget by several convenience functions:
     * Default constructor initializes all states to UNUSED instead of INVALID
     * Constructor optionally takes another target and acts as a copy constructor in that case
     * Constructor takes arbitrary keyword arguments specifying target attributes
     * Setting a target attribute implicitly sets its state attribute:
       * Use None for UNUSED
       * Use "*" for WILDCARD
       * Use any other value for VALID
    """

    _attrs_with_state = ("cage", "node", "slot", "chipType", "pos", "chipUnitType", "chipUnitNum", "thread", "unitId")

    def __setattr__(self, name, value):
        if name in Target._attrs_with_state:
            if value == "*":
                super(Target, self).__setattr__(name + "State", ecmd.ECMD_TARGET_FIELD_WILDCARD)
            elif value is None:
                super(Target, self).__setattr__(name + "State", ecmd.ECMD_TARGET_FIELD_UNUSED)
            else:
                super(Target, self).__setattr__(name, value)
                super(Target, self).__setattr__(name + "State", ecmd.ECMD_TARGET_FIELD_VALID)
        else:
            super(Target, self).__setattr__(name, value)

    def __getattribute__(self, name):
        if name in Target._attrs_with_state:
            return _map_attr_with_state(self, name)
        return super(Target, self).__getattribute__(name)

    def __init__(self, template=None, **kwargs):
        super(Target, self).__init__()
        if template:
            for attr in Target._attrs_with_state:
                self.__setattr__(attr, _map_attr_with_state(template, attr))
        else:
            for attr in Target._attrs_with_state:
                self.__setattr__(attr, None)
        for (attr, value) in kwargs.items():
            if attr not in self._attrs_with_state:
                raise TypeError("unexpected keyword argument '%s'" % attr)
            setattr(self, attr, value)

    def __repr__(self):
        return ecmd.ecmdWriteTarget(self, ecmd.ECMD_DISPLAY_TARGET_DEFAULT).replace("\t", " ").strip()

    def related_targets(self, target_type, mode=ecmd.ECMD_DYNAMIC_LOOP,
                        filter_chip_units=None):
        """
        List all targets of the given "chip[.unit]" type that are "related" to this target.

        Examples:
          For a "pu" target, "pu.c" will yield all cores.
          For a "pu.eq" target, "pu.c" will yield all cores attached to that cache.
          For a "pu.c" target, "pu.ex" will yield the core's virtual EX target.
          For a "pu.perv" target for chiplet 0x2E, "pu.c" will yield core 14,
            but "pu.eq" will yield nothing because the perv target is the core's perv target.

        @param filter_chip_units If specified, a list of chipUnitNums to only include in the result.

        @type target_type str
        @type mode int
        @type filter_chip_units list(int)
        @rtype list(Target)
        """

        result = ecmd.ecmdChipTargetList()
        _rcwrap(ecmd.ecmdRelatedTargets(self, target_type, result, mode))
        return [self.__class__(target) for target in result if filter_chip_units is None or target.chipUnitNum in filter_chip_units]

    # alias to match eCMD API spelling
    relatedTargets = related_targets

    def fapi2GetAttr(self, i_id):
        rc, data = ecmd.fapi2GetAttr(self, i_id)
        if rc == 0x0206005A: # FAPI_UNSUPPORTED_ATTRIBUTE
            raise KeyError(i_id)
        _rcwrap(rc)
        return data

    def fapi2SetAttr(self, i_id, i_data):
        _rcwrap(ecmd.fapi2SetAttr(self, i_id, i_data))

class Ecmd(object):
    def __init__(self, dll="", version="ver14,ver15", args=None, **kwargs):
        self.dll = dll
        self.version = version
        self.args = args
        self.extensions = kwargs

    def __enter__(self):
        _rcwrap(ecmd.ecmdLoadDll(self.dll, self.version))
        setGlobalVar(ecmd.ECMD_GLOBALVAR_QUIETERRORMODE, 1)
        if self.args:
            _rcwrap(ecmd.ecmdCommandArgs(self.args))
        for (name, version) in self.extensions.items():
            getattr(ecmd, name+"InitExtension")(version)
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        ecmd.ecmdUnloadDll()

def loopTargets(target, looptype, mode=ecmd.ECMD_DYNAMIC_LOOP, warn_if_no_targets=False):
    """
    target can be either a Target or a string containing a chip[.unit] specification
    @rtype: list(Target)
    """
    try:
        if "." in target:
            chip_type, unit_type = target.split(".", 2)
            unit_num = "*"
        else:
            chip_type = target
            unit_type = None
            unit_num = None

        target = Target(chipType=chip_type, chipUnitType=unit_type,
                        cage="*", node="*", slot="*", pos="*", chipUnitNum=unit_num)
    except TypeError:
        pass # target seems to be a Target, which is what we need

    state = ecmd.ecmdLooperData()
    my_target = target.__class__(target)
    result = []
    _rcwrap(ecmd.ecmdLooperInit(my_target, looptype, state, mode))
    while ecmd.ecmdLooperNext(my_target, state, mode):
        result.append(target.__class__(my_target))

    if warn_if_no_targets and not result:
        print("WARNING: Your selection of targets did not appear to yield any results.")
        print("         Nothing will happen. You might need to specify a -c parameter, like -call.")

    return result

def loadDataBuffer(filename, save_format = ecmd.ECMD_SAVE_FORMAT_BINARY):
    """
    Load a file saved by ecmdDataBuffer and return an EcmdBitArray containing the data
    @rtype: EcmdBitArray
    """
    buf = ecmd.ecmdDataBuffer()
    _bufwrap(buf.readFile(filename, save_format))
    return _from_ecmdDataBuffer(buf)

def saveDataBuffer(data, filename, save_format = ecmd.ECMD_SAVE_FORMAT_BINARY):
    """
    Save the contents of a bitstring to a file in ecmdDataBuffer format
    """
    buf = _to_ecmdDataBuffer(data)
    _bufwrap(buf.writeFile(filename, save_format))

def convertFromDataBuffer(buf):
    """
    Convert an ecmdDataBuffer (e.g. from a result list object) into an EcmdBitArray
    """
    return _from_ecmdDataBuffer(buf)

def convertToDataBuffer(buf):
    """
    Convert a bit string into an ecmdDataBuffer
    """
    return _to_ecmdDataBuffer(buf)
