// Copyright ***********************************************************
//                                                                        
// File fapiPlatHwAccess.C                                   
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                         
// (C) COPYRIGHT IBM CORP. 1998                                         
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

#include <ecmdClientCapi.H>
#include <ecmdSharedUtils.H>
#include <ecmdClientEnums.H>
#include <fapiClientCapi.H>

#include <fapiHwAccess.H> 
#include <fapiMvpdAccess.H> 
#include <fapiSharedUtils.H> 
#ifndef ECMD_STATIC_FUNCTIONS
#include <fapiClientEnums.H>
#include <fapiTarget.H>
#endif

#include <fapiDllCapi.H>
#include <ecmdDllCapi.H>

#include <ecmdUtils.H>

static const char ECMD_DLL_NOT_LOADED_ERROR[] = ": eCMD Function called before DLL has been loaded\n";
static const char ECMD_UNABLE_TO_FIND_FUNCTION_ERROR[] = ": Unable to find function, must be an invalid DLL - program aborting\n";

using namespace fapi;

#ifndef ECMD_STATIC_FUNCTIONS

 #include <dlfcn.h>

/* This is from ecmdClientCapiFunc.C */
 extern void * dlHandle;
 extern void * DllFnTable[];
 extern void * fapiDllFnTable[];

#else

 #include <fapiDllCapi.H>

#endif


extern bool fapiInitialized;
#ifndef ECMD_STRIP_DEBUG
extern int ecmdClientDebug;
extern int fppCallCount;
extern bool ecmdDebugOutput;
#endif

extern "C"  {

ReturnCode platGetRing(const Target& i_handle, const uint32_t i_address, ecmdDataBufferBase & o_data, const uint32_t i_ringMode) {

  ReturnCode rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiGetRing%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

  if (!fapiInitialized) {
    fprintf(stderr,"dllFapiGetRing: eCMD Extension not initialized before function called\n");
    fprintf(stderr,"dllFapiGetRing: OR eCMD fapi Extension not supported by plugin\n");
    exit(ECMD_DLL_INVALID);
  }

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_handle);
     args.push_back((void*) &i_address);
     args.push_back((void*) &o_data);
     args.push_back((void*) &i_ringMode);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"ReturnCode fapi::GetRing(const Target& i_handle, const uint32_t i_address, ecmdDataBufferBase & o_data, const uint32_t i_ringMode)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapi::GetRing");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapiGetRing(i_handle, i_address, o_data, i_ringMode);
#else
  if (fapiDllFnTable[ECMD_FAPIGETRING] == NULL) {
     fapiDllFnTable[ECMD_FAPIGETRING] = (void*)dlsym(dlHandle, "dllFapiGetRing");
     if (fapiDllFnTable[ECMD_FAPIGETRING] == NULL) {
       fprintf(stderr,"dllFapiGetRing%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const Target&,  const uint32_t,  ecmdDataBufferBase &, const uint32_t ) = 
      (ReturnCode(*)(const Target&,  const uint32_t,  ecmdDataBufferBase &, const uint32_t))fapiDllFnTable[ECMD_FAPIGETRING];
  rc =    (*Function)(i_handle, i_address, o_data, i_ringMode);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiGetRing");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"ReturnCode fapiGetRing(const Target& i_handle, const uint32_t i_address, ecmdDataBufferBase & o_data)",args);
   }
#endif

  return rc;
}

ReturnCode platPutRing(const Target& i_handle, const uint32_t i_address, ecmdDataBufferBase & i_data, const uint32_t i_ringMode) {

  ReturnCode rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiPutRing%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

  if (!fapiInitialized) {
    fprintf(stderr,"dllFapiPutRing: eCMD Extension not initialized before function called\n");
    fprintf(stderr,"dllFapiPutRing: OR eCMD fapi Extension not supported by plugin\n");
    exit(ECMD_DLL_INVALID);
  }

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_handle);
     args.push_back((void*) &i_address);
     args.push_back((void*) &i_data);
     args.push_back((void*) &i_ringMode);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"ReturnCode fapiPutRing(const Target& i_handle, const uint32_t i_address, ecmdDataBufferBase & i_data, const uint32_t i_ringMode)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapiPutRing");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapiPutRing(i_handle, i_address, i_data, i_ringMode);
