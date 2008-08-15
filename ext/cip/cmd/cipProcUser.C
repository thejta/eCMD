/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File cipProcUser.C                                   
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
#include <ecmdClientCapi.H>
#include <cipInterpreter.H>
#include <ecmdStructs.H>
#include <cipClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdSharedUtils.H>
#include <ecmdCommandUtils.H>
#include <stdio.h>

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
#ifndef CIP_REMOVE_INSTRUCTION_FUNCTIONS
uint32_t cipInstructUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  ecmdLooperData looperdata;            ///< Store internal Looper data
  bool executeAll = false;      ///< Run start/stop on all procs
  bool verbose    = false;      ///< Display iar after each step
  ecmdDataBuffer  iarData;      ///< Data read from IAR
  int  steps = 1;               ///< Number of steps to run
  ecmdChipData chipData;        ///< So we can determine if it's P6 or not
  bool p6Mode = false;          ///< To save us on string compares below

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  if (ecmdParseOption(&argc, &argv, "-v"))
    verbose = true;

  /* We can no longer loop at a thread level, so we need to fish out what the user wants and turn it into a thread parm */
    /* JTA 09/27/06 */
  uint32_t thread = 0x8;
  for (int i = 0; i < argc; i++){
    if ((!strcasecmp(argv[i], "-all")) || (!strcasecmp(argv[i], "-tall"))){
      thread = 0xC;
      break;
    } else if (!strcasecmp(argv[i], "-t1")){ 
      thread = 0x4;
      break;
    } else if (!strcasecmp(argv[i], "-t0")){ 
      thread = 0x8;
      break;
    }
  }

  if (ecmdParseOption(&argc, &argv, "all"))
    executeAll = true;

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 1) {
    ecmdOutputError("cipinstruct - Too few arguments specified; you need at least start/stop/step.\n");
    ecmdOutputError("cipinstruct - Type 'cipinstruct -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  /* Grab the number of steps */
  if (argc > 1) {
    steps = atoi(argv[1]);
  }

  /* Run the all functions */
  if (executeAll) {
    if (!strcasecmp(argv[0],"start")) {
      ecmdOutput("Starting instructions on all processors ...\n");
      rc = cipStartAllInstructions();
    } else if (!strcasecmp(argv[0], "sreset")) {
      ecmdOutput("Starting instructions on all processors via S-Reset ...\n");
      rc = cipStartAllInstructionsSreset();
    } else if (!strcasecmp(argv[0], "stop")) {
      ecmdOutput("Stopping instructions on all processors ...\n");
      rc = cipStopAllInstructions();
    } else if (!strcasecmp(argv[0], "step")) {
      ecmdOutputError("cipinstruct - Cannot step all processors, you must use target args not 'all' keyword\n");
      return ECMD_INVALID_ARGS;
    } else {
      ecmdOutputError("cipinstruct - Invalid instruct mode, must be start|stop|step \n");
      return ECMD_INVALID_ARGS;
    }

    if (rc) {
      ecmdOutputError( "cipinstruct - Error occured performing instruct function\n" );
      return rc;
    }

  } else {

    // Get the chip data for the special P6 code below
    // We need to do 1 loop so we can get a valid target for the ecmdGetChipData call
    target.chipType = ECMD_CHIPT_PROCESSOR;
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
    target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

    rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
    if (rc) return rc;

    while ( ecmdLooperNext(target, looperdata) ) {
      rc = ecmdGetChipData(target, chipData);
      if (rc) return rc;
      break; // Only one time through
    }

    if (chipData.chipType == "p6") {
      p6Mode = true;
    } else {
      thread = 0xFFFFFFFF; // This is a signal to the plugin to ignore the thread
    }

    /* Loop through the steps so we step all procs in somewhat sync */
    for (int step = 0; step < steps; step ++) {

      //Setup the target that will be used to query the system config 
      target.chipType = ECMD_CHIPT_PROCESSOR;
      target.chipTypeState = ECMD_TARGET_FIELD_VALID;
      target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_FIELD_WILDCARD;
      if (p6Mode == true) {
        target.threadState = ECMD_TARGET_FIELD_UNUSED;
      } else {
        target.threadState = ECMD_TARGET_FIELD_WILDCARD;
        target.chipUnitType = "core";
        target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
      }

      rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
      if (rc) return rc;

      if (!strcasecmp(argv[0],"start")) {
        ecmdOutput("Starting processor instructions ...\n");
      } else if (!strcasecmp(argv[0], "sreset")) {
        ecmdOutput("Starting processor instructions via S-Reset ...\n");
      } else if (!strcasecmp(argv[0], "stop")) {
        ecmdOutput("Stopping processor instructions ...\n");
      } else if (!strcasecmp(argv[0], "step")) {
        char buf[100];
        sprintf(buf,"Stepping processor instructions (%d)  ...\n",step+1);
        ecmdOutput(buf);
      } else {
        ecmdOutputError("cipinstruct - Invalid instruct mode, must be start|stop|step \n");
        return ECMD_INVALID_ARGS;
      }


      while ( ecmdLooperNext(target, looperdata) ) {

        if (!strcasecmp(argv[0],"start")) {
          rc = cipStartInstructions(target, thread);
        } else if (!strcasecmp(argv[0], "sreset")) {
          rc = cipStartInstructionsSreset(target, thread);
        } else if (!strcasecmp(argv[0], "stop")) {
          rc = cipStopInstructions(target, thread);
        } else if (!strcasecmp(argv[0], "step")) {
          rc = cipStepInstructions(target, 1, thread);
        }

        if (rc == ECMD_TARGET_NOT_CONFIGURED) {
          continue;
        }
        else if (rc) {
          printed = "cipinstruct - Error occured performing instruct function on ";
          printed += ecmdWriteTarget(target) + "\n";
          ecmdOutputError( printed.c_str() );
          return rc;
        }
        else {
          validPosFound = true;     
        }

        if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
          printed = ecmdWriteTarget(target) + "\n";
          ecmdOutput(printed.c_str());
        }
        if (verbose) {
          rc = getSpr(target,"iar", iarData);
          if (rc) {
            ecmdOutputWarning("cipinstruct - Unable to read 'iar' from chip, verbose mode disabled\n");
            verbose = false;
          } else {
            printed = "iar\t";
            printed += ecmdWriteDataFormatted(iarData, "x");
            ecmdOutput( printed.c_str() );
          }
        }

      }

      if (!validPosFound) {
        //this is an error common across all UI functions
        ecmdOutputError("cipinstruct - Unable to find a valid chip to execute command on\n");
        return ECMD_TARGET_NOT_CONFIGURED;
      }
    } /* End for loop */

  } /* End !all */

  return rc;
}
#endif // CIP_REMOVE_INSTRUCTION_FUNCTIONS

