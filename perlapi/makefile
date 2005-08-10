# Makefile for the ecmd Perl Module
# Written by Chris Engel

# $Header$

OS           := $(shell uname)
SITE         := $(shell fs wscell | cut -d\' -f2)


# List the extensions to include in the compile
EXTENSIONS   := cip cmd gip

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
INCLUDES     := ecmdClientCapi.H  ecmdDataBuffer.H  ecmdReturnCodes.H ecmdStructs.H ecmdClientPerlapi.H
INT_INCLUDES := 
SOURCE       := ecmdClientPerlapi.C ecmdClientPerlapiFunc.C
MAKEPM_OUT   := ecmdClientPerlapiFunc.C ecmdClientPerlapiFunc.H ecmdClientPerlapiIterators.H ecmdClientPerlapiIterators.pl ecmdRenames.i
DEFINES      := 
CFLAGS       := -I. -I../capi/export -g -I../dll
SWIGFLAGS    := -c++ -perl5 -exportall -const -I../capi/export -I../dll -DECMD_PERLAPI

# Ok, tack on all the extension stuff
VPATH        := ${VPATH}$(foreach ext, ${EXTENSIONS},:../ext/${ext}/capi/export:../ext/${ext}/perlapi)
#These are needed to define a space char for substitutions later on
empty :=
space := ${empty} ${empty}
VPATH := $(subst ${space},,${VPATH})

INCLUDES     := ${INCLUDES} $(foreach ext, ${EXTENSIONS}, ${ext}ClientPerlapi.H )
ifeq (${OS},Linux)
  LINK_OBJS  := $(foreach ext, ${EXTENSIONS}, ../ext/${ext}/capi/export/${ext}ClientCapi_x86.a)
endif
ifeq (${OS},AIX)
  LINK_OBJS  := $(foreach ext, ${EXTENSIONS}, ../ext/${ext}/capi/export/${ext}ClientCapi_aix.a)
endif
CFLAGS       := ${CFLAGS} $(foreach ext, ${EXTENSIONS}, -I../ext/${ext}/capi/export -I../ext/${ext}/perlapi)
SWIGFLAGS    := ${SWIGFLAGS} $(foreach ext, ${EXTENSIONS}, -I../ext/${ext}/perlapi)
SOURCE       := ${SOURCE} $(foreach ext, ${EXTENSIONS}, ${ext}ClientPerlapi.C ${ext}ClientPerlapiFunc.C)
MAKEPM_OUT   := ${MAKEPM_OUT} $(foreach ext, ${EXTENSIONS}, ${ext}ClientPerlapiFunc.C)
MAKEPM_OUT   := ${MAKEPM_OUT} $(foreach ext, ${EXTENSIONS}, ${ext}ClientPerlapiFunc.H)

# *****************************************************************************
# The Linux Setup stuff
# *****************************************************************************
ifeq (${OS},Linux)
  SUBDIR    := obj_linux/
  CC        := g++
  TARGET    := ecmd
  LD        := ${CC}
  LDFLAGS   := -shared -e dll.exp -L../capi/export -lecmd_x86
  LINK_OBJS := ${LINK_OBJS} ../capi/export/ecmdClientCapi_x86.a 
  CFLAGS    := ${CFLAGS} -ftemplate-depth-30 -Wall
  PERLINC   := -I${CTEPATH}/tools/perl/5.8.1/lib/5.8.1/i386_linux24/CORE
  SWIG      := ${CTEPATH}/tools/ecmd/utils/swig/bin/swig
  SWIGFLAGS := ${SWIGFLAGS} -I${CTEPATH}/tools/ecmd/utils/swig/lib -I${CTEPATH}/tools/ecmd/utils/swig/lib/perl5
  GPATH   := ${SUBDIR}
endif

# *****************************************************************************
# The Aix Setup stuff
# *****************************************************************************
ifeq (${OS},AIX)
  SUBDIR    := obj_aix/
# Pick the compiler, for Rochester,Austin,Pok you can use a local compiler which is 6.0.0.8, all other sites must run remote from rochester
  CC      := /afs/rchland.ibm.com/rs_aix51/lpp/vacpp.6008/usr/vacpp/bin/xlC_r.6008
  ifeq (${SITE},apd.pok.ibm.com)
    CC      := xlC_r
  endif
  ifeq (${SITE},awd.austin.ibm.com)
    CC      := xlC_r
  endif
# And the Shared Lib linker - same rules as above
  LD      := /afs/rch/rs_aix51/lpp/vacpp.6008/usr/vacpp/bin/makeC++SharedLib_r
  ifeq (${SITE},awd.austin.ibm.com)
    LD      := /usr/vacpp/bin/makeC++SharedLib_r
  endif
  ifeq (${SITE},apd.pok.ibm.com)
    LD      := /usr/vacpp/bin/makeC++SharedLib_r
  endif

  TARGET    := ecmd
  LDFLAGS   := -e cronuspm.export -p 0 -lpthreads -bI:${CTEPATH}/tools/perl/5.8.1/lib/5.8.1/rs_aix51/CORE/perl.exp -brtl -L ../capi/export -l ecmd_aix
  LINK_OBJS := ${LINK_OBJS} ../capi/export/ecmdClientCapi_aix.a
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
all: dir ${TARGET}.so ${TARGET}.pm test
	@touch t.o tecmd.so tecmd.pm t_wrap.c
	@mv *_wrap.c *.o *ecmd.so *ecmd.pm  ${SUBDIR}
	@echo "Exporting eCMD PerlApi to ${OS}/ ..."
	@cp ${SUBDIR}${TARGET}.so ${OS}/
	@cp ${SUBDIR}${TARGET}.pm ${OS}/
ifeq (${OS},AIX)
	@slibclean
endif

clean:
	rm -rf ${SUBDIR}
	rm -f *.o *wrap*
	rm -f *ClientPerlapiFunc.*
	rm -f ecmdClientPerlapiIterators.pl
	rm -f ecmdClientPerlapiIterators.H
	rm -f ecmdRename.i

objclean:
	rm -rf ${SUBDIR}
	rm -f *.o

dir:
	@mkdir -p ${SUBDIR}
	@mkdir -p ${OS}

test:
	@echo "***** If you see perl load errors this build of the perl module is invalid, ignore ECMD_DLL_FILE errors ****"
	@./testBuild.pl


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
$(SOURCE_OBJS): %.o : %.C ${INCLUDES} ${INT_INCLUDES} ${MAKEPM_OUT}
	$(CC) -c $(CFLAGS) ${PERLINC} $< -o $@ $(DEFINES)

${TARGET}.pm ecmdClientPerlapi_wrap.c: ${INCLUDES} ecmdClientPerlapi.i ecmdString.i ecmdVector.i ecmdList.i ecmdCommon.i ecmdClientPerlapi.pl 
	${SWIG} ${SWIGFLAGS} -o ecmdClientPerlapi_wrap.c ecmdClientPerlapi.i	

ecmdClientPerlapi_wrap.o: ecmdClientPerlapi_wrap.c
	${CC} -c ${CFLAGS} ${PERLINC} $< -o $@ ${DEFINES}

# *****************************************************************************
# Create the Perl Shared Library
# *****************************************************************************
${TARGET}.so: ${SOURCE_OBJS}  ecmdClientPerlapi_wrap.o  
	${LD} ${LDFLAGS} $^ ${LINK_OBJS} -o $@ 


# *****************************************************************************
# Autogenerate the Perl Wrapper
# *****************************************************************************
${MAKEPM_OUT}: ${INCLUDES} makepm.pl 
	@echo "==== Auto Generate Perl Wrapper Functions"
	@chmod 777 makepm.pl
ifeq (${SITE},awd.austin.ibm.com)
	/afs/awd/projects/gr/lab/local/perl5/bin/perl makepm.pl ecmd
	$(foreach ext, ${EXTENSIONS}, ${shell /afs/awd/projects/gr/lab/local/perl5/bin/perl makepm.pl ${ext} })
else
	./makepm.pl ecmd
	$(foreach ext, ${EXTENSIONS}, ${shell ./makepm.pl ${ext} })
endif

