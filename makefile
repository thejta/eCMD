# $Header$
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

# Only do the pyapi if it's checked out
ifneq ($(findstring pyapi,$(shell /bin/ls -d *)),)
  PYAPI_BUILD := ecmdpyapi
endif

# Only do the py3api if it's checked out
ifneq ($(findstring py3api,$(shell /bin/ls -d *)),)
  PY3API_BUILD := ecmdpy3api
endif

# Only do the pyapi and py3api if not x86 and not aix
ifeq (${TARGET_ARCH},$(filter ${TARGET_ARCH},x86 aix aix64))
  PYAPI_BUILD := 
  PY3API_BUILD := 
else
  PYAPI_BUILD := ${PYAPI_BUILD}
  PY3API_BUILD := ${PY3API_BUILD}
endif

# Now create our build targets
BUILD_TARGETS := ecmdcapi ${BUILD_TARGETS} ecmdcmd ${CMD_EXT_BUILD} dllstub ${PERLAPI_BUILD} ${PYAPI_BUILD} ${PY3API_BUILD}

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

ecmdcmd: ecmdcapi $(subst cmd,,${EXTENSIONS})
	@echo "eCMD Core Command line Client ${TARGET_ARCH} ..."
	@cd ecmd && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "


ecmdperlapi: ecmdcmd ${CMD_EXT_BUILD}
	@echo "eCMD Perl Module ${TARGET_ARCH} ..."
	@cd perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

ecmdpyapi: ecmdcmd ${CMD_EXT_BUILD}
	@echo "eCMD Python Module ${TARGET_ARCH} ..."
	@cd pyapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

ecmdpy3api: ecmdcmd ${CMD_EXT_BUILD}
	@echo "eCMD Python3 Module ${TARGET_ARCH} ..."
	@cd py3api && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

# All of the individual extensions
########################
# CIP Extension
########################
cip: cipcapi cipperlapi cippyapi 

cipcapi: ecmdcapi
	@echo "Cronus/IP Extension C API ${TARGET_ARCH} ..."
	@cd ext/cip/capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/cip/cmd && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

cipperlapi:
	@echo "Cronus/IP Extension Perl API ${TARGET_ARCH} ..."
	@cd ext/cip/perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

cippyapi:
	@echo "Cronus/IP Extension Python API ${TARGET_ARCH} ..."
	@cd ext/cip/pyapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

########################
# CRO Extension
########################
cro: crocapi croperlapi cropyapi

crocapi: ecmdcapi
	@echo "Cronus Extension C API ${TARGET_ARCH} ..."
	@cd ext/cro/capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/cro/cmd && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

croperlapi:
	@echo "Cronus Extension Perl API ${TARGET_ARCH} ..."
	@cd ext/cro/perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

cropyapi:
	@echo "Cronus Extension Python API ${TARGET_ARCH} ..."
	@cd ext/cro/pyapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

########################
# Scand Extension
########################
scand: scandcapi scandperlapi scandpyapi

scandcapi: ecmdcapi
	@echo "Scand Extension C API ${TARGET_ARCH} ..."
	@cd ext/scand/capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

scandperlapi:
	@echo "Scand Extension Perl API ${TARGET_ARCH} ..."
	@cd ext/scand/perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

scandpyapi:
	@echo "Scand Extension Python API ${TARGET_ARCH} ..."
	@cd ext/scand/pyapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

########################
# EIP Extension
########################
eip: eipcapi eipperlapi eippyapi

eipcapi: ecmdcapi
	@echo "Eclipz IP Extension C API ${TARGET_ARCH} ..."
	@cd ext/eip/capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/eip/cmd && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

eipperlapi:
	@echo "Eclipz IP Extension Perl API ${TARGET_ARCH} ..."
	@cd ext/eip/perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

eippyapi:
	@echo "Eclipz IP Extension Python API ${TARGET_ARCH} ..."
	@cd ext/eip/pyapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

########################
# AIP Extension
########################
aip: aipcapi aipperlapi aippyapi

aipcapi: ecmdcapi
	@echo "Apollo IP Extension C API ${TARGET_ARCH} ..."
	@cd ext/aip/capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/aip/cmd && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

