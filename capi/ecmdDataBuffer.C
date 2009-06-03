/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File ecmdDataBuffer.C                                   
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
// End Copyright *******************************************************
                               
// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder     Description                       
//  ---- -------- ---- -------- -----     -----------------------------
//  @01  STG4466       03/10/05 Prahl     Fix up Beam errors
// End Change Log *****************************************************

// Get rid of some annoying lint errors that aren't real - cje
//lint -e613 Possible use of null pointer, lint doesn't understand we use iv_NumBits to check length and pointer validity
//lint -e668 Possible passing of a null pointer, same thing as above
//lint -e527 Lint doesn't understand that iv_UserOwned is changed in shareBuffer

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>
#include <netinet/in.h> /* for htonl */
#include <fstream>
#include <iostream>

#include <ecmdDefines.H>
#include <ecmdDataBuffer.H>
#include <prdfCompressBuffer.H>

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------
#ifdef FIPSODE
tracDesc_t g_etrc; /** Trace Descriptor **/
TRAC_INIT(&g_etrc, "ECMD", 0x1000);
#elif defined ZSERIES_SWITCH
#define TRACE_ID ECMDBUF
#endif

//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------
#define EDB_RANDNUM 0x12345678
#define EDB_ADMIN_HEADER_SIZE 1
#define EDB_ADMIN_FOOTER_SIZE 1
// This define is the sum of EDB_ADMIN_HEADER_SIZE + EDB_ADMIN_FOOTER_SIZE
#define EDB_ADMIN_TOTAL_SIZE 2
#define EDB_RETURN_CODE 0

// New Constants for improved performance
//#define MIN(x,y)            (((x)<(y))?x:y) - Removed 7/23/08 because prdfCompressBuffer.H defines this function
#define UNIT_SZ             32

#define RETURN_ERROR(i_rc) if ((iv_RealData != NULL) && (iv_RealData[EDB_RETURN_CODE] == 0)) { iv_RealData[EDB_RETURN_CODE] = i_rc; } return i_rc;
#define SET_ERROR(i_rc) if ((iv_RealData != NULL) && (iv_RealData[EDB_RETURN_CODE] == 0)) { iv_RealData[EDB_RETURN_CODE] = i_rc; }

//----------------------------------------------------------------------
//  Forward declarations
//----------------------------------------------------------------------
uint32_t ecmdExtract(uint32_t *i_sourceData, uint32_t i_startBit, uint32_t i_numBitsToExtract, uint32_t *o_destData);
void * ecmdBigEndianMemCopy(void * dest, const void *src, size_t count);

// new declaration here for performance improvement
// This function does NOT do input checks and does NOT handle xstate
inline uint32_t ecmdFastInsert(uint32_t *i_target,const uint32_t * i_dataIn, uint32_t i_targetStart, uint32_t i_len, uint32_t i_sourceStart); 


//----------------------------------------------------------------------
//  Inlined Functions used to improve Performance
//----------------------------------------------------------------------
inline /* leave this inlined */
uint32_t fast_mask32(int32_t i_pos, int32_t i_len) {
  /* generates an arbitrary 32-bit mask using two
   operations, not too shabby */

  static const uint32_t l_mask32[] = {
    0x00000000,
    0x80000000, 0xC0000000, 0xE0000000, 0xF0000000,
    0xF8000000, 0xFC000000, 0xFE000000, 0xFF000000,
    0xFF800000, 0xFFC00000, 0xFFE00000, 0xFFF00000,
    0xFFF80000, 0xFFFC0000, 0xFFFE0000, 0xFFFF0000,
    0xFFFF8000, 0xFFFFC000, 0xFFFFE000, 0xFFFFF000,
    0xFFFFF800, 0xFFFFFC00, 0xFFFFFE00, 0xFFFFFF00,
    0xFFFFFF80, 0xFFFFFFC0, 0xFFFFFFE0, 0xFFFFFFF0,
    0xFFFFFFF8, 0xFFFFFFFC, 0xFFFFFFFE, 0xFFFFFFFF,
  };
  return l_mask32[i_len] >> i_pos;
}

inline /* leave this inlined */
uint32_t fast_set32(uint32_t i_trg, int32_t i_pos, int32_t i_len) {
  return fast_mask32(i_pos, i_len) | i_trg;
}

inline /* leave this inlined */
uint32_t fast_clear32(uint32_t i_trg, int32_t i_pos, int32_t i_len) {
  return fast_mask32(i_pos, i_len) & ~i_trg;
}

inline /* leave this inlined */
uint8_t fast_reverse8(uint8_t data) {
  static const uint8_t reverse8[] = {
    0x00,0x80,0x40,0xc0,0x20,0xa0,0x60,0xe0,
    0x10,0x90,0x50,0xd0,0x30,0xb0,0x70,0xf0,
    0x08,0x88,0x48,0xc8,0x28,0xa8,0x68,0xe8,
    0x18,0x98,0x58,0xd8,0x38,0xb8,0x78,0xf8,
    0x04,0x84,0x44,0xc4,0x24,0xa4,0x64,0xe4,
    0x14,0x94,0x54,0xd4,0x34,0xb4,0x74,0xf4,
    0x0c,0x8c,0x4c,0xcc,0x2c,0xac,0x6c,0xec,
    0x1c,0x9c,0x5c,0xdc,0x3c,0xbc,0x7c,0xfc,
    0x02,0x82,0x42,0xc2,0x22,0xa2,0x62,0xe2,
    0x12,0x92,0x52,0xd2,0x32,0xb2,0x72,0xf2,
    0x0a,0x8a,0x4a,0xca,0x2a,0xaa,0x6a,0xea,
    0x1a,0x9a,0x5a,0xda,0x3a,0xba,0x7a,0xfa,
    0x06,0x86,0x46,0xc6,0x26,0xa6,0x66,0xe6,
    0x16,0x96,0x56,0xd6,0x36,0xb6,0x76,0xf6,
    0x0e,0x8e,0x4e,0xce,0x2e,0xae,0x6e,0xee,
    0x1e,0x9e,0x5e,0xde,0x3e,0xbe,0x7e,0xfe,
    0x01,0x81,0x41,0xc1,0x21,0xa1,0x61,0xe1,
    0x11,0x91,0x51,0xd1,0x31,0xb1,0x71,0xf1,
    0x09,0x89,0x49,0xc9,0x29,0xa9,0x69,0xe9,
    0x19,0x99,0x59,0xd9,0x39,0xb9,0x79,0xf9,
    0x05,0x85,0x45,0xc5,0x25,0xa5,0x65,0xe5,
    0x15,0x95,0x55,0xd5,0x35,0xb5,0x75,0xf5,
    0x0d,0x8d,0x4d,0xcd,0x2d,0xad,0x6d,0xed,
    0x1d,0x9d,0x5d,0xdd,0x3d,0xbd,0x7d,0xfd,
    0x03,0x83,0x43,0xc3,0x23,0xa3,0x63,0xe3,
    0x13,0x93,0x53,0xd3,0x33,0xb3,0x73,0xf3,
    0x0b,0x8b,0x4b,0xcb,0x2b,0xab,0x6b,0xeb,
    0x1b,0x9b,0x5b,0xdb,0x3b,0xbb,0x7b,0xfb,
    0x07,0x87,0x47,0xc7,0x27,0xa7,0x67,0xe7,
    0x17,0x97,0x57,0xd7,0x37,0xb7,0x77,0xf7,
    0x0f,0x8f,0x4f,0xcf,0x2f,0xaf,0x6f,0xef,
    0x1f,0x9f,0x5f,0xdf,0x3f,0xbf,0x7f,0xff,
  };
  return reverse8[data];
}

inline /* leave this inlined */
uint32_t fast_reverse32(uint32_t data) {
  return fast_reverse8((data & 0xFF000000) >> 24) |
    fast_reverse8((data & 0x00FF0000) >> 16) << 8 |
    fast_reverse8((data & 0x0000FF00) >> 8) << 16 |
    fast_reverse8(data & 0x000000FF) << 24;
}

//---------------------------------------------------------------------
//  Constructors
//---------------------------------------------------------------------
ecmdDataBuffer::ecmdDataBuffer()  // Default constructor
: iv_Capacity(0), iv_NumWords(0), iv_NumBits(0), iv_Data(NULL), iv_RealData(NULL)
{
  iv_UserOwned = true;
  iv_BufferOptimizable = false;

#ifndef REMOVE_SIM
  iv_DataStr = NULL;
  iv_XstateEnabled = false;
#endif
}

ecmdDataBuffer::ecmdDataBuffer(uint32_t i_numBits)
: iv_Capacity(0), iv_NumWords(0), iv_NumBits(0), iv_Data(NULL), iv_RealData(NULL)
{
  iv_UserOwned = true;
  iv_BufferOptimizable = false;

#ifndef REMOVE_SIM
  iv_DataStr = NULL;
  iv_XstateEnabled = false;
#endif

  if (i_numBits > 0) {
    setBitLength(i_numBits);
  }
}

ecmdDataBuffer::ecmdDataBuffer(const ecmdDataBuffer& i_other) 
: iv_Capacity(0), iv_NumWords(0), iv_NumBits(0), iv_Data(NULL), iv_RealData(NULL)
{
  iv_UserOwned = true;
  iv_BufferOptimizable = false;

#ifndef REMOVE_SIM
  iv_DataStr = NULL;
  iv_XstateEnabled = false;
#endif

  if (i_other.iv_NumBits != 0) {

    this->setBitLength(i_other.iv_NumBits);
    // iv_Data
    memcpy(iv_Data, i_other.iv_Data, iv_NumWords * 4);
    // Error state
    iv_RealData[EDB_RETURN_CODE] = i_other.iv_RealData[EDB_RETURN_CODE];


#ifndef REMOVE_SIM
    if (i_other.isXstateEnabled()) {
      /* enable my xstate */
      enableXstateBuffer();
      strncpy(iv_DataStr, i_other.iv_DataStr, iv_NumBits);
    }
#endif
  }
  /* else do nothing.  already have an empty buffer */

}

//---------------------------------------------------------------------
//  Destructor
//---------------------------------------------------------------------
ecmdDataBuffer::~ecmdDataBuffer()
{
    // Only call clear() if buffer is owned by this user (ie, not shared)
    if(iv_UserOwned) clear();
}

//---------------------------------------------------------------------
//  Public Member Function Specifications
//---------------------------------------------------------------------
uint32_t ecmdDataBuffer::clear() {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if(!iv_UserOwned)  // If this buffer is shared
  {
      if (!isBufferOptimizable()) { // If the buffer is not optimizable, error
        ETRAC0("**** ERROR (ecmdDataBuffer) : Attempt to modify non user owned buffer size.");
        RETURN_ERROR(ECMD_DBUF_NOT_OWNER);
      }
      else {  // It's a shared/optimizable buffer, don't flag error
        return ECMD_DBUF_SUCCESS;
      }
  }

  if (iv_RealData != NULL) {

    /* Let's check our header,footer info */
    if (iv_RealData[iv_NumWords + EDB_ADMIN_HEADER_SIZE] != EDB_RANDNUM) {
      /* Ok, something is wrong here */
      ETRAC2("**** SEVERE ERROR (ecmdDataBuffer) : iv_RealData[0]: %X, iv_NumWords: %X",iv_RealData[EDB_RETURN_CODE],iv_NumWords);
      ETRAC1("**** SEVERE ERROR (ecmdDataBuffer) : iv_RealData[iv_NumWords + EDB_ADMIN_HEADER_SIZE]: %X",iv_RealData[iv_NumWords + EDB_ADMIN_HEADER_SIZE]);
      ETRAC0("**** SEVERE ERROR (ecmdDataBuffer) : PROBLEM WITH DATABUFFER - INVALID HEADER/FOOTER");
      abort();
    }

    /* That looked okay, reset everything else */
    /* Only do the delete if we alloc'd something */
    if (iv_RealData != iv_LocalData) {
      delete[] iv_RealData;
    }
    iv_RealData = NULL;
    iv_Capacity = 0;
    iv_NumWords = 0;
    iv_NumBits = 0;
  }

#ifndef REMOVE_SIM
  if (iv_DataStr != NULL) {
    delete[] iv_DataStr;
    iv_DataStr = NULL;
  }
  iv_XstateEnabled = false;
#endif

  return rc;
}


uint32_t ecmdDataBuffer::getWordLength() const { return iv_NumWords; }
uint32_t ecmdDataBuffer::getBitLength() const { return iv_NumBits; }
uint32_t ecmdDataBuffer::getByteLength() const { return (iv_NumBits + 7) / 8; }
uint32_t ecmdDataBuffer::getCapacity() const { return iv_Capacity; }

uint32_t ecmdDataBuffer::setWordLength(uint32_t i_newNumWords) {
  return setBitLength(i_newNumWords * 32);
}  

uint32_t ecmdDataBuffer::setByteLength(uint32_t i_newNumBytes) {
  return setBitLength(i_newNumBytes * 8);
}  

uint32_t ecmdDataBuffer::setBitLength(uint32_t i_newNumBits) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if ((i_newNumBits == 0) && (iv_NumBits == 0)) {
    // Do Nothing:  this data doesn't already have iv_RealData,iv_Data defined, and it doesn't want to define it
    return rc;
  }

  /* Assign i_newNumBits to iv_NumBits and figure out how many words that is */
  iv_NumBits = i_newNumBits;
  iv_NumWords = (iv_NumBits + 31) / 32;

  /* Now call setCapacity to do all the data buffer resizing and setup */
  rc = setCapacity(iv_NumWords);
  if (rc) return rc;

  return rc;
}  

uint32_t ecmdDataBuffer::setCapacity (uint32_t i_newCapacity) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if(!iv_UserOwned) {
    ETRAC0("**** ERROR (ecmdDataBuffer) : Attempt to modify non user owned buffer size.");
    RETURN_ERROR(ECMD_DBUF_NOT_OWNER);
  }

  /* only resize to make the capacity bigger */
  if (iv_Capacity < i_newCapacity) {
    iv_Capacity = i_newCapacity;

    /* Now setup iv_RealData */
    if (iv_Capacity <= 2) {
      /* We are using iv_LocalData, so point iv_RealData to the start of that */
      iv_RealData = iv_LocalData;
    } else {
      /* If we are going from <= 64 to > 64, there was no malloc done so can't do delete */
      if ((iv_RealData != NULL) && (iv_RealData != iv_LocalData)) {
        delete[] iv_RealData;
      }
      iv_RealData = NULL;

      iv_RealData = new uint32_t[iv_Capacity + EDB_ADMIN_TOTAL_SIZE]; 
      if (iv_RealData == NULL) {
        ETRAC0("**** ERROR : ecmdDataBuffer::setCapacity : Unable to allocate memory for new databuffer");
        RETURN_ERROR(ECMD_DBUF_INIT_FAIL);
      }
    }

    /* Now setup iv_Data to point into the offset inside of iv_RealData */
    iv_Data = iv_RealData + EDB_ADMIN_HEADER_SIZE;

#ifndef REMOVE_SIM
    if (iv_XstateEnabled) {
      if (iv_DataStr != NULL) {
        delete[] iv_DataStr;
      }
      iv_DataStr = NULL;

      iv_DataStr = new char[(iv_Capacity*32)+42];
      if (iv_DataStr == NULL) {
        ETRAC0("**** ERROR : ecmdDataBuffer::setCapacity : Unable to allocate Xstate memory for new databuffer");
        RETURN_ERROR(ECMD_DBUF_INIT_FAIL);
      }
    }
#endif
  }

  /* We are all setup, now init everything to 0 */
  /* We want to do this regardless of if the buffer was resized.  This function is meant to be a destructive operation */
  /* Ok, now setup the header, and tail */
  iv_RealData[EDB_RETURN_CODE] = 0; ///< Reset error code
  iv_RealData[iv_NumWords + EDB_ADMIN_HEADER_SIZE] = EDB_RANDNUM;

  rc = flushTo0();
  if (rc) return rc;

  return rc;
}

uint32_t ecmdDataBuffer::shrinkBitLength(uint32_t i_newNumBits) {

  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (i_newNumBits > iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::shrinkBitLength: New Bit Length (%d) > current NumBits (%d)", i_newNumBits, iv_NumBits);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
    RETURN_ERROR(rc);
  }

  /* If the length is the same, do nothing */
  if (i_newNumBits == iv_NumBits) {
    return rc;
  }

  // before shrinking, clear all data that is going to now be invalid
  this->clearBit(i_newNumBits, (iv_NumBits-i_newNumBits));
  if (rc != ECMD_DBUF_SUCCESS) { 
    ETRAC3("**** ERROR : ecmdDataBuffer::shrinkBitLength: Error Back from clearBit(%d, %d). rc=0x%x", i_newNumBits, (iv_NumBits-i_newNumBits), rc); 
    RETURN_ERROR(rc);  
  }

  uint32_t newNumWords = (i_newNumBits + 31) / 32;

  iv_NumWords = newNumWords;
  iv_NumBits = i_newNumBits;

  /* Ok, now setup the header, and tail */
  iv_RealData[iv_NumWords + EDB_ADMIN_HEADER_SIZE] = EDB_RANDNUM;

  return rc;
}

uint32_t ecmdDataBuffer::growBitLength(uint32_t i_newNumBits) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  uint32_t prevwordsize;
  uint32_t prevbitsize;

  if(!iv_UserOwned) {
    ETRAC0("**** ERROR (ecmdDataBuffer::growBitLength) : Attempt to modify non user owned buffer size.");
    RETURN_ERROR(ECMD_DBUF_NOT_OWNER);
  }

  /* Maybe we don't need to do anything */
  if (iv_NumBits == i_newNumBits) {
    return rc;
  } else if (i_newNumBits < iv_NumBits) {
    /* You can't grow smaller, use shrink */
    ETRAC0("**** ERROR (ecmdDataBuffer::growBitLength) : Attempted to grow to a smaller size then current buffer size.");
    RETURN_ERROR(ECMD_DBUF_INVALID_ARGS);
  }

  /* We need to verify we have room to do this shifting */
  /* Set our new length */
  prevwordsize = iv_NumWords;
  prevbitsize = iv_NumBits;
  iv_NumWords = (i_newNumBits + 31) / 32;
  iv_NumBits = i_newNumBits;
  if (iv_NumWords > iv_Capacity) {
    /* UhOh we are out of room, have to resize */
    uint32_t * tempBuf = new uint32_t[prevwordsize];
    if (tempBuf == NULL) {
      ETRAC0("**** ERROR : ecmdDataBuffer::growBitLength : Unable to allocate temp buffer");
      RETURN_ERROR(ECMD_DBUF_INIT_FAIL);
    }
    memcpy(tempBuf, iv_Data, prevwordsize * 4);

#ifndef REMOVE_SIM
    char* temp = NULL;
    if (iv_XstateEnabled) {
      temp = new char[prevbitsize+42];
      if (temp == NULL) {
        ETRAC0("**** ERROR : ecmdDataBuffer::growBitLength : Unable to allocate temp X-State buffer");
        RETURN_ERROR(ECMD_DBUF_INIT_FAIL);
      }
      strncpy(temp, iv_DataStr, prevbitsize);
    }
#endif
    /* Now resize with the new capacity */
    rc = setCapacity(iv_NumWords);
    if (rc) { if(tempBuf) delete[] tempBuf; return rc;}

    /* Restore the data */
    ecmdBigEndianMemCopy(iv_Data, tempBuf, (prevbitsize + 7) / 8);
    delete[] tempBuf;

#ifndef REMOVE_SIM
    if (iv_XstateEnabled) {
      strncpy(iv_DataStr, temp, prevbitsize); // copy back into iv_DataStr
      delete[] temp;
    }
#endif

    /* Clear any odd bits in the byte */
    for (uint32_t idx = prevbitsize; (idx < iv_NumBits) && (idx % 8); idx ++) {
      clearBit(idx);
    }

  } else if (prevwordsize < iv_NumWords) {
    /* We didn't have to grow the buffer capacity, but we did move into a new word(s) so clear that data space out */
    for (uint32_t idx = prevwordsize; idx < iv_NumWords; idx++) {
      memset(&iv_Data[idx], 0, 4);  // Clear the word

#ifndef REMOVE_SIM
      if (iv_XstateEnabled) {
        memset(&(iv_DataStr[(idx * 32)]), '0', 32); /* init to 0 */
      }
#endif
    }
  }    

  /* Only reset this stuff if things have changed */
  if (prevwordsize != iv_NumWords) {
    iv_RealData[EDB_RETURN_CODE] = 0;  // error state
    iv_RealData[iv_NumWords + EDB_ADMIN_HEADER_SIZE] = EDB_RANDNUM;
  }

  return rc;
}

