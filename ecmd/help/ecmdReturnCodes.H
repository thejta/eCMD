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


/* These are the prefixs that the plugins will use to specify their specific errors as well as eCMD errors itself */
#define ECMD_ERR_UNKNOWN                        0x00000000 ///< This error code wasn't flagged to which plugin it came from
#define ECMD_ERR_ECMD                           0x01000000 ///< Error came from eCMD
#define ECMD_ERR_CRONUS                         0x02000000 ///< Error came from Cronus
#define ECMD_ERR_IP                             0x04000000 ///< Error came from IP GFW
#define ECMD_ERR_Z                              0x08000000 ///< Error came from Z GFW

#define ECMD_SUCCESS                            0x0    ///< API Returned Successfully

  /* For now let's define all return codes in the 0x1000-0x1FFF range */

#define ECMD_INVALID_DLL_VERSION                (ECMD_ERR_ECMD | 0x1000) ///< Dll Version didn't match the Client version detected
#define ECMD_INVALID_DLL_FILENAME               (ECMD_ERR_ECMD | 0x1001) ///< Unable to find filename to load or file doesn't exist
#define ECMD_DLL_LOAD_FAILURE                   (ECMD_ERR_ECMD | 0x1002) ///< Error occured on call to dlopen
#define ECMD_DLL_UNLOAD_FAILURE                 (ECMD_ERR_ECMD | 0x1003) ///< Error occurred on call to dlclose
#define ECMD_DLL_UNINITIALIZED                  (ECMD_ERR_ECMD | 0x1004) ///< A function was called before ecmdLoadDll was called
#define ECMD_DLL_INVALID                        (ECMD_ERR_ECMD | 0x1005) ///< If we are unable to lookup a function in the Dll
#define ECMD_FAILURE                            (ECMD_ERR_ECMD | 0x1010) ///< General Failure occurred in eCMD
#define ECMD_TARGET_NOT_CONFIGURED              (ECMD_ERR_ECMD | 0x1011) ///< Chip target provided was not configured in the system
#define ECMD_FUNCTION_NOT_SUPPORTED             (ECMD_ERR_ECMD | 0x1012) ///< Returned if a specific Dll instance doesn't support the function you called
#define ECMD_UNKNOWN_FILE                       (ECMD_ERR_ECMD | 0x1013) ///< ecmdQueryFileLocation was unable to find the file you requested
#define ECMD_INVALID_ARGS                       (ECMD_ERR_ECMD | 0x1020) ///< Not enough arguments provided to the function
#define ECMD_INVALID_SPY_ENUM                   (ECMD_ERR_ECMD | 0x1021) ///< getSpyEnum or putSpyEnum used an invalid enum
#define ECMD_SPY_FAILED_ECC_CHECK               (ECMD_ERR_ECMD | 0x1022) ///< getSpy or getSpyEnum failed with invalid ECC detected in the hardware
#define ECMD_SPY_NOT_ENUMERATED                 (ECMD_ERR_ECMD | 0x1023) ///< getSpyEnum or putSpyEnum was called on a non-enumerated spy
#define ECMD_SPY_IS_EDIAL                       (ECMD_ERR_ECMD | 0x1024) ///< getSpy or Putspy was called on an edial
#define ECMD_INVALID_SPY                        (ECMD_ERR_ECMD | 0x1025) ///< Spy functions found an invalid Spy name or type
#define ECMD_DATA_OVERFLOW                      (ECMD_ERR_ECMD | 0x1026) ///< Too much data was provided to a write function
#define ECMD_DATA_UNDERFLOW                     (ECMD_ERR_ECMD | 0x1027) ///< Too little data was provided to a write function
#define ECMD_INVALID_RING                       (ECMD_ERR_ECMD | 0x1028) ///< Invalid ring name was provided
#define ECMD_INVALID_ARRAY                      (ECMD_ERR_ECMD | 0x1029) ///< Invalid array name was provided
#define ECMD_INVALID_CONFIG                     (ECMD_ERR_ECMD | 0x1030) ///< There was an error processing the configuration information
#define ECMD_CLOCKS_IN_INVALID_STATE            (ECMD_ERR_ECMD | 0x1031) ///< Chip Clocks were in an invalid state to perform the operation
#define ECMD_NON_JTAG_CHIP                      (ECMD_ERR_ECMD | 0x1032) ///< JTag function called  on non-jtag attached chip
#define ECMD_NON_FSI_CHIP                       (ECMD_ERR_ECMD | 0x1033) ///< Fsi function called on non-fsi attached chip
#define ECMD_INVALID_SPR                        (ECMD_ERR_ECMD | 0x1034) ///< Invalid SPR was specified to get/put spr functions
#define ECMD_INVALID_GPR                        (ECMD_ERR_ECMD | 0x1035) ///< Invalid GPR number was specified to get/put gpr functions
#define ECMD_INVALID_FPR                        (ECMD_ERR_ECMD | 0x1036) ///< Invalid FPR number was specified to get/put fpr functions
#define ECMD_RING_CACHE_ENABLED                 (ECMD_ERR_ECMD | 0x1037) ///< Ring Cache enabled during call non-cache enabled function
#define ECMD_INVALID_CONFIG_NAME                (ECMD_ERR_ECMD | 0x1038) ///< An Invalid name was used to set/get a configuation setting
#define ECMD_SPY_GROUP_MISMATCH                 (ECMD_ERR_ECMD | 0x1039) ///< A mismatch was found reading a group spy not all groups set the same
#define ECMD_INVALID_CLOCK_DOMAIN               (ECMD_ERR_ECMD | 0x1040) ///< An invalid clock domain name was specified
#define ECMD_CLOCKS_ALREADY_OFF                 (ECMD_ERR_ECMD | 0x1041) ///< A stopclocks was requested when clocks are already off
#define ECMD_CLOCKS_ALREADY_ON                  (ECMD_ERR_ECMD | 0x1042) ///< A startclocks was requested when clocks are already on
#define ECMD_UNABLE_TO_OPEN_SCANDEF             (ECMD_ERR_ECMD | 0x1043) ///< eCMD was unable to open the scandef
#define ECMD_INVALID_LATCHNAME                  (ECMD_ERR_ECMD | 0x1044) ///< eCMD was unable to find the specified latch in the scandef
#define ECMD_POLLING_FAILURE                    (ECMD_ERR_ECMD | 0x1045) ///< eCMD failed waiting for a poll to match expected value
#define ECMD_TARGET_INVALID_TYPE                (ECMD_ERR_ECMD | 0x1046) ///< Target specified an object that was inappropriate for the function
#define ECMD_EXTENSION_NOT_SUPPORTED            (ECMD_ERR_ECMD | 0x1047) ///< The current plugin does not supported the requested extension
#define ECMD_ISTEPS_INVALID_STEP                (ECMD_ERR_ECMD | 0x1048) ///< An invalid step name was provided
#define ECMD_UNABLE_TO_OPEN_SCANDEFHASH         (ECMD_ERR_ECMD | 0x1049) ///< eCMD was unable to open the scandefhash
#define ECMD_SCANDEFHASH_MULT_RINGS             (ECMD_ERR_ECMD | 0x1050) ///< Multiple ring keys matching the same latchname found
#define ECMD_UNABLE_TO_OPEN_SCOMDEF             (ECMD_ERR_ECMD | 0x1051) ///< eCMD was unable to open scomdef file
#define ECMD_SCOMADDRESS_NOT_FOUND              (ECMD_ERR_ECMD | 0x1052) ///< Scom Address not found in the ScomDef file
#define ECMD_INVALID_ENTRY_REQUESTED            (ECMD_ERR_ECMD | 0x1053) ///< An invalid entry was requested
#define ECMD_INSTRUCTIONS_IN_INVALID_STATE      (ECMD_ERR_ECMD | 0x1054) ///< Instructions were in an invalid state for operation
#define ECMD_INVALID_MEMORY_ADDRESS             (ECMD_ERR_ECMD | 0x1055) ///< Memory Address was not on a 8-byte boundary
#define ECMD_RETRY_WITH_VIRTUAL_ADDR            (ECMD_ERR_IP   | 0x1056) ///< Requests that the user retries operation with returned Virtual Address
#define ECMD_UNABLE_TO_MAP_HASHID		(ECMD_ERR_IP   | 0x1057) ///< Mapping function was unable to match the hashid given by the user
#define ECMD_UNABLE_TO_OPEN_ARRAYDEF            (ECMD_ERR_ECMD | 0x1058) ///< eCMD was unable to open arraydef file
#define ECMD_INVALID_RETURN_DATA                (ECMD_ERR_ECMD | 0x1059) ///< Data returned (if any) is incomplete or invalid... caller beware
#define ECMD_CHIP_UNKNOWN_FOR_MAPPING           (ECMD_ERR_ECMD | 0x105A) ///< Chip type did not map to a known EDC table
#define ECMD_INVALID_GP_REGISTER                (ECMD_ERR_ECMD | 0x105B) ///< Invalid GP Register was provided

