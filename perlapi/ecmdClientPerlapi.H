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



extern "C" int ecmdPerlInterfaceErrorCheck (int ec);

class ecmdClientPerlapi
{
  public:

  /** @name Initialzation Functions */
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


    void cleanup();
  //@}



    /** @name Chip Functions */
    //@{ 

  /**
   * @brief Performs a scom operation on the selected chip and address
   * @retval Scombits retrieved from address
   * @param i_target Chip Target to operate on: ie "pu -n2 -p3 -c1"
   * @param i_address Scom address

   - Data format for all functions is hex-left aligned to change see ecmdSetDataMode function
   - use getError to check for errors
   */
    char * getScom(const char * i_target, int i_address);

  /**
   * @brief performs a scom operation on the selected chip and address, inserting data from datastr
   * @retval 0 if no errors, non-zero if node/chip errors or error inside eCMD DLL
   * @param i_target Chip Target to operate on: ie "pu -n2 -p3 -c1"
   * @param i_address Scom address
   * @param i_data string containing data to insert

   - Data format for all functions is hex-left aligned to change see ecmdSetDataMode function
   - use getError to check for errors
   */
    int putScom(const char * i_target, int i_address, const char * i_data);

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

 NOTE : This function forces a flush of the ring caches


*/
/*    char* getRing (const char * i_target, const char * i_ringName); */
    int getRing (const char * i_target, const char * i_ringName, char **o_data); 

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

 NOTE : This function forces a flush of the ring caches


*/
    int putRing (const char * i_target, const char * i_ringName, const char * i_data); 


//@}



  /**
   * @brief issues an ACCESS command to the specified chip and returns the status of the command
   * @retval A hex string representing the status of the ACCESS command issued
   * @param i_target Chip Target to operate on: ie "pu -n2 -p3 -c1"
   * @param i_command 32 bit ACCESS command to issue

   - Data format for all functions is hex-left aligned to change see ecmdSetDataMode function
   - use getError to check for errors
   */

/***
  char* sendcmd(const char * i_target, const char* i_command); 
***/

    //@}

  /** @name Data Functions */
  //@{

  /**
   * @brief Sets the preferred format of data passed to and returned from function calls
   * @retval none
   * @param i_newmode Can take any value supported by the -o command line arg
   - Examples 'xl', 'xr', 'b'
   */


/***
  void  ecmdSetDataMode(const char* i_newmode);
***/


  //@}

    void add(char *retval);

    void add2(char **retval);


  private:
    int setupTarget(const char * i_targetStr, ecmdChipTarget & o_target);

    ecmdChipTarget myTarget;
    std::string myFormat;
    std::string dataStr;

};

#endif /* _ecmdClientPerlapi_H */
