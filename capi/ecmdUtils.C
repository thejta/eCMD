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

#include <ecmdUtils.H>
#include <ecmdClientCapi.H>
#include <ecmdReturnCodes.H>
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
  ECMD_FORMAT_BXW
} ecmdFormatState_t;

//--------------------------------------------------------------------
// Macros
//--------------------------------------------------------------------
//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------
#ifndef ECMD_STRIP_DEBUG
/* This is from ecmdClientCapi.C */
extern int ecmdClientDebug;
#endif

//----------------------------------------------------------------------
//  User Types
//----------------------------------------------------------------------



uint32_t ecmdConfigLooperInit (ecmdChipTarget & io_target, ecmdConfigLoopType_t i_looptype, ecmdLooperData& io_state) {

  uint32_t rc = ECMD_SUCCESS;

  ecmdChipTarget queryTarget = io_target;

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug > 1) {
    std::string printed = "ECMD DEBUG (ecmdConfigLooperInit) : Entering\n"; ecmdOutput(printed.c_str());
  }
#endif

  if (queryTarget.cageState == ECMD_TARGET_FIELD_UNUSED) {
    queryTarget.cageState = ECMD_TARGET_QUERY_IGNORE;
  }

  if (queryTarget.nodeState == ECMD_TARGET_FIELD_UNUSED) {
    queryTarget.nodeState = ECMD_TARGET_QUERY_IGNORE;
  }

  if (queryTarget.slotState == ECMD_TARGET_FIELD_UNUSED) {
    queryTarget.slotState = ECMD_TARGET_QUERY_IGNORE;
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

  /* Initialize defaults into the incoming target */
  if (io_target.cageState == ECMD_TARGET_QUERY_WILDCARD) {
    io_target.cage = 0;
  }

  if (io_target.nodeState == ECMD_TARGET_QUERY_WILDCARD) {
    io_target.node = 0;
  }

  if (io_target.slotState == ECMD_TARGET_QUERY_WILDCARD) {
    io_target.slot = 0;
  }

  if (io_target.chipTypeState == ECMD_TARGET_QUERY_WILDCARD) {
    io_target.chipType = "na";
  }

  if (io_target.posState == ECMD_TARGET_QUERY_WILDCARD) {
    io_target.pos = 0;
  }

  if (io_target.coreState == ECMD_TARGET_QUERY_WILDCARD) {
    io_target.core = 0;
  }

  if (io_target.threadState == ECMD_TARGET_QUERY_WILDCARD) {
    io_target.thread = 0;
  }



  if (i_looptype == ECMD_SELECTED_TARGETS_LOOP) 
    rc = ecmdQuerySelected(queryTarget, io_state.ecmdSystemConfigData);
  else
    rc = ecmdQueryConfig(queryTarget, io_state.ecmdSystemConfigData);
  if (rc) return rc;

  io_state.ecmdCurCage = io_state.ecmdSystemConfigData.cageData.begin();
  io_state.ecmdLooperInitFlag = true;
  io_state.prevTarget = io_target;

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug > 1) {
    std::string printed = "ECMD DEBUG (ecmdConfigLooperInit) : Exiting\n"; ecmdOutput(printed.c_str());
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

  uint8_t level = CAGE;
  uint8_t valid = 1;

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug > 1) {
    std::string printed = "ECMD DEBUG (ecmdConfigLooperNext) : Entering\n"; ecmdOutput(printed.c_str());
  }
