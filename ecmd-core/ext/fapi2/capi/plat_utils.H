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
 * @file plat_utils.H
 * @brief platform definitions for fapi2 targets
 */

#ifndef __FAPI2_PLAT_UTILS__
#define __FAPI2_PLAT_UTILS__

#include <stdint.h>


namespace fapi2plat
{
    /**  @brief This function answers the question, is scan tracing turned on?
     *   The implementation of this function is specific to the platform.
     *
     *   @returns Boolean indication
     */
    bool isScanTraceEnabled();

    /**  @brief Alter the state of scand tracing.
     *   The implementation of this function is specific to the platform.
     *
     *   @param[in] i_enable True to enable or false to disable scan trace.
     *   @return  void
     */
    void setScanTrace(bool i_enable);

    const fapi2::TargetType TARGET_TYPE_SPECIAL_WAKEUP_TARGET = fapi2::TARGET_TYPE_PROC_CHIP |
                                                                fapi2::TARGET_TYPE_CORE |
                                                                fapi2::TARGET_TYPE_EX |
                                                                fapi2::TARGET_TYPE_EQ;

    fapi2::ReturnCode specialWakeup(const fapi2::Target<fapi2plat::TARGET_TYPE_SPECIAL_WAKEUP_TARGET, fapi2::plat_target_handle_t> & i_target, bool i_enable);
};

namespace fapi2
{
template<TargetType T, typename V>
inline ReturnCode platSpecialWakeup( const Target<T, V>& i_target, bool i_enable)
{
    return fapi2plat::specialWakeup(i_target, i_enable);
}

}
#endif
