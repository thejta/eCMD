//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2003,2016 IBM International Business Machines Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
//IBM_PROLOG_END_TAG



//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <iomanip>

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
  std::ostringstream tmp;
  tmp << "0x" << std::hex << std::setw(i_alignLen) << std::setfill('0') << std::uppercase << iv_value;
  return tmp.str();
}
std::string ecmdBit64::getDecimalValue() {
  std::ostringstream tmp;
  tmp << std::dec << iv_value;
  return tmp.str();
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
  
