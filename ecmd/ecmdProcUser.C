/* $Header$ */
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

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <stdio.h>
#include <map>

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

#ifndef ECMD_REMOVE_PROCESSOR_FUNCTIONS
uint32_t ecmdGetSprUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  std::list<ecmdNameEntry> posEntries;       ///< List of thread spr's to fetch, to use getSprMultiple
  std::map< std::string, std::list<ecmdNameEntry> > cuEntries;       ///< List of spr's to fetch, to use getSprMultiple
  std::map< std::string, std::list<ecmdNameEntry> >::iterator cuEntryIter;
  std::map< std::string, std::list<ecmdNameEntry> > threadEntries;       ///< List of thread spr's to fetch, to use getSprMultiple
  std::map< std::string, std::list<ecmdNameEntry> >::iterator threadEntryIter;
  std::list<ecmdNameEntry>::iterator nameIter;
  ecmdNameEntry  entry;               ///< Spr entry to fetch
  ecmdChipTarget target;                ///< Current target
  ecmdLooperData looperData;            ///< Store internal Looper data
  ecmdChipTarget cuTarget;              ///< Current target
  ecmdLooperData cuLooperData;          ///< Store internal Looper data
  ecmdChipTarget threadTarget;          ///< Current thread target
  ecmdLooperData threadLooperData;      ///< Store internal thread Looper data
  int idx;
  ecmdProcRegisterInfo procInfo; ///< Used to figure out if an SPR is threaded or not
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

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 1) {
    ecmdOutputError("getspr - Too few arguments specified; you need at least one spr.\n");
    ecmdOutputError("getspr - Type 'getspr -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;


  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {

    /* Clear my lists out */
    posEntries.clear();
    cuEntries.clear();
    threadEntries.clear();

    /* Walk through the arguments and create our list of sprs */
    /* We have to re-establish this list on each position because one position may be dd2.0 and another 3.0 and the spr state changed */
    for (idx = 0; idx < argc; idx ++) {
      sprName = argv[idx];

      /* First thing we need to do is find out for this particular target if the SPR is threaded */
      rc = ecmdQueryProcRegisterInfo(target, sprName.c_str(), procInfo);
      if (rc) {
        printed = "getspr - Error occured getting spr info for ";
        printed += sprName;
        printed += " on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
      }

      if (procInfo.mode == ECMD_PROCREG_WRITE_ONLY)
      {
        // Error! Trying to read/get Write-Only Register 
        printed = "getspr - Can't Read From Write-Only Register '";
        printed += sprName + "' \n";
        ecmdOutputError( printed.c_str() );
        coeRc = ECMD_READING_FROM_WRITE_ONLY_REG;
        continue;
      }

      /* If it's thread, push onto one list.  Otherwise, push onto another */
      entry.name = sprName;
      if (procInfo.isChipUnitRelated) {
        if (procInfo.threadReplicated) {
          threadEntries[procInfo.relatedChipUnit].push_back(entry);
        } else {
          cuEntries[procInfo.relatedChipUnit].push_back(entry);
        }
      } else {
        posEntries.push_back(entry);
      }
    }


    /* Now go through and get all of our pos spr's */
    if (posEntries.size()) {

      /* Actually go fetch the data */
      rc = getSprMultiple(target, posEntries);
      if (rc) {
        printed = "getspr - Error occured performing getSprMultiple on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
      }
      else {
        validPosFound = true;     
      }

      printed = ecmdWriteTarget(target) + "\n";
      ecmdOutput( printed.c_str() );
      for (nameIter = posEntries.begin(); nameIter != posEntries.end(); nameIter ++) {

        printed = nameIter->name + "\t";

        printed += ecmdWriteDataFormatted(nameIter->buffer, format);

        ecmdOutput( printed.c_str() );
      }
      ecmdOutput("\n");
    }

    /* Now go through and get all of our chipUnit spr's */
    if (!cuEntries.empty()) {

      for (cuEntryIter = cuEntries.begin(); cuEntryIter != cuEntries.end(); cuEntryIter++) {
        /* Setup our target */
        cuTarget = target;
        if (cuEntryIter->first != "") {
          cuTarget.chipUnitType = cuEntryIter->first;
          cuTarget.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
        }
        cuTarget.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
        cuTarget.threadState = ECMD_TARGET_FIELD_UNUSED;

        /* Init the chipUnit loop */
        rc = ecmdLooperInit(cuTarget, ECMD_SELECTED_TARGETS_LOOP, cuLooperData);
        if (rc) return rc;

        /* If this isn't a chipUnit spr we will fall into while loop and break at the end, if it is we will call run through configloopernext */
        while (ecmdLooperNext(cuTarget, cuLooperData) && (!coeRc || coeMode)) {

          /* Actually go fetch the data */
          rc = getSprMultiple(cuTarget, cuEntryIter->second);
          if (rc) {
            printed = "getspr - Error occured performing getSprMultiple on ";
            printed += ecmdWriteTarget(cuTarget) + "\n";
            ecmdOutputError( printed.c_str() );
            coeRc = rc;
            continue;
          }
          else {
            validPosFound = true;     
          }

          printed = ecmdWriteTarget(cuTarget) + "\n";
          ecmdOutput( printed.c_str() );
          for (nameIter = cuEntryIter->second.begin(); nameIter != cuEntryIter->second.end(); nameIter++) {

            printed = nameIter->name + "\t";

            printed += ecmdWriteDataFormatted(nameIter->buffer, format);

            ecmdOutput( printed.c_str() );
          }
          ecmdOutput("\n");
        }
      }
    }

    /* Now go through and get all of our chipUnit spr's */
    if (!threadEntries.empty()) {

      for (threadEntryIter = threadEntries.begin(); threadEntryIter != threadEntries.end(); threadEntryIter++) {
        /* Setup our target */
        threadTarget = target;
        if (threadEntryIter->first != "") {
          threadTarget.chipUnitType = threadEntryIter->first;  //@SJ-fixed with defect 649018
          threadTarget.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
        }
        threadTarget.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
        threadTarget.threadState = ECMD_TARGET_FIELD_WILDCARD;

        /* Init the thread loop */
        rc = ecmdLooperInit(threadTarget, ECMD_SELECTED_TARGETS_LOOP, threadLooperData);
        if (rc) return rc;

        /* If this isn't a thread spr we will fall into while loop and break at the end, if it is we will call run through configloopernext */
        while (ecmdLooperNext(threadTarget, threadLooperData) && (!coeRc || coeMode)) {

          /* Actually go fetch the data */
          rc = getSprMultiple(threadTarget, threadEntryIter->second);
          if (rc) {
            printed = "getspr - Error occured performing getSprMultiple on ";
            printed += ecmdWriteTarget(threadTarget) + "\n";
            ecmdOutputError( printed.c_str() );
            coeRc = rc;
            continue;
          }
          else {
            validPosFound = true;     
          }

          printed = ecmdWriteTarget(threadTarget) + "\n";
          ecmdOutput( printed.c_str() );
          for (nameIter = threadEntryIter->second.begin(); nameIter != threadEntryIter->second.end(); nameIter++) {

            printed = nameIter->name + "\t";

            printed += ecmdWriteDataFormatted(nameIter->buffer, format);

            ecmdOutput( printed.c_str() );
          }
          ecmdOutput("\n");
        }
      }
    }

  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("getspr - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdPutSprUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  ecmdLooperData looperData;    ///< Store internal Looper data
  ecmdChipTarget subTarget;        ///< Current target
  ecmdLooperData subLooperData;    ///< Store internal Looper data
  std::string inputformat = "x";                ///< Default input format
  std::string dataModifier = "insert";          ///< Default data Modifier (And/Or/insert)
  ecmdDataBuffer cmdlineBuffer;        ///< Buffer to store data to write with
  ecmdDataBuffer buffer;     ///< Buffer to store data from the spr
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  std::string sprName;          ///< Name of spr to write 
  uint32_t startBit = ECMD_UNSET; ///< Startbit to insert data
  uint32_t numBits = 0;         ///< Number of bits to insert data
  ecmdProcRegisterInfo procInfo; ///< Used to figure out if an SPR is threaded or not 
  ecmdChipTarget threadTarget;        ///< Current thread target
  ecmdLooperData threadLooperData;    ///< Store internal thread Looper data
  char* cmdlinePtr = NULL;            ///< Pointer to data in argv array

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

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 2) {
    ecmdOutputError("putspr - Too few arguments specified; you need at least an sprName  and some data.\n");
    ecmdOutputError("putspr - Type 'putspr -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

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


    rc = ecmdReadDataFormatted(cmdlineBuffer, argv[3], inputformat);
    if (rc) {
      printed = "putspr - Problems occurred parsing input data, must be an invalid format\n";
      ecmdOutputError(printed.c_str());
      return rc;
    }
  } else if (argc == 2) {

    cmdlinePtr = argv[1];

  } else {
    ecmdOutputError("putspr - Too many arguments specified; you probably added an option that wasn't recognized.\n");
    ecmdOutputError("putspr - Type 'putspr -h' for usage.\n");
    return ECMD_INVALID_ARGS;

  }


  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {

    /* First thing we need to do is find out for this particular target if the SPR is threaded */
    rc = ecmdQueryProcRegisterInfo(target, sprName.c_str(), procInfo);
    if (rc) {
      printed = "putspr - Error occured getting spr info for ";
      printed += sprName;
      printed += " on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }

    if (procInfo.mode == ECMD_PROCREG_READ_ONLY)
    {
      // Error! Trying to read/get Write-Only Register 
      printed = "putspr - Can't Write to Read-Only Register '";
      printed += sprName + "' \n";
      ecmdOutputError( printed.c_str() );
      coeRc = ECMD_WRITING_TO_READ_ONLY_REG;
      continue;
    }

    /* If we have a cmdlinePtr, read it in now that we have a length we can use */
    if (cmdlinePtr != NULL) {
      if (dataModifier == "insert") {
        rc = ecmdReadDataFormatted(buffer, cmdlinePtr, inputformat, procInfo.bitLength);
      } else {
        rc = ecmdReadDataFormatted(cmdlineBuffer, cmdlinePtr, inputformat, procInfo.bitLength);
      }
      if (rc) {
        ecmdOutputError("putspr - Problems occurred parsing input data, must be an invalid format\n");
        coeRc = rc;
        continue;
      }
    }

    // We've done the chipUnit loop and gotten the SPR info, now figure out how to loop.
    subTarget = target;
    if (procInfo.isChipUnitRelated) {
      if (procInfo.relatedChipUnit != "") {
        subTarget.chipUnitType = procInfo.relatedChipUnit;
        subTarget.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
      }
      subTarget.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
      if (procInfo.threadReplicated) {
        subTarget.threadState = ECMD_TARGET_FIELD_WILDCARD;
      }
    }

    rc = ecmdLooperInit(subTarget, ECMD_SELECTED_TARGETS_LOOP, subLooperData);
    if (rc) break;

    while (ecmdLooperNext(subTarget, subLooperData) && (!coeRc || coeMode)) {

      /* The user did the r/m/w version, so we need to do a get spr */
      /* Do we need to perform a read/modify/write op ? */
      if ((dataModifier != "insert") || (startBit != ECMD_UNSET)) {
        rc = getSpr(subTarget, sprName.c_str(), buffer);
        if (rc) {
          printed = "putspr - Error occurred performing getspr on ";
          printed += ecmdWriteTarget(subTarget) + "\n";
          ecmdOutputError( printed.c_str() );
          coeRc = rc;
          continue;
        }
        
        rc = ecmdApplyDataModifier(buffer, cmdlineBuffer, (startBit == ECMD_UNSET ? 0 : startBit), dataModifier);
        if (rc) {
          coeRc = rc;
          continue;
        }
      }

      rc = putSpr(subTarget, sprName.c_str(), buffer);
      if (rc) {
        printed = "putspr - Error occured performing putspr on ";
        printed += ecmdWriteTarget(subTarget) + "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
      } else {
        validPosFound = true;     
      }

      if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
        printed = ecmdWriteTarget(subTarget) + "\n";
        ecmdOutput(printed.c_str());
      }
    }
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  //this is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("putspr - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}



uint32_t ecmdGetGprFprUser(int argc, char * argv[], ECMD_DA_TYPE daType) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdChipTarget subTarget;        ///< Current target
  ecmdLooperData subLooperdata;            ///< Store internal Looper data
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  std::list<ecmdIndexEntry> entries;    ///< List of gpr's to fetch, to use getGprMultiple
  std::list<ecmdIndexEntry> entries_copy;    ///< List of gpr's to fetch, to use getGprMultiple
  ecmdIndexEntry  entry;         ///< Gpr entry to fetch
  int idx;
  std::string function;         ///< What function are we running (based on type)
  int numEntries = 1;           ///< Number of consecutive entries to retrieve
  int startEntry = 0;           ///< Entry to start on
  char buf[100];                ///< Temporary string buffer
  std::string sprName;
  ecmdProcRegisterInfo procInfo; ///< Used to figure out if an SPR is threaded or not 

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr == NULL) {
    format = "x";
  }
  else {
    format = formatPtr;
  }

  // Set commandline name based up on the type
  if (daType == ECMD_GPR) {
    function = "getgpr";
    sprName = "gpr";
  } else {
    function = "getfpr";
    sprName = "fpr";
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
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  /* Walk through the arguments and create our list of gprs */
  startEntry = atoi(argv[0]);
  if (argc > 1) {
    numEntries = atoi(argv[1]);
  }

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    /* Clear out our list */
    entries.clear();

    /* First thing we need to do is find out for this particular target if the SPR is threaded */
    rc = ecmdQueryProcRegisterInfo(target, sprName.c_str(), procInfo);
    if (rc) {
      printed = function + " - Error occured getting info for ";
      printed += sprName;
      printed += " on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }

    if ((startEntry + numEntries) > (int)procInfo.totalEntries) {
      printed = function + " - Num Entries requested exceeds maximum number available on: ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = ECMD_INVALID_ARGS;
      continue;
    }

    for (idx = startEntry; idx < startEntry + numEntries; idx ++) {
      entry.index = idx;
      entries.push_back(entry);
    }

    /* Now setup our chipUnit/thread loop */
    subTarget = target;
    if (procInfo.isChipUnitRelated) {
      if (procInfo.relatedChipUnit != "") {
        subTarget.chipUnitType = procInfo.relatedChipUnit;
        subTarget.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
      }
      subTarget.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
      if (procInfo.threadReplicated) {
        subTarget.threadState = ECMD_TARGET_FIELD_WILDCARD;
      }
    }

    rc = ecmdLooperInit(subTarget, ECMD_SELECTED_TARGETS_LOOP, subLooperdata);
    if (rc) {
      coeRc = rc;
      continue;
    }

    while (ecmdLooperNext(subTarget, subLooperdata) && (!coeRc || coeMode)) {

      /* Restore to our initial list */
      entries_copy = entries;

      /* Actually go fetch the data */
      if (daType == ECMD_GPR)
        rc = getGprMultiple(subTarget, entries_copy);
      else
        rc = getFprMultiple(subTarget, entries_copy);

      if (rc) {
        printed = function + " - Error occured performing getMultiple on ";
        printed += ecmdWriteTarget(subTarget) + "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
      }
      else {
        validPosFound = true;     
      }

      printed = ecmdWriteTarget(subTarget) + "\n";
      ecmdOutput( printed.c_str() );
      for (std::list<ecmdIndexEntry>::iterator nameIter = entries_copy.begin(); nameIter != entries_copy.end(); nameIter ++) {

        sprintf(buf,"%02d\t", nameIter->index);
        printed = buf;

        printed += ecmdWriteDataFormatted(nameIter->buffer, format);

        ecmdOutput( printed.c_str() );
      }

      ecmdOutput("\n");
    }
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    printed = function + " - Unable to find a valid chip to execute command on\n";
    ecmdOutputError(printed.c_str());
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdPutGprFprUser(int argc, char * argv[], ECMD_DA_TYPE daType) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  ecmdLooperData looperdata;    ///< Store internal Looper data
  ecmdChipTarget subTarget;        ///< Current target
  ecmdLooperData subLooperdata;            ///< Store internal Looper data
  std::string inputformat = "x";                ///< Default input format
  std::string dataModifier = "insert";          ///< Default data Modifier (And/Or/insert)
  ecmdDataBuffer cmdlineBuffer; ///< Buffer to store data to write with
  ecmdDataBuffer buffer;        ///< Buffer to store data from the read
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  uint32_t entry;               ///< Index entry to write 
  uint32_t startBit = ECMD_UNSET; ///< Startbit to insert data
  uint32_t numBits = 0;         ///< Number of bits to insert data
  std::string function;         ///< Current function being run based on daType
  std::string sprName;
  char* cmdlinePtr = NULL;         ///< Pointer to data in argv array
  ecmdProcRegisterInfo procInfo; ///< Used to figure out if an SPR is threaded or not

  /* get format flag, if it's there */
  char* formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    inputformat = formatPtr;
  }

  formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-b");
  if (formatPtr != NULL) {
    dataModifier = formatPtr;
  }

  // Set commandline name based up on the type
  if (daType == ECMD_GPR) {
    function = "getgpr";
    sprName = "gpr";
  } else {
    function = "getfpr";
    sprName = "fpr";
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
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

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
    
    rc = ecmdReadDataFormatted(cmdlineBuffer, argv[3], inputformat);
    if (rc) {
      printed = function + "Problems occurred parsing input data, must be an invalid format\n";
      ecmdOutputError(printed.c_str());
      return rc;
    }
  } else if (argc == 2) {

    cmdlinePtr = argv[1];

  } else {
    printed = function + " - Too many arguments specified; you probably added an option that wasn't recognized.\n";
    ecmdOutputError(printed.c_str());
    printed = function + " - Type '"; printed += function; printed += " -h' for usage.\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
  }

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    /* First thing we need to do is find out for this particular target if the SPR is threaded */
    rc = ecmdQueryProcRegisterInfo(target, sprName.c_str(), procInfo);
    if (rc) {
      printed = function + " - Error occured getting info for ";
      printed += sprName;
      printed += " on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }

    /* If we have a cmdlinePtr, read it in now that we have a length we can use */
    if (cmdlinePtr != NULL) {
      if (dataModifier == "insert") {
        rc = ecmdReadDataFormatted(buffer, cmdlinePtr, inputformat, procInfo.bitLength);
      } else {
        rc = ecmdReadDataFormatted(cmdlineBuffer, cmdlinePtr, inputformat, procInfo.bitLength);
      }
      if (rc) {
        printed = function + " - Problems occurred parsing input data, must be an invalid format\n";
        ecmdOutputError(printed.c_str());
        coeRc = rc;
        continue;
      }
    }

    /* Now setup our chipUnit/thread loop */
    subTarget = target;
    if (procInfo.isChipUnitRelated) {
      if (procInfo.relatedChipUnit != "") {
        subTarget.chipUnitType = procInfo.relatedChipUnit;
        subTarget.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
      }
      subTarget.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
      if (procInfo.threadReplicated) {
        subTarget.threadState = ECMD_TARGET_FIELD_WILDCARD;
      }
    }

    rc = ecmdLooperInit(subTarget, ECMD_SELECTED_TARGETS_LOOP, subLooperdata);
    if (rc) {
      coeRc = rc;
      continue;
    }

    while (ecmdLooperNext(subTarget, subLooperdata) && (!coeRc || coeMode)) {

      /* Do we need to perform a read/modify/write op ? */
      if ((dataModifier != "insert") || (startBit != ECMD_UNSET)) {
        if (daType == ECMD_GPR) {
          rc = getGpr(subTarget, entry, buffer);
        } else {
          rc = getFpr(subTarget, entry, buffer);
        }
        if (rc) {
          printed = function + " - Error occured performing getgpr/fpr on ";
          printed += ecmdWriteTarget(subTarget) + "\n";
          ecmdOutputError( printed.c_str() );
          coeRc = rc;
          continue;
        }

        rc = ecmdApplyDataModifier(buffer, cmdlineBuffer, (startBit == ECMD_UNSET ? 0 : startBit), dataModifier);
        if (rc) {
          coeRc = rc;
          continue;
        }
      }

      if (daType == ECMD_GPR) {
        rc = putGpr(subTarget, entry, buffer);
      } else {
        rc = putFpr(subTarget, entry, buffer);
      }

      if (rc) {
        printed = function + " - Error occured performing command on ";
        printed += ecmdWriteTarget(subTarget) + "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;
        continue;
      } else {
        validPosFound = true;
      }

      if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
        printed = ecmdWriteTarget(subTarget) + "\n";
        ecmdOutput(printed.c_str());
      }
    }
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    printed = function + " - Unable to find a valid chip to execute command on\n";
    ecmdOutputError(printed.c_str());
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}



#endif // ECMD_REMOVE_PROCESSOR_FUNCTIONS
