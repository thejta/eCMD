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


#define ECMD_SUCCESS                            0x0    ///< API Returned Successfully

  /* For now let's define all return codes in the 0x1000-0x1FFF range */

#define ECMD_INVALID_DLL_VERSION                0x1000 ///< Dll Version didn't match the Client version detected
#define ECMD_INVALID_DLL_FILENAME               0x1001 ///< Unable to find filename to load or file doesn't exist
#define ECMD_DLL_LOAD_FAILURE                   0x1002 ///< Error occured on call to dlopen
#define ECMD_DLL_UNLOAD_FAILURE                 0x1003 ///< Error occurred on call to dlclose
#define ECMD_DLL_UNINITIALIZED                  0x1004 ///< A function was called before ecmdLoadDll was called
#define ECMD_DLL_INVALID                        0x1005 ///< If we are unable to lookup a function in the Dll
#define ECMD_FAILURE                            0x1010 ///< General Failure occurred in eCMD
#define ECMD_TARGET_NOT_CONFIGURED              0x1011 ///< Chip target provided was not configured in the system
#define ECMD_FUNCTION_NOT_SUPPORTED             0x1012 ///< Returned if a specific Dll instance doesn't support the function you called
#define ECMD_UNKNOWN_FILE                       0x1013 ///< ecmdQueryFileLocation was unable to find the file you requested
#define ECMD_INVALID_ARGS                       0x1020 ///< Not enough arguments provided to the function
#define ECMD_INVALID_SPY_ENUM                   0x1021 ///< getSpyEnum or putSpyEnum used an invalid enum
#define ECMD_SPY_FAILED_ECC_CHECK               0x1022 ///< getSpy or getSpyEnum failed with invalid ECC detected in the hardware
#define ECMD_SPY_NOT_ENUMERATED                 0x1023 ///< getSpyEnum or putSpyEnum was called on a non-enumerated spy
#define ECMD_SPY_IS_EDIAL                       0x1024 ///< getSpy or Putspy was called on an edial
#define ECMD_INVALID_SPY                        0x1025 ///< Spy functions found an invalid Spy name or type
#define ECMD_DATA_OVERFLOW                      0x1026 ///< Too much data was provided to a write function
#define ECMD_DATA_UNDERFLOW                     0x1027 ///< Too little data was provided to a write function
#define ECMD_INVALID_RING                       0x1028 ///< Invalid ring name was provided
#define ECMD_INVALID_ARRAY                      0x1029 ///< Invalid array name was provided
#define ECMD_INVALID_CONFIG                     0x1030 ///< There was an error processing the configuration information
#define ECMD_CLOCKS_IN_INVALID_STATE            0x1031 ///< Chip Clocks were in an invalid state to perform the operation
#define ECMD_NON_JTAG_CHIP                      0x1032 ///< JTag function called  on non-jtag attached chip
#define ECMD_NON_FSI_CHIP                       0x1033 ///< Fsi function called on non-fsi attached chip
#define ECMD_INVALID_SPR                        0x1034 ///< Invalid SPR was specified to get/put spr functions
#define ECMD_INVALID_GPR                        0x1035 ///< Invalid GPR number was specified to get/put gpr functions
#define ECMD_INVALID_FPR                        0x1036 ///< Invalid FPR number was specified to get/put fpr functions
#define ECMD_RING_CACHE_ENABLED                 0x1037 ///< Ring Cache enabled during call non-cache enabled function
#define ECMD_INVALID_CONFIG_NAME                0x1038 ///< An Invalid name was used to set/get a configuation setting
#define ECMD_SPY_GROUP_MISMATCH                 0x1039 ///< A mismatch was found reading a group spy not all groups set the same
#define ECMD_INVALID_CLOCK_DOMAIN               0x1040 ///< An invalid clock domain name was specified
#define ECMD_CLOCKS_ALREADY_OFF                 0x1041 ///< A stopclocks was requested when clocks are already off
#define ECMD_CLOCKS_ALREADY_ON                  0x1042 ///< A startclocks was requested when clocks are already on
#define ECMD_UNABLE_TO_OPEN_SCANDEF             0x1043 ///< eCMD was unable to open the scandef
#define ECMD_INVALID_LATCHNAME                  0x1044 ///< eCMD was unable to find the specified latch in the scandef
#define ECMD_POLLING_FAILURE                    0x1045 ///< eCMD failed waiting for a poll to match expected value
#define ECMD_TARGET_INVALID_TYPE                0x1046 ///< Target specified an object that was inappropriate for the function
#define ECMD_EXTENSION_NOT_SUPPORTED            0x1047 ///< The current plugin does not supported the requested extension
#define ECMD_ISTEPS_INVALID_STEP                0x1048 ///< An invalid step name was provided

/* Return codes for command interpreter */
#define ECMD_INT_UNKNOWN_COMMAND                0x1900  ///< Command interpreter didn't understand command
#define ECMD_EXPECT_FAILURE                     0x1901  ///< An expect was performed and a miscompare was found
#define ECMD_SCANDEF_LOOKUP_FAILURE             0x1902  ///< An Error occurred trying to process the scandef file
#define ECMD_DATA_BOUNDS_OVERFLOW               0x1903  ///< The user specified to get/put data that was larger then ECMD_MAX_DATA_BITS

/* dataBuffer specific return codes */
#define ECMD_DBUF_SUCCESS                       0x0    ///< DataBuffer returned successfully
#define ECMD_DBUF_INIT_FAIL                     0x2000 ///< Initialization of the DataBuffer failed
#define ECMD_DBUF_BUFFER_OVERFLOW               0x2010 ///< Attempt to read/write data beyond the length of the DataBuffer
#define ECMD_DBUF_XSTATE_ERROR                  0x2020 ///< An 'X' character occured where it was not expected
#define ECMD_DBUF_UNDEFINED_FUNCTION            0x2030 ///< Function not included in this version of DataBuffer
#define ECMD_DBUF_INVALID_ARGS                  0x2040 ///< Args provided to dataBuffer were invalid
#define ECMD_DBUF_INVALID_DATA_FORMAT           0x2041 ///< String data didn't match expected input format



#endif /* ecmdReturnCodes_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
