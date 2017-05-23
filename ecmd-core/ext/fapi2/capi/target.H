/* IBM_PROLOG_BEGIN_TAG                                                   */
/* 
 * Copyright 2017 IBM International Business Machines Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/* IBM_PROLOG_END_TAG                                                     */
/**
 * @file target.H
 * @brief platform specializations for fapi2 targets
 */

#ifndef __FAPI2_TARGET__
#define __FAPI2_TARGET__

#include <plat_target.H>
#include <fapi2_target.H>
#include <multicast.H>
#include <stdio.h>
#include <attribute_ids.H>

#include <fapi2Structs.H>
#include <fapi2SharedUtils.H>
#include <ecmdSharedUtils.H>
#include <ecmdStructs.H>
extern "C" {
    uint32_t fapi2GetAssociatedTargets(
        const ecmdChipTarget& i_target,
        const fapi2::TargetType i_associatedTargetType,
        std::list<const ecmdChipTarget*>& o_targets,
        const fapi2::TargetState i_state);
    uint32_t fapi2GetAttribute(const ecmdChipTarget & i_target, const uint32_t i_id, fapi2::AttributeData & o_data);
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
{
    iv_handle = i_right.iv_handle;
    return *this;
}

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
    return iv_handle == i_right.iv_handle;
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
    return iv_handle != i_right.iv_handle;
}

///
/// @brief Less Than Comparison Operator
/// @param[in] i_right Reference to Target to compare.
/// @return bool. True if less than i_right.
/// @note Platforms need to define this so that the physical
/// targets are determined to be less than rather than just the handles
///
template<TargetType K, typename V>
bool Target<K, V>::operator<(const Target& i_right) const
{
    return iv_handle < i_right.iv_handle;
}

