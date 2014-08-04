# Base info and default build rules
SUBDIR     := py3api/
include ../makefile.rules

# Setup the python3 include path
# In the makefile.config the user can specify this as PY3INC
# We then need to copy that to PYINC so it just works with the included makefile
ifneq ($(strip ${PY3INC}),)
  PYINC := ${PY3INC}
else
  # Not given, so pick a default python3 include path
  PYINC := -I/usr/include/python3.3m
endif

# Set the VPATH to pickup all the python 2 files
VPATH := ../pyapi:${VPATH}

# Set testBuild.py to use python3
ECMDPYTHONBIN := /usr/bin/python3

# Set the CFLAGS to pickup out of the pyapi directory
CFLAGS := -I../pyapi ${CFLAGS}
# As of 07/24/2014 - this ECMD_PY3API define is ONLY needed to properly generate
# the typemap in ecmdCommon.i.  That 1 additional line is needed for python 3.  JTA
SWIGFLAGS := -py3 -I../pyapi -DECMD_PY3API ${SWIGFLAGS}

# Include the python 2 makefile
include ../pyapi/makefile