bool ecmdDataBuffer::getBit(uint32_t i_bit) const {
  /* This just calls is bit set and returns that value */
  return this->isBitSet(i_bit);
}


uint32_t ecmdDataBuffer::setBit(uint32_t i_bit) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (i_bit >= iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::setBit: bit %d >= NumBits (%d)", i_bit, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }

  int index = i_bit/32;
  iv_Data[index] |= 0x00000001 << (31 - (i_bit-(index * 32)));
#ifndef REMOVE_SIM
  if (iv_XstateEnabled) {
    iv_DataStr[i_bit] = '1';
  }
#endif

  return rc;
}

uint32_t ecmdDataBuffer::setBit(uint32_t i_bit, uint32_t i_len) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (i_bit+i_len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::setBit: bit %d + len %d > NumBits (%d)", i_bit, i_len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }

  for (uint32_t idx = 0; idx < i_len; idx ++) {
    rc |= this->setBit(i_bit + idx);
  }    

  return rc;
}


uint32_t ecmdDataBuffer::writeBit(uint32_t i_bit, uint32_t i_value) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (i_value) rc = setBit(i_bit);
  else rc = clearBit(i_bit);
  return rc;
}




uint32_t ecmdDataBuffer::setWord(uint32_t i_wordOffset, uint32_t i_value) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (i_wordOffset >= iv_NumWords) {
    ETRAC2("**** ERROR : ecmdDataBuffer::setWord: wordoffset %d >= NumWords (%d)", i_wordOffset, iv_NumWords);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }

  // Create mask if part of this word is not in the valid part of the ecmdDataBuffer 
  if (((i_wordOffset + 1) == iv_NumWords) && (iv_NumBits % 32)) {
    /* Create my mask */
    uint32_t bitMask = 0xFFFFFFFF;
    /* Shift it left by the amount of unused bits */
    bitMask <<= ((32 * iv_NumWords) - iv_NumBits);
    /* Clear the unused bits */
    i_value &= bitMask;
  }

  iv_Data[i_wordOffset] = i_value;

#ifndef REMOVE_SIM
  if (iv_XstateEnabled) {
    uint32_t startBit = i_wordOffset * 32;
    uint32_t mask = 0x80000000;
    for (int i = 0; i < 32; i++) {
      if (i_value & mask) {
        iv_DataStr[startBit+i] = '1';
      }
      else {
        iv_DataStr[startBit+i] = '0';
      }

      mask >>= 1;
    }
  }
#endif

  return rc;
}

uint32_t ecmdDataBuffer::setByte(uint32_t i_byteOffset, uint8_t i_value) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (i_byteOffset >= getByteLength()) {
    ETRAC2("**** ERROR : ecmdDataBuffer::setByte: byteOffset %d >= NumBytes (%d)", i_byteOffset, getByteLength());
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }

  // Create mask if part of this byte is not in the valid part of the ecmdDataBuffer 
  if (((i_byteOffset + 1) == getByteLength()) && (iv_NumBits % 8)) {
    /* Create my mask */
    uint8_t bitMask = 0xFF;
    /* Shift it left by the amount of unused bits */
    bitMask <<= ((8 * getByteLength()) - iv_NumBits);
    /* Clear the unused bits */
    i_value &= bitMask;
  }

#if defined (i386)
  ((uint8_t*)(this->iv_Data))[i_byteOffset^3] = i_value;
#else
  ((uint8_t*)(this->iv_Data))[i_byteOffset] = i_value;
#endif

#ifndef REMOVE_SIM
  if (iv_XstateEnabled) {
    uint32_t startBit = i_byteOffset * 8;
    uint8_t mask = 0x80;
    for (int i = 0; i < 8; i++) {
      if (i_value & mask) {
        iv_DataStr[startBit+i] = '1';
      }
      else {
        iv_DataStr[startBit+i] = '0';
      }

      mask >>= 1;
    }
  }
#endif

  return rc;
}

uint8_t ecmdDataBuffer::getByte(uint32_t i_byteOffset) const {
  if (i_byteOffset >= getByteLength()) {
    ETRAC2("**** ERROR : ecmdDataBuffer::getByte: byteOffset %d >= NumBytes (%d)", i_byteOffset, getByteLength());
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return 0;
  }
#if defined (i386)
  return ((uint8_t*)(this->iv_Data))[i_byteOffset^3];
#else
  return ((uint8_t*)(this->iv_Data))[i_byteOffset];
#endif
}


uint32_t ecmdDataBuffer::setHalfWord(uint32_t i_halfwordoffset, uint16_t i_value) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (i_halfwordoffset >= ((getByteLength()+1)/2)) {
    ETRAC2("**** ERROR : ecmdDataBuffer::setHalfWord: halfWordOffset %d >= NumHalfWords (%d)", i_halfwordoffset, ((getByteLength()+1)/2));
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }

  // Create mask if part of this byte is not in the valid part of the ecmdDataBuffer 
  if (((i_halfwordoffset + 1) == ((getByteLength()+1)/2)) && (iv_NumBits % 16)) {
    /* Create my mask */
    uint16_t bitMask = 0xFFFF;
    /* Shift it left by the amount of unused bits */
    bitMask <<= ((16 * ((getByteLength()+1)/2)) - iv_NumBits);
    /* Clear the unused bits */
    i_value &= bitMask;
  }

  uint32_t value32 = (uint32_t)i_value;
  if (i_halfwordoffset % 2) {
    iv_Data[i_halfwordoffset/2] = (iv_Data[i_halfwordoffset/2] & 0xFFFF0000) | (value32 & 0x0000FFFF);
  } else {
    iv_Data[i_halfwordoffset/2] = (iv_Data[i_halfwordoffset/2] & 0x0000FFFF) | ((value32 << 16) & 0xFFFF0000);
  }

#ifndef REMOVE_SIM
  if (iv_XstateEnabled) {
    uint32_t startBit = i_halfwordoffset * 16;
    uint16_t mask = 0x8000;
    for (int i = 0; i < 16; i++) {
      if (i_value & mask) {
        iv_DataStr[startBit+i] = '1';
      }
      else {
        iv_DataStr[startBit+i] = '0';
      }
      
      mask >>= 1;
    }
  }
#endif
  return rc;
}
 

uint16_t ecmdDataBuffer::getHalfWord(uint32_t i_halfwordoffset) const {
  if (i_halfwordoffset >= ((getByteLength()+1)/2)) {
    ETRAC2("**** ERROR : ecmdDataBuffer::getHalfWord: halfWordOffset %d >= NumHalfWords (%d)", i_halfwordoffset, ((getByteLength()+1)/2));
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return 0;
  }
  uint16_t ret;
  if (i_halfwordoffset % 2) 
    ret = (uint16_t)(iv_Data[i_halfwordoffset/2] & 0x0000FFFF);
  else
    ret = (uint16_t)(iv_Data[i_halfwordoffset/2] >> 16);
  return ret;
}

uint32_t ecmdDataBuffer::setDoubleWord(uint32_t i_doublewordoffset, uint64_t i_value) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (i_doublewordoffset >= ((getWordLength()+1)/2)) {
    ETRAC2("**** ERROR : ecmdDataBuffer::setDoubleWord: doubleWordOffset %d >= NumDoubleWords (%d)"
	   , i_doublewordoffset, ((getWordLength()+1)/2));
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }

  // Create mask if part of this byte is not in the valid part of the ecmdDataBuffer 
  if (((i_doublewordoffset + 1) == ((getWordLength()+1)/2)) && (iv_NumBits % 64)) {
    /* Create my mask */
#ifdef _LP64
    uint64_t bitMask = 0xFFFFFFFFFFFFFFFFul;
#else
    uint64_t bitMask = 0xFFFFFFFFFFFFFFFFull;
#endif
    /* Shift it left by the amount of unused bits */
    bitMask <<= ((64 * ((getWordLength()+1)/2)) - iv_NumBits);
    /* Clear the unused bits */
    i_value &= bitMask;
  }

#ifdef _LP64
  uint32_t hivalue = (uint32_t)((i_value & 0xFFFFFFFF00000000ul) >> 32);
  uint32_t lovalue = (uint32_t)((i_value & 0x00000000FFFFFFFFul));
#else
  uint32_t hivalue = (uint32_t)((i_value & 0xFFFFFFFF00000000ull) >> 32);
  uint32_t lovalue = (uint32_t)((i_value & 0x00000000FFFFFFFFull));
#endif

  iv_Data[i_doublewordoffset*2] = hivalue;
  /* Don't set the second word if we are on oddwords */
  if (!((i_doublewordoffset*2)+1 >= getWordLength()) ) {
    iv_Data[(i_doublewordoffset*2)+1] = lovalue;
  }

  
#ifndef REMOVE_SIM
  if (iv_XstateEnabled) {
    uint32_t startBit = i_doublewordoffset * 64;
    int bits = 64;
    if (!((i_doublewordoffset*2)+1 >= getWordLength()) ) {
      bits = 32;
    }
#ifdef _LP64
    uint64_t mask = 0x8000000000000000ul;
#else
    uint64_t mask = 0x8000000000000000ull;
#endif
    for (int i = 0; i < bits; i++) {
      if (i_value & mask) {
        iv_DataStr[startBit+i] = '1';
      }
      else {
        iv_DataStr[startBit+i] = '0';
      }

      mask >>= 1;
    }
  }
#endif
  return rc;
}

uint64_t ecmdDataBuffer::getDoubleWord(uint32_t i_doublewordoffset) const {
  // Round up to the next word and check length
  if (i_doublewordoffset >= ((getWordLength()+1)/2)) {
    ETRAC2("**** ERROR : ecmdDataBuffer::getDoubleWord: doubleWordOffset %d >= NumDoubleWords (%d)", 
	   i_doublewordoffset, ((getWordLength()+1)/2));
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return 0;
  }
  uint64_t ret;
  ret = ((uint64_t)(iv_Data[i_doublewordoffset*2])) << 32;
  // If we have an odd length of words we can't pull the second word if we are at the end
  if (!((i_doublewordoffset*2)+1 >= getWordLength()) ) {
    ret |= iv_Data[(i_doublewordoffset*2)+1];
  }
  return ret;
}

uint32_t ecmdDataBuffer::clearBit(uint32_t i_bit) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (i_bit >= iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::clearBit: bit %d >= NumBits (%d)", i_bit, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }

  int index = i_bit/32;
  iv_Data[index] &= ~(0x00000001 << (31 - (i_bit-(index * 32))));
#ifndef REMOVE_SIM
  if (iv_XstateEnabled) {
    iv_DataStr[i_bit] = '0';
  }
#endif

  return rc;
}

uint32_t ecmdDataBuffer::clearBit(uint32_t i_bit, uint32_t i_len) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (i_bit+i_len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::clearBit: bit %d + len %d > NumBits (%d)", i_bit, i_len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }
  
  for (uint32_t idx = 0; idx < i_len; idx ++) {
    rc |= this->clearBit(i_bit + idx);
  }   
  
  return rc;
}

uint32_t ecmdDataBuffer::flipBit(uint32_t i_bit) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (i_bit >= iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::flipBit: bit %d >= NumBits (%d)", i_bit, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }
#ifndef REMOVE_SIM
  if ((iv_XstateEnabled) && this->hasXstate(i_bit, 1)) {
    ETRAC1("**** ERROR : ecmdDataBuffer::flipBit: cannot flip non-binary data at bit %d", i_bit);
    RETURN_ERROR(ECMD_DBUF_XSTATE_ERROR);
  }
#endif

  if (this->isBitSet(i_bit)) {
    rc = this->clearBit(i_bit);      
  } else {
    rc = this->setBit(i_bit);
  }

  return rc;
}

uint32_t ecmdDataBuffer::flipBit(uint32_t i_bit, uint32_t i_len) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (i_bit+i_len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::flipBit: i_bit %d + i_len %d > NumBits (%d)", i_bit, i_len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }

  for (uint32_t i = 0; i < i_len; i++) {
    this->flipBit(i_bit+i);
  }

  return rc;
}

bool   ecmdDataBuffer::isBitSet(uint32_t i_bit) const {
  if (i_bit >= iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::isBitSet: i_bit %d >= NumBits (%d)", i_bit, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return false;
  }

#ifndef REMOVE_SIM
  if (iv_XstateEnabled) {
    if (iv_DataStr[i_bit] != '1' && iv_DataStr[i_bit] != '0') {
      ETRAC1("**** ERROR : ecmdDataBuffer::isBitSet: non-binary character detected in data at bit %d", i_bit);
      SET_ERROR(ECMD_DBUF_XSTATE_ERROR);
      return false;
    }
  }
#endif

  uint32_t index = i_bit/32;
  return (iv_Data[index] & 0x00000001 << (31 - (i_bit-(index * 32)))); 
}

bool   ecmdDataBuffer::isBitSet(uint32_t i_bit, uint32_t i_len) const {
  if (i_bit+i_len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::isBitSet: i_bit %d + i_len %d > NumBits (%d)", i_bit, i_len, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return false;
  }

  bool rc = true;
  for (uint32_t i = 0; i < i_len; i++) {
    if (!this->isBitSet(i_bit + i)) {
      rc = false;
      break;
    }
  }
  return rc;
}

bool   ecmdDataBuffer::isBitClear(uint32_t i_bit) const {
  if (i_bit >= iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::isBitClear: i_bit %d >= NumBits (%d)", i_bit, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return false;
  }

#ifndef REMOVE_SIM
  if (iv_XstateEnabled) {
    if (iv_DataStr[i_bit] != '1' && iv_DataStr[i_bit] != '0') {
      ETRAC0( "**** ERROR : ecmdDataBuffer::isBitClear: non-binary character detected in data string");
      SET_ERROR(ECMD_DBUF_XSTATE_ERROR);
      return false;
    }
  }
#endif

  uint32_t index = i_bit/32;
  return (!(iv_Data[index] & 0x00000001 << (31 - (i_bit-(index * 32))))); 
}

bool ecmdDataBuffer::isBitClear(uint32_t i_bit, uint32_t i_len) const
{
  if (i_bit+i_len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::isBitClear: i_bit %d + i_len %d > NumBits (%d)", i_bit, i_len, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return false;
  }

  bool rc = true;
  for (uint32_t i = 0; i < i_len; i++) {
    if (!this->isBitClear(i_bit + i)) {
      rc = false;
      break;
    }
  }

  return rc;
}

uint32_t ecmdDataBuffer::getNumBitsSet(uint32_t i_bit, uint32_t i_len) const {
  if (i_bit+i_len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::getNumBitsSet: i_bit %d + i_len %d > NumBits (%d)", i_bit, i_len, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return 0;
  }
  static const uint8_t l_num_bits[] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
  };

  uint32_t count = 0;

  do {
    const uint32_t * p_data = iv_Data + i_bit / UNIT_SZ;
    int32_t slop = i_bit % UNIT_SZ;

    /* "cnt" = largest number of bits to be counted each pass */
    int32_t cnt = MIN(i_len, UNIT_SZ);
    cnt = MIN(cnt, UNIT_SZ - slop);

    uint32_t bits = *p_data;

    /* "slop" = unaligned bits */
    if (slop || cnt < UNIT_SZ)
      bits &= fast_mask32(slop, cnt);

    /* count the set bits in each byte */
    count += l_num_bits[(bits & 0x000000FF) >> 0];
    count += l_num_bits[(bits & 0x0000FF00) >> 8];
    count += l_num_bits[(bits & 0x00FF0000) >> 16];
    count += l_num_bits[(bits & 0xFF000000) >> 24];

    i_bit += cnt;
    i_len -= cnt;
  } while (0 < i_len);

  return count;
}

uint32_t ecmdDataBuffer::shiftRight(uint32_t i_shiftNum) {

  uint32_t rc = ECMD_DBUF_SUCCESS;

  uint32_t thisCarry;
  uint32_t prevCarry = 0x00000000;

  uint32_t i;

  /* If we are going to shift off the end we can just clear everything out */
  if (i_shiftNum >= iv_NumBits) {
    rc = flushTo0();
    return rc;
  }


  // shift iv_Data array
  for (uint32_t iter = 0; iter < i_shiftNum; iter++) {
    for (i = 0; i < iv_NumWords; i++) {

      if (this->iv_Data[i] & 0x00000001) 
        thisCarry = 0x80000000;
      else
        thisCarry = 0x00000000;

      // perform shift
      this->iv_Data[i] >>= 1;
      this->iv_Data[i] &= 0x7fffffff;  // backfill with a zero

      // add carry
      this->iv_Data[i] |= prevCarry;

      // set up for next time
      prevCarry = thisCarry; 
    }
  }

#ifndef REMOVE_SIM
  if (iv_XstateEnabled) {
    // shift char
    char* temp = new char[iv_NumBits+42];
    for (i = 0; i < i_shiftNum+1; i++) temp[i] = '0'; // backfill with zeros
    temp[iv_NumBits] = '\0';
    strncpy(&temp[i_shiftNum], iv_DataStr, iv_NumBits-i_shiftNum);  
    strcpy(iv_DataStr, temp); // copy back into iv_DataStr
    delete[] temp;
  }
#endif
  return rc;
}

uint32_t ecmdDataBuffer::shiftLeft(uint32_t i_shiftNum) {

  uint32_t rc = ECMD_DBUF_SUCCESS;

  uint32_t thisCarry;
  uint32_t prevCarry = 0x00000000;
  uint32_t i;

  /* If we are going to shift off the end we can just clear everything out */
  if (i_shiftNum >= iv_NumBits) {
    rc = flushTo0();
    return rc;
  }

  // shift iv_Data array
  for (uint32_t iter = 0; iter < i_shiftNum; iter++) {
    prevCarry = 0;
    for (i = iv_NumWords-1; i != 0xFFFFFFFF; i--) {

      if (this->iv_Data[i] & 0x80000000) 
        thisCarry = 0x00000001;
      else
        thisCarry = 0x00000000;

      // perform shift
      this->iv_Data[i] <<= 1;
      this->iv_Data[i] &= 0xfffffffe; // backfill with a zero

      // add carry
      this->iv_Data[i] |= prevCarry;

      // set up for next time
      prevCarry = thisCarry; 
    }
  }

#ifndef REMOVE_SIM
  if (iv_XstateEnabled) {
    // shift char
    char* temp = new char[iv_NumBits+42];
    for (uint32_t j = iv_NumBits - i_shiftNum - 1; j < iv_NumBits; j++) temp[j] = '0'; // backfill with zeros
    temp[iv_NumBits] = '\0';
    strncpy(temp, &iv_DataStr[i_shiftNum], iv_NumBits - i_shiftNum);  
    strcpy(iv_DataStr, temp); // copy back into iv_DataStr
    delete[] temp;
  }
#endif
  return rc;

}


