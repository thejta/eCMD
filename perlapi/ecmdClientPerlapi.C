
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

  myFormat = "pxr";

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




char * ecmdClientPerlapi::getScom (const char * i_target, int i_address) {

  int rc = setupTarget(i_target, myTarget);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return NULL;

  ecmdDataBuffer buffer;
  rc = ::getScom(myTarget, i_address, buffer);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return NULL;

  dataStr = ecmdWriteDataFormatted(buffer, myFormat);
  return (char*)(dataStr.c_str());
}

int ecmdClientPerlapi::putScom (const char * i_target, int i_address, const char * i_data) {

  int rc = setupTarget(i_target, myTarget);
  if (rc) return rc;

  ecmdDataBuffer buffer; 
  rc = ecmdReadDataFormatted(buffer, i_data, myFormat);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

  rc = ::putScom(myTarget, i_address, buffer);

  ecmdPerlInterfaceErrorCheck(rc);
  return rc;
}


 
/*char* ecmdClientPerlapi::getRing (const char * i_target, const char * i_ringName) {*/
int ecmdClientPerlapi::getRing (const char * i_target, const char * i_ringName, char **o_data) {

  char* tmp;
printf("o_data %.08x : *o_data %.08X\n",o_data, *o_data);

  printf("comming into c code o_data = %s.\n",*o_data);
  printf("in the c code printing o_data = %08X\n",*o_data);

  int rc = setupTarget(i_target, myTarget);
/*  if (rc) return NULL; */
  if (rc) return rc;

  int flushrc = ecmdFlushRingCache();
  printf("just flushed\n");
  if (flushrc) {
/*     return NULL;*/
     return flushrc;
  }

  ecmdDataBuffer buffer;
  printf("made my buffer\n");

  rc = ::getRing(myTarget, i_ringName, buffer);
  printf("out of getring\n");
  ecmdPerlInterfaceErrorCheck(rc);
  printf("after errorCheck\n");

/*  if (rc) return NULL;*/
  if (rc) return rc;

  dataStr = ecmdWriteDataFormatted(buffer, myFormat);
  printf("after writeDataFormatted\n");

  tmp = new char[dataStr.length()+1];
  strcpy(tmp,dataStr.c_str());
  *o_data = tmp;

  printf("in the c code printing tmp = %s\n",tmp);
  printf("in the c code printing o_data = %s\n",*o_data);

  printf("in the c code printing o_data = %08X\n",*o_data);
printf("o_data %.08x : *o_data %.08X\n",o_data, *o_data);
printf("tmp %.08X : &tmp %.08X\n",tmp, &tmp);
/*  return (char*)(dataStr.c_str());*/
//  return rc;
  return 10;


}


int ecmdClientPerlapi::putRing (const char * i_target, const char * i_ringName, const char * i_data) {

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

  ecmdCommandArgs(&numArgs, &args);
  rc = ecmdConfigLooperInit(o_target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

  ecmdConfigLooperNext(o_target, looperdata);

  if (args != NULL) {
    delete[] args;
  }

  return rc;
}
