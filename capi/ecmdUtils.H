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
#include <string>
#include <vector>

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

/* Functions in here are defined as extern C for the following reasons:
 1)  Keeps Function names small by preventing C++ "mangling"
 2)  Allows (C-based) perl interpreter to access these functions

*/
extern "C" {

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
 * @param i_expectBits The number of defined bits in the o_numPtr array returned
 * @see ecmdGenB32FromHex
 * @see ecmdGenB32FromHexLeft
 */
uint32_t ecmdGenB32FromHexRight (uint32_t * o_numPtr, const char * i_hexChars, int i_expectBits = 0);

//@}


/** @name Command Line Parsing Functions */
//@{

/**
 * @brief Iterates over argv, looking for given option string, removes it if found
 * @retval 1 if option found, 0 otherwise
 * @param io_argc Pointer to number of elements in io_argv array
 * @param io_argv Array of strings passed in from command line
 * @param i_option Option to look for
 * @see ecmdParseOptionWithArgs

 */
int ecmdParseOption (int * io_argc, char ** io_argv[], const char * i_option);

/**
 * @brief Iterates over argv, looking for given option string, removes it if found
 * @retval Value of option arg if found, NULL otherwise
 * @param io_argc Pointer to number of elements in io_argv array
 * @param io_argv Array of strings passed in from command line
 * @param i_option Option to look for
 * @see ecmdParseOptionWithArgs

 */
char * ecmdParseOptionWithArgs(int * io_argc, char ** io_argv[], const char * i_option);


void ecmdParseTokens (std::string & line, std::vector<std::string> & tokens);

//@}


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
 @param i_data ecmdDataBuffer where data to format is stored
 @param i_format Flag that tells how to parse the data into a string, e.g., "b" = binary, "x" = hex left
 @param address A base address value that can be used in formating certain data- i.e., data from memory
 */
std::string ecmdWriteDataFormatted (ecmdDataBuffer & i_data, std::string & i_format, int address = 0);


} /* Extern C */

#endif