uint32_t ecmdDataBuffer::shiftRightAndResize(uint32_t i_shiftNum) {

  uint32_t rc = ECMD_DBUF_SUCCESS;

  uint32_t thisCarry;
  uint32_t prevCarry = 0x00000000;

  uint32_t i, prevlen;

  if(!iv_UserOwned)
  {
      ETRAC0("**** ERROR (ecmdDataBuffer) : Attempt to modify non user owned buffer size.");
      RETURN_ERROR(ECMD_DBUF_NOT_OWNER);
  }

  /* We need to verify we have room to do this shifting */
  /* Set our new length */
  iv_NumWords = (iv_NumBits + i_shiftNum + 31) / 32;
  if (iv_NumWords > iv_Capacity) {
    /* UhOh we are out of room, have to resize */
    prevlen = iv_Capacity;
    uint32_t * tempBuf = new uint32_t[prevlen];
    if (tempBuf == NULL) {
      ETRAC0("**** ERROR : ecmdDataBuffer::shiftRightAndResize : Unable to allocate temp buffer");
      RETURN_ERROR(ECMD_DBUF_INIT_FAIL);
    }
    memcpy(tempBuf, iv_Data, prevlen * 4);

#ifndef REMOVE_SIM
    char* temp = NULL;
    if (iv_XstateEnabled) {
      temp = new char[iv_NumBits+42];
      if (temp == NULL) {
        ETRAC0("**** ERROR : ecmdDataBuffer::shiftRightAndResize : Unable to allocate temp X-State buffer");
	delete[] tempBuf;
        RETURN_ERROR(ECMD_DBUF_INIT_FAIL);
      }
      strncpy(temp, iv_DataStr, iv_NumBits);
    }
#endif
    /* Now resize with the new capacity */
    rc = setCapacity(iv_NumWords);
    if (rc) {
#ifndef REMOVE_SIM
      if (temp != NULL) delete[] temp;
#endif
      delete[] tempBuf;
      return rc;
    }

    /* Restore the data */
    memcpy(iv_Data, tempBuf, prevlen * 4);
    delete[] tempBuf;

#ifndef REMOVE_SIM
    if (iv_XstateEnabled) {
      strncpy(iv_DataStr, temp, iv_NumBits); // copy back into iv_DataStr
      delete[] temp;
    }
#endif
  }

  iv_RealData[iv_NumWords + EDB_ADMIN_HEADER_SIZE] = EDB_RANDNUM;

  // shift iv_Data array
  if (!(i_shiftNum % 32)) {
    /* We will do this faster if we are shifting nice word boundaries */
    uint32_t numwords = i_shiftNum / 32;

    for (uint32_t witer = iv_NumWords - numwords - 1; witer != 0xFFFFFFFF; witer--) {
      iv_Data[witer + numwords] = iv_Data[witer];
    }
    /* Zero out the bottom of the array */
    for (uint32_t w = 0; w < numwords; w ++) iv_Data[w] = 0;

  } else {
    for (uint32_t iter = 0; iter < i_shiftNum; iter++) {
      for (i = 0; i < iv_NumWords; i++) {

        if (this->iv_Data[i] & 0x00000001) 
          thisCarry = 0x80000000;
        else
          thisCarry = 0x00000000;

        // perform shift
        this->iv_Data[i] >>= 1;
        this->iv_Data[i] &= 0x7fffffff;  // backfill with a zero

        // add carry
        this->iv_Data[i] |= prevCarry;

        // set up for next time
        prevCarry = thisCarry; 
      }
    }
  }

  iv_NumBits += i_shiftNum;

#ifndef REMOVE_SIM
  if (iv_XstateEnabled) {
    // shift char
    char* temp = new char[iv_NumBits+42];
    if (temp == NULL) {
      ETRAC0("**** ERROR : ecmdDataBuffer::shiftRightAndResize : Unable to allocate temp X-State buffer");
      RETURN_ERROR(ECMD_DBUF_INIT_FAIL);
    }
    for (i = 0; i < i_shiftNum; i++) temp[i] = '0'; // backfill with zeros
    temp[iv_NumBits] = '\0';
    strncpy(&temp[i_shiftNum], iv_DataStr, iv_NumBits-i_shiftNum);  
    strcpy(iv_DataStr, temp); // copy back into iv_DataStr
    delete[] temp;
  }
#endif
 return rc;
}

uint32_t ecmdDataBuffer::shiftLeftAndResize(uint32_t i_shiftNum) {

  uint32_t rc = ECMD_DBUF_SUCCESS;

  uint32_t thisCarry;
  uint32_t prevCarry = 0x00000000;
  uint32_t i;

  if(!iv_UserOwned)
  {
      ETRAC0("**** ERROR (ecmdDataBuffer) : Attempt to modify non user owned buffer size.");
      RETURN_ERROR(ECMD_DBUF_NOT_OWNER);
  }

  /* If we are going to shift off the end we can just clear everything out */
  if (i_shiftNum >= iv_NumBits) {
    rc = setBitLength(0);
    return rc;
  }

  // shift iv_Data array
  for (uint32_t iter = 0; iter < i_shiftNum; iter++) {
    prevCarry = 0;
    for (i = (iv_NumWords-1); i != 0xFFFFFFFF; i--) {

      if (this->iv_Data[i] & 0x80000000) 
        thisCarry = 0x00000001;
      else
        thisCarry = 0x00000000;

      // perform shift
      this->iv_Data[i] <<= 1;
      this->iv_Data[i] &= 0xfffffffe; // backfill with a zero

      // add carry
      this->iv_Data[i] |= prevCarry;

      // set up for next time
      prevCarry = thisCarry; 
    }
  }

  /* Adjust our lengths based on the shift */
  iv_NumBits -= i_shiftNum;
  iv_NumWords = (iv_NumBits +31) / 32;
  iv_RealData[iv_NumWords + EDB_ADMIN_HEADER_SIZE] = EDB_RANDNUM;

#ifndef REMOVE_SIM
  if (iv_XstateEnabled) {
    // shift char
    char* temp = new char[iv_NumBits+42];
    if (temp == NULL) {
      ETRAC0("**** ERROR : ecmdDataBuffer::shiftLeftAndResize : Unable to allocate temp X-State buffer");
      RETURN_ERROR(ECMD_DBUF_INIT_FAIL);
    }
    temp[iv_NumBits] = '\0';
    strncpy(temp, &iv_DataStr[i_shiftNum], iv_NumBits);  
    strcpy(iv_DataStr, temp); // copy back into iv_DataStr
    delete[] temp;
  }
#endif
  return rc;
}

uint32_t ecmdDataBuffer::rotateRight(uint32_t i_rotateNum) {

  uint32_t rc = ECMD_DBUF_SUCCESS;

  bool lastBitSet;
  // rotate iv_Data
  for (uint32_t iter = 0; iter < i_rotateNum; iter++) {
    lastBitSet = this->isBitSet(iv_NumBits-1);   // save the last bit
    rc = this->shiftRight(1);   // right-shift
    if (rc) break;
    if (lastBitSet)    // insert into beginning
      rc = this->setBit(0); 
    else
      rc = this->clearBit(0);
    if (rc) break;
  }
  return rc;
}

uint32_t ecmdDataBuffer::rotateLeft(uint32_t i_rotateNum) {

  uint32_t rc = ECMD_DBUF_SUCCESS;

  bool firstBitSet;
  // rotate iv_Data
  for (uint32_t iter = 0; iter < i_rotateNum; iter++) {
    firstBitSet = this->isBitSet(0);   // save the first bit
    rc = this->shiftLeft(1);   // left-shift
    if (rc) break;
    if (firstBitSet)   // insert at the end
      rc = this->setBit(iv_NumBits-1);
    else
      rc = this->clearBit(iv_NumBits-1);
    if (rc) break;
  }
  return rc;
}

uint32_t ecmdDataBuffer::flushTo0() {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (iv_NumWords > 0) {
    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */
#ifndef REMOVE_SIM
    if (iv_XstateEnabled)
      rc = this->fillDataStr('0');
#endif
  }
  return rc;
}

uint32_t ecmdDataBuffer::flushTo1() {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (iv_NumWords > 0) {
    for (uint32_t i = 0; i < iv_NumWords; i++) {
      setWord(i, 0xFFFFFFFF);
    }
  }
  return rc;
}

uint32_t ecmdDataBuffer::invert() { 
  uint32_t rc = ECMD_DBUF_SUCCESS;
  rc = this->flipBit(0, iv_NumBits);
  return rc;
}

uint32_t ecmdDataBuffer::reverse() {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  uint32_t l_words=0;
  uint32_t l_slop =  (iv_NumBits % UNIT_SZ);

  if(l_slop)
        l_words = iv_NumBits/32+1;
  else
        l_words = iv_NumBits/32;
    
  // reverse words
  for(uint32_t i=0;i< l_words/2;i++){
      uint32_t l_tmp = fast_reverse32(iv_Data[l_words-1-i]); 
      iv_Data[l_words-1-i] = fast_reverse32(iv_Data[i]);
      iv_Data[i] = l_tmp;
  }

  // if odd number of words, reverse middle word; if only 1 word, reverse this word
  if(l_words&1){
      iv_Data[l_words/2 ] = fast_reverse32(iv_Data[l_words/2]);
  }
    
  // now account for slop
  if (l_slop != 0){

    for(uint32_t i=0;i<l_words;i++){ // loop through all words

      if((l_words>1)&&(i!=(l_words-1))){ // deal with most words here                
          uint32_t mask = 0xffffffff >> (32-l_slop);
          uint32_t tmp1 = (iv_Data[i]& mask)<< (32-l_slop);
          
          mask =~mask;
          uint32_t tmp2 = (iv_Data[i+1] & mask)>>l_slop;

          tmp1 |=tmp2;
          iv_Data[i]=tmp1;

      } else { //dealing with the last word separately; Also, handle if there is only one word here
          uint32_t mask = 0xffffffff >> (32-l_slop);
          iv_Data[l_words-1] = (iv_Data[l_words-1]& mask) <<(32-l_slop); 
      }
    } // end of for loop through all words
  } // end of slop check
   
#ifndef REMOVE_SIM   
  if (iv_XstateEnabled) {
    /* We have xstates, re-generate the binary data */
      strncpy(&(iv_DataStr[0]), (this->genBinStr()).c_str(), iv_NumBits);
  }
#endif


  return rc;
}

uint32_t ecmdDataBuffer::applyInversionMask(const ecmdDataBuffer & i_invMaskBuffer, uint32_t i_invByteLen) {
  return applyInversionMask(i_invMaskBuffer.iv_Data, (i_invMaskBuffer.getByteLength() < i_invByteLen) ? i_invMaskBuffer.getByteLength() : i_invByteLen);
}


uint32_t ecmdDataBuffer::applyInversionMask(const uint32_t * i_invMask, uint32_t i_invByteLen) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  /* Do the smaller of data provided or size of buffer */
  uint32_t wordlen = (i_invByteLen / 4) + 1 < iv_NumWords ? (i_invByteLen / 4) + 1 : iv_NumWords;

  for (uint32_t i = 0; i < wordlen; i++) {
    iv_Data[i] = iv_Data[i] ^ i_invMask[i]; /* Xor */
  }

  /* We need to make sure our last word is clean if numBits isn't on a word boundary */
  if ((wordlen == iv_NumWords) && (iv_NumBits % 32)) {
    /* Reading out the last word and writing it back will clear any bad bits on */
    uint32_t myWord = getWord((wordlen-1));
    rc = setWord((wordlen-1), myWord);
    if (rc) return rc;
  }
     
#ifndef REMOVE_SIM   
  if (iv_XstateEnabled) {
    uint32_t xbuf_size = (i_invByteLen * 8) < iv_NumBits ? (i_invByteLen * 8) : iv_NumBits;
    uint32_t curbit = 0;

    for (uint32_t word = 0; word < wordlen; word ++) {
      for (uint32_t bit = 0; bit < 32; bit ++) {
        
        if (curbit >= xbuf_size) break;

        if (i_invMask[word] & (0x80000000 >> bit)) {
          if (iv_DataStr[curbit] == '0') iv_DataStr[curbit] = '1';
          else if (iv_DataStr[curbit] == '1') iv_DataStr[curbit] = '0';
        }
        curbit ++;
      }
    }
  }
#endif
  return rc;
}

inline uint32_t ecmdFastInsert(uint32_t *i_target,const uint32_t * i_dataIn, uint32_t i_targetStart, uint32_t i_len, uint32_t i_sourceStart) {

     uint32_t rc = ECMD_DBUF_SUCCESS;

     do {
        const uint32_t * p_src = i_dataIn + i_sourceStart / UNIT_SZ;
        uint32_t * p_trg = i_target + i_targetStart / UNIT_SZ;

        // "slop" = unaligned bits 
        int32_t src_slop = i_sourceStart % UNIT_SZ;
        int32_t trg_slop = i_targetStart % UNIT_SZ;
        // "shift" = amount of shifting needed for target alignment 
        int32_t shift = trg_slop - src_slop;

        int32_t cnt = i_len;

            // "cnt" = largest number of bits to be moved each pass 
            cnt = MIN(cnt, UNIT_SZ);
            cnt = MIN(cnt, UNIT_SZ - src_slop);
            cnt = MIN(cnt, UNIT_SZ - trg_slop);

            // generate the source mask only once 
            uint32_t mask = fast_mask32(src_slop, cnt);
            // read the source bits only once 
            uint32_t src_bits = *p_src & mask;

            // ideally (i << -1) would yield (i >> 1), but it
            //   doesn't, so we need an extra branch here 
            if (shift < 0) {
                shift = -shift;
                src_bits <<= shift;
                mask <<= shift;
            } else {
                src_bits >>= shift;
                mask >>= shift;
            }

            // clear source '0' bits in the target 
            *p_trg &= ~mask;
            // set source '1' bits in the target  
            *p_trg |= src_bits;


        i_sourceStart += cnt;
        i_targetStart += cnt;

        i_len -= cnt;
    } while (0 < i_len);

    return rc;  
}


