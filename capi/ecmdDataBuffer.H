#ifndef ecmdDataBuffer_H 
#define ecmdDataBuffer_H

// Copyright **********************************************************
//                                                                      
// File ecmdDataBuffer.H                                               
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

/* $Header$ */

/**
 * @file ecmdDataBuffer.H
 * @brief Provides a means to handle data from the eCMD C API
 *
 * DataBuffers handle and store data in a Big Endian fashion with Bit 0 being the MSB
*/


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <string>
#include <inttypes.h>

//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------

//--------------------------------------------------------------------
// Macros
//--------------------------------------------------------------------

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  User Types
//----------------------------------------------------------------------


class ecmdDataBuffer {

public:

  /** @name ecmdDataBuffer Constructors */
  //@{
  /**
   * @brief Default Constructor
   * @post buffer is not allocated, can be allocated later with setWordLength, setCapacity or setBitLength
   */
  ecmdDataBuffer();

  /**
   * @brief Constructor
   * @param i_numWords Size of data to initialize in 32-bit words
   * @post ecmdDataBuffer is initialzed with a buffer
   */
  ecmdDataBuffer(int i_numWords); 


  /**
   * @brief Copy Constructor
   * @param other Buffer to copy
   */
  ecmdDataBuffer(const ecmdDataBuffer &other);

  /**
   * @brief Default Destructor
   */
  ~ecmdDataBuffer();
  //@}


  // Member Functions
  /** @name Buffer Size Function */
  //@{
  /**
   * @brief Return the length of the buffer in words
   * @retval Buffer length in words rounded up
   */
  int   getWordLength() const;
  /**
   * @brief Return the length of the buffer in bytes
   * @retval Buffer length in bytes rounded up
   */
  int   getByteLength() const;
  /**
   * @brief Return the length of the buffer in bits
   * @retval Buffer length in bits
   */
  int   getBitLength() const;
  /**
   * @brief Return the actual capacity of the internal buffer in words
   * @retval Actual capacity of internal buffer
   */
  int   getCapacity() const;
  /**
   * @brief Reinitialize the Buffer to specified length
   * @param i_newNumWords Length of new buffer in words
   * @post Buffer is reinitialized
   *
   * CAUTION : All data stored in buffer will be lost
   */
  void  setWordLength(int i_newNumWords);   
  /**
   * @brief Reinitialize the Buffer to specified length
   * @param i_newNumBits Length of new buffer in bits
   * @post Buffer is reinitialized
   *
   * CAUTION : All data stored in buffer will be lost
   */
  void  setBitLength(int i_newNumBits);
  /**
   * @brief Reinitialize the internal buffer to specified length
   * @param i_newNumWords length of internal data buffer in words
   * @post Internal buffer is reinitialzied
   *
   * CAUTION : All data stored in buffer will be lost
   */
  void setCapacity (int i_newNumWords);

  //@}



  /** @name Bit/Word Manipulation Functions */
  //@{
  /**
   * @brief Turn on a bit in buffer
   * @param i_bit Bit in buffer to turn on
   */
  void  setBit(int i_bit);
  /**
   * @brief Turn on a bit in buffer
   * @param i_bit start bit in buffer to turn on
   * @param i_len Number of consecutive bits from start bit to turn on
   */
  void  setBit(int i_bit, int i_len);

  /**
   * @brief Set a word of data in buffer
   * @param i_wordoffset Offset of word to set
   * @param i_value 32 bits of data to put into word
   */
  void  setWord(int i_wordoffset, uint32_t i_value);

  /**
   * @brief Fetch a word from ecmdDataBuffer
   * @param i_wordoffset Offset of word to fetch
   * @retval Value of word requested
   */
  uint32_t getWord(int i_wordoffset);

  /**
   * @brief Set a byte of data in buffer
   * @param i_byteoffset Offset of byte to set
   * @param i_value 8 bits of data to put into byte
   */
  void  setByte(int i_byteoffset, uint8_t i_value);

  /**
   * @brief Fetch a byte from ecmdDataBuffer
   * @param i_byteoffset Offset of byte to fetch
   * @retval Value of byte requested
   */
  uint8_t getByte(int i_byteoffset);

