/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source$                                 */
/*                                                                        */
/* IBM CONFIDENTIAL                                                       */
/*                                                                        */
/* EKB Project                                                            */
/*                                                                        */
/* COPYRIGHT 2011,2015                                                    */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* The source code for this program is not published or otherwise         */
/* divested of its trade secrets, irrespective of what has been           */
/* deposited with the U.S. Copyright Office.                              */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
/**
 *  @file plat_utils.C
 *  @brief Implements fapi2 common utilities
 */

#include <stdint.h>
#include <plat_trace.H>
#include <return_code.H>
#include <error_info.H>
#include <assert.h>

#include <dlfcn.h>

#include <ecmdUtils.H>

#include <fapi2ClientEnums.H>
#include <fapi2DllCapi.H>

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

namespace fapi2
{
    ///
    /// @brief Log an error.
    ///
    void logError(
        fapi2::ReturnCode & io_rc,
        fapi2::errlSeverity_t i_sev,
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
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"void fapi2::logError(fapi2::ReturnCode & io_rc, fapi2::errlSeverity_t i_sev, bool i_unitTestError)",args);
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

        void (*Function)(ReturnCode &, errlSeverity_t, bool) = 
            (void (*)(ReturnCode &, errlSeverity_t, bool))fapi2DllFnTable[ECMD_FAPI2LOGERROR];
        (*Function)(io_rc, i_sev, i_unitTestError);
#endif

#ifndef ECMD_STRIP_DEBUG
        if (ecmdClientDebug != 0)
        {
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapi2::logError");
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"void fapi2::logError(fapi2::ReturnCode & io_rc, fapi2::errlSeverity_t i_sev, bool i_unitTestError)",args);
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
        // Release the ffdc information now that we're done with it.
        io_rc.forgetData();
    }

    ///
    /// @brief Delay this thread.
    ///
    ReturnCode delay(uint64_t i_nanoSeconds, uint64_t i_simCycles, bool i_fixed)
    {
        uint32_t rc = ECMD_SUCCESS;
        ReturnCode l_fapiRc(FAPI2_RC_SUCCESS);

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
            args.push_back((void*) &i_fixed);
            fppCallCount++;
            myTcount = fppCallCount;
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"fapi2::ReturnCode fapi2::delay(uint64_t i_nanoSeconds, uint64_t i_simCycles, bool i_fixed)",args);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapi2::delay");
        }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
        rc = dllFapi2Delay(i_nanoSeconds, i_simCycles, i_fixed);
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

        uint32_t (*Function)(uint64_t, uint64_t, bool) = 
            (uint32_t(*)(uint64_t, uint64_t, bool))fapi2DllFnTable[ECMD_FAPI2DELAY];
        rc = (*Function)(i_nanoSeconds, i_simCycles, i_fixed);
#endif

#ifndef ECMD_STRIP_DEBUG
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &rc);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapi2::delay");
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"fapi2::ReturnCode fapi2::delay(uint64_t i_nanoSeconds, uint64_t i_simCycles, bool i_fixed)",args);
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

    ///
    /// @brief Assert a condition, and halt
    ///
    /// @param[in] a boolean representing the assertion
    ///
    void Assert(bool i_expression)
    {
        assert(i_expression);
    }

};

namespace fapi2plat
{
    bool isScanTraceEnabled()
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
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapi2plat::isScanTraceEnabled");
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
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapi2plat::isScanTraceEnabled");
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

    void setScanTrace(bool i_enable)
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
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"void fapi2plat::setScanTrace(bool i_enable)",args);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapi2plat::setScanTrace");
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
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapi2plat::setScanTrace");
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"void fapi2plat::setScanTrace(bool i_enable)",args);
        }
#endif

        if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE))
        {
            std::string errorString;
            errorString = ecmdGetErrorMsg(0x1, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
            if (errorString.size()) ecmdOutput(errorString.c_str());
        }
    }
};
