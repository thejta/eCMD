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
#include <fapiHwAccess.H> 
#include <fapiSystemConfig.H>
#include <fapiReturnCode.H>
#include <fapiTarget.H>
#include <fapiSharedUtils.H>
#include <fapiStructs.H>
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

static const char ECMD_DLL_NOT_LOADED_ERROR[] = ": eCMD Function called before DLL has been loaded\n";
static const char ECMD_UNABLE_TO_FIND_FUNCTION_ERROR[] = ": Unable to find function, must be an invalid DLL - program aborting\n";


#ifndef ECMD_STATIC_FUNCTIONS
/* This is from ecmdClientCapiFunc.C */
extern void * dlHandle;
/* These are from fapiClientCapiFunc.C */
extern void * fapiDllFnTable[];
extern void * DllFnTable[];
#endif
extern bool fapiInitialized;

using namespace fapi;
#ifndef ECMD_STRIP_DEBUG
extern int ecmdClientDebug;
extern int fppCallCount;
extern bool ecmdDebugOutput;
#endif



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

uint32_t fapiHwpInvoker(ecmdChipTarget & i_target, const std::string & i_sharedObjectName, const std::string & i_sharedObjectEntryPoint, std::list<uint64_t> &i_sharedObjectArgs) {

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"fapiHwpInvoker%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_target);
     args.push_back((void*) &i_sharedObjectName);
     args.push_back((void*) &i_sharedObjectEntryPoint);
     args.push_back((void*) &i_sharedObjectArgs);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t fapiHwpInvoker(ecmdChipTarget & i_target, const std::string & i_sharedObjectName, const std::string & i_sharedObjectEntryPoint, std::list<uint64_t> &i_sharedObjectArgs)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapiHwpInvoker");
  }
#endif

   fapi::Target myFapiTarget;
   ecmdTargetToFapiTarget(i_target, myFapiTarget);
   void* handle = dlopen(i_sharedObjectName.c_str(), RTLD_NOW | RTLD_GLOBAL);
   if (!handle) {
     const char *dlopen_error = dlerror();
     printf("ERROR: fapiHwpInvoker: Problems opening '%s' : %s\n", i_sharedObjectName.c_str(), dlopen_error);
     return ECMD_FAILURE;
    }
    
   // load the symbol
   typedef fapi::ReturnCode (*fapi_ring_t)(fapi::Target, std::list<uint64_t>);
   // reset errors
   dlerror();
   fapi_ring_t func = (fapi_ring_t) dlsym(handle, i_sharedObjectEntryPoint.c_str());
   const char *dlsym_error = dlerror();
   if (dlsym_error) {
     printf("ERROR: fapiHwpInvoker::Cannot load symbol '%s': %s\n", i_sharedObjectEntryPoint.c_str(), dlsym_error);
     dlclose(handle);
     return ECMD_FAILURE;
   }
    
   // use it to do the calculation
   // printf("fapiHwpInvoker::Calling '%s'\n", i_sharedObjectEntryPoint.c_str());
   fapi::ReturnCode fapiRc = func( myFapiTarget, i_sharedObjectArgs);
    
   // close the library
   //printf("fapiHwpInvoker::Closing '%s'\n", i_sharedObjectName.c_str());
   dlclose(handle);

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &fapiRc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiHwpInvoker");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t fapiHwpInvoker(ecmdChipTarget & i_target, const std::string & i_sharedObjectName, const std::string & i_sharedObjectEntryPoint, std::list<uint64_t> &i_sharedObjectArgs)",args);
   }
