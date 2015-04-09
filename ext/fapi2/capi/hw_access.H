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

#ifndef FAPI2_HWACCESS_H_
#define FAPI2_HWACCESS_H_

#ifdef FAPI_SUPPORT_SPY_AS_ENUM
#include <fapiSpyIds.H>
#endif

#include <stdint.h>
#include <thread>
#include <hw_access_structs.H>
#include <buffer.H>
#include <variable_buffer.H>
#include <return_code.H>
#include <target.H>
#include <plat_hw_access.H>

#include <utils.H>
#include <plat_utils.H>
#include <plat_trace.H>

#ifdef FAPI_SUPPORT_MULTI_SCOM
#include <fapiMultiScom.H>
#endif

namespace fapi2
{

    //--------------------------------------------------------------------------
    // PIB Error Functions
    //--------------------------------------------------------------------------

    /// @brief Sets the PIB error mask - platform dependant
    /// @param[in]  i_mask     The new error mask
    inline void setPIBErrorMask(uint8_t i_mask)
    {
        // Keeps the compiler from complaining about the unused i_mask
        static_cast<void>(i_mask);

        // Platform specific funtion needs to be defined here.
        // For Cronus/FSP/Hostboot, this is a no-op.
        return;
    }

    /// @brief Gets the PIB error mask - platform dependant
    /// @return uint8_t The current PIB error mask
    inline uint8_t getPIBErrorMask(void)
    {
        // Platform specific funtion needs to be defined here.
        // For Cronus/FSP/Hostboot, this always returns 0
        return 0;
    }

    //--------------------------------------------------------------------------
    // Operational Mode Error Functions
    //--------------------------------------------------------------------------

    /// @enum OpModes operational Mode Error Functions
    enum OpModes
    {
        // These are bit-masks in case they need to be or'd together
        NORMAL           = 0x00,
        IGNORE_HW_ERROR  = 0x01,
        DO_NOT_DO_WAKEUP = 0x02,
    };

    /// @brief Sets the operational mode
    /// @param[in]  i_mode     The new mode
    inline void setOpMode(const OpModes i_mode)
    {
        // Keeps the compiler from complaining about the unused i_mode
        static_cast<void>(i_mode);

        // No-op for now. Should set thread-local operational mode
        return;
    }

    /// @brief Gets the operational mode
    /// @return the operational mode
    inline OpModes getOpMode(void)
    {
        // No-op for now. Should read thread-local operational mode
        return NORMAL;
    }


    //--------------------------------------------------------------------------
    // HW Communication Functions
    //--------------------------------------------------------------------------

    /// @brief Reads a SCOM register from a chip.
    /// @tparam K template parameter, passed in target.
    /// @param[in]  i_target     HW target to operate on.
    /// @param[in]  i_address    SCOM register address to read from.
    /// @param[out] o_data       Buffer that holds data read from HW target.
    /// @return fapi2::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    template< TargetType K >
    inline ReturnCode getScom(const Target<K>& i_target, const uint64_t i_address,
                              buffer<uint64_t>& o_data)
    {
        ReturnCode l_rc;
        bool l_traceit = fapi2plat::isScanTraceEnabled();

        // call the platform implementation
        l_rc = platGetScom( i_target, i_address, o_data );

        if (l_rc != FAPI2_RC_SUCCESS)
        {
            std::string target_string;
            fapi2::toString(i_target, target_string);
            FAPI_ERR("getScom failed - Target %s, Addr " UINT64_HEX16_PRESC_FORMAT,
                    target_string.c_str(), i_address);
        }

        if( l_traceit )
        {
            std::string target_string;
            fapi2::toString(i_target, target_string);
            FAPI_SCAN( "TRACE : GETSCOM     : %s : " UINT64_HEX16_PRESC_FORMAT " " UINT64_HEX16_PRESC_FORMAT ,
                        target_string.c_str(),
                        i_address,
                        static_cast<uint64_t>(o_data) );
        }

        return l_rc;
    }

