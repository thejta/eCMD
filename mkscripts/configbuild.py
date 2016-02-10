#!/usr/bin/env python

# This script will look for any ecmd-* dirs at the top level and
# setup a number of variables used through out the make
# Those variables are then written out to a makefile included by the build

import os
import glob
import platform
import textwrap

# Track all the vars we plan to print out
buildvars = dict()

# First, determine our ECMD_ROOT directory
# This will be one up from the directory where this script lives
ECMD_ROOT,trash = os.path.split(os.path.dirname(os.path.realpath(__file__)))

buildvars["ECMD_ROOT"] = ECMD_ROOT

# The main ecmd repo contains ecmd-core
# Set that variable
ECMD_CORE = os.path.join(ECMD_ROOT, "ecmd-core")
buildvars["ECMD_CORE"] = ECMD_CORE

# Now look for the ecmd repos configured in here
ECMD_REPOS = ""
for repo in glob.glob(ECMD_ROOT + "/ecmd-*"):
    ECMD_REPOS += os.path.basename(repo) + " "
ECMD_REPOS = ECMD_REPOS[:-1] # Pull off trailing space

buildvars["ECMD_REPOS"] = ECMD_REPOS

# Setup our utils locations
for repo in ECMD_REPOS.split(" "):
    testpath = os.path.join(ECMD_ROOT, repo, "utils")
    if (os.path.exists(testpath)):
        buildvars[repo + "_UTILS"] = testpath

# Setup our plugins locations
ECMD_PLUGINS = ""
for repo in ECMD_REPOS.split(" "):
    testpath = os.path.join(ECMD_ROOT, repo, "plugins")
    if (os.path.exists(testpath)):
        buildvars[repo + "_PLUGINS"] = testpath

    for plugin in glob.glob(os.path.join(testpath, "*")):
        baseplugin = os.path.basename(plugin)
        ECMD_PLUGINS += baseplugin + " "
        buildvars["PLUGIN_" + baseplugin + "_PATH"] = os.path.join(testpath, baseplugin)

buildvars["ECMD_PLUGINS"] = ECMD_PLUGINS[:-1] # Pull off trailing space


# The extensions

# If the extensions aren't given, loop thru and create the list
EXTENSIONS = ""
if "EXTENSIONS" in os.environ:
    EXTENSIONS = os.environ["EXTENSIONS"]
else:
    for repo in ECMD_REPOS.split(" "):
        for ext in glob.glob(os.path.join(ECMD_ROOT, repo, "ext/*")):
            EXTENSIONS += os.path.basename(ext) + " "
    EXTENSIONS = EXTENSIONS[:-1] # Pull off trailing space

# Remove "template" from the extensions list
EXTENSIONS = EXTENSIONS.replace(" template", "")

buildvars["EXTENSIONS"] = EXTENSIONS

EXT_CAPI = ""
EXT_CMD = ""
EXT_PERLAPI = ""
EXT_PYAPI = ""
for ext in EXTENSIONS.split():
    for repo in ECMD_REPOS.split(" "):
        testpath = os.path.join(ECMD_ROOT, repo, "ext", ext)
        if (os.path.exists(testpath)):
            buildvars["EXT_" + ext + "_PATH"] = testpath

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

buildvars["EXT_CAPI"] = EXT_CAPI[:-1] # Pull off trailing space
buildvars["EXT_CMD"] = EXT_CMD[:-1] # Pull off trailing space
buildvars["EXT_PERLAPI"] = EXT_PERLAPI[:-1] # Pull off trailing space
buildvars["EXT_PYAPI"] = EXT_PYAPI[:-1] # Pull off trailing space

# The extension template is contained in ecmd-core, just set that path
EXT_TEMPLATE_PATH = os.path.join(ECMD_CORE, "ext/template")
buildvars["EXT_TEMPLATE_PATH"] = EXT_TEMPLATE_PATH


# Grab the HOST_ARCH
HOST_ARCH = platform.machine()
buildvars["HOST_ARCH"] = HOST_ARCH

# Set the host base arch.  Just happens to be the first 3 characters
buildvars["HOST_BARCH"] = HOST_ARCH[0:3]

# If the TARGET_ARCH was passed in, use that.  Otherwise, HOST_ARCH
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
dtuple = platform.linux_distribution()

if ("Red Hat Enterprise" in dtuple[0]):
    DISTRO = "el" + dtuple[1][0]
elif (dtuple[0] == "Ubuntu"):
    DISTRO = "ub" + dtuple[1]
