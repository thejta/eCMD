// Copyright ***********************************************************
//                                                                      
// File ecmdDllCapi.C                                   
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

/* $Header$ */

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define ecmdDllCapi_C
#include <stdio.h>
#include <list>
#include <algorithm>
#include <fstream>
#include <string.h>
#include <stdlib.h>

#include <ecmdDllCapi.H>
#include <ecmdStructs.H>
#include <ecmdSharedUtils.H>

#undef ecmdDllCapi_C
//----------------------------------------------------------------------
//  User Types
//----------------------------------------------------------------------

/** @brief Used to hold info out of the scandef for get/putlatch etc. */
struct ecmdLatchInfo {
  std::string ringName;                 ///< Name of ring that contains this latch
  std::string latchName;                ///< Full Latch Name (including any parens)
  uint32_t ringOffset;                  ///< Ring Offset
  uint32_t length;                      ///< Length of entry
  uint32_t latchStartBit;               ///< Start bit in latch (comes from parens in latch name) 
  uint32_t latchEndBit;                 ///< End bit in latch (comes from parens in latch name) 

};

/** @brief Used to buffer scandef data to avoid searching for each chip ec */
struct ecmdLatchBufferEntry {
  std::list<ecmdLatchInfo> entry;       ///< Data from Scandef
  std::string scandefName;              ///< Name of scandef where data was retrieved
  std::string latchName;                ///< Latch name used to search for this data
  std::string ringName;                 ///< Ring name used to search for this data (empty string if == NULL)
};




/**
 * @brief Used for storing error messages internally to dll
 */
struct ecmdError {
  uint32_t errorCode;        ///< Numeric error code- see ecmdReturnCodes.H
  std::string whom;     ///< Function that registered error
  std::string message;  ///< Message about the error
};

std::list<ecmdError> ecmdErrorList;

struct ecmdUserInfo {

  std::string cage;
  std::string node;
  std::string slot;
  std::string pos;
  std::string core;
  std::string thread;

  bool allTargetSpecified;

} ecmdUserArgs;


//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------
#ifdef ENABLE_MPATROL
#ifdef _AIX
/* This is to fix a missing symbol problem when compiling on aix with mpatrol */
char **p_xargv;
#endif
#endif

//----------------------------------------------------------------------
//  Macros
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Internal Function Prototypes
//----------------------------------------------------------------------

uint32_t dllReadScandef(ecmdChipTarget & target, const char* i_ringName, const char* i_latchName, ecmdLatchMode_t i_mode, ecmdLatchBufferEntry & o_latchdata);

/** @brief Used to sort latch entries from the scandef */
bool operator< (const ecmdLatchInfo & lhs, const ecmdLatchInfo & rhs) {


  if (lhs.ringName != rhs.ringName)
    return lhs.ringName < rhs.ringName;

  int lhsLeftParen = lhs.latchName.find('(');
  int rhsLeftParen = rhs.latchName.find('(');

  if (lhsLeftParen == std::string::npos || rhsLeftParen == std::string::npos || lhsLeftParen != rhsLeftParen) {
    return lhs.latchName < rhs.latchName;
  }

  std::string lhsSub = lhs.latchName.substr(0, lhsLeftParen);
  std::string rhsSub = rhs.latchName.substr(0, rhsLeftParen);

  if (lhsSub != rhsSub) {
    return lhs.latchName < rhs.latchName;
  }

  return lhs.latchStartBit < rhs.latchStartBit;
}

/** @brief Used to sort latch entries from the scandef */
bool operator!= (const ecmdLatchInfo & lhs, const ecmdLatchInfo & rhs) {

  if (lhs.ringName != rhs.ringName)
    return true;

  int lhsLeftParen = lhs.latchName.find('(');
  int rhsLeftParen = rhs.latchName.find('(');

  if (lhsLeftParen != rhsLeftParen) {
    return true;
  }

  return (lhs.latchName.substr(0, lhsLeftParen) != rhs.latchName.substr(0,rhsLeftParen));
}


uint8_t dllRemoveCurrentElement(int curPos, std::string userArgs);

/* Returns true if all chars of str are decimal numbers */
bool dllIsValidTargetString(std::string str);

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------
#ifndef ECMD_STRIP_DEBUG
/* @brief This is used to output Debug messages on the DLL side */
uint32_t ecmdGlobal_DllDebug = 0;
#endif
/* @brief This is a global var set by -quiet */
uint32_t ecmdGlobal_quiet = 0;

/* @brief Used by get/putlatch to buffer scandef entries in memory to improve performance */
std::list<ecmdLatchBufferEntry> latchBuffer;

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------

