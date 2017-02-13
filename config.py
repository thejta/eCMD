#!/usr/bin/env python

# This script will look for any ecmd-* dirs at the top level and
# setup a number of variables used through out the make
# Those variables are then written out to a makefile.config
# makefile.config is included by makefile.base

# Python module imports
import os
import sys
import glob
import platform
import textwrap
import re
import argparse

parser = argparse.ArgumentParser(description="This script creates all the variables necessary to buid eCMD", add_help = False)
# Group for required args so the help displays properly
reqgroup = parser.add_argument_group('Required Arguments')
# Group for the optional args so the help displays properly
optgroup = parser.add_argument_group('Optional Arguments')

# These args can also be set by declaring their environment variable
# before calling this script.
# If you specify both, the cmdline arg wins over the env variable
# --help
optgroup.add_argument("-h", "--help", help="Show this message and exit", action="help")

# --install-path
optgroup.add_argument("--install-path", help="Path to install to")

# --host
optgroup.add_argument("--host", help="The host architecture")

# --target
optgroup.add_argument("--target", help="The target architecture")

# --swig
optgroup.add_argument("--swig", help="The swig executable to use")

# --perl
optgroup.add_argument("--perl", help="The perl executable to use")

# --python
optgroup.add_argument("--python", help="The python executable to use")

# --python3
optgroup.add_argument("--python3", help="The python3 executable to use")

# --doxygen
optgroup.add_argument("--doxygen", help="The doxygen executable to use")

# --output-root
optgroup.add_argument("--output-root", help="The location to place build output")

# --extensions
optgroup.add_argument("--extensions", help="Filter down the list of extensions to build")

# --no-sim
optgroup.add_argument("--no-sim", help="Enable REMOVE_SIM in build", action='store_true')

# --no-perl
optgroup.add_argument("--no-perl", help="Disable perl module build", action='store_true')

# --no-python
optgroup.add_argument("--no-python", help="Disable python module build", action='store_true')

# --no-python3
optgroup.add_argument("--no-python3", help="Disable python3 module build", action='store_true')

# Parse the cmdline for the args we just added
args = parser.parse_args()

# Store any variables we wish to write to the makefiles here
buildvars = dict()

# First, determine our ECMD_ROOT variable
# ECMD_ROOT is the top level directory of the ecmd source repo
# ECMD_ROOT is used to derive a number of variable throughout this script
ECMD_ROOT = os.path.dirname(os.path.realpath(__file__))
buildvars["ECMD_ROOT"] = ECMD_ROOT

# If the OUTPUT_ROOT was passed in, use that for base directory for generated
# files. Otherwise use ECMD_ROOT
# See if the user specified it via the script cmdline
# If not, pull it from the env or set the default
if (args.output_root != None):
    OUTPUT_ROOT = args.output_root
else:
    if "OUTPUT_ROOT" in os.environ:
        OUTPUT_ROOT = os.environ["OUTPUT_ROOT"]
    else:
        OUTPUT_ROOT = ECMD_ROOT
buildvars["OUTPUT_ROOT"] = OUTPUT_ROOT

# The main ecmd repo contains the ecmd-core subdir
# This is where the main eCMD source code is stored
# ECMD_CORE will point to this location
ECMD_CORE = os.path.join(ECMD_ROOT, "ecmd-core")
buildvars["ECMD_CORE"] = ECMD_CORE

# In addition to ecmd-core, a user could add any number of ecmd-* repos under main
# Look for those other repos and create ECMD_REPOS that stores all of them
# We store the results in a form that make can loop over using foreach
ECMD_REPOS = ""
for repo in glob.glob(ECMD_ROOT + "/ecmd-*"):
    ECMD_REPOS += os.path.basename(repo) + " "
ECMD_REPOS = ECMD_REPOS[:-1] # Pull off trailing space
buildvars["ECMD_REPOS"] = ECMD_REPOS

# Each of the sub repos could contain a utils directory
# We want the top level make to be able to build any utils out there
# Loop through each of the ECMD_REPOS above and see if they have a utils dir
# We create ECMD_REPOS_UTILS that has all the repos that have a util dir
ECMD_REPOS_UTILS = ""
for repo in ECMD_REPOS.split(" "):
    # Create the utils dir path
    testpath = os.path.join(ECMD_ROOT, repo, "utils")
    # See if this repo has a utils dir, if it does add it to the list
    if (os.path.exists(testpath)):
        ECMD_REPOS_UTILS += repo + " "
