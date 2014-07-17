# Makefile for a very simple DLL.  Can be used to test builds

# Base info and default build rules
SUBDIR     := dllStub/
include ../makefile.rules

# *****************************************************************************
# The Common Setup stuff
# *****************************************************************************
TARGET       := stub.dll
CFLAGS       := ${CFLAGS} -I ../capi -I ../dll
VPATH        := ${VPATH}:../capi:../dll

# *****************************************************************************
# Setup all the files going into the build
# *****************************************************************************

# The ECMD_INCLUDES are files provided by eCMD that if changed, we would want to recompile on
ECMD_INCLUDES := ecmdClientCapi.H ecmdDataBuffer.H ecmdReturnCodes.H ecmdStructs.H ecmdUtils.H ecmdClientEnums.H ecmdSharedUtils.H ecmdDefines.H

# The source files and includes in our local dirs that are going into the build
TGT_INCLUDES  := ecmdDllCapi.H
TGT_SOURCE    := ecmdDllCapi.C ecmdDllStub.C ecmdDllStubInfo.C

# Combine all the includes into one variable for the build
INCLUDES      := ${ECMD_INCLUDES} ${TGT_INCLUDES}

# Pull the sim functions out of the stub DLL
DEFINES := ${DEFINES} -DREMOVE_SIM

# *****************************************************************************
# The Main Targets
# *****************************************************************************
all: dir ${TARGET}
        # Remove the object after each build to force a rebuild to update the date
	@rm ${OBJPATH}/ecmdDllStubInfo.o

clean: objclean

install:
  # Do nothing

objclean:
	rm -rf ${OBJPATH} 

dir:
	@mkdir -p ${OBJPATH}

# *****************************************************************************
# Object Build Targets
# *****************************************************************************
TGT_SOURCE_OBJS  = $(basename ${TGT_SOURCE})
TGT_SOURCE_OBJS := $(addprefix ${OBJPATH}, ${TGT_SOURCE_OBJS})
TGT_SOURCE_OBJS := $(addsuffix .o, ${TGT_SOURCE_OBJS})

# *****************************************************************************
# Compile code for the common C++ objects if their respective
# code has been changed.  Or, compile everything if a header
# file has changed.
# *****************************************************************************
${TGT_SOURCE_OBJS}: ${OBJPATH}%.o : %.C ${INCLUDES}
	${CC} -c ${CFLAGS} $< -o $@ ${DEFINES}

# *****************************************************************************
# Create the Target
# *****************************************************************************
${TARGET}: ${TGT_SOURCE_OBJS}
	${LD} ${SLDFLAGS} -o ${OBJPATH}${TARGET} $^ ../capi/obj_${TARGET_ARCH}/ecmdClientCapi.a -L../capi/obj_${TARGET_ARCH} -lecmd

# *****************************************************************************
# Debug rule for any makefile testing 
# *****************************************************************************
debug:
	@echo ${OBJROOT}
	@echo ${VPATH}


