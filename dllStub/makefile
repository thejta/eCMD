# Makefile for the ecmd Capi
# Written by Chris Engel

# $Header$

OS           := $(shell uname)

VPATH        := ../capi
INCLUDES     := ecmdDataBuffer.H  ecmdReturnCodes.H
INT_INCLUDES := ecmdDllCapi.H 
SOURCE       := ecmdDllCapiTest.C ecmdDllCapi.C
DEFINES      := 
CFLAGS       := -I. -I../capi -g
LDFLAGS      :=


# *****************************************************************************
# The Linux Setup stuff
# *****************************************************************************
ifeq (${OS},Linux)
  SUBDIR   := linux/
  CC := g++
  LD := ${CC}
  TARGET = ecmdDllStub_x86.so
  LDFLAGS := ${LDFLAGS} -shared  -e dll.exp 
endif

# *****************************************************************************
# The Aix Setup stuff
# *****************************************************************************
ifeq (${OS},AIX)
  SUBDIR  := aix/
  CC := xlC -+
  LD := makeC++SharedLib
  TARGET = ecmdDllStub_aix.so
  LDFLAGS := ${LDFLAGS} -e ecmdDllStub_aix.export -p 0
  CFLAGS  := ${CFLAGS} -qstaticinline
endif

VPATH     := ${VPATH}:${SUBDIR}


# *****************************************************************************
# The Main Targets
# *****************************************************************************
all: dir ${TARGET}
	@touch t.o t.so
	@mv *.o *.so  ${SUBDIR}
	@echo "Exporting eCMD Dll Stub to export/ ..."
	@cp ${SUBDIR}${TARGET} export/
ifeq (${OS},AIX)
	@cp ecmdDllStub_aix.export export/
endif

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
	$(CC) -c $(CFLAGS) $< -o $@ $(DEFINES)


# *****************************************************************************
# Create the Client Archive
# *****************************************************************************
${TARGET}: ${SOURCE_OBJS}
	$(LD) ${LDFLAGS} -o $@ $^


