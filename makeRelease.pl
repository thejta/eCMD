#!/usr/bin/perl
# File makeRelease.pl created by Christopher Engel,894574 at 13:15:50 on Thu Jun  3 2004. 


use strict;


my $type;
my $debug = 0;
my $release = "";
my $home = "/afs/watson/projects/vlsi/cte/tools/ecmd/";
my $cvsroot = ":pserver:cengel\@rhea.rchland.ibm.com:11020/home/cvs/ecmd";
my $x;
my @cvs_gmake_modules = ("capi", "ext/cip/capi", "ext/scand/capi", "ext/cro/capi", "ecmd", "perlapi" );
my @cvs_nogmake_modules = ("bin", "dll", "ext/template/capi", "ext/template/cmd", "ext/cip/cmd", "ext/cro/cmd");
my $module;
my $input;
my $tagcvs;
my $rc;
my $command;

######################################################################################
# Print the help and parse the command line args
######################################################################################
if ($#ARGV < 1 || $ARGV[0] eq '-h') {
  print("makeRelease.pl aix|linux <release> [debug]\n");
  exit;
}

for ($x = 0; $x <= $#ARGV; $x++) {
  if ($ARGV[$x] eq "aix" || $ARGV[$x] eq "linux" ) {
    $type = $ARGV[$x];
  } elsif ($ARGV[$x] eq "debug") {
    $debug = 1;
  } else {
    $release = $ARGV[$x];
  }
}


# Only do this on the linux side
if ($type eq "linux") {

 #################
 # Go create the directories
 printf("Creating Release space ...\n");
 system("mkdir -p $home/$release/.cvs");

 chdir "$home/$release/.cvs/";

 printf("Tag CVS (y/n) : ");
 $tagcvs = get_entry();

 print("Checking out cvs ...\n");
 foreach $module (@cvs_gmake_modules) {
   if ($release eq "dev" || $tagcvs eq "y") {
     $command = "cvs -d $cvsroot checkout $module";
   } else {
     $command = "cvs -d $cvsroot checkout -r $release $module";
   }
   if ($debug) { printf("DEBUG | $command\n"); 
   } else {
     $rc = system($command); if ($rc) {exit;}
   }
 }
 foreach $module (@cvs_nogmake_modules) {
   if ($release eq "dev" || $tagcvs eq "y") {
     $command = "cvs -d $cvsroot checkout $module";
   } else {
     $command = "cvs -d $cvsroot checkout -r $release $module";
   }
   if ($debug) { printf("DEBUG | $command\n"); 
   } else {
     $rc = system($command); if ($rc) {exit;}
   }
 }

 
 if ($tagcvs eq "y") {
   # Print out the version string, make sure it is correct
   print("\n\nDisplaying eCMD Version String\n");
   system("grep ECMD_CAPI_VERSION capi/ecmdStructs.H |grep define");

   printf("Is Version String correct (y/n) : ");
   $input = get_entry();
 
   if ($input eq "n") {
     print("Aborting release, update string and rerun\n");
     exit();
   }

 }
}


print("\n\nBuild the modules ...\n");
foreach $module (@cvs_gmake_modules) {
  print("\nModule : $module\n");

  chdir "$home/$release/.cvs/$module";
  $command = "gmake";
  if ($debug) { printf("DEBUG | $command\n"); 
  } else {
     $rc = system($command); if ($rc) {exit;}
  }
}

if ($type eq "linux") {
  printf("Are all compiles done (ex aix) (y/n) : ");
  $input = get_entry();

  if ($input eq "y") {
    if ($tagcvs eq "y") {

      chdir "$home/$release/.cvs/";
      print("Tagging CVS with $release\n");
      foreach $module (@cvs_gmake_modules) {
        $command = "cvs -d $cvsroot tag $release $module";
        if ($debug) { printf("DEBUG | $command\n"); 
        } else {
          $rc = system($command); if ($rc) {exit;}
        }
      }
      foreach $module (@cvs_nogmake_modules) {
        $command = "cvs -d $cvsroot tag $release $module";
        if ($debug) { printf("DEBUG | $command\n"); 
        } else {
          $rc = system($command); if ($rc) {exit;}
        }
      }
    }
  }


  if ($input eq "y") {
    print("Releasing files ...\n");
    chdir "$home/$release/";

    $command = "cp -R .cvs/bin .";
    if ($debug) { printf("DEBUG | $command\n"); 
    } else {
     $rc = system($command); if ($rc) {exit;}
    }
    $command = "cp -R .cvs/ecmd/help .";
    if ($debug) { printf("DEBUG | $command\n"); 
    } else {
     $rc = system($command); if ($rc) {exit;}
    }

    # The Modules
    foreach $module (@cvs_gmake_modules) {
      $command = "mkdir -p $module";
      if ($debug) { printf("DEBUG | $command\n"); 
      } else {
        $rc = system($command); if ($rc) {exit;}
      }
      $command = "cp .cvs/$module/export/* $module/";
      if ($debug) { printf("DEBUG | $command\n"); 
      } else {
        $rc = system($command); if ($rc) {exit;}
      }
    }


    # Clean up a few that didn't go where I wanted them 

    # The Shared Libraries
    $command = "mkdir -p lib";
    if ($debug) { printf("DEBUG | $command\n"); 
    } else {
     $rc = system($command); if ($rc) {exit;}
    }
    $command = "mv capi/*.so lib/";
    if ($debug) { printf("DEBUG | $command\n"); 
    } else {
     $rc = system($command); if ($rc) {exit;}
    }

    # The Command Line exe
    $command = "mv ecmd/* bin/";
    if ($debug) { printf("DEBUG | $command\n"); 
    } else {
     $rc = system($command); if ($rc) {exit;}
    }
    $command = "rmdir ecmd";
    if ($debug) { printf("DEBUG | $command\n"); 
    } else {
     $rc = system($command); if ($rc) {exit;}
    }



  } # End input="y"


  printf("\nStart CTE Shadow (y/n) : ");
  $input = get_entry();

  if ($input eq "y") {
    $command = "export CTEPATH=/afs/rch/rel/common/cte;/afs/rch/rel/common/cte/tools/bin/ctepass -email cjengel\@us.ibm.com -c ALL tools/ecmd/$release";
    if ($debug) { printf("DEBUG | $command\n"); 
    } else {
     $rc = system($command); if ($rc) {exit;}
    }
  }

  printf("\nStart CTE Shadow of rel/prod links (y/n) : ");
  $input = get_entry();

  if ($input eq "y") {
    $command = "export CTEPATH=/afs/rch/rel/common/cte;/afs/rch/rel/common/cte/tools/bin/ctepass -email cjengel\@us.ibm.com -c ALL tools/ecmd";
    if ($debug) { printf("DEBUG | $command\n"); 
    } else {
     $rc = system($command); if ($rc) {exit;}
    }
  }



} # End if linux

print("eCMD Release '$release' completed!\n");


######################################################################################
# Function to read stdin and check for y or n (a majority of the program prompts)
######################################################################################
sub get_entry {
  my $entry = "NULL";
  my $done = 0;

  while (!$done) {
    chomp($entry = <STDIN>);
    if ($entry eq "y" || $entry eq "n") {
      return $entry;
    } else {
      printf("Please enter either \"y\" or \"n\": ");
    }
  }
}
