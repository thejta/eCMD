# $Header$
# Makefile for the ecmd Extensions

# Base info and default build rules
SUBDIR     := ext/${EXTENSION_NAME}/cmd/
include ../../../makefile.rules

EXTENSION_NAME_u := $(shell echo ${EXTENSION_NAME} | tr 'a-z' 'A-Z')
EXTENSION_NAME_u1 := $(shell perl -e 'printf(ucfirst(${EXTENSION_NAME}))')

INCLUDES     := ${INCLUDES} ${EXTENSION_NAME}Interpreter.H 
CAPI_INCLUDES := ${CAPI_INCLUDES} ${EXTENSION_NAME}Structs.H ${EXTENSION_NAME}ClientCapi.H
INT_INCLUDES := ecmdClientCapi.H  ecmdDataBuffer.H  ecmdReturnCodes.H ecmdStructs.H ecmdUtils.H ecmdClientEnums.H ${CAPI_INCLUDES}

#DEFINES      := 
CFLAGS       := ${CFLAGS} -I../../../capi -I../capi -I../../../ecmd/

SOURCE       := ${SOURCE} ${EXTENSION_NAME}Interpreter.C

# *****************************************************************************
# The Common Setup stuff
# *****************************************************************************
TARGET = ${EXTENSION_NAME}CmdInterpreter_${OS}.a

# *****************************************************************************
# The x86 Linux Setup stuff
# *****************************************************************************
ifeq (${OS},x86)
  CFLAGS := ${CFLAGS} -ftemplate-depth-30 -Wall
endif

# *****************************************************************************
# The ppc Linux Setup stuff
# *****************************************************************************
ifeq (${OS},ppc)
  CFLAGS := ${CFLAGS} -ftemplate-depth-30 -Wall
endif

# *****************************************************************************
# The Aix Setup stuff
# *****************************************************************************
ifeq (${OS},aix)
  CFLAGS  := ${CFLAGS} -+ -qstaticinline -qnoinline
endif

VPATH := ${VPATH}:${OBJPATH}:../../../capi:../../template/capi:../capi


# *****************************************************************************
# The Main Targets
# *****************************************************************************
all: dir ${TARGET} 

clean: objclean

objclean:
	rm -rf ${OBJPATH}

install:
	@echo "Installing ${EXTENSION_NAME_u} eCMD Extension Command Interpreter to ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/ ..."
	@mkdir -p ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/
	cp ${OBJPATH}/${TARGET} ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/.
	@cp ${EXTENSION_NAME}Interpreter.H ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/.
	@cp ../capi/${EXTENSION_NAME}Structs.H ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/.
	@cp ../capi/${EXTENSION_NAME}ClientCapi.H ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/.

dir:
	@mkdir -p ${OBJPATH}



# *****************************************************************************
# Object Build Targets
# *****************************************************************************
SOURCE_OBJS  = $(basename $(SOURCE))
SOURCE_OBJS := $(addprefix ${OBJPATH}, $(SOURCE_OBJS))
SOURCE_OBJS := $(addsuffix .o, $(SOURCE_OBJS))

# *****************************************************************************
# Compile code for the common C++ objects if their respective
# code has been changed.  Or, compile everything if a header
# file has changed.
# *****************************************************************************
$(SOURCE_OBJS): ${OBJPATH}%.o : %.C ${INCLUDES} ${INT_INCLUDES}
	$(CC) -c $(CFLAGS) $< -o $@ $(DEFINES)


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

