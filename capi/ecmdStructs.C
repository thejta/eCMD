
// Copyright ***********************************************************
//                                                                      
// File ecmdStructs.C                                   
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 2002                                        
//                                                                      
// The source code for this program is not published or otherwise       
// divested of its trade secrets, irrespective of what has been         
// deposited with the U.S. Copyright Office.                            
//                                                                      
// End Copyright *******************************************************

/* $Header$ */

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define ecmdStructs_C
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>

#include <ecmdStructs.H>
#undef ecmdStructs_C

//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Forward declarations
//----------------------------------------------------------------------

//---------------------------------------------------------------------
//  Constructors
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//  Destructor
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//  Public Member Function Specifications
//---------------------------------------------------------------------


/** @brief Used to sort Cage entries in an ecmdCageData list. */
bool operator< (const ecmdCageData& lhs, const ecmdCageData& rhs) {

    if (lhs.cageId < rhs.cageId)
        return 1;
        
    return 0;
}

/** @brief Used to sort Node entries in an ecmdNodeData list. */
bool operator< (const ecmdNodeData& lhs, const ecmdNodeData& rhs) {

    if (lhs.nodeId < rhs.nodeId)
        return 1;
        
    return 0;
}

/** @brief Used to sort Slot entries in an ecmdSlotData list. */
bool operator< (const ecmdSlotData& lhs, const ecmdSlotData& rhs) {

    if (lhs.slotId < rhs.slotId)
        return 1;
        
    return 0;
}

/** @brief Used to sort Chip entries (based on Pos) in an ecmdChipData list. */
bool operator< (const ecmdChipData& lhs, const ecmdChipData& rhs) {

    if (lhs.chipType < rhs.chipType)
      return 1
    else if (lhs.pos < rhs.pos)
      return 1;
        
    return 0;
}

/** @brief Used to sort Core entries in an ecmdCoreData list. */
bool operator< (const ecmdCoreData& lhs, const ecmdCoreData& rhs) {

    if (lhs.coreId < rhs.coreId)
        return 1;
        
    return 0;

}

/** @brief Used to sort Thread entries in an ecmdThreadData list. */
bool operator< (const ecmdThreadData& lhs, const ecmdThreadData& rhs) {

    if (lhs.threadId < rhs.threadId)
        return 1;
        
    return 0;

}

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              prahl    Initial Creation
//
// End Change Log *****************************************************

