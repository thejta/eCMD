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
 * @file target.H
 * @brief platform specializations for fapi2 targets
 */

#ifndef __FAPI2_TARGET__
#define __FAPI2_TARGET__

#include <plat_target.H>
#include <fapi2_target.H>
#include <stdio.h>

#include <fapi2SharedUtils.H>
#include <ecmdSharedUtils.H>
#include <ecmdStructs.H>
extern "C" {
uint32_t fapi2GetAssociatedTargets(
    const ecmdChipTarget & i_target, 
    const fapi2::TargetType i_associatedTargetType, 
    std::list<const ecmdChipTarget *> & o_targets, 
    const fapi2::TargetState i_state); 
}

namespace fapi2
{
    ///
    /// @brief Assignment Operator.
    /// @param[in] i_right Reference to Target to assign from.
    /// @return Reference to 'this' Target
    ///
    template<TargetType K, typename V>
    Target<K, V>& Target<K, V>::operator=(const Target& i_right)
    { iv_handle = i_right.iv_handle; return *this; }

    ///
    /// @brief Equality Comparison Operator
    /// @param[in] i_right Reference to Target to compare.
    /// @return bool. True if equal.
    /// @note Platforms need to define this so that the physical
    /// targets are determined to be equivilent rather than just the handles
    ///
    template<TargetType K, typename V>
    bool Target<K, V>::operator==(const Target& i_right) const
    {
        return ((*iv_handle) == (*(i_right.iv_handle)));
    }

    ///
    /// @brief Inquality Comparison Operator
    /// @param[in] i_right Reference to Target to compare.
    /// @return bool. True if not equal.
    /// @note Platforms need to define this so that the physical
    /// targets are determined to be equivilent rather than just the handles
    ///
    template<TargetType K, typename V>
    bool Target<K, V>::operator!=(const Target& i_right) const
    {
        return !((*iv_handle) == (*(i_right.iv_handle)));
    }

