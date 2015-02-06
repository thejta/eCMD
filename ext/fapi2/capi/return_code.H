/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source$                                                             */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2012,2014                        */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
/**
 * @file return_code.H
 * @brief definitions for fapi2 return codes
 */

#ifndef __FAPI2_RETURN_CODE__
#define __FAPI2_RETURN_CODE__

#include <stdint.h>

namespace fapi2
{

///
/// @brief Enumeration of return codes
///
    enum ReturnCodes
    {
        ///< Success
        FAPI2_RC_SUCCESS = 0,

        // Flag bits indicating which code generated the error.
        FAPI2_RC_FAPI2_MASK = 0x04000000, ///< FAPI2 mask
        FAPI2_RC_PLAT_MASK =  0x02000000, ///< Platform mask
        FAPI2_RC_HWP_MASK =   0x00000000, ///< HWP mask

        //
        // FAPI generated return codes
        //

        FAPI2_RC_INVALID_ATTR_GET            = FAPI2_RC_FAPI2_MASK | 0x01,
        ///< Initfile requested an attribute with an invalid attribute ID

        FAPI2_RC_INVALID_CHIP_EC_FEATURE_GET = FAPI2_RC_FAPI2_MASK | 0x02,
        ///< HWP requested a chip EC feature with an invalid attribute ID

        FAPI2_RC_INVALID_MULTISCOM_LENGTH    = FAPI2_RC_FAPI2_MASK | 0x03,
        ///< Invalid multiscom parameters

        FAPI2_RC_INVALID_PARAMETER           = FAPI2_RC_FAPI2_MASK | 0x04,
        ///< Invalid parameters to a FAPI2 function

        FAPI2_RC_OVERFLOW                    = FAPI2_RC_FAPI2_MASK | 0x05,
        ///< Overflow condition, typically a buffer operation

        //
        // PLAT generated return codes. Additional details may be contained in
        // ReturnCode platData (this can only be looked at by PLAT code)
        //

        FAPI2_RC_PLAT_ERR_SEE_DATA   = FAPI2_RC_PLAT_MASK | 0x01,
        ///< Generic platform error

        FAPI2_RC_PLAT_ERR_ADU_LOCKED = FAPI2_RC_PLAT_MASK | 0x02,
        ///< Operation to AlterDisplay unit failed because it is locked

        FAPI2_RC_PLAT_NOT_SUPPORTED_AT_RUNTIME = FAPI2_RC_PLAT_MASK | 0x03,
        ///< Operation not supported by HB runtime
    };

    class FirstFailureData
    {};

    ///
    /// @brief Class representing a FAPI2 ReturnCode
    ///
    class ReturnCode : public FirstFailureData
    {
    public:

        ///
        /// @brief Constructor.
        /// @param[in] The rc to set
        ///
        ReturnCode(const ReturnCodes i_rc = fapi2::FAPI2_RC_SUCCESS):
            iv_rc(i_rc)
            {};

        ///
        /// @brief integral type conversion function. Returns the error code
        /// @return The error code
        ///
        operator ReturnCodes() const { return iv_rc; }

    private:
        ReturnCodes iv_rc;
    };
};

#endif
