
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

#include <ecmdDataBuffer.H>
#include <ecmdReturnCodes.H>

//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------
#ifndef DATABUFFER_HEADER
#define DATABUFFER_HEADER 0xBEEFBEEF
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
: iv_NumWords(0), iv_NumBits(0), iv_Data(NULL), iv_RealData(NULL), iv_Capacity(0)
{
#ifndef REMOVE_SIM
  iv_DataStr = NULL;
  iv_isXstate = 0;
#endif
}

ecmdDataBuffer::ecmdDataBuffer(int numWords)
: iv_NumWords(numWords), iv_NumBits(numWords*32), iv_Capacity(0), iv_RealData(NULL), iv_Data(NULL)
{
#ifndef REMOVE_SIM
  iv_DataStr = NULL;
  iv_isXstate = 0;
#endif
  setWordLength(numWords);

}

ecmdDataBuffer::ecmdDataBuffer(const ecmdDataBuffer& other) {

  this->setWordLength(other.iv_NumWords);
  for (int i = 0; i < iv_NumWords; i++) 
    iv_Data[i] = other.iv_Data[i];


#ifndef REMOVE_SIM
  strcpy(iv_DataStr, other.iv_DataStr);
  iv_isXstate = other.iv_isXstate;
#endif

}

//---------------------------------------------------------------------
//  Destructor
//---------------------------------------------------------------------
ecmdDataBuffer::~ecmdDataBuffer()
{

  if ((iv_RealData != NULL)) {
    /* Let's check our header,tail info */
    if ((iv_RealData[0] != DATABUFFER_HEADER) || (iv_RealData[1] != iv_NumWords) || (iv_RealData[3] != iv_RealData[iv_NumWords + 4])) {
      /* Ok, something is wrong here */
      printf("iv_RealData[0]: %X, iv_RealData[1]: %X, iv_NumWords: %X\n",iv_RealData[0],iv_RealData[1],iv_NumWords);
      printf("iv_RealData[3]: %X, iv_RealData[iv_NumWords + 4]: %X\n",iv_RealData[3],iv_RealData[iv_NumWords + 4]);
      printf("**** SEVERE ERROR (ecmdDataBuffer) : PROBLEM WITH DATABUFFER - INVALID HEADER/TAIL\n");
      exit(1);
    }

    delete[] iv_RealData;
    iv_RealData = NULL;

  }

#ifndef REMOVE_SIM
  if (iv_DataStr != NULL) {
    delete[] iv_DataStr;
    iv_DataStr = NULL;
  }
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

  iv_NumWords = newNumWords;
  iv_NumBits = iv_NumWords * 32;  /* that's as accurate as we can get */

  if (iv_Capacity < newNumWords) {  /* we need to resize iv_Data member */

    if (iv_RealData != NULL)
      delete[] iv_RealData;

    iv_Capacity = newNumWords;

    iv_RealData = new uint32_t[iv_Capacity + 10]; 
    iv_Data = iv_RealData + 4;
    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */

#ifndef REMOVE_SIM
    if (iv_DataStr != NULL)
      delete[] iv_DataStr;

    iv_DataStr = new char[iv_NumBits + 42];
    this->fillDataStr('0'); /* init to 0 */
    iv_isXstate = 0;
#endif

    /* Ok, now setup the header, and tail */
    iv_RealData[0] = DATABUFFER_HEADER;
    iv_RealData[1] = iv_NumWords;
    iv_RealData[3] = randNum;
    iv_RealData[iv_NumWords + 4] = randNum;

  } else { /* no need to resize */
    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */

#ifndef REMOVE_SIM
    this->fillDataStr('0'); /* init to 0 */
    iv_isXstate = 0;
#endif

    /* Ok, now setup the header, and tail */
    iv_RealData[0] = DATABUFFER_HEADER;
    iv_RealData[1] = iv_NumWords;
    iv_RealData[3] = randNum;
    iv_RealData[iv_NumWords + 4] = randNum;
  }


}  

