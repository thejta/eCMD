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

/// @file plat_hw_access.H
///
/// @brief Platform hardware-access definitions
///

#ifndef _FAPI2_PLAT_HWACCESS_H_
#define _FAPI2_PLAT_HWACCESS_H_

#include <hw_access_def.H>
#include <stdint.h>
#include <thread>
#include <buffer.H>
#include <variable_buffer.H>
#include <return_code.H>
#include <target.H>

#ifdef FAPI_SUPPORT_MULTI_SCOM
#include <fapiMultiScom.H>
#endif

namespace fapi2plat
{
    const fapi2::TargetType TARGET_TYPE_SCOM_TARGET = fapi2::TARGET_TYPE_PROC_CHIP |
                                                      fapi2::TARGET_TYPE_MEMBUF_CHIP |
                                                      fapi2::TARGET_TYPE_EX |
                                                      fapi2::TARGET_TYPE_MBA |
                                                      fapi2::TARGET_TYPE_MCS |
                                                      fapi2::TARGET_TYPE_XBUS |
                                                      fapi2::TARGET_TYPE_ABUS |
                                                      fapi2::TARGET_TYPE_L4 |
                                                      fapi2::TARGET_TYPE_CORE |
                                                      fapi2::TARGET_TYPE_EQ |
                                                      fapi2::TARGET_TYPE_MCA |
                                                      fapi2::TARGET_TYPE_MCBIST |
                                                      fapi2::TARGET_TYPE_MI |
                                                      fapi2::TARGET_TYPE_DMI |
                                                      fapi2::TARGET_TYPE_OBUS |
                                                      fapi2::TARGET_TYPE_NV |
                                                      fapi2::TARGET_TYPE_SBE |
                                                      fapi2::TARGET_TYPE_PPE |
                                                      fapi2::TARGET_TYPE_PERV |
                                                      fapi2::TARGET_TYPE_PEC |
                                                      fapi2::TARGET_TYPE_PHB;

    const fapi2::TargetType TARGET_TYPE_CFAM_TARGET = fapi2::TARGET_TYPE_PROC_CHIP |
                                                      fapi2::TARGET_TYPE_MEMBUF_CHIP;

    const fapi2::TargetType TARGET_TYPE_SCAN_TARGET = fapi2::TARGET_TYPE_PROC_CHIP |
                                                      fapi2::TARGET_TYPE_MEMBUF_CHIP |
                                                      fapi2::TARGET_TYPE_CORE |
                                                      fapi2::TARGET_TYPE_EQ;
    //--------------------------------------------------------------------------
    // HW Communication Functions
    //--------------------------------------------------------------------------

    /// @brief Platform-level implementation called by GetScom()
    ///  Hardware procedures writers will not call this function.
    /// @brief Reads a SCOM register from a chip.
    /// @param[in]  i_target     HW target to operate on.
    /// @param[in]  i_address    SCOM register address to read from.
    /// @param[out] o_date       Buffer that holds data read from HW target.
    /// @return fapi::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    fapi2::ReturnCode getScom(const fapi2::Target<fapi2plat::TARGET_TYPE_SCOM_TARGET>& i_target,
                              const uint64_t i_address,
                              fapi2::buffer<uint64_t>& o_data);

    fapi2::ReturnCode putScom(const fapi2::Target<fapi2plat::TARGET_TYPE_SCOM_TARGET>& i_target,
                              const uint64_t i_address,
                              const fapi2::buffer<uint64_t> i_data);

    fapi2::ReturnCode putScomUnderMask(const fapi2::Target<fapi2plat::TARGET_TYPE_SCOM_TARGET>& i_target,
                                       const uint64_t i_address,
                                       const fapi2::buffer<uint64_t> i_data,
                                       const fapi2::buffer<uint64_t> i_mask);

    fapi2::ReturnCode getCfamRegister(const fapi2::Target<fapi2plat::TARGET_TYPE_CFAM_TARGET>& i_target,
                                      const uint32_t i_address,
                                      fapi2::buffer<uint32_t>& o_data);

    fapi2::ReturnCode putCfamRegister(const fapi2::Target<fapi2plat::TARGET_TYPE_CFAM_TARGET>& i_target,
                                      const uint32_t i_address,
                                      const fapi2::buffer<uint32_t> i_data);

    fapi2::ReturnCode getRing(const fapi2::Target<fapi2plat::TARGET_TYPE_SCAN_TARGET>& i_target,
                              const scanRingId_t i_address,
                              fapi2::variable_buffer& o_data,
                              const fapi2::RingMode i_ringMode);

    fapi2::ReturnCode putRing(const fapi2::Target<fapi2plat::TARGET_TYPE_SCAN_TARGET>& i_target,
                              const scanRingId_t i_address,
                              const fapi2::variable_buffer& i_data,
                              const fapi2::RingMode i_ringMode);

};

#endif // _FAPI2_PLAT_HWACCESS_H_
