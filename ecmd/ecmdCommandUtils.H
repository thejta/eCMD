#ifndef ecmdCommandUtils_h
#define ecmdCommandUtils_h
// Copyright **********************************************************
//                                                                      
// File ecmdCommandUtils.H                                               
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 1996                                         
//                                                                      
// The source code for this program is not published or otherwise       
// divested of its trade secrets, irrespective of what has been         
// deposited with the U.S. Copyright Office.                             
//                                                                      
// End Copyright ******************************************************
/* $Header$ */

// Class Description *************************************************
//                                                                      
//  Name:  ecmdCommandUtils
//  Base class: 
//
//  Description: 
//  Usage:
//
// End Class Description *********************************************

/**
 @file ecmdCommandUtils.H
 @brief eCMD Command Utilities
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <ecmdStructs.H>
#include <ecmdDataBuffer.H>

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------

/**
 @brief Fetch the detailed chip data structure for the selected target
 @retval ECMD_SUCCESS if chip data for target is found, non-zero otherwise
 @param i_target ecmdChipTarget that information is requested for
 @param o_data ecmdChipData struct that contains detailed info on chip ec level, etc.
 */
int ecmdGetChipData (ecmdChipTarget & i_target, ecmdChipData & o_data);

/**
 @brief Compares the two data buffers to see if they contain the same data
 @retval 1 if match, 0 if mismatch
 @param i_data The actual data fetched from the chip
 @param i_expected The user-supplied data to match the fetched data to
 */
int ecmdCheckExpected (ecmdDataBuffer & i_data, ecmdDataBuffer & i_expected);


/**
 @brief Write help text file for command
 @param i_command Command to lookup help text for
 @retval ECMD_SUCCESS if successful
 @retval nonzero if failure
*/
int ecmdPrintHelp(const char* i_command);

#endif /* ecmdCommandUtils_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              willsj Initial Creation
//
// End Change Log *****************************************************
