#ifndef ecmdClientCapi_H
#define ecmdClientCapi_H

/* $Header$ */

/**
 * @file ecmdClientCapi.H
 * @brief eCMD C/C++ Client Interface
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include "ecmdReturnCodes.H"

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------

/** @name Load/Unload Functions */
//@{
/**
 @brief Load the eCMD DLL
 @retval ECMD_SUCCESS if successful load   nonzero if unsuccessful
 @post eCMD DLL is loaded into memory
 @see initDll
 @see unloadDll

       - This function loads the DLL based first on CRONUS_DLL env variable, next on CRONUS_EXE env. variable.
       - Name limit of 255 characters.
       - Function also inits a symbol table of DLL functions.
       - Errors in loading are printed to STDOUT.
*/
ecmdrc_t ecmdLoadDll();

/**
 @brief Unload the eCMD DLL
 @retval ECMD_SUCCESS if successful load  nonzero if unsuccessful
 @see loadDll

 - Errors in unloading are printed to STDOUT.
*/

ecmdrc_t ecmdUnloadDll(); 

/**
 @brief Initialize the eCMD DLL
 @retval ECMD_SUCCESS if successful load  nonzero if unsuccessful
 @param argc Passed from Command line Arguments
 @param argv Passed from Command line Arguments
 @pre loadDll must have been called
 @post Global options (ex. -debug, -p#, -c#) will be removed from arg list
 @see loadDll

 - initializes objects in the DLL.
 - argc/argv get passed to the eCMD DLL.
 - Global options such as -debug flags and -p#, -c# will be parsed out.
 - Position flags can be queried later with functions like wasposselected()
*/

ecmdrc_t ecmdInitDll(int argc, char* argv[]); 

//@}


/** @name Error Handling Functions */
//@{

/**
 @brief Retrieve
*/
ecmdrc_t ecmdGetErrorMsg(ecmdrc_t errorcode, char* message);

ecmdrc_t ecmdRegisterErrorMsg(ecmdrc_t errorcode, const char* message);

//@}

/** @name Output Functions */
//@{

//@}



#endif /* ecmdClientCapi_H */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
