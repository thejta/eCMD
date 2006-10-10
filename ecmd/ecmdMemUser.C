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
#include <stdio.h>
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

uint32_t ecmdGetMemUser(int argc, char * argv[], ECMD_DA_TYPE memMode) {
  uint32_t rc = ECMD_SUCCESS;

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

  //Setup the target that will be used to query the system config
  // Memctrl DA is on the cage depth and proc/dma are on the processor pos depth
  if (memMode == ECMD_MEM_MEMCTRL) {
    target.cageState = ECMD_TARGET_FIELD_WILDCARD;
    target.nodeState = target.chipTypeState = target.slotState = 
      target.posState = target.threadState = 
      target.coreState = ECMD_TARGET_FIELD_UNUSED;
  } else if ((memMode == ECMD_MEM_PROC) || (memMode == ECMD_MEM_DMA)) {
    target.chipType = ECMD_CHIPT_PROCESSOR;
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = 
      target.posState = ECMD_TARGET_FIELD_WILDCARD;
    target.threadState = target.coreState = ECMD_TARGET_FIELD_UNUSED;
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
#ifdef _LP64
	   sprintf(outstr, "%s - Data miscompare occured at (address %lX) (bit %d) (byte %d bit %d\n", 
		 cmdlineName.c_str(), address, mismatchBit, mismatchBit/8,
		   mismatchBit%8);
#else
	   sprintf(outstr, "%s - Data miscompare occured at (address %llX) (bit %d) (byte %d:0x%X bit %d)\n", 
		   cmdlineName.c_str(), address, mismatchBit, mismatchBit/8,
		   mismatchBit/8, mismatchBit%8);
#endif
	   ecmdOutputError( outstr );
	 }

     	 return ECMD_EXPECT_FAILURE;
	
      }

    } else {
      if (filename != NULL) {
	rc = returnData.writeFile(filename, ECMD_SAVE_FORMAT_BINARY_DATA);
       
	if (rc) {
	  printLine += cmdlineName + " - Problems occurred writing data into file" + filename +"\n";
	  ecmdOutputError(printLine.c_str()); 
	  return rc;
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
  }else if(((argc > 2)&&((filename == NULL) && (dcardfilename == NULL))) || (((argc > 1)&&(filename != NULL)) || ((argc > 0)&&(dcardfilename != NULL)))) {
    printLine = cmdlineName + " - Too many arguments specified; you only need an address and input data|file or just a dcard file.\n";
    ecmdOutputError(printLine.c_str());
    printLine = cmdlineName + " - Type '" + cmdlineName + " -h' for usage.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  
  }
  //Setup the target that will be used to query the system config 
  // Memctrl DA is on the cage depth and proc/dma are on the processor pos depth
  if (memMode == ECMD_MEM_MEMCTRL) {
    target.cageState = ECMD_TARGET_FIELD_WILDCARD;
    target.nodeState = target.chipTypeState = target.slotState = target.posState = target.threadState = target.coreState = ECMD_TARGET_FIELD_UNUSED;
  } else if ((memMode == ECMD_MEM_PROC) || (memMode == ECMD_MEM_DMA)) {
    target.chipType = ECMD_CHIPT_PROCESSOR;
    target.chipTypeState =   ECMD_TARGET_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
    target.threadState = target.coreState = ECMD_TARGET_FIELD_UNUSED;
  }


  // Get the address
  if (dcardfilename == NULL) {
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
    rc = ecmdReadDcard(dcardfilename, memdata);
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
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while ( ecmdConfigLooperNext(target, looperdata) ) {

    for (memdataIter = memdata.begin(); memdataIter != memdata.end(); memdataIter++) {

      /* Let's verify we have an even byte length of data */
      if (memdataIter->data.getBitLength() != (memdataIter->data.getByteLength() * 8)) {
        printLine = cmdlineName + " - Invalid data, must specify an even byte length of data\n";
        ecmdOutputError(printLine.c_str());
        return ECMD_INVALID_ARGS;
      } else if (memdataIter->data.getByteLength() == 0) {
        printLine = cmdlineName + " - Invalid data, byte length of zero detected on incoming data\n";
        ecmdOutputError(printLine.c_str());
        return ECMD_INVALID_ARGS;
      }        


      if (memMode == ECMD_MEM_DMA) {
        rc = putMemDma(target, memdataIter->address, memdataIter->data.getByteLength(), memdataIter->data);
      } else if (memMode == ECMD_MEM_MEMCTRL) {
        rc = putMemMemCtrl(target, memdataIter->address, memdataIter->data.getByteLength(), memdataIter->data);
      } else if (memMode == ECMD_MEM_PROC) {
        rc = putMemProc(target, memdataIter->address, memdataIter->data.getByteLength(), memdataIter->data);
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

uint32_t ecmdCacheFlushUser(int argc, char* argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  
  ecmdChipTarget target;                        ///< Current target being operated on
  bool validPosFound = false;                   ///< Did the looper find anything?
  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string cacheTypeStr;                        ///< User input for the cache to be flushed
  std::string printed;                          ///< Output data
  ecmdCacheType_t cacheType;                    ///< cache type to be flushed

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


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
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;
  
  //get the cachetype
  cacheTypeStr = argv[1];
  //Make the cachetype lower case
  transform(cacheTypeStr.begin(), cacheTypeStr.end(), cacheTypeStr.begin(), (int(*)(int)) tolower);
  
  if (cacheTypeStr == "l1i") {
    cacheType = ECMD_CACHE_LEVEL1I;
    target.coreState = ECMD_TARGET_FIELD_WILDCARD;      /// adjust looper for cores
  } else if (cacheTypeStr == "l1d") {
    cacheType = ECMD_CACHE_LEVEL1D;
    target.coreState = ECMD_TARGET_FIELD_WILDCARD;      /// adjust looper for cores
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

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;


  while ( ecmdConfigLooperNext(target, looperdata) ) {
    rc = ecmdCacheFlush(target, cacheType);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "cacheflush - Error occured performing cacheflush on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }

    
    printed = ecmdWriteTarget(target);
    printed += "\n";
    ecmdOutput( printed.c_str() );
  }


  if (!validPosFound) {
    //this is an error common across all UI functions
    ecmdOutputError("cacheflush - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  
  return rc;
}


