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
#include <inttypes.h>
#include <string.h>
#include <stdio.h>

#include <ecmdCommandUtils.H>
#include <ecmdIntReturnCodes.H>
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

static std::list<ecmdCageData> ecmdSystemConfigData;
static std::list<ecmdCageData>::iterator ecmdCurCage;
static std::list<ecmdNodeData>::iterator ecmdCurNode;
static std::list<ecmdChipData>::iterator ecmdCurChip;
static std::list<ecmdCoreData>::iterator ecmdCurCore;
static std::list<ecmdThreadData>::iterator ecmdCurThread;
static uint8_t ecmdLooperInitFlag;

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------

int ecmdConfigLooperInit (ecmdChipTarget & io_target) {

  int rc = ECMD_SUCCESS;

  ecmdChipTarget queryTarget = io_target;

  if (queryTarget.cageState == ECMD_TARGET_FIELD_UNUSED) {
    queryTarget.cageState = ECMD_TARGET_QUERY_IGNORE;
  }

  if (queryTarget.nodeState == ECMD_TARGET_FIELD_UNUSED) {
    queryTarget.nodeState = ECMD_TARGET_QUERY_IGNORE;
  }

  if (queryTarget.chipTypeState == ECMD_TARGET_FIELD_UNUSED) {
    queryTarget.chipTypeState = ECMD_TARGET_QUERY_IGNORE;
  }

  if (queryTarget.posState == ECMD_TARGET_FIELD_UNUSED) {
    queryTarget.posState = ECMD_TARGET_QUERY_IGNORE;
  }

  if (queryTarget.coreState == ECMD_TARGET_FIELD_UNUSED) {
    queryTarget.coreState = ECMD_TARGET_QUERY_IGNORE;
  }

  if (queryTarget.threadState == ECMD_TARGET_FIELD_UNUSED) {
    queryTarget.threadState = ECMD_TARGET_QUERY_IGNORE;
  }

  rc = ecmdQuerySelected(queryTarget, ecmdSystemConfigData);

  ecmdCurCage = ecmdSystemConfigData.begin();
  ecmdLooperInitFlag = 1;

  return rc;
}

int ecmdConfigLooperNext (ecmdChipTarget & io_target) {

  uint8_t CAGE = 0;
  uint8_t NODE = 1;
  uint8_t CHIP = 2;
  uint8_t CORE = 3;
  uint8_t THREAD = 4;

  uint8_t level = CAGE;
  uint8_t valid = 1;

  if (ecmdCurCage == ecmdSystemConfigData.end()) {
    return 0;
  }

  if (io_target.cage != (*ecmdCurCage).cageId || ecmdLooperInitFlag) {

    io_target.cage = (*ecmdCurCage).cageId;
    ecmdCurNode = (*ecmdCurCage).nodeData.begin();

    if (io_target.nodeState == ECMD_TARGET_FIELD_UNUSED || ecmdCurNode == (*ecmdCurCage).nodeData.end()) {
      valid = 0;
    }
    else {
      level = NODE;
    }

  }
  else {
    level = NODE;
  }

  if (level == NODE && (io_target.node != (*ecmdCurNode).nodeId || ecmdLooperInitFlag)) {

    io_target.node = (*ecmdCurNode).nodeId;
    ecmdCurChip = (*ecmdCurNode).chipData.begin();

    if (io_target.chipTypeState == ECMD_TARGET_FIELD_UNUSED || ecmdCurChip == (*ecmdCurNode).chipData.end()) {
      valid = 0;
    }
    else {
      level = CHIP;
    }

  }
  else if (valid) {
    level = CHIP;
  }

  if (level == CHIP && (io_target.chipType != (*ecmdCurChip).chipType || io_target.pos != (*ecmdCurChip).pos || ecmdLooperInitFlag)) {

    io_target.chipType = (*ecmdCurChip).chipType;
    io_target.pos = (*ecmdCurChip).pos;
    ecmdCurCore = (*ecmdCurChip).coreData.begin();

    if (io_target.coreState == ECMD_TARGET_FIELD_UNUSED || ecmdCurCore == (*ecmdCurChip).coreData.end()) {
      valid = 0;
    }
    else {
      level = CORE;
    }

  }
  else if (valid) {
    level = CORE;
  }

  if (level == CORE && (io_target.core != (*ecmdCurCore).coreId || ecmdLooperInitFlag)) {

    io_target.core = (*ecmdCurCore).coreId;
    ecmdCurThread = (*ecmdCurCore).threadData.begin();

    if (io_target.threadState == ECMD_TARGET_FIELD_UNUSED || ecmdCurThread == (*ecmdCurCore).threadData.end()) {
      valid = 0;
    }
    else {
      level = THREAD;
    }

  }
  else if (valid) {
    level = THREAD;
  }

  if (level == THREAD && (io_target.thread != (*ecmdCurThread).threadId || ecmdLooperInitFlag)) {

    io_target.thread = (*ecmdCurThread).threadId;

  }

  switch (level) {

    case 4:  //thread
      ecmdCurThread++;
      if (ecmdCurThread != (*ecmdCurCore).threadData.end()) {
        break;
      }

    case 3:  //core
      ecmdCurCore++;
      if (ecmdCurCore != (*ecmdCurChip).coreData.end()) {
        break;
      }

    case 2:  //chip
      ecmdCurChip++;
      if (ecmdCurChip != (*ecmdCurNode).chipData.end()) {
        break;
      }

    case 1:  //node
      ecmdCurNode++;
      if (ecmdCurNode != (*ecmdCurCage).nodeData.end()) {
        break;
      }

    case 0:  //cage
      ecmdCurCage++;
      break;

    default:
      //shouldn't get here
      break;
  }

  if (ecmdLooperInitFlag) {
    ecmdLooperInitFlag = 0;
  }

  return 1;

}


