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

#define ECMD_MAX_CHIP_TYPE_LENGTH 15

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              willsj Initial Creation
//
// End Change Log *****************************************************

/**
 @brief Commentary?

*/
struct ecmdChipTarget {

  char chipType[ECMD_MAX_CHIP_TYPE_LENGTH];  // name of chip to access

  uint32_t cage;  // cage that contains node with chip
  uint32_t node;  // node that contains chip
  uint32_t pos;   // position of chip within node
  uint32_t core;  // which core on chip to access, if chip is multi-core
  uint32_t thread; // which thread on chip to access, if chip is multi-threaded

  uint8_t cageInvalid;  // flag to indicate whether selected cage is configured in the system
  uint8_t nodeInvalid;  // flag to indicate whether selected node is configured in the system
  uint8_t posInvalid;   // flag to indicate whether selected position is configured in the system
  uint8_t coreInvalid;  // flag to indicate whether the selected core is configured on the selected chip in the system
  uint8_t threadInvalid; // flag to indicate whether the selected thread is active on the selected chip in the system

};


#endif