#else
  if (fapiDllFnTable[ECMD_FAPIPUTRING] == NULL) {
     fapiDllFnTable[ECMD_FAPIPUTRING] = (void*)dlsym(dlHandle, "dllFapiPutRing");
     if (fapiDllFnTable[ECMD_FAPIPUTRING] == NULL) {
       fprintf(stderr,"dllFapiPutRing%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const Target&,  const uint32_t, ecmdDataBufferBase &, const uint32_t i_ringMode) = 
      (ReturnCode(*)(const Target&,  const uint32_t, ecmdDataBufferBase &, const uint32_t))fapiDllFnTable[ECMD_FAPIPUTRING];
  rc =    (*Function)(i_handle, i_address, i_data, i_ringMode);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiPutRing");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"ReturnCode fapiPutRing(const Target& i_handle, const uint32_t i_address, ecmdDataBufferBase & i_data, const uint32_t i_ringMode)",args);
   }
#endif

  return rc;
}
ReturnCode platModifyRing(const fapi::Target& i_target, const uint32_t i_address, ecmdDataBufferBase & i_data, const fapi::ChipOpModifyMode i_modifyMode, const uint32_t i_ringMode = 0)
{
    fprintf(stderr,"dllModifyRing%s",ECMD_DLL_NOT_LOADED_ERROR);
    ReturnCode rc;
    rc.setEcmdError(ECMD_DLL_INVALID);
    return rc;


}
ReturnCode platGetScom(const Target& i_target, const uint64_t i_address, ecmdDataBufferBase & o_data) 
{
  ReturnCode rc;
  uint32_t l_ecmdRc;

  ecmdDataBuffer l_ecmd_buffer;

  ecmdChipTarget   ecmdTarget;
  fapiTargetToEcmdTarget(i_target, ecmdTarget); 

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
     args.push_back((void*) &l_ecmd_buffer);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t getScom(ecmdChipTarget & ecmdTarget, uint64_t i_address, ecmdDataBufferBase & o_data)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"getScom");
  }
#endif

   ecmdChipTarget cacheTarget;
   cacheTarget = ecmdTarget;
   ecmdSetTargetDepth(cacheTarget, ECMD_DEPTH_CHIP);
   if (ecmdIsRingCacheEnabled(cacheTarget)) {
      rc.setEcmdError(ECMD_RING_CACHE_ENABLED);
      return rc;
   }
#ifdef ECMD_STATIC_FUNCTIONS
  l_ecmdRc = dllGetScom(ecmdTarget, i_address, l_ecmd_buffer); 
  if (l_ecmdRc)
  {
    rc.setEcmdError(l_ecmdRc); 
  }
