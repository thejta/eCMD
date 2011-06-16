/**
 * @file fapiHwAccess.H
 * @brief Hardware Procedure Framework (fapi) eCMD Extension

 * Extension Owner : John Farrugia
*/

#ifndef fapiHwAccess_H 
#define fapiHwAccess_H

#include <stdint.h>
#include <list>
#include <fapiTarget.H>
//JFDEBUG no longer needed#include <fapiChipletType.H>
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


/** @name Processor Functions */
//@{

/**
 @brief Scans the ring from the selected chip into the data buffer
 @retval FAPI_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Opaque struct that specifies the target to operate on
 @param i_address Address of ring to read from
 @param o_data DataBuffer object that holds data read from ring
*/
ReturnCode GetRing(const Target& i_target, const uint32_t i_address, ecmdDataBufferBase & o_data);

/**
 @brief Scans ring from the data buffer into the selected chip in the selected ring
 @retval FAPI_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Opaque struct that specifies the target to operate on
 @param i_address Address of ring to write to
 @param i_data DataBuffer object that holds data to write into ring
*/
ReturnCode PutRing(const Target& i_target, const uint32_t i_address, ecmdDataBufferBase & i_data);

/**
 @brief Scoms bits from the selected address into the data buffer
 @retval FAPI_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Opaque struct that specifies the target to operate on
 @param i_address Scom address to read from
 @param o_data DataBuffer object that holds data read from address

 NOTE : For processor cores/chipUnits, only "core0/chipUnit0" addresses are supported, other core/chipUnit addresses cause a failure
*/
ReturnCode GetScom(const Target& i_target, const uint64_t i_address, ecmdDataBufferBase & o_data);

/**
 @brief Scoms bits from the data buffer into the selected address
 @retval FAPI_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Opaque struct that specifies the target to operate on
 @param i_address Scom address to write to
 @param i_data DataBuffer object that holds data to write into address

 NOTE : For processor cores/chipUnits, only "core0/chipUnit0" addresses are supported, other core/chipUnit addresses cause a failure

*/
ReturnCode PutScom(const Target& i_target, const uint32_t i_address, ecmdDataBufferBase & i_data);

/**
 @brief Perform a read/modify/write scom operation using the mask and data
 @retval FAPI_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Opaque struct that specifies the target to operate on
 @param i_address Scom address to write to
 @param i_data DataBuffer object that holds data to write into address
 @param i_mask DataBuffer of the bits to apply from i_data to the data read from the address

 NOTE : For processor cores/chipUnits, only "core0/chipUnit0" addresses are supported, other core/chipUnit addresses cause a failure<br>
*/
ReturnCode PutScomUnderMask(const Target& i_target, /* JFDEBUG const */ uint64_t i_address, ecmdDataBufferBase & i_data, const ecmdDataBufferBase & i_mask);


/**
 @brief Read data from the selected CFAM register address into the data buffer
 @retval FAPI_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Opaque struct that specifies the target to operate on
 @param i_address CFAM address to read from
 @param o_data DataBuffer object that holds data read from address
*/
ReturnCode GetCfamRegister(const Target& i_target, const uint32_t i_address, ecmdDataBufferBase & o_data);

/**
 @brief Write data into the selected CFAM register address
 @retval FAPI_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Opaque struct that specifies the target to operate on
 @param i_address CFAM address to write to
 @param i_data DataBuffer object that holds data to write into address
*/
ReturnCode PutCfamRegister(const Target& i_target, const uint32_t i_address, ecmdDataBufferBase & i_data);


/** @name Looping Functions */
//@{


/**
 @brief Get the functional chiplets for a specific target
 @param i_target Opaque struct that specifies the target to operate on
 @param i_chiplet Specify the chiplet type to fetch data for (see fapiChipletType.H) //FIXME these chiplets names are hardcoded. Fix this! -farrugia
 @param o_entries List of Target& stucts, one entry per functional chiplet
 @retval FAPI_SUCCESS if successful
 @retval nonzero on failure
*/
ReturnCode GetFunctionalChiplets(const Target& i_target, const TargetType & i_chiplet, std::list<Target> & o_entries);


/**
 @brief Get the existing chiplets for a specific target
 @param i_target Opaque struct that specifies the target to operate on
 @param i_chiplet Specify the chiplet type to fetch data for (see fapiChipletType.H) //FIXME these chiplets names are hardcoded. Fix this! -farrugia
 @param o_entries List of fapiHandle_i stucts, one entry per existing chiplet
 @retval FAPI_SUCCESS if successful
 @retval nonzero on failure
*/
ReturnCode GetExistingChiplets(const Target& i_target, const TargetType & i_chiplet, std::list<Target> &o_entries);

/**
 * @brief Get list of cores associated with the specified input target.
 * @param i_target  Input target
 * @param o_count  receives count of number of targets returned
 * @param o_targets  Pointer to array of Target objects to contain the handles to the associated cores
 * @return Return code from operation
 */
ReturnCode GetCoreTargets(const Target& i_target, uint32_t& o_count, Target* o_targets);

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


/*** -------------------------------------- ***/
/*** The following is doxygen documentation ***/
/*** -------------------------------------- ***/

/**
 * @mainpage


@section int Hardware Procedure Framework (fapi) eCMD Extension BETA 

Click on any of the tabs above to peruse the fapi extension.  Start with 'Files' then 'fapi.H'

Contact John Farrugia with comments, questions, suggestions

<br>
Last Approved: 
<hr>

*/

#endif