  /**
   * @brief Clear a bit in buffer
   * @param i_bit Bit in buffer to turn off
   */
  void  clearBit(int i_bit);
  /**
   * @brief Clear multiple bits in buffer
   * @param i_bit Start bit in buffer to turn off
   * @param i_len Number of consecutive bits from start bit to off
   */
  void  clearBit(int i_bit, int i_len);

  /**
   * @brief Invert bit
   * @param i_bit Bit in buffer to invert
   */
  void  flipBit(int i_bit);
  /**
   * @brief Invert multiple bits
   * @param i_bit Start bit in buffer to invert
   * @param i_len Number of consecutive bits to invert
   */
  void  flipBit(int i_bit, int i_len);

  /**
   * @brief Test if bit is set
   * @param i_bit Bit to test
   * @retval true if bit is set - false if bit is clear
   */
  int   isBitSet(int i_bit);
  /**
   * @brief Test if multiple bits are set
   * @param i_bit Start bit to test
   * @param i_len Number of consecutive bits to test
   * @retval true if bit is set - false if bit is clear
   */
  int   isBitSet(int i_bit, int i_len);
  /**
   * @brief Test if bit is clear
   * @param i_bit Bit to test
   * @retval true if bit is clear - false if bit is set
   */
  int   isBitClear(int i_bit);
  /**
   * @brief Test if multiple bits are clear
   * @param i_bit Start bit to test
   * @param i_len Number of consecutive bits to test
   * @retval true if bit is clear - false if bit is set
   */
  int   isBitClear(int i_bit, int i_len);
  /**
   * @brief Count number of bits set in a range
   * @param i_bit Start bit to test
   * @param i_len Number of consecutive bits to test
   * @retval Number of bits set in range
   */      
  int   getNumBitsSet(int i_bit, int i_len);
  //@}


  /** @name Buffer Manipulation Functions */
  //@{
  /**
   * @brief Shift data to right
   * @param i_shiftnum Number of bits to shift
   * @post Bits in buffer are shifted to right by specified number of bits - data is shifted off the end
   * @post Buffer size is unchanged
   */
  void  shiftRight(int i_shiftnum);
  /**
   * @brief Shift data to left
   * @param i_shiftnum Number of bits to shift
   * @post Bits in buffer are shifted to left by specified number of bits - data is shifted off the beginning
   * @post Buffer size is unchanged
   */
  void  shiftLeft(int i_shiftnum);
  /**
   * @brief Shift data to right - resizing buffer
   * @param i_shiftnum Number of bits to shift
   * @post Bits in buffer are shifted to right by specified number of bits
   * @post Buffer size is resized to accomodate shift
   */
  void  shiftRightAndResize(int i_shiftnum);
  /**
   * @brief Shift data to left - resizing buffer
   * @param i_shiftnum Number of bits to shift
   * @post Bits in buffer are shifted to left by specified number of bits - data is shifted off the beginning
   * @post Buffer size is resized to accomodate shift
   */
  void  shiftLeftAndResize(int i_shiftnum);
  /**
   * @brief Rotate data to right
   * @param i_rotatenum Number of bits to rotate
   * @post Bits in buffer are rotated to the right by specified number of bits - data is rotated to the beginning
   */
  void  rotateRight(int i_rotatenum);
  /**
   * @brief Rotate data to left
   * @param i_rotatenum Number of bits to rotate
   * @post Bits in buffer are rotated to the left by specified number of bits - data is rotated to the end
   */
  void  rotateLeft(int i_rotatenum);

  /**
   * @brief Clear entire buffer to 0's
   */
  void  flushTo0();
  /**
   * @brief Set entire buffer to 1's
   */
  void  flushTo1();
  /**
   * @brief Invert entire buffer
   */
  void  invert();  /* Performs bit inversion on entire DataBuffer class */

  /**
   * @brief Apply an inversion mask to data inside buffer
   * @param i_invMask Buffer that stores inversion mask
   * @param i_invByteLen Buffer length provided in bytes
   */
  void applyInversionMask(uint32_t * i_invMask, int i_invByteLen);


