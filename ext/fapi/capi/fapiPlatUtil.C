// IBM_PROLOG_BEGIN_TAG 
// This is an automatically generated prolog. 
//  
// fipsrefactordoc src/hwpf/plat/fapiPlatUtil.C 1.3 
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
 *  @file platUtil.C
 *
 *  @brief Implements the fapiUtil.H utility functions.
 *
 *  Note that platform code must provide the implementation.
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 *                          mjjones     04/13/2011  Created.
 *
 */
#include <stdio.h>
#include <fapiReturnCodes.H>
#include <fapiReturnCode.H>
//******************************************************************************
// fapiAssert
//******************************************************************************
using namespace fapi;
void fapiAssert(bool i_expression)
{
  //if (!i_expression) exit(FAPI_RC_ASSERT);
  if (!i_expression) exit(0);
}

ReturnCode fapiDelay(uint64_t i_nanoSeconds, uint64_t, uint64_t i_simCycles){
  return 0;
}