ECMD_REPOS_UTILS = ECMD_REPOS_UTILS[:-1] # Pull off trailing space
buildvars["ECMD_REPOS_UTILS"] = ECMD_REPOS_UTILS

# The plugins logic is similar to the utils logic, with one complication
# The things we are interested in are actually contained in each plugin directory present
# So we will create ECMD_REPOS_PLUGINS to show who has a plugins dir
# We will also create ECMD_PLUGINS, that has the name of each plugin found in the plugins dir
# Since ECMD_PLUGINS, won't be tied to the repo is comes from, 
# We also create a number of PATH variables that can be used to access the plugin path
# By looping over ECMD_PLUGINS, we can dynamically access PLUGINS_plugin_PATH
ECMD_REPOS_PLUGINS = ""
ECMD_PLUGINS = ""
for repo in ECMD_REPOS.split(" "):
    # Create the plugins dir path
    testpath = os.path.join(ECMD_ROOT, repo, "plugins")
    # If it exists, setup a number of variables from that dir
    if (os.path.exists(testpath)):
        ECMD_REPOS_PLUGINS += repo + " "
        # Look to see what plugins exist in the directory
        for plugin in glob.glob(os.path.join(testpath, "*")):
            baseplugin = os.path.basename(plugin)
            ECMD_PLUGINS += baseplugin + " "
            buildvars["PLUGIN_" + baseplugin + "_PATH"] = os.path.join(testpath, baseplugin)
ECMD_REPOS_PLUGINS = ECMD_REPOS_PLUGINS[:-1] # Pull off trailing space
buildvars["ECMD_REPOS_PLUGINS"] = ECMD_REPOS_PLUGINS
ECMD_PLUGINS = ECMD_PLUGINS[:-1] # Pull off trailing space
# Take our random order string and create a sorted string
buildvars["ECMD_PLUGINS"] = ' ' . join(sorted(ECMD_PLUGINS.split(" ")))

# The extensions logic is also similar to utils/plugins - but with yet another level of complication
# The user can set EXTENSIONS before calling this script, and it's what the script will use
# If the user doesn't set it, loop through all the repos and look for what extenions exist
# See if the user specified it via the script cmdline
# If not, pull it from the env or set the default
EXTENSIONS = ""
if (args.extensions != None):
    EXTENSIONS = args.extensions
else:
    if "EXTENSIONS" in os.environ:
        EXTENSIONS = os.environ["EXTENSIONS"]
    else:
        # Not given, so dynamically create the list based on what's in the ECMD_REPOS
        for repo in ECMD_REPOS.split(" "):
            for ext in glob.glob(os.path.join(ECMD_ROOT, repo, "ext/*")):
                EXTENSIONS += os.path.basename(ext) + " "
        EXTENSIONS = EXTENSIONS[:-1] # Pull off trailing space

# Remove "template" from the extensions list
EXTENSIONS = EXTENSIONS.replace("template", "")
# Now cleanup any multiple, leading or trailing spaces
# Those throw off the make foreach looping
EXTENSIONS = re.sub("\s+", " ", EXTENSIONS) # Multiple spaces into 1
EXTENSIONS = EXTENSIONS.strip() # Remove leading/trailing whitespace
# Take our random order string and create a sorted string
buildvars["EXTENSIONS"] = ' ' . join(sorted(EXTENSIONS.split(" ")))

# We now have our EXTENSIONS to use
# However, every extension doesn't support every build type
# For example, and extension may have a C-API, but not a cmdline
# Loop through the extensions and create a variable for each build type
# We also create an EXT_ext_PATH variable that points to the topdir of the extension
EXT_CAPI = ""
EXT_CMD = ""
EXT_PERLAPI = ""
EXT_PYAPI = ""
# Loop through the extensions
for ext in sorted(EXTENSIONS.split()):
    # Loop through the repos to find where the extension lives
    for repo in ECMD_REPOS.split(" "):
        testpath = os.path.join(ECMD_ROOT, repo, "ext", ext)
        # If we found it, setup a number of things
        if (os.path.exists(testpath)):
            buildvars["EXT_" + ext + "_PATH"] = testpath

            # Now that we have a valid ext dir, test it for each build type
            testpath = os.path.join(ECMD_ROOT, repo, "ext", ext, "capi")
            if (os.path.exists(testpath)):
                EXT_CAPI += ext + " "

            testpath = os.path.join(ECMD_ROOT, repo, "ext", ext, "cmd")
            if (os.path.exists(testpath)):
                EXT_CMD += ext + " "

            testpath = os.path.join(ECMD_ROOT, repo, "ext", ext, "perlapi")
            if (os.path.exists(testpath)):
                EXT_PERLAPI += ext + " "

            testpath = os.path.join(ECMD_ROOT, repo, "ext", ext, "pyapi")
            if (os.path.exists(testpath)):
                EXT_PYAPI += ext + " "
