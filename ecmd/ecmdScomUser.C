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

// Module Description **************************************************
//
// Description: 
//
// End Module Description **********************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define ecmdDaScomUser_C
#include <ecmdCommandUtils.H>
#include <ecmdIntReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <stdio.h>
#include <time.h>
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

int ecmdGetScomUser(int argc, char* argv[]) {
  int rc = ECMD_SUCCESS;

  bool expectFlag = false;
  bool maskFlag = false;

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  //expect and mask flags check
  if (ecmdParseOption(&argc, &argv, "-exp")) {
    expectFlag = true;

    if (ecmdParseOption(&argc, &argv, "-mask")) {
      maskFlag = true;
    }
  }

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr == NULL) {
    format = "xw";
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
  if (argc < 2) {  //chip + address
    ecmdOutputError("Too few arguments specified; you need at least a chip and an address.\nType 'getscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //get chip name
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get address to fetch
  uint32_t address = ecmdGenB32FromHexRight(&address, argv[1]);
  if (address == 0xFFFFFFFF) {
    ecmdOutputError("Address argument was not a string of hex characters.\nType 'putscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //container to store data
  ecmdDataBuffer buffer(3);  //the 3 is just a placeholder
  ecmdDataBuffer * expected = NULL;  //don't want to allocate this unless I have to
  ecmdDataBuffer * mask = NULL;  //ditto as for expected

  if (expectFlag) {
    int argLength = argc - 2;  //account for chip and address args

    if (maskFlag) {
      argLength /= 2;
      mask = new ecmdDataBuffer(argLength);
    }

    expected = new ecmdDataBuffer(argLength);

    for (int i = 0; i < argLength; i++) {

      expected->insertFromHexLeft(argv[i+2], i * 32, 32);

      if (maskFlag)
        mask->insertFromHexLeft(argv[i+2+argLength], i * 32, 32);
 
    }

  }
  else if (argc > 2) {
    ecmdOutputError("Too many arguments specified; you probably added an option that wasn't recognized.\nType 'getscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  bool validPosFound = false;
  rc = ecmdConfigLooperInit(target);
  if (rc) return rc;

  std::string printed;

  while ( ecmdConfigLooperNext(target) ) {

    rc = getScom(target, address, buffer);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "Error occured performing getscom on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }

    if (expectFlag) {

      if (maskFlag) {
        buffer.setAnd(*mask, 0, buffer.getBitLength());
      }

      if (!ecmdCheckExpected(buffer, *expected)) {

        //@ make this stuff sprintf'd
        char outstr[50];
        printed = ecmdWriteTarget(target);
        sprintf(outstr, " Data miscompare occured at address: %.8X\n", address);
        printed += outstr;


        printed += "Actual";
        if (maskFlag) {
          printed += " (with mask): ";
        }
        else {
          printed += "            : ";
        }

        printed += ecmdWriteDataFormatted(buffer, format);

        printed += "Expected          : ";
        printed += ecmdWriteDataFormatted(*expected, format);
        ecmdOutputError( printed.c_str() );
      }

    }
    else {

      printed = ecmdWriteTarget(target);
      printed += ecmdWriteDataFormatted(buffer, format);
      ecmdOutput( printed.c_str() );

    }

  }

  if (expectFlag && expected != NULL) {
    delete expected;
    expected = NULL;

    if (maskFlag && mask != NULL) {
      delete mask;
      mask = NULL;
    }

  }

  if (!validPosFound) {
    //this is an error common across all UI functions
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

int ecmdPutScomUser(int argc, char* argv[]) {

  int rc = ECMD_SUCCESS;
  bool andFlag = false;
  bool orFlag = false;

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  if (ecmdParseOption(&argc, &argv, "-and"))
    andFlag = true;
  else if (ecmdParseOption(&argc, &argv, "-or"))
    orFlag = true;

  /************************************************************************/
  /* Parse Common Cmdline Args,                                           */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/

  if (argc < 3) {  //chip + address + some data
    ecmdOutputError("Too few arguments specified; you need at least a chip, an address, and some data.\nType 'putscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //set chip name
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  uint32_t address = ecmdGenB32FromHexRight(&address, argv[1]);
  if (address == 0xFFFFFFFF) {
    ecmdOutputError("Address argument was not a string of hex characters.\nType 'putscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  // container for data to write
  ecmdDataBuffer buffer(argc-2);

  //parse data to write
  for (int i = 0; i < argc-2; i++) {
    buffer.insertFromHexRight(argv[i+2], 0, i*32, 32);
  }

  ecmdDataBuffer * fetchBuffer = NULL;  //only allocate if I have to
  if (andFlag || orFlag) {
    fetchBuffer = new ecmdDataBuffer(argc-2);
  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  bool validPosFound = false;
  rc = ecmdConfigLooperInit(target);
  if (rc) return rc;
  std::string printed;

  while (ecmdConfigLooperNext(target)) {

    if (andFlag || orFlag) {

      rc = getScom(target, address, *fetchBuffer);

      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
      }
      else if (rc) {
        printed = "Error occured performing getscom on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }
      else {
        validPosFound = true;     
      }

      int minLength = (buffer.getBitLength() < fetchBuffer->getBitLength()) ? buffer.getBitLength() : fetchBuffer->getBitLength();

      if (andFlag) {
        fetchBuffer->setAnd(buffer, 0, minLength);
      }
      else if (orFlag) {
        fetchBuffer->setOr(buffer, 0, minLength);
      }

      rc = putScom(target, address, *fetchBuffer);
      if (rc) {
        printed = "Error occured performing putscom on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }

    }
    else {

      rc = putScom(target, address, buffer);
      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
      }
      else if (rc) {
        printed = "Error occured performing putscom on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }
      else {
        validPosFound = true;     
      }

    }
  
  }

  if (andFlag || orFlag) {
    delete fetchBuffer;
    fetchBuffer = NULL;
  }

  if (!validPosFound) {
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

int ecmdPollScomUser(int argc, char* argv[]) {
  int rc = ECMD_SUCCESS;

  bool expectFlag = false;
  bool maskFlag = false;
  bool verboseFlag = false;

  uint8_t NONE_T = 0;
  uint8_t SECONDS_T = 1;
  uint8_t CYCLES_T = 2;
  uint8_t ITERATIONS_T = 3;

  uint8_t intervalFlag = NONE_T;
  uint8_t limitFlag = NONE_T;
  uint32_t interval = 5;
  uint32_t maxPolls = 1050;
  uint32_t numPolls = 0x1;
  uint32_t timerStart = 0x0;

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  //expect and mask flags check
  if (ecmdParseOption(&argc, &argv, "-exp")) {
    expectFlag = true;

    if (ecmdParseOption(&argc, &argv, "-mask")) {
      maskFlag = true;
    }
  }

  if (ecmdParseOption(&argc, &argv, "-verbose")) {
    verboseFlag = true;
  }

  char * curArg = ecmdParseOptionWithArgs(&argc, &argv, "-interval");
  if (curArg != NULL) {
    interval = atoi(curArg);
    if (strstr(curArg, "c")) {
      intervalFlag = CYCLES_T;
    }
    else {
      intervalFlag = SECONDS_T;
    }
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
  }

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr == NULL) {
    format = "xw";
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
  if (argc < 2) {  //chip + address
    ecmdOutputError("Too few arguments specified; you need at least a chip and an address.\nType 'pollscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //get chip name
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get address to fetch
  uint32_t address = ecmdGenB32FromHexRight(&address, argv[1]);
  if (address == 0xFFFFFFFF) {
    ecmdOutputError("Address argument was not a string of hex characters.\nType 'pollscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //container to store data
  ecmdDataBuffer buffer(3);  //the 3 is just a placeholder
  ecmdDataBuffer * expected = NULL;  //don't want to allocate this unless I have to
  ecmdDataBuffer * mask = NULL;  //ditto as for expected

  if (expectFlag) {
    int argLength = argc - 2;  //account for chip and address args

    if (maskFlag) {
      argLength /= 2;
      mask = new ecmdDataBuffer(argLength);
    }

    expected = new ecmdDataBuffer(argLength);

    for (int i = 0; i < argLength; i++) {

      expected->insertFromHexLeft(argv[i+2], i * 32, 32);

      if (maskFlag)
        mask->insertFromHexLeft(argv[i+2+argLength], i * 32, 32);
 
    }

  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/
  bool validPosFound = false;
  rc = ecmdConfigLooperInit(target);
  if (rc) return rc;
  std::string printed;

  while (ecmdConfigLooperNext(target)) {

    bool done = false;
    timerStart = time(NULL);

    printed = ecmdWriteTarget(target);
    char outstr[30];
    sprintf(outstr, "Polling address %.4X...\n", address);
    printed += outstr;
    ecmdOutput( printed.c_str() );

    while (!done) {
 
      rc = getScom(target, address, buffer);
      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        break;
      }
      else if (rc) {
        printed = "Error occured performing getscom on ";
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
      if (limitFlag == ITERATIONS_T && numPolls >= maxPolls) done = 1;
      else if (limitFlag == CYCLES_T && numPolls > maxPolls) done = 1;
      else if (limitFlag == SECONDS_T && maxPolls != 0 && (time(NULL) > timerStart + maxPolls)) done = 1;

      if (expectFlag) {

        if (maskFlag) {
          buffer.setAnd(*mask, 0, buffer.getBitLength());
        }

        if (!ecmdCheckExpected(buffer, *expected)) {

          //mismatches
          if (done || verboseFlag) {

            printed += "Actual";
            if (maskFlag) {
              printed += " (with mask): ";
            }
            else {
              printed += "            : ";
            }

            printed += ecmdWriteDataFormatted(buffer, format);

            printed += "Expected          : ";
            printed += ecmdWriteDataFormatted(*expected, format);

            if (done) {
              char outstr[50];
              sprintf(outstr, "Data miscompare occured at address: %.8X\n", address);
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
        printed += ecmdWriteDataFormatted(buffer, format);

        if (done) {
          printed += ecmdWriteTarget(target);
          printed += "\tPolling Complete\n";
        }

        ecmdOutput( printed.c_str() );
      }

    }  //while (!done)

  }

  if (expectFlag) {
    delete expected;
    expected = NULL;

    if (maskFlag) {
      delete mask;
      mask = NULL;
    }

  }

  if (!validPosFound) {
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
