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
#ifdef ENABLE_MPATROL
 #include <mpatrol.h>
#endif
#ifdef FIPSODE
#include <tracinterface.H>
#endif

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------
#ifdef FIPSODE
extern tracDesc_t g_etrc; /** Trace Descriptor **/
#endif

//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------
/* Define these if for some reason we are building without ecmdReturnCodes.H */
#ifndef ECMD_DBUF_SUCCESS
 /* dataBuffer specific return codes */
 #define ECMD_DBUF_SUCCESS                       0x0    ///< DataBuffer returned successfully
 #define ECMD_DBUF_INIT_FAIL                     (0x01000000 | 0x2000) ///< Initialization of the DataBuffer failed
 #define ECMD_DBUF_BUFFER_OVERFLOW               (0x01000000 | 0x2010) ///< Attempt to read/write data beyond the length of the DataBuffer
 #define ECMD_DBUF_XSTATE_ERROR                  (0x01000000 | 0x2020) ///< An 'X' character occured where it was not expected
 #define ECMD_DBUF_UNDEFINED_FUNCTION            (0x01000000 | 0x2030) ///< Function not included in this version of DataBuffer
 #define ECMD_DBUF_INVALID_ARGS                  (0x01000000 | 0x2040) ///< Args provided to dataBuffer were invalid
 #define ECMD_DBUF_INVALID_DATA_FORMAT           (0x01000000 | 0x2041) ///< String data didn't match expected input format
 #define ECMD_DBUF_FOPEN_FAIL                    (0x01000000 | 0x2050) ///< File open on file for reading or writing the data buffer failed 
 #define ECMD_DBUF_FILE_FORMAT_MISMATCH          (0x01000000 | 0x2051) ///< In readFile specified format not found in the data file
 #define ECMD_DBUF_NOT_OWNER                     (0x01000000 | 0x2060) ///< Don't own this buffer so can't do this operation
#endif

//--------------------------------------------------------------------
// Macros
//--------------------------------------------------------------------
#ifdef FIPSODE
#define ETRAC0(fmt) \
  TRACFCOMP0(g_etrc, fmt);
#define ETRAC1(fmt, arg1) \
  TRACFCOMP1(g_etrc, fmt, arg1)
#define ETRAC2(fmt, arg1, arg2) \
  TRACFCOMP2(g_etrc, fmt, arg1, arg2)
#define ETRAC3(fmt, arg1, arg2, arg3) \
  TRACFCOMP3(g_etrc, fmt, arg1, arg2, arg3)
#define ETRAC4(fmt, arg1, arg2, arg3, arg4) \
  TRACFCOMP4(g_etrc, fmt, arg1, arg2, arg3, arg4)
#define ETRAC5(fmt, arg1, arg2, arg3, arg4, arg5) \
  TRACFCOMP5(g_etrc, fmt, arg1, arg2, arg3, arg4, arg5)
#define ETRAC6(fmt, arg1, arg2, arg3, arg4, arg5, arg6) \
  TRACFCOMP6(g_etrc, fmt, arg1, arg2, arg3, arg4, arg5, arg6)
#define ETRAC7(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
  TRACFCOMP7(g_etrc, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
  arg7)
#define ETRAC8(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
  TRACFCOMP8(g_etrc, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
  arg7, arg8)
#define ETRAC9(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
  TRACFCOMP9(g_etrc, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
  arg7, arg8, arg9)
#else
#define ETRAC0(fmt) printf( "%s> ETRC: " fmt "\n", __FUNCTION__);
#define ETRAC1(fmt, arg1) printf( "%s> ETRC: " fmt "\n", __FUNCTION__, arg1);
#define ETRAC2(fmt, arg1, arg2) printf( "%s> ETRC: " fmt "\n", __FUNCTION__, arg1, arg2);
#define ETRAC3(fmt, arg1, arg2, arg3) printf( "%s> ETRC: " fmt "\n", __FUNCTION__, arg1, arg2, arg3);
#define ETRAC4(fmt, arg1, arg2, arg3, arg4) printf( "%s> ETRC: " fmt "\n", __FUNCTION__, arg1, arg2, arg3, arg4);
#define ETRAC5(fmt, arg1, arg2, arg3, arg4, arg5) printf( "%s> ETRC: " fmt "\n", __FUNCTION__, arg1, arg2, arg3, arg4, arg5);
#define ETRAC6(fmt, arg1, arg2, arg3, arg4, arg5, arg6) printf( "%s> ETRC: " fmt "\n", __FUNCTION__, arg1, arg2, arg3, arg4, arg5, arg6);
#define ETRAC7(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7) printf( "%s> ETRC: " fmt "\n", __FUNCTION__, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
#define ETRAC8(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) printf( "%s> ETRC: " fmt "\n", __FUNCTION__, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
#define ETRAC9(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) printf( "%s> ETRC: " fmt "\n", __FUNCTION__, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
#endif

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------

//--------------------------------------------------------------------
//  Defines                                                
//--------------------------------------------------------------------
/**
 @brief This is the different formats in which the output file will be written
*/

typedef enum {
  ECMD_SAVE_FORMAT_BINARY,      ///< binary file with header with info like bit length, format etc
  ECMD_SAVE_FORMAT_BINARY_DATA, ///< binary file with data only - will NOT work with scan ring data as length is rounded up to next byte
  ECMD_SAVE_FORMAT_ASCII,       ///< ascii text file with header having same info like binary hdr
  ECMD_SAVE_FORMAT_XSTATE,      ///< xstate text file with header having same info like binary hdr

} ecmdFormatType_t;

/**
 @brief This is the different write modes for writing databuffer into a file
*/

