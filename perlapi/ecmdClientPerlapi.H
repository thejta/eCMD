#ifndef _ecmdClientPerlapi_H
#define _ecmdClientPerlapi_H

/* File ecmdClientPerlapi.H created by Joshua Wills on Mon Nov 24 2003. */

// Copyright **********************************************************
//
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 2003
//                                                                      
// The source code for this program is not published or otherwise       
// divested of its trade secrets, irrespective of what has been         
// deposited with the U.S. Copyright Office.                             
//                                                                      
// End Copyright ******************************************************

//for Linux compiles
#ifdef list
#undef list
#endif

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <string>
#include <ecmdStructs.H>

/**
 * @file ecmdClientPerlapi.H
 * @brief eCMD Perl API

 * Usage :
   <pre>
 *   if (`uname` eq "AIX") {
 *     require ecmd_aix_perl;
 *   } else {
 *     require ecmd_x86_perl;
 *   }    
 *   my $ep = new ecmdClientPerlapi();
   </pre>

*/

extern "C" int ecmdPerlInterfaceErrorCheck (int ec);

/**
 * @brief eCMD Perl API

 * Usage : 
   <pre>
 *   if (`uname` eq "AIX") {
 *     require ecmd_aix_perl;
 *   } else {
 *     require ecmd_x86_perl;
 *   }    
 *   my $ep = new ecmdClientPerlapi();
   </pre>
*/

class ecmdClientPerlapi
{
  public:

  /** @name Load/Unload Functions */
  //@{
    /**
     @brief eCMD Perl Module Constructor
     */
    ecmdClientPerlapi ();
    /**
     @brief eCMD Perl Module Destructor
     */
    ~ecmdClientPerlapi ();

  /**
   @brief Initialize the eCMD DLL
   @retval 0 if successful load  1 if unsuccessful
   @param i_dllName Full path and filename of the eCMD Dll to load
   @param i_options Passed from Command line Arguments
   @pre ecmdClientPerlapi constructor must have been called
   @post Global options (ex. -p#, -c#) will be removed from arg list

   - initializes the eCMD Dll.
   - Global options such as -p#, -c# will be parsed out.
   - Position flags can be queried later with functions like wasposselected()
   */
    int initDll(const char * i_dllName, const char * i_options = NULL);


    /**
     @brief Clean up the Perl Module
     */
    void cleanup();

    /**
     @brief Pass any unknown command line paramaters to the DLL for processing (ex. -p#, Cronus -debug)
     @retval ECMD_SUCCESS if successful
     @retval nonzero if unsuccessful
     @param i_argv Passed from Command line Arguments
     @pre loadDll must have been called
     @post Global options (ex. -debug, -p#, -c#) will be removed from arg list
     @see loadDll

     - argc/argv get passed to the eCMD DLL.
     - Global options such as -debug flags and -p#, -c# will be parsed out.
     - Position flags can be queried later with functions like ?????
     NOTE : This function does not affect ring caching
     */

    int ecmdCommandArgs(char** i_argv[]); 

  //@}



/** @name Chip Scan Functions */
//@{
    /**
     @brief Scans the selected number of bits from the selected position in the selected ring into the data buffer
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
    int getRing (const char * i_target, const char * i_ringName, char** o_data); 

    /**
     @brief Scans the selected number of bits from the data buffer into the selected position in the selected ring
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
    int putRing (const char * i_target, const char * i_ringName, const char * i_data); 



    /**
     @brief Reads the selected spy into the data buffer (only full name matching supported)
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
     @param i_startBit Startbit in latchname to fetct data
     @param i_numBits Number of bits to fetch from startbit
     @param i_ringName Name of ring to search for latch if == "", entire scandef is searched
     @param i_mode LatchName search mode

     NOTE : This function is ring cache enabled

     */
    int getLatch (const char * i_target, const char* i_ringName, const char * i_latchName, const char * o_data, int i_startBit, int i_numBits);