    /// @brief Writes a SCOM register on a chip.
    /// @tparam K template parameter, passed in target.
    /// @param[in] i_target     HW target to operate on.
    /// @param[in] i_address    SCOM register address to write to.
    /// @param[in] i_data       Buffer that holds data to write into address.
    /// @return fapi2::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    template< TargetType K >
    inline ReturnCode putScom(const Target<K>& i_target, const uint64_t i_address,
                              buffer<uint64_t> i_data)
    {
        ReturnCode l_rc;
        bool l_traceit = fapi2plat::isScanTraceEnabled();

        // call the platform implementation
        l_rc = platPutScom( i_target, i_address, i_data );

        if (l_rc != FAPI2_RC_SUCCESS)
        {
            std::string target_string;
            fapi2::toString(i_target, target_string);
            FAPI_ERR("putScom failed - Target %s, Addr " UINT64_HEX16_PRESC_FORMAT,
                    target_string.c_str(), i_address);
        }

        if( l_traceit )
        {
            std::string target_string;
            fapi2::toString(i_target, target_string);
            FAPI_SCAN( "TRACE : PUTSCOM     : %s : " UINT64_HEX16_PRESC_FORMAT " " UINT64_HEX16_PRESC_FORMAT ,
                        target_string.c_str(),
                        i_address,
                        static_cast<uint64_t>(i_data) );
        }

        return l_rc;
    }

    /// @brief Read-modify-write a SCOM register on a chip
    /// @tparam K template parameter, passed in target.
    /// @param[in] i_target     HW target to operate on.
    /// @param[in] i_address    SCOM register address to write to.
    /// @param[in] i_data       Buffer that holds data to be modified.
    /// @param[in] i_modifyMode The modify mode (or/and/xor).
    /// @return fapi2::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    template< TargetType K >
    inline ReturnCode modifyScom(const Target<K>& i_target,
                                 const uint64_t i_address,
                                 const buffer<uint64_t>& i_data,
                                 const ChipOpModifyMode i_modifyMode);

    /// @brief Writes a SCOM register under mask on a chip
    /// @tparam K template parameter, passed in target.
    /// @param[in] i_target     HW target to operate on.
    /// @param[in] i_address    SCOM register address to write to.
    /// @param[in] i_data       Buffer that holds data to write into address.
    /// @param[in] i_mask       Buffer that holds the mask value.
    /// @return fapi2::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    template< TargetType K >
    inline ReturnCode putScomUnderMask(const Target<K>& i_target,
                                       const uint64_t i_address,
                                       buffer<uint64_t>& i_data,
                                       buffer<uint64_t>& i_mask)
    {
        ReturnCode l_rc;
        bool l_traceit = fapi2plat::isScanTraceEnabled();

        // call the platform implementation
        l_rc = platPutScomUnderMask( i_target, i_address, i_data, i_mask );

        if (l_rc != FAPI2_RC_SUCCESS)
        {
            std::string target_string;
            fapi2::toString(i_target, target_string);
            FAPI_ERR("putScomUnderMask failed - Target %s, Addr " UINT64_HEX16_PRESC_FORMAT,
                    target_string.c_str(), i_address);
        }

        if( l_traceit )
        {
            std::string target_string;
            fapi2::toString(i_target, target_string);
            FAPI_SCAN( "TRACE : PUTSCOMMASK : %s : " UINT64_HEX16_PRESC_FORMAT " " UINT64_HEX16_PRESC_FORMAT " " UINT64_HEX16_PRESC_FORMAT,
                        target_string.c_str(),
                        i_address,
                        static_cast<uint64_t>(i_data),
                        static_cast<uint64_t>(i_mask) );
        }

        return l_rc;
    }

