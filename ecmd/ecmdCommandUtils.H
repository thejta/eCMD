#ifndef ecmdCommandUtils_h
#define ecmdCommandUtils_h
/* $Header$ */
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
 @brief Compares the two data buffers to see if they contain the same data
 @retval 1 if match, 0 if mismatch
 @param i_data The actual data fetched from the chip
 @param i_expected The user-supplied data to match the fetched data to
 @param o_mismatchBit Return the first bit that mismatched in the data, set to ECMD_UNSET to not calculate when calling, will be set to ECMD_UNSET if wasn't a bit mismatch but was a length difference
 */
uint32_t ecmdCheckExpected (ecmdDataBuffer & i_data, ecmdDataBuffer & i_expected, uint32_t & o_mismatchBit);

/**
 @brief Applies the buffer i_newData to the io_data buffer based upon the specified i_modifier for length of i_newData
 @retval ECMD_SUCCESS on success
 @retval non-zero on failure
 @param io_data Data buffer to modify
 @param i_newData Data to insert into io_data based on modifier
 @param i_startbit Startbit of io_data to begin modification
 @param i_modifier Type of data insertion to do 'and', 'or', 'insert'
*/
uint32_t ecmdApplyDataModifier (ecmdDataBuffer & io_data, ecmdDataBuffer & i_newData, uint32_t i_startbit, std::string i_modifier);


/**
 @brief Write help text file for command
 @param i_command Command to lookup help text for
 @retval ECMD_SUCCESS if successful
 @retval nonzero if failure
*/
uint32_t ecmdPrintHelp(const char* i_command);


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

/**
 @brief Returns the string define name for specified returnCode (ie "ECMD_SPY_GROUP_MISMATCH")
 @param i_returnCode Return code to lookup
 @retval String name of return code ("UNDEFINED" if unknown)
*/
std::string ecmdParseReturnCode(uint32_t i_returnCode);

/**
 @brief Parse the chip field and look for the chipUnit
 @param i_chipField The chip field from the command line to be parsed
 @param o_chipType The chip type from the field
 @param o_chipUnitType The chipUnit type from the field (if one was found)
 @retval ECMD_SUCCESS if successful
 @retval nonzero if failure
*/
uint32_t ecmdParseChipField(std::string i_chipField, std::string &o_chipType, std::string &o_chipUnitType);

/**
 @brief Returns a list of commands that were read from stdin
 @param o_commands
 @retval ECMD_SUCCESS when EOF is reached (list may still contain valid commands
 @retval list size
*/
uint32_t ecmdParseStdinCommands(std::vector< std::string > & o_commands);

/**
 @brief Parses the specific target field list specified by the user and updates the target States
 @param io_argc Pointer to number of elements in io_argv array
 @param io_argv Array of strings passed in from command line
 @param i_targetField Specifies target field-cage,node,slot etc
 @param io_target Target to be updated based on the user input
 @param o_targetFieldType Specifies One versus Many. E.g. Whether one pos was specified or more than 1
 @param o_targetFieldList Update this string with the list of targets specified
 @retval ECMD_SUCCESS or ECMD_INVALID_ARGS
*/
uint32_t ecmdParseTargetFields(int *io_argc, char ** io_argv[], char *i_targetField, ecmdChipTarget &io_target, uint8_t
&o_targetFieldType, std::string &o_targetFieldList);

/**
 @brief Checks if the string is a valid target
 @param i_str String to be checked for decimal characters
 @retval Returns true if all chars of str are decimal numbers
*/
bool isTargetStringValid(std::string i_str);

/**
 @brief Convert the target string into a uint32_t list
 @param i_userArgs Input target String to be parsed
 @param o_targetList Output list to be updated with the targets
*/
void getTargetList (std::string i_userArgs, std::list<uint32_t> &targetList);

#endif /* ecmdCommandUtils_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              willsj Initial Creation
//
// End Change Log *****************************************************
