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

#ifndef sedcScomdefParser_h
#define sedcScomdefParser_h

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
