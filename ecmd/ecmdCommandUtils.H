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
 @brief Compares the two data buffers to see if they contain the same data
 @retval 1 if match, 0 if mismatch
 @param i_data The actual data fetched from the chip
 @param i_expected The user-supplied data to match the fetched data to
 */
uint32_t ecmdCheckExpected (ecmdDataBuffer & i_data, ecmdDataBuffer & i_expected);

/**
 @brief Applies the buffer i_newData to the io_data buffer based upon the specified i_modifier for length of i_newData
 @retval ECMD_SUCCESS on success
 @retval non-zero on failure
 @param io_data Data buffer to modify
 @param i_newData Data to insert into io_data based on modifier
 @param i_startbit Startbit of io_data to begin modification
 @param i_modifier Type of data insertion to do 'and', 'or', 'insert'
*/
uint32_t ecmdApplyDataModifier (ecmdDataBuffer & io_data, ecmdDataBuffer & i_newData, int i_startbit, std::string i_modifier);


/**
 @brief Write help text file for command
 @param i_command Command to lookup help text for
 @retval ECMD_SUCCESS if successful
 @retval nonzero if failure
*/
uint32_t ecmdPrintHelp(const char* i_command);

/** @name Gen Functions */
//@{
/**
 * @brief Default function for converting hex strings to unsigned int arrays
 * @retval First element of the parsed data, or 0xFFFFFFFF if error
 * @param o_numPtr The array that stores the data parsed from the input string
 * @param i_hexChars input string of hex data- alignment stuff handled by Left and Right functions
 * @param startPos
 * @see ecmdGenB32FromHexRight
 * @see ecmdGenB32FromHexLeft
 */
uint32_t ecmdGenB32FromHex (uint32_t * o_numPtr, const char * i_hexChars, int startPos);

/**
 * @brief Convert a string of left-aligned Hex chars into a left-aligned unsigned int array
 * @retval The first element of the parsed string data, or 0xFFFFFFFF if error
 * @param o_numPtr The array that stores the data parsed from the input string
 * @param i_hexChars A string of hex characters
 * @see ecmdGenB32FromHexRight
 * @see ecmdGenB32FromHex
 */
uint32_t ecmdGenB32FromHexLeft (uint32_t * o_numPtr, const char * i_hexChars);

/**
 * @brief Convert a string of right-aligned Hex chars into a left-aligned unsigned int array
 * @retval The first element of the parsed string data, or 0xFFFFFFFF if error
 * @param o_numPtr The array that stores the data parsed from the input string
 * @param i_hexChars A string of hex characters
 * @param i_expectBits The number of defined bits in the o_numPtr array returned
 * @see ecmdGenB32FromHex
 * @see ecmdGenB32FromHexLeft
 */
uint32_t ecmdGenB32FromHexRight (uint32_t * o_numPtr, const char * i_hexChars, int i_expectBits = 0);

//@}


/**
 @brief Returns true if all chars of str are decimal numbers
 @retval true if all chars are decimal
 @retval false if any non-decimal character is detected
 @param str Input string to perform comparison against
*/
bool ecmdIsAllDecimal(const char* str);

/**
 @brief Returns true if all chars of str are hex numbers
 @retval true if all chars are hex
 @retval false if any non-hex character is detected
 @param str Input string to perform comparison against
*/
bool ecmdIsAllHex(const char* str);



#endif /* ecmdCommandUtils_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              willsj Initial Creation
//
// End Change Log *****************************************************
