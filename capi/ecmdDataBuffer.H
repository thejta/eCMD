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

/**
 * @file ecmdDataBuffer.H
 * @brief Provides a means to handle data from the eCMD C API
 *
 * DataBuffers handle data in a Big Endian fashion with Bit 0 being the MSB
*/


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <stdint.h>

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

     /**
      * @brief Default Constructor
      */
     ecmdDataBuffer();

     /**
      * @brief Constructor
      * @param numBits Size of data to initialize in bits
      * @post ecmdDataBuffer is initialzed with a buffer
      */
     ecmdDataBuffer(int numBits); 

     /**
      * @brief Default Destructor
      */
     ~ecmdDataBuffer();

     // Member Functions
     /** @name Buffer Size Function */
     //@{
     /**
      * @brief Return the length of the buffer in words
      * @retval Buffer length in words
      */
     int   getWordLength() const;
     /**
      * @brief Return the length of the buffer in bits
      * @retval Buffer length in bits
      */
     int   getBitLength() const;
     /**
      * @brief Return the actual capacity of the internal buffer
      * @retval Actual capacity of internal buffer
      */
     int   getCapacity() const;
     /**
      * @brief Reinitialize the Buffer to specified length
      * @param newNumWords Length of new buffer in words
      * @post Buffer is reinitialized
      *
      * NOTE : All data stored in buffer will be lost
      */
     void  setWordLength(int newNumWords);   
     /**
      * @brief Reinitialize the Buffer to specified length
      * @param newNumBits Length of new buffer in bits
      * @post Buffer is reinitialized
      *
      * NOTE : All data stored in buffer will be lost
      */
     void  setBitLength(int newNumBits);
     /**
      * @brief Reinitialize the internal buffer to specified length
      * @param newNumWords length of internal data buffer in words
      * @post Internal buffer is reinitialzied
      */
     void setCapacity (int newNumWords);

     //@}



     /** @name Bit/Word Manipulation Functions */
     //@{
     /**
      * @brief Turn on a bit in buffer
      * @param bit Bit in buffer to turn on
      */
     void  setBit(int bit);
     /**
      * @brief Turn on a bit in buffer
      * @param bit start bit in buffer to turn on
      * @param len Number of consecutive bits from start bit to turn on
      */
     void  setBit(int bit, int len);
     /**
      * @brief Turn on a bit in buffer
      * @param bitoffset bit in buffer to turn on
      * @param datastr Character value to set bit - can be "0", "1", "X"
      */
     void  setBit(int bitoffset, const char* datastr); // data can be 0,1,'x','X' 
     /**
      * @brief Set a word of data in buffer
      * @param wordoffset Offset of word to set
      * @param value 32 bits of data to put into word
      */
     void  setWord(int wordoffset, uint32_t value);

     /**
      * @brief Fetch a word from ecmdDataBuffer
      * @param wordoffset Offset of word to fetch
      * @retval Value of word requested
      */
     uint32_t getWord(int wordoffset);

     /**
      * @brief Clear a bit in buffer
      * @param bit Bit in buffer to turn off
      */
     void  clearBit(int bit);
     /**
      * @brief Clear multiple bits in buffer
      * @param bit Start bit in buffer to turn off
      * @param len Number of consecutive bits from start bit to off
      */
     void  clearBit(int bit, int len);

     /**
      * @brief Invert bit
      * @param bit Bit in buffer to invert
      */
     void  flipBit(int bit);
     /**
      * @brief Invert multiple bits
      * @param bit Start bit in buffer to invert
      * @param len Number of consecutive bits to invert
      */
     void  flipBit(int bit, int len);

     /**
      * @brief Test if bit is set
      * @param bit Bit to test
      * @retval 1 if bit is set - 0 if bit is clear
      */
     int   isBitSet(int bit);
     /**
      * @brief Test if multiple bits are set
      * @param bit Start bit to test
      * @param len Number of consecutive bits to test
      * @retval 1 if bit is set - 0 if bit is clear
      */
     int   isBitSet(int bit, int len);
     /**
      * @brief Test if bit is clear
      * @param bit Bit to test
      * @retval 1 if bit is clear - 0 if bit is set
      */
     int   isBitClear(int bit);
     /**
      * @brief Test if multiple bits are clear
      * @param bit Start bit to test
      * @param len Number of consecutive bits to test
      * @retval 1 if bit is clear - 0 if bit is set
      */
     int   isBitClear(int bit, int len);
     /**
      * @brief Count number of bits set in a range
      * @param bit Start bit to test
      * @param len Number of consecutive bits to test
      * @retval Number of bits set in range
      */      
     int   getNumBitsSet(int bit, int len);
     //@}


     /** @name Buffer Manipulation Functions */
     //@{
     /**
      * @brief Shift data to right
      * @param shiftnum Number of bits to shift
      * @post Bits in buffer are shifted to right by specified number of bits - data is shifted off the end
      */
     void  shiftRight(int shiftnum);
     /**
      * @brief Shift data to left
      * @param shiftnum Number of bits to shift
      * @post Bits in buffer are shifted to left by specified number of bits - data is shifted off the beginning
      */
     void  shiftLeft(int shiftnum);
     /**
      * @brief Rotate data to right
      * @param rotatenum Number of bits to rotate
      * @post Bits in buffer are rotated to the right by specified number of bits - data is rotated to the beginning
      */
     void  rotateRight(int rotatenum);
     /**
      * @brief Rotate data to left
      * @param rotatenum Number of bits to rotate
      * @post Bits in buffer are rotated to the left by specified number of bits - data is rotated to the end
      */
     void  rotateLeft(int rotatenum);

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
      * @brief Insert part of another DataBuffer into this one
      * @param bufferIn DataBuffer to copy data from - data is taken left aligned
      * @param start Start bit to insert to
      * @param len Length of bits to insert
      * @post Data is copied from bufferIn to this DataBuffer in specified location
      */
     void  insert(ecmdDataBuffer & bufferIn, int start, int len);
     /**
      * @brief Insert a uint32_t array into this DataBuffer
      * @param datain uint32_t array to copy into this DataBuffer - data is taken left aligned
      * @param start Start bit to insert into
      * @param len Length of bits to insert
      * @post Data is copied from datain into this DataBuffer at specified location
      */
     void  insert(uint32_t * datain, int start, int len);
     /**
      * @brief Insert a uint32_t into the DataBuffer
      * @param datain uint32_t value to copy into DataBuffer - data is taken left aligned
      * @param start Start bit to insert into
      * @param len Length of bits to insert (must be <= 32)
      * @post Data is copied from datain into this DataBuffer at specified location
      */
     void  insert(uint32_t datain, int start, int len);
     /**
      * @brief Copy data from this DataBuffer into another
      * @param bufferOut DataBuffer to copy into - data is placed left aligned
      * @param start Start bit of data to copy
      * @param len Length of consecutive bits to copy
      * @post Data is copied from specified location in this DataBuffer to bufferOut
      */
     void  extract(ecmdDataBuffer & bufferOut, int start, int len);
     /**
      * @brief Copy data from this DataBuffer into another
      * @param outdata uint32_t buffer to copy into - data is placed left aligned - must be pre-allocated
      * @param start Start bit of data to copy
      * @param len Length of consecutive bits to copy
      * @post Data is copied from specified location in this DataBuffer to outdata
      */
     void  extract(uint32_t * outdata, int start, int len);

     /* these functions OR the datain into the DataBuffer buffer */
     /**
      * @brief OR data into DataBuffer
      * @param bufferIn DataBuffer to OR data from - data is taken left aligned
      * @param startbit Start bit to OR to
      * @param len Length of bits to OR
      * @post Data is ORed from bufferIn to this DataBuffer in specified location
      */
     void setOr(ecmdDataBuffer & bufferIn, int startbit, int len);
     /**
      * @brief OR data into DataBuffer
      * @param datain uint32_t buffer to OR data from - data is taken left aligned
      * @param startbit Start bit to OR to
      * @param len Length of bits to OR
      * @post Data is ORed from datain to this DataBuffer in specified location
      */
     void setOr(uint32_t * datain, int startbit, int len);
     /**
      * @brief OR data into DataBuffer
      * @param datain uint32_t to OR data from - data is taken left aligned
      * @param startbit Start bit to OR to
      * @param len Length of bits to OR (must be <= 32)
      * @post Data is ORed from datain to this DataBuffer in specified location
      */
     void setOr(uint32_t datain, int startbit, int len);
     /**
      * @brief OR data into DataBuffer
      * @param bufferIn DataBuffer to OR data from - data is taken left aligned
      * @post Entire data is ORed from bufferIn to this DataBuffer
      */
     void merge(ecmdDataBuffer & bufferIn); // does a setor on the whole buffer

     /* these functions AND the datain into the DataBuffer buffer */
     /**
      * @brief AND data into DataBuffer
      * @param bufferIn Bitvector to AND data from - data is taken left aligned
      * @param startbit Start bit to AND to
      * @param len Length of bits to AND
      * @post Data is ANDed from bufferIn to this DataBuffer in specified location
      */
     void setAnd(ecmdDataBuffer & bufferIn, int startbit, int len);
     /**
      * @brief AND data into DataBuffer
      * @param datain uint32_t buffer to AND data from - data is taken left aligned
      * @param startbit Start bit to AND to
      * @param len Length of bits to AND
      * @post Data is ANDed from datain to this DataBuffer in specified location
      */
     void setAnd(uint32_t *datain, int startbit, int len);
     /**
      * @brief AND data into DataBuffer
      * @param datain uint32_t to AND data from - data is taken left aligned
      * @param startbit Start bit to AND to
      * @param len Length of bits to AND (must be <= 32)
      * @post Data is ANDed from datain to this DataBuffer in specified location
      */
     void setAnd(uint32_t datain, int startbit, int len);

     /**
      * @brief Copy entire contents of this ecmdDataBuffer into newcopy 
      * @param copyBuffer DataBuffer to copy data into
      * @post copyBuffer is an exact duplicate of this DataBuffer
      */
     void  copy(ecmdDataBuffer & copyBuffer); // Copies data into other

     /* These are only to be used to apply a buffer to the entire ecmdDataBuffer, not just sections */
     /**
      * @brief Copy buffer into this ecmdDataBuffer
      * @param buf Buffer to copy from
      * @param bytes Byte length to copy
      */
     void  memCopyIn(uint32_t* buf, int bytes); /* Does a memcpy from supplied buffer into ecmdDataBuffer */
     /**
      * @brief Copy DataBuffer into supplied uint32_t buffer
      * @param buf Buffer to copy into - must be pre-allocated
      * @param bytes Byte length to copy
      */
     void  memCopyOut(uint32_t* buf, int bytes); /* Does a memcpy from ecmdDataBuffer into supplied buffer */

      //@}


      /** @name Parity Functions */
      //@{
      /**
       * @brief Generate odd parity over a range of bits
       * @param start Start bit of range
       * @param stop Stop bit of range
       * @retval 0 or 1 depending on parity of range
       */
     int  oddParity(int start, int stop);
      /**
       * @brief Generate even parity over a range of bits
       * @param start Start bit of range
       * @param stop Stop bit of range
       * @retval 0 or 1 depending on parity of range
       */
     int  evenParity(int start, int stop);
      /**
       * @brief Generate odd parity over a range of bits and insert into DataBuffer
       * @param start Start bit of range
       * @param stop Stop bit of range
       * @param insertpos Bit position to insert parity
       * @retval 0 on success - nonzero on failure
       */
     int  oddParity(int start, int stop, int insertpos); 
      /**
       * @brief Generate even parity over a range of bits and insert into DataBuffer
       * @param start Start bit of range
       * @param stop Stop bit of range
       * @param insertpos Bit position to insert parity
       * @retval 0 on success - nonzero on failure
       */
     int  evenParity(int start, int stop, int insertpos); 
     //@}


     /** @name Buffer Character Conversion Functions */
     //@{
     /**
      * @brief Return Data as a hex left aligned char string
      * @param start Start bit of data to convert
      * @param bitlen Number of consecutive bits to convert
      * @retval String containing requested data
      */
     const char* genHexLeftStr(int start, int bitlen);
     /**
      * @brief Return Data as a hex right aligned char string
      * @param start Start bit of data to convert
      * @param bitlen Number of consecutive bits to convert
      * @retval String containing requested data
      */
     const char* genHexRightStr(int start, int bitlen); 
     /**
      * @brief Return Data as a binary char string
      * @param start Start bit of data to convert
      * @param bitlen Number of consecutive bits to convert
      * @retval String containing requested data
      */
     const char* genBinStr(int start, int bitlen); 
     /**
      * @brief Return entire buffer as a hex left aligned char string
      * @retval String containing requested data
      */
     const char* genHexLeftStr();
     /**
      * @brief Return entire buffer as a hex right aligned char string
      * @retval String containing requested data
      */
     const char* genHexRightStr(); 
     /**
      * @brief Return entire buffer as a binary char string
      * @retval String containing requested data
      */
     const char* genBinStr();
     //@}

     /** @name Simulation Buffer Functions */
     //@{
     /**
      * @brief Check Entire buffer for any X-state values
      * @retval 1 if xstate found 0 if none
      */
     int   isXstate(); /* check the whole DataBuffer */
     /**
      * @brief Check section buffer for any X-state values
      * @param start Start bit to test
      * @param length Number of consecutive bits to test
      * @retval 1 if xstate found 0 if none
      */
     int   isXstate(int start, int length); /* check subset */

     //@}

   private:  //data
     int iv_Capacity;
     int iv_NumWords;
     int iv_NumBits;
     uint32_t * iv_Data;
     uint32_t * iv_RealData;
     char* iv_DataOutStr; /* buffer for gethexlstr functions */


   private: //methods
     ecmdDataBuffer(const ecmdDataBuffer &other);

#ifndef REMOVE_SIM_BUFFERS
   private:
     char* iv_DataStr; /* binary or x data */

   private:
     void fillDataStr(char fillChar);
#endif


};


#endif  /* ecmdDataBuffer_H */

    
