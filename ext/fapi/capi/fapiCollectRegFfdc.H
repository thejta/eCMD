/**
 *  @file fapiCollectRegFfdc.H
 *
 *  @brief Defines the fapiCollectRegFfdc function that collects chip register
 *         FFDC data. This is called automatically by FAPI_SET_HWP_ERROR (when
 *         a HWP creates an error) and FAPI_ADD_INFO_TO_HWP_ERROR (when an FFDC
 *         HWP adds error information to an existing error) if the error XML
 *         contains a <collectRegisterFfdc> element. This function should not
 *         be called directly by any user code. The function implementation is
 *         automatically generated from FAPI Error XML files.
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 *                          mjjones     09/19/2012  Created.
 */

#ifndef FAPICOLLECTREGFFDC_H_
#define FAPICOLLECTREGFFDC_H_

#include <fapiHwpErrorInfo.H>

namespace fapi
{

class Target;
class ReturnCode;

/**
 * @brief Collects Register FFDC from a chip
 *
 * This should only be called by FAPI during FAPI_SET_HWP_ERROR or
 * FAPI_ADD_INFO_TO_HWP_ERROR
 *
 * @param[in]  i_target Reference to Target to collect FFDC from
 * @param[in]  i_ffdcId FFDC Identifier
 * @param[out] o_rc     Reference to ReturnCode that FFDC is added to
 */
void fapiCollectRegFfdc(const fapi::Target & i_target,
                        const fapi::HwpFfdcId i_ffdcId,
                        fapi::ReturnCode & o_rc);
}

#endif
