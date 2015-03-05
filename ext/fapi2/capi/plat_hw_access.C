// Copyright ***********************************************************
//                                                                        
// File plat_hw_access.C                                   
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
#include <hw_access.H> 

#include <stdio.h>

#include <ecmdClientCapi.H>
#include <ecmdSharedUtils.H>
#include <ecmdClientEnums.H>
#include <fapi2ClientCapi.H>

//#include <fapiMvpdAccess.H> 
#include <fapi2SharedUtils.H> 

#ifndef ECMD_STATIC_FUNCTIONS
#include <fapi2ClientEnums.H>
#include <target.H>
#endif

#include <fapi2DllCapi.H>
#include <ecmdDllCapi.H>

#include <ecmdUtils.H>

static const char ECMD_DLL_NOT_LOADED_ERROR[] = ": eCMD Function called before DLL has been loaded\n";
static const char ECMD_UNABLE_TO_FIND_FUNCTION_ERROR[] = ": Unable to find function, must be an invalid DLL - program aborting\n";

#ifndef ECMD_STATIC_FUNCTIONS

#include <dlfcn.h>

/* This is from ecmdClientCapiFunc.C */
extern void * dlHandle;
extern void * DllFnTable[];
extern void * fapi2DllFnTable[];

#else

#include <fapi2DllCapi.H>

#endif


extern bool fapi2Initialized;
#ifndef ECMD_STRIP_DEBUG
extern int ecmdClientDebug;
extern int fppCallCount;
extern bool ecmdDebugOutput;
#endif

namespace fapi2
{

    ReturnCode platGetScom(ecmdChipTarget& i_target,
                           const uint64_t i_address,
                           buffer<uint64_t>& o_data)
    {
        ReturnCode rc(FAPI2_RC_SUCCESS);
        uint32_t l_ecmdRc;

        ecmdDataBuffer l_ecmd_buffer;

#ifndef ECMD_STATIC_FUNCTIONS
        if (dlHandle == NULL)
        {
            fprintf(stderr,"dllGetScom%s",ECMD_DLL_NOT_LOADED_ERROR);
            exit(ECMD_DLL_INVALID);
        }
#endif

#ifndef ECMD_STRIP_DEBUG
        int myTcount;
        std::vector< void * > args;
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &i_target);
            args.push_back((void*) &i_address);
            args.push_back((void*) &l_ecmd_buffer);
            fppCallCount++;
            myTcount = fppCallCount;
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t getScom(ecmdChipTarget & i_target, uint64_t i_address, ecmdDataBuffer & o_data)",args);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"getScom");
        }
#endif

        ecmdChipTarget cacheTarget;
        cacheTarget = i_target;
        ecmdSetTargetDepth(cacheTarget, ECMD_DEPTH_CHIP);
        if (ecmdIsRingCacheEnabled(cacheTarget))
        {
            rc = (ReturnCodes) ECMD_RING_CACHE_ENABLED;
            return rc;
        }
#ifdef ECMD_STATIC_FUNCTIONS
        l_ecmdRc = dllGetScom(i_target, i_address, l_ecmd_buffer); 
        if (l_ecmdRc)
        {
            rc = (ReturnCodes) l_ecmdRc;
        }
#else
        if (DllFnTable[ECMD_GETSCOM] == NULL)
        {
            DllFnTable[ECMD_GETSCOM] = (void*)dlsym(dlHandle, "dllGetScom");
            if (DllFnTable[ECMD_GETSCOM] == NULL)
            {
                fprintf(stderr,"dllGetScom%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
                ecmdDisplayDllInfo();
                exit(ECMD_DLL_INVALID);
            }
        }

        uint32_t (*Function)(ecmdChipTarget &,  uint64_t,  ecmdDataBuffer &) = 
            (uint32_t(*)(ecmdChipTarget &,  uint64_t,  ecmdDataBuffer &))DllFnTable[ECMD_GETSCOM];
        l_ecmdRc = (*Function)(i_target, i_address, l_ecmd_buffer);
        if (l_ecmdRc)
        {
            rc = (ReturnCodes) l_ecmdRc; 
        }
#endif

#ifndef ECMD_STRIP_DEBUG
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &l_ecmdRc);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"getScom");
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t getScom(ecmdChipTarget & i_target, uint64_t i_address, ecmdDataBuffer & o_data)",args);
        }
