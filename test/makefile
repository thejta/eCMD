# Makefile for the ecmd Capi
# Written by Chris Engel

# $Header$

OS           := $(shell uname)

VPATH        := ../capi/export ../dllStub ../capi
INCLUDES     := ecmdClientCapi.H  ecmdDataBuffer.H  ecmdReturnCodes.H ecmdStructs.H
INT_INCLUDES := 
SOURCE       := ecmdLoadTest.C
STATIC_SOURCE := ecmdDllCapiTest.C ecmdDllCapi.C ecmdDataBuffer.C ecmdClientCapiFunc.C ecmdClientCapi.C
DEFINES      := 
CFLAGS       := -I. -I../capi/export -I../capi/ -g


# *****************************************************************************
# The Linux Setup stuff
# *****************************************************************************
ifeq (${OS},Linux)
  SUBDIR  := linux/
  CC      := g++
  TARGET  := ecmdClientTest_x86
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
endif



# *****************************************************************************
# The Main Targets
# *****************************************************************************
all: dir ${TARGET} ${TARGET}_static
	@echo "Exporting eCMD Client to export/ ..."
	@cp ${SUBDIR}${TARGET} export/
	@cp ${SUBDIR}${TARGET}_static export/

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


# *****************************************************************************
# Compile code for the common C++ objects if their respective
# code has been changed.  Or, compile everything if a header
# file has changed.
# *****************************************************************************
$(SOURCE_OBJS): %.o : %.C ${INCLUDES} ${INT_INCLUDES}
	$(CC) -c $(CFLAGS) $< -o ${SUBDIR}$@ $(DEFINES)
$(STATIC_SOURCE_OBJS): %_static.o : %.C ${INCLUDES} ${INT_INCLUDES}
	$(CC) -c $(CFLAGS) $< -o ${SUBDIR}$@ $(DEFINES) -DECMD_STATIC_FUNCTIONS


# *****************************************************************************
# Create the Client 
# *****************************************************************************
${TARGET}: ${SOURCE_OBJS}
	@echo "Linking Dynamic version of test client ..."
	${LD} ${LDFLAGS} $(addprefix ${SUBDIR}, $^) ../capi/export/ecmdClientCapi_x86.a -o ${SUBDIR}$@ 

${TARGET}_static: ${STATIC_SOURCE_OBJS}
	@echo "Linking Static version of test client ..."
	${LD} ${LDFLAGS} $(addprefix ${SUBDIR}, $^)  -o ${SUBDIR}$@ 

