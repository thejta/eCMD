/* $Header$ */
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
//  @01  STG4466       07/26/07 hjh       coe
//   
// End Change Log *****************************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <stdio.h>
#include <ctype.h>

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
#ifndef ECMD_REMOVE_SPY_FUNCTIONS
uint32_t ecmdGetSpyUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  bool expectFlag = false;
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdLooperData cuLooper;              ///< Store internal Looper data for the chipUnit loop
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
  ecmdChipTarget cuTarget;              ///< Current target being operated on for the chipUnits
  bool isChipUnitSpy;                   ///< Is this a chipUnit spy ?
  std::list<ecmdSpyData> spyDataList;   ///< Spy information returned by ecmdQuerySpy
  ecmdSpyData spyData;                  ///< Spy information returned by ecmdQuerySpy
  std::list<ecmdSpyGroupData> spygroups; ///< Spygroups information returned by GetSpyGroups
  bool enabledCache = false;            ///< Did we enable the cache ?
  ecmdQueryDetail_t detail = ECMD_QUERY_DETAIL_LOW;  ///< Should we get all the possible info about this spy?
  uint8_t oneLoop = 0;                      ///< Used to break out of the chipUnit loop after the first pass for non chipUnit operations

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

  if (verbose) {
    detail = ECMD_QUERY_DETAIL_HIGH;
  }
    
  
  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  /* Now done with args, do an error check */
  if (argc < 2) {  //chip + address
    ecmdOutputError("getspy - Too few arguments specified; you need at least a chip and a spy.\n");
    ecmdOutputError("getspy - Type 'getspy -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get spy name
  std::string spyName = argv[1];
  uint32_t startBit = 0x0, numBits = 0x0;

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdConfigLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    /* We are going to enable ring caching to speed up performance */
    /* Since we are in a target looper, the state fields should be set properly so just use this target */
    if (!ecmdIsRingCacheEnabled(target)) {
      rc = ecmdEnableRingCache(target);
      if (rc) {
        ecmdOutputError("getspy - ecmdEnableRingCache call failed!\n");
        coeRc = rc;
        continue;
      }
      enabledCache = true;
    }

    rc = ecmdQuerySpy(target, spyDataList, spyName.c_str(), detail);
    if (rc || spyDataList.empty()) {
      printed = "getspy - Error occured looking up data on spy " + spyName + " on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }

    spyData = *(spyDataList.begin());
    isChipUnitSpy = spyData.isChipUnitRelated;

    /* Make sure the user didn't request enum output on an ispy */
    if (((outputformat == "enum") || (inputformat == "enum")) && !spyData.isEnumerated) {
      ecmdOutputError("getspy - Spy doesn't support enumerations, can't use -ienum or -oenum\n");
      rc = ECMD_INVALID_ARGS;
      break;  
    }

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
      rc = ECMD_INVALID_ARGS;
      break;
    }

    /* Now that we know whether it is enumerated or not, we can finally finish our arg parsing */
    if (outputformat != "enum") {
      if (argc > 2) {
        if (!ecmdIsAllDecimal(argv[2])) {
          ecmdOutputError("getspy - Non-decimal numbers detected in startbit field\n");
          rc = ECMD_INVALID_ARGS;
          break;
        }
        startBit = (uint32_t)atoi(argv[2]);
      }
      else {
        startBit = 0x0;
      }

      if (argc > 3) {
        if (!ecmdIsAllDecimal(argv[3])) {
          ecmdOutputError("getspy - Non-decimal numbers detected in numbits field\n");
          rc = ECMD_INVALID_ARGS;
          break;
        }
        numBits = (uint32_t)atoi(argv[3]);
      }
      else {
        numBits = ECMD_UNSET;
      }

      if (argc > 4) {
        ecmdOutputError("getspy - Too many arguments specified; you probably added an option that wasn't recognized.\n");
        ecmdOutputError("getspy - Type 'getspy -h' for usage.\n");
        rc = ECMD_INVALID_ARGS;
        break;
      }

      /* Bounds check */
      if ((numBits != ECMD_UNSET) && (startBit + numBits) > ECMD_MAX_DATA_BITS) {
        char errbuf[100];
        sprintf(errbuf,"getspy - Too much data requested > %d bits\n", ECMD_MAX_DATA_BITS);
        ecmdOutputError(errbuf);
        rc = ECMD_DATA_BOUNDS_OVERFLOW;
        break;
      }

    } else if (argc > 2) {
      ecmdOutputError("getspy - Too many arguments specified; you probably added an option that wasn't recognized.\n");
      ecmdOutputError("getspy - It is also possible you specified <start> <numbits> with an enumerated alias or dial\n");
      ecmdOutputError("getspy - Type 'getspy -h' for usage.\n");
      rc = ECMD_INVALID_ARGS;
      break;
    }

    if (expectFlag) {
      if (inputformat != "enum") {
        rc = ecmdReadDataFormatted(expectedRaw, expectArg.c_str(), inputformat);
        if (rc) break;
      } else {
        expectedEnum = expectArg;
      }
    }

    /* Setup our chipUnit looper if needed */
    cuTarget = target;
    if (isChipUnitSpy) {
      cuTarget.chipTypeState = cuTarget.cageState = cuTarget.nodeState = cuTarget.slotState = cuTarget.posState = ECMD_TARGET_FIELD_VALID;
      /* Error check the chipUnit returned */
      if (spyData.relatedChipUnit != chipUnitType) {
        printed = "getspy - Provided chipUnit \"";
        printed += chipUnitType;
        printed += "\"doesn't match chipUnit returned by querySpy \"";
        printed += spyData.relatedChipUnit + "\"\n";
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
      rc = ecmdConfigLooperInit(cuTarget, ECMD_SELECTED_TARGETS_LOOP, cuLooper);
      if (rc) break;
    } else { // !isChipUnitSpy
      if (chipUnitType != "") {
        printed = "getspy - A chipUnit \"";
        printed += chipUnitType;
        printed += "\" was given on a non chipUnit spy\n";
        ecmdOutputError(printed.c_str());
        rc = ECMD_INVALID_ARGS;
        break;
      }
      // Setup the variable oneLoop variable for this non-chipUnit case
      oneLoop = 1;
    }

    /* If this isn't a chipUnit spy we will fall into while loop and break at the end, if it is we will call run through configloopernext */
    while ((isChipUnitSpy ? ecmdConfigLooperNext(cuTarget, cuLooper) : (oneLoop--)) && (!coeRc || coeMode)) {

      if (outputformat == "enum") {
        rc = getSpyEnum(cuTarget, spyName.c_str(), enumValue);
      }
      else {
        rc = getSpy(cuTarget, spyName.c_str(), spyBuffer);
      }

      if (rc == ECMD_SPY_FAILED_ECC_CHECK) {
        if (spyData.epCheckers.empty()) {
          ecmdOutputError("getspy - Got back the Spy Failed ECC return code, but no epcheckers specified\n");
        }
        ecmdDataBuffer inLatches, outLatches, errorMask;
        std::list<std::string>::iterator epcheckersIter = spyData.epCheckers.begin();
        int flag = 0;
        printed = "getspy - epcheckers \"";
        while (epcheckersIter != spyData.epCheckers.end()) {
          rc = getSpyEpCheckers(cuTarget, epcheckersIter->c_str(), inLatches, outLatches, errorMask);
          if (errorMask.getNumBitsSet(0,errorMask.getBitLength())) {
            if (flag) 
              printed += ", ";
            printed += *epcheckersIter;
            flag = 1;
          }
          epcheckersIter++;
        }
        printed += "\" mismatched on ";
        printed += ecmdWriteTarget(cuTarget) + "\n";
        ecmdOutputError( printed.c_str() );
        if (!verbose) {
          ecmdOutputError("Use -v option to get the detailed failure information\n");
          coeRc = rc;
          continue;
        } else {
          ecmdOutput("============================================================\n");
        }
      } else if (rc == ECMD_SPY_GROUP_MISMATCH) {
        printed = "getspy - Problems reading group spy - found a mismatch on ";
        printed += ecmdWriteTarget(cuTarget) + "\n";
        ecmdOutputError( printed.c_str() );
        if (!verbose) {
          ecmdOutputError("Use -v option to get the detailed failure information\n"); 
          coeRc = rc;
          continue;
        } else {
          ecmdOutput("============================================================\n");
        }
      } else if (rc) {
        printed = "getspy - Error occured performing getspy on ";
        printed += ecmdWriteTarget(cuTarget) + "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
      }
      validPosFound = true;     

      printed = ecmdWriteTarget(cuTarget) + " " + spyName;

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
            coeRc = ECMD_EXPECT_FAILURE;
            continue;
          }
        }
      }
      else {

        uint32_t bitsToFetch = 0x0;
        if (numBits == ECMD_UNSET) {
          bitsToFetch = spyBuffer.getBitLength() - startBit;
        }
        else {
          bitsToFetch = numBits;
        }

        buffer.setBitLength(bitsToFetch);
        spyBuffer.extract(buffer, startBit, bitsToFetch);

        char outstr[200];
        if (!expectFlag) {

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

          uint32_t mismatchBit = 0;
          if (!ecmdCheckExpected(buffer, expectedRaw, mismatchBit)) {
            printed =  "getspy - Mismatch found on spy : " + spyName + "\n";
            ecmdOutputError( printed.c_str() );

            if (mismatchBit != ECMD_UNSET) {
              sprintf(outstr, "First bit mismatch found at bit %d\n",startBit + mismatchBit);
              ecmdOutputError( outstr );
            }

            printed =  "getspy - Actual                : ";
            printed += ecmdWriteDataFormatted(buffer, outputformat);
            ecmdOutputError( printed.c_str() );

            printed =  "getspy - Expected              : ";
            printed += ecmdWriteDataFormatted(expectedRaw, outputformat);
            ecmdOutputError( printed.c_str() );
            coeRc =  ECMD_EXPECT_FAILURE;
            continue;
          }
        }
      }
      /* Check if verbose then print details */
      if (verbose) {
        /* Get Spy Groups */
        getSpyGroups(cuTarget, spyName.c_str(), spygroups);
        if (rc && rc != ECMD_SPY_GROUP_MISMATCH && rc != ECMD_SPY_FAILED_ECC_CHECK) {
          coeRc = rc;
          continue;
        }
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
          rc = getSpyEpCheckers(cuTarget, epcheckersIter->c_str(), inLatches, outLatches, errorMask);
          if (rc && rc != ECMD_SPY_FAILED_ECC_CHECK && rc != ECMD_SPY_GROUP_MISMATCH) {
            coeRc = rc;
            continue;
          }
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
        /* Spy Latch Data */
        std::list<ecmdSpyLatchData>::iterator latchDataIter = spyData.spyLatches.begin();
        ecmdOutput("===== latch information for this spy ");
        printed = spyName + ": =====\n";
        ecmdOutput(printed.c_str());
        char tempstr[50];
        uint32_t offset = 0;
        if (!spyData.isEnumerated) { // Can't do this on enumerated spies
          while (latchDataIter != spyData.spyLatches.end()) {
            // Format my data
            sprintf(tempstr,"   %s%-8s ", (((uint32_t)latchDataIter->length > 8) ? "0x" : "0b"), (((uint32_t)latchDataIter->length > 8) ? spyBuffer.genHexLeftStr(offset, (uint32_t)latchDataIter->length).c_str() : spyBuffer.genBinStr(offset, (uint32_t)latchDataIter->length).c_str()));
            printed = tempstr;
            // Now tack on the latch name
            printed += latchDataIter->latchName;
            printed += "\n";
            ecmdOutput(printed.c_str());

            // Walk some stuff
            offset += (uint32_t)latchDataIter->length;
            latchDataIter++;
          }
        }
        ecmdOutput("============================================================\n");
      }
    } /* End cuLooper */

    /* Now that we are moving onto the next target, let's flush the cache we have */
    if (enabledCache) {
      enabledCache = false;
      uint32_t trc = ecmdDisableRingCache(target);
      if (trc) {
        ecmdOutputError("getspy - Problems disabling the ring cache\n");
        coeRc = trc;
        continue;
      }
    }
  } /* End poslooper */
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  if (!validPosFound) {
    ecmdOutputError("getspy - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  if (enabledCache) {
    enabledCache = false;
    uint32_t trc = ecmdDisableRingCache(target);
    if (trc) {
      ecmdOutputError("getspy - Problems disabling the ring cache\n");
      coeRc = trc;
    }
  }

  return rc;
}

