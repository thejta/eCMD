# Makefile for the ecmd CIP Extension Capi
# Written by Chris Engel

# $Header$

OS           := $(shell uname)

INCLUDES     := cipClientCapi.H cipStructs.H
INT_INCLUDES := ecmdClientCapi.H  ecmdDataBuffer.H  ecmdReturnCodes.H ecmdStructs.H ecmdUtils.H ecmdClientEnums.H
SOURCE       := cipClientCapiFunc.C
DEFINES      := 
CFLAGS       := -I. -I../../capi/export -g


# *****************************************************************************
# The Linux Setup stuff
# *****************************************************************************
ifeq (${OS},Linux)
  SUBDIR   := linux/
  CC := g++
  TARGET = cipClientCapi_x86.a
  CFLAGS := ${CFLAGS} -ftemplate-depth-25
  GPATH   := ${SUBDIR}
endif

# *****************************************************************************
# The Aix Setup stuff
# *****************************************************************************
ifeq (${OS},AIX)
  SUBDIR  := aix/
  CC := xlC -+
  TARGET = cipClientCapi_aix.a
  CFLAGS  := ${CFLAGS} -qstaticinline
endif

VPATH := ${SUBDIR}:../../capi/export


# *****************************************************************************
# The Main Targets
# *****************************************************************************
all: dir ${TARGET}
	@touch t.o t.a
	@mv *.o *.a ${SUBDIR}
	@rm ${SUBDIR}/t.o ${SUBDIR}/t.a
	@echo "Exporting CIP eCMD Client Extension Api to export/ ..."
	@cp ${SUBDIR}${TARGET} export/
	@cp ${INCLUDES} export/

clean:
	rm -rf ${SUBDIR}
	rm cipClientCapiFunc.C

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
	ar r $@ $^



# *****************************************************************************
# Autogenerate the Client side of the Dll
# *****************************************************************************
cipClientCapiFunc.C cipDllCapi.H cipClientEnums.H: cipClientCapi.H makedll.pl 
	@echo "==== Auto Generate Client Functions"
	@chmod 777 makedll.pl
	./makedll.pl cip
