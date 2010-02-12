/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File ecmdVersion.C                                   
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
/**
 @file ecmdVersion.C
 @brief Utility program to display just the eCMD Version in a string to use as a path for CTE
*/


//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <dlfcn.h>
#include <stdio.h>
#include <string>
#include <inttypes.h>

#include <ecmdReturnCodes.H>

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
/**
 @brief This is a backdoor mechanism to display the version of the current eCMD dll. DO NOT USE WILL CAUSE YOUR PROGRAM TO DIE
 @param options Options to change display mode
*/
uint32_t ecmdCheckDllVersion(const char* options);

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------

int main (int argc, char *argv[])
{
  uint32_t rc = 0;


  /* Fix for defect 18174, if we are running the eCMD dev release, we can't query the dll version we just have to return dev here to pick up all the proper supporting files, hopefully the dll will be happy with that, if not it will be found when the real dll is loaded */
#ifdef ECMD_DEV
  printf("dev");
#else 

  /* This is going to be real simple, we just call the dll and it does all the work */
  /* Pass in any args we may have, if argv[1] is null that is handled properly */
  rc = ecmdCheckDllVersion(argv[1]);
#endif

  return rc;
}


uint32_t ecmdCheckDllVersion(const char* options) {


  void * dlHandle = NULL;
  const char* dlError;
  uint32_t rc = ECMD_SUCCESS;
  std::string i_dllName;


  /* Only do this if it hasn't been done already */
  if (dlHandle != NULL) {
    fprintf(stderr,"ERROR: ecmdCheckDllVersion: This function cannot be called after ecmdLoadDll\n");
    fprintf(stderr,"ERROR: ecmdCheckDllVersion: Aborting execution!\n");
    abort();
  }

#ifdef _AIX
  /* clean up the machine from previous tests */
  system("slibclean");
#endif

  /* --------------------- */
  /* load DLL              */
  /* --------------------- */
  /* Let's try to get it from the env var */
  char * tempptr = getenv("ECMD_DLL_FILE");  /* is there a ECMD_DLL_FILE environment variable? */
  if (tempptr != NULL) {
    i_dllName = tempptr;
  } else {
    fprintf(stderr,"ERROR: ecmdCheckDllVersion: Unable to find DLL to load, you must set ECMD_DLL_FILE\n");
    return ECMD_INVALID_DLL_FILENAME;
  }


  dlHandle = dlopen(i_dllName.c_str(), RTLD_LAZY);
  if (!dlHandle) {
    if ((dlError = dlerror()) != NULL) {
      fprintf(stderr,"ERROR: ecmdCheckDllVersion: Problems loading '%s' : %s\n", i_dllName.c_str(), dlError);
      return ECMD_DLL_LOAD_FAILURE;
    }
  }

  /* Now we need to call loadDll on the dll itself so it can initialize */

  uint32_t (*Function)(const char *) = 
    (uint32_t(*)(const char *))(void*)dlsym(dlHandle, "dllCheckDllVersion");
  if (!Function) {
    fprintf(stderr,"ecmdCheckDllVersion: Unable to find dllCheckDllVersion function, must be an invalid DLL\n");
    rc = ECMD_DLL_LOAD_FAILURE;
  } else {
    rc = (*Function)(options);
  }

  if (dlHandle) dlclose(dlHandle);
  /* The dllCheckDllVersion does an exit so we will never get here */
  return rc;
}


// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
