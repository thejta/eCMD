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
#include <ecmdReturnCodes.H>
#include <ecmdStructs.H>
#include <ecmdDataBuffer.H>

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------

/* Functions in here are defined as extern C for the following reasons:
 1)  Keeps Function names small by preventing C++ "mangling"
 2)  Allows (C-based) perl interpreter to access these functions

*/
extern "C" {

/** @name Load/Unload Functions */
//@{
/**
 @brief Load the eCMD DLL
 @param i_dllName Specify the full path and name of the dll to load, 
 @retval ECMD_SUCCESS if successful load
 @retval ECMD_INVALID_DLL_VERSION if Dll version loaded doesn't match client version
 @retval ECMD_INVALID_DLL_FILENAME if dllName and ECMD_DLL_FILE are not specified
 @retval ECMD_DLL_LOAD_FAILURE if failure occurs on call to dlopen
 @retval nonzero if unsuccessful
 @post eCMD DLL is loaded into memory and initialized
 @see unloadDll

       - This function loads the DLL based on dllName if specified, otherwise the env var ECMD_DLL_FILE is used
       - Name limit of 255 characters.
       - Errors in loading are printed to STDERR.
*/
uint32_t ecmdLoadDll(std::string i_dllName);

/**
 @brief Unload the eCMD DLL
 @retval ECMD_SUCCESS if successful unload
 @retval ECMD_DLL_LOAD_FAILURE if failure occurs on call to dlclose
 @retval nonzero if failure on dll's unload
 @see loadDll

 - Errors in unloading are printed to STDERR
*/

uint32_t ecmdUnloadDll(); 

/**
 @brief Pass any unknown command line paramaters to the DLL for processing (ex. -p#, Cronus -debug)
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_argc Passed from Command line Arguments
 @param i_argv Passed from Command line Arguments
 @pre loadDll must have been called
 @post Global options (ex. -debug, -p#, -c#) will be removed from arg list
 @see loadDll

 - argc/argv get passed to the eCMD DLL.
 - Global options such as -debug flags and -p#, -c# will be parsed out.
 - Position flags can be queried later with functions like ?????
 NOTE : This function does not affect ring caching
*/

uint32_t ecmdCommandArgs(int * i_argc, char** i_argv[]); 

//@}






/** @name Query Functions */
//@{

/**
 @brief Query information about the Dll that is loaded
 @param o_dllInfo Return data with data from the current dll loaded
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure

 This interface allows you to query what particular instance of the DLL is loaded (i.e Cronus/IP/Z),
  along with additional information.
 NOTE : This function does not affect ring caching
*/

uint32_t ecmdQueryDllInfo(ecmdDllInfo & o_dllInfo);

/**
 @brief Query configuration information from the DLL
 @param i_target Struct that contains partial information to limit query results
 @param o_queryData Return data from query
 @param i_detail Specify the level of detail that should be returned with the query
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure

 The Valid bits of the target are used to refine the query

 The target paramater should be filled in with as much data as you know to limit the query, (including the chipType).
 When a field state is set to ECMD_TARGET_QUERY_WILDCARD the query function will iterate on all possible values for that
 entry and return the relevant data.
 When a field state is set to ECMD_TARGET_QUERY_IGNORE the query function will stop iterating at that level and below


 Ex: to query what positions of the Nova chip are on cage 1, node 2:<p>
   cage = 1, node = 2, pos = 'wildcard', chipType = 'Nova', core = 'wildcard', thread = 'wildcard'<p>
 Ex: to query what positions of the Nova chip are in the entire system:<p>
   cage = 'wildcard', node = 'wildcard', pos = 'wildcard', chipType = 'Nova', core = 'wildcard', thread = 'wildcard'<p>
 Ex: to query all the chips on cage 3, node 0:<p>
   cage = 3, node = 0, pos = 'wildcard', chipType = 'wildcard', core = 'wildcard', thread = 'wildcard'<p>
 Ex: to query all the chips in the entire system:<p>
   cage = 'wildcard', node = 'wildcard', pos = 'wildcard', chipType = 'wildcard', core = 'wildcard', thread = 'wildcard'<p>
 Ex: to query the total nodes in a system:<p>
   cage = 'wildcard', node = 'wildcard', pos = 'ignore', chipType = 'ignore', core = 'ignore', thread = 'ignore'<p>
 NOTE : This function does not affect ring caching
*/
   
uint32_t ecmdQueryConfig(ecmdChipTarget & i_target, ecmdQueryData & o_queryData, ecmdQueryDetail_t i_detail = ECMD_QUERY_DETAIL_HIGH);

/**
 @brief Query User Selected Targeting information from the DLL, i.e (-p#,-c#,-t#)
 @param io_target Struct that contains partial information to limit query results - chipType is unused
 @param o_queryData Return data from query
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure

 This function acts just like ecmdQueryConfig except it operates on what targets were selected by the
  user args -n#, -p#, -c#, -t#

 Use of this function is the same as ecmdQueryConfig<p>
 When -talive is specified all threads configured will be returned in o_queryData and io_target.threadState
   will be set to ECMD_TARGET_THREAD_ALIVE.
 NOTE : This function does not affect ring caching
*/
uint32_t ecmdQuerySelected(ecmdChipTarget & io_target, ecmdQueryData & o_queryData);

/**
 @brief Query Ring information from the DLL
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information of chip to use
 @param o_queryData Return list from query
 @param i_ringName if != NULL used to refine query to a single ring
 @retval ECMD_INVALID_RING if i_ringName is not valid for target
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure

 NOTE : This function does not affect ring caching
*/
uint32_t ecmdQueryRing(ecmdChipTarget & i_target, std::list<ecmdRingData> & o_queryData, const char * i_ringName = NULL);

/**
 @brief Query Array information from the DLL
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information of chip to use
 @param o_queryData Return data from query
 @param i_arrayName array to access data for
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARRAY if i_arrayName is not valid for target
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure

 NOTE : This function does not affect ring caching
*/
uint32_t ecmdQueryArray(ecmdChipTarget & i_target, ecmdArrayData & o_queryData, const char * i_arrayName);

/**
 @brief Query Spy information from the DLL
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information of chip to use
 @param o_queryData Return data from query
 @param i_spyName Spy to access data for
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_INVALID_SPY if spy name is not valid for target
 @retval nonzero on failure

 NOTE : This function does not affect ring caching
*/
uint32_t ecmdQuerySpy(ecmdChipTarget & i_target, ecmdSpyData & o_queryData, const char * i_spyName);

/**
 @brief Query the location of a specific file type for the selected target
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information
 @param i_fileType Enum that specifies which type of file you are looking for scandef/spydef/arraydef
 @param o_fileLocation Return string with full path and filename to location 
 @retval ECMD_SUCCESS if successful
 @retval ECMD_UNKNOWN_FILE if unable to find requested file
 @retval nonzero if unsuccessful

 NOTE : This function does not affect ring caching
*/
uint32_t ecmdQueryFileLocation(ecmdChipTarget & i_target, ecmdFileType_t i_fileType, std::string & o_fileLocation);


/**
 @brief Query if a particular target is configured in the system
 @param i_target Target to query in system configuration
 @param i_queryData If specified this data will be used, otherwise a call to ecmdQueryConfig will be made
 @retval true if Target is configured in system
 @retval false if Target is not configured in system

 NOTE : This function calls ecmdQueryConfig and searchs for the specified target
 NOTE : The target State fields must be filled in as either VALID or UNUSED
*/
bool ecmdQueryTargetConfigured(ecmdChipTarget i_target, ecmdQueryData * i_queryData = NULL);
//@}





/** @name Scan Functions */
//@{
/**
 @brief Scans the ring from the selected chip into the data buffer
 @retval ECMD_INVALID_RING if ringname is not valid for target
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @param i_target Struct that contains chip and cage/node/slot/position/core information of ring to read
 @param i_ringName Name of ring to read from
 @param o_data DataBuffer object that holds data read from ring
 @see putRing



*/
uint32_t getRing (ecmdChipTarget & i_target, const char * i_ringName, ecmdDataBuffer & o_data); 

/**
 @brief Scans ring from the data buffer into the selected chip in the selected ring
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_RING if ringname is not valid for target
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @param i_target Struct that contains chip and cage/node/slot/position/core information of ring to write
 @param i_ringName Name of ring to write to
 @param i_data DataBuffer object that holds data to write into ring
 @see getRing



*/
uint32_t putRing (ecmdChipTarget & i_target, const char * i_ringName, ecmdDataBuffer & i_data); 

/**
 @brief Reads the selected spy into the data buffer
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_SUCCESS if successful read
 @retval ECMD_UNABLE_TO_OPEN_SCANDEF eCMD was unable to open the scandef
 @retval ECMD_INVALID_RING if ringname is not valid for target
 @retval ECMD_INVALID_LATCHNAME if latchname not found in scandef
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core information
 @param i_latchName Name of latch to read (can be a partial or full name based on i_mode)
 @param o_data list of Entries containing all latches found matching i_latchName
 @param i_ringName Name of ring to search for latch if == NULL, entire scandef is searched
 @param i_mode LatchName search mode

 NOTE : This function is ring cache enabled

*/
uint32_t getLatch (ecmdChipTarget & i_target, const char* i_ringName, const char * i_latchName, std::list<ecmdLatchEntry> & o_data, ecmdLatchMode_t i_mode);


/**
 @brief Writes the data buffer into the all latches matching i_latchName
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_UNABLE_TO_OPEN_SCANDEF eCMD was unable to open the scandef
 @retval ECMD_INVALID_RING if ringname is not valid for target
 @retval ECMD_INVALID_LATCHNAME if latchname not found in scandef
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core information 
 @param i_latchName Name of latch to write (can be a partial or full name based on i_mode)
 @param i_data DataBuffer object that holds data to write into latch
 @param i_ringName Name of ring to search for latch if == NULL, entire scandef is searched
 @param i_mode LatchName search mode
 @param i_startBit Startbit in latchname to insert data
 @param i_numBits Number of bits to insert from startbit
 @param o_matchs Number of latchs found that matched your name and data was inserted

 NOTE : This function is ring cache enabled

*/
uint32_t putLatch (ecmdChipTarget & i_target, const char* i_ringName, const char * i_latchName, ecmdDataBuffer & i_data, uint32_t i_startBit, uint32_t i_numBits, uint32_t & o_matchs, ecmdLatchMode_t i_mode);


/**
 @brief Scans the specified number of bits from the selected chip and ring address into the data buffer
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @param i_target Struct that contains chip and cage/node/slot/position information of ring to read
 @param i_address Address of ring to read from
 @param i_bitLength Bit Length to scan for
 @param o_data DataBuffer object that holds data read from ring
 @see putRingWithModifier

 NOTE : This is a debug interface and should not be used in normal situations
 NOTE : This function does not handle processor cores for you, the i_address will be taken and used with no modifications
          so you are responsible for specifying the correct core address
 NOTE : This function will only scan for the length provided, if this length doesn't match the actual length of the ring
          corruption may occur


*/
uint32_t getRingWithModifier (ecmdChipTarget & i_target, uint32_t i_address, uint32_t i_bitLength, ecmdDataBuffer & o_data); 

/**
 @brief Scans the specified number of bits from the data buffer into the selected chip in the selected ring address
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @param i_target Struct that contains chip and cage/node/slot/position information of ring to write
 @param i_address Address of ring to write to
 @param i_bitLength Bit Length to scan for
 @param i_data DataBuffer object that holds data to write into ring
 @see getRingWithModifier

 NOTE : This is a debug interface and should not be used in normal situations
 NOTE : This function does not handle processor cores for you, the i_address will be taken and used with no modifications
          so you are responsible for specifying the correct core address
 NOTE : This function will only scan for the length provided, if this length doesn't match the actual length of the ring
          corruption may occur


*/
uint32_t putRingWithModifier (ecmdChipTarget & i_target, uint32_t i_address, uint32_t i_bitLength, ecmdDataBuffer & i_data); 



//@}




/** @name Scom Functions  */
//@{

/**
 @brief Scoms bits from the selected address into the data buffer
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core information of scom address to read
 @param i_address Scom address to read from
 @param o_data DataBuffer object that holds data read from address
 @see putScom


*/
uint32_t getScom (ecmdChipTarget & i_target, uint32_t i_address, ecmdDataBuffer & o_data);

/**
 @brief Scoms bits from the data buffer into the selected address
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core information of scom address to write
 @param i_address Scom address to write to
 @param i_data DataBuffer object that holds data to write into address
 @see getScom


*/
uint32_t putScom (ecmdChipTarget & i_target, uint32_t i_address, ecmdDataBuffer & i_data);

//@}
/* End Scom Functions */





/** @name Jtag Functions */
//@{

/**
 @brief Send a JTAG instruction and modifier to the specified chip
 @param i_target Struct that contains chip and cage/node/slot/position information of scom address to write
 @param i_instruction Right aligned instruction to send to chip
 @param i_modifier Right aligned instruction modifier to send
 @param o_status Instruction status register value retrieved
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_NON_JTAG_CHIP Chip Target is a non-jtag attached chip
 @retval nonzero if unsuccessful

 NOTE : Proper parity will be generated on the command and modifier
*/
uint32_t sendCmd (ecmdChipTarget & i_target, uint32_t i_instruction, uint32_t i_modifier, ecmdDataBuffer & o_status);

//@}
/* End Jtag Functions */





/** @name FSI Functions */
//@{

/**
 @brief Read data from the selected CFAM register address into the data buffer
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @param i_target Struct that contains chip and cage/node/slot/position information 
 @param i_address CFAM address to read from
 @param o_data DataBuffer object that holds data read from address


*/
uint32_t getCfamRegister (ecmdChipTarget & i_target, uint32_t i_address, ecmdDataBuffer & o_data);

/**
 @brief Write data into the selected CFAM register address 
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position information 
 @param i_address CFAM address to write to
 @param i_data DataBuffer object that holds data to write into address


*/
uint32_t putCfamRegister (ecmdChipTarget & i_target, uint32_t i_address, ecmdDataBuffer & i_data);

//@}
/* End FSI Functions */





/** @name Spy Functions */
//@{

/**
 @brief Reads the selected spy into the data buffer
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SPY_FAILED_ECC_CHECK if invalid ECC detected on Spy read
 @retval ECMD_INVALID_SPY Spy name is invalid or Spy is an ECC Grouping
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_SPY_IS_EDIAL Spy is an edial have to use getSpyEnum
 @retval ECMD_SPY_GROUP_MISMATCH A mismatch was found reading a group spy not all groups set the same
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core information of spy to read
 @param i_spyName Name of spy to read from
 @param o_data DataBuffer object that holds data read from spy

 NOTE : This function is ring cache enabled

*/
uint32_t getSpy (ecmdChipTarget & i_target, const char * i_spyName, ecmdDataBuffer & o_data);

/**
 @brief Reads the selected spy and returns it's assocaiated enum
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SPY_FAILED_ECC_CHECK if invalid ECC detected on Spy read - valid Spy Data still returned
 @retval ECMD_INVALID_SPY Spy name is invalid or Spy is an ECC Grouping
 @retval ECMD_INVALID_SPY_ENUM if value in hardware doesn't map to a valid enum
 @retval ECMD_SPY_NOT_ENUMERATED Spy is not enumerated must use getSpy
 @retval ECMD_SPY_GROUP_MISMATCH A mismatch was found reading a group spy not all groups set the same
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core information of spy to read
 @param i_spyName Name of spy to read from
 @param o_enumValue Enum value read from the spy

 NOTE : This function is ring cache enabled

*/
uint32_t getSpyEnum (ecmdChipTarget & i_target, const char * i_spyName, std::string & o_enumValue);

/**
 @brief Read an ECC grouping and return the in and out bits as well as a error mask if any out bits are invalid
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_INVALID_SPY Spy name is invalid or Spy is not an ECC Grouping
 @retval ECMD_SPY_FAILED_ECC_CHECK if invalid ECC detected on Spy read - valid Spy Data still returned
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core information of spy to read
 @param i_spyEpCheckersName Name of spy to read from
 @param o_inLatchData Return the data for the input to the eccGroup
 @param o_outLatchData Return the Ecc data associated with the outbits of the eccGroup
 @param o_eccErrorMask Return a mask for the Ecc data a 1 in the mask means the associated eccData was in error
 @retval nonzero if unsuccessful

 NOTE : This function is ring cache enabled
*/
uint32_t getSpyEpCheckers (ecmdChipTarget & i_target, const char * i_spyEpCheckersName, ecmdDataBuffer & o_inLatchData, ecmdDataBuffer & o_outLatchData, ecmdDataBuffer & o_eccErrorMask);

/**
 @brief Reads the selected spy and load all the spy groups into provided list
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SPY_FAILED_ECC_CHECK if invalid ECC detected on Spy read
 @retval ECMD_INVALID_SPY Spy name is invalid or Spy is an ECC Grouping
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_SPY_IS_EDIAL Spy is an edial have to use getSpyEnum
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core information of spy to read
 @param i_spyName Name of spy to read from
 @param o_groups List of structures containing the group data and deadbits mask

 NOTE : This function is ring cache enabled

*/
uint32_t getSpyGroups (ecmdChipTarget & i_target, const char * i_spyName, std::list < ecmdSpyGroupData > & o_groups);

/**
 @brief Writes the data buffer into the selected spy
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_INVALID_SPY Spy name is invalid or Spy is an ECC Grouping
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_SPY_IS_EDIAL Spy is an edial have to use putSpyEnum
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core information of spy to write
 @param i_spyName Name of spy to write to
 @param i_data DataBuffer object that holds data to write into spy

 NOTE : This function is ring cache enabled

*/
uint32_t putSpy (ecmdChipTarget & i_target, const char * i_spyName, ecmdDataBuffer & i_data);

/**
 @brief Writes the enum into the selected spy
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_INVALID_SPY Spy name is invalid or Spy is an ECC Grouping
 2retval ECMD_SPY_NOT_ENUMERATED Spy is not enumerated must use putSpy
 @retval ECMD_INVALID_SPY_ENUM if enum value specified is not valid
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core information of spy to write
 @param i_spyName Name of spy to write to
 @param i_enumValue String enum value to load into the spy

 NOTE : This function is ring cache enabled
*/
uint32_t putSpyEnum (ecmdChipTarget & i_target, const char * i_spyName, const std::string i_enumValue);

//@}



/** @name Ring Cache Functions */
//@{

/**
 @brief Enables internal caching of read/writes of scan rings to the chip for functions like getring/getspy/getspr
 @post Ring caching is enabled on cache enabled functions
 - Functions that support caching are documented in the detailed description of the function
 - Functions that do not affect the state of the cache are documented in the detailed description of the function
 - Any non-cache enabled function will force a flush of the cache before performing the operation
 - Some Dll's may not support ring caching, they will not fail on these functions but you will not see the performance gains
*/
void ecmdEnableRingCache();

/**
 @brief Disable internal caching of reads/writes of scan rings
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful

 NOTE: A Flush of the cache is performed before disabling the cache
*/
uint32_t  ecmdDisableRingCache();

/**
 @brief Flush all modified data from the internal cache to the hardware, then remove all rings from cache
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
*/
uint32_t  ecmdFlushRingCache();

/**
 @brief Returns true/false to signify if caching is currently enabled
 @retval true if ring caching is enabled
 @retval false if ring caching is disabled
*/
bool      ecmdIsRingCacheEnabled();

//@}




/** @name Array Functions */
//@{

/**
 @brief Reads bits from the selected array into the data buffer
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARRAY if i_arrayName is not valid for target
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position information of array to read
 @param i_arrayName Name of array to read from
 @param o_data DataBuffer object that holds data read from address
 @param i_address Array Address to read from - length of DataBuffer should be set to length of valid address data
 @see putArray
 @see getArrayMultiple


*/
uint32_t getArray (ecmdChipTarget & i_target, const char * i_arrayName, ecmdDataBuffer & i_address, ecmdDataBuffer & o_data);

/**
 @brief Reads bits from multiple array addresses/elements into the list of data buffers
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARRAY if i_arrayName is not valid for target
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position information of array to read
 @param i_arrayName Name of array to read from
 @param io_entries list of array entries to fetch
 @see putArray
 @see getArray

 NOTE : To use this function the io_entries list should be pre-loaded with the addresses to fetch, the associated dataBuffers will be loaded upon return

 The return value of this function is set to the first non-zero return code found when
  retrieving multiple entries.  The function will still continue through all entries requested.
  You must walk through the list returned to find out which entry caused the failure.

*/
uint32_t getArrayMultiple (ecmdChipTarget & i_target, const char * i_arrayName, std::list<ecmdArrayEntry> & io_entries);


/**
 @brief Writes bits from the data buffer into the selected array
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARRAY if i_arrayName is not valid for target
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_SUCCESS if successful
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @param i_target Struct that contains chip and cage/node/slot/position information of array to write
 @param i_arrayName Name of array to write to
 @param i_data DataBuffer object that holds data to write into array
 @param i_address Array Address to write to - length of DataBuffer should be set to length of valid address data
 @see getArray


*/
uint32_t putArray (ecmdChipTarget & i_target, const char * i_arrayName, ecmdDataBuffer & i_address, ecmdDataBuffer & i_data);

/**
 @brief Writes bits from the list of entries into the selected array
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARRAY if i_arrayName is not valid for target
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @param i_target Struct that contains chip and cage/node/slot/position information of array to write
 @param i_arrayName Name of array to write to
 @param i_entries List of addresses and data to write to chip
 @see getArray

 NOTE : i_entries should be pre-loaded with address and data

 The return value of this function is set to the first non-zero return code found when
  retrieving multiple entries.  The function will still continue through all entries requested.
  You must walk through the list returned to find out which entry caused the failure.

*/
uint32_t putArrayMultiple (ecmdChipTarget & i_target, const char * i_arrayName, std::list<ecmdArrayEntry> & i_entries);

//@}



/** @name Clock Functions */
//@{

/**
 @brief Query the state of the clocks for a domain
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_INVALID_CLOCK_DOMAIN An invalid clock domain name was specified
 @param i_target Struct that contains chip and cage/node/slot/position information
 @param i_clockDomain Clock domain to query - as defined in scandef - use "ALL" to check all domains
 @param o_clockState State of clocks for that domain
*/

uint32_t ecmdQueryClockState (ecmdChipTarget & i_target, const char * i_clockDomain, ecmdClockState_t & o_clockState);


/**
 @brief Start the clocks in the domain specified
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_INVALID_CLOCK_DOMAIN An invalid clock domain name was specified
 @retval ECMD_CLOCKS_ALREADY_ON The clocks in the specified domain are already on
 @retval ECMD_CLOCKS_IN_INVALID_STATE The clock in the specified domain are in an unknown state (not all on/off)
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @param i_target Struct that contains chip and cage/node/slot/position information
 @param i_clockDomain Clock domain to start - as defined in scandef - use "ALL" to start all domains
 @param i_forceState Force the clocks into the appropriate state - ignore if not in correct state to start

 * NOTE : i_clockDomain has to be "ALL" unless i_target refers to a specific chip
 *         if i_target refers to a Node or Cage then an individual clockDomain cannot be specified
*/
uint32_t startClocks (ecmdChipTarget & i_target, const char * i_clockDomain, bool i_forceState = false);

/**
 @brief Stop the clocks in the domain specified
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_INVALID_CLOCK_DOMAIN An invalid clock domain name was specified
 @retval ECMD_CLOCKS_ALREADY_OFF The clocks in the specified domain are already off
 @retval ECMD_CLOCKS_IN_INVALID_STATE The clock in the specified domain are in an unknown state (not all on/off)
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @param i_target Struct that contains chip and cage/node/slot/position information
 @param i_clockDomain Clock domain to stop - as defined in scandef - use "ALL" to stop all domains
 @param i_forceState Force the clocks into the appropriate state - ignore if not in correct state to start

 * NOTE : i_clockDomain has to be "ALL" unless i_target refers to a specific chip
 *         if i_target refers to a Node or Cage then an individual clockDomain cannot be specified
*/
uint32_t stopClocks (ecmdChipTarget & i_target, const char * i_clockDomain, bool i_forceState = false);

//@}


/** @name iSteps Functions */
//@{


/**
 @brief Run iSteps by number
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_ISTEPS_INVALID_STEP An invalid step number was provided
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @post iSteps specified  are complete
 @param i_steps Bit mask defining which steps to run

 NOTE - function returns on first failure and remaining steps are not run

*/
uint32_t iStepsByNumber (ecmdDataBuffer & i_steps);

/**
 @brief Run a single iStep by name
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_ISTEPS_INVALID_STEP An invalid step name was provided
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @post iStep specified is complete
 @param i_stepName List of iStep names to run

*/
uint32_t iStepsByName(std::string i_stepName);

/**
 @brief Run multiple iSteps by name
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_ISTEPS_INVALID_STEP An invalid step name was provided
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @post iSteps specified  are complete
 @param i_stepNames List of iStep names to run

 NOTE - Steps are run in order as is appropriate for proper system configuration, not by order provided in list
 NOTE - function returns on first failure and remaining steps are not run
*/
uint32_t iStepsByNameMultiple(std::list< std::string > i_stepNames);

/**
 @brief Run all iSteps by name starting with i_stepNameBegin and ending with i_stepNameEnd
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_ISTEPS_INVALID_STEP An invalid step name was provided
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @post iSteps specified  are complete
 @param i_stepNameBegin Starting iStep to run
 @param i_stepNameEnd Ending iStep to run

 NOTE - function returns on first failure and remaining steps are not run
*/
uint32_t iStepsByNameRange(std::string  i_stepNameBegin, std::string  i_stepNameEnd);


//@}





/** @name Processor Functions */
//@{


/**
 @brief Query Information about a Processor Register (SPR/GPR/FPR)
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information
 @param i_name Name of the Register to fetch data about (can be either a specific SPR or GPR/FPR)
 @param o_data Data retrieved about the register
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_SPR Spr name is invalid
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
*/
uint32_t ecmdQueryProcRegisterInfo(ecmdChipTarget & i_target, const char* i_name, ecmdProcRegisterInfo & o_data);

/**
 @brief Reads the selected Processor Architected Special Purpose Register (SPR) into the data buffer
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_SPR Spr name is invalid
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information
 @param i_sprName Name of spr to read from
 @param o_data DataBuffer object that holds data read from spr


*/
uint32_t getSpr (ecmdChipTarget & i_target, const char * i_sprName, ecmdDataBuffer & o_data);

/**
 @brief Reads the selected Processor Architected Special Purpose Register (SPR) into the data buffer
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_SPR Spr name is invalid
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information
 @param io_entries List of entries to fetch ecmdNameEntry.name field must be filled in

 - NOTE : There are special keywords that can be specified to fetch groups of entries, they
          are used by adding only an entry to io_entries and setting ecmdNameEntry.name = <keyword>
   - "ALLTHREADED" : To fetch all threaded (replicated) SPR's for particular target
   - "ALLSHARED"   : To fetch all non-threaded SPR's for particular target

 The return value of this function is set to the first non-zero return code found when
  retrieving multiple entries.  The function will still continue through all entries requested.
  You must walk through the list returned to find out which entry caused the failure.

*/
uint32_t getSprMultiple (ecmdChipTarget & i_target, std::list<ecmdNameEntry> & io_entries);


/**
 @brief Writes the data buffer into the selected Processor Architected Special Purpose Register (SPR)
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_INVALID_SPR Spr name is invalid
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information 
 @param i_sprName Name of spr to write to
 @param i_data DataBuffer object that holds data to write into spr


*/
uint32_t putSpr (ecmdChipTarget & i_target, const char * i_sprName, ecmdDataBuffer & i_data);

/**
 @brief Writes the data buffer into the selected Processor Architected Special Purpose Register (SPR)
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_INVALID_SPR Spr name is invalid
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information 
 @param i_entries List of entries to write all ecmdNameEntry fields must be filled in

 The return value of this function is set to the first non-zero return code found when
  retrieving multiple entries.  The function will still continue through all entries requested.
  You must walk through the list returned to find out which entry caused the failure.

*/
uint32_t putSprMultiple (ecmdChipTarget & i_target, std::list<ecmdNameEntry> & i_entries);

/**
 @brief Reads the selected Processor Architected General Purpose Register (GPR) into the data buffer
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_GPR Gpr number is invalid
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information
 @param i_gprNum Number of gpr to read from
 @param o_data DataBuffer object that holds data read from gpr


*/
uint32_t getGpr (ecmdChipTarget & i_target, uint32_t i_gprNum, ecmdDataBuffer & o_data);

/**
 @brief Reads the selected Processor Architected General Purpose Register (GPR) into the data buffer
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_GPR Gpr number is invalid
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information
 @param io_entries List of entries to fetch ecmdIndexEntry.index field must be filled in

 The return value of this function is set to the first non-zero return code found when
  retrieving multiple entries.  The function will still continue through all entries requested.
  You must walk through the list returned to find out which entry caused the failure.


*/
uint32_t getGprMultiple (ecmdChipTarget & i_target, std::list<ecmdIndexEntry> & io_entries);


/**
 @brief Writes the data buffer into the selected Processor Architected General Purpose Register (GPR)
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_GPR Gpr number is invalid
 @retval ECMD_SUCCESS if successful
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information 
 @param i_gprNum Number of gpr to write to
 @param i_data DataBuffer object that holds data to write into gpr


*/
uint32_t putGpr (ecmdChipTarget & i_target, uint32_t i_gprNum, ecmdDataBuffer & i_data);

/**
 @brief Writes the data buffer into the selected Processor Architected General Purpose Register (GPR)
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_GPR Gpr number is invalid
 @retval ECMD_SUCCESS if successful
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information 
 @param i_entries List of entries to write all ecmdIndexEntry fields must be filled in


 The return value of this function is set to the first non-zero return code found when
  retrieving multiple entries.  The function will still continue through all entries requested.
  You must walk through the list returned to find out which entry caused the failure.


*/
uint32_t putGprMultiple (ecmdChipTarget & i_target, std::list<ecmdIndexEntry> & i_entries);

/**
 @brief Reads the selected Processor Architected Floating Point Register (FPR) into the data buffer
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_INVALID_FPR Fpr number is invalid
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information
 @param i_fprNum Number of fpr to read from
 @param o_data DataBuffer object that holds data read from fpr


*/
uint32_t getFpr (ecmdChipTarget & i_target, uint32_t i_fprNum, ecmdDataBuffer & o_data);

/**
 @brief Reads the selected Processor Architected Floating Point Register (FPR) into the data buffer
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_FPR Fpr number is invalid
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information
 @param io_entries List of entries to fetch ecmdIndexEntry.index field must be filled in

 The return value of this function is set to the first non-zero return code found when
  retrieving multiple entries.  The function will still continue through all entries requested.
  You must walk through the list returned to find out which entry caused the failure.

*/
uint32_t getFprMultiple (ecmdChipTarget & i_target, std::list<ecmdIndexEntry> & io_entries);

/**
 @brief Writes the data buffer into the selected Processor Architected Floating Point Register (FPR)
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_INVALID_FPR Fpr number is invalid
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information 
 @param i_fprNum Number of fpr to write to
 @param i_data DataBuffer object that holds data to write into fpr


*/
uint32_t putFpr (ecmdChipTarget & i_target, uint32_t i_fprNum, ecmdDataBuffer & i_data);

/**
 @brief Writes the data buffer into the selected Processor Architected Floating Point Register (FPR)
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_FPR Fpr number is invalid
 @retval ECMD_SUCCESS if successful
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core/thread information 
 @param i_entries List of entries to write all ecmdIndexEntry fields must be filled in

 The return value of this function is set to the first non-zero return code found when
  retrieving multiple entries.  The function will still continue through all entries requested.
  You must walk through the list returned to find out which entry caused the failure.


*/
uint32_t putFprMultiple (ecmdChipTarget & i_target, std::list<ecmdIndexEntry> & i_entries);


//@}
/* End Processor Functions */



/** @name Trace Array Functions */
//@{

/**
 @brief Dump all entries of specified trace array
 @param i_target Target info to specify what to configure (target states must be set)
 @param i_name Name of trace array - names may vary for each product/chip
 @param o_data Vector of trace array data retrieved
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
*/
uint32_t getTraceArray(ecmdChipTarget & i_target, const char* i_name, std::vector <ecmdDataBuffer> & o_data);

/**
 @brief Dump all entries of specified trace array
 @param i_target Target info to specify what to configure (target states must be set)
 @param o_data List of trace array data retrieved
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful

 The return value of this function is set to the first non-zero return code found when
  retrieving multiple entries.  The function will still continue through all entries requested.
  You must walk through the list returned to find out which entry caused the failure.

 - NOTE : to fetch all Trace Arrays available add only one entry to io_entries and set
    ecmdNameVectorEntry.name = "ALL"
*/
uint32_t getTraceArrayMultiple(ecmdChipTarget & i_target, std::list <ecmdNameVectorEntry> & o_data);

//@}



/** @name Memory Functions */
//@{

/**
 @brief Reads System Mainstore through the processor chip using a real address
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position information
 @param i_address Starting address to read from
 @param i_bytes Number of bytes to write
 @param o_data DataBuffer object that holds data read from memory


*/
uint32_t getMemProc (ecmdChipTarget & i_target, uint64_t i_address, uint32_t i_bytes, ecmdDataBuffer & o_data);


/**
 @brief Writes System Mainstore through the processor chip using a real address
 @retval ECMD_TARGET_INVALID_TYPE if target is not a processor
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position information 
 @param i_address Starting address to write to
 @param i_bytes Number of bytes to write
 @param i_data DataBuffer object that holds data to write into memory


*/
uint32_t putMemProc (ecmdChipTarget & i_target, uint64_t i_address, uint32_t i_bytes, ecmdDataBuffer & i_data);

/**
 @brief Reads System Mainstore through the PSI or DMA interface (whichever is avialable) using a real address
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains cage/node information
 @param i_address Starting address to read from
 @param i_bytes Number of bytes to write
 @param o_data DataBuffer object that holds data read from memory


*/
uint32_t getMemDma (ecmdChipTarget & i_target, uint64_t i_address, uint32_t i_bytes, ecmdDataBuffer & o_data);


/**
 @brief Writes System Mainstore through the PSI or DMA interface (whichever is avialable) using a real address
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that contains cage/node information
 @param i_address Starting address to write to
 @param i_bytes Number of bytes to write
 @param i_data DataBuffer object that holds data to write into memory


*/
uint32_t putMemDma (ecmdChipTarget & i_target, uint64_t i_address, uint32_t i_bytes, ecmdDataBuffer & i_data);

/**
 @brief Reads System Mainstore through the memory controller using a real address
 @retval ECMD_TARGET_INVALID_TYPE if target is not a memory controller
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position information
 @param i_address Starting address to read from
 @param i_bytes Number of bytes to write
 @param o_data DataBuffer object that holds data read from memory

 WARNING : This operation is typically not cache-coherent

*/
uint32_t getMemMemCtrl (ecmdChipTarget & i_target, uint64_t i_address, uint32_t i_bytes, ecmdDataBuffer & o_data);


/**
 @brief Writes System Mainstore through the memory controller using a real address
 @retval ECMD_TARGET_INVALID_TYPE if target is not a memory controller
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position information 
 @param i_address Starting address to write to
 @param i_bytes Number of bytes to write
 @param i_data DataBuffer object that holds data to write into memory

 WARNING : This operation is typically not cache-coherent

*/
uint32_t putMemMemCtrl (ecmdChipTarget & i_target, uint64_t i_address, uint32_t i_bytes, ecmdDataBuffer & i_data);


//@}
/* End Memory Functions */




/** @name Simulation Functions */
//@{
#ifndef REMOVE_SIM

/**
 @brief Enable/Disable Simulation AET Logging
 @param i_function Should be either 'on'/'off'/'flush'
 @retval ECMD_SUCCESS if successful
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval nonzero on failure
*/
uint32_t simaet(const char* i_function);

/**
 @brief Store a checkpoint to specified file
 @param i_checkpoint Name of checkpoint to write to
 @retval ECMD_SUCCESS if successful
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval nonzero on failure
*/
uint32_t simcheckpoint(const char* i_checkpoint);

/**
 @brief Clock the model
 @param i_cycles Number of cycles to clock model
 @retval ECMD_SUCCESS if successful
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval nonzero on failure
*/
uint32_t simclock(uint32_t i_cycles);

/**
 @brief Echo message to stdout and sim log
 @param i_message Message to echo
 @retval ECMD_SUCCESS if successful
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval nonzero on failure
*/
uint32_t simecho(const char* i_message);

/**
 @brief Close down the simulation model
 @param i_rc [Optional] Send a testcase failure return code to the simulation
 @param i_message [Optional[ Send a testcase failure message to the simulation
 @retval ECMD_SUCCESS if successful
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval nonzero on failure
*/
uint32_t simexit(uint32_t i_rc = 0, const char* i_message = NULL);

/**
 @brief Perform expect on facility using name
 @param i_facname Facility name
 @param i_expect Value to expect on facility
 @param i_bitlength Length of data to expect
 @param i_row Optional: Array Facility row
 @param i_offset Optional: Facility offset
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
uint32_t simEXPECTFAC(const char* i_facname, uint32_t i_bitlength, ecmdDataBuffer & i_expect, uint32_t i_row = 0, uint32_t i_offset = 0);

/**
 @brief Perform expect on TCFAC facility
 @param i_tcfacname Facility name
 @param i_expect Value to expect on facility
 @param i_bitlength Length of data to expect
 @param i_row Optional: Array Facility row
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
uint32_t simexpecttcfac(const char* i_tcfacname, uint32_t i_bitlength, ecmdDataBuffer & i_expect, uint32_t i_row = 0);

/**
 @brief Fetch current model cycle count
 @param o_cyclecount Current model cycle count
 @retval ECMD_SUCCESS if successful
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval nonzero on failure
*/
uint32_t simgetcurrentcycle(uint32_t & o_cyclecount);

/**
 @brief Retrieve a Facility using a name
 @param i_facname Facility name
 @param i_bitlength Bit length to read from facility
 @param o_data Data read from facility
 @param i_row Optional: Array row
 @param i_offset Optional : Facility offset
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
uint32_t simGETFAC(const char* i_facname, uint32_t i_bitlength, ecmdDataBuffer & o_data, uint32_t i_row = 0, uint32_t i_offset = 0);

/**
 @brief Retrieve a Facility using a name - preserving Xstate
 @param i_facname Facility name
 @param i_bitlength Bit length to read from facility
 @param o_data Data read from facility
 @param i_row Optional: Array row
 @param i_offset Optional : Facility offset
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
uint32_t simGETFACX(const char* i_facname, uint32_t i_bitlength, ecmdDataBuffer & o_data, uint32_t i_row = 0, uint32_t i_offset = 0);

/**
 @brief Retrieve a TCFAC facility
 @param i_tcfacname TCFAC name
 @param o_data Value read
 @param i_row Optional: Array Facility row
 @param i_startbit Optional: Startbit to read
 @param i_bitlength Optional: Length of data to read
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
uint32_t simgettcfac(const char* i_tcfacname, ecmdDataBuffer & o_data, uint32_t i_row = 0, uint32_t i_startbit = 0, uint32_t i_bitlength = 0);

/**
 @brief Initialize the simulation
 @param i_checkpoint Checkpoint to load : 'none' to skip
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
uint32_t siminit(const char* i_checkpoint);


/**
 @brief Poll a facility waiting for expected valud
 @param i_facname Facility name
 @param i_bitlength Bit length to expect
 @param i_expect Data to expect in facility
 @param i_row Optional: Array row
 @param i_offset Optional : Facility offset
 @param i_maxcycles Optional : Maximum number of cycles to run 
 @param i_pollinterval Option : Number of clock cycles to run between each poll
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_POLLING_FAILURE Polling completed without reaching expected value
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
uint32_t simPOLLFAC(const char* i_facname, uint32_t i_bitlength, ecmdDataBuffer & i_expect, uint32_t i_row = 0, uint32_t i_offset = 0, uint32_t i_maxcycles = 1, uint32_t i_pollinterval = 1);

/**
 @brief Write a Facility using a name
 @param i_facname Facility name
 @param i_bitlength Bit length to write to facility
 @param i_data Data to write
 @param i_row Optional: Array row
 @param i_offset Optional : Facility offset
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
uint32_t simPUTFAC(const char* i_facname, uint32_t i_bitlength, ecmdDataBuffer & i_data, uint32_t i_row = 0, uint32_t i_offset = 0);

/**
 @brief Write a Facility using a name - preserving Xstate
 @param i_facname Facility name
 @param i_bitlength Bit length to write to facility
 @param i_data Data to write
 @param i_row Optional: Array row
 @param i_offset Optional : Facility offset
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
uint32_t simPUTFACX(const char* i_facname, uint32_t i_bitlength, ecmdDataBuffer & i_data, uint32_t i_row = 0, uint32_t i_offset = 0);

/**
 @brief Write a TCFAC facility
 @param i_tcfacname TCFAC name
 @param i_data Value to write
 @param i_row Optional: Array Facility row
 @param i_numrows Optional: Number of rows to write
 @param i_bitlength Bit length to write to facility
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
uint32_t simputtcfac(const char* i_tcfacname, uint32_t i_bitlength, ecmdDataBuffer & i_data, uint32_t i_row = 0, uint32_t i_numrows = 0);

/**
 @brief Load a checkpoint into model
 @param i_checkpoint Name of checkpoint
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
uint32_t simrestart(const char* i_checkpoint);

/**
 @brief Stick a Facility using a name
 @param i_facname Facility name
 @param i_bitlength Bit length to stick to facility
 @param i_data Data to stick
 @param i_row Optional: Array row
 @param i_offset Optional : Facility offset
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
uint32_t simSTKFAC(const char* i_facname, uint32_t i_bitlength, ecmdDataBuffer & i_data, uint32_t i_row = 0, uint32_t i_offset = 0);

/**
 @brief Stick a TCFAC facility
 @param i_tcfacname TCFAC name
 @param i_data Value to stick
 @param i_row Optional: Array Facility row
 @param i_numrows Optional: Number of rows to stick
 @param i_bitlength Bit length to write to facility
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
uint32_t simstktcfac(const char* i_tcfacname, uint32_t i_bitlength, ecmdDataBuffer & i_data, uint32_t i_row = 0, uint32_t i_numrows = 0);

/**
 @brief Run RTX SUBCMD
 @param i_command Command
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
uint32_t simSUBCMD(const char* i_command);


/**
 @brief Set TCK Interval setting in the model for JTAG Master
 @param i_tckinterval new setting for tck interval when using JTAG
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
uint32_t simtckinterval(uint32_t i_tckinterval);



/**
 @brief Unstick a Facility using a name
 @param i_facname Facility name
 @param i_bitlength Bit length to unstick to facility
 @param i_row Optional: Array row
 @param i_offset Optional : Facility offset
 @retval ECMD_SUCCESS if successful
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval nonzero on failure
*/
uint32_t simUNSTICK(const char* i_facname, uint32_t i_bitlength, uint32_t i_row = 0, uint32_t i_offset = 0);

/**
 @brief Unstick a TCFAC facility
 @param i_tcfacname TCFAC name
 @param i_data Value to unstick to 
 @param i_row Optional: Array Facility row
 @param i_numrows Optional: Number of rows to unstick
 @param i_bitlength Bit length to unstick to facility
 @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
uint32_t simunsticktcfac(const char* i_tcfacname, uint32_t i_bitlength, ecmdDataBuffer & i_data, uint32_t i_row = 0, uint32_t i_numrows = 0); 


/**
 @brief Fetch the hierarchy for the specified chip target relative to the latch names in the scandef
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/slot/position/core information
 @param o_hierarchy Return the model hierarchy for this target

NOTE - To retrieve the hierarchy of a processor core the core field must be set and the state set to ECMD_TARGET_QUERY_FIELD_VALID
*/
uint32_t simGetHierarchy(ecmdChipTarget & i_target, std::string & o_hierarchy);

#endif /* ifndef REMOVE_SIM */
//@}
/* End Simulation Functions */


/** @name Error Handling Functions */
//@{

/**
 @brief Retrieve additional error information for errorcode
 @param i_errorCode Error code to lookup up message for
 @param i_parseReturnCode If true will search through return codes definitions to return define name of error code
 @retval point to NULL terminated string containing error data, NULL if error occurs

*/
std::string ecmdGetErrorMsg(uint32_t i_errorCode, bool i_parseReturnCode = true);

/**
 @brief Register an Error Message that has occured

*/
uint32_t ecmdRegisterErrorMsg(uint32_t i_errorCode, const char* i_whom, const char* i_message);

//@}




/** @name Output Functions */
//@{

/**
 @brief Output a message related to an error
 @param i_message String to output

*/
void ecmdOutputError(const char* i_message);

/**
 @brief Output a message related to an warning
 @param i_message String to output

*/
void ecmdOutputWarning(const char* i_message);

/**
 @brief Output a message to the screen or logs
 @param i_message String to output

*/
void ecmdOutput(const char* i_message);

//@}

/** @name Misc Functions */
//@{

/**
 @brief Retrieve the value of some ecmdGlobalVars
 @param i_type Specifies which global var you are looking for
 @retval Value of global var

*/
uint32_t ecmdGetGlobalVar(ecmdGlobalVarType_t i_type);


/**
 @brief Enable/Disable a trace mode
 @param i_type Specifies which trace mode to enable
 @param i_enable Enable or disable
*/
void ecmdSetTraceMode(ecmdTraceType_t i_type, bool i_enable);

/**
 @brief Query the state of a trace mode
 @param i_type Specifies which trace mode to query
 @retval Value of trace mode enable
*/
bool ecmdQueryTraceMode(ecmdTraceType_t i_type);

/** 
 @brief Function to delay a procedure either by running sim cycles or by doing a millisecond delay
 @param i_simCycles Number of sim cycles to run in simulation mode
 @param i_msDelay Number of milliseconds to delay in hardware mode
*/
uint32_t ecmdDelay(uint32_t i_simCycles, uint32_t i_msDelay);



//@}


/** @name Configuration Functions */
//@{

/**
 @brief Retrieve the value of a Configuration Setting
 @param i_target struct that contains chip and cage/node/slot/position/core information if necessary
 @param i_name Name of setting as defined by eCMD Api
 @param o_validOutput Indicator if o_valueAlpha, o_valueNumeric (or both) are valid.
 @param o_valueAlpha Alpha value of setting (if appropriate)
 @param o_valueNumeric Numeric value of setting (if appropriate)
 @retval ECMD_INVALID_CONFIG_NAME Name specified is not valid
 @retval ECMD_SUCCESS if successful
*/

uint32_t ecmdGetConfiguration(ecmdChipTarget & i_target, std::string i_name, ecmdConfigValid_t & o_validOutput, std::string & o_valueAlpha, uint32_t & o_valueNumeric);

/**
 @brief Set the value of a Configuration Setting
 @param i_target struct that contains chip and cage/node/slot/position/core information if necessary
 @param i_name Name of setting as defined by eCMD Api
 @param i_validInput Indicator if i_valueAlpha, i_valueNumeric (or both) are valid.
 @param i_valueAlpha Alpha value of setting (if appropriate)
 @param i_valueNumeric Numeric value of setting (if appropriate)
 @retval ECMD_DBUF_INVALID_DATA_FORMAT Value is not in correct format for specified configuration setting
 @retval ECMD_INVALID_CONFIG_NAME Name specified is not valid
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/

uint32_t ecmdSetConfiguration(ecmdChipTarget & i_target, std::string i_name, ecmdConfigValid_t i_validInput, std::string & i_valueAlpha, uint32_t i_valueNumeric);


/**
 @brief Deconfigure a target in the system
 @param i_target Target info to specify what to deconfigure (target states must be set)
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
uint32_t ecmdDeconfigureTarget(ecmdChipTarget & i_target);

/**
 @brief Configure a target in the system - must be previously known to the system
 @param i_target Target info to specify what to configure (target states must be set)
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
uint32_t ecmdConfigureTarget(ecmdChipTarget & i_target);

//@}



}  //extern "C"


#endif /* ecmdClientCapi_H */

/*** -------------------------------------- ***/
/*** The following is doxygen documentation ***/
/*** -------------------------------------- ***/

/**
 * @mainpage

@section int Introduction

Common Hardware Access Programming Interface (eCMD)

This is the documentation of the eCMD C/C++ Programming Api

@section inc eCMD Core Include Files

To compile client code to use the C++ API, the following header files are required:
<ul>
<li> ecmdClientCapi.H
<li> ecmdDataBuffer.H
<li> ecmdStructs.H
<li> ecmdReturnCodes.H
<li> ecmdUtils.H
<li> ecmdSharedUtils.H
</ul>

@section link Link objects
To link the client code on AIX, the following is required:
<ul>
<li>ecmdClientCapi_aix.a
<li>libecmd_aix.so
<li>xlC v5.0
</ul>
To create Linux x86 binaries, the following is required:
<ul>
<li>ecmdClientCapi_x86.a
<li>libecmd_x86.so
<li>g++ ????
</ul>

<hr>
@section ext eCMD Extensions
These are extensions to the core eCMD interface, not all eCMD Plugins support these extensions.
@subsection cip CIP (Cronus/IP) Extension
<ul>
This extensions provides interfaces to start/stop processor instructions and breakpoint handling.<br>
Include files :
<ul>
<li> cipClientCapi.H</li>
<li> cipStructs.H</li>
</ul>
</ul>
@subsection cronus Cronus Extension
<ul>
This extensions provides Cronus only interfaces.<br>
Include files :
<ul>
<li> croClientCapi.H</li>
<li> croStructs.H</li>
</ul>
</ul>

<hr>
@section dllv DLL Version
The eCMD Capi client code is built with a ECMD_CAPI_VERSION that gets passed into the DLL with the initDll function. If the version passed in does not match the version compiled into the DLL, the init will fail. The programmer needs to get a new copy of the .a archive and rebuild there client to correct this problem.

<br>
<hr>
@section bvc The ecmdDataBuffer class
Data is passed between the client and the DLL with the ecmdDataBuffer class.  The ecmdDataBuffer object is linked on both the client side and the DLL side.

The ecmdDataBuffer maintains data both as unsigned integers and as a character string.  The class contains methods for accessing and modifying data as well as converting data to strings (e.g. hex, left-aligned).  The ecmdDataBuffer class allocates the memory for the conversion-to-string routines and returns a char* pointer to the memory.  The client should allocate its own memory and do a strcpy if the string is to be preserved upon the next ecmdDataBuffer conversion-to-string call.

<br>
<hr>
@section makefile Makefile Example
These examples assume you linked to the required files in a subdir called dll.<p>
For Cronus these files can be found in your location at .../cronus/ecommon/dll<p>
@subsection Aixmake Aix
<pre>
testclient: testclient.o dll/ecmdClientCapi_aix.a
	xlC -+ -g -brtl -L../capi/export -lecmd_aix testclient.o dll/ecmdClientCapi_aix.a -o testclient

testclient.o: testclient.C dll/ecmdClientCapi.H dll/ecmdDataBuffer.H dll/ecmdReturnCodes.H dll/ecmdStructs.H dll/ecmdUtils.H
	xlC -+ -g -c -Idll/ testclient.C -o testclient.o
</pre>

@subsection Linuxmake Linux x86
<pre>
testclient.linux: testclient_linux.o dll/ecmdClientCapi_x86.a
	g++ -g -ldl -L../capi/export -lecmd_x86 testclient_linux.o dll/ecmdClientCapi_x86.a -o testclient.linux

testclient_linux.o: testclient.c dll/ecmdClientCapi.H dll/ecmdDataBuffer.H dll/ecmdReturnCodes.H dll/ecmdStructs.H 
	g++ -g -c -Idll/  -ftemplate-depth-30 testclient.c -o testclient_linux.o
</pre>


<hr>
@section example Example


<pre>
&#35;include <list>
&#35;include <string>

&#35;include <ecmdClientCapi.H>
&#35;include <ecmdDataBuffer.H>


int main (int argc, char *argv[])
{

  // A buffer to store our data
  ecmdDataBuffer data;          
  uint32_t rc = 0;
  // This is the chip target to operate on
  ecmdChipTarget target;       


  // Load and initialize the eCMD Dll 
  // Which DLL to load is determined by the ECMD_DLL_FILE environment variable 
  rc = ecmdLoadDll("");
  if (rc) {
    printf("**** ERROR : Problems loading eCMD Dll!");
    return rc;
  }

  // Pass your arguments to the Dll so it can parse out any common args 
  // Common args like -p# -c# will be removed from arg list upon return 
  rc = ecmdCommandArgs(&argc, &argv); 
  if (rc) return rc;

  // Let's setup our target 
  target.cage = target.node = target.slot = 0;
  target.chipType = "pu";
  target.pos = target.core = 0;
  // We have to tell the Dll what type of target we are querying
  // We are not dealing with cores here so let the Dll know we want to know everything above that
  target.coreState = ECMD_TARGET_FIELD_UNUSED

  // Is this target configured ? 
  if (ecmdQueryTargetConfigured(target)) {
    printf("pu 0:0 is configured");
  } else {
    printf("**** ERROR : pu 0:0 is not configured, unable to complete test");
    return 1;
  }

  // ----------------- 
  // Ring's            
  // ----------------- 
  rc = getRing (target, "sgxbs", data);
  if (rc) return rc;
  printf("Scanned ring sgxbs - length = %d",data.getBitLength());

  // We need to set a few bits
  // Set an entire word
  data.setWord(1, 0xFEEDBEEF);  
  // Set bit 2
  data.setBit(2);               
  // Set bits 5-9
  data.setBit(5,5);             
  // Clear bit 12
  data.clearBit(12);            

  // Scan the ring back in 
  rc = putRing (target, "sgxbs", data);
  if (rc) return rc;


  // ----------------- 
  // Spy's             
  // ----------------- 
  // We will enable ring caching this will reduce the scans to the hardware 
  ecmdEnableRingCache();

  // First we will try a non-enumerated spy 
  rc = getSpy (target, "MYSPY", data);
  if (rc) return rc;
  data.setWord(0,0xAAAAAAAA);
  rc = putSpy (target, "MYSPY", data);
  if (rc) return rc;

  // Now an enumerated spy 
  std::string enumval;
  rc = getSpyEnum (target, "MYENUMSPY", enumval);
  if (rc) return rc;
  printf("pu 0:0 MYENUMSPY is set to : %s",enumval.c_str());
  rc = putSpyEnum (target, "MYENUMSPY", "ENABLE");
  if (rc) return rc;

  // Now that we are done with that, flush all the rings to the hardware that were modified 
  rc = ecmdDisableRingCache();
  if (rc) return rc;


  // ----------------- 
  // Scom's            
  // ----------------- 
  
  rc = getScom (target, 0x800003, data);
  if (rc) return rc;
  printf("pu 0:0 800003 %.08X %.08X",data.getWord(0),data.getWord(1));
  data.setWord(1,0x5555AAAA);
  rc = putScom (target, 0x800003, data);
  if (rc) return rc;


  // -------------- 
  // Config Looping 
  // -------------- 
  // I want to loop on all the pu chips that the user selected with -p# -n#
  // Looping on selected positions only works when ecmdCommandArgs has been previously called 

  // Setup the target we will use
  // We want to loop on all 'pu' chips so set that, everything else is wildcard
  target.chipType = "pu";
  target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  // For the function we are doing we know that we don't care about threads 
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  bool validPosFound = false;
  ecmdLooperData looperdata;
  // Initialize the config looper, tell it to loop on targets selected by the user -p# -c# 
  // To loop on all targets in the system, not just those selected change this to : ECMD_ALL_TARGETS_LOOP 
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  // This loop will continue as long as valid targets are found 
  //  each time returning with the target variable filled it 
  while ( ecmdConfigLooperNext(target, looperdata) ) {

    // We will dump all the idregs 
    rc = getRing(target, "idreg", data);
    printf("Idreg for %s : 0x%.08X", ecmdWriteTarget(target).c_str(), data.getWord(0));

    // Signify that we looped at least once 
    validPosFound = true;
  }
  if (!validPosFound) {
    // We never went into the while loop this means the positions the user selected where not in the system 
    printf("**** ERROR : Position selected was not valid");
  }


  // Unload the eCMD Dll, this should always be the last thing you do 
  ecmdUnloadDll();

  return rc;

}
</pre>


*/

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
