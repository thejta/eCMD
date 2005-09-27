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

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder     Description                       
//  ---- -------- ---- -------- -----     -----------------------------
//  @01  STG4466       03/10/05 Prahl     Fix up Beam errors
//   
// End Change Log *****************************************************

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
  uint32_t rc = ECMD_SUCCESS, getspyrc = ECMD_SUCCESS;

  bool expectFlag = false;
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdLooperData corelooper;            ///< Store internal Looper data for the core loop
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
  ecmdChipTarget coretarget;            ///< Current target being operated on for the cores
  bool isCoreSpy;                       ///< Is this a core spy ?
  std::list<ecmdSpyData> spyDataList;   ///< Spy information returned by ecmdQuerySpy
  ecmdSpyData spyData;                  ///< Spy information returned by ecmdQuerySpy
  std::list<ecmdSpyGroupData> spygroups; ///< Spygroups information returned by GetSpyGroups
  bool enabledCache = false;            ///< Did we enable the cache ?

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
  //Check verbose option
  bool verbose = ecmdParseOption(&argc, &argv, "-v");
  
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
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get spy name
  std::string spyName = argv[1];
  uint32_t startBit = 0x0, numBits = 0x0;


  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  /* We are going to enable ring caching to speed up performance */
  if (!ecmdIsRingCacheEnabled()) {
    enabledCache = true;
    ecmdEnableRingCache();
  }



  while ( ecmdConfigLooperNext(target, looperdata) ) {

    rc = ecmdQuerySpy(target, spyDataList, spyName.c_str(), ECMD_QUERY_DETAIL_LOW);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    } else if (rc || spyDataList.empty()) {
      printed = "getspy - Error occured looking up data on spy " + spyName + " on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    spyData = *(spyDataList.begin());
    isCoreSpy = spyData.isCoreRelated;

    /* Ok, we need to find out what type of spy we are dealing with here, to find out how to output */
    if ((outputformat == "default") || (inputformat == "default")) {

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

    /* Setup our Core looper if needed */
    coretarget = target;
    if (isCoreSpy) {
      coretarget.chipTypeState = coretarget.cageState = coretarget.nodeState = coretarget.slotState = coretarget.posState = ECMD_TARGET_FIELD_VALID;
      coretarget.coreState = ECMD_TARGET_FIELD_WILDCARD;
      coretarget.threadState = ECMD_TARGET_FIELD_UNUSED;

      /* Init the core loop */
      rc = ecmdConfigLooperInit(coretarget, ECMD_SELECTED_TARGETS_LOOP, corelooper);
      if (rc) return rc;
    }

    /* If this isn't a core ring we will fall into while loop and break at the end, if it is we will call run through configloopernext */
    while (!isCoreSpy ||
           ecmdConfigLooperNext(coretarget, corelooper)) {

      if (outputformat == "enum") {
        rc = getSpyEnum(coretarget, spyName.c_str(), enumValue);
      }
      else {
        rc = getSpy(coretarget, spyName.c_str(), spyBuffer);
      }

      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
      } else if (rc == ECMD_SPY_FAILED_ECC_CHECK) {
        if (spyData.epCheckers.empty()) {
          ecmdOutputError("getspy - Got back the Spy Failed ECC return code, but no epcheckers specified\n");
        }
        ecmdDataBuffer inLatches, outLatches, errorMask;
        std::list<std::string>::iterator epcheckersIter = spyData.epCheckers.begin();
        int flag=0;
        printed = "getspy - epcheckers \"";
        while (epcheckersIter != spyData.epCheckers.end()) {
          rc = getSpyEpCheckers(coretarget, epcheckersIter->c_str(), inLatches, outLatches, errorMask);
          if(errorMask.getNumBitsSet(0,errorMask.getBitLength())) {
            if(flag) 
              printed += ", ";
            printed += *epcheckersIter;
            flag = 1;
          }
          epcheckersIter++;
        }
        printed += "\" mismatched on ";
        printed += ecmdWriteTarget(coretarget) + "\n";
        ecmdOutputError( printed.c_str() );
        if ( !verbose ) {
          ecmdOutputError("Use -v option to get the detailed failure information\n");
          return rc;
        } else {
          ecmdOutput("============================================================\n");
          getspyrc = rc;
        }
      } else if ((rc == ECMD_SPY_GROUP_MISMATCH) ) {
        printed = "getspy - Problems reading group spy - found a mismatch on ";
        printed += ecmdWriteTarget(coretarget) + "\n";
        ecmdOutputError( printed.c_str() );
        if(!verbose) {
          ecmdOutputError("Use -v option to get the detailed failure information\n"); 
          return(rc);
        } else {
          ecmdOutput("============================================================\n");
          getspyrc = rc;
        }
      } else if (rc) {
        printed = "getspy - Error occured performing getspy on ";
        printed += ecmdWriteTarget(coretarget) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }
      validPosFound = true;     

      printed = ecmdWriteTarget(coretarget) + " " + spyName;

      if (outputformat == "enum") {
        if (!expectFlag) {
          printed += "\n" + enumValue + "\n";
          ecmdOutput( printed.c_str() );
        } else {
          if (expectedEnum != enumValue) {
            printed =  "getspy - Mismatch found on spy : " + spyName + "\n";
            ecmdOutputError( printed.c_str() );
            printed =  "getspy - Actual                : " + enumValue + "\n";
            ecmdOutputError( printed.c_str() );
            printed =  "getspy - Expected              : " + expectedEnum + "\n";
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
            printed =  "getspy - Mismatch found on spy : " + spyName + "\n";
            ecmdOutputError( printed.c_str() );
            printed =  "getspy - Actual                : ";
            printed += ecmdWriteDataFormatted(buffer, outputformat);
            ecmdOutputError( printed.c_str() );

            printed =  "getspy - Expected              : ";
            printed += ecmdWriteDataFormatted(expectedRaw, outputformat);
            ecmdOutputError( printed.c_str() );
            return ECMD_EXPECT_FAILURE;
          }

        }

      }
      /* Check if verbose then print details */
      if (verbose) {
        /* Get Spy Groups */
        getSpyGroups(coretarget, spyName.c_str(), spygroups);
        if (rc && rc != ECMD_SPY_GROUP_MISMATCH && rc != ECMD_SPY_FAILED_ECC_CHECK) return rc;
        std::list<ecmdSpyGroupData>::iterator groupiter = spygroups.begin();
        ecmdOutput("===== GroupData information for this spy ");
        printed = spyName + ": =====\n";
        ecmdOutput(printed.c_str());
        int i =0;
        char gpnum[50];
        while (groupiter != spygroups.end()) {
          sprintf(gpnum,"%3.3d",i);
          printed = "        Group " + (std::string)gpnum + ": 0x" + groupiter->extractBuffer.genHexLeftStr() + "\n";
          ecmdOutput(printed.c_str());
          printed = "   Dead Bits Mask: 0x" + groupiter->deadbitsMask.genHexLeftStr() + "\n";
          ecmdOutput(printed.c_str());
          i++;
          groupiter++;
        }
        /* Ecc Checkers */
        /* Must have entries - setup variables and iterators, start looping */
        ecmdDataBuffer inLatches, outLatches, errorMask;
        std::list<std::string>::iterator epcheckersIter = spyData.epCheckers.begin();
        ecmdOutput("===== epcheckers information for this spy ");
        printed = spyName + ": =====\n";
        ecmdOutput(printed.c_str());
        while (epcheckersIter != spyData.epCheckers.end()) {
          rc = getSpyEpCheckers(coretarget, epcheckersIter->c_str(), inLatches, outLatches, errorMask);
          if (rc && rc != ECMD_SPY_FAILED_ECC_CHECK && rc != ECMD_SPY_GROUP_MISMATCH) return rc;
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

      }
      if(getspyrc) 
        return(getspyrc);     
      if (!isCoreSpy) break;
    } /* End CoreLooper */

  } /* End poslooper */

  if (enabledCache) {
    rc = ecmdDisableRingCache();
    if (rc) {
      ecmdOutputError("getspy - Problems disabling the ring cache\n");
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
  ecmdLooperData corelooper;            ///< Store internal Looper data for the core loop
  std::string inputformat = "default";  ///< Input data format
  std::string dataModifier = "insert";  ///< Default data modifier
  uint32_t startBit = 0;                //@01 add init 
  uint32_t numBits  = 0; 
  ecmdDataBuffer buffer;                ///< Buffer to hold input data
  ecmdDataBuffer spyBuffer;             ///< Buffer to hold current spy data
  ecmdChipTarget target;                ///< Current target
  ecmdChipTarget coretarget;            ///< Current target being operated on for the cores
  bool isCoreSpy;                       ///< Is this a core spy ?
  bool validPosFound = false;           ///< Did the looper find anything?
  std::string printed;                  ///< Output data
  std::list<ecmdSpyData> spyDataList;   ///< Spy information returned by ecmdQuerySpy
  ecmdSpyData spyData;                  ///< Spy information returned by ecmdQuerySpy
  bool enabledCache = false;            ///< Did we enable the cache ?

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
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get spy name
  std::string spyName = argv[1];

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;


  /* We are going to enable ring caching to speed up performance */
  if (!ecmdIsRingCacheEnabled()) {
    enabledCache = true;
    ecmdEnableRingCache();
  }

  while ( ecmdConfigLooperNext(target, looperdata) ) {

    /* Ok, we need to find out what type of spy we are dealing with here, to find out how to output */
    rc = ecmdQuerySpy(target, spyDataList, spyName.c_str(), ECMD_QUERY_DETAIL_LOW);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    } else if (rc || spyDataList.empty()) {
      printed = "putspy - Error occured looking up data on spy " + spyName + " on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }

    spyData = *(spyDataList.begin());
    isCoreSpy = spyData.isCoreRelated;
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

    /* Setup our Core looper if needed */
    coretarget = target;
    if (isCoreSpy) {
      coretarget.chipTypeState = coretarget.cageState = coretarget.nodeState = coretarget.slotState = coretarget.posState = ECMD_TARGET_FIELD_VALID;
      coretarget.coreState = ECMD_TARGET_FIELD_WILDCARD;
      coretarget.threadState = ECMD_TARGET_FIELD_UNUSED;

      /* Init the core loop */
      rc = ecmdConfigLooperInit(coretarget, ECMD_SELECTED_TARGETS_LOOP, corelooper);
      if (rc) return rc;
    }


    /* If this isn't a core ring we will fall into while loop and break at the end, if it is we will call run through configloopernext */
    while (!isCoreSpy ||
           ecmdConfigLooperNext(coretarget, corelooper)) {

      if (inputformat == "enum") {
        rc = putSpyEnum(coretarget, spyName.c_str(), argv[argc-1] );
      }
      else {
        rc = getSpy(coretarget, spyName.c_str(), spyBuffer);
      }

      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
      } else if ((rc == ECMD_SPY_GROUP_MISMATCH) && (numBits == (uint32_t) spyData.bitLength)) {
        /* We will go on if the user was going to write the whole spy anyway */
        ecmdOutputWarning("putspy - Problems reading group spy - found a mismatch - going ahead with write\n");
        rc = 0;
      } else if (rc == ECMD_SPY_GROUP_MISMATCH) {
        /* If the user was only going to write part of the spy we can't go on because we don't ahve valid data to merge with */
        printed = "putspy - Problems reading group spy - found a mismatch - to force write don't use start/numbits - on ";
        printed += ecmdWriteTarget(coretarget) + "\n";
        ecmdOutputError( printed.c_str() );
        ecmdOutputError("Use getspy with the -v option to get the detailed failure information\n");
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

        printed += ecmdWriteTarget(coretarget) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }

      validPosFound = true;     

      if (inputformat != "enum") {


        rc = ecmdApplyDataModifier(spyBuffer, buffer,  startBit, dataModifier);
        if (rc) return rc;

        rc = putSpy(coretarget, spyName.c_str(), spyBuffer);
        if (rc) {
          printed = "putspy - Error occured performing putspy on ";
          printed += ecmdWriteTarget(coretarget) + "\n";
          ecmdOutputError( printed.c_str() );
          return rc;
        }

      }

      /* Flush out the cache to actually write the data to the chips */
      if (enabledCache) {
        rc = ecmdFlushRingCache();
        if (rc) {
          printed = "putspy - Error occured flushing cache on ";
          printed += ecmdWriteTarget(coretarget) + "\n";
          ecmdOutputError( printed.c_str() );
          return rc;
        }
      }

      if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
        printed = ecmdWriteTarget(coretarget) + "\n";
        ecmdOutput(printed.c_str());
      }

      if (!isCoreSpy) break;
    } /* End CoreLooper */

  } /* End poslooper */

  if (enabledCache) {
    rc = ecmdDisableRingCache();
    if (rc) {
      ecmdOutputError("putspy - Problems disabling the ring cache\n");
      return rc;
    }
  }

  if (!validPosFound) {
    ecmdOutputError("putspy - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}


