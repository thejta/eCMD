// Copyright ***********************************************************
//                                                                      
// File ecmdSpyUser.C                                  
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
#define ecmdDaSpyUser_C
#include <stdio.h>
#include <ctype.h>

#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <ecmdInterpreter.H>
#include <ecmdSharedUtils.H>

#undef ecmdDaSpyUser_C
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

uint32_t ecmdGetSpyUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  bool expectFlag = false;
  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string outputformat = "default"; ///< Output format - default to 'enum' if enumerated otherwise 'x'
  std::string inputformat = "default";  ///< Expect data input format
  std::string expectArg;                ///< String containing expect data
  ecmdDataBuffer spyBuffer;             ///< Buffer to hold entire spy contents
  ecmdDataBuffer buffer;                ///< Buffer to hold user requested part of spy
  ecmdDataBuffer expectedRaw;           ///< Buffer to hold Raw expected data
  std::string expectedEnum;             ///< Buffer to hold enum expected data
  bool validPosFound = false;           ///< Did the looper find something?
  std::string printed;                  ///< Output string data
  std::string enumValue;                ///< The enum value returned
  ecmdChipTarget target;                ///< Current target being operated on
  ecmdSpyData spyData;                  ///< Spy information returned by ecmdQuerySpy

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  //expect and mask flags check
  char * expectArgTmp = ecmdParseOptionWithArgs(&argc, &argv, "-exp");
  if (expectArgTmp != NULL) {
    expectFlag = true;
    expectArg = expectArgTmp;
  }

  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr != NULL) {
    outputformat = formatPtr;
  }

  formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    inputformat = formatPtr;
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
    ecmdOutputError("getspy - Too few arguments specified; you need at least a chip and a spy.\n");
    ecmdOutputError("getspy - Type 'getspy -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get spy name
  std::string spyName = argv[1];
  uint32_t startBit = 0x0, numBits = 0x0;


  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  /* We are going to enable ring caching to speed up performance */
  ecmdEnableRingCache();



  while ( ecmdConfigLooperNext(target, looperdata) ) {

    /* Ok, we need to find out what type of spy we are dealing with here, to find out how to output */
    if ((outputformat == "default") || (inputformat == "default")) {
      rc = ecmdQuerySpy(target, spyData, spyName.c_str());
      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
      } else if (rc) {
        printed = "getspy - Error occured looking up data on spy " + spyName + " on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }

      if (spyData.isEnumerated) {
        if (outputformat == "default") outputformat = "enum";
        if (inputformat == "default") inputformat = "enum";
      } else {
        if (outputformat == "default") outputformat = "x";
        if (inputformat == "default") inputformat = "x";
      }
    } 
    if ((outputformat == "enum") && (inputformat != "enum")) {
      /* We can't do an expect on non-enumerated when they want a fetch of enumerated */
      ecmdOutputError("getspy - When reading enumerated spy's both input and output format's must be of type 'enum'\n");
      return ECMD_INVALID_ARGS;
    }


    /* Now that we know whether it is enumerated or not, we can finally finish our arg parsing */
    if (outputformat != "enum") {
      if (argc > 2) {
        if (!ecmdIsAllDecimal(argv[2])) {
          ecmdOutputError("getspy - Non-decimal numbers detected in startbit field\n");
          return ECMD_INVALID_ARGS;
        }
        startBit = atoi(argv[2]);
      }
      else {
        startBit = 0x0;
      }

      if (argc > 3) {
        if (!ecmdIsAllDecimal(argv[3])) {
          ecmdOutputError("getspy - Non-decimal numbers detected in numbits field\n");
          return ECMD_INVALID_ARGS;
        }
        numBits = atoi(argv[3]);
      }
      else {
        numBits = 0xFFFFFFFF;
      }
      if (argc > 4) {
        ecmdOutputError("getspy - Too many arguments specified; you probably added an option that wasn't recognized.\n");
        ecmdOutputError("getspy - Type 'getspy -h' for usage.\n");
        return ECMD_INVALID_ARGS;
      }
        

      /* Bounds check */
      if ((numBits != 0xFFFFFFFF) && (startBit + numBits) > ECMD_MAX_DATA_BITS) {
        char errbuf[100];
        sprintf(errbuf,"getspy - Too much data requested > %d bits\n", ECMD_MAX_DATA_BITS);
        ecmdOutputError(errbuf);
        return ECMD_DATA_BOUNDS_OVERFLOW;
      }

    } else if (argc > 2) {
      ecmdOutputError("getspy - Too many arguments specified; you probably added an option that wasn't recognized.\n");
      ecmdOutputError("getspy - It is also possible you specified <start> <numbits> with an enumerated alias or dial\n");
      ecmdOutputError("getspy - Type 'getspy -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }


    if (expectFlag) {
      if (inputformat != "enum")
        rc = ecmdReadDataFormatted(expectedRaw, expectArg.c_str(), inputformat);
      else
        expectedEnum = expectArg;
        
      if (rc) return rc;
    }


    if (outputformat == "enum") {
      rc = getSpyEnum(target, spyName.c_str(), enumValue);
    }
    else {
      rc = getSpy(target, spyName.c_str(), spyBuffer);
    }

    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    } else if (rc == ECMD_SPY_FAILED_ECC_CHECK) {
      if (spyData.epCheckers.empty()) {
        ecmdOutputError("getspy - Got back the Spy Failed ECC return code, but no epcheckers specified\n");
      }
      /* Must have entries - setup variables and iterators, start looping */
      ecmdDataBuffer inLatches, outLatches, errorMask;
      std::list<std::string>::iterator epcheckersIter = spyData.epCheckers.begin();
      ecmdOutput("===== The following epcheckers mismatched for this spy =====\n");
      while (epcheckersIter != spyData.epCheckers.end()) {
        rc = getSpyEpCheckers(target, epcheckersIter->c_str(), inLatches, outLatches, errorMask);
        if (rc && rc != ECMD_SPY_FAILED_ECC_CHECK) return rc;
        printed = *epcheckersIter + "\n";
        ecmdOutput(printed.c_str());
        printed = "   In Latches: 0x" + inLatches.genHexLeftStr() + "\n";
        ecmdOutput(printed.c_str());
        printed = "  Out Latches: 0x" + outLatches.genHexLeftStr() + "\n";
        ecmdOutput(printed.c_str());
        printed = "   Error Mask: 0x" + errorMask.genHexLeftStr() + "\n";
        ecmdOutput(printed.c_str());
        epcheckersIter++;
      }
      ecmdOutput("============================================================\n");
    } else if (rc) {
        printed = "getspy - Error occured performing getspy on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    validPosFound = true;     

    printed = ecmdWriteTarget(target) + " " + spyName;

    if (outputformat == "enum") {
      if (!expectFlag) {
        printed += "\n" + enumValue + "\n";
        ecmdOutput( printed.c_str() );
      } else {
        if (expectedEnum != enumValue) {
          printed =  "getspy - Actual            : " + enumValue + "\n";
          ecmdOutputError( printed.c_str() );
          printed =  "getspy - Expected          : " + expectedEnum + "\n";
          ecmdOutputError( printed.c_str() );
          return ECMD_EXPECT_FAILURE;
        }
      }
    }
    else {

      uint32_t bitsToFetch = 0x0;
      if (numBits == 0xFFFFFFFF) {
        bitsToFetch = spyBuffer.getBitLength() - startBit;
      }
      else {
        bitsToFetch = numBits;
      }

      buffer.setBitLength(bitsToFetch);
      spyBuffer.extract(buffer, startBit, bitsToFetch);

      if (!expectFlag) {

        char outstr[20];
        sprintf(outstr, "(%d:%d)", startBit, startBit + bitsToFetch - 1);
        printed += outstr;

        std::string dataStr = ecmdWriteDataFormatted(buffer, outputformat);
        if (dataStr[0] != '\n') {
          printed += "\n";
        }
        printed += dataStr;
        ecmdOutput( printed.c_str() );
      }
      else {

        if (!ecmdCheckExpected(buffer, expectedRaw)) {
          //@ make this stuff sprintf'd
          printed =  "getspy - Actual            : ";
          printed += ecmdWriteDataFormatted(buffer, outputformat);
          ecmdOutputError( printed.c_str() );

          printed =  "getspy - Expected          : ";
          printed += ecmdWriteDataFormatted(expectedRaw, outputformat);
          ecmdOutputError( printed.c_str() );
          return ECMD_EXPECT_FAILURE;
        }

      }

    }

    /* Flush out the cache we don't need to keep entries from the previous chips */
    rc = ecmdFlushRingCache();
    if (rc) {
      printed = "getspy - Error occured flushing cache on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
      

  }

  if (!validPosFound) {
    ecmdOutputError("getspy - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdPutSpyUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string inputformat = "default";  ///< Input data format
  std::string dataModifier = "insert";  ///< Default data modifier
  uint32_t startBit, numBits = 0;
  ecmdDataBuffer buffer;                ///< Buffer to hold input data
  ecmdDataBuffer spyBuffer;             ///< Buffer to hold current spy data
  ecmdChipTarget target;                ///< Current target
  bool validPosFound = false;           ///< Did the looper find anything?
  std::string printed;                  ///< Output data
  ecmdSpyData spyData;                  ///< Spy information returned by ecmdQuerySpy

  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    inputformat = formatPtr;
  }

  formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-b");
  if (formatPtr != NULL) {
    dataModifier = formatPtr;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc < 3) {  //chip + address
    ecmdOutputError("putspy - Too few arguments specified; you need at least a chip, a spy, and data.\n");
    ecmdOutputError("putspy - Type 'putspy -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get spy name
  std::string spyName = argv[1];

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;


  /* We are going to enable ring caching to speed up performance */
  ecmdEnableRingCache();

  while ( ecmdConfigLooperNext(target, looperdata) ) {

    /* Ok, we need to find out what type of spy we are dealing with here, to find out how to output */
    rc = ecmdQuerySpy(target, spyData, spyName.c_str());
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    } else if (rc) {
      printed = "putspy - Error occured looking up data on spy " + spyName + " on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }

    if (spyData.isEnumerated) {
      if (inputformat == "default") inputformat = "enum";
    } else {
      if (inputformat == "default") inputformat = "x";
    }

    /* Now that we know whether it is enumerated or not, we can finally finish our arg parsing */
    if (inputformat != "enum") {
      if (argc > 3) {
        if (!ecmdIsAllDecimal(argv[2])) {
          ecmdOutputError("putspy - Non-decimal numbers detected in startbit field\n");
          return ECMD_INVALID_ARGS;
        }
        startBit = atoi(argv[2]);
      }
      else {
        startBit = 0x0;
      }

      if (argc > 4) {
        if (!ecmdIsAllDecimal(argv[3])) {
          ecmdOutputError("putspy - Non-decimal numbers detected in numbits field\n");
          return ECMD_INVALID_ARGS;
        }
        numBits = atoi(argv[3]);
      }
      else {
        numBits = spyData.bitLength - startBit;
      }
      if (argc > 5) {
        ecmdOutputError("putspy - Too many arguments specified; you probably added an option that wasn't recognized.\n");
        ecmdOutputError("putspy - Type 'putspy -h' for usage.\n");
        return ECMD_INVALID_ARGS;
      }
      rc = ecmdReadDataFormatted(buffer, argv[argc-1] , inputformat, numBits);
      if (rc) {
        ecmdOutputError("putspy - Problems occurred parsing input data, must be an invalid format\n");
        return rc;
      }
        
    } else if (argc > 3) {
      ecmdOutputError("putspy - Too many arguments specified; you probably added an option that wasn't recognized.\n");
      ecmdOutputError("putspy - It is also possible you specified <start> <numbits> with an enumerated alias or dial\n");
      ecmdOutputError("putspy - Type 'putspy -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }


    if (inputformat == "enum") {
      rc = putSpyEnum(target, spyName.c_str(), argv[argc-1] );
    }
    else {
      rc = getSpy(target, spyName.c_str(), spyBuffer);
    }

    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    } else if ((rc == ECMD_SPY_GROUP_MISMATCH) && (numBits == spyData.bitLength)) {
      /* We will go on if the user was going to write the whole spy anyway */
      ecmdOutputWarning("putspy - Problems reading group spy - found a mismatch - going ahead with write\n");
      rc = 0;
    } else if (rc == ECMD_SPY_GROUP_MISMATCH) {
      /* If the user was only going to write part of the spy we can't go on because we don't ahve valid data to merge with */
      printed = "putspy - Problems reading group spy - found a mismatch - to force write don't use start/numbits - on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
      
    } else if (rc == ECMD_SPY_FAILED_ECC_CHECK) {
      ecmdOutputWarning("putspy - Problems reading spy - ECC check failed - going ahead with write\n");
      rc = 0;
    } else if (rc) {
      if (inputformat == "enum") {
        printed = "putspy - Error occured performing putspy (enumerated) on ";
      }
      else {
        printed = "putspy - Error occured performing getspy on ";
      }

      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }

    validPosFound = true;     

    if (inputformat != "enum") {


      rc = ecmdApplyDataModifier(spyBuffer, buffer,  startBit, dataModifier);
      if (rc) return rc;

      rc = putSpy(target, spyName.c_str(), spyBuffer);
      if (rc) {
        printed = "putspy - Error occured performing putspy on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }

    }

    /* Flush out the cache to actually write the data to the chips */
    rc = ecmdFlushRingCache();
    if (rc) {
      printed = "putspy - Error occured flushing cache on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }

    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      printed = ecmdWriteTarget(target) + "\n";
      ecmdOutput(printed.c_str());
    }


  }

  if (!validPosFound) {
    ecmdOutputError("putspy - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}


