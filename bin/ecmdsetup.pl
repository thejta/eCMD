#!/usr/bin/perl
# File ecmd_setup.pl created by Jason Albert,6A5244 at 13:50:19 on Fri May 13 2005. 

# Why wouldn't you use strict???
use strict;
use Cwd 'chdir';
##########################################################################
# Figure out where the user is calling this script from
#

# Meghna needs me to save away the directory the script was called from, I'll do that here
my $callingPwd;
#Commenting this since this seems to be resolving the links in the path
#chomp($callingPwd = `pwd`);
#This seems to keep the directory intact. Tested in ksh, Jason will test in csh
chomp($callingPwd = `cd .;pwd`);
# Now do the rest
my $pwd;
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

# installpath points to root of install
my $installPath = $pwd;
$installPath =~ s/\/([^\/..]*\/?)$/\//;

# This is a slick bit of trickeration if I may say so myself.
# We have to change to the directory the script is in for the module requires below
# This will enable the modules to be found in the local directory in the @INC
chdir "$pwd";

# Now add the relative paths the modules reside in
use lib '../plugins/cro';
use lib '../plugins/scand';
use lib '../plugins/gip';
use lib '../plugins/mbo';

#########################################
# Setup the modules to include
# Base support, always there
require ecmdsetup;
my $ecmd = new ecmdsetup();

# Create the other setup objects based upon what is available
my ($cro, $scand, $gip, $mbo);
# Cronus
my $croUse = 0;
if (-d "../plugins/cro") {
  $croUse = 1;
  require crosetup;
  $cro = new crosetup();
}

# Scand
my $scandUse = 0;
if (-d "../plugins/scand") {
  $scandUse = 1;
  require scandsetup;
  $scand = new scandsetup();
}

# GFW I/P
my $gipUse = 0;
if (-d "../plugins/gip") {
  $gipUse = 1;
  require gipsetup;
  $gip = new gipsetup();
}

# Mambo
my $mboUse = 0;
if (-d "../plugins/mbo") {
  $mboUse = 1;
  require mbosetup;
  $mbo = new mbosetup();
}

##########################################################################
#  Variables
#
my %modified;  # 0 is no change, 1 set needed, -1 unset needed 
my $release;
my $plugin;
my $product;
my $temp;
my $shortcut = 0;
my $localInstall = 1;  # Assume it's a local install and then disprove it by comparing ctepaths to CTEPATH
my $copyLocal = 0;  # Does the user want ECMD_EXE and ECMD_DLL_FILE copied to /tmp and run from there?
my $cleanup = 0;  # Call only cleanup on the plugins to remove anything they might have put out there.
# These ctepaths are in regular expression format for the search below.
# This allows the user to put just rchland or rchland.ibm.com, etc..
my @ctepaths = ("/afs/rchland(|\.ibm\.com)/rel/common/cte",
                "/afs/awd(|\.austin\.ibm\.com)/projects/cte",
                "/afs/austin(|\.ibm\.com)/projects/cte",
                "/afs/apd(|\.pok\.ibm\.com)/func/vlsi/cte",
                "/afs/vlsilab(|\.boeblingen\.ibm\.com)/proj/cte",
                "/afs/bb/proj/cte",
                "/afs/btv(|\.ibm\.com)/data/vlsi/cte",
                "/afs/raleigh(|\.ibm\.com)/cadtools/cte",
                "/afs/watson(|\.ibm\.com)/projects/vlsi/cte");

#####################################################
# Look to see if help was requested
#
if ("@ARGV" =~ /-h/) {
  help();
  $cro->help();
  exit;
}

##########################################################################
# If the user isn't setup for CTE, let's do it for them.
#
#if ($ENV{"CTEPATH"} eq "") {
#  @tempArr = split(/\//,$pwd);
#  # Blow off the first 4 entries that are ecmd dirs
#  pop(@tempArr); # bin
#  pop(@tempArr); # release
#  pop(@tempArr); # ecmd
#  pop(@tempArr); # tools
#  $" = "/"; # Make it so the array below seperates on / instead of space
#  $ENV{"CTEPATH"} = "@tempArr";
#  $" = " "; # Reset
#  #Finally, mark it modified
#  $modified{"CTEPATH"} = 1;
#  printf("echo CTEPATH unset! Setting to %s;\n",$ENV{"CTEPATH"});
#}

