// IBM_PROLOG_BEGIN_TAG 
// This is an automatically generated prolog. 
//  
// fipsrefactordoc src/hwpf/fapi/fapiReturnCodeDataRef.C 1.3 
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
 *  @file fapiReturnCodeDataRef.C
 *
 *  @brief Implements the FAPI part of the ReturnCodeDataRef class.
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 *                          mjjones     04/13/2011  Created.
 *                          camvanng	05/31/2011  Added debug traces
 *
 */

#include <fapiReturnCodeDataRef.H>
#include <fapiUtil.H>

namespace fapi
{

//******************************************************************************
// Constructor
//******************************************************************************
ReturnCodeDataRef::ReturnCodeDataRef(const void * i_pData) :
    iv_refCount(1), iv_pData(i_pData)
{

}

//******************************************************************************
// Destructor
//******************************************************************************
ReturnCodeDataRef::~ReturnCodeDataRef()
{
    if (iv_refCount != 0)
    {
        FAPI_ERR("ReturnCodeDataRef. Bug. Destruct with refcount");
        fapiAssert(false);
    }
    else
    {
        // Call platform implemented deleteData
        (void) deleteData();
    }
}

//******************************************************************************
// incRefCount function
//******************************************************************************
void ReturnCodeDataRef::incRefCount()
{
	FAPI_DBG("ReturnCodeDataRef::incRefCount: iv_refCount = %i on entry", iv_refCount);
    iv_refCount++;
}

//******************************************************************************
// decRefCountCheckZero function
//******************************************************************************
bool ReturnCodeDataRef::decRefCountCheckZero()
{
	FAPI_DBG("ReturnCodeDataRef::decRefCountCheckZero: iv_refCount = %i on entry", iv_refCount);

    if (iv_refCount == 0)
    {
        FAPI_ERR("ReturnCodeDataRef. Bug. Dec with zero refcount");
        fapiAssert(false);
    }
    else
    {
        iv_refCount--;
    }
    return (iv_refCount == 0);
}

//******************************************************************************
// getData function
//******************************************************************************
const void * ReturnCodeDataRef::getData() const
{
    return iv_pData;
}

//******************************************************************************
// releaseData function
//******************************************************************************
const void * ReturnCodeDataRef::releaseData()
{
    const void * l_pData = iv_pData;
    iv_pData = NULL;
    return l_pData;
}

}
