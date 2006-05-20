# Makefile for the ecmd Capi
# Written by Chris Engel

# $Header$

OS           := $(shell uname)
SITE         := $(shell fs wscell | cut -d\' -f2)

# We need to do extra for Linux
ifeq (${OS},Linux)
  ifneq ($(strip $(shell uname -a |grep ppc)),)
    OS := Linux_ppc
  else
    OS := Linux_x86
  endif
endif

INCLUDES     := ecmdClientCapi.H  ecmdDataBuffer.H  ecmdReturnCodes.H ecmdStructs.H ecmdUtils.H ecmdClientEnums.H ecmdSharedUtils.H
EXPORTS      := ${INCLUDES}
INT_INCLUDES := ecmdDllCapi.H
CAPI_SOURCE  := ecmdClientCapi.C ecmdUtils.C ecmdClientCapiFunc.C
SLIB_SOURCE  := ecmdDataBuffer.C ecmdStructs.C ecmdSharedUtils.C
#DEFINES      := 
CFLAGS       := ${CFLAGS} -I. -g

# Tell the SEDC code we import that we just want the scomdef parser
INCLUDES     := ${INCLUDES} sedcScomdefParser.H sedcScomdefClasses.H sedcDefines.H sedcCommonParser.H sedcCommonClasses.H
CAPI_SOURCE  := ${CAPI_SOURCE} sedcScomdefParser.C sedcScomdefClasses.C sedcCommonParser.C sedcCommonClasses.C
VPATH        := ${VPATH}sedcScomdef
CFLAGS       := ${CFLAGS} -IsedcScomdef


# *****************************************************************************
# The Linux Setup stuff
# *****************************************************************************
ifeq (${OS},Linux_x86)
  SUBDIR   := linux/
  CC       := g++
  LD       := ${CC}
  TARGET   := ecmdClientCapi_x86.a
  SLIB     := libecmd_x86.so
  CFLAGS   := ${CFLAGS} -ftemplate-depth-25 -Wall
  SLIBLDFLAGS :=  -shared  -e sdll.exp
  GPATH   := ${SUBDIR}

# Let's see if we can use distcc
  ifneq (${DISTCC_HOSTS},)
    CC    := distcc ${CC}
  endif
endif

# *****************************************************************************
# The Linux Setup stuff
# *****************************************************************************
ifeq (${OS},Linux_ppc)
  SUBDIR   := linux_ppc/
  CC       := g++
  LD       := ${CC}
  TARGET   := ecmdClientCapi_ppc.a
  SLIB     := libecmd_ppc.so
  CFLAGS   := ${CFLAGS} -ftemplate-depth-25 -Wall
  SLIBLDFLAGS :=  -shared  -e sdll.exp
  GPATH   := ${SUBDIR}

# Let's see if we can use distcc
  ifneq (${DISTCC_HOSTS},)
    CC    := distcc ${CC}
  endif
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
# And the Shared Lib linker - same rules as above
  LD      := /afs/rch/rs_aix51/lpp/vacpp.6008/usr/vacpp/bin/makeC++SharedLib
  ifeq (${SITE},awd.austin.ibm.com)
    LD      := /usr/vacpp/bin/makeC++SharedLib
  endif
  ifeq (${SITE},apd.pok.ibm.com)
    LD      := /usr/vacpp/bin/makeC++SharedLib
  endif

  TARGET  := ecmdClientCapi_aix.a
  SLIB    := libecmd_aix.so
  CFLAGS  := ${CFLAGS} -+ -qstaticinline -qnoinline
  SLIBLDFLAGS :=  -e libecmd_aix.export -p 0
endif

VPATH := ${VPATH}:${SUBDIR}

# *****************************************************************************
# The MPatrol Stuff
# *****************************************************************************
ifeq (${SITE},rchland.ibm.com)
  MPATROL_HOME := /afs/rchland/rel/common/cte/tools/ecmd/utils/mpatrol/
endif
ifeq (${SITE},awd.austin.ibm.com)
  MPATROL_HOME := /afs/awd.austin.ibm.com/projects/cte/tools/ecmd/utils/mpatrol/
endif
ifeq (${MPATROL},yes) 
  DEFINES   := ${DEFINES} -DENABLE_MPATROL

  ifeq (${OS},Linux_x86)
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
	@cp -p ${SUBDIR}${TARGET} export/
	@cp -p ${SUBDIR}${SLIB} export/
	@cp -p ${EXPORTS} export/
	-@cp -p ecmdDllCapi.H ../dll/

clean:
	rm -rf ${SUBDIR} 
	rm -f ecmdClientCapiFunc.C
	rm -f ecmdDllCapi.H
	rm -f ecmdClientEnums.H

objclean:
	rm -rf ${SUBDIR} 


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
ifeq (${SITE},awd.austin.ibm.com)
	/afs/awd/projects/gr/lab/local/perl5/bin/perl makedll.pl ecmd
else
	./makedll.pl ecmd
endif
