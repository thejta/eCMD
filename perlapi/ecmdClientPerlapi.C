
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
#include <ecmdDataBuffer.H>
#include <ecmdReturnCodes.H>
#include <ecmdUtils.H>
#include <ecmdClientPerlapi.H>
#include <ecmdSharedUtils.H>


static int myErrorCode = ECMD_SUCCESS;
static int safeMode = 1;

int ecmdPerlInterfaceErrorCheck (int errorCode) {

  if (errorCode == -1) {
    errorCode = myErrorCode;
    myErrorCode = ECMD_SUCCESS;

    if (errorCode != ECMD_SUCCESS) {
      ::ecmdOutputError( (::ecmdGetErrorMsg(errorCode) + "\n").c_str());
    }

    return errorCode;
  }
  else if (errorCode != ECMD_SUCCESS) {
    myErrorCode = errorCode;
  }

  return ECMD_SUCCESS;
}

ecmdClientPerlapi::ecmdClientPerlapi () {

}

ecmdClientPerlapi::~ecmdClientPerlapi () {

  this->cleanup();
}

void ecmdClientPerlapi::cleanup()  {
  ecmdUnloadDll();
}


int ecmdClientPerlapi::initDll (const char * i_dllName, const char * i_options) {

  int rc = ECMD_SUCCESS;
  std::string dllName = "";
  if (i_dllName != NULL) {
    dllName = i_dllName;
  }

  rc = ecmdLoadDll(dllName);
  ecmdPerlInterfaceErrorCheck(rc);

  return rc;

}


int ecmdClientPerlapi::setupTarget (const char * i_targetStr, ecmdChipTarget & o_target) {

  int rc = ECMD_SUCCESS;
  std::string printed;
  if (i_targetStr == NULL) {
    ecmdOutputError("ecmdClientPerlapi::setupTarget - It appears the target string is null\n");
    return ECMD_INVALID_ARGS;
  }
  std::vector<std::string> tokens;

  ecmdParseTokens(i_targetStr, " ,.", tokens);

  /* Set our initial states */
  o_target.cage = o_target.node = o_target.slot = o_target.pos = o_target.core = o_target.thread = 0;
  o_target.cageState = o_target.nodeState = o_target.slotState = o_target.posState = o_target.coreState = o_target.threadState = ECMD_TARGET_FIELD_VALID;
  o_target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;

  for (std::vector<std::string>::iterator tokit = tokens.begin(); tokit != tokens.end(); tokit ++) {
    if ((*tokit)[0] == '-') {
      switch((*tokit)[1]) {
        case 'k': case 'K':
          o_target.cage = atoi(tokit->substr(2,20).c_str());
          break;
        case 'n': case 'N':
          o_target.node = atoi(tokit->substr(2,20).c_str());
          break;
        case 's': case 'S':
          o_target.slot = atoi(tokit->substr(2,20).c_str());
          break;
        case 'p': case 'P':
          o_target.pos = atoi(tokit->substr(2,20).c_str());
          break;
        case 'c': case 'C':
          o_target.core = atoi(tokit->substr(2,20).c_str());
          break;
        case 't': case 'T':
          o_target.thread = atoi(tokit->substr(2,20).c_str());
          break;
        default:
          ecmdOutputError((((printed = "ecmdClientPerlapi::setupTarget - It appears the target string contained an invalid target parm : ") + i_targetStr) + "\n").c_str());
          return ECMD_INVALID_ARGS;
          
      }
    } else {
      /* If the first char is a digit then they must have specified a p1..3 or p1,4 and we tokenized it above, this is not good */
      if (isdigit((*tokit)[0])) {
        ecmdOutputError((((printed = "ecmdClientPerlapi::setupTarget - It appears the target string contained a range or multiple positions : ") + i_targetStr) + "\n").c_str());
        return ECMD_INVALID_ARGS;
      } else if (o_target.chipTypeState == ECMD_TARGET_FIELD_VALID) {
        ecmdOutputError((((printed = "ecmdClientPerlapi::setupTarget - It appears the target string contained multiple chip names : ") + i_targetStr) + "\n").c_str());
        return ECMD_INVALID_ARGS;
      }
      o_target.chipTypeState = ECMD_TARGET_FIELD_VALID;
      o_target.chipType = *tokit;
    }
  }

  return rc;
}



