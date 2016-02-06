# Makefile for the ecmd Extensions

# *****************************************************************************
# Define base info and include any global variables
# *****************************************************************************
SUBDIR     := ext/${EXTENSION_NAME}/cmd/

EXTENSION_NAME_u := $(shell echo ${EXTENSION_NAME} | tr 'a-z' 'A-Z')
EXTENSION_NAME_u1 := $(shell perl -e 'printf(ucfirst(${EXTENSION_NAME}))')

### Includes
INCLUDES      := ${INCLUDES} ${EXTENSION_NAME}Interpreter.H 
CAPI_INCLUDES := ${CAPI_INCLUDES} ${EXTENSION_NAME}Structs.H ${EXTENSION_NAME}ClientCapi.H
INT_INCLUDES  := ${INT_INCLUDES} ecmdClientCapi.H ecmdDataBufferBase.H ecmdDataBuffer.H
INT_INCLUDES  := ${INT_INCLUDES} ecmdReturnCodes.H ecmdStructs.H ecmdUtils.H ecmdClientEnums.H

### Source
SOURCE       := ${SOURCE} ${EXTENSION_NAME}Interpreter.C

### Variables used for the build
CFLAGS   := ${CFLAGS} -I${ECMD_CORE}/capi -I${ECMD_CORE}/cmd/ -I${EXT_${EXTENSION_NAME}_PATH}/capi -I${ECMD_CORE}/dll -I${SRCPATH}
INCLUDES := ${INCLUDES} ${CAPI_INCLUDES}
VPATH    := ${VPATH}:${OBJPATH}:${ECMD_CORE}/capi:${ECMD_CORE}/cmd:${EXT_${EXTENSION_NAME}_PATH}/capi:${SRCPATH}

# *****************************************************************************
# Define our output files
# *****************************************************************************
TARGET = ${EXTENSION_NAME}CmdInterpreter.a

# *****************************************************************************
# The Main Targets
# *****************************************************************************
# The run-all rule is defined in makefile.rules
all:
	${run-all}

generate:
  # Do nothing

build: ${TARGET}

test:
  # Do nothing

install:
	@echo "Installing ${EXTENSION_NAME_u} eCMD Extension Command Interpreter to ${INSTALL_PATH}/${TARGET_ARCH}/lib/ ..."
	@mkdir -p ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/
	cp ${OUTLIB}/${TARGET} ${INSTALL_PATH}/${TARGET_ARCH}/lib/.
	@echo "Installing ${EXTENSION_NAME_u} eCMD Extension Command Interpreter headers to ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/ ..."
	@cp ${EXTENSION_NAME}Interpreter.H ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/.
	@cp ../capi/${EXTENSION_NAME}Structs.H ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/.
	@cp ../capi/${EXTENSION_NAME}ClientCapi.H ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/.


# *****************************************************************************
# Object Build Targets
# *****************************************************************************
SOURCE_OBJS  = $(basename ${SOURCE})
SOURCE_OBJS := $(addprefix ${OBJPATH}, ${SOURCE_OBJS})
SOURCE_OBJS := $(addsuffix .o, ${SOURCE_OBJS})

# *****************************************************************************
# Compile code for the common C++ objects if their respective
# code has been changed.  Or, compile everything if a header
# file has changed.
# *****************************************************************************
${SOURCE_OBJS}: ${OBJPATH}%.o : %.C ${INCLUDES} ${INT_INCLUDES}
	@echo Compiling $<
	${VERBOSE}${CC} -c ${CFLAGS} $< -o $@ ${DEFINES}

# *****************************************************************************
# Create the Client Archive
# *****************************************************************************
${TARGET}: ${SOURCE_OBJS} ${LINK_OBJS}
	@echo Creating $@
	${VERBOSE}${AR} r ${OUTLIB}/${TARGET} $^

# *****************************************************************************
# Include any global default rules
# *****************************************************************************
include ${ECMD_ROOT}/makefile.rules
