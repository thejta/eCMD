#ifndef _ecmdClientPerlApi_H
#define _ecmdClientPerlApi_H

/* $Header$ */
// Copyright **********************************************************
//
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 2002
//                                                                      
// The source code for this program is not published or otherwise       
// divested of its trade secrets, irrespective of what has been         
// deposited with the U.S. Copyright Office.                             
//                                                                      
// End Copyright ******************************************************

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------

// get rid of setbit macro define in /usr/include/sys/param.h
#ifdef setbit
#undef setbit
#endif

#include <ecmdDataBuffer.H>

typedef enum {
  DATAMODE_HEX_LEFT,
  DATAMODE_HEX_RIGHT,
  DATAMODE_DECIMAL,
  DATAMODE_BINARY,
  DATAMODE_DEFAULT
} DatastrFormatEnum;


class ecmdperl
{
public:
  ecmdperl(int i_numbits = 96);
  ~ecmdperl();

  /**
   @brief Perl Scripts should NOT call this function!
   @retval None

   - Perl scripts should ALWAYS call the DESTROY() function to cleanup their scripts - NEVER cleanup().  Core dumping will occur.
   */
  void cleanup();

  /**
   @brief Initialize the eCMD DLL
   @retval 0 if successful load  1 if unsuccessful
   @param i_options Passed from Command line Arguments
   @pre croperl constructor must have been called
   @post Global options (ex. -debug, -p#, -c#) will be removed from arg list

   - initializes eCMD objects in the DLL.
   - argc/argv get passed to the eCMD DLL.
   - Global options such as -debug flags and -p#, -c# will be parsed out.
   - Position flags can be queried later with functions like wasposselected()
   */

  int initDll(const char* i_options = NULL);

  /** @name Chip Position Functions */
  //@{

  /**
   * @brief Changes the default cage used in the functions
   * @retval 0 indicates command succeeded, else returns a eCMD/DLL error
   * @param i_newcage Cage number that is new default

   */
  int setdefaultcage(int i_newcage);

  /**
   * @brief Changes the default node used in the chip accessor functions
   * @retval 0 indicates command succeeded, else returns a eCMD/DLL error
   * @param i_newnode Node number that is new default

   */
  int setdefaultnode(int i_newnode);

  /**
   * @brief Changes the default pos used in the chip accessor functions
   * @retval 0 indicates command succeeded, else returns a eCMD/DLL error
   * @param i_newpos Pos number that is new default

   */
  int setdefaultpos(int i_newpos);

  /**
   * @brief Changes the default core used in the chip accessor functions
   * @retval 0 indicates command succeeded, else returns a eCMD/DLL error
   * @param i_newcore Core number that is new default

   */
  int setdefaultcore(int i_newcore);

  /**
   * @brief Return the maximum position valid for this chip
   * @retval maximum chip possible
   * @param i_chipname Name of chip
   * @param i_cage Cage number
   * @param i_node Node number
   *
   * - Use getError to check for errors
   */
  int getmaxpos(const char* i_chipname, int i_cage = -1, int i_node = -1);

  /**
   * @brief Return the maximum core valid for this chip and position
   * @retval maximum core possible
   * @param i_chipname Name of chip,
   * @param i_pos Chip position,
   * @param i_node Node number
   * @param i_cage Cage number
   *
   * - Presently, this function only applies to processor chips, i.e., chipname = "pu"
   * - Use getError to check for errors
   */
  int getmaxcore(const char* i_chipname, int i_cage = -1, int i_node = -1, int i_pos = -1);

  /**
   * @brief Checks to see if a chip with the given name exists in the system
   * @retval 0 if chip named does not exist, 1 if it does
   * @param i_chipname Name of chip
   * @param i_node Node number
   * @param i_cage Cage number
   *
   * - Use getError to check if i_node does not exist
   */
  int checkchipname(const char* i_chipname, int i_cage = -1, int i_node = -1);

  /**
   * @brief Checks to see if the selected cage is configured in the system
   * @retval 0 if the cage is not configured, 1 if it is
   * @param i_cage Cage number
   *
   * - Use getError to check if cage does not exist
   */
  int iscageconfigured(int i_cage = -1);

  /**
   * @brief Checks to see if the selected node is configured in the system
   * @retval 0 if the node is not configured, 1 if it is
   * @param i_node Node number
   * @param i_cage Cage number
   *
   * - Use getError to check if node does not exist
   */
  int isnodeconfigured(int i_cage = -1, int i_node = -1);

