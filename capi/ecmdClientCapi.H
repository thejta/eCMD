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
int ecmdLoadDll(std::string i_dllName);

/**
 @brief Unload the eCMD DLL
 @retval ECMD_SUCCESS if successful unload
 @retval ECMD_DLL_LOAD_FAILURE if failure occurs on call to dlclose
 @retval nonzero if failure on dll's unload
 @see loadDll

 - Errors in unloading are printed to STDERR
*/

int ecmdUnloadDll(); 

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

int ecmdCommandArgs(int * i_argc, char** i_argv[]); 

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

int ecmdQueryDllInfo(ecmdDllInfo & o_dllInfo);


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
   cage = 1, node = 2, pos = <wildcard>, chipType = 'Nova', core = <wildcard>, thread = <wildcard><p>
 Ex: to query what positions of the Nova chip are in the entire system:<p>
   cage = <wildcard>, node = <wildcard>, pos = <wildcard>, chipType = 'Nova', core = <wildcard>, thread = <wildcard><p>
 Ex: to query all the chips on cage 3, node 0:<p>
   cage = 3, node = 0, pos = <wildcard>, chipType = <wildcard>, core = <wildcard>, thread = <wildcard><p>
 Ex: to query all the chips in the entire system:<p>
   cage = <wildcard>, node = <wildcard>, pos = <wildcard>, chipType = <wildcard>, core = <wildcard>, thread = <wildcard><p>
 Ex: to query the total nodes in a system:<p>
   cage = <wildcard>, node = <wildcard>, pos = <ignore>, chipType = <ignore>, core = <ignore>, thread = <ignore><p>
 NOTE : This function does not affect ring caching
*/
   
int ecmdQueryConfig(ecmdChipTarget & i_target, ecmdQueryData & o_queryData, ecmdQueryDetail_t i_detail = ECMD_QUERY_DETAIL_HIGH);

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
int ecmdQuerySelected(ecmdChipTarget & io_target, ecmdQueryData & o_queryData);

/**
 @brief Query Ring information from the DLL
 @param i_target Struct that contains chip and cage/node/position/core/thread information of chip to use
 @param o_queryData Return list from query
 @param i_ringName if != NULL used to refine query to a single ring
 @retval ECMD_INVALID_RING if i_ringName is not valid for target
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
 NOTE : This function does not affect ring caching
*/
int ecmdQueryRing(ecmdChipTarget & i_target, std::list<ecmdRingData> & o_queryData, const char * i_ringName = NULL);

/**
 @brief Query Array information from the DLL
 @param i_target Struct that contains chip and cage/node/position/core/thread information of chip to use
 @param o_queryData Return data from query
 @param i_arrayName array to access data for
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARRAY if i_arrayName is not valid for target
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
 NOTE : This function does not affect ring caching
*/
int ecmdQueryArray(ecmdChipTarget & i_target, ecmdArrayData & o_queryData, const char * i_arrayName);

/**
 @brief Query Spy information from the DLL
 @param i_target Struct that contains chip and cage/node/position/core/thread information of chip to use
 @param o_queryData Return vector from query
 @param i_spyName Spy to access data for
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_INVALID_SPY if spy name is not valid for target
 @retval nonzero on failure
 NOTE : This function does not affect ring caching
*/
int ecmdQuerySpy(ecmdChipTarget & i_target, std::list<ecmdSpyData> & o_queryData, const char * i_spyName);

/**
 @brief Query the location of a specific file type for the selected target
 @param i_target Struct that contains chip and cage/node/position/core/thread information
 @param i_fileType Enum that specifies which type of file you are looking for scandef/spydef/arraydef
 @param o_fileLocation Return string with full path and filename to location 
 @retval ECMD_SUCCESS if successful
 @retval ECMD_UNKNOWN_FILE if unable to find requested file
 @retval nonzero if unsuccessful
 NOTE : This function does not affect ring caching
*/
int ecmdQueryFileLocation(ecmdChipTarget & i_target, ecmdFileType_t i_fileType, std::string & o_fileLocation);


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
 @brief Scans the selected number of bits from the selected position in the selected ring into the data buffer
 @retval ECMD_INVALID_RING if ringname is not valid for target
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/position/core/thread information of ring to read
 @param i_ringName Name of ring to read from
 @param o_data DataBuffer object that holds data read from ring
 @see putRing
 NOTE : This function is ring cache enabled


