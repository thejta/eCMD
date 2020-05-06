//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2003,2016 IBM International Business Machines Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
//IBM_PROLOG_END_TAG

#ifndef cipClientCapi_h
#define cipClientCapi_h


/**
 * @file cipClientCapi.H
 * @brief Converged IP Systems eCMD Extension

 * Extension Owner : Matt Light

 @section cip CIP (Converged IP Systems) Extension
 This extension provides interfaces to start/stop processor instructions and breakpoint handling.<br>

 Include files :
 <ul>
 <li> cipClientCapi.H</li>
 <li> cipStructs.H</li>
 <li> Library : libcip.so</li>
 </ul>

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

#if !defined(ECMD_PERLAPI) && !defined(ECMD_PYAPI)
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
 @param i_target Struct that contains chip and cage/node/slot/position/chipUnit information 
 @param i_thread The thread to start instructions on.  0x8 = thread 0, 0x4 = thread 1, 0xC = both threads 
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_INSTRUCTIONS_IN_INVALID_STATE Current state of instructions is invalid to perform the operation

 NOTE: i_thread is only needed for P6 and used with a chipUnit depth target.  All other chips use thread level looping and ignore i_thread.

 TARGET DEPTH  : chipUnit, thread<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipStartInstructions (const ecmdChipTarget & i_target, uint32_t i_thread = 0xFFFFFFFF);

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
 @param i_target Struct that contains chip and cage/node/slot/position/chipUnit information 
 @param i_thread The thread to start instructions on.  0x8 = thread 0, 0x4 = thread 1, 0xC = both threads 
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_INSTRUCTIONS_IN_INVALID_STATE Current state of instructions is invalid to perform the operation

 NOTE: i_thread is only needed for P6 and used with a chipUnit depth target.  All other chips use thread level looping and ignore i_thread.

 TARGET DEPTH  : chipUnit, thread<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipStartInstructionsSreset (const ecmdChipTarget & i_target, uint32_t i_thread = 0xFFFFFFFF);

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
 @param i_target Struct that contains chip and cage/node/slot/position/chipUnit information 
 @param i_thread The thread to stop instructions on.  0x8 = thread 0, 0x4 = thread 1, 0xC = both threads 
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_INSTRUCTIONS_IN_INVALID_STATE Current state of instructions is invalid to perform the operation

 NOTE: i_thread is only needed for P6 and used with a core depth target.  All other chips use thread level looping and ignore i_thread.

 TARGET DEPTH  : chipUnit, thread<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipStopInstructions (const ecmdChipTarget & i_target, uint32_t i_thread = 0xFFFFFFFF);

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
 @param i_target Struct that contains chip and cage/node/slot/position/chipUnit information
 @param i_steps Number of steps to execute
 @param i_thread The thread to step instructions on.  0x8 = thread 0, 0x4 = thread 1, 0xC = both threads 
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_INSTRUCTIONS_IN_INVALID_STATE Current state of instructions is invalid to perform the operation

 NOTE: i_thread is only needed for P6 and used with a core depth target.  All other chips use thread level looping and ignore i_thread.

 TARGET DEPTH  : chipUnit, thread<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipStepInstructions (const ecmdChipTarget & i_target, uint32_t i_steps, uint32_t i_thread = 0xFFFFFFFF);

/**
 @brief Call Special Wakeup
 @param i_target Struct that contains chip and cage/node/slot/position/chipUnit information
 @param i_enable Set and check special wakeup with True. Clear special wakeup with False
 @param i_mode Future use variable
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation

 TARGET DEPTH  : chipUnit<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipSpecialWakeup (const ecmdChipTarget & i_target, bool i_enable, uint32_t i_mode = 0x0);
#endif // CIP_REMOVE_INSTRUCTION_FUNCTIONS
#ifndef CIP_REMOVE_BREAKPOINT_FUNCTIONS
/**
 @brief Set a hardware breakpoint in Processor using a real address
 @param i_target Struct that contains chip and cage/node/slot/position/chipUnit information
 @param i_address Address to set breakpoint at
 @param i_type Type of breakpoint to set
 @param i_thread The thread to set a breakpoint on.  0x8 = thread 0, 0x4 = thread 1, 0xC = both threads 
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation

 NOTE: i_thread is only needed for P6 and used with a core depth target.  All other chips use thread level looping and ignore i_thread.

 TARGET DEPTH  : chipUnit, thread<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipSetBreakpoint (const ecmdChipTarget & i_target, uint64_t i_address, ecmdBreakpointType_t i_type, uint32_t i_thread = 0xFFFFFFFF);

/**
 @brief Clear a hardware breakpoint from Processor using a real address
 @param i_target Struct that contains chip and cage/node/slot/position/chipUnit information
 @param i_address Address to clear breakpoint at
 @param i_type Type of breakpoint to set
 @param i_thread The thread to clear a breakpoint on.  0x8 = thread 0, 0x4 = thread 1, 0xC = both threads 
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation

 NOTE: i_thread is only needed for P6 and used with a core depth target.  All other chips use thread level looping and ignore i_thread.

 TARGET DEPTH  : chipUnit, thread<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipClearBreakpoint (const ecmdChipTarget & i_target, uint64_t i_address, ecmdBreakpointType_t i_type, uint32_t i_thread = 0xFFFFFFFF);
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
 @param i_target Struct that contains chip and cage/node/slot/position/chipUnit/thread information
 @param i_vrNum Number of vr to read from
 @param o_data DataBuffer object that holds data read from vr

 TARGET DEPTH  : chipUnit, thread<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipGetVr (const ecmdChipTarget & i_target, uint32_t i_vrNum, ecmdDataBuffer & o_data);

/**
 @brief Reads the selected Processor Architected VMX Register (VR) into the data buffers
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARGS Vr number is invalid
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/chipUnit/thread information
 @param io_entries List of entries to fetch ecmdIndexEntry.index field must be filled in

 The return value of this function is set to the first non-zero return code found when
  retrieving multiple entries.  The entry that caused the failure in the list will also be marked with
  the same return code. That data and all subsequent entries in the list will not be fetched and the data
  should be considered invalid.<br>

 TARGET DEPTH  : chipUnit, thread<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipGetVrMultiple (const ecmdChipTarget & i_target, std::list<ecmdIndexEntry> & io_entries);


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
 @param i_target Struct that contains chip and cage/node/slot/position/chipUnit/thread information 
 @param i_vrNum Number of vr to write to
 @param i_data DataBuffer object that holds data to write into vr

 TARGET DEPTH  : chipUnit, thread<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipPutVr (const ecmdChipTarget & i_target, uint32_t i_vrNum, const ecmdDataBuffer & i_data);

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
 @param i_target Struct that contains chip and cage/node/slot/position/chipUnit/thread information 
 @param i_entries List of entries to write all ecmdIndexEntry fields must be filled in


 The return value of this function is set to the first non-zero return code found when
  writing multiple entries.  The function will NOT continue through all subsequent entries.<br>

 TARGET DEPTH  : chipUnit, thread<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipPutVrMultiple (const ecmdChipTarget & i_target, std::list<ecmdIndexEntry> & i_entries);
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
uint32_t cipGetMemProc (const ecmdChipTarget & i_target, uint64_t i_address, uint32_t i_bytes, ecmdDataBuffer & o_memoryData, ecmdDataBuffer & o_memoryTags, ecmdDataBuffer & o_memoryEcc, ecmdDataBuffer & o_memoryEccError);


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
uint32_t cipPutMemProc (const ecmdChipTarget & i_target, uint64_t i_address, uint32_t i_bytes, const ecmdDataBuffer & i_memoryData, const ecmdDataBuffer & i_memoryTags, const ecmdDataBuffer & i_memoryErrorInject);

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
uint32_t cipGetMemMemCtrl (const ecmdChipTarget & i_target, uint64_t i_address, uint32_t i_bytes, ecmdDataBuffer & o_memoryData, ecmdDataBuffer & o_memoryTags, ecmdDataBuffer & o_memoryEcc, ecmdDataBuffer & o_memoryEccError, ecmdDataBuffer & o_memorySpareBits);


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
uint32_t cipPutMemMemCtrl (const ecmdChipTarget & i_target, uint64_t i_address, uint32_t i_bytes, const ecmdDataBuffer & i_memoryData, const ecmdDataBuffer & i_memoryTags, const ecmdDataBuffer & i_memoryEcc, const ecmdDataBuffer & i_memorySpareBits, const ecmdDataBuffer & i_memoryErrorInject);
#endif // CIP_REMOVE_MEMORY_FUNCTIONS

#ifndef CIP_REMOVE_VSR_FUNCTIONS
/**
 @brief Reads the selected Processor Architected VSX Register (VSR) into the data buffer
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARGS Vsr number is invalid
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/chipUnit/thread information
 @param i_vsrNum Number of vsr to read from
 @param o_data DataBuffer object that holds data read from vsr

 TARGET DEPTH  : chipUnit, thread<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipGetVsr (const ecmdChipTarget & i_target, uint32_t i_vsrNum, ecmdDataBuffer & o_data);

/**
 @brief Reads the selected Processor Architected VSX Register (VSR) into the data buffers
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARGS Vsr number is invalid
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/chipUnit/thread information
 @param io_entries List of entries to fetch ecmdIndexEntry.index field must be filled in

 The return value of this function is set to the first non-zero return code found when
  retrieving multiple entries.  The entry that caused the failure in the list will also be marked with
  the same return code. That data and all subsequent entries in the list will not be fetched and the data
  should be considered invalid.<br>

 TARGET DEPTH  : chipUnit, thread<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipGetVsrMultiple (const ecmdChipTarget & i_target, std::list<ecmdIndexEntry> & io_entries);


/**
 @brief Writes the data buffer into the selected Processor Architected VSX Register (VSR)
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARGS Vsr number is invalid
 @retval ECMD_SUCCESS if successful
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disaled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/chipUnit/thread information 
 @param i_vsrNum Number of vsr to write to
 @param i_data DataBuffer object that holds data to write into vsr

 TARGET DEPTH  : chipUnit, thread<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipPutVsr (const ecmdChipTarget & i_target, uint32_t i_vsrNum, const ecmdDataBuffer & i_data);

/**
 @brief Writes the data buffer into the selected Processor Architected VSX Register (VSR)
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARGS Vsr number is invalid
 @retval ECMD_SUCCESS if successful
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/chipUnit/thread information 
 @param i_entries List of entries to write all ecmdIndexEntry fields must be filled in


 The return value of this function is set to the first non-zero return code found when
  writing multiple entries.  The function will NOT continue through all subsequent entries.<br>

 TARGET DEPTH  : chipUnit, thread<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipPutVsrMultiple (const ecmdChipTarget & i_target, std::list<ecmdIndexEntry> & i_entries);
#endif // CIP_REMOVE_VSR_FUNCTIONS
//@}
/* End Memory Functions */


