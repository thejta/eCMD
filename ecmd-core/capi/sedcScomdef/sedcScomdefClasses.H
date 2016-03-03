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

#ifndef sedcScomdefClasses_h
#define sedcScomdefClasses_h

// Class Description *************************************************
//                                                                      
//  Name:  sedcScomdefClasses
//  Base class: 
//
//  Description: 
//  Usage:
//
// End Class Description *********************************************

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <string>
#include <vector>
#include <list>
#include <inttypes.h>
#include <sedcCommonClasses.H>


//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------
class sedcScomdefLatch
{
   public:
     // Constructors
     inline sedcScomdefLatch():
     address(0x0)
     {};

     // Destructor
     virtual inline ~sedcScomdefLatch() {};

     // Operators
     sedcScomdefLatch& operator=(const sedcScomdefLatch &rhs);  ///< Used for copy
     sedcScomdefLatch(const sedcScomdefLatch &rhs);  ///< Used for copy
     int operator<(const sedcScomdefLatch &rhs) const;  ///< Used for sort

     /* Other functions */
     void reset(); ///< Resets all the variables to they state they were in the constructor

  public:  // Data
    uint64_t        address;        ///< The scom address of the entry
    sedcLatchLine   sdLine;         ///< The cut up line from parse latch line that has all the stuff I need.

  private:  // functions

  private:  // Data

};

class sedcScomdefDefLine
{
   public:
     // Constructors
     inline sedcScomdefDefLine():
     lhsNum(-1),
     rhsNum(-1),
     length(-1)
     {};

     // Destructor
     virtual inline ~sedcScomdefDefLine() {};


     // Operators
     sedcScomdefDefLine& operator=(const sedcScomdefDefLine &rhs);  ///< Used for copy
     sedcScomdefDefLine(const sedcScomdefDefLine &rhs);  ///< Used for copy

     /* Other functions */
     void reset(); ///< Resets all the variables to they state they were in the constructor

  public:  // Data
    int           lhsNum;    ///< The number on the left of the x:y bits line or the only number if a single bit
    int           rhsNum;    ///< The number on the right of the x:y bits line or a -1 if no :
    int           length;    ///< The length between x & y.  Doing it because I'm a nice guy and stuck on a train for 5 hours
    std::string            dialName;  ///< The dial name associated with this entry.  Found on the same line as the bits.
    std::list<std::string> detail;   ///< If there is more than just the dialname given, this will have all that text.

  private:  // functions

  private:  // Data

};


class sedcScomdefEntry
{
   public:
     // Constructors
     inline sedcScomdefEntry():
     valid(true), 
     states(0),
     mask(0xFFFFFFFFFFFFFFFFull),
     clkstate(SEDC_CLK_RUNNING)
     {};

     // Destructor
     virtual inline ~sedcScomdefEntry() {};

     // Operators
     sedcScomdefEntry& operator=(const sedcScomdefEntry &rhs);  ///< Used for copy
     sedcScomdefEntry(const sedcScomdefEntry &rhs);  ///< Used for copy
     int operator==(const sedcScomdefEntry &rhs) const;  ///< Used for finds
     int operator<(const sedcScomdefEntry &rhs) const;  ///< Used for lower bound

     void reset();

  public:  // Data
    bool                           valid;          ///< This scomdef entry is valid
    uint32_t                       states;         ///< The things I found in the scomdef
    std::vector<uint64_t>          addresses;      ///< The scom address of the entry
    std::string                    name;           ///< The figtree name of the scom address
    std::list<std::string>         description;    ///< A list of strings for the description section of the scomdef
    std::string                    clkdomain;      ///< For now, a string of the clock domains
    uint64_t                       mask;           ///< A mask of bits to check
    sedcClockState                 clkstate;       ///< the state the clocks have to be in
    std::list<sedcScomdefDefLine>  definition;     ///< A list of the contents of the definition section
    std::vector<sedcLatchLine>     latches;        ///< A list of the latches section in 

  private:  // functions

  private:  // Data

};


class sedcScomdefContainer
{
   public:
     // Constructors
     inline sedcScomdefContainer() {};

     // Destructor
     virtual inline ~sedcScomdefContainer() {};

     // Operators
     sedcScomdefContainer& operator=(const sedcScomdefContainer &rhs);  ///< Used for copy
     sedcScomdefContainer(const sedcScomdefContainer &rhs);  ///< Used for copy
     //int operator==(const sedcScomdefContainer &rhs) const;  ///< Used for finds

     /* Other functions */
     void insertLatches(uint64_t address, std::vector<sedcLatchLine> &latches);

     void scomdefDumper();  ///< Loop through the valid entries and dump their info

  public:  // Data
    std::list<sedcScomdefEntry>      scomdefEntries;        ///< A list of all the sedcScomdefEntries read in
    std::vector<sedcScomdefLatch>    scomdefLatches;        ///< A list of the latches section in 

  private:  // functions

  private:  // Data

};

#endif /* sedcScomdefClasses_h */
