# Makefile for the ecmd Perl Module
# Written by Chris Engel

# $Header$

OS           := $(shell uname)
SITE         := $(shell fs wscell | cut -d\' -f2)

VPATH        := ../capi/export:../capi
INCLUDES     := ecmdClientCapi.H  ecmdDataBuffer.H  ecmdReturnCodes.H ecmdStructs.H
INT_INCLUDES := 
SOURCE       := ecmdClientPerlapi.C ecmdStructs.C ecmdSharedUtils.C ecmdDataBuffer.C
DEFINES      := 
CFLAGS       := -I. -I../capi/export -g 
SWIGFLAGS    := -c++ -perl5 -shadow 

# *****************************************************************************
# The Linux Setup stuff
# *****************************************************************************
ifeq (${OS},Linux)
  SUBDIR    := linux/
  CC        := g++
  TARGET    := ecmd_x86_perl
  LD        := ${CC}
  LDFLAGS   := -shared -e dll.exp
  LINK_OBJS := ../capi/export/ecmdClientCapi_x86.a
  CFLAGS    := ${CFLAGS} -ftemplate-depth-30
  PERLINC   := -I/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/common/contrib/linux/perl/lib/5.6.1/i386-linux/CORE
  SWIG      := /afs/rchland.ibm.com/rel/common/bringup.tools/cronus/tools/swig/linux/swig/bin/swig
  SWIGFLAGS := ${SWIGFLAGS} -I/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/tools/swig/linux/swig/lib -I/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/tools/swig/linux/swig/lib/swig_lib/perl5 -I/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/tools/swig/aix/swig/lib/swig_lib/
  GPATH   := ${SUBDIR}
endif

# *****************************************************************************
# The Aix Setup stuff
# *****************************************************************************
ifeq (${OS},AIX)
  SUBDIR    := aix/
  ifeq (${SITE},rchland.ibm.com)
    CC      := /afs/rchland.ibm.com/rs_aix51/lpp/vacpp.6002/usr/vacpp/bin/xlC.6002
  else
    CC      := xlC
  endif
  TARGET    := ecmd_aix_perl
  LD        := /afs/rch/rs_aix51/lpp/vacpp.6002/usr/vacpp/bin/makeC++SharedLib
  LDFLAGS   := -e cronuspm.export -p 0 -lpthreads -bI:/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/common/contrib/aix/perl/lib/5.6.1/aix/CORE/perl.exp
  LINK_OBJS := ../capi/export/ecmdClientCapi_aix.a
  CFLAGS    := ${CFLAGS}  -qstaticinline -qnoinline -+
  PERLINC   := -I/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/common/contrib/aix/perl/lib/5.6.1/aix/CORE
  SWIG      := /afs/rchland.ibm.com/rel/common/bringup.tools/cronus/tools/swig/aix/swig/bin/swig
  SWIGFLAGS := ${SWIGFLAGS} -I/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/tools/swig/aix/swig/lib -I/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/tools/swig/aix/swig/lib/swig_lib/perl5 -I/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/tools/swig/aix/swig/lib/swig_lib/
  DEFINES   := ${DEFINES} -DHAS_BOOL=1
endif

VPATH     := ${VPATH}:${SUBDIR}

# *****************************************************************************
# The Main Targets
# *****************************************************************************
all: dir ${TARGET}.so ${TARGET}.pm
	@touch t.o t_x86 t_aix t_ppc
	@mv *.o ${SUBDIR}
	@mv *x86* *_aix* *_ppc*  ${SUBDIR}
	@echo "Exporting eCMD Client to export/ ..."
	@cp ${SUBDIR}${TARGET}.so export/
	@cp ${SUBDIR}${TARGET}.pm export/
	@cp ChipTarget.pm export/
ifeq (${OS},AIX)
	@slibclean
endif

clean:
	rm -rf ${SUBDIR}
	rm -f *.o *_wrap.c

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

${TARGET}.pm ecmdClientPerlapi_wrap${TARGET}.c: ecmdClientPerlapi.i
	sed "s/MODULE_NAME/${TARGET}/g" ecmdClientPerlapi.i > ecmdClientPerlApi_new${TARGET}.i
	${SWIG} ${SWIGFLAGS} -o ecmdClientPerlapi_wrap${TARGET}.c ecmdClientPerlApi_new${TARGET}.i	

ecmdClientPerlapi_wrap${TARGET}.o: ecmdClientPerlapi_wrap${TARGET}.c
	${CC} -c ${CFLAGS} ${PERLINC} $< -o $@ ${DEFINES}

# *****************************************************************************
# Create the Client 
# *****************************************************************************
${TARGET}.so: ${SOURCE_OBJS}  ecmdClientPerlapi_wrap${TARGET}.o
	${LD} ${LDFLAGS} $^ ${LINK_OBJS} -o $@ 