    /// @brief Reads a CFAM register from a chip.
    ///        CFAM register is 32-bit wide.
    /// @tparam K template parameter, passed in target.
    /// @param[in]  i_target  HW target to operate on.
    /// @param[in]  i_address CFAM register address to read from.
    /// @param[out] o_data    Buffer that holds data read from HW target.
    /// @return fapi2::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    template< TargetType K >
    inline ReturnCode getCfamRegister(const Target<K>& i_target,
                                      const uint32_t i_address,
                                      buffer<uint32_t>& o_data)
    {
        ReturnCode l_rc;
        bool l_traceit = fapi2plat::isScanTraceEnabled();

        // call the platform implementation
        l_rc = platGetCfamRegister( i_target, i_address, o_data );

        if (l_rc != FAPI2_RC_SUCCESS)
        {
            std::string target_string;
            fapi2::toString(i_target, target_string);
            FAPI_ERR("getCfamRegister failed - Target %s, Addr %.8X",
                    target_string.c_str(), i_address);
        }

        if( l_traceit )
        {
            std::string target_string;
            fapi2::toString(i_target, target_string);
            FAPI_SCAN( "TRACE : GETCFAMREG  : %s : %.8X %.8X",
                target_string.c_str(),
                i_address,
                static_cast<uint32_t>(o_data) );
        }

        return l_rc;
    }

    /// @brief Writes a CFAM register on a chip.
    ///        CFAM register is 32-bit wide.
    /// @tparam K template parameter, passed in target.
    /// @param[in] i_target  HW target to operate on.
    /// @param[in] i_address CFAM register address to write to.
    /// @param[in] i_data    Buffer that holds data to write into address.
    /// @return fapi2::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    template< TargetType K >
    inline ReturnCode putCfamRegister(const Target<K>& i_target,
                                      const uint32_t i_address,
                                      buffer<uint32_t>& i_data)
    {
        ReturnCode l_rc;
        bool l_traceit = fapi2plat::isScanTraceEnabled();

        // call the platform implementation
        l_rc = platPutCfamRegister( i_target, i_address, i_data );

        if (l_rc != FAPI2_RC_SUCCESS)
        {
            std::string target_string;
            fapi2::toString(i_target, target_string);
            FAPI_ERR("platPutCfamRegister failed - Target %s, Addr %.8X",
                  target_string.c_str(), i_address);
        }

        if( l_traceit )
        {
            std::string target_string;
            fapi2::toString(i_target, target_string);
            FAPI_SCAN( "TRACE : PUTCFAMREG  : %s : %.8X %.8X",
                target_string.c_str(),
                i_address,
                static_cast<uint32_t>(i_data) );
        }

        return l_rc;
    }

    /// @brief Read-modify-write a CFAM register on a chip.
    ///        CFAM register is 32-bit wide.
    /// @tparam K template parameter, passed in target.
    /// @param[in] i_target     HW target to operate on.
    /// @param[in] i_address    CFAM register address to modify.
    /// @param[in] i_data       Buffer that holds data to be modified.
    /// @param[in] i_modifyMode The modify mode (or/and/xor).
    /// @return fapi2::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    template< TargetType K >
    inline ReturnCode modifyCfamRegister(const Target<K>& i_target,
                                         const uint32_t i_address,
                                         const buffer<uint32_t>& i_data,
                                         const ChipOpModifyMode i_modifyMode);

    /// @brief Reads a ring from a chip.
    /// @tparam K template parameter, passed in target.
    /// @param[in]  i_target   Target to operate on.
    /// @param[in]  i_address  Ring address to read from.
    /// @param[out] o_data     Buffer that holds data read from HW target.
    /// @param[in]  i_ringMode Ring operation mode.
    /// @return fapi2::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    template< TargetType K >
    inline ReturnCode getRing(const Target<K>& i_target,
                              const scanRingId_t i_address,
                              variable_buffer& o_data,
                              const RingMode i_ringMode = 0)
    {
        fapi2::ReturnCode l_rc;
        bool l_traceit = fapi2plat::isScanTraceEnabled();

        // call the platform implementation
        l_rc = platGetRing( i_target, i_address, o_data, i_ringMode );

        if (l_rc != FAPI2_RC_SUCCESS)
        {
            std::string target_string;
            fapi2::toString(i_target, target_string);
            FAPI_ERR("getRing failed - Target %s, Addr 0x%.8X", 
                     target_string.c_str(), i_address);
        }

        if( l_traceit )
        {
            std::string target_string;
            fapi2::toString(i_target, target_string);
            uint64_t l_data = 0;
            o_data.extract(l_data, 0, 64);
            FAPI_SCAN( "TRACE : GETRING     : %s : %.8X " UINT64_HEX16_PRESC_FORMAT, 
                       target_string.c_str(),
                       i_address,
                       l_data );
        }

        return l_rc;
    }

