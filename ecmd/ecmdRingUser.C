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


/** @brief Used to hold info out of the scandef for get/putlatch etc. */
struct ecmdLatchInfo {
  std::string latchName;                ///< Full Latch Name (including any parens)
  uint32_t ringOffset;                  ///< Ring Offset
  uint32_t length;                      ///< Length of entry
  uint32_t latchStartBit;               ///< Start bit in latch (comes from parens in latch name) 
  uint32_t latchEndBit;                 ///< End bit in latch (comes from parens in latch name) 

};

/** @brief Used to buffer scandef data to avoid searching for each chip ec */
struct ecmdLatchBufferEntry {
  std::list<ecmdLatchInfo> entry;       ///< Data from Scandef
  std::string scandefname;              ///< Name of scandef where data was retrieved
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

  return lhs.latchStartBit < rhs.latchStartBit;
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
    ecmdOutputError("getringdump - Too few arguments specified; you need at least a chip and a ring name.\n");
    ecmdOutputError("getringdump - Type 'getring -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  std::string ringName;

  ecmdDataBuffer ringBuffer;
  ecmdDataBuffer buffer;

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
        scandefFile = "getringdump - Error occured locating scandef file in dir: " + scandefFile + "\n";
        ecmdOutputError(scandefFile.c_str());
        return rc;
      }
 
      ins.open(scandefFile.c_str());
      if (ins.fail()) {
        scandefFile = "getringdump - Error occured opening scandef file: " + scandefFile + "\n";
        ecmdOutputError(scandefFile.c_str());
        return ECMD_INVALID_ARGS;  //change this
      }

