%module MODULE_NAME
%include typemaps.i
%include cpointer.i

%typemap(in) char ** o_data (char* cvalue) {
  SV* tempsv;
  if (!SvROK($input)) {
    croak("ecmdClientPerlapi.i::expected a reference\n");
  }
  tempsv = SvRV($input);
  if (!SvPOK(tempsv)) {
    croak("ecmdClientPerlapi.i::expected a string reference\n");
  }
  cvalue = SvPV(tempsv,PL_na);
  $1 = (char**)malloc(sizeof(char*));

  *$1 = cvalue;
}

%typemap(argout) char ** o_data {
  SV *tempsv;
  if (*$1 != NULL) {
    tempsv = SvRV($arg);
    sv_setpv(tempsv, *$1);
    free(*$1);
  }
  *$1 = NULL;
  free($1);
}

%typemap(in) char ** o_enumValue (char* cvalue) {
  SV* tempsv;
  if (!SvROK($input)) {
    croak("ecmdClientPerlapi.i::expected a reference\n");
  }
  tempsv = SvRV($input);
  if (!SvPOK(tempsv)) {
    croak("ecmdClientPerlapi.i::expected a string reference\n");
  }
  cvalue = SvPV(tempsv,PL_na);
  $1 = (char**)malloc(sizeof(char*));

  *$1 = cvalue;
}

%typemap(argout) char ** o_enumValue {
  SV *tempsv;
  if (*$1 != NULL) {
    tempsv = SvRV($arg);
    sv_setpv(tempsv, *$1);
    free(*$1);
  }
  *$1 = NULL;
  free($1);
}

#%typemap(freearg) char ** o_enumValue {
#	free($1);
#}


%typemap(in) int *o_matchs (int dvalue),int &o_matchs (int dvalue)  {
  SV *tempsv;
  if (!SvROK($input)) {
    SWIG_croak("ecmdClientPerlapi.i::expected a reference");
  }
  tempsv = SvRV($input);
  if (!SvIOK(tempsv)) {
    SWIG_croak("ecmdClientPerlapi.i::expected a integer reference");
  }
  dvalue = SvIV(tempsv);
  $1 = &dvalue;
}

%typemap(in) int *o_width (int dvalue),int &o_width (int dvalue)  {
  SV *tempsv;
  if (!SvROK($input)) {
    SWIG_croak("ecmdClientPerlapi.i::expected a reference");
  }
  tempsv = SvRV($input);
  if (!SvIOK(tempsv)) {
    SWIG_croak("ecmdClientPerlapi.i::expected a integer reference");
  }
  dvalue = SvIV(tempsv);
  $1 = &dvalue;
}


%typemap(argout) int *o_matchs, int &o_matchs {
  SV *tempsv;
  tempsv = SvRV($input);
  if (!$1) SWIG_croak("ecmdClientPerlapi.i::expected a reference");
  sv_setiv(tempsv, (IV) *$1);
}

%typemap(argout) int *o_width, int &o_width {
  SV *tempsv;
  tempsv = SvRV($input);
  if (!$1) SWIG_croak("ecmdClientPerlapi.i::expected a reference");
  sv_setiv(tempsv, (IV) *$1);
}

%typemap(in) char * i_argv[] {
	AV *tempav;
	I32 len;
	int i;
	SV **tv;
	if (!SvROK($input))
	   croak("ecmdClientPerlapi.i::Argumen $argnum is not a reference.\n");
	if (SvTYPE(SvRV($input)) != SVt_PVAV)
	   croak("ecmdClientPerlapi.i::Argumen $argnum is not an array.\n");
	tempav = (AV*)SvRV($input);
	len = av_len(tempav);
	$1 = (char **) malloc((len+2)*sizeof(char *));
	for(i=0; i <= len; i++) {
	   tv = av_fetch(tempav, i, 0);
	   $1[i] = (char *) SvPV(*tv,PL_na);
	}
	$1[i] = NULL;
};

%typemap(freearg) char *i_argv[] {
	free($1);
}


%typemap(argout) char **i_argv {

	AV *myav;
	SV **svs;
	int i = 0, len = 0;
	/* figure out how many elements we have */
	while($1[len]) len++;
	svs = (SV **) malloc(len*sizeof(SV *));
	for (i =0; i < len; i++) {
	  svs[i] = sv_newmortal();
	  sv_setpv((SV*)svs[i],$1[i]);
	}
	myav = av_make(len,svs);
	free(svs);
	$result = newRV((SV*)myav);
	sv_2mortal($result);
	argvi++;
}


%exception {
	$function
	if ((ecmdPerlInterfaceErrorCheck(-1)) &&
	   (ecmdQuerySafeMode() == 1)) {
		croak("ecmdClientPerlapi.i::Error occured in eCMD Perl module - execution halted\n");
	}
}

%{
#include "ecmdClientPerlapi.H"
%}

%include ecmdClientPerlapi.H
