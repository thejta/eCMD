all:
	@echo " "
	@echo "Building Core Client API ..."
	@cd capi;gmake
	@echo " "

	@echo " "
	@echo "Building Cronus/IP Extension API ..."
	@cd ext/cip/capi;gmake
	@cd ext/cip/cmd;gmake
	@echo " "


	@echo " "
	@echo "Building Cronus Extension API ..."
	@cd ext/cro/capi;gmake
	@cd ext/cro/cmd;gmake
	@echo " "


	@echo " "
	@echo "Building Scand Extension API ..."
	@cd ext/scand/capi;gmake
	@echo " "


	@echo " "
	@echo "Building Eclipz IP Extension API ..."
	@cd ext/eip/capi;gmake
	@cd ext/eip/cmd;gmake
	@echo " "

	@echo " "
	@echo "Building GFW IP Extension API ..."
	@cd ext/gip/capi;gmake
	@cd ext/gip/cmd;gmake
	@echo " "

	@echo " "
	@echo "Building Z Series Extension API ..."
	@cd ext/zse/capi;gmake
	@cd ext/zse/cmd;gmake
	@echo " "


	@echo "Building Core Command line Client ..."
	@cd ecmd;gmake
	@echo " "

	@echo "Building Command line Extension API ..."
	@cd ext/cmd/capi;gmake
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

	@cd ecmd;gmake objclean

	@cd ext/cmd/capi;gmake objclean

	@cd perlapi;gmake objclean