#else
  if (DllFnTable[ECMD_GETSCOM] == NULL) {
     DllFnTable[ECMD_GETSCOM] = (void*)dlsym(dlHandle, "dllGetScom");
     if (DllFnTable[ECMD_GETSCOM] == NULL) {
       fprintf(stderr,"dllGetScom%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)(ecmdChipTarget &,  uint64_t,  ecmdDataBuffer &) = 
      (uint32_t(*)(ecmdChipTarget &,  uint64_t,  ecmdDataBuffer &))DllFnTable[ECMD_GETSCOM];
  l_ecmdRc =    (*Function)(ecmdTarget, i_address, l_ecmd_buffer);
  if (l_ecmdRc)
  {
    rc.setEcmdError(l_ecmdRc); 
  }
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &l_ecmdRc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"getScom");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t getScom(ecmdChipTarget & ecmdTarget, uint64_t i_address, ecmdDataBufferBase & o_data)",args);
   }
#endif

  if (l_ecmdRc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(l_ecmdRc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  o_data = l_ecmd_buffer;

  return rc;
}

ReturnCode platPutScom(const Target& i_target, const uint64_t i_address,  ecmdDataBufferBase & i_data) 
{
  ReturnCode rc;
  uint32_t l_ecmdRc;

  ecmdDataBuffer l_ecmd_buffer;
  l_ecmd_buffer = i_data;

  ecmdChipTarget   ecmdTarget;
  ecmdChipTarget * ecmdTargetPtr;
  ecmdTargetPtr = (ecmdChipTarget *) i_target.get();
  ecmdTarget = (*ecmdTargetPtr);                

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
     args.push_back((void*) &l_ecmd_buffer);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t putScom(ecmdChipTarget & ecmdTarget, uint64_t i_address, ecmdDataBufferBase & i_data)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"putScom");
  }
#endif

   ecmdChipTarget cacheTarget;
   cacheTarget = ecmdTarget;
   ecmdSetTargetDepth(cacheTarget, ECMD_DEPTH_CHIP);
   if (ecmdIsRingCacheEnabled(cacheTarget)){
     rc.setEcmdError(ECMD_RING_CACHE_ENABLED);
     return rc;
   }
#ifdef ECMD_STATIC_FUNCTIONS
  l_ecmdRc = dllPutScom(ecmdTarget, i_address, l_ecmd_buffer); 
  if (l_ecmdRc)
  {
    rc.setEcmdError(l_ecmdRc);
  }
#else
  if (DllFnTable[ECMD_PUTSCOM] == NULL) {
     DllFnTable[ECMD_PUTSCOM] = (void*)dlsym(dlHandle, "dllPutScom");
     if (DllFnTable[ECMD_PUTSCOM] == NULL) {
       fprintf(stderr,"dllPutScom%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)(ecmdChipTarget &,  uint64_t,  ecmdDataBuffer &) = 
      (uint32_t(*)(ecmdChipTarget &,  uint64_t,  ecmdDataBuffer &))DllFnTable[ECMD_PUTSCOM];
  l_ecmdRc =    (*Function)(ecmdTarget, i_address, l_ecmd_buffer);
  if (l_ecmdRc)
  {
    rc.setEcmdError(l_ecmdRc);
  }
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &l_ecmdRc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"putScom");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t putScom(ecmdChipTarget & ecmdTarget, uint64_t i_address, ecmdDataBufferBase & i_data)",args);
   }
#endif

  if (l_ecmdRc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(l_ecmdRc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;
}

ReturnCode platPutScomUnderMask(const Target& i_target, const  uint64_t i_address, ecmdDataBufferBase & i_data, ecmdDataBufferBase & i_mask) 
{
  ReturnCode rc;
  uint32_t l_ecmdRc;
 
  ecmdDataBuffer l_ecmd_buffer, l_ecmd_buffer_mask;
  l_ecmd_buffer = i_data;
  l_ecmd_buffer_mask = i_mask;

  ecmdChipTarget   ecmdTarget;
  ecmdChipTarget * ecmdTargetPtr;
  ecmdTargetPtr = (ecmdChipTarget *) i_target.get();
  ecmdTarget = (*ecmdTargetPtr);  

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
     args.push_back((void*) &ecmdTarget);
     args.push_back((void*) &i_address);
     args.push_back((void*) &l_ecmd_buffer);
     args.push_back((void*) &l_ecmd_buffer_mask);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"ReturnCode fapi::PutScomUnderMask(const Target& i_handle, const uint64_t i_address, ecmdDataBufferBase & i_data, const ecmdDataBufferBase & i_mask)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapi::PutScomUnderMask");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  l_ecmdRc = dllPutScomUnderMask(ecmdTarget, i_address, l_ecmd_buffer, l_ecmd_buffer_mask);
  if (l_ecmdRc)
  {
    rc.setEcmdError(l_ecmdRc);
  }
#else
  if (DllFnTable[ECMD_PUTSCOMUNDERMASK] == NULL) {
     DllFnTable[ECMD_PUTSCOMUNDERMASK] = (void*)dlsym(dlHandle, "dllPutScomUnderMask");
     if (DllFnTable[ECMD_PUTSCOMUNDERMASK] == NULL) {
       fprintf(stderr,"dllPutScomUnderMask%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)(const ecmdChipTarget&, const uint64_t,  ecmdDataBuffer &,  const ecmdDataBuffer &) = 
      (uint32_t(*)(const ecmdChipTarget&,  const uint64_t,  ecmdDataBuffer &,  const ecmdDataBuffer &))DllFnTable[ECMD_PUTSCOMUNDERMASK];
  l_ecmdRc =    (*Function)(ecmdTarget, i_address, l_ecmd_buffer, l_ecmd_buffer_mask);
  if (l_ecmdRc)
  {
    rc.setEcmdError(l_ecmdRc);
  }
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &l_ecmdRc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapi::PutScomUnderMask");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"ReturnCode fapi::PutScomUnderMask(const Target& i_handle,  const uint64_t i_address,  const ecmdDataBufferBase & i_data, const ecmdDataBufferBase & i_mask)",args);
   }
#endif

  return rc;
}

#ifdef FAPI_SUPPORT_MULTI_SCOM
fapi::ReturnCode platMultiScom(const fapi::Target & i_target, fapi::MultiScom & io_multiScomList)
{

  ReturnCode rc;
  uint32_t l_ecmdRc;
  
  ecmdChipTarget   ecmdTarget;
  ecmdChipTarget * ecmdTargetPtr;
  ecmdTargetPtr = (ecmdChipTarget *) i_target.get();
  ecmdTarget = (*ecmdTargetPtr);                

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiMultiScom%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_target);
     args.push_back((void*) &io_multiScomList);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"fapi::ReturnCode fapiMultiScom(const fapi::Target & i_target, fapi::MultiScom & io_multiScomList)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapiMultiScom");
  }
#endif

   ecmdChipTarget cacheTarget;
   cacheTarget = ecmdTarget;
   ecmdSetTargetDepth(cacheTarget, ECMD_DEPTH_CHIP);
   if (ecmdIsRingCacheEnabled(cacheTarget)){
     rc.setEcmdError(ECMD_RING_CACHE_ENABLED);
     return rc;
   }
#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapiMultiScom(i_target, io_multiScomList);
#else
  if (fapiDllFnTable[ECMD_FAPIMULTISCOM] == NULL) {
     fapiDllFnTable[ECMD_FAPIMULTISCOM] = (void*)dlsym(dlHandle, "dllFapiMultiScom");
     if (fapiDllFnTable[ECMD_FAPIMULTISCOM] == NULL) {
       fprintf(stderr,"dllFapiMultiScom%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const fapi::Target &, fapi::MultiScom &) = 
      (ReturnCode(*)(const fapi::Target &, fapi::MultiScom &))fapiDllFnTable[ECMD_FAPIMULTISCOM];
  rc =    (*Function)(i_target, io_multiScomList);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &l_ecmdRc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiMultiScom");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"fapi::ReturnCode fapiMultiScom(const fapi::Target & i_target, fapi::MultiScom & io_multiScomList)",args);
   }
#endif

  return rc;

}
#endif

ReturnCode platGetCfamRegister(const Target& i_target, const uint32_t i_address, ecmdDataBufferBase & o_data)
{
  ReturnCode rc; 
  uint32_t l_ecmdRc;

  ecmdDataBuffer l_ecmd_buffer;
 
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
     args.push_back((void*) &l_ecmd_buffer);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t getCfamRegister(ecmdChipTarget & ecmdTarget, uint32_t i_address, ecmdDataBufferBase & o_data)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"getCfamRegister");
  }
#endif

   ecmdChipTarget cacheTarget;
   cacheTarget = ecmdTarget;
   ecmdSetTargetDepth(cacheTarget, ECMD_DEPTH_CHIP);
   if (ecmdIsRingCacheEnabled(cacheTarget)){
     rc.setEcmdError(ECMD_RING_CACHE_ENABLED);
     return rc;
   }
#ifdef ECMD_STATIC_FUNCTIONS
  l_ecmdRc = dllGetCfamRegister(ecmdTarget, i_address, l_ecmd_buffer); 
  if (l_ecmdRc)
  {
    rc.setEcmdError(l_ecmdRc);
  }
#else
  if (DllFnTable[ECMD_GETCFAMREGISTER] == NULL) {
     DllFnTable[ECMD_GETCFAMREGISTER] = (void*)dlsym(dlHandle, "dllGetCfamRegister");
     if (DllFnTable[ECMD_GETCFAMREGISTER] == NULL) {
       fprintf(stderr,"dllGetCfamRegister%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)(ecmdChipTarget &,  uint32_t,  ecmdDataBuffer &) = 
      (uint32_t(*)(ecmdChipTarget &,  uint32_t,  ecmdDataBuffer &))DllFnTable[ECMD_GETCFAMREGISTER];
  l_ecmdRc =    (*Function)(ecmdTarget, i_address, l_ecmd_buffer);
  if (l_ecmdRc)
  {
    rc.setEcmdError(l_ecmdRc);
  }
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &l_ecmdRc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"getCfamRegister");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t getCfamRegister(ecmdChipTarget & ecmdTarget, uint32_t i_address, ecmdDataBufferBase & o_data)",args);
   }
#endif

  if (l_ecmdRc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(l_ecmdRc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  o_data = l_ecmd_buffer;

  return rc;
}


ReturnCode platPutCfamRegister(const Target& i_target, const uint32_t i_address, ecmdDataBufferBase & i_data)
{
  ReturnCode rc;
  uint32_t l_ecmdRc;

  ecmdDataBuffer l_ecmd_buffer;
  l_ecmd_buffer = i_data;
  
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
     args.push_back((void*) &l_ecmd_buffer);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t putCfamRegister(ecmdChipTarget & ecmdTarget, uint32_t i_address, ecmdDataBuffer & i_data)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"putCfamRegister");
  }
#endif

   ecmdChipTarget cacheTarget;
   cacheTarget = ecmdTarget;
   ecmdSetTargetDepth(cacheTarget, ECMD_DEPTH_CHIP);
   if (ecmdIsRingCacheEnabled(cacheTarget)){
     rc.setEcmdError(ECMD_RING_CACHE_ENABLED);
     return rc;
   }
#ifdef ECMD_STATIC_FUNCTIONS
  l_ecmdRc = dllPutCfamRegister(ecmdTarget, i_address, l_ecmd_buffer); 
  if (l_ecmdRc)
  {
    rc.setEcmdError(l_ecmdRc);
  }
#else
  if (DllFnTable[ECMD_PUTCFAMREGISTER] == NULL) {
     DllFnTable[ECMD_PUTCFAMREGISTER] = (void*)dlsym(dlHandle, "dllPutCfamRegister");
     if (DllFnTable[ECMD_PUTCFAMREGISTER] == NULL) {
       fprintf(stderr,"dllPutCfamRegister%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)(ecmdChipTarget &,  uint32_t,  ecmdDataBuffer &) = 
      (uint32_t(*)(ecmdChipTarget &,  uint32_t,  ecmdDataBuffer &))DllFnTable[ECMD_PUTCFAMREGISTER];
  l_ecmdRc =    (*Function)(ecmdTarget, i_address, l_ecmd_buffer);
  if (l_ecmdRc)
  {
    rc.setEcmdError(l_ecmdRc);
  }
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &l_ecmdRc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"putCfamRegister");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t putCfamRegister(ecmdChipTarget & ecmdTarget, uint32_t i_address, ecmdDataBuffer & i_data)",args);
   }
#endif

  if (l_ecmdRc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(l_ecmdRc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;
}

ReturnCode platModifyCfamRegister(const Target& i_target, const uint32_t i_address, ecmdDataBufferBase & i_data, const fapi::ChipOpModifyMode i_modifyMode)
{
    fprintf(stderr,"dllModifyCfamRegister%s",ECMD_DLL_NOT_LOADED_ERROR);
    ReturnCode rc;
    rc.setEcmdError(ECMD_DLL_INVALID);
    return rc;
}

ReturnCode fapiGetMvpdField(const fapi::MvpdRecord i_record,
                                  const fapi::MvpdKeyword i_keyword,
                                  const fapi::Target &i_procTarget,
                                  uint8_t * const i_pBuffer,
                                  uint32_t &io_fieldSize)
{

  ReturnCode rc;
  uint32_t l_ecmdRc;
  
  ecmdChipTarget   ecmdTarget;
  ecmdChipTarget * ecmdTargetPtr;
  ecmdTargetPtr = (ecmdChipTarget *) i_procTarget.get();
  ecmdTarget = (*ecmdTargetPtr);                

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiGetVpdField%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_record);
     args.push_back((void*) &i_keyword);
     args.push_back((void*) &i_procTarget);
     args.push_back((void*) &i_pBuffer);
     args.push_back((void*) &io_fieldSize);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t fapiGetMvpdField(const fapi::MvpdRecord i_record, const fapi::MvpdKeyword i_keyword, const fapi::Target &i_procTarget, uint8_t * const i_pBuffer, uint32_t &io_fieldSize) )",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapiGetMvpdField");
  }
#endif

   ecmdChipTarget cacheTarget;
   cacheTarget = ecmdTarget;
   ecmdSetTargetDepth(cacheTarget, ECMD_DEPTH_CHIP);
   if (ecmdIsRingCacheEnabled(cacheTarget)){
     rc.setEcmdError(ECMD_RING_CACHE_ENABLED);
     return rc;
   }
#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapiGetMvpdField(i_record, i_keyword, i_procTarget, i_pBuffer, io_fieldSize);
#else
  if (fapiDllFnTable[ECMD_FAPIGETMVPDFIELD] == NULL) {
     fapiDllFnTable[ECMD_FAPIGETMVPDFIELD] = (void*)dlsym(dlHandle, "dllFapiGetMvpdField");
     if (fapiDllFnTable[ECMD_FAPIGETMVPDFIELD] == NULL) {
       fprintf(stderr,"dllFapiGetMVpdField%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const fapi::MvpdRecord, const fapi::MvpdKeyword, const fapi::Target &,  uint8_t *, uint32_t &) = 
      (ReturnCode(*)(const fapi::MvpdRecord, const fapi::MvpdKeyword, const fapi::Target &,  uint8_t *, uint32_t &))fapiDllFnTable[ECMD_FAPIGETMVPDFIELD];
  rc =    (*Function)(i_record, i_keyword, i_procTarget, i_pBuffer, io_fieldSize);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &l_ecmdRc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiGetMvpdField");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t fapiGetMvpdField(const fapi::MvpdRecord i_record, const fapi::MvpdKeyword i_keyword, const fapi::Target &i_procTarget, uint8_t * const i_pBuffer, uint32_t &io_fieldSize)",args);
   }
#endif

  return rc;

}

ReturnCode fapiSetMvpdField(const fapi::MvpdRecord i_record,
                            const fapi::MvpdKeyword i_keyword,
                            const fapi::Target &i_procTarget,
                            const uint8_t * const i_pBuffer,
                            const uint32_t i_fieldSize)
{

  ReturnCode rc;
  uint32_t l_ecmdRc;
  
  ecmdChipTarget   ecmdTarget;
  ecmdChipTarget * ecmdTargetPtr;
  ecmdTargetPtr = (ecmdChipTarget *) i_procTarget.get();
  ecmdTarget = (*ecmdTargetPtr);                

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiGetVpdField%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_record);
     args.push_back((void*) &i_keyword);
     args.push_back((void*) &i_procTarget);
     args.push_back((void*) &i_pBuffer);
     args.push_back((void*) &i_fieldSize);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t fapiSetMvpdField(const fapi::MvpdRecord i_record, const fapi::MvpdKeyword i_keyword, const fapi::Target &i_procTarget, const uint8_t * const i_pBuffer, const uint32_t i_fieldSize) )",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapiSetMvpdField");
  }