//@{
#ifndef CIP_REMOVE_PORE_FUNCTIONS

/**
 @brief Scoms bits from the data buffer into the selected address
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Struct that specifies the target to operate on (see target depth and states below)
 @param i_address Scom address to write to
 @param i_data DataBuffer object that holds data to write into address
 @param i_type Specify type of SCOM write to perform mode - See the CIP_PORE_xxx defines
 @retval ECMD_SUCCESS if successful
 
 @see cipPorePutScomUnderMask

 NOTE : For processor cores/chipUnits, only "core0/chipUnit0" addresses are supported, other chipUnit addresses cause a failure<br>
 TARGET DEPTH  : pos, chipUnit<br>
 TARGET STATES : Unused<br>


*/
uint32_t cipPorePutScom(const ecmdChipTarget & i_target, uint64_t i_address, const ecmdDataBuffer & i_data, uint32_t i_type);

/**
 @brief Perform a read/modify/write scom operation using the mask and data
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Struct that specifies the target to operate on (see target depth and states below)
 @param i_address Scom address to write to
 @param i_data DataBuffer object that holds data to write into address
 @param i_mask DataBuffer of the bits to apply from i_data to the data read from the address
 @param i_type Specify type of SCOM write to perform mode - See the CIP_PORE_xxx defines
 @see cipPorePutScom

 NOTE : For processor cores/chipUnits, only "core0/chipUnit0" addresses are supported, other chipUnit addresses cause a failure<br>
 TARGET DEPTH  : pos, chipUnit<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipPorePutScomUnderMask(const ecmdChipTarget & i_target, uint64_t i_address, const ecmdDataBuffer & i_data, const ecmdDataBuffer & i_mask, uint32_t i_type);
 
/**
 @brief Writes the data buffer into the selected spy
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_INVALID_SPY Spy name is invalid or Spy is an ECC Grouping
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_SPY_IS_EDIAL Spy is an edial have to use putSpyEnum
 @retval nonzero if unsuccessful
 @param i_target Struct that specifies the target to operate on (see target depth and states below)
 @param i_spyName Name of spy to write to
 @param i_data DataBuffer object that holds data to write into spy
 @param i_type Specify type of write to perform mode - See the CIP_PORE_xxx defines

 NOTE : This function is ring cache enabled<br>
 TARGET DEPTH  : pos, chipUnit<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipPorePutSpy(const ecmdChipTarget & i_target, const char * i_spyName, const ecmdDataBuffer & i_data, uint32_t i_type);

/**
 @brief Writes the enum into the selected spy
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_INVALID_SPY Spy name is invalid or Spy is an ECC Grouping
 2retval ECMD_SPY_NOT_ENUMERATED Spy is not enumerated must use putSpy
 @retval ECMD_INVALID_SPY_ENUM if enum value specified is not valid
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that specifies the target to operate on (see target depth and states below)
 @param i_spyName Name of spy to write to
 @param i_enumValue String enum value to load into the spy
 @param i_type Specify type of write to perform mode - See the CIP_PORE_xxx defines

 NOTE : This function is ring cache enabled<br>
 TARGET DEPTH  : pos, chipUnit<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipPorePutSpyEnum(const ecmdChipTarget & i_target, const char * i_spyName, const std::string i_enumValue, uint32_t i_type);
 


/**
 @brief Writes the data buffer into the selected Processor Architected Special Purpose Register (SPR)
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_INVALID_SPR Spr name is invalid
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that specifies the target to operate on (see target depth and states below)
 @param i_sprName Name of spr to write to
 @param i_data DataBuffer object that holds data to write into spr
 @param i_type Specify type of write to perform mode - See the CIP_PORE_xxx defines

 TARGET DEPTH  : chipUnit, thread<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipPorePutSpr (const ecmdChipTarget & i_target, const char * i_sprName, const ecmdDataBuffer & i_data, uint32_t i_type);

/**
 @brief Writes the data buffer into the selected Processor Architected Special Purpose Register (SPR)
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_INVALID_SPR Spr name is invalid
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that specifies the target to operate on (see target depth and states below)
 @param i_entries List of entries to write all ecmdNameEntry fields must be filled in
 @param i_type Specify type of write to perform mode - See the CIP_PORE_xxx defines

 The return value of this function is set to the first non-zero return code found when
  writing multiple entries.  The function will NOT continue through all subsequent entries.<br>

 TARGET DEPTH  : chipUnit, thread<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipPorePutSprMultiple (const ecmdChipTarget & i_target, std::list<ecmdNameEntry> & i_entries, uint32_t i_type);

/**
 @brief Query PORE image information from the DLL
 @param i_target Struct that specifies the target to operate on (see target depth and states below)
 @param o_poreImageInfo Return struct containing PORE image information
 @param i_imageName if == NULL i_target is used to build iamge name 
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure

 TARGET DEPTH  : pos<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipPoreQueryImage(const ecmdChipTarget & i_target, cipPoreImageInfo & o_poreImageInfo, const char * i_imageName = NULL);

/**
 @brief Load the PORE image into mainstore
 @param i_target Struct that specifies the target to operate on (see target depth and states below)
 @param i_baseAddress Mainstore address to load image into
 @param i_imageName if == NULL i_target is used to build iamge name 
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure

 TARGET DEPTH  : pos<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipPoreLoadImage(const ecmdChipTarget & i_target, uint64_t i_baseAddress, const char * i_imageName = NULL);

/**
 @brief Fill specified section of Pore image with return instructions
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Struct that specifies the target to operate on (see target depth and states below)
 @param i_section Specify section of Pore image to fill with return instructions - See the CIP_PORE_xxx defines
 @retval ECMD_SUCCESS if successful

 TARGET DEPTH  : pos, chipUnit<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipPoreFillReturn(const ecmdChipTarget & i_target, uint32_t i_section);

#endif // CIP_REMOVE_PORE_FUNCTIONS
//@}

//@{
#ifndef CIP_REMOVE_RW_FUNCTIONS

/**
 @brief Read entries from cache via RISCWatch
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Struct that specifies the target to operate on (see target depth and states below)
 @param i_startline cache line to start reading from
 @param i_count number of cache lines to read
 @param o_records list of cache line records read
 @param i_mode i-cache = 0, d-cache = 1
 @retval ECMD_SUCCESS if successful

 TARGET DEPTH  : pos, chipUnit<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipRWReadCache(const ecmdChipTarget & i_target, uint32_t i_startline, uint32_t i_count, std::list<cipRWCacheRec> & o_records, uint32_t i_mode);

/**
 @brief Read entries from TLB via RISCWatch
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Struct that specifies the target to operate on (see target depth and states below)
 @param i_startline TLB line to start reading from
 @param i_count number of TLB records to read
 @param o_records list of TLB records read
 @retval ECMD_SUCCESS if successful

*/
uint32_t cipRWReadTLB(const ecmdChipTarget & i_target, uint32_t i_startline, uint32_t i_count, std::list<cipRWTLBRec> & o_records);

