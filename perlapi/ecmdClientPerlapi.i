%module MODULE_NAME
%include typemaps.i
%include cpointer.i

%typemap(in) char ** o_data(char* cvalue) {
  SV* tempsv;
  if (!SvROK($input)) {
    croak("expected a reference\n");
  }
  tempsv = SvRV($input);
  if (!SvPOK(tempsv)) {
    croak("expected a string reference\n");
  }
  cvalue = SvPV(tempsv,PL_na);
  $1 = (char**)malloc(sizeof(char*));

  *$1 = cvalue;
}

%typemap(argout) char **  o_data {
  SV *tempsv;
  if (*$1 != NULL) {
    tempsv = SvRV($arg);
    sv_setpv(tempsv, *$1);
    free(*$1);
  }
  *$1 = NULL;
  free($1);
}

%exception {
	$function
	if (ecmdPerlInterfaceErrorCheck(-1)) {
		croak("Error occured in eCMD Perl module - execution halted\n");
	}
}

%{
#include "ecmdClientPerlapi.H"
%}

%include ecmdClientPerlapi.H
