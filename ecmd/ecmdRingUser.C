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
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <stdio.h>
#include <time.h>
#include <fstream>
#include <vector>
#undef ecmdRingUser_C
//----------------------------------------------------------------------
//  User Types
//----------------------------------------------------------------------
struct ecmdLatchInfo {
  std::string latchName;
  uint32_t ringOffset;
  uint32_t length;
};

bool operator< (const ecmdLatchInfo & lhs, const ecmdLatchInfo & rhs) {

  int lhsLeftParen = lhs.latchName.find('(');
  int rhsLeftParen = rhs.latchName.find('(');

  if (lhsLeftParen == std::string::npos || rhsLeftParen == std::string::npos || lhsLeftParen != rhsLeftParen) {
    return lhs.latchName < rhs.latchName;
  }

  std::string lhsSub = lhs.latchName.substr(0, lhsLeftParen);
  std::string rhsSub = rhs.latchName.substr(0, rhsLeftParen);

  if (lhsSub != rhsSub) {
    return lhs.latchName < rhs.latchName;
  }

  int lhsVal = atoi(lhs.latchName.substr(lhsLeftParen+1, lhs.latchName.length()-lhsLeftParen-1).c_str());
  int rhsVal = atoi(rhs.latchName.substr(rhsLeftParen+1, rhs.latchName.length()-rhsLeftParen-1).c_str());

  return lhsVal < rhsVal;
}

