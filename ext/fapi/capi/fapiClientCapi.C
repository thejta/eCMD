/* NOTE : This file has been generated from the eCMD extension temp-late */
/* DO NOT EDIT THISFILE (unless you are editing the temp-late */


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

ReturnCode fapiRunSo(const Target & i_target, const char * i_sharedObjectName, const char * i_sharedObjectEntryPoint){
   
   printf("fapi::RunSo Opening %s...\n", i_sharedObjectName);
   void* handle = dlopen(i_sharedObjectName, RTLD_NOW | RTLD_GLOBAL);
    
   if (!handle) {
     printf("fapi::RunSo::Cannot open library: %s", dlerror()); //JFDEBUG
     return 1;  //JF FIXME
    }
    
   // load the symbol
   printf("fapi::RunSo Loading symbol %s...\n", i_sharedObjectEntryPoint);
   typedef fapi::ReturnCode (*fapi_ring_t)(fapi::Target);

   
    // reset errors
    dlerror();
    fapi_ring_t func = (fapi_ring_t) dlsym(handle, i_sharedObjectEntryPoint);
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        printf("Cannot load symbol: %s\n", dlsym_error);
        dlclose(handle);
        return 1;
    }
    
    fapi::ReturnCode fapiRc = func( i_target );
    
    // close the library
    printf("fapi::RunSo::Closing library...\n");
    dlclose(handle);

  return  fapiRc;
}

ReturnCode fapiRunSoWithArgs(const Target & i_target, const char * i_sharedObjectName, const char * i_sharedObjectEntryPoint, std::list<uint64_t> & i_sharedObjectArgs){
   
   printf("fapi::RunSoWithArgs Opening %s...\n", i_sharedObjectName);
   void* handle = dlopen(i_sharedObjectName, RTLD_NOW | RTLD_GLOBAL);
    
   if (!handle) {
     printf("fapi::RunSoWithArgs::Cannot open library: %s", dlerror()); //JFDEBUG
     return 1;  //JF FIXME
    }
    
   // load the symbol
   printf("fapi::RunSoWithArgs Loading symbol %s...\n", i_sharedObjectEntryPoint);
   typedef fapi::ReturnCode (*fapi_ring_t)(fapi::Target, std::list<uint64_t>);

   
    // reset errors
    dlerror();
    fapi_ring_t func = (fapi_ring_t) dlsym(handle, i_sharedObjectEntryPoint);
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        printf("Cannot load symbol: %s\n", dlsym_error);
        dlclose(handle);
        return 1;
    }
    
    fapi::ReturnCode fapiRc = func( i_target, i_sharedObjectArgs);
    
    // close the library
    printf("fapi::RunSoWithArgs::Closing library...\n");
    dlclose(handle);

  return  fapiRc;
}

/* These functions were auto-generated then modified  - farrugia */
ReturnCode fapiGetScom(const Target& i_target, const uint64_t i_address, ecmdDataBufferBase & o_data) {
  ReturnCode rc;

  ecmdChipTarget   ecmdTarget;
  ecmdChipTarget * ecmdTargetPtr;
  ecmdTargetPtr = (ecmdChipTarget *) i_target.get();
  ecmdTarget = (*ecmdTargetPtr);                

/*
  ecmdDataBuffer *dummy;
  dummy = o_data.getBuff();
 */ 
#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllGetScom%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &ecmdTarget);
     args.push_back((void*) &i_address);
     args.push_back((void*) &o_data);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t getScom(ecmdChipTarget & ecmdTarget, uint64_t i_address, ecmdDataBufferBase & o_data)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"getScom");
  }
#endif

   ecmdChipTarget cacheTarget;
   cacheTarget = ecmdTarget;
   ecmdSetTargetDepth(cacheTarget, ECMD_DEPTH_CHIP);
   if (ecmdIsRingCacheEnabled(cacheTarget)) return ECMD_RING_CACHE_ENABLED;
