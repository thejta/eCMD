#ifndef ecmdSharedUtils_h
#define ecmdSharedUtils_h
// Copyright **********************************************************
//
// File ecmdSharedUtils.H
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
//  Name:  ecmdSharedUtils
//  Base class:
//
//  Description:
//  Usage:
//
// End Class Description *********************************************

/* $Header$ */

/**
 * @file ecmdSharedUtils.H
 * @brief Useful functions for use throughout the ecmd C API and Plugin
 *
 */


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <string>
#include <vector>
// AK: added for hash routine  10/07/2004
#include <inttypes.h>

#include <ecmdDataBuffer.H>

//--------------------------------------------------------------------
//  Forward References
//--------------------------------------------------------------------

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
bool ecmdParseOption (int * io_argc, char ** io_argv[], const char * i_option);

/**
 * @brief Iterates over argv, looking for given option string, removes it if found
 * @retval Value of option arg if found, NULL otherwise
 * @param io_argc Pointer to number of elements in io_argv array
 * @param io_argv Array of strings passed in from command line
 * @param i_option Option to look for
 * @see ecmdParseOptionWithArgs

 */
char * ecmdParseOptionWithArgs(int * io_argc, char ** io_argv[], const char * i_option);


/**
 * @brief Breaks the string line into tokens based on all chars in seperators
 * @param line String to tokenize
 * @param seperators String of characters to use as seperators
 * @param tokens Vector of strings that contain all the tokens
*/
void ecmdParseTokens (std::string line, const char* seperators, std::vector<std::string> & tokens);

/**
 * @brief Turns the data in the buffer into ebcdic text
 * @param i_data Data to convert
 * @param start Bit to start at
 * @param bitLen Number of bits
*/
std::string ecmdGenEbcdic (ecmdDataBuffer &i_data, int start, int bitLen);

//@}

/**
 * @brief Converts strings to unsigned int values. The input format is 0xABCDEF.
 * @param i_str String in hexadecimal notation
 * @date Tue Sep 21 13:22:33 2004
 * @retval uint32_t value of converted input string
 */
extern "C" uint32_t ecmdHexToUInt32(const char* i_str);

/**
 * @brief Calculates a 32bit hash value for a given string.
 *
 * LICENSE:
 * By Bob Jenkins, 1996.  bob_jenkins@burtleburtle.net.  You may use this
 * code any way you wish, private, educational, or commercial.  It's free.
 * See http://burtleburtle.net/bob/hash/doobs.html
 *
 * @param i_str String to convert to hash
 * @param i_c Start value for hash.
 * @retval Hash value
 */
extern "C" uint32_t ecmdHashString32(const char *i_str, uint32_t i_c);


#endif /* ecmdSharedUtils_h */

// Change Log *********************************************************
//
//  Flag Reason   Vers Date     Coder  Description
//  ---- -------- ---- -------- -----  -------------------------------
//                              cengel Initial Creation
//                              kloss  added hash functions: hashString32, hexToUInt32
//
// End Change Log *****************************************************
