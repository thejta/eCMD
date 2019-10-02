//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2018 IBM International Business Machines Corp.
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


//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <plat_i2c_access.H> 

#include <stdio.h>

static const char ECMD_DLL_NOT_LOADED_ERROR[] = ": eCMD Function called before DLL has been loaded\n";
static const char ECMD_UNABLE_TO_FIND_FUNCTION_ERROR[] = ": Unable to find function, must be an invalid DLL - program aborting\n";

#ifndef ECMD_STATIC_FUNCTIONS
#include <fapi2ClientEnums.H>
#include <dlfcn.h>
extern void * dlHandle;
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

namespace fapi2plat
{

fapi2::ReturnCode getI2c(const ecmdChipTarget& i_target,
                         const size_t i_get_size,
                         const std::vector<uint8_t>& i_data,
                         std::vector<uint8_t>& o_data)
{
    const char * dllFunctionName = "dllFapi2GetI2c";
    fapi2::ReturnCode rc(fapi2::FAPI2_RC_SUCCESS);
    uint32_t l_ecmdRc;

#ifndef ECMD_STATIC_FUNCTIONS
    if (dlHandle == NULL)
    {
        fprintf(stderr,"%s%s",dllFunctionName,ECMD_DLL_NOT_LOADED_ERROR);
        exit(ECMD_DLL_INVALID);
    }
#endif

    if (!fapi2Initialized)
    {
        fprintf(stderr,"%s: eCMD Extension not initialized before function called\n", dllFunctionName);
        fprintf(stderr,"%s: OR eCMD fapi Extension not supported by plugin\n", dllFunctionName);
        exit(ECMD_DLL_INVALID);
    }

#ifndef ECMD_STRIP_DEBUG
    int myTcount;
    std::vector< void * > args;
    if (ecmdClientDebug != 0)
    {
        args.push_back((void*) &i_target);
        args.push_back((void*) &i_get_size);
        args.push_back((void*) &i_data);
        args.push_back((void*) &o_data);
        fppCallCount++;
        myTcount = fppCallCount;
        ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t getI2c(const ecmdChipTarget & i_target, const size_t i_get_size, const std::vector<uint8_t>& i_data, std::vector<uint8_t>& o_data)",args);
        ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"getI2c");
    }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
    l_ecmdRc = dllFapi2GetI2c(i_target, i_get_size, i_data, o_data);
#else
    if (fapi2DllFnTable[ECMD_FAPI2GETI2C] == NULL)
    {
        fapi2DllFnTable[ECMD_FAPI2GETI2C] = (void*)dlsym(dlHandle, dllFunctionName);
        if (fapi2DllFnTable[ECMD_FAPI2GETI2C] == NULL)
        {
            fprintf(stderr,"%s%s",dllFunctionName,ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
            ecmdDisplayDllInfo();
            exit(ECMD_DLL_INVALID);
        }
    }

    uint32_t (*Function)(const ecmdChipTarget&, const size_t, const std::vector<uint8_t>&, std::vector<uint8_t>&) = 
        (uint32_t(*)(const ecmdChipTarget&, const size_t, const std::vector<uint8_t>&, std::vector<uint8_t>&))fapi2DllFnTable[ECMD_FAPI2GETI2C];
    l_ecmdRc = (*Function)(i_target, i_get_size, i_data, o_data);
#endif
    if (l_ecmdRc)
    {
        rc = (fapi2::ReturnCodes) l_ecmdRc; 
    }

#ifndef ECMD_STRIP_DEBUG
    if (ecmdClientDebug != 0)
    {
        args.push_back((void*) &l_ecmdRc);
        ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"getI2c");
        ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t getI2c(const ecmdChipTarget & i_target, const size_t i_get_size, const std::vector<uint8_t>& i_data, std::vector<uint8_t>& o_data)",args);
    }
#endif

    return rc;
}

fapi2::ReturnCode putI2c(const ecmdChipTarget& i_target,
                         const std::vector<uint8_t>& i_data)
{
    const char * dllFunctionName = "dllFapi2PutI2c";
    fapi2::ReturnCode rc(fapi2::FAPI2_RC_SUCCESS);
    uint32_t l_ecmdRc;

#ifndef ECMD_STATIC_FUNCTIONS
    if (dlHandle == NULL)
    {
        fprintf(stderr,"%s%s",dllFunctionName,ECMD_DLL_NOT_LOADED_ERROR);
        exit(ECMD_DLL_INVALID);
    }
#endif

    if (!fapi2Initialized)
    {
        fprintf(stderr,"%s: eCMD Extension not initialized before function called\n", dllFunctionName);
        fprintf(stderr,"%s: OR eCMD fapi Extension not supported by plugin\n", dllFunctionName);
        exit(ECMD_DLL_INVALID);
    }

#ifndef ECMD_STRIP_DEBUG
    int myTcount;
    std::vector< void * > args;
    if (ecmdClientDebug != 0)
    {
        args.push_back((void*) &i_target);
        args.push_back((void*) &i_data);
        fppCallCount++;
        myTcount = fppCallCount;
        ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t putI2c(const ecmdChipTarget & i_target, const std::vector<uint8_t>& i_data)",args);
        ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"putI2c");
    }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
    l_ecmdRc = dllFapi2PutI2c(i_target, i_data);
#else
    if (fapi2DllFnTable[ECMD_FAPI2PUTI2C] == NULL)
    {
        fapi2DllFnTable[ECMD_FAPI2PUTI2C] = (void*)dlsym(dlHandle, dllFunctionName);
        if (fapi2DllFnTable[ECMD_FAPI2PUTI2C] == NULL)
        {
            fprintf(stderr,"%s%s",dllFunctionName,ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
            ecmdDisplayDllInfo();
            exit(ECMD_DLL_INVALID);
        }
    }

    uint32_t (*Function)(const ecmdChipTarget&, const std::vector<uint8_t>&) = 
        (uint32_t(*)(const ecmdChipTarget&, const std::vector<uint8_t>&))fapi2DllFnTable[ECMD_FAPI2PUTI2C];
    l_ecmdRc = (*Function)(i_target, i_data);
#endif
    if (l_ecmdRc)
    {
        rc = (fapi2::ReturnCodes) l_ecmdRc; 
    }

#ifndef ECMD_STRIP_DEBUG
    if (ecmdClientDebug != 0)
    {
        args.push_back((void*) &l_ecmdRc);
        ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"putI2c");
        ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t putI2c(const ecmdChipTarget & i_target, std::vector<uint8_t>& i_data)",args);
    }
#endif

    return rc;

}

};

