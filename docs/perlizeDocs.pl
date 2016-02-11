#!/usr/bin/perl
# File perlizeDocs.pl created by Jason Albert,6A5244 at 15:07:10 on Thu May 17 2007. 

use strict;

my $rc;

# Open up all the .H files and modify source to change the C variables to perl variables
my $outputDirectory = $ENV{"DOXYGEN_PERLAPI_PATH"};
my @files = split(/\s+/,`ls $outputDirectory/`);
for (my $x = 0; $x <= $#files; $x++) {
  if (-f "$outputDirectory/$files[$x]") {
    open (INFILE, "<$outputDirectory/$files[$x]") or die "Couldn't open the file $files[$x] for input\n";
    my @fileLines = <INFILE>;
    my $fileLine;
    close INFILE;

    open (OUTFILE, ">$outputDirectory/$files[$x]") or die "Couldn't open the file $files[$x] for output\n";

    foreach $fileLine (@fileLines) {

      # std::list<ecmdXXX> to ecmdXXXList
      $fileLine =~ s/std::list\s*<\s*([^\s]*?)\s*>/$1List/g;

      # std::vector<ecmdXXX> to ecmdXXXVector
      $fileLine =~ s/std::vector\s*<\s*([^\s]*?)\s*>/$1Vector/g;

      # Turn a uint32_t reference into a scalarref
      $fileLine =~ s/uint32_t\s*&/scalarref /g;

      # Turn a char** reference into an arrayref
      $fileLine =~ s/char\s*\*\*/arrayref /g;

      #const char * to scalar
      $fileLine =~ s/const char\s*\*/scalar /g;

      #char * to scalar
      $fileLine =~ s/char\s*\*/scalar/g;

      # vars to convert to scalar
      $fileLine =~ s/(const\s*)?(uint32_t|uint16_t|uint8_t|bool|std::string)/scalar/g;

      # uint64_t to ecmdBit64
      $fileLine =~ s/uint64_t/ecmdBit64/g;

      print OUTFILE $fileLine;
    }

    close OUTFILE;
  }
}
