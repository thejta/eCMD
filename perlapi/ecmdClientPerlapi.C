
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
  perlFormat = "b";

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



int ecmdClientPerlapi::getScom (const char* i_target, int i_address, char** o_data) {

  ecmdChipTarget myTarget;
  std::string dataStr;

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


  dataStr = buffer.genBinStr();

  char* tmp;
  tmp = new char[dataStr.length()+1];
  strcpy(tmp,dataStr.c_str());
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
  std::string dataStr;

  rc = setupTarget(i_target, myTarget);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

  rc = ::getRing(myTarget, i_ringName, buffer);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;


  dataStr = buffer.genBinStr();

  char* tmp;
  tmp = new char[dataStr.length()+1];
  strcpy(tmp,dataStr.c_str());
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


int ecmdClientPerlapi::ecmdCommandArgs(char** i_argv[]){

  return 0;
}

/***
int ecmdClientPerlapi::ecmdCommandArgs(int* i_argc, char** i_argv[]) {

  return 0;
}

***/
int ecmdClientPerlapi::sendCmd(const char* i_target, int i_instruction, int i_modifier, char** o_status) {

  return 0;
}


int ecmdClientPerlapi::getCfamRegister (const char* i_target, int i_address, char** o_data){

  return 0;
}

int ecmdClientPerlapi::putCfamRegister (const char* i_target, int i_address, const char* i_data){

  return 0;
}

int ecmdClientPerlapi::getSpy (const char* i_target, const char * i_spyName, char** o_data){

  return 0;
}

int ecmdClientPerlapi::getSpyEnum (const char* i_target, const char * i_spyName, char** o_enumValue){

  return 0;
}

int ecmdClientPerlapi::getSpyEccGrouping (const char* i_target, const char * i_spyEccGroupName, char** o_groupData, char** o_eccData, char** o_eccErrorMask){

  return 0;
}


int ecmdClientPerlapi::putSpy (const char* i_target, const char * i_spyName, const char* i_data){

  return 0;
}

int ecmdClientPerlapi::putSpyEnum (const char* i_target, const char * i_spyName, const char* i_enumValue){

  return 0;
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

int ecmdClientPerlapi::getArray (const char* i_target, const char * i_arrayName, const char* i_address, char** o_data){

  return 0;
}

int ecmdClientPerlapi::putArray (const char* i_target, const char * i_arrayName, const char* i_address, const char* i_data){

  return 0;
}

int ecmdClientPerlapi::simaet(const char* i_function){

  return 0;
}

int ecmdClientPerlapi::simcheckpoint(const char* i_checkpoint){

  return 0;
}

int ecmdClientPerlapi::simclock(int i_cycles){

  return 0;
}

int ecmdClientPerlapi::simecho(const char* i_message){

  return 0;
}

int ecmdClientPerlapi::simexit(){

  return 0;
}

int ecmdClientPerlapi::simEXPECTFAC(const char* i_facname, int i_bitlength, const char* i_expect, int i_row, int i_offset){

  return 0;
}

int ecmdClientPerlapi::simexpecttcfac(const char* i_tcfacname, int i_bitlength, const char* i_expect, int i_row){

  return 0;
}

int ecmdClientPerlapi::simgetcurrentcycle(char** o_cyclecount){

  return 0;
}

int ecmdClientPerlapi::simGETFAC(const char* i_facname, int i_bitlength, char** o_data, int i_row, int i_offset){

  return 0;
}

int ecmdClientPerlapi::simGETFACX(const char* i_facname, int i_bitlength, char** o_data, int i_row, int i_offset){

  return 0;
}

int ecmdClientPerlapi::simgettcfac(const char* i_tcfacname, char** o_data, int i_row, int i_startbit, int i_bitlength){

  return 0;
}

int ecmdClientPerlapi::siminit(const char* i_checkpoint){

  return 0;
}

int ecmdClientPerlapi::simPUTFAC(const char* i_facname, int i_bitlength, const char* i_data, int i_row, int i_offset){

  return 0;
}

int ecmdClientPerlapi::simPUTFACX(const char* i_facname, int i_bitlength, const char* i_data, int i_row, int i_offset){

  return 0;
}

int ecmdClientPerlapi::simputtcfac(const char* i_tcfacname, int i_bitlength, const char* i_data, int i_row, int i_numrows){

  return 0;
}

int ecmdClientPerlapi::simrestart(const char* i_checkpoint){

  return 0;
}

int ecmdClientPerlapi::simSTKFAC(const char* i_facname, int i_bitlength, const char* i_data, int i_row, int i_offset){

  return 0;
}

int ecmdClientPerlapi::simstktcfac(const char* i_tcfacname, int i_bitlength, const char* i_data, int i_row, int i_numrows){

  return 0;
}

int ecmdClientPerlapi::simSUBCMD(const char* i_command){

  return 0;
}

int ecmdClientPerlapi::simtckinterval(int i_tckinterval){

  return 0;
}

int ecmdClientPerlapi::simUNSTICK(const char* i_facname, int i_bitlength, int i_row, int i_offset){

  return 0;
}

int ecmdClientPerlapi::simunsticktcfac(const char* i_tcfacname, int i_bitlength, const char* i_data, int i_row, int i_numrows){

  return 0;
}

char* ecmdClientPerlapi::ecmdGetErrorMsg(int i_errorCode){

  return NULL;
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



