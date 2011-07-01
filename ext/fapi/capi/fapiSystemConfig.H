// IBM_PROLOG_BEGIN_TAG 
// This is an automatically generated prolog. 
//  
// fipsrefactordoc src/hwpf/fapi/fapiSystemConfig.H 1.2 
//  
// IBM CONFIDENTIAL 
//  
// OBJECT CODE ONLY SOURCE MATERIALS 
//  
// COPYRIGHT International Business Machines Corp. 2011 
// All Rights Reserved 
//  
// The source code for this program is not published or otherwise 
// divested of its trade secrets, irrespective of what has been 
// deposited with the U.S. Copyright Office. 
//  
// IBM_PROLOG_END_TAG 
/**
 *  @file fapiSystemConfig.H
 *
 *  @brief Defines the System Config query functions that platform code must
 *         implement. It is an eCMD requirement that these be "C" functions.
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 *                          mjjones     06/07/2011  Created.
 */

#ifndef FAPISYSTEMCONFIG_H_
#define FAPISYSTEMCONFIG_H_

#include <stdint.h>
#include <fapiReturnCode.H>
#include <fapiTarget.H>


extern "C"
{

/**
 * @brief Gets the functional chiplets that are children of the supplied target
 *
 * @param[in]  i_target     Parent target
 * @param[in]  i_targetType Type of chiplet required
 * @param[out] o_chiplets   Reference to vector that is filled in with the
 *                          result chiplets
 *
 * @return ReturnCode. Zero on success, else error
 */
fapi::ReturnCode fapiGetFunctionalChiplets(const fapi::Target& i_target,
                                       const fapi::TargetType i_chipletType,
                                       std::vector<fapi::Target> & o_chiplets);

/**
 * @brief Gets the existing chiplets that are children of the supplied target
 *
 * @param[in]  i_target     Parent target
 * @param[in]  i_targetType Type of chiplet required
 * @param[out] o_chiplets   Reference to vector that is filled in with the
 *                          result chiplets
 *
 * @return ReturnCode. Zero on success, else error
 */
fapi::ReturnCode fapiGetExistingChiplets(const fapi::Target& i_target,
                                     const fapi::TargetType i_chipletType,
                                     std::vector<fapi::Target> & o_chiplets);

/**
 * @brief Gets the functional DIMMs that are children of the supplied target
 *
 * @param[in]  i_target Parent target
 * @param[out] o_dimms  Reference to vector that is filled in with the result
 *                      DIMMs
 *
 * @return ReturnCode. Zero on success, else error
 */
fapi::ReturnCode fapiGetFunctionalDimms(const fapi::Target& i_target,
                                    std::vector<fapi::Target> & o_dimms);

/**
 * @brief Gets the existing DIMMs that are children of the supplied target
 *
 * @param[in]  i_target Parent target
 * @param[out] o_dimms  Reference to vector that is filled in with the result
 *                      DIMMs
 *
 * @return ReturnCode. Zero on success, else error
 */
fapi::ReturnCode fapiGetExistingDimms(const fapi::Target& i_target,
                                  std::vector<fapi::Target> & o_dimms);

} // extern "C"

#endif // FAPISYSTEMCONFIG_H_
