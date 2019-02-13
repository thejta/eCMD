//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2019 IBM International Business Machines Corp.
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
#include <plat_hw_access.H> 

#include <stdio.h>

#include <ecmdClientCapi.H>
#include <ecmdSharedUtils.H>
#include <ecmdClientEnums.H>
#include <fapi2ClientCapi.H>

#include <mvpd_access.H> 
#include <fapi2SharedUtils.H> 
#include <variable_buffer_utils.H>

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

namespace fapi2plat
{
    fapi2::ReturnCode setMulticastGroupMap(const ecmdChipTarget & i_target,
                                           const std::vector< fapi2::MulticastGroupMapping > & i_mappings)
    {
        fapi2::ReturnCode rc(fapi2::FAPI2_RC_SUCCESS);
        uint32_t l_ecmdRc;

#ifndef ECMD_STATIC_FUNCTIONS
        if (dlHandle == NULL)
        {
            fprintf(stderr,"dllFapi2SetMcastGroupMap%s",ECMD_DLL_NOT_LOADED_ERROR);
            exit(ECMD_DLL_INVALID);
        }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
        l_ecmdRc = dllFapi2SetMcastGroupMap(i_target, i_mappings);
#else
        if (fapi2DllFnTable[ECMD_FAPI2SETMCASTGROUPMAP] == NULL)
        {
            fapi2DllFnTable[ECMD_FAPI2SETMCASTGROUPMAP] = (void*)dlsym(dlHandle, "dllFapi2SetMcastGroupMap");
            if (fapi2DllFnTable[ECMD_FAPI2SETMCASTGROUPMAP] == NULL)
            {
                fprintf(stderr,"dllFapi2SetMcastGroupMap%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
                ecmdDisplayDllInfo();
                exit(ECMD_DLL_INVALID);
            }
        }

        uint32_t (*Function)(const ecmdChipTarget &, const std::vector< fapi2::MulticastGroupMapping > &) =
            (uint32_t(*)(const ecmdChipTarget &, const std::vector< fapi2::MulticastGroupMapping > &))fapi2DllFnTable[ECMD_FAPI2SETMCASTGROUPMAP];
        l_ecmdRc = (*Function)(i_target, i_mappings);
#endif
        if (l_ecmdRc)
        {
            rc = (fapi2::ReturnCodes) l_ecmdRc; 
        }

        if (l_ecmdRc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE))
        {
            std::string errorString;
            errorString = ecmdGetErrorMsg(l_ecmdRc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
            if (errorString.size())
                ecmdOutput(errorString.c_str());
        }

        return rc;
    }

    fapi2::ReturnCode getScomMulticast(const ecmdChipTarget& i_target,
                                       const uint32_t i_type,
                                       const uint32_t i_group,
                                       const uint64_t i_address,
                                       fapi2::buffer<uint64_t>& o_data)
    {
        fapi2::ReturnCode rc(fapi2::FAPI2_RC_SUCCESS);
        uint32_t l_ecmdRc;

        ecmdDataBuffer l_ecmd_buffer;

#ifndef ECMD_STATIC_FUNCTIONS
        if (dlHandle == NULL)
        {
            fprintf(stderr,"dllFapi2GetScomMulticast%s",ECMD_DLL_NOT_LOADED_ERROR);
            exit(ECMD_DLL_INVALID);
        }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
        l_ecmdRc = dllFapi2GetScomMulticast(i_target, i_type, i_group, i_address, l_ecmd_buffer); 
#else
        if (fapi2DllFnTable[ECMD_FAPI2GETSCOMMULTICAST] == NULL)
        {
            fapi2DllFnTable[ECMD_FAPI2GETSCOMMULTICAST] = (void*)dlsym(dlHandle, "dllFapi2GetScomMulticast");
            if (fapi2DllFnTable[ECMD_FAPI2GETSCOMMULTICAST] == NULL)
            {
                fprintf(stderr,"dllFapi2GetScomMulticast%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
                ecmdDisplayDllInfo();
                exit(ECMD_DLL_INVALID);
            }
        }

        uint32_t (*Function)(const ecmdChipTarget &, const uint32_t, const uint32_t, const uint64_t, ecmdDataBuffer &) = 
            (uint32_t(*)(const ecmdChipTarget &, const uint32_t, const uint32_t, const uint64_t, ecmdDataBuffer &))fapi2DllFnTable[ECMD_FAPI2GETSCOMMULTICAST];
        l_ecmdRc = (*Function)(i_target, i_type, i_group, i_address, l_ecmd_buffer);
#endif
        if (l_ecmdRc)
        {
            rc = (fapi2::ReturnCodes) l_ecmdRc; 
        }

        if (l_ecmdRc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE))
        {
            std::string errorString;
            errorString = ecmdGetErrorMsg(l_ecmdRc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
            if (errorString.size())
                ecmdOutput(errorString.c_str());
        }

        o_data = l_ecmd_buffer.getDoubleWord(0);

        return rc;
    }

    fapi2::ReturnCode putScomMulticast(const ecmdChipTarget & i_target,
                                       const uint32_t i_type,
                                       const uint32_t i_group,
                                       const uint64_t i_address,
                                       const fapi2::buffer<uint64_t> i_data)
    {
        fapi2::ReturnCode rc;
        uint32_t l_ecmdRc;

        ecmdDataBuffer l_ecmd_buffer(64);
        l_ecmd_buffer.setDoubleWord(0, i_data);

#ifndef ECMD_STATIC_FUNCTIONS
        if (dlHandle == NULL)
        {
            fprintf(stderr,"dllFapi2PutScomMulticast%s",ECMD_DLL_NOT_LOADED_ERROR);
            exit(ECMD_DLL_INVALID);
        }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
        l_ecmdRc = dllFapi2PutScomMulticast(i_target, i_type, i_group, i_address, l_ecmd_buffer); 
#else
        if (fapi2DllFnTable[ECMD_FAPI2PUTSCOMMULTICAST] == NULL)
        {
            fapi2DllFnTable[ECMD_FAPI2PUTSCOMMULTICAST] = (void*)dlsym(dlHandle, "dllFapi2PutScomMulticast");
            if (fapi2DllFnTable[ECMD_FAPI2PUTSCOMMULTICAST] == NULL)
            {
                fprintf(stderr,"dllFapi2PutScomMulticast%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
                ecmdDisplayDllInfo();
                exit(ECMD_DLL_INVALID);
            }
        }

        uint32_t (*Function)(const ecmdChipTarget &, const uint32_t, const uint32_t, const uint64_t, const ecmdDataBuffer &) = 
            (uint32_t(*)(const ecmdChipTarget &, const uint32_t, const uint32_t, const uint64_t, const ecmdDataBuffer &))fapi2DllFnTable[ECMD_FAPI2PUTSCOMMULTICAST];
        l_ecmdRc = (*Function)(i_target, i_type, i_group, i_address, l_ecmd_buffer);
#endif
        if (l_ecmdRc)
        {
            rc = (fapi2::ReturnCodes) l_ecmdRc;
        }

        if (l_ecmdRc && !ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETERRORMODE))
        {
            std::string errorString;
            errorString = ecmdGetErrorMsg(l_ecmdRc, false, ecmdGetGlobalVar(ECMD_GLOBALVAR_CMDLINEMODE), false);
            if (errorString.size())
                ecmdOutput(errorString.c_str());
        }

        return rc;
    }

} // namespace fapi2plat
