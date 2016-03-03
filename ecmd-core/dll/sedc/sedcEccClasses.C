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


// Module Description **************************************************
//
// Description: 
//
// End Module Description **********************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define sedcEccClasses_C
#include <sedcEccClasses.H>

#undef sedcEccClasses_C

/*******************************************************************************************/
/* sedcEplatchesLine -+- sedcEplatchesLine -+- sedcEplatchesLine -+- sedcEplatchesLine */
/* sedcEplatchesLine -+- sedcEplatchesLine -+- sedcEplatchesLine -+- sedcEplatchesLine */
/*******************************************************************************************/
//----------------------------------------------------------------------
//  Constructors
//----------------------------------------------------------------------
// Moved inline in header

//----------------------------------------------------------------------
//  Destructor
//----------------------------------------------------------------------
// Moved inline in header

sedcEplatchesLine::sedcEplatchesLine(const sedcEplatchesLine &rhs) {

  state = rhs.state;
  comment = rhs.comment;

}

int sedcEplatchesLine::operator=(const sedcEplatchesLine &rhs) {

  state = rhs.state;
  comment = rhs.comment;

  return 0;
}

void sedcEplatchesLine::reset() {
  state = 0x0;
  comment = "";
}


/***********************************************************************************************/
/* sedcEplatchesEntry -+- sedcEplatchesEntry -+- sedcEplatchesEntry -+- sedcEplatchesEntry */
/* sedcEplatchesEntry -+- sedcEplatchesEntry -+- sedcEplatchesEntry -+- sedcEplatchesEntry */
/***********************************************************************************************/
//----------------------------------------------------------------------
//  Constructors
//----------------------------------------------------------------------
// Moved inline in header

//----------------------------------------------------------------------
//  Destructor
//----------------------------------------------------------------------
// Moved inline in header

sedcEplatchesEntry::sedcEplatchesEntry(const sedcEplatchesEntry &rhs) {

  valid = rhs.valid;
  name = rhs.name;
  function = rhs.function;
  inSpy = rhs.inSpy;
  outSpy= rhs.outSpy;
  eplatchesLines = rhs.eplatchesLines;
}

int sedcEplatchesEntry::operator=(const sedcEplatchesEntry &rhs) {

  valid = rhs.valid;
  name = rhs.name;
  function = rhs.function;
  inSpy = rhs.inSpy;
  outSpy = rhs.outSpy;
  eplatchesLines = rhs.eplatchesLines;

  return 0;
}

void sedcEplatchesEntry::reset() {
  valid = false;
  name = "";
  function = "";
}


/*******************************************************************************/
/* sedcEccfuncLine -+- sedcEccfuncLine -+- sedcEccfuncLine -+- sedcEccfuncLine */
/* sedcEccfuncLine -+- sedcEccfuncLine -+- sedcEccfuncLine -+- sedcEccfuncLine */
/*******************************************************************************/
//----------------------------------------------------------------------
//  Constructors
//----------------------------------------------------------------------
// Moved inline in header

//----------------------------------------------------------------------
//  Destructor
//----------------------------------------------------------------------
// Moved inline in header

sedcEccfuncLine::sedcEccfuncLine(const sedcEccfuncLine &rhs) {

  state = rhs.state;
  comment = rhs.comment;
  parityType = rhs.parityType;
  tableValue = rhs.tableValue;
  tableLength = rhs.tableLength;
}

int sedcEccfuncLine::operator=(const sedcEccfuncLine &rhs) {

  state = rhs.state;
  comment = rhs.comment;
  parityType = rhs.parityType;
  tableValue = rhs.tableValue;
  tableLength = rhs.tableLength;

  return 0;
}

void sedcEccfuncLine::reset() {
  state = 0x0;
  comment = "";
  parityType = "";
  tableValue.clear();
  tableLength = 0;
}


/***********************************************************************************/
/* sedcEccfuncEntry -+- sedcEccfuncEntry -+- sedcEccfuncEntry -+- sedcEccfuncEntry */
/* sedcEccfuncEntry -+- sedcEccfuncEntry -+- sedcEccfuncEntry -+- sedcEccfuncEntry */
/***********************************************************************************/
//----------------------------------------------------------------------
//  Constructors
//----------------------------------------------------------------------
// Moved inline in header

//----------------------------------------------------------------------
//  Destructor
//----------------------------------------------------------------------
// Moved inline in header

sedcEccfuncEntry::sedcEccfuncEntry(const sedcEccfuncEntry &rhs) {
  valid = rhs.valid;
  name = rhs.name;
  inBits = rhs.inBits;
  outBits = rhs.outBits;
  eccfuncLines = rhs.eccfuncLines;
}

int sedcEccfuncEntry::operator=(const sedcEccfuncEntry &rhs) {
  valid = rhs.valid;
  name = rhs.name;
  inBits = rhs.inBits;
  outBits = rhs.outBits;
  eccfuncLines = rhs.eccfuncLines;

  return 0;
}
