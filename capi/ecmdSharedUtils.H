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

//@}



#endif /* ecmdSharedUtils_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
