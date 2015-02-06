// IBM_PROLOG_BEGIN_TAG 
// This is an automatically generated prolog. 
//  
// fipsrefactordoc src/hwpf/plat/fapi2PlatTrace.H 1.2 
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
 *  @file fapi2PlatTrace.H
 *
 *  @brief Defines the FAPI trace macros.
 *
 *  Note that platform code must provide the implementation.
 *
 *  FAPI has provided a default implementation of printfs. Platform code must
 *  provide an alternate implementation if needed.
 */


#ifndef FAPI2PLATTRACE_H
#define FAPI2PLATTRACE_H

#include <stdio.h>
#include <string.h>
#include <fapi2ClientCapi.H>
#include <fapi2DllCapi.H>

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


// Information traces (standard flight recorder that can wrap often)
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


// Important traces (should not wrap often)
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

// Error traces (should not wrap often)
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

// Debug traces (can wrap often)
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


// Common SCAN traces for hardware access 
#define FAPI_SCAN(_fmt_, _args_...) \
do \
{   char scan_user_str[255]; \
    snprintf(scan_user_str, 255, "" _fmt_ "\n", ##_args_); \
    fapi2OutputScanTrace(scan_user_str); \
} while(0)



#endif // FAPI2PLATTRACE_H