#endif

        if (l_ecmdRc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE))
        {
            std::string errorString;
            errorString = ecmdGetErrorMsg(l_ecmdRc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
            if (errorString.size()) ecmdOutput(errorString.c_str());
        }

        o_data = l_ecmd_buffer.getDoubleWord(0);

        return rc;
    }

    ReturnCode platPutScom(ecmdChipTarget & i_target, const uint64_t i_address,  fapi2::buffer<uint64_t> & i_data) 
    {
        ReturnCode rc;
        uint32_t l_ecmdRc;

        ecmdDataBuffer l_ecmd_buffer(64);
        l_ecmd_buffer.setDoubleWord(0, i_data);

#ifndef ECMD_STATIC_FUNCTIONS
        if (dlHandle == NULL)
        {
            fprintf(stderr,"dllPutScom%s",ECMD_DLL_NOT_LOADED_ERROR);
            exit(ECMD_DLL_INVALID);
        }
#endif

#ifndef ECMD_STRIP_DEBUG
        int myTcount;
        std::vector< void * > args;
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &i_target);
            args.push_back((void*) &i_address);
            args.push_back((void*) &l_ecmd_buffer);
            fppCallCount++;
            myTcount = fppCallCount;
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t putScom(ecmdChipTarget & i_target, uint64_t i_address, ecmdDataBufferBase & i_data)",args);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"putScom");
        }
#endif

        ecmdChipTarget cacheTarget;
        cacheTarget = i_target;
        ecmdSetTargetDepth(cacheTarget, ECMD_DEPTH_CHIP);
        if (ecmdIsRingCacheEnabled(cacheTarget))
        {
            rc = (ReturnCodes) ECMD_RING_CACHE_ENABLED;
            return rc;
        }
#ifdef ECMD_STATIC_FUNCTIONS
        l_ecmdRc = dllPutScom(i_target, i_address, l_ecmd_buffer); 
        if (l_ecmdRc)
        {
            rc = (ReturnCodes) l_ecmdRc;
        }
#else
        if (DllFnTable[ECMD_PUTSCOM] == NULL)
        {
            DllFnTable[ECMD_PUTSCOM] = (void*)dlsym(dlHandle, "dllPutScom");
            if (DllFnTable[ECMD_PUTSCOM] == NULL)
            {
                fprintf(stderr,"dllPutScom%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
                ecmdDisplayDllInfo();
                exit(ECMD_DLL_INVALID);
            }
        }

        uint32_t (*Function)(ecmdChipTarget &,  uint64_t,  ecmdDataBuffer &) = 
            (uint32_t(*)(ecmdChipTarget &,  uint64_t,  ecmdDataBuffer &))DllFnTable[ECMD_PUTSCOM];
        l_ecmdRc = (*Function)(i_target, i_address, l_ecmd_buffer);
        if (l_ecmdRc)
        {
            rc = (ReturnCodes) l_ecmdRc;
        }
#endif

#ifndef ECMD_STRIP_DEBUG
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &l_ecmdRc);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"putScom");
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t putScom(ecmdChipTarget & i_target, uint64_t i_address, ecmdDataBufferBase & i_data)",args);
        }
#endif

        if (l_ecmdRc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE))
        {
            std::string errorString;
            errorString = ecmdGetErrorMsg(l_ecmdRc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
            if (errorString.size()) ecmdOutput(errorString.c_str());
        }

        return rc;
    }

    ReturnCode platPutScomUnderMask(ecmdChipTarget & i_target, const uint64_t i_address, fapi2::buffer<uint64_t> & i_data, fapi2::buffer<uint64_t> & i_mask) 
    {
        ReturnCode rc;
        uint32_t l_ecmdRc;
 
        ecmdDataBuffer l_ecmd_buffer(64), l_ecmd_buffer_mask(64);
        l_ecmd_buffer.setDoubleWord(0, i_data);
        l_ecmd_buffer_mask.setDoubleWord(0, i_mask);

#ifndef ECMD_STATIC_FUNCTIONS
        if (dlHandle == NULL)
        {
            fprintf(stderr,"dllPutScomUnderMask%s",ECMD_DLL_NOT_LOADED_ERROR);
            exit(ECMD_DLL_INVALID);
        }
#endif

        if (!fapi2Initialized)
        {
            fprintf(stderr,"dllPutScomUnderMask: eCMD Extension not initialized before function called\n");
            fprintf(stderr,"dllPutScomUnderMask: OR eCMD fapi Extension not supported by plugin\n");
            exit(ECMD_DLL_INVALID);
        }

#ifndef ECMD_STRIP_DEBUG
        int myTcount;
        std::vector< void * > args;
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &i_target);
            args.push_back((void*) &i_address);
            args.push_back((void*) &l_ecmd_buffer);
            args.push_back((void*) &l_ecmd_buffer_mask);
            fppCallCount++;
            myTcount = fppCallCount;
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"ReturnCode putScomUnderMask(ecmdChipTarget & i_target, const uint64_t i_address, ecmdDataBufferBase & i_data, const ecmdDataBufferBase & i_mask)",args);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"putScomUnderMask");
        }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
        l_ecmdRc = dllPutScomUnderMask(i_target, i_address, l_ecmd_buffer, l_ecmd_buffer_mask);
        if (l_ecmdRc)
        {
            rc = (ReturnCodes) l_ecmdRc;
        }