void ecmdWriteTarget (ecmdChipTarget & i_target) {

  std::string printed;
  char util[7];

  if (i_target.chipTypeState != ECMD_TARGET_FIELD_UNUSED) {
    printed = i_target.chipType + ":";
    //printed += " ";  //space vs. colon?
  }

  //always do cage
  sprintf(util, "k%d", i_target.cage);
  printed += util;

  if (i_target.nodeState != ECMD_TARGET_FIELD_UNUSED) {
    sprintf(util, ":n%d", i_target.node);
    printed += util;

    if (i_target.posState != ECMD_TARGET_FIELD_UNUSED) {

      if (i_target.pos < 10) {
        sprintf(util, ":p0%d", i_target.pos);
      }
      else {
        sprintf(util, ":p%d", i_target.pos);
      }
      printed += util;

      if (i_target.coreState != ECMD_TARGET_FIELD_UNUSED) {
        sprintf(util, ":c%d", i_target.core);
        printed += util;

        if (i_target.threadState != ECMD_TARGET_FIELD_UNUSED) {
          sprintf(util, ":t%d", i_target.thread);
          printed += util;
        }
        else {
          printed += "   ";  //adjust spacing
        }

      } //core
      else {
        printed += "      ";  //adjust spacing
      }

    } //pos
    else {
      printed += "          ";  //adjust spacing
    }
    
  } //node

  //set a space between the target info and the data
  printed += " "; 

  ecmdOutput(printed.c_str());

}

int ecmdWriteDataFormatted (ecmdDataBuffer & i_data, const char * i_format) {

  std::string printed;
  int formTagLength = strlen(i_format);

  if (formTagLength > 1 && (i_format[formTagLength-1] == 'w' || i_format[formTagLength-1] == 'b')) {

    uint8_t state = 0;
    uint8_t HEXLEFT = 1;
    uint8_t HEXRIGHT = 2;
    uint8_t BINARY = 3;

    if (formTagLength == 2) {
      if (i_format[0] == 'x') {
        state = HEXLEFT;
      }
      else if (i_format[0] == 'b') {
        state = BINARY;
      }
    }
    else if (formTagLength == 3) {
      if (i_format[0] == 'x') {
        if (i_format[1] == 'l') {
          state = HEXLEFT;
        }
        else if (i_format[1] == 'r') {
          state = HEXRIGHT;
        }
      }
    }

    if (state) {

      int numBits = i_data.getBitLength();
      int maxBits = 32;
      if (i_format[formTagLength-1] == 'b') {
        maxBits = 4;
      }

      int startBit = 0;
      int numToFetch = numBits < maxBits ? numBits : maxBits;

      while (numToFetch > 0) {

        if (state == HEXLEFT) {
          printed = i_data.genHexLeftStr(startBit, numToFetch);
        }
        else if (state == HEXRIGHT) {
          printed = i_data.genHexRightStr(startBit, numToFetch);
        }
        else { //binary data
          printed = i_data.genBinStr(startBit, numToFetch);
        }

        ecmdOutput( printed.c_str() );
        ecmdOutput(" ");

        startBit += numToFetch;
        numBits -= numToFetch;
        numToFetch = (numBits < maxBits) ? numBits: maxBits;
      }

      ecmdOutput("\n");
      return ECMD_SUCCESS;
    }
  }
  else {

    if (!strcmp(i_format, "x") || !strcmp(i_format, "xl")) {
      printed = i_data.genHexLeftStr();
    }
    else if (!strcmp(i_format, "xr")) {
      printed = i_data.genHexRightStr();
    }
    else if (!strcmp(i_format, "b")) {
      printed = i_data.genBinStr();
    }
    else if (!strcmp(i_format, "d")) {
      // printed = i_data.genDecStr();  need to implement this
    }

    ecmdOutput(printed.c_str());
    ecmdOutput("\n");
    return ECMD_SUCCESS;
  }

  //if we made it this far, it's a special format 

  return ECMD_SUCCESS;
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

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
