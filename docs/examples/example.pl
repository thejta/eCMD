#!/bin/ksh 
#! -*- perl -*-

eval '
exec $ECMDPERLBIN -x -S $0 ${1+"$@"}
'
if 0;

use strict;
use ecmd;

my $rc = 0;

$| = 1;  # set autoflush

# Load the eCMD PerlModule,
#  first parm is a pointer to the plugin, "" says use environment which is suggested
#  second parm is a comma seperated list of eCMD Major version supported by this script, ie "ver4,ver5"
if (ecmdLoadDll("","ver5,ver6,ver7")) { die "Fatal errors initializing DLL"; }

#####
#
# End of Standard header section.
#
#####

# Pull the common command args
$rc = ecmdCommandArgs(\@ARGV);


if (0) {
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
  print "*\n";
  print "*\n";
  print "*\n";
}


 # put/getring
if (0) {

  print "---- starting putring -----\n";
  my $edb1 = new ecmd::ecmdDataBuffer();
  my $target = new ecmd::ecmdChipTarget();
  # Let's setup our target, two ways to do it
  $target->{chipType} = "pu";
  $target->{cage} = 0;
  $target->{node} = 0;
  $target->{slot} = 0;
  $target->{pos}  = 0;
  $target->{chipUnitNum} = 0;

  print "---- starting getRing -----\n";
  $rc = getRing($target, "vital_func", $edb1);

  printf("getRing word 0 = 0x%X\n", $edb1->getWord(0));
  printf("actual length = %s\n", $edb1->getBitLength());
  printf("Hex data = %s\n", $edb1->genHexLeftStr());

#  $edb1->setBitLength(2078); #length of vital_func
  $edb1->flushTo0();
  $edb1->setBit(0, 15); # Set bits 0-10
  #    $rc = ecmdDisableSafeMode();

  printf("putRing word 0 = 0x%X\n", $edb1->getWord(0));
  $rc = putRing($target, "vital_func", $edb1);
  #    $rc = putRing($$target, "vital_func", "FEEDBEEF");
  if ($rc) {
    print "Problems calling eCMD putring\n";
  }
  $edb1->flushTo0();
  print "---- starting getRing -----\n";
  $rc = getRing($target, "vital_func", $edb1);

  printf("getRing word 0 = 0x%X\n", $edb1->getWord(0));
  printf("actual length = %s\n", $edb1->getBitLength());
  printf("Hex data = %s\n", $edb1->genHexLeftStr());
  print "*\n";
  print "*\n";
  print "*\n";
  print "*\n";
  print "*\n";

}


 # put/getscom
if (0) {
  my $edb1 = new ecmd::ecmdDataBuffer();
  my $target = new ecmd::ecmdChipTarget();
  # Let's setup our target, two ways to do it
  $target->{chipType} = "pu";
  $target->{cage} = 0;
  $target->{node} = 0;
  $target->{slot} = 0;
  $target->{pos}  = 0;
  $target->{chipUnitNum} = 0;

  print "---- starting getscom -----\n";
  $rc = getScom($target, "0x5002C0", $edb1);

  printf("getscom word 0 = 0x%X\n", $edb1->getWord(0));
  printf("Hex data = %s\n", $edb1->genHexLeftStr());
  printf("rc = %08x*\n",$rc);
  print "*\n";
  print "*\n";
  print "---- starting second getscom -----\n";
  $rc = getScom($target, 0x5002C0, $edb1);

  printf("getscom word 0 = 0x%X\n", $edb1->getWord(0));
  printf("Hex data = %s\n", $edb1->genHexLeftStr());
  printf("rc = %08x*\n",$rc);
  print "*\n";
  print "---- starting third getscom -----\n";
  $rc = getScom($target, "1x5002C0", $edb1);

  printf("getscom word 0 = 0x%X\n", $edb1->getWord(0));
  printf("Hex data = %s\n", $edb1->genHexLeftStr());
  printf("rc = %08x*\n",$rc);
  print "*\n";
  print "---- starting forth getscom -----\n";
  $rc = getScom($target, "5002c0", $edb1);

  printf("getscom word 0 = 0x%X\n", $edb1->getWord(0));
  printf("Hex data = %s\n", $edb1->genHexLeftStr());
  printf("rc = %08x*\n",$rc);
  print "*\n";

}



 # put/getArray
