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
 @brief Initializes data structures and code to loop over configured and selected elements of the system
 @param io_target Initial ecmdChipTarget that may contain information used in building the struct to loop over
 @retval ECMD_SUCCESS if initialization succeeded, error code if otherwise
 @see ecmdConfigLooperNext
 */
int ecmdConfigLooperInit (ecmdChipTarget & io_target);

/**
 @brief Loops over configured and selected elements of the system, updating target to point to them
 @param io_target ecmdChipTarget that contains info about next target to process
 @retval 1 if io_target is valid, 0 if it is not
 @see ecmdConfigLooperInit
 */
int ecmdConfigLooperNext (ecmdChipTarget & io_target);

/**
 @brief Fetch the detailed chip data structure for the selected target
 @retval ECMD_SUCCESS if chip data for target is found, non-zero otherwise
 @param i_target ecmdChipTarget that information is requested for
 @param o_data ecmdChipData struct that contains detailed info on chip ec level, etc.
 */
int ecmdGetChipData (ecmdChipTarget & i_target, ecmdChipData & o_data);

/**
 @brief Returns a formatted string containing the data in the given ecmdChipTarget
 @retval String with formatted target data
 @param i_target ecmdChipTarget containing data to format into string
 */
std::string ecmdWriteTarget (ecmdChipTarget & i_target);

/**
 @brief Reads data from data string into data buffer based on a format type
 @retval ECMD_SUCCESS if data is well-formatted, non-zero otherwise
 @param o_data ecmdDataBuffer where data from data string is placed.
 @param i_dataStr string of characters containing data
 @param i_format Flag that tells how to parse the data string, e.g., "b" = binary, "x" = hex left
 */
int ecmdReadDataFormatted (ecmdDataBuffer & o_data, const char * i_dataStr, std::string & i_format);

/**
 @brief Formats data from data buffer into a string according to format flag and returns the string
 @retval String of formatted data
 @param o_data ecmdDataBuffer where data to format is stored
 @param i_format Flag that tells how to parse the data into a string, e.g., "b" = binary, "x" = hex left
 @param address A base address value that can be used in formating certain data- i.e., data from memory
 */
std::string ecmdWriteDataFormatted (ecmdDataBuffer & i_data, std::string & i_format, int address = 0);

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
