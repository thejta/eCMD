#!/usr/bin/perl
# File createDocumentation.pl created by Jason Albert,6A5244 at 15:07:10 on Thu May 17 2007. 

use strict;

my $command;
my $rc;
# use xsite path as default to get newer version - can chage this with -dox_path
my $doxygen_path = "/opt/xsite/cte/tools/doxygen/";

if (($#ARGV != 2) && ($#ARGV != 4)) {
  printf("Invalid number of command line options given!\n");
  exit(1);
}

#####################################################
# Get the OS
#
my $OS;
if (`uname` eq "AIX\n") {
  $OS = "aix";
} elsif (`uname -a|grep ppc` ne "") {
  $OS = "ppc";
} else {
  $OS = "x86";
}

# ARGV[0] will be the place to write this all out to
my $outputDirectory = shift(@ARGV);
# ARGV[1] will be the root of the ecmd repository
my $repoBase = shift(@ARGV);
# ARGV[2] will be the release we are generating
my $version = shift(@ARGV);

# If we still have args, assume they are some optional ones
if ($#ARGV != -1) {
  for (my $x = 0; $x <= $#ARGV; $x++) {
      my $ARG = shift @ARGV;
      if ($ARG eq "-dox_path") {
	  $doxygen_path = shift @ARGV;
      }
      else {
	  printf("Invalid command line option given!\n");
	  exit(1);
      }
  }
}


system("mkdir -p $outputDirectory");

# Set the environment for the script to have the latex install in CTE at the front of the path
#$ENV{"PATH"} = "/gsa/rchgsa/projects/e/ecmd/utils/$OS/bin:" . $ENV{"PATH"};

# Do the C-API
printf("Creating C-API Documentation (html)...\n\n");
system("mkdir -p $outputDirectory/Capi");
system("mkdir -p $outputDirectory/Capi/examples");

# Copy over the header files we are going to process
$rc = system("cp $repoBase/capi/ecmdStructs.H $outputDirectory/Capi/.");
if ($rc) { return $rc; }
$rc = system("cp $repoBase/capi/ecmdDataBuffer.H $outputDirectory/Capi/.");
if ($rc) { return $rc; }
$rc = system("cp $repoBase/capi/ecmdDataBufferBase.H $outputDirectory/Capi/.");
if ($rc) { return $rc; }
$rc = system("cp $repoBase/capi/ecmdReturnCodes.H $outputDirectory/Capi/.");
if ($rc) { return $rc; }
$rc = system("cp $repoBase/capi/ecmdUtils.H $outputDirectory/Capi/.");
if ($rc) { return $rc; }
$rc = system("cp $repoBase/capi/ecmdClientCapi.H $outputDirectory/Capi/.");
if ($rc) { return $rc; }
$rc = system("cp $repoBase/capi/ecmdSharedUtils.H $outputDirectory/Capi/.");
if ($rc) { return $rc; }
$rc = system($ENV{ECMD_ROOT} . "/mkscripts/makeext.py doxygen capi $outputDirectory/Capi/");
if ($rc) { return $rc; }

# Copy over example files
$rc = system("cp $repoBase/docs/examples/ecmdclient.C $outputDirectory/Capi/examples/.");
if ($rc) { return $rc; }
$rc = system("cp $repoBase/docs/examples/makefile $outputDirectory/Capi/examples/.");
if ($rc) { return $rc; }
$rc = system("cp $repoBase/capi/capi.dox $outputDirectory/Capi/examples/.");
if ($rc) { return $rc; }


# Now copy over all of the extension file headers that are available
# Filter the list down based upon the environment variable if it is set
my @extensions;
if ($ENV{"EXTENSIONS"} ne "") {
  @extensions = split(/\s+/, $ENV{"EXTENSIONS"})
} else {
  @extensions = split(/\s+/, `ls $repoBase/ext/ | grep -v template`);
}

# Create the list of extension defines to be fed into doxygen
# We'll use the loop below to create it so we don't do two loops in a row
my $extensionDefines;

for (my $x = 0; $x <= $#extensions; $x++) {
  
  # Create the extension define
  $extensionDefines .= "ECMD_" . uc($extensions[$x]) . "_EXTENSION_SUPPORT ";

  $rc = system("cp $repoBase/ext/$extensions[$x]/capi/$extensions[$x]ClientCapi.H $outputDirectory/Capi/.");
  if ($rc) { return $rc; }

  $rc = system("cp $repoBase/ext/$extensions[$x]/capi/$extensions[$x]Structs.H $outputDirectory/Capi/.");
  if ($rc) { return $rc; }

  # fapi/fapi2 specific stuff
  # There are a lot of extra header files in these extensions, and so that this script doesn't have to get modified
  # every time a new one is added, we'll just copy all of them over.
  if (($extensions[$x] eq "fapi") || ($extensions[$x] eq "fapi2")) {
    $rc = system("cp $repoBase/ext/$extensions[$x]/capi/*.H $outputDirectory/Capi/.");
    if ($rc) { return $rc; }
    # removing a few  files that blow up the memory in latex pdf creation later. 
    # Including them also made the webpage doc difficult to find more useful info
    if ($extensions[$x] eq "fapi") {
	$rc = system("rm $outputDirectory/Capi/fapiAttributeIds.H $outputDirectory/Capi/fapiHwpReturnCodes.H $outputDirectory/Capi/fapiHwpErrorInfo.H");
	if ($rc) { return $rc; }
    }
    if ($extensions[$x] eq "fapi2") {
	$rc = system("rm $outputDirectory/Capi/attribute_ids.H");
	if ($rc) { return $rc; }
    }
  }
}

# Update the version strings
#system("sed \"s/VERSION/$version/g\" ecmdDoxygen.config | sed \"s/RELEASE/$release/g\" > $outputDirectory/ecmdDoxygen.config");
$rc = system("sed \"s!ECMDVERSION!$version!g\" $repoBase/docs/ecmdDoxygen.config | sed \"s!INPUTOUTPUT_PATH!$outputDirectory/Capi!g\" > $outputDirectory/Capi/ecmdDoxygen.config");
if ($rc) { return $rc; }

# Update extension defines
$rc = system("sed -i \"s!ECMDEXTDEFINES!$extensionDefines!g\" $outputDirectory/Capi/ecmdDoxygen.config");
if ($rc) { return $rc; }

$rc = system("cd $outputDirectory/Capi; $doxygen_path/doxygen ecmdDoxygen.config");
if ($rc) { return $rc; }

printf("Creating C-API Documentation (pdf)...\n\n");
$rc = system("cd $outputDirectory/Capi/latex; make; mv refman.pdf ecmdClientCapi.pdf");
#if ($rc) { return $rc; }

# Now do the Perl-API
printf("\n\nCreating Perl-API Documentation (html) ...\n\n");
system("mkdir -p $outputDirectory/Perlapi");
system("mkdir -p $outputDirectory/Perlapi/examples");

# Copy over the header files we are going to process
$rc = system("cp $repoBase/perlapi/ecmdBit64.H $outputDirectory/Perlapi/.");
if ($rc) { return $rc; }
$rc = system("cp $repoBase/perlapi/ecmdPerlApiTypes.H $outputDirectory/Perlapi/.");
if ($rc) { return $rc; }
$rc = system("cp $repoBase/capi/ecmdDataBuffer.H $outputDirectory/Perlapi/.");
if ($rc) { return $rc; }
$rc = system("cp $repoBase/capi/ecmdStructs.H $outputDirectory/Perlapi/.");
if ($rc) { return $rc; }
$rc = system("cp $repoBase/capi/ecmdUtils.H $outputDirectory/Perlapi/.");
if ($rc) { return $rc; }
$rc = system("cp $repoBase/capi/ecmdSharedUtils.H $outputDirectory/Perlapi/.");
if ($rc) { return $rc; }
$rc = system($ENV{ECMD_ROOT} . "/mkscripts/makeext.py doxygen perlapi $outputDirectory/Perlapi/");
if ($rc) { return $rc; }

# Copy over example files
$rc = system("cp $repoBase/docs/examples/example.pl $outputDirectory/Perlapi/examples/.");
if ($rc) { return $rc; }

# Generate the base
$rc = system("cd $repoBase/perlapi/;$repoBase/perlapi/makepm.pl ecmd ecmdClientPerlapiFunc.H");
if ($rc) { return $rc; }

# I'm grepping out the *PerlapiFunc.H to eliminate a doxygen error that happens from having a filename
# in the comments that is different from the actual file
$rc = system("cat $repoBase/perlapi/ecmdClientPerlapi.H $repoBase/perlapi/ecmdClientPerlapiFunc.H | grep -v ecmdClientPerlapiFunc.H > $outputDirectory/Perlapi/ecmdClientPerlapi.H");
if ($rc) { return $rc; }

# Now copy over all of the extension file headers that are available
for (my $x = 0; $x <= $#extensions; $x++) {
  if (($extensions[$x] ne "fapi") && ($extensions[$x] ne "fapi2")) {
    $rc = system("cd $repoBase/ext/$extensions[$x]/perlapi/;$repoBase/perlapi/makepm.pl $extensions[$x] $extensions[$x]ClientPerlapiFunc.H");
    if ($rc) { return $rc; }
    # I'm grepping out the *PerlapiFunc.H to eliminate a doxygen error that happens from having a filename
    # in the comments that is different from the actual file
    $rc = system("cat $repoBase/ext/$extensions[$x]/perlapi/$extensions[$x]ClientPerlapi.H $repoBase/ext/$extensions[$x]/perlapi/$extensions[$x]ClientPerlapiFunc.H | grep -v $extensions[$x]ClientPerlapiFunc.H > $outputDirectory/Perlapi/$extensions[$x]ClientPerlapi.H");
    if ($rc) { return $rc; }
    # Grab the structs files too
    $rc = system("cp $repoBase/ext/$extensions[$x]/capi/$extensions[$x]Structs.H $outputDirectory/Perlapi/.");
    if ($rc) { return $rc; }
  } else {
    $rc = system("cp $repoBase/ext/$extensions[$x]/perlapi/$extensions[$x]ClientPerlapi.H $outputDirectory/Perlapi/.");
    if ($rc) { return $rc; }
  }
}

# Open up all the .H files and modify source to change the C variables to perl variables
my @files = split(/\s+/,`ls $outputDirectory/Perlapi/`);
for (my $x = 0; $x <= $#files; $x++) {
  if (-f "$outputDirectory/Perlapi/$files[$x]") {
    open (INFILE, "<$outputDirectory/Perlapi/$files[$x]") or die "Couldn't open the file $files[$x] for input\n";
    my @fileLines = <INFILE>;
    my $fileLine;
    close INFILE;

    open (OUTFILE, ">$outputDirectory/Perlapi/$files[$x]") or die "Couldn't open the file $files[$x] for output\n";

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

# Update the version strings
#system("sed \"s/VERSION/$version/g\" ecmdDoxygen.config | sed \"s/RELEASE/$release/g\" > $outputDirectory/ecmdDoxygen.config");
$rc = system("sed \"s!ECMDVERSION!$version!g\" $repoBase/docs/ecmdDoxygenPm.config | sed \"s!INPUTOUTPUT_PATH!$outputDirectory/Perlapi!g\" > $outputDirectory/Perlapi/ecmdDoxygenPm.config");
if ($rc) { return $rc; }

# Update extension defines
$rc = system("sed -i \"s!ECMDEXTDEFINES!$extensionDefines!g\" $outputDirectory/Perlapi/ecmdDoxygenPm.config");
if ($rc) { return $rc; }

$rc = system("cd $outputDirectory/Perlapi; $doxygen_path/doxygen ecmdDoxygenPm.config");
if ($rc) { return $rc; }

printf("Creating Perl-API Documentation (pdf)...\n\n");
$rc = system("cd $outputDirectory/Perlapi/latex; make; mv refman.pdf ecmdClientPerlapi.pdf");
#if ($rc) { return $rc; }

# Do the Python API
printf("Creating Python API Documentation (html)...\n\n");
system("mkdir -p $outputDirectory/Pythonapi");
system("mkdir -p $outputDirectory/Pythonapi/examples");

# Copy over the header files we are going to process
$rc = system("cp $repoBase/pyapi/ecmdPyApiTypes.H $outputDirectory/Pythonapi/.");
if ($rc) { return $rc; }
$rc = system("cp $repoBase/capi/ecmdStructs.H $outputDirectory/Pythonapi/.");
if ($rc) { return $rc; }
$rc = system("cp $repoBase/capi/ecmdDataBuffer.H $outputDirectory/Pythonapi/.");
if ($rc) { return $rc; }
$rc = system("cp $repoBase/capi/ecmdDataBufferBase.H $outputDirectory/Pythonapi/.");
if ($rc) { return $rc; }
$rc = system("cp $repoBase/capi/ecmdReturnCodes.H $outputDirectory/Pythonapi/.");
if ($rc) { return $rc; }
$rc = system("cp $repoBase/capi/ecmdUtils.H $outputDirectory/Pythonapi/.");
if ($rc) { return $rc; }
$rc = system("cp $repoBase/capi/ecmdClientCapi.H $outputDirectory/Pythonapi/.");
if ($rc) { return $rc; }
$rc = system("cp $repoBase/capi/ecmdSharedUtils.H $outputDirectory/Pythonapi/.");
if ($rc) { return $rc; }
# The one additional python file that is needed
$rc = system("cp $repoBase/pyapi/ecmdClientPyapi.H $outputDirectory/Pythonapi/.");
if ($rc) { return $rc; }
$rc = system($ENV{ECMD_ROOT} . "/mkscripts/makeext.py doxygen pyapi $outputDirectory/Pythonapi/");
if ($rc) { return $rc; }

# Combine ecmdClientCapi.H and ecmdClientPyApi.H into one file
$rc = system("cat $repoBase/pyapi/ecmdClientPyapi.H $repoBase/capi/ecmdClientCapi.H > $outputDirectory/Pythonapi/ecmdClientPyapi.H");
if ($rc) { return $rc; }

# Copy over example files
$rc = system("cp $repoBase/docs/examples/example.py $outputDirectory/Pythonapi/examples/.");
if ($rc) { return $rc; }


for (my $x = 0; $x <= $#extensions; $x++) {
  
  # When doing fapi[2], only do the Pyapi file that has the init extension in it
  # For all other extensions, do them all
  if (($extensions[$x] ne "fapi") && ($extensions[$x] ne "fapi2")) {
    # I'm grepping out the *Capi.H to eliminate a doxygen error that happens from having a filename
    # in the comments that is different from the actual file
    $rc = system("cat $repoBase/ext/$extensions[$x]/capi/$extensions[$x]ClientCapi.H $repoBase/ext/$extensions[$x]/pyapi/$extensions[$x]ClientPyapi.H | grep -v $extensions[$x]ClientCapi.H > $outputDirectory/Pythonapi/$extensions[$x]ClientPyapi.H");
    if ($rc) { return $rc; }

    $rc = system("cp $repoBase/ext/$extensions[$x]/capi/$extensions[$x]Structs.H $outputDirectory/Pythonapi/.");
    if ($rc) { return $rc; }
  } else {
    $rc = system("cp $repoBase/ext/$extensions[$x]/pyapi/$extensions[$x]ClientPyapi.H $outputDirectory/Pythonapi/.");
    if ($rc) { return $rc; }
  }
}

# Open up all the .H files and modify source to change the C variables to pthon variables
my @files = split(/\s+/,`ls $outputDirectory/Pythonapi/`);
for (my $x = 0; $x <= $#files; $x++) {
  if (-f "$outputDirectory/Pythonapi/$files[$x]") {
    open (INFILE, "<$outputDirectory/Pythonapi/$files[$x]") or die "Couldn't open the file $files[$x] for input\n";
    my @fileLines = <INFILE>;
    my $fileLine;
    close INFILE;

    open (OUTFILE, ">$outputDirectory/Pythonapi/$files[$x]") or die "Couldn't open the file $files[$x] for output\n";

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

# Update the version strings
#system("sed \"s/VERSION/$version/g\" ecmdDoxygen.config | sed \"s/RELEASE/$release/g\" > $outputDirectory/ecmdDoxygen.config");
$rc = system("sed \"s!ECMDVERSION!$version!g\" $repoBase/docs/ecmdDoxygenPython.config | sed \"s!INPUTOUTPUT_PATH!$outputDirectory/Pythonapi!g\" > $outputDirectory/Pythonapi/ecmdDoxygenPython.config");
if ($rc) { return $rc; }

# Update extension defines
$rc = system("sed -i \"s!ECMDEXTDEFINES!$extensionDefines!g\" $outputDirectory/Pythonapi/ecmdDoxygenPython.config");
if ($rc) { return $rc; }

$rc = system("cd $outputDirectory/Pythonapi; $doxygen_path/doxygen ecmdDoxygenPython.config");
if ($rc) { return $rc; }

printf("Creating Python API Documentation (pdf)...\n\n");
$rc = system("cd $outputDirectory/Pythonapi/latex; make; mv refman.pdf ecmdClientPythonapi.pdf");
#if ($rc) { return $rc; }
