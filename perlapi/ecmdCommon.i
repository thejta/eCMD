// $Header$
// Additional typemaps for cases not covered by SWIG
// Jason Albert

%typemap(in) char** io_argv (AV* tempAV) {
  int perlLength;
  int i;
  SV  **tv;

  if (!SvROK($input))
    croak("Argument $argnum is not a reference.");
  if (SvTYPE(SvRV($input)) != SVt_PVAV)
    croak("Argument $argnum is not an array.");
  tempAV = (AV*)SvRV($input);
  perlLength = av_len(tempAV);
  perlLength++; // Increment this to keep the C and Perl idea of length the same
  $1 = (char **) malloc((perlLength+1)*sizeof(char *));
  for (i = 0; i < perlLength; i++) {
    tv = av_fetch(tempAV, i, 0);	
    $1[i] = (char *) SvPV(*tv,PL_na);
  }
  $1[i] = NULL;
};

// NOTE:  This typemap only supports the char** array coming back being the same size or smaller than the
//        original perl array. It can be extended to suppot larger char** array's, but not needed currently.
%typemap(argout) char** io_argv {
  AV* tempAV = (AV*)SvRV($arg);  // This makes sense in the generated code
  SV **svs;
  int i = 0,cLength = 0,perlLength = 0;

  perlLength = av_len(tempAV);
  perlLength++; // Increment this to keep the C and Perl idea of length the same
  /* Figure out how many elements we have */
  while ($1[cLength]) cLength++;
  /* Loop through and reaasign everything to the perl array */
  for (i = 0; i < cLength ; i++) {
    svs = av_fetch(tempAV, i, 0);  // Get a pointer to the scalar in the array	
    sv_setpv(*svs, $1[i]);  // Modify the value of the scalar returned to the value returned
  };

  // If the C char** array that returned had been shortened, the perl array needs to be as well
  while (cLength < perlLength) {
    av_pop(tempAV);
    perlLength--;
  }
}

%typemap(in) enum SWIGTYPE &REFERENCE ($1_basetype dvalue)
{
  SV *tempsv;
  if (!SvROK($input)) {
    SWIG_croak("expected a reference");
  }
  tempsv = SvRV($input);
  //if (!SvIOK(tempsv)) {
  //  SWIG_croak("expected an enum reference");
  //}
  dvalue = ($1_basetype) SvIV(tempsv);
  $1 = &dvalue;
}

%typemap(argout) enum SWIGTYPE &REFERENCE {
  SV *tempsv;
  tempsv = SvRV($input);
  if (!$1) SWIG_croak("expected a reference");
  sv_setiv(tempsv, (IV) *$1);
}

// This is needed to get uint32_t by reference working in functions with optional arg wrapping.
%typecheck(SWIG_TYPECHECK_INTEGER)
	 int &, short &, long &,
 	 unsigned int &, unsigned short &, unsigned long &,
	 signed char &, unsigned char &,
	 long long &, unsigned long long &, uint32_t &
{
  $1 = SvROK($input) ? 1 : 0;
}


// These typemaps are setup to take either string values or ecmdBit64 classes for uint64_t args
// typecheck looking for either an incoming ecmdBit64 or a string
%typecheck(SWIG_TYPECHECK_UINT64)
        uint64_t, uint64_t &, uint64_t *
{
  ecmdBit64* tmp;
  if (SWIG_ConvertPtr($input, (void **) &tmp, SWIGTYPE_p_ecmdBit64,0) >= 0) {
    _v = 1;
  } else {
    _v = SvPOK($input) ? 1 : 0;
  }
}



%typemap(in) ecmdBit64 {
  ecmdBit64* tmp;
  if (SWIG_ConvertPtr($input, (void **) &tmp, SWIGTYPE_p_ecmdBit64,0) >= 0) {
    $1 = tmp->getRawValue();
  } else {
    $1 = (uint64_t) strtoull(SvPV($input, PL_na), 0, 0);
  }
}

%typemap(in) ecmdBit64 &REFERENCE ($1_basetype dvalue), ecmdBit64 *REFERENCE ($1_basetype dvalue){

  ecmdBit64* tmp;
  if (SWIG_ConvertPtr($input, (void **) &tmp, SWIGTYPE_p_ecmdBit64,0) >= 0) {
    dvalue = tmp->getRawValue();
    $1 = &dvalue;
  } else {
    dvalue = (uint64_t) strtoull(SvPV($input, PL_na), 0, 0);
    $1 = &dvalue;
  }
}

%typemap(argout) ecmdBit64 &REFERENCE, ecmdBit64 *REFERENCE {
    char temp[256];
    ecmdBit64* tmp;

    if (argvi >= items) {
	EXTEND(sp,1);
    }
    if (SWIG_ConvertPtr($arg, (void **) &tmp, SWIGTYPE_p_ecmdBit64,0) >= 0) {
      tmp->setRawValue(*$1);
    } else {
      char* prev = SvPV($arg, PL_na);
      if (prev != NULL && prev[0] == '0' && prev[1] == 'x') {
        sprintf(temp,"0x%llX", (unsigned long long)*($1));
      } else {
        sprintf(temp,"%llu", (unsigned long long)*($1));
      }
      sv_setpv($arg,temp);
    }
}

// This typemap is for functions that return uint64_t(ecmdDataBuffer::getDoubleWord) and also uint64_t members of structures returned ecmdQueryArray

%typemap(out) uint64_t {
// This commented stuff returns the data as a string instead of ecmdBit64
//  char temp[256];
//  if (argvi >= items) EXTEND(sp, 1);	// bump stack ptr, if needed
//  sprintf(temp,"%llu", (unsigned long long)($1));
//  sv_setpv($result = sv_newmortal(),temp);
//  ++argvi;

  if (argvi >= items) EXTEND(sp, 1);	// bump stack ptr, if needed
  $result = sv_newmortal();
  ecmdBit64* tmp = new ecmdBit64();
  SWIG_MakePtr(ST(argvi++), (void *) tmp, SWIGTYPE_p_ecmdBit64, $shadow|$owner);
  tmp->setRawValue($1);

}
