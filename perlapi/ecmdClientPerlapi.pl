# $Header$
# ---------------------------------------------------------------
# This file gets included into the ecmd.pm when swig generates it
# Include any extra perl functionality here
# ---------------------------------------------------------------

# This adds the code to enable the ecmdDataBuffer operators
package ecmd::ecmdDataBuffer;

use overload
  fallback => 1, # Use the default operators for the ones not defined here
  '==' => \&operatorEqualTo,
  '!=' => \&operatorNotEqualTo,
   '&' => \&operatorAnd,
   '|' => \&operatorOr,
;

sub operatorEqualTo {
  my ($self,$rhs) = @_;
  return $self->operatorEqualTo($rhs);
}

sub operatorNotEqualTo {
  my ($self,$rhs) = @_;
  return $self->operatorNotEqualTo($rhs);
}

sub operatorAnd {
  my ($self,$rhs) = @_;
  return $self->operatorAnd($rhs);
}

sub operatorOr {
  my ($self,$rhs) = @_;
  return $self->operatorOr($rhs);
}