aipperlapi:
	@echo "Apollo IP Extension Perl API ${TARGET_ARCH} ..."
	@cd ext/aip/perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

aippyapi:
	@echo "Apollo IP Extension Python API ${TARGET_ARCH} ..."
	@cd ext/aip/pyapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

########################
# GIP Extension
########################
gip: gipcapi gipperlapi gippyapi

gipcapi: ecmdcapi
	@echo "GFW IP Extension C API ${TARGET_ARCH} ..."
	@cd ext/gip/capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/gip/cmd && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

gipperlapi:
	@echo "GFW IP Extension Perl API ${TARGET_ARCH} ..."
	@cd ext/gip/perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

gippyapi:
	@echo "GFW IP Extension Python API ${TARGET_ARCH} ..."
	@cd ext/gip/pyapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

########################
# ZSE Extension
########################
zse: zsecapi zseperlapi zsepyapi

zsecapi: ecmdcapi
	@echo "Z Series Extension C API ${TARGET_ARCH} ..."
	@cd ext/zse/capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/zse/cmd && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

zseperlapi:
	@echo "Z Series Extension Perl API ${TARGET_ARCH} ..."
	@cd ext/zse/perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

zsepyapi:
	@echo "Z Series Extension Python API ${TARGET_ARCH} ..."
	@cd ext/zse/pyapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

########################
# MBO Extension
########################
mbo: mbocapi mboperlapi mbopyapi

mbocapi: ecmdcapi
	@echo "Mambo Extension C API ${TARGET_ARCH} ..."
	@cd ext/mbo/capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/mbo/cmd && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

mboperlapi:
	@echo "Mambo Extension Perl API ${TARGET_ARCH} ..."
	@cd ext/mbo/perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

mbopyapi:
	@echo "Mambo Extension Python API ${TARGET_ARCH} ..."
	@cd ext/mbo/pyapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

########################
# BML Extension
########################
bml: bmlcapi bmlperlapi bmlpyapi

bmlcapi: ecmdcapi
	@echo "BML Extension C API ${TARGET_ARCH} ..."
	@cd ext/bml/capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/bml/cmd && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

bmlperlapi:
	@echo "BML Extension Perl API ${TARGET_ARCH} ..."
	@cd ext/bml/perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

bmlpyapi:
	@echo "BML Extension Python API ${TARGET_ARCH} ..."
	@cd ext/bml/pyapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

########################
# fapi Extension
########################
fapi: fapicapi fapiperlapi fapipyapi

fapicapi: ecmdcapi
	@echo "FAPI Extension C API ${TARGET_ARCH} ..."
	@cd ext/fapi/capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/fapi/cmd && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

fapiperlapi:
	@echo "FAPI Extension Perl API ${TARGET_ARCH} ..."
	@cd ext/fapi/perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

fapipyapi:
	@echo "FAPI Extension Python API ${TARGET_ARCH} ..."
	@cd ext/fapi/pyapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

########################
# CMD Extension
########################
cmd: cmdcapi cmdperlapi cmdpyapi

cmdcapi: ecmdcmd
	@echo "Command line Extension C API ${TARGET_ARCH} ..."
	@cd ext/cmd/capi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

cmdperlapi:
	@echo "Command line Extension Perl API ${TARGET_ARCH} ..."
	@cd ext/cmd/perlapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

cmdpyapi:
	@echo "Command line Extension Python API ${TARGET_ARCH} ..."
	@cd ext/cmd/pyapi && ${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
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
	@cp -R `find bin/* | grep -v CVS` ${INSTALL_PATH}/bin/.
	@$(foreach ext, ${EXTENSIONS}, if [ -d ext/${ext}/bin/ ]; then find ext/${ext}/bin -type f ! -path "*/CVS/*" -exec cp {} ${INSTALL_PATH}/bin/. \; ; fi;)
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
	@cp -R `find ecmd/help/* | grep -v CVS` ${INSTALL_PATH}/help/.
	@$(foreach ext, ${EXTENSIONS}, if [ -d ext/${ext}/cmd/help ]; then find ext/${ext}/cmd/help -type f ! -path "*/CVS/*" -exec cp {} ${INSTALL_PATH}/help/. \; ; fi;)
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
