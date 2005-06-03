#!/usr/bin/perl
# File ecmdsetup.pm created by Jason Albert,6A5244 at 13:27:35 on Mon May 16 2005. 

package ecmdsetup;

use strict;

# ARGHH!!!  Germany's WAY OLD perl doesn't support this.  Luckily we don't sem to need it
#require Exporter;
#our @ISA = qw(Exporter);
#our @EXPORT = qw ();

sub new {
  my $invocant = shift;
  my $class = ref($invocant) || $invocant;
  my $self = {};
  bless ($self, $class);
  return $self;
}

sub write_environment {
  shift(@_);  # Don't need it
  my $shell = shift(@_);  # Grab the shell the user was in
  my $modRef = shift(@_);  # Grab a reference to the hash reference I passed in.
  my ($value, $key);

  while (($key, $value) = each(%{$modRef})) {
    if ($value == 1) {
      if ($shell eq "ksh") {
        printf("export %s=\"%s\";",$key,$ENV{$key});
      } elsif ($shell eq "csh") {
        printf("setenv %s \"%s\";",$key,$ENV{$key});
      }
    } elsif ($value == -1) {
      if ($shell eq "ksh") {
        printf("unset %s;",$key);
      } elsif ($shell eq "csh") {
        printf("unsetenv %s;",$key);
      }
    } elsif ($value == 0) {
      # Do nothing, no change
    } else {
      printf("Unknown modified value found in hash!  key: $key  value: $value\n");
      exit;
    }
  }
}


1;
