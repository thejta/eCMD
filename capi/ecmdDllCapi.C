// Copyright ***********************************************************
//                                                                      
// File ecmdDllCapi.C                                   
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 1996                                         
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
#define ecmdDllCapi_C
#include <stdio.h>

#include "ecmdDllCapi.H"

#undef ecmdDllCapi_C
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

int dllLoadDll (const char* i_clientVersion) {

  /* First off let's check our version */
  if (strcmp(i_clientVersion,ECMD_CAPI_VERSION)) {
    fprintf(stderr,"**** FATAL : eCMD DLL and your client are not compatible\n");
    fprintf(stderr,"**** FATAL : Client Version : %s   : DLL Version : %s\n",i_clientVersion, ECMD_CAPI_VERSION);
/*
    if (version < DLL_VERSION)
      fprintf(stderr,"**** FATAL : You must rebuild your client to continue\n");
    else
      fprintf(stderr,"**** FATAL : Contact the Cronus team to have the DLL rebuilt to match your client to continue\n");
*/
    exit(999);
  }

  return dllInitDll();

}

int dllUnloadDll() {
  int rc = 0;
  
  rc = dllFreeDll();

  return rc;
}


std::string dllGetErrorMsg(int i_errorCode) {
  std::string ret = "UNKNOWN";

  return ret;
}

int dllRegisterErrorMsg(int i_errorCode, const i_char* whom, const char* i_message) {
  int rc = ECMD_SUCCESS;

  return rc;
}

int dllQuerySelected(ecmdChipTarget & i_target, std::vector<ecmdCageData> & o_queryData) {
  int rc = ECMD_SUCCESS;

  return rc;
}

int dllCommonCommandArgs(int*  io_argc, char** io_argv[]) {
  int rc = ECMD_SUCCESS;

  /* We need to pull out the targeting options here */

  /* Call the dllSpecificFunction */
  rc = dllSpecificCommandArgs(io_argc,io_argv);

  return rc;
}


// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
