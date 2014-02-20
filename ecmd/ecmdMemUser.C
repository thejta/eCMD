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

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <ctype.h>
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
#ifndef ECMD_REMOVE_MEMORY_FUNCTIONS

uint32_t ecmdGetMemUser(int argc, char * argv[], ECMD_DA_TYPE memMode) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string outputformat = "mem";     ///< Output format - default to 'mem'
  ecmdDataBuffer returnData;            ///< Buffer to hold return data from memory
  bool validPosFound = false;           ///< Did the looper find anything?
  ecmdChipTarget target;                ///< Current target being operated on
  uint32_t channel = 0xFFFFFFFF;        ///< Channel to use for getsram
  uint64_t address;                     ///< The address from the command line
  uint32_t numBytes = ECMD_UNSET;       ///< Number of bytes from the command line
  std::string cmdlineName;              ///< Stores the name of what the command line function would be.
  int match;                            ///< For sscanf
  std::string printLine;                ///< Output data

  bool expectFlag = false;              ///< Are we doing an expect?
  bool maskFlag = false;                ///< Are we masking our expect data?
  char* expectPtr = NULL;               ///< Pointer to expected data in arg list
  char* maskPtr = NULL;                 ///< Pointer to mask data in arg list
  ecmdDataBuffer expected;                      ///< Buffer to store expected data
  ecmdDataBuffer mask;                          ///< Buffer for mask of expected data
  std::string inputformat = "x";                ///< Input format of data


  /************************************************************************/
  /* Setup the cmdlineName variable                                       */
  /************************************************************************/
  if (memMode == ECMD_MEM_DMA) {
    cmdlineName = "getmemdma";
  } else if (memMode == ECMD_MEM_MEMCTRL) {
    cmdlineName = "getmemmemctrl";
  } else if (memMode == ECMD_MEM_PROC) {
    cmdlineName = "getmemproc";
  } else if (memMode == ECMD_SRAM) {
    cmdlineName = "getsram";
  }

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/

  /* get format flag, if it's there */
  char * outputFormatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (outputFormatPtr != NULL) {
    outputformat = outputFormatPtr;
  }  
  char * inputFormatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (inputFormatPtr != NULL) {
    inputformat = inputFormatPtr;
  }

  /* Get the channel for getsram */
  if (memMode == ECMD_SRAM) {
    char * channelPtr = ecmdParseOptionWithArgs(&argc, &argv, "-ch");
    if (channelPtr != NULL) {
      if (!ecmdIsAllDecimal(channelPtr)) {
        printLine = cmdlineName + " - Non-dec characters detected in channel option\n";
        ecmdOutputError(printLine.c_str());
        return ECMD_INVALID_ARGS;
      }
      channel = (uint32_t)atoi(channelPtr);
    } else {
      printLine = cmdlineName + " - channel must be specified with -ch option. Please contact plugin owner for correct channel. Cronus/FSP default channel: 2.\n";
      ecmdOutputError(printLine.c_str());
      return ECMD_INVALID_ARGS;
    }
  }

  /* Get the filename if -fb is specified */
  char * filename = ecmdParseOptionWithArgs(&argc, &argv, "-fb");
  
  /* Get the filename if -fd is specified */
  char * dcardfilename = ecmdParseOptionWithArgs(&argc, &argv, "-fd");

  if(((filename != NULL) || (dcardfilename != NULL)) && 
     ((outputFormatPtr != NULL) || (inputFormatPtr != NULL))) {
    printLine = cmdlineName + " - Options -f and -o/-i can't be specified together for format. Specify either one.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  } 
  if((dcardfilename != NULL) && (filename != NULL)) {
    printLine = cmdlineName + "Options -fb and -fd can't be specified together for format. Specify either one.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }

  //expect and mask flags check
  if (filename == NULL && dcardfilename == NULL) {
    if ((expectPtr = ecmdParseOptionWithArgs(&argc, &argv, "-exp")) != NULL) {
      expectFlag = true;
      
      if ((maskPtr = ecmdParseOptionWithArgs(&argc, &argv, "-mask")) != NULL) {
	maskFlag = true;
      }
    }
  } else {
    // If we are passing in data with a file, just look for -exp
    expectFlag = ecmdParseOption(&argc, &argv, "-exp");
  }


  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  //Setup the target that will be used to query the system config
  // Memctrl DA is on the cage depth and proc/dma are on the processor pos depth
  if (memMode == ECMD_MEM_MEMCTRL) {
    target.cageState = ECMD_TARGET_FIELD_WILDCARD;
    target.nodeState = target.chipTypeState = target.slotState = 
      target.posState = target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;
  } else if ((memMode == ECMD_MEM_PROC) || (memMode == ECMD_MEM_DMA) || (memMode == ECMD_SRAM)) {
    target.chipType = ECMD_CHIPT_PROCESSOR;
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = 
      target.posState = ECMD_TARGET_FIELD_WILDCARD;
    target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;
  }

  // Read in the expect data
  if (expectFlag) {
    if ((filename == NULL) && (dcardfilename == NULL)) {

      rc = ecmdReadDataFormatted(expected, expectPtr, inputformat);
      if (rc) {
	ecmdOutputError((cmdlineName + " - Problems occurred parsing expected data, must be an invalid format\n").c_str());
	return rc;
      }

      if (maskFlag) {
	rc = ecmdReadDataFormatted(mask, maskPtr, inputformat);
	if (rc) {
	  ecmdOutputError((cmdlineName + " - Problems occurred parsing mask data, must be an invalid format\n").c_str());
	  return rc;
	}
	
      }

    } else {
      // Read from a file 
      if (filename != NULL) {
	rc = expected.readFile(filename, ECMD_SAVE_FORMAT_BINARY_DATA);
	if (rc) {
	  ecmdOutputError((cmdlineName + " - Problems occurred parsing expected data from file " + filename + " , must be an invalid format\n").c_str());
	  return rc;
	}
      } else {
	// Dcard
	// Pulled support for this as it is a pain, could be added if needed, 
	//   ecmdReadDcard returns a list of memory entries because there could 
	//   be holes, so would have to loop down below
	ecmdOutputError((cmdlineName + " - Currently Dcard support is not supported with -exp\n").c_str());
	return ECMD_INVALID_ARGS;
	//	rc = ecmdReadDcard(dcardfilename, expected);
	//	if (rc) {
	//	  printLine = cmdlineName + " - Problems occurred parsing input data from file " + dcardfilename +", must be an invalid format\n";
	//	  ecmdOutputError(printLine.c_str());
	//	  return rc;
	//	}

      }
      // Let's pull the length from the file
      numBytes = expected.getByteLength();
    }
	
  }

  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if ((numBytes == ECMD_UNSET) && (argc < 2)) {  //chip + address
    printLine = cmdlineName + " - Too few arguments specified; you need at least an address and number of bytes.\n";
    ecmdOutputError(printLine.c_str());
    printLine = cmdlineName + " - Type '" + cmdlineName + " -h' for usage.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  } else if (argc < 1) { 
    printLine = cmdlineName + " - Too few arguments specified; you need at least an address.\n";
    ecmdOutputError(printLine.c_str());
    printLine = cmdlineName + " - Type '" + cmdlineName + " -h' for usage.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }

  // Get the address
  if (!ecmdIsAllHex(argv[0])) {
    printLine = cmdlineName + " - Non-hex characters detected in address field\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }
#ifdef _LP64
  match = sscanf(argv[0], "%lx", &address);
#else
  match = sscanf(argv[0], "%llx", &address);
#endif
  if (match != 1) {
    ecmdOutputError("Error occurred processing address!\n");
    return ECMD_INVALID_ARGS;
  }

  // Get the number of bytes
  if (numBytes == ECMD_UNSET) {
    numBytes = (uint32_t)atoi(argv[1]);
  }


  // do not allow numBytes >= 512MB
  if (numBytes >= 0x20000000) {
    printLine = cmdlineName + " - Number of bytes must be < 512MB\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }


  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    if (memMode == ECMD_MEM_DMA) {
      rc = getMemDma(target, address, numBytes, returnData);
    } else if (memMode == ECMD_MEM_MEMCTRL) {
      rc = getMemMemCtrl(target, address, numBytes, returnData);
    } else if (memMode == ECMD_MEM_PROC) {
      rc = getMemProc(target, address, numBytes, returnData);
    } else if (memMode == ECMD_SRAM) {
      rc = getSram(target, channel, address, numBytes, returnData);
    }

    if (rc) {
      printLine = cmdlineName + " - Error occured performing " + cmdlineName + " on ";
      printLine += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printLine.c_str() );
      coeRc = rc;
      continue;
    }
    else {
      validPosFound = true;     
    }
    
    printLine = ecmdWriteTarget(target);
    if (expectFlag) {
      uint32_t mismatchBit = 0;

      if (maskFlag) {
        returnData.setAnd(mask, 0, returnData.getBitLength());
      }

      if (!ecmdCheckExpected(returnData, expected, mismatchBit)) {

        //@ make this stuff sprintf'd
        char outstr[300];
        printLine = ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printLine.c_str() );
        if (mismatchBit != ECMD_UNSET) {
          sprintf(outstr, "%s - Data miscompare occured at (address %llX) (bit %d) (byte %d:0x%X bit %d)\n", 
                  cmdlineName.c_str(), (unsigned long long)address, mismatchBit, mismatchBit/8,
                  mismatchBit/8, mismatchBit%8);
          ecmdOutputError( outstr );
        }
        coeRc = ECMD_EXPECT_FAILURE;
        continue;
      }
    } else {
      if (filename != NULL) {
	rc = returnData.writeFile(filename, ECMD_SAVE_FORMAT_BINARY_DATA);
       
	if (rc) {
	  printLine += cmdlineName + " - Problems occurred writing data into file" + filename +"\n";
	  ecmdOutputError(printLine.c_str());
          break;
	}
	ecmdOutput( printLine.c_str() );
      } else if (dcardfilename != NULL) {
	std::string dataStr = ecmdWriteDataFormatted(returnData, "memd", address);
	std::ofstream ops;
	ops.open(dcardfilename);
	if (ops.fail()) {
	  char mesg[1000];
	  sprintf(mesg, "Unable to open file : %s for write", dcardfilename);
	  ecmdOutputError(mesg);
	  return ECMD_DBUF_FOPEN_FAIL;
	}
	if (dataStr[0] != '\n') {
	  printLine += "\n";
	}
	printLine += dataStr;
	ops << printLine.c_str();
	ops.close();

      } else  {
     
	std::string dataStr = ecmdWriteDataFormatted(returnData, outputformat, address);
	if (dataStr[0] != '\n') {
	  printLine += "\n";
	}
	printLine += dataStr;
	ecmdOutput( printLine.c_str() );
      }
    }  
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    printLine = cmdlineName + " - Unable to find a valid chip to execute command on\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdPutMemUser(int argc, char * argv[], ECMD_DA_TYPE memMode) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string inputformat = "x";        ///< Output format - default to 'mem'
  ecmdDataBuffer inputData;             ///< Buffer to hold the data intended for memory
  std::list<ecmdMemoryEntry> memdata;   ///< Data from the D-Card format file 
  std::list<ecmdMemoryEntry>::iterator memdataIter; ///< to iterate on memdata list 
  ecmdMemoryEntry memEntry;             ///< to store data from the user
  bool validPosFound = false;           ///< Did the looper find anything?
  ecmdChipTarget target;                ///< Current target being operated on
  uint32_t channel = 0xFFFFFFFF;        ///< Channel to use for getsram
  uint64_t address = 0;                 ///< The address from the command line
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
  } else if (memMode == ECMD_SRAM) {
    cmdlineName = "putsram";
  }

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    inputformat = formatPtr;
  }

  /* Get the channel for putsram */
  if (memMode == ECMD_SRAM) {
    char * channelPtr = ecmdParseOptionWithArgs(&argc, &argv, "-ch");
    if (channelPtr != NULL) {
      if (!ecmdIsAllDecimal(channelPtr)) {
        printLine = cmdlineName + " - Non-dec characters detected in channel option\n";
        ecmdOutputError(printLine.c_str());
        return ECMD_INVALID_ARGS;
      }
      channel = (uint32_t)atoi(channelPtr);
    } else {
      printLine = cmdlineName + " - channel must be specified with -ch option. Please contact plugin owner for correct channel. Cronus/FSP default channel: 2.\n";
      ecmdOutputError(printLine.c_str());
      return ECMD_INVALID_ARGS;
    }
  }

  /* Get the filename if -fb is specified */
  char * filename = ecmdParseOptionWithArgs(&argc, &argv, "-fb");
 
  /* Get the filename to file in D-Card format */ 
  char *dcardfilename = ecmdParseOptionWithArgs(&argc, &argv, "-fd");

  if(((filename != NULL) || (dcardfilename != NULL)) && (formatPtr != NULL) ) {
    printLine = cmdlineName + "Options -f and -i can't be specified together for format. Specify either one.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  } 

  if((dcardfilename != NULL) && (filename != NULL)) {
    printLine = cmdlineName + "Options -fb and -fd can't be specified together for format. Specify either one.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }
  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  if ( (argc < 2)&&((filename == NULL) && (dcardfilename == NULL)) ) {  //chip + address
    printLine = cmdlineName + " - Too few arguments specified; you need at least an address and data to write.\n";
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
  } else if(((argc > 2)&&((filename == NULL) && (dcardfilename == NULL))) || ((argc > 1)&&(filename != NULL))) {
    printLine = cmdlineName + " - Too many arguments specified; you only need an address and input data|file.\n";
    ecmdOutputError(printLine.c_str());
    printLine = cmdlineName + " - Type '" + cmdlineName + " -h' for usage.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  } else if((argc > 1) && (dcardfilename != NULL)) {
    printLine = cmdlineName + " - Too many arguments specified; you only need an optional address offset and a dcard file.\n";
    ecmdOutputError(printLine.c_str());
    printLine = cmdlineName + " - Type '" + cmdlineName + " -h' for usage.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }
  //Setup the target that will be used to query the system config 
  // Memctrl DA is on the cage depth and proc/dma are on the processor pos depth
  if (memMode == ECMD_MEM_MEMCTRL) {
    target.cageState = ECMD_TARGET_FIELD_WILDCARD;
    target.nodeState = target.chipTypeState = target.slotState = target.posState = target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;
  } else if ((memMode == ECMD_MEM_PROC) || (memMode == ECMD_MEM_DMA) || (memMode == ECMD_SRAM)) {
    target.chipType = ECMD_CHIPT_PROCESSOR;
    target.chipTypeState =   ECMD_TARGET_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
    target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;
  }


  // Get the address
  if ((dcardfilename == NULL) || (dcardfilename != NULL && argv[0] != NULL)) {
   if (!ecmdIsAllHex(argv[0])) {
     printLine = cmdlineName + " - Non-hex characters detected in address field\n";
     ecmdOutputError(printLine.c_str());
     return ECMD_INVALID_ARGS;
   }
#ifdef _LP64
   match = sscanf(argv[0], "%lx", &address);
#else
   match = sscanf(argv[0], "%llx", &address);
#endif
   if (match != 1) {
    ecmdOutputError("Error occurred processing address!\n");
    return ECMD_INVALID_ARGS;
   }
  }
  
  
  // Read in the input data
  if(filename != NULL) {
    rc = inputData.readFile(filename, ECMD_SAVE_FORMAT_BINARY_DATA);
    if (rc) {
     printLine = cmdlineName + " - Problems occurred parsing input data from file " + filename +", must be an invalid format\n";
     ecmdOutputError(printLine.c_str());
     return rc;
    }
    memEntry.address = address;
    memEntry.data = inputData;
    memdata.push_back(memEntry);
    
  } else if(dcardfilename != NULL) {
    rc = ecmdReadDcard(dcardfilename, memdata, address);
    if (rc) {
     printLine = cmdlineName + " - Problems occurred parsing input data from file " + dcardfilename +", must be an invalid format\n";
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
   memEntry.address = address;
   memEntry.data = inputData;
   memdata.push_back(memEntry);
  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/
  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    for (memdataIter = memdata.begin(); memdataIter != memdata.end(); memdataIter++) {

      /* Let's verify we have an even byte length of data */
      if (memdataIter->data.getBitLength() != (memdataIter->data.getByteLength() * 8)) {
        printLine = cmdlineName + " - Invalid data, must specify an even byte length of data\n";
        ecmdOutputError(printLine.c_str());
        rc = ECMD_INVALID_ARGS;
        break;
      } else if (memdataIter->data.getByteLength() == 0) {
        printLine = cmdlineName + " - Invalid data, byte length of zero detected on incoming data\n";
        ecmdOutputError(printLine.c_str());
        rc = ECMD_INVALID_ARGS;
        break;
      }        


      if (memMode == ECMD_MEM_DMA) {
        rc = putMemDma(target, memdataIter->address, memdataIter->data.getByteLength(), memdataIter->data);
      } else if (memMode == ECMD_MEM_MEMCTRL) {
        rc = putMemMemCtrl(target, memdataIter->address, memdataIter->data.getByteLength(), memdataIter->data);
      } else if (memMode == ECMD_MEM_PROC) {
        rc = putMemProc(target, memdataIter->address, memdataIter->data.getByteLength(), memdataIter->data);
      } else if (memMode == ECMD_SRAM) {
        rc = putSram(target, channel, memdataIter->address, memdataIter->data.getByteLength(), memdataIter->data);
      }

      if (rc) {
        printLine = cmdlineName + " - Error occured performing " + cmdlineName + " on ";
        printLine += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printLine.c_str() );
        coeRc = rc;
        continue;
      }
      else {
        validPosFound = true;     
      }
    }

    // Write out who we wrote too
    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      printLine = ecmdWriteTarget(target) + "\n";
      ecmdOutput(printLine.c_str());
    }
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound && !rc) {
    printLine = cmdlineName + " - Unable to find a valid chip to execute command on\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdCacheFlushUser(int argc, char* argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;
  
  ecmdChipTarget target;                ///< Current target being operated on
  ecmdLooperData looperData;            ///< Store internal Looper data
  ecmdChipTarget cuTarget;              ///< Current target being operated on
  ecmdLooperData cuLooperData;          ///< Store internal Looper data
  bool validPosFound = false;           ///< Did the looper find anything?
  std::string cacheTypeStr;             ///< User input for the cache to be flushed
  std::string printed;                  ///< Output data
  ecmdCacheType_t cacheType;            ///< cache type to be flushed
  ecmdCacheData queryCacheData;
  uint8_t oneLoop = 0;                  ///< Used to break out of the chipUnit loop after the first pass for non chipUnit operations

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
  if (argc < 2) {  
    ecmdOutputError("cacheflush - Too few arguments specified; you need at least a chip and a cachetype.\n");
    ecmdOutputError("cacheflush - Type 'cacheflush -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }


  if (argc > 2) {
    ecmdOutputError("cacheflush - Too many arguments specified; you probably added an option that wasn't recognized.\n");
    ecmdOutputError("cacheflush - Type 'cacheflush -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;
  
  //get the cachetype
  cacheTypeStr = argv[1];
  //Make the cachetype lower case
  transform(cacheTypeStr.begin(), cacheTypeStr.end(), cacheTypeStr.begin(), (int(*)(int)) tolower);
  
  if (cacheTypeStr == "l1i") {
    cacheType = ECMD_CACHE_LEVEL1I;
  } else if (cacheTypeStr == "l1d") {
    cacheType = ECMD_CACHE_LEVEL1D;
  } else if (cacheTypeStr == "l2") {
    cacheType = ECMD_CACHE_LEVEL2;
  } else if (cacheTypeStr == "l3") {
    cacheType = ECMD_CACHE_LEVEL3;
  } else if (cacheTypeStr == "l4") {
    cacheType = ECMD_CACHE_LEVEL4;
  } else { 
    ecmdOutputError("cacheflush - Unknown cache type specified.\n");
    ecmdOutputError("cacheflush - Type 'cacheflush -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }


  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;


  while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {

    /* Query for info about the cache so we can loop properly */
    rc = ecmdQueryCache(target, cacheType, queryCacheData);
    if (rc) {
      printed = "cacheflush - Error occurred performing ecmdQueryCache on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }

    /* Setup our chipUnit looper if needed */
    cuTarget = target;
    if (queryCacheData.isChipUnitRelated) {
      /* Error check the chipUnit returned */
      if (!queryCacheData.isChipUnitMatch(chipUnitType)) {
        printed = "cacheflush - Provided chipUnit \"";
        printed += chipUnitType;
        printed += "\" doesn't match chipUnit returned by queryCache \"";
        printed += queryCacheData.relatedChipUnit + "\"\n";
        ecmdOutputError(printed.c_str());
        rc = ECMD_INVALID_ARGS;
        break;
      }
      /* If we have a chipUnit, set the state fields properly */
      if (chipUnitType != "") {
        cuTarget.chipUnitType = chipUnitType;
        cuTarget.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
      }
      cuTarget.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
      cuTarget.threadState = ECMD_TARGET_FIELD_UNUSED;

      /* Init the chipUnit loop */
      rc = ecmdLooperInit(cuTarget, ECMD_SELECTED_TARGETS_LOOP, cuLooperData);
      if (rc) break;
    } else { // !queryCacheData.isChipUnitRelated
      if (chipUnitType != "") {
        printed = "cacheflush - A chipUnit \"";
        printed += chipUnitType;
        printed += "\" was given on a non chipUnit scom address.\n";
        ecmdOutputError(printed.c_str());
        rc = ECMD_INVALID_ARGS;
        break;
      }
      // Setup the variable oneLoop variable for this non-chipUnit case
      oneLoop = 1;
    }

    /* If this isn't a chipUnit scom we will fall into while loop and break at the end, if it is we will call run through configloopernext */
    while ((queryCacheData.isChipUnitRelated ? ecmdLooperNext(cuTarget, cuLooperData) : (oneLoop--)) && (!coeRc || coeMode)) {

      rc = ecmdCacheFlush(cuTarget, cacheType);
      if (rc) {
        printed = "cacheflush - Error occured performing cacheflush on ";
        printed += ecmdWriteTarget(cuTarget);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
      } else {
        validPosFound = true;     
      }

      printed = ecmdWriteTarget(cuTarget);
      printed += "\n";
      ecmdOutput( printed.c_str() );
    }
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("cacheflush - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }
  
  return rc;
}

uint32_t ecmdGetMemPbaUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string outputformat = "mem";     ///< Output format - default to 'mem'
  ecmdDataBuffer returnData;            ///< Buffer to hold return data from memory
  bool validPosFound = false;           ///< Did the looper find anything?
  ecmdChipTarget target;                ///< Current target being operated on
  uint64_t address;                     ///< The address from the command line
  uint32_t numBytes = ECMD_UNSET;       ///< Number of bytes from the command line
  std::string cmdlineName;              ///< Stores the name of what the command line function would be.
  int match;                            ///< For sscanf
  std::string printLine;                ///< Output data

  bool expectFlag = false;              ///< Are we doing an expect?
  bool maskFlag = false;                ///< Are we masking our expect data?
  char* expectPtr = NULL;               ///< Pointer to expected data in arg list
  char* maskPtr = NULL;                 ///< Pointer to mask data in arg list
  ecmdDataBuffer expected;                      ///< Buffer to store expected data
  ecmdDataBuffer mask;                          ///< Buffer for mask of expected data
  std::string inputformat = "x";                ///< Input format of data


  /************************************************************************/
  /* Setup the cmdlineName variable                                       */
  /************************************************************************/
  cmdlineName = "getmempba";

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/

  /* get format flag, if it's there */
  char * outputFormatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (outputFormatPtr != NULL) {
    outputformat = outputFormatPtr;
  }  
  char * inputFormatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (inputFormatPtr != NULL) {
    inputformat = inputFormatPtr;
  }


  /* Get the filename if -fb is specified */
  char * filename = ecmdParseOptionWithArgs(&argc, &argv, "-fb");
  
  /* Get the filename if -fd is specified */
  char * dcardfilename = ecmdParseOptionWithArgs(&argc, &argv, "-fd");

  if(((filename != NULL) || (dcardfilename != NULL)) && 
     ((outputFormatPtr != NULL) || (inputFormatPtr != NULL))) {
    printLine = cmdlineName + " - Options -f and -o/-i can't be specified together for format. Specify either one.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  } 
  if((dcardfilename != NULL) && (filename != NULL)) {
    printLine = cmdlineName + "Options -fb and -fd can't be specified together for format. Specify either one.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }

  //expect and mask flags check
  if (filename == NULL && dcardfilename == NULL) {
    if ((expectPtr = ecmdParseOptionWithArgs(&argc, &argv, "-exp")) != NULL) {
      expectFlag = true;
      
      if ((maskPtr = ecmdParseOptionWithArgs(&argc, &argv, "-mask")) != NULL) {
	maskFlag = true;
      }
    }
  } else {
    // If we are passing in data with a file, just look for -exp
    expectFlag = ecmdParseOption(&argc, &argv, "-exp");
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  //Setup the target that will be used to query the system config
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = 
    target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  // Read in the expect data
  if (expectFlag) {
    if ((filename == NULL) && (dcardfilename == NULL)) {

      rc = ecmdReadDataFormatted(expected, expectPtr, inputformat);
      if (rc) {
	ecmdOutputError((cmdlineName + " - Problems occurred parsing expected data, must be an invalid format\n").c_str());
	return rc;
      }

      if (maskFlag) {
	rc = ecmdReadDataFormatted(mask, maskPtr, inputformat);
	if (rc) {
	  ecmdOutputError((cmdlineName + " - Problems occurred parsing mask data, must be an invalid format\n").c_str());
	  return rc;
	}
	
      }

    } else {
      // Read from a file 
      if (filename != NULL) {
	rc = expected.readFile(filename, ECMD_SAVE_FORMAT_BINARY_DATA);
	if (rc) {
	  ecmdOutputError((cmdlineName + " - Problems occurred parsing expected data from file " + filename + " , must be an invalid format\n").c_str());
	  return rc;
	}
      } else {
	// Dcard
	// Pulled support for this as it is a pain, could be added if needed, 
	//   ecmdReadDcard returns a list of memory entries because there could 
	//   be holes, so would have to loop down below
	ecmdOutputError((cmdlineName + " - Currently Dcard support is not supported with -exp\n").c_str());
	return ECMD_INVALID_ARGS;
	//	rc = ecmdReadDcard(dcardfilename, expected);
	//	if (rc) {
	//	  printLine = cmdlineName + " - Problems occurred parsing input data from file " + dcardfilename +", must be an invalid format\n";
	//	  ecmdOutputError(printLine.c_str());
	//	  return rc;
	//	}

      }
      // Let's pull the length from the file
      numBytes = expected.getByteLength();
    }
	
  }

  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if ((numBytes == ECMD_UNSET) && (argc < 2)) {  //chip + address
    printLine = cmdlineName + " - Too few arguments specified; you need at least an address and number of bytes.\n";
    ecmdOutputError(printLine.c_str());
    printLine = cmdlineName + " - Type '" + cmdlineName + " -h' for usage.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  } else if (argc < 1) { 
    printLine = cmdlineName + " - Too few arguments specified; you need at least an address.\n";
    ecmdOutputError(printLine.c_str());
    printLine = cmdlineName + " - Type '" + cmdlineName + " -h' for usage.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }

  // Get the address
  if (!ecmdIsAllHex(argv[0])) {
    printLine = cmdlineName + " - Non-hex characters detected in address field\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }
#ifdef _LP64
  match = sscanf(argv[0], "%lx", &address);
#else
  match = sscanf(argv[0], "%llx", &address);
#endif
  if (match != 1) {
    ecmdOutputError("Error occurred processing address!\n");
    return ECMD_INVALID_ARGS;
  }

  // Get the number of bytes
  if (numBytes == ECMD_UNSET) {
    numBytes = (uint32_t)atoi(argv[1]);
  }


  // do not allow numBytes >= 512MB
  if (numBytes >= 0x20000000) {
    printLine = cmdlineName + " - Number of bytes must be < 512MB\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }


  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    rc = getMemPba(target, address, numBytes, returnData);

    if (rc) {
      printLine = cmdlineName + " - Error occured performing " + cmdlineName + " on ";
      printLine += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printLine.c_str() );
      coeRc = rc;
      continue;
    }
    else {
      validPosFound = true;     
    }
    
    printLine = ecmdWriteTarget(target);
    if (expectFlag) {
      uint32_t mismatchBit = 0;

      if (maskFlag) {
        returnData.setAnd(mask, 0, returnData.getBitLength());
      }

      if (!ecmdCheckExpected(returnData, expected, mismatchBit)) {

        //@ make this stuff sprintf'd
        char outstr[300];
        printLine = ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printLine.c_str() );
        if (mismatchBit != ECMD_UNSET) {
          sprintf(outstr, "%s - Data miscompare occured at (address %llX) (bit %d) (byte %d:0x%X bit %d)\n", 
                  cmdlineName.c_str(), (unsigned long long)address, mismatchBit, mismatchBit/8,
                  mismatchBit/8, mismatchBit%8);
          ecmdOutputError( outstr );
        }
        coeRc = ECMD_EXPECT_FAILURE;
        continue;
      }
    } else {
      if (filename != NULL) {
	rc = returnData.writeFile(filename, ECMD_SAVE_FORMAT_BINARY_DATA);
       
	if (rc) {
	  printLine += cmdlineName + " - Problems occurred writing data into file" + filename +"\n";
	  ecmdOutputError(printLine.c_str());
          break;
	}
	ecmdOutput( printLine.c_str() );
      } else if (dcardfilename != NULL) {
	std::string dataStr = ecmdWriteDataFormatted(returnData, "memd", address);
	std::ofstream ops;
	ops.open(dcardfilename);
	if (ops.fail()) {
	  char mesg[1000];
	  sprintf(mesg, "Unable to open file : %s for write", dcardfilename);
	  ecmdOutputError(mesg);
	  return ECMD_DBUF_FOPEN_FAIL;
	}
	if (dataStr[0] != '\n') {
	  printLine += "\n";
	}
	printLine += dataStr;
	ops << printLine.c_str();
	ops.close();

      } else  {
     
	std::string dataStr = ecmdWriteDataFormatted(returnData, outputformat, address);
	if (dataStr[0] != '\n') {
	  printLine += "\n";
	}
	printLine += dataStr;
	ecmdOutput( printLine.c_str() );
      }
    }  
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    printLine = cmdlineName + " - Unable to find a valid chip to execute command on\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdPutMemPbaUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string inputformat = "x";        ///< Output format - default to 'mem'
  ecmdDataBuffer inputData;             ///< Buffer to hold the data intended for memory
  std::list<ecmdMemoryEntry> memdata;   ///< Data from the D-Card format file 
  std::list<ecmdMemoryEntry>::iterator memdataIter; ///< to iterate on memdata list 
  ecmdMemoryEntry memEntry;             ///< to store data from the user
  bool validPosFound = false;           ///< Did the looper find anything?
  ecmdChipTarget target;                ///< Current target being operated on
  uint64_t address = 0;                 ///< The address from the command line
  std::string cmdlineName;              ///< Stores the name of what the command line function would be.
  int match;                            ///< For sscanf
  std::string printLine;                ///< Output data
  uint32_t pbaMode = PBA_MODE_LCO;  ///< Procedure mode to use - default to lco

  /************************************************************************/
  /* Setup the cmdlineName variable                                       */
  /************************************************************************/
  cmdlineName = "putmempba";

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
 
  /* Get the filename to file in D-Card format */ 
  char *dcardfilename = ecmdParseOptionWithArgs(&argc, &argv, "-fd");

  if(((filename != NULL) || (dcardfilename != NULL)) && (formatPtr != NULL) ) {
    printLine = cmdlineName + "Options -f and -i can't be specified together for format. Specify either one.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  } 

  if((dcardfilename != NULL) && (filename != NULL)) {
    printLine = cmdlineName + "Options -fb and -fd can't be specified together for format. Specify either one.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }

  char * modestring = ecmdParseOptionWithArgs(&argc, &argv, "-mode");
  if (modestring != NULL) {
    if (strncmp(modestring, "inj", 3) == 0) {
      pbaMode = PBA_MODE_INJECT;
    } else if (strcmp(modestring, "lco") == 0) {
      pbaMode = PBA_MODE_LCO;
    } else if (strcmp(modestring, "dma") == 0) {
      pbaMode = PBA_MODE_DMA;
    } else {
      printLine = cmdlineName + "Unknown mode option. Valid options are lco dma or inj.\n";
      ecmdOutputError(printLine.c_str());
      return ECMD_INVALID_ARGS;
    }
  }
  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  if ( (argc < 2)&&((filename == NULL) && (dcardfilename == NULL)) ) {  //chip + address
    printLine = cmdlineName + " - Too few arguments specified; you need at least an address and data to write.\n";
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
  } else if(((argc > 2)&&((filename == NULL) && (dcardfilename == NULL))) || ((argc > 1)&&(filename != NULL))) {
    printLine = cmdlineName + " - Too many arguments specified; you only need an address and input data|file.\n";
    ecmdOutputError(printLine.c_str());
    printLine = cmdlineName + " - Type '" + cmdlineName + " -h' for usage.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  } else if((argc > 1) && (dcardfilename != NULL)) {
    printLine = cmdlineName + " - Too many arguments specified; you only need an optional address offset and a dcard file.\n";
    ecmdOutputError(printLine.c_str());
    printLine = cmdlineName + " - Type '" + cmdlineName + " -h' for usage.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }
  //Setup the target that will be used to query the system config 
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  if (pbaMode == PBA_MODE_LCO) {
    target.chipUnitType = "ex";
    target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
    target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
    target.threadState = ECMD_TARGET_FIELD_UNUSED;
  } else {
    target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;
  }


  // Get the address
  if ((dcardfilename == NULL) || (dcardfilename != NULL && argv[0] != NULL)) {
   if (!ecmdIsAllHex(argv[0])) {
     printLine = cmdlineName + " - Non-hex characters detected in address field\n";
     ecmdOutputError(printLine.c_str());
     return ECMD_INVALID_ARGS;
   }
#ifdef _LP64
   match = sscanf(argv[0], "%lx", &address);
#else
   match = sscanf(argv[0], "%llx", &address);
#endif
   if (match != 1) {
    ecmdOutputError("Error occurred processing address!\n");
    return ECMD_INVALID_ARGS;
   }
  }
  
  
  // Read in the input data
  if(filename != NULL) {
    rc = inputData.readFile(filename, ECMD_SAVE_FORMAT_BINARY_DATA);
    if (rc) {
     printLine = cmdlineName + " - Problems occurred parsing input data from file " + filename +", must be an invalid format\n";
     ecmdOutputError(printLine.c_str());
     return rc;
    }
    memEntry.address = address;
    memEntry.data = inputData;
    memdata.push_back(memEntry);
    
  } else if(dcardfilename != NULL) {
    rc = ecmdReadDcard(dcardfilename, memdata, address);
    if (rc) {
     printLine = cmdlineName + " - Problems occurred parsing input data from file " + dcardfilename +", must be an invalid format\n";
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
   memEntry.address = address;
   memEntry.data = inputData;
   memdata.push_back(memEntry);
  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/
  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    for (memdataIter = memdata.begin(); memdataIter != memdata.end(); memdataIter++) {

      /* Let's verify we have an even byte length of data */
      if (memdataIter->data.getBitLength() != (memdataIter->data.getByteLength() * 8)) {
        printLine = cmdlineName + " - Invalid data, must specify an even byte length of data\n";
        ecmdOutputError(printLine.c_str());
        rc = ECMD_INVALID_ARGS;
        break;
      } else if (memdataIter->data.getByteLength() == 0) {
        printLine = cmdlineName + " - Invalid data, byte length of zero detected on incoming data\n";
        ecmdOutputError(printLine.c_str());
        rc = ECMD_INVALID_ARGS;
        break;
      }        


      rc = putMemPba(target, memdataIter->address, memdataIter->data.getByteLength(), memdataIter->data, pbaMode);

      if (rc) {
        printLine = cmdlineName + " - Error occured performing " + cmdlineName + " on ";
        printLine += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printLine.c_str() );
        coeRc = rc;
        continue;
      }
      else {
        validPosFound = true;     
      }
    }

    // Write out who we wrote too
    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      printLine = ecmdWriteTarget(target) + "\n";
      ecmdOutput(printLine.c_str());
    }
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound && !rc) {
    printLine = cmdlineName + " - Unable to find a valid chip to execute command on\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

#endif // ECMD_REMOVE_MEMORY_FUNCTIONS

