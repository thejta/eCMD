
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

  ecmdOutput(i_data); ecmdOutput("\n");
  ecmdDataBuffer buffer; 
  rc = ecmdReadDataFormatted(buffer, i_data, myFormat);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

  ecmdOutput(buffer.genHexLeftStr().c_str()); ecmdOutput("\n");
  rc = ::putScom(myTarget, i_address, buffer);
  ecmdOutput(buffer.genHexLeftStr().c_str()); ecmdOutput("\n");

  ecmdPerlInterfaceErrorCheck(rc);
  return rc;
}



/* paul was here */


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
/*  rc = ecmdConfigLooperInit(o_target);*/
  rc = ecmdConfigLooperInit(o_target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) return rc;

  ecmdConfigLooperNext(o_target, looperdata);

  if (args != NULL) {
    delete[] args;
  }

  return rc;
}
