#ifndef cipClientCapi_h
#define cipClientCapi_h

/* $Header$ */

/**
 * @file cipClientCapi.H
 * @brief Cronus & IP eCMD Extension

 * Extension Owner : Chris Engel
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <ecmdReturnCodes.H>
#include <ecmdStructs.H>
#include <ecmdDataBuffer.H>
#include <cipStructs.H>

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------


/* Functions in here are defined as extern C for the following reasons:
 1)  Keeps Function names small by preventing C++ "mangling"
 2)  Allows (C-based) perl interpreter to access these functions

*/
#ifndef DOCUMENTATION
extern "C" {
#endif

#ifndef ECMD_PERLAPI
/** @name Load/Unload Functions */
//@{

/**
 @brief Initialize eCMD CIP Extension DLL
 @retval ECMD_SUCCESS if successful load
 @retval ECMD_INVALID_DLL_VERSION if Dll version loaded doesn't match client version
 @retval nonzero if unsuccessful
 @post eCMD CIP Extension is initialized and version checked

*/
uint32_t cipInitExtension();


//@}
#endif


/** @name Processor Functions */
//@{

/**
 @brief Start Instructions
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information 
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation

 TARGET DEPTH  : Thread
 TARGET STATES : Unused

*/
uint32_t cipStartInstructions (ecmdChipTarget & i_target);

/**
 @brief Start Instructions on all configured processors
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation

*/
uint32_t cipStartAllInstructions ();

/**
 @brief Stop Instructions
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information 
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation

 TARGET DEPTH  : Thread
 TARGET STATES : Unused

*/
uint32_t cipStopInstructions (ecmdChipTarget & i_target);

/**
 @brief Stop All Instructions
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation

*/
uint32_t cipStopAllInstructions ();

/**
 @brief Step Instructions
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information
 @param i_steps Number of steps to execute
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation

 TARGET DEPTH  : Thread
 TARGET STATES : Unused

*/
uint32_t cipStepInstructions (ecmdChipTarget & i_target, uint32_t i_steps);

/**
 @brief Set a hardware breakpoint in Processor using a real address
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information
 @param i_address Address to set breakpoint at
 @param i_type Type of breakpoint to set
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation

 TARGET DEPTH  : Core
 TARGET STATES : Unused

*/
uint32_t cipSetBreakpoint (ecmdChipTarget & i_target, uint64_t i_address, ecmdBreakpointType_t & i_type);

/**
 @brief Clear a hardware breakpoint from Processor using a real address
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information
 @param i_address Address to clear breakpoint at
 @param i_type Type of breakpoint to set
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation

 TARGET DEPTH  : Core
 TARGET STATES : Unused

*/
uint32_t cipClearBreakpoint (ecmdChipTarget & i_target, uint64_t i_address, ecmdBreakpointType_t & i_type);


/**
 @brief Reads the selected Processor Architected VMX Register (VR) into the data buffer
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARGS Vr number is invalid
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information
 @param i_vrNum Number of vr to read from
 @param o_data DataBuffer object that holds data read from vr

 TARGET DEPTH  : Core
 TARGET STATES : Unused
*/
uint32_t cipGetVr (ecmdChipTarget & i_target, uint32_t i_vrNum, ecmdDataBuffer & o_data);

/**
 @brief Reads the selected Processor Architected VMX Register (VR) into the data buffers
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARGS Vr number is invalid
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information
 @param io_entries List of entries to fetch ecmdIndexEntry.index field must be filled in

 The return value of this function is set to the first non-zero return code found when
  retrieving multiple entries.  The entry that caused the failure in the list will also be marked with
  the same return code. That data and all subsequent entries in the list will not be fetched and the data
  should be considered invalid.

 TARGET DEPTH  : Core
 TARGET STATES : Unused
*/
uint32_t cipGetVrMultiple (ecmdChipTarget & i_target, std::list<ecmdIndexEntry> & io_entries);


/**
 @brief Writes the data buffer into the selected Processor Architected VMX Register (VR)
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARGS Vr number is invalid
 @retval ECMD_SUCCESS if successful
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disaled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information 
 @param i_vrNum Number of vr to write to
 @param i_data DataBuffer object that holds data to write into vr

 TARGET DEPTH  : Core
 TARGET STATES : Unused
*/
uint32_t cipPutVr (ecmdChipTarget & i_target, uint32_t i_vrNum, ecmdDataBuffer & i_data);

/**
 @brief Writes the data buffer into the selected Processor Architected VMX Register (VR)
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARGS Vr number is invalid
 @retval ECMD_SUCCESS if successful
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information 
 @param i_entries List of entries to write all ecmdIndexEntry fields must be filled in


 The return value of this function is set to the first non-zero return code found when
  writing multiple entries.  The function will NOT continue through all subsequent entries.

 TARGET DEPTH  : Core
 TARGET STATES : Unused
*/
uint32_t cipPutVrMultiple (ecmdChipTarget & i_target, std::list<ecmdIndexEntry> & i_entries);


//@}
/* End Processor Functions */


/** @name Memory Functions */
//@{

/**
 @brief Reads System Mainstore through the processor chip using a real address
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position information
 @param i_address Starting address to read from
 @param i_bytes Number of bytes to write
 @param o_memoryData DataBuffer object that holds data read from memory
 @param o_memoryTags 1 bit of tag for every 64 bits of memory data

 NOTE : This function requires that the address be aligned on a 64 bit boundary

 TARGET DEPTH  : Pos
 TARGET STATES : Unused
*/
uint32_t cipGetMemProc (ecmdChipTarget & i_target, uint64_t i_address, uint32_t i_bytes, ecmdDataBuffer & o_memoryData, ecmdDataBuffer & o_memoryTags);


/**
 @brief Writes System Mainstore through the processor chip using a real address
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position information 
 @param i_address Starting address to write to
 @param i_bytes Number of bytes to write
 @param i_memoryData DataBuffer object that holds data to write into memory
 @param i_memoryTags 1 bit of tag for every 64 bits of memory data

 NOTE : This function requires that the address be aligned on a 64 bit boundary


 TARGET DEPTH  : Pos
 TARGET STATES : Unused
*/
uint32_t cipPutMemProc (ecmdChipTarget & i_target, uint64_t i_address, uint32_t i_bytes, ecmdDataBuffer & i_memoryData, ecmdDataBuffer & i_memoryTags);


/**
 @brief Reads System Mainstore through the memory controller using a real address
 @retval ECMD_TARGET_INVALID_TYPE if target is not a memory controller
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position information
 @param i_address Starting address to read from
 @param i_bytes Number of bytes to write
 @param o_memoryData DataBuffer object that holds data read from memory
 @param o_memoryTags 1 bit of tag for every 64 bits of memory data

 NOTE : This function requires that the address be aligned on a 64 bit boundary

 TARGET DEPTH  : Pos
 TARGET STATES : Unused
*/
uint32_t cipGetMemMemCtrl (ecmdChipTarget & i_target, uint64_t i_address, uint32_t i_bytes, ecmdDataBuffer & o_memoryData, ecmdDataBuffer & o_memoryTags);


/**
 @brief Writes System Mainstore through the memory controller using a real address
 @retval ECMD_TARGET_INVALID_TYPE if target is not a memory controller
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position information 
 @param i_address Starting address to write to
 @param i_bytes Number of bytes to write
 @param i_memoryData DataBuffer object that holds data to write into memory
 @param i_memoryTags 1 bit of tag for every 64 bits of memory data

 NOTE : This function requires that the address be aligned on a 64 bit boundary

 TARGET DEPTH  : Pos
 TARGET STATES : Unused

*/
uint32_t cipPutMemMemCtrl (ecmdChipTarget & i_target, uint64_t i_address, uint32_t i_bytes, ecmdDataBuffer & i_memoryData, ecmdDataBuffer & i_memoryTags);

//@}
/* End Memory Functions */

#ifndef DOCUMENTATION
} /* end extern "c" */
#endif

#endif /* cipClientCapi_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