uint32_t ecmdDataBuffer::insert(const ecmdDataBuffer &i_bufferIn, uint32_t i_targetStart, uint32_t i_len, uint32_t i_sourceStart) {

  uint32_t rc = ECMD_DBUF_SUCCESS;    

  if (i_targetStart+i_len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::insert: i_targetStart %d + i_len %d > iv_NumBits (%d)",
           i_targetStart, i_len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else if (i_targetStart >= iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::insert: i_targetStart %d >= iv_NumBits (%d)",
           i_targetStart, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else if (i_len > iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::insert: i_len %d > iv_NumBits (%d)",
           i_len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }


  rc = ecmdFastInsert(this->iv_Data,i_bufferIn.iv_Data, i_targetStart, i_len, i_sourceStart );
  if (rc) return rc;

#ifndef REMOVE_SIM   
  // If the input buffer has xstates enabled, we need to enable ours 
  if (i_bufferIn.iv_XstateEnabled) {
    enableXstateBuffer();
    if (i_targetStart+i_len <= iv_NumBits) {
      strncpy(&(iv_DataStr[i_targetStart]), (i_bufferIn.genXstateStr(i_sourceStart, i_len)).c_str(), i_len);
    }
  } else if (iv_XstateEnabled) {
    // We have xstates, but the incoming buffer didn't, just grab the binary data 
    if (i_targetStart+i_len <= iv_NumBits) {
      strncpy(&(iv_DataStr[i_targetStart]), (i_bufferIn.genBinStr(i_sourceStart, i_len)).c_str(), i_len);
    }
  }      
#endif  

  return rc;      
}

uint32_t ecmdDataBuffer::insert(const uint32_t * i_dataIn, uint32_t i_targetStart, uint32_t i_len, uint32_t i_sourceStart) {      

  uint32_t rc = ECMD_DBUF_SUCCESS;
    
  if (i_targetStart+i_len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::insert: i_targetStart %d + i_len %d > iv_NumBits (%d)", i_targetStart, i_len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else if (i_targetStart >= iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::insert: i_targetStart %d >= iv_NumBits (%d)", i_targetStart, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else if (i_len > iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::insert: i_len %d > iv_NumBits (%d)", i_len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }
    
  rc = ecmdFastInsert(this->iv_Data,i_dataIn, i_targetStart, i_len, i_sourceStart);
  if (rc) return rc;

#ifndef REMOVE_SIM   
  if (iv_XstateEnabled) {
    /* We have xstates, generate the binary data */
    if (i_targetStart+i_len <= iv_NumBits) {
      strncpy(&(iv_DataStr[i_targetStart]), (this->genBinStr(i_sourceStart, i_len)).c_str(), i_len);
    }
  }
#endif

  return rc;
}

uint32_t ecmdDataBuffer::insert(uint32_t i_dataIn, uint32_t i_targetStart, uint32_t i_len, uint32_t i_sourceStart) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if ( i_sourceStart + i_len > 32 ) {
    ETRAC2("**** ERROR : ecmdDataBuffer::insert: i_sourceStart %d + i_len %d > sizeof i_dataIn (32)\n", i_sourceStart, i_len );
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else if (i_sourceStart >= 32) {
    ETRAC1("**** ERROR : ecmdDataBuffer::insert: i_sourceStart %d >= sizeof i_dataIn (32)", i_sourceStart);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else if (i_len > 32) {
    ETRAC1("**** ERROR : ecmdDataBuffer::insert: i_len %d > sizeof i_dataIn (32)", i_len);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }
  // other input checks are perfomred in the insert function called below
  
  rc = this->insert(&i_dataIn, i_targetStart, i_len, i_sourceStart);

  return rc;
}

uint32_t ecmdDataBuffer::insertFromRight(const uint32_t * i_datain, uint32_t i_start, uint32_t i_len) {

  uint32_t rc = ECMD_DBUF_SUCCESS;

  int offset;
  if((i_len % 32) == 0) {
    offset = 0;
  } else {
    offset = 32 - (i_len % 32);
  }  

  if (i_start+i_len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::insertFromRight: start %d + len %d > iv_NumBits (%d)", i_start, i_len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }
  // other input checks happen below in setBit and clearBit
    
  uint32_t mask = 0x80000000 >> offset;
  for (uint32_t i = 0; i < i_len; i++) {
    if (i_datain[(i+offset)/32] & mask) {
      rc = this->setBit(i_start+i);
    }
    else { 
      rc = this->clearBit(i_start+i);
    }

    mask >>= 1;
    if (mask == 0x00000000) {
      mask = 0x80000000;
    }
    if (rc) break;
  }

  return rc;
}

uint32_t ecmdDataBuffer::insertFromRight(uint32_t i_datain, uint32_t i_start, uint32_t i_len) {

  if (i_len > 32) {
    ETRAC1("**** ERROR : ecmdDataBuffer::insertFromRight: i_len %d > sizeof i_dataIn (32)", i_len);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
// other input checks are perfomred in the insertFromRight function called below
  }

    return this->insertFromRight(&i_datain, i_start, i_len);
}

uint32_t ecmdDataBuffer::insert(const uint8_t *i_dataIn, uint32_t i_targetStart, uint32_t i_len, uint32_t i_sourceStart) {

  uint32_t rc = ECMD_DBUF_SUCCESS;


  if (i_targetStart+i_len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::insert: i_targetStart %d + i_len %d > iv_NumBits (%d)", i_targetStart, i_len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
// other input checks are perfomred in the setBit(), clearBit() functions called below
  } else {

    const uint8_t * sourcePtr = i_dataIn;

    for (uint32_t i=0; i< i_len; i++) {

      int index = (i+i_sourceStart)/8;
      if (sourcePtr[index]& 0x00000001 << (7-(i+i_sourceStart -(index*8)))) {
        rc = this->setBit(i_targetStart + i );
      }else {
        rc= this->clearBit(i_targetStart + i );
      }
      if (rc) break;
    }
  }
  return rc;
}

uint32_t ecmdDataBuffer::insertFromRight(const uint8_t *i_datain, uint32_t i_start, uint32_t i_len) {

 
  uint32_t rc = ECMD_DBUF_SUCCESS;

  int offset;
  if((i_len % 8) == 0) {
    offset = 0;
  } else {
    offset = 8 - (i_len % 8);
  }  

  if (i_start+i_len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::insertFromRight: start %d + len %d > iv_NumBits (%d)", i_start, i_len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }
  // other input checks happen below in setBit and clearBit
    
  uint8_t mask = 0x80 >> offset;
  for (uint32_t i = 0; i < i_len; i++) {
    if (i_datain[(i+offset)/8] & mask) {
      rc = this->setBit(i_start+i);
    }
    else { 
      rc = this->clearBit(i_start+i);
    }

    mask >>= 1;
    if (mask == 0x00) {
      mask = 0x80;
    }
    if (rc) break;
  }

  return rc;

 }



uint32_t ecmdDataBuffer::extract(ecmdDataBuffer& o_bufferOut, uint32_t i_start, uint32_t i_len) const {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  // ecmdExtract can't make good input checks, so we have to do that here
  if (i_start + i_len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::extract: start %d + len %d > iv_NumBits (%d)\n", i_start, i_len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else if (i_start >= iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::extract: start %d >= iv_NumBits (%d)\n", i_start, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else if (i_len > iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::extract: len %d > iv_NumBits (%d)\n", i_len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }

  rc = o_bufferOut.setBitLength(i_len);
  if (rc) return rc;

  rc = ecmdExtract(this->iv_Data, i_start, i_len, o_bufferOut.iv_Data);
  if (rc) return rc;   

#ifndef REMOVE_SIM
  /* We have xstates, force the output buffer to have them as well */
  if (iv_XstateEnabled) {
    o_bufferOut.enableXstateBuffer();
    if (i_start+i_len <= iv_NumBits) {
      strncpy(o_bufferOut.iv_DataStr, (genXstateStr(i_start, i_len)).c_str(), i_len);
      o_bufferOut.iv_DataStr[i_len] = '\0';
    }
    /* Bufferout has xstates but we don't we still need to apply the binary data to it */
  } else if (o_bufferOut.iv_XstateEnabled) {
    if (i_start+i_len <= iv_NumBits) {
      strncpy(o_bufferOut.iv_DataStr, (genBinStr(i_start, i_len)).c_str(), i_len);
      o_bufferOut.iv_DataStr[i_len] = '\0';
    }
  }      
#endif

  return rc;
}




uint32_t ecmdDataBuffer::extract(uint32_t *o_dataOut, uint32_t i_start, uint32_t i_len) const {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  // ecmdExtract can't make good input checks, so we have to do that here
  if (i_start + i_len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::extract: i_start %d + i_len %d > iv_NumBits (%d)\n", i_start, i_len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else if (i_start >= iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::extract: i_start %d >= iv_NumBits (%d)", i_start, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else if (i_len > iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::extract: i_len %d > iv_NumBits (%d)", i_len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else if (i_len == 0) {
    return ECMD_DBUF_SUCCESS;
  }

  rc = ecmdExtract(this->iv_Data, i_start, i_len, o_dataOut);
  if (rc) {
    RETURN_ERROR(rc);
  }

#ifndef REMOVE_SIM
  if (iv_XstateEnabled) {
    /* If we are using this interface and find Xstate data we have a problem */
    if (hasXstate(i_start, i_len)) {
      ETRAC0("**** WARNING : ecmdDataBuffer::extract: Cannot extract when non-binary (X-State) character      present\n");
      RETURN_ERROR(ECMD_DBUF_XSTATE_ERROR);
    }
  }
#endif
  return rc;
}

uint32_t ecmdDataBuffer::extract(uint8_t * o_data, uint32_t i_start, uint32_t i_bitLen) const {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  // Error checking
  if (i_start + i_bitLen > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::extract: i_start %d + i_bitLen %d > iv_NumBits (%d)\n", i_start, i_bitLen, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else if (i_start >= iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::extract: i_start %d >= iv_NumBits (%d)", i_start, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else if (i_bitLen > iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::extract: i_bitLen %d > iv_NumBits (%d)", i_bitLen, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else if (i_bitLen == 0) {
    return ECMD_DBUF_SUCCESS;
  }

  // Put the users data into a temporary buffer, which will align it on byte boundaries.
  // Then just loop over the extractBuffer, placing it byte by byte into o_data
  ecmdDataBuffer extractBuffer(i_bitLen);
  rc = extractBuffer.insert(*this, 0, i_bitLen, i_start);
  if (rc) {
    RETURN_ERROR(rc);
  }

  // Now do a byte loop, setting the data in o_data
  int numBytes = extractBuffer.getByteLength();
  for (int i = 0; i < numBytes; i++) {
    o_data[i] = extractBuffer.getByte(i);
  }

#ifndef REMOVE_SIM
  if (iv_XstateEnabled) {
    /* If we are using this interface and find Xstate data we have a problem */
    if (hasXstate(i_start, i_bitLen)) {
      ETRAC0("**** WARNING : ecmdDataBuffer::extract: Cannot extract when non-binary (X-State) character      present\n");
      RETURN_ERROR(ECMD_DBUF_XSTATE_ERROR);
    }
  }
#endif

  return rc;
}


// extractPreserve() takes data from current and inserts it in the passed in
//  buffer at a given offset. This is the same as insert() with the args and
//  the data flow reversed, so insert() is called to do the work
uint32_t ecmdDataBuffer::extractPreserve(ecmdDataBuffer & o_bufferOut, uint32_t i_start, uint32_t i_len, uint32_t i_targetStart) const {
// input checks done in the insert function
  return o_bufferOut.insert( *this, i_targetStart, i_len, i_start );
}

// extractPreserve() with a generic data buffer is hard to work on, so the
// output buffer is first copied into an ecmdDataBuffer object, then insert()
// is called to do the work
uint32_t ecmdDataBuffer::extractPreserve(uint32_t *o_outBuffer, uint32_t i_start, uint32_t i_len, uint32_t i_targetStart) const {
  
  uint32_t rc = ECMD_DBUF_SUCCESS;
// input checks done in the insert function

  const uint32_t numWords = ( i_targetStart + i_len + 31 ) / 32;
  if ( numWords == 0 ) return rc;

  ecmdDataBuffer *tempBuf = new ecmdDataBuffer;

  if ( NULL == tempBuf ) {
      ETRAC0("**** ERROR : ecmdDataBuffer::extractPreserve : Unable to allocate memory for new databuffer\n");
      RETURN_ERROR(ECMD_DBUF_INIT_FAIL);
  } 

  rc = tempBuf->setWordLength( numWords );

  if ( rc == ECMD_DBUF_SUCCESS ) 
    rc = tempBuf->memCopyIn( o_outBuffer, numWords * 4);

  if ( rc == ECMD_DBUF_SUCCESS ) 
    rc = tempBuf->insert( *this, i_targetStart, i_len, i_start);

  if ( rc == ECMD_DBUF_SUCCESS ) 
    rc = tempBuf->memCopyOut( o_outBuffer, numWords * 4);

  delete tempBuf;
  return rc;

}

uint32_t ecmdDataBuffer::extractPreserve(uint8_t * o_data, uint32_t i_start, uint32_t i_len, uint32_t i_targetStart) const {


  uint32_t rc = ECMD_DBUF_SUCCESS;
// input checks done in the insert function

  //const uint32_t numWords = ( i_targetStart + i_len + 31 ) / 32;
  const uint32_t numBytes = ( i_targetStart + i_len + 7 ) / 8;
  if ( numBytes == 0 ) return rc;

  ecmdDataBuffer *tempBuf = new ecmdDataBuffer;

  if ( NULL == tempBuf ) {
      ETRAC0("**** ERROR : ecmdDataBuffer::extractPreserve : Unable to allocate memory for new databuffer\n");
      RETURN_ERROR(ECMD_DBUF_INIT_FAIL);
  } 

  rc = tempBuf->setByteLength( numBytes );

  if ( rc == ECMD_DBUF_SUCCESS ) 
    rc = tempBuf->memCopyIn( o_data, numBytes);

  if ( rc == ECMD_DBUF_SUCCESS ) 
    rc = tempBuf->insert( *this, i_targetStart, i_len, i_start);

  if ( rc == ECMD_DBUF_SUCCESS ) 
    rc = tempBuf->memCopyOut( o_data, numBytes);

  delete tempBuf;
  return rc;
} 

uint32_t ecmdDataBuffer::extractToRight(ecmdDataBuffer & o_bufferOut, uint32_t i_start, uint32_t i_len) const {
  uint32_t rc = ECMD_DBUF_SUCCESS;

// input checks done in the extract function
  rc = this->extract(o_bufferOut, i_start, i_len);
  if (rc) return rc;

  if (i_len < 32)
    rc = o_bufferOut.shiftRightAndResize(32 - i_len);
  return rc;
}

uint32_t ecmdDataBuffer::extractToRight(uint32_t * o_data, uint32_t i_start, uint32_t i_len) const {
  uint32_t rc = ECMD_DBUF_SUCCESS;

// input checks done in the extract function
  rc = this->extract(o_data, i_start, i_len);

  if (i_len < 32)
    *o_data >>= (32 - i_len);
  return rc;
}

uint32_t ecmdDataBuffer::extractToRight(uint8_t * o_data, uint32_t i_start, uint32_t i_len) const {

  uint32_t rc = ECMD_DBUF_SUCCESS;
// input checks done in the extract function
  rc = this->extract(o_data, i_start, i_len);

  if (i_len < 8)
    *o_data >>= (8 - i_len);
  return rc;
} 

uint32_t ecmdDataBuffer::concat(const ecmdDataBuffer & i_buf0,
                            const ecmdDataBuffer & i_buf1) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  rc = this->setBitLength(i_buf0.iv_NumBits + i_buf1.iv_NumBits); if (rc) return rc;
  rc = this->insert(i_buf0, 0, i_buf0.iv_NumBits); if (rc) return rc;
  rc = this->insert(i_buf1, i_buf0.iv_NumBits, i_buf1.iv_NumBits);
  return rc;
}

uint32_t ecmdDataBuffer::concat(const ecmdDataBuffer & i_buf0,
                            const ecmdDataBuffer & i_buf1,
                            const ecmdDataBuffer & i_buf2) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  rc = this->setBitLength(i_buf0.iv_NumBits + i_buf1.iv_NumBits + i_buf2.iv_NumBits); if (rc) return rc;
  rc = this->insert(i_buf0, 0, i_buf0.iv_NumBits); if (rc) return rc;
  rc = this->insert(i_buf1, i_buf0.iv_NumBits, i_buf1.iv_NumBits); if (rc) return rc;
  rc = this->insert(i_buf2, i_buf0.iv_NumBits + i_buf1.iv_NumBits, i_buf2.iv_NumBits);
  return rc;
}

uint32_t ecmdDataBuffer::setOr(const ecmdDataBuffer& i_bufferIn, uint32_t i_startBit, uint32_t i_len) {
#ifndef REMOVE_SIM
  if (i_bufferIn.iv_XstateEnabled) enableXstateBuffer();
#endif
  if (i_len > i_bufferIn.iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::setOr: len %d > NumBits of incoming buffer (%d)", i_len, i_bufferIn.iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }
  return this->setOr(i_bufferIn.iv_Data, i_startBit, i_len);
}

uint32_t ecmdDataBuffer::setOr(const uint32_t * i_dataIn, uint32_t i_startBit, uint32_t i_len) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (i_startBit + i_len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::setOr: bit %d + len %d > NumBits (%d)", i_startBit, i_len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }
  // other input checks done as part of setBit()

  uint32_t mask = 0x80000000;
  for (uint32_t i = 0; i < i_len; i++) {
    if (i_dataIn[i/32] & mask) {
      rc = this->setBit(i_startBit + i);
    }
    mask >>= 1;
    if (mask == 0x00000000) {
      mask = 0x80000000;
    }
    if (rc) break;
  }

  return rc;
}

uint32_t ecmdDataBuffer::setOr(uint32_t i_dataIn, uint32_t i_startBit, uint32_t i_len) {
// input checks done as part of setOr()
  return this->setOr(&i_dataIn, i_startBit, i_len);
}

uint32_t ecmdDataBuffer::setXor(const ecmdDataBuffer& i_bufferIn, uint32_t i_startBit, uint32_t i_len) {
#ifndef REMOVE_SIM
  if (i_bufferIn.iv_XstateEnabled) enableXstateBuffer();
#endif
  if (i_len > i_bufferIn.iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::setXor: len %d > NumBits of incoming buffer (%d)", i_len, i_bufferIn.iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }
// other input checks done as part of setXor()
  return this->setXor(i_bufferIn.iv_Data, i_startBit, i_len);
}

uint32_t ecmdDataBuffer::setXor(const uint32_t * i_dataIn, uint32_t i_startBit, uint32_t i_len) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (i_startBit + i_len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::setOr: bit %d + len %d > NumBits (%d)", i_startBit, i_len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }
  // other input checks done as part of writeBit()

  uint32_t mask = 0x80000000;
  for (uint32_t i = 0; i < i_len; i++) {
    rc = this->writeBit(i_startBit + i, ((i_dataIn[i/32] & mask) ^ (this->iv_Data[i/32] & mask)));
    mask >>= 1;
    if (mask == 0x00000000) {
      mask = 0x80000000;
    }
    if (rc) break;
  }

  return rc;
}

uint32_t ecmdDataBuffer::setXor(uint32_t i_dataIn, uint32_t i_startBit, uint32_t i_len) {
// input checks done as part of setXor()
  return this->setXor(&i_dataIn, i_startBit, i_len);
}

uint32_t ecmdDataBuffer::merge(const ecmdDataBuffer& i_bufferIn) {
#ifndef REMOVE_SIM
  if (i_bufferIn.iv_XstateEnabled) enableXstateBuffer();
#endif
  if (iv_NumBits != i_bufferIn.iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::merge: NumBits in (%d) do not match NumBits (%d)", i_bufferIn.iv_NumBits, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    return this->setOr(i_bufferIn, 0, iv_NumBits);
  }
}

uint32_t ecmdDataBuffer::setAnd(const ecmdDataBuffer& i_bufferIn, uint32_t i_startBit, uint32_t i_len) {
#ifndef REMOVE_SIM
  if (i_bufferIn.iv_XstateEnabled) enableXstateBuffer();
#endif
  if (i_len > i_bufferIn.iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::setAnd: len %d > NumBits of incoming buffer (%d)", i_len, i_bufferIn.iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }
// other input checks done as part of setAnd()
  return this->setAnd(i_bufferIn.iv_Data, i_startBit, i_len);
}

uint32_t ecmdDataBuffer::setAnd(const uint32_t * i_dataIn, uint32_t i_startBit, uint32_t i_len) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (i_startBit + i_len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::setAnd: i_start %d + i_len %d > iv_NumBits (%d)", i_startBit, i_len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
// other input checks done as part of setClearBit()
  } else {
    uint32_t mask = 0x80000000;
    for (uint32_t i = 0; i < i_len; i++) {
      if (!(i_dataIn[i/32] & mask)) {
        this->clearBit(i_startBit + i);
      }
      mask >>= 1;
      if (mask == 0x00000000) {
        mask = 0x80000000;
      }
      if (rc) break;
    }
  }
  return rc;
}

uint32_t ecmdDataBuffer::setAnd(uint32_t i_dataIn, uint32_t i_startBit, uint32_t i_len) {
// input checks done as part of setAnd()
  return this->setAnd(&i_dataIn, i_startBit, i_len);
}

uint32_t ecmdDataBuffer::oddParity(uint32_t i_start, uint32_t i_stop) const {

  int charOffset;
  int posOffset;
  uint32_t counter;
  int parity = 1;
  uint32_t mask;

  if (i_start >= iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::oddParity: i_start %d >= iv_NumBits (%d)\n", i_start, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else if (i_stop >= iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::oddParity: i_stop %d >= iv_NumBits (%d)\n", i_stop, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else if (i_start > i_stop) {
    ETRAC2("**** ERROR : ecmdDataBuffer::oddParity: i_start %d >= i_stop (%d)\n", i_start, i_stop);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {

    charOffset = i_start / 32;
    posOffset = i_start - charOffset * 32;
    mask = 0x80000000 >> posOffset;

    for (counter = 0; counter < (i_stop - i_start + 1); counter++) {
      if (mask & iv_Data[charOffset]) {
        parity ^= 1;
      }
      posOffset++;
      mask >>= 1;
      if (posOffset > 31) {
        charOffset++;
        posOffset = 0;
        mask = 0x80000000;
      }
    }

  }

  return parity;

}

uint32_t ecmdDataBuffer::evenParity(uint32_t i_start, uint32_t i_stop) const {
// input checks done as part of oddParity()
  if (this->oddParity(i_start, i_stop))
    return 0;
  else
    return 1;
}

uint32_t ecmdDataBuffer::oddParity(uint32_t i_start, uint32_t i_stop, uint32_t i_insertPos) {
// input checks done as part of oddParity()
  if (this->oddParity(i_start,i_stop))
    this->setBit(i_insertPos);
  else 
    this->clearBit(i_insertPos);
  return 0;
}

uint32_t ecmdDataBuffer::evenParity(uint32_t i_start, uint32_t i_stop, uint32_t i_insertPos) {
// input checks done as part of evenParity()...which calls oddParity
  if (this->evenParity(i_start,i_stop))
    this->setBit(i_insertPos);
  else
    this->clearBit(i_insertPos);
  return 0;
}

uint32_t ecmdDataBuffer::getWord(uint32_t i_wordOffset) const {
  if (i_wordOffset >= iv_NumWords) {
    ETRAC2("**** ERROR : ecmdDataBuffer::getWord: i_wordOffset %d >= NumWords (%d)", i_wordOffset, iv_NumWords);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return 0;
  }
  return this->iv_Data[i_wordOffset];
}


std::string ecmdDataBuffer::genHexLeftStr(uint32_t i_start, uint32_t i_bitLen) const {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  int tempNumWords = (i_bitLen + 31) / 32;
  std::string ret;
  char cPtr[10];

  if (i_start+i_bitLen > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::genHexLeftStr: i_start %d + i_len %d >= NumBits (%d)", i_start, i_bitLen, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return ret;
  } else if (i_start >= iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::genHexLeftStr: i_start %d >= iv_NumBits (%d)", i_start, iv_NumBits);
    return ret;
  } else if (i_bitLen > iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::genHexLeftStr: i_bitLen %d > iv_NumBits (%d)", i_bitLen, iv_NumBits);
    return ret;
  }

  if ((i_start==0) && (i_bitLen==iv_NumBits)){
    for (int w = 0; w < tempNumWords; w++) {      
      sprintf(cPtr, "%.8X", iv_Data[w]);
      ret.append(cPtr);
    }
  } else {
    ecmdDataBuffer tmpBuffer(tempNumWords*32);
    tmpBuffer.flushTo0();

    rc = ecmdFastInsert(tmpBuffer.iv_Data,this->iv_Data,0,i_bitLen, i_start);
    if (rc) return ret;

    for (int w = 0; w < tempNumWords; w++) {
      sprintf(cPtr, "%.8X", tmpBuffer.iv_Data[w]);
      ret.append(cPtr);
    }
  }

  int overCount = (int)(32*tempNumWords - i_bitLen) / 4;

  if (overCount > 0) {      
    ret.erase(ret.length() - overCount, ret.length()-1);
  }

#ifndef REMOVE_SIM
  // If we are using this interface and find Xstate data we have a problem 
  if ((iv_XstateEnabled) && hasXstate(i_start, i_bitLen)) {
    ETRAC0("**** WARNING : ecmdDataBuffer::genHexLeftStr: Cannot extract when non-binary (X-State) character present");
    SET_ERROR(ECMD_DBUF_XSTATE_ERROR);
  }
#endif

  return ret; 
}


std::string ecmdDataBuffer::genHexRightStr(uint32_t i_start, uint32_t i_bitLen) const {

  /* Do gen a hex right string, we just shift the data right to nibble align and then do a genHexLeft - tricky eh */
  int shiftAmt = i_bitLen % 4 ? 4 - (i_bitLen % 4) : 0;
  std::string ret;
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (i_start+i_bitLen > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::genHexRightStr: i_start %d + i_len %d >= NumBits (%d)", i_start, i_bitLen, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return ret;
  }
// more input checks done as part of shiftRightAndResize and genHexLeftStr()

  ecmdDataBuffer temp;
  temp.setBitLength(i_bitLen);
  extract(temp, i_start, i_bitLen);

  if (shiftAmt) {
    rc = temp.shiftRightAndResize(shiftAmt);
    if (rc) {
      return ret;
    }
  }
  ret = temp.genHexLeftStr();

#ifndef REMOVE_SIM
  /* If we are using this interface and find Xstate data we have a problem */
  if ((iv_XstateEnabled) && hasXstate(i_start, i_bitLen)) {
    ETRAC0("**** WARNING : ecmdDataBuffer::genHexRightStr: Cannot extract when non-binary (X-State) character present");
    SET_ERROR(ECMD_DBUF_XSTATE_ERROR);
  }
#endif
    
  return ret;
}

std::string ecmdDataBuffer::genBinStr(uint32_t i_start, uint32_t i_bitLen) const {

  int tempNumWords = (i_bitLen + 31) / 32;
  std::string ret;
  char* data = new char[i_bitLen + 1];
  bool createdTemp = false;
  uint32_t* tempData = NULL;
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (i_start+i_bitLen > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::genBinStr: i_start %d + i_len %d >= NumBits (%d)", i_start, i_bitLen, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    if (data) // make sure we have the buffer before running delete on it.
    {
        delete[] data;
    }
    return ret;
  }
// more input checks done as part of extract()

  /* Let's see if we can get a perf bonus */
  if ((i_start == 0) && (i_bitLen == iv_NumBits))
    tempData = iv_Data;
  else {
    tempData = new uint32_t[tempNumWords];
    /* extract iv_Data */
    rc = this->extract(&tempData[0], i_start, i_bitLen);
    if (rc) {
      if (data) // make sure we have the buffer before running delete on it.
      {
          delete[] data;
      }

      if (tempData) // make sure we have the buffer before running delete on it.
      {
          delete[] tempData;
      }
      return ret;
    }
    createdTemp = true;
  }

  uint32_t mask = 0x80000000;
  int curWord = 0;

  for (uint32_t w = 0; w < i_bitLen; w++) {
    if (tempData[curWord] & mask) {
      data[w] = '1';
    }
    else {
      data[w] = '0';
    }

    mask >>= 1;

    if (!mask) {
      curWord++;
      mask = 0x80000000;
    }

  }
  /* Terminate this puppy */
  data[i_bitLen] = '\0';

  if (createdTemp)
    delete[] tempData;

  ret = data;
  delete[] data;

#ifndef REMOVE_SIM
  /* If we are using this interface and find Xstate data we have a problem */
  if ((iv_XstateEnabled) && hasXstate(i_start, i_bitLen)) {
    ETRAC0("**** WARNING : ecmdDataBuffer::genBinStr: Cannot extract when non-binary (X-State) character present");
    SET_ERROR(ECMD_DBUF_XSTATE_ERROR);
  }
#endif

  return ret;
}

std::string ecmdDataBuffer::genAsciiStr(uint32_t i_start, uint32_t i_bitLen) const {
  std::string ret;
  uint8_t tempByte;

  if ((i_start + i_bitLen) > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::genAsciiStr: start %d + bitLen %d >= NumBits (%d)", i_start, i_bitLen, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return ret;
  } else if (i_start >= iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::genAsciiStr: bit %d >= NumBits (%d)", i_start, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return ret;
  } else if (i_bitLen > iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::genAsciiStr: bitLen %d > NumBits (%d)", i_bitLen, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return ret;
  } else if (i_bitLen == 0) {
    ret = "";
    return ret;
  }

  // Create a temp data buffer, then just loop through it calling getByte
  // We create this temp buffer so that we can get data the user wanted aligned on a byte boundary
  // If the user specfied an i_start of 2, it's necessary to get everything aligned
  ecmdDataBuffer asciiBuffer(i_bitLen);
  asciiBuffer.insert(*this, 0, i_bitLen, i_start);

  int numBytes = asciiBuffer.getByteLength();
  for (int i = 0; i < numBytes; i++) { /* byte loop */
    tempByte = asciiBuffer.getByte(i);  /* grab a byte */  	  
    if (tempByte < 32 || tempByte > 126) { /* decimal 32 == space, 127 == DEL */
      ret += ".";  /* non-printing: use a . */
    } else {
      ret += tempByte; /* convert to ascii */
    }  
  }

#ifndef REMOVE_SIM
  /* If we are using this interface and find Xstate data we have a problem */
  if ((iv_XstateEnabled) && hasXstate(i_start, i_bitLen)) {
    ETRAC0("**** WARNING : ecmdDataBuffer::genAsciiStr: Cannot extract when non-binary (X-State) character present");
    SET_ERROR(ECMD_DBUF_XSTATE_ERROR);
  }
#endif

  return ret;
}
std::string ecmdDataBuffer::genAsciiPrintStr(uint32_t i_start, uint32_t i_bitlen) const {

  // Call genAsciiStr to get the string, this will take care of all our error checking
  std::string ret = genAsciiStr(i_start, i_bitlen);

  // Now search through the buffer, dealing with special characters
  size_t linePos = 0;
  while (linePos != std::string::npos) {

    // Handle %
    linePos = ret.find("%", linePos);
    if (linePos != std::string::npos) {
      ret.insert(linePos, "%");
      linePos += 2; // Skip past what we just inserted
    }

  }

  return ret;
}

#ifndef REMOVE_SIM
std::string ecmdDataBuffer::genXstateStr(uint32_t i_start, uint32_t i_bitLen) const {
  std::string ret;
  if (!iv_XstateEnabled) {
    ETRAC0("**** ERROR : ecmdDataBuffer::genXstateStr: Xstate operation called on buffer without xstate's enabled");
    SET_ERROR(ECMD_DBUF_XSTATE_NOT_ENABLED);
    return ret;
  }
    

  if (i_start+i_bitLen > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::genXstateStr: i_start %d + i_len %d >= NumBits (%d)", i_start, i_bitLen, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return ret;
  }

  char * copyStr = new char[i_bitLen + 4];
  strncpy(copyStr, &iv_DataStr[i_start], i_bitLen);
  copyStr[i_bitLen] = '\0';

  ret = copyStr;

  delete[] copyStr;

  return ret;
}
#endif /* REMOVE_SIM */

std::string ecmdDataBuffer::genHexLeftStr() const { return this->genHexLeftStr(0, iv_NumBits); }
std::string ecmdDataBuffer::genHexRightStr() const { return this->genHexRightStr(0, iv_NumBits); }
std::string ecmdDataBuffer::genBinStr() const { return this->genBinStr(0, iv_NumBits); }
std::string ecmdDataBuffer::genAsciiStr() const { return this->genAsciiStr(0, iv_NumBits); }
#ifndef REMOVE_SIM
std::string ecmdDataBuffer::genXstateStr() const { return this->genXstateStr(0, iv_NumBits); }
#endif /* REMOVE_SIM */

uint32_t ecmdDataBuffer::insertFromHexLeftAndResize (const char * i_hexChars, uint32_t i_start, uint32_t i_length) {
  if (i_length == 0) {
    i_length = strlen(i_hexChars) * 4;
  }

  /* If the user left the data identifier on the front, remove it */
  if (i_start == 0 && (strlen(i_hexChars)> 3)) {
    if (i_hexChars[0] == '0' && i_hexChars[1] == 'x') {
      if (i_hexChars[2] == 'l') {
        i_hexChars += 3; // Advance the pointer past the special characters
        i_length -= 12;    // And shrink the bit length
      } else {
        i_hexChars += 2; // Advance the pointer two characters to bypass it
        i_length -= 8;     // And shrink the bit length
      }
    }
  }

  int rc = setBitLength(i_length);
  if (rc) return rc;
  return insertFromHexLeft(i_hexChars, i_start, i_length);
}

uint32_t ecmdDataBuffer::insertFromHexLeft (const char * i_hexChars, uint32_t i_start, uint32_t i_length) {
  int rc = ECMD_DBUF_SUCCESS;
  int i;

  int bitlength = (i_length == 0 ? strlen(i_hexChars) * 4 : i_length);

  /* If the user left the data identifier on the front, remove it */
  if (i_start == 0 && (strlen(i_hexChars)> 3)) {
    if (i_hexChars[0] == '0' && i_hexChars[1] == 'x') {
      if (i_hexChars[2] == 'l') {
        i_hexChars += 3; // Advance the pointer past the special characters
        bitlength -= 12;    // And shrink the bit length
      } else {
        i_hexChars += 2; // Advance the pointer two characters to bypass it
        bitlength -= 8;     // And shrink the bit length
      }
    }
  }

  if (bitlength == 0) {
    /* They don't want anything inserted */
    return rc;
  }

  int wordLength = (bitlength + 31) / 32;

  uint32_t * number_ptr = new uint32_t[wordLength];
  for (i = 0; i < wordLength; i++) {
    number_ptr[i] = 0x0;
  }

  uint32_t tmpb32 = 0x0;
  char nextOne[2];
  nextOne[1] = '\0';

  int wordSize = wordLength * 8;
  int loopCount = (wordSize < (int)strlen(i_hexChars) ? wordSize : (int)strlen(i_hexChars));

  for (i = 0; i < loopCount ; i++) {
    if ((i & 0xFFF8) == i)
      number_ptr[i>>3] = 0x0;
    if (!isxdigit(i_hexChars[i])) {
      delete[] number_ptr;        //@01a delete buffer to prevent memory leak.
      RETURN_ERROR(ECMD_DBUF_INVALID_DATA_FORMAT);
    }
    nextOne[0] = i_hexChars[i];
    tmpb32 = strtoul(nextOne, NULL, 16);
    number_ptr[i>>3] |= (tmpb32 << (28 - (4 * (i & 0x07))));
  }


  this->insert(number_ptr, i_start, bitlength);

  delete[] number_ptr;

  return rc;
}

uint32_t ecmdDataBuffer::insertFromHexRightAndResize (const char * i_hexChars, uint32_t i_start, uint32_t i_length) {
  if (i_length == 0) {
    i_length = strlen(i_hexChars) * 4;
  }

  /* If the user left the data identifier on the front, remove it */
  if (i_start == 0 && (strlen(i_hexChars)> 3)) {
    if (i_hexChars[0] == '0' && i_hexChars[1] == 'x' && i_hexChars[2] == 'r') {
      i_hexChars += 3; // Advance the pointer past the special characters
      i_length -= 12;  // And shrink the bit length
    }
  }

  int rc = setBitLength(i_length);
  if (rc) return rc;
  return insertFromHexRight(i_hexChars, i_start, i_length);
}

uint32_t ecmdDataBuffer::insertFromHexRight (const char * i_hexChars, uint32_t i_start, uint32_t i_expectedLength) {
  int rc = ECMD_DBUF_SUCCESS;
  ecmdDataBuffer insertBuffer;
  int bitlength = (i_expectedLength == 0 ? strlen(i_hexChars) * 4 : i_expectedLength);

  /* If the user left the data identifier on the front, remove it */
  if (i_start == 0 && (strlen(i_hexChars)> 3)) {
    if (i_hexChars[0] == '0' && i_hexChars[1] == 'x' && i_hexChars[2] == 'r') {
      i_hexChars += 3; // Advance the pointer past the special characters
      bitlength -= 12;  // And shrink the bit length
    }
  }

  if (bitlength == 0) {
    /* They don't want anything inserted */
    return rc;
  }

  /* Number of valid nibbles */
  uint32_t nibbles = (bitlength + 3) / 4;

  /* If they provided us more data then we expect we will have to offset into the data to start reading */
  uint32_t dataOverFlowOffset = strlen(i_hexChars) > nibbles ? strlen(i_hexChars) - nibbles : 0;

  /* First we will insert it as if it is left aligned , then we will just shift it over */

  /* First we set to the length of actual data */
  insertBuffer.setBitLength(strlen(i_hexChars) * 4);

  /* We need to offset into i_hexChars if they provided us more data then we expect */
  rc = insertBuffer.insertFromHexLeft(&(i_hexChars[dataOverFlowOffset]), 0, strlen(i_hexChars) * 4);
  if (rc) return rc;

  /* Now we have to shiftRight to align if they didn't provide as much data as was expected */
  /* So our data becomes right aligned in a buffer of size expectedlength */
  if (strlen(i_hexChars) < nibbles) {
    rc = insertBuffer.shiftRightAndResize((nibbles - strlen(i_hexChars)) * 4);
    if (rc) return rc;
  }

  /* Now we have left aligned data, we just shift to right the odd bits of the nibble to align to the right */
  if (bitlength % 4) {
    rc = insertBuffer.shiftLeftAndResize(4 - (bitlength % 4));
    if (rc) return rc;
  }
  /* Now we have our data insert into ourselves */

  this->insert(insertBuffer, i_start, bitlength);

  return rc;
}

uint32_t ecmdDataBuffer::insertFromBinAndResize (const char * i_binChars, uint32_t i_start, uint32_t i_length) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (i_length == 0) {
    i_length = strlen(i_binChars);
  }

  rc = setBitLength(i_length);
  if (rc) return rc;

  /* If the user left the data identifier on the front, remove it */
  if (i_start == 0 && (strlen(i_binChars)> 2)) {
    if (i_binChars[0] == '0' && i_binChars[1] == 'b') {
      i_binChars += 2; // Advance the pointer past the special characters
      i_length -= 2;   // And shrink the bit length
    }
  }

  return insertFromBin(i_binChars, i_start);
}

uint32_t ecmdDataBuffer::insertFromBin (const char * i_binChars, uint32_t i_start, uint32_t i_length) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  // input checking done with clearBit() and setBit()
  if (i_length == 0) {
    i_length = strlen(i_binChars);
  }

  /* If the user left the data identifier on the front, remove it */
  if (i_start == 0 && (strlen(i_binChars)> 2)) {
    if (i_binChars[0] == '0' && i_binChars[1] == 'b') {
      i_binChars += 2; // Advance the pointer past the special characters
      i_length -= 2;   // And shrink the bit length
    }
  }


  for (uint32_t i = 0; i < i_length; i++) {
    if (i_binChars[i] == '0') {
      this->clearBit(i_start+i);
    }
    else if (i_binChars[i] == '1') {
      this->setBit(i_start+i);
    } else {
      RETURN_ERROR(ECMD_DBUF_INVALID_DATA_FORMAT);
    }
  }

  return rc;
}

uint32_t ecmdDataBuffer::insertFromAsciiAndResize(const char * i_asciiChars, uint32_t i_start) {
  int rc = setBitLength(strlen(i_asciiChars)*8);
  if (rc) return rc;
  return insertFromAscii(i_asciiChars, i_start);
}

uint32_t ecmdDataBuffer::insertFromAscii(const char * i_asciiChars, uint32_t i_start) {
  /* We can just call insert on this for the bitLength of the char buffer */
  uint32_t bitLength = strlen(i_asciiChars)*8;
  return insert((uint8_t *)i_asciiChars, i_start, bitLength, 0);
}

uint32_t ecmdDataBuffer::copy(ecmdDataBuffer &o_newCopy) const {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  rc = o_newCopy.setBitLength(iv_NumBits);
  
  if (!rc && iv_NumBits != 0) {
    // iv_Data
    memcpy(o_newCopy.iv_Data, iv_Data, iv_NumWords * 4);
    // Error state
    o_newCopy.iv_RealData[EDB_RETURN_CODE] = iv_RealData[EDB_RETURN_CODE];
#ifndef REMOVE_SIM
    if (iv_XstateEnabled) {
      /* enable the xstate in the copy */
      o_newCopy.enableXstateBuffer();
      strncpy(o_newCopy.iv_DataStr, iv_DataStr, iv_NumBits);
    } else if (o_newCopy.iv_XstateEnabled) {
      /* disable the xstate in the copy */
      o_newCopy.disableXstateBuffer();
    }
#endif
  }
  return rc;

}

/* Copy Constructor */
ecmdDataBuffer& ecmdDataBuffer::operator=(const ecmdDataBuffer & i_master) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  rc = setBitLength(i_master.iv_NumBits);

  if (!rc && iv_NumBits != 0) {
    // iv_Data
    memcpy(iv_Data, i_master.iv_Data, iv_NumWords * 4);
    // Error state
    iv_RealData[EDB_RETURN_CODE] = i_master.iv_RealData[EDB_RETURN_CODE];
#ifndef REMOVE_SIM
    if (i_master.iv_XstateEnabled) {
      enableXstateBuffer();
      strncpy(iv_DataStr, i_master.iv_DataStr, i_master.iv_NumBits);
    } else if (iv_XstateEnabled) {
      disableXstateBuffer();
    }
#endif
  }
  return *this;
}


uint32_t ecmdDataBuffer::memCopyIn(const uint32_t* i_buf, uint32_t i_bytes) { /* Does a memcpy from supplied buffer into ecmdDataBuffer */
  uint32_t rc = ECMD_DBUF_SUCCESS;

  uint32_t cbytes = i_bytes < getByteLength() ? i_bytes : getByteLength();
  if (cbytes == 0) {
    ETRAC0("**** ERROR : ecmdDataBuffer: memCopyIn: Copy performed on buffer with length of 0");
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }

  ecmdBigEndianMemCopy(iv_Data, i_buf, cbytes);

  /* We're worried we might have data on in our last byte copied in that excedes numbits */
  if (cbytes == getByteLength()) {
    /* We'll cheat and do a getByte and then write that value back so the masking logic is done */
    uint8_t myByte = getByte((getByteLength() - 1));
    rc = setByte((getByteLength() - 1), myByte);
    if (rc) return rc;
  }

#ifndef REMOVE_SIM
  if (iv_XstateEnabled) {
    uint32_t mask = 0x80000000;
    uint32_t curWord = 0;

    for (uint32_t w = 0; w < cbytes*8; w++) {
      if (iv_Data[curWord] & mask) {
        iv_DataStr[w] = '1';
      }
      else {
        iv_DataStr[w] = '0';
      }

      mask >>= 1;

      if (!mask) {
        curWord++;
        mask = 0x80000000;
      }
    }
  }

#endif
  return rc;
}

uint32_t ecmdDataBuffer::memCopyOut(uint32_t* o_buf, uint32_t i_bytes) const { /* Does a memcpy from ecmdDataBuffer into supplied buffer */
  uint32_t rc = ECMD_DBUF_SUCCESS;
  uint32_t cbytes = i_bytes < getByteLength() ? i_bytes : getByteLength();
  if (cbytes == 0) {
    ETRAC0("**** ERROR : ecmdDataBuffer: memCopyOut: Copy performed on buffer with length of 0");
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    ecmdBigEndianMemCopy(o_buf, iv_Data, cbytes);
  }
  return rc;
}

uint32_t ecmdDataBuffer::memCopyIn(const uint8_t* i_buf, uint32_t i_bytes) { /* Does a memcpy from supplied buffer into ecmdDataBuffer */
  uint32_t rc = ECMD_DBUF_SUCCESS;
  uint32_t cbytes = i_bytes < getByteLength() ? i_bytes : getByteLength();
  if (cbytes == 0) {
    ETRAC0("**** ERROR : ecmdDataBuffer: memCopyIn: Copy performed on buffer with length of 0");
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    for (uint32_t i=0; i<cbytes; i++) {
      setByte(i, i_buf[i]);
    }
  }
  return rc;
}

uint32_t ecmdDataBuffer::memCopyOut(uint8_t* o_buf, uint32_t i_bytes) const { /* Does a memcpy from supplied buffer into ecmdDataBuffer */
  uint32_t rc = ECMD_DBUF_SUCCESS;
  uint32_t cbytes = i_bytes < getByteLength() ? i_bytes : getByteLength();
  if (cbytes == 0) {
    ETRAC0("**** ERROR : ecmdDataBuffer: memCopyIn: Copy performed on buffer with length of 0");
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    for (uint32_t i=0; i<cbytes; i++) {
      o_buf[i] = getByte(i);
    }
  }
  return rc;
}

uint32_t ecmdDataBuffer::flatten(uint8_t * o_data, uint32_t i_len) const {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  uint32_t * o_ptr = (uint32_t *) o_data;

  if ((i_len < 8) || (iv_Capacity*32 > ((i_len - 8) * 8))) {
    ETRAC2("**** ERROR : ecmdDataBuffer::flatten: i_len %d bytes is too small to flatten a capacity of %d words ", i_len, iv_Capacity);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);

  } else {
    memset(o_data, 0, this->flattenSize());
    o_ptr[0] = htonl(iv_Capacity*32);
    o_ptr[1] = htonl(iv_NumBits);
    if (iv_Capacity > 0) {
      for (uint32_t i = 0; i < iv_Capacity; i++)
        o_ptr[2+i] = htonl(iv_Data[i]);
    }
  }
  return rc;
}

uint32_t ecmdDataBuffer::unflatten(const uint8_t * i_data, uint32_t i_len) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  uint32_t newCapacity;
  uint32_t newBitLength;
  uint32_t * i_ptr = (uint32_t *) i_data;
  uint32_t newWordLength;

  newCapacity = (ntohl(i_ptr[0]) + 31) / 32;
  newBitLength = ntohl(i_ptr[1]);

  if ((i_len < 8) || (newCapacity > ((i_len - 8) * 8))) {
    ETRAC2("**** ERROR : ecmdDataBuffer::unflatten: i_len %d bytes is too small to unflatten a capacity of %d words ", i_len, newCapacity);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);

  } else if (newBitLength > newCapacity * 32) {
    ETRAC2("**** ERROR : ecmdDataBuffer::unflatten: iv_NumBits %d cannot be greater than iv_Capacity*32 %d", newBitLength, newCapacity*32);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    rc = this->setCapacity(newCapacity);
    if (rc != ECMD_DBUF_SUCCESS) {
      ETRAC2("**** ERROR : ecmdDataBuffer::unflatten: this->setCapacity() Failed. rc=0x%08x, newCapacity = %d words ", rc, newCapacity);
      RETURN_ERROR(rc);
    }

    rc = this->setBitLength(newBitLength);
    if (rc != ECMD_DBUF_SUCCESS) {
      ETRAC2("**** ERROR : ecmdDataBuffer::unflatten: this->setBitLength() Failed. rc=0x%08x, newBitLength = %d bits ", rc, newBitLength);
      RETURN_ERROR(rc);
    }

    newWordLength = getWordLength();
    if (newCapacity > 0)
	for (uint32_t i = 0; i < newWordLength ; i++) {
	    rc = setWord(i, ntohl(i_ptr[i+2]));
	    if (rc != ECMD_DBUF_SUCCESS) {
		ETRAC5("**** ERROR : ecmdDataBuffer::unflatten: this->setWord() Failed. rc=0x%08x  newBitLength = %d bits, newWordLength= %d words, newCapacity = %d words, loop parm i = %d ", rc, newBitLength, newWordLength, newCapacity, i);
		RETURN_ERROR(rc);
	    }
	}
  }
  return rc;
}

uint32_t ecmdDataBuffer::flattenSize() const {
  return (iv_Capacity + 2) * 4;
}


/**
 * @brief Initializes the X-state buffer, from then on all changes are reflected in Xstate
 * @post Xstate buffer is created and initialized to value of current raw buffer
 * @retval ECMD_DBUF_SUCCESS on success
 * @retval ECMD_DBUF_INIT_FAIL failure occurred allocating X-state array
 * @retval ECMD_DBUF_NOT_OWNER when called on buffer not owned
 */
#ifndef REMOVE_SIM
uint32_t ecmdDataBuffer::enableXstateBuffer() {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if(!iv_UserOwned)
  {
    ETRAC0("**** ERROR (ecmdDataBuffer::enableXstateBuffer) : Attempt to modify non user owned buffer size.");
    RETURN_ERROR(ECMD_DBUF_NOT_OWNER);
  }

  /* If it is already enabled, we don't do it again */
  if (iv_XstateEnabled) return rc;

  if (iv_NumBits > 0) {
    /* Check for null here to satisfy lint, but should always be NULL coming into this */
    if (iv_DataStr == NULL) iv_DataStr = new char[iv_NumBits + 42];
    if (iv_DataStr == NULL) {
      ETRAC0("**** ERROR : ecmdDataBuffer::enableXstateBuffer : Unable to allocate Xstate memory for new databuffer");
      RETURN_ERROR(ECMD_DBUF_INIT_FAIL);
    }
    iv_DataStr[iv_NumBits] = '\0';

    /* Copy the raw data into the xstate buffer */
    uint32_t mask = 0x80000000;
    uint32_t curWord = 0;
    uint32_t numBits = getBitLength();

    for (uint32_t w = 0; w < numBits; w++) {
      if (iv_Data[curWord] & mask) {
        iv_DataStr[w] = '1';
      }
      else {
        iv_DataStr[w] = '0';
      }

      mask >>= 1;

      if (!mask) {
        curWord++;
        mask = 0x80000000;
      }
    }
  }
  iv_XstateEnabled = true;
  return rc;
}
#endif /* REMOVE_SIM */

/**
 * @brief Removes the X-state buffer, from then on no changes are made to Xstate
 * @post Xstate buffer is deallocated
 * @retval ECMD_DBUF_SUCCESS on success
  * @retval ECMD_DBUF_NOT_OWNER when called on buffer not owned
  */
#ifndef REMOVE_SIM
uint32_t ecmdDataBuffer::disableXstateBuffer() {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if(!iv_UserOwned)
  {
    ETRAC0("**** ERROR (ecmdDataBuffer::disableXstateBuffer) : Attempt to modify non user owned buffer size.");
    RETURN_ERROR(ECMD_DBUF_NOT_OWNER);
  }
  if (iv_DataStr != NULL) {
    delete[] iv_DataStr;
    iv_DataStr = NULL;
  }
  iv_XstateEnabled = false;
  return rc;

 }
#endif /* REMOVE_SIM */

 /**
  * @brief Query to find out if this buffer has X-states enabled
  * @retval true if the Xstate buffer is active
  * @retval false if the Xstate buffer is not active
  */
#ifndef REMOVE_SIM
bool ecmdDataBuffer::isXstateEnabled() const {
 return iv_XstateEnabled;
}
#endif /* REMOVE_SIM */



#ifndef REMOVE_SIM
uint32_t ecmdDataBuffer::flushToX(char i_value) {
  uint32_t rc = ECMD_DBUF_SUCCESS;


  if (iv_NumWords > 0) {
    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */
    rc = this->fillDataStr(i_value);
  }
  return rc;
}
#endif /* REMOVE_SIM */

#ifndef REMOVE_SIM
bool ecmdDataBuffer::hasXstate() const {
  if (!iv_XstateEnabled) {
    ETRAC0("**** ERROR : ecmdDataBuffer::hasXstate: Xstate operation called on buffer without xstate's enabled");
    SET_ERROR(ECMD_DBUF_XSTATE_NOT_ENABLED);
    return false;
  }

  return (hasXstate(0,iv_NumBits));
}
#endif /* REMOVE_SIM */

#ifndef REMOVE_SIM
bool   ecmdDataBuffer::hasXstate(uint32_t i_start, uint32_t i_length) const {
  if (!iv_XstateEnabled) {
    ETRAC0("**** ERROR : ecmdDataBuffer::hasXstate: Xstate operation called on buffer without xstate's enabled");
    SET_ERROR(ECMD_DBUF_XSTATE_NOT_ENABLED);
    return false;
  }

  uint32_t stopBit = i_start + i_length;
  uint32_t minStop = iv_NumBits < stopBit ? iv_NumBits : stopBit; /* pick the smallest */

  for (uint32_t i = i_start; i < minStop; i++) {
    if (iv_DataStr[i] != '0' && iv_DataStr[i] != '1')
      return true;
  }
  return false;
}
#endif /* REMOVE_SIM */

/**
 * @brief Retrieve an Xstate value from the buffer
 * @param i_bit Bit to retrieve

 * NOTE - To retrieve multipe bits use genXstateStr
 */
#ifndef REMOVE_SIM
char ecmdDataBuffer::getXstate(uint32_t i_bit) const {
  if (!iv_XstateEnabled) {
    ETRAC0("**** ERROR : ecmdDataBuffer::getXstate: Xstate operation called on buffer without xstate's enabled");
    SET_ERROR(ECMD_DBUF_XSTATE_NOT_ENABLED);
    return '0';
  }

  if (i_bit >= iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::getXstate: bit %d >= NumBits (%d)", i_bit, iv_NumBits);
    SET_ERROR(ECMD_DBUF_XSTATE_ERROR);
    return '0';
  }
  return iv_DataStr[i_bit];
}
#endif /* REMOVE_SIM */

/**
 * @brief Set an Xstate value in the buffer
 * @param i_bit Bit to set
 */
#ifndef REMOVE_SIM
uint32_t ecmdDataBuffer::setXstate(uint32_t i_bit, char i_value) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (!iv_XstateEnabled) {
    ETRAC0("**** ERROR : ecmdDataBuffer::setXstate: Xstate operation called on buffer without xstate's enabled");
    RETURN_ERROR(ECMD_DBUF_XSTATE_NOT_ENABLED);
  }

  if (i_bit >= iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::setXstate: bit %d >= NumBits (%d)", i_bit, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    if (i_value == '0') rc = clearBit(i_bit);
    else if (i_value == '1') rc = setBit(i_bit);
    else if (!isxdigit(i_value)) {
      /* We call clearbit to write the raw bit to 0 */
      rc = clearBit(i_bit);
      iv_DataStr[i_bit] = i_value;
    } else {
      ETRAC1("**** ERROR : ecmdDataBuffer::setXstate: unrecognized Xstate character: %c", i_value);
      RETURN_ERROR(ECMD_DBUF_XSTATE_ERROR);
    }
  }
  return rc;
}
#endif /* REMOVE_SIM */

#ifndef REMOVE_SIM
uint32_t ecmdDataBuffer::setXstate(uint32_t i_bit, char i_value, uint32_t i_length) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (!iv_XstateEnabled) {
    ETRAC0("**** ERROR : ecmdDataBuffer::setXstate: Xstate operation called on buffer without xstate's enabled");
    RETURN_ERROR(ECMD_DBUF_XSTATE_NOT_ENABLED);
  }

  if (i_bit + i_length > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::setXstate: bit %d + len %d > NumBits (%d)", i_bit, i_length, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    for (uint32_t idx = 0; idx < i_length; idx ++) rc |= this->setXstate(i_bit + idx, i_value);    
  }
  return rc;
}
#endif /* REMOVE_SIM */

/**
 * @brief Set a range of Xstate values in buffer
 * @param i_bitoffset bit in buffer to start inserting
 * @param i_datastr Character value to set bit - can be "0", "1", "X"
 */
#ifndef REMOVE_SIM
uint32_t ecmdDataBuffer::setXstate(uint32_t i_bitOffset, const char* i_datastr) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (!iv_XstateEnabled) {
    ETRAC0("**** ERROR : ecmdDataBuffer::setXstate: Xstate operation called on buffer without xstate's enabled");
    RETURN_ERROR(ECMD_DBUF_XSTATE_NOT_ENABLED);
  }

  uint32_t len = (uint32_t)strlen(i_datastr);
  uint32_t i;

  if (i_bitOffset+len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::setXstate: i_bitOffset %d + len %d > NumBits (%d)", i_bitOffset, len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {

    for (i = 0; i < len; i++) {
      if (i_datastr[i] == '0') rc = clearBit(i_bitOffset+i);
      else if (i_datastr[i] == '1') rc = setBit(i_bitOffset+i);
      else if (!isxdigit(i_datastr[i])) {
        rc = clearBit(i_bitOffset+i);
        iv_DataStr[i_bitOffset+i] = i_datastr[i];
      } 
      else {
        ETRAC1("**** ERROR : ecmdDataBuffer::setXstate: unrecognized Xstate character: %c", i_datastr[i]);
      RETURN_ERROR(ECMD_DBUF_XSTATE_ERROR);
      }
    }

  }
  return rc;

}
#endif /* REMOVE_SIM */


#ifndef REMOVE_SIM
uint32_t ecmdDataBuffer::memCopyInXstate(const char * i_buf, uint32_t i_bits) { /* Does a memcpy from supplied buffer into ecmdDataBuffer */
  uint32_t rc = ECMD_DBUF_SUCCESS;

  /* cbytes is equal to the bit length of data */
  uint32_t cbytes = i_bits < getBitLength() ? i_bits : getBitLength();
  uint32_t index;

  if (!iv_XstateEnabled) {
    ETRAC0("**** ERROR : ecmdDataBuffer::memCopyInXstate: Xstate operation called on buffer without xstate's enabled");
    RETURN_ERROR(ECMD_DBUF_XSTATE_NOT_ENABLED);
  }
  if (strlen(i_buf) < i_bits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::memCopyInXstate: supplied buffer(%d) shorter than i_bits(%d)", strlen(i_buf), i_bits);
    RETURN_ERROR(ECMD_DBUF_INVALID_DATA_FORMAT);
  }

  /* Put the data into the Xstate array */
  strncpy(iv_DataStr, i_buf, cbytes);
  iv_DataStr[cbytes] = '\0';

  /* Now slide it over to the raw buffer */
  for (uint32_t bit = 0; bit < cbytes; bit++) {
    index = bit/32;
    if (i_buf[bit] == '1') {
      iv_Data[index] |= 0x00000001 << (31 - (bit-(index * 32)));
    } else {
      iv_Data[index] &= (~0x00000001 << (31 - (bit-(index * 32))));
    }
  }

  return rc;
}
#endif /* REMOVE_SIM */

#ifndef REMOVE_SIM
uint32_t ecmdDataBuffer::memCopyOutXstate(char * o_buf, uint32_t i_bits) const { /* Does a memcpy from ecmdDataBuffer into supplied buffer */
  uint32_t rc = ECMD_DBUF_SUCCESS;
  uint32_t cbytes = i_bits < getBitLength() ? i_bits : getBitLength();
  if (!iv_XstateEnabled) {
    ETRAC0("**** ERROR : ecmdDataBuffer::memCopyOutXstate: Xstate operation called on buffer without xstate's enabled");
    RETURN_ERROR(ECMD_DBUF_XSTATE_NOT_ENABLED);
  }

  strncpy(o_buf, iv_DataStr, cbytes);
  o_buf[cbytes] = '\0';

  return rc;
}
#endif /* REMOVE_SIM */



int ecmdDataBuffer::operator == (const ecmdDataBuffer& i_other) const {

  /* Check the length */
  uint32_t maxBits = 32;
  uint32_t numBits = getBitLength();
  uint32_t numToFetch = numBits < maxBits ? numBits : maxBits;
  uint32_t myData, otherData;
  uint32_t wordCounter = 0;

  if (getBitLength() != i_other.getBitLength()) {
    return 0;
  }

  if (getBitLength() == 0) /* two empty buffers are equal */
    return 1;

  /* Now run through the data */
  while (numToFetch > 0) {

    myData = iv_Data[wordCounter];
    otherData = i_other.iv_Data[wordCounter];

    if (numToFetch == maxBits) {
      if (myData != otherData) 
        return 0;
    }
    else {
      uint32_t mask = 0x80000000;
      for (uint32_t i = 0; i < numToFetch; i++, mask >>= 1) {
        if ( (myData & mask) != (otherData & mask) ) {
          return 0;
        }
      }
    }

    numBits -= numToFetch;
    numToFetch = (numBits < maxBits) ? numBits : maxBits;
    wordCounter++;
  }

#ifndef REMOVE_SIM
  /* Check the X-state buffer */
  if (iv_XstateEnabled && i_other.iv_XstateEnabled) {
    if (strncmp(iv_DataStr, i_other.iv_DataStr, iv_NumBits)) {
      return 0;
    }
  }
#endif

  /* Must have matched */
  return 1;
}

int ecmdDataBuffer::operator != (const ecmdDataBuffer& i_other) const {
  return !(*this == i_other);
}

ecmdDataBuffer ecmdDataBuffer::operator & (const ecmdDataBuffer& i_other) const {

  ecmdDataBuffer newItem = *this;

  if (iv_NumBits != i_other.iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::operater &: NumBits in (%d) do not match NumBits (%d)", i_other.iv_NumBits, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    newItem.setAnd(i_other.iv_Data, 0, iv_NumBits);
  }

  return newItem;
}

ecmdDataBuffer ecmdDataBuffer::operator | (const ecmdDataBuffer& i_other) const {

  ecmdDataBuffer newItem = *this;

  newItem.setOr(i_other.iv_Data, 0, iv_NumBits);

  return newItem;
}


//---------------------------------------------------------------------
//  Private Member Function Specifications
//---------------------------------------------------------------------
#ifndef REMOVE_SIM
uint32_t ecmdDataBuffer::fillDataStr(char i_fillChar) {
  if (!iv_XstateEnabled) {
    ETRAC0("**** ERROR : ecmdDataBuffer::fillDataStr: Xstate operation called on buffer without xstate's enabled");
    RETURN_ERROR(ECMD_DBUF_XSTATE_NOT_ENABLED);
  }
  if (iv_NumWords > 0) {
    memset(iv_DataStr, i_fillChar, iv_NumBits);
    iv_DataStr[iv_NumBits] = '\0';  
  }
  return ECMD_DBUF_SUCCESS;
}
#endif

uint32_t ecmdExtract(uint32_t *i_sourceData, uint32_t i_startBit, uint32_t i_numBitsToExtract, uint32_t *o_destData) {
  uint32_t temp;
  uint32_t len; 
  uint32_t mask1;
  uint32_t mask2;
  uint32_t offset;
  uint32_t index; 
  uint32_t count; 

  // Error check
  if ((i_numBitsToExtract == 0) || (i_sourceData == NULL)){
    ETRAC0("**** ERROR : ecmdDataBuffer ecmdExtract: Number of bits to extract = 0");
    o_destData = NULL;
    return ECMD_DBUF_INVALID_ARGS;
  } 

  count = (i_numBitsToExtract + 31) / 32;

  /*------------------------------------------------------------------*/
  /* calculate number of fws (32-bit pieces) of the destination buffer*/
  /* to be processed.                                                 */
  /*----------------------------line 98-------------------------------*/
  /* all 32-bit (or < 32 bits) pieces of the destination buffer */
  for (uint32_t i = 0; i < count; i++) {

    len = i_numBitsToExtract;
    /* length of bits to process is > 32 */
    if (len > 32) {
      len = 32;
    }

    /*******************************************************************/
    /* calculate index for accessing proper fw of the scan ring buffer */
    /*******************************************************************/
    index = i_startBit/32;

    /*----------------------------------------------------------------*/
    /* generate the mask to zero out some extra extracted bits (if    */
    /* there are any) in the temporary buffer.                        */
    /*----------------------------------------------------------------*/
    if (len == 32) {
      mask1 = 0xFFFFFFFF;
    } else {
      mask1 = ~(0xFFFFFFFF << len);
    }

    /*-------------line 121--------------------------------------------*/
    /* generate the mask to prevent zeroing of unused bits positions  */
    /* in the destination buffer.                                     */
    /*----------------------------------------------------------------*/
    if (len == 0) {
      mask2 = 0xFFFFFFFF;
    } else {
      mask2 = ~(mask1 << (32-len));
    }

    /******************************************************************/
    /* NOTE:- In this loop a max of 32 bits are extracted at a time.  */
    /* we may require to access either one or two fw's of scan ring   */
    /* buffer depending on the starting bit position & number of bits */
    /* to be extracted.                                               */
    /******************************************************************/
    /* only one fw of scan ring buffer required to do extract */
    if (index == ((i_startBit + (len-1))/32)) {
      /*--------------------------------------------------------------*/
      /* Extract required bits from the proper fw of the scan ring    */
      /* buffer as shown below (follow the steps):                    */
      /* step1 - right justify bits to be extracted from the fw of the*/
      /*         scan ring buffer.(we may have extra bits which are   */
      /*         not required to be extracted, in the high order bit  */
      /*         positions but they will be masked off later on).     */
      /* step2 - left justify the extracted bits in the temp buffer.  */
      /* result = (dest buffer with reqd bits zeroed) | step2         */
      /*          (Unused bit positions in the dest buffer will not   */
      /*           be changed.)                                       */
      /*--------------------------------------------------------------*/
      /* step1 */
      temp = ((*(i_sourceData + index)) >> (32-((i_startBit + len) - (index*32))));
      if ((32-((i_startBit + len) - (index*32))) >= 32)
        temp = 0x0;

      if ((32 - len) >= 32)
        temp = 0x0;
      else
        temp = (temp & mask1) << (32 - len); /* step2 */

      *(o_destData + i) = (*(o_destData + i) & mask2) | temp;
      /* two fws of scan ring buffer required to do extract */
    } else {
      /*-----------------line 158--------------------------------------*/
      /* calculate number of bits to process in the 1st fw of the     */
      /* scan ring buffer.(fw pointed by index)                       */
      /*--------------------------------------------------------------*/
      offset = (32 * (index + 1)) - i_startBit;

      /*--------------------------------------------------------------*/
      /* Extract required bits from the proper fws of the scan ring   */
      /* buffer as shown below (follow the steps):                    */
      /* step1 - Shift 1st fw of the scan ring buffer left by the     */
      /*         number of bits to be extracted from the 2nd fw.      */
      /*         Shift 2nd fw of the scan ring buffer right such that */
      /*         the required bits to be extracted are right justifed */
      /*         in that fw.                                          */
      /*         OR the results of the above shifts and save it in a  */
      /*         temporary buffer. (we will have required bits        */
      /*         extracted and right justifed in the temp buffer. Also*/
      /*         we may have some extra bits in the high order bits   */
      /*         position of the temp buffer, but they will be masked */
      /*         off later on.)                                       */
      /* step2 - left justify the extracted bits in the temp buffer.  */
      /* result = (dest buffer with reqd bits zeroed) | step2         */
      /*          (Unused bit positions in the dest buffer will not   */
      /*           be changed.)                                       */
      /*--------------------------------------------------------------*/
      /* step1 */
      uint32_t val1 = 0x0;
      uint32_t val2 = 0x0;
      if (len-offset < 32) {
        val1 = ((*(i_sourceData+index)) << (len-offset));  /* 1st fw*/
      }

      if ((32-(len-offset)) < 32) {
        val2 = ((*(i_sourceData+index+1)) >> (32-(len-offset)));
      }
      temp = (val1 | val2);/* 2nd fw */

      if (32-len >= 32) {
        temp = 0x0;
      } else {
        temp = (temp & mask1) << (32-len); /* step2 */
      }

      *(o_destData+i) = (*(o_destData+i) & mask2) | temp;
    }
    i_numBitsToExtract -= 32; /* decrement length by a fw */
    i_startBit += 32; /* increment start by a fw */
  }

  return ECMD_DBUF_SUCCESS;
}

uint32_t ecmdDataBuffer::writeFileMultiple(const char * i_filename, ecmdFormatType_t i_format, ecmdWriteMode_t i_mode, uint32_t & o_dataNumber, const char * i_facName) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  std::ofstream ops;
  std::ifstream ins;
  uint32_t totalFileSz, begOffset=0, tmphdrfield=0, szcount = 0, curDBOffset=0, tableSz=0;
  uint32_t numBytes = getByteLength();
  uint32_t numBits = getBitLength();
  bool firstDBWrite = false;
  char *offsetTableData=NULL, propertyStr[201];
  ecmdFormatType_t existingFmt;
  std::string asciidatastr,xstatestr;
  uint32_t *buffer;
  
 //Check if format asked for is the same as what was used b4
  #ifndef REMOVE_SIM
  if (iv_XstateEnabled) {
    if((hasXstate()) && (i_format != ECMD_SAVE_FORMAT_XSTATE)) {
      ETRAC0( "**** ERROR : ecmdDataBuffer: writeFileMultiple: Buffer has Xstate data but non-xstate save mode requested");
      return(ECMD_DBUF_XSTATE_ERROR);
    }
  }
  #else
  if (i_format == ECMD_SAVE_FORMAT_XSTATE) {
    ETRAC0( "**** ERROR : ecmdDataBuffer: writeFileMultiple: FORMAT_XSTATE not supported in this configuration");
    return(ECMD_DBUF_XSTATE_ERROR);
  }    
  #endif
  
  
 if (i_format == ECMD_SAVE_FORMAT_BINARY_DATA ) {
   if (i_mode != ECMD_WRITE_MODE) {
    ETRAC0("**** ERROR : File Format ECMD_SAVE_FORMAT_BINARY_DATA only supported in ECMD_WRITE_MODE");
    RETURN_ERROR(ECMD_DBUF_INVALID_ARGS);
   } else if (i_facName != NULL) {
    ETRAC0("**** ERROR : File Format ECMD_SAVE_FORMAT_BINARY_DATA does not support storing of string-property");
    RETURN_ERROR(ECMD_DBUF_INVALID_ARGS);
   }
 }

  if ((i_facName != NULL) && (strlen(i_facName) > 200)) {
    ETRAC0("**** ERROR : Input FacName string of length greater than 200 bytes not supported");
    RETURN_ERROR(ECMD_DBUF_INVALID_ARGS);
  } 
  if (i_facName != NULL) tmphdrfield = 0x80000000; // set the string property bit
  
  //Open file for Read if it exists
  ins.open(i_filename);
    
  if ((!ins.fail()) && (i_mode != ECMD_WRITE_MODE) && (i_format != ECMD_SAVE_FORMAT_BINARY_DATA)) {
    ins.seekg(0, std::ios::end);
    totalFileSz = ins.tellg();
    if (totalFileSz == 0) {
      firstDBWrite = true;
    }
    else {
      ins.seekg(totalFileSz-8);//goto the BEGIN of the offset table
      ins.read((char *)&begOffset,4); begOffset = htonl(begOffset);
      ins.read((char *)&existingFmt,4); existingFmt = (ecmdFormatType_t)htonl(existingFmt);
      if (existingFmt != i_format) {
        ETRAC0("**** ERROR : Format requested does not match up with the file Format.");
	RETURN_ERROR(ECMD_DBUF_INVALID_ARGS);
      }
      ins.seekg(begOffset);
      offsetTableData = new char[totalFileSz-begOffset];
      //Read the offset Table starting from BEGIN till before the END
      ins.read(offsetTableData, totalFileSz-begOffset-12);//Take off the 4byte END,4 Byte Beg Offset, 4byte format
      if (ins.fail()) {
       ETRAC1("**** ERROR : Read of the offset table failed on file : %s",i_filename);
       if (offsetTableData != NULL) delete[] offsetTableData;
       RETURN_ERROR(ECMD_DBUF_FILE_OPERATION_FAIL); 
      }
      tableSz = totalFileSz-begOffset-12;
    }
    ins.close();
  } else { firstDBWrite = true;}
  
  //Open file for read/write
  if (i_mode == ECMD_APPEND_MODE) {
   if (!firstDBWrite) {
    ops.open(i_filename,  std::ios_base::in|std::ios_base::out );
   }
   else {
    ops.open(i_filename,  std::ios_base::out );
   }
  } else if(i_mode == ECMD_WRITE_MODE) {
    ops.open(i_filename, std::ofstream::out | std::ofstream::trunc);
  }
  if (ops.fail()) {
    ETRAC1("**** ERROR : Unable to open file : %s for write",i_filename);
    if (offsetTableData != NULL) delete[] offsetTableData;
    RETURN_ERROR(ECMD_DBUF_FOPEN_FAIL);  
  }
  
  if (!firstDBWrite) {
    ops.seekp(begOffset);//Write the New Buffer after the last buffer, over the current Offset Table 
  }
  
  curDBOffset = ops.tellp(); curDBOffset = htonl(curDBOffset); //Get the pointer to the current data 
  
  //Write Header
  if (i_format != ECMD_SAVE_FORMAT_BINARY_DATA) {
    if (i_format == ECMD_SAVE_FORMAT_BINARY) {
      //BIN Hdr
      ops.write("START",8);
      numBits	  = htonl(numBits);			 ops.write((char *)&numBits,4);//Bit length field
      i_format    = (ecmdFormatType_t)htonl(i_format);   ops.write((char*)&i_format,4);//Format field
      tmphdrfield = htonl(tmphdrfield); 		 ops.write((char*)&tmphdrfield,4);//Leave 1 words extra room here
      if (i_facName != NULL) {
        memset(propertyStr, '\0', 201);
        strcpy(propertyStr, i_facName);
	ops.write(propertyStr, 200); // String associated with the data buffer
      }
    }
    else {
      //ASCII or XSTATE Hdr
      ops << "START";
      char tmpstr[9]; //@01c Bumped to 9 to account for NULL terminator
      sprintf(tmpstr,"%08X",numBits); ops.write(tmpstr,8);
      sprintf(tmpstr,"%08X",i_format); ops.write(tmpstr,8);
      sprintf(tmpstr,"%08X",tmphdrfield); ops.write(tmpstr,8);
      ops << "\n";
      if (i_facName != NULL) {
       ops << (i_facName); // String associated with the data buffer
       ops << "\n";
      }
    }
    if (ops.fail()) {
     ETRAC1("**** ERROR : Write of the header failed on file : %s",i_filename);
    if (offsetTableData != NULL) delete[] offsetTableData;
     RETURN_ERROR(ECMD_DBUF_FILE_OPERATION_FAIL); 
    }
  }
  //Write DataBuffer
  if ( i_format == ECMD_SAVE_FORMAT_BINARY) {
    //Write Data
    buffer = new uint32_t[getWordLength()];
    memCopyOut(buffer, numBytes);
    //Convert into Network Byte order-Big Endian before writing
    int len=0;
    for(uint32_t i=0; i< getWordLength(); i++) {
     buffer[i]=htonl(buffer[i]);
     len = 4;
     if ( i == (getWordLength()-1)) {
       len = numBytes - (i*4);
     }
     ops.write((char *)&buffer[i],len);
    }
    delete[] buffer; buffer = NULL;
    ops.write("END",3);//Key
    if (ops.fail()) {
     ETRAC1("**** ERROR : Write operation in format ECMD_SAVE_FORMAT_BINARY failed on file : %s",i_filename);
     if (offsetTableData != NULL) delete[] offsetTableData;
     RETURN_ERROR(ECMD_DBUF_FILE_OPERATION_FAIL); 
    }
  }  
  else if ( i_format == ECMD_SAVE_FORMAT_ASCII) {
   while ((uint32_t)szcount < numBits) {
     if((uint32_t)szcount+32 < numBits) {
      asciidatastr = genHexLeftStr(szcount, 32);
     } else {
      asciidatastr = genHexLeftStr(szcount, numBits-(szcount));
     }
     if (szcount !=0 ) {
      if((szcount % 256) == 0) {
  	ops << "\n"; 
      }
      else if((szcount % 32) == 0) {
  	ops << " ";
      }
     }
     ops << asciidatastr.c_str();
     szcount+= 32;
   }
   ops << "\nEND\n";
   if (ops.fail()) {
     ETRAC1("**** ERROR : Write operation in format ECMD_SAVE_FORMAT_ASCII failed on file : %s",i_filename);
     if (offsetTableData != NULL) delete[] offsetTableData;
     RETURN_ERROR(ECMD_DBUF_FILE_OPERATION_FAIL); 
   }
  } 
#ifndef REMOVE_SIM
  else if ( i_format == ECMD_SAVE_FORMAT_XSTATE) {
  if (!iv_XstateEnabled) {
    ETRAC0("**** ERROR : ecmdDataBuffer::getXstate: Xstate operation called on buffer without xstate's enabled");
    if (offsetTableData != NULL) delete[] offsetTableData;
    RETURN_ERROR(ECMD_DBUF_XSTATE_NOT_ENABLED);
  }
   while ((uint32_t)szcount < numBits) {
     if((uint32_t)szcount+64 < numBits) {
      xstatestr = genXstateStr(szcount, 64) ;
     } else {
      xstatestr = genXstateStr(szcount, numBits-(szcount)) ;
     }
     ops << xstatestr.c_str();
     if ((uint32_t)szcount+64<numBits)
      ops << "\n";
     szcount+= 64;
   }
   ops << "\nEND\n";
   if (ops.fail()) {
     ETRAC1("**** ERROR : Write operation in format ECMD_SAVE_FORMAT_XSTATE failed on file : %s",i_filename);
     if (offsetTableData != NULL) delete[] offsetTableData;
     RETURN_ERROR(ECMD_DBUF_FILE_OPERATION_FAIL); 
   }
  }
#endif
  else if (i_format == ECMD_SAVE_FORMAT_BINARY_DATA) {
    buffer = new uint32_t[getWordLength()];
    memCopyOut(buffer, numBytes);
    int len=0;
    //Convert into Network Byte order-Big Endian before writing
    for(uint32_t i=0; i< getWordLength(); i++) {
     buffer[i]=htonl(buffer[i]);
     if (((i+1)*4) > numBytes) {
      len = numBytes - (i*4);
     } else {
       len = 4;
     }
     ops.write((char *)&buffer[i],len);
    }
    delete[] buffer; buffer = NULL;
    if (ops.fail()) {
     ETRAC1("**** ERROR : Write operation in format ECMD_SAVE_FORMAT_BINARY_DATA failed on file : %s",i_filename);
     if (offsetTableData != NULL) delete[] offsetTableData;
     RETURN_ERROR(ECMD_DBUF_FILE_OPERATION_FAIL); 
    }
    ops.close();
  } 
  
  //Write the Offset Table Back with the new offset
  if (i_format != ECMD_SAVE_FORMAT_BINARY_DATA) {
     begOffset = ops.tellp();  begOffset = htonl(begOffset);
     if (!firstDBWrite) {
      ops.write(offsetTableData, tableSz);
      //Get the size of Offset Table after subtracting 8Byte BEGIN
      o_dataNumber = (tableSz - 8)/4; //Each Databuffer offset is a uint32_t (4 bytes)
     }
     else {
      ops.write("BEGIN",8);
      o_dataNumber = 0;
     }
     ops.write((char *)&curDBOffset, sizeof(curDBOffset));
     ops.write("END",4);
     ops.write((char *)&begOffset, sizeof(begOffset));
     i_format = (ecmdFormatType_t)htonl(i_format);   ops.write((char*)&i_format,4);
     ops.close();
  }
  
  if (offsetTableData != NULL) delete[] offsetTableData;
  return(rc);
}

uint32_t ecmdDataBuffer::writeFile(const char * filename, ecmdFormatType_t format, const char * i_facName) {
  ecmdWriteMode_t mode = ECMD_WRITE_MODE;
  uint32_t dataNumber;
  return this->writeFileMultiple(filename, format, mode, dataNumber, i_facName) ;
}

uint32_t ecmdDataBuffer::writeFileStream(std::ostream & o_filestream) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  uint32_t *buffer;
  uint32_t numBytes = getByteLength();
  
  buffer = new uint32_t[getWordLength()];
  rc = memCopyOut(buffer, numBytes);
  if (rc) return rc;
  
  int len=0;
  //Convert into Network Byte order-Big Endian before writing
  for(uint32_t i=0; i < getWordLength(); i++) {
   buffer[i] = htonl(buffer[i]);
   if (((i+1)*4) > numBytes) {
    len = numBytes - (i*4);
   } else {
     len = 4;
   }
   o_filestream.write((char *)&buffer[i],len);
   if (o_filestream.fail()) {
      ETRAC0("**** ERROR : Write operation failed.");
      delete[] buffer;
      RETURN_ERROR(ECMD_DBUF_FILE_OPERATION_FAIL); 
   }

  }
  delete[] buffer;
  return rc;
}

uint32_t ecmdDataBuffer::queryNumOfBuffers(const char * i_filename, ecmdFormatType_t i_format, uint32_t &o_num) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  std::ifstream ins;
  uint32_t endOffset = 0, totalFileSz=0;
  bool endFound = false;
  char endKeyword[4];
  
  ins.open(i_filename);
    
  if (ins.fail()) {
    ETRAC1("**** ERROR : Unable to open file : %s for reading",i_filename);
    RETURN_ERROR(ECMD_DBUF_FOPEN_FAIL);  
  }
  
  ins.seekg(0, std::ios::end);
  totalFileSz = ins.tellg();
  if (totalFileSz == 0) {
    ETRAC1("**** ERROR : File : %s is empty",i_filename);
    RETURN_ERROR(ECMD_DBUF_INVALID_ARGS);
  } else {
    ins.seekg(0); // Goto the beginning of the file 
  }
  
  ecmdFormatType_t existingFmt;
  uint32_t begOffset=0;
  if (i_format == ECMD_SAVE_FORMAT_BINARY_DATA) {
    o_num = 1; // Can have only 1 databuffer with this format
    return rc; 
  }
  
  ins.seekg(totalFileSz-8); //get the Begin offset of the offset table
  ins.read((char *)&begOffset,4); begOffset = htonl(begOffset);
  ins.read((char *)&existingFmt,4); existingFmt = (ecmdFormatType_t)htonl(existingFmt);
  if (existingFmt != i_format) {
    ETRAC0("**** ERROR : Format requested does not match up with the file Format.");
    RETURN_ERROR(ECMD_DBUF_INVALID_ARGS);
  }
  //Find out the END offset
  ins.seekg(begOffset+8); //goto the beginning of the table
  while(!endFound) {
    ins.read(endKeyword,4);  
    if (strcmp(endKeyword, "END") == 0) {
      endOffset = ins.tellg(); endOffset -= 4;
      endFound = true;
    }
    if ((uint32_t)ins.tellg() >= totalFileSz) break;
  }
  if (!endFound) {
    ETRAC0("**** ERROR : END keyword not found. Invalid File Format.");
    RETURN_ERROR(ECMD_DBUF_FILE_FORMAT_MISMATCH);
  } else {
    o_num = (endOffset - (begOffset+8)) / 4;
  }
  
  return rc;
  
}

uint32_t ecmdDataBuffer::readFileMultiple(const char * i_filename, ecmdFormatType_t i_format, const char * i_prpty, uint32_t &o_dataNumber) {
  uint32_t *dataOffsets;
  bool propertyMatch = false;
  std::string facNameFromFile; 
  std::string i_property; 
  std::ifstream ins;
  uint32_t property = 0;
  uint32_t endOffset = 0, totalFileSz=0;
  bool endFound = false;
  char key[6], hexstr[8], endKeyword[4], fac[201];
  ecmdFormatType_t format;

  if (i_prpty == NULL) {
    ETRAC0("**** ERROR : property field NULL.");
    RETURN_ERROR(ECMD_DBUF_INVALID_ARGS);
  } else { 
    i_property = i_prpty; 
  }

  if (i_format == ECMD_SAVE_FORMAT_BINARY_DATA) {
    ETRAC0("**** ERROR : File Format ECMD_SAVE_FORMAT_BINARY_DATA not supported when property value is used as input.");
    RETURN_ERROR(ECMD_DBUF_INVALID_ARGS);
  }

  ins.open(i_filename);

  if (ins.fail()) {
    ETRAC1("**** ERROR : Unable to open file : %s for reading",i_filename);
    RETURN_ERROR(ECMD_DBUF_FOPEN_FAIL);
  }

  ins.seekg(0, std::ios::end);
  totalFileSz = ins.tellg();
  if (totalFileSz == 0) {
    ETRAC1("**** ERROR : File : %s is empty",i_filename);
    RETURN_ERROR(ECMD_DBUF_INVALID_ARGS);
  } else {
    ins.seekg(0); // Goto the beginning of the file
  }

  //Read the DataBuffer offset table-Seek to the correct DataBuffer Hdr
    ecmdFormatType_t existingFmt;
    uint32_t begOffset=0 ;

    ins.seekg(totalFileSz-8);//get the Begin offset of the offset table
    ins.read((char *)&begOffset,4); begOffset = htonl(begOffset);
    ins.read((char *)&existingFmt,4); existingFmt = (ecmdFormatType_t)htonl(existingFmt);
    if (existingFmt != i_format) {
      ETRAC0("**** ERROR : Format requested does not match up with the file Format.");
      RETURN_ERROR(ECMD_DBUF_INVALID_ARGS);
    }

    //Find out the END offset
    ins.seekg(begOffset+8); //goto the beginning of the table
    while(!endFound) {
      ins.read(endKeyword,4);
      if (strcmp(endKeyword, "END") == 0) {
        endOffset = ins.tellg(); endOffset -= 4;
        endFound = true;
      }
      if ((uint32_t)ins.tellg() >= totalFileSz) break;
    }
    if (!endFound) {
      ETRAC0("**** ERROR : END keyword not found. Invalid File Format.");
      RETURN_ERROR(ECMD_DBUF_FILE_FORMAT_MISMATCH);
    }
    uint32_t numOfDataBuffers = (endOffset-(begOffset+8))/4;
    dataOffsets = new uint32_t[numOfDataBuffers];

    for (uint32_t i=0; i<numOfDataBuffers; i++) {
      ins.seekg(begOffset+8+(4*i));
      ins.read((char *)&dataOffsets[i],4);  dataOffsets[i] = htonl(dataOffsets[i]);
    }

    /* Look for the property value */
    for (uint32_t i=0; i<numOfDataBuffers; i++) {
      ins.seekg(dataOffsets[i]);
      if (i_format == ECMD_SAVE_FORMAT_BINARY) {
        ins.seekg(12,std::ios::cur);
        ins.read((char *)&format,4);    format = (ecmdFormatType_t)htonl(format);
        ins.read((char *)&property,4);    property = htonl(property);
        if (property == 0x80000000) {
         ins.read(fac, 200);
         fac[200] = '\0';
         facNameFromFile = fac;
         if (facNameFromFile.find(i_prpty, 0, facNameFromFile.length()) != std::string::npos){
           o_dataNumber = i;  
           propertyMatch = true;
           break;
         }
        }
      } else if( (i_format == ECMD_SAVE_FORMAT_ASCII) || (i_format == ECMD_SAVE_FORMAT_XSTATE)) {
        ins.width(6); ins >> key;
        ins.width(9);  ins >> hexstr;
        ins.width(9); ins >> hexstr;
        format = (ecmdFormatType_t) strtoul(hexstr, NULL, 16);
        ins.getline(hexstr, 9);
        if (strcmp(hexstr, "80000000") == 0) {
          ins.getline(fac, 200);
          facNameFromFile = fac;
          if (facNameFromFile.find(i_property.c_str(), 0, facNameFromFile.length()) != std::string::npos){
            o_dataNumber = i;
            propertyMatch = true;
            break;
          }
        }
      }
    }
    delete [] dataOffsets;
    if (!propertyMatch) {
       ETRAC1("**** ERROR : Match for property: %s not found",i_property.c_str());
       RETURN_ERROR(ECMD_DBUF_INVALID_ARGS);
    } else {
      std::string ofac;
      return this->readFileMultiple(i_filename, i_format, o_dataNumber, &ofac);
    }
}

uint32_t ecmdDataBuffer::readFileMultiple(const char * i_filename, ecmdFormatType_t i_format, uint32_t i_dataNumber, std::string *o_facName) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  std::ifstream ins;
  uint32_t numBits = 0, numBytes = 0, hexbitlen = 0, property = 0, *buffer;
  uint32_t endOffset = 0, totalFileSz=0;
  bool endFound = false;
  char key[6], hexstr[8], endKeyword[4], fac[201];

#ifndef REMOVE_SIM
  uint32_t NumDwords = 0;
  char binstr[64];
#endif 

#ifdef REMOVE_SIM
  if (i_format == ECMD_SAVE_FORMAT_XSTATE) {
    ETRAC0( "**** ERROR : ecmdDataBuffer: readFileMultiple: FORMAT_XSTATE not supported in this configuration");
    return(ECMD_DBUF_XSTATE_ERROR);
  }    
#endif
  
  ins.open(i_filename);
    
  if (ins.fail()) {
    ETRAC1("**** ERROR : Unable to open file : %s for reading",i_filename);
    RETURN_ERROR(ECMD_DBUF_FOPEN_FAIL);  
  }
 
  ins.seekg(0, std::ios::end);
  totalFileSz = ins.tellg();
  if (totalFileSz == 0) {
    ETRAC1("**** ERROR : File : %s is empty",i_filename);
    RETURN_ERROR(ECMD_DBUF_INVALID_ARGS);
  } else {
    ins.seekg(0); // Goto the beginning of the file 
  }
  //Read the DataBuffer offset table-Seek to the right DataBuffer Hdr
  if (i_dataNumber != 0) {
    ecmdFormatType_t existingFmt;
    uint32_t begOffset=0, dataOffset;
    if (i_format == ECMD_SAVE_FORMAT_BINARY_DATA) {
      ETRAC0("**** ERROR : File Format ECMD_SAVE_FORMAT_BINARY_DATA not supported when file contains multiple DataBuffers.");
      RETURN_ERROR(ECMD_DBUF_INVALID_ARGS);
    }
    
    ins.seekg(totalFileSz-8);//get the Begin offset of the offset table
    ins.read((char *)&begOffset,4); begOffset = htonl(begOffset);
    ins.read((char *)&existingFmt,4); existingFmt = (ecmdFormatType_t)htonl(existingFmt);
    if (existingFmt != i_format) {
      ETRAC0("**** ERROR : Format requested does not match up with the file Format.");
      RETURN_ERROR(ECMD_DBUF_INVALID_ARGS);
    }
    //Find out the END offset
    ins.seekg(begOffset+8); //goto the beginning of the table
    while(!endFound) {
      ins.read(endKeyword,4);  
      if (strcmp(endKeyword, "END") == 0) {
        endOffset = ins.tellg(); endOffset -= 4;
        endFound = true;
      }
      if ((uint32_t)ins.tellg() >= totalFileSz) break;
    }
    if (!endFound) {
      ETRAC0("**** ERROR : END keyword not found. Invalid File Format.");
      RETURN_ERROR(ECMD_DBUF_FILE_FORMAT_MISMATCH);
    }
    if ((begOffset+8+(4*i_dataNumber)) >= endOffset) {
      ETRAC0("**** ERROR : Data Number requested exceeds the maximum Data Number in the File.");
      RETURN_ERROR(ECMD_DBUF_DATANUMBER_NOT_FOUND);
    }
    ins.seekg(begOffset+8+(4*i_dataNumber));//seek to the right databuffer header
    ins.read((char *)&dataOffset,4);  dataOffset = htonl(dataOffset);
    if (ins.fail()) {
     ETRAC1("**** ERROR : Read of the dataOffset failed on file : %s",i_filename);
     RETURN_ERROR(ECMD_DBUF_FILE_OPERATION_FAIL); 
    }
    ins.seekg(dataOffset);
    if (ins.eof()) {
     ETRAC0("**** ERROR : Data Offset is greater than the file size.");
     RETURN_ERROR(ECMD_DBUF_FILE_OPERATION_FAIL); 
    }
  }
 
  if ( i_format == ECMD_SAVE_FORMAT_BINARY) {
    // Read Hdr
    ins.read(key,5); key[5]='\0';
    if(strcmp(key,"START")!=0) {
      ETRAC0("**** ERROR : Keyword START not found.");
      RETURN_ERROR(ECMD_DBUF_FILE_FORMAT_MISMATCH); 
    }
    ins.seekg(3,std::ios::cur);
    ins.read((char *)&numBits,4);    numBits = htonl(numBits);
    ins.read((char *)&i_format,4);    i_format = (ecmdFormatType_t)htonl(i_format);
    if (i_format != ECMD_SAVE_FORMAT_BINARY ) {
      ETRAC0("**** ERROR : Format mismatch. Expected ECMD_SAVE_FORMAT_BINARY.");
      RETURN_ERROR(ECMD_DBUF_FILE_FORMAT_MISMATCH);  
    }
    ins.read((char *)&property,4);    property = htonl(property);
    if (property == 0x80000000) {
      ins.read(fac, 200);
      fac[200] = '\0'; 
      if (o_facName != NULL)
        *o_facName = fac;
    }
    this->setBitLength(numBits);
    numBytes=getByteLength();
    //Read Data
    buffer = new uint32_t[getWordLength()];
    ins.read((char *)buffer,numBytes);
    if (ins.fail()) {
      ETRAC1("**** ERROR : Read operation in format ECMD_SAVE_FORMAT_BINARY failed on file : %s",i_filename);
      delete[] buffer;
      RETURN_ERROR(ECMD_DBUF_FILE_OPERATION_FAIL); 
    }
    for(uint32_t i=0; i< getWordLength(); i++) {
     buffer[i]=htonl(buffer[i]);
    }
    rc = memCopyIn(buffer, numBytes); 
    delete[] buffer; buffer = NULL;
    if (rc) return rc;
  } else if ( i_format == ECMD_SAVE_FORMAT_BINARY_DATA) {
    ins.seekg(0, std::ios::end);
    numBytes = ins.tellg();
    numBits = numBytes * 8;
    this->setBitLength(numBits);
    ins.seekg(0, std::ios::beg);
    buffer = new uint32_t[getWordLength()];
    ins.read((char *)buffer,numBytes);
    if (ins.fail()) {
      ETRAC1("**** ERROR : Read operation in format ECMD_SAVE_FORMAT_BINARY_DATA failed on file : %s",i_filename);
      delete[] buffer;
      RETURN_ERROR(ECMD_DBUF_FILE_OPERATION_FAIL); 
    }
    for(uint32_t i=0; i< getWordLength(); i++) {
     buffer[i]=htonl(buffer[i]);
    }
    rc = memCopyIn(buffer, numBytes); 
    delete[] buffer; buffer = NULL;
    if (rc) return rc;
  } else if(i_format ==  ECMD_SAVE_FORMAT_ASCII) {
    ins.width(6); ins >> key; 
    if(strcmp(key,"START")!=0) {
      ETRAC0("**** ERROR : Keyword START not found.");
      RETURN_ERROR(ECMD_DBUF_FILE_FORMAT_MISMATCH); 
    }
    ins.width(9);  ins >> hexstr; 
    numBits = strtoul(hexstr, NULL, 16); 
    ins.width(9); ins >> hexstr;  
    i_format = (ecmdFormatType_t) strtoul(hexstr, NULL, 16);
    if (i_format != ECMD_SAVE_FORMAT_ASCII ) {
      ETRAC0("**** ERROR : Format mismatch. Expected ECMD_SAVE_FORMAT_ASCII.");
      RETURN_ERROR(ECMD_DBUF_FILE_FORMAT_MISMATCH);  
    }
    ins.getline(hexstr, 9); 
    if (strcmp(hexstr, "80000000") == 0) {
      ins.getline(fac, 200);
      if (o_facName != NULL)
        *o_facName = fac;
    } 
    this->setBitLength(numBits);
    for (uint32_t i = 0; i < iv_NumWords; i++) {
      ins.width(9);  ins >> hexstr;
      if (((i*32)+32) > numBits) {
        hexstr[strlen(hexstr)] = '\0'; //strip newline char
	hexbitlen = numBits - (i*32);
      } else {
        hexbitlen = 32;
      }
      rc = insertFromHexLeft (hexstr, i*32, hexbitlen); if (rc) return rc;
      ins.seekg(1,std::ios::cur);//Space or Newline char
    }
#ifndef REMOVE_SIM
  } else if( i_format == ECMD_SAVE_FORMAT_XSTATE) {
      /// cje need to enable xstate
    ins.width(6); ins >> key; 
    if(strcmp(key,"START")!=0) {
      ETRAC0("**** ERROR : Keyword START not found.");
      RETURN_ERROR(ECMD_DBUF_FILE_FORMAT_MISMATCH);
    }
    ins.width(9);  ins >> hexstr;
    numBits = strtoul(hexstr, NULL, 16);
    ins.width(9); ins >> hexstr;
    i_format = (ecmdFormatType_t) strtoul(hexstr, NULL, 16);
    if (i_format != ECMD_SAVE_FORMAT_XSTATE ) {
      ETRAC0("**** ERROR : Format mismatch. Expected ECMD_SAVE_FORMAT_XSTATE.");
      RETURN_ERROR(ECMD_DBUF_FILE_FORMAT_MISMATCH);
    }
    ins.getline(hexstr, 9);
    if (strcmp(hexstr, "80000000") == 0) { // String property set
      ins.getline(fac, 200);
      if (o_facName != NULL)
        *o_facName = fac;
    }
    
    this->setBitLength(numBits);
    NumDwords = (iv_NumBits + 63) / 64;
    for (uint32_t i = 0; i < NumDwords; i++) {
      ins.width(65);  ins >> binstr;
      if ((i*64)+64 > numBits) 
        binstr[strlen(binstr)] = '\0'; //strip newline char
      rc = setXstate(i*64, binstr); if (rc) return rc;
      ins.seekg(1,std::ios::cur);// New line char
    }
#endif
  }
  ins.close();
  return(rc);
}

