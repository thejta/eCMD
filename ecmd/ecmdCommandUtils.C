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

static ecmdQueryData ecmdSystemConfigData;
static std::list<ecmdCageData>::iterator ecmdCurCage;
static std::list<ecmdNodeData>::iterator ecmdCurNode;
static std::list<ecmdChipData>::iterator ecmdCurChip;
static std::list<ecmdCoreData>::iterator ecmdCurCore;
static std::list<ecmdThreadData>::iterator ecmdCurThread;
static uint8_t ecmdLooperInitFlag;

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
} ecmdFormatState_t;

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
  if (rc) return rc;

  ecmdCurCage = ecmdSystemConfigData.cageData.begin();
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

  if (ecmdCurCage == ecmdSystemConfigData.cageData.end()) {
    return 0;
  }

  if (io_target.cage != (*ecmdCurCage).cageId || ecmdLooperInitFlag) {

    io_target.cage = (*ecmdCurCage).cageId;
    ecmdCurNode = (*ecmdCurCage).nodeData.begin();

    if (io_target.nodeState == ECMD_TARGET_FIELD_UNUSED || ecmdCurNode == (*ecmdCurCage).nodeData.end()) {
      valid = 0;
      io_target.node = 0;
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

  if (level == CHIP && (io_target.chipType != (*ecmdCurChip).chipType || io_target.pos != (*ecmdCurChip).pos || ecmdLooperInitFlag)) {

    io_target.chipType = (*ecmdCurChip).chipType;
    io_target.pos = (*ecmdCurChip).pos;
    ecmdCurCore = (*ecmdCurChip).coreData.begin();

    if (io_target.coreState == ECMD_TARGET_FIELD_UNUSED || ecmdCurCore == (*ecmdCurChip).coreData.end()) {
      valid = 0;
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

  if (level == CORE && (io_target.core != (*ecmdCurCore).coreId || ecmdLooperInitFlag)) {

    io_target.core = (*ecmdCurCore).coreId;
    ecmdCurThread = (*ecmdCurCore).threadData.begin();

    if (io_target.threadState == ECMD_TARGET_FIELD_UNUSED || ecmdCurThread == (*ecmdCurCore).threadData.end()) {
      valid = 0;
      io_target.thread = 0;
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

int ecmdGetChipData (ecmdChipTarget & i_target, ecmdChipData & o_data) {
  int rc = ECMD_SUCCESS;

  ecmdChipTarget tmp = i_target;
  ecmdQueryData needlesslySlow;
  tmp.cageState = tmp.nodeState = tmp.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  rc = ecmdQueryConfig(tmp, needlesslySlow, ECMD_QUERY_DETAIL_HIGH);
  if (rc) return rc;

  if (needlesslySlow.cageData.empty()) return ECMD_INVALID_ARGS;
  if (needlesslySlow.cageData.front().cageId != i_target.cage) return ECMD_INVALID_ARGS;
  if (needlesslySlow.cageData.front().nodeData.empty()) return ECMD_INVALID_ARGS;
  if (needlesslySlow.cageData.front().nodeData.front().nodeId != i_target.node) return ECMD_INVALID_ARGS;
  if (needlesslySlow.cageData.front().nodeData.front().chipData.empty()) return ECMD_INVALID_ARGS;

  o_data = needlesslySlow.cageData.front().nodeData.front().chipData.front();
  if (o_data.chipType != i_target.chipType || o_data.pos != i_target.pos) {
    return ECMD_INVALID_ARGS;
  }
    
  return rc;
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

  return printed;

}

int ecmdReadDataFormatted (ecmdDataBuffer & o_data, const char * i_dataStr, std::string & i_format) {
  int rc = ECMD_SUCCESS;

  std::string localFormat = i_format;

  //ignore leading 'p'- it's for perl stuff
  if (localFormat[0] == 'p') {
    localFormat = localFormat.substr(1, localFormat.size());
  }

  if (localFormat == "x" || localFormat == "xl") {
    o_data.setBitLength(strlen(i_dataStr) * 4);
    rc = o_data.insertFromHexLeft(i_dataStr);
  }
  else if (localFormat == "xr") {
    o_data.setBitLength(strlen(i_dataStr) * 4);
    rc = o_data.insertFromHexRight(i_dataStr);
  }     
  else if (localFormat == "b") {
    o_data.setBitLength(strlen(i_dataStr));
    rc = o_data.insertFromBin(i_dataStr);
  }
  else {
    ecmdOutputError( ("Did not recognize input format string " + localFormat).c_str() );
    rc = ECMD_INVALID_ARGS;
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
      if (curState != ECMD_FORMAT_B) {
        good = false;
        break;
      }

      curState = ECMD_FORMAT_BN;
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
    printed += i_format;
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
    if (!perlMode) printed = "0x" + printed;
  }
  else if (curState == ECMD_FORMAT_B) {
    printed = i_data.genBinStr();
    if (!perlMode) printed = "0b" + printed;
  }
  else if (curState == ECMD_FORMAT_BX) {
    //do something
  }
  else {

    char * outstr = new char[40];
    int curCol = 0;
    int colCount = 0;
    int blockSize = 32;
    if (curState == ECMD_FORMAT_BN) blockSize = 4;
    int curOffset = 0;
    int numBlocks = (i_data.getWordLength() * 32) / blockSize;
    
    if (numCols) {
      printed = "\n00: ";
    }

    for (int i = 0; i < numBlocks; i++) {

      if (curState == ECMD_FORMAT_XW) {
        printed += i_data.genHexLeftStr(curOffset, blockSize);
      }
      else if (curState == ECMD_FORMAT_XRW) {
        printed += i_data.genHexRightStr(curOffset, blockSize);
      }
      else {
        printed += i_data.genBinStr(curOffset, blockSize);
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