///
/// @brief Get this target's immediate parent
/// @tparam T The type of the parent
/// @return Target<T> a target representing the parent
///
template<TargetType K, typename V>
template<TargetType T>
inline Target<T, V> Target<K, V>::getParent(void) const
{
    // General compound chiplet check for proc parent

    const fapi2::TargetType TARGET_TYPE_PROC_CHIPLETS = fapi2::TARGET_TYPE_EX |
            fapi2::TARGET_TYPE_MCS |
            fapi2::TARGET_TYPE_XBUS |
            fapi2::TARGET_TYPE_CORE |
            fapi2::TARGET_TYPE_EQ |
            fapi2::TARGET_TYPE_MCA |
            fapi2::TARGET_TYPE_MCBIST |
            fapi2::TARGET_TYPE_MI |
            fapi2::TARGET_TYPE_CAPP |
            fapi2::TARGET_TYPE_DMI |
            fapi2::TARGET_TYPE_OBUS |
            fapi2::TARGET_TYPE_OBUS_BRICK |
            fapi2::TARGET_TYPE_SBE |
            fapi2::TARGET_TYPE_PPE |
            fapi2::TARGET_TYPE_PERV |
            fapi2::TARGET_TYPE_PEC |
            fapi2::TARGET_TYPE_PHB |
            fapi2::TARGET_TYPE_PROC_CHIP |
            fapi2::TARGET_TYPE_MC;

    static_assert( !((T == fapi2::TARGET_TYPE_PROC_CHIP) &&
                     ((K & TARGET_TYPE_PROC_CHIPLETS) == fapi2::TARGET_TYPE_NONE)),
                   "fapi2::TARGET_TYPE_PROC_CHIP is not a valid parent" );

    const fapi2::TargetType TARGET_TYPE_PERV_CHIPLETS = fapi2::TARGET_TYPE_EQ |
            fapi2::TARGET_TYPE_CORE |
            fapi2::TARGET_TYPE_XBUS |
            fapi2::TARGET_TYPE_OBUS |
            fapi2::TARGET_TYPE_CAPP |
            fapi2::TARGET_TYPE_OBUS_BRICK |
            fapi2::TARGET_TYPE_MCBIST |
            fapi2::TARGET_TYPE_MCS |
            fapi2::TARGET_TYPE_MCA |
            fapi2::TARGET_TYPE_MI |
            fapi2::TARGET_TYPE_DMI |
            fapi2::TARGET_TYPE_PEC |
            fapi2::TARGET_TYPE_PHB |
            fapi2::TARGET_TYPE_MC;

    static_assert( !((T == fapi2::TARGET_TYPE_PERV) &&
                     ((K & TARGET_TYPE_PERV_CHIPLETS) == fapi2::TARGET_TYPE_NONE)),
                   "fapi2::TARGET_TYPE_PERV is not a valid parent" );

    // Specific parent checks for each TargetType

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
    // MCS -> MCBIST
    // MCS -> PERV
    // MCS -> PROC
    static_assert(!((K == fapi2::TARGET_TYPE_MCS) &&
                    (T != fapi2::TARGET_TYPE_MCBIST) &&
                    (T != fapi2::TARGET_TYPE_PERV) &&
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
    // CORE -> PERV
    // CORE -> PROC
    static_assert(!((K == fapi2::TARGET_TYPE_CORE) &&
                    (T != fapi2::TARGET_TYPE_EX) &&
                    (T != fapi2::TARGET_TYPE_EQ) &&
                    (T != fapi2::TARGET_TYPE_PERV) &&
                    (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                  "improper parent of fapi2::TARGET_TYPE_CORE");

    // valid parents for EQ
    // EQ -> PROC
    // EQ -> PERV
    static_assert(!((K == fapi2::TARGET_TYPE_EQ) &&
                    (T != fapi2::TARGET_TYPE_PERV) &&
                    (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                  "improper parent of fapi2::TARGET_TYPE_EQ");

    // valid parents for MCA
    // MCA -> MCS
    // MCA -> MCBIST
    // MCA -> PERV
    // MCA -> PROC
    static_assert(!((K == fapi2::TARGET_TYPE_MCA) &&
                    (T != fapi2::TARGET_TYPE_MCS) &&
                    (T != fapi2::TARGET_TYPE_MCBIST) &&
                    (T != fapi2::TARGET_TYPE_PERV) &&
                    (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                  "improper parent of fapi2::TARGET_TYPE_MCA");

    // valid parents for MCBIST
    // MCBIST -> PERV
    // MCBIST -> PROC
    static_assert(!((K == fapi2::TARGET_TYPE_MCBIST) &&
                    (T != fapi2::TARGET_TYPE_PERV) &&
                    (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                  "improper parent of fapi2::TARGET_TYPE_MCBIST");

    // valid parents for MI
    // MI -> MC
    // MI -> PERV
    // MI -> PROC
    static_assert(!((K == fapi2::TARGET_TYPE_MI) &&
                    (T != fapi2::TARGET_TYPE_MC) &&
                    (T != fapi2::TARGET_TYPE_PERV) &&
                    (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                  "improper parent of fapi2::TARGET_TYPE_MI");

    // valid parents for DMI
    // DMI -> MI
    // DMI -> MC
    // DMI -> PERV
    // DMI -> PROC
    static_assert(!((K == fapi2::TARGET_TYPE_DMI) &&
                    (T != fapi2::TARGET_TYPE_MI) &&
                    (T != fapi2::TARGET_TYPE_MC) &&
                    (T != fapi2::TARGET_TYPE_PERV) &&
                    (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                  "improper parent of fapi2::TARGET_TYPE_DMI");

    // valid parents for MC
    // MC -> PERV
    // MC -> PROC
    static_assert(!((K == fapi2::TARGET_TYPE_MC) &&
                    (T != fapi2::TARGET_TYPE_PERV) &&
                    (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                  "improper parent of fapi2::TARGET_TYPE_MC");

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
    // PEC -> PERV
    // PEC -> PROC
    static_assert(!((K == fapi2::TARGET_TYPE_PEC) &&
                    (T != fapi2::TARGET_TYPE_PERV) &&
                    (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                  "improper parent of fapi2::TARGET_TYPE_PEC");

    // valid parents for PHB
    // PHB -> PERV
    // PHB -> PROC
    static_assert(!((K == fapi2::TARGET_TYPE_PHB) &&
                    (T != fapi2::TARGET_TYPE_PERV) &&
                    (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                  "improper parent of fapi2::TARGET_TYPE_PHB");

    // valid parents for XBUS
    // XBUS -> PERV
    // XBUS -> PROC
    static_assert(!((K == fapi2::TARGET_TYPE_XBUS) &&
                    (T != fapi2::TARGET_TYPE_PERV) &&
                    (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                  "improper parent of fapi2::TARGET_TYPE_XBUS");

    // valid parents for OBUS
    // OBUS -> PERV
    // OBUS -> PROC
    static_assert(!((K == fapi2::TARGET_TYPE_OBUS) &&
                    (T != fapi2::TARGET_TYPE_PERV) &&
                    (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                  "improper parent of fapi2::TARGET_TYPE_OBUS");

    // valid parents for OBUS_BRICK
    // OBUS_BRICK -> PERV
    // OBUS_BRICK -> OBUS
    // OBUS_BRICK -> PROC
    static_assert(!((K == fapi2::TARGET_TYPE_OBUS_BRICK) &&
                    (T != fapi2::TARGET_TYPE_PERV) &&
                    (T != fapi2::TARGET_TYPE_OBUS) &&
                    (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                  "improper parent of fapi2::TARGET_TYPE_OBUS_BRICK");

    // valid parents for CAPP
    // CAPP -> PERV
    // CAPP -> PROC
    static_assert(!((K == fapi2::TARGET_TYPE_CAPP) &&
                    (T != fapi2::TARGET_TYPE_PERV) &&
                    (T != fapi2::TARGET_TYPE_PROC_CHIP)),
                  "improper parent of fapi2::TARGET_TYPE_CAPP");

    std::list<const ecmdChipTarget*> l_targets;

    plat_target_handle_t l_handle = this->get();
    const ecmdChipTarget * l_pEcmdTarget = l_handle;
    uint32_t l_rc = fapi2GetAssociatedTargets(*l_pEcmdTarget, T, l_targets, TARGET_STATE_FUNCTIONAL);

    if (l_rc)
    {
        // ERROR
        return Target<T, V>();
    }

    if (l_targets.empty())
    {
        // ERROR
        return Target<T, V>();
    }

    return Target<T, V>(plat_target_handle_t(l_targets.front()));
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
inline std::vector<Target<T, V> >
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

    // valid children for PERV
    // PERV -> EQ
    // PERV -> CORE
    // PERV -> XBUS
    // PERV -> OBUS
    // PERV -> CAPP
    // PERV -> NV
    // PERV -> MCBIST
    // PERV -> MCS
    // PERV -> MCA
    // PERV -> PEC
    // PERV -> PHB
    // PERV -> MI
    // PERV -> DMI
    // PERV -> MC
    static_assert(!((K == fapi2::TARGET_TYPE_PERV) &&
                    (T != fapi2::TARGET_TYPE_EQ) &&
                    (T != fapi2::TARGET_TYPE_CORE) &&
                    (T != fapi2::TARGET_TYPE_XBUS) &&
                    (T != fapi2::TARGET_TYPE_OBUS) &&
                    (T != fapi2::TARGET_TYPE_CAPP) &&
                    (T != fapi2::TARGET_TYPE_OBUS_BRICK) &&
                    (T != fapi2::TARGET_TYPE_MCBIST) &&
                    (T != fapi2::TARGET_TYPE_MCS) &&
                    (T != fapi2::TARGET_TYPE_MCA) &&
                    (T != fapi2::TARGET_TYPE_PEC) &&
                    (T != fapi2::TARGET_TYPE_PHB) &&
                    (T != fapi2::TARGET_TYPE_MI) &&
                    (T != fapi2::TARGET_TYPE_DMI) &&
                    (T != fapi2::TARGET_TYPE_MC)),
                  "improper child of fapi2::TARGET_TYPE_PERV");

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

    // valid children for MC
    // MC -> MI
    // MC -> DMI
    static_assert(!((K == fapi2::TARGET_TYPE_MC) &&
                    (T != fapi2::TARGET_TYPE_MI) &&
                    (T != fapi2::TARGET_TYPE_DMI)),
                  "improper child of fapi2::TARGET_TYPE_MC");

    // valid children for OBUS
    // OBUS -> OBUS_BRICK
    static_assert(!((K == fapi2::TARGET_TYPE_OBUS) &&
                    (T != fapi2::TARGET_TYPE_OBUS_BRICK)),
                  "improper child of fapi2::TARGET_TYPE_OBUS");


    std::list<const ecmdChipTarget*> l_targets;
    std::vector<Target<T, V> > l_children;

    plat_target_handle_t l_handle = this->get();
    const ecmdChipTarget * l_pEcmdTarget = l_handle;
    uint32_t l_rc = fapi2GetAssociatedTargets(*l_pEcmdTarget, T, l_targets, i_state);

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
        l_children.push_back(Target<T, V>(plat_target_handle_t(*l_target)));
        l_target++;
    }

    return l_children;
}

///
/// @brief Get this target's children, filtered
/// @tparam T The type of the parent
/// @param[in] i_filter The desired chiplet filter
/// @param[in] i_state The desired TargetState of the children
/// @return std::vector<Target<T> > a vector of present/functional
/// children
///
template<TargetType K, typename V>
template< TargetType T>
inline std::vector<Target<T, V> >
Target<K, V>::getChildren(const TargetFilter i_filter,
    const TargetState i_state) const
{
    const fapi2::TargetType TARGET_TYPE_FILTER_CHIPLETS =
            fapi2::TARGET_TYPE_XBUS |
            fapi2::TARGET_TYPE_OBUS |
            fapi2::TARGET_TYPE_CAPP |
            fapi2::TARGET_TYPE_OBUS_BRICK |
            fapi2::TARGET_TYPE_MCBIST |
            fapi2::TARGET_TYPE_MCS |
            fapi2::TARGET_TYPE_MCA |
            fapi2::TARGET_TYPE_MI |
            fapi2::TARGET_TYPE_DMI |
            fapi2::TARGET_TYPE_PEC |
            fapi2::TARGET_TYPE_PHB |
            fapi2::TARGET_TYPE_PERV |
            fapi2::TARGET_TYPE_CORE |
            fapi2::TARGET_TYPE_EX |
            fapi2::TARGET_TYPE_MC;

    static_assert( !((T & TARGET_TYPE_FILTER_CHIPLETS) == fapi2::TARGET_TYPE_NONE),
                   "not a valid chiplet for filtering" );

    auto l_children = Target<K, V>::getChildren<T>(i_state);
    auto l_child = l_children.begin();

    while (l_child != l_children.end())
    {
        uint64_t l_filter_mask = 0x0;
        uint32_t l_pervasive_chipunit = 0; // default to invalid chipunit
        if (T == fapi2::TARGET_TYPE_PERV)
        {
            // use chipunit of the pervasive target
            plat_target_handle_t l_handle = l_child->get();
            const ecmdChipTarget * l_pEcmdTarget = l_handle;
            l_pervasive_chipunit = l_pEcmdTarget->chipUnitNum;
        }
        else
        {
            // get pervasive parent target
            std::list<const ecmdChipTarget*> l_targets;

            plat_target_handle_t l_handle = l_child->get();
            const ecmdChipTarget * l_pEcmdTarget = l_handle;
            uint32_t l_rc = fapi2GetAssociatedTargets(*l_pEcmdTarget, fapi2::TARGET_TYPE_PERV, l_targets, TARGET_STATE_FUNCTIONAL);

            // check if we get back a valid target
            if ((l_rc == 0) && (!l_targets.empty()))
            {
                l_pervasive_chipunit = l_targets.front()->chipUnitNum;
            }
        }

        switch (l_pervasive_chipunit)
        {
            case 1:
                l_filter_mask = TARGET_FILTER_TP;         // Pervasive 1
                break;
            case 2:
                l_filter_mask = TARGET_FILTER_NEST_NORTH; // Pervasive 2
                break;
            case 3:
                l_filter_mask = TARGET_FILTER_NEST_EAST; // Pervasive 3
                break;
            case 4:
                l_filter_mask = TARGET_FILTER_NEST_SOUTH;  // Pervasive 4
                break;
            case 5:
                l_filter_mask = TARGET_FILTER_NEST_WEST;  // Pervasive 5
                break;
            case 6:
                l_filter_mask = TARGET_FILTER_XBUS;       // Pervasive 6
                break;
            case 7:
                l_filter_mask = TARGET_FILTER_MC_WEST;    // Pervasive 7
                break;
            case 8:
                l_filter_mask = TARGET_FILTER_MC_EAST;    // Pervasive 8
                break;
            case 9:
                l_filter_mask = TARGET_FILTER_OBUS0;      // Pervasive 9
                break;
            case 10:
                l_filter_mask = TARGET_FILTER_OBUS1;      // Pervasive 10
                break;
            case 11:
                l_filter_mask = TARGET_FILTER_OBUS2;      // Pervasive 11
                break;
            case 12:
                l_filter_mask = TARGET_FILTER_OBUS3;      // Pervasive 12
                break;
            case 13:
                l_filter_mask = TARGET_FILTER_PCI0;       // Pervasive 13
                break;
            case 14:
                l_filter_mask = TARGET_FILTER_PCI1;       // Pervasive 14
                break;
            case 15:
                l_filter_mask = TARGET_FILTER_PCI2;       // Pervasive 15
                break;
            case 16:
                l_filter_mask = TARGET_FILTER_CACHE0;     // Pervasive 16
                break;
            case 17:
                l_filter_mask = TARGET_FILTER_CACHE1;     // Pervasive 17
                break;
            case 18:
                l_filter_mask = TARGET_FILTER_CACHE2;     // Pervasive 18
                break;
            case 19:
                l_filter_mask = TARGET_FILTER_CACHE3;     // Pervasive 19
                break;
            case 20:
                l_filter_mask = TARGET_FILTER_CACHE4;     // Pervasive 20
                break;
            case 21:
                l_filter_mask = TARGET_FILTER_CACHE5;     // Pervasive 21
                break;
            case 32:
                l_filter_mask = TARGET_FILTER_CORE0;      // Pervasive 32
                break;
            case 33:
                l_filter_mask = TARGET_FILTER_CORE1;      // Pervasive 33
                break;
            case 34:
                l_filter_mask = TARGET_FILTER_CORE2;      // Pervasive 34
                break;
            case 35:
                l_filter_mask = TARGET_FILTER_CORE3;      // Pervasive 35
                break;
            case 36:
                l_filter_mask = TARGET_FILTER_CORE4;      // Pervasive 36
                break;
            case 37:
                l_filter_mask = TARGET_FILTER_CORE5;      // Pervasive 37
                break;
            case 38:
                l_filter_mask = TARGET_FILTER_CORE6;      // Pervasive 38
                break;
            case 39:
                l_filter_mask = TARGET_FILTER_CORE7;      // Pervasive 39
                break;
            case 40:
                l_filter_mask = TARGET_FILTER_CORE8;      // Pervasive 40
                break;
            case 41:
                l_filter_mask = TARGET_FILTER_CORE9;      // Pervasive 41
                break;
            case 42:
                l_filter_mask = TARGET_FILTER_CORE10;     // Pervasive 42
                break;
            case 43:
                l_filter_mask = TARGET_FILTER_CORE11;     // Pervasive 43
                break;
            case 44:
                l_filter_mask = TARGET_FILTER_CORE12;     // Pervasive 44
                break;
            case 45:
                l_filter_mask = TARGET_FILTER_CORE13;     // Pervasive 45
                break;
            case 46:
                l_filter_mask = TARGET_FILTER_CORE14;     // Pervasive 46
                break;
            case 47:
                l_filter_mask = TARGET_FILTER_CORE15;     // Pervasive 47
                break;
            case 48:
                l_filter_mask = TARGET_FILTER_CORE16;     // Pervasive 48
                break;
            case 49:
                l_filter_mask = TARGET_FILTER_CORE17;     // Pervasive 49
                break;
            case 50:
                l_filter_mask = TARGET_FILTER_CORE18;     // Pervasive 50
                break;
            case 51:
                l_filter_mask = TARGET_FILTER_CORE19;     // Pervasive 51
                break;
            case 52:
                l_filter_mask = TARGET_FILTER_CORE20;     // Pervasive 52
                break;
            case 53:
                l_filter_mask = TARGET_FILTER_CORE21;     // Pervasive 53
                break;
            case 54:
                l_filter_mask = TARGET_FILTER_CORE22;     // Pervasive 54
                break;
            case 55:
                l_filter_mask = TARGET_FILTER_CORE23;     // Pervasive 55
                break;
            default:
                l_filter_mask = 0x0;
                break;
        }

        if ((l_filter_mask & i_filter) == 0x0)
        {
            // filter out
            l_child = l_children.erase(l_child);
        }
        else
        {
            l_child++;
        }
    }

    return l_children;
}

///
/// @brief Get the target at the other end of a bus
/// @tparam T The type of the target on the other end
/// @param[out] o_target A target representing the thing on the other end
/// @param[in] i_state The desired TargetState of the other end
/// @return FAPI2_RC_SUCCESS if OK, platforms will return a non-success
/// ReturnCode in the event of failure
/// @note o_target is only valid if return is FAPI2_RC_SUCCESS
///

template<TargetType K, typename V>
template<TargetType T>
inline fapi2::ReturnCodes
Target<K, V>::getOtherEnd(Target<T, V>& o_target, const TargetState i_state) const
{
    // FIXME add static asserts
    std::list<const ecmdChipTarget*> l_targets;

    plat_target_handle_t l_handle = this->get();
    const ecmdChipTarget * l_pEcmdTarget = l_handle;
    uint32_t l_rc = fapi2GetAssociatedTargets(*l_pEcmdTarget, T, l_targets, i_state);

    if ((l_rc) || (l_targets.empty()))
    {
        // ERROR
        return FAPI2_RC_FALSE;
    }

    o_target = Target<T, V>(l_targets.front());
    return FAPI2_RC_SUCCESS;
}

///
/// @brief Is the target functional?
/// @return true if target is functional, false if non-functional
///

template<TargetType K, typename V>
inline bool
Target<K, V>::isFunctional(void) const
{
    // static target test
    static_assert( is_same<K, fapi2::ATTR_FUNCTIONAL_TargetType>(),
                   "fapi2::Target::isFunctional() not valid for this target type" );
    // return non-functional by default
    fapi2::ATTR_FUNCTIONAL_Type l_functional = fapi2::ENUM_ATTR_FUNCTIONAL_NON_FUNCTIONAL;
    // see if this target has a good ecmdChipTarget
    plat_target_handle_t l_handle = this->get();
    const ecmdChipTarget * l_pEcmdTarget = l_handle;
    if (l_pEcmdTarget != nullptr)
    {
        fapi2::AttributeData o_data;
        uint32_t l_ecmd_rc = 0;

        o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT8;
        l_ecmd_rc = fapi2GetAttribute(*l_pEcmdTarget, fapi2::ATTR_FUNCTIONAL, o_data); 
        if (l_ecmd_rc == 0)
        {
            l_functional = o_data.faUint8;
        }
    }
    return (l_functional == fapi2::ENUM_ATTR_FUNCTIONAL_FUNCTIONAL);
}

///
/// @brief Returns the chiplet number associated with the Target
/// @return The chiplet number for the Target. 0 is returned if the
/// Target does not have a chiplet number (for ex, the PROC_CHIP Target)
/// @note For logical targets such as the EX, the chiplet number of
/// their immediate parent chiplet is returned
///
template<TargetType K, typename V>
inline uint8_t
Target<K, V>::getChipletNumber(void) const
{
    uint8_t l_pervasive_chipunit = 0;
    plat_target_handle_t l_handle = this->get();
    const ecmdChipTarget * l_pEcmdTarget = l_handle;

    // check if this is a PERV target
    if ((l_pEcmdTarget->chipUnitTypeState == ECMD_TARGET_FIELD_VALID) &&
        (l_pEcmdTarget->chipUnitType == "perv") &&
        (l_pEcmdTarget->chipUnitNumState == ECMD_TARGET_FIELD_VALID))
    {
        l_pervasive_chipunit = l_pEcmdTarget->chipUnitNum;
    }
    // check if we can find a perv target from this target
    else if ((l_pEcmdTarget->chipUnitTypeState == ECMD_TARGET_FIELD_VALID) &&
             ((l_pEcmdTarget->chipUnitType == "eq") ||
              (l_pEcmdTarget->chipUnitType == "c") ||
              (l_pEcmdTarget->chipUnitType == "xbus") ||
              (l_pEcmdTarget->chipUnitType == "obus") ||
              (l_pEcmdTarget->chipUnitType == "capp") ||
              (l_pEcmdTarget->chipUnitType == "nv") ||
              (l_pEcmdTarget->chipUnitType == "mcbist") ||
              (l_pEcmdTarget->chipUnitType == "mcs") ||
              (l_pEcmdTarget->chipUnitType == "mca") ||
              (l_pEcmdTarget->chipUnitType == "mi") ||
              (l_pEcmdTarget->chipUnitType == "dmi") ||
              (l_pEcmdTarget->chipUnitType == "pec")  ||
              (l_pEcmdTarget->chipUnitType == "phb") ||
              (l_pEcmdTarget->chipUnitType == "mc")) &&
             (l_pEcmdTarget->chipUnitNumState == ECMD_TARGET_FIELD_VALID))
    {
        std::list<const ecmdChipTarget*> l_targets;
        uint32_t l_rc = fapi2GetAssociatedTargets(*l_pEcmdTarget, fapi2::TARGET_TYPE_PERV, l_targets, TARGET_STATE_FUNCTIONAL);
        if ((l_rc == 0) && (!l_targets.empty()))
        {
            l_pervasive_chipunit = l_targets.front()->chipUnitNum;
        }
    }
    // Special case for ex target 
    else if  ((l_pEcmdTarget->chipUnitTypeState == ECMD_TARGET_FIELD_VALID) &&
        (l_pEcmdTarget->chipUnitType == "ex") &&
        (l_pEcmdTarget->chipUnitNumState == ECMD_TARGET_FIELD_VALID))
    {
        std::list<const ecmdChipTarget*> l_eq_targets;
        uint32_t l_rc = fapi2GetAssociatedTargets(*l_pEcmdTarget, fapi2::TARGET_TYPE_EQ, l_eq_targets, TARGET_STATE_FUNCTIONAL);
        if ((l_rc == 0) && (!l_eq_targets.empty()))
        {
            std::list<const ecmdChipTarget*> l_targets;
            //auto l_eq_target = l_eq_targets.front();
            l_rc = fapi2GetAssociatedTargets(*(l_eq_targets.front()), fapi2::TARGET_TYPE_PERV, l_targets, TARGET_STATE_FUNCTIONAL);
            if ((l_rc == 0) && (!l_targets.empty()))
            {
                l_pervasive_chipunit = l_targets.front()->chipUnitNum;
            }

        }
    }

    return l_pervasive_chipunit;
}

///
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
template< TargetType T, typename V >
inline void toString(const Target<T, V>& i_target, char* i_buffer, size_t i_bsize)
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
/// @param[in] i_target A pointer to the Target<T>
/// @param[in] i_buffer buffer to write in to
/// @param[in] i_bsize  size of the buffer
/// @return void
/// @post The contents of the buffer is replaced with the string
/// representation of the target
///
template< TargetType T, typename V >
inline void toString(const Target<T, V>* i_target, char* i_buffer, size_t i_bsize)
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
template<TargetType T, typename V>
inline Target<T, V> getTarget(uint64_t Ordinal)
{
    // For testing
    return Target<T, V>(NULL);
}

template<>
inline Target<fapi2::TARGET_TYPE_PROC_CHIP, fapi2::plat_target_handle_t> getTarget(uint64_t Ordinal)
{
    fapi2::Target<fapi2::TARGET_TYPE_SYSTEM, fapi2::plat_target_handle_t> l_system;
    ecmdChipTarget l_ecmdTarget;
    fapiTargetToEcmdTarget(l_system, l_ecmdTarget);

    std::list<const ecmdChipTarget*> l_targets;
    uint32_t l_ecmd_rc = fapi2GetAssociatedTargets(l_ecmdTarget, fapi2::TARGET_TYPE_PROC_CHIP, l_targets, TARGET_STATE_PRESENT);

    if ((l_ecmd_rc) || (l_targets.empty()))
    {
        // ERROR
        return Target<fapi2::TARGET_TYPE_PROC_CHIP, fapi2::plat_target_handle_t>(NULL);
    }

    // search fapi positions
    std::list<const ecmdChipTarget*> l_matching_targets;
    uint32_t l_matches = 0;
    for (auto l_target : l_targets)
    {
        fapi2::AttributeData o_data;

        o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT32;
        l_ecmd_rc = fapi2GetAttribute(*l_target, fapi2::ATTR_FAPI_POS, o_data); 
        if ((l_ecmd_rc == 0) && (Ordinal == o_data.faUint32))
        {
            l_matching_targets.push_back(l_target);
            l_matches++;
        }
    }

    if ((l_matches > 1) || (l_matches == 0))
    {
        // ERROR
        return Target<fapi2::TARGET_TYPE_PROC_CHIP, fapi2::plat_target_handle_t>(NULL);
    }

    return Target<fapi2::TARGET_TYPE_PROC_CHIP, fapi2::plat_target_handle_t>(l_matching_targets.front());
}

#ifdef FAPI2_ENABLE_PLATFORM_GET_TARGET
template<TargetType T, typename V=fapi2::plat_target_handle_t>
inline Target<T,V>* getTarget(TargetType i_type, uint8_t instance)
{
    fapi2::Target<fapi2::TARGET_TYPE_SYSTEM, V> l_system;
    ecmdChipTarget l_ecmdTarget;
    fapiTargetToEcmdTarget(l_system, l_ecmdTarget);

    std::list<const ecmdChipTarget*> l_targets;
    uint32_t l_ecmd_rc = fapi2GetAssociatedTargets(l_ecmdTarget, i_type, l_targets, TARGET_STATE_PRESENT);

    if ((l_ecmd_rc) || (l_targets.empty()))
    {
        // ERROR
        return nullptr;
    }

    printf("getTarget, %08X, %d\n", i_type, instance);
    // There is no ATTR_FAPI_POS for the system target
    if (i_type == fapi2::TARGET_TYPE_SYSTEM)
    {
        return new Target<T,V>(l_targets.front());
    }

    // search fapi positions
    std::list<const ecmdChipTarget*> l_matching_targets;
    uint32_t l_matches = 0;
    for (auto l_target : l_targets)
    {
        fapi2::AttributeData o_data;

        o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT32;
        l_ecmd_rc = fapi2GetAttribute(*l_target, fapi2::ATTR_FAPI_POS, o_data); 
        if ((l_ecmd_rc == 0) && (instance == o_data.faUint32))
        {
            l_matching_targets.push_back(l_target);
            l_matches++;
        }
    }

    if ((l_matches > 1) || (l_matches == 0))
    {
        // ERROR
        return nullptr;
    }

    return new Target<T,V>(l_matching_targets.front());
}
#endif // FAPI2_ENABLE_PLATFORM_GET_TARGET

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
template<TargetType T, typename V, typename B>
inline void toString(const Target<T, V>& i_target, B& i_buffer)
{
}

}

#endif
