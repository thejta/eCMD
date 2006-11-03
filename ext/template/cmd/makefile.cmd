# Makefile for the ecmd Extensions
# Written by Chris Engel

# $Header$

ECMD_ROOT       := ${PWD}/../../../
# The default build rules
include ${ECMD_ROOT}/makefile.rules

EXTENSION_NAME_u := $(shell echo ${EXTENSION_NAME} | tr 'a-z' 'A-Z')
EXTENSION_NAME_u1 := $(shell perl -e 'printf(ucfirst(${EXTENSION_NAME}))')

INCLUDES     := ${INCLUDES} ${EXTENSION_NAME}Interpreter.H 
CAPI_INCLUDES := ${CAPI_INCLUDES} ${EXTENSION_NAME}Structs.H ${EXTENSION_NAME}ClientCapi.H
INT_INCLUDES := ecmdClientCapi.H  ecmdDataBuffer.H  ecmdReturnCodes.H ecmdStructs.H ecmdUtils.H ecmdClientEnums.H ${CAPI_INCLUDES}

#DEFINES      := 
CFLAGS       := ${CFLAGS} -I. -I../../../capi/export -I../capi/export -I../../../ecmd/ -g

SOURCE       := ${SOURCE} ${EXTENSION_NAME}Interpreter.C


# *****************************************************************************
# The x86 Linux Setup stuff
# *****************************************************************************
ifeq (${OS},Linux_x86)
  TARGET = ${EXTENSION_NAME}CmdInterpreter_x86.a
  CFLAGS := ${CFLAGS} -ftemplate-depth-30 -Wall
endif

# *****************************************************************************
# The ppc Linux Setup stuff
# *****************************************************************************
ifeq (${OS},Linux_ppc)
  TARGET = ${EXTENSION_NAME}CmdInterpreter_ppc.a
  CFLAGS := ${CFLAGS} -ftemplate-depth-30 -Wall
endif

# *****************************************************************************
# The Aix Setup stuff
# *****************************************************************************
ifeq (${OS},AIX)
  TARGET = ${EXTENSION_NAME}CmdInterpreter_aix.a
  CFLAGS  := ${CFLAGS} -+ -qstaticinline -qnoinline
endif

VPATH := ${VPATH}${SUBDIR}:../../../capi/export:../../template/capi:../capi/export


# *****************************************************************************
# The Main Targets
# *****************************************************************************
all: dir ${TARGET} 
	@touch t.o t.a
	@mv *.o *.a ${SUBDIR}
	@rm ${SUBDIR}/t.o ${SUBDIR}/t.a
	@echo "Exporting ${EXTENSION_NAME_u} eCMD Extension Command Interpreter to export/ ..."
	@cp -p ${SUBDIR}${TARGET} export/
	@cp -p ${INCLUDES} export/
	@cp -p  $(foreach file, ${CAPI_INCLUDES}, ../capi/${file}) export/


clean: objclean exportclean

objclean:
	rm -rf ${SUBDIR}

exportclean:
	rm -rf export/

install:
	@echo "Installing ${EXTENSION_NAME_u} eCMD Extension Command Interpreter to ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/ ..."
	@mkdir -p ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/
	cp export/${TARGET} ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/.
	@cp ${EXTENSION_NAME}Interpreter.H ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/.
	@cp ../capi/${EXTENSION_NAME}Structs.H ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/.
	@cp ../capi/${EXTENSION_NAME}ClientCapi.H ${INSTALL_PATH}/ext/${EXTENSION_NAME}/cmd/.

dir:
	@mkdir -p ${SUBDIR}
	@mkdir -p export



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
$(SOURCE_OBJS): %.o : %.C ${INCLUDES} ${INT_INCLUDES}
	$(CC) -c $(CFLAGS) $< -o $@ $(DEFINES)


# *****************************************************************************
# Create the Client Archive
# *****************************************************************************
${TARGET}: ${SOURCE_OBJS} ${LINK_OBJS}
	ar r $@ $^


