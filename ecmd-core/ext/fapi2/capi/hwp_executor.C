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


//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <dlfcn.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <fapi2Structs.H>
#include <return_code.H>
#include <fapi2DllCapi.H> 
#include <fapi2ClientCapi.H>
#include <ecmdReturnCodes.H>

// these should be in the fapi namespace, right? -farrugia JFDEBUG
   
#undef FAPI_ERR
#define FAPI_ERR(_fmt_, _args_...) printf("FAPI ERR>: " _fmt_ "\n", ##_args_)   //JFDEBUG local defined due to dll load errors

namespace fapi2plat
{
    // dlopens a shared library and returns the handle
    int openSharedLib(const std::string & i_libName, void * & o_pLibHandle, int flag)
    {
        uint32_t rc = fapi2::FAPI2_RC_SUCCESS;
        std::string sharedLibPath;

#ifdef __linux__
#ifdef _LP64
#ifdef __powerpc__
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        std::string tmp = (i_libName + "_ppc64le.so");
#else
        std::string tmp = (i_libName + "_ppc64.so");
#endif
#else
        std::string tmp = (i_libName + "_x86_64.so");
#endif // end _LP64
#else
        std::string tmp = (i_libName + "_x86.so");
#endif //end __linux__
#else
#ifdef _LP64
        std::string tmp = (i_libName + "_aix64.so");
#else
        std::string tmp = (i_libName + "_aix.so");
#endif
#endif 

#if defined(ECMD_STATIC_FUNCTIONS)
        rc = dllFapi2QueryFileLocation(fapi2::FAPI_FILE_HWP, tmp, sharedLibPath, "default");
#else 
        rc = fapi2QueryFileLocation(fapi2::FAPI_FILE_HWP, tmp, sharedLibPath, "default");
#endif 
        if (rc)
        {
            FAPI_ERR("fapi2QueryFileLocation failed with rc = 0x%x\n", rc);
            return rc;
        }
    
        o_pLibHandle = dlopen(sharedLibPath.c_str(), RTLD_LAZY | flag);
        if (o_pLibHandle == NULL)
        {
            FAPI_ERR("dlopen error '%s'\n", dlerror());
            return ECMD_FAILURE;
        }

        return rc;
    }

    int openSharedLib(const std::string & i_libName, void * & o_pLibHandle)
    {
        return openSharedLib(i_libName, o_pLibHandle, 0);
    }

    // Gets a function symbol address from a dlopened shared library
    int getSymAddr(const char * i_pFuncName, void * i_pLibHandle, void * & o_pSymAddr)
    {
        o_pSymAddr = dlsym(i_pLibHandle, i_pFuncName);

        if (o_pSymAddr == NULL)
        {
            FAPI_ERR("dlsym error '%s'\n", dlerror());
            return ECMD_FAILURE;
        }

        return fapi2::FAPI2_RC_SUCCESS;
    }

    // dlcloses a shared library
    void closeSharedLib(void * i_pLibHandle)
    {
        int l_res = dlclose(i_pLibHandle);

        if (l_res)
        {
            FAPI_ERR("dlclose error '%s'\n", dlerror());
        }
    }
} // namespace fapi2plat
