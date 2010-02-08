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
#ifndef ECMD_REMOVE_POWER_FUNCTIONS
uint32_t ecmdSystemPowerUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  ecmdPowerState_t  o_state;

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc < 1) {
    ecmdOutputError("systempower - At least one argument ('on', 'off', 'query') is required for systempower.\n");
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
  } else if (!strcmp(argv[0], "query")) {


    rc = ecmdQuerySystemPower(o_state);

    if (rc) {
      return ECMD_FAILURE;
    }
    else {
      if (o_state==ECMD_POWER_STATE_UNKNOWN) {
        ecmdOutput("Systempower is unknown ...\n");
      }
      else if (o_state==ECMD_POWER_STATE_ON) {
        ecmdOutput("Systempower is ON ...\n");
      }
      else if (o_state==ECMD_POWER_STATE_OFF) {
        ecmdOutput("Systempower is OFF ...\n");
      }
      else if (o_state==ECMD_POWER_STATE_TRANSITION) {
        ecmdOutput("Systempower is in Transition ...\n");
      }
    }
  } else {
    ecmdOutputError("systempower - Invalid argument passed to systempower. Accepted arguments: ('on', 'off', 'query').\n");
    return ECMD_INVALID_ARGS;
  }

  return rc;

}

uint32_t ecmdFruPowerUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

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

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  if (argc < 2) {
    ecmdOutputError("frupower - At least one argument ('on', 'off', 'query') is required for frupower.\n");
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
  } else if (!strcmp(argv[0], "query")) {
    state = "query";
  } else {
    ecmdOutputError("frupower - Invalid argument passed to frupower. Accepted arguments: ('on', 'off', 'query').\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used 
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[1], chipType, chipUnitType);
  if (chipUnitType != "") {
    ecmdOutputError("getvpdkeyword - chipUnit specified on the command line, this function doesn't support chipUnits.\n");
    return ECMD_INVALID_ARGS;
  }
  target.chipType = chipType;
  if (target.chipType == "nochip") {
    target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
    target.posState = ECMD_TARGET_FIELD_UNUSED;
  } else {
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    target.posState = ECMD_TARGET_FIELD_WILDCARD;
  }
  target.cageState = target.nodeState = target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;


  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/
  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

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
    } else if (state == "query") {
      printed = "Frupower of ";
      printed += ecmdWriteTarget(target);
      ecmdPowerState_t  o_state = ECMD_POWER_STATE_UNKNOWN;

      rc = ecmdQueryFruPower(target,  o_state);

      if (rc==0)  {
        if (o_state==ECMD_POWER_STATE_UNKNOWN) {
          printed += " is unknown  \n ";
          ecmdOutput(printed.c_str());
        }
        else if (o_state==ECMD_POWER_STATE_ON) {
          printed += " is ON  \n ";
          ecmdOutput(printed.c_str());
        }
        else if (o_state==ECMD_POWER_STATE_OFF) {
          printed += " is OFF  \n ";
          ecmdOutput(printed.c_str());
        }
        else if (o_state==ECMD_POWER_STATE_TRANSITION) {
          printed += " is in Transition  \n ";
          ecmdOutput(printed.c_str());
        }
      }
    }

    if (rc) {
      printed = "frupower - Error occurred performing frupower ";
      printed += state  + " function ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }
    else {
      validPosFound = true;     
    }
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("frupower - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdPowerModeUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdChipTarget target;                ///< Current target operating on
  bool validPosFound = false;           ///< Did the looper find anything to execute on
  std::string printed;
  std::string mode;

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  if (argc < 1) {
    ecmdOutputError("powermode - At least one argument ('save', 'normal', 'query', 'turbo') is required for powermode.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 1) {
    ecmdOutputError("powermode - Too many arguments to powermode, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  if (!strcmp(argv[0], "normal")) {
    mode = "normal";
  } else if (!strcmp(argv[0], "save")) {
    mode = "save";
  } else if (!strcmp(argv[0], "turbo")) {
    mode = "turbo";
  } else if (!strcmp(argv[0], "query")) {
    mode = "query";
  } else if (!strcmp(argv[0], "dynamic_save")) {
    mode = "dynamic_save";
  } else if (!strcmp(argv[0], "dynamic_save_mp")) {
    mode = "dynamic_save_mp";
  } else {
    ecmdOutputError("powermode - Invalid argument passed to powermode. Accepted arguments: ('save', 'normal', 'query', 'turbo').\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used 
    target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
    target.posState = ECMD_TARGET_FIELD_UNUSED;
  target.cageState = ECMD_TARGET_FIELD_WILDCARD;
  target.nodeState = ECMD_TARGET_FIELD_UNUSED;
  target.slotState = ECMD_TARGET_FIELD_UNUSED;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;


  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/
  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    if (mode == "normal") {
      printed = ecmdWriteTarget(target) + ".. ";
      printed += "Normal Mode\n";
      ecmdOutput(printed.c_str());
      rc = ecmdSetPowerMode(target, ECMD_POWER_MODE_NORMAL);
    } else if (mode == "save") {
      printed = ecmdWriteTarget(target) + ".. ";
      printed += "Power Save Mode\n";
      ecmdOutput(printed.c_str());
      rc = ecmdSetPowerMode(target, ECMD_POWER_MODE_SAVE);
    } else if (mode == "turbo") {
      printed = ecmdWriteTarget(target) + ".. ";
      printed += "Turbo Mode\n";
      ecmdOutput(printed.c_str());
      rc = ecmdSetPowerMode(target, ECMD_POWER_MODE_TURBO);
    } else if (mode == "dynamic_save") {
      printed = ecmdWriteTarget(target) + ".. ";
      printed += "Dynamic Powersave Mode\n";
      ecmdOutput(printed.c_str());
      rc = ecmdSetPowerMode(target, ECMD_POWER_MODE_SAVE_DYNAMIC);
    } else if (mode == "dynamic_save_mp") {
      printed = ecmdWriteTarget(target) + ".. ";
      printed += "Dynamic Powersave Max Performance Mode\n";
      ecmdOutput(printed.c_str());
      rc = ecmdSetPowerMode(target, ECMD_POWER_MODE_SAVE_DYNAMIC_MP);
   } else if (mode == "query") {
     ecmdPowerMode_t o_mode=ECMD_POWER_MODE_UNKNOWN;
     rc = ecmdGetPowerMode(target, o_mode);
     printed = "Query Mode :";

     if (o_mode == ECMD_POWER_MODE_SAVE) {
       printed += " Save \n";
     } else  if (o_mode == ECMD_POWER_MODE_NORMAL){
       printed += " Normal \n";
     } else  if (o_mode== ECMD_POWER_MODE_TURBO)   {
       printed += " Turbo \n";
     } else  if (o_mode== ECMD_POWER_MODE_SAVE_DYNAMIC)   {
       printed += " Dynamic Powersave \n";
     } else  if (o_mode== ECMD_POWER_MODE_SAVE_DYNAMIC_MP)   {
       printed += " Dynamic Powersave Max Performance \n";
     } else  if (o_mode== ECMD_POWER_MODE_IN_TRANSITION)   {
       printed += " Transition \n";
     } else {
       printed += " unknown \n";
     }

     ecmdOutput(printed.c_str());
   }

    if (rc) {
      printed = "powermode - Error occurred performing powermode ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }
    else {
      validPosFound = true;     
    }
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("powermode - Unable to find a target to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}
uint32_t ecmdBiasVoltageUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;
  ecmdLooperData looperData;            ///< Store internal Looper data
  ecmdChipTarget target;                ///< Current target operating on
  ecmdChipTarget vdTarget;                ///< Current target operating on
  ecmdLooperData vdLooperData;            ///< Store internal Looper data
  bool validPosFound = false;           ///< Did the looper find anything to execute on
  std::string printed;
  bool waitState = true;                ///< Wait for the command to finish or return immediately
  std::string biasString;
  uint32_t biasValue;
  uint32_t voltageLevel;

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
  if (ecmdParseOption(&argc, &argv, "-immediate")) {
    waitState = false;
  }

  if (argc != 2) {  
    ecmdOutputError("biasvoltage - Incorrect arguments specified; you need voltageLevel and biasLevel.\n");
    ecmdOutputError("biasvoltage - Type 'biasvoltage -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  // Setup the target that will be used to loop 
  // First we will do a default node level loop, and then try a slot level loop if the user specifed one
  target.cageState = target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
  target.slotState = target.posState = target.chipTypeState = target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  // Get the leve the user specified
  voltageLevel = (uint32_t)atoi(argv[0]);

  if (!ecmdIsAllDecimal(argv[0])) {
    ecmdOutputError("biasvoltage - Non-decimal numbers detected in level field\n");
    return ECMD_INVALID_ARGS;
  }


  //get clockspeed
  size_t strpos;
  biasString = argv[1];
  transform(biasString.begin(), biasString.end(), biasString.begin(), (int(*)(int)) tolower);
  
  ecmdVoltageType_t voltageType = ECMD_VOLTAGE_UNKNOWN;  // defaulting to remove compiler warnings
  if ((strpos = biasString.find("nom")) != std::string::npos)  {
    voltageType = ECMD_VOLTAGE_NOMINAL;
  } else if ((strpos = biasString.find("npu")) != std::string::npos) {
    voltageType = ECMD_VOLTAGE_PERCENT_UP;
  } else if ((strpos = biasString.find("npd")) != std::string::npos) {
    voltageType = ECMD_VOLTAGE_PERCENT_DOWN;
  } else if ((strpos = biasString.find("pspu")) != std::string::npos) {
    voltageType = ECMD_VOLTAGE_POWERSAVE_PERCENT_UP;
  } else if ((strpos = biasString.find("pspd")) != std::string::npos) {
    voltageType = ECMD_VOLTAGE_POWERSAVE_PERCENT_DOWN;
  } else {
    ecmdOutputError("biasvoltage - a valid biasValue Keyword not found biasValue field\n");
    ecmdOutputError("biasvoltage - Type 'biasvoltage -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  
  biasString.erase(strpos, biasString.length()-strpos);
  
  if (!ecmdIsAllDecimal(biasString.c_str())) {
    ecmdOutputError("biasvoltage - Non-Decimal characters detected in biasValue field\n");
    return ECMD_INVALID_ARGS;
  } 
  
  biasValue = (uint32_t)atoi(biasString.c_str());

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;
  
  while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {

    // Now try a slot level loop on this node loop
    // Preserve the valid states from the looperNext, but reset the slot to wildcard
    vdTarget = target;
    vdTarget.slotState = ECMD_TARGET_FIELD_WILDCARD;
    rc = ecmdLooperInit(vdTarget, ECMD_SELECTED_TARGETS_LOOP_VD, vdLooperData);
    if (rc) break;

    while (ecmdLooperNext(vdTarget, vdLooperData) && (!coeRc || coeMode)) {
      rc = ecmdBiasVoltage(vdTarget, voltageLevel, voltageType, biasValue, waitState);

      if (rc) {
        printed = "biasvoltage - Error occurred performing ecmdBiasVoltage on ";
        printed += ecmdWriteTarget(vdTarget) + "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
      }
      else {
        validPosFound = true;     
      }

      if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
        printed = ecmdWriteTarget(vdTarget) + "\n";
        ecmdOutput(printed.c_str());
      }

    }
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("biasvoltage - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdQueryBiasStateUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;
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

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc != 1) {  //level
    ecmdOutputError("querybiasstate - Only the voltage level to query needs to be given.\n");
    ecmdOutputError("querybiasstate - Type 'querybiasstate -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config
  target.cageState = target.nodeState = target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.posState = target.chipTypeState = target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get level to fetch
  if (!ecmdIsAllDecimal(argv[0])) {
    ecmdOutputError("querybiasstate - Non-decimal numbers detected in level field\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t voltageLevel = (uint32_t)atoi(argv[0]);

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;


  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    rc = ecmdQueryBiasState(target, voltageLevel, currentVoltage, targetVoltage, timeLeft);
    if (rc) {
      printed = "querybiasstate - Error occured performing getcfam on ";
      printed += ecmdWriteTarget(target);
      printed += "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }
    else {
      validPosFound = true;
    }

    printed = ecmdWriteTarget(target);
    sprintf(returnStr, "currentVoltage: %d, targetVoltage: %d, timeLeft: %dms\n", currentVoltage, targetVoltage, timeLeft);
    printed += returnStr;
    ecmdOutput(printed.c_str());
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("querybiasstate - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}
#endif // ECMD_REMOVE_POWER_FUNCTIONS
