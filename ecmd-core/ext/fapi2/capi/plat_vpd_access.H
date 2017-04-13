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
/// @file plat_vpd_access.H
///
/// @brief Platform vpd-access definitions
///

#ifndef _FAPI2_PLAT_VPDACCESS_H_
#define _FAPI2_PLAT_VPDACCESS_H_

#include <fapi2_vpd_access.H>

namespace fapi2plat
{
fapi2::ReturnCode getVPD(const fapi2::Target<fapi2::TARGET_TYPE_MCS, fapi2::plat_target_handle_t>& i_target,
                         fapi2::VPDInfo<fapi2::TARGET_TYPE_MCS>& io_vpd_info,
                         uint8_t* o_blob);
};

namespace fapi2
{

inline fapi2::ReturnCode platGetVPD(const fapi2::Target<fapi2::TARGET_TYPE_MCS, fapi2::plat_target_handle_t>& i_target,
                                    VPDInfo<fapi2::TARGET_TYPE_MCS>& io_vpd_info,
                                    uint8_t* o_blob)
{
    return fapi2plat::getVPD(i_target, io_vpd_info, o_blob);
}

};
#endif // _FAPI2_PLAT_VPDACCESS_H_