  /**
   * @brief Insert part of another DataBuffer into this one
   * @param i_bufferIn DataBuffer to copy data from - data is taken left aligned
   * @param i_start Start bit to insert to
   * @param i_len Length of bits to insert
   * @post Data is copied from bufferIn to this DataBuffer in specified location
   */
  void  insert(ecmdDataBuffer & i_bufferIn, int i_start, int i_len);
  /**
   * @brief Insert a uint32_t array into this DataBuffer
   * @param i_datain uint32_t array to copy into this DataBuffer - data is taken left aligned
   * @param i_start Start bit to insert into
   * @param i_len Length of bits to insert
   * @post Data is copied from datain into this DataBuffer at specified location
   */
  void  insert(uint32_t * i_datain, int i_start, int i_len);
  /**
   * @brief Insert a uint32_t into the DataBuffer
   * @param i_datain uint32_t value to copy into DataBuffer - data is taken left aligned
   * @param i_start Start bit to insert into
   * @param i_len Length of bits to insert (must be <= 32)
   * @post Data is copied from datain into this DataBuffer at specified location
   */
  void  insert(uint32_t i_datain, int i_start, int i_len);
  /**
   * @brief Copy data from this DataBuffer into another
   * @param o_bufferOut DataBuffer to copy into - data is placed left aligned
   * @param i_start Start bit of data in this DataBuffer to copy
   * @param i_len Length of consecutive bits to copy
   * @post Data is copied from specified location in this DataBuffer to bufferOut
   */
  void  extract(ecmdDataBuffer & o_bufferOut, int i_start, int i_len);
  /**
   * @brief Copy data from this DataBuffer into another
   * @param o_data uint32_t buffer to copy into - data is placed left aligned - must be pre-allocated
   * @param i_start Start bit of data in DataBuffer to copy
   * @param i_len Length of consecutive bits to copy
   * @post Data is copied from specified location in this DataBuffer to o_data
   */
  void  extract(uint32_t * o_data, int i_start, int i_len);

  /* these functions OR the datain into the DataBuffer buffer */
  /**
   * @brief OR data into DataBuffer
   * @param i_bufferIn DataBuffer to OR data from - data is taken left aligned
   * @param i_startbit Start bit to OR to
   * @param i_len Length of bits to OR
   * @post Data is ORed from i_bufferIn to this DataBuffer in specified location
   */
  void setOr(ecmdDataBuffer & i_bufferIn, int i_startbit, int i_len);
  /**
   * @brief OR data into DataBuffer
   * @param i_datain uint32_t buffer to OR data from - data is taken left aligned
   * @param i_startbit Start bit to OR to
   * @param i_len Length of bits to OR
   * @post Data is ORed from datain to this DataBuffer in specified location
   */
  void setOr(uint32_t * i_datain, int i_startbit, int i_len);
  /**
   * @brief OR data into DataBuffer
   * @param i_datain uint32_t to OR data from - data is taken left aligned
   * @param i_startbit Start bit to OR to
   * @param i_len Length of bits to OR (must be <= 32)
   * @post Data is ORed from datain to this DataBuffer in specified location
   */
  void setOr(uint32_t i_datain, int i_startbit, int i_len);
  /**
   * @brief OR data into DataBuffer
   * @param i_bufferIn DataBuffer to OR data from - data is taken left aligned
   * @post Entire data is ORed from bufferIn to this DataBuffer
   */
  void merge(ecmdDataBuffer & i_bufferIn); // does a setor on the whole buffer

  /* these functions AND the datain into the DataBuffer buffer */
  /**
   * @brief AND data into DataBuffer
   * @param i_bufferIn Bitvector to AND data from - data is taken left aligned
   * @param i_startbit Start bit to AND to
   * @param i_len Length of bits to AND
   * @post Data is ANDed from bufferIn to this DataBuffer in specified location
   */
  void setAnd(ecmdDataBuffer & i_bufferIn, int i_startbit, int i_len);
  /**
   * @brief AND data into DataBuffer
   * @param i_datain uint32_t buffer to AND data from - data is taken left aligned
   * @param i_startbit Start bit to AND to
   * @param i_len Length of bits to AND
   * @post Data is ANDed from datain to this DataBuffer in specified location
   */
  void setAnd(uint32_t * i_datain, int i_startbit, int i_len);
  /**
   * @brief AND data into DataBuffer
   * @param i_datain uint32_t to AND data from - data is taken left aligned
   * @param i_startbit Start bit to AND to
   * @param i_len Length of bits to AND (must be <= 32)
   * @post Data is ANDed from datain to this DataBuffer in specified location
   */
  void setAnd(uint32_t i_datain, int i_startbit, int i_len);