*/
int getRing (ecmdChipTarget & i_target, const char * i_ringName, ecmdDataBuffer & o_data); 

/**
 @brief Scans the selected number of bits from the data buffer into the selected position in the selected ring
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_RING if ringname is not valid for target
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @param i_target Struct that contains chip and cage/node/position/core/thread information of ring to write
 @param i_ringName Name of ring to write to
 @param i_data DataBuffer object that holds data to write into ring
 @see getRing
 NOTE : This function is ring cache enabled


*/
int putRing (ecmdChipTarget & i_target, const char * i_ringName, ecmdDataBuffer & i_data); 


//@}




/** @name Scom Functions  */
//@{

/**
 @brief Scoms bits from the selected address into the data buffer
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/position/core/thread information of scom address to read
 @param i_address Scom address to read from
 @param o_data DataBuffer object that holds data read from address
 @see putScom
 NOTE : This function forces a flush of the ring caches

*/
int getScom (ecmdChipTarget & i_target, uint32_t i_address, ecmdDataBuffer & o_data);

/**
 @brief Scoms bits from the data buffer into the selected address
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @param i_target Struct that contains chip and cage/node/position/core/thread information of scom address to write
 @param i_address Scom address to write to
 @param i_data DataBuffer object that holds data to write into address
 NOTE : This function forces a flush of the ring caches
 @see getScom

*/
int putScom (ecmdChipTarget & i_target, uint32_t i_address, ecmdDataBuffer & i_data);

//@}


/** @name Jtag Functions */
//@{

/**
 @brief Send a JTAG instruction and modifier to the specified chip
 @param i_target Struct that contains chip and cage/node/position/core/thread information of scom address to write
 @param i_instruction Right aligned instruction to send to chip
 @param i_modifier Right aligned instruction modifier to send
 @param o_status Instruction status register value retrieved
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_NON_JTAG_CHIP Chip Target is a non-jtag attached chip
 @retval nonzero if unsuccessful
 NOTE : Proper parity will be generated on the command and modifier
 NOTE : This function forces a flush of the ring caches
*/
int sendCmd (ecmdChipTarget & i_target, uint32_t i_instruction, uint32_t i_modifier, ecmdDataBuffer & o_status);



/** @name Spy Functions */
//@{

/**
 @brief Reads the selected spy into the data buffer
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SPY_FAILED_ECC_CHECK if invalid ECC detected on Spy read
 @retval ECMD_INVALID_SPY Spy name is invalid or Spy is an ECC Grouping
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_SPY_IS_EDIAL Spy is an edial have to use getSpyEnum
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/position/core/thread information of spy to read
 @param i_spyName Name of spy to read from
 @param o_data DataBuffer object that holds data read from spy
 NOTE : This function is ring cache enabled

*/
int getSpy (ecmdChipTarget & i_target, const char * i_spyName, ecmdDataBuffer & o_data);

/**
 @brief Reads the selected spy and returns it's assocaiated enum
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SPY_FAILED_ECC_CHECK if invalid ECC detected on Spy read - valid Spy Data still returned
 @retval ECMD_INVALID_SPY Spy name is invalid or Spy is an ECC Grouping
 @retval ECMD_INVALID_SPY_ENUM if value in hardware doesn't map to a valid enum
 @retval ECMD_SPY_NOT_ENUMERATED Spy is not enumerated must use getSpy
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_SUCCESS if successful read
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/position/core/thread information of spy to read
 @param i_spyName Name of spy to read from
 @param o_enumValue Enum value read from the spy
 NOTE : This function is ring cache enabled

*/
int getSpyEnum (ecmdChipTarget & i_target, const char * i_spyName, std::string & o_enumValue);

/**
 @brief Read an ECC grouping and return the in and out bits as well as a error mask if any out bits are invalid
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_INVALID_SPY Spy name is invalid or Spy is not an ECC Grouping
 @retval ECMD_SPY_FAILED_ECC_CHECK if invalid ECC detected on Spy read - valid Spy Data still returned
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/position/core/thread information of spy to read
 @param i_spyEccGroupName Name of spy to read from
 @param o_groupData Return the data for the input to the eccGroup
 @param o_eccData Return the Ecc data associated with the outbits of the eccGroup
 @param o_eccErrorMask Return a mask for the Ecc data a 1 in the mask means the associated eccData was in error
 @retval nonzero if unsuccessful
 NOTE : This function is ring cache enabled
*/
int getSpyEccGrouping (ecmdChipTarget & i_target, const char * i_spyEccGroupName, ecmdDataBuffer & o_groupData, ecmdDataBuffer & o_eccData, ecmdDataBuffer & o_eccErrorMask);