int ecmdClientPerlapi::getScom (const char* i_target, int i_address, char** o_data) {

  ecmdChipTarget myTarget;

  int rc = setupTarget(i_target, myTarget);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) {
    *o_data = NULL;
    return rc;
  }

  ecmdDataBuffer buffer;
  rc = ::getScom(myTarget, i_address, buffer);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) {
    o_data = NULL;
    return rc;
  }

  char* tmp;
  tmp = new char[buffer.getBitLength()+1];
  strcpy(tmp,buffer.genBinStr().c_str());
  *o_data = tmp;

  return rc;
}



int ecmdClientPerlapi::putScom (const char * i_target, int i_address, const char * i_data) {

  ecmdChipTarget myTarget;

  int rc = setupTarget(i_target, myTarget);
  if (rc) return rc;

  ecmdDataBuffer buffer;

  buffer.setBitLength(strlen(i_data));
  rc = buffer.insertFromBin(i_data);

  rc = ::putScom(myTarget, i_address, buffer);

  ecmdPerlInterfaceErrorCheck(rc);
  return rc;
}

 
 
int ecmdClientPerlapi::getRing (const char * i_target, const char * i_ringName, char **o_data) {

  int rc = 0;
  ecmdDataBuffer buffer;
  ecmdChipTarget myTarget;

  rc = setupTarget(i_target, myTarget);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

  rc = ::getRing(myTarget, i_ringName, buffer);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

  char* tmp;
  tmp = new char[buffer.getBitLength()+1];
  strcpy(tmp,buffer.genBinStr().c_str());
  *o_data = tmp;

  return rc;
}


int ecmdClientPerlapi::putRing (const char * i_target, const char * i_ringName, const char * i_data) {

  ecmdChipTarget myTarget;

  int rc = setupTarget(i_target, myTarget);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

  ecmdDataBuffer buffer;

  buffer.setBitLength(strlen(i_data));
  rc = buffer.insertFromBin(i_data);

  rc = ::putRing(myTarget, i_ringName, buffer);

  ecmdPerlInterfaceErrorCheck(rc);
  return rc;
}


int ecmdClientPerlapi::getSpy (const char* i_target, const char * i_spyName, char** o_data){
  int rc = 0;
  ecmdDataBuffer buffer;
  ecmdChipTarget myTarget;

  rc = setupTarget(i_target, myTarget);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

  rc = ::getSpy(myTarget, i_spyName, buffer);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

  char* tmp;
  tmp = new char[buffer.getBitLength()+1];
  strcpy(tmp,buffer.genBinStr().c_str());
  *o_data = tmp;

  return rc;
}

int ecmdClientPerlapi::putSpy (const char* i_target, const char * i_spyName, const char* i_data){

  ecmdChipTarget myTarget;

  int rc = setupTarget(i_target, myTarget);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

  ecmdDataBuffer buffer;

  buffer.setBitLength(strlen(i_data));
  rc = buffer.insertFromBin(i_data);

  rc = ::putSpy(myTarget, i_spyName, buffer);

  ecmdPerlInterfaceErrorCheck(rc);
  return rc;
}