uint32_t dllLoadDll (const char* i_clientVersion, uint32_t debugLevel) {

  /* First off let's check our version */
  /* Let's found our '.' char because we only fail if the Major number changes */
  int majorlength = (int)(strchr(i_clientVersion, '.') - i_clientVersion);

  if (strncmp(i_clientVersion,ECMD_CAPI_VERSION,majorlength)) {
    fprintf(stderr,"**** FATAL : eCMD DLL and your client Major version numbers don't match, they are not compatible\n");
    fprintf(stderr,"**** FATAL : Client Version : %s   : DLL Version : %s\n",i_clientVersion, ECMD_CAPI_VERSION);

    if (atoi(i_clientVersion) < atoi(ECMD_CAPI_VERSION)) {
      fprintf(stderr,"**** FATAL : Your client is older then the eCMD Dll Plugin you are running\n");
      fprintf(stderr,"**** FATAL : You must grab the latest client libraries and rebuild your client to continue\n");
      fprintf(stderr,"**** FATAL : Information on where to obtain these files is at http://rhea.rchland.ibm.com/eCMD/\n");
    } else {
      fprintf(stderr,"**** FATAL : It appears your client is newer then the eCMD Dll Plugin you are running\n");
      fprintf(stderr,"**** FATAL : Contact the eCMD team to have the Plugin rebuilt to match your client\n");
      fprintf(stderr,"**** FATAL : Or get ahold of down level client libraries and rebuild your client to match\n");
      fprintf(stderr,"**** FATAL : Contact information can be found at http://rhea.rchland.ibm.com/eCMD/\n");

    }

    exit(999);
  }

  /* Now we are going to check the version of the shared lib we loaded */
  if (strncmp(ecmdGetSharedLibVersion().c_str(),ECMD_CAPI_VERSION,majorlength)) {
    fprintf(stderr,"**** FATAL : eCMD Shared Library and your Plugin Major version numbers don't match, they are not compatible\n");
    fprintf(stderr,"**** FATAL : Shared Library Version : %s   : DLL Version : %s\n",ecmdGetSharedLibVersion().c_str(), ECMD_CAPI_VERSION);

    if (atoi(ecmdGetSharedLibVersion().c_str()) < atoi(ECMD_CAPI_VERSION)) {
      fprintf(stderr,"**** FATAL : Your shared library is older then the eCMD Dll Plugin you are running\n");
      fprintf(stderr,"**** FATAL : You must grab the latest library to continue\n");
      fprintf(stderr,"**** FATAL : Information on where to obtain these files is at http://rhea.rchland.ibm.com/eCMD/\n");
    } else {
      fprintf(stderr,"**** FATAL : It appears your shared library is newer then the eCMD Dll Plugin you are running\n");
      fprintf(stderr,"**** FATAL : Contact the eCMD team to have the Plugin rebuilt to match\n");
      fprintf(stderr,"**** FATAL : Or get ahold of a down level shared library and rerun\n");
      fprintf(stderr,"**** FATAL : Contact information can be found at http://rhea.rchland.ibm.com/eCMD/\n");

    }

    exit(999);
  }


  ecmdGlobal_DllDebug = debugLevel;

#ifndef ECMD_STRIP_DEBUG
  if (ecmdGlobal_DllDebug > 0) {
    printf("ECMD DEBUG : Client Version     '%s'\n", i_clientVersion);
    printf("ECMD DEBUG : Plugin Version     '%s'\n", ECMD_CAPI_VERSION);
    printf("ECMD DEBUG : Shared Lib Version '%s'\n", ecmdGetSharedLibVersion().c_str());
  }
#endif

  return dllInitDll();

}

uint32_t dllUnloadDll() {
  uint32_t rc = 0;
  
  rc = dllFreeDll();

  return rc;
}


std::string dllGetErrorMsg(uint32_t i_errorCode) {
  std::string ret;
  std::list<ecmdError>::iterator cur;

  for (cur = ecmdErrorList.begin(); cur != ecmdErrorList.end(); cur++) {
    if ( (*cur).errorCode == i_errorCode ) {
      ret  = "====== EXTENDED ERROR MSG : " + (*cur).whom + " ===============\n";
      ret += (*cur).message;
      ret += "==============================================================\n";
      break;
    }
  }

  return ret;
}

uint32_t dllRegisterErrorMsg(uint32_t i_errorCode, const char* i_whom, const char* i_message) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdError curError;
  curError.errorCode = i_errorCode;
  curError.whom = i_whom;
  curError.message = i_message;

  ecmdErrorList.push_back(curError);

  return rc;
}

