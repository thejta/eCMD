// IBM_PROLOG_BEGIN_TAG 
// This is an automatically generated prolog. 
//  
// fipsrefactordoc src/hwpf/plat/fapi2PlatUtil.C 1.3 
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
#include <return_code.H>

#include <ecmdClientCapi.H>
#include <ecmdSharedUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientEnums.H>
#include <ecmdUtils.H>
#include <fapi2ClientEnums.H>
#include <fapi2PlatTrace.H>
#include <fapi2Util.H>

#include <fapi2DllCapi.H>
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
extern void * fapi2DllFnTable[FAPI2_NUMFUNCTIONS];
#endif

extern bool fapi2Initialized;

#ifndef ECMD_STRIP_DEBUG
extern int ecmdClientDebug;
extern int fppCallCount;
extern bool ecmdDebugOutput;
#endif






//******************************************************************************
// fapiAssert
//******************************************************************************
namespace fapi2
{
    void assert(bool i_expression)
    {
        //if (!i_expression) exit(FAPI_RC_ASSERT);
        if (!i_expression)
        {
            //FAPI_ERR("**** ASSERT condition hit ****"); // JFDEBUG this probably should be added back in. -farrugia
            exit(ECMD_FAILURE);
        }
    }

    ReturnCode delay(uint64_t i_nanoSeconds, uint64_t i_simCycles)
    {

        uint32_t rc = ECMD_SUCCESS;
        ReturnCode l_fapiRc(FAPI2_RC_SUCCESS);
        //l_fapiRc.setEcmdError(ECMD_SUCCESS);

#ifndef ECMD_STATIC_FUNCTIONS
        if (dlHandle == NULL)
        {
            fprintf(stderr,"dllFapi2Delay%s",ECMD_DLL_NOT_LOADED_ERROR);
            exit(ECMD_DLL_INVALID);
        }
#endif

        if (!fapi2Initialized)
        {
            fprintf(stderr,"dllFapi2Delay: eCMD Extension not initialized before function called\n");
            fprintf(stderr,"dllFapi2Delay: OR eCMD fapi Extension not supported by plugin\n");
            exit(ECMD_DLL_INVALID);
        }

#ifndef ECMD_STRIP_DEBUG
        int myTcount;
        std::vector< void * > args;
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &i_nanoSeconds);
            args.push_back((void*) &i_simCycles);
            fppCallCount++;
            myTcount = fppCallCount;
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t fapi2::delay(uint32_t i_simCycles, uint32_t i_msDelay)",args);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapi2::delay");
        }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
        rc = dllFapi2Delay(i_nanoSeconds, i_simCycles);
#else
        if (fapi2DllFnTable[ECMD_FAPI2DELAY] == NULL)
        {
            fapi2DllFnTable[ECMD_FAPI2DELAY] = (void*)dlsym(dlHandle, "dllFapi2Delay");
            if (fapi2DllFnTable[ECMD_FAPI2DELAY] == NULL)
            {
                fprintf(stderr,"dllFapi2Delay%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
                ecmdDisplayDllInfo();
                exit(ECMD_DLL_INVALID);
            }
        }

        uint32_t (*Function)(uint64_t, uint64_t) = 
            (uint32_t(*)(uint64_t, uint64_t))fapi2DllFnTable[ECMD_FAPI2DELAY];
        rc = (*Function)(i_nanoSeconds, i_simCycles);
#endif

#ifndef ECMD_STRIP_DEBUG
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &rc);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapi2::delay");
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t fapi2::delay(uint32_t i_simCycles, uint32_t i_msDelay)",args);
        }
#endif

        if (rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE))
        {
            std::string errorString;
            errorString = ecmdGetErrorMsg(rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
            if (errorString.size()) ecmdOutput(errorString.c_str());
        }

        l_fapiRc = (ReturnCodes)rc;
        return l_fapiRc;
    }


    bool platIsScanTraceEnabled()
    {

        bool rc;

#ifndef ECMD_STATIC_FUNCTIONS
        if (dlHandle == NULL)
        {
            fprintf(stderr,"dllFapi2PlatIsScanTraceEnabled%s",ECMD_DLL_NOT_LOADED_ERROR);
            exit(ECMD_DLL_INVALID);
        }
#endif

        if (!fapi2Initialized)
        {
            fprintf(stderr,"dllFapi2PlatIsScanTraceEnabled: eCMD Extension not initialized before function called\n");
            fprintf(stderr,"dllFapi2PlatIsScanTraceEnabled: OR eCMD fapi Extension not supported by plugin\n");
            exit(ECMD_DLL_INVALID);
        }

#ifndef ECMD_STRIP_DEBUG
        int myTcount;
        std::vector< void * > args;
        if (ecmdClientDebug != 0)
        {
            fppCallCount++;
            myTcount = fppCallCount;
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"",args);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapi2::platIsScanTraceEnabled");
        }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
        rc = dllFapi2PlatIsScanTraceEnabled();