int ecmdClientPerlapi::getLatch (const char * i_target, const char* i_ringName, const char * i_latchName,  char** o_data, int i_startBit, int i_numBits) {

  int rc = 0;
  int foundOne;
  ecmdDataBuffer buffer;
  ecmdChipTarget myTarget;

  rc = setupTarget(i_target, myTarget);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

  std::list<ecmdLatchEntry> latchEntries;
  std::list<ecmdLatchEntry>::iterator curEntry;

  if (strlen(i_ringName) == 0)
    rc = ::getLatch(myTarget, NULL, i_latchName, latchEntries, ECMD_LATCHMODE_FULL);
  else
    rc = ::getLatch(myTarget, i_ringName, i_latchName, latchEntries, ECMD_LATCHMODE_FULL);

  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;


  if(i_startBit < 0 ) {
    /* just in case we get stupid */
    return ECMD_DATA_UNDERFLOW;
  }


  for(curEntry = latchEntries.begin(), foundOne =0; curEntry != latchEntries.end(); curEntry++) {

    if (i_startBit > curEntry->latchStartBit) {
      /* fail because we are out of range. */
      return ECMD_DATA_UNDERFLOW;
    }

    if ((i_startBit + i_numBits -1) > curEntry->latchEndBit) {
      /* fail because we want to much data */
      return ECMD_DATA_UNDERFLOW;
    }

    if((i_startBit <= curEntry->latchStartBit) &&
       ((i_startBit + i_numBits -1) <= curEntry->latchEndBit) ) {
      /* we are here because this entry falls within the range we want. */

      if (foundOne) {
        /* we should not be here since we found one already! */
        return ECMD_DATA_UNDERFLOW;
      }
      buffer.setBitLength(i_numBits);
      rc = curEntry->buffer.extract(buffer, i_startBit, i_numBits);
      /* data should not be in buffer left alligned */
      foundOne++;
    }

  } /* end of for loop */

  if(foundOne == 0) {
    /* probably handled prior to this spot with the RC from ::getLatch */
    return ECMD_DATA_UNDERFLOW;
  }

  char* tmp;
  tmp = new char[buffer.getBitLength()+1];
  strcpy(tmp,buffer.genBinStr().c_str());
  *o_data = tmp;

  return rc;
}

int ecmdClientPerlapi::putLatch (const char * i_target, const char* i_ringName, const char * i_latchName, const char * i_data, int i_startBit, int i_numBits, int &o_matchs) {

  ecmdChipTarget myTarget;

  int rc = setupTarget(i_target, myTarget);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

  ecmdDataBuffer buffer;
  uint32_t matchs;

  buffer.setBitLength(strlen(i_data));
  rc = buffer.insertFromBin(i_data);

  if (strlen(i_ringName) == 0)
    rc = ::putLatch(myTarget, NULL, i_latchName, buffer, i_startBit, i_numBits, matchs, ECMD_LATCHMODE_FULL);
  else
    rc = ::putLatch(myTarget, i_ringName, i_latchName, buffer, i_startBit, i_numBits, matchs, ECMD_LATCHMODE_FULL);

  o_matchs = matchs; 

  ecmdPerlInterfaceErrorCheck(rc);

  return rc;
}


int ecmdClientPerlapi::getArray (const char* i_target, const char * i_arrayName, const char* i_address, char** o_data){

  int rc = 0;
  ecmdDataBuffer address;
  ecmdDataBuffer buffer;
  ecmdChipTarget myTarget;

  address.setBitLength(strlen(i_address));
  rc = address.insertFromBin(i_address);

  rc = setupTarget(i_target, myTarget);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

  rc = ::getArray(myTarget, i_arrayName, address, buffer);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

  char* tmp;
  tmp = new char[buffer.getBitLength()+1];
  strcpy(tmp,buffer.genBinStr().c_str());
  *o_data = tmp;

  return rc;
}

int ecmdClientPerlapi::putArray (const char* i_target, const char * i_arrayName, const char* i_address, const char* i_data){

  ecmdChipTarget myTarget;

  int rc = setupTarget(i_target, myTarget);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

  ecmdDataBuffer address;
  ecmdDataBuffer buffer;

  buffer.setBitLength(strlen(i_data));
  rc = buffer.insertFromBin(i_data);

  address.setBitLength(strlen(i_address));
  rc = address.insertFromBin(i_address);

  rc = ::putArray(myTarget, i_arrayName, address, buffer);

  ecmdPerlInterfaceErrorCheck(rc);
  return rc;
}


/***
void ecmdClientPerlapi::add(char *retval) {
  printf("Inside function: %c %c %c\n",retval[0],retval[1],retval[2]);

  retval[0] = 'L'; retval[1] = 'p';

 strcpy(retval,"Looky here - I made it");
}

void ecmdClientPerlapi::add2(char **retval) {
  printf("Inside function: %s\n",*retval);


  *retval = (char*)malloc(sizeof (char[100]));
  strcpy(*retval,"Looky here - I made it");
}
***/




