#ifndef cipClientCapi_h
#define cipClientCapi_h

/* $Header$ */

/**
 * @file cipClientCapi.H
 * @brief Cronus & IP eCMD Extension
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <ecmdReturnCodes.H>
#include <ecmdStructs.H>
#include <ecmdDataBuffer.H>

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------

/* Functions in here are defined as extern C for the following reasons:
 1)  Keeps Function names small by preventing C++ "mangling"
 2)  Allows (C-based) perl interpreter to access these functions

*/
extern "C" {


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
 @brief Set Breakpoint in Processor
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
 @brief Clear Breakpoint from Processor
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
