
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
#include <stdint.h>

#include <ecmdDataBuffer.H>
#include <ecmdClientCapi.H>

//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------
#ifndef SCANDATA_HEADER
#define SCANDATA_HEADER 0xBEEFBEEF
#endif

uint32_t ECMD_MASK[]
= {
  0x80000000, 0x40000000, 0x20000000, 0x10000000,
  0x08000000, 0x04000000, 0x02000000, 0x01000000,
  0x00800000, 0x00400000, 0x00200000, 0x00100000,
  0x00080000, 0x00040000, 0x00020000, 0x00010000,
  0x00008000, 0x00004000, 0x00002000, 0x00001000,
  0x00000800, 0x00000400, 0x00000200, 0x00000100,
  0x00000080, 0x00000040, 0x00000020, 0x00000010,
  0x00000008, 0x00000004, 0x00000002, 0x00000001};


//----------------------------------------------------------------------
//  Forward declarations
//----------------------------------------------------------------------
void myextract(uint32_t *scr_ptr, uint32_t start_bit_num, uint32_t num_bits_to_extract, uint32_t *out_data_ptr);
void * mybigEndianMemcpy(void * dest, const void *src, size_t count);

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
: iv_NumWords(0), iv_NumBits(0), iv_Data(NULL), iv_DataStr(NULL), iv_DataOutStr(NULL)
{
   ecmdRegisterErrorMsg(ECMD_FAILURE,"ERROR: ecmdDataBuffer::ecmdDataBuffer: Default constructor used for ecmdDataBuffer creation!\n");
}

ecmdDataBuffer::ecmdDataBuffer(int numBits)
: iv_NumWords(1), iv_NumBits(numBits), iv_Data(NULL), iv_DataStr(NULL), iv_DataOutStr(NULL)
{

  uint32_t randNum = 0x12345678;
  iv_NumWords = (iv_NumBits - 1) / 32 + 1;

  iv_RealData = new uint32_t[iv_NumWords + 10]; 
  iv_Data = iv_RealData + 4;
  memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */

  iv_DataStr = new char[iv_NumBits + 1];
  this->fillDataStr('0'); /* init to 0 */

// we don't really need to do this right now 
//  iv_DataOutStr = new char[iv_NumBits + 1];
//  strcpy(iv_DataOutStr, iv_DataStr);

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
  strcpy(iv_DataStr, other.iv_DataStr);

}

//---------------------------------------------------------------------
//  Destructor
//---------------------------------------------------------------------
ecmdDataBuffer::~ecmdDataBuffer()
{

  delete[] iv_RealData;
  delete[] iv_DataStr;
  if (iv_DataOutStr != NULL) delete[] iv_DataOutStr;

  iv_RealData = NULL;
  iv_DataStr = NULL;
  iv_DataOutStr = NULL;
}

//---------------------------------------------------------------------
//  Public Member Function Specifications
//---------------------------------------------------------------------
int   ecmdDataBuffer::getWordLength() const { return iv_NumWords; }
int   ecmdDataBuffer::getBitLength() const { return iv_NumBits; }

void  ecmdDataBuffer::setWordLength(int newNumWords) {

  uint32_t randNum = 0x12345678;

  if (iv_NumWords < newNumWords) {  /* we need to resize iv_Data member */
    delete[] iv_RealData;
    iv_NumWords = newNumWords;

    iv_RealData = new uint32_t[iv_NumWords + 10]; 
    iv_Data = iv_RealData + 4;
    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */

    /* Ok, now setup the header, and tail */
    iv_RealData[0] = SCANDATA_HEADER;
    iv_RealData[1] = iv_NumWords;
    iv_RealData[3] = randNum;
    iv_RealData[iv_NumWords + 4] = randNum;

  } else { /* no need to resze */
    iv_NumWords = newNumWords;
    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */

    /* Ok, now setup the header, and tail */
    iv_RealData[0] = SCANDATA_HEADER;
    iv_RealData[1] = iv_NumWords;
    iv_RealData[3] = randNum;
    iv_RealData[iv_NumWords + 4] = randNum;
  }

  iv_NumBits = iv_NumWords * 32;  /* that's as accurate as we can get */

  if (!(strlen(this->iv_DataStr) > iv_NumBits)) { /* we need to resize the iv_DataStr member */
    delete[] iv_DataStr;
    iv_DataStr = new char[iv_NumBits+1];
  }
  this->fillDataStr('0'); /* init to 0 */
}  

