// Copyright ***********************************************************
//                                                                      
// File ecmdJtagUser.C                                  
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
/* $Header$ */

// Module Description **************************************************
//
// Description: 
//
// End Module Description **********************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define ecmdJtagUser_C
#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <ecmdSharedUtils.H>

#undef ecmdJtagUser_C
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


uint32_t ecmdSendCmdUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  bool expectFlag = false;
  bool xstateFlag = false;
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdChipTarget target;                ///< Current target being operated on
  uint32_t instruction;                 ///< Instruction to send to chip
  uint32_t modifier;                    ///< Modifier to send to chip
  ecmdDataBuffer statusBuffer;          ///< Buffer to hold return status data
  bool validPosFound = false;           ///< Did the looper find something to run on?

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
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
  if (argc < 3) {  //chip + instruction + modifier
    ecmdOutputError("sendcmd - Too few arguments specified; you need at least a chip, instruction, and modifier.\n");
    ecmdOutputError("sendcmd - Type 'sendcmd -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = target.coreState = ECMD_TARGET_FIELD_UNUSED;

  // we need the instruction and modifier

  if (strlen(argv[1]) > 2) {
    ecmdOutputError("sendcmd - The instruction has two be <= 8 bits\n");
    return ECMD_INVALID_ARGS;
  } else if (!ecmdIsAllHex(argv[1])) {
    ecmdOutputError("sendcmd - Instruction data contained some non-hex characters\n");
    return ECMD_INVALID_ARGS;
  }
  ecmdGenB32FromHexRight(&instruction, argv[1], 32);

  if (strlen(argv[2]) > 6) {
    ecmdOutputError("sendcmd - The modifier has two be <= 24 bits\n");
    return ECMD_INVALID_ARGS;
  } else if (!ecmdIsAllHex(argv[2])) {
    ecmdOutputError("sendcmd - Modifier data contained some non-hex characters\n");
    return ECMD_INVALID_ARGS;
  }
  ecmdGenB32FromHexRight(&modifier, argv[2], 32);


  if (argc > 3) {
    ecmdOutputError("sendcmd - Too many arguments specified; you probably added an option that wasn't recognized.\n");
    ecmdOutputError("sendcmd - Type 'sendcmd -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  char outstr[30];
  std::string printed;

  while ( ecmdConfigLooperNext(target, looperdata) ) {

    rc = sendCmd(target, instruction, modifier, statusBuffer);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "sendcmd - Error occured performing sendcmd on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }


    printed = ecmdWriteTarget(target) + "  ";
    std::string dataStr = ecmdWriteDataFormatted(statusBuffer, format);
    printed += dataStr;
    ecmdOutput( printed.c_str() );
    

  }

  if (!validPosFound) {
    ecmdOutputError("Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}
