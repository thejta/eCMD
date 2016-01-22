# The main makefile for all rules

# Base info and default build rules
SUBDIR     := " "
include makefile.rules

# *****************************************************************************
# Some basic setup before we start trying to build stuff
# *****************************************************************************

# Yes, this looks horrible but it sets up everything properly
# so that the next sed in the install_setup rule produces the right output
# If an install is being done to a CTE path, replace it with $CTEPATH so it'll work everywhere
CTE_INSTALL_PATH := $(shell echo ${INSTALL_PATH} | sed "s@/.*cte/@\"\\\\$$\"CTEPATH/@")

# The cmd extension has to be built after ecmdcmd, so if it's in the extension list pull it out and add after ecmdcmd
ifneq (,$(findstring cmd,${EXTENSIONS}))
  CMD_EXT_BUILD := cmdcapi
endif
# Pull cmd then use the list to build the {ext}capi targets
EXT_TARGETS := $(subst cmd,,${EXTENSIONS})
EXT_TARGETS := $(foreach ext, ${EXT_TARGETS}, ${ext}capi)

# All of our extensions build with the same rules, create the list here
EXT_CAPI_RULES := $(foreach ext, ${EXTENSIONS}, ${ext}capi)
EXT_PERLAPI_RULES := $(foreach ext, ${EXTENSIONS}, ${ext}perlapi)
EXT_PYAPI_RULES := $(foreach ext, ${EXTENSIONS}, ${ext}pyapi)

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
BUILD_TARGETS := ecmdcapi ${EXT_TARGETS} ecmdcmd ${CMD_EXT_BUILD} dllstub ${PERLAPI_BUILD} ${PYAPI_BUILD} ${PY3API_BUILD}

# *****************************************************************************
# The Main Targets
# *****************************************************************************

# The default
all: ${BUILD_TARGETS}

# Runs the objclean targets in addition to removing generated source
clean: ${BUILD_TARGETS} ecmdutils

# Remove the obj_* dir for the system type you are building on
objclean: ${BUILD_TARGETS} ecmdutils

# The core eCMD pieces
ecmdcapi:
	@echo "eCMD Core Client C-API ${TARGET_ARCH} ..."
	@cd capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

ecmdcmd: ecmdcapi ${EXT_TARGETS}
	@echo "eCMD Core Command line Client ${TARGET_ARCH} ..."
	@cd cmd && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

ecmdperlapi: ecmdcmd $(foreach ext, ${EXTENSIONS}, ${ext}perlapi)
	@echo "eCMD Perl Module ${TARGET_ARCH} ..."
	@cd perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

ecmdpyapi: ecmdcmd  $(foreach ext, ${EXTENSIONS}, ${ext}pyapi)
	@echo "eCMD Python Module ${TARGET_ARCH} ..."
	@cd pyapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

ecmdpy3api: ecmdcmd  $(foreach ext, ${EXTENSIONS}, ${ext}pyapi)
	@echo "eCMD Python3 Module ${TARGET_ARCH} ..."
	@cd py3api && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

########################
# Build EXTENSIONS
########################
${EXT_CAPI_RULES}: ecmdcapi
	@echo "$(subst capi,,$@) Extension C API ${TARGET_ARCH} ..."
	@cd ext/$(subst capi,,$@)/capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/$(subst capi,,$@)/cmd && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

${EXT_PERLAPI_RULES}:
	@echo "$(subst perlapi,,$@) Extension Perl API ${TARGET_ARCH} ..."
	@cd ext/$(subst perlapi,,$@)/perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

${EXT_PYAPI_RULES}:
	@echo "$(subst pyapi,,$@) Extension Python API ${TARGET_ARCH} ..."
	@cd ext/$(subst pyapi,,$@)/pyapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

########################
# Utils
########################
ecmdutils:
	@echo "eCMD Utilities ${TARGET_ARCH} ..."
	@cd utils && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

########################
# DLL Stub
########################
dllstub:
	@echo "eCMD DLL Stub ${TARGET_ARCH} ..."
	@cd dllStub && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

# Runs the install routines for all targets
install: install_setup ${BUILD_TARGETS} install_finish

# Copy over the help files, etc.. before installing the executables and libraries
install_setup:

        # Create the install path
	@mkdir -p ${INSTALL_PATH}

	@echo "Creating bin dir ..."
	@mkdir -p ${INSTALL_PATH}/bin
	@cp -R `find bin/*` ${INSTALL_PATH}/bin/.
	@$(foreach ext, ${EXTENSIONS}, if [ -d ext/${ext}/bin/ ]; then find ext/${ext}/bin -type f -exec cp {} ${INSTALL_PATH}/bin/. \; ; fi;)
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

	@echo "Setting up ecmdaliases files ..."
	@sed "s@\$$PWD@${CTE_INSTALL_PATH}/bin@g" bin/ecmdaliases.ksh > ${INSTALL_PATH}/bin/ecmdaliases.ksh
	@sed "s@\$$PWD@${CTE_INSTALL_PATH}/bin@g" bin/ecmdaliases.csh > ${INSTALL_PATH}/bin/ecmdaliases.csh
	@echo " "

	@echo "Creating help dir ..."
	@mkdir -p ${INSTALL_PATH}/help
	@cp -R `find cmd/help/*` ${INSTALL_PATH}/help/.
	@$(foreach ext, ${EXTENSIONS}, if [ -d ext/${ext}/cmd/help ]; then find ext/${ext}/cmd/help -type f -exec cp {} ${INSTALL_PATH}/help/. \; ; fi;)
        # Do the IP istep help files
        # Eclipz
	@cp systems/ip/eclipz/help/istep_ipeclipz.htxt ${INSTALL_PATH}/help/.
        # Apollo
	@cp systems/ip/apollo/help/istep_ipapollo.htxt ${INSTALL_PATH}/help/.

	@echo " "

ifneq ($(findstring plugins,$(shell /bin/ls -d *)),)
	@echo "Copying over setup perl modules ..."
	@find plugins/ -type f -name "*setup.pm" -exec cp {} ${INSTALL_PATH}/bin/. \;
	@echo " "
endif

# Do final cleanup things such as fixing permissions
install_finish: install_setup ${BUILD_TARGETS}

# Build the utils and install them
ifneq ($(findstring utils,$(shell /bin/ls -d *)),)
	@echo "Building utils ..."
	@cd utils && ${MAKE} ${GMAKEFLAGS}
	@echo " "

	@echo "Installing utils ..."
	@cd utils && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "
endif

	@echo "Fixing bin dir file permissions ..."
	@chmod 775 ${INSTALL_PATH}/bin/*

	@echo "Fixing ${TARGET_ARCH}/bin dir file permissions ..."
	@chmod 775 ${INSTALL_PATH}/${TARGET_ARCH}/bin/*

	@echo ""
	@echo "*** Install Done! ***"
	@echo ""

# *****************************************************************************
# Debug rule for any makefile testing 
# *****************************************************************************
debug: ${BUILD_TARGETS}
	@echo ${OBJROOT}
	@echo ${ECMD_ROOT}

