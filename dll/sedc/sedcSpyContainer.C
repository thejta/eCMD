/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File sedcSpyContainer.C                                   
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
#define sedcSpyContainer_C
#include <stdio.h>
#include <sedcSpyContainer.H>
#include <ecmdSharedUtils.H>

#undef sedcSpyContainer_C
//----------------------------------------------------------------------
//  Constructors
//----------------------------------------------------------------------
// Moved inline in header

//----------------------------------------------------------------------
//  Destructor
//----------------------------------------------------------------------
sedcSpyContainer::~sedcSpyContainer() {

  /* Delete my assigned pointers */
  if (aeiEntry != NULL)
    delete aeiEntry; aeiEntry = NULL;

  if (synonymEntry != NULL)
    delete synonymEntry; synonymEntry = NULL;

  if (eplatchesEntry != NULL)
    delete eplatchesEntry; eplatchesEntry = NULL;

  if (eccfuncEntry != NULL)
    delete eccfuncEntry; eccfuncEntry = NULL;

}

sedcSpyContainer::sedcSpyContainer(const sedcSpyContainer &rhs) {

  valid = rhs.valid;
  type = rhs.type;
  name = rhs.name;
  hashKey = rhs.hashKey;
  /* aeiEntry */
  if (rhs.aeiEntry != NULL) {
    aeiEntry = new sedcAEIEntry;
    *aeiEntry = *rhs.aeiEntry;
  } else {
    aeiEntry = NULL;
  }

  /* synonymEntry */
  if (rhs.synonymEntry != NULL) {
    synonymEntry = new sedcSynonymEntry;
    *synonymEntry = *rhs.synonymEntry;
  } else {
    synonymEntry = NULL;
  }

  /* EplatchesEntry */
  if (rhs.eplatchesEntry != NULL) {
    eplatchesEntry = new sedcEplatchesEntry;
    *eplatchesEntry = *rhs.eplatchesEntry;
  } else {
    eplatchesEntry = NULL;
  }

  /* eccfuncEntry */
  if (rhs.eccfuncEntry != NULL) {
    eccfuncEntry = new sedcEccfuncEntry;
    *eccfuncEntry = *rhs.eccfuncEntry;
  } else {
    eccfuncEntry = NULL;
  }

}

int sedcSpyContainer::operator=(const sedcSpyContainer &rhs) {

  valid = rhs.valid;
  type = rhs.type;
  name = rhs.name;
  hashKey = rhs.hashKey;

  /* aeiEntry */
  if (aeiEntry != 0)
  {
     delete aeiEntry;
     aeiEntry = 0;
  }
  if (rhs.aeiEntry != NULL) {
    aeiEntry = new sedcAEIEntry;
    *aeiEntry = *rhs.aeiEntry;
  }

  /* synonymEntry */
  if (synonymEntry != 0)
  {
     delete synonymEntry;
     synonymEntry = 0;
  }
  if (rhs.synonymEntry != NULL) {
    synonymEntry = new sedcSynonymEntry;
    *synonymEntry = *rhs.synonymEntry;
  }

  /* eplatchesEntry */
  if (eplatchesEntry != 0)
  {
     delete eplatchesEntry;
     eplatchesEntry = 0;
  }
  if (rhs.eplatchesEntry != NULL) {
    eplatchesEntry = new sedcEplatchesEntry;
    *eplatchesEntry = *rhs.eplatchesEntry;
  }

  /* eccfuncEntry */
  if (eccfuncEntry != 0)
  {
     delete eccfuncEntry;
     eccfuncEntry = 0;
  }
  if (rhs.eccfuncEntry != NULL) {
    eccfuncEntry = new sedcEccfuncEntry;
    *eccfuncEntry = *rhs.eccfuncEntry;
  }

  return 0;
}


/* Inline this speed things up - JTA 09/11/05 */
//int sedcSpyContainer::operator==(const sedcSpyContainer &rhs) const {
//  return (hashKey == rhs.hashKey && name==rhs.name);
//}

int sedcSpyContainer::operator<(const sedcSpyContainer &rhs) const {
  return (hashKey < rhs.hashKey);
}

