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
#include <sedcScomdefClasses.H>
#include <stdio.h>


/***********************************************************************************/
/* sedcScomdefLatch -+- sedcScomdefLatch -+- sedcScomdefLatch -+- sedcScomdefLatch */
/* sedcScomdefLatch -+- sedcScomdefLatch -+- sedcScomdefLatch -+- sedcScomdefLatch */
/***********************************************************************************/
//----------------------------------------------------------------------
//  Constructors
//----------------------------------------------------------------------
// Moved inline in header

//----------------------------------------------------------------------
//  Destructor
//----------------------------------------------------------------------
// Moved inline in header

sedcScomdefLatch::sedcScomdefLatch(const sedcScomdefLatch &rhs) {

  address = rhs.address;
  sdLine = rhs.sdLine;

}

sedcScomdefLatch& sedcScomdefLatch::operator=(const sedcScomdefLatch &rhs) {

  address = rhs.address;
  sdLine = rhs.sdLine;

  return *this;
}

int sedcScomdefLatch::operator<(const sedcScomdefLatch &rhs) const {
  return (sdLine.hashKey < rhs.sdLine.hashKey);
}

void sedcScomdefLatch::reset() {
  address = 0x0;
  sdLine.reset();
}

/*******************************************************************************************/
/* sedcScomdefDefLine -+- sedcScomdefDefLine -+- sedcScomdefDefLine -+- sedcScomdefDefLine */
/* sedcScomdefDefLine -+- sedcScomdefDefLine -+- sedcScomdefDefLine -+- sedcScomdefDefLine */
/*******************************************************************************************/
//----------------------------------------------------------------------
//  Constructors
//----------------------------------------------------------------------
// Moved inline in header

//----------------------------------------------------------------------
//  Destructor
//----------------------------------------------------------------------
// Moved inline in header

sedcScomdefDefLine::sedcScomdefDefLine(const sedcScomdefDefLine &rhs) {

  lhsNum = rhs.lhsNum;
  rhsNum = rhs.rhsNum;
  length = rhs.length;
  dialName = rhs.dialName;
  detail = rhs.detail;

}

sedcScomdefDefLine& sedcScomdefDefLine::operator=(const sedcScomdefDefLine &rhs) {

  lhsNum = rhs.lhsNum;
  rhsNum = rhs.rhsNum;
  length = rhs.length;
  dialName = rhs.dialName;
  detail = rhs.detail;

  return *this;
}

void sedcScomdefDefLine::reset() {
  lhsNum = -1;
  rhsNum = -1;
  length = -1;
  dialName = "";
  detail.clear();
}

/********************************************************************************************************/
/* sedcScomdefEntry -+- sedcScomdefEntry -+- sedcScomdefEntry -+- sedcScomdefEntry -+- sedcScomdefEntry */
/* sedcScomdefEntry -+- sedcScomdefEntry -+- sedcScomdefEntry -+- sedcScomdefEntry -+- sedcScomdefEntry */
/********************************************************************************************************/
//----------------------------------------------------------------------
//  Constructors
//----------------------------------------------------------------------
// Moved inline in header

//----------------------------------------------------------------------
//  Destructor
//----------------------------------------------------------------------
// Moved inline in header

sedcScomdefEntry::sedcScomdefEntry(const sedcScomdefEntry &rhs) {

  valid = rhs.valid;
  states = rhs.states;
  addresses = rhs.addresses;
  mask = rhs.mask;
  name = rhs.name;
  description = rhs.description;
  clkdomain = rhs.clkdomain;
  clkstate = rhs.clkstate;
  definition = rhs.definition;
  latches = rhs.latches;

}

sedcScomdefEntry& sedcScomdefEntry::operator=(const sedcScomdefEntry &rhs) {

  valid = rhs.valid;
  states = rhs.states;
  addresses = rhs.addresses;
  mask = rhs.mask;
  name = rhs.name;
  description = rhs.description;
  clkdomain = rhs.clkdomain;
  clkstate = rhs.clkstate;
  definition = rhs.definition;
  latches = rhs.latches;

  return *this;
}

