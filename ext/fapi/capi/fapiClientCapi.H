/**
 * @file fapiClientCapi.H
 * @brief Hardware Procedure Framework (fapi) eCMD Extension

 * Extension Owner : John Farrugia
*/

#ifndef fapiClientCapi_H 
#define fapiClientCapi_H

#include <stdint.h>
#include <list>
#include <fapiTarget.H>
#include <fapiReturnCode.H>
#include <fapiStructs.H>  

#include <ecmdStructs.H>  
#include <ecmdDataBufferBase.H>


//------------------------------------------------------------------------------
// HW Communication Functions
//------------------------------------------------------------------------------

extern "C" {

 
/** @name Load/Unload Functions (NOTE: Available only in eCMD HWPF FAPI extention implementation, not FW)*/
//@{
/**
 @brief Initialize the FAPI extention - This call is required by eCMD programs
 @retval FAPI_SUCCESS if successful
 @retval nonzero if unsuccessful
*/
 uint32_t fapiInitExtension();  
//@}

/** @name Misc Functions (NOTE: Available only in eCMD HWPF FAPI extention implementation, not FW)*/
//@{
/**
 * @brief Run a shared object from within a fapi HW procedures with args
 * @param i_target  Input target
 * @param i_sharedObjectName Shared library to open 
 * @param i_sharedObjectEntryPoint Shared library entry point
 * @param i_sharedObjectArgs List of uint64_t's to pass to shared lib 
 * @return Return code from shared object
 */
uint32_t fapiHwpInvoker(ecmdChipTarget & i_target, const std::string & i_sharedObjectName, const std::string & i_sharedObjectEntryPoint, std::list<uint64_t> &i_sharedObjectArgs);

/**
 @brief Query the location of a specific HWP
 @param i_fileType Enum that specifies which type of file you are looking for (see fapiStructs.H)
 @param i_fileName Name of file you are looking for 
 @param o_fileLocation Return string with full path and filename to location 
 @param i_version String to specify a version (e.g., 1.1, 710.4, default).  "default" is default
 @retval ECMD_SUCCESS if successful
 @retval ECMD_UNKNOWN_FILE if unable to find requested file
 @retval nonzero if unsuccessful

*/
uint32_t fapiQueryFileLocation(fapi::FileType_t i_fileType, std::string & i_fileName, std::string & o_fileLocation, std::string i_version = "default");

/**
 @brief Retrieve the value of an Attribute Setting
 @param i_target Struct that specifies the target to operate on
 @param i_id ID of attribute to query
 @param o_data Structure containing the valid data
 @retval ECMD_INVALID_CONFIG_NAME Name specified is not valid
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful

 <b>
 NOTE: </b>
 <b>
   <ul>
   <li>Hardware procedures (HWP) should not call this API directly as it's only supported by eCMD, not FW. </li>
   <li>HWPs should call the FAPI_ATTR_GET macro to read an attribute (see fapiAttributeService.H)</li>
  </ul>
</b>

*/
uint32_t fapiGetAttribute(const fapi::Target & i_target, const uint32_t i_id, fapi::AttributeData & o_data);


/**
 @brief Set the value of an Attribute 
 @param i_target Struct that specifies the target to operate on
 @param i_id ID of attribute to query
 @param i_data Structure containing the valid data
 @retval ECMD_INVALID_CONFIG_NAME Name specified is not valid
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful

 <b>
 NOTE </b>
 <b>
   <ul>
   <li>Hardware procedures (HWP) should not call this API directly as it's only supported by eCMD, not FW. </li>
   <li>HWPs should call the FAPI_ATTR_SET macro to set an attribute (see fapiAttributeService.H) </li>
   </ul>
 </b>
*/
uint32_t fapiSetAttribute(const fapi::Target & i_target, const uint32_t i_id, fapi::AttributeData & i_data);


//@}

} //extern c

#endif
