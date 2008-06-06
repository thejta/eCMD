/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File ecmdDaScomUser.C                                   
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

uint32_t ecmdGetScomUser(int argc, char* argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;
  uint32_t e_rc = ECMD_SUCCESS;                 ///< Expect rc

  bool expectFlag = false;
  bool maskFlag = false;
  char* verbosePtr = NULL;
  char* expectPtr = NULL;                       ///< Pointer to expected data in arg list
  char* maskPtr = NULL;                         ///< Pointer to mask data in arg list
  ecmdDataBuffer expected;                      ///< Buffer to store expected data
  ecmdDataBuffer mask;                          ///< Buffer for mask of expected data
  std::string outputformat = "x";               ///< Output Format to display
  std::string inputformat = "x";                ///< Input format of data
  ecmdChipTarget target;                        ///< Current target being operated on
  ecmdChipTarget cuTarget;                      ///< Current target being operated on for the chipUnit
  bool isChipUnitScom;                          ///< Is this a chipUnit scom ?
  std::list<ecmdScomData> queryScomData;        ///< Scom data 
  ecmdDataBuffer scombuf;                       ///< Buffer to hold scom data
  ecmdDataBuffer buffer;                        ///< Data requested by the user
  bool validPosFound = false;                   ///< Did the looper find anything?
  ecmdLooperData looperData;                    ///< Store internal Looper data
  ecmdLooperData cuLooper;                      ///< Store internal Looper data for the chipUnit loop
  std::string printed;                          ///< Output data
  uint32_t startbit = ECMD_UNSET;               ///< Startbit in the scom data
  uint32_t numbits = 0;                         ///< Number of bits to diplay
  uint8_t oneLoop = 0;                              ///< Used to break out of the chipUnit loop after the first pass for non chipUnit operations

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  //expect and mask flags check
  if ((expectPtr = ecmdParseOptionWithArgs(&argc, &argv, "-exp")) != NULL) {
    expectFlag = true;

    if ((maskPtr = ecmdParseOptionWithArgs(&argc, &argv, "-mask")) != NULL) {
      maskFlag = true;
    }
  }

  if (ecmdParseOption(&argc, &argv, "-v")) {
    verbosePtr = "-v";
  }
  else if (ecmdParseOption(&argc, &argv, "-vs0")) {
    verbosePtr = "-vs0";
  }
  else if (ecmdParseOption(&argc, &argv, "-vs1")) {
    verbosePtr = "-vs1";
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

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 2) {  //chip + address
    ecmdOutputError("getscom - Too few arguments specified; you need at least a chip and an address.\n");
    ecmdOutputError("getscom - Type 'getscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get address to fetch
  if (!ecmdIsAllHex(argv[1])) {
    ecmdOutputError("getscom - Non-hex characters detected in address field\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t address = ecmdGenB32FromHexRight(&address, argv[1]);


  if (expectFlag) {
    rc = ecmdReadDataFormatted(expected, expectPtr, inputformat);
    if (rc) {
      ecmdOutputError("getscom - Problems occurred parsing expected data, must be an invalid format\n");
      return rc;
    }

    if (maskFlag) {
      rc = ecmdReadDataFormatted(mask, maskPtr, inputformat);
      if (rc) {
        ecmdOutputError("getscom - Problems occurred parsing mask data, must be an invalid format\n");
        return rc;
      }
    }
  }
  
  if (argc > 2) {
    if (argc != 4) {
      ecmdOutputError("getscom - Too many arguments specified; you probably added an unsupported option.\n");
      ecmdOutputError("getscom - Type 'getscom -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }

    if (!ecmdIsAllDecimal(argv[2])) {
      ecmdOutputError("getscom - Non-decimal characters detected in startbit field\n");
      return ECMD_INVALID_ARGS;
    }
    startbit = (uint32_t)atoi(argv[2]);
    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("getscom - Non-decimal characters detected in numbits field\n");
      return ECMD_INVALID_ARGS;
    }
    numbits = (uint32_t)atoi(argv[3]);


    /* Bounds check */
    if ((startbit + numbits) > ECMD_MAX_DATA_BITS) {
      char errbuf[100];
      sprintf(errbuf,"getscom - Too much data requested > %d bits\n", ECMD_MAX_DATA_BITS);
      ecmdOutputError(errbuf);
      return ECMD_DATA_BOUNDS_OVERFLOW;
    } else if (numbits == 0) {
      ecmdOutputError("getscom - Number of bits == 0, operation not performed\n");
      return ECMD_INVALID_ARGS;
    }
    
  } 
  
  
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;


  while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {
    
    /* Now we need to find out if this is a cu scom or not */
    rc = ecmdQueryScom(target, queryScomData, address, ECMD_QUERY_DETAIL_LOW);
    if (rc) {
      printed = "getscom - Error occurred performing queryscom on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }

    if (queryScomData.size() != 1) {
      ecmdOutputError("getscom - Too much/little scom information returned from the dll, unable to determine if it is a chipUnit scom\n");
      coeRc = ECMD_DLL_INVALID;
      continue;
    }
    isChipUnitScom = queryScomData.begin()->isChipUnitRelated;

    /* Setup our chipUnit looper if needed */
    cuTarget = target;
    if (isChipUnitScom) {
      cuTarget.chipTypeState = cuTarget.cageState = cuTarget.nodeState = cuTarget.slotState = cuTarget.posState = ECMD_TARGET_FIELD_VALID;
      /* Error check the chipUnit returned */
      if (queryScomData.begin()->relatedChipUnit != chipUnitType) {
        printed = "getscom - Provided chipUnit \"";
        printed += chipUnitType;
        printed += "\" doesn't match chipUnit returned by queryScom \"";
        printed += queryScomData.begin()->relatedChipUnit + "\"\n";
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
    } else { // !isChipUnitScom
      if (chipUnitType != "") {
        printed = "getscom - A chipUnit \"";
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
    while ((isChipUnitScom ? ecmdLooperNext(cuTarget, cuLooper) : (oneLoop--)) && (!coeRc || coeMode)) {
	   
     rc = getScom(cuTarget, address, scombuf);
     if (rc) {
       printed = "getscom - Error occured performing getscom on ";
       printed += ecmdWriteTarget(cuTarget);
       printed += "\n";
       ecmdOutputError( printed.c_str() );
       coeRc = rc;
       continue;
     } else {
       validPosFound = true;
     }
 
     if (startbit != ECMD_UNSET) {
       scombuf.extract(buffer, startbit, numbits);
     } else {
       scombuf.extract(buffer, 0, scombuf.getBitLength());
     }
 
     if (expectFlag) {

       if (maskFlag) {
     	 buffer.setAnd(mask, 0, buffer.getBitLength());
       }

       uint32_t mismatchBit = ECMD_UNSET;
       if (!ecmdCheckExpected(buffer, expected, mismatchBit)) {

     	 //@ make this stuff sprintf'd
     	 char outstr[50];
     	 printed = ecmdWriteTarget(cuTarget);
     	 sprintf(outstr, "\ngetscom - Data miscompare occured at address: %.8X\n", address);
     	 printed += outstr;
     	 ecmdOutputError( printed.c_str() );


     	 printed = "getscom - Actual";
     	 if (maskFlag) {
     	   printed += " (with mask): ";
     	 }
     	 else {
     	   printed += " 	   : ";
     	 }
     	 printed += ecmdWriteDataFormatted(buffer, outputformat);
     	 ecmdOutputError( printed.c_str() );

     	 printed = "getscom - Expected  	   : ";
     	 printed += ecmdWriteDataFormatted(expected, outputformat);
     	 ecmdOutputError( printed.c_str() );
     	 e_rc = ECMD_EXPECT_FAILURE;
       }

     }
     else {

       printed = ecmdWriteTarget(cuTarget);
       printed += ecmdWriteDataFormatted(buffer, outputformat);
       ecmdOutput( printed.c_str() );
 
       if ((verbosePtr != NULL) && !expectFlag) {
     	 //even if rc returned is non-zero we want to continue to the next chip
#ifndef FIPSODE
        ecmdDisplayScomData(cuTarget, address, buffer, verbosePtr);
#else
	ecmdOutputWarning("ecmdDisplayScomData not supported on FSP\n");
#endif
       }
     }
    } /* End cuLooper */
  } /* End PosLooper */
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("getscom - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  /* If we failed an expect let's return that */
  if (e_rc) return e_rc;

  return rc;
}

uint32_t ecmdPutScomUser(int argc, char* argv[]) {

  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;
  std::string inputformat = "x";                ///< Default input format
  std::string dataModifier = "insert";          ///< Default data Modifier (And/Or/insert)
  ecmdLooperData looperData;                    ///< Store internal Looper data
  ecmdLooperData cuLooper;                      ///< Store internal Looper data for the chipUnit loop
  bool isChipUnitScom;                          ///< Is this a chipUnit scom ?
  ecmdChipTarget target;                        ///< Chip target being operated on
  ecmdChipTarget cuTarget;                      ///< Current target being operated on for the chipUnit
  std::list<ecmdScomData> queryScomData;        ///< Scom data 
  uint32_t address;                             ///< Scom address
  ecmdDataBuffer buffer;                        ///< Container to store read/write data
  ecmdDataBuffer cmdlineBuffer;                 ///< Buffer to store data to be inserted
  bool validPosFound = false;                   ///< Did the config looper actually find a chip ?
  std::string printed;                          ///< String for printed data
  uint32_t startbit = ECMD_UNSET;               ///< Startbit to insert data
  uint32_t numbits = 0;                         ///< Number of bits to insert data
  uint8_t oneLoop = 0;                          ///< Used to break out of the chipUnit loop after the first pass for non chipUnit operations
  char* cmdlinePtr = NULL;                      ///< Pointer to data in argv array

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

  /************************************************************************/
  /* Parse Common Cmdline Args,                                           */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  if (argc < 3) {  //chip + address + some data
    ecmdOutputError("putscom - Too few arguments specified; you need at least a chip, an address, and some data.\n");
    ecmdOutputError("putscom - Type 'putscom -h' for usage.\n");
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
    ecmdOutputError("putscom - Non-hex characters detected in address field\n");
    return ECMD_INVALID_ARGS;
  }
  address = ecmdGenB32FromHexRight(&address, argv[1]);

  /* Did they specify a start/numbits */
  if (argc > 3) {
    if (argc != 5) {
      ecmdOutputError("putscom - Too many arguments specified; you probably added an unsupported option.\n");
      ecmdOutputError("putscom - Type 'putscom -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }

    if (!ecmdIsAllDecimal(argv[2])) {
      ecmdOutputError("putscom - Non-decimal characters detected in startbit field\n");
      return ECMD_INVALID_ARGS;
    }
    startbit = (uint32_t)atoi(argv[2]);
    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("putscom - Non-decimal characters detected in numbits field\n");
      return ECMD_INVALID_ARGS;
    }
    numbits = (uint32_t)atoi(argv[3]);


    /* Bounds check */
    if ((startbit + numbits) > ECMD_MAX_DATA_BITS) {
      char errbuf[100];
      sprintf(errbuf,"putscom - Too much data requested > %d bits\n", ECMD_MAX_DATA_BITS);
      ecmdOutputError(errbuf);
      return ECMD_DATA_BOUNDS_OVERFLOW;
    } else if (numbits == 0) {
      ecmdOutputError("putscom - Number of bits == 0, operation not performed\n");
      return ECMD_INVALID_ARGS;
    }

    rc = ecmdReadDataFormatted(cmdlineBuffer, argv[4], inputformat, numbits);
    if (rc) {
      ecmdOutputError("putscom - Problems occurred parsing input data, must be an invalid format\n");
      return rc;
    }  
  } else {

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
      printed = "putscom - Error occurred performing queryscom on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }
    if (queryScomData.size() != 1) {
      ecmdOutputError("putscom - Too much/little scom information returned from the dll, unable to determine if it is a chipUnit scom\n");
      rc = ECMD_DLL_INVALID;
      continue;
    }
    isChipUnitScom = queryScomData.begin()->isChipUnitRelated;

    /* If we have a cmdlinePtr, read it in now that we have a length we can use */
    if (cmdlinePtr != NULL) {
      if (dataModifier == "insert") {
        rc = ecmdReadDataFormatted(buffer, cmdlinePtr, inputformat, queryScomData.begin()->length);
      } else {
        rc = ecmdReadDataFormatted(cmdlineBuffer, cmdlinePtr, inputformat, queryScomData.begin()->length);
      }
      if (rc) {
        ecmdOutputError("putscom - Problems occurred parsing input data, must be an invalid format\n");
        coeRc = rc;
        continue;
      }
    }

    /* Setup our chipUnit looper if needed */
    cuTarget = target;
    if (isChipUnitScom) {
      cuTarget.chipTypeState = cuTarget.cageState = cuTarget.nodeState = cuTarget.slotState = cuTarget.posState = ECMD_TARGET_FIELD_VALID;
      /* Error check the chipUnit returned */
      if (queryScomData.begin()->relatedChipUnit != chipUnitType) {
        printed = "putscom - Provided chipUnit \"";
        printed += chipUnitType;
        printed += "\" doesn't match chipUnit returned by queryScom \"";
        printed += queryScomData.begin()->relatedChipUnit + "\"\n";
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
    } else { // !isChipUnitScom
      if (chipUnitType != "") {
        printed = "putscom - A chipUnit \"";
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
    while ((isChipUnitScom ? ecmdLooperNext(cuTarget, cuLooper) : (oneLoop--)) && (!coeRc || coeMode)) {

      /* Do we need to perform a read/modify/write op ? */
      if ((dataModifier != "insert") || (startbit != ECMD_UNSET)) {

        rc = getScom(cuTarget, address, buffer);
        if (rc) {
          printed = "putscom - Error occured performing getscom on ";
          printed += ecmdWriteTarget(cuTarget);
          printed += "\n";
          ecmdOutputError( printed.c_str() );
          coeRc = rc;
          continue;
        } else {
          validPosFound = true;
        }

        rc = ecmdApplyDataModifier(buffer, cmdlineBuffer, (startbit == ECMD_UNSET ? 0 : startbit), dataModifier);
        if (rc) {
          coeRc = rc;
          continue;
        }
      }

      /* My data is all setup, now write it */
      rc = putScom(cuTarget, address, buffer);
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
    ecmdOutputError("putscom - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdPollScomUser(int argc, char* argv[]) {
  uint32_t rc = ECMD_SUCCESS , coeRc = ECMD_SUCCESS;

  bool expectFlag = false;
  bool maskFlag = false;
  bool verboseFlag = false;
  bool misMatchFlag = false;
  std::string outputformat = "x";               ///< Output format
  std::string inputformat = "x";                ///< Input format
  ecmdChipTarget target;                        ///< Target we are operating on
  ecmdChipTarget cuTarget;                      ///< Current target being operated on for the cores
  std::list<ecmdScomData> queryScomData;        ///< Scom data 
  bool validPosFound = false;                   ///< Did the looper find anything?
  ecmdLooperData looperData;                    ///< Store internal Looper data
  ecmdLooperData cuLooper;                      ///< Store internal Looper data for the chipUnit loop
  bool isChipUnitScom;                          ///< Is this a chipUnit scom ?
  ecmdDataBuffer buffer;                        ///< Store current scom data
  ecmdDataBuffer expected;                      ///< Store expected data
  ecmdDataBuffer mask;                          ///< Store mask data
  uint8_t oneLoop = 0;                              ///< Used to break out of the chipUnit loop after the first pass for non chipUnit operations

  uint8_t NONE_T = 0;
  uint8_t SECONDS_T = 1;
  uint8_t CYCLES_T = 2;
  uint8_t ITERATIONS_T = 3;

  uint8_t intervalFlag = NONE_T;
  uint8_t limitFlag = NONE_T;
  uint32_t interval = 5;
  uint32_t maxPolls = 1050;
  uint32_t numPolls = 1;
  time_t timerStart = 0x0;

  char * curArg;                                ///< Used for arg parsing
  char outstr[100];


  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/

  //expect and mask flags check
  if ((curArg = ecmdParseOptionWithArgs(&argc, &argv, "-exp")) != NULL) {
    expectFlag = true;

    rc = ecmdReadDataFormatted(expected, curArg, inputformat);
    if (rc) return rc;

    if ((curArg = ecmdParseOptionWithArgs(&argc, &argv, "-mask")) != NULL) {
      maskFlag = true;

      rc = ecmdReadDataFormatted(mask, curArg, inputformat);
      if (rc) return rc;
      
    }
  }

  if ((curArg = ecmdParseOptionWithArgs(&argc, &argv, "-mismatch")) != NULL) {
    misMatchFlag = true;

    rc = ecmdReadDataFormatted(expected, curArg, inputformat);
    if (rc) return rc;
  }



  if (ecmdParseOption(&argc, &argv, "-verbose")) {
    verboseFlag = true;
  }

  curArg = ecmdParseOptionWithArgs(&argc, &argv, "-interval");
  if (curArg != NULL) {
    interval = (uint32_t)atoi(curArg);
    if (strstr(curArg, "c")) {
      intervalFlag = CYCLES_T;
    }
    else {
      intervalFlag = SECONDS_T;
    }
  } else {
    /* The default */
    interval = 5;
    intervalFlag = SECONDS_T;
  }
    

  curArg = ecmdParseOptionWithArgs(&argc, &argv, "-limit");
  if (curArg != NULL) {
    maxPolls = (uint32_t)atoi(curArg);
    if (strstr(curArg, "s")) {
      limitFlag = SECONDS_T;
    } else if (strstr(curArg, "c")) {
      limitFlag = CYCLES_T;
    } else { 
      limitFlag = ITERATIONS_T;  /* default when using -limit */
    }
  } else {
    maxPolls  = 1000;
    limitFlag = ITERATIONS_T;  /* default when using -limit */
  }
    

  if (limitFlag != ITERATIONS_T && limitFlag != intervalFlag) {
    ecmdOutputError("pollscom - Invalid interval/limit pair.\n");
    ecmdOutputError("pollscom - Type 'pollscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

#ifdef REMOVE_SIM
  if (intervalFlag == CYCLES_T) {
    ecmdOutputError("pollscom - Can't use cycles in non-simulation mode");
    return ECMD_INVALID_ARGS;
  }
#endif

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


  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode
  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 2) {  //chip + address
    ecmdOutputError("pollscom - Too few arguments specified; you need at least a chip and an address.\n");
    ecmdOutputError("pollscom - Type 'pollscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get address to fetch
  if (!ecmdIsAllHex(argv[1])) {
    ecmdOutputError("pollscom - Non-hex characters detected in address field\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t address = ecmdGenB32FromHexRight(&address, argv[1]);


  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/
  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;
  std::string printed;

  while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {
    
    /* Now we need to find out if this is a chipUnit scom or not */
    rc = ecmdQueryScom(target, queryScomData, address, ECMD_QUERY_DETAIL_LOW);
    if (rc) {
      printed = "pollscom - Error occurred performing queryscom on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }
    if (queryScomData.size() != 1) {
      ecmdOutputError("pollscom - Too much/little scom information returned from the dll, unable to determine if it is a chipUnit scom\n");
      rc = ECMD_DLL_INVALID;
      continue;
    }
    isChipUnitScom = queryScomData.begin()->isChipUnitRelated;

    /* Setup our chipUnit looper if needed */
    cuTarget = target;
    if (isChipUnitScom) {
      cuTarget.chipTypeState = cuTarget.cageState = cuTarget.nodeState = cuTarget.slotState = cuTarget.posState = ECMD_TARGET_FIELD_VALID;
      /* Error check the chipUnit returned */
      if (queryScomData.begin()->relatedChipUnit != chipUnitType) {
        printed = "pollscom - Provided chipUnit \"";
        printed += chipUnitType;
        printed += "\" doesn't match chipUnit returned by queryScom \"";
        printed += queryScomData.begin()->relatedChipUnit + "\"\n";
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
    } else { // !isChipUnitScom
      if (chipUnitType != "") {
        printed = "pollscom - A chipUnit \"";
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
    while ((isChipUnitScom ? ecmdLooperNext(cuTarget, cuLooper) : (oneLoop--)) && (!coeRc || coeMode)) {
	   
     bool done = false;
     timerStart = time(NULL);

     printed = ecmdWriteTarget(cuTarget);
     sprintf(outstr, "Polling address %.6X...\n", address);
     printed += outstr;
     ecmdOutput( printed.c_str()) ;

     rc = 0;
     while (!done && rc ==0) {
 
       rc = getScom(cuTarget, address, buffer);
       if (rc) {
     	 printed = "pollscom - Error occured performing getscom on ";
     	 printed += ecmdWriteTarget(cuTarget);
     	 printed += "\n";
     	 ecmdOutputError( printed.c_str() );
         coeRc = rc;
         continue;
       }
       else {
     	 validPosFound = true;
       }

       /* ------------------------ */
       /* check for last iteration */
       /* ------------------------ */
       if ((limitFlag == ITERATIONS_T || limitFlag == CYCLES_T) && numPolls >= maxPolls) done = 1;
       else if (limitFlag == SECONDS_T && maxPolls != 0 && (time(NULL) > timerStart + (time_t)maxPolls)) done = 1;

       uint32_t mismatchBit = ECMD_UNSET;

       if (expectFlag) {

     	 if (maskFlag) {
     	   buffer.setAnd(mask, 0, buffer.getBitLength());
     	 }

	 
     	 if (!ecmdCheckExpected(buffer, expected, mismatchBit)) {

     	   //mismatches
     	   if (done || verboseFlag) {

     	     printed = "pollscom - Actual";
     	     if (maskFlag) {
     	       printed += " (with mask): ";
     	     }
     	     else {
     	       printed += "	       : ";
     	     }

     	     printed += ecmdWriteDataFormatted(buffer, outputformat);

     	     printed += "pollscom - Expected	      : ";
     	     printed += ecmdWriteDataFormatted(expected, outputformat);

     	     if (done) {
     	       sprintf(outstr, "pollscom - Data miscompare occured at address: %.8X\n", address);
     	       printed = outstr + printed;
     	       ecmdOutputError( printed.c_str() );
	       coeRc = ECMD_EXPECT_FAILURE;
               continue;
     	     }
     	     else {
     	       ecmdOutput( printed.c_str() );
     	     }

     	   }
 
     	 }
     	 else {
     	   done = 1;  //matches
     	 }

       } else if (misMatchFlag) {
         if (!ecmdCheckExpected(buffer, expected, mismatchBit)) {
           done = 1; //found a mismatch
           if (verboseFlag){
             printed = "";
             sprintf(outstr, "pollscom - Data mismatch occurred at 0x%.8X\n", address);
             printed = outstr + printed;
             ecmdOutput( printed.c_str() );
             return 0;
           }
         } else {
           if (verboseFlag){
             printed = "pollscom - Actual == Expected ";
             printed += "	       : ";
             printed += ecmdWriteDataFormatted(buffer, outputformat);
             ecmdOutput( printed.c_str() );
           }

           if (done){
             sprintf(outstr, "pollscom - Mismatch never occurred at address 0x%.8X\n", address);
             printed = outstr + printed;
             ecmdOutputError( printed.c_str() );
             coeRc = ECMD_EXPECT_FAILURE;
             continue;
           }

         }
       }
       else {

     	 printed = "Actual	      : ";
     	 printed += ecmdWriteDataFormatted(buffer, outputformat);

     	 if (done) {
     	   printed += ecmdWriteTarget(cuTarget);
     	   printed += "\tPolling Complete\n";
     	 }

     	 ecmdOutput( printed.c_str() );
       }

       //update poll counters
       if (limitFlag == ITERATIONS_T) {
     	 numPolls++;

     	 if (intervalFlag == CYCLES_T) {
#ifndef REMOVE_SIM
          rc = simclock(interval);
          if (rc) return rc;
#endif
         } else if (intervalFlag == SECONDS_T) {
          sleep(interval);
         }

       }
       else if (limitFlag == CYCLES_T && intervalFlag == CYCLES_T) {

#ifndef REMOVE_SIM
         numPolls += interval;
         rc = simclock(interval);
         if (rc) return rc;
#endif

       }
       else if (limitFlag == SECONDS_T && intervalFlag == SECONDS_T) {
         sleep(interval);
       }
       else {

         ecmdOutputError("pollscom - Invalid limit/interval argument pair");
         return ECMD_INVALID_ARGS;

       }
        
      
     }  //while (!done)
    } /* End cuLooper */
  } /* End PosLooper */
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("pollscom - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}



// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//                              HJH      coe
//
// End Change Log *****************************************************