/**
 @brief Writes the data buffer into the selected position in the selected spy
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_INVALID_SPY Spy name is invalid or Spy is an ECC Grouping
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_SPY_IS_EDIAL Spy is an edial have to use putSpyEnum
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/position/core/thread information of spy to write
 @param i_spyName Name of spy to write to
 @param i_data DataBuffer object that holds data to write into spy
 NOTE : This function is ring cache enabled

*/
int putSpy (ecmdChipTarget & i_target, const char * i_spyName, ecmdDataBuffer & i_data);

/**
 @brief Writes the enum into the selected position in the selected spy
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_SUCCESS if successful
 @retval ECMD_INVALID_SPY Spy name is invalid or Spy is an ECC Grouping
 2retval ECMD_SPY_NOT_ENUMERATED Spy is not enumerated must use putSpy
 @retval ECMD_INVALID_SPY_ENUM if enum value specified is not valid
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/position/core/thread information of spy to write
 @param i_spyName Name of spy to write to
 @param i_enumValue String enum value to load into the spy
 NOTE : This function is ring cache enabled
*/
int putSpyEnum (ecmdChipTarget & i_target, const char * i_spyName, const std::string i_enumValue);

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
int  ecmdDisableRingCache();

/**
 @brief Flush all modified data from the internal cache to the hardware, then remove all rings from cache
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
*/
int  ecmdFlushRingCache();

//@}




/** @name Array Functions */
//@{

/**
 @brief Reads bits from the selected array into the data buffer
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARRAY if i_arrayName is not valid for target
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/position/core/thread information of array to read
 @param i_arrayName Name of array to read from
 @param o_data DataBuffer object that holds data read from address
 @param i_address Array Address to read from - length of DataBuffer should be set to length of valid address data
 @see putArray
 @see getArrayMultiple
 NOTE : This function forces a flush of the ring caches

*/
int getArray (ecmdChipTarget & i_target, const char * i_arrayName, ecmdDataBuffer & i_address, ecmdDataBuffer & o_data);

/**
 @brief Reads bits from multiple array addresses/elements into the list of data buffers
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARRAY if i_arrayName is not valid for target
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval ECMD_SUCCESS if successful
 @retval nonzero if unsuccessful
 @param i_target Struct that contains chip and cage/node/position/core/thread information of array to read
 @param i_arrayName Name of array to read from
 @param io_entries list of array entries to fetch
 @see putArray
 @see getArray
 NOTE : To use this function the io_entries list should be pre-loaded with the addresses to fetch, the associated dataBuffers will be loaded upon return
 NOTE : This function forces a flush of the ring caches

*/
int getArrayMultiple (ecmdChipTarget & i_target, const char * i_arrayName, std::list<ecmdArrayEntry> & io_entries);


/**
 @brief Writes bits from the data buffer into the selected array
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARRAY if i_arrayName is not valid for target
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_SUCCESS if successful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @param i_target Struct that contains chip and cage/node/position/core/thread information of array to write
 @param i_arrayName Name of array to write to
 @param i_data DataBuffer object that holds data to write into array
 @param i_address Array Address to write to - length of DataBuffer should be set to length of valid address data
 @see getArray
 NOTE : This function forces a flush of the ring caches

*/
int putArray (ecmdChipTarget & i_target, const char * i_arrayName, ecmdDataBuffer & i_address, ecmdDataBuffer & i_data);

