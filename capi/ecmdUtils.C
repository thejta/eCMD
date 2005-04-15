// Copyright **********************************************************
//                                                                      
// File ecmdDataBuffer.C                                               
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

/* $Header$ */

/**
 * @file ecmdUtils.C
 * @brief Useful functions for use throughout the ecmd C API
 *
 */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder     Description                       
//  ---- -------- ---- -------- -----     -----------------------------
//  @01  STG4466       03/10/05 Prahl     Fix up Beam errors
//   
// End Change Log *****************************************************

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <inttypes.h>
#include <string>
#include <vector>
#include <list>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <fstream>

#include <ecmdUtils.H>
#include <ecmdSharedUtils.H>
#include <ecmdClientCapi.H>
#include <ecmdReturnCodes.H>
#include <ecmdCommandUtils.H>

#ifndef FIPSODE
# include <sedcScomdefParser.H>
# include <sedcScomdefClasses.H>
#endif

//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------



typedef enum {
  ECMD_FORMAT_NONE,
  ECMD_FORMAT_X,
  ECMD_FORMAT_XR,
  ECMD_FORMAT_XW,
  ECMD_FORMAT_XRW,
  ECMD_FORMAT_B,
  ECMD_FORMAT_BN,
  ECMD_FORMAT_BW,
  ECMD_FORMAT_BX,
  ECMD_FORMAT_BXN,
  ECMD_FORMAT_BXW,
  ECMD_FORMAT_MEM,
  ECMD_FORMAT_MEMA,
  ECMD_FORMAT_MEMD,
  ECMD_FORMAT_MEME
} ecmdFormatState_t;


char frontFPPTxt[40]; //@01c Bumped from 35 to 40 (max str len = 37)
extern int  fppCallCount;


//--------------------------------------------------------------------
// Macros
//--------------------------------------------------------------------

void ecmdIncrementLooperIterators (uint8_t level, ecmdLooperData& io_state);
std::string printEcmdChipTargetState_t(ecmdChipTargetState_t state);
/**
 @brief Print ecmdDataBuffer's for the EFPP
 @param i_data Databuffer to print
 @param i_tabStop Any additional spacing that should be done in display (ie "  \t")
*/
void printEcmdDataBuffer(std::string variableType, std::string variableName, ecmdDataBuffer & i_data, std::string i_tabStop);

uint32_t readScomDefFile(uint32_t address, std::ifstream &scomdefFile);
//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------
#ifdef FIPSODE
tracDesc_t g_ptrc; /** Procedure Trace Descriptor **/
TRAC_INIT(&g_ptrc, "PTRC", 0x8000);
#endif

#ifndef ECMD_STRIP_DEBUG
/* This is from ecmdClientCapi.C */
extern int ecmdClientDebug;
#endif

//----------------------------------------------------------------------
//  User Types
//----------------------------------------------------------------------



uint32_t ecmdConfigLooperInit (ecmdChipTarget & io_target, ecmdConfigLoopType_t i_looptype, ecmdLooperData& io_state) {

  uint32_t rc = ECMD_SUCCESS;
  ecmdChipTarget queryTarget;
  int myTcount =0;

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug >= 8) {
    std::vector< void * > args;
    args.push_back((void*) &io_target);
    args.push_back((void*) &i_looptype);
    args.push_back((void*) &io_state);
    args.push_back((void*) &rc);
    fppCallCount++;
    myTcount = fppCallCount;
    ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t ecmdConfigLooperInit (ecmdChipTarget & io_target, ecmdConfigLoopType_t i_looptype, ecmdLooperData& io_state)",args);
  }

#endif

  /* Are we using a unitid ? */
  if ((io_target.chipTypeState == ECMD_TARGET_QUERY_FIELD_VALID) && (io_target.chipType.length() > 0) && (io_target.chipType[0] == 'u')) {

    /* Ok, we need to strip the u off the front for this call */
    std::string unitid = io_target.chipType.substr(1);
    io_state.unitIdTargets.clear();
    rc = ecmdUnitIdStringToTarget(unitid, io_state.unitIdTargets);
    if (rc == ECMD_INVALID_ARGS)
      ecmdOutputError("ecmdConfigLooperInit - Invalid Unitid specified\n");
    else if (rc == ECMD_FUNCTION_NOT_SUPPORTED)
      ecmdOutputError("ecmdConfigLooperInit - Current plugin doesn't support Unitid's\n");

    if (rc) return rc;
    io_state.ecmdUseUnitid = true;
    io_state.ecmdLooperInitFlag = true;
    io_state.prevTarget = io_target;
    io_state.curUnitIdTarget = io_state.unitIdTargets.begin();


    /* Ok, we still need to call queryconfig, we will use this to make sure the targets that come back actually exist */
    queryTarget.cageState = ECMD_TARGET_QUERY_WILDCARD;
    queryTarget.nodeState = ECMD_TARGET_QUERY_WILDCARD;
    queryTarget.slotState = ECMD_TARGET_QUERY_WILDCARD;
    queryTarget.chipTypeState = ECMD_TARGET_QUERY_WILDCARD;
    queryTarget.posState = ECMD_TARGET_QUERY_WILDCARD;
    queryTarget.coreState = ECMD_TARGET_QUERY_WILDCARD;
    queryTarget.threadState = ECMD_TARGET_QUERY_WILDCARD;

    rc = ecmdQueryConfig(queryTarget, io_state.ecmdSystemConfigData);
    if (rc) return rc;

    /* Standard physical targets */
  } else {


    io_state.ecmdUseUnitid = false;
    queryTarget = io_target;


    /* Setup the Query target */
    if (queryTarget.cageState == ECMD_TARGET_FIELD_UNUSED)        queryTarget.cageState = ECMD_TARGET_QUERY_IGNORE;
    if (queryTarget.nodeState == ECMD_TARGET_FIELD_UNUSED)        queryTarget.nodeState = ECMD_TARGET_QUERY_IGNORE;
    if (queryTarget.slotState == ECMD_TARGET_FIELD_UNUSED)        queryTarget.slotState = ECMD_TARGET_QUERY_IGNORE;
    if (queryTarget.chipTypeState == ECMD_TARGET_FIELD_UNUSED)    queryTarget.chipTypeState = ECMD_TARGET_QUERY_IGNORE;
    if (queryTarget.posState == ECMD_TARGET_FIELD_UNUSED)         queryTarget.posState = ECMD_TARGET_QUERY_IGNORE;
    if (queryTarget.coreState == ECMD_TARGET_FIELD_UNUSED)        queryTarget.coreState = ECMD_TARGET_QUERY_IGNORE;
    if (queryTarget.threadState == ECMD_TARGET_FIELD_UNUSED)      queryTarget.threadState = ECMD_TARGET_QUERY_IGNORE;

    /* Initialize defaults into the incoming target */
    if (io_target.cageState == ECMD_TARGET_QUERY_WILDCARD)     io_target.cage = 0;
    if (io_target.nodeState == ECMD_TARGET_QUERY_WILDCARD)     io_target.node = 0;
    if (io_target.slotState == ECMD_TARGET_QUERY_WILDCARD)     io_target.slot = 0;
    if (io_target.chipTypeState == ECMD_TARGET_QUERY_WILDCARD) io_target.chipType = "na";
    if (io_target.posState == ECMD_TARGET_QUERY_WILDCARD)      io_target.pos = 0;
    if (io_target.coreState == ECMD_TARGET_QUERY_WILDCARD)     io_target.core = 0;
    if (io_target.threadState == ECMD_TARGET_QUERY_WILDCARD)   io_target.thread = 0;

    /* Set all the states to valid, unless they are unused */
    if (io_target.cageState != ECMD_TARGET_FIELD_UNUSED)          io_target.cageState = ECMD_TARGET_FIELD_VALID;
    if (io_target.nodeState != ECMD_TARGET_FIELD_UNUSED)          io_target.nodeState = ECMD_TARGET_FIELD_VALID;
    if (io_target.slotState != ECMD_TARGET_FIELD_UNUSED)          io_target.slotState = ECMD_TARGET_FIELD_VALID;
    if (io_target.chipTypeState != ECMD_TARGET_FIELD_UNUSED)      io_target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    if (io_target.posState != ECMD_TARGET_FIELD_UNUSED)           io_target.posState = ECMD_TARGET_FIELD_VALID;
    if (io_target.coreState != ECMD_TARGET_FIELD_UNUSED)          io_target.coreState = ECMD_TARGET_FIELD_VALID;
    if (io_target.threadState != ECMD_TARGET_FIELD_UNUSED)        io_target.threadState = ECMD_TARGET_FIELD_VALID;

    if (i_looptype == ECMD_ALL_TARGETS_LOOP) 
      rc = ecmdQueryConfig(queryTarget, io_state.ecmdSystemConfigData);
    else {
      rc = ecmdQuerySelected(queryTarget, io_state.ecmdSystemConfigData, i_looptype);

      /* Selected queries can change our states, so let's update them */
      if (queryTarget.cageState == ECMD_TARGET_QUERY_IGNORE)      io_target.cageState = ECMD_TARGET_FIELD_UNUSED;
      if (queryTarget.nodeState == ECMD_TARGET_QUERY_IGNORE)      io_target.nodeState = ECMD_TARGET_FIELD_UNUSED;
      if (queryTarget.slotState == ECMD_TARGET_QUERY_IGNORE)      io_target.slotState = ECMD_TARGET_FIELD_UNUSED;
      if (queryTarget.chipTypeState == ECMD_TARGET_QUERY_IGNORE)  io_target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
      if (queryTarget.posState == ECMD_TARGET_QUERY_IGNORE)       io_target.posState = ECMD_TARGET_FIELD_UNUSED;
      if (queryTarget.coreState == ECMD_TARGET_QUERY_IGNORE)      io_target.coreState = ECMD_TARGET_FIELD_UNUSED;
      if (queryTarget.threadState == ECMD_TARGET_QUERY_IGNORE)    io_target.threadState = ECMD_TARGET_FIELD_UNUSED;
    }
    if (rc) return rc;

    io_state.ecmdCurCage = io_state.ecmdSystemConfigData.cageData.begin();
    io_state.ecmdLooperInitFlag = true;
    io_state.prevTarget = io_target;
  }
#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug >= 8) {
    std::vector< void * > args;
    args.push_back((void*) &io_target);
    args.push_back((void*) &i_looptype);
    args.push_back((void*) &io_state);
    args.push_back((void*) &rc);

    ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t ecmdConfigLooperInit (ecmdChipTarget & io_target, ecmdConfigLoopType_t i_looptype, ecmdLooperData& io_state)",args);
  }

#endif

  return rc;
}