EXT_CAPI = EXT_CAPI[:-1] # Pull off trailing space
buildvars["EXT_CAPI"] = EXT_CAPI
EXT_CMD = EXT_CMD[:-1] # Pull off trailing space
buildvars["EXT_CMD"] = EXT_CMD
EXT_PERLAPI = EXT_PERLAPI[:-1] # Pull off trailing space
buildvars["EXT_PERLAPI"] = EXT_PERLAPI
EXT_PYAPI = EXT_PYAPI[:-1] # Pull off trailing space
buildvars["EXT_PYAPI"] = EXT_PYAPI

# The extension template is contained in ecmd-core, just set that path
EXT_TEMPLATE_PATH = os.path.join(ECMD_CORE, "ext", "template")
buildvars["EXT_TEMPLATE_PATH"] = EXT_TEMPLATE_PATH

##############################
# We've figured out a bunch of stuff about our eCMD source available
# Now let's setup up all the info about our build environment

# Grab the HOST_ARCH
# See if the user specified it via the script cmdline
# If not, set the default
HOST_ARCH = ""
if (args.host != None):
    HOST_ARCH = args.host
else:
    if (platform.system() == "AIX"):
        atuple = platform.architecture()
        if (atuple[0] == "32bit"):
            HOST_ARCH="aix"
        else:
            HOST_ARCH="aix64"
    else:
        HOST_ARCH = platform.machine()
buildvars["HOST_ARCH"] = HOST_ARCH

# Set the host base arch.  Just happens to be the first 3 characters
buildvars["HOST_BARCH"] = HOST_ARCH[0:3]

# If the TARGET_ARCH was passed in, use that.  Otherwise, HOST_ARCH
# See if the user specified it via the script cmdline
# If not, pull it from the env or set the default
if (args.target != None):
    TARGET_ARCH = args.target
else:
    if "TARGET_ARCH" in os.environ:
        TARGET_ARCH = os.environ["TARGET_ARCH"]
    else:
        TARGET_ARCH = HOST_ARCH
buildvars["TARGET_ARCH"] = TARGET_ARCH

# Set the target base arch.  Just happens to be the first 3 characters
TARGET_BARCH = TARGET_ARCH[0:3]
buildvars["TARGET_BARCH"] = TARGET_BARCH

# Determine the distro
DISTRO = ""
if (platform.system() == "Linux"):
    dtuple = platform.linux_distribution()
    if ("Red Hat Enterprise" in dtuple[0]):
        DISTRO = "el" + dtuple[1][0]
    elif (dtuple[0] == "Ubuntu"):
        DISTRO = "ub" + dtuple[1]
    elif (dtuple[0] == "Fedora"):
        DISTRO = "fc" + dtuple[1]
    elif (dtuple[0] == "debian"):
        DISTRO = "deb" + dtuple[1][0]
else:
    DISTRO="aix"
buildvars["DISTRO"] = DISTRO

# Now that we have the distro and arch, look for the disto makefile
# A number of standard distro configs are included
# Those can be overriden in the included repos
# If more than 1 override file is found in the included repos, throw an error
# The user can set DISTRO_MAKEFILE and then we don't do this logic at all
DISTRO_MAKEFILE = ""
if "DISTRO_MAKEFILE" in os.environ:
    DISTRO_MAKEFILE = os.environ["DISTRO_MAKEFILE"]
