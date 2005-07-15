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

/* $Header$ */

                               
// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder     Description                       
//  ---- -------- ---- -------- -----     -----------------------------
//  @01  STG4466       03/10/05 Prahl     Fix up Beam errors
//   
// End Change Log *****************************************************

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

using namespace std;

#include <ecmdDataBuffer.H>

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------
#ifdef FIPSODE
tracDesc_t g_etrc; /** Trace Descriptor **/
TRAC_INIT(&g_etrc, "ECMD", 0x1000);
#endif

//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------
#ifndef DATABUFFER_HEADER
#define DATABUFFER_HEADER 0xBEEFBEEF
#endif

#ifdef ENABLE_MPATROL
#ifdef _AIX
/* This is to fix a missing symbol problem when compiling on aix with mpatrol */
char **p_xargv;
#endif
#endif

/** cje put this in temp to disable xstates completely, will be removed with api changes */
//#define DISABLE_XSTATE

#define RETURN_ERROR(i_rc) if ((iv_RealData != NULL) && (iv_RealData[2] == 0)) { iv_RealData[2] = i_rc; } return i_rc;
#define SET_ERROR(i_rc) if ((iv_RealData != NULL) && (iv_RealData[2] == 0)) { iv_RealData[2] = i_rc; }

//----------------------------------------------------------------------
//  Forward declarations
//----------------------------------------------------------------------
uint32_t ecmdExtract(uint32_t *scr_ptr, uint32_t start_bit_num, uint32_t num_bits_to_extract, uint32_t *out_data_ptr);
void * ecmdBigEndianMemCopy(void * dest, const void *src, size_t count);

//----------------------------------------------------------------------
//  Data Storage Header Format - Example has 4 words
//   Pointer       offset    data
//  real_data  ->  000000   BEEFBEEF <numwords> <errcde> 12345678
//       data  ->  000010   <data>   <data>     <data>   <data>
//             ->  000020   12345678

//---------------------------------------------------------------------
//  Constructors
//---------------------------------------------------------------------
ecmdDataBuffer::ecmdDataBuffer()  // Default constructor
: iv_Capacity(0), iv_NumWords(0), iv_NumBits(0), iv_Data(NULL), iv_RealData(NULL)
{
#ifndef REMOVE_SIM
  iv_DataStr = NULL;
#endif

  iv_UserOwned = true;
  iv_BufferOptimizable = false;
}

ecmdDataBuffer::ecmdDataBuffer(uint32_t numBits)
: iv_Capacity(0), iv_NumWords(0), iv_NumBits(0), iv_Data(NULL), iv_RealData(NULL)
{
  iv_UserOwned = true;
  iv_BufferOptimizable = false;

#ifndef REMOVE_SIM
  iv_DataStr = NULL;
#endif
  if (numBits > 0)
    setBitLength(numBits);

}

ecmdDataBuffer::ecmdDataBuffer(const ecmdDataBuffer& other) 
: iv_Capacity(0), iv_NumWords(0), iv_NumBits(0), iv_Data(NULL), iv_RealData(NULL)
{
#ifndef REMOVE_SIM
  iv_DataStr = NULL;
#endif

  iv_UserOwned = true;
  iv_BufferOptimizable = false;

  if (other.iv_NumBits != 0) {

    this->setBitLength(other.iv_NumBits);
    // iv_Data
    memcpy(iv_Data, other.iv_Data, iv_NumWords * 4);
    // Error state
    iv_RealData[2] = other.iv_RealData[2];


#ifndef REMOVE_SIM
    /// cje This needs to be update to enable/disable xstates in the copy buffer
    if (iv_DataStr != NULL) {
      /* cje enable the xstate in the copy */
      strncpy(iv_DataStr, other.iv_DataStr, iv_NumBits);
    } else if (other.iv_DataStr != NULL) {
      /* cje disable the xstate in the copy */
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

  if ((iv_RealData != NULL)) {
    /* Let's check our header,tail info */
    if ((iv_RealData[0] != DATABUFFER_HEADER) || (iv_RealData[1] != iv_NumWords) || (iv_RealData[3] != iv_RealData[iv_NumWords + 4])) {
      /* Ok, something is wrong here */
      ETRAC3("**** SEVERE ERROR (ecmdDataBuffer) : iv_RealData[0]: %X, iv_RealData[1]: %X, iv_NumWords: %X",iv_RealData[0],iv_RealData[1],iv_NumWords);
      ETRAC2("**** SEVERE ERROR (ecmdDataBuffer) : iv_RealData[3]: %X, iv_RealData[iv_NumWords + 4]: %X",iv_RealData[3],iv_RealData[iv_NumWords + 4]);
      ETRAC0("**** SEVERE ERROR (ecmdDataBuffer) : PROBLEM WITH DATABUFFER - INVALID HEADER/TAIL");
      abort();
    }

    /* That looked okay, reset everything else */
    delete[] iv_RealData;
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
#endif

  return rc;
}


uint32_t   ecmdDataBuffer::getWordLength() const { return iv_NumWords; }
uint32_t   ecmdDataBuffer::getBitLength() const { return iv_NumBits; }
uint32_t   ecmdDataBuffer::getByteLength() const { return iv_NumBits % 8 ? (iv_NumBits / 8) + 1 : iv_NumBits / 8;}
uint32_t   ecmdDataBuffer::getCapacity() const { return iv_Capacity; }

uint32_t  ecmdDataBuffer::setWordLength(uint32_t newNumWords) {

  return setBitLength(newNumWords * 32);;

}  

uint32_t  ecmdDataBuffer::setByteLength(uint32_t newNumBytes) {

  return setBitLength(newNumBytes * 8);;

}  

uint32_t  ecmdDataBuffer::setBitLength(uint32_t newNumBits) {

  uint32_t rc = ECMD_DBUF_SUCCESS;

  if(!iv_UserOwned)
  {
      ETRAC0("**** ERROR (ecmdDataBuffer) : Attempt to modify non user owned buffer size.");
      RETURN_ERROR(ECMD_DBUF_NOT_OWNER);
  }

  if ((newNumBits == 0) && (iv_NumBits == 0)) {
      // Do Nothing:  this data doesn't already have iv_RealData,iv_Data defined, and it doesn't want to define it
      return rc;
  }

  else if ((newNumBits == iv_NumBits) && newNumBits != 0) {
    /* Just clear the buffer */
    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */
#ifndef REMOVE_SIM
    if (iv_DataStr != NULL)
      this->fillDataStr('0'); /* init to 0 */
#endif
    iv_RealData[2] = 0; ///< Reset error code
    return rc;  /* nothing to do */
  }

  uint32_t newNumWords = newNumBits % 32 ? (newNumBits / 32 + 1) : newNumBits / 32;
  uint32_t randNum = 0x12345678;

  iv_NumWords = newNumWords;
  iv_NumBits = newNumBits;

  if (iv_Capacity < newNumWords) {  /* we need to resize iv_Data member */
    if (iv_RealData != NULL)
      delete[] iv_RealData;

    iv_RealData = NULL;

    iv_Capacity = newNumWords;

    iv_RealData = new uint32_t[iv_Capacity + 10];
    if (iv_RealData == NULL) {
      ETRAC0("**** ERROR : ecmdDataBuffer::setBitLength : Unable to allocate memory for new databuffer");
      RETURN_ERROR(ECMD_DBUF_INIT_FAIL);
    }

    iv_Data = iv_RealData + 4;
    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */

#ifndef REMOVE_SIM
    if (iv_DataStr != NULL)
      delete[] iv_DataStr;

    iv_DataStr = NULL;
#ifndef DISABLE_XSTATE
    /* cje Need to see if we really want this buffer */
    /* For now we disable the buffer for anything > 500,000 bits, if a scan ring is that big we have a problem */
    if (iv_NumBits < 500000) {
      iv_DataStr = new char[iv_NumBits + 42];

      if (iv_DataStr == NULL) {
        ETRAC0("**** ERROR : ecmdDataBuffer::setBitLength : Unable to allocate Xstate memory for new databuffer");
        RETURN_ERROR(ECMD_DBUF_INIT_FAIL);
      }

      this->fillDataStr('0'); /* init to 0 */
    }
#endif
#endif


  } else if (iv_NumBits != 0) { /* no need to resize */

    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */

#ifndef REMOVE_SIM
    if (iv_DataStr != NULL)
      this->fillDataStr('0'); /* init to 0 */
#endif

  }
#ifndef REMOVE_SIM
  else if (iv_DataStr != NULL) /* decreasing bit length to zero */
    iv_DataStr[0] = '\0'; 
#endif

  /* Ok, now setup the header, and tail */
  iv_RealData[0] = DATABUFFER_HEADER;
  iv_RealData[1] = iv_NumWords;
  iv_RealData[2] = 0; ///< Reset error code
  iv_RealData[3] = randNum;
  iv_RealData[iv_NumWords + 4] = randNum;

  return rc;
}  

uint32_t ecmdDataBuffer::setCapacity (uint32_t newCapacity) {

  uint32_t rc = ECMD_DBUF_SUCCESS;

  if(!iv_UserOwned)
  {
      ETRAC0("**** ERROR (ecmdDataBuffer) : Attempt to modify non user owned buffer size.");
      RETURN_ERROR(ECMD_DBUF_NOT_OWNER);
  }

 /* only resize to make the capacity bigger */
  if (iv_Capacity < newCapacity) {
    uint32_t randNum = 0x12345678;

    iv_Capacity = newCapacity;
    if (iv_RealData != NULL)
      delete[] iv_RealData;

    iv_RealData = NULL;

    iv_RealData = new uint32_t[iv_Capacity + 10]; 

    if (iv_RealData == NULL) {
      ETRAC0("**** ERROR : ecmdDataBuffer::setCapacity : Unable to allocate memory for new databuffer");
      RETURN_ERROR(ECMD_DBUF_INIT_FAIL);
    }

    iv_Data = iv_RealData + 4;
    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */

    /* Ok, now setup the header, and tail */
    iv_RealData[0] = DATABUFFER_HEADER;
    iv_RealData[1] = iv_NumWords;
    iv_RealData[2] = 0; ///< Reset error code
    iv_RealData[3] = randNum;
    iv_RealData[iv_NumWords + 4] = randNum;

#ifndef REMOVE_SIM
    if (iv_DataStr != NULL)
      delete[] iv_DataStr;

    iv_DataStr = NULL;

#ifndef DISABLE_XSTATE
    /* cje need to see if we want this buffer at all */
    /* For now we disable the buffer for anything > 500,000 bits, if a scan ring is that big we have a problem */
    if (iv_Capacity*32 < 500000) {
      iv_DataStr = new char[(iv_Capacity*32)+42];

      if (iv_DataStr == NULL) {
        ETRAC0("**** ERROR : ecmdDataBuffer::setCapacity : Unable to allocate Xstate memory for new databuffer");
        RETURN_ERROR(ECMD_DBUF_INIT_FAIL);
      }

      this->fillDataStr('0'); /* init to 0 */
    }
#endif
#endif

  }
  return rc;

}

uint32_t ecmdDataBuffer::shrinkBitLength(uint32_t i_newNumBits) {

  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (i_newNumBits > iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::shrinkBitLength: New Bit Length (%d) > current NumBits (%d)", i_newNumBits, iv_NumBits);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
    RETURN_ERROR(rc);
  }
  if (i_newNumBits == iv_NumBits)
    return rc;  /* nothing to do */

  uint32_t newNumWords = i_newNumBits % 32 ? i_newNumBits / 32 + 1 : i_newNumBits / 32;
  uint32_t randNum = 0x12345678;

  iv_NumWords = newNumWords;
  iv_NumBits = i_newNumBits;

  /* Ok, now setup the header, and tail */
  iv_RealData[0] = DATABUFFER_HEADER;
  iv_RealData[1] = iv_NumWords;
  iv_RealData[3] = randNum;
  iv_RealData[iv_NumWords + 4] = randNum;

  return rc;
}

uint32_t ecmdDataBuffer::growBitLength(uint32_t i_newNumBits) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  uint32_t prevwordsize;
  uint32_t prevbitsize;

  if(!iv_UserOwned)
  {
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
  iv_NumWords = i_newNumBits % 32 ? (i_newNumBits / 32) + 1 : i_newNumBits / 32;
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
    if (iv_DataStr != NULL) {
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
    if (rc) return rc;

    /* Restore the data */
    ecmdBigEndianMemCopy(iv_Data, tempBuf, prevbitsize % 8 ? (prevbitsize / 8) + 1 : prevbitsize / 8);
    delete[] tempBuf;

#ifndef REMOVE_SIM
    if (iv_DataStr != NULL) {
      strncpy(iv_DataStr, temp, prevbitsize); // copy back into iv_DataStr
      delete[] temp;
    }
#endif

    /* Clear any odd bits in the byte */
    for (uint32_t idx = prevbitsize; (idx < iv_NumBits) && (idx % 8); idx ++) {
      clearBit(idx);
    }

  } else {
    /* Didn't need to resize, but need to clear new space added */
    /* Clear any odd bits in a byte */
    uint32_t idx;
    for (idx = prevbitsize; (idx < iv_NumBits) && (idx % 8); idx ++) {
      clearBit(idx);
    }
    /* memset the rest */
    memset(&(((uint8_t*)iv_Data)[idx/8]), 0, iv_NumBits % 8 ? (iv_NumBits / 8) + 1 - (idx/8): iv_NumBits / 8 - (idx/8)); /* init to 0 */
#ifndef REMOVE_SIM
    if (iv_DataStr != NULL) {
      memset(&(iv_DataStr[idx]), '0', (iv_NumBits - idx) ); /* init to 0 */
    }
#endif
  }    

  iv_RealData[1] = iv_NumWords;
  iv_RealData[2] = 0;  // error state
  iv_RealData[iv_NumWords + 4] = 0x12345678;
  return rc;
}


uint32_t  ecmdDataBuffer::setBit(uint32_t bit) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (bit >= iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::setBit: bit %d >= NumBits (%d)", bit, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    int index = bit/32;
    iv_Data[index] |= 0x00000001 << (31 - (bit-(index * 32)));
#ifndef REMOVE_SIM
    if (iv_DataStr != NULL) {
      iv_DataStr[bit] = '1';
    }
#endif
  }
  return rc;
}

uint32_t  ecmdDataBuffer::setBit(uint32_t bit, uint32_t len) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (bit+len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::setBit: bit %d + len %d > NumBits (%d)", bit, len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    for (uint32_t idx = 0; idx < len; idx ++) rc |= this->setBit(bit + idx);    
  }
  return rc;
}


uint32_t ecmdDataBuffer::writeBit(uint32_t i_bit, uint32_t i_value) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (i_value) rc = setBit(i_bit);
  else rc = clearBit(i_bit);
  return rc;
}