uint32_t ecmdConfigLooperNext (ecmdChipTarget & io_target, ecmdLooperData& io_state) {

  const uint8_t CAGE = 0;
  const uint8_t NODE = 1;
  const uint8_t SLOT = 2;
  const uint8_t CHIP = 3;
  const uint8_t CORE = 4;
  const uint8_t THREAD = 5;
  bool done = false;
  uint8_t level;
  uint32_t rc = 0;
  bool freshLoop = false;               ///< Have we moved forward to a new target at a hier level of hierarchy 

#ifndef ECMD_STRIP_DEBUG
  int myTcount=0;
  if (ecmdClientDebug >= 8) {
     std::vector< void * > args;
     args.push_back((void*) &io_target);
     args.push_back((void*) &io_state);
     args.push_back((void*) &rc);

     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t ecmdConfigLooperNext (ecmdChipTarget & io_target, ecmdLooperData& io_state)",args);
   }

#endif

  /* Are we using unitids ? */
  if (io_state.ecmdUseUnitid) {
    /* We at the end ? */
    while (!done) {
      if (io_state.curUnitIdTarget == io_state.unitIdTargets.end()) {
#ifndef ECMD_STRIP_DEBUG
        if (ecmdClientDebug >= 8) {
          std::vector< void * > args;
          args.push_back((void*) &io_target);
          args.push_back((void*) &io_state);
          args.push_back((void*) &rc);

          ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t ecmdConfigLooperNext (ecmdChipTarget & io_target, ecmdLooperData& io_state)",args);
        }
#endif
        return 0;
      }

      io_target = *(io_state.curUnitIdTarget);
      io_state.curUnitIdTarget ++;

      /* Is this target actually configured, if not try the next one */
      if (ecmdQueryTargetConfigured(io_target, &(io_state.ecmdSystemConfigData))) {
        done = true;
      }
    } /* while !done */


    /* Not using unitid's use physical targets */
  } else {
    while (!done) {
      level = CAGE;
      uint8_t valid = 1;


      /* We are at the end of the loop, nothing left to loop on, get out of here */
      if (io_state.ecmdCurCage == io_state.ecmdSystemConfigData.cageData.end()) {

#ifndef ECMD_STRIP_DEBUG
        if (ecmdClientDebug >= 8) {
          std::vector< void * > args;
          args.push_back((void*) &io_target);
          args.push_back((void*) &io_state);
          args.push_back((void*) &rc);

          ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t ecmdConfigLooperNext (ecmdChipTarget & io_target, ecmdLooperData& io_state)",args);
        }
#endif
        return rc;
      }

      /* ******** NOTE : The iterators in io_state always point to the next instance to use */
      /*           (the one that should be returned from this function ****************     */

      /* Enter if : */
      /* First time in config looper */
      /* last cage != current cage */
      /* node state changed since last call */
      if (io_state.ecmdLooperInitFlag ||
          io_target.cage != (*io_state.ecmdCurCage).cageId ||
          io_state.prevTarget.nodeState != io_target.nodeState) {

        /* If our cage didn't change from the last loop, but our node state did we may need to force an increment */
        if ((io_target.cage == (*io_state.ecmdCurCage).cageId) &&
            (io_state.prevTarget.nodeState != ECMD_TARGET_FIELD_UNUSED) && (io_target.nodeState == ECMD_TARGET_FIELD_UNUSED)) {
          /* When we called into the plugin they told us that whatever we are doing is not core dependent, so stop looping on it */
          /* Increment the iterators to point to the next target */
          ecmdIncrementLooperIterators(level, io_state);
          /* Restart the process */
          io_state.prevTarget = io_target;
          continue;
        }

        /* Data is valid, let's setup this part of the target */
        io_target.cage = (*io_state.ecmdCurCage).cageId;
        io_state.ecmdCurNode = (*io_state.ecmdCurCage).nodeData.begin();
        valid = 0;
        freshLoop = true;

        if (io_target.nodeState == ECMD_TARGET_FIELD_UNUSED || io_state.ecmdCurNode == (*io_state.ecmdCurCage).nodeData.end()) {
          io_target.node = 0;
        }
        else {
          level = NODE;
        }

      }
      else {
        level = NODE;
      }

      /* Enter if : */
      /* Level == Node (the user is looping with nodes  */
      /* !valid - current node iterator isn't valid */
      /* last node != current node */
      /* slot state changed since last call */
      if (level == NODE &&
          (!valid ||
           io_target.node != (*io_state.ecmdCurNode).nodeId ||
           io_state.prevTarget.slotState != io_target.slotState)) {

        /* If our node didn't change from the last loop, but our slot state did we may need to force an increment */
        if (!freshLoop &&
            (io_target.node == (*io_state.ecmdCurNode).nodeId) &&
            (io_state.prevTarget.slotState != ECMD_TARGET_FIELD_UNUSED) && (io_target.slotState == ECMD_TARGET_FIELD_UNUSED)) {
          /* When we called into the plugin they told us that whatever we are doing is not core dependent, so stop looping on it */
          /* Increment the iterators to point to the next target */
          ecmdIncrementLooperIterators(level, io_state);
          /* Restart the process */
          io_state.prevTarget = io_target;
          continue;
        }

        /* Data is valid, let's setup this part of the target */
        io_target.node = (*io_state.ecmdCurNode).nodeId;
        io_state.ecmdCurSlot = (*io_state.ecmdCurNode).slotData.begin();
        valid = 0;
        freshLoop = true;

        if (io_target.nodeState == ECMD_TARGET_FIELD_UNUSED || io_state.ecmdCurSlot == (*io_state.ecmdCurNode).slotData.end()) {
          io_target.slot = 0;
        }
        else {
          level = SLOT;
        }

      }
      else if (valid) {
        level = SLOT;
      }

      /* Enter if : */
      /* Level == Slot (the user is looping with Slots  */
      /* !valid - current Slot iterator isn't valid */
      /* last Slot != current Slot */
      /* chippos or chiptype state changed since last call */
      if (level == SLOT &&
          (!valid ||
           io_target.slot != (*io_state.ecmdCurSlot).slotId || 
           io_state.prevTarget.chipTypeState != io_target.chipTypeState ||
           io_state.prevTarget.posState != io_target.posState )) {

        /* If our slot  didn't change from the last loop, but our pos/chiptype state did we may need to force an increment */
        if (!freshLoop &&
            (io_target.slot == (*io_state.ecmdCurSlot).slotId) &&
            ((io_state.prevTarget.chipTypeState != ECMD_TARGET_FIELD_UNUSED) && (io_target.chipTypeState == ECMD_TARGET_FIELD_UNUSED) ||
             (io_state.prevTarget.posState != ECMD_TARGET_FIELD_UNUSED) && (io_target.posState == ECMD_TARGET_FIELD_UNUSED))) {
          /* When we called into the plugin they told us that whatever we are doing is not core dependent, so stop looping on it */
          /* Increment the iterators to point to the next target */
          ecmdIncrementLooperIterators(level, io_state);
          /* Restart the process */
          io_state.prevTarget = io_target;
          continue;
        }

        /* Data is valid, let's setup this part of the target */
        io_target.slot = (*io_state.ecmdCurSlot).slotId;
        io_state.ecmdCurChip = (*io_state.ecmdCurSlot).chipData.begin();
        valid = 0;
        freshLoop = true;

        if (io_target.chipTypeState == ECMD_TARGET_FIELD_UNUSED || io_target.posState == ECMD_TARGET_FIELD_UNUSED || io_state.ecmdCurChip == (*io_state.ecmdCurSlot).chipData.end()) {
          io_target.chipType = "";
          io_target.pos = 0;
        }
        else {
          level = CHIP;
        }

      }
      else if (valid) {
        level = CHIP;
      }


      /* Enter if : */
      /* Level == Chip (the user is looping with Chips  */
      /* !valid - current Chip iterator isn't valid */
      /* last ChipType != current ChipType */
      /* last Chip pos != current Chip pos */
      /* Core state changed since last call */
      if (level == CHIP &&
          (!valid ||
           io_target.chipType != (*io_state.ecmdCurChip).chipType ||
           io_target.pos != (*io_state.ecmdCurChip).pos ||
           io_state.prevTarget.coreState != io_target.coreState)) {

        /* If our pos/chiptype didn't change from the last loop, but our core state did we may need to force an increment */
        if (!freshLoop && 
            (io_target.chipType == (*io_state.ecmdCurChip).chipType) &&
            (io_target.pos == (*io_state.ecmdCurChip).pos) &&
            (io_state.prevTarget.coreState != ECMD_TARGET_FIELD_UNUSED) && (io_target.coreState == ECMD_TARGET_FIELD_UNUSED)) {
          /* When we called into the plugin they told us that whatever we are doing is not core dependent, so stop looping on it */
          /* Increment the iterators to point to the next target */
          ecmdIncrementLooperIterators(level, io_state);
          /* Restart the process */
          io_state.prevTarget = io_target;
          continue;
        }

        /* Data is valid, let's setup this part of the target */
        io_target.chipType = (*io_state.ecmdCurChip).chipType;
        io_target.pos = (*io_state.ecmdCurChip).pos;
        io_state.ecmdCurCore = (*io_state.ecmdCurChip).coreData.begin();
        valid = 0;
        freshLoop = true;

        if (io_target.coreState == ECMD_TARGET_FIELD_UNUSED || io_state.ecmdCurCore == (*io_state.ecmdCurChip).coreData.end()) {
          io_target.core = 0;
          io_target.thread = 0;
        }
        else {
          level = CORE;
        }

      }
      else if (valid) {
        level = CORE;
      }

      /* Enter if : */
      /* Level == Core (the user is looping with Cores  */
      /* !valid - current Core iterator isn't valid */
      /* last Core != current Core */
      /* thread state changed since last call */
      if (level == CORE &&
          (!valid ||
           io_target.core != (*io_state.ecmdCurCore).coreId ||
           io_state.prevTarget.threadState != io_target.threadState)) {

        /* If our core didn't change from the last loop, but our thread state did we may need to force an increment */
        if (!freshLoop &&
            (io_target.core == (*io_state.ecmdCurCore).coreId) &&
            (io_state.prevTarget.threadState != ECMD_TARGET_FIELD_UNUSED) && (io_target.threadState == ECMD_TARGET_FIELD_UNUSED)) {
          /* When we called into the plugin they told us that whatever we are doing is not thread dependent, so stop looping on it */
          /* Increment the iterators to point to the next target */
          ecmdIncrementLooperIterators(level, io_state);
          /* Restart the process */
          io_state.prevTarget = io_target;
          continue;
        }

        /* Data is valid, let's setup this part of the target */
        io_target.core = (*io_state.ecmdCurCore).coreId;
        io_state.ecmdCurThread = (*io_state.ecmdCurCore).threadData.begin();
        valid = 0;
        freshLoop = true;

        if (io_target.threadState == ECMD_TARGET_FIELD_UNUSED || io_state.ecmdCurThread == (*io_state.ecmdCurCore).threadData.end()) {
          io_target.thread = 0;
        }
        else {
          level = THREAD;
        }

      }
      else if (valid) {
        level = THREAD;
      }

      /* Enter if : */
      /* Level == Thread (the user is looping with Threads  */
      /* !valid - current Thread iterator isn't valid */
      /* last Thread != current Thread */
      if (level == THREAD && (!valid || io_target.thread != (*io_state.ecmdCurThread).threadId)) {

        /* Data is valid, let's setup this part of the target */
        io_target.thread = (*io_state.ecmdCurThread).threadId;

      }

      /* We got here, must be done */
      done = true;

    } /* End while */

    /* Increment the iterators to point to the next target */
    ecmdIncrementLooperIterators(level, io_state);

  } /* end phys/unitid if */

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug > 1) {
    std::string printed = "ECMD DEBUG (ecmdConfigLooperNext) : Found next Target : " + ecmdWriteTarget(io_target); printed += "\n";
    ecmdOutput(printed.c_str());
  }