#ifndef CIP_REMOVE_BREAKPOINT_FUNCTIONS
uint32_t cipBreakpointUser(int argc, char* argv[]){
  uint32_t rc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  ecmdLooperData looperdata;    ///< Store internal Looper data
  ecmdBreakpointType_t type;  	///< Type of breakpoint to use 
  uint64_t address;		///< 64 bits address of breakpoint
  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  /* We can no longer loop at a thread level, so we need to fish out what the user wants and turn it into a thread parm */
  /* JTA 09/27/06 */
  uint32_t thread = 0x8;
  for (int i = 0; i < argc; i++){
    if ((!strcasecmp(argv[i], "-all")) || (!strcasecmp(argv[i], "-tall"))){
      thread = 0xC;
      break;
    } else if (!strcasecmp(argv[i], "-t1")){ 
      thread = 0x4;
      break;
    } else if (!strcasecmp(argv[i], "-t0")){ 
      thread = 0x8;
      break;
    }
  }
 

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 3) {
    ecmdOutputError("cipbreakpoint - Too few arguments specified; you need at least set|clear, type and address.\n");
    ecmdOutputError("cipbreakpoint - Type 'cipbreakpoint -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  
  
  // Find out the type of break point
  if (!strcasecmp(argv[1], "IABR")) {
    type = CIP_BREAKPOINT_IABR;
  } else if (!strcasecmp(argv[1], "DABR")) {
    type = CIP_BREAKPOINT_DABR;
  } else if (!strcasecmp(argv[1], "CIABR")) {
    type = CIP_BREAKPOINT_CIABR;
  } else {
     ecmdOutputError("cipbreakpoint - Invalid breakpoint type, must be IABR | DABR | CIABR \n");
     return ECMD_INVALID_ARGS;
  }

  address = strtoull(argv[2], NULL, 16);
  
  //Setup the target that will be used to query the system config 
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_FIELD_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;
  
  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
        
  while ( ecmdLooperNext(target, looperdata) ) {


    if (!strcasecmp(argv[0],"set")) {
      rc = cipSetBreakpoint(target, address, type, thread);
    } else if (!strcasecmp(argv[0], "clear")) {
      rc = cipClearBreakpoint(target,address, type, thread);
    } else {
       printed = "cipbreakpoint - Invalid argument '";
       printed += (std::string)argv[0];
       printed += "' must be set|clear \n";
      ecmdOutputError(printed.c_str());
      return ECMD_INVALID_ARGS;
    }

    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
      printed = "cipbreakpoint - Error occured performing breakpoint function on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    else {
      validPosFound = true;	
    }

    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      printed = ecmdWriteTarget(target) + "\n";
      ecmdOutput(printed.c_str());
    }

  }

  if (!validPosFound) {
    //this is an error common across all UI functions
    ecmdOutputError("cipbreakpoint - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }
  return rc;
}
#endif // CIP_REMOVE_BREAKPOINT_FUNCTIONS

#ifndef CIP_REMOVE_VR_FUNCTIONS
uint32_t cipGetVrUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  std::list<ecmdIndexEntry> entries;    ///< List of vr's to fetch, to use getVrMultiple
  std::list<ecmdIndexEntry> entries_copy;    ///< List of vr's to fetch, to use getVrMultiple
  ecmdIndexEntry  entry;         ///< Vr entry to fetch
  ecmdLooperData looperdata;            ///< Store internal Looper data
  int idx;
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

  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 1) {
    printed = "cipgetvr - Too few arguments specified; you need at least one vr.\n";
    ecmdOutputError(printed.c_str());
    printed = "cipgetvr - Type 'cipgetvr -h' for usage.\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = target.threadState = ECMD_TARGET_FIELD_WILDCARD;


  /* Walk through the arguments and create our list of vrs */
  startEntry = atoi(argv[0]);
  if (argc > 1) {
    numEntries = atoi(argv[1]);
  }
  for (idx = startEntry; idx < numEntries; idx ++) {
    entry.index = idx;
    entries.push_back(entry);
  }

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while ( ecmdLooperNext(target, looperdata) ) {

    /* Restore to our initial list */
    entries_copy = entries;


    /* Actually go fetch the data */
    rc = cipGetVrMultiple(target, entries_copy);

    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
      printed = "cipgetvr - Error occured performing cipGetVrMultiple on ";
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

      sprintf(buf,"%.02X\t", entit->index);
      printed = buf;

      printed += ecmdWriteDataFormatted(entit->buffer, format);

      ecmdOutput( printed.c_str() );
    }

    ecmdOutput("\n");
  }

  if (!validPosFound) {
    printed = "cipgetvr - Unable to find a valid chip to execute command on\n";
    //this is an error common across all UI functions
    ecmdOutputError(printed.c_str());
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t cipPutVrUser(int argc, char * argv[]) { 
  uint32_t rc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  std::string inputformat = "x";                ///< Default input format
  std::string dataModifier = "insert";          ///< Default data Modifier (And/Or/insert)
  ecmdDataBuffer buffer;        ///< Buffer to store data to write with
  ecmdDataBuffer sprBuffer;     ///< Buffer to store data from the spr
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  ecmdLooperData looperdata;            ///< Store internal Looper data
  int entry;                    ///< Index entry to write 
  uint32_t startBit = ECMD_UNSET; ///< Startbit to insert data
  uint32_t numBits = 0;         ///< Number of bits to insert data
  char* dataPtr = NULL;         ///< Pointer to spr data in argv array
  
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


  
  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 2) {
    printed = "cipputvr - Too few arguments specified; you need at least one vr  and some data.\n";
    ecmdOutputError(printed.c_str());
    printed = "r - Type 'cipputvr -h' for usage.\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = target.threadState = ECMD_TARGET_FIELD_WILDCARD;

  entry = atoi(argv[0]);

  if (argc == 4) {
    if (!ecmdIsAllDecimal(argv[1])) {
      printed = "cipputvr - Non-decimal numbers detected in startbit field\n";
    ecmdOutputError(printed.c_str());
      return ECMD_INVALID_ARGS;
    }
    startBit = (uint32_t)atoi(argv[1]);

    if (!ecmdIsAllDecimal(argv[2])) {
      printed = "cipputvr - Non-decimal numbers detected in numbits field\n";
    ecmdOutputError(printed.c_str());
      return ECMD_INVALID_ARGS;
    }
    numBits = (uint32_t)atoi(argv[2]);
    
    
    dataPtr = argv[3];

  } else if (argc == 2) {

    dataPtr = argv[1];

  } else {
    printed = "cipputvr - Too many arguments specified; you probably added an option that wasn't recognized.\n";
    ecmdOutputError(printed.c_str());
    printed = "cipputvr - Type 'cipputvr -h' for usage.\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
    
  }


  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while ( ecmdLooperNext(target, looperdata) ) {


    rc = cipGetVr(target, entry, sprBuffer);
    
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "cipputvr - Error occured performing getvr on ";
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
        printed = "cipputvr - Problems occurred parsing input data, must be an invalid format\n";
        ecmdOutputError(printed.c_str());
        return rc;
      }

      dataPtr = NULL;
    }

    rc = ecmdApplyDataModifier(sprBuffer, buffer,  startBit, dataModifier);
    if (rc) return rc;


    cipPutVr(target, entry, sprBuffer);

    if (rc) {
      printed = "cipputvr - Error occured performing command on ";
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
    printed = "cipputvr - Unable to find a valid chip to execute command on\n";
    ecmdOutputError(printed.c_str());
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}
#endif // CIP_REMOVE_VR_FUNCTIONS

#ifndef CIP_REMOVE_MEMORY_FUNCTIONS
uint32_t ecmdCipGetMemProcUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string outputformat = "mem";     ///< Output format - default to 'mem'
  ecmdDataBuffer memoryData;            ///< Buffer to hold return data from memory
  ecmdDataBuffer memoryTags;            ///< Buffer to hold return data from memory tags
  ecmdDataBuffer memoryEcc;             ///< Buffer to hold return data from memory ecc
  ecmdDataBuffer memoryEccError;        ///< Buffer to hold return data from memory ecc errors
  bool validPosFound = false;           ///< Did the looper find anything?
  ecmdChipTarget target;                ///< Current target being operated on
  uint64_t address;                     ///< The address from the command line
  uint32_t numBytes = ECMD_UNSET;       ///< Number of bytes from the command line
  std::string cmdlineName;              ///< Stores the name of what the command line function would be.
  int match;                            ///< For sscanf
  std::string printLine;                ///< Output data

  /************************************************************************/
  /* Setup the cmdlineName variable                                       */
  /************************************************************************/
  cmdlineName = "cipgetmemproc";

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  //Setup the target that will be used to query the system config
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;
  
  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if ((numBytes == ECMD_UNSET) && (argc < 2)) {  //chip + address
    printLine = cmdlineName + " - Too few arguments specified; you need at least an address and number of bytes.\n";
    ecmdOutputError(printLine.c_str());
    printLine = cmdlineName + " - Type '" + cmdlineName + " -h' for usage.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  } else if (argc < 1) { 
    printLine = cmdlineName + " - Too few arguments specified; you need at least an address.\n";
    ecmdOutputError(printLine.c_str());
    printLine = cmdlineName + " - Type '" + cmdlineName + " -h' for usage.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }

  // Get the address
  if (!ecmdIsAllHex(argv[0])) {
    printLine = cmdlineName + " - Non-hex characters detected in address field\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }
#ifdef _LP64
  match = sscanf(argv[0], "%lx", &address);
#else
  match = sscanf(argv[0], "%llx", &address);
#endif
  if (match != 1) {
    ecmdOutputError("Error occurred processing address!\n");
    return ECMD_INVALID_ARGS;
  }

  // Get the number of bytes
  if (numBytes == ECMD_UNSET) {
    numBytes = (uint32_t)atoi(argv[1]);
  }

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    rc = cipGetMemProc(target, address, numBytes, memoryData, memoryTags, memoryEcc, memoryEccError);
    if (rc) {
      printLine = cmdlineName + " - Error occured performing " + cmdlineName + " on ";
      printLine += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printLine.c_str() );
      coeRc = rc;
      continue;
    }
    else {
      validPosFound = true;     
    }

    // Now print the data
    printLine = ecmdWriteTarget(target);
    printLine += "\n";

    // Not using something like ecmdWriteDataFormatted because of all the special printing with tags, etc..
    // This code isn't pretty, but it gets the job done
    uint32_t wordsDone = 0;
    char tempstr[100];
    uint64_t myAddr = address;

    // Handle whole 64 bit blocks
    while ((memoryData.getWordLength() - wordsDone) > 2) {
      sprintf(tempstr,"%016llX: %08X %08X %s %s %s", (uint64_t)myAddr, memoryData.getWord(wordsDone), memoryData.getWord(wordsDone+1), memoryTags.genBinStr((wordsDone/2),1).c_str(), memoryEcc.genBinStr(((wordsDone/2) * 8), 8).c_str(), memoryEccError.genBinStr((wordsDone/2),1).c_str());
      printLine += tempstr;

      printLine += "\n";
      myAddr += 8;
      wordsDone += 2;
    }

    // See if we have any hangers on
    if (memoryData.getByteLength() > (wordsDone * 4)) {
      uint32_t byteCount = 0;
      /* The address */
      sprintf(tempstr,"%016llX: ", (uint64_t)myAddr);
      printLine += tempstr;
      /* Put on the remaining data */
      for (uint32_t byte = wordsDone * 4; byte < memoryData.getByteLength(); byte++, byteCount++) {
        printLine += memoryData.genHexLeftStr((byte * 4), 8);
        /* Put a space between words */
        if (byteCount == 3) {
          printLine += " ";
        }
      }
      /* Pad in for unused bytes */
      for (uint32_t byte = byteCount; byte < 8; byte++) {
        printLine += "  ";
        /* Put a space between words */
        if (byte == 3) {
          printLine += " ";
        }
      }
      /* Throw the tag information on the end */
      sprintf(tempstr," %s %s %s", memoryTags.genBinStr((wordsDone/2),1).c_str(), memoryEcc.genBinStr(((wordsDone/2) * 8), 8).c_str(), memoryEccError.genBinStr((wordsDone/2),1).c_str());
      printLine += tempstr;
      printLine += "\n";
    }

    /* Put it all to the screen */
    ecmdOutput( printLine.c_str() );
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    printLine = cmdlineName + " - Unable to find a valid chip to execute command on\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdCipPutMemProcUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string inputformat = "x";        ///< Output format - default to 'mem'
  ecmdDataBuffer inputData;             ///< Buffer to hold the data intended for memory
  ecmdDataBuffer inputTag;              ///< Buffer to hold the data intended for memory tags
  ecmdDataBuffer inputEcc;              ///< Buffer to hold the data intended for memory ecc
  bool validPosFound = false;           ///< Did the looper find anything?
  ecmdChipTarget target;                ///< Current target being operated on
  uint64_t address = 0;                 ///< The address from the command line
  std::string cmdlineName;              ///< Stores the name of what the command line function would be.
  int match;                            ///< For sscanf
  std::string printLine;                ///< Output data

  /************************************************************************/
  /* Setup the cmdlineName variable                                       */
  /************************************************************************/
  cmdlineName = "cipputmemproc";
  
  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
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

  if (argc < 4) {  //chip + address
    printLine = cmdlineName + " - Too few arguments specified; you need an address, data, tag bit and error inject key.\n";
    ecmdOutputError(printLine.c_str());
    printLine = cmdlineName + " - Type '" + cmdlineName + " -h' for usage.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  } else if(argc > 4) {
    printLine = cmdlineName + " - Too many arguments specified; you only need an address, data, tag bit and error inject key.\n";
    ecmdOutputError(printLine.c_str());
    printLine = cmdlineName + " - Type '" + cmdlineName + " -h' for usage.\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState =   ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;
  
  // Get the address
  if (!ecmdIsAllHex(argv[0])) {
    printLine = cmdlineName + " - Non-hex characters detected in address field\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }
#ifdef _LP64
  match = sscanf(argv[0], "%lx", &address);
#else
  match = sscanf(argv[0], "%llx", &address);
#endif
  if (match != 1) {
    ecmdOutputError("Error occurred processing address!\n");
    return ECMD_INVALID_ARGS;
  }
  
  // Read in the input data
  rc = ecmdReadDataFormatted(inputData, argv[1] , inputformat);
  if (rc) {
    printLine = cmdlineName + " - Problems occurred parsing input data, must be an invalid format\n";
    ecmdOutputError(printLine.c_str());
    return rc;
  }

  if (inputData.getBitLength() != 64) {
    printLine = cmdlineName + " - Exactly 64 bits of input data has to be provided\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_INVALID_ARGS;
  }

  // Read the input memory tag
  inputTag.insertFromBinAndResize(argv[2]);

  // Read the input memory ecc
  inputEcc.insertFromBinAndResize(argv[3]);

  
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/
  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    rc = cipPutMemProc(target, address, 64, inputData, inputTag, inputEcc);
    if (rc) {
      printLine = cmdlineName + " - Error occured performing " + cmdlineName + " on ";
      printLine += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printLine.c_str() );
      coeRc = rc;
      continue;
    }
    else {
      validPosFound = true;     
    }

    // Write out who we wrote too
    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      printLine = ecmdWriteTarget(target) + "\n";
      ecmdOutput(printLine.c_str());
    }
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    printLine = cmdlineName + " - Unable to find a valid chip to execute command on\n";
    ecmdOutputError(printLine.c_str());
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}
#endif // CIP_REMOVE_MEMORY_FUNCTIONS
