# Makefile for the ecmd capi

# *****************************************************************************
# Define base info and include any global variables
# *****************************************************************************
SUBDIR     := capi/
include ../../makefile.vars

### Includes
CAPI_INCLUDES := ecmdClientCapi.H  ecmdDataBufferBase.H ecmdDataBuffer.H ecmdReturnCodes.H 
CAPI_INCLUDES := ${CAPI_INCLUDES} ecmdStructs.H ecmdUtils.H ecmdClientEnums.H ecmdSharedUtils.H 
CAPI_INCLUDES := ${CAPI_INCLUDES} ecmdDefines.H  prdfCompressBuffer.H ecmdTraceDefines.H
INT_INCLUDES  := ecmdDllCapi.H

### Source
CAPI_SOURCE := ecmdClientCapi.C ecmdUtils.C ecmdClientCapiFunc.C
SLIB_SOURCE := ecmdDataBufferBase.C ecmdDataBuffer.C ecmdStructs.C ecmdSharedUtils.C

### Generated source by makedll.pl
GENERATED_INCLUDES := ecmdDllCapi.H ecmdClientEnums.H
GENERATED_SOURCE   := ecmdClientCapiFunc.C
CFLAGS             := ${CFLAGS} -I${SRCPATH}
VPATH              := ${VPATH}:${SRCPATH}

# Tell the SEDC code we import that we just want the scomdef parser
SEDC_INCLUDES := sedcScomdefParser.H sedcScomdefClasses.H sedcDefines.H 
SEDC_INCLUDES := ${SEDC_INCLUDES} sedcCommonParser.H sedcCommonClasses.H
CAPI_SOURCE   := ${CAPI_SOURCE} sedcScomdefParser.C sedcScomdefClasses.C 
CAPI_SOURCE   := ${CAPI_SOURCE} sedcCommonParser.C sedcCommonClasses.C
VPATH         := ${VPATH}:sedcScomdef
CFLAGS        := ${CFLAGS} -IsedcScomdef
# Variables for install
SEDC_INCLUDES_INSTALL := $(addprefix sedcScomdef/, ${SEDC_INCLUDES})

# Add on the object output path
VPATH := ${VPATH}:${OBJPATH}

# Compile libecmd optimized
SLIB_CFLAGS   := ${CFLAGS} ${OPT_LEVEL}

# Direct where to get zlib from
ifeq (${TARGET_ARCH},aix)
  ZLIB := /gsa/${GSACELL}/projects/e/ecmd/utils/aix/lib/libz.a
else ifeq (${TARGET_ARCH},x86)
  ZLIB := /gsa/${GSACELL}/projects/e/ecmd/utils/x86/lib/libz.a
else
  ZLIB := -lz
endif

### Concat variables together for the build
INCLUDES := ${CAPI_INCLUDES} ${SEDC_INCLUDES}

# *****************************************************************************
# Define our output files
# *****************************************************************************
TARGET   := ecmdClientCapi.a
SLIB     := libecmd.so

# *****************************************************************************
# The Main Targets
# *****************************************************************************
# The all rule variables are defined in makefile.vars
all:
	@${CMD_DIR_MSG}
	@${CMD_DIR}
	@${CMD_GEN_MSG}
	@${CMD_GEN}
	@${CMD_BLD_MSG}
	@${CMD_BLD}

generate: ${GENERATED_SOURCE} ${GENERATED_INCLUDES}

build: ${TARGET} ${SLIB}

install:
	@echo "Installing eCMD Client API to ${INSTALL_PATH}/capi/ ..."
	@mkdir -p ${INSTALL_PATH}/capi/
	cp ${OBJPATH}/${TARGET} ${INSTALL_PATH}/${TARGET_ARCH}/lib/.
	cp ${CAPI_INCLUDES} ${INSTALL_PATH}/capi/.
	cp ${SEDC_INCLUDES_INSTALL} ${INSTALL_PATH}/capi/.
	chmod 644 ${INSTALL_PATH}/capi/*
	@echo "Installing eCMD Shared Lib to ${INSTALL_PATH}/${TARGET_ARCH}/lib/ ..."
	cp ${OBJPATH}/${SLIB} ${INSTALL_PATH}/${TARGET_ARCH}/lib/.


# *****************************************************************************
# Object Build Targets
# *****************************************************************************
CAPI_SOURCE_OBJS := $(basename ${CAPI_SOURCE})
CAPI_SOURCE_OBJS := $(addprefix ${OBJPATH}, ${CAPI_SOURCE_OBJS})
CAPI_SOURCE_OBJS := $(addsuffix .o, ${CAPI_SOURCE_OBJS})
SLIB_SOURCE_OBJS := $(basename ${SLIB_SOURCE})
SLIB_SOURCE_OBJS := $(addprefix ${OBJPATH}, ${SLIB_SOURCE_OBJS})
SLIB_SOURCE_OBJS := $(addsuffix .o, ${SLIB_SOURCE_OBJS})

# *****************************************************************************
# Compile code for the common C++ objects if their respective
# code has been changed.  Or, compile everything if a header
# file has changed.
# *****************************************************************************
${CAPI_SOURCE_OBJS}: ${OBJPATH}%.o : %.C ${INCLUDES} ${INT_INCLUDES}
	@echo Compiling $<
	${VERBOSE}${CC} -c ${CFLAGS} $< -o $@ ${DEFINES}
${SLIB_SOURCE_OBJS}: ${OBJPATH}%.o : %.C ${INCLUDES} ${INT_INCLUDES}
	@echo Compiling $<
	${VERBOSE}${CC} -c ${SLIB_CFLAGS} $< -o $@ ${DEFINES}


# *****************************************************************************
# Create the Client Archive
# *****************************************************************************
${TARGET}: ${CAPI_SOURCE_OBJS}
	@echo Creating $@
	${VERBOSE}${AR} r ${OUTLIB}/${TARGET} $^

# *****************************************************************************
# Create the Shared Library
# *****************************************************************************
${SLIB}: ${SLIB_SOURCE_OBJS}
	@echo Linking $@
	${VERBOSE}${LD} ${SLDFLAGS} -o ${OUTLIB}/${SLIB} $^ ${ZLIB}

# *****************************************************************************
# Autogenerate the Client side of the Dll
# *****************************************************************************
${GENERATED_SOURCE} ${GENERATED_INCLUDES}: ecmdClientCapi.H ${ECMD_ROOT}/mkscripts/makedll.pl
	@echo "Generating $@"
	@${ECMD_ROOT}/mkscripts/makedll.pl ecmd $@

# *****************************************************************************
# Include any global default rules
# *****************************************************************************
include ${ECMD_ROOT}/makefile.rules
