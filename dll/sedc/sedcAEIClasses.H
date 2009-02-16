#ifndef sedcAEIClasses_h
#define sedcAEIClasses_h
/* $Header$ */
// Copyright **********************************************************
//                                                                      
// File sedcAEIClasses.H                                               
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
//  Name:  sedcAEIClasses
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
#include <list>
#include <string>
#include <vector>
#include <sedcCommonClasses.H>

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------

class sedcAEIEnum
{
   public:
     // Constructors
     inline sedcAEIEnum() {};

     // Destructor
     virtual ~sedcAEIEnum() {};


     // Operators
     int operator=(const sedcAEIEnum &rhs);  ///< Used for copy
     sedcAEIEnum(const sedcAEIEnum &rhs);  ///< Used for copy


  public:  // Data
    std::string  enumName;
    std::vector<unsigned int> enumValue;  // Done this way so it can dynamically resize as needed
    unsigned int enumLength;              // Length of the data we read in, in nibbles

  private:  // functions

  private:  // Data

};


class sedcAEIEntry
{
   public:
     // Constructors
     inline sedcAEIEntry() {};

     // Destructor
     virtual inline ~sedcAEIEntry() {};


     // Operators
     int operator=(const sedcAEIEntry &rhs);  ///< Used for copy
     sedcAEIEntry(const sedcAEIEntry &rhs);  ///< Used for copy
     int operator==(const sedcAEIEntry &rhs) const;  ///< Used for finds

  public:  // Data
    bool           valid;     ///< Indicates if the spy read from the file has valid syntax
    std::string    name;      ///< String name of the spy we are working with
    unsigned int   states;    ///< Contains info on all states contained in a spy (ecc, group bits, clock dependence,     etc..)
    unsigned int   length;                ///< The length of the spy as the user would see it from the command line 
    std::list<sedcLatchLine> aeiLines;      ///< Linked List holding all of the line by line spy info
    std::list<sedcAEIEnum>   aeiEnums;      ///< Linked List holding any enum values
    std::list<std::string>   aeiEpcheckers; ///< Linked List holding all the eplatches sets that are touched by our latches
    std::list<sedcLatchLine> aeiLatches;      ///< Linked List holding all the latches found in a spy in EDC mode, before being run though the builder.

  private:  // functions

  private:  // Data

};

#endif /* sedcAEIClasses_h */
