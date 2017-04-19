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
 *  @file plat_utils_assert.C
 *  @brief Implements fapi2 assert utility
 */

#include <assert.h>

namespace fapi2
{
    ///
    /// @brief Assert a condition, and halt
    ///
    /// @param[in] a boolean representing the assertion
    ///
    void Assert(bool i_expression)
    {
        assert(i_expression);
    }
};
