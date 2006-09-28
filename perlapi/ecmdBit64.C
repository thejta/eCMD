// Copyright ***********************************************************
//                                                                      
// File ecmdBit64.C                                   
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
//   
// End Change Log *****************************************************


//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------

#include <ecmdDefines.H>
#include <ecmdBit64.H>
#include <inttypes.h>

uint64_t getHexValue(const char* hexValue);

ecmdBit64::ecmdBit64() : value(0) {
}

ecmdBit64::ecmdBit64(const char* hexValue) : value(0) {
  value = getHexValue(hexValue);
}
ecmdBit64::ecmdBit64(uint32_t newValue) : value(newValue) {
}
ecmdBit64::ecmdBit64(uint32_t hiValue, uint32_t loValue) {
  value = hiValue;
  value = (value << 32) | (uint64_t)loValue;
}

void ecmdBit64::setValue(const char* hexValue) {
  value = getHexValue(hexValue);
}
void ecmdBit64::setValue(uint32_t newValue) {
  value = (uint64_t)newValue;
}
void ecmdBit64::setValue(uint32_t hiValue, uint32_t loValue) {
  value = hiValue;
  value = (value << 32) | (uint64_t)loValue;
}

std::string ecmdBit64::getValue(bool i_Oxprefix) {
  char tmp[100];
  
  if (i_Oxprefix)
    sprintf(tmp,"0x%llX",value);
  else
    sprintf(tmp,"%llX",value);
  return (tmp);
}
void ecmdBit64::getValue(uint32_t & hiValue, uint32_t & loValue) {
  hiValue = (uint32_t)(value >> 32);
  loValue = (uint32_t)value;
}

int ecmdBit64::operator==(const ecmdBit64 & i_other) const {
  return (value == i_other.value);
}
int ecmdBit64::operator==(uint32_t i_other) const {
  return (value == (uint64_t)i_other);
}
int ecmdBit64::operator!=(const ecmdBit64 & i_other) const {
  return (value != i_other.value);
}
int ecmdBit64::operator!=(uint32_t i_other) const {
  return (value != (uint64_t)i_other);
}

ecmdBit64& ecmdBit64::operator=(const ecmdBit64 & i_master) {
  value = i_master.value;
  return *this;
}

ecmdBit64 ecmdBit64::operator + (const ecmdBit64 & i_other) const {
  ecmdBit64 newItem = *this;
  newItem.value += i_other.value;
  return newItem;
}
ecmdBit64 ecmdBit64::operator + (uint32_t i_other) const {
  ecmdBit64 newItem = *this;
  newItem.value += i_other;
  return newItem;
}

ecmdBit64 ecmdBit64::operator - (const ecmdBit64 & i_other) const {
  ecmdBit64 newItem = *this;
  newItem.value -= i_other.value;
  return newItem;
}
ecmdBit64 ecmdBit64::operator - (uint32_t i_other) const {
  ecmdBit64 newItem = *this;
  newItem.value -= i_other;
  return newItem;
}

ecmdBit64 ecmdBit64::operator * (const ecmdBit64 & i_other) const {
  ecmdBit64 newItem = *this;
  newItem.value *= i_other.value;
  return newItem;
}
ecmdBit64 ecmdBit64::operator * (uint32_t i_other) const {
  ecmdBit64 newItem = *this;
  newItem.value *= i_other;
  return newItem;
}

ecmdBit64 ecmdBit64::operator / (const ecmdBit64 & i_other) const {
  ecmdBit64 newItem = *this;
  newItem.value /= i_other.value;
  return newItem;
}
ecmdBit64 ecmdBit64::operator / (uint32_t i_other) const {
  ecmdBit64 newItem = *this;
  newItem.value /= i_other;
  return newItem;
}

ecmdBit64 ecmdBit64::operator % (const ecmdBit64 & i_other) const {
  ecmdBit64 newItem = *this;
  newItem.value %= i_other.value;
  return newItem;
}
ecmdBit64 ecmdBit64::operator % (uint32_t i_other) const {
  ecmdBit64 newItem = *this;
  newItem.value %= i_other;
  return newItem;
}

ecmdBit64 ecmdBit64::operator ! () const {
  ecmdBit64 newItem = *this;
  newItem.value = !value;
  return newItem;
}

ecmdBit64 ecmdBit64::operator ~ () const {
  ecmdBit64 newItem = *this;
  newItem.value = ~value;
  return newItem;
}

ecmdBit64 ecmdBit64::operator << (int shift) const {
  ecmdBit64 newItem = *this;
  newItem.value = value << shift;
  return newItem;
}

ecmdBit64 ecmdBit64::operator >> (int shift) const {
  ecmdBit64 newItem = *this;
  newItem.value = value >> shift;
  return newItem;
}

int ecmdBit64::operator < (const ecmdBit64 & i_other) const {
  return (value < i_other.value);
}
int ecmdBit64::operator < (uint32_t i_other) const {
  return (value < (uint64_t)i_other);
}

int ecmdBit64::operator <= (const ecmdBit64 & i_other) const {
  return (value <= i_other.value);
}
int ecmdBit64::operator <= (uint32_t i_other) const {
  return (value <= (uint64_t)i_other);
}

int ecmdBit64::operator > (const ecmdBit64 & i_other) const {
  return (value > i_other.value);
}
int ecmdBit64::operator > (uint32_t i_other) const {
  return (value > (uint64_t)i_other);
}

int ecmdBit64::operator >= (const ecmdBit64 & i_other) const {
  return (value >= i_other.value);
}
int ecmdBit64::operator >= (uint32_t i_other) const {
  return (value >= (uint64_t)i_other);
}

ecmdBit64 ecmdBit64::operator | (const ecmdBit64 & i_other) const {
  ecmdBit64 newItem = *this;
  newItem.value |= i_other.value;
  return newItem;
}

ecmdBit64 ecmdBit64::operator ^ (const ecmdBit64 & i_other) const {
  ecmdBit64 newItem = *this;
  newItem.value ^= i_other.value;
  return newItem;
}

ecmdBit64 ecmdBit64::operator & (const ecmdBit64 & i_other) const {
  ecmdBit64 newItem = *this;
  newItem.value &= i_other.value;
  return newItem;
}

void ecmdBit64::operator ++ (int num) {
  value++;
}
void ecmdBit64::operator -- (int num) {
  value--;
}


uint64_t getHexValue(const char* hexValue) {
  uint64_t ret = 0;
  if (strlen(hexValue) > 2 &&
      hexValue[0] == '0' &&
      hexValue[1] == 'x') 
    sscanf(hexValue,"0x%llx",&ret);
  else
    sscanf(hexValue,"%llx",&ret);
  return ret;
}
  