int ecmdClientPerlapi::ecmdCommandArgs(char* i_argv[]){

  int rc =0;
  int looper =0;

  while(i_argv[looper] != NULL) looper++;

  rc = ::ecmdCommandArgs(&looper, &i_argv);

  ecmdPerlInterfaceErrorCheck(rc);
  return rc;
}


int ecmdClientPerlapi::sendCmd(const char* i_target, int i_instruction, int i_modifier, char** o_status) {

  return 0;
}


int ecmdClientPerlapi::getCfamRegister (const char* i_target, int i_address, char** o_data){

  return 0;
}

int ecmdClientPerlapi::putCfamRegister (const char* i_target, int i_address, const char* i_data){

  return 0;
}


int ecmdClientPerlapi::getSpyEnum (const char* i_target, const char * i_spyName, char** o_enumValue){

  int rc = 0;
  std::string buffer; 
  ecmdChipTarget myTarget;

  rc = setupTarget(i_target, myTarget);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

  rc = ::getSpyEnum(myTarget, i_spyName, buffer);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

  char* tmp;
  tmp = new char[buffer.size()+1];
  strcpy(tmp,buffer.c_str());
  *o_enumValue = tmp;

  return rc;
}

int ecmdClientPerlapi::getSpyEccGrouping (const char* i_target, const char * i_spyEccGroupName, char** o_groupData, char** o_eccData, char** o_eccErrorMask){

  return 0;
}



int ecmdClientPerlapi::putSpyEnum (const char* i_target, const char * i_spyName, const char* i_enumValue){

  ecmdChipTarget myTarget;

  int rc = setupTarget(i_target, myTarget);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

  std::string buffer;

  buffer = i_enumValue;

  rc = ::putSpyEnum(myTarget, i_spyName, buffer);

  ecmdPerlInterfaceErrorCheck(rc);
  return rc;
}

void ecmdClientPerlapi::ecmdEnableRingCache(){

  return ;
}

int  ecmdClientPerlapi::ecmdDisableRingCache(){

  return 0;
}

int  ecmdClientPerlapi::ecmdFlushRingCache(){

  return 0;
}

int ecmdClientPerlapi::simaet(const char* i_function){

  int rc = 0;

  rc = ::simaet(i_function);

  ecmdPerlInterfaceErrorCheck(rc);
  return rc;

}

int ecmdClientPerlapi::simcheckpoint(const char* i_checkpoint){

  int rc = 0;
  rc = ::simcheckpoint(i_checkpoint);
  ecmdPerlInterfaceErrorCheck(rc);
  return rc;

}

int ecmdClientPerlapi::simclock(int i_cycles){

  int rc = 0;
  rc = ::simclock(i_cycles);
  ecmdPerlInterfaceErrorCheck(rc);
  return rc;

}

int ecmdClientPerlapi::simecho(const char* i_message){

  int rc = 0;
  rc = ::simecho(i_message);
  ecmdPerlInterfaceErrorCheck(rc);
  return rc;

}

int ecmdClientPerlapi::simexit(uint32_t i_rc , const char* i_message ){

  int rc = 0;
  rc = ::simexit(i_rc, i_message);
  ecmdPerlInterfaceErrorCheck(rc);
  return rc;

}

int ecmdClientPerlapi::simEXPECTFAC(const char* i_facname, int i_bitlength, const char* i_expect, int i_row, int i_offset){

  int rc = 0;
  ecmdDataBuffer buffer;

  buffer.setBitLength(strlen(i_expect));
  rc = buffer.insertFromBin(i_expect);

  rc = ::simEXPECTFAC(i_facname, i_bitlength, buffer, i_row, i_offset);
  ecmdPerlInterfaceErrorCheck(rc);
  return rc;

}

int ecmdClientPerlapi::simexpecttcfac(const char* i_tcfacname, int i_bitlength, const char* i_expect, int i_row){

  int rc = 0;
  ecmdDataBuffer buffer;

  buffer.setBitLength(strlen(i_expect));
  rc = buffer.insertFromBin(i_expect);
  rc = ::simexpecttcfac(i_tcfacname, i_bitlength, buffer, i_row);
  ecmdPerlInterfaceErrorCheck(rc);
  return rc;

}

