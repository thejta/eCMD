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
#include <sedcCommonClasses.H>
#include <ecmdSharedUtils.H>
#include <stdio.h>


/*****************************************************************************************/
/* sedcLatchLine -+- sedcLatchLine -+- sedcLatchLine -+- sedcLatchLine -+- sedcLatchLine */
/* sedcLatchLine -+- sedcLatchLine -+- sedcLatchLine -+- sedcLatchLine -+- sedcLatchLine */
/*****************************************************************************************/
//----------------------------------------------------------------------
//  Constructors
//----------------------------------------------------------------------
// Moved inline in header

//----------------------------------------------------------------------
//  Destructor
//----------------------------------------------------------------------
// Moved inline in header

sedcLatchLine::sedcLatchLine(const sedcLatchLine &rhs) {

  state = rhs.state;
  length = rhs.length;
  offsetJTAG = rhs.offsetJTAG;
  offsetFSI = rhs.offsetFSI;
  lhsNum = rhs.lhsNum;
  rhsNum = rhs.rhsNum;
  direction = rhs.direction;
  latchName = rhs.latchName;
  latchExtras = rhs.latchExtras;
  comment = rhs.comment;
  hashKey = rhs.hashKey;
  holderID = rhs.holderID;

}

sedcLatchLine& sedcLatchLine::operator=(const sedcLatchLine &rhs) {

  state = rhs.state;
  length = rhs.length;
  offsetJTAG = rhs.offsetJTAG;
  offsetFSI = rhs.offsetFSI;
  lhsNum = rhs.lhsNum;
  rhsNum = rhs.rhsNum;
  direction = rhs.direction;
  latchName = rhs.latchName;
  latchExtras = rhs.latchExtras;
  comment = rhs.comment;
  hashKey = rhs.hashKey;
  holderID = rhs.holderID;

  return *this;
}


int sedcLatchLine::operator<(const sedcLatchLine &rhs) const {

  return lhsNum<rhs.lhsNum;
}

void sedcLatchLine::reset() {
  state = 0x0;
  length = -1;
  offsetJTAG = -1;
  offsetFSI = -1;
  lhsNum = -1;
  rhsNum = -1;
  direction = -1;
  latchName = "";
  latchExtras = "";
  comment = "";
  hashKey = 0x0;
  holderID = 99;

}

void sedcLatchLine::setName(std::string newLatchName) {
  latchName = newLatchName;
  hashKey = ecmdHashString64(latchName.c_str(), 0);
}

/************************************************************************************/
/* sedcFileLine -+- sedcFileLine -+- sedcFileLine -+- sedcFileLine -+- sedcFileLine */
/* sedcFileLine -+- sedcFileLine -+- sedcFileLine -+- sedcFileLine -+- sedcFileLine */
/************************************************************************************/
//----------------------------------------------------------------------
//  Constructors
//----------------------------------------------------------------------
// Moved inline in header

//----------------------------------------------------------------------
//  Destructor
//----------------------------------------------------------------------
// Moved inline in header

sedcFileLine::sedcFileLine(const sedcFileLine &rhs) {

  realLine = rhs.realLine;
  tokens = rhs.tokens;
  comment = rhs.comment;

}

sedcFileLine& sedcFileLine::operator=(const sedcFileLine &rhs) {

  realLine = rhs.realLine;
  tokens = rhs.tokens;
  comment = rhs.comment;

  return *this;
}

void sedcFileLine::reset() {
  realLine = "";
  tokens.clear();
  comment = "";
}
