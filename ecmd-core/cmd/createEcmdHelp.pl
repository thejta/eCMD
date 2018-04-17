#!/usr/bin/perl
# File createEcmdHelp.pl created by Meghna Paruthi, at 11:16:31 on Thu Dec 16 2004. 

use strict;

#This seems to keep the directory intact. Tested in ksh, Jason will test in csh
my $callingPwd;
my $pwd;
chomp($callingPwd = `cd .;pwd`);
# Now do the rest
my @tempArr = split(/\/+/,$0);
pop(@tempArr); # The script name
$" = "/"; # Make it so the array below seperates on / instead of space
#If it starts with a ., it's being called with relative path
if (substr($0,0,1) ne "/") {
  $pwd = $callingPwd;  # Get where I got called from
  $pwd = $pwd . "/" . "@tempArr";  # Now use the relative to find out where the script is 
} else { # Absolute path
  $pwd = "@tempArr";
}
$" = " "; # Reset
chomp($pwd = `cd $pwd;pwd`);  # Get to where this script resides


# Filter the list down based upon the environment variable if it is set
my @extensions;
if ($ENV{"EXTENSIONS"} ne "") {
  @extensions = sort split(/\s+/, $ENV{"EXTENSIONS"})
}

# Add the common command onto the front
my @helpCategories = ("common", @extensions);

if ($#ARGV != 0) {
  die "output file not specified";
}

#############################################
# Hash for the Common Cmds and the Extensions
#############################################
my %helpCategHash;

foreach my $categ(@helpCategories) {
  if($categ eq "common") {
    $helpCategHash{$categ}{msg} = "        Common Commands:\n";
    $helpCategHash{$categ}{dir} = "./help/";
  } else {
    my $pathname = "EXT_" . $categ . "_PATH";
    my $helppath = $ENV{$pathname} . "/cmd/help/";
    if (-d $helppath ) {
      $helpCategHash{$categ}{msg} = "        $categ Extension Commands:\n";
      $helpCategHash{$categ}{dir} = $helppath;
    }
  }
}


##Open eCmd help file for writing
my $ecmdHelpHtxt = $ARGV[0];
if ( ! open( FH, "> $ecmdHelpHtxt" ) ){
    printf("ERROR: Cannot open ecmd help text file $ecmdHelpHtxt to write.");
    exit -1;
}
	 
############################################
# Print the eCmd Common Help Section 
############################################
print FH "Syntax: ecmd\n\n";

print FH "        ECMD:           Core Common Function\n\n";

print FH "        Function:       Top level view of the commands available from the eCMD command\n";
print FH "                        line.  For more details enter the command of interest at the \n";
print FH "                        command line specifing the \"-h\" flag.\n\n";

print FH "        Example:        ecmd -h\n";
print FH "        --------------------------------------------------------------------------------\n\n";


###########################################################
# Get a list of ecmds in the Different Categories & print it
###########################################################
foreach my $categ(@helpCategories) {
  if ($helpCategHash{$categ}{msg} ne "") {
    print FH $helpCategHash{$categ}{msg};
    print FH "        --------------------------------------------------------------------------------\n\n";
    my @common_cmds = &getCmdList($helpCategHash{$categ}{dir});
    foreach(@common_cmds) {
      print FH "        $_\n";
    }
    print FH "\n\n";
  }
}

close FH;

##############################################
# Function to get the list of helpfile names
##############################################
sub getCmdList {
  my( $helpDir ) = @_;
  
  #Get a list of htxt files
  my @helpFiles = `ls $helpDir | grep '\.htxt'`;
  
  foreach my $file(@helpFiles) {
    chomp $file;
    $file =~ s/\.htxt//;
  }
  
  sort @helpFiles;
  
  my @printableRows;
  my $prevcmd;
  my $i=0;
  #For the printable version wrap after 30 rows
  foreach my $cmd (@helpFiles) {
    $cmd =~ /^([a-zA-Z])/; my $curCmdFirstLetter = $1;
    $prevcmd =~ /^([a-zA-Z])/; my $prevCmdFirstLetter = $1;
    
    #Put blank before the cmd beginning with a new letter
    if($curCmdFirstLetter ne $prevCmdFirstLetter) {
      $printableRows[$i++] .= sprintf("%25s");
    }
    $printableRows[$i++] .= sprintf("%-25s",$cmd);
    #Start a new column
    if($i == 30) {$i = 0;}
    
    $prevcmd = $cmd;
  }
  return @printableRows;
}
