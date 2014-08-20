# Base info and default build rules
PYTHON_NAME := python3
SUBDIR     := py3api/
include ../makefile.rules

SWIG ?= /gsa/rchgsa/projects/e/ecmd/utils/swig-3.0.2/x86_64/bin/swig

# Setup the python3 include path
# In the makefile.config the user can specify this as PY3INC
# We then need to copy that to PYINC so it just works with the included makefile
ifneq ($(strip ${PY3INC}),)
  PYINC := ${PY3INC}
else
  # Not given, so pick a default python3 include path
  PYINC := -I/gsa/rchgsa/projects/e/ecmd/utils/python-3.4.1/x86_64/include/python3.4m
endif

# Set the VPATH to pickup all the python 2 files
VPATH := ../pyapi:${VPATH}

# Set testBuild.py to use python3
# Allow the user a way to set the python3 binary path
ifneq ($(strip ${ECMDPYTHON3BIN}),)
  ECMDPYTHONBIN := ${ECMDPYTHON3BIN}
else
  ECMDPYTHONBIN := /gsa/rchgsa/projects/e/ecmd/utils/python-3.4.1/x86_64/bin/python3
endif

# Set the CFLAGS to pickup out of the pyapi directory
CFLAGS := -I../pyapi ${CFLAGS}
# As of 07/24/2014 - this ECMD_PY3API define is ONLY needed to properly generate
# the typemap in ecmdCommon.i.  That 1 additional line is needed for python 3.  JTA
SWIGFLAGS := -py3 -I../pyapi -DECMD_PY3API ${SWIGFLAGS}

# Include the python 2 makefile
include ../pyapi/makefile
