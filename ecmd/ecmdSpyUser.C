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
/* $Header$ */

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
  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string outputformat = "x";       ///< Output format
  std::string inputformat = "x";        ///< Expect data input format
  std::string expectArg;                ///< String containing expect data
  ecmdDataBuffer spyBuffer;             ///< Buffer to hold entire spy contents
  ecmdDataBuffer buffer;                ///< Buffer to hold user requested part of spy
  ecmdDataBuffer expected;              ///< Buffer to hold expected data

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  //expect and mask flags check
  char * expectArgTmp = ecmdParseOptionWithArgs(&argc, &argv, "-exp");
  if (expectArgTmp != NULL) {
    expectFlag = true;
    expectArg = expectArgTmp;
  }

  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr != NULL) {
    outputformat = formatPtr;
  }

  if (outputformat == "enum") {
    enumFlag = true;
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
    ecmdOutputError("getspy - Too few arguments specified; you need at least a chip and a spy.\n");
    ecmdOutputError("getspy - Type 'getspy -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get spy name
  std::string spyName = argv[1];
  uint32_t startBit = 0x0, numBits = 0x0;

  if (!enumFlag) {
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



  if (expectFlag) {
    rc = ecmdReadDataFormatted(expected, expectArg.c_str(), inputformat);
    if (rc) return rc;
  }


  bool validPosFound = false;
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  std::string printed;
  std::string enumValue;

  /* We are going to enable ring caching to speed up performance */
  ecmdEnableRingCache();


  while ( ecmdConfigLooperNext(target, looperdata) ) {

    if (enumFlag) {
      rc = getSpyEnum(target, spyName.c_str(), enumValue);
    }
    else {
      rc = getSpy(target, spyName.c_str(), spyBuffer);
    }

    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "getspy - Error occured performing getspy on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }

    printed = ecmdWriteTarget(target) + " " + spyName;

    if (enumFlag) {
      printed += "\n" + enumValue + "\n";
      ecmdOutput( printed.c_str() );
    }
    else {

      uint32_t bitsToFetch = 0x0;
      if (numBits = 0xFFFFFFFF) {
        bitsToFetch = spyBuffer.getBitLength();
      }
      else {
        bitsToFetch = numBits;
      }

      buffer.setBitLength(bitsToFetch);
      buffer.insert(spyBuffer, startBit, bitsToFetch);

      if (!expectFlag) {

        char outstr[20];
        sprintf(outstr, "(%d:%d)", startBit, startBit + bitsToFetch - 1);
        printed += outstr;

        std::string dataStr = ecmdWriteDataFormatted(buffer, outputformat);
        if (dataStr[0] != '\n') {
          printed += "\n";
        }
        printed += dataStr;
        ecmdOutput( printed.c_str() );
      }
      else {

        if (!ecmdCheckExpected(buffer, expected)) {
          //@ make this stuff sprintf'd
          printed =  "getspy - Actual            : ";
          printed += ecmdWriteDataFormatted(buffer, outputformat);

          printed += "getspy - Expected          : ";
          printed += ecmdWriteDataFormatted(expected, outputformat);
          ecmdOutputError( printed.c_str() );
        }

      }

    }

    /* Flush out the cache we don't need to keep entries from the previous chips */
    rc = ecmdFlushRingCache();
    if (rc) {
      printed = "getspy - Error occured flushing cache on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
      

  }

  if (!validPosFound) {
    ecmdOutputError("getspy - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

int ecmdPutSpyUser(int argc, char * argv[]) {
  int rc = ECMD_SUCCESS;

  bool enumFlag = false;
  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string format = "x";             ///< Input data format
  std::string dataModifier = "insert";  ///< Default data modifier
  uint32_t startBit, numBits = 0;
  ecmdDataBuffer buffer;                ///< Buffer to hold input data
  ecmdDataBuffer spyBuffer;             ///< Buffer to hold current spy data

  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    format = formatPtr;
  }
  if (format == "enum") {
    enumFlag = true;
  }

  formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-b");
  if (formatPtr != NULL) {
    dataModifier = formatPtr;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc < 3) {  //chip + address
    ecmdOutputError("putspy - Too few arguments specified; you need at least a chip, a spy, and data.\n");
    ecmdOutputError("putspy - Type 'getspy -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get spy name
  std::string spyName = argv[1];
  std::string spyData = argv[argc-1]; //always the last arg

#if 0
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
#endif

  if (!enumFlag) {


    if (argc > 3) {
      startBit = atoi(argv[2]);
    }
    else {
      startBit = 0x0;
    }

    if (argc > 4) {
      numBits = atoi(argv[3]);
    }

    rc = ecmdReadDataFormatted(buffer, spyData.c_str() , format, numBits);
    if (rc) {
      ecmdOutputError("putspy - Problems occurred parsing input data, must be an invalid format\n");
      return rc;
    }

  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  bool validPosFound = false;
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  std::string printed;

  /* We are going to enable ring caching to speed up performance */
  ecmdEnableRingCache();

  while ( ecmdConfigLooperNext(target, looperdata) ) {

    if (enumFlag) {
      rc = putSpyEnum(target, spyName.c_str(), spyData);
    }
    else {
      rc = getSpy(target, spyName.c_str(), spyBuffer);
    }

    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
      if (enumFlag) {
        printed = "putspy - Error occured performing putspy (enumerated) on ";
      }
      else {
        printed = "putspy - Error occured performing putspy on ";
      }

      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    else {
      validPosFound = true;     
    }

    if (!enumFlag) {


      rc = ecmdApplyDataModifier(spyBuffer, buffer,  startBit, dataModifier);
      if (rc) return rc;

      rc = putSpy(target, spyName.c_str(), spyBuffer);
      if (rc) {
        printed = "putspy - Error occured performing putspy on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }

    }

    /* Flush out the cache to actually write the data to the chips */
    rc = ecmdFlushRingCache();
    if (rc) {
      printed = "putspy - Error occured flushing cache on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }

  }

  if (!validPosFound) {
    ecmdOutputError("putspy - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}


