#ifndef cmdClientPyapi_h
#define cmdClientPyapi_h

/* $Header$ */

/**
 * @file cmdClientPyapi.H
 * @brief Command Line eCMD Pyapi Extension

 * Extension Owner : Chris Engel
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


/** @name Load/Unload Functions */
//@{

/**
   @brief Initialize eCMD CMD Extension DLL
   @param i_clientVersion Comma seperated list of eCMD Python api major numbers this script supports, see details
   @retval ECMD_SUCCESS if successful load
   @retval ECMD_INVALID_DLL_VERSION if Dll version loaded doesn't match client version
   @retval nonzero if unsuccessful
   @post eCMD CMD Extension is initialized and version checked

   VERSIONS :
   eCMD at times has to make changes to add/remove functionality and parameters to functions.  This could cause
   incompatability in your script if you used functions that have changed.  The i_clientVersion string is used
   to tell eCMD which major releases you support such that your script will not continue execution if it encounters
   a version that is either not known about or not supported.  This is similar to how the eCMD C-Api works except in
   Python you can support multiple versions with one script as long as the changes that were made between the versions
   do not affect your script.

   USAGE :
   if (cmdInitExtension("ver3,ver4")):
       print("ERROR: Fatal errors initializing DLL")
       sys.exit(0)
   
*/
int cmdInitExtension(const char * i_clientVersion);

//@}


#endif /* cmdClientCapi_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
