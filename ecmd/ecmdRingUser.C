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
#include <ecmdSharedUtils.H>

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

uint32_t ecmdGetRingDumpUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  time_t curTime = time(NULL);
  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string ringName;                 ///< Ring name being worked on
  ecmdDataBuffer ringBuffer;            ///< Buffer to store entire ring contents
  ecmdDataBuffer buffer;                ///< Buffer to extract individual latch contents
  ecmdChipTarget target;                ///< Current target being operated on
  bool validPosFound = false;           ///< Did the looper find something ?
  std::string format = "default";       ///< Output format
  std::ifstream ins;                    ///< File stream
  ecmdChipData chipData;                ///< Chip data to find out bus info
  uint32_t bustype;                     ///< Stores if the chip was JTAG or FSI attached 

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
  char outstr[500];

  while ( ecmdConfigLooperNext(target, looperdata) ) {

    /* We need to find out if this chip is JTAG or FSI attached to handle the scandef properly */
    rc = ecmdGetChipData(target, chipData);
    if (rc) {
      printed = "getringdump - Problems retrieving chip information on : ";
      printed += ecmdWriteTarget(target);
      printed += "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    /* Now make sure the plugin gave us some bus info */
    if (!(chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK)) {
      printed = "getringdump - eCMD plugin did not implement ecmdChipData.chipFlags unable to determine if FSI or JTAG attached chip\n";
      ecmdOutputError( printed.c_str() );
      return ECMD_DLL_INVALID;
    } else if (((chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK) != ECMD_CHIPFLAG_JTAG) &&
               ((chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK) != ECMD_CHIPFLAG_FSI) ) {
      printed = "getringdump - eCMD plugin returned an invalid bustype in ecmdChipData.chipFlags\n";
      ecmdOutputError( printed.c_str() );
      return ECMD_DLL_INVALID;
    }
    /* Store our type */
    bustype = chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK;

      
    for (int i = 1; i < argc; i++) {

      std::string ringName = argv[i];

      /* find scandef file */
      std::string scandefFile;
      rc = ecmdQueryFileLocation(target, ECMD_FILE_SCANDEF, scandefFile);
      if (rc) {
        scandefFile = "getringdump - Error occurred locating scandef file in dir: " + scandefFile + "\n";
        ecmdOutputError(scandefFile.c_str());
        return rc;
      }
 
      ins.open(scandefFile.c_str());
      if (ins.fail()) {
        scandefFile = "getringdump - Error occurred opening scandef file: " + scandefFile + "\n";
        ecmdOutputError(scandefFile.c_str());
        return ECMD_INVALID_ARGS;  //change this
      }

      rc = getRing(target, ringName.c_str(), ringBuffer);
      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
      }
      else if (rc) {
        printed = "getringdump - Error occurred performing getring on ";
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

          if (curLine[0] == 'E' && curLine.find("END") != std::string::npos) {
            done = true;
          }
          else if ((curLine[0] == 'L') && curLine.find("Length") != std::string::npos) {
            /* Let's do a length check */
            ecmdParseTokens(curLine, " \t\n=", splitArgs);
            if ((splitArgs.size() >= 2) && ringBuffer.getBitLength() != atoi(splitArgs[1].c_str())) {
              sprintf(outstr, "getringdump - Warning : Length mismatch between ring fetched and scandef : fetched(%d) scandef(%d) on ring (%s)\n", ringBuffer.getBitLength(),atoi(splitArgs[1].c_str()),ringName.c_str());
              ecmdOutputWarning(outstr);
            }
          }
          else if (curLine.length() == 0 || curLine[0] == '\0' || curLine[0] == '*' || curLine[0] == '#') {
            //do nothing
          }
          else if ((curLine[0] != ' ') && (curLine[0] != '\t')) {
            // do nothing
          }
          else {

            ecmdParseTokens(curLine, " \t\n", splitArgs);

            printed = splitArgs[4];

            numBits = atoi(splitArgs[0].c_str());
            if (bustype == ECMD_CHIPFLAG_FSI) {
              startBit = atoi(splitArgs[1].c_str());
              ringBuffer.extract(buffer, startBit, numBits);
            } else {
              /* When extracting JTAG we have to reverse the buffer */
              startBit = atoi(splitArgs[2].c_str());
              ringBuffer.extract(buffer, startBit - numBits + 1, numBits);
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
            
          }

        }
        /* Can we find the ring we are looking for on this line */
        else if ((curLine[0] == 'N') && (curLine.find(ringName) != std::string::npos)) {
          ecmdParseTokens(curLine, " \t\n=", splitArgs);
          if ((splitArgs.size() >= 2) && splitArgs[1] == ringName)
            found = true;
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
  ecmdLatchMode_t latchMode = ECMD_LATCHMODE_PARTIAL;   ///< Default to pattern matching on latch name
  char* expectDataPtr = NULL;
  ecmdLooperData looperdata;                    ///< Store internal Looper data
  std::string outputformat = "default";         ///< Output Format to display
  std::string inputformat = "x";                ///< Input format of data
  std::string curOutputFormat;                  ///< Current output format to use for current latch
  ecmdDataBuffer expected;                      ///< Buffer to store output data
  ecmdChipTarget target;                        ///< Target we are operating on
  std::string printed;
  std::list<ecmdLatchEntry> latchdata;           ///< Data returned from getLatch
  char temp[100];                               ///< Temp string buffer
  ecmdDataBuffer buffer;                        ///< Buffer for extracted data
  std::string ringName;                         ///< Ring name to fetch
  std::string latchName;                        ///< Latch name to fetch

  bool validPosFound = false;                   ///< Did we find a valid chip in the looper
  bool validLatchFound = false;                 ///< Did we find a valid latch

  if ((expectDataPtr = ecmdParseOptionWithArgs(&argc, &argv, "-exp")) != NULL) {
    expectFlag = true;
  }

  if (ecmdParseOption(&argc, &argv, "-exact")) {
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


  if (expectFlag) {
    /* Grab the data for the expect */
    rc = ecmdReadDataFormatted(expected, expectDataPtr, inputformat);
    if (rc) {
      ecmdOutputError("getlatch - Problems occurred parsing expected data, must be an invalid format\n");
      return rc;
    }
  }

  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  uint32_t startBit = 0x0FFFFFFF, curStartBit, numBits = 0x0FFFFFFF, curNumBits;

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
     startBit = atoi(argv[2]);
     if (!strcmp(argv[3], "end")) {
       numBits = 0x0FFFFFFF;
     }
     else {
       if (!ecmdIsAllDecimal(argv[3])) {
         ecmdOutputError("getlatch - Non-decimal numbers detected in numbits field\n");
         return ECMD_INVALID_ARGS;
       }
       numBits = atoi(argv[3]);
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

  } else {
    ecmdOutputError("getlatch - Unknown arguments passed.\n");
    ecmdOutputError("getlatch - Type 'getlatch -h' for usage.");
    return ECMD_INVALID_ARGS;
  }



  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;


  while ( ecmdConfigLooperNext(target, looperdata) ) {


    /* Let's go grab our data */
    if (ringName.length() != 0) 
      rc = getLatch(target, ringName.c_str(), latchName.c_str(), latchdata, latchMode);
    else
      rc = getLatch(target, NULL, latchName.c_str(), latchdata, latchMode);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    } else if (rc == ECMD_INVALID_LATCHNAME) {
        printed = "getlatch - Error occurred performing getlatch on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        ecmdOutputError("getlatch - Unable to find latchname in scandef file\n");
        return rc;
    } else if (rc) {
        printed = "getlatch - Error occurred performing getlatch on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }

    printed = ecmdWriteTarget(target) + "\n";
    ecmdOutput(printed.c_str());

    /* We need to loop over the data we got */
    for (std::list<ecmdLatchEntry>::iterator latchit = latchdata.begin(); latchit != latchdata.end(); latchit ++) {


      if (startBit == 0x0FFFFFFF) {
        curStartBit = latchit->latchStartBit;
        curNumBits = latchit->buffer.getBitLength();
      } else if (numBits == 0x0FFFFFFF) {
        curStartBit = startBit;
        curNumBits = latchit->buffer.getBitLength() - (startBit - latchit->latchStartBit);
      } else {
        curStartBit = startBit;
        curNumBits = numBits;
      }

      /* See if there is data in here that we want */
      if ((curStartBit + curNumBits < latchit->latchStartBit) || (curStartBit > latchit->latchEndBit)) {
        /* Nope nothing */
        continue;
      } else
        validLatchFound = true;

      /* Does the user want too much data? */
      if ((curStartBit + curNumBits - 1) > latchit->latchEndBit)
        curNumBits = latchit->latchEndBit - curStartBit + 1;

      /* was the startbit before this latch ? */
      if (curStartBit < latchit->latchStartBit) {
        curNumBits -= (latchit->latchStartBit - curStartBit);
        curStartBit = latchit->latchStartBit;
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
      latchit->buffer.extract(buffer, 0, curNumBits );

      if (expectFlag) {


        if (!ecmdCheckExpected(buffer, expected)) {

          //@ make this stuff sprintf'd
          sprintf(temp, "getlatch - Data miscompare occurred for latch: %s\n", latchit->latchName.c_str());
          printed = temp;
          ecmdOutputError( printed.c_str() );


          printed = "getlatch - Actual            : ";
          printed += ecmdWriteDataFormatted(buffer, curOutputFormat);
          ecmdOutputError( printed.c_str() );

          printed = "getlatch - Expected          : ";
          printed += ecmdWriteDataFormatted(expected, curOutputFormat);
          ecmdOutputError( printed.c_str() );
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
        printed = "getbits - Error occurred performing getring on ";
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
        printed = "putbits - Error occurred performing getring on ";
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
        printed = "putbits - Error occurred performing putring on ";
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

  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string format = "x";             ///< Output format
  std::string dataModifier = "insert";          ///< Default data Modifier (And/Or/insert)
  ecmdChipTarget target;                ///< Current target being operated on
  bool validPosFound = false;           ///< Did the looper find anything ?
  bool validLatchFound = false;                 ///< Did we find a valid latch
  std::string printed;
  std::list<ecmdLatchEntry> latchs;     ///< Latchs retrieved from getLatch
  std::list<ecmdLatchEntry>::iterator latchit;  ///< Iterator over the latchs
  ecmdLatchMode_t latchMode = ECMD_LATCHMODE_PARTIAL;   ///< Default to pattern matching on latch name
  ecmdDataBuffer buffer;                ///< Buffer to store data from user
  uint32_t matchs;                      ///< Number of matchs returned from putlatch

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    format = formatPtr;
  }

  if (ecmdParseOption(&argc, &argv, "-exact")) {
    latchMode = ECMD_LATCHMODE_FULL;
  }

  formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-b");
  if (formatPtr != NULL) {
    dataModifier = formatPtr;
  }


  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  uint32_t startBit = 0x0FFFFFFF, curStartBit, numBits = 0, curNumBits;
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
     startBit = atoi(argv[2]);
     if (!ecmdIsAllDecimal(argv[3])) {
       ecmdOutputError("putlatch - Non-decimal numbers detected in numbits field\n");
       return ECMD_INVALID_ARGS;
     }
     numBits = atoi(argv[3]);

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
      startBit = atoi(argv[3]);
      if (!ecmdIsAllDecimal(argv[4])) {
        ecmdOutputError("putlatch - Non-decimal numbers detected in numbits field\n");
        return ECMD_INVALID_ARGS;
      }
      numBits = atoi(argv[4]);

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


  //data is always the last arg
  rc = ecmdReadDataFormatted(buffer, argv[argc-1], format, numBits);
  if (rc) {
    ecmdOutputError("putlatch - Problems occurred parsing input data, must be an invalid format\n");
    return rc;
  }

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;


  while ( ecmdConfigLooperNext(target, looperdata) ) {

    if (ringName.length() != 0) 
      rc = getLatch(target, ringName.c_str(), latchName.c_str(), latchs, latchMode);
    else
      rc = getLatch(target, NULL, latchName.c_str(), latchs, latchMode);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    } else if (rc == ECMD_INVALID_LATCHNAME) {
        printed = "putlatch - Error occurred performing getlatch on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        ecmdOutputError("putlatch - Unable to find latchname in scandef file\n");
        return rc;
    } else if (rc) {
      printed = "putlatch - Error occurred performing getlatch on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    else {
      validPosFound = true;     
    }



    /* Walk through all the latchs recieved */
    for (latchit = latchs.begin(); latchit != latchs.end(); latchit ++ ) {


      if (startBit == 0x0FFFFFFF) {
        curStartBit = 0;
        curNumBits = latchit->buffer.getBitLength();
      } else {
        curStartBit = startBit;
        curNumBits = numBits;
      }

      /* See if there is data in here that we want */
      if ((curStartBit + curNumBits < latchit->latchStartBit) || (curStartBit > latchit->latchEndBit)) {
        /* Nope nothing */
        continue;
      } else
        validLatchFound = true;

      /* Does the user want too much data? */
      if ((curStartBit + curNumBits - 1) > latchit->latchEndBit)
        curNumBits = latchit->latchEndBit - curStartBit + 1;

      /* was the startbit before this latch ? */
      if (curStartBit < latchit->latchStartBit) {
        curNumBits -= (latchit->latchStartBit - curStartBit);
        curStartBit = latchit->latchStartBit;
      }

      /* Let's apply our data */
      rc = ecmdApplyDataModifier(latchit->buffer, buffer, curStartBit - latchit->latchStartBit, dataModifier);
      if (rc) {
        printed = "putlatch - Error occurred inserting data of " + latchit->latchName + " on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }
      
      /* We can do a full latch compare here now to make sure we don't cause matching problems */
      if (ringName.length() != 0) 
        rc = putLatch(target, ringName.c_str(), latchit->latchName.c_str(), latchit->buffer, curStartBit, curNumBits, matchs, ECMD_LATCHMODE_FULL);
      else
        rc = putLatch(target, NULL, latchit->latchName.c_str(), latchit->buffer, curStartBit, curNumBits, matchs,  ECMD_LATCHMODE_FULL);
      if (rc) {
        printed = "putlatch - Error occurred performing putlatch of " + latchit->latchName + " on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      } else if (matchs > 1) {
        printed = "putlatch - Error occurred performing putlatch, multiple matchs found on write, data corruption may have occurred on " + ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        rc = ECMD_FAILURE;
        return rc;
      }

    }

    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      printed = ecmdWriteTarget(target) + "\n";
      ecmdOutput(printed.c_str());
    }
    
    if (!validLatchFound) {
      ecmdOutputError("putlatch - Unable to find a latch with the given startbit\n");
      return ECMD_INVALID_LATCHNAME;
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
  bool foundProblem;                    ///< Did we find a mismatch ?

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
  char outstr[300];
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
      foundProblem = false;

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
          printed = "checkrings - Error occurred performing putring on ";
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
          printed = "checkrings - Error occurred performing getring on ";
          printed += ecmdWriteTarget(target) + "\n";
          ecmdOutputError( printed.c_str() );
          return rc;
        }

        if (ringBuffer.getWord(0) != pattern) {
          sprintf(outstr, "checkrings - Data fetched from ring %s did not match Pattern: %.08X Data: %.08X\n", ringName.c_str(), pattern, ringBuffer.getWord(0));
          ecmdOutputWarning( outstr );
          printed = "checkrings - Error occurred performing checkring on " + ecmdWriteTarget(target) + "\n";
          ecmdOutputWarning( printed.c_str() );
        }
        else {
          /* Walk the ring looking for errors */
          /* We need to not check the very last bit because it is the access latch and isn't actually scannable BZ#134 */
          for (int bit = 32; bit < ringBuffer.getBitLength() - 1; bit ++ ) {
            if (i % 2) {
              if (ringBuffer.isBitSet(bit)) {
                sprintf(outstr,"checkrings - Non-one bits found in 1's ring test at bit %d for ring %s\n", bit, ringName.c_str());
                ecmdOutputWarning( outstr );
                foundProblem = true;
              }
            } else {
              if (ringBuffer.isBitClear(bit)) {
                sprintf(outstr,"checkrings - Non-zero bits found in 0's ring test at bit %d for ring %s\n", bit, ringName.c_str());
                ecmdOutputWarning( outstr);
                foundProblem = true;
              }
            }
          }
          if (foundProblem) {
            printed = "checkrings - Error occurred performing a checkring on " + ecmdWriteTarget(target) + "\n";
            ecmdOutputWarning( printed.c_str() );
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
      printed = "putpattern - Error occurred retrieving scan ring data on ";
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
        printed = "putpattern - Error occurred performing putring on ";
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



