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

  ecmdOutput("Looper Initialized\n");

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

  ecmdOutput("Entering Looper Next\n");

  if (ecmdCurCage == ecmdSystemConfigData.end()) {
    ecmdOutput("Immediate bailout\n");
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
      else {
        ecmdOutput("Fell through thread\n");
      }

    case 3:  //core
      ecmdCurCore++;
      if (ecmdCurCore != (*ecmdCurChip).coreData.end()) {
        break;
      }
      else {
        ecmdOutput("Fell through core\n");
      }

    case 2:  //chip
      ecmdCurChip++;
      if (ecmdCurChip != (*ecmdCurNode).chipData.end()) {
        break;
      }
      else {
        ecmdOutput("Fell through chip\n");
      }

    case 1:  //node
      ecmdCurNode++;
      if (ecmdCurNode != (*ecmdCurCage).nodeData.end()) {
        break;
      }
      else {
        ecmdOutput("Fell through node\n");
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

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