typedef enum {
  ECMD_WRITE_MODE,      ///< Overrwrite the data if the file already exists
  ECMD_APPEND_MODE,     ///< Add databuffer to the end of the file
} ecmdWriteMode_t;
//----------------------------------------------------------------------
//  User Types
//----------------------------------------------------------------------
/**
 @brief This is used to help low-level implementation of the ecmdDataBuffer, this CAN NOT be used by any eCMD client or data corruption will occur
*/
class ecmdDataBufferImplementationHelper {
public:
  static uint32_t* getDataPtr( void* i_buffer );
  static void applyRawBufferToXstate( void* i_buffer );

};
  


/**
 @brief Provides a means to handle data from the eCMD C API
*/

class ecmdDataBuffer {

  friend class ecmdDataBufferImplementationHelper;

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
   * @param i_numBits Size of data in bits to initialize
   * @post ecmdDataBuffer is initialized and zero'd out
   */
  ecmdDataBuffer(uint32_t i_numBits); 


  /**
   * @brief Copy Constructor
   * @param other Buffer to copy
   */
  ecmdDataBuffer(const ecmdDataBuffer &other);

  /**
   * @brief Default Destructor
   */
  virtual ~ecmdDataBuffer();
  //@}


  // Member Functions
  /** @name Buffer Size Functions */
  //@{
  /**
   * @brief Called by the destructor, available to user to reset buffer to default constructor state
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_NOT_OWNER when called on buffer not owned
   * @retval nonzero on failure
   * @post Memory deallocated and size set to 0
   */
  uint32_t clear();
  /**
   * @brief Return the length of the buffer in words
   * @retval Buffer length in words rounded up
   */
  uint32_t   getWordLength() const;
  /**
   * @brief Return the length of the buffer in bytes
   * @retval Buffer length in bytes rounded up
   */
  uint32_t   getByteLength() const;
  /**
   * @brief Return the length of the buffer in bits
   * @retval Buffer length in bits
   */
  uint32_t   getBitLength() const;
  /**
   * @brief Return the actual capacity of the internal buffer in words
   * @retval Actual capacity in words of internal buffer
   */
  uint32_t   getCapacity() const;
  /**
   * @brief Reinitialize the Buffer to specified length
   * @param i_newNumWords Length of new buffer in words
   * @post Buffer is reinitialized and zero'd out
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_INIT_FAIL failure occurred setting new length
   * @retval ECMD_DBUF_NOT_OWNER when called on buffer not owned
   * 
   * NOTE : Capacity will be adjusted to fit new size if neccesary
   * CAUTION : All data stored in buffer will be lost
   */
  uint32_t  setWordLength(uint32_t i_newNumWords);   
  /**
   * @brief Reinitialize the Buffer to specified length
   * @param i_newNumBytes Length of new buffer in bytes
   * @post Buffer is reinitialized and zero'd out
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_INIT_FAIL failure occurred setting new length
   * @retval ECMD_DBUF_NOT_OWNER when called on buffer not owned
   * 
   * NOTE : Capacity will be adjusted to fit new size if neccesary
   * CAUTION : All data stored in buffer will be lost
   */
  uint32_t  setByteLength(uint32_t i_newNumBytes);   
  /**
   * @brief Reinitialize the Buffer to specified length
   * @param i_newNumBits Length of new buffer in bits
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_INIT_FAIL failure occurred setting new length
   * @retval ECMD_DBUF_NOT_OWNER when called on buffer not owned
   * @post Buffer is reinitialized and zero'd out
   *
   * NOTE : Capacity will be adjusted to fit new size if neccesary
   * CAUTION : All data stored in buffer will be lost
   */
  uint32_t  setBitLength(uint32_t i_newNumBits);
  /**
   * @brief Reinitialize the internal buffer to specified length
   * @param i_newNumWords length of internal data buffer in words
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_INIT_FAIL failure occurred setting new length
   * @retval ECMD_DBUF_NOT_OWNER when called on buffer not owned
   * @post Internal buffer is reinitialized and zero'd out.  Requests to decrease the capacity are ignored
   *
   * CAUTION : All data stored in buffer will be lost
   */
  uint32_t setCapacity (uint32_t i_newNumWords);


  /**
   * @brief Shrink buffer size to a new bit size
   * @param i_newNumBits New bit length for buffer (must be <= current buffer length)
   * @retval ECMD_DBUF_SUCCESS on success
   * @post Internal buffer size is reset but data inside new size is not lost
   */
  uint32_t shrinkBitLength(uint32_t i_newNumBits);

  /**
   * @brief Expand buffer size to a new bit size maintaining current data
   * @param i_newNumBits New bit length for buffer
   * @retval ECMD_DBUF_SUCCESS on success
   * @post Internal buffer size is reset but data inside is not lost
   *
   * NOTE : Capacity will be adjusted to fit new size if neccesary
   */
  uint32_t growBitLength(uint32_t i_newNumBits);
  //@}



  /** @name Bit/Word Manipulation Functions */
  //@{
  /**
   * @brief Turn on a bit in buffer
   * @param i_bit Bit in buffer to turn on
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW i_bit is not contained in the size of this buffer
   */
  uint32_t  setBit(uint32_t i_bit);
  /**
   * @brief Turn on a bit in buffer
   * @param i_bit start bit in buffer to turn on
   * @param i_len Number of consecutive bits from start bit to turn on
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW i_bit is not contained in the size of this buffer
   */
  uint32_t  setBit(uint32_t i_bit, uint32_t i_len);

  /**
   * @brief Write a bit to specified value in buffer
   * @param i_bit Bit in buffer to turn on
   * @param i_value Value to write
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW i_bit is not contained in the size of this buffer
   */
  uint32_t writeBit(uint32_t i_bit, uint32_t i_value);

  /**
   * @brief Set a word of data in buffer
   * @param i_wordoffset Offset of word to set
   * @param i_value 32 bits of data to put into word
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW i_wordoffset is not contained in the size of this buffer
   */
  uint32_t  setWord(uint32_t i_wordoffset, uint32_t i_value);

