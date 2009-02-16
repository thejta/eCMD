#ifndef sedcSpyParser_H
#define sedcSpyParser_H
/* $Header$ */
// Copyright **********************************************************
//
// File sedcSpyParser.H
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

// Class Description *************************************************
//
//  Name:  sedcSpyParser
//  Base class:
//
//  Description:
//  Usage:
//
// End Class Description *********************************************

// Change Log *********************************************************
//
//  Flag Reason   Vers Date     Coder    Description
//  ---- -------- ---- -------- -------- ------------------------------
//                              albertj  Initial Creation
//
// End Change Log *****************************************************

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <sedcSpyContainer.H>
#include <inttypes.h>

/**
 * @brief Traffic cop to call the different individual parser types.
 * @brief Takes an ifstream file pointing to the proper position to start reading from
 * @param inputFile File to read for the spy to verify
 * @param errorMsgs Any error messages to go with a spy that was marked invalid
 * @param runtimeFlags Possible directives on how to parse
 * @retval retrunDC Returns a class containing the data type that could be read
 */
sedcSpyContainer sedcSpyParser(std::ifstream &spyFile, std::vector<std::string> &errMsgs, uint32_t &runtimeFlags);

/**
 * @brief Parse single idial/edial/alias entry for proper syntax - does not do group bit length verification, etc.
 * @brief Takes an ifstream file pointing to the proper position to start reading from
 * @param inputFile File to read for the spy to verify
 * @param errorMsgs Any error messages to go with a spy that was marked invalid
 * @param runtimeFlags Possible directives on how to parse
 * @retval aeiEntry Returns a class contain the spy read from the file
 */
sedcAEIEntry sedcAEIParser(std::ifstream &spyFile, std::vector<std::string> &errMsgs, uint32_t &runtimeFlags);

/**
 * @brief Parse single synonym entry for proper syntax
 * @brief Takes an ifstream file pointing to the proper position to start reading from
 * @param inputFile File to read for the spy to verify
 * @param errorMsgs Any error messages to go with a spy that was marked invalid
 * @param runtimeFlags Possible directives on how to parse
 * @retval synonymEntry Returns a class containing the synonym read from the file
 */
sedcSynonymEntry sedcSynonymParser(std::ifstream &spyFile, std::vector<std::string> &errMsgs, uint32_t &runtimeFlags);

/**
 * @brief Parse single eplatches entry for proper syntax
 * @brief Takes an ifstream file pointing to the proper position to start reading from
 * @param inputFile File to read for the spy to verify
 * @param errorMsgs Any error messages to go with a spy that was marked invalid
 * @param runtimeFlags Possible directives on how to parse
 * @retval sedcEplatchesEntry Returns a class containing the eplatches read from the file
 */
sedcEplatchesEntry sedcEplatchesParser(std::ifstream &spyFile, std::vector<std::string> &errMsgs, uint32_t &runtimeFlags);

/**
 * @brief Parse single eccfunc entry for proper syntax
 * @brief Takes an ifstream file pointing to the proper position to start reading from
 * @param inputFile File to read for the spy to verify
 * @param errorMsgs Any error messages to go with a spy that was marked invalid
 * @param runtimeFlags Possible directives on how to parse
 * @retval sedcEccfuncEntry Returns a class containing the eccfunc read from the file
 */
sedcEccfuncEntry sedcEccfuncParser(std::ifstream &spyFile, std::vector<std::string> &errMsgs, uint32_t &runtimeFlags);

/**
 * @brief Carves up an enum line we pulled from the spy file
 * @param myLine The tokenized line from the spy file to be parsed
 * @param errorMsgs Any error messages to go with a spy that was marked invalid
 * @param runtimeFlags Possible directives on how to parse
 * @retval sedcSpyEnum Returns a class with the enum and the bits.
 */
sedcAEIEnum sedcParseEnumLine(sedcFileLine &myLine, bool &valid, std::vector<std::string> &errorMsgs, uint32_t &runtimeFlags);

/**
 * @brief Carves up a line and returns it into a vector by the seperator, it special handles {} as well
 * @brief If you are looking to just carve up any old line, use ecmdParseTokens, this is special for the spy format
 * @param line The line to tokenize
 * @param seperators What to search on
 * @param tokens A vector of tokens
 * @retval void
 */
void sedcCreateSpyTokens(std::string line, const char* seperators, sedcFileLine &myLine);


#endif /* sedcSpyParser_H */
