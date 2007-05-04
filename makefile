# The default build rules

ECMD_ROOT     := ${PWD}/
include makefile.rules

# *****************************************************************************
# Some basic setup before we start trying to build stuff
# *****************************************************************************

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
	@echo "eCMD Core Client C-API ..."
	@cd capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

ecmdcmd: ecmdcapi $(subst cmd,,${EXTENSIONS})
	@echo "eCMD Core Command line Client ..."
	@cd ecmd && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "


ecmdperlapi: ecmdcmd ${CMD_EXT_BUILD}
	@echo "eCMD Perl Module ..."
	@cd perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

# All of the individual extensions
########################
# CIP Extension
########################
cip: cipcapi cipperlapi

cipcapi: ecmdcapi
	@echo "Cronus/IP Extension C-API ..."
	@cd ext/cip/capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/cip/cmd && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

cipperlapi:
	@echo "Cronus/IP Extension Perl-API ..."
	@cd ext/cip/perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

########################
# CRO Extension
########################
cro: crocapi croperlapi

crocapi: ecmdcapi
	@echo "Cronus Extension C-API ..."
	@cd ext/cro/capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/cro/cmd && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

croperlapi:
	@echo "Cronus Extension Perl-API ..."
	@cd ext/cro/perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

########################
# Scand Extension
########################
scand: scandcapi scandperlapi

scandcapi: ecmdcapi
	@echo "Scand Extension C-API ..."
	@cd ext/scand/capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

scandperlapi:
	@echo "Scand Extension Perl-API ..."
	@cd ext/scand/perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

########################
# EIP Extension
########################
eip: eipcapi eipperlapi

eipcapi: ecmdcapi
	@echo "Eclipz IP Extension C-API ..."
	@cd ext/eip/capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/eip/cmd && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

eipperlapi:
	@echo "Eclipz IP Extension Perl-API ..."
	@cd ext/eip/perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

########################
# GIP Extension
########################
gip: gipcapi gipperlapi

gipcapi: ecmdcapi
	@echo "GFW IP Extension C-API ..."
	@cd ext/gip/capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/gip/cmd && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

gipperlapi:
	@echo "GFW IP Extension Perl-API ..."
	@cd ext/gip/perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

########################
# ZSE Extension
########################
zse: zsecapi zseperlapi

zsecapi: ecmdcapi
	@echo "Z Series Extension C-API ..."
	@cd ext/zse/capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/zse/cmd && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

zseperlapi:
	@echo "Z Series Extension Perl-API ..."
	@cd ext/zse/perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

########################
# MBO Extension
########################
mbo: mbocapi mboperlapi

mbocapi: ecmdcapi
	@echo "Mambo Extension C-API ..."
	@cd ext/mbo/capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/mbo/cmd && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

mboperlapi:
	@echo "Mambo Extension Perl-API ..."
	@cd ext/mbo/perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

########################
# BML Extension
########################
bml: bmlcapi bmlperlapi

bmlcapi: ecmdcapi
	@echo "BML Extension C-API ..."
	@cd ext/bml/capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/bml/cmd && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

bmlperlapi:
	@echo "BML Extension Perl-API ..."
	@cd ext/bml/perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

########################
# CMD Extension
########################
cmd: cmdcapi cmdperlapi

cmdcapi: ecmdcmd
	@echo "Command line Extension C-API ..."
	@cd ext/cmd/capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

cmdperlapi:
	@echo "Command line Extension Perl-API ..."
	@cd ext/cmd/perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

# Runs the objclean and export clean targets in addition to removing generated source
clean: ${BUILD_TARGETS}

# Remove the obj_* dir for the system type you are building on
objclean: ${BUILD_TARGETS}

# Remove the export dir if it exists
exportclean: ${BUILD_TARGETS}

# Runs the install routines for all targets
install: install_setup ${BUILD_TARGETS} install_finish

# Copy over the help files, etc.. before installing the executables and libraries
install_setup:

        # Create the install path
	@mkdir -p ${INSTALL_PATH}

	@echo "Creating bin dir ..."
	@mkdir -p ${INSTALL_PATH}/bin
	@cp -R `find bin/* | grep -v CVS` ${INSTALL_PATH}/bin/.
	@cp -R `find $(foreach ext, ${EXTENSIONS},ext/${ext}/bin/*) | grep -v CVS | grep -v ecmdWrapper` ${INSTALL_PATH}/bin/.
	@echo " "

	@echo "Setting up ecmdaliases files ..."
	@sed "s@\$$PWD@${CTE_INSTALL_PATH}/bin@g" bin/ecmdaliases.ksh > ${INSTALL_PATH}/bin/ecmdaliases.ksh
	@sed "s@\$$PWD@${CTE_INSTALL_PATH}/bin@g" bin/ecmdaliases.csh > ${INSTALL_PATH}/bin/ecmdaliases.csh
	@echo " "

	@echo "Copying over setup scripts ..."
	@sed "s@\$$PWD@${CTE_INSTALL_PATH}/bin@g" bin/ecmdaliases.ksh > ${INSTALL_PATH}/bin/ecmdaliases.ksh
	@sed "s@\$$PWD@${CTE_INSTALL_PATH}/bin@g" bin/ecmdaliases.csh > ${INSTALL_PATH}/bin/ecmdaliases.csh
	@echo " "

	@echo "Creating help dir ..."
	@mkdir -p ${INSTALL_PATH}/help
	@cp -R `find ecmd/help/* | grep -v CVS` ${INSTALL_PATH}/help/.
	@cp -R `find $(foreach ext, ${EXTENSIONS},ext/${ext}/cmd/help/*) | grep -v CVS`  ${INSTALL_PATH}/help/.
	@echo " "

ifneq ($(findstring plugins,$(shell /bin/ls -d *)),)
	@echo "Creating plugins dir ..."
	@mkdir -p ${INSTALL_PATH}/plugins
	@cp -R plugins/ ${INSTALL_PATH}/.
	@rm -r `find ${INSTALL_PATH}/plugins/* -type d -name CVS`
	@echo " "
endif

	@echo "Copying over setup perl modules ..."
	@find plugins/ -type f -name "*setup.pm" -exec cp {} bin/. \;
	@echo " "

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

	@echo ""
	@echo "*** Install Done! ***"
	@echo ""

# Just print some vars
vars:
	@echo ${ECMD_ROOT}
	@echo ${BUILD_TARGETS}
	@echo ${EXTENSIONS}
	@echo ${GMAKEFLAGS}
