# Makefile for the ecmd Perl Module
# Written by Chris Engel

# $Header$

OS           := $(shell uname)
SITE         := $(shell fs wscell | cut -d\' -f2)

# Ok, find our CTE path if they don't already have one
ifeq ($(strip $(CTEPATH)),)
  CTEPATH      := "UNKNOWN CTE CELL"
  ifeq (${SITE},rchland.ibm.com)
    CTEPATH    := /afs/rchland.ibm.com/rel/common/cte
  endif
  ifeq (${SITE},awd.austin.ibm.com)
    CTEPATH    := /afs/awd.austin.ibm.com/projects/cte
  endif
  ifeq (${SITE},apd.pok.ibm.com)
    CTEPATH    := /afs/apd.pok.ibm.com/func/vlsi/cte
  endif
  ifeq (${SITE},vlsilab.boeblingen.ibm.com)
    CTEPATH    := /afs/vlsilab.boeblingen.ibm.com/proj/cte
  endif
endif

VPATH        := ../capi/export:../capi
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
  LDFLAGS   := -shared -e dll.exp -L../capi/export -lecmd_x86
  LINK_OBJS := ../capi/export/ecmdClientCapi_x86.a
  CFLAGS    := ${CFLAGS} -ftemplate-depth-30
  PERLINC   := -I${CTEPATH}/tools/perl/5.8.1/lib/5.8.1/i386_linux24/CORE
  SWIG      := ${CTEPATH}/tools/ecmd/utils/swig/bin/swig
  SWIGFLAGS := ${SWIGFLAGS} -I${CTEPATH}/tools/ecmd/utils/swig/lib -I${CTEPATH}/tools/ecmd/utils/swig/lib/perl5
  GPATH   := ${SUBDIR}
endif

# *****************************************************************************
# The Aix Setup stuff
# *****************************************************************************
ifeq (${OS},AIX)
  SUBDIR    := aix/
  ifeq (${SITE},rchland.ibm.com)
    CC      := /afs/rchland.ibm.com/rs_aix51/lpp/vacpp.6002/usr/vacpp/bin/xlC_r.6002
  else
    CC      := xlC
  endif
  TARGET    := ecmd_aix_perl
  LD        := /afs/rch/rs_aix51/lpp/vacpp.6002/usr/vacpp/bin/makeC++SharedLib_r
  LDFLAGS   := -e cronuspm.export -p 0 -lpthreads -bI:${CTEPATH}/tools/perl/5.8.1/lib/5.8.1/rs_aix51/CORE/perl.exp ../capi/export/libecmd_aix.so
# Can't seem to get aix to take the shared lib
# -L../capi/export -lecmd_aix
  LINK_OBJS := ../capi/export/ecmdClientCapi_aix.a
  CFLAGS    := ${CFLAGS}  -qstaticinline -qnoinline -+
  PERLINC   := -I${CTEPATH}/tools/perl/5.8.1/lib/5.8.1/rs_aix51/CORE
  SWIG      := ${CTEPATH}/tools/ecmd/utils/swig/bin/swig
  SWIGFLAGS := ${SWIGFLAGS} -I${CTEPATH}/tools/ecmd/utils/swig/lib -I${CTEPATH}/tools/ecmd/utils/swig/lib/perl5
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
	rm -f *.o *wrap*

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


