// $Header$
// Additional typemaps for cases not covered by SWIG
// Jason Albert

%typemap(in) char* i_argv[] (AV* tempAV) {
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
%typemap(argout) char* i_argv[] {
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

