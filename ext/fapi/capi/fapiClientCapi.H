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

#include <ecmdStructs.H>  // needed for HwpInvoker
#include <ecmdDataBufferBase.H>

namespace fapi
{

// Miscellaneous constants
#define MAX_NUM_CORES 32

//------------------------------------------------------------------------------
// HW Communication Functions
//------------------------------------------------------------------------------

extern "C" {
 
/** @name Load/Unload Functions */
//@{
/**
 @brief Initialize the FAPI extention - This call is required by eCMD programs
 @retval FAPI_SUCCESS if successful
 @retval nonzero if unsuccessful
*/
 uint32_t fapiInitExtension();  
//@}

/** @name Misc Functions (NOTE: Available only in this beta release - may not be available when the HWPF is officially released)*/
//@{
/**
 * @brief Run a shared object from within a fapi HW procedures
 * @param i_target  Input target
 * @param i_sharedObjectName  Shared library to open 
 * @param i_sharedObjectEntryPoint  Shared library entry point
 * @return Return code from shared object
 */
ReturnCode RunSo(const Target & i_target, const char * i_sharedObjectName, const char * i_sharedObjectEntryPoint);

/**
 * @brief Run a shared object from within a fapi HW procedures with args
 * @param i_target  Input target
 * @param i_sharedObjectName Shared library to open 
 * @param i_sharedObjectEntryPoint Shared library entry point
 * @param i_sharedObjectArgs List of uint64_t's to pass to shared lib 
 * @return Return code from shared object
 */
ReturnCode RunSoWithArgs(const Target & i_target, const char * i_sharedObjectName, const char * i_sharedObjectEntryPoint,  std::list<uint64_t> & i_sharedObjectArgs);



/**
 * @brief Run a shared object from within a fapi HW procedures with args
 * @param i_target  Input target
 * @param i_sharedObjectName Shared library to open 
 * @param i_sharedObjectEntryPoint Shared library entry point
 * @param i_sharedObjectArgs List of uint64_t's to pass to shared lib 
 * @return Return code from shared object
 */
uint32_t HwpInvoker(ecmdChipTarget & i_target, const std::string & i_sharedObjectName, const std::string & i_sharedObjectEntryPoint, std::list<uint64_t> &i_sharedObjectArgs);
}
//@}


}


#endif