  /**
   * @brief Checks to see if a chip position is configured in the system
   * @retval 0 if the position is not configured, 1 if it is
   * @param i_chipname Name of chip
   * @param i_pos Position number to check 
   * @param i_node Node number
   * @param i_cage Cage number
   * @param i_len allows you to check multiple contiguous positions at the same time
   *
   * - Use getError to check if node does not exist
   */
  int isposconfigured(const char* i_chipname, int i_cage = -1, int i_node = -1, int i_pos = -1, int i_len = 1);

  /**
   * @brief Checks to see if a chip core is configured at a given position in the system
   * @retval 0 if the core is not configured, 1 if it is
   * @param i_chipname Name of chip
   * @param i_pos Position number to check
   * @param i_core Core number to check
   * @param i_node Node number
   * @param i_cage Cage number
   * @param i_len allows you to check multiple contiguous cores at the same time
   *
   * - Presently, this function only applies to processor chips, i.e., chipname = "pu"
   * - Use getError to check if node does not exist
   */
  int iscoreconfigured(const char* i_chipname, int i_cage = -1, int i_node = -1, int i_pos = -1, int i_core = -1, int i_len = 1);

  /**
   * @brief Checks to see if the user selected the given cage when the initDll function was called
   * @retval 0 if the cage was not passed in to initDll with a -n# arg, 1 if it was
   * @param i_cage Cage number
   *
   * - Use getError to check if i_node does not exist
   */
  int wascageselected(int i_cage);

  /**
   * @brief Checks to see if the user selected the given node when the initDll function was called
   * @retval 0 if the node was not passed in to initDll with a -n# arg, 1 if it was
   * @param i_node Node number
   * @param i_cage Cage number
   *
   * - Use getError to check if node does not exist
   */
  int wasnodeselected(int i_cage, int i_node);

  /**
   * @brief Checks to see if the user selected the given position when the initDll function was called
   * @retval 0 if the pos was not passed in to initDll with a -p# arg, 1 if it was
   * @param i_cage Cage number
   * @param i_node Node number
   * @param i_pos Position number
   *
   * - Use getError to check if node does not exist
   */
  int wasposselected(int i_cage, int i_node, int i_pos);

  /**
   * @brief Checks to see if the user selected the given core when the initDll function was called
   * @retval 0 if the core was not passed in to initDll with a -c# arg, 1 if it was
   * @param i_cage Cage number
   * @param i_node Node number
   * @param i_pos Position number
   * @param i_core Core number
   *
   * - Use getError to check if node does not exist
   */
  int wascoreselected(int i_cage, int i_node, int i_pos, int i_core);

  //@}


  /** @name Chip Functions */
  //@{

  /**
   * @brief performs a scom operation on the selected chip and address
   * @retval Scombits retrieved from address
   * @param i_chipname Name of chip
   * @param i_addr Scom address
   * @param i_pos Position number
   * @param i_core Core number
   * @param i_node Node number
   * @param i_cage Cage number

   - addr is right-aligned
   - data returned is right-aligned hex by default (can be changed with setdatamode())
   - use getError to check for errors
   */
  char* getscom(const char* i_chipname, const char* i_addr, int i_cage = -1, int i_node = -1, int i_pos = -1, int i_core = -1);

  /**
   * @brief performs a scom operation on the selected chip and address, inserting data from datastr
   * @retval 0 if no errors, non-zero if node/chip errors or error inside eCMD DLL
   * @param i_chipname Name of chip
   * @param i_addr Scom address
   * @param i_datastr string containing data to insert
   * @param i_cage Cage number
   * @param i_pos Position number
   * @param i_core Core number
   * @param i_node Node number

   - addr is right-aligned
   - datastr is right-aligned hex by default (can be changed with setdatamode())
   - use getError to check for errors
   */
  int putscom(const char* i_chipname, const char* i_addr, const char* i_datastr, int i_cage = -1, int i_node = -1, int i_pos = -1, int i_core = -1);

  /**
   * @brief performs a scan operation on the selected chip and ring and returns the selected bits
   * @retval Scanbits returned from ring
   * @param i_chipname Name of chip
   * @param i_ringname Name of ring
   * @param i_startbit bit position in the ring to start retrieving data from
   * @param i_numbits number of valid bits to fetch 
   * @param i_cage Cage number
   * @param i_pos Position number
   * @param i_core Core number
   * @param i_node Node number

   - data returned is binary by default (can be changed with setdatamode())
   - use getError to check for errors
   */
  char* getbits(const char* i_chipname, const char* i_ringname, int i_startbit, int i_numbits, int i_cage = -1, int i_node = -1, int i_pos = -1, int i_core = -1);  