    /**
     @brief Writes the data buffer into the all latches matching i_latchName (only full name matching supported)
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
     @param i_ringName Name of ring to search for latch if == "", entire scandef is searched
     @param i_startBit Startbit in latchname to insert data
     @param i_numBits Number of bits to insert from startbit
     @param o_matchs Number of latchs found that matched your name and data was inserted

     NOTE : This function is ring cache enabled

     */
    int putLatch (const char * i_target, const char* i_ringName, const char * i_latchName, const char * i_data, int i_startBit, int i_numBits, int & o_matchs);

    //@}



    /** @name Chip Scom Functions */
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
    int getScom (const char* i_target, int i_address, char** o_data);

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
    int putScom (const char* i_target, int i_address, const char* i_data);

    //@}



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
    int sendCmd (const char* i_target, int i_instruction, int i_modifier, char** o_status);

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
    int getCfamRegister (const char* i_target, int i_address, char** o_data);

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
    int putCfamRegister (const char* i_target, int i_address, const char* i_data);

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
    int getSpy (const char* i_target, const char * i_spyName, char** o_data);

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
    int getSpyEnum (const char* i_target, const char * i_spyName, char** o_enumValue);

    /**
     @brief Read an ECC grouping and return the in and out bits as well as a error mask if any out bits are invalid
     @retval ECMD_TARGET_NOT_CONFIGURED if target is not available in the system
     @retval ECMD_SUCCESS if successful
     @retval ECMD_INVALID_SPY Spy name is invalid or Spy is not an ECC Grouping
     @retval ECMD_SPY_FAILED_ECC_CHECK if invalid ECC detected on Spy read - valid Spy Data still returned
     @retval ECMD_CLOCKS_IN_INVALID_STATE Chip Clocks were in an invalid state to perform the operation
     @retval nonzero if unsuccessful
     @param i_target Struct that contains chip and cage/node/slot/position/core information of spy to read
     @param i_spyEccGroupName Name of spy to read from
     @param o_groupData Return the data for the input to the eccGroup
     @param o_eccData Return the Ecc data associated with the outbits of the eccGroup
     @param o_eccErrorMask Return a mask for the Ecc data a 1 in the mask means the associated eccData was in error
     @retval nonzero if unsuccessful

     NOTE : This function is ring cache enabled
     */
    int getSpyEccGrouping (const char* i_target, const char * i_spyEccGroupName, char** o_groupData, char** o_eccData, char** o_eccErrorMask);

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
     @param i_target Struct that contains chip and cage/node/slot/position/core information of spy to write
     @param i_spyName Name of spy to write to
     @param i_data DataBuffer object that holds data to write into spy

     NOTE : This function is ring cache enabled

     */
    int putSpy (const char* i_target, const char * i_spyName, const char* i_data);

    /**
     @brief Writes the enum into the selected position in the selected spy
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
    int putSpyEnum (const char* i_target, const char * i_spyName, const char* i_enumValue);

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
    int getArray (const char* i_target, const char * i_arrayName, const char* i_address, char** o_data);


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
    int putArray (const char* i_target, const char * i_arrayName, const char* i_address, const char* i_data);

    //@}



    /** @name Simulation Functions */
    //@{

    /**
     @brief Enable/Disable Simulation AET Logging
     @param i_function Should be either 'on'/'off'/'flush'
     @retval ECMD_SUCCESS if successful
     @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
     @retval nonzero on failure
     */
    int simaet(const char* i_function);

    /**
     @brief Store a checkpoint to specified file
     @param i_checkpoint Name of checkpoint to write to
     @retval ECMD_SUCCESS if successful
     @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
     @retval nonzero on failure
     */
    int simcheckpoint(const char* i_checkpoint);

    /**
     @brief Clock the model
     @param i_cycles Number of cycles to clock model
     @retval ECMD_SUCCESS if successful
     @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
     @retval nonzero on failure
     */
    int simclock(int i_cycles);