##########################################################################
# Figure out if the user is on a local copy of CTE
#
for (my $x = 0; $x <= $#ctepaths && $localInstall != 0; $x++) {
  if ($ENV{"CTEPATH"} =~ m!${ctepaths[$x]}!) {
    $localInstall = 0;
  }
}

##########################################################################
# Get the users shell
#
my $shell = shift(@ARGV);

# If you add a shell here, you need to update the output printing below
if ($shell eq "ksh") {
} elsif ($shell eq "csh") {
} else {
  printf("echo Your shell is unsupported\\!;");
  exit;
}

##########################################################################
# Get the release
#
$release = shift(@ARGV);

# Here is where we put in the magic to allow the user to just put a period to cover all three ecmd parms
if ($release eq ".") {
  if ($ENV{"ECMD_RELEASE"} eq "" || $ENV{"ECMD_PLUGIN"} eq "" || $ENV{"ECMD_PRODUCT"} eq "") {
    printf("echo You can\\'t specify the \\'.\\' shortcut without having specified the release, product and plugin previously\\!;");
    exit;
  } else {
    $shortcut = 1;
  }
}

if ($shortcut) {
  $release = $ENV{"ECMD_RELEASE"};
}

# We'll see if the release is supported based upon the existence of the bin directory
if ($release ne "auto" && !$localInstall) {
  $temp = $ENV{"CTEPATH"} . "/tools/ecmd/" . $release . "/bin";
  if (!(-d $temp)) {
    printf("echo The eCMD release '$release' you specified is not known\\!;");
    exit;
  }
}

##########################################################################
# Get the plugin
#
if ($shortcut) {
  $plugin = $ENV{"ECMD_PLUGIN"};
} else {
  $plugin = shift(@ARGV);
}
if ($plugin eq "cro" && $croUse) {
} elsif ($plugin eq "gip" && $gipUse) {
} elsif ($plugin eq "scand" && $scandUse) {
} elsif ($plugin eq "mbo" && $mboUse) {
} else {
  printf("echo The eCMD plugin '$plugin' you specified is not known\\!;");
  exit;
}


##########################################################################
# Get the product
#
if ($shortcut) {
  $product = $ENV{"ECMD_PRODUCT"};
} else {
  $product = shift(@ARGV);
}

# We no longer want to error check the product.  It is just passed on through to the plugin
#if ($product eq "eclipz") {
#} else {
#  printf("echo The eCMD product you specified is not known\\!;");
#  exit;
#}

##########################################################################
# Loop through the args left and see if any are for ecmd
#
for (my $x = 0; $x <= $#ARGV;) {
  if ($ARGV[$x] eq "copylocal") {
    $copyLocal = 1;
    splice(@ARGV,$x,1);  # Remove so plugin doesn't see it
  } elsif ($ARGV[$x] eq "cleanup") {
    $cleanup = 1;
    printf("echo Removing eCMD and Plugin settings from environment;");
    splice(@ARGV,$x,1);  # Remove so plugin doesn't see it
  } else {
    # We have to walk the array here because the splice shortens up the array
    $x++;
  }
}


##########################################################################
# Cleanup any ecmd bin dirs that might be in the path
#
# Pull out any of the matching cases
# This expression matches anything after a : up to /tools/ecmd/<anything>/bin and then a : or end of line
if ($localInstall) {
  $ENV{"PATH"} =~ s!$installPath/bin!:!g;
} else {
  $ENV{"PATH"} =~ s!([^:]*?)/tools/ecmd/([^\/]*?)/bin(:|$)!:!g;
}
# We might have left a : on the front, remove it
$ENV{"PATH"} =~ s/^://g;
# Same with the back, might have left a :
$ENV{"PATH"} =~ s/:$//g;
# Any multiple : cases, reduce to one
$ENV{"PATH"} =~ s/(:+)/:/g;
# Now mark the path modifed
$modified{"PATH"} = 1;

##########################################################################
# Call cleanup on plugins
#

# Only do this if the plugin has changed from last time
if ($ENV{"ECMD_PLUGIN"} ne $plugin || $cleanup) {
  if ($croUse) { $cro->cleanup(\%modified); }
  if ($scandUse) { $scand->cleanup(\%modified, $release); }
  if ($gipUse) { $gip->cleanup(\%modified); }
  if ($mboUse) { $mbo->cleanup(\%modified, $localInstall, $installPath); }
}

