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
#ifndef _FAPI2_PLAT_MMIO_ACCESS_H_
#define _FAPI2_PLAT_MMIO_ACCESS_H_

#include <stdint.h>
#include <vector>

#include <return_code.H>
#include <target.H>

namespace fapi2plat
{

fapi2::ReturnCode getMMIO(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP, fapi2::plat_target_handle_t>& i_target,
                          const uint64_t i_mmioAddr,
                          const size_t i_transSize,
                          std::vector<uint8_t>& o_data);

fapi2::ReturnCode putMMIO(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP, fapi2::plat_target_handle_t>& i_target,
                          const uint64_t i_mmioAddr,
                          const size_t i_transSize,
                          const std::vector<uint8_t>& i_data);

};

#endif // _FAPI2_PLAT_MMIO_ACCESS_H_

