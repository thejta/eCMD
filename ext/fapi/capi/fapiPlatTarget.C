// IBM_PROLOG_BEGIN_TAG 
// This is an automatically generated prolog. 
//  
// fipsrefactordoc src/hwpf/plat/fapiPlatTarget.C 1.2 
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
 *  @file platTarget.C
 *
 *  @brief Implements the platform part of the Target class.
 *
 *  Note that platform code must provide the implementation.
 *
 *  FAPI has provided a default implementation for platforms that use the
 *  handle pointer to point to a Component that is not created/deleted when a
 *  Target object is created/deleted (i.e. two Target objects that reference
 *  the same component have the same pointer). It could be possible for a
 *  platform specific ID structure to be created and pointed to each time a new
 *  Target is created, in that case, the pointed to object's type needs to be
 *  be known in order to do a deep compare/copy and a delete.
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 *                          mjjones     04/13/2011  Created. Based on Hlava prototype
 *
 */

#include <fapiTarget.H>

namespace fapi
{

//******************************************************************************
// Compare the handle
//
// If the pointers point to the same component then the handles are the same
//******************************************************************************
bool Target::compareHandle(const Target & i_right) const
{
    return (iv_pHandle == i_right.iv_pHandle);
}

//******************************************************************************
// Copy the handle
//
// Note shallow copy of iv_pHandle. Both Targets point to the same component
//******************************************************************************
void Target::copyHandle(const Target & i_right)
{
    iv_pHandle = i_right.iv_pHandle;
}

//******************************************************************************
// Delete the handle
//******************************************************************************
void Target::deleteHandle()
{
    // Intentionally does nothing. The component must not be deleted
}

}
