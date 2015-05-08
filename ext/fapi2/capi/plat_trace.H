/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source$                                                            */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2011,2015                        */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
/**
 *  @file plat_trace.H
 *  @brief Defines the FAPI2 trace macros.
 *
 *  Note that platform code must provide the implementation.
 *
 *  FAPI has provided a default implementation. Platform code must
 *  provide an alternate implementation if needed.
 */

#ifndef FAPI2_PLATTRACE_H_
#define FAPI2_PLATTRACE_H_

#include <stdio.h>
#include <string.h>
#include <fapi2ClientCapi.H>

#ifdef _LP64
#ifndef UINT64_HEX_FORMAT
  #define UINT64_HEX_FORMAT "%lX"
#endif
#ifndef UINT64_HEX16_FORMAT
  #define UINT64_HEX16_FORMAT "%016lX"
#endif
#ifndef UINT64_HEX16_PRESC_FORMAT
  #define UINT64_HEX16_PRESC_FORMAT "%.16lX"
#endif
#ifndef UINT64_DEC_FORMAT
  #define UINT64_DEC_FORMAT "%ld"
#endif
#else
#ifndef UINT64_HEX_FORMAT
  #define UINT64_HEX_FORMAT "%llX"
#endif
#ifndef UINT64_HEX16_FORMAT
  #define UINT64_HEX16_FORMAT "%016llX"
#endif
#ifndef UINT64_HEX16_PRESC_FORMAT
  #define UINT64_HEX16_PRESC_FORMAT "%.16llX"
#endif
#ifndef UINT64_DEC_FORMAT
  #define UINT64_DEC_FORMAT "%lld"
#endif
#endif

// Why not a #define, why is this in the fapi2 namespace?
// To prevent problems with Cronus and the fapi1 definitions.
namespace fapi2
{
    static const uint32_t MAX_ECMD_STRING_LEN = 64;
};

