// IBM_PROLOG_BEGIN_TAG 
// This is an automatically generated prolog. 
//  
// fipsrefactordoc src/hwpf/plat/fapiPlatReturnCodeDataRef.C 1.3 
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
 *  @file platReturnCodeDataRef.C
 *
 *  @brief Implements the platform part of the ReturnCodeDataRef class.
 *
 *  Note that platform code must provide the implementation. FAPI has provided
 *  an example for platforms that do not attach ReturnCodeData to a ReturnCode.
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 *                          mjjones     04/13/2011  Created.
 *                          camvanng	05/31/2011	Added debug trace
 *
 */

#include <fapiReturnCodeDataRef.H>
#include <fapiPlatTrace.H>

namespace fapi
{

//******************************************************************************
// deleteData function
//******************************************************************************
void ReturnCodeDataRef::deleteData()
{
	FAPI_DBG("ReturnCodeDataRef::deleteData");

    // If platform does not attach ReturnCodeData to a ReturnCode then it can
    // do nothing (this function will never be called), but if it does then it
    // must be deleted with the correct type.

   
    // In the eCMD fapi extention we'll do nothing for now - farrugia
    //    delete (reinterpret_cast<TYPE *>(const_cast<void *>(iv_pData)));
}

}
