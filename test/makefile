# Makefile for the ecmd Capi
# Written by Chris Engel

# $Header$

OS           := $(shell uname)

VPATH        := ../capi/export
INCLUDES     := ecmdClientCapi.H  ecmdDataBuffer.H  ecmdReturnCodes.H ecmdStructs.H
INT_INCLUDES := 
SOURCE       := ecmdLoadTest.C
DEFINES      := 
CFLAGS       := -I. -I../capi/export -g


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
all: dir ${TARGET}
	@echo "Exporting eCMD Client to export/ ..."
	@cp ${SUBDIR}${TARGET} export/

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

# *****************************************************************************
# Compile code for the common C++ objects if their respective
# code has been changed.  Or, compile everything if a header
# file has changed.
# *****************************************************************************
$(SOURCE_OBJS): %.o : %.C ${INCLUDES} ${INT_INCLUDES}
	$(CC) -c $(CFLAGS) $< -o ${SUBDIR}$@ $(DEFINES)


# *****************************************************************************
# Create the Client 
# *****************************************************************************
${TARGET}: ${SOURCE_OBJS}
	${LD} ${LDFLAGS} $(addprefix ${SUBDIR}, $^) ../capi/export/ecmdClientCapi_x86.a -o ${SUBDIR}$@ 