uint32_t dllQuerySelected(ecmdChipTarget & i_target, ecmdQueryData & o_queryData) {
  uint32_t rc = ECMD_SUCCESS;

  uint8_t SINGLE = 0;
  uint8_t MULTI = 1;
  uint8_t ALL = 2;

  uint8_t cageType;
  uint8_t nodeType;
  uint8_t slotType;
  uint8_t posType;
  uint8_t coreType;
  uint8_t threadType;

  std::string allFlag = "all";
  std::string patterns = ",.";

  //update target with useful info in the ecmdUserArgs struct
  //cage
  if ((ecmdUserArgs.allTargetSpecified == true) || (ecmdUserArgs.cage == allFlag)) {
    i_target.cageState = ECMD_TARGET_QUERY_WILDCARD;
    cageType = ALL;
  }
  else if (ecmdUserArgs.cage.find_first_of(patterns) < ecmdUserArgs.cage.length()) {
    if (!dllIsValidTargetString(ecmdUserArgs.cage)) {
      dllOutputError("dllQuerySelected - Cage (-k#) argument contained invalid characters\n");
      return ECMD_INVALID_ARGS;
    }
    i_target.cageState = ECMD_TARGET_QUERY_WILDCARD;
    cageType = MULTI;
  }
  else {

    if (ecmdUserArgs.cage.length() != 0) {
      if (!dllIsValidTargetString(ecmdUserArgs.cage)) {
        dllOutputError("dllQuerySelected - Cage (-k#) argument contained invalid characters\n");
        return ECMD_INVALID_ARGS;
      }
      i_target.cage = atoi(ecmdUserArgs.cage.c_str());
    }
    else {
      i_target.cage = 0x0;
    }

    cageType = SINGLE;
    i_target.cageState = ECMD_TARGET_QUERY_FIELD_VALID;
  }

  //node
  if ((ecmdUserArgs.allTargetSpecified == true) || (ecmdUserArgs.node == allFlag)) {
    i_target.nodeState = ECMD_TARGET_QUERY_WILDCARD;
    nodeType = ALL;
  }
  else if (ecmdUserArgs.node.find_first_of(patterns) < ecmdUserArgs.node.length()) {
    if (!dllIsValidTargetString(ecmdUserArgs.node)) {
      dllOutputError("dllQuerySelected - Node (-n#) argument contained invalid characters\n");
      return ECMD_INVALID_ARGS;
    }
    i_target.nodeState = ECMD_TARGET_QUERY_WILDCARD;
    nodeType = MULTI;
  }
  else {

    if (ecmdUserArgs.node.length() != 0) {
      if (!dllIsValidTargetString(ecmdUserArgs.node)) {
        dllOutputError("dllQuerySelected - Node (-n#) argument contained invalid characters\n");
        return ECMD_INVALID_ARGS;
      }
      i_target.node = atoi(ecmdUserArgs.node.c_str());
    }
    else {
      i_target.node = 0x0;
    }

    nodeType = SINGLE;
    i_target.nodeState = ECMD_TARGET_QUERY_FIELD_VALID;
  }

  //slot
  if ((ecmdUserArgs.allTargetSpecified == true) || (ecmdUserArgs.slot == allFlag)) {
    i_target.slotState = ECMD_TARGET_QUERY_WILDCARD;
    slotType = ALL;
  }
  else if (ecmdUserArgs.slot.find_first_of(patterns) < ecmdUserArgs.slot.length()) {
    if (!dllIsValidTargetString(ecmdUserArgs.slot)) {
      dllOutputError("dllQuerySelected - Slot (-s#) argument contained invalid characters\n");
      return ECMD_INVALID_ARGS;
    }
    i_target.slotState = ECMD_TARGET_QUERY_WILDCARD;
    slotType = MULTI;
  }
  else {

    if (ecmdUserArgs.slot.length() != 0) {
      if (!dllIsValidTargetString(ecmdUserArgs.slot)) {
        dllOutputError("dllQuerySelected - Slot (-s#) argument contained invalid characters\n");
        return ECMD_INVALID_ARGS;
      }
      i_target.slot = atoi(ecmdUserArgs.slot.c_str());
    }
    else {
      i_target.slot = 0x0;
    }

    slotType = SINGLE;
    i_target.slotState = ECMD_TARGET_QUERY_FIELD_VALID;
  }

  //position
  if ((ecmdUserArgs.allTargetSpecified == true) || (ecmdUserArgs.pos == allFlag)) {
    posType = ALL;
    i_target.posState = ECMD_TARGET_QUERY_WILDCARD;
  }
  else if (ecmdUserArgs.pos.find_first_of(patterns) < ecmdUserArgs.pos.length()) {
    if (!dllIsValidTargetString(ecmdUserArgs.pos)) {
      dllOutputError("dllQuerySelected - Position (-p#) argument contained invalid characters\n");
      return ECMD_INVALID_ARGS;
    }
    posType = MULTI;
    i_target.posState = ECMD_TARGET_QUERY_WILDCARD;
  }
  else {

    if (ecmdUserArgs.pos.length() != 0) {
      if (!dllIsValidTargetString(ecmdUserArgs.pos)) {
        dllOutputError("dllQuerySelected - Position (-p#) argument contained invalid characters\n");
        return ECMD_INVALID_ARGS;
      }
      i_target.pos = atoi(ecmdUserArgs.pos.c_str());
    }
    else {
      i_target.pos = 0x0;
    }

    posType = SINGLE;
    i_target.posState = ECMD_TARGET_QUERY_FIELD_VALID;
  }

  //core
  if ((ecmdUserArgs.allTargetSpecified == true) || (ecmdUserArgs.core == allFlag)) {
    i_target.coreState = ECMD_TARGET_QUERY_WILDCARD;
    coreType = ALL;
  }
  else if (ecmdUserArgs.core.find_first_of(patterns) < ecmdUserArgs.core.length()) {
    if (!dllIsValidTargetString(ecmdUserArgs.core)) {
      dllOutputError("dllQuerySelected - Core (-c#) argument contained invalid characters\n");
      return ECMD_INVALID_ARGS;
    }
    i_target.coreState = ECMD_TARGET_QUERY_WILDCARD;
    coreType = MULTI;
  }
  else {

    if (ecmdUserArgs.core.length() != 0) {
      if (!dllIsValidTargetString(ecmdUserArgs.core)) {
        dllOutputError("dllQuerySelected - Core (-c#) argument contained invalid characters\n");
        return ECMD_INVALID_ARGS;
      }
      i_target.core = atoi(ecmdUserArgs.core.c_str());
    }
    else {
      i_target.core = 0x0;
    }

    coreType = SINGLE;
    i_target.coreState = ECMD_TARGET_QUERY_FIELD_VALID;
  }

  //thread
  if ((ecmdUserArgs.allTargetSpecified == true) || (ecmdUserArgs.thread == allFlag) || (ecmdUserArgs.thread == "alive")) {
    i_target.threadState = ECMD_TARGET_QUERY_WILDCARD;
    threadType = ALL;
  }
  else if (ecmdUserArgs.thread.find_first_of(patterns) < ecmdUserArgs.thread.length()) {
    if (!dllIsValidTargetString(ecmdUserArgs.thread)) {
      dllOutputError("dllQuerySelected - Thread (-t#) argument contained invalid characters\n");
      return ECMD_INVALID_ARGS;
    }
    i_target.threadState = ECMD_TARGET_QUERY_WILDCARD;
    threadType = MULTI;    
  }
  else {

    if (ecmdUserArgs.thread.length() != 0) {
      if (!dllIsValidTargetString(ecmdUserArgs.thread)) {
        dllOutputError("dllQuerySelected - Thread (-t#) argument contained invalid characters\n");
        return ECMD_INVALID_ARGS;
      }
      i_target.thread = atoi(ecmdUserArgs.thread.c_str());
    }
    else {
      i_target.thread = 0x0;
    }

    threadType = SINGLE;
    i_target.threadState = ECMD_TARGET_QUERY_FIELD_VALID;
  }


  /* Okay, target setup as best we can, let's go out to query cnfg with it */
  rc = dllQueryConfig(i_target, o_queryData, ECMD_QUERY_DETAIL_LOW);


  /* now I need to go in and clean out any excess stuff */
  std::list<ecmdCageData>::iterator curCage = o_queryData.cageData.begin();

  while (curCage != o_queryData.cageData.end()) {

    if (cageType == MULTI) {
      if (dllRemoveCurrentElement((*curCage).cageId, ecmdUserArgs.cage)) {
        curCage = o_queryData.cageData.erase(curCage);
        continue;
      }
    }

    std::list<ecmdNodeData>::iterator curNode = (*curCage).nodeData.begin();

    while (curNode != (*curCage).nodeData.end()) {

      if (nodeType == MULTI) {
        if (dllRemoveCurrentElement((*curNode).nodeId, ecmdUserArgs.node)) {
          curNode = (*curCage).nodeData.erase(curNode);
          continue;
        }
      }

      std::list<ecmdSlotData>::iterator curSlot = (*curNode).slotData.begin();

      while (curSlot != (*curNode).slotData.end()) {

        if (slotType == MULTI) {
          if (dllRemoveCurrentElement((*curSlot).slotId, ecmdUserArgs.slot)) {
            curSlot = (*curNode).slotData.erase(curSlot);
            continue;
          }
        }

        std::list<ecmdChipData>::iterator curChip = (*curSlot).chipData.begin();

        while (curChip != (*curSlot).chipData.end()) {

          if (posType == MULTI) {
            if (dllRemoveCurrentElement((*curChip).pos, ecmdUserArgs.pos)) {
              curChip = (*curSlot).chipData.erase(curChip);
              continue;
            }
          }

          std::list<ecmdCoreData>::iterator curCore = (*curChip).coreData.begin();

          while (curCore != (*curChip).coreData.end()) {

            if (coreType == MULTI) {
              if (dllRemoveCurrentElement((*curCore).coreId, ecmdUserArgs.core)) {
                curCore = (*curChip).coreData.erase(curCore);
                continue;
              }
            }

            std::list<ecmdThreadData>::iterator curThread = (*curCore).threadData.begin();

            while (curThread != (*curCore).threadData.end()) {

              if (threadType == MULTI) {
                if (dllRemoveCurrentElement((*curThread).threadId, ecmdUserArgs.thread)) {
                  curThread = (*curCore).threadData.erase(curThread);
                  continue;
                }
              }

              curThread++;
            }  /* while curThread */

            curCore++;
          }  /* while curCore */

          curChip++; 
        }  /* while curChip */

        curSlot++;
      }  /* while curSlot */

      curNode++;
    }  /* while curNode */

    curCage++; 
  }  /* while curCage */

  return rc;

}

