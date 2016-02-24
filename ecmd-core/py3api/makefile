# Base info and default build rules

# *****************************************************************************
# Include the common base makefile
# *****************************************************************************
include ../../makefile.base

# Setup the python3 include path
# In the distro or makefile.config the user can specify this as PY3INC
# We then need to copy that to PYINC so it just works with the included makefile
ifneq ($(strip ${PY3INC}),)
  PYINC := ${PY3INC}
endif

OUTPY := ${OUTPY3}
SWIG_SOURCE := ecmdClientPy3apiSwig.C

# Set the VPATH to pickup all the python 2 files
VPATH := ../pyapi:${VPATH}

# Override what will be the python2 default
PYTHON_NAME := python3

# Set testBuild.py to use python3
ECMDPYTHONBIN := ${ECMDPYTHON3BIN}

# Set the CFLAGS to pickup out of the pyapi directory
CFLAGS := -I../pyapi ${CFLAGS}
# As of 07/24/2014 - this ECMD_PY3API define is ONLY needed to properly generate
# the typemap in ecmdCommon.i.  That 1 additional line is needed for python 3.  JTA
SWIGFLAGS := -py3 -I../py3api/distro -DECMD_PY3API ${SWIGFLAGS}

# Include the python 2 makefile
include ../pyapi/makefile
