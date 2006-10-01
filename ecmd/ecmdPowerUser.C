/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File ecmdPowerUser.C                                  
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 2003                                         
//                                                                      
// The source code for this program is not published or otherwise       
// divested of its trade secrets, irrespective of what has been         
// deposited with the U.S. Copyright Office.                            
//                                                                      
// End Copyright *******************************************************

// Module Description **************************************************
//
// Description: 
//
// End Module Description **********************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <stdio.h>
#include <fstream>
#include <algorithm>

#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <ecmdInterpreter.H>
#include <ecmdSharedUtils.H>

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

uint32_t ecmdSystemPowerUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc < 1) {
    ecmdOutputError("systempower - At least one argument ('on', 'off') is required for systempower.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 1) {
    ecmdOutputError("systempower - Too many arguments to systempower, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }
  
  if (!strcmp(argv[0], "on")) {
    ecmdOutput("Powering on the system ...\n");
    rc = ecmdSystemPowerOn();
  } else if (!strcmp(argv[0], "off")) {
    ecmdOutput("Powering off the system ...\n");
    rc = ecmdSystemPowerOff();
  } else {
    ecmdOutputError("systempower - Invalid argument passed to systempower. Accepted arguments: ('on', 'off').\n");
    return ECMD_INVALID_ARGS;
  }

  return rc;

}

uint32_t ecmdFruPowerUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdChipTarget target;                ///< Current target operating on
  bool validPosFound = false;           ///< Did the looper find anything to execute on
  std::string printed;
  std::string state;
  bool smart = false;

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  /* get the smart flag, if it's there */
  smart = ecmdParseOption(&argc, &argv, "-smart");

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc < 2) {
    ecmdOutputError("frupower - At least one argument ('on', 'off') is required for frupower.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 2) {
    ecmdOutputError("frupower - Too many arguments to frupower, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  if (!strcmp(argv[0], "on")) {
    state = "on";
  } else if (!strcmp(argv[0], "off")) {
    state = "off";
  } else {
    ecmdOutputError("frupower - Invalid argument passed to frupower. Accepted arguments: ('on', 'off').\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used 
  target.chipType = argv[1];
  if (target.chipType == "nochip") {
    target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
    target.posState = ECMD_TARGET_FIELD_UNUSED;
  } else {
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    target.posState = ECMD_TARGET_FIELD_WILDCARD;
  }
  target.cageState = target.nodeState = target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;


  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while ( ecmdConfigLooperNext(target, looperdata) ) {

    if (state == "on") {
      printed = "Powering on";
      printed += ecmdWriteTarget(target) + " ..\n";
      ecmdOutput(printed.c_str());
      rc = ecmdFruPowerOn(target, smart);
    } else if (state == "off") {
      printed = "Powering off";
      printed += ecmdWriteTarget(target) + " ..\n";
      ecmdOutput(printed.c_str());
      rc = ecmdFruPowerOff(target, smart);
    }

    if (rc) {
      printed = "frupower - Error occurred performing frupower on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    else {
      validPosFound = true;     
    }
  }

  if (!validPosFound) {
    ecmdOutputError("frupower - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdBiasVoltageUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdChipTarget target;                ///< Current target operating on
  bool validPosFound = false;           ///< Did the looper find anything to execute on
  std::string printed;
  
  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc != 4 && argc != 3) {  
    ecmdOutputError("biasvoltage - Incorrect arguments specified; you need level, direction and value.\n");
    ecmdOutputError("biasvoltage - Type 'biasvoltage -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  
  
  //Setup the target that will be used to query the system config 
  target.cageState = target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
  target.slotState = target.posState = target.chipTypeState = target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  if (!ecmdIsAllDecimal(argv[0])) {
    ecmdOutputError("biasvoltage - Non-decimal numbers detected in level field\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t level = (uint32_t)atoi(argv[0]);

  // Push toupper for the comparision below.  This will allow the user to use lower case - JTA
  std::string dirStr = argv[1];
  transform(dirStr.begin(), dirStr.end(), dirStr.begin(), (int(*)(int)) toupper);
  
  ecmdVoltageType_t direction = ECMD_VOLTAGE_UNKNOWN;  // defaulting to remove compiler warnings
  if (dirStr == "NOM") direction = ECMD_VOLTAGE_NOMINAL;
  else if (dirStr == "DOWN")  direction = ECMD_VOLTAGE_PERCENT_DOWN;
  else if (dirStr == "UP")  direction = ECMD_VOLTAGE_PERCENT_UP;
  else {
    ecmdOutputError("biasvoltage - Invalid value for direction. Valid Values : NOM, UP, DOWN\n");
    return ECMD_INVALID_ARGS;
  }

  if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("biasvoltage - Non-decimal numbers detected in value field\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t value = (uint32_t)atoi(argv[2]);

  bool waitState = true;
  if (argc == 4) {
    std::string waitStr = argv[3];
    transform(waitStr.begin(), waitStr.end(), waitStr.begin(), (int(*)(int)) toupper);

    if (waitStr == "IMM") {
      waitState = false;
    }
  }


  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  
  while ( ecmdConfigLooperNext(target, looperdata) ) {

    rc = ecmdBiasVoltage(target, level, direction, value, waitState);
    
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "biasvoltage - Error occurred performing ecmdGpioReadLatch on ";
	printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }
    
    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      printed = ecmdWriteTarget(target) + "\n";
      ecmdOutput(printed.c_str());
    }

  }

  if (!validPosFound) {
    ecmdOutputError("biasvoltage - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdQueryBiasStateUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;
  ecmdChipTarget target;                        ///< Current target being operated on
  bool validPosFound = false;                   ///< Did the looper find anything?
  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string printed;                          ///< Output data
  uint32_t currentVoltage;
  uint32_t targetVoltage;
  uint32_t timeLeft;
  char returnStr[100];

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc != 1) {  //level
    ecmdOutputError("querybiasstate - Only the voltage level to query needs to be given.\n");
    ecmdOutputError("querybiasstate - Type 'querybiasstate -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config
  target.cageState = target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
  target.slotState = target.posState = target.chipTypeState = target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get level to fetch
  if (!ecmdIsAllDecimal(argv[0])) {
    ecmdOutputError("querybiasstate - Non-decimal numbers detected in level field\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t level = (uint32_t)atoi(argv[0]);

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;


  while ( ecmdConfigLooperNext(target, looperdata) ) {

    rc = ecmdQueryBiasState(target, level, currentVoltage, targetVoltage, timeLeft);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
      printed = "querybiasstate - Error occured performing getcfam on ";
      printed += ecmdWriteTarget(target);
      printed += "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    else {
      validPosFound = true;
    }

    printed = ecmdWriteTarget(target);
    sprintf(returnStr, "currentVoltage: %d, targetVoltage: %d, timeLeft: %dms\n", currentVoltage, targetVoltage, timeLeft);
    printed += returnStr;
    ecmdOutput(printed.c_str());
  }


  if (!validPosFound) {
    //this is an error common across all UI functions
    ecmdOutputError("querybiasstate - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}
