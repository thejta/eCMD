// IBM_PROLOG_BEGIN_TAG 
// This is an automatically generated prolog. 
//  
// fipsrefactordoc src/hwpf/plat/fapiPlatTrace.H 1.2 
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
 *  @file fapiPlatTrace.H
 *
 *  @brief Defines the FAPI trace macros.
 *
 *  Note that platform code must provide the implementation.
 *
 *  FAPI has provided a default implementation of printfs. Platform code must
 *  provide an alternate implementation if needed.
 */


#ifndef PLATTRACE_H_
#define PLATTRACE_H_

#include <stdio.h>
#include "fapiClientCapi.H"
#include "fapiDllCapi.H"

// Information traces (standard flight recorder that can wrap often)
#define FAPI_INF(_fmt_, _args_...) \
do \
{   char inf_user_str[255]; \
    char inf_sum_str[512]; \
    snprintf(inf_sum_str, 128, "%s:%s:%d: ", __FILE__, __FUNCTION__, __LINE__); \
    snprintf(inf_user_str, 255, "" _fmt_"\n", ##_args_); \
    strncat(inf_sum_str, inf_user_str, 255); \
    fapiOutputInfo(inf_sum_str); \
} while(0)


// Important traces (should not wrap often)
#define FAPI_IMP(_fmt_, _args_...) \
do \
{   char imp_user_str[255]; \
    char imp_sum_str[512]; \
    snprintf(imp_sum_str, 128, "%s:%s:%d: ", __FILE__, __FUNCTION__, __LINE__); \
    snprintf(imp_user_str, 255, ""_fmt_"\n", ##_args_); \
    strncat(imp_sum_str, imp_user_str, 255); \
    fapiOutputImportant(imp_sum_str); \
} while(0)

// Error traces (should not wrap often)
#define FAPI_ERR(_fmt_, _args_...) \
do \
{   char err_user_str[255]; \
    char err_sum_str[512];  \
    snprintf(err_sum_str, 128, "%s:%s:%d: ", __FILE__, __FUNCTION__, __LINE__); \
    snprintf(err_user_str, 255, ""_fmt_"\n", ##_args_); \
    strncat(err_sum_str, err_user_str, 255); \
    fapiOutputError(err_sum_str); \
} while(0)

// Debug traces (can wrap often)
#define FAPI_DBG(_fmt_, _args_...) \
do \
{   char dbg_user_str[255]; \
    char dbg_sum_str[512];  \
    snprintf(dbg_sum_str, 128, "%s:%s:%d: ", __FILE__, __FUNCTION__, __LINE__); \
    snprintf(dbg_user_str, 255, ""_fmt_"\n", ##_args_); \
    strncat(dbg_sum_str, dbg_user_str, 255); \
    fapiOutputDebug(dbg_sum_str); \
} while(0)


// Common SCAN traces for hardware access 
#define FAPI_SCAN(_fmt_, _args_...) \
do \
{   char scan_user_str[255]; \
    snprintf(scan_user_str, 255, ""_fmt_"\n", ##_args_); \
    fapiOutputScanTrace(scan_user_str); \
} while(0)



#endif // PLATTRACE_H_
