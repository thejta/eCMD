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


static ecmdQueryData ecmdSystemConfigData;
static std::list<ecmdCageData>::iterator ecmdCurCage;
static std::list<ecmdNodeData>::iterator ecmdCurNode;
static std::list<ecmdSlotData>::iterator ecmdCurSlot;
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

//--------------------------------------------------------------------
// Macros
//--------------------------------------------------------------------
/**
 * @brief Iterates over argv, removing null pointers and decrementing argc
 * @retval None
 * @param io_argc Pointer to number of elements in io_argv array
 * @param io_argv Array of strings passed in from command line

 - Utility function for ecmdParseOption and ecmdParseOptionWithArgs
 */
void ecmdRemoveNullPointers (int * io_argc, char ** io_argv[]);

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

uint32_t ecmdGenB32FromHex (uint32_t * o_numPtr, const char * i_hexChars, int startPos) {

  uint32_t tempB32 = 0;
  int	counter = 0;
  char	twoChars[2] = "0";

  if ((o_numPtr == NULL) || (i_hexChars == NULL) || (strlen(i_hexChars) == 0))
    return 0xFFFFFFFF;

  o_numPtr[0] = 0x0;
  o_numPtr[startPos>>3]= 0x0;

  for (counter = 0; counter < strlen(i_hexChars); counter++) {
    if (((counter + startPos) & 0xFFF8) == (counter + startPos))
      o_numPtr[(counter+startPos)>>3] = 0x0;
    twoChars[0] = i_hexChars[counter];
    tempB32 = strtoul((char *) & twoChars[0], NULL, 16);
    o_numPtr[(counter+startPos)>>3] |= (tempB32 << (28 - (4 * ((counter + startPos) & 0x07))));
  }

  return o_numPtr[0];

}


uint32_t ecmdGenB32FromHexLeft (uint32_t * o_numPtr, const char * i_hexChars) {

  if ((i_hexChars == NULL) || (o_numPtr == NULL))
    return 0xFFFFFFFF;

  return ecmdGenB32FromHex(o_numPtr, i_hexChars, 0);

}

uint32_t ecmdGenB32FromHexRight (uint32_t * o_numPtr, const char * i_hexChars, int expectBits) {

  int stringSize;

  if ((i_hexChars == NULL) || (o_numPtr == NULL))
    return 0xFFFFFFFF;

  /* ----------------------------------------------------------------- */	
  /* Telling us we are expecting few bits than the user input          */	
  /* ----------------------------------------------------------------- */	
  stringSize = strlen(i_hexChars);

  if (expectBits==0) 
    expectBits = ((stringSize << 2) + 31) & 0xFFFE0;

  /* ----------------------------------------------------------------- */
  /* Can't figure out if this case is bad...                           */
  /* ----------------------------------------------------------------- */
  if (expectBits<(stringSize*4)) 
    expectBits = (4-(expectBits & 0x3))+expectBits+(stringSize*4-expectBits&0xFFFC);

  expectBits = (expectBits - stringSize*4)>>2; 

  return ecmdGenB32FromHex(o_numPtr, i_hexChars, expectBits );


}



void ecmdRemoveNullPointers (int *argc, char **argv[]) {
  int counter=0;
  int counter2=0;

  for (counter=0;counter<(*argc+1);counter++) {
    for (counter2=counter;counter2<*argc;counter2++) {
      if ((*argv)[counter]==NULL) {
        (*argv)[counter]=(*argv)[counter2];
        (*argv)[counter2]=NULL;
      }
    }
  }

  for (counter=0;counter<(*argc);counter++) {
    if ((*argv)[counter]==NULL) {
      *argc=counter;
      return;
    }
  }
}


int ecmdParseOption (int *argc, char **argv[], const char *option) {
  int counter = 0;
  int foundit = 0;

  for (counter = 0; counter < *argc ; counter++) {
    if (((*argv)[counter] != NULL) && (strncmp((*argv)[counter],option,strlen(option))==0)) {
      (*argv)[counter]=NULL;
      foundit = 1;
      break;
    }
  }

  ecmdRemoveNullPointers(argc, argv);
  return foundit;
}

