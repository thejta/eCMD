
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
#include <ecmdClientCapi.H>
#include <ecmdDataBuffer.H>
#include <ecmdReturnCodes.H>
#include <ecmdUtils.H>
#include <string.h>
#include <ecmdClientPerlapi.H>
#include <stdio.h>


static int myErrorCode = ECMD_SUCCESS;

int ecmdPerlInterfaceErrorCheck (int errorCode) {

  if (errorCode == -1) {

    if (myErrorCode != ECMD_SUCCESS) {
      ecmdOutputError( (ecmdGetErrorMsg(myErrorCode) + "\n").c_str());
    }

    return myErrorCode;
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



int ecmdClientPerlapi::getScom (const char* i_target, int i_address, char** o_data) {

/* char * ecmdClientPerlapi::getScom (const char * i_target, int i_address) { */
  ecmdChipTarget myTarget;
  std::string dataStr;
  std::string myFormat;


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

  myFormat = "b";

  dataStr = ecmdWriteDataFormatted(buffer, myFormat);

  char* tmp;
  tmp = new char[dataStr.length()+1];
  strcpy(tmp,dataStr.c_str());
  *o_data = tmp;

/*  o_data = dataStr; */

/*  return (char*)(dataStr.c_str());*/
  return rc;
}



int ecmdClientPerlapi::putScom (const char * i_target, int i_address, const char * i_data) {

  ecmdChipTarget myTarget;
  std::string dataStr;
  std::string myFormat;

  int rc = setupTarget(i_target, myTarget);
  if (rc) return rc;

  ecmdDataBuffer buffer;

  myFormat = "b";
  rc = ecmdReadDataFormatted(buffer, i_data, myFormat);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

  rc = ::putScom(myTarget, i_address, buffer);

  ecmdPerlInterfaceErrorCheck(rc);
  return rc;
}

 
 
int ecmdClientPerlapi::getRing (const char * i_target, const char * i_ringName, char **o_data) {

  char* tmp;
  std::string dataStr;
  std::string::size_type dataStrLen;
  std::string myFormat;
  ecmdChipTarget myTarget;

  myFormat = "b";

  printf("o_data %.08x : *o_data %.08X\n",o_data, *o_data);
  if(o_data == NULL) {
    printf("o_data is NULL\n");

  }

  printf("coming into c code o_data = %s.\n",*o_data);
  printf("in the c code printing o_data = %08X\n",*o_data);

  int rc = setupTarget(i_target, myTarget);

  if (rc) return rc;


  int flushrc = ecmdFlushRingCache();
  printf("just flushed\n");
  if (flushrc) {
   return flushrc;
  }

  ecmdDataBuffer buffer;
  printf("made my buffer\n");

  rc = ::getRing(myTarget, i_ringName, buffer);
  printf("out of getring\n");
  ecmdPerlInterfaceErrorCheck(rc);
  printf("after errorCheck\n");

  printf("This is buffer size is from 0 to 64 :%d\n", buffer.getBitLength()); 
 
  if (rc) return rc;

  printf("yo yo  \n");

  dataStr = ecmdWriteDataFormatted(buffer, myFormat);
  printf("after writeDataFormatted\n");
  dataStrLen = dataStr.length();
  printf("dataStr.length = %d\n",dataStrLen);
  printf("dataStr = %s\n",dataStr.c_str());

  tmp = new char[dataStrLen+1];
  printf("after new command\n");
  strcpy(tmp,dataStr.c_str());
  printf("after strcpy command\n");
  printf("in the c code printing o_data 08X = %08X\n",o_data);
  o_data = &tmp; 

  printf("in the c code printing tmp = %s\n",tmp);
  printf("in the c code printing o_data = %s\n",*o_data);
  printf("in the c code printing o_data 08X *o_data= %08X\n",*o_data);

  printf("o_data %.08x : *o_data %.08X\n",o_data, *o_data);
  printf("tmp %.08X : &tmp %.08X\n",tmp, &tmp);
  return rc;
}


int ecmdClientPerlapi::putRing (const char * i_target, const char * i_ringName, const char * i_data) {

  ecmdChipTarget myTarget;
  std::string dataStr;
  std::string myFormat;
  myFormat = "b";

  int rc = setupTarget(i_target, myTarget);
  if (rc) return rc;

  ecmdDataBuffer buffer; 
  rc = ecmdReadDataFormatted(buffer, i_data, myFormat);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

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

  if (i_targetStr == NULL) {
    ecmdPerlInterfaceErrorCheck(ECMD_INVALID_ARGS);
    return ECMD_INVALID_ARGS;
  }

  char *my_i_targetStr = new char[sizeof strlen(i_targetStr)+1];
  int rc = ECMD_SUCCESS, i = 0;
  char * curArg;
  int longestArg = 0;

  strcpy(my_i_targetStr,i_targetStr);


  if (curArg = strtok(my_i_targetStr, " ")) {
    longestArg = strlen(curArg);
    o_target.chipType = curArg;
    o_target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  }

  int numArgs = 1;
  char ** args;

  while (curArg = strtok(NULL, " ")) {
    numArgs++;
  }

  args = new char*[numArgs];
  for (i = 0; i < numArgs; i++) {
    args[i] = new char[longestArg+1];
  }

  if (curArg = strtok(my_i_targetStr, " ")) {

    i = 0;
    while (curArg = strtok(NULL, " ")) {
      strcpy(args[i], curArg);
      i++;
    }

  }
  delete my_i_targetStr;

  ecmdLooperData looperdata;

/* have to figure out how to handle the argc for this.  pvl 11/17/04 */
  ecmdCommandArgs(&args);
/*  ecmdCommandArgs(&numArgs, &args);*/

  rc = ecmdConfigLooperInit(o_target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

  ecmdConfigLooperNext(o_target, looperdata);

  if (args != NULL) {
    delete[] args;
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

  return ;
}

void ecmdClientPerlapi::ecmdOutputWarning(const char* i_message){

  return ;
}

void ecmdClientPerlapi::ecmdOutput(const char* i_message){

  return ;
}



