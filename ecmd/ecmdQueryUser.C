// Copyright ***********************************************************
//                                                                      
// File ecmdQueryUser.C                                   
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 1996                                         
//                                                                      
// The source code for this program is not published or otherwise       
// divested of its trade secrets, irrespective of what has been         
// deposited with the U.S. Copyright Office.                            
//                                                                      
// End Copyright *******************************************************
/* $Header$ */

// Module Description **************************************************
//
// Description: 
//
// End Module Description **********************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define ecmdQueryUser_C
#include <stdio.h>

#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>

#undef ecmdQueryUser_C
//----------------------------------------------------------------------
//  User Types
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Macros
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Internal Function Prototypes
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------


int ecmdQueryUser(int argc, char* argv[]) {
  int rc = ECMD_SUCCESS;
  std::string printed;


  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc == 0) {
    ecmdOutputError("ecmdquery - Too few arguments specified; you need at least a query mode.\n");
    ecmdOutputError("ecmdquery - Type 'ecmdquery -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  if (!strcmp(argv[0], "rings")) {

    char invmask = 'N';
    char chkable  = 'N';
    char broadmode = 'N';
    std::list<ecmdRingData> ringdata;
    std::list<ecmdRingData>::iterator ringit;
    std::list<std::string>::iterator strit;

    if (argc < 2) {
      ecmdOutputError("ecmdquery - Too few arguments specified for rings; you need at least a query rings <chipname>.\n");
      ecmdOutputError("ecmdquery - Type 'ecmdquery -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }

    ecmdChipData chipdata;

    //Setup the target that will be used to query the system config 
    ecmdChipTarget target;
    target.chipType = argv[1];
    target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_QUERY_WILDCARD;
    target.threadState = target.coreState = ECMD_TARGET_FIELD_UNUSED;

    /************************************************************************/
    /* Kickoff Looping Stuff                                                */
    /************************************************************************/

    bool validPosFound = false;
    rc = ecmdConfigLooperInit(target);
    if (rc) return rc;

    char buf[200];

    while ( ecmdConfigLooperNext(target) ) {

      rc = ecmdQueryRing(target, ringdata,argv[2]);
      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
      } else if (rc) {
        printed = "ecmdquery - Error occured performing ring query on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }
      else {
        validPosFound = true;     
      }

      /* Let's look up other info about the chip, namely the ec level */
      rc = ecmdGetChipData (target, chipdata);
      if (rc) {
        printed = "ecmdquery - Unable to lookup ec information for chip ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }
        
      sprintf(buf,"\nAvailable rings for %s ec %d:\n", ecmdWriteTarget(target).c_str(), chipdata.chipEc); ecmdOutput(buf);
      printed = "Ring Names                           Address    Length   Mask Chkable BroadSide ClockState\n"; ecmdOutput(printed.c_str());
      printed = "-----------------------------------  --------   ------   ---- ------- --------- ----------\n"; ecmdOutput(printed.c_str());

      for (ringit = ringdata.begin(); ringit != ringdata.end(); ringit ++) {

        printed = "";
        /* The Ring Names */
        for (strit = ringit->ringNames.begin(); strit != ringit->ringNames.end(); strit ++) {
          if (strit != ringit->ringNames.begin()) printed += ", ";
          printed += (*strit);
        }
        for (int i = printed.length(); i <= 36; i++) { 
          printed += " ";
        }

        if(ringit->hasInversionMask) {
          invmask = 'Y';
        } else {
          invmask = 'N';
        }

        if (ringit->isCheckable) {
          chkable = 'Y';
        } else chkable = 'N';

        if (ringit->supportsBroadsideLoad) {
          broadmode = 'Y';
        } else broadmode = 'N';

        sprintf(buf,"0x%.6X\t%d\t  %c     %c         %c     ", ringit->address, ringit->bitLength, invmask, chkable, broadmode);
        printed += buf;

        if (ringit->clockState == ECMD_CLOCKSTATE_UNKNOWN)
          printed += "UNKNOWN\n";
        else if (ringit->clockState == ECMD_CLOCKSTATE_ON)
          printed += "ON\n";
        else if (ringit->clockState == ECMD_CLOCKSTATE_OFF)
          printed += "OFF\n";
        else if (ringit->clockState == ECMD_CLOCKSTATE_NA)
          printed += "NA\n";

        ecmdOutput(printed.c_str());
      }
    }

    if (!validPosFound) {
      ecmdOutputError("ecmdquery - Unable to find a valid chip to execute command on\n");
      return ECMD_TARGET_NOT_CONFIGURED;
    }

  } else if (!strcmp(argv[0],"version")) {

    /* Let's display the dllInfo to the user */
    ecmdDllInfo info;
    rc = ecmdQueryDllInfo(info);
    if (rc) {
      ecmdOutputError("ecmdquery - Problems occurred trying to get Dll Info\n");
      return rc;
    }
    ecmdOutput("================================================\n");
    printed = "Dll Type         : ";
    if (info.dllType == ECMD_DLL_STUB)
      printed += "Stub\n";
    else if (info.dllType == ECMD_DLL_STUB)
      printed += "Stub\n";
    else if (info.dllType == ECMD_DLL_CRONUS)
      printed += "Cronus\n";
    else if (info.dllType == ECMD_DLL_IPSERIES)
      printed += "IP-Series\n";
    else if (info.dllType == ECMD_DLL_ZSERIES)
      printed += "Z-Series\n";
    else 
      printed = "Unknown\n";
    ecmdOutput(printed.c_str());

    printed = "Dll Product      : ";
    if (info.dllProduct == ECMD_DLL_PRODUCT_ECLIPZ)
      printed += "Eclipz\n";
    else
      printed += "Unknown\n";
    ecmdOutput(printed.c_str());

    printed = "Dll Environment  : ";
    if (info.dllEnv == ECMD_DLL_ENV_HW)
      printed += "Hardware\n";
    else
      printed += "Simulation\n";
    ecmdOutput(printed.c_str());

    printed = "Dll Build Date   : "; printed += info.dllBuildDate; printed += "\n"; ecmdOutput(printed.c_str());
    printed = "Dll Capi Version : "; printed += info.dllCapiVersion; printed += "\n"; ecmdOutput(printed.c_str());
    ecmdOutput("================================================\n");

    

  } else {
    /* Invalid Query Mode */
    ecmdOutputError("ecmdquery - Invalid Query Mode.\n");
    ecmdOutputError("ecmdquery - Type 'ecmdquery -h' for usage.\n");
    return ECMD_INVALID_ARGS;
    

  }


  return rc;
}






// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
