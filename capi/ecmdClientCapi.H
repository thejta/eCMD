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
 @retval ECMD_SUCCESS if successful load
 @retval ECMD_INVALID_DLL_VERSION if Dll version loaded doesn't match client version
 @retval nonzero if unsuccessful
 @post eCMD DLL is loaded into memory and initialized
 @see unloadDll

       - This function loads the DLL based on ???.
       - Name limit of 255 characters.
       - Errors in loading are printed to STDOUT.
*/
int ecmdLoadDll();

/**
 @brief Unload the eCMD DLL
 @retval ECMD_SUCCESS if successful load
 @retval nonzero if unsuccessful
 @see loadDll

 - Errors in unloading are printed to STDOUT.
*/

int ecmdUnloadDll(); 

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

int ecmdInitDll(int argc, char* argv[]); 

//@}


/** @name Scan Functions */
//@{
/**
 @brief Scans the selected number of bits from the selected position in the selected ring into the data buffer
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of ring to read
 @param ringName Name of ring to read from
 @param data DataBuffer object that holds data read from ring
 @param startPos Position of first bit in the ring to read from
 @param numBits Number of bits to read
 @see putRing


*/
int getRing (ecmdChipTarget & target, const char * ringName, ecmdDataBuffer & data, int startPos, int numBits); 

/**
 @brief Scans the selected number of bits from the data buffer into the selected position in the selected ring
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of ring to write
 @param ringName Name of ring to write to
 @param data DataBuffer object that holds data to write into ring
 @param startPos Position of first bit in the ring to write to
 @param numBits Number of bits to write
 @see getRing


*/
int putRing (ecmdChipTarget & target, const char * ringName, ecmdDataBuffer & data, int startPos, int numBits); 

/**
 @brief Scans all of the bits from the selected ring on the target into the selected file
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of ring to read
 @param ringName Name of ring to read from
 @param fileName Name of file to write data to
 @see ringWrite

 - comment on file format, naming convention, should we give it a default file name?

*/
int ringRead (ecmdChipTarget & target, const char * ringName, const char * fileName);

/**
 @brief Scans all of the bits from the given file into theselected ring on the target
 @retval ECMD_SUCCESS if successful write, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of ring to write
 @param ringName Name of ring to write to
 @param fileName Name of file to read data from
 @see ringRead

 - comment on file format, naming convention, should we give it a default file name?

*/
int ringWrite (ecmdChipTarget & target, const char * ringName, const char * fileName);

/**
 @brief Scans the selected number of bits from the selected position of the selected register in the selected ring into the data buffer
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of register to read
 @param ringName Name of ring to read from
 @param registerName Name of register in the ring to read from
 @param data DataBuffer object that holds data read from register
 @param startPos Position of first bit in the register to read from
 @param numBits Number of bits to read
 @see putRegister


*/
int getRegister (ecmdChipTarget & target, const char * ringName, const char * registerName, ecmdDataBuffer & data, int startPos, int numBits);

/**
 @brief Scans the selected number of bits from the data buffer into the selected position in the selected register
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of register to write
 @param ringName Name of ring to write to
 @param registerName Name of register in the ring to write to
 @param data DataBuffer object that holds data to write into register
 @param startPos Position of first bit in the register to write to
 @param numBits Number of bits to write
 @see getRegister


*/
int putRegister (ecmdChipTarget & target, const char * ringName, const char * registerName, ecmdDataBuffer & data, int startPos, int numBits);

//@}


/** @name Query Functions */
//@{

int ecmdQueryConfig();

int ecmdQueryRing();

int ecmdQueryArray();

int ecmdQueryAlias();

/**
 @brief Query the location of a specific file type for the selected target
 @param target Struct that contains chip and cage/node/position/core/thread information
 @param fileType Enum that specifies which type of file you are looking for scandef/spydef/arraydef
 @param fileLocation Return string with full path and filename to location - allocated by Capi - must be deallocated
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
*/
int ecmdQueryFileLocation(ecmdChipTarget & target, ecmdFileType_t fileType, char * fileLocation);


