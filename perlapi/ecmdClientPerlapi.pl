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

# This adds the code to enable the ecmdBit64 operators
package ecmd::ecmdBit64;

use overload
  fallback => 1, # Use the default operators for the ones not defined here
  '==' => \&operatorEqualTo,
  '!=' => \&operatorNotEqualTo,
   '<' => \&operatorLessThan,
  '<=' => \&operatorLessEqualThan,
   '>' => \&operatorGreaterThan,
  '>=' => \&operatorGreaterEqualThan,
   '&' => \&operatorAnd,
   '|' => \&operatorOr,
   '^' => \&operatorXor,
   '+' => \&operatorPlus,
   '-' => \&operatorMinus,
   '*' => \&operatorMultiply,
   '/' => \&operatorDivide,
   '%' => \&operatorMod,
   '!' => \&operatorNot,
   '~' => \&operatorBitNot,
  '++' => \&operatorIncrement,
  '--' => \&operatorDecrement,
  '<<' => \&operatorLeftShift,
  '>>' => \&operatorRightShift,
;

sub operatorEqualTo {
  my ($self,$rhs) = @_;
  return $self->operatorEqualTo($rhs);
}

sub operatorNotEqualTo {
  my ($self,$rhs) = @_;
  return $self->operatorNotEqualTo($rhs);
}

sub operatorLessThan {
  my ($self,$rhs) = @_;
  return $self->operatorLessThan($rhs);
}

sub operatorLessEqualThan {
  my ($self,$rhs) = @_;
  return $self->operatorLessEqualThan($rhs);
}

sub operatorGreaterThan {
  my ($self,$rhs) = @_;
  return $self->operatorGreaterThan($rhs);
}

sub operatorGreaterEqualThan {
  my ($self,$rhs) = @_;
  return $self->operatorGreaterEqualThan($rhs);
}

sub operatorAnd {
  my ($self,$rhs) = @_;
  return $self->operatorAnd($rhs);
}

sub operatorOr {
  my ($self,$rhs) = @_;
  return $self->operatorOr($rhs);
}

sub operatorXor {
  my ($self,$rhs) = @_;
  return $self->operatorXor($rhs);
}

sub operatorPlus {
  my ($self,$rhs) = @_;
  return $self->operatorPlus($rhs);
}
sub operatorMinus {
  my ($self,$rhs) = @_;
  return $self->operatorMinus($rhs);
}
sub operatorMultiply {
  my ($self,$rhs) = @_;
  return $self->operatorMultiply($rhs);
}
sub operatorDivide {
  my ($self,$rhs) = @_;
  return $self->operatorDivide($rhs);
}
sub operatorMod {
  my ($self,$rhs) = @_;
  return $self->operatorMod($rhs);
}
sub operatorNot {
  my ($self) = @_;
  return $self->operatorNot();
}
sub operatorBitNot {
  my ($self) = @_;
  return $self->operatorBitNot();
}
sub operatorIncrement {
  my ($self) = @_;
  return $self->operatorIncrement(1);
}
sub operatorDecrement {
  my ($self) = @_;
  return $self->operatorDecrement(1);
}
sub operatorLeftShift {
  my ($self,$rhs) = @_;
  return $self->operatorLeftShift($rhs);
}
sub operatorRightShift {
  my ($self,$rhs) = @_;
  return $self->operatorRightShift($rhs);
}
