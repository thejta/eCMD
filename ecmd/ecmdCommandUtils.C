// Copyright ***********************************************************
//                                                                      
// File ecmdCommandUtils.C                                   
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 1996                                         
//                                                                      
// The source code for this program is not published or otherwise       
// divested of its trade secrets, irrespective of what has been         
// deposited with the U.S. Copyright Office.                            
//                                                                      
// End Copyright *******************************************************
/* $Header$ */

// Module Description **************************************************
//
// Description: 
//
// End Module Description **********************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define ecmdCommandUtils_C
#include <list>
#include <fstream>
#include <inttypes.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#undef ecmdCommandUtils_C
//----------------------------------------------------------------------
//  User Types
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Macros
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Internal Function Prototypes
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------

int ecmdGetChipData (ecmdChipTarget & i_target, ecmdChipData & o_data) {
  int rc = ECMD_SUCCESS;

  ecmdChipTarget tmp = i_target;
  ecmdQueryData needlesslySlow;
  tmp.cageState = tmp.nodeState = tmp.slotState = tmp.chipTypeState = tmp.posState = ECMD_TARGET_QUERY_FIELD_VALID;
  rc = ecmdQueryConfig(tmp, needlesslySlow, ECMD_QUERY_DETAIL_HIGH);
  if (rc) return rc;

  if (needlesslySlow.cageData.empty()) return ECMD_INVALID_ARGS;
  if (needlesslySlow.cageData.front().cageId != i_target.cage) return ECMD_INVALID_ARGS;
  if (needlesslySlow.cageData.front().nodeData.empty()) return ECMD_INVALID_ARGS;
  if (needlesslySlow.cageData.front().nodeData.front().nodeId != i_target.node) return ECMD_INVALID_ARGS;
  if (needlesslySlow.cageData.front().nodeData.front().slotData.empty()) return ECMD_INVALID_ARGS;
  if (needlesslySlow.cageData.front().nodeData.front().slotData.front().slotId != i_target.slot) return ECMD_INVALID_ARGS;
  if (needlesslySlow.cageData.front().nodeData.front().slotData.front().chipData.empty()) return ECMD_INVALID_ARGS;

  o_data = needlesslySlow.cageData.front().nodeData.front().slotData.front().chipData.front();
  if (o_data.chipType != i_target.chipType || o_data.pos != i_target.pos) {
    return ECMD_INVALID_ARGS;
  }
    
  return rc;
}

int ecmdCheckExpected (ecmdDataBuffer & i_data, ecmdDataBuffer & i_expected) {

  int wordCounter = 0;
  uint32_t maxBits = 32;
  uint32_t numBits = i_data.getBitLength();
  uint32_t numToFetch = numBits < maxBits ? numBits : maxBits;
  uint32_t curData, curExpected;

  while (numToFetch > 0) {

    curData = i_data.getWord(wordCounter);
    curExpected = i_expected.getWord(wordCounter);

    if (numToFetch == maxBits) {
      if (curData != curExpected) 
        return 0;
    }
    else {
      uint32_t mask = 0x80000000;
      for (int i = 0; i < numToFetch; i++, mask >>= 1) {
        if ( (curData & mask) != (curExpected & mask) ) {
          return 0;
        }
      }
    }

    numBits -= numToFetch;
    numToFetch = (numBits < maxBits) ? numBits : maxBits;
    wordCounter++;
  }

  return 1;
        
}

int ecmdPrintHelp(const char* i_command) {

  int rc = ECMD_SUCCESS;
  std::string file;
  ecmdChipTarget target;
  std::ifstream ins;
  std::string curLine;

  /* Get the path to the help text files */
  rc = ecmdQueryFileLocation(target, ECMD_FILE_HELPTEXT, file);
  if (rc) return rc;

  file += i_command; file += ".htxt";

  /* Let's go open this guy */
  ins.open(file.c_str());
  if (ins.fail()) {
    ecmdOutputError(("Error occured opening help text file: " + file).c_str());
    return ECMD_INVALID_ARGS;  //change this
  }

  while (getline(ins, curLine)) {
    curLine += '\n';
    ecmdOutput(curLine.c_str());
  }
  ins.close();

  return rc;

}
  


// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
