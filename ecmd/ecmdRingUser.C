// Copyright ***********************************************************
//                                                                      
// File ecmdRingUser.C                                  
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

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define ecmdRingUser_C
#include <ecmdCommandUtils.H>
#include <ecmdIntReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <stdio.h>
#include <fstream>
#include <vector>
#undef ecmdRingUser_C
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

int ecmdGetRingDumpUser(int argc, char * argv[]) {
  int rc = ECMD_SUCCESS;

  char * format = ecmdParseOptionWithArgs(&argc, &argv, "-f");
  if (format == NULL) {
    format = "default";
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc < 2) {
    ecmdOutputError("Too few arguments specified; you need at least a chip and a ring name.\nType 'getring -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  ecmdChipTarget target;
  target.chipType = argv[0];

  char * ringName;

  /* find scandef file */
  std::string scandefDir;
  rc = ecmdQueryFileLocation(target, ECMD_FILE_SCANDEF, scandefDir);
  if (rc) {
    scandefDir = "Error occured locating scandef file in dir: " + scandefDir;
    ecmdOutputError(scandefDir.c_str());
    return rc;
  }

  std::string scandefFile = scandefDir + "/test.scandef";
 
  std::ifstream ins(scandefFile.c_str());
  if (ins.fail()) {
    scandefFile = "Error occured opening scandef file: " + scandefFile;
    ecmdOutputError(scandefFile.c_str());
    return ECMD_INVALID_ARGS;  //change this
  }

  ecmdDataBuffer ringBuffer(3);
  ecmdDataBuffer buffer(3);

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  bool validPosFound = false;
  rc = ecmdConfigLooperInit(target);
  if (rc) return rc;

  std::string printed;
  std::string curLine;

  while ( ecmdConfigLooperNext(target) ) {

    for (int i = 1; i < argc; i++) {

      ringName = argv[i];

      rc = getRing(target, ringName, ringBuffer);
      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
      }
      else if (rc) {
        printed = "Error occured performing getring on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }
      else {
        validPosFound = true;     
      }

      bool found = false;
      bool done = false;
      std::vector<std::string> splitArgs;
      int startBit, numBits;

      while (getline(ins, curLine) && !done) {

        if (found) {

          if (curLine[0] == '*' && curLine[2] == 'r' && curLine[6] == '=') {
            done = true;
          }
          else if (curLine[0] == '\0' || curLine[0] == '*' || curLine[0] == '#') {
            //do nothing
          }
          else {

            ecmdParseTokens(curLine, splitArgs);

            printed = splitArgs[3];

            numBits = atoi(splitArgs[0].c_str());
            startBit = atoi(splitArgs[1].c_str());

            if (!strcmp(format, "default")) {

              if (numBits <= 8) {
                printed += " 0b" + ringBuffer.genBinStr(startBit, numBits);
              }
              else {
                printed += " 0x" + ringBuffer.genHexLeftStr(startBit, numBits);
              }

              printed += "\n";

            }
            else {
              ringBuffer.extract(buffer, startBit, numBits);
              printed += ecmdWriteDataFormatted(buffer, format);
            }

            ecmdOutput(printed.c_str());
            
          }

        }
        else if (curLine.find(ringName) != std::string::npos) {
          found = true;
        }

      }

      if (!found) {
        //panic
      }

      //reset the scandef file
      ins.seekg(0);

    }

  }

  return rc;
}

int ecmdGetRegisterUser(int argc, char * argv[]) {
  int rc = ECMD_SUCCESS;

  return rc;
}

int ecmdGetBitsUser(int argc, char * argv[]) {
  int rc = ECMD_SUCCESS;

  bool expectFlag = false;
  bool rawFlag = false;
  bool xstateFlag = false;

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  //expect and mask flags check
  if (ecmdParseOption(&argc, &argv, "-exp")) {
    expectFlag = true;
  }

  if (ecmdParseOption(&argc, &argv, "-r")) {
    rawFlag = true;
  }

  if (ecmdParseOption(&argc, &argv, "-X")) {
    xstateFlag = true;
  }

  /* get format flag, if it's there */
  char * format = ecmdParseOptionWithArgs(&argc, &argv, "-f");
  if (format == NULL) {
    format = "x";
  }
  
  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 4 && !rawFlag) {  //chip + address
    ecmdOutputError("Too few arguments specified; you need at least a chip, ring, startbit, and numbits.\nType 'getbits -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc < 2 && rawFlag) {
    ecmdOutputError("Too few arguments specified; you need at least a chip and a ring name.\nType 'getbits -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //get chip name
  ecmdChipTarget target;
  target.chipType = argv[0];

  char * ringName = argv[1];

  int startBit, numBits;

  if (!rawFlag) {
    startBit = atoi(argv[2]);
    numBits = atoi(argv[3]);
  }

  //container to store data
  ecmdDataBuffer ringBuffer(3);
  ecmdDataBuffer buffer(3);  //the 3 is just a placeholder
  ecmdDataBuffer * expected = NULL;  //don't want to allocate this unless I have to

  if (expectFlag) {
    int argLength = argc - 4;  //account for chip, ringname, startbit, and numbits args args
    expected = new ecmdDataBuffer(argLength);

    for (int i = 0; i < argLength; i++) {
      expected->insertFromHexLeft(argv[i+4], i * 32, 32);
    }

  }
  else if (argc > 4) {
    ecmdOutputError("Too many arguments specified; you probably added an option that wasn't recognized.\nType 'getbits -h' for usage.\n");
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

    rc = getRing(target, ringName, ringBuffer);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "Error occured performing getring on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }

    if (!rawFlag) {
      buffer.setBitLength(numBits);
      ringBuffer.extract(buffer, startBit, numBits);
    }
    else {
      buffer.setBitLength(ringBuffer.getBitLength());
      ringBuffer.extract(buffer, 0, ringBuffer.getBitLength());
    }

    if (expectFlag) {

      if (!ecmdCheckExpected(buffer, *expected)) {

        //@ make this stuff sprintf'd
        printed =  "Actual            : ";
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
  }

  if (!validPosFound) {
    //this is an error common across all UI functions
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

int ecmdPutBitsUser(int argc, char * argv[]) {
  int rc = ECMD_SUCCESS;

  bool xstateFlag = false;
  bool hexFlag = false;

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  //expect and mask flags check
  if (ecmdParseOption(&argc, &argv, "-X")) {
    xstateFlag = true;
  }

  if (ecmdParseOption(&argc, &argv, "-x")) {
    hexFlag = true;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 4) {  //chip + address
    ecmdOutputError("Too few arguments specified; you need at least a chip, ring, startbit, and data block.\nType 'putbits -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 4) {
    ecmdOutputError("Too many arguments specified; you probably added an unsupported option.\nType 'putbits -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //get chip name
  ecmdChipTarget target;
  target.chipType = argv[0];

  //get ring name and starting position
  char * ringName = argv[1];
  int startBit = atoi(argv[2]);
  
  //container to store data
  ecmdDataBuffer ringBuffer(3);
  ecmdDataBuffer buffer(3);  //the 3 is just a placeholder

  if (hexFlag) {
    buffer.insertFromHexLeft(argv[3]);
  }
  else {
    buffer.insertFromBin(argv[3]);
  }
  
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  bool validPosFound = false;
  rc = ecmdConfigLooperInit(target);
  if (rc) return rc;

  std::string printed;

  while ( ecmdConfigLooperNext(target) ) {

    rc = getRing(target, ringName, ringBuffer);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "Error occured performing getring on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }

    ringBuffer.insert(buffer, startBit, buffer.getBitLength());

    rc = putRing(target, ringName, ringBuffer);
    if (rc) {
        printed = "Error occured performing putring on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }

  }

  if (!validPosFound) {
    //this is an error common across all UI functions
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

int ecmdPutRegisterUser(int argc, char * argv[]) {
  int rc = ECMD_SUCCESS;

  return rc;
}

int ecmdCheckRingsUser(int argc, char * argv[]) {
  int rc = ECMD_SUCCESS;

  return rc;
}


int ecmdPutPatternUser(int argc, char * argv[]) {
  int rc = ECMD_SUCCESS;

  return rc;
}