uint32_t ecmdDataBuffer::readFile(const char * i_filename, ecmdFormatType_t i_format, std::string *o_facName) {
  uint32_t dataNumber=0;
  return this->readFileMultiple(i_filename, i_format, dataNumber, o_facName);
}

uint32_t ecmdDataBuffer::readFileStream(std::istream & i_filestream, uint32_t i_bitlength) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  uint32_t numBytes = (i_bitlength + 7) / 8;
  uint32_t *buffer;
  
  this->setBitLength(i_bitlength);
  
  buffer = new uint32_t[getWordLength()];
  i_filestream.read((char *)buffer, numBytes);
  if (i_filestream.fail()) {
      ETRAC0("**** ERROR : Read operation failed.");
      delete[] buffer;
      RETURN_ERROR(ECMD_DBUF_FILE_OPERATION_FAIL); 
  }

  
  for (uint32_t i=0; i< getWordLength(); i++) {
   buffer[i] = htonl(buffer[i]);
  }
  rc = memCopyIn(buffer, numBytes);
  delete[] buffer;
  return rc;
}

uint32_t ecmdDataBuffer::shareBuffer(ecmdDataBuffer* i_sharingBuffer)
{
    uint32_t rc = ECMD_DBUF_SUCCESS;

    if(i_sharingBuffer == NULL)
	return(ECMD_DBUF_INVALID_ARGS);
    //delete data if currently has any
    if (i_sharingBuffer->iv_RealData != NULL)
    {
	i_sharingBuffer->clear();
    }

    //copy the buffer called from minus the owner flag
    i_sharingBuffer->iv_Capacity = iv_Capacity;
    i_sharingBuffer->iv_NumWords = iv_NumWords;
    i_sharingBuffer->iv_NumBits = iv_NumBits;
    i_sharingBuffer->iv_Data = iv_Data;
    i_sharingBuffer->iv_RealData = iv_RealData;
    i_sharingBuffer->iv_UserOwned = false;
#ifndef REMOVE_SIM
    i_sharingBuffer->iv_DataStr = iv_DataStr;
    i_sharingBuffer->iv_XstateEnabled = iv_XstateEnabled;
#endif
    return(rc);
}