void  ecmdDataBuffer::setBitLength(int newNumBits) {

  int newNumWords = (newNumBits - 1) / 32 + 1;
  uint32_t randNum = 0x12345678;

  iv_NumWords = newNumWords;
  iv_NumBits = newNumBits;

  if (iv_Capacity < newNumWords) {  /* we need to resize iv_Data member */
    if (iv_RealData != NULL)
      delete[] iv_RealData;
    iv_Capacity = newNumWords;

    iv_RealData = new uint32_t[iv_Capacity + 10]; 
    iv_Data = iv_RealData + 4;
    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */

#ifndef REMOVE_SIM
    if (iv_DataStr != NULL)
      delete[] iv_DataStr;

    iv_DataStr = new char[iv_NumBits + 42];
    this->fillDataStr('0'); /* init to 0 */
    iv_isXstate = 0;
#endif

    /* Ok, now setup the header, and tail */
    iv_RealData[0] = DATABUFFER_HEADER;
    iv_RealData[1] = iv_NumWords;
    iv_RealData[3] = randNum;
    iv_RealData[iv_NumWords + 4] = randNum;

  } else { /* no need to resize */

    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */

#ifndef REMOVE_SIM
    this->fillDataStr('0'); /* init to 0 */
    iv_isXstate = 0;
#endif

    /* Ok, now setup the header, and tail */
    iv_RealData[0] = DATABUFFER_HEADER;
    iv_RealData[1] = iv_NumWords;
    iv_RealData[3] = randNum;
    iv_RealData[iv_NumWords + 4] = randNum;
  }



}  

void ecmdDataBuffer::setCapacity (int newCapacity) {

  /* only resize to make the capacity bigger */
  if (iv_Capacity < newCapacity) {
    uint32_t randNum = 0x12345678;

    iv_Capacity = newCapacity;
    if (iv_RealData != NULL)
      delete[] iv_RealData;

    iv_RealData = new uint32_t[iv_Capacity + 10]; 
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
    iv_DataStr = new char[iv_NumBits+42];
  
    this->fillDataStr('0'); /* init to 0 */
#endif

  }

}

void  ecmdDataBuffer::setBit(int bit) {
  if (bit >= iv_NumBits) {
    printf("ecmdDataBuffer::setBit: bit %d >= NumBits (%d)\n", bit, iv_NumBits);
  } else {
    int index = bit/32;
    iv_Data[index] |= 0x00000001 << (31 - (bit-(index * 32)));
#ifndef REMOVE_SIM
    iv_DataStr[bit] = '1';
#endif
  }
}