if (0) {

  my $target = new ecmd::ecmdChipTarget();
  # Let's setup our target, two ways to do it
  $target->{chipType} = "pu";
  $target->{cage} = 0;
  $target->{node} = 0;
  $target->{slot} = 0;
  $target->{pos}  = 0;
  $target->{chipUnitNum} = 0;

  my $data    = new ecmd::ecmdDataBuffer();
  my $address = new ecmd::ecmdDataBuffer();

  $data->setBitLength(48);
  $data->flushTo0();
  $address->setBitLength(11); #addresses for arrays are always 11 bits
  $address->flushTo0();
  print "---- starting putArray -----\n";
  print "The array is L20_SLCA_DIR_0 \n";
  $data->setWord(0, 0xabebfeef);
  #    $data->setWord(1, 0);
  printf("Hex Data = %s\n",$data->genHexLeftStr()); #array is 48

  $rc = putArray($target, "L20_SLCA_DIR_0", $address, $data);
  printf("Hex Data = %s\n",$data->genHexLeftStr(0,48));
  printf("putArray RC = %08x\n",$rc);
  printf("Hex data = %s\n",$data->genHexLeftStr());

  print "---- starting getArray -----\n";
  $rc = getArray($target, "L20_SLCA_DIR_0", $address, $data);
  printf("word count= %d\n",$data->getWordLength());
  printf("Hex data = %s\n",$data->genHexLeftStr());

  print "*\n";
  print "*\n";
  print "*\n";
  print "*\n";
  print "*\n";
}

 # put/getspy
if (0) {
  my $target = new ecmd::ecmdChipTarget();
  # Let's setup our target, two ways to do it
  $target->{chipType} = "pu";
  $target->{cage} = 0;
  $target->{node} = 0;
  $target->{slot} = 0;
  $target->{pos}  = 0;
  $target->{chipUnitNum} = 0;

  my $data = new ecmd::ecmdDataBuffer();

  $data->setBitLength(32); #default
  $data->flushTo0();
  print "---- starting putspy -----\n";

  print "The spy in question tpcfam.tpvlogic.tp_scani.frequency_meas\n";
  print "it's 12 bits long\n";
  print "\n";


  $data->setWord(0, 0x12300000);
  printf("word0 = %s\n", $data->getWord(0));
  printf("Hex data = %s\n",$data->genHexLeftStr());

  printf("Let's make an error by writing to the nonexistant potato spy\n",$rc);
  $rc = ecmdDisablePerlSafeMode();
#  $rc = ecmdEnablePerlSafeMode();
  $rc = putSpy($target, "potato", $data);
  printf("rc decimal = %d\n",$rc);
  printf("rc hex     = %08X\n",$rc);


  $rc = putSpy($target, "tpcfam.tpvlogic.tp_scani.frequency_meas", $data);

  print "---- starting getspy -----\n";
  $rc = getSpy($target, "tpcfam.tpvlogic.tp_scani.frequency_meas", $data);
  printf("word count= %d\n",$data->getWordLength());
  printf("getspy hexLeftStr = %s\n", $data->genHexLeftStr());
  print "*\n";
  print "*\n";
  print "*\n";
  print "*\n";
  print "*\n";
}

 # put/getlatch
