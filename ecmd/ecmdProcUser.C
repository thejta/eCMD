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
#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <ecmdSharedUtils.H>


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

  ecmdChipTarget target;        ///< Current target
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  std::list<ecmdNameEntry> entries;    ///< List of spr's to fetch, to use getSprMultiple
  std::list<ecmdNameEntry> entries_copy;    ///< List of spr's to fetch, to use getSprMultiple
  ecmdNameEntry  entry;         ///< Spr entry to fetch
  ecmdLooperData looperdata;            ///< Store internal Looper data
  int idx;

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
    ecmdOutputError("getspr - Type 'getspr -h' for usage.");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = "pu";
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;


  /* Walk through the arguments and create our list of sprs */
  for (idx = 0; idx < argc; idx ++) {
    entry.name = argv[idx];
    entries.push_back(entry);
  }

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while ( ecmdConfigLooperNext(target, looperdata) ) {

  

    /* Restore to our initial list */
    entries_copy = entries;


    /* Actually go fetch the data */
    rc = getSprMultiple(target, entries);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
      printed = "getspr - Error occured performing getSprMultiple on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    else {
      validPosFound = true;     
    }

    printed = ecmdWriteTarget(target) + "\n";
    ecmdOutput( printed.c_str() );
    for (std::list<ecmdNameEntry>::iterator entit = entries.begin(); entit != entries.end(); entit ++) {

      printed = entit->name + "\t";

      printed += ecmdWriteDataFormatted(entit->buffer, format);

      ecmdOutput( printed.c_str() );
    }

    /* Now that we are done, clear the list for the next iteration - fixes BZ#49 */
    entries.clear();

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

  ecmdChipTarget target;        ///< Current target
  std::string inputformat = "x";                ///< Default input format
  std::string dataModifier = "insert";          ///< Default data Modifier (And/Or/insert)
  ecmdDataBuffer buffer;        ///< Buffer to store data to write with
  ecmdDataBuffer sprBuffer;     ///< Buffer to store data from the spr
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string sprName;          ///< Name of spr to write 
  int startBit = -1;                            ///< Startbit to insert data
  int numBits = 0;                              ///< Number of bits to insert data
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
    ecmdOutputError("putspr - Too few arguments specified; you need at least an sprName  and some data.\n");
    ecmdOutputError("putspr - Type 'putspr -h' for usage.");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = "pu";
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  sprName = argv[0];

  if (argc == 4) {
    if (!ecmdIsAllDecimal(argv[1])) {
      ecmdOutputError("putspr - Non-decimal numbers detected in startbit field\n");
      return ECMD_INVALID_ARGS;
    }
    startBit = atoi(argv[1]);

    if (!ecmdIsAllDecimal(argv[2])) {
      ecmdOutputError("putspr - Non-decimal numbers detected in numbits field\n");
      return ECMD_INVALID_ARGS;
    }
    numBits = atoi(argv[2]);
    
    
    dataPtr = argv[3];

  } else if (argc == 2) {

    dataPtr = argv[1];

  } else {
    ecmdOutputError("putspr - Too many arguments specified; you probably added an option that wasn't recognized.\n");
    ecmdOutputError("putspr - Type 'putspr -h' for usage.\n");
    return ECMD_INVALID_ARGS;
    
  }


  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while ( ecmdConfigLooperNext(target, looperdata) ) {


    rc = getSpr(target, sprName.c_str(), sprBuffer);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "putspr - Error occured performing getspr on ";
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
      if (startBit == -1 ) {
        startBit = 0;
        numBits = sprBuffer.getBitLength();
      }

      rc = ecmdReadDataFormatted(buffer, dataPtr, inputformat, numBits);
      if (rc) {
        ecmdOutputError("putspr - Problems occurred parsing input data, must be an invalid format\n");
        return rc;
      }

      dataPtr = NULL;
    }

    rc = ecmdApplyDataModifier(sprBuffer, buffer,  startBit, dataModifier);
    if (rc) return rc;


    rc = putSpr(target, sprName.c_str(), buffer);

    if (rc) {
        printed = "putspr - Error occured performing putspr on ";
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
    ecmdOutputError("putspr - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}