  /**
   * @brief Fetch a word from ecmdDataBuffer
   * @param i_wordoffset Offset of word to fetch
   * @retval Value of word requested
   */
  uint32_t getWord(uint32_t i_wordoffset) const;

  /**
   * @brief Set a byte of data in buffer
   * @param i_byteoffset Offset of byte to set
   * @param i_value 8 bits of data to put into byte
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW i_byteoffset is not contained in the size of this buffer
   */
  uint32_t  setByte(uint32_t i_byteoffset, uint8_t i_value);

  /**
   * @brief Fetch a byte from ecmdDataBuffer
   * @param i_byteoffset Offset of byte to fetch
   * @retval Value of byte requested
   *
   * NOTE : If offset > buffer length retval = 0 and error printed
   */
  uint8_t getByte(uint32_t i_byteoffset) const;

  /**
   * @brief Clear a bit in buffer
   * @param i_bit Bit in buffer to turn off
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW i_bit is not contained in the size of this buffer
   */
  uint32_t  clearBit(uint32_t i_bit);
  /**
   * @brief Clear multiple bits in buffer
   * @param i_bit Start bit in buffer to turn off
   * @param i_len Number of consecutive bits from start bit to off
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW i_bit is not contained in the size of this buffer
   */
  uint32_t  clearBit(uint32_t i_bit, uint32_t i_len);

  /**
   * @brief Invert bit
   * @param i_bit Bit in buffer to invert
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW i_bit is not contained in the size of this buffer
   */
  uint32_t  flipBit(uint32_t i_bit);
  /**
   * @brief Invert multiple bits
   * @param i_bit Start bit in buffer to invert
   * @param i_len Number of consecutive bits to invert
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW i_bit is not contained in the size of this buffer
   */
  uint32_t  flipBit(uint32_t i_bit, uint32_t i_len);

  /**
   * @brief Test if bit is set
   * @param i_bit Bit to test
   * @retval true if bit is set - false if bit is clear
   */
  bool   isBitSet(uint32_t i_bit) const;
  /**
   * @brief Test if multiple bits are set
   * @param i_bit Start bit to test
   * @param i_len Number of consecutive bits to test
   * @retval true if all bits in range are set - false if any bit is clear
   */
  bool   isBitSet(uint32_t i_bit, uint32_t i_len) const;
  /**
   * @brief Test if bit is clear
   * @param i_bit Bit to test
   * @retval true if bit is clear - false if bit is set
   */
  bool   isBitClear(uint32_t i_bit) const;
  /**
   * @brief Test if multiple bits are clear
   * @param i_bit Start bit to test
   * @param i_len Number of consecutive bits to test
   * @retval true if all bits in range are clear - false if any bit is set
   */
  bool   isBitClear(uint32_t i_bit, uint32_t i_len) const;
  /**
   * @brief Count number of bits set in a range
   * @param i_bit Start bit to test
   * @param i_len Number of consecutive bits to test
   * @retval Number of bits set in range
   */      
  uint32_t   getNumBitsSet(uint32_t i_bit, uint32_t i_len) const;
  //@}


  /** @name Buffer Manipulation Functions */
  //@{
  /**
   * @brief Shift data to right
   * @param i_shiftnum Number of bits to shift
   * @post Bits in buffer are shifted to right by specified number of bits - data is shifted off the end
   * @post Buffer size is unchanged
   * @retval ECMD_DBUF_SUCCESS on success
   */
  uint32_t  shiftRight(uint32_t i_shiftnum);
  /**
   * @brief Shift data to left
   * @param i_shiftnum Number of bits to shift
   * @post Bits in buffer are shifted to left by specified number of bits - data is shifted off the beginning
   * @post Buffer size is unchanged
   * @retval ECMD_DBUF_SUCCESS on success
   */
  uint32_t  shiftLeft(uint32_t i_shiftnum);
  /**
   * @brief Shift data to right - resizing buffer
   * @param i_shiftnum Number of bits to shift
   * @post Bits in buffer are shifted to right by specified number of bits
   * @post Buffer size is resized to accomodate shift
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_NOT_OWNER when called on buffer not owned
   */
  uint32_t  shiftRightAndResize(uint32_t i_shiftnum);
  /**
   * @brief Shift data to left - resizing buffer
   * @param i_shiftnum Number of bits to shift
   * @post Bits in buffer are shifted to left by specified number of bits - data is shifted off the beginning
   * @post Buffer size is resized to accomodate shift
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_NOT_OWNER when called on buffer not owned
   */
  uint32_t  shiftLeftAndResize(uint32_t i_shiftnum);
  /**
   * @brief Rotate data to right
   * @param i_rotatenum Number of bits to rotate
   * @post Bits in buffer are rotated to the right by specified number of bits - data is rotated to the beginning
   * @retval ECMD_DBUF_SUCCESS on success
   */
  uint32_t  rotateRight(uint32_t i_rotatenum);
  /**
   * @brief Rotate data to left
   * @param i_rotatenum Number of bits to rotate
   * @post Bits in buffer are rotated to the left by specified number of bits - data is rotated to the end
   * @retval ECMD_DBUF_SUCCESS on success
   */
  uint32_t  rotateLeft(uint32_t i_rotatenum);

  /**
   * @brief Clear entire buffer to 0's
   * @retval ECMD_DBUF_SUCCESS on success
   */
  uint32_t  flushTo0();
  /**
   * @brief Set entire buffer to 1's
   * @retval ECMD_DBUF_SUCCESS on success
   */
  uint32_t  flushTo1();
  /**
   * @brief Invert entire buffer
   * @retval ECMD_DBUF_SUCCESS on success
   */
  uint32_t  invert();

  /**
   * @brief Bit reverse entire buffer
   * @retval ECMD_DBUF_SUCCESS on success
   */
  uint32_t reverse();