#endif


  if (io_state.ecmdLooperInitFlag) {
    io_state.ecmdLooperInitFlag = false;
  }
  /* Store away the target */
  io_state.prevTarget = io_target;

  rc =1;
#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug >= 8) {
    std::vector< void * > args;
    args.push_back((void*) &io_target);
    args.push_back((void*) &io_state);
    args.push_back((void*) &rc);

    ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t ecmdConfigLooperNext (ecmdChipTarget & io_target, ecmdLooperData& io_state)",args);
  }
#endif

  return rc;

}

void ecmdIncrementLooperIterators (uint8_t level, ecmdLooperData& io_state) {
  /* Let's start incrementing our lowest pointer so it points to the next object for the subsequent call to this function */
  const uint8_t CAGE = 0;
  const uint8_t NODE = 1;
  const uint8_t SLOT = 2;
  const uint8_t CHIP = 3;
  const uint8_t CORE = 4;
  const uint8_t THREAD = 5;

  switch (level) {

    case THREAD:  //thread
      io_state.ecmdCurThread++;
      /* Did we find another thread, if not we will try core */
      if (io_state.ecmdCurThread != (*io_state.ecmdCurCore).threadData.end()) {
        break;
      }

    case CORE:  //core
      io_state.ecmdCurCore++;
      /* Did we find another core, if not we will try chip */
      if (io_state.ecmdCurCore != (*io_state.ecmdCurChip).coreData.end()) {
        break;
      }

    case CHIP:  //chip
      io_state.ecmdCurChip++;
      /* Did we find another chip, if not we will try slot */
      if (io_state.ecmdCurChip != (*io_state.ecmdCurSlot).chipData.end()) {
        break;
      }

    case SLOT:  //slot
      io_state.ecmdCurSlot++;
      /* Did we find another slot, if not we will try node */
      if (io_state.ecmdCurSlot != (*io_state.ecmdCurNode).slotData.end()) {
        break;
      }

    case NODE:  //node
      io_state.ecmdCurNode++;
      /* Did we find another node, if not we will try cage */
      if (io_state.ecmdCurNode != (*io_state.ecmdCurCage).nodeData.end()) {
        break;
      }

    case CAGE:  //cage
      io_state.ecmdCurCage++;
      break;

    default:
      //shouldn't get here
      break;
  }
}


uint32_t ecmdReadDataFormatted (ecmdDataBuffer & o_data, const char * i_dataStr, std::string i_format, int i_expectedLength) {
  uint32_t rc = ECMD_SUCCESS;

  std::string localFormat = i_format;
  int bitlength;

  //ignore leading 'p'- it's for perl stuff
  if (localFormat[0] == 'p') {
    localFormat = localFormat.substr(1, localFormat.size());
  }

  if (localFormat == "x" || localFormat == "xl") {
    bitlength = strlen(i_dataStr) * 4;
    if (i_expectedLength != 0) bitlength = i_expectedLength;
    o_data.setBitLength(bitlength);
    rc = o_data.insertFromHexLeft(i_dataStr, 0, bitlength);
  }
  else if (localFormat == "xr") {
    bitlength = strlen(i_dataStr) * 4;
    if (i_expectedLength != 0) bitlength = i_expectedLength;
    o_data.setBitLength(bitlength);
    rc = o_data.insertFromHexRight(i_dataStr, 0, bitlength);
  }     
  else if (localFormat == "b") {
    bitlength = strlen(i_dataStr);
    if (i_expectedLength != 0) bitlength = i_expectedLength;
    o_data.setBitLength(bitlength);
    rc = o_data.insertFromBin(i_dataStr);
  }
  else if (localFormat == "bX") {
    bitlength = strlen(i_dataStr);
    if (i_expectedLength != 0) bitlength = i_expectedLength;
    o_data.setBitLength(bitlength);
    o_data.setXstate(0,i_dataStr);
  }
  else if (localFormat == "d") {
    if(strlen(i_dataStr) > 10) {
     ecmdOutputError( "Integer overflow. Decimal number should be less that 4G.\n" );
     rc = ECMD_INVALID_ARGS;
    }
    else if(strlen(i_dataStr) == 10) {
      //Highest number for 32 bits is 4294967295
      if( strcmp(i_dataStr, "4294967295") > 0 ) {
        ecmdOutputError( "Integer overflow. Decimal number should be less that 4G.\n" );
        rc = ECMD_INVALID_ARGS;
      }
    }
    if(!rc) {
     uint32_t decdata = decToUInt32(i_dataStr);
     bitlength = 32;
     if (i_expectedLength != 0) bitlength = i_expectedLength;
     /* Put it in the whole word and then shift, as decimal data is assumed to be right aligned */
     o_data.setBitLength(bitlength);
     rc = o_data.insertFromRight(decdata, 0, bitlength);
    }
  }   
  else {
    ecmdOutputError( ("Did not recognize input format string " + localFormat + "\n").c_str() );
    rc = ECMD_INVALID_ARGS;
  }
  if (rc == ECMD_DBUF_INVALID_DATA_FORMAT) {
    ecmdOutputError(( "Input Data contained some invalid characters for format specified : '" + localFormat + "'!\n").c_str());
  }

  return rc;
}

