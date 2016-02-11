#!/usr/bin/perl
# File pythonizeDocs.pl created by Jason Albert,6A5244 at 15:07:10 on Thu May 17 2007. 

use strict;

my $rc;

# Open up all the .H files and modify source to change the C variables to pthon variables
my $outputDirectory = $ENV{"DOXYGEN_PYAPI_PATH"};
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

      print OUTFILE $fileLine;
    }

    close OUTFILE;
  }
}