#endif

   ecmdChipTarget cacheTarget;
   cacheTarget = ecmdTarget;
   ecmdSetTargetDepth(cacheTarget, ECMD_DEPTH_CHIP);
   if (ecmdIsRingCacheEnabled(cacheTarget)){
     rc.setEcmdError(ECMD_RING_CACHE_ENABLED);
     return rc;
   }
#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapiSetMvpdField(i_record, i_keyword, i_procTarget, i_pBuffer, i_fieldSize);
#else
  if (fapiDllFnTable[ECMD_FAPISETMVPDFIELD] == NULL) {
     fapiDllFnTable[ECMD_FAPISETMVPDFIELD] = (void*)dlsym(dlHandle, "dllFapiSetMvpdField");
     if (fapiDllFnTable[ECMD_FAPISETMVPDFIELD] == NULL) {
       fprintf(stderr,"dllFapiSetMVpdField%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const fapi::MvpdRecord, const fapi::MvpdKeyword, const fapi::Target &,  const uint8_t *, const uint32_t ) = 
      (ReturnCode(*)(const fapi::MvpdRecord, const fapi::MvpdKeyword, const fapi::Target &,  const uint8_t *, const uint32_t ))fapiDllFnTable[ECMD_FAPISETMVPDFIELD];
  rc =    (*Function)(i_record, i_keyword, i_procTarget, i_pBuffer, i_fieldSize);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &l_ecmdRc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiSetMvpdField");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t fapiSetMvpdField(const fapi::MvpdRecord i_record, const fapi::MvpdKeyword i_keyword, const fapi::Target &i_procTarget, const uint8_t * const i_pBuffer, const uint32_t i_fieldSize)",args);
   }
#endif

  return rc;

}