  /**
   * @brief performs a scan operation on the selected chip and ring, inserting data from the datastr.
   * @retval 0 if no errors, non-zero if node/chip errors or error inside eCMD DLL
   * @param i_chipname Name of chip
   * @param i_ringname Name of ring
   * @param i_startbit bit position in the ring to start inserting data at
   * @param i_numbits number of valid bits in datastr
   * @param i_datastr string of data to insert
   * @param i_cage Cage number
   * @param i_pos Position number
   * @param i_core Core number
   * @param i_node Node number

   - datastr is binary by default (can be changed with setdatamode())
   - use getError to check for errors
   */
  int putbits(const char* i_chipname, const char* i_ringname, int i_startbit, int i_numbits, const char* i_datastr, int i_cage = -1, int i_node = -1, int i_pos = -1, int i_core = -1);

  /**
   * @brief Reads a Spy from the selected chip
   * @retval Data returned from the spy
   * @param i_chipname Name of chip
   * @param i_spyname Name of alias
   * @param i_startbit Bit position in the spy to start returning data from
   * @param i_numbits Number of bits to retrieve from spy
   * @param i_cage Cage number
   * @param i_pos Position number
   * @param i_core Core number
   * @param i_node Node number

   - data returned is left-aligned hex by default (can be changed with setdatamode())
   - use getError to check for errors
   */
  char* getspy(const char* i_chipname, const char* i_aliasname, int i_startbit = -1, int i_numbits = -1, int i_cage = -1, int i_node = -1, int i_pos = -1, int i_core = -1); 

  /**
   * @brief Reads an enumerated Spy from the selected chip
   * @retval Data returned from the spy
   * @param i_chipname Name of chip
   * @param i_spyname Name of alias
   * @param i_startbit Bit position in the spy to start returning data from
   * @param i_numbits Number of bits to retrieve from spy
   * @param i_cage Cage number
   * @param i_pos Position number
   * @param i_core Core number
   * @param i_node Node number

   - data returned is left-aligned hex by default (can be changed with setdatamode())
   - use getError to check for errors
   */
  char* getspyenum(const char* i_chipname, const char* i_aliasname, int i_startbit = -1, int i_numbits = -1, int i_cage = -1, int i_node = -1, int i_pos = -1, int i_core = -1); 

  /**
   * @brief performs a scan operation on the selected chip and alias from the alias file, inserting data from datastr.
   * @retval 0 if no errors, non-zero if node/chip errors or error inside eCMD DLL
   * @param i_chipname Name of chip
   * @param i_aliasname Name of alias
   * @param i_startbit bit position in the alias to start inserting data at
   * @param i_numbits number of valid bits in datastr
   * @param i_datastr data to insert
   * @param i_cage Cage number
   * @param i_pos Position number
   * @param i_core Core number
   * @param i_node Node number

   - datain is left-aligned hex by default (can be changed with setdatamode())
   - use getError to check for errors
   */
  int putalias(const char* i_chipname, const char* i_aliasname, int i_startbit, int i_numbits, const char* i_datastr, int i_cage = -1, int i_node = -1, int i_pos = -1, int i_core = -1);

  /**
   * @brief performs a scan operation on the selected chip and ring, inserting the pattern repeatedly for the length of the scan ring.
   * @retval 0 if no errors, non-zero if node/chip errors or error inside eCMD DLL
   * @param i_chipname Name of chip
   * @param i_ringname Name of ring
   * @param i_pattern Pattern that gets repeated for the length of the ring
   * @param i_pos Position number
   * @param i_core Core number
   * @param i_node Node number

   - pattern is left-aligned hex by default (can be changed with setdatamode())
   - use getError to check for errors
   */

  int putpattern(const char* i_chipname, const char* i_ringname, const char* i_pattern, int i_cage = -1, int i_node = -1, int i_pos = -1, int i_core = -1);