else:
    # This loop includes ecmd-core, which should never have mkconfigs
    for repo in ECMD_REPOS.split(" "):
        mkfile = os.path.join(ECMD_ROOT, repo, "mkconfigs", DISTRO, "make-" + HOST_ARCH + "-" + TARGET_ARCH)
        if os.path.exists(mkfile):
            if (DISTRO_MAKEFILE != ""):
                print("ERROR: conflicting distro makefiles found!")
                print("ERROR: %s" % DISTRO_MAKE)
                print("ERROR: %S" % mkfile)
                print("Please set DISTRO_MAKEFILE to your choice and re-run this command")
                sys.exit(1)
            else:
                DISTRO_MAKEFILE = mkfile
    
    # If there wasn't one found in the repos, see if the top level has one
    if (DISTRO_MAKEFILE == ""):
        mkfile = os.path.join(ECMD_ROOT, "mkconfigs", DISTRO, "make-" + HOST_ARCH + "-" + TARGET_ARCH)
        if (os.path.exists(mkfile)):
            DISTRO_MAKEFILE = mkfile
        else:
            print("ERROR: No distro makefile found for your configuration!")
            print("ERROR: DISTRO=%s, HOST_ARCH=%s, TARGET_ARCH=%s" % (DISTRO, HOST_ARCH, TARGET_ARCH))
            print("ERROR: Please consider creating a makefile for your build type")
            print("ERROR: Or set DISTRO_MAKEFILE to your makefile and re-run this command")
            sys.exit(1)
    
buildvars["DISTRO_MAKEFILE"] = DISTRO_MAKEFILE

######## Default things we need setup for every compile ########
# CC = the compiler
# CC_R = the reentrant compiler, only different for AIX
# CFLAGS = flags to pass to the compiler
# LD = the linker
# LD_R = the reentrant linker, only different for AIX
# LDFLAGS = flags to pass to the linker when linking exe's
# SLDFLAGS = flags to pass to the linker when linking shared libs
# AR = the archive creator
# DEFINES = -D defines to pass thru

# Assign default values, unless the user set them before calling the script
# The distro makefiles also have the ability to override
CC = ""
CC_R = ""
LD = ""
LD_R = ""
AR = ""
# Compiler
if "CC" in os.environ:
    CC = os.environ["CC"]
else:
    CC = "/usr/bin/g++"
buildvars["CC"] = CC

# Compiler Reentrant
if "CC_R" in os.environ:
    CC_R = os.environ["CC_R"]
else:
    CC_R = "/usr/bin/g++"
buildvars["CC_R"] = CC_R

# Linker
if "LD" in os.environ:
    LD = os.environ["LD"]
else:
    LD = "/usr/bin/g++"
buildvars["LD"] = LD

# Linker Reentrant
if "LD_R" in os.environ:
    LD_R = os.environ["LD_R"]
else:
    LD_R = "/usr/bin/g++"
buildvars["LD_R"] = LD_R

# Archive
if "AR" in os.environ:
    AR = os.environ["AR"]
else:
    AR = "/usr/bin/ar"
buildvars["AR"] = AR

# All objects from the build go to a common dir at the top level
# OBJPATH includes TARGET_ARCH to allow for side by side builds
# This does come with the stipulation that all source must have unique names
OBJPATH = os.path.join(OUTPUT_ROOT, "obj_" + TARGET_ARCH)
OBJPATH += "/" # Tack this on so the .C->.o rules run properly
buildvars["OBJPATH"] = OBJPATH

# All generated source from makedll.pl, swig, etc.. go into this directory
# Previous source was dumped in the local dir and it was hard to distinguish it from normal source
SRCPATH = os.path.join(OUTPUT_ROOT, "src_" + TARGET_ARCH)
buildvars["SRCPATH"] = SRCPATH

# Setup the variable defaults
DEFINES = ""
GPATH = ""
CFLAGS = ""
LDFLAGS = ""
SLDFLAGS = ""

# Common compile flags across any OS
CFLAGS = "-g -I."

# If the user passed thru extra defines, grab them
if "DEFINES" in os.environ:
    DEFINES = os.environ["DEFINES"]

# Setup common variables across distros
if (TARGET_BARCH == "x86" or TARGET_BARCH == "ppc"):
    DEFINES += " -DLINUX"
    GPATH += " " + OBJPATH
    CFLAGS += " -Wall"
    if (TARGET_ARCH.find("64") != -1):
        CFLAGS += " -m64 -fPIC"
        LDFLAGS += " -m64 -fPIC"
        SLDFLAGS += " -shared -m64 -fPIC"
    else:
        CFLAGS += " -m32 -fPIC"
        LDFLAGS += " -m32 -fPIC"
        SLDFLAGS += " -shared -m32 -fPIC"
else:
    DEFINES += " -DAIX"

# See if REMOVE_SIM is enabled from the cmdline
if (args.no_sim):
    DEFINES += " -DREMOVE_SIM"
    
