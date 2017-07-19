//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2017 IBM International Business Machines Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
//IBM_PROLOG_END_TAG

#include <plat_mbvpd_access.H>

#include <dlfcn.h>

#include <fapi2DllCapi.H>
#include <fapi2ClientEnums.H>

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

namespace fapi2plat
{
    fapi2::ReturnCode getMBvpdField(const fapi2::MBvpdRecord i_record,
                                    const fapi2::MBvpdKeyword i_keyword,
                                    const ecmdChipTarget& i_target,
                                    uint8_t* const i_pBuffer,
                                    size_t& io_fieldSize)
    {
        fapi2::ReturnCode rc;
        uint32_t l_ecmdRc;

#ifndef ECMD_STATIC_FUNCTIONS
        if (dlHandle == NULL)
        {
            fprintf(stderr,"dllFapi2GetMBvpdField%s",ECMD_DLL_NOT_LOADED_ERROR);
            exit(ECMD_DLL_INVALID);
        }
#endif
        if (!fapi2Initialized)
        {
            fprintf(stderr,"dllFapi2GetMBvpdField: eCMD Extension not initialized before function called\n");
            fprintf(stderr,"dllFapi2GetMBvpdField: OR eCMD fapi Extension not supported by plugin\n");
            exit(ECMD_DLL_INVALID);
        }

#ifndef ECMD_STRIP_DEBUG
        int myTcount;
        std::vector< void * > args;
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &i_record);
            args.push_back((void*) &i_keyword);
            args.push_back((void*) &i_target);
            args.push_back((void*) &i_pBuffer);
            args.push_back((void*) &io_fieldSize);
            fppCallCount++;
            myTcount = fppCallCount;
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t dllFapi2GetMBvpdField(const fapi2::MBvpdRecord i_record, const fapi2::MBvpdKeyword i_keyword, const ecmdChipTarget& i_target, uint8_t* const i_pBuffer, size_t& io_fieldSize) )",args);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"dllFapi2GetMBvpdField");
        }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
        l_ecmdRc = dllFapi2GetMBvpdField(i_record, i_keyword, i_target, i_pBuffer, io_fieldSize);
#else
        if (fapi2DllFnTable[ECMD_FAPI2GETMBVPDFIELD] == NULL)
        {
            fapi2DllFnTable[ECMD_FAPI2GETMBVPDFIELD] = (void*)dlsym(dlHandle, "dllFapi2GetMBvpdField");
            if (fapi2DllFnTable[ECMD_FAPI2GETMBVPDFIELD] == NULL)
            {
                fprintf(stderr,"dllFapi2GetMBvpdField%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
                ecmdDisplayDllInfo();
                exit(ECMD_DLL_INVALID);
            }
        }

        uint32_t (*Function)(const fapi2::MBvpdRecord, const fapi2::MBvpdKeyword, const ecmdChipTarget &, uint8_t* const, size_t&) = 
            (uint32_t(*)(const fapi2::MBvpdRecord, const fapi2::MBvpdKeyword, const ecmdChipTarget &, uint8_t* const, size_t&))fapi2DllFnTable[ECMD_FAPI2GETMBVPDFIELD];
        l_ecmdRc = (*Function)(i_record, i_keyword, i_target, i_pBuffer, io_fieldSize);
#endif
        if (l_ecmdRc)
        {
            rc = (fapi2::ReturnCodes) l_ecmdRc; 
        }

#ifndef ECMD_STRIP_DEBUG
        if (ecmdClientDebug != 0) {
            args.push_back((void*) &l_ecmdRc);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"dllFapi2GetMBvpdField");
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t dllFapi2GetMBvpdField(const fapi2::MBvpdRecord i_record, const fapi2::MBvpdKeyword i_keyword, const ecmdChipTarget& i_target, uint8_t* const i_pBuffer, size_t& io_fieldSize) )",args);
        }
