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


%typemap(in) int *o_matchs (int dvalue),int &o_matchs (int dvalue)  {
  SV *tempsv;
  if (!SvROK($input)) {
    SWIG_croak("expected a reference");
  }
  tempsv = SvRV($input);
  if (!SvIOK(tempsv)) {
    SWIG_croak("expected a integer reference");
  }
  dvalue = SvIV(tempsv);
  $1 = &dvalue;
}


%typemap(argout) int *o_matchs, int &o_matchs {
  SV *tempsv;
  tempsv = SvRV($input);
  if (!$1) SWIG_croak("expected a reference");
  sv_setiv(tempsv, (IV) *$1);
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
