// Copyright ***********************************************************
//                                                                      
// File ecmdDaSimUser.C                                  
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

// Module Description **************************************************
//
// Description: 
//
// End Module Description **********************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define ecmdDaSimUser_C
#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <stdlib.h>
#undef ecmdDaSimUser_C
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

int ecmdSimaetUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

  if (argc < 1) {
    ecmdOutputError("At least one argument ('on', 'off', or 'flush') is required for simaet.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 1) {
    ecmdOutputError("Too many arguments to simaet, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simaet(argv[0]);

  return rc;

}


int ecmdSimcheckpointUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

  if (argc < 1) {
    ecmdOutputError("At least one argument (a checkpoint filename) is required for simcheckpoint.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 1) {
    ecmdOutputError("Too many arguments to simcheckpoint, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simcheckpoint(argv[0]);

  return rc;

}


int ecmdSimclockUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

  if (argc < 1) {
    ecmdOutputError("At least one argument (a number of clock cycles) is required for simclock.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 1) {
    ecmdOutputError("Too many arguments to simclock, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  int numcycles = atoi(argv[0]);
  rc = simclock(numcycles);

  return rc;

}


int ecmdSimechoUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

  if (argc < 1) {
    ecmdOutputError("At least one argument (a message to print) is required for simecho.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 1) {
    ecmdOutputError("Too many arguments to simecho, you probably didn't put your message in quotes.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simecho(argv[0]);

  return rc;

}


int ecmdSimexitUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

  if (argc > 0) {
    ecmdOutputError("Too many arguments to simexit, none are required.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simexit();

  return rc;

}

int ecmdSimEXPECTFACUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

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
    ecmdOutputError("Too few arguments to simEXPECTFAC, you need at least a symbol , data, and a length.\n");
    return ECMD_INVALID_ARGS;
  }

  uint32_t symbol = 0x0;
  ecmdGenB32FromHexRight(&symbol, argv[0]);

  ecmdDataBuffer buffer(3);
  rc = ecmdReadDataFormatted(buffer, argv[1], format);
  if (rc) return rc;

  uint32_t bitLength = atoi(argv[2]);

  uint32_t row = 0, offset = 0;
  if (argc > 3) {
    row = atoi(argv[3]);
  }
  if (argc > 4) {
    offset = atoi(argv[4]);
  }

  if (argc > 5) {
    ecmdOutputError("Too many arguments to simEXPECTFAC, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simEXPECTFAC(symbol, bitLength, buffer, row, offset);

  return rc;

}

int ecmdSimEXPECTFACSUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

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
    ecmdOutputError("Too few arguments to simEXPECTFACS, you need at least a symbol , data, and a length.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];

  ecmdDataBuffer buffer(3);
  rc = ecmdReadDataFormatted(buffer, argv[1], format);
  if (rc) return rc;

  uint32_t bitLength = atoi(argv[2]);

  uint32_t row = 0, offset = 0;
  if (argc > 3) {
    row = atoi(argv[3]);
  }
  if (argc > 4) {
    offset = atoi(argv[4]);
  }

  if (argc > 5) {
    ecmdOutputError("Too many arguments to simEXPECTFACS, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simEXPECTFACS(facname, bitLength, buffer, row, offset);

  return rc;

}

int ecmdSimexpecttcfacUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

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
    ecmdOutputError("Too few arguments to simexpecttcfac, you need at least a symbol , data, and a length.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];

  ecmdDataBuffer buffer(3);
  rc = ecmdReadDataFormatted(buffer, argv[1], format);
  if (rc) return rc;

  uint32_t bitLength = atoi(argv[2]);

  uint32_t row = 0;
  if (argc > 3) {
    row = atoi(argv[3]);
  }

  if (argc > 4) {
    ecmdOutputError("Too many arguments to simexpecttcfac, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simexpecttcfac(facname, bitLength, buffer, row);

  return rc;

}


int ecmdSimGETFACUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

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
    ecmdOutputError("Too few arguments to simGETFAC, you need at least a symbol and a length.\n");
    return ECMD_INVALID_ARGS;
  }

  uint32_t symbol = 0x0;
  ecmdGenB32FromHexRight(&symbol, argv[0]);

  uint32_t bitLength = atoi(argv[1]);

  uint32_t row = 0, offset = 0;
  if (argc > 2) {
    row = atoi(argv[2]);
  }
  if (argc > 3) {
    offset = atoi(argv[3]);
  }

  if (argc > 4) {
    ecmdOutputError("Too many arguments to simGETFAC, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  ecmdDataBuffer buffer(3);

  rc = simGETFAC(symbol, bitLength, buffer, row, offset);

  std::string printed = ecmdWriteDataFormatted(buffer, format);
  ecmdOutput(printed.c_str());

  return rc;

}

int ecmdSimGETFACSUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

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
    ecmdOutputError("Too few arguments to simGETFACS, you need at least a facname and bit length.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];
  uint32_t bitLength = atoi(argv[1]);

  uint32_t row = 0, offset = 0;
  if (argc > 2) {
    row = atoi(argv[2]);
  }
  if (argc > 3) {
    offset = atoi(argv[3]);
  }

  if (argc > 4) {
    ecmdOutputError("Too many arguments to simGETFACS, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  ecmdDataBuffer buffer(3);

  rc = simGETFACS(facname, bitLength, buffer, row, offset);

  std::string printed = ecmdWriteDataFormatted(buffer, format);
  ecmdOutput(printed.c_str());

  return rc;

}

int ecmdSimGETFACXUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

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
    ecmdOutputError("Too few arguments to simGETFACX, you need at least a facname and bit length.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];
  uint32_t bitLength = atoi(argv[1]);

  uint32_t row = 0, offset = 0;
  if (argc > 2) {
    row = atoi(argv[2]);
  }
  if (argc > 3) {
    offset = atoi(argv[3]);
  }

  if (argc > 4) {
    ecmdOutputError("Too many arguments to simGETFACX, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  ecmdDataBuffer buffer(3);

  rc = simGETFACX(facname, bitLength, buffer, row, offset);

  std::string printed = ecmdWriteDataFormatted(buffer, format);
  ecmdOutput(printed.c_str());

  return rc;

}

int ecmdSimgettcfacUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

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


  if (argc < 1) {
    ecmdOutputError("Too few arguments to simgettcfac, you need at least a tcfacname.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];

  uint32_t row = 0, startBit = 0, bitLength = 0;

  if (argc > 1) {
    row = atoi(argv[1]);
  }
  if (argc > 2) {
    startBit = atoi(argv[2]);
  }
  if (argc > 3) {
    bitLength = atoi(argv[3]);
  }

  if (argc > 4) {
    ecmdOutputError("Too many arguments to simgettcfac, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  ecmdDataBuffer buffer(3);

  rc = simgettcfac(facname, buffer, row, startBit, bitLength);

  std::string printed = ecmdWriteDataFormatted(buffer, format);
  ecmdOutput(printed.c_str());

  return rc;

}

int ecmdSimgetcurrentcycleUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

  if (argc > 0) {
    ecmdOutputError("Too many arguments to simgetcurrentcycle, none are required.\n");
    return ECMD_INVALID_ARGS;
  }

  int cycle_count = 0x0;
  rc = simgetcurrentcycle(cycle_count);
  if (rc) return rc;

  char outstr[40];
  sprintf(outstr, "Current cycle count is %d\n", cycle_count);
  ecmdOutput(outstr);

  return rc;

}


int ecmdSiminitUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

  if (argc < 1) {
    ecmdOutputError("At least one argument (a checkpoint filename or 'NONE') is required for siminit.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 1) {
    ecmdOutputError("Too many arguments to siminit, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = siminit(argv[0]);

  return rc;

}

int ecmdSimPUTFACUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

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
    ecmdOutputError("Too few arguments to simPUTFAC, you need at least a symbol , data, and a length.\n");
    return ECMD_INVALID_ARGS;
  }

  uint32_t symbol = 0x0;
  ecmdGenB32FromHexRight(&symbol, argv[0]);

  ecmdDataBuffer buffer(3);
  rc = ecmdReadDataFormatted(buffer, argv[1], format);
  if (rc) return rc;

  uint32_t bitLength = atoi(argv[2]);

  uint32_t row = 0, offset = 0;
  if (argc > 3) {
    row = atoi(argv[3]);
  }
  if (argc > 4) {
    offset = atoi(argv[4]);
  }

  if (argc > 5) {
    ecmdOutputError("Too many arguments to simPUTFAC, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simPUTFAC(symbol, bitLength, buffer, row, offset);

  return rc;

}

int ecmdSimPUTFACSUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

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
    ecmdOutputError("Too few arguments to simPUTFACS, you need at least a symbol , data, and a length.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];

  ecmdDataBuffer buffer(3);
  rc = ecmdReadDataFormatted(buffer, argv[1], format);
  if (rc) return rc;

  uint32_t bitLength = atoi(argv[2]);

  uint32_t row = 0, offset = 0;
  if (argc > 3) {
    row = atoi(argv[3]);
  }
  if (argc > 4) {
    offset = atoi(argv[4]);
  }

  if (argc > 5) {
    ecmdOutputError("Too many arguments to simPUTFACS, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simPUTFACS(facname, bitLength, buffer, row, offset);

  return rc;

}


int ecmdSimPUTFACXUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

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
    ecmdOutputError("Too few arguments to simPUTFACX, you need at least a symbol , data, and a length.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];

  ecmdDataBuffer buffer(3);
  rc = ecmdReadDataFormatted(buffer, argv[1], format);
  if (rc) return rc;

  uint32_t bitLength = atoi(argv[2]);

  uint32_t row = 0, offset = 0;
  if (argc > 3) {
    row = atoi(argv[3]);
  }
  if (argc > 4) {
    offset = atoi(argv[4]);
  }

  if (argc > 5) {
    ecmdOutputError("Too many arguments to simPUTFACX, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simPUTFACX(facname, bitLength, buffer, row, offset);

  return rc;

}

int ecmdSimputtcfacUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

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
    ecmdOutputError("Too few arguments to simputtcfac, you need at least a symbol , data, and a length.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];

  ecmdDataBuffer buffer(3);
  rc = ecmdReadDataFormatted(buffer, argv[1], format);
  if (rc) return rc;

  uint32_t bitLength = atoi(argv[2]);

  uint32_t row = 0, numRows = 0;
  if (argc > 3) {
    row = atoi(argv[3]);
  }
  if (argc > 4) {
    numRows = atoi(argv[4]);
  }

  if (argc > 5) {
    ecmdOutputError("Too many arguments to simputtcfac, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simputtcfac(facname, bitLength, buffer, row, numRows);

  return rc;

}

int ecmdSimrestartUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

  if (argc < 1) {
    ecmdOutputError("At least one argument (a checkpoint filename) is required for simrestart.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 1) {
    ecmdOutputError("Too many arguments to simrestart, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simrestart(argv[0]);

  return rc;

}

int ecmdSimSTKFACUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

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
    ecmdOutputError("Too few arguments to simSTKFAC, you need at least a symbol , data, and a length.\n");
    return ECMD_INVALID_ARGS;
  }

  uint32_t symbol = 0x0;
  ecmdGenB32FromHexRight(&symbol, argv[0]);

  ecmdDataBuffer buffer(3);
  rc = ecmdReadDataFormatted(buffer, argv[1], format);
  if (rc) return rc;

  uint32_t bitLength = atoi(argv[2]);

  uint32_t row = 0, offset = 0;
  if (argc > 3) {
    row = atoi(argv[3]);
  }
  if (argc > 4) {
    offset = atoi(argv[4]);
  }

  if (argc > 5) {
    ecmdOutputError("Too many arguments to simSTKFAC, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simSTKFAC(symbol, bitLength, buffer, row, offset);

  return rc;

}

int ecmdSimSTKFACSUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

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
    ecmdOutputError("Too few arguments to simSTKFACS, you need at least a symbol , data, and a length.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];

  ecmdDataBuffer buffer(3);
  rc = ecmdReadDataFormatted(buffer, argv[1], format);
  if (rc) return rc;

  uint32_t bitLength = atoi(argv[2]);

  uint32_t row = 0, offset = 0;
  if (argc > 3) {
    row = atoi(argv[3]);
  }
  if (argc > 4) {
    offset = atoi(argv[4]);
  }

  if (argc > 5) {
    ecmdOutputError("Too many arguments to simSTKFACS, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simSTKFACS(facname, bitLength, buffer, row, offset);

  return rc;

}

int ecmdSimstktcfacUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

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
    ecmdOutputError("Too few arguments to simstktcfac, you need at least a symbol , data, and a length.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];

  ecmdDataBuffer buffer(3);
  rc = ecmdReadDataFormatted(buffer, argv[1], format);
  if (rc) return rc;

  uint32_t bitLength = atoi(argv[2]);

  uint32_t row = 0, numRows = 0;
  if (argc > 3) {
    row = atoi(argv[3]);
  }
  if (argc > 4) {
    numRows = atoi(argv[4]);
  }

  if (argc > 5) {
    ecmdOutputError("Too many arguments to simstktcfac, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simstktcfac(facname, bitLength, buffer, row, numRows);

  return rc;

}

int ecmdSimSUBCMDUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

  if (argc < 1) {
    ecmdOutputError("At least one argument (a command to execute) is required for simSUBCMD.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 1) {
    ecmdOutputError("Too many arguments to simSUBCMD, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simSUBCMD(argv[0]);

  return rc;

}

int ecmdSimsymbolUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

  if (argc < 1) {
    ecmdOutputError("At least one argument (a facname) is required for simsymbol.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 1) {
    ecmdOutputError("Too many arguments to simsymbol, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];
  uint32_t symbol = 0x0;

  rc = simsymbol(facname, symbol);
  if (rc) return rc;

  char outstr[50];
  sprintf(outstr, "Symbol for fac %s is %d\n", facname, symbol);
  ecmdOutput(outstr);

  return rc;

}

int ecmdSimUNSTICKUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

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
    ecmdOutputError("Too few arguments to simUNSTICK, you need at least a symbol , data, and a length.\n");
    return ECMD_INVALID_ARGS;
  }

  uint32_t symbol = 0x0;
  ecmdGenB32FromHexRight(&symbol, argv[0]);

  uint32_t bitLength = atoi(argv[1]);

  uint32_t row = 0, offset = 0;
  if (argc > 2) {
    row = atoi(argv[2]);
  }
  if (argc > 3) {
    offset = atoi(argv[3]);
  }

  if (argc > 4) {
    ecmdOutputError("Too many arguments to simUNSTICK, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simUNSTICK(symbol, bitLength, row, offset);

  return rc;

}

int ecmdSimUNSTICKSUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

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
    ecmdOutputError("Too few arguments to simUNSTICKS, you need at least a symbol , data, and a length.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];

  uint32_t bitLength = atoi(argv[1]);

  uint32_t row = 0, offset = 0;
  if (argc > 2) {
    row = atoi(argv[2]);
  }
  if (argc > 3) {
    offset = atoi(argv[3]);
  }

  if (argc > 4) {
    ecmdOutputError("Too many arguments to simUNSTICKS, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simUNSTICKS(facname, bitLength, row, offset);

  return rc;

}

int ecmdSimunsticktcfacUser(int argc, char * argv[]) {

  int rc = ECMD_SUCCESS;

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
    ecmdOutputError("Too few arguments to simunsticktcfac, you need at least a symbol , data, and a length.\n");
    return ECMD_INVALID_ARGS;
  }

  char * facname = argv[0];

  ecmdDataBuffer buffer(3);
  rc = ecmdReadDataFormatted(buffer, argv[1], format);
  if (rc) return rc;

  uint32_t bitLength = atoi(argv[2]);

  uint32_t row = 0, numRows = 0;
  if (argc > 3) {
    row = atoi(argv[3]);
  }
  if (argc > 4) {
    numRows = atoi(argv[4]);
  }

  if (argc > 5) {
    ecmdOutputError("Too many arguments to simunsticktcfac, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }

  rc = simunsticktcfac(facname, bitLength, buffer, row, numRows);

  return rc;

}

#endif