/* ----------------------------------------------------------------- */
/* Function will parse for an option and eliminate it from a list    */
/* while returning a pointer to the option that is used.             */
/* If no option is available, the function will return NULL          */
/* ----------------------------------------------------------------- */
char * ecmdParseOptionWithArgs(int *argc, char **argv[], const char *option) {
  int counter = 0;
  int foundit = 0;
  char *returnValue=NULL;

  for (counter = 0; counter < *argc ; counter++) {
    if (((*argv)[counter] != NULL) && (strncmp((*argv)[counter],option,strlen(option))==0)) { 

      if (strlen((*argv)[counter])>strlen(option)) {
        returnValue = &((*argv)[counter][strlen(option)]);
        (*argv)[counter]=NULL;
      } else {
        if ((counter+1)<*argc) {
          returnValue = (*argv)[counter+1];
          (*argv)[counter]=NULL;
          (*argv)[counter+1]=NULL;
        } else {
          returnValue = NULL;
        }
      }
    }
  }

  ecmdRemoveNullPointers(argc, argv);

  return returnValue;
}

void ecmdParseTokens (std::string & line, std::vector<std::string> & tokens) {

  tokens.clear();
  bool done = false;
  std::string curToken;
  int curOffset = 0;
  bool nonSpace = false;

  for (int i = 0; i < line.length(); i++) {

    if (isspace(line[i]) && nonSpace) {
      tokens.push_back(line.substr(curOffset, i - curOffset));
      nonSpace = false;
    }
    else if (!isspace(line[i]) && !nonSpace) {
      nonSpace = true;
      curOffset = i;
    }

  }

  if (nonSpace) {
    tokens.push_back(line.substr(curOffset, line.length()));
  }

}


int ecmdConfigLooperInit (ecmdChipTarget & io_target, ecmdConfigLoopType_t i_looptype) {

  int rc = ECMD_SUCCESS;

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

  if (i_looptype == ECMD_SELECTED_TARGETS_LOOP) 
    rc = ecmdQuerySelected(queryTarget, ecmdSystemConfigData);
  else
    rc = ecmdQueryConfig(queryTarget, ecmdSystemConfigData);
  if (rc) return rc;

  ecmdCurCage = ecmdSystemConfigData.cageData.begin();
  ecmdLooperInitFlag = 1;

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug > 1) {
    std::string printed = "ECMD DEBUG (ecmdConfigLooperInit) : Exiting\n"; ecmdOutput(printed.c_str());
  }
#endif

  return rc;
}

