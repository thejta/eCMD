# Base info and default build rules
PYTHON_NAME := python3
SUBDIR     := py3api/
include ../makefile.rules

# Setup the python3 include path
# In the distro or makefile.config the user can specify this as PY3INC
# We then need to copy that to PYINC so it just works with the included makefile
ifneq ($(strip ${PY3INC}),)
  PYINC := ${PY3INC}
endif

# Set the VPATH to pickup all the python 2 files
VPATH := ../pyapi:${VPATH}

# Set testBuild.py to use python3
# Allow the user a way to set the python3 binary path
ifneq ($(strip ${ECMDPYTHON3BIN}),)
  ECMDPYTHONBIN := ${ECMDPYTHON3BIN}
endif

# Set the CFLAGS to pickup out of the pyapi directory
CFLAGS := -I../pyapi ${CFLAGS}
# As of 07/24/2014 - this ECMD_PY3API define is ONLY needed to properly generate
# the typemap in ecmdCommon.i.  That 1 additional line is needed for python 3.  JTA
SWIGFLAGS := -py3 -I../py3api/distro/${DISTRO} -DECMD_PY3API ${SWIGFLAGS}

# Include the python 2 makefile
include ../pyapi/makefile

# Reset the subdir after including the python 2 makefile
SUBDIR := py3api/
