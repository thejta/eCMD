
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

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>

#include <ecmdDataBuffer.H>
#include <ecmdReturnCodes.H>

//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------
#ifndef SCANDATA_HEADER
#define SCANDATA_HEADER 0xBEEFBEEF
#endif

//----------------------------------------------------------------------
//  Forward declarations
//----------------------------------------------------------------------
void ecmdExtract(uint32_t *scr_ptr, uint32_t start_bit_num, uint32_t num_bits_to_extract, uint32_t *out_data_ptr);
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
: iv_NumWords(0), iv_NumBits(0), iv_Data(NULL), iv_DataOutStr(NULL), iv_ErrorCode(0), iv_ErrorMsg(NULL)
{
   registerErrorMsg(DBUF_INIT_FAIL,"ERROR: ecmdDataBuffer::ecmdDataBuffer: Default constructor used for ecmdDataBuffer creation!");
}

ecmdDataBuffer::ecmdDataBuffer(int numBits)
: iv_NumWords(1), iv_NumBits(numBits), iv_Data(NULL), iv_DataOutStr(NULL), iv_ErrorCode(0), iv_ErrorMsg(NULL)
{

  uint32_t randNum = 0x12345678;
  iv_Capacity = iv_NumWords = (iv_NumBits - 1) / 32 + 1;

  iv_RealData = new uint32_t[iv_Capacity + 10]; 
  iv_Data = iv_RealData + 4;
  memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */

#ifndef REMOVE_SIM_BUFFERS
  iv_DataStr = new char[iv_NumBits + 1];
  this->fillDataStr('0'); /* init to 0 */
  
#endif

  /* Ok, now setup the header, and tail */
  iv_RealData[0] = SCANDATA_HEADER;
  iv_RealData[1] = iv_NumWords;
  iv_RealData[3] = randNum;
  iv_RealData[iv_NumWords + 4] = randNum;

}

ecmdDataBuffer::ecmdDataBuffer(const ecmdDataBuffer& other) {

  this->setWordLength(other.iv_NumWords);
  for (int i = 0; i < iv_NumWords; i++) 
    iv_Data[i] = other.iv_Data[i];


  if (iv_DataOutStr != NULL) strcpy(iv_DataOutStr, other.iv_DataOutStr);

#ifndef REMOVE_SIM_BUFFERS
  strcpy(iv_DataStr, other.iv_DataStr);
#endif

}

//---------------------------------------------------------------------
//  Destructor
//---------------------------------------------------------------------
ecmdDataBuffer::~ecmdDataBuffer()
{

  delete[] iv_RealData;
  iv_RealData = NULL;
  if (iv_DataOutStr != NULL) delete[] iv_DataOutStr;
  iv_DataOutStr = NULL;

#ifndef REMOVE_SIM_BUFFERS
  delete[] iv_DataStr;
  iv_DataStr = NULL;
#endif

}

//---------------------------------------------------------------------
//  Public Member Function Specifications
//---------------------------------------------------------------------
int   ecmdDataBuffer::getWordLength() const { return iv_NumWords; }
int   ecmdDataBuffer::getBitLength() const { return iv_NumBits; }
int   ecmdDataBuffer::getCapacity() const { return iv_Capacity; }

void  ecmdDataBuffer::setWordLength(int newNumWords) {

  uint32_t randNum = 0x12345678;

  if (iv_Capacity < newNumWords) {  /* we need to resize iv_Data member */
    delete[] iv_RealData;
    iv_Capacity = iv_NumWords = newNumWords;

    iv_RealData = new uint32_t[iv_Capacity + 10]; 
    iv_Data = iv_RealData + 4;
    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */

    /* Ok, now setup the header, and tail */
    iv_RealData[0] = SCANDATA_HEADER;
    iv_RealData[1] = iv_NumWords;
    iv_RealData[3] = randNum;
    iv_RealData[iv_NumWords + 4] = randNum;

  } else { /* no need to resize */
    iv_NumWords = newNumWords;
    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */

    /* Ok, now setup the header, and tail */
    iv_RealData[0] = SCANDATA_HEADER;
    iv_RealData[1] = iv_NumWords;
    iv_RealData[3] = randNum;
    iv_RealData[iv_NumWords + 4] = randNum;
  }

  iv_NumBits = iv_NumWords * 32;  /* that's as accurate as we can get */

#ifndef REMOVE_SIM_BUFFERS
  if (!(strlen(this->iv_DataStr) > iv_NumBits)) { /* we need to resize the iv_DataStr member */
    delete[] iv_DataStr;
    iv_DataStr = new char[iv_NumBits+1];
  }
  this->fillDataStr('0'); /* init to 0 */
#endif
}  

