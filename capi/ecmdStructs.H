#ifndef ecmdStructs_H
#define ecmdStructs_H
// Copyright **********************************************************
//                                                                      
// File ecmdStructs.H                                               
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 2003                                         
//                                                                      
// The source code for this program is not published or otherwise       
// divested of its trade secrets, irrespective of what has been         
// deposited with the U.S. Copyright Office.                             
//                                                                      
// End Copyright ******************************************************
/**
  @file ecmdStructs.H
  @brief All the Structures required for the eCMD Capi
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <stdint.h>

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//  Defines                                                
//--------------------------------------------------------------------

#define ECMD_CAPI_VERSION ".1"

#define ECMD_MAX_CHIP_TYPE_LENGTH 15  ///< @see ecmdChipTarget



// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              willsj Initial Creation
//
// End Change Log *****************************************************

/**
 @brief Structure used to designate which cec object you would like the function to operate on

*/
struct ecmdChipTarget {

  char chipType[ECMD_MAX_CHIP_TYPE_LENGTH];  ///< name of chip to access

  uint32_t cage;  ///< cage that contains node with chip
  uint32_t node;  ///< node that contains chip
  uint32_t pos;   ///< position of chip within node
  uint32_t core;  ///< which core on chip to access, if chip is multi-core
  uint32_t thread; ///< which thread on chip to access, if chip is multi-threaded

  uint8_t cageLoopValid;  ///< Tell the calling function that this function operates at a cage level
  uint8_t nodeLoopValid;  ///< Tell the calling function that this function operates at a node level
  uint8_t posLoopValid;   ///< Tell the calling function that this function operates at a pos level
  uint8_t coreLoopValid;  ///< Tell the calling function that this function operates at a core level
  uint8_t threadLoopValid; ///< Tell the calling function that this function operates at a thread level
  uint8_t loopValidChecked;  ///< Flag that indicates that we have signaled to the user level how to loop 
};


#endif