void  ecmdDataBuffer::setBit(int bit, int len) {
  if (bit+len > iv_NumBits) {
    printf("ecmdDataBuffer::setBit: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
    return;
  } else {
    int index = (bit + len) / 32;
    for (int idx = 0; idx < len; idx ++) this->setBit(bit + idx);    
  }
}


void  ecmdDataBuffer::setBit(int bitOffset, const char* binStr) {
#ifdef REMOVE_SIM

#else
  int len = strlen(binStr);
  int i;

  if (bitOffset+len > iv_NumBits) {
    printf("ecmdDataBuffer::setBit: bitOffset %d + len %d > NumBits (%d)\n", bitOffset, len, iv_NumBits);
  } else {

    for (i = 0; i < len; i++) {
      if (binStr[i] == '0') clearBit(bitOffset+i);
      else if (binStr[i] == '1') setBit(bitOffset+i);
      else if (binStr[i] == 'x' || binStr[i] == 'X') {
        iv_DataStr[bitOffset+i] = 'X';
      } 
      else {
        printf("ecmdDataBuffer::setBit: unrecognized character: %c\n", binStr[i]);
      }
    }

  }
#endif

}


void  ecmdDataBuffer::setWord(int wordOffset, uint32_t value) {

  if (wordOffset >= iv_NumWords) {
    printf("ecmdDataBuffer::setWord: wordoffset %d >= NumWords (%d)\n", wordOffset, iv_NumWords);
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
}

void  ecmdDataBuffer::clearBit(int bit) {
  if (bit >= iv_NumBits) {
    printf("ecmdDataBuffer::clearBit: bit %d >= NumBits (%d)\n", bit, iv_NumBits);
  } else {  
    int index = bit/32;
    iv_Data[index] &= ~(0x00000001 << (31 - (bit-(index * 32))));
#ifndef REMOVE_SIM
    iv_DataStr[bit] = '0';
#endif
  }
}

void  ecmdDataBuffer::clearBit(int bit, int len) {
  if (bit+len > iv_NumBits) {
    printf("ecmdDataBuffer::clearBit: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
  } else {
    for (int idx = 0; idx < len; idx ++) this->clearBit(bit + idx);    
  }
}

void  ecmdDataBuffer::flipBit(int bit) {
  if (bit >= iv_NumBits) {
    printf("ecmdDataBuffer::flipBit: bit %d >= NumBits (%d)\n", bit, iv_NumBits);
  } else {
#ifndef REMOVE_SIM
    if (this->hasXstate(bit, 1)) {
      printf("ecmdDataBuffer::flipBit: cannot flip non-binary data at bit %d\n", bit);
    } else {
#endif
      if (this->isBitSet(bit)) {
        this->clearBit(bit);      
      } else {
        this->setBit(bit);
      }
#ifndef REMOVE_SIM
    }
#endif
  }
}

void  ecmdDataBuffer::flipBit(int bit, int len) {
  if (bit+len > iv_NumBits) {
    printf("ecmdDataBuffer::flipBit: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
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
    printf("ecmdDataBuffer::isBitSet: bit %d >= NumBits (%d)\n", bit, iv_NumBits);
    return 0;
  } else {
#ifndef REMOVE_SIM
    if (iv_DataStr[bit] != '1' && iv_DataStr[bit] != '0') {
      printf("ecmdDataBuffer::isBitSet: non-binary character detected in data at bit %d\n", bit);
      return 0;
    }
#endif
    int index = bit/32;
    return (iv_Data[index] & 0x00000001 << (31 - (bit-(index * 32)))); 
  }
}

int   ecmdDataBuffer::isBitSet(int bit, int len) {
  if (bit+len > iv_NumBits) {
    printf("ecmdDataBuffer::isBitSet: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
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
    printf("ecmdDataBuffer::isBitClear: bit %d >= NumBits (%d)\n", bit, iv_NumBits);
    return 0;
  } else {
#ifndef REMOVE_SIM
    if (iv_DataStr[bit] != '1' && iv_DataStr[bit] != '0') {
      printf( "ecmdDataBuffer::isBitClear: non-binary character detected in data string\n");
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
    printf("ecmdDataBuffer::isBitClear: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
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
    printf("ecmdDataBuffer::getNumBitsSet: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
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

  iv_NumBits += shiftNum;

#ifndef REMOVE_SIM
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

  /* If we are going to shift off the end we can just clear everything out */
  if (shiftNum >= iv_NumBits) {
    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */
    this->fillDataStr('0'); /* init to 0 */
    iv_NumBits = iv_NumWords = 0;
    return;
  }

  // shift iv_Data array
  for (int iter = 0; iter < shiftNum; iter++) {
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

  iv_NumBits -= shiftNum;

#ifndef REMOVE_SIM
  // shift char
  char* temp = new char[iv_NumBits+1];
  for (i = 0; i < iv_NumBits; i++) temp[i] = '0'; // backfill with zeros
  temp[iv_NumBits] = '\0';
  strncpy(temp, &iv_DataStr[shiftNum], iv_NumBits);  
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
#ifndef REMOVE_SIM
  this->fillDataStr('0');
#endif
}

void  ecmdDataBuffer::flushTo1() {
  for (int i = 0; i < iv_NumWords; i++) iv_Data[i] = 0xFFFFFFFF;
#ifndef REMOVE_SIM   
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
    printf(temp);
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
  bufferOut.setBitLength(len);
  this->extract(bufferOut.iv_Data, start, len);
}

void ecmdDataBuffer::extract(uint32_t *dataOut, int start, int len) {

  if (len > iv_NumBits) {
    char temp[100];
    printf( "ecmdDataBuffer::extract: len %d > NumBits (%d)\n", len, iv_NumBits);
  } else {

    ecmdExtract(this->iv_Data, start, len, dataOut);

#ifndef REMOVE_SIM
    if (this->hasXstate()) {  /* fast strchr */
      for (int i = start; i < len; i++) { /* now get exact bit */
        if (this->hasXstate(start, 1)) {
          char temp[100];
          printf("ecmdDataBuffer::extract: Cannot extract non-binary character at bit %d\n", i);
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
    char temp[100];
    printf("ecmdDataBuffer::setOr: bit %d + len %d > NumBits (%d)\n", startBit, len, iv_NumBits);
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
    char temp[100];
    printf("ecmdDataBuffer::merge: NumBits in (%d) do not match NumBits (%d)\n", bufferIn.iv_NumBits, iv_NumBits);
  } else {
    this->setOr(bufferIn, 0, iv_NumBits);
  }
}

void ecmdDataBuffer::setAnd(ecmdDataBuffer& bufferIn, int startBit, int len) {
  this->setAnd(bufferIn.iv_Data, startBit, len);
}

void ecmdDataBuffer::setAnd(uint32_t * dataIn, int startBit, int len) {
  if (startBit + len > iv_NumBits) {
    char temp[100];
    printf("ecmdDataBuffer::setAnd: bit %d + len %d > iv_NumBits (%d)\n", startBit, len, iv_NumBits);
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
    char temp[100];
    printf("ecmdDataBuffer::getWord: wordOffset %d > NumWords-1 (%d)\n", wordOffset, iv_NumWords-1);
    return 0;
  }
  return this->iv_Data[wordOffset];
}

std::string ecmdDataBuffer::genHexLeftStr(int start, int bitLen) {

  int tempNumWords = (bitLen - 1)/32 + 1;
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

  int overCount = (32*tempNumWords - bitLen) / 4;

  if (overCount > 0) {
    ret.erase(ret.length() - overCount, ret.length()-1);
  }

  return ret;
}

std::string ecmdDataBuffer::genHexRightStr(int start, int bitLen) {

  int tempNumWords = (bitLen - 1)/32 + 1;
//  int lastNibble = (bitLen - 1)/4 + 1;
  std::string ret;
  int offsetNibble = 0;

  tempNumWords++;
  ecmdDataBuffer padded(tempNumWords); 

  /* resize iv_DataOutStr if necessary */

  int shiftAmt = 32 - (bitLen % 32);
  extract(padded, start, bitLen);
  if (shiftAmt % 32) {
    padded.shiftRight(shiftAmt); /* fill left side with zeros */
    offsetNibble = shiftAmt/4;
  }
  ret = padded.genHexLeftStr();  /* grab hex string into iv_DataOutStr */

  //iv_DataOutStr = &iv_DataOutStr[offsetNibble];  /* chop off left side */
  //iv_DataOutStr[lastNibble] = '\0'; /* chop off right side */

  if (offsetNibble > 0) {
    ret.erase(0, offsetNibble);
  }

//  ret.erase(lastNibble, ret.length());

  return ret;
}

std::string ecmdDataBuffer::genBinStr(int start, int bitLen) {

  int tempNumWords = (bitLen - 1)/32 + 1;
  std::string ret;
  /* extract iv_Data */
  uint32_t* tempData = new uint32_t[tempNumWords];
  this->extract(&tempData[0], start, bitLen);
  uint32_t mask = 0x80000000;
  int curWord = 0;

  for (int w = 0; w < bitLen; w++) {
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

  return ret;
}

std::string ecmdDataBuffer::genXstateStr(int start, int bitLen) { 
  std::string ret;
#ifndef REMOVE_SIM
  char * copyStr = new char[strlen(iv_DataStr)];
  strcpy(copyStr, &iv_DataStr[start]);
  if (bitLen < strlen(copyStr)) {
    copyStr[bitLen+1] = '\0';
  }

  ret = copyStr;
#else
  printf( "ecmdDataBuffer: genXstateStr: Not defined in this configuration");
#endif
  return ret;
}

std::string ecmdDataBuffer::genHexLeftStr() { return this->genHexLeftStr(0, iv_NumBits); }
std::string ecmdDataBuffer::genHexRightStr() { return this->genHexRightStr(0, iv_NumBits); }
std::string ecmdDataBuffer::genBinStr() { return this->genBinStr(0, iv_NumBits); }
std::string ecmdDataBuffer::genXstateStr() { return this->genXstateStr(0, iv_NumBits); }

int ecmdDataBuffer::insertFromHexLeft (const char * i_hexChars, int start, int length) {
  int rc = ECMD_SUCCESS;
  int i;

  int strLen = strlen(i_hexChars);

  if (length == 0) {

    if (strLen > 0) {
      length = strLen * 4;
    }
    else {
      //error out
      return ECMD_DBUF_INVALID_ARGS;
    }

  }

  int wordLength = (strLen - 1)/8 + 1;

  uint32_t * number_ptr = new uint32_t[wordLength];
  for (i = 0; i < wordLength; i++) {
    number_ptr[i] = 0x0;
  }

  uint32_t tmpb32 = 0x0;
  char * nextOne = new char[2];
  nextOne[1] = '\0';
  for (i = 0; i < strLen; i++) {
    nextOne[0] = i_hexChars[i];
    tmpb32 = strtoul(&nextOne[0], NULL, 16);
    number_ptr[i>>3] |= tmpb32 << (28 - (i << 2));
  }

  this->insert(number_ptr, start, length);

  delete[] nextOne;
  delete[] number_ptr;

  return rc;
}

int ecmdDataBuffer::insertFromHexRight (const char * i_hexChars, int expectedLength, int start, int length) {
  int rc = ECMD_SUCCESS;
  int i;

  int strLen = strlen(i_hexChars);

  //if the string isn't aligned along word/byte boundaries, we
  //move it over
  if (expectedLength > 0) {
    start += expectedLength - strLen*4;
  }

  if (length == 0) {

    if (strLen > 0) {
      length = strLen * 4;
    }
    else {
      //error out
      return ECMD_DBUF_INVALID_ARGS;
    }

  }

  int wordLength = (strLen - 1)/8 + 1;

  uint32_t * number_ptr = new uint32_t[wordLength];
  for (i = 0; i < wordLength; i++) {
    number_ptr[i] = 0x0;
  }

  uint32_t tmpb32 = 0x0;
  char * nextOne = new char[2];
  nextOne[1] = '\0';
  for (i = 0; i < strLen; i++) {
    nextOne[0] = i_hexChars[i];
    tmpb32 = strtoul(&nextOne[0], NULL, 16);
    number_ptr[i>>3] |= tmpb32 << (28 - (i << 2));
  }

  this->insert(number_ptr, start, length);

  delete[] nextOne;
  delete[] number_ptr;

  return rc;
}

int ecmdDataBuffer::insertFromBin (const char * i_binChars, int start) {
  int rc = ECMD_SUCCESS;

  int strLen = strlen(i_binChars);

  for (int i = 0; i < strLen; i++) {
    if (i_binChars[i] == '0') {
      this->clearBit(start+i);
    }
    else {
      this->setBit(start+i);
    }
  }

  return rc;
}

void ecmdDataBuffer::copy(ecmdDataBuffer &newCopy) {

  newCopy.setBitLength(iv_NumBits);
  // iv_Data
  for (int i = 0; i < iv_NumWords; i++) {
    newCopy.iv_Data[i] = iv_Data[i];
  }
  // char

#ifndef REMOVE_SIM
  strncpy(newCopy.iv_DataStr, iv_DataStr, iv_NumBits);
#endif

}

/* Copy Constructor */
int ecmdDataBuffer::operator=(ecmdDataBuffer & i_master) {

  setBitLength(i_master.iv_NumBits);
  // iv_Data
  for (int i = 0; i < i_master.iv_NumWords; i++) {
    iv_Data[i] = i_master.iv_Data[i];
  }
  // char

#ifndef REMOVE_SIM
  strncpy(iv_DataStr, i_master.iv_DataStr, i_master.iv_NumBits);
#endif
  return 0;
}


void  ecmdDataBuffer::memCopyIn(uint32_t* buf, int bytes) { /* Does a memcpy from supplied buffer into ecmdDataBuffer */
  setBitLength(bytes * 8);
  ecmdBigEndianMemCopy(iv_Data, buf, bytes);
#ifndef REMOVE_SIM
  strcpy(iv_DataStr, genBinStr(0,bytes*4).c_str());
#endif
}
void  ecmdDataBuffer::memCopyOut(uint32_t* buf, int bytes) { /* Does a memcpy from ecmdDataBuffer into supplied buffer */
  ecmdBigEndianMemCopy(buf, iv_Data, bytes);
}


int ecmdDataBuffer::setXstate (int state) {
#ifdef REMOVE_SIM
  printf("ecmdDataBuffer: setXstate: Not defined in this configuration");
  return ECMD_DBUF_UNDEFINED_FUNCTION;
#else
  if (state == 0 || state == 1) {
    iv_isXstate = state;
    return 0;
  }
  else {
    printf( "ecmdDataBuffer: setXstate: Invalid state- must be 0 or 1");
    return ECMD_DBUF_INVALID_ARGS;
  }
#endif
}

int  ecmdDataBuffer::isXstate() {
#ifdef REMOVE_SIM
  printf( "ecmdDataBuffer: isXstate: Not defined in this configuration");
  return 0;
#else
  return iv_isXstate;
#endif
}

int   ecmdDataBuffer::hasXstate() {  
#ifdef REMOVE_SIM
  printf( "ecmdDataBuffer: hasXstate: Not defined in this configuration");
  return 0;
#else
  return (hasXstate(0,iv_NumBits));
#endif
}

// actually use this for ANY non-binary char, not just X's
int   ecmdDataBuffer::hasXstate(int start, int length) {
#ifdef REMOVE_SIM
  printf("ecmdDataBuffer: hasXstate: Not defined in this configuration");
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

//---------------------------------------------------------------------
//  Private Member Function Specifications
//---------------------------------------------------------------------
#ifndef REMOVE_SIM
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
