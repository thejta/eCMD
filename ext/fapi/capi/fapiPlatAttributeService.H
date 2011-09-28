/**
 *  @file fapiPlatAttributeService.H
 *
 *  @brief Defines the PLAT attribute access macros and defines which macro
 *         handles each attribute.
 *
 *  Note that platform code must provide the code.
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 *                          mjjones     06/27/2011  Created.
 */

#ifndef FAPIPLATATTRIBUTESERVICE_H_
#define FAPIPLATATTRIBUTESERVICE_H_
#include <stdint.h>
#include <fapiAttributeIds.H>
#include <fapiClientCapi.H>
#include <fapiStructs.H>

#define ATTR_SCRATCH_UINT8_1_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SCRATCH_UINT8_1_SETMACRO PLAT_ATTR_SET_GLOBAL_INT

#define ATTR_SCRATCH_UINT8_2_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SCRATCH_UINT8_2_SETMACRO PLAT_ATTR_SET_GLOBAL_INT

#define ATTR_SCRATCH_UINT32_1_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SCRATCH_UINT32_1_SETMACRO PLAT_ATTR_SET_GLOBAL_INT

#define ATTR_SCRATCH_UINT32_2_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SCRATCH_UINT32_2_SETMACRO PLAT_ATTR_SET_GLOBAL_INT

#define ATTR_SCRATCH_UINT64_1_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SCRATCH_UINT64_1_SETMACRO PLAT_ATTR_SET_GLOBAL_INT

#define ATTR_SCRATCH_UINT64_2_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SCRATCH_UINT64_2_SETMACRO PLAT_ATTR_SET_GLOBAL_INT

#define ATTR_SCRATCH_UINT8_ARRAY_1_GETMACRO      PLAT_ATTR_GET_UINT8_1D_ARRAY
#define ATTR_SCRATCH_UINT8_ARRAY_2_GETMACRO      PLAT_ATTR_GET_UINT8_3D_ARRAY
#define ATTR_SCRATCH_UINT8_ARRAY_1_SETMACRO      PLAT_ATTR_SET_UINT8_1D_ARRAY
#define ATTR_SCRATCH_UINT8_ARRAY_2_SETMACRO      PLAT_ATTR_SET_UINT8_3D_ARRAY

#define ATTR_SCRATCH_UINT32_ARRAY_1_GETMACRO      PLAT_ATTR_GET_UINT32_1D_ARRAY
#define ATTR_SCRATCH_UINT32_ARRAY_2_GETMACRO      PLAT_ATTR_GET_UINT32_2D_ARRAY
#define ATTR_SCRATCH_UINT32_ARRAY_1_SETMACRO      PLAT_ATTR_SET_UINT32_1D_ARRAY
#define ATTR_SCRATCH_UINT32_ARRAY_2_SETMACRO      PLAT_ATTR_SET_UINT32_2D_ARRAY

#define ATTR_SCRATCH_UINT64_ARRAY_1_GETMACRO      PLAT_ATTR_GET_UINT64_1D_ARRAY
#define ATTR_SCRATCH_UINT64_ARRAY_2_GETMACRO      PLAT_ATTR_GET_UINT64_2D_ARRAY
#define ATTR_SCRATCH_UINT64_ARRAY_1_SETMACRO      PLAT_ATTR_SET_UINT64_1D_ARRAY
#define ATTR_SCRATCH_UINT64_ARRAY_2_SETMACRO      PLAT_ATTR_SET_UINT64_2D_ARRAY




/******************************************************************************/
/*                          * Global macros *                                 */
/* These macros are called by the macros above to invoke the appropriate API. */
/* These macros do not need to change when a new attribute is introduced.     */
/******************************************************************************/

/* global get uint8_t 1D array macro */
#define PLAT_ATTR_GET_UINT8_1D_ARRAY(ID, PTARGET, VAL) \
     _getAttributeArrayShort(fapi::ID, PTARGET, VAL)
/* global set uint8_t 1D array macro */
#define PLAT_ATTR_SET_UINT8_1D_ARRAY(ID, PTARGET, VAL) \
     _setAttributeArrayShort(fapi::ID, PTARGET, VAL)

/* global get uint8_t 2D array macro */
#define PLAT_ATTR_GET_UINT8_2D_ARRAY(ID, PTARGET, VAL) \
     _getAttributeArrayShort(fapi::ID, PTARGET, VAL[0])
/* global set uint8_t 2D array macro */
#define PLAT_ATTR_SET_UINT8_2D_ARRAY(ID, PTARGET, VAL) \
     _setAttributeArrayShort(fapi::ID, PTARGET, VAL[0])

/* global get uint8_t 3D array macro */
#define PLAT_ATTR_GET_UINT8_3D_ARRAY(ID, PTARGET, VAL) \
     _getAttributeArrayShort(fapi::ID, PTARGET, VAL[0][0])
/* global set uint8_t 3D array macro */
#define PLAT_ATTR_SET_UINT8_3D_ARRAY(ID, PTARGET, VAL) \
     _setAttributeArrayShort(fapi::ID, PTARGET, VAL[0][0])

