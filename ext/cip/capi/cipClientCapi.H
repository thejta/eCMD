#ifndef cipClientCapi_h
#define cipClientCapi_h

/* $Header$ */

/**
 * @file cipClientCapi.H
 * @brief Cronus & IP eCMD Extension

 * Extension Owner : Jason Albert/Michael Baiocchi
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <ecmdDefines.H>
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
#ifndef CIP_REMOVE_INSTRUCTION_FUNCTIONS
/**
 @brief Start Instructions
 @param i_target Struct that contains chip and cage/node/slot/position/core information 
 @param i_thread The thread to start instructions on.  0x8 = thread 0, 0x4 = thread 1, 0xC = both threads 
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_INSTRUCTIONS_IN_INVALID_STATE Current state of instructions is invalid to perform the operation

 NOTE: i_thread is only needed for P6 and used with a core depth target.  All other chips use thread level looping and ignore i_thread.

 TARGET DEPTH  : chipUnit/core, thread<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipStartInstructions (ecmdChipTarget & i_target, uint32_t i_thread = 0xFFFFFFFF);

/**
 @brief Start Instructions on all configured processors
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_INSTRUCTIONS_IN_INVALID_STATE Current state of instructions is invalid to perform the operation

*/
uint32_t cipStartAllInstructions ();

/**
 @brief Start Instructions using an S-Reset
 @param i_target Struct that contains chip and cage/node/slot/position/core information 
 @param i_thread The thread to start instructions on.  0x8 = thread 0, 0x4 = thread 1, 0xC = both threads 
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_INSTRUCTIONS_IN_INVALID_STATE Current state of instructions is invalid to perform the operation

 NOTE: i_thread is only needed for P6 and used with a core depth target.  All other chips use thread level looping and ignore i_thread.

 TARGET DEPTH  : chipUnit/core, thread<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipStartInstructionsSreset (ecmdChipTarget & i_target, uint32_t i_thread = 0xFFFFFFFF);

/**
 @brief Start Instructions on all configured processors using an S-Reset
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_INSTRUCTIONS_IN_INVALID_STATE Current state of instructions is invalid to perform the operation

*/
uint32_t cipStartAllInstructionsSreset ();

/**
 @brief Stop Instructions
 @param i_target Struct that contains chip and cage/node/slot/position/core information 
 @param i_thread The thread to stop instructions on.  0x8 = thread 0, 0x4 = thread 1, 0xC = both threads 
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_INSTRUCTIONS_IN_INVALID_STATE Current state of instructions is invalid to perform the operation

 NOTE: i_thread is only needed for P6 and used with a core depth target.  All other chips use thread level looping and ignore i_thread.

 TARGET DEPTH  : chipUnit/core, thread<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipStopInstructions (ecmdChipTarget & i_target, uint32_t i_thread = 0xFFFFFFFF);

/**
 @brief Stop All Instructions
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_INSTRUCTIONS_IN_INVALID_STATE Current state of instructions is invalid to perform the operation

*/
uint32_t cipStopAllInstructions ();

/**
 @brief Step Instructions
 @param i_target Struct that contains chip and cage/node/slot/position/core information
 @param i_steps Number of steps to execute
 @param i_thread The thread to step instructions on.  0x8 = thread 0, 0x4 = thread 1, 0xC = both threads 
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_INSTRUCTIONS_IN_INVALID_STATE Current state of instructions is invalid to perform the operation

 NOTE: i_thread is only needed for P6 and used with a core depth target.  All other chips use thread level looping and ignore i_thread.

 TARGET DEPTH  : chipUnit/core, thread<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipStepInstructions (ecmdChipTarget & i_target, uint32_t i_steps, uint32_t i_thread = 0xFFFFFFFF);
#endif // CIP_REMOVE_INSTRUCTION_FUNCTIONS
#ifndef CIP_REMOVE_BREAKPOINT_FUNCTIONS
/**
 @brief Set a hardware breakpoint in Processor using a real address
 @param i_target Struct that contains chip and cage/node/slot/position/core information
 @param i_address Address to set breakpoint at
 @param i_type Type of breakpoint to set
 @param i_thread The thread to set a breakpoint on.  0x8 = thread 0, 0x4 = thread 1, 0xC = both threads 
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation

 NOTE: i_thread is only needed for P6 and used with a core depth target.  All other chips use thread level looping and ignore i_thread.

 TARGET DEPTH  : chipUnit/core, thread<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipSetBreakpoint (ecmdChipTarget & i_target, uint64_t i_address, ecmdBreakpointType_t & i_type, uint32_t i_thread = 0xFFFFFFFF);

/**
 @brief Clear a hardware breakpoint from Processor using a real address
 @param i_target Struct that contains chip and cage/node/slot/position/core information
 @param i_address Address to clear breakpoint at
 @param i_type Type of breakpoint to set
 @param i_thread The thread to clear a breakpoint on.  0x8 = thread 0, 0x4 = thread 1, 0xC = both threads 
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation

 NOTE: i_thread is only needed for P6 and used with a core depth target.  All other chips use thread level looping and ignore i_thread.

 TARGET DEPTH  : chipUnit/core, thread<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipClearBreakpoint (ecmdChipTarget & i_target, uint64_t i_address, ecmdBreakpointType_t & i_type, uint32_t i_thread = 0xFFFFFFFF);
#endif // CIP_REMOVE_BREAKPOINT_FUNCTIONS
#ifndef CIP_REMOVE_VR_FUNCTIONS
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

 TARGET DEPTH  : chipUnit/core, thread<br>
 TARGET STATES : Unused<br>
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
  should be considered invalid.<br>

 TARGET DEPTH  : chipUnit/core, thread<br>
 TARGET STATES : Unused<br>
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

 TARGET DEPTH  : chipUnit/core, thread<br>
 TARGET STATES : Unused<br>
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
  writing multiple entries.  The function will NOT continue through all subsequent entries.<br>

 TARGET DEPTH  : chipUnit/core, thread<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipPutVrMultiple (ecmdChipTarget & i_target, std::list<ecmdIndexEntry> & i_entries);
#endif // CIP_REMOVE_VR_FUNCTIONS


//@}
/* End Processor Functions */