uint32_t  ecmdDataBuffer::setWord(uint32_t wordOffset, uint32_t value) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (wordOffset >= iv_NumWords) {
    ETRAC2("**** ERROR : ecmdDataBuffer::setWord: wordoffset %d >= NumWords (%d)", wordOffset, iv_NumWords);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    iv_Data[wordOffset] = value;
    
#ifndef REMOVE_SIM
    if (iv_DataStr != NULL) {
      int startBit = wordOffset * 32;
      uint32_t mask = 0x80000000;
      for (int i = 0; i < 32; i++) {
        if (value & mask) {
          iv_DataStr[startBit+i] = '1';
        }
        else {
          iv_DataStr[startBit+i] = '0';
        }
        
        mask >>= 1;
      }
    }
#endif

  }
  return rc;
}

uint32_t  ecmdDataBuffer::setByte(uint32_t byteOffset, uint8_t value) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (byteOffset >= getByteLength()) {
    ETRAC2("**** ERROR : ecmdDataBuffer::setByte: byteOffset %d >= NumBytes (%d)", byteOffset, getByteLength());
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
#if defined (i386)
    ((uint8_t*)(this->iv_Data))[byteOffset^3] = value;
#else
    ((uint8_t*)(this->iv_Data))[byteOffset] = value;
#endif
    
#ifndef REMOVE_SIM
    if (iv_DataStr != NULL) {
      int startBit = byteOffset * 8;
      uint8_t mask = 0x80;
      for (int i = 0; i < 8; i++) {
        if (value & mask) {
          iv_DataStr[startBit+i] = '1';
        }
        else {
          iv_DataStr[startBit+i] = '0';
        }
        
        mask >>= 1;
      }
    }
#endif

  }
  return rc;
}

uint8_t ecmdDataBuffer::getByte(uint32_t byteOffset) const {
  if (byteOffset >= getByteLength()) {
    ETRAC2("**** ERROR : ecmdDataBuffer::getByte: byteOffset %d >= NumBytes (%d)", byteOffset, getByteLength());
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return 0;
  }
#if defined (i386)
  return ((uint8_t*)(this->iv_Data))[byteOffset^3];
#else
  return ((uint8_t*)(this->iv_Data))[byteOffset];
#endif
}


uint32_t  ecmdDataBuffer::setHalfWord(uint32_t i_halfwordoffset, uint16_t i_value) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (i_halfwordoffset >= (getWordLength()*2)) {
    ETRAC2("**** ERROR : ecmdDataBuffer::setHalfWord: halfWordOffset %d >= NumHalfWords (%d)", i_halfwordoffset, (getWordLength()*2));
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }
  uint32_t value32 = (uint32_t)i_value;
  if (i_halfwordoffset % 2) {
    iv_Data[i_halfwordoffset/2] = (iv_Data[i_halfwordoffset/2] & 0xFFFF0000) | (value32 & 0x0000FFFF);
  } else {
    iv_Data[i_halfwordoffset/2] = (iv_Data[i_halfwordoffset/2] & 0x0000FFFF) | ((value32 << 16) & 0xFFFF0000);
  }

#ifndef REMOVE_SIM
  if (iv_DataStr != NULL) {
    int startBit = i_halfwordoffset * 16;
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
  if (i_halfwordoffset >= (getWordLength()*2)) {
    ETRAC2("**** ERROR : ecmdDataBuffer::getHalfWord: halfWordOffset %d >= NumHalfWords (%d)", i_halfwordoffset, (getWordLength()*2));
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

uint32_t  ecmdDataBuffer::setDoubleWord(uint32_t i_doublewordoffset, uint64_t i_value) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (i_doublewordoffset >= (getWordLength()/2)) {
    ETRAC2("**** ERROR : ecmdDataBuffer::setDoubleWord: doubleWordOffset %d >= NumDoubleWords (%d)", i_doublewordoffset, (getWordLength()/2));
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  }
  uint32_t hivalue = (uint32_t)((i_value & 0xFFFFFFFF00000000ull) >> 32);
  uint32_t lovalue = (uint32_t)((i_value & 0x00000000FFFFFFFFull));

  iv_Data[i_doublewordoffset*2] = hivalue;
  iv_Data[(i_doublewordoffset*2)+1] = lovalue;

  
#ifndef REMOVE_SIM
  if (iv_DataStr != NULL) {
    int startBit = i_doublewordoffset * 64;
    uint64_t mask = 0x8000000000000000ull;
    for (int i = 0; i < 64; i++) {
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
  if (i_doublewordoffset >= (getWordLength()/2)) {
    ETRAC2("**** ERROR : ecmdDataBuffer::getDoubleWord: doubleWordOffset %d >= NumDoubleWords (%d)", i_doublewordoffset, (getWordLength()/2));
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return 0;
  }
  uint64_t ret;
  ret = ((uint64_t)(iv_Data[i_doublewordoffset*2])) << 32;
  ret |= iv_Data[(i_doublewordoffset*2)+1];
  return ret;
}

uint32_t  ecmdDataBuffer::clearBit(uint32_t bit) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (bit >= iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::clearBit: bit %d >= NumBits (%d)", bit, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {  
    int index = bit/32;
    iv_Data[index] &= ~(0x00000001 << (31 - (bit-(index * 32))));
#ifndef REMOVE_SIM
    if (iv_DataStr != NULL) {
      iv_DataStr[bit] = '0';
    }
#endif
  }
  return rc;
}

uint32_t  ecmdDataBuffer::clearBit(uint32_t bit, uint32_t len) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (bit+len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::clearBit: bit %d + len %d > NumBits (%d)", bit, len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    for (uint32_t idx = 0; idx < len; idx ++) rc |= this->clearBit(bit + idx);    
  }
  return rc;
}

uint32_t  ecmdDataBuffer::flipBit(uint32_t bit) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (bit >= iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::flipBit: bit %d >= NumBits (%d)", bit, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
#ifndef REMOVE_SIM
    if ((iv_DataStr != NULL) && this->hasXstate(bit, 1)) {
      ETRAC1("**** ERROR : ecmdDataBuffer::flipBit: cannot flip non-binary data at bit %d", bit);
      RETURN_ERROR(ECMD_DBUF_XSTATE_ERROR);
    } else {
#endif
      if (this->isBitSet(bit)) {
        rc = this->clearBit(bit);      
      } else {
        rc = this->setBit(bit);
      }
#ifndef REMOVE_SIM
    }
#endif
  }
  return rc;
}

uint32_t  ecmdDataBuffer::flipBit(uint32_t bit, uint32_t len) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (bit+len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::flipBit: bit %d + len %d > NumBits (%d)", bit, len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    for (uint32_t i = 0; i < len; i++) {
      this->flipBit(bit+i);
    }
  }
  return rc;
}

bool   ecmdDataBuffer::isBitSet(uint32_t bit) const {
  if (bit >= iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::isBitSet: bit %d >= NumBits (%d)", bit, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return false;
  } else {
#ifndef REMOVE_SIM
    if (iv_DataStr != NULL) {
      if (iv_DataStr[bit] != '1' && iv_DataStr[bit] != '0') {
        ETRAC1("**** ERROR : ecmdDataBuffer::isBitSet: non-binary character detected in data at bit %d", bit);
        SET_ERROR(ECMD_DBUF_XSTATE_ERROR);
        return false;
      }
    }
#endif
    int index = bit/32;
    return (iv_Data[index] & 0x00000001 << (31 - (bit-(index * 32)))); 
  }
  return false;
}

