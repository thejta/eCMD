# Makefile for the ecmd Extensions
# Written by Chris Engel

# $Header$



EXTENSION_NAME_u := $(shell echo ${EXTENSION_NAME} | tr 'a-z' 'A-Z')
EXTENSION_NAME_u1 := $(shell perl -e 'printf(ucfirst(${EXTENSION_NAME}))')

OS           := $(shell uname)
SITE         := $(shell fs wscell | cut -d\' -f2)

INCLUDES     := ${EXTENSION_NAME}Interpreter.H 
INT_INCLUDES := ecmdClientCapi.H  ecmdDataBuffer.H  ecmdReturnCodes.H ecmdStructs.H ecmdUtils.H ecmdClientEnums.H ${EXTENSION_NAME}Structs.H ${EXTENSION_NAME}ClientCapi.H

DEFINES      := 
CFLAGS       := ${CFLAGS} -I. -I../../../capi/export -I../capi/export -I../../../ecmd/ -g

SOURCE       := ${SOURCE} ${EXTENSION_NAME}Interpreter.C






# *****************************************************************************
# The Linux Setup stuff
# *****************************************************************************
ifeq (${OS},Linux)
  SUBDIR   := linux/
  CC := g++
  TARGET = ${EXTENSION_NAME}CmdInterpreter_x86.a
  CFLAGS := ${CFLAGS} -ftemplate-depth-30 -Wall
  GPATH   := ${SUBDIR}
endif

# *****************************************************************************
# The Aix Setup stuff
# *****************************************************************************
ifeq (${OS},AIX)
  SUBDIR  := aix/
# Pick the compiler, for Rochester,Austin,Pok you can use a local compiler which is 6.0.0.8, all other sites must run remote from rochester
  CC      := /afs/rchland.ibm.com/rs_aix51/lpp/vacpp.6008/usr/vacpp/bin/xlC.6008
  ifeq (${SITE},apd.pok.ibm.com)
    CC      := xlC
  endif
  ifeq (${SITE},awd.austin.ibm.com)
    CC      := xlC
  endif

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
	@cp -p ../capi/${EXTENSION_NAME}ClientCapi.H ../capi/${EXTENSION_NAME}Structs.H  export/


clean objclean:
	rm -rf ${SUBDIR}

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


