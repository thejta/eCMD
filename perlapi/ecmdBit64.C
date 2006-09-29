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
uint64_t getHexValue(const char* strValue);

ecmdBit64::ecmdBit64() : value(0) {
}

ecmdBit64::ecmdBit64(const char* strValue) : value(0) {
  value = getHexValue(strValue);
}
ecmdBit64::ecmdBit64(const ecmdBit64 & newValue) {
  value = newValue.value;
}
ecmdBit64::ecmdBit64(uint32_t newValue) : value(newValue) {
}
ecmdBit64::ecmdBit64(uint32_t hiValue, uint32_t loValue) {
  value = hiValue;
  value = (value << 32) | (uint64_t)loValue;
}

void ecmdBit64::setValue(const char* strValue) {
  value = getHexValue(strValue);
}
void ecmdBit64::setValue(const ecmdBit64 & newValue) {
  value = newValue.value;
}
void ecmdBit64::setValue(uint32_t newValue) {
  value = (uint64_t)newValue;
}
void ecmdBit64::setValue(uint32_t hiValue, uint32_t loValue) {
  value = hiValue;
  value = (value << 32) | (uint64_t)loValue;
}

std::string ecmdBit64::getValue(bool i_decimal) {
  char tmp[100];
  if (!i_decimal)
    sprintf(tmp,"0x%llX",value);
  else
    sprintf(tmp,"%llu",value);
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


uint64_t getHexValue(const char* strValue) {
  return strtoull(strValue,0,0);
}
  