    /// @brief Writes a ring to a chip.
    /// @tparam K template parameter, passed in target.
    /// @param[in] i_target   Target to operate on.
    /// @param[in] i_address  Ring address to write to.
    /// @param[in] i_data     Buffer that contains RS4 compressed ring data
    ///                       to write into address
    /// @param[in] i_ringMode Ring operation mode.
    /// @return fapi2::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    template< TargetType K >
    inline ReturnCode putRing(const Target<K>& i_target,
                              const scanRingId_t i_address,
                              variable_buffer& i_data,
                              const RingMode i_ringMode = 0)
    {
        fapi2::ReturnCode l_rc;
        bool l_traceit = fapi2plat::isScanTraceEnabled();

        // call the platform implementation
        l_rc = platPutRing( i_target, i_address, i_data, i_ringMode );

        if (l_rc != FAPI2_RC_SUCCESS)
        {
            std::string target_string;
            fapi2::toString(i_target, target_string);
            FAPI_ERR("putRing failed - Target %s, Addr 0x%.8X", 
                     target_string.c_str(), i_address);
        }

        if( l_traceit )
        {
            std::string target_string;
            fapi2::toString(i_target, target_string);
            uint64_t l_data = 0;
            i_data.extract(l_data, 0, 64);
            FAPI_SCAN( "TRACE : PUTRING     : %s : %.8X " UINT64_HEX16_PRESC_FORMAT, 
                       target_string.c_str(),
                       i_address,
                       l_data );
        }

        return l_rc;
    }

    /// @brief Writes a ring to a chip.
    /// @tparam K template parameter, passed in target.
    /// @param[in] i_target   Target to operate on.
    /// @param[in] i_address  Ring address to write to.
    /// @param[in] i_data     Pointer to location that contains RS4 compressed
    //  ring data to write into address
    /// @param[in] i_ringMode Ring operation mode.
    /// @return fapi2::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    template< TargetType K >
    inline ReturnCode putRing(const Target<K>& i_target,
                              const scanRingId_t i_address,
                              const void* i_data,
                              const RingMode i_ringMode = 0);

    /// @brief Read-modify-write a ring on a chip.
    /// @tparam K template parameter, passed in target.
    /// @param[in] i_target     Target to operate on.
    /// @param[in] i_address    Ring address to modify.
    /// @param[in] i_data       Buffer that contains RS4 compressed ring data
    ///                         to be modified.
    /// @param[in] i_modifyMode The modify mode (or/and/xor)
    /// @param[in] i_ringMode   Ring operation mode.
    /// @return fapi2::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    template< TargetType K >
    inline ReturnCode modifyRing(const Target<K>& i_target,
                                 const scanRingId_t i_address,
                                 variable_buffer& i_data,
                                 const ChipOpModifyMode i_modifyMode,
                                 const RingMode i_ringMode = 0);