#endif

  /* We are at the end of the loop, nothing left to loop on, get out of here */
  if (io_state.ecmdCurCage == io_state.ecmdSystemConfigData.cageData.end()) {
    return 0;
  }

  /* ******** NOTE : The iterators in io_state always point to the next instance to use */
  /*           (the one that should be returned from this function ****************     */

  /* Enter if : */
  /* First time in config looper */
  /* last cage != current cage */
  if (io_state.ecmdLooperInitFlag || io_target.cage != (*io_state.ecmdCurCage).cageId) {

    io_target.cage = (*io_state.ecmdCurCage).cageId;
    io_state.ecmdCurNode = (*io_state.ecmdCurCage).nodeData.begin();
    valid = 0;

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
  if (level == NODE && (!valid || io_target.node != (*io_state.ecmdCurNode).nodeId)) {

    io_target.node = (*io_state.ecmdCurNode).nodeId;
    io_state.ecmdCurSlot = (*io_state.ecmdCurNode).slotData.begin();
    valid = 0;
  
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
  if (level == SLOT && (!valid || io_target.slot != (*io_state.ecmdCurSlot).slotId)) {

    io_target.slot = (*io_state.ecmdCurSlot).slotId;
    io_state.ecmdCurChip = (*io_state.ecmdCurSlot).chipData.begin();
    valid = 0;
  
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
  if (level == CHIP && (!valid || io_target.chipType != (*io_state.ecmdCurChip).chipType || io_target.pos != (*io_state.ecmdCurChip).pos )) {

    io_target.chipType = (*io_state.ecmdCurChip).chipType;
    io_target.pos = (*io_state.ecmdCurChip).pos;
    io_state.ecmdCurCore = (*io_state.ecmdCurChip).coreData.begin();
    valid = 0;

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
  if (level == CORE && (!valid || io_target.core != (*io_state.ecmdCurCore).coreId)) {

    io_target.core = (*io_state.ecmdCurCore).coreId;
    io_state.ecmdCurThread = (*io_state.ecmdCurCore).threadData.begin();
    valid = 0;

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

    io_target.thread = (*io_state.ecmdCurThread).threadId;

  }

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug > 1) {
    std::string printed = "ECMD DEBUG (ecmdConfigLooperNext) : Found next Target : " + ecmdWriteTarget(io_target); printed += "\n";
    ecmdOutput(printed.c_str());
  }
#endif

  /* Let's start incrementing our lowest pointer so it points to the next object for the subsequent call to this function */
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

  if (io_state.ecmdLooperInitFlag) {
    io_state.ecmdLooperInitFlag = false;
  }

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug > 1) {
    std::string printed = "ECMD DEBUG (ecmdConfigLooperNext) : Exiting\n"; ecmdOutput(printed.c_str());
  }
#endif

  return 1;

}

uint32_t ecmdReadDataFormatted (ecmdDataBuffer & o_data, const char * i_dataStr, std::string & i_format, int i_expectedLength) {
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
  else {
    ecmdOutputError( ("Did not recognize input format string " + localFormat + "\n").c_str() );
    rc = ECMD_INVALID_ARGS;
  }
  if (rc == ECMD_DBUF_INVALID_DATA_FORMAT) {
    ecmdOutputError(( "Input Data contained some invalid characters for format specified : '" + localFormat + "'!\n").c_str());
  }

  return rc;
}

std::string ecmdWriteDataFormatted (ecmdDataBuffer & i_data, std::string & i_format, int address) {
  std::string printed;
  int formTagLen = i_format.length();
  ecmdFormatState_t curState = ECMD_FORMAT_NONE;
  int numCols = 0;
  bool good = true;
  bool perlMode = false;

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
    else if (i_format[i] == 'p') {
      perlMode = true;
    }
    else if (isdigit(i_format[i])) {
      numCols = atoi(&i_format[i]);
      break;
    }
    else {
      good = false;
      break;
    }

  }

  if (curState == ECMD_FORMAT_NONE) {
    good = false;
  }

  if (!good) {
    //check for other possible formats the string might match
  }

  if (!good) {
    printed = "Unrecognized format string: ";
    printed += i_format + "\n";
    ecmdOutputError(printed.c_str());
    printed = "";
    return printed;
  }

  if (curState == ECMD_FORMAT_X) {
    printed += i_data.genHexLeftStr();
    if (!perlMode) printed = "0x" + printed;
  }
  else if (curState == ECMD_FORMAT_XR) {
    printed = i_data.genHexRightStr();
    if (!perlMode) printed = "0xr" + printed;
  }
  else if (curState == ECMD_FORMAT_B) {
    printed = i_data.genBinStr();
    if (!perlMode) printed = "0b" + printed;
  }
  else if (curState == ECMD_FORMAT_BX) {
    //do something
    printed = i_data.genXstateStr();
    if (!perlMode) printed = "0bX" + printed;
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
        printed += ecmdBitsHeader(4, blockSize, numCols, dataBitLength);
      } 

      printed += "\n00: ";
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
        if (curCol < 10) {
          sprintf(outstr, "\n0%d: ", curCol);
        }
        else {
          sprintf(outstr, "\n%d: ", curCol % 100);
        }

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
  }

  if (!perlMode) {
    printed += "\n";
  }

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
  if (o_data.chipType != i_target.chipType || o_data.pos != i_target.pos) {
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