  /**
   * @brief Apply an inversion mask to data inside buffer
   * @param i_invMask Buffer that stores inversion mask
   * @param i_invByteLen Buffer length provided in bytes
   * @retval ECMD_DBUF_SUCCESS on success
   */
  uint32_t applyInversionMask(const uint32_t * i_invMask, uint32_t i_invByteLen);

  /**
   * @brief Apply an inversion mask to data inside buffer
   * @brief Just a wrapper that takes in a ecmdDataBuffer and calls uint32_t applyInversionMask
   * @param i_invMaskBuffer Buffer that stores inversion mask
   * @param i_invByteLen Buffer length provided in bytes
   * @retval ECMD_DBUF_SUCCESS on success
   */
  uint32_t applyInversionMask(const ecmdDataBuffer & i_invMaskBuffer, uint32_t i_invByteLen);

  /**
   * @brief Copy part of another DataBuffer into this one
   * @param i_bufferIn DataBuffer to copy data from - data is taken left aligned
   * @param i_targetStart Start bit to insert to
   * @param i_len Length of bits to insert
   * @param i_sourceStart Start bit in i_bufferIn - default value is zero
   * @pre DataBuffer must be pre-allocated
   * @post Data is copied from i_bufferIn to this DataBuffer in specified location
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t  insert(const ecmdDataBuffer & i_bufferIn, uint32_t i_targetStart, uint32_t i_len, uint32_t i_sourceStart = 0);
  /**
   * @brief Copy part of a uint32_t array into this DataBuffer
   * @param i_datain uint32_t array to copy into this DataBuffer - data is taken left aligned
   * @param i_targetStart Start bit to insert into
   * @param i_len Length of bits to insert
   * @param i_sourceStart Start bit in i_datain - default value is zero
   * @pre DataBuffer must be pre-allocated
   * @post Data is copied from i_datain to this DataBuffer in specified location
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t  insert(const uint32_t * i_datain, uint32_t i_targetStart, uint32_t i_len, uint32_t i_sourceStart = 0);
  /**
   * @brief Copy part of a uint32_t into the DataBuffer
   * @param i_datain uint32_t value to copy into DataBuffer - data is taken left aligned
   * @param i_targetStart Start bit to insert into
   * @param i_len Length of bits to insert (must be <= 32)
   * @param i_sourceStart Start bit in i_datain - default value is zero
   * @pre DataBuffer must be pre-allocated
   * @post Data is copied from bufferIn to this DataBuffer in specified location
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t  insert(uint32_t i_datain, uint32_t i_targetStart, uint32_t i_len, uint32_t i_sourceStart = 0);
  /**
   * @brief Copy a right aligned (decimal) uint32_t array into this DataBuffer
   * @param i_datain uint32_t array to copy into this DataBuffer - data is taken right aligned
   * @param i_start Start bit to insert into
   * @param i_len Length of bits to insert
   * @pre DataBuffer must be pre-allocated
   * @post Data is copied from datain into this DataBuffer at specified location
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   *
   * NOTE : Data is assumed to be aligned on the word boundary of i_len
   */
  uint32_t  insertFromRight(const uint32_t * i_datain, uint32_t i_start, uint32_t i_len);
  /**
   * @brief Copy a right aligned (decimal) uint32_t into the DataBuffer
   * @param i_datain uint32_t value to copy into DataBuffer - data is taken right aligned
   * @param i_start Start bit to insert into
   * @param i_len Length of bits to insert (must be <= 32)
   * @pre DataBuffer must be pre-allocated
   * @post Data is copied from datain into this DataBuffer at specified location
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t  insertFromRight(uint32_t i_datain, uint32_t i_start, uint32_t i_len);


  /**
   * @brief Copy data from this DataBuffer into another
   * @param o_bufferOut DataBuffer to copy into - data is placed left aligned
   * @param i_start Start bit of data in this DataBuffer to copy
   * @param i_len Length of consecutive bits to copy
   * @post Data is copied from specified location in this DataBuffer to bufferOut
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range

   * NOTE : The o_bufferOut buffer is resized to the extract length and any data in the buffer is lost

   */
  uint32_t  extract(ecmdDataBuffer & o_bufferOut, uint32_t i_start, uint32_t i_len) const;
  /**
   * @brief Copy data from this DataBuffer into another
   * @param o_data uint32_t buffer to copy into - data is placed left aligned - must be pre-allocated
   * @param i_start Start bit of data in DataBuffer to copy
   * @param i_len Length of consecutive bits to copy
   * @post Data is copied from specified location in this DataBuffer to o_data
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t  extract(uint32_t * o_data, uint32_t i_start, uint32_t i_len) const;

  /**
   * @brief Copy data from this buffer into another at a given offset, preserving the size and other data in the output buffer
   * @param o_bufferOut Target data buffer where data is copied into
   * @param i_start Start bit in this DataBuffer to begin copy
   * @param i_len Length of consecutive bits to copy
   * @param i_targetStart Start bit in output buffer where data is copied defaults to zero
   * @post Data is copied from offset in this buffer to offset in out buffer
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval EMCD_DBUF_BUFFER_OVERFLOW data requested is out of range in one of the 2 buffers
   */
  uint32_t extractPreserve(ecmdDataBuffer & o_bufferOut, uint32_t i_start, uint32_t i_len, uint32_t i_targetStart = 0) const;

  /**
   * @brief Copy data from this DataBuffer into a generic output buffer at a given offset
   * @param o_data Array of data to write into, must be pre-allocated
   * @param i_start Start bit in this DataBuffer to begin the copy
   * @param i_len Length of consecutive bits to copy
   * @param i_targetStart Starting bit in output data to place extracted data, defaults to zero
   * @post Data is copied from offset in this DataBuffer to offset in output buffer
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_INIT_FAIL unable to allocate databuffer
   * @retval ECMD_DBUF BUFFER_OVERFLOW request is out of range for this DataBuffer, output buffer is NOT checked for overflow
   */
  uint32_t extractPreserve(uint32_t * o_data, uint32_t i_start, uint32_t i_len, uint32_t i_targetStart = 0) const; 

