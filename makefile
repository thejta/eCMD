# Let's see if we can use distcc
ifneq (${DISTCC_HOSTS},)
  GMAKEFLAGS    := -j8
endif


all:
	@echo " "
	@echo "Building Core Client API ..."
	@cd capi;gmake ${GMAKEFLAGS}
	@echo " "

	@echo " "
	@echo "Building Cronus/IP Extension API ..."
	@cd ext/cip/capi;gmake  ${GMAKEFLAGS}
	@cd ext/cip/cmd;gmake ${GMAKEFLAGS}
	@echo " "


	@echo " "
	@echo "Building Cronus Extension API ..."
	@cd ext/cro/capi;gmake ${GMAKEFLAGS}
	@cd ext/cro/cmd;gmake ${GMAKEFLAGS}
	@echo " "


	@echo " "
	@echo "Building Scand Extension API ..."
	@cd ext/scand/capi;gmake ${GMAKEFLAGS}
	@echo " "


	@echo " "
	@echo "Building Eclipz IP Extension API ..."
	@cd ext/eip/capi;gmake ${GMAKEFLAGS}
	@cd ext/eip/cmd;gmake ${GMAKEFLAGS}
	@echo " "

	@echo " "
	@echo "Building GFW IP Extension API ..."
	@cd ext/gip/capi;gmake ${GMAKEFLAGS}
	@cd ext/gip/cmd;gmake ${GMAKEFLAGS}
	@echo " "

	@echo " "
	@echo "Building Z Series Extension API ..."
	@cd ext/zse/capi;gmake ${GMAKEFLAGS}
	@cd ext/zse/cmd;gmake ${GMAKEFLAGS}
	@echo " "

	@echo " "
	@echo "Building Mambo Extension API ..."
	@cd ext/mbo/capi;gmake ${GMAKEFLAGS}
	@cd ext/mbo/cmd;gmake ${GMAKEFLAGS}
	@echo " "


	@echo "Building Core Command line Client ..."
	@cd ecmd;gmake ${GMAKEFLAGS}
	@echo " "

	@echo "Building Command line Extension API ..."
	@cd ext/cmd/capi;gmake ${GMAKEFLAGS}
	@echo " "

	@echo " "

	@echo "Building Perl Module ..."
	@cd perlapi;gmake
	@echo " "

clean:
	@cd capi;gmake clean

	@cd ext/cip/capi;gmake clean
	@cd ext/cip/cmd;gmake clean

	@cd ext/cro/capi;gmake clean
	@cd ext/cro/cmd;gmake clean

	@cd ext/scand/capi;gmake clean


	@cd ext/eip/capi;gmake clean
	@cd ext/eip/cmd;gmake clean

	@cd ext/gip/capi;gmake clean
	@cd ext/gip/cmd;gmake clean

	@cd ext/zse/capi;gmake clean
	@cd ext/zse/cmd;gmake clean

	@cd ext/mbo/capi;gmake clean
	@cd ext/mbo/cmd;gmake clean

	@cd ecmd;gmake clean

	@cd ext/cmd/capi;gmake clean

	@cd perlapi;gmake clean


objclean:
	@cd capi;gmake objclean

	@cd ext/cip/capi;gmake objclean
	@cd ext/cip/cmd;gmake objclean

	@cd ext/cro/capi;gmake objclean
	@cd ext/cro/cmd;gmake objclean

	@cd ext/scand/capi;gmake objclean


	@cd ext/eip/capi;gmake objclean
	@cd ext/eip/cmd;gmake objclean

	@cd ext/gip/capi;gmake objclean
	@cd ext/gip/cmd;gmake objclean

	@cd ext/zse/capi;gmake objclean
	@cd ext/zse/cmd;gmake objclean

	@cd ext/mbo/capi;gmake objclean
	@cd ext/mbo/cmd;gmake objclean

	@cd ecmd;gmake objclean

	@cd ext/cmd/capi;gmake objclean

	@cd perlapi;gmake objclean
