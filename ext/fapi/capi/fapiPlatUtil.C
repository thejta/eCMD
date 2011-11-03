// IBM_PROLOG_BEGIN_TAG 
// This is an automatically generated prolog. 
//  
// fipsrefactordoc src/hwpf/plat/fapiPlatUtil.C 1.3 
//  
// IBM CONFIDENTIAL 
//  
// OBJECT CODE ONLY SOURCE MATERIALS 
//  
// COPYRIGHT International Business Machines Corp. 2011 
// All Rights Reserved 
//  
// The source code for this program is not published or otherwise 
// divested of its trade secrets, irrespective of what has been 
// deposited with the U.S. Copyright Office. 
//  
// IBM_PROLOG_END_TAG 
/**
 *  @file platUtil.C
 *
 *  @brief Implements the fapiUtil.H utility functions.
 *
 *  Note that platform code must provide the implementation.
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 *                          mjjones     04/13/2011  Created.
 *
 */
#include <stdio.h>
#include <dlfcn.h>
#include <fapiReturnCodes.H>
#include <fapiReturnCode.H>

#include <ecmdClientCapi.H>
#include <ecmdSharedUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientEnums.H>
#include <ecmdUtils.H>
#include <fapiClientEnums.H>
#include <fapiPlatTrace.H>

#include <fapiDllCapi.H>
#include <ecmdDllCapi.H>


//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------

static const char ECMD_DLL_NOT_LOADED_ERROR[] = ": eCMD Function called before DLL has been loaded\n";
static const char ECMD_UNABLE_TO_FIND_FUNCTION_ERROR[] = ": Unable to find function, must be an invalid DLL - program aborting\n";


#ifndef ECMD_STATIC_FUNCTIONS
/* This is from ecmdClientCapiFunc.C */
extern void * dlHandle;
/* These are from fapiClientCapiFunc.C */
extern void * fapiDllFnTable[FAPI_NUMFUNCTIONS];
#endif
extern bool fapiInitialized;

using namespace fapi;
#ifndef ECMD_STRIP_DEBUG
extern int ecmdClientDebug;
extern int fppCallCount;
extern bool ecmdDebugOutput;
#endif






//******************************************************************************
// fapiAssert
//******************************************************************************
using namespace fapi;

