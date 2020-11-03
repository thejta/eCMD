//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2019 IBM International Business Machines Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
//IBM_PROLOG_END_TAG


//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <fstream>
#include <stdio.h>
#include <string.h>
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
#ifndef ECMD_REMOVE_SPI_FUNCTIONS
uint32_t ecmdGetSpiUser(int argc, char * argv[]) {
  uint32_t coeRc = ECMD_SUCCESS;
  uint32_t rc = ECMD_SUCCESS;
  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string outputformat = "xl";      ///< Output Format to display
  ecmdChipTarget target;                ///< Current target operating on
  ecmdDataBuffer data;                  ///< SPI Data from the specified engine/select/device
  bool validPosFound = false;           ///< Did the looper find anything to execute on
  bool outputformatflag = false;
  ecmdChipTarget target1;               ///< Current target operating on-for second looper
  ecmdLooperData looperdata1;           ///< looper to do the real work
  int targetCount = 0;                  ///< counts the number of targets user specified
  uint32_t mode = 0;
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
    printed = "getspi - Options '-f' cannot be specified with '-o' option.\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
  } 
  
  /* check for ecc enable */
  if (ecmdParseOption(&argc, &argv, "-ecc")) {
    mode |= ECMD_SPI_ECC_ENABLE;
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
  if (argc != 5) {  
    ecmdOutputError("getspi - Incorrect number of arguments specified; you need chip, engineId, select, address, numbytes.\n");
    ecmdOutputError("getspi - Type 'getspi -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (chipUnitType != "") {
    ecmdOutputError("getspi - chipUnit specified on the command line, this function doesn't support chipUnits.\n");
    return ECMD_INVALID_ARGS;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  target1 = target; //Created for the second looper needed for -f case with multiple positions
  
  if (!ecmdIsAllDecimal(argv[1])) {
    ecmdOutputError("getspi - Non-decimal numbers detected in engineId field\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint32_t engineId = (uint32_t)atoi(argv[1]);
  
  if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("getspi - Non-decimal numbers detected in select field\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint32_t select = (uint32_t)atoi(argv[2]);
  
  if (!ecmdIsAllHex(argv[3])) {
    ecmdOutputError("getspi - Non-hex characters detected in address field\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint64_t address = 0;
#ifdef _LP64
  address = strtoul(argv[3], NULL, 16);
#else
  address = strtoull(argv[3], NULL, 16);
#endif
  
  if (!ecmdIsAllDecimal(argv[4])) {
    ecmdOutputError("getspi - Non-decimal numbers detected in numBytes field\n");
    return ECMD_INVALID_ARGS;
  } 
  
  uint32_t numBytes = (uint32_t)atoi(argv[4]);
  
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

    rc = spiRead(target1, engineId, select, address, numBytes, mode, data);
    if (rc) { 
      printed = "getspi - Error occurred performing spiRead on ";
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
      std::string newFilename;
      if (targetCount > 1) { //If multiple targets postfix the target info to the file
        char targetStr[50];
        snprintf(targetStr, 50, "k%dn%ds%dp%d", target1.cage, target1.node, target1.slot, target1.pos); 
        newFilename = (std::string)filename+"."+(std::string)targetStr;
      } else newFilename = (std::string)filename; 

      rc = data.writeFile(newFilename.c_str(), ECMD_SAVE_FORMAT_BINARY_DATA);

      if (rc) {
        printed += "getspi - Problems occurred writing data into file " + newFilename + "\n";
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
    ecmdOutputError("getspi - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;  
}

uint32_t ecmdPutSpiUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;
  uint32_t coeRc = ECMD_SUCCESS;  
  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string inputformat = "xl";       ///< format of input data
  ecmdChipTarget target;                ///< Current target operating on
  ecmdDataBuffer data;                  ///< buffer for the Data to write into the module vpd keyword
  bool validPosFound = false;           ///< Did the looper find anything to execute on
  bool inputformatflag = false;
  std::string printed;
  uint32_t mode = 0;
  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  
  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    inputformat = formatPtr;
    inputformatflag = true;
  }

  /* Get the filename if -f is specified */
  char * filename = ecmdParseOptionWithArgs(&argc, &argv, "-f");

  if((filename != NULL) && (inputformatflag) ) {
    printed = "putspi - Options '-f' cannot be specified with '-i' option.\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
  } 
  
  /* check for ecc enable */
  if (ecmdParseOption(&argc, &argv, "-ecc")) {
    mode |= ECMD_SPI_ECC_ENABLE;
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
  if (!((argc == 5) || ((argc == 4) && (filename != NULL)))) {  
    ecmdOutputError("putspi - Too few arguments specified; you need at least a chip, engineId, select, address, data/filename.\n");
    ecmdOutputError("putspi - Type 'putspi -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  
  //Setup the target that will be used to query the system config 
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (chipUnitType != "") {
    ecmdOutputError("putspi - chipUnit specified on the command line, this function doesn't support chipUnits.\n");
    return ECMD_INVALID_ARGS;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  if (!ecmdIsAllDecimal(argv[1])) {
    ecmdOutputError("putspi - Non-decimal numbers detected in engineId field\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint32_t engineId = (uint32_t)atoi(argv[1]);
  
  if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("putspi - Non-decimal numbers detected in select field\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint32_t select = (uint32_t)atoi(argv[2]);
  
  if (!ecmdIsAllHex(argv[3])) {
    ecmdOutputError("putspi - Non-hex characters detected in address field\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint64_t address = 0;
#ifdef _LP64
  address = strtoul(argv[3], NULL, 16);
#else
  address = strtoull(argv[3], NULL, 16);
#endif
  
  if (filename != NULL) {
    rc = data.readFile(filename, ECMD_SAVE_FORMAT_BINARY_DATA);
    if (rc) {
      printed = "putspi - Problems occurred in reading data from file " + (std::string)filename + "\n";
      ecmdOutputError(printed.c_str()); 
      return rc;
    }
  } else  {
    //container to store data
    rc = ecmdReadDataFormatted(data, argv[4], inputformat);
    if (rc) {
      ecmdOutputError("putspi - Problems occurred parsing input data, must be an invalid format\n");
      return rc;
    }
  }
  
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  
  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    rc = spiWrite(target, engineId, select, address, mode, data);
    if (rc) {
      printed = "putspi - Error occurred performing spiWrite on ";
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
    ecmdOutputError("putspi - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;  
}
#endif // ECMD_REMOVE_SPI_FUNCTIONS
