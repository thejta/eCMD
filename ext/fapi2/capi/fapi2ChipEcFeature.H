// $Id$
// $Source$

/**
 *  @file fapi2ChipEcFeature.H
 *
 *  @brief Defines the fapiQueryChipEcFeature function that allows HWPs to
 *         query if a particular chip has a feature determined by its EC level.
 *         Chip EC features are specified in attribute XML files and the
 *         fapiQueryChipEcFeature function implementation is auto-generated.
 *         HWPs should not call this function directly, but should access the
 *         corresponding HWPF attribute using FAPI_ATTR_GET
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 *                          mjjones     03/26/2012  Created.
 */

#ifndef FAPI2CHIPECFEATURE_H_
#define FAPI2CHIPECFEATURE_H_

#include <stdint.h>
#include <fapi2AttributeIds.H>
#include <return_code.H>

namespace fapi2
{

class Target;

/**
 * @brief Queries if a Chip has a particular feature
 *
 * This should only be called by FAPI during the processing of a FAPI_ATTR_GET
 * for a Chip EC Feature attribute
 *
 * @param[in]  i_id         Attribute ID of the Chip EC Feature
 * @param[in]  i_pTarget    Pointer to chip target
 * @param[out] o_hasFeature Set to 1 if chip has feature else 0
 * @return ReturnCode. Zero on success, else platform specified error
 */
ReturnCode queryChipEcFeature(AttributeId i_id,
                              const Target * i_pTarget,
                              uint8_t & o_hasFeature);
}

#endif
