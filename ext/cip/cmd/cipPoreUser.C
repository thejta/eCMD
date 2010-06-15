// Copyright ***********************************************************
//                                                                      
// File cipPoreUser.C                                   
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

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <ecmdClientCapi.H>
#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <ecmdSharedUtils.H>
#include <cipClientCapi.H>
#include <cipInterpreter.H>


//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------
#ifndef CIP_REMOVE_PORE_FUNCTIONS
uint32_t cipPorePutScomUser(int argc, char* argv[]) {

  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;
  std::string inputformat = "x";                ///< Default input format
  std::string dataModifier = "insert";          ///< Default data Modifier (And/Or/insert)
  ecmdLooperData looperData;                    ///< Store internal Looper data
  ecmdLooperData cuLooper;                      ///< Store internal Looper data for the chipUnit loop
  ecmdChipTarget target;                        ///< Chip target being operated on
  ecmdChipTarget cuTarget;                      ///< Current target being operated on for the chipUnit
  std::list<ecmdScomData> queryScomData;        ///< Scom data 
  std::list<ecmdScomData>::iterator scomData;   ///< Scom data 
  uint32_t address;                             ///< Scom address
  ecmdDataBuffer buffer;                        ///< Container to store write data
  ecmdDataBuffer mask;                          ///< Container to store write mask
  ecmdDataBuffer cmdlineBuffer;                 ///< Buffer to store data to be inserted
  bool validPosFound = false;                   ///< Did the config looper actually find a chip ?
  std::string printed;                          ///< String for printed data
  uint32_t startbit = ECMD_UNSET;               ///< Startbit to insert data
  uint32_t numbits = 0;                         ///< Number of bits to insert data
  uint8_t oneLoop = 0;                          ///< Used to break out of the chipUnit loop after the first pass for non chipUnit operations
  char* cmdlinePtr = NULL;                      ///< Pointer to data in argv array
  uint8_t argCount = 0;                         ///< Used for argument error checking

  uint32_t  poreWriteType = (CIP_PORE_SCOMINIT_SECTION | CIP_PORE_APPEND);
  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  char* formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    inputformat = formatPtr;
  }

  formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-b");
  if (formatPtr != NULL) {
    dataModifier = formatPtr;
  }

  /* Grab the modes we can run in - parse them here so ecmdCommandArgs doesn't grab the -a, -b, -n etc*/
  if (ecmdParseOption(&argc, &argv, "-append")) {
    argCount++;
    /* This is on by default, do nothing */
  }
  if (ecmdParseOption(&argc, &argv, "-replace")) {
    argCount++;
    poreWriteType &= 0xFFFFFFE0;   // turn off append
    poreWriteType |= CIP_PORE_REPLACE;
  } 
  if (ecmdParseOption(&argc, &argv, "-noop")) {
    argCount++;
    poreWriteType &= 0xFFFFFFE0;   // turn off append
    poreWriteType |= CIP_PORE_NOOP;
  } 
  if (dataModifier == "and"){
    argCount++;
    poreWriteType &= 0xFFFFFFE0;   // turn off append
    poreWriteType |= CIP_PORE_OVERLAY_AND;
  } 
  if (dataModifier == "or"){
    argCount++;
    poreWriteType &= 0xFFFFFFE0;   // turn off append
    poreWriteType |= CIP_PORE_OVERLAY_OR;
  }
  
  if (argCount > 1){
    ecmdOutputError("cipporeputscom - Too many poreWriteTypes were specified. Pick one: -append, -replace, -noop, -bor, -band.\n");
    ecmdOutputError("cipporeputscom - Type 'cipporeputscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  argCount = 0; //reset
  if (ecmdParseOption(&argc, &argv, "-scaninit")) {
    argCount++;
    poreWriteType &= 0xFFFFFFBF;   // turn off scominit 
    poreWriteType |= CIP_PORE_SCANINIT_SECTION;
  } 
  if (ecmdParseOption(&argc, &argv, "-scominit")) {
    argCount++;
    /* This is on by default, do nothing */
  }
  if (argCount > 1){
    ecmdOutputError("cipporeputscom - Too many poreWriteTypes were specified. Pick one: -scominit, -scaninit.\n");
    ecmdOutputError("cipporeputscom - Type 'cipporeputscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }


  /************************************************************************/
  /* Parse Common Cmdline Args,                                           */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  if (argc < 3) {  //chip + address + some data
    ecmdOutputError("cipporeputscom - Too few arguments specified; you need at least a chip, an address, and some data.\n");
    ecmdOutputError("cipporeputscom - Type 'cipporeputscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  if (!ecmdIsAllHex(argv[1])) {
    ecmdOutputError("cipporeputscom - Non-hex characters detected in address field\n");
    return ECMD_INVALID_ARGS;
  }

  // Check address field in argv[1] is less than 8 chars + NULL terminator since
  //  we are restricting this to a uint32_t
  if ( strlen(argv[1]) > 8 ) // strlen does NOT count NULL terminator
  {
    ecmdOutputError("cipporeputscom - Address field is too large (>8 chars). It is restricted to a uint32_t\n");
    return ECMD_INVALID_ARGS;
  }

  address = ecmdGenB32FromHexRight(&address, argv[1]);


  /* Did they specify a start/numbits */
  if (argc > 3) {
    if (argc != 5) {
      ecmdOutputError("cipporeputscom - Too many arguments specified; you probably added an unsupported option.\n");
      ecmdOutputError("cipporeputscom - Type 'cipporeputscom -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }

    if (!ecmdIsAllDecimal(argv[2])) {
      ecmdOutputError("cipporeputscom - Non-decimal characters detected in startbit field\n");
      return ECMD_INVALID_ARGS;
    }
    startbit = (uint32_t)atoi(argv[2]);
    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("cipporeputscom - Non-decimal characters detected in numbits field\n");
      return ECMD_INVALID_ARGS;
    }
    numbits = (uint32_t)atoi(argv[3]);


    /* Bounds check */
    if ((startbit + numbits) > ECMD_MAX_DATA_BITS) {
      char errbuf[100];
      sprintf(errbuf,"cipporeputscom - Too much data requested > %d bits\n", ECMD_MAX_DATA_BITS);
      ecmdOutputError(errbuf);
      return ECMD_DATA_BOUNDS_OVERFLOW;
    } else if (numbits == 0) {
      ecmdOutputError("cipporeputscom - Number of bits == 0, operation not performed\n");
      return ECMD_INVALID_ARGS;
    }

    rc = ecmdReadDataFormatted(cmdlineBuffer, argv[4], inputformat, numbits);
    if (rc) {
      ecmdOutputError("cipporeputscom - Problems occurred parsing input data, must be an invalid format\n");
      return rc;
    }  
  } else {

    // Check that data is numerical
    // Check that data is numerical
    if (!ecmdIsAllHex(argv[2])) {
      ecmdOutputError("cipporeputscom - Non-hex characters detected in data field\n");
      return ECMD_INVALID_ARGS;
    }

    cmdlinePtr = argv[2];

  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {
  
    /* Now we need to find out if this is a chipUnit scom or not */
    rc = ecmdQueryScom(target, queryScomData, address, ECMD_QUERY_DETAIL_LOW);
    if (rc) {
      printed = "cipporeputscom - Error occurred performing queryscom on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }
    if (queryScomData.size() != 1) {
      ecmdOutputError("cipporeputscom - Too much/little scom information returned from the dll, unable to determine if it is a chipUnit scom\n");
      rc = ECMD_DLL_INVALID;
      continue;
    }
    scomData = queryScomData.begin();

    /* If we have a cmdlinePtr, read it in now that we have a length we can use */
    if (cmdlinePtr != NULL) {
      if (dataModifier == "insert") {
        rc = ecmdReadDataFormatted(buffer, cmdlinePtr, inputformat, scomData->length);
      } else {
        rc = ecmdReadDataFormatted(cmdlineBuffer, cmdlinePtr, inputformat, scomData->length);
      }
      if (rc) {
        ecmdOutputError("cipporeputscom - Problems occurred parsing input data, must be an invalid format\n");
        coeRc = rc;
        continue;
      }
    }

    /* Setup our chipUnit looper if needed */
    cuTarget = target;
    if (scomData->isChipUnitRelated) {
      /* Error check the chipUnit returned */
      if (!scomData->isChipUnitMatch(chipUnitType)) {
        printed = "cipporeputscom - Provided chipUnit \"";
        printed += chipUnitType;
        printed += "\" doesn't match chipUnit returned by queryScom \"";
        printed += scomData->relatedChipUnit + "\"\n";
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
      rc = ecmdLooperInit(cuTarget, ECMD_SELECTED_TARGETS_LOOP, cuLooper);
      if (rc) break;
    } else { // !scomData->isChipUnitRelated
      if (chipUnitType != "") {
        printed = "cipporeputscom - A chipUnit \"";
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
    while ((scomData->isChipUnitRelated ? ecmdLooperNext(cuTarget, cuLooper) : (oneLoop--)) && (!coeRc || coeMode)) {

      /* Do we need to perform a read/modify/write op ? */
      if ((dataModifier != "insert") || (startbit != ECMD_UNSET)) {

        buffer.setBitLength(scomData->length);
        mask.setBitLength(scomData->length);
        rc = ecmdCreateDataMaskModifier(buffer, mask, cmdlineBuffer, (startbit == ECMD_UNSET ? 0 : startbit), dataModifier, scomData->endianMode);
        if (rc) {
          coeRc = rc;
          continue;
        }

        rc = cipPorePutScomUnderMask(cuTarget, address, buffer, mask, poreWriteType);
        if (rc) {
          printed = "cipporeputscom - Error occured performing cipPorePutScomUnderMask on ";
          printed += ecmdWriteTarget(cuTarget);
          printed += "\n";
          ecmdOutputError( printed.c_str() );
          coeRc = rc;
          continue;
        } else {
          validPosFound = true;
        }
      } else {

        rc = cipPorePutScom(cuTarget, address, buffer, poreWriteType);
        if (rc) {
          printed = "putscom - Error occured performing putscom on ";
          printed += ecmdWriteTarget(cuTarget);
          printed += "\n";
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
  } /* End PosLooper */
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("cipporeputscom - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}


#ifndef ECMD_REMOVE_SPY_FUNCTIONS
uint32_t cipPorePutSpyUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS , coeRc = ECMD_SUCCESS;


  ecmdLooperData looperData;            ///< Store internal Looper data
  ecmdLooperData cuLooper;              ///< Store internal Looper data for the chipUnit loop
  std::string inputformat = "default";  ///< Input data format
  std::string dataModifier = "insert";  ///< Default data modifier
  uint32_t startBit = ECMD_UNSET;       //@01 add init 
  uint32_t numBits  = 0; 
  ecmdDataBuffer buffer;                ///< Buffer to hold input data
  ecmdDataBuffer spyBuffer;             ///< Buffer to hold current spy data
  ecmdChipTarget target;                ///< Current target
  ecmdChipTarget cuTarget;              ///< Current target being operated on for the chipUnits
  bool validPosFound = false;           ///< Did the looper find anything?
  std::string printed;                  ///< Output data
  std::list<ecmdSpyData> spyDataList;           ///< Spy information returned by ecmdQuerySpy
  std::list<ecmdSpyData>::iterator spyData;     ///< Spy information returned by ecmdQuerySpy
  //bool enabledCache = false;            ///< Did we enable the cache ?
  uint8_t oneLoop = 0;                      ///< Used to break out of the chipUnit loop after the first pass for non chipUnit operations
  uint8_t argCount = 0;                         ///< Used for argument error checking

  uint32_t  poreWriteType = (CIP_PORE_SCOMINIT_SECTION | CIP_PORE_APPEND);

  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    inputformat = formatPtr;
  }


  formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-b");
  if (formatPtr != NULL) {
    dataModifier = formatPtr;
  }


  /* Grab the modes we can run in - parse them here so ecmdCommandArgs doesn't grab the -a, -b, -n etc*/
  if (ecmdParseOption(&argc, &argv, "-append")) {
    argCount++;
    /* This is on by default, do nothing */
  }
  if (ecmdParseOption(&argc, &argv, "-replace")) {
    argCount++;
    poreWriteType &= 0xFFFFFFE0;   // turn off append
    poreWriteType |= CIP_PORE_REPLACE;
  } 
  if (ecmdParseOption(&argc, &argv, "-noop")) {
    argCount++;
    poreWriteType &= 0xFFFFFFE0;   // turn off append
    poreWriteType |= CIP_PORE_NOOP;
  } 
  if (dataModifier == "and"){
    argCount++;
    poreWriteType &= 0xFFFFFFE0;   // turn off append
    poreWriteType |= CIP_PORE_OVERLAY_AND;
  } 
  if (dataModifier == "or"){
    argCount++;
    poreWriteType &= 0xFFFFFFE0;   // turn off append
    poreWriteType |= CIP_PORE_OVERLAY_OR;
  }
  
  if (argCount > 1){
    ecmdOutputError("cipporeputspy - Too many poreWriteTypes were specified. Pick one: -append, -replace, -noop, -bor, -band.\n");
    ecmdOutputError("cipporeputspy - Type 'cipporeputscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  argCount = 0; //reset
  if (ecmdParseOption(&argc, &argv, "-scaninit")) {
    argCount++;
    poreWriteType &= 0xFFFFFFBF;   // turn off scominit 
    poreWriteType |= CIP_PORE_SCANINIT_SECTION;
  } 
  if (ecmdParseOption(&argc, &argv, "-scominit")) {
    argCount++;
    /* This is on by default, do nothing */
  }
  if (argCount > 1){
    ecmdOutputError("cipporeputspy - Too many poreWriteTypes were specified. Pick one: -scominit, -scaninit.\n");
    ecmdOutputError("cipporeputspy - Type 'cipporeputscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;



   /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode



  if (argc < 3) {  //chip + address
    ecmdOutputError("cipporeputspy - Too few arguments specified; you need at least a chip, a spy, and data.\n");
    ecmdOutputError("cipporeputspy - Type 'cipporeputspy -h' for usage.\n");
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

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {

    // as of STGC00401862  we are disabling caching for spy accesses
    /* We are going to enable ring caching to speed up performance */
    //if (!ecmdIsRingCacheEnabled(target)) {
    //  rc = ecmdEnableRingCache(target);
    //  if (rc) {
    //    ecmdOutputError("cipporeputspy - ecmdEnableRingCache call failed!\n");
    //    coeRc = rc;
    //    continue;
    //  }
    //  enabledCache = true;
    //}

    /* Ok, we need to find out what type of spy we are dealing with here, to find out how to output */
    rc = ecmdQuerySpy(target, spyDataList, spyName.c_str(), ECMD_QUERY_DETAIL_LOW);
    if (rc || spyDataList.empty()) {
      printed = "cipporeputspy - Error occured looking up data on spy " + spyName + " on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }

    spyData = spyDataList.begin();
    if (spyData->isEnumerated) {
      if (inputformat == "default") inputformat = "enum";
    } else {
      if (inputformat == "default") inputformat = "x";
    }

    /* Now that we know whether it is enumerated or not, we can finally finish our arg parsing */
    if (inputformat != "enum") {
      if (argc > 3) {
        if (argc != 5) {
          ecmdOutputError("cipporeputspy - Too many arguments specified; you probably added an option that wasn't recognized.\n");
          ecmdOutputError("cipporeputspy - Type 'cipporeputspy -h' for usage.\n");
          rc = ECMD_INVALID_ARGS;
          break;
        }

        if (!ecmdIsAllDecimal(argv[2])) {
          ecmdOutputError("cipporeputspy - Non-decimal numbers detected in startbit field\n");
          rc = ECMD_INVALID_ARGS;
          break;
        }
        startBit = (uint32_t)atoi(argv[2]);

        if (!ecmdIsAllDecimal(argv[3])) {
          ecmdOutputError("cipporeputspy - Non-decimal numbers detected in numbits field\n");
          rc = ECMD_INVALID_ARGS;
          break;
        }
        numBits = (uint32_t)atoi(argv[3]);

        rc = ecmdReadDataFormatted(buffer, argv[4], inputformat, numBits);
        if (rc) {
          ecmdOutputError("cipporeputspy - Problems occurred parsing input data, must be an invalid format\n");
          break;
        }

      } else {
        rc = ecmdReadDataFormatted(buffer, argv[2], inputformat, spyData->bitLength);
        if (rc) {
          ecmdOutputError("cipporeputspy - Problems occurred parsing input data, must be an invalid format\n");
          break;
        }
      }
    }

    /* Setup our chipUnit looper if needed */
    cuTarget = target;
    if (spyData->isChipUnitRelated) {
      /* Error check the chipUnit returned */
      if (!spyData->isChipUnitMatch(chipUnitType)) {
        printed = "cipporeputspy - Provided chipUnit \"";
        printed += chipUnitType;
        printed += "\"doesn't match chipUnit returned by querySpy \"";
        printed += spyData->relatedChipUnit + "\n";
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
      rc = ecmdLooperInit(cuTarget, ECMD_SELECTED_TARGETS_LOOP, cuLooper);
      if (rc) break;
    } else { // !spyData->isChipUnitRelated
      if (chipUnitType != "") {
        printed = "cipporeputspy - A chipUnit \"";
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
    while ((spyData->isChipUnitRelated ? ecmdLooperNext(cuTarget, cuLooper) : (oneLoop--)) && (!coeRc || coeMode)) {

      if ((inputformat != "enum") && ((dataModifier != "insert") || (startBit != ECMD_UNSET))) {

        rc = getSpy(cuTarget, spyName.c_str(), spyBuffer);

        if ((rc == ECMD_SPY_GROUP_MISMATCH) && (numBits == spyData->bitLength)) {
          /* We will go on if the user was going to write the whole spy anyway */
          ecmdOutputWarning("cipporeputspy - Problems reading group spy - found a mismatch - going ahead with write\n");
          rc = 0;
        } else if (rc == ECMD_SPY_GROUP_MISMATCH) {
          /* If the user was only going to write part of the spy we can't go on because we don't ahve valid data to merge with */
          printed = "cipporeputspy - Problems reading group spy - found a mismatch - to force write don't use start/numbits - on ";
          printed += ecmdWriteTarget(cuTarget) + "\n";
          ecmdOutputError( printed.c_str() );
          ecmdOutputError("Use getspy with the -v option to get the detailed failure information\n");
          coeRc = rc;
          continue;
        } else if (rc == ECMD_SPY_FAILED_ECC_CHECK) {
          ecmdOutputWarning("cipporeputspy - Problems reading spy - ECC check failed - going ahead with write\n");
          rc = 0;
        } else if (rc) {
          printed = "cipporeputspy - Error occured performing getspy on ";
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

        rc = cipPorePutSpy(cuTarget, spyName.c_str(), spyBuffer, poreWriteType);
        if (rc) {
          printed = "cipporeputspy - Error occured performing cipporeputspy on ";
          printed += ecmdWriteTarget(cuTarget) + "\n";
          ecmdOutputError( printed.c_str() );
          coeRc = rc;
          continue;
        }
      } else {

        if (inputformat == "enum") {
          rc = cipPorePutSpyEnum(cuTarget, spyName.c_str(), argv[argc-1], poreWriteType);
        } else {
          rc = cipPorePutSpy(cuTarget, spyName.c_str(), buffer,poreWriteType);
        }

        if (rc) {
          printed = "cipporeputspy - Error occured performing cipporeputspy on ";
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

    // as of STGC00401862  we are disabling caching for spy accesses
    /* Now that we are moving onto the next target, let's flush the cache we have */
    //if (enabledCache) {
    //  uint32_t trc = ecmdDisableRingCache(target);
    //  if (trc) {
    //    ecmdOutputError("cipporeputspy - Problems disabling the ring cache\n");
    //    coeRc = trc;
    //    continue;
    //  }
    //  enabledCache = false;
    //}

  } /* End poslooper */
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  if (!validPosFound) {
    ecmdOutputError("cipporeputspy - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}
#endif
  
uint32_t cipPorePutSprUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  ecmdLooperData looperData;    ///< Store internal Looper data
  ecmdChipTarget subTarget;        ///< Current target
  ecmdLooperData subLooperData;    ///< Store internal Looper data
  std::string inputformat = "x";                ///< Default input format
  std::string dataModifier = "insert";          ///< Default data Modifier (And/Or/insert)
  ecmdDataBuffer cmdlineBuffer;        ///< Buffer to store data to write with
  ecmdDataBuffer buffer;     ///< Buffer to store data from the spr
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  std::string sprName;          ///< Name of spr to write 
  uint32_t startBit = ECMD_UNSET; ///< Startbit to insert data
  uint32_t numBits = 0;         ///< Number of bits to insert data
  ecmdProcRegisterInfo procInfo; ///< Used to figure out if an SPR is threaded or not 
  ecmdChipTarget threadTarget;        ///< Current thread target
  ecmdLooperData threadLooperData;    ///< Store internal thread Looper data
  char* cmdlinePtr = NULL;            ///< Pointer to data in argv array
  uint8_t argCount = 0;                         ///< Used for argument error checking

  uint32_t  poreWriteType = (CIP_PORE_SCOMINIT_SECTION | CIP_PORE_APPEND);

  /* get format flag, if it's there */
  char* formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    inputformat = formatPtr;
  }

  formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-b");
  if (formatPtr != NULL) {
    dataModifier = formatPtr;
  }

  /* Grab the modes we can run in - parse them here so ecmdCommandArgs doesn't grab the -a, -b, -n etc*/
  if (ecmdParseOption(&argc, &argv, "-append")) {
    argCount++;
    /* This is on by default, do nothing */
  }
  if (ecmdParseOption(&argc, &argv, "-replace")) {
    argCount++;
    poreWriteType &= 0xFFFFFFE0;   // turn off append
    poreWriteType |= CIP_PORE_REPLACE;
  } 
  if (ecmdParseOption(&argc, &argv, "-noop")) {
    argCount++;
    poreWriteType &= 0xFFFFFFE0;   // turn off append
    poreWriteType |= CIP_PORE_NOOP;
  } 
  if (dataModifier == "and"){
    argCount++;
    poreWriteType &= 0xFFFFFFE0;   // turn off append
    poreWriteType |= CIP_PORE_OVERLAY_AND;
  } 
  if (dataModifier == "or"){
    argCount++;
    poreWriteType &= 0xFFFFFFE0;   // turn off append
    poreWriteType |= CIP_PORE_OVERLAY_OR;
  }
  
  if (argCount > 1){
    ecmdOutputError("cipporeputspr - Too many poreWriteTypes were specified. Pick one: -append, -replace, -noop, -bor, -band.\n");
    ecmdOutputError("cipporeputspr - Type 'cipporeputscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  argCount = 0; //reset
  if (ecmdParseOption(&argc, &argv, "-scaninit")) {
    argCount++;
    poreWriteType &= 0xFFFFFFBF;   // turn off scominit 
    poreWriteType |= CIP_PORE_SCANINIT_SECTION;
  } 
  if (ecmdParseOption(&argc, &argv, "-scominit")) {
    argCount++;
    /* This is on by default, do nothing */
  }
  if (argCount > 1){
    ecmdOutputError("cipporeputspr - Too many poreWriteTypes were specified. Pick one: -scominit, -scaninit.\n");
    ecmdOutputError("cipporeputspr - Type 'cipporeputscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

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
    ecmdOutputError("cipporeputspr - Too few arguments specified; you need at least an sprName  and some data.\n");
    ecmdOutputError("cipporeputspr - Type 'cipporeputspr -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  sprName = argv[0];

  if (argc == 4) {
    if (!ecmdIsAllDecimal(argv[1])) {
      ecmdOutputError("cipporeputspr - Non-decimal numbers detected in startbit field\n");
      return ECMD_INVALID_ARGS;
    }
    startBit = (uint32_t)atoi(argv[1]);

    if (!ecmdIsAllDecimal(argv[2])) {
      ecmdOutputError("cipporeputspr - Non-decimal numbers detected in numbits field\n");
      return ECMD_INVALID_ARGS;
    }
    numBits = (uint32_t)atoi(argv[2]);


    rc = ecmdReadDataFormatted(cmdlineBuffer, argv[3], inputformat);
    if (rc) {
      printed = "cipporeputspr - Problems occurred parsing input data, must be an invalid format\n";
      ecmdOutputError(printed.c_str());
      return rc;
    }
  } else if (argc == 2) {

    cmdlinePtr = argv[1];

  } else {
    ecmdOutputError("cipporeputspr - Too many arguments specified; you probably added an option that wasn't recognized.\n");
    ecmdOutputError("cipporeputspr - Type 'cipporeputspr -h' for usage.\n");
    return ECMD_INVALID_ARGS;

  }


  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {

    /* First thing we need to do is find out for this particular target if the SPR is threaded */
    rc = ecmdQueryProcRegisterInfo(target, sprName.c_str(), procInfo);
    if (rc) {
      printed = "cipporeputspr - Error occured getting spr info for ";
      printed += sprName;
      printed += " on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }

    if (procInfo.mode == ECMD_PROCREG_READ_ONLY)
    {
      // Error! Trying to read/get Write-Only Register 
      printed = "cipporeputspr - Can't Write to Read-Only Register '";
      printed += sprName + "' \n";
      ecmdOutputError( printed.c_str() );
      coeRc = ECMD_WRITING_TO_READ_ONLY_REG;
      continue;
    }

    /* If we have a cmdlinePtr, read it in now that we have a length we can use */
    if (cmdlinePtr != NULL) {
      if (dataModifier == "insert") {
        rc = ecmdReadDataFormatted(buffer, cmdlinePtr, inputformat, procInfo.bitLength);
      } else {
        rc = ecmdReadDataFormatted(cmdlineBuffer, cmdlinePtr, inputformat, procInfo.bitLength);
      }
      if (rc) {
        ecmdOutputError("cipporeputspr - Problems occurred parsing input data, must be an invalid format\n");
        coeRc = rc;
        continue;
      }
    }

    // We've done the chipUnit loop and gotten the SPR info, now figure out how to loop.
    subTarget = target;
    if (procInfo.isChipUnitRelated) {
      if (procInfo.relatedChipUnit != "") {
        subTarget.chipUnitType = procInfo.relatedChipUnit;
        subTarget.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
      }
      subTarget.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
      if (procInfo.threadReplicated) {
        subTarget.threadState = ECMD_TARGET_FIELD_WILDCARD;
      }
    }

    rc = ecmdLooperInit(subTarget, ECMD_SELECTED_TARGETS_LOOP, subLooperData);
    if (rc) break;

    while (ecmdLooperNext(subTarget, subLooperData) && (!coeRc || coeMode)) {

      /* The user did the r/m/w version, so we need to do a get spr */
      /* Do we need to perform a read/modify/write op ? */
      if ((dataModifier != "insert") || (startBit != ECMD_UNSET)) {
        rc = getSpr(subTarget, sprName.c_str(), buffer);
        if (rc) {
          printed = "cipporeputspr - Error occurred performing getspr on ";
          printed += ecmdWriteTarget(subTarget) + "\n";
          ecmdOutputError( printed.c_str() );
          coeRc = rc;
          continue;
        }
        
        rc = ecmdApplyDataModifier(buffer, cmdlineBuffer, (startBit == ECMD_UNSET ? 0 : startBit), dataModifier);
        if (rc) {
          coeRc = rc;
          continue;
        }
      }

      rc = cipPorePutSpr(subTarget, sprName.c_str(), buffer, poreWriteType);
      if (rc) {
        printed = "cipporeputspr - Error occured performing cipporeputspr on ";
        printed += ecmdWriteTarget(subTarget) + "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
      } else {
        validPosFound = true;     
      }

      if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
        printed = ecmdWriteTarget(subTarget) + "\n";
        ecmdOutput(printed.c_str());
      }
    }
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  //this is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("cipporeputspr - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}


uint32_t cipPoreQueryImageUser(int argc, char * argv[]) {
 uint32_t rc = ECMD_SUCCESS; uint32_t coeRc = ECMD_SUCCESS;
 ecmdChipTarget target; 
 std::string printed;
 ecmdLooperData looperData;     ///< Store internal Looper data
 std::string query;
 cipPoreImageInfo o_imageInfo;
 char * imageName = NULL;
 char printbuffer[100];
 bool validPosFound = false;           ///< Did the looper find anything?


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
  if (argc > 2) {
    ecmdOutputError("cipporequeryimage - Invalid number of args. Too many specified. \n");
    ecmdOutputError("cipporequeryimage - Type 'cipporequeryimage -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  if (argc < 1) {  //chip + imagename
    ecmdOutputError("cipporequeryimage - Too few arguments specified; you need at least a chip.\n");
    ecmdOutputError("cipporequeryimage - Type 'cipporeputscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  /* Check for image name here */
  if (argc == 2){
    imageName = argv[1];
    rc = cipPoreQueryImage(target, o_imageInfo, imageName);
    if (rc) {
      printed = "cipporequeryimage - Error occured getting image info for ";
      ecmdOutputError( printed.c_str() );
      sprintf(printbuffer,"%s", imageName); 
      ecmdOutput( printbuffer );
      printed = "\n";
      ecmdOutput( printed.c_str() );
    } else {
    
      printed = "-------------------------------------------------------\n"; ecmdOutput(printed.c_str());
      printed = "PORE Image Information for image = ";
      ecmdOutput(printed.c_str());
      ecmdOutput(imageName);
      printed = "\n"; ecmdOutput(printed.c_str());
      printed = "-------------------------------------------------------\n"; ecmdOutput(printed.c_str());
      sprintf(printbuffer, "Version       = 0x%x\n", o_imageInfo.version);
      ecmdOutput(printbuffer);
      sprintf(printbuffer, "Build Date    = 0x%x\n", o_imageInfo.build_date);
      ecmdOutput(printbuffer);
      sprintf(printbuffer, "Build Time    = 0x%x\n", o_imageInfo.build_time);
      ecmdOutput(printbuffer);
      sprintf(printbuffer, "Load Date     = 0x%x\n", o_imageInfo.load_date);
      ecmdOutput(printbuffer);
      sprintf(printbuffer, "Load Time     = 0x%x\n", o_imageInfo.load_time);
      ecmdOutput(printbuffer);
      sprintf(printbuffer, "Base Address  = 0x%llx\n", o_imageInfo.base_address);
      ecmdOutput(printbuffer);
      printed =           "Builder       = "; printed += o_imageInfo.builder;  printed += "\n"; ecmdOutput(printed.c_str());
      printed = "-------------------------------------------------------\n"; ecmdOutput(printed.c_str());
    }

  } else {
  
    rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
    if (rc) return rc;
  
    while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {
  
      /* First thing we need to do is find out for this particular target if the SPR is threaded */
      rc = cipPoreQueryImage(target, o_imageInfo, imageName);
      if (rc) {
        printed = "cipporequeryimage - Error occured getting image info for ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
      } else {
        validPosFound = true;     
      
        printed = "-------------------------------------------------------\n"; ecmdOutput(printed.c_str());
        printed = "PORE Image Information for target = ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutput(printed.c_str());
        printed = "-------------------------------------------------------\n"; ecmdOutput(printed.c_str());
        sprintf(printbuffer, "Version       = 0x%x\n", o_imageInfo.version);
        ecmdOutput(printbuffer);
        sprintf(printbuffer, "Build Date    = 0x%x\n", o_imageInfo.build_date);
        ecmdOutput(printbuffer);
        sprintf(printbuffer, "Build Time    = 0x%x\n", o_imageInfo.build_time);
        ecmdOutput(printbuffer);
        sprintf(printbuffer, "Load Date     = 0x%x\n", o_imageInfo.load_date);
        ecmdOutput(printbuffer);
        sprintf(printbuffer, "Load Time     = 0x%x\n", o_imageInfo.load_time);
        ecmdOutput(printbuffer);
        sprintf(printbuffer, "Base Address  = 0x%llx\n", o_imageInfo.base_address);
        ecmdOutput(printbuffer);
        printed =           "Builder       = "; printed += o_imageInfo.builder;  printed += "\n"; ecmdOutput(printed.c_str());
        printed = "-------------------------------------------------------\n"; ecmdOutput(printed.c_str());
      }
    }
    // coeRc will be the return code from in the loop, coe mode or not.
    if (coeRc) return coeRc;
  
    //this is an error common across all UI functions
    if (!validPosFound) {
      ecmdOutputError("cipporequeryimage - Unable to find a valid chip to execute command on\n");
      return ECMD_TARGET_NOT_CONFIGURED;
    }
  }
 return rc;

}
  
uint32_t cipPoreLoadImageUser(int argc, char * argv[]) {
 uint32_t rc = ECMD_SUCCESS; uint32_t coeRc = ECMD_SUCCESS;
 ecmdChipTarget target; 
 std::string printed;
 ecmdLooperData looperData;     ///< Store internal Looper data
 std::string query;
 cipPoreImageInfo o_imageInfo;
 char * imageName = NULL;
 bool validPosFound = false;           ///< Did the looper find anything?
 uint32_t baseAddress;

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
  if (argc > 3) {
    ecmdOutputError("cipporeloadimage - Invalid number of args. Too many specified. \n");
    ecmdOutputError("cipporeloadimage - Type 'cipporeloadimage -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  if (argc < 2){
    ecmdOutputError("cipporeloadimage - Invalid number of args. Too few specified. \n");
    ecmdOutputError("cipporeloadimage - Type 'cipporeloadimage -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  if (argc == 3)
   imageName = argv[2];
  baseAddress = (uint32_t)atoi(argv[1]);

  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD; 
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

 
  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;
  
  while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {

    /* First thing we need to do is find out for this particular target if the SPR is threaded */
    rc = cipPoreLoadImage(target, baseAddress, imageName);
    if (rc) {
      printed = "cipporequeryimage - Error occured getting image info for ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    } else {
      validPosFound = true;     
    }
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;
 
  //this is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("cipporequeryimage - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }
 return rc;
}
#endif //CIP_REMOVE_PORE_FUNCTIONS