void  ecmdDataBuffer::setBitLength(int newNumBits) {

  int newNumWords = (newNumBits - 1) / 32 + 1;
  uint32_t randNum = 0x12345678;

  if (iv_Capacity < newNumWords) {  /* we need to resize iv_Data member */
    delete[] iv_RealData;
    iv_Capacity = iv_NumWords = newNumWords;

    iv_RealData = new uint32_t[iv_Capacity + 10]; 
    iv_Data = iv_RealData + 4;
    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */

    /* Ok, now setup the header, and tail */
    iv_RealData[0] = SCANDATA_HEADER;
    iv_RealData[1] = iv_NumWords;
    iv_RealData[3] = randNum;
    iv_RealData[iv_NumWords + 4] = randNum;

  } else { /* no need to resize */

    iv_NumWords = newNumWords;

    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */

    /* Ok, now setup the header, and tail */
    iv_RealData[0] = SCANDATA_HEADER;
    iv_RealData[1] = iv_NumWords;
    iv_RealData[3] = randNum;
    iv_RealData[iv_NumWords + 4] = randNum;
  }

  iv_NumBits = newNumBits;

#ifndef REMOVE_SIM_BUFFERS
  if (!(strlen(this->iv_DataStr) > iv_NumBits)) { /* we need to resize the iv_DataStr member */
    delete[] iv_DataStr;
    iv_DataStr = new char[iv_NumBits+1];
  }
  this->fillDataStr('0'); /* init to 0 */
#endif

}  

void ecmdDataBuffer::setCapacity (int newCapacity) {

  /* only resize to make the capacity bigger */
  if (iv_Capacity < newCapacity) {
    uint32_t randNum = 0x12345678;

    iv_Capacity = newCapacity;
    delete[] iv_RealData;

   iv_RealData = new uint32_t[iv_Capacity + 10]; 
    iv_Data = iv_RealData + 4;
    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */

    /* Ok, now setup the header, and tail */
    iv_RealData[0] = SCANDATA_HEADER;
    iv_RealData[1] = iv_NumWords;
    iv_RealData[3] = randNum;
    iv_RealData[iv_NumWords + 4] = randNum;
  }

}

void  ecmdDataBuffer::setBit(int bit) {
  if (bit >= iv_NumBits) {
    char temp[50];
    sprintf(temp, "ecmdDataBuffer::setBit: bit %d >= NumBits (%d)\n", bit, iv_NumBits);
    registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
  } else {
    int index = bit/32;
    iv_Data[index] |= 0x00000001 << (31 - (bit-(index * 32)));
#ifndef REMOVE_SIM_BUFFERS
    iv_DataStr[bit] = '1';
#endif
  }
}