//@}

/** @name Scom Functions  */
//@{

/**
 @brief Scoms bits from the selected address into the data buffer
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of scom address to read
 @param address Scom address to read from
 @param data DataBuffer object that holds data read from address
 @see putScom

*/
int getScom (ecmdChipTarget & target, uint32_t address, ecmdDataBuffer & data);

/**
 @brief Scoms bits from the data buffer into the selected address
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of scom address to write
 @param address Scom address to write to
 @param data DataBuffer object that holds data to write into address
 @see getScom

*/
int putScom (ecmdChipTarget & target, uint32_t address, ecmdDataBuffer & data);

//@}


/** @name Spy Functions */
//@{

/**
 @brief Scans the selected number of bits from the selected position in the selected alias into the data buffer
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of alias to read
 @param aliasName Name of alias to read from
 @param data DataBuffer object that holds data read from alias
 @param startPos Position of first bit in the alias to read from
 @param numBits Number of bits to read
 @see putAlias

*/
int getSpy (ecmdChipTarget & target, const char * spyName, ecmdDataBuffer & data, int startPos, int numBits);

/**
 @brief Scans the selected number of bits from the selected position in the selected alias into the data buffer
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of alias to read
 @param aliasName Name of alias to read from
 @param enumValue Enum value
 @param startPos Position of first bit in the alias to read from
 @param numBits Number of bits to read
 @see putAlias

*/
int getSpyEnum (ecmdChipTarget & target, const char * spyName, char * enumValue, int startPos, int numBits);

/**
 @brief Scans the selected number of bits from the data buffer into the selected position in the selected alias
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of alias to write
 @param aliasName Name of alias to write to
 @param data DataBuffer object that holds data to write into alias
 @param startPos Position of first bit in the alias to write to
 @param numBits Number of bits to write
 @see getAlias


*/
int putSpy (ecmdChipTarget & target, const char * spyName, ecmdDataBuffer & data, int startPos, int numBits);

/**
 @brief Scans the selected number of bits from the data buffer into the selected position in the selected alias
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of alias to write
 @param aliasName Name of alias to write to
 @param data DataBuffer object that holds data to write into alias
 @param startPos Position of first bit in the alias to write to
 @param numBits Number of bits to write
 @see getAlias


*/
int putSpyEnum (ecmdChipTarget & target, const char * spyName, ecmdDataBuffer & data, int startPos, int numBits);

//@}

/** @name Array Functions */
//@{

/**
 @brief Reads bits from the selected array into the data buffer
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of array to read
 @param arrayName Name of array to read from
 @param data DataBuffer object that holds data read from address
 @see putArray

*/
int getArray (ecmdChipTarget & target, const char * arrayName, uint32_t * address, ecmdDataBuffer & data);

/**
 @brief Writes bits from the data buffer into the selected array
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of array to write
 @param arrayName Name of array to write to
 @param data DataBuffer object that holds data to write into array
 @see getArray

*/
int putArray (ecmdChipTarget & target, const char * arrayName, uint32_t * address, ecmdDataBuffer & data);

//@}


/** @name Flush and IPL Functions */
//@{

/**
 * @brief System Flush
 * @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 * @post The system is cleaned up and ready for iplsys
 * @see iplSys

*/
int flushSys ();

/**
 * @brief Initial Program Load
 * @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 * @pre An ecmdFlushSys call is generally required before iplsys
 * @post The system is IPLed
 * @see flushSys

*/
int iplSys ();

//@}


/** @name Error Handling Functions */
//@{

/**
 @brief Retrieve additional error information for errorcode
 @retval point to NULL terminated string containing error data, NULL if error occurs
*/
const char* ecmdGetErrorMsg(int errorCode);

/**
 @brief Register an Error Message that has occured
*/
int ecmdRegisterErrorMsg(int errorCode, const char* message);

//@}

/** @name Output Functions */
//@{

//@}



}  //extern "C"


#endif /* ecmdClientCapi_H */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