void  ecmdDataBuffer::setBitLength(int newNumBits) {

  iv_NumWords = (newNumBits - 1) / 32 + 1;
  uint32_t randNum = 0x12345678;

  if (iv_NumBits < newNumBits) {  /* we need to resize iv_Data member */
    delete[] iv_RealData;

    iv_RealData = new uint32_t[iv_NumWords + 10]; 
    iv_Data = iv_RealData + 4;
    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */

    /* Ok, now setup the header, and tail */
    iv_RealData[0] = SCANDATA_HEADER;
    iv_RealData[1] = iv_NumWords;
    iv_RealData[3] = randNum;
    iv_RealData[iv_NumWords + 4] = randNum;

  } else { /* no need to resize */

    memset(iv_Data, 0, iv_NumWords * 4); /* init to 0 */

    /* Ok, now setup the header, and tail */
    iv_RealData[0] = SCANDATA_HEADER;
    iv_RealData[1] = iv_NumWords;
    iv_RealData[3] = randNum;
    iv_RealData[iv_NumWords + 4] = randNum;
  }

  iv_NumBits = newNumBits;

  if (!(strlen(this->iv_DataStr) > iv_NumBits)) { /* we need to resize the iv_DataStr member */
    delete[] iv_DataStr;
    iv_DataStr = new char[iv_NumBits+1];
  }
  this->fillDataStr('0'); /* init to 0 */

}  

void  ecmdDataBuffer::setBit(int bit) {
  if (bit >= iv_NumBits) {
    char temp[50];
    sprintf(temp, "ecmdDataBuffer::setBit: bit %d >= NumBits (%d)\n", bit, iv_NumBits);
    ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
  } else {
    int index = bit/32;
    iv_Data[index] |= 0x00000001 << (31 - (bit-(index * 32)));
    iv_DataStr[bit] = '1';
  }
}

void  ecmdDataBuffer::setBit(int bit, int len) {
  if (bit+len > iv_NumBits) {
    char temp[60];
    sprintf(temp, "ecmdDataBuffer::setBit: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
    ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
    return;
  } else {
    int index = (bit + len) / 32;
    for (int idx = 0; idx < len; idx ++) this->setBit(bit + idx);    
  }
}

void  ecmdDataBuffer::setBit(int bitOffset, const char* binStr) {

  int len = strlen(binStr);
  int i;

  if (bitOffset+len > iv_NumBits) {
    char temp[60];
    sprintf(temp, "ecmdDataBuffer::setBit: bitOffset %d + len %d > NumBits (%d)\n", bitOffset, len, iv_NumBits);
    ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
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
        ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
      }
    }

  }
}

void  ecmdDataBuffer::setWord(int wordOffset, uint32_t value) {

  if (wordOffset >= iv_NumWords) {
    char temp[60];
    sprintf(temp, "ecmdDataBuffer::setWord: wordoffset %d >= NumWords (%d)\n", wordOffset, iv_NumWords);
    ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
  } else {
    iv_Data[wordOffset] = value;
    
    int startBit = wordOffset * 32;
    for (int i = 0; i < 32; i++) {
      if (value & ECMD_MASK[i])
        iv_DataStr[startBit+i] = '1';
      else
        iv_DataStr[startBit+i] = '0';
    }
  }
}

void  ecmdDataBuffer::clearBit(int bit) {
  if (bit >= iv_NumBits) {
    char temp[50];
    sprintf(temp, "ecmdDataBuffer::clearBit: bit %d >= NumBits (%d)\n", bit, iv_NumBits);
    ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
  } else {  
    int index = bit/32;
    iv_Data[index] &= ~(0x00000001 << (31 - (bit-(index * 32))));
    iv_DataStr[bit] = '0';
  }
}