/**
 @brief Reads memory associated with chip via RISCWatch or HW Procedure
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/chipunit information
 @param i_address Starting address to read from
 @param i_bytes Number of bytes to write
 @param o_data DataBuffer object that holds data read from memory
 @param i_mode i-mem = 0, d-mem = 1, HW procedure=2 (default)

 TARGET DEPTH  : pos, chipUnit<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipRWReadMem(const ecmdChipTarget & i_target, uint32_t i_address, uint32_t i_bytes, ecmdDataBuffer & o_data, uint32_t i_mode = 2);

/**
 @brief Writes memory associated with chip via RISCWatch or HW Procedure
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/chipunit information
 @param i_address Starting address to read from
 @param i_bytes Number of bytes to write
 @param i_data DataBuffer object that holds data to write to memory
 @param i_mode i-mem = 0, d-mem = 1, HW procedure=2 (default)

 TARGET DEPTH  : pos, chipUnit<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipRWWriteMem(const ecmdChipTarget & i_target, uint32_t i_address, uint32_t i_bytes, const ecmdDataBuffer & i_data, uint32_t i_mode = 2);

/**
 @brief Reads the selected Processor Architected General Purpose Register (GPR) into the data buffer
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_GPR Dcr number is invalid
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that specifies the target to operate on (see target depth and states below)
 @param i_dcrNum Number of dcr to read from
 @param o_data DataBuffer object that holds data read from dcr

 TARGET DEPTH  : chipUnit, thread<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipRWGetDcr (const ecmdChipTarget & i_target, uint32_t i_dcrNum, ecmdDataBuffer & o_data);

/**
 @brief Reads the selected Processor Architected General Purpose Register (GPR) into the data buffer
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_GPR Dcr number is invalid
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that specifies the target to operate on (see target depth and states below)
 @param io_entries List of entries to fetch ecmdIndexEntry.index field must be filled in

 The return value of this function is set to the first non-zero return code found when
  retrieving multiple entries.  The entry that caused the failure in the list will also be marked with
  the same return code. That data and all subsequent entries in the list will not be fetched and the data
  should be considered invalid.<br>

 TARGET DEPTH  : chipUnit, thread<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipRWGetDcrMultiple (const ecmdChipTarget & i_target, std::list<ecmdIndexEntry> & io_entries);


/**
 @brief Writes the data buffer into the selected Processor Architected General Purpose Register (GPR)
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_GPR Dcr number is invalid
 @retval ECMD_SUCCESS if successful
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that specifies the target to operate on (see target depth and states below)
 @param i_dcrNum Number of dcr to write to
 @param i_data DataBuffer object that holds data to write into dcr

 TARGET DEPTH  : chipUnit, thread<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipRWPutDcr (const ecmdChipTarget & i_target, uint32_t i_dcrNum, const ecmdDataBuffer & i_data);

/**
 @brief Writes the data buffer into the selected Processor Architected General Purpose Register (GPR)
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_GPR Dcr number is invalid
 @retval ECMD_SUCCESS if successful
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that specifies the target to operate on (see target depth and states below)
 @param i_entries List of entries to write all ecmdIndexEntry fields must be filled in


 The return value of this function is set to the first non-zero return code found when
  writing multiple entries.  The function will NOT continue through all subsequent entries.<br>

 TARGET DEPTH  : chipUnit, thread<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipRWPutDcrMultiple (const ecmdChipTarget & i_target, std::list<ecmdIndexEntry> & i_entries);

/**
 @brief Checks the processor status
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Struct that specifies the target to operate on (see target depth and states below)
 @param o_status Status of the targeted processor

 The return value of this function is set to the first non-zero return code found when
  writing multiple entries.  The function will NOT continue through all subsequent entries.<br>

 TARGET DEPTH  : chipUnit<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipRWProcStatus (const ecmdChipTarget & i_target, cipRWProcStatus_t & o_status);

#endif // CIP_REMOVE_RW_FUNCTIONS
//@}

//@{
#ifndef CIP_REMOVE_MBOX_FUNCTIONS
/**
 @brief Send a mailbox message to plugin/platform specific mailbox mechanism(s)
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Struct that contains cage information 
 @param i_msg_type The message type to send

*/
uint32_t cipSendMboxMsg(const ecmdChipTarget & i_target, mbox_message_t i_msg_type = CIP_MBOX_MSG_UNKNOWN);
#endif // CIP_REMOVE_MBOX_FUNCTIONS
//@}


