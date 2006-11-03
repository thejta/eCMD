# Let's see if we can use distcc
ifneq (${DISTCC_HOSTS},)
  GMAKEFLAGS    := -j8
endif

# Setup the install path if the user didn't specify one
ifeq ($(strip $(INSTALL_PATH)),)
  INSTALL_PATH := $(shell pwd)
  INSTALL_PATH := ${INSTALL_PATH}/install
  export INSTALL_PATH
  # Tack this onto the GMAKEFLAGS so that sub makes get them
  # GMAKEFLAGS   := ${GMAKEFLAGS} INSTALL_PATH=${INSTALL_PATH} 
endif

# Yes, this looks horrible but it sets up everything properly
# so that the next sed in the install_setup rule produces the right output
# If an install is being done to a CTE path, replace it with $CTEPATH so it'll work everywhere
CTE_INSTALL_PATH := $(shell echo ${INSTALL_PATH} | sed "s@/.*cte/@\"\\\\$$\"CTEPATH/@")

all:
	@echo "Core Client API ..."
	@cd capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

ifneq ($(findstring cip,${EXTENSIONS}),)
	@echo "Cronus/IP Extension API ..."
	@cd ext/cip/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/cip/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "
endif

ifneq ($(findstring cro,${EXTENSIONS}),)
	@echo "Cronus Extension API ..."
	@cd ext/cro/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/cro/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "
endif

ifneq ($(findstring scand,${EXTENSIONS}),)
	@echo "Scand Extension API ..."
	@cd ext/scand/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "
endif

ifneq ($(findstring eip,${EXTENSIONS}),)
	@echo "Eclipz IP Extension API ..."
	@cd ext/eip/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/eip/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "
endif

ifneq ($(findstring gip,${EXTENSIONS}),)
	@echo "GFW IP Extension API ..."
	@cd ext/gip/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/gip/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "
endif

ifneq ($(findstring zse,${EXTENSIONS}),)
	@echo "Z Series Extension API ..."
	@cd ext/zse/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/zse/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "
endif

ifneq ($(findstring mbo,${EXTENSIONS}),)
	@echo "Mambo Extension API ..."
	@cd ext/mbo/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/mbo/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "
endif

ifneq ($(findstring bml,${EXTENSIONS}),)
	@echo "BML Extension API ..."
	@cd ext/bml/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/bml/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "
endif

	@echo "Core Command line Client ..."
	@cd ecmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

ifneq ($(findstring cmd,${EXTENSIONS}),)
	@echo "Command line Extension API ..."
	@cd ext/cmd/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "
endif

ifneq ($(findstring perlapi,$(shell /bin/ls -d *)),)
	@echo "Perl Module ..."
	@cd perlapi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "
endif

clean: all

objclean: all

exportclean: all

install: install_setup all

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

