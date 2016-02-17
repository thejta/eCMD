#!/usr/bin/env python

# This script will look for any ecmd-* dirs at the top level and
# setup a number of variables used through out the make
# Those variables are then written out to a makefile.config
# makefile.config is included by makefile.base

# Python imports
import os
import glob
import platform
import textwrap
import re

# Store any variables we wish to write to the makefiles here
buildvars = dict()

# First, determine our ECMD_ROOT variable
# ECMD_ROOT is the top level directory of the ecmd source repo
# ECMD_ROOT is used to derive a number of variable throughout this script
# ECMD_ROOT is one up from the directory where this script lives
ECMD_ROOT = os.path.dirname(os.path.realpath(__file__))
buildvars["ECMD_ROOT"] = ECMD_ROOT

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
EXTENSIONS = ""
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
re.sub("\s+", " ", EXTENSIONS) # Multiple spaces into 1
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

# Now that we have the distro and arch, look for the disto makefile
# A number of standard distro configs are included
# Those can be overriden in the included repos

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

# All objects from the build go to a common dir at the top level
# OBJPATH includes TARGET_ARCH to allow for side by side builds
# This does come with the stipulation that all source must have unique names
OBJPATH = os.path.join(ECMD_ROOT, "obj_" + TARGET_ARCH)
OBJPATH += "/" # Tack this on so the .C->.o rules run properly
buildvars["OBJPATH"] = OBJPATH

# All generated source from makedll.pl, swig, etc.. go into this directory
# Previous source was dumped in the local dir and it was hard to distinguish it from normal source
SRCPATH = os.path.join(ECMD_ROOT, "src_" + TARGET_ARCH)
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
OUTPATH = os.path.join(ECMD_ROOT, "out_" + TARGET_ARCH)
buildvars["OUTPATH"] = OUTPATH
buildvars["OUTBIN"] = os.path.join(OUTPATH, "bin")
buildvars["OUTLIB"] = os.path.join(OUTPATH, "lib")
buildvars["OUTPERL"] = os.path.join(OUTPATH, "perl")
buildvars["OUTPY"] = os.path.join(OUTPATH, "pyapi")
buildvars["OUTPY3"] = os.path.join(OUTPATH, "py3api")

##################
# Setup info around an install

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

##################
# Write out all our variables to makefile.config

# Now go thru everything that has been setup and write it out to the file
print("Writing %s" % os.path.join(ECMD_ROOT, "makefile.config"))
config = open(os.path.join(ECMD_ROOT, "makefile.config"), 'w')
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

config.close()
