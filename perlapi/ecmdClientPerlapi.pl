# ---------- OPERATOR OVERLOADS -------------

package ecmd::ecmdDataBuffer;

use overload
  fallback => 1, # This says I won't be defining all operators and to use the default for the rest
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
