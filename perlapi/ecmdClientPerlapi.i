%module ecmdClientPerlapi
%include typemaps.i

%except(perl5) {
	$function
	if (ecmdPerlInterfaceErrorCheck(-1)) {
		croak("Error occured in Perl module\n");
	}
}

%{
#include "ecmdClientPerlapi.H"
%}

%include ecmdClientPerlapi.H