uint32_t decToUInt32(const char *decstr) {
    uint32_t decdata;
    
    decdata=0;
    for(int i=0; decstr[i] >= '0' && decstr[i] <= '9';++i) {
     decdata = 10 * decdata + (decstr[i] - '0');
    }
    return decdata;
}
std::string ecmdWriteDataFormatted (ecmdDataBuffer & i_data, std::string i_format, uint64_t address) {
  std::string printed;
  int formTagLen = i_format.length();
  ecmdFormatState_t curState = ECMD_FORMAT_NONE;
  int numCols = 0;
  bool good = true;

  // We have to check for the memory types before we fall into the looping below
  if (i_format.substr(0,3) == "mem") {
    if (i_format[3] == 'a')
      curState = ECMD_FORMAT_MEMA;
    else if (i_format[3] == 'd')
      curState = ECMD_FORMAT_MEMD;
    else if (i_format[3] == 'e')
      curState = ECMD_FORMAT_MEME;
    else
      curState = ECMD_FORMAT_MEM;
  } else { // No mem
    for (int i = 0; i < formTagLen; i++) {

      if (i_format[i] == 'x') {
        if (curState != ECMD_FORMAT_NONE) {
          good = false;
          break;
        }

        curState = ECMD_FORMAT_X;
      }
      else if (i_format[i] == 'l') {
        if (curState != ECMD_FORMAT_X) {
          good = false;
          break;
        }

      }
      else if (i_format[i] == 'r') {
        if (curState != ECMD_FORMAT_X) {
          good = false;
          break;
        }

        curState = ECMD_FORMAT_XR;
      }
      else if (i_format[i] == 'b') {
        if (curState != ECMD_FORMAT_NONE) {
          good = false;
          break;
        }

        curState = ECMD_FORMAT_B;
      }
      else if (i_format[i] == 'n') {
        if (curState == ECMD_FORMAT_B) {
          curState = ECMD_FORMAT_BN;
        } else if (curState == ECMD_FORMAT_BX) {
          curState = ECMD_FORMAT_BXN;
        } else {
          good = false;
          break;
        }

      }
      else if (i_format[i] == 'w') {
        if (curState == ECMD_FORMAT_X) {
          curState = ECMD_FORMAT_XW;
        }
        else if (curState == ECMD_FORMAT_XR) {
          curState = ECMD_FORMAT_XRW;
        }
        else if (curState == ECMD_FORMAT_B) {
          curState = ECMD_FORMAT_BW;
        }
        else if (curState == ECMD_FORMAT_BX) {
          curState = ECMD_FORMAT_BXW;
        }
        else {
          good = false;
          break;
        }

      }
      else if (i_format[i] == 'X') {
        if (curState != ECMD_FORMAT_B) {
          good = false;
          break;
        }

        curState = ECMD_FORMAT_BX;
      }
      else if (isdigit(i_format[i])) {
        numCols *= 10;
        numCols += atoi(&i_format[i]);
        break;
      }
      else {
        good = false;
        break;
      }

    }
  }

  if (curState == ECMD_FORMAT_NONE) {
    good = false;
  }

  if (!good) {
    printed = "Unrecognized format string: ";
    printed += i_format + "\n";
    ecmdOutputError(printed.c_str());
    printed = "";
    return printed;
  }

  if (curState == ECMD_FORMAT_X) {
    printed = "0x" + i_data.genHexLeftStr();
    printed += "\n";
  }
  else if (curState == ECMD_FORMAT_XR) {
    printed = "0xr" + i_data.genHexRightStr();
    printed += "\n";
  }
  else if (curState == ECMD_FORMAT_B) {
    printed = "0b" + i_data.genBinStr();
    printed += "\n";
  }
  else if (curState == ECMD_FORMAT_BX) {
    //do something
    printed = "0b" + i_data.genXstateStr();
    printed += "\n";
  }
  else if (curState == ECMD_FORMAT_MEM || curState == ECMD_FORMAT_MEMA || curState == ECMD_FORMAT_MEME) {
    int myAddr = address;
    uint32_t wordsDone = 0;
    std::string lastBytes;
    char tempstr[400];
    uint32_t wordsDonePrev;
    int numLastBytes=0;
    int i=0;
    
    //if not exact word multiple
    if ( (i_data.getWordLength()*4) > (i_data.getByteLength()) ) {
      numLastBytes = 4 - ((i_data.getWordLength()*4) - (i_data.getByteLength()));
    }
    // Loop through the complete 4 word blocks
    while ((i_data.getWordLength() - wordsDone) > 3) {
      wordsDonePrev = wordsDone;
      //last word
      if ( (i_data.getWordLength() == (wordsDone+4)) && (numLastBytes != 0)) {
        lastBytes = i_data.genHexLeftStr(((wordsDone+3)*32), (numLastBytes*8));
        sprintf(tempstr,"%016X: %08X %08X %08X %s", myAddr, i_data.getWord(wordsDone), i_data.getWord(wordsDone+1), i_data.getWord(wordsDone+2), lastBytes.c_str());
	i=0;
	while (i < (4-numLastBytes)) {
	  strcat(tempstr, "  "); i++;
	}
      }
      else {
        sprintf(tempstr,"%016X: %08X %08X %08X %08X", myAddr, i_data.getWord(wordsDone), i_data.getWord(wordsDone+1), i_data.getWord(wordsDone+2), i_data.getWord(wordsDone+3));
      }
      printed += tempstr;
      // Text printing additions
      if (curState == ECMD_FORMAT_MEMA || curState == ECMD_FORMAT_MEME) {
        // ASCII
        if (curState == ECMD_FORMAT_MEMA) {
          sprintf(tempstr,"   [%s]",i_data.genAsciiStr(wordsDonePrev*32, 128).c_str());
        }
        // EBCDIC
        if (curState == ECMD_FORMAT_MEME) {
	  if ( (i_data.getWordLength() == (wordsDone+4)) && (numLastBytes != 0)) {
	    sprintf(tempstr,"   [%s]",ecmdGenEbcdic(i_data,wordsDonePrev*32, (128-(8*(4-numLastBytes)))).c_str());
	  }
	  else {
            sprintf(tempstr,"   [%s]",ecmdGenEbcdic(i_data,wordsDonePrev*32, 128).c_str());
	  }
        }
	printed += tempstr;
      }
      printed += "\n";
      myAddr += 16;
      wordsDone += 4;
    }
    // Done with all the 4 word blocks, see if we've got some straglers left
    if ((i_data.getWordLength() - wordsDone) != 0) {
      wordsDonePrev = wordsDone;
      // Print the address
      sprintf(tempstr,"%016X:", myAddr);
      printed += tempstr;
      // Now throw on the words
      while ((uint32_t) wordsDone < i_data.getWordLength()) {
        //last word
        if ( (i_data.getWordLength() == (wordsDone+1)) && (numLastBytes != 0)) {
	  lastBytes = i_data.genHexLeftStr((wordsDone*32), (numLastBytes*8));
          sprintf(tempstr," %s", lastBytes.c_str());
	  i=0;
	  while (i < (4-numLastBytes)) {
	   strcat(tempstr, "  "); i++;
	  }
	  wordsDone++;
        }
	else {
          sprintf(tempstr," %08X",i_data.getWord(wordsDone++));
	}
        printed += tempstr;
      }
      // Text printing additions
      if (curState == ECMD_FORMAT_MEMA || curState == ECMD_FORMAT_MEME) {
        // Insert spaces from the end of the last incomplete line for alignment
        for (uint32_t y = 0; y < (4 - (wordsDone - wordsDonePrev)); y++) {
          printed.insert(printed.length(),"         ");
        }
        // ASCII
        if (curState == ECMD_FORMAT_MEMA) {
          sprintf(tempstr,"   [%s]",i_data.genAsciiStr(wordsDonePrev*32, (wordsDone - wordsDonePrev)*32).c_str());
        }
        // EBCDIC
        if (curState == ECMD_FORMAT_MEME) {
	  if (numLastBytes) {
	    sprintf(tempstr,"   [%s]",ecmdGenEbcdic(i_data,wordsDonePrev*32, (((wordsDone - wordsDonePrev)*32) - ((4-numLastBytes)*8))).c_str());
	  }
	  else {
            sprintf(tempstr,"   [%s]",ecmdGenEbcdic(i_data,wordsDonePrev*32, (wordsDone - wordsDonePrev)*32).c_str());
	  }
        }
        printed += tempstr;
      }
      printed += "\n";
    }
  }
  else if (curState == ECMD_FORMAT_MEMD) {
    int myAddr = address;
/*    int wordsDone = 0; */
    char tempstr[400];
    int y=0;
    
    
    //if not exact word multiple-To be used for printing in the byte format
    /*int numLastBytes=0;
    std::string lastBytes;
    if ( (i_data.getWordLength()*4) > (i_data.getByteLength()) ) {
      numLastBytes = 4 - ((i_data.getWordLength()*4) - (i_data.getByteLength()));
    }*/
    
    // Print out the data
    for (uint32_t x = 0; x < (i_data.getWordLength() / 2); x++) {
      //To be used for printing in the byte format
      /*if ( ((i_data.getWordLength()) == y+2) && (numLastBytes != 0)) {
        lastBytes = i_data.genHexLeftStr(((y+1)*32), (numLastBytes*8));
        sprintf(tempstr,"D %016X: %08X%s\n", myAddr, i_data.getWord(y++), lastBytes.c_str());
      }
      else {
        sprintf(tempstr,"D %016X: %08X%08X\n", myAddr, i_data.getWord(y++), i_data.getWord(y++));
      }*/
      sprintf(tempstr,"D %016X: %08X%08X\n", myAddr, i_data.getWord(y++), i_data.getWord(y++));
      printed += tempstr;
      myAddr += 8;
    }

    if (i_data.getWordLength() % 2) {
      //To be used for printing in the byte format
      /*if ( numLastBytes != 0) {
        lastBytes = i_data.genHexLeftStr(((i_data.getWordLength() - 1)*32), (numLastBytes*8));
	sprintf(tempstr,"D %016X: %s\n", myAddr, lastBytes.c_str());
      }
      else {
        sprintf(tempstr,"D %016X: %08X00000000\n", myAddr, i_data.getWord((i_data.getWordLength() - 1)));
      }*/
      sprintf(tempstr,"D %016X: %08X00000000\n", myAddr, i_data.getWord((i_data.getWordLength() - 1)));
      printed += tempstr;
    }
  }
  else {

    char * outstr = new char[40];
    int curCol = 0;
    int colCount = 0;
    int blockSize = 32;
    if ((curState == ECMD_FORMAT_BN) || (curState == ECMD_FORMAT_BXN))  blockSize = 4;
    int curOffset = 0;
    int numBlocks = i_data.getBitLength() % blockSize ? i_data.getBitLength() / blockSize + 1: i_data.getBitLength() / blockSize;
    int dataBitLength = i_data.getBitLength();

    if (numCols) {

      if (curState == ECMD_FORMAT_BN || curState == ECMD_FORMAT_BW || curState == ECMD_FORMAT_B || curState == ECMD_FORMAT_BXN || curState == ECMD_FORMAT_BXW) {
        printed += "\n";
        printed += ecmdBitsHeader(5, blockSize, numCols, dataBitLength);
      } 

      printed += "\n000: ";
    }

    for (int i = 0; i < numBlocks; i++) {

      if (curState == ECMD_FORMAT_XW) {
        printed += i_data.genHexLeftStr(curOffset, blockSize < (dataBitLength - curOffset) ? blockSize : (dataBitLength - curOffset));
      }
      else if (curState == ECMD_FORMAT_XRW) {
        printed += i_data.genHexRightStr(curOffset, blockSize < (dataBitLength - curOffset) ? blockSize : (dataBitLength - curOffset));
      }
      else if ((curState == ECMD_FORMAT_BX) || (curState == ECMD_FORMAT_BXN) || (curState == ECMD_FORMAT_BXW))  {
        printed +=  i_data.genXstateStr(curOffset, blockSize < (dataBitLength - curOffset) ? blockSize : (dataBitLength - curOffset));
      }
      else {
        printed += i_data.genBinStr(curOffset, blockSize < (dataBitLength - curOffset) ? blockSize : (dataBitLength - curOffset));
      }

      colCount++;
      if (numCols && colCount == numCols && (i != numBlocks-1) ) {
        curCol += numCols;

        sprintf(outstr, "\n%3.3d: ", curCol);

        printed += outstr;
        colCount = 0;
      }
      else {
        printed += " ";
      }

      curOffset += blockSize;
    }

    delete outstr;
    outstr = NULL;
    printed += "\n";
  }

  // Tack this onto the end of anything returned
  //printed += "\n";
    
  return printed;

}

std::string ecmdBitsHeader(int initCharOffset, int blockSize, int numCols, int i_maxBitWidth) {

  std::string topLine(initCharOffset, ' ');
  std::string bottomLine(initCharOffset, ' ');

  int bitsToPrint = blockSize * numCols < i_maxBitWidth ? blockSize * numCols : i_maxBitWidth;
  int numSpaces = numCols - 1;
  char curNum[2];
  int blockCount = 0;
  int lineCount = 0;
  int topLineTrack = -1;

  for (int i = 0; i < bitsToPrint + numSpaces; i++) {

    if (blockCount == blockSize) {
      topLine += " ";
      bottomLine += " ";
      blockCount = 0;
    }
    else {

      int topLineCount = lineCount / 10;
      if (topLineCount != topLineTrack) {
        sprintf(curNum, "%d", topLineCount);
        topLine += curNum;
        topLineTrack = topLineCount;
      }
      else {
        topLine += " ";
      }

      sprintf(curNum, "%d", lineCount % 10);
      bottomLine += curNum;

      lineCount++;
      blockCount++;

      /* Let's see if we are done with data */
      if (lineCount >= i_maxBitWidth) break;
    }

  }

  return topLine + "\n" + bottomLine;
}


std::string ecmdWriteTarget (ecmdChipTarget & i_target) {

  std::string printed;
  char util[7];

  if (i_target.chipTypeState != ECMD_TARGET_FIELD_UNUSED) {
    printed = i_target.chipType + "\t";
  }

  //always do cage
  sprintf(util, "k%d", i_target.cage);
  printed += util;

  if (i_target.nodeState != ECMD_TARGET_FIELD_UNUSED) {
    sprintf(util, ":n%d", i_target.node);
    printed += util;

    if (i_target.slotState != ECMD_TARGET_FIELD_UNUSED) {
      sprintf(util, ":s%d", i_target.slot);
      printed += util;


      if ((i_target.posState != ECMD_TARGET_FIELD_UNUSED) && (i_target.chipTypeState != ECMD_TARGET_FIELD_UNUSED)) {

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

    } //slot
    else {
      printed += "             ";  //adjust spacing
    }

  } //node

  //set a space between the target info and the data
  printed += " "; 

  return printed;

}

uint32_t ecmdGetChipData (ecmdChipTarget & i_target, ecmdChipData & o_data) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdChipTarget tmp = i_target;
  ecmdQueryData needlesslySlow;
  tmp.cageState = tmp.nodeState = tmp.slotState = tmp.chipTypeState = tmp.posState = ECMD_TARGET_QUERY_FIELD_VALID;
  tmp.coreState = tmp.threadState = ECMD_TARGET_QUERY_IGNORE;
  rc = ecmdQueryConfig(tmp, needlesslySlow, ECMD_QUERY_DETAIL_HIGH);
  if (rc) return rc;

  if (needlesslySlow.cageData.empty()) return ECMD_TARGET_NOT_CONFIGURED;
  if (needlesslySlow.cageData.front().cageId != i_target.cage) return ECMD_TARGET_NOT_CONFIGURED;
  if (needlesslySlow.cageData.front().nodeData.empty()) return ECMD_TARGET_NOT_CONFIGURED;
  if (needlesslySlow.cageData.front().nodeData.front().nodeId != i_target.node) return ECMD_TARGET_NOT_CONFIGURED;
  if (needlesslySlow.cageData.front().nodeData.front().slotData.empty()) return ECMD_TARGET_NOT_CONFIGURED;
  if (needlesslySlow.cageData.front().nodeData.front().slotData.front().slotId != i_target.slot) return ECMD_TARGET_NOT_CONFIGURED;
  if (needlesslySlow.cageData.front().nodeData.front().slotData.front().chipData.empty()) return ECMD_TARGET_NOT_CONFIGURED;

  o_data = needlesslySlow.cageData.front().nodeData.front().slotData.front().chipData.front();
  if ((o_data.chipType != i_target.chipType && o_data.chipCommonType != i_target.chipType && o_data.chipShortType != i_target.chipType) || o_data.pos != i_target.pos) {
    return ECMD_TARGET_NOT_CONFIGURED;
  }
    
  return rc;
}