//@{
#ifndef CIP_REMOVE_PMC_VOLTAGE_FUNCTIONS
/**
 @brief Reads voltage values from processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/chipunit information
 @param i_vdd_mode Mode to return vdd voltage value. CIP_PMC_VOLTAGE_MODE_IGNORE, CIP_PMC_VOLTAGE_MODE_VID, or CIP_PMC_VOLTAGE_MODE_VOLT
 @param o_vdd vdd voltage value returned when i_vdd_mode not set to CIP_PMC_VOLTAGE_MODE_IGNORE
 @param i_vcs_mode Mode to return vcs voltage value. CIP_PMC_VOLTAGE_MODE_IGNORE, CIP_PMC_VOLTAGE_MODE_VID, or CIP_PMC_VOLTAGE_MODE_VOLT
 @param o_vcs vcs voltage value returned when i_vcs_mode not set to CIP_PMC_VOLTAGE_MODE_IGNORE

 TARGET DEPTH  : pos<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipGetPmcVoltage(const ecmdChipTarget & i_target, uint32_t i_vdd_mode, uint32_t & o_vdd, uint32_t i_vcs_mode, int32_t & o_vcs);

/**
 @brief Writes voltage values to processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/chipunit information
 @param i_vdd_mode Mode to write vdd voltage value. CIP_PMC_VOLTAGE_MODE_IGNORE, CIP_PMC_VOLTAGE_MODE_VID, or CIP_PMC_VOLTAGE_MODE_VOLT
 @param i_vdd vdd voltage value to write when i_vdd_mode not set to CIP_PMC_VOLTAGE_MODE_IGNORE
 @param i_vcs_mode Mode to write vcs voltage value. CIP_PMC_VOLTAGE_MODE_IGNORE, CIP_PMC_VOLTAGE_MODE_VID, or CIP_PMC_VOLTAGE_MODE_VOLT
 @param i_vcs vcs voltage value to write when i_vcs_mode not set to CIP_PMC_VOLTAGE_MODE_IGNORE
 @param i_step_size when step size is not equal to zero, will increment voltage change in step size specified.

 NOTE : When CIP_PMC_VOLTAGE_MODE_VID is selected, the input should be limited from 0x0 to 0xFF in size.<br>
 TARGET DEPTH  : pos<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipPutPmcVoltage(const ecmdChipTarget & i_target, uint32_t i_vdd_mode, uint32_t i_vdd, uint32_t i_vcs_mode, int32_t i_vcs, uint8_t i_step_size = 0);
#endif // CIP_REMOVE_PMC_VOLTAGE_FUNCTIONS
//@}

/**
 @brief Get the System Info structure
 @param o_sysInfo Buffer containing the SysInfo struct
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
*/
uint32_t cipGetSysInfo(cipSysInfo_t & o_sysInfo);