#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllGetScom(ecmdTarget, i_address, o_data);
#else
  if (DllFnTable[ECMD_GETSCOM] == NULL) {
     DllFnTable[ECMD_GETSCOM] = (void*)dlsym(dlHandle, "dllGetScom");
     if (DllFnTable[ECMD_GETSCOM] == NULL) {
       fprintf(stderr,"dllGetScom%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)(ecmdChipTarget &,  uint64_t,  ecmdDataBufferBase &) = 
      (uint32_t(*)(ecmdChipTarget &,  uint64_t,  ecmdDataBufferBase &))DllFnTable[ECMD_GETSCOM];
  rc =    (*Function)(ecmdTarget, i_address, o_data);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"getScom");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t getScom(ecmdChipTarget & ecmdTarget, uint64_t i_address, ecmdDataBufferBase & o_data)",args);
   }
#endif

  if (rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

   /* Copy the data over */
   /*
   if (rc.ok())
     o_data.setBuff(dummy);
   */  

  return rc;
}



ReturnCode fapiPutScom(const Target& i_target, const uint32_t i_address,  ecmdDataBufferBase & i_data) {

  ReturnCode rc;

  ecmdChipTarget   ecmdTarget;
  ecmdChipTarget * ecmdTargetPtr;
  ecmdTargetPtr = (ecmdChipTarget *) i_target.get();
  ecmdTarget = (*ecmdTargetPtr);                

/*
  ecmdDataBuffer *dummy;
  dummy = i_data.getBuff();
*/

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllPutScom%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &ecmdTarget);
     args.push_back((void*) &i_address);
     args.push_back((void*) &i_data);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t putScom(ecmdChipTarget & ecmdTarget, uint64_t i_address, ecmdDataBufferBase & i_data)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"putScom");
  }
#endif

   ecmdChipTarget cacheTarget;
   cacheTarget = ecmdTarget;
   ecmdSetTargetDepth(cacheTarget, ECMD_DEPTH_CHIP);
   if (ecmdIsRingCacheEnabled(cacheTarget)) return ECMD_RING_CACHE_ENABLED;
#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllPutScom(ecmdTarget, i_address, i_data);
#else
  if (DllFnTable[ECMD_PUTSCOM] == NULL) {
     DllFnTable[ECMD_PUTSCOM] = (void*)dlsym(dlHandle, "dllPutScom");
     if (DllFnTable[ECMD_PUTSCOM] == NULL) {
       fprintf(stderr,"dllPutScom%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)(ecmdChipTarget &,  uint64_t,  ecmdDataBufferBase &) = 
      (uint32_t(*)(ecmdChipTarget &,  uint64_t,  ecmdDataBufferBase &))DllFnTable[ECMD_PUTSCOM];
  rc =    (*Function)(ecmdTarget, i_address, i_data);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"putScom");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t putScom(ecmdChipTarget & ecmdTarget, uint64_t i_address, ecmdDataBufferBase & i_data)",args);
   }
#endif

  if (rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;
}
#if 0
// re-add when ready to support this 
ReturnCode fapi::PutScomUnderMask(const Target& i_handle, /* JFDEBUG const */ uint64_t i_address, ecmdDataBufferBase & i_data, const ecmdDataBufferBase & i_mask) {

  ReturnCode rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllPutScomUnderMask%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

  if (!fapiInitialized) {
    fprintf(stderr,"dllPutScomUnderMask: eCMD Extension not initialized before function called\n");
    fprintf(stderr,"dllPutScomUnderMask: OR eCMD fapi Extension not supported by plugin\n");
    exit(ECMD_DLL_INVALID);
  }

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_handle);
     args.push_back((void*) &i_address);
     args.push_back((void*) &i_data);
     args.push_back((void*) &i_mask);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"ReturnCode fapi::PutScomUnderMask(const Target& i_handle, /* JFDEBUG const */ uint64_t i_address, /* JFDEBUG const */DataBuffer & i_data, const DataBuffer & i_mask)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapi::PutScomUnderMask");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllPutScomUnderMask(i_handle, i_address, i_data, i_mask);
