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

void ECMDPERLAPI::ecmdUnloadDll() {
  ::ecmdUnloadDll();
}


uint32_t ECMDPERLAPI::ecmdLoadDll (const char * i_dllName, const char * i_clientVersion) {

  uint32_t rc = ECMD_SUCCESS;
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

uint32_t ECMDPERLAPI::ecmdCommandArgs(char** i_argv) {

  uint32_t rc = 0;
  int looper = 0;

  while(i_argv[looper] != NULL) looper++;

  rc = ::ecmdCommandArgs(&looper, &i_argv);

  return rc;
}

#ifndef REMOVE_SIM
/* This is overwritten to handle passing in NULL for i_fusionRandObject */
uint32_t ECMDPERLAPI::simFusionRand32(uint32_t i_min , uint32_t i_max , const char* i_fusionRandObject ) { 
  return ::simFusionRand32(i_min, i_max, i_fusionRandObject);
}
uint64_t ECMDPERLAPI::simFusionRand64(uint64_t i_min , uint64_t i_max , const char* i_fusionRandObject ) { 
  return ::simFusionRand64(i_min, i_max, i_fusionRandObject);
}
#endif

#ifndef ECMD_REMOVE_LATCH_FUNCTIONS
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

/* This is overwritten to handle passing in NULL for ringName */
uint32_t ECMDPERLAPI::getLatchHidden(ecmdChipTarget & i_target, const char* i_ringName, const char * i_latchName, std::list<ecmdLatchEntry> & o_data, ecmdLatchMode_t i_mode, uint32_t i_ring_mode) { 
  uint32_t rc = ECMD_SUCCESS;
  if (strlen(i_ringName) == 0)
    rc = ::getLatchHidden(i_target, NULL, i_latchName, o_data, i_mode, i_ring_mode);
  else
    rc = ::getLatchHidden(i_target, i_ringName, i_latchName, o_data, i_mode, i_ring_mode);
  return rc;
}

/* This is overwritten to handle passing in NULL for ringName */
uint32_t ECMDPERLAPI::putLatchHidden(ecmdChipTarget & i_target, const char* i_ringName, const char * i_latchName, ecmdDataBuffer & i_data, uint32_t i_startBit, uint32_t i_numBits, uint32_t & o_matchs, ecmdLatchMode_t i_mode, uint32_t i_ring_mode) { 
  uint32_t rc = ECMD_SUCCESS;
  if (strlen(i_ringName) == 0)
    rc = ::putLatchHidden(i_target, NULL, i_latchName, i_data, i_startBit, i_numBits, o_matchs, i_mode, i_ring_mode);
  else
    rc = ::putLatchHidden(i_target, i_ringName, i_latchName, i_data, i_startBit, i_numBits, o_matchs, i_mode, i_ring_mode);
  return rc;
}

/* This is overwritten to handle passing in NULL for ringName */
uint32_t ECMDPERLAPI::getLatchImage(ecmdChipTarget & i_target, const char* i_ringName, const char * i_latchName, std::list<ecmdLatchEntry> & o_data, ecmdLatchMode_t i_mode, ecmdDataBuffer & i_ringImage) { 
  uint32_t rc = ECMD_SUCCESS;
  if (strlen(i_ringName) == 0)
    rc = ::getLatchImage(i_target, NULL, i_latchName, o_data, i_mode, i_ringImage);
  else
    rc = ::getLatchImage(i_target, i_ringName, i_latchName, o_data, i_mode, i_ringImage);
  return rc;
}

/* This is overwritten to handle passing in NULL for ringName */
uint32_t ECMDPERLAPI::putLatchImage(ecmdChipTarget & i_target, const char* i_ringName, const char * i_latchName, ecmdDataBuffer & i_data, uint32_t i_startBit, uint32_t i_numBits, uint32_t & o_matchs, ecmdLatchMode_t i_mode, ecmdDataBuffer & i_ringImage) { 
  uint32_t rc = ECMD_SUCCESS;
  if (strlen(i_ringName) == 0)
    rc = ::putLatchImage(i_target, NULL, i_latchName, i_data, i_startBit, i_numBits, o_matchs, i_mode, i_ringImage);
  else
    rc = ::putLatchImage(i_target, i_ringName, i_latchName, i_data, i_startBit, i_numBits, o_matchs, i_mode, i_ringImage);
  return rc;
}

uint32_t ECMDPERLAPI::getLatchOpt(ecmdChipTarget & i_target, std::list<ecmdLatchEntry> & i_scandefLatchInfo, std::list<ecmdLatchEntry> & o_data, uint32_t i_ring_mode) {
    uint32_t rc = ::getLatchOpt(i_target, i_scandefLatchInfo, o_data, i_ring_mode);
    return rc;
}

uint32_t ECMDPERLAPI::putLatchOpt(ecmdChipTarget & i_target, ecmdDataBuffer & i_data, uint32_t i_startBit, uint32_t i_numBits, uint32_t & o_matches, std::list<ecmdLatchEntry> & i_scandefLatchInfo, uint32_t i_ring_mode) {
    uint32_t rc = ::putLatchOpt(i_target, i_data, i_startBit, i_numBits, o_matches, i_scandefLatchInfo, i_ring_mode);
    return rc;
}

uint32_t ECMDPERLAPI::getLatchImageOpt(ecmdChipTarget & i_target, std::list<ecmdLatchEntry> & o_data, std::list<ecmdLatchEntry> & i_scandefLatchInfo, ecmdDataBuffer & i_ringImage) {
    uint32_t rc = ::getLatchImageOpt(i_target, o_data, i_scandefLatchInfo, i_ringImage);
    return rc;
}

uint32_t ECMDPERLAPI::putLatchImageOpt(ecmdChipTarget & i_target, ecmdDataBuffer & i_data, uint32_t i_startBit, uint32_t i_numBits, uint32_t & o_matches, std::list<ecmdLatchEntry> & i_scandefLatchInfo, ecmdDataBuffer & io_ringImage) {
    uint32_t rc = ::putLatchImageOpt(i_target, i_data, i_startBit, i_numBits, o_matches, i_scandefLatchInfo, io_ringImage);
    return rc;
}


#endif

bool ECMDPERLAPI::ecmdParseOption (char ** io_argv, const char * i_option) {

  int looper = 0;

  while(io_argv[looper] != NULL) looper++;

  return ::ecmdParseOption(&looper, &io_argv,i_option);

}

char * ECMDPERLAPI::ecmdParseOptionWithArgs(char ** io_argv, const char * i_option) {

  char * ret = NULL;
  int looper = 0;

  while(io_argv[looper] != NULL) looper++;

  ret = ::ecmdParseOptionWithArgs(&looper, &io_argv,i_option);

  return ret;
}
