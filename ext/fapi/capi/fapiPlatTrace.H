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
{ char user_str[255]; \
char sum_str[512];  \
snprintf(sum_str, 128, "%s:%d: ", __FUNCTION__, __LINE__); \
snprintf(user_str, 255, "" _fmt_"\n", ##_args_); \
strncat(sum_str, user_str, 255); \
fapiOutputInfo(sum_str); } 

// Important traces (should not wrap often)
#define FAPI_IMP(_fmt_, _args_...) \
{ char user_str[255]; \
char sum_str[512];  \
snprintf(sum_str, 128, "%s:%d: ", __FUNCTION__, __LINE__); \
snprintf(user_str, 255, ""_fmt_"\n", ##_args_); \
strncat(sum_str, user_str, 255); \
fapiOutputImportant(sum_str); } 

// Error traces (should not wrap often)
#define FAPI_ERR(_fmt_, _args_...) \
{ char user_str[255]; \
char sum_str[512];  \
snprintf(sum_str, 128, "%s:%d: ", __FUNCTION__, __LINE__); \
snprintf(user_str, 255, ""_fmt_"\n", ##_args_); \
strncat(sum_str, user_str, 255); \
fapiOutputError(sum_str); } 

// Debug traces (can wrap often)
#define FAPI_DBG(_fmt_, _args_...) \
{ char user_str[255]; \
char sum_str[512];  \
snprintf(sum_str, 128, "%s:%d: ", __FUNCTION__, __LINE__); \
snprintf(user_str, 255, ""_fmt_"\n", ##_args_); \
strncat(sum_str, user_str, 255); \
fapiOutputDebug(sum_str); } 


// Common SCAN traces for hardware access 
#define FAPI_SCAN(_fmt_, _args_...) \
{ char user_str[255]; \
snprintf(user_str, 255, ""_fmt_"\n", ##_args_); \
fapiOutputScanTrace(user_str); } 



#endif // PLATTRACE_H_
