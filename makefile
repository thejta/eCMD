# The default build rules
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
  # Do this so it's picked up by submakes
  export INSTALL_PATH
endif

# Include the makefile.confg if the config script was run, this will override anything above
-include makefile.config

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

# Now create our build order
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

ecmdcmd:
	@echo "eCMD Core Command line Client ..."
	@cd ecmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "


ecmdperlapi:
	@echo "eCMD Perl Module ..."
	@cd perlapi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

# All of the individual extensions
cip:
	@echo "Cronus/IP Extension API ..."
	@cd ext/cip/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/cip/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

cro:
	@echo "Cronus Extension API ..."
	@cd ext/cro/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/cro/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

scand:
	@echo "Scand Extension API ..."
	@cd ext/scand/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

eip:
	@echo "Eclipz IP Extension API ..."
	@cd ext/eip/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/eip/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

gip:
	@echo "GFW IP Extension API ..."
	@cd ext/gip/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/gip/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

zse:
	@echo "Z Series Extension API ..."
	@cd ext/zse/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/zse/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

mbo:
	@echo "Mambo Extension API ..."
	@cd ext/mbo/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/mbo/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

bml:
	@echo "BML Extension API ..."
	@cd ext/bml/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/bml/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

cmd:
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
	cp -Rf `find ext/*/bin/* | grep -v CVS` ${INSTALL_PATH}/bin/.
	@echo " "

	@echo "Setting up install scripts ..."
	@sed "s@\$$PWD@${CTE_INSTALL_PATH}/bin@g" bin/ecmdaliases.ksh > ${INSTALL_PATH}/bin/ecmdaliases.ksh
	@sed "s@\$$PWD@${CTE_INSTALL_PATH}/bin@g" bin/ecmdaliases.csh > ${INSTALL_PATH}/bin/ecmdaliases.csh
	@echo " "

	@echo "Creating help dir ..."
	@mkdir -p ${INSTALL_PATH}/help
	cp -R `find ecmd/help/* | grep -v CVS` ${INSTALL_PATH}/help/.
	cp -R `find ext/*/cmd/help/* | grep -v CVS` ${INSTALL_PATH}/help/.
	@echo " "

	@echo "Creating plugins dir ..."
	@mkdir -p ${INSTALL_PATH}/plugins
	cp -R `find plugins/* | grep -v CVS` ${INSTALL_PATH}/plugins/.
	@echo " "

	@echo "Building utils ..."
	@cd utils;${MAKE} ${GMAKEFLAGS}
	@echo " "

	@echo "Installing utils ..."
	@cd utils;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

# Just print some vars
vars:
	@echo ${BUILD_TARGETS}

