# Makefile for the ecmd Capi
# Written by Chris Engel

# $Header$

OS       := $(shell uname)

INCLUDES := ecmdClientCapi.H  ecmdDataBuffer.H  ecmdDllCapi.H  ecmdReturnCodes.H
SOURCE   := ecmdDataBuffer.C ecmdClientCapi.C
DEFINES  :=
CFLAGS   := -I.


# *****************************************************************************
# The Linux Setup stuff
# *****************************************************************************
ifeq (${OS},Linux)
  SUBDIR   := linux/
  CC := g++
  TARGET = ecmdClientCapi_x86.a
endif

# *****************************************************************************
# The Aix Setup stuff
# *****************************************************************************
ifeq (${OS},AIX)
  SUBDIR  := aix/
  CC := xlC -+
  TARGET = ecmdClientCapi_aix.a
endif



# *****************************************************************************
# The Main Targets
# *****************************************************************************
all: dir ${TARGET}

clean:
	rm -rf ${SUBDIR}

dir:
	@mkdir -p ${SUBDIR}



# *****************************************************************************
# Object Build Targets
# *****************************************************************************
SOURCE_OBJS  = $(basename $(SOURCE))
SOURCE_OBJS := $(addsuffix .o, $(SOURCE_OBJS))

# *****************************************************************************
# Compile code for the common C++ objects if their respective
# code has been changed.  Or, compile everything if a header
# file has changed.
# *****************************************************************************
$(SOURCE_OBJS): %.o : %.C ${INCLUDES}
	$(CC) -c $(CFLAGS) $< -o ${SUBDIR}$@ $(DEFINES)


# *****************************************************************************
# Create the Client Archive
# *****************************************************************************
${TARGET}: ${SOURCE_OBJS}
	ar r $@ $(addprefix ${SUBDIR}, $^)