ReturnCode fapiGetMBvpdField(const fapi::MBvpdRecord i_record,
                                  const fapi::MBvpdKeyword i_keyword,
                                  const fapi::Target &i_memBufTarget,
                                  uint8_t * const i_pBuffer,
                                  uint32_t &io_fieldSize)
{

  ReturnCode rc;
  uint32_t l_ecmdRc;
  
  ecmdChipTarget   ecmdTarget;
  ecmdChipTarget * ecmdTargetPtr;
  ecmdTargetPtr = (ecmdChipTarget *) i_memBufTarget.get();
  ecmdTarget = (*ecmdTargetPtr);                

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiGetMBvpdField%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_record);
     args.push_back((void*) &i_keyword);
     args.push_back((void*) &i_memBufTarget);
     args.push_back((void*) &i_pBuffer);
     args.push_back((void*) &io_fieldSize);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t fapiGetMBvpdField(const fapi::MBvpdRecord i_record, const fapi::MBvpdKeyword i_keyword, const fapi::Target &i_memBufTarget, uint8_t * const i_pBuffer, uint32_t &io_fieldSize) )",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapiGetMBvpdField");
  }
#endif

   ecmdChipTarget cacheTarget;
   cacheTarget = ecmdTarget;
   ecmdSetTargetDepth(cacheTarget, ECMD_DEPTH_CHIP);
   if (ecmdIsRingCacheEnabled(cacheTarget)){
     rc.setEcmdError(ECMD_RING_CACHE_ENABLED);
     return rc;
   }