bool   ecmdDataBuffer::isBitSet(uint32_t bit, uint32_t len) const {
  if (bit+len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::isBitSet: bit %d + len %d > NumBits (%d)", bit, len, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return false;
  } else {
    bool rc = true;
    for (uint32_t i = 0; i < len; i++) {
      if (!this->isBitSet(bit + i)) {
        rc = false;
        break;
      }
    }
    return rc;
  }
  return false;
}

bool   ecmdDataBuffer::isBitClear(uint32_t bit) const {
  if (bit >= iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::isBitClear: bit %d >= NumBits (%d)", bit, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return false;
  } else {
#ifndef REMOVE_SIM
    if (iv_DataStr != NULL) {
      if (iv_DataStr[bit] != '1' && iv_DataStr[bit] != '0') {
        ETRAC0( "**** ERROR : ecmdDataBuffer::isBitClear: non-binary character detected in data string");
        SET_ERROR(ECMD_DBUF_XSTATE_ERROR);
        return false;
      }
    }
#endif
    int index = bit/32;
    return (!(iv_Data[index] & 0x00000001 << (31 - (bit-(index * 32))))); 
  }
  return false;
}

bool   ecmdDataBuffer::isBitClear(uint32_t bit, uint32_t len) const
{
  if (bit+len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::isBitClear: bit %d + len %d > NumBits (%d)", bit, len, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return false;
  } else {
    int rc = true;
    for (uint32_t i = 0; i < len; i++) {
      if (!this->isBitClear(bit + i)) {
        rc = false;
        break;
      }
    }
    return rc;
  }
  return false;
}

uint32_t   ecmdDataBuffer::getNumBitsSet(uint32_t bit, uint32_t len) const {
  if (bit+len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::getNumBitsSet: bit %d + len %d > NumBits (%d)", bit, len, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return 0;
  } else {
    int count = 0;
    for (uint32_t i = 0; i < len; i++) {
      if (this->isBitSet(bit + i)) count++;
    }
    return count;
  }
}