void ecmdDataBuffer::queryErrorState( uint32_t & o_errorState) {
  if (iv_RealData != NULL) {
    o_errorState = iv_RealData[EDB_RETURN_CODE];
  } else {
    o_errorState = 0;
  }
}

/* Here is the plan for the compression format

 3 byte header, includes a version
 4 byte length
 Then data the data as returned by the compression algorithm that PRD is kindly letting us use
*/

uint32_t ecmdDataBuffer::compressBuffer() {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  ecmdDataBuffer compressedBuffer;
  uint32_t byteOffset = 0;

  /* Get the length, and make sure it doesn't over flow our length variable */
  uint32_t length = this->getBitLength();

  /* Set it big enough for the header/length below.  Then we'll grow as need */
  compressedBuffer.setBitLength(56);

  /* Set the header, which is C2A3FV, where V is the version */
  compressedBuffer.setByte(byteOffset++, 0xC2);
  compressedBuffer.setByte(byteOffset++, 0xA3);
  compressedBuffer.setByte(byteOffset++, 0xF2);

  /* Set the length, which is 4 bytes long */
  compressedBuffer.setByte(byteOffset++, ((0xFF000000 & length) >> 24));
  compressedBuffer.setByte(byteOffset++, ((0x00FF0000 & length) >> 16));
  compressedBuffer.setByte(byteOffset++, ((0x0000FF00 & length) >> 8));
  compressedBuffer.setByte(byteOffset++, (0x000000FF & length));

  /* Now setup our inputs and call the compress */
  size_t uncompressedSize = this->getByteLength();
  size_t compressedSize = PrdfCompressBuffer::compressedBufferMax(uncompressedSize);
  uint8_t* uncompressedData = new uint8_t[uncompressedSize];
  uint8_t* compressedData = new uint8_t[compressedSize];
  /* The data has to be copied into a uint8_t buffer.  If you try to pass in (uint8_t*)this->iv_Data
   instead of uncompressedData, you have big endian vs little endian issues */
  this->extract(uncompressedData, 0, this->getBitLength());
  
  PrdfCompressBuffer::compressBuffer(uncompressedData, uncompressedSize, compressedData, compressedSize);

  /* Now grow the buffer to the size of the compressed data */
  compressedBuffer.growBitLength((compressedBuffer.getBitLength() + (compressedSize * 8)));

  /* Insert the data and cleanup after ourselves */
  compressedBuffer.insert(compressedData, (byteOffset * 8), (compressedSize * 8));
  delete[] uncompressedData;
  delete[] compressedData;

  /* Finally, copy the compressBuffer into our current buffer */
  *this = compressedBuffer;

  return rc;
}