uint32_t dllCommonCommandArgs(int*  io_argc, char** io_argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  /* We need to pull out the targeting options here, and
   store them away for future use */
  char * curArg;

  //-trace
  if ((curArg = ecmdParseOptionWithArgs(io_argc, io_argv, "-trace="))) {
    /* Grab all the tokens */
    std::vector<std::string> tokens;
    std::vector<std::string>::iterator tokit;

    ecmdParseTokens(curArg," \t\n,", tokens);
    for (tokit = tokens.begin(); tokit != tokens.end(); tokit ++) {
      if (!strcmp(tokit->c_str(), "scan")) {
        dllSetTraceMode(ECMD_TRACE_SCAN,true);
      } else if (!strcmp(tokit->c_str(), "prcd")) {
        dllSetTraceMode(ECMD_TRACE_PROCEDURE,true);
      } else {
        dllOutputWarning(("dllCommonCommandArgs - Unknown Trace type detected '" + *tokit + "' : Ignoring\n").c_str());
      }

    }
  }


  ecmdUserArgs.allTargetSpecified = false;
  if (ecmdParseOption(io_argc, io_argv, "-all"))
    ecmdUserArgs.allTargetSpecified = true;
    

  //cage - the "-k" was Larry's idea, I just liked it - jtw
  curArg = ecmdParseOptionWithArgs(io_argc, io_argv, "-k");
  if ((ecmdUserArgs.allTargetSpecified == true) && curArg) {
    dllOutputError("dllCommonCommandArgs - Cannot specify -all and -k# at the same time\n");
    return ECMD_INVALID_ARGS;
  } else if (curArg != NULL) {
    ecmdUserArgs.cage = curArg;
  }
  else {
    ecmdUserArgs.cage = "";
  }

  //node
  curArg = ecmdParseOptionWithArgs(io_argc, io_argv, "-n");
  if ((ecmdUserArgs.allTargetSpecified == true) && curArg) {
    dllOutputError("dllCommonCommandArgs - Cannot specify -all and -n# at the same time\n");
    return ECMD_INVALID_ARGS;
  } else if (curArg != NULL) {
    ecmdUserArgs.node = curArg;
  }
  else {
    ecmdUserArgs.node = "";
  }

  //slot
  curArg = ecmdParseOptionWithArgs(io_argc, io_argv, "-s");
    if ((ecmdUserArgs.allTargetSpecified == true) && curArg) {
    dllOutputError("dllCommonCommandArgs - Cannot specify -all and -s# at the same time\n");
    return ECMD_INVALID_ARGS;
  } else if (curArg != NULL) {
    ecmdUserArgs.slot = curArg;
  }
  else {
    ecmdUserArgs.slot = "";
  }

  //position
  curArg = ecmdParseOptionWithArgs(io_argc, io_argv, "-p");
  if ((ecmdUserArgs.allTargetSpecified == true) && curArg) {
    dllOutputError("dllCommonCommandArgs - Cannot specify -all and -p# at the same time\n");
    return ECMD_INVALID_ARGS;
  } else if (curArg != NULL) {
    ecmdUserArgs.pos = curArg;
  }
  else {
    ecmdUserArgs.pos = "";
  }

  //core
  curArg = ecmdParseOptionWithArgs(io_argc, io_argv, "-c");
  if ((ecmdUserArgs.allTargetSpecified == true) && curArg) {
    dllOutputError("dllCommonCommandArgs - Cannot specify -all and -c# at the same time\n");
    return ECMD_INVALID_ARGS;
  } else if (curArg != NULL) {
    ecmdUserArgs.core = curArg;
  }
  else {
    ecmdUserArgs.core = "";
  }

  //thread
  curArg = ecmdParseOptionWithArgs(io_argc, io_argv, "-t");
  if ((ecmdUserArgs.allTargetSpecified == true) && curArg) {
    dllOutputError("dllCommonCommandArgs - Cannot specify -all and -t# at the same time\n");
    return ECMD_INVALID_ARGS;
  } else if (curArg != NULL) {
    ecmdUserArgs.thread = curArg;
  }
  else {
    ecmdUserArgs.thread = "";
  }

  /* Grab the quiet mode flag */
  if (ecmdParseOption(io_argc, io_argv, "-quiet"))
    ecmdGlobal_quiet = 1;



  /* Call the dllSpecificFunction */
  rc = dllSpecificCommandArgs(io_argc,io_argv);

  return rc;
}