extern "C"{
void fapiAssert(bool i_expression)
{
  //if (!i_expression) exit(FAPI_RC_ASSERT);
  if (!i_expression){ 
    FAPI_ERR("**** ASSERT condition hit ****");
    exit(ECMD_FAILURE);
  }
}

ReturnCode fapiDelay(uint64_t i_nanoSeconds, uint64_t i_simCycles){

  uint32_t rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiDelay%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif


if (!fapiInitialized) {
   fprintf(stderr,"dllFapiDelay: eCMD Extension not initialized before function called\n");
   fprintf(stderr,"dllFapiDelay: OR eCMD fapi Extension not supported by plugin\n");
   exit(ECMD_DLL_INVALID);
}

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_nanoSeconds);
     args.push_back((void*) &i_simCycles);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t fapiDelay(uint32_t i_simCycles, uint32_t i_msDelay)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapiDelay");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapiDelay(i_nanoSeconds, i_simCycles);
#else
  if (fapiDllFnTable[ECMD_FAPIDELAY] == NULL) {
     fapiDllFnTable[ECMD_FAPIDELAY] = (void*)dlsym(dlHandle, "dllFapiDelay");
     if (fapiDllFnTable[ECMD_FAPIDELAY] == NULL) {
       fprintf(stderr,"dllFapiDelay%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)(uint64_t,  uint64_t) = 
      (uint32_t(*)(uint64_t,  uint64_t))fapiDllFnTable[ECMD_FAPIDELAY];
  rc =    (*Function)(i_nanoSeconds, i_simCycles);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiDelay");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t fapiDelay(uint32_t i_simCycles, uint32_t i_msDelay)",args);
   }
#endif

  if (rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;
}


bool platIsScanTraceEnabled(){

  bool rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllPlatIsScanTraceEnabled%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

if (!fapiInitialized) {
   fprintf(stderr,"dllPlatIsScanTraceEnabled: eCMD Extension not initialized before function called\n");
   fprintf(stderr,"dllPlatIsScanTraceEnabled: OR eCMD fapi Extension not supported by plugin\n");
   exit(ECMD_DLL_INVALID);
}

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"platIsScanTraceEnabled");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllPlatIsScanTraceEnabled();
#else
  if (fapiDllFnTable[ECMD_PLATISSCANTRACEENABLED] == NULL) {
     fapiDllFnTable[ECMD_PLATISSCANTRACEENABLED] = (void*)dlsym(dlHandle, "dllPlatIsScanTraceEnabled");
     if (fapiDllFnTable[ECMD_PLATISSCANTRACEENABLED] == NULL) {
       fprintf(stderr,"dllPlatIsScanTraceEnabled%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  bool (*Function)() = 
      (bool (*)())fapiDllFnTable[ECMD_PLATISSCANTRACEENABLED];
  rc =    (*Function)();
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"platIsScanTraceEnabled");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"", args);
   }
#endif

  if (rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;
}

void platSetScanTrace(bool i_enable){

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllPlatSetScanTrace%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

if (!fapiInitialized) {
   fprintf(stderr,"dllPlatSetScanTrace: eCMD Extension not initialized before function called\n");
   fprintf(stderr,"dllPlatSetScanTrace: OR eCMD fapi Extension not supported by plugin\n");
   exit(ECMD_DLL_INVALID);
}

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_enable);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"void platSetScanTrace(bool i_enable)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"platSetScanTrace");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  dllPlatSetScanTrace(i_enable);
#else
  if (fapiDllFnTable[ECMD_PLATSETSCANTRACE] == NULL) {
     fapiDllFnTable[ECMD_PLATSETSCANTRACE] = (void*)dlsym(dlHandle, "dllPlatSetScanTrace");
     if (fapiDllFnTable[ECMD_PLATSETSCANTRACE] == NULL) {
       fprintf(stderr,"dllPlatSetScanTrace%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

   void (*Function)(bool) = 
      (void (*)(bool))fapiDllFnTable[ECMD_PLATSETSCANTRACE];
      (*Function)(i_enable);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"platSetScanTrace");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"void platSetScanTrace(bool i_enable)",args);
   }
#endif

  if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(0x1, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }
}

void fapiLogError(fapi::ReturnCode & io_rc) {

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiLogError%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

if (!fapiInitialized) {
   fprintf(stderr,"dllFapiLogError: eCMD Extension not initialized before function called\n");
   fprintf(stderr,"dllFapiLogError: OR eCMD fapi Extension not supported by plugin\n");
   exit(ECMD_DLL_INVALID);
}

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &io_rc);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"void fapiLogError(fapi::ReturnCode & io_rc)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapiLogError");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  dllFapiLogError(io_rc);
#else
  if (fapiDllFnTable[ECMD_FAPILOGERROR] == NULL) {
     fapiDllFnTable[ECMD_FAPILOGERROR] = (void*)dlsym(dlHandle, "dllFapiLogError");
     if (fapiDllFnTable[ECMD_FAPILOGERROR] == NULL) {
       fprintf(stderr,"dllFapiLogError%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

   void (*Function)(fapi::ReturnCode &) = 
      (void (*)(fapi::ReturnCode &))fapiDllFnTable[ECMD_FAPILOGERROR];
      (*Function)(io_rc);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiLogError");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"void fapiLogError(fapi::ReturnCode & io_rc)",args);
   }
#endif

  #if 0
  if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(0x1, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }
  #endif
}


fapi::ReturnCode fapiLoadInitFile(const char * i_file, const char *& o_addr, size_t & o_size){
  fapi::ReturnCode rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiLoadInitFile%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif


if (!fapiInitialized) {
   fprintf(stderr,"dllFapiLoadInitFile: eCMD Extension not initialized before function called\n");
   fprintf(stderr,"dllFapiLoadInitFile: OR eCMD fapi Extension not supported by plugin\n");
   exit(ECMD_DLL_INVALID);
}

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_file);
     args.push_back((void*) &o_addr);
     args.push_back((void*) &o_size );
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"fapi::ReturnCode fapiLoadInitFile(const char * i_file, const char *& o_addr, size_t & o_size)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapiLoadInitFile");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapiLoadInitFile(i_file, o_addr, o_size);
#else
  if (fapiDllFnTable[ECMD_FAPILOADINITFILE] == NULL) {
     fapiDllFnTable[ECMD_FAPILOADINITFILE] = (void*)dlsym(dlHandle, "dllFapiLoadInitFile");
     if (fapiDllFnTable[ECMD_FAPILOADINITFILE] == NULL) {
       fprintf(stderr,"dllFapiLoadInitFile%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const char *,  const char *&, size_t&) = 
      (ReturnCode(*)(const char *,  const char *&, size_t&))fapiDllFnTable[ECMD_FAPILOADINITFILE];
  rc =    (*Function)(i_file, o_addr, o_size);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiLoadInitFile");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"fapi::ReturnCode fapiLoadInitFile(const char * i_file, const char *& o_addr, size_t & o_size)",args);
   }
#endif

  if (rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;
}

fapi::ReturnCode fapiUnloadInitFile(const char * i_file, const char *& io_addr, size_t & io_size){
  fapi::ReturnCode rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiUnloadInitFile%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif


if (!fapiInitialized) {
   fprintf(stderr,"dllFapiUnloadInitFile: eCMD Extension not initialized before function called\n");
   fprintf(stderr,"dllFapiUnloadInitFile: OR eCMD fapi Extension not supported by plugin\n");
   exit(ECMD_DLL_INVALID);
}

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_file);
     args.push_back((void*) &io_addr);
     args.push_back((void*) &io_size );
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"fapi::ReturnCode fapiUnloadInitFile(const char * i_file, const char *& io_addr, size_t & io_size)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapiUnloadInitFile");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapiUnloadInitFile(i_file, io_addr, io_size);
#else
  if (fapiDllFnTable[ECMD_FAPIUNLOADINITFILE] == NULL) {
     fapiDllFnTable[ECMD_FAPIUNLOADINITFILE] = (void*)dlsym(dlHandle, "dllFapiUnloadInitFile");
     if (fapiDllFnTable[ECMD_FAPIUNLOADINITFILE] == NULL) {
       fprintf(stderr,"dllFapiUnloadInitFile%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const char *,  const char *&, size_t&) = 
      (ReturnCode(*)(const char *,  const char *&, size_t&))fapiDllFnTable[ECMD_FAPIUNLOADINITFILE];
  rc =    (*Function)(i_file, io_addr, io_size);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiUnloadInitFile");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"fapi::ReturnCode fapiUnloadInitFile(const char * i_file, const char *& io_addr, size_t & io_size)",args);
   }
#endif

  if (rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;
}



} //end extern
