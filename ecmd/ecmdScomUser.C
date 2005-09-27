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
/* $Header$ */

// Module Description **************************************************
//
// Description: 
//
// End Module Description **********************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define ecmdDaScomUser_C
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




#undef ecmdDaScomUser_C
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
  uint32_t rc = ECMD_SUCCESS;
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
  ecmdChipTarget coretarget;                    ///< Current target being operated on for the cores
  std::list<ecmdScomData> queryScomData;        ///< Ring data 
  ecmdDataBuffer scombuf;                       ///< Buffer to hold scom data
  ecmdDataBuffer buffer;                        ///< Data requested by the user
  bool validPosFound = false;                   ///< Did the looper find anything?
  ecmdLooperData looperdata;                    ///< Store internal Looper data
  ecmdLooperData corelooper;                    ///< Store internal Looper data for the core loop
  std::string printed;                          ///< Output data
  int startbit = -1;                            ///< Startbit in the scom data
  int numbits = 0;                              ///< Number of bits to diplay
  
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


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 2) {  //chip + address
    ecmdOutputError("getscom - Too few arguments specified; you need at least a chip and an address.\n");
    ecmdOutputError("getscom - Type 'getscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_QUERY_WILDCARD;
  target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get address to fetch
  
  if (!ecmdIsAllHex(argv[1])) {
    ecmdOutputError("getscom - Non-hex characters detected in address field\n");
    return ECMD_INVALID_ARGS;
  } else if (strlen(argv[1]) > 6) {
    ecmdOutputError("getscom - Scom addresses must be <= 24 bits in length\n");
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
    startbit = atoi(argv[2]);
    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("getscom - Non-decimal characters detected in numbits field\n");
      return ECMD_INVALID_ARGS;
    }
    numbits = atoi(argv[3]);


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

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;


  while ( ecmdConfigLooperNext(target, looperdata) ) {
    
    bool isCoreScom;                              ///< Is this a core scom ?
    
    /* Now we need to find out if this is a core scom or not */
    rc = ecmdQueryScom(target, queryScomData, address);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
    }
    else if (rc) {
      printed = "getscom - Error occurred performing queryscom on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    if (queryScomData.size() != 1) {
      ecmdOutputError("getscom - Too much/little ring information returned from the dll, unable to determine if it is a core scom\n");
      return ECMD_DLL_INVALID;
    }
    isCoreScom = queryScomData.begin()->isCoreRelated;

    /* Setup our Core looper if needed */
    coretarget = target;
    if (isCoreScom) {
      coretarget.chipTypeState = coretarget.cageState = coretarget.nodeState = coretarget.slotState = coretarget.posState = ECMD_TARGET_QUERY_FIELD_VALID;
      coretarget.coreState = ECMD_TARGET_QUERY_WILDCARD;
      coretarget.threadState = ECMD_TARGET_FIELD_UNUSED;

      /* Init the core loop */
      rc = ecmdConfigLooperInit(coretarget, ECMD_SELECTED_TARGETS_LOOP, corelooper);
      if (rc) return rc;
    }

    /* If this isn't a core ring we will fall into while loop and break at the end, if it is we will call run through configloopernext */
    while (!isCoreScom ||
           ecmdConfigLooperNext(coretarget, corelooper)) {
	   
     rc = getScom(coretarget, address, scombuf);
     if (rc == ECMD_TARGET_NOT_CONFIGURED) {
       continue;
     }
     else if (rc) {
     	 printed = "getscom - Error occured performing getscom on ";
     	 printed += ecmdWriteTarget(coretarget);
     	 printed += "\n";
     	 ecmdOutputError( printed.c_str() );
     	 return rc;
     }
     else {
       validPosFound = true;
     }
 
     if (startbit != -1) {
       scombuf.extract(buffer, startbit, numbits);
     } else {
       scombuf.extract(buffer, 0, scombuf.getBitLength());
     }
 
     if (expectFlag) {

       if (maskFlag) {
     	 buffer.setAnd(mask, 0, buffer.getBitLength());
       }

       if (!ecmdCheckExpected(buffer, expected)) {

     	 //@ make this stuff sprintf'd
     	 char outstr[50];
     	 printed = ecmdWriteTarget(coretarget);
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

     	 printed = "getscom - Expected  	: ";
     	 printed += ecmdWriteDataFormatted(expected, outputformat);
     	 ecmdOutputError( printed.c_str() );
     	 e_rc = ECMD_EXPECT_FAILURE;
       }

     }
     else {

       printed = ecmdWriteTarget(coretarget);
       printed += ecmdWriteDataFormatted(buffer, outputformat);
       ecmdOutput( printed.c_str() );
 
       if ((verbosePtr != NULL) && !expectFlag) {
     	 //even if rc returned is non-zero we want to continue to the next chip
#ifndef FIPSODE
        ecmdDisplayScomData(coretarget, address, buffer, verbosePtr);
#else
	ecmdOutputWarning("ecmdDisplayScomData not supported on FSP\n");
#endif
       }
     }
     if (!isCoreScom) break;
    } /* End CoreLooper */

  } /* End PosLooper */
  
  if (!validPosFound) {
    //this is an error common across all UI functions
    ecmdOutputError("getscom - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  /* If we failed an expect let's return that */
  if (e_rc) return e_rc;

  return rc;
}

uint32_t ecmdPutScomUser(int argc, char* argv[]) {

  uint32_t rc = ECMD_SUCCESS;
  std::string inputformat = "x";                ///< Default input format
  std::string dataModifier = "insert";          ///< Default data Modifier (And/Or/insert)
  ecmdDataBuffer fetchBuffer;                   ///< Buffer to store read/modify/write data
  ecmdLooperData looperdata;                    ///< Store internal Looper data
  ecmdLooperData corelooper;                    ///< Store internal Looper data for the core loop
  ecmdChipTarget target;                        ///< Chip target being operated on
  ecmdChipTarget coretarget;                    ///< Current target being operated on for the cores
  std::list<ecmdScomData> queryScomData;        ///< Scom data 
  uint32_t address;                             ///< Scom address
  ecmdDataBuffer buffer;                        ///< Container to store write data
  bool validPosFound = false;                   ///< Did the config looper actually find a chip ?
  std::string printed;                          ///< String for printed data
  int startbit = -1;                            ///< Startbit to insert data
  int numbits = 0;                              ///< Number of bits to insert data

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


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/

  if (argc < 3) {  //chip + address + some data
    ecmdOutputError("putscom - Too few arguments specified; you need at least a chip, an address, and some data.\n");
    ecmdOutputError("putscom - Type 'putscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_QUERY_WILDCARD;
  target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  if (!ecmdIsAllHex(argv[1])) {
    ecmdOutputError("putscom - Non-hex characters detected in address field\n");
    return ECMD_INVALID_ARGS;
  } else if (strlen(argv[1]) > 6) {
    ecmdOutputError("putscom - Scom addresses must be <= 24 bits in length\n");
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
    startbit = atoi(argv[2]);
    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("putscom - Non-decimal characters detected in numbits field\n");
      return ECMD_INVALID_ARGS;
    }
    numbits = atoi(argv[3]);


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

    rc = ecmdReadDataFormatted(buffer, argv[4], inputformat, numbits);
    if (rc) {
      ecmdOutputError("putscom - Problems occurred parsing input data, must be an invalid format\n");
      return rc;
    }
    
    
  } else {  

    rc = ecmdReadDataFormatted(buffer, argv[2], inputformat);
    if (rc) {
      ecmdOutputError("putscom - Problems occurred parsing input data, must be an invalid format\n");
      return rc;
    }
  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdConfigLooperNext(target, looperdata)) {
    
    bool isCoreScom;                              ///< Is this a core scom ?
    
    /* Now we need to find out if this is a core scom or not */
    rc = ecmdQueryScom(target, queryScomData, address);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
    }
    else if (rc) {
      printed = "putscom - Error occurred performing queryscom on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    if (queryScomData.size() != 1) {
      ecmdOutputError("putscom - Too much/little ring information returned from the dll, unable to determine if it is a core scom\n");
      return ECMD_DLL_INVALID;
    }
    isCoreScom = queryScomData.begin()->isCoreRelated;

    /* Setup our Core looper if needed */
    coretarget = target;
    if (isCoreScom) {
      coretarget.chipTypeState = coretarget.cageState = coretarget.nodeState = coretarget.slotState = coretarget.posState = ECMD_TARGET_QUERY_FIELD_VALID;
      coretarget.coreState = ECMD_TARGET_QUERY_WILDCARD;
      coretarget.threadState = ECMD_TARGET_FIELD_UNUSED;

      /* Init the core loop */
      rc = ecmdConfigLooperInit(coretarget, ECMD_SELECTED_TARGETS_LOOP, corelooper);
      if (rc) return rc;
    }

    /* If this isn't a core ring we will fall into while loop and break at the end, if it is we will call run through configloopernext */
    while (!isCoreScom ||
           ecmdConfigLooperNext(coretarget, corelooper)) {
	   
     /* Do we need to perform a read/modify/write op ? */
     if ((dataModifier != "insert") || (startbit != -1)) {


       rc = getScom(coretarget, address, fetchBuffer);

       if (rc == ECMD_TARGET_NOT_CONFIGURED) {
     	 continue;
       }
       else if (rc) {
     	 printed = "putscom - Error occured performing getscom on ";
     	 printed += ecmdWriteTarget(coretarget);
     	 printed += "\n";
     	 ecmdOutputError( printed.c_str() );
     	 return rc;
       }
       else {
     	 validPosFound = true;
       }

       rc = ecmdApplyDataModifier(fetchBuffer, buffer, (startbit == -1 ? 0 : startbit), dataModifier);
       if (rc) return rc;

       rc = putScom(coretarget, address, fetchBuffer);
       if (rc) {
     	 printed = "putscom - Error occured performing putscom on ";
     	 printed += ecmdWriteTarget(coretarget);
     	 printed += "\n";
     	 ecmdOutputError( printed.c_str() );
     	 return rc;
       }

     }
     else {

       rc = putScom(coretarget, address, buffer);
       if (rc == ECMD_TARGET_NOT_CONFIGURED) {
     	 continue;
       }
       else if (rc) {
     	 printed = "putscom - Error occured performing putscom on ";
     	 printed += ecmdWriteTarget(coretarget);
     	 printed += "\n";
     	 ecmdOutputError( printed.c_str() );
     	 return rc;
       }
       else {
     	 validPosFound = true;
       }

     }

     if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
       printed = ecmdWriteTarget(coretarget) + "\n";
       ecmdOutput(printed.c_str());
     }
     if (!isCoreScom) break;
    } /* End CoreLooper */
  } /* End PosLooper */
  
  if (!validPosFound) {
    ecmdOutputError("putscom - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdPollScomUser(int argc, char* argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  bool expectFlag = false;
  bool maskFlag = false;
  bool verboseFlag = false;
  std::string outputformat = "x";               ///< Output format
  std::string inputformat = "x";                ///< Input format
  ecmdChipTarget target;                        ///< Target we are operating on
  bool validPosFound = false;                   ///< Did the looper find anything?
  ecmdLooperData looperdata;                    ///< Store internal Looper data
  ecmdDataBuffer buffer;                        ///< Store current scom data
  ecmdDataBuffer expected;                      ///< Store expected data
  ecmdDataBuffer mask;                          ///< Store mask data

  uint8_t NONE_T = 0;
  uint8_t SECONDS_T = 1;
  uint8_t CYCLES_T = 2;
  uint8_t ITERATIONS_T = 3;

  uint8_t intervalFlag = NONE_T;
  uint8_t limitFlag = NONE_T;
  uint32_t interval = 5;
  uint32_t maxPolls = 1050;
  uint32_t numPolls = 1;
  uint32_t timerStart = 0x0;

  char * curArg;                                ///< Used for arg parsing


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

  if (ecmdParseOption(&argc, &argv, "-verbose")) {
    verboseFlag = true;
  }

  curArg = ecmdParseOptionWithArgs(&argc, &argv, "-interval");
  if (curArg != NULL) {
    interval = atoi(curArg);
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
    maxPolls = atoi(curArg);
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


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 2) {  //chip + address
    ecmdOutputError("pollscom - Too few arguments specified; you need at least a chip and an address.\n");
    ecmdOutputError("pollscom - Type 'pollscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get address to fetch
  if (!ecmdIsAllHex(argv[1])) {
    ecmdOutputError("pollscom - Non-hex characters detected in address field\n");
    return ECMD_INVALID_ARGS;
  } else if (strlen(argv[1]) > 6) {
    ecmdOutputError("pollscom - Scom addresses must be <= 24 bits in length\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t address = ecmdGenB32FromHexRight(&address, argv[1]);


  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata, ECMD_VARIABLE_DEPTH_LOOP);
  if (rc) return rc;
  std::string printed;

  while (ecmdConfigLooperNext(target, looperdata)) {

    bool done = false;
    timerStart = time(NULL);

    printed = ecmdWriteTarget(target);
    char outstr[30];
    sprintf(outstr, "Polling address %.6X...\n", address);
    printed += outstr;
    ecmdOutput( printed.c_str() );

    while (!done) {
 
      rc = getScom(target, address, buffer);
      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        break;
      }
      else if (rc) {
        printed = "pollscom - Error occured performing getscom on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }
      else {
        validPosFound = true;     
      }

      /* ------------------------ */
      /* check for last iteration */
      /* ------------------------ */
      if ((limitFlag == ITERATIONS_T || limitFlag == CYCLES_T) && numPolls >= maxPolls) done = 1;
      else if (limitFlag == SECONDS_T && maxPolls != 0 && ((uint32_t) time(NULL) > timerStart + maxPolls)) done = 1;

      if (expectFlag) {

        if (maskFlag) {
          buffer.setAnd(mask, 0, buffer.getBitLength());
        }

        if (!ecmdCheckExpected(buffer, expected)) {

          //mismatches
          if (done || verboseFlag) {

            printed = "pollscom - Actual";
            if (maskFlag) {
              printed += " (with mask): ";
            }
            else {
              printed += "            : ";
            }

            printed += ecmdWriteDataFormatted(buffer, outputformat);

            printed += "pollscom - Expected          : ";
            printed += ecmdWriteDataFormatted(expected, outputformat);

            if (done) {
              char outstr[50];
              sprintf(outstr, "pollscom - Data miscompare occured at address: %.8X\n", address);
              printed = outstr + printed;
              ecmdOutputError( printed.c_str() );
            }
            else {
              ecmdOutput( printed.c_str() );
            }

          }
            
        }
        else {
          done = 1;  //matches
        }

      }
      else {

        printed = "Actual            : ";
        printed += ecmdWriteDataFormatted(buffer, outputformat);

        if (done) {
          printed += ecmdWriteTarget(target);
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

  }


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
//
// End Change Log *****************************************************