uint8_t dllRemoveCurrentElement (int curPos, std::string userArgs) {
  uint8_t remove = 1;

  std::string curSubstr;
  int curOffset = 0;
  int nextOffset = 0;
  int tmpOffset = 0;

  while (curOffset < userArgs.length()) {

    nextOffset = userArgs.find(',',curOffset);
    if (nextOffset == std::string::npos) {
      nextOffset = userArgs.length();
    }

    curSubstr = userArgs.substr(curOffset, nextOffset - curOffset);

    if ((tmpOffset = curSubstr.find("..",0)) < curSubstr.length()) {

      int lowerBound = atoi(curSubstr.substr(0,tmpOffset).c_str());
      int upperBound = atoi(curSubstr.substr(tmpOffset+2, curSubstr.length()).c_str());

      if (lowerBound <= curPos && curPos <= upperBound) {
        remove = 0;
        break;
      }
    }
    else {

      int curValidPos = atoi(curSubstr.c_str());
      if (curValidPos == curPos) {
        remove = 0;
        break;
      }
    }

    curOffset = nextOffset+1;

  }

  return remove;
}

/* Returns true if all chars of str are decimal numbers */
bool dllIsValidTargetString(std::string str) {

  bool ret = true;
  for (int x = 0; x < str.length(); x ++) {
    if (isdigit(str[x])) {
    } else if (str[x] == ',') {
    } else if (str[x] == '.' && str[x+1] == '.') {
      x++;
    } else {
      ret = false;
      break;
    }
  }

  return ret;
}

uint32_t dllGetGlobalVar(ecmdGlobalVarType_t i_type) {

  uint32_t ret = 0;

  if (i_type == ECMD_GLOBALVAR_DEBUG) {
#ifndef ECMD_STRIP_DEBUG
    ret = ecmdGlobal_DllDebug;
#endif
  } else if (i_type == ECMD_GLOBALVAR_QUIETMODE) {
    ret = ecmdGlobal_quiet;
  }

  return ret;
}

uint32_t dllGetLatch(ecmdChipTarget & target, const char* i_ringName, const char * i_latchName, std::list<ecmdLatchEntry> & o_data, ecmdLatchMode_t i_mode) {
  uint32_t rc = 0;

  ecmdLatchBufferEntry curEntry;
  std::list< ecmdLatchInfo >::iterator curLatchInfo;    ///< Iterator for walking through latches
  ecmdDataBuffer ringBuffer;                    ///< Buffer to store entire ring
  ecmdDataBuffer buffer(100 /* words */);       ///< Space for extracted latch data
  ecmdDataBuffer buffertemp(100 /* words */);   ///< Temp space for extracted latch data
  bool enabledCache = false;                    ///< This is turned on if we enabled the cache, so we can disable on exit
  ecmdLatchEntry curData;                       ///< Data to load into return list
  std::string curRing;                          ///< Current ring being operated on

  if (!dllIsRingCacheEnabled()) {
    enabledCache = true;
    dllEnableRingCache();
  }

  rc = dllReadScandef(target, i_ringName, i_latchName, i_mode, curEntry);
  if (rc) return rc;

  /* Single exit point */
  while (1) {

    uint32_t bitsToFetch = 0x0FFFFFFF;  // Grab all bits
    int curLatchBit = -1;               // This is the actual latch bit we are looking for next
    int curStartBit = 0;                // This is the offset into the current entry to start extraction
    int curBufferBit = 0;               // Current bit to insert into buffered register
    int curBitsToFetch = bitsToFetch;   // This is the total number of bits left to fetch
    int dataStartBit = -1;              // Actual start bit of buffered register
    int dataEndBit = -1;                // Actual end bit of buffered register
    std::string latchname = "";
    curRing = "";

    for (curLatchInfo = curEntry.entry.begin(); (curLatchInfo != curEntry.entry.end()) && (curBitsToFetch > 0); curLatchInfo++) {



      /* Let's grab the ring for this latch entry */
      if (curRing != curLatchInfo->ringName) {
        rc = dllGetRing(target, curLatchInfo->ringName.c_str(), ringBuffer);
        if (rc) {
#ifndef ECMD_STRIP_DEBUG
          if (ecmdGlobal_DllDebug) 
            dllOutputError("dllGetLatch - Problems reading ring from chip\n");
#endif
          break;
        }
        curRing = curLatchInfo->ringName;
      }
      /* Do we have previous data here , or some missing bits in the scandef latchs ?*/
      if (((dataStartBit != -1) && (curLatchBit != curLatchInfo->latchStartBit) && (curLatchBit != curLatchInfo->latchEndBit)) ||
          ((latchname == "") || (latchname.substr(0, latchname.find('(')) != curLatchInfo->latchName.substr(0, curLatchInfo->latchName.find('('))))) {
        /* I have some good data here */
        if (latchname != "") {

          /* Display this if we aren't expecting or the expect failed */
          char temp[20];
          curData.latchStartBit = dataStartBit;
          curData.latchEndBit = dataEndBit;
          buffer.extract(curData.buffer, 0, dataEndBit - dataStartBit + 1);
          curData.rc = ECMD_SUCCESS;
          o_data.push_back(curData);

        }

        /* If this is a fresh one we need to reset everything */
        if ((latchname == "") || (latchname != curLatchInfo->latchName.substr(0, latchname.length()))) {
          dataStartBit = dataEndBit = -1;
          curStartBit = 0;
          curBitsToFetch = 0x0FFFFFFF;
          curBufferBit = 0;
          latchname = curLatchInfo->latchName.substr(0, curLatchInfo->latchName.find('('));
          curLatchBit = curLatchInfo->latchStartBit < curLatchInfo->latchEndBit ? curLatchInfo->latchStartBit : curLatchInfo->latchEndBit;
          curData.latchName = latchname;
          curData.ringName = curLatchInfo->ringName;
        } else {
          /* This is the case where the scandef had holes in the register, so we will continue with this latch, but skip some bits */
          dataStartBit = dataEndBit = -1;
          curBufferBit = 0;
          /* Decrement the bits to fetch by the hole in the latch */
          curBitsToFetch -= (curLatchInfo->latchStartBit < curLatchInfo->latchEndBit ? curLatchInfo->latchStartBit : curLatchInfo->latchEndBit) - curLatchBit;
          curLatchBit = curLatchInfo->latchStartBit < curLatchInfo->latchEndBit ? curLatchInfo->latchStartBit : curLatchInfo->latchEndBit;
        }
      }

      /* Do we want anything in here */
      /* Check if the bits are ordered from:to (0:10) or just (1) */
      if (((curLatchInfo->latchEndBit >= curLatchInfo->latchStartBit) && (curStartBit <= curLatchInfo->latchEndBit) && (curLatchBit <= curLatchInfo->latchEndBit)) ||
          /* Check if the bits are ordered to:from (10:0) */
          ((curLatchInfo->latchStartBit > curLatchInfo->latchEndBit) && (curStartBit <= curLatchInfo->latchStartBit) && (curLatchBit <= curLatchInfo->latchStartBit))) {

        bitsToFetch = ((curLatchInfo->length - curStartBit) < curBitsToFetch) ? curLatchInfo->length - curStartBit : curBitsToFetch;

        /* Setup the actual data bits displayed */
        if (dataStartBit == -1) {
          dataStartBit = curLatchBit + curStartBit;
          dataEndBit = dataStartBit - 1;
        }
        dataEndBit += bitsToFetch;


        /* Extract bits if ordered from:to (0:10) */
        if (curLatchInfo->latchEndBit > curLatchInfo->latchStartBit) {
          ringBuffer.extract(buffertemp, curLatchInfo->ringOffset + curStartBit, bitsToFetch);
          buffer.insert(buffertemp, curBufferBit, bitsToFetch); curBufferBit += bitsToFetch;

          curLatchBit = curLatchInfo->latchEndBit + 1;
        } else {
          /* Extract if bits are ordered to:from (10:0) or just (1) */
          for (int bit = 0; bit < bitsToFetch; bit ++) {
            if (ringBuffer.isBitSet(curLatchInfo->ringOffset + (curLatchInfo->length - 1) - curStartBit - bit))
              buffer.setBit(curBufferBit++);
            else
              buffer.clearBit(curBufferBit++);
          }

          curLatchBit = curLatchInfo->latchStartBit + 1;

        }

        curStartBit = 0;
        curBitsToFetch -= bitsToFetch;
      } else {
        /* Nothing was there that we needed, let's try the next entry */
        curLatchBit = curLatchInfo->latchStartBit < curLatchInfo->latchEndBit ? curLatchInfo->latchEndBit + 1: curLatchInfo->latchStartBit + 1;

        curStartBit -= curLatchInfo->length;
      }

    } /* end latchinfo for loop */

    /* We have good data here, let's push it */
    if (latchname != "") {

      /* Display this if we aren't expecting or the expect failed */
      char temp[20];
      curData.latchStartBit = dataStartBit;
      curData.latchEndBit = dataEndBit;
      buffer.extract(curData.buffer, 0, dataEndBit - dataStartBit + 1);
      curData.rc = ECMD_SUCCESS;
      o_data.push_back(curData);

    }

    break;
  } /* end while (exit point) */

  if (enabledCache) {
    rc = dllDisableRingCache();
  }

  return rc;
}

