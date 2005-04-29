// Copyright ***********************************************************
//                                                                      
// File ecmdVpdUser.C                                  
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
#define ecmdVpdUser_C
#include <stdio.h>
#include <fstream>

#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <ecmdInterpreter.H>
#include <ecmdSharedUtils.H>

#undef ecmdVpdUser_C
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

uint32_t ecmdGetVpdKeywordUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;
  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string outputformat = "xl";      ///< Output Format to display
  ecmdChipTarget target;                ///< Current target operating on
  ecmdDataBuffer data;                  ///< Data from the module vpd record
  bool validPosFound = false;           ///< Did the looper find anything to execute on
  bool outputformatflag = false;
  std::ofstream ops;                    ///< Output stream for writing vpd data into
  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  
  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr != NULL) {
    outputformat = formatPtr;
    outputformatflag = true;
  }
  
  /* Get the filename if -fb is specified */
  char * filename = ecmdParseOptionWithArgs(&argc, &argv, "-fb");
  std::string printed;

  if((filename != NULL) && (outputformatflag) ) {
    printed = "getvpdkeyword - Options '-fb' cannot be specified with '-o' option.\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
  } 
  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 3) {  
    ecmdOutputError("getvpdkeyword - Too few arguments specified; you need at least a recordname, keyword and numbytes.\n");
    ecmdOutputError("getvpdkeyword - Type 'getvpdkeyword -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if (argc > 3) {
    ecmdOutputError("getvpdkeyword - Too many arguments specified; you only need recordname, keyword and numbytes.\n");
    ecmdOutputError("getvpdkeyword - Type 'getvpdkeyword -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_QUERY_WILDCARD;
  target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  char *recordName = argv[0];
  char *keyWord = argv[1];
  
  if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("getvpdkeyword - Non-decimal numbers detected in numBytes field\n");
    return ECMD_INVALID_ARGS;
  }
  
  int numBytes = atoi(argv[2]);
  
  if (filename != NULL) {
   ops.open(filename);
   if (ops.fail()) {
     printed = "getvpdkeyword - Unable to open file " + (std::string)filename + " for write\n";
     ecmdOutputError(printed.c_str());
     return ECMD_UNKNOWN_FILE;
   }
  }
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  
  while ( ecmdConfigLooperNext(target, looperdata) ) {

    rc = getModuleVpdKeyword(target, recordName, keyWord, numBytes, data);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "getvpdkeyword - Error occurred performing getModuleVpdKeyword on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }


    printed = ecmdWriteTarget(target) + "\n";
    if (filename != NULL) {
      rc = data.writeFileStream(ops);
     
      if (rc) {
       printed += "getvpdkeyword - Problems occurred writing data into file " + (std::string)filename + "\n";
       ecmdOutputError(printed.c_str()); 
       return rc;
      }
      ecmdOutput( printed.c_str() );
    } 
    else {
      std::string dataStr = ecmdWriteDataFormatted(data, outputformat);
      printed += dataStr;
      ecmdOutput( printed.c_str() );
    } 
    

  }
  if (filename != NULL) {
    ops.close();
  }
  if (!validPosFound) {
    ecmdOutputError("getvpdkeyword - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }
  return rc;

}

uint32_t ecmdPutVpdKeywordUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;
  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string inputformat = "xl";       ///< format of input data
  ecmdChipTarget target;                ///< Current target operating on
  ecmdDataBuffer data;                  ///< buffer for the Data to write into the module vpd keyword
  bool validPosFound = false;           ///< Did the looper find anything to execute on
  bool inputformatflag = false;
  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  
  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    inputformat = formatPtr;
    inputformatflag = true;
  }
  
  /* Get the filename if -fb is specified */
  char * filename = ecmdParseOptionWithArgs(&argc, &argv, "-fb");
  std::string printed;

  if((filename != NULL) && (inputformatflag) ) {
    printed = "putvpdkeyword - Options '-fb' cannot be specified with '-i' option.\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
  } 
  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if ((argc < 3) && (filename == NULL)) {  
    ecmdOutputError("putvpdkeyword - Too few arguments specified; you need at least a recordname, keyword and data.\n");
    ecmdOutputError("putvpdkeyword - Type 'putvpdkeyword -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if ((argc < 2) && (filename != NULL)) {
    ecmdOutputError("putvpdkeyword - Too few arguments specified; you need at least a recordname, keyword and input file.\n");
    ecmdOutputError("putvpdkeyword - Type 'putvpdkeyword -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if ((argc > 2) && (filename != NULL)) {
    ecmdOutputError("putvpdkeyword - Too many arguments specified; you need a recordname, keyword and input file.\n");
    ecmdOutputError("putvpdkeyword - Type 'putvpdkeyword -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if ((argc > 3) && (filename == NULL)) {
    ecmdOutputError("putvpdkeyword - Too many arguments specified; you need a recordname, keyword and data.\n");
    ecmdOutputError("putvpdkeyword - Type 'putvpdkeyword -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_QUERY_WILDCARD;
  target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  char *recordName = argv[0];
  char *keyWord = argv[1];
  
  if (filename != NULL) {
    rc = data.readFile(filename, ECMD_SAVE_FORMAT_BINARY_DATA);
    if (rc) {
      printed = "putvpdkeyword - Problems occurred in reading data from file " + (std::string)filename + "\n";
      ecmdOutputError(printed.c_str()); 
      return rc;
    }
  } else  {
    //container to store data
    rc = ecmdReadDataFormatted(data, argv[2], inputformat);
    if (rc) {
      ecmdOutputError("putvpdkeyword - Problems occurred parsing input data, must be an invalid format\n");
      return rc;
    }
  }
  
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  
  while ( ecmdConfigLooperNext(target, looperdata) ) {

    rc = putModuleVpdKeyword(target, recordName, keyWord, data);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "putvpdkeyword - Error occurred performing putModuleVpdKeyword on ";
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
    ecmdOutputError("putvpdkeyword - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }
  return rc;

}

uint32_t ecmdPutVpdImageUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;
  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string inputformat = "xl";       ///< format of input data
  ecmdChipTarget target;                ///< Current target operating on
  ecmdDataBuffer data;                  ///< buffer for the Data to write into the module vpd keyword
  bool validPosFound = false;           ///< Did the looper find anything to execute on
  bool inputformatflag = false;
  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  
  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    inputformat = formatPtr;
    inputformatflag = true;
  }
  
  /* Get the filename if -fb is specified */
  char * filename = ecmdParseOptionWithArgs(&argc, &argv, "-fb");
  std::string printed;

  if((filename != NULL) && (inputformatflag) ) {
    printed = "putvpdimage - Options '-fb' cannot be specified with '-i' option.\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
  } 
  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if ((argc < 1) && (filename == NULL)) {  
    ecmdOutputError("putvpdimage - Too few arguments specified; you need to at least specify the data to write.\n");
    ecmdOutputError("putvpdimage - Type 'putvpdimage -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if ((argc > 0) && (filename != NULL)) {
    ecmdOutputError("putvpdimage - Too many arguments specified; you only need to specify data OR input file.\n");
    ecmdOutputError("putvpdimage - Type 'putvpdimage -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if ((argc > 1) && (filename == NULL)) {
    ecmdOutputError("putvpdimage - Too many arguments specified; you only need to specify data OR input file.\n");
    ecmdOutputError("putvpdimage - Type 'putvpdimage -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } 

  //Setup the target that will be used to query the system config 
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_QUERY_WILDCARD;
  target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  if (filename != NULL) {
    rc = data.readFile(filename, ECMD_SAVE_FORMAT_BINARY_DATA);
    if (rc) {
      printed = "putvpdimage - Problems occurred in reading data from file " + (std::string)filename + "\n";
      ecmdOutputError(printed.c_str()); 
      return rc;
    }
  } else  {
    //container to store data
    rc = ecmdReadDataFormatted(data, argv[0], inputformat);
    if (rc) {
      ecmdOutputError("putvpdimage - Problems occurred parsing input data, must be an invalid format\n");
      return rc;
    }
  }
  
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  
  while ( ecmdConfigLooperNext(target, looperdata) ) {

    rc = putModuleVpdImage(target, data);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "putvpdimage - Error occurred performing putModuleVpdImage on ";
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
    ecmdOutputError("putvpdimage - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }
  return rc;

}

uint32_t ecmdGetVpdImageUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;
  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string outputformat = "xl";      ///< Output Format to display
  ecmdChipTarget target;                ///< Current target operating on
  ecmdDataBuffer data;                  ///< Data from the module vpd record
  bool validPosFound = false;           ///< Did the looper find anything to execute on
  bool outputformatflag = false;
  std::ofstream ops;                    ///< Output stream for writing vpd data into
  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  
  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr != NULL) {
    outputformat = formatPtr;
    outputformatflag = true;
  }
  
  /* Get the filename if -fb is specified */
  char * filename = ecmdParseOptionWithArgs(&argc, &argv, "-fb");
  std::string printed;

  if((filename != NULL) && (outputformatflag) ) {
    printed = "getvpdimage - Options '-fb' cannot be specified with '-o' option.\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
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
    ecmdOutputError("getvpdimage - Too few arguments specified; you need at least the numbytes.\n");
    ecmdOutputError("getvpdimage - Type 'getvpdimage -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if (argc > 1) {
    ecmdOutputError("getvpdimage - Too many arguments specified; you only need numbytes.\n");
    ecmdOutputError("getvpdimage - Type 'getvpdimage -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_QUERY_WILDCARD;
  target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  if (!ecmdIsAllDecimal(argv[0])) {
    ecmdOutputError("getvpdimage - Non-decimal numbers detected in numBytes field\n");
    return ECMD_INVALID_ARGS;
  }
  
  int numBytes = atoi(argv[0]);
  
  if (filename != NULL) {
   ops.open(filename);
   if (ops.fail()) {
     printed = "getvpdimage - Unable to open file " + (std::string)filename + " for write\n";
     ecmdOutputError(printed.c_str());
     return ECMD_UNKNOWN_FILE;
   }
  }
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  
  while ( ecmdConfigLooperNext(target, looperdata) ) {

    rc = getModuleVpdImage(target, numBytes, data);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "getvpdimage - Error occurred performing getModuleVpdImage on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }


    printed = ecmdWriteTarget(target) + "\n";
    if (filename != NULL) {
      rc = data.writeFileStream(ops);
     
      if (rc) {
       printed += "getvpdimage - Problems occurred writing data into file " + (std::string)filename + "\n";
       ecmdOutputError(printed.c_str()); 
       return rc;
      }
      ecmdOutput( printed.c_str() );
    } 
    else {
      std::string dataStr = ecmdWriteDataFormatted(data, outputformat);
      printed += dataStr;
      ecmdOutput( printed.c_str() );
    } 
  }
  if (filename != NULL) {
    ops.close();
  }
  if (!validPosFound) {
    ecmdOutputError("getvpdimage - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }
  return rc;

}
