// Copyright ***********************************************************
//                                                                      
// File ecmdMiscUser.C                                   
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
#define ecmdMiscUser_C
#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <ecmdSharedUtils.H>

#undef ecmdMiscUser_C
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

uint32_t ecmdGetConfigUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  std::string configName;       ///< Name of config variable to fetch
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  ecmdConfigValid_t validOutput;  ///< Indicator if valueAlpha, valueNumeric (or both) are valid
  std::string  valueAlpha;       ///< Alpha value of setting (if appropriate)
  uint32_t  valueNumeric;        ///< Numeric value of setting (if appropriate)
  ecmdDataBuffer numData;	 ///< Initialise data buffer with the numeric value
  std::string printed;           ///< Print Buffer
  ecmdLooperData looperdata;     ///< Store internal Looper data

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr == NULL) {
    format = "x";
  }
  else {
    format = formatPtr;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 1) {
    ecmdOutputError("getconfig - Too few arguments specified; you need at least a ConfigName.\n");
    ecmdOutputError("getconfig - Type 'getconfig -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config
  if( argc == 2) {
   target.chipType = argv[0];
   target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
   configName = argv[1];
  }
  else {
   target.chipTypeState = ECMD_TARGET_QUERY_IGNORE;
   configName = argv[0];
  }

  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while ( ecmdConfigLooperNext(target, looperdata) ) {

    /* Actually go fetch the data */
    rc = ecmdGetConfiguration(target, configName, validOutput, valueAlpha, valueNumeric);

    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
      printed = "getconfig - Error occured performing ecmdGetConfiguration on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    else {
      validPosFound = true;
    }

    printed = ecmdWriteTarget(target) + "\n";
    
    if( validOutput == ECMD_CONFIG_VALID_FIELD_ALPHA) {
     printed += configName + " = " + valueAlpha + "\n";
     ecmdOutput(printed.c_str());
    }
    else if(( validOutput == ECMD_CONFIG_VALID_FIELD_NUMERIC) || (validOutput ==  ECMD_CONFIG_VALID_FIELD_BOTH)) {
     numData.setWord(0, valueNumeric);
     printed += configName + " = ";
     printed += ecmdWriteDataFormatted(numData, format);
     printed += "\n";
     ecmdOutput(printed.c_str()); 
    }
  }

  if (!validPosFound) {
    //this is an error common across all UI functions
    ecmdOutputError("getconfig - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;

}


uint32_t ecmdSetConfigUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  std::string configName;       ///< Name of config variable to fetch
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  ecmdConfigValid_t validInput;  ///< Indicator if valueAlpha, valueNumeric (or both) are valid
  std::string  valueAlpha;       ///< Input of type Ascii
  uint32_t  valueNumeric;        ///< Input of type Numeric  
  ecmdDataBuffer inputBuffer;	 ///< Initialise data buffer with the numeric value(if input is Numeric)
  char inputVal[400];		 ///< Value to set configuration variable to
  std::string printed;           ///< Print Buffer
  ecmdLooperData looperdata;     ///< Store internal Looper data

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr == NULL) {
    format = "a";
  }
  else {
    format = formatPtr;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 2) {
    ecmdOutputError("setconfig - Too few arguments specified; you need at least a ConfigName, Value to set it to.\n");
    ecmdOutputError("setconfig - Type 'setconfig -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config
  if( argc == 3) {
   target.chipType = argv[0];
   target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
   configName = argv[1];
   strcpy(inputVal, argv[2]);
  }
  else {
   target.chipTypeState = ECMD_TARGET_QUERY_IGNORE;
   configName = argv[0];
   strcpy(inputVal, argv[1]);
  }
  if(format == "a") {
    valueAlpha = inputVal;
    validInput = ECMD_CONFIG_VALID_FIELD_ALPHA;
  } else {
    rc = ecmdReadDataFormatted(inputBuffer, inputVal, format);
    if (rc) {
     ecmdOutputError("setconfig - Problems occurred parsing input data, must be an invalid format\n");
     return rc;
    }
    int numBits = inputBuffer.getBitLength();
    if(numBits > 32 ) {
     ecmdOutputError("setconfig - Problems occurred parsing input data, Bitlength should be <= 32 bits\n");
     return ECMD_INVALID_ARGS;
    }
    inputBuffer.shiftRightAndResize(32-numBits);
    valueNumeric = inputBuffer.getWord(0);
    validInput = ECMD_CONFIG_VALID_FIELD_NUMERIC;
  }
    
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while ( ecmdConfigLooperNext(target, looperdata) ) {
    /* Actually go fetch the data */
    rc = ecmdSetConfiguration(target, configName, validInput, valueAlpha, valueNumeric);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
      printed = "setconfig - Error occured performing ecmdSetConfiguration on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    else {
      validPosFound = true;
    }
    printed = ecmdWriteTarget(target) + "\n";
    ecmdOutput( printed.c_str() );
  }

  if (!validPosFound) {
    //this is an error common across all UI functions
    ecmdOutputError("setconfig - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
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