uint32_t dllPutLatch(ecmdChipTarget & i_target, const char* i_ringName, const char * i_latchName, ecmdDataBuffer & i_data, ecmdLatchMode_t i_mode) {

  uint32_t rc = 0;
  ecmdLatchBufferEntry curEntry;
  std::list< ecmdLatchInfo >::iterator curLatchInfo;
  ecmdDataBuffer ringBuffer;            ///< Buffer to store entire ring
  ecmdDataBuffer bufferCopy;            ///< Copy of data to be inserted
  std::string curRing;                  ///< Current ring being operated on
  std::string curLatchName;             ///< Current latch name being operated on

//  if (!dllIsRingCacheEnabled()) {
//    enabledCache = true;
//    dllEnableRingCache();
//  }

  rc = dllReadScandef(i_target, i_ringName, i_latchName, i_mode, curEntry);
  if (rc) return rc;

  /* single exit point */
  while (1) {



    uint32_t bitsToInsert = i_data.getBitLength();
    int curLatchBit = -1;               // This is the actual latch bit we are looking for next
    int curStartBit = 0;         // This is the offset into the current entry to start extraction
    int curBitsToInsert = bitsToInsert;      // This is the total number of bits left to Insert

    for (curLatchInfo = curEntry.entry.begin(); curLatchInfo != curEntry.entry.end(); curLatchInfo++) {

      /* Let's grab the ring for this latch entry */
      if (curRing != curLatchInfo->ringName) {
        if (curRing.length() > 0) {
          rc = dllPutRing(i_target, curRing.c_str(), ringBuffer);
          if (rc) {
#ifndef ECMD_STRIP_DEBUG
            if (ecmdGlobal_DllDebug) 
              dllOutputError("dllPutLatch - Problems writing ring into chip\n");
#endif
            break;
          }
        }
          
        rc = dllGetRing(i_target, curLatchInfo->ringName.c_str(), ringBuffer);
        if (rc) {
#ifndef ECMD_STRIP_DEBUG
          if (ecmdGlobal_DllDebug) 
            dllOutputError("dllPutLatch - Problems reading ring from chip\n");
#endif
          break;
        }
        curRing = curLatchInfo->ringName;
      }

      if ((curLatchBit == -1) || (curLatchName != curLatchInfo->latchName.substr(0, curLatchInfo->latchName.find('(')))) {
        curLatchBit = curLatchInfo->latchStartBit < curLatchInfo->latchEndBit ? curLatchInfo->latchStartBit : curLatchInfo->latchEndBit;
        curLatchName = curLatchInfo->latchName.substr(0, curLatchInfo->latchName.find('('));
        /* Make a copy of insert data so we don't lose it */
        bufferCopy = i_data;
      }

      /* Check if the bits are ordered from:to (0:10) or just (1) */
      if (((curLatchInfo->latchEndBit >= curLatchInfo->latchStartBit) && (curStartBit <= curLatchInfo->latchEndBit) && (curLatchBit <= curLatchInfo->latchEndBit)) ||
          /* Check if the bits are ordered to:from (10:0) */
          ((curLatchInfo->latchStartBit > curLatchInfo->latchEndBit) && (curStartBit <= curLatchInfo->latchStartBit) && (curLatchBit <= curLatchInfo->latchStartBit))) {

        bitsToInsert = ((curLatchInfo->length - curStartBit) < curBitsToInsert) ? curLatchInfo->length - curStartBit : curBitsToInsert;


        /* Extract bits if ordered from:to (0:10) */
        if (curLatchInfo->latchEndBit > curLatchInfo->latchStartBit) {

          ringBuffer.insert(bufferCopy, curLatchInfo->ringOffset + curStartBit, bitsToInsert);
          /* Get rid of the data we just inserted, to line up the next piece */
          bufferCopy.shiftLeft(bitsToInsert);

          curLatchBit = curLatchInfo->latchEndBit + 1;
        } else {
          /* Extract if bits are ordered to:from (10:0) or just (1) */
          for (int bit = 0; bit < bitsToInsert; bit ++) {
            if (bufferCopy.isBitSet(bit)) 
              ringBuffer.setBit(curLatchInfo->ringOffset + (curLatchInfo->length - 1) - curStartBit - bit);
            else
              ringBuffer.clearBit(curLatchInfo->ringOffset + (curLatchInfo->length - 1) - curStartBit - bit);
          }
          /* Get rid of the data we just inserted, to line up the next piece */
          bufferCopy.shiftLeft(bitsToInsert);

          curLatchBit = curLatchInfo->latchStartBit + 1;

        }

        curStartBit = 0;
        curBitsToInsert -= bitsToInsert;
      } else {
        /* Nothing was there that we needed, let's try the next entry */
        curLatchBit = curLatchInfo->latchStartBit < curLatchInfo->latchEndBit ? curLatchInfo->latchEndBit + 1: curLatchInfo->latchStartBit + 1;

        curStartBit -= curLatchInfo->length;
      }

    }

    if (curRing.length() > 0) {
      rc = dllPutRing(i_target, curRing.c_str(), ringBuffer);
      if (rc) {
#ifndef ECMD_STRIP_DEBUG
        if (ecmdGlobal_DllDebug) 
          dllOutputError("dllPutLatch - Problems writing ring into chip\n");
#endif
        break;
      }
    }

    /* exit single point */
    break;

  }

//  if (enabledCache) {
//    /* Write all the data to the chip */
//    rc = dllFlushRingCache(); if (rc) return rc;
//    rc = dllDisableRingCache();
//  }

  return rc;
}