uint32_t ecmdDataBuffer::uncompressBuffer() {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  uint32_t length = 0;
  ecmdDataBuffer uncompressedBuffer;
  uint32_t byteOffset = 0;

  /* See if the compression header is there */
  uint32_t header = this->getWord(0);
  if ((header & 0xFFFFF000) != 0xC2A3F000) {
    ETRAC1("**** ERROR : Compression header doesn't match.  Found: 0x%X.", header);
    RETURN_ERROR(ECMD_DBUF_INVALID_ARGS); 
  }
  /* Make sure it's a supported version of compression */
  if ((header & 0x00000F00) != 0x00000200) {
    ETRAC1("**** ERROR : Unknown version. Found: 0x%X.", header);
    RETURN_ERROR(ECMD_DBUF_INVALID_ARGS); 
  }
  byteOffset+=3;

  /* Get the length, use it to set the uncompress buffer length */
  // split the following line up to avoid warnings:
  // length = (this->getByte(byteOffset++) << 24) | (this->getByte(byteOffset++) << 16) | (this->getByte(byteOffset++) << 8) | this->getByte(byteOffset++);
  length  = this->getByte(byteOffset++) << 24;
  length |= this->getByte(byteOffset++) << 16;
  length |= this->getByte(byteOffset++) << 8;
  length |= this->getByte(byteOffset++);

  uncompressedBuffer.setBitLength(length);

  /* Setup our inputs and call the uncompress */
  size_t uncompressedSize = uncompressedBuffer.getByteLength();
  size_t compressedSize = this->getByteLength() - byteOffset;
  uint8_t* uncompressedData = new uint8_t[uncompressedSize];
  uint8_t* compressedData = new uint8_t[compressedSize];
  /* The data has to be copied into a uint8_t buffer.  If you try to pass in (uint8_t*)this->iv_Data
   instead of compressedData, you have big endian vs little endian issues */
  this->extract(compressedData, (byteOffset * 8), (compressedSize * 8));

  PrdfCompressBuffer::uncompressBuffer(compressedData, compressedSize, uncompressedData, uncompressedSize);

  /* Error check the length */
  if (uncompressedBuffer.getByteLength() != uncompressedSize) {
    ETRAC2("**** ERROR : Expected byte length of %d, got back %d", uncompressedBuffer.getByteLength(), uncompressedSize);
    RETURN_ERROR(ECMD_DBUF_MISMATCH); 
  }

  /* Insert the data and cleanup after ourselves */
  uncompressedBuffer.insert(uncompressedData, 0, length);
  delete[] uncompressedData;
  delete[] compressedData;

  /* Finally, copy the uncompressBuffer into our current buffer */
  *this = uncompressedBuffer;

  return rc;
}



