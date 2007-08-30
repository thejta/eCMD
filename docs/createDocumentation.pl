#!/usr/bin/perl
# File createDocumentation.pl created by Jason Albert,6A5244 at 15:07:10 on Thu May 17 2007. 

use strict;

my $command;
my $rc;

if ($#ARGV != 2) {
  printf("Invalid number of command line options given!\n");
  exit(1);
}

# ARGV[0] will be the place to write this all out to
my $outputDirectory = shift(@ARGV);
# ARGV[1] will be the root of the ecmd CVS
my $cvsBase = shift(@ARGV);
# ARGV[2] will be the release we are generating
my $version = shift(@ARGV);

system("mkdir -p $outputDirectory");

# Do the C-API
printf("Creating C-API Documentation (html)...\n\n");
system("mkdir -p $outputDirectory/Capi");

# Copy over the header files we are going to process
$rc = system("cp $cvsBase/capi/ecmdStructs.H $outputDirectory/Capi/.");
if ($rc) { return $rc; }
$rc = system("cp $cvsBase/capi/ecmdDataBuffer.H $outputDirectory/Capi/.");
if ($rc) { return $rc; }
$rc = system("cp $cvsBase/capi/ecmdReturnCodes.H $outputDirectory/Capi/.");
if ($rc) { return $rc; }
$rc = system("cp $cvsBase/capi/ecmdUtils.H $outputDirectory/Capi/.");
if ($rc) { return $rc; }
$rc = system("cp $cvsBase/capi/ecmdClientCapi.H $outputDirectory/Capi/.");
if ($rc) { return $rc; }
$rc = system("cp $cvsBase/capi/ecmdSharedUtils.H $outputDirectory/Capi/.");
if ($rc) { return $rc; }

# Now copy over all of the extension file headers that are available
my @extensions = split(/\s+/, `ls $cvsBase/ext/ | grep -v CVS | grep -v template`);
for (my $x = 0; $x <= $#extensions; $x++) {
  $rc = system("cp $cvsBase/ext/$extensions[$x]/capi/$extensions[$x]ClientCapi.H $outputDirectory/Capi/.");
  if ($rc) { return $rc; }
  $rc = system("cp $cvsBase/ext/$extensions[$x]/capi/$extensions[$x]Structs.H $outputDirectory/Capi/.");
  if ($rc) { return $rc; }
}

# Update the version strings
#system("sed \"s/VERSION/$version/g\" ecmdDoxygen.config | sed \"s/RELEASE/$release/g\" > $outputDirectory/ecmdDoxygen.config");
$rc = system("sed \"s!VERSION!$version!g\" $cvsBase/docs/ecmdDoxygen.config | sed \"s!INPUTOUTPUT_PATH!$outputDirectory/Capi!g\" > $outputDirectory/Capi/ecmdDoxygen.config");
if ($rc) { return $rc; }

$rc = system("cd $outputDirectory/Capi; /usr/bin/doxygen ecmdDoxygen.config");
if ($rc) { return $rc; }

printf("Creating C-API Documentation (pdf)...\n\n");
$rc = system("cd $outputDirectory/Capi/latex; gmake; mv refman.pdf ecmdClientCapi.pdf");
if ($rc) { return $rc; }

# Now do the Perl-API
printf("\n\nCreating Perl-API Documentation (html) ...\n\n");
system("mkdir -p $outputDirectory/Perlapi");

# Copy over the header files we are going to process
$rc = system("cp $cvsBase/perlapi/ecmdBit64.H $outputDirectory/Perlapi/.");
if ($rc) { return $rc; }
$rc = system("cp $cvsBase/capi/ecmdDataBuffer.H $outputDirectory/Perlapi/.");
if ($rc) { return $rc; }
$rc = system("cp $cvsBase/capi/ecmdStructs.H $outputDirectory/Perlapi/.");
if ($rc) { return $rc; }
$rc = system("cp $cvsBase/capi/ecmdUtils.H $outputDirectory/Perlapi/.");
if ($rc) { return $rc; }
$rc = system("cp $cvsBase/capi/ecmdSharedUtils.H $outputDirectory/Perlapi/.");
if ($rc) { return $rc; }

# Generate the base
$rc = system("cd $cvsBase/perlapi/;./makepm.pl ecmd ecmdClientPerlapiFunc.H");
if ($rc) { return $rc; }

# I'm grepping out the *PerlapiFunc.H to eliminate a doxygen error that happens from having a filename
# in the comments that is different from the actual file
$rc = system("cat $cvsBase/perlapi/ecmdClientPerlapi.H $cvsBase/perlapi/ecmdClientPerlapiFunc.H | grep -v ecmdClientPerlapiFunc.H > $outputDirectory/Perlapi/ecmdClientPerlapi.H");
if ($rc) { return $rc; }

# Now copy over all of the extension file headers that are available
my @extensions = split(/\s+/, `ls $cvsBase/ext/ | grep -v CVS | grep -v template`);
for (my $x = 0; $x <= $#extensions; $x++) {
  $rc = system("cd $cvsBase/ext/$extensions[$x]/perlapi/;./makepm.pl $extensions[$x] $extensions[$x]ClientPerlapiFunc.H");
  if ($rc) { return $rc; }
  # I'm grepping out the *PerlapiFunc.H to eliminate a doxygen error that happens from having a filename
  # in the comments that is different from the actual file
  $rc = system("cat $cvsBase/ext/$extensions[$x]/perlapi/$extensions[$x]ClientPerlapi.H $cvsBase/ext/$extensions[$x]/perlapi/$extensions[$x]ClientPerlapiFunc.H | grep -v $extensions[$x]ClientPerlapiFunc.H > $outputDirectory/Perlapi/$extensions[$x]ClientPerlapi.H");
  if ($rc) { return $rc; }
}

# Update the version strings
#system("sed \"s/VERSION/$version/g\" ecmdDoxygen.config | sed \"s/RELEASE/$release/g\" > $outputDirectory/ecmdDoxygen.config");
$rc = system("sed \"s!VERSION!$version!g\" $cvsBase/docs/ecmdDoxygenPm.config | sed \"s!INPUTOUTPUT_PATH!$outputDirectory/Perlapi!g\" > $outputDirectory/Perlapi/ecmdDoxygenPm.config");
if ($rc) { return $rc; }

$rc = system("cd $outputDirectory/Perlapi; /usr/bin/doxygen ecmdDoxygenPm.config");
if ($rc) { return $rc; }

printf("Creating Perl-API Documentation (pdf)...\n\n");
$rc = system("cd $outputDirectory/Perlapi/latex; gmake; mv refman.pdf ecmdClientPerlapi.pdf");
if ($rc) { return $rc; }

