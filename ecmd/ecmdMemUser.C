/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File ecmdMemUser.C                                  
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
#define ecmdMemUser_C
#include <stdio.h>
#include <ctype.h>

#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <ecmdInterpreter.H>
#include <ecmdSharedUtils.H>

#undef ecmdMemUser_C
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

uint32_t ecmdGetMemUser(int argc, char * argv[], ECMD_DA_TYPE memMode) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string outputformat = "mem";     ///< Output format - default to 'mem'
  ecmdDataBuffer returnData;            ///< Buffer to hold return data from memory
  bool validPosFound = false;           ///< Did the looper find anything?
  ecmdChipTarget target;                ///< Current target being operated on
  uint64_t address;                     ///< The address from the command line
  uint32_t numBytes;                    ///< Number of bytes from the command line
  std::string cmdlineName;              ///< Stores the name of what the command line function would be.
  int match;                            ///< For sscanf
  std::string printLine;                ///< Output data

  /************************************************************************/
  /* Setup the cmdlineName variable                                       */
  /************************************************************************/
  if (memMode == ECMD_MEM_DMA) {
    cmdlineName = "getmemdma";
  } else if (memMode == ECMD_MEM_MEMCTRL) {
    cmdlineName = "getmemmemctrl";
  } else if (memMode == ECMD_MEM_PROC) {
    cmdlineName = "getmemproc";
  }

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr != NULL) {
    outputformat = formatPtr;
  }

  /* Get the filename if -fb is specified */
  char * filename = ecmdParseOptionWithArgs(&argc, &argv, "-fb");
  
  if((filename != NULL) && (formatPtr != NULL) ) {
    printLine = cmdlineName + " - Options -fb and -o can't be specified together for format. Specify either one.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  } 
  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 2) {  //chip + address
    printLine = cmdlineName + " - Too few arguments specified; you need at least an address and number of bytes.\n";
    ecmdOutputError(printLine.c_str());
    printLine = cmdlineName + " - Type '" + cmdlineName + " -h' for usage.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  if (memMode == ECMD_MEM_DMA) {
    target.cageState = target.nodeState = ECMD_TARGET_QUERY_WILDCARD;
    target.chipTypeState = target.slotState = target.posState = target.threadState = target.coreState = ECMD_TARGET_FIELD_UNUSED;
  } else if (memMode == ECMD_MEM_MEMCTRL) {
    target.chipType = ECMD_CHIPT_MEM_CNTRL;
    target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_QUERY_WILDCARD;
    target.threadState = target.coreState = ECMD_TARGET_FIELD_UNUSED;
  } else if (memMode == ECMD_MEM_PROC) {
    target.chipType = ECMD_CHIPT_PROCESSOR;
    target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_QUERY_WILDCARD;
    target.threadState = target.coreState = ECMD_TARGET_FIELD_UNUSED;
  }

  // Get the address
  if (!ecmdIsAllHex(argv[0])) {
    printLine = cmdlineName + " - Non-hex characters detected in address field\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }
  match = sscanf(argv[0], "%llx", &address);
  if (match != 1) {
    ecmdOutputError("Error occurred processing address!\n");
    return ECMD_INVALID_ARGS;
  }

  // Get the number of bits
  numBytes = atoi(argv[1]);

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while ( ecmdConfigLooperNext(target, looperdata) ) {

    if (memMode == ECMD_MEM_DMA) {
      rc = getMemDma(target, address, numBytes, returnData);
    } else if (memMode == ECMD_MEM_MEMCTRL) {
      rc = getMemMemCtrl(target, address, numBytes, returnData);
    } else if (memMode == ECMD_MEM_PROC) {
      rc = getMemProc(target, address, numBytes, returnData);
    }

    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
      printLine = cmdlineName + " - Error occured performing " + cmdlineName + " on ";
      printLine += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printLine.c_str() );
      return rc;
    }
    else {
      validPosFound = true;     
    }
    
    printLine = ecmdWriteTarget(target);
    if (filename != NULL) {
      rc = returnData.writeFile(filename, ECMD_SAVE_FORMAT_BINARY_DATA);
       
      if (rc) {
       printLine += cmdlineName + " - Problems occurred writing data into file" + filename +"\n";
       ecmdOutputError(printLine.c_str()); 
       return rc;
      }
      ecmdOutput( printLine.c_str() );
    } else  {
     
     std::string dataStr = ecmdWriteDataFormatted(returnData, outputformat, address);
     if (dataStr[0] != '\n') {
       printLine += "\n";
     }
     printLine += dataStr;
     ecmdOutput( printLine.c_str() );
    }

    
  }

  if (!validPosFound) {
    printLine = cmdlineName + " - Unable to find a valid chip to execute command on\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdPutMemUser(int argc, char * argv[], ECMD_DA_TYPE memMode) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string inputformat = "mem";      ///< Output format - default to 'mem'
  ecmdDataBuffer inputData;             ///< Buffer to hold the data intended for memory
  bool validPosFound = false;           ///< Did the looper find anything?
  ecmdChipTarget target;                ///< Current target being operated on
  uint64_t address;                     ///< The address from the command line
  std::string cmdlineName;              ///< Stores the name of what the command line function would be.
  int match;                            ///< For sscanf
  std::string printLine;                ///< Output data

  /************************************************************************/
  /* Setup the cmdlineName variable                                       */
  /************************************************************************/
  if (memMode == ECMD_MEM_DMA) {
    cmdlineName = "putmemdma";
  } else if (memMode == ECMD_MEM_MEMCTRL) {
    cmdlineName = "putmemmemctrl";
  } else if (memMode == ECMD_MEM_PROC) {
    cmdlineName = "putmemproc";
  }

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    inputformat = formatPtr;
  }
  /* Get the filename if -fb is specified */
  char * filename = ecmdParseOptionWithArgs(&argc, &argv, "-fb");
  
  if((filename != NULL) && (formatPtr != NULL) ) {
    printLine = cmdlineName + "Options -fb and -i can't be specified together for format. Specify either one.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  } 
  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if ((argc < 2)&&(filename == NULL)) {  //chip + address
    printLine = cmdlineName + " - Too few arguments specified; you need at least an address and number of bytes.\n";
    ecmdOutputError(printLine.c_str());
    printLine = cmdlineName + " - Type '" + cmdlineName + " -h' for usage.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }else if((argc < 1)&&(filename != NULL)) { 
    printLine = cmdlineName + " - Too few arguments specified; you need at least an address and input data file.\n";
    ecmdOutputError(printLine.c_str());
    printLine = cmdlineName + " - Type '" + cmdlineName + " -h' for usage.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }
  //Setup the target that will be used to query the system config 
  if (memMode == ECMD_MEM_DMA) {
    target.cageState = target.nodeState = ECMD_TARGET_QUERY_WILDCARD;
    target.chipTypeState = target.slotState = target.posState = target.threadState = target.coreState = ECMD_TARGET_FIELD_UNUSED;
  } else if (memMode == ECMD_MEM_MEMCTRL) {
    target.chipType = ECMD_CHIPT_MEM_CNTRL;
    target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_QUERY_WILDCARD;
    target.threadState = target.coreState = ECMD_TARGET_FIELD_UNUSED;
  } else if (memMode == ECMD_MEM_PROC) {
    target.chipType = ECMD_CHIPT_PROCESSOR;
    target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_QUERY_WILDCARD;
    target.threadState = target.coreState = ECMD_TARGET_FIELD_UNUSED;
  }


  // Get the address
  if (!ecmdIsAllHex(argv[0])) {
    printLine = cmdlineName + " - Non-hex characters detected in address field\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }
  match = sscanf(argv[0], "%llx", &address);
  if (match != 1) {
    ecmdOutputError("Error occurred processing address!\n");
    return ECMD_INVALID_ARGS;
  }
  
  // Read in the input data
  if(filename != NULL) {
    rc = inputData.readFile(filename, ECMD_SAVE_FORMAT_BINARY_DATA);
    if (rc) {
     printLine = cmdlineName + " - Problems occurred parsing input data from file" + filename +", must be an invalid format\n";
     ecmdOutputError(printLine.c_str());
     return rc;
    }
  } else  {
   rc = ecmdReadDataFormatted(inputData, argv[1] , inputformat);
   if (rc) {
     printLine = cmdlineName + " - Problems occurred parsing input data, must be an invalid format\n";
     ecmdOutputError(printLine.c_str());
     return rc;
   }
  }
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while ( ecmdConfigLooperNext(target, looperdata) ) {

    if (memMode == ECMD_MEM_DMA) {
      rc = putMemDma(target, address, inputData.getByteLength(), inputData);
    } else if (memMode == ECMD_MEM_MEMCTRL) {
      rc = putMemMemCtrl(target, address, inputData.getByteLength(), inputData);
    } else if (memMode == ECMD_MEM_PROC) {
      rc = putMemProc(target, address, inputData.getByteLength(), inputData);
    }

    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
      printLine = cmdlineName + " - Error occured performing " + cmdlineName + " on ";
      printLine += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printLine.c_str() );
      return rc;
    }
    else {
      validPosFound = true;     
    }

    // Write out who we wrote too
    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      printLine = ecmdWriteTarget(target) + "\n";
      ecmdOutput(printLine.c_str());
    }
  }

  if (!validPosFound) {
    printLine = cmdlineName + " - Unable to find a valid chip to execute command on\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}


