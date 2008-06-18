#!/bin/sh 
#! -*- perl -*-

eval '
if [ "X$ECMDPERLBIN" = "X" ]; then
 if [ "X$CTEPATH" = "X" ]; then echo "CTEPATH env var is not set."; exit 1; fi
 export ECMDPERLBIN=$CTEPATH/tools/perl/5.8.1/bin/perl;
 export CTEPERLPATH=$CTEPATH/tools/perl/5.8.1;
 if [[ `uname` = "Linux" ]]; then
  export CTEPERLLIB=$CTEPERLLIB:$CTEPERLPATH/lib/5.8.1:./obj_x86/;
  export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:../capi/export";
 else
  export CTEPERLLIB=$CTEPERLLIB:$CTEPERLPATH/lib/5.8.1:./obj_aix/;
  export LIBPATH="$LIBPATH:../capi/export";
 fi
fi

exec $ECMDPERLBIN -x -S $0 ${1+"$@"}
'
if 0;

use strict;
use ecmd; # Must be a use, not a require, for enum support to work

my $rc = 0;

$| = 1;  # set autoflush

if (ecmdLoadDll("","ver5,ver6,ver7,ver8,ver9,ver10")) { exit(0); }


$rc = ecmdCommandArgs(\@ARGV);


if (1) {
  my $o_dllInfo = new ecmd::ecmdDllInfo;

  print "---- starting ecmdQueryDllInfo -----\n";
  $rc = ecmdQueryDllInfo($o_dllInfo);
  printf("some dll info:\n");
  printf("dllType        = %s\n",$o_dllInfo->{dllType});
  printf("dllProduct     = %s\n",$o_dllInfo->{dllProduct});
  printf("dllProductType = %s\n",$o_dllInfo->{dllProductType});
  printf("dllEnv         = %s\n",$o_dllInfo->{dllEnv});
  printf("dllBuildDate   = %s\n",$o_dllInfo->{dllBuildDate});
  printf("dllCapiVersion = %s\n",$o_dllInfo->{dllCapiVersion});
  printf("dllBuildInfo   = %s\n",$o_dllInfo->{dllBuildInfo});
}

printf("\n\n\n");
+printf("*********************************************************\n");
printf("Build appears to be successful\n");
printf("*********************************************************\n");


#################################
#################################
###### must be last line ########
#################################
#################################

ecmdUnloadDll(); 