    /// @note
    /// These spy access interfaces are only used in FSP and cronus
    /// HB does not allow spy access

#if defined(FAPI_SUPPORT_SPY_AS_ENUM) || defined(FAPI_SUPPORT_SPY_AS_STRING)
    /// @brief Reads a spy from a chip.
    /// @tparam K template parameter, passed in target.
    /// @param[in]  i_target   Target to operate on.
    /// @param[in]  i_spyId    Id of the spy whose data to be read.
    /// @param[out] o_data     Buffer that holds data read from HW target.
    /// @return fapi2::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    ///
    /// @note:  The string version is only supported for cronus.
    ///
    ///         The fapi design to support both FSP and cronus use of get and
    ///         put spy functions is dependant on the SPY names being expanded
    ///         to resemble a valid C identifier. This design places some
    ///         restrictions on the SPY names which can be used.
    ///
    ///         1. if the spy name contains a # procedure writers should replace
    ///         it with an __P__ for example -
    ///
    ///         ABUS.RX0.RXPACKS#0.RXPACK.RD.LC.LC.ACT_DIS
    ///         becomes
    ///         ABUS.RX0.RXPACKS__P__0.RXPACK.RD.LC.LC.ACT_DIS
    ///
    ///         2. if the spy name has a number following a "." it must have an
    ///         underscore prepended to the number.
    ///
    ///         EH.TPCHIP.2KX100_ARY_CLK_EDGES_DLY
    ///         becomes
    ///         EH.TPCHIP._2KX100_ARY_CLK_EDGES_DLY
    ///
    ///         Example SPY name:
    ///         The hardware procedure should call the function like:
    ///
    ///          ABUS.RX0.RXPACKS#0.RXPACK.RD.LC.LC.ACT_DIS
    ///
    ///          fapi2::ReturnCode rc = fapiGetSpy( targ,
    ///                  ABUS.RX0.RXPACKS__P__0.RXPACK.RD.LC.LC.ACT_DIS, data );
    ///
    /// @note The ID is not in quotes the fapi code will handle adding
    ///       the quotes for the cronus environment
    ///
#endif

#ifdef FAPI_SUPPORT_SPY_AS_ENUM

#ifndef DOCUMENTATION
#define fapiGetSpy(TARGET, ID, DATA) \
    _fapiGetSpy(TARGET, FAPI_SPY_NAMES::ID.value, DATA )
#endif
    template< TargetType K >
    inline ReturnCode _fapiGetSpy(const Target<K>& i_target,
                                  const spyId_t i_spyId,
                                  variable_buffer& o_data);
#endif

#ifdef FAPI_SUPPORT_SPY_AS_STRING

#ifndef DOCUMENTATION
#define fapiGetSpy(TARGET, ID, DATA) _fapiGetSpy(TARGET, #ID, DATA)
#endif
    template< TargetType K >
    inline ReturnCode _fapiGetSpy(const Target<K>& i_target,
                                  const char * const i_spyId,
                                  variable_buffer& o_data);
#endif

#if defined(FAPI_SUPPORT_SPY_AS_ENUM) || defined(FAPI_SUPPORT_SPY_AS_STRING)
    /// @brief Writes a spy on a chip.
    /// @tparam K template parameter, passed in target.
    /// @param[in]  i_target   Target to operate on.
    /// @param[in]  i_spyId    Id of the spy to write data to.
    /// @param[out] i_data     Buffer that holds data to write into spy.
    /// @return fapi2::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    ///
    /// @note:  The string version is only supported for cronus.
    ///
    ///         The fapi design to support both FSP and cronus use of get and
    ///         put spy functions is dependent on the SPY names being expanded
    ///         to resemble a valid C identifier. This design places some
    ///         restrictions on the SPY names which can be used.
    ///
    ///         1. if the spy name contains a # procedure writers should replace
    ///         is with an __P__ for example -
    ///
    ///         ABUS.RX0.RXPACKS#0.RXPACK.RD.LC.LC.ACT_DIS
    ///         becomes
    ///         ABUS.RX0.RXPACKS__P__0.RXPACK.RD.LC.LC.ACT_DIS
    ///
    ///         2. if the spy name has a number following a "." it must have an
    ///         underscore prepended to the number.
    ///
    ///         EH.TPCHIP.2KX100_ARY_CLK_EDGES_DLY
    ///         becomes
    ///         EH.TPCHIP._2KX100_ARY_CLK_EDGES_DLY
    ///
    ///         Example SPY name:
    ///         The hardware procedure should call the function like:
    ///
    ///          ABUS.RX0.RXPACKS#0.RXPACK.RD.LC.LC.ACT_DIS
    ///
    ///          fapi2::ReturnCode rc = fapiPutSpy( targ,
    ///                  ABUS.RX0.RXPACKS__P__0.RXPACK.RD.LC.LC.ACT_DIS, data );
    ///
    /// @note The ID is not in quotes the fapi code will handle adding
    ///         the quotes for the cronus environment
    ///
#endif

#ifdef FAPI_SUPPORT_SPY_AS_ENUM
#ifndef DOCUMENTATION
#define fapiPutSpy(TARGET, ID, DATA) \
    _fapiPutSpy(TARGET, FAPI_SPY_NAMES::ID.value, DATA)
#endif
    template< TargetType K >
    inline ReturnCode _fapiPutSpy(const Target<K>& i_target,
                                  const spyId_t i_spyId,
                                  variable_buffer& i_data);
#endif

#ifdef FAPI_SUPPORT_SPY_AS_STRING
#ifndef DOCUMENTATION
#define fapiPutSpy(TARGET, ID, DATA) _fapiPutSpy(TARGET, #ID, DATA)
#endif
    template< TargetType K >
    inline ReturnCode _fapiPutSpy(const Target<K>& i_target,
                                  const char* const i_spyId,
                                  variable_buffer& i_data);

#endif

#if defined(FAPI_SUPPORT_SPY_AS_ENUM) || defined(FAPI_SUPPORT_SPY_AS_STRING)

