# Let's see if we can use distcc
ifneq (${DISTCC_HOSTS},)
  GMAKEFLAGS    := -j8
endif

# Setup the install path if the user didn't specify one
ifeq ($(strip $(INSTALL_PATH)),)
  INSTALL_PATH := $(shell pwd)
  INSTALL_PATH := ${INSTALL_PATH}/install
  # Tack this onto the GMAKEFLAGS so that sub makes get them
  GMAKEFLAGS   := ${GMAKEFLAGS} INSTALL_PATH=${INSTALL_PATH} 
endif
# Yes, this looks horrible but it sets up everything properly
# so that the next sed produces the right output
CTE_INSTALL_PATH := $(shell echo ${INSTALL_PATH} | sed "s@/.*cte/@\"\\\\$$\"CTEPATH/@")

all:
	@echo "Core Client API ..."
	@cd capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

	@echo "Cronus/IP Extension API ..."
	@cd ext/cip/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/cip/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "


	@echo "Cronus Extension API ..."
	@cd ext/cro/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/cro/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "


	@echo "Scand Extension API ..."
	@cd ext/scand/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "


	@echo "Eclipz IP Extension API ..."
	@cd ext/eip/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/eip/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

	@echo "GFW IP Extension API ..."
	@cd ext/gip/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/gip/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

	@echo "Z Series Extension API ..."
	@cd ext/zse/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/zse/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

	@echo "Mambo Extension API ..."
	@cd ext/mbo/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/mbo/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

	@echo "Core Command line Client ..."
	@cd ecmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

	@echo "Command line Extension API ..."
	@cd ext/cmd/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

	@echo "Perl Module ..."
	@cd perlapi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

clean: all

objclean: all

exportclean: all

install: install_setup all

install_setup:
	@echo "Creating bin dir ..."
	@mkdir -p ${INSTALL_PATH}/bin
	cp -R `find bin/* | grep -v CVS` ${INSTALL_PATH}/bin/.
	cp -R `find ext/*/bin/* | grep -v CVS` ${INSTALL_PATH}/bin/.
	@echo " "

	@echo "Setting up install scripts ..."
	@echo ${CTE_INSTALL_PATH}
	# Using @ as the seperator so that the / in the file names don't mess things up
	sed "s@\$$PWD@${CTE_INSTALL_PATH}/bin@g" bin/ecmdaliases.ksh > ${INSTALL_PATH}/bin/ecmdaliases.ksh
	sed "s@\$$PWD@${CTE_INSTALL_PATH}/bin@g" bin/ecmdaliases.csh > ${INSTALL_PATH}/bin/ecmdaliases.csh
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

avail:
	@echo " "
	@echo "Core Client API ..."
	@cd capi;${MAKE} all

	@echo " "
	@echo "Extension API's ..."
	@$(foreach dir, $(shell /bin/ls -d ext/*/capi | grep -v template | grep -v cmd), (cd ${dir}; ${MAKE} all; echo " ");)

	@echo "Extension Command Interpreters ..."
	@$(foreach dir, $(shell /bin/ls -d ext/*/cmd | grep -v template), (cd ${dir}; ${MAKE} all; echo " ");)

	@echo " "
	@echo "Core Command line Client ..."
	@$(foreach dir, $(shell /bin/ls -d ecmd), cd ${dir}; ${MAKE} all)


	@echo " "
	@echo "Command line Extension API ..."
	@$(foreach dir, $(shell /bin/ls -d ext/cmd/capi), cd ${dir}; ${MAKE} all)

	@echo " "
	@echo "Perl Module ..."
	@$(foreach dir, $(shell /bin/ls -d perlapi), cd ${dir}; ${MAKE} all)