  /**
   * @brief Copy entire contents of this ecmdDataBuffer into o_copyBuffer 
   * @param o_copyBuffer DataBuffer to copy data into
   * @post copyBuffer is an exact duplicate of this DataBuffer
   */
  void  copy(ecmdDataBuffer & o_copyBuffer); 

  /**
   * @brief Copy Constructor
   * @param i_master DataBuffer to copy from
   */
  int operator=(ecmdDataBuffer & i_master);

  /* These are only to be used to apply a buffer to the entire ecmdDataBuffer, not just sections */
  /**
   * @brief Copy buffer into this ecmdDataBuffer
   * @param i_buf Buffer to copy from
   * @param i_bytes Byte length to copy
   * @post  Xstate and Raw buffer are set to value in i_buf for smaller of i_bytes or buffer capacity
   */
  void  memCopyIn(uint32_t * i_buf, int i_bytes); /* Does a memcpy from supplied buffer into ecmdDataBuffer */
  /**
   * @brief Copy DataBuffer into supplied uint32_t buffer
   * @param o_buf Buffer to copy into - must be pre-allocated
   * @param i_bytes Byte length to copy
   * @post o_buf has contents of databuffer for smaller of i_bytes or buffer capacity
   */
  void  memCopyOut(uint32_t * o_buf, int i_bytes); /* Does a memcpy from ecmdDataBuffer into supplied buffer */

  //@}


  /** @name Parity Functions */
  //@{
  /**
   * @brief Generate odd parity over a range of bits
   * @param i_start Start bit of range
   * @param i_stop Stop bit of range
   * @retval 0 or 1 depending on parity of range
   */
  int  oddParity(int i_start, int i_stop);
  /**
   * @brief Generate even parity over a range of bits
   * @param i_start Start bit of range
   * @param i_stop Stop bit of range
   * @retval 0 or 1 depending on parity of range
   */
  int  evenParity(int i_start, int i_stop);
  /**
   * @brief Generate odd parity over a range of bits and insert into DataBuffer
   * @param i_start Start bit of range
   * @param i_stop Stop bit of range
   * @param i_insertpos Bit position to insert parity
   * @retval 0 on success - nonzero on failure
   */
  int  oddParity(int i_start, int i_stop, int i_insertpos); 
  /**
   * @brief Generate even parity over a range of bits and insert into DataBuffer
   * @param i_start Start bit of range
   * @param i_stop Stop bit of range
   * @param i_insertpos Bit position to insert parity
   * @retval 0 on success - nonzero on failure
   */
  int  evenParity(int i_start, int i_stop, int i_insertpos); 
  //@}


  /** @name Buffer Character Conversion Functions */
  //@{
  /**
   * @brief Return Data as a hex left aligned char string
   * @param i_start Start bit of data to convert
   * @param i_bitlen Number of consecutive bits to convert
   * @retval String containing requested data
   */
  std::string genHexLeftStr(int i_start, int i_bitlen);
  /**
   * @brief Return Data as a hex right aligned char string
   * @param i_start Start bit of data to convert
   * @param i_bitlen Number of consecutive bits to convert
   * @retval String containing requested data
   */
  std::string genHexRightStr(int i_start, int i_bitlen); 
  /**
   * @brief Return Data as a binary char string
   * @param i_start Start bit of data to convert
   * @param i_bitlen Number of consecutive bits to convert
   * @retval String containing requested data
   */
  std::string genBinStr(int i_start, int i_bitlen); 

  /**
   * @brief Return entire buffer as a hex left aligned char string
   * @retval String containing requested data
   */
  std::string genHexLeftStr();
  /**
   * @brief Return entire buffer as a hex right aligned char string
   * @retval String containing requested data
   */
  std::string genHexRightStr(); 
  /**
   * @brief Return entire buffer as a binary char string
   * @retval String containing requested data
   */
  std::string genBinStr();

  /**
   * @brief Retrieve a section of the Xstate Data
   * @param i_start Start bit of data to retrieve
   * @param i_bitlen Number of consecutive bits to retrieve
   * @retval String containing requested data
   */
  std::string genXstateStr(int i_start, int i_bitlen);

  /**
   * @brief Retrieve entire Xstate Data buffer
   * @retval String containing requested data
   */
  std::string genXstateStr();


  //@}


  /** @name String to Data conversion functions*/
  //@{

