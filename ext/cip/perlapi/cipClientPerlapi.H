#ifndef cipClientPerlapi_h
#define cipClientPerlapi_h

/* $Header$ */

/**
 * @file cipClientPerlapi.H
 * @brief Cronus & IP eCMD Perlapi Extension

 * Extension Owner : Chris Engel
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------

//for Linux compiles
#ifdef list
#undef list
#endif


#include <ecmdReturnCodes.H>
#include <ecmdStructs.H>
#include <ecmdDataBuffer.H>
#include <cipStructs.H>

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------
extern "C" int ecmdPerlInterfaceErrorCheck (int ec);
extern "C" int ecmdQuerySafeMode();


/* Functions in here are defined as extern C for the following reasons:
 1)  Keeps Function names small by preventing C++ "mangling"
 2)  Allows (C-based) perl interpreter to access these functions

*/
class cipClientPerlapi
{
public:

  /** @name Load/Unload Functions */
  //@{
  /**
   @brief eCMD Perl Module Constructor
   */
  cipClientPerlapi ();
  /**
   @brief eCMD Perl Module Destructor
   */
  ~cipClientPerlapi ();


  /**
   @brief Initialize eCMD CIP Extension DLL
   @retval ECMD_SUCCESS if successful load
   @retval ECMD_INVALID_DLL_VERSION if Dll version loaded doesn't match client version
   @retval nonzero if unsuccessful
   @post eCMD CIP Extension is initialized and version checked

   */
  int cipInitExtension();


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
  int cipStartInstructions (const char* i_target);

  /**
   @brief Start Instructions on all configured processors
   @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
   @retval ECMD_SUCCESS if successful
   @retval nonzero if unsuccessful
   @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation

   */
  int cipStartAllInstructions ();

  /**
   @brief Stop Instructions
   @param i_target Struct that contains chip and cage/node/slot/position/core/thread information 
   @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
   @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
   @retval ECMD_SUCCESS if successful
   @retval nonzero if unsuccessful
   @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation


   */
  int cipStopInstructions (const char* i_target);

  /**
   @brief Stop All Instructions
   @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
   @retval ECMD_SUCCESS if successful
   @retval nonzero if unsuccessful
   @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation

   */
  int cipStopAllInstructions ();

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
  int cipStepInstructions (const char* i_target, int i_steps);

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
  //int cipSetBreakpoint (ecmdChipTarget & i_target, uint64_t i_address, ecmdBreakpointType_t & i_type);

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
  //int cipClearBreakpoint (ecmdChipTarget & i_target, uint64_t i_address, ecmdBreakpointType_t & i_type);


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
   @param rNum Number of vr to read from
   @param o_data DataBuffer object that holds data read from vr

   */
  int cipGetVr (const char* i_target, int i_vrNum, char** o_data);


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

   */
  int cipPutVr (const char* i_target, int i_vrNum, const char* i_data);



  //@}
  /* End Processor Functions */


};

#endif /* cipClientCapi_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
