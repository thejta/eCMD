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

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>
#include <netinet/in.h> /* for htonl */

#include <ecmdDataBuffer.H>

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

//----------------------------------------------------------------------
//  Forward declarations
//----------------------------------------------------------------------
uint32_t ecmdExtract(uint32_t *scr_ptr, uint32_t start_bit_num, uint32_t num_bits_to_extract, uint32_t *out_data_ptr);
void * ecmdBigEndianMemCopy(void * dest, const void *src, size_t count);

//----------------------------------------------------------------------
//  Data Storage Header Format - Example has 4 words
//   Pointer       offset    data
//  real_data  ->  000000   BEEFBEEF <numwords> 00000000 12345678
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
}

ecmdDataBuffer::ecmdDataBuffer(uint32_t numBits)
: iv_Capacity(0), iv_NumWords(0), iv_NumBits(0), iv_Data(NULL), iv_RealData(NULL)
{
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

  if (other.iv_NumBits != 0) {

    this->setBitLength(other.iv_NumBits);
    for (uint32_t i = 0; i < iv_NumWords; i++) 
      iv_Data[i] = other.iv_Data[i];


#ifndef REMOVE_SIM
    strncpy(iv_DataStr, other.iv_DataStr, iv_NumBits);
#endif
  }
  /* else do nothing.  already have an empty buffer */

}

//---------------------------------------------------------------------
//  Destructor
//---------------------------------------------------------------------
ecmdDataBuffer::~ecmdDataBuffer()
{
  clear();
}

//---------------------------------------------------------------------
//  Public Member Function Specifications
//---------------------------------------------------------------------
uint32_t ecmdDataBuffer::clear() {

  uint32_t rc = ECMD_DBUF_SUCCESS;

  if ((iv_RealData != NULL)) {
    /* Let's check our header,tail info */
    if ((iv_RealData[0] != DATABUFFER_HEADER) || (iv_RealData[1] != iv_NumWords) || (iv_RealData[3] != iv_RealData[iv_NumWords + 4])) {
      /* Ok, something is wrong here */
      printf("**** SEVERE ERROR (ecmdDataBuffer) : iv_RealData[0]: %X, iv_RealData[1]: %X, iv_NumWords: %X\n",iv_RealData[0],iv_RealData[1],iv_NumWords);
      printf("**** SEVERE ERROR (ecmdDataBuffer) : iv_RealData[3]: %X, iv_RealData[iv_NumWords + 4]: %X\n",iv_RealData[3],iv_RealData[iv_NumWords + 4]);
      printf("**** SEVERE ERROR (ecmdDataBuffer) : PROBLEM WITH DATABUFFER - INVALID HEADER/TAIL\n");
      rc = ECMD_DBUF_BUFFER_OVERFLOW;
      exit(rc);
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

  uint32_t rc = ECMD_DBUF_SUCCESS;

  rc = setBitLength(newNumWords * 32);

  return rc;

}  

uint32_t  ecmdDataBuffer::setBitLength(uint32_t newNumBits) {

  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (newNumBits == iv_NumBits)
    return rc;  /* nothing to do */

  uint32_t newNumWords = newNumBits % 32 ? newNumBits / 32 + 1 : newNumBits / 32;
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
      printf("**** ERROR : ecmdDataBuffer::setBitLength : Unable to allocate memory for new databuffer\n");
      return ECMD_DBUF_INIT_FAIL;
    }

    iv_Data = iv_RealData + 4;
    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */

#ifndef REMOVE_SIM
    if (iv_DataStr != NULL)
      delete[] iv_DataStr;

    iv_DataStr = NULL;

    iv_DataStr = new char[iv_NumBits + 42];

    if (iv_DataStr == NULL) {
      printf("**** ERROR : ecmdDataBuffer::setBitLength : Unable to allocate Xstate memory for new databuffer\n");
      return ECMD_DBUF_INIT_FAIL;
    }

    this->fillDataStr('0'); /* init to 0 */
#endif


  } else if (iv_NumBits != 0) { /* no need to resize */

    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */

#ifndef REMOVE_SIM
    this->fillDataStr('0'); /* init to 0 */
#endif

  }
#ifndef REMOVE_SIM
  else /* decreasing bit length to zero */
    iv_DataStr[0] = '\0'; 
#endif

  /* Ok, now setup the header, and tail */
  iv_RealData[0] = DATABUFFER_HEADER;
  iv_RealData[1] = iv_NumWords;
  iv_RealData[3] = randNum;
  iv_RealData[iv_NumWords + 4] = randNum;

  return rc;
}  

