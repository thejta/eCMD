# Makefile for the ecmd Perl Module
# Written by Chris Engel

# $Header$

OS           := $(shell uname)

VPATH        := ../capi/export
INCLUDES     := ecmdClientCapi.H  ecmdDataBuffer.H  ecmdReturnCodes.H ecmdStructs.H
INT_INCLUDES := 
SOURCE       := ecmdClientPerlapi.C
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
  SWIGFLAGS := ${SWIGFLAGS} -I/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/tools/swig/linux/swig/lib -I/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/tools/swig/linux/swig/lib/swig_lib/perl5
  GPATH   := ${SUBDIR}
endif

# *****************************************************************************
# The Aix Setup stuff
# *****************************************************************************
ifeq (${OS},AIX)
  SUBDIR    := aix/
  CC        := xlC -+
  TARGET    := ecmd_aix_perl
  LD        := makeC++SharedLib
  LDFLAGS   := -e cronuspm.export -p 0 -lpthreads -bI:/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/common/contrib/aix/perl/lib/5.6.1/aix/CORE/perl.exp
  LINK_OBJS := ../capi/export/ecmdClientCapi_aix.a
  CFLAGS    := ${CFLAGS} -qstaticinline
  PERLINC   := -I/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/common/contrib/aix/perl/lib/5.6.1/aix/CORE
  SWIG      := /afs/rchland.ibm.com/rel/common/bringup.tools/cronus/tools/swig/aix/swig/bin/swig
  SWIGFLAGS := ${SWIGFLAGS} -I/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/tools/swig/aix/swig/lib -I/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/tools/swig/aix/swig/lib/swig_lib/perl5 
  DEFINES   := ${DEFINES} -DHAS_BOOL=1
endif

VPATH     := ${VPATH}:${SUBDIR}

# *****************************************************************************
# The Main Targets
# *****************************************************************************
all: dir ${TARGET}.so
	@touch t.o t_x86 t_aix t_ppc
	@mv *.o ${SUBDIR}
	@mv *x86* *_aix* *_ppc*  ${SUBDIR}
	@echo "Exporting eCMD Client to export/ ..."
	@cp ${SUBDIR}${TARGET}.so export/
	@cp ${SUBDIR}${TARGET}.pm export/
	@cp ChipTarget.pm export/

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

ecmdClientPerlapi_wrap${TARGET}.c: ecmdClientPerlapi.i
	sed "s/MODULE_NAME/${TARGET}/g" ecmdClientPerlapi.i > ecmdClientPerlApi_new${TARGET}.i
	${SWIG} ${SWIGFLAGS} -o ecmdClientPerlapi_wrap${TARGET}.c ecmdClientPerlApi_new${TARGET}.i	

ecmdClientPerlapi_wrap${TARGET}.o: ecmdClientPerlapi_wrap${TARGET}.c
	${CC} -c ${CFLAGS} ${PERLINC} $< -o $@ ${DEFINES}

# *****************************************************************************
# Create the Client 
# *****************************************************************************
${TARGET}.so: ${SOURCE_OBJS}  ecmdClientPerlapi_wrap${TARGET}.o
	${LD} ${LDFLAGS} $^ ${LINK_OBJS} -o $@ 


