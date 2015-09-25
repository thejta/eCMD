#include <list>
#include <string>
#include <stdio.h>

#include <ecmdClientCapi.H>
#include <ecmdDataBuffer.H>
#include <ecmdUtils.H>
#include <ecmdSharedUtils.H>


int main (int argc, char *argv[])
{

  // A buffer to store our data
  ecmdDataBuffer data;          
  uint32_t rc = 0;
  // This is the chip target to operate on
  ecmdChipTarget target;       


  // Load and initialize the eCMD Dll 
  // Which DLL to load is determined by the ECMD_DLL_FILE environment variable 
  rc = ecmdLoadDll("");
  if (rc) {
    printf("**** ERROR : Problems loading eCMD Dll!\n");
    return rc;
  }

  // Pass your arguments to the Dll so it can parse out any common args 
  // Common args like -p# -c# will be removed from arg list upon return 
  rc = ecmdCommandArgs(&argc, &argv); 
  if (rc) return rc;

  // Let's setup our target, here we manually target processor 0
  target.cage = target.node = target.slot = 0;
  target.chipType = "pu";
  target.pos = 0;
  // We have to tell the Dll what type of target we are querying
  // We are not dealing with chipunits here so let the Dll know we want to know everything above that
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  // Is this target configured ? 
  if (ecmdQueryTargetConfigured(target)) {
    printf("pu 0 is configured\n");
  } else {
    printf("**** ERROR : pu 0:0 is not configured, unable to complete test\n");
    return 1;
  }

  // ----------------- 
  // Ring's            
  // ----------------- 
  rc = getRing (target, "perv_vitl_chain", data);
  if (rc) return rc;
  printf("Scanned ring perv_vitl_chain - length = %d\n",data.getBitLength());

  // We need to set a few bits
  // Set an entire word
  data.setWord(1, 0xFEEDBEEF);  
  // Set bit 2
  data.setBit(2);               
  // Set bits 5-9
  data.setBit(5,5);             
  // Clear bit 12
  data.clearBit(12);            

  // Scan the ring back in 
  rc = putRing (target, "perv_vitl_chain", data);
  if (rc) return rc;


  // Let's setup our target, here we manually target processor 1 and the core chipunit number 3
  target.cage = target.node = target.slot = 0;
  target.chipType = "pu";
  target.pos = 1;
  target.chipUnitType = "core";
  target.chipUnitNum = 3;
  // We have to tell the Dll what type of target we are querying
  // We are not dealing with thread here so let the Dll know we want to know everything above that
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  // Is this target configured ? 
  if (ecmdQueryTargetConfigured(target)) {
    printf("pu 0:0 is configured\n");
  } else {
    printf("**** ERROR : pu 1: core 3 is not configured, unable to complete test\n");
    return 1;
  }

  // ----------------- 
  // Spy's             
  // ----------------- 
  // We will enable ring caching this will reduce the scans to the hardware 
  ecmdEnableRingCache(target);

  // First we will try a non-enumerated spy 
  rc = getSpy (target, "MYSPY", data);
  if (rc) return rc;
  data.setWord(0,0xAAAAAAAA);
  rc = putSpy (target, "MYSPY", data);
  if (rc) return rc;

  // Now an enumerated spy 
  std::string enumval;
  rc = getSpyEnum (target, "MYENUMSPY", enumval);
  if (rc) return rc;
  printf("pu 0:0 MYENUMSPY is set to : %s\n",enumval.c_str());
  rc = putSpyEnum (target, "MYENUMSPY", "ENABLE");
  if (rc) return rc;

  // Now that we are done with that, flush all the rings to the hardware that were modified 
  rc = ecmdDisableRingCache(target);
  if (rc) return rc;

  // ----------------- 
  // Scom's            
  // ----------------- 
  
  rc = getScom (target, 0x800003, data);
  if (rc) return rc;
  printf("pu 0:0 800003 %.08X %.08X\n",data.getWord(0),data.getWord(1));
  data.setWord(1,0x5555AAAA);
  rc = putScom (target, 0x800003, data);
  if (rc) return rc;


  // -------------- 
  // Config Looping 
  // -------------- 
  // I want to loop on all the pu chips that the user selected with -p# -n#
  // Looping on selected positions only works when ecmdCommandArgs has been previously called 

  // Setup the target we will use
  // We want to loop on all 'pu' chips and the ex chiplets so set that, everything else is wildcard
  target.chipType = "pu";
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitType = "ex";
  target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
  // For the function we are doing we know that we don't care about threads 
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  bool validPosFound = false;
  ecmdLooperData looperdata;
  // Initialize the config looper, tell it to loop on targets selected by the user -p# -c# 
  // To loop on all targets in the system, not just those selected change this to : ECMD_ALL_TARGETS_LOOP 
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  // This loop will continue as long as valid targets are found 
  //  each time returning with the target variable filled it 
  while ( ecmdConfigLooperNext(target, looperdata) ) {

    // We will dump the ring
    rc = getRing(target, "ex_func_core", data);
    printf("First word of ex_func_core for %s : 0x%.08X\n", ecmdWriteTarget(target).c_str(), data.getWord(0));

    // Signify that we looped at least once 
    validPosFound = true;
  }
  if (!validPosFound) {
    // We never went into the while loop this means the positions the user selected where not in the system 
    printf("**** ERROR : Position selected was not valid\n");
  }


  // Unload the eCMD Dll, this should always be the last thing you do 
  ecmdUnloadDll();

  return rc;

}
