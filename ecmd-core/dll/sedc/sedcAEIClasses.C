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
#define sedcAEIClasses_C
#include <sedcAEIClasses.H>
#include <ecmdSharedUtils.H>
#include <stdio.h>
#undef sedcAEIClasses_C


/*******************************************************************************/
/* sedcAEIEnum -+- sedcAEIEnum -+- sedcAEIEnum -+- sedcAEIEnum -+- sedcAEIEnum */
/* sedcAEIEnum -+- sedcAEIEnum -+- sedcAEIEnum -+- sedcAEIEnum -+- sedcAEIEnum */
/*******************************************************************************/
//----------------------------------------------------------------------
//  Constructors
//----------------------------------------------------------------------
// Moved inline in header

//----------------------------------------------------------------------
//  Destructor
//----------------------------------------------------------------------
// Moved inline in header

sedcAEIEnum::sedcAEIEnum(const sedcAEIEnum &rhs) {

  enumName = rhs.enumName;
  enumValue = rhs.enumValue;
  enumLength = rhs.enumLength;
}

int sedcAEIEnum::operator=(const sedcAEIEnum &rhs) {

  enumName = rhs.enumName;
  enumValue = rhs.enumValue;
  enumLength = rhs.enumLength;

  return 0;
}

/************************************************************************************/
/* sedcAEIEntry -+- sedcAEIEntry -+- sedcAEIEntry -+- sedcAEIEntry -+- sedcAEIEntry */
/* sedcAEIEntry -+- sedcAEIEntry -+- sedcAEIEntry -+- sedcAEIEntry -+- sedcAEIEntry */
/************************************************************************************/
//----------------------------------------------------------------------
//  Constructors
//----------------------------------------------------------------------
// Moved inline in header

//----------------------------------------------------------------------
//  Destructor
//----------------------------------------------------------------------
// Moved inline in header

sedcAEIEntry::sedcAEIEntry(const sedcAEIEntry &rhs) {

  valid = rhs.valid;
  name = rhs.name;
  states = rhs.states;
  length = rhs.length;
  aeiLines = rhs.aeiLines;
  aeiEnums = rhs.aeiEnums;
  aeiEpcheckers = rhs.aeiEpcheckers;
  aeiLatches = rhs.aeiLatches;

}

int sedcAEIEntry::operator=(const sedcAEIEntry &rhs) {

  valid = rhs.valid;
  name = rhs.name;
  states = rhs.states;
  length = rhs.length;
  aeiLines = rhs.aeiLines;
  aeiEnums = rhs.aeiEnums;
  aeiEpcheckers = rhs.aeiEpcheckers;
  aeiLatches = rhs.aeiLatches;

  return 0;
}

int sedcAEIEntry::operator==(const sedcAEIEntry &rhs) const {

  return name==rhs.name;
}
