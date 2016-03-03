//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2003,2016 IBM International Business Machines Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
//IBM_PROLOG_END_TAG

#ifndef sedcSpyParser_H
#define sedcSpyParser_H

// Class Description *************************************************
//
//  Name:  sedcSpyParser
//  Base class:
//
//  Description:
//  Usage:
//
// End Class Description *********************************************

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