#else
  if (DllFnTable[ECMD_PUTSCOMUNDERMASK] == NULL) {
     DllFnTable[ECMD_PUTSCOMUNDERMASK] = (void*)dlsym(dlHandle, "dllPutScomUnderMask");
     if (DllFnTable[ECMD_PUTSCOMUNDERMASK] == NULL) {
       fprintf(stderr,"dllPutScomUnderMask%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const Target&,  /* JFDEBUG const */ uint64_t,  ecmdDataBufferBase &,  const ecmdDataBufferBase &) = 
      (ReturnCode(*)(const Target&,  /* JFDEBUG const */ uint64_t,  ecmdDataBufferBase &,  const ecmdDataBufferBase &))DllFnTable[ECMD_PUTSCOMUNDERMASK];
  rc =    (*Function)(i_handle, i_address, i_data, i_mask);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapi::PutScomUnderMask");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"ReturnCode fapi::PutScomUnderMask(const Target& i_handle, /* JFDEBUG const */ uint64_t i_address, /* JFDEBUG const */DataBuffer & i_data, const DataBuffer & i_mask)",args);
   }
#endif

  return rc;
}

#endif

ReturnCode fapiGetCfamRegister(const Target& i_target, const uint32_t i_address, ecmdDataBufferBase & o_data){

  ReturnCode rc; 

  ecmdChipTarget   ecmdTarget;
  ecmdChipTarget * ecmdTargetPtr;
  ecmdTargetPtr = (ecmdChipTarget *) i_target.get();
  ecmdTarget = (*ecmdTargetPtr);                

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllGetCfamRegister%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &ecmdTarget);
     args.push_back((void*) &i_address);
     args.push_back((void*) &o_data);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t getCfamRegister(ecmdChipTarget & ecmdTarget, uint32_t i_address, ecmdDataBufferBase & o_data)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"getCfamRegister");
  }
#endif

   ecmdChipTarget cacheTarget;
   cacheTarget = ecmdTarget;
   ecmdSetTargetDepth(cacheTarget, ECMD_DEPTH_CHIP);
   if (ecmdIsRingCacheEnabled(cacheTarget)) return ECMD_RING_CACHE_ENABLED;
#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllGetCfamRegister(ecmdTarget, i_address, o_data);
#else
  if (DllFnTable[ECMD_GETCFAMREGISTER] == NULL) {
     DllFnTable[ECMD_GETCFAMREGISTER] = (void*)dlsym(dlHandle, "dllGetCfamRegister");
     if (DllFnTable[ECMD_GETCFAMREGISTER] == NULL) {
       fprintf(stderr,"dllGetCfamRegister%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)(ecmdChipTarget &,  uint32_t,  ecmdDataBufferBase &) = 
      (uint32_t(*)(ecmdChipTarget &,  uint32_t,  ecmdDataBufferBase &))DllFnTable[ECMD_GETCFAMREGISTER];
  rc =    (*Function)(ecmdTarget, i_address, o_data);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"getCfamRegister");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t getCfamRegister(ecmdChipTarget & ecmdTarget, uint32_t i_address, ecmdDataBufferBase & o_data)",args);
   }
#endif

  if (rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;
}


ReturnCode fapiPutCfamRegister(const Target& i_target, const uint32_t i_address, ecmdDataBufferBase & i_data){

  ReturnCode rc;
  
  ecmdChipTarget   ecmdTarget;
  ecmdChipTarget * ecmdTargetPtr;
  ecmdTargetPtr = (ecmdChipTarget *) i_target.get();
  ecmdTarget = (*ecmdTargetPtr);                

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllPutCfamRegister%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &ecmdTarget);
     args.push_back((void*) &i_address);
     args.push_back((void*) &i_data);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t putCfamRegister(ecmdChipTarget & ecmdTarget, uint32_t i_address, ecmdDataBuffer & i_data)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"putCfamRegister");
  }
#endif

   ecmdChipTarget cacheTarget;
   cacheTarget = ecmdTarget;
   ecmdSetTargetDepth(cacheTarget, ECMD_DEPTH_CHIP);
   if (ecmdIsRingCacheEnabled(cacheTarget)) return ECMD_RING_CACHE_ENABLED;
