#ifndef ecmdUtils_H
#define ecmdUtils_H

// Copyright **********************************************************
//                                                                      
// File ecmdDataBuffer.H                                               
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
 * @file ecmdUtils.H
 * @brief Useful functions for use throughout the ecmd C API
 *
 */


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <inttypes.h>

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
 * @brief Convert a string of left-aligned Hex chars into an unsigned int array
 * @retval The first element of the parsed string data, or 0xFFFFFFFF if error
 * @param o_numPtr The array that stores the data parsed from the input string
 * @param i_hexChars A string of hex characters
 * @see ecmdGenB32FromHexRight
 * @see ecmdGenB32FromHex
 */
uint32_t ecmdGenB32FromHexLeft (uint32_t * o_numPtr, const char * i_hexChars);

/**
 * @brief Convert a string of right-aligned Hex chars into an unsigned int array
 * @retval The first element of the parsed string data, or 0xFFFFFFFF if error
 * @param o_numPtr The array that stores the data parsed from the input string
 * @param i_hexChars A string of hex characters
 * @param expectBits The number of defined bits in the o_numPtr array returned
 * @see ecmdGenB32FromHex
 * @see ecmdGenB32FromHexLeft
 */
uint32_t ecmdGenB32FromHexRight (uint32_t * o_numPtr, const char * i_hexChars, int expectBits = 0);


#endif
