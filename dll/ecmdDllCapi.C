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

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder     Description                       
//  ---- -------- ---- -------- -----     -----------------------------
//  @01  STG4466       03/10/05 Prahl     Fix up Beam errors
//   
// End Change Log *****************************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define ecmdDllCapi_C
#include <stdio.h>
#include <list>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h> /* for htonl */

#include <ecmdDllCapi.H>
#include <ecmdStructs.H>
#include <ecmdSharedUtils.H>

using namespace std;

#undef ecmdDllCapi_C
//----------------------------------------------------------------------
//  User Types
//----------------------------------------------------------------------

/** @brief Used to hold info out of the scandef for get/putlatch etc. */
struct ecmdLatchInfo {
  std::string ringName;                 ///< Name of ring that contains this latch
  std::string latchName;                ///< Full Latch Name (including any parens)
  uint32_t fsiRingOffset;               ///< Ring Offset for FSI
  uint32_t jtagRingOffset;              ///< Ring Offset for JTAG
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

/** @brief Used to hold latchname keys and offsets into the scandef **/
struct ecmdLatchHashInfo {
 uint32_t latchHashKey;                 ///< HashKey for the Latchname in Uppercase
 uint32_t latchOffset;                  ///< Offset for the latchname in the scandef file
 uint32_t ringBeginOffset;              ///< Begin Offset for the latch Ring in the scandef file
 uint32_t ringEndOffset;                ///< End Offset for the latch Ring in the scandef file
 bool ringFound;
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

/* This is used by the ecmdPush/PopCommandArgs functions */
std::list<ecmdUserInfo> ecmdArgsStack;

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
uint32_t dllReadScandefHash(ecmdChipTarget & target, const char* i_ringName,const char* i_latchName, ecmdLatchBufferEntry & o_latchdata) ;
uint32_t dllGetChipData (ecmdChipTarget & i_target, ecmdChipData & o_data);
std::string dllParseReturnCode(uint32_t i_returnCode);


/** @brief Used to sort latch entries from the scandef */
bool operator< (const ecmdLatchInfo & lhs, const ecmdLatchInfo & rhs) {


  if (lhs.ringName != rhs.ringName)
    return lhs.ringName < rhs.ringName;

 int lhsLeftParen = lhs.latchName.find('(');
  int rhsLeftParen = rhs.latchName.find('(');

  if ((uint32_t) lhsLeftParen == std::string::npos || (uint32_t) rhsLeftParen == std::string::npos || lhsLeftParen != rhsLeftParen) {
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



#ifndef ECMD_STRIP_DEBUG
  ecmdGlobal_DllDebug = debugLevel;

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

uint32_t dllCheckDllVersion (const char* options) {
  uint32_t rc = 0;
  char ver[20];
  strcpy(ver, ECMD_CAPI_VERSION);
  char major[10];
  char minor[10];


  int majorlength = (int)(strchr(ver, '.') - ver);
  strncpy(major, ver, majorlength);
  major[majorlength] = '\0';
  strncpy(minor, &(ver[majorlength + 1]), strlen(ver) - majorlength - 1);
  minor[strlen(ver) - majorlength - 1] = '\0';

  /* Default is just the major number */
  if ((options == NULL) || (strlen(options) == 0)) {
    printf("ver%s",major);
  } else if (!strcmp(options,"full")) {
    printf("ver-%s-%s",major,minor);
  }

  /* Force an exit here as the dll is not properly initialized we can't allow things to continue */
  exit(0);

  return rc;
}

std::string dllGetErrorMsg(uint32_t i_errorCode, bool i_parseReturnCode) {
  std::string ret;
  std::list<ecmdError>::iterator cur;
  char tmp[200];
  bool first = true;

  for (cur = ecmdErrorList.begin(); cur != ecmdErrorList.end(); cur++) {
    if ( (*cur).errorCode == i_errorCode ) {
      if (first) {
        ret  = "====== EXTENDED ERROR MSG : " + (*cur).whom + " ===============\n";
        first = false;
      } else {
        ret  += "------\n";
      }
      ret += (*cur).message;
    }
  }
  if (!first) {
    /* We must have found something */
    if (i_parseReturnCode) {
      ret  += "------\n";
      sprintf(tmp,"RETURN CODE (0x%X): %s\n",i_errorCode,dllParseReturnCode(i_errorCode).c_str());
      ret += tmp;
    }
    ret += "==============================================================\n";
  }

  /* We want to parse the return code even if we didn't finded extended error info */
  if ((ret.length() == 0) && (i_parseReturnCode)) {
    sprintf(tmp,"ecmdGetErrorMsg - RETURN CODE (0x%X): %s\n",i_errorCode, dllParseReturnCode(i_errorCode).c_str());
    ret = tmp;
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

uint32_t dllQuerySelected(ecmdChipTarget & i_target, ecmdQueryData & o_queryData, ecmdConfigLoopType_t i_looptype) {
  uint32_t rc = ECMD_SUCCESS;

  uint8_t SINGLE = 0;
  uint8_t MULTI = 1;
  uint8_t ALL = 2;

  //@01c Add init to 1
  uint8_t cageType = 1;
  uint8_t nodeType = 1;
  uint8_t slotType = 1;
  uint8_t posType  = 1;
  uint8_t coreType = 1;
  uint8_t threadType = 1;

  std::string allFlag = "all";
  std::string patterns = ",.";

  /* Let's setup for the Variable depth, walk up until we find something specified */
  if ((i_looptype == ECMD_SELECTED_TARGETS_LOOP_VD) || (i_looptype == ECMD_SELECTED_TARGETS_LOOP_VD_DEFALL)) {
    /* If they specified -all, we do the lowest depth, like normal */
    if (!ecmdUserArgs.allTargetSpecified) {
      if ((i_target.threadState == ECMD_TARGET_QUERY_IGNORE) || (ecmdUserArgs.thread == "")) {
        i_target.threadState = ECMD_TARGET_QUERY_IGNORE;

        if ((i_target.coreState == ECMD_TARGET_QUERY_IGNORE) || (ecmdUserArgs.core == "")) {
          i_target.coreState = ECMD_TARGET_QUERY_IGNORE;

          if ((i_target.posState == ECMD_TARGET_QUERY_IGNORE) || (ecmdUserArgs.pos == "")) {
            i_target.posState = ECMD_TARGET_QUERY_IGNORE;

            if ((i_target.slotState == ECMD_TARGET_QUERY_IGNORE) || (ecmdUserArgs.slot == "")) {
              i_target.slotState = ECMD_TARGET_QUERY_IGNORE;

              if ((i_target.nodeState == ECMD_TARGET_QUERY_IGNORE) || (ecmdUserArgs.node == "")) {
                i_target.nodeState = ECMD_TARGET_QUERY_IGNORE;

                if ((i_target.cageState == ECMD_TARGET_QUERY_IGNORE) || (ecmdUserArgs.cage == "")) {
                  i_target.cageState = ECMD_TARGET_QUERY_IGNORE;


                } /* cage */
              } /* node */
            } /* slot */
          } /* pos */
        } /* core */
      } /* thread */
    }
    /* Go back to a standard loop now that states are set */
    if (i_looptype == ECMD_SELECTED_TARGETS_LOOP_VD)
      i_looptype = ECMD_SELECTED_TARGETS_LOOP;
    else
      i_looptype = ECMD_SELECTED_TARGETS_LOOP_DEFALL;
  }

  //update target with useful info in the ecmdUserArgs struct
  //cage
  if (i_target.cageState == ECMD_TARGET_QUERY_IGNORE) {
    /* If the cage is set to ignore we can't return anything so let's just short circuit */
    return ECMD_SUCCESS;
  }

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
      cageType = SINGLE;
      i_target.cageState = ECMD_TARGET_QUERY_FIELD_VALID;
    } else if (i_looptype == ECMD_SELECTED_TARGETS_LOOP_DEFALL) {
      /* Default to all */
      i_target.cageState = ECMD_TARGET_QUERY_WILDCARD;
      cageType = ALL;
    } else {
      i_target.cage = 0x0;
      cageType = SINGLE;
      i_target.cageState = ECMD_TARGET_QUERY_FIELD_VALID;
    }

  }

  //node
  if (i_target.nodeState != ECMD_TARGET_QUERY_IGNORE) {
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
        nodeType = SINGLE;
        i_target.nodeState = ECMD_TARGET_QUERY_FIELD_VALID;
      } else if (i_looptype == ECMD_SELECTED_TARGETS_LOOP_DEFALL) {
        /* Default to all */
        i_target.nodeState = ECMD_TARGET_QUERY_WILDCARD;
        nodeType = ALL;
      }
      else {
        i_target.node = 0x0;
        nodeType = SINGLE;
        i_target.nodeState = ECMD_TARGET_QUERY_FIELD_VALID;
      }

    }
  }

  //slot
  if (i_target.slotState != ECMD_TARGET_QUERY_IGNORE) {
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
        slotType = SINGLE;
        i_target.slotState = ECMD_TARGET_QUERY_FIELD_VALID;
      } else if (i_looptype == ECMD_SELECTED_TARGETS_LOOP_DEFALL) {
        /* Default to all */
        i_target.slotState = ECMD_TARGET_QUERY_WILDCARD;
        slotType = ALL;
      }
      else {
        i_target.slot = 0x0;
        slotType = SINGLE;
        i_target.slotState = ECMD_TARGET_QUERY_FIELD_VALID;
      }

    }
  }

