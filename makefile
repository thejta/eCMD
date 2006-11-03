# The default build rules

ECMD_ROOT     := ${PWD}/
include makefile.rules

# *****************************************************************************
# Some basic setup before we start trying to build stuff
# *****************************************************************************

# Let's see if we can use distcc
ifneq (${DISTCC_HOSTS},)
  GMAKEFLAGS    := -j8
endif

# Setup the install path if the user didn't specify one
ifeq ($(strip $(INSTALL_PATH)),)
  INSTALL_PATH := $(shell pwd)
  INSTALL_PATH := ${INSTALL_PATH}/install
  export INSTALL_PATH
endif


# Yes, this looks horrible but it sets up everything properly
# so that the next sed in the install_setup rule produces the right output
# If an install is being done to a CTE path, replace it with $CTEPATH so it'll work everywhere
CTE_INSTALL_PATH := $(shell echo ${INSTALL_PATH} | sed "s@/.*cte/@\"\\\\$$\"CTEPATH/@")

# Last thing to be done, setup all of our build targets for use below
BUILD_TARGETS := ${EXTENSIONS}
# The cmd extension has to be built after ecmdcmd, so if it's in the extension list pull it out and add after ecmdcmd
ifneq (,$(findstring cmd,${BUILD_TARGETS}))
  CMD_EXT_BUILD := cmd
endif
BUILD_TARGETS := $(subst cmd,,${BUILD_TARGETS})

# Only do the perlapi if it's checked out
ifneq ($(findstring perlapi,$(shell /bin/ls -d *)),)
  PERLAPI_BUILD := ecmdperlapi
endif

# Now create our build targets
BUILD_TARGETS := ecmdcapi ${BUILD_TARGETS} ecmdcmd ${CMD_EXT_BUILD} ${PERLAPI_BUILD}

# *****************************************************************************
# The Main Targets
# *****************************************************************************

# The default
all: ${BUILD_TARGETS}

# The core eCMD pieces
ecmdcapi:
	@echo "eCMD Core Client API ..."
	@cd capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

ecmdcmd: ecmdcapi $(subst cmd,,${EXTENSIONS})
	@echo "eCMD Core Command line Client ..."
	@cd ecmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "


ecmdperlapi: ecmdcmd ${CMD_EXT_BUILD}
	@echo "eCMD Perl Module ..."
	@cd perlapi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

# All of the individual extensions
cip: ecmdcapi
	@echo "Cronus/IP Extension API ..."
	@cd ext/cip/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/cip/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

cro: ecmdcapi
	@echo "Cronus Extension API ..."
	@cd ext/cro/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/cro/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

scand: ecmdcapi
	@echo "Scand Extension API ..."
	@cd ext/scand/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

eip: ecmdcapi
	@echo "Eclipz IP Extension API ..."
	@cd ext/eip/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/eip/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

gip: ecmdcapi
	@echo "GFW IP Extension API ..."
	@cd ext/gip/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/gip/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

zse: ecmdcapi
	@echo "Z Series Extension API ..."
	@cd ext/zse/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/zse/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

mbo: ecmdcapi
	@echo "Mambo Extension API ..."
	@cd ext/mbo/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/mbo/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

bml: ecmdcapi
	@echo "BML Extension API ..."
	@cd ext/bml/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/bml/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

cmd: ecmdcmd
	@echo "Command line Extension API ..."
	@cd ext/cmd/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

# Runs the objclean and export clean targets in addition to removing generated source
clean: ${BUILD_TARGETS}

# Remove the obj_* dir for the system type you are building on
objclean: ${BUILD_TARGETS}

# Remove the export dir if it exists
exportclean: ${BUILD_TARGETS}

# Runs the install routines for all targets
install: install_setup ${BUILD_TARGETS}

# Copy over the help files, etc.. before installing the executables and libraries
install_setup:
	@echo "Creating bin dir ..."
	@mkdir -p ${INSTALL_PATH}/bin
	cp -R `find bin/* | grep -v CVS` ${INSTALL_PATH}/bin/.
	cp -Rf `find ext/*/bin/* | grep -v CVS |grep -v ecmdWrapper` ${INSTALL_PATH}/bin/.
	@echo " "

	@echo "Setting up ecmdaliases files ..."
	@sed "s@\$$PWD@${CTE_INSTALL_PATH}/bin@g" bin/ecmdaliases.ksh > ${INSTALL_PATH}/bin/ecmdaliases.ksh
	@sed "s@\$$PWD@${CTE_INSTALL_PATH}/bin@g" bin/ecmdaliases.csh > ${INSTALL_PATH}/bin/ecmdaliases.csh
	@echo " "

	@echo "Creating help dir ..."
	@mkdir -p ${INSTALL_PATH}/help
	cp -R `find ecmd/help/* | grep -v CVS` ${INSTALL_PATH}/help/.
	cp -R `find ext/*/cmd/help/* | grep -v CVS` ${INSTALL_PATH}/help/.
	@echo " "

ifneq ($(findstring plugins,$(shell /bin/ls -d *)),)
	@echo "Creating plugins dir ..."
	@mkdir -p ${INSTALL_PATH}/plugins
	cp -R `find plugins/* | grep -v CVS` ${INSTALL_PATH}/plugins/.
	@echo " "
endif

ifneq ($(findstring utils,$(shell /bin/ls -d *)),)
	@echo "Building utils ..."
	@cd utils;${MAKE} ${GMAKEFLAGS}
	@echo " "

	@echo "Installing utils ..."
	@cd utils;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "
endif

# Just print some vars
vars:
	@echo ${ECMD_ROOT}
	@echo ${BUILD_TARGETS}
	@echo ${EXTENSIONS}