bool operator!= (const ecmdLatchInfo & lhs, const ecmdLatchInfo & rhs) {

  int lhsLeftParen = lhs.latchName.find('(');
  int rhsLeftParen = rhs.latchName.find('(');

  if (lhsLeftParen != rhsLeftParen) {
    return 1;
  }

  return (lhs.latchName.substr(0, lhsLeftParen) != rhs.latchName.substr(0,rhsLeftParen));
}

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
  time_t curTime = time(NULL);

  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr == NULL) {
    format = "default";
  }
  else {
    format = formatPtr;
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
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;        /* We don't use threads for this function */

  std::string ringName;

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
  std::string ringPrefix = "ring=";
  std::ifstream ins;
  char outstr[30];

  while ( ecmdConfigLooperNext(target) ) {

    for (int i = 1; i < argc; i++) {

      std::string ringName = argv[i];
      std::string ringArg = ringPrefix + ringName;

      /* find scandef file */
      std::string scandefFile;
      rc = ecmdQueryFileLocation(target, ECMD_FILE_SCANDEF, scandefFile);
      if (rc) {
        scandefFile = "Error occured locating scandef file in dir: " + scandefFile;
        ecmdOutputError(scandefFile.c_str());
        return rc;
      }
 
      ins.open(scandefFile.c_str());
      if (ins.fail()) {
        scandefFile = "Error occured opening scandef file: " + scandefFile;
        ecmdOutputError(scandefFile.c_str());
        return ECMD_INVALID_ARGS;  //change this
      }

      rc = getRing(target, ringName.c_str(), ringBuffer);
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

      //print ring header stuff
      printed = ecmdWriteTarget(target);
      printed += "\n*************************************************\n* ECMD Dump scan ring contents, ";
      printed += ctime(&curTime);
      sprintf(outstr, "* Position %d:%d, ", target.pos, target.core);
      printed += outstr;
      printed += target.chipType + " " + ringName + " Ring\n";
      ecmdChipData chipData;
      rc = ecmdGetChipData(target, chipData);
      if (rc) {
        printed = "Error occured retrieving chip data on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }

      sprintf(outstr, "* Chip EC %d\n", chipData.chipEc);
      printed += outstr;
      sprintf(outstr, "* Ring length: %d bits\n", ringBuffer.getBitLength());
      printed += outstr;
      ecmdOutput(printed.c_str());

      bool found = false;
      bool done = false;
      std::vector<std::string> splitArgs;
      int startBit, numBits;

      while (getline(ins, curLine) && !done) {

        if (found) {

          if (curLine.find(ringPrefix) != std::string::npos) {
            done = true;
          }
          else if (curLine.length() == 0 || curLine[0] == '\0' || curLine[0] == '*' || curLine[0] == '#') {
            //do nothing
          }
          else {

            ecmdParseTokens(curLine, splitArgs);

            printed = splitArgs[3];

            numBits = atoi(splitArgs[0].c_str());
            startBit = atoi(splitArgs[1].c_str());

            if (format == "default") {

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
        else if (curLine.find(ringArg) != std::string::npos) {
          found = true;
        }

      }

      if (!found) {
        //panic
      }

      ins.close();
    }

  }

  if (!validPosFound) {
    ecmdOutputError("Unable to find a valid target to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}


int ecmdGetLatchUser(int argc, char * argv[]) {
  int rc = ECMD_SUCCESS;

  bool expectFlag = false;
  bool exactFlag = false;

  if (ecmdParseOption(&argc, &argv, "-exp")) {
    expectFlag = true;
  }

  if (ecmdParseOption(&argc, &argv, "-exact")) {
    exactFlag = true;
  }

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr == NULL) {
    format = "b";
  }
  else {
    format = formatPtr;
  }
  
  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc < 3) {  //chip + address
    ecmdOutputError("Too few arguments specified; you need at least a chip, ring, and latch name.\nType 'getlatch -h' for usage.");
    return ECMD_INVALID_ARGS;
  }

  //get chip name
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;        /* We don't use threads for this function */

  std::string ringName = argv[1];
  std::string latchName = argv[2];

  uint32_t startBit = 0x0, numBits = 0xFFFFFFFF;

  if (argc > 3) {
    startBit = atoi(argv[3]);
    if (!strcmp(argv[4], "end")) {
      numBits = 0xFFFFFFFF;
    }
    else {
      numBits = atoi(argv[4]);
    }
  }


  ecmdDataBuffer ringBuffer(3);
  ecmdDataBuffer buffer(3);

  bool validPosFound = false;
  rc = ecmdConfigLooperInit(target);
  if (rc) return rc;

  std::string printed;
  std::list< ecmdLatchInfo >::iterator curLatchInfo;

  while ( ecmdConfigLooperNext(target) ) {

    rc = getRing(target, ringName.c_str(), ringBuffer);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
      printed = "Error occured performing getring on ";
      printed += ecmdWriteTarget(target);
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    else {
      validPosFound = true;     
    }

    /* find scandef file */
    std::string scandefFile;
    rc = ecmdQueryFileLocation(target, ECMD_FILE_SCANDEF, scandefFile);
    if (rc) {
      ecmdOutputError(("Error occured locating scandef file: " + scandefFile).c_str());
      return rc;
    }

    std::ifstream ins(scandefFile.c_str());
    if (ins.fail()) {
      ecmdOutputError(("Error occured opening scandef file: " + scandefFile).c_str());
      return ECMD_INVALID_ARGS;  //change this
    }

    //let's go hunting in the scandef for this register (pattern)
    std::list< ecmdLatchInfo > latchInfo;
    ecmdLatchInfo curLatch;
    
    std::string curLine;
    std::vector<std::string> curArgs(4);

    std::string ringPrefix = "ring=";
    std::string ringArg = ringPrefix + ringName;

    bool done = false;
    bool found = false;

    while (getline(ins, curLine) && !done) {

      if (found) {
        if (curLine.length() == 0 || curLine[0] == '\0' || curLine[0] == '*' || curLine[0] == '#') {
          //nada
        }
        else if (curLine.find(ringPrefix) != std::string::npos) {
          done = true;
        }
        else if (!exactFlag && (curLine.find(latchName) != std::string::npos)) {

          ecmdParseTokens(curLine, curArgs);
          curLatch.length = atoi(curArgs[0].c_str());
          curLatch.ringOffset = atoi(curArgs[1].c_str());
          curLatch.latchName = curArgs[3];
          latchInfo.push_back(curLatch);
        }
        else if (exactFlag) {

          ecmdParseTokens(curLine, curArgs);

          if (latchName == curArgs[3]) {
            curLatch.length = atoi(curArgs[0].c_str());
            curLatch.ringOffset = atoi(curArgs[1].c_str());
            curLatch.latchName = curArgs[3];
            latchInfo.push_back(curLatch);
          }

        }

      }
      else {
        if (curLine.find(ringArg) != std::string::npos) {
          found = true;
        }
      }

    }

    ins.close();

    if (!found) {
      ecmdOutputError(("Could not find ring name " + ringName).c_str());
      return ECMD_INVALID_ARGS;
    }

    if (latchInfo.empty()) {
      ecmdOutputError(("No registers found that matched " + latchName).c_str());
      return ECMD_INVALID_ARGS;
    }

    latchInfo.sort();

    if (startBit != 0 || numBits != 0xFFFFFFFF) {
      //confirm that we only fetched ONE latch name
      curLatchInfo = latchInfo.begin();
      while (curLatchInfo != latchInfo.end()) {

        curLatch = (*curLatchInfo);
        curLatchInfo++;
        if (curLatchInfo == latchInfo.end()) break;

        if (curLatch != (*curLatchInfo)) {
          std::string printed = "Multiple latch names found: ";
          printed += curLatch.latchName + " and ";
          printed += (*curLatchInfo).latchName;
          ecmdOutputError( printed.c_str() );
          return ECMD_INVALID_ARGS;
        }
      }
    }

    printed = ecmdWriteTarget(target) + "\n";
    uint32_t bitsToFetch = numBits;
    uint32_t bitsFetched = 0;

    for (curLatchInfo = latchInfo.begin(); numBits > 0 && curLatchInfo != latchInfo.end(); curLatchInfo++) {

      bitsToFetch = ((*curLatchInfo).length < numBits) ? (*curLatchInfo).length : numBits;
      ringBuffer.extract(buffer, (*curLatchInfo).ringOffset, bitsToFetch);

      printed += (*curLatchInfo).latchName + " " + ecmdWriteDataFormatted(buffer, format);
      numBits -= bitsToFetch;
      bitsFetched += bitsToFetch;
  
    }
    
    ecmdOutput( printed.c_str() );

  }

  if (!validPosFound) {
    ecmdOutputError("Unable to find a valid target to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

int ecmdGetBitsUser(int argc, char * argv[]) {
  int rc = ECMD_SUCCESS;

  bool expectFlag = false;
  bool xstateFlag = false;

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  //expect and mask flags check
  if (ecmdParseOption(&argc, &argv, "-exp")) {
    expectFlag = true;
  }

  if (ecmdParseOption(&argc, &argv, "-X")) {
    xstateFlag = true;
  }

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr == NULL) {
    format = "b";
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
  if (argc < 4) {  //chip + address
    ecmdOutputError("Too few arguments specified; you need at least a chip, ring, startbit, and numbits.\nType 'getbits -h' for usage.");
    return ECMD_INVALID_ARGS;
  }

  //get chip name
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;        /* We don't use threads for this function */

  std::string ringName = argv[1];

  uint32_t startBit = atoi(argv[2]);
  uint32_t numBits;

  if (!strcmp(argv[3], "end")) {
    numBits = 0xFFFFFFFF;
  }
  else {
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
    ecmdOutputError("Too many arguments specified; you probably added an option that wasn't recognized.\nType 'getbits -h' for usage.");
    return ECMD_INVALID_ARGS;
  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  bool validPosFound = false;
  rc = ecmdConfigLooperInit(target);
  if (rc) return rc;
  char outstr[30];
  std::string printed;

  while ( ecmdConfigLooperNext(target) ) {

    rc = getRing(target, ringName.c_str(), ringBuffer);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "Error occured performing getring on ";
        printed += ecmdWriteTarget(target);
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }


    numBits = (ringBuffer.getBitLength() - startBit < numBits) ? ringBuffer.getBitLength() - startBit : numBits;
    buffer.setBitLength(numBits);
    ringBuffer.extract(buffer, startBit, numBits);

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
      printed = ecmdWriteTarget(target) + "  " + ringName;
      sprintf(outstr, "(%d:%d)", startBit, startBit + numBits - 1);
      printed += outstr;
      std::string dataStr = ecmdWriteDataFormatted(buffer, format);
      if (dataStr[0] != '\n') {
        printed += "\n";
      }
      printed += dataStr;
      ecmdOutput( printed.c_str() );
    }

  }

  if (expectFlag && expected != NULL) {
    delete expected;
    expected = NULL;
  }

  if (!validPosFound) {
    ecmdOutputError("Unable to find a valid target to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

int ecmdPutBitsUser(int argc, char * argv[]) {
  int rc = ECMD_SUCCESS;

  bool xstateFlag = false;

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  //expect and mask flags check
  if (ecmdParseOption(&argc, &argv, "-X")) {
    xstateFlag = true;
  }

  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr == NULL) {
    format = "b";
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
  if (argc < 4) {  //chip + address
    ecmdOutputError("Too few arguments specified; you need at least a chip, ring, startbit, and data block.\nType 'putbits -h' for usage.");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 4) {
    ecmdOutputError("Too many arguments specified; you probably added an unsupported option.\nType 'putbits -h' for usage.");
    return ECMD_INVALID_ARGS;
  }

  //get chip name
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;        /* We don't use threads for this function */

  //get ring name and starting position
  std::string ringName = argv[1];
  int startBit = atoi(argv[2]);
  
  //container to store data
  ecmdDataBuffer ringBuffer(3);
  ecmdDataBuffer buffer(3);  //the 3 is just a placeholder

  rc = ecmdReadDataFormatted(buffer, argv[3], format);
  if (rc) return rc;

  
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  bool validPosFound = false;
  rc = ecmdConfigLooperInit(target);
  if (rc) return rc;

  std::string printed;

  while ( ecmdConfigLooperNext(target) ) {

    rc = getRing(target, ringName.c_str(), ringBuffer);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "Error occured performing getring on ";
        printed += ecmdWriteTarget(target);
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }

    if (startBit + buffer.getBitLength() > ringBuffer.getBitLength()) {
      ecmdOutputError("startbit + numbits > ring length, buffer overflow");
      return ECMD_INVALID_ARGS;
    }

    ringBuffer.insert(buffer, startBit, buffer.getBitLength());

    rc = putRing(target, ringName.c_str(), ringBuffer);
    if (rc) {
        printed = "Error occured performing putring on ";
        printed += ecmdWriteTarget(target);
        ecmdOutputError( printed.c_str() );
        return rc;
    }

  }

  if (!validPosFound) {
    ecmdOutputError("Unable to find a valid target to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

int ecmdPutLatchUser(int argc, char * argv[]) {
  int rc = ECMD_SUCCESS;

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr == NULL) {
    format = "b";
  }
  else {
    format = formatPtr;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc < 4) {  //chip + address
    ecmdOutputError("Too few arguments specified; you need at least a chip, ring, latch name, and data.\nType 'putlatch -h' for usage.");
    return ECMD_INVALID_ARGS;
  }

  //get chip name
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;        /* We don't use threads for this function */

  std::string ringName = argv[1];
  std::string latchName = argv[2];

  ecmdDataBuffer buffer(3);

  //data is always the last arg
  rc = ecmdReadDataFormatted(buffer, argv[argc-1], format);
  if (rc) return rc;

  int startBit = 0, numBits = -1;

  if (argc > 4) {
    startBit = atoi(argv[3]);
    numBits = atoi(argv[4]);
  }
  else {
    numBits = buffer.getBitLength();
  }

  ecmdDataBuffer ringBuffer(3);
  ecmdDataBuffer bufferCopy(3);

  bool validPosFound = false;
  rc = ecmdConfigLooperInit(target);
  if (rc) return rc;

  std::string printed;
  std::list< ecmdLatchInfo >::iterator curLatchInfo;

  while ( ecmdConfigLooperNext(target) ) {

    rc = getRing(target, ringName.c_str(), ringBuffer);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
      printed = "Error occured performing getring on ";
      printed += ecmdWriteTarget(target);
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    else {
      validPosFound = true;     
    }

    /* find scandef file */
    std::string scandefFile;
    rc = ecmdQueryFileLocation(target, ECMD_FILE_SCANDEF, scandefFile);
    if (rc) {
      ecmdOutputError(("Error occured locating scandef file: " + scandefFile).c_str());
      return rc;
    }

    std::ifstream ins(scandefFile.c_str());
    if (ins.fail()) {
      ecmdOutputError(("Error occured opening scandef file: " + scandefFile).c_str());
      return ECMD_INVALID_ARGS;  //change this
    }

    //let's go hunting in the scandef for this register (pattern)
    std::list< ecmdLatchInfo > latchInfo;
    ecmdLatchInfo curLatch;
    ecmdLatchInfo tmpLatch;

    std::string curLine;
    std::vector<std::string> curArgs(4);

    std::string ringPrefix = "ring=";
    std::string ringArg = ringPrefix + ringName;

    bool done = false;
    bool found = false;

    while (getline(ins, curLine) && !done) {

      if (found) {
        if (curLine.length() == 0 || curLine[0] == '\0' || curLine[0] == '*' || curLine[0] == '#') {
          //nada
        }
        else if (curLine.find(ringPrefix) != std::string::npos) {
          done = true;
        }
        else if (curLine.find(latchName) != std::string::npos) {

          ecmdParseTokens(curLine, curArgs);
          curLatch.length = atoi(curArgs[0].c_str());
          curLatch.ringOffset = atoi(curArgs[1].c_str());
          curLatch.latchName = curArgs[3];
          latchInfo.push_back(curLatch);
        }
      }
      else {
        if (curLine.find(ringArg) != std::string::npos) {
          found = true;
        }
      }

    }

    ins.close();

    if (!found) {
      ecmdOutputError(("Could not find ring name " + ringName).c_str());
      return ECMD_INVALID_ARGS;
    }

    if (latchInfo.empty()) {
      ecmdOutputError(("No registers found that matched " + latchName).c_str());
      return ECMD_INVALID_ARGS;
    }

    latchInfo.sort();

    //confirm that we only fetched ONE latch name
    curLatchInfo = latchInfo.begin();
    while (curLatchInfo != latchInfo.end()) {

      curLatch = (*curLatchInfo);
      curLatchInfo++;
      if (curLatchInfo == latchInfo.end()) break;

      if (curLatch != (*curLatchInfo)) {
        std::string printed = "Multiple latch names found: ";
        printed += curLatch.latchName + " and ";
        printed += (*curLatchInfo).latchName;
        ecmdOutputError( printed.c_str() );
        return ECMD_INVALID_ARGS;
      }
    }


    bufferCopy.copy(buffer);
   
    for (curLatchInfo = latchInfo.begin(); bufferCopy.getBitLength() > 0 && curLatchInfo != latchInfo.end(); curLatchInfo++) {

      ringBuffer.insert(bufferCopy, (*curLatchInfo).ringOffset, (*curLatchInfo).length);
      bufferCopy.shiftLeft((*curLatchInfo).length);
    }

    rc = putRing(target, ringName.c_str(), ringBuffer);
    if (rc) {
        printed = "Error occured performing putring on ";
        printed += ecmdWriteTarget(target);
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    
  }

  if (!validPosFound) {
    ecmdOutputError("Unable to find a valid target to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

int ecmdCheckRingsUser(int argc, char * argv[]) {
  int rc = ECMD_SUCCESS;

  bool allRingsFlag = false;

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc < 2) {  //chip + address
    ecmdOutputError("Too few arguments specified; you need at least a chip and a ring.\nType 'checkrings -h' for usage.");
    return ECMD_INVALID_ARGS;
  }
  if (argc > 2) {
    ecmdOutputError("Too many arguments specified; you probably added an unsupported option.\nType 'checkrings -h' for usage.");
    return ECMD_INVALID_ARGS;
  }


  //get chip name
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;

  std::string ringName = argv[1];

  if (ringName == "all")
    allRingsFlag = true;

  
  ecmdDataBuffer ringBuffer(1048);  //nice and big
  std::string printed;
  char outstr[20];
  uint32_t pattern0 = 0xAAAA0000;
  uint32_t pattern1 = 0x5555FFFF;
  uint32_t pattern = 0x0;

  bool validPosFound = false;
  rc = ecmdConfigLooperInit(target);
  if (rc) return rc;

  std::list<ecmdRingData> queryRingData;

  while (ecmdConfigLooperNext(target)) {

    if (allRingsFlag) {
      rc = ecmdQueryRing(target, queryRingData);
    }
    else {
      rc = ecmdQueryRing(target, queryRingData, ringName.c_str());
    }

    if (rc) {
      return rc;
    }

    std::list<ecmdRingData>::iterator curRingData = queryRingData.begin();

    while (curRingData != queryRingData.end()) {

      ringName = (*curRingData).ringNames.front();
      ringBuffer.setBitLength((*curRingData).bitLength);

      if (!curRingData->isCheckable && allRingsFlag) {
        curRingData++;
        continue;
      }

      for (int i = 0; i < 2; i++) {

        if (i % 2) {
          pattern = pattern0;
          ringBuffer.flushTo0();
        }
        else {
          pattern = pattern1;
          ringBuffer.flushTo1();
        }

        ringBuffer.setWord(0, pattern);  //write the pattern

        rc = putRing(target, ringName.c_str(), ringBuffer);
        if (rc == ECMD_TARGET_NOT_CONFIGURED) {
          break;
        }
        else if (rc) {
          printed = "Error occured performing putring on ";
          printed += ecmdWriteTarget(target);
          ecmdOutputError( printed.c_str() );
          return rc;
        }
        else {
          validPosFound = true;
        }

        rc = getRing(target, ringName.c_str(), ringBuffer);
        if (rc) {
          printed = "Error occured performing getring on ";
          printed += ecmdWriteTarget(target);
          ecmdOutputError( printed.c_str() );
          return rc;
        }

        if (ringBuffer.getWord(0) != pattern) {
          sprintf(outstr, "Pattern: %.8X Data: %.8X\n", pattern, ringBuffer.getWord(0));
          printed = "Data fetched from ring " + ringName + " did not match ";
          printed += outstr;
          printed += "\nError occured performing checkring on " + ecmdWriteTarget(target);
          ecmdOutputWarning( printed.c_str() );
        }
        else if (ringBuffer.isBitSet(32, ringBuffer.getBitLength() - 32)) {
          if (i % 2) {
            printed = "Non-zero";
          }
          else {
            printed = "Non-one";
          }
          printed += " bits found after pattern in ring " + ringName;
          printed += "\nError occured performing checkring on " + ecmdWriteTarget(target);
          ecmdOutputWarning( printed.c_str() );
        }

      }

      curRingData++;
    }
    


  }
  
  if (!validPosFound) {
    ecmdOutputError("Unable to find a valid target to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}


int ecmdPutPatternUser(int argc, char * argv[]) {
  int rc = ECMD_SUCCESS;

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr == NULL) {
    format = "xr";
  }
  else {
    format = formatPtr;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc < 3) {  //chip + address
    ecmdOutputError("Too few arguments specified; you need at least a chip, ring, and pattern.\nType 'putpattern -h' for usage.");
    return ECMD_INVALID_ARGS;
  }
  if (argc > 3) {
    ecmdOutputError("Too many arguments specified; you probably added an unsupported option.\nType 'putpattern -h' for usage.");
    return ECMD_INVALID_ARGS;
  }


  //get chip name
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;

  std::string ringName = argv[1];

  ecmdDataBuffer buffer(3);
  rc = ecmdReadDataFormatted(buffer, argv[2], format);
  if (rc) return rc;

  ecmdDataBuffer ringBuffer(3);
  std::string printed;

  bool validPosFound = false;
  rc = ecmdConfigLooperInit(target);
  if (rc) return rc;

  while (ecmdConfigLooperNext(target)) {

    rc = getRing(target, ringName.c_str(), ringBuffer);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
      printed = "Error occured performing getring on ";
      printed += ecmdWriteTarget(target);
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    else {
      validPosFound = true;     
    }

    uint32_t curOffset = 0;
    uint32_t numBitsToInsert = 0;
    uint32_t numBitsInRing = ringBuffer.getBitLength();
    while (curOffset < numBitsInRing) {
      numBitsToInsert = (32 < numBitsInRing - curOffset) ? 32 : numBitsInRing - curOffset;
      ringBuffer.insert(buffer, curOffset, numBitsToInsert);
      curOffset += numBitsToInsert;
    }

    rc = putRing(target, ringName.c_str(), ringBuffer);
    if (rc) {
        printed = "Error occured performing putring on ";
        printed += ecmdWriteTarget(target);
        ecmdOutputError( printed.c_str() );
        return rc;
    }

  }

  if (!validPosFound) {
    ecmdOutputError("Unable to find a valid target to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}