  //position
  if (i_target.posState != ECMD_TARGET_QUERY_IGNORE) {

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
        posType = SINGLE;
        i_target.posState = ECMD_TARGET_QUERY_FIELD_VALID;
      } else if (i_looptype == ECMD_SELECTED_TARGETS_LOOP_DEFALL) {
        /* Default to all */
        i_target.posState = ECMD_TARGET_QUERY_WILDCARD;
        posType = ALL;
      }
      else {
        i_target.pos = 0x0;
        posType = SINGLE;
        i_target.posState = ECMD_TARGET_QUERY_FIELD_VALID;
      }

    }
  }

  //core
  if (i_target.coreState != ECMD_TARGET_QUERY_IGNORE) {
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
        coreType = SINGLE;
        i_target.coreState = ECMD_TARGET_QUERY_FIELD_VALID;
      } else if (i_looptype == ECMD_SELECTED_TARGETS_LOOP_DEFALL) {
        /* Default to all */
        i_target.coreState = ECMD_TARGET_QUERY_WILDCARD;
        coreType = ALL;
      }
      else {
        i_target.core = 0x0;
        coreType = SINGLE;
        i_target.coreState = ECMD_TARGET_QUERY_FIELD_VALID;
      }

    }
  }

  //thread
  if (i_target.threadState != ECMD_TARGET_QUERY_IGNORE) {
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
        threadType = SINGLE;
        i_target.threadState = ECMD_TARGET_QUERY_FIELD_VALID;
      } else if (i_looptype == ECMD_SELECTED_TARGETS_LOOP_DEFALL) {
        /* Default to all */
        i_target.threadState = ECMD_TARGET_QUERY_WILDCARD;
        threadType = ALL;
      }
      else {
        i_target.thread = 0x0;
        threadType = SINGLE;
        i_target.threadState = ECMD_TARGET_QUERY_FIELD_VALID;
      }

    }
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
    

  //cage - the "-k" was Larry's idea, I just liked it - 
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

void dllPushCommandArgs() {
  ecmdArgsStack.push_back(ecmdUserArgs);
  ecmdUserArgs.cage = ecmdUserArgs.node = ecmdUserArgs.slot = ecmdUserArgs.pos = ecmdUserArgs.core = ecmdUserArgs.thread = "";
}

void dllPopCommandArgs() {
  if (!ecmdArgsStack.empty()) {
    ecmdUserArgs = ecmdArgsStack.back();
    ecmdArgsStack.pop_back();
  }
}