##########################################################################
# Flag the ECMD_* variables as modified if appropriate
#
if (!$shortcut) {
  $ENV{"ECMD_RELEASE"} = $release;
  $modified{"ECMD_RELEASE"} = 1;
  $ENV{"ECMD_PLUGIN"} = $plugin;
  $modified{"ECMD_PLUGIN"} = 1;
  $ENV{"ECMD_PRODUCT"} = $product;
  $modified{"ECMD_PRODUCT"} = 1;
}
if ($cleanup) {
  $modified{"ECMD_RELEASE"} = -1;
  $modified{"ECMD_PLUGIN"} = -1;
  $modified{"ECMD_PRODUCT"} = -1;
}

##########################################################################
# Call setup on plugin specified
#
if (!$cleanup) {
  if ($plugin eq "cro" && $croUse) {
    $cro->setup(\%modified, $localInstall, $product, "ecmd", @ARGV);
  }
  if ($plugin eq "scand" && $scandUse) {
    $scand->setup(\%modified, $localInstall, $product, $callingPwd, @ARGV);
  }
  if ($plugin eq "gip" && $gipUse) {
    $gip->setup(\%modified, $localInstall, $product, @ARGV);
  }
  if ($plugin eq "mbo" && $mboUse) {
    $mbo->setup(\%modified, $localInstall, $product, $installPath, @ARGV);
  }
}

##########################################################################
# Add bin directory to path
#
if (!$cleanup) {
  if ($localInstall) {
    $ENV{"PATH"} = $installPath . "/bin:" . $ENV{"PATH"};
  } else {
    $ENV{"PATH"} = $ENV{"CTEPATH"} . "/tools/ecmd/" . $ENV{"ECMD_RELEASE"} . "/bin:" . $ENV{"PATH"};
  }
  $modified{"PATH"} = 1;
}

####################################################
# Do the copy to /tmp if local was given
#
if ($copyLocal) {
  my $command;
  my @tempArr;
  if ($cleanup) {
    printf("echo Removing directory /tmp/\$ECMD_TARGET/;");
    $command = "rm -r /tmp/" . $ENV{"ECMD_TARGET"};
    system("$command");
  } else {
    printf("echo Copying ECMD_EXE and ECMD_DLL_FILE to /tmp/\$ECMD_TARGET/;");
    $command = "/tmp/" . $ENV{"ECMD_TARGET"};
    if (!(-d $command)) { #if the directory isn't there, create it
      $command = "mkdir " . $command;
      system("$command");
    }
    @tempArr = split(/\//,$ENV{"ECMD_EXE"});
    $command = "cp " . $ENV{"ECMD_EXE"} . " /tmp/" . $ENV{"ECMD_TARGET"} . "/" . $tempArr[$#tempArr];
    system("$command");
    $ENV{"ECMD_EXE"} = "/tmp/" . $ENV{"ECMD_TARGET"} . "/" . $tempArr[$#tempArr];
    $modified{"ECMD_EXE"} = 1;
    @tempArr = split(/\//,$ENV{"ECMD_DLL_FILE"});
    $command = "cp " . $ENV{"ECMD_DLL_FILE"} . " /tmp/" . $ENV{"ECMD_TARGET"} . "/" . $tempArr[$#tempArr];
    system("$command");
    $ENV{"ECMD_DLL_FILE"} = "/tmp/" . $ENV{"ECMD_TARGET"} . "/" . $tempArr[$#tempArr];
    $modified{"ECMD_DLL_FILE"} = 1;
  }
}

##########################################################################
# Write out the modified environment
#
$ecmd->write_environment($shell,\%modified);


#  Umm.. yeah.. I'm going to need you to work this weekend on the help text.  Mkay..
sub help {
  printf("echo ecmdsetup \\<release\\> \\<plugin\\> \\<product\\> \\[copylocal\\] \\[cleanup\\] \\<plugin options\\>;");
  printf("echo \\<release\\> - Any eCMD Version currently supported in CVS \\(ex rel, ver5, ver4-3\\);");
  printf("echo \\<plugin\\> - cro\\|scand\\|gip\\|mbo;");
  printf("echo \\<product\\> - eclipz, etc..;");
  printf("echo \\[copylocal\\] - Copy the \\\$ECMD_EXE and \\\$ECMD_DLL_FILE to /tmp/\\\$ECMD_TARGET/;");
  printf("echo \\[cleanup\\] - Remove all eCMD and Plugin settings from environment;");
  printf("echo \\<plugin options\\> - anything else passed into the script is passed onto the plugin;");
  printf("echo -h                               - this help text;");
}
