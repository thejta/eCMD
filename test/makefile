# Makefile for the ecmd Capi
# Written by Chris Engel

# $Header$

OS           := $(shell uname)

VPATH        := ../capi/export:../dllStub:../capi:../ecmd/
INCLUDES     := ecmdClientCapi.H  ecmdDataBuffer.H  ecmdReturnCodes.H ecmdStructs.H
INT_INCLUDES := 
SOURCE       := ecmdLoadTest.C
STATIC_SOURCE := ecmdDllCapiTest.C ecmdDllCapi.C ecmdDataBuffer.C ecmdClientCapiFunc.C ecmdClientCapi.C ecmdUtils.C
ECMD_SOURCE  := ecmdCommandUtils.C ecmdDaScomUser.C ecmdInterpreter.C ecmdMain.C
ECMD_INCLUDES := ecmdCommandUtils.H ecmdIntReturnCodes.H ecmdInterpreter.H
DEFINES      := 
CFLAGS       := -I. -I../capi/export -I../capi/ -I ../ecmd/ -g


# *****************************************************************************
# The Linux Setup stuff
# *****************************************************************************
ifeq (${OS},Linux)
  SUBDIR  := linux/
  CC      := g++
  TARGET  := ecmdClientTest_x86
  EC_TARGET := ecmd_x86
  LD      := ${CC}
  LDFLAGS := -ldl
endif

# *****************************************************************************
# The Aix Setup stuff
# *****************************************************************************
ifeq (${OS},AIX)
  SUBDIR  := aix/
  CC      := xlC -+
  TARGET  := ecmdClientTest_aix
  EC_TARGET := ecmd_aix
  CFLAGS  := ${CFLAGS} -qstaticinline
  LD      := ${CC}
endif

VPATH     := ${VPATH}:${SUBDIR}


# *****************************************************************************
# The Main Targets
# *****************************************************************************
all: dir ${TARGET} ${TARGET}_static
	@touch t.o t_x86 t_aix t_ppc t_static
	@mv *.o *_x86 *_aix *_ppc *_static ${SUBDIR}
	@echo "Exporting eCMD Client to export/ ..."
	@cp ${SUBDIR}${TARGET} export/
	@cp ${SUBDIR}${TARGET}_static export/
	@cp ${SUBDIR}${EC_TARGET} export/


cmd: ${EC_TARGET}

clean:
	rm -rf ${SUBDIR}

dir:
	@mkdir -p ${SUBDIR}
	@mkdir -p export



# *****************************************************************************
# Object Build Targets
# *****************************************************************************
SOURCE_OBJS  = $(basename $(SOURCE))
SOURCE_OBJS := $(addsuffix .o, $(SOURCE_OBJS))
STATIC_SOURCE_OBJS  = $(basename $(SOURCE)) $(basename $(STATIC_SOURCE))
STATIC_SOURCE_OBJS := $(addsuffix _static.o, $(STATIC_SOURCE_OBJS))
EC_SOURCE_OBJS  = $(basename $(ECMD_SOURCE))
EC_SOURCE_OBJS := $(addsuffix _static.o, $(EC_SOURCE_OBJS))


# *****************************************************************************
# Compile code for the common C++ objects if their respective
# code has been changed.  Or, compile everything if a header
# file has changed.
# *****************************************************************************
$(SOURCE_OBJS): %.o : %.C ${INCLUDES} ${INT_INCLUDES}
	$(CC) -c $(CFLAGS) $< -o $@ $(DEFINES)
$(STATIC_SOURCE_OBJS): %_static.o : %.C ${INCLUDES} ${INT_INCLUDES}
	$(CC) -c $(CFLAGS) $< -o $@ $(DEFINES) -DECMD_STATIC_FUNCTIONS
$(EC_SOURCE_OBJS): %_static.o : %.C ${INCLUDES} ${INT_INCLUDES} ${ECMD_INCLUDES}
	$(CC) -c $(CFLAGS) $< -o $@ $(DEFINES) -DECMD_STATIC_FUNCTIONS


# *****************************************************************************
# Create the Client 
# *****************************************************************************
${TARGET}: ${SOURCE_OBJS}
	@echo " "
	@echo "Linking Dynamic version of test client ..."
ifeq (${OS},Linux)
	${LD} ${LDFLAGS} $^ ../capi/export/ecmdClientCapi_x86.a -o $@ 
endif
ifeq (${OS},AIX)
	${LD} ${LDFLAGS} $^ ../capi/export/ecmdClientCapi_aix.a -o $@ 
endif

${TARGET}_static: ${STATIC_SOURCE_OBJS}
	@echo " "
	@echo "Linking Static version of test client ..."
	${LD} ${LDFLAGS} $^  -o $@ 

${EC_TARGET}: ${EC_SOURCE_OBJS} ${STATIC_SOURCE_OBJS}
	@echo " "
	@echo "Linking Static version of command line ..."
	${LD} ${LDFLAGS} $^  -o $@ 


