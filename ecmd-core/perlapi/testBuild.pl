#!/bin/sh 
#! -*- perl -*-

eval '
exec $ECMDPERLBIN -x -S $0 ${1+"$@"}
'
if 0;

use strict;
use ecmd; # Must be a use, not a require, for enum support to work

my $rc = 0;

$| = 1;  # set autoflush

if (ecmdLoadDll("","ver5,ver6,ver7,ver8,ver9,ver10,ver11,ver12,ver13,ver14,ver15")) { exit(0); }

$rc = ecmdCommandArgs(\@ARGV);

# Test the data buffer
my $data1 = new ecmd::ecmdDataBuffer(8);
my $data2 = new ecmd::ecmdDataBuffer(8);
my $data3 = new ecmd::ecmdDataBuffer(8);

$data1->setByte(0,0xF8);
printf("data1 = %s\n", $data1->genHexLeftStr());
$data2->setByte(0,0x1F);
printf("data2 = %s\n", $data2->genHexLeftStr());

$data3 = $data1 & $data2;

if ($data3->genHexLeftStr() eq "18") {
  printf("Quick Databuffer check PASSED! - %s\n", $data3->genHexLeftStr());
} else {
  printf("Quick Databuffer check FAILED! - %s\n", $data1->genHexLeftStr());
  exit(1);
}

my $data4 = new ecmd::ecmdDataBuffer(64);
my $val=736;
#$val=0x80000;
$data4->insertFromRight($val,0,28);

my $o_dllInfo = new ecmd::ecmdDllInfo;

# Test loading the plugin
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

printf("\n");
printf("*********************************************************\n");
printf("Build appears to be successful\n");
printf("*********************************************************\n");


#################################
#################################
###### must be last line ########
#################################
#################################

ecmdUnloadDll(); 
