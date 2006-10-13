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
#include <stdlib.h>

#include <ecmdDefines.H>
#include <ecmdBit64.H>
#include <inttypes.h>

/* Pass in either dec '1234' or hex '0xFEED' */
uint64_t genHexValue(const char* strValue);

ecmdBit64::ecmdBit64() : iv_value(0) {
}

ecmdBit64::ecmdBit64(const char* strValue) : iv_value(0) {
  iv_value = genHexValue(strValue);
}
ecmdBit64::ecmdBit64(const ecmdBit64 & newValue) {
  iv_value = newValue.iv_value;
}
ecmdBit64::ecmdBit64(uint32_t newValue) : iv_value(newValue) {
}
ecmdBit64::ecmdBit64(uint32_t hiValue, uint32_t loValue) {
  iv_value = hiValue;
  iv_value = (iv_value << 32) | (uint64_t)loValue;
}

void ecmdBit64::setValue(const char* strValue) {
  iv_value = genHexValue(strValue);
}
void ecmdBit64::setValue(const ecmdBit64 & newValue) {
  iv_value = newValue.iv_value;
}
void ecmdBit64::setValue(uint32_t newValue) {
  iv_value = (uint64_t)newValue;
}
void ecmdBit64::setValue(uint32_t hiValue, uint32_t loValue) {
  iv_value = hiValue;
  iv_value = (iv_value << 32) | (uint64_t)loValue;
}

std::string ecmdBit64::getHexValue(uint32_t i_alignLen) {
  char tmp[100];
  sprintf(tmp,"0x%0*llX",i_alignLen, iv_value);
  return (tmp);
}
std::string ecmdBit64::getDecimalValue() {
  char tmp[100];
  sprintf(tmp,"%llu",iv_value);
  return (tmp);
}
void ecmdBit64::getValue(uint32_t & hiValue, uint32_t & loValue) {
  hiValue = (uint32_t)(iv_value >> 32);
  loValue = (uint32_t)iv_value;
}

int ecmdBit64::operator==(const ecmdBit64 & i_other) const {
  return (iv_value == i_other.iv_value);
}
int ecmdBit64::operator==(uint32_t i_other) const {
  return (iv_value == (uint64_t)i_other);
}
int ecmdBit64::operator!=(const ecmdBit64 & i_other) const {
  return (iv_value != i_other.iv_value);
}
int ecmdBit64::operator!=(uint32_t i_other) const {
  return (iv_value != (uint64_t)i_other);
}

ecmdBit64& ecmdBit64::operator=(const ecmdBit64 & i_master) {
  iv_value = i_master.iv_value;
  return *this;
}

ecmdBit64 ecmdBit64::operator + (const ecmdBit64 & i_other) const {
  ecmdBit64 newItem = *this;
  newItem.iv_value += i_other.iv_value;
  return newItem;
}
ecmdBit64 ecmdBit64::operator + (uint32_t i_other) const {
  ecmdBit64 newItem = *this;
  newItem.iv_value += i_other;
  return newItem;
}

ecmdBit64 ecmdBit64::operator - (const ecmdBit64 & i_other) const {
  ecmdBit64 newItem = *this;
  newItem.iv_value -= i_other.iv_value;
  return newItem;
}
ecmdBit64 ecmdBit64::operator - (uint32_t i_other) const {
  ecmdBit64 newItem = *this;
  newItem.iv_value -= i_other;
  return newItem;
}

ecmdBit64 ecmdBit64::operator * (const ecmdBit64 & i_other) const {
  ecmdBit64 newItem = *this;
  newItem.iv_value *= i_other.iv_value;
  return newItem;
}
ecmdBit64 ecmdBit64::operator * (uint32_t i_other) const {
  ecmdBit64 newItem = *this;
  newItem.iv_value *= i_other;
  return newItem;
}

ecmdBit64 ecmdBit64::operator / (const ecmdBit64 & i_other) const {
  ecmdBit64 newItem = *this;
  newItem.iv_value /= i_other.iv_value;
  return newItem;
}
ecmdBit64 ecmdBit64::operator / (uint32_t i_other) const {
  ecmdBit64 newItem = *this;
  newItem.iv_value /= i_other;
  return newItem;
}

ecmdBit64 ecmdBit64::operator % (const ecmdBit64 & i_other) const {
  ecmdBit64 newItem = *this;
  newItem.iv_value %= i_other.iv_value;
  return newItem;
}
ecmdBit64 ecmdBit64::operator % (uint32_t i_other) const {
  ecmdBit64 newItem = *this;
  newItem.iv_value %= i_other;
  return newItem;
}

ecmdBit64 ecmdBit64::operator ! () const {
  ecmdBit64 newItem = *this;
  newItem.iv_value = !iv_value;
  return newItem;
}

ecmdBit64 ecmdBit64::operator ~ () const {
  ecmdBit64 newItem = *this;
  newItem.iv_value = ~iv_value;
  return newItem;
}

ecmdBit64 ecmdBit64::operator << (int shift) const {
  ecmdBit64 newItem = *this;
  newItem.iv_value = iv_value << shift;
  return newItem;
}

ecmdBit64 ecmdBit64::operator >> (int shift) const {
  ecmdBit64 newItem = *this;
  newItem.iv_value = iv_value >> shift;
  return newItem;
}

int ecmdBit64::operator < (const ecmdBit64 & i_other) const {
  return (iv_value < i_other.iv_value);
}
int ecmdBit64::operator < (uint32_t i_other) const {
  return (iv_value < (uint64_t)i_other);
}

int ecmdBit64::operator <= (const ecmdBit64 & i_other) const {
  return (iv_value <= i_other.iv_value);
}
int ecmdBit64::operator <= (uint32_t i_other) const {
  return (iv_value <= (uint64_t)i_other);
}

int ecmdBit64::operator > (const ecmdBit64 & i_other) const {
  return (iv_value > i_other.iv_value);
}
int ecmdBit64::operator > (uint32_t i_other) const {
  return (iv_value > (uint64_t)i_other);
}

int ecmdBit64::operator >= (const ecmdBit64 & i_other) const {
  return (iv_value >= i_other.iv_value);
}
int ecmdBit64::operator >= (uint32_t i_other) const {
  return (iv_value >= (uint64_t)i_other);
}

ecmdBit64 ecmdBit64::operator | (const ecmdBit64 & i_other) const {
  ecmdBit64 newItem = *this;
  newItem.iv_value |= i_other.iv_value;
  return newItem;
}

ecmdBit64 ecmdBit64::operator | (const uint32_t i_rhs) const {
  ecmdBit64 newItem = *this;
  newItem.iv_value |= (uint64_t)i_rhs;
  return newItem;
}

ecmdBit64 ecmdBit64::operator ^ (const ecmdBit64 & i_other) const {
  ecmdBit64 newItem = *this;
  newItem.iv_value ^= i_other.iv_value;
  return newItem;
}

ecmdBit64 ecmdBit64::operator & (const ecmdBit64 & i_other) const {
  ecmdBit64 newItem = *this;
  newItem.iv_value &= i_other.iv_value;
  return newItem;
}

void ecmdBit64::operator ++ (int num) {
  iv_value++;
}
void ecmdBit64::operator -- (int num) {
  iv_value--;
}


uint64_t genHexValue(const char* strValue) {
  return strtoull(strValue,0,0);
}
  
