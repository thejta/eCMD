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

// Module Description **************************************************
//
// Description: CIP Extension Processor functions
//
// End Module Description **********************************************

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

uint32_t cipInstructUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  ecmdLooperData looperdata;            ///< Store internal Looper data
  bool executeAll = false;      ///< Run start/stop on all procs
  int  steps = 1;               ///< Number of steps to run

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
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


    /* Loop through the steps so we step all procs in somewhat sync */
    for (int step = 0; step < steps; step ++) {

      //Setup the target that will be used to query the system config 
      target.chipType = ECMD_CHIPT_PROCESSOR;
      target.chipTypeState = ECMD_TARGET_FIELD_VALID;
      target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = target.threadState = ECMD_TARGET_FIELD_WILDCARD;


      rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
      if (rc) return rc;

      if (!strcasecmp(argv[0],"start")) {
        ecmdOutput("Starting processor instructions ...\n");
      } else if (!strcasecmp(argv[0], "sreset")) {
        ecmdOutput("Starting processor instructions via S-Reset ...\n");
      } else if (!strcasecmp(argv[0], "stop")) {
        ecmdOutput("Stopping processor instructions ...\n");
      } else if (!strcasecmp(argv[0], "step")) {
        ecmdOutput("Stepping processor instructions ...\n");
      } else {
        ecmdOutputError("cipinstruct - Invalid instruct mode, must be start|stop|step \n");
        return ECMD_INVALID_ARGS;
      }


      while ( ecmdConfigLooperNext(target, looperdata) ) {


        if (!strcasecmp(argv[0],"start")) {
          rc = cipStartInstructions(target);
        } else if (!strcasecmp(argv[0], "sreset")) {
          rc = cipStartInstructionsSreset(target);
        } else if (!strcasecmp(argv[0], "stop")) {
          rc = cipStopInstructions(target);
        } else if (!strcasecmp(argv[0], "step")) {
          rc = cipStepInstructions(target, 1);
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
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = target.threadState = ECMD_TARGET_FIELD_WILDCARD;
  
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
        
  while ( ecmdConfigLooperNext(target, looperdata) ) {


    if (!strcasecmp(argv[0],"set")) {
      rc = cipSetBreakpoint(target, address, type);
    } else if (!strcasecmp(argv[0], "clear")) {
      rc = cipClearBreakpoint(target,address, type );
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

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while ( ecmdConfigLooperNext(target, looperdata) ) {

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


  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while ( ecmdConfigLooperNext(target, looperdata) ) {


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



// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
