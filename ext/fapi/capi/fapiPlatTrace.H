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
 *  @file platTrace.H
 *
 *  @brief Defines the FAPI trace macros.
 *
 *  Note that platform code must provide the implementation.
 *
 *  FAPI has provided a default implementation of printfs. Platform code must
 *  provide an alternate implementation if needed.
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 *                          mjjones     04/13/2011  Created.
 *                          camvanng	05/31/2011  Added debug trace macro
 *
 */

#ifndef PLATTRACE_H_
#define PLATTRACE_H_

#include <stdio.h>

// Information traces (standard flight recorder that can wrap often)
#define FAPI_INF(_fmt_, _args_...) printf("FAPI TRC>: "_fmt_"\n", ##_args_)

// Important traces (should not wrap often)
#define FAPI_IMP(_fmt_, _args_...) printf("FAPI IMP>: "_fmt_"\n", ##_args_)

// Error traces (should not wrap often)
#define FAPI_ERR(_fmt_, _args_...) printf("FAPI ERR>: "_fmt_"\n", ##_args_)

// Debug traces (can wrap often)
#define FAPI_DBG(_fmt_, _args_...) printf("FAPI DBG>: "_fmt_"\n", ##_args_)

#endif // PLATTRACE_H_
