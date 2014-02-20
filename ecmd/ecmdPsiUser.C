/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File ecmdCommandUtils.C                                   
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
#include <stdio.h>
#include <string.h>

#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <ecmdInterpreter.H>
#include <ecmdSharedUtils.H>

#ifndef ECMD_REMOVE_ADAL_FUNCTIONS
uint32_t ecmdAdalPsiUser(int argc, char* argv[]) {

  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;
  
  std::string printed;                  //< Output data
  ecmdChipTarget target;
  ecmdLooperData looperdata;            //< Store internal Looper data
  std::string action;
  bool validPosFound = false;           //< Did we find a valid chip in the looper

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc == 0) {
    ecmdOutputError("psi - Too few arguments, Type 'psi -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  /* First arg is the operation*/
  if (!strcmp(argv[0],"init")) {
    action = argv[0];
  } else if (!strcmp(argv[0],"enable")) {
    action = argv[0];
  } else if (!strcmp(argv[0],"verify")) {
    action = argv[0];
  } else if (!strcmp(argv[0],"setspeed")) {
    action = argv[0];
  } else {
    ecmdOutputError("psi - Invalid psi operation specified.\n");
    ecmdOutputError("psi - Type 'psi -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }  

  if (argc < 3 ) { //check if chipselect and port are provided
    ecmdOutputError("psi - Too few arguments specified; you need a chipselect and a port.\n");
    ecmdOutputError("psi - Type 'psi -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  target.chipType = argv[1];
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  uint32_t l_psiPort  = atoi(argv[2]);


  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  
  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

      if (action == "init") {
        rc = ecmdAdalPsiInit(target,l_psiPort);
	if(rc == ECMD_SUCCESS)
	{
	  printed = action + " success\n";
	  ecmdOutput(printed.c_str());
	}
      } 
      else if (action == "enable"){
        if(argc < 4) //check the input enableState
        {
          ecmdOutputError("psi - Too few arguments specified; you need enable state.\n"); 
	  ecmdOutputError("psi - Type 'psi -h' for usage.\n"); 
	  return ECMD_INVALID_ARGS; 
	}
	uint32_t l_enableState = atoi(argv[3]);
        rc = ecmdAdalPsiLinkEnable(target,l_psiPort,l_enableState);
	if(rc == ECMD_SUCCESS)
	{
	  printed = action + " success\n";
	  ecmdOutput(printed.c_str());
	}
      } 
      else if (action == "verify") {
        uint32_t l_outputState = 0;
        rc =  ecmdAdalPsiLinkVerify(target,l_psiPort,l_outputState);
	printed = "PSI link ";
	printed += argv[2];
	printed += " is ";
	if(rc == ECMD_SUCCESS)
	{
	  switch(l_outputState)
	  {
	    case 0x00:
	    	printed += " in INVALID state";
		break;
	    case 0x01:
	    	printed += " DISABLED and UNCONFIGURED";
		break;
	    case 0x02:
	    	printed += " STANDBY";
		break;
	    case 0x04:
	    	printed += " ACTIVE";
		break;
	    case 0x08:
	    	printed += " DISABLED and in FAILED state";
		break;
	  }
	  printed += "\n";
	  ecmdOutput(printed.c_str());
	}
      }
      else if (action == "setspeed") {
        if(argc < 5)
        {
          ecmdOutputError("psi - Too few arguments specified; you need speed and mode.\n"); 
	  ecmdOutputError("psi - Type 'psi -h' for usage.\n"); 
	  return ECMD_INVALID_ARGS; 
	}

	uint32_t l_speed = atoi(argv[3]);
	uint32_t l_mode  = atoi(argv[4]);
        
        rc = ecmdAdalPsiSetSpeed(target,l_psiPort,l_speed,l_mode);
	if(rc == ECMD_SUCCESS)
	{
	  printed = action + " success\n";
	  ecmdOutput(printed.c_str());
	}
      }
      else
      {
        //already checked for invalid operation
      }

    if (rc) {
      printed = "psi - operation " + action + " failed on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError(printed.c_str());
      coeRc = rc;
    }
    else {
      validPosFound = true;     
    }
  } //end - configLooper 

  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("psi - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}
#endif // ECMD_REMOVE_ADAL_FUNCTIONS

