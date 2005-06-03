#!/usr/bin/perl
# File ecmd_setup.pl created by Jason Albert,6A5244 at 13:50:19 on Fri May 13 2005. 

# Why wouldn't you use strict???
use strict;
use Cwd 'chdir';

##########################################################################
# Figure out where the user is calling this script from
#
my $pwd;
my @tempArr = split(/\/+/,$0);
pop(@tempArr); # The script name
$" = "/"; # Make it so the array below seperates on / instead of space
#If it starts with a ., it's being called with relative path
if (substr($0,0,1) ne "/") {
  chomp($pwd = `pwd`);  # Get where I got called from
  $pwd = $pwd . "/" . "@tempArr";  # Now use the relative to find out where the script is 
} else { # Absolute path
  $pwd = "@tempArr";
}
$" = " "; # Reset
chomp($pwd = `cd $pwd;pwd`);  # Get to where this script resides

# This is a slick bit of trickeration if I may say so myself.
# We have to change to the directory the script is in for the module requires below
# This will enable the modules to be found in the local directory in the @INC
chdir "$pwd";
# Now add the relative paths the modules reside in
use lib '../plugins/cro';
use lib '../plugins/scand';
use lib '../plugins/gip';

# The setup modules to include
require ecmdsetup;
require crosetup;
require scandsetup;
require gipsetup;

##########################################################################
#  Variables
#
my %modified;  # 0 is no change, 1 set needed, -1 unset needed 
my $release;
my $plugin;
my $product;
my $temp;
my $shortcut = 0;
my $local = 1;  # Assume it's local and then disprove it by comparing ctepaths to CTEPATH
# These ctepaths are in regular expression format for the search below.
# This allows the user to put just rchland or rchland.ibm.com, etc..
my @ctepaths = ("\/afs\/rchland(|\.ibm\.com)\/rel\/common\/cte",
                "\/afs\/awd(|\.austin\.ibm\.com)\/projects\/cte",
                "\/afs\/apd(|\.pok\.ibm\.com)\/func\/vlsi\/cte",
                "\/afs\/vlsilab(|\.boeblingen\.ibm\.com)\/proj\/cte",
                "\/afs\/btv(|\.ibm\.com)\/data\/vlsi\/cte",
                "\/afs\/raleigh(|\.ibm\.com)\/cadtools\/cte",
                "\/afs\/watson(|\.ibm\.com)\/projects\/vlsi\/cte");

# Create the setup objects
my $ecmd = new ecmdsetup();
my $cro = new crosetup();
my $scand = new scandsetup();
my $gip = new gipsetup();


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
for (my $x = 0; $x <= $#ctepaths && $local != 0; $x++) {
  if ($ENV{"CTEPATH"} =~ /${ctepaths[$x]}/) {
    $local = 0;
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
  printf("echo Your shell is unsupported!;\n");
  exit;
}

##########################################################################
# Get the release
#
$release = shift(@ARGV);

# Here is where we put in the magic to allow the user to just put a period to cover all three ecmd parms
if ($release eq ".") {
  if ($ENV{"ECMD_RELEASE"} eq "" || $ENV{"ECMD_PLUGIN"} eq "" || $ENV{"ECMD_PRODUCT"} eq "") {
    printf("echo here!;");
    printf("echo You can\\'t specify the \\'.\\' shortcut without having specified the release, product and plugin previously!;");
    exit;
  } else {
    $shortcut = 1;
  }
}

if ($shortcut) {
  $release = $ENV{"ECMD_RELEASE"};
} else {
  # Also set the ECMD_RELEASE variable
  $ENV{"ECMD_RELEASE"} = $release;
  $modified{"ECMD_RELEASE"} = 1;
}

# We'll see if the release is supported based upon the existence of the bin directory
$temp = $ENV{"CTEPATH"} . "/tools/ecmd/" . $release . "/bin";
if (!(-d $temp)) {
  printf("echo The eCMD release you specified is not known!;\n");
  exit;
}

##########################################################################
# Get the plugin
#
if ($shortcut) {
  $plugin = $ENV{"ECMD_PLUGIN"};
} else {
  $plugin = shift(@ARGV);

  # Also set the ECMD_PLUGIN variable
  $ENV{"ECMD_PLUGIN"} = $plugin;
  $modified{"ECMD_PLUGIN"} = 1;
}

if ($plugin eq "cro") {
} elsif ($plugin eq "gip") {
} elsif ($plugin eq "scand") {
} else {
  printf("echo The eCMD plugin you specified is not known!;\n");
  exit;
}


##########################################################################
# Get the product
#
if ($shortcut) {
  $product = $ENV{"ECMD_PRODUCT"};
} else {
  $product = shift(@ARGV);

  # Also set the ECMD_PRODUCT variable
  $ENV{"ECMD_PRODUCT"} = $product;
  $modified{"ECMD_PRODUCT"} = 1;
}

# We no longer want to error check the product.  It is just passed on through to the plugin
#if ($product eq "eclipz") {
#} else {
#  printf("echo The eCMD product you specified is not known!;\n");
#  exit;
#}

##########################################################################
# Cleanup any ecmd bin dirs that might be in the path
#
# Pull out any of the matching cases
# This expression matches anything after a : up to /tools/ecmd/<anything>/bin and then a : or end of line
$ENV{"PATH"} =~ s/([^:]*?)\/tools\/ecmd\/([^\/]*?)\/bin(:|$)/:/g;
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
$cro->cleanup(\%modified);
$scand->cleanup(\%modified, $release);
$gip->cleanup(\%modified);

##########################################################################
# Add bin directory to path
#
$ENV{"PATH"} = $ENV{"CTEPATH"} . "/tools/ecmd/" . $release . "/bin:" . $ENV{"PATH"};
$modified{"PATH"} = 1;

##########################################################################
# Call setup on plugin specified
#
if ($plugin eq "cro") {
  $cro->setup(\%modified, $local, $product, "ecmd", @ARGV);
}
if ($plugin eq "scand") {
  $scand->setup(\%modified, $local, $product, $release, @ARGV);
}
if ($plugin eq "gip") {
  $gip->setup(\%modified, @ARGV);
}

##########################################################################
# Write out the modified environment
#
$ecmd->write_environment($shell,\%modified);


#  Umm.. yeah.. I'm going to need you to work this weekend on the help text.  Mkay..
sub help {
  printf("echo ecmdsetup \\<release\\> \\<plugin\\> \\<product\\> \\<plugin options\\>;");
  printf("echo \\<release\\> - Any eCMD Version currently supported in CVS \\(ex rel, ver5, ver4-3\\);");
  printf("echo \\<plugin\\> - cro\\|scand\\|gip;");
  printf("echo \\<product\\> - eclipz, etc..;");
  printf("echo \\<plugin options\\> - anything else passed into the script is passed onto the plugin;");
  printf("echo -h                               - this help text;");
}