#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllPutCfamRegister(ecmdTarget, i_address, i_data);
#else
  if (DllFnTable[ECMD_PUTCFAMREGISTER] == NULL) {
     DllFnTable[ECMD_PUTCFAMREGISTER] = (void*)dlsym(dlHandle, "dllPutCfamRegister");
     if (DllFnTable[ECMD_PUTCFAMREGISTER] == NULL) {
       fprintf(stderr,"dllPutCfamRegister%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)(ecmdChipTarget &,  uint32_t,  ecmdDataBufferBase &) = 
      (uint32_t(*)(ecmdChipTarget &,  uint32_t,  ecmdDataBufferBase &))DllFnTable[ECMD_PUTCFAMREGISTER];
  rc =    (*Function)(ecmdTarget, i_address, i_data);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"putCfamRegister");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t putCfamRegister(ecmdChipTarget & ecmdTarget, uint32_t i_address, ecmdDataBuffer & i_data)",args);
   }
#endif

  if (rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;
}


uint32_t fapiHwpInvoker(ecmdChipTarget & i_target, const std::string & i_sharedObjectName, const std::string & i_sharedObjectEntryPoint, std::list<uint64_t> &i_sharedObjectArgs) {

  uint32_t rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllHwpInvoker%s",ECMD_DLL_NOT_LOADED_ERROR);
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
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t HwpInvoker(ecmdChipTarget & i_target, const std::string & i_sharedObjectName, const std::string & i_sharedObjectEntryPoint, std::list<uint64_t> &i_sharedObjectArgs)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"HwpInvoker");
  }
#endif

