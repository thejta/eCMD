
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
#include <cipClientPerlapi.H>
#include <cipClientCapi.H>
#include <ecmdSharedUtils.H>


//extern int myErrorCode = ECMD_SUCCESS;
//extern int safeMode = 1;
int setupTarget(const char * i_targetStr, ecmdChipTarget & o_target);
int ecmdPerlInterfaceErrorCheck (int errorCode);

cipClientPerlapi::cipClientPerlapi () {
}

cipClientPerlapi::~cipClientPerlapi () {
}


int cipClientPerlapi::cipInitExtension() {
  return ::cipInitExtension();
}


int cipClientPerlapi::cipStartInstructions (const char* i_target) {
  int rc = 0;
  return rc;
}


int cipClientPerlapi::cipStartAllInstructions (){
  int rc = 0;
  return rc;
}

int cipClientPerlapi::cipStopInstructions (const char* i_target){
  int rc = 0;
  return rc;
}

int cipClientPerlapi::cipStopAllInstructions (){
  int rc = 0;
  return rc;
}

int cipClientPerlapi::cipStepInstructions (const char* i_target, int i_steps){
  int rc = 0;
  return rc;
}

int cipClientPerlapi::cipGetVr (const char* i_target, int i_vrNum, char** o_data) {
  ecmdChipTarget myTarget;

  int rc = setupTarget(i_target, myTarget);
  ecmdPerlInterfaceErrorCheck(rc);
  if (rc) {
    *o_data = NULL;
    return rc;
  }

  ecmdDataBuffer buffer;
  rc = ::cipGetVr(myTarget, i_vrNum, buffer);
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

int cipClientPerlapi::cipPutVr (const char* i_target, int i_vrNum, const char* i_data){
  ecmdChipTarget myTarget;
  
  int rc = setupTarget(i_target, myTarget);
  if (rc) return rc;

  ecmdDataBuffer buffer;

  buffer.setBitLength(strlen(i_data));
  rc = buffer.insertFromBin(i_data);

  rc = ::cipPutVr(myTarget, i_vrNum, buffer);

  ecmdPerlInterfaceErrorCheck(rc);
  return rc;
}

