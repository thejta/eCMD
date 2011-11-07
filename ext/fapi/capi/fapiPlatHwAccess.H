/**
 *  @file fapiPlatHwAccess.H
 *
 *  @brief Defines hardware-access functions for the platform layer.  
 *         Hardware procedure writers will not call these functions. 
 *       
 *         Implementors of platform code will provide worker routines
 *         for these functions in file hwpf/plat/fapiPlatHwAccess.C.
 *         fapi.H will not include this file, so platform implementors
 *         will have to include this file explicitly. 
 * 
 *         These platform entry points are called by fapi functions from 
 *         hwpf/fapi/fapiHwAccess.C which are wrapper functions that 
 *         output scand traces common to all platforms.  
 * 
 *         These functions have the same argument signatures as the
 *         fapi-level functions, but the function names her start with 
 *         "plat."
 *     
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 *                          monte       8sep2011    new
 */

#ifndef FAPIPLATHWACCESS_H_
#define FAPIPLATHWACCESS_H_

#include <stdint.h>
#include <ecmdDataBufferBase.H>
#include <fapiReturnCode.H>
#include <fapiTarget.H>
#include <fapiHwAccess.H>
#include <fapiUtil.H>


extern "C"
{

//------------------------------------------------------------------------------
// HW Communication Functions to be implemented at the platform layer.
//------------------------------------------------------------------------------


/**
 * @brief Platform-level implementation called by GetScom()
 * Hardware procedures writers will not call this function.
 * 
 * 
 * @param[in] i_target Target to operate on
 * @param[in] i_address Scom address to read from
 * @param[out] o_data ecmdDataBufferBase object that holds data read from
 *                    address
 * @return ReturnCode. Zero on success, else platform specified error
 */
fapi::ReturnCode platGetScom(const fapi::Target& i_target,
                             const uint64_t i_address,
                             ecmdDataBufferBase & o_data);


/**
 * @brief Platform-level implementation called by PutScom().
 * Hardware procedures writers will not call this function.
 * 
 * @param[in] i_target Target to operate on
 * @param[in] i_address Scom address to write to
 * @param[in] i_data ecmdDataBufferBase object that holds data to write into
 *                   address
 * @return ReturnCode. Zero on success, else platform specified error
 */
fapi::ReturnCode platPutScom(const fapi::Target& i_target,
                             const uint64_t i_address,
                             ecmdDataBufferBase & i_data);


/**
 * @brief Platform-level implementation called by PutScomUnderMask()
 * Hardware procedures writers will not call this function.
 * 
 * @param[in] i_target Target to operate on
 * @param[in] i_address Scom address to write to
 * @param[in] i_data ecmdDataBufferBase object that holds the data
 * @param[in] i_mask ecmdDataBufferBase object that holds the mask (i_data to
 *                   write)
 * @return ReturnCode. Zero on success, else platform specified error
 */
fapi::ReturnCode platPutScomUnderMask(const fapi::Target& i_target,
                                      const uint64_t i_address,
                                      ecmdDataBufferBase & i_data,
                                      ecmdDataBufferBase & i_mask);


/**
 * @brief Platorm-level implementation called by GetCfamRegister()
 * Hardware procedures writers will not call this function.
 * 
 * @param[in] i_target Target to operate on
 * @param[in] i_address CFAM address to read from
 * @param[out] o_data ecmdDataBufferBase object that holds data read from
 *                    address
 * @return ReturnCode. Zero on success, else platform specified error
 */
fapi::ReturnCode platGetCfamRegister(const fapi::Target& i_target,
                                     const uint32_t i_address,
                                     ecmdDataBufferBase & o_data);


/**
 * @brief Platform-level implementation called by PutCfamRegister()
 * Hardware procedures writers will not call this function.
 * 
 * @param[in] i_target Target to operate on
 * @param[in] i_address CFAM address to write to
 * @param[in] i_data ecmdDataBufferBase object that holds data to write into
 *                   address
 * @return ReturnCode. Zero on success, else platform specified error
 */
fapi::ReturnCode platPutCfamRegister(const fapi::Target& i_target,
                                     const uint32_t i_address,
                                     ecmdDataBufferBase & i_data);


/**
 * @brief Platform-level implementation of ModifyCfamRegister()
 * Hardware procedures writers will not call this function.
 * 
 * @param[in] i_target Target to operate on
 * @param[in] i_address CFAM address to write to
 * @param[in] i_data ecmdDataBufferBase object that holds the modifying data
 * @param[in] i_modifyMode The modify mode (or/and/xor)
 * @return ReturnCode. Zero on success, else platform specified error
 */
fapi::ReturnCode platModifyCfamRegister(const fapi::Target& i_target,
                                  const uint32_t i_address,
                                  ecmdDataBufferBase & i_data,
                                  const fapi::ChipOpModifyMode i_modifyMode);


} // extern "C"

#endif // FAPIPLATHWACCESS_H_
