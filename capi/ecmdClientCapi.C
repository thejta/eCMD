// Copyright ***********************************************************
//                                                                      
// File ecmdClientCapi.C                                   
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

/* $Header$ */

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <dlfcn.h>
#include <string>
#include <stdio.h>

#include <ecmdClientCapi.H>
#include <ecmdDllCapi.H>

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
/* These are from ecmdClientCapiFunc.C */
extern void * dlHandle;
#endif

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************



int ecmdLoadDll(std::string i_dllName) {


  const char* dlError;
  int rc = ECMD_SUCCESS;

#ifndef ECMD_STATIC_FUNCTIONS
#ifdef _AIX
  /* clean up the machine from previous tests */
  system("slibclean");
#endif

  /* --------------------- */
  /* load DLL              */
  /* --------------------- */
  if (i_dllName.size() == 0) {
    /* Let's try to get it from the env var */
    char * tempptr = getenv("ECMD_DLL_FILE");  /* is there a ECMD_DLL_FILE environment variable? */
    if (tempptr != NULL) {
      i_dllName = tempptr;
    } else {
      fprintf(stderr,"ecmdLoadDll: Unable to find DLL to load, you must set ECMD_DLL_FILE\n");
      return ECMD_INVALID_DLL_FILENAME;
    }
  }

  printf("loadDll: loading %s ...\n", i_dllName.c_str()); 
  dlHandle = dlopen(i_dllName.c_str(), RTLD_LAZY);
  if (!dlHandle) {
    if ((dlError = dlerror()) != NULL) {
      printf("ERROR: loadDll: Problems loading '%s' : %s\n", i_dllName.c_str(), dlError);
      return ECMD_DLL_LOAD_FAILURE;
    }
  } else {
    printf("loadDll: load successful\n");
  }

  /* Now we need to call loadDll on the dll itself so it can initialize */

  int (*Function)(const char *) = 
      (int(*)(const char *))(void*)dlsym(dlHandle, "dllLoadDll");
  if (!Function) {
    fprintf(stderr,"ecmdLoadDll: Unable to find LoadDll function, must be an invalid DLL\n");
    rc = ECMD_DLL_LOAD_FAILURE;
  } else {
    rc = (*Function)(ECMD_CAPI_VERSION);
  }

#else
  rc = dllLoadDll(ECMD_CAPI_VERSION);

#endif /* ECMD_STATIC_FUNCTIONS */

  return rc;
}

int ecmdUnloadDll() {

  int rc = ECMD_SUCCESS;
  int c_rc = ECMD_SUCCESS;

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllUnloadDll();

#else

  if (dlHandle) {
    /* call DLL unload */
    int (*Function)() =
      (int(*)())(void*)dlsym(dlHandle, "dllUnloadDll");
    if (!Function) {
      fprintf(stderr,"ecmdUnloadDll: Unable to find UnloadDll function, must be an invalid DLL\n");
      rc = ECMD_DLL_UNLOAD_FAILURE;
      return rc;
    }
    rc = (*Function)();


    /* release DLL */
    const char* dlError;

    c_rc = dlclose(dlHandle);
    if (c_rc) {
      if ((dlError = dlerror()) != NULL) {
        fprintf(stderr,"ERROR: ecmdUnloadDll: %s\n", dlError);
        return ECMD_DLL_UNLOAD_FAILURE;
      }
    }
  }
  dlHandle = NULL;
#endif

  return rc;
}

int ecmdCommandArgs(int* i_argc, char** i_argv[]) {

  int rc = ECMD_SUCCESS;

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllCommonCommandArgs(i_argc, i_argv);

#else

  /* call DLL unload */
  int (*Function)(int*, char***) =
    (int(*)(int*, char***))(void*)dlsym(dlHandle, "dllCommonCommandArgs");
  if (!Function) {
    fprintf(stderr,"ecmdCommandArgs: Unable to find dllCommonCommandArgs function, must be an invalid DLL\n");
    rc = ECMD_DLL_INVALID;
    return rc;
  }
  rc = (*Function)(i_argc, i_argv);
  
#endif

  return rc;
}

