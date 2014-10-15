// $Id$
// $Source$

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
 *                          mjjones     06/30/2011  Added #include
 *                          mjjones     07/05/2011  Removed const from data
 *                          mjjones     07/25/2011  Added support for FFDC
 *                          mjjones     09/22/2011  Added support for Error Info
 *                          mjjones     07/11/2012  Removed some tracing
 */

#include <string.h>
#include <stdlib.h>
#include <fapiReturnCodeDataRef.H>
//JFDEBUG #include <fapiUtil.H>
//JFDEBUG see comment below #include <fapiPlatTrace.H>

/* *****************************************************************************/
/* Note:                                                                       */
/* I manually added these macros, added the stdio.h include, and commented out */
/* the fapiPlatTrace.H include because the macros that are defined in          */
/* fapiPlatTrace.H call the fapiOutput*() APIs.  These APIs are in the archive */
/* and this file is in the shared lib and therefore causes undefined symbols in*/
/* the shared lib (ie libfapi.so)                                              */
/* -farrugia 08.25.2011                                                        */
/* *****************************************************************************/
#include <stdio.h>
//#include <fapiPlatTrace.H>
// Information traces (standard flight recorder that can wrap often)
#define FAPI_INF(_fmt_, _args_...) printf("FAPI TRC>: " _fmt_ "\n", ##_args_)

// Important traces (should not wrap often)
#define FAPI_IMP(_fmt_, _args_...) printf("FAPI IMP>: " _fmt_ "\n", ##_args_)

// Error traces (should not wrap often)
#define FAPI_ERR(_fmt_, _args_...) printf("FAPI ERR>: " _fmt_ "\n", ##_args_)

// Debug traces (can wrap often)
#define FAPI_DBG(_fmt_, _args_...) printf("FAPI DBG>: " _fmt_ "\n", ##_args_)



namespace fapi
{

//******************************************************************************
// Constructor
//******************************************************************************
ReturnCodeDataRef::ReturnCodeDataRef()
: iv_refCount(1),
  iv_pPlatData(NULL),
  iv_pErrorInfo(NULL)
{

}

//******************************************************************************
// Destructor
//******************************************************************************
ReturnCodeDataRef::~ReturnCodeDataRef()
{
    if (iv_refCount != 0)
    {
        FAPI_ERR("ReturnCodeDataRef. Bug. Destruct with refcount: %d",
                 iv_refCount);
        //fapiAssert(false);
        exit(false);
    }

    deletePlatData();
    delete iv_pErrorInfo;
    iv_pErrorInfo = NULL;
}

//******************************************************************************
// incRefCount function
//******************************************************************************
void ReturnCodeDataRef::incRefCount()
{
    //JFDEBUG	FAPI_DBG("ReturnCodeDataRef::incRefCount: iv_refCount = %d on entry", iv_refCount);
    iv_refCount++;
}

//******************************************************************************
// decRefCountCheckZero function
//******************************************************************************
bool ReturnCodeDataRef::decRefCountCheckZero()
{
    //JFDEBUG 	FAPI_DBG("ReturnCodeDataRef::decRefCountCheckZero: iv_refCount = %d on " "entry", iv_refCount);

    if (iv_refCount == 0)
    {
        FAPI_ERR("ReturnCodeDataRef. Bug. Dec with zero refcount");
        //fapiAssert(false);
        exit(false);
    }
    else
    {
        iv_refCount--;
    }
    return (iv_refCount == 0);
}

//******************************************************************************
// setPlatData function
//******************************************************************************
void ReturnCodeDataRef::setPlatData(void * i_pPlatData)
{
    // Delete any current PlatData
    if (iv_pPlatData)
    {
        FAPI_ERR("ReturnCodeDataRef. setPlatData when existing data");
        deletePlatData();
    }

    iv_pPlatData = i_pPlatData;
}

//******************************************************************************
// getPlatData function
//******************************************************************************
void * ReturnCodeDataRef::getPlatData() const
{
    return iv_pPlatData;
}

//******************************************************************************
// releasePlatData function
//******************************************************************************
void * ReturnCodeDataRef::releasePlatData()
{
    void * l_pPlatData = iv_pPlatData;
    iv_pPlatData = NULL;
    return l_pPlatData;
}

//******************************************************************************
// getErrorInfo function
//******************************************************************************
ErrorInfo * ReturnCodeDataRef::getErrorInfo()
{
    return iv_pErrorInfo;
}

//******************************************************************************
// getCreateErrorInfo function
//******************************************************************************
ErrorInfo & ReturnCodeDataRef::getCreateErrorInfo()
{
    if (iv_pErrorInfo == NULL)
    {
        iv_pErrorInfo = new ErrorInfo();
    }

    return *iv_pErrorInfo;
}

//******************************************************************************
// Overload Operator new function
//******************************************************************************
#ifdef FAPI_CUSTOM_MALLOC
void * ReturnCodeDataRef::operator new(size_t i_sz)
{
    return fapiMalloc(i_sz);
}
#endif

//******************************************************************************
// Overload Operator delete function
//******************************************************************************
#ifdef FAPI_CUSTOM_MALLOC
void ReturnCodeDataRef::operator delete(void * i_ptr)
{
    fapiFree(i_ptr);
}
#endif

}
