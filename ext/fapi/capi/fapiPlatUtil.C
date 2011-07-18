// IBM_PROLOG_BEGIN_TAG 
// This is an automatically generated prolog. 
//  
// fipsrefactordoc src/hwsv/server/hwpf/plat/fapiPlatUtil.C 1.1 
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

#include <fapiReturnCode.H>
#include <fapiTarget.H>
#include <ecmdReturnCodes.H>

namespace fapi
{

//******************************************************************************
// fapiAssert
//******************************************************************************
void fapiAssert(bool i_expression)
{

}

//******************************************************************************
// fapiLockHardware
//******************************************************************************
ReturnCode fapiLockHardware(const Target& i_target)
{
  ReturnCode rc;
  rc = ECMD_FUNCTION_NOT_SUPPORTED;
  return rc;
}

//******************************************************************************
// fapiUnlockHardware
//******************************************************************************
ReturnCode fapiUnlockHardware(const Target& i_target)
{
  return ECMD_FUNCTION_NOT_SUPPORTED;
}

}