# Export everything we defined
buildvars["DEFINES"] = DEFINES
buildvars["GPATH"] = GPATH
buildvars["CFLAGS"] = CFLAGS
buildvars["LDFLAGS"] = LDFLAGS
buildvars["SLDFLAGS"] = SLDFLAGS

# Setup the output path info for the created binaries and libraries
# We have one top level output path where all output binaries go
# This could be shared libs, archives or executables
# OUTPATH includes the TARGET_ARCH to allow for side by side builds
OUTPATH = os.path.join(OUTPUT_ROOT, "out_" + TARGET_ARCH)
buildvars["OUTPATH"] = OUTPATH
buildvars["OUTBIN"] = os.path.join(OUTPATH, "bin")
buildvars["OUTLIB"] = os.path.join(OUTPATH, "lib")
buildvars["OUTPERL"] = os.path.join(OUTPATH, "perl")
buildvars["OUTPY"] = os.path.join(OUTPATH, "pyapi")
buildvars["OUTPY3"] = os.path.join(OUTPATH, "py3api")

# Check for optional args to disable building of perl and python module
if (args.no_perl):
    buildvars["CREATE_PERLAPI"] = "no"

if (args.no_python):
    buildvars["CREATE_PYAPI"] = "no"

if (args.no_python3):
    buildvars["CREATE_PY3API"] = "no"

# Use the default path for a swig install
# See if the user specified it via the script cmdline
# If not, pull it from the env or set the default
# The user can override before calling this script or in the distro makefile
SWIG = ""
if (args.swig != None):
    SWIG = args.swig
else:
    if "SWIG" in os.environ:
        SWIG = os.environ["SWIG"]
    else:
        SWIG = "/usr/bin/swig"
buildvars["SWIG"] = SWIG

# Use the default path for a perl install
# See if the user specified it via the script cmdline
# If not, pull it from the env or set the default
# The user can override before calling this script or in the distro makefile
ECMDPERLBIN = ""
if (args.perl != None):
    ECMDPERLBIN = args.perl
else:
    if "ECMDPERLBIN" in os.environ:
        ECMDPERLBIN = os.environ["ECMDPERLBIN"]
    else:
        ECMDPERLBIN = "/usr/bin/perl"
buildvars["ECMDPERLBIN"] = ECMDPERLBIN

# Use the default path for a python install
# See if the user specified it via the script cmdline
# If not, pull it from the env or set the default
# The user can override before calling this script or in the distro makefile
ECMDPYTHONBIN = ""
if (args.python != None):
    ECMDPYTHONBIN = args.python
else:
    if "ECMDPYTHONBIN" in os.environ:
        ECMDPYTHONBIN = os.environ["ECMDPYTHONBIN"]
    else:
        ECMDPYTHONBIN = "/usr/bin/python"
buildvars["ECMDPYTHONBIN"] = ECMDPYTHONBIN

# Use the default path for a python3 install
# See if the user specified it via the script cmdline
# If not, pull it from the env or set the default
# The user can override before calling this script or in the distro makefile
ECMDPYTHON3BIN = ""
if (args.python3 != None):
    ECMDPYTHON3BIN = args.python3
else:
    if "ECMDPYTHON3BIN" in os.environ:
        ECMDPYTHON3BIN = os.environ["ECMDPYTHON3BIN"]
    else:
        ECMDPYTHON3BIN = "/usr/bin/python3"
buildvars["ECMDPYTHON3BIN"] = ECMDPYTHON3BIN

##################
# Setup info around an install

# See if the user specified it via the script cmdline
# If not, pull it from the env or set the default
if (args.install_path != None):
    INSTALL_PATH = args.install_path
else:
    # If INSTALL_PATH wasn't given, install into our local dir
    if "INSTALL_PATH" not in os.environ:
        INSTALL_PATH = os.path.join(ECMD_ROOT, "install")
    else:
        INSTALL_PATH = os.environ["INSTALL_PATH"]
buildvars["INSTALL_PATH"] = INSTALL_PATH

# If not given, create INSTALL_BIN_PATH off INSTALL_PATH
# INSTALL_BIN_PATH is in ecmdaliases.ksh/ecmdaliases.csh
# This can be different from INSTALL_PATH to enable a network install
# You could set it a shell variable that is evaluated at runtime
# Useful in common environments that may be shadowed around sites
# Setting it properly is tricky in that case.
# If you want $COMMONPATH to end up in the aliases files,
# INSTALL_BIN_PATH needs to be \\\\$$"COMMONPATH" in makefile.config
# That accounts for \'s and $'s that get ate by make during the install rule
# If you want to call config.py to set the value in the env, do it like this:
# INSTALL_BIN_PATH="\\\\\\\\\$\$\"COMMONPATH\"" ./config.py
# That escapes all the right things so you end up with \\\\$$ in makefile.config
if "INSTALL_BIN_PATH" not in os.environ:
    INSTALL_BIN_PATH = os.path.join(INSTALL_PATH, "bin")
