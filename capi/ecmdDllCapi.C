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
#include <fstream.h>
#include <string.h>
#include <stdlib.h>

#include <ecmdDllCapi.H>
#include <ecmdUtils.H>
#include <ecmdStructs.H>

#undef ecmdDllCapi_C
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
void dllRemoveNullPointers (int * io_argc, char ** io_argv[]);

char * dllParseOptionWithArgs(int * io_argc, char ** io_argv[], const char * i_option);

uint8_t dllRemoveCurrentElement(int curPos, std::string userArgs);

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------
/**
 * @brief Used for storing error messages internally to dll
 */
struct ecmdError {
  int errorCode;        ///< Numeric error code- see ecmdReturnCodes.H
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

} ecmdUserArgs;

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------

int dllLoadDll (const char* i_clientVersion) {

  /* First off let's check our version */
  if (strcmp(i_clientVersion,ECMD_CAPI_VERSION)) {
    fprintf(stderr,"**** FATAL : eCMD DLL and your client are not compatible\n");
    fprintf(stderr,"**** FATAL : Client Version : %s   : DLL Version : %s\n",i_clientVersion, ECMD_CAPI_VERSION);
/*
    if (version < DLL_VERSION)
      fprintf(stderr,"**** FATAL : You must rebuild your client to continue\n");
    else
      fprintf(stderr,"**** FATAL : Contact the Cronus team to have the DLL rebuilt to match your client to continue\n");
*/
    exit(999);
  }

  return dllInitDll();

}

int dllUnloadDll() {
  int rc = 0;
  
  rc = dllFreeDll();

  return rc;
}


std::string dllGetErrorMsg(int i_errorCode) {
  std::string ret;
  std::list<ecmdError>::iterator cur;

  for (cur = ecmdErrorList.begin(); cur != ecmdErrorList.end(); cur++) {
    if ( (*cur).errorCode == i_errorCode ) {
      ret = (*cur).whom + ": " + (*cur).message + " ";
      break;
    }
  }

  ecmdChipTarget dummy;
  std::string filePath;
  int rc = dllQueryFileLocation(dummy, ECMD_FILE_HELPTEXT, filePath); 

  filePath += "ecmdReturnCodes.H";
  if (rc) {
    ret = "ERROR FINDING DECODE FILE";
    return ret;
  }

  /* jtw 10/6/03 - code below largely copied from cronusrc.c */
  char str[800];
  char num[30];
  char name[200];
  int found = 0;
  char* tempstr = NULL;
  unsigned int comprc;

  ifstream ins(filePath.c_str());

  if (ins.fail()) {
    ret = "ERROR OPENING DECODE FILE";
    return ret;
  }

  while (!ins.eof()) { /*  && (strlen(str) != 0) */
    ins.getline(str,799,'\n');
    if (!strncmp(str,"#define",7)) {
      strtok(str," \t");        /* get rid of the #define */
      tempstr = strtok(NULL," \t");
      if (tempstr == NULL) continue;
      strcpy(name,tempstr);
      tempstr = strtok(NULL," \t");
      if (tempstr == NULL) continue;
      strcpy(num,tempstr);
      sscanf(num,"%x",&comprc);
      if (comprc == i_errorCode) {
        ret = name;
        found = 1;
        break;
      }
    }
  }

  ins.close();

  if (!found) {
    ret = "UNDEFINED";
  }

  return ret;
}

int dllRegisterErrorMsg(int i_errorCode, const char* i_whom, const char* i_message) {
  int rc = ECMD_SUCCESS;

  ecmdError curError;
  curError.errorCode = i_errorCode;
  curError.whom = i_whom;
  curError.message = i_message;

  ecmdErrorList.push_back(curError);

  return rc;
}

