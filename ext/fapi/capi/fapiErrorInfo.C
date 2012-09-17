/**
 *  @file fapiErrorInfo.C
 *
 *  @brief Implements the ErrorInfo structs and classes
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 *                          mjjones     08/05/2011  Created
 *                          mjjones     08/24/2011  Added ErrorInfoGard.
 *                          mjjones     09/22/2011  Major updates
 *                          mjjones     03/16/2012  Add FfdcType. Remove copy
 *                                                  ctor and assignment operator
 *                          mjjones     08/14/2012  Merge Callout/Deconfig/Gard
 *                                                  structures into one
 */

#include <fapiErrorInfo.H>
#include <string.h>

namespace fapi
{

//******************************************************************************
// ErrorInfoFfdc Constructor
//******************************************************************************
ErrorInfoFfdc::ErrorInfoFfdc(const void * i_pFfdc,
                             const uint32_t i_size,
                             const FfdcType i_type)
: iv_size(i_size), iv_type(i_type)
{
    iv_pFfdc = new uint8_t[i_size];
    memcpy(iv_pFfdc, i_pFfdc, i_size);
}

//******************************************************************************
// ErrorInfoFfdc Destructor
//******************************************************************************
ErrorInfoFfdc::~ErrorInfoFfdc()
{
    delete [] iv_pFfdc;
    iv_pFfdc = NULL;
}

//******************************************************************************
// ErrorInfoFfdc getData function
//******************************************************************************
const void * ErrorInfoFfdc::getData(uint32_t & o_size) const
{
    o_size = iv_size;
    return iv_pFfdc;
}

//******************************************************************************
// ErrorInfoFfdc getType function
//******************************************************************************
FfdcType ErrorInfoFfdc::getType() const
{
    return iv_type;
}

//******************************************************************************
// ErrorInfoCDG Constructor
//******************************************************************************
ErrorInfoCDG::ErrorInfoCDG(const Target & i_target)
: iv_target(i_target), iv_callout(false), iv_calloutPriority(PRI_LOW),
  iv_deconfigure(false), iv_gard(false)
{

}

//******************************************************************************
// ErrorInfo Destructor
//******************************************************************************
ErrorInfo::~ErrorInfo()
{
    for (ErrorInfo::ErrorInfoFfdcItr_t l_itr = iv_ffdcs.begin();
         l_itr != iv_ffdcs.end(); ++l_itr)
    {
        delete (*l_itr);
        (*l_itr) = NULL;
    }

    for (ErrorInfo::ErrorInfoCDGItr_t l_itr = iv_CDGs.begin();
         l_itr != iv_CDGs.end(); ++l_itr)
    {
        delete (*l_itr);
        (*l_itr) = NULL;
    }
}

//******************************************************************************
// ErrorInfo getCreateErrorInfoCDG
//******************************************************************************
ErrorInfoCDG & ErrorInfo::getCreateErrorInfoCDG(const Target & i_target)
{
    ErrorInfoCDG * l_pInfo = NULL;

    for (ErrorInfo::ErrorInfoCDGCItr_t l_itr = iv_CDGs.begin();
         l_itr != iv_CDGs.end(); ++l_itr)
    {
        if ((*l_itr)->iv_target == i_target)
        {
            l_pInfo = (*l_itr);
            break;
        }
    }

    if (l_pInfo == NULL)
    {
        l_pInfo = new ErrorInfoCDG(i_target);
        iv_CDGs.push_back(l_pInfo);
    }

    return *l_pInfo;
}

}