#endif

  if (uint32_t(fapiRc) && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(uint32_t(fapiRc), false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return uint32_t(fapiRc);
}



uint32_t fapiQueryFileLocation(fapi::FileType_t i_fileType, std::string & i_fileName,  std::string & o_fileLocation, std::string i_version) {

  uint32_t rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiQueryFileLocation%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_fileType);
     args.push_back((void*) &i_fileName);
     args.push_back((void*) &o_fileLocation);
     args.push_back((void*) &i_version);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t fapiQueryFileLocation(fapi::FileType_t i_fileType, std::string & i_fileName, std::string & o_fileLocation, std::string i_version)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapiQueryFileLocation");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapiQueryFileLocation(i_fileType, i_fileName, o_fileLocation, i_version);
#else
  if (fapiDllFnTable[ECMD_FAPIQUERYFILELOCATION] == NULL) {
     fapiDllFnTable[ECMD_FAPIQUERYFILELOCATION] = (void*)dlsym(dlHandle, "dllFapiQueryFileLocation");
     if (fapiDllFnTable[ECMD_FAPIQUERYFILELOCATION] == NULL) {
       fprintf(stderr,"dllFapiQueryFileLocation%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)(fapi::FileType_t,  std::string &, std::string &,  std::string) = 
      (uint32_t(*)(fapi::FileType_t,  std::string &, std::string &,  std::string))fapiDllFnTable[ECMD_FAPIQUERYFILELOCATION];
  rc =    (*Function)(i_fileType, i_fileName, o_fileLocation, i_version);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiQueryFileLocation");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t fapiQueryFileLocation(fapi::FileType_t i_fileType, std::string & i_fileName, std::string & o_fileLocation, std::string i_version)",args);
   }
#endif

  if (rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;
}

uint32_t fapiGetAttribute(const fapi::Target & i_target, const uint32_t i_id, fapi::AttributeData & o_data){

  uint32_t rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiGetAttribute%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_target);
     args.push_back((void*) &i_id);
     args.push_back((void*) &o_data);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t fapiGetAttribute(const fapi::Target & i_target, const uint32_t i_id, fapi::AttributeData & o_data)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapiGetAttribute");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapiGetAttribute(i_target, i_id, o_data);
#else
  if (fapiDllFnTable[ECMD_FAPIGETATTRIBUTE] == NULL) {
     fapiDllFnTable[ECMD_FAPIGETATTRIBUTE] = (void*)dlsym(dlHandle, "dllFapiGetAttribute");
     if (fapiDllFnTable[ECMD_FAPIGETATTRIBUTE] == NULL) {
       fprintf(stderr,"dllFapiGetAttribute%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)(const fapi::Target &, const uint32_t,  fapi::AttributeData &) = 
      (uint32_t(*)(const fapi::Target &, const uint32_t,  fapi::AttributeData &))fapiDllFnTable[ECMD_FAPIGETATTRIBUTE];
  rc =    (*Function)(i_target, i_id, o_data);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiGetAttribute");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t fapiGetAttribute(const fapi::Target & i_target, const uint32_t i_id, fapi::AttributeData & o_data)",args);
   }
#endif

  if (rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;
}

uint32_t fapiSetAttribute(const fapi::Target & i_target, const uint32_t i_id, fapi::AttributeData & i_data){

  uint32_t rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiSetAttribute%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_target);
     args.push_back((void*) &i_id);
     args.push_back((void*) &i_data);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t ecmdGetConfigurationComplex(const fapi::Target & i_target, const uint32_t i_id, ecmdConfigData & i_data)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"ecmdGetConfigurationComplex");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapiSetAttribute(i_target, i_id, i_data);
#else
  if (fapiDllFnTable[ECMD_FAPISETATTRIBUTE] == NULL) {
     fapiDllFnTable[ECMD_FAPISETATTRIBUTE] = (void*)dlsym(dlHandle, "dllFapiSetAttribute");
     if (fapiDllFnTable[ECMD_FAPISETATTRIBUTE] == NULL) {
       fprintf(stderr,"dllFapiSetAttribute%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)(const fapi::Target &, const uint32_t,  fapi::AttributeData &) = 
      (uint32_t(*)(const fapi::Target &, const uint32_t,  fapi::AttributeData &))fapiDllFnTable[ECMD_FAPISETATTRIBUTE];
  rc =    (*Function)(i_target, i_id, i_data);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiSetAttribute");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t fapiSetAttribute(const fapi::Target & i_target, const uint32_t i_id, fapi::AttributeData & i_data)",args);
   }
#endif

  if (rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;
}


void fapiOutputError(const char* i_message) {

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiOutputError%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  dllFapiOutputError(i_message);
#else
  if (fapiDllFnTable[ECMD_FAPIOUTPUTERROR] == NULL) {
     fapiDllFnTable[ECMD_FAPIOUTPUTERROR] = (void*)dlsym(dlHandle, "dllFapiOutputError");
     if (fapiDllFnTable[ECMD_FAPIOUTPUTERROR] == NULL) {
       fprintf(stderr,"dllFapiOutputError%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  void (*Function)(const char*) = 
      (void(*)(const char*))fapiDllFnTable[ECMD_FAPIOUTPUTERROR];
   (*Function)(i_message);
#endif

}

void fapiOutputInfo(const char* i_message) {

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiOutputInfo%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  dllFapiOutputInfo(i_message);
#else
  if (fapiDllFnTable[ECMD_FAPIOUTPUTINFO] == NULL) {
     fapiDllFnTable[ECMD_FAPIOUTPUTINFO] = (void*)dlsym(dlHandle, "dllFapiOutputInfo");
     if (fapiDllFnTable[ECMD_FAPIOUTPUTINFO] == NULL) {
       fprintf(stderr,"dllFapiOutputInfo%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  void (*Function)(const char*) = 
      (void(*)(const char*))fapiDllFnTable[ECMD_FAPIOUTPUTINFO];
   (*Function)(i_message);
#endif

}

void fapiOutputImportant(const char* i_message) {

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiOutputImportant%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  dllFapiOutputImportant(i_message);
#else
  if (fapiDllFnTable[ECMD_FAPIOUTPUTIMPORTANT] == NULL) {
     fapiDllFnTable[ECMD_FAPIOUTPUTIMPORTANT] = (void*)dlsym(dlHandle, "dllFapiOutputImportant");
     if (fapiDllFnTable[ECMD_FAPIOUTPUTIMPORTANT] == NULL) {
       fprintf(stderr,"dllFapiOutputImportant%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  void (*Function)(const char*) = 
      (void(*)(const char*))fapiDllFnTable[ECMD_FAPIOUTPUTIMPORTANT];
   (*Function)(i_message);
#endif

}

void fapiOutputDebug(const char* i_message) {

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiOutputDebug%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  dllFapiOutputDebug(i_message);
#else
  if (fapiDllFnTable[ECMD_FAPIOUTPUTDEBUG] == NULL) {
     fapiDllFnTable[ECMD_FAPIOUTPUTDEBUG] = (void*)dlsym(dlHandle, "dllFapiOutputDebug");
     if (fapiDllFnTable[ECMD_FAPIOUTPUTDEBUG] == NULL) {
       fprintf(stderr,"dllFapiOutputDebug%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  void (*Function)(const char*) = 
      (void(*)(const char*))fapiDllFnTable[ECMD_FAPIOUTPUTDEBUG];
   (*Function)(i_message);
#endif
}

void fapiOutputScanTrace(const char* i_message) {

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiOutputScanTrace%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  dllFapiOutputScanTrace(i_message);
#else
  if (fapiDllFnTable[ECMD_FAPIOUTPUTSCANTRACE] == NULL) {
     fapiDllFnTable[ECMD_FAPIOUTPUTSCANTRACE] = (void*)dlsym(dlHandle, "dllFapiOutputScanTrace");
     if (fapiDllFnTable[ECMD_FAPIOUTPUTSCANTRACE] == NULL) {
       fprintf(stderr,"dllFapiOutputScanTrace%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  void (*Function)(const char*) = 
      (void(*)(const char*))fapiDllFnTable[ECMD_FAPIOUTPUTSCANTRACE];
   (*Function)(i_message);
#endif
}
