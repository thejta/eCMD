/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File ecmdSimUser.C                                  
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

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>

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
#ifndef REMOVE_SIM

uint32_t ecmdSimaetUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc < 1) {
    ecmdOutputError("simaet - At least one argument ('on', 'off', or 'flush') is required for simaet.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 1) {
    ecmdOutputError("simaet - Too many arguments to simaet, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simaet(argv[0]);

  return rc;

}


uint32_t ecmdSimcheckpointUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc < 1) {
    ecmdOutputError("simcheckpoint - At least one argument (a checkpoint filename) is required for simcheckpoint.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 1) {
    ecmdOutputError("simcheckpoint - Too many arguments to simcheckpoint, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simcheckpoint(argv[0]);

  return rc;

}


uint32_t ecmdSimclockUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc < 1) {
    ecmdOutputError("simclock - At least one argument (a number of clock cycles) is required for simclock.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 1) {
    ecmdOutputError("simclock - Too many arguments to simclock, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  if (!ecmdIsAllDecimal(argv[0])) {
    ecmdOutputError("simclock - Non-decimal numbers detected in cycles field\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t numcycles = (uint32_t)atoi(argv[0]);
  rc = simclock(numcycles);

  return rc;

}


uint32_t ecmdSimechoUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;
  std::string message;

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc < 1) {
    ecmdOutputError("simecho - At least one argument (a message to print) is required for simecho.\n");
    return ECMD_INVALID_ARGS;
  }
  for (int idx = 0; idx < argc; idx ++) {
    message += argv[idx];
    message += " ";
  }

  rc = simecho(message.c_str());

  return rc;

}


uint32_t ecmdSimexitUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc == 2) {
    if (!ecmdIsAllDecimal(argv[0])) {
      ecmdOutputError("simexit - Non-decimal numbers detected in return code field\n");
      return ECMD_INVALID_ARGS;
    }
    uint32_t prc = (uint32_t)atoi(argv[0]);
    rc = simexit(prc, argv[1]);
    
  } else if (argc > 0) {
    ecmdOutputError("simexit - Too many arguments to simexit, none are required.\n");
    return ECMD_INVALID_ARGS;

  } else {

    rc = simexit();
  }

  return rc;

}

uint32_t ecmdSimEXPECTFACUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;


  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr == NULL) {
    format = "xr";
  }
  else {
    format = formatPtr;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  if (argc < 3) {
    ecmdOutputError("simEXPECTFAC - Too few arguments to simEXPECTFACS, you need at least a symbol , data, and a length.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];

  if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("simEXPECTFAC - Non-decimal numbers detected in bitLength field\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t bitLength = (uint32_t)atoi(argv[2]);

  ecmdDataBuffer buffer;
  ecmdDataBuffer expected;
  rc = ecmdReadDataFormatted(expected, argv[1], format, (int)bitLength);
  if (rc) {
    ecmdOutputError("simEXPECTFAC - Problems occurred parsing input data, must be an invalid format\n");
    return rc;
  }


  uint32_t row = 0, offset = 0;
  if (argc > 3) {
    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("simEXPECTFAC - Non-decimal numbers detected in row field\n");
      return ECMD_INVALID_ARGS;
    }
    row = (uint32_t)atoi(argv[3]);
  }
  if (argc > 4) {
    if (!ecmdIsAllDecimal(argv[4])) {
      ecmdOutputError("simEXPECTFAC - Non-decimal numbers detected in offset field\n");
      return ECMD_INVALID_ARGS;
    }
    offset = (uint32_t)atoi(argv[4]);
  }

  if (argc > 5) {
    ecmdOutputError("simEXPECTFAC - Too many arguments to simEXPECTFACS, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  /* Ok, let's call GETFAC and do the comparison */
  rc = simGETFAC(facname, bitLength, buffer, row, offset);
  if (rc) return rc;

  uint32_t mismatchedBit = ECMD_UNSET;
  if (!ecmdCheckExpected ( buffer, expected, mismatchedBit)) {
/*    char buf[200]; */
    std::string printed;
    ecmdOutputError("simEXPECTFAC - Expect failure\n");
    printed = "simEXPECTFAC - Actual   : " + ecmdWriteDataFormatted(buffer, format); 
    ecmdOutputError(printed.c_str());
    printed = "simEXPECTFAC - Expected : " + ecmdWriteDataFormatted(expected, format); 
    ecmdOutputError(printed.c_str());
    return ECMD_EXPECT_FAILURE;
  }

  return rc;

}

uint32_t ecmdSimexpecttcfacUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr == NULL) {
    format = "xr";
  }
  else {
    format = formatPtr;
  }

  /* They want a subset of bits, not the entire thing */
  bool useSubset = false;
  if (ecmdParseOption(&argc, &argv, "-subset"))
    useSubset = true;

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  if (argc < 2) {
    ecmdOutputError("simexpecttcfac - Too few arguments to simexpecttcfac, you need at least a symbol and data.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];

  uint32_t row = 0, startBit = 0, bitLength = 0;


  ecmdDataBuffer buffer;
  ecmdDataBuffer expected;

  if (!useSubset) {
    if (argc > 2) {
      if (!ecmdIsAllDecimal(argv[2])) {
        ecmdOutputError("simexpecttcfac - Non-decimal numbers detected in row field\n");
        return ECMD_INVALID_ARGS;
      }
      row = (uint32_t)atoi(argv[2]);
    }

    if (argc > 3) {
      ecmdOutputError("simexpecttcfac - Too many arguments to simexpecttcfac, you probably added a non-supported option.\n");
      return ECMD_INVALID_ARGS;
    }

  } else {
    if (argc < 4) {
      ecmdOutputError("simexpecttcfac - Too few arguments to simexpecttcfac, you need to specify startbits and numbits with -subset.\n");
      return ECMD_INVALID_ARGS;
    }
    if (!ecmdIsAllDecimal(argv[2])) {
      ecmdOutputError("simexpecttcfac - Non-decimal numbers detected in startBit field\n");
      return ECMD_INVALID_ARGS;
    }
    startBit = (uint32_t)atoi(argv[2]);
    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("simexpecttcfac - Non-decimal numbers detected in bitLength field\n");
      return ECMD_INVALID_ARGS;
    }
    bitLength = (uint32_t)atoi(argv[3]);

    if (argc > 4) {
      ecmdOutputError("simexpecttcfac - Too many arguments to simexpecttcfac, you probably added a non-supported option.\n");
      return ECMD_INVALID_ARGS;
    }
  }

  rc = ecmdReadDataFormatted(expected, argv[1], format, (int)bitLength);
  if (rc) {
    ecmdOutputError("simexpecttcfac - Problems occurred parsing input data, must be an invalid format\n");
    return rc;
  }


  /* Ok, let's call gettcfac and do the comparison */
  rc = simgettcfac(facname, buffer, row, startBit, bitLength);
  if (rc) return rc;

  uint32_t mismatchedBit = ECMD_UNSET;
  if (!ecmdCheckExpected ( buffer, expected, mismatchedBit)) {
/*    char buf[200]; */
    std::string printed;
    ecmdOutputError("simexpecttcfac - Expect failure\n");
    printed = "simexpecttcfac - Actual   : " + ecmdWriteDataFormatted(buffer, format); 
    ecmdOutputError(printed.c_str());
    printed = "simexpecttcfac - Expected : " + ecmdWriteDataFormatted(expected, format); 
    ecmdOutputError(printed.c_str());
    return ECMD_EXPECT_FAILURE;
  }


  return rc;

}


uint32_t ecmdSimGETFACUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr == NULL) {
    format = "xr";
  }
  else {
    format = formatPtr;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  if (argc < 2) {
    ecmdOutputError("simGETFAC - Too few arguments to simGETFACS, you need at least a facname and bit length.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];
  if (!ecmdIsAllDecimal(argv[1])) {
    ecmdOutputError("simGETFAC - Non-decimal numbers detected in bitLength field\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t bitLength = (uint32_t)atoi(argv[1]);

  uint32_t row = 0, offset = 0;
  if (argc > 2) {
    if (!ecmdIsAllDecimal(argv[2])) {
      ecmdOutputError("simGETFAC - Non-decimal numbers detected in row field\n");
      return ECMD_INVALID_ARGS;
    }
    row = (uint32_t)atoi(argv[2]);
  }
  if (argc > 3) {
    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("simGETFAC - Non-decimal numbers detected in offset field\n");
      return ECMD_INVALID_ARGS;
    }
    offset = (uint32_t)atoi(argv[3]);
  }

  if (argc > 4) {
    ecmdOutputError("simGETFAC - Too many arguments to simGETFACS, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  ecmdDataBuffer buffer;

  rc = simGETFAC(facname, bitLength, buffer, row, offset);

  if (!rc) {
    std::string printed = ecmdWriteDataFormatted(buffer, format);
    ecmdOutput(printed.c_str());
  }

  return rc;

}

uint32_t ecmdSimGETFACXUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr == NULL) {
    format = "bX";
  }
  else {
    format = formatPtr;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  if (argc < 2) {
    ecmdOutputError("simGETFACX - Too few arguments to simGETFACX, you need at least a facname and bit length.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];
  if (!ecmdIsAllDecimal(argv[1])) {
    ecmdOutputError("simGETFACX - Non-decimal numbers detected in bitLength field\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t bitLength = (uint32_t)atoi(argv[1]);

  uint32_t row = 0, offset = 0;
  if (argc > 2) {
    if (!ecmdIsAllDecimal(argv[2])) {
      ecmdOutputError("simGETFACX - Non-decimal numbers detected in row field\n");
      return ECMD_INVALID_ARGS;
    }
    row = (uint32_t)atoi(argv[2]);
  }
  if (argc > 3) {
    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("simGETFACX - Non-decimal numbers detected in offset field\n");
      return ECMD_INVALID_ARGS;
    }
    offset = (uint32_t)atoi(argv[3]);
  }

  if (argc > 4) {
    ecmdOutputError("simGETFACX - Too many arguments to simGETFACX, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  ecmdDataBuffer buffer;

  rc = simGETFACX(facname, bitLength, buffer, row, offset);

  if (!rc) {
    std::string printed = ecmdWriteDataFormatted(buffer, format);
    ecmdOutput(printed.c_str());
  }

  return rc;

}

uint32_t ecmdSimgettcfacUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr == NULL) {
    format = "xr";
  }
  else {
    format = formatPtr;
  }

  /* They want a subset of bits, not the entire thing */
  bool useSubset = false;
  if (ecmdParseOption(&argc, &argv, "-subset"))
    useSubset = true;

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  if (argc < 1) {
    ecmdOutputError("simgettcfac - Too few arguments to simgettcfac, you need at least a tcfacname.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];

  uint32_t row = 0, startBit = 0, bitLength = 0;

  if (!useSubset) {
    if (argc > 1) {
      if (!ecmdIsAllDecimal(argv[1])) {
        ecmdOutputError("simgettcfac - Non-decimal numbers detected in row field\n");
        return ECMD_INVALID_ARGS;
      }
      row = (uint32_t)atoi(argv[1]);
    }

    if (argc > 2) {
      ecmdOutputError("simgettcfac - Too many arguments to simgettcfac, you probably added a non-supported option.\n");
      return ECMD_INVALID_ARGS;
    }

  } else {
    if (argc < 3) {
      ecmdOutputError("simgettcfac - Too few arguments to simgettcfac, you need to specify startbits and numbits with -subset.\n");
      return ECMD_INVALID_ARGS;
    }
    /* Originally this code assumed the row would always be 0, we need to allow the user to pass in a row */
    /* If argc == 4, then a row number was given.  Use that.  Otherwise, default to 0 - JTA 04/23/06 */
    int argOffset = 0;
    if (argc == 4) {
      if (!ecmdIsAllDecimal(argv[1])) {
        ecmdOutputError("simgettcfac - Non-decimal numbers detected in row field\n");
        return ECMD_INVALID_ARGS;
      }
      row = (uint32_t)atoi(argv[1]);
      argOffset = 1;
    }

    if (!ecmdIsAllDecimal(argv[(1 + argOffset)])) {
      ecmdOutputError("simgettcfac - Non-decimal numbers detected in startBit field\n");
      return ECMD_INVALID_ARGS;
    }
    startBit = (uint32_t)atoi(argv[(1 + argOffset)]);

    if (!ecmdIsAllDecimal(argv[(2 + argOffset)])) {
      ecmdOutputError("simgettcfac - Non-decimal numbers detected in bitLength field\n");
      return ECMD_INVALID_ARGS;
    }
    bitLength = (uint32_t)atoi(argv[(2 + argOffset)]);

    if ((argOffset && argc > 4) || (!argOffset && argc > 3)) {
      ecmdOutputError("simgettcfac - Too many arguments to simgettcfac, you probably added a non-supported option.\n");
      return ECMD_INVALID_ARGS;
    }
  }


  ecmdDataBuffer buffer;

  rc = simgettcfac(facname, buffer, row, startBit, bitLength);

  if (!rc) {
    std::string printed = ecmdWriteDataFormatted(buffer, format);
    ecmdOutput(printed.c_str());
  }

  return rc;

}

uint32_t ecmdSimgetcurrentcycleUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  if (argc > 0) {
    ecmdOutputError("simgetcurrentcycle - Too many arguments to simgetcurrentcycle, none are required.\n");
    return ECMD_INVALID_ARGS;
  }

  uint64_t cycle_count = 0x0;
  rc = simgetcurrentcycle(cycle_count);
  if (rc) return rc;

  char outstr[40];
  sprintf(outstr, "Current cycle count is %llu\n", (unsigned long long)cycle_count);
  ecmdOutput(outstr);

  return rc;

}


uint32_t ecmdSiminitUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc < 1) {
    ecmdOutputError("siminit - At least one argument (a checkpoint filename or 'NONE') is required for siminit.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 1) {
    ecmdOutputError("siminit - Too many arguments to siminit, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = siminit(argv[0]);

  return rc;

}

uint32_t ecmdSimPUTFACUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr == NULL) {
    format = "xr";
  }
  else {
    format = formatPtr;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  if (argc < 3) {
    ecmdOutputError("simPUTFAC - Too few arguments to simPUTFACS, you need at least a symbol , data, and a length.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];

  if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("simPUTFAC - Non-decimal numbers detected in bitLength field\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t bitLength = (uint32_t)atoi(argv[2]);

  ecmdDataBuffer buffer;
  rc = ecmdReadDataFormatted(buffer, argv[1], format, (int)bitLength);
  if (rc) {
    ecmdOutputError("simPUTFAC - Problems occurred parsing input data, must be an invalid format\n");
    return rc;
  }


  uint32_t row = 0, offset = 0;
  if (argc > 3) {
    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("simPUTFAC - Non-decimal numbers detected in row field\n");
      return ECMD_INVALID_ARGS;
    }
    row = (uint32_t)atoi(argv[3]);
  }
  if (argc > 4) {
    if (!ecmdIsAllDecimal(argv[4])) {
      ecmdOutputError("simPUTFAC - Non-decimal numbers detected in offset field\n");
      return ECMD_INVALID_ARGS;
    }
    offset = (uint32_t)atoi(argv[4]);
  }

  if (argc > 5) {
    ecmdOutputError("simPUTFAC - Too many arguments to simPUTFACS, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simPUTFAC(facname, bitLength, buffer, row, offset);

  return rc;

}


uint32_t ecmdSimPUTFACXUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr == NULL) {
    format = "bX";
  }
  else {
    format = formatPtr;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  if (argc < 3) {
    ecmdOutputError("simPUTFACX - Too few arguments to simPUTFACX, you need at least a symbol , data, and a length.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];

  if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("simPUTFACX - Non-decimal numbers detected in bitLength field\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t bitLength = (uint32_t)atoi(argv[2]);

  ecmdDataBuffer buffer;
  rc = ecmdReadDataFormatted(buffer, argv[1], format, (int)bitLength);
  if (rc) {
    ecmdOutputError("simPUTFACX - Problems occurred parsing input data, must be an invalid format\n");
    return rc;
  }


  uint32_t row = 0, offset = 0;
  if (argc > 3) {
    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("simPUTFACX - Non-decimal numbers detected in row field\n");
      return ECMD_INVALID_ARGS;
    }
    row = (uint32_t)atoi(argv[3]);
  }
  if (argc > 4) {
    if (!ecmdIsAllDecimal(argv[4])) {
      ecmdOutputError("simPUTFACX - Non-decimal numbers detected in offset field\n");
      return ECMD_INVALID_ARGS;
    }
    offset = (uint32_t)atoi(argv[4]);
  }

  if (argc > 5) {
    ecmdOutputError("simPUTFACX - Too many arguments to simPUTFACX, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simPUTFACX(facname, bitLength, buffer, row, offset);

  return rc;

}

uint32_t ecmdSimputtcfacUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr == NULL) {
    format = "xr";
  }
  else {
    format = formatPtr;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  if (argc < 2) {
    ecmdOutputError("simputtcfac - Too few arguments to simputtcfac, you need at least a symbol and data.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];

  ecmdDataBuffer buffer;
  rc = ecmdReadDataFormatted(buffer, argv[1], format);
  if (rc) {
    ecmdOutputError("simputtcfac - Problems occurred parsing input data, must be an invalid format\n");
    return rc;
  }

  uint32_t row = 0, numRows = 0;
  if (argc == 4) {
    if (!ecmdIsAllDecimal(argv[2])) {
      ecmdOutputError("simputtcfac - Non-decimal numbers detected in row field\n");
      return ECMD_INVALID_ARGS;
    }
    row = (uint32_t)atoi(argv[2]);

    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("simputtcfac - Non-decimal numbers detected in numRows field\n");
      return ECMD_INVALID_ARGS;
    }
    numRows = (uint32_t)atoi(argv[3]);


    if (argc > 4) {
      ecmdOutputError("simputtcfac - Too many arguments to simputtcfac, you probably added a non-supported option.\n");
      return ECMD_INVALID_ARGS;
    }

  } else if (argc > 2) {

    ecmdOutputError("simputtcfac - Too many arguments to simputtcfac, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simputtcfac(facname, buffer.getBitLength(), buffer, row, numRows);

  return rc;

}

uint32_t ecmdSimrestartUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc < 1) {
    ecmdOutputError("simrestart - At least one argument (a checkpoint filename) is required for simrestart.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 1) {
    ecmdOutputError("simrestart - Too many arguments to simrestart, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simrestart(argv[0]);

  return rc;

}

uint32_t ecmdSimSTKFACUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr == NULL) {
    format = "xr";
  }
  else {
    format = formatPtr;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  if (argc < 3) {
    ecmdOutputError("simSTKFAC - Too few arguments to simSTKFACS, you need at least a symbol , data, and a length.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];

  if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("simSTKFAC - Non-decimal numbers detected in bitLength field\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t bitLength = (uint32_t)atoi(argv[2]);

  ecmdDataBuffer buffer;
  rc = ecmdReadDataFormatted(buffer, argv[1], format, (int)bitLength);
  if (rc) {
    ecmdOutputError("simSTKFAC - Problems occurred parsing input data, must be an invalid format\n");
    return rc;
  }


  uint32_t row = 0, offset = 0;
  if (argc > 3) {
    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("simSTKFAC - Non-decimal numbers detected in row field\n");
      return ECMD_INVALID_ARGS;
    }
    row = (uint32_t)atoi(argv[3]);
  }
  if (argc > 4) {
    if (!ecmdIsAllDecimal(argv[4])) {
      ecmdOutputError("simSTKFAC - Non-decimal numbers detected in offset field\n");
      return ECMD_INVALID_ARGS;
    }
    offset = (uint32_t)atoi(argv[4]);
  }

  if (argc > 5) {
    ecmdOutputError("simSTKFAC - Too many arguments to simSTKFACS, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simSTKFAC(facname, bitLength, buffer, row, offset);

  return rc;

}

uint32_t ecmdSimSTKFACXUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr == NULL) {
    format = "bX";
  }
  else {
    format = formatPtr;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  if (argc < 3) {
    ecmdOutputError("simSTKFACX - Too few arguments to simSTKFACX, you need at least a symbol , data, and a length.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];

  if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("simSTKFACX - Non-decimal numbers detected in bitLength field\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t bitLength = (uint32_t)atoi(argv[2]);

  ecmdDataBuffer buffer;
  rc = ecmdReadDataFormatted(buffer, argv[1], format, (int)bitLength);
  if (rc) {
    ecmdOutputError("simSTKFACX - Problems occurred parsing input data, must be an invalid format\n");
    return rc;
  }


  uint32_t row = 0, offset = 0;
  if (argc > 3) {
    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("simSTKFACX - Non-decimal numbers detected in row field\n");
      return ECMD_INVALID_ARGS;
    }
    row = (uint32_t)atoi(argv[3]);
  }
  if (argc > 4) {
    if (!ecmdIsAllDecimal(argv[4])) {
      ecmdOutputError("simSTKFACX - Non-decimal numbers detected in offset field\n");
      return ECMD_INVALID_ARGS;
    }
    offset = (uint32_t)atoi(argv[4]);
  }

  if (argc > 5) {
    ecmdOutputError("simSTKFACX - Too many arguments to simSTKFACS, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simSTKFACX(facname, bitLength, buffer, row, offset);

  return rc;

}

uint32_t ecmdSimstktcfacUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr == NULL) {
    format = "xr";
  }
  else {
    format = formatPtr;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  if (argc < 3) {
    ecmdOutputError("simstktcfac - Too few arguments to simstktcfac, you need at least a symbol , data, and a length.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];

  if (!ecmdIsAllDecimal(argv[2])) {
    ecmdOutputError("simstktcfac - Non-decimal numbers detected in bitLength field\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t bitLength = (uint32_t)atoi(argv[2]);

  ecmdDataBuffer buffer;
  rc = ecmdReadDataFormatted(buffer, argv[1], format, (int)bitLength);
  if (rc) {
    ecmdOutputError("simstktcfac - Problems occurred parsing input data, must be an invalid format\n");
    return rc;
  }

  uint32_t row = 0, numRows = 0;
  if (argc > 3) {
    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("simstktcfac - Non-decimal numbers detected in row field\n");
      return ECMD_INVALID_ARGS;
    }
    row = (uint32_t)atoi(argv[3]);
  }
  if (argc > 4) {
    if (!ecmdIsAllDecimal(argv[4])) {
      ecmdOutputError("simstktcfac - Non-decimal numbers detected in numRows field\n");
      return ECMD_INVALID_ARGS;
    }
    numRows = (uint32_t)atoi(argv[4]);
  }

  if (argc > 5) {
    ecmdOutputError("simstktcfac - Too many arguments to simstktcfac, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simstktcfac(facname, bitLength, buffer, row, numRows);

  return rc;

}

uint32_t ecmdSimSUBCMDUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  char buf[1000];
  buf[0] = '\0';

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  if (argc < 1) {
    ecmdOutputError("simSUBCMD - At least one argument (a command to execute) is required for simSUBCMD.\n");
    return ECMD_INVALID_ARGS;
  }

  for (int idx = 0; idx < argc; idx ++) {
    strcat(buf,argv[idx]);
    strcat(buf," ");
  }

  rc = simSUBCMD(buf);

  return rc;

}

uint32_t ecmdSimCallFusionCommandUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;
  std::string object, id, command, returnString, printed;

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc < 3) {
    ecmdOutputError("simcallfusioncommand - At least 3 arguments (fusion object, replica ID and command) is required for simcallfusioncommand.\n");
    return ECMD_INVALID_ARGS;
  }

  // Parse the input args
  object = argv[0];
  id = argv[1];

  // The command could have spaces in it, so make sure we pull it all in
  for (int idx = 2; idx < argc; idx++) {
    command += argv[idx];
    command += " ";
  }

  returnString = simCallFusionCommand(object.c_str(), id.c_str(), command.c_str());

  //Print Output
  printed = "simcallfusioncommand - Output from executing the command '" + command + "':\n\n";
  ecmdOutput(printed.c_str());
  
  if(returnString.length() != 0) {
    ecmdOutput(returnString.c_str());
  } else {
    ecmdOutput("No Output received from simCallFusionCommand\n");
  }

  return rc;
}


uint32_t ecmdSimTckIntervalUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;
  uint32_t cycles ;

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  if (argc < 1) {
    ecmdOutputError("simtckinterval - At least one argument (a cycle count) is required for simtckinterval.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 1) {
    ecmdOutputError("simtckinterval - Too many arguments to simtckinterval, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  if (!ecmdIsAllDecimal(argv[0])) {
    ecmdOutputError("simtckinterval - Non-decimal numbers detected in cycles field\n");
    return ECMD_INVALID_ARGS;
  }
  cycles = (uint32_t)atoi(argv[0]);

  rc = simtckinterval(cycles);

  return rc;

}



uint32_t ecmdSimUNSTICKUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr == NULL) {
    format = "xr";
  }
  else {
    format = formatPtr;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  if (argc < 2) {
    ecmdOutputError("simUNSTICK - Too few arguments to simUNSTICKS, you need at least a symbol and a length.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];

  if (!ecmdIsAllDecimal(argv[1])) {
    ecmdOutputError("simUNSTICK - Non-decimal numbers detected in bitLength field\n");
    return ECMD_INVALID_ARGS;
  }
  uint32_t bitLength = (uint32_t)atoi(argv[1]);

  uint32_t row = 0, offset = 0;
  if (argc > 2) {
    if (!ecmdIsAllDecimal(argv[2])) {
      ecmdOutputError("simUNSTICK - Non-decimal numbers detected in row field\n");
      return ECMD_INVALID_ARGS;
    }
    row = (uint32_t)atoi(argv[2]);
  }
  if (argc > 3) {
    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("simUNSTICK - Non-decimal numbers detected in offset field\n");
      return ECMD_INVALID_ARGS;
    }
    offset = (uint32_t)atoi(argv[3]);
  }

  if (argc > 4) {
    ecmdOutputError("simUNSTICK - Too many arguments to simUNSTICKS, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simUNSTICK(facname, bitLength, row, offset);

  return rc;

}

uint32_t ecmdSimunsticktcfacUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  /* get format flag, if it's there */
  std::string format;
  uint32_t bitLength = 0;
  uint32_t row = 0, numRows = 0;
  ecmdDataBuffer buffer;

  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr == NULL) {
    format = "xr";
  }
  else {
    format = formatPtr;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;



  if (argc < 1) {
    ecmdOutputError("simunsticktcfac - Too few arguments to simunsticktcfac, you need at least a symbol.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];

  if (argc > 1) {
    if (!ecmdIsAllDecimal(argv[2])) {
      ecmdOutputError("simunsticktcfac - Non-decimal numbers detected in bitLength field\n");
      return ECMD_INVALID_ARGS;
    }
    bitLength = (uint32_t)atoi(argv[2]);

    rc = ecmdReadDataFormatted(buffer, argv[1], format, (int)bitLength);
    if (rc) {
      ecmdOutputError("simunsticktcfac - Problems occurred parsing input data, must be an invalid format\n");
      return rc;
    }

    if (argc > 3) {
      if (!ecmdIsAllDecimal(argv[3])) {
        ecmdOutputError("simunsticktcfac - Non-decimal numbers detected in row field\n");
        return ECMD_INVALID_ARGS;
      }
      row = (uint32_t)atoi(argv[3]);
    }
    if (argc > 4) {
      if (!ecmdIsAllDecimal(argv[4])) {
        ecmdOutputError("simunsticktcfac - Non-decimal numbers detected in numRows field\n");
        return ECMD_INVALID_ARGS;
      }
      numRows = (uint32_t)atoi(argv[4]);
    }

    if (argc > 5) {
      ecmdOutputError("simunsticktcfac - Too many arguments to simunsticktcfac, you probably added a non-supported option.\n");
      return ECMD_INVALID_ARGS;
    }
  }

  rc = simunsticktcfac(facname, bitLength, buffer, row, numRows);

  return rc;

}

uint32_t ecmdSimGetHierarchyUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;
  ecmdChipTarget target;                /// Current target being operated on
  std::string  hierarchy;		/// Return the model hierarchy for this target
  ecmdLooperData looperdata;            /// Store internal Looper data
  std::string printed;
  bool validPosFound = false;
  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode


  if (argc < 1) {
    ecmdOutputError("simgethierarchy - Too few arguments specified; you need at least a chip name.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 2) {
    ecmdOutputError("simgethierarchy - Too many arguments to simgethierarchy, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }
  
  //Setup the target  
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);

  int depth = 0;                 ///< depth found from Command line parms
  int CHIPUNIT = 1;
  if (ecmdParseOption(&argc, &argv, "-dc"))        depth = CHIPUNIT;

  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitType = chipUnitType;
  target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;

  if (depth != CHIPUNIT){
    target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
    target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;
  }
  target.cageState = target.nodeState = target.slotState = target.posState =  ECMD_TARGET_FIELD_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {
    validPosFound = true;
    rc = simGetHierarchy(target,  hierarchy);
    if (rc) {
      coeRc = rc;
      continue;
    }

    printed = "Model hierarchy for target ";
    printed += ecmdWriteTarget(target);
    printed += "is :\n" + hierarchy + "\n";
    ecmdOutput(printed.c_str());
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

 
  if (!validPosFound) {
    ecmdOutputError("simgethierarchy - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }


  return rc;
}

uint32_t ecmdSimOutputFusionMessageUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;
  std::string header;              ///< Message header
  std::string message;             ///< Message text
  ecmdFusionSeverity_t severity;   ///< Severity
  ecmdFusionMessageType_t type;    ///< Message type
  std::string file;                ///< File where the message is originating frm
  bool fileSpecified = false;      ///< Is the file is specified
  uint32_t line = 0;               ///< Line in File where message is originating from
  
  char * sev = ecmdParseOptionWithArgs(&argc, &argv, "-severity");
  if ((sev == NULL) || (strcmp(sev, "info") == 0)) {
    severity = ECMD_SIM_INFO;
  } else if (strcmp(sev, "error") == 0) {
    severity = ECMD_SIM_ERROR;
  } else if (strcmp(sev, "warning") == 0) {
    severity = ECMD_SIM_WARNING;
  } else if (strcmp(sev, "plain") == 0) {
    severity = ECMD_SIM_PLAIN;
  } else {
    ecmdOutputError("simoutputfusionmessage - Invalid value specified for severity.");
    ecmdOutputError("simoutputfusionmessage - Valid values are : error, warning, info, plain\n");
    return ECMD_INVALID_ARGS;
  }
  
  char * msgtype = ecmdParseOptionWithArgs(&argc, &argv, "-msgtype");
  if ((msgtype == NULL) || (strcmp(msgtype, "testcase") == 0)) {
    type = ECMD_SIM_MSG_TESTCASE;
  } else if (strcmp(msgtype, "exception") == 0) {
    type = ECMD_SIM_MSG_EXCEPTION;
  } else if (strcmp(msgtype, "cmd_rs") == 0) {
    type = ECMD_SIM_MSG_CMD_RS;
  } else if (strcmp(msgtype, "cmd_exe") == 0) {
    type = ECMD_SIM_MSG_CMD_EXE;
  } else if (strcmp(msgtype, "debug") == 0) {
    type = ECMD_SIM_MSG_DEBUG;
  } else if (strcmp(msgtype, "broadside") == 0) {
    type = ECMD_SIM_MSG_BROADSIDE;
  } else if (strcmp(msgtype, "end_sd_msgs") == 0) {
    type = ECMD_SIM_MSG_END_SD_MSGS;
  } else if (strcmp(msgtype, "always") == 0) {
    type = ECMD_SIM_MSG_ALWAYS;
  } else {
    ecmdOutputError("simoutputfusionmessage - Invalid value specified for msgtype.");
    ecmdOutputError("simoutputfusionmessage - Valid values are : exception, testcase, cmd_rs, cmd_exe, debug, broadside, end_sd_msgs, always\n");
    return ECMD_INVALID_ARGS;
  }
  
  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  if (argc < 2) {
    ecmdOutputError("simoutputfusionmessage - Too few arguments specified;  you need at least a header and a message.\n");
    return ECMD_INVALID_ARGS;
  }
  
  header = argv[0];
  message = argv[1];
  
  if (argc > 2) {
    file = argv[2];
    fileSpecified = true;
  } 
  
  if (argc > 3) {
    if (!ecmdIsAllDecimal(argv[3])) {
     ecmdOutputError("simoutputfusionmessage - Non-decimal numbers detected in line field\n");
     return ECMD_INVALID_ARGS;
    }
    line = (uint32_t)atoi(argv[3]);
  } 
  
  if (fileSpecified) {
   rc = simOutputFusionMessage(header.c_str(), message.c_str(), severity, type, file.c_str(), line);
  } else
   rc = simOutputFusionMessage(header.c_str(), message.c_str(), severity, type, NULL, line);

  return rc;

}

uint32_t ecmdSimPutDialUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;
  ecmdChipTarget target;                ///< Current target being operated on
  ecmdLooperData looperData;            ///< Store internal Looper data
  std::string printed;                  ///< Output string data
  bool validPosFound = false;           ///< Did the looper find something?

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  if (argc != 3) {
    ecmdOutputError("simputdial - Incorrect arguments to simgetdial.  Run simputdial -h for proper syntax.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {

    rc = simPutDial(argv[1], argv[2]);
    if (rc) {
      printed = "simputdial - Error occured performing simputdial on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError(printed.c_str());
      coeRc = rc;
      continue;
    } else {
      validPosFound = true;
    }

    printed = ecmdWriteTarget(target) + "\n";
    ecmdOutputError(printed.c_str());
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  if (!validPosFound) {
    ecmdOutputError("getspy - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdSimGetDialUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;
  ecmdChipTarget target;                ///< Current target being operated on
  ecmdLooperData looperData;            ///< Store internal Looper data
  std::string value;                    ///< The return value
  std::string printed;                  ///< Output string data
  bool validPosFound = false;           ///< Did the looper find something?

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  if (argc != 2) {
    ecmdOutputError("simgetdial - Incorrect arguments to simgetdial.  Run simgetdial -h for proper syntax.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {

    rc = simGetDial(argv[1], value);
    if (rc) {
      printed = "simgetdial - Error occured performing simgetdial on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError(printed.c_str());
      coeRc = rc;
      continue;
    } else {
      validPosFound = true;
    }

    printed = ecmdWriteTarget(target) + " " + value.c_str() + "\n";
    ecmdOutputError(printed.c_str());
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  if (!validPosFound) {
    ecmdOutputError("getspy - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

#endif
