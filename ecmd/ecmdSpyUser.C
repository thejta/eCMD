// Copyright ***********************************************************
//                                                                      
// File ecmdDaSpyUser.C                                  
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
#define ecmdDaSpyUser_C
#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <stdio.h>
#include <ctype.h>
#undef ecmdDaSpyUser_C
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

int ecmdGetSpyUser(int argc, char * argv[]) {
  int rc = ECMD_SUCCESS;

  bool expectFlag = false;
  bool enumFlag = false;

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  //expect and mask flags check
  char * expectArgTmp = ecmdParseOptionWithArgs(&argc, &argv, "-exp");
  std::string expectArg;
  if (expectArgTmp != NULL) {
    expectFlag = true;
    expectArg = expectArgTmp;
  }

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr == NULL) {
    format = "x";
  }
  else {
    format = formatPtr;
  }

  if (format == "e") {
    enumFlag = true;
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
    ecmdOutputError("Too few arguments specified; you need at least a chip and a spy.\nType 'getspy -h' for usage.");
    return ECMD_INVALID_ARGS;
  }

  //get chip name
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;

  //get spy name
  std::string spyName = argv[1];
  uint32_t startBit = 0x0, numBits = 0x0;

  if (enumFlag) {
    if (argc > 2) {
      startBit = atoi(argv[2]);
    }
    else {
      startBit = 0x0;
    }

    if (argc > 3) {
      numBits = atoi(argv[3]);
    }
    else {
      numBits = 0xFFFFFFFF;
    }
  }

  ecmdDataBuffer * spyBuffer = NULL;
  ecmdDataBuffer * buffer = NULL;
  ecmdDataBuffer * expected = NULL;

  if (!enumFlag) {
    spyBuffer = new ecmdDataBuffer(3);
    buffer = new ecmdDataBuffer(3);
  }

  if (expectFlag) {
    uint32_t expectBits = expectArg.length() << 2;
    expected = new ecmdDataBuffer( 1 + (expectBits/32) );
    expected->insertFromHexLeft(expectArg.c_str(), 0, expectBits);
  }


  bool validPosFound = false;
  rc = ecmdConfigLooperInit(target);
  if (rc) return rc;
  std::string printed;
  std::string enumValue;

  while ( ecmdConfigLooperNext(target) ) {

    if (enumFlag) {
      rc = getSpyEnum(target, spyName.c_str(), enumValue);
    }
    else {
      rc = getSpy(target, spyName.c_str(), *spyBuffer);
    }

    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "Error occured performing getspy on ";
        printed += ecmdWriteTarget(target);
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }

    printed = ecmdWriteTarget(target) + " " + spyName;

    if (enumFlag) {
      printed += " " + enumValue + "\n";
      ecmdOutput( printed.c_str() );
    }
    else {

      uint32_t bitsToFetch = 0x0;
      if (numBits = 0xFFFFFFFF) {
        bitsToFetch = spyBuffer->getBitLength();
      }
      else {
        bitsToFetch = numBits;
      }

      buffer->setBitLength(bitsToFetch);
      spyBuffer->insert(*buffer, startBit, bitsToFetch);

      if (!expectFlag) {

        char outstr[20];
        sprintf(outstr, "(%d:%d)", startBit, startBit + bitsToFetch - 1);
        printed += outstr;

        std::string dataStr = ecmdWriteDataFormatted(*buffer, format);
        if (dataStr[0] != '\n') {
          printed += "\n";
        }
        printed += dataStr;
        ecmdOutput( printed.c_str() );
      }
      else {

        if (!ecmdCheckExpected(*buffer, *expected)) {
          //@ make this stuff sprintf'd
          printed =  "Actual            : ";
          printed += ecmdWriteDataFormatted(*buffer, format);

          printed += "Expected          : ";
          printed += ecmdWriteDataFormatted(*expected, format);
          ecmdOutputError( printed.c_str() );
        }

      }

    }

  }

  if (expectFlag && expected != NULL) {
    delete expected;
    expected = NULL;
  }

  if (!enumFlag) {

    if (buffer != NULL) {
      delete buffer;
      buffer = NULL;
    }
    if (spyBuffer != NULL) {
      delete spyBuffer;
      spyBuffer = NULL;
    }

  }

  if (!validPosFound) {
    //this is an error common across all UI functions
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

int ecmdPutSpyUser(int argc, char * argv[]) {
  int rc = ECMD_SUCCESS;

  bool enumFlag = false;

  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr == NULL) {
    format = "x";
  }
  else {
    format = formatPtr;
  }

  if (format == "e") {
    enumFlag = true;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc < 3) {  //chip + address
    ecmdOutputError("Too few arguments specified; you need at least a chip, a spy, and data.\nType 'getspy -h' for usage.");
    return ECMD_INVALID_ARGS;
  }

  //get chip name
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;

  //get spy name
  std::string spyName = argv[1];
  std::string spyData = argv[argc-1]; //always the last arg

  
  if (enumFlag) {
    for (int i = 0; i < spyData.length(); i++) {
      if (!isxdigit(spyData[i])) {
        break;
      }
    }
    //all chars were hex digits, so we'll work
    //under the assumption the data is not an enum
    //enumFlag = false;
  }

  uint32_t startBit, numBits;
  ecmdDataBuffer * buffer;
  ecmdDataBuffer * spyBuffer;

  if (!enumFlag) {

    buffer = new ecmdDataBuffer(3);
    spyBuffer = new ecmdDataBuffer(3);
    rc = ecmdReadDataFormatted(*buffer, spyData.c_str() , format);
    if (rc) return rc;

    if (argc > 3) {
      startBit = atoi(argv[2]);
    }
    else {
      startBit = 0x0;
    }

    if (argc > 4) {
      numBits = atoi(argv[3]);
    }
    else {
      numBits = buffer->getBitLength();
    }

  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  bool validPosFound = false;
  rc = ecmdConfigLooperInit(target);
  if (rc) return rc;

  std::string printed;
  
  while ( ecmdConfigLooperNext(target) ) {

    if (enumFlag) {
      rc = putSpyEnum(target, spyName.c_str(), spyData);
    }
    else {
      rc = getSpy(target, spyName.c_str(), *spyBuffer);
    }

    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
      if (enumFlag) {
        printed = "Error occured performing putspy (enumerated) on ";
      }
      else {
        printed = "Error occured performing getspy on ";
      }

      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    else {
      validPosFound = true;     
    }

    if (!enumFlag) {

      if (spyBuffer->getBitLength() < numBits) {
        printed = "Number of bits specified is longer than number of bits in spy on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        
      }

      spyBuffer->insert(*buffer, startBit, numBits);

      rc = putSpy(target, spyName.c_str(), *spyBuffer);
      if (rc) {
        printed = "Error occured performing putspy on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }

    }

  }

  return rc;
}


