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
/// @file mmio_access.H
///
/// @brief I2C access functions that needs to be specialized for
/// platform implementation.
///
/// NOTE : This is just an example implementation for compiling against

#ifndef _FAPI2_MMIO_ACCESS_H_
#define _FAPI2_MMIO_ACCESS_H_

#include <vector>
#include <iostream>
#include <iomanip>

#include <plat_mmio_access.H>
#include <fapi2_mmio_access.H>

namespace fapi2
{

/// @brief Reads data via mmio from the target
template< TargetType K, typename V >
inline ReturnCode getMMIO(const Target<K, V>& i_target,
                          const uint64_t i_mmioAddr,
                          const size_t i_transSize,
                          std::vector<uint8_t>& o_data)
{
    return fapi2plat::getMMIO(i_target, i_mmioAddr, i_transSize, o_data);
}

/// @brief Writes data via mmio to the target.
template< TargetType K, typename V >
inline ReturnCode putMMIO(const Target<K, V>& i_target,
                          const uint64_t i_mmioAddr,
                          const size_t i_transSize,
                          const std::vector<uint8_t>& i_data)
{
    return fapi2plat::putMMIO(i_target, i_mmioAddr, i_transSize, i_data);
}

};


#endif // _FAPI2_MMIO_ACCESS_H_