/**
 @brief Writes bits from the list of entries into the selected array
 @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
 @retval ECMD_INVALID_ARRAY if i_arrayName is not valid for target
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @retval ECMD_DATA_UNDERFLOW  Too little data was provided to a write function
 @retval ECMD_SUCCESS if successful
 @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
 @retval nonzero if unsuccessful
 @retval ECMD_DATA_OVERFLOW Too much data was provided for a write
 @param i_target Struct that contains chip and cage/node/position/core/thread information of array to write
 @param i_arrayName Name of array to write to
 @param i_entries List of addresses and data to write to chip
 @see getArray
 NOTE : This function forces a flush of the ring caches
 NOTE : i_entries should be pre-loaded with address and data

*/
int putArrayMultiple (ecmdChipTarget & i_target, const char * i_arrayName, std::list<ecmdArrayEntry> & i_entries);

//@}






/** @name Flush and IPL Functions */
//@{

/**
 * @brief System Flush
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero if unsuccessful
 * @post The system is cleaned up and ready for iplsys
 * @see iplSys
 NOTE : This function forces a flush of the ring caches

*/
int flushSys ();

/**
 * @brief Initial Program Load
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero if unsuccessful
 * @pre An ecmdFlushSys call is generally required before iplsys
 * @post The system is IPLed
 * @see flushSys
 NOTE : This function forces a flush of the ring caches

*/
int iplSys ();

//@}


/** @name Simulation Functions */
//@{
#ifndef REMOVE_SIM

