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
extern "C" {

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


*/
uint32_t cipClearBreakpoint (ecmdChipTarget & i_target, uint64_t i_address, ecmdBreakpointType_t & i_type);


/**
 @brief Reads the selected Processor Architected VMX Register (VPR) into the data buffer
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARGS Vpr number is invalid
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information
 @param i_vprNum Number of vpr to read from
 @param o_data DataBuffer object that holds data read from vpr

*/
uint32_t cipGetVpr (ecmdChipTarget & i_target, uint32_t i_vprNum, ecmdDataBuffer & o_data);

/**
 @brief Reads the selected Processor Architected VMX Register (VPR) into the data buffers
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARGS Vpr number is invalid
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information
 @param io_entries List of entries to fetch ecmdIndexEntry.index field must be filled in

 The return value of this function is set to the first non-zero return code found when
  retrieving multiple entries.  The function will still continue through all entries requested.
  You must walk through the list returned to find out which entry caused the failure.

*/
uint32_t cipGetVprMultiple (ecmdChipTarget & i_target, std::list<ecmdIndexEntry> & io_entries);


/**
 @brief Writes the data buffer into the selected Processor Architected VMX Register (VPR)
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARGS Vpr number is invalid
 @retval ECMD_SUCCESS if successful
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disaled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information 
 @param i_vprNum Number of vpr to write to
 @param i_data DataBuffer object that holds data to write into vpr

*/
uint32_t cipPutVpr (ecmdChipTarget & i_target, uint32_t i_vprNum, ecmdDataBuffer & i_data);

/**
 @brief Writes the data buffer into the selected Processor Architected VMX Register (VPR)
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARGS Vpr number is invalid
 @retval ECMD_SUCCESS if successful
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information 
 @param i_entries List of entries to write all ecmdIndexEntry fields must be filled in


 The return value of this function is set to the first non-zero return code found when
  retrieving multiple entries.  The function will still continue through all entries requested.
  You must walk through the list returned to find out which entry caused the failure.

*/
uint32_t cipPutVprMultiple (ecmdChipTarget & i_target, std::list<ecmdIndexEntry> & i_entries);


//@}
/* End Processor Functions */

} /* end extern "c" */

#endif /* cipClientCapi_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
