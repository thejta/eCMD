#ifndef ecmdReturnCodes_h
#define ecmdReturnCodes_h
// Copyright **********************************************************
//                                                                      
// File ecmdReturnCodes.H                                               
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 1996                                         
//                                                                      
// The source code for this program is not published or otherwise       
// divested of its trade secrets, irrespective of what has been         
// deposited with the U.S. Copyright Office.                             
//                                                                      
// End Copyright ******************************************************

/* $Header$ */

/**
 * @file ecmdReturnCodes.H
 * @brief All Return Codes for the eCmd Capi
*/


#define ECMD_SUCCESS                    0x0    ///< API Returned Successfully

  /* For now let's define all return codes in the 0x1000-0x1FFF range */

#define ECMD_INVALID_DLL_VERSION        0x1000 ///< Dll Version didn't match the Client version detected
#define ECMD_INVALID_DLL_FILENAME       0x1001 ///< Unable to find filename to load or file doesn't exist
#define ECMD_DLL_LOAD_FAILURE           0x1002 ///< Error occured on call to dlopen
#define ECMD_DLL_UNLOAD_FAILURE         0x1003 ///< Error occurred on call to dlclose
#define ECMD_DLL_UNINITIALIZED          0x1004 ///< A function was called before ecmdLoadDll was called
#define ECMD_DLL_INVALID                0x1005 ///< If we are unable to lookup a function in the Dll
#define ECMD_FAILURE                    0x1010 ///< General Failure occurred in eCMD
#define ECMD_POS_NOT_CONFIGURED         0x1011 ///< Chip target provided was not configured in the system
#define ECMD_INVALID_ARGS               0x1020 ///< Not enough arguments provided to the function
#define ECMD_INVALID_SPY_ENUM		0x1021 ///< getSpyEnum or putSpyEnum used an invalid enum
#define ECMD_FUNCTION_NOT_SUPPORTED     0x1012 ///< Returned if a specific Dll instance doesn't support the function you called

/* dataBuffer specific return codes */
#define ECMD_DBUF_SUCCESS               0x0    ///< DataBuffer returned successfully
#define ECMD_DBUF_INIT_FAIL             0x2000 ///< Initialization of the DataBuffer failed
#define ECMD_DBUF_BUFFER_OVERFLOW       0x2010 ///< Attempt to read/write data beyond the length of the DataBuffer
#define ECMD_DBUF_XSTATE_ERROR          0x2020 ///< An 'X' character occured where it was not expected
#define ECMD_DBUF_UNDEFINED_FUNCTION    0x2030 ///< Function not included in this version of DataBuffer
#define ECMD_DBUF_INVALID_ARGS          0x2040 ///< Args provided to dataBuffer were invalid

#endif /* ecmdReturnCodes_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
