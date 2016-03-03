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

#ifndef ecmdExtInterpreter_h
#define ecmdExtInterpreter_h


/**
 @file ecmdExtInterpreter.H
 @brief eCMD Extention Command Interpreter function prototypes
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------

/**
 @brief Walk through the enabled extension interpreters
 @param argc Command line arguments
 @param argv Command line arguments
 @param io_rc The return code from calling the interpreters
 @retval ECMD_SUCCESS On successful command
 @retval nonzero On command failure
*/
uint32_t ecmdCallExtInterpreters(int argc, char* argv[], uint32_t & io_rc);

#endif /* ecmdExtInterpreter_h */
