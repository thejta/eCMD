#ifndef ecmdReturnCodes_h
#define ecmdReturnCodes_h
/* $Header$ */
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

/**
 * @file ecmdReturnCodes.H
 * @brief All Return Codes for the eCmd Capi
*/

#include <ecmdDefines.H>


/* These are the prefixs that the plugins will use to specify their specific errors as well as eCMD errors itself */
#define ECMD_ERR_UNKNOWN                        0x00000000 ///< This error code wasn't flagged to which plugin it came from
#define ECMD_ERR_ECMD                           0x01000000 ///< Error came from eCMD
#define ECMD_ERR_CRONUS                         0x02000000 ///< Error came from Cronus
#define ECMD_ERR_IP                             0x04000000 ///< Error came from IP GFW
#define ECMD_ERR_Z                              0x08000000 ///< Error came from Z GFW
#define ECMD_ERR_SCAND                          0x10000000 ///< Error came from Z GFW
#define ECMD_ERR_LHT                            0x20000000 ///< Error came from Linux Host Tool

#define ECMD_SUCCESS                            0x0    ///< API Returned Successfully

/* For now let's define all return codes in the 0x1000-0x1FFF range */
#define ECMD_INVALID_DLL_VERSION                (ECMD_ERR_ECMD | 0x1001) ///< Dll Version didn't match the Client version detected
#define ECMD_INVALID_DLL_FILENAME               (ECMD_ERR_ECMD | 0x1003) ///< Unable to find filename to load or file doesn't exist
#define ECMD_DLL_LOAD_FAILURE                   (ECMD_ERR_ECMD | 0x1005) ///< Error occured on call to dlopen
#define ECMD_DLL_UNLOAD_FAILURE                 (ECMD_ERR_ECMD | 0x1007) ///< Error occurred on call to dlclose
#define ECMD_DLL_UNINITIALIZED                  (ECMD_ERR_ECMD | 0x1009) ///< A function was called before ecmdLoadDll was called
#define ECMD_DLL_INVALID                        (ECMD_ERR_ECMD | 0x100B) ///< If we are unable to lookup a function in the Dll
#define ECMD_FAILURE                            (ECMD_ERR_ECMD | 0x100D) ///< General Failure occurred in eCMD
#define ECMD_TARGET_NOT_CONFIGURED              (ECMD_ERR_ECMD | 0x100F) ///< Chip target provided was not configured in the system
#define ECMD_FUNCTION_NOT_SUPPORTED             (ECMD_ERR_ECMD | 0x1011) ///< Returned if a specific Dll instance doesn't support the function you called
#define ECMD_UNKNOWN_FILE                       (ECMD_ERR_ECMD | 0x1013) ///< ecmdQueryFileLocation was unable to find the file you requested
#define ECMD_UNKNOWN_HELP_FILE                  (ECMD_ERR_ECMD | 0x1014) ///< Helpfile could not be read
#define ECMD_INVALID_ARGS                       (ECMD_ERR_ECMD | 0x1015) ///< Not enough arguments provided to the function
#define ECMD_INVALID_SPY_ENUM                   (ECMD_ERR_ECMD | 0x1017) ///< getSpyEnum or putSpyEnum used an invalid enum
#define ECMD_SPY_FAILED_ECC_CHECK               (ECMD_ERR_ECMD | 0x1019) ///< getSpy or getSpyEnum failed with invalid ECC detected in the hardware
#define ECMD_SPY_NOT_ENUMERATED                 (ECMD_ERR_ECMD | 0x101B) ///< getSpyEnum or putSpyEnum was called on a non-enumerated spy
#define ECMD_SPY_IS_EDIAL                       (ECMD_ERR_ECMD | 0x101D) ///< getSpy or Putspy was called on an edial
#define ECMD_INVALID_SPY                        (ECMD_ERR_ECMD | 0x101F) ///< Spy functions found an invalid Spy name or type
#define ECMD_DATA_OVERFLOW                      (ECMD_ERR_ECMD | 0x1021) ///< Too much data was provided to a write function
#define ECMD_DATA_UNDERFLOW                     (ECMD_ERR_ECMD | 0x1023) ///< Too little data was provided to a write function
#define ECMD_INVALID_RING                       (ECMD_ERR_ECMD | 0x1025) ///< Invalid ring name was provided
#define ECMD_INVALID_ARRAY                      (ECMD_ERR_ECMD | 0x1027) ///< Invalid array name was provided
#define ECMD_INVALID_CONFIG                     (ECMD_ERR_ECMD | 0x1029) ///< There was an error processing the configuration information
#define ECMD_CLOCKS_IN_INVALID_STATE            (ECMD_ERR_ECMD | 0x102B) ///< Chip Clocks were in an invalid state to perform the operation
#define ECMD_NON_JTAG_CHIP                      (ECMD_ERR_ECMD | 0x102D) ///< JTag function called  on non-jtag attached chip
#define ECMD_NON_FSI_CHIP                       (ECMD_ERR_ECMD | 0x102F) ///< Fsi function called on non-fsi attached chip
#define ECMD_INVALID_SPR                        (ECMD_ERR_ECMD | 0x1031) ///< Invalid SPR was specified to get/put spr functions
#define ECMD_INVALID_GPR                        (ECMD_ERR_ECMD | 0x1033) ///< Invalid GPR number was specified to get/put gpr functions
#define ECMD_INVALID_FPR                        (ECMD_ERR_ECMD | 0x1035) ///< Invalid FPR number was specified to get/put fpr functions
#define ECMD_RING_CACHE_ENABLED                 (ECMD_ERR_ECMD | 0x1037) ///< Ring Cache enabled during call non-cache enabled function
#define ECMD_INVALID_CONFIG_NAME                (ECMD_ERR_ECMD | 0x1039) ///< An Invalid name was used to set/get a configuation setting
#define ECMD_INVALID_CONFIG_DATA                (ECMD_ERR_ECMD | 0x103B) ///< An Invalid data type was used to set/get a configuation setting
#define ECMD_SPY_GROUP_MISMATCH                 (ECMD_ERR_ECMD | 0x103D) ///< A mismatch was found reading a group spy not all groups set the same
#define ECMD_INVALID_CLOCK_DOMAIN               (ECMD_ERR_ECMD | 0x103F) ///< An invalid clock domain name was specified
#define ECMD_CLOCKS_ALREADY_OFF                 (ECMD_ERR_ECMD | 0x1041) ///< A stopclocks was requested when clocks are already off
#define ECMD_CLOCKS_ALREADY_ON                  (ECMD_ERR_ECMD | 0x1043) ///< A startclocks was requested when clocks are already on
#define ECMD_UNABLE_TO_OPEN_SCANDEF             (ECMD_ERR_ECMD | 0x1045) ///< eCMD was unable to open the scandef
#define ECMD_INVALID_LATCHNAME                  (ECMD_ERR_ECMD | 0x1047) ///< eCMD was unable to find the specified latch in the scandef
#define ECMD_POLLING_FAILURE                    (ECMD_ERR_ECMD | 0x1049) ///< eCMD failed waiting for a poll to match expected value
#define ECMD_TARGET_INVALID_TYPE                (ECMD_ERR_ECMD | 0x104B) ///< Target specified an object that was inappropriate for the function
#define ECMD_EXTENSION_NOT_SUPPORTED            (ECMD_ERR_ECMD | 0x104D) ///< The current plugin does not supported the requested extension
#define ECMD_ISTEPS_INVALID_STEP                (ECMD_ERR_ECMD | 0x104F) ///< An invalid step name was provided
#define ECMD_UNABLE_TO_OPEN_SCANDEFHASH         (ECMD_ERR_ECMD | 0x1051) ///< eCMD was unable to open the scandefhash
#define ECMD_SCANDEFHASH_MULT_RINGS             (ECMD_ERR_ECMD | 0x1053) ///< Multiple ring keys matching the same latchname found
#define ECMD_UNABLE_TO_OPEN_SCOMDEF             (ECMD_ERR_ECMD | 0x1055) ///< eCMD was unable to open scomdef file
#define ECMD_SCOMADDRESS_NOT_FOUND              (ECMD_ERR_ECMD | 0x1057) ///< Scom Address not found in the ScomDef file
#define ECMD_INVALID_ENTRY_REQUESTED            (ECMD_ERR_ECMD | 0x1059) ///< An invalid entry was requested
#define ECMD_INSTRUCTIONS_IN_INVALID_STATE      (ECMD_ERR_ECMD | 0x105B) ///< Instructions were in an invalid state for operation
#define ECMD_INVALID_MEMORY_ADDRESS             (ECMD_ERR_ECMD | 0x105D) ///< Memory Address was not on a proper byte boundary
#define ECMD_RETRY_WITH_VIRTUAL_ADDR            (ECMD_ERR_IP   | 0x105F) ///< Requests that the user retries operation with returned Virtual Address
#define ECMD_UNABLE_TO_MAP_HASHID		(ECMD_ERR_IP   | 0x1061) ///< Mapping function was unable to match the hashid given by the user
#define ECMD_UNABLE_TO_OPEN_ARRAYDEF            (ECMD_ERR_ECMD | 0x1063) ///< eCMD was unable to open arraydef file
#define ECMD_INVALID_RETURN_DATA                (ECMD_ERR_ECMD | 0x1065) ///< Data returned (if any) is incomplete or invalid... caller beware
#define ECMD_CHIP_UNKNOWN_FOR_MAPPING           (ECMD_ERR_ECMD | 0x1067) ///< Chip type did not map to a known EDC table
#define ECMD_INVALID_GP_REGISTER                (ECMD_ERR_ECMD | 0x1069) ///< Invalid GP Register was provided
#define ECMD_INVALID_SCOM                       (ECMD_ERR_ECMD | 0x106B) ///< Invalid SCOM Register was provided
#define ECMD_RING_NOT_FOUND                     (ECMD_ERR_ECMD | 0x106D) ///< Ring requested was not found
#define ECMD_SCOM_NOT_FOUND                     (ECMD_ERR_ECMD | 0x106F) ///< Scom requested was not found
#define ECMD_ARRAY_NOT_FOUND                    (ECMD_ERR_ECMD | 0x1071) ///< Array requested was not found
#define ECMD_ARRAY_INVALID_OBJECT               (ECMD_ERR_ECMD | 0x1072) ///< No array table available for this chip
#define ECMD_TRACEARRAY_NOT_FOUND               (ECMD_ERR_ECMD | 0x1073) ///< Trace Array requested was not found
#define ECMD_CFAMREG_NOT_FOUND                  (ECMD_ERR_ECMD | 0x1075) ///< Cfam Register requested was not found
#define ECMD_CFAMADDRESS_OUT_OF_RANGE           (ECMD_ERR_ECMD | 0x1076) ///< Cfam Address requested was out of scom and scan range
#define ECMD_GPREG_NOT_FOUND                    (ECMD_ERR_ECMD | 0x1077) ///< GP Register requested was not found
#define ECMD_SPY_NOT_FOUND                      (ECMD_ERR_ECMD | 0x1079) ///< Spy requested was not found
#define ECMD_IRSTAT_NOT_FOUND                   (ECMD_ERR_ECMD | 0x107B) ///< IRSTAT requested was not found
#define ECMD_WRITING_TO_READ_ONLY_REG           (ECMD_ERR_ECMD | 0x107D) ///< Writing to Read-Only Register
#define ECMD_READING_FROM_WRITE_ONLY_REG        (ECMD_ERR_ECMD | 0x107F) ///< Reading from Write-Only Register
#define ECMD_INVALID_INIT_ID                    (ECMD_ERR_ECMD | 0x1081) ///< Initialization ID  not found in eng data
#define ECMD_INVALID_INIT_FILE                  (ECMD_ERR_ECMD | 0x1083) ///< File not found 
#define ECMD_INVALID_INIT_MODE                  (ECMD_ERR_ECMD | 0x1085) ///< Initialization mode not supported
#define ECMD_NOT_CHIPUNIT_0                     (ECMD_ERR_ECMD | 0x108A) ///< The item passed in wasn't for chipUnit 0
#define ECMD_ARRAY_SCANRING_ERROR               (ECMD_ERR_ECMD | 0x108B) ///< Failed to create scanring object during array access
#define ECMD_ARRAY_SETUP_CONTROLS_ERROR         (ECMD_ERR_ECMD | 0x108D) ///< Failed to setup array controls
#define ECMD_ARRAY_CACHE_NOT_OFF_ERROR          (ECMD_ERR_ECMD | 0x108F) ///< Cache must be turned off before accessing an array
#define ECMD_ARRAY_SCOM_RETRY_ERROR             (ECMD_ERR_ECMD | 0x1091) ///< Read/write done indicator not set after 10 retries during scom 1 or 2 array access
#define ECMD_SCANRING_ILLEGAL_OPER              (ECMD_ERR_ECMD | 0x1093) ///< Scan/scom access with wrong mode or hw scanring access with cache on
#define ECMD_SCANRING_INVALID_OBJECT            (ECMD_ERR_ECMD | 0x1095) ///< No proper scan/scom init or wrong ringtype requested
#define ECMD_SCANRING_ACCESS_FAIL               (ECMD_ERR_ECMD | 0x1097) ///< Scanring access failed with bad return code from i390
#define ECMD_SCANRING_INVALID_STATE             (ECMD_ERR_ECMD | 0x1099) ///< Scanring shift mode error - selftest still running or clocks running
#define ECMD_SCANRING_SCAN_PROTECT              (ECMD_ERR_ECMD | 0x109B) ///< Attempt to access scanring although clocks were running
#define ECMD_SPYDEF_INVALID                     (ECMD_ERR_ECMD | 0x109D) ///< The (binary) spy definition is corrupt
#define ECMD_SPY_NO_RING_FOUND                  (ECMD_ERR_ECMD | 0x109F) ///< Failed to create scanring object
#define ECMD_SPY_EXTRACT_DATA_FAIL              (ECMD_ERR_ECMD | 0x10A1) ///< The (binary) spy definition is corrupt
#define ECMD_SPY_INSERT_DATA_FAIL               (ECMD_ERR_ECMD | 0x10A3) ///< The (binary) spy definition is corrupt
#define ECMD_SPY_FLIP_BITS_FAIL                 (ECMD_ERR_ECMD | 0x10A5) ///< The (binary) spy definition is corrupt
#define ECMD_SPY_WRITE_FAIL                     (ECMD_ERR_ECMD | 0x10A7) ///< Spy size vs buffer mismatch or unknown enum
#define ECMD_SPY_INVALID_READ_OPER              (ECMD_ERR_ECMD | 0x10A9) ///< Spy enum read operation on non-enum or vice versa
#define ECMD_SPY_INVALID_WRITE_OPER             (ECMD_ERR_ECMD | 0x10AB) ///< Spy enum write op on non-enum or vice versa or write op to non-defined enum for this spy
#define ECMD_SPY_INVALID_OBJECT                 (ECMD_ERR_ECMD | 0x10AD) ///< Spy not properly initialized
#define ECMD_SCOM_INVALID_OBJECT                (ECMD_ERR_ECMD | 0x10AF) ///< Scom not properly initialized
#define ECMD_HREG_ECMDDATABUFFER_ERROR          (ECMD_ERR_ECMD | 0x10B1) ///< Scanring buffer op failed or corrupt scanring binary or memory mgmt issue
#define ECMD_HREG_ADDRESS_INVALID_PARAM         (ECMD_ERR_ECMD | 0x10B3) ///< Address translation failed due to invalied combination of parameters
#define ECMD_SENSOR_HUMIDITY_NOT_AVAILABLE       (ECMD_ERR_ECMD | 0x10C1) ///< Humidity Sensor data not available
#define ECMD_SENSOR_AIR_DENSITY_NOT_AVAILABLE   (ECMD_ERR_ECMD | 0x10C3) ///< Air Density Sensor data not available
#define ECMD_SENSOR_UNIT_NOT_SUPPORTED          (ECMD_ERR_ECMD | 0x10C5) ///< Unit not supported for sensor
#define ECMD_SENSOR_ID_NOT_SUPPORTED            (ECMD_ERR_ECMD | 0x10C7) ///< Sensor Id not supported for sensor
#define ECMD_SENSOR_THERMAL_NOT_SUPPORTED       (ECMD_ERR_ECMD | 0x10C9) ///< Thermal Sensor data not available
#define ECMD_FATAL_FAILURE                      (ECMD_ERR_ECMD | 0x10CB) ///< Fatal error during plugin load due to library versions mismatch
#define ECMD_WILDCARD_CHAR_NOT_SUPPORTED        (ECMD_ERR_ECMD | 0x10CD) ///< Wildcard 'x' was used with a command that doesn't support it
#define ECMD_WILDCARD_MISUSE                    (ECMD_ERR_ECMD | 0x10CF) ///< Wildcard 'x' was used incorrectly - i.e. as a chipunit 
#define ECMD_UNABLE_TO_OPEN_SPYDEF              (ECMD_ERR_ECMD | 0x10D1) ///< eCMD was unable to open the spydef



