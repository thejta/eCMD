#ifndef sedcScomdefParser_h
#define sedcScomdefParser_h
/* $Header$ */
// Copyright **********************************************************
//                                                                      
// File sedcScomdefParser.H                                               
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

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                     09/23/03 albertj  Initial Creation
//
// End Change Log *****************************************************

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <sedcScomdefClasses.H>

/**
 * @brief Parse single scomdef entry 
 * @brief Takes an ifstream file pointing to the proper position to start reading from
 * @param returnScomdef Returns a class containing the scomdef entry read from the file
 * @param scomdefFile File to read for the scomdef entry
 * @param errorMsgs Any error messages to go with a array that was marked invalid
 * @param runtimeFlags Possible directives on how to parse
 */
void sedcScomdefParser(sedcScomdefEntry &returnScom, std::ifstream &scomdefFile, std::vector<std::string> &errMsgs, uint32_t runtimeFlags);

/**
 * @brief Carves up a line and returns it into a vector by the seperator, it special handles {} as well
 * @brief If you are looking to just carve up any old line, use ecmdParseTokens, this is special for the array format
 * @param line The line to tokenize
 * @param seperators What to search on
 * @param tokens A vector of tokens
 * @retval void
 */
void sedcCreateScomdefTokens(std::string line, const char* seperators, sedcFileLine &myLine);

#endif /* sedcScomdefParser_h */
