# Makefile for the ecmd Capi
# Written by Chris Engel

# $Header$

OS           := $(shell uname)
SITE         := $(shell fs wscell | cut -d\' -f2)

VPATH        := ../capi/export
INCLUDES     := ecmdClientCapi.H  ecmdDataBuffer.H  ecmdReturnCodes.H ecmdStructs.H ecmdUtils.H ecmdSharedUtils.H
INT_INCLUDES := ecmdInterpreter.H
SOURCE       := ecmdMain.C ecmdInterpreter.C ecmdScomUser.C ecmdCommandUtils.C ecmdRingUser.C ecmdQueryUser.C ecmdSpyUser.C ecmdSimUser.C ecmdArrayUser.C ecmdJtagUser.C ecmdProcUser.C ecmdMemUser.C ecmdIstepUser.C ecmdMiscUser.C
DEFINES      := 
CFLAGS       := -I. -I../capi/export -g 


# Common stuff for all extension interpreters
EXTENSIONS   := cip cro

VPATH        := ${VPATH}$(foreach ext, ${EXTENSIONS},:../ext/${ext}/cmd:../ext/${ext}/capi/export)
INCLUDES     := ${INCLUDES} $(foreach ext, ${EXTENSIONS}, ${ext}Interpreter.H ${ext}ClientCapi.H ${ext}Structs.H)
SOURCE       := ${SOURCE} $(foreach ext, ${EXTENSIONS}, ${ext}Interpreter.C)
ifeq (${OS},Linux)
  LINK_OBJS  := $(foreach ext, ${EXTENSIONS}, ../ext/${ext}/capi/export/${ext}ClientCapi_x86.a)
endif
ifeq (${OS},AIX)
  LINK_OBJS  := $(foreach ext, ${EXTENSIONS}, ../ext/${ext}/capi/export/${ext}ClientCapi_aix.a)
endif
CFLAGS       := ${CFLAGS} $(foreach ext, ${EXTENSIONS}, -I../ext/${ext}/capi/export -I../ext/${ext}/cmd)

# The CIP Interpreter specific source
SOURCE       := ${SOURCE} cipProcUser.C

# The Cro Interpreter specific source


# *****************************************************************************
# The Linux Setup stuff
# *****************************************************************************
ifeq (${OS},Linux)
  SUBDIR  := linux/
  CC      := g++
  TARGET  := ecmd_x86
  LD      := ${CC}
  LDFLAGS := -ldl -L../capi/export -lecmd_x86
  LINK_OBJS := ${LINK_OBJS} ../capi/export/ecmdClientCapi_x86.a
  CFLAGS := ${CFLAGS} -ftemplate-depth-30
  GPATH   := ${SUBDIR}
endif

# *****************************************************************************
# The Aix Setup stuff
# *****************************************************************************
ifeq (${OS},AIX)
  SUBDIR  := aix/
  ifeq (${SITE},rchland.ibm.com)
    CC      := /afs/rchland.ibm.com/rs_aix51/lpp/vacpp.6002/usr/vacpp/bin/xlC.6002
  else
    CC      := xlC
  endif
  TARGET  := ecmd_aix
  LD      := ${CC}
  LDFLAGS := -brtl -bloadmap:PARM -w -L../capi/export -lecmd_aix
  LINK_OBJS := ${LINK_OBJS} ../capi/export/ecmdClientCapi_aix.a
  CFLAGS  := ${CFLAGS} -+ -qstaticinline -qnoinline
endif

VPATH     := ${VPATH}:${SUBDIR}


# *****************************************************************************
# The MPatrol Stuff
# *****************************************************************************
MPATROL_HOME := /afs/rchland/rel/common/bringup.tools/cronus/ecmd/mpatrol/
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
all: dir ${TARGET}
	@touch t.o t_x86 t_aix t_ppc
	@mv *.o *_x86 *_aix *_ppc  ${SUBDIR}
	@echo "Exporting eCMD Client to export/ ..."
	@cp ${SUBDIR}${TARGET} export/

clean:
	rm -rf ${SUBDIR}

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
# Create the Client 
# *****************************************************************************
${TARGET}: ${SOURCE_OBJS} ${LINK_OBJS}
	${LD} ${LDFLAGS} $^ -o $@ 


