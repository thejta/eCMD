// $Id$
// $Source$

/**
 *  @file fapi2SystemConfig.H
 *
 *  @brief Defines the System Config query functions that platform code must
 *         implement. It is an eCMD requirement that these be "C" functions.
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 *                          mjjones     09/12/2011  Updated with new functions.
 *                          mjjones     02/17/2012  Updated fapiGetAssociatedDimms
 *                                                  description to only allow MBA
 *
 */

#ifndef FAPI2SYSTEMCONFIG_H_
#define FAPI2SYSTEMCONFIG_H_

#include <stdint.h>
#include <vector>
#include <return_code.H>
#include <target.H>
#include <target_states.H>

namespace fapi2
{

    /**
     * @brief Gets the Target on the other side of the memory channel
     * - Input MEMBUF_CHIP, Output DMI
     * - Input DMI, Output MEMBUF_CHIP
     *
     * @param[in] i_target Input MCS_CHIPLET or MEMBUF_CHIP Target
     * @param[out] o_target Reference to Target that is set to the Target on
     * the other side of the memory channel, if there is no such
     * Target in the specified state then an error is returned
     * @param[in] i_state Only return a target in the specified state. Either
     * present or functional (default)
     *
     * @return ReturnCode. Zero on success, else error
     */
    ReturnCode getOtherSideOfMemChannel(
        const Target<TARGET_TYPE_MEMBUF_CHIP | TARGET_TYPE_DMI>& i_target,
        Target<TARGET_TYPE_MEMBUF_CHIP | TARGET_TYPE_DMI> & o_target,
        const TargetState i_state = TARGET_STATE_FUNCTIONAL);

    /** 
     * @brief Gets the chiplets that are children of the input chip
     * 
     * @param[in]  i_chip        Input chip
     * @param[in]  i_chipletType Type of chiplet required
     * @param[out] o_chiplets    Reference to vector that is cleared then filled in
     *                           with the children chiplet Targets
     * @param[in]  i_state       Only return targets of the specified state. Either
     *                           present or functional (default)
     * 
     * @return ReturnCode. Zero on success, else error 
     */ 
#if 0
    ReturnCode getChildChiplets(
        const Target & i_chip,
        const TargetType i_chipletType, 
        std::vector<Target> & o_chiplets,
        const TargetState i_state = TARGET_STATE_FUNCTIONAL);
#endif

    /** 
     * @brief Gets the DIMMs that are logically associated with the input MBA target
     *
     * @param[in]  i_target Input MBA chiplet target
     * @param[out] o_dimms  Reference to vector that is cleared then filled in with
     *                      the associated DIMM Targets
     * @param[in]  i_state  Only return targets of the specified state. Either
     *                      present or functional (default)
     * 
     * @return ReturnCode. Zero on success, else error 
     */ 
    ReturnCode getAssociatedDimms(
        const Target<TARGET_TYPE_MBA>& i_target, 
        std::vector<Target<TARGET_TYPE_DIMM> > & o_dimms,
        const TargetState i_state = TARGET_STATE_FUNCTIONAL); 

    /** 
     * @brief Gets the parent chip of the input chiplet
     *
     * @param[in]  i_chiplet Input chiplet
     * @param[out] o_chip    Reference to Target that is set to the parent chip
     * 
     * @return ReturnCode. Zero on success, else error 
     */ 
#if 0
    ReturnCode getParentChip(
        const Target& i_chiplet,
        Target & o_chip);
#endif

} // namespace fapi

#endif // FAPI2SYSTEMCONFIG_H_
