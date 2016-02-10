# The main makefile for all rules

# *****************************************************************************
# Define base info and include any global variables
# *****************************************************************************
SUBDIR     := " "
include makefile.vars

ifndef ECMD_ROOT
$(error makefile.vars not found.  Please run mkscripts/buildconfig.py)
endif


# *****************************************************************************
# Some basic setup before we start trying to build stuff
# *****************************************************************************

# Install
# There are two ways to specify the install path.  Both are givin
# INSTALL_PATH is the resolved path to where the files should be installed - i.e. /usr/local/ecmd
# INSTALL_PATH_REF is directly the value you give on the cmdline 
# INSTALL_PATH=\$\$\\CTEPATH make install

INSTALL_PATH_REF := ${INSTALL_PATH}
INSTALL_PATH := $(shell echo ${INSTALL_PATH})

# For an extension specified, we build a complete set of rules for it
EXT_CAPI_RULES    := $(foreach ext, ${EXT_CAPI}, ${ext}capi)
EXT_CMD_RULES     := $(foreach ext, ${EXT_CMD}, ${ext}cmd)
EXT_PERLAPI_RULES := $(foreach ext, ${EXT_PERLAPI}, ${ext}perlapi)
EXT_PYAPI_RULES   := $(foreach ext, ${EXT_PYAPI}, ${ext}pyapi)

# Build our targets
EXT_CAPI_TARGETS    := $(foreach ext, ${EXT_CAPI}, ${ext}capi)
EXT_CMD_TARGETS     := $(foreach ext, ${EXT_CMD}, ${ext}cmd)
EXT_PERLAPI_TARGETS := $(foreach ext, ${EXT_PERLAPI}, ${ext}perlapi)
EXT_PYAPI_TARGETS   := $(foreach ext, ${EXT_PYAPI}, ${ext}pyapi)

# The cmd extension has to be built after ecmdcmd, so if it's in the extension list pull it out and add after ecmdcmd
ifneq (,$(findstring cmdcapi,${EXT_CAPI_TARGETS}))
  CMD_EXT_BUILD := cmdcapi
endif
# Pull cmd then use the list to build ext targets
EXT_CAPI_TARGETS := $(subst cmdcapi,,${EXT_CAPI_TARGETS})

# These variables can be controlled from the distro or makefile.config to determine if python/perl should be built
# The default here is yes
CREATE_PERLAPI ?= yes
CREATE_PYAPI ?= yes
CREATE_PY3API ?= yes

# Set the actual build targets used in the build if yes
ifeq (${CREATE_PERLAPI},yes)
  PERLAPI_BUILD := ecmdperlapi
endif
ifeq (${CREATE_PYAPI},yes)
  PYAPI_BUILD := ecmdpyapi
endif
ifeq (${CREATE_PY3API},yes)
  PY3API_BUILD := ecmdpy3api
endif

# Now create our build targets
BUILD_TARGETS := ecmdcapi ecmdcmd ${CMD_EXT_BUILD} dllstub ${PERLAPI_BUILD} ${PYAPI_BUILD} ${PY3API_BUILD}

# *****************************************************************************
# The Main Targets
# *****************************************************************************

# The default rule is going to do a number of things:
# 1) Create the output directories
# 2) Generate source
# 3) Build source
# 4) Test the build

# The default
all:
	${run-all}

generate: ${BUILD_TARGETS}

build: ${BUILD_TARGETS}

test: ${BUILD_TARGETS}

# The core eCMD pieces
ecmdcapi:
	@echo "eCMD Core Client C-API ${TARGET_ARCH} ..."
	@${MAKE} -C ${ECMD_CORE}/capi ${MAKECMDGOALS} ${MAKEFLAGS}
	@echo " "

ecmdcmd: ecmdcapi ${EXT_CAPI_TARGETS} ${EXT_CMD_TARGETS}
	@echo "eCMD Core Command line Client ${TARGET_ARCH} ..."
	@${MAKE} -C ${ECMD_CORE}/cmd ${MAKECMDGOALS} ${MAKEFLAGS}
	@echo " "

ecmdperlapi: ecmdcmd ${EXT_PERLAPI_TARGETS}
	@echo "eCMD Perl Module ${TARGET_ARCH} ..."
	@${MAKE} -C ${ECMD_CORE}/perlapi ${MAKECMDGOALS} ${MAKEFLAGS}
	@echo " "

ecmdpyapi: ecmdcmd ${EXT_PYAPI_TARGETS}
	@echo "eCMD Python Module ${TARGET_ARCH} ..."
	@${MAKE} -C ${ECMD_CORE}/pyapi ${MAKECMDGOALS} ${MAKEFLAGS}
	@echo " "

ecmdpy3api: ecmdcmd ${EXT_PYAPI_TARGETS}
	@echo "eCMD Python3 Module ${TARGET_ARCH} ..."
	@${MAKE} -C ${ECMD_CORE}/py3api ${MAKECMDGOALS} ${MAKEFLAGS}
	@echo " "

########################
# Build EXTENSIONS
########################
${EXT_CAPI_RULES}: ecmdcapi
	@echo "$(subst capi,,$@) Extension C API ${TARGET_ARCH} ..."
	@${MAKE} -C ${EXT_$(subst capi,,$@)_PATH}/capi ${MAKECMDGOALS} ${MAKEFLAGS}
	@echo " "