uint32_t   ecmdDataBuffer::shiftRight(uint32_t shiftNum) {

  uint32_t rc = ECMD_DBUF_SUCCESS;

  uint32_t thisCarry;
  uint32_t prevCarry = 0x00000000;

  uint32_t i;

  /* If we are going to shift off the end we can just clear everything out */
  if (shiftNum >= iv_NumBits) {
    rc = flushTo0();
    return rc;
  }


  // shift iv_Data array
  for (uint32_t iter = 0; iter < shiftNum; iter++) {
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
  if (iv_DataStr != NULL) {
    // shift char
    char* temp = new char[iv_NumBits+42];
    for (i = 0; i < shiftNum+1; i++) temp[i] = '0'; // backfill with zeros
    temp[iv_NumBits] = '\0';
    strncpy(&temp[shiftNum], iv_DataStr, iv_NumBits-shiftNum);  
    strcpy(iv_DataStr, temp); // copy back into iv_DataStr
    delete[] temp;
  }
#endif
  return rc;
}

uint32_t   ecmdDataBuffer::shiftLeft(uint32_t shiftNum) {

  uint32_t rc = ECMD_DBUF_SUCCESS;

  uint32_t thisCarry;
  uint32_t prevCarry = 0x00000000;
  int i;

  /* If we are going to shift off the end we can just clear everything out */
  if (shiftNum >= iv_NumBits) {
    rc = flushTo0();
    return rc;
  }

  // shift iv_Data array
  for (uint32_t iter = 0; iter < shiftNum; iter++) {
    prevCarry = 0;
    for (i = iv_NumWords-1; i >= 0; i--) {

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
  if (iv_DataStr != NULL) {
    // shift char
    char* temp = new char[iv_NumBits+42];
    for (uint32_t j = iv_NumBits - shiftNum - 1; j < iv_NumBits; j++) temp[j] = '0'; // backfill with zeros
    temp[iv_NumBits] = '\0';
    strncpy(temp, &iv_DataStr[shiftNum], iv_NumBits - shiftNum);  
    strcpy(iv_DataStr, temp); // copy back into iv_DataStr
    delete[] temp;
  }
#endif
  return rc;

}


uint32_t   ecmdDataBuffer::shiftRightAndResize(uint32_t shiftNum) {

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
  iv_NumWords = (iv_NumBits + shiftNum + 31) / 32;
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
    if (iv_DataStr != NULL) {
      temp = new char[iv_NumBits+42];
      if (temp == NULL) {
        ETRAC0("**** ERROR : ecmdDataBuffer::shiftRightAndResize : Unable to allocate temp X-State buffer");
        RETURN_ERROR(ECMD_DBUF_INIT_FAIL);
      }
      strncpy(temp, iv_DataStr, iv_NumBits);
    }
#endif
    /* Now resize with the new capacity */
    rc = setCapacity(iv_NumWords);
    if (rc) return rc;

    /* Restore the data */
    memcpy(iv_Data, tempBuf, prevlen * 4);
    delete[] tempBuf;

#ifndef REMOVE_SIM
    if (iv_DataStr != NULL) {
      strncpy(iv_DataStr, temp, iv_NumBits); // copy back into iv_DataStr
      delete[] temp;
    }
#endif
  }

  iv_RealData[1] = iv_NumWords;
  iv_RealData[iv_NumWords + 4] = 0x12345678;

  // shift iv_Data array
  if (!(shiftNum % 32)) {
    /* We will do this faster if we are shifting nice word boundaries */
    int numwords = shiftNum / 32;

    for (int witer = (int)iv_NumWords - (int)numwords - 1; witer >= 0; witer --) {
      iv_Data[witer + numwords] = iv_Data[witer];
    }
    /* Zero out the bottom of the array */
    for (int w = 0; w < numwords; w ++) iv_Data[w] = 0;

  } else {
    for (uint32_t iter = 0; iter < shiftNum; iter++) {
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

  iv_NumBits += shiftNum;

#ifndef REMOVE_SIM
  if (iv_DataStr != NULL) {
    // shift char
    char* temp = new char[iv_NumBits+42];
    if (temp == NULL) {
      ETRAC0("**** ERROR : ecmdDataBuffer::shiftRightAndResize : Unable to allocate temp X-State buffer");
      RETURN_ERROR(ECMD_DBUF_INIT_FAIL);
    }
    for (i = 0; i < shiftNum; i++) temp[i] = '0'; // backfill with zeros
    temp[iv_NumBits] = '\0';
    strncpy(&temp[shiftNum], iv_DataStr, iv_NumBits-shiftNum);  
    strcpy(iv_DataStr, temp); // copy back into iv_DataStr
    delete[] temp;
  }
#endif
  return rc;
}

uint32_t   ecmdDataBuffer::shiftLeftAndResize(uint32_t shiftNum) {

  uint32_t rc = ECMD_DBUF_SUCCESS;

  uint32_t thisCarry;
  uint32_t prevCarry = 0x00000000;
  int i;

  if(!iv_UserOwned)
  {
      ETRAC0("**** ERROR (ecmdDataBuffer) : Attempt to modify non user owned buffer size.");
      RETURN_ERROR(ECMD_DBUF_NOT_OWNER);
  }

  /* If we are going to shift off the end we can just clear everything out */
  if (shiftNum >= iv_NumBits) {
    rc = setBitLength(0);
    return rc;
  }

  // shift iv_Data array
  for (uint32_t iter = 0; iter < shiftNum; iter++) {
    prevCarry = 0;
    for (i = (int)iv_NumWords-1; i >= 0; i--) {

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
  iv_NumBits -= shiftNum;
  iv_NumWords = (iv_NumBits +31) / 32;
  iv_RealData[1] = iv_NumWords;
  iv_RealData[iv_NumWords + 4] = 0x12345678;

#ifndef REMOVE_SIM
  if (iv_DataStr != NULL) {
    // shift char
    char* temp = new char[iv_NumBits+42];
    if (temp == NULL) {
      ETRAC0("**** ERROR : ecmdDataBuffer::shiftLeftAndResize : Unable to allocate temp X-State buffer");
      RETURN_ERROR(ECMD_DBUF_INIT_FAIL);
    }
    temp[iv_NumBits] = '\0';
    strncpy(temp, &iv_DataStr[shiftNum], iv_NumBits);  
    strcpy(iv_DataStr, temp); // copy back into iv_DataStr
    delete[] temp;
  }
#endif
  return rc;
}

uint32_t  ecmdDataBuffer::rotateRight(uint32_t rotateNum) {

  uint32_t rc = ECMD_DBUF_SUCCESS;

  int lastBitSet;
  // rotate iv_Data
  for (uint32_t iter = 0; iter < rotateNum; iter++) {
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

uint32_t  ecmdDataBuffer::rotateLeft(uint32_t rotateNum) {

  uint32_t rc = ECMD_DBUF_SUCCESS;

  int firstBitSet;
  // rotate iv_Data
  for (uint32_t iter = 0; iter < rotateNum; iter++) {
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

uint32_t  ecmdDataBuffer::flushTo0() {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (iv_NumWords > 0) {
    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */
#ifndef REMOVE_SIM
    if (iv_DataStr != NULL)
      rc = this->fillDataStr('0');
#endif
  }
  return rc;
}

uint32_t  ecmdDataBuffer::flushTo1() {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (iv_NumWords > 0) {
    for (uint32_t i = 0; i < iv_NumWords; i++) iv_Data[i] = 0xFFFFFFFF;
#ifndef REMOVE_SIM   
    if (iv_DataStr != NULL)
      rc = this->fillDataStr('1');
#endif
  }
  return rc;
}

uint32_t ecmdDataBuffer::invert() { 
  uint32_t rc = ECMD_DBUF_SUCCESS;
  rc = this->flipBit(0, iv_NumBits);
  return rc;
}

uint32_t ecmdDataBuffer::reverse() {
  /* For now we will just do this bit by bit */
  uint32_t rc = ECMD_DBUF_SUCCESS;
  int middle = iv_NumBits / 2;
  for (int i = 0; i < middle; i ++ ) {
    if (isBitSet(i)) {
      if (isBitSet(iv_NumBits - 1 - i)) {
        rc = setBit(i);
      } else {
        rc = clearBit(i);
      }
      if (rc) break;
      rc = setBit(iv_NumBits - 1 - i);
    } else {
      if (isBitSet(iv_NumBits - 1 - i)) {
        rc = setBit(i);
      } else {
        rc = clearBit(i);
      }
      if (rc) break;
      rc = clearBit(iv_NumBits - 1 - i);
    }
    if (rc) break;
  } /* end for */
  return rc;
}

uint32_t ecmdDataBuffer::applyInversionMask(const ecmdDataBuffer & i_invMaskBuffer, uint32_t i_invByteLen) {
  return applyInversionMask(i_invMaskBuffer.iv_Data, i_invMaskBuffer.getByteLength());
}


uint32_t ecmdDataBuffer::applyInversionMask(const uint32_t * i_invMask, uint32_t i_invByteLen) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  /* Do the smaller of data provided or size of buffer */
  int wordlen = (i_invByteLen / 4) + 1 < iv_NumWords ? (i_invByteLen / 4) + 1 : iv_NumWords;

  for (int i = 0; i < wordlen; i++) {
    iv_Data[i] = iv_Data[i] ^ i_invMask[i]; /* Xor */
  }
     
#ifndef REMOVE_SIM   
  if (iv_DataStr != NULL) {
    int xbuf_size = (i_invByteLen * 8) < iv_NumBits ? (i_invByteLen * 8) : iv_NumBits;
    int curbit = 0;

    for (int word = 0; word < wordlen; word ++) {
      for (int bit = 0; bit < 32; bit ++) {
        
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


uint32_t  ecmdDataBuffer::insert(const ecmdDataBuffer &i_bufferIn, uint32_t i_targetStart, uint32_t i_len, uint32_t i_sourceStart) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (i_sourceStart + i_len > i_bufferIn.iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::insert: i_sourceStart %d + i_len %d > i_bufferIn.iv_NumBits (%d)\n", i_sourceStart, i_len, i_bufferIn.iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {

    rc = this->insert(i_bufferIn.iv_Data, i_targetStart, i_len, i_sourceStart);
    /* Now apply the Xstate stuff */
#ifndef REMOVE_SIM   
    /* Do we want to disable xstate here ? */
    if ((iv_DataStr != NULL) && (i_bufferIn.iv_DataStr != NULL)) {
      if (i_targetStart+i_len <= iv_NumBits) {
        strncpy(&(iv_DataStr[i_targetStart]), (i_bufferIn.genXstateStr(i_sourceStart, i_len)).c_str(), i_len);
      }
    }
#endif
  }
  return rc;
}

uint32_t  ecmdDataBuffer::insert(const uint32_t *i_dataIn, uint32_t i_targetStart, uint32_t i_len, uint32_t i_sourceStart) {
  uint32_t rc = ECMD_DBUF_SUCCESS;


  if (i_targetStart+i_len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::insert: i_targetStart %d + i_len %d > iv_NumBits (%d)", i_targetStart, i_len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    
    uint32_t mask = 0x80000000 >> (i_sourceStart % 32);
    const uint32_t * sourcePtr = i_dataIn;
    for (uint32_t i = 0; i < i_len; i++) {
      if (sourcePtr[(i+i_sourceStart)/32] & mask) {
        rc = this->setBit(i_targetStart+i);
      }
      else { 
        rc = this->clearBit(i_targetStart+i);
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

uint32_t  ecmdDataBuffer::insert(uint32_t i_dataIn, uint32_t i_targetStart, uint32_t i_len, uint32_t i_sourceStart) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if ( i_sourceStart + i_len > 32 ) {
    ETRAC2("**** ERROR : ecmdDataBuffer::insert: i_sourceStart %d + i_len %d > sizeof i_dataIn (32)\n", i_sourceStart, i_len );
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {

    rc = this->insert(&i_dataIn, i_targetStart, i_len, i_sourceStart);
  }
  return rc;
}

uint32_t  ecmdDataBuffer::insertFromRight(const uint32_t * i_datain, uint32_t i_start, uint32_t i_len) {

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
  } else {
    
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
  }
  return rc;
}
uint32_t  ecmdDataBuffer::insertFromRight(uint32_t i_datain, uint32_t i_start, uint32_t i_len) {
  return this->insertFromRight(&i_datain, i_start, i_len);
}

uint32_t ecmdDataBuffer::extract(ecmdDataBuffer& bufferOut, uint32_t start, uint32_t len) const {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if ((start > iv_NumBits) || (start + len > iv_NumBits)) {
    ETRAC2( "**** ERROR : ecmdDataBuffer::extract: start + len %d > NumBits (%d)", start + len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    rc = bufferOut.setBitLength(len);
    if (rc) return rc;

    rc = ecmdExtract(this->iv_Data, start, len, bufferOut.iv_Data);
    if (rc) {
      RETURN_ERROR(rc);
    }


#ifndef REMOVE_SIM   
    if (iv_DataStr != NULL) {
      if (start+len <= iv_NumBits) {
        strncpy(bufferOut.iv_DataStr, (genXstateStr(start, len)).c_str(), len);
        bufferOut.iv_DataStr[len] = '\0';
      }
    }
#endif
  }
  return rc;
}

uint32_t ecmdDataBuffer::extract(uint32_t *dataOut, uint32_t start, uint32_t len) const {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (start + len > iv_NumBits) {
    ETRAC2( "**** ERROR : ecmdDataBuffer::extract: start + len %d > NumBits (%d)\n", start + len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {

    rc = ecmdExtract(this->iv_Data, start, len, dataOut);
    if (rc) {
      RETURN_ERROR(rc);
    }

#ifndef REMOVE_SIM
    if (iv_DataStr != NULL) {
      /* If we are using this interface and find Xstate data we have a problem */
      if (hasXstate(start, len)) {
        ETRAC0("**** WARNING : ecmdDataBuffer::extract: Cannot extract when non-binary (X-State) character      present\n");
        RETURN_ERROR(ECMD_DBUF_XSTATE_ERROR);
      }
    }
#endif
  }
  return rc;
}

// extractPreserve() takes data from current and inserts it in the passed in
//  buffer at a given offset. This is the same as insert() with the args and
//  the data flow reversed, so insert() is called to do the work
uint32_t ecmdDataBuffer::extractPreserve(ecmdDataBuffer & bufferOut, uint32_t start, uint32_t len, uint32_t targetStart) const {
  return bufferOut.insert( *this, targetStart, len, start );
}

// extractPreserve() with a generic data buffer is hard to work on, so the
// output buffer is first copied into an ecmdDataBuffer object, then insert()
// is called to do the work
uint32_t ecmdDataBuffer::extractPreserve(uint32_t *outBuffer, uint32_t start, uint32_t len, uint32_t targetStart) const {
  
  uint32_t rc = ECMD_DBUF_SUCCESS;

  const uint32_t numWords = ( targetStart + len + 31 ) / 32;
  if ( numWords == 0 ) return rc;

  ecmdDataBuffer *tempBuf = new ecmdDataBuffer;

  if ( NULL == tempBuf ) {
      ETRAC0("**** ERROR : ecmdDataBuffer::extractPreserve : Unable to allocate memory for new databuffer\n");
      RETURN_ERROR(ECMD_DBUF_INIT_FAIL);
  } 

  rc = tempBuf->setWordLength( numWords );

  if ( rc == ECMD_DBUF_SUCCESS ) 
    rc = tempBuf->memCopyIn( outBuffer, numWords * 4);

  if ( rc == ECMD_DBUF_SUCCESS ) 
    rc = tempBuf->insert( *this, targetStart, len, start);

  if ( rc == ECMD_DBUF_SUCCESS ) 
    rc = tempBuf->memCopyOut( outBuffer, numWords * 4);

  delete tempBuf;
  return rc;

}

uint32_t ecmdDataBuffer::extractToRight(ecmdDataBuffer & o_bufferOut, uint32_t i_start, uint32_t i_len) const {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  rc = this->extract(o_bufferOut, i_start, i_len);
  if (rc) return rc;

  if (i_len < 32)
    rc = o_bufferOut.shiftRightAndResize(32 - i_len);
  return rc;
}

uint32_t ecmdDataBuffer::extractToRight(uint32_t * o_data, uint32_t i_start, uint32_t i_len) const {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  rc = this->extract(o_data, i_start, i_len);

  if (i_len < 32)
    *o_data >>= 32 - i_len;
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

uint32_t ecmdDataBuffer::setOr(const ecmdDataBuffer& bufferIn, uint32_t startBit, uint32_t len) {
  return this->setOr(bufferIn.iv_Data, startBit, len);
}

uint32_t ecmdDataBuffer::setOr(const uint32_t * dataIn, uint32_t startBit, uint32_t len) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (startBit + len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::setOr: bit %d + len %d > NumBits (%d)", startBit, len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    uint32_t mask = 0x80000000;
    for (uint32_t i = 0; i < len; i++) {
      if (dataIn[i/32] & mask) {
        rc = this->setBit(startBit + i);
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

uint32_t ecmdDataBuffer::setOr(uint32_t dataIn, uint32_t startBit, uint32_t len) {
  return this->setOr(&dataIn, startBit, len);
}

uint32_t ecmdDataBuffer::setXor(const ecmdDataBuffer& bufferIn, uint32_t startBit, uint32_t len) {
  return this->setXor(bufferIn.iv_Data, startBit, len);
}

uint32_t ecmdDataBuffer::setXor(const uint32_t * dataIn, uint32_t startBit, uint32_t len) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (startBit + len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::setOr: bit %d + len %d > NumBits (%d)", startBit, len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    uint32_t mask = 0x80000000;
    for (uint32_t i = 0; i < len; i++) {
      rc = this->writeBit(startBit + i, ((dataIn[i/32] & mask) ^ (this->iv_Data[i/32] & mask)));
      mask >>= 1;
      if (mask == 0x00000000) {
        mask = 0x80000000;
      }
      if (rc) break;
    }
  }
  return rc;
}

uint32_t ecmdDataBuffer::setXor(uint32_t dataIn, uint32_t startBit, uint32_t len) {
  return this->setXor(&dataIn, startBit, len);
}

uint32_t ecmdDataBuffer::merge(const ecmdDataBuffer& bufferIn) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (iv_NumBits != bufferIn.iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::merge: NumBits in (%d) do not match NumBits (%d)", bufferIn.iv_NumBits, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    return this->setOr(bufferIn, 0, iv_NumBits);
  }
  return rc;
}

uint32_t ecmdDataBuffer::setAnd(const ecmdDataBuffer& bufferIn, uint32_t startBit, uint32_t len) {
  return this->setAnd(bufferIn.iv_Data, startBit, len);
}

uint32_t ecmdDataBuffer::setAnd(const uint32_t * dataIn, uint32_t startBit, uint32_t len) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (startBit + len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::setAnd: bit %d + len %d > iv_NumBits (%d)", startBit, len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    uint32_t mask = 0x80000000;
    for (uint32_t i = 0; i < len; i++) {
      if (!(dataIn[i/32] & mask)) {
        this->clearBit(startBit + i);
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

uint32_t ecmdDataBuffer::setAnd(uint32_t dataIn, uint32_t startBit, uint32_t len) {
  return this->setAnd(&dataIn, startBit, len);
}

uint32_t   ecmdDataBuffer::oddParity(uint32_t start, uint32_t stop) const {

  int charOffset;
  int posOffset;
  uint32_t counter;
  int parity = 1;
  uint32_t mask;

  charOffset = start / 32;
  posOffset = start - charOffset * 32;
  mask = 0x80000000 >> posOffset;

  for (counter = 0; counter < (stop - start + 1); counter++) {
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

  return parity;

}

uint32_t   ecmdDataBuffer::evenParity(uint32_t start, uint32_t stop) const {
  if (this->oddParity(start, stop))
    return 0;
  else
    return 1;
}

uint32_t   ecmdDataBuffer::oddParity(uint32_t start, uint32_t stop, uint32_t insertPos) {
  if (this->oddParity(start,stop))
    this->setBit(insertPos);
  else 
    this->clearBit(insertPos);
  return 0;
}

uint32_t   ecmdDataBuffer::evenParity(uint32_t start, uint32_t stop, uint32_t insertPos) {
  if (this->evenParity(start,stop))
    this->setBit(insertPos);
  else
    this->clearBit(insertPos);
  return 0;
}

uint32_t ecmdDataBuffer::getWord(uint32_t wordOffset) const {
  if (wordOffset >= iv_NumWords) {
    ETRAC2("**** ERROR : ecmdDataBuffer::getWord: wordOffset %d >= NumWords (%d)", wordOffset, iv_NumWords);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return 0;
  }
  return this->iv_Data[wordOffset];
}

std::string ecmdDataBuffer::genHexLeftStr(uint32_t start, uint32_t bitLen) const {

  int tempNumWords = (bitLen + 31) / 32;
  std::string ret;
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (start+bitLen > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::genHexLeftStr: bit %d + len %d >= NumBits (%d)", start, bitLen, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return ret;
  }

  char cPtr[10];
  /* extract iv_Data */
  ecmdDataBuffer tmpBuffer(tempNumWords*32);
  tmpBuffer.flushTo0();

  rc = this->extract(tmpBuffer, start, bitLen);
  if (rc) {
    return ret;
  }    

  for (int w = 0; w < tempNumWords; w++) {
    sprintf(cPtr, "%.8X", tmpBuffer.getWord(w));
    ret.append(cPtr);
  }

  int overCount = (int)(32*tempNumWords - bitLen) / 4;

  if (overCount > 0) {
    ret.erase(ret.length() - overCount, ret.length()-1);
  }

#ifndef REMOVE_SIM
  /* If we are using this interface and find Xstate data we have a problem */
  if ((iv_DataStr != NULL) && hasXstate(start, bitLen)) {
    ETRAC0("**** WARNING : ecmdDataBuffer::genHexLeftStr: Cannot extract when non-binary (X-State) character present");
    SET_ERROR(ECMD_DBUF_XSTATE_ERROR);
  }
#endif

  return ret;
}

std::string ecmdDataBuffer::genHexRightStr(uint32_t start, uint32_t bitLen) const {

  /* Do gen a hex right string, we just shift the data right to nibble align and then do a genHexLeft - tricky eh */
  int shiftAmt = bitLen % 4 ? 4 - (bitLen % 4) : 0;
  std::string ret;
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (start+bitLen > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::genHexRightStr: bit %d + len %d >= NumBits (%d)", start, bitLen, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return ret;
  }

  ecmdDataBuffer temp;
  temp.setBitLength(bitLen);
  extract(temp, start, bitLen);

  if (shiftAmt) {
    rc = temp.shiftRightAndResize(shiftAmt);
    if (rc) {
      return ret;
    }
  }
  ret = temp.genHexLeftStr();

#ifndef REMOVE_SIM
  /* If we are using this interface and find Xstate data we have a problem */
  if ((iv_DataStr != NULL) && hasXstate(start, bitLen)) {
    ETRAC0("**** WARNING : ecmdDataBuffer::genHexRightStr: Cannot extract when non-binary (X-State) character present");
    SET_ERROR(ECMD_DBUF_XSTATE_ERROR);
  }
#endif
    
  return ret;
}

std::string ecmdDataBuffer::genBinStr(uint32_t start, uint32_t bitLen) const {

  int tempNumWords = (bitLen + 31) / 32;
  std::string ret;
  char* data = new char[bitLen + 1];
  bool createdTemp = false;
  uint32_t* tempData = NULL;
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (start+bitLen > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::genBinStr: bit %d + len %d >= NumBits (%d)", start, bitLen, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    if (data) // make sure we have the buffer before running delete on it.
    {
        delete[] data;
    }
    return ret;
  }

  /* Let's see if we can get a perf bonus */
  if ((start == 0) && (bitLen == iv_NumBits))
    tempData = iv_Data;
  else {
    tempData = new uint32_t[tempNumWords];
    /* extract iv_Data */
    rc = this->extract(&tempData[0], start, bitLen);
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

  for (uint32_t w = 0; w < bitLen; w++) {
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
  data[bitLen] = '\0';

  if (createdTemp)
    delete[] tempData;

  ret = data;
  delete[] data;

#ifndef REMOVE_SIM
  /* If we are using this interface and find Xstate data we have a problem */
  if ((iv_DataStr != NULL) && hasXstate(start, bitLen)) {
    ETRAC0("**** WARNING : ecmdDataBuffer::genBinStr: Cannot extract when non-binary (X-State) character present");
    SET_ERROR(ECMD_DBUF_XSTATE_ERROR);
  }
#endif

  return ret;
}

std::string ecmdDataBuffer::genAsciiStr(uint32_t start, uint32_t bitLen) const {

  int numwords = (bitLen - 1)/32 + 1;
  int startWord = start/32;
  std::string ret;
  int i, j;
  uint32_t temp;
  char tempstr[4];

  if (start+bitLen > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::genAsciiStr: bit %d + len %d >= NumBits (%d)", start, bitLen, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return ret;
  }

  for (i = 0; i < numwords; i++) { /* word loop */
    for (j = 0; j < 4; j++) { /* byte loop */
      temp = (iv_Data[(startWord + i)] >> (24-8*j)) & 0x000000ff;  /* grab 8 bits           */  	  
      if (temp < 32 || temp > 126) {                /* decimal 32 == space, 127 == DEL */
        tempstr[0] = '.';                           /* non-printing: use a . */
        tempstr[1] = '\0';
      } else {
        sprintf(tempstr, "%c", temp);               /* convert to ascii      */
      }  
      ret.insert(ret.length(),tempstr);
    } 
  } 

#ifndef REMOVE_SIM
  /* If we are using this interface and find Xstate data we have a problem */
  if ((iv_DataStr != NULL) && hasXstate(start, bitLen)) {
    ETRAC0("**** WARNING : ecmdDataBuffer::genAsciiStr: Cannot extract when non-binary (X-State) character present");
    SET_ERROR(ECMD_DBUF_XSTATE_ERROR);
  }
#endif

  return ret;
}

std::string ecmdDataBuffer::genXstateStr(uint32_t start, uint32_t bitLen) const {
  std::string ret;
#ifndef REMOVE_SIM
  if (iv_DataStr == NULL) {
    ETRAC0("**** ERROR : ecmdDataBuffer::genXstateStr: Xstate operation called on buffer without xstate's enabled");
    SET_ERROR(ECMD_DBUF_XSTATE_NOT_ENABLED);
    return ret;
  }
    

  if (start+bitLen > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::genXstateStr: bit %d + len %d >= NumBits (%d)", start, bitLen, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    return ret;
  }

  char * copyStr = new char[bitLen + 4];
  strncpy(copyStr, &iv_DataStr[start], bitLen);
  copyStr[bitLen] = '\0';

  ret = copyStr;

  delete[] copyStr;
#else
  ETRAC0( "**** ERROR : ecmdDataBuffer: genXstateStr: Not defined in this configuration");
  SET_ERROR(ECMD_DBUF_XSTATE_ERROR);
#endif
  return ret;
}

std::string ecmdDataBuffer::genHexLeftStr() const { return this->genHexLeftStr(0, iv_NumBits); }
std::string ecmdDataBuffer::genHexRightStr() const { return this->genHexRightStr(0, iv_NumBits); }
std::string ecmdDataBuffer::genBinStr() const { return this->genBinStr(0, iv_NumBits); }
std::string ecmdDataBuffer::genAsciiStr() const { return this->genAsciiStr(0, iv_NumBits); }
std::string ecmdDataBuffer::genXstateStr() const { return this->genXstateStr(0, iv_NumBits); }

uint32_t ecmdDataBuffer::insertFromHexLeftAndResize (const char * i_hexChars, uint32_t start, uint32_t length) {
  if (length == 0) 
    length = strlen(i_hexChars) * 4;
  int rc = setBitLength(length);
  if (rc) return rc;
  return insertFromHexLeft(i_hexChars, start, length);
}

uint32_t ecmdDataBuffer::insertFromHexLeft (const char * i_hexChars, uint32_t start, uint32_t length) {
  int rc = ECMD_DBUF_SUCCESS;
  int i;

  int bitlength = length == 0 ? strlen(i_hexChars) * 4 : length;

  if (bitlength == 0) {
    /* They don't want anything inserted */
    return rc;
  }

  int wordLength = (bitlength % 32) ? bitlength / 32 + 1 : bitlength / 32;

  uint32_t * number_ptr = new uint32_t[wordLength];
  for (i = 0; i < wordLength; i++) {
    number_ptr[i] = 0x0;
  }

  uint32_t tmpb32 = 0x0;
  char nextOne[2];
  nextOne[1] = '\0';

  for (i = 0; i < (int) strlen(i_hexChars); i++) {
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


  this->insert(number_ptr, start, bitlength);

  delete[] number_ptr;

  return rc;
}

uint32_t ecmdDataBuffer::insertFromHexRightAndResize (const char * i_hexChars, uint32_t start, uint32_t length) {
  if (length == 0) 
    length = strlen(i_hexChars) * 4;
  int rc = setBitLength(length);
  if (rc) return rc;
  return insertFromHexRight(i_hexChars, start, length);
}

uint32_t ecmdDataBuffer::insertFromHexRight (const char * i_hexChars, uint32_t start, uint32_t expectedLength) {
  int rc = ECMD_DBUF_SUCCESS;
  ecmdDataBuffer insertBuffer;
  int bitlength = expectedLength == 0 ? strlen(i_hexChars) * 4 : expectedLength;

  if (bitlength == 0) {
    /* They don't want anything inserted */
    return rc;
  }

  /* Number of valid nibbles */
  uint32_t nibbles = bitlength % 4 ? bitlength / 4 + 1 : bitlength / 4;

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

  this->insert(insertBuffer, start, bitlength);

  return rc;
}

uint32_t ecmdDataBuffer::insertFromBinAndResize (const char * i_hexChars, uint32_t start) {
  int rc = setBitLength(strlen(i_hexChars));
  if (rc) return rc;
  return insertFromBin(i_hexChars, start);
}

uint32_t ecmdDataBuffer::insertFromBin (const char * i_binChars, uint32_t start) {
  int rc = ECMD_DBUF_SUCCESS;

  int strLen = strlen(i_binChars);

  for (int i = 0; i < strLen; i++) {
    if (i_binChars[i] == '0') {
      this->clearBit(start+i);
    }
    else if (i_binChars[i] == '1') {
      this->setBit(start+i);
    } else {
      RETURN_ERROR(ECMD_DBUF_INVALID_DATA_FORMAT);
    }
  }

  return rc;
}

uint32_t ecmdDataBuffer::copy(ecmdDataBuffer &newCopy) const {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  rc = newCopy.setBitLength(iv_NumBits);
  
  if (!rc && iv_NumBits != 0) {
    // iv_Data
    memcpy(newCopy.iv_Data, iv_Data, iv_NumWords * 4);
    // Error state
    newCopy.iv_RealData[2] = iv_RealData[2];
#ifndef REMOVE_SIM
    if (iv_DataStr != NULL) {
      // cje enable xstates in copied buffer 
      strncpy(newCopy.iv_DataStr, iv_DataStr, iv_NumBits);
    } else if (iv_DataStr == NULL) {
      /* cje disable xstates in copied buffer */
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
    iv_RealData[2] = i_master.iv_RealData[2];
#ifndef REMOVE_SIM
    if (iv_DataStr != NULL) {
      // cje enable xstates in copied buffer 
      strncpy(iv_DataStr, i_master.iv_DataStr, i_master.iv_NumBits);
    } else if (iv_DataStr == NULL) {
      /* cje disable xstates in copied buffer */
    }
#endif
  }
  return *this;
}


uint32_t  ecmdDataBuffer::memCopyIn(const uint32_t* buf, uint32_t bytes) { /* Does a memcpy from supplied buffer into ecmdDataBuffer */
  uint32_t rc = ECMD_DBUF_SUCCESS;

  uint32_t cbytes = bytes < getByteLength() ? bytes : getByteLength();
  if (cbytes == 0) {
    ETRAC0("**** ERROR : ecmdDataBuffer: memCopyIn: Copy performed on buffer with length of 0");
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    ecmdBigEndianMemCopy(iv_Data, buf, cbytes);
#ifndef REMOVE_SIM
    if (iv_DataStr != NULL) {
      uint32_t mask = 0x80000000;
      int curWord = 0;

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
  }
  return rc;
}
uint32_t  ecmdDataBuffer::memCopyOut(uint32_t* buf, uint32_t bytes) const { /* Does a memcpy from ecmdDataBuffer into supplied buffer */
  uint32_t rc = ECMD_DBUF_SUCCESS;
  int cbytes = bytes < getByteLength() ? bytes : getByteLength();
  if (cbytes == 0) {
    ETRAC0("**** ERROR : ecmdDataBuffer: memCopyOut: Copy performed on buffer with length of 0");
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    ecmdBigEndianMemCopy(buf, iv_Data, cbytes);
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

  newCapacity = (ntohl(i_ptr[0]) + 31) / 32;
  newBitLength = ntohl(i_ptr[1]);

  if ((i_len < 8) || (newCapacity > ((i_len - 8) * 8))) {
    ETRAC2("**** ERROR : ecmdDataBuffer::unflatten: i_len %d bytes is too small to unflatten a capacity of %d words ", i_len, newCapacity);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);

  } else if (newBitLength > newCapacity * 32) {
    ETRAC2("**** ERROR : ecmdDataBuffer::unflatten: iv_NumBits %d cannot be greater than iv_Capacity*32 %d", newBitLength, newCapacity*32);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    this->setCapacity(newCapacity);
    this->setBitLength(newBitLength);
    if (newCapacity > 0)
      for (uint32_t i = 0; i < newCapacity; i++)
        setWord(i, ntohl(i_ptr[i+2]));
  }
  return rc;
}

uint32_t ecmdDataBuffer::flattenSize() const {
  return (iv_Capacity + 2) * 4;
}

uint32_t  ecmdDataBuffer::flushToX(char i_value) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (iv_DataStr == NULL) {
    ETRAC0("**** ERROR : ecmdDataBuffer::flushToX: Xstate operation called on buffer without xstate's enabled");
    RETURN_ERROR(ECMD_DBUF_XSTATE_NOT_ENABLED);
  }

  if (iv_NumWords > 0) {
    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */
#ifndef REMOVE_SIM
    rc = this->fillDataStr(i_value);
#endif
  }
  return rc;
}

bool ecmdDataBuffer::hasXstate() const {
#ifdef REMOVE_SIM
  ETRAC0("**** ERROR : ecmdDataBuffer: hasXstate: Not defined in this configuration");
  SET_ERROR(ECMD_DBUF_XSTATE_ERROR);
  return false;
#else
  if (iv_DataStr == NULL) {
    ETRAC0("**** ERROR : ecmdDataBuffer::hasXstate: Xstate operation called on buffer without xstate's enabled");
    SET_ERROR(ECMD_DBUF_XSTATE_NOT_ENABLED);
    return false;
  }

  return (hasXstate(0,iv_NumBits));
#endif
}

bool   ecmdDataBuffer::hasXstate(uint32_t start, uint32_t length) const {
#ifdef REMOVE_SIM
  ETRAC0("**** ERROR : ecmdDataBuffer: hasXstate: Not defined in this configuration");
  SET_ERROR(ECMD_DBUF_XSTATE_ERROR);
  return false;
#else
  if (iv_DataStr == NULL) {
    ETRAC0("**** ERROR : ecmdDataBuffer::hasXstate: Xstate operation called on buffer without xstate's enabled");
    SET_ERROR(ECMD_DBUF_XSTATE_NOT_ENABLED);
    return false;
  }

  uint32_t stopBit = start + length;
  uint32_t minStop = iv_NumBits < stopBit ? iv_NumBits : stopBit; /* pick the smallest */

  for (uint32_t i = start; i < minStop; i++) {
    if (iv_DataStr[i] != '0' && iv_DataStr[i] != '1')
      return true;
  }
  return false;
#endif
}

/**
 * @brief Retrieve an Xstate value from the buffer
 * @param i_bit Bit to retrieve

 * NOTE - To retrieve multipe bits use genXstateStr
 */
char ecmdDataBuffer::getXstate(uint32_t i_bit) const {
#ifdef REMOVE_SIM
  ETRAC0("**** ERROR : ecmdDataBuffer: getXstate: Not defined in this configuration");
  SET_ERROR(ECMD_DBUF_XSTATE_ERROR);
  return '0';
#else
  if (iv_DataStr == NULL) {
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
#endif
}

/**
 * @brief Set an Xstate value in the buffer
 * @param i_bit Bit to set
 */
uint32_t ecmdDataBuffer::setXstate(uint32_t i_bit, char i_value) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

#ifdef REMOVE_SIM
  ETRAC0("**** ERROR : ecmdDataBuffer: setXstate: Not defined in this configuration");
  RETURN_ERROR(ECMD_DBUF_XSTATE_ERROR);

#else
  if (iv_DataStr == NULL) {
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
#endif
  return rc;
}

uint32_t ecmdDataBuffer::setXstate(uint32_t i_bit, char i_value, uint32_t i_length) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

#ifdef REMOVE_SIM
  ETRAC0("**** ERROR : ecmdDataBuffer: setXstate: Not defined in this configuration");
      RETURN_ERROR(ECMD_DBUF_XSTATE_ERROR);

#else
  if (iv_DataStr == NULL) {
    ETRAC0("**** ERROR : ecmdDataBuffer::setXstate: Xstate operation called on buffer without xstate's enabled");
    RETURN_ERROR(ECMD_DBUF_XSTATE_NOT_ENABLED);
  }

  if (i_bit + i_length > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::setXstate: bit %d + len %d > NumBits (%d)", i_bit, i_length, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    for (uint32_t idx = 0; idx < i_length; idx ++) rc |= this->setXstate(i_bit + idx, i_value);    
  }
#endif
  return rc;
}

/**
 * @brief Set a range of Xstate values in buffer
 * @param i_bitoffset bit in buffer to start inserting
 * @param i_datastr Character value to set bit - can be "0", "1", "X"
 */
uint32_t ecmdDataBuffer::setXstate(uint32_t bitOffset, const char* i_datastr) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

#ifdef REMOVE_SIM
  ETRAC0("**** ERROR : ecmdDataBuffer: setXstate: Not defined in this configuration");
      RETURN_ERROR(ECMD_DBUF_XSTATE_ERROR);

#else
  if (iv_DataStr == NULL) {
    ETRAC0("**** ERROR : ecmdDataBuffer::setXstate: Xstate operation called on buffer without xstate's enabled");
    RETURN_ERROR(ECMD_DBUF_XSTATE_NOT_ENABLED);
  }

  int len = strlen(i_datastr);
  int i;

  if (bitOffset+len > iv_NumBits) {
    ETRAC3("**** ERROR : ecmdDataBuffer::setXstate: bitOffset %d + len %d > NumBits (%d)", bitOffset, len, iv_NumBits);
    RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {

    for (i = 0; i < len; i++) {
      if (i_datastr[i] == '0') rc = clearBit(bitOffset+i);
      else if (i_datastr[i] == '1') rc = setBit(bitOffset+i);
      else if (!isxdigit(i_datastr[i])) {
        rc = clearBit(bitOffset+i);
        iv_DataStr[bitOffset+i] = i_datastr[i];
      } 
      else {
        ETRAC1("**** ERROR : ecmdDataBuffer::setXstate: unrecognized Xstate character: %c", i_datastr[i]);
      RETURN_ERROR(ECMD_DBUF_XSTATE_ERROR);
      }
    }

  }
#endif
  return rc;

}


/**
 * @brief Copy buffer into the Xstate data of this ecmdDataBuffer
 * @param i_buf Buffer to copy from
 * @param i_bytes Byte length to copy (char length)
 */
uint32_t  ecmdDataBuffer::memCopyInXstate(const char * i_buf, uint32_t i_bytes) { /* Does a memcpy from supplied buffer into ecmdDataBuffer */
  uint32_t rc = ECMD_DBUF_SUCCESS;

#ifndef REMOVE_SIM
  /* cbytes is equal to the bit length of data */
  int cbytes = i_bytes < getBitLength() ? i_bytes : getBitLength();
  int index;
#endif

#ifdef REMOVE_SIM
  ETRAC0("**** ERROR : ecmdDataBuffer: memCopyInXstate: Not defined in this configuration");
      RETURN_ERROR(ECMD_DBUF_XSTATE_ERROR);
#else
  if (iv_DataStr == NULL) {
    ETRAC0("**** ERROR : ecmdDataBuffer::memCopyInXstate: Xstate operation called on buffer without xstate's enabled");
    RETURN_ERROR(ECMD_DBUF_XSTATE_NOT_ENABLED);
  }

  /* Put the data into the Xstate array */
  strncpy(iv_DataStr, i_buf, cbytes);
  iv_DataStr[cbytes] = '\0';

  /* Now slide it over to the raw buffer */

  for (int bit = 0; bit < cbytes; bit++) {
    index = bit/32;
    if (i_buf[bit] == '1') {
      iv_Data[index] |= 0x00000001 << (31 - (bit-(index * 32)));
    } else {
      iv_Data[index] &= (~0x00000001 << (31 - (bit-(index * 32))));
    }
  }

#endif
  return rc;
}

/**
 * @brief Copy DataBuffer into supplied char buffer from Xstate data
 * @param o_buf Buffer to copy into - must be pre-allocated
 * @param i_bytes Byte length to copy
 */
uint32_t  ecmdDataBuffer::memCopyOutXstate(char * o_buf, uint32_t i_bytes) const { /* Does a memcpy from ecmdDataBuffer into supplied buffer */
  uint32_t rc = ECMD_DBUF_SUCCESS;
#ifndef REMOVE_SIM
  int cbytes = i_bytes < getByteLength() ? i_bytes : getByteLength();
#endif

#ifdef REMOVE_SIM
  ETRAC0("**** ERROR : ecmdDataBuffer: memCopyOutXstate: Not defined in this configuration");
      RETURN_ERROR(ECMD_DBUF_XSTATE_ERROR);
#else
  if (iv_DataStr == NULL) {
    ETRAC0("**** ERROR : ecmdDataBuffer::memCopyOutXstate: Xstate operation called on buffer without xstate's enabled");
    RETURN_ERROR(ECMD_DBUF_XSTATE_NOT_ENABLED);
  }

  strncpy(o_buf, iv_DataStr, cbytes);
  o_buf[cbytes] = '\0';

#endif
  return rc;
}



int ecmdDataBuffer::operator == (const ecmdDataBuffer& other) const {

  /* Check the length */
  uint32_t maxBits = 32;
  uint32_t numBits = getBitLength();
  uint32_t numToFetch = numBits < maxBits ? numBits : maxBits;
  uint32_t myData, otherData;
  int wordCounter = 0;

  if (getBitLength() != other.getBitLength()) {
    return 0;
  }

  if (getBitLength() == 0) /* two empty buffers are equal */
    return 1;

  /* Now run through the data */
  while (numToFetch > 0) {

    myData = iv_Data[wordCounter];
    otherData = other.iv_Data[wordCounter];

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
  if (iv_DataStr != NULL) {
    if (strncmp(iv_DataStr, other.iv_DataStr, iv_NumBits)) {
      return 0;
    }
  }
#endif

  /* Must have matched */
  return 1;
}

int ecmdDataBuffer::operator != (const ecmdDataBuffer& other) const {
  return !(*this == other);
}

ecmdDataBuffer ecmdDataBuffer::operator & (const ecmdDataBuffer& other) const {

  ecmdDataBuffer newItem = *this;

  if (iv_NumBits != other.iv_NumBits) {
    ETRAC2("**** ERROR : ecmdDataBuffer::operater &: NumBits in (%d) do not match NumBits (%d)", other.iv_NumBits, iv_NumBits);
    SET_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
  } else {
    newItem.setAnd(other.iv_Data, 0, iv_NumBits);
  }

  return newItem;
}

ecmdDataBuffer ecmdDataBuffer::operator | (const ecmdDataBuffer& other) const {

  ecmdDataBuffer newItem = *this;

  newItem.setOr(other.iv_Data, 0, iv_NumBits);

  return newItem;
}


//---------------------------------------------------------------------
//  Private Member Function Specifications
//---------------------------------------------------------------------
#ifndef REMOVE_SIM
uint32_t ecmdDataBuffer::fillDataStr(char fillChar) {
  if (iv_DataStr == NULL) {
    ETRAC0("**** ERROR : ecmdDataBuffer::fillDataStr: Xstate operation called on buffer without xstate's enabled");
    RETURN_ERROR(ECMD_DBUF_XSTATE_NOT_ENABLED);
  }
  if (iv_NumWords > 0) {
    memset(iv_DataStr, fillChar, iv_NumBits);
    iv_DataStr[iv_NumBits] = '\0';  
  }
  return ECMD_DBUF_SUCCESS;
}
#endif

uint32_t ecmdExtract(uint32_t *scr_ptr, uint32_t start_bit_num, uint32_t num_bits_to_extract, uint32_t *out_iv_Data_ptr)
{
  uint32_t i;   
  uint32_t temp;
  uint32_t len; 
  uint32_t mask1;
  uint32_t mask2;
  uint32_t offset;
  uint32_t index; 
  uint32_t count; 

  /*------------------------------------------------------------------*/
  /* calculate number of fws (32-bit pieces) of the destination buffer*/
  /* to be processed.                                                 */
  /*----------------------------line 98--------------------------------*/

  if (num_bits_to_extract == 0){
    ETRAC0("**** ERROR : extract: Number of bits to extract = 0");
    out_iv_Data_ptr = NULL;
    return ECMD_DBUF_INVALID_ARGS;
  }

  count = (num_bits_to_extract + 31) / 32;

  for /* all 32-bit (or < 32 bits) pieces of the destination buffer */
    (i = 0; i < count; i++)
  {
    len = num_bits_to_extract;

    if /* length of bits to process is > 32 */
      (len > 32)
    {
      len = 32;
    }

    /*******************************************************************/
    /* calculate index for accessing proper fw of the scan ring buffer */
    /*******************************************************************/
    index = start_bit_num/32;

    /*----------------------------------------------------------------*/
    /* generate the mask to zero out some extra extracted bits (if    */
    /* there are any) in the temporary buffer.                        */
    /*----------------------------------------------------------------*/
    if (len >= 32)
      mask1 = 0xFFFFFFFF;
    else
      mask1 = ~(0xffffffff << len);

    /*-------------line 121--------------------------------------------*/
    /* generate the mask to prevent zeroing of unused bits positions  */
    /* in the destination buffer.                                     */
    /*----------------------------------------------------------------*/
    if (32-len >= 32)
      mask2 = 0xffffffff;
    else 
      mask2 = ~(mask1 << (32-len));

    /******************************************************************/
    /* NOTE:- In this loop a max of 32 bits are extracted at a time.  */
    /* we may require to access either one or two fw's of scan ring   */
    /* buffer depending on the starting bit position & number of bits */
    /* to be extracted.                                               */
    /******************************************************************/
    if /* only one fw of scan ring buffer required to do extract */
      (index == ((start_bit_num+(len-1))/32))
    {
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
      temp = ((*(scr_ptr+index)) >>   /* step1 */
              (32-((start_bit_num+len)-(index*32))));
      if ((32-((start_bit_num+len)-(index*32))) >= 32)
        temp = 0x0;

      if ((32-len) >= 32)
        temp = 0x0;
      else
        temp = (temp & mask1) << (32-len); /* step2 */

      *(out_iv_Data_ptr+i) = (*(out_iv_Data_ptr+i) & mask2) | temp;
    }
    else /* two fws of scan ring buffer required to do extract */
    {
      /*-----------------line 158--------------------------------------*/
      /* calculate number of bits to process in the 1st fw of the     */
      /* scan ring buffer.(fw pointed by index)                       */
      /*--------------------------------------------------------------*/
      offset = (32 * (index+1)) - start_bit_num;
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
      if (len-offset < 32)
        val1 = ((*(scr_ptr+index)) << (len-offset)); /* 1st fw*/

      if ((32-(len-offset)) < 32)
        val2 = ((*(scr_ptr+index+1)) >> (32-(len-offset)));
      temp = (val1 | val2);/* 2nd fw */

      if (32-len >= 32)
        temp = 0x0;
      else
        temp = (temp & mask1) << (32-len); /* step2 */

      *(out_iv_Data_ptr+i) = (*(out_iv_Data_ptr+i) & mask2) | temp;

    }
    num_bits_to_extract -= 32; /* decrement length by a fw */
    start_bit_num += 32; /* increment start by a fw */
  }
  return ECMD_DBUF_SUCCESS;
}

uint32_t ecmdDataBuffer::writeFileMultiple(const char * i_filename, ecmdFormatType_t i_format, ecmdWriteMode_t i_mode, uint32_t & o_dataNumber) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  std::ofstream ops;
  std::ifstream ins;
  uint32_t totalFileSz, begOffset=0, tmphdrfield=0, szcount = 0, curDBOffset=0, tableSz=0;
  uint32_t numBytes = getByteLength();
  uint32_t numBits = getBitLength();
  bool firstDBWrite = false;
  char *offsetTableData=NULL;
  ecmdFormatType_t existingFmt;
  std::string asciidatastr,xstatestr;
  uint32_t *buffer;
  
 //Check if format asked for is the same as what was used b4
  #ifndef REMOVE_SIM
  if (iv_DataStr != NULL) {
    if((hasXstate()) && (i_format != ECMD_SAVE_FORMAT_XSTATE)) {
      ETRAC0( "**** ERROR : ecmdDataBuffer: writeFileMultiple: Buffer has Xstate data but non-xstate save mode requested");
      return(ECMD_DBUF_XSTATE_ERROR);
    }
  }
  #endif
  
  
 if ((i_format == ECMD_SAVE_FORMAT_BINARY_DATA) && (i_mode != ECMD_WRITE_MODE)) {
   ETRAC0("**** ERROR : File Format ECMD_SAVE_FORMAT_BINARY_DATA only supported in ECMD_WRITE_MODE");
   RETURN_ERROR(ECMD_DBUF_INVALID_ARGS);
 }

  //Open file for Read if it exists
  ins.open(i_filename);
    
  if ((!ins.fail()) && (i_mode != ECMD_WRITE_MODE) && (i_format != ECMD_SAVE_FORMAT_BINARY_DATA)) {
    ins.seekg(0, ios::end);
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
       RETURN_ERROR(ECMD_DBUF_FILE_OPERATION_FAIL); 
      }
      tableSz = totalFileSz-begOffset-12;
    }
    ins.close();
  } else { firstDBWrite = true;}
  
  //Open file for read/write
  if (i_mode == ECMD_APPEND_MODE) {
   if (!firstDBWrite) {
    ops.open(i_filename,  ios_base::in|ios_base::out );
   }
   else {
    ops.open(i_filename,  ios_base::out );
   }
  } else if(i_mode == ECMD_WRITE_MODE) {
    ops.open(i_filename, ofstream::out | ofstream::trunc);
  }
  if (ops.fail()) {
    ETRAC1("**** ERROR : Unable to open file : %s for write",i_filename);
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
    }
    else {
      //ASCII or XSTATE Hdr
      ops << "START";
      char tmpstr[9]; //@01c Bumped to 9 to account for NULL terminator
      sprintf(tmpstr,"%08X",numBits); ops.write(tmpstr,8);
      sprintf(tmpstr,"%08X",i_format); ops.write(tmpstr,8);
      sprintf(tmpstr,"%08X",tmphdrfield); ops.write(tmpstr,8);
      ops << "\n";
    }
    if (ops.fail()) {
     ETRAC1("**** ERROR : Write of the header failed on file : %s",i_filename);
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
    ops.write("END",3);//Key
    if (ops.fail()) {
     ETRAC1("**** ERROR : Write operation in format ECMD_SAVE_FORMAT_BINARY failed on file : %s",i_filename);
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
      if((szcount%256) == 0) {
  	ops << "\n"; 
      }
      else if((szcount%32) == 0) {
  	ops << " ";
      }
     }
     ops << asciidatastr.c_str();
     szcount+= 32;
   }
   ops << "\nEND\n";
   if (ops.fail()) {
     ETRAC1("**** ERROR : Write operation in format ECMD_SAVE_FORMAT_ASCII failed on file : %s",i_filename);
     RETURN_ERROR(ECMD_DBUF_FILE_OPERATION_FAIL); 
   }
  } 
  else if ( i_format == ECMD_SAVE_FORMAT_XSTATE) {
  if (iv_DataStr == NULL) {
    ETRAC0("**** ERROR : ecmdDataBuffer::getXstate: Xstate operation called on buffer without xstate's enabled");
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
     RETURN_ERROR(ECMD_DBUF_FILE_OPERATION_FAIL); 
   }
  }
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
    if (ops.fail()) {
     ETRAC1("**** ERROR : Write operation in format ECMD_SAVE_FORMAT_BINARY_DATA failed on file : %s",i_filename);
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
  
  return(rc);
}

uint32_t  ecmdDataBuffer::writeFile(const char * filename, ecmdFormatType_t format) {
  ecmdWriteMode_t mode = ECMD_WRITE_MODE;
  uint32_t dataNumber;
  return this->writeFileMultiple(filename, format, mode, dataNumber) ;
}

uint32_t  ecmdDataBuffer::writeFileStream(std::ostream & o_filestream) {
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
      RETURN_ERROR(ECMD_DBUF_FILE_OPERATION_FAIL); 
   }

  }
  return rc;
}

uint32_t  ecmdDataBuffer::readFileMultiple(const char * filename, ecmdFormatType_t format, uint32_t i_dataNumber) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  std::ifstream ins;
  uint32_t numBits = 0, numBytes = 0, NumDwords = 0, hexbitlen = 0, *buffer;
  uint32_t endOffset = 0;
  bool endFound = false;
  char key[6], hexstr[8], binstr[64], endKeyword[4];
  
  ins.open(filename);
    
  if (ins.fail()) {
    ETRAC1("**** ERROR : Unable to open file : %s for reading",filename);
    RETURN_ERROR(ECMD_DBUF_FOPEN_FAIL);  
  }
  //Read the DataBuffer offset table-Seek to the right DataBuffer Hdr
  if (i_dataNumber != 0) {
    ecmdFormatType_t existingFmt;
    uint32_t begOffset=0, totalFileSz=0, dataOffset;
    if (format == ECMD_SAVE_FORMAT_BINARY_DATA) {
      ETRAC0("**** ERROR : File Format ECMD_SAVE_FORMAT_BINARY_DATA not supported when file contains multiple DataBuffers.");
      RETURN_ERROR(ECMD_DBUF_INVALID_ARGS);
    }
    ins.seekg(0, ios::end);
    totalFileSz = ins.tellg();
    if (totalFileSz == 0) {
      ETRAC1("**** ERROR : File : %s is empty",filename);
      RETURN_ERROR(ECMD_DBUF_INVALID_ARGS);
    }
    else {
      ins.seekg(totalFileSz-8);//get the Begin offset of the offset table
      ins.read((char *)&begOffset,4); begOffset = htonl(begOffset);
      ins.read((char *)&existingFmt,4); existingFmt = (ecmdFormatType_t)htonl(existingFmt);
      if (existingFmt != format) {
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
        //ETRAC0("**** ERROR : END keyword not found. Invalid File Format.");
	RETURN_ERROR(ECMD_DBUF_FILE_FORMAT_MISMATCH);
      }
      if ((begOffset+8+(4*i_dataNumber)) >= endOffset) {
        //ETRAC0("**** ERROR : Data Number requested exceeds the maximum Data Number in the File.");
	RETURN_ERROR(ECMD_DBUF_DATANUMBER_NOT_FOUND);
      }
      ins.seekg(begOffset+8+(4*i_dataNumber));//seek to the right databuffer header
      ins.read((char *)&dataOffset,4);  dataOffset = htonl(dataOffset);
      if (ins.fail()) {
       ETRAC1("**** ERROR : Read of the dataOffset failed on file : %s",filename);
       RETURN_ERROR(ECMD_DBUF_FILE_OPERATION_FAIL); 
      }
      ins.seekg(dataOffset);
      if (ins.eof()) {
       ETRAC0("**** ERROR : Data Offset is greater than the file size.");
       RETURN_ERROR(ECMD_DBUF_FILE_OPERATION_FAIL); 
      }
    }
  }
 
  if ( format == ECMD_SAVE_FORMAT_BINARY) {
    // Read Hdr
    ins.read(key,5); key[5]='\0';
    if(strcmp(key,"START")!=0) {
      ETRAC0("**** ERROR : Keyword START not found.");
      RETURN_ERROR(ECMD_DBUF_FILE_FORMAT_MISMATCH); 
    }
    ins.seekg(3,ios::cur);
    ins.read((char *)&numBits,4);    numBits = htonl(numBits);
    ins.read((char *)&format,4);    format = (ecmdFormatType_t)htonl(format);
    if (format != ECMD_SAVE_FORMAT_BINARY ) {
      ETRAC0("**** ERROR : Format mismatch. Expected ECMD_SAVE_FORMAT_BINARY.");
      RETURN_ERROR(ECMD_DBUF_FILE_FORMAT_MISMATCH);  
    }
    ins.seekg(4,ios::cur);//1 empty words in hdr
    this->setBitLength(numBits);
    numBytes=getByteLength();
    //Read Data
    buffer = new uint32_t[getWordLength()];
    ins.read((char *)buffer,numBytes);
    if (ins.fail()) {
      ETRAC1("**** ERROR : Read operation in format ECMD_SAVE_FORMAT_BINARY failed on file : %s",filename);
      RETURN_ERROR(ECMD_DBUF_FILE_OPERATION_FAIL); 
    }
    for(uint32_t i=0; i< getWordLength(); i++) {
     buffer[i]=htonl(buffer[i]);
    }
    memCopyIn(buffer, numBytes);
  } else if ( format == ECMD_SAVE_FORMAT_BINARY_DATA) {
    ins.seekg(0, ios::end);
    numBytes = ins.tellg();
    numBits = numBytes * 8;
    this->setBitLength(numBits);
    ins.seekg(0, ios::beg);
    buffer = new uint32_t[getWordLength()];
    ins.read((char *)buffer,numBytes);
    if (ins.fail()) {
      ETRAC1("**** ERROR : Read operation in format ECMD_SAVE_FORMAT_BINARY_DATA failed on file : %s",filename);
      RETURN_ERROR(ECMD_DBUF_FILE_OPERATION_FAIL); 
    }
    for(uint32_t i=0; i< getWordLength(); i++) {
     buffer[i]=htonl(buffer[i]);
    }
    memCopyIn(buffer, numBytes);
  } else if(format ==  ECMD_SAVE_FORMAT_ASCII) {
    ins.width(6); ins >> key; 
    if(strcmp(key,"START")!=0) {
      ETRAC0("**** ERROR : Keyword START not found.");
      RETURN_ERROR(ECMD_DBUF_FILE_FORMAT_MISMATCH); 
    }
    ins.width(9);  ins >> hexstr; 
    numBits = strtoul(hexstr, NULL, 16); 
    ins.width(9); ins >> hexstr;  
    format = (ecmdFormatType_t) strtoul(hexstr, NULL, 16);
    if (format != ECMD_SAVE_FORMAT_ASCII ) {
      ETRAC0("**** ERROR : Format mismatch. Expected ECMD_SAVE_FORMAT_ASCII.");
      RETURN_ERROR(ECMD_DBUF_FILE_FORMAT_MISMATCH);  
    }
    ins.seekg(9,ios::cur);//1 empty words in hdr + new line
    this->setBitLength(numBits);
    for (uint32_t i = 0; i < iv_NumWords; i++) {
      ins.width(9);  ins >> hexstr;
      if (((i*32)+32) > numBits) {
        hexstr[strlen(hexstr)] = '\0'; //strip newline char
	hexbitlen = numBits - (i*32);
      } else {
        hexbitlen = 32;
      }
      insertFromHexLeft (hexstr, i*32, hexbitlen);
      ins.seekg(1,ios::cur);//Space or Newline char
    }
  } else if( format == ECMD_SAVE_FORMAT_XSTATE) {
    #ifdef REMOVE_SIM
      ETRAC0("**** ERROR : ecmdDataBuffer: XState: Not defined in this configuration");
      RETURN_ERROR(ECMD_DBUF_XSTATE_ERROR);
    #endif
      /// cje need to enable xstate
    ins.width(6); ins >> key; 
    if(strcmp(key,"START")!=0) {
      ETRAC0("**** ERROR : Keyword START not found.");
      RETURN_ERROR(ECMD_DBUF_FILE_FORMAT_MISMATCH);
    }
    ins.width(9);  ins >> hexstr;
    numBits = strtoul(hexstr, NULL, 16);
    ins.width(9); ins >> hexstr;
    format = (ecmdFormatType_t) strtoul(hexstr, NULL, 16);
    if (format != ECMD_SAVE_FORMAT_XSTATE ) {
      ETRAC0("**** ERROR : Format mismatch. Expected ECMD_SAVE_FORMAT_XSTATE.");
      RETURN_ERROR(ECMD_DBUF_FILE_FORMAT_MISMATCH);
    }
    
    ins.seekg(9,ios::cur);//1 empty words in hdr + new line
    this->setBitLength(numBits);
    NumDwords = iv_NumBits % 64 ? (iv_NumBits / 64) + 1 : iv_NumBits / 64;
    for (uint32_t i = 0; i < NumDwords; i++) {
      ins.width(65);  ins >> binstr;
      if ((i*64)+64 > numBits) 
        binstr[strlen(binstr)] = '\0'; //strip newline char
      setXstate(i*64, binstr);
      ins.seekg(1,ios::cur);// New line char
    }
  }
  ins.close();
  return(rc);
}


uint32_t  ecmdDataBuffer::readFile(const char * i_filename, ecmdFormatType_t i_format) {
  uint32_t dataNumber=0;
  return this->readFileMultiple(i_filename, i_format, dataNumber);
}

uint32_t  ecmdDataBuffer::readFileStream(std::istream & i_filestream, uint32_t i_bitlength) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  uint32_t numBytes = i_bitlength % 8 ? (i_bitlength / 8) + 1 : i_bitlength / 8;
  uint32_t *buffer;
  
  this->setBitLength(i_bitlength);
  
  buffer = new uint32_t[getWordLength()];
  i_filestream.read((char *)buffer, numBytes);
  if (i_filestream.fail()) {
      ETRAC0("**** ERROR : Read operation failed.");
      RETURN_ERROR(ECMD_DBUF_FILE_OPERATION_FAIL); 
  }

  
  for (uint32_t i=0; i< getWordLength(); i++) {
   buffer[i] = htonl(buffer[i]);
  }
  rc = memCopyIn(buffer, numBytes);
  
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
#endif
    return(rc);
}

void ecmdDataBuffer::queryErrorState( uint32_t & o_errorState) {
  if (iv_RealData != NULL) {
    o_errorState = iv_RealData[2];
  } else {
    o_errorState = 0;
  }
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
  }
  else if (remainder == 1) {
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
  if (buff->iv_DataStr == NULL) {
    return;
  }
  strcpy(buff->iv_DataStr,buff->genBinStr().c_str());
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
ecmdOptimizableDataBuffer::ecmdOptimizableDataBuffer(uint32_t numBits)
 : ecmdDataBuffer(numBits) {
       iv_BufferOptimizable = true;     
}
