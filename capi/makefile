# Makefile for the ecmd Capi
# Written by Chris Engel

# $Header$

OS           := $(shell uname)

INCLUDES     := ecmdClientCapi.H  ecmdDataBuffer.H  ecmdReturnCodes.H ecmdStructs.H
INT_INCLUDES := ecmdDllCapi.H
SOURCE       := ecmdDataBuffer.C ecmdClientCapi.C ecmdClientCapiFunc.C
DEFINES      := 
CFLAGS       := -I. -g


# *****************************************************************************
# The Linux Setup stuff
# *****************************************************************************
ifeq (${OS},Linux)
  SUBDIR   := linux/
  CC := g++
  TARGET = ecmdClientCapi_x86.a
endif

# *****************************************************************************
# The Aix Setup stuff
# *****************************************************************************
ifeq (${OS},AIX)
  SUBDIR  := aix/
  CC := xlC -+
  TARGET = ecmdClientCapi_aix.a
endif

VPATH := ${SUBDIR}


# *****************************************************************************
# The Main Targets
# *****************************************************************************
all: dir ${TARGET}
	@touch t.o t.a
	@mv *.o *.a ${SUBDIR}
	@rm ${SUBDIR}/t.o ${SUBDIR}/t.a
	@echo "Exporting eCMD Client Api to export/ ..."
	@cp ${SUBDIR}${TARGET} export/
	@cp ${INCLUDES} export/

clean:
	rm -rf ${SUBDIR}
	rm ecmdClientCapiFunc.C
	rm ecmdDllCapi.H

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
ecmdClientCapiFunc.C ecmdDllCapi.H: ecmdClientCapi.H makedll.pl 
	@echo "==== Auto Generate Client Functions"
	@chmod 777 makedll.pl
	./makedll.pl 