  /**
   * @brief performs a array alter operation on the selected chip and array.
   * @retval 0 if no errors, non-zero if node/chip errors or error inside eCMD DLL
   * @param i_chipname Name of chip
   * @param i_arrayname Name of array from array file
   * @param i_addr Address within array to alter
   * @param i_data string containing data to be inserted at array address
   * @param i_pos Position number
   * @param i_core Core number
   * @param i_node Node number
   * @see getarray

   - writes one address at a time
   - address is right-aligned
   - data is left-aligned hex by default (can be changed with setdatamode())
   - use getError to check for errors
   */
  int putarray(const char* i_chipname, const char* i_arrayname, const char* i_addr, const char* i_data, int i_cage = -1, int i_node = -1, int i_pos = -1, int i_core = -1);

  /**
   * @brief performs a array display operation on the selected chip and array.
   * @retval Data returned from the array
   * @param i_chipname Name of chip
   * @param i_arrayname Name of array from array file
   * @param i_addr Address within array to display
   * @param i_pos Position number
   * @param i_core Core number
   * @param i_node Node number
   * @see putarray

   - reads one address at a time
   - address is right-aligned
   - data returned is left-aligned hex by default (can be changed with setdatamode())
   - use getError to check for errors
   */
  char* getarray(const char* i_chipname, const char* i_arrayname, const char* i_addr, int i_cage = -1, int i_node = -1, int i_pos = -1, int i_core = -1);

  /**
   * @brief issues an ACCESS command to the specified chip and returns the status of the command
   * @retval A hex string representing the status of the ACCESS command issued
   * @param i_chipname Name of chip
   * @param i_command ACCESS command to issue
   * @param i_pos Position number
   * @param i_core Core number
   * @param i_node Node number

   - command is right-aligned hex by default (can be changed with setdatamode())
   - status data returned is right-aligned hex by default (can be changed with setdatamode())
   - use getError to check for errors
   */
  char* sendcmd(const char* i_chipname, const char* i_command, int i_cage = -1, int i_node = -1, int i_pos = -1); 

  //@}

  /** @name Ring Functions */
  // @{

  /**
   * @brief returns length of specified ring on chip
   * @retval 0 if there was an error, else length of ring on chip
   * @param i_chipname Name of chip
   * @param i_ringname Name of ring
   * @param i_pos Position number
   * @param i_core Core number
   * @param i_node Node number

   - use getError to check for errors
   */
  int getringlength(const char* i_chipname, const char* i_ringname, int i_cage = -1, int i_node = -1, int i_pos = -1, int i_core = -1);


  //@}

  /** @name Ring Cache Functions */
  // @{

  /**
   * @brief enables internal caching of reads/writes to the chips through chip functions and spr functions
   * @retval 0 if no errors, non-zero if node/chip errors or error inside eCMD DLL
   * @see disableScanDataCache
   * @see flushScanDataCache

   - calling this function enables eCMD to internally cache rings that it reads out from the chip, enabling users to drastically improve performance when they are doing several getbits/getalias/getspr calls.
   - There are several "gotchas" associated with using scan data caches- you will not actually write any data out to the chip(s) until you call flushDataCache(), or if you use scan data cache's with the array/gpr/fpr functions, which must write to the chip in order to work.
   - Please use with care.
   */

  int enableDataCache ();

  /**
   * @brief disables internal caching of reads/writes to the chips through chip functions and spr functions
   * @retval 0 if no errors, non-zero if node/chip errors or error inside eCMD DLL
   * @see enableDataCache
   * @see flushDataCache

   - turns off the internal ring caching enabled by useScanDataCache
   - this function does not actually flush the cached data to the chip, flushScanDataCache does that.
   */
  int disableDataCache ();

  /**
   * @brief writes out the data cached in the scandata cache out to the chip.
   * @retval 0 if no errors, non-zero if node/chip errors or error inside eCMD DLL
   * @see enableDataCache
   * @see disableDataCache

   - This function will not disable internal caching, even though it will write out any rings currently in the internal cache.
   - disableDataCache() will actually turn off the internal caching functionality.
   */
  int flushDataCache ();

  //@}

  /** @name System Functions */
  //@{

  /** @name Data Accessor Functions

   - Internally, the croperl interface contains 10 bitvectors that can be referenced individually and updated using these functions.

   */

  //@{

  /**
   * @brief Returns the number of 32-bit words of space available in a bitvector
   * @retval Number of words
   * @param i_idx Index (0-9) of bitvector to check

   */
  int   getnumwords(int i_idx = -1);

  /**
   * @brief Returns the number of bits of space available in a bitvector
   * @retval Number of bits
   * @param i_idx Index (0-9) of bitvector to check

   */
  int   getnumbits(int i_idx = -1);