    /// @brief Writes spy data into a buffer holding ring data image
    ///        This API is used by L2/L3 repair to put column repair data
    ///        into a ring buffer image.
    /// @tparam K template parameter, passed in target.
    /// @param[in]  i_target Target to operate on.
    /// @param[in]  i_spyId  Id of the spy.
    /// @param[in]  i_data   Buffer that holds spy data to write into ring
    ///                      image.
    /// @param[out] o_data   Buffer that holds updated ring image.
    /// @return fapi2::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    ///
    /// @note:  The string version is only supported for cronus.
    ///
    ///         The fapi design to support both FSP and cronus use of get and
    ///         put spy functions is dependent on the SPY names being expanded
    ///         to resemble a valid C identifier. This design places some
    ///         restrictions on the SPY names which can be used.
    ///
    ///         See fapiPutSpy for details on spy id specifics.
    ///
#endif

#ifdef FAPI_SUPPORT_SPY_AS_ENUM
#ifndef DOCUMENTATION
#define fapiPutSpyImage(TARGET, ID, DATA1, DATA2) \
    _fapiPutSpyImage(TARGET,                                            \
                     FAPI_SPY_NAMES::ID.value,                          \
                     DATA1, DATA2)
#endif
    template< TargetType K >
    inline ReturnCode _fapiPutSpyImage(const Target<K>& i_target,
                                       const spyId_t i_spyId,
                                       variable_buffer& i_data,
                                       variable_buffer& o_imageData);
#endif

#ifdef FAPI_SUPPORT_SPY_AS_STRING
// fapiPutSpyImage function Cronus version
#ifndef DOCUMENTATION
#define fapiPutSpyImage(TARGET, ID, DATA1, DATA2) \
    _fapiPutSpyImage(TARGET, #ID,                                       \
                     DATA1,DATA2)
#endif