/* global get uint8_t 4D array macro */
#define PLAT_ATTR_GET_UINT8_4D_ARRAY(ID, PTARGET, VAL) \
     _getAttributeArrayShort(fapi::ID, PTARGET, VAL[0][0][0])
/* global set uint8_t 4D array macro */
#define PLAT_ATTR_SET_UINT8_4D_ARRAY(ID, PTARGET, VAL) \
     _setAttributeArrayShort(fapi::ID, PTARGET, VAL[0][0][0])




/* global get uint32_t 1D array macro */
#define PLAT_ATTR_GET_UINT32_1D_ARRAY(ID, PTARGET, VAL) \
     _getAttributeArrayWord(fapi::ID, PTARGET, VAL)
/* global set uint32_t 1D array macro */
#define PLAT_ATTR_SET_UINT32_1D_ARRAY(ID, PTARGET, VAL) \
     _setAttributeArrayWord(fapi::ID, PTARGET, VAL)

/* global get uint32_t 2D array macro */
#define PLAT_ATTR_GET_UINT32_2D_ARRAY(ID, PTARGET, VAL) \
     _getAttributeArrayWord(fapi::ID, PTARGET, VAL[0])
/* global set uint32_t 2D array macro */
#define PLAT_ATTR_SET_UINT32_2D_ARRAY(ID, PTARGET, VAL) \
     _setAttributeArrayWord(fapi::ID, PTARGET, VAL[0])

/* global get uint32_t 3D array macro */
#define PLAT_ATTR_GET_UINT32_3D_ARRAY(ID, PTARGET, VAL) \
     _getAttributeArrayWord(fapi::ID, PTARGET, VAL[0][0])
/* global set uint32_t 3D array macro */
#define PLAT_ATTR_SET_UINT32_3D_ARRAY(ID, PTARGET, VAL) \
     _setAttributeArrayWord(fapi::ID, PTARGET, VAL[0][0])

/* global get uint32_t 4D array macro */
#define PLAT_ATTR_GET_UINT32_4D_ARRAY(ID, PTARGET, VAL) \
     _getAttributeArrayWord(fapi::ID, PTARGET, VAL[0][0][0])
/* global set uint32_t 4D array macro */
#define PLAT_ATTR_SET_UINT32_4D_ARRAY(ID, PTARGET, VAL) \
     _setAttributeArrayWord(fapi::ID, PTARGET, VAL[0][0][0])


/* global get uint64_t 1D array macro */
#define PLAT_ATTR_GET_UINT64_1D_ARRAY(ID, PTARGET, VAL) \
     _getAttributeArrayDoubleWord(fapi::ID, PTARGET, VAL)
/* global set uint64_t 1D array macro */
#define PLAT_ATTR_SET_UINT64_1D_ARRAY(ID, PTARGET, VAL) \
     _setAttributeArrayDoubleWord(fapi::ID, PTARGET, VAL)

/* global get uint64_t 2D array macro */
#define PLAT_ATTR_GET_UINT64_2D_ARRAY(ID, PTARGET, VAL) \
     _getAttributeArrayDoubleWord(fapi::ID, PTARGET, VAL[0])
/* global set uint64_t 2D array macro */
#define PLAT_ATTR_SET_UINT64_2D_ARRAY(ID, PTARGET, VAL) \
     _setAttributeArrayDoubleWord(fapi::ID, PTARGET, VAL[0])

/* global get uint64_t 3D array macro */
#define PLAT_ATTR_GET_UINT64_3D_ARRAY(ID, PTARGET, VAL) \
     _getAttributeArrayDoubleWord(fapi::ID, PTARGET, VAL[0][0])
/* global set uint64_t 3D array macro */
#define PLAT_ATTR_SET_UINT64_3D_ARRAY(ID, PTARGET, VAL) \
     _setAttributeArrayDoubleWord(fapi::ID, PTARGET, VAL[0][0])

/* global get uint64_t 4D array macro */
#define PLAT_ATTR_GET_UINT64_4D_ARRAY(ID, PTARGET, VAL) \
     _getAttributeArrayDoubleWord(fapi::ID, PTARGET, VAL[0][0][0])
/* global set uint64_t 4D array macro */
#define PLAT_ATTR_SET_UINT64_4D_ARRAY(ID, PTARGET, VAL) \
     _setAttributeArrayDoubleWord(fapi::ID, PTARGET, VAL[0][0][0])

/* global get int macro (uint8_t, 32 and 64) */
#define PLAT_ATTR_GET_GLOBAL_INT(ID, PTARGET, VAL) \
     _get<fapi::ID##_Type>(fapi::ID, PTARGET, VAL)


/* global get int macro (uint8_t, 32 and 64) */
#define PLAT_ATTR_GET_GLOBAL_INT(ID, PTARGET, VAL) \
     _get<fapi::ID##_Type>(fapi::ID, PTARGET, VAL)