#else
        if (fapi2DllFnTable[ECMD_FAPI2PLATISSCANTRACEENABLED] == NULL)
        {
            fapi2DllFnTable[ECMD_FAPI2PLATISSCANTRACEENABLED] = (void*)dlsym(dlHandle, "dllFapi2PlatIsScanTraceEnabled");
            if (fapi2DllFnTable[ECMD_FAPI2PLATISSCANTRACEENABLED] == NULL)
            {
                fprintf(stderr,"dllFapi2PlatIsScanTraceEnabled%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
                ecmdDisplayDllInfo();
                exit(ECMD_DLL_INVALID);
            }
        }

        bool (*Function)() = 
            (bool (*)())fapi2DllFnTable[ECMD_FAPI2PLATISSCANTRACEENABLED];
        rc = (*Function)();
#endif

#ifndef ECMD_STRIP_DEBUG
        if (ecmdClientDebug != 0)
        {
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapi2::platIsScanTraceEnabled");
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"", args);
        }
#endif

        if (rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE))
        {
            std::string errorString;
            errorString = ecmdGetErrorMsg(rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
            if (errorString.size()) ecmdOutput(errorString.c_str());
        }

        return rc;
    }

    void platSetScanTrace(bool i_enable)
    {

#ifndef ECMD_STATIC_FUNCTIONS
        if (dlHandle == NULL)
        {
            fprintf(stderr,"dllFapi2PlatSetScanTrace%s",ECMD_DLL_NOT_LOADED_ERROR);
            exit(ECMD_DLL_INVALID);
        }
#endif

        if (!fapi2Initialized)
        {
            fprintf(stderr,"dllFapi2PlatSetScanTrace: eCMD Extension not initialized before function called\n");
            fprintf(stderr,"dllFapi2PlatSetScanTrace: OR eCMD fapi Extension not supported by plugin\n");
            exit(ECMD_DLL_INVALID);
        }

#ifndef ECMD_STRIP_DEBUG
        int myTcount;
        std::vector< void * > args;
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &i_enable);
            fppCallCount++;
            myTcount = fppCallCount;
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"void fapi2::platSetScanTrace(bool i_enable)",args);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapi2::platSetScanTrace");
        }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
        dllFapi2PlatSetScanTrace(i_enable);
#else
        if (fapi2DllFnTable[ECMD_FAPI2PLATSETSCANTRACE] == NULL)
        {
            fapi2DllFnTable[ECMD_FAPI2PLATSETSCANTRACE] = (void*)dlsym(dlHandle, "dllFapi2PlatSetScanTrace");
            if (fapi2DllFnTable[ECMD_FAPI2PLATSETSCANTRACE] == NULL)
            {
                fprintf(stderr,"dllFapi2PlatSetScanTrace%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
                ecmdDisplayDllInfo();
                exit(ECMD_DLL_INVALID);
            }
        }

        void (*Function)(bool) = 
            (void (*)(bool))fapi2DllFnTable[ECMD_FAPI2PLATSETSCANTRACE];
        (*Function)(i_enable);
#endif

#ifndef ECMD_STRIP_DEBUG
        if (ecmdClientDebug != 0)
        {
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapi2::platSetScanTrace");
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"void fapi2::platSetScanTrace(bool i_enable)",args);
        }
