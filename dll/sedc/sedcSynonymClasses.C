/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File sedcSynonymClasses.C                                   
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
#define sedcSynonymClasses_C
#include <sedcSynonymClasses.H>

#undef sedcSynonymClasses_C


/*******************************************************************************/
/* sedcSynonymLine -+- sedcSynonymLine -+- sedcSynonymLine -+- sedcSynonymLine */
/* sedcSynonymLine -+- sedcSynonymLine -+- sedcSynonymLine -+- sedcSynonymLine */
/*******************************************************************************/
//----------------------------------------------------------------------
//  Constructors
//----------------------------------------------------------------------
// Moved inline in header


//----------------------------------------------------------------------
//  Destructor
//----------------------------------------------------------------------
// Moved inline in header

sedcSynonymLine::sedcSynonymLine(const sedcSynonymLine &rhs) {

  state = rhs.state;
  lineName = rhs.lineName;
  lineExtras = rhs.lineExtras;

}

int sedcSynonymLine::operator=(const sedcSynonymLine &rhs) {

  state = rhs.state;
  lineName = rhs.lineName;
  lineExtras = rhs.lineExtras;

  return 0;
}

void sedcSynonymLine::reset() {
  state = 0x0;
  lineName = "";
  lineExtras = "";
}

/***********************************************************************************/
/* sedcSynonymEntry -+- sedcSynonymEntry -+- sedcSynonymEntry -+- sedcSynonymEntry */
/* sedcSynonymEntry -+- sedcSynonymEntry -+- sedcSynonymEntry -+- sedcSynonymEntry */
/***********************************************************************************/
//----------------------------------------------------------------------
//  Constructors
//----------------------------------------------------------------------
// Moved inline in header

//----------------------------------------------------------------------
//  Destructor
//----------------------------------------------------------------------
// Moved inline in header

sedcSynonymEntry::sedcSynonymEntry(const sedcSynonymEntry &rhs) {

  valid = rhs.valid;
  name = rhs.name;
  realName = rhs.realName;
  synonymLines = rhs.synonymLines;

}

int sedcSynonymEntry::operator=(const sedcSynonymEntry &rhs) {

  valid = rhs.valid;
  name = rhs.name;
  realName = rhs.realName;
  synonymLines = rhs.synonymLines;

  return 0;
}