#else
        if (DllFnTable[ECMD_PUTSCOMUNDERMASK] == NULL)
        {
            DllFnTable[ECMD_PUTSCOMUNDERMASK] = (void*)dlsym(dlHandle, "dllPutScomUnderMask");
            if (DllFnTable[ECMD_PUTSCOMUNDERMASK] == NULL)
            {
                fprintf(stderr,"dllPutScomUnderMask%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
                ecmdDisplayDllInfo();
                exit(ECMD_DLL_INVALID);
            }
        }

        uint32_t (*Function)(const ecmdChipTarget&, const uint64_t,  ecmdDataBuffer &,  const ecmdDataBuffer &) = 
            (uint32_t(*)(const ecmdChipTarget&,  const uint64_t,  ecmdDataBuffer &,  const ecmdDataBuffer &))DllFnTable[ECMD_PUTSCOMUNDERMASK];
        l_ecmdRc = (*Function)(i_target, i_address, l_ecmd_buffer, l_ecmd_buffer_mask);
        if (l_ecmdRc)
        {
            rc = (ReturnCodes) l_ecmdRc;
        }
#endif

#ifndef ECMD_STRIP_DEBUG
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &l_ecmdRc);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"putScomUnderMask");
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"ReturnCode putScomUnderMask(ecmdChipTarget & i_target, const uint64_t i_address,  const ecmdDataBufferBase & i_data, const ecmdDataBufferBase & i_mask)",args);
        }
#endif

        return rc;
    }

    ReturnCode platGetCfamRegister(ecmdChipTarget& i_target,
                                   const uint32_t i_address,
                                   buffer<uint32_t>& o_data)
    {
        ReturnCode rc(FAPI2_RC_SUCCESS);
        uint32_t l_ecmdRc;

        ecmdDataBuffer l_ecmd_buffer;

#ifndef ECMD_STATIC_FUNCTIONS
        if (dlHandle == NULL)
        {
            fprintf(stderr,"dllGetCfamRegister%s",ECMD_DLL_NOT_LOADED_ERROR);
            exit(ECMD_DLL_INVALID);
        }
#endif

#ifndef ECMD_STRIP_DEBUG
        int myTcount;
        std::vector< void * > args;
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &i_target);
            args.push_back((void*) &i_address);
            args.push_back((void*) &l_ecmd_buffer);
            fppCallCount++;
            myTcount = fppCallCount;
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t getCfamRegister(ecmdChipTarget & i_target, uint32_t i_address, ecmdDataBufferBase & o_data)",args);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"getCfamRegister");
        }
#endif

        ecmdChipTarget cacheTarget;
        cacheTarget = i_target;
        ecmdSetTargetDepth(cacheTarget, ECMD_DEPTH_CHIP);
        if (ecmdIsRingCacheEnabled(cacheTarget))
        {
            rc = (ReturnCodes) ECMD_RING_CACHE_ENABLED;
            return rc;
        }
#ifdef ECMD_STATIC_FUNCTIONS
        l_ecmdRc = dllGetCfamRegister(i_target, i_address, l_ecmd_buffer); 
        if (l_ecmdRc)
        {
            rc = (ReturnCodes) l_ecmdRc; 
        }
