// $Id$
// $Source$

/**
 *  @file fapi2Macro.H
 *
 *  @brief Defines FAPI Macros.
 *
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 */
#ifndef FAPI2MACRO_H_
#define FAPI2MACRO_H_

#include <fapi2PlatMacro.H>

/**
 * @brief FAPI try macro
 *
 */

#define PIB_ERROR_MASK( _value) \
    PIB_PLAT_ERROR_MASK( _value )

#define FAPI_ERROR_MASK(_value) \
    FAPI_PLAT_ERROR_MASK( _value )

#define FAPI_SET_HWP_ERROR(RC, ERROR) \
    RC = (fapi2::ReturnCodes) fapi2::ERROR; \
    ERROR##_CALL_FUNCS_TO_COLLECT_FFDC(RC); \
    ERROR##_CALL_FUNCS_TO_COLLECT_REG_FFDC(RC); \
    ERROR##_ADD_ERROR_INFO(RC)


#endif // FAPI2MACRO_H_
