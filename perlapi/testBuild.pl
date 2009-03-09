#!/bin/sh 
#! -*- perl -*-

# Couldn't use $CC_VER here for some reason, so just hardocded to 3.4.6
eval '
if [ "X$ECMDPERLBIN" = "X" ]; then
 if [ "X$CTEPATH" = "X" ]; then echo "CTEPATH env var is not set."; exit 1; fi
 export ECMDPERLBIN=$CTEPATH/tools/perl/5.8.1/bin/perl;
 export CTEPERLPATH=$CTEPATH/tools/perl/5.8.1;
 if [[ `uname` = "Linux" ]]; then
  export CTEPERLLIB=$CTEPERLLIB:$CTEPERLPATH/lib/5.8.1:./obj_x86/;
  export LD_LIBRARY_PATH="../lib/x86/3.4.6/:../capi/obj_x86/";
 else
  export CTEPERLLIB=$CTEPERLLIB:$CTEPERLPATH/lib/5.8.1:./obj_aix/;
  export LIBPATH="../capi/obj_x86";
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

my $data1 = new ecmd::ecmdDataBuffer(8);
my $data2 = new ecmd::ecmdDataBuffer(8);
my $data3 = new ecmd::ecmdDataBuffer(8);

$data1->setByte(0,0xF8);
$data2->setByte(0,0x1F);

$data3 = $data1 & $data2;

printf("%s\n", $data3->genHexLeftStr(0,8));

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