elif (dtuple[0] == "Fedora"):
    DISTRO = "fc" + dtuple[1]
elif (dtuple[0] == "debian"):
    DISTRO = "deb" + dtuple[1][0]

buildvars["DISTRO"] = DISTRO

# If INSTALL_PATH wasn't given, install into our local dir
if "INSTALL_PATH" not in os.environ:
    INSTALL_PATH = os.path.join(ECMD_ROOT, "install")
    buildvars["INSTALL_PATH"] = INSTALL_PATH

# Setup the output path info for the created binaries and libraries
OUTPATH = os.path.join(ECMD_ROOT, "out_" + TARGET_ARCH)
buildvars["OUTPATH"] = OUTPATH
buildvars["OUTBIN"] = os.path.join(OUTPATH, "bin")
buildvars["OUTLIB"] = os.path.join(OUTPATH, "lib")
buildvars["OUTPERL"] = os.path.join(OUTPATH, "perl")
buildvars["OUTPY"] = os.path.join(OUTPATH, "pyapi")
buildvars["OUTPY3"] = os.path.join(OUTPATH, "py3api")

OBJPATH = os.path.join(ECMD_ROOT, "obj_" + TARGET_ARCH)
OBJPATH += "/"
buildvars["OBJPATH"] = OBJPATH
SRCPATH = os.path.join(ECMD_ROOT, "src_" + TARGET_ARCH)
buildvars["SRCPATH"] = SRCPATH

######## Default things we need setup for every compile ########
# SUBDIR = where the objects are moved to after compile
# CC_VER = the version of the compiler we are using.  Used by the install rule to grab right shared lib versions
# CC = the compiler
# CC_R = the reentrant compiler, only different for AIX
# CFLAGS = flags to pass to the compiler
# LD = the linker
# LD_R = the reentrant linker, only different for AIX
# LDFLAGS = flags to pass to the linker when linking exe's
# SLDFLAGS = flags to pass to the linker when linking shared libs
# AR = the archive creator

# Setup the C flags
DEFINES = ""
GPATH = ""
CFLAGS = ""
LDFLAGS = ""
SLDFLAGS = ""
# Common compile flags across any OS
CFLAGS = "-g -I."

if "DEFINES" in os.environ:
    DEFINES = os.environ["DEFINES"]

# Setup common flags across an OS
if (TARGET_BARCH == "x86" or TARGET_BARCH == "ppc"):
    DEFINES += " -DLINUX"
    GPATH += " " + OBJPATH
    CFLAGS += " -Wall"
    if (TARGET_ARCH.find("64")):
        CFLAGS += " -m64 -fPIC"
        LDFLAGS += " -m64 -fPIC"
        SLDFLAGS += " -shared -m64 -fPIC"
    else:
        CFLAGS += " -m32 -fPIC"
        LDFLAGS += " -m32 -fPIC"
        SLDFLAGS += " -shared -m32 -fPIC"
else:
    DEFINES += " -DAIX"

buildvars["DEFINES"] = DEFINES
buildvars["GPATH"] = GPATH
buildvars["CFLAGS"] = CFLAGS
buildvars["LDFLAGS"] = LDFLAGS
buildvars["SLDFLAGS"] = SLDFLAGS

print textwrap.dedent("""\
ifeq (${MAKEFILE_VARS},)

MAKEFILE_VARS := y

# Turn off implicit source checkout rules in gmake to speed it up
%: %,v
%: RCS/%,v
%: RCS/%
%: s.%
%: SCCS/s.%

# Remove the @ when debugging build problems to see more output
VERBOSE := @

#These are needed to define a space char for substitutions later on
empty :=
space := ${empty} ${empty}
""")

print("")
print("# These variables are generated by buildconfig.py")
print("")

for var in sorted(buildvars):
    print("%s := %s" % (var, buildvars[var]))

for var in sorted(buildvars):
    print("export %s" % var)

print("")
print("")

print textwrap.dedent("""\
# If not set in the included makefiles, set optimization level 3 for libecmd.so
OPT_LEVEL ?= -O3

# If not disabled in the included makefiles, enable build tests
TEST_BUILD ?= yes

# Include the makefile for this build type
include ${ECMD_ROOT}/mkconfigs/${DISTRO}/make-${HOST_ARCH}-${TARGET_ARCH}

# Include the makefile.config, this will override anything above
-include ${ECMD_ROOT}/makefile.config

endif # MAKEFILE_VARS
""")