int ecmdConfigLooperNext (ecmdChipTarget & io_target) {

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

  if (ecmdCurCage == ecmdSystemConfigData.cageData.end()) {
    return 0;
  }

  if (io_target.cage != (*ecmdCurCage).cageId || ecmdLooperInitFlag) {

    io_target.cage = (*ecmdCurCage).cageId;
    ecmdCurNode = (*ecmdCurCage).nodeData.begin();
    valid = 0;

    if (io_target.nodeState == ECMD_TARGET_FIELD_UNUSED || ecmdCurNode == (*ecmdCurCage).nodeData.end()) {
      io_target.node = 0;
    }
    else {
      level = NODE;
    }

  }
  else {
    level = NODE;
  }

  if (level == NODE && (!valid || io_target.node != (*ecmdCurNode).nodeId)) {

    io_target.node = (*ecmdCurNode).nodeId;
    ecmdCurSlot = (*ecmdCurNode).slotData.begin();
    valid = 0;
  
    if (io_target.nodeState == ECMD_TARGET_FIELD_UNUSED || ecmdCurSlot == (*ecmdCurNode).slotData.end()) {
      io_target.slot = 0;
    }
    else {
      level = SLOT;
    }

  }
  else if (valid) {
    level = SLOT;
  }

  if (level == SLOT && (!valid || io_target.slot != (*ecmdCurSlot).slotId)) {

    io_target.slot = (*ecmdCurSlot).slotId;
    ecmdCurChip = (*ecmdCurSlot).chipData.begin();
    valid = 0;
  
    if (io_target.chipTypeState == ECMD_TARGET_FIELD_UNUSED || io_target.posState == ECMD_TARGET_FIELD_UNUSED || ecmdCurChip == (*ecmdCurSlot).chipData.end()) {
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


  if (level == CHIP && (!valid || io_target.chipType != (*ecmdCurChip).chipType || io_target.pos != (*ecmdCurChip).pos)) {

    io_target.chipType = (*ecmdCurChip).chipType;
    io_target.pos = (*ecmdCurChip).pos;
    ecmdCurCore = (*ecmdCurChip).coreData.begin();
    valid = 0;

    if (io_target.coreState == ECMD_TARGET_FIELD_UNUSED || ecmdCurCore == (*ecmdCurChip).coreData.end()) {
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

  if (level == CORE && (!valid || io_target.core != (*ecmdCurCore).coreId)) {

    io_target.core = (*ecmdCurCore).coreId;
    ecmdCurThread = (*ecmdCurCore).threadData.begin();
    valid = 0;

    if (io_target.threadState == ECMD_TARGET_FIELD_UNUSED || ecmdCurThread == (*ecmdCurCore).threadData.end()) {
      io_target.thread = 0;
    }
    else {
      level = THREAD;
    }

  }
  else if (valid) {
    level = THREAD;
  }

  if (level == THREAD && (!valid || io_target.thread != (*ecmdCurThread).threadId)) {

    io_target.thread = (*ecmdCurThread).threadId;

  }

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug > 1) {
    std::string printed = "ECMD DEBUG (ecmdConfigLooperNext) : Found next Target : " + ecmdWriteTarget(io_target); printed += "\n";
    ecmdOutput(printed.c_str());
  }
#endif

  switch (level) {

    case THREAD:  //thread
      ecmdCurThread++;
      if (ecmdCurThread != (*ecmdCurCore).threadData.end()) {
        break;
      }

    case CORE:  //core
      ecmdCurCore++;
      if (ecmdCurCore != (*ecmdCurChip).coreData.end()) {
        break;
      }

    case CHIP:  //chip
      ecmdCurChip++;
      if (ecmdCurChip != (*ecmdCurSlot).chipData.end()) {
        break;
      }

    case SLOT:  //slot
      ecmdCurSlot++;
      if (ecmdCurSlot != (*ecmdCurNode).slotData.end()) {
        break;
      }

    case NODE:  //node
      ecmdCurNode++;
      if (ecmdCurNode != (*ecmdCurCage).nodeData.end()) {
        break;
      }

    case CAGE:  //cage
      ecmdCurCage++;
      break;

    default:
      //shouldn't get here
      break;
  }

  if (ecmdLooperInitFlag) {
    ecmdLooperInitFlag = 0;
  }

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug > 1) {
    std::string printed = "ECMD DEBUG (ecmdConfigLooperNext) : Exiting\n"; ecmdOutput(printed.c_str());
  }
#endif

  return 1;

}

int ecmdReadDataFormatted (ecmdDataBuffer & o_data, const char * i_dataStr, std::string & i_format, int i_expectedLength) {
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
    rc = o_data.insertFromHexRight(i_dataStr, i_expectedLength);
  }     
  else if (localFormat == "b") {
    o_data.setBitLength(strlen(i_dataStr));
    rc = o_data.insertFromBin(i_dataStr);
  }
  else {
    ecmdOutputError( ("Did not recognize input format string " + localFormat + "\n").c_str() );
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

      if (curState == ECMD_FORMAT_BN || curState == ECMD_FORMAT_BW || curState == ECMD_FORMAT_B) {
        printed += ecmdBitsHeader(4, blockSize, numCols);
      }

      printed += "\n00: ";
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

std::string ecmdBitsHeader(int initCharOffset, int blockSize, int numCols) {

  std::string topLine(initCharOffset, ' ');
  std::string bottomLine(initCharOffset, ' ');

  int bitsToPrint = blockSize * numCols;
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