// Information traces (go into fast trace buffer that can wrap often)
#ifdef _LP64
#define FAPI_INF(_fmt_, _args_...) \
if(fapi2IsOutputInfoEnabled()) \
{   char inf_user_str[255]; \
    char inf_sum_str[512]; \
    const char* input = "" _fmt_ "\n"; \
    size_t fmtSize = (strlen(input) + 1); \
    char fmt_cpy[fmtSize]; \
    fapi2FixOutputFormat(fmt_cpy, input, fmtSize); \
    snprintf(inf_sum_str, 128, "%s:%s:%d: ", __FILE__, __FUNCTION__, __LINE__); \
    snprintf(inf_user_str, 255, fmt_cpy, ##_args_); \
    strncat(inf_sum_str, inf_user_str, 255); \
    fapi2OutputInfo(inf_sum_str); \
}
#else
#define FAPI_INF(_fmt_, _args_...) \
if(fapi2IsOutputInfoEnabled()) \
{   char inf_user_str[255]; \
    char inf_sum_str[512]; \
    snprintf(inf_sum_str, 128, "%s:%s:%d: ", __FILE__, __FUNCTION__, __LINE__); \
    snprintf(inf_user_str, 255, "" _fmt_ "\n", ##_args_); \
    strncat(inf_sum_str, inf_user_str, 255); \
    fapi2OutputInfo(inf_sum_str); \
}
#endif

// Important traces (go into slow trace buffer that should not wrap often)
#ifdef _LP64
#define FAPI_IMP(_fmt_, _args_...) \
if(fapi2IsOutputInfoEnabled()) \
{   char imp_user_str[255]; \
    char imp_sum_str[512]; \
    const char* input = "" _fmt_ "\n"; \
    size_t fmtSize = (strlen(input) + 1);	\
    char fmt_cpy[fmtSize]; \
    fapi2FixOutputFormat(fmt_cpy, input, fmtSize); \
    snprintf(imp_sum_str, 128, "%s:%s:%d: ", __FILE__, __FUNCTION__, __LINE__); \
    snprintf(imp_user_str, 255, fmt_cpy, ##_args_); \
    strncat(imp_sum_str, imp_user_str, 255); \
    fapi2OutputImportant(imp_sum_str); \
}
#else
#define FAPI_IMP(_fmt_, _args_...) \
if(fapi2IsOutputInfoEnabled()) \
{   char imp_user_str[255]; \
    char imp_sum_str[512]; \
    snprintf(imp_sum_str, 128, "%s:%s:%d: ", __FILE__, __FUNCTION__, __LINE__); \
    snprintf(imp_user_str, 255, "" _fmt_ "\n", ##_args_); \
    strncat(imp_sum_str, imp_user_str, 255); \
    fapi2OutputImportant(imp_sum_str); \
}
#endif

// Error traces (go into slow trace buffer that should not wrap often)
#ifdef _LP64
#define FAPI_ERR(_fmt_, _args_...) \
do \
{   char err_user_str[255]; \
    char err_sum_str[512];  \
    const char* input = "" _fmt_ "\n"; \
    size_t fmtSize = (strlen(input) + 1); \
    char fmt_cpy[fmtSize]; \
    fapi2FixOutputFormat(fmt_cpy, input, fmtSize); \
    snprintf(err_sum_str, 128, "%s:%s:%d: ", __FILE__, __FUNCTION__, __LINE__); \
    snprintf(err_user_str, 255, fmt_cpy, ##_args_); \
    strncat(err_sum_str, err_user_str, 255); \
    fapi2OutputError(err_sum_str); \
} while(0)
#else
#define FAPI_ERR(_fmt_, _args_...) \
do \
{   char err_user_str[255]; \
    char err_sum_str[512];  \
    snprintf(err_sum_str, 128, "%s:%s:%d: ", __FILE__, __FUNCTION__, __LINE__); \
    snprintf(err_user_str, 255, "" _fmt_ "\n", ##_args_); \
    strncat(err_sum_str, err_user_str, 255); \
    fapi2OutputError(err_sum_str); \
} while(0)
#endif

// Debug traces (go into fast trace buffer that can wrap often)
#ifdef _LP64
#define FAPI_DBG(_fmt_, _args_...) \
if (fapi2IsOutputDebugEnabled()) \
{   char dbg_user_str[255]; \
    char dbg_sum_str[512];  \
    const char* input = "" _fmt_ "\n"; \
    size_t fmtSize = (strlen(input) + 1);	\
    char fmt_cpy[fmtSize]; \
    fapi2FixOutputFormat(fmt_cpy, input, fmtSize); \
    snprintf(dbg_sum_str, 128, "%s:%s:%d: ", __FILE__, __FUNCTION__, __LINE__); \
    snprintf(dbg_user_str, 255, fmt_cpy, ##_args_); \
    strncat(dbg_sum_str, dbg_user_str, 255); \
    fapi2OutputDebug(dbg_sum_str); \
}
#else
#define FAPI_DBG(_fmt_, _args_...) \
if (fapi2IsOutputDebugEnabled()) \
{   char dbg_user_str[255]; \
    char dbg_sum_str[512];  \
    snprintf(dbg_sum_str, 128, "%s:%s:%d: ", __FILE__, __FUNCTION__, __LINE__); \
    snprintf(dbg_user_str, 255, "" _fmt_ "\n", ##_args_); \
    strncat(dbg_sum_str, dbg_user_str, 255); \
    fapi2OutputDebug(dbg_sum_str); \
}
#endif

// Scan traces
#define FAPI_SCAN(_fmt_, _args_...) \
do \
{   char scan_user_str[255]; \
    snprintf(scan_user_str, 255, "" _fmt_ "\n", ##_args_); \
    fapi2OutputScanTrace(scan_user_str); \
} while(0)

// Manufacturing traces
#ifdef _LP64
#define FAPI_MFG(_fmt_, _args_...) \
if (fapi2IsOutputManufacturingEnabled()) \
{   char mfg_user_str[255]; \
    char mfg_sum_str[512];  \
    const char* input = "" _fmt_ "\n"; \
    size_t fmtSize = (strlen(input) + 1);	\
    char fmt_cpy[fmtSize]; \
    fapi2FixOutputFormat(fmt_cpy, input, fmtSize); \
    snprintf(mfg_sum_str, 128, "%s:%s:%d: ", __FILE__, __FUNCTION__, __LINE__); \
    snprintf(mfg_user_str, 255, fmt_cpy, ##_args_); \
    strncat(mfg_sum_str, mfg_user_str, 255); \
    fapi2OutputManufacturing(mfg_sum_str); \
}
#else
#define FAPI_MFG(_fmt_, _args_...) \
if (fapi2IsOutputManufacturingEnabled()) \
{   char mfg_user_str[255]; \
    char mfg_sum_str[512];  \
    snprintf(mfg_sum_str, 128, "%s:%s:%d: ", __FILE__, __FUNCTION__, __LINE__); \
    snprintf(mfg_user_str, 255, "" _fmt_ "\n", ##_args_); \
    strncat(mfg_sum_str, mfg_user_str, 255); \
    fapi2OutputManufacturing(mfg_sum_str); \
}
#endif

#endif // FAPI2_PLATTRACE_H_