uint32_t ecmdDataBuffer::setCapacity (uint32_t newCapacity) {

  uint32_t rc = ECMD_DBUF_SUCCESS;

  /* only resize to make the capacity bigger */
  if (iv_Capacity < newCapacity) {
    uint32_t randNum = 0x12345678;

    iv_Capacity = newCapacity;
    if (iv_RealData != NULL)
      delete[] iv_RealData;

    iv_RealData = NULL;

    iv_RealData = new uint32_t[iv_Capacity + 10]; 

    if (iv_RealData == NULL) {
      printf("**** ERROR : ecmdDataBuffer::setCapacity : Unable to allocate memory for new databuffer\n");
      return ECMD_DBUF_INIT_FAIL;
    }

    iv_Data = iv_RealData + 4;
    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */

    /* Ok, now setup the header, and tail */
    iv_RealData[0] = DATABUFFER_HEADER;
    iv_RealData[1] = iv_NumWords;
    iv_RealData[3] = randNum;
    iv_RealData[iv_NumWords + 4] = randNum;

#ifndef REMOVE_SIM
    if (iv_DataStr != NULL)
      delete[] iv_DataStr;

    iv_DataStr = NULL;

    iv_DataStr = new char[(iv_Capacity*32)+42];

    if (iv_DataStr == NULL) {
      printf("**** ERROR : ecmdDataBuffer::setCapacity : Unable to allocate Xstate memory for new databuffer\n");
      return ECMD_DBUF_INIT_FAIL;
    }

    this->fillDataStr('0'); /* init to 0 */
#endif

  }
  return rc;

}

uint32_t ecmdDataBuffer::shrinkBitLength(uint32_t i_newNumBits) {

  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (i_newNumBits > iv_NumBits) {
    printf("**** ERROR : ecmdDataBuffer::shrinkBitLength: New Bit Length (%d) > current NumBits (%d)\n", i_newNumBits, iv_NumBits);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
    return rc;
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

uint32_t  ecmdDataBuffer::setBit(uint32_t bit) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (bit >= iv_NumBits) {
    printf("**** ERROR : ecmdDataBuffer::setBit: bit %d >= NumBits (%d)\n", bit, iv_NumBits);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
  } else {
    int index = bit/32;
    iv_Data[index] |= 0x00000001 << (31 - (bit-(index * 32)));
#ifndef REMOVE_SIM
    iv_DataStr[bit] = '1';
#endif
  }
  return rc;
}

uint32_t  ecmdDataBuffer::setBit(uint32_t bit, uint32_t len) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (bit+len > iv_NumBits) {
    printf("**** ERROR : ecmdDataBuffer::setBit: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
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
    printf("**** ERROR : ecmdDataBuffer::setWord: wordoffset %d >= NumWords (%d)\n", wordOffset, iv_NumWords);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
  } else {
    iv_Data[wordOffset] = value;
    
#ifndef REMOVE_SIM
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
#endif

  }
  return rc;
}

uint32_t  ecmdDataBuffer::setByte(uint32_t byteOffset, uint8_t value) {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (byteOffset >= getByteLength()) {
    printf("**** ERROR : ecmdDataBuffer::setByte: byteOffset %d >= NumBytes (%d)\n", byteOffset, getByteLength());
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
  } else {
#if defined (i386)
    ((uint8_t*)(this->iv_Data))[byteOffset^3] = value;
#else
    ((uint8_t*)(this->iv_Data))[byteOffset] = value;
#endif
    
#ifndef REMOVE_SIM
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
#endif

  }
  return rc;
}

uint8_t ecmdDataBuffer::getByte(uint32_t byteOffset) const {
  if (byteOffset >= getByteLength()) {
    printf("**** ERROR : ecmdDataBuffer::getByte: byteOffset %d >= NumBytes (%d)\n", byteOffset, getByteLength());
    return 0;
  }
#if defined (i386)
  return ((uint8_t*)(this->iv_Data))[byteOffset^3];
#else
  return ((uint8_t*)(this->iv_Data))[byteOffset];
#endif
}


uint32_t  ecmdDataBuffer::clearBit(uint32_t bit) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (bit >= iv_NumBits) {
    printf("**** ERROR : ecmdDataBuffer::clearBit: bit %d >= NumBits (%d)\n", bit, iv_NumBits);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
  } else {  
    int index = bit/32;
    iv_Data[index] &= ~(0x00000001 << (31 - (bit-(index * 32))));
#ifndef REMOVE_SIM
    iv_DataStr[bit] = '0';
#endif
  }
  return rc;
}

