#include <list>
#include <string>

#include <ecmdClientCapi.H>
#include <ecmdDataBuffer.H>
#include <ecmdUtils.H>


int main (int argc, char *argv[])
{

  ecmdDataBuffer data;          /* A buffer to store our data */
  int rc = 0;
  ecmdChipTarget target;        /* This is the chip target to operate on */


  /* Load and initialize the eCMD Dll */
  /* Which DLL to load is determined by the ECMD_DLL_FILE environment variable */
  rc = ecmdLoadDll("");
  if (rc) {
    printf("**** ERROR : Problems loading eCMD Dll\n");
    return rc;
  }

  /* Pass your arguments to the Dll so it can parse out any common args */
  /* Common args like -p# -c# will be removed from arg list upon return */
  rc = ecmdCommandArgs(&argc, &argv); 
  if (rc) return rc;

  /* Let's setup our target */
  target.cage = target.node = target.slot = 0;
  target.chipType = "pu";
  target.pos = target.core = 0;

  /* Is this target configured ? */
  if (ecmdQueryTargetConfigured(target)) {
    printf("pu 0:0 is configured\n");
  } else {
    printf("**** ERROR : pu 0:0 is not configured, unable to complete test\n");
    return 1;
  }

  /* ----------------- */
  /* Ring's            */
  /* ----------------- */
  rc = getRing (target, "sgxbs", data);
  if (rc) return rc;
  printf("Scanned ring sgxbs - length = %d\n",data.getBitLength());

  /* We need to set a few bits */
  data.setWord(1, 0xFEEDBEEF);  /* Set an entire word */
  data.setBit(2);               /* Set bit 2 */
  data.setBit(5,5);             /* Set bits 5-9 */
  data.clearBit(12);            /* Clear bit 12 */

  /* Scan the ring back in */
  rc = putRing (target, "sgxbs", data);
  if (rc) return rc;


  /* ----------------- */
  /* Spy's             */
  /* ----------------- */
  /* We will enable ring caching this will reduce the scans to the hardware */
  ecmdEnableRingCache();

  /* First we will try a non-enumerated spy */
  rc = getSpy (target, "MYSPY", data);
  if (rc) return rc;
  data.setWord(0,0xAAAAAAAA);
  rc = putSpy (target, "MYSPY", data);
  if (rc) return rc;

  /* Now an enumerated spy */
  std::string enumval;
  rc = getSpyEnum (target, "MYENUMSPY", enumval);
  if (rc) return rc;
  printf("pu 0:0 MYENUMSPY is set to : %s\n",enumval.c_str());
  rc = putSpyEnum (target, "MYENUMSPY", "ENABLE");
  if (rc) return rc;

  /* Now that we are done with that, flush all the rings to the hardware that were modified */
  rc = ecmdDisableRingCache();
  if (rc) return rc;


  /* ----------------- */
  /* Scom's            */
  /* ----------------- */
  
  rc = getScom (target, 0x800003, data);
  if (rc) return rc;
  data.setWord(1,0x5555AAAA);
  rc = putScom (target, 0x800003, data);
  if (rc) return rc;



  // -------------- 
  // Config Looping 
  // -------------- 
  // I want to loop on all the pu chips that the user selected with -p# -n#
  // Looping on selected positions only works when ecmdCommandArgs has been previously called 

  // Setup the target we will use 
  target.chipType = "pu";
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  // For the function we are doing we know that we don't care about threads 
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  bool validPosFound = false;
  // Initialize the config looper, tell it to loop on targets selected by the user -p# -c# 
  // To loop on all targets in the system, not just those selected change this to : ECMD_ALL_TARGETS_LOOP 
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP);
  if (rc) return rc;

  // This loop will continue as long as valid targets are found 
  //  each time returning with the target variable filled it 
  while ( ecmdConfigLooperNext(target) ) {

    // We will dump all the idregs 
    rc = getRing(target, "idreg", data);
    printf("pu: %d  0x%.08X\n", target.pos, data.getWord(0));

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
