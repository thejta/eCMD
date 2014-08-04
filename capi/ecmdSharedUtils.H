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
#include <inttypes.h>

#include <ecmdDefines.H>
#include <ecmdDataBuffer.H>
#include <ecmdStructs.H>

//--------------------------------------------------------------------
// ENUM Definitions
//--------------------------------------------------------------------
/**
 @brief Used by ecmdSetTargetDepth
*/
typedef enum {
  ECMD_DEPTH_CAGE = 0,
  ECMD_DEPTH_NODE,
  ECMD_DEPTH_SLOT,
  ECMD_DEPTH_CHIP,
  ECMD_DEPTH_CORE,  // Re-added on 9/27/07 by JTA.  We need both CORE and CHIPUNIT for ecmdSetTargetDepth
  ECMD_DEPTH_CHIPUNIT,
  ECMD_DEPTH_THREAD
} ecmdTargetDepth_t;

//--------------------------------------------------------------------
//  Forward References
//--------------------------------------------------------------------
/* The following two functions are overridden in ecmdClientPerlapi.H to remove io_argc */
#if !defined(ECMD_PERLAPI) && !defined(ECMD_PYAPI)

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

//@}
#endif

/**
 * @brief Breaks the string line into tokens based on all chars in seperators
 * @param line String to tokenize
 * @param seperators String of characters to use as seperators
 * @param tokens Vector of strings that contain all the tokens
*/
void ecmdParseTokens(std::string line, const char* seperators, std::vector<std::string> & tokens);

/** @name Gen Functions */
//@{

/**
 * @brief Turns the data in the buffer into ebcdic text
 * @param i_data Data to convert
 * @param start Bit to start at
 * @param bitLen Number of bits
*/
std::string ecmdGenEbcdic(ecmdDataBuffer &i_data, int start, int bitLen);


/**
 * @brief Default function for converting hex strings to unsigned int arrays
 * @retval First element of the parsed data, or 0xFFFFFFFF if error
 * @param o_numPtr The array that stores the data parsed from the input string
 * @param i_hexChars input string of hex data- alignment stuff handled by Left and Right functions
 * @param startPos
 * @see ecmdGenB32FromHexRight
 * @see ecmdGenB32FromHexLeft
 */
uint32_t ecmdGenB32FromHex(uint32_t * o_numPtr, const char * i_hexChars, int startPos);

/**
 * @brief Convert a string of left-aligned Hex chars into a left-aligned unsigned int array
 * @retval The first element of the parsed string data, or 0xFFFFFFFF if error
 * @param o_numPtr The array that stores the data parsed from the input string
 * @param i_hexChars A string of hex characters
 * @see ecmdGenB32FromHexRight
 * @see ecmdGenB32FromHex
 */
uint32_t ecmdGenB32FromHexLeft(uint32_t * o_numPtr, const char * i_hexChars);

/**
 * @brief Convert a string of right-aligned Hex chars into a left-aligned unsigned int array
 * @retval The first element of the parsed string data, or 0xFFFFFFFF if error
 * @param o_numPtr The array that stores the data parsed from the input string
 * @param i_hexChars A string of hex characters
 * @param i_expectBits The number of defined bits in the o_numPtr array returned
 * @see ecmdGenB32FromHex
 * @see ecmdGenB32FromHexLeft
 */
uint32_t ecmdGenB32FromHexRight(uint32_t * o_numPtr, const char * i_hexChars, int i_expectBits = 0);

//@}


/**
 * @brief Converts strings to unsigned int values. The input format is 0xABCDEF.
 * @param i_str String in hexadecimal notation
 * @date Tue Sep 21 13:22:33 2004
 * @retval uint32_t value of converted input string
 */
uint32_t ecmdHexToUInt32(const char* i_str);

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
uint32_t ecmdHashString32(const char *i_str, uint32_t i_c);

/**
 * @brief Calculates a 64bit hash value for a given string.
 *
 * LICENSE:
 * By Bob Jenkins, 2006.  bob_jenkins@burtleburtle.net.  You may use this
 * code any way you wish, private, educational, or commercial.  It's free.
 * See http://burtleburtle.net/bob/hash/doobs.html
 *     http://burtleburtle.net/bob/c/lookup3.c
 *     modified hashlittle2 function
 *
 * @param i_str String to convert to hash
 * @param i_initval Start value for hash.
 * @retval Hash value
 */
uint64_t ecmdHashString64(const char *i_str, uint64_t i_initval);

/**
 *  @brief Sets State Fields of Chip Target based on depth
 *  @param io_target an ecmdChipTarget struct representing a specific eCmd target
 *  @param i_depth an ecmdTargetDepth_t enum representing depth to be set valid
 *  @retval ECMD_SUCCESS if setting successful
 *  @retval ECMD_INVALID_ARGS if unsuccessful in finding a matching depth
 *  @post State Fields of Chip Target are set to either ECMD_TARGET_FIELD VALID or ECMD_TARGET_FIELD_UNUSED

 TARGET DEPTH  : Input To This Function
 TARGET STATES : Gets Set By This Function

 */
uint32_t ecmdSetTargetDepth(ecmdChipTarget & io_target, ecmdTargetDepth_t i_depth);

/**
 *  @brief Sets Fields of ecmdMemoryEntry_t from the DCard data in the file
 *  @param i_filename file to read the d-card data from
 *  @param o_data list to be updated with the d-card data
 *  @param i_addressOffset Increase the address in the file by the addressOffset passed in
 *  @retval ECMD_SUCCESS if setting successful
 *  @retval ECMD_INVALID_ARGS if unsuccessful in finding a matching depth
 */
uint32_t ecmdReadDcard(const char *i_filename, std::list<ecmdMemoryEntry> &o_data, uint64_t i_addressOffset = 0);

/**
 @brief Returns a formatted string containing the data in the given ecmdChipTarget
 @return String with formatted target data
 @param i_target ecmdChipTarget containing data to format into string
 @param i_displayMode Mode to format data

 TARGET DEPTH  : cage, node, slot, pos, chipUnit/core, thread
 TARGET STATES : Must be Initialized
 */
std::string ecmdWriteTarget(ecmdChipTarget & i_target, ecmdTargetDisplayMode_t i_displayMode = ECMD_DISPLAY_TARGET_PLUGIN_MODE);

/**
 @brief Called by the plugin to set the preferred outputmode of @see ecmdWriteTarget
 @param i_displayMode The display mode returned by the plugin
*/
void ecmdSetTargetDisplayMode(ecmdTargetDisplayMode_t i_displayMode);

/**
 @brief Reads in a target string, returns a filled in target
 @return String with formatted target data
 @param i_targetStr String in a "k0:nall:s0:pu:p0" format to be turned into a target
 @param o_target ecmdChipTarget initialized based upon i_targetStr
 */
uint32_t ecmdReadTarget(std::string i_targetStr, ecmdChipTarget & o_target);

/**
 @brief Parse the chip field and look for the chipUnit
 @param i_chipField The chip field from the command line to be parsed
 @param o_chipType The chip type from the field
 @param o_chipUnitType The chipUnit type from the field (if one was found)
 @param supportsWildCard default to fault, set to true if wildcard is supported.
 @retval ECMD_SUCCESS if successful
 @retval nonzero if failure
*/
uint32_t ecmdParseChipField(std::string i_chipField, std::string &o_chipType, std::string &o_chipUnitType, bool supportsWildCard = false);

#endif /* ecmdSharedUtils_h */
