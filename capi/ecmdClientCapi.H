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
 @param dllName Specify the full path and name of the dll to load, 
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
int ecmdLoadDll(std::string dllName);

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
 @param argc Passed from Command line Arguments
 @param argv Passed from Command line Arguments
 @pre loadDll must have been called
 @post Global options (ex. -debug, -p#, -c#) will be removed from arg list
 @see loadDll

 - argc/argv get passed to the eCMD DLL.
 - Global options such as -debug flags and -p#, -c# will be parsed out.
 - Position flags can be queried later with functions like ?????
*/

int ecmdCommandArgs(int argc, char* argv[]); 

//@}






/** @name Query Functions */
//@{
/**
 @brief Query configuration information from the DLL
 @param target Struct that contains partial information to limit query results
 @param queryData Return vector from query - in numerical order by cageId
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure

 The Valid bits of the target are used to refine the query

 The target paramater should be filled in with as much data as you know to limit the query, (including the chipType).
 When a field is nonValid the query function will iterate on all possible values for that
 entry and return the relevant data.


 Ex: to query what positions of the Nova chip are on cage 1, node 2:<p>
   cage = 1, node = 2, pos = <nonvalid>, chipType = 'Nova', core = <nonvalid>, thread = <nonvalid><p>
 Ex: to query what positions of the Nova chip are in the entire system:<p>
   cage = <nonvalid>, node = <nonvalid>, pos = <nonvalid>, chipType = 'Nova', core = <nonvalid>, thread = <nonvalid><p>
 Ex: to query all the chips on cage 3, node 0:<p>
   cage = 3, node = 0, pos = <nonvalid>, chipType = <nonvalid>, core = <nonvalid>, thread = <nonvalid><p>
 Ex: to query all the chips in the entire system:<p>
   cage = <nonvalid>, node = <nonvalid>, pos = <nonvalid>, chipType = <nonvalid>, core = <nonvalid>, thread = <nonvalid><p>
*/
   
int ecmdQueryConfig(ecmdChipTarget & target, std::vector<ecmdCageData> & queryData);

/**
 @brief Query User Selected Targeting information from the DLL, i.e (-p#,-c#,-t#)
 @param target Struct that contains partial information to limit query results - chipType is unused
 @param queryData Return vector from query - in numerical order by cageId
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure

 This function acts just like ecmdQueryConfig except it operates on what targets were selected by the
  user args -n#, -p#, -c#, -t#

 Use of this function is the same as ecmdQueryConfig except chipType is not used
*/
int ecmdQuerySelected(ecmdChipTarget & target, std::vector<ecmdCageData> & queryData);

/**
 @brief Query Ring information from the DLL
 @param target Struct that contains chip and cage/node/position/core/thread information of chip to use
 @param queryData Return vector from query
 @param ringName if != NULL used to refine query to a single ring
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
int ecmdQueryRing(ecmdChipTarget & target, std::vector<ecmdRingData> & queryData, const char * ringName = NULL);

/**
 @brief Query Array information from the DLL
 @param target Struct that contains chip and cage/node/position/core/thread information of chip to use
 @param queryData Return vector from query
 @param arrayName array to access data for
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
int ecmdQueryArray(ecmdChipTarget & target, std::vector<ecmdArrayData> & queryData, const char * arrayName);

/**
 @brief Query Spy information from the DLL
 @param target Struct that contains chip and cage/node/position/core/thread information of chip to use
 @param queryData Return vector from query
 @param spyName Spy to access data for
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
int ecmdQuerySpy(ecmdChipTarget & target, std::vector<ecmdSpyData> & queryData, const char * spyName);

/**
 @brief Query the location of a specific file type for the selected target
 @param target Struct that contains chip and cage/node/position/core/thread information
 @param fileType Enum that specifies which type of file you are looking for scandef/spydef/arraydef
 @param fileLocation Return string with full path and filename to location 
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
*/
int ecmdQueryFileLocation(ecmdChipTarget & target, ecmdFileType_t fileType, std::string fileLocation);


/**
 @brief Query wether the target specified was selected by the user with -n#, -p#, -c#, -t#
 @param target Struct that contains the cage/node/position/core/thread information (chipType not used)
 @retval FALSE if target not selected
 @retval TRUE if target selected
  - The Valid bits of the target are used to refine the query @see ecmdQueryConfig
*/
uint8_t ecmdQueryTargetSelected(ecmdChipTarget & target);

//@}





/** @name Scan Functions */
//@{
/**
 @brief Scans the selected number of bits from the selected position in the selected ring into the data buffer
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of ring to read
 @param ringName Name of ring to read from
 @param data DataBuffer object that holds data read from ring
 @see putRing


*/
int getRing (ecmdChipTarget & target, const char * ringName, ecmdDataBuffer & data); 

/**
 @brief Scans the selected number of bits from the data buffer into the selected position in the selected ring
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of ring to write
 @param ringName Name of ring to write to
 @param data DataBuffer object that holds data to write into ring
 @see getRing


*/
int putRing (ecmdChipTarget & target, const char * ringName, ecmdDataBuffer & data); 


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
 @brief Scans the selected number of bits from the selected position in the selected spy into the data buffer
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of spy to read
 @param spyName Name of spy to read from
 @param data DataBuffer object that holds data read from spy

*/
int getSpy (ecmdChipTarget & target, const char * spyName, ecmdDataBuffer & data);

/**
 @brief Scans the selected number of bits from the selected position in the selected spy into the data buffer
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of spy to read
 @param spyName Name of spy to read from
 @param enumValue Enum value read from the spy

*/
int getSpyEnum (ecmdChipTarget & target, const char * spyName, std::string enumValue);

/**
 @brief Scans the selected number of bits from the data buffer into the selected position in the selected spy
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of spy to write
 @param spyName Name of spy to write to
 @param data DataBuffer object that holds data to write into spy

*/
int putSpy (ecmdChipTarget & target, const char * spyName, ecmdDataBuffer & data);

/**
 @brief Scans the selected number of bits from the data buffer into the selected position in the selected spy
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of spy to write
 @param spyName Name of spy to write to
 @param enumValue String enum value to load into the spy

*/
int putSpyEnum (ecmdChipTarget & target, const char * spyName, const std::string enumValue);

//@}





/** @name Array Functions */
//@{

/**
 @brief Reads bits from the selected array into the data buffer
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of array to read
 @param arrayName Name of array to read from
 @param data DataBuffer object that holds data read from address
 @param address Array Address to read from - how long is this ??????
 @see putArray

*/
int getArray (ecmdChipTarget & target, const char * arrayName, uint32_t * address, ecmdDataBuffer & data);

/**
 @brief Writes bits from the data buffer into the selected array
 @retval ECMD_SUCCESS if successful read, nonzero if unsuccessful
 @param target Struct that contains chip and cage/node/position/core/thread information of array to write
 @param arrayName Name of array to write to
 @param data DataBuffer object that holds data to write into array
 @param address Array Address to write to - how long is this ??????
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
 @param errorCode Error code to lookup up message for
 @retval point to NULL terminated string containing error data, NULL if error occurs
*/
std::string ecmdGetErrorMsg(int errorCode);

/**
 @brief Register an Error Message that has occured
*/
int ecmdRegisterErrorMsg(int errorCode, const char* whom, const char* message);

//@}




/** @name Output Functions */
//@{

/**
 @brief Output a message related to an error
 @param message String to output
*/
void ecmdOutputError(const char* message);

/**
 @brief Output a message related to an warning
 @param message String to output
*/
void ecmdOutputWarning(const char* message);

/**
 @brief Output a message to the screen or logs
 @param message String to output
*/
void ecmdOutput(const char* message);

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
