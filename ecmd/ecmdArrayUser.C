// Copyright ***********************************************************
//                                                                      
// File ecmdArrayUser.C                                  
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
#define ecmdArrayUser_C
#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#undef ecmdArrayUser_C
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

int ecmdGetArrayUser(int argc, char * argv[]) {
  int rc = ECMD_SUCCESS;

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
  if (argc < 3) {
    ecmdOutputError("Too few arguments specified; you need at least a chip, an array, and an address.\nType 'getarray -h' for usage.");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  std::string arrayName = argv[1];

  ecmdDataBuffer address(3);
  rc = address.insertFromHexRight(argv[2]);
  if (rc) return rc;

  ecmdDataBuffer buffer(3);
  buffer.flushTo0();

  bool validPosFound = false;
  rc = ecmdConfigLooperInit(target);
  if (rc) return rc;
  std::string printed;

  while ( ecmdConfigLooperNext(target) ) {

    rc = getArray(target, arrayName.c_str(), address, buffer);

    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "Error occured performing getarray on ";
        printed += ecmdWriteTarget(target);
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }

    printed = ecmdWriteTarget(target) + " " + arrayName + " " + address.genHexRightStr();

    std::string dataStr = ecmdWriteDataFormatted(buffer, format);
    if (dataStr[0] != '\n') {
      printed += "\n";
    }

    printed += dataStr;

    ecmdOutput( printed.c_str() );

  }

  if (!validPosFound) {
    //this is an error common across all UI functions
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

int ecmdPutArrayUser(int argc, char * argv[]) {
  int rc = ECMD_SUCCESS;

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
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
  if (argc < 4) {
    ecmdOutputError("Too few arguments specified; you need at least a chip, an array, an address, and some data.\nType 'putarray -h' for usage.");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  std::string arrayName = argv[1];

  ecmdDataBuffer address(3);
  rc = address.insertFromHexRight(argv[2]);
  if (rc) return rc;

  ecmdDataBuffer buffer(3);
  rc = ecmdReadDataFormatted(buffer, argv[3], format);
  if (rc) return rc;


  bool validPosFound = false;
  rc = ecmdConfigLooperInit(target);
  if (rc) return rc;
  std::string printed;

  while ( ecmdConfigLooperNext(target) ) {

    rc = putArray(target, arrayName.c_str(), address, buffer);

    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "Error occured performing putarray on ";
        printed += ecmdWriteTarget(target);
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }

  }

  if (!validPosFound) {
    //this is an error common across all UI functions
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}


