#ifndef ecmdUtils_H
#define ecmdUtils_H

// Copyright **********************************************************
//                                                                      
// File ecmdUtils.H                                               
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

/* $Header$ */

/**
 * @file ecmdUtils.H
 * @brief Useful functions for use throughout the ecmd C API
 *
 */


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <inttypes.h>
#include <string>
#include <vector>

#include <ecmdClientCapi.H>

//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------

//--------------------------------------------------------------------
// Macros
//--------------------------------------------------------------------

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  User Types
//----------------------------------------------------------------------


/**
 @brief Initializes data structures and code to loop over configured and selected elements of the system
 @param io_target Initial ecmdChipTarget that may contain information used in building the struct to loop over
 @param i_looptype Specify type of all, all chips in system or all chips selected by user
 @param io_state Used internally by ConfigLooper to keep track of state, unique instance must be passed into each loop and must be passed to ecmdConfigLooperNext
 @retval ECMD_SUCCESS if initialization succeeded, error code if otherwise
 @see ecmdConfigLooperNext
 */
uint32_t ecmdConfigLooperInit (ecmdChipTarget & io_target, ecmdConfigLoopType_t i_looptype, ecmdLooperData& io_state);

/**
 @brief Loops over configured and selected elements of the system, updating target to point to them
 @param io_target ecmdChipTarget that contains info about next target to process
 @param io_state Used internally to keep track of state, must be passed from output of ecmdConfigLooperInit
 @retval 1 if io_target is valid, 0 if it is not
 @see ecmdConfigLooperInit
 */
uint32_t ecmdConfigLooperNext (ecmdChipTarget & io_target, ecmdLooperData& io_state);


/**
 @brief Reads data from data string into data buffer based on a format type
 @retval ECMD_SUCCESS if data is well-formatted, non-zero otherwise
 @param o_data ecmdDataBuffer where data from data string is placed.
 @param i_dataStr string of characters containing data
 @param i_format Flag that tells how to parse the data string, e.g., "b" = binary, "x" = hex left
 @param i_expectedLength If length of data is known before hand , should be passed is necessary for right aligned data that is not byte aligned lengths
 */
uint32_t ecmdReadDataFormatted (ecmdDataBuffer & o_data, const char * i_dataStr, std::string & i_format, int i_expectedLength = 0);

/**
 @brief Formats data from data buffer into a string according to format flag and returns the string
 @return String of formatted data
 @param i_data ecmdDataBuffer where data to format is stored
 @param i_format Flag that tells how to parse the data into a string, e.g., "b" = binary, "x" = hex left
 @param address A base address value that can be used in formating certain data- i.e., data from memory
 */
std::string ecmdWriteDataFormatted (ecmdDataBuffer & i_data, std::string & i_format, int address = 0);

/**
 @brief Print the bits header used in the output formats
 @param i_initCharOffset char offset on screen to start printing
 @param i_blockSize Binary block size (ie. column char size)
 @param i_numCols Number of columns to display
 @param i_maxBitWidth Maximum number of bits to display - this is actual data valid so we don't display more columns then we need
 @return String of formatted data
*/
std::string ecmdBitsHeader (int i_initCharOffset, int i_blockSize, int i_numCols, int i_maxBitWidth);


/**
 @brief Returns a formatted string containing the data in the given ecmdChipTarget
 @return String with formatted target data
 @param i_target ecmdChipTarget containing data to format into string
 */
std::string ecmdWriteTarget (ecmdChipTarget & i_target);


/**
 @brief Fetch the detailed chip data structure for the selected target
 @retval ECMD_SUCCESS if chip data for target is found, non-zero otherwise
 @param i_target ecmdChipTarget that information is requested for
 @param o_data ecmdChipData struct that contains detailed info on chip ec level, etc.
 */
uint32_t ecmdGetChipData (ecmdChipTarget & i_target, ecmdChipData & o_data);

/**
 @brief Function calls ecmdQueryDllInfo and displays the output to stdout
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
uint32_t ecmdDisplayDllInfo();

/**
 @brief Print the parameters pass to a given function.
 @retval Void return
 @param enum efppInOut Helps printed output for IN or OUT message
 @param char* fprototypeStr cut and paste of function prototype line.
 @param  <all others> parameters associated with given calling function.
 */
void ecmdFunctionParmPrinter(efppInOut_t inOut, char *fprototypeStr, ...);



#endif
