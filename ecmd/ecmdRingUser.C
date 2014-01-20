/* $Header$ */
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

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <stdio.h>
#include <time.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <netinet/in.h> /* for htonl */

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

#ifndef ECMD_REMOVE_RING_FUNCTIONS
struct ecmdCheckRingData {
  std::string ringName;                 ///< Name of ring
  std::string chipUnitType;                 ///< Name of ring
  int chipUnitNum;                         ///< chipUnit value -1 if not a chipUnit ring
};
#endif // ECMD_REMOVE_RING_FUNCTIONS

//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Macros
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Internal Function Prototypes
//----------------------------------------------------------------------
#ifndef ECMD_REMOVE_LATCH_FUNCTIONS
uint32_t readScandefFile(ecmdChipTarget & target, const char* i_ringName, ecmdDataBuffer &ringBuffer, std::list< ecmdLatchDataEntry > & o_latchdata);
void printLatchInfo( std::string latchname, ecmdDataBuffer buffer, uint32_t dataStartBit, uint32_t dataEndBit, std::string format, ecmdLatchType_t isMultiBitLatch);

/** @brief Used to sort latch entries from the scandef */
bool operator< (const ecmdLatchDataEntry & lhs, const ecmdLatchDataEntry & rhs) {


  if (lhs.ringName != rhs.ringName)
    return lhs.ringName < rhs.ringName;

  size_t lhsLeftParen = lhs.latchName.find_last_of('(');
  size_t rhsLeftParen = rhs.latchName.find_last_of('(');

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
bool operator!= (const ecmdLatchDataEntry & lhs, const ecmdLatchDataEntry & rhs) {

  if (lhs.ringName != rhs.ringName)
    return true;

  size_t lhsLeftParen = lhs.latchName.find_last_of('(');
  size_t rhsLeftParen = rhs.latchName.find_last_of('(');

  if (lhsLeftParen != rhsLeftParen) {
    return true;
  }

  return (lhs.latchName.substr(0, lhsLeftParen) != rhs.latchName.substr(0,rhsLeftParen));
}
#endif // ECMD_REMOVE_LATCH_FUNCTIONS



//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------
#ifndef ECMD_REMOVE_LATCH_FUNCTIONS
uint32_t ecmdGetRingDumpUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;
  time_t curTime = time(NULL);
  ecmdLooperData looperData;            ///< Store internal Looper data
  ecmdLooperData cuLooper;              ///< Store internal Looper data for the chipUnit loop
  ecmdChipTarget target;                ///< Current target being operated on
  ecmdChipTarget cuTarget;              ///< Current target being operated on for the chipUnit
  bool validPosFound = false;           ///< Did the looper find something ?
  std::string format = "default";       ///< Output format
  ecmdChipData chipData;                ///< Chip data to find out bus info
  uint32_t bustype;                     ///< Stores if the chip was JTAG or FSI attached 
  bool unsort=false;			///< Sort the latchnames before printing them out
  std::list<ecmdRingData> queryRingData;        ///< Ring data 
  std::list<ecmdRingData>::iterator ringData;   ///< Ring data from query
  std::string printed;                  ///< Output data
  char outstr[1000];                    ///< Output string
  ecmdDataBuffer ringBuffer;            ///< Buffer to store entire ring contents
  ecmdDataBuffer buffer;                ///< Buffer to extract individual latch contents
  ecmdDataBuffer buffertemp(500 /* bits */);   ///< Temp space for extracted latch data
  uint8_t oneLoop = 0;                         ///< Used to break out of the chipUnit loop after the first pass for non chipUnit operations

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  unsort = ecmdParseOption(&argc, &argv, "-unsorted");
  
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr != NULL) {
    format = formatPtr;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  if (argc < 2) {
    ecmdOutputError("getringdump - Too few arguments specified; you need at least a chip and a ring name.\n");
    ecmdOutputError("getringdump - Type 'getring -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
     
  //Loop over the rings
  for (int i = 1; i < argc; i++) {

    std::string ringName = argv[i];
    
    transform(ringName.begin(), ringName.end(), ringName.begin(), (int(*)(int)) tolower);
	
    //Setup the target that will be used to query the system config
    std::string chipType, chipUnitType;
    rc = ecmdParseChipField(argv[0], chipType, chipUnitType);
    if (rc) { 
      ecmdOutputError("getringdump - eCMD wildcard character detected however it is not supported by this command.\n");
      return rc;
    }
    target.chipType = chipType;
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
    target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

    /************************************************************************/
    /* Kickoff Looping Stuff						     */
    /************************************************************************/
    rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
    if (rc) return rc;

    while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {

      /* We need to find out if this chip is JTAG or FSI attached to handle the scandef properly */
      /* Do this on a side copy so we don't mess up the looper states */
      ecmdChipTarget dtarget = target;
      rc = ecmdGetChipData(dtarget, chipData);
      if (rc) {
        printed = "getringdump - Problems retrieving chip information on : ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
      } 
      /* Now make sure the plugin gave us some bus info */
      if (!(chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK)) {
        printed = "getringdump - eCMD plugin did not implement ecmdChipData.chipFlags unable to determine if FSI or JTAG attached chip\n";
        ecmdOutputError( printed.c_str() );
        rc = ECMD_DLL_INVALID;
        break;
      } else if (((chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK) != ECMD_CHIPFLAG_JTAG) &&
                 ((chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK) != ECMD_CHIPFLAG_FSI) ) {
        printed = "getringdump - eCMD plugin returned an invalid bustype in ecmdChipData.chipFlags\n";
        ecmdOutputError( printed.c_str() );
        rc = ECMD_DLL_INVALID;
        break;
      }
      /* Store our type */
      bustype = chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK;

      /* Now we need to find out if this is a chipUnit ring or not */
      rc = ecmdQueryRing(target, queryRingData, ringName.c_str(), ECMD_QUERY_DETAIL_LOW);
      if (rc) {
        printed = "getringdump - Error occurred performing queryring on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
      }
      if (queryRingData.size() != 1) {
        ecmdOutputError("getringdump - Too much/little ring information returned from the dll, unable to determine if it is a chipUnit ring\n");
        coeRc = ECMD_DLL_INVALID;
        continue;
      }

      ringData = queryRingData.begin();

      /* Setup our chipUnit looper if needed */
      cuTarget = target;
      if (ringData->isChipUnitRelated) {
        /* Error check the chipUnit returned */
        if (!ringData->isChipUnitMatch(chipUnitType)) {
          printed = "getringdump - Provided chipUnit \"";
          printed += chipUnitType;
          printed += "\"doesn't match chipUnit returned by queryRing \"";
          printed += ringData->relatedChipUnit + "\"\n";
          ecmdOutputError(printed.c_str());
          rc = ECMD_INVALID_ARGS;
          break;
        }
        /* If we have a chipUnit, set the state fields properly */
        if (chipUnitType != "") {
          cuTarget.chipUnitType = chipUnitType;
          cuTarget.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
        }
        cuTarget.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
        cuTarget.threadState = ECMD_TARGET_FIELD_UNUSED;

        /* Init the chipUnit loop */
        rc = ecmdLooperInit(cuTarget, ECMD_SELECTED_TARGETS_LOOP, cuLooper);
        if (rc) break;
      } else { // !ringData->isChipUnitRelated
        if (chipUnitType != "") {
          printed = "getringdump - A chipUnit \"";
          printed += chipUnitType;
          printed += "\" was given on a non chipUnit ring\n";
          ecmdOutputError(printed.c_str());
          rc = ECMD_INVALID_ARGS;
          break;
        }
      // Setup the variable oneLoop variable for this non-chipUnit case
      oneLoop = 1;
    }

    /* If this isn't a chipUnit ring we will fall into while loop and break at the end, if it is we will call run through configloopernext */
    while ((ringData->isChipUnitRelated ? ecmdLooperNext(cuTarget, cuLooper) : (oneLoop--)) && (!coeRc || coeMode)) {

        rc = getRing(cuTarget, ringName.c_str(), ringBuffer);
        if (rc) {
          printed = "getringdump - Error occurred performing getring on ";
          printed += ecmdWriteTarget(cuTarget);
          printed += "\n";
          ecmdOutputError( printed.c_str() );
          coeRc = rc;
          continue;
        }
        else {
          validPosFound = true;     
        }

        std::list< ecmdLatchDataEntry > curEntry;
        std::list< ecmdLatchDataEntry >::iterator curLatchInfo;    ///< Iterator for walking through latches

        rc = readScandefFile(target, ringName.c_str(), ringBuffer, curEntry);
        if (rc) break;

        //print ring header stuff
        printed = ecmdWriteTarget(cuTarget);
        printed += "\n*************************************************\n* ECMD Dump scan ring contents, ";
        printed += ctime(&curTime);
        if (cuTarget.chipUnitNumState == ECMD_TARGET_FIELD_UNUSED) {       
          sprintf(outstr, "* Position k%d:n%d:s%d:p%d, ", cuTarget.cage, cuTarget.node, cuTarget.slot, cuTarget.pos);
        } else {
          sprintf(outstr, "* Position k%d:n%d:s%d:p%d:c%d, ", cuTarget.cage, cuTarget.node, cuTarget.slot, cuTarget.pos, cuTarget.chipUnitNum);
        }
        printed += outstr;
        printed += cuTarget.chipType + " " + ringName + " Ring\n";

        sprintf(outstr, "* Chip EC %X\n", chipData.chipEc);
        printed += outstr;
        sprintf(outstr, "* Ring length: %d bits\n", ringBuffer.getBitLength());
        printed += outstr;
        ecmdOutput(printed.c_str());     


        uint32_t startBit, numBits;

        uint32_t bitsToFetch = ECMD_UNSET;       // Grab all bits
        uint32_t curLatchBit = ECMD_UNSET;       // This is the actual latch bit we are looking for next
        uint32_t curBufferBit = 0;               // Current bit to insert into buffered register
        uint32_t curBitsToFetch = bitsToFetch;   // This is the total number of bits left to fetch
        uint32_t dataStartBit = ECMD_UNSET;      // Actual start bit of buffered register
        uint32_t dataEndBit = ECMD_UNSET;        // Actual end bit of buffered register
        std::string latchname = "";
        ecmdLatchType_t isMultiBitLatch = ECMD_LATCHTYPE_SINGLEBIT;
	
        if(unsort) {
          for (curLatchInfo = curEntry.begin(); curLatchInfo != curEntry.end(); curLatchInfo++) {
            if(ringName == curLatchInfo->ringName) {

              printed = curLatchInfo->latchName;
              numBits = curLatchInfo->length;
              if (bustype == ECMD_CHIPFLAG_FSI) {
                startBit = curLatchInfo->fsiRingOffset;
                rc = ringBuffer.extract(buffer, startBit, numBits);
                if (rc) break;
              } else {
                /* When extracting JTAG we have to reverse the buffer */
                startBit = curLatchInfo->jtagRingOffset;
                rc = ringBuffer.extract(buffer, startBit - numBits + 1, numBits);
                if (rc) break;
                buffer.reverse();
              }
              if (format == "default") {

                if (numBits <= 8) {
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

              ecmdOutput(printed.c_str());

            }/* End If ringname match */ 

          }/* End for-latches-loop */

        }//end case unsort
        else {
          //Sort the entries
          curEntry.sort();

          for (curLatchInfo = curEntry.begin(); curLatchInfo != curEntry.end(); curLatchInfo++) {
            if(ringName == curLatchInfo->ringName) {

              if (curLatchInfo->latchType == ECMD_LATCHTYPE_ARRAY) {
                if (bustype == ECMD_CHIPFLAG_FSI) {
                  rc = ringBuffer.extract(buffer, curLatchInfo->fsiRingOffset, 1);
                  if (rc) break;
                } else { //JTAG 
                  rc = ringBuffer.extract(buffer, curLatchInfo->jtagRingOffset - 1, 1);
                  if (rc) break;
                }

                // Print out the latch
                printLatchInfo( latchname, buffer, curLatchInfo->latchStartBit, curLatchInfo->latchEndBit, format, ECMD_LATCHTYPE_ARRAY);             	
              } else {

                if (((dataStartBit != ECMD_UNSET) && (curLatchBit !=  curLatchInfo->latchStartBit) && (curLatchBit !=  curLatchInfo->latchEndBit)) ||
                    ((latchname == "") || (latchname != curLatchInfo->latchName.substr(0, curLatchInfo->latchName.rfind('('))))) {
                  /* I have some good data here */
                  if (latchname != "") {
                    printLatchInfo( latchname, buffer, dataStartBit, dataEndBit, format, isMultiBitLatch);             	
                  }

                  /* If this is a fresh one we need to reset everything */
                  if ((latchname == "") || (latchname != curLatchInfo->latchName.substr(0, curLatchInfo->latchName.rfind('(')))) {
                    dataStartBit = dataEndBit = ECMD_UNSET;
                    curBitsToFetch = ECMD_UNSET;
                    curBufferBit = 0;
                    isMultiBitLatch = ECMD_LATCHTYPE_SINGLEBIT;
                    latchname = curLatchInfo->latchName.substr(0, curLatchInfo->latchName.rfind('('));
                    if (curLatchInfo->latchName.rfind('(') != std::string::npos) {
                      isMultiBitLatch = ECMD_LATCHTYPE_MULTIBIT;
                    }
                    curLatchBit = curLatchInfo->latchStartBit < curLatchInfo->latchEndBit ? curLatchInfo->latchStartBit : curLatchInfo->latchEndBit;

                  } else {
                    /* This is the case where the scandef had holes in the register, so we will continue with this latch, but skip some bits */
                    dataStartBit = dataEndBit = ECMD_UNSET;
                    curBufferBit = 0;
                    /* Decrement the bits to fetch by the hole in the latch */
                    curBitsToFetch -= (curLatchInfo->latchStartBit < curLatchInfo->latchEndBit ? curLatchInfo->latchStartBit : curLatchInfo->latchEndBit) - curLatchBit;
                    curLatchBit = curLatchInfo->latchStartBit < curLatchInfo->latchEndBit ? curLatchInfo->latchStartBit : curLatchInfo->latchEndBit;
                  }
                }

                /* Do we want anything in here */
                /* Check if the bits are ordered from:to (0:10) or just (1) */
                if (((curLatchInfo->latchEndBit >= curLatchInfo->latchStartBit) && (curLatchBit <= curLatchInfo->latchEndBit)) ||
                    //@01c swapped type casting on curLatchBit and curLatchInfo->latchEndBit 
                    /* Check if the bits are ordered to:from (10:0) */
                    ((curLatchInfo->latchStartBit >  curLatchInfo->latchEndBit)  && ((uint32_t) curLatchBit <= curLatchInfo->latchStartBit))) {


                  bitsToFetch = (curLatchInfo->length  < curBitsToFetch) ? curLatchInfo->length : curBitsToFetch;

                  /* Setup the actual data bits displayed */
                  if (dataStartBit == ECMD_UNSET) {
                    dataStartBit = curLatchBit;
                    dataEndBit = dataStartBit - 1;
                  }
                  dataEndBit += bitsToFetch;

                  if (bustype == ECMD_CHIPFLAG_FSI) {
                    rc = ringBuffer.extract(buffertemp, curLatchInfo->fsiRingOffset, bitsToFetch);
                    if (rc) break;
                    /* Extract bits if ordered from:to (0:10) */
                    if (curLatchInfo->latchEndBit > curLatchInfo->latchStartBit) {
                      curLatchBit = curLatchInfo->latchEndBit + 1;
                      /* Extract if bits are ordered to:from (10:0) or just (1) */
                    } else {
                      if (bitsToFetch > 1) buffertemp.reverse();
                      curLatchBit = curLatchInfo->latchStartBit + 1;
                    }
                  } else {
                    //JTAG 
                    rc = ringBuffer.extract(buffertemp, curLatchInfo->jtagRingOffset - bitsToFetch + 1, bitsToFetch);
                    if (rc) break;
                    /* Extract bits if ordered from:to (0:10) */
                    if (curLatchInfo->latchEndBit > curLatchInfo->latchStartBit) {
                      buffertemp.reverse();
                      curLatchBit = curLatchInfo->latchEndBit + 1;
                    } else {
                      /* Extract if bits are ordered to:from (10:0) or just (1) */
                      curLatchBit = curLatchInfo->latchStartBit + 1;
                    }
                  }
                  if(curBufferBit == 0) {
                    buffer=buffertemp;
                  }
                  else {
                    buffer.growBitLength((buffer.getBitLength() + bitsToFetch));
                    rc = buffer.insert(buffertemp, curBufferBit, bitsToFetch);
                    if (rc) break;
                  }
                  curBufferBit += bitsToFetch;
                  //lint -e1058
                  if(curLatchInfo == --curEntry.end()) {
                    printLatchInfo( latchname, buffer, dataStartBit, dataEndBit, format, isMultiBitLatch);
                  }
                } else {
                  /* Nothing was there that we needed, let's try the next entry */
                  curLatchBit = curLatchInfo->latchStartBit < curLatchInfo->latchEndBit ? curLatchInfo->latchEndBit + 1: curLatchInfo->latchStartBit + 1;

                }
              }
            }/* End If ringname match */ 

          }/* End for-latches-loop */

        }/* Case-Sort */
      } /* End cuLooper */
    }/* End ChipLooper */
    // coeRc will be the return code from in the loop, coe mode or not.
    if (coeRc) return coeRc;
  } /* End Args Loop */

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("getringdump - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdGetLatchUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  bool expectFlag = false;
  ecmdLatchMode_t latchMode = ECMD_LATCHMODE_FULL;   ///< Default to full match on latch name
  char* expectDataPtr = NULL;
  ecmdLooperData looperData;                    ///< Store internal Looper data
  ecmdLooperData cuLooper;	                ///< Store internal Looper data for the chipUnit loop
  std::string outputformat = "default";         ///< Output Format to display
  std::string inputformat = "x";                ///< Input format of data
  std::string curOutputFormat;                  ///< Current output format to use for current latch
  ecmdDataBuffer expected;                      ///< Buffer to store output data
  ecmdChipTarget target;                        ///< Target we are operating on
  ecmdChipTarget cuTarget;                      ///< Current target being operated on for the chipUnit
  std::list<ecmdLatchData> queryLatchData;      ///< Latch data 
  std::list<ecmdLatchData>::iterator latchData; ///< Latch data from the query
  std::string printed;
  std::list<ecmdLatchEntry> latchEntry;         ///< Data returned from getLatch
  char temp[300];                               ///< Temp string buffer
  ecmdDataBuffer buffer;                        ///< Buffer for extracted data
  std::string ringName;                         ///< Ring name to fetch
  std::string latchName;                        ///< Latch name to fetch
  uint8_t oneLoop = 0;                          ///< Used to break out of the chipUnit loop after the first pass for non chipUnit operations

  bool validPosFound = false;                   ///< Did we find a valid chip in the looper
  bool validLatchFound = false;                 ///< Did we find a valid latch

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  if ((expectDataPtr = ecmdParseOptionWithArgs(&argc, &argv, "-exp")) != NULL) {
    expectFlag = true;
  }

  if (ecmdParseOption(&argc, &argv, "-partial")) {
    latchMode = ECMD_LATCHMODE_PARTIAL;
  } else if (ecmdParseOption(&argc, &argv, "-exact")) {
    latchMode = ECMD_LATCHMODE_FULL;
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

  if (argc < 2) {
    ecmdOutputError("getlatch - Too few arguments passed.\n");
    ecmdOutputError("getlatch - Type 'getlatch -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  std::string chipType, chipUnitType;
  rc = ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (rc) { 
    ecmdOutputError("getlatch - eCMD wildcard character detected however it is not supported by this command.\n");
    return rc;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  uint32_t startBit = ECMD_UNSET, curStartBit, numBits = ECMD_UNSET, curNumBits;

  /* Ok, now for the rest of the args, this is getting messy */
  if ((argc == 2) || (argc == 4)) {
    /* This is the case where the user didn't specify a ringname */

    latchName = argv[1];

    /* The user specified start/numbits */
    if (argc == 4) {

      if (!ecmdIsAllDecimal(argv[2])) {
	ecmdOutputError("getlatch - Non-decimal numbers detected in startbit field\n");
	return ECMD_INVALID_ARGS;
      }
      startBit = (uint32_t)atoi(argv[2]);
      if (!strcmp(argv[3], "end")) {
	numBits = ECMD_UNSET;
      }
      else {
	if (!ecmdIsAllDecimal(argv[3])) {
	  ecmdOutputError("getlatch - Non-decimal numbers detected in numbits field\n");
	  return ECMD_INVALID_ARGS;
	}
	numBits = (uint32_t)atoi(argv[3]);
      }

      /* Bounds check */
      if ((startBit + numBits) > ECMD_MAX_DATA_BITS) {
	char errbuf[100];
	sprintf(errbuf,"getlatch - Too much data requested > %d bits\n", ECMD_MAX_DATA_BITS);
	ecmdOutputError(errbuf);
	return ECMD_DATA_BOUNDS_OVERFLOW;
      }
    }

  } else if ((argc == 3) || (argc == 5)) {

    ringName = argv[1];
    latchName = argv[2];


    /* The user specified start/numbits */
    if (argc == 5) {

      if (!ecmdIsAllDecimal(argv[3])) {
        ecmdOutputError("getlatch - Non-decimal numbers detected in startbit field\n");
        return ECMD_INVALID_ARGS;
      }
      startBit = (uint32_t)atoi(argv[3]);
      if (!strcmp(argv[4], "end")) {
        numBits = ECMD_UNSET;
      }
      else {
        if (!ecmdIsAllDecimal(argv[4])) {
          ecmdOutputError("getlatch - Non-decimal numbers detected in numbits field\n");
          return ECMD_INVALID_ARGS;
        }
        numBits = (uint32_t)atoi(argv[4]);
      }

      /* Bounds check */
      if ((startBit + numBits) > ECMD_MAX_DATA_BITS) {
        char errbuf[100];
        sprintf(errbuf,"getlatch - Too much data requested > %d bits\n", ECMD_MAX_DATA_BITS);
        ecmdOutputError(errbuf);
        return ECMD_DATA_BOUNDS_OVERFLOW;
      }
    }

  } else {
    ecmdOutputError("getlatch - Unknown arguments passed.\n");
    ecmdOutputError("getlatch - Type 'getlatch -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }


  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {

    /* Now we need to find out if this is a chipUnit latch or not */
    if (ringName.length() != 0) 
      rc = ecmdQueryLatch(target, queryLatchData, latchMode, latchName.c_str(), ringName.c_str(), ECMD_QUERY_DETAIL_LOW); 
    else 
      rc = ecmdQueryLatch(target, queryLatchData, latchMode, latchName.c_str(), NULL, ECMD_QUERY_DETAIL_LOW); 
  
    if (rc == ECMD_INVALID_LATCHNAME) {
      printed = "getlatch - Error occurred performing querylatch on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      ecmdOutputError("getlatch - Unable to find latchname in scandef file\n");
      if (latchMode == ECMD_LATCHMODE_FULL)
	ecmdOutputError("getlatch - Try using '-partial' to enable pattern matching on the latch name\n");
      coeRc = rc;
      continue;
    } else if (rc) {
      printed = "getlatch - Error occurred performing querylatch on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }
    
    if (queryLatchData.size() < 1) {
      ecmdOutputError("getlatch - Too much/little latch information returned from the dll, unable to determine if it is a chipUnit latch\n");
      return ECMD_DLL_INVALID;
    }  

    // Query good, assign over
    latchData = queryLatchData.begin();

    /* Setup our chipUnit looper if needed */
    cuTarget = target;
    if (latchData->isChipUnitRelated) {
      /* Error check the chipUnit returned */
      if (!latchData->isChipUnitMatch(chipUnitType)) {
        printed = "getlatch - Provided chipUnit \"";
        printed += chipUnitType;
        printed += "\" doesn't match chipUnit returned by queryLatch \"";
        printed += latchData->relatedChipUnit + "\"\n";
        ecmdOutputError(printed.c_str());
        rc = ECMD_INVALID_ARGS;
        break;
      }
      /* If we have a chipUnit, set the state fields properly */
      if (chipUnitType != "") {
        cuTarget.chipUnitType = chipUnitType;
        cuTarget.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
      }
      cuTarget.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
      cuTarget.threadState = ECMD_TARGET_FIELD_UNUSED;

      /* Init the chipUnit loop */
      rc = ecmdLooperInit(cuTarget, ECMD_SELECTED_TARGETS_LOOP, cuLooper);
      if (rc) break;
    } else { // !latchData->isChipUnitRelated
      if (chipUnitType != "") {
        printed = "getlatch - A chipUnit \"";
        printed += chipUnitType;
        printed += "\" was given on a non chipUnit ring\n";
        ecmdOutputError(printed.c_str());
        rc = ECMD_INVALID_ARGS;
        break;
      }
      // Setup the variable oneLoop variable for this non-chipUnit case
      oneLoop = 1;
    }

    /* If this isn't a chipUnit latch we will fall into while loop and break at the end, if it is we will call run through configloopernext */
    while ((latchData->isChipUnitRelated ? ecmdLooperNext(cuTarget, cuLooper) : (oneLoop--)) && (!coeRc || coeMode)) {
	   
      /* Let's go grab our data */
      if (ringName.length() != 0)
	rc = getLatch(cuTarget, ringName.c_str(), latchName.c_str(), latchEntry, latchMode);
      else
	rc = getLatch(cuTarget, NULL, latchName.c_str(), latchEntry, latchMode);
      if (rc == ECMD_INVALID_LATCHNAME) {
	printed = "getlatch - Error occurred performing getlatch on ";
	printed += ecmdWriteTarget(cuTarget) + "\n";
	ecmdOutputError( printed.c_str() );
	ecmdOutputError("getlatch - Unable to find latchname in scandef file\n");
	coeRc = rc;
        continue;
      } else if (rc) {
	printed = "getlatch - Error occurred performing getlatch on ";
	printed += ecmdWriteTarget(cuTarget) + "\n";
	ecmdOutputError( printed.c_str() );
	coeRc = rc;
        continue;
      }
      else {
	validPosFound = true;
      }

      printed = ecmdWriteTarget(cuTarget) + "\n";
      ecmdOutput(printed.c_str());

      /* We need to loop over the data we got */
      for (std::list<ecmdLatchEntry>::iterator latchit = latchEntry.begin(); latchit != latchEntry.end(); latchit ++) {


	if (startBit == ECMD_UNSET) {
	  curStartBit = (uint32_t)latchit->latchStartBit;
	  curNumBits = latchit->buffer.getBitLength();
	} else if (numBits == ECMD_UNSET) {
	  curStartBit = startBit;
	  curNumBits = (uint32_t)((int)latchit->buffer.getBitLength() - (int)((int)startBit - (int)latchit->latchStartBit));
	} else {
	  curStartBit = startBit;
	  curNumBits = numBits;
	}

	/* See if there is data in here that we want */
	if ((curStartBit + curNumBits < (uint32_t) latchit->latchStartBit) || (curStartBit > (uint32_t) latchit->latchEndBit)) {
	  /* Nope nothing */
	  continue;
	} else
	  validLatchFound = true;

	/* Does the user want too much data? */
	if ((curStartBit + curNumBits - 1) > (uint32_t) latchit->latchEndBit)
	  curNumBits = (uint32_t) latchit->latchEndBit - curStartBit + 1;

	/* was the startbit before this latch ? */
	if (curStartBit < (uint32_t) latchit->latchStartBit) {
	  curNumBits -= ((uint32_t) latchit->latchStartBit - curStartBit);
	  curStartBit = (uint32_t) latchit->latchStartBit;
	}
 


	if (outputformat == "default") {

	  if (curNumBits <= 8) {
	    curOutputFormat = "b";
	  } else {
	    curOutputFormat = "xl";
	  }

	} else
	  curOutputFormat = outputformat;

	/* Let's extract the piece the user wanted */
	rc =(uint32_t) ( latchit->buffer.extract(buffer,(uint32_t)((int) curStartBit - (int)latchit->latchStartBit),(uint32_t) curNumBits )); if (rc) return rc;

	if (expectFlag) {

	  /* Grab the data for the expect */
	  if (inputformat == "d") 
	  {
	    //char msgbuf[100];
	    //sprintf(msgbuf,"Resizing expected value to requested number of bits (%d).\n", curNumBits);
	    //ecmdOutput(msgbuf);
	    
	    rc = ecmdReadDataFormatted(expected, expectDataPtr, inputformat, curNumBits);
	  }
	  else
	  {
	    rc = ecmdReadDataFormatted(expected, expectDataPtr, inputformat);
	  }
	  if (rc) {
	    ecmdOutputError("getlatch - Problems occurred parsing expected data, must be an invalid format\n");
	    return rc;
	  }
 

	  uint32_t mismatchBit = 0;
	  if (!ecmdCheckExpected(buffer, expected, mismatchBit)) {

	    //@ make this stuff sprintf'd
	    sprintf(temp, "getlatch - Data miscompare occurred for latch: %s\n", latchit->latchName.c_str());
	    printed = temp;
	    ecmdOutputError( printed.c_str() );

	    if (mismatchBit != ECMD_UNSET) {
	      sprintf(temp, "First bit mismatch found at bit %d\n",startBit + mismatchBit);
	      ecmdOutputError( temp );
	    }

	    printed = "getlatch - Actual 	   : ";
	    printed += ecmdWriteDataFormatted(buffer, curOutputFormat);
	    ecmdOutputError( printed.c_str() );

	    printed = "getlatch - Expected	   : ";
	    printed += ecmdWriteDataFormatted(expected, curOutputFormat);
	    ecmdOutputError( printed.c_str() );
            coeRc = ECMD_EXPECT_FAILURE;
	  }

	}
	else {

	  if (curNumBits == 1)
	    sprintf(temp,"%s(%d) ", latchit->latchName.c_str(), curStartBit);
	  else
	    sprintf(temp,"%s(%d:%d) ", latchit->latchName.c_str(), curStartBit, curStartBit+curNumBits-1);
	  printed = temp;
	  printed += ecmdWriteDataFormatted(buffer, curOutputFormat);
	  ecmdOutput( printed.c_str() );

	}
      } /* End for loop */

      if (!validLatchFound) {
	ecmdOutputError("getlatch - Unable to find a latch with the given startbit\n");
	return ECMD_INVALID_LATCHNAME;
      }
    } /* End cuLooper */
  } /* End PosLooper */
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("getlatch - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}
#endif // ECMD_REMOVE_LATCH_FUNCTIONS


#ifndef ECMD_REMOVE_RING_FUNCTIONS
uint32_t ecmdGetBitsUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;
  bool expectFlag = false;
  char* expectDataPtr = NULL;
  ecmdLooperData looperData;            ///< Store internal Looper data
  ecmdLooperData cuLooper;              ///< Store internal Looper data for the chipUnit loop
  std::string outputformat = "b";       ///< Output Format to display
  std::string inputformat = "b";        ///< Input format of data
  ecmdChipTarget target;                ///< Current target operating on
  ecmdChipTarget cuTarget;              ///< Current target being operated on for the chipUnit
  std::list<ecmdRingData> queryRingData;        ///< Ring data 
  std::list<ecmdRingData>::iterator ringData;   ///< Ring data from query 
  std::string ringName;                 ///< Ring to fetch
  uint32_t startBit;                    ///< Start bit to fetch
  uint32_t numBits;                     ///< Number of bits to fetch
  ecmdDataBuffer ringBuffer;            ///< Buffer for entire ring
  ecmdDataBuffer buffer;                ///< Buffer for portion user requested
  ecmdDataBuffer expected;              ///< Buffer to store expected data
  bool validPosFound = false;           ///< Did the looper find anything to execute on
  bool outputformatflag = false;
  bool inputformatflag = false; 
  uint8_t oneLoop = 0;                  ///< Used to break out of the chipUnit loop after the first pass for non chipUnit operations

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
    outputformatflag = true;
  }
  formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    inputformat = formatPtr;
    inputformatflag = true;
  }
  /* Get the filename if -f is specified */
  char * filename = ecmdParseOptionWithArgs(&argc, &argv, "-f");
  std::string printed;

  if((filename != NULL) && (inputformatflag || outputformatflag || expectFlag) ) {
    printed = "getbits - Options '-f' cannot be specified with '-i', '-o' or '-exp' options.\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
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
  if ((argc < 4) && (filename == NULL)) {  //chip + address
    ecmdOutputError("getbits - Too few arguments specified; you need at least a chip, ring, startbit, and numbits.\n");
    ecmdOutputError("getbits - Type 'getbits -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if((argc < 2) && (filename != NULL)) {  //chip + address
    ecmdOutputError("getbits - Too few arguments specified; you need at least a chip, ring, and outputfile.\n");
    ecmdOutputError("getbits - Type 'getbits -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  std::string chipType, chipUnitType;
  rc = ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (rc) { 
    ecmdOutputError("getbits - eCMD wildcard character detected however it is not supported by this command.\n");
    return rc;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  ringName = argv[1];

  if (filename != NULL) {
    startBit = 0;
    numBits = ECMD_UNSET;
  } 
  else {
  
    if (!ecmdIsAllDecimal(argv[2])) {
      ecmdOutputError("getbits - Non-decimal numbers detected in startbit field\n");
      return ECMD_INVALID_ARGS;
    }
    startBit = (uint32_t)atoi(argv[2]);

    if (!strcmp(argv[3], "end")) {
      numBits = ECMD_UNSET;
    }
    else {
      if (!ecmdIsAllDecimal(argv[3])) {
	ecmdOutputError("getbits - Non-decimal numbers detected in numbits field\n");
	return ECMD_INVALID_ARGS;
      }
      numBits = (uint32_t)atoi(argv[3]);


      /* Bounds check */
      if ((startBit + numBits) > ECMD_MAX_DATA_BITS) {
	char errbuf[100];
	sprintf(errbuf,"getbits - Too much data requested > %d bits\n", ECMD_MAX_DATA_BITS);
	ecmdOutputError(errbuf);
	return ECMD_DATA_BOUNDS_OVERFLOW;
      } else if (numBits == 0) {
	ecmdOutputError("getbits - Zero bit length requested\n");
	return ECMD_DATA_UNDERFLOW;
      }
    }
  }


  if (argc > 4) {
    ecmdOutputError("getbits - Too many arguments specified; you probably added an option that wasn't recognized.\n");
    ecmdOutputError("getbits - Type 'getbits -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/
  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;
  char outstr[300];
  
  while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {

    /* Now we need to find out if this is a chipUnit ring or not */
    rc = ecmdQueryRing(target, queryRingData, ringName.c_str(), ECMD_QUERY_DETAIL_LOW);
    if (rc) {
      printed = "getbits - Error occurred performing queryring on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }
    if (queryRingData.size() != 1) {
      ecmdOutputError("getbits - Too much/little ring information returned from the dll, unable to determine if it is a chipUnit ring\n");
      coeRc = ECMD_DLL_INVALID;
      continue;
    }

    ringData = queryRingData.begin();

    /* Setup our chipUnit looper if needed */
    cuTarget = target;
    if (ringData->isChipUnitRelated) {
      /* Error check the chipUnit returned */
      if (!ringData->isChipUnitMatch(chipUnitType)) {
        printed = "getbits - Provided chipUnit \"";
        printed += chipUnitType;
        printed += "\" doesn't match chipUnit returned by queryRing \"";
        printed += ringData->relatedChipUnit + "\"\n";
        ecmdOutputError(printed.c_str());
        rc = ECMD_INVALID_ARGS;
        break;
      }
      /* If we have a chipUnit, set the state fields properly */
      if (chipUnitType != "") {
        cuTarget.chipUnitType = chipUnitType;
        cuTarget.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
      }
      cuTarget.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
      cuTarget.threadState = ECMD_TARGET_FIELD_UNUSED;

      /* Init the chipUnit loop */
      rc = ecmdLooperInit(cuTarget, ECMD_SELECTED_TARGETS_LOOP, cuLooper);
      if (rc) break;
    } else { // !ringData->isChipUnitRelated
      if (chipUnitType != "") {
        printed = "getbits - A chipUnit \"";
        printed += chipUnitType;
        printed += "\" was given on a non chipUnit ring\n";
        ecmdOutputError(printed.c_str());
        rc = ECMD_INVALID_ARGS;
        break;
      }
      // Setup the variable oneLoop variable for this non-chipUnit case
      oneLoop = 1;
    }

    /* If this isn't a chipUnit ring we will fall into while loop and break at the end, if it is we will call run through configloopernext */
    while ((ringData->isChipUnitRelated ? ecmdLooperNext(cuTarget, cuLooper) : (oneLoop--)) && (!coeRc || coeMode)) {

      rc = getRing(cuTarget, ringName.c_str(), ringBuffer);
      if (rc) {
        printed = "getbits - Error occurred performing getring on ";
        printed += ecmdWriteTarget(cuTarget) + "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
      }
      else {
        validPosFound = true;     
      }


      numBits = (ringBuffer.getBitLength() - startBit < numBits) ? ringBuffer.getBitLength() - startBit : numBits;
      rc = ringBuffer.extract(buffer, startBit, numBits);
      if (rc) break;

      if (expectFlag) {
	/* Grab the data for the expect */
	if (inputformat == "d") 
	{
	  //char msgbuf[100];
	  //sprintf(msgbuf,"Resizing expected value to requested number of bits (%d).\n", numBits);
	  //ecmdOutput(msgbuf);
	    
	  rc = ecmdReadDataFormatted(expected, expectDataPtr, inputformat, numBits);
	}
	else
	{
	  rc = ecmdReadDataFormatted(expected, expectDataPtr, inputformat);
	}
	if (rc) {
	  ecmdOutputError("getlatch - Problems occurred parsing expected data, must be an invalid format\n");
	  return rc;
	}

	uint32_t mismatchBit = 0;
        if (!ecmdCheckExpected(buffer, expected, mismatchBit)) {

          //@ make this stuff sprintf'd
          printed = ecmdWriteTarget(cuTarget) + "  " + ringName;
          sprintf(outstr, "(%d:%d)\n", startBit, startBit + numBits - 1);
          printed += outstr;
          ecmdOutputError( printed.c_str() );
	  if (mismatchBit != ECMD_UNSET) {
	    sprintf(outstr, "First bit mismatch found at bit %d\n",startBit + mismatchBit);
	    ecmdOutputError( outstr );
	  }
          printed =  "Actual            : ";
          printed += ecmdWriteDataFormatted(buffer, outputformat);
          ecmdOutputError( printed.c_str() );

          printed = "Expected          : ";
          printed += ecmdWriteDataFormatted(expected, outputformat);
          ecmdOutputError( printed.c_str() );
          coeRc = ECMD_EXPECT_FAILURE;
          continue;
        }

      }
      else {
        printed = ecmdWriteTarget(cuTarget) + "  " + ringName;
        sprintf(outstr, "(%d:%d)", startBit, startBit + numBits - 1);
        printed += outstr;
        if (filename != NULL) {
          rc = buffer.writeFile(filename, ECMD_SAVE_FORMAT_ASCII);

          if (rc) {
            printed += "getbits - Problems occurred writing data into file" + (std::string)filename + "\n";
            ecmdOutputError(printed.c_str()); 
            break;
          }
          ecmdOutput( printed.c_str() );
        } 
        else {
          std::string dataStr = ecmdWriteDataFormatted(buffer, outputformat);
          if (dataStr[0] != '\n') {
            printed += "\n";
          }
          printed += dataStr;
          ecmdOutput( printed.c_str() );
        } 

      } /* End !expectFlag */
    } /* End cuLooper */
  } /* End posLooper */
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("getbits - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdPutBitsUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  ecmdLooperData looperData;            ///< Store internal Looper data
  ecmdLooperData cuLooper;              ///< Store internal Looper data for the chipUnit loop
  std::string format = "b";             ///< Input format
  std::string dataModifier = "insert";  ///< Default data Modifier (And/Or/insert)
  ecmdChipTarget target;                ///< Current target being operated on
  ecmdChipTarget cuTarget;              ///< Current target being operated on for the chipUnit
  std::list<ecmdRingData> queryRingData;        ///< Ring data 
  std::list<ecmdRingData>::iterator ringData;   ///< Ring data from query
  ecmdDataBuffer ringBuffer;            ///< Buffer to store entire ring
  ecmdDataBuffer buffer;                ///< Buffer to store data insert data
  bool validPosFound = false;           ///< Did the looper find something ?
  bool formatflag = false;
  uint8_t oneLoop = 0;                  ///< Used to break out of the chipUnit loop after the first pass for non chipUnit operations

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  //expect and mask flags check

  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    format = formatPtr;
    formatflag = true;
  }
  formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-b");
  if (formatPtr != NULL) {
    dataModifier = formatPtr;
  }

  /* Get the filename if -f is specified */
  char * filename = ecmdParseOptionWithArgs(&argc, &argv, "-f");
  std::string printed;

  if((filename != NULL) && (formatflag) ) {
    printed = "putbits - Options '-f' and '-i' can't be specified together for format. Specify either one.\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
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
  if ((argc < 4 ) && (filename == NULL)) {  //chip + address
    ecmdOutputError("putbits - Too few arguments specified; you need at least a chip, ring, startbit, and data block.\n");
    ecmdOutputError("putbits - Type 'putbits -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  else if((argc < 2) && (filename != NULL) ) {
    ecmdOutputError("putbits - Too few arguments specified; you need at least a chip, ring, and data file.\n");
    ecmdOutputError("putbits - Type 'putbits -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 4) {
    ecmdOutputError("putbits - Too many arguments specified; you probably added an unsupported option.\n");
    ecmdOutputError("putbits - Type 'putbits -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  std::string chipType, chipUnitType;
  rc = ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (rc) { 
    ecmdOutputError("putbits - eCMD wildcard character detected however it is not supported by this command.\n");
    return rc;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get ring name and starting position
  std::string ringName = argv[1];
  uint32_t startBit = 0;

  if(filename != NULL) {
    rc = buffer.readFile(filename, ECMD_SAVE_FORMAT_ASCII);
    if (rc) {
      printed = "putbits - Problems occurred parsing input data from file" + (std::string)filename + ", must be an invalid format\n";
      ecmdOutputError(printed.c_str());
      return rc;
    }
  }
  else {
   
    if (!ecmdIsAllDecimal(argv[2])) {
      ecmdOutputError("putbits - Non-decimal numbers detected in startbit field\n");
      return ECMD_INVALID_ARGS;
    }
    startBit = (uint32_t)atoi(argv[2]);
   
    //container to store data
    rc = ecmdReadDataFormatted(buffer, argv[3], format);
    if (rc) {
      ecmdOutputError("putbits - Problems occurred parsing input data, must be an invalid format\n");
      return rc;
    }
  } 
  
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/
  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {

    /* Now we need to find out if this is a chipUnit ring or not */
    rc = ecmdQueryRing(target, queryRingData, ringName.c_str(), ECMD_QUERY_DETAIL_LOW);
    if (rc) {
      printed = "putbits - Error occurred performing queryring on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }
    if (queryRingData.size() != 1) {
      ecmdOutputError("putbits - Too much/little ring information returned from the dll, unable to determine if it is a chipUnit ring\n");
      coeRc = ECMD_DLL_INVALID;
      continue;
    }
    ringData = queryRingData.begin();

    /* Setup our chipUnit looper if needed */
    cuTarget = target;
    if (ringData->isChipUnitRelated) {
      /* Error check the chipUnit returned */
      if (!ringData->isChipUnitMatch(chipUnitType)) {
        printed = "putbits - Provided chipUnit \"";
        printed += chipUnitType;
        printed += "\" doesn't match chipUnit returned by queryRing \"";
        printed += ringData->relatedChipUnit + "\"\n";
        ecmdOutputError(printed.c_str());
        rc = ECMD_INVALID_ARGS;
        break;
      }
      /* If we have a chipUnit, set the state fields properly */
      if (chipUnitType != "") {
        cuTarget.chipUnitType = chipUnitType;
        cuTarget.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
      }
      cuTarget.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
      cuTarget.threadState = ECMD_TARGET_FIELD_UNUSED;

      /* Init the chipUnit loop */
      rc = ecmdLooperInit(cuTarget, ECMD_SELECTED_TARGETS_LOOP, cuLooper);
      if (rc) break;
    } else { // !ringData->isChipUnitRelated
      if (chipUnitType != "") {
        printed = "putbits - A chipUnit \"";
        printed += chipUnitType;
        printed += "\" was given on a non chipUnit ring\n";
        ecmdOutputError(printed.c_str());
        rc = ECMD_INVALID_ARGS;
        break;
      }
      // Setup the variable oneLoop variable for this non-chipUnit case
      oneLoop = 1;
    }

    /* If this isn't a chipUnit ring we will fall into while loop and break at the end, if it is we will call run through configloopernext */
    while ((ringData->isChipUnitRelated ? ecmdLooperNext(cuTarget, cuLooper) : (oneLoop--)) && (!coeRc || coeMode)) {

      /* If the data the user gave on the command line is as long as the ring, and starts at bit 0
         we don't need to do the getRing since they gave us an entire ring image */
      if ((startBit == 0) && (buffer.getBitLength() >= ringData->bitLength)) {
        ringBuffer.setBitLength(ringData->bitLength);
      } else {
        rc = getRing(cuTarget, ringName.c_str(), ringBuffer);
        if (rc) {
          printed = "putbits - Error occurred performing getring on ";
          printed += ecmdWriteTarget(cuTarget) + "\n";
          ecmdOutputError( printed.c_str() );
          coeRc = rc;
          continue;
        }
      }

      rc = ecmdApplyDataModifier(ringBuffer, buffer, startBit, dataModifier);
      if (rc) break;

      rc = putRing(cuTarget, ringName.c_str(), ringBuffer);
      if (rc) {
	printed = "putbits - Error occurred performing putring on ";
	printed += ecmdWriteTarget(cuTarget) + "\n";
	ecmdOutputError( printed.c_str() );
	coeRc = rc;
        continue;
      }
      else {
        validPosFound = true;     
      }

      if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
	printed = ecmdWriteTarget(cuTarget) + "\n";
	ecmdOutput(printed.c_str());
      }
    } /* End cuLooper */
  } /* End posloop */
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("putbits - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}
#endif // ECMD_REMOVE_RING_FUNCTIONS


#ifndef ECMD_REMOVE_LATCH_FUNCTIONS
uint32_t ecmdPutLatchUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  ecmdLooperData looperData;            ///< Store internal Looper data
  ecmdLooperData cuLooper;              ///< Store internal Looper data for the chipUnit loop
  std::string format = "x";             ///< Output format
  std::string dataModifier = "insert";  ///< Default data Modifier (And/Or/insert)
  ecmdChipTarget target;                ///< Current target being operated on
  ecmdChipTarget cuTarget;              ///< Current target being operated on for the chipUnit
  std::list<ecmdLatchData> queryLatchData;      ///< Latch data 
  std::list<ecmdLatchData>::iterator latchData; ///< Latch data from the query
  bool validPosFound = false;           ///< Did the looper find anything ?
  bool validLatchFound = false;         ///< Did we find a valid latch
  std::string printed;
  std::list<ecmdLatchEntry> latches;     ///< latches retrieved from getLatch
  std::list<ecmdLatchEntry>::iterator latchit;  ///< Iterator over the latches
  ecmdLatchMode_t latchMode = ECMD_LATCHMODE_FULL;   ///< Default to full match on latch name
  ecmdDataBuffer buffer;                ///< Buffer to store data from user
  ecmdDataBuffer buffer_copy;           ///< Copy of buffer for manipulation
  uint32_t matchs;                      ///< Number of matchs returned from putlatch
  bool enabledCache = false;            ///< Did we enable the cache ?
  uint8_t oneLoop = 0;                  ///< Used to break out of the chipUnit loop after the first pass for non chipUnit operations

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    format = formatPtr;
  }

  if (ecmdParseOption(&argc, &argv, "-partial")) {
    latchMode = ECMD_LATCHMODE_PARTIAL;
  } else if (ecmdParseOption(&argc, &argv, "-exact")) {
    latchMode = ECMD_LATCHMODE_FULL;
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

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  //Setup the target that will be used to query the system config 
  std::string chipType, chipUnitType;
  rc = ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (rc) { 
    ecmdOutputError("putlatch - eCMD wildcard character detected however it is not supported by this command.\n");
    return rc;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  uint32_t startBit = ECMD_UNSET, curStartBit, numBits = 0, curNumBits;
  std::string ringName;                 ///< Ring name selected ("" if none)
  std::string latchName;                ///< Latch name selected


  /* Ok, this gets a bit nasty to handle the optional ring name */
  if ((argc == 3) || (argc == 5)) {
    /* This is the case where the user didn't specify a ringname */

    latchName = argv[1];

    /* The user specified start/numbits */
    if (argc == 5) {

      if (!ecmdIsAllDecimal(argv[2])) {
	ecmdOutputError("putlatch - Non-decimal numbers detected in startbit field\n");
	return ECMD_INVALID_ARGS;
      }
      startBit = (uint32_t)atoi(argv[2]);
      if (!ecmdIsAllDecimal(argv[3])) {
	ecmdOutputError("putlatch - Non-decimal numbers detected in numbits field\n");
	return ECMD_INVALID_ARGS;
      }
      numBits = (uint32_t)atoi(argv[3]);

      /* Bounds check */
      if ((startBit + numBits) > ECMD_MAX_DATA_BITS) {
	char errbuf[100];
	sprintf(errbuf,"putlatch - Too much data requested > %d bits\n", ECMD_MAX_DATA_BITS);
	ecmdOutputError(errbuf);
	return ECMD_DATA_BOUNDS_OVERFLOW;
      }
    }

  } else if ((argc == 4) || (argc == 6)) {

    ringName = argv[1];
    latchName = argv[2];


    /* The user specified start/numbits */
    if (argc == 6) {

      if (!ecmdIsAllDecimal(argv[3])) {
        ecmdOutputError("putlatch - Non-decimal numbers detected in startbit field\n");
        return ECMD_INVALID_ARGS;
      }
      startBit = (uint32_t)atoi(argv[3]);
      if (!ecmdIsAllDecimal(argv[4])) {
        ecmdOutputError("putlatch - Non-decimal numbers detected in numbits field\n");
        return ECMD_INVALID_ARGS;
      }
      numBits = (uint32_t)atoi(argv[4]);

      /* Bounds check */
      if ((startBit + numBits) > ECMD_MAX_DATA_BITS) {
        char errbuf[100];
        sprintf(errbuf,"putlatch - Too much data requested > %d bits\n", ECMD_MAX_DATA_BITS);
        ecmdOutputError(errbuf);
        return ECMD_DATA_BOUNDS_OVERFLOW;
      }
    }

  } else {
    ecmdOutputError("putlatch - Unknown arguments passed.\n");
    ecmdOutputError("putlatch - Type 'getlatch -h' for usage.");
    return ECMD_INVALID_ARGS;
  }

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {

    /* We are going to enable the ring cache to get performance out of this beast */
    /* Since we are in a target looper, the state fields should be set properly so just use this target */
    if (!ecmdIsRingCacheEnabled(target)) {
      rc = ecmdEnableRingCache(target);
      if (rc) {
        ecmdOutputError("getlatch - ecmdEnableRingCache call failed!\n");
        coeRc = rc;
        continue;
      }
      enabledCache = true;
    }

    if (ringName.length() != 0) 
      rc = ecmdQueryLatch(target, queryLatchData, latchMode, latchName.c_str(), ringName.c_str(), ECMD_QUERY_DETAIL_LOW);
    else
      rc = ecmdQueryLatch(target, queryLatchData, latchMode, latchName.c_str(), NULL, ECMD_QUERY_DETAIL_LOW); 
    if (rc == ECMD_INVALID_LATCHNAME) {
      printed = "putlatch - Error occurred performing querylatch on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      ecmdOutputError("putlatch - Unable to find latchname in scandef file\n");
      if (latchMode == ECMD_LATCHMODE_FULL)
        ecmdOutputError("getlatch - Try using '-partial' to enable pattern matching on the latch name\n");
      coeRc = rc;
      continue;
    } else if (rc) {
      printed = "putlatch - Error occurred performing querylatch on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }
    if (queryLatchData.size() < 1) {
      ecmdOutputError("putlatch - Too much/little latch information returned from the dll, unable to determine if it is a chipUnit latch\n");
      return ECMD_DLL_INVALID;
    }

    /* We have to look at the data here so that we can read in right aligned data properly - JTA 04/07/06 */
    //data is always the last arg
    rc = ecmdReadDataFormatted(buffer, argv[argc-1], format, ((numBits) ? (int)numBits : queryLatchData.begin()->bitLength));
    if (rc) {
      ecmdOutputError("putlatch - Problems occurred parsing input data, must be an invalid format\n");
      break;
    }

    // Query good, assign over
    latchData = queryLatchData.begin();

    /* Setup our chipUnit looper if needed */
    cuTarget = target;
    if (latchData->isChipUnitRelated) {
      /* Error check the chipUnit returned */
      if (!latchData->isChipUnitMatch(chipUnitType)) {
        printed = "putlatch - Provided chipUnit \"";
        printed += chipUnitType;
        printed += "\" doesn't match chipUnit returned by queryLatch \"";
        printed += latchData->relatedChipUnit + "\"\n";
        ecmdOutputError(printed.c_str());
        rc = ECMD_INVALID_ARGS;
        break;
      }
      /* If we have a chipUnit, set the state fields properly */
      if (chipUnitType != "") {
        cuTarget.chipUnitType = chipUnitType;
        cuTarget.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
      }
      cuTarget.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
      cuTarget.threadState = ECMD_TARGET_FIELD_UNUSED;

      /* Init the chipUnit loop */
      rc = ecmdLooperInit(cuTarget, ECMD_SELECTED_TARGETS_LOOP, cuLooper);
      if (rc) break;
    } else { // !latchData->isChipUnitRelated
      if (chipUnitType != "") {
        printed = "putlatch - A chipUnit \"";
        printed += chipUnitType;
        printed += "\" was given on a non chipUnit ring\n";
        ecmdOutputError(printed.c_str());
        rc = ECMD_INVALID_ARGS;
        break;
      }
      // Setup the variable oneLoop variable for this non-chipUnit case
      oneLoop = 1;
    }

    /* If this isn't a chipUnit latch we will fall into while loop and break at the end, if it is we will call run through configloopernext */
    while ((latchData->isChipUnitRelated ? ecmdLooperNext(cuTarget, cuLooper) : (oneLoop--)) && (!coeRc || coeMode)) {

      if (ringName.length() != 0)
        rc = getLatch(cuTarget, ringName.c_str(), latchName.c_str(), latches, latchMode);
      else
        rc = getLatch(cuTarget, NULL, latchName.c_str(), latches, latchMode);
      if (rc == ECMD_INVALID_LATCHNAME) {
        printed = "putlatch - Error occurred performing getlatch on ";
        printed += ecmdWriteTarget(cuTarget) + "\n";
        ecmdOutputError( printed.c_str() );
        ecmdOutputError("putlatch - Unable to find latchname in scandef file\n");
        coeRc = rc;
        continue;
      } else if (rc) {
        printed = "putlatch - Error occurred performing getlatch on ";
        printed += ecmdWriteTarget(cuTarget) + "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
      }
      else {
        validPosFound = true;
      }

      /* Walk through all the latches recieved */
      for (latchit = latches.begin(); latchit != latches.end(); latchit ++ ) {

        buffer_copy = buffer;

        if (startBit == ECMD_UNSET) {
          curStartBit = (uint32_t)latchit->latchStartBit;
          curNumBits = latchit->buffer.getBitLength();
        } else {
          curStartBit = startBit;
          curNumBits = numBits;
        }

        /* See if there is data in here that we want */
        if ((curStartBit + curNumBits < (uint32_t) latchit->latchStartBit) || (curStartBit > (uint32_t) latchit->latchEndBit)) {
          /* Nope nothing */
          continue;
        } else
          validLatchFound = true;

        /* Does the user want too much data? */
        if ((curStartBit + curNumBits - 1) > (uint32_t) latchit->latchEndBit)
          curNumBits = (uint32_t) latchit->latchEndBit - curStartBit + 1;

        /* was the startbit before this latch ? */
        if (curStartBit < (uint32_t) latchit->latchStartBit) {
          curNumBits -= ((uint32_t) latchit->latchStartBit - curStartBit);
          curStartBit = (uint32_t) latchit->latchStartBit;
        }

        /* Let's apply our data */
        /* We are going to throw away extra data that the user provided if it doesn't fit this latch */
        if (curNumBits < buffer_copy.getBitLength()) {
          buffer_copy.shrinkBitLength(curNumBits); //lint -e732
        }
        rc = (uint32_t)ecmdApplyDataModifier(latchit->buffer, buffer_copy,(int) ((int)curStartBit - (int)latchit->latchStartBit), dataModifier);
        if (rc) {
          printed = "putlatch - Error occurred inserting data of " + latchit->latchName + " on ";
          printed += ecmdWriteTarget(cuTarget) + "\n";
          ecmdOutputError( printed.c_str() );
          coeRc = rc;
          continue;
        }

        /* We can do a full latch compare here now to make sure we don't cause matching problems */
        if (ringName.length() != 0) {
          rc = putLatch(cuTarget, ringName.c_str(), latchit->latchName.c_str(), latchit->buffer,(uint32_t) (latchit->latchStartBit), (uint32_t)(latchit->latchEndBit - latchit->latchStartBit + 1), matchs, ECMD_LATCHMODE_FULL);
        } else {
          rc = putLatch(cuTarget, NULL, latchit->latchName.c_str(), latchit->buffer, (uint32_t)latchit->latchStartBit, (uint32_t)(latchit->latchEndBit - latchit->latchStartBit + 1), matchs,  ECMD_LATCHMODE_FULL);
        }
        if (rc) {
          printed = "putlatch - Error occurred performing putlatch of " + latchit->latchName + " on ";
          printed += ecmdWriteTarget(cuTarget) + "\n";
          ecmdOutputError( printed.c_str() );
          coeRc = rc;
          continue;
        } else if (matchs > 1) {
          printed = "putlatch - Error occurred performing putlatch, multiple matchs found on write, data corruption may have occurred on " + ecmdWriteTarget(cuTarget) + "\n";
          ecmdOutputError( printed.c_str() );
          coeRc = ECMD_FAILURE;
          continue;
        }

      }

      if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
        printed = ecmdWriteTarget(cuTarget) + "\n";
        ecmdOutput(printed.c_str());
      }

      if (!validLatchFound) {
        ecmdOutputError("putlatch - Unable to find a latch with the given startbit\n");
        rc = ECMD_INVALID_LATCHNAME;
        break;
      }
    } /* End cuLooper */

    /* Now that we are moving onto the next target, let's flush the cache we have */
    if (enabledCache) {
      rc = ecmdDisableRingCache(target);
      if (rc) {
        ecmdOutputError("putlatch - Problems disabling the ring cache\n");
        break;
      }
      enabledCache = false;
    }
  } /* End PosLooper */
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  /* This catches if a loop above was exited prematurely */
  /* Otherwise it wouldn't be necessary because the same check at the end of the posloop would work */
  if (enabledCache) {
    rc = ecmdDisableRingCache(target);
    if (rc) {
      ecmdOutputError("putlatch - Problems disabling the ring cache\n");
      return rc;
    }
  }

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("putlatch - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}
#endif // ECMD_REMOVE_LATCH_FUNCTIONS

#ifndef ECMD_REMOVE_RING_FUNCTIONS
uint32_t ecmdCheckRingsUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;
  uint32_t ret_rc = ECMD_SUCCESS;

  bool allRingsFlag = false;
  ecmdLooperData looperData;            ///< Store internal Looper data
  ecmdLooperData cuLooper;              ///< Store internal Looper data for the chipUnit loop
  ecmdChipTarget target;                ///< Current target being operated on
  ecmdChipTarget cuTarget;              ///< Current target being operated on for the chipUnit
  ecmdDataBuffer ringOrgBuffer;         ///< Original Ring Buffer to be restored after the pattern test
  ecmdDataBuffer ringBuffer;            ///< Buffer to store ring
  ecmdDataBuffer readRingBuffer;          ///< Read out the data to test the pattern
  std::list<ecmdRingData> queryRingData;///< Ring data 
  bool validPosFound = false;           ///< Did the looper find anything ?
  bool foundProblem;                    ///< Did we find a mismatch ?
  ecmdCheckRingData ringlog;            ///< Used to push new entries
  std::list<ecmdCheckRingData> failedRings;   ///< Names of rings that failed
  std::list<ecmdCheckRingData> passedRings;   ///< Names of rings that failed
  bool verbose = false;                 ///< Verbose error display
  uint8_t oneLoop = 0;                  ///< Used to break out of the chipUnit loop after the first pass for non chipUnit operations
  std::string printed;
  ecmdChipData chipData;                ///< Chip data to find out bus info
  bool saveRestore;              ///< Save restore the ring data?
  // Selects for the different scan tests
  bool flush0;
  bool flush1;
  bool pattern0;
  bool pattern1;
  bool pattern = false;
  // Does the user want to force a broadside read or write of the data
  bool bsRead = false;
  bool bsWrite = false;
  std::string bsOriginalState;
  std::string bsModifiedState;
  uint32_t configNum;
  ecmdConfigValid_t configValid;
  ecmdDataBuffer passedPattern;
  char* patternptr = NULL; 
  std::string inputformat = "x";

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  verbose = ecmdParseOption(&argc, &argv, "-v");

  saveRestore = !ecmdParseOption(&argc, &argv, "-nosr");

  flush0 = ecmdParseOption(&argc, &argv, "-flush0");

  flush1 = ecmdParseOption(&argc, &argv, "-flush1");

  pattern0 = ecmdParseOption(&argc, &argv, "-pattern0");

  pattern1 = ecmdParseOption(&argc, &argv, "-pattern1");


  if ((patternptr = ecmdParseOptionWithArgs(&argc, &argv, "-pattern")) != NULL) 
  {
     pattern = true;
     rc = ecmdReadDataFormatted(passedPattern, patternptr, inputformat);
     if (rc) 
     {
       ecmdOutputError("Checkrings - Problems occurred parsing expected data, must be an invalid format\n");
       return rc;
     }
  }

  

  /* If none of the options were specified, turn them all on */
  if ((flush0 + flush1 + pattern0 + pattern1 + pattern) == 0) {
    flush0 = true;
    flush1 = true;
    pattern0 = true;
    pattern1 = true;    
  }

  bsRead = ecmdParseOption(&argc, &argv, "-bsread");

  bsWrite = ecmdParseOption(&argc, &argv, "-bswrite");

  /* Error check some conditions related to these options */
  if (bsRead || bsWrite) {

    // If both are on, tell the user to just turn on broadside mode
    if (bsRead && bsWrite) {
      ecmdOutputError("checkrings - If you want to do both a broadside read and write, please set SIM_BROADSIDE_MODE to include scan.\n");
      return ECMD_INVALID_ARGS;
    }

    // Make sure broadside isn't already on with these flags
    rc = ecmdGetConfiguration(cuTarget, "SIM_BROADSIDE_MODE", configValid, bsOriginalState, configNum);
    if (rc) return rc;
    
    /* Set a global flag to know if we are starting out with broadside on */
    if (bsOriginalState.find("scan") != std::string::npos) {
      ecmdOutputError("checkrings - To use the -bsread/-bswrite options, SIM_BROADSIDE_MODE \"scan\" can't be on to start.\n");
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
  std::string chipType, chipUnitType;
  rc = ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (rc) { 
    ecmdOutputError("checkrings - eCMD wildcard character detected however it is not supported by this command.\n");
    return rc;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  std::string ringName = argv[1];

  if (ringName == "all") {
    allRingsFlag = true;
  }

  char outstr[300];
  uint32_t checkPattern = 0xA5A5A5A5;
  std::string repPattern;
  
  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;


  while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {

    // Get the chip data so we know if we have to deal with headerchecks
    rc = ecmdGetChipData(target, chipData);
    if (rc) {
      coeRc = rc;
      continue;
    }

    if (allRingsFlag) {
      rc = ecmdQueryRing(target, queryRingData, NULL, ECMD_QUERY_DETAIL_LOW);
    } else {
      rc = ecmdQueryRing(target, queryRingData, ringName.c_str(), ECMD_QUERY_DETAIL_LOW);
    }
    if (rc) {
      coeRc = rc;
      continue;
    }

    /* If it was an all rings check, and we have chipUnit, loop through the data and only */
    /* use the rings that match the chipunit given - JTA 12/17/08 */
    if (allRingsFlag && chipUnitType != "") {
      std::list<ecmdRingData>::iterator curRingData = queryRingData.begin();
      std::list<ecmdRingData>::iterator lastRingData;

      while (curRingData != queryRingData.end()) {
        /* Doesn't match, so remove */
        if (!curRingData->isChipUnitRelated || !curRingData->isChipUnitMatch(chipUnitType)) {
          lastRingData = curRingData;
          lastRingData--; // backup to a point before we were going to erase
          queryRingData.erase(curRingData);
          curRingData = lastRingData; // Now go to our backup point, so the ++ below advances
        }
        curRingData++;
      }
    }

    std::list<ecmdRingData>::iterator curRingData = queryRingData.begin();

    while (curRingData != queryRingData.end()) {

      /* Setup our chipUnit looper if needed */
      cuTarget = target;
      if (curRingData->isChipUnitRelated) {
        // We have to do different things for an all check vs. a specific ring
        // For an all check, it needs to be a chip level target and we use the chipunit returned
        // For a ring specific check, what the user gave has to match what the query returned
        if (!allRingsFlag && !curRingData->isChipUnitMatch(chipUnitType)) {
          printed = "checkrings - Provided chipUnit \"";
          printed += chipUnitType;
          printed += "\" doesn't match chipUnit returned by queryRing \"";
          printed += curRingData->relatedChipUnit + "\"\n";
          ecmdOutputError(printed.c_str());
          coeRc = ECMD_INVALID_ARGS;
          break;
        }

        /* If we have a chipUnit, set the state fields properly */
        if (curRingData->relatedChipUnit != "") {
          cuTarget.chipUnitType = curRingData->relatedChipUnit;
          cuTarget.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
        }
        cuTarget.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
        cuTarget.threadState = ECMD_TARGET_FIELD_UNUSED;

        /* Init the chipUnit loop */
        /* For an all loop, we want to default to all targets at the chipUnit level
          It doesn't make sense to have the "all" option just do 0 by default */
        if (allRingsFlag) {
          rc = ecmdLooperInit(cuTarget, ECMD_SELECTED_TARGETS_LOOP_DEFALL, cuLooper);
        } else {
          rc = ecmdLooperInit(cuTarget, ECMD_SELECTED_TARGETS_LOOP, cuLooper);
        }
        if (rc) break;
      } else { // !curRingData->isChipUnitRelated
        // If it's not an all rings test, make sure the user didn't specify a chipunit
        if (!allRingsFlag && chipUnitType != "") {
          printed = "checkrings - A chipUnit \"";
          printed += chipUnitType;
          printed += "\" was given on a non chipUnit ring\n";
          ecmdOutputError(printed.c_str());
          coeRc = ECMD_INVALID_ARGS;
          break;
        }
        // Setup the variable oneLoop variable for this non-chipUnit case
        oneLoop = 1;
      }

      /* If this isn't a chipUnit ring we will fall into while loop and break at the end, if it is we will call run through configloopernext */
      while ((curRingData->isChipUnitRelated ? ecmdLooperNext(cuTarget, cuLooper) : (oneLoop--)) && (!coeRc || coeMode)) {

        ringName = curRingData->ringNames.front();

        if (!curRingData->isCheckable && allRingsFlag) {
          break;
        }

        ringlog.ringName = ringName;
        if (curRingData->isChipUnitRelated) {
          ringlog.chipUnitNum = cuTarget.chipUnitNum;
          ringlog.chipUnitType = cuTarget.chipUnitType;
        } else {
          ringlog.chipUnitNum = -1;
        }

        /* Print out the current target */
        printed = ecmdWriteTarget(cuTarget) + "\n"; ecmdOutput(printed.c_str());

        //Save the Ring state
        if (saveRestore) {
          printed = "Saving the ring state before performing pattern testing.\n";
          ecmdOutput(printed.c_str());   
          rc = getRing(cuTarget, ringName.c_str(), ringOrgBuffer);
          if (rc) {
            printed = "checkrings - Error occurred performing getring on ";
            printed += ecmdWriteTarget(cuTarget) + "\n";
            ecmdOutputError(printed.c_str());   
            coeRc = rc;
          
            /* Go onto the next one */
            failedRings.push_back(ringlog);
            continue;
          }
        }

        ringBuffer.setBitLength(curRingData->bitLength);
        foundProblem = false;

        for (int i = 0; i < 5; i++) {

          if (i == 0) {
            if (!flush0) {
              continue;
            }
            repPattern = "0000000";
            ringBuffer.flushTo0();
          }
          else if (i == 1) {
            if (!flush1) {
              continue;
            }
            repPattern = "1111111";
            ringBuffer.flushTo1();
          }
          else if (i == 2) {
            if (!pattern0) {
              continue;
            }
            // repeating pattern of 1001010s
            repPattern = "1001010";
            for (uint32_t y=0; y<ringBuffer.getBitLength(); ) {
              ringBuffer.setBit(y++);
              if (y<ringBuffer.getBitLength()) {ringBuffer.clearBit(y++);  }
              if (y<ringBuffer.getBitLength()) {ringBuffer.clearBit(y++);  }
              if (y<ringBuffer.getBitLength()) {ringBuffer.setBit(y++);}
              if (y<ringBuffer.getBitLength()) {ringBuffer.clearBit(y++);  }
              if (y<ringBuffer.getBitLength()) {ringBuffer.setBit(y++);}
              if (y<ringBuffer.getBitLength()) {ringBuffer.clearBit(y++);  }
            }
          }
          else if (i == 3) {
            if (!pattern1) {
              continue;
            }
            // repeating pattern of 0110101s
            repPattern = "0110101";
            for (uint32_t y=0; y<ringBuffer.getBitLength(); ) {
              ringBuffer.clearBit(y++);
              if (y<ringBuffer.getBitLength()) {ringBuffer.setBit(y++);  }
              if (y<ringBuffer.getBitLength()) {ringBuffer.setBit(y++);  }
              if (y<ringBuffer.getBitLength()) {ringBuffer.clearBit(y++);}
              if (y<ringBuffer.getBitLength()) {ringBuffer.setBit(y++);  }
              if (y<ringBuffer.getBitLength()) {ringBuffer.clearBit(y++);}
              if (y<ringBuffer.getBitLength()) {ringBuffer.setBit(y++);  }
            }
          }
          else if (i == 4) {
            if (!pattern) {
              continue;
            }

            uint32_t curOffset = 0;
            uint32_t numBitsToInsert = 0;
            uint32_t numBitsInRing = ringBuffer.getBitLength();
            ringBuffer.setBitLength(numBitsInRing);
            while (curOffset < numBitsInRing) {
              numBitsToInsert = (32 < (numBitsInRing - curOffset)) ? 32 : (numBitsInRing - curOffset);
              //printf("numBitsInRing =%d, curOffset = %d numBitsToInsert = %d \n", numBitsInRing, curOffset, numBitsToInsert);
              rc = ringBuffer.insert(passedPattern, curOffset, numBitsToInsert);
              if (rc) break;
              curOffset += numBitsToInsert;
            }
          }

          // Stick a fixed checkPattern at the front so we can tell if the ring shifted
          // Only do this on non 32 bit header check chips.
          // On those chips, the hardware is taking care of doing the check, so clear the data
          if (chipData.chipFlags & ECMD_CHIPFLAG_32BIT_HEADERCHECK) {
            ringBuffer.clearBit(0, 32);
          } else {
            ringBuffer.setWord(0, checkPattern);  //write the pattern
          }

           /* If the user wants to force a broadside write, handle that before doing the put below */
           if (bsWrite) {
             if (bsOriginalState == "none") {
               bsModifiedState = "scan";
             } else { // Other options are on, tack it on
               bsModifiedState = bsOriginalState + ",scan";       
             }
             /* Set it */
             rc = ecmdSetConfiguration(cuTarget, "SIM_BROADSIDE_MODE", ECMD_CONFIG_VALID_FIELD_ALPHA, bsModifiedState, configNum);
             if (rc) return rc;
           }

          // Write in my data and get it back out.
          rc = putRing(cuTarget, ringName.c_str(), ringBuffer);
          if (rc) {
            printed = "checkrings - Error occurred performing putring on ";
            printed += ecmdWriteTarget(cuTarget) + "\n";
            ecmdOutputError( printed.c_str() );
            foundProblem = true;
            coeRc = rc;
            continue;
          }
          else {
            validPosFound = true;
          }

          /* If we did a broadside write, restore state */
          if (bsWrite) {
            rc = ecmdSetConfiguration(cuTarget, "SIM_BROADSIDE_MODE", ECMD_CONFIG_VALID_FIELD_ALPHA, bsOriginalState, configNum);
            if (rc) return rc;
          }

          /* If the user wants to force a broadside read, handle that before doing the get below */
          if (bsRead) {
            if (bsOriginalState == "none") {
              bsModifiedState = "scan";
            } else { // Other options are on, tack it on
              bsModifiedState = bsOriginalState + ",scan";       
            }
            /* Set it */
            rc = ecmdSetConfiguration(cuTarget, "SIM_BROADSIDE_MODE", ECMD_CONFIG_VALID_FIELD_ALPHA, bsModifiedState, configNum);
            if (rc) return rc;
          }
  
          if(pattern)
          {
            printed = "Performing  " + passedPattern.genHexLeftStr() + "'s test on " + ringName + " ...\n";
          }
          else
          {
            printed = "Performing  " + repPattern + "'s test on " + ringName + " ...\n";
          }
          ecmdOutput(printed.c_str());
          rc = getRing(cuTarget, ringName.c_str(), readRingBuffer);
          if (rc) {
            printed = "checkrings - Error occurred performing getring on ";
            printed += ecmdWriteTarget(cuTarget) + "\n";
            ecmdOutputError( printed.c_str() );
            foundProblem = true;
            coeRc = rc;
            continue;
          }

          /* If we did a broadside read, restore state */
          if (bsRead) {
            rc = ecmdSetConfiguration(cuTarget, "SIM_BROADSIDE_MODE", ECMD_CONFIG_VALID_FIELD_ALPHA, bsOriginalState, configNum);
            if (rc) return rc;
          }

          /******* Check our results *******/

          // Do not test the last bit or the ring (The access bit)
          if (readRingBuffer.isBitSet((readRingBuffer.getBitLength())-1)) {
            ringBuffer.setBit((readRingBuffer.getBitLength())-1);
          } else {
            ringBuffer.clearBit((readRingBuffer.getBitLength())-1);
          }

          // On the hardware headercheck chips, clear the headercheck data out so we don't get false fails
          // On non-headercheck chips, check the starting pattern
          if (chipData.chipFlags & ECMD_CHIPFLAG_32BIT_HEADERCHECK) {
            readRingBuffer.clearBit(0,32);
          } else {
            if (readRingBuffer.getWord(0) != checkPattern) {
              sprintf(outstr, "checkrings - Data fetched from ring %s did not match expected header: %.08X.  Found: %.08X\n", ringName.c_str(), checkPattern, readRingBuffer.getWord(0));
              ecmdOutputWarning( outstr );
              foundProblem = true;
            }
          }

          if (readRingBuffer != ringBuffer) {
            sprintf(outstr, "checkrings - Data fetched from ring %s did not match repeated pattern of %s's\n", ringName.c_str(),
                    repPattern.c_str());
            ecmdOutputWarning( outstr );
            if (verbose) {
              printed = "Offset  Data\n";
              printed += "------------------------------------------------------------------------\n";
              ecmdOutput( printed.c_str() );
              for (uint32_t y=0; y < readRingBuffer.getBitLength();) {
                printf("%6d ", y);
                if ( (y+64) > readRingBuffer.getBitLength()) {
                  printed = readRingBuffer.genBinStr(y,(readRingBuffer.getBitLength()-y)) + "\n";
                } else {
                  printed = readRingBuffer.genBinStr(y,64) + "\n";
                }
                y += 64;
                ecmdOutput( printed.c_str() );
              }
            }
            foundProblem = true;
          } 

          if (foundProblem) {
            printed = "checkrings - Error occurred performing a checkring on " + ecmdWriteTarget(cuTarget) + "\n";
            ecmdOutputWarning( printed.c_str() );
          }

        } /* Test for loop */

        //Restore ring state
        if (saveRestore) {
          printed = "Restoring the ring state.\n\n";
          ecmdOutput( printed.c_str() );
          rc = putRing(cuTarget, ringName.c_str(), ringOrgBuffer);
          if (rc) {
            printed = "checkrings - Error occurred performing putring on ";
            printed += ecmdWriteTarget(cuTarget) + "\n";
            ecmdOutputError( printed.c_str() );
            coeRc = rc;
            failedRings.push_back(ringlog);
            continue;
          }
        }

        if (foundProblem) {
          failedRings.push_back(ringlog);
        } else {
          passedRings.push_back(ringlog);
        }
      } /* end chipUnit looper */
      curRingData++;
    }
    
    ecmdOutput("\n\n");
    ecmdOutput("============================================\n");
    ecmdOutput("CheckRings Summary : \n");
    sprintf(outstr,"Passed Rings : %lu \n", (unsigned long)passedRings.size());
    ecmdOutput(outstr);
    sprintf(outstr,"Failed Rings : %lu\n", (unsigned long)failedRings.size());
    ecmdOutput(outstr);
    ecmdOutput("============================================\n");
    sprintf(outstr,"Passed Rings : %lu\n", (unsigned long)passedRings.size());
    ecmdOutput(outstr);
    for (std::list<ecmdCheckRingData>::iterator strit = passedRings.begin(); strit != passedRings.end(); strit ++) {
      if (strit->chipUnitNum == -1) {
        printed = strit->ringName + " passed.\n";
        ecmdOutput(printed.c_str());
      } else {
        sprintf(outstr,"%s on %s %d passed.\n",strit->ringName.c_str(), strit->chipUnitType.c_str(), strit->chipUnitNum);
        ecmdOutput(outstr);
      }
    }
    ecmdOutput("============================================\n");
    sprintf(outstr,"Failed Rings : %lu\n", (unsigned long)failedRings.size());
    ecmdOutput(outstr);
    for (std::list<ecmdCheckRingData>::iterator strit2 = failedRings.begin(); strit2 != failedRings.end(); strit2 ++) {
      if (strit2->chipUnitNum == -1) {
        printed = strit2->ringName + " failed.\n";
        ecmdOutput(printed.c_str());
      } else {
        sprintf(outstr,"%s on %s %d failed.\n",strit2->ringName.c_str(), strit2->chipUnitType.c_str(), strit2->chipUnitNum);
        ecmdOutput(outstr);
      }

    }
    ecmdOutput("============================================\n");
    ecmdOutput("CheckRings Summary : \n");
    sprintf(outstr,"Passed Rings : %lu \n", (unsigned long)passedRings.size());
    ecmdOutput(outstr);
    sprintf(outstr,"Failed Rings : %lu\n", (unsigned long)failedRings.size());
    ecmdOutput(outstr);
    ecmdOutput("============================================\n");

    /* Make the final return code a failure if we found a miscompare */
    if (failedRings.size() != 0) {
      ret_rc = ECMD_EXPECT_FAILURE;
      if (!verbose) {
        ecmdOutput("checkrings - To get additional failure data run again with the -v option\n");
      }
    } 
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  if (!validPosFound) {
    ecmdOutputError("checkrings - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return ret_rc;
}


uint32_t ecmdPutPatternUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  std::list<ecmdRingData> queryRingData;///< Ring query data
  std::list<ecmdRingData>::iterator ringData;  ///< Ring data from query
  ecmdLooperData looperData;            ///< Store internal Looper data
  ecmdLooperData cuLooper;              ///< Store internal Looper data for the chipUnit loop
  ecmdChipTarget target;                ///< Current target being operated on
  ecmdChipTarget cuTarget;              ///< Current target being operated on for the chipUnit
  ecmdDataBuffer ringBuffer;            ///< Buffer to store entire ring
  std::string printed;                  ///< Output print data
  bool validPosFound = false;           ///< Did the looper find anything?
  ecmdDataBuffer buffer;                ///< Buffer to store pattern
  std::string format = "xr";            ///< Default input format
  uint8_t oneLoop = 0;                  ///< Used to break out of the chipUnit loop after the first pass for non chipUnit operations

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
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

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

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
  std::string chipType, chipUnitType;
  rc = ecmdParseChipField(argv[0], chipType, chipUnitType);
  if (rc) { 
    ecmdOutputError("putpattern - eCMD wildcard character detected however it is not supported by this command.\n");
    return rc;
  }
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  std::string ringName = argv[1];

  rc = ecmdReadDataFormatted(buffer, argv[2], format, 32);
  if (rc) {
    ecmdOutputError("putpattern - Problems occurred parsing input data, must be an invalid format\n");
    return rc;
  }

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {

    rc = ecmdQueryRing(target, queryRingData, ringName.c_str(), ECMD_QUERY_DETAIL_LOW);
    if (rc) {
      printed = "putpattern - Error occurred retrieving scan ring data on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    } else if (queryRingData.empty()) {
      printed = "putpattern - Unable to lookup data on ring " + ringName + ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = ECMD_INVALID_ARGS;
      continue;
    } 
    ringData = queryRingData.begin();

    /* Setup our chipUnit looper if needed */
    cuTarget = target;
    if (ringData->isChipUnitRelated) {
      /* Error check the chipUnit returned */
      if (!ringData->isChipUnitMatch(chipUnitType)) {
        printed = "putpattern - Provided chipUnit \"";
        printed += chipUnitType;
        printed += "\" doesn't match chipUnit returned by queryRing \"";
        printed += ringData->relatedChipUnit + "\"\n";
        ecmdOutputError(printed.c_str());
        rc = ECMD_INVALID_ARGS;
        break;
      }
      /* If we have a chipUnit, set the state fields properly */
      if (chipUnitType != "") {
        cuTarget.chipUnitType = chipUnitType;
        cuTarget.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
      }
      cuTarget.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
      cuTarget.threadState = ECMD_TARGET_FIELD_UNUSED;

      /* Init the chipUnit loop */
      rc = ecmdLooperInit(cuTarget, ECMD_SELECTED_TARGETS_LOOP, cuLooper);
      if (rc) break;
    } else { // !ringData->isChipUnitRelated
      if (chipUnitType != "") {
        printed = "putpattern - A chipUnit \"";
        printed += chipUnitType;
        printed += "\" was given on a non chipUnit ring\n";
        ecmdOutputError(printed.c_str());
        rc = ECMD_INVALID_ARGS;
        break;
      }
      // Setup the variable oneLoop variable for this non-chipUnit case
      oneLoop = 1;
    }

    /* If this isn't a chipUnit ring we will fall into while loop and break at the end, if it is we will call run through configloopernext */
    while ((ringData->isChipUnitRelated ? ecmdLooperNext(cuTarget, cuLooper) : (oneLoop--)) && (!coeRc || coeMode)) {
      uint32_t curOffset = 0;
      uint32_t numBitsToInsert = 0;
      uint32_t numBitsInRing = queryRingData.front().bitLength;
      ringBuffer.setBitLength(numBitsInRing);
      while (curOffset < numBitsInRing) {
        numBitsToInsert = (32 < (numBitsInRing - curOffset)) ? 32 : (numBitsInRing - curOffset);
        rc = ringBuffer.insert(buffer, curOffset, numBitsToInsert);
        if (rc) break;
        curOffset += numBitsToInsert;
      }

      rc = putRing(cuTarget, ringName.c_str(), ringBuffer);
      if (rc) {
        printed = "putpattern - Error occurred performing putring on ";
        printed += ecmdWriteTarget(cuTarget) + "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
      } else {
        validPosFound = true;     
      }

      if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
        printed = ecmdWriteTarget(cuTarget) + "\n";
        ecmdOutput(printed.c_str());
      }
    } /* End cuLooper */
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("putpattern - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdRingCacheUser(int argc, char* argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;
  
  std::string printed;                          ///< Output data
  ecmdChipTarget target;
  ecmdLooperData looperData;            ///< Store internal Looper data
  ecmdChipTarget vdTarget;
  ecmdLooperData vdLooperData;            ///< Store internal Looper data
  std::string action;
  bool posLoop = false;
  bool validPosFound = false;                   ///< Did we find a valid chip in the looper

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
  if (argc == 0) {
    ecmdOutputError("ringcache - Need to specify an operation on the ring cache 'enable', 'disable', 'flush', 'query'.\n");
    ecmdOutputError("ringcache - Type 'ringcache -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  /* First arg is what to do, let's parse that */
  if (!strcmp(argv[0],"enable")) {
    action = argv[0];
  } else if (!strcmp(argv[0],"disable")) {
    action = argv[0];
  } else if (!strcmp(argv[0],"flush")) {
    action = argv[0];
  } else if (!strcmp(argv[0],"query")) {
    action = argv[0];
  } else {
    ecmdOutputError("ringcache - Invalid ringcache argument specified.\n");
    ecmdOutputError("ringcache - Type 'ringcache -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }    

  if (argc == 1) {
    /* We're going loop at the cage level by default, then below we'll get all variable */
    target.cageState = ECMD_TARGET_FIELD_WILDCARD;
    target.nodeState = target.slotState = target.chipTypeState = target.posState = target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;
  } else if (argc == 2) {
    posLoop = true;
    target.chipType = argv[1];
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
    target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;
  } else {
    ecmdOutputError("ringcache - Too many arguments specified; you probably added an option that wasn't recognized.\n");
    ecmdOutputError("ringcache - Type 'ringcache -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = ecmdLooperInit(target, ECMD_ALL_TARGETS_LOOP, looperData);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {

    vdTarget = target;
    // Setup the variable depth looping if not a pos level loop
    if (!posLoop) {
      vdTarget.nodeState = vdTarget.slotState = ECMD_TARGET_FIELD_WILDCARD;
    }

    rc = ecmdLooperInit(vdTarget, ECMD_SELECTED_TARGETS_LOOP_VD, vdLooperData);
    if (rc) break;

    while (ecmdLooperNext(vdTarget, vdLooperData) && (!coeRc || coeMode)) {

      if (action == "enable") {
        rc = ecmdEnableRingCache(vdTarget);
      } else if (action == "disable") {
        rc = ecmdDisableRingCache(vdTarget);
      } else if (action == "flush") {
        rc = ecmdFlushRingCache(vdTarget);
      } else if (action == "query") {
        printed = "eCMD ring cache is ";
        if (ecmdIsRingCacheEnabled(vdTarget)) {
          printed += "enabled";
        } else {
          printed += "disabled";
        }
        printed += " on " + ecmdWriteTarget(vdTarget) + "\n";
        ecmdOutput(printed.c_str());
      } // No else case is needed, the actions get error checked above

      if (rc) {
        printed = "ringcache - Error occurred performing ringcache on ";
        printed += ecmdWriteTarget(vdTarget) + "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
      } else {
        validPosFound = true;
      }
    }
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("ringcache - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}
#endif // ECMD_REMOVE_RING_FUNCTIONS

#ifndef ECMD_REMOVE_LATCH_FUNCTIONS
/**
   @brief Parse the scandef for the ringname provided and load all the latches into latchBuffer for later retrieval
   @param target Chip target to operate on
   @param i_ringName Ringname for which the latches need to be looked up
   @param o_latchdata Return latches data read from scandef
*/
uint32_t readScandefFile(ecmdChipTarget & target, const char* i_ringName, ecmdDataBuffer &ringBuffer, std::list< ecmdLatchDataEntry > & o_latchdata) {
  uint32_t rc = ECMD_SUCCESS;
  std::string scandefFile;                      ///< Full path to scandef file
  std::string scandefHashFile;                  ///< Full path to scandefhash file
  std::string i_ring;                           ///< Ring that caller specified
  std::string printed;
  uint32_t i_ringkey;
  
  if (i_ringName != NULL) {
    i_ring = i_ringName;
    transform(i_ring.begin(), i_ring.end(), i_ring.begin(), (int(*)(int)) tolower);
    i_ringkey = ecmdHashString32(i_ring.c_str(), 0);
  }
  else {
    rc = ECMD_INVALID_RING;
    printed = "readScandefFile - Ring Name needs to be specified to do the lookup.\n";
    ecmdOutputError(printed.c_str());
    return rc;
  }

  //Try looking for the offset in scandef hash  
  bool ringOffsetFound = false;
  uint32_t curRingKey;
  uint32_t ringBeginOffset = 0;
  bool foundRing = false;
  std::string l_version = "default";    
      
  while(1) {
    /* Find the ring offset from the scandefhash file */
    rc = ecmdQueryFileLocation(target, ECMD_FILE_SCANDEFHASH, scandefHashFile, l_version);
    if (rc) {
      break;
    }

    std::ifstream insh(scandefHashFile.c_str());
    if (insh.fail()) {
      break;
    }
      
    uint32_t numRings =0;
    insh.read((char *)& numRings, 4);
    numRings = htonl(numRings);
      
    //Seek to the ring area in the hashfile
    insh.seekg ( 8 );
      
    while ( (uint32_t)insh.tellg() != (((numRings * 8) * 2) + 8) ) {//Loop until end of ring area
      insh.read( (char *)& curRingKey, 4 ); //Read the ringKey
      insh.read( (char *)& ringBeginOffset, 4 ); //Read the begin offset
      curRingKey = htonl(curRingKey);
      ringBeginOffset = htonl(ringBeginOffset);
	
      if (i_ringkey == curRingKey) {
	foundRing = true;
	break;
      }
      insh.seekg (8, std::ios::cur); //Skip the ringKey-end offset pair
	  
    }
      
    if (foundRing) {
      ringOffsetFound = true;
    }
    insh.close();
      
    break;
  }
  rc = 0;
  /* find scandef file */
  rc = ecmdQueryFileLocation(target, ECMD_FILE_SCANDEF, scandefFile, l_version);
  if (rc) {
    printed = "readScandefFile - Error occured locating scandef file: " + scandefFile + "\n";
    ecmdOutputError(printed.c_str());
    return rc;
  }

  std::ifstream ins(scandefFile.c_str());
  if (ins.fail()) {
    rc = ECMD_UNABLE_TO_OPEN_SCANDEF; 
    printed = "readScandefFile - Error occured opening scandef file: " + scandefFile + "\n";
    ecmdOutputError(printed.c_str());        //break;
    return rc;
  }
      
  std::string curLine;
  bool done = false;
  size_t  leftParen;
  size_t  colon;
      
  foundRing = false;
      
  //Seek to the beginning of the ring
  if (ringOffsetFound == true) ins.seekg((long)(size_t)ringBeginOffset);

  while (getline(ins, curLine) && !done) {
  
    //let's go hunting in the scandef for latches for this ring
    ecmdLatchDataEntry curLatch;

    std::vector<std::string> splitArgs;
    char outstr[1000];
    std::vector<std::string> curArgs(4);
    std::string temp;

    if (foundRing) {

      if (curLine[0] == 'E' && curLine.find("END") != std::string::npos) {
	if (i_ringName != NULL) done = true;
	continue;
      }
      else if ((curLine[0] == 'L') && curLine.find("Length") != std::string::npos) {
	/* Let's do a length check */
	ecmdParseTokens(curLine, " \t\n=", splitArgs);
	if ((splitArgs.size() >= 2) && ringBuffer.getBitLength() != (uint32_t) atoi(splitArgs[1].c_str())) {
	  sprintf(outstr, "readScandefFile - Warning : Length mismatch between ring fetched and scandef : fetched(%d) scandef(%d) on ring (%s)\n", ringBuffer.getBitLength(),atoi(splitArgs[1].c_str()),i_ringName);
	  ecmdOutputWarning(outstr);
	}
	continue;
      }
      else if (curLine.length() == 0 || curLine[0] == '\0' || curLine[0] == '*' || curLine[0] == '#') {
	//do nothing
	continue;
      }
      else if ((curLine[0] != ' ') && (curLine[0] != '\t')) {
	// do nothing
	continue;
      }
      else  {

	/* Transform to upper case */
	transform(curLine.begin(), curLine.end(), curLine.begin(), (int(*)(int)) toupper);

	ecmdParseTokens(curLine, " \t\n", curArgs);
	if (curArgs.size() >= 5) {
	  curLatch.length = (uint32_t)atoi(curArgs[0].c_str());
	  curLatch.fsiRingOffset = (uint32_t)atoi(curArgs[1].c_str());
	  curLatch.jtagRingOffset = (uint32_t)atoi(curArgs[2].c_str());
	  curLatch.latchName = curArgs[4];
	} else /* Not enought tokens for a valid latch line */
	  continue;
      }
          

      /* Let's parse out the start/end bit if they exist */
      leftParen = curLatch.latchName.rfind('(');
      if (leftParen == std::string::npos) {
	/* This latch doesn't have any parens */
	curLatch.latchStartBit = curLatch.latchEndBit = 0;
        curLatch.latchType = ECMD_LATCHTYPE_NOBIT;
      } else {
	temp = curLatch.latchName.substr(leftParen+1, curLatch.latchName.length() - leftParen - 1);
	curLatch.latchStartBit = (uint32_t)atoi(temp.c_str());

	/* Is this a multibit or single bit */
	if ((colon = temp.find(':')) != std::string::npos) {
	  curLatch.latchEndBit = (uint32_t)atoi(temp.substr(colon+1, temp.length()).c_str());
          curLatch.latchType = ECMD_LATCHTYPE_MULTIBIT;
	} else if ((colon = temp.find(',')) != std::string::npos) {
	  curLatch.latchEndBit = (uint32_t)atoi(temp.substr(colon+1, temp.length()).c_str());
          curLatch.latchType = ECMD_LATCHTYPE_ARRAY;
	} else {
	  curLatch.latchEndBit = curLatch.latchStartBit;
          curLatch.latchType = ECMD_LATCHTYPE_SINGLEBIT;
	}
      }
      curLatch.ringName = i_ring;
      o_latchdata.push_back(curLatch);
	  

    }
    /* The user specified a ring for us to look in */
    else if ((i_ringName != NULL) &&
	     ((curLine[0] == 'N') && (curLine.find("Name") != std::string::npos))) {
      ecmdParseTokens(curLine, " \t\n=", curArgs);
      /* Push the ring name to lower case */
      transform(curArgs[1].begin(), curArgs[1].end(), curArgs[1].begin(), (int(*)(int))tolower);
      if ((curArgs.size() >= 2) && curArgs[1] == i_ring) {
	foundRing = true;
      }

          
    }     
  }

  ins.close();
      
  if (!foundRing) {
    rc = ECMD_INVALID_RING;
    printed = "readScandefFile - Could not find ring name " + i_ring + "\n";
    ecmdOutputError(printed.c_str());
    return rc;
  }

  if (o_latchdata.empty()) {
    rc = ECMD_SCANDEF_LOOKUP_FAILURE;
    printed = "readScandefFile - Unable to find ring '" + i_ring + "'\n";
    ecmdOutputError(printed.c_str());
    printed = "readScandefFile - Scandef Used : " + scandefFile + "\n";
    ecmdOutputError(printed.c_str());
    return rc;
  }

  return rc;
}

void printLatchInfo(std::string latchname, ecmdDataBuffer buffer, uint32_t dataStartBit, uint32_t dataEndBit, std::string format, ecmdLatchType_t latchType) {
  char temp[50];
  std::string printed;
    
  printed = latchname;
  if (latchType == ECMD_LATCHTYPE_ARRAY) {
    sprintf(temp,"(%d,%d)", dataStartBit, dataEndBit);
    printed += temp;
  } else if (dataEndBit-dataStartBit >= 1) {
    sprintf(temp,"(%d:%d)", dataStartBit, dataEndBit);
    printed += temp;
  } else if ((dataEndBit == dataStartBit) && (latchType == ECMD_LATCHTYPE_MULTIBIT)) {
    sprintf(temp,"(%d)", dataStartBit);
    printed += temp;
  }
  if (format == "default") {
    if ((dataEndBit-dataStartBit) <= 8 || (latchType == ECMD_LATCHTYPE_ARRAY)) {
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
 
  ecmdOutput(printed.c_str());
}
#endif // ECMD_REMOVE_LATCH_FUNCTIONS