if (0) {
  my $target = new ecmd::ecmdChipTarget();
  # Let's setup our target, two ways to do it
  $target->{chipType} = "pu";
  $target->{cage} = 0;
  $target->{node} = 0;
  $target->{slot} = 0;
  $target->{pos}  = 0;
  $target->{chipUnitNum} = 0;

  my $matches =0;
  my $edb1 = new ecmd::ecmdDataBuffer();

  print "---- starting putlatch -----\n";
  $edb1->setBitLength(12); #default
  $edb1->flushTo0();
  $edb1->setBit(0, 2); # Set bits 0-10
  $rc = putLatch($target, "vital_func", "TPCFAM.TPVLOGIC.TPPSC.FUSE_LIMIT.Z.L2", $edb1, 0, 12, \$matches, ECMD_LATCHMODE_FULL);
  printf("matches = %d\n",$matches);


  my $latchList      = new ecmd::listEcmdLatchEntry();
  my $latchEntryIter = new ecmd::listEcmdLatchEntryIterator();
  my $entry          = new ecmd::ecmdLatchEntry;

  print "---- starting getlatch -----\n";
  printf("Latch TPCFAM.TPVLOGIC.TPPSC.FUSE_LIMIT.Z.L2\n");
  $rc = getLatch($target, "vital_func", "TPCFAM.TPVLOGIC.TPPSC.FUSE_LIMIT.Z.L2",  $latchList, ECMD_LATCHMODE_FULL);
  if($rc) {
    printf("rc = ep->getLatch(target, \"vital_func\", \"TPCFAM.TPVLOGIC.TPPSC.FUSE_LIMIT.Z.L2\",  latchList, 0, width);\n");
    printf("rc = %08X  msg= %s\n",$rc,ecmdGetErrorMsg($rc));
    exit;
  }
#
#struct ecmdLatchEntry {
#  std::string    latchName;             ///< Latch name of entry
#  std::string    ringName;              ///< Ring that latch came from
#  ecmdDataBuffer buffer;                ///< Latch data
#  int            latchStartBit;         ///< Start bit of data inside latch
#  int            latchEndBit;           ///< End bit of data inside latch
#  uint32_t       rc;                    ///< Error code in retrieving this entry
#};

  $latchEntryIter->setIter($latchList->begin());
  while($latchEntryIter != $latchList->end()) {
    $entry = $latchEntryIter->getValue();
    printf(" name    : %s\n", $entry->{latchName});
    printf(" ringname: %s\n", $entry->{ringName});
    printf(" buffer  : %s\n", $entry->{buffer}->genHexLeftStr());
    printf(" start   : %s\n", $entry->{latchStartBit});
    printf(" stop    : %s\n", $entry->{latchEndBit});

    $latchEntryIter++;
  }

  print "*\n";
  print "*\n";
}



 # put/getspyenum
if (0) {
  my $target = new ecmd::ecmdChipTarget();
  # Let's setup our target, two ways to do it
  $target->{chipType} = "pu";
  $target->{cage} = 0;
  $target->{node} = 0;
  $target->{slot} = 0;
  $target->{pos}  = 0;
  $target->{chipUnitNum} = 0;

  my $data = new ecmd::ecmdDataBuffer();

  $data->setBitLength(32); #default
  $data->flushTo0();
  print "---- starting putSpyEnum -----\n";

  print "the spy in question TPCFAM.TPVLOGIC.TPPSC.SCOM_HANG_LIMIT\n";
  print "the enum is 16000_CYCLES\n";

  my $spyEnum = "16000_CYCLES";

  $rc = putSpyEnum($target, "TPCFAM.TPVLOGIC.TPPSC.SCOM_HANG_LIMIT", $spyEnum );

  print "---- starting getspyenum -----\n";

  $spyEnum = "garbage going in";

  printf("Here is the enum string before getSpyEnum: %s\n",$spyEnum);
  $rc = getSpyEnum($target, "TPCFAM.TPVLOGIC.TPPSC.SCOM_HANG_LIMIT", $spyEnum );
  printf("Here is the enum string: %s\n",$spyEnum);
  print "*\n";
  print "*\n";
  print "*\n";
  print "*\n";
  print "*\n";
}



 # simPUTFAC/simGETFAC
if (0) {
  my $data = new ecmd::ecmdDataBuffer();
  $data->setBitLength(32); #default
  $data->flushTo0();
  print "---- starting simPUTFAC -----\n";

  print "the fac in question P0.FSPE.SPC.SP.CMPS.TCKFCRL2\n";
  print "it's 32 bits long\n";
  print "\n";

  $data->setWord(0, 0x12300000);
  printf("word0    = %s\n", $data->getWord(0));
  printf("Hex Data = %s\n",$data->genHexLeftStr());

  $rc = simPUTFAC("P0.FSPE.SPC.SP.CMPS.TCKFCRL2",32,$data,0,0);
  printf("Hex Data after simPUTFAC= %s\n",$data->genHexLeftStr());

  printf("---- starting simclock -----\n");
  $rc = simclock(10);

  printf("---- starting simecho -----\n");
  $rc = simecho("I just clocked it by 10\n");

  printf("---- starting simGETFAC -----\n");
  $data->setBitLength(32); #default
  $data->flushTo0();
  $rc = simGETFAC("P0.FSPE.SPC.SP.CMPS.TCKFCRL2",32,$data,0,0);
  printf("just after simGETFAC\n");
  printf("word count= %d\n",$data->getWordLength());
  printf("simGETFAC hexLeftStr = %s\n", $data->genHexLeftStr(0,$data->getBitLength()));
  print "*\n";
  print "*\n";
  print "*\n";
}



 # simEXPECTFAC/simGETFAC