    template< TargetType K >
    inline ReturnCode _fapiPutSpyImage(const Target<K>& i_target,
                                       const char* const i_spyId,
                                       variable_buffer& i_data,
                                       variable_buffer& o_imageData);
#endif

#if defined(FAPI_SUPPORT_SPY_AS_ENUM) || defined(FAPI_SUPPORT_SPY_AS_STRING)
    /// @brief Reads spy data from a ring image buffer
    /// @param[in]   i_target      Target to operate on
    /// @param[in]   i_spyId       The spy's id
    /// @param[out]  o_data        Buffer that holds data read from ring image.
    /// @param[in]   i_imageData   Buffer that holds ring image to read data
    ///                            from.
    /// @return fapi2::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    ///
    /// @note:  The string version is only supported for cronus.
    ///
    ///         The fapi design to support both FSP and cronus use of get and
    ///         put spy functions is dependent on the SPY names being expanded
    ///         to resemble a valid C identifier. This design places some
    ///         restrictions on the SPY names which can be used.
    ///
    ///         See fapiPutSpy for details on spy id specifics.
    ///
#endif

#ifdef FAPI_SUPPORT_SPY_AS_ENUM
#ifndef DOCUMENTATION
#define fapiGetSpyImage(TARGET, ID, DATA1, DATA2) \
    _fapiGetSpyImage(TARGET,                                            \
                     FAPI_SPY_NAMES::ID.value,                          \
                     DATA1, DATA2)
#endif
    template< TargetType K >
    inline ReturnCode _fapiGetSpyImage(const Target<K>& i_target,
                                       const spyId_t i_spyId,
                                       variable_buffer & o_data,
                                       const variable_buffer & i_imageData);
#endif

#ifdef FAPI_SUPPORT_SPY_AS_STRING
// fapiGetSpyImage function Cronus version
#ifndef DOCUMENTATION
#define fapiGetSpyImage(TARGET, ID, DATA1, DATA2) \
    _fapiGetSpyImage(TARGET,                                            \
                     #ID, DATA1,DATA2)
#endif

    template< TargetType K >
    inline ReturnCode _fapiGetSpyImage(const Target<K>& i_target,
                                       const char* const i_spyId,
                                       variable_buffer & o_data,
                                       const variable_buffer & i_imageData);
#endif

#ifdef FAPI_SUPPORT_MULTI_SCOM

    /// @brief Performs a multiple SCOM operation
    ///        This interface performs multiple SCOM operations on a chip in the
    ///        order specified by the input MultiScom object.
    ///        See fapiMultiScom.H for details of how to populate the MultiScom
    ///        object with SCOM operations.
    ///
    /// @tparam K template parameter, passed in target.
    /// @param[in]     i_target Target to operate on.
    /// @param[in,out] io_multiScomObj Reference to a MultiScom object,
    ///                                pre-populated with SingleScomInfo entries
    ///                                to perform multiple SCOMs on input target
    /// @return fapi2::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    ///
    /// @note   This is a synchronous interface and would return after all the
    ///         SCOM operations are completed or on the first failed operation
    ///
    /// @note   SCOMs will be performed in the order they were added to the
    ///         input MultiScom object
    ///
    /// @note   In case of errors, the platform code is responsible to collect
    ///         and add all the required error info and FFDC into the error data
    ///         for debugging
    ///
    /// @note   If the SCOM operations added are specific to a processor chip,
    ///         then the FSI Shift Engine configured in scatter-gather DMA mode
    ///         extension would be used to execute the SCOM operations in a
    ///         performance optimize mode. In this mode, the special
    ///         SCOM_BULK_READ_MODE and SCOM_BULK_WRITE_MODE operations are
    ///         supported that allow a large bulk of SCOM access (in multiple of
    ///         64 bits) for targets that support auto-increment. The
    ///         SCOM_WRITE_UNDER_MASK operation is not supported in this mode
    ///
    /// @note   If the SCOM operations added are specific to a memory buffer
    ///         chip, then the regular SCOM engine is used to execute the SCOM
    ///         operations. SCOM_WRITE_UNDER_MASK operation is supported in
    ///         this mode, but the special SCOM_BULK_READ_MODE and
    ///         SCOM_BULK_WRITE_MODE operations are not supported due to
    ///         hardware limitations.
    ///
    template< TargetType K >
    ReturnCode fapiMultiScom (const Target<K>& i_target,
                                    MultiScom&  io_multiScomObj);
#endif  // FAPI_SUPPORT_MULTI_SCOM

};

#endif // FAPI2HWACCESS_H_
