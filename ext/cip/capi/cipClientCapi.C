// Copyright ***********************************************************
//                                                                      
// File cipClientCapi.C                                   
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
#include <stdio.h>
#include <dlfcn.h>

#include <ecmdReturnCodes.H>
#include <cipClientCapi.H>

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
#ifndef ECMD_STATIC_FUNCTIONS
/* This is from ecmdClientCapiFunc.C */
extern void * dlHandle;
/* These are from cipClientCapiFunc.C */
extern void * cipDllFnTable[];
extern bool cipInitialized;
#endif

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------
uint32_t cipInitExtension() {

  int rc = ECMD_SUCCESS;
#ifdef ECMD_STATIC_FUNCTIONS
  /* Nothing to see here */
  /* We don't have to worry about initialization because their client won't compile unless the extension is supported */
  cipInitialized = true;

#else

  if (dlHandle == NULL) {
    fprintf(stderr,"cipInitExtension: eCMD CIP Extension Initialization function called before DLL has been loaded\n");
    exit(ECMD_DLL_INVALID);
  }

  if (!cipInitialized) {
    /* look for init function */
    uint32_t (*Function)(const char *) =
      (uint32_t(*)(const char *))(void*)dlsym(dlHandle, "dllCipInitExtension");
    if (!Function) {
      /* This extension is not supported by this plugin */
      rc = ECMD_EXTENSION_NOT_SUPPORTED;
    } else {
      rc = (*Function)(ECMD_CIP_CAPI_VERSION);
      if (!rc) cipInitialized = true;
    }

  }
  
#endif
  return rc;

}



// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