void  ecmdDataBuffer::setBit(int bit, int len) {
  if (bit+len > iv_NumBits) {
    char temp[60];
    sprintf(temp, "ecmdDataBuffer::setBit: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
    registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
    return;
  } else {
    int index = (bit + len) / 32;
    for (int idx = 0; idx < len; idx ++) this->setBit(bit + idx);    
  }
}


void  ecmdDataBuffer::setBit(int bitOffset, const char* binStr) {
#ifdef REMOVE_SIM_BUFFERS

#else
  int len = strlen(binStr);
  int i;

  if (bitOffset+len > iv_NumBits) {
    char temp[60];
    sprintf(temp, "ecmdDataBuffer::setBit: bitOffset %d + len %d > NumBits (%d)\n", bitOffset, len, iv_NumBits);
    registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
  } else {

    for (i = 0; i < len; i++) {
      if (binStr[i] == '0') clearBit(bitOffset+i);
      else if (binStr[i] == '1') setBit(bitOffset+i);
      else if (binStr[i] == 'x' || binStr[i] == 'X') {
        iv_DataStr[bitOffset+i] = 'X';
      } 
      else {
        char temp[60];
        sprintf(temp, "ecmdDataBuffer::setBit: unrecognized character: %c\n", binStr[i]);
        registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
      }
    }

  }
#endif

}


void  ecmdDataBuffer::setWord(int wordOffset, uint32_t value) {

  if (wordOffset >= iv_NumWords) {
    char temp[60];
    sprintf(temp, "ecmdDataBuffer::setWord: wordoffset %d >= NumWords (%d)\n", wordOffset, iv_NumWords);
    registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
  } else {
    iv_Data[wordOffset] = value;
    
#ifndef REMOVE_SIM_BUFFERS
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
}

void  ecmdDataBuffer::clearBit(int bit) {
  if (bit >= iv_NumBits) {
    char temp[50];
    sprintf(temp, "ecmdDataBuffer::clearBit: bit %d >= NumBits (%d)\n", bit, iv_NumBits);
    registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
  } else {  
    int index = bit/32;
    iv_Data[index] &= ~(0x00000001 << (31 - (bit-(index * 32))));
#ifndef REMOVE_SIM_BUFFERS
    iv_DataStr[bit] = '0';
#endif
  }
}

void  ecmdDataBuffer::clearBit(int bit, int len) {
  if (bit+len > iv_NumBits) {
    char temp[60];
    sprintf(temp, "ecmdDataBuffer::clearBit: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
    registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
  } else {
    for (int idx = 0; idx < len; idx ++) this->clearBit(bit + idx);    
  }
}

void  ecmdDataBuffer::flipBit(int bit) {
  if (bit >= iv_NumBits) {
    char temp[50];
    sprintf(temp, "ecmdDataBuffer::flipBit: bit %d >= NumBits (%d)\n", bit, iv_NumBits);
    registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
  } else {
#ifndef REMOVE_SIM_BUFFERS
    if (this->isXstate(bit, 1)) {
      char temp[60];
      sprintf(temp, "ecmdDataBuffer::flipBit: cannot flip non-binary data at bit %d\n", bit);
      registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
    } else {
#endif
      if (this->isBitSet(bit)) {
        this->clearBit(bit);      
      } else {
        this->setBit(bit);
      }
#ifndef REMOVE_SIM_BUFFERS
    }
#endif
  }
}

void  ecmdDataBuffer::flipBit(int bit, int len) {
  if (bit+len > iv_NumBits) {
    char temp[50];
    sprintf(temp, "ecmdDataBuffer::flipBit: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
    registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
    return;
  } else {
    char temp[60];
    for (int i = 0; i < len; i++) {
      this->flipBit(bit+i);
    }
  }
}

int   ecmdDataBuffer::isBitSet(int bit) {
  if (bit >= iv_NumBits) {
    char temp[50];
    sprintf(temp, "ecmdDataBuffer::isBitSet: bit %d >= NumBits (%d)\n", bit, iv_NumBits);
    registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
    return 0;
  } else {
#ifndef REMOVE_SIM_BUFFERS
    if (iv_DataStr[bit] != '1' && iv_DataStr[bit] != '0') {
      char temp[70];
      sprintf(temp, "ecmdDataBuffer::isBitSet: non-binary character detected in data at bit %d\n", bit);
      registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
      return 0;
    }
#endif
    int index = bit/32;
    return (iv_Data[index] & 0x00000001 << (31 - (bit-(index * 32)))); 
  }
}

int   ecmdDataBuffer::isBitSet(int bit, int len) {
  if (bit+len > iv_NumBits) {
    char temp[50];
    sprintf(temp, "ecmdDataBuffer::isBitSet: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
    registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
    return 0;
  } else {
    int rc = 1;
    for (int i = 0; i < len; i++) {
      if (!this->isBitSet(bit + i)) {
        rc = 0;
        break;
      }
    }
    return rc;
  }
}

int   ecmdDataBuffer::isBitClear(int bit) {
  if (bit >= iv_NumBits) {
    char temp[50];
    sprintf(temp, "ecmdDataBuffer::isBitClear: bit %d >= NumBits (%d)\n", bit, iv_NumBits);
    registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
    return 0;
  } else {
#ifndef REMOVE_SIM_BUFFERS
    if (iv_DataStr[bit] != '1' && iv_DataStr[bit] != '0') {
      registerErrorMsg(DBUF_BUFFER_OVERFLOW, "ecmdDataBuffer::isBitClear: non-binary character detected in data string\n");
      return 0;
    }
#endif
    int index = bit/32;
    return (!(iv_Data[index] & 0x00000001 << (31 - (bit-(index * 32))))); 
  }
}

int   ecmdDataBuffer::isBitClear(int bit, int len)
{
  if (bit+len > iv_NumBits) {
    char temp[50];
    sprintf(temp, "ecmdDataBuffer::isBitClear: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
    registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
    return 0;
  } else {
    int rc = 1;
    for (int i = 0; i < len; i++) {
      if (!this->isBitClear(bit + i)) {
        rc = 0;
        break;
      }
    }
    return rc;
  }
}

int   ecmdDataBuffer::getNumBitsSet(int bit, int len) {
  if (bit+len > iv_NumBits) {
    char temp[50];
    sprintf(temp, "ecmdDataBuffer::getNumBitsSet: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
    registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
    return 0;
  } else {
    int count = 0;
    for (int i = bit; i < len; i++) {
      if (this->isBitSet(bit)) count++;
    }
    return count;
  }
}

void   ecmdDataBuffer::shiftRight(int shiftNum) {

  uint32_t thisCarry;
  uint32_t prevCarry = 0x00000000;

  int i;

  // shift iv_Data array
  for (int iter = 0; iter < shiftNum; iter++) {
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

#ifndef REMOVE_SIM_BUFFERS
  // shift char
  char* temp = new char[iv_NumBits+1];
  for (i = 0; i < iv_NumBits; i++) temp[i] = '0'; // backfill with zeros
  temp[iv_NumBits] = '\0';
  strncpy(&temp[shiftNum], iv_DataStr, iv_NumBits-shiftNum);  
  strcpy(iv_DataStr, temp); // copy back into iv_DataStr
  delete[] temp;
#endif
}

void   ecmdDataBuffer::shiftLeft(int shiftNum) {

  uint32_t thisCarry;
  uint32_t prevCarry = 0x00000000;
  int i;

  // shift iv_Data array
  for (int iter = 0; iter < shiftNum; iter++) {
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

#ifndef REMOVE_SIM_BUFFERS
  // shift char
  char* temp = new char[iv_NumBits+1];
  for (i = 0; i < iv_NumBits; i++) temp[i] = '0'; // backfill with zeros
  temp[iv_NumBits] = '\0';
  strncpy(temp, &iv_DataStr[shiftNum], iv_NumBits-shiftNum);  
  strcpy(iv_DataStr, temp); // copy back into iv_DataStr
  delete[] temp;
#endif
}

void  ecmdDataBuffer::rotateRight(int rotateNum) {

  int lastBitSet;
  // rotate iv_Data
  for (int iter = 0; iter < rotateNum; iter++) {
    lastBitSet = this->isBitSet(iv_NumBits-1);   // save the last bit
    this->shiftRight(1);   // right-shift
    if (lastBitSet)    // insert into beginning
      this->setBit(0); 
    else
      this->clearBit(0);
  }
}

void  ecmdDataBuffer::rotateLeft(int rotateNum) {

  int firstBitSet;
  // rotate iv_Data
  for (int iter = 0; iter < rotateNum; iter++) {
    firstBitSet = this->isBitSet(0);   // save the first bit
    this->shiftLeft(1);   // left-shift
    if (firstBitSet)   // insert at the end
      this->setBit(iv_NumBits-1);
    else
      this->clearBit(iv_NumBits-1);
  }
}

void  ecmdDataBuffer::flushTo0() {
  memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */
#ifndef REMOVE_SIM_BUFFERS
  this->fillDataStr('0');
#endif
}

void  ecmdDataBuffer::flushTo1() {
  for (int i = 0; i < iv_NumWords; i++) iv_Data[i] = 0xFFFFFFFF;
#ifndef REMOVE_SIM_BUFFERS   
  this->fillDataStr('1');
#endif
}

void ecmdDataBuffer::invert() { 
  this->flipBit(0, iv_NumBits);
}

void  ecmdDataBuffer::insert(ecmdDataBuffer &bufferIn, int start, int len) {
  this->insert(bufferIn.iv_Data, start, len);
}

void  ecmdDataBuffer::insert(uint32_t *dataIn, int start, int len) {

  if (start+len > iv_NumBits) {
    char temp[50];
    sprintf(temp, "ecmdDataBuffer::insert: bit %d + len %d > iv_NumBits (%d)\n", start, len, iv_NumBits);
    registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
  } else {
    
    uint32_t mask = 0x80000000;
    for (int i = 0; i < len; i++) {
      if (dataIn[i/32] & mask) {
        this->setBit(start+i);
      }
      else { 
        this->clearBit(start+i);
      }

      mask >>= 1;
      if (mask == 0x00000000) {
        mask = 0x80000000;
      }
    }
  }
}

void  ecmdDataBuffer::insert(uint32_t dataIn, int start, int len) {
  this->insert(&dataIn, start, len);
}

void ecmdDataBuffer::extract(ecmdDataBuffer& bufferOut, int start, int len) {
  this->extract(bufferOut.iv_Data, start, len);
}

void ecmdDataBuffer::extract(uint32_t *dataOut, int start, int len) {

  if (len > iv_NumBits) {
    char temp[60];
    sprintf(temp, "ecmdDataBuffer::extract: len %d > NumBits (%d)\n", len, iv_NumBits);
    registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
  } else {

    ecmdExtract(this->iv_Data, start, len, dataOut);

#ifndef REMOVE_SIM_BUFFERS
    if (this->isXstate()) {  /* fast strchr */
      for (int i = start; i < len; i++) { /* now get exact bit */
        if (this->isXstate(start, 1)) {
          char temp[80];
          sprintf(temp, "ecmdDataBuffer::extract: Cannot extract non-binary character at bit %d\n", i);
          registerErrorMsg(DBUF_XSTATE_ERROR, temp);
        }
      }
    }
#endif
  }
}

void ecmdDataBuffer::setOr(ecmdDataBuffer& bufferIn, int startBit, int len) {
  this->setOr(bufferIn.iv_Data, startBit, len);
}

void ecmdDataBuffer::setOr(uint32_t * dataIn, int startBit, int len) {

  if (startBit + len > iv_NumBits) {
    char temp[50];
    sprintf(temp, "ecmdDataBuffer::setOr: bit %d + len %d > NumBits (%d)\n", startBit, len, iv_NumBits);
    registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
  } else {
    uint32_t mask = 0x80000000;
    for (int i = 0; i < len; i++) {
      if (dataIn[i/32] & mask) {
        this->setBit(startBit + i);
      }
      mask >>= 1;
      if (mask == 0x00000000) {
        mask = 0x80000000;
      }
    }
  }  
}

void ecmdDataBuffer::setOr(uint32_t dataIn, int startBit, int len) {
  this->setOr(&dataIn, startBit, len);
}

void ecmdDataBuffer::merge(ecmdDataBuffer& bufferIn) {
  if (iv_NumBits != bufferIn.iv_NumBits) {
    char temp[60];
    sprintf(temp, "ecmdDataBuffer::merge: NumBits in (%d) do not match NumBits (%d)\n", bufferIn.iv_NumBits, iv_NumBits);
    registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
  } else {
    this->setOr(bufferIn, 0, iv_NumBits);
  }
}

void ecmdDataBuffer::setAnd(ecmdDataBuffer& bufferIn, int startBit, int len) {
  this->setAnd(bufferIn.iv_Data, startBit, len);
}

void ecmdDataBuffer::setAnd(uint32_t * dataIn, int startBit, int len) {
  if (startBit + len > iv_NumBits) {
    char temp[60];
    sprintf(temp, "ecmdDataBuffer::setAnd: bit %d + len %d > iv_NumBits (%d)\n", startBit, len, iv_NumBits);
    registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
  } else {
    uint32_t mask = 0x80000000;
    for (int i = 0; i < len; i++) {
      if (!(dataIn[i/32] & mask)) {
        this->clearBit(startBit + i);
      }
      mask >>= 1;
      if (mask == 0x00000000) {
        mask = 0x80000000;
      }
    }
  }
}

void ecmdDataBuffer::setAnd(uint32_t dataIn, int startBit, int len) {
  this->setAnd(&dataIn, startBit, len);
}

int   ecmdDataBuffer::oddParity(int start, int stop) {

  int charOffset;
  int posOffset;
  int counter;
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

int   ecmdDataBuffer::evenParity(int start, int stop) {
  if (this->oddParity(start, stop))
    return 0;
  else
    return 1;
}

int   ecmdDataBuffer::oddParity(int start, int stop, int insertPos) {
  if (this->oddParity(start,stop))
    this->setBit(insertPos);
  else 
    this->clearBit(insertPos);
  return 0;
}

int   ecmdDataBuffer::evenParity(int start, int stop, int insertPos) {
  if (this->evenParity(start,stop))
    this->setBit(insertPos);
  else
    this->clearBit(insertPos);
  return 0;
}

uint32_t ecmdDataBuffer::getWord(int wordOffset) {
  if (wordOffset > iv_NumWords-1) {
    char temp[50];
    sprintf(temp, "ecmdDataBuffer::getWord: wordOffset %d > NumWords-1 (%d)\n", wordOffset, iv_NumWords-1);
    registerErrorMsg(DBUF_BUFFER_OVERFLOW, temp);
    return 0;
  }
  return this->iv_Data[wordOffset];
}

const char* ecmdDataBuffer::genHexLeftStr(int start, int bitLen) {

  int tempNumWords = (bitLen - 1)/32 + 1;
  int lastnibble = (bitLen - 1)/4 + 1;
  char* cPtr = iv_DataOutStr;

  /* resize iv_DataOutStr if necessary */
  if (iv_DataOutStr == NULL) {
    iv_DataOutStr = new char[tempNumWords*8+1];
    cPtr = iv_DataOutStr;
  } else if (strlen(iv_DataOutStr) < tempNumWords*8) {
    delete[] iv_DataOutStr;
    iv_DataOutStr = new char[tempNumWords*8+1];
    cPtr = iv_DataOutStr;
  }

  /* extract iv_Data */
  uint32_t* tempData = new uint32_t[tempNumWords];
  this->extract(&tempData[0], start, bitLen);

  for (int w = 0; w < tempNumWords; w++) {
    sprintf(cPtr, "%.8X", tempData[w]);
    cPtr = &cPtr[8];
  }

  iv_DataOutStr[lastnibble] = '\0';
  delete[] tempData;
  return iv_DataOutStr;
}

const char* ecmdDataBuffer::genHexRightStr(int start, int bitLen) {

  int tempNumWords = (bitLen - 1)/32 + 1;
  int lastNibble = (bitLen - 1)/4 + 1;

  tempNumWords++;
  ecmdDataBuffer padded(bitLen+32); /* pad with extra word */
  padded.setWordLength(tempNumWords);

  /* resize iv_DataOutStr if necessary */
  if (iv_DataOutStr == NULL) {
    iv_DataOutStr = new char[tempNumWords*8+1];
  } else if (strlen(iv_DataOutStr) < tempNumWords*8) {
    delete[] iv_DataOutStr;
    iv_DataOutStr = new char[tempNumWords*8+1];
  }

  int shiftAmt = 32 - bitLen % 32;
  extract(padded, start, bitLen);
  padded.shiftRight(shiftAmt); /* fill left side with zeros */
  strcpy(iv_DataOutStr, padded.genHexLeftStr());  /* grab hex string into iv_DataOutStr */
  int offsetNibble = (shiftAmt - 1)/4;
  iv_DataOutStr = &iv_DataOutStr[offsetNibble];  /* chop off left side */
  iv_DataOutStr[lastNibble] = '\0'; /* chop off right side */
  return iv_DataOutStr;
}

const char* ecmdDataBuffer::genBinStr(int start, int bitLen) {

  /* resize iv_DataOutStr if necessary */
  if (iv_DataOutStr == NULL) {
    iv_DataOutStr = new char[bitLen+1];
  } else if (strlen(iv_DataOutStr) < bitLen) {
    delete[] iv_DataOutStr;
    iv_DataOutStr = new char[bitLen+1];
  }

#ifndef REMOVE_SIM_BUFFERS
  // iv_DataStr should already have updated iv_Data
  strncpy(iv_DataOutStr, &iv_DataStr[start], bitLen);
#endif

  iv_DataOutStr[bitLen] = '\0';
  return iv_DataOutStr;    
}

const char* ecmdDataBuffer::genHexLeftStr() { return this->genHexLeftStr(0, iv_NumBits); }
const char* ecmdDataBuffer::genHexRightStr() { return this->genHexRightStr(0, iv_NumBits); }
const char* ecmdDataBuffer::genBinStr() { return this->genBinStr(0, iv_NumBits); }


void ecmdDataBuffer::copy(ecmdDataBuffer &newCopy) {

  newCopy.setWordLength(iv_NumWords);
  // iv_Data
  for (int i = 0; i < iv_NumWords; i++) {
    newCopy.iv_Data[i] = iv_Data[i];
  }
  // char

#ifndef REMOVE_SIM_BUFFERS
  strcpy(newCopy.iv_DataStr, iv_DataStr);
#endif

}

void  ecmdDataBuffer::memCopyIn(uint32_t* buf, int bytes) { /* Does a memcpy from supplied buffer into ecmdDataBuffer */
  ecmdBigEndianMemCopy(iv_Data, buf, bytes);
}
void  ecmdDataBuffer::memCopyOut(uint32_t* buf, int bytes) { /* Does a memcpy from ecmdDataBuffer into supplied buffer */
  ecmdBigEndianMemCopy(buf, iv_Data, bytes);
}


int   ecmdDataBuffer::isXstate() {  /* check for only X's */
#ifdef REMOVE_SIM_BUFFERS
  registerErrorMsg(DBUF_UNDEFINED_FUNCTION, "ecmdDataBuffer: isXstate: Not defined in this configuration");
  return 0;
#else
  return (strchr(iv_DataStr, 'x') || strchr(iv_DataStr, 'X'));
#endif
}

// actually use this for ANY non-binary char, not just X's
int   ecmdDataBuffer::isXstate(int start, int length) {
#ifdef REMOVE_SIM_BUFFERS
  registerErrorMsg(DBUF_UNDEFINED_FUNCTION, "ecmdDataBuffer: isXstate: Not defined in this configuration");
  return 0;
#else
  int stopBit = start + length;
  int minStop = iv_NumBits < stopBit ? iv_NumBits : stopBit; /* pick the smallest */

  for (int i = start; i < minStop; i++) {
    if (iv_DataStr[i] != '0' && iv_DataStr[i] != '1')
      return 1;
  }
  return 0;
#endif
}

char * ecmdDataBuffer::getErrorMsg (int errorCode) {
  return iv_ErrorMsg;
}

int ecmdDataBuffer::registerErrorMsg (int errorCode, char * message) {
  iv_ErrorCode = errorCode;
  iv_ErrorMsg = message;
  return 0;
}

//---------------------------------------------------------------------
//  Private Member Function Specifications
//---------------------------------------------------------------------
#ifndef REMOVE_SIM_BUFFERS
void ecmdDataBuffer::fillDataStr(char fillChar) {

  for (int i = 0; i < iv_NumBits; i++) iv_DataStr[i] = fillChar;
  iv_DataStr[iv_NumBits] = '\0';  
  
}
#endif

void ecmdExtract(uint32_t *scr_ptr, uint32_t start_bit_num, uint32_t num_bits_to_extract, uint32_t *out_iv_Data_ptr)
{
  uint32_t i;   
  uint32_t temp;
  uint32_t len; 
  uint32_t mask1;
  uint32_t mask2;
  uint32_t offset;
  uint32_t index; 
  int   count; 

  /*------------------------------------------------------------------*/
  /* calculate number of fws (32-bit pieces) of the destination buffer*/
  /* to be processed.                                                 */
  /*----------------------------line 98--------------------------------*/

  if (num_bits_to_extract == 0){
    printf("ERROR: extract: Number of bits to extract = 0\n");
    out_iv_Data_ptr = NULL;
    return;
  }

  count = ((num_bits_to_extract-1)/32) + 1;

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
  return;
}

void * ecmdBigEndianMemCopy(void * dest, const void *src, size_t count)
{
#ifdef __GNU__
  char *tmp = (char *) dest, *s = (char *) src;
  int remainder = 0, whole_num = 0;

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