#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapiGetMBvpdField(i_record, i_keyword, i_memBufTarget, i_pBuffer, io_fieldSize);
#else
  if (fapiDllFnTable[ECMD_FAPIGETMBVPDFIELD] == NULL) {
     fapiDllFnTable[ECMD_FAPIGETMBVPDFIELD] = (void*)dlsym(dlHandle, "dllFapiGetMBvpdField");
     if (fapiDllFnTable[ECMD_FAPIGETMBVPDFIELD] == NULL) {
       fprintf(stderr,"dllFapiGetMBvpdField%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const fapi::MBvpdRecord, const fapi::MBvpdKeyword, const fapi::Target &,  uint8_t *, uint32_t &) = 
      (ReturnCode(*)(const fapi::MBvpdRecord, const fapi::MBvpdKeyword, const fapi::Target &,  uint8_t *, uint32_t &))fapiDllFnTable[ECMD_FAPIGETMBVPDFIELD];
  rc =    (*Function)(i_record, i_keyword, i_memBufTarget, i_pBuffer, io_fieldSize);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &l_ecmdRc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiGetMBvpdField");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t fapiGetMBvpdField(const fapi::MBvpdRecord i_record, const fapi::MBvpdKeyword i_keyword, const fapi::Target &i_memBufTarget, uint8_t * const i_pBuffer, uint32_t &io_fieldSize)",args);
   }
#endif

  return rc;

}

ReturnCode fapiSetMBvpdField(
	const fapi::MBvpdRecord i_record,
	const fapi::MBvpdKeyword i_keyword,
	const fapi::Target &i_memBufTarget,
	const uint8_t * const i_pBuffer,
	const uint32_t i_fieldSize)
{

  ReturnCode rc;
  uint32_t l_ecmdRc;
  
  ecmdChipTarget   ecmdTarget;
  ecmdChipTarget * ecmdTargetPtr;
  ecmdTargetPtr = (ecmdChipTarget *) i_memBufTarget.get();
  ecmdTarget = (*ecmdTargetPtr);                

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiSetMBvpdField%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_record);
     args.push_back((void*) &i_keyword);
     args.push_back((void*) &i_memBufTarget);
     args.push_back((void*) &i_pBuffer);
     args.push_back((void*) &i_fieldSize);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t fapiSetMBvpdField(const fapi::MBvpdRecord i_record, const fapi::MBvpdKeyword i_keyword, const fapi::Target &i_memBufTarget, uint8_t * const i_pBuffer, const uint32_t i_fieldSize) )",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapiSetMBvpdField");
  }
#endif

   ecmdChipTarget cacheTarget;
   cacheTarget = ecmdTarget;
   ecmdSetTargetDepth(cacheTarget, ECMD_DEPTH_CHIP);
   if (ecmdIsRingCacheEnabled(cacheTarget)){
     rc.setEcmdError(ECMD_RING_CACHE_ENABLED);
     return rc;
   }
#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapiSetMBvpdField(i_record, i_keyword, i_memBufTarget, i_pBuffer, i_fieldSize);
#else
  if (fapiDllFnTable[ECMD_FAPISETMBVPDFIELD] == NULL) {
     fapiDllFnTable[ECMD_FAPISETMBVPDFIELD] = (void*)dlsym(dlHandle, "dllFapiSetMBvpdField");
     if (fapiDllFnTable[ECMD_FAPISETMBVPDFIELD] == NULL) {
       fprintf(stderr,"dllFapiSetMBvpdField%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const fapi::MBvpdRecord, const fapi::MBvpdKeyword, const fapi::Target &, const uint8_t *, uint32_t) = 
      (ReturnCode(*)(const fapi::MBvpdRecord, const fapi::MBvpdKeyword, const fapi::Target &, const uint8_t *, uint32_t))fapiDllFnTable[ECMD_FAPISETMBVPDFIELD];
  rc =    (*Function)(i_record, i_keyword, i_memBufTarget, i_pBuffer, i_fieldSize);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &l_ecmdRc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiSetMBvpdField");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t fapiSetMBvpdField(const fapi::MBvpdRecord i_record, const fapi::MBvpdKeyword i_keyword, const fapi::Target &i_memBufTarget, uint8_t * const i_pBuffer, uint32_t &i_fieldSize)",args);
   }
#endif

  return rc;

}



