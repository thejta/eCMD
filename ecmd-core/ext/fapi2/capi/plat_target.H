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
 * @file plat_target.H
 * @brief platform definitions for fapi2 targets
 */

#ifndef __FAPI2_PLAT_TARGET__
#define __FAPI2_PLAT_TARGET__

#include <ecmdStructs.H>
#include <ecmdChipTargetCompare.H>
#include <multicast_defs.H>
#include <iostream>
#include <ecmdSharedUtils.H>
//
// Define what a platform handle looks like. For Hostboot,
// for example, this might be a void*. For the SBE, this
// will be a uint64_t ...
//
namespace fapi2
{
    typedef struct _ecmd_target_handle
    {
        // default contstructor
        _ecmd_target_handle()
            {
                value = NULL;
                multicast = false;
                multicast_type = MULTICAST_OR;
                multicast_group = 0xFFFFFFFF;
            }
        // constructor with ecmdChipTarget pointer will set
        _ecmd_target_handle(const ecmdChipTarget * i_value, const bool i_multicast = false)
            {
                value = i_value;
                multicast = i_multicast;
                multicast_type = MULTICAST_OR;
                multicast_group = 0xFFFFFFFF;
            }

        bool operator==(const _ecmd_target_handle& rhs) const
            { return (*value) == (*rhs.value); }

        bool operator!=(const _ecmd_target_handle& rhs) const
            { return !((*value) == (*rhs.value)); }

        bool operator<(const _ecmd_target_handle& rhs) const
        {
            ecmdChipTargetCompare ectc;
            return ectc(*value, *(rhs.value));
        }

        _ecmd_target_handle& operator=(const _ecmd_target_handle& i_right)
            {
                value = i_right.value;
                multicast = i_right.multicast;
                multicast_type = i_right.multicast_type;
                multicast_group = i_right.multicast_group;
                return *this;
            }

        inline operator const ecmdChipTarget *() const { return value; }
        inline operator uint64_t() const
            {
                uint64_t l_value = 0;
                if (value != NULL)
                {
                    l_value |= static_cast<uint64_t>(value->node) << 16;
                    l_value |= static_cast<uint64_t>(value->pos) << 8;
                    l_value |= static_cast<uint64_t>(value->chipUnitNum);
                }
                return l_value;
            }

        const ecmdChipTarget * value;
        bool multicast;
        MulticastType multicast_type;
        MulticastGroup multicast_group;
    } plat_target_handle_t;
}

inline std::ostream& operator<<(std::ostream& os, const fapi2::_ecmd_target_handle& eth)
{
    if (eth.value)
        os << ecmdWriteTarget(*(eth.value), ECMD_DISPLAY_TARGET_HYBRID);
    else
        os << "k0";
    return os;
}

#endif