      rc = getRing(target, ringName.c_str(), ringBuffer);
      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
      }
      else if (rc) {
        printed = "getringdump - Error occured performing getring on ";
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
        printed = "getringdump - Error occured retrieving chip data on ";
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

          if (curLine[0] == '*' && curLine.find(ringPrefix) != std::string::npos) {
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
        else if (curLine[0] == '*' && curLine.find(ringArg) != std::string::npos) {
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
    ecmdOutputError("getringdump - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}


int ecmdGetLatchUser(int argc, char * argv[]) {
  int rc = ECMD_SUCCESS;

  bool expectFlag = false;
  bool exactFlag = false;
  bool noCompressFlag = false;
  bool foundit;
  std::list<ecmdLatchBufferEntry> latchBuffer;
  std::list<ecmdLatchBufferEntry>::iterator bufferit;
  std::list<ecmdLatchInfo>::iterator latchit;
  ecmdLatchBufferEntry curEntry;

  if (ecmdParseOption(&argc, &argv, "-exp")) {
    expectFlag = true;
  }

  if (ecmdParseOption(&argc, &argv, "-exact")) {
    exactFlag = true;
  }

  if (ecmdParseOption(&argc, &argv, "-nocompress")) {
    noCompressFlag = true;
  }

  /* get format flag, if it's there */
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

  if (argc < 3) {  //chip + address
    ecmdOutputError("getlatch - Too few arguments specified; you need at least a chip, ring, and latch name.\n");
    ecmdOutputError("getlatch - Type 'getlatch -h' for usage.");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  std::string ringName = argv[1];
  std::string latchName = argv[2];

  uint32_t startBit = 0x0, numBits = 0x0FFFFFFF;

  if (argc > 3) {
    startBit = atoi(argv[3]);
    if (!strcmp(argv[4], "end")) {
      numBits = 0x0FFFFFFF;
    }
    else {
      numBits = atoi(argv[4]);
    }
  }


  ecmdDataBuffer ringBuffer;
  ecmdDataBuffer buffer(10000);
  ecmdDataBuffer buffertemp(10000);

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
      printed = "getlatch - Error occured performing getring on ";
      printed += ecmdWriteTarget(target) + "\n";
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
      ecmdOutputError(("getlatch - Error occured locating scandef file: " + scandefFile + "\n").c_str());
      return rc;
    }

    /* Let's see if we have already looked up this info */
    foundit = false;
    for (bufferit = latchBuffer.begin(); bufferit != latchBuffer.end(); bufferit ++) {
      if (bufferit->scandefname == scandefFile) {
        curEntry = (*bufferit);
        foundit = true;
        break;
      }
    }

    /* We don't have it already, let's go looking */
    if (!foundit) {
      std::ifstream ins(scandefFile.c_str());
      if (ins.fail()) {
        ecmdOutputError(("getlatch - Error occured opening scandef file: " + scandefFile + "\n").c_str());
        return ECMD_INVALID_ARGS;  //change this
      }

      //let's go hunting in the scandef for this register (pattern)
      ecmdLatchInfo curLatch;

      std::string curLine;
      std::vector<std::string> curArgs(4);

      std::string ringPrefix = "ring=";
      std::string ringArg = ringPrefix + ringName;
      std::string temp;

      bool done = false;
      bool found = false;
      int  leftParen;
      int  colon;

      while (getline(ins, curLine) && !done) {

        if (found) {

          if ((curLine[0] == '*') && curLine.find(ringPrefix) != std::string::npos) {
            done = true; continue;
          }
          else if (curLine.length() == 0 || curLine[0] == '\0' || curLine[0] == '*' || curLine[0] == '#') {
            continue;
            //nada
          }
          else if (!exactFlag && (curLine.find(latchName) != std::string::npos)) {

            ecmdParseTokens(curLine, curArgs);
            curLatch.length = atoi(curArgs[0].c_str());
            curLatch.ringOffset = atoi(curArgs[1].c_str());
            curLatch.latchName = curArgs[3];
          }
          else if (exactFlag) {

            ecmdParseTokens(curLine, curArgs);

            if (latchName == curArgs[3]) {
              curLatch.length = atoi(curArgs[0].c_str());
              curLatch.ringOffset = atoi(curArgs[1].c_str());
              curLatch.latchName = curArgs[3];
            }

          } else {
            /* Not one we want */
            continue;
          }

          /* Let's parse out the start/end bit if they exist */
          leftParen = curLatch.latchName.find('(');
          if (leftParen == std::string::npos) {
            /* This latch doesn't have any parens */
            curLatch.latchStartBit = curLatch.latchEndBit = 0;
          } else {
            temp = curLatch.latchName.substr(leftParen+1, curLatch.latchName.length() - leftParen - 1);
            curLatch.latchStartBit = atoi(temp.c_str());

            /* Is this a multibit or single bit */
            if ((colon = temp.find(':')) != std::string::npos) {
              curLatch.latchEndBit = atoi(temp.substr(colon+1, temp.length()).c_str());
            } else {
              curLatch.latchEndBit = curLatch.latchStartBit;
            }
          }
          curEntry.entry.push_back(curLatch);

        }
        else {
          if (curLine[0] == '*' && curLine.find(ringArg) != std::string::npos) {
            found = true;
          }
        }

      }

      ins.close();

      if (!found) {
        ecmdOutputError(("getlatch - Could not find ring name " + ringName + "\n").c_str());
        return ECMD_INVALID_ARGS;
      }

      if (curEntry.entry.empty()) {
        ecmdOutputError(("getlatch - No registers found that matched " + latchName + "\n").c_str());
        return ECMD_INVALID_ARGS;
      }

      curEntry.scandefname = scandefFile;
      curEntry.entry.sort();

#if 0
      /* This check needs to be done better to compensate for compressed latchs that may be missing bits as well */
      if ((startBit != 0 || numBits != 0x0FFFFFFF) && !compressFlag) {
        //confirm that we only fetched ONE latch name if they aren't compressing and using a start/numbitsy
        curLatchInfo = latchInfo.begin();
        while (curLatchInfo != latchInfo.end()) {

          curLatch = (*curLatchInfo);
          curLatchInfo++;
          if (curLatchInfo == latchInfo.end()) break;

          if (curLatch != (*curLatchInfo)) {
            std::string printed = "getlatch - Multiple latch names found cannot specify start/num bits without -compress flag : ";
            printed += curLatch.latchName + " and ";
            printed += (*curLatchInfo).latchName + "\n";
            ecmdOutputError( printed.c_str() );
            return ECMD_INVALID_ARGS;
          }
        }
      }
#endif

      /* Let's push this entry onto the stack */
      latchBuffer.push_back(curEntry);
    
    }

    printed = ecmdWriteTarget(target) + "\n";
    ecmdOutput(printed.c_str());
    uint32_t bitsToFetch = numBits;
    uint32_t bitsFetched = 0;
    int curLatchBit = -1;               // This is the actual latch bit we are looking for next
    int curStartBit = startBit;         // This is the offset into the current entry to start extraction
    int curEndBit = -1;
    int curBufferBit = 0;               // Current bit to insert into buffered register
    int curBitsToFetch = numBits;       // This is the total number of bits left to fetch
    int dataStartBit = -1;              // Actual start bit of buffered register
    int dataEndBit = -1;                // Actual end bit of buffered register
    std::string latchname = "";

    for (curLatchInfo = curEntry.entry.begin(); curLatchInfo != curEntry.entry.end(); curLatchInfo++) {

      if (noCompressFlag) {
        bitsToFetch = ((*curLatchInfo).length < curBitsToFetch) ? (*curLatchInfo).length : curBitsToFetch;
        ringBuffer.extract(buffer, (*curLatchInfo).ringOffset, bitsToFetch);

        printed = curLatchInfo->latchName + " ";
        if (format == "default") {

          if (bitsToFetch <= 8) {
            printed += " 0b" + buffer.genBinStr();
          }
          else {
            printed += " 0x" + buffer.genHexLeftStr();
          }

          printed += "\n";

        }
        else {
          printed += ecmdWriteDataFormatted(buffer, format);
        }
        ecmdOutput( printed.c_str());
        curBitsToFetch -= bitsToFetch;
        bitsFetched += bitsToFetch;


        /**** NON-Compressed output ****/
      } else {


        /* Do we have previous data here , or some missing bits in the scandef latchs ?*/
        if (((dataStartBit != -1) && (curLatchBit != curLatchInfo->latchStartBit) && (curLatchBit != curLatchInfo->latchEndBit)) ||
            ((latchname == "") || (latchname != curLatchInfo->latchName.substr(0, latchname.length())))) {
//printf("I'm in here dumping things\n");
          /* I have some good data here */
          if (latchname != "") {
            char temp[20];
            printed =  latchname;
            sprintf(temp,"(%d:%d)", dataStartBit, dataEndBit);
            printed += temp;
            buffer.extract(buffertemp, 0, dataEndBit - dataStartBit + 1);
            if (format == "default") {
              if (buffer.getBitLength() <= 8)  printed += " 0b" + buffertemp.genBinStr();
              else printed += " 0x" + buffertemp.genHexLeftStr();
              printed += "\n";
            } else {
              printed += ecmdWriteDataFormatted(buffertemp, format);
            }
            ecmdOutput( printed.c_str());
          }
          /* If this is a fresh one we need to reset everything */
          if ((latchname == "") || (latchname != curLatchInfo->latchName.substr(0, latchname.length()))) {
            curEndBit = dataStartBit = dataEndBit = -1;
            curStartBit = startBit;
            curBitsToFetch = numBits;
            curBufferBit = 0;
            latchname = curLatchInfo->latchName.substr(0, curLatchInfo->latchName.find('('));
            curLatchBit = curLatchInfo->latchStartBit < curLatchInfo->latchEndBit ? curLatchInfo->latchStartBit : curLatchInfo->latchEndBit;
          } else {
            /* This is the case where the scandef had holes in the register, so we will continue with this latch, but skip some bits */
            dataStartBit = dataEndBit = -1;
            curBufferBit = 0;
            /* Decrement the bits to fetch by the hole in the latch */
            curBitsToFetch -= (curLatchInfo->latchStartBit < curLatchInfo->latchEndBit ? curLatchInfo->latchStartBit : curLatchInfo->latchEndBit) - curLatchBit;
            curLatchBit = curLatchInfo->latchStartBit < curLatchInfo->latchEndBit ? curLatchInfo->latchStartBit : curLatchInfo->latchEndBit;
          }
        }
//printf("New Entry : %s : %d:%d\n",latchname.c_str(), curLatchInfo->latchStartBit, curLatchInfo->latchEndBit);

        /* Do we want anything in here */
//printf("fetlen %d : curStartBit %d : curLatchBit %d : curBufferBit %d : curBitsToFetch %d : dataStartBit %d : dataEndBit %d\n",bitsToFetch, curStartBit, curLatchBit, curBufferBit, curBitsToFetch, dataStartBit, dataEndBit);

        /* Check if the bits are ordered from:to (0:10) or just (1) */
        if (((curLatchInfo->latchEndBit >= curLatchInfo->latchStartBit) && (curStartBit <= curLatchInfo->latchEndBit) && (curLatchBit <= curLatchInfo->latchEndBit)) ||
            /* Check if the bits are ordered to:from (10:0) */
            ((curLatchInfo->latchStartBit > curLatchInfo->latchEndBit) && (curStartBit <= curLatchInfo->latchStartBit) && (curLatchBit <= curLatchInfo->latchStartBit))) {

          bitsToFetch = ((curLatchInfo->length - curStartBit) < curBitsToFetch) ? curLatchInfo->length - curStartBit : curBitsToFetch;

          /* Setup the actual data bits displayed */
          if (dataStartBit == -1) {
            dataStartBit = curLatchBit + curStartBit;
            dataEndBit = dataStartBit - 1;
          }
          dataEndBit += bitsToFetch;


          /* Extract bits if ordered from:to (0:10) */
          if (curLatchInfo->latchEndBit > curLatchInfo->latchStartBit) {
            ringBuffer.extract(buffertemp, curLatchInfo->ringOffset + curStartBit, bitsToFetch);
            buffer.insert(buffertemp, curBufferBit, bitsToFetch); curBufferBit += bitsToFetch;

            curLatchBit = curLatchInfo->latchEndBit + 1;
          } else {
            /* Extract if bits are ordered to:from (10:0) or just (1) */
            for (int bit = 0; bit < bitsToFetch; bit ++) {
              if (ringBuffer.isBitSet(curLatchInfo->ringOffset + (curLatchInfo->length - 1) - curStartBit - bit))
                buffer.setBit(curBufferBit++);
              else
                buffer.clearBit(curBufferBit++);
            }

            curLatchBit = curLatchInfo->latchStartBit + 1;

          }

          curStartBit = 0;
          curBitsToFetch -= bitsToFetch;
        } else {
          /* Nothing was there that we needed, let's try the next entry */
          curLatchBit = curLatchInfo->latchStartBit < curLatchInfo->latchEndBit ? curLatchInfo->latchEndBit + 1: curLatchInfo->latchStartBit + 1;

          curStartBit -= curLatchInfo->length;
        }
//printf("fetlen %d : curStartBit %d : curLatchBit %d : curBufferBit %d : curBitsToFetch %d : dataStartBit %d : dataEndBit %d\n",bitsToFetch, curStartBit, curLatchBit, curBufferBit, curBitsToFetch, dataStartBit, dataEndBit);
      }
    }

    /* We need to dump any data we have stored here */
    if (!noCompressFlag) {
      char temp[20];
      /* Do we have previous data here , and some missing bits in the scandef latchs ?*/
      if ((curLatchBit != -1)) {
        printed =  latchname;
        sprintf(temp,"(%d:%d)", dataStartBit, dataEndBit);
        printed += temp;
        buffer.extract(buffertemp, 0, dataEndBit - dataStartBit + 1);
        if (format == "default") {
          if (buffer.getBitLength() <= 8)  printed += " 0b" + buffertemp.genBinStr();
          else printed += " 0x" + buffertemp.genHexLeftStr();
          printed += "\n";
        } else {
          printed += ecmdWriteDataFormatted(buffertemp, format);
        }
        ecmdOutput( printed.c_str());
      }
    }
  }


  if (!validPosFound) {
    ecmdOutputError("getlatch - Unable to find a valid chip to execute command on\n");
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
    ecmdOutputError("getbits - Too few arguments specified; you need at least a chip, ring, startbit, and numbits.\n");
    ecmdOutputError("getbits - Type 'getbits -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

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
  ecmdDataBuffer ringBuffer;
  ecmdDataBuffer buffer; 
  ecmdDataBuffer * expected = NULL;  //don't want to allocate this unless I have to

  if (expectFlag) {
    int argLength = argc - 4;  //account for chip, ringname, startbit, and numbits args args
    expected = new ecmdDataBuffer(argLength);

    for (int i = 0; i < argLength; i++) {
      expected->insertFromHexLeft(argv[i+4], i * 32, 32);
    }

  }
  else if (argc > 4) {
    ecmdOutputError("getbits - Too many arguments specified; you probably added an option that wasn't recognized.\n");
    ecmdOutputError("getbits - Type 'getbits -h' for usage.\n");
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
        printed = "getbits - Error occured performing getring on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }


    numBits = (ringBuffer.getBitLength() - startBit < numBits) ? ringBuffer.getBitLength() - startBit : numBits;
    ringBuffer.extract(buffer, startBit, numBits);

    if (expectFlag) {

      if (!ecmdCheckExpected(buffer, *expected)) {

        //@ make this stuff sprintf'd
        printed =  "Actual            : ";
        printed += ecmdWriteDataFormatted(buffer, format);
        ecmdOutputError( printed.c_str() );

        printed = "Expected          : ";
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
    ecmdOutputError("getbits - Unable to find a valid chip to execute command on\n");
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
    ecmdOutputError("putbits - Too few arguments specified; you need at least a chip, ring, startbit, and data block.\n");
    ecmdOutputError("putbits - Type 'putbits -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 4) {
    ecmdOutputError("putbits - Too many arguments specified; you probably added an unsupported option.\n");
    ecmdOutputError("putbits - Type 'putbits -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get ring name and starting position
  std::string ringName = argv[1];
  int startBit = atoi(argv[2]);
  
  //container to store data
  ecmdDataBuffer ringBuffer;
  ecmdDataBuffer buffer; 

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
        printed = "putbits - Error occured performing getring on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }

    if (startBit + buffer.getBitLength() > ringBuffer.getBitLength()) {
      ecmdOutputError("putbits - startbit + numbits > ring length, buffer overflow\n");
      return ECMD_INVALID_ARGS;
    }

    ringBuffer.insert(buffer, startBit, buffer.getBitLength());

    rc = putRing(target, ringName.c_str(), ringBuffer);
    if (rc) {
        printed = "putbits - Error occured performing putring on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }

  }

  if (!validPosFound) {
    ecmdOutputError("putbits - Unable to find a valid chip to execute command on\n");
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
    format = "x";
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
    ecmdOutputError("putlatch - Too few arguments specified; you need at least a chip, ring, latch name, and data.\n");
    ecmdOutputError("putlatch - Type 'putlatch -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  std::string ringName = argv[1];
  std::string latchName = argv[2];

  ecmdDataBuffer buffer;

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

  ecmdDataBuffer ringBuffer;
  ecmdDataBuffer bufferCopy;

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
      printed = "putlatch - Error occured performing getring on ";
      printed += ecmdWriteTarget(target) + "\n";
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
      ecmdOutputError(("putlatch - Error occured locating scandef file: " + scandefFile + "\n").c_str());
      return rc;
    }

    std::ifstream ins(scandefFile.c_str());
    if (ins.fail()) {
      ecmdOutputError(("putlatch - Error occured opening scandef file: " + scandefFile + "\n").c_str());
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
        if (curLine.length() == 0 || curLine[0] == '\0' || curLine[0] == '#') {
          //nada
        }
        else if (curLine.find(ringPrefix) != std::string::npos) {
          done = true;
        }
        else if ((curLine[0] != '*') && curLine.find(latchName) != std::string::npos) {

          ecmdParseTokens(curLine, curArgs);
          curLatch.length = atoi(curArgs[0].c_str());
          curLatch.ringOffset = atoi(curArgs[1].c_str());
          curLatch.latchName = curArgs[3];
          latchInfo.push_back(curLatch);
        }
      }
      else {
        if ((curLine[0] == '*') && curLine.find(ringArg) != std::string::npos) {
          found = true;
        }
      }

    }

    ins.close();

    if (!found) {
      ecmdOutputError(("putlatch - Could not find ring name " + ringName + "\n").c_str());
      return ECMD_INVALID_ARGS;
    }

    if (latchInfo.empty()) {
      ecmdOutputError(("putlatch - No registers found that matched " + latchName + "\n").c_str());
      return ECMD_INVALID_ARGS;
    }

    latchInfo.sort();

    /* Let's make sure the latches we found all have the same name */
    /* We might also want to verify that we found consecutive bits for the latch */
    std::string curLatchName;
    for (curLatchInfo = latchInfo.begin(); curLatchInfo != latchInfo.end(); curLatchInfo++) {
      if (curLatchInfo == latchInfo.begin())
        curLatchName = curLatchInfo->latchName;
      else if (curLatchName != curLatchInfo->latchName) {
        ecmdOutputError("putlatch - Found multiple latches that match name provided, please provide additional latch name - unable to perform putlatch\n");
        return ECMD_INVALID_ARGS;
      }
    }

    buffer.copy(bufferCopy);



    int curstartbit = 0;
    int curoffset = 0;
    int totallen = 0, insertlen;
    bool foundit = false;
    for (curLatchInfo = latchInfo.begin(); curLatchInfo != latchInfo.end(); curLatchInfo++) {

      /* Let's look for the beginning of the range we are looking for */
      if (!foundit && (startBit < curstartbit + curLatchInfo->length)) {
        curoffset = startBit - curstartbit;
        foundit = true;
        insertlen = curLatchInfo->length - curoffset;
      } else if (foundit) {
        curoffset = 0;
        insertlen = curLatchInfo->length;
      }
      if (foundit) {
        insertlen = totallen + insertlen > numBits ? numBits - totallen : insertlen;

        ringBuffer.insert(bufferCopy, curLatchInfo->ringOffset + curoffset, insertlen);
        bufferCopy.shiftLeft(insertlen);

        totallen += insertlen;

        /* We are done, let's get out of here */
        if (totallen >= numBits) break;
      }
    }

    rc = putRing(target, ringName.c_str(), ringBuffer);
    if (rc) {
        printed = "putlatch - Error occured performing putring on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    
  }

  if (!validPosFound) {
    ecmdOutputError("putlatch - Unable to find a valid chip to execute command on\n");
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
    ecmdOutputError("checkrings - Too few arguments specified; you need at least a chip and a ring.\n");
    ecmdOutputError("checkrings - Type 'checkrings -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  if (argc > 2) {
    ecmdOutputError("checkrings - Too many arguments specified; you probably added an unsupported option.\n");
    ecmdOutputError("checkrings - Type 'checkrings -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }


  //Setup the target that will be used to query the system config 
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  std::string ringName = argv[1];

  if (ringName == "all")
    allRingsFlag = true;

  
  ecmdDataBuffer ringBuffer; 
  std::string printed;
  char outstr[200];
  uint32_t pattern0 = 0xAAAA0000;
  uint32_t pattern1 = 0x5555FFFF;
  uint32_t pattern = 0x0;

  bool validPosFound = false;
  bool printedTarget;
  rc = ecmdConfigLooperInit(target);
  if (rc) return rc;

  std::list<ecmdRingData> queryRingData;

  while (ecmdConfigLooperNext(target)) {
    printedTarget = false;

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
          printed = "checkrings - Error occured performing putring on ";
          printed += ecmdWriteTarget(target) + "\n";
          ecmdOutputError( printed.c_str() );
          return rc;
        }
        else {
          validPosFound = true;
        }

        /* Print out the current target */
        if (!printedTarget) {
          printedTarget = true;
          printed = ecmdWriteTarget(target) + "\n"; ecmdOutput(printed.c_str());
        }

        if (i % 2) {
          printed = "Performing 0's test on " + ringName + " ...\n";
          ecmdOutput(printed.c_str());
        }
        else {
          printed = "Performing 1's test on " + ringName + " ...\n";
          ecmdOutput(printed.c_str());
        }


        rc = getRing(target, ringName.c_str(), ringBuffer);
        if (rc) {
          printed = "checkrings - Error occured performing getring on ";
          printed += ecmdWriteTarget(target) + "\n";
          ecmdOutputError( printed.c_str() );
          return rc;
        }

        if (ringBuffer.getWord(0) != pattern) {
          sprintf(outstr, "checkrings - Data fetched from ring %s did not match Pattern: %.08X Data: %.08X\n", ringName.c_str(), pattern, ringBuffer.getWord(0));
          ecmdOutputWarning( outstr );
          printed = "checkrings - Error occured performing checkring on " + ecmdWriteTarget(target) + "\n";
          ecmdOutputWarning( printed.c_str() );
        }
        else {
          /* Walk the ring looking for errors */
          for (int bit = 32; bit < ringBuffer.getBitLength(); bit ++ ) {
            if (i % 2) {
              if (ringBuffer.isBitSet(bit)) {
                printed += "checkrings - Non-one bits found in 1's ring test at bit " + bit;
                printed += " for ring " + ringName;
                printed += "\n";
                ecmdOutputWarning( printed.c_str() );
                printed = "checkrings - Error occured performing checkring on " + ecmdWriteTarget(target) + "\n";
                ecmdOutputWarning( printed.c_str() );
              }
            } else {
              if (ringBuffer.isBitClear(bit)) {
                printed += "checkrings - Non-zero bits found in 0's ring test at bit " + bit;
                printed += " for ring " + ringName;
                printed += "\n";
                ecmdOutputWarning( printed.c_str() );
                printed = "checkrings - Error occured performing checkring on " + ecmdWriteTarget(target) + "\n";
                ecmdOutputWarning( printed.c_str() );
              }
            }
          }
        }

      }

      curRingData++;
    }
    


  }
  
  if (!validPosFound) {
    ecmdOutputError("checkrings - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}


int ecmdPutPatternUser(int argc, char * argv[]) {
  int rc = ECMD_SUCCESS;

  std::list<ecmdRingData> queryRingData;
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
    ecmdOutputError("putpattern - Too few arguments specified; you need at least a chip, ring, and pattern.\n");
    ecmdOutputError("putpattern - Type 'putpattern -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  if (argc > 3) {
    ecmdOutputError("putpattern - Too many arguments specified; you probably added an unsupported option.\n");
    ecmdOutputError("putpattern - Type 'putpattern -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }


  //Setup the target that will be used to query the system config 
  ecmdChipTarget target;
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  std::string ringName = argv[1];

  ecmdDataBuffer buffer;
  rc = ecmdReadDataFormatted(buffer, argv[2], format);
  if (rc) return rc;

  ecmdDataBuffer ringBuffer;
  std::string printed;

  bool validPosFound = false;
  rc = ecmdConfigLooperInit(target);
  if (rc) return rc;

  while (ecmdConfigLooperNext(target)) {

    rc = ecmdQueryRing(target, queryRingData, ringName.c_str());
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
      printed = "putpattern - Error occured retrieving scan ring data on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    } else if (queryRingData.empty()) {
      printed = "putpattern - Unable to lookup data on ring " + ringName + ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return ECMD_INVALID_ARGS;

    } else {
      validPosFound = true;     
    }

    uint32_t curOffset = 0;
    uint32_t numBitsToInsert = 0;
    uint32_t numBitsInRing = queryRingData.front().bitLength;
    ringBuffer.setBitLength(numBitsInRing);
    while (curOffset < numBitsInRing) {
      numBitsToInsert = (32 < numBitsInRing - curOffset) ? 32 : numBitsInRing - curOffset;
      ringBuffer.insert(buffer, curOffset, numBitsToInsert);
      curOffset += numBitsToInsert;
    }

    rc = putRing(target, ringName.c_str(), ringBuffer);
    if (rc) {
        printed = "putpattern - Error occured performing putring on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }

  }

  if (!validPosFound) {
    ecmdOutputError("putpattern - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}