    /**
     @brief Echo message to stdout and sim log
     @param i_message Message to echo
     @retval ECMD_SUCCESS if successful
     @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
     @retval nonzero on failure
     */
    int simecho(const char* i_message);

    /**
     @brief Close down the simulation model
     @retval ECMD_SUCCESS if successful
     @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
     @retval nonzero on failure
     */
    int simexit();

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
    int simEXPECTFAC(const char* i_facname, int i_bitlength, const char* i_expect, int i_row = 0, int i_offset = 0);

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
    int simexpecttcfac(const char* i_tcfacname, int i_bitlength, const char* i_expect, int i_row = 0);

    /**
     @brief Fetch current model cycle count
     @param o_cyclecount Current model cycle count
     @retval ECMD_SUCCESS if successful
     @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
     @retval nonzero on failure
     */
    int simgetcurrentcycle(char** o_cyclecount);

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
    int simGETFAC(const char* i_facname, int i_bitlength, char** o_data, int i_row = 0, int i_offset = 0);

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
    int simGETFACX(const char* i_facname, int i_bitlength, char** o_data, int i_row = 0, int i_offset = 0);

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
    int simgettcfac(const char* i_tcfacname, char** o_data, int i_row = 0, int i_startbit = 0, int i_bitlength = 0);

    /**
     @brief Initialize the simulation
     @param i_checkpoint Checkpoint to load : 'none' to skip
     @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
     @retval ECMD_SUCCESS if successful
     @retval nonzero on failure
     */
    int siminit(const char* i_checkpoint);

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
    int simPUTFAC(const char* i_facname, int i_bitlength, const char* i_data, int i_row = 0, int i_offset = 0);

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
    int simPUTFACX(const char* i_facname, int i_bitlength, const char* i_data, int i_row = 0, int i_offset = 0);

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
    int simputtcfac(const char* i_tcfacname, int i_bitlength, const char* i_data, int i_row = 0, int i_numrows = 0);

    /**
     @brief Load a checkpoint into model
     @param i_checkpoint Name of checkpoint
     @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
     @retval ECMD_SUCCESS if successful
     @retval nonzero on failure
     */
    int simrestart(const char* i_checkpoint);

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
    int simSTKFAC(const char* i_facname, int i_bitlength, const char* i_data, int i_row = 0, int i_offset = 0);

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
    int simstktcfac(const char* i_tcfacname, int i_bitlength, const char* i_data, int i_row = 0, int i_numrows = 0);

    /**
     @brief Run RTX SUBCMD
     @param i_command Command
     @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
     @retval ECMD_SUCCESS if successful
     @retval nonzero on failure
     */
    int simSUBCMD(const char* i_command);


    /**
     @brief Set TCK Interval setting in the model for JTAG Master
     @param i_tckinterval new setting for tck interval when using JTAG
     @retval ECMD_RING_CACHE_ENABLED Ring Cache enabled function - must be disabled to use this function
     @retval ECMD_SUCCESS if successful
     @retval nonzero on failure
     */
    int simtckinterval(int i_tckinterval);



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
    int simUNSTICK(const char* i_facname, int i_bitlength, int i_row = 0, int i_offset = 0);

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
    int simunsticktcfac(const char* i_tcfacname, int i_bitlength, const char* i_data, int i_row = 0, int i_numrows = 0); 

    //@}
    /* End Simulation Functions */

    /** @name Error Handling Functions */
    //@{

    /**
     @brief Retrieve additional error information for errorcode
     @param i_errorCode Error code to lookup up message for
     @retval point to NULL terminated string containing error data, NULL if error occurs

     */
    char* ecmdGetErrorMsg(int i_errorCode);


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

  private:
    int setupTarget(const char * i_targetStr, ecmdChipTarget & o_target);

};





/*** -------------------------------------- ***/
/*** The following is doxygen documentation ***/
/*** -------------------------------------- ***/


