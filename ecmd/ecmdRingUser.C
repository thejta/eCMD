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
/* $Header$ */

// Module Description **************************************************
//
// Description: 
//
// End Module Description **********************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define ecmdRingUser_C
#include <stdio.h>
#include <time.h>
#include <fstream>
#include <vector>
#include <algorithm>


#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <ecmdInterpreter.H>

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


/** @brief Used to sort latch entries from the scandef */
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

/** @brief Used to sort latch entries from the scandef */
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

uint32_t ecmdGetRingDumpUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  time_t curTime = time(NULL);
  bool newFileFormat = false;           ///< This is set if we find the new Eclipz scandef format 
  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string ringName;                 ///< Ring name being worked on
  ecmdDataBuffer ringBuffer;            ///< Buffer to store entire ring contents
  ecmdDataBuffer buffer;                ///< Buffer to extract individual latch contents
  ecmdChipTarget target;                ///< Current target being operated on
  bool validPosFound = false;           ///< Did the looper find something ?
  std::string format = "default";       ///< Output format
  std::ifstream ins;                    ///< File stream


  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr != NULL) {
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
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;


  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  std::string printed;
  std::string curLine;
  std::string ringPrefix = "ring=";
  char outstr[30];

  while ( ecmdConfigLooperNext(target, looperdata) ) {

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

          if (!newFileFormat && curLine[0] == '*' && curLine.find(ringPrefix) != std::string::npos) {
            done = true;
          }
          else if (newFileFormat && curLine[0] == 'E' && curLine.find("END") != std::string::npos) {
            done = true;
          }
          else if (curLine.length() == 0 || curLine[0] == '\0' || curLine[0] == '*' || curLine[0] == '#') {
            //do nothing
          }
          else if (newFileFormat && (curLine[0] != ' ') && (curLine[0] != '\t')) {
            // do nothing
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
        /* Can we find the ring we are looking for on this line */
        else if (((curLine[0] == '*') && (curLine.find(ringArg) != std::string::npos)) ||
                 ((curLine[0] == 'N') && (curLine.find(ringName) != std::string::npos))) {
          found = true;
          if (curLine.substr(0,4) == "Name") {
            newFileFormat = true;
          }
        }

      }

      if (!found) {
        printed = "getringdump - Unable to find ring '";
        printed += ringName;
        printed += "' in the scandef\n";
        ecmdOutputError(printed.c_str());
        printed = "getringdump - Scandef Used : ";
        printed += scandefFile;
        printed += "\n";
        ecmdOutputError(printed.c_str());
        return ECMD_SCANDEF_LOOKUP_FAILURE;
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


uint32_t ecmdGetLatchUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  bool expectFlag = false;
  bool exactFlag = false;
  bool noCompressFlag = false;
  bool foundit;
  std::list<ecmdLatchBufferEntry> latchBuffer;
  std::list<ecmdLatchBufferEntry>::iterator bufferit;
  ecmdLatchBufferEntry curEntry;
  char* expectDataPtr = NULL;
  bool newFileFormat = false;                   /* This is set if we find the new Eclipz scandef format */
  ecmdLooperData looperdata;                    ///< Store internal Looper data
  std::string outputformat = "default";         ///< Output Format to display
  std::string inputformat = "x";                ///< Input format of data
  ecmdDataBuffer expected;                      ///< Buffer to store output data
  ecmdChipTarget target;                        ///< Target we are operating on
  std::string printed;
  std::list< ecmdLatchInfo >::iterator curLatchInfo;    ///< Iterator for walking through latches
  std::string scandefFile;                      ///< Full path to scandef file
  ecmdDataBuffer ringBuffer;                    ///< Buffer to store entire ring
  ecmdDataBuffer buffer(100 /* words */);       ///< Space for extracted latch data
  ecmdDataBuffer buffertemp(100 /* words */);   ///< Temp space for extracted latch data

  bool validPosFound = false;

  if ((expectDataPtr = ecmdParseOptionWithArgs(&argc, &argv, "-exp")) != NULL) {
    expectFlag = true;
  }

  if (ecmdParseOption(&argc, &argv, "-exact")) {
    exactFlag = true;
  }

  if (ecmdParseOption(&argc, &argv, "-nocompress")) {
    noCompressFlag = true;
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


  if (expectFlag) {
    /* Grab the data for the expect */
    rc = ecmdReadDataFormatted(expected, expectDataPtr, inputformat);
    if (rc) {
      ecmdOutputError("getscom - Problems occurred parsing expected data, must be an invalid format\n");
      return rc;
    }
  }

  if (expectFlag && noCompressFlag) {
    ecmdOutputError("getlatch - Using -exp and -nocompress is not allowed\n");
    return ECMD_INVALID_ARGS;
  }

  if (argc < 3) {  //chip + ring + latch
    ecmdOutputError("getlatch - Too few arguments specified; you need at least a chip, ring, and latch name.\n");
    ecmdOutputError("getlatch - Type 'getlatch -h' for usage.");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  std::string ringName = argv[1];
  std::string latchName = argv[2];
  /* Transform to upper case for case-insensitive comparisons */
  transform(latchName.begin(), latchName.end(), latchName.begin(), toupper);

  uint32_t startBit = 0x0, numBits = 0x0FFFFFFF;

  if (argc > 3) {

    /* We don't allow start and numbits with the -nocompress */
    if (noCompressFlag) {
      ecmdOutputError("getlatch - Using startbit and numbits is not allowed in combination with the -nocompressflag\n");
      return ECMD_INVALID_ARGS;
    }

    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("getlatch - Non-decimal numbers detected in startbit field\n");
      return ECMD_INVALID_ARGS;
    }
    startBit = atoi(argv[3]);
    if (!strcmp(argv[4], "end")) {
      numBits = 0x0FFFFFFF;
    }
    else {
      if (!ecmdIsAllDecimal(argv[4])) {
        ecmdOutputError("getlatch - Non-decimal numbers detected in numbits field\n");
        return ECMD_INVALID_ARGS;
      }
      numBits = atoi(argv[4]);
    }

    /* Bounds check */
    if ((startBit + numBits) > ECMD_MAX_DATA_BITS) {
      char errbuf[100];
      sprintf(errbuf,"getlatch - Too much data requested > %d bits\n", ECMD_MAX_DATA_BITS);
      ecmdOutputError(errbuf);
      return ECMD_DATA_BOUNDS_OVERFLOW;
    }

  }


  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;


  while ( ecmdConfigLooperNext(target, looperdata) ) {

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

          if (!newFileFormat && curLine[0] == '*' && curLine.find(ringPrefix) != std::string::npos) {
            done = true; continue;
          }
          else if (newFileFormat && curLine[0] == 'E' && curLine.find("END") != std::string::npos) {
            done = true; continue;
          }
          else if (curLine.length() == 0 || curLine[0] == '\0' || curLine[0] == '*' || curLine[0] == '#') {
            //do nothing
            continue;
          }
          else if (newFileFormat && (curLine[0] != ' ') && (curLine[0] != '\t')) {
            // do nothing
            continue;
          }
          else if (!exactFlag && (curLine.find(latchName) != std::string::npos)) {

            /* Transform to upper case */
            transform(curLine.begin(), curLine.end(), curLine.begin(), toupper);

            ecmdParseTokens(curLine, curArgs);
            curLatch.length = atoi(curArgs[0].c_str());
            curLatch.ringOffset = atoi(curArgs[1].c_str());
            curLatch.latchName = curArgs[3];
          }
          else if (exactFlag) {

            /* Transform to upper case */
            transform(curLine.begin(), curLine.end(), curLine.begin(), toupper);
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
        else if (((curLine[0] == '*') && (curLine.find(ringArg) != std::string::npos)) ||
                 ((curLine[0] == 'N') && (curLine.find(ringName) != std::string::npos))) {
          found = true;
          if (curLine.substr(0,4) == "Name") {
            newFileFormat = true;
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

      /* Let's push this entry onto the stack */
      latchBuffer.push_back(curEntry);
    
    }

    printed = ecmdWriteTarget(target) + "\n";
    ecmdOutput(printed.c_str());
    uint32_t bitsToFetch = numBits;
    int curLatchBit = -1;               // This is the actual latch bit we are looking for next
    int curStartBit = startBit;         // This is the offset into the current entry to start extraction
    int curBufferBit = 0;               // Current bit to insert into buffered register
    int curBitsToFetch = numBits;       // This is the total number of bits left to fetch
    int dataStartBit = -1;              // Actual start bit of buffered register
    int dataEndBit = -1;                // Actual end bit of buffered register
    std::string latchname = "";

    for (curLatchInfo = curEntry.entry.begin(); (curLatchInfo != curEntry.entry.end()) && (curBitsToFetch > 0); curLatchInfo++) {

      if (noCompressFlag) {
        bitsToFetch = ((*curLatchInfo).length < curBitsToFetch) ? (*curLatchInfo).length : curBitsToFetch;
        ringBuffer.extract(buffer, (*curLatchInfo).ringOffset, bitsToFetch);

        printed = curLatchInfo->latchName + " ";
        if (outputformat == "default") {

          if (bitsToFetch <= 8) {
            printed += " 0b" + buffer.genBinStr();
          }
          else {
            printed += " 0x" + buffer.genHexLeftStr();
          }

          printed += "\n";

        }
        else {
          printed += ecmdWriteDataFormatted(buffer, outputformat);
        }
        ecmdOutput( printed.c_str());
        curBitsToFetch -= bitsToFetch;


        /**** NON-Compressed output ****/
      } else {


        /* Do we have previous data here , or some missing bits in the scandef latchs ?*/
        if (((dataStartBit != -1) && (curLatchBit != curLatchInfo->latchStartBit) && (curLatchBit != curLatchInfo->latchEndBit)) ||
            ((latchname == "") || (latchname.substr(0, latchname.find('(')) != curLatchInfo->latchName.substr(0, curLatchInfo->latchName.find('('))))) {
          /* I have some good data here */
          if (latchname != "") {

                            
            /* Display this if we aren't expecting or the expect failed */
            if (!expectFlag || (expectFlag && !ecmdCheckExpected(expected, buffer))) {
              char temp[20];
              printed =  latchname;
              sprintf(temp,"(%d:%d)", dataStartBit, dataEndBit);
              printed += temp;
              buffer.extract(buffertemp, 0, dataEndBit - dataStartBit + 1);
              if (outputformat == "default") {
                if (buffertemp.getBitLength() <= 8)  printed += " 0b" + buffertemp.genBinStr();
                else printed += " 0x" + buffertemp.genHexLeftStr();
                printed += "\n";
              } else {
                printed += ecmdWriteDataFormatted(buffertemp, outputformat);
              }
              ecmdOutput( printed.c_str());

              /* They did an expect and we must have failed */
              if (expectFlag) {
                ecmdOutputError("getlatch - Data miscompare found\n");
                return ECMD_EXPECT_FAILURE;
              }
            }
          }
          /* If this is a fresh one we need to reset everything */
          if ((latchname == "") || (latchname != curLatchInfo->latchName.substr(0, latchname.length()))) {
            dataStartBit = dataEndBit = -1;
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

        /* Do we want anything in here */
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
      }
    }

    /* We need to dump any data we have stored here */
    if (!noCompressFlag) {
      char temp[20];
      /* Do we have previous data here , and some missing bits in the scandef latchs ?*/
      if ((curLatchBit != -1)) {
        /* Display this if we aren't expecting or the expect failed */
        if (!expectFlag || (expectFlag && !ecmdCheckExpected(expected, buffer))) {
          printed =  latchname;
          sprintf(temp,"(%d:%d)", dataStartBit, dataEndBit);
          printed += temp;
          buffer.extract(buffertemp, 0, dataEndBit - dataStartBit + 1);
          if (outputformat == "default") {
            if (buffertemp.getBitLength() <= 8)  printed += " 0b" + buffertemp.genBinStr();
            else printed += " 0x" + buffertemp.genHexLeftStr();
            printed += "\n";
          } else {
            printed += ecmdWriteDataFormatted(buffertemp, outputformat);
          }
          ecmdOutput( printed.c_str());
          /* They did an expect and we must have failed */
          if (expectFlag) {
            ecmdOutputError("getlatch - Data miscompare found\n");
            return ECMD_EXPECT_FAILURE;
          }

        }
      }
    }
  }


  if (!validPosFound) {
    ecmdOutputError("getlatch - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdGetBitsUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  bool expectFlag = false;
  char* expectDataPtr = NULL;
  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string outputformat = "b";       ///< Output Format to display
  std::string inputformat = "b";        ///< Input format of data
  ecmdChipTarget target;                ///< Current target operating on
  std::string ringName;                 ///< Ring to fetch
  uint32_t startBit;                    ///< Start bit to fetch
  uint32_t numBits;                     ///< Number of bits to fetch
  ecmdDataBuffer ringBuffer;            ///< Buffer for entire ring
  ecmdDataBuffer buffer;                ///< Buffer for portion user requested
  ecmdDataBuffer expected;              ///< Buffer to store expected data
  bool validPosFound = false;           ///< Did the looper find anything to execute on

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  //expect and mask flags check
  if ((expectDataPtr = ecmdParseOptionWithArgs(&argc, &argv, "-exp")) != NULL) {
    expectFlag = true;
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
  if (argc < 4) {  //chip + address
    ecmdOutputError("getbits - Too few arguments specified; you need at least a chip, ring, startbit, and numbits.\n");
    ecmdOutputError("getbits - Type 'getbits -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  ringName = argv[1];

  if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("getbits - Non-decimal numbers detected in startbit field\n");
    return ECMD_INVALID_ARGS;
  }
  startBit = atoi(argv[2]);

  if (!strcmp(argv[3], "end")) {
    numBits = 0xFFFFFFFF;
  }
  else {
    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("getbits - Non-decimal numbers detected in numbits field\n");
      return ECMD_INVALID_ARGS;
    }
    numBits = atoi(argv[3]);


    /* Bounds check */
    if ((startBit + numBits) > ECMD_MAX_DATA_BITS) {
      char errbuf[100];
      sprintf(errbuf,"getbits - Too much data requested > %d bits\n", ECMD_MAX_DATA_BITS);
      ecmdOutputError(errbuf);
      return ECMD_DATA_BOUNDS_OVERFLOW;
    }
  }


  if (expectFlag) {
    rc = ecmdReadDataFormatted(expected, expectDataPtr, inputformat);
    if (rc) {
      ecmdOutputError("getbits - Problems occurred parsing expected data, must be an invalid format\n");
      return rc;
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

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  char outstr[30];
  std::string printed;

  while ( ecmdConfigLooperNext(target, looperdata) ) {

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

      if (!ecmdCheckExpected(buffer, expected)) {

        //@ make this stuff sprintf'd
        printed = ecmdWriteTarget(target) + "  " + ringName;
        sprintf(outstr, "(%d:%d)\n", startBit, startBit + numBits - 1);
        printed += outstr;
        ecmdOutputError( printed.c_str() );
        printed =  "Actual            : ";
        printed += ecmdWriteDataFormatted(buffer, outputformat);
        ecmdOutputError( printed.c_str() );

        printed = "Expected          : ";
        printed += ecmdWriteDataFormatted(expected, outputformat);
        ecmdOutputError( printed.c_str() );
        return ECMD_EXPECT_FAILURE;
      }

    }
    else {
      printed = ecmdWriteTarget(target) + "  " + ringName;
      sprintf(outstr, "(%d:%d)", startBit, startBit + numBits - 1);
      printed += outstr;
      std::string dataStr = ecmdWriteDataFormatted(buffer, outputformat);
      if (dataStr[0] != '\n') {
        printed += "\n";
      }
      printed += dataStr;
      ecmdOutput( printed.c_str() );
    }

  }

  if (!validPosFound) {
    ecmdOutputError("getbits - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdPutBitsUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string format = "b";             ///< Input format
  std::string dataModifier = "insert";          ///< Default data Modifier (And/Or/insert)
  ecmdChipTarget target;                ///< Current target being operated on
  ecmdDataBuffer ringBuffer;            ///< Buffer to store entire ring
  ecmdDataBuffer buffer;                ///< Buffer to store data insert data
  bool validPosFound = false;           ///< Did the looper find something ?

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  //expect and mask flags check

  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    format = formatPtr;
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
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get ring name and starting position
  std::string ringName = argv[1];


  if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("putbits - Non-decimal numbers detected in startbit field\n");
    return ECMD_INVALID_ARGS;
  }
  int startBit = atoi(argv[2]);
  
  //container to store data

  rc = ecmdReadDataFormatted(buffer, argv[3], format);
  if (rc) {
    ecmdOutputError("putbits - Problems occurred parsing input data, must be an invalid format\n");
    return rc;
  }

  
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  std::string printed;

  while ( ecmdConfigLooperNext(target, looperdata) ) {

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

    
    rc = ecmdApplyDataModifier(ringBuffer, buffer, startBit, dataModifier);
    if (rc) return rc;

    rc = putRing(target, ringName.c_str(), ringBuffer);
    if (rc) {
        printed = "putbits - Error occured performing putring on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }

    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      printed = ecmdWriteTarget(target) + "\n";
      ecmdOutput(printed.c_str());
    }


  }

  if (!validPosFound) {
    ecmdOutputError("putbits - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdPutLatchUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  bool foundit;
  std::list<ecmdLatchBufferEntry> latchBuffer;
  std::list<ecmdLatchBufferEntry>::iterator bufferit;
  ecmdLatchBufferEntry curEntry;
  bool newFileFormat = false;           ///< This is set if we find the new Eclipz scandef format 
  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string format = "x";             ///< Output format
  ecmdChipTarget target;                ///< Current target being operated on
  ecmdDataBuffer ringBuffer;            ///< Buffer to store entire ring
  ecmdDataBuffer bufferCopy;            ///< Copy of data to be inserted
  ecmdDataBuffer buffer;                ///< Master copy of data to be inserted
  bool validPosFound = false;           ///< Did the looper find anything ?
  std::string printed;
  std::list< ecmdLatchInfo >::iterator curLatchInfo;
  std::string scandefFile;              ///< Full path to scandef file

  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    format = formatPtr;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  std::string ringName = argv[1];
  std::string latchName = argv[2];


  //data is always the last arg
  rc = ecmdReadDataFormatted(buffer, argv[argc-1], format);
  if (rc) {
    ecmdOutputError("putlatch - Problems occurred parsing input data, must be an invalid format\n");
    return rc;
  }

  int startBit = 0, numBits = -1;


  if (argc > 4) {
    if (argc < 6) {
      ecmdOutputError("putlatch - Too few arguments specified; you need at least a chip, ring, latch name, startbit, endbit, and data.\n");
      ecmdOutputError("putlatch - Type 'putlatch -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    } else if (argc > 6) {
      ecmdOutputError("putlatch - Too many arguments specified; you probably added an unsupported option.\n");
      ecmdOutputError("putlatch - Type 'putlatch -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }

    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("putlatch - Non-decimal numbers detected in startbit field\n");
      return ECMD_INVALID_ARGS;
    }
    startBit = atoi(argv[3]);
    if (!ecmdIsAllDecimal(argv[4])) {
      ecmdOutputError("putlatch - Non-decimal numbers detected in numbits field\n");
      return ECMD_INVALID_ARGS;
    }
    numBits = atoi(argv[4]);
  }
  else {
    if (argc < 4) {
      ecmdOutputError("putlatch - Too few arguments specified; you need at least a chip, ring, latch name, and data.\n");
      ecmdOutputError("putlatch - Type 'putlatch -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }

    numBits = buffer.getBitLength();
  }

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;


  while ( ecmdConfigLooperNext(target, looperdata) ) {

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
    rc = ecmdQueryFileLocation(target, ECMD_FILE_SCANDEF, scandefFile);
    if (rc) {
      ecmdOutputError(("putlatch - Error occured locating scandef file: " + scandefFile + "\n").c_str());
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

    if (!foundit) {
      std::ifstream ins(scandefFile.c_str());
      if (ins.fail()) {
        ecmdOutputError(("putlatch - Error occured opening scandef file: " + scandefFile + "\n").c_str());
        return ECMD_INVALID_ARGS;  //change this
      }

      //let's go hunting in the scandef for this register (pattern)
      ecmdLatchInfo curLatch;
      ecmdLatchInfo tmpLatch;

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
          if (!newFileFormat && curLine[0] == '*' && curLine.find(ringPrefix) != std::string::npos) {
            done = true; continue;
          }
          else if (newFileFormat && curLine[0] == 'E' && curLine.find("END") != std::string::npos) {
            done = true; continue;
          }
          else if (curLine.length() == 0 || curLine[0] == '\0' || curLine[0] == '*' || curLine[0] == '#') {
            //do nothing
            continue;
          }
          else if (newFileFormat && (curLine[0] != ' ') && (curLine[0] != '\t')) {
            // do nothing
            continue;
          }
          else if ((curLine[0] != '*') && curLine.find(latchName) != std::string::npos) {

            ecmdParseTokens(curLine, curArgs);
            curLatch.length = atoi(curArgs[0].c_str());
            curLatch.ringOffset = atoi(curArgs[1].c_str());
            curLatch.latchName = curArgs[3];
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
        else if (((curLine[0] == '*') && (curLine.find(ringArg) != std::string::npos)) ||
                 ((curLine[0] == 'N') && (curLine.find(ringName) != std::string::npos))) {
          found = true;
          if (curLine.substr(0,4) == "Name") {
            newFileFormat = true;
          }
        }

      }

      ins.close();

      if (!found) {
        ecmdOutputError(("putlatch - Could not find ring name " + ringName + "\n").c_str());
        return ECMD_INVALID_ARGS;
      }

      if (curEntry.entry.empty()) {
        ecmdOutputError(("putlatch - No registers found that matched " + latchName + "\n").c_str());
        return ECMD_INVALID_ARGS;
      }

      curEntry.scandefname = scandefFile;
      curEntry.entry.sort();

      /* Let's push this entry onto the stack */
      latchBuffer.push_back(curEntry);

      /* Let's make sure the latches we found all have the same name */
      /* We might also want to verify that we found consecutive bits for the latch */
      std::string curLatchName;
      for (curLatchInfo = curEntry.entry.begin(); curLatchInfo != curEntry.entry.end(); curLatchInfo++) {
        if (curLatchInfo == curEntry.entry.begin())
          curLatchName = curLatchInfo->latchName;
        else if (curLatchName.substr(0, curLatchName.find('(')) != curLatchInfo->latchName.substr(0, curLatchInfo->latchName.find('(')) ) {
          ecmdOutputError("putlatch - Found multiple latches that match name provided, please provide additional latch name - unable to perform putlatch\n");
          return ECMD_INVALID_ARGS;
        }
      }
    } /* End !found in latch buffer */

    buffer.copy(bufferCopy);


    uint32_t bitsToInsert = numBits;
    int curLatchBit = -1;               // This is the actual latch bit we are looking for next
    int curStartBit = startBit;         // This is the offset into the current entry to start extraction
    int curBitsToInsert = numBits;      // This is the total number of bits left to Insert

    for (curLatchInfo = curEntry.entry.begin(); curLatchInfo != curEntry.entry.end(); curLatchInfo++) {


      if (curLatchBit == -1)
        curLatchBit = curLatchInfo->latchStartBit < curLatchInfo->latchEndBit ? curLatchInfo->latchStartBit : curLatchInfo->latchEndBit;

      /* Check if the bits are ordered from:to (0:10) or just (1) */
      if (((curLatchInfo->latchEndBit >= curLatchInfo->latchStartBit) && (curStartBit <= curLatchInfo->latchEndBit) && (curLatchBit <= curLatchInfo->latchEndBit)) ||
          /* Check if the bits are ordered to:from (10:0) */
          ((curLatchInfo->latchStartBit > curLatchInfo->latchEndBit) && (curStartBit <= curLatchInfo->latchStartBit) && (curLatchBit <= curLatchInfo->latchStartBit))) {

        bitsToInsert = ((curLatchInfo->length - curStartBit) < curBitsToInsert) ? curLatchInfo->length - curStartBit : curBitsToInsert;


        /* Extract bits if ordered from:to (0:10) */
        if (curLatchInfo->latchEndBit > curLatchInfo->latchStartBit) {

          ringBuffer.insert(bufferCopy, curLatchInfo->ringOffset + curStartBit, bitsToInsert);
          /* Get rid of the data we just inserted, to line up the next piece */
          bufferCopy.shiftLeft(bitsToInsert);

          curLatchBit = curLatchInfo->latchEndBit + 1;
        } else {
          /* Extract if bits are ordered to:from (10:0) or just (1) */
          for (int bit = 0; bit < bitsToInsert; bit ++) {
            if (bufferCopy.isBitSet(bit)) 
              ringBuffer.setBit(curLatchInfo->ringOffset + (curLatchInfo->length - 1) - curStartBit - bit);
            else
              ringBuffer.clearBit(curLatchInfo->ringOffset + (curLatchInfo->length - 1) - curStartBit - bit);
          }
          /* Get rid of the data we just inserted, to line up the next piece */
          bufferCopy.shiftLeft(bitsToInsert);

          curLatchBit = curLatchInfo->latchStartBit + 1;

        }

        curStartBit = 0;
        curBitsToInsert -= bitsToInsert;
      } else {
        /* Nothing was there that we needed, let's try the next entry */
        curLatchBit = curLatchInfo->latchStartBit < curLatchInfo->latchEndBit ? curLatchInfo->latchEndBit + 1: curLatchInfo->latchStartBit + 1;

        curStartBit -= curLatchInfo->length;
      }

    }

    rc = putRing(target, ringName.c_str(), ringBuffer);
    if (rc) {
        printed = "putlatch - Error occured performing putring on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }

    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      printed = ecmdWriteTarget(target) + "\n";
      ecmdOutput(printed.c_str());
    }
    
  }

  if (!validPosFound) {
    ecmdOutputError("putlatch - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdCheckRingsUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  bool allRingsFlag = false;
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdChipTarget target;                ///< Current target being operated on
  ecmdDataBuffer ringBuffer;            ///< Buffer to store ring
  std::list<ecmdRingData> queryRingData;///< Ring data 
  bool validPosFound = false;           ///< Did the looper find anything ?
  bool printedTarget;                   ///< Have we printed the target out yet?

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
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  std::string ringName = argv[1];

  if (ringName == "all")
    allRingsFlag = true;

  
  std::string printed;
  char outstr[200];
  uint32_t pattern0 = 0xAAAA0000;
  uint32_t pattern1 = 0x5555FFFF;
  uint32_t pattern = 0x0;

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;


  while (ecmdConfigLooperNext(target, looperdata)) {
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


uint32_t ecmdPutPatternUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  std::list<ecmdRingData> queryRingData;///< Ring query data
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdChipTarget target;                ///< Current target being operated on
  ecmdDataBuffer ringBuffer;            ///< Buffer to store entire ring
  std::string printed;                  ///< Output print data
  bool validPosFound = false;           ///< Did the looper find anything?
  ecmdDataBuffer buffer;                ///< Buffer to store pattern
  std::string format = "xr";            ///< Default input format

  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
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
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  std::string ringName = argv[1];

  rc = ecmdReadDataFormatted(buffer, argv[2], format, 32);
  if (rc) {
    ecmdOutputError("putpattern - Problems occurred parsing input data, must be an invalid format\n");
    return rc;
  }

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdConfigLooperNext(target, looperdata)) {

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

    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      printed = ecmdWriteTarget(target) + "\n";
      ecmdOutput(printed.c_str());
    }

  }

  if (!validPosFound) {
    ecmdOutputError("putpattern - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}



