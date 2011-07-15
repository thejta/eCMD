/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File ecmdI2cGpioUser.C                                  
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

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <stdio.h>
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
#ifndef ECMD_REMOVE_I2C_FUNCTIONS
uint32_t ecmdGetI2cUser(int argc, char * argv[]) {
  uint32_t coeRc = ECMD_SUCCESS;
  uint32_t rc = ECMD_SUCCESS;
  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string outputformat = "xl";      ///< Output Format to display
  ecmdChipTarget target;                ///< Current target operating on
  ecmdDataBuffer data;                  ///< I2C Data from the specified engine/port/device
  bool validPosFound = false;           ///< Did the looper find anything to execute on
  bool outputformatflag = false;
  std::string newFilename;              ///< filename with target postfix incase of multi positions
  ecmdChipTarget target1;               ///< Current target operating on-for second looper
  ecmdLooperData looperdata1;           ///< looper to do the real work
  char targetStr[50];                   ///< target postfix for the filename incase of multi positions
  int targetCount=0;                    ///< counts the number of targets user specified
  ecmdI2cBusSpeed_t busspeed = ECMD_I2C_BUSSPEED_400KHZ; ///< bus speed to run i2c in khz
  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  
  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr != NULL) {
    outputformat = formatPtr;
    outputformatflag = true;
  }
  
  /* Get the filename if -f is specified */
  char * filename = ecmdParseOptionWithArgs(&argc, &argv, "-f");
  std::string printed;

  if((filename != NULL) && (outputformatflag) ) {
    printed = "geti2c - Options '-f' cannot be specified with '-o' option.\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
  } 
  
  /* get the bus speed, if it's there */
  char * busspeedstr = ecmdParseOptionWithArgs(&argc, &argv, "-busspeed");
  if (busspeedstr != NULL) {
    if (strcmp(busspeedstr, "50")==0) {
      busspeed = ECMD_I2C_BUSSPEED_50KHZ;
    } else if (strcmp(busspeedstr, "100")==0) {
      busspeed = ECMD_I2C_BUSSPEED_100KHZ;
    } else if (strcmp(busspeedstr, "400")!=0) {
      printed = "geti2c - Invalid value for busspeed. Possible values are - 400, 100, 50.\n";
      ecmdOutputError(printed.c_str());
      return ECMD_INVALID_ARGS;
    }
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
  if (argc < 5) {  
    ecmdOutputError("geti2c - Too few arguments specified; you need at least a chip, engineId, port, slaveAddr, numbytes.\n");
    ecmdOutputError("geti2c - Type 'geti2c -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if ((argc > 5) && (argc < 7)) {
    ecmdOutputError("geti2c - Too few arguments specified; you need at least a chip, engineId, port, slaveAddr, numbytes, offset and fieldsize.\n");
    ecmdOutputError("geti2c - Type 'geti2c -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if ( argc > 7) {
    ecmdOutputError("geti2c - Too many arguments specified; you only need chip, engineId, port, slaveAddr, numbytes, offset and fieldsize.\n");
    ecmdOutputError("geti2c - Type 'geti2c -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (chipUnitType != "") {
    ecmdOutputError("geti2c - chipUnit specified on the command line, this function doesn't support chipUnits.\n");
    return ECMD_INVALID_ARGS;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  target1 = target; //Created for the second looper needed for -f case with multiple positions
  
  if (!ecmdIsAllDecimal(argv[1])) {
    ecmdOutputError("geti2c - Non-decimal numbers detected in engineId field\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint32_t engineId = (uint32_t)atoi(argv[1]);
  
  if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("geti2c - Non-decimal numbers detected in port field\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint32_t port = (uint32_t)atoi(argv[2]);
  
  if (!ecmdIsAllHex(argv[3])) {
    ecmdOutputError("geti2c - Non-hex characters detected in slaveAddr field\n");
    return ECMD_INVALID_ARGS;
  } else if (strlen(argv[3]) > 8) {
    ecmdOutputError("geti2c - slave Address must be <= 32 bits in length\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint32_t slaveAddr = ecmdGenB32FromHexRight(&slaveAddr, argv[3]);
  
  if (!ecmdIsAllDecimal(argv[4])) {
    ecmdOutputError("geti2c - Non-decimal numbers detected in numBytes field\n");
    return ECMD_INVALID_ARGS;
  } 
  
  uint32_t numBytes = (uint32_t)atoi(argv[4]);
  
  uint32_t offset = 0, fieldSize = 0; 
  if (argc > 5) {
    if (!ecmdIsAllDecimal(argv[5])) {
      ecmdOutputError("geti2c - Non-decimal numbers detected in the offset field\n");
      return ECMD_INVALID_ARGS;
    }
    
    offset = (uint32_t)atoi(argv[5]);
    
    if (!ecmdIsAllDecimal(argv[6])) {
      ecmdOutputError("geti2c - Non-decimal numbers detected in fieldSize field\n");
      return ECMD_INVALID_ARGS;
    }
    
    fieldSize = (uint32_t)atoi(argv[6]);
  }
  
  //Run the loop to Check the number of targets
  if (filename != NULL) {
    rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
    if (rc) return rc;
 
    while ( ecmdLooperNext(target, looperdata) ) {
      targetCount++;
    }
  }
  
  //Looper to do the actual work
  rc = ecmdLooperInit(target1, ECMD_SELECTED_TARGETS_LOOP, looperdata1);
  if (rc) return rc;
              
  while ( ecmdLooperNext(target1, looperdata1)&& (!coeRc || coeMode)) {

    if (argc > 5) {
      rc = ecmdI2cReadOffset(target1, engineId, port, slaveAddr, busspeed , offset, fieldSize, numBytes, data);
    } else {
      rc = ecmdI2cRead(target1, engineId, port, slaveAddr, busspeed, numBytes, data);
    }
    if (rc) { 
      if (argc > 5) {
        printed = "geti2c - Error occurred performing ecmdI2cReadOffset on ";
      } else {
        printed = "geti2c - Error occurred performing ecmdI2cRead on ";
      }
      printed += ecmdWriteTarget(target1) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }
    else {
      validPosFound = true;     
    }

    printed = ecmdWriteTarget(target1) + "\n";
    if (filename != NULL) {
      if (targetCount > 1) { //If multiple targets postfix the target info to the file
        sprintf(targetStr, "k%dn%ds%dp%d", target1.cage, target1.node, target1.slot, target1.pos); 
        newFilename = (std::string)filename+"."+(std::string)targetStr;
      } else newFilename = (std::string)filename; 

      rc = data.writeFile(newFilename.c_str(), ECMD_SAVE_FORMAT_BINARY_DATA);

      if (rc) {
        printed += "geti2c - Problems occurred writing data into file " + newFilename + "\n";
        ecmdOutputError(printed.c_str()); 
        coeRc = rc;
        continue;
      }
      ecmdOutput( printed.c_str() );

    } 
    else {
      std::string dataStr = ecmdWriteDataFormatted(data, outputformat);
      printed += dataStr;
      ecmdOutput( printed.c_str() );
    } 
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  if (!validPosFound) {
    ecmdOutputError("geti2c - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;  
}

uint32_t ecmdPutI2cUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;
  uint32_t coeRc = ECMD_SUCCESS;  
  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string inputformat = "xl";       ///< format of input data
  ecmdChipTarget target;                ///< Current target operating on
  ecmdDataBuffer data;                  ///< buffer for the Data to write into the module vpd keyword
  bool validPosFound = false;           ///< Did the looper find anything to execute on
  bool inputformatflag = false;
  ecmdI2cBusSpeed_t busspeed = ECMD_I2C_BUSSPEED_400KHZ; ///< bus speed to run i2c in khz
  std::string printed;
  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  
  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    inputformat = formatPtr;
    inputformatflag = true;
  }
  
  
  /* get the bus speed, if it's there */
  char * busspeedstr = ecmdParseOptionWithArgs(&argc, &argv, "-busspeed");
  if (busspeedstr != NULL) {
    if (strcmp(busspeedstr, "50")==0) {
      busspeed = ECMD_I2C_BUSSPEED_50KHZ;
    } else if (strcmp(busspeedstr, "100")==0) {
      busspeed = ECMD_I2C_BUSSPEED_100KHZ;
    } else if (strcmp(busspeedstr, "400")!=0) {
      printed = "puti2c - Invalid value for busspeed. Possible values are - 400, 100, 50.\n";
      ecmdOutputError(printed.c_str());
      return ECMD_INVALID_ARGS;
    }
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
  if (argc < 5) {  
    ecmdOutputError("puti2c - Too few arguments specified; you need at least a chip, engineId, port, slaveAddr, data/filename.\n");
    ecmdOutputError("puti2c - Type 'puti2c -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if ((argc > 5) && (argc < 7)) {
    ecmdOutputError("puti2c - Too few arguments specified; you need at least a chip, engineId, port, slaveAddr, data/filename, offset and fieldsize.\n");
    ecmdOutputError("puti2c - Type 'puti2c -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if ( argc > 7) {
    ecmdOutputError("puti2c - Too many arguments specified; you only need chip, engineId, port, slaveAddr, data/filename, offset and fieldsize.\n");
    ecmdOutputError("puti2c - Type 'puti2c -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  
  /* Get the filename if -f is specified */
  char * filename = ecmdParseOptionWithArgs(&argc, &argv, "-f");

  if((filename != NULL) && (inputformatflag) ) {
    printed = "puti2c - Options '-f' cannot be specified with '-i' option.\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
  } 
  
  //Setup the target that will be used to query the system config 
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (chipUnitType != "") {
    ecmdOutputError("puti2c - chipUnit specified on the command line, this function doesn't support chipUnits.\n");
    return ECMD_INVALID_ARGS;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  if (!ecmdIsAllDecimal(argv[1])) {
    ecmdOutputError("puti2c - Non-decimal numbers detected in engineId field\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint32_t engineId = (uint32_t)atoi(argv[1]);
  
  if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("puti2c - Non-decimal numbers detected in port field\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint32_t port = (uint32_t)atoi(argv[2]);
  
  if (!ecmdIsAllHex(argv[3])) {
    ecmdOutputError("puti2c - Non-hex characters detected in slaveAddr field\n");
    return ECMD_INVALID_ARGS;
  } else if (strlen(argv[3]) > 8) {
    ecmdOutputError("puti2c - slave Address must be <= 32 bits in length\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint32_t slaveAddr = ecmdGenB32FromHexRight(&slaveAddr, argv[3]);
  
  uint32_t offset = 0, fieldSize = 0; 
  if (((filename != NULL) && (argc > 4)) || ((filename == NULL) && (argc > 5))) {
    std::string offsetstr, fieldstr;

    if (filename != NULL) {
      offsetstr = argv[4];
      fieldstr = argv[5];
    } else {
      offsetstr = argv[5];
      fieldstr = argv[6];
    }
    if (!ecmdIsAllDecimal(offsetstr.c_str())) {
      ecmdOutputError("puti2c - Non-decimal numbers detected in the offset field\n");
      return ECMD_INVALID_ARGS;
    }
    
    offset = (uint32_t)atoi(offsetstr.c_str());
    
    if (!ecmdIsAllDecimal(fieldstr.c_str())) {
      ecmdOutputError("puti2c - Non-decimal numbers detected in fieldSize field\n");
      return ECMD_INVALID_ARGS;
    }
    
    fieldSize = (uint32_t)atoi(fieldstr.c_str());
  }
  
  
  if (filename != NULL) {
    rc = data.readFile(filename, ECMD_SAVE_FORMAT_BINARY_DATA);
    if (rc) {
      printed = "puti2c - Problems occurred in reading data from file " + (std::string)filename + "\n";
      ecmdOutputError(printed.c_str()); 
      return rc;
    }
  } else  {
    //container to store data
    rc = ecmdReadDataFormatted(data, argv[4], inputformat);
    if (rc) {
      ecmdOutputError("puti2c - Problems occurred parsing input data, must be an invalid format\n");
      return rc;
    }
  }
  
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  
  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    if (((filename != NULL) && (argc > 4)) || ((filename == NULL) && (argc > 5))) {
      rc = ecmdI2cWriteOffset(target, engineId, port, slaveAddr, busspeed , offset, fieldSize, data);
    } else {
      rc = ecmdI2cWrite(target, engineId, port, slaveAddr, busspeed, data);
    }
    if (rc) {
      if (((filename != NULL) && (argc > 4)) || ((filename == NULL) && (argc > 5))) {
        printed = "puti2c - Error occurred performing ecmdI2cWriteOffset on ";
      } else {
        printed = "puti2c - Error occurred performing ecmdI2cWrite on ";
      }

      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;                                           
      continue;
    }
    else {
      validPosFound = true;     
    }

    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      printed = ecmdWriteTarget(target) + "\n";
      ecmdOutput(printed.c_str());
    }
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  if (!validPosFound) {
    ecmdOutputError("puti2c - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;  
}

uint32_t ecmdI2cResetUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;
  uint32_t coeRc = ECMD_SUCCESS;
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdChipTarget target;                ///< Current target operating on
  bool validPosFound = false;           ///< Did the looper find anything to execute on
  std::string printed;
  
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
  if (argc < 3) {  
    ecmdOutputError("i2creset - Too few arguments specified; you need at least a chip, engineId, port.\n");
    ecmdOutputError("i2creset - Type 'i2creset -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if ( argc > 3) {
    ecmdOutputError("i2creset - Too many arguments specified; you only need chip, engineId, port.\n");
    ecmdOutputError("i2creset - Type 'i2creset -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  
  
  //Setup the target that will be used to query the system config 
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (chipUnitType != "") {
    ecmdOutputError("i2creset - chipUnit specified on the command line, this function doesn't support chipUnits.\n");
    return ECMD_INVALID_ARGS;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  if (!ecmdIsAllDecimal(argv[1])) {
    ecmdOutputError("i2creset - Non-decimal numbers detected in engineId field\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint32_t engineId = (uint32_t)atoi(argv[1]);
  
  if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("i2creset - Non-decimal numbers detected in port field\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint32_t port = (uint32_t)atoi(argv[2]);
  
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  
  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    rc = ecmdI2cReset(target, engineId, port);
    if (rc) {
      printed = "i2creset - Error occurred performing ecmdI2cReset on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;                                      
      continue;
    } else {
      validPosFound = true;     
    }

    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      printed = ecmdWriteTarget(target) + "\n";
      ecmdOutput(printed.c_str());
    }
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  if (!validPosFound) {
    ecmdOutputError("i2creset - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;  
}


//I2cMultiple stuff
/******Used in ecmdI2cMultipleUser for parsting the geti2c calls*******/

uint32_t getI2cMultipleParser(int & argc,char * argv[],ecmdI2CCmdEntry & o_cmd){

  uint32_t rc = ECMD_SUCCESS;

  std::string printed;
  o_cmd.busSpeed = ECMD_I2C_BUSSPEED_400KHZ; //bus speed to run i2c in khz.This is default if not specified on cmdLine
  // get the bus speed, if it's there 
  char * busspeedstr = ecmdParseOptionWithArgs(&argc, &argv, "-busspeed");
  if (busspeedstr != NULL) {
    if (strcmp(busspeedstr, "50")==0) {
      o_cmd.busSpeed = ECMD_I2C_BUSSPEED_50KHZ;
    } else if (strcmp(busspeedstr, "100")==0) {
      o_cmd.busSpeed = ECMD_I2C_BUSSPEED_100KHZ;
    } else if (strcmp(busspeedstr, "400")!=0) {
      printed = "getI2cMultipleParser - Invalid value for busspeed. Possible values are - 400, 100, 50.\n";
      ecmdOutputError(printed.c_str());
      return ECMD_INVALID_ARGS;
    }
  }

  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 5) {
    ecmdOutputError("getI2cMultipleParser - Too few arguments specified; you need at least an engineId, port, slaveAddr, numbytes.\n");
    ecmdOutputError("getI2cMultipleParser - Type 'geti2c -h' for usage, but ignore the 'chip' part "
			"since this is the multiple command that all use the same 'chip' args\n");
    return ECMD_INVALID_ARGS;
  } else if ((argc > 5) && (argc < 7)) {
    ecmdOutputError("getI2cMultipleParser - Too few arguments specified; you need at least an engineId, port, slaveAddr, numbytes, offset and fieldsize.\n");
    ecmdOutputError("getI2cMultipleParser - Type 'geti2c -h' for usage, but ignore the 'chip' part "
			"since this is the multiple command that all use the same 'chip' args\n");
    return ECMD_INVALID_ARGS;
  } else if ( argc > 7) {
    ecmdOutputError("getI2cMultipleParser - Too many arguments specified; you only need a engineId, port, slaveAddr, numbytes, offset and fieldsize.\n");
    ecmdOutputError("getI2cMultipleParser - Type 'geti2c -h' for usage, but ignore the 'chip' part "
			"since this is the multiple command that all use the same 'chip' args\n");
    return ECMD_INVALID_ARGS;
  }

  if (!ecmdIsAllDecimal(argv[1])) {
    ecmdOutputError("getI2cMultipleParser - Non-decimal numbers detected in engineId field\n");
    return ECMD_INVALID_ARGS;
  }

  o_cmd.engineId = (uint32_t)atoi(argv[1]);

  if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("getI2cMultipleParser - Non-decimal numbers detected in port field\n");
    return ECMD_INVALID_ARGS;
  }

  o_cmd.port = (uint32_t)atoi(argv[2]);

  if (!ecmdIsAllHex(argv[3])) {
    ecmdOutputError("getI2cMultipleParser - Non-hex characters detected in slaveAddr field\n");
    return ECMD_INVALID_ARGS;
  } else if (strlen(argv[3]) > 8) {
    ecmdOutputError("getI2cMultipleParser - slave Address must be <= 32 bits in length\n");
    return ECMD_INVALID_ARGS;
  }

  o_cmd.slaveaddress = ecmdGenB32FromHexRight(&o_cmd.slaveaddress, argv[3]);

  if (!ecmdIsAllDecimal(argv[4])) {
    ecmdOutputError("getI2cMultipleParser - Non-decimal numbers detected in numBytes field\n");
    return ECMD_INVALID_ARGS;
  }

  o_cmd.readByteLength = (uint32_t)atoi(argv[4]);

  if (argc > 5) {
    if (!ecmdIsAllDecimal(argv[5])) {
      ecmdOutputError("getI2cMultipleParser - Non-decimal numbers detected in the offset field\n");
      return ECMD_INVALID_ARGS;
    }

    o_cmd.byteOffset = (uint32_t)atoi(argv[5]);

    if (!ecmdIsAllDecimal(argv[6])) {
      ecmdOutputError("getI2cMultipleParser - Non-decimal numbers detected in fieldSize field\n");
      return ECMD_INVALID_ARGS;
    }
    o_cmd.offsetFieldSize = (uint32_t)atoi(argv[6]);
    o_cmd.ecmdI2CCmd = ECMD_I2C_READOFFSET;
  }
  else{
    o_cmd.ecmdI2CCmd = ECMD_I2C_READ;
  }
//Setting the data Buffer
  ecmdDataBuffer l_readBuffer;
  l_readBuffer.setByteLength(o_cmd.readByteLength);
  o_cmd.data= l_readBuffer;


  return rc;
}


/******Used in ecmdI2cMultipleUser for parsting the puti2c calls*******/

uint32_t putI2cMultipleParser(int & argc,char * argv[],ecmdI2CCmdEntry & o_cmd){

  uint32_t rc = ECMD_SUCCESS;
  std::string inputformat = "xl";       // format of input data 
  std::string printed; 
  bool inputformatflag = false;
  // get format flag, if it's there 
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    inputformat = formatPtr;
    inputformatflag = true;
  }
  
  o_cmd.busSpeed = ECMD_I2C_BUSSPEED_400KHZ; // bus speed to run i2c in khz.This is default if not specified on cmdLine 
  // get the bus speed, if it's there 
  char * busspeedstr = ecmdParseOptionWithArgs(&argc, &argv, "-busspeed");
  if (busspeedstr != NULL) {
    if (strcmp(busspeedstr, "50")==0) {
      o_cmd.busSpeed = ECMD_I2C_BUSSPEED_50KHZ;
    } else if (strcmp(busspeedstr, "100")==0) {
      o_cmd.busSpeed = ECMD_I2C_BUSSPEED_100KHZ;
    } else if (strcmp(busspeedstr, "400")!=0) {
      printed = "putI2cMultipleParser - Invalid value for busspeed. Possible values are - 400, 100, 50.\n";
      ecmdOutputError(printed.c_str());
      return ECMD_INVALID_ARGS;
    }
  }
  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 5) {  
    ecmdOutputError("putI2cMultipleParser - Too few arguments specified; you need at least a engineId, port, slaveAddr, data\n");
    ecmdOutputError("putI2cMultipleParser - Type 'puti2c -h' for usage, but ignore the 'chip' part "
			"since this is the multiple command that all use the same 'chip' args\n");
    return ECMD_INVALID_ARGS;
  } else if ((argc > 5) && (argc < 7)) {
    ecmdOutputError("putI2cMultipleParser - Too few arguments specified; you need at least a  engineId, port, slaveAddr, data, offset and fieldsize.\n");
    ecmdOutputError("putI2cMultipleParser - Type 'puti2c -h' for usage, but ignore the 'chip' part "
			"since this is the multiple command that all use the same 'chip' args\n");
    return ECMD_INVALID_ARGS;
  } else if ( argc > 7) {
    ecmdOutputError("putI2cMultipleParser - Too many arguments specified; you only need a engineId, port, slaveAddr, data, offset and fieldsize.\n");
    ecmdOutputError("putI2cMultipleParser - Type 'puti2c -h' for usage, but ignore the 'chip' part "
			"since this is the multiple command that all use the same 'chip' args\n");
    return ECMD_INVALID_ARGS;
  }

  if (!ecmdIsAllDecimal(argv[1])) {
    ecmdOutputError("putI2cMultipleParser - Non-decimal numbers detected in engineId field\n");
    return ECMD_INVALID_ARGS;
  }
  
  o_cmd.engineId = (uint32_t)atoi(argv[1]);
  
  if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("putI2cMultipleParser - Non-decimal numbers detected in port field\n");
    return ECMD_INVALID_ARGS;
  }
  
  o_cmd.port = (uint32_t)atoi(argv[2]);
  
  if (!ecmdIsAllHex(argv[3])) {
    ecmdOutputError("putI2cMultipleParser - Non-hex characters detected in slaveAddr field\n");
    return ECMD_INVALID_ARGS;
  } else if (strlen(argv[3]) > 8) {
    ecmdOutputError("putI2cMultipleParser - slave Address must be <= 32 bits in length\n");
    return ECMD_INVALID_ARGS;
  }
  
  o_cmd.slaveaddress = ecmdGenB32FromHexRight(&o_cmd.slaveaddress, argv[3]);
  
    if (argc > 5) {
      std::string offsetstr, fieldstr;

      offsetstr = argv[5];
      fieldstr = argv[6];
      if (!ecmdIsAllDecimal(offsetstr.c_str())) {
        ecmdOutputError("putI2cMultipleParser - Non-decimal numbers detected in the offset field\n");
        return ECMD_INVALID_ARGS;
      }
    
       o_cmd.byteOffset = (uint32_t)atoi(offsetstr.c_str());
    
       if (!ecmdIsAllDecimal(fieldstr.c_str())) {
         ecmdOutputError("putI2cMultipleParser - Non-decimal numbers detected in fieldSize field\n");
         return ECMD_INVALID_ARGS;
       }
    
      o_cmd.offsetFieldSize = (uint32_t)atoi(fieldstr.c_str());
      o_cmd.ecmdI2CCmd = ECMD_I2C_WRITEOFFSET;
    }
    else{
      o_cmd.ecmdI2CCmd = ECMD_I2C_WRITE;
    }

    //container to store data
    rc = ecmdReadDataFormatted(o_cmd.data, argv[4], inputformat);
    if (rc) {
      ecmdOutputError("putI2cMultipleParser - Problems occurred parsing input data, must be an invalid format\n");
      return rc;
    }
    return rc; 
 
}


/******Used in ecmdI2cMultipleUser for parsting the i2creset calls*******/

uint32_t i2cResetMultipleParser(int & argc,char * argv[],ecmdI2CCmdEntry & o_cmd){

  uint32_t rc = ECMD_SUCCESS;
  std::string printed;
  
  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 3) {  
    ecmdOutputError("i2cResetMultipleParser - Too few arguments specified; you need at least a chip, engineId, port.\n");
    ecmdOutputError("i2cResetMultipleParser - Type 'i2creset -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if ( argc > 3) {
    ecmdOutputError("i2cResetMultipleParser - Too many arguments specified; you only need chip, engineId, port.\n");
    ecmdOutputError("i2cResetMultipleParser - Type 'i2creset -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  
  if (!ecmdIsAllDecimal(argv[1])) {
    ecmdOutputError("i2cResetMultipleParser - Non-decimal numbers detected in engineId field\n");
    return ECMD_INVALID_ARGS;
  }
  
  o_cmd.engineId = (uint32_t)atoi(argv[1]);
  
  if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("i2cResetMultipleParser - Non-decimal numbers detected in port field\n");
    return ECMD_INVALID_ARGS;
  }
  
  o_cmd.port = (uint32_t)atoi(argv[2]);
 
  o_cmd.ecmdI2CCmd = ECMD_I2C_RESET; 

  return rc;

}


/*This function is to process the multiple i2c cmds present in the cmdLine or the input file*/

uint32_t ecmdI2cMultipleUser(int argc, char * argv[]) {

  uint32_t coeRc = ECMD_SUCCESS;
  uint32_t rc = ECMD_SUCCESS;
  ecmdLooperData looperdata;            // Store internal Looper data
  std::string outputformat = "xl";      // Output Format to display
  ecmdChipTarget target;                // Current target operating on
  ecmdDataBuffer data;                  // I2C Data from the specified engine/port/device
  bool validPosFound = false;           // Did the looper find anything to execute on
  bool outputformatflag = false;
  ecmdChipTarget target1;               // Current target operating on-for second looper
  ecmdLooperData looperdata1;           // looper to do the real work
  int targetCount=0;                    // counts the number of targets user specified
  std::list<ecmdI2CCmdEntry> o_cmdList; // multiple command list to be processed
  std::list<ecmdI2CCmdEntry>::iterator itr; //iterator to iterate on the o_cmdList 
  ecmdI2CCmdEntry o_cmd;		// This object is passed in the parser functions for get/puti2c and i2creset
  char temp_buf[200];			// this will be used in sprintf
  /************************************************************************/
  /* Parse Common FLAGS here!                                             */
  /************************************************************************/
 
  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr != NULL) {
    outputformat = formatPtr;
    outputformatflag = true;
  }

  std::string printed;  
  // Get the inputfilename if '-fi' is specified 
  char * inputfilename = ecmdParseOptionWithArgs(&argc, &argv, "-fi");


  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode



  /************************************************************************/
  /* Creating the cmds for the Parser functions for get/puti2c, i2creset  */
  /************************************************************************/


  uint32_t l_rc = ECMD_SUCCESS; //variable in case the error is returned from the parser function
  // This part is to process the multiple commands from a input File
  if(inputfilename!=NULL){
     //Error handling when -fi is specified in the cmdLine
     if(argc!=1){
        ecmdOutputError("i2cmultiple - Too many args specified,You only need a single argument i.e 'chip' in case -fi is used\n");
	sprintf(temp_buf,"Extra arg present are: %s and the following args,\n",argv[1]);
	ecmdOutputError(temp_buf);
        ecmdOutputError("i2cmultiple - Type 'i2cmultiple -h' for usage.\n");
        return ECMD_INVALID_ARGS;
     }
     
     std::ifstream ifile;	
     ifile.open(inputfilename);
     if(!ifile){
     	ecmdOutputError("i2cmultiple - Error In Opening the Input File \n");
	return ECMD_INVALID_ARGS;
	
     }
     std::string argv_temp;	//This is used to save the command string obtained by the getline() from the input File.
     while(std::getline(ifile,argv_temp,'\n')){//getting the cmd String line by line to convert it later into a char ** array
	std::vector<std::string> splitArgs;
	ecmdParseTokens(argv_temp," ", splitArgs);//converting the argv_temp string a into vector
	int len =splitArgs.size();         //obtaining the length of vector

	char ** argv_temp_helper = new char*[len+1];	//array to obtain the char * array from the Vector,1 is added as per c++

        int argc_temp;	//To store the length of the cmd char * array.  
	int i =0;
    argv_temp_helper[0]= NULL; //beam
	for(i = 0;i<len;i++){
		argv_temp_helper[i]= (char *)(splitArgs[i].c_str());   //obtaining the char * array from the vector
	}
	argc_temp = len;	//saving the length of the cmd array
        if(argv_temp.find("geti2c",0)!= std::string::npos)	//finding the geti2c in the argv_temp str obtained by getline()
	{  
          rc = getI2cMultipleParser(argc_temp,argv_temp_helper,o_cmd); //if geti2c is found,pass the argv_temp_helper array in the parser
          if(rc){
		sprintf(temp_buf,"i2cmultiple - Error in getI2cMultipleParser,rc: 0x%x\n",rc);
                ecmdOutputError(temp_buf);
	  	l_rc = rc; //need to ask how to tackle it, see line no. 2047
          }
	}
	else if(argv_temp.find("puti2c",0)!= std::string::npos)
	{
          rc = putI2cMultipleParser(argc_temp,argv_temp_helper,o_cmd);
          if(rc){
                sprintf(temp_buf,"i2cmultiple - Error in putI2cMultipleParser,rc: 0x%x\n",rc);
                ecmdOutputError(temp_buf);
	  	l_rc = rc;
          }
	}
	else if(argv_temp.find("i2creset",0)!= std::string::npos)
	{
          rc = i2cResetMultipleParser(argc_temp,argv_temp_helper,o_cmd);
          if(rc){
                sprintf(temp_buf,"i2cmultiple - Error in i2cResetMultipleParser,rc: 0x%x\n",rc);
                ecmdOutputError(temp_buf);
	  	l_rc = rc;
          }
	}
        else{
               if (argv_temp_helper[0]!=NULL) {           //beam
                 sprintf(temp_buf,"i2cmultiple - Invalid cmd '%s' present in the input File\n",argv_temp_helper[0]);
               } else {
                 sprintf(temp_buf,"i2cmultiple - Invalid cmd NULL present in the input File\n");
               }
	   ecmdOutputError(temp_buf);
	   l_rc = ECMD_INVALID_ARGS;
	}
        argv_temp.clear();
	delete [] argv_temp_helper;     //beam 
	argv_temp_helper = NULL;
	if(l_rc){
	   l_rc = ECMD_SUCCESS;
	   continue;		//If error then continue looking for the next cmd in the input File.
	}
	else{			//else push back in the cmd list
           o_cmdList.push_back(o_cmd);
	}
     } 	//ending the while loop
  }	//input file operation is completed
  else { // This part is to process the multiple commands from cmd Line
    //Error handling in case of cmdLine
    std::string colon = ":";	//This is used as a separator for the different cmds in the cmd Line.
    
    if(argc < 3){
        ecmdOutputError("i2cmultiple - Too few arguments specified,You need a chip,colon i.e. ':',and a command i.e. geti2c,puti2c,i2creset"
			"with their valid params\n");
	ecmdOutputError("i2cmultiple - Type 'i2cmultiple -h' for usage.\n");
	return ECMD_INVALID_ARGS;
    }
    if(argv[argc-1]!=colon){
	ecmdOutputError("i2cmultiple - You need to have a colon i.e ':' in the end of the cmd line. \n");
	return ECMD_INVALID_ARGS;
    }  
    
    std::string cmdList[] = {"geti2c","puti2c","i2creset"};  //This is the list of commands to look in the cmd line
    bool ctrvar = true;		//it controls the parsing of cmds by looking for the first cmd
    int ctrvar_helper = 0;	//This variable is used in calculting the length of cmds present b/w the colons in the cmd line
    int argc_helper = argc;
    char ** argv_helper = argv;
     
    while((argc_helper>0)){	
      if(ctrvar){	   //checking if the cmd is the first in the cmdLine
         ctrvar_helper = 2;//In case of first cmd in the cmd line, we also have the 'chip' and the ':' as the params,
	              //so we set the ctrvar_helper as 2,this will later be used in calculating the actual cmd length for geti2c etc.
         ctrvar = false;   //Now,setting the ctrvar for the other commands in the cmdLine
       }		  	 
       else{
         ctrvar_helper = 0;    //for the other cmds i will be set as 0,because we do not have the 'chip' and ':' now
       }
       int j = ctrvar_helper;          //initializing it with ctrvar_helper to bypass the chip and ':' in case of first cmd.
       
       while((argv_helper[j]!=colon)){
	 j++;
       }

       int cmdsize = j+1;//argc_helper & argv_helper pointer will be decremented & incremented with cmdsize after each cmd handling
       int argc_temp = cmdsize -ctrvar_helper-1;//This is the actual length of a particular cmdLine for geti2c, puti2c ot i2creset
                                                //which will be passed in the parser function.

       char ** argv_temp = new char*[argc_temp+1]; //This char * array will be passed in the parser function
       argv_temp[0]=NULL;       //beam
       for(int k = 0;(k<argc_temp)&&(ctrvar_helper<cmdsize);k++,ctrvar_helper++){//creating the cmd to be passed in the parser funcs
            argv_temp[k]=argv_helper[ctrvar_helper];
       }
       if (argv_temp[0]!=NULL) {     //beam
       if(argv_temp[0]==cmdList[0]){	//Now checking if the cmd passed has the geti2c in it as the first argument
	    rc = getI2cMultipleParser(argc_temp,argv_temp,o_cmd);
	    if(rc){
	      sprintf(temp_buf,"i2cmultiple - Error in getI2cMultipleParser,rc: 0x%x\n",rc);
                  ecmdOutputError(temp_buf);
	      l_rc = rc;
	    }
       }
       else if(argv_temp[0]==cmdList[1]){	//checking for the puti2c
          rc = putI2cMultipleParser(argc_temp,argv_temp,o_cmd);
          if(rc){
                sprintf(temp_buf,"i2cmultiple - Error in putI2cMultipleParser,rc: 0x%x\n",rc);
                ecmdOutputError(temp_buf);
		l_rc = rc;
          }
       }
       else if(argv_temp[0]==cmdList[2]){	//checking for the i2creset
          rc = i2cResetMultipleParser(argc_temp,argv_temp,o_cmd);
          if(rc){
                sprintf(temp_buf,"i2cmultiple - Error in i2cResetMultipleParser,rc: 0x%x\n",rc);
                ecmdOutputError(temp_buf);
		l_rc = rc;
          }
       } 
       else {
           sprintf(temp_buf,"i2cmultiple - Invalid cmd '%s' present in the cmdLine\n",argv_temp[0]);
           ecmdOutputError(temp_buf);
           l_rc = ECMD_INVALID_ARGS;
       }
       }     //beam
       argc_helper = argc_helper - cmdsize;	//decrementing argc_helper pointer by cmdsize
       argv_helper = argv_helper + cmdsize;	//incrementing argv_helper pointer by cmdsize
       
       if(l_rc){
	l_rc = ECMD_SUCCESS;
    delete [] argv_temp;       //beam
    argv_temp = NULL;          //beam
	continue;	//if error returned from the parser function then continue looking for the next cmd in the cmd Line
       }
       else {		//else puch_back in the list
	o_cmdList.push_back(o_cmd);
       }    
       delete [] argv_temp;
       argv_temp = NULL; 
    }	//ending the while loop
  }	//multiple cmd Line operation is completed 

  if(o_cmdList.empty()){	//If the cmd List obtained is empty then return Error.
     ecmdOutputError("i2cmultiple - The ecmdI2CCmdEntry cmdList is empty,Therefore Failing\n");
     return ECMD_FAILURE;
  }

  //Setup the target that will be used to query the system config
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (chipUnitType != "") {
    ecmdOutputError("i2cmultiple - chipUnit specified on the command line, this function doesn't support chipUnits.\n");
    return ECMD_INVALID_ARGS;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  target1 = target; //Created for the second looper needed for -fi case with multiple positions
 
  //Run the loop to Check the number of targets
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  
  while ( ecmdConfigLooperNext(target, looperdata) ) {
      targetCount++;
  }
  //Looper to do the actual work
  rc = ecmdConfigLooperInit(target1, ECMD_SELECTED_TARGETS_LOOP, looperdata1);
  if (rc) return rc;
              
  while ( ecmdConfigLooperNext(target1, looperdata1)&& (!coeRc || coeMode)) 
  {

	  rc = ecmdI2CMultipleCmds(target1,o_cmdList);
	  if(rc){
              printed += "i2cmultiple - Error occurred performing on ecmdI2CMultipleCmds";
              printed += ecmdWriteTarget(target1) + "\n";
              ecmdOutputError( printed.c_str() );
              coeRc = rc;
              continue;

          }
          else
          {
	    validPosFound = true;
            printed  +=ecmdWriteTarget(target1) + "\n";
            for(itr = o_cmdList.begin();itr!=o_cmdList.end();itr++)
	    {
              if(itr->ecmdI2CCmd == ECMD_I2C_WRITEOFFSET){
                printed +="Performing ECMD_I2C_WRITEOFFSET..\n";
                sprintf(temp_buf,"Data Written: %s\n\n",itr->data.genHexLeftStr().c_str());
                printed += temp_buf;
              }
              else if(itr->ecmdI2CCmd == ECMD_I2C_WRITE){
                printed +="Performing ECMD_I2C_WRITE..\n";
                sprintf(temp_buf,"Data Written: %s\n\n",itr->data.genHexLeftStr().c_str());
                printed += temp_buf;
              }
              else if(itr->ecmdI2CCmd == ECMD_I2C_READOFFSET){
                printed +="Performing ECMD_I2C_READOFFSET..\n";
		printed += "Data Read:";
	        std::string dataStr = ecmdWriteDataFormatted(itr->data, outputformat);
	        printed += dataStr;
		printed += "\n";
              }
              else if(itr->ecmdI2CCmd == ECMD_I2C_READ){
                printed +="Performing ECMD_I2C_READ..\n";
		printed += "Data Read:";
                std::string dataStr = ecmdWriteDataFormatted(itr->data, outputformat);
                printed += dataStr;
		printed += "\n";

              }
              else if(itr->ecmdI2CCmd == ECMD_I2C_RESET){
                printed +="ECMD_I2C_RESET ...Completed\n\n";
              }
              else {
                ecmdOutputError("i2cmultiple - Unknown Command Performed..Failing\n\n");
                rc = ECMD_FAILURE;
              }
            }
          }
      	  ecmdOutput(printed.c_str());
          printed.clear();
	
  } 
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  if (!validPosFound) {
    ecmdOutputError("i2cmultiple - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }
  if(l_rc) return l_rc;	//Returning if error is returned from the parser function
  
  return rc;  
}



#endif // ECMD_REMOVE_I2C_FUNCTIONS

#ifndef ECMD_REMOVE_GPIO_FUNCTIONS
uint32_t ecmdPutGpioLatchUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  uint32_t coeRc = ECMD_SUCCESS; 
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdChipTarget target;                ///< Current target operating on
  ecmdDataBuffer buffer;                ///< Container to store write data
  ecmdDataBuffer mask;                  ///< Container to store mask data
  bool validPosFound = false;           ///< Did the looper find anything to execute on
  std::string inputformat = "b";        ///< Input Format to display
  uint32_t value;                       ///< Value to write to pin/s
  std::string printed;
  
  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
 
  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    inputformat = formatPtr;
  }
  
  /* get mask value for multiple pins, if it's there */
  char * maskPtr = ecmdParseOptionWithArgs(&argc, &argv, "-mask");
  if (maskPtr != NULL) {
   rc = ecmdReadDataFormatted(mask, maskPtr, inputformat, 32);
   if (rc) {
    ecmdOutputError("putgpiolatch - Problems occurred parsing mask value, must be an invalid format\n");
    return rc;
   }
   if (mask.getBitLength() > 32) {
    ecmdOutputError("putgpiolatch - Mask length cannot exceed 32 bits\n");
    return ECMD_INVALID_ARGS;
   }
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
  if (((maskPtr != NULL) && (argc < 4)) || ((maskPtr == NULL) && (argc < 5)) ) {  
    ecmdOutputError("putgpiolatch - Too few arguments specified; you need at least a chip, engineId, pin/mask, mode, data.\n");
    ecmdOutputError("putgpiolatch - Type 'putgpiolatch -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if ( ((maskPtr != NULL) && (argc > 4)) || ((maskPtr == NULL) && (argc > 5))) {
    ecmdOutputError("putgpiolatch - Too many arguments specified; you only need chip, engineId, pin/mask, mode, data.\n");
    ecmdOutputError("putgpiolatch - Type 'putgpiolatch -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  
  
  //Setup the target that will be used to query the system config 
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (chipUnitType != "") {
    ecmdOutputError("putgpiolatch - chipUnit specified on the command line, this function doesn't support chipUnits.\n");
    return ECMD_INVALID_ARGS;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  if (!ecmdIsAllDecimal(argv[1])) {
    ecmdOutputError("putgpiolatch - Non-decimal numbers detected in engineId field\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint32_t engineId = (uint32_t)atoi(argv[1]);
  
  uint32_t pin = 0;
  std::string modeStr, dataStr;
  
  if (maskPtr == NULL) {
   if (!ecmdIsAllDecimal(argv[2])) {
     ecmdOutputError("putgpiolatch - Non-decimal numbers detected in pin field\n");
     return ECMD_INVALID_ARGS;
   }
 
   pin = (uint32_t)atoi(argv[2]);
   modeStr = argv[3];
   dataStr = argv[4];
  } else {
   modeStr = argv[2];
   dataStr = argv[3];
  }

  // Push toupper for the comparision below.  This will allow the user to use lower case - JTA
  transform(modeStr.begin(), modeStr.end(), modeStr.begin(), (int(*)(int)) toupper);

  ecmdDioMode_t mode = ECMD_DIO_INPUT;  // defaulting to remove compiler warnings
  if (modeStr == "IN") mode = ECMD_DIO_INPUT;
  else if (modeStr == "OD")  mode = ECMD_DIO_OPEN_DRAIN;
  else if (modeStr == "OS")  mode = ECMD_DIO_OPEN_SOURCE;
  else if (modeStr == "PP")  mode = ECMD_DIO_PUSH_PULL;
  else {
    ecmdOutputError("putgpiolatch - Invalid value for mode. Valid Values : IN(Input) OD(Open Drain) OS(Open Source) PP(Push Pull)\n");
    return ECMD_INVALID_ARGS;
  }

  if (maskPtr != NULL) {
   rc = ecmdReadDataFormatted(buffer, dataStr.c_str(), inputformat, 32);
   if (rc) {
     ecmdOutputError("putgpiolatch - Problems occurred parsing input data, must be an invalid format\n");
     return rc;
   }
   if (buffer.getBitLength() > 32) {
     ecmdOutputError("putgpiolatch - Input Data length cannot exceed 32 bits\n");
     return ECMD_INVALID_ARGS;
   } 
   value = buffer.getWord(0);
  } else {
   rc = ecmdReadDataFormatted(buffer, dataStr.c_str(), inputformat);
   if (rc) {
     ecmdOutputError("putgpiolatch - Problems occurred parsing input data, must be an invalid format\n");
     return rc;
   }
   if (buffer.getBitLength() > 1) {
     ecmdOutputError("putgpiolatch - Input Data length should be 1\n");
     return ECMD_INVALID_ARGS;
   }
   if (buffer.isBitSet(0)) {
    value = 1;
   } else value = 0;
  }
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  
  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    if (maskPtr != NULL) {
      rc = ecmdGpioWriteLatches(target, engineId, mode, mask.getWord(0), value );
    } else {
      rc = ecmdGpioWriteLatch(target, engineId, pin, mode, value );
    }

    if (rc) {
      if (maskPtr != NULL) {
        printed = "putgpiolatch - Error occurred performing ecmdGpioWriteLatches on ";
      } else {
        printed = "putgpiolatch - Error occurred performing ecmdGpioWriteLatch on ";
      }
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;                        
      continue;
    } else {
      validPosFound = true;     
    }

    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      printed = ecmdWriteTarget(target) + "\n";
      ecmdOutput(printed.c_str());
    }
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  if (!validPosFound) {
    ecmdOutputError("putgpiolatch - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;  
}

uint32_t ecmdGpioConfigUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  uint32_t coeRc = ECMD_SUCCESS;
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdChipTarget target;                ///< Current target operating on
  bool validPosFound = false;           ///< Did the looper find anything to execute on
  std::string printed;
  

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
  if (argc < 4) {  
    ecmdOutputError("gpioconfig - Too few arguments specified; you need at least a chip, engineId, pin, mode.\n");
    ecmdOutputError("gpioconfig - Type 'gpioconfig -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if ( argc > 4) {
    ecmdOutputError("gpioconfig - Too many arguments specified; you only need chip, engineId, pin, mode.\n");
    ecmdOutputError("gpioconfig - Type 'gpioconfig -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  
  
  //Setup the target that will be used to query the system config 
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (chipUnitType != "") {
    ecmdOutputError("gpioconfig - chipUnit specified on the command line, this function doesn't support chipUnits.\n");
    return ECMD_INVALID_ARGS;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  if (!ecmdIsAllDecimal(argv[1])) {
    ecmdOutputError("gpioconfig - Non-decimal numbers detected in engineId field\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint32_t engineId = (uint32_t)atoi(argv[1]);
  
  if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("gpioconfig - Non-decimal numbers detected in pin field\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint32_t pin = (uint32_t)atoi(argv[2]);

  // Push toupper for the comparision below.  This will allow the user to use lower case - JTA
  std::string modeStr = argv[3];
  transform(modeStr.begin(), modeStr.end(), modeStr.begin(), (int(*)(int)) toupper);
  
  ecmdDioMode_t mode = ECMD_DIO_INPUT;  // defaulting to remove compiler warnings
  if (modeStr == "IN") mode = ECMD_DIO_INPUT;
  else if (modeStr == "OD")  mode = ECMD_DIO_OPEN_DRAIN;
  else if (modeStr == "OS")  mode = ECMD_DIO_OPEN_SOURCE;
  else if (modeStr == "PP")  mode = ECMD_DIO_PUSH_PULL;
  else {
    ecmdOutputError("gpioconfig - Invalid value for mode. Valid Values : IN(Input) OD(Open Drain) OS(Open Source) PP(Push Pull)\n");
    return ECMD_INVALID_ARGS;
  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  
  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    rc = ecmdGpioConfigPin(target, engineId, pin, mode);
    if (rc) {
      printed = "gpioconfig - Error occurred performing ecmdGpioConfigPin on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;                           
      continue;
    }
    else {
      validPosFound = true;     
    }

    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      printed = ecmdWriteTarget(target) + "\n";
      ecmdOutput(printed.c_str());
    }
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  if (!validPosFound) {
    ecmdOutputError("gpioconfig - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;  
}


uint32_t ecmdGetGpioPinUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  uint32_t coeRc = ECMD_SUCCESS;
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdChipTarget target;                ///< Current target operating on
  ecmdDataBuffer data;                  ///< DataBuffer to store state into
  ecmdDataBuffer mask;                  ///< Container to store the mask value for multiple pins
  bool validPosFound = false;           ///< Did the looper find anything to execute on
  std::string outputformat = "b";       ///< Output Format to display
  std::string inputformat = "xl";       ///< Output Format to display
  uint32_t state;                       ///< State read on pin (0/1)
  std::string printed;
  
  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
 
  /* get format flag, if it's there */
  char * oformatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (oformatPtr != NULL) {
    outputformat = oformatPtr;
  }
  
  char * iformatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (iformatPtr != NULL) {
    inputformat = iformatPtr;
  }

  /* get mask value for multiple pins, if it's there */
  char * maskPtr = ecmdParseOptionWithArgs(&argc, &argv, "-mask");
  if (maskPtr != NULL) {
   rc = ecmdReadDataFormatted(mask, maskPtr, inputformat, 32);
   if (rc) {
    ecmdOutputError("getgpiopin - Problems occurred parsing mask value, must be an invalid format\n");
    return rc;
   }
   if (mask.getBitLength() > 32) {
    ecmdOutputError("getgpiopin - Mask length should be <= 32 bits.\n");
    return ECMD_INVALID_ARGS;
   }
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
  if (((maskPtr == NULL) && (argc < 3))  ||  ((maskPtr != NULL) && (argc < 2))){  
    ecmdOutputError("getgpiopin - Too few arguments specified; you need at least a chip, engineId, pin/mask.\n");
    ecmdOutputError("getgpiopin - Type 'getgpiopin -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if ( ((maskPtr == NULL) && (argc > 3))  ||  ((maskPtr != NULL) && (argc > 2))) {
    ecmdOutputError("getgpiopin - Too many arguments specified; you only need chip, engineId, pin/mask.\n");
    ecmdOutputError("getgpiopin - Type 'getgpiopin -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  
  
  //Setup the target that will be used to query the system config 
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (chipUnitType != "") {
    ecmdOutputError("getgpiopin - chipUnit specified on the command line, this function doesn't support chipUnits.\n");
    return ECMD_INVALID_ARGS;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  if (!ecmdIsAllDecimal(argv[1])) {
    ecmdOutputError("getgpiopin - Non-decimal numbers detected in engineId field\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint32_t engineId = (uint32_t)atoi(argv[1]);
  
  
  uint32_t pin=0;
  if (maskPtr == NULL) {
   if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("getgpiopin - Non-decimal numbers detected in pin field\n");
    return ECMD_INVALID_ARGS;
   }
   pin = (uint32_t)atoi(argv[2]);
  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  
  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    if (maskPtr != NULL) {
      rc = ecmdGpioReadPins(target, engineId, mask.getWord(0), state);
    } else {
      rc = ecmdGpioReadPin(target, engineId, pin,  state);
    }

    if (rc) {
      if (maskPtr != NULL) {
        printed = "getgpiopin - Error occurred performing ecmdGpioReadPins on ";
      } else {
        printed = "getgpiopin - Error occurred performing ecmdGpioReadPin on ";
      }
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;                       
      continue;
    }
    else {
      validPosFound = true;     
    }

    printed = ecmdWriteTarget(target) + "\n";

    data.setBitLength(32);
    data.insert(state, 0, 32 );   
    std::string dataStr = ecmdWriteDataFormatted(data, outputformat);
    printed += dataStr;
    ecmdOutput( printed.c_str() );
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  if (!validPosFound) {
    ecmdOutputError("getgpiopin - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }
  
  return rc;  
}

uint32_t ecmdGetGpioLatchUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;
  uint32_t coeRc = ECMD_SUCCESS;
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdChipTarget target;                ///< Current target operating on
  ecmdDataBuffer data;                  ///< DataBuffer to stores state into
  bool validPosFound = false;           ///< Did the looper find anything to execute on
  std::string outputformat = "b";      ///< Output Format to display
  uint32_t state;                       ///< State read on pin (0/1)
  std::string printed;
  
  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
 
  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr != NULL) {
    outputformat = formatPtr;
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
  if (argc < 4) {  
    ecmdOutputError("getgpiolatch - Too few arguments specified; you need at least a chip, engineId, pin, mode.\n");
    ecmdOutputError("getgpiolatch - Type 'getgpiolatch -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if ( argc > 4) {
    ecmdOutputError("getgpiolatch - Too many arguments specified; you only need chip, engineId, pin, mode.\n");
    ecmdOutputError("getgpiolatch - Type 'getgpiolatch -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  
  
  //Setup the target that will be used to query the system config 
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (chipUnitType != "") {
    ecmdOutputError("getgpiolatch - chipUnit specified on the command line, this function doesn't support chipUnits.\n");
    return ECMD_INVALID_ARGS;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  if (!ecmdIsAllDecimal(argv[1])) {
    ecmdOutputError("getgpiolatch - Non-decimal numbers detected in engineId field\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint32_t engineId = (uint32_t)atoi(argv[1]);
  
  if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("getgpiolatch - Non-decimal numbers detected in pin field\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint32_t pin = (uint32_t)atoi(argv[2]);
  
  // Push toupper for the comparision below.  This will allow the user to use lower case - JTA
  std::string modeStr = argv[3];
  transform(modeStr.begin(), modeStr.end(), modeStr.begin(), (int(*)(int)) toupper);

  ecmdDioMode_t mode = ECMD_DIO_INPUT;  // defaulting to remove compiler warnings
  if (modeStr == "IN") mode = ECMD_DIO_INPUT;
  else if (modeStr == "OD")  mode = ECMD_DIO_OPEN_DRAIN;
  else if (modeStr == "OS")  mode = ECMD_DIO_OPEN_SOURCE;
  else if (modeStr == "PP")  mode = ECMD_DIO_PUSH_PULL;
  else {
    ecmdOutputError("getgpiolatch - Invalid value for mode. Valid Values : IN(Input) OD(Open Drain) OS(Open Source) PP(Push Pull)\n");
    return ECMD_INVALID_ARGS;
  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  
  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    rc = ecmdGpioReadLatch(target, engineId, pin, mode, state);
    if (rc) {
      printed = "getgpiolatch - Error occurred performing ecmdGpioReadLatch on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;                                          
      continue;
    }
    else {
      validPosFound = true;     
    }

    printed = ecmdWriteTarget(target) + "\n";

    data.setBitLength(32);
    data.insert(state, 0, 32 );   
    std::string dataStr = ecmdWriteDataFormatted(data, outputformat);
    printed += dataStr;
    ecmdOutput( printed.c_str() );
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  if (!validPosFound) {
    ecmdOutputError("getgpiolatch - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdGetGpioRegUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  uint32_t coeRc = ECMD_SUCCESS;
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdChipTarget target;                ///< Current target operating on
  ecmdDataBuffer data;                  ///< DataBuffer to store state into
  ecmdDataBuffer mask;                  ///< Container to store the mask value for multiple pins
  bool validPosFound = false;           ///< Did the looper find anything to execute on
  std::string outputformat = "xl";      ///< Output Format to display
  uint32_t value;                       ///< value read from register
  std::string printed;
  
  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
 
  /* get format flag, if it's there */
  char * oformatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (oformatPtr != NULL) {
    outputformat = oformatPtr;
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
  if (argc < 3){  
    ecmdOutputError("getgpioreg - Too few arguments specified; you need at least a chip, engineId, configReg.\n");
    ecmdOutputError("getgpioreg - Type 'getgpioreg -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if (argc > 3) {
    ecmdOutputError("getgpioreg - Too many arguments specified; you only need chip, engineId, configReg.\n");
    ecmdOutputError("getgpioreg - Type 'getgpiopin -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  
  
  //Setup the target that will be used to query the system config 
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (chipUnitType != "") {
    ecmdOutputError("getgpioreg - chipUnit specified on the command line, this function doesn't support chipUnits.\n");
    return ECMD_INVALID_ARGS;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  if (!ecmdIsAllDecimal(argv[1])) {
    ecmdOutputError("getgpioreg - Non-decimal numbers detected in engineId field\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint32_t engineId = (uint32_t)atoi(argv[1]);
  
  /* Get the configReg */
  uint32_t configReg = 0;
  sscanf(argv[2], "%x", &configReg);


  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  
  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    rc = ecmdGpioReadConfigRegister(target, engineId, configReg, value);
    if (rc) {
      printed = "getgpioreg - Error occurred performing ecmdGpioReadPin on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;                                       
      continue;
    }
    else {
      validPosFound = true;     
    }

    printed = ecmdWriteTarget(target) + "\n";

    data.setBitLength(32);
    data.insert(value, 0, 32 );   
    std::string dataStr = ecmdWriteDataFormatted(data, outputformat);
    printed += dataStr;
    ecmdOutput( printed.c_str() );
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  if (!validPosFound) {
    ecmdOutputError("getgpioreg - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }
  
  return rc;  
}

uint32_t ecmdPutGpioRegUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  uint32_t coeRc = ECMD_SUCCESS;
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdChipTarget target;                ///< Current target operating on
  ecmdDataBuffer buffer;                ///< Container to store write data
  ecmdDataBuffer mask;                  ///< Container to store mask data
  bool validPosFound = false;           ///< Did the looper find anything to execute on
  std::string inputformat = "xl";       ///< Input Format to display
  uint32_t value=0;                     ///< Value to write to pin/s
  std::string printed;
  
  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
 
  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
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
  if (argc < 5) {  
    ecmdOutputError("putgpioreg - Too few arguments specified; you need at least a chip, engineId, configReg, value, mode.\n");
    ecmdOutputError("putgpioreg - Type 'putgpioreg -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if (argc > 5) {
    ecmdOutputError("putgpioreg - Too many arguments specified; you only need chip, engineId, configReg, value, mode.\n");
    ecmdOutputError("putgpioreg - Type 'putgpioreg -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  
  
  //Setup the target that will be used to query the system config 
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (chipUnitType != "") {
    ecmdOutputError("putgpioreg - chipUnit specified on the command line, this function doesn't support chipUnits.\n");
    return ECMD_INVALID_ARGS;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  if (!ecmdIsAllDecimal(argv[1])) {
    ecmdOutputError("putgpioreg - Non-decimal numbers detected in engineId field\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t engineId = (uint32_t)atoi(argv[1]);
  
  /* Get the configReg */
  uint32_t configReg = 0;
  sscanf(argv[2], "%x", &configReg);

  /* Get the mode and data strings */
  std::string modeStr, dataStr;
  modeStr = argv[3];
  dataStr = argv[4];

  // Push toupper for the comparision below.  This will allow the user to use lower case - JTA
  transform(modeStr.begin(), modeStr.end(), modeStr.begin(), (int(*)(int)) toupper);

  ecmdGpioWriteMode_t mode = ECMD_GPIO_CONFIG_WRITE;  // defaulting to remove compiler warnings
  if (modeStr == "WR") mode = ECMD_GPIO_CONFIG_WRITE;
  else if (modeStr == "SB")  mode = ECMD_GPIO_SET_BIT;
  else if (modeStr == "CB")  mode = ECMD_GPIO_CLEAR_BIT;
  else {
    ecmdOutputError("putgpioreg - Invalid value for mode. Valid Values : WR(Write Data as is) SB(Set Bits in Data) CB(Clear Bits in Data)\n");
    return ECMD_INVALID_ARGS;
  }

  /* Read in the data */
  rc = ecmdReadDataFormatted(buffer, dataStr.c_str(), inputformat);
  if (rc) {
    ecmdOutputError("putgpioreg - Problems occurred parsing input data, must be an invalid format\n");
    return rc;
  }    
  if (buffer.getBitLength() != 32) {
    ecmdOutputError("putgpioreg - Input Data length should be 32 bits\n");
    return ECMD_INVALID_ARGS;
  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  
  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    rc = ecmdGpioWriteConfigRegister(target, engineId, mode, configReg, value);
    if (rc) {
      printed = "putgpioreg - Error occurred performing putgpioreg on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;                                 
      continue;
    }
    else {
      validPosFound = true;     
    }

    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      printed = ecmdWriteTarget(target) + "\n";
      ecmdOutput(printed.c_str());
    }
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  if (!validPosFound) {
    ecmdOutputError("putgpioreg - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;  
}
#endif //ECMD_REMOVE_GPIO_FUNCTIONS