/**
 * @mainpage

<br>

@section perlapi eCMD Perl API
 In addition to a C/C++ API, eCMD provides a Perl API.  To do this, Perl's DynaLoader module handles the loading of a special eCMD shared object and the Perl XS interface handles the function calls between eCMD C code and Perl.
<br><br>
 The API interface is implemented as a "ecmdClientPerlapi" object.  The ecmdClientPerlapi class has methods to initialize eCMD, make standard function calls.  With the Perl API, scan data is passed with binary character strings between the client Perl script and the shared object's ecmdClientPerlapi class.

 <br>
 <hr>
 @section packs Packages

 To load the eCMD shared object, Perl scripts should <i>use</i> (or <i>require</i>) the Perl package appropriate for the desired environment (Aix or Linux).  Keep in mind the ECMD_DLL_FILE env variable must be set to the appropriate eCMD Plugin that you would like to run.

 <br><br>
 It is possible to use the <i>require</i> command in Perl to dynamically load the correct module for the emvironment you are running in, so your code will work across multiple platforms.  For example:

 <br><br>
 <pre>
 #!/usr/bin/perl

 my $osname = `uname`;
 chomp $osname;
 if ($osname eq "AIX") {
   require ecmd_aix_perl;
 } else {
   require ecmd_x86_perl;
 }    

 </pre>

 <br>
 <hr>
 @section perlv Perl Version
 Scripts need Perl version 5.8.1.  eCMD provides the proper version for both the AIX and Linux in all our supported sites and remote installs.  All scripts using the perl module should start with the following: 

 <pre>
 #!/bin/ksh 
 #! -*- perl -*-

 eval '
 if [ "X$CTEPATH" = "X" ]; then echo "CTEPATH env var is not set, please initialize your CTE environment"; exit 1; fi
 PERL5LIB=$CTEPATH/tools/perl/5.8.1/lib:$CTEPATH/tools/ecmd/rel/perlapi;
 export PERL5LIB;
 exec $CTEPATH/tools/perl/5.8.1/bin/perl -x -S $0 ${1+"$@"}
 '
 if 0;
 </pre>

 This picks the proper perl based upon if you're running AIX or Linux and runs our copy in CRONUS_HOME.

 <br>
 <hr>
 @section dform Data Passing
 Data is passed between the client Perl script and the eCMD shared object in the form of binary character strings.  To make data manipulation in Perl much easier we have provided the ecmdDataBuffer Perl module.  This allows you to have the same bit manipulation functions that are available on the C++ Api. <br>
Here is an example usage of the ecmdDataBuffer module:
<pre>

  # Create a pointer to an DataBuffer class
  my $data = new ecmdDataBuffer();
  # Set the first word of data in this class
  $data->setWord(0,0xFEEDBEEF);

  # Read data from the chip
  my $rc = 0;
  # The \$$ tells perl that we are passing in the class object by reference such that data can be returned out
  # The general rule is if the function is defined as char** in the Api documentation pass in your variable as &#92$$
  # If the function is defined as const char* then pass it in as $$ as seen below.
  $rc = $ep->getRing("pu -p1", "idreg", \$$data);

  # What is in the first word
  print("Data : $data->getWord(0));

  #Change the value
  $data->setWord(0,0xAAAA5555);
  printf("Data : %s\\n",$data->getWord(0));

  # Write my new value to the chip
  $rc = $ep->putRing("pu -p1", "idreg", $$data);
</pre>

 <br><br>
 <hr>
@section target Chip/Object targeting
Most functions take a const char* i_target argument which is where you specify the target that you would like to operate on.  There are two ways to do this, you can pass in the target as a string that is formatted as it would be on the command line (ie "pu -k2 -s3 -p2") or you can use the ChipTarget perl module provided.  This allows for easier manipulation of the target you are operating on.

<pre>
  my $target = newChipTarget("pu -p2");
  # The following two calls are equivalent
  $rc = $ep->putRing("pu -p2", "idreg", $$data);
  $rc = $ep->putRing($$target, "idreg", $$data);

  # Now I can go to the next position
  $target->pos(3);
  $rc = $ep->putRing($$target, "idreg", $$data);
</pre>


<br><br>
 <hr>
 @section perluse Perl API Usage

 The following should be observed when using the Perl API.

 <ol>
 <li> The client should instantiate a new ecmdClientPerlapi object.
 <li> The initDll() function should ALWAYS be the first function called.
 <li> Data is sent from Perl to Cronus via strings for functions like putalias, putscom, etc.
 <li> Perl should be picked up as shown in the example below to grab the eCMD supported version
 </ol>

 <br>
 <hr>
 @section perlscript Example Perl Script

 <pre>
 #!/bin/ksh 
 #! -*- perl -*-

 eval '
 if [ "X$CTEPATH" = "X" ]; then echo "CTEPATH env var is not set, please initialize your CTE environment"; exit 1; fi
 PERL5LIB=$CTEPATH/tools/perl/5.8.1/lib:$CTEPATH/tools/ecmd/rel/perlapi;
 export PERL5LIB;
 exec $CTEPATH/tools/perl/5.8.1/bin/perl -x -S $0 ${1+"$@"}
 '
 if 0;

 # Grab the additional eCMD Modules;
 require ecmdDataBuffer;
 require ChipTarget;

 my $osname = `uname`;
 my $ep;

 chomp $osname;
 if ($osname eq "AIX") {
   require ecmd_aix_perl; 
   $ep = new ecmd_aix_perl::ecmdClientPerlapi();
 } else {
   require ecmd_x86_perl; 
   $ep = new ecmd_x86_perl::ecmdClientPerlapi();
 }

 $| = 1;  # set autoflush

 if ($ep->initDll("")) { die "Fatal errors initializing DLL"; }


 # Setup my target
 my $target = new ChipTarget("pu");
 $target->pos(0);
 $target->core(0);
 $target->node(0);

 # Setup my data
 my $data = new ecmdDataBuffer();
 # Set the first word of data in this class
 $data->setWord(0,0xFEEDBEEF);

 my $rc = 0;

 # put/getscom
 if (1) {
    print "---- starting putscom -----\\n";
    $data->setWord(0, 0xabebfeef);
    printf("word 0 = %s\\n", $data->getWord(0));
    printf("word 1 = %s\\n", $data->getWord(1));
    printf("string = %s\\n",$data->genHexLeftStr(0,64));

    $rc = $ep->putScom($$target, "800000", $$data);
    printf("string = %s\\n",$data->getstr());

    print "---- starting getscom -----\\n";
    $rc = $ep->getScom($$target, "800000", \$$data);
    printf("word count= %d\n",$data->getWordLength());

    printf("getscom string word 0 = %s\\n", $data->getWord(0));
    printf("getscom genHLS word 0 = %s\\n", $data->genHexLeftStr(0,32));
    printf("getscom string word 1 = %s\\n", $data->getWord(1));
    printf("getscom genHLS word 1 = %s\\n", $data->genHexLeftStr(32,32));
    printf("getscom string word 2 = %s\\n", $data->getWord(2));
 }

 # put/getring
 if (0) {

    print "---- starting putring -----\\n";
    $data->setBitLength(32); # length of your ring goes here
    $data->flushTo0();
    $data->setBit(0, 10); # Set bits 0-10
 
    printf("putRing word 0 = %s\\n", $data->getWord(0));
    $rc = $ep->putRing($$target, "testring", $$data);
    if ($rc) {
       print "Problems calling eCMD putring\\n";
    }
    print "---- starting getRing -----\\n";
    $rc = $ep->getRing($$target, "testring", \$$data);
    printf("getRing word 0 = %s\\n", $data->getWord(0));
    printf("actual length  = %s\\n", $data->getBitLength());

 }


 $ep->cleanup();
 </pre>

*/


#endif /* _ecmdClientPerlapi_H */
