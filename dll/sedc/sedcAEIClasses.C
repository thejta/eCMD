/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File sedcAEIClasses.C                                   
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
// End Copyright *******************************************************

// Module Description **************************************************
//
// Description: 
//
// End Module Description **********************************************

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                     12/09/03 albertj  Initial Creation
//
// End Change Log *****************************************************

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