  /**
   * @brief Copy data from this DataBuffer into another DataBuffer and right justify
   * @param o_bufferOut DataBuffer to copy into - data is placed right aligned
   * @param i_start Start bit of data in DataBuffer to copy
   * @param i_len Length of consecutive bits to copy
   * @post Data is copied from specified location in this DataBuffer to o_bufferOut, right aligned.  Data is only right aligned if i_len < 32
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t extractToRight(ecmdDataBuffer & o_bufferOut, uint32_t i_start, uint32_t i_len) const;
  /**
   * @brief Copy data from this DataBuffer into a uint32_t buffer
   * @param o_data uint32_t buffer to copy into - data is placed right aligned - must be pre-allocated
   * @param i_start Start bit of data in DataBuffer to copy
   * @param i_len Length of consecutive bits to copy
   * @post Data is copied from specified location in this DataBuffer to o_data, right aligned.  Data is only right aligned if i_len < 32
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t extractToRight(uint32_t * o_data, uint32_t i_start, uint32_t i_len) const;

  /**
   * @brief Concatenate 2 DataBuffers into in this one
   * @param i_buf0 First DataBuffer to concatenate; copied to beginning of this buffer
   * @param i_buf1 Second DataBuffer to concatenate; copied to this buffer after the first buffer
   * @post Space is allocated, and data from the 2 DataBuffers is concatenated and copied to this buffer
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t concat(const ecmdDataBuffer & i_buf0, const ecmdDataBuffer & i_buf1);
  /**
   * @brief Concatenate 3 DataBuffers into in this one
   * @param i_buf0 First DataBuffer to concatenate; copied to beginning of this buffer
   * @param i_buf1 Second DataBuffer to concatenate; copied to this buffer after the first buffer
   * @param i_buf2 Third DataBuffer to concatenate; copied to this buffer after the second buffer
   * @post Space is allocated, and data from the 3 DataBuffers is concatenated and copied to this buffer
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t concat(const ecmdDataBuffer & i_buf0, const ecmdDataBuffer & i_buf1, const ecmdDataBuffer & i_buf2);


  /* these functions OR the datain into the DataBuffer buffer */
  /**
   * @brief OR data into DataBuffer
   * @param i_bufferIn DataBuffer to OR data from - data is taken left aligned
   * @param i_startbit Start bit to OR to
   * @param i_len Length of bits to OR
   * @post Data is ORed from i_bufferIn to this DataBuffer in specified location
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t setOr(const ecmdDataBuffer & i_bufferIn, uint32_t i_startbit, uint32_t i_len);
  /**
   * @brief OR data into DataBuffer
   * @param i_datain uint32_t buffer to OR data from - data is taken left aligned
   * @param i_startbit Start bit to OR to
   * @param i_len Length of bits to OR
   * @post Data is ORed from datain to this DataBuffer in specified location
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t setOr(const uint32_t * i_datain, uint32_t i_startbit, uint32_t i_len);
  /**
   * @brief OR data into DataBuffer
   * @param i_datain uint32_t to OR data from - data is taken left aligned
   * @param i_startbit Start bit to OR to
   * @param i_len Length of bits to OR (must be <= 32)
   * @post Data is ORed from datain to this DataBuffer in specified location
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t setOr(uint32_t i_datain, uint32_t i_startbit, uint32_t i_len);
  /**
   * @brief OR data into DataBuffer
   * @param i_bufferIn DataBuffer to OR data from - data is taken left aligned
   * @post Entire data is ORed from bufferIn to this DataBuffer
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t merge(const ecmdDataBuffer & i_bufferIn); // does a setor on the whole buffer

  /* these functions XOR the datain into the DataBuffer buffer */
  /**
   * @brief XOR data into DataBuffer
   * @param i_bufferIn DataBuffer to XOR data from - data is taken left aligned
   * @param i_startbit Start bit to XOR to
   * @param i_len Length of bits to XOR
   * @post Data is XORed from i_bufferIn to this DataBuffer in specified location
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t setXor(const ecmdDataBuffer & i_bufferIn, uint32_t i_startbit, uint32_t i_len);
  /**
   * @brief XOR data into DataBuffer
   * @param i_datain uint32_t buffer to XOR data from - data is taken left aligned
   * @param i_startbit Start bit to XOR to
   * @param i_len Length of bits to XOR
   * @post Data is XORed from datain to this DataBuffer in specified location
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t setXor(const uint32_t * i_datain, uint32_t i_startbit, uint32_t i_len);
  /**
   * @brief XOR data into DataBuffer
   * @param i_datain uint32_t to XOR data from - data is taken left aligned
   * @param i_startbit Start bit to XOR to
   * @param i_len Length of bits to XOR (must be <= 32)
   * @post Data is XORed from datain to this DataBuffer in specified location
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t setXor(uint32_t i_datain, uint32_t i_startbit, uint32_t i_len);

  /* these functions AND the datain into the DataBuffer buffer */
  /**
   * @brief AND data into DataBuffer
   * @param i_bufferIn Bitvector to AND data from - data is taken left aligned
   * @param i_startbit Start bit to AND to
   * @param i_len Length of bits to AND
   * @post Data is ANDed from bufferIn to this DataBuffer in specified location
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t setAnd(const ecmdDataBuffer & i_bufferIn, uint32_t i_startbit, uint32_t i_len);
  /**
   * @brief AND data into DataBuffer
   * @param i_datain uint32_t buffer to AND data from - data is taken left aligned
   * @param i_startbit Start bit to AND to
   * @param i_len Length of bits to AND
   * @post Data is ANDed from datain to this DataBuffer in specified location
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t setAnd(const uint32_t * i_datain, uint32_t i_startbit, uint32_t i_len);
  /**
   * @brief AND data into DataBuffer
   * @param i_datain uint32_t to AND data from - data is taken left aligned
   * @param i_startbit Start bit to AND to
   * @param i_len Length of bits to AND (must be <= 32)
   * @post Data is ANDed from datain to this DataBuffer in specified location
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t setAnd(uint32_t i_datain, uint32_t i_startbit, uint32_t i_len);

  /**
   * @brief Copy entire contents of this ecmdDataBuffer into o_copyBuffer 
   * @param o_copyBuffer DataBuffer to copy data into
   * @post copyBuffer is allocated, is an exact duplicate of this DataBuffer
   * @retval ECMD_DBUF_SUCCESS on success
   */
  uint32_t  copy(ecmdDataBuffer & o_copyBuffer) const;

#ifndef ECMD_PERLAPI
  /**
   * @brief Copy Constructor
   * @param i_master DataBuffer to copy from
   * @post this DataBuffer is allocated, is an exact duplicate of the other
   */
  ecmdDataBuffer& operator=(const ecmdDataBuffer & i_master);
#endif

