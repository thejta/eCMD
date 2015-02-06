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
 *  @file hw_access.H
 *
 *  @brief Defines the hardware access functions that platform code must
 *         implement.
 */

#ifndef FAPI2_PLATHWACCESS_H_
#define FAPI2_PLATHWACCESS_H_

#ifdef FAPI_SUPPORT_SPY_AS_ENUM
#include <fapiSpyIds.H>
#endif

#include <hw_access_structs.H>
#include <stdint.h>
#include <thread>
#include <buffer.H>
#include <variable_buffer.H>
#include <return_code.H>
#include <target.H>

#ifdef FAPI_SUPPORT_MULTI_SCOM
#include <fapiMultiScom.H>
#endif

namespace fapi2
{

    //--------------------------------------------------------------------------
    // HW Communication Functions
    //--------------------------------------------------------------------------

    /// @brief Platform-level implementation called by GetScom()
    ///  Hardware procedures writers will not call this function.
    /// @brief Reads a SCOM register from a chip.
    /// @tparam K template parameter, passed in target.
    /// @param[in]  i_target     HW target to operate on.
    /// @param[in]  i_address    SCOM register address to read from.
    /// @param[out] o_date       Buffer that holds data read from HW target.
    /// @return fapi::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    template< TargetType K >
    ReturnCode platGetScom(const Target<K>& i_target, const uint64_t i_address,
                           buffer<uint64_t>& o_data)
    {
        static_assert(true, "platGetScom called with incorrect TargetType");
        return (ReturnCodes) 1;
    }

#define PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE) \
    template<> \
    ReturnCode platGetScom(const Target<TARGET_TYPE> & i_target, \
                           const uint64_t i_address, \
                           buffer<uint64_t>& o_data);

    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_PROC_CHIP)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MEMBUF_CHIP)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_EX)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MBA)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MCS)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_XBUS)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_ABUS)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_L4)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_CORE)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_EQ)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MCA)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MCBIST)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MIA)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MIS)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_DMI)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_OBUS)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_NV)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_SBE)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_PPE)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_PERV)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_PEC)
    PLAT_GET_SCOM_TEMPLATE_MACRO(TARGET_TYPE_PHB)
#undef PLAT_GET_SCOM_TEMPLATE_MACRO

    template< TargetType K >
    ReturnCode platPutScom(const Target<K>& i_target, const uint64_t i_address,
                           buffer<uint64_t>& i_data)
    {
        static_assert(true, "platPutScom called with incorrect TargetType");
        return (ReturnCodes) 1;
    }

#define PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE) \
    template<> \
    ReturnCode platPutScom(const Target<TARGET_TYPE> & i_target, \
                           const uint64_t i_address, \
                           buffer<uint64_t>& i_data);

    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_PROC_CHIP)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MEMBUF_CHIP)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_EX)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MBA)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MCS)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_XBUS)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_ABUS)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_L4)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_CORE)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_EQ)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MCA)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MCBIST)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MIA)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_MIS)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_DMI)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_OBUS)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_NV)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_SBE)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_PPE)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_PERV)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_PEC)
    PLAT_PUT_SCOM_TEMPLATE_MACRO(TARGET_TYPE_PHB)
#undef PLAT_PUT_SCOM_TEMPLATE_MACRO

    template< TargetType K >
    ReturnCode platPutScomUnderMask(const Target<K>& i_target, const uint64_t i_address,
                                    buffer<uint64_t>& i_data, buffer<uint64_t>& i_mask)
    {
        static_assert(true, "platPutScomUnderMask called with incorrect TargetType");
        return (ReturnCodes) 1;
    }

#define PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE) \
    template<> \
    ReturnCode platPutScomUnderMask(const Target<TARGET_TYPE> & i_target, \
                                    const uint64_t i_address, \
                                    buffer<uint64_t>& i_data, \
                                    buffer<uint64_t>& i_mask);

    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_PROC_CHIP)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_MEMBUF_CHIP)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_EX)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_MBA)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_MCS)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_XBUS)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_ABUS)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_L4)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_CORE)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_EQ)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_MCA)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_MCBIST)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_MIA)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_MIS)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_DMI)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_OBUS)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_NV)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_SBE)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_PPE)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_PERV)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_PEC)
    PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO(TARGET_TYPE_PHB)
