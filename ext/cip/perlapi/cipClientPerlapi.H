#ifndef cipClientPerlapi_h
#define cipClientPerlapi_h

/* $Header$ */

/**
 * @file cipClientPerlapi.H
 * @brief Cronus & IP eCMD Perlapi Extension

 * Extension Owner : Chris Engel
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------


#include <ecmdReturnCodes.H>
#include <ecmdStructs.H>
#include <ecmdDataBuffer.H>
#include <cipStructs.H>

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------


/* Functions in here are defined as extern C for the following reasons:
 1)  Keeps Function names small by preventing C++ "mangling"
 2)  Allows (C-based) perl interpreter to access these functions

*/
#ifndef DOCUMENTATION
namespace CIPPERLAPI {
#endif

    /** @name Load/Unload Functions */
    //@{

    /**
     @brief Initialize eCMD CIP Extension DLL
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
     if (cipInitExtension("ver3,ver4")) { die "Fatal errors initializing DLL"; }

     */
    int cipInitExtension(const char * i_clientVersion);

    //@}





#ifndef DOCUMENTATION
}       // End namespace
#endif

#endif /* cipClientCapi_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