/* global set int macro (uint8_t, 32 and 64) */
#define PLAT_ATTR_SET_GLOBAL_INT(ID, PTARGET, VAL) \
     _set<fapi::ID##_Type>(fapi::ID, PTARGET, VAL)

/******************************************************************************/
// Get string 
/******************************************************************************/

namespace fapi {

template<typename T>
ReturnCode _get(const AttributeId i_id,
                const Target * const i_pTarget,
                char * & o_pValue)
{
      printf("Unsupported type specified\n");
      return 0xFFFFFFFF;
}
template<> // Specialized template function defined in fapiAttributeService.C
ReturnCode _get<char *> (const AttributeId i_id,
                         const Target * const i_pTarget,
                         char * & o_value);

//******************************************************************************
// Get uint8_t 
//******************************************************************************
template<typename T>
ReturnCode _get(const AttributeId i_id,
                const Target * const i_pTarget,
                uint8_t& o_value)
{
      printf("Unsupported type specified\n");
      return 0xFFFFFFFF;
}

template<> // Specialized template function defined in fapiAttributeService.C
ReturnCode _get<uint8_t> (const AttributeId i_id,
                           const Target * const i_pTarget,
                           uint8_t & o_value);

//******************************************************************************
// Get uint32_t 
//******************************************************************************
template<typename T>
ReturnCode _get(const AttributeId i_id,
                const Target * const i_pTarget,
                uint32_t& o_value)
{
      printf("Unsupported type specified\n");
      return 0xFFFFFFFF;
}

template<> // Specialized template function defined in fapiAttributeService.C
ReturnCode _get<uint32_t> (const AttributeId i_id,
                           const Target * const i_pTarget,
                           uint32_t & o_value);


//******************************************************************************
// Get uint64_t 
//******************************************************************************
template<typename T>
ReturnCode _get(const AttributeId i_id,
                const Target * const i_pTarget,
                uint64_t& o_value)
{
      printf("Unsupported type specified\n");
      return 0xFFFFFFFF;
}
template<> // Specialized template function defined in fapiAttributeService.C
ReturnCode _get<uint64_t> (const AttributeId i_id,
                           const Target * const i_pTarget,
                           uint64_t & o_value);

//******************************************************************************
// Get and set arrays  
//******************************************************************************
ReturnCode _getAttributeArrayShort(const AttributeId i_id, const Target * const i_pTarget, uint8_t * o_pValues);
ReturnCode _setAttributeArrayShort(const AttributeId i_id, const Target * const i_pTarget, uint8_t * i_pValues);
ReturnCode _getAttributeArrayWord(const AttributeId i_id, const Target * const i_pTarget, uint32_t * o_pValues);
ReturnCode _setAttributeArrayWord(const AttributeId i_id, const Target * const i_pTarget, uint32_t * i_pValues);
ReturnCode _getAttributeArrayDoubleWord(const AttributeId i_id, const Target * const i_pTarget, uint64_t * o_pValues);
ReturnCode _setAttributeArrayDoubleWord(const AttributeId i_id, const Target * const i_pTarget, uint64_t * i_pValues);

//******************************************************************************
// Set String 
//******************************************************************************
template<typename T>
ReturnCode _set(const AttributeId i_id,
                const Target * const i_pTarget,
                char * & i_pValue)
{
      printf("Unsupported type specified\n");
      return 0xFFFFFFFF;
}
template<> // Specialized template function defined in fapiAttributeService.C
ReturnCode _set<char *> (const AttributeId i_id,
                         const Target * const i_pTarget,
                         char * & i_value);

//******************************************************************************
// Set uint8_t 
//******************************************************************************
template<typename T>
ReturnCode _set(const AttributeId i_id,
                const Target * const i_pTarget,
                uint8_t& i_value)
{
    //InvalidTypeRequestedForAttribute();
    return FAPI_RC_SUCCESS;
}

template<> // Specialized template function defined in fapiAttributeService.C
ReturnCode _set<uint8_t> (const AttributeId i_id,
                           const Target * const i_pTarget,
                           uint8_t & i_value);

//******************************************************************************
// Set uint32_t 
//******************************************************************************
template<typename T>
ReturnCode _set(const AttributeId i_id,
                const Target * const i_pTarget,
                uint32_t& i_value)
{
    //InvalidTypeRequestedForAttribute();
    return FAPI_RC_SUCCESS;
}

template<> // Specialized template function defined in fapiAttributeService.C
ReturnCode _set<uint32_t> (const AttributeId i_id,
                           const Target * const i_pTarget,
                           uint32_t & i_value);

//******************************************************************************
// Set uint64_t 
//******************************************************************************
template<typename T>
ReturnCode _set(const AttributeId i_id,
                const Target * const i_pTarget,
                const uint64_t & i_value)
{
    //InvalidTypeRequestedForAttribute();
    return FAPI_RC_SUCCESS;
}

template<> // Specialized template function defined in fapiAttributeService.C
ReturnCode _set<uint64_t> (const AttributeId i_id,
                           const Target * const i_pTarget,
                           const uint64_t & i_value);

} //namespace
#endif // FAPIPLATATTRIBUTESERVICE_H_hip
