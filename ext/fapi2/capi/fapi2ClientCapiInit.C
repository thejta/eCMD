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

#include <fapi2ClientCapi.H>
#ifndef ECMD_STATIC_FUNCTIONS
#include <fapi2ClientEnums.H>
#else
#include <fapi2DllCapi.H>
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
void * fapi2DllFnTable[FAPI2_NUMFUNCTIONS];
extern void * DllFnTable[];
#endif

/* Our initialization flag */
 bool fapi2Initialized = false;

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------
uint32_t fapi2InitExtension() {

  int rc = ECMD_SUCCESS;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"fapi2InitExtension: eCMD FAPI Extension Initialization function called before DLL has been loaded\n");
    exit(ECMD_DLL_INVALID);
  }
#endif

  /* Only do this if it hasn't been done already */
  if (fapi2Initialized) {
    return ECMD_SUCCESS;
  }

#ifndef ECMD_STATIC_FUNCTIONS
  /* look for init function */
  uint32_t (*Function)(const char *) =
    (uint32_t(*)(const char *))(void*)dlsym(dlHandle, "dllFapi2InitExtension");
  if (!Function) {
    /* This extension is not supported by this plugin */
    rc = ECMD_EXTENSION_NOT_SUPPORTED;
  } else {
    rc = (*Function)(ECMD_FAPI2_CAPI_VERSION);
    if (!rc) fapi2Initialized = true;
  }
  
  /* Clear out the function table */
  for (int func = 0; func < FAPI2_NUMFUNCTIONS; func ++) {
    fapi2DllFnTable[func] = NULL;
  }
#else

  rc = dllFapi2InitExtension(ECMD_FAPI2_CAPI_VERSION);
  if (!rc) fapi2Initialized = true;

#endif /* ECMD_STATIC_FUNCTIONS */

  /* Now as part of defect 18081 we register to the core client that we have been initialized */
  ecmdRegisterExtensionInitState(&fapi2Initialized);

  if (rc) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(rc, false, false, false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;
}
