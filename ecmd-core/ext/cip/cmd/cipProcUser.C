//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2003,2016 IBM International Business Machines Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
//IBM_PROLOG_END_TAG


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
#include <string.h>
#include <fstream>
#include <algorithm> // for transform

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
#ifndef ECMD_REMOVE_PROCESSOR_FUNCTIONS
bool ecmdIsValidChip(const char * pcChipName, ecmdChipTarget &iTarget);
#endif

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------
#ifndef CIP_REMOVE_INSTRUCTION_FUNCTIONS
uint32_t cipInstructUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  ecmdChipTarget subTarget;        ///< Current target
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  bool threadFound = false;     ///< Did we find a thread for the position ?
  bool firstThreadLoop = true;  ///< Is this the first thread command for this core ?
  std::string printed;          ///< Print Buffer
  ecmdLooperData looperData;            ///< Store internal Looper data
  ecmdLooperData subLooperData;            ///< Store internal Looper data
  bool executeAll = false;      ///< Run start/stop on all procs
  bool verbose    = false;      ///< Display iar after each step
  ecmdDataBuffer  iarData;      ///< Data read from IAR
  int  steps = 1;               ///< Number of steps to run
  ecmdChipData chipData;        ///< So we can determine the processor
  ecmdLoopMode_t loopMode;      ///< The mode in which we will loop over threads

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
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
    ecmdOutputError("cipinstruct - Too few arguments specified; you need at least start/stop/step.\n");
    ecmdOutputError("cipinstruct - Type 'cipinstruct -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  // check if first argument is a chip
  int argOffset = 0;
  ecmdChipTarget validTarget;
  validTarget.chipUnitTypeState = validTarget.chipUnitNumState = validTarget.threadState = ECMD_TARGET_FIELD_UNUSED;
  bool validChipFound = ecmdIsValidChip(argv[0], validTarget);
  if (validChipFound == true)
  {
    // if all is selected chip can not be set on command line
    if (executeAll == true)
    {
      ecmdOutputError("cipinstruct - all option cannot be used when chip target is specified\n");
      ecmdOutputError("cipinstruct - Type 'cipinstruct -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }
    argOffset = 1;
  }

  // Check to see if invalid syntax was used and chip was input after subcommand
  if ((!validChipFound) && (argc > 1))
  {
      ecmdChipTarget testTarget;
      testTarget.chipUnitTypeState = validTarget.chipUnitNumState = validTarget.threadState = ECMD_TARGET_FIELD_UNUSED;
      bool wrongOrderChipFound = ecmdIsValidChip(argv[1], testTarget);
      if (wrongOrderChipFound)
      {
          ecmdOutputError("cipinstruct - Chip type found in wrong order; chip must be before start/stop/etc commands.\n");
          ecmdOutputError("cipinstruct - Type 'cipinstruct -h' for usage.\n");
          return ECMD_INVALID_ARGS;
      }
  }

  if (argc < (1 + argOffset)) {
    ecmdOutputError("cipinstruct - Too few arguments specified; you need at least start/stop/step after chip.\n");
    ecmdOutputError("cipinstruct - Type 'cipinstruct -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  /* Grab the number of steps */
  if (argc > (1 + argOffset)) {
    steps = atoi(argv[1 + argOffset]);
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

    // Get the chip data for the special code below
    // We need to do 1 loop so we can get a valid target for the ecmdGetChipData call
    target.chipType = ECMD_CHIPT_PROCESSOR;
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
    target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

    rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
    if (rc) return rc;

    while ( ecmdLooperNext(target, looperData) ) {
      rc = ecmdGetChipData(target, chipData);
      if (rc) return rc;
      break; // Only one time through
    }

    thread = 0xFFFFFFFF; // This is a signal to the plugin to ignore the thread

    /* Loop through the steps so we step all procs in somewhat sync */
    for (int step = 0; step < steps; step++) {

      //Setup the target that will be used to query the system config 
      target.chipType = ECMD_CHIPT_PROCESSOR;
      target.chipTypeState = ECMD_TARGET_FIELD_VALID;
      target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
      if (validChipFound == true) {
        target.chipType = validTarget.chipType;
        target.chipUnitType = validTarget.chipUnitType;
        target.chipUnitTypeState = validTarget.chipUnitTypeState;
        target.chipUnitNumState = validTarget.chipUnitNumState;
        target.threadState = ECMD_TARGET_FIELD_UNUSED;
      } else {
        target.chipUnitType = "core";
        target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
        target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
        target.threadState = ECMD_TARGET_FIELD_UNUSED;
      }

      rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
      if (rc) return rc;

      if (!strcasecmp(argv[0 + argOffset],"start")) {
        ecmdOutput("Starting processor instructions ...\n");
      } else if (!strcasecmp(argv[0 + argOffset], "sreset")) {
        ecmdOutput("Starting processor instructions via S-Reset ...\n");
      } else if (!strcasecmp(argv[0 + argOffset], "stop")) {
        ecmdOutput("Stopping processor instructions ...\n");
      } else if (!strcasecmp(argv[0 + argOffset], "step")) {
        char buf[100];
        sprintf(buf,"Stepping processor instructions (%d)  ...\n",step+1);
        ecmdOutput(buf);
      } else {
        ecmdOutputError("cipinstruct - Invalid instruct mode, must be start|stop|step \n");
        return ECMD_INVALID_ARGS;
      }

      /* This will get us looping through the cores in forward order */
      while (ecmdLooperNext(target, looperData)) {

        threadFound = false;
        firstThreadLoop = true;
        /* On some chips, we have to start the threads in reverse order to keep the chip in the proper SMT mode */
        /* This code will accomplish that by looping over cores in order above, but threads in reverse below */
        if (target.chipUnitType == "occ") {
          subTarget = target;
          loopMode = ECMD_DYNAMIC_LOOP;
        } else {
          subTarget = target;
          subTarget.threadState = ECMD_TARGET_FIELD_WILDCARD;
          // We only want to loop backwards over the start/sreset, the rest need to go forwards like normal
          if (!strcasecmp(argv[0 + argOffset],"start") || !strcasecmp(argv[0 + argOffset],"sreset")) {
            loopMode = ECMD_DYNAMIC_REVERSE_LOOP;
          } else {
            loopMode = ECMD_DYNAMIC_LOOP;
          }
        }

        rc = ecmdLooperInit(subTarget, ECMD_SELECTED_TARGETS_LOOP, subLooperData, loopMode);
        if (rc) return rc;

        while (ecmdLooperNext(subTarget, subLooperData) && (!coeRc || coeMode)) {

          if (firstThreadLoop) {
            if (!strcasecmp(argv[0 + argOffset],"start") || !strcasecmp(argv[0 + argOffset],"sreset")) {
              uint32_t specialWakeupMode = 0;
              rc = cipSpecialWakeup(target, true, specialWakeupMode);
              if (rc) {
                printed = "cipinstruct - Error occured performing special wakeup on ";
                printed += ecmdWriteTarget(target) + "\n";
                ecmdOutputError( printed.c_str() );
                coeRc = rc;
                continue;
              }
            }
            firstThreadLoop = false;
          }

          if (!strcasecmp(argv[0 + argOffset],"start")) {
            rc = cipStartInstructions(subTarget, thread);
          } else if (!strcasecmp(argv[0 + argOffset], "sreset")) {
            rc = cipStartInstructionsSreset(subTarget, thread);
          } else if (!strcasecmp(argv[0 + argOffset], "stop")) {
            rc = cipStopInstructions(subTarget, thread);
          } else if (!strcasecmp(argv[0 + argOffset], "step")) {
            rc = cipStepInstructions(subTarget, 1, thread);
          }

          if (rc) {
            printed = "cipinstruct - Error occured performing instruct function on ";
            printed += ecmdWriteTarget(subTarget) + "\n";
            ecmdOutputError( printed.c_str() );
            coeRc = rc;
            continue;
          }
          else {
            validPosFound = true;     
            threadFound = true;
          }

          if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
            printed = ecmdWriteTarget(subTarget) + "\n";
            ecmdOutput(printed.c_str());
          }
          if (verbose) {
            rc = getSpr(subTarget,"iar", iarData);
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
        if (threadFound) {
          if (!strcasecmp(argv[0 + argOffset],"start") || !strcasecmp(argv[0 + argOffset],"sreset")) {
            uint32_t specialWakeupMode = 0;
            rc = cipSpecialWakeup(target, false, specialWakeupMode);
            if (rc) {
              printed = "cipinstruct - Error occured performing special wakeup on ";
              printed += ecmdWriteTarget(target) + "\n";
              ecmdOutputError( printed.c_str() );
              coeRc = rc;
              continue;
            }
          }
        }
      }

      // coeRc will be the return code from in the loop, coe mode or not.
      if (coeRc) return coeRc;

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
  target.cageState = target.nodeState = target.slotState = target.posState = target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
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
  ecmdChipTarget subTarget;     ///< Current target with additional fields set, like chipUnitType
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
  ecmdProcRegisterInfo procInfo; ///< Used to figure out if an SPR is threaded or not
  std::string sprName = "vr";
  std::string function = "cipgetvr";

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
  target.cageState = target.nodeState = target.slotState = target.posState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_WILDCARD;


  /* Walk through the arguments and create our list of vrs */
  startEntry = atoi(argv[0]);
  if (argc > 1) {
    numEntries = atoi(argv[1]);
  }

  //set the register size to 4 words as the VR register is of 128 bits.
  entry.buffer.setWordLength(4);  
  
  for (idx = startEntry; idx < startEntry + numEntries; idx ++) {
    entry.index = idx;
    entries.push_back(entry);
  }


  
  /* First thing we need to do is find out for this particular target if the SPR is threaded */
  rc = ecmdQueryProcRegisterInfo(target, sprName.c_str(), procInfo);
  if (rc) {
    printed = function + " - Error occured getting info for ";
    printed += sprName;
    printed += " on ";
    printed += ecmdWriteTarget(target) + "\n";
    ecmdOutputError( printed.c_str() );
    return rc;
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



  rc = ecmdLooperInit(subTarget, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while ( ecmdLooperNext(subTarget, looperdata) ) {

    /* Restore to our initial list */
    entries_copy = entries;


    /* Actually go fetch the data */
    rc = cipGetVrMultiple(subTarget, entries_copy);

    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
      printed = "cipgetvr - Error occured performing cipGetVrMultiple on ";
      printed += ecmdWriteTarget(subTarget) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    else {
      validPosFound = true;     
    }

    printed = ecmdWriteTarget(subTarget) + "\n";
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
  ecmdChipTarget subTarget;     ///< Current target with additional fields set, like chipUnitType
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
  ecmdProcRegisterInfo procInfo; ///< Used to figure out if an SPR is threaded or not
  std::string sprName = "vr";
  std::string function = "cipputvr";
  
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
    printed = "cipputvr - Too few arguments specified; you need at least one vr and some data.\n";
    ecmdOutputError(printed.c_str());
    printed = "cipputvr - Type 'cipputvr -h' for usage.\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_WILDCARD;

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
  
  /* First thing we need to do is find out for this particular target if the SPR is threaded */
  rc = ecmdQueryProcRegisterInfo(target, sprName.c_str(), procInfo);
  if (rc) {
    printed = function + " - Error occured getting info for ";
    printed += sprName;
    printed += " on ";
    printed += ecmdWriteTarget(target) + "\n";
    ecmdOutputError( printed.c_str() );
    return rc;
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


  rc = ecmdLooperInit(subTarget, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

//set the register size to 4 words as the VR register is of 128 bits.

  sprBuffer.setWordLength(4);

  while ( ecmdLooperNext(subTarget, looperdata) ) {


    rc = cipGetVr(subTarget, entry, sprBuffer);
    
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "cipputvr - Error occured performing getvr on ";
        printed += ecmdWriteTarget(subTarget) + "\n";
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


    cipPutVr(subTarget, entry, sprBuffer);

    if (rc) {
      printed = "cipputvr - Error occured performing command on ";
      printed += ecmdWriteTarget(subTarget) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }

    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      printed = ecmdWriteTarget(subTarget) + "\n";
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

  match = sscanf(argv[0], UINT64_HEX_FORMAT, &address);
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
      sprintf(tempstr,UINT64_HEX16_FORMAT ": %08X %08X %s %s %s", (uint64_t)myAddr, memoryData.getWord(wordsDone), memoryData.getWord(wordsDone+1), memoryTags.genBinStr((wordsDone/2),1).c_str(), memoryEcc.genBinStr(((wordsDone/2) * 8), 8).c_str(), memoryEccError.genBinStr((wordsDone/2),1).c_str());
      printLine += tempstr;

      printLine += "\n";
      myAddr += 8;
      wordsDone += 2;
    }

    // See if we have any hangers on
    if (memoryData.getByteLength() > (wordsDone * 4)) {
      uint32_t byteCount = 0;
      /* The address */
      sprintf(tempstr,UINT64_HEX16_FORMAT ": ", (uint64_t)myAddr);
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
  match = sscanf(argv[0], UINT64_HEX_FORMAT, &address);
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

#ifndef CIP_REMOVE_VSR_FUNCTIONS
uint32_t cipGetVsrUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  ecmdChipTarget subTarget;     ///< Current target with additional fields set, like chipUnitType
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  std::list<ecmdIndexEntry> entries;    ///< List of vsr's to fetch, to use getVsrMultiple
  std::list<ecmdIndexEntry> entries_copy;    ///< List of vsr's to fetch, to use getVsrMultiple
  ecmdIndexEntry  entry;         ///< Vsr entry to fetch
  ecmdLooperData looperdata;            ///< Store internal Looper data
  int idx;
  int numEntries = 1;           ///< Number of consecutive entries to retrieve
  int startEntry = 0;           ///< Entry to start on
  char buf[100];                ///< Temporary string buffer
  ecmdProcRegisterInfo procInfo; ///< Used to figure out if an SPR is threaded or not
  std::string sprName = "vsr";
  std::string function = "cipgetvsr";

 
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
    printed = "cipgetvsr - Too few arguments specified; you need at least one vsr.\n";
    ecmdOutputError(printed.c_str());
    printed = "cipgetvsr - Type 'cipgetvsr -h' for usage.\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_WILDCARD;

  subTarget = target;

  /* Walk through the arguments and create our list of vsrs */
  startEntry = atoi(argv[0]);
  if (argc > 1) {
    numEntries = atoi(argv[1]);
  }

  for (idx = startEntry; idx < startEntry + numEntries; idx ++) {
    entry.index = idx;
    entries.push_back(entry);
  }


  
  /* First thing we need to do is find out for this particular target if the SPR is threaded */
  rc = ecmdQueryProcRegisterInfo(target, sprName.c_str(), procInfo);
  if (rc) {
    printed = function + " - Error occured getting info for ";
    printed += sprName;
    printed += " on ";
    printed += ecmdWriteTarget(target) + "\n";
    ecmdOutputError( printed.c_str() );
    return rc;
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

  rc = ecmdLooperInit(subTarget, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while ( ecmdLooperNext(subTarget, looperdata) ) {

    /* Restore to our initial list */
    entries_copy = entries;

    /* Actually go fetch the data */
    rc = cipGetVsrMultiple(subTarget, entries_copy);

    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
      printed = "cipgetvsr - Error occured performing cipGetVsrMultiple on ";
      printed += ecmdWriteTarget(subTarget) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    else {
      validPosFound = true;     
    }

    printed = ecmdWriteTarget(subTarget) + "\n";
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
    printed = "cipgetvsr - Unable to find a valid chip to execute command on\n";
    //this is an error common across all UI functions
    ecmdOutputError(printed.c_str());
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t cipPutVsrUser(int argc, char * argv[]) { 
  uint32_t rc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  ecmdChipTarget subTarget;     ///< Current target with additional fields set, like chipUnitType
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
  ecmdProcRegisterInfo procInfo; ///< Used to figure out if an SPR is threaded or not
  std::string sprName = "vsr";
  std::string function = "cipputvsr";
  
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
    printed = "cipputvsr - Too few arguments specified; you need at least one vsr and some data.\n";
    ecmdOutputError(printed.c_str());
    printed = "cipputvsr - Type 'cipputvsr -h' for usage.\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_WILDCARD;

  entry = atoi(argv[0]);

  if (argc == 4) {
    if (!ecmdIsAllDecimal(argv[1])) {
      printed = "cipputvsr - Non-decimal numbers detected in startbit field\n";
    ecmdOutputError(printed.c_str());
      return ECMD_INVALID_ARGS;
    }
    startBit = (uint32_t)atoi(argv[1]);

    if (!ecmdIsAllDecimal(argv[2])) {
      printed = "cipputvsr - Non-decimal numbers detected in numbits field\n";
    ecmdOutputError(printed.c_str());
      return ECMD_INVALID_ARGS;
    }
    numBits = (uint32_t)atoi(argv[2]);
    
    
    dataPtr = argv[3];

  } else if (argc == 2) {

    dataPtr = argv[1];

  } else {
    printed = "cipputvsr - Too many arguments specified; you probably added an option that wasn't recognized.\n";
    ecmdOutputError(printed.c_str());
    printed = "cipputvsr - Type 'cipputvsr -h' for usage.\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
    
  }

  
  
  /* First thing we need to do is find out for this particular target if the SPR is threaded */
  rc = ecmdQueryProcRegisterInfo(target, sprName.c_str(), procInfo);
  if (rc) {
    printed = function + " - Error occured getting info for ";
    printed += sprName;
    printed += " on ";
    printed += ecmdWriteTarget(target) + "\n";
    ecmdOutputError( printed.c_str() );
    return rc;
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



  rc = ecmdLooperInit(subTarget, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while ( ecmdLooperNext(subTarget, looperdata) ) {


    rc = cipGetVsr(subTarget, entry, sprBuffer);
    
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "cipputvsr - Error occured performing getvsr on ";
        printed += ecmdWriteTarget(subTarget) + "\n";
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
        printed = "cipputvsr - Problems occurred parsing input data, must be an invalid format\n";
        ecmdOutputError(printed.c_str());
        return rc;
      }

      dataPtr = NULL;
    }

    rc = ecmdApplyDataModifier(sprBuffer, buffer,  startBit, dataModifier);
    if (rc) return rc;


    cipPutVsr(subTarget, entry, sprBuffer);

    if (rc) {
      printed = "cipputvsr - Error occured performing command on ";
      printed += ecmdWriteTarget(subTarget) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }

    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      printed = ecmdWriteTarget(subTarget) + "\n";
      ecmdOutput(printed.c_str());
    }

  }

  if (!validPosFound) {
    //this is an error common across all UI functions
    printed = "cipputvsr - Unable to find a valid chip to execute command on\n";
    ecmdOutputError(printed.c_str());
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}
#endif // CIP_REMOVE_VSR_FUNCTIONS

#ifndef CIP_REMOVE_RW_FUNCTIONS
uint32_t cipRWReadCacheUser(int argc, char * argv[])
{
    uint32_t l_rc = ECMD_SUCCESS;

    ecmdChipTarget l_target;        ///< Current target
    ecmdLooperData l_looperdata;    ///< Store internal Looper data
    // Commenting l_validPosFound out as it was causing compiler warnings.  Nothing was actually being done with the value.
    //bool l_validPosFound = false;   ///< Did we find something to actually execute on ?
    std::list<cipRWCacheRec> l_records; ///< Cache records from the occ
    std::string printed;

    /************************************************************************/
    /* Parse Common Cmdline Args                                            */
    /************************************************************************/

    l_rc = ecmdCommandArgs(&argc, &argv);
    if (l_rc) return l_rc;

    /************************************************************************/
    /* Parse Local ARGS here!                                               */
    /************************************************************************/
    if (argc < 4)
    {
        ecmdOutputError("ciprwreadcache - Too few arguments specified\n");
        ecmdOutputError("ciprwreadcache - Type 'ciprwreadcache -h' for usage.\n");
        return ECMD_INVALID_ARGS;
    }

    //Setup the target that will be used to query the system config
    std::string l_chipType, l_chipUnitType;
    l_rc = ecmdParseChipField(argv[0], l_chipType, l_chipUnitType, true /* supports wildcard usage */);
    if (l_rc) {
        ecmdOutputError("ciprwreadcache - Wildcard character detected however it is not being used correctly.\n");
        return l_rc;
    }

    l_target.cageState = l_target.nodeState = l_target.slotState = l_target.posState = ECMD_TARGET_FIELD_WILDCARD;
    l_target.chipUnitTypeState = l_target.chipUnitNumState = l_target.threadState = ECMD_TARGET_FIELD_UNUSED;

    if (l_chipType == "x") {
        l_target.chipTypeState = ECMD_TARGET_FIELD_WILDCARD;
    } else {
        l_target.chipType = l_chipType;
        l_target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    }

    if (l_chipUnitType != "") {
        l_target.chipUnitType = l_chipUnitType;
        l_target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
        l_target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
    }

    l_rc = ecmdLooperInit(l_target, ECMD_SELECTED_TARGETS_LOOP, l_looperdata);
    if (l_rc) return l_rc;

    // ciprwreadcache <target> <startline> <count> <I|D>

    /* argv[1] = startline */
    if (!ecmdIsAllDecimal(argv[1]))
    {
        ecmdOutputError("ciprwreadcache - Non-decimal characters detected in startline field\n");
        return ECMD_INVALID_ARGS;
    }
    uint32_t l_startline = atoi(argv[1]);

    /* argv[2] = count*/
    if (!ecmdIsAllDecimal(argv[2]))
    {
        ecmdOutputError("ciprwreadcache - Non-decimal characters detected in count field\n");
        return ECMD_INVALID_ARGS;
    }
    uint32_t l_count = atoi(argv[2]);

    /* argv[3] = mode */
    char l_memoryModeChar = argv[3][0];
    uint32_t l_memoryMode = 0;
    // convert memory mode to int
    if ((l_memoryModeChar == 'i') || (l_memoryModeChar == 'I'))
    {
        l_memoryMode = 0;
    }
    else if ((l_memoryModeChar == 'd') || (l_memoryModeChar == 'D'))
    {
        l_memoryMode = 1;
    }
    else
    {
        ecmdOutputError("ciprwreadcache - Error invalid memory mode specified!\n");
        return ECMD_INVALID_ARGS;
    }

    while (ecmdLooperNext(l_target, l_looperdata))
    {
        /* Display Header */
        printed = ecmdWriteTarget(l_target);
        printed += "\n";
        ecmdOutput(printed.c_str());
        printed = "Tag        Data     1        2        3        4        5        6        7         Valid   LRU     Dirty\n";
        //         00000000   00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000  00      00      00
        ecmdOutput(printed.c_str());

        l_rc = cipRWReadCache(l_target, l_startline, l_count, l_records, l_memoryMode);

        if (l_rc == ECMD_TARGET_NOT_CONFIGURED)
        {
            continue;
        }
        else
        {
	  //l_validPosFound = true;
            std::list<cipRWCacheRec>::iterator l_recordsIter = l_records.begin();
            while (l_recordsIter != l_records.end())
            {
                char buf[200];
                snprintf(buf, 200, "%08X   %08X %08X %08X %08X %08X %08X %08X %08X  %02X      %02X      %02X\n",
                         l_recordsIter->tag, l_recordsIter->data[0], l_recordsIter->data[1],
                         l_recordsIter->data[2], l_recordsIter->data[3],
                         l_recordsIter->data[4], l_recordsIter->data[5],
                         l_recordsIter->data[6], l_recordsIter->data[7],
                         l_recordsIter->valid, l_recordsIter->lru, l_recordsIter->dirty);
                ecmdOutput(buf);
                l_recordsIter++;
            }
        }
    }

    return l_rc;
}

uint32_t cipRWReadTLBUser(int argc, char * argv[])
{
    uint32_t l_rc = ECMD_SUCCESS;

    ecmdChipTarget l_target;        ///< Current target
    ecmdLooperData l_looperdata;    ///< Store internal Looper data
    //Commenting l_validPosFound out to avoid compiler warnings.  Nothing was actually being checked with the value.
    //bool l_validPosFound = false;   ///< Did we find something to actually execute on ?
    std::list<cipRWTLBRec> l_records; ///< Cache records from the occ
    std::string printed;

    /************************************************************************/
    /* Parse Common Cmdline Args                                            */
    /************************************************************************/

    l_rc = ecmdCommandArgs(&argc, &argv);
    if (l_rc) return l_rc;

    /************************************************************************/
    /* Parse Local ARGS here!                                               */
    /************************************************************************/
    if (argc < 3)
    {
        ecmdOutputError("ciprwreadtlb - Too few arguments specified\n");
        ecmdOutputError("ciprwreadtlb - Type 'ciprwreadtlb -h' for usage.\n");
        return ECMD_INVALID_ARGS;
    }

    //Setup the target that will be used to query the system config
    std::string l_chipType, l_chipUnitType;
    l_rc = ecmdParseChipField(argv[0], l_chipType, l_chipUnitType, true /* supports wildcard usage */);
    if (l_rc) {
        ecmdOutputError("ciprwreadtlb - Wildcard character detected however it is not being used correctly.\n");
        return l_rc;
    }

    l_target.cageState = l_target.nodeState = l_target.slotState = l_target.posState = ECMD_TARGET_FIELD_WILDCARD;
    l_target.chipUnitTypeState = l_target.chipUnitNumState = l_target.threadState = ECMD_TARGET_FIELD_UNUSED;

    if (l_chipType == "x") {
        l_target.chipTypeState = ECMD_TARGET_FIELD_WILDCARD;
    } else {
        l_target.chipType = l_chipType;
        l_target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    }

    if (l_chipUnitType != "") {
        l_target.chipUnitType = l_chipUnitType;
        l_target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
        l_target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
    }

    l_rc = ecmdLooperInit(l_target, ECMD_SELECTED_TARGETS_LOOP, l_looperdata);
    if (l_rc) return l_rc;

    // ciprwreadtlb <target> <startline> <count>

    /* argv[1] = startline */
    if (!ecmdIsAllDecimal(argv[1]))
    {
        ecmdOutputError("ciprwreadtlb - Non-decimal characters detected in startline field\n");
        return ECMD_INVALID_ARGS;
    }
    uint32_t l_startline = atoi(argv[1]);

    /* argv[2] = count*/
    if (!ecmdIsAllDecimal(argv[2]))
    {
        ecmdOutputError("ciprwreadtlb - Non-decimal characters detected in count field\n");
        return ECMD_INVALID_ARGS;
    }
    uint32_t l_count = atoi(argv[2]);

    while (ecmdLooperNext(l_target, l_looperdata))
    {
        /* Display Header */
        printed = ecmdWriteTarget(l_target);
        printed += "\n";
        ecmdOutput(printed.c_str());
        printed = "EPN      RPN      size ebit kbit valid tid par0 par1 ex wr zsel wimg\n";
        //         00000000 00000000 00   00   00   00    00  00   00   00 00 00   00
        ecmdOutput(printed.c_str());

        l_rc = cipRWReadTLB(l_target, l_startline, l_count, l_records);

        if (l_rc == ECMD_TARGET_NOT_CONFIGURED)
        {
            continue;
        }
        else
        {
	  //l_validPosFound = true;
            std::list<cipRWTLBRec>::iterator l_recordsIter = l_records.begin();
            while (l_recordsIter != l_records.end())
            {
                char buf[100];
                snprintf(buf, 100, "%08X %08X %02X   %02X   %02X   %02X    %02X  %02X   %02X   %02X %02X %02X   %02X\n",
                         l_recordsIter->epn, l_recordsIter->rpn, l_recordsIter->size,
                         l_recordsIter->ebit, l_recordsIter->kbit, l_recordsIter->valid,
                         l_recordsIter->tid, l_recordsIter->par[0], l_recordsIter->par[1],
                         l_recordsIter->ex, l_recordsIter->wr, l_recordsIter->zsel, l_recordsIter->wimg);
                ecmdOutput(buf);
                l_recordsIter++;
            }
        }
    }

    return l_rc;
}

uint32_t cipRWReadMemUser(int argc, char * argv[])
{
    uint32_t l_rc = ECMD_SUCCESS, l_coeRc = ECMD_SUCCESS;
    ecmdChipTarget l_target;                ///< Current target
    bool l_validPosFound = false;           ///< Did we find something to actually execute on ?

    ecmdLooperData l_looperdata;            ///< Store internal Looper data
    std::string l_outputformat = "mem";     ///< Output format - default to 'mem'
    ecmdDataBuffer l_returnData;            ///< Buffer to hold return data from memory

    bool l_expectFlag = false;              ///< Are we doing an expect?
    bool l_maskFlag = false;                ///< Are we masking our expect data?
    char* l_expectPtr = NULL;               ///< Pointer to expected data in arg list
    char* l_maskPtr = NULL;                 ///< Pointer to mask data in arg list
    ecmdDataBuffer l_expected;              ///< Buffer to store expected data
    ecmdDataBuffer l_mask;                  ///< Buffer for mask of expected data
    std::string l_inputformat = "x";        ///< Input format of data

    uint32_t l_address;                     ///< The address from the command line
    uint32_t l_numBytes = ECMD_UNSET;       ///< Number of bytes from the command line
    int l_match;                            ///< For sscanf
    char l_memoryModeChar;                  ///< Which memory to access from the command line

    /************************************************************************/
    /* Parse Local FLAGS here!                                              */
    /************************************************************************/

    /* get format flag, if it's there */
    char * l_outputFormatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
    if (l_outputFormatPtr != NULL)
    {
        l_outputformat = l_outputFormatPtr;
    }
    char * l_inputFormatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
    if (l_inputFormatPtr != NULL) {
        l_inputformat = l_inputFormatPtr;
    }

    /* Get the filename if -fb is specified */
    char * l_filename = ecmdParseOptionWithArgs(&argc, &argv, "-fb");

    /* Get the filename if -fd is specified */
    char * l_dcardfilename = ecmdParseOptionWithArgs(&argc, &argv, "-fd");

    if (((l_filename != NULL) || (l_dcardfilename != NULL)) &&
       ((l_outputFormatPtr != NULL) || (l_inputFormatPtr != NULL)))
    {
        ecmdOutputError("ciprwreadmem - Options -f and -o/-i can't be specified together for format. Specify either one.\n");
        return ECMD_INVALID_ARGS;
    }
    if ((l_dcardfilename != NULL) && (l_filename != NULL))
    {
        ecmdOutputError("ciprwreadmem - Options -fb and -fd can't be specified together for format. Specify either one.\n");
        return ECMD_INVALID_ARGS;
    }

    //expect and mask flags check
    if (l_filename == NULL && l_dcardfilename == NULL)
    {
        if ((l_expectPtr = ecmdParseOptionWithArgs(&argc, &argv, "-exp")) != NULL)
        {
            l_expectFlag = true;

            if ((l_maskPtr = ecmdParseOptionWithArgs(&argc, &argv, "-mask")) != NULL)
            {
                l_maskFlag = true;
            }
        }
    }
    else
    {
        // If we are passing in data with a file, just look for -exp
        l_expectFlag = ecmdParseOption(&argc, &argv, "-exp");
    }


    /************************************************************************/
    /* Parse Common Cmdline Args                                            */
    /************************************************************************/
    l_rc = ecmdCommandArgs(&argc, &argv);
    if (l_rc) return l_rc;

    /* Global args have been parsed, we can read if -coe was given */
    bool l_coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

    // Read in the expect data
    if (l_expectFlag)
    {
        if ((l_filename == NULL) && (l_dcardfilename == NULL))
        {

            l_rc = ecmdReadDataFormatted(l_expected, l_expectPtr, l_inputformat);
            if (l_rc)
            {
                ecmdOutputError("ciprwreadmem - Problems occurred parsing expected data, must be an invalid format\n");
                return l_rc;
            }

            if (l_maskFlag)
            {
                l_rc = ecmdReadDataFormatted(l_mask, l_maskPtr, l_inputformat);
                if (l_rc)
                {
                    ecmdOutputError("ciprwreadmem - Problems occurred parsing mask data, must be an invalid format\n");
                    return l_rc;
                }
            }
        }
        else
        {
            // Read from a file
            if (l_filename != NULL)
            {
                l_rc = l_expected.readFile(l_filename, ECMD_SAVE_FORMAT_BINARY_DATA);
                if (l_rc)
                {
                    std::string printed;
                    printed = "ciprwreadmem - Problems occurred parsing expected data from file ";
                    printed += l_filename;
                    printed += ", must be an invalid format\n";
                    ecmdOutputError(printed.c_str());
                    return l_rc;
                }
            }
            else
            {
                ecmdOutputError("ciprwreadmem - Currently Dcard support is not supported with -exp\n");
                return ECMD_INVALID_ARGS;

            }
            // Let's pull the length from the file
            l_numBytes = l_expected.getByteLength();
        }
    }

    /************************************************************************/
    /* Parse Local ARGS here!                                               */
    /************************************************************************/
    // ciprwreadmem <target> <address> <numbytes> <I|D|P>
    if ((l_numBytes == ECMD_UNSET) && (argc < 4)) //target + address + bytes + mode
    {
        ecmdOutputError("ciprwreadmem - Too few arguments specified; you need at least a target, address, number of bytes, and mode.\n");
        ecmdOutputError("ciprwreadmem - Type 'ciprwreadmem -h' for usage.\n");
        return ECMD_INVALID_ARGS;
    }
    else if (argc < 3)
    {
        ecmdOutputError("ciprwreadmem - Too few arguments specified; you need at least a target, address, and mode.\n");
        ecmdOutputError("ciprwreadmem - Type 'ciprwreadmem -h' for usage.\n");
        return ECMD_INVALID_ARGS;
    }

    //Setup the target that will be used to query the system config
    std::string l_chipType, l_chipUnitType;
    l_rc = ecmdParseChipField(argv[0], l_chipType, l_chipUnitType, true /* supports wildcard usage */);
    if (l_rc) {
        ecmdOutputError("ciprwreadmem - Wildcard character detected however it is not being used correctly.\n");
        return l_rc;
    }

    l_target.cageState = l_target.nodeState = l_target.slotState = l_target.posState = ECMD_TARGET_FIELD_WILDCARD;
    l_target.chipUnitTypeState = l_target.chipUnitNumState = l_target.threadState = ECMD_TARGET_FIELD_UNUSED;

    if (l_chipType == "x") {
        l_target.chipTypeState = ECMD_TARGET_FIELD_WILDCARD;
    } else {
        l_target.chipType = l_chipType;
        l_target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    }

    if (l_chipUnitType != "") {
        l_target.chipUnitType = l_chipUnitType;
        l_target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
        l_target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
    }

    // Get the address
    if (!ecmdIsAllHex(argv[1]))
    {
        ecmdOutputError("ciprwreadmem - Non-hex characters detected in address field\n");
        return ECMD_INVALID_ARGS;
    }
    l_match = sscanf(argv[1], "%x", &l_address);
    if (l_match != 1)
    {
        ecmdOutputError("ciprwreadmem - Error occurred processing address!\n");
        return ECMD_INVALID_ARGS;
    }

    // get Instruction, Data, or Physical memory
    if (l_numBytes == ECMD_UNSET)
    {
        l_memoryModeChar = argv[3][0];
    }
    else
    {
        l_memoryModeChar = argv[2][0];
    }

    uint32_t l_memoryMode = 0;
    // convert memory mode to int
    if ((l_memoryModeChar == 'i') || (l_memoryModeChar == 'I'))
    {
        l_memoryMode = 0;
    }
    else if ((l_memoryModeChar == 'd') || (l_memoryModeChar == 'D'))
    {
        l_memoryMode = 1;
    }
    else if ((l_memoryModeChar == 'p') || (l_memoryModeChar == 'P'))
    {
        l_memoryMode = 2;
    }
    else
    {
        ecmdOutputError("ciprwreadmem - Error invalid memory mode specified!\n");
        return ECMD_INVALID_ARGS;
    }


    // Get the number of bytes
    if (l_numBytes == ECMD_UNSET)
    {
        l_numBytes = (uint32_t)atoi(argv[2]);
    }

    // do not allow l_numBytes >= 512MB
    if (l_numBytes >= 0x20000000)
    {
        ecmdOutputError("ciprwreadmem - Number of bytes must be < 512MB\n");
        return ECMD_INVALID_ARGS;
    }

    l_rc = ecmdLooperInit(l_target, ECMD_SELECTED_TARGETS_LOOP, l_looperdata);
    if (l_rc) return l_rc;

    while (ecmdLooperNext(l_target, l_looperdata) && (!l_coeRc || l_coeMode))
    {

        l_rc = cipRWReadMem(l_target, l_address, l_numBytes, l_returnData, l_memoryMode);
        if (l_rc)
        {
            std::string printed;
            printed = "ciprwreadmem - Error occured performing ciprwreadmem on ";
            printed += ecmdWriteTarget(l_target) + "\n";
            ecmdOutputError(printed.c_str());
            l_coeRc = l_rc;
            continue;
        }
        else
        {
            l_validPosFound = true;
        }

        std::string printLine = ecmdWriteTarget(l_target);
        if (l_expectFlag)
        {
            uint32_t l_mismatchBit = 0;

            if (l_maskFlag)
            {
                l_returnData.setAnd(l_mask, 0, l_returnData.getBitLength());
            }

            if (!ecmdCheckExpected(l_returnData, l_expected, l_mismatchBit))
            {

                //@ make this stuff sprintf'd
                char outstr[300];
                std::string printed;
                printed = ecmdWriteTarget(l_target) + "\n";
                ecmdOutputError( printed.c_str() );
                if (l_mismatchBit != ECMD_UNSET)
                {
                    sprintf(outstr, "ciprwreadmem - Data miscompare occured at (address %lX) (bit %d) (byte %d:0x%X bit %d)\n",
                            (unsigned long)l_address, l_mismatchBit, l_mismatchBit/8,
                            l_mismatchBit/8, l_mismatchBit%8);
                    ecmdOutputError( outstr );
                }
                l_coeRc = ECMD_EXPECT_FAILURE;
                continue;
            }
        }
        else
        {
            if (l_filename != NULL)
            {
	            l_rc = l_returnData.writeFile(l_filename, ECMD_SAVE_FORMAT_BINARY_DATA);

	            if (l_rc)
                {
                    std::string printed;
	                printed = "ciprwreadmem - Problems occurred writing data into file";
                    printed += l_filename;
                    printed +=  +"\n";
	                ecmdOutputError(printed.c_str());
                    break;
	            }
	            ecmdOutput( printLine.c_str() );
            }
            else if (l_dcardfilename != NULL)
            {
	            std::string dataStr = ecmdWriteDataFormatted(l_returnData, "memd", l_address);
	            std::ofstream ops;
	            ops.open(l_dcardfilename);
	            if (ops.fail()) {
	                char mesg[1000];
	                sprintf(mesg, "Unable to open file : %s for write", l_dcardfilename);
	                ecmdOutputError(mesg);
	                return ECMD_DBUF_FOPEN_FAIL;
	            }
	            if (dataStr[0] != '\n')
                {
	                printLine += "\n";
	            }
	            printLine += dataStr;
	            ops << printLine.c_str();
	            ops.close();

            }
            else
            {

	            std::string dataStr = ecmdWriteDataFormatted(l_returnData, l_outputformat, l_address);
	            if (dataStr[0] != '\n')
                {
	                printLine += "\n";
	            }
	            printLine += dataStr;
	            ecmdOutput( printLine.c_str() );
            }
        }
    }
    // l_coeRc will be the return code from in the loop, coe mode or not.
    if (l_coeRc) return l_coeRc;

    // This is an error common across all UI functions
    if (!l_validPosFound)
    {
        ecmdOutputError("ciprwreadmem - Unable to find a valid chip to execute command on\n");
        return ECMD_TARGET_NOT_CONFIGURED;
    }

    return l_rc;
}

uint32_t cipRWWriteMemUser(int argc, char * argv[])
{
    // ciprwwritemem <address> <data> <I|D|P>

    uint32_t l_rc = ECMD_SUCCESS, l_coeRc = ECMD_SUCCESS;

    ecmdLooperData l_looperdata;            ///< Store internal Looper data
    std::string l_inputformat = "x";          ///< Input format - default to 'x'
    ecmdDataBuffer l_inputData;             ///< Buffer to hold the data intended for memory
    std::list<ecmdMemoryEntry> l_memdata;   ///< Data from the D-Card format file
    std::list<ecmdMemoryEntry>::iterator l_memdataIter; ///< to iterate on l_memdata list
    ecmdMemoryEntry l_memEntry;             ///< to store data from the user
    bool l_validPosFound = false;           ///< Did the looper find anything?
    ecmdChipTarget l_target;                ///< Current target being operated on
    uint32_t l_address = 0;                 ///< The address from the command line
    int l_match;                            ///< For sscanf
    char l_memoryModeChar;                  ///< Which memory to access from the command line
    std::string printLine;                  ///< Output data

    /************************************************************************/
    /* Parse Local FLAGS here!                                              */
    /************************************************************************/
    /* get format flag, if it's there */
    char * l_formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
    if (l_formatPtr != NULL)
    {
        l_inputformat = l_formatPtr;
    }
    /* Get the filename if -fb is specified */
    char * l_filename = ecmdParseOptionWithArgs(&argc, &argv, "-fb");

    /* Get the filename to file in D-Card format */
    char *l_dcardfilename = ecmdParseOptionWithArgs(&argc, &argv, "-fd");

    if (((l_filename != NULL) || (l_dcardfilename != NULL)) && (l_formatPtr != NULL) )
    {
        ecmdOutputError("ciprwwritemem - Options -f and -i can't be specified together for format. Specify either one.\n");
        return ECMD_INVALID_ARGS;
    }

    if ((l_dcardfilename != NULL) && (l_filename != NULL))
    {
        ecmdOutputError("ciprwwritemem - Options -fb and -fd can't be specified together for format. Specify either one.\n");
        return ECMD_INVALID_ARGS;
    }
    /************************************************************************/
    /* Parse Common Cmdline Args                                            */
    /************************************************************************/
    l_rc = ecmdCommandArgs(&argc, &argv);
    if (l_rc) return l_rc;

    /* Global args have been parsed, we can read if -coe was given */
    bool l_coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

    if ((argc < 4) && ((l_filename == NULL) && (l_dcardfilename == NULL))) //target + address + data + mode
    {
        ecmdOutputError("ciprwwritemem - Too few arguments specified; you need at least a target, address, data to write, and mode.\n");
        ecmdOutputError("ciprwwritemem - Type 'ciprwwritemem -h' for usage.\n");
        return ECMD_INVALID_ARGS;
    }
    else if((argc < 3) && (l_filename != NULL))
    {
        ecmdOutputError("ciprwwritemem - Too few arguments specified; you need at least a target, address, input data file, and mode.\n");
        ecmdOutputError("ciprwwritemem - Type 'ciprwwritemem -h' for usage.\n");
        return ECMD_INVALID_ARGS;
    }
    else if(((argc > 4) && ((l_filename == NULL) && (l_dcardfilename == NULL))) || ((argc > 3) && (l_filename != NULL)))
    {
        ecmdOutputError("ciprwwritemem - Too many arguments specified; you only need a target, address, input data|file, and mode.\n");
        ecmdOutputError("ciprwwritemem - Type 'ciprwwritemem -h' for usage.\n");
        return ECMD_INVALID_ARGS;
    }
    else if((argc > 3) && (l_dcardfilename != NULL))
    {
        ecmdOutputError("ciprwwritemem - Too many arguments specified; you only need a target, an address offset, a dcard file, and mode.\n");
        ecmdOutputError("ciprwwritemem - Type 'ciprwwritemem -h' for usage.\n");
        return ECMD_INVALID_ARGS;
    }

    //Setup the target that will be used to query the system config
    std::string l_chipType, l_chipUnitType;
    l_rc = ecmdParseChipField(argv[0], l_chipType, l_chipUnitType, true /* supports wildcard usage */);
    if (l_rc) {
        ecmdOutputError("ciprwreadmem - Wildcard character detected however it is not being used correctly.\n");
        return l_rc;
    }

    l_target.cageState = l_target.nodeState = l_target.slotState = l_target.posState = ECMD_TARGET_FIELD_WILDCARD;
    l_target.chipUnitTypeState = l_target.chipUnitNumState = l_target.threadState = ECMD_TARGET_FIELD_UNUSED;

    if (l_chipType == "x") {
        l_target.chipTypeState = ECMD_TARGET_FIELD_WILDCARD;
    } else {
        l_target.chipType = l_chipType;
        l_target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    }

    if (l_chipUnitType != "") {
        l_target.chipUnitType = l_chipUnitType;
        l_target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
        l_target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
    }

    // Get the address
    if ((l_dcardfilename == NULL) || (l_dcardfilename != NULL && argv[1] != NULL))
    {
        if (!ecmdIsAllHex(argv[1]))
        {
            ecmdOutputError("ciprwwritemem - Non-hex characters detected in address field\n");
            return ECMD_INVALID_ARGS;
        }
        l_match = sscanf(argv[1], "%x", &l_address);
        if (l_match != 1)
        {
            ecmdOutputError("Error occurred processing address!\n");
            return ECMD_INVALID_ARGS;
        }
    }

    // get Instruction, Data, or Physical memory
    if(l_filename == NULL && l_dcardfilename == NULL)
    {
        l_memoryModeChar = argv[3][0];
    }
    else
    {
        l_memoryModeChar = argv[2][0];
    }

    uint32_t l_memoryMode = 0;
    // convert memory mode to int
    if ((l_memoryModeChar == 'i') || (l_memoryModeChar == 'I'))
    {
        l_memoryMode = 0;
    }
    else if ((l_memoryModeChar == 'd') || (l_memoryModeChar == 'D'))
    {
        l_memoryMode = 1;
    }
    else if ((l_memoryModeChar == 'p') || (l_memoryModeChar == 'P'))
    {
        l_memoryMode = 2;
    }
    else
    {
        ecmdOutputError("ciprwwritemem - Error invalid memory mode specified!\n");
        return ECMD_INVALID_ARGS;
    }

    // Read in the input data
    if(l_filename != NULL) {
        l_rc = l_inputData.readFile(l_filename, ECMD_SAVE_FORMAT_BINARY_DATA);
        if (l_rc)
        {
            printLine = "ciprwwritemem - Problems occurred parsing input data from file ";
            printLine += l_filename;
            printLine += ", must be an invalid format\n";
            ecmdOutputError(printLine.c_str());
            return l_rc;
        }
        l_memEntry.address = l_address;
        l_memEntry.data = l_inputData;
        l_memdata.push_back(l_memEntry);

    }
    else if(l_dcardfilename != NULL)
    {
        l_rc = ecmdReadDcard(l_dcardfilename, l_memdata, l_address);
        if (l_rc)
        {
            printLine = "ciprwwritemem - Problems occurred parsing input data from file ";
            printLine += l_dcardfilename;
            printLine += ", must be an invalid format\n";
            ecmdOutputError(printLine.c_str());
            return l_rc;
        }
    }
    else
    {
        l_rc = ecmdReadDataFormatted(l_inputData, argv[2] , l_inputformat);
        if (l_rc)
        {
            ecmdOutputError("ciprwwritemem - Problems occurred parsing input data, must be an invalid format\n");
            return l_rc;
        }
        l_memEntry.address = l_address;
        l_memEntry.data = l_inputData;
        l_memdata.push_back(l_memEntry);
    }

    /************************************************************************/
    /* Kickoff Looping Stuff                                                */
    /************************************************************************/
    l_rc = ecmdLooperInit(l_target, ECMD_SELECTED_TARGETS_LOOP, l_looperdata);
    if (l_rc) return l_rc;

    while (ecmdLooperNext(l_target, l_looperdata) && (!l_coeRc || l_coeMode))
    {
        for (l_memdataIter = l_memdata.begin(); l_memdataIter != l_memdata.end(); l_memdataIter++)
        {

            /* Let's verify we have an even byte length of data */
            if (l_memdataIter->data.getBitLength() != (l_memdataIter->data.getByteLength() * 8)) {
                ecmdOutputError("ciprwwritemem - Invalid data, must specify an even byte length of data\n");
                l_rc = ECMD_INVALID_ARGS;
                break;
            }
            else if (l_memdataIter->data.getByteLength() == 0)
            {
                ecmdOutputError("ciprwwritemem - Invalid data, byte length of zero detected on incoming data\n");
                l_rc = ECMD_INVALID_ARGS;
                break;
            }

            l_rc = cipRWWriteMem(l_target, l_memdataIter->address, l_memdataIter->data.getByteLength(), l_memdataIter->data, l_memoryMode);
            if (l_rc)
            {
                printLine = "ciprwwritemem - Error occured performing ciprwwritemem on ";
                printLine += ecmdWriteTarget(l_target) + "\n";
                ecmdOutputError( printLine.c_str() );
                l_coeRc = l_rc;
                continue;
            }
            else
            {
                l_validPosFound = true;
            }
        }

        // Write out who we wrote too
        if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
            printLine = ecmdWriteTarget(l_target) + "\n";
            ecmdOutput(printLine.c_str());
        }
    }
    // l_coeRc will be the return code from in the loop, coe mode or not.
    if (l_coeRc) return l_coeRc;

    // This is an error common across all UI functions
    if (!l_validPosFound && !l_rc) {
        ecmdOutputError("ciprwwritemem - Unable to find a valid chip to execute command on\n");
        return ECMD_TARGET_NOT_CONFIGURED;
    }

    return l_rc;
}

uint32_t cipRWGetDcrUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdChipTarget subTarget;        ///< Current target
  ecmdLooperData subLooperdata;            ///< Store internal Looper data
  bool validPosFound = false, validChipFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  std::list<ecmdIndexEntry> entries;    ///< List of dcr's to fetch, to use cipRWGetDcrMultiple
  std::list<ecmdIndexEntry> entries_copy;    ///< List of dcr's to fetch, to use cipRWGetDcrMultiple
  ecmdIndexEntry  entry;         ///< Dcr entry to fetch
  int idx;
  std::string function;         ///< What function are we running (based on type)
  int numEntries = 1;           ///< Number of consecutive entries to retrieve
  int startEntry = 0;           ///< Entry to start on
  char buf[100];                ///< Temporary string buffer
  std::string sprName;
  ecmdProcRegisterInfo procInfo; ///< Used to figure out if an SPR is threaded or not 
  std::string chipType, chipUnitType;

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
  function = "ciprwgetdcr";
  sprName = "dcr";

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
    printed = function + " - Too few arguments specified; you need at least one dcr.\n";
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

  /* Walk through the arguments and create our list of dcrs */
  validChipFound = ecmdIsAllDecimal(argv[0]) ? false : ecmdIsValidChip(argv[0], target);
  if(!validChipFound && ecmdIsAllDecimal(argv[0]))
  {
     startEntry = atoi(argv[0]);
     if (argc > 1) {
       numEntries = atoi(argv[1]);
     }  
  }
  else
  {
     if (argc < 2) {
        printed = function + " - Too few arguments specified; you need at least one dcr.\n";
        ecmdOutputError(printed.c_str());
        printed = function + " - Type '"; printed += function; printed += " -h' for usage.\n";
        ecmdOutputError(printed.c_str());
        return ECMD_INVALID_ARGS;
     }
     else
     {
         //Setup the target
         //target.chipType = argv[0];
         startEntry = atoi(argv[1]);
         if (argc > 2) {
           numEntries = atoi(argv[2]);
         }
          rc = ecmdParseChipField(argv[0], chipType, chipUnitType, true /* supports wildcard usage */);
          if (rc) {
             ecmdOutputError("Wildcard character detected.\n");
             return rc;
          }
     }
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
      rc = cipRWGetDcrMultiple(subTarget, entries_copy);

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

uint32_t cipRWPutDcrUser(int argc, char * argv[]) {
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
  // Commenting numBits out to avoid compiler warnings.  Nothing was actually being done with the values.
  //uint32_t numBits = 0;         ///< Number of bits to insert data
  std::string function;         ///< Current function being run based on daType
  std::string sprName;
  char* cmdlinePtr = NULL;         ///< Pointer to data in argv array
  ecmdProcRegisterInfo procInfo; ///< Used to figure out if an SPR is threaded or not
  bool validChipFound = false;
  std::string chipType, chipUnitType;
  int idx = 0;

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
  function = "ciprwputdcr";
  sprName = "dcr";

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

  validChipFound = ecmdIsValidChip(argv[0], target);
  if(validChipFound)
  {
    if (argc < 3) {
       printed = function + " - Too few arguments specified; you need at least an gpr/fpr Name  and some data.\n";
       ecmdOutputError(printed.c_str());
       printed = function + " - Type '"; printed += function; printed += " -h' for usage.\n";
       ecmdOutputError(printed.c_str());
       return ECMD_INVALID_ARGS;
    }
    idx = 1;  //gpr starts from first index
    rc = ecmdParseChipField(argv[0], chipType, chipUnitType, true /* supports wildcard usage */);
    if (rc) {
        ecmdOutputError("Wildcard character detected.\n");
        return rc;
    }

  }

  entry = (uint32_t)atoi(argv[0 + idx]);

  if (argc == (4 + idx)) {

    if (!ecmdIsAllDecimal(argv[1 + idx])) {
      printed = function + " - Non-decimal numbers detected in startbit field\n";
      ecmdOutputError(printed.c_str());
      return ECMD_INVALID_ARGS;
    }
    startBit = (uint32_t)atoi(argv[1 + idx]);

    if (!ecmdIsAllDecimal(argv[2 + idx])) {
      printed = function + " - Non-decimal numbers detected in numbits field\n";
      ecmdOutputError(printed.c_str());
      return ECMD_INVALID_ARGS;
    }
    //numBits = (uint32_t)atoi(argv[2 + idx]);
    
    rc = ecmdReadDataFormatted(cmdlineBuffer, argv[3], inputformat);
    if (rc) {
      printed = function + "Problems occurred parsing input data, must be an invalid format\n";
      ecmdOutputError(printed.c_str());
      return rc;
    }
  } else if (argc == (2 + idx)) {

    cmdlinePtr = argv[1 + idx];

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
        rc = cipRWGetDcr(subTarget, entry, buffer);
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

      rc = cipRWPutDcr(subTarget, entry, buffer);

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

uint32_t cipRWProcStatusUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  ecmdLooperData looperdata;            ///< Store internal Looper data
  bool validPosFound = false, validChipFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  std::string function;         ///< What function are we running (based on type)
  std::string chipType, chipUnitType;

  // Set commandline name based up on the type
  function = "ciprwprocstatus";

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
    printed = function + " - Too few arguments specified; you need to specify the target.\n";
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

  /* Walk through the arguments and create our list of dcrs */
  validChipFound = ecmdIsValidChip(argv[0], target);
  if(validChipFound)
  {
    rc = ecmdParseChipField(argv[0], chipType, chipUnitType, true /* supports wildcard usage */);
    if (rc) {
      ecmdOutputError("Wildcard character detected.\n");
      return rc;
    }
  }

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {
    cipRWProcStatus_t status;
    /* Actually go fetch the data */
    rc = cipRWProcStatus(target, status);

    if (rc) {
      printed = function + " - Error occured performing cipRWProcStatus on ";
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
    switch (status)
    {
        case CIP_RW_PROC_STATUS_UNKNOWN:
            printed = "UNKNOWN";
            break;
        case CIP_RW_PROC_STATUS_RUNNING:
            printed = "RUNNING";
            break;
        case CIP_RW_PROC_STATUS_STOPPED:
            printed = "STOPPED";
            break;
        case CIP_RW_PROC_STATUS_HALTED:
            printed = "HALTED";
            break;
        case CIP_RW_PROC_STATUS_CHECKSTOPPED:
            printed = "CHECKSTOPPED";
            break;
        case CIP_RW_PROC_STATUS_POWEROFF:
            printed = "POWEROFF";
            break;
        default:
            printed = function + " - unknown status code on ";
            printed += ecmdWriteTarget(target) + "\n";
            ecmdOutputError( printed.c_str() );
            printed = "";
            break;
    }
    ecmdOutput( printed.c_str() );

    ecmdOutput("\n");
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
#endif // CIP_REMOVE_RW_FUNCTIONS

#ifndef CIP_REMOVE_PMC_VOLTAGE_FUNCTIONS
uint32_t cipGetPmcVoltageUser(int argc, char * argv[])
{
    // cipgetpmcvoltage chip type mode [type mode]
    // type vdd or vcs
    // mode vid or mv
    // output defaults to decimal

    uint32_t l_rc = ECMD_SUCCESS, l_coeRc = ECMD_SUCCESS;

    ecmdLooperData l_looperdata;            ///< Store internal Looper data
    std::string l_outputformat = "d";       ///< Output format - default to decimal
    ecmdChipTarget l_target;                ///< Current target being operated on
    bool l_validPosFound = false;           ///< Did the looper find anything?

    /************************************************************************/
    /* Parse Local FLAGS here!                                              */
    /************************************************************************/

    /* get format flag, if it's there */
    char * l_outputFormatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
    if (l_outputFormatPtr != NULL)
    {
        l_outputformat = l_outputFormatPtr;
    }

    /************************************************************************/
    /* Parse Common Cmdline Args                                            */
    /************************************************************************/
    l_rc = ecmdCommandArgs(&argc, &argv);
    if (l_rc) return l_rc;

    /* Global args have been parsed, we can read if -coe was given */
    bool l_coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

    /************************************************************************/
    /* Parse Local ARGS here!                                               */
    /************************************************************************/

    // check number of arguments
    if (argc < 3)
    {
        ecmdOutputError("cipgetpmcvoltage - chip, type, and mode must be specified\n");
        ecmdOutputError("cipgetpmcvoltage - Type 'cipgetpmcvoltage -h' for usage.\n");
        return ECMD_INVALID_ARGS;
    }
    if ((argc != 3) && (argc != 5))
    {
        ecmdOutputError("cipgetpmcvoltage - Incorrect number of arguments specified.\n");
        ecmdOutputError("cipgetpmcvoltage - Type 'cipgetpmcvoltage -h' for usage.\n");
        return ECMD_INVALID_ARGS;
    }

    //Setup the target that will be used to query the system config
    std::string l_chipType, l_chipUnitType;
    l_rc = ecmdParseChipField(argv[0], l_chipType, l_chipUnitType, true /* supports wildcard usage */);
    if (l_rc)
    {
        ecmdOutputError("cipgetpmcvoltage - Wildcard character detected however it is not being used correctly.\n");
        return l_rc;
    }

    if (l_chipUnitType != "")
    {
        ecmdOutputError("cipgetpmcvoltage - chipUnit specified on the command line, this function doesn't support chipUnits.\n");
        return ECMD_INVALID_ARGS;
    }

    if (l_chipType == "x")
    {
        l_target.chipTypeState = ECMD_TARGET_FIELD_WILDCARD;
    }
    else
    {
        l_target.chipType = l_chipType;
        l_target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    }

    l_target.cageState = l_target.nodeState = l_target.slotState = l_target.posState = ECMD_TARGET_FIELD_WILDCARD;
    l_target.chipUnitTypeState = l_target.chipUnitNumState = l_target.threadState = ECMD_TARGET_FIELD_UNUSED;

    uint32_t l_vdd_mode = CIP_PMC_VOLTAGE_MODE_IGNORE;
    uint32_t l_vcs_mode = CIP_PMC_VOLTAGE_MODE_IGNORE;
    uint32_t * l_mode_pointer = NULL;

    //argv[1] vdd or vcs
    if (strcasecmp(argv[1], "vdd") == 0)
    {
        l_mode_pointer = &l_vdd_mode;
    }
    else if (strcasecmp(argv[1], "vcs") == 0)
    {
        l_mode_pointer = &l_vcs_mode;
    }
    else
    {
        ecmdOutputError("cipgetpmcvoltage - unknown voltage type. Use vdd or vcs.\n");
        return ECMD_INVALID_ARGS;
    }
    //argv[2] vid or mv
    if (strcasecmp(argv[2], "vid") == 0)
    {
        *l_mode_pointer = CIP_PMC_VOLTAGE_MODE_VID;
    }
    else if (strcasecmp(argv[2], "mv") == 0)
    {
        *l_mode_pointer = CIP_PMC_VOLTAGE_MODE_VOLT;
    }
    else
    {
        ecmdOutputError("cipgetpmcvoltage - unknown voltage mode. Use vid or mv.\n");
        return ECMD_INVALID_ARGS;
    }

    if (argc > 3)
    {
        //argv[3] vdd or vcs
        if (strcasecmp(argv[3], "vdd") == 0)
        {
            if (l_vdd_mode != CIP_PMC_VOLTAGE_MODE_IGNORE)
            {
                ecmdOutputError("cipgetpmcvoltage - vdd already selected\n");
                return ECMD_INVALID_ARGS;
            }
            l_mode_pointer = &l_vdd_mode;
        }
        else if (strcasecmp(argv[3], "vcs") == 0)
        {
            if (l_vcs_mode != CIP_PMC_VOLTAGE_MODE_IGNORE)
            {
                ecmdOutputError("cipgetpmcvoltage - vcs already selected\n");
                return ECMD_INVALID_ARGS;
            }
            l_mode_pointer = &l_vcs_mode;
        }
        else
        {
            ecmdOutputError("cipgetpmcvoltage - unknown voltage type. Use vdd or vcs.\n");
            return ECMD_INVALID_ARGS;
        }
        //argv[4] vid or mv
        if (strcasecmp(argv[4], "vid") == 0)
        {
            *l_mode_pointer = CIP_PMC_VOLTAGE_MODE_VID;
        }
        else if (strcasecmp(argv[4], "mv") == 0)
        {
            *l_mode_pointer = CIP_PMC_VOLTAGE_MODE_VOLT;
        }
        else
        {
            ecmdOutputError("cipgetpmcvoltage - unknown voltage mode. Use vid or mv.\n");
            return ECMD_INVALID_ARGS;
        }
    }

    /************************************************************************/
    /* Kickoff Looping Stuff                                                */
    /************************************************************************/
    l_rc = ecmdLooperInit(l_target, ECMD_SELECTED_TARGETS_LOOP, l_looperdata);
    if (l_rc) return l_rc;

    while (ecmdLooperNext(l_target, l_looperdata) && (!l_coeRc || l_coeMode))
    {
        uint32_t l_vdd = 0x0;
        int32_t l_vcs = 0x0;
        l_rc = cipGetPmcVoltage(l_target, l_vdd_mode, l_vdd, l_vcs_mode, l_vcs);
        if (l_rc)
        {
            std::string l_printed;
            l_printed = "cipgetpmcvoltage - Error occured performing cipgetpmcvoltage on ";
            l_printed += ecmdWriteTarget(l_target) + "\n";
            ecmdOutputError(l_printed.c_str());
            l_coeRc = l_rc;
            continue;
        }
        else
        {
            l_validPosFound = true;

            std::string l_printed = ecmdWriteTarget(l_target);
            ecmdOutput(l_printed.c_str());
            if (l_outputformat == "d")
            {
                if (l_vdd_mode != CIP_PMC_VOLTAGE_MODE_IGNORE)
                {
                    char buf[20];
                    snprintf(buf, 20, "vdd %d%s", l_vdd, (l_vcs_mode == CIP_PMC_VOLTAGE_MODE_IGNORE ? "\n" : "    "));
                    ecmdOutput(buf);
                }
                if (l_vcs_mode != CIP_PMC_VOLTAGE_MODE_IGNORE)
                {
                    char buf[20];
                    snprintf(buf, 20, "vcs %d\n", l_vcs);
                    ecmdOutput(buf);
                }
            }
            else
            {
                if (l_vdd_mode != CIP_PMC_VOLTAGE_MODE_IGNORE)
                {
                    char buf[20];
                    snprintf(buf, 20, "vdd 0x%02X%s\n", l_vdd, (l_vcs_mode == CIP_PMC_VOLTAGE_MODE_IGNORE ? "\n" : "    "));
                    ecmdOutput(buf);
                }
                if (l_vcs_mode != CIP_PMC_VOLTAGE_MODE_IGNORE)
                {
                    char buf[20];
                    snprintf(buf, 20, "vcs 0x%02X\n", l_vcs);
                    ecmdOutput(buf);
                }
            }
        }
    }
    // l_coeRc will be the return code from in the loop, coe mode or not.
    if (l_coeRc) return l_coeRc;

    // This is an error common across all UI functions
    if (!l_validPosFound && !l_rc) {
        ecmdOutputError("cipgetpmcvoltage - Unable to find a valid chip to execute command on\n");
        return ECMD_TARGET_NOT_CONFIGURED;
    }

    return l_rc;
}

uint32_t cipPutPmcVoltageUser(int argc, char * argv[])
{
    // cipputpmcvoltage chip type mode value [type mode value] [-step size]
    // type vdd or vcs
    // mode vid or mv
    // value if mv for vcs may be negative, vid values are uint8_t
    // size for -step are uint8_t
    // input defaults to decimal

    uint32_t l_rc = ECMD_SUCCESS, l_coeRc = ECMD_SUCCESS;

    ecmdLooperData l_looperdata;            ///< Store internal Looper data
    std::string l_inputformat = "d";        ///< Input format - default to decimal
    ecmdChipTarget l_target;                ///< Current target being operated on
    bool l_validPosFound = false;           ///< Did the looper find anything?
    uint8_t l_step_size = 0x0;              ///< Step size input

    /************************************************************************/
    /* Parse Local FLAGS here!                                              */
    /************************************************************************/

    /* get format flag, if it's there */
    char * l_inputFormatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
    if (l_inputFormatPtr != NULL)
    {
        l_inputformat = l_inputFormatPtr;
    }

    /* check for step size arguement */
    char * l_stepSizePtr = ecmdParseOptionWithArgs(&argc, &argv, "-step");
    if (l_stepSizePtr != NULL)
    {
        uint32_t l_tempStepSize = 0x0;
        int l_formatFound = 0;
        if (l_inputformat == "d")
        {
            if (ecmdIsAllDecimal(l_stepSizePtr))
            {
                l_formatFound = sscanf(l_stepSizePtr, "%u", &l_tempStepSize);
            }
        }
        else
        {
            if (ecmdIsAllHex(l_stepSizePtr))
            {
                l_formatFound = sscanf(l_stepSizePtr, "%X", &l_tempStepSize);
            }
        }
        if (l_formatFound != 1)
        {
            ecmdOutputError("cipputpmcvoltage - invalid step size specified\n");
            ecmdOutputError("cipputpmcvoltage - Type 'cipputpmcvoltage -h' for usage.\n");
            return ECMD_INVALID_ARGS;
        }
        if (l_tempStepSize > 0xFF)
        {
            ecmdOutputError("cipputpmcvoltage - step size is greater than allowed\n");
            return ECMD_INVALID_ARGS;
        }
        l_step_size = l_tempStepSize;
    }

    /************************************************************************/
    /* Parse Common Cmdline Args                                            */
    /************************************************************************/
    l_rc = ecmdCommandArgs(&argc, &argv);
    if (l_rc) return l_rc;

    /* Global args have been parsed, we can read if -coe was given */
    bool l_coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

    /************************************************************************/
    /* Parse Local ARGS here!                                               */
    /************************************************************************/

    // check number of arguments
    if (argc < 4)
    {
        ecmdOutputError("cipputpmcvoltage - chip, type, mode, and value must be specified\n");
        ecmdOutputError("cipputpmcvoltage - Type 'cipputpmcvoltage -h' for usage.\n");
        return ECMD_INVALID_ARGS;
    }
    if ((argc != 4) && (argc != 7))
    {
        ecmdOutputError("cipputpmcvoltage - Incorrect number of arguments specified.\n");
        ecmdOutputError("cipputpmcvoltage - Type 'cipputpmcvoltage -h' for usage.\n");
        return ECMD_INVALID_ARGS;
    }

    //Setup the target that will be used to query the system config
    std::string l_chipType, l_chipUnitType;
    l_rc = ecmdParseChipField(argv[0], l_chipType, l_chipUnitType, true /* supports wildcard usage */);
    if (l_rc)
    {
        ecmdOutputError("cipputpmcvoltage - Wildcard character detected however it is not being used correctly.\n");
        return l_rc;
    }

    if (l_chipUnitType != "")
    {
        ecmdOutputError("cipputpmcvoltage - chipUnit specified on the command line, this function doesn't support chipUnits.\n");
        return ECMD_INVALID_ARGS;
    }

    if (l_chipType == "x")
    {
        l_target.chipTypeState = ECMD_TARGET_FIELD_WILDCARD;
    }
    else
    {
        l_target.chipType = l_chipType;
        l_target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    }

    l_target.cageState = l_target.nodeState = l_target.slotState = l_target.posState = ECMD_TARGET_FIELD_WILDCARD;
    l_target.chipUnitTypeState = l_target.chipUnitNumState = l_target.threadState = ECMD_TARGET_FIELD_UNUSED;

    uint32_t l_vdd_mode = CIP_PMC_VOLTAGE_MODE_IGNORE;
    uint32_t l_vcs_mode = CIP_PMC_VOLTAGE_MODE_IGNORE;
    uint32_t * l_mode_pointer = NULL;
    bool l_vdd_select = false;
    uint32_t l_vdd = 0x0;
    int32_t l_vcs = 0x0;

    //argv[1] vdd or vcs
    if (strcasecmp(argv[1], "vdd") == 0)
    {
        l_mode_pointer = &l_vdd_mode;
        l_vdd_select = true;
    }
    else if (strcasecmp(argv[1], "vcs") == 0)
    {
        l_mode_pointer = &l_vcs_mode;
    }
    else
    {
        ecmdOutputError("cipputpmcvoltage - unknown voltage type. Use vdd or vcs.\n");
        return ECMD_INVALID_ARGS;
    }
    //argv[2] vid or mv 
    if (strcasecmp(argv[2], "vid") == 0)
    {
        *l_mode_pointer = CIP_PMC_VOLTAGE_MODE_VID;
    }
    else if (strcasecmp(argv[2], "mv") == 0)
    {
        *l_mode_pointer = CIP_PMC_VOLTAGE_MODE_VOLT;
    }
    else
    {
        ecmdOutputError("cipputpmcvoltage - unknown voltage mode. Use vid or mv.\n");
        return ECMD_INVALID_ARGS; 
    }
    //argv[3] value
    int l_formatFound = 0;
    if (l_inputformat == "d")
    {
        if (l_vdd_select)
        {
            if (ecmdIsAllDecimal(argv[3]))
            {
                l_formatFound = sscanf(argv[3], "%u", &l_vdd);
            }
        }
        else
        {
            char * l_tempargv = argv[3];
            if (l_tempargv[0] == '-')
            {
                l_tempargv++;
            }

            if (ecmdIsAllDecimal(l_tempargv))
            {
                l_formatFound = sscanf(argv[3], "%d", &l_vcs);
            }
        }
    }
    else
    {
        if (l_vdd_select)
        {
            if (ecmdIsAllHex(argv[3]))
            {
                l_formatFound = sscanf(argv[3], "%X", &l_vdd);
            }
        }
        else
        {
            if (ecmdIsAllHex(argv[3]))
            {
                l_formatFound = sscanf(argv[3], "%X", (uint32_t *) &l_vcs);
            }
        }
    }
    if (l_formatFound != 1)
    {
        ecmdOutputError("cipputpmcvoltage - invalid voltage value specified\n");
        ecmdOutputError("cipputpmcvoltage - Type 'cipputpmcvoltage -h' for usage.\n");
        return ECMD_INVALID_ARGS;
    }

    if (argc > 4)
    {
        l_vdd_select = false;
        //argv[4] vdd or vcs
        if (strcasecmp(argv[4], "vdd") == 0)
        {
            l_mode_pointer = &l_vdd_mode;
            l_vdd_select = true;
        }
        else if (strcasecmp(argv[4], "vcs") == 0)
        {
            l_mode_pointer = &l_vcs_mode;
        }
        else
        {
            ecmdOutputError("cipputpmcvoltage - unknown voltage type. Use vdd or vcs.\n");
            return ECMD_INVALID_ARGS;
        }
        //argv[5] vid or mv 
        if (strcasecmp(argv[5], "vid") == 0)
        {
            *l_mode_pointer = CIP_PMC_VOLTAGE_MODE_VID;
        }
        else if (strcasecmp(argv[5], "mv") == 0)
        {
            *l_mode_pointer = CIP_PMC_VOLTAGE_MODE_VOLT;
        }
        else
        {
            ecmdOutputError("cipputpmcvoltage - unknown voltage mode. Use vid or mv.\n");
            return ECMD_INVALID_ARGS; 
        }
        //argv[6] value
        l_formatFound = 0;
        if (l_inputformat == "d")
        {
            if (l_vdd_select)
            {
                if (ecmdIsAllDecimal(argv[6]))
                {
                    l_formatFound = sscanf(argv[6], "%u", &l_vdd);
                }
            }
            else
            {
                char * l_tempargv = argv[6];
                if (l_tempargv[0] == '-')
                {
                    l_tempargv++;
                }

                if (ecmdIsAllDecimal(l_tempargv))
                {
                    l_formatFound = sscanf(argv[6], "%d", &l_vcs);
                }
            }
        }
        else
        {
            if (l_vdd_select)
            {
                if (ecmdIsAllHex(argv[6]))
                {
                    l_formatFound = sscanf(argv[6], "%X", &l_vdd);
                }
            }
            else
            {
                if (ecmdIsAllHex(argv[6]))
                {
                    l_formatFound = sscanf(argv[6], "%X", (uint32_t *) &l_vcs);
                }
            }
        }
        if (l_formatFound != 1)
        {
            ecmdOutputError("cipputpmcvoltage - invalid voltage value specified\n");
            ecmdOutputError("cipputpmcvoltage - Type 'cipputpmcvoltage -h' for usage.\n");
            return ECMD_INVALID_ARGS;
        }
    }

    if ((l_vdd_mode == CIP_PMC_VOLTAGE_MODE_VID) && (l_vdd > 0xFF))
    {
        ecmdOutputError("cipputpmcvoltage - vdd vid mode voltage value is greater than allowed\n");
        return ECMD_INVALID_ARGS;
    }
    if ((l_vcs_mode == CIP_PMC_VOLTAGE_MODE_VID) && (l_vcs > 0xFF))
    {
        ecmdOutputError("cipputpmcvoltage - vcs vid mode voltage value is greater than allowed\n");
        return ECMD_INVALID_ARGS;
    }

    /************************************************************************/
    /* Kickoff Looping Stuff                                                */
    /************************************************************************/
    l_rc = ecmdLooperInit(l_target, ECMD_SELECTED_TARGETS_LOOP, l_looperdata);
    if (l_rc) return l_rc;
    
    while (ecmdLooperNext(l_target, l_looperdata) && (!l_coeRc || l_coeMode))
    {
        l_rc = cipPutPmcVoltage(l_target, l_vdd_mode, l_vdd, l_vcs_mode, l_vcs, l_step_size);
        if (l_rc)
        {
            std::string l_printed;
            l_printed = "cipputpmcvoltage - Error occured performing cipputpmcvoltage on ";
            l_printed += ecmdWriteTarget(l_target) + "\n";
            ecmdOutputError(l_printed.c_str());
            l_coeRc = l_rc;
            continue;
        }
        else
        {
            l_validPosFound = true;
    
            std::string l_printed = ecmdWriteTarget(l_target);
            l_printed += "\n";
            ecmdOutput(l_printed.c_str());
        }
    }
    // l_coeRc will be the return code from in the loop, coe mode or not.
    if (l_coeRc) return l_coeRc;

    // This is an error common across all UI functions
    if (!l_validPosFound && !l_rc) {
        ecmdOutputError("cipputpmcvoltage - Unable to find a valid chip to execute command on\n");
        return ECMD_TARGET_NOT_CONFIGURED;
    }

    return l_rc;
}
#endif // CIP_REMOVE_PMC_VOLTAGE_FUNCTIONS

#ifndef CIP_REMOVE_MEMORY_FUNCTIONS
uint32_t cipGetMemProcVarUser(int argc, char * argv[])
{
  uint32_t rc = ECMD_SUCCESS;
  char buf[2000];
  uint32_t coeRc = ECMD_SUCCESS;
  std::string printed;
  std::string temp;
  ecmdChipTarget target;                        ///< Current target being operated on
  ecmdChipTarget cuTarget;                      ///< Current target being operated on for the chipUnit
  ecmdLooperData looperdata;
  bool validPosFound = false;                   ///< Did the looper find anything?
  uint32_t l_addressSize;
  bool addrTypeSet= false;

  // Declare/setup additional parameters.
  ecmdDataBuffer l_bufTags, l_bufECC, l_bufErr;
  ecmdDataBuffer l_address_96(96);
  ecmdDataBuffer l_memDataRead;
  ecmdDataBuffer l_realAddress;
  uint32_t       l_bytes = 0;
  cipXlateVariables l_xlate;


  // set defaults to xlate structure
  l_xlate.tagsActive = true;
  l_xlate.mode32bit = false;
  l_xlate.writeECC = false;
  l_xlate.manualXlateFlag = false;
  l_xlate.addrType = CIP_MAINSTORE_REAL_ADDR;
  l_xlate.partitionId = 0;
 
  // current option is to override the partition Id, if the user does not provide any
  // we set the partition id to 0
  char* partionId= ecmdParseOptionWithArgs(&argc, &argv, "-partitionId");
  if (partionId != NULL ){
      if (!ecmdIsAllDecimal(partionId)) {
           ecmdOutputError("cipgetmemprocvar- Non-Decimal characters detected in partition field\n");
           return ECMD_INVALID_ARGS;
      }else{
           l_xlate.partitionId = (uint32_t)atoi(partionId);
      }
  }

  // clear all the databuffers
  l_bufTags.clear();
  l_bufECC.clear();
  l_bufErr.clear();
  l_memDataRead.clear();
  l_realAddress.clear();

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
    ecmdOutputError("cipgetmemprocvar - Too few arguments specified; you need Address type and address.\n");
    ecmdOutputError("cipgetmemprocvar - Type 'cipgetmemprocvar -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  
  // now check to see what is the type of address we are looking at
  if( argc >=1){
      temp = argv[0];
      transform(temp.begin(), temp.end(), temp.begin(), (int(*)(int)) tolower);
          if(temp == "real"){
            l_xlate.addrType = CIP_MAINSTORE_REAL_ADDR;
            addrTypeSet=true;
          } else if (temp == "effective") {
            l_xlate.addrType = CIP_MAINSTORE_EFFECTIVE_ADDR;
            addrTypeSet=true;
          } else if (temp == "virtual"){
            l_xlate.addrType = CIP_MAINSTORE_VIRTUAL_ADDR;
            addrTypeSet=true;
          } else {
              ecmdOutputError("cipgetmemprocvar- Address type needs to be one of the address types(real/effective/virtual).\n");
              return ECMD_INVALID_ARGS;
         }
  }

  //check for address if the type is set
  if(addrTypeSet){
     // check if the address supplied is valid hex
      if( argc >=2 ){
          if (!ecmdIsAllHex(argv[1])) {
            ecmdOutputError("cipgetmemprocvar- Non-hex characters detected in address field\n");
            return ECMD_INVALID_ARGS;
          }
          //  we are restricting address to 96 bits
          if ( strlen(argv[1]) > 24 ) // strlen does NOT count NULL terminator
          {
             ecmdOutputError("cipgetmemprocvar- Address field is too large (>24 chars). It is restricted to 96 bits (12bytes)\n");
             return ECMD_INVALID_ARGS;
          }
          l_address_96.flushTo0();
          l_addressSize = strlen(argv[1]) * 4; //total bits
          rc = l_address_96.insertFromHexRight(argv[1], (l_address_96.getBitLength() - l_addressSize),l_addressSize );
      } else {
          ecmdOutputError("cipgetmemprocvar- Adress needs to be provided to get mem\n");
          return ECMD_INVALID_ARGS;
      }
  }

  // now look for # of bytes, we need to make sure that this is at word boundary and multiple of 8 bytes
  if( argc >=3){
    // if we come here then validate the # of bytes
    uint32_t noBytes = (uint32_t)atoi(argv[2]);
    if(noBytes % 8) 
    {
       ecmdOutputError("cipgetmemprocvar- No of bytes must be at word boundary and multiples of 8 bytes\n");
       return ECMD_INVALID_ARGS;
    }
    // now set the bytes
    l_bytes = noBytes;
  }

  // find chipUnit type
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  std::string  processingUnitName;

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  //< So we can determine processing unit name
  if ( ecmdLooperNext(target, looperdata) ) {
    rc = ecmdGetProcessingUnit(target,processingUnitName);
    if (rc) return rc;
  }

  if (processingUnitName.empty()) {
    ecmdOutputError("cipputgetprocvar- unable to find chipunit type for this command\n");
    return ECMD_INVALID_ARGS;
  }
  // set up target at chipUnit level
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitType = processingUnitName;
  target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
  // setting the thread state to wildcard to accept any thread value passed by user
  target.threadState = ECMD_TARGET_FIELD_WILDCARD;

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/
  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  // I dont think we need a looper here, but does not harm.
  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

       rc = cipGetMemProcVariousAddrType(target,           // input
                                        l_address_96,         // input
                                        l_bytes,           // input
                                        l_xlate,           // input
                                        l_memDataRead,     // ouput
                                        l_bufTags,         // output (tags)
                                        l_bufECC,          // output (ecc)
                                        l_bufErr,          // output (err)
                                        l_realAddress);    // output
       if (rc != ECMD_SUCCESS)
       {
          // Failed to read mem now set message and bail out
          printed="\n";printed+=ecmdWriteTarget(target); printed+="\n"; ecmdOutput(printed.c_str());
          sprintf(buf, "cipgetmemprocvar - Failed to read from address 0x%s, rc = 0x%x\n", l_address_96.genHexLeftStr().c_str(), rc);
          coeRc=rc;
          ecmdOutputError(buf);
       }else{
         //the max bit length is 80 bits for the VA, so I will copy the 96 bits to 80bits and display
         ecmdDataBuffer ll_address_80(80);
         rc = l_address_96.extractPreserve(ll_address_80,16,80);
         if (rc != ECMD_SUCCESS)
         {
            // Failed to read mem now set message and bail out
            sprintf(buf, "cipgetmemprocvar - Failed extractPreserve\n");
            coeRc=rc;
            ecmdOutputError(buf);
            // we dont want to continue if there is ecmdDB issue, so break
            break;
         }
         // for looper inlcude the target info in the file.
         printed=ecmdWriteTarget(target); printed+="\n"; ecmdOutput(printed.c_str());
         printed= "Address Req          Bytes Req Mem Tags Mem ECC Err\n"; ecmdOutput(printed.c_str());
         printed= "==================== ========= ======== ===========\n"; ecmdOutput(printed.c_str());
         const char * l_ecc = "";
         if (l_bufErr.getBitLength())
         {
            l_ecc = l_bufErr.genHexLeftStr().c_str();
         }
         sprintf(buf,"%-20s %9d %-8s %-11s\n",ll_address_80.genHexLeftStr().c_str(),l_bytes,
                      l_bufTags.genHexLeftStr().c_str(),l_ecc);
         ecmdOutput(buf);
         // now display the  data
         ecmdOutput("GETMEM DATA READ\n");
         ecmdOutput("================\n");
         printed  = ecmdWriteDataFormatted(l_memDataRead,"mem", l_realAddress.getDoubleWord(0));
         ecmdOutput(printed.c_str());
         // ECC varies depending on the size of data, so display it at the end
         ecmdOutput("Memory ECC\n");
         ecmdOutput("==========\n");
         sprintf(buf,"%-32s\n",l_ecc);
         ecmdOutput(buf);
       }

      validPosFound = true;//setting true as we did find a target to make sensor call
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("cipgetmemprocvar - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }
  return rc;
}

uint32_t cipPutMemProcVarUser(int argc, char * argv[])
{
  uint32_t rc = ECMD_SUCCESS;
  char buf[2000];
  uint32_t coeRc = ECMD_SUCCESS;
  std::string printed;
  std::string temp;
  ecmdChipTarget target;                        ///< Current target being operated on
  ecmdChipTarget cuTarget;                      ///< Current target being operated on for the chipUnit
  ecmdLooperData looperdata;
  bool validPosFound = false;                   ///< Did the looper find anything?
  uint32_t l_addressSize;
  bool addrTypeSet= false;

  // Declare/setup additional parameters.
  ecmdDataBuffer l_bufTags, l_Data;
  ecmdDataBuffer l_address_96(96);
  ecmdDataBuffer l_realAddress;
  uint32_t       l_bytes = 8;
  cipXlateVariables l_xlate;


  // set defaults to xlate structure
  l_xlate.tagsActive = true;
  l_xlate.mode32bit = false;
  l_xlate.writeECC = false;
  l_xlate.manualXlateFlag = false;
  l_xlate.addrType = CIP_MAINSTORE_REAL_ADDR;
  l_xlate.partitionId = 0;

  // current option is to override the partition Id, if the user does not provide any
  // we set the partition id to 0
  char* partionId= ecmdParseOptionWithArgs(&argc, &argv, "-partitionId");
  if (partionId != NULL ){
      if (!ecmdIsAllDecimal(partionId)) {
           ecmdOutputError("cipputmemprocvar- Non-Decimal characters detected in partition field\n");
           return ECMD_INVALID_ARGS;
      }else{
           l_xlate.partitionId = (uint32_t)atoi(partionId);
      }
  }

  // make sure we clear all the DB's
  l_bufTags.clear();
  l_Data.clear();
  l_realAddress.clear();

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
    ecmdOutputError("cipputmemprocvar - Too few arguments specified; you need Address type and address.\n");
    ecmdOutputError("cipputmemprocvar - Type 'cipputmemprocvar -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  
  // now check to see what is the type of address we are looking at
  if( argc >=1){
      temp = argv[0];
      transform(temp.begin(), temp.end(), temp.begin(), (int(*)(int)) tolower);
          if(temp == "real"){
            l_xlate.addrType = CIP_MAINSTORE_REAL_ADDR;
            addrTypeSet=true;
          } else if (temp == "effective") {
            l_xlate.addrType = CIP_MAINSTORE_EFFECTIVE_ADDR;
            addrTypeSet=true;
          } else if (temp == "virtual"){
            l_xlate.addrType = CIP_MAINSTORE_VIRTUAL_ADDR;
            addrTypeSet=true;
          } else {
              ecmdOutputError("cipputmemprocvar- Address type needs to be one of the address types(real/effective/virtual).\n");
              return ECMD_INVALID_ARGS;
         }
  }

  //check for address if the type is set
  if(addrTypeSet){
     // check if the address supplied is valid hex
      if( argc >=2 ){
          if (!ecmdIsAllHex(argv[1])) {
            ecmdOutputError("cipputmemprocvar- Non-hex characters detected in address field\n");
            return ECMD_INVALID_ARGS;
          }
          //  we are restricting address to 96 bits
          if ( strlen(argv[1]) > 24 ) // strlen does NOT count NULL terminator
          {
             ecmdOutputError("cipputmemprocvar- Address field is too large (>24 chars). It is restricted to 96 bits (12bytes)\n");
             return ECMD_INVALID_ARGS;
          }
          l_address_96.flushTo0();
          l_addressSize = strlen(argv[1]) * 4; //total bits, 4=nibble=1 character
          rc = l_address_96.insertFromHexRight(argv[1], (l_address_96.getBitLength() - l_addressSize),l_addressSize );
      } else {
          ecmdOutputError("cipputmemprocvar- Adress needs to be provided to get mem\n");
          return ECMD_INVALID_ARGS;
      }
  }

  // now look for # of bytes, we need to make sure that this is at word boundary and multiple of 8 bytes
  if( argc >=3){
          // if we come here then validate the # of bytes
          uint32_t noBytes = (uint32_t)atoi(argv[2]);
          if(noBytes % 8) 
          {
            ecmdOutputError("cipputmemprocvar- No of bytes must be at word boundary and multiples of 8 bytes\n");
            return ECMD_INVALID_ARGS;
          }
          // now set the bytes
          l_bytes = noBytes;
  }

  // we will need to get the data from user, make sure all the data is in hex format
  if( argc >=4 ){
          if (!ecmdIsAllHex(argv[3])) {
            ecmdOutputError("cipputmemprocvar- Non-hex characters detected in data field\n");
            return ECMD_INVALID_ARGS;
          }
          //  we are restricting address to 96 bits
          if ( strlen(argv[3]) > (l_bytes * 2) ) // strlen does NOT count NULL terminator
          {
             sprintf(buf,"cipputmemprocvar- Data Overflow, Size required - %d bytes\n",l_bytes);
             ecmdOutputError(buf);
             return ECMD_INVALID_ARGS;
          }else if ( strlen(argv[3]) <  (l_bytes * 2) )
          {
             sprintf(buf,"cipputmemprocvar- Data underflow, Size required - %d bytes\n",l_bytes);
             ecmdOutputError(buf);
             return ECMD_INVALID_ARGS;
          } else {
          uint32_t l_DataSize = strlen(argv[3]) * 4; //total bits
          // set the size dor databuffer
          l_Data.setBitLength(l_DataSize);
          rc = l_Data.insertFromHexRight(argv[3], 0,l_DataSize );
          }
  }

  // find chipUnit type
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  std::string  processingUnitName;

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  //< So we can determine processing unit name
  if ( ecmdLooperNext(target, looperdata) ) {
    rc = ecmdGetProcessingUnit(target,processingUnitName);
    if (rc) return rc;
  }

  if (processingUnitName.empty()) {
    ecmdOutputError("cipputmemprocvar- unable to find chipunit type for this command\n");
    return ECMD_INVALID_ARGS;
  }

  // set up target at chipUnit level
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitType = processingUnitName;
  target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
  // setting the thread state to wildcard to accept any thread value passed by user
  target.threadState = ECMD_TARGET_FIELD_WILDCARD;

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/
  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  // I dont think we need a looper here, but does not harm.
  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

       rc = cipPutMemProcVariousAddrType(target,           // input
                                        l_address_96,         // input
                                        l_bytes,           // input
                                        l_xlate,           // input
                                        l_Data,            // input
                                        l_bufTags,         // in/output (tags)
                                        l_realAddress);    // output
       if (rc != ECMD_SUCCESS)
       {
          // Failed to put mem now set message and bail out
          printed="\n";printed+=ecmdWriteTarget(target); printed+="\n"; ecmdOutput(printed.c_str());
          sprintf(buf, "cipputmemprocvar - Failed to write address 0x%s, rc = 0x%x\n", l_address_96.genHexLeftStr().c_str(), rc);
          coeRc=rc;
          ecmdOutputError(buf);
       }else{
         //the max bit length is 80 bits for the VA, so I will copy the 96 bits to 80bits and display
         ecmdDataBuffer ll_address_80(80);
         l_address_96.extractPreserve(ll_address_80,16,80);
         if (rc != ECMD_SUCCESS)
         {
            // Failed to read mem now set message and bail out
            sprintf(buf, "cipputmemprocvar - Failed extractPreserve\n");
            coeRc=rc;
            ecmdOutputError(buf);
            // we dont want to continue if we have an ecmdDB issue
            break;
         }
         printed=ecmdWriteTarget(target); printed+="\n"; ecmdOutput(printed.c_str());
         sprintf(buf, "cipputmemprocvar - Writting address 0x%s Success\n",ll_address_80.genHexLeftStr().c_str());
         ecmdOutput(buf);
       }

      validPosFound = true;//setting true as we did find a target to make sensor call
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("cipgetmemprocvar - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }
  return rc;
}
#endif