/**
 * @brief Enable/Disable Simulation AET Logging
 * @param i_function Should be either 'on'/'off'/'flush'
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simaet(const char* i_function);

/**
 * @brief Store a checkpoint to specified file
 * @param i_checkpoint Name of checkpoint to write to
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simcheckpoint(const char* i_checkpoint);

/**
 * @brief Clock the model
 * @param i_cycles Number of cycles to clock model
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simclock(int i_cycles);

/**
 * @brief Echo message to stdout and sim log
 * @param i_message Message to echo
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simecho(const char* i_message);

/**
 * @brief Close down the simulation model
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simexit();

/**
 * @brief Perform expect on facility using symbol
 * @param i_symbol Symbol id of facility
 * @param i_expect Value to expect on facility
 * @param i_bitlength Length of data to expect
 * @param i_row Optional: Array Facility row
 * @param i_offset Optional: Facility offset
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simEXPECTFAC(unsigned int i_symbol, int i_bitlength, ecmdDataBuffer & i_expect, int i_row = 0, int i_offset = 0);

/**
 * @brief Perform expect on facility using name
 * @param i_facname Facility name
 * @param i_expect Value to expect on facility
 * @param i_bitlength Length of data to expect
 * @param i_row Optional: Array Facility row
 * @param i_offset Optional: Facility offset
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simEXPECTFACS(const char* i_facname, int i_bitlength, ecmdDataBuffer & i_expect, int i_row = 0, int i_offset = 0);

/**
 * @brief Perform expect on TCFAC facility
 * @param i_tcfacname Facility name
 * @param i_expect Value to expect on facility
 * @param i_bitlength Length of data to expect
 * @param i_row Optional: Array Facility row
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simexpecttcfac(const char* i_tcfacname, int i_bitlength, ecmdDataBuffer & i_expect, int i_row = 0);

/**
 * @brief Fetch current model cycle count
 * @param o_cyclecount Current model cycle count
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simgetcurrentcycle(int & o_cyclecount);

/**
 * @brief Retrieve a Facility using a symbol id
 * @param i_symbol Facility symbol id
 * @param i_bitlength Bit length to read from facility
 * @param o_data Data read from facility
 * @param i_row Optional: Array row
 * @param i_offset Optional : Facility offset
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simGETFAC(unsigned int i_symbol, int i_bitlength, ecmdDataBuffer & o_data, int i_row = 0, int i_offset = 0);

/**
 * @brief Retrieve a Facility using a name
 * @param i_facname Facility name
 * @param i_bitlength Bit length to read from facility
 * @param o_data Data read from facility
 * @param i_row Optional: Array row
 * @param i_offset Optional : Facility offset
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simGETFACS(const char* i_facname, int i_bitlength, ecmdDataBuffer & o_data, int i_row = 0, int i_offset = 0);

/**
 * @brief Retrieve a Facility using a name - preserving Xstate
 * @param i_facname Facility name
 * @param i_bitlength Bit length to read from facility
 * @param o_data Data read from facility
 * @param i_row Optional: Array row
 * @param i_offset Optional : Facility offset
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simGETFACX(const char* i_facname, int i_bitlength, ecmdDataBuffer & o_data, int i_row = 0, int i_offset = 0);

/**
 * @brief Retrieve a TCFAC facility
 * @param i_tcfacname TCFAC name
 * @param o_data Value read
 * @param i_row Optional: Array Facility row
 * @param i_startbit Optional: Startbit to read
 * @param i_bitlength Optional: Length of data to read
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simgettcfac(const char* i_tcfacname, ecmdDataBuffer & o_data, int i_row = 0, int i_startbit = 0, int i_bitlength = 0);

/**
 * @brief Initialize the simulation
 * @param i_checkpoint Checkpoint to load : 'none' to skip
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int siminit(const char* i_checkpoint);

/**
 * @brief Write a Facility using a symbol id
 * @param i_symbol Facility symbol id
 * @param i_bitlength Bit length to write to facility
 * @param i_data Data to write
 * @param i_row Optional: Array row
 * @param i_offset Optional : Facility offset
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simPUTFAC(unsigned int i_symbol, int i_bitlength, ecmdDataBuffer & i_data, int i_row = 0, int i_offset = 0);

/**
 * @brief Write a Facility using a name
 * @param i_facname Facility name
 * @param i_bitlength Bit length to write to facility
 * @param i_data Data to write
 * @param i_row Optional: Array row
 * @param i_offset Optional : Facility offset
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simPUTFACS(const char* i_facname, int i_bitlength, ecmdDataBuffer & i_data, int i_row = 0, int i_offset = 0);

/**
 * @brief Write a Facility using a name - preserving Xstate
 * @param i_facname Facility name
 * @param i_bitlength Bit length to write to facility
 * @param i_data Data to write
 * @param i_row Optional: Array row
 * @param i_offset Optional : Facility offset
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simPUTFACX(const char* i_facname, int i_bitlength, ecmdDataBuffer & i_data, int i_row = 0, int i_offset = 0);

/**
 * @brief Write a TCFAC facility
 * @param i_tcfacname TCFAC name
 * @param i_data Value to write
 * @param i_row Optional: Array Facility row
 * @param i_numrows Optional: Number of rows to write
 * @param i_bitlength Bit length to write to facility
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simputtcfac(const char* i_tcfacname, int i_bitlength, ecmdDataBuffer & i_data, int i_row = 0, int i_numrows = 0);

/**
 * @brief Load a checkpoint into model
 * @param i_checkpoint Name of checkpoint
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simrestart(const char* i_checkpoint);

/**
 * @brief Stick a Facility using a symbol id
 * @param i_symbol Facility symbol id
 * @param i_bitlength Bit length to stick to facility
 * @param i_data Data to stick
 * @param i_row Optional: Array row
 * @param i_offset Optional : Facility offset
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simSTKFAC(unsigned int i_symbol, int i_bitlength, ecmdDataBuffer & i_data, int i_row = 0, int i_offset = 0);

/**
 * @brief Stick a Facility using a name
 * @param i_facname Facility name
 * @param i_bitlength Bit length to stick to facility
 * @param i_data Data to stick
 * @param i_row Optional: Array row
 * @param i_offset Optional : Facility offset
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simSTKFACS(const char* i_facname, int i_bitlength, ecmdDataBuffer & i_data, int i_row = 0, int i_offset = 0);

/**
 * @brief Stick a TCFAC facility
 * @param i_tcfacname TCFAC name
 * @param i_data Value to stick
 * @param i_row Optional: Array Facility row
 * @param i_numrows Optional: Number of rows to stick
 * @param i_bitlength Bit length to write to facility
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simstktcfac(const char* i_tcfacname, int i_bitlength, ecmdDataBuffer & i_data, int i_row = 0, int i_numrows = 0);

/**
 * @brief Run RTX SUBCMD
 * @param i_command Command
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simSUBCMD(const char* i_command);

/**
 * @brief Look up a Facility symbol
 * @param i_facname Facility name
 * @param o_symbol Symbol id
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simsymbol(const char* i_facname, unsigned int & o_symbol);

/**
 * @brief Unstick a Facility using a symbol id
 * @param i_symbol Facility symbol id
 * @param i_bitlength Bit length to unstick to facility
 * @param i_row Optional: Array row
 * @param i_offset Optional : Facility offset
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simUNSTICK(unsigned int i_symbol, int i_bitlength, int i_row = 0, int i_offset = 0);

/**
 * @brief Unstick a Facility using a name
 * @param i_facname Facility name
 * @param i_bitlength Bit length to unstick to facility
 * @param i_row Optional: Array row
 * @param i_offset Optional : Facility offset
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simUNSTICKS(const char* i_facname, int i_bitlength, int i_row = 0, int i_offset = 0);

/**
 * @brief Unstick a TCFAC facility
 * @param i_tcfacname TCFAC name
 * @param i_data Value to unstick to 
 * @param i_row Optional: Array Facility row
 * @param i_numrows Optional: Number of rows to unstick
 * @param i_bitlength Bit length to unstick to facility
 * @retval ECMD_SUCCESS if successful
 * @retval nonzero on failure
*/
int simunsticktcfac(const char* i_tcfacname, int i_bitlength, ecmdDataBuffer & i_data, int i_row = 0, int i_numrows = 0);

