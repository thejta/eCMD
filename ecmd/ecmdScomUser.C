// Copyright ***********************************************************
//                                                                      
// File ecmdDaScomUser.C                                   
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 1996                                         
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
#define ecmdDaScomUser_C
#include <stdio.h>
#include <time.h>
#include <fstream>
#include <unistd.h>

#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <ecmdInterpreter.H>
#include <ecmdSharedUtils.H>

#ifndef FIPSODE
# include <sedcScomdefParser.H>
# include <sedcScomdefClasses.H>
#endif


#undef ecmdDaScomUser_C
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
uint32_t readScomDefFile(uint32_t address, std::ifstream &scomdefFile);

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------

uint32_t ecmdGetScomUser(int argc, char* argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  uint32_t e_rc = ECMD_SUCCESS;                 ///< Expect rc

  bool expectFlag = false;
  bool maskFlag = false;
  bool verboseFlag = false;
  char* expectPtr = NULL;                       ///< Pointer to expected data in arg list
  char* maskPtr = NULL;                         ///< Pointer to mask data in arg list
  ecmdDataBuffer expected;                      ///< Buffer to store expected data
  ecmdDataBuffer mask;                          ///< Buffer for mask of expected data
  std::string outputformat = "x";               ///< Output Format to display
  std::string inputformat = "x";                ///< Input format of data
  ecmdChipTarget target;                        ///< Current target being operated on
  ecmdDataBuffer buffer;                        ///< Buffer to hold scom data
  bool validPosFound = false;                   ///< Did the looper find anything?
  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string printed;                          ///< Output data
  std::string scomdefFileStr;                   ///< Full Path to the Scomdef file
#ifndef FIPSODE
  sedcScomdefEntry scomEntry;                   ///< Returns a class containing the scomdef entry read from the file
#endif
  std::vector<std::string> errMsgs;             ///< Any error messages to go with a array that was marked invalid
  unsigned int runtimeFlags=0;                    ///< Directives on how to parse
  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  //expect and mask flags check
  if ((expectPtr = ecmdParseOptionWithArgs(&argc, &argv, "-exp")) != NULL) {
    expectFlag = true;

    if ((maskPtr = ecmdParseOptionWithArgs(&argc, &argv, "-mask")) != NULL) {
      maskFlag = true;
    }
  }

  if (ecmdParseOption(&argc, &argv, "-v")) {
    verboseFlag = true;
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
  if (argc < 2) {  //chip + address
    ecmdOutputError("getscom - Too few arguments specified; you need at least a chip and an address.\n");
    ecmdOutputError("getscom - Type 'getscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get address to fetch
  
  if (!ecmdIsAllHex(argv[1])) {
    ecmdOutputError("getscom - Non-hex characters detected in address field\n");
    return ECMD_INVALID_ARGS;
  } else if (strlen(argv[1]) > 6) {
    ecmdOutputError("getscom - Scom addresses must be <= 24 bits in length\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t address = ecmdGenB32FromHexRight(&address, argv[1]);


  if (expectFlag) {

    rc = ecmdReadDataFormatted(expected, expectPtr, inputformat);
    if (rc) {
      ecmdOutputError("getscom - Problems occurred parsing expected data, must be an invalid format\n");
      return rc;
    }

    if (maskFlag) {
      rc = ecmdReadDataFormatted(mask, maskPtr, inputformat);
      if (rc) {
        ecmdOutputError("getscom - Problems occurred parsing mask data, must be an invalid format\n");
        return rc;
      }

    }


  }
  if (argc > 2) {
    ecmdOutputError("getscom - Too many arguments specified; you probably added an option that wasn't recognized.\n");
    ecmdOutputError("getscom - Type 'getscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  
  
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;


  while ( ecmdConfigLooperNext(target, looperdata) ) {
    rc = getScom(target, address, buffer);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "getscom - Error occured performing getscom on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }

    if (expectFlag) {

      if (maskFlag) {
        buffer.setAnd(mask, 0, buffer.getBitLength());
      }

      if (!ecmdCheckExpected(buffer, expected)) {

        //@ make this stuff sprintf'd
        char outstr[50];
        printed = ecmdWriteTarget(target);
        sprintf(outstr, "\ngetscom - Data miscompare occured at address: %.8X\n", address);
        printed += outstr;
        ecmdOutputError( printed.c_str() );


        printed = "getscom - Actual";
        if (maskFlag) {
          printed += " (with mask): ";
        }
        else {
          printed += "            : ";
        }

        printed += ecmdWriteDataFormatted(buffer, outputformat);
        ecmdOutputError( printed.c_str() );

        printed = "getscom - Expected          : ";
        printed += ecmdWriteDataFormatted(expected, outputformat);
        ecmdOutputError( printed.c_str() );
        e_rc = ECMD_EXPECT_FAILURE;
      }

    }
    else {

      printed = ecmdWriteTarget(target);
      printed += ecmdWriteDataFormatted(buffer, outputformat);
      ecmdOutput( printed.c_str() );
      
      #ifndef FIPSODE
      if (verboseFlag && !expectFlag) {
        
	rc = ecmdQueryFileLocation(target, ECMD_FILE_SCOMDATA, scomdefFileStr);
        if (rc) {
          printed = "getScom - Error occured locating scomdef file: " + scomdefFileStr + "\nSkipping -v parsing\n";
          ecmdOutputWarning(printed.c_str());
	  rc = 0;
          continue;
        }
      
        std::ifstream scomdefFile(scomdefFileStr.c_str());
        if(scomdefFile.fail()) {
          printed = "readScomdefFile - Error occured opening scomdef file: " + scomdefFileStr + "\nSkipping -v parsing\n";
          ecmdOutputWarning(printed.c_str());
	  rc = 0;	     
          continue;
        }
        rc = readScomDefFile(address, scomdefFile);
        if (rc == ECMD_SCOMADDRESS_NOT_FOUND) {
	  ecmdOutputWarning("Skipping -v parsing\n");
	  rc = 0;
	  continue;
        }
        scomEntry = sedcScomdefParser(scomdefFile, errMsgs, runtimeFlags);
    
	
    	std::list< std::string >::iterator descIt;
    	std::list<sedcScomdefDefLine>::iterator definIt;
	std::list< std::string >::iterator bitDetIt;
	char bitDesc[1000];
    	
    	sprintf(bitDesc,"Name       : %20s%s\nDesc       : %20s", " ",scomEntry.name.c_str()," ");  
	ecmdOutput(bitDesc);
	
    	for (descIt = scomEntry.description.begin(); descIt != scomEntry.description.end(); descIt++) {
    	  sprintf(bitDesc,"%s", descIt->c_str());
	  ecmdOutput(bitDesc);
    	}
	ecmdOutput("\n");
	for (definIt = scomEntry.definition.begin(); definIt != scomEntry.definition.end(); definIt++) {
	  if(definIt->rhsNum == -1) {
    	    sprintf(bitDesc, "Bit(%d)", definIt->lhsNum);
    	  }
    	  else {
    	    sprintf(bitDesc, "Bit(%d:%d)", definIt->lhsNum,definIt->rhsNum);
    	  }
	  sprintf(bitDesc, "%-10s : ",bitDesc);
	  ecmdOutput(bitDesc);
	  
	  if (definIt->length <= 8) {
	    std::string binstr = buffer.genBinStr(definIt->lhsNum, definIt->length);
	    sprintf(bitDesc, "0b%-16s  %s\n",binstr.c_str(),definIt->dialName.c_str());
	  }
	  else {
	    std::string hexLeftStr = buffer.genHexLeftStr(definIt->lhsNum, definIt->length);
	    sprintf(bitDesc, "0x%-16s  %s\n",hexLeftStr.c_str(),definIt->dialName.c_str());
	  }
	  ecmdOutput(bitDesc);
	  std::string bitDescStr;
    	  for (bitDetIt = definIt->detail.begin(); bitDetIt != definIt->detail.end(); bitDetIt++) {
	    sprintf(bitDesc, "%32s ", " ");
	    //Would print the entires string no matter how long it is
	    //bitDescStr = (std::string)bitDesc + *bitDetIt +"\n";
	    //ecmdOutput(bitDescStr.c_str());
	    
	    std::string tmpstr;
	    int curptr =0, len, maxdesclen=80;
	    while (curptr < (*bitDetIt).length()) {
	      if (((*bitDetIt).length() - curptr) < maxdesclen) {
	       len = (*bitDetIt).length() - curptr;
	      }
	      else {
	       len = maxdesclen;
	      }
	      tmpstr = (*bitDetIt).substr(curptr,len);
	      bitDescStr = (std::string)bitDesc + tmpstr + "\n";
	      ecmdOutput(bitDescStr.c_str());
	      curptr += len;
	    }
    	    
    	  }
    	  
    	}
      }
      #endif
    }
  }


  if (!validPosFound) {
    //this is an error common across all UI functions
    ecmdOutputError("getscom - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  /* If we failed an expect let's return that */
  if (e_rc) return e_rc;

  return rc;
}

uint32_t ecmdPutScomUser(int argc, char* argv[]) {

  uint32_t rc = ECMD_SUCCESS;
  std::string inputformat = "x";                ///< Default input format
  std::string dataModifier = "insert";          ///< Default data Modifier (And/Or/insert)
  ecmdDataBuffer fetchBuffer;                   ///< Buffer to store read/modify/write data
  ecmdLooperData looperdata;                    ///< Store internal Looper data
  ecmdChipTarget target;                        ///< Chip target being operated on
  uint32_t address;                             ///< Scom address
  ecmdDataBuffer buffer;                        ///< Container to store write data
  bool validPosFound = false;                   ///< Did the config looper actually find a chip ?
  std::string printed;                          ///< String for printed data
  int startbit = -1;                            ///< Startbit to insert data
  int numbits = 0;                              ///< Number of bits to insert data

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  char* formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    inputformat = formatPtr;
  }

  formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-b");
  if (formatPtr != NULL) {
    dataModifier = formatPtr;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args,                                           */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/

  if (argc < 3) {  //chip + address + some data
    ecmdOutputError("putscom - Too few arguments specified; you need at least a chip, an address, and some data.\n");
    ecmdOutputError("putscom - Type 'putscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  if (!ecmdIsAllHex(argv[1])) {
    ecmdOutputError("putscom - Non-hex characters detected in address field\n");
    return ECMD_INVALID_ARGS;
  } else if (strlen(argv[1]) > 6) {
    ecmdOutputError("putscom - Scom addresses must be <= 24 bits in length\n");
    return ECMD_INVALID_ARGS;
  }
  address = ecmdGenB32FromHexRight(&address, argv[1]);

  /* Did they specify a start/numbits */
  if (argc > 3) {
    if (argc != 5) {
      ecmdOutputError("putscom - Too many arguments specified; you probably added an unsupported option.\n");
      ecmdOutputError("putscom - Type 'putscom -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }

    if (!ecmdIsAllDecimal(argv[2])) {
      ecmdOutputError("putscom - Non-decimal characters detected in startbit field\n");
      return ECMD_INVALID_ARGS;
    }
    startbit = atoi(argv[2]);
    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("putscom - Non-decimal characters detected in numbits field\n");
      return ECMD_INVALID_ARGS;
    }
    numbits = atoi(argv[3]);


    /* Bounds check */
    if ((startbit + numbits) > ECMD_MAX_DATA_BITS) {
      char errbuf[100];
      sprintf(errbuf,"putscom - Too much data requested > %d bits\n", ECMD_MAX_DATA_BITS);
      ecmdOutputError(errbuf);
      return ECMD_DATA_BOUNDS_OVERFLOW;
    } else if (numbits == 0) {
      ecmdOutputError("putscom - Number of bits == 0, operation not performed\n");
      return ECMD_INVALID_ARGS;
    }

    rc = ecmdReadDataFormatted(buffer, argv[4], inputformat, numbits);
    if (rc) {
      ecmdOutputError("putscom - Problems occurred parsing input data, must be an invalid format\n");
      return rc;
    }
    
    
  } else {  

    rc = ecmdReadDataFormatted(buffer, argv[2], inputformat);
    if (rc) {
      ecmdOutputError("putscom - Problems occurred parsing input data, must be an invalid format\n");
      return rc;
    }
  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdConfigLooperNext(target, looperdata)) {

    /* Do we need to perform a read/modify/write op ? */
    if ((dataModifier != "insert") || (startbit != -1)) {


      rc = getScom(target, address, fetchBuffer);

      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
      }
      else if (rc) {
        printed = "putscom - Error occured performing getscom on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }
      else {
        validPosFound = true;     
      }

      rc = ecmdApplyDataModifier(fetchBuffer, buffer, (startbit == -1 ? 0 : startbit), dataModifier);
      if (rc) return rc;

      rc = putScom(target, address, fetchBuffer);
      if (rc) {
        printed = "putscom - Error occured performing putscom on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }

    }
    else {

      rc = putScom(target, address, buffer);
      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
      }
      else if (rc) {
        printed = "putscom - Error occured performing putscom on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }
      else {
        validPosFound = true;     
      }

    }

    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      printed = ecmdWriteTarget(target) + "\n";
      ecmdOutput(printed.c_str());
    }
  
  }


  if (!validPosFound) {
    ecmdOutputError("putscom - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdPollScomUser(int argc, char* argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  bool expectFlag = false;
  bool maskFlag = false;
  bool verboseFlag = false;
  std::string outputformat = "x";               ///< Output format
  std::string inputformat = "x";                ///< Input format
  ecmdChipTarget target;                        ///< Target we are operating on
  bool validPosFound = false;                   ///< Did the looper find anything?
  ecmdLooperData looperdata;                    ///< Store internal Looper data
  ecmdDataBuffer buffer;                        ///< Store current scom data
  ecmdDataBuffer expected;                      ///< Store expected data
  ecmdDataBuffer mask;                          ///< Store mask data

  uint8_t NONE_T = 0;
  uint8_t SECONDS_T = 1;
  uint8_t CYCLES_T = 2;
  uint8_t ITERATIONS_T = 3;

  uint8_t intervalFlag = NONE_T;
  uint8_t limitFlag = NONE_T;
  uint32_t interval = 5;
  uint32_t maxPolls = 1050;
  uint32_t numPolls = 1;
  uint32_t timerStart = 0x0;

  char * curArg;                                ///< Used for arg parsing


  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/

  //expect and mask flags check
  if ((curArg = ecmdParseOptionWithArgs(&argc, &argv, "-exp")) != NULL) {
    expectFlag = true;

    rc = ecmdReadDataFormatted(expected, curArg, inputformat);
    if (rc) return rc;

    if ((curArg = ecmdParseOptionWithArgs(&argc, &argv, "-mask")) != NULL) {
      maskFlag = true;

      rc = ecmdReadDataFormatted(mask, curArg, inputformat);
      if (rc) return rc;
      
    }
  }

  if (ecmdParseOption(&argc, &argv, "-verbose")) {
    verboseFlag = true;
  }

  curArg = ecmdParseOptionWithArgs(&argc, &argv, "-interval");
  if (curArg != NULL) {
    interval = atoi(curArg);
    if (strstr(curArg, "c")) {
      intervalFlag = CYCLES_T;
    }
    else {
      intervalFlag = SECONDS_T;
    }
  } else {
    /* The default */
    interval = 5;
    intervalFlag = SECONDS_T;
  }
    

  curArg = ecmdParseOptionWithArgs(&argc, &argv, "-limit");
  if (curArg != NULL) {
    maxPolls = atoi(curArg);
    if (strstr(curArg, "s")) {
      limitFlag = SECONDS_T;
    } else if (strstr(curArg, "c")) {
      limitFlag = CYCLES_T;
    } else { 
      limitFlag = ITERATIONS_T;  /* default when using -limit */
    }
  } else {
    maxPolls  = 1000;
    limitFlag = ITERATIONS_T;  /* default when using -limit */
  }
    

  if (limitFlag != ITERATIONS_T && limitFlag != intervalFlag) {
    ecmdOutputError("pollscom - Invalid interval/limit pair.\n");
    ecmdOutputError("pollscom - Type 'pollscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

#ifdef REMOVE_SIM
  if (intervalFlag == CYCLES_T) {
    ecmdOutputError("pollscom - Can't use cycles in non-simulation mode");
    return ECMD_INVALID_ARGS;
  }
#endif

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
  if (argc < 2) {  //chip + address
    ecmdOutputError("pollscom - Too few arguments specified; you need at least a chip and an address.\n");
    ecmdOutputError("pollscom - Type 'pollscom -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //get address to fetch
  if (!ecmdIsAllHex(argv[1])) {
    ecmdOutputError("pollscom - Non-hex characters detected in address field\n");
    return ECMD_INVALID_ARGS;
  } else if (strlen(argv[1]) > 6) {
    ecmdOutputError("pollscom - Scom addresses must be <= 24 bits in length\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t address = ecmdGenB32FromHexRight(&address, argv[1]);


  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  std::string printed;

  while (ecmdConfigLooperNext(target, looperdata)) {

    bool done = false;
    timerStart = time(NULL);

    printed = ecmdWriteTarget(target);
    char outstr[30];
    sprintf(outstr, "Polling address %.6X...\n", address);
    printed += outstr;
    ecmdOutput( printed.c_str() );

    while (!done) {
 
      rc = getScom(target, address, buffer);
      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        break;
      }
      else if (rc) {
        printed = "pollscom - Error occured performing getscom on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }
      else {
        validPosFound = true;     
      }

      /* ------------------------ */
      /* check for last iteration */
      /* ------------------------ */
      if ((limitFlag == ITERATIONS_T || limitFlag == CYCLES_T) && numPolls >= maxPolls) done = 1;
      else if (limitFlag == SECONDS_T && maxPolls != 0 && ((uint32_t) time(NULL) > timerStart + maxPolls)) done = 1;

      if (expectFlag) {

        if (maskFlag) {
          buffer.setAnd(mask, 0, buffer.getBitLength());
        }

        if (!ecmdCheckExpected(buffer, expected)) {

          //mismatches
          if (done || verboseFlag) {

            printed = "pollscom - Actual";
            if (maskFlag) {
              printed += " (with mask): ";
            }
            else {
              printed += "            : ";
            }

            printed += ecmdWriteDataFormatted(buffer, outputformat);

            printed += "pollscom - Expected          : ";
            printed += ecmdWriteDataFormatted(expected, outputformat);

            if (done) {
              char outstr[50];
              sprintf(outstr, "pollscom - Data miscompare occured at address: %.8X\n", address);
              printed = outstr + printed;
              ecmdOutputError( printed.c_str() );
            }
            else {
              ecmdOutput( printed.c_str() );
            }

          }
            
        }
        else {
          done = 1;  //matches
        }

      }
      else {

        printed = "Actual            : ";
        printed += ecmdWriteDataFormatted(buffer, outputformat);

        if (done) {
          printed += ecmdWriteTarget(target);
          printed += "\tPolling Complete\n";
        }

        ecmdOutput( printed.c_str() );
      }

      //update poll counters
      if (limitFlag == ITERATIONS_T) {
        numPolls++;

        if (intervalFlag == CYCLES_T) {
#ifndef REMOVE_SIM
          rc = simclock(interval);
          if (rc) return rc;
#endif
        } else if (intervalFlag == SECONDS_T) {
          sleep(interval);
        }

      }
      else if (limitFlag == CYCLES_T && intervalFlag == CYCLES_T) {

#ifndef REMOVE_SIM
        numPolls += interval;
        rc = simclock(interval);
        if (rc) return rc;
#endif

      }
      else if (limitFlag == SECONDS_T && intervalFlag == SECONDS_T) {
        sleep(interval);
      }
      else {

        ecmdOutputError("pollscom - Invalid limit/interval argument pair");
        return ECMD_INVALID_ARGS;

      }
        
    
    
      
    }  //while (!done)

  }


  if (!validPosFound) {
    ecmdOutputError("pollscom - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t readScomDefFile(uint32_t address, std::ifstream &scomdefFile) {
  uint32_t rc = ECMD_SUCCESS;
  std::string scomdefFileStr;                      ///< Full path to scomdef file
  std::string printed;
  
  
  std::string curLine;
  uint32_t beginPtr;
  uint32_t beginLen;

  bool done = false; 
  std::vector<std::string> curArgs(4);
  
  while (getline(scomdefFile, curLine) && !done) {
    //Remove leading whitespace
    uint32_t curStart = curLine.find_first_not_of(" \t", 0);
    if (curStart != std::string::npos) {
      curLine = curLine.substr(curStart,curLine.length());
    }
    if((curLine[0] == 'B') && (curLine.find("BEGIN Scom") != std::string::npos)) {
      beginPtr = scomdefFile.tellg();
      beginLen = curLine.length();
    }
    if((curLine[0] == 'A') && (curLine.find("Address") != std::string::npos)) {
      ecmdParseTokens(curLine, " \t\n={}", curArgs);
      uint32_t addrFromFile = ecmdGenB32FromHexRight(&addrFromFile, curArgs[1].c_str());
      if ((curArgs.size() >= 2) && addrFromFile == address) {
        done = true;
      }
    }
  }
  if (done) {
    scomdefFile.seekg(beginPtr-beginLen-1);
  }
  else {
    ecmdOutputWarning("Unable to find Scom Address in the Scomdef file\n");
    rc = ECMD_SCOMADDRESS_NOT_FOUND;
  }
  return rc;
}


// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