  /* These are only to be used to apply a buffer to the entire ecmdDataBuffer, not just sections */
  /**
   * @brief Copy buffer into this ecmdDataBuffer
   * @param i_buf Buffer to copy from
   * @param i_bytes Byte length to copy
   * @pre DataBuffer must be pre-allocated
   * @post  Xstate and Raw buffer are set to value in i_buf for smaller of i_bytes or buffer capacity
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t  memCopyIn(const uint32_t * i_buf, uint32_t i_bytes); /* Does a memcpy from supplied buffer into ecmdDataBuffer */
  /**
   * @brief Copy DataBuffer into supplied uint32_t buffer
   * @param o_buf Buffer to copy into - must be pre-allocated
   * @param i_bytes Byte length to copy
   * @post o_buf has contents of databuffer for smaller of i_bytes or buffer capacity
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t  memCopyOut(uint32_t * o_buf, uint32_t i_bytes) const; /* Does a memcpy from ecmdDataBuffer into supplied buffer */

  /**
   * @brief Flatten all the object data into a uint8_t buffer
   * @param o_data Byte buffer to write the flattened data to - should 
   * @param i_len Number of bytes in the o_data buffer
   * @post o_data buffer has a flattened version of the DataBuffer - must be pre-allocated
   * Data format (all in network byte order):
   * First Word:      iv_Capacity*32 (in bits)
   * Second Word:     iv_NumBits
   * Remaining Words: Buffer data
   */
  uint32_t flatten(uint8_t * o_data, uint32_t i_len) const;
  /**
   * @brief Unflatten object data from a uint8_t buffer into this DataBuffer
   * @param i_data Byte buffer to read the flattened data from
   * @param i_len Number of bytes in the i_data buffer
   * @post This DataBuffer is allocated and initialized with the unflattened version of i_data
   * Data format (all in network byte order):
   * First Word:      iv_Capacity*32 (in bits)
   * Second Word:     iv_NumBits
   * Remaining Words: Buffer data
   */
  uint32_t unflatten(const uint8_t * i_data, uint32_t i_len);
  /**
   * @brief Return number of bytes needed for a buffer to flatten the object
   * @retval Number of bytes needed
   */
  uint32_t flattenSize(void) const;

  //@}


  /** @name Parity Functions */
  //@{
  /**
   * @brief Generate odd parity over a range of bits
   * @param i_start Start bit of range
   * @param i_stop Stop bit of range
   * @retval 0 or 1 depending on parity of range
   */
  uint32_t  oddParity(uint32_t i_start, uint32_t i_stop) const;
  /**
   * @brief Generate even parity over a range of bits
   * @param i_start Start bit of range
   * @param i_stop Stop bit of range
   * @retval 0 or 1 depending on parity of range
   */
  uint32_t  evenParity(uint32_t i_start, uint32_t i_stop) const;
  /**
   * @brief Generate odd parity over a range of bits and insert into DataBuffer
   * @param i_start Start bit of range
   * @param i_stop Stop bit of range
   * @param i_insertpos Bit position to insert parity
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t  oddParity(uint32_t i_start, uint32_t i_stop, uint32_t i_insertpos);
  /**
   * @brief Generate even parity over a range of bits and insert into DataBuffer
   * @param i_start Start bit of range
   * @param i_stop Stop bit of range
   * @param i_insertpos Bit position to insert parity
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t  evenParity(uint32_t i_start, uint32_t i_stop, uint32_t i_insertpos); 
  //@}


  /** @name Buffer Character Conversion Functions */
  //@{
  /**
   * @brief Return Data as a hex left aligned char string
   * @param i_start Start bit of data to convert
   * @param i_bitlen Number of consecutive bits to convert
   * @retval String containing requested data
   */
  std::string genHexLeftStr(uint32_t i_start, uint32_t i_bitlen) const;
  /**
   * @brief Return Data as a hex right aligned char string
   * @param i_start Start bit of data to convert
   * @param i_bitlen Number of consecutive bits to convert
   * @retval String containing requested data
   */
  std::string genHexRightStr(uint32_t i_start, uint32_t i_bitlen) const;
  /**
   * @brief Return Data as a binary char string
   * @param i_start Start bit of data to convert
   * @param i_bitlen Number of consecutive bits to convert
   * @retval String containing requested data
   */
  std::string genBinStr(uint32_t i_start, uint32_t i_bitlen) const; 
  /**
   * @brief Return Data as an ASCII char string.  If it's out of range, a . is printed.
   * @param i_start Start bit of data to convert
   * @param i_bitlen Number of consecutive bits to convert
   * @retval String containing requested data
   */
  std::string genAsciiStr(uint32_t i_start, uint32_t i_bitlen) const; 