else:
    INSTALL_BIN_PATH = os.environ["INSTALL_BIN_PATH"]
buildvars["INSTALL_BIN_PATH"] = INSTALL_BIN_PATH

# If DOXYGEN_PATH wasn't given, use INSTALL_PATH
if "DOXYGEN_PATH" not in os.environ:
    DOXYGEN_PATH = os.path.join(INSTALL_PATH, "doxygen")
else:
    DOXYGEN_PATH = os.environ["DOXYGEN_PATH"]
# Setup our capi/perl/python paths based on DOXYGEN_PATH
DOXYGEN_CAPI_PATH = os.path.join(DOXYGEN_PATH, "Capi")
DOXYGEN_PERLAPI_PATH = os.path.join(DOXYGEN_PATH, "Perlapi")
DOXYGEN_PYAPI_PATH = os.path.join(DOXYGEN_PATH, "Pythonapi")
buildvars["DOXYGEN_PATH"] = DOXYGEN_PATH
buildvars["DOXYGEN_CAPI_PATH"] = DOXYGEN_CAPI_PATH
buildvars["DOXYGEN_PERLAPI_PATH"] = DOXYGEN_PERLAPI_PATH
buildvars["DOXYGEN_PYAPI_PATH"] = DOXYGEN_PYAPI_PATH

# Pull the version out of ecmdStructs.H if not given
DOXYGEN_ECMD_VERSION = ""
if "DOXYGEN_ECMD_VERSION" not in os.environ:
    verfile = open(os.path.join(ECMD_CORE, "capi", "ecmdStructs.H"), "r")
    for line in verfile:
        linesplit = line.split()
        if ((len(linesplit) > 1) and (linesplit[1] == "ECMD_CAPI_VERSION")):
            DOXYGEN_ECMD_VERSION = line.split()[2] # "M.m"
            DOXYGEN_ECMD_VERSION = DOXYGEN_ECMD_VERSION[1:] # M.m"
            DOXYGEN_ECMD_VERSION = DOXYGEN_ECMD_VERSION[0:-1] # M.m
            break
    verfile.close()
else:
    DOXYGEN_ECMD_VERSION = os.environ["DOXYGEN_ECMD_VERSION"]
buildvars["DOXYGEN_ECMD_VERSION"] = DOXYGEN_ECMD_VERSION

# Define where to get the doxygen executable from
# See if the user specified it via the script cmdline
# If not, pull it from the env or set the default
DOXYGENBIN = ""
if (args.doxygen != None):
    DOXYGENBIN = args.doxygen
else:
    if "DOXYGENBIN" not in os.environ:
        DOXYGENBIN = "/usr/bin/doxygen"
    else:
        DOXYGENBIN = os.environ["DOXYGENBIN"]
buildvars["DOXYGENBIN"] = DOXYGENBIN

##################
# Write out all our variables to makefile.config
if "MAKEFILE_CONFIG_NAME" not in os.environ:
    MAKEFILE_CONFIG_NAME = "makefile.config"
else:
    MAKEFILE_CONFIG_NAME = os.environ["MAKEFILE_CONFIG_NAME"]

# Now go thru everything that has been setup and write it out to the file
print("Writing %s" % os.path.join(ECMD_ROOT, MAKEFILE_CONFIG_NAME))
config = open(os.path.join(ECMD_ROOT, MAKEFILE_CONFIG_NAME), 'w')
config.write("\n")
config.write("# These variables are generated by config.py\n")
config.write("\n")

# Write out all the variables
for var in sorted(buildvars):
    config.write("%s := %s\n" % (var, buildvars[var]))

config.write("\n")

# Export them so they can be referenced by any scripts used in the build
for var in sorted(buildvars):
    config.write("export %s\n" % var)

config.write("\n")

# Write our optional extension makefile.config includes
# This allows you to add values to variables defined above
for ext in sorted(EXTENSIONS.split(" ")):
    config.write("-include %s\n" % os.path.join(buildvars["EXT_" + ext + "_PATH"], "makefile.config")) 

config.close()
