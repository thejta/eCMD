# Makefile for the ecmd Capi
# Written by Chris Engel

# $Header$

OS           := $(shell uname)

INCLUDES     := ecmdClientCapi.H  ecmdDataBuffer.H  ecmdReturnCodes.H ecmdStructs.H ecmdUtils.H ecmdClientEnums.H ecmdSharedUtils.H
INT_INCLUDES := ecmdDllCapi.H
CAPI_SOURCE  := ecmdClientCapi.C ecmdUtils.C ecmdClientCapiFunc.C
SLIB_SOURCE  := ecmdDataBuffer.C ecmdStructs.C ecmdSharedUtils.C
DEFINES      := 
CFLAGS       := -I. -g


# *****************************************************************************
# The Linux Setup stuff
# *****************************************************************************
ifeq (${OS},Linux)
  SUBDIR   := linux/
  CC       := g++
  LD       := ${CC}
  TARGET   := ecmdClientCapi_x86.a
  SLIB     := libecmd_x86.so
  CFLAGS   := ${CFLAGS} -ftemplate-depth-25
  SLIBLDFLAGS :=  -shared  -e sdll.exp
  GPATH   := ${SUBDIR}
endif

# *****************************************************************************
# The Aix Setup stuff
# *****************************************************************************
ifeq (${OS},AIX)
  SUBDIR  := aix/
  CC      := xlC -+
  LD      := makeC++SharedLib
  TARGET  := ecmdClientCapi_aix.a
  SLIB    := libecmd_aix.so
  CFLAGS  := ${CFLAGS} -qstaticinline
  SLIBLDFLAGS :=  -e libecmd_aix.export -p 0
endif

VPATH := ${SUBDIR}


# *****************************************************************************
# The MPatrol Stuff
# *****************************************************************************
MPATROL_HOME := /afs/rchland/rel/common/bringup.tools/cronus/ecmd/mpatrol/
ifeq (${MPATROL},yes) 
  DEFINES   := ${DEFINES} -DENABLE_MPATROL

  ifeq (${OS},Linux)
    CFLAGS  := ${CFLAGS} -fcheck-memory-usage -I${MPATROL_HOME}include/
    LDFLAGS := ${LDFLAGS} -L${MPATROL_HOME}x86/ -lmpatrol -lbfd 
    TARGET   := ecmdClientCapi_x86_mp.a
    SLIB     := libecmd_x86_mp.so
  endif

  ifeq (${OS},AIX)
    CFLAGS  := ${CFLAGS} -I${MPATROL_HOME}include/
    LDFLAGS := ${LDFLAGS} -lld ${MPATROL_HOME}/aix/libmpatrol.a
#    SLIBLDFLAGS := ${SLIBLDFLAGS} -lld -bI:/afs/rch/rel/common/bringup.tools/cronus/ecmd/mpatrol/aix/mpatrol.exp
    SLIBLDFLAGS := ${SLIBLDFLAGS} -lld ${MPATROL_HOME}/aix/libmpatrol.a
    TARGET  := ecmdClientCapi_aix_mp.a
    SLIB    := libecmd_aix_mp.so
  endif


endif

# *****************************************************************************
# The Main Targets
# *****************************************************************************
all: dir ${TARGET} ${SLIB}
	@touch t.o t.a t.so
	@mv *.o *.a *.so ${SUBDIR}
	@rm ${SUBDIR}/t.o ${SUBDIR}/t.a ${SUBDIR}/t.so
	@echo "Exporting eCMD Client Api/Shared Lib to export/ ..."
	@cp ${SUBDIR}${TARGET} export/
	@cp ${SUBDIR}${SLIB} export/
	@cp ${INCLUDES} export/
	-@cp ecmdDllCapi.H ../dll/

clean:
	rm -rf ${SUBDIR} export/
	rm ecmdClientCapiFunc.C
	rm ecmdDllCapi.H

dir:
	@mkdir -p ${SUBDIR}
	@mkdir -p export



# *****************************************************************************
# Object Build Targets
# *****************************************************************************
CAPI_SOURCE_OBJS  = $(basename $(CAPI_SOURCE))
CAPI_SOURCE_OBJS := $(addsuffix .o, $(CAPI_SOURCE_OBJS))
SLIB_SOURCE_OBJS  = $(basename $(SLIB_SOURCE))
SLIB_SOURCE_OBJS := $(addsuffix .o, $(SLIB_SOURCE_OBJS))

# *****************************************************************************
# Compile code for the common C++ objects if their respective
# code has been changed.  Or, compile everything if a header
# file has changed.
# *****************************************************************************
$(CAPI_SOURCE_OBJS): %.o : %.C ${INCLUDES} ${INT_INCLUDES}
	$(CC) -c $(CFLAGS) $< -o $@ $(DEFINES)
$(SLIB_SOURCE_OBJS): %.o : %.C ${INCLUDES} ${INT_INCLUDES}
	$(CC) -c $(CFLAGS) $< -o $@ $(DEFINES)


# *****************************************************************************
# Create the Client Archive
# *****************************************************************************
${TARGET}: ${CAPI_SOURCE_OBJS}
	@echo "Creating Static client library ..."
	ar r $@ $^

# *****************************************************************************
# Create the Shared Library
# *****************************************************************************
${SLIB}: ${SLIB_SOURCE_OBJS}
	@echo "Linking Shared Library ..."
	$(LD) ${SLIBLDFLAGS} -o $@ $^ 



# *****************************************************************************
# Autogenerate the Client side of the Dll
# *****************************************************************************
ecmdClientCapiFunc.C ecmdDllCapi.H ecmdClientEnums.H: ecmdClientCapi.H makedll.pl 
	@echo "==== Auto Generate Client Functions"
	@chmod 777 makedll.pl
	./makedll.pl ecmd