uint32_t ecmdDisplayDllInfo() {

  uint32_t rc = ECMD_SUCCESS;
  std::string printed;

  /* Let's display the dllInfo to the user */
  ecmdDllInfo info;
  rc = ecmdQueryDllInfo(info);
  if (rc) {
    ecmdOutputError("ecmdDisplayDllInfo - Problems occurred trying to get Dll Info\n");
    return rc;
  }
  ecmdOutput("================================================\n");
  printed = "Dll Type         : ";
  if (info.dllType == ECMD_DLL_STUB)
    printed += "Stub\n";
  else if (info.dllType == ECMD_DLL_STUB)
    printed += "Stub\n";
  else if (info.dllType == ECMD_DLL_CRONUS)
    printed += "Cronus\n";
  else if (info.dllType == ECMD_DLL_IPSERIES)
    printed += "IP-Series\n";
  else if (info.dllType == ECMD_DLL_ZSERIES)
    printed += "Z-Series\n";
  else if (info.dllType == ECMD_DLL_SCAND)
    printed += "ScanD\n";
  else 
    printed = "Unknown\n";
  ecmdOutput(printed.c_str());

  printed = "Dll Product      : ";
  if (info.dllProduct == ECMD_DLL_PRODUCT_ECLIPZ)
    printed += "Eclipz\n";
  else
    printed += "Unknown\n";
  ecmdOutput(printed.c_str());

  printed = "Dll Product Type : ";
  printed += info.dllProductType + "\n";
  ecmdOutput(printed.c_str());

  printed = "Dll Environment  : ";
  if (info.dllEnv == ECMD_DLL_ENV_HW)
    printed += "Hardware\n";
  else
    printed += "Simulation\n";
  ecmdOutput(printed.c_str());

  printed = "Dll Build Date   : "; printed += info.dllBuildDate; printed += "\n"; ecmdOutput(printed.c_str());
  printed = "Dll Capi Version : "; printed += info.dllCapiVersion; printed += "\n"; ecmdOutput(printed.c_str());
  printed = "Dll Build Info   : "; printed += info.dllBuildInfo; printed += "\n"; ecmdOutput(printed.c_str());
  ecmdOutput("================================================\n");


  return rc;

}