  /**
   * @brief Return entire buffer as a hex left aligned char string
   * @retval String containing requested data
   */
  std::string genHexLeftStr() const;
  /**
   * @brief Return entire buffer as a hex right aligned char string
   * @retval String containing requested data
   */
  std::string genHexRightStr() const;
  /**
   * @brief Return entire buffer as a binary char string
   * @retval String containing requested data
   */
  std::string genBinStr() const;
  /**
   * @brief Return Data as an ASCII char string.  If it's out of range, a . is printed.
   * @retval String containing requested data
   */
  std::string genAsciiStr() const; 

  /**
   * @brief Retrieve a section of the Xstate Data
   * @param i_start Start bit of data to retrieve
   * @param i_bitlen Number of consecutive bits to retrieve
   * @retval String containing requested data
   */
  std::string genXstateStr(uint32_t i_start, uint32_t i_bitlen) const;

  /**
   * @brief Retrieve entire Xstate Data buffer
   * @retval String containing requested data
   */
  std::string genXstateStr() const;


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
  uint32_t insertFromHexLeft (const char * i_hexChars, uint32_t i_start = 0, uint32_t i_length = 0);

  /**
   * @brief Convert data from a hex left-aligned string and insert it into this data buffer - and set's buffer length to size of data
   * @param i_hexChars Hex Left-aligned string of data to insert 
   * @param i_start Starting position in data buffer to insert to, 0 by default
   * @param i_length Length of data to insert, defaults to length of i_hexChars, zeroes are padded or data dropped from right if necessary
   * @retval ECMD_DBUF_INVALID_DATA_FORMAT if non-hex chars detected in i_hexChars
   * @retval ECMD_SUCCESS on success
   * @retval non-zero on failure
   */
  uint32_t insertFromHexLeftAndResize (const char * i_hexChars, uint32_t i_start = 0, uint32_t i_length = 0);

  /**
   * @brief Convert data from a hex right-aligned string and insert it into this data buffer
   * @param i_hexChars Hex Right-aligned string of data to insert
   * @param i_expectedLength The expected length of the string data, zeros are padded or data dropped from the left if necessary
   * @param i_start Starting position in data buffer to insert to, 0 by default
   * @retval ECMD_DBUF_INVALID_DATA_FORMAT if non-hex chars detected in i_hexChars
   * @retval ECMD_SUCCESS on success
   * @retval non-zero on failure
   */
  uint32_t insertFromHexRight (const char * i_hexChars, uint32_t i_start = 0, uint32_t i_expectedLength = 0);

  /**
   * @brief Convert data from a hex right-aligned string and insert it into this data buffer - and set's buffer length to size of data
   * @param i_hexChars Hex Right-aligned string of data to insert
   * @param i_expectedLength The expected length of the string data, zeros are padded or data dropped from the left if necessary
   * @param i_start Starting position in data buffer to insert to, 0 by default
   * @retval ECMD_DBUF_INVALID_DATA_FORMAT if non-hex chars detected in i_hexChars
   * @retval ECMD_SUCCESS on success
   * @retval non-zero on failure
   */
  uint32_t insertFromHexRightAndResize (const char * i_hexChars, uint32_t i_start = 0, uint32_t i_expectedLength = 0);

  /**
   * @brief Convert data from a binary string and insert it into this data buffer
   * @retval 0 on success- non-zero on failure
   * @param i_binChars String of 0's and 1's to insert 
   * @param i_start Starting position in data buffer to insert to, 0 by default
   * @retval ECMD_DBUF_INVALID_DATA_FORMAT if non-binary chars detected in i_binChars
   * @retval ECMD_SUCCESS on success
   * @retval non-zero on failure

   */
  uint32_t insertFromBin (const char * i_binChars, uint32_t i_start = 0);

  /**
   * @brief Convert data from a binary string and insert it into this data buffer - and set's buffer length to size of data
   * @retval 0 on success- non-zero on failure
   * @param i_binChars String of 0's and 1's to insert 
   * @param i_start Starting position in data buffer to insert to, 0 by default
   * @retval ECMD_DBUF_INVALID_DATA_FORMAT if non-binary chars detected in i_binChars
   * @retval ECMD_SUCCESS on success
   * @retval non-zero on failure

   */
  uint32_t insertFromBinAndResize (const char * i_binChars, uint32_t i_start = 0);
  //@}


  /** @name Simulation Buffer Functions */
  //@{

  /**
   * @brief Load entire buffer with an X-state value
   * @param i_value Value to load into buffer
   * @retval ECMD_DBUF_SUCCESS on success
   */
  uint32_t flushToX(char i_value);
 
  /**
   * @brief Check Entire buffer for any X-state values
   * @retval true if xstate found false if none
   */
  bool   hasXstate() const; /* check the whole DataBuffer */
  /**
   * @brief Check section of buffer for any X-state values
   * @param i_start Start bit to test
   * @param i_length Number of consecutive bits to test
   * @retval true if xstate found false if none
   */
  bool   hasXstate(uint32_t i_start, uint32_t i_length) const; /* check subset */

  /**
   * @brief Retrieve an Xstate value from the buffer
   * @param i_bit Bit to retrieve

   * NOTE - To retrieve multiple bits use genXstateStr
   */
  char  getXstate(uint32_t i_bit) const;