/* Return codes for command interpreter */
#define ECMD_INT_UNKNOWN_COMMAND                (ECMD_ERR_ECMD | 0x1900)  ///< Command interpreter didn't understand command
#define ECMD_EXPECT_FAILURE                     (ECMD_ERR_ECMD | 0x1901)  ///< An expect was performed and a miscompare was found
#define ECMD_SCANDEF_LOOKUP_FAILURE             (ECMD_ERR_ECMD | 0x1902)  ///< An Error occurred trying to process the scandef file
#define ECMD_DATA_BOUNDS_OVERFLOW               (ECMD_ERR_ECMD | 0x1903)  ///< The user specified to get/put data that was larger then ECMD_MAX_DATA_BITS

/* dataBuffer specific return codes */
#ifndef ECMD_DBUF_SUCCESS
 #define ECMD_DBUF_SUCCESS                       0x0    ///< DataBuffer returned successfully
 #define ECMD_DBUF_INIT_FAIL                     (ECMD_ERR_ECMD | 0x2000) ///< Initialization of the DataBuffer failed
 #define ECMD_DBUF_BUFFER_OVERFLOW               (ECMD_ERR_ECMD | 0x2010) ///< Attempt to read/write data beyond the length of the DataBuffer
 #define ECMD_DBUF_XSTATE_ERROR                  (ECMD_ERR_ECMD | 0x2020) ///< An 'X' character occured where it was not expected
 #define ECMD_DBUF_UNDEFINED_FUNCTION            (ECMD_ERR_ECMD | 0x2030) ///< Function not included in this version of DataBuffer
 #define ECMD_DBUF_INVALID_ARGS                  (ECMD_ERR_ECMD | 0x2040) ///< Args provided to dataBuffer were invalid
 #define ECMD_DBUF_INVALID_DATA_FORMAT           (ECMD_ERR_ECMD | 0x2041) ///< String data didn't match expected input format
 #define ECMD_DBUF_FOPEN_FAIL                    (ECMD_ERR_ECMD | 0x2050) ///< File open on file for reading or writing the data buffer failed 
 #define ECMD_DBUF_FILE_FORMAT_MISMATCH          (ECMD_ERR_ECMD | 0x2051) ///< In readFile specified format not found in the data file
 #define ECMD_DBUF_DATANUMBER_NOT_FOUND          (ECMD_ERR_ECMD | 0x2052) ///< In readFileMultiple specified data number  not found in file
 #define ECMD_DBUF_FILE_OPERATION_FAIL           (ECMD_ERR_ECMD | 0x2053) ///< File operation failed
 #define ECMD_DBUF_NOT_OWNER                     (ECMD_ERR_ECMD | 0x2060) ///< Don't own this buffer so can't do this operation
# define ECMD_DBUF_XSTATE_NOT_ENABLED            (ECMD_ERR_ECMD | 0x2062) ///< Xstate function called on a buffer that doesn't have xstates enabled
#endif


#endif /* ecmdReturnCodes_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
