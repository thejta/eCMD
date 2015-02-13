#ifndef fapi2SharedUtils_h
#define fapi2SharedUtils_h
// Copyright **********************************************************
//
// File fapi2SharedUtils.H
//
// IBM Confidential
// OCO Source Materials
// 9400 Licensed Internal Code
// (C) COPYRIGHT IBM CORP. 1996
//
// The source code for this program is not published or otherwise
// divested of its trade secrets, irrespective of what has been
// deposited with the U.S. Copyright Office.
//
// End Copyright ******************************************************


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

#include <target.H>

namespace fapi2
{
    /** @name Misc Functions */
    //@{

    /**
     * @brief Copies contents of a fapi target (fapi2::Target) to an eCMD chip target (ecmdChipTarget)
     * @param i_fapiTarget fapi2::Target to convert
     * @param o_ecmdTarget generated ecmdChipTarget 
     */
    template< TargetType K >
    void fapiTargetToEcmdTarget(const fapi2::Target<K> i_fapiTarget, ecmdChipTarget & o_ecmdTarget)
    {
        o_ecmdTarget = *i_fapiTarget;
    }

    template< TargetType K >
    void fapiTargetPointerToEcmdTarget(const fapi2::Target<K> * const i_pFapiTarget, ecmdChipTarget & o_ecmdTarget)
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


#if 0
    /**
     * @brief Turns an eCMD chip target (ecmdChipTarget) into a fapi target (fapi2::Target)
     * @param i_ecmdTarget ecmdChipTarget to convert
     * @param o_fapiTarget generated fapi2::Target
     */
    template< TargetType K >
    void ecmdTargetToFapiTarget(ecmdChipTarget & i_ecmdTarget, fapi2::Target<K> & o_fapiTarget)
    {
        o_fapiTarget = Target<K>(&i_ecmdTarget);
    }
#endif

    //@}

} // namespace fapi2

#endif /* fapi2SharedUtils_h */