  /**
   * @brief Sets a sequence of bits in a bitvector to the value in binarystr
   * @retval none
   * @param i_offset Offset (from left-aligned 0) in bitvector to start writing the pattern at
   * @param i_binarystr Pattern of data to write
   * @param i_idx Index (0-9) of bitvector to check

   */
  void  setbit(int i_offset, const char* i_binarystr, int i_idx = -1);

  /**
   * @brief Clears a sequence of bits in a bitvector
   * @retval none
   * @param i_offset Offset (from left-aligned 0) in bitvector to start clearing from
   * @param i_numbits Number of bits (starting from offset) to clear
   * @param i_idx Index (0-9) of bitvector to check

   */
  void  clearbit(int i_offset, int i_numbits, int i_idx = -1);

  /**
   * @brief Checks to see whether a particular sequence of bits are all "1"
   * @retval 1 if all bits in the sequence are "1", 0 otherwise
   * @param i_offset Offset (from left-aligned 0) in bitvector to start checking from
   * @param i_numbits Number of bits (starting from offset) to check
   * @param i_idx Index (0-9) of bitvector to check

   */
  int   isbitset(int i_offset, int ni_umbits, int i_idx = -1);

  /**
   * @brief Checks to see whether a particular sequence of bits are all "0"
   * @retval 1 if all bits in the sequence are "0", 0 otherwise
   * @param i_offset Offset (from left-aligned 0) in bitvector to start checking from
   * @param i_numbits Number of bits (starting from offset) to check
   * @param i_idx Index (0-9) of bitvector to check

   */
  int   isbitclear(int i_offset, int i_numbits, int i_idx = -1);

  /**
   * @brief Counts to see how many bits from [offset,offset+numbits-1] are set
   * @retval Number of bits in the interval that are equal to "1"
   * @param i_offset Offset (from left-aligned 0) in bitvector to start checking from
   * @param i_numbits Number of bits (starting from offset) to check
   * @param i_idx Index (0-9) of bitvector to check

   */
  int   getnumbits_set(int i_offset, int i_numbits, int i_idx = -1);

  /**
   * @brief Sets one of the 32-bit words in the selected bitvector to the value in datastr
   * @retval none
   * @param i_offset Word to write
   * @param i_datastr 8-char hex string to write
   * @param i_idx Index (0-9) of bitvector to check

   */
  void  setword (int i_offset, const char* i_datastr, int i_idx = -1);

  /**
   * @brief Returns the value of one of the 32-bit words in the selected bitvector
   * @retval 32-bit INTEGER containing value of selected word
   * @param i_offset Word to read
   * @param i_idx Index (0-9) of bitvector to check

   - NOTE:  DOES NOT RETURN A CHAR STRING.
   */
  unsigned int getword (int i_offset, int i_idx = -1); /* data returned as a 32 bit int */

  /**
   * @brief Sets a sequence of bits in the selected bitvector to the value in datastr
   * @retval none
   * @param i_offset Bit offset (from left-aligned 0) to start writing at
   * @param i_numbits Number of bits to write
   * @param i_datastr Pattern of data to write- must be "1"'s and "0"'s
   * @param i_idx Index (0-9) of bitvector to check

   */
  void  setdata(int i_offset, int i_numbits, const char* i_datastr, int i_idx = -1);

  /**
   * @brief Reads a sequence of bits in the selected bitvector
   * @retval Data read, format of data is default or changed with setdatamode()
   * @param i_offset Bit offset (from left-aligned 0) to start reading from
   * @param i_numbits Number of bits to read
   * @param i_idx Index (0-9) of bitvector to check

   */
  char* getdata(int i_offset, int i_numbits, int i_idx = -1);

  /**
   * @brief Flush selected bitvector to all zeros
   * @retval none
   * @param i_idx Index (0-9) of bitvector to flush

   */
  void  flushto0(int i_idx = -1);

  /**
   * @brief Flush selected bitvector to all ones
   * @retval none
   * @param i_idx Index (0-9) of bitvector to flush

   */
  void  flushto1(int i_idx = -1);

  /**
   * @brief Create a new bitvector at the specified idx
   * @retval none
   * @param i_idx Index of new bitvector to create
   * @param i_numbits Number of bits to create the new bitvector with

   - Please only use this function if you know what you're doing.
   */
  void  initbitvector(int i_idx, int i_numbits);