  /**
   * @brief Convert data from a hex left-aligned string and insert it into this data buffer
   * @param i_hexChars Hex Left-aligned string of data to insert 
   * @param i_start Starting position in data buffer to insert to, 0 by default
   * @param i_length Length of data to insert, defaults to length of i_hexChars, zeroes are padded or data dropped from right if necessary
   * @retval ECMD_DBUF_INVALID_DATA_FORMAT if non-hex chars detected in i_hexChars
   * @retval ECMD_SUCCESS on success
   * @retval non-zero on failure
   */
  int insertFromHexLeft (const char * i_hexChars, int i_start = 0, int i_length = 0);

  /**
   * @brief Convert data from a hex right-aligned string and insert it into this data buffer
   * @param i_hexChars Hex Right-aligned string of data to insert
   * @param i_expectedLength The expected length of the string data, zeros are padded or data dropped from the left if necessary
   * @param i_start Starting position in data buffer to insert to, 0 by default
   * @retval ECMD_DBUF_INVALID_DATA_FORMAT if non-hex chars detected in i_hexChars
   * @retval ECMD_SUCCESS on success
   * @retval non-zero on failure
   */
  int insertFromHexRight (const char * i_hexChars, int i_start = 0, int i_expectedLength = 0);

  /**
   * @brief Convert data from a binary string and insert it into this data buffer
   * @retval 0 on success- non-zero on failure
   * @param i_binChars String of 0's and 1's to insert 
   * @param i_start Starting position in data buffer to insert to, 0 by default
   * @retval ECMD_DBUF_INVALID_DATA_FORMAT if non-binary chars detected in i_binChars
   * @retval ECMD_SUCCESS on success
   * @retval non-zero on failure

   */
  int insertFromBin (const char * i_binChars, int i_start = 0);

  //@}


  /** @name Simulation Buffer Functions */
  //@{

 
  /**
   * @brief Check Entire buffer for any X-state values
   * @retval 1 if xstate found 0 if none
   */
  int   hasXstate(); /* check the whole DataBuffer */
  /**
   * @brief Check section of buffer for any X-state values
   * @param i_start Start bit to test
   * @param i_length Number of consecutive bits to test
   * @retval 1 if xstate found 0 if none
   */
  int   hasXstate(int i_start, int i_length); /* check subset */

  /**
   * @brief Retrieve an Xstate value from the buffer
   * @param i_bit Bit to retrieve

   * NOTE - To retrieve multiple bits use genXstateStr
   */
  char  getXstate(int i_bit);

  /**
   * @brief Set an Xstate value in the buffer
   * @param i_bit Bit to set
   * @param i_value Xstate value to set
   */
  void setXstate(int i_bit, char i_value);

  /**
   * @brief Set a range of Xstate values in buffer
   * @param i_bitoffset bit in buffer to start inserting
   * @param i_datastr Character value to set bit - can be "0XX0", "1", "X"
   */
  void  setXstate(int i_bitoffset, const char* i_datastr); 

  /**
   * @brief Copy buffer into the Xstate data of this ecmdDataBuffer
   * @param i_buf Buffer to copy from
   * @param i_bytes Byte length to copy (char length)
   * @post  Xstate and Raw buffer are set to value in i_buf for smaller of i_bytes or buffer capacity
   */
  void  memCopyInXstate(char * i_buf, int i_bytes); /* Does a memcpy from supplied buffer into ecmdDataBuffer */
  /**
   * @brief Copy DataBuffer into supplied char buffer from Xstate data
   * @param o_buf Buffer to copy into - must be pre-allocated
   * @param i_bytes Byte length to copy (char length)
   * @post o_buf has contents of databuffer for smaller of i_bytes or buffer capacity
   */
  void  memCopyOutXstate(char * o_buf, int i_bytes); /* Does a memcpy from ecmdDataBuffer into supplied buffer */


  //@}

private:  //data
  int iv_Capacity;              ///< Actual buffer capacity - always >= iv_NumWords
  int iv_NumWords;              ///< Specified buffer size rounded to next word
  int iv_NumBits;               ///< Specified buffer size in bits
  uint32_t * iv_Data;           ///< Pointer to buffer inside iv_RealData
  uint32_t * iv_RealData;       ///< Real buffer - with header and tail


private: //methods


private:
#ifndef REMOVE_SIM
  char* iv_DataStr; /* binary or x data */
  void fillDataStr(char fillChar);
#endif

};


#endif  /* ecmdDataBuffer_H */

    
