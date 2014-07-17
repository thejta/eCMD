# $Header$
# Makefile for the ecmd cmdline

# Base info and default build rules
SUBDIR     := ecmd/
include ../makefile.rules

VPATH        := ../capi
INCLUDES     := ecmdClientCapi.H  ecmdDataBuffer.H  ecmdDataBufferBase.H ecmdReturnCodes.H ecmdStructs.H ecmdUtils.H ecmdSharedUtils.H
INT_INCLUDES := ecmdInterpreter.H ecmdCommandUtils.H
SOURCE       := ecmdMain.C ecmdInterpreter.C ecmdScomUser.C ecmdCommandUtils.C ecmdRingUser.C ecmdQueryUser.C ecmdSpyUser.C ecmdSimUser.C ecmdArrayUser.C ecmdJtagUser.C ecmdProcUser.C ecmdMemUser.C ecmdIstepUser.C ecmdMiscUser.C ecmdVpdUser.C ecmdI2cGpioUser.C ecmdPowerUser.C ecmdPsiUser.C
#DEFINES      := 
CFLAGS       := ${CFLAGS} -I../capi -I../dll 

VER_SOURCE   := ecmdVersion.C

# Don't want to include the cmd extension
EXTENSIONS   := $(subst cmd,,${EXTENSIONS})
# Don't want to include the scand extension, there isn't one
EXTENSIONS   := $(subst scand,,${EXTENSIONS})
# Don't want to include the fapi extension, there isn't one
EXTENSIONS   := $(subst fapi,,${EXTENSIONS})

VPATH        := ${VPATH}$(foreach ext, ${EXTENSIONS},:${OBJROOT}/ext/${ext}/cmd/${OBJDIR}:${OBJROOT}/ext/${ext}/capi/${OBJDIR}:../ext/${ext}/cmd/:../ext/${ext}/capi/)
VPATH := $(subst ${space},,${VPATH})

INCLUDES     := ${INCLUDES} $(foreach ext, ${EXTENSIONS}, ${ext}Interpreter.H ${ext}ClientCapi.H ${ext}Structs.H)
CFLAGS       := ${CFLAGS} $(foreach ext, ${EXTENSIONS},-I../ext/${ext}/cmd -I../ext/${ext}/capi -DECMD_$(shell echo ${ext} | tr "a-zA-Z" "A-Za-z")_EXTENSION_SUPPORT)

# *****************************************************************************
# The Common Setup stuff
# *****************************************************************************
TARGET     := ecmd
VER_TARGET := ecmdVersion
LINK_OBJS  := ${LINK_OBJS} ${OBJROOT}/capi/${OBJDIR}/ecmdClientCapi.a
LINK_OBJS  := ${LINK_OBJS} $(foreach ext, ${EXTENSIONS}, ${ext}ClientCapi.a ${OBJROOT}/ext/${ext}/cmd/${OBJDIR}/${ext}CmdInterpreter.a)

# *****************************************************************************
# The x86 Linux Setup stuff
# *****************************************************************************
ifeq (${TARGET_OS},x86)
  ifeq (${TARGET_ARCH},x86)
    LDLIBS    := ${LDLIBS} -ldl -L${OBJROOT}/capi/${OBJDIR} -lecmd
  else
    LDLIBS    := ${LDLIBS} -ldl -L${OBJROOT}/capi/${OBJDIR} -lecmd -lz
  endif
endif

# *****************************************************************************
# The ppc Linux Setup stuff
# *****************************************************************************
ifeq (${TARGET_OS},ppc)
  LDLIBS    := ${LDLIBS} -ldl -L${OBJROOT}/capi/${OBJDIR} -lecmd
endif

# *****************************************************************************
# The Aix Setup stuff
# *****************************************************************************
ifeq (${TARGET_OS},aix)
  ifeq (${TARGET_ARCH},aix)
    LDLIBS    := -brtl -w -L${OBJROOT}/capi/${OBJDIR} -lecmd
  else
    LDLIBS    := -brtl -w -L${OBJROOT}/capi/${OBJDIR} -lecmd -lz
  endif
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

#Remove create_links after fair warning has been given to users to update paths
install: create_links
	cp ${OBJPATH}/${TARGET} ${INSTALL_PATH}/${TARGET_ARCH}/bin/
	cp ${OBJPATH}/${VER_TARGET} ${INSTALL_PATH}/${TARGET_ARCH}/bin/

create_links:
  ifeq (${TARGET_ARCH},$(filter ${TARGET_ARCH},x86 aix))
	@echo "Creating temporary symlinks for ecmd and ecmdVersion"
	@ln -sf ../${TARGET_ARCH}/bin/${TARGET} ${INSTALL_PATH}/bin/${TARGET}_${TARGET_ARCH}
	@ln -sf ../${TARGET_ARCH}/bin/${VER_TARGET} ${INSTALL_PATH}/bin/${VER_TARGET}_${TARGET_ARCH}	
  endif

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
	${LD} ${LDFLAGS} $^ -o ${OBJPATH}${TARGET} ${LDLIBS}

${VER_TARGET}: ${VER_SOURCE_OBJS}
ifeq (${TARGET_OS},aix)
	${LD} ${LDFLAGS} -brtl -w $^ -o ${OBJPATH}${VER_TARGET} ${LDLIBS}
else
	${LD} ${LDFLAGS} $^ -o ${OBJPATH}${VER_TARGET} ${LDLIBS}
endif


# *****************************************************************************
# Debug rule for any makefile testing 
# *****************************************************************************
debug:
	@echo ${ECMD_ROOT}
	@echo ${SUBDIR}
