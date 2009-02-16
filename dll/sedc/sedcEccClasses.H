#ifndef sedcEccClasses_h
#define sedcEccClasses_h
/* $Header$ */
// Copyright **********************************************************
//                                                                      
// File sedcEccClasses.H                                               
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 2003                                         
//                                                                      
// The source code for this program is not published or otherwise       
// divested of its trade secrets, irrespective of what has been         
// deposited with the U.S. Copyright Office.                             
//                                                                      
// End Copyright ******************************************************

// Class Description *************************************************
//                                                                      
//  Name:  sedcEccClasses
//  Base class: 
//
//  Description: 
//  Usage:
//
// End Class Description *********************************************

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              albertj  Initial Creation
//
// End Change Log *****************************************************

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <string>
#include <vector>
#include <sedcAEIClasses.H>

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------
class sedcEplatchesLine
{
   public:
     // Constructors
     inline sedcEplatchesLine():
     state(0x0)
     {};

     // Destructor
     virtual inline ~sedcEplatchesLine() {};

     // Operators
     int operator=(const sedcEplatchesLine &rhs);  ///< Used for copy
     sedcEplatchesLine(const sedcEplatchesLine &rhs);  ///< Used for copy

     /* Other functions */
     void reset(); ///< Resets all the variables to they state they were in the constructor

  public:  // Data
    unsigned int state;         ///< Contains information about what states are on
    std::string  comment;       ///< Any comments that might be on the line

  private:  // functions

  private:  // Data

};

class sedcEplatchesEntry
{
   public:
     // Constructors
     inline sedcEplatchesEntry():
     valid(false)
     {};

     // Destructor
     virtual inline ~sedcEplatchesEntry() {};


     // Operators
     int operator=(const sedcEplatchesEntry &rhs);  ///< Used for copy
     sedcEplatchesEntry(const sedcEplatchesEntry &rhs);  ///< Used for copy

     /* Other functions */
     void reset(); ///< Resets all the variables to they state they were in the constructor

  public:  // Data
    bool          valid;                                       ///< Indicates if the eplatches read from the file has valid syntax
    std::string   name;                                 ///< String name of the entry we are
    std::string   function;                             ///< String name of the eccfunc to lookup and use
    sedcAEIEntry  inSpy;                                ///< A spy representing the in latches for ECC or Parity
    sedcAEIEntry  outSpy;                               ///< A spy representing the out latches for ECC or Parity
    std::list<sedcEplatchesLine> eplatchesLines;        ///< The line by line representation of the eplatches

  private:  // functions

  private:  // Data

};

class sedcEccfuncLine
{
   public:
     // Constructors
     inline sedcEccfuncLine():
     state(false),
     tableLength(0)
     {};

     // Destructor
     virtual inline ~sedcEccfuncLine() {};

     // Operators
     int operator=(const sedcEccfuncLine &rhs);  ///< Used for copy
     sedcEccfuncLine(const sedcEccfuncLine &rhs);  ///< Used for copy

     /* Other functions */
     void reset(); ///< Resets all the variables to they state they were in the constructor

  public:  // Data
    unsigned int state;                         ///< Contains information about what states are on
    std::string  comment;                       ///< Any comments that might be on the line
    std::string  parityType;                    ///< The parity type to gen on this line of the ecc
    std::vector<unsigned int>  tableValue;      ///< The table to apply against the in latches to get the out bit
    int  tableLength;                           ///< The length of the table in nibbles  

  private:  // functions

  private:  // Data

};

class sedcEccfuncEntry
{
   public:
     // Constructors
     inline sedcEccfuncEntry():
     valid(false),
     inBits(-1),
     outBits(-1)
     {};

     // Destructor
     virtual inline ~sedcEccfuncEntry() {};


     // Operators
     int operator=(const sedcEccfuncEntry &rhs);  ///< Used for copy
     sedcEccfuncEntry(const sedcEccfuncEntry &rhs);  ///< Used for copy

  public:  // Data
    bool          valid;                        ///< Indicates if the eccfunc read from the file has valid syntax
    std::string   name;                         ///< String name of the entry we are
    int  inBits;                                ///< The number of bits to be fed into the ecc calc
    int  outBits;                               ///< The number of bits to be retreived from the ecc calc
    std::list<sedcEccfuncLine> eccfuncLines;    ///< A line by line breakdown of the eccfunc entry

  private:  // functions

  private:  // Data

};


#endif /* sedcEccClasses_h */