uint8_t dllRemoveCurrentElement (int curPos, std::string userArgs) {
  uint8_t remove = 1;

  std::string curSubstr;
  int curOffset = 0;
  int nextOffset = 0;
  int tmpOffset = 0;

  while (curOffset < (int) userArgs.length()) {

    nextOffset = userArgs.find(',',curOffset);
    if ((uint32_t) nextOffset == std::string::npos) {
      nextOffset = userArgs.length();
    }

    curSubstr = userArgs.substr(curOffset, nextOffset - curOffset);

    if ((uint32_t)( tmpOffset = curSubstr.find("..",0)) < curSubstr.length()) {

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
  for (uint32_t x = 0; x < str.length(); x ++) {
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
  ecmdDataBuffer buffer(500 /* bits */);        ///< Space for extracted latch data
  ecmdDataBuffer buffertemp(500 /* bits */);    ///< Temp space for extracted latch data
  bool enabledCache = false;                    ///< This is turned on if we enabled the cache, so we can disable on exit
  ecmdLatchEntry curData;                       ///< Data to load into return list
  std::string curRing;                          ///< Current ring being operated on
  ecmdChipData chipData;                ///< Chip data to find out bus info
  uint32_t bustype;                             ///< Type of bus we are attached to JTAG vs FSI
  std::string printed;

  if (!dllIsRingCacheEnabled()) {
    enabledCache = true;
    dllEnableRingCache();
  }

  /* Let's find out if we are JTAG of FSI here */
  rc = dllGetChipData(target, chipData);
  if (rc) {
    printed = "Problems retrieving chip information on target\n";
    dllRegisterErrorMsg(rc, "dllGetLatch", printed.c_str() );
    return rc;
  }
  /* Now make sure the plugin gave us some bus info */
  if (!(chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK)) {
    dllRegisterErrorMsg(ECMD_DLL_INVALID, "dllGetLatch", "eCMD plugin did not implement ecmdChipData.chipFlags unable to determine if FSI or JTAG attached chip\n");
    return ECMD_DLL_INVALID;
  } else if (((chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK) != ECMD_CHIPFLAG_JTAG) &&
             ((chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK) != ECMD_CHIPFLAG_FSI) ) {
    dllRegisterErrorMsg(ECMD_DLL_INVALID, "dllGetLatch", "eCMD plugin returned an invalid bustype in ecmdChipData.chipFlags\n");
    return ECMD_DLL_INVALID;
  }
  /* Store our type */
  bustype = chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK;

  
  if( i_mode == ECMD_LATCHMODE_FULL) {
    rc = dllReadScandefHash(target, i_ringName, i_latchName, curEntry);
    if( rc && (((rc != ECMD_UNKNOWN_FILE) &&(rc != ECMD_UNABLE_TO_OPEN_SCANDEFHASH)) && ((rc == ECMD_INVALID_LATCHNAME)||(rc == ECMD_INVALID_RING)))) {
      return rc;
    }
  }
  if (rc || (i_mode != ECMD_LATCHMODE_FULL)) {
    rc = dllReadScandef(target, i_ringName, i_latchName, i_mode, curEntry);
    if (rc) return rc;
  }

  /* Single exit point */
  while (1) {

    uint32_t bitsToFetch = 0x0FFFFFFF;  // Grab all bits
    int curLatchBit = -1;               // This is the actual latch bit we are looking for next
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
          dllRegisterErrorMsg(rc, "dllGetLatch", "Problems reading ring from chip\n");
          break;
        }
        curRing = curLatchInfo->ringName;
      }
      /* Do we have previous data here , or some missing bits in the scandef latchs ?*/
      if (((dataStartBit != -1) && (curLatchBit != (int) curLatchInfo->latchStartBit) && (curLatchBit != (int) curLatchInfo->latchEndBit)) ||
          ((latchname == "") || (latchname.substr(0, latchname.rfind('(')) != curLatchInfo->latchName.substr(0, curLatchInfo->latchName.rfind('('))))) {
        /* I have some good data here */
        if (latchname != "") {

          curData.latchStartBit = dataStartBit;
          curData.latchEndBit = dataEndBit;
          rc = buffer.extract(curData.buffer, 0, dataEndBit - dataStartBit + 1); if (rc) return rc;
          curData.rc = ECMD_SUCCESS;
          o_data.push_back(curData);

        }

        /* If this is a fresh one we need to reset everything */
        if ((latchname == "") || (latchname != curLatchInfo->latchName.substr(0, latchname.length()))) {
          dataStartBit = dataEndBit = -1;
          curBitsToFetch = 0x0FFFFFFF;
          curBufferBit = 0;
          latchname = curLatchInfo->latchName.substr(0, curLatchInfo->latchName.rfind('('));
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
      if (((curLatchInfo->latchEndBit >= curLatchInfo->latchStartBit) && (curLatchBit <= (int) curLatchInfo->latchEndBit)) ||
          /* Check if the bits are ordered to:from (10:0) */
          ((curLatchInfo->latchStartBit > curLatchInfo->latchEndBit)  && (curLatchBit <= (int) curLatchInfo->latchStartBit))) {

        bitsToFetch = (curLatchInfo->length  < curBitsToFetch) ? curLatchInfo->length : curBitsToFetch;

        /* Setup the actual data bits displayed */
        if (dataStartBit == -1) {
          dataStartBit = curLatchBit;
          dataEndBit = dataStartBit - 1;
        }
        dataEndBit += bitsToFetch;

        /* ********* */
        /* FSI Order */
        /* ********* */

        if (bustype == ECMD_CHIPFLAG_FSI) {
          rc = ringBuffer.extract(buffertemp, curLatchInfo->fsiRingOffset, bitsToFetch); if (rc) return rc;

          /* Extract bits if ordered from:to (0:10) */
          if (curLatchInfo->latchEndBit > curLatchInfo->latchStartBit) {
            curLatchBit = curLatchInfo->latchEndBit + 1;
            /* Extract if bits are ordered to:from (10:0) or just (1) */
          } else {
            if (bitsToFetch > 1) buffertemp.reverse();
            curLatchBit = curLatchInfo->latchStartBit + 1;
          }
          rc = buffer.insert(buffertemp, curBufferBit, bitsToFetch); if (rc) return rc;
	  curBufferBit += bitsToFetch;

        /* ********* */
        /*JTAG Order */
        /* ********* */
        } else {
          rc = ringBuffer.extract(buffertemp, curLatchInfo->jtagRingOffset - bitsToFetch + 1, bitsToFetch); if (rc) return rc;
          /* Extract bits if ordered from:to (0:10) */
          if (curLatchInfo->latchEndBit > curLatchInfo->latchStartBit) {
            buffertemp.reverse();
            curLatchBit = curLatchInfo->latchEndBit + 1;
          } else {
            /* Extract if bits are ordered to:from (10:0) or just (1) */
            curLatchBit = curLatchInfo->latchStartBit + 1;
          }
          rc = buffer.insert(buffertemp, curBufferBit, bitsToFetch); if (rc) return rc;
	  curBufferBit += bitsToFetch;

        }

        curBitsToFetch -= bitsToFetch;
      } else {
        /* Nothing was there that we needed, let's try the next entry */
        curLatchBit = curLatchInfo->latchStartBit < curLatchInfo->latchEndBit ? curLatchInfo->latchEndBit + 1: curLatchInfo->latchStartBit + 1;

      }

    } /* end latchinfo for loop */

    /* We have good data here, let's push it */
    if (latchname != "") {

      /* Display this if we aren't expecting or the expect failed */
      curData.latchStartBit = dataStartBit;
      curData.latchEndBit = dataEndBit;
      rc = buffer.extract(curData.buffer, 0, dataEndBit - dataStartBit + 1); if (rc) return rc;
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

uint32_t dllPutLatch(ecmdChipTarget & i_target, const char* i_ringName, const char * i_latchName, ecmdDataBuffer & i_data, uint32_t i_startBit, uint32_t i_numBits, uint32_t & o_matchs, ecmdLatchMode_t i_mode) {

  uint32_t rc = 0;
  ecmdLatchBufferEntry curEntry;
  std::list< ecmdLatchInfo >::iterator curLatchInfo;
  ecmdDataBuffer ringBuffer;            ///< Buffer to store entire ring
  ecmdDataBuffer bufferCopy;            ///< Copy of data to be inserted
  ecmdDataBuffer buffertemp;            ///< Temp buffer to allow reversing in JTAG mode
  std::string curRing;                  ///< Current ring being operated on
  std::string curLatchName;             ///< Current latch name being operated on
  ecmdChipData chipData;                ///< Chip data to find out bus info
  uint32_t bustype;                             ///< Type of bus we are attached to JTAG vs FSI
  std::string printed;
  bool enabledCache = false;                    ///< This is turned on if we enabled the cache, so we can disable on exit

  o_matchs = 0;

  if (!dllIsRingCacheEnabled()) {
    enabledCache = true;
    dllEnableRingCache();
  }

  /* Do we have the right amount of data ? */
  if (i_data.getBitLength() > i_numBits) {
    rc = ECMD_DATA_OVERFLOW;
    dllRegisterErrorMsg(rc, "dllPutLatch", "Data buffer length is greater then numBits requested to write\n");
    return rc;
  } else if (i_data.getBitLength() < i_numBits) {
    rc = ECMD_DATA_UNDERFLOW;
    dllRegisterErrorMsg(rc, "dllPutLatch", "Data buffer length is less then numBits requested to write\n");
    return rc;
  }

  /* Let's find out if we are JTAG of FSI here */
  rc = dllGetChipData(i_target, chipData);
  if (rc) {
    dllRegisterErrorMsg(rc, "dllPutLatch", "Problems retrieving chip information on target\n");
    return rc;
  }
  /* Now make sure the plugin gave us some bus info */
  if (!(chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK)) {
    dllRegisterErrorMsg(ECMD_DLL_INVALID, "dllPutLatch", "eCMD plugin did not implement ecmdChipData.chipFlags unable to determine if FSI or JTAG attached chip\n");
    return ECMD_DLL_INVALID;
  } else if (((chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK) != ECMD_CHIPFLAG_JTAG) &&
             ((chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK) != ECMD_CHIPFLAG_FSI) ) {
    dllRegisterErrorMsg(ECMD_DLL_INVALID, "dllPutLatch", "eCMD plugin returned an invalid bustype in ecmdChipData.chipFlags\n");
    return ECMD_DLL_INVALID;
  }
  /* Store our type */
  bustype = chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK;

  if( i_mode == ECMD_LATCHMODE_FULL) {
    rc = dllReadScandefHash(i_target, i_ringName, i_latchName, curEntry);
    if( rc && (((rc != ECMD_UNKNOWN_FILE) &&(rc != ECMD_UNABLE_TO_OPEN_SCANDEFHASH)) && ((rc == ECMD_INVALID_LATCHNAME)||(rc == ECMD_INVALID_RING)))) {
      return rc;
    }
  }
  if (rc || (i_mode != ECMD_LATCHMODE_FULL)) {
    rc = dllReadScandef(i_target, i_ringName, i_latchName, i_mode, curEntry);
    if (rc) return rc;
  }

  

  /* single exit point */
  while (1) {



    uint32_t bitsToInsert = i_data.getBitLength();
    int curLatchBit = -1;                       // This is the actual latch bit we are looking for next
    int curBitsToInsert = bitsToInsert;         // This is the total number of bits left to Insert
    int curStartBitToInsert;                    // This is the offset into the current entry for insertion

    for (curLatchInfo = curEntry.entry.begin(); curLatchInfo != curEntry.entry.end(); curLatchInfo++) {

      /* Let's grab the ring for this latch entry */
      if (curRing != curLatchInfo->ringName) {
        if (curRing.length() > 0) {
          rc = dllPutRing(i_target, curRing.c_str(), ringBuffer);
          if (rc) {
            dllRegisterErrorMsg(rc, "dllPutLatch", "Problems writing ring into chip\n");
            break;
          }
        }
          
        rc = dllGetRing(i_target, curLatchInfo->ringName.c_str(), ringBuffer);
        if (rc) {
          dllRegisterErrorMsg(rc, "dllPutLatch", "Problems reading ring from chip\n");
          break;
        }
        curRing = curLatchInfo->ringName;
      }

      if ((curLatchBit == -1) || (curLatchName != curLatchInfo->latchName.substr(0, curLatchInfo->latchName.rfind('(')))) {
        curLatchBit = curLatchInfo->latchStartBit < curLatchInfo->latchEndBit ? curLatchInfo->latchStartBit : curLatchInfo->latchEndBit;
        curLatchName = curLatchInfo->latchName.substr(0, curLatchInfo->latchName.rfind('('));
        /* Make a copy of insert data so we don't lose it */
        bufferCopy = i_data;
        bitsToInsert = i_data.getBitLength();
        curBitsToInsert = bitsToInsert;

        /* We found something, bump the matchs */
        o_matchs ++;
      }

      /* Check if the bits are ordered from:to (0:10) or just (1) and we want to modify */
      if (((curLatchInfo->latchEndBit >= curLatchInfo->latchStartBit) && (i_startBit <= curLatchInfo->latchEndBit) && (i_startBit + i_data.getBitLength() >= curLatchInfo->latchStartBit)) ||
          /* Check if the bits are ordered to:from (10:0) */
          ((curLatchInfo->latchStartBit > curLatchInfo->latchEndBit) && (i_startBit <= curLatchInfo->latchStartBit) && (i_startBit + i_data.getBitLength() >= curLatchInfo->latchEndBit))) {

        if (i_startBit < curLatchInfo->latchStartBit) {
          /* If the data started before this entry */
          curStartBitToInsert = 0;
        } else {
          /* If the data starts in the middle of this entry */
          curStartBitToInsert = (curLatchInfo->latchEndBit >= curLatchInfo->latchStartBit) ? i_startBit - curLatchInfo->latchStartBit : i_startBit - curLatchInfo->latchEndBit;
        }
        bitsToInsert = ((curLatchInfo->length - curStartBitToInsert) < curBitsToInsert) ? curLatchInfo->length - curStartBitToInsert : curBitsToInsert;


        /* ********* */
        /* FSI Order */
        /* ********* */

        if (bustype == ECMD_CHIPFLAG_FSI) {

          rc = bufferCopy.extract(buffertemp, 0, bitsToInsert); if (rc) return rc;

          /* insert bits if ordered from:to (0:10) */
          if (curLatchInfo->latchEndBit > curLatchInfo->latchStartBit) {

            curLatchBit = curLatchInfo->latchEndBit + 1;
            /* insert if bits are ordered to:from (10:0) or just (1) */
          } else {
            if (bitsToInsert > 1) buffertemp.reverse();
            curLatchBit = curLatchInfo->latchStartBit + 1;

          }
          rc = ringBuffer.insert(buffertemp, curLatchInfo->fsiRingOffset + curStartBitToInsert, bitsToInsert); if (rc) return rc;
          /* Get rid of the data we just inserted, to line up the next piece */
          bufferCopy.shiftLeft(bitsToInsert);

        /* ********* */
        /*JTAG Order */
        /* ********* */
        } else {
          rc = bufferCopy.extract(buffertemp, 0, bitsToInsert); if (rc) return rc;

          /* insert bits if ordered from:to (0:10) */
          if (curLatchInfo->latchEndBit > curLatchInfo->latchStartBit) {
            /* Reverse the data to go into the scanring */
            buffertemp.reverse();

            curLatchBit = curLatchInfo->latchEndBit + 1;

            /* insert if bits are ordered to:from (10:0) or just (1) */
          } else {

            curLatchBit = curLatchInfo->latchStartBit + 1;

          }

          rc = ringBuffer.insert(buffertemp, curLatchInfo->jtagRingOffset - curLatchInfo->length  + 1 + curStartBitToInsert, bitsToInsert);
	  if (rc) return rc;
          /* Get rid of the data we just inserted, to line up the next piece */
          bufferCopy.shiftLeft(bitsToInsert);

        }

        curBitsToInsert -= bitsToInsert;
      } else {
        /* Nothing was there that we needed, let's try the next entry */
        curLatchBit = curLatchInfo->latchStartBit < curLatchInfo->latchEndBit ? curLatchInfo->latchEndBit + 1: curLatchInfo->latchStartBit + 1;

      }

    }

    if (curRing.length() > 0) {
      rc = dllPutRing(i_target, curRing.c_str(), ringBuffer);
      if (rc) {
        dllRegisterErrorMsg(rc, "dllPutLatch", "Problems writing ring into chip\n");
        break;
      }
    }

    /* exit single point */
    break;

  }

  if (enabledCache) {
    /* Write all the data to the chip */
    rc = dllFlushRingCache(); if (rc) return rc;
    rc = dllDisableRingCache();
  }

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
  std::string scandefFile;                      ///< Full path to scandef file
  bool foundit;                                 ///< Did I find the latch info that I have already looked up
  std::string latchName = i_latchName;          ///< Store our latchname in a stl string
  std::string curRing;                          ///< Current ring being read in
  std::string i_ring;                           ///< Ring that caller specified

  /* Transform to upper case for case-insensitive comparisons */
  transform(latchName.begin(), latchName.end(), latchName.begin(), (int(*)(int)) toupper);

  if (i_ringName != NULL) {
    i_ring = i_ringName;
    transform(i_ring.begin(), i_ring.end(), i_ring.begin(), (int(*)(int)) tolower);
  }

  /* single exit point */
  while (1) {
    /* Let's see if we have already looked up this info */
    foundit = false;
    
    /* find scandef file */
    rc = dllQueryFileLocation(target, ECMD_FILE_SCANDEF, scandefFile);
    if (rc) {
      dllRegisterErrorMsg(rc, "dllReadScandef", ("Error occured locating scandef file: " + scandefFile + "\n").c_str());
      return rc;
    }
      
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
          if (bufferit->ringName == i_ring) {
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

      std::ifstream ins(scandefFile.c_str());
      if (ins.fail()) {
        rc = ECMD_UNABLE_TO_OPEN_SCANDEF; 
        dllRegisterErrorMsg(rc, "dllReadScandef", ("Error occured opening scandef file: " + scandefFile + "\n").c_str());
        break;
      }

      //let's go hunting in the scandef for this register (pattern)
      ecmdLatchInfo curLatch;

      std::string curLine;
      std::vector<std::string> curArgs(4);

      std::string temp;

      bool done = false;
      bool foundRing = false;
      int  leftParen;
      int  colon;

      while (getline(ins, curLine) && !done) {

        if (foundRing) {

          if (curLine[0] == 'E' && curLine.find("END") != std::string::npos) {
            if (i_ringName != NULL) done = true;
            continue;
          }
	  else if ((i_ringName == NULL) &&
                   ((curLine[0] == 'N') && (curLine.substr(0,4) == "Name"))) {
            ecmdParseTokens(curLine, " \t\n=", curArgs);
            if (curArgs.size() != 2) {
              rc = ECMD_SCANDEF_LOOKUP_FAILURE;
              dllRegisterErrorMsg(rc, "dllReadScandef", ("Parse failure reading ring name from line : '" + curLine + "'\n").c_str());
              break;
            }
            transform(curArgs[1].begin(), curArgs[1].end(), curArgs[1].begin(), (int(*)(int)) tolower);
            /* Get just the ringname */
            curRing = curArgs[1];
	    continue;
          }         
          else if (curLine.length() == 0 || curLine[0] == '\0' || curLine[0] == '*' || curLine[0] == '#') {
            //do nothing
            continue;
          }
          else if ((curLine[0] != ' ') && (curLine[0] != '\t')) {
            // do nothing
            continue;
          }
          else if (!(i_mode == ECMD_LATCHMODE_FULL) && (curLine.find(latchName) != std::string::npos)) {

            /* Transform to upper case */
            transform(curLine.begin(), curLine.end(), curLine.begin(), (int(*)(int)) toupper);

            ecmdParseTokens(curLine, " \t\n", curArgs);
            curLatch.length = atoi(curArgs[0].c_str());
            curLatch.fsiRingOffset = atoi(curArgs[1].c_str());
            curLatch.jtagRingOffset = atoi(curArgs[2].c_str());
            curLatch.latchName = curArgs[4];
          }
          else if (i_mode == ECMD_LATCHMODE_FULL) {

            /* Transform to upper case */
            transform(curLine.begin(), curLine.end(), curLine.begin(), (int(*)(int)) toupper);
            ecmdParseTokens(curLine, " \t\n", curArgs);

            if ((curArgs.size() >= 5) && latchName == curArgs[4].substr(0,curArgs[4].find_last_of("("))) {
              curLatch.length = atoi(curArgs[0].c_str());
              curLatch.fsiRingOffset = atoi(curArgs[1].c_str());
              curLatch.jtagRingOffset = atoi(curArgs[2].c_str());
              curLatch.latchName = curArgs[4];
            } else
              continue;

          } else {
            /* Not one we want */
            continue;
          }

          /* Let's parse out the start/end bit if they exist */
          leftParen = curLatch.latchName.rfind('(');
          if ((uint32_t) leftParen == std::string::npos) {
            /* This latch doesn't have any parens */
            curLatch.latchStartBit = curLatch.latchEndBit = 0;
          } else {
            temp = curLatch.latchName.substr(leftParen+1, curLatch.latchName.length() - leftParen - 1);
            curLatch.latchStartBit = atoi(temp.c_str());

            /* Is this a multibit or single bit */
            if ((uint32_t) (colon = temp.find(':')) != std::string::npos) {
              curLatch.latchEndBit = atoi(temp.substr(colon+1, temp.length()).c_str());
            } else if ((uint32_t)(colon = temp.find(',')) != std::string::npos) {
              dllOutputError("dllReadScandef - Array's not currently supported with getlatch\n");
              return ECMD_FUNCTION_NOT_SUPPORTED;
            } else {
              curLatch.latchEndBit = curLatch.latchStartBit;
            }
          }
          curLatch.ringName = curRing;
          o_latchdata.entry.push_back(curLatch);

        }
        /* The user specified a ring for use to look in */
        else if ((i_ringName != NULL) &&
                  ((curLine[0] == 'N') && ((uint32_t) curLine.find("Name") != std::string::npos))) {
          ecmdParseTokens(curLine, " \t\n=", curArgs);
          /* Push the ring name to lower case */
          transform(curArgs[1].begin(), curArgs[1].end(), curArgs[1].begin(), (int(*)(int)) tolower);
          if ((curArgs.size() >= 2) && curArgs[1] == i_ring) {
            foundRing = true;
            curRing = i_ring;
          }

          /* The user didn't specify a ring for us, we will search them all */
        } else if ((i_ringName == NULL) &&
                   ((curLine[0] == 'N') && (curLine.substr(0,4) == "Name"))) {
          ecmdParseTokens(curLine, " \t\n=", curArgs);
          if (curArgs.size() != 2) {
            rc = ECMD_SCANDEF_LOOKUP_FAILURE;
            dllRegisterErrorMsg(rc, "dllReadScandef", ("Parse failure reading ring name from line : '" + curLine + "'\n").c_str());
            break;
          }
          transform(curArgs[1].begin(), curArgs[1].end(), curArgs[1].begin(), (int(*)(int)) tolower);
          /* Get just the ringname */
          curRing = curArgs[1];
          foundRing = true;
        }                    
      }

      ins.close();

      if (!foundRing) {
        std::string tmp = i_ringName;
        rc = ECMD_INVALID_RING;
        dllRegisterErrorMsg(rc, "dllReadScandef", ("Could not find ring name " + tmp + "\n").c_str());
        break;
      }

      if (o_latchdata.entry.empty()) {
        rc = ECMD_INVALID_LATCHNAME;
        dllRegisterErrorMsg(rc, "dllReadScandef", ("no registers found that matched " + latchName + "\n").c_str());
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

/**
 @brief Lookup the scandef hashfile for the latchname and ring offset and then seek to the offset in the scandef for the latchname provided and load into latchBuffer for later retrieval
 @param target Chip target to operate on
 @param i_ringName Optional ring name, ignored if == NULL
 @param i_latchName latch name to search for in scandef
 @param o_latchdata Return latch data read from scandef
*/
uint32_t dllReadScandefHash(ecmdChipTarget & target, const char* i_ringName, const char*  i_latchName, ecmdLatchBufferEntry & o_latchdata) {

  uint32_t rc = ECMD_SUCCESS;
  std::list<ecmdLatchBufferEntry>::iterator bufferit;
  std::list< ecmdLatchHashInfo > latchHashDet; ///< List of the LatchHash Keys and their offsets
  std::list< ecmdLatchHashInfo >::iterator latchHashDetIter;  
  ecmdLatchHashInfo curLatchHashInfo;           ///< Current Latch Hash Key, Offset
  uint32_t latchKeyToLookFor;                   ///< Hash Key for i_latchName
  uint32_t ringKey;                             ///< Hash Key for i_ringName
  std::string scandefFile;                      ///< Full path to scandef file
  std::string scandefHashFile;                  ///< Full path to scandefhash file
  bool foundit;                                 ///< Did I find the latch info that I have already looked up
  std::string latchName = i_latchName;          ///< Store our latchname in a stl string
  std::string curRing;                          ///< Current ring being read in
  std::string i_ring;                           ///< Ring that caller specified

  /* Transform to upper case for case-insensitive comparisons */
  transform(latchName.begin(), latchName.end(), latchName.begin(), (int(*)(int)) toupper);

  if (i_ringName != NULL) {
    i_ring = i_ringName;
    transform(i_ring.begin(), i_ring.end(), i_ring.begin(), (int(*)(int)) tolower);
  }

  
  /* single exit point */
  while (1) {
    /* Let's see if we have already looked up this info */
    foundit = false;
    /* find scandef file */
    rc = dllQueryFileLocation(target, ECMD_FILE_SCANDEF, scandefFile);
    if (rc) {
      dllRegisterErrorMsg(rc, "dllReadScandefHash", ("Error occured locating scandef file: " + scandefFile + "\n").c_str());
      return rc;
    }

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
          if (bufferit->ringName == i_ring) {
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

      /* Get the Hash Key for the latchName */
      latchKeyToLookFor = ecmdHashString32(latchName.c_str(), 0);
      
      if (i_ringName != NULL) {
        ringKey = ecmdHashString32(i_ring.c_str(), 0);
      }
      /* find scandef hash file */
      rc = dllQueryFileLocation(target, ECMD_FILE_SCANDEFHASH, scandefHashFile);
      if (rc) {
        dllRegisterErrorMsg(rc, "dllReadScandefHash", ("Error occured locating scandef hash file: " + scandefHashFile + "\n").c_str());
        return rc;
      }

      std::ifstream insh(scandefHashFile.c_str());
      if (insh.fail()) {
        rc = ECMD_UNABLE_TO_OPEN_SCANDEFHASH;
        dllRegisterErrorMsg(rc, "dllReadScandefHash", ("Error occured opening scandef file: " + scandefHashFile + "\n").c_str());
        break;
      }
 
      bool foundLatch = false;
      uint32_t numRings =0;
      insh.read((char *)& numRings, 4);
      numRings = htonl(numRings);

      //Get Offset of the end of file
      insh.seekg (0, ios::end); 
      uint32_t end = insh.tellg(); 
      
      uint32_t curLKey; //LatchKey 
      uint32_t curLOffset; //LatchOffset

      //Binary Search through the latches
      //Index-low,mid,high
      //Latch Section
       uint32_t low=(((numRings * 8) * 2) + 8)/8;//Each ring is repeated twice - One for BEGIN offset and One for END
       uint32_t mid=0;
       uint32_t high = end/8-1;
       while(low <= high) {
         mid = (low + high) / 2;
         insh.seekg ( mid*8 );
         insh.read( (char *)& curLKey, 4);
         curLKey = htonl(curLKey); 
         if(latchKeyToLookFor == curLKey) {
           //Goto the first occurence of this latch
           while(latchKeyToLookFor == curLKey) {
             insh.seekg(-12, ios::cur);
             insh.read( (char *)& curLKey, 4);
             curLKey = htonl(curLKey); 
           }
           //Go back to the matching latch
           insh.seekg(4, ios::cur);
           insh.read( (char *)& curLKey, 4);
	   curLKey = htonl(curLKey); 
           while(latchKeyToLookFor == curLKey) {
            insh.read( (char *)& curLOffset, 4 );
            curLOffset = htonl(curLOffset);
            curLatchHashInfo.latchOffset = curLOffset;
            curLatchHashInfo.ringFound = false;
	    latchHashDet.push_back(curLatchHashInfo);
            foundLatch = true;
            //Read next latch
            insh.read( (char *)& curLKey, 4);
	    curLKey = htonl(curLKey); 
           }
           break;
         }
         if (latchKeyToLookFor < curLKey)
           high = mid - 1;
         else
           low  = mid + 1;
       }


      if (!foundLatch) {
        rc = ECMD_INVALID_LATCHNAME;
        dllRegisterErrorMsg(rc, "dllReadScandefHash", ("Could not find a Latchname '" + latchName + "'  Key Match in the scandef hash.\n").c_str());
        break;
      }
      
      //Seek to the ring area in the hashfile
      insh.seekg ( 8 ); 
      
      bool ringFound = false;
      std::list< ecmdLatchHashInfo >::iterator latchIter;
      
      
      //Go back to the ring area and find out the ring for the latchname
      //Error out if latch is found in multiple rings
      for (latchHashDetIter = latchHashDet.begin(); latchHashDetIter != latchHashDet.end(); latchHashDetIter++) {   
        uint32_t curRingKey, ringBeginOffset, ringEndOffset;
	
	//Flag an error if the other latches dont fall into the same ring 
	if(ringFound) {
	 //Start another loop to make sure the new latches have offsets falling in the old latch's ring boundaries
	 for (latchIter = latchHashDet.begin(); latchIter != latchHashDet.end(); latchIter++) {
	   if(latchIter->ringFound) {
	     if((latchHashDetIter->latchOffset > latchIter->ringBeginOffset) && (latchHashDetIter->latchOffset < latchIter->ringEndOffset)) {
	 	latchHashDetIter->ringBeginOffset = latchIter->ringBeginOffset;
	 	latchHashDetIter->ringEndOffset = latchIter->ringEndOffset;
	 	latchHashDetIter->ringFound = true;
	 	continue;
	     }
	     else {
	        rc = ECMD_SCANDEFHASH_MULT_RINGS;
		dllRegisterErrorMsg(rc, "dllReadScandefHash", ("Same latchname : '" + latchName + "' found in multiple rings in the scandefhash\n").c_str());
                return rc;
	     }
	   }
	 }
	}
	else {
	 while ( (uint32_t)insh.tellg() != (((numRings * 8) * 2) + 8) ) {//Loop until end of ring area
	  insh.read( (char *)& curRingKey, 4 ); //Read the ringKey
	  insh.read( (char *)& ringBeginOffset, 4 ); //Read the begin offset
	  insh.read( (char *)& curRingKey, 4 ); //Read the ringKey
	  insh.read( (char *)& ringEndOffset, 4 ); //Read the end offset
	
	  curRingKey = htonl(curRingKey);
	  ringBeginOffset = htonl(ringBeginOffset);
	  ringEndOffset = htonl(ringEndOffset);
	
	
 
	  if((latchHashDetIter->latchOffset > ringBeginOffset) && (latchHashDetIter->latchOffset < ringEndOffset)) {
	    if ((i_ringName != NULL) && (ringKey != curRingKey)) {
	      //The ring user specified does not match the one looked up in the scandefhash
	      std::string tmp = i_ringName;
              rc = ECMD_INVALID_RING;
              dllRegisterErrorMsg(rc, "dllReadScandefHash", ("Ring " + tmp + " that user specified is not the same as the ring match found in the hash\n").c_str());
              break;
	    }
	    latchHashDetIter->ringBeginOffset = ringBeginOffset;
	    latchHashDetIter->ringEndOffset = ringEndOffset;
	    latchHashDetIter->ringFound = true;
	    ringFound = true;
	  }
	 }
	}
      }   
      insh.close();
      
      if (!ringFound) {
        rc = ECMD_INVALID_RING;
        dllRegisterErrorMsg(rc, "dllReadScandefHash", ("Could not find a ring key match for latch '" + latchName + "'\n").c_str());
        break;
      }
      
      /**********Scandef World after this *****************/
      std::ifstream ins(scandefFile.c_str());
      if (ins.fail()) {
        rc = ECMD_UNABLE_TO_OPEN_SCANDEF; 
        dllRegisterErrorMsg(rc, "dllReadScandefHash", ("Error occured opening scandef file: " + scandefFile + "\n").c_str());
        break;
      }

      //let's go hunting in the scandef for this register (pattern)
      ecmdLatchInfo curLatch;

      std::string curLine;
      std::vector<std::string> curArgs(4);

      std::string temp;

      int  leftParen;
      int  colon;
      
      //Get the Ring Begin Offset and seek till there
      latchHashDetIter = latchHashDet.begin();
      ins.seekg(latchHashDetIter->ringBeginOffset);
      
      bool foundRing = false;
      
      while (getline(ins, curLine) && !foundRing) {

        
        /* The user specified a ring for use to look in */
        if ((i_ringName != NULL) &&
                  ((curLine[0] == 'N') && (curLine.find("Name") != std::string::npos))) {
          ecmdParseTokens(curLine, " \t\n=", curArgs);
          /* Push the ring name to lower case */
          transform(curArgs[1].begin(), curArgs[1].end(), curArgs[1].begin(), (int(*)(int)) tolower);
          if ((curArgs.size() >= 2) && curArgs[1] == i_ring) {
            foundRing = true;
            curRing = i_ring;
          }

          /* The user didn't specify a ring for us, we will search them all */
        } else if ((i_ringName == NULL) &&
                   ((curLine[0] == 'N') && (curLine.substr(0,4) == "Name"))) {
          ecmdParseTokens(curLine, " \t\n=", curArgs);
          if (curArgs.size() != 2) {
            rc = ECMD_SCANDEF_LOOKUP_FAILURE;
            dllRegisterErrorMsg(rc, "dllReadScandef", ("Parse failure reading ring name from line : '" + curLine + "'\n").c_str());
            break;
          }
          transform(curArgs[1].begin(), curArgs[1].end(), curArgs[1].begin(), (int(*)(int)) tolower);
          /* Get just the ringname */
          curRing = curArgs[1];
          foundRing = true;
        }                    
      }
      if( foundRing) { 
      
       for (latchHashDetIter = latchHashDet.begin(); latchHashDetIter != latchHashDet.end(); latchHashDetIter++) {

             ins.seekg(latchHashDetIter->latchOffset);
	     
             getline(ins, curLine);
 
             /* Transform to upper case */
             transform(curLine.begin(), curLine.end(), curLine.begin(), (int(*)(int)) toupper);
	     ecmdParseTokens(curLine, " \t\n", curArgs);
             if(curArgs.size() != 5) {
	       rc = ECMD_SCANDEF_LOOKUP_FAILURE;
               dllRegisterErrorMsg(rc, "dllReadScandef", ("Latch Offset pointer incorrect. Points to : '" + curLine + "'\n").c_str());
               break;
	     }
             if (latchName == curArgs[4].substr(0,curArgs[4].find_last_of("("))) {
	       curLatch.length = atoi(curArgs[0].c_str());
               curLatch.fsiRingOffset = atoi(curArgs[1].c_str());
               curLatch.jtagRingOffset = atoi(curArgs[2].c_str());
               curLatch.latchName = curArgs[4];
             } else
               continue;//Error here??
 
           /* Let's parse out the start/end bit if they exist */
           leftParen = curLatch.latchName.rfind('(');
           if ((uint32_t) leftParen == std::string::npos) {
             /* This latch doesn't have any parens */
             curLatch.latchStartBit = curLatch.latchEndBit = 0;
           } else {
             temp = curLatch.latchName.substr(leftParen+1, curLatch.latchName.length() - leftParen - 1);
             curLatch.latchStartBit = atoi(temp.c_str());
             /* Is this a multibit or single bit */
             if ((uint32_t)(colon = temp.find(':')) != std::string::npos) {
               curLatch.latchEndBit = atoi(temp.substr(colon+1, temp.length()).c_str());
             } else if ((uint32_t)(colon = temp.find(',')) != std::string::npos) {
               dllOutputError("dllReadScandef - Array's not currently supported with getlatch\n");
               return ECMD_FUNCTION_NOT_SUPPORTED;
             } else {
               curLatch.latchEndBit = curLatch.latchStartBit;
             }
           }
           curLatch.ringName = curRing;
           o_latchdata.entry.push_back(curLatch);

       }
      
      }
      ins.close();

      if (!foundRing) {
        std::string tmp = i_ringName;
	rc = ECMD_INVALID_RING;
        dllRegisterErrorMsg(rc, "dllReadScandefHash", ("Could not find ring name " + tmp + "\n").c_str());
        break;
      }

      if (o_latchdata.entry.empty()) {
        rc = ECMD_INVALID_LATCHNAME;
	dllRegisterErrorMsg(rc, "dllReadScandefHash", ("no registers found that matched " + latchName + "\n").c_str());
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


uint32_t dllSimpolltcfac(const char* i_tcfacname, ecmdDataBuffer & i_expect, uint32_t i_row, uint32_t i_startbit, uint32_t i_bitlength, uint32_t i_maxcycles, uint32_t i_pollinterval) {

  uint32_t curcycles = 0 , rc = ECMD_SUCCESS;
  ecmdDataBuffer actual_data;

  while (curcycles < i_maxcycles) {
    rc = dllSimgettcfac(i_tcfacname,actual_data,i_row,i_startbit,i_bitlength);
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

uint32_t dllGetChipData (ecmdChipTarget & i_target, ecmdChipData & o_data) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdChipTarget tmp = i_target;
  ecmdQueryData needlesslySlow;
  tmp.cageState = tmp.nodeState = tmp.slotState = tmp.chipTypeState = tmp.posState = ECMD_TARGET_QUERY_FIELD_VALID;
  tmp.coreState = tmp.threadState = ECMD_TARGET_QUERY_IGNORE;
  rc = dllQueryConfig(tmp, needlesslySlow, ECMD_QUERY_DETAIL_HIGH);
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


std::string dllParseReturnCode(uint32_t i_returnCode) {
  std::string ret = "";

  ecmdChipTarget dummy;
  std::string filePath;
  uint32_t rc = dllQueryFileLocation(dummy, ECMD_FILE_HELPTEXT, filePath); 

  if (rc || (filePath.length()==0)) {
    ret = "ERROR FINDING DECODE FILE";
    return ret;
  }

  filePath += "ecmdReturnCodes.H";


  std::string line;
  std::vector< std::string > tokens;
  std::string source, retdefine;
  int found = 0;
  uint32_t comprc;


  std::ifstream ins(filePath.c_str());

  if (ins.fail()) {
    ret = "ERROR OPENING DECODE FILE";
    return ret;
  }

  /* This is what I am trying to parse from ecmdReturnCodes.H */

  /* #define ECMD_ERR_UNKNOWN                        0x00000000 ///< This error code wasn't flagged to which plugin it came from        */
  /* #define ECMD_ERR_ECMD                           0x01000000 ///< Error came from eCMD                                               */
  /* #define ECMD_ERR_CRONUS                         0x02000000 ///< Error came from Cronus                                             */
  /* #define ECMD_ERR_IP                             0x04000000 ///< Error came from IP GFW                                             */
  /* #define ECMD_ERR_Z                              0x08000000 ///< Error came from Z GFW                                              */
  /* #define ECMD_INVALID_DLL_VERSION                (ECMD_ERR_ECMD | 0x1000) ///< Dll Version                                          */


  while (!ins.eof()) { /*  && (strlen(str) != 0) */
    getline(ins,line,'\n');
    /* Let's strip off any comments */
    line = line.substr(0, line.find_first_of("/"));
    ecmdParseTokens(line, " \n()|", tokens);

    /* Didn't find anything */
    if (line.size() < 2) continue;

    if (tokens[0] == "#define") {
      /* Let's see if we have one of they return code source defines */
      if ((tokens.size() == 3) && (tokens[1] != "ECMD_SUCCESS") && (tokens[1] != "ECMD_DBUF_SUCCESS")) {
        sscanf(tokens[2].c_str(),"0x%x",&comprc);
        if ((i_returnCode & 0xFF000000) == comprc) {
          /* This came from this source, we will save that as we may use it later */
          source = tokens[1];
        }
      } else if ((i_returnCode & 0xFF000000) != ECMD_ERR_ECMD) {
        /* We aren't going to find this return code in here since it didn't come from us */
      } else if (tokens.size() >= 4) {
        /* This is a standard return code define */
        sscanf(tokens[3].c_str(),"0x%x",&comprc);
        if ((i_returnCode & 0x00FFFFFF) == comprc) {
          /* This came from this source, we will save that as we may use it later */
          retdefine = tokens[1];
          found = 1;
          break;
        }
      }        
    }
  }

  ins.close();

  if (!found && source.length() == 0) {
    ret = "UNDEFINED";
  } else if (!found) {
    ret = "UNDEFINED FROM " + source;
  } else {
    ret = retdefine;
  }
  return ret;
}

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