#undef PLAT_PUT_SCOM_UNDER_MASK_TEMPLATE_MACRO

    template< TargetType K >
    ReturnCode platGetCfamRegister(const Target<K>& i_target,
                                 const uint32_t i_address,
                                 buffer<uint32_t>& o_data)
    {
        static_assert(true, "platGetCfamRegister called with incorrect TargetType");
        return (ReturnCodes) 1;
    }

    template<>
    ReturnCode platGetCfamRegister(const Target<TARGET_TYPE_PROC_CHIP>& i_target,
                                   const uint32_t i_address,
                                   buffer<uint32_t>& o_data);

    template<>
    ReturnCode platGetCfamRegister(const Target<TARGET_TYPE_MEMBUF_CHIP>& i_target,
                                   const uint32_t i_address,
                                   buffer<uint32_t>& o_data);

    template< TargetType K >
    ReturnCode platPutCfamRegister(const Target<K>& i_target,
                                   const uint32_t i_address,
                                   buffer<uint32_t>& i_data)
    {
        static_assert(true, "platPutCfamRegister called with incorrect TargetType");
        return (ReturnCodes) 1;
    }

    template<>
    ReturnCode platPutCfamRegister(const Target<TARGET_TYPE_PROC_CHIP>& i_target,
                                  const uint32_t i_address,
                                  buffer<uint32_t>& i_data);

    template<>
    ReturnCode platPutCfamRegister(const Target<TARGET_TYPE_MEMBUF_CHIP>& i_target,
                                  const uint32_t i_address,
                                  buffer<uint32_t>& i_data);

    template< TargetType K >
    ReturnCode platGetRing(const Target<K>& i_target,
                           const scanRingId_t i_address,
                           variable_buffer& o_data,
                           const RingMode i_ringMode)
    {
        static_assert(true, "platGetRing called with incorrect TargetType");
        return (ReturnCodes) 1;
    }

    template<>
    ReturnCode platGetRing(const Target<TARGET_TYPE_PROC_CHIP>& i_target,
                           const scanRingId_t i_address,
                           variable_buffer& o_data,
                           const RingMode i_ringMode);

    template<>
    ReturnCode platGetRing(const Target<TARGET_TYPE_MEMBUF_CHIP>& i_target,
                           const scanRingId_t i_address,
                           variable_buffer& o_data,
                           const RingMode i_ringMode);

    template<>
    ReturnCode platGetRing(const Target<TARGET_TYPE_CORE>& i_target,
                           const scanRingId_t i_address,
                           variable_buffer& o_data,
                           const RingMode i_ringMode);

    template<>
    ReturnCode platGetRing(const Target<TARGET_TYPE_EQ>& i_target,
                           const scanRingId_t i_address,
                           variable_buffer& o_data,
                           const RingMode i_ringMode);

    template< TargetType K >
    ReturnCode platPutRing(const Target<K>& i_target,
                           const scanRingId_t i_address,
                           variable_buffer& i_data,
                           const RingMode i_ringMode)
    {
        static_assert(true, "platPutRing called with incorrect TargetType");
        return (ReturnCodes) 1;
    }

    template<>
    ReturnCode platPutRing(const Target<TARGET_TYPE_PROC_CHIP>& i_target,
                           const scanRingId_t i_address,
                           variable_buffer& i_data,
                           const RingMode i_ringMode);

    template<>
    ReturnCode platPutRing(const Target<TARGET_TYPE_MEMBUF_CHIP>& i_target,
                           const scanRingId_t i_address,
                           variable_buffer& i_data,
                           const RingMode i_ringMode);

    template<>
    ReturnCode platPutRing(const Target<TARGET_TYPE_CORE>& i_target,
                           const scanRingId_t i_address,
                           variable_buffer& i_data,
                           const RingMode i_ringMode);

    template<>
    ReturnCode platPutRing(const Target<TARGET_TYPE_EQ>& i_target,
                           const scanRingId_t i_address,
                           variable_buffer& i_data,
                           const RingMode i_ringMode);
};

#endif // FAPI2_PLATHWACCESS_H_