int dllQuerySelected(ecmdChipTarget & i_target, ecmdQueryData & o_queryData) {
  int rc = ECMD_SUCCESS;

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
  if (ecmdUserArgs.cage == allFlag) {
    i_target.cageState = ECMD_TARGET_QUERY_WILDCARD;
    cageType = ALL;
  }
  else if (ecmdUserArgs.cage.find_first_of(patterns) < ecmdUserArgs.cage.length()) {
    i_target.cageState = ECMD_TARGET_QUERY_WILDCARD;
    cageType = MULTI;
  }
  else {

    if (ecmdUserArgs.cage.length() != 0) {
      i_target.cage = atoi(ecmdUserArgs.cage.c_str());
    }
    else {
      i_target.cage = 0x0;
    }

    cageType = SINGLE;
    i_target.cageState = ECMD_TARGET_QUERY_FIELD_VALID;
  }

  //node
  if (ecmdUserArgs.node == allFlag) {
    i_target.nodeState = ECMD_TARGET_QUERY_WILDCARD;
    nodeType = ALL;
  }
  else if (ecmdUserArgs.node.find_first_of(patterns) < ecmdUserArgs.node.length()) {
    i_target.nodeState = ECMD_TARGET_QUERY_WILDCARD;
    nodeType = MULTI;
  }
  else {

    if (ecmdUserArgs.node.length() != 0) {
      i_target.node = atoi(ecmdUserArgs.node.c_str());
    }
    else {
      i_target.node = 0x0;
    }

    nodeType = SINGLE;
    i_target.nodeState = ECMD_TARGET_QUERY_FIELD_VALID;
  }

  //slot
  if (ecmdUserArgs.slot == allFlag) {
    i_target.slotState = ECMD_TARGET_QUERY_WILDCARD;
    slotType = ALL;
  }
  else if (ecmdUserArgs.slot.find_first_of(patterns) < ecmdUserArgs.slot.length()) {
    i_target.slotState = ECMD_TARGET_QUERY_WILDCARD;
    slotType = MULTI;
  }
  else {

    if (ecmdUserArgs.slot.length() != 0) {
      i_target.slot = atoi(ecmdUserArgs.slot.c_str());
    }
    else {
      i_target.slot = 0x0;
    }

    slotType = SINGLE;
    i_target.slotState = ECMD_TARGET_QUERY_FIELD_VALID;
  }

  //position
  if (ecmdUserArgs.pos == allFlag) {
    posType = ALL;
    i_target.posState = ECMD_TARGET_QUERY_WILDCARD;
  }
  else if (ecmdUserArgs.pos.find_first_of(patterns) < ecmdUserArgs.pos.length()) {
    posType = MULTI;
    i_target.posState = ECMD_TARGET_QUERY_WILDCARD;
  }
  else {

    if (ecmdUserArgs.pos.length() != 0) {
      i_target.pos = atoi(ecmdUserArgs.pos.c_str());
    }
    else {
      i_target.pos = 0x0;
    }

    posType = SINGLE;
    i_target.posState = ECMD_TARGET_QUERY_FIELD_VALID;
  }

  //core
  if (ecmdUserArgs.core == allFlag ) {
    i_target.coreState = ECMD_TARGET_QUERY_WILDCARD;
    coreType = ALL;
  }
  else if (ecmdUserArgs.core.find_first_of(patterns) < ecmdUserArgs.core.length()) {
    i_target.coreState = ECMD_TARGET_QUERY_WILDCARD;
    coreType = MULTI;
  }
  else {

    if (ecmdUserArgs.core.length() != 0) {
      i_target.core = atoi(ecmdUserArgs.core.c_str());
    }
    else {
      i_target.core = 0x0;
    }

    coreType = SINGLE;
    i_target.coreState = ECMD_TARGET_QUERY_FIELD_VALID;
  }

  //thread
  if (ecmdUserArgs.thread == allFlag || ecmdUserArgs.thread == "alive") {
    i_target.threadState = ECMD_TARGET_QUERY_WILDCARD;
    threadType = ALL;
  }
  else if (ecmdUserArgs.thread.find_first_of(patterns) < ecmdUserArgs.thread.length()) {
    i_target.threadState = ECMD_TARGET_QUERY_WILDCARD;
    threadType = MULTI;    
  }
  else {

    if (ecmdUserArgs.thread.length() != 0) {
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

int dllCommonCommandArgs(int*  io_argc, char** io_argv[]) {
  int rc = ECMD_SUCCESS;

  /* We need to pull out the targeting options here, and
   store them away for future use */
  char * curArg;
  //cage - the "-k" was Larry's idea, I just liked it - jtw
  curArg = dllParseOptionWithArgs(io_argc, io_argv, "-k");
  if (curArg != NULL) {
    ecmdUserArgs.cage = curArg;
  }
  else {
    ecmdUserArgs.cage = "";
  }

  //node
  curArg = dllParseOptionWithArgs(io_argc, io_argv, "-n");
  if (curArg != NULL) {
    ecmdUserArgs.node = curArg;
  }
  else {
    ecmdUserArgs.node = "";
  }

  //slot
  curArg = dllParseOptionWithArgs(io_argc, io_argv, "-s");
  if (curArg != NULL) {
    ecmdUserArgs.slot = curArg;
  }
  else {
    ecmdUserArgs.slot = "";
  }

  //position
  curArg = dllParseOptionWithArgs(io_argc, io_argv, "-p");
  if (curArg != NULL) {
    ecmdUserArgs.pos = curArg;
  }
  else {
    ecmdUserArgs.pos = "";
  }

  //core
  curArg = dllParseOptionWithArgs(io_argc, io_argv, "-c");
  if (curArg != NULL) {
    ecmdUserArgs.core = curArg;
  }
  else {
    ecmdUserArgs.core = "";
  }

  //thread
  curArg = dllParseOptionWithArgs(io_argc, io_argv, "-t");
  if (curArg != NULL) {
    ecmdUserArgs.thread = curArg;
  }
  else {
    ecmdUserArgs.thread = "";
  }

  /* Call the dllSpecificFunction */
  rc = dllSpecificCommandArgs(io_argc,io_argv);

  return rc;
}



void dllRemoveNullPointers (int *argc, char **argv[]) {
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

char * dllParseOptionWithArgs(int *argc, char **argv[], const char *option) {
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

  dllRemoveNullPointers(argc, argv);

  return returnValue;
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

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