sedcAEIEntry sedcSpyContainer::getAEIEntry() {
  /* Far from the best error handling, but if you are here you're doing it wrong */
  if (aeiEntry == NULL) {
    printf("You tried to fetch the aeiEntry pointer when it was NULL!\n");
    exit(1);
  }

  return *aeiEntry;
}

sedcAEIEntry& sedcSpyContainer::getAEIEntryRef() {
  /* Far from the best error handling, but if you are here you're doing it wrong */
  if (aeiEntry == NULL) {
    printf("You tried to fetch the aeiEntry pointer when it was NULL!\n");
    exit(1);
  }

  return *aeiEntry;
}

void sedcSpyContainer::setAEIEntry(sedcAEIEntry newAEIEntry) {
  if (aeiEntry != 0)
  {
     delete aeiEntry;
     aeiEntry = 0;
  }
  aeiEntry = new sedcAEIEntry;
  *aeiEntry = newAEIEntry;
}

sedcSynonymEntry sedcSpyContainer::getSynonymEntry() {
  /* Far from the best error handling, but if you are here you're doing it wrong */
  if (synonymEntry == NULL) {
    printf("You tried to fetch the synonymEntry pointer when it was NULL!\n");
    exit(1);
  }

  return *synonymEntry;
}

sedcSynonymEntry& sedcSpyContainer::getSynonymEntryRef() {
  /* Far from the best error handling, but if you are here you're doing it wrong */
  if (synonymEntry == NULL) {
    printf("You tried to fetch the synonymEntry pointer when it was NULL!\n");
    exit(1);
  }

  return *synonymEntry;
}

void sedcSpyContainer::setSynonymEntry(sedcSynonymEntry newSynonymEntry) {
  if (synonymEntry != 0)
  {
     delete synonymEntry;
     synonymEntry = 0;
  }
  synonymEntry = new sedcSynonymEntry;
  *synonymEntry = newSynonymEntry;
}

sedcEplatchesEntry sedcSpyContainer::getEplatchesEntry() {
  /* Far from the best error handling, but if you are here you're doing it wrong */
  if (eplatchesEntry == NULL) {
    printf("You tried to fetch the eplatchesEntry pointer when it was NULL!\n");
    exit(1);
  }

  return *eplatchesEntry;
}

sedcEplatchesEntry& sedcSpyContainer::getEplatchesEntryRef() {
  /* Far from the best error handling, but if you are here you're doing it wrong */
  if (eplatchesEntry == NULL) {
    printf("You tried to fetch the eplatchesEntry pointer when it was NULL!\n");
    exit(1);
  }

  return *eplatchesEntry;
}

void sedcSpyContainer::setEplatchesEntry(sedcEplatchesEntry newEplatchesEntry) {
  if (eplatchesEntry != 0)
  {
     delete eplatchesEntry;
     eplatchesEntry = 0;
  }
  eplatchesEntry = new sedcEplatchesEntry;
  *eplatchesEntry = newEplatchesEntry;
}

sedcEccfuncEntry sedcSpyContainer::getEccfuncEntry() {
  /* Far from the best error handling, but if you are here you're doing it wrong */
  if (eccfuncEntry == NULL) {
    printf("You tried to fetch the eccfuncEntry pointer when it was NULL!\n");
    exit(1);
  }

  return *eccfuncEntry;
}

sedcEccfuncEntry& sedcSpyContainer::getEccfuncEntryRef() {
  /* Far from the best error handling, but if you are here you're doing it wrong */
  if (eccfuncEntry == NULL) {
    printf("You tried to fetch the eccfuncEntry pointer when it was NULL!\n");
    exit(1);
  }

  return *eccfuncEntry;
}

void sedcSpyContainer::setEccfuncEntry(sedcEccfuncEntry newEccfuncEntry) {
  if (eccfuncEntry != 0)
  {
     delete eccfuncEntry;
     eccfuncEntry = 0;
  }
  eccfuncEntry = new sedcEccfuncEntry;
  *eccfuncEntry = newEccfuncEntry;
}

void sedcSpyContainer::setName(std::string newName) {
  name = newName;
  hashKey = ecmdHashString32(name.c_str(), 0);
}