  /**
   * @brief Change the default bitvector to use in the data functions
   * @retval none
   * @param i_newdefault Index of the default bitvector to use in data accessor functions

   - If no idx is specified in the above functions, the default bitvector idx (usually 0) is used.
   - This function allows you to change what that default is.
   - Please only use this function if you know what you're doing.
   */
  void  setdefaultbitvector(int i_newdefault);

  //@}


  /**
   * @brief Saves away the value of Croperl internal variables
   * @retval None
   * @see restorestate

   - The idea here is to be able to change the data format within a subroutine call without affecting the calling code.
   - Effectively checkpoints the internal state of the croperl object, so it can be restored with restorestate().
   */
  void savestate();


  /**
   * @brief Restores the value of Croperl internal variables saved with savestate()
   * @retval None
   * @see savestate

   - If you call restorestate() without having called savestate(), the croperl object will revert to the initialization state.
   */
  void restorestate();


  /** @name Simulation Functions */
  //@{

  //@}


  /** @name eCMD Environment Functions */
  // @{

  /**
   * @brief Sets the preferred format of data returned from function calls
   * @retval none
   * @param i_newmode Can take values "default", "hexleft", "hexright", "decimal", or "binary". 

   */
  void  setdatamode(const char* i_newmode);


  //@}

  /** @name Error Handling */
  //@{

  /** 
   @brief The value of the current DLL error code
   @retval DLL error variable, or 0 if there are no errors

   - The DLL error variable "freezes" on the first error
   */   
  int getError() { return i_myErr; } /* return > 0 if error */

  /**
   * @brief The error message corresponding to the current DLL error
   * @retval DLL error string

   - Croperl CLEARS the value of the error and the string once you call this function.
   - valid only if getDllError() returns non-zero.
   - String is the first error logged.
   */
  char* getErrorStr();

  /**
   * @brief Clears internal error state
   * @retval none

   - Croperl CLEARS the value of the error and the string once you call this function.
   */
  void clearError();

  /**
   * @brief Used for debugging Perl interface by dumping its internal state
   * @retval none
   * @param i_numwords Amount of data to dump

   */
  void dump(int i_numwords = 3); 

  //@}



private:

  /* --------- */
  /* functions */
  /* --------- */
  croperl(croperl & me);
  void setErr(int err, const char* str);

  /* ---- */
  /* data */
  /* ---- */
  int myDefPos;
  int myDefCore;
  int myDefNode;
  DatastrFormatEnum myDataFormat;
  DatastrFormatEnum bakDataFormat;

  /* C <-> Perl data storage structures */
  bitvector* myData;
  bitvector** myDataList;
  int myDefBitvector;
  char* myDataStr; /* this is only updated when needed */

  /* Error handling */    
  unsigned int myErr;
  char* myErrStr;
  char* myAuxErrStr;
  char* myDebugStr;
  int printerror;

  /* debug */
  unsigned int croperl_dbg;

  int NUM_BITVECTORS;
};