#else
        if (DllFnTable[ECMD_GETCFAMREGISTER] == NULL)
        {
            DllFnTable[ECMD_GETCFAMREGISTER] = (void*)dlsym(dlHandle, "dllGetCfamRegister");
            if (DllFnTable[ECMD_GETCFAMREGISTER] == NULL)
            {
                fprintf(stderr,"dllGetCfamRegister%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
                ecmdDisplayDllInfo();
                exit(ECMD_DLL_INVALID);
            }
        }

        uint32_t (*Function)(ecmdChipTarget &,  uint32_t,  ecmdDataBuffer &) = 
            (uint32_t(*)(ecmdChipTarget &,  uint32_t,  ecmdDataBuffer &))DllFnTable[ECMD_GETCFAMREGISTER];
        l_ecmdRc = (*Function)(i_target, i_address, l_ecmd_buffer);
        if (l_ecmdRc)
        {
            rc = (ReturnCodes) l_ecmdRc; 
        }
#endif

#ifndef ECMD_STRIP_DEBUG
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &l_ecmdRc);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"getCfamRegister");
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t getCfamRegister(ecmdChipTarget & i_target, uint32_t i_address, ecmdDataBufferBase & o_data)",args);
        }
#endif

        if (l_ecmdRc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE))
        {
            std::string errorString;
            errorString = ecmdGetErrorMsg(l_ecmdRc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
            if (errorString.size()) ecmdOutput(errorString.c_str());
        }

        o_data = l_ecmd_buffer.getWord(0);

        return rc;
    }

    ReturnCode platPutCfamRegister(ecmdChipTarget& i_target,
                                  const uint32_t i_address,
                                  buffer<uint32_t>& i_data)
    {
        ReturnCode rc(FAPI2_RC_SUCCESS);
        uint32_t l_ecmdRc;

        ecmdDataBuffer l_ecmd_buffer(32);
        l_ecmd_buffer.setWord(0, i_data);

#ifndef ECMD_STATIC_FUNCTIONS
        if (dlHandle == NULL)
        {
            fprintf(stderr,"dllPutCfamRegister%s",ECMD_DLL_NOT_LOADED_ERROR);
            exit(ECMD_DLL_INVALID);
        }
#endif

#ifndef ECMD_STRIP_DEBUG
        int myTcount;
        std::vector< void * > args;
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &i_target);
            args.push_back((void*) &i_address);
            args.push_back((void*) &l_ecmd_buffer);
            fppCallCount++;
            myTcount = fppCallCount;
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t putCfamRegister(ecmdChipTarget & i_target, uint32_t i_address, ecmdDataBuffer & i_data)",args);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"putCfamRegister");
        }
#endif

        ecmdChipTarget cacheTarget;
        cacheTarget = i_target;
        ecmdSetTargetDepth(cacheTarget, ECMD_DEPTH_CHIP);
        if (ecmdIsRingCacheEnabled(cacheTarget))
        {
            rc = (ReturnCodes) ECMD_RING_CACHE_ENABLED;
            return rc;
        }
#ifdef ECMD_STATIC_FUNCTIONS
        l_ecmdRc = dllPutCfamRegister(i_target, i_address, l_ecmd_buffer); 
        if (l_ecmdRc)
        {
            rc = (ReturnCodes) l_ecmdRc; 
        }
#else
        if (DllFnTable[ECMD_PUTCFAMREGISTER] == NULL)
        {
            DllFnTable[ECMD_PUTCFAMREGISTER] = (void*)dlsym(dlHandle, "dllPutCfamRegister");
            if (DllFnTable[ECMD_PUTCFAMREGISTER] == NULL)
            {
                fprintf(stderr,"dllPutCfamRegister%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
                ecmdDisplayDllInfo();
                exit(ECMD_DLL_INVALID);
            }
        }

        uint32_t (*Function)(ecmdChipTarget &,  uint32_t,  ecmdDataBuffer &) = 
            (uint32_t(*)(ecmdChipTarget &,  uint32_t,  ecmdDataBuffer &))DllFnTable[ECMD_PUTCFAMREGISTER];
        l_ecmdRc = (*Function)(i_target, i_address, l_ecmd_buffer);
        if (l_ecmdRc)
        {
            rc = (ReturnCodes) l_ecmdRc; 
        }
#endif

#ifndef ECMD_STRIP_DEBUG
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &l_ecmdRc);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"putCfamRegister");
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t putCfamRegister(ecmdChipTarget & i_target, uint32_t i_address, ecmdDataBuffer & i_data)",args);
        }