int sedcScomdefEntry::operator==(const sedcScomdefEntry &rhs) const {

  return (addresses[0] == rhs.addresses[0]);
}

int sedcScomdefEntry::operator<(const sedcScomdefEntry &rhs) const {
  return (addresses[0] < rhs.addresses[0]);
}

void sedcScomdefEntry::reset()
{
   valid = true; 
   states = 0;
   addresses.clear();
   name = "";
   description.clear();
   clkdomain = "";
   clkstate = SEDC_CLK_RUNNING;
   definition.clear();
   latches.clear();
}

/***************************************************************************************************/
/* sedcScomdefContainer -+- sedcScomdefContainer -+- sedcScomdefContainer -+- sedcScomdefContainer */
/* sedcScomdefContainer -+- sedcScomdefContainer -+- sedcScomdefContainer -+- sedcScomdefContainer */
/***************************************************************************************************/
//----------------------------------------------------------------------
//  Constructors
//----------------------------------------------------------------------
// Moved inline in header

//----------------------------------------------------------------------
//  Destructor
//----------------------------------------------------------------------
// Moved inline in header

sedcScomdefContainer::sedcScomdefContainer(const sedcScomdefContainer &rhs) {

  scomdefEntries = rhs.scomdefEntries;
  scomdefLatches = rhs.scomdefLatches;

}

sedcScomdefContainer& sedcScomdefContainer::operator=(const sedcScomdefContainer &rhs) {

  scomdefEntries = rhs.scomdefEntries;
  scomdefLatches = rhs.scomdefLatches;

  return *this;
}

void sedcScomdefContainer::insertLatches(uint64_t address, std::vector<sedcLatchLine> &latches) {

  unsigned int x;
  sedcScomdefLatch myScomdefLatch;

  /* Loop through the latches read in from the entry, associate an address and store away in the big list */
  for (x = 0; x < latches.size(); x++) {
    myScomdefLatch.address = address;
    myScomdefLatch.sdLine = latches[x];
    scomdefLatches.push_back(myScomdefLatch);
  }
  
}


void sedcScomdefContainer::scomdefDumper() {
  std::list<std::string>::iterator strIter;
  std::vector<sedcLatchLine>::iterator latchIter;
  std::list<sedcScomdefDefLine>::iterator defIter;
  std::list<sedcScomdefEntry>::iterator entryIter;

  for (entryIter = scomdefEntries.begin(); entryIter != scomdefEntries.end(); entryIter++) {
#ifdef _LP64
    printf("\n\nValid: %d, States: %#08X, Address: %#016lX\n", entryIter->valid, entryIter->states, entryIter->addresses[0]);
#else
    printf("\n\nValid: %d, States: %#08X, Address: %#016llX\n", entryIter->valid, entryIter->states, entryIter->addresses[0]);
#endif
    printf("Name: %s\n",entryIter->name.c_str());
    printf("ClockDomain: %s, State: %d\n", entryIter->clkdomain.c_str(), entryIter->clkstate);
    printf("Description:\n");
    for (strIter = entryIter->description.begin(); strIter != entryIter->description.end(); strIter++) {
      printf("  %s\n", strIter->c_str());
    }
    printf("latches\n");
    for (latchIter = entryIter->latches.begin(); latchIter != entryIter->latches.end(); latchIter++) {
      printf("  FSI: %d JTAG: %d, lhsNum:%d rhsNum: %d length: %d latchName: %s\n", latchIter->offsetFSI, latchIter->offsetJTAG, latchIter->lhsNum, latchIter->rhsNum, latchIter->length, latchIter->latchName.c_str());
    }
    printf("Definition\n");
    for (defIter = entryIter->definition.begin(); defIter != entryIter->definition.end(); defIter++) {
      printf("  DialName: %s lhsNum: %d rhsNum: %d length: %d\n", defIter->dialName.c_str(), defIter->lhsNum, defIter->rhsNum, defIter->length);
      for (strIter = defIter->detail.begin(); strIter != defIter->detail.end(); strIter++) {
        printf("  %s\n", strIter->c_str());
      }
    }
  }
}
