# $Header$
# ---------------------------------------------------------------
# This file gets included into the ecmd.pm when swig generates it
# Include any extra perl functionality here
# ---------------------------------------------------------------

### As of Release 8.3, this file no longer needed to be included ###
### If perl code needs to be inserted into the ecmd pel module in the future, please add it to this
### file and re-include it in ecmdClientPerlapi.i

package ecmd::ecmdDataBuffer;
use overload
    "&" => sub { $_[0]->__and__($_[1])},
    "|" => sub { $_[0]->__or__($_[1])},
    "fallback" => 1;

package ecmd::ecmdBit64;
use overload
    "|" => sub { $_[0]->__or__($_[1])},
    "&" => sub { $_[0]->__and__($_[1])},
    # It seem that SWIG only generates the __lshift__ and __rshift__ functions, but not the operators for them - JTA 09/13/09
    "<<" => sub { $_[0]->__lshift__($_[1])},
    ">>" => sub { $_[0]->__rshift__($_[1])},
    "fallback" => 1;