#endif

        return rc;
    }

    fapi2::ReturnCode setMBvpdField(const fapi2::MBvpdRecord i_record,
                                    const fapi2::MBvpdKeyword i_keyword,
                                    const ecmdChipTarget& i_target,
                                    const uint8_t* const i_pBuffer,
                                    const size_t i_fieldSize)
    {
        fapi2::ReturnCode rc;
        uint32_t l_ecmdRc;

#ifndef ECMD_STATIC_FUNCTIONS
        if (dlHandle == NULL)
        {
            fprintf(stderr,"dllFapi2SetMBvpdField%s",ECMD_DLL_NOT_LOADED_ERROR);
            exit(ECMD_DLL_INVALID);
        }
#endif
        if (!fapi2Initialized)
        {
            fprintf(stderr,"dllFapi2SetMBvpdField: eCMD Extension not initialized before function called\n");
            fprintf(stderr,"dllFapi2SetMBvpdField: OR eCMD fapi Extension not supported by plugin\n");
            exit(ECMD_DLL_INVALID);
        }

#ifndef ECMD_STRIP_DEBUG
        int myTcount;
        std::vector< void * > args;
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &i_record);
            args.push_back((void*) &i_keyword);
            args.push_back((void*) &i_target);
            args.push_back((void*) &i_pBuffer);
            args.push_back((void*) &i_fieldSize);
            fppCallCount++;
            myTcount = fppCallCount;
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t dllFapi2SetMBvpdField(const fapi2::MBvpdRecord i_record, const fapi2::MBvpdKeyword i_keyword, const ecmdChipTarget& i_target, const uint8_t* const i_pBuffer, const size_t i_fieldSize) )",args);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"dllFapi2SetMBvpdField");
        }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
        l_ecmdRc = dllFapi2SetMBvpdField(i_record, i_keyword, i_target, i_pBuffer, i_fieldSize);
#else
        if (fapi2DllFnTable[ECMD_FAPI2SETMBVPDFIELD] == NULL)
        {
            fapi2DllFnTable[ECMD_FAPI2SETMBVPDFIELD] = (void*)dlsym(dlHandle, "dllFapi2SetMBvpdField");
            if (fapi2DllFnTable[ECMD_FAPI2SETMBVPDFIELD] == NULL)
            {
                fprintf(stderr,"dllFapi2SetMBvpdField%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
                ecmdDisplayDllInfo();
                exit(ECMD_DLL_INVALID);
            }
        }

        uint32_t (*Function)(const fapi2::MBvpdRecord, const fapi2::MBvpdKeyword, const ecmdChipTarget &, const uint8_t* const, const size_t) = 
            (uint32_t(*)(const fapi2::MBvpdRecord, const fapi2::MBvpdKeyword, const ecmdChipTarget &, const uint8_t* const, const size_t))fapi2DllFnTable[ECMD_FAPI2SETMBVPDFIELD];
        l_ecmdRc = (*Function)(i_record, i_keyword, i_target, i_pBuffer, i_fieldSize);
#endif
        if (l_ecmdRc)
        {
            rc = (fapi2::ReturnCodes) l_ecmdRc; 
        }

#ifndef ECMD_STRIP_DEBUG
        if (ecmdClientDebug != 0) {
            args.push_back((void*) &l_ecmdRc);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"dllFapi2SetMBvpdField");
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t dllFapi2SetMBvpdField(const fapi2::MBvpdRecord i_record, const fapi2::MBvpdKeyword i_keyword, const ecmdChipTarget& i_target, const uint8_t* const i_pBuffer, const size_t i_fieldSize) )",args);
        }
#endif

        return rc;
    }

    fapi2::ReturnCode getMBvpdField(const fapi2::MBvpdRecord i_record,
                                    const fapi2::MBvpdKeyword i_keyword,
                                    const fapi2::Target<fapi2::TARGET_TYPE_MEMBUF_CHIP>& i_target,
                                    uint8_t* const i_pBuffer,
                                    size_t& io_fieldSize)
    {
        ecmdChipTarget ecmdTarget;
        fapiTargetToEcmdTarget(i_target, ecmdTarget);
        return fapi2plat::getMBvpdField(i_record, i_keyword, ecmdTarget, i_pBuffer, io_fieldSize);
    }

    fapi2::ReturnCode setMBvpdField(const fapi2::MBvpdRecord i_record,
                                    const fapi2::MBvpdKeyword i_keyword,
                                    const fapi2::Target<fapi2::TARGET_TYPE_MEMBUF_CHIP>& i_target,
                                    const uint8_t* const i_pBuffer,
                                    const size_t i_fieldSize)
    {
        ecmdChipTarget ecmdTarget;
        fapiTargetToEcmdTarget(i_target, ecmdTarget);
        return fapi2plat::setMBvpdField(i_record, i_keyword, ecmdTarget, i_pBuffer, i_fieldSize);
    }
}; // namespace fapi2plat