int ecmdClientPerlapi::simgetcurrentcycle(uint32_t & o_cyclecount){

  int rc = 0;
  rc = ::simgetcurrentcycle(o_cyclecount);
  ecmdPerlInterfaceErrorCheck(rc);
  return rc;

}

int ecmdClientPerlapi::simGETFAC(const char* i_facname, int i_bitlength, char** o_data, int i_row, int i_offset){

  int rc = 0;
  ecmdDataBuffer buffer;
  rc = ::simGETFAC(i_facname, i_bitlength, buffer, i_row, i_offset);
  ecmdPerlInterfaceErrorCheck(rc);

  if (rc) {
    o_data = NULL;
    return rc;
  }

  char* tmp;
  tmp = new char[buffer.getBitLength()+1];
  strcpy(tmp,buffer.genBinStr().c_str());
  *o_data = tmp;

  return rc;

}

int ecmdClientPerlapi::simGETFACX(const char* i_facname, int i_bitlength, char** o_data, int i_row, int i_offset){

  int rc = 0;
  ecmdDataBuffer buffer;
  rc = ::simGETFACX(i_facname, i_bitlength, buffer, i_row, i_offset);
  ecmdPerlInterfaceErrorCheck(rc);

  if (rc) {
    o_data = NULL;
    return rc;
  }

  char* tmp;
  tmp = new char[buffer.getBitLength()+1];
  strcpy(tmp,buffer.genBinStr().c_str());
  *o_data = tmp;

  return rc;

}

int ecmdClientPerlapi::simgettcfac(const char* i_tcfacname, char** o_data, int i_row, int i_startbit, int i_bitlength){

  int rc = 0;
  ecmdDataBuffer buffer;
  rc = ::simgettcfac(i_tcfacname, buffer, i_row, i_startbit, i_bitlength);
  ecmdPerlInterfaceErrorCheck(rc);

  if (rc) {
    o_data = NULL;
    return rc;
  }

  char* tmp;
  tmp = new char[buffer.getBitLength()+1];
  strcpy(tmp,buffer.genBinStr().c_str());
  *o_data = tmp;

  return rc;

}

int ecmdClientPerlapi::siminit(const char* i_checkpoint){

  int rc = 0;
  rc = ::siminit(i_checkpoint);
  ecmdPerlInterfaceErrorCheck(rc);
  return rc;

}

int ecmdClientPerlapi::simPOLLFAC(const char* i_facname, int i_bitlength, const char* i_expect, int i_row, int i_offset, int i_maxcycles, int i_pollinterval) {

  int rc = 0;
  ecmdDataBuffer buffer;

  buffer.setBitLength(strlen(i_expect));
  rc = buffer.insertFromBin(i_expect);

  rc = ::simPOLLFAC(i_facname, i_bitlength, buffer, i_row, i_offset, i_maxcycles, i_pollinterval);
  ecmdPerlInterfaceErrorCheck(rc);
  return rc;

}

int ecmdClientPerlapi::simpolltcfac(const char* i_tcfacname, const char* i_expect, int i_row, int i_startbit, int i_bitlength, int i_maxcycles, int i_pollinterval) {

  int rc = 0;
  ecmdDataBuffer buffer;

  buffer.setBitLength(strlen(i_expect));
  rc = buffer.insertFromBin(i_expect);

  rc = ::simpolltcfac(i_tcfacname, buffer, i_row, i_startbit, i_bitlength, i_maxcycles, i_pollinterval);
  ecmdPerlInterfaceErrorCheck(rc);
  return rc;
}


int ecmdClientPerlapi::simPUTFAC(const char* i_facname, int i_bitlength, const char* i_data, int i_row, int i_offset){

  int rc = 0;

  ecmdDataBuffer buffer;

  buffer.setBitLength(strlen(i_data));
  rc = buffer.insertFromBin(i_data);


  rc = ::simPUTFAC(i_facname, i_bitlength, buffer, i_row, i_offset);
  ecmdPerlInterfaceErrorCheck(rc);

  return rc;

}