    ///
    /// @brief Get this target's immediate parent
    /// @tparam T The type of the parent
    /// @return Target<T> a target representing the parent
    ///
    template<TargetType K, typename V>
    template<TargetType T>
    inline Target<T> Target<K, V>::getParent(void) const
    {
        // valid parents for DIMM
        // DIMM -> MCA
        // DIMM -> MBA
        static_assert(!((K == fapi2::TARGET_TYPE_DIMM) &&
                        (T != fapi2::TARGET_TYPE_MCA) &&
                        (T != fapi2::TARGET_TYPE_MBA)),
                      "improper parent of fapi2::TARGET_TYPE_DIMM");

        // valid parents for PROC
        // PROC -> SYSTEM
        static_assert(!((K == fapi2::TARGET_TYPE_PROC_CHIP) &&
                        (T != fapi2::TARGET_TYPE_SYSTEM)),
                      "improper parent of fapi2::TARGET_TYPE_PROC_CHIP");

        // valid parents for MEMBUF
        // MEMBUF -> SYSTEM
        // MEMBUF -> DMI
        static_assert(!((K == fapi2::TARGET_TYPE_MEMBUF_CHIP) &&
                        (T != fapi2::TARGET_TYPE_SYSTEM) &&
                        (T != fapi2::TARGET_TYPE_DMI)),
                      "improper parent of fapi2::TARGET_TYPE_MEMBUF_CHIP");

        // valid parents for EX
        // EX -> EQ
        // EX -> PROC
        static_assert(!((K == fapi2::TARGET_TYPE_EX) &&
                        (T != fapi2::TARGET_TYPE_EQ) &&
                        (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                      "improper parent of fapi2::TARGET_TYPE_EX");

        // valid parents for MBA
        // MBA -> MEMBUF
        static_assert(!((K == fapi2::TARGET_TYPE_MBA) &&
                        (T != fapi2::TARGET_TYPE_MEMBUF_CHIP)),
                      "improper parent of fapi2::TARGET_TYPE_MBA");

        // valid parents for MCS
        // MCS -> PROC
        // MCS -> MCBIST
        static_assert(!((K == fapi2::TARGET_TYPE_MCS) &&
                        (T != fapi2::TARGET_TYPE_MCBIST) &&
                        (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                      "improper parent of fapi2::TARGET_TYPE_MCS");

        // valid parents for L4
        // L4 -> MEMBUF
        static_assert(!((K == fapi2::TARGET_TYPE_L4) &&
                        (T != fapi2::TARGET_TYPE_MEMBUF_CHIP)),
                      "improper parent of fapi2::TARGET_TYPE_L4");

        // valid parents for CORE
        // CORE -> EX
        // CORE -> EQ
        // CORE -> PROC
        static_assert(!((K == fapi2::TARGET_TYPE_CORE) &&
                        (T != fapi2::TARGET_TYPE_EX) &&
                        (T != fapi2::TARGET_TYPE_EQ) &&
                        (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                      "improper parent of fapi2::TARGET_TYPE_CORE");

        // valid parents for EQ
        // EQ -> PROC
        static_assert(!((K == fapi2::TARGET_TYPE_EQ) &&
                        (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                      "improper parent of fapi2::TARGET_TYPE_EQ");

        // valid parents for MCA
        // MCA -> MCS
        // MCA -> MCBIST
        // MCA -> PROC
        static_assert(!((K == fapi2::TARGET_TYPE_MCA) &&
                        (T != fapi2::TARGET_TYPE_MCS) &&
                        (T != fapi2::TARGET_TYPE_MCBIST) &&
                        (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                      "improper parent of fapi2::TARGET_TYPE_MCA");

        // valid parents for MCBIST
        // MCBIST -> PROC
        static_assert(!((K == fapi2::TARGET_TYPE_MCBIST) &&
                        (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                      "improper parent of fapi2::TARGET_TYPE_MCBIST");

        // valid parents for MI
        // MI -> PROC
        static_assert(!((K == fapi2::TARGET_TYPE_MI) &&
                        (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                      "improper parent of fapi2::TARGET_TYPE_MI");

        // valid parents for DMI
        // DMI -> MI
        // DMI -> PROC
        static_assert(!((K == fapi2::TARGET_TYPE_DMI) &&
                        (T != fapi2::TARGET_TYPE_MI) &&
                        (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                      "improper parent of fapi2::TARGET_TYPE_DMI");

        // valid parents for SBE
        // SBE -> PROC
        static_assert(!((K == fapi2::TARGET_TYPE_SBE) &&
                        (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                      "improper parent of fapi2::TARGET_TYPE_SBE");

        // valid parents for PPE
        // PPE -> PROC
        static_assert(!((K == fapi2::TARGET_TYPE_PPE) &&
                        (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                      "improper parent of fapi2::TARGET_TYPE_PPE");

        // valid parents for PERV
        // PERV -> PROC
        static_assert(!((K == fapi2::TARGET_TYPE_PERV) &&
                        (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                      "improper parent of fapi2::TARGET_TYPE_PERV");

        // valid parents for PEC
        // PEC -> PROC
        static_assert(!((K == fapi2::TARGET_TYPE_PEC) &&
                        (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                      "improper parent of fapi2::TARGET_TYPE_PEC");

        // valid parents for PHB
        // PHB -> PROC
        static_assert(!((K == fapi2::TARGET_TYPE_PHB) &&
                        (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                      "improper parent of fapi2::TARGET_TYPE_PHB");

// FIXME        TARGET_TYPE_XBUS
// FIXME        TARGET_TYPE_OBUS
// FIXME        TARGET_TYPE_NV

        std::list<const ecmdChipTarget *> l_targets;

        uint32_t l_rc = fapi2GetAssociatedTargets(**this, T, l_targets, TARGET_STATE_FUNCTIONAL);
        if (l_rc)
        {
            // ERROR
            return Target<T>();
        }
        if (l_targets.empty())
        {
            // ERROR
            return Target<T>();
        }
        return Target<T>(l_targets.front());
    }

    ///
    /// @brief Get this target's children
    /// @tparam T The type of the parent
    /// @param[in] i_state The desired TargetState of the children
    /// @return std::vector<Target<T> > a vector of present/functional
    /// children
    /// @warning The children of EX's (cores) are expected to be returned
    /// in order. That is, core 0 is std::vector[0].
    ///
    template<TargetType K, typename V>
    template< TargetType T>
    inline std::vector<Target<T> >
    Target<K, V>::getChildren(const TargetState i_state) const
    {
#define INVALID_CHILD(PARENT, CHILD) \
    static_assert(!((K == PARENT) && (T == CHILD)), \
                  #CHILD " is not a child of " #PARENT );

        // invalid children for proc
        INVALID_CHILD(fapi2::TARGET_TYPE_PROC_CHIP, fapi2::TARGET_TYPE_NONE)
        INVALID_CHILD(fapi2::TARGET_TYPE_PROC_CHIP, fapi2::TARGET_TYPE_SYSTEM)
        INVALID_CHILD(fapi2::TARGET_TYPE_PROC_CHIP, fapi2::TARGET_TYPE_DIMM)
        INVALID_CHILD(fapi2::TARGET_TYPE_PROC_CHIP, fapi2::TARGET_TYPE_PROC_CHIP)
        INVALID_CHILD(fapi2::TARGET_TYPE_PROC_CHIP, fapi2::TARGET_TYPE_MEMBUF_CHIP)
        INVALID_CHILD(fapi2::TARGET_TYPE_PROC_CHIP, fapi2::TARGET_TYPE_MBA)
#undef INVALID_CHILD

        // valid children for MEMBUF
        // MEMBUF -> L4
        // MEMBUF -> MBA
        static_assert(!((K == fapi2::TARGET_TYPE_MEMBUF_CHIP) &&
                        (T != fapi2::TARGET_TYPE_L4) &&
                        (T != fapi2::TARGET_TYPE_MBA)),
                      "improper child of fapi2::TARGET_TYPE_MEMBUF_CHIP");

        // valid children for system
        // SYSTEM -> PROC
        // SYSTEM -> MEMBUF
        // SYSTEM -> DIMM ??? disabled for now
        static_assert(!((K == fapi2::TARGET_TYPE_SYSTEM) &&
                        (T != fapi2::TARGET_TYPE_PROC_CHIP) &&
                        (T != fapi2::TARGET_TYPE_MEMBUF_CHIP)),
                      "improper child of fapi2::TARGET_TYPE_SYSTEM");

        // valid children for EQ
        // EQ -> CORE
        // EQ -> EX
        static_assert(!((K == fapi2::TARGET_TYPE_EQ) &&
                        (T != fapi2::TARGET_TYPE_CORE) &&
                        (T != fapi2::TARGET_TYPE_EX)),
                      "improper child of fapi2::TARGET_TYPE_EQ");

        // valid children for EX
        // EX -> CORE
        static_assert(!((K == fapi2::TARGET_TYPE_EX) &&
                        (T != fapi2::TARGET_TYPE_CORE)),
                      "improper child of fapi2::TARGET_TYPE_EX");

        // Nimbus Memory
        // valid children for MCS
        // MCS -> MCA
        static_assert(!((K == fapi2::TARGET_TYPE_MCS) &&
                        (T != fapi2::TARGET_TYPE_MCA)),
                      "improper child of fapi2::TARGET_TYPE_MCS");

        // valid children for MCA
        // MCA -> DIMM
        static_assert(!((K == fapi2::TARGET_TYPE_MCA) &&
                        (T != fapi2::TARGET_TYPE_DIMM)),
                      "improper child of fapi2::TARGET_TYPE_MCA");

        // valid children for MCBIST
        // MCBIST -> MCA
        // MCBIST -> MCS
        static_assert(!((K == fapi2::TARGET_TYPE_MCBIST) &&
                        (T != fapi2::TARGET_TYPE_MCA) &&
                        (T != fapi2::TARGET_TYPE_MCS)),
                      "improper child of fapi2::TARGET_TYPE_MCBIST");

        // Cumulus Memory
        // valid children for MI
        // MI -> DMI
        static_assert(!((K == fapi2::TARGET_TYPE_MI) &&
                        (T != fapi2::TARGET_TYPE_DMI)),
                      "improper child of fapi2::TARGET_TYPE_MI");

        // valid children for DMI
        // DMI -> MEMBUF
        static_assert(!((K == fapi2::TARGET_TYPE_DMI) &&
                        (T != fapi2::TARGET_TYPE_MEMBUF_CHIP)),
                      "improper child of fapi2::TARGET_TYPE_DMI");

        // valid children for MBA
        // MBA -> DIMM
        static_assert(!((K == fapi2::TARGET_TYPE_MBA) &&
                        (T != fapi2::TARGET_TYPE_DIMM)),
                      "improper child of fapi2::TARGET_TYPE_MBA");

        std::list<const ecmdChipTarget *> l_targets;
        std::vector<Target<T> > l_children;

        uint32_t l_rc = fapi2GetAssociatedTargets(**this, T, l_targets, i_state);
        if (l_rc)
        {
            // ERROR
            return l_children;
        }
        if (l_targets.empty())
        {
            // ERROR
            return l_children;
        }
        auto l_target = l_targets.begin();
        while (l_target != l_targets.end())
        {
            l_children.push_back(Target<T>(*l_target));
            l_target++;
        }
        return l_children;
    }

    ///
    /// @brief Get the target at the other end of a bus - dimm included
    /// @tparam T The type of the parent
    /// @param[in] i_state The desired TargetState of the children
    /// @return Target<T> a target representing the thing on the other end
    /// @note Can be easily changed to a vector if needed
    ///
    template<TargetType K, typename V>
    template<TargetType T>
    inline Target<T>
    Target<K, V>::getOtherEnd(const TargetState i_state) const
    {
        // FIXME add static asserts
        std::list<const ecmdChipTarget *> l_targets;

        uint32_t l_rc = fapi2GetAssociatedTargets(**this, T, l_targets, i_state);
        if (l_rc)
        {
            // ERROR
            return Target<T>();
        }
        if (l_targets.empty())
        {
            // ERROR
            return Target<T>();
        }
        return Target<T>(l_targets.front());
    }


    ///
    /// @brief Return the string interpretation of this target
    /// @tparam T The type of the target
    /// @param[in] i_target Target<T>
    /// @param[in] i_buffer buffer to write in to
    /// @param[in] i_bsize size of the buffer
    /// @return void
    /// @post The contents of the buffer is replaced with the string
    /// representation of the target
    ///
    template< TargetType T >
    inline void toString(const Target<T>& i_target, char* i_buffer, size_t i_bsize)
    {
        ecmdChipTarget l_ecmdTarget;
        fapiTargetToEcmdTarget(i_target, l_ecmdTarget);
        std::string l_ecmdTargetString = ecmdWriteTarget(l_ecmdTarget, ECMD_DISPLAY_TARGET_HYBRID);
        snprintf(i_buffer, i_bsize, "%s", l_ecmdTargetString.c_str());
    }

    ///
    /// @brief Return the string interpretation of this target
    /// @tparam T The type of the target
    /// @tparam B The type of the buffer
    /// @param[in] A pointer to the Target<T>
    /// @param[in] i_buffer buffer to write in to
    /// @param[in] i_bsize size of the buffer
    /// @return void
    /// @post The contents of the buffer is replaced with the string
    /// representation of the target
    ///
    template< TargetType T >
    inline void toString(const Target<T>* i_target, char* i_buffer, size_t i_bsize)
    {
        ecmdChipTarget l_ecmdTarget;
        fapiTargetPointerToEcmdTarget(i_target, l_ecmdTarget);
        std::string l_ecmdTargetString = ecmdWriteTarget(l_ecmdTarget, ECMD_DISPLAY_TARGET_HYBRID);
        snprintf(i_buffer, i_bsize, "%s", l_ecmdTargetString.c_str());
    }

    ///
    /// @brief Get an enumerated target of a specific type
    /// @tparam T The type of the target
    /// @param[in] Ordinal representing the ordinal number of
    /// the desired target
    /// @return Target<T> the target requested
    ///
    template<TargetType T>
    inline Target<T> getTarget(uint64_t Ordinal)
    {
        // For testing
        return Target<T>(NULL);
    }

    ///
    /// @brief Return the string interpretation of this target
    /// @tparam T The type of the target
    /// @tparam B The type of the buffer
    /// @param[in] i_target Target<T>
    /// @param[in] i_buffer buffer
    /// @return void
    /// @post The contents of the buffer is replaced with the string
    /// representation of the target
    ///
    template<TargetType T, typename B>
    inline void toString(const Target<T>& i_target, B& i_buffer)
    {
    }

}

#endif