#endif /* ifndef REMOVE_SIM */
//@}



/** @name Error Handling Functions */
//@{

/**
 @brief Retrieve additional error information for errorcode
 @param i_errorCode Error code to lookup up message for
 @retval point to NULL terminated string containing error data, NULL if error occurs
 NOTE : This function does not affect ring caching
*/
std::string ecmdGetErrorMsg(int i_errorCode);

/**
 @brief Register an Error Message that has occured
 NOTE : This function does not affect ring caching
*/
int ecmdRegisterErrorMsg(int i_errorCode, const char* i_whom, const char* i_message);

//@}




/** @name Output Functions */
//@{

/**
 @brief Output a message related to an error
 @param i_message String to output
 NOTE : This function does not affect ring caching
*/
void ecmdOutputError(const char* i_message);

/**
 @brief Output a message related to an warning
 @param i_message String to output
 NOTE : This function does not affect ring caching
*/
void ecmdOutputWarning(const char* i_message);

/**
 @brief Output a message to the screen or logs
 @param i_message String to output
 NOTE : This function does not affect ring caching
*/
void ecmdOutput(const char* i_message);

//@}



}  //extern "C"


#endif /* ecmdClientCapi_H */


/**
 * @mainpage

@section int Introduction

Common Hardware Access Programming Interface (eCMD)

This is the documentation of the eCMD C/C++ Programming Api

@section inc Include Files

To compile client code to use the C++ API, the following header files are required:
<ul>
<li> ecmdClientCapi.H
<li> ecmdDataBuffer.H
<li> ecmdStructs.H
<li> ecmdReturnCodes.H
<li> ecmdUtils.H
</ul>

@section link Link objects
To link the client code on AIX, the following is required:
<ul>
<li>ecmdClientCapi_aix.a
<li>ecmdClientCapi.export
<li>xlC v5.0
</ul>
To create Linux x86 binaries, the following is required:
<ul>
<li>ecmdClientCapi_x86.a
<li>g++ v2.96
</ul>
To create Linux ppc binaries, the following is required:
<ul>
<li>ecmdClientCapi_ppc.a
<li>g++ ????
</ul>
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
@subsection Aixmake Aix
<pre>
testclient: testclient.o ecmdClientCapi.H ecmdDataBuffer.H ecmdReturnCodes.H ecmdStructs.H ecmdClientCapi_aix.a
        xlC -+ -g testclient.o ecmdClientCapi_aix.a -bI:ecmdClientCapi.export -o testclient

testclient.o: testclient.c ecmdClientCapi.H ecmdDataBuffer.H ecmdReturnCodes.H ecmdStructs.H ecmdClientCapi_aix.a
	xlC -+ -g -c testclient.c -o testclient.o
</pre>

@subsection Linuxmake Linux x86
<pre>
testclient.linux: testclient_linux.o ecmdClientCapi.H ecmdDataBuffer.H ecmdReturnCodes.H ecmdStructs.H ecmdClientCapi_x86.a
	g++ -g -ldl testclient_linux.o ecmdClientCapi_x86.a -o testclient.linux

testclient_linux.o: testclient.c ecmdClientCapi.H ecmdDataBuffer.H ecmdReturnCodes.H ecmdStructs.H ecmdClientCapi_x86.a
	g++ -g -c -I./ testclient.c -o testclient_linux.o
</pre>
*/

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
