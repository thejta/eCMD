# $Header$
# Makefile for the ecmd cmdline

# Base info and default build rules
SUBDIR     := ecmd/
include ../makefile.rules

VPATH        := ../capi
INCLUDES     := ecmdClientCapi.H  ecmdDataBuffer.H  ecmdReturnCodes.H ecmdStructs.H ecmdUtils.H ecmdSharedUtils.H
INT_INCLUDES := ecmdInterpreter.H ecmdCommandUtils.H
SOURCE       := ecmdMain.C ecmdInterpreter.C ecmdScomUser.C ecmdCommandUtils.C ecmdRingUser.C ecmdQueryUser.C ecmdSpyUser.C ecmdSimUser.C ecmdArrayUser.C ecmdJtagUser.C ecmdProcUser.C ecmdMemUser.C ecmdIstepUser.C ecmdMiscUser.C ecmdVpdUser.C ecmdI2cGpioUser.C ecmdPowerUser.C ecmdPsiUser.C
DEFINES      := 
CFLAGS       := ${CFLAGS} -I../capi -I../dll 

VER_SOURCE   := ecmdVersion.C

# Don't want to include the cmd extension
EXTENSIONS   := $(subst cmd,,${EXTENSIONS})
# Don't want to include the scand extension, there isn't one
EXTENSIONS   := $(subst scand,,${EXTENSIONS})

VPATH        := ${VPATH}$(foreach ext, ${EXTENSIONS},:${OBJROOT}/ext/${ext}/cmd/${OBJDIR}:${OBJROOT}/ext/${ext}/capi/${OBJDIR}:../ext/${ext}/cmd/:../ext/${ext}/capi/)
VPATH := $(subst ${space},,${VPATH})

INCLUDES     := ${INCLUDES} $(foreach ext, ${EXTENSIONS}, ${ext}Interpreter.H ${ext}ClientCapi.H ${ext}Structs.H)
CFLAGS       := ${CFLAGS} $(foreach ext, ${EXTENSIONS},-I../ext/${ext}/cmd -I../ext/${ext}/capi -DECMD_$(shell echo ${ext} | tr "a-zA-Z" "A-Za-z")_EXTENSION_SUPPORT)

# *****************************************************************************
# The Common Setup stuff
# *****************************************************************************
TARGET     := ecmd_${OS}
VER_TARGET := ecmdVersion_${OS}
LINK_OBJS  := ${LINK_OBJS} ${OBJROOT}/capi/${OBJDIR}/ecmdClientCapi_${OS}.a
LINK_OBJS  := ${LINK_OBJS} $(foreach ext, ${EXTENSIONS}, ${ext}ClientCapi_${OS}.a ${OBJROOT}/ext/${ext}/cmd/${OBJDIR}/${ext}CmdInterpreter_${OS}.a)

# *****************************************************************************
# The x86 Linux Setup stuff
# *****************************************************************************
ifeq (${OS},x86)
  LDFLAGS    := ${LDFLAGS} -ldl -L${OBJROOT}/capi/${OBJDIR} -lecmd_x86
  CFLAGS     := ${CFLAGS} -ftemplate-depth-30 -Wall
endif

# *****************************************************************************
# The ppc Linux Setup stuff
# *****************************************************************************
ifeq (${OS},ppc)
  LDFLAGS    := ${LDFLAGS} -ldl -L${OBJROOT}/capi/${OBJDIR} -lecmd_ppc
  CFLAGS     := ${CFLAGS} -ftemplate-depth-30 -Wall
  GPATH      := ${OBJDIR}
endif

# *****************************************************************************
# The Aix Setup stuff
# *****************************************************************************
ifeq (${OS},aix)
  LDFLAGS    := -brtl -w -L${OBJROOT}/capi/${OBJDIR} -lecmd_aix
  CFLAGS     := ${CFLAGS} -+ -qstaticinline -qnoinline
endif

VPATH     := ${VPATH}:${OBJPATH}

# *****************************************************************************
# The Main Targets
# *****************************************************************************
all: dir ${TARGET} ${VER_TARGET}

clean: objclean

objclean:
	rm -rf ${OBJPATH}

dir:
	@mkdir -p ${OBJPATH}

install:
	cp ${OBJPATH}/${TARGET} ${INSTALL_PATH}/bin/
	cp ${OBJPATH}/${VER_TARGET} ${INSTALL_PATH}/bin/

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
SOURCE_OBJS := $(addprefix ${OBJPATH}, $(SOURCE_OBJS))
SOURCE_OBJS := $(addsuffix .o, $(SOURCE_OBJS))
VER_SOURCE_OBJS  = $(basename $(VER_SOURCE))
VER_SOURCE_OBJS := $(addprefix ${OBJPATH}, $(VER_SOURCE_OBJS))
VER_SOURCE_OBJS := $(addsuffix .o, $(VER_SOURCE_OBJS))

# *****************************************************************************
# Compile code for the common C++ objects if their respective
# code has been changed.  Or, compile everything if a header
# file has changed.
# *****************************************************************************
$(SOURCE_OBJS): ${OBJPATH}%.o : %.C ${INCLUDES} ${INT_INCLUDES}
	$(CC) -c $(CFLAGS) $< -o $@ $(DEFINES)
$(VER_SOURCE_OBJS): ${OBJPATH}%.o : %.C ${INCLUDES} 
	$(CC) -c $(CFLAGS) $< -o $@ $(DEFINES)


# *****************************************************************************
# Create the Client 
# *****************************************************************************
${TARGET}: ${SOURCE_OBJS} ${LINK_OBJS}
	${LD} ${LDFLAGS} $^ -o ${OBJPATH}${TARGET}

${VER_TARGET}: ${VER_SOURCE_OBJS}
ifeq (${OS},aix)
	${LD} ${LDFLAGS}  -brtl -w $^ -o ${OBJPATH}${VER_TARGET}
endif
ifeq (${OS},x86)
	${LD} ${LDFLAGS} -ldl $^ -o ${OBJPATH}${VER_TARGET}
endif
ifeq (${OS},ppc)
	${LD} ${LDFLAGS} -ldl $^ -o ${OBJPATH}${VER_TARGET}
endif


# *****************************************************************************
# Debug rule for any makefile testing 
# *****************************************************************************
debug:
	@echo ${ECMD_ROOT}
	@echo ${SUBDIR}
