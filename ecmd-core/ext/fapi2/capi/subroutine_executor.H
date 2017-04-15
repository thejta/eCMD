/* IBM_PROLOG_BEGIN_TAG                                                   */
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
/* IBM_PROLOG_END_TAG                                                     */
///
/// @file subroutine_executor.H
///
/// @brief Defines the PLAT Subroutine Executor Macro.
///
/// The PLAT Subroutine Executor macro is called by
/// FAPI_CALL_SUBROUTINE when a hardware procedure when
/// a subroutine is needed, typicaly a chipop function.
///
/// Example implementation of plat code

#ifndef SUBROUTINEEXECUTOR_H_
#define SUBROUTINEEXECUTOR_H_

#include <fapi2_subroutine_executor.H>

#include <hwp_executor.H>

#include <plat_trace.H>

/**
 * @brief Subroutine Executor macro example code - Platforms will need to
 *        implement as needed for their enviroment.
 *
 * This macro calls a PLAT macro which will do any platform specific work to
 * execute the Subroutine (e.g. dlopening a shared library)
 */

/*
    const char * l_function = "fapi_subroutine_enabled";
    const uint32_t l_version = 0x1;
    const char * l_subroutine
    uint32_t l_enabled
*/

#if 1
#define FAPI_PLAT_CALL_SUBROUTINE(RC, FUNC, _args_...) \
{\
    void * l_pLibHandle = NULL; \
    void * l_pSymAddr = NULL; \
    std::list<void *> l_args; \
    const char * l_function = "fapi_subroutine_enabled"; \
    l_args.push_back((void *)l_function); \
    const uint32_t l_version = 0x1; \
    l_args.push_back((void *)&l_version); \
    const char * l_subroutine = #FUNC; \
    l_args.push_back((void *)l_subroutine); \
    uint32_t l_enabled = 0; \
    l_args.push_back(&l_enabled); \
    uint32_t l_u32_rc_xyzabc = fapi2GeneralApi(l_args); \
    const uint32_t l_not_supported = ECMD_FUNCTION_NOT_SUPPORTED; \
    if ((l_rc == l_not_supported) || (l_enabled == 0)) \
    { \
        l_u32_rc_xyzabc = fapi2plat::openSharedLib(#FUNC, l_pLibHandle); \
    } \
    else \
    { \
        l_u32_rc_xyzabc = fapi2plat::openSharedLib("sub_" #FUNC, l_pLibHandle); \
    } \
    if (l_u32_rc_xyzabc == 0) \
    { \
        l_u32_rc_xyzabc = fapi2plat::getSymAddr(#FUNC, l_pLibHandle, l_pSymAddr); \
        if (l_u32_rc_xyzabc == 0) \
        { \
            FUNC##_FP_t l_pFunc = reinterpret_cast<FUNC##_FP_t> (l_pSymAddr); \
            RC = l_pFunc(_args_); \
        } \
	else \
	{ \
          RC = (fapi2::ReturnCodes) l_u32_rc_xyzabc; \
	} \
        fapi2plat::closeSharedLib(l_pLibHandle); \
    } \
    else \
    { \
      RC = (fapi2::ReturnCodes) l_u32_rc_xyzabc; \
    } \
}

namespace fapi2plat
{
    int openSharedLib(const std::string & i_libName, void * & o_pLibHandle);
    int getSymAddr(const char * i_pFuncName, void * i_pLibHandle, void * & o_pSymAddr);
    void closeSharedLib(void * i_pLibHandle);
}
#else
#define FAPI_PLAT_CALL_SUBROUTINE(RC, FUNC, _args...) \
        FAPI_PLAT_EXEC_HWP(RC, FUNC, ##_args)
#endif

#endif