#ifndef ECMD_STRIP_DEBUG
/**********************************************************************************/
/**********************************************************************************/
/*                                                                                */
/* efppInOut = an enum that tells me if we are working in a before or after mode  */
/*             ECMD_FPP_FUNCTIONIN is intended to be the first thing done at the  */
/*             top of a function.                                                 */
/*             ECMD_FPP_FUNCTIONOUT is the last thing done.                       */
/*                                                                                */
/* fprototypeStr = a cut and paste string of the actual function prototype        */
/*                                                                                */
/* other parameters must be handled on a case by case basis based on the parsed   */
/*                  value within fprototypeStr.                                   */
/*                                                                                */
/*                                                                                */
/**********************************************************************************/
/* if you can think of a better way to detect and handle parameter types let me   */
/* know.  I think this method allows us to handle structures in a convinient way. */
/*                                                                                */
/* FLEXIBILITY and EXPANDABILITY                                                  */
/*                                                                                */
/**********************************************************************************/
/**********************************************************************************/
void ecmdFunctionParmPrinter(int tCount, efppInOut_t inOut, const char * fprototypeStr, std::vector < void * > args) {
/* declare variables */
  int looper, looper2;
  std::vector<std::string> tokens;
  std::vector<std::string> parmTokens;
  std::vector<std::string> parmEntryTokens;
  std::vector<std::string> fReturnType;

  char variableType[100]; /* this will be something like "char*" or "BIT32" */
  std::vector<std::string> variableName; /* this will be something like "fprototypeStr" no * or & in this */
  std::string printed;

  int mysize;
  int look4rc,outputRC,dataLooper;

  char tempIntStr[400];


  look4rc = outputRC = 0;
/* validate the type of call we are doing, return if invalid */
  if(inOut == ECMD_FPP_FUNCTIONIN) {
    look4rc =0;
    sprintf(frontFPPTxt,"ECMD DEBUG (ecmdFPP) : ENTER(%03d) : ",tCount);
  } else if (inOut == ECMD_FPP_FUNCTIONOUT) {
    look4rc =1;
    sprintf(frontFPPTxt,"ECMD DEBUG (ecmdFPP) : EXIT (%03d) : ",tCount);
  } else {
    printed = "ECMD DEBUG (ecmdFPP) : ERROR::ecmdFunctionParmPrinter  Invalid Enum type on function call.\n";
    ecmdOutput(printed.c_str());
    return;
  }


/* print the original function prototype */
  printed = frontFPPTxt;
  printed += "Function prototype: ";
  printed += fprototypeStr;
  printed += "\n";
  ecmdOutput(printed.c_str());




/* parse the parameters */
  ecmdParseTokens(fprototypeStr, "()", tokens); /* this chops off the leading junk */
/* example: */
/* tokens[0] = "void ecmdFunctionParmPrinter"             */
/* tokens[1] = "enum efppInOut, char *fprototypeStr, ..." */
/* tokens[2] = " {"                                       */
/* tokens.size() = 3                                      */

  ecmdParseTokens(tokens[0].c_str(), " ", fReturnType); /* this tokenizes the function name and return type */
/* example: */
/* fReturnType[0] = "void"             */
/* fReturnType[1] = "ecmdFunctionParmPrinter" */
/* fReturnType.size() = 2                                      */

  if(look4rc) {
    if((!strcmp(fReturnType[0].c_str(),"void")) && (look4rc)) {
      /* it's a void return so don't do return code printing. */
      outputRC =0;
    } else {
      outputRC =1;
    }
  }  

  if(tokens.size() >1) {
    ecmdParseTokens(tokens[1].c_str(), ",", parmTokens); /* this tokenizes the meat and potatoes */
  }
/* example: */
/* parmTokens[0] = "enum efppInOut"       */
/* parmTokens[1] = " char *fprototypeStr" */
/* parmTokens[2] = " ..."                 */
/* parmTokens.size90 = 3                    */

/* exit if we are on a functionin and debug level 8 */
  if((ecmdClientDebug == 8) && (inOut == ECMD_FPP_FUNCTIONIN)) {
    return;
  }

/* exit if we are on a functionOut, debug level 8 and no return code */
  if((ecmdClientDebug == 8) && (inOut == ECMD_FPP_FUNCTIONOUT) && (outputRC ==0)) {
    return;
  }


/* exit if we are on a functionOut, debug level 8 and no return code */
  if((ecmdClientDebug == 8) && (inOut == ECMD_FPP_FUNCTIONOUT) && (outputRC ==1)) {

/* go check return code to see if it's zero or not. */
    if((!strcmp(fReturnType[0].c_str(),"uint32_t")) && (look4rc)) {
      uint32_t* dummy = (uint32_t*)(args[parmTokens.size()]);
      if (*dummy ==0) {

        /* print the splat line to show it's the end of the exit */
        printed = frontFPPTxt;
        printed += "\t ***************************************\n";
        ecmdOutput(printed.c_str());

        return;  /* normal return code so just exit */
      }
    } else {
    /* if return type is something other than uint32_t then we should probably return since it would not */
    /* match the intended meening of the debug8 exit plan */
      /* print the splat line to show it's the end of the exit */
      printed = frontFPPTxt;
      printed += "\t ***************************************\n";
      ecmdOutput(printed.c_str());

      return;
    }
  }



/* remember the leading and trailing spaces, they could mess up a compare */

/* we need to handle each one on it's own, and make sure the types match the string provided */

  printed = frontFPPTxt;
  printed += "\t ***************************************\n";
  ecmdOutput(printed.c_str());

  mysize = parmTokens.size();

  for(looper =0; (uint32_t) looper < parmTokens.size() + outputRC; looper++) {

    if(((uint32_t)looper == parmTokens.size()) && (outputRC ==1)) {
      /* we are on the last parameter we need to say the parm is a return code. */

      strcpy(variableType,fReturnType[0].c_str());
      variableName.clear();
      variableName.push_back("RETURN CODE");
      
    } else {

      ecmdParseTokens(parmTokens[looper].c_str(), " ", parmEntryTokens);
      /* example: */
      /* parmEntryTokens[0] = "enum"      */
      /* parmEntryTokens[1] = "efppInOut" */
      /* parmEntryTokens.size() = 2       */

      /* rules of engagement: */
      /* must have at least 2 entries, */
      /* last entry is variable name */
      /* last entry could have leading * or & character so strip it and cat it on to the previous string for consistancy */

      mysize = parmEntryTokens.size();

      if(mysize < 2) {
        printed = frontFPPTxt;
        printed += "ERROR::ecmdFunctionParmPrinter  We have an invalid parameter for parm entry # ";
        sprintf(tempIntStr,"%d",looper);
        printed += tempIntStr;
        printed += "\n";
        ecmdOutput(printed.c_str());

        continue;
      }

      strcpy(variableType,"");

      for(looper2=0; looper2 < (mysize -1); looper2++) {
        /* used -1 because we don't want the variable name yet */
        if(looper2 !=0) strcat(variableType," ");
        strcat(variableType,parmEntryTokens[looper2].c_str());
      }

      if(parmEntryTokens[mysize-1].c_str()[0] == '*') {
        strcat(variableType,"*");
        /* handle the "char **name" oddities too */
        if(parmEntryTokens[mysize-1].c_str()[1] == '*') strcat(variableType,"*");
      } else if(parmEntryTokens[mysize-1].c_str()[0] == '&') {
        strcat(variableType,"&");
      }

      ecmdParseTokens(parmEntryTokens[mysize-1].c_str(), "*&", variableName);
    }

    
/****************************************/
/* at this point variableType is somehting like : "enum" or "char*" or "const char*" */
/* variableName[0] is the variable name */

/* we need to strcmp on variableType and then declare an object of that type. it will go away */
/* when it falls out of scope */

    if((variableType         == NULL) ||
       (strlen(variableType) == 0   )   ){
      printed = frontFPPTxt;
      printed += "ERROR::ecmdFunctionParmPrinter  variableType parsing messed up big time for parm number ";
      sprintf(tempIntStr,"%d",looper);
      printed += tempIntStr;
      printed += "\n";
      ecmdOutput(printed.c_str());
      continue;
    }


    if(!strcmp(variableType,"char")) {
/* char */
      char * dummy = (char *)(args[looper]);

      printed = frontFPPTxt;
      printed += "\t type : ";
      printed += variableType;
      printed += " : variable name : ";
      printed += variableName[0];
      printed += " = ";
      sprintf(tempIntStr,"%c",*dummy);
      printed += tempIntStr;
      printed += "\n";
      ecmdOutput(printed.c_str());

    } else if((!strcmp(variableType,"const char *"))||
              (!strcmp(variableType,"const char*"))   ){
/* const char * */

      char ** dummy = (char **)(args[looper]);

      printed = frontFPPTxt;
      printed += "\t type : ";
      printed += variableType;
      printed += " : variable name : ";
      printed += variableName[0];
      printed += "\n";
      ecmdOutput(printed.c_str());

      printed = frontFPPTxt;
      printed += "\t \t value : ";
      if (*dummy == NULL) printed += "NULL";
      else printed += *dummy;
      printed += "\n";
      printed += frontFPPTxt;
      printed += "\t ***************************************\n";
      ecmdOutput(printed.c_str());

    } else if((!strcmp(variableType,"std::string"))       ||
              (!strcmp(variableType,"const std::string")) ||
              (!strcmp(variableType,"std::string&"))      ||
              (!strcmp(variableType,"std::string &"))       ){
/* std::string */

      std::string* dummy = (std::string*)(args[looper]);
      printed = frontFPPTxt;
      printed += "\t type : ";
      printed += variableType;
      printed += " : variable name : ";
      printed += variableName[0];
      printed += " = ";
      printed += dummy->c_str();
      printed += "\n";
      printed += frontFPPTxt;
      printed += "\t ***************************************\n";
      ecmdOutput(printed.c_str());
    } else if((!strcmp(variableType,"std::list<ecmdRingData> &"))   ||
              (!strcmp(variableType,"std::list<ecmdArrayEntry> &")) ||
              (!strcmp(variableType,"std::list<ecmdNameEntry> &"))  ||
              (!strcmp(variableType,"std::list<ecmdIndexEntry> &"))  ||
              (!strcmp(variableType,"std::list <ecmdNameVectorEntry> &"))   ){
/* std::list<something> & */
      printed = frontFPPTxt;
      printed += "\t type : ";
      printed += variableType;
      printed += " : variable name : ";
      printed += variableName[0];
      printed += " = ";
      printed += "placeholder for std::list dump\n";
      printed += frontFPPTxt;
      printed += "\t ***************************************\n";
      ecmdOutput(printed.c_str());



    } else if (!strcmp(variableType,"std::list<ecmdLatchEntry> &")) {
      std::list<ecmdLatchEntry>* dummy = (std::list<ecmdLatchEntry>*)(args[looper]);

      sprintf(tempIntStr, "%s\t type : %s : variable name : %s\n",frontFPPTxt, variableType, variableName[0].c_str()); ecmdOutput(tempIntStr);
      dataLooper = 0;
      for (std::list<ecmdLatchEntry>::iterator entit = dummy->begin(); entit != dummy->end(); entit ++) {
        sprintf(tempIntStr,"%s\t \t entry : %d\n",frontFPPTxt, dataLooper++); ecmdOutput(tempIntStr);
        sprintf(tempIntStr,"%s\t \t \t value : std::string    latchName  = %s\n",frontFPPTxt, entit->latchName.c_str()); ecmdOutput(tempIntStr);
        sprintf(tempIntStr,"%s\t \t \t value : std::string    ringName   = %s\n",frontFPPTxt, entit->ringName.c_str()); ecmdOutput(tempIntStr);
        printEcmdDataBuffer("ecmdDataBuffer", "buffer", entit->buffer, "\t\t ");
        sprintf(tempIntStr,"%s\t \t \t value : int        latchStartBit  = %d\n",frontFPPTxt, entit->latchStartBit); ecmdOutput(tempIntStr);
        sprintf(tempIntStr,"%s\t \t \t value : int          latchEndBit  = %d\n",frontFPPTxt, entit->latchEndBit); ecmdOutput(tempIntStr);
        sprintf(tempIntStr,"%s\t \t \t value : uint32_t              rc  = %X\n",frontFPPTxt, entit->rc); ecmdOutput(tempIntStr);
      }

      printed = "\n";
      printed += frontFPPTxt;
      printed += "\t ***************************************\n";
      ecmdOutput(printed.c_str());

    } else if(!strcmp(variableType,"std::vector <ecmdDataBuffer> &")) {
/* std::vector <ecmdDataBuffer> & */
      printed = frontFPPTxt;
      printed += "\t type : ";
      printed += variableType;
      printed += " : variable name : ";
      printed += variableName[0];
      printed += " = ";
      printed += "placeholder for std::vector dump\n";
      printed += frontFPPTxt;
      printed += "\t ***************************************\n";
      ecmdOutput(printed.c_str());

    } else if(!strcmp(variableType,"uint32_t") ||
              !strcmp(variableType,"uint32_t &") ||
              !strcmp(variableType,"uint32_t&") ) {
/* uint32_t */

      uint32_t* dummy = (uint32_t*)(args[looper]);
      printed = frontFPPTxt;
      printed += "\t type : ";
      printed += variableType;
      printed += " : variable name : ";
      printed += variableName[0];
      printed += " : ";
      if(dummy ==0) {
        sprintf(tempIntStr,"d=0 0x0");
      } else {
        sprintf(tempIntStr,"d=%u 0x%.08X",*dummy,*dummy);
      }
      printed += tempIntStr;
      printed += "\n";
      printed += frontFPPTxt;
      printed += "\t ***************************************\n";

      if((inOut == ECMD_FPP_FUNCTIONOUT) &&
         (ecmdClientDebug == 8)          &&
         (*dummy == 0)                     ) {
        return;
      }

      ecmdOutput(printed.c_str());

    } else if(!strcmp(variableType,"uint64_t")) {
/* uint64_t */
      uint64_t* dummy = (uint64_t*)(args[looper]);

      printed = frontFPPTxt;
      printed += "\t type : ";
      printed += variableType;
      printed += " : variable name : ";
      printed += variableName[0];
      printed += " : ";
      if(dummy ==0) {
        sprintf(tempIntStr,"d=0 0x0");
      } else {
        sprintf(tempIntStr,"0x%016lluX",(uint64_t)(*dummy));
      }
      printed += tempIntStr;
      printed += "\n";
      printed += frontFPPTxt;
      printed += "\t ***************************************\n";
      ecmdOutput(printed.c_str());

    } else if((!strcmp(variableType,"ecmdDllType_t"))           ||
              (!strcmp(variableType,"ecmdDllProduct_t"))        ||
              (!strcmp(variableType,"ecmdDllEnv_t"))            ||
              (!strcmp(variableType,"ecmdChipTargetState_t"))   ||
              (!strcmp(variableType,"ecmdChipInterfaceType_t")) ||
              (!strcmp(variableType,"ecmdQueryDetail_t"))       ||
              (!strcmp(variableType,"ecmdClockState_t"))        ||
              (!strcmp(variableType,"ecmdSpyType_t"))           ||
              (!strcmp(variableType,"ecmdFileType_t"))          ||
              (!strcmp(variableType,"ecmdConfigLoopType_t"))    ||
              (!strcmp(variableType,"ecmdGlobalVarType_t"))     ||
              (!strcmp(variableType,"ecmdTraceType_t"))         ||
              (!strcmp(variableType,"ecmdLatchMode_t"))         ||
              (!strcmp(variableType,"efppInOut_t"))               ){
/* enums */
      int* dummy = (int*)(args[looper]);

      printed = frontFPPTxt;
      printed += "\t type : ";
      printed += variableType;
      printed += " : variable name : ";
      printed += variableName[0];
      printed += " = ";
      sprintf(tempIntStr,"%u",*dummy);
      printed += tempIntStr;

      printed += "\n";
      printed += frontFPPTxt;
      printed += "\t ***************************************\n";
      ecmdOutput(printed.c_str());

      
    } else if(!strcmp(variableType,"int *")) {
/* int * */
      int *dummy = (int*)(args[looper]);

      printed = frontFPPTxt;
      printed += "\t type : ";
      printed += variableType;
      printed += " : variable name : ";
      printed += variableName[0];
      printed += " = ";
      sprintf(tempIntStr,"%d",*dummy);
      printed += tempIntStr;

      printed += "\n";
      printed += frontFPPTxt;
      printed += "\t ***************************************\n";
      ecmdOutput(printed.c_str());

    } else if(!strcmp(variableType,"bool")) {
/* bool */
      bool* dummy = (bool*)(args[looper]);

      printed = frontFPPTxt;
      printed += "\t type : ";
      printed += variableType;
      printed += " : variable name : ";
      printed += variableName[0];
      printed += " = ";

      if(dummy) {
        printed += "TRUE";

      } else {
        printed += "FALSE";
      }

      printed += "\n";
      printed += frontFPPTxt;
      printed += "\t ***************************************\n";
      ecmdOutput(printed.c_str());

    } else if(!strcmp(variableType,"char**")) {
/* char** */
      char** dummy = (char**)(args[looper]);

      printed = frontFPPTxt;
      printed += "\t type : ";
      printed += variableType;
      printed += " : variable name : ";
      printed += variableName[0];
      printed += " = ";
      printed += *dummy;
      printed += "\n";
      printed += frontFPPTxt;
      printed += "\t ***************************************\n";
      ecmdOutput(printed.c_str());


    } else if((!strcmp(variableType,"ecmdGroupData &"))  ||
              (!strcmp(variableType,"ecmdArrayData &"))  ||
              (!strcmp(variableType,"ecmdArrayEntry &"))  ||
              (!strcmp(variableType,"ecmdNameEntry &"))  ||
              (!strcmp(variableType,"ecmdIndexEntry &"))  ||
              (!strcmp(variableType,"ecmdSpyData &"))  ||
              
              (!strcmp(variableType,"ecmdProcRegisterInfo &"))  ||
              (!strcmp(variableType,"ecmdLooperData &"))  ||
              (!strcmp(variableType,"ecmdThreadData &"))  ||
              (!strcmp(variableType,"ecmdCoreData &"))  ||
              (!strcmp(variableType,"ecmdSlotData &"))  ||
              (!strcmp(variableType,"ecmdNodeData &"))  ||
              (!strcmp(variableType,"ecmdCageData &"))  ||
              (!strcmp(variableType,"ecmdRingData &"))  ||
              (!strcmp(variableType,"ecmdCageData &"))
              ) {
/* default structures not coded yet */
      printed = frontFPPTxt;
      printed += "\t type : ";
      printed += variableType;
      printed += " : variable name : ";
      printed += variableName[0];
      printed += "\n";
      printed += frontFPPTxt;
      printed += "\t \t value = ";
      printed += "placeholder for structure dump\n";
      printed += frontFPPTxt;
      printed += "\t ***************************************\n";
      ecmdOutput(printed.c_str());

    } else if((!strcmp(variableType,"ecmdQueryData &")) ||
              (!strcmp(variableType,"ecmdQueryData&"))    ) {

      ecmdQueryData* dummy = (ecmdQueryData*)(args[looper]);


      printed = frontFPPTxt;
      printed += "\t type : ";
      printed += variableType;
      printed += " : variable name : ";
      printed += variableName[0];
      printed += "\n";
      ecmdOutput(printed.c_str());

      std::list<ecmdCageData>::iterator ecmdCurCage;
      std::list<ecmdNodeData>::iterator ecmdCurNode;
      std::list<ecmdSlotData>::iterator ecmdCurSlot;
      std::list<ecmdChipData>::iterator ecmdCurChip;
      std::list<ecmdCoreData>::iterator ecmdCurCore;
      std::list<ecmdThreadData>::iterator ecmdCurThread;
      char buf[100];
      if (dummy->cageData.empty()) {
        printed = frontFPPTxt;
        printed += "\t \t value = EMPTY\n"; ecmdOutput(printed.c_str());
      } else {

        for (ecmdCurCage = dummy->cageData.begin(); ecmdCurCage != dummy->cageData.end(); ecmdCurCage ++) {
          sprintf(buf,"%s\t \t k%d\n",frontFPPTxt, ecmdCurCage->cageId); ecmdOutput(buf);
          for (ecmdCurNode = ecmdCurCage->nodeData.begin(); ecmdCurNode != ecmdCurCage->nodeData.end(); ecmdCurNode ++) {
            sprintf(buf,"%s\t \t   n%d\n",frontFPPTxt, ecmdCurNode->nodeId); ecmdOutput(buf);

            for (ecmdCurSlot = ecmdCurNode->slotData.begin(); ecmdCurSlot != ecmdCurNode->slotData.end(); ecmdCurSlot ++) {
              sprintf(buf,"%s\t \t     s%d\n",frontFPPTxt, ecmdCurSlot->slotId); ecmdOutput(buf);

              for (ecmdCurChip = ecmdCurSlot->chipData.begin(); ecmdCurChip != ecmdCurSlot->chipData.end(); ecmdCurChip ++) {
                sprintf(buf,"%s\t \t       %s:p%d\n",frontFPPTxt, ecmdCurChip->chipType.c_str(), ecmdCurChip->pos); ecmdOutput(buf);

                for (ecmdCurCore = ecmdCurChip->coreData.begin(); ecmdCurCore != ecmdCurChip->coreData.end(); ecmdCurCore ++) {
                  sprintf(buf,"%s\t \t         c%d\n",frontFPPTxt, ecmdCurCore->coreId); ecmdOutput(buf);

                  for (ecmdCurThread = ecmdCurCore->threadData.begin(); ecmdCurThread != ecmdCurCore->threadData.end(); ecmdCurThread ++) {
                    sprintf(buf,"%s\t \t           t%d\n",frontFPPTxt, ecmdCurThread->threadId); ecmdOutput(buf);
                  } /* curThreadIter */

                } /* curCoreIter */

              } /* curChipIter */

            } /* curSlotIter */

          } /* curNodeIter */

        } /* curCageIter */
      }
      printed = frontFPPTxt;
      printed += "\t ***************************************\n";
      ecmdOutput(printed.c_str());

    } else if((!strcmp(variableType,"ecmdDataBuffer &")) ||
              (!strcmp(variableType,"ecmdDataBuffer&"))    ) {
/***
private:  //data
  int iv_Capacity;              ///< Actual buffer capacity - always >= iv_NumWords
  int iv_NumWords;              ///< Specified buffer size rounded to next word
  int iv_NumBits;               ///< Specified buffer size in bits
  uint32_t * iv_Data;           ///< Pointer to buffer inside iv_RealData
  uint32_t * iv_RealData;       ///< Real buffer - with header and tail
***/
      ecmdDataBuffer* dummy = (ecmdDataBuffer*)(args[looper]);

      printEcmdDataBuffer(variableType, variableName[0], *dummy, "");

    } else if((!strcmp(variableType,"ecmdChipTarget &")) ||
              (!strcmp(variableType,"ecmdChipTarget&"))    ) {

      /***
       struct ecmdChipTarget {

         uint32_t    cage;
         uint32_t    node;
         uint32_t    slot;
         std::string chipType;
         uint32_t    pos;
         uint8_t     core;
         uint8_t     thread;

         ecmdChipTargetState_t cageState;
         ecmdChipTargetState_t nodeState;
         ecmdChipTargetState_t slotState;
         ecmdChipTargetState_t chipTypeState;
         ecmdChipTargetState_t posState;
         ecmdChipTargetState_t coreState;
         ecmdChipTargetState_t threadState;

         uint32_t unitId;
         ecmdChipTargetState_t unitIdState;
       };
       ***/

      ecmdChipTarget* dummy = (ecmdChipTarget*)(args[looper]);

      printed = frontFPPTxt;
      printed += "\t type : ";
      printed += variableType;
      printed += " : variable name : ";
      printed += variableName[0];
      printed += "\n";
      ecmdOutput(printed.c_str());

      printed = frontFPPTxt;
      printed += "\t \t value : uint32_t    cage     = ";
      sprintf(tempIntStr,"%u",dummy->cage);
      printed += tempIntStr;
      printed += "\tState = ";
      printed += printEcmdChipTargetState_t(dummy->cageState);
      ecmdOutput(printed.c_str());

      printed = frontFPPTxt;
      printed += "\t \t value : uint32_t    node     = ";
      sprintf(tempIntStr,"%u",dummy->node);
      printed += tempIntStr;
      printed += "\tState = ";
      printed += printEcmdChipTargetState_t(dummy->nodeState);
      ecmdOutput(printed.c_str());

      printed = frontFPPTxt;
      printed += "\t \t value : uint32_t    slot     = ";
      sprintf(tempIntStr,"%u",dummy->slot);
      printed += tempIntStr;
      printed += "\tState = ";
      printed += printEcmdChipTargetState_t(dummy->slotState);
      ecmdOutput(printed.c_str());

      printed = frontFPPTxt;
      printed += "\t \t value : std::string chiptype = ";
      printed += dummy->chipType.c_str();
      printed += "\tState = ";
      printed += printEcmdChipTargetState_t(dummy->chipTypeState);
      ecmdOutput(printed.c_str());

      printed = frontFPPTxt;
      printed += "\t \t value : uint32_t    pos      = ";
      sprintf(tempIntStr,"%u",dummy->pos);
      printed += tempIntStr;
      printed += "\tState = ";
      printed += printEcmdChipTargetState_t(dummy->posState);
      ecmdOutput(printed.c_str());

      printed = frontFPPTxt;
      printed += "\t \t value : uint32_t    core     = ";
      sprintf(tempIntStr,"%u",dummy->core);
      printed += tempIntStr;
      printed += "\tState = ";
      printed += printEcmdChipTargetState_t(dummy->coreState);
      ecmdOutput(printed.c_str());

      printed = frontFPPTxt;
      printed += "\t \t value : uint32_t    thread   = ";
      sprintf(tempIntStr,"%d",dummy->thread);
      printed += tempIntStr;
      printed += "\tState = ";
      printed += printEcmdChipTargetState_t(dummy->threadState);
      ecmdOutput(printed.c_str());

      printed = frontFPPTxt;
      printed += "\t \t value : uint32_t    unitId   = ";
      sprintf(tempIntStr,"0x%.08X",dummy->unitId);
      printed += tempIntStr;
      printed += "\tState = ";
      printed += printEcmdChipTargetState_t(dummy->unitIdState);
      ecmdOutput(printed.c_str());


      printed = frontFPPTxt;
      printed += "\t ***************************************\n";
      ecmdOutput(printed.c_str());

    } else if((!strcmp(variableType,"ecmdDllInfo &")) ||
              (!strcmp(variableType,"ecmdDllInfo&"))    ) {

/***
struct ecmdDllInfo {
  ecmdDllType_t         dllType;        ///< Dll instance type running
  ecmdDllProduct_t      dllProduct;     ///< Dll product supported
  ecmdDllEnv_t          dllEnv;         ///< Dll environment (Simulation vs Hardware)
  std::string           dllBuildDate;   ///< Date the Dll was built
  std::string           dllCapiVersion; ///< should be set to ECMD_CAPI_VERSION
  std::string           dllBuildInfo;   ///< Any additional info the Dll/Plugin would like to pass
};
***/

      ecmdDllInfo* dummy = (ecmdDllInfo*)(args[looper]);


      printed = frontFPPTxt;
      printed += "\t type : ";
      printed += variableType;
      printed += " : variable name : ";
      printed += variableName[0];
      printed += "\n";
      printed += frontFPPTxt;
      printed += "\t \t value : ecmdDllType_t         dllType = ";

      if (dummy->dllType == ECMD_DLL_STUB)
        printed += "Stub\n";
      else if (dummy->dllType == ECMD_DLL_STUB)
        printed += "Stub\n";
      else if (dummy->dllType == ECMD_DLL_CRONUS)
        printed += "Cronus\n";
      else if (dummy->dllType == ECMD_DLL_IPSERIES)
        printed += "IP-Series\n";
      else if (dummy->dllType == ECMD_DLL_ZSERIES)
        printed += "Z-Series\n";
      else if (dummy->dllType == ECMD_DLL_SCAND)
        printed += "ScanD\n";
      else 
        printed += "Unknown\n";

      ecmdOutput(printed.c_str());


      printed = frontFPPTxt;
      printed += "\t \t value : ecmdDllProduct_t      dllProduct = ";

      if (dummy->dllProduct == ECMD_DLL_PRODUCT_ECLIPZ)
        printed += "Eclipz\n";
      else
        printed += "Unknown\n";
      ecmdOutput(printed.c_str());


      printed = frontFPPTxt;
      printed += "\t \t value : ecmdDllEnv_t          dllEnv = ";
      if (dummy->dllEnv == ECMD_DLL_ENV_HW)
        printed += "Hardware\n";
      else
        printed += "Simulation\n";
      ecmdOutput(printed.c_str());

      printed = frontFPPTxt;
      printed += "\t \t value : std::string           dllBuildDate = ";
//      printed += "strings not working yet";
      printed += dummy->dllBuildDate.c_str();
      printed += "\n";
      ecmdOutput(printed.c_str());

      printed = frontFPPTxt;
      printed += "\t \t value : std::string           dllCapiVersion = ";
//      printed += "strings not working yet";
      printed += dummy->dllCapiVersion.c_str();
      printed += "\n";
      ecmdOutput(printed.c_str());

      printed = frontFPPTxt;
      printed += "\t \t value : std::string           dllBuildInfo = ";
      printed += dummy->dllBuildInfo.c_str();
//      printed += "strings not working yet";

      printed += "\n";
      ecmdOutput(printed.c_str());

      printed = frontFPPTxt;
      printed += "\t ***************************************\n";
      ecmdOutput(printed.c_str());
/*#endif*/

    } else {
      printed = "WARNING::ecmdFunctionParmPrinter  Unknown variableType = ";
      printed += variableType;
      printed += " \n";
      printed += frontFPPTxt;
      printed += "\t ***************************************\n";
      ecmdOutput(printed.c_str());
    }    

  }
  printed = frontFPPTxt;
  printed += "*********************************************************************************\n";
  ecmdOutput(printed.c_str());
  printed = frontFPPTxt;
  printed += "*********************************************************************************\n";
  ecmdOutput(printed.c_str());

}

