// Copyright ***********************************************************
//                                                                      
// File ecmdMiscUser.C                                   
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
#define ecmdMiscUser_C
#include <stdio.h>

#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <ecmdInterpreter.H>
#include <ecmdSharedUtils.H>

#undef ecmdMiscUser_C
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

uint32_t ecmdGetConfigUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  std::string configName;       ///< Name of config variable to fetch
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  bool formatSpecfied = false;   ///< Was the -o option specified
  ecmdConfigValid_t validOutput;  ///< Indicator if valueAlpha, valueNumeric (or both) are valid
  std::string  valueAlpha;       ///< Alpha value of setting (if appropriate)
  uint32_t  valueNumeric;        ///< Numeric value of setting (if appropriate)
  ecmdDataBuffer numData;	 ///< Initialise data buffer with the numeric value
  std::string printed;           ///< Print Buffer
  ecmdLooperData looperdata;     ///< Store internal Looper data

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr == NULL) {
    format = "x";
  }
  else {
    format = formatPtr;
    formatSpecfied = true;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 1) {
    ecmdOutputError("getconfig - Too few arguments specified; you need at least a ConfigName.\n");
    ecmdOutputError("getconfig - Type 'getconfig -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config
  if( argc == 2) {
   target.chipType = argv[0];
   target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
   configName = argv[1];
  }
  else {
   target.chipTypeState = ECMD_TARGET_QUERY_IGNORE;
   configName = argv[0];
  }

  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while ( ecmdConfigLooperNext(target, looperdata) ) {

    /* Actually go fetch the data */
    rc = ecmdGetConfiguration(target, configName, validOutput, valueAlpha, valueNumeric);

    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
      printed = "getconfig - Error occured performing ecmdGetConfiguration on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    else {
      validPosFound = true;
    }

    printed = ecmdWriteTarget(target) + "\n";
    
    if( (validOutput == ECMD_CONFIG_VALID_FIELD_ALPHA) || ((validOutput ==  ECMD_CONFIG_VALID_FIELD_BOTH) &&
    (!formatSpecfied))) {
     printed += configName + " = " + valueAlpha + "\n";
     ecmdOutput(printed.c_str());
    }
    else if(( validOutput == ECMD_CONFIG_VALID_FIELD_NUMERIC) || ((validOutput ==  ECMD_CONFIG_VALID_FIELD_BOTH) && (formatSpecfied))) {
     numData.setWord(0, valueNumeric);
     printed += configName + " = ";
     printed += ecmdWriteDataFormatted(numData, format);
     printed += "\n";
     ecmdOutput(printed.c_str()); 
    }
  }

  if (!validPosFound) {
    //this is an error common across all UI functions
    ecmdOutputError("getconfig - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;

}


uint32_t ecmdSetConfigUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  std::string configName;       ///< Name of config variable to fetch
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  ecmdConfigValid_t validInput;  ///< Indicator if valueAlpha, valueNumeric (or both) are valid
  std::string  valueAlpha;       ///< Input of type Ascii
  uint32_t  valueNumeric;        ///< Input of type Numeric  
  ecmdDataBuffer inputBuffer;	 ///< Initialise data buffer with the numeric value(if input is Numeric)
  char inputVal[400];		 ///< Value to set configuration variable to
  std::string printed;           ///< Print Buffer
  ecmdLooperData looperdata;     ///< Store internal Looper data

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr == NULL) {
    format = "a";
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
  if (argc < 2) {
    ecmdOutputError("setconfig - Too few arguments specified; you need at least a ConfigName, Value to set it to.\n");
    ecmdOutputError("setconfig - Type 'setconfig -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config
  if( argc == 3) {
   target.chipType = argv[0];
   target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
   configName = argv[1];
   strcpy(inputVal, argv[2]);
  }
  else {
   target.chipTypeState = ECMD_TARGET_QUERY_IGNORE;
   configName = argv[0];
   strcpy(inputVal, argv[1]);
  }
  if(format == "a") {
    valueAlpha = inputVal;
    validInput = ECMD_CONFIG_VALID_FIELD_ALPHA;
  } else {
    rc = ecmdReadDataFormatted(inputBuffer, inputVal, format);
    if (rc) {
     ecmdOutputError("setconfig - Problems occurred parsing input data, must be an invalid format\n");
     return rc;
    }
    int numBits = inputBuffer.getBitLength();
    if(numBits > 32 ) {
     ecmdOutputError("setconfig - Problems occurred parsing input data, Bitlength should be <= 32 bits\n");
     return ECMD_INVALID_ARGS;
    }
    inputBuffer.shiftRightAndResize(32-numBits);
    valueNumeric = inputBuffer.getWord(0);
    validInput = ECMD_CONFIG_VALID_FIELD_NUMERIC;
  }
    
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while ( ecmdConfigLooperNext(target, looperdata) ) {
    /* Actually go fetch the data */
    rc = ecmdSetConfiguration(target, configName, validInput, valueAlpha, valueNumeric);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
      printed = "setconfig - Error occured performing ecmdSetConfiguration on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    else {
      validPosFound = true;
    }
    printed = ecmdWriteTarget(target) + "\n";
    ecmdOutput( printed.c_str() );
  }

  if (!validPosFound) {
    //this is an error common across all UI functions
    ecmdOutputError("setconfig - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;

}

uint32_t ecmdGetCfamUser(int argc, char* argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  bool expectFlag = false;
  bool maskFlag = false;
  char* expectPtr = NULL;                       ///< Pointer to expected data in arg list
  char* maskPtr = NULL;                         ///< Pointer to mask data in arg list
  ecmdDataBuffer expected;                      ///< Buffer to store expected data
  ecmdDataBuffer mask;                          ///< Buffer for mask of expected data
  std::string outputformat = "x";               ///< Output Format to display
  std::string inputformat = "x";                ///< Input format of data
  ecmdChipTarget target;                        ///< Current target being operated on
  ecmdDataBuffer buffer;                        ///< Buffer to hold Cfam data
  bool validPosFound = false;                   ///< Did the looper find anything?
  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string printed;                          ///< Output data

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
    ecmdOutputError("getcfam - Too few arguments specified; you need at least a chip and an address.\n");
    ecmdOutputError("getcfam - Type 'getcfam -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get address to fetch

  if (!ecmdIsAllHex(argv[1])) {
    ecmdOutputError("getcfam - Non-hex characters detected in address field\n");
    return ECMD_INVALID_ARGS;
  } else if (strlen(argv[1]) > 6) {
    ecmdOutputError("getcfam - Cfam addresses must be <= 24 bits in length\n"); 
    return ECMD_INVALID_ARGS;
  }
  uint32_t address = ecmdGenB32FromHexRight(&address, argv[1]);


  if (expectFlag) {

    rc = ecmdReadDataFormatted(expected, expectPtr, inputformat);
    if (rc) {
      ecmdOutputError("getcfam - Problems occurred parsing expected data, must be an invalid format\n");
      return rc;
    }

    if (maskFlag) {
      rc = ecmdReadDataFormatted(mask, maskPtr, inputformat);
      if (rc) {
        ecmdOutputError("getcfam - Problems occurred parsing mask data, must be an invalid format\n");
        return rc;
      }

    }


  }
  else if (argc > 2) { 
    ecmdOutputError("getcfam - Too many arguments specified; you probably added an option that wasn't recognized.\n");
    ecmdOutputError("getcfam - Type 'getcfam -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;


  while ( ecmdConfigLooperNext(target, looperdata) ) {

    rc = getCfamRegister(target, address, buffer);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "getcfam - Error occured performing getcfam on ";
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
        buffer.setAnd(mask, 0, buffer.getBitLength());
      }

      if (!ecmdCheckExpected(buffer, expected)) {

        //@ make this stuff sprintf'd
        char outstr[50];
        printed = ecmdWriteTarget(target);
        sprintf(outstr, "\ngetcfam - Data miscompare occured at address: %.8X\n", address);
        printed += outstr;
        ecmdOutputError( printed.c_str() );


        printed = "getcfam - Actual";
        if (maskFlag) {
          printed += " (with mask): ";
        }
        else {
          printed += "            : ";
        }

        printed += ecmdWriteDataFormatted(buffer, outputformat);
        ecmdOutputError( printed.c_str() );

        printed = "getcfam - Expected          : ";
        printed += ecmdWriteDataFormatted(expected, outputformat);
        ecmdOutputError( printed.c_str() );
      }

    }
    else {

      printed = ecmdWriteTarget(target);
      printed += ecmdWriteDataFormatted(buffer, outputformat);
      ecmdOutput( printed.c_str() );

    }

  }


  if (!validPosFound) {
    //this is an error common across all UI functions
    ecmdOutputError("getcfam - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdPutCfamUser(int argc, char* argv[]) {

  uint32_t rc = ECMD_SUCCESS;
  std::string inputformat = "x";                ///< Default input format
  std::string dataModifier = "insert";          ///< Default data Modifier (And/Or/insert)
  ecmdDataBuffer fetchBuffer;                   ///< Buffer to store read/modify/write data
  ecmdLooperData looperdata;                    ///< Store internal Looper data
  ecmdChipTarget target;                        ///< Chip target being operated on
  uint32_t address;                             ///< Cfam address
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
    ecmdOutputError("putcfam - Too few arguments specified; you need at least a chip, an address, and some data.\n");
    ecmdOutputError("putcfam - Type 'putcfam -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  if (!ecmdIsAllHex(argv[1])) {
    ecmdOutputError("putcfam - Non-hex characters detected in address field\n");
    return ECMD_INVALID_ARGS;
  } else if (strlen(argv[1]) > 6) {
    ecmdOutputError("putcfam - Cfam addresses must be <= 24 bits in length\n");
    return ECMD_INVALID_ARGS;
  }
  address = ecmdGenB32FromHexRight(&address, argv[1]);

  /* Did they specify a start/numbits */
  if (argc > 3) {
    if (argc != 5) {
      ecmdOutputError("putcfam - Too many arguments specified; you probably added an unsupported option.\n");

      ecmdOutputError("putcfam - Type 'putcfam -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }

    if (!ecmdIsAllDecimal(argv[2])) {
      ecmdOutputError("putcfam - Non-decimal characters detected in startbit field\n");
      return ECMD_INVALID_ARGS;
    }
    startbit = atoi(argv[2]);
    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("putcfam - Non-decimal characters detected in numbits field\n");
      return ECMD_INVALID_ARGS;
    }
    numbits = atoi(argv[3]);


    /* Bounds check */
    if ((startbit + numbits) > ECMD_MAX_DATA_BITS) {
      char errbuf[100];
      sprintf(errbuf,"putcfam - Too much data requested > %d bits\n", ECMD_MAX_DATA_BITS);
      ecmdOutputError(errbuf);
      return ECMD_DATA_BOUNDS_OVERFLOW;
    } else if (numbits == 0) {
      ecmdOutputError("putcfam - Number of bits == 0, operation not performed\n");
      return ECMD_INVALID_ARGS;
    }

    rc = ecmdReadDataFormatted(buffer, argv[4], inputformat, numbits);
    if (rc) {
      ecmdOutputError("putcfam - Problems occurred parsing input data, must be an invalid format\n");
      return rc;
    }


  } else {

    rc = ecmdReadDataFormatted(buffer, argv[2], inputformat);
    if (rc) {
      ecmdOutputError("putcfam - Problems occurred parsing input data, must be an invalid format\n");
      return rc;
    }
  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdConfigLooperNext(target, looperdata)) {

    /* Do we need to perform a read/modify/write op ? */
    if ((dataModifier != "insert") || (startbit != -1)) {


      rc = getCfamRegister(target, address, fetchBuffer);

      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
      }
      else if (rc) {
        printed = "putcfam - Error occured performing getcfam on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }
      else {
        validPosFound = true;
      }

      rc = ecmdApplyDataModifier(fetchBuffer, buffer, (startbit == -1 ? 0 : startbit), dataModifier);
      if (rc) return rc;

      rc = putCfamRegister(target, address, fetchBuffer);
      if (rc) {
        printed = "putcfam - Error occured performing putcfam on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }

    }
    else {

      rc = putCfamRegister(target, address, buffer);
      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
      }
      else if (rc) {
        printed = "putcfam - Error occured performing putcfam on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }
      else {
        validPosFound = true;
      }

    }

    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      printed = ecmdWriteTarget(target) + "\n";
      ecmdOutput(printed.c_str());
    }

  }


  if (!validPosFound) {
    ecmdOutputError("putcfam - Unable to find a valid chip to execute command on\n");
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
