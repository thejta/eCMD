# $Header$
# Makefile for the ecmd Extensions

# Base info and default build rules
SUBDIR     := ext/${EXTENSION_NAME}/cmd/
include ../../../makefile.rules

EXTENSION_NAME_u := $(shell echo ${EXTENSION_NAME} | tr 'a-z' 'A-Z')
EXTENSION_NAME_u1 := $(shell perl -e 'printf(ucfirst(${EXTENSION_NAME}))')

INCLUDES     := ${INCLUDES} ${EXTENSION_NAME}Interpreter.H 
CAPI_INCLUDES := ${CAPI_INCLUDES} ${EXTENSION_NAME}Structs.H ${EXTENSION_NAME}ClientCapi.H
INT_INCLUDES := ecmdClientCapi.H  ecmdDataBufferBase.H  ecmdDataBuffer.H ecmdReturnCodes.H ecmdStructs.H ecmdUtils.H ecmdClientEnums.H ${CAPI_INCLUDES}

#DEFINES      := 
CFLAGS       := ${CFLAGS} -I../../../capi -I../capi -I../../../ecmd/ -I../../../dll

SOURCE       := ${SOURCE} ${EXTENSION_NAME}Interpreter.C

# *****************************************************************************
# The Common Setup stuff
# *****************************************************************************
#TARGET = ${EXTENSION_NAME}CmdInterpreter_${OS}.a
TARGET = ${EXTENSION_NAME}CmdInterpreter.a

VPATH := ${VPATH}:${OBJPATH}:../../../capi:../../template/capi:../capi


# *****************************************************************************
# The Main Targets
# *****************************************************************************
all: dir ${TARGET} 

clean: objclean

objclean:
	rm -rf ${OBJPATH}

#Remove create_links after fair warning has been given to users to update paths
install: create_links
	@echo "Installing ${EXTENSION_NAME_u} eCMD Extension Command Interpreter to ${INSTALL_PATH}/${TARGET_ARCH}/lib/ ..."
	@mkdir -p ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/
	cp ${OBJPATH}/${TARGET} ${INSTALL_PATH}/${TARGET_ARCH}/lib/.
	@echo "Installing ${EXTENSION_NAME_u} eCMD Extension Command Interpreter headers to ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/ ..."
	@cp ${EXTENSION_NAME}Interpreter.H ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/.
	@cp ../capi/${EXTENSION_NAME}Structs.H ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/.
	@cp ../capi/${EXTENSION_NAME}ClientCapi.H ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/.

create_links:
  ifeq (${TARGET_ARCH},$(filter ${TARGET_ARCH},x86 aix))
	@echo "Creating temporary symlinks for ${EXTENSION_NAME_u} eCMD Extension Command Interpreter"
	@mkdir -p ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/
	@ln -sf ../../../${TARGET_ARCH}/lib/${TARGET} ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/${EXTENSION_NAME}CmdInterpreter_${TARGET_ARCH}.a
  endif

dir:
	@mkdir -p ${OBJPATH}



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
	${CC} -c ${CFLAGS} $< -o $@ ${DEFINES}


# *****************************************************************************
# Create the Client Archive
# *****************************************************************************
${TARGET}: ${SOURCE_OBJS} ${LINK_OBJS}
	${AR} r ${OBJPATH}${TARGET} $^

# *****************************************************************************
# Debug rule for any makefile testing 
# *****************************************************************************
debug:
	@echo ${ECMD_ROOT}
	@echo ${SUBDIR}