std::string printEcmdChipTargetState_t(ecmdChipTargetState_t i_state) {

/***
typedef enum {
  ECMD_TARGET_UNKNOWN_STATE,    ///< State field has not been initialized

  ECMD_TARGET_FIELD_VALID,      ///< Associated State Field is valid for this function
  ECMD_TARGET_FIELD_UNUSED,     ///< Associated State Field is unused for this function

  ECMD_TARGET_QUERY_FIELD_VALID,///< Associated State Field is valid for the query
  ECMD_TARGET_QUERY_WILDCARD,   ///< Associated State Field should be itterated on and all valid results returned
  ECMD_TARGET_QUERY_IGNORE,     ///< Query should be limited to data above this field, ignoring data 

  ECMD_TARGET_THREAD_ALIVE      ///< Used when calling thread dependent functions tell the function to check for the thread to be alive before running
} ecmdChipTargetState_t;
***/

  if (i_state == ECMD_TARGET_UNKNOWN_STATE ) {
    return "ECMD_TARGET_UNKNOWN_STATE\n";
  } else if (i_state == ECMD_TARGET_FIELD_VALID ) {
    return "ECMD_TARGET_FIELD_VALID\n";
  } else if (i_state == ECMD_TARGET_FIELD_UNUSED ) {
    return "ECMD_TARGET_FIELD_UNUSED\n";
  } else if (i_state == ECMD_TARGET_QUERY_FIELD_VALID ) {
    return "ECMD_TARGET_QUERY_FIELD_VALID\n";
  } else if (i_state == ECMD_TARGET_QUERY_WILDCARD ) {
    return "ECMD_TARGET_QUERY_WILDCARD\n";
  } else if (i_state == ECMD_TARGET_QUERY_IGNORE ) {
    return "ECMD_TARGET_QUERY_IGNORE\n";
  } else if (i_state == ECMD_TARGET_THREAD_ALIVE ) {
    return "ECMD_TARGET_THREAD_ALIVE\n";
  } else {
    return "ERROR INVALID TYPE\n";
  }

}
uint32_t ecmdDisplayScomData(ecmdChipTarget & i_target, uint32_t i_address, ecmdDataBuffer & i_data, const char* i_format) {
  uint32_t rc = ECMD_SUCCESS;
  std::string scomdefFileStr;                   ///< Full Path to the Scomdef file
  sedcScomdefEntry scomEntry;                ///< Returns a class containing the scomdef entry read from the file
  unsigned int runtimeFlags=0;                    ///< Directives on how to parse
  bool verboseFlag = false;
  bool verboseBitsSetFlag = false;              ///< Print Bit description only if bit/s are set
  bool verboseBitsClearFlag = false;            ///< Print Bit description only if No bits are set
  std::string printed;                          ///< Output data
  std::vector<std::string> errMsgs;             ///< Any error messages to go with a array that was marked invalid

  if ((std::string)i_format == "-v") {
    verboseFlag = true;
  }
  if ((std::string)i_format == "-vs0") {
    verboseBitsClearFlag = true;
  }
  if ((std::string)i_format == "-vs1") {
    verboseBitsSetFlag = true;
  }
  rc = ecmdQueryFileLocation(i_target, ECMD_FILE_SCOMDATA, scomdefFileStr);
  if (rc) {
    printed = "ecmdDisplayScomData - Error occured locating scomdef file: " + scomdefFileStr + "\nSkipping -v parsing\n";
    ecmdOutputWarning(printed.c_str());
    return rc;
  }
  
  std::ifstream scomdefFile(scomdefFileStr.c_str());
  if(scomdefFile.fail()) {
    printed = "ecmdDisplayScomData - Error occured opening scomdef file: " + scomdefFileStr + "\nSkipping -v parsing\n";
    ecmdOutputWarning(printed.c_str());
    rc = ECMD_UNABLE_TO_OPEN_SCOMDEF;
    return rc;
  }
  rc = readScomDefFile(i_address, scomdefFile);
  if (rc == ECMD_SCOMADDRESS_NOT_FOUND) {
    ecmdOutputWarning("ecmdDisplayScomData - Scom Address not found. Skipping -v parsing\n");
    return rc;
  }
  scomEntry = sedcScomdefParser(scomdefFile, errMsgs, runtimeFlags);
  

  std::list< std::string >::iterator descIt;
  std::list<sedcScomdefDefLine>::iterator definIt;
  std::list< std::string >::iterator bitDetIt;
  char bitDesc[1000];
  
  sprintf(bitDesc,"Name       : %20s%s\nDesc	   : %20s", " ",scomEntry.name.c_str()," ");  
  ecmdOutput(bitDesc);

  for (descIt = scomEntry.description.begin(); descIt != scomEntry.description.end(); descIt++) {
    sprintf(bitDesc,"%s", descIt->c_str());
    ecmdOutput(bitDesc);
  }
  ecmdOutput("\n");
  //Print Bits description
  for (definIt = scomEntry.definition.begin(); definIt != scomEntry.definition.end(); definIt++) {
    if ((i_data.getNumBitsSet(definIt->lhsNum, definIt->length) && verboseBitsSetFlag) ||
    ((!i_data.getNumBitsSet(definIt->lhsNum, definIt->length)) && verboseBitsClearFlag) || (verboseFlag)) {
      
      if(definIt->rhsNum == -1) {
  	sprintf(bitDesc, "Bit(%d)", definIt->lhsNum);
      }
      else {
  	sprintf(bitDesc, "Bit(%d:%d)", definIt->lhsNum,definIt->rhsNum);
      }
      sprintf(bitDesc, "%-10s : ",bitDesc);
      ecmdOutput(bitDesc);

      if (definIt->length <= 8) {
  	std::string binstr = i_data.genBinStr(definIt->lhsNum, definIt->length);
  	sprintf(bitDesc, "0b%-16s  %s\n",binstr.c_str(),definIt->dialName.c_str());
      }
      else {
  	std::string hexLeftStr = i_data.genHexLeftStr(definIt->lhsNum, definIt->length);
  	sprintf(bitDesc, "0x%-16s  %s\n",hexLeftStr.c_str(),definIt->dialName.c_str());
      }
      ecmdOutput(bitDesc);
      std::string bitDescStr;
      for (bitDetIt = definIt->detail.begin(); bitDetIt != definIt->detail.end(); bitDetIt++) {
  	sprintf(bitDesc, "%32s ", " ");
  	//Would print the entires string no matter how long it is
  	//bitDescStr = (std::string)bitDesc + *bitDetIt +"\n";
  	//ecmdOutput(bitDescStr.c_str());

  	std::string tmpstr;
  	uint32_t curptr =0, len, maxdesclen=80;
  	while (curptr < (*bitDetIt).length()) {
  	  if (((*bitDetIt).length() - curptr) < maxdesclen) {
  	   len = (*bitDetIt).length() - curptr;
  	  }
  	  else {
  	   len = maxdesclen;
  	  }
  	  tmpstr = (*bitDetIt).substr(curptr,len);
  	  bitDescStr = (std::string)bitDesc + tmpstr + "\n";
  	  ecmdOutput(bitDescStr.c_str());
  	  curptr += len;
  	}
 
      }//end for
    }//end if 
  }// end for
  return rc;
}