/**
 @brief Parse the scandef for the latchname provided and load into latchBuffer for later retrieval
 @param target Chip target to operate on
 @param i_ringName Optional ring name, ignored if == NULL
 @param i_latchName latch name to search for in scandef
 @param i_mode Mode to search with, full or partial name matchs
 @param o_latchdata Return latch data read from scandef
*/
uint32_t dllReadScandef(ecmdChipTarget & target, const char* i_ringName, const char* i_latchName, ecmdLatchMode_t i_mode, ecmdLatchBufferEntry & o_latchdata) {
  uint32_t rc = ECMD_SUCCESS;
  std::list<ecmdLatchBufferEntry>::iterator bufferit;
  std::list< ecmdLatchInfo >::iterator curLatchInfo;    ///< Iterator for walking through latches
  std::string scandefFile;                      ///< Full path to scandef file
  bool foundit;                                 ///< Did I find the latch info that I have already looked up
  bool newFileFormat = false;                   ///< This is set if we find the new Eclipz scandef format 
  std::string latchName = i_latchName;          ///< Store our latchname in a stl string
  std::string curRing;                          ///< Current ring being read in

  /* Transform to upper case for case-insensitive comparisons */
  transform(latchName.begin(), latchName.end(), latchName.begin(), toupper);

  /* single exit point */
  while (1) {
    /* Let's see if we have already looked up this info */
    foundit = false;
    for (bufferit = latchBuffer.begin(); bufferit != latchBuffer.end(); bufferit ++) {
      /* Does the scandeffile and the current latch match to what we have stored ? */
      if (bufferit->scandefName == scandefFile && bufferit->latchName == latchName) {
        /* If the user passed NULL we have to search entire scandef */
        if (i_ringName == NULL) {
          if (bufferit->ringName.length() == 0) {
            o_latchdata = (*bufferit);
            foundit = true;
            break;
          }
        } else {
          if (bufferit->ringName == i_ringName) {
            o_latchdata = (*bufferit);
            foundit = true;
            break;
          }
        }
      }
    } /* End for search loop */

    /* We're done, get out of here */
    if (foundit) return rc;


    /* We don't have it already, let's go looking */
    if (!foundit) {

      /* find scandef file */
      rc = dllQueryFileLocation(target, ECMD_FILE_SCANDEF, scandefFile);
      if (rc) {
#ifndef ECMD_STRIP_DEBUG
        if (ecmdGlobal_DllDebug) 
          dllOutputError(("putlatch - Error occured locating scandef file: " + scandefFile + "\n").c_str());
#endif
        return rc;
      }

      std::ifstream ins(scandefFile.c_str());
      if (ins.fail()) {
#ifndef ECMD_STRIP_DEBUG
        if (ecmdGlobal_DllDebug) 
          dllOutputError(("dllGetLatch - Error occured opening scandef file: " + scandefFile + "\n").c_str());
#endif
        rc = ECMD_UNABLE_TO_OPEN_SCANDEF; 
        break;
      }

      //let's go hunting in the scandef for this register (pattern)
      ecmdLatchInfo curLatch;

      std::string curLine;
      std::vector<std::string> curArgs(4);

      std::string ringPrefix = "ring=";
      std::string ringArg;
      if (i_ringName != NULL)
        ringArg = ringPrefix + i_ringName;
      std::string temp;

      bool done = false;
      bool foundRing = false;
      int  leftParen;
      int  colon;

      while (getline(ins, curLine) && !done) {

        if (foundRing) {

          /* We hit the end of this ring defition with the old syntax */
          if (!newFileFormat && curLine[0] == '*' && curLine.find(ringPrefix) != std::string::npos) {
            if (i_ringName != NULL) {
              done = true; break;
            } else {
              int offset = curLine.find(ringPrefix) + ringPrefix.length();
              curRing = curLine.substr(offset, std::string::npos);
              /* Strip the tail off the line */
              curRing.erase(curRing.find_first_of(" \t"));
              continue;
            }
            /* we hit the end of this ring definition with the new syntax */
          } else if (newFileFormat && curLine[0] == 'E' && curLine.find("END") != std::string::npos) {
            done = true; break;
          }
          else if (curLine.length() == 0 || curLine[0] == '\0' || curLine[0] == '*' || curLine[0] == '#') {
            //do nothing
            continue;
          }
          else if (newFileFormat && (curLine[0] != ' ') && (curLine[0] != '\t')) {
            // do nothing
            continue;
          }
          else if (!(i_mode == ECMD_LATCHMODE_FULL) && (curLine.find(latchName) != std::string::npos)) {

            /* Transform to upper case */
            transform(curLine.begin(), curLine.end(), curLine.begin(), toupper);

            ecmdParseTokens(curLine, " \t\n", curArgs);
            curLatch.length = atoi(curArgs[0].c_str());
            curLatch.ringOffset = atoi(curArgs[1].c_str());
            curLatch.latchName = curArgs[3];
          }
          else if (i_mode == ECMD_LATCHMODE_FULL) {

            /* Transform to upper case */
            transform(curLine.begin(), curLine.end(), curLine.begin(), toupper);
            ecmdParseTokens(curLine, " \t\n", curArgs);

            if (latchName == curArgs[3].substr(0,curArgs[3].find_first_of("("))) {
              curLatch.length = atoi(curArgs[0].c_str());
              curLatch.ringOffset = atoi(curArgs[1].c_str());
              curLatch.latchName = curArgs[3];
            } else
              continue;

          } else {
            /* Not one we want */
            continue;
          }

          /* Let's parse out the start/end bit if they exist */
          leftParen = curLatch.latchName.find('(');
          if (leftParen == std::string::npos) {
            /* This latch doesn't have any parens */
            curLatch.latchStartBit = curLatch.latchEndBit = 0;
          } else {
            temp = curLatch.latchName.substr(leftParen+1, curLatch.latchName.length() - leftParen - 1);
            curLatch.latchStartBit = atoi(temp.c_str());

            /* Is this a multibit or single bit */
            if ((colon = temp.find(':')) != std::string::npos) {
              curLatch.latchEndBit = atoi(temp.substr(colon+1, temp.length()).c_str());
            } else {
              curLatch.latchEndBit = curLatch.latchStartBit;
            }
          }
          curLatch.ringName = curRing;
          o_latchdata.entry.push_back(curLatch);

        }
        /* The user specified a ring for use to look in */
        else if ((i_ringName != NULL) &&
                 (((curLine[0] == '*') && (curLine.find(ringArg) != std::string::npos)) ||
                  ((curLine[0] == 'N') && (curLine.find(i_ringName) != std::string::npos)))) {
          foundRing = true;
          if (curLine.substr(0,4) == "Name") {
            newFileFormat = true;
          }
          curRing = i_ringName;

          /* The user didn't specify a ring for us, we will search them all */
        } else if ((i_ringName == NULL) &&
                   ((curLine[0] == '*') && (curLine.find(ringPrefix) != std::string::npos))) {
          int offset = curLine.find(ringPrefix) + ringPrefix.length();
          curRing = curLine.substr(offset, std::string::npos);
          /* Strip the tail off the line */
          curRing.erase(curRing.find_first_of(" \t"));
          foundRing = true;
        } else if ((i_ringName == NULL) &&
                   ((curLine[0] == 'N') && (curLine.substr(0,4) == "Name"))) {
          curRing = curLine.substr(5, std::string::npos);
          /* Strip the tail off the line */
          curRing.erase(curRing.find_first_of(" \t"));
          foundRing = true;
        }                    
      }

      ins.close();

      if (!foundRing) {
#ifndef ECMD_STRIP_DEBUG
        if (ecmdGlobal_DllDebug) {
          std::string tmp = i_ringName;
          dllOutputError(("dllGetLatch - Could not find ring name " + tmp + "\n").c_str());
        }
#endif
        rc = ECMD_INVALID_RING;
        break;
      }

      if (o_latchdata.entry.empty()) {
#ifndef ECMD_STRIP_DEBUG
        if (ecmdGlobal_DllDebug) 
          dllOutputError(("dllGetLatch - no registers found that matched " + latchName + "\n").c_str());
#endif
        rc = ECMD_INVALID_LATCHNAME;
        break;
      }

      o_latchdata.scandefName = scandefFile;
      if (i_ringName != NULL)
        o_latchdata.ringName = i_ringName;
      else
        o_latchdata.ringName = "";
      o_latchdata.latchName = latchName;
      o_latchdata.entry.sort();

      /* Let's push this entry onto the stack */
      latchBuffer.push_back(o_latchdata);

    } /* end !foundit */
  } /* end single exit point */

  return rc;
}

uint32_t dllSimPOLLFAC(const char* i_facname, uint32_t i_bitlength, ecmdDataBuffer & i_expect, uint32_t i_row = 0, uint32_t i_offset = 0, uint32_t i_maxcycles = 1, uint32_t i_pollinterval = 1) {

  uint32_t curcycles = 0 , rc = ECMD_SUCCESS;
  ecmdDataBuffer actual_data;

  actual_data.setBitLength(i_bitlength);

  while (curcycles < i_maxcycles) {
    rc = dllSimGETFAC(i_facname,i_bitlength,actual_data,i_row,i_offset);
    if (rc) return rc;

    /* We found what we expected */
    if (i_expect == actual_data) {
      return rc;
    }

    dllSimclock(i_pollinterval);
    curcycles += i_pollinterval;
  }

  /* We must have timed out */
  return ECMD_POLLING_FAILURE;
}


// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
