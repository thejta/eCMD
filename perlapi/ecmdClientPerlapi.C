/* $Header$ */
// Copyright **********************************************************
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
// End Copyright ******************************************************

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <ecmdClientCapi.H>
#include <ecmdReturnCodes.H>
#include <ecmdUtils.H>
#include <ecmdClientPerlapi.H>
#include <ecmdClientPerlapiFunc.H>
#include <ecmdSharedUtils.H>

// NOTE:
// gcc compiler used in zSeries build environment has an include order
// dependency on the EXTERN.h, perl.h & XSUB.h header files.  They have
// to be at the end of the included files.
//
// Include the Swig Perl headers for Croak
#include "EXTERN.h"    
#include "perl.h"
#include "XSUB.h"

/* The 3 safe mode functions below are now no-ops in version 6.x.  Will be removed in v7 - JTA 12/07/05 */
bool ECMDPERLAPI::ecmdQuerySafeMode() {
  return false;
}

void ECMDPERLAPI::ecmdDisablePerlSafeMode() { return; }

void ECMDPERLAPI::ecmdEnablePerlSafeMode() { return; }


void ECMDPERLAPI::ecmdUnloadDll()  {
  ::ecmdUnloadDll();
}


int ECMDPERLAPI::ecmdLoadDll (const char * i_dllName, const char * i_clientVersion) {

  int rc = ECMD_SUCCESS;
  std::string dllName = "";
  if (i_dllName != NULL) {
    dllName = i_dllName;
  }

  rc = ::ecmdLoadDll(dllName);


  /* Check our Perl Major Version */
  char capiVersion[10];
  strcpy(capiVersion,"ver");
  strcat(capiVersion,ECMD_CAPI_VERSION);
  int majorlength = (int)(strchr(capiVersion, '.') - capiVersion);
  /* Strip off the minor version */
  capiVersion[majorlength] = '\0';

  if (!strstr(i_clientVersion,capiVersion)) {
    fprintf(stderr,"**** FATAL : eCMD Perl Module and your client major version numbers don't match, they are not compatible\n");
    fprintf(stderr,"**** FATAL : Client Version(s) : %s   : Perl Module Version : %s\n",i_clientVersion, ECMD_CAPI_VERSION);

    croak("(ecmdLoadDll) :: Perl Module version mismatch - execution halted\n");
  }

  return rc;

}

int ECMDPERLAPI::ecmdCommandArgs(char** i_argv){

  int rc =0;
  int looper =0;

  while(i_argv[looper] != NULL) looper++;

  rc = ::ecmdCommandArgs(&looper, &i_argv);

  return rc;
}

/* This is overwritten because the retval is a number not a return code */
uint32_t ECMDPERLAPI::simFusionRand32(uint32_t i_min , uint32_t i_max , const char* i_fusionRandObject ) { 
  return ::simFusionRand32(i_min, i_max, i_fusionRandObject);
}

/* This is overwritten to handle passing in NULL for ringName */
uint32_t ECMDPERLAPI::getLatch(ecmdChipTarget & i_target, const char* i_ringName, const char * i_latchName, std::list<ecmdLatchEntry> & o_data, ecmdLatchMode_t i_mode) { 
  uint32_t rc = ECMD_SUCCESS;
  if (strlen(i_ringName) == 0)
    rc = ::getLatch(i_target, NULL, i_latchName, o_data, i_mode);
  else
    rc = ::getLatch(i_target, i_ringName, i_latchName, o_data, i_mode);
  return rc;
}

/* This is overwritten to handle passing in NULL for ringName */
uint32_t ECMDPERLAPI::putLatch(ecmdChipTarget & i_target, const char* i_ringName, const char * i_latchName, ecmdDataBuffer & i_data, uint32_t i_startBit, uint32_t i_numBits, uint32_t & o_matchs, ecmdLatchMode_t i_mode) { 
  uint32_t rc = ECMD_SUCCESS;
  if (strlen(i_ringName) == 0)
    rc = ::putLatch(i_target, NULL, i_latchName, i_data, i_startBit, i_numBits, o_matchs, i_mode);
  else
    rc = ::putLatch(i_target, i_ringName, i_latchName, i_data, i_startBit, i_numBits, o_matchs, i_mode);
  return rc;
}

bool ECMDPERLAPI::ecmdParseOption (char ** io_argv, const char * i_option){

  int looper =0;

  while(io_argv[looper] != NULL) looper++;

  return ::ecmdParseOption(&looper, &io_argv,i_option);

}

char * ECMDPERLAPI::ecmdParseOptionWithArgs(char ** io_argv, const char * i_option){

  char * ret = NULL;
  int looper =0;

  while(io_argv[looper] != NULL) looper++;

  ret = ::ecmdParseOptionWithArgs(&looper, &io_argv,i_option);

  return ret;
}
