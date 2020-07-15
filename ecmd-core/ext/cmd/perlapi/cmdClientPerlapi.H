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

#ifndef cmdClientPerlapi_h
#define cmdClientPerlapi_h


/**
 * @file cmdClientPerlapi.H
 * @brief Command Line eCMD Perlapi Extension

 * Primary Extension Owner   : Kahn Evans
 * Secondary Extension Owner : Steven Janssen

 @section cmd CMD Command line Extension
 This extension provides interfaces to call command line functions and have formatted data displayed to stdout or returned to the caller.  It supports command from the core command line and also all extensions.<br>

 Include files :
 <ul>
 <li> cmdClientPerlapi.H</li>
 <li> cmdStructs.H</li>
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
namespace CMDPERLAPI {
#endif

    /** @name Load/Unload Functions */
    //@{

    /**
     @brief Initialize eCMD CMD Extension 
     @param i_clientVersion Comma seperated list of eCMD Perl api major numbers this script supports, see details
     @retval ECMD_SUCCESS if successful load
     @retval ECMD_INVALID_DLL_VERSION if Dll version loaded doesn't match client version
     @retval nonzero if unsuccessful
     @post eCMD CIP Extension is initialized and version checked

     VERSIONS :
     eCMD at times has to make changes to add/remove functionality and parameters to functions.  This could cause
     incompatability in your script if you used functions that have changed.  The i_clientVersion string is used
     to tell eCMD which major releases you support such that your script will not continue execution if it encounters
     a version that is either not known about or not supported.  This is similar to how the eCMD C-Api works except in
     Perl you can support multiple versions with one script as long as the changes that were made between the versions
     do not affect your script.

     USAGE :
     if (cmdInitExtension("ver3,ver4")) { die "Fatal errors initializing DLL"; }

     */
  int cmdInitExtension(const char * i_clientVersion);

    //@}





#ifndef DOCUMENTATION
}       // End namespace
#endif

#endif /* cmdClientCapi_h */