${EXT_CMD_RULES}: ecmdcapi
	@echo "$(subst cmd,,$@) Extension cmdline ${TARGET_ARCH} ..."
	@${MAKE} -C ${EXT_$(subst cmd,,$@)_PATH}/cmd ${MAKECMDGOALS} ${MAKEFLAGS}
	@echo " "

${EXT_PERLAPI_RULES}:
	@echo "$(subst perlapi,,$@) Extension Perl API ${TARGET_ARCH} ..."
	@${MAKE} -C ${EXT_$(subst perlapi,,$@)_PATH}/perlapi ${MAKECMDGOALS} ${MAKEFLAGS}
	@echo " "

${EXT_PYAPI_RULES}:
	@echo "$(subst pyapi,,$@) Extension Python API ${TARGET_ARCH} ..."
	@${MAKE} -C ${EXT_$(subst pyapi,,$@)_PATH}/pyapi ${MAKECMDGOALS} ${MAKEFLAGS}
	@echo " "

########################
# Utils
########################
ecmdutils:
	@echo "eCMD Utilities ${TARGET_ARCH} ..."
	@${MAKE} -C ecmd-core/utils ${MAKECMDGOALS} ${MAKEFLAGS}
	@echo " "

########################
# DLL Stub
########################
dllstub:
	@echo "eCMD DLL Stub ${TARGET_ARCH} ..."
	@${MAKE} -C ${ECMD_ROOT}/dllStub ${MAKECMDGOALS} ${MAKEFLAGS}
	@echo " "

# Runs the install routines for all targets
install: install_setup ${BUILD_TARGETS} install_finish

# Copy over the help files, etc.. before installing the executables and libraries
install_setup:

        # Create the install path
	@mkdir -p ${INSTALL_PATH}

	@echo "Creating bin dir ..."
	@mkdir -p ${INSTALL_PATH}/bin
	@echo " "

	@echo "Creating ${TARGET_ARCH}/bin dir ..."
	@mkdir -p ${INSTALL_PATH}/${TARGET_ARCH}/bin
	@echo " "

	@echo "Creating ${TARGET_ARCH}/lib dir ..."
	@mkdir -p ${INSTALL_PATH}/${TARGET_ARCH}/lib
	@echo " "

	@echo "Creating ${TARGET_ARCH}/perl dir ..."
	@mkdir -p ${INSTALL_PATH}/${TARGET_ARCH}/perl
	@echo " "

	@echo "Copying cmdline wrappers to bin"
	@cp -R `find ${ECMD_CORE}/bin/*` ${INSTALL_PATH}/bin/.
	@$(foreach ext, ${EXT_CMD}, echo "  copying ${ext}"; cp -an `find ${EXT_${ext}_PATH}/bin/*` ${INSTALL_PATH}/bin/.;) 
	@echo " "

	@echo "Setting up ecmdaliases files ..."
	@sed "s@\$$PWD@${INSTALL_PATH_REF}/bin@g" ${ECMD_CORE}/bin/ecmdaliases.ksh > ${INSTALL_PATH}/bin/ecmdaliases.ksh
	@sed "s@\$$PWD@${INSTALL_PATH_REF}/bin@g" ${ECMD_CORE}/bin/ecmdaliases.csh > ${INSTALL_PATH}/bin/ecmdaliases.csh
	@echo " "

	@echo "Creating help dir ..."
	@mkdir -p ${INSTALL_PATH}/help
	@cp -R `find ${ECMD_CORE}/cmd/help/*` ${INSTALL_PATH}/help/.
	@$(foreach ext, ${EXTENSIONS}, if [ -d ${EXT_${ext}_PATH}/cmd/help ]; then find ${EXT_${ext}_PATH}/cmd/help -type f -exec cp {} ${INSTALL_PATH}/help/. \; ; fi;)
	@echo " "

	@echo "Copying over setup perl modules ..."
	@mkdir -p ${INSTALL_PATH}/bin/plugins
	@$(foreach plugin, ${ECMD_PLUGINS}, cp ${PLUGIN_${plugin}_PATH}/*setup.pm ${INSTALL_PATH}/bin/plugins/.;)
	@echo " "

# Do final cleanup things such as fixing permissions
install_finish: install_setup ${BUILD_TARGETS}

# Build the utils and install them
	@echo "Building utils ..."
	@$(foreach util, ${ECMD_REPOS_UTILS}, ${MAKE} -C ${util}/utils/ ${MAKEFLAGS};)
	@echo " "

	@echo "Installing utils ..."
	@$(foreach util, ${ECMD_REPOS_UTILS}, ${MAKE} -C ${util}/utils/ ${MAKECMDGOALS} ${MAKEFLAGS};)
	@echo " "

	@echo "Fixing bin dir file permissions ..."
	@chmod 775 ${INSTALL_PATH}/bin/*

	@echo "Fixing ${TARGET_ARCH}/bin dir file permissions ..."
	@chmod 775 ${INSTALL_PATH}/${TARGET_ARCH}/bin/*

	@echo ""
	@echo "*** Install Done! ***"
	@echo ""

# *****************************************************************************
# Include any global default rules
# *****************************************************************************
include ${ECMD_ROOT}/makefile.rules
