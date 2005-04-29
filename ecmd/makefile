# Makefile for the ecmd Capi
# Written by Chris Engel

# $Header$

OS           := $(shell uname)
SITE         := $(shell fs wscell | cut -d\' -f2)

VPATH        := ../capi/export
INCLUDES     := ecmdClientCapi.H  ecmdDataBuffer.H  ecmdReturnCodes.H ecmdStructs.H ecmdUtils.H ecmdSharedUtils.H
INT_INCLUDES := ecmdInterpreter.H
SOURCE       := ecmdMain.C ecmdInterpreter.C ecmdScomUser.C ecmdCommandUtils.C ecmdRingUser.C ecmdQueryUser.C ecmdSpyUser.C ecmdSimUser.C ecmdArrayUser.C ecmdJtagUser.C ecmdProcUser.C ecmdMemUser.C ecmdIstepUser.C ecmdMiscUser.C ecmdVpdUser.C
DEFINES      := 
CFLAGS       := -I. -I../capi/export -I../dll -g 


VER_SOURCE   := ecmdVersion.C

# Common stuff for all extension interpreters
EXTENSIONS   := cip cro eip

VPATH        := ${VPATH}$(foreach ext, ${EXTENSIONS},:../ext/${ext}/cmd/export::../ext/${ext}/capi/export)
#These are needed to define a space char for substitutions later on
empty :=
space := ${empty} ${empty}
VPATH := $(subst ${space},,${VPATH})

INCLUDES     := ${INCLUDES} $(foreach ext, ${EXTENSIONS}, ${ext}Interpreter.H ${ext}ClientCapi.H ${ext}Structs.H)
ifeq (${OS},Linux)
  LINK_OBJS  := $(foreach ext, ${EXTENSIONS}, ${ext}ClientCapi_x86.a ${ext}CmdInterpreter_x86.a)
endif
ifeq (${OS},AIX)
  LINK_OBJS  := $(foreach ext, ${EXTENSIONS}, ${ext}ClientCapi_aix.a ${ext}CmdInterpreter_aix.a)
endif
CFLAGS       := ${CFLAGS} $(foreach ext, ${EXTENSIONS}, -I../ext/${ext}/cmd/export)



# *****************************************************************************
# The Linux Setup stuff
# *****************************************************************************
ifeq (${OS},Linux)
  SUBDIR  := linux/
  CC      := g++
  TARGET  := ecmd_x86
  VER_TARGET  := ecmdVersion_x86
  LD      := ${CC}
  LDFLAGS := -ldl -L../capi/export -lecmd_x86
  LINK_OBJS := ${LINK_OBJS} ../capi/export/ecmdClientCapi_x86.a
  CFLAGS := ${CFLAGS} -ftemplate-depth-30 -Wall
  GPATH   := ${SUBDIR}
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

  TARGET  := ecmd_aix
  VER_TARGET  := ecmdVersion_aix
  LD      := ${CC}
  LDFLAGS := -brtl -bloadmap:PARM -w -L../capi/export
  ifeq (${MPATROL},yes) 
    LINK_OBJS := ${LINK_OBJS} ../capi/export/ecmdClientCapi_aix_mp.a
    LDFLAGS := ${LDFLAGS}  -lecmd_aix_mp
  else
    LINK_OBJS := ${LINK_OBJS} ../capi/export/ecmdClientCapi_aix.a
    LDFLAGS := ${LDFLAGS}  -lecmd_aix 
  endif
  CFLAGS  := ${CFLAGS} -+ -qstaticinline -qnoinline
endif

VPATH     := ${VPATH}:${SUBDIR}


# *****************************************************************************
# The MPatrol Stuff
# *****************************************************************************
ifeq (${SITE},rchland.ibm.com)
  MPATROL_HOME := /afs/rchland/rel/common/cte/tools/ecmd/mpatrol
endif
ifeq (${SITE},awd.austin.ibm.com)
  MPATROL_HOME := /afs/awd.austin.ibm.com/projects/cte/tools/ecmd/mpatrol
endif

ifeq (${MPATROL},yes) 
  DEFINES   := ${DEFINES} -DENABLE_MPATROL

  ifeq (${OS},Linux)
    CFLAGS  := ${CFLAGS} -fcheck-memory-usage -I${MPATROL_HOME}include/
    LDFLAGS := ${LDFLAGS} -L${MPATROL_HOME}x86/ -lmpatrol -lbfd 
  endif

  ifeq (${OS},AIX)
    CFLAGS  := ${CFLAGS} -I${MPATROL_HOME}include/
    LDFLAGS := ${LDFLAGS} -lld ${MPATROL_HOME}/aix/libmpatrol.a
  endif


endif


# *****************************************************************************
# The Main Targets
# *****************************************************************************
all: dir ${TARGET} ${VER_TARGET}
	@touch t.o t_x86 t_aix t_ppc
	@mv *.o *_x86 *_aix *_ppc  ${SUBDIR}
	@echo "Exporting eCMD Client to export/ ..."
	@cp -p ${SUBDIR}${TARGET} export/
	@cp -p ${SUBDIR}${VER_TARGET} export/

clean:
	rm -rf ${SUBDIR}

dir:
	@mkdir -p ${SUBDIR}
	@mkdir -p export

# Build the extensions
extensions:
	@echo " "
	@echo "Building Main Client Api ..."
	@cd ../capi;gmake
	@echo " "
	@echo "Building Extension Client Api's ..."
	@cd ../ext/template/capi;$(foreach ext, ${EXTENSIONS}, cd ../../${ext}/capi;gmake; echo " ";)
	@echo " "
	@echo "Building Extension Command Interpreters ..."
	@cd ../ext/template/cmd;$(foreach ext, ${EXTENSIONS}, cd ../../${ext}/cmd;gmake; echo " ";)
	@echo " "
	@echo "Building Main Command Interpreter ..."

# *****************************************************************************
# Object Build Targets
# *****************************************************************************
SOURCE_OBJS  = $(basename $(SOURCE))
SOURCE_OBJS := $(addsuffix .o, $(SOURCE_OBJS))
VER_SOURCE_OBJS  = $(basename $(VER_SOURCE))
VER_SOURCE_OBJS := $(addsuffix .o, $(VER_SOURCE_OBJS))

# *****************************************************************************
# Compile code for the common C++ objects if their respective
# code has been changed.  Or, compile everything if a header
# file has changed.
# *****************************************************************************
$(SOURCE_OBJS): %.o : %.C ${INCLUDES} ${INT_INCLUDES}
	$(CC) -c $(CFLAGS) $< -o $@ $(DEFINES)
$(VER_SOURCE_OBJS): %.o : %.C ${INCLUDES} 
	$(CC) -c $(CFLAGS) $< -o $@ $(DEFINES)


# *****************************************************************************
# Create the Client 
# *****************************************************************************
${TARGET}: ${SOURCE_OBJS} ${LINK_OBJS}
	${LD} ${LDFLAGS} $^ -o $@ 

${VER_TARGET}: ${VER_SOURCE_OBJS}
ifeq (${OS},AIX)
	${LD}  -brtl -bloadmap:PARM -w $^ -o $@ 
endif
ifeq (${OS},Linux)
	${LD}  -ldl $^ -o $@ 
endif