#endif

        if (l_ecmdRc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE))
        {
            std::string errorString;
            errorString = ecmdGetErrorMsg(l_ecmdRc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
            if (errorString.size()) ecmdOutput(errorString.c_str());
        }

        return rc;
    }

    ReturnCode platGetRing(ecmdChipTarget & i_target, const scanRingId_t i_address, fapi2::variable_buffer & o_data, const uint32_t i_ringMode)
    {
        ReturnCode rc(FAPI2_RC_SUCCESS);
        uint32_t l_ecmdRc;
        ecmdDataBufferBase l_ecmd_buffer;

#ifndef ECMD_STATIC_FUNCTIONS
        if (dlHandle == NULL)
        {
            fprintf(stderr,"dllFapi2GetRing%s",ECMD_DLL_NOT_LOADED_ERROR);
            exit(ECMD_DLL_INVALID);
        }
#endif

        if (!fapi2Initialized)
        {
            fprintf(stderr,"dllFapi2GetRing: eCMD Extension not initialized before function called\n");
            fprintf(stderr,"dllFapi2GetRing: OR eCMD fapi Extension not supported by plugin\n");
            exit(ECMD_DLL_INVALID);
        }

#ifndef ECMD_STRIP_DEBUG
        int myTcount;
        std::vector< void * > args;
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &i_target);
            args.push_back((void*) &i_address);
            args.push_back((void*) &l_ecmd_buffer);
            args.push_back((void*) &i_ringMode);
            fppCallCount++;
            myTcount = fppCallCount;
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t getRing(ecmdChipTarget & i_target, const scanRingId_t i_address, ecmdDataBufferBase & o_data, const uint32_t i_ringMode)",args);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"getRing");
        }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
        l_ecmdRc = dllFapi2GetRing(i_target, i_address, l_ecmd_buffer, i_ringMode);
        if (l_ecmdRc)
        {
            rc = (ReturnCodes) l_ecmdRc; 
        }
#else
        if (fapi2DllFnTable[ECMD_FAPI2GETRING] == NULL)
        {
            fapi2DllFnTable[ECMD_FAPI2GETRING] = (void*)dlsym(dlHandle, "dllFapi2GetRing");
            if (fapi2DllFnTable[ECMD_FAPI2GETRING] == NULL)
            {
                fprintf(stderr,"dllFapi2GetRing%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
                ecmdDisplayDllInfo();
                exit(ECMD_DLL_INVALID);
            }
        }

        uint32_t (*Function)(ecmdChipTarget&,  const scanRingId_t,  ecmdDataBufferBase &, const uint32_t ) = 
            (uint32_t(*)(ecmdChipTarget&,  const scanRingId_t,  ecmdDataBufferBase &, const uint32_t))fapi2DllFnTable[ECMD_FAPI2GETRING];
        l_ecmdRc = (*Function)(i_target, i_address, l_ecmd_buffer, i_ringMode);
        if (l_ecmdRc)
        {
            rc = (ReturnCodes) l_ecmdRc; 
        }
#endif

#ifndef ECMD_STRIP_DEBUG
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &l_ecmdRc);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"getRing");
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t getRing(ecmdChipTarget & i_target, const scanRingId_t i_address, ecmdDataBufferBase & o_data)",args);
        }
#endif

#if 0
        uint32_t* l_data = ecmdDataBufferBaseImplementationHelper::getDataPtr(&l_ecmd_buffer);
        if (l_data != NULL)
        {
            o_data.insert(l_data, 0, l_ecmd_buffer.getBitLength(), 0);
        }
        else
        {
            // ERROR
        }
