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
#include <stdio.h>
#include <fstream>

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
  std::string newFilename;              ///< filename with target postfix incase of multi positions
  ecmdChipTarget target1;               ///< Current target operating on-for second looper
  ecmdLooperData looperdata1;           ///< looper to do the real work
  char targetStr[50];                   ///< target postfix for the filename incase of multi positions
  int targetCount=0;                    ///< counts the number of targets user specified
  
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
  if (argc < 5) {  
    ecmdOutputError("getvpdkeyword - Too few arguments specified; you need at least a chip, vpdtype, recordname, keyword and numbytes.\n");
    ecmdOutputError("getvpdkeyword - Type 'getvpdkeyword -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if (argc > 5) {
    ecmdOutputError("getvpdkeyword - Too many arguments specified; you only need chip, vpdtype, recordname, keyword and numbytes.\n");
    ecmdOutputError("getvpdkeyword - Type 'getvpdkeyword -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  target1 = target; //Created for the second looper needed for -fb case with multiple positions

  char *vpdType = argv[1];
  if (!strcasecmp(vpdType, "MOD") && !strcasecmp(vpdType, "FRU")) {
    ecmdOutputError("getvpdkeyword - You have to specify either mod or fru for the vpd type\n");
    return ECMD_INVALID_ARGS;
  }

  char *recordName = argv[2];
  char *keyWord = argv[3];
  
  if (!ecmdIsAllDecimal(argv[4])) {
    ecmdOutputError("getvpdkeyword - Non-decimal numbers detected in numBytes field\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint32_t numBytes = (uint32_t)atoi(argv[4]);
  
  //Run the loop to Check the number of targets
  if (filename != NULL) {
    rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
    if (rc) return rc;
 
    while ( ecmdConfigLooperNext(target, looperdata) ) {
      targetCount++;
    }
  }
  
  //Looper to do the actual work
  rc = ecmdConfigLooperInit(target1, ECMD_SELECTED_TARGETS_LOOP, looperdata1);
  if (rc) return rc;
              
  while ( ecmdConfigLooperNext(target1, looperdata1) ) {

    if (!strcasecmp(vpdType, "MOD")) {
      rc = getModuleVpdKeyword(target1, recordName, keyWord, numBytes, data);
    } else {
      rc = getFruVpdKeyword(target1, recordName, keyWord, numBytes, data);
    }
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "getvpdkeyword - Error occurred performing ";
        printed += (!strcasecmp(vpdType, "MOD") ? "getModuleVpdKeyword" : "getFruVpdKeyword");
        printed += " on " + ecmdWriteTarget(target1) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }

    printed = ecmdWriteTarget(target1) + "\n";
    if (filename != NULL) {
      if (targetCount > 1) {
        sprintf(targetStr, "k%dn%ds%dp%d", target1.cage, target1.node, target1.slot, target1.pos); 
        newFilename = (std::string)filename+"."+(std::string)targetStr;
      }
      else { newFilename = (std::string)filename; }
      
      rc = data.writeFile(newFilename.c_str(), ECMD_SAVE_FORMAT_BINARY_DATA);
     
      if (rc) {
       printed += "getvpdkeyword - Problems occurred writing data into file " + newFilename + "\n";
       ecmdOutputError(printed.c_str()); 
       return rc;
      }
      ecmdOutput( printed.c_str() );
      
      ops.close();
    } 
    else {
      std::string dataStr = ecmdWriteDataFormatted(data, outputformat);
      printed += dataStr;
      ecmdOutput( printed.c_str() );
    } 
    
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
  if ((argc < 5) && (filename == NULL)) {  
    ecmdOutputError("putvpdkeyword - Too few arguments specified; you need at least a chip, vpdtype, recordname, keyword and data.\n");
    ecmdOutputError("putvpdkeyword - Type 'putvpdkeyword -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if ((argc < 4) && (filename != NULL)) {
    ecmdOutputError("putvpdkeyword - Too few arguments specified; you need at least a chip, vpdtype, recordname, keyword and input file.\n");
    ecmdOutputError("putvpdkeyword - Type 'putvpdkeyword -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if ((argc > 4) && (filename != NULL)) {
    ecmdOutputError("putvpdkeyword - Too many arguments specified; you need a chip, vpdtype, recordname, keyword and input file.\n");
    ecmdOutputError("putvpdkeyword - Type 'putvpdkeyword -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if ((argc > 5) && (filename == NULL)) {
    ecmdOutputError("putvpdkeyword - Too many arguments specified; you need a chip, vpdtype, recordname, keyword and data.\n");
    ecmdOutputError("putvpdkeyword - Type 'putvpdkeyword -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  char *vpdType = argv[1];
  if (!strcasecmp(vpdType, "MOD") && !strcasecmp(vpdType, "FRU")) {
    ecmdOutputError("putvpdkeyword - You have to specify either mod or fru for the vpd type\n");
    return ECMD_INVALID_ARGS;
  }

  char *recordName = argv[2];
  char *keyWord = argv[3];
  
  if (filename != NULL) {
    rc = data.readFile(filename, ECMD_SAVE_FORMAT_BINARY_DATA);
    if (rc) {
      printed = "putvpdkeyword - Problems occurred in reading data from file " + (std::string)filename + "\n";
      ecmdOutputError(printed.c_str()); 
      return rc;
    }
  } else  {
    //container to store data
    rc = ecmdReadDataFormatted(data, argv[4], inputformat);
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

    if (!strcasecmp(vpdType, "MOD")) {
      rc = putModuleVpdKeyword(target, recordName, keyWord, data);
    } else {
      rc = putFruVpdKeyword(target, recordName, keyWord, data);
    }
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "putvpdkeyword - Error occurred performing putModuleVpdKeyword ";
        printed += (!strcasecmp(vpdType, "MOD") ? "putModuleVpdKeyword" : "putFruVpdKeyword");
        printed += " on " + ecmdWriteTarget(target) + "\n";
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
  if ((argc < 2) && (filename == NULL)) {  
    ecmdOutputError("putvpdimage - Too few arguments specified; you need to at least specify the chip, vpdtype and data to write.\n");
    ecmdOutputError("putvpdimage - Type 'putvpdimage -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if ((argc > 1) && (filename != NULL)) {
    ecmdOutputError("putvpdimage - Too many arguments specified; you only need to specify chip, vpdtype and data OR input file.\n");
    ecmdOutputError("putvpdimage - Type 'putvpdimage -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if ((argc > 2) && (filename == NULL)) {
    ecmdOutputError("putvpdimage - Too many arguments specified; you only need to specify chip, vpdtype and data OR input file.\n");
    ecmdOutputError("putvpdimage - Type 'putvpdimage -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } 

  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  char *vpdType = argv[1];
  if (!strcasecmp(vpdType, "MOD") && !strcasecmp(vpdType, "FRU")) {
    ecmdOutputError("putvpdimage - You have to specify either mod or fru for the vpd type\n");
    return ECMD_INVALID_ARGS;
  }

  if (filename != NULL) {
    rc = data.readFile(filename, ECMD_SAVE_FORMAT_BINARY_DATA);
    if (rc) {
      printed = "putvpdimage - Problems occurred in reading data from file " + (std::string)filename + "\n";
      ecmdOutputError(printed.c_str()); 
      return rc;
    }
  } else  {
    //container to store data
    rc = ecmdReadDataFormatted(data, argv[1], inputformat);
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

    if (!strcasecmp(vpdType, "MOD")) {
      rc = putModuleVpdImage(target, data);
    } else {
      rc = putFruVpdImage(target, data);
    }
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "putvpdimage - Error occurred performing putModuleVpdImage ";
        printed += (!strcasecmp(vpdType, "MOD") ? "putModuleVpdImage" : "putFruVpdImage");
        printed += " on " + ecmdWriteTarget(target) + "\n";
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
  std::string newFilename;              ///< filename with target postfix incase of multi positions
  ecmdChipTarget target1;               ///< Current target operating on-for second looper
  ecmdLooperData looperdata1;           ///< looper to do the real work
  char targetStr[50];                   ///< target postfix for the filename incase of multi positions
  int targetCount=0;                    ///< counts the number of targets user specified
  
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
  if (argc < 3) {  
    ecmdOutputError("getvpdimage - Too few arguments specified; you need at least the chip, vpdtype and numbytes.\n");
    ecmdOutputError("getvpdimage - Type 'getvpdimage -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if (argc > 3) {
    ecmdOutputError("getvpdimage - Too many arguments specified; you only need chip, vpdtype and numbytes.\n");
    ecmdOutputError("getvpdimage - Type 'getvpdimage -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  target1 = target; //Created for the second looper needed for -fb case with multiple positions

  char *vpdType = argv[1];
  if (!strcasecmp(vpdType, "MOD") && !strcasecmp(vpdType, "FRU")) {
    ecmdOutputError("getvpdimage - You have to specify either mod or fru for the vpd type\n");
    return ECMD_INVALID_ARGS;
  }

  if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("getvpdimage - Non-decimal numbers detected in numBytes field\n");
    return ECMD_INVALID_ARGS;
  }
  
  uint32_t numBytes = (uint32_t)atoi(argv[2]);
  
  //Run the loop to Check the number of targets
  if (filename != NULL) {
    rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
    if (rc) return rc;
 
    while ( ecmdConfigLooperNext(target, looperdata) ) {
      targetCount++;
    }
  }
  
  //Looper to do the actual work
  rc = ecmdConfigLooperInit(target1, ECMD_SELECTED_TARGETS_LOOP, looperdata1);
  if (rc) return rc;
  
  while ( ecmdConfigLooperNext(target1, looperdata1) ) {

    if (!strcasecmp(vpdType, "MOD")) {
      rc = getModuleVpdImage(target1, numBytes, data);
    } else {
      rc = getFruVpdImage(target1, numBytes, data);
    }
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "getvpdimage - Error occurred performing getModuleVpdImage on ";
        printed += (!strcasecmp(vpdType, "MOD") ? "getModuleVpdImage" : "getFruVpdImage");
        printed += " on " + ecmdWriteTarget(target1) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }


    printed = ecmdWriteTarget(target1) + "\n";
    if (filename != NULL) {
      
      if (targetCount > 1) {
        sprintf(targetStr, "k%dn%ds%dp%d", target1.cage, target1.node, target1.slot, target1.pos); 
        newFilename = (std::string)filename+"."+(std::string)targetStr;
      }
      else { newFilename = (std::string)filename; }
      
      rc = data.writeFile(newFilename.c_str(), ECMD_SAVE_FORMAT_BINARY_DATA);
     
      if (rc) {
       printed += "getvpdimage - Problems occurred writing data into file " + newFilename + "\n";
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
  
  if (!validPosFound) {
    ecmdOutputError("getvpdimage - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }
  return rc;

}