uint32_t  ecmdDataBuffer::clearBit(uint32_t bit, uint32_t len) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (bit+len > iv_NumBits) {
    printf("**** ERROR : ecmdDataBuffer::clearBit: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
  } else {
    for (uint32_t idx = 0; idx < len; idx ++) rc |= this->clearBit(bit + idx);    
  }
  return rc;
}

uint32_t  ecmdDataBuffer::flipBit(uint32_t bit) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  if (bit >= iv_NumBits) {
    printf("**** ERROR : ecmdDataBuffer::flipBit: bit %d >= NumBits (%d)\n", bit, iv_NumBits);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
  } else {
#ifndef REMOVE_SIM
    if (this->hasXstate(bit, 1)) {
      printf("**** ERROR : ecmdDataBuffer::flipBit: cannot flip non-binary data at bit %d\n", bit);
      rc = ECMD_DBUF_XSTATE_ERROR;
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
    printf("**** ERROR : ecmdDataBuffer::flipBit: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
  } else {
    for (uint32_t i = 0; i < len; i++) {
      this->flipBit(bit+i);
    }
  }
  return rc;
}

bool   ecmdDataBuffer::isBitSet(uint32_t bit) const {
  if (bit >= iv_NumBits) {
    printf("**** ERROR : ecmdDataBuffer::isBitSet: bit %d >= NumBits (%d)\n", bit, iv_NumBits);
    return false;
  } else {
#ifndef REMOVE_SIM
    if (iv_DataStr[bit] != '1' && iv_DataStr[bit] != '0') {
      printf("**** ERROR : ecmdDataBuffer::isBitSet: non-binary character detected in data at bit %d\n", bit);
      return false;
    }
#endif
    int index = bit/32;
    return (iv_Data[index] & 0x00000001 << (31 - (bit-(index * 32)))); 
  }
  return false;
}

bool   ecmdDataBuffer::isBitSet(uint32_t bit, uint32_t len) const {
  if (bit+len > iv_NumBits) {
    printf("**** ERROR : ecmdDataBuffer::isBitSet: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
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
    printf("**** ERROR : ecmdDataBuffer::isBitClear: bit %d >= NumBits (%d)\n", bit, iv_NumBits);
    return false;
  } else {
#ifndef REMOVE_SIM
    if (iv_DataStr[bit] != '1' && iv_DataStr[bit] != '0') {
      printf( "**** ERROR : ecmdDataBuffer::isBitClear: non-binary character detected in data string\n");
      return false;
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
    printf("**** ERROR : ecmdDataBuffer::isBitClear: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
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
    printf("**** ERROR : ecmdDataBuffer::getNumBitsSet: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
    return 0;
  } else {
    int count = 0;
    for (uint32_t i = bit; i < len; i++) {
      if (this->isBitSet(i)) count++;
    }
    return count;
  }
}

uint32_t   ecmdDataBuffer::shiftRight(uint32_t shiftNum) {

  uint32_t rc = ECMD_DBUF_SUCCESS;

  uint32_t thisCarry;
  uint32_t prevCarry = 0x00000000;

  uint32_t i;

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
  // shift char
  char* temp = new char[iv_NumBits+42];
  for (i = 0; i < shiftNum+1; i++) temp[i] = '0'; // backfill with zeros
  temp[iv_NumBits] = '\0';
  strncpy(&temp[shiftNum], iv_DataStr, iv_NumBits-shiftNum);  
  strcpy(iv_DataStr, temp); // copy back into iv_DataStr
  delete[] temp;
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
  // shift char
  char* temp = new char[iv_NumBits+42];
  for (uint32_t j = iv_NumBits - shiftNum - 1; j < iv_NumBits; j++) temp[j] = '0'; // backfill with zeros
  temp[iv_NumBits] = '\0';
  strncpy(temp, &iv_DataStr[shiftNum], iv_NumBits - shiftNum);  
  strcpy(iv_DataStr, temp); // copy back into iv_DataStr
  delete[] temp;

#endif
  return rc;

}


uint32_t   ecmdDataBuffer::shiftRightAndResize(uint32_t shiftNum) {

  uint32_t rc = ECMD_DBUF_SUCCESS;

  uint32_t thisCarry;
  uint32_t prevCarry = 0x00000000;

  uint32_t i, prevlen;

  /* We need to verify we have room to do this shifting */
  /* Set our new length */
  iv_NumWords = (iv_NumBits + shiftNum + 31) / 32;
  if (iv_NumWords > iv_Capacity) {
    /* UhOh we are out of room, have to resize */
    prevlen = iv_Capacity;
    uint32_t * tempBuf = new uint32_t[prevlen];
    if (tempBuf == NULL) {
      printf("**** ERROR : ecmdDataBuffer::shiftRightAndResize : Unable to allocate temp buffer\n");
      return ECMD_DBUF_INIT_FAIL;
    }
    memcpy(tempBuf, iv_Data, prevlen * 4);

#ifndef REMOVE_SIM
    char* temp = new char[iv_NumBits+42];
    if (temp == NULL) {
      printf("**** ERROR : ecmdDataBuffer::shiftRightAndResize : Unable to allocate temp X-State buffer\n");
      return ECMD_DBUF_INIT_FAIL;
    }
    strncpy(temp, iv_DataStr, iv_NumBits);
#endif
    /* Now resize with the new capacity */
    rc = setCapacity(iv_NumWords);
    if (rc) return rc;

    /* Restore the data */
    memcpy(iv_Data, tempBuf, prevlen * 4);
    delete[] tempBuf;

#ifndef REMOVE_SIM
    strncpy(iv_DataStr, temp, iv_NumBits); // copy back into iv_DataStr
    delete[] temp;
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
  // shift char
  char* temp = new char[iv_NumBits+42];
  if (temp == NULL) {
    printf("**** ERROR : ecmdDataBuffer::shiftRightAndResize : Unable to allocate temp X-State buffer\n");
    return ECMD_DBUF_INIT_FAIL;
  }
  for (i = 0; i < shiftNum; i++) temp[i] = '0'; // backfill with zeros
  temp[iv_NumBits] = '\0';
  strncpy(&temp[shiftNum], iv_DataStr, iv_NumBits-shiftNum);  
  strcpy(iv_DataStr, temp); // copy back into iv_DataStr
  delete[] temp;
#endif
  return rc;
}

uint32_t   ecmdDataBuffer::shiftLeftAndResize(uint32_t shiftNum) {

  uint32_t rc = ECMD_DBUF_SUCCESS;

  uint32_t thisCarry;
  uint32_t prevCarry = 0x00000000;
  int i;

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
  // shift char
  char* temp = new char[iv_NumBits+42];
  if (temp == NULL) {
    printf("**** ERROR : ecmdDataBuffer::shiftLeftAndResize : Unable to allocate temp X-State buffer\n");
    return ECMD_DBUF_INIT_FAIL;
  }
  temp[iv_NumBits] = '\0';
  strncpy(temp, &iv_DataStr[shiftNum], iv_NumBits);  
  strcpy(iv_DataStr, temp); // copy back into iv_DataStr
  delete[] temp;
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
#endif
  return rc;

}


uint32_t  ecmdDataBuffer::insert(const ecmdDataBuffer &i_bufferIn, uint32_t i_targetStart, uint32_t i_len, uint32_t i_sourceStart) {
  uint32_t rc = ECMD_DBUF_SUCCESS;
  rc = this->insert(i_bufferIn.iv_Data, i_targetStart, i_len, i_sourceStart);
    /* Now apply the Xstate stuff */
#ifndef REMOVE_SIM   
    if (i_targetStart+i_len <= iv_NumBits) {
      strncpy(&(iv_DataStr[i_targetStart]), (i_bufferIn.genXstateStr(0, i_len)).c_str(), i_len);
    }
#endif
  return rc;
}

uint32_t  ecmdDataBuffer::insert(const uint32_t *i_dataIn, uint32_t i_targetStart, uint32_t i_len, uint32_t i_sourceStart) {
  uint32_t rc = ECMD_DBUF_SUCCESS;


  if (i_targetStart+i_len > iv_NumBits) {
    printf("**** ERROR : ecmdDataBuffer::insert: i_targetStart %d + i_len %d > iv_NumBits (%d)\n", i_targetStart, i_len, iv_NumBits);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
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
  return this->insert(&i_dataIn, i_targetStart, i_len, i_sourceStart);
}

uint32_t  ecmdDataBuffer::insertFromRight(const uint32_t * i_datain, uint32_t i_start, uint32_t i_len) {

  uint32_t rc = ECMD_DBUF_SUCCESS;

  int offset = 32 - (i_len % 32);

  if (i_start+i_len > iv_NumBits) {
    printf("**** ERROR : ecmdDataBuffer::insertFromRight: start %d + len %d > iv_NumBits (%d)\n", i_start, i_len, iv_NumBits);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
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

  if (start + len > iv_NumBits) {
    printf( "**** ERROR : ecmdDataBuffer::extract: start + len %d > NumBits (%d)\n", start + len, iv_NumBits);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
  } else {
    rc = bufferOut.setBitLength(len);
    if (rc) return rc;

    rc = ecmdExtract(this->iv_Data, start, len, bufferOut.iv_Data);
    if (rc) return rc;


#ifndef REMOVE_SIM   
    if (start+len <= iv_NumBits) {
      strncpy(bufferOut.iv_DataStr, (genXstateStr(start, len)).c_str(), len);
      bufferOut.iv_DataStr[len] = '\0';
    }
#endif
  }
  return rc;
}

uint32_t ecmdDataBuffer::extract(uint32_t *dataOut, uint32_t start, uint32_t len) const {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  if (start + len > iv_NumBits) {
    printf( "**** ERROR : ecmdDataBuffer::extract: start + len %d > NumBits (%d)\n", start + len, iv_NumBits);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
  } else {

    rc = ecmdExtract(this->iv_Data, start, len, dataOut);

#ifndef REMOVE_SIM
    /* If we are using this interface and find Xstate data we have a problem */
    if (hasXstate(start, len)) {
      printf("**** WARNING : ecmdDataBuffer::extract: Cannot extract when non-binary (X-State) character present\n");
      rc = ECMD_DBUF_XSTATE_ERROR;
    }
#endif
  }
  return rc;
}

uint32_t ecmdDataBuffer::extractToRight(ecmdDataBuffer & o_bufferOut, uint32_t i_start, uint32_t i_len) const {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  rc = this->extract(o_bufferOut, i_start, i_len);

  if (i_len < 32)
    o_bufferOut.shiftRightAndResize(32 - i_len);
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
    printf("**** ERROR : ecmdDataBuffer::setOr: bit %d + len %d > NumBits (%d)\n", startBit, len, iv_NumBits);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
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
    printf("**** ERROR : ecmdDataBuffer::setOr: bit %d + len %d > NumBits (%d)\n", startBit, len, iv_NumBits);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
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
    printf("**** ERROR : ecmdDataBuffer::merge: NumBits in (%d) do not match NumBits (%d)\n", bufferIn.iv_NumBits, iv_NumBits);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
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
    printf("**** ERROR : ecmdDataBuffer::setAnd: bit %d + len %d > iv_NumBits (%d)\n", startBit, len, iv_NumBits);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
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
    printf("**** ERROR : ecmdDataBuffer::getWord: wordOffset %d >= NumWords (%d)\n", wordOffset, iv_NumWords);
    return 0;
  }
  return this->iv_Data[wordOffset];
}

std::string ecmdDataBuffer::genHexLeftStr(uint32_t start, uint32_t bitLen) const {

  int tempNumWords = (bitLen + 31) / 32;
  std::string ret;

  char cPtr[10];
  /* extract iv_Data */
  ecmdDataBuffer tmpBuffer(tempNumWords*32);
  tmpBuffer.flushTo0();

  this->extract(tmpBuffer, start, bitLen);

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
    if (hasXstate(start, bitLen)) {
      printf("**** WARNING : ecmdDataBuffer::genHexLeftStr: Cannot extract when non-binary (X-State) character present\n");
    }
#endif

  return ret;
}

std::string ecmdDataBuffer::genHexRightStr(uint32_t start, uint32_t bitLen) const {

  /* Do gen a hex right string, we just shift the data right to nibble align and then do a genHexLeft - tricky eh */
  int shiftAmt = bitLen % 4 ? 4 - (bitLen % 4) : 0;
  std::string ret;

  ecmdDataBuffer temp;
  temp.setBitLength(bitLen);
  extract(temp, start, bitLen);

  if (shiftAmt) {
    temp.shiftRightAndResize(shiftAmt);
  }
  ret = temp.genHexLeftStr();

#ifndef REMOVE_SIM
    /* If we are using this interface and find Xstate data we have a problem */
    if (hasXstate(start, bitLen)) {
      printf("**** WARNING : ecmdDataBuffer::genHexRightStr: Cannot extract when non-binary (X-State) character present\n");
    }
#endif
    
  return ret;
}

std::string ecmdDataBuffer::genBinStr(uint32_t start, uint32_t bitLen) const {

  int tempNumWords = (bitLen + 31) / 32;
  std::string ret;
  /* extract iv_Data */
  uint32_t* tempData = new uint32_t[tempNumWords];
  this->extract(&tempData[0], start, bitLen);
  uint32_t mask = 0x80000000;
  int curWord = 0;

  for (uint32_t w = 0; w < bitLen; w++) {
    if (tempData[curWord] & mask) {
      ret.append("1");
    }
    else {
      ret.append("0");
    }

    mask >>= 1;

    if (!mask) {
      curWord++;
      mask = 0x80000000;
    }

  }

  delete[] tempData;

#ifndef REMOVE_SIM
    /* If we are using this interface and find Xstate data we have a problem */
    if (hasXstate(start, bitLen)) {
      printf("**** WARNING : ecmdDataBuffer::genBinStr: Cannot extract when non-binary (X-State) character present\n");
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

  //if (!str || numbytes < 1) return 1;

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
    if (hasXstate(start, bitLen)) {
      printf("**** WARNING : ecmdDataBuffer::genAsciiStr: Cannot extract when non-binary (X-State) character present\n");
    }
#endif

  return ret;
}

std::string ecmdDataBuffer::genXstateStr(uint32_t start, uint32_t bitLen) const {
  std::string ret;
#ifndef REMOVE_SIM
  char * copyStr = new char[bitLen + 4];
  strncpy(copyStr, &iv_DataStr[start], bitLen);
  copyStr[bitLen] = '\0';

  ret = copyStr;

  delete[] copyStr;
#else
  printf( "**** ERROR : ecmdDataBuffer: genXstateStr: Not defined in this configuration");
#endif
  return ret;
}

std::string ecmdDataBuffer::genHexLeftStr() const { return this->genHexLeftStr(0, iv_NumBits); }
std::string ecmdDataBuffer::genHexRightStr() const { return this->genHexRightStr(0, iv_NumBits); }
std::string ecmdDataBuffer::genBinStr() const { return this->genBinStr(0, iv_NumBits); }
std::string ecmdDataBuffer::genAsciiStr() const { return this->genAsciiStr(0, iv_NumBits); }
std::string ecmdDataBuffer::genXstateStr() const { return this->genXstateStr(0, iv_NumBits); }

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
      return ECMD_DBUF_INVALID_DATA_FORMAT;
    }
    nextOne[0] = i_hexChars[i];
    tmpb32 = strtoul(nextOne, NULL, 16);
    number_ptr[i>>3] |= (tmpb32 << (28 - (4 * (i & 0x07))));
  }


  this->insert(number_ptr, start, bitlength);

  delete[] number_ptr;

  return rc;
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
    insertBuffer.shiftRightAndResize((nibbles - strlen(i_hexChars)) * 4);
  }

  /* Now we have left aligned data, we just shift to right the odd bits of the nibble to align to the right */
  if (bitlength % 4)
    insertBuffer.shiftLeftAndResize(4 - (bitlength % 4));
  
  /* Now we have our data insert into ourselves */

  this->insert(insertBuffer, start, bitlength);

  return rc;
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
      return ECMD_DBUF_INVALID_DATA_FORMAT;
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

#ifndef REMOVE_SIM
    // char
    strncpy(newCopy.iv_DataStr, iv_DataStr, iv_NumBits);
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
#ifndef REMOVE_SIM
    // char
    strncpy(iv_DataStr, i_master.iv_DataStr, i_master.iv_NumBits);
#endif
  }
  return *this;
}


uint32_t  ecmdDataBuffer::memCopyIn(const uint32_t* buf, uint32_t bytes) { /* Does a memcpy from supplied buffer into ecmdDataBuffer */
  uint32_t rc = ECMD_DBUF_SUCCESS;

  int cbytes = bytes < getByteLength() ? bytes : getByteLength();
  if (cbytes == 0) {
    printf("**** ERROR : ecmdDataBuffer: memCopyIn: Copy performed on buffer with length of 0\n");
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
  } else {
    ecmdBigEndianMemCopy(iv_Data, buf, cbytes);
#ifndef REMOVE_SIM
    strcpy(iv_DataStr, genBinStr().c_str());
#endif
  }
  return rc;
}
uint32_t  ecmdDataBuffer::memCopyOut(uint32_t* buf, uint32_t bytes) const { /* Does a memcpy from ecmdDataBuffer into supplied buffer */
  uint32_t rc = ECMD_DBUF_SUCCESS;
  int cbytes = bytes < getByteLength() ? bytes : getByteLength();
  if (cbytes == 0) {
    printf("**** ERROR : ecmdDataBuffer: memCopyOut: Copy performed on buffer with length of 0\n");
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
  } else {
    ecmdBigEndianMemCopy(buf, iv_Data, cbytes);
  }
  return rc;
}

uint32_t ecmdDataBuffer::flatten(uint8_t * o_data, uint32_t i_len) const {
  uint32_t rc = ECMD_DBUF_SUCCESS;

  uint32_t * o_ptr = (uint32_t *) o_data;

  if ((i_len < 8) || (iv_Capacity*32 > ((i_len - 8) * 8))) {
    printf("**** ERROR : ecmdDataBuffer::flatten: i_len %d bytes is too small to flatten a capacity of %d words \n", i_len, iv_Capacity);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;

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
    printf("**** ERROR : ecmdDataBuffer::unflatten: i_len %d bytes is too small to unflatten a capacity of %d words \n", i_len, newCapacity);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;

  } else if (newBitLength > newCapacity * 32) {
    printf("**** ERROR : ecmdDataBuffer::unflatten: iv_NumBits %d cannot be greater than iv_Capacity*32 %d\n", newBitLength, newCapacity*32);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
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

bool ecmdDataBuffer::hasXstate() const {
#ifdef REMOVE_SIM
  printf( "**** ERROR : ecmdDataBuffer: hasXstate: Not defined in this configuration\n");
  return false;
#else
  return (hasXstate(0,iv_NumBits));
#endif
}

bool   ecmdDataBuffer::hasXstate(uint32_t start, uint32_t length) const {
#ifdef REMOVE_SIM
  printf("**** ERROR : ecmdDataBuffer: hasXstate: Not defined in this configuration\n");
  return false;
#else
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
  printf("**** ERROR : ecmdDataBuffer: getXstate: Not defined in this configuration");
  return '0';
#else
  if (i_bit >= iv_NumBits) {
    printf("**** ERROR : ecmdDataBuffer::getXstate: bit %d >= NumBits (%d)\n", i_bit, iv_NumBits);
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
  printf("**** ERROR : ecmdDataBuffer: setXstate: Not defined in this configuration");
  rc = ECMD_DBUF_XSTATE_ERROR;

#else
  if (i_bit >= iv_NumBits) {
    printf("**** ERROR : ecmdDataBuffer::setXstate: bit %d >= NumBits (%d)\n", i_bit, iv_NumBits);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
  } else {
    if (i_value == '0') rc = clearBit(i_bit);
    else if (i_value == '1') rc = setBit(i_bit);
    else if (!isxdigit(i_value)) {
      /* We call clearbit to write the raw bit to 0 */
      rc = clearBit(i_bit);
      iv_DataStr[i_bit] = i_value;
    } else {
      printf("**** ERROR : ecmdDataBuffer::setXstate: unrecognized Xstate character: %c\n", i_value);
      rc = ECMD_DBUF_XSTATE_ERROR;
    }
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
  printf("**** ERROR : ecmdDataBuffer: setXstate: Not defined in this configuration");
  rc = ECMD_DBUF_XSTATE_ERROR;

#else
  int len = strlen(i_datastr);
  int i;

  if (bitOffset+len > iv_NumBits) {
    printf("**** ERROR : ecmdDataBuffer::setXstate: bitOffset %d + len %d > NumBits (%d)\n", bitOffset, len, iv_NumBits);
    rc = ECMD_DBUF_BUFFER_OVERFLOW;
  } else {

    for (i = 0; i < len; i++) {
      if (i_datastr[i] == '0') rc = clearBit(bitOffset+i);
      else if (i_datastr[i] == '1') rc = setBit(bitOffset+i);
      else if (!isxdigit(i_datastr[i])) {
        rc = clearBit(bitOffset+i);
        iv_DataStr[bitOffset+i] = i_datastr[i];
      } 
      else {
        printf("**** ERROR : ecmdDataBuffer::setXstate: unrecognized Xstate character: %c\n", i_datastr[i]);
        rc = ECMD_DBUF_XSTATE_ERROR;
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

  /* cbytes is equal to the bit length of data */
  int cbytes = i_bytes < getBitLength() ? i_bytes : getBitLength();
  int index;
#ifdef REMOVE_SIM
  printf("**** ERROR : ecmdDataBuffer: memCopyInXstate: Not defined in this configuration");
  rc = ECMD_DBUF_XSTATE_ERROR;
#else
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
  int cbytes = i_bytes < getByteLength() ? i_bytes : getByteLength();
#ifdef REMOVE_SIM
  printf("**** ERROR : ecmdDataBuffer: memCopyOutXstate: Not defined in this configuration");
  rc = ECMD_DBUF_XSTATE_ERROR;
#else

  strncpy(o_buf, iv_DataStr, cbytes);
  o_buf[cbytes] = '\0';

#endif
  return rc;
}


//---------------------------------------------------------------------
//  Private Member Function Specifications
//---------------------------------------------------------------------
#ifndef REMOVE_SIM
uint32_t ecmdDataBuffer::fillDataStr(char fillChar) {
  if (iv_NumWords > 0) {
    memset(iv_DataStr, fillChar, iv_NumBits);
    iv_DataStr[iv_NumBits] = '\0';  
  }
  return ECMD_DBUF_SUCCESS;
}
#endif


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
  if (strncmp(iv_DataStr, other.iv_DataStr, iv_NumBits)) {
    return 0;
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
    printf("**** ERROR : ecmdDataBuffer::operater &: NumBits in (%d) do not match NumBits (%d)\n", other.iv_NumBits, iv_NumBits);
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
    printf("**** ERROR : extract: Number of bits to extract = 0\n");
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
