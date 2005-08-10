/* NOTE : This file has been generated from the eCMD extension temp-late */
/* DO NOT EDIT THISFILE (unless you are editing the temp-late */






// Copyright ***********************************************************
//                                                                      
// File templateClientCapi.C                                   
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
/* $Header$ */
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
#include <ecmdUtils.H>

#include <templateClientCapi.H>
#include <templateClientEnums.H>

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
/* These are from templateClientCapiFunc.C */
extern void * templateDllFnTable[];
#endif
extern bool templateInitialized;

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------
uint32_t templateInitExtension() {

  int rc = ECMD_SUCCESS;
#ifdef ECMD_STATIC_FUNCTIONS
  /* Nothing to see here */
  /* We don't have to worry about initialization because their client won't compile unless the extension is supported */
  templateInitialized = true;

#else

  if (dlHandle == NULL) {
    fprintf(stderr,"templateInitExtension: eCMD TEMPLATE Extension Initialization function called before DLL has been loaded\n");
    exit(ECMD_DLL_INVALID);
  }

  if (!templateInitialized) {
    /* look for init function */
    uint32_t (*Function)(const char *) =
      (uint32_t(*)(const char *))(void*)dlsym(dlHandle, "dllTemplateInitExtension");
    if (!Function) {
      /* This extension is not supported by this plugin */
      rc = ECMD_EXTENSION_NOT_SUPPORTED;
    } else {
      rc = (*Function)(ECMD_TEMPLATE_CAPI_VERSION);
      if (!rc) templateInitialized = true;
    }

    /* Clear out the function table */
    for (int func = 0; func < TEMPLATE_NUMFUNCTIONS; func ++) {
      templateDllFnTable[func] = NULL;
    }

  }
  
#endif

  /* Now as part of defect 18081 we register to the core client that we have been initialized */
  ecmdRegisterExtensionInitState(&templateInitialized);

  return rc;

}



// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