/** @name Memory Functions */
//@{
#ifndef CIP_REMOVE_MEMORY_FUNCTIONS
/**
 @brief Reads System Mainstore through the processor chip using an effective address
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
 @param i_xlateVars Scruct with numerous translation variables
 @param o_memoryData DataBuffer object that holds data read from memory
 @param o_memoryTags 1 Tag bit for every 64 bits of memory data
 @param o_memoryEcc 8 ECC bits for every 64 bits of memory data
 @param o_memoryEccError 1 ECC Error bit for every 64 bits of memory data
 @param o_realAddress Caculated Real Address

 NOTE : This function requires that the address be aligned on an 8-byte boundary<br>

 TARGET DEPTH  : Thread<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipGetMemProcVariousAddrType(const ecmdChipTarget & i_target, ecmdDataBuffer i_address, uint32_t i_bytes, cipXlateVariables i_xlateVars, ecmdDataBuffer & o_memoryData, ecmdDataBuffer & o_memoryTags, ecmdDataBuffer & o_memoryEcc, ecmdDataBuffer & o_memoryEccError, ecmdDataBuffer & o_realAddress);

/**
 @brief Writes System Mainstore through the processor chip using an effective address
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
 @param i_xlateVars Scruct with numerous translation variables
 @param i_memoryData DataBuffer object that holds data read from memory
 @param io_memoryTags 1 Tag bit for every 64 bits of memory data (If this has length of zero, the user wants the HW to generate this info; otherwise, use their their values.)
 @param o_realAddress Caculated Real Address

 NOTE : This function requires that the address be aligned on an 8-byte boundary<br>

 TARGET DEPTH  : Thread<br>
 TARGET STATES : Unused<br>
*/
uint32_t cipPutMemProcVariousAddrType(const ecmdChipTarget & i_target, ecmdDataBuffer i_address, uint32_t i_bytes, cipXlateVariables i_xlateVars, const ecmdDataBuffer & i_memoryData, ecmdDataBuffer & io_memoryTags, ecmdDataBuffer & o_realAddress);
#endif // CIP_REMOVE_MEMORY_FUNCTIONS
//@}
/* End Memory Functions */