/* Return codes for command interpreter */
#define ECMD_INT_UNKNOWN_COMMAND                (ECMD_ERR_ECMD | 0x1901)  ///< Command interpreter didn't understand command
#define ECMD_EXPECT_FAILURE                     (ECMD_ERR_ECMD | 0x1903)  ///< An expect was performed and a miscompare was found
#define ECMD_SCANDEF_LOOKUP_FAILURE             (ECMD_ERR_ECMD | 0x1905)  ///< An Error occurred trying to process the scandef file
#define ECMD_DATA_BOUNDS_OVERFLOW               (ECMD_ERR_ECMD | 0x1907)  ///< The user specified to get/put data that was larger then ECMD_MAX_DATA_BITS
#define ECMD_SPYDEF_LOOKUP_FAILURE              (ECMD_ERR_ECMD | 0x1909)  ///< An Error occurred trying to process the scandef file

/* dataBuffer specific return codes */
#ifndef ECMD_DBUF_SUCCESS
 #define ECMD_DBUF_SUCCESS                       0x0    ///< DataBuffer returned successfully
 #define ECMD_DBUF_INIT_FAIL                     (ECMD_ERR_ECMD | 0x2001) ///< Initialization of the DataBuffer failed
 #define ECMD_DBUF_BUFFER_OVERFLOW               (ECMD_ERR_ECMD | 0x2011) ///< Attempt to read/write data beyond the length of the DataBuffer
 #define ECMD_DBUF_XSTATE_ERROR                  (ECMD_ERR_ECMD | 0x2021) ///< An 'X' character occured where it was not expected
 #define ECMD_DBUF_UNDEFINED_FUNCTION            (ECMD_ERR_ECMD | 0x2031) ///< Function not included in this version of DataBuffer
 #define ECMD_DBUF_INVALID_ARGS                  (ECMD_ERR_ECMD | 0x2041) ///< Args provided to dataBuffer were invalid
 #define ECMD_DBUF_INVALID_DATA_FORMAT           (ECMD_ERR_ECMD | 0x2043) ///< String data didn't match expected input format
 #define ECMD_DBUF_FOPEN_FAIL                    (ECMD_ERR_ECMD | 0x2051) ///< File open on file for reading or writing the data buffer failed 
 #define ECMD_DBUF_FILE_FORMAT_MISMATCH          (ECMD_ERR_ECMD | 0x2053) ///< In readFile specified format not found in the data file
 #define ECMD_DBUF_DATANUMBER_NOT_FOUND          (ECMD_ERR_ECMD | 0x2055) ///< In readFileMultiple specified data number  not found in file
 #define ECMD_DBUF_FILE_OPERATION_FAIL           (ECMD_ERR_ECMD | 0x2057) ///< File operation failed
 #define ECMD_DBUF_NOT_OWNER                     (ECMD_ERR_ECMD | 0x2061) ///< Don't own this buffer so can't do this operation
 #define ECMD_DBUF_XSTATE_NOT_ENABLED            (ECMD_ERR_ECMD | 0x2063) ///< Xstate function called on a buffer that doesn't have xstates enabled
 #define ECMD_DBUF_MISMATCH                      (ECMD_ERR_ECMD | 0x2065) ///< There was a mismatch between a comparision operation in the buffer
 #define ECMD_DBUF_NULL_POINTER                  (ECMD_ERR_ECMD | 0x2067) ///< Null pointer passed 
#endif

#define ECMD_GET_ALL_REMAINING_ERRORS            (ECMD_ERR_ECMD | 0xFFFF) ///< Retrieve all errors from getErrorMsg.  Not to be used.

#endif /* ecmdReturnCodes_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
