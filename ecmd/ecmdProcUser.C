// Copyright ***********************************************************
//                                                                      
// File ecmdProcUser.C                                   
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
#include <stdio.h>

#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <ecmdSharedUtils.H>
#include <ecmdInterpreter.H>


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


uint32_t ecmdGetSprUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdChipTarget coreTarget;        ///< Current target
  ecmdLooperData coreLooperData;    ///< Store internal Looper data
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  std::list<ecmdNameEntry> threadEntries;    ///< List of thread spr's to fetch, to use getSprMultiple
  std::list<ecmdNameEntry> coreEntries;      ///< List of core spr's to fetch, to use getSprMultiple
  ecmdNameEntry  entry;               ///< Spr entry to fetch
  ecmdChipTarget threadTarget;        ///< Current thread target
  ecmdLooperData threadLooperData;    ///< Store internal thread Looper data
  int idx;
  ecmdProcRegisterInfo sprInfo; ///< Used to figure out if an SPR is threaded or not
  std::string sprName;

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
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


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 1) {
    ecmdOutputError("getspr - Too few arguments specified; you need at least one spr.\n");
    ecmdOutputError("getspr - Type 'getspr -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  coreTarget.chipType = ECMD_CHIPT_PROCESSOR;
  coreTarget.chipTypeState = ECMD_TARGET_FIELD_VALID;
  coreTarget.cageState = coreTarget.nodeState = coreTarget.slotState = coreTarget.posState = coreTarget.coreState = ECMD_TARGET_FIELD_WILDCARD;
  coreTarget.threadState = ECMD_TARGET_FIELD_UNUSED;


  rc = ecmdConfigLooperInit(coreTarget, ECMD_SELECTED_TARGETS_LOOP, coreLooperData);
  if (rc) return rc;

  while ( ecmdConfigLooperNext(coreTarget, coreLooperData) ) {

    /* Clear my lists out */
    threadEntries.clear();
    coreEntries.clear();

    /* Walk through the arguments and create our list of sprs */
    /* We have to re-establish this list on each position because one position may be dd2.0 and another 3.0 and the spr state changed */
    for (idx = 0; idx < argc; idx ++) {
      sprName = argv[idx];

      /* If it's the two special keywords, handle those before calling ecmdQueryProcRegisterInfo */
      if (sprName == "ALL_SHARED") {
        sprInfo.threadReplicated = 0;
      } else if (sprName == "ALL_THREADED") {
        sprInfo.threadReplicated = 1;
      } else {

        /* First thing we need to do is find out for this particular target if the SPR is threaded */
        rc = ecmdQueryProcRegisterInfo(coreTarget, sprName.c_str(), sprInfo);
        if (rc) {
          printed = "getspr - Error occured getting spr info for ";
          printed += sprName;
          printed += " on ";
          printed += ecmdWriteTarget(coreTarget) + "\n";
          ecmdOutputError( printed.c_str() );
          return rc;
        }
      }

      /* If it's thread, push onto one list.  Otherwise, push onto another */
      entry.name = sprName;
      if (sprInfo.threadReplicated) {
        threadEntries.push_back(entry);
      } else {
        coreEntries.push_back(entry);
      }
    }


    /* Now go through and get all of our core spr's */
    if (coreEntries.size()) {

      /* Actually go fetch the data */
      rc = getSprMultiple(coreTarget, coreEntries);
      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
      }
      else if (rc) {
        printed = "getspr - Error occured performing getSprMultiple on ";
        printed += ecmdWriteTarget(coreTarget) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }
      else {
        validPosFound = true;     
      }

      printed = ecmdWriteTarget(coreTarget) + "\n";
      ecmdOutput( printed.c_str() );
      for (std::list<ecmdNameEntry>::iterator entit = coreEntries.begin(); entit != coreEntries.end(); entit ++) {

        printed = entit->name + "\t";

        printed += ecmdWriteDataFormatted(entit->buffer, format);

        ecmdOutput( printed.c_str() );
      }
      ecmdOutput("\n");
    }

    /* Now go through and get all of our thread spr's */
    if (threadEntries.size()) {

      //Setup the target that will be used to query the system config 
      // Copy everything over, just change the threadState
      threadTarget = coreTarget;
      threadTarget.threadState = ECMD_TARGET_FIELD_WILDCARD;

      rc = ecmdConfigLooperInit(threadTarget, ECMD_SELECTED_TARGETS_LOOP, threadLooperData);
      if (rc) return rc;

      while ( ecmdConfigLooperNext(threadTarget, threadLooperData) ) {

        /* Actually go fetch the data */
        rc = getSprMultiple(threadTarget, threadEntries);
        if (rc == ECMD_TARGET_NOT_CONFIGURED) {
          continue;
        }
        else if (rc) {
          printed = "getspr - Error occured performing getSprMultiple on ";
          printed += ecmdWriteTarget(threadTarget) + "\n";
          ecmdOutputError( printed.c_str() );
          return rc;
        }
        else {
          validPosFound = true;     
        }

        printed = ecmdWriteTarget(threadTarget) + "\n";
        ecmdOutput( printed.c_str() );
        for (std::list<ecmdNameEntry>::iterator entit = threadEntries.begin(); entit != threadEntries.end(); entit ++) {

          printed = entit->name + "\t";

          printed += ecmdWriteDataFormatted(entit->buffer, format);

          ecmdOutput( printed.c_str() );
        }
        ecmdOutput("\n");
      }
    }
  }

  if (!validPosFound) {
    //this is an error common across all UI functions
    ecmdOutputError("getspr - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdPutSprUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdChipTarget coreTarget;        ///< Current target
  ecmdLooperData coreLooperData;    ///< Store internal Looper data
  std::string inputformat = "x";                ///< Default input format
  std::string dataModifier = "insert";          ///< Default data Modifier (And/Or/insert)
  ecmdDataBuffer buffer;        ///< Buffer to store data to write with
  ecmdDataBuffer sprBuffer;     ///< Buffer to store data from the spr
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  std::string sprName;          ///< Name of spr to write 
  uint32_t startBit = ECMD_UNSET; ///< Startbit to insert data
  uint32_t numBits = 0;         ///< Number of bits to insert data
  char* dataPtr = NULL;         ///< Pointer to spr data in argv array
  ecmdProcRegisterInfo sprInfo; ///< Used to figure out if an SPR is threaded or not 
  ecmdChipTarget threadTarget;        ///< Current thread target
  ecmdLooperData threadLooperData;    ///< Store internal thread Looper data
  bool doReadModifyWrite = false;     ///< Do we need to do a get before the put

  /* get format flag, if it's there */
  char* formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    inputformat = formatPtr;
  }

  formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-b");
  if (formatPtr != NULL) {
    doReadModifyWrite = true;
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
  if (argc < 2) {
    ecmdOutputError("putspr - Too few arguments specified; you need at least an sprName  and some data.\n");
    ecmdOutputError("putspr - Type 'putspr -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  coreTarget.chipType = ECMD_CHIPT_PROCESSOR;
  coreTarget.chipTypeState = ECMD_TARGET_FIELD_VALID;
  coreTarget.cageState = coreTarget.nodeState = coreTarget.slotState = coreTarget.posState = coreTarget.coreState = ECMD_TARGET_FIELD_WILDCARD;
  coreTarget.threadState = ECMD_TARGET_FIELD_UNUSED;

  sprName = argv[0];

  if (argc == 4) {
    if (!ecmdIsAllDecimal(argv[1])) {
      ecmdOutputError("putspr - Non-decimal numbers detected in startbit field\n");
      return ECMD_INVALID_ARGS;
    }
    startBit = (uint32_t)atoi(argv[1]);

    if (!ecmdIsAllDecimal(argv[2])) {
      ecmdOutputError("putspr - Non-decimal numbers detected in numbits field\n");
      return ECMD_INVALID_ARGS;
    }
    numBits = (uint32_t)atoi(argv[2]);
    
    
    dataPtr = argv[3];

    // Mark for read/modify/write below
    doReadModifyWrite = true;

  } else if (argc == 2) {

    dataPtr = argv[1];

  } else {
    ecmdOutputError("putspr - Too many arguments specified; you probably added an option that wasn't recognized.\n");
    ecmdOutputError("putspr - Type 'putspr -h' for usage.\n");
    return ECMD_INVALID_ARGS;
    
  }


  rc = ecmdConfigLooperInit(coreTarget, ECMD_SELECTED_TARGETS_LOOP, coreLooperData);
  if (rc) return rc;

  while ( ecmdConfigLooperNext(coreTarget, coreLooperData) ) {

    /* First thing we need to do is find out for this particular target if the SPR is threaded */
    rc = ecmdQueryProcRegisterInfo(coreTarget, sprName.c_str(), sprInfo);
    if (rc) {
      printed = "putspr - Error occured getting spr info for ";
      printed += sprName;
      printed += " on ";
      printed += ecmdWriteTarget(coreTarget) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }

    /* Set the buffer length */
    sprBuffer.setBitLength(sprInfo.bitLength);
    
    /* We now have the length of the SPR, read the data in */
    /* They didn't specify a range */
    if (startBit == ECMD_UNSET ) {
      startBit = 0;
      numBits = sprInfo.bitLength;
    }
    rc = ecmdReadDataFormatted(buffer, dataPtr, inputformat, numBits);
    if (rc) {
      ecmdOutputError("putspr - Problems occurred parsing input data, must be an invalid format\n");
      return rc;
    }

    // We've done the core loop and gotten the SPR info, now we need to loop on threads if this SPR has them.
    // Copy everything over, just change the threadState
    threadTarget = coreTarget;
    threadTarget.threadState = (sprInfo.threadReplicated ? ECMD_TARGET_FIELD_WILDCARD : ECMD_TARGET_FIELD_UNUSED);

    rc = ecmdConfigLooperInit(threadTarget, ECMD_SELECTED_TARGETS_LOOP, threadLooperData);
    if (rc) return rc;

    while ( ecmdConfigLooperNext(threadTarget, threadLooperData) ) {

      /* The user did the r/m/w version, so we need to do a get spr */
      if (doReadModifyWrite) {
        rc = getSpr(threadTarget, sprName.c_str(), sprBuffer);
        if (rc == ECMD_TARGET_NOT_CONFIGURED) {
          continue;
        }
        else if (rc) {
          printed = "putspr - Error occured performing getspr on ";
          printed += ecmdWriteTarget(threadTarget) + "\n";
          ecmdOutputError( printed.c_str() );
          return rc;
        }
        else {
          validPosFound = true;     
        }
      }

      rc = ecmdApplyDataModifier(sprBuffer, buffer,  startBit, dataModifier);
      if (rc) return rc;


      rc = putSpr(threadTarget, sprName.c_str(), sprBuffer);

      if (rc) {
        printed = "putspr - Error occured performing putspr on ";
        printed += ecmdWriteTarget(threadTarget) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      } else {
        validPosFound = true;     
      }

      if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
        printed = ecmdWriteTarget(threadTarget) + "\n";
        ecmdOutput(printed.c_str());
      }
    }
  }

  if (!validPosFound) {
    //this is an error common across all UI functions
    ecmdOutputError("putspr - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}



uint32_t ecmdGetGprFprUser(int argc, char * argv[], ECMD_DA_TYPE daType) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  std::list<ecmdIndexEntry> entries;    ///< List of gpr's to fetch, to use getGprMultiple
  std::list<ecmdIndexEntry> entries_copy;    ///< List of gpr's to fetch, to use getGprMultiple
  ecmdIndexEntry  entry;         ///< Gpr entry to fetch
  ecmdLooperData looperdata;            ///< Store internal Looper data
  int idx;
  std::string function;         ///< What function are we running (based on type)
  int numEntries = 1;           ///< Number of consecutive entries to retrieve
  int startEntry = 0;           ///< Entry to start on
  char buf[100];                ///< Temporary string buffer

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
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


  if (daType == ECMD_GPR)
    function = "getgpr";
  else
    function = "getfpr";

  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 1) {
    printed = function + " - Too few arguments specified; you need at least one gpr/fpr.\n";
    ecmdOutputError(printed.c_str());
    printed = function + " - Type '"; printed += function; printed += " -h' for usage.\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = target.threadState = ECMD_TARGET_FIELD_WILDCARD;


  /* Walk through the arguments and create our list of gprs */
  startEntry = atoi(argv[0]);
  if (argc > 1) {
    numEntries = atoi(argv[1]);
  }
  for (idx = startEntry; idx < startEntry + numEntries; idx ++) {
    entry.index = idx;
    entries.push_back(entry);
  }

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while ( ecmdConfigLooperNext(target, looperdata) ) {

  

    /* Restore to our initial list */
    entries_copy = entries;


    /* Actually go fetch the data */
    if (daType == ECMD_GPR)
      rc = getGprMultiple(target, entries_copy);
    else
      rc = getFprMultiple(target, entries_copy);

    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
      printed = function + " - Error occured performing getMultiple on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    else {
      validPosFound = true;     
    }

    printed = ecmdWriteTarget(target) + "\n";
    ecmdOutput( printed.c_str() );
    for (std::list<ecmdIndexEntry>::iterator entit = entries_copy.begin(); entit != entries_copy.end(); entit ++) {

      sprintf(buf,"%02d\t", entit->index);
      printed = buf;

      printed += ecmdWriteDataFormatted(entit->buffer, format);

      ecmdOutput( printed.c_str() );
    }

    ecmdOutput("\n");
  }

  if (!validPosFound) {
    printed = function + " - Unable to find a valid chip to execute command on\n";
    //this is an error common across all UI functions
    ecmdOutputError(printed.c_str());
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdPutGprFprUser(int argc, char * argv[], ECMD_DA_TYPE daType) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  std::string inputformat = "x";                ///< Default input format
  std::string dataModifier = "insert";          ///< Default data Modifier (And/Or/insert)
  ecmdDataBuffer buffer;        ///< Buffer to store data to write with
  ecmdDataBuffer sprBuffer;     ///< Buffer to store data from the spr
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  ecmdLooperData looperdata;    ///< Store internal Looper data
  uint32_t entry;               ///< Index entry to write 
  uint32_t startBit = ECMD_UNSET; ///< Startbit to insert data
  uint32_t numBits = 0;         ///< Number of bits to insert data
  char* dataPtr = NULL;         ///< Pointer to spr data in argv array
  std::string function;         ///< Current function being run based on daType

  /* get format flag, if it's there */
  char* formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    inputformat = formatPtr;
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


  if (daType == ECMD_GPR)
    function = "putgpr";
  else
    function = "putfpr";


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 2) {
    printed = function + " - Too few arguments specified; you need at least an gpr/fpr Name  and some data.\n";
    ecmdOutputError(printed.c_str());
    printed = function + " - Type '"; printed += function; printed += " -h' for usage.\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = target.threadState = ECMD_TARGET_FIELD_WILDCARD;

  entry = (uint32_t)atoi(argv[0]);

  if (argc == 4) {
    if (!ecmdIsAllDecimal(argv[1])) {
      printed = function + " - Non-decimal numbers detected in startbit field\n";
      ecmdOutputError(printed.c_str());
      return ECMD_INVALID_ARGS;
    }
    startBit = (uint32_t)atoi(argv[1]);

    if (!ecmdIsAllDecimal(argv[2])) {
      printed = function + " - Non-decimal numbers detected in numbits field\n";
      ecmdOutputError(printed.c_str());
      return ECMD_INVALID_ARGS;
    }
    numBits = (uint32_t)atoi(argv[2]);
    
    
    dataPtr = argv[3];

  } else if (argc == 2) {

    dataPtr = argv[1];

  } else {
    printed = function + " - Too many arguments specified; you probably added an option that wasn't recognized.\n";
    ecmdOutputError(printed.c_str());
    printed = function + " - Type '"; printed += function; printed += " -h' for usage.\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
    
  }


  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while ( ecmdConfigLooperNext(target, looperdata) ) {


    if (daType == ECMD_GPR)
      rc = getGpr(target, entry, sprBuffer);
    else
      rc = getFpr(target, entry, sprBuffer);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = function + " - Error occured performing getgpr/fpr on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }

    /* Only do this once */
    if (dataPtr != NULL) {

      /* They didn't specify a range */
      if (startBit == ECMD_UNSET ) {
        startBit = 0;
        numBits = sprBuffer.getBitLength();
      }

      rc = ecmdReadDataFormatted(buffer, dataPtr, inputformat, numBits);
      if (rc) {
        printed = function + " - Problems occurred parsing input data, must be an invalid format\n";
        ecmdOutputError(printed.c_str());
        return rc;
      }

      dataPtr = NULL;
    }

    rc = ecmdApplyDataModifier(sprBuffer, buffer,  startBit, dataModifier);
    if (rc) return rc;


    if (daType == ECMD_GPR)
      rc = putGpr(target, entry, sprBuffer);
    else
      rc = putFpr(target, entry, sprBuffer);

    if (rc) {
      printed = function + " - Error occured performing command on ";
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
    //this is an error common across all UI functions
    printed = function + " - Unable to find a valid chip to execute command on\n";
    ecmdOutputError(printed.c_str());
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}