void * ecmdBigEndianMemCopy(void * dest, const void *src, size_t count)
{
#if defined (i386)
  char *tmp = (char *) dest, *s = (char *) src;
  int remainder = 0;
  uint32_t whole_num = 0;

  remainder = count % 4;
  whole_num = count - remainder;

  /* note: whole_num + remainder = count */

  if (whole_num == count) {
    while (count--) *tmp++ = *s++;
    return dest;
  }
  if (whole_num) {
    while (whole_num--) *tmp++ = *s++;
  }
  if (remainder == 3) {
    tmp[1] = s[1];
    tmp[2] = s[2];
    tmp[3] = s[3];
  }
  else if (remainder == 2) {
    tmp[2] = s[2];
    tmp[3] = s[3];
  }//'constant condition' has been added to avoid BEAM errors
  else if (remainder == 1) { /*constant condition*/
    tmp[3] = s[3];
  }

  return dest;
#else
  return memcpy(dest,src,count);
#endif

}

uint32_t* ecmdDataBufferImplementationHelper::getDataPtr( void* i_buffer ) {
  if (i_buffer == NULL) return NULL;
  ecmdDataBuffer* buff = (ecmdDataBuffer*)i_buffer;
  return buff->iv_Data;
};

void ecmdDataBufferImplementationHelper::applyRawBufferToXstate( void* i_buffer ) {
  if (i_buffer == NULL) return;
#ifndef REMOVE_SIM
  ecmdDataBuffer* buff = (ecmdDataBuffer*)i_buffer;
  if (!buff->iv_XstateEnabled) {
    return;
  }
  strcpy(buff->iv_DataStr,buff->genXstateStr().c_str());
#endif
  return;
}

/********************************************************************************
       These routines belong to derived class ecmdOptimizableDataBuffer
 ********************************************************************************/

/**
 * @brief Default constructor for ecmdOptimizableDataBuffer class
 */
ecmdOptimizableDataBuffer::ecmdOptimizableDataBuffer()
{
   iv_BufferOptimizable = true;     
}

/**
 * @brief Constructor with bit length specified
 */
ecmdOptimizableDataBuffer::ecmdOptimizableDataBuffer(uint32_t i_numBits)
 : ecmdDataBuffer(i_numBits) {
       iv_BufferOptimizable = true;     
}