uint32_t readScomDefFile(uint32_t address, std::ifstream &scomdefFile) {
  uint32_t rc = ECMD_SUCCESS;
  std::string scomdefFileStr;                      ///< Full path to scomdef file
  std::string printed;
  
  
  std::string curLine;
  uint32_t beginPtr;
  uint32_t beginLen;

  bool done = false; 
  std::vector<std::string> curArgs(4);
  
  while (getline(scomdefFile, curLine) && !done) {
    //Remove leading whitespace
    uint32_t curStart = curLine.find_first_not_of(" \t", 0);
    if (curStart != std::string::npos) {
      curLine = curLine.substr(curStart,curLine.length());
    }
    if((curLine[0] == 'B') && (curLine.find("BEGIN Scom") != std::string::npos)) {
      beginPtr = scomdefFile.tellg();
      beginLen = curLine.length();
    }
    if((curLine[0] == 'A') && (curLine.find("Address") != std::string::npos)) {
      ecmdParseTokens(curLine, " \t\n={}", curArgs);
      uint32_t addrFromFile = ecmdGenB32FromHexRight(&addrFromFile, curArgs[1].c_str());
      if ((curArgs.size() >= 2) && addrFromFile == address) {
        done = true;
      }
    }
  }
  if (done) {
    scomdefFile.seekg(beginPtr-beginLen-1);
  }
  else {
    ecmdOutputWarning("Unable to find Scom Address in the Scomdef file\n");
    rc = ECMD_SCOMADDRESS_NOT_FOUND;
  }
  return rc;
}

void printEcmdDataBuffer(std::string variableType, std::string variableName, ecmdDataBuffer & i_data, std::string tabStop) {
  std::string printed;
  char tempIntStr[180];
  int dataLooper;

  printed = frontFPPTxt;
  printed += "\t "+tabStop+"type : ";
  printed += variableType;
  printed += " : variable name : ";
  printed += variableName;
  printed += "\n";
  ecmdOutput(printed.c_str());

  printed = frontFPPTxt;
  printed += "\t \t "+tabStop+"value : int    iv_Capacity = ";
  sprintf(tempIntStr,"%d",i_data.getCapacity());
  printed += tempIntStr;
  printed += "\n";
  ecmdOutput(printed.c_str());

  printed = frontFPPTxt;
  printed += "\t \t "+tabStop+"value : int    iv_NumWords = ";
  sprintf(tempIntStr,"%d",i_data.getWordLength());
  printed += tempIntStr;
  printed += "\n";
  ecmdOutput(printed.c_str());

  printed = frontFPPTxt;
  printed += "\t \t "+tabStop+"value : int    iv_NumBits  = ";
  sprintf(tempIntStr,"%d",i_data.getBitLength());
  printed += tempIntStr;
  printed += "\n";
  ecmdOutput(printed.c_str());

  if(i_data.getBitLength() == 0) {
    printed = frontFPPTxt;
    printed += "\t \t "+tabStop+"value : uint32_t[] iv_Data =\n";
    printed += frontFPPTxt;
    printed += "\t "+tabStop+"***************************************\n";
    ecmdOutput(printed.c_str());
    return;
  }

  if(i_data.hasXstate()) {
    if (ecmdClientDebug == 9) {
      printed = frontFPPTxt;
      printed += "\t \t "+tabStop+"value : XSTATE iv_DataStr  = ";
      printed += i_data.genXstateStr(0, 64 < i_data.getBitLength() ? 64 : i_data.getBitLength());
      printed += "\n";
      ecmdOutput(printed.c_str());
    } else {
      printed = frontFPPTxt;
      printed += "\t \t "+tabStop+"value : XSTATE iv_DataStr  = ";
      printed += i_data.genXstateStr();
      printed += "\n";
      ecmdOutput(printed.c_str());
    }
  } else {
    if (ecmdClientDebug == 9) {
      printed = frontFPPTxt;
      printed += "\t \t "+tabStop+"value : uint32_t[] iv_Data = ";
      printed += i_data.genHexLeftStr(0, 64 < i_data.getBitLength() ? 64 : i_data.getBitLength());
      printed += "\n";
      ecmdOutput(printed.c_str());
    } else {

      char tbuf[10];
      printed = frontFPPTxt;
      printed += "\t \t "+tabStop+"value : uint32_t[] iv_Data = ";

      if (i_data.getWordLength() > 4) {
        printed += "\n"; ecmdOutput(printed.c_str());
        printed = frontFPPTxt;
        printed += "\t \t \t\t"+tabStop+"";
      }

      for (dataLooper = 0; (uint32_t) dataLooper < i_data.getWordLength(); dataLooper ++) {
        if (!(dataLooper % 4) && (dataLooper != 0)) {
          printed += "\n";
          ecmdOutput(printed.c_str());
          printed = frontFPPTxt;
          printed += "\t \t \t\t"+tabStop+"";
        }
        sprintf(tbuf,"%.08X ",i_data.getWord(dataLooper));
        printed += tbuf;
      }
      printed += "\n";
      ecmdOutput(printed.c_str());

    }
  }
  printed = frontFPPTxt;
  printed += "\t "+tabStop+"***************************************\n";
  ecmdOutput(printed.c_str());

  return ;
}

#endif /* strip_debug */



