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

  /**
   * @brief performs a scan operation on the selected chip and ring and returns the selected bits
   * @retval Scanbits returned from ring
   * @param i_target Chip Target to operate on: ie "pu -n2 -p3 -c1"
   * @param i_ringname Name of ring
   * @param i_startbit bit position in the ring to start retrieving data from
   * @param i_numbits number of valid bits to fetch 

   - Data format for all functions is hex-left aligned to change see ecmdSetDataMode function
   - use getError to check for errors
   */
  char* getbits(const char * i_target, const char* i_ringname, int i_startbit, int i_numbits);  

  /**
   * @brief performs a scan operation on the selected chip and ring, inserting data from the datastr.
   * @retval 0 if no errors, non-zero if node/chip errors or error inside Cronus DLL
   * @param i_target Chip Target to operate on: ie "pu -n2 -p3 -c1"
   * @param i_ringname Name of ring
   * @param i_startbit bit position in the ring to start inserting data at
   * @param i_numbits number of valid bits in datastr
   * @param i_datastr string of data to insert

   - Data format for all functions is hex-left aligned to change see ecmdSetDataMode function
   - use getError to check for errors
   */
  int putbits(const char * i_target, const char* i_ringname, int i_startbit, int i_numbits, const char* i_datastr);

  /**
   * @brief issues an ACCESS command to the specified chip and returns the status of the command
   * @retval A hex string representing the status of the ACCESS command issued
   * @param i_target Chip Target to operate on: ie "pu -n2 -p3 -c1"
   * @param i_command 32 bit ACCESS command to issue

   - Data format for all functions is hex-left aligned to change see ecmdSetDataMode function
   - use getError to check for errors
   */
  char* sendcmd(const char * i_target, const char* i_command); 


    //@}

  /** @name Data Functions */
  //@{

  /**
   * @brief Sets the preferred format of data passed to and returned from function calls
   * @retval none
   * @param i_newmode Can take any value supported by the -o command line arg
   - Examples 'xl', 'xr', 'b'
   */
  void  ecmdSetDataMode(const char* i_newmode);
  
  //@}


  private:
    int setupTarget(const char * i_targetStr, ecmdChipTarget & o_target);

    ecmdChipTarget myTarget;
    std::string myFormat;
    std::string dataStr;

};

#endif /* _ecmdClientPerlapi_H */
