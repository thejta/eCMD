/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File fapiClientCapi.C                                   
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

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <stdio.h>
#include <dlfcn.h>

#include <ecmdClientCapi.H>
#include <ecmdSharedUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientEnums.H>
#include <ecmdUtils.H>

#include <fapiClientCapi.H>
#ifndef ECMD_STATIC_FUNCTIONS
#include <fapiClientEnums.H>
#else
#include <fapiDllCapi.H>
#include <ecmdDllCapi.H>
#endif

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
/* These are from fapiClientCapiFunc.C */
void * fapiDllFnTable[FAPI_NUMFUNCTIONS];
extern void * DllFnTable[];
#endif

/* Our initialization flag */
 bool fapiInitialized = false;

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------
uint32_t fapiInitExtension() {

  int rc = ECMD_SUCCESS;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"fapiInitExtension: eCMD FAPI Extension Initialization function called before DLL has been loaded\n");
    exit(ECMD_DLL_INVALID);
  }
#endif

  /* Only do this if it hasn't been done already */
  if (fapiInitialized) {
    return ECMD_SUCCESS;
  }

#ifndef ECMD_STATIC_FUNCTIONS
  /* look for init function */
  uint32_t (*Function)(const char *) =
    (uint32_t(*)(const char *))(void*)dlsym(dlHandle, "dllFapiInitExtension");
  if (!Function) {
    /* This extension is not supported by this plugin */
    rc = ECMD_EXTENSION_NOT_SUPPORTED;
  } else {
    rc = (*Function)(ECMD_FAPI_CAPI_VERSION);
    if (!rc) fapiInitialized = true;
  }
  
  /* Clear out the function table */
  for (int func = 0; func < FAPI_NUMFUNCTIONS; func ++) {
    fapiDllFnTable[func] = NULL;
  }
#else

  rc = dllFapiInitExtension(ECMD_FAPI_CAPI_VERSION);
  if (!rc) fapiInitialized = true;

#endif /* ECMD_STATIC_FUNCTIONS */

  /* Now as part of defect 18081 we register to the core client that we have been initialized */
  ecmdRegisterExtensionInitState(&fapiInitialized);

  if (rc) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(rc, false, false, false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;
}
