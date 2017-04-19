//IBM_PROLOG_BEGIN_TAG
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
//IBM_PROLOG_END_TAG

#ifndef fapi2SharedUtils_h
#define fapi2SharedUtils_h


/**
 * @file fapi2SharedUtils.H
 * @brief Useful functions for use throughout the fapi C API and Plugin
 *
 */


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <inttypes.h>

#include <ecmdStructs.H>
#include <ecmdUtils.H>

#include <target.H>

namespace fapi2
{
    /** @name Misc Functions */
    //@{

    /**
     * @brief Copies contents of a fapi target (fapi2::Target) to an eCMD chip target (ecmdChipTarget)
     * @param[in] i_fapiTarget fapi2::Target to convert
     * @param[out] o_ecmdTarget generated ecmdChipTarget 
     */
    template< TargetType K, typename V >
    inline void fapiTargetToEcmdTarget(const fapi2::Target<K, V> i_fapiTarget, ecmdChipTarget & o_ecmdTarget)
    {
        const fapi2::plat_target_handle_t & l_handle = i_fapiTarget.get();
        const ecmdChipTarget * l_pEcmdTarget = static_cast<const ecmdChipTarget *>(l_handle);
        if (l_pEcmdTarget != NULL)
        {
            o_ecmdTarget = *l_pEcmdTarget;
        }
        else
        {
            o_ecmdTarget.cageState = ECMD_TARGET_FIELD_VALID;
            o_ecmdTarget.nodeState = ECMD_TARGET_FIELD_UNUSED;
        }
    }

    /**
     * @brief Copies contents of a fapi target pointer (fapi2::Target *) to an eCMD chip target (ecmdChipTarget)
     * @param[in] i_pFapiTarget fapi2::Target pointer to convert
     * @param[out] o_ecmdTarget generated ecmdChipTarget 
     */
    template< TargetType K, typename V >
    inline void fapiTargetPointerToEcmdTarget(const fapi2::Target<K, V> * const i_pFapiTarget, ecmdChipTarget & o_ecmdTarget)
    {
        if (i_pFapiTarget != NULL)
        {
            fapiTargetToEcmdTarget(*i_pFapiTarget, o_ecmdTarget);
        }
        else
        {
            o_ecmdTarget.cageState = ECMD_TARGET_FIELD_VALID;
            o_ecmdTarget.nodeState = ECMD_TARGET_FIELD_UNUSED;
        }
    }
    //@}

} // namespace fapi2

#endif /* fapi2SharedUtils_h */