#endif

        return rc;
    }

    ReturnCode platPutRing(ecmdChipTarget & i_target, const scanRingId_t i_address, fapi2::variable_buffer & i_data, const uint32_t i_ringMode)
    {
        ReturnCode rc(FAPI2_RC_SUCCESS);
        uint32_t l_ecmdRc;
        ecmdDataBufferBase l_ecmd_buffer;

        // l_ecmd_buffer = i_data; // FIXME

#ifndef ECMD_STATIC_FUNCTIONS
        if (dlHandle == NULL)
        {
            fprintf(stderr,"dllFapi2PutRing%s",ECMD_DLL_NOT_LOADED_ERROR);
            exit(ECMD_DLL_INVALID);
        }
#endif

        if (!fapi2Initialized)
        {
            fprintf(stderr,"dllFapi2PutRing: eCMD Extension not initialized before function called\n");
            fprintf(stderr,"dllFapi2PutRing: OR eCMD fapi Extension not supported by plugin\n");
            exit(ECMD_DLL_INVALID);
        }

#ifndef ECMD_STRIP_DEBUG
        int myTcount;
        std::vector< void * > args;
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &i_target);
            args.push_back((void*) &i_address);
            args.push_back((void*) &l_ecmd_buffer);
            args.push_back((void*) &i_ringMode);
            fppCallCount++;
            myTcount = fppCallCount;
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t putRing(ecmdChipTarget & i_target, const scanRingId_t i_address, ecmdDataBufferBase & i_data, const uint32_t i_ringMode)",args);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"putRing");
        }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
        l_ecmdRc = dllFapi2PutRing(i_target, i_address, l_ecmd_buffer, i_ringMode);
        if (l_ecmdRc)
        {
            rc = (ReturnCodes) l_ecmdRc; 
        }
#else
        if (fapi2DllFnTable[ECMD_FAPI2PUTRING] == NULL)
        {
            fapi2DllFnTable[ECMD_FAPI2PUTRING] = (void*)dlsym(dlHandle, "dllFapi2PutRing");
            if (fapi2DllFnTable[ECMD_FAPI2PUTRING] == NULL)
            {
                fprintf(stderr,"dllFapi2PutRing%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
                ecmdDisplayDllInfo();
                exit(ECMD_DLL_INVALID);
            }
        }

        uint32_t (*Function)(ecmdChipTarget&,  const scanRingId_t, ecmdDataBufferBase &, const uint32_t i_ringMode) = 
            (uint32_t(*)(ecmdChipTarget&,  const scanRingId_t, ecmdDataBufferBase &, const uint32_t))fapi2DllFnTable[ECMD_FAPI2PUTRING];
        l_ecmdRc = (*Function)(i_target, i_address, l_ecmd_buffer, i_ringMode);
        if (l_ecmdRc)
        {
            rc = (ReturnCodes) l_ecmdRc; 
        }
#endif

#ifndef ECMD_STRIP_DEBUG
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &l_ecmdRc);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"putRing");
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t putRing(ecmdChipTarget & i_target, const scanRingId_t i_address, ecmdDataBufferBase & i_data, const uint32_t i_ringMode)",args);
        }
#endif

        return rc;
    }

#define PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE) \
    template<> \
    ReturnCode platGetScom(const Target<TARGET_TYPE>& i_target, \
                           const uint64_t i_address, \
                           buffer<uint64_t>& o_data) \
    { \
        ecmdChipTarget ecmdTarget; \
        fapiTargetToEcmdTarget(i_target, ecmdTarget);  \
        return platGetScom(ecmdTarget, i_address, o_data); \
    }

    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_PROC_CHIP)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MEMBUF_CHIP)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_EX)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MBA)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MCS)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_XBUS)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_ABUS)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_L4)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_CORE)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_EQ)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MCA)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MCBIST)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MIA)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MIS)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_DMI)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_OBUS)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_NV)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_SBE)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_PPE)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_PERV)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_PEC)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_PHB)
#undef PLAT_GET_SCOM_TEMPLATE_MACRO

#define PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE) \
    template<> \
    ReturnCode platPutScom(const Target<TARGET_TYPE> & i_target, \
                           const uint64_t i_address, \
                           buffer<uint64_t>& i_data) \
    { \
        ecmdChipTarget ecmdTarget; \
        fapiTargetToEcmdTarget(i_target, ecmdTarget);  \
        return platPutScom(ecmdTarget, i_address, i_data); \
    }

    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_PROC_CHIP)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MEMBUF_CHIP)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_EX)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MBA)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MCS)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_XBUS)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_ABUS)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_L4)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_CORE)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_EQ)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MCA)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MCBIST)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MIA)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MIS)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_DMI)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_OBUS)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_NV)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_SBE)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_PPE)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_PERV)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_PEC)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_PHB)
#undef PLAT_PUT_SCOM_TEMPLATE_MACRO