if (0) {


  print "---- starting simEXPECTFAC -----\n";
  my $data = new ecmd::ecmdDataBuffer();
  $data->setBitLength(32); #default
  $data->flushTo0();

  print "the fac in question P0.FSPE.SPC.SP.CMPS.TCKFCRL2\n";
  print "it's 32 bits long\n";
  print "\n";

  $data->setWord(0, 0x12300000);
  printf("Hex data = %s\n",$data->genHexLeftStr());

  $rc = simPUTFAC("P0.FSPE.SPC.SP.CMPS.TCKFCRL2",32,$data,0,0);

  printf("---- starting simclock -----\n");
  $rc = simclock(10);

  printf("---- starting simecho -----\n");
  $rc = simecho("I just clocked it by 10\n");


  printf("---- starting simgetcurrentcycle -----\n");
  my $o_cyclecount =0;
  $rc = simgetcurrentcycle(\$o_cyclecount);
  printf("simgetcurrentcycle = %d\n",$o_cyclecount);

  printf("---- starting simGETFAC -----\n");
  $data->flushTo0();
  $rc = simEXPECTFAC("P0.FSPE.SPC.SP.CMPS.TCKFCRL2",32,$data,0,0);
  printf("word count= %d\n",$data->getWordLength());
  printf("rc was = %d\n",$rc);
  $rc = simEXPECTFAC("P0.FSPE.SPC.SP.CMPS.TCKFCRL2",32,$data,0);
  printf("word count= %d\n",$data->getWordLength());
  printf("rc was = %d\n",$rc);
  $rc = simEXPECTFAC("P0.FSPE.SPC.SP.CMPS.TCKFCRL2",32,$data);
  printf("word count= %d\n",$data->getWordLength());
  printf("rc was = %d\n",$rc);
  printf("simGETFAC hexLeftStr = %s\n", $data->genHexLeftStr());

#uint32_t simexpecttcfac(const char* i_tcfacname, uint32_t i_bitlength, ecmdDataBuffer & i_expect, uint32_t i_row = 0);
  $rc = simexpecttcfac("potato", 32, $data, 0);

  printf("word count= %d\n",$data->getWordLength());
  printf("rc was = %d\n",$rc);
  printf("simExpectTcFac hexLeftStr = %s\n", $data->genHexLeftStr());

  $rc = simexpecttcfac("potato", 32, $data);

  printf("word count= %d\n",$data->getWordLength());
  printf("rc was = %d\n",$rc);
  printf("simExpectTcFac hexLeftStr = %s\n", $data->genHexLeftStr());

  print "*\n";
  print "*\n";
  print "*\n";
}



 # put/getmemproc
if (0) {

#uint32_t getMemProc (ecmdChipTarget & i_target, uint64_t i_address, uint32_t i_bytes, ecmdDataBuffer & o_data);

  my $edb1 = new ecmd::ecmdDataBuffer();
  my $target = new ecmd::ecmdChipTarget();
  my $address64 = "0x8000000000000000";
  my $bytes     = 0;

  # Let's setup our target, two ways to do it
  $target->{chipType} = "pu";
  $target->{cage} = 0;
  $target->{node} = 0;
  $target->{slot} = 0;
  $target->{pos}  = 0;
  $target->{chipUnitNum} = 0;

  print "---- starting putMemProc -----\n";
  $edb1->setBitLength(32); #default
  $edb1->flushTo0();
  $edb1->setBit(0, 15); # Set bits 0-10
  $rc = putMemProc($target, $address64, $bytes, $edb1);

  print "---- starting getMemProc -----\n";
  #uint32_t getMemProc (ecmdChipTarget & i_target, uint64_t i_address, uint32_t i_bytes, ecmdDataBuffer & o_data);
  $edb1->flushTo0();
  $rc = getMemProc($target, $address64, $bytes, $edb1);

  printf("actual length = %s\n", $edb1->getBitLength());
  printf("Hex data = %s\n", $edb1->genHexLeftStr(0, $edb1->getBitLength()));

#uint32_t putMemProc (ecmdChipTarget & i_target, uint64_t i_address, uint32_t i_bytes, ecmdDataBuffer & i_data);
  print "*\n";
  print "*\n";
  print "*\n";

}