  /**
   * @brief Set an Xstate value in the buffer
   * @param i_bit Bit to set
   * @param i_value Xstate value to set
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t setXstate(uint32_t i_bit, char i_value);

  /**
   * @brief Set an Xstate value in the buffer
   * @param i_bit Bit to set
   * @param i_value Xstate value to set
   * @param i_length Number of consecutive bits to set to i_value
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t setXstate(uint32_t i_bit, char i_value, uint32_t i_length);

  /**
   * @brief Set a range of Xstate values in buffer
   * @param i_bitoffset bit in buffer to start inserting
   * @param i_datastr Character value to set bit - can be "0XX0", "1", "X"
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t  setXstate(uint32_t i_bitoffset, const char* i_datastr); 

  /**
   * @brief Copy buffer into the Xstate data of this ecmdDataBuffer
   * @param i_buf Buffer to copy from
   * @param i_bytes Byte length to copy (char length)
   * @post  Xstate and Raw buffer are set to value in i_buf for smaller of i_bytes or buffer capacity
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t  memCopyInXstate(const char * i_buf, uint32_t i_bytes); /* Does a memcpy from supplied buffer into ecmdDataBuffer */
  /**
   * @brief Copy DataBuffer into supplied char buffer from Xstate data
   * @param o_buf Buffer to copy into - must be pre-allocated
   * @param i_bytes Byte length to copy (char length)
   * @post o_buf has contents of databuffer for smaller of i_bytes or buffer capacity
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_BUFFER_OVERFLOW operation requested out of range
   */
  uint32_t  memCopyOutXstate(char * o_buf, uint32_t i_bytes) const; /* Does a memcpy from ecmdDataBuffer into supplied buffer */

  //@}

  /** @name Misc Functions */
  //@{

  /**
   * @brief Write buffer out into a file in the format specified. 
   * @param i_filename file to write to
   * @param i_format format to write in
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_FOPEN_FAIL Unable to open the file for write
   * @retval ECMD_DBUF_XSTATE_ERROR If Xstate values are detected on non-Xstate format request
   */
  uint32_t  writeFile(const char * i_filename, ecmdFormatType_t i_format); /* Writes out the buffer (in the specified format) into the file */
  
  /**
   * @brief Writes/Appends buffer out into a file in the format specified
   * @param i_filename file to write to
   * @param i_format format to write in
   * @param i_mode mode to open the file in 
   * @param o_dataNumber the sequence number for this data, used by readFileMultiple to pick the right databuffer
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_FOPEN_FAIL Unable to open the file for write
   * @retval ECMD_DBUF_XSTATE_ERROR If Xstate values are detected on non-Xstate format request
   * ECMD_SAVE_FORMAT_BINARY_DATA not accepted when ecmdWriteMode_t is ECMD_APPEND_MODE 
   */
  uint32_t  writeFileMultiple(const char * i_filename, ecmdFormatType_t i_format, ecmdWriteMode_t i_mode, uint32_t &  o_dataNumber); /* Writes out the buffer (in the specified format) into the file */
  
  /**
   * @brief Read data from the file into the buffer  
   * @param i_filename to read from
   * @param i_format data format to expect in the file
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_FILE_FORMAT_MISMATCH specified format not found in the file
   * @retval ECMD_DBUF_FOPEN_FAIL Unable to open the file for read
   * @retval ECMD_DBUF_XSTATE_ERROR If XState format is requested when XState is not defined for the configuration
   */
  uint32_t  readFile(const char * i_filename, ecmdFormatType_t i_format); /* Writes out the buffer (in the specified format) into the file */
  
  /**
   * @brief Read data from the file into the buffer  
   * @param i_filename to read from
   * @param i_format data format to expect in the file
   * @param i_dataNumber data requested in case of multiple databuffers
   * @retval ECMD_DBUF_SUCCESS on success
   * @retval ECMD_DBUF_FILE_FORMAT_MISMATCH specified format not found in the file
   * @retval ECMD_DBUF_FOPEN_FAIL Unable to open the file for read
   * @retval ECMD_DBUF_XSTATE_ERROR If XState format is requested when XState is not defined for the configuration
   * ECMD_SAVE_FORMAT_BINARY_DATA not accepted when i_dataNumber != 0
   */
  uint32_t  readFileMultiple(const char * i_filename, ecmdFormatType_t i_format, uint32_t i_dataNumber=0); /* Reads out the buffer (in the  specified format) from the file */
  
  /**
   * @brief This function will take the passed in buffer, delete any current data it holds, and point its data var to that which is owned by the one being called with.  It will not have iv_UserOwned flag set, so it should not delete the buffer it points to, nor resize it, but it can alter the data.  The use of this function is for caching data for reads.
   * @param i_sharingBuffer input buffer
   * @retval ECMD_DBUF_SUCCESS on success
   */   
  uint32_t shareBuffer(ecmdDataBuffer* i_sharingBuffer);

  //@}

  /** @name Operator overloads */
  //@{
  /**
   * @brief Overload the == operator
   */
  int operator == (const ecmdDataBuffer& other) const;    

  /**
   * @brief Overload the != operator
   */
  int operator != (const ecmdDataBuffer& other) const;    

  /**
   * @brief Overload the & operator
   */
  ecmdDataBuffer operator & (const ecmdDataBuffer& other) const;    

  /**
   * @brief Overload the | operator
   */
  ecmdDataBuffer operator | (const ecmdDataBuffer& other) const;    

  //@}


protected:  //data
  uint32_t iv_Capacity;         ///< Actual buffer capacity - always >= iv_NumWords
  uint32_t iv_NumWords;         ///< Specified buffer size rounded to next word
  uint32_t iv_NumBits;          ///< Specified buffer size in bits
  uint32_t * iv_Data;           ///< Pointer to buffer inside iv_RealData
  uint32_t * iv_RealData;       ///< Real buffer - with header and tail
  bool     iv_UserOwned;        ///< Whether or not this buffer owns the data

protected: //methods


protected:
#ifndef REMOVE_SIM
  char* iv_DataStr; /* binary or x data */
  uint32_t fillDataStr(char fillChar);
#endif

};


#endif  /* ecmdDataBuffer_H */