#endif

        if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE))
        {
            std::string errorString;
            errorString = ecmdGetErrorMsg(0x1, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
            if (errorString.size()) ecmdOutput(errorString.c_str());
        }
    }

    void logError(ReturnCode & io_rc,
                  fapiErrlSeverity_t i_sev,
                  bool i_unitTestError)
    {

#ifndef ECMD_STATIC_FUNCTIONS
        if (dlHandle == NULL)
        {
            fprintf(stderr,"dllFapi2LogError%s",ECMD_DLL_NOT_LOADED_ERROR);
            exit(ECMD_DLL_INVALID);
        }
#endif

        if (!fapi2Initialized)
        {
            fprintf(stderr,"dllFapi2LogError: eCMD Extension not initialized before function called\n");
            fprintf(stderr,"dllFapi2LogError: OR eCMD fapi Extension not supported by plugin\n");
            exit(ECMD_DLL_INVALID);
        }

#ifndef ECMD_STRIP_DEBUG
        int myTcount;
        std::vector< void * > args;
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &io_rc);
            args.push_back((void*) &i_sev);
            args.push_back((void*) &i_unitTestError);
            fppCallCount++;
            myTcount = fppCallCount;
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"void fapi2::logError(fapi2::ReturnCode & io_rc, fapi2::fapiErrlSeverity_t i_sev, bool i_unitTestError)",args);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapi2::logError");
        }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
        dllFapi2LogError(io_rc, i_sev, i_unitTestError);
#else
        if (fapi2DllFnTable[ECMD_FAPI2LOGERROR] == NULL)
        {
            fapi2DllFnTable[ECMD_FAPI2LOGERROR] = (void*)dlsym(dlHandle, "dllFapi2LogError");
            if (fapi2DllFnTable[ECMD_FAPI2LOGERROR] == NULL)
            {
                fprintf(stderr,"dllFapi2LogError%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
                ecmdDisplayDllInfo();
                exit(ECMD_DLL_INVALID);
            }
        }

        void (*Function)(ReturnCode &, fapiErrlSeverity_t, bool) = 
            (void (*)(ReturnCode &, fapiErrlSeverity_t, bool))fapi2DllFnTable[ECMD_FAPI2LOGERROR];
        (*Function)(io_rc, i_sev, i_unitTestError);
#endif

#ifndef ECMD_STRIP_DEBUG
        if (ecmdClientDebug != 0)
        {
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapi2::logError");
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"void fapi2::logError(fapi2::ReturnCode & io_rc, fapi2::fapiErrlSeverity_t i_sev, bool i_unitTestError)",args);
        }
#endif

#if 0
        if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE))
        {
            std::string errorString;
            errorString = ecmdGetErrorMsg(0x1, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
            if (errorString.size()) ecmdOutput(errorString.c_str());
        }
#endif
}

#if 0