# getTraceArray
if (0) {

  #uint32_t getTraceArray(ecmdChipTarget & i_target, const char* i_name, std::vector <ecmdDataBuffer> & o_data);

  my $edb1 = new ecmd::vectorEcmdDataBuffer();
  my $target = new ecmd::ecmdChipTarget();
  my $name = "potato";

  # Let's setup our target, two ways to do it
  $target->{chipType} = "pu";
  $target->{cage} = 0;
  $target->{node} = 0;
  $target->{slot} = 0;
  $target->{pos}  = 0;
  $target->{chipUnitNum} = 0;

  print "---- starting getTraceArray -----\n";
  $rc = getTraceArray($target, $name, $edb1);

  for (my $x = 0; $x < $edb1->size(); $x++) {
    printf("Vector Entry %d: Hex Data: %s", $x, $edb1->get($x)->genHexLeftStr());
  }

#uint32_t getTraceArrayMultiple(ecmdChipTarget & i_target, std::list <ecmdNameVectorEntry> & o_data);
  my $listVE = new ecmd::listEcmdNameVectorEntry;
  print "*\n";
  print "*\n";
  print "*\n";

}



# ConfigLooper
if (1) {
  my $target = new ecmd::ecmdChipTarget();
  my $qData = new ecmd::ecmdQueryData();

  my $looperdata = new ecmd::ecmdLooperData;     #< Store internal Looper data

  # Let's setup our target, two ways to do it
  $target->{chipType} = "pu";
  $target->{cage} = 0;
  $target->{node} = 0;
  $target->{slot} = 0;
  $target->{pos}  = 0;
  $target->{chipUnitNum} = 0;
  $target->{cageState} = ECMD_TARGET_FIELD_WILDCARD;
  $target->{nodeState} = ECMD_TARGET_FIELD_WILDCARD;
  $target->{slotState} = ECMD_TARGET_FIELD_WILDCARD;
  $target->{posState}  = ECMD_TARGET_FIELD_WILDCARD;
  $target->{chipUnitState} = ECMD_TARGET_FIELD_WILDCARD;
  $target->{threadState} = ECMD_TARGET_FIELD_UNUSED;

  $target->{chipTypeState} = ECMD_TARGET_FIELD_VALID;

#  my $configName = "SCAN_SAFE_MODE";
  my $configName = "SIM_BROADSIDE_MODE";
  my $validOutput;
  my $valueAlpha;
  my $valueNumeric =0;
  my $printed = "";

  print "---- starting ecmdConfigLooperInit -----\n";
  $rc = ecmdConfigLooperInit($target, ECMD_SELECTED_TARGETS_LOOP, $looperdata);
  print "got here\n";

  while ( ecmdConfigLooperNext($target, $looperdata) ) {
    print "got here2\n";


    $rc = ecmdGetConfiguration($target, $configName, \$validOutput, $valueAlpha, \$valueNumeric);
    print "got here3\n";
    print "$validOutput\n";
    print "configName = $configName\n";
    print "valueAlpha = $valueAlpha\n";

    if(($validOutput == ECMD_CONFIG_VALID_FIELD_ALPHA) || ($validOutput ==  ECMD_CONFIG_VALID_FIELD_BOTH)  ) {
     $printed = $configName . " = " . $valueAlpha . "\n";
     ecmdOutput($printed);
    }
  }


}




#################################
#################################
###### must be last line ########
#################################
#################################

ecmdUnloadDll(); 
