/* $Header$ */
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

uint32_t ecmdGetConfigUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  std::string configName;       ///< Name of config variable to fetch
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  bool formatSpecfied = false;   ///< Was the -o option specified
  ecmdConfigValid_t validOutput;  ///< Indicator if valueAlpha, valueNumeric (or both) are valid
  std::string  valueAlpha;       ///< Alpha value of setting (if appropriate)
  uint32_t  valueNumeric;        ///< Numeric value of setting (if appropriate)
  ecmdDataBuffer numData(32);	 ///< Initialise data buffer with the numeric value
  std::string printed;           ///< Print Buffer
  ecmdLooperData looperdata;     ///< Store internal Looper data

  int CAGE = 1, NODE = 2, SLOT = 3, POS = 4, CORE = 5;
  int depth = 0;                 ///< depth found from Command line parms

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

  if (ecmdParseOption(&argc, &argv, "-dk"))             depth = CAGE;
  else if (ecmdParseOption(&argc, &argv, "-dn"))        depth = NODE;
  else if (ecmdParseOption(&argc, &argv, "-ds"))        depth = SLOT;
  else if (ecmdParseOption(&argc, &argv, "-dp"))        depth = POS;
  else if (ecmdParseOption(&argc, &argv, "-dc"))        depth = CORE;

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
  if (argc < 1) {
    ecmdOutputError("getconfig - Too few arguments specified; you need at least a ConfigName.\n");
    ecmdOutputError("getconfig - Type 'getconfig -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }


  //Setup the target that will be used to query the system config
  if (argc > 2) {
    ecmdOutputError("getconfig - Too many arguments specified; you probably added an unsupported option.\n");
    ecmdOutputError("getconfig - Type 'getconfig -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if( argc == 2) {
    /* Apply the default depth */
    if (depth == 0) depth = POS;
    if (depth < POS) {
      ecmdOutputError("getconfig - Invalid Depth parm specified when a chipselect was specified.\n");
      return ECMD_INVALID_ARGS;
    }     
    target.chipType = argv[0];
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    configName = argv[1];
  } else {
    if (depth == 0) depth = CAGE;
    configName = argv[0];
  
  }
  /* Now set our states based on depth */
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_FIELD_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;
  if (depth == POS)             target.coreState = ECMD_TARGET_FIELD_UNUSED;
  else if (depth == SLOT)       target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
  else if (depth == NODE)       target.slotState = ECMD_TARGET_FIELD_UNUSED;
  else if (depth == CAGE)       target.nodeState = ECMD_TARGET_FIELD_UNUSED;

  
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdConfigLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    /* Actually go fetch the data */
    rc = ecmdGetConfiguration(target, configName, validOutput, valueAlpha, valueNumeric);

    if (rc) {
      printed = "getconfig - Error occured performing ecmdGetConfiguration on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
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

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("getconfig - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }
  // Now check if our coeRc accumulated anything and return if it has
  if (coeRc) return coeRc;

  return rc;
}


uint32_t ecmdSetConfigUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  std::string configName;       ///< Name of config variable to fetch
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  ecmdConfigValid_t validInput;  ///< Indicator if valueAlpha, valueNumeric (or both) are valid
  std::string  valueAlpha = "";  ///< Input of type Ascii   @01c add init
  uint32_t  valueNumeric= 0;     ///< Input of type Numeric @01c add init 
  ecmdDataBuffer inputBuffer;	 ///< Initialise data buffer with the numeric value(if input is Numeric)
  char inputVal[400];		 ///< Value to set configuration variable to
  std::string printed;           ///< Print Buffer
  ecmdLooperData looperdata;     ///< Store internal Looper data

  int CAGE = 1, NODE = 2, SLOT = 3, POS = 4, CORE = 5;
  int depth = 0;                 ///< depth found from Command line parms

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr == NULL) {
    format = "a";
  }
  else {
    format = formatPtr;
  }
  if (ecmdParseOption(&argc, &argv, "-dk"))             depth = CAGE;
  else if (ecmdParseOption(&argc, &argv, "-dn"))        depth = NODE;
  else if (ecmdParseOption(&argc, &argv, "-ds"))        depth = SLOT;
  else if (ecmdParseOption(&argc, &argv, "-dp"))        depth = POS;
  else if (ecmdParseOption(&argc, &argv, "-dc"))        depth = CORE;

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
    ecmdOutputError("setconfig - Too few arguments specified; you need at least a ConfigName, Value to set it to.\n");
    ecmdOutputError("setconfig - Type 'setconfig -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config
  if (argc > 3) {
    ecmdOutputError("setconfig - Too many arguments specified; you probably added an unsupported option.\n");
    ecmdOutputError("setconfig - Type 'setconfig -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if( argc == 3) {
    /* Apply the default depth */
    if (depth == 0) depth = POS;
    if (depth < POS) {
      ecmdOutputError("setconfig - Invalid Depth parm specified when a chipselect was specified.\n");
      return ECMD_INVALID_ARGS;
    }     
    target.chipType = argv[0];
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    configName = argv[1];
    strcpy(inputVal, argv[2]);
  }
  else {
    if (depth == 0) depth = CAGE;
    target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
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
    uint32_t numBits = inputBuffer.getBitLength();
    if(numBits > 32 ) {
     ecmdOutputError("setconfig - Problems occurred parsing input data, Bitlength should be <= 32 bits\n");
     return ECMD_INVALID_ARGS;
    }
    inputBuffer.shiftRightAndResize(32-numBits);
    valueNumeric = inputBuffer.getWord(0);
    validInput = ECMD_CONFIG_VALID_FIELD_NUMERIC;
  }
    
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_FIELD_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  /* Now set our states based on depth */
  if (depth == POS)             target.coreState = ECMD_TARGET_FIELD_UNUSED;
  else if (depth == SLOT)       target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
  else if (depth == NODE)       target.slotState = ECMD_TARGET_FIELD_UNUSED;
  else if (depth == CAGE)       target.nodeState = ECMD_TARGET_FIELD_UNUSED;

  
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdConfigLooperNext(target, looperdata) && (!coeRc || coeMode)) {
    /* Actually go fetch the data */
    rc = ecmdSetConfiguration(target, configName, validInput, valueAlpha, valueNumeric);
    if (rc) {
      printed = "setconfig - Error occured performing ecmdSetConfiguration on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }
    else {
      validPosFound = true;
    }
    printed = ecmdWriteTarget(target) + "\n";
    ecmdOutput( printed.c_str() );
  }

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("setconfig - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }
  // Now check if our coeRc accumulated anything and return if it has
  if (coeRc) return coeRc;

  return rc;
}

uint32_t ecmdGetCfamUser(int argc, char* argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

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

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 2) {  //chip + address
    ecmdOutputError("getcfam - Too few arguments specified; you need at least a chip and an address.\n");
    ecmdOutputError("getcfam - Type 'getcfam -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (chipUnitType != "") {
    ecmdOutputError("getcfam - chipUnit specified on the command line, this function doesn't support chipUnits.\n");
    return ECMD_INVALID_ARGS;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

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
  if (argc > 2) { 
    ecmdOutputError("getcfam - Too many arguments specified; you probably added an option that wasn't recognized.\n");
    ecmdOutputError("getcfam - Type 'getcfam -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;


  while (ecmdConfigLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    rc = getCfamRegister(target, address, buffer);
    if (rc) {
        printed = "getcfam - Error occured performing getcfam on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
    }
    else {
      validPosFound = true;
    }

    if (expectFlag) {

      if (maskFlag) {
        buffer.setAnd(mask, 0, buffer.getBitLength());
      }

      uint32_t mismatch = ECMD_UNSET;
      if (!ecmdCheckExpected(buffer, expected, mismatch)) {

        //@ make this stuff sprintf'd
        char outstr[75];
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


  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("getcfam - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }
  // Now check if our coeRc accumulated anything and return if it has
  if (coeRc) return coeRc;

  return rc;
}

uint32_t ecmdPutCfamUser(int argc, char* argv[]) {

  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;
  std::string inputformat = "x";                ///< Default input format
  std::string dataModifier = "insert";          ///< Default data Modifier (And/Or/insert)
  ecmdDataBuffer fetchBuffer;                   ///< Buffer to store read/modify/write data
  ecmdLooperData looperdata;                    ///< Store internal Looper data
  ecmdChipTarget target;                        ///< Chip target being operated on
  uint32_t address;                             ///< Cfam address
  ecmdDataBuffer buffer;                        ///< Container to store write data
  bool validPosFound = false;                   ///< Did the config looper actually find a chip ?
  std::string printed;                          ///< String for printed data
  uint32_t startbit = ECMD_UNSET;               ///< Startbit to insert data
  uint32_t numbits = 0;                         ///< Number of bits to insert data

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

  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/

  if (argc < 3) {  //chip + address + some data
    ecmdOutputError("putcfam - Too few arguments specified; you need at least a chip, an address, and some data.\n");
    ecmdOutputError("putcfam - Type 'putcfam -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (chipUnitType != "") {
    ecmdOutputError("putcfam - chipUnit specified on the command line, this function doesn't support chipUnits.\n");
    return ECMD_INVALID_ARGS;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

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
    startbit = (uint32_t)atoi(argv[2]);
    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("putcfam - Non-decimal characters detected in numbits field\n");
      return ECMD_INVALID_ARGS;
    }
    numbits = (uint32_t)atoi(argv[3]);


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

    rc = ecmdReadDataFormatted(buffer, argv[4], inputformat, (int)numbits);
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

  while (ecmdConfigLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    /* Do we need to perform a read/modify/write op ? */
    if ((dataModifier != "insert") || (startbit != ECMD_UNSET)) {


      rc = getCfamRegister(target, address, fetchBuffer);

      if (rc) {
        printed = "putcfam - Error occured performing getcfam on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
      }
      else {
        validPosFound = true;
      }

      rc = ecmdApplyDataModifier(fetchBuffer, buffer, (startbit == ECMD_UNSET ? 0 : startbit), dataModifier);
      if (rc) break;

      rc = putCfamRegister(target, address, fetchBuffer);
      if (rc) {
        printed = "putcfam - Error occured performing putcfam on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
      }

    }
    else {

      rc = putCfamRegister(target, address, buffer);
      if (rc) {
        printed = "putcfam - Error occured performing putcfam on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
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


  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("putcfam - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }
  // Now check if our coeRc accumulated anything and return if it has
  if (coeRc) return coeRc;

  return rc;
}

uint32_t ecmdMakeSPSystemCallUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  std::string command;          ///< Print Buffer
  std::string standardop = "";       ///< Standard out captured by running command
  ecmdLooperData looperdata;    ///< Store internal Looper data


  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  //Pull out the system call
  if(argc == 0) {
      ecmdOutputError("makespsystemcall - Command to run on the SE/SP not specified.\n");
      ecmdOutputError("makespsystemcall - Type 'makespsystemcall -h' for usage.\n");
      return ECMD_INVALID_ARGS;
  }
  else {
      command = "";
      for(int i=0; i<argc; i++) {
        command += argv[i];
        command += " ";
      }
  }

  target.cageState = target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
  target.slotState = target.chipTypeState = target.posState = target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdConfigLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    /* Actually go fetch the data */
    rc = makeSPSystemCall(target, command, standardop);
    if (rc) {
      printed = "makespsystemcall - Error occured performing makeSPSystemCall on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      if (standardop.length() != 0) {
       printed = "makespsystemcall - Output from executing the command '" + command + "':\n\n";
       ecmdOutput( printed.c_str() );
       ecmdOutput( standardop.c_str() );
      }
      coeRc = rc;
      continue;
    }
    else {
      validPosFound = true;
    }
    //Print Output
    printed = "makespsystemcall - Output from executing the command '" + command + "':\n\n";
    ecmdOutput( printed.c_str() );
    
    if(standardop.length() != 0) {
     ecmdOutput( standardop.c_str() );
    }
    else {
     ecmdOutput( "No Output received from makeSPSystemCall\n");
    }
  }

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("makespsystemcall - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }
  // Now check if our coeRc accumulated anything and return if it has
  if (coeRc) return coeRc;

  return rc;
}


uint32_t ecmdDeconfigUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;           ///< Print Buffer
  ecmdLooperData looperdata;     ///< Store internal Looper data


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

  //Setup the target that will be used to query the system config
  if( argc == 1) {
   target.chipType = argv[0];
   target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  }
  else {
   target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
  }

  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_FIELD_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP_VD, looperdata);
  if (rc) return rc;

  while (ecmdConfigLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    /* Actually go fetch the data */
    rc = ecmdDeconfigureTarget(target);

    if (rc) {
      printed = "deconfig - Error occured performing ecmdDeconfigureTarget on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }
    else {
      validPosFound = true;
    }

    printed = ecmdWriteTarget(target) + "deconfigured.\n";
    ecmdOutput( printed.c_str() );
  }

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("getconfig - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }
  // Now check if our coeRc accumulated anything and return if it has
  if (coeRc) return coeRc;

  return rc;
}

uint32_t ecmdReconfigUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  uint8_t ONE = 0;
  uint8_t MANY = 1;

  ecmdChipTarget target;        ///< Current target
  std::string printed;           ///< Print Buffer
  
  std::string cage;	
  std::string node;	
  std::string slot;	
  std::string pos;	
  std::string core;	
  
  uint8_t cageType;
  uint8_t nodeType;
  uint8_t slotType;
  uint8_t posType;
  uint8_t coreType;
  
  
  std::list<uint32_t>              cageList;
  std::list<uint32_t>::iterator    cageListIter;
  std::list<uint32_t>              nodeList;
  std::list<uint32_t>::iterator    nodeListIter;
  std::list<uint32_t>              slotList;
  std::list<uint32_t>::iterator    slotListIter;
  std::list<uint32_t>              posList;
  std::list<uint32_t>::iterator    posListIter;
  std::list<uint32_t>              coreList;
  std::list<uint32_t>::iterator    coreListIter;
  
  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/


  if(ecmdParseOption(&argc, &argv, "-all")) {
     printed = "reconfig - 'all' option for targets not supported in reconfig.\n";
     ecmdOutputError( printed.c_str() );
     return rc;
  }


  //Set all the states Unused to begin with. Then set them to valid based on the args
  target.cageState     = ECMD_TARGET_FIELD_UNUSED;
  target.nodeState     = ECMD_TARGET_FIELD_UNUSED;
  target.slotState     = ECMD_TARGET_FIELD_UNUSED;
  target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
  target.posState      = ECMD_TARGET_FIELD_UNUSED;
  target.coreState     = ECMD_TARGET_FIELD_UNUSED;
  target.threadState   = ECMD_TARGET_FIELD_UNUSED;
  
  
  rc = ecmdParseTargetFields(&argc, &argv, "cage", target, cageType, cage);
  if(rc) return rc;
  rc = ecmdParseTargetFields(&argc, &argv, "node", target, nodeType, node);
  if(rc) return rc;
  rc = ecmdParseTargetFields(&argc, &argv, "slot", target, slotType, slot);
  if(rc) return rc;
  rc = ecmdParseTargetFields(&argc, &argv, "pos", target, posType, pos);
  if(rc) return rc;
  rc = ecmdParseTargetFields(&argc, &argv, "core", target, coreType, core);
  if(rc) return rc;
  
  
  

  //Thread
  char *targetPtr = ecmdParseOptionWithArgs(&argc, &argv, "-t");
  if(targetPtr != NULL) {
    ecmdOutputError("reconfig - Thread (-t#) argument not supported\n");
    return ECMD_INVALID_ARGS;
  }

  //ChipType
  if( argc == 1) {
   target.chipType = argv[0];
   target.cageState = ECMD_TARGET_FIELD_VALID;
   target.nodeState = ECMD_TARGET_FIELD_VALID;
   target.slotState = ECMD_TARGET_FIELD_VALID;
   target.chipTypeState = ECMD_TARGET_FIELD_VALID;
   target.posState = ECMD_TARGET_FIELD_VALID;
  }
  else {
   target.posState = ECMD_TARGET_FIELD_UNUSED;
   target.coreState = ECMD_TARGET_FIELD_UNUSED;
  }
  
  //No Args Case
  if(target.cageState == ECMD_TARGET_FIELD_UNUSED) {
    ecmdOutputError("reconfig - No target args specified.\n");
    return ECMD_INVALID_ARGS;
  }
    
  //Go through each of the targets and configure them 
  if (cageType == ONE) {
    cageList.push_back(target.cage);
  }
  else if (cageType == MANY) {
    getTargetList(cage, cageList);
  }
  if (cageList.empty()) {
    ecmdOutputError("reconfig - no cage input found.\n");
    return ECMD_INVALID_CONFIG;
  }
  /* STart walking the cages */
  for (cageListIter = cageList.begin(); cageListIter != cageList.end(); cageListIter++) {
     target.cage = *cageListIter;
     
     nodeList.clear();

     if (nodeType == ONE) {
       nodeList.push_back(target.node);
     }
     else if (nodeType == MANY) {
       getTargetList(node, nodeList);
     }
	
     for (nodeListIter = nodeList.begin(); nodeListIter != nodeList.end(); nodeListIter ++) {
     	target.node = *nodeListIter;
       
     	slotList.clear();
     	
     	if (slotType == ONE) {
     	   slotList.push_back(target.slot);
     	}
     	else if (slotType == MANY) {
     	   getTargetList(slot, slotList);
     	}

	 /* Walk the slots */
        for (slotListIter = slotList.begin(); slotListIter != slotList.end(); slotListIter ++) {
          target.slot = *slotListIter;
	  
	 /* Walk the slots */
         if (target.chipTypeState != ECMD_TARGET_FIELD_UNUSED) {
           
	  posList.clear();
          //ChipType has been previously set
	  if (posType == ONE) {
           posList.push_back(target.pos);
          }
          else if (posType == MANY) {
           getTargetList(pos, posList);
          }

	  /* Now start walking chip pos's */
	  for (posListIter = posList.begin(); posListIter != posList.end(); posListIter ++) {
	    target.pos = *posListIter;
	   
	    coreList.clear();

	    if (coreType == ONE) {
              coreList.push_back(target.core);
            }
            else if (coreType == MANY) {
              getTargetList(core, coreList);
            }
	    
	    /* Ok, walk the cores */
            for (coreListIter = coreList.begin(); coreListIter != coreList.end(); coreListIter ++) {
	      target.core = (uint8_t)*coreListIter;

	      rc = ecmdConfigureTarget(target);

 	     if (rc == ECMD_TARGET_NOT_CONFIGURED) {
 	      printed = "reconfig - Error occured performing ecmdConfigureTarget on ";
 	      printed += ecmdWriteTarget(target) + ". Target is not available in the system.\n";
 	      ecmdOutputError( printed.c_str() );
 	      continue;
 	     }
 	     else if (rc) {
 	      printed = "reconfig - Error occured performing ecmdConfigureTarget on ";
 	      printed += ecmdWriteTarget(target) + "\n";
 	      ecmdOutputError( printed.c_str() );
 	      return rc;
 	     }
             printed = ecmdWriteTarget(target) + "configured.\n";
 	     ecmdOutput( printed.c_str() );
	
	    }//Loop cores

	       
	  }//Loop pos 

	 }// end if chiptype Used
	 else {
	   rc = ecmdConfigureTarget(target);

 	   if (rc == ECMD_TARGET_NOT_CONFIGURED) {
 	      printed = "reconfig - Error occured performing ecmdConfigureTarget on ";
 	      printed += ecmdWriteTarget(target) + ". Target is not available in the system.\n";
 	      ecmdOutputError( printed.c_str() );
 	      continue;
 	   }
 	   else if (rc) {
 	     printed = "reconfig - Error occured performing ecmdConfigureTarget on ";
 	     printed += ecmdWriteTarget(target) + "\n";
 	     ecmdOutputError( printed.c_str() );
 	     return rc;
 	   }
           printed = ecmdWriteTarget(target) + "configured.\n";
 	   ecmdOutput( printed.c_str() );
	 
	 }

	}// Loop slots 
	    
      } //Loop nodes 
      
    }// Loop cages

  return rc;
}


uint32_t ecmdGetGpRegisterUser(int argc, char* argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  bool expectFlag = false;
  bool maskFlag = false;
  char* expectPtr = NULL;                       ///< Pointer to expected data in arg list
  char* maskPtr = NULL;                         ///< Pointer to mask data in arg list
  ecmdDataBuffer expected;                      ///< Buffer to store expected data
  ecmdDataBuffer mask;                          ///< Buffer for mask of expected data
  std::string outputformat = "x";               ///< Output Format to display
  std::string inputformat = "x";                ///< Input format of data
  ecmdChipTarget target;                        ///< Current target being operated on
  ecmdDataBuffer buffer;                        ///< Buffer to hold gp register data
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

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 2) {  //chip + address
    ecmdOutputError("getgpreg - Too few arguments specified; you need at least a chip and a gpregister.\n");
    ecmdOutputError("getgpreg - Type 'getgpreg -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (chipUnitType != "") {
    ecmdOutputError("getgpreg - chipUnit specified on the command line, this function doesn't support chipUnits.\n");
    return ECMD_INVALID_ARGS;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get address to fetch
  if (!ecmdIsAllDecimal(argv[1])) {
    ecmdOutputError("getgpreg - Non-decimal characters detected in reg num field\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t gpRegister = (uint32_t)atoi(argv[1]);


  if (expectFlag) {

    rc = ecmdReadDataFormatted(expected, expectPtr, inputformat);
    if (rc) {
      ecmdOutputError("getgpreg - Problems occurred parsing expected data, must be an invalid format\n");
      return rc;
    }

    if (maskFlag) {
      rc = ecmdReadDataFormatted(mask, maskPtr, inputformat);
      if (rc) {
        ecmdOutputError("getgpreg - Problems occurred parsing mask data, must be an invalid format\n");
        return rc;
      }

    }


  }
  if (argc > 2) { 
    ecmdOutputError("getgpreg - Too many arguments specified; you probably added an option that wasn't recognized.\n");
    ecmdOutputError("getgpreg - Type 'getgpreg -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;


  while (ecmdConfigLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    rc = getGpRegister(target, gpRegister, buffer);
    if (rc) {
        printed = "getgpreg - Error occured performing getgpreg on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
    }
    else {
      validPosFound = true;
    }

    if (expectFlag) {

      if (maskFlag) {
        buffer.setAnd(mask, 0, buffer.getBitLength());
      }

      uint32_t mismatch = ECMD_UNSET;
      if (!ecmdCheckExpected(buffer, expected, mismatch)) {

        //@ make this stuff sprintf'd
        char outstr[75];
        printed = ecmdWriteTarget(target);
        sprintf(outstr, "\ngetgpreg - Data miscompare occured at address: %d\n", gpRegister);
        printed += outstr;
        ecmdOutputError( printed.c_str() );


        printed = "getgpreg - Actual";
        if (maskFlag) {
          printed += " (with mask): ";
        }
        else {
          printed += "            : ";
        }

        printed += ecmdWriteDataFormatted(buffer, outputformat);
        ecmdOutputError( printed.c_str() );

        printed = "getgpreg - Expected          : ";
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

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("getgpreg - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }
  // Now check if our coeRc accumulated anything and return if it has
  if (coeRc) return coeRc;

  return rc;
}

uint32_t ecmdPutGpRegisterUser(int argc, char* argv[]) {

  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;
  std::string inputformat = "x";                ///< Default input format
  std::string dataModifier = "insert";          ///< Default data Modifier (And/Or/insert)
  ecmdDataBuffer fetchBuffer;                   ///< Buffer to store read/modify/write data
  ecmdLooperData looperdata;                    ///< Store internal Looper data
  ecmdChipTarget target;                        ///< Chip target being operated on
  ecmdDataBuffer buffer;                        ///< Container to store write data
  bool validPosFound = false;                   ///< Did the config looper actually find a chip ?
  std::string printed;                          ///< String for printed data
  uint32_t startbit = ECMD_UNSET;               ///< Startbit to insert data
  uint32_t numbits = 0;                         ///< Number of bits to insert data

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

  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/

  if (argc < 3) {  //chip + gpRegister + some data
    ecmdOutputError("putgpreg - Too few arguments specified; you need at least a chip, reg num, and some data.\n");
    ecmdOutputError("putgpreg - Type 'putgpreg -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (chipUnitType != "") {
    ecmdOutputError("putgpreg - chipUnit specified on the command line, this function doesn't support chipUnits.\n");
    return ECMD_INVALID_ARGS;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  if (!ecmdIsAllDecimal(argv[1])) {
    ecmdOutputError("putgpreg - Non-decimal characters detected in reg num field\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t gpRegister = (uint32_t)atoi(argv[1]);

  /* Did they specify a start/numbits */
  if (argc > 3) {
    if (argc != 5) {
      ecmdOutputError("putgpreg - Too many arguments specified; you probably added an unsupported option.\n");

      ecmdOutputError("putgpreg - Type 'putgpreg -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }

    if (!ecmdIsAllDecimal(argv[2])) {
      ecmdOutputError("putgpreg - Non-decimal characters detected in startbit field\n");
      return ECMD_INVALID_ARGS;
    }
    startbit = (uint32_t)atoi(argv[2]);
    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("putgpreg - Non-decimal characters detected in numbits field\n");
      return ECMD_INVALID_ARGS;
    }
    numbits = (uint32_t)atoi(argv[3]);


    /* Bounds check */
    if ((startbit + numbits) > ECMD_MAX_DATA_BITS) {
      char errbuf[100];
      sprintf(errbuf,"putgpreg - Too much data requested > %d bits\n", ECMD_MAX_DATA_BITS);
      ecmdOutputError(errbuf);
      return ECMD_DATA_BOUNDS_OVERFLOW;
    } else if (numbits == 0) {
      ecmdOutputError("putgpreg - Number of bits == 0, operation not performed\n");
      return ECMD_INVALID_ARGS;
    }

    rc = ecmdReadDataFormatted(buffer, argv[4], inputformat, (int)numbits);
    if (rc) {
      ecmdOutputError("putgpreg - Problems occurred parsing input data, must be an invalid format\n");
      return rc;
    }


  } else {

    rc = ecmdReadDataFormatted(buffer, argv[2], inputformat);
    if (rc) {
      ecmdOutputError("putgpreg - Problems occurred parsing input data, must be an invalid format\n");
      return rc;
    }
  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdConfigLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    /* Do we need to perform a read/modify/write op ? */
    if ((dataModifier != "insert") || (startbit != ECMD_UNSET)) {

      rc = getGpRegister(target, gpRegister, fetchBuffer);
      if (rc) {
        printed = "putgpreg - Error occured performing putgpreg on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
      }
      else {
        validPosFound = true;
      }

      rc = ecmdApplyDataModifier(fetchBuffer, buffer, (startbit == ECMD_UNSET ? 0 : startbit), dataModifier);
      if (rc) break;

      rc = putGpRegister(target, gpRegister, fetchBuffer);
      if (rc) {
        printed = "putgpreg - Error occured performing putgpreg on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
      }

    }
    else {

      rc = putGpRegister(target, gpRegister, buffer);
      if (rc) {
        printed = "putgpreg - Error occured performing putgpreg on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
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


  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("putgpreg - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }
  // Now check if our coeRc accumulated anything and return if it has
  if (coeRc) return coeRc;

  return rc;
}

uint32_t ecmdEchoUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;
  std::string message;

  bool warning = false;
  bool error = false;

  warning = ecmdParseOption(&argc, &argv, "-warning");
  error = ecmdParseOption(&argc, &argv, "-error");

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc < 1) {
    ecmdOutputError("simecho - At least one argument (a message to print) is required for simecho.\n");
    return ECMD_INVALID_ARGS;
  }
  for (int idx = 0; idx < argc; idx ++) {
    message += argv[idx];
    message += " ";
  }
  message += "\n";

  if (error) {
    ecmdOutputError(message.c_str());
  } else if (warning) {
    ecmdOutputWarning(message.c_str());
  } else {
    ecmdOutput(message.c_str());
  }

  return rc;

}

//Adding the ecmdUnitIdUser function

uint32_t ecmdUnitIdUser(int argc, char* argv[]) {
    
  uint32_t rc = ECMD_SUCCESS;
  ecmdLooperData looperdata; 
  std::string output;
  char buf[1000]; 
  ecmdChipTarget target;
  
  if (argc < 1) {
    ecmdOutputError("unitid - Too few arguments specified,Type 'unitid -h' for help.\n");
    return ECMD_INVALID_ARGS;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  
  //Parsing the CmdLine for uid2tgt
  if (!strcmp(argv[0], "uid2tgt")){
  
    if(argv[0] != NULL) {
        rc = ecmdCommandArgs(&argc, &argv);
        if (rc) return rc;
    }
  
    if (argc < 2) {
        ecmdOutputError("unitid - Too few arguments specified for 'uid2tgt' CmdLine,you need at least a 'unitid' in 'hex' or 'string' format\n");
        ecmdOutputError("unitid - Type 'unitid -h' for usage.\n");
        return ECMD_INVALID_ARGS;
    }
    
    if (argc > 2) {
        ecmdOutputError("unitid - Too many arguments specified,you probably added an unsupported option.\n");
        ecmdOutputError("unitid - Type 'unitid -h' for usage.\n");
        return ECMD_INVALID_ARGS;
    }

    
  //define the target list,which will be obtained when the unitid will get processed
      
    std::list<ecmdChipTarget> target_list;
    std::list<ecmdChipTarget>::iterator target_list_iterator;
    
    rc = ecmdUnitIdStringToTarget(argv[1], target_list);
    if (rc){
        sprintf(buf,"ecmdUnitIdStringToTarget Failed with rc = 0x%x and unitId = %s\n",rc,argv[1]);
        output = (std::string)buf;
        ecmdOutputError(output.c_str());
        return rc;
    }  
    if(target_list.size()==1){
        
        target_list_iterator = target_list.begin();
        sprintf(buf,"For Inputs Id = %s, Target is '%s' \n", argv[1],(ecmdWriteTarget(*target_list_iterator)).c_str());
        output = (std::string)buf;    
        ecmdOutput(output.c_str());          
    }else{
        
        output="This is a Group Id Input ..... Multiple Targets will Return For Input Id: ";
        output+=argv[1];
        output+="\n";
        ecmdOutput(output.c_str());
        for(target_list_iterator = target_list.begin();target_list_iterator != target_list.end();target_list_iterator++){
            sprintf(buf,"'%s' \t",(ecmdWriteTarget(*target_list_iterator)).c_str());
            output = (std::string)buf;    
            ecmdOutput(output.c_str());  
        }
    }
  //Parsing the CmdLine for tgt2uid 
  }else if(!strcmp(argv[0], "tgt2uid")){
    
  /*This code is based on 'ecmdquery configd' such that we loop through all the
    possible tgts that the user asked for and determine the unitid.*/
     
    uint8_t ONE = 0;
    uint8_t MANY = 1;

    std::string cage;   
    std::string node;   
    std::string slot;   
    std::string pos;    
    std::string core;
    std::string thread;   
    
    uint8_t cageType;
    uint8_t nodeType;
    uint8_t slotType;
    uint8_t posType;
    uint8_t coreType;
    uint8_t threadType;
  
    std::list<uint32_t>          cageList;
    std::list<uint32_t>::iterator    cageListIter;
    std::list<uint32_t>          nodeList;
    std::list<uint32_t>::iterator    nodeListIter;
    std::list<uint32_t>          slotList;
    std::list<uint32_t>::iterator    slotListIter;
    std::list<uint32_t>          posList;
    std::list<uint32_t>::iterator    posListIter;
    std::list<uint32_t>          coreList;
    std::list<uint32_t>::iterator    coreListIter;
    std::list<uint32_t>          threadList;
    std::list<uint32_t>::iterator    threadListIter;
   
    /************************************************************************/
    /* Parse Common Cmdline Args                                            */
    /************************************************************************/

    //Set all the states Unused to begin with. Then set them to valid based on the args
    target.cageState     = ECMD_TARGET_FIELD_UNUSED;
    target.nodeState     = ECMD_TARGET_FIELD_UNUSED;
    target.slotState     = ECMD_TARGET_FIELD_UNUSED;
    target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
    target.posState      = ECMD_TARGET_FIELD_UNUSED;
    target.coreState     = ECMD_TARGET_FIELD_UNUSED;
    target.threadState   = ECMD_TARGET_FIELD_UNUSED;
    
    rc = ecmdParseTargetFields(&argc, &argv, "cage", target, cageType, cage);
    if(rc) return rc;
    rc = ecmdParseTargetFields(&argc, &argv, "node", target, nodeType, node);
    if(rc) return rc;
    rc = ecmdParseTargetFields(&argc, &argv, "slot", target, slotType, slot);
    if(rc) return rc;
    rc = ecmdParseTargetFields(&argc, &argv, "pos", target, posType, pos);
    if(rc) return rc;
    rc = ecmdParseTargetFields(&argc, &argv, "core", target, coreType, core);
    if(rc) return rc;
    rc = ecmdParseTargetFields(&argc, &argv, "thread", target, threadType, thread);
    if(rc) return rc;
    
    //checking the Target for unitid to obtain
    if (argc < 2) {
        ecmdOutputError("unitid - Too few arguments specified for 'tgt2uid' CmdLine,you need at least a 'targetName'.\n");
        ecmdOutputError("unitid - Type 'unitId -h' for usage.\n");
        return ECMD_INVALID_ARGS;
    }
    else {
        target.chipType = argv[1];
        target.chipTypeState = ECMD_TARGET_FIELD_VALID;
        target.cageState = target.nodeState = target.slotState = target.posState =target.coreState= target.threadState= ECMD_TARGET_FIELD_VALID;     
    }
       
    //Go through each of the targets and check if they are configured  
    if (cageType == ONE) {
      cageList.push_back(target.cage);
    }
    else if (cageType == MANY) {
      getTargetList(cage, cageList);
    }
    if (cageList.empty()) { 
      ecmdOutputError("unitid - no cage input found.\n");
      return ECMD_INVALID_CONFIG;
    }
    // Start walking the cages 
    for (cageListIter = cageList.begin(); cageListIter != cageList.end(); cageListIter++) {
       
       target.cage = *cageListIter;
       nodeList.clear();

       if (nodeType == ONE) {
            nodeList.push_back(target.node);
       }
       else if (nodeType == MANY) {
            getTargetList(node, nodeList);
       }
    
       for (nodeListIter = nodeList.begin(); nodeListIter != nodeList.end(); nodeListIter ++) {
        
          target.node = *nodeListIter;
          slotList.clear();
        
          if (slotType == ONE) {
             slotList.push_back(target.slot);
          }
          else if (slotType == MANY) {
             getTargetList(slot, slotList);
          }

          /* Walk the slots */
          for (slotListIter = slotList.begin(); slotListIter != slotList.end(); slotListIter ++) {
      
             target.slot = *slotListIter;
           
             /* Walk the slots */
             if (target.chipTypeState != ECMD_TARGET_FIELD_UNUSED) {
           
                posList.clear();
                //ChipType has been previously set
                if (posType == ONE) {
                    posList.push_back(target.pos);
                }
                else if (posType == MANY) {
                    getTargetList(pos, posList);
                }

                /* Now start walking chip pos's */
                for (posListIter = posList.begin(); posListIter != posList.end(); posListIter ++) {
        
                    target.pos = *posListIter;
                    coreList.clear();

                    if (coreType == ONE) {
                        coreList.push_back(target.core);
                    }
                    else if (coreType == MANY) {
                        getTargetList(core, coreList);
                    }
        
                    /* Ok, walk the cores */
                    for (coreListIter = coreList.begin(); coreListIter != coreList.end(); coreListIter ++) {
          
                        target.core = (uint8_t)(*coreListIter);
                        threadList.clear();

                        if (threadType == ONE) {
                            threadList.push_back(target.thread);
                        }
                        else if (threadType == MANY) {
                            getTargetList(thread, threadList);
                        }
        
                        // Ok, walk the threads 
                        for (threadListIter = threadList.begin(); threadListIter != threadList.end(); threadListIter ++) {
                        
                            target.thread = (uint8_t)(*threadListIter); 
                        
                            rc = ecmdTargetToUnitId(target);
                            if (rc){ 
                                return rc;
                            }
                            
                            sprintf(buf,"For target = %s, unitId is '0x%X' \n",(ecmdWriteTarget(target)).c_str(),target.unitId);
        
                            output = (std::string)buf;
                            ecmdOutput(output.c_str());
               
                        }//Loop threads
               
                    }//Loop cores
           
                }//Loop pos 

             }// end if chiptype Used
     
          }// Loop slots 
        
       }//Loop nodes 
      
    }// Loop cages  
  }else{
    ecmdOutputError("unitid- Invalid arguments-Type 'unitid -h' for correct arguments\n");
    return ECMD_INVALID_ARGS;
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
