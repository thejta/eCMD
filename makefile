# Let's see if we can use distcc
ifneq (${DISTCC_HOSTS},)
  GMAKEFLAGS    := -j8
endif


all:
	@echo " "
	@echo "Core Client API ..."
	@cd capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

	@echo " "
	@echo "Cronus/IP Extension API ..."
	@cd ext/cip/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/cip/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "


	@echo " "
	@echo "Cronus Extension API ..."
	@cd ext/cro/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/cro/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "


	@echo " "
	@echo "Scand Extension API ..."
	@cd ext/scand/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "


	@echo " "
	@echo "Eclipz IP Extension API ..."
	@cd ext/eip/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/eip/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

	@echo " "
	@echo "GFW IP Extension API ..."
	@cd ext/gip/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/gip/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

	@echo " "
	@echo "Z Series Extension API ..."
	@cd ext/zse/capi;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@cd ext/zse/cmd;${MAKE} ${MAKECMDGOALS} ${GMAKEFLAGS}
	@echo " "

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

	@echo " "

	@echo "Perl Module ..."
	@cd perlapi;${MAKE} ${MAKECMDGOALS}
	@echo " "

clean: all

objclean: all

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