/** @name Breakpoint Functions */
//@{
#ifndef CIP_REMOVE_BREAKPOINT_FUNCTIONS
/**
 @brief Set a software breakpoint in Processor 
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information
 @param i_address Address to set breakpoint at
 @param i_xlateVars Scruct with numerous translation variables
 @param o_brkptTableEntries List of cipBrkptTableEntry structs
 @param o_virtualAddress Caculated Virtual Address
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RETRY_WITH_VIRTUAL_ADDR Requests that the user retries operation with returned Virtual Address
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful

 TARGET DEPTH  : Thread<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipSetSoftwareBreakpoint (const ecmdChipTarget & i_target, const ecmdDataBuffer & i_address, const cipXlateVariables i_xlateVars, std::list<cipBrkptTableEntry> & o_brkptTableEntries, ecmdDataBuffer & o_virtualAddress);

/**
 @brief Clear a software breakpoint in Processor 
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information
 @param i_address Address to clear breakpoint at (see NOTE below)
 @param i_xlateVars Scruct with numerous translation variables
 @param o_brkptTableEntries List of cipBrkptTableEntry structs
 @param o_virtualAddress Caculated Virtual Address
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RETRY_WITH_VIRTUAL_ADDR Requests that the user retries operation with returned Virtual Address
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 NOTE : If i_address is an empty ecmdDataBuffer (bitLength=0), ALL Breakpoints on the system will be cleared

 TARGET DEPTH  : Thread<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipClearSoftwareBreakpoint (const ecmdChipTarget & i_target, const ecmdDataBuffer & i_address, const cipXlateVariables i_xlateVars, std::list<cipBrkptTableEntry> & o_brkptTableEntries, ecmdDataBuffer & o_virtualAddress);

/**
 @brief Get software breakpoint table
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information
 @param i_address Address to get breakpoint at
 @param i_xlateVars Scruct with numerous translation variables
 @param o_brkptTableEntries List of cipBrkptTableEntry structs
 @param o_virtualAddress Caculated Virtual Address
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RETRY_WITH_VIRTUAL_ADDR Requests that the user retries operation with returned Virtual Address
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful

 TARGET DEPTH  : Thread<br>
 TARGET STATES : Unused<br>

*/
uint32_t cipGetSoftwareBreakpoint (const ecmdChipTarget & i_target, const ecmdDataBuffer & i_address, const cipXlateVariables i_xlateVars, std::list<cipBrkptTableEntry> & o_brkptTableEntries, ecmdDataBuffer & o_virtualAddress);

/**
 @brief Function to wait for a breakpoint event to occur
 @param i_NumSecs Number of seconds to wait for event.
 @param o_events List of  cipSoftwareEvent_t structs 
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
*/
uint32_t cipWaitAsyncEvent(uint32_t i_NumSecs, std::list<cipSoftwareEvent_t> & o_events);
#endif // CIP_REMOVE_BREAKPOINT_FUNCTIONS
//@}
/* End Breakpoint Functions */

#ifndef DOCUMENTATION
} /* end extern "c" */
#endif

#endif /* cipClientCapi_h */