/**
 * @mainpage

<br>

@section perlapi eCMD Perl API
 In addition to a C/C++ API, eCMD provides a Perl API.  To do this, Perl's DynaLoader module handles the loading of a special eCMD shared object and the Perl XS interface handles the function calls between eCMD C code and Perl.
<br><br>
 The API interface is implemented as a "croperl" object.  The croperl class has methods to initialize Cronus, make standard function calls, and manipulate data.  This is in contrast to the C/C++ DLL, where data manipulation is handled with the bitvector class.  With the Perl API, scan data is passed with character strings between the client Perl script and the shared object's croperl class.

 <br>
 <hr>
 @section packs Packages

 To load the Cronus shared object, Perl scripts should <i>use</i> (or <i>require</i>) the Perl package appropriate for the desired environment (hw/sim), code level (usr, dev, rel) and environment (AIX or Linux).  The packages are named like the following:
 <br>
 <ul>
 <li> dll_perlintf[hw|sim]_[level].pm  (for AIX)
 <li> dll_perlintf[hw|sim]_[levlel]_linux.pm (for Linux)
 </ul>
 <br>
 The packages use DynaLoader to load the corresponding Cronus shared object.  The name of the packages are the same as the names of the Cronus shared objects and are tied together both in the package and the shared object.
 <br><br>
 It is possible to use the <i>require</i> command in Perl to dynamically load the correct module for the emvironment you are running in, so your code will work across multiple platforms.  For example:

 <br><br>
 <pre>
 #!/usr/bin/perl

 my $cronusexe = $ENV{"CRONUS_EXE"};
 @filestat = stat("/usr/include/linux/zorro.h");
 if ($cronusexe =~ /sim/) {
    if (@filestat[1] eq "") {
        require dll_perlintfsim_dev;
    } else {
        require dll_perlintfsim_dev_linux;
    }    
 } else {
    if (@filestat[1] eq "") {
        require dll_perlintfhw_dev;
    } else {
        require dll_perlintfhw_dev_linux;
    }
 }

 </pre>

 <br>
 <hr>
 @section perlv Perl Version
 Scripts need Perl version 5.005_02 or greater for AIX, version 5.6.1 or greater for Linux.  You can check the version of your perl execuatble by typing "perl -version".  Some good places to look are:

 <ul>
 <li> In Rochester DFS:  /.../rchland.ibm.com/fs/rs_aix43/contrib/perl/5.005_02/bin/perl
 <li> In Austin AFS AWD: /afs/awd/projects/gr/lab/local/perl5/bin/perl
 <li> On any Linux machine:  /usr/bin/perl
 </ul>

 <br>
 <hr>
 @section dform Data Format
 Data is passed between the client Perl script and the Cronus shared object in the form of character strings.  In the "default" mode, data is formatted as in the table below.  It is possible to change the default mode by calling the croperl setdefaultmode() function.  Allowed values are "default", "hexleft", "hexright", "binary", or "decimal".

 <br><br>
 <table BORDER=1 WIDTH="75%">
 <tr ALIGN=CENTER bgcolor="#999996">
 <td ALIGN=CENTER><b>Function</b>
 <td ALIGN=CENTER><b>Default Data Format</b>
 </tr>
 <tr ALIGN=CENTER bgcolor="#666633">
 <td><b>getscom</b>
 <td><b>right-aligned hex</b>
 </tr>
 <tr ALIGN=CENTER bgcolor="#666633">
 <td><b>putscom</b>
 <td><b>right-aligned hex</b>
 </tr>
 <tr ALIGN=CENTER bgcolor="#666633">
 <td><b>getbits</b>
 <td><b>binary</b>
 </tr>
 <tr ALIGN=CENTER bgcolor="#666633">
 <td><b>putbits</b>
 <td><b>binary</b>
 </tr>
 <tr ALIGN=CENTER bgcolor="#666633">
 <td><b>getalias</b>
 <td><b>left-aligned hex</b>
 </tr>
 <tr ALIGN=CENTER bgcolor="#666633">
 <td><b>putalias</b>
 <td><b>left-aligned hex</b>
 </tr>
 <tr ALIGN=CENTER bgcolor="#666633">
 <td><b>putpattern</b>
 <td><b>left-aligned hex</b>
 </tr>
 <tr ALIGN=CENTER bgcolor="#666633">
 <td><b>getarray</b>
 <td><b>left-aligned hex</b>
 </tr>
 <tr ALIGN=CENTER bgcolor="#666633">
 <td><b>putarray</b>
 <td><b>left-aligned hex</b>
 </tr>
 <tr ALIGN=CENTER bgcolor="#666633">
 <td><b>sendcmd (both the command the status received)</b>
 <td><b>right-aligned hex</b>
 </tr>
 <tr ALIGN=CENTER bgcolor="#666633">
 <td><b>spr/gpr/fpr commands</b>
 <td><b>right-aligned hex</b>
 </tr>
 <tr ALIGN=CENTER bgcolor="#666633">
 <td><b>getmem (addr and data returned)</b>
 <td><b>right-aligned hex</b>
 </tr>
 <tr ALIGN=CENTER bgcolor="#666633">
 <td><b>putmem (addr and datastr to write)</b>
 <td><b>right-aligned hex</b>
 </tr>
 <tr ALIGN=CENTER bgcolor="#666633">
 <td><b>getdma (addr and data returned)</b>
 <td><b>right-aligned hex</b>
 </tr>
 <tr ALIGN=CENTER bgcolor="#666633">
 <td><b>putdma (addr and datastr to write)</b>
 <td><b>right-aligned hex</b>
 </tr>
 </table>

 <br><br>
 <hr>
 @section perluse Perl API Usage

 The following should be observed when using the Perl API.

 <ol>
 <li> The client should instantiate a new croperl object.
 <li> The initDll() function should ALWAYS be the first function called from croperl.  A string of init options can be passed to it, e.g., "initDll("-debug1.5")".
 <li> Global Cronus options like "-p1" or "-n0" also get parsed by initDll().  They get stored in a data structure inside of Cronus can can be queried using the wasposselected() function.  The selected positions can be passed back into Cronus during chip calls like getbits or putalias.
 <li> Data is sent from Perl to Cronus via strings for functions like putalias, putscom, etc.
 <li> When using the API, the cronus cnfg file is read only once.  If any changes to the file are made, the perl interface needs to be shutdown and restarted to pick them up.
 <li> Errors should be checked using the getError() and getErrorStr() methods.
 </ol>

 <br>
 <hr>
 @section perlscript Example Perl Script

 <pre>
 #!/.../rchland.ibm.com/fs/rs_aix43/contrib/perl/5.005_02/bin/perl

 BEGIN {unshift @INC, "/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/squadrons/dll"}

 use dll_perlintfhw_dev;

 $| = 1;  # set autoflush

 my $cp = new croperl();
 my $options = "-debug1"; # allows passing in -debug and -p# from command line
 if ($cp->initDll($options)) { die "Fatal errors initializing DLL"; }


 my $datastr = ();
 #$cp->displayversion();
 $cp->setCroperlDebug(1);
 $cp->setCroperlDebug(8);

 my $chip = "pu";
 $cp->setdefaultpos(0);
 $cp->setdefaultcore(0);
 $cp->setdefaultnode(0);
 if (!($cp->checkchipname($chip))) { die "Unknown chip: $chip!"; }

 #$cp->dump(4);
 $cp->setDebug("15.1"); # force some debug flags

 # system functions
 if (0) {
    my $r = $cp->getdata(0,4,0);
    print "R= $r";	
 }

 # put/getscom
 if (0) {
    print "---- starting putscom -----";
    $cp->putscom($chip, "800000", "abefeef");
    $cp->setdatamode("binary");
    print "---- starting getscom -----";
    $datastr = $cp->getscom($chip, "800000");
    print "getscom string = $datastr";
    print "getscom 800000 = " . sprintf("%0.8X %0.8X\\n", $cp->getword(0), $cp->getword(1));
 }

 # put/getbits   
 if (0) {
 #    $cp->useScanDataCache();
    print "---- starting putbits -----";
    my $thispos = 0;
    $cp->setdatamode("binary");
    $cp->putbits($chip, "cp_ifu", 0, 12, "101011110101", $thispos);
    if ($cp->getError()) {
       print "err(" . $cp->getError() . "):" . $cp->getErrorStr() . "\\n";
    }
    print "---- starting getbits -----";
    $cp->setdatamode("hexleft");
    $datastr = $cp->getbits($chip, "cp_ifu", 0, 16, $thispos);
    print "getbits string = $datastr\\n";

    print "---- starting putpattern -----";
    $cp->setdatamode("binary");
    $cp->putpattern($chip, "cp_ifu", "10100000", $thispos);
    $datastr = $cp->getbits($chip, "cp_ifu", 0, 16, $thispos);
    print "getbits data = " . sprintf("%0.8X %0.8X\\n", $cp->getword(0), $cp->getword(1));
    $cp->setdatamode("hexleft");
    $cp->putpattern($chip, "cp_ifu", "facef00d", $thispos);
    $datastr = $cp->getbits($chip, "cp_ifu", 0, $cp->getringlength($chip, "cp_ifu", $thispos), $thispos); 
    print "getbits data = " . sprintf("%0.8X %0.8X %0.8X %0.8X\\n", $cp->getword(0), $cp->getword(1), $cp->getword(2), $cp->getword(3));
 #    $cp->flushScanDataCache("pu",0);
 }

 # put/getalias
 if (1) {
    $cp->enableScanDataCache();
    print "---- starting putalias -----";
    $cp->setdatamode("binary");
    $cp->putalias($chip, "HID1", 0, 8, "10101111");
    print "---- starting getalias -----";
    $datastr = $cp->getalias($chip, "HID1", 0, 6);
    print "getalias string = $datastr";
    $cp->flushScanDataCache("pu",0);
 }

 #put/getarray
 if (0) {
    $cp->clearDebug("8");
    $cp->putarray("pu","ific0", "03f", "feedbeef");
    if ($cp->getError()) {
	print "err: " . $cp->getError() . ":" . $cp->getErrorStr() . "\\n";
    }
    $datastr = $cp->getarray("pu","ific0", "35b");

 }

 $cp->cleanup();
 </pre>

*/

#endif /* _dll_perlintf_h */