ReturnCode loadInitFile(const Target & i_Target, const char * i_file, const char *& o_addr, size_t & o_size){
  ReturnCode rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapi2LoadInitFile%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif


if (!fapi2Initialized) {
   fprintf(stderr,"dllFapi2LoadInitFile: eCMD Extension not initialized before function called\n");
   fprintf(stderr,"dllFapi2LoadInitFile: OR eCMD fapi Extension not supported by plugin\n");
   exit(ECMD_DLL_INVALID);
}

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_Target);
     args.push_back((void*) &i_file);
     args.push_back((void*) &o_addr);
     args.push_back((void*) &o_size );
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"fapi::ReturnCode fapi::loadInitFile(const fapi::Target & i_Target, const char * i_file, const char *& o_addr, size_t & o_size)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapi::loadInitFile");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapi2LoadInitFile(i_Target, i_file, o_addr, o_size);
#else
  if (fapi2DllFnTable[ECMD_FAPI2LOADINITFILE] == NULL) {
     fapi2DllFnTable[ECMD_FAPI2LOADINITFILE] = (void*)dlsym(dlHandle, "dllFapi2LoadInitFile");
     if (fapi2DllFnTable[ECMD_FAPI2LOADINITFILE] == NULL) {
       fprintf(stderr,"dllFapi2LoadInitFile%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const fapi::Target &, const char *,  const char *&, size_t&) = 
      (ReturnCode(*)(const fapi::Target &, const char *,  const char *&, size_t&))fapi2DllFnTable[ECMD_FAPI2LOADINITFILE];
  rc =    (*Function)(i_Target, i_file, o_addr, o_size);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapi::loadInitFile");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"fapi::ReturnCode fapi::loadInitFile(const char * i_file, const char *& o_addr, size_t & o_size)",args);
   }
#endif

  if (rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;
}

fapi::ReturnCode unloadInitFile(const char * i_file, const char *& io_addr, size_t & io_size){
  fapi::ReturnCode rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapi2UnloadInitFile%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif


if (!fapi2Initialized) {
   fprintf(stderr,"dllFapi2UnloadInitFile: eCMD Extension not initialized before function called\n");
   fprintf(stderr,"dllFapi2UnloadInitFile: OR eCMD fapi Extension not supported by plugin\n");
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
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"fapi::ReturnCode fapi::unloadInitFile(const char * i_file, const char *& io_addr, size_t & io_size)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapi::unloadInitFile");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapi2UnloadInitFile(i_file, io_addr, io_size);
#else
  if (fapi2DllFnTable[ECMD_FAPI2UNLOADINITFILE] == NULL) {
     fapi2DllFnTable[ECMD_FAPI2UNLOADINITFILE] = (void*)dlsym(dlHandle, "dllFapi2UnloadInitFile");
     if (fapi2DllFnTable[ECMD_FAPI2UNLOADINITFILE] == NULL) {
       fprintf(stderr,"dllFapi2UnloadInitFile%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const char *,  const char *&, size_t&) = 
      (ReturnCode(*)(const char *,  const char *&, size_t&))fapi2DllFnTable[ECMD_FAPI2UNLOADINITFILE];
  rc =    (*Function)(i_file, io_addr, io_size);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapi::unloadInitFile");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"fapi::ReturnCode fapi::unloadInitFile(const char * i_file, const char *& io_addr, size_t & io_size)",args);
   }
#endif

  if (rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;
}

fapi::ReturnCode specialWakeup(const fapi::Target & i_target, const bool i_enable)
{
    fapi::ReturnCode rc;

#ifndef ECMD_STATIC_FUNCTIONS
    if (dlHandle == NULL) {
	fprintf(stderr,"dllFapi2SpecialWakeup%s",ECMD_DLL_NOT_LOADED_ERROR);
	exit(ECMD_DLL_INVALID);
    }
#endif


    if (!fapi2Initialized) {
	fprintf(stderr,"dllFapi2SpecialWakeup: eCMD Extension not initialized before function called\n");
	fprintf(stderr,"dllFapi2SpecialWakeup: OR eCMD fapi Extension not supported by plugin\n");
	exit(ECMD_DLL_INVALID);
    }

#ifndef ECMD_STRIP_DEBUG
    int myTcount;
    std::vector< void * > args;
    if (ecmdClientDebug != 0) {
	args.push_back((void*) &i_target);
	args.push_back((void*) &i_enable);
	fppCallCount++;
	myTcount = fppCallCount;
	ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"fapi::ReturnCode fapi::specialWakeup(const fapi::Target & i_target, const bool i_enable)",args);
	ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapi::specialWakeup");
    }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
    rc = dllFapi2SpecialWakeup(i_target, i_enable);
#else
    if (fapi2DllFnTable[ECMD_FAPI2SPECIALWAKEUP] == NULL) {
	fapi2DllFnTable[ECMD_FAPI2SPECIALWAKEUP] = (void*)dlsym(dlHandle, "dllFapi2SpecialWakeup");
	if (fapi2DllFnTable[ECMD_FAPI2SPECIALWAKEUP] == NULL) {
	    fprintf(stderr,"dllFapi2SpecialWakeup%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
	    ecmdDisplayDllInfo();
	    exit(ECMD_DLL_INVALID);
	}
    }

    ReturnCode (*Function)(const Target &,  const bool ) = 
	(ReturnCode(*)(const Target &,  const bool ))fapi2DllFnTable[ECMD_FAPI2SPECIALWAKEUP];
    rc =    (*Function)(i_target, i_enable);
#endif

#ifndef ECMD_STRIP_DEBUG
    if (ecmdClientDebug != 0) {
	args.push_back((void*) &rc);
	ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapi::specialWakeup");
	ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"fapi::ReturnCode fapi::specialWakeup(const fapi::Target & i_target , const bool i_enable)",args);
    }
#endif

    if (rc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE)) {
	std::string errorString;
	errorString = ecmdGetErrorMsg(rc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
	if (errorString.size()) ecmdOutput(errorString.c_str());
    }

    return rc;
}
#endif



} // namespace fapi2