void  ecmdDataBuffer::clearBit(int bit, int len) {
  if (bit+len > iv_NumBits) {
    char temp[60];
    sprintf(temp, "ecmdDataBuffer::clearBit: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
    ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
  } else {
    for (int idx = 0; idx < len; idx ++) this->clearBit(bit + idx);    
  }
}

void  ecmdDataBuffer::flipBit(int bit) {
  if (bit >= iv_NumBits) {
    char temp[50];
    sprintf(temp, "ecmdDataBuffer::flipBit: bit %d >= NumBits (%d)\n", bit, iv_NumBits);
    ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
  } else {
    if (this->isXstate(bit, 1)) {
      char temp[60];
      sprintf(temp, "ecmdDataBuffer::flipBit: cannot flip non-binary data at bit %d\n", bit);
      ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
    } else if (this->isBitSet(bit)) {
      this->clearBit(bit);      
    } else {
      this->setBit(bit);
    }
  }
}

void  ecmdDataBuffer::flipBit(int bit, int len) {
  if (bit+len > iv_NumBits) {
    char temp[50];
    sprintf(temp, "ecmdDataBuffer::flipBit: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
    ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
    return;
  } else {
    char temp[60];
    for (int i = 0; i < len; i++) {
      if (this->isXstate(bit+i, 1)) {
        sprintf(temp, "ecmdDataBuffer::flipBit: cannot flip non-binary data at bit %d\n", bit+i);
        ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
        return;
      } else if (this->isBitSet(bit+i)) {
        this->clearBit(bit+i);
      } else {
        this->setBit(bit+i);
      }
    }
  }
}

int   ecmdDataBuffer::isBitSet(int bit) {
  if (bit >= iv_NumBits) {
    char temp[50];
    sprintf(temp, "ecmdDataBuffer::isBitSet: bit %d >= NumBits (%d)\n", bit, iv_NumBits);
    ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
    return 0;
  } else {
    if (iv_DataStr[bit] != '1' && iv_DataStr[bit] != '0') {
      char temp[70];
      sprintf(temp, "ecmdDataBuffer::isBitSet: non-binary character detected in data at bit %d\n", bit);
      ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
      return 0;
    }
    int index = bit/32;
    return (iv_Data[index] & 0x00000001 << (31 - (bit-(index * 32)))); 
  }
}

int   ecmdDataBuffer::isBitSet(int bit, int len) {
  if (bit+len > iv_NumBits) {
    char temp[50];
    sprintf(temp, "ecmdDataBuffer::isBitSet: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
    ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
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
    ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
    return 0;
  } else {
    if (iv_DataStr[bit] != '1' && iv_DataStr[bit] != '0') {
      ecmdRegisterErrorMsg(ECMD_FAILURE, "ecmdDataBuffer::isBitClear: non-binary character detected in data string\n");
      return 0;
    }
    int index = bit/32;
    return (!(iv_Data[index] & 0x00000001 << (31 - (bit-(index * 32))))); 
  }
}

int   ecmdDataBuffer::isBitClear(int bit, int len)
{
  if (bit+len > iv_NumBits) {
    char temp[50];
    sprintf(temp, "ecmdDataBuffer::isBitClear: bit %d + len %d > NumBits (%d)\n", bit, len, iv_NumBits);
    ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
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
    ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
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

  // shift char
  char* temp = new char[iv_NumBits+1];
  for (i = 0; i < iv_NumBits; i++) temp[i] = '0'; // backfill with zeros
  temp[iv_NumBits] = '\0';
  strncpy(&temp[shiftNum], iv_DataStr, iv_NumBits-shiftNum);  
  strcpy(iv_DataStr, temp); // copy back into iv_DataStr
  
  delete[] temp;
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

  // shift char
  char* temp = new char[iv_NumBits+1];
  for (i = 0; i < iv_NumBits; i++) temp[i] = '0'; // backfill with zeros
  temp[iv_NumBits] = '\0';
  strncpy(temp, &iv_DataStr[shiftNum], iv_NumBits-shiftNum);  
  strcpy(iv_DataStr, temp); // copy back into iv_DataStr
  
  delete[] temp;
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
  this->fillDataStr('0');
}

void  ecmdDataBuffer::flushTo1() {
  for (int i = 0; i < iv_NumWords; i++) iv_Data[i] = 0xFFFFFFFF;    
  this->fillDataStr('1');
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
    ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
  } else {
    
    uint32_t thisMask;
    uint32_t thisWord;
    for (int i = 0; i < len; i++) {
      thisMask = ECMD_MASK[i%32];
      thisWord = dataIn[i/32];
      if (dataIn[i/32] & ECMD_MASK[i%32])
        this->setBit(start+i);
      else 
        this->clearBit(start+i);
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
    ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
  } else {

    myextract(this->iv_Data, start, len, dataOut);

    if (this->isXstate()) {  /* fast strchr */
      for (int i = start; i < len; i++) { /* now get exact bit */
        if (this->isXstate(start, 1)) {
          char temp[80];
          sprintf(temp, "ecmdDataBuffer::extract: Cannot extract non-binary character at bit %d\n", i);
          ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
        }
      }
    }
  }
}

void ecmdDataBuffer::setOr(ecmdDataBuffer& bufferIn, int startBit, int len) {
  this->setOr(bufferIn.iv_Data, startBit, len);
}

void ecmdDataBuffer::setOr(uint32_t * dataIn, int startBit, int len) {

  if (startBit + len > iv_NumBits) {
    char temp[50];
    sprintf(temp, "ecmdDataBuffer::setOr: bit %d + len %d > NumBits (%d)\n", startBit, len, iv_NumBits);
    ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
  } else {

    for (int i = 0; i < len; i++) {
      if (dataIn[i/32] & ECMD_MASK[i % 32]) this->setBit(startBit + i);
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
    ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
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
    ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
  } else {
    for (int i = 0; i < len; i++) {
      if (!(dataIn[i/32] & ECMD_MASK[i % 32])) this->clearBit(startBit + i);
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

  charOffset = start / 32;
  posOffset = start - charOffset * 32;

  for (counter = 0; counter < (stop - start + 1); counter++) {
    if (ECMD_MASK[posOffset] & iv_Data[charOffset]) {
      parity ^= 1;
    }
    posOffset++;
    if (posOffset > 31) {
      charOffset++;
      posOffset = 0;
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
    ecmdRegisterErrorMsg(ECMD_FAILURE, temp);
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

  // iv_DataStr should already have updated iv_Data
  strncpy(iv_DataOutStr, &iv_DataStr[start], bitLen);
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
  strcpy(newCopy.iv_DataStr, iv_DataStr);

}

void  ecmdDataBuffer::memCopyIn(uint32_t* buf, int bytes) { /* Does a memcpy from supplied buffer into ecmdDataBuffer */
  mybigEndianMemcpy(iv_Data, buf, bytes);
}
void  ecmdDataBuffer::memCopyOut(uint32_t* buf, int bytes) { /* Does a memcpy from ecmdDataBuffer into supplied buffer */
  mybigEndianMemcpy(buf, iv_Data, bytes);
}

int   ecmdDataBuffer::isXstate() {  /* check for only X's */
  return (strchr(iv_DataStr, 'x') || strchr(iv_DataStr, 'X'));
}

// actually use this for ANY non-binary char, not just X's
int   ecmdDataBuffer::isXstate(int start, int length) {

  int stopBit = start + length;
  int minStop = iv_NumBits < stopBit ? iv_NumBits : stopBit; /* pick the smallest */

  for (int i = start; i < minStop; i++) {
    if (iv_DataStr[i] != '0' && iv_DataStr[i] != '1')
      return 1;
  }
  return 0;
}


//---------------------------------------------------------------------
//  Private Member Function Specifications
//---------------------------------------------------------------------
void ecmdDataBuffer::fillDataStr(char fillChar) {

  for (int i = 0; i < iv_NumBits; i++) iv_DataStr[i] = fillChar;
  iv_DataStr[iv_NumBits] = '\0';  
  
}

void myextract(uint32_t *scr_ptr, uint32_t start_bit_num, uint32_t num_bits_to_extract, uint32_t *out_iv_Data_ptr)
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

void * mybigEndianMemcpy(void * dest, const void *src, size_t count)
{
#ifdef LINUX
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
