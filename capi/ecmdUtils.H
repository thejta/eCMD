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

//@}


/** @name Command Line Parsing Functions */
//@{

/**
 * @brief Iterates over argv, removing null pointers and decrementing argc
 * @retval None
 * @param io_argc Pointer to number of elements in io_argv array
 * @param io_argv Array of strings passed in from command line
 * @see ecmdParseOption
 * @see ecmdParseOptionWithArgs

 - Utility function for ecmdParseOption and ecmdParseOptionWithArgs
 */
void ecmdRemoveNullPointers (int * io_argc, char ** io_argv[]);

/**
 * @brief Iterates over argv, looking for given option string, removes it if found
 * @retval 1 if option found, 0 otherwise
 * @param io_argc Pointer to number of elements in io_argv array
 * @param io_argv Array of strings passed in from command line
 * @param i_option Option to look for
 * @see ecmdRemoveNullPointers
 * @see ecmdParseOptionWithArgs

 */
int ecmdParseOption (int * io_argc, char ** io_argv[], const char * i_option);

/**
 * @brief Iterates over argv, looking for given option string, removes it if found
 * @retval Value of option arg if found, NULL otherwise
 * @param io_argc Pointer to number of elements in io_argv array
 * @param io_argv Array of strings passed in from command line
 * @param i_option Option to look for
 * @see ecmdRemoveNullPointers
 * @see ecmdParseOptionWithArgs

 */
char * ecmdParseOptionWithArgs(int * io_argc, char ** io_argv[], const char * i_option);
//@}

#endif