int ecmdClientPerlapi::simPUTFACX(const char* i_facname, int i_bitlength, const char* i_data, int i_row, int i_offset){

  int rc = 0;
  ecmdDataBuffer buffer;

  buffer.setBitLength(strlen(i_data));
  rc = buffer.insertFromBin(i_data);

  rc = ::simPUTFACX(i_facname, i_bitlength, buffer, i_row, i_offset);
  ecmdPerlInterfaceErrorCheck(rc);
  return rc;

}

int ecmdClientPerlapi::simputtcfac(const char* i_tcfacname, int i_bitlength, const char* i_data, int i_row, int i_numrows){

  int rc = 0;
  ecmdDataBuffer buffer;

  buffer.setBitLength(strlen(i_data));
  rc = buffer.insertFromBin(i_data);
  rc = ::simputtcfac(i_tcfacname, i_bitlength, buffer, i_row, i_numrows);
  ecmdPerlInterfaceErrorCheck(rc);
  return rc;

}

int ecmdClientPerlapi::simrestart(const char* i_checkpoint){

  int rc = 0;
  rc = ::simrestart(i_checkpoint);
  ecmdPerlInterfaceErrorCheck(rc);
  return rc;

}

int ecmdClientPerlapi::simSTKFAC(const char* i_facname, int i_bitlength, const char* i_data, int i_row, int i_offset){

  int rc = 0;
  ecmdDataBuffer buffer;

  buffer.setBitLength(strlen(i_data));
  rc = buffer.insertFromBin(i_data);
  rc = ::simSTKFAC(i_facname, i_bitlength, buffer, i_row, i_offset);
  ecmdPerlInterfaceErrorCheck(rc);
  return rc;

}

int ecmdClientPerlapi::simstktcfac(const char* i_tcfacname, int i_bitlength, const char* i_data, int i_row, int i_numrows){

  int rc = 0;
  ecmdDataBuffer buffer;

  buffer.setBitLength(strlen(i_data));
  rc = buffer.insertFromBin(i_data);
  rc = ::simstktcfac(i_tcfacname, i_bitlength, buffer, i_row, i_numrows);
  ecmdPerlInterfaceErrorCheck(rc);
  return rc;

}

int ecmdClientPerlapi::simSUBCMD(const char* i_command){

  int rc = 0;
  rc = ::simSUBCMD(i_command);
  ecmdPerlInterfaceErrorCheck(rc);
  return rc;

}

int ecmdClientPerlapi::simtckinterval(int i_tckinterval){

  int rc = 0;
  rc = ::simtckinterval(i_tckinterval);
  ecmdPerlInterfaceErrorCheck(rc);
  return rc;

}

int ecmdClientPerlapi::simUNSTICK(const char* i_facname, int i_bitlength, int i_row, int i_offset){

  int rc = 0;
  rc = ::simUNSTICK(i_facname, i_bitlength, i_row, i_offset);
  ecmdPerlInterfaceErrorCheck(rc);
  return rc;

}

int ecmdClientPerlapi::simunsticktcfac(const char* i_tcfacname, int i_bitlength, const char* i_data, int i_row, int i_numrows){

  int rc = 0;
  ecmdDataBuffer buffer;

  buffer.setBitLength(strlen(i_data));
  rc = buffer.insertFromBin(i_data);
  rc = ::simunsticktcfac(i_tcfacname, i_bitlength, buffer, i_row, i_numrows);
  ecmdPerlInterfaceErrorCheck(rc);
  return rc;

}

char* ecmdClientPerlapi::ecmdGetErrorMsg(int i_errorCode){

  return NULL;
}

void ecmdClientPerlapi::ecmdEnableSafeMode() {
  safeMode = 1;
}

void ecmdClientPerlapi::ecmdDisableSafeMode() {
  safeMode = 0;
}

int ecmdQuerySafeMode() {
  return (int) safeMode;
}


void ecmdClientPerlapi::ecmdOutputError(const char* i_message){
  ::ecmdOutputError(i_message);
  return ;
}

void ecmdClientPerlapi::ecmdOutputWarning(const char* i_message){
  ::ecmdOutputWarning(i_message);
  return ;
}

void ecmdClientPerlapi::ecmdOutput(const char* i_message){
  ::ecmdOutput(i_message);
  return ;
}