uint32_t ecmdPutSpyUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS , coeRc = ECMD_SUCCESS;


  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdLooperData cuLooper;              ///< Store internal Looper data for the chipUnit loop
  std::string inputformat = "default";  ///< Input data format
  std::string dataModifier = "insert";  ///< Default data modifier
  uint32_t startBit = ECMD_UNSET;       //@01 add init 
  uint32_t numBits  = 0; 
  ecmdDataBuffer buffer;                ///< Buffer to hold input data
  ecmdDataBuffer spyBuffer;             ///< Buffer to hold current spy data
  ecmdChipTarget target;                ///< Current target
  ecmdChipTarget cuTarget;              ///< Current target being operated on for the chipUnits
  bool isChipUnitSpy;                   ///< Is this a chipUnit spy ?
  bool validPosFound = false;           ///< Did the looper find anything?
  std::string printed;                  ///< Output data
  std::list<ecmdSpyData> spyDataList;   ///< Spy information returned by ecmdQuerySpy
  ecmdSpyData spyData;                  ///< Spy information returned by ecmdQuerySpy
  bool enabledCache = false;            ///< Did we enable the cache ?
  uint8_t oneLoop = 0;                      ///< Used to break out of the chipUnit loop after the first pass for non chipUnit operations

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



   /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode



  if (argc < 3) {  //chip + address
    ecmdOutputError("putspy - Too few arguments specified; you need at least a chip, a spy, and data.\n");
    ecmdOutputError("putspy - Type 'putspy -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get spy name
  std::string spyName = argv[1];

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdConfigLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    /* We are going to enable ring caching to speed up performance */
    if (!ecmdIsRingCacheEnabled(target)) {
      rc = ecmdEnableRingCache(target);
      if (rc) {
        ecmdOutputError("putspy - ecmdEnableRingCache call failed!\n");
        coeRc = rc;
        continue;
      }
      enabledCache = true;
    }

    /* Ok, we need to find out what type of spy we are dealing with here, to find out how to output */
    rc = ecmdQuerySpy(target, spyDataList, spyName.c_str(), ECMD_QUERY_DETAIL_LOW);
    if (rc || spyDataList.empty()) {
      printed = "putspy - Error occured looking up data on spy " + spyName + " on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }

    spyData = *(spyDataList.begin());
    isChipUnitSpy = spyData.isChipUnitRelated;
    if (spyData.isEnumerated) {
      if (inputformat == "default") inputformat = "enum";
    } else {
      if (inputformat == "default") inputformat = "x";
    }

    /* Now that we know whether it is enumerated or not, we can finally finish our arg parsing */
    if (inputformat != "enum") {
      if (argc > 3) {
        if (argc != 5) {
          ecmdOutputError("putspy - Too many arguments specified; you probably added an option that wasn't recognized.\n");
          ecmdOutputError("putspy - Type 'putspy -h' for usage.\n");
          rc = ECMD_INVALID_ARGS;
          break;
        }

        if (!ecmdIsAllDecimal(argv[2])) {
          ecmdOutputError("putspy - Non-decimal numbers detected in startbit field\n");
          rc = ECMD_INVALID_ARGS;
          break;
        }
        startBit = (uint32_t)atoi(argv[2]);

        if (!ecmdIsAllDecimal(argv[3])) {
          ecmdOutputError("putspy - Non-decimal numbers detected in numbits field\n");
          rc = ECMD_INVALID_ARGS;
          break;
        }
        numBits = (uint32_t)atoi(argv[3]);

        rc = ecmdReadDataFormatted(buffer, argv[4], inputformat, numBits);
        if (rc) {
          ecmdOutputError("putspy - Problems occurred parsing input data, must be an invalid format\n");
          break;
        }

      } else {
        rc = ecmdReadDataFormatted(buffer, argv[2], inputformat, spyData.bitLength);
        if (rc) {
          ecmdOutputError("putspy - Problems occurred parsing input data, must be an invalid format\n");
          break;
        }
      }
    }

    /* Setup our chipUnit looper if needed */
    cuTarget = target;
    if (isChipUnitSpy) {
      cuTarget.chipTypeState = cuTarget.cageState = cuTarget.nodeState = cuTarget.slotState = cuTarget.posState = ECMD_TARGET_FIELD_VALID;
      /* Error check the chipUnit returned */
      if (spyData.relatedChipUnit != chipUnitType) {
        printed = "putspy - Provided chipUnit \"";
        printed += chipUnitType;
        printed += "\"doesn't match chipUnit returned by querySpy \"";
        printed += spyData.relatedChipUnit + "\n";
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
      rc = ecmdConfigLooperInit(cuTarget, ECMD_SELECTED_TARGETS_LOOP, cuLooper);
      if (rc) break;
    } else { // !isChipUnitSpy
      if (chipUnitType != "") {
        printed = "putspy - A chipUnit \"";
        printed += chipUnitType;
        printed += "\" was given on a non chipUnit spy\n";
        ecmdOutputError(printed.c_str());
        rc = ECMD_INVALID_ARGS;
        break;
      }
      // Setup the variable oneLoop variable for this non-chipUnit case
      oneLoop = 1;
    }

    /* If this isn't a chipUnit spy we will fall into while loop and break at the end, if it is we will call run through configloopernext */
    while ((isChipUnitSpy ? ecmdConfigLooperNext(cuTarget, cuLooper) : (oneLoop--)) && (!coeRc || coeMode)) {

      if ((inputformat != "enum") && ((dataModifier != "insert") || (startBit != ECMD_UNSET))) {

        rc = getSpy(cuTarget, spyName.c_str(), spyBuffer);

        if ((rc == ECMD_SPY_GROUP_MISMATCH) && (numBits == (uint32_t) spyData.bitLength)) {
          /* We will go on if the user was going to write the whole spy anyway */
          ecmdOutputWarning("putspy - Problems reading group spy - found a mismatch - going ahead with write\n");
          rc = 0;
        } else if (rc == ECMD_SPY_GROUP_MISMATCH) {
          /* If the user was only going to write part of the spy we can't go on because we don't ahve valid data to merge with */
          printed = "putspy - Problems reading group spy - found a mismatch - to force write don't use start/numbits - on ";
          printed += ecmdWriteTarget(cuTarget) + "\n";
          ecmdOutputError( printed.c_str() );
          ecmdOutputError("Use getspy with the -v option to get the detailed failure information\n");
          coeRc = rc;
          continue;
        } else if (rc == ECMD_SPY_FAILED_ECC_CHECK) {
          ecmdOutputWarning("putspy - Problems reading spy - ECC check failed - going ahead with write\n");
          rc = 0;
        } else if (rc) {
          printed = "putspy - Error occured performing getspy on ";
          printed += ecmdWriteTarget(cuTarget) + "\n";
          ecmdOutputError( printed.c_str() );
          coeRc = rc;
          continue;
        } else {
          validPosFound = true;     
        }

        rc = ecmdApplyDataModifier(spyBuffer, buffer, (startBit == ECMD_UNSET ? 0 : startBit), dataModifier);
        if (rc) { 
          coeRc = rc;
          continue;
        }

        rc = putSpy(cuTarget, spyName.c_str(), spyBuffer);
        if (rc) {
          printed = "putspy - Error occured performing putspy on ";
          printed += ecmdWriteTarget(cuTarget) + "\n";
          ecmdOutputError( printed.c_str() );
          coeRc = rc;
          continue;
        }
      } else {

        if (inputformat == "enum") {
          rc = putSpyEnum(cuTarget, spyName.c_str(), argv[argc-1] );
        } else {
          rc = putSpy(cuTarget, spyName.c_str(), buffer);
        }

        if (rc) {
          printed = "putspy - Error occured performing putspy on ";
          printed += ecmdWriteTarget(cuTarget) + "\n";
          ecmdOutputError( printed.c_str() );
          coeRc = rc;
          continue;
        } else {
          validPosFound = true;
        }
      }

      if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
        printed = ecmdWriteTarget(cuTarget) + "\n";
        ecmdOutput(printed.c_str());
      }
    } /* End cuLooper */

    /* Now that we are moving onto the next target, let's flush the cache we have */
    if (enabledCache) {
      uint32_t trc = ecmdDisableRingCache(target);
      if (trc) {
        ecmdOutputError("putspy - Problems disabling the ring cache\n");
        coeRc = trc;
        continue;
      }
      enabledCache = false;
    }

  } /* End poslooper */
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  if (!validPosFound) {
    ecmdOutputError("putspy - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}
#endif // ECMD_REMOVE_SPY_FUNCTIONS