#define PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE) \
    template<> \
    ReturnCode platPutScomUnderMask(const Target<TARGET_TYPE> & i_target, \
                                    const uint64_t i_address, \
                                    buffer<uint64_t>& i_data, \
                                    buffer<uint64_t>& i_mask) \
    { \
        ecmdChipTarget ecmdTarget; \
        fapiTargetToEcmdTarget(i_target, ecmdTarget);  \
        return platPutScomUnderMask(ecmdTarget, i_address, i_data, i_mask); \
    }

    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_PROC_CHIP)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_MEMBUF_CHIP)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_EX)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_MBA)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_MCS)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_XBUS)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_ABUS)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_L4)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_CORE)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_EQ)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_MCA)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_MCBIST)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_MIA)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_MIS)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_DMI)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_OBUS)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_NV)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_SBE)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_PPE)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_PERV)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_PEC)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_PHB)
#undef PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO

    template<>
    ReturnCode platGetCfamRegister(const Target<TARGET_TYPE_PROC_CHIP>& i_target,
                                   const uint32_t i_address,
                                   buffer<uint32_t>& o_data)
    {
        ecmdChipTarget ecmdTarget;
        fapiTargetToEcmdTarget(i_target, ecmdTarget); 
        return platGetCfamRegister(ecmdTarget, i_address, o_data);
    }

    template<>
    ReturnCode platGetCfamRegister(const Target<TARGET_TYPE_MEMBUF_CHIP>& i_target,
                                   const uint32_t i_address,
                                   buffer<uint32_t>& o_data)
    {
        ecmdChipTarget ecmdTarget;
        fapiTargetToEcmdTarget(i_target, ecmdTarget); 
        return platGetCfamRegister(ecmdTarget, i_address, o_data);
    }


    template<>
    ReturnCode platPutCfamRegister(const Target<TARGET_TYPE_PROC_CHIP>& i_target,
                                  const uint32_t i_address,
                                  buffer<uint32_t>& i_data)
    {
        ecmdChipTarget ecmdTarget;
        fapiTargetToEcmdTarget(i_target, ecmdTarget); 
        return platPutCfamRegister(ecmdTarget, i_address, i_data);
    }

    template<>
    ReturnCode platPutCfamRegister(const Target<TARGET_TYPE_MEMBUF_CHIP>& i_target,
                                  const uint32_t i_address,
                                  buffer<uint32_t>& i_data)
    {
        ecmdChipTarget ecmdTarget;
        fapiTargetToEcmdTarget(i_target, ecmdTarget); 
        return platPutCfamRegister(ecmdTarget, i_address, i_data);
    }

#define PLAT_GET_RING_TEMPLATE_MACRO(TARGET_TYPE) \
    template<> \
    ReturnCode platGetRing(const Target<TARGET_TYPE>& i_target, \
                           const scanRingId_t i_address, \
                           variable_buffer& o_data, \
                           const RingMode i_ringMode) \
    { \
        ecmdChipTarget ecmdTarget; \
        fapiTargetToEcmdTarget(i_target, ecmdTarget);  \
        return platGetRing(ecmdTarget, i_address, o_data, i_ringMode); \
    }

    PLAT_GET_RING_TEMPLATE_MACRO(TARGET_TYPE_PROC_CHIP)
    PLAT_GET_RING_TEMPLATE_MACRO(TARGET_TYPE_MEMBUF_CHIP)
    PLAT_GET_RING_TEMPLATE_MACRO(TARGET_TYPE_CORE)
    PLAT_GET_RING_TEMPLATE_MACRO(TARGET_TYPE_EQ)
#undef PLAT_GET_RING_TEMPLATE_MACRO

#define PLAT_PUT_RING_TEMPLATE_MACRO(TARGET_TYPE) \
    template<> \
    ReturnCode platPutRing(const Target<TARGET_TYPE>& i_target, \
                           const scanRingId_t i_address, \
                           variable_buffer& i_data, \
                           const RingMode i_ringMode) \
    { \
        ecmdChipTarget ecmdTarget; \
        fapiTargetToEcmdTarget(i_target, ecmdTarget);  \
        return platPutRing(ecmdTarget, i_address, i_data, i_ringMode); \
    }

    PLAT_PUT_RING_TEMPLATE_MACRO(TARGET_TYPE_PROC_CHIP)
    PLAT_PUT_RING_TEMPLATE_MACRO(TARGET_TYPE_MEMBUF_CHIP)
    PLAT_PUT_RING_TEMPLATE_MACRO(TARGET_TYPE_CORE)
    PLAT_PUT_RING_TEMPLATE_MACRO(TARGET_TYPE_EQ)
#undef PLAT_PUT_RING_TEMPLATE_MACRO


} // namespace fapi2