#if 0
#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllHwpInvoker(i_target, i_sharedObjectName, i_sharedObjectEntryPoint, &i_sharedObjectArgs);
#else
  if (DllFnTable[ECMD_HWPINVOKER] == NULL) {
     DllFnTable[ECMD_HWPINVOKER] = (void*)dlsym(dlHandle, "dllHwpInvoker");
     if (DllFnTable[ECMD_HWPINVOKER] == NULL) {
       fprintf(stderr,"dllHwpInvoker%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)(ecmdChipTarget &,  const std::string &,  const std::string &,  std::list<uint64_t>) = 
      (uint32_t(*)(ecmdChipTarget &,  const std::string &,  const std::string &,  std::list<uint64_t>))DllFnTable[ECMD_HWPINVOKER];
  rc =    (*Function)(i_target, i_sharedObjectName, i_sharedObjectEntryPoint, i_sharedObjectArgs);
#endif
#endif 
  ecmdChipTarget * i_targetPtr;
  i_targetPtr = &i_target;

   //printf("hwpInvoker::Opening %s...\n", i_sharedObjectName.c_str());
   void* handle = dlopen(i_sharedObjectName.c_str(), RTLD_NOW | RTLD_GLOBAL);
   if (!handle) {
   //  printf("hwpInvoker::Cannot open library: %s\n", dlerror()); //JFDEBUG
     /*std::string errorStr;
     errorStr = "Cannot open library " + err;
     errorStr += "\n";
     ecmdOutputError(errorStr.c_str());*/
     rc = 1;  //JF FIXME
    }
    
   // load the symbol
   printf("HwpInvoker::Loading symbol %s...\n", i_sharedObjectName.c_str());
     /* FAPI ext based .so entry point was selected */
     typedef fapi::ReturnCode (*fapi_ring_t)(fapi::Target, std::list<uint64_t>);
   
     // reset errors
     dlerror();
     fapi_ring_t func = (fapi_ring_t) dlsym(handle, i_sharedObjectEntryPoint.c_str());
     const char *dlsym_error = dlerror();
     if (dlsym_error) {
       printf("Cannot load symbol '%s'\n", i_sharedObjectEntryPoint.c_str());
       //cerr << "Cannot load symbol 'hwProcEntryPointWithArgs': " << dlsym_error << '\n';
       dlclose(handle);
       rc = 1;
     }
    
     // use it to do the calculation
     //ecmdOutput("Calling hwProcEntryPoint...\n");
     printf("HwpInvoker::Calling '%s'\n", i_sharedObjectEntryPoint.c_str());
     fapi::Target myFapiTarget;
     myFapiTarget.set(i_targetPtr);
     fapi::ReturnCode fapiRc = func( myFapiTarget, i_sharedObjectArgs);
    
     // close the library
     printf("HwpInvoker::Closing '%s'\n", i_sharedObjectName.c_str());
     dlclose(handle);



#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"HwpInvoker");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t HwpInvoker(ecmdChipTarget & i_target, const std::string & i_sharedObjectName, const std::string & i_sharedObjectEntryPoint, std::list<uint64_t> &i_sharedObjectArgs)",args);
   }
#endif

  if (rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
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
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t fapi::QueryFileLocation(fapi::FileType_t i_fileType, std::string & i_fileName, std::string & o_fileLocation, std::string i_version)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapi::QueryFileLocation");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapiQueryFileLocation(i_fileType, i_fileName, o_fileLocation, i_version);
#else
  if (DllFnTable[ECMD_FAPIQUERYFILELOCATION] == NULL) {
     DllFnTable[ECMD_FAPIQUERYFILELOCATION] = (void*)dlsym(dlHandle, "dllFapiQueryFileLocation");
     if (DllFnTable[ECMD_FAPIQUERYFILELOCATION] == NULL) {
       fprintf(stderr,"dllFapiQueryFileLocation%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)(fapi::FileType_t,  std::string &, std::string &,  std::string) = 
      (uint32_t(*)(fapi::FileType_t,  std::string &, std::string &,  std::string))DllFnTable[ECMD_FAPIQUERYFILELOCATION];
  rc =    (*Function)(i_fileType, i_fileName, o_fileLocation, i_version);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapi::QueryFileLocation");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t fapi::QueryFileLocation(fapi::FileType_t i_fileType, std::string & i_fileName, std::string & o_fileLocation, std::string i_version)",args);
   }
#endif

  if (rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;
}





#if 0


uint32_t fapi::HwpInvoker(ecmdChipTarget & i_target, const std::string & i_sharedObjectName, const std::string & i_sharedObjectEntryPoint, std::list<uint64_t> &i_sharedObjectArgs){


  ecmdChipTarget * i_targetPtr;
  i_targetPtr = &i_target;
   
   //if (debug.isOn('F','T'))
   printf("hwpInvoker::Opening %s...\n", i_sharedObjectName.c_str());
   void* handle = dlopen(i_sharedObjectName.c_str(), RTLD_NOW | RTLD_GLOBAL);

    
   if (!handle) {
     printf("hwpInvoker::Cannot open library: %s\n", dlerror()); //JFDEBUG
     /*std::string errorStr;
     errorStr = "Cannot open library " + err;
     errorStr += "\n";
     ecmdOutputError(errorStr.c_str());*/
     return 1;  //JF FIXME
    }
    
   // load the symbol
   //if (debug.isOn('F','T'))
   printf("hwpInvoker::Loading symbol %s...\n", i_sharedObjectName.c_str());

     /* FAPI ext based .so entry point was selected */
     typedef fapi::ReturnCode (*fapi_ring_t)(fapi::Target, std::list<uint64_t>);
   
     // reset errors
     dlerror();
     fapi_ring_t func = (fapi_ring_t) dlsym(handle, i_sharedObjectEntryPoint.c_str());
     const char *dlsym_error = dlerror();
     if (dlsym_error) {
       printf("Cannot load symbol '%s'\n", i_sharedObjectEntryPoint.c_str());
       //cerr << "Cannot load symbol 'hwProcEntryPointWithArgs': " << dlsym_error << '\n';
       dlclose(handle);
       return 1;
     }
    
     // use it to do the calculation
     //ecmdOutput("Calling hwProcEntryPoint...\n");
     printf("Calling '%s'\n", i_sharedObjectEntryPoint.c_str());
     fapi::Target myFapiTarget;
     myFapiTarget.set(i_targetPtr);
     fapi::ReturnCode fapiRc = func( myFapiTarget, i_sharedObjectArgs);
    
     // close the library
     printf("Closing '%s'\n", i_sharedObjectName.c_str());
     dlclose(handle);
 
     return uint32_t(fapiRc);

}
#endif
