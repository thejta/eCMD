# Makefile for the ecmd Capi
# Written by Chris Engel

# $Header$

ECMD_ROOT    := ${PWD}/../
# The default build rules
include ${ECMD_ROOT}makefile.rules

VPATH        := ../capi/export
INCLUDES     := ecmdClientCapi.H  ecmdDataBuffer.H  ecmdReturnCodes.H ecmdStructs.H ecmdUtils.H ecmdSharedUtils.H
INT_INCLUDES := ecmdInterpreter.H ecmdCommandUtils.H
SOURCE       := ecmdMain.C ecmdInterpreter.C ecmdScomUser.C ecmdCommandUtils.C ecmdRingUser.C ecmdQueryUser.C ecmdSpyUser.C ecmdSimUser.C ecmdArrayUser.C ecmdJtagUser.C ecmdProcUser.C ecmdMemUser.C ecmdIstepUser.C ecmdMiscUser.C ecmdVpdUser.C ecmdI2cGpioUser.C ecmdPowerUser.C
#DEFINES      := 
CFLAGS       := ${CFLAGS} -I. -I../capi/export -I../dll -g 


VER_SOURCE   := ecmdVersion.C

# Don't want to include the cmd extension
EXTENSIONS   := $(subst cmd,,${EXTENSIONS})
# Don't want to include the scand extension, there isn't one
EXTENSIONS   := $(subst scand,,${EXTENSIONS})


VPATH        := ${VPATH}$(foreach ext, ${EXTENSIONS},:../ext/${ext}/cmd/export::../ext/${ext}/capi/export)
#These are needed to define a space char for substitutions later on
empty :=
space := ${empty} ${empty}
VPATH := $(subst ${space},,${VPATH})

INCLUDES     := ${INCLUDES} $(foreach ext, ${EXTENSIONS}, ${ext}Interpreter.H ${ext}ClientCapi.H ${ext}Structs.H)
ifeq (${OS},Linux_x86)
  LINK_OBJS  := $(foreach ext, ${EXTENSIONS}, ${ext}ClientCapi_x86.a ${ext}CmdInterpreter_x86.a)
endif
ifeq (${OS},Linux_ppc)
  LINK_OBJS  := $(foreach ext, ${EXTENSIONS}, ${ext}ClientCapi_ppc.a ${ext}CmdInterpreter_ppc.a)
endif
ifeq (${OS},AIX)
  LINK_OBJS  := $(foreach ext, ${EXTENSIONS}, ${ext}ClientCapi_aix.a ${ext}CmdInterpreter_aix.a)
endif
CFLAGS       := ${CFLAGS} $(foreach ext, ${EXTENSIONS}, -I../ext/${ext}/cmd/export)



# *****************************************************************************
# The x86 Linux Setup stuff
# *****************************************************************************
ifeq (${OS},Linux_x86)
  TARGET  := ecmd_x86
  VER_TARGET  := ecmdVersion_x86
  LDFLAGS := ${LDFLAGS} -ldl -L../capi/export -lecmd_x86
  LINK_OBJS := ${LINK_OBJS} ../capi/export/ecmdClientCapi_x86.a
  CFLAGS := ${CFLAGS} -ftemplate-depth-30 -Wall
endif

# *****************************************************************************
# The ppc Linux Setup stuff
# *****************************************************************************
ifeq (${OS},Linux_ppc)
  TARGET  := ecmd_ppc
  VER_TARGET  := ecmdVersion_ppc
  LDFLAGS := ${LDFLAGS} -ldl -L../capi/export -lecmd_ppc
  LINK_OBJS := ${LINK_OBJS} ../capi/export/ecmdClientCapi_ppc.a
  CFLAGS := ${CFLAGS} -ftemplate-depth-30 -Wall
  GPATH   := ${SUBDIR}
endif

# *****************************************************************************
# The Aix Setup stuff
# *****************************************************************************
ifeq (${OS},AIX)
  TARGET  := ecmd_aix
  VER_TARGET  := ecmdVersion_aix
  LD      := ${CC}  # This is right, we want to overload the default of makeC++SharedLib
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

  ifeq (${OS},Linux_x86)
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
	@echo "Exporting eCMD Client to export/ ..."
	@-mv *.o ${TARGET} ${VER_TARGET} ${SUBDIR}
	@cp -p ${SUBDIR}${TARGET} export/
	@cp -p ${SUBDIR}${VER_TARGET} export/

clean: objclean exportclean

objclean:
	rm -rf ${SUBDIR}

exportclean:
	rm -rf export/

dir:
	@mkdir -p ${SUBDIR}
	@mkdir -p export

install:
	cp export/${TARGET} ${INSTALL_PATH}/bin/
	cp export/${VER_TARGET} ${INSTALL_PATH}/bin/


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
	${LD} ${LDFLAGS}  -brtl -bloadmap:PARM -w $^ -o $@ 
endif
ifeq (${OS},Linux_x86)
	${LD} ${LDFLAGS} -ldl $^ -o $@ 
endif
ifeq (${OS},Linux_ppc)
	${LD} ${LDFLAGS} -ldl $^ -o $@ 
endif
