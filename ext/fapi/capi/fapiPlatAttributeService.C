// IBM_PROLOG_BEGIN_TAG 
// This is an automatically generated prolog. 
//  
// fipsrefactordoc src/hwpf/fapi/fapiAttributeService.C 1.1 
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
 *  @file fapiAttributeService.C
 *
 *  @brief Implements the AttributeService functions.
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 *                          mjjones     06/07/2011  Created.
 */
#include <stdio.h>
#include "fapiAttributeService.H"
#include "fapiPlatTrace.H"

#include "fapiClientCapi.H"
#include "fapiHwAccess.H"
#include "fapiPlatAttributeService.H"


#include "ecmdReturnCodes.H"


namespace fapi {

//******************************************************************************
// Get string
//******************************************************************************
template<>
ReturnCode _get<char *> (const AttributeId i_id,
                         const Target * const i_pTarget,
                         char * & o_value)
{
   fapi::AttributeData o_data;
   fapi::ReturnCode l_fapi_rc; 
   uint32_t l_ecmd_rc = ECMD_SUCCESS; 

   o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_STRING;
   l_ecmd_rc = fapiGetAttribute(*(i_pTarget), i_id, o_data); 
   l_fapi_rc.setEcmdError(l_ecmd_rc);
   if (l_fapi_rc)
   { 
     return l_fapi_rc;
   }

   o_value = o_data.faString;
   return l_fapi_rc;
}

//******************************************************************************
// Get uint8_t
//******************************************************************************
template<>
ReturnCode _get<uint8_t> (const AttributeId i_id,
                          const Target * const i_pTarget,
                          uint8_t & o_value)
{
   fapi::AttributeData o_data;
   fapi::ReturnCode l_fapi_rc; 
   uint32_t l_ecmd_rc = ECMD_SUCCESS; 

   o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT8;
   l_ecmd_rc = fapiGetAttribute(*(i_pTarget), i_id, o_data); 
   l_fapi_rc.setEcmdError(l_ecmd_rc);
   if (l_fapi_rc)
   { 
     return l_fapi_rc;
   }

   o_value = o_data.faUint8;
   return l_fapi_rc;
}

//******************************************************************************
// Get uint32_t
//******************************************************************************
template<>
ReturnCode _get<uint32_t> (const AttributeId i_id,
                           const Target * const i_pTarget,
                           uint32_t & o_value)
{
   fapi::AttributeData o_data;
   fapi::ReturnCode l_fapi_rc; 
   uint32_t l_ecmd_rc = ECMD_SUCCESS;

   o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT32;
   l_ecmd_rc = fapiGetAttribute(*(i_pTarget), i_id, o_data); 
   l_fapi_rc.setEcmdError(l_ecmd_rc);
   if (l_fapi_rc)
   { 
     return l_fapi_rc;
   }

   o_value = o_data.faUint32;
   return l_fapi_rc;
}

//******************************************************************************
// Get uint64_t
//******************************************************************************
template<>
ReturnCode _get<uint64_t> (const AttributeId i_id,
                           const Target * const i_pTarget,
                           uint64_t & o_value)
{
   fapi::AttributeData o_data;
   fapi::ReturnCode l_fapi_rc; 
   uint32_t l_ecmd_rc = ECMD_SUCCESS;

   o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT64;
   l_ecmd_rc = fapiGetAttribute(*(i_pTarget), i_id, o_data); 
   l_fapi_rc.setEcmdError(l_ecmd_rc);
   if (l_fapi_rc)
   { 
     return l_fapi_rc;
   }

   o_value = o_data.faUint64;
   return l_fapi_rc;
}

//******************************************************************************
// Get uint8_t array
//******************************************************************************
ReturnCode _getAttributeArrayShort(const AttributeId i_id, 
                                   const Target * const i_pTarget, 
                                   uint8_t * o_pValues)
{
   fapi::AttributeData o_data;
   fapi::ReturnCode l_fapi_rc; 
   uint32_t l_ecmd_rc = ECMD_SUCCESS;

   o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT8ARY;
   o_data.faUint8ary = o_pValues;

   l_ecmd_rc = fapiGetAttribute(*(i_pTarget), i_id, o_data); 
   l_fapi_rc.setEcmdError(l_ecmd_rc);
   return l_fapi_rc;
}

//******************************************************************************
// Get uint32_t array
//******************************************************************************
ReturnCode _getAttributeArrayWord(const AttributeId i_id, 
                                  const Target * const i_pTarget, 
                                  uint32_t * o_pValues)
{
   fapi::AttributeData o_data;
   fapi::ReturnCode l_fapi_rc; 
   uint32_t l_ecmd_rc = ECMD_SUCCESS; 

   o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT32ARY;
   o_data.faUint32ary = o_pValues;

   l_ecmd_rc = fapiGetAttribute(*(i_pTarget), i_id, o_data); 
   l_fapi_rc.setEcmdError(l_ecmd_rc);
   return l_fapi_rc;
}

//******************************************************************************
// Get uint64_t array
//******************************************************************************
ReturnCode _getAttributeArrayDoubleWord(const AttributeId i_id, 
                                        const Target * const i_pTarget, 
                                        uint64_t * o_pValues) 
{
   fapi::AttributeData o_data;
   fapi::ReturnCode l_fapi_rc; 
   uint32_t l_ecmd_rc = ECMD_SUCCESS; 

   o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT64ARY;
   o_data.faUint64ary = o_pValues;

   l_ecmd_rc = fapiGetAttribute(*(i_pTarget), i_id, o_data); 
   l_fapi_rc.setEcmdError(l_ecmd_rc);
   return l_fapi_rc;
}

//******************************************************************************
// Set uint8_t
//******************************************************************************
template<>
ReturnCode _set<uint8_t> (const AttributeId i_id,
                           const Target * const i_pTarget,
                           uint8_t & i_value)
{
   fapi::AttributeData i_data;
   fapi::ReturnCode l_fapi_rc; 
   uint32_t l_ecmd_rc = ECMD_SUCCESS; 

   i_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT8;
   i_data.faUint8 = i_value;

   l_ecmd_rc = fapiSetAttribute(*(i_pTarget), i_id, i_data);
   l_fapi_rc.setEcmdError(l_ecmd_rc);
   return l_fapi_rc;
}

//******************************************************************************
// Set uint32_t
//******************************************************************************
template<>
ReturnCode _set<uint32_t> (const AttributeId i_id,
                           const Target * const i_pTarget,
                           uint32_t & i_value)
{
   fapi::AttributeData i_data;
   fapi::ReturnCode l_fapi_rc; 
   uint32_t l_ecmd_rc = ECMD_SUCCESS; 

   i_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT32;
   i_data.faUint32 = i_value;

   l_ecmd_rc = fapiSetAttribute(*(i_pTarget), i_id, i_data);
   l_fapi_rc.setEcmdError(l_ecmd_rc);
   return l_fapi_rc;
}

//******************************************************************************
// Set uint64_t
//******************************************************************************
template<>
ReturnCode _set<uint64_t> (const AttributeId i_id,
                           const Target * const i_pTarget,
                           const uint64_t & i_value)
{
   fapi::AttributeData i_data;
   fapi::ReturnCode l_fapi_rc; 
   uint32_t l_ecmd_rc = ECMD_SUCCESS; 

   i_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT64;
   i_data.faUint64 = i_value;

   l_ecmd_rc = fapiSetAttribute(*(i_pTarget), i_id, i_data);
   l_fapi_rc.setEcmdError(l_ecmd_rc);
   return l_fapi_rc;
}

//******************************************************************************
// Set uint8_t array
//******************************************************************************
ReturnCode _setAttributeArrayShort(const AttributeId i_id, 
                                   const Target * const i_pTarget, 
                                   uint8_t * i_pValues)
{
   fapi::AttributeData i_data;
   fapi::ReturnCode l_fapi_rc; 
   uint32_t l_ecmd_rc = ECMD_SUCCESS; 

   i_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT8ARY;
   i_data.faUint8ary = i_pValues;

   l_ecmd_rc = fapiSetAttribute(*(i_pTarget), i_id, i_data); 
   l_fapi_rc.setEcmdError(l_ecmd_rc);
   return l_fapi_rc;
}

//******************************************************************************
// Set uint32_t array
//******************************************************************************
ReturnCode _setAttributeArrayWord(const AttributeId i_id, 
                                  const Target * const i_pTarget, 
                                  uint32_t * i_pValues) 
{
   fapi::AttributeData i_data;
   fapi::ReturnCode l_fapi_rc; 
   uint32_t l_ecmd_rc = ECMD_SUCCESS; 

   i_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT32ARY;
   i_data.faUint32ary = i_pValues;

   l_ecmd_rc = fapiSetAttribute(*(i_pTarget), i_id, i_data);
   l_fapi_rc.setEcmdError(l_ecmd_rc);
   return l_fapi_rc;
}

//******************************************************************************
// Set uint64_t array
//******************************************************************************
ReturnCode _setAttributeArrayDoubleWord(const AttributeId i_id, 
                                  const Target * const i_pTarget, 
                                  uint64_t * i_pValues) 
{
   fapi::AttributeData i_data;
   fapi::ReturnCode l_fapi_rc; 
   uint32_t l_ecmd_rc = ECMD_SUCCESS; 

   i_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT64ARY;
   i_data.faUint64ary = i_pValues;

   l_ecmd_rc = fapiSetAttribute(*(i_pTarget), i_id, i_data); 
   l_fapi_rc.setEcmdError(l_ecmd_rc);
   return l_fapi_rc;
}

//******************************************************************************
// Get string
//******************************************************************************
template<>
ReturnCode _set<char *> (const AttributeId i_id,
                         const Target * const i_pTarget,
                         char * & i_value)
{
   fapi::AttributeData o_data;
   fapi::ReturnCode l_fapi_rc; 
   uint32_t l_ecmd_rc = ECMD_SUCCESS; 

   o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_STRING;
   o_data.faString = i_value;

   l_ecmd_rc = fapiSetAttribute(*(i_pTarget), i_id, o_data); 
   l_fapi_rc.setEcmdError(l_ecmd_rc);
   return l_fapi_rc;
}

} //End Namespace