/** @name Memory Functions */
//@{
#ifndef CIP_REMOVE_MEMORY_FUNCTIONS
/**
 @brief Reads System Mainstore through the processor chip using a real address
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_INVALID_MEMORY_ADDRESS Memory Address was not on a 8-byte boundary
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position information
 @param i_address Starting address to read from
 @param i_bytes Number of bytes to write
 @param o_memoryData DataBuffer object that holds data read from memory
 @param o_memoryTags 1 Tag bit for every 64 bits of memory data
 @param o_memoryEcc 8 ECC bits for every 64 bits of memory data
 @param o_memoryEccError 1 ECC Error bit for every 64 bits of memory data

 NOTE : This function requires that the address be aligned on a 64 bit boundary<br>

 TARGET DEPTH  : Pos<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipGetMemProc (ecmdChipTarget & i_target, uint64_t i_address, uint32_t i_bytes, ecmdDataBuffer & o_memoryData, ecmdDataBuffer & o_memoryTags, ecmdDataBuffer & o_memoryEcc, ecmdDataBuffer & o_memoryEccError);


/**
 @brief Writes System Mainstore through the processor chip using a real address
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_INVALID_MEMORY_ADDRESS Memory Address was not on a 8-byte boundary
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position information 
 @param i_address Starting address to write to
 @param i_bytes Number of bytes to write
 @param i_memoryData DataBuffer object that holds data to write into memory
 @param i_memoryTags 1 Tag bit for every 64 bits of memory data (If this has length of zero, the user wants the HW to generate this info; otherwise, use their their values. 
 @param i_memoryErrorInject 2 Error Inject bits for every 64 bits of memory data (If this has a length of zero, no error inject;  otherwise 0b00, 0x11 no error inject, 0b01 single-bit error inject, 0b10 double-bit error inject.)

 NOTE : This function requires that the address be aligned on a 64 bit boundary<br>

 TARGET DEPTH  : Pos<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipPutMemProc (ecmdChipTarget & i_target, uint64_t i_address, uint32_t i_bytes, ecmdDataBuffer & i_memoryData, ecmdDataBuffer & i_memoryTags, ecmdDataBuffer & i_memoryErrorInject);

/**
 @brief Reads System Mainstore through the memory controller using a real address
 @retval ECMD_TARGET_INVALID_TYPE if target is not a memory controller
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_INVALID_MEMORY_ADDRESS Memory Address was not on a 8-byte boundary
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position information
 @param i_address Starting address to read from
 @param i_bytes Number of bytes to write
 @param o_memoryData DataBuffer object that holds data read from memory
 @param o_memoryTags 2 Tag bits for every 32 bytes of memory data
 @param o_memoryEcc 24 ECC bits for every 32 bytes of memory data
 @param o_memoryEccError 1 ECC Error bit for every 32 bytes of memory data
 @param o_memorySpareBits  2 Spare bits for every 32 bytes of memory data

 NOTE : This function requires that the address be aligned on a 32-byte boundary<br>

 TARGET DEPTH  : Cage<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipGetMemMemCtrl (ecmdChipTarget & i_target, uint64_t i_address, uint32_t i_bytes, ecmdDataBuffer & o_memoryData, ecmdDataBuffer & o_memoryTags, ecmdDataBuffer & o_memoryEcc, ecmdDataBuffer & o_memoryEccError, ecmdDataBuffer & o_memorySpareBits);


/**
 @brief Writes System Mainstore through the memory controller using a real address
 @retval ECMD_TARGET_INVALID_TYPE if target is not a memory controller
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_INVALID_MEMORY_ADDRESS Memory Address was not on a 8-byte boundary
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position information 
 @param i_address Starting address to write to
 @param i_bytes Number of bytes to write
 @param i_memoryData DataBuffer object that holds data to write into memory
 @param i_memoryTags [Optional] 2 Tag bits for every 32 bytes of memory data
 @param i_memoryEcc 24 ECC bits for every 32 bytes of memory data
 @param i_memorySpareBits [Optional] 2 Spare bits for every 32 bytes of memory data
 @param i_memoryErrorInject 2 Error Inject bits for every 64 bits of memory data (If this has a length of zero, no error inject;  otherwise 0b00, 0x11 no error inject, 0b01 single-bit error inject, 0b10 double-bit error inject.)


 NOTE : This function requires that the address be aligned on a 32-byte boundary<br>
 NOTE : If ecmdDataBuffers for i_memoryTags and i_memorySpareBIts have a length of zero, the user wants the HW to generate this info.  If these ecmdDataBuffers have a length, the the user wants their values to be used.<br>

 TARGET DEPTH  : Cage<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipPutMemMemCtrl (ecmdChipTarget & i_target, uint64_t i_address, uint32_t i_bytes, ecmdDataBuffer & i_memoryData, ecmdDataBuffer & i_memoryTags, ecmdDataBuffer & i_memoryEcc, ecmdDataBuffer & i_memorySpareBits, ecmdDataBuffer & i_memoryErrorInject);
#endif // CIP_REMOVE_MEMORY_FUNCTIONS
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
