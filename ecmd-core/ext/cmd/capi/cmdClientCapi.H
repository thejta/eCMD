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

#ifndef cmdClientCapi_h
#define cmdClientCapi_h


/**
 * @file cmdClientCapi.H
 * @brief eCMD Command line extension

 * Primary Extension Owner   : Kahn Evans
 * Secondary Extension Owner : Steven Janssen

 @section cmd CMD Command line Extension
 This extension provides interfaces to call command line functions and have formatted data displayed to stdout or returned to the caller.  It supports command from the core command line and also all extensions.<br>

 Include files :
 <ul>
 <li> cmdClientCapi.H</li>
 <li> cmdStructs.H</li>
 <li> Library : libcmd.so</li>
 </ul>

*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <ecmdReturnCodes.H>
#include <ecmdStructs.H>
#include <ecmdDataBuffer.H>
#include <cmdStructs.H>

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------


/* Functions in here are defined as extern C for the following reasons:
 1)  Keeps Function names small by preventing C++ "mangling"
 2)  Allows (C-based) perl interpreter to access these functions

*/
#ifndef DOCUMENTATION
extern "C" {
#endif

#if !defined(ECMD_PERLAPI) && !defined(ECMD_PYAPI)
/** @name Load/Unload Functions */
//@{

/**
 @brief Initialize eCMD CMD Extension DLL
 @retval ECMD_SUCCESS if successful load
 @retval ECMD_INVALID_DLL_VERSION if Dll version loaded doesn't match client version
 @retval nonzero if unsuccessful
 @post eCMD CIP Extension is initialized and version checked

*/
uint32_t cmdInitExtension();

//@}
#endif

/** @name Command Line Execution Functions */
//@{

/**
 @brief Run the command line command passed in , output goes to stdout and is not returned
 @param i_command Command to pass to interpreter, ie 'ecmdquery version'
 @retval ECMD_SUCCESS if successful load
 @retval nonzero if unsuccessful
*/
uint32_t cmdRunCommand(std::string i_command);

/**
 @brief Run the command line command passed in , stdout is captured and returned
 @param i_command Command to pass to interpreter, ie 'ecmdquery version'
 @param o_output Standard out from running command
 @retval ECMD_SUCCESS if successful load
 @retval nonzero if unsuccessful
*/
uint32_t cmdRunCommandCaptureOutput(std::string i_command, std::string & o_output);

/**
 @brief Run the command line command passed in , output goes to stdout and is not returned
 @param i_file File to parse for commands to pass to interpreter
 @retval ECMD_SUCCESS if successful load
 @retval nonzero if unsuccessful
*/
uint32_t cmdRunCommandFile(std::string i_file);

/**
 @brief Run the command line command passed in , stdout is captured and returned
 @param i_file File to parse for commands to pass to interpreter
 @param o_output Standard out from running command
 @retval ECMD_SUCCESS if successful load
 @retval nonzero if unsuccessful
*/
uint32_t cmdRunCommandFileCaptureOutput(std::string i_file, std::string & o_output);
//@}

#ifndef DOCUMENTATION
} /* end extern "c" */
#endif

#endif /* cmdClientCapi_h */