fapi::ReturnCode platGetSpy(const fapi::Target& i_target, const char * const i_spyId, ecmdDataBufferBase & o_data)
{

    ReturnCode rc;
    uint32_t l_ecmd_rc;

    ecmdDataBuffer l_ecmd_buffer;
    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target); 

#ifndef ECMD_STATIC_FUNCTIONS
    if (dlHandle == NULL) 
    {
	fprintf(stderr,"dllGetSpy%s",ECMD_DLL_NOT_LOADED_ERROR);
	exit(ECMD_DLL_INVALID);
    }
#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_target);
     args.push_back((void*) &i_spyId);
     args.push_back((void*) &l_ecmd_buffer);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t getSpy(ecmdChipTarget & i_target, const char * i_spyId, ecmdDataBuffer & l_ecmd_buffer)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"getSpy");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  l_ecmd_rc = dllGetSpy(l_ecmd_target, i_spyId, l_ecmd_buffer);
  if (l_ecmd_rc)
  {
    rc.setEcmdError(l_ecmd_rc); 
  }
#else
  if (DllFnTable[ECMD_GETSPY] == NULL) {
     DllFnTable[ECMD_GETSPY] = (void*)dlsym(dlHandle, "dllGetSpy");
     if (DllFnTable[ECMD_GETSPY] == NULL) {
       fprintf(stderr,"dllGetSpy%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)(ecmdChipTarget &,  const char * ,  ecmdDataBuffer &) = 
      (uint32_t(*)(ecmdChipTarget &,  const char * ,  ecmdDataBuffer &))DllFnTable[ECMD_GETSPY];
  l_ecmd_rc =    (*Function)(l_ecmd_target, i_spyId, l_ecmd_buffer);
  if (l_ecmd_rc)
  {
    rc.setEcmdError(l_ecmd_rc); 
  }

#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &l_ecmd_rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"getSpy");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t getSpy(ecmdChipTarget & i_target, const char * i_spyName, ecmdDataBuffer & l_ecmd_buffer)",args);
   }
#endif

  if (l_ecmd_rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(l_ecmd_rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  // Copy the data over
  if (rc.ok())
  {
    o_data = l_ecmd_buffer;
  }

  return rc;
}

fapi::ReturnCode platPutSpy(const fapi::Target& i_target,
                            const char *  const i_spyId,
                            ecmdDataBufferBase & i_data)
{
    ReturnCode rc;
    uint32_t l_ecmd_rc;

    ecmdDataBuffer l_ecmd_buffer;
    l_ecmd_buffer = i_data;

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target); 

#ifndef ECMD_STATIC_FUNCTIONS
    if (dlHandle == NULL) 
    {
	fprintf(stderr,"dllPutSpy%s",ECMD_DLL_NOT_LOADED_ERROR);
	exit(ECMD_DLL_INVALID);
    }
#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_target);
     args.push_back((void*) &i_spyId);
     args.push_back((void*) &l_ecmd_buffer);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t putSpy(ecmdChipTarget & i_target, const char * i_spyId, ecmdDataBuffer & l_ecmd_buffer)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"putgetSpy");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  l_ecmd_rc = dllPutSpy(l_ecmd_target, i_spyId, l_ecmd_buffer);
  if (l_ecmd_rc)
  {
    rc.setEcmdError(l_ecmd_rc); 
  }
#else
  if (DllFnTable[ECMD_PUTSPY] == NULL) {
     DllFnTable[ECMD_PUTSPY] = (void*)dlsym(dlHandle, "dllPutSpy");
     if (DllFnTable[ECMD_PUTSPY] == NULL) {
       fprintf(stderr,"dllPutSpy%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)(ecmdChipTarget &,  const char * ,  ecmdDataBuffer &) = 
      (uint32_t(*)(ecmdChipTarget &,  const char * ,  ecmdDataBuffer &))DllFnTable[ECMD_PUTSPY];
  l_ecmd_rc =    (*Function)(l_ecmd_target, i_spyId, l_ecmd_buffer);
  if (l_ecmd_rc)
  {
    rc.setEcmdError(l_ecmd_rc); 
  }

#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &l_ecmd_rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"putSpy");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t putSpy(ecmdChipTarget & i_target, const char * i_spyName, ecmdDataBuffer & l_ecmd_buffer)",args);
   }
#endif

  if (l_ecmd_rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(l_ecmd_rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;
}

fapi::ReturnCode platPutSpyImage(const fapi::Target& i_target,
                                 const char* const i_spyId,
                                 const ecmdDataBufferBase & i_data,
                                 ecmdDataBufferBase & io_imageData)
{
    ReturnCode rc;
    uint32_t l_ecmd_rc;

    ecmdDataBuffer l_ecmd_buffer;
    l_ecmd_buffer = i_data;

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target); 

    ecmdDataBuffer l_ecmd_buffer_image_data;
    l_ecmd_buffer_image_data = io_imageData;

#ifndef ECMD_STATIC_FUNCTIONS
    if (dlHandle == NULL) 
    {
	fprintf(stderr,"dllPutSpy%s",ECMD_DLL_NOT_LOADED_ERROR);
	exit(ECMD_DLL_INVALID);
    }
#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_target);
     args.push_back((void*) &i_spyId);
     args.push_back((void*) &l_ecmd_buffer);
     args.push_back((void*) &l_ecmd_buffer_image_data);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t putSpyImage(ecmdChipTarget & i_target, const char * i_spyId, ecmdDataBuffer & l_ecmd_buffer)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"putSpyImage");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  l_ecmd_rc = dllPutSpyImage(l_ecmd_target, i_spyId, l_ecmd_buffer, l_ecmd_buffer_image_data);
  if (l_ecmd_rc)
  {
    rc.setEcmdError(l_ecmd_rc); 
  }
#else
  if (DllFnTable[ECMD_PUTSPYIMAGE] == NULL) {
     DllFnTable[ECMD_PUTSPYIMAGE] = (void*)dlsym(dlHandle, "dllPutSpyImage");
     if (DllFnTable[ECMD_PUTSPYIMAGE] == NULL) {
       fprintf(stderr,"dllPutSpyImage%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)(ecmdChipTarget &,  const char * ,  ecmdDataBuffer &, ecmdDataBuffer &) = 
      (uint32_t(*)(ecmdChipTarget &,  const char * ,  ecmdDataBuffer &, ecmdDataBuffer &))DllFnTable[ECMD_PUTSPYIMAGE];
  l_ecmd_rc =    (*Function)(l_ecmd_target, i_spyId, l_ecmd_buffer, l_ecmd_buffer_image_data);
  if (l_ecmd_rc)
  {
    rc.setEcmdError(l_ecmd_rc); 
  }

#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &l_ecmd_rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"putSpyImage");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t putSpyImage(ecmdChipTarget & i_target, const char * i_spyName, ecmdDataBuffer & l_ecmd_buffer, ecmdDataBuffer & l_ecmd_data_buffer_image_data)",args);
   }
#endif

  if (l_ecmd_rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(l_ecmd_rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;

}

fapi::ReturnCode platGetSpyImage(const fapi::Target& i_target,
                                 const char * const i_spyId,
                                 ecmdDataBufferBase & o_data,
                                 const ecmdDataBufferBase & i_imageData)
{
    ReturnCode rc;
    uint32_t l_ecmd_rc;

    ecmdDataBuffer l_ecmd_buffer;
    l_ecmd_buffer = o_data;

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target); 

    ecmdDataBuffer l_ecmd_buffer_image_data;
    l_ecmd_buffer_image_data = i_imageData;

#ifndef ECMD_STATIC_FUNCTIONS
    if (dlHandle == NULL) 
    {
	fprintf(stderr,"dllGettSpy%s",ECMD_DLL_NOT_LOADED_ERROR);
	exit(ECMD_DLL_INVALID);
    }
#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_target);
     args.push_back((void*) &i_spyId);
     args.push_back((void*) &l_ecmd_buffer);
     args.push_back((void*) &l_ecmd_buffer_image_data);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t getSpyImage(ecmdChipTarget & i_target, const char * i_spyId, ecmdDataBuffer & l_ecmd_buffer)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"getSpyImage");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  l_ecmd_rc = dllGetSpyImage(l_ecmd_target, i_spyId, l_ecmd_buffer, l_ecmd_buffer_image_data);
  if (l_ecmd_rc)
  {
    rc.setEcmdError(l_ecmd_rc); 
  }
#else
  if (DllFnTable[ECMD_GETSPYIMAGE] == NULL) {
     DllFnTable[ECMD_GETSPYIMAGE] = (void*)dlsym(dlHandle, "dllGetSpyImage");
     if (DllFnTable[ECMD_GETSPYIMAGE] == NULL) {
       fprintf(stderr,"dllGetSpyImage%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)(ecmdChipTarget &,  const char * ,  ecmdDataBuffer &, ecmdDataBuffer &) = 
      (uint32_t(*)(ecmdChipTarget &,  const char * ,  ecmdDataBuffer &, ecmdDataBuffer &))DllFnTable[ECMD_GETSPYIMAGE];
  l_ecmd_rc =    (*Function)(l_ecmd_target, i_spyId, l_ecmd_buffer, l_ecmd_buffer_image_data);
  if (l_ecmd_rc)
  {
    rc.setEcmdError(l_ecmd_rc); 
  }

#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &l_ecmd_rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"getSpyImage");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t getSpyImage(ecmdChipTarget & i_target, const char * i_spyName, ecmdDataBuffer & l_ecmd_buffer, ecmdDataBuffer & l_ecmd_data_buffer_image_data)",args);
   }
#endif

  if (l_ecmd_rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(l_ecmd_rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;


}




} //Namespace
