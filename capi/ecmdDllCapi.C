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

int dllLoadDll (const char* clientVersion) {

  /* First off let's check our version */
  if (strcmp(clientVersion,ECMD_CAPI_VERSION)) {
    fprintf(stderr,"**** FATAL : eCMD DLL and your client are not compatible\n");
    fprintf(stderr,"**** FATAL : Client Version : %s   : DLL Version : %s\n",clientVersion, ECMD_CAPI_VERSION);
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

  return 0;
}


string dllGetErrorMsg(int errorCode) {
  string ret = "UNKNOWN";

  return ret;
}

int dllRegisterErrorMsg(int errorCode, const char* whom, const char* message) {
  int rc = ECMD_SUCCESS;

  return rc;
}



// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
