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
#include <ifstream>
#include <string.h>
#include <stdlib.h>

#include <ecmdDllCapi.H>
#include <ecmdUtils.H>

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

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------
std::list<ecmdError> ecmdErrorList;

ecmdUserSelectInfo ecmdUserArgs;

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
      if (atol(num) == rc) {
        ret += name;
        found = 1;
        break;
      }
    }
  }

  ins.close();

  if (!found) {
    ret += "UNDEFINED";
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

int dllQuerySelected(ecmdChipTarget & i_target, std::vector<ecmdCageData> & o_queryData) {
  int rc = ECMD_SUCCESS;

  //update target with useful info in the ecmdUserArgs struct
  //cage
  if (!ecmdUserArgs.cage) {
    i_target.cage = 0x0;
    i_target.cageState = ECMD_TARGET_QUERY_FIELD_VALID;
  }
  else if (ecmdUserArgs.cage == "all" || ecmdUserArgs.cage.find(",") != std::string::npos) {
    i_target.cageState = ECMD_TARGET_QUERY_WILDCARD;
  }

  //node
  if (!ecmdUserArgs.node) {
    i_target.node = 0x0;
    i_target.nodeState = ECMD_TARGET_QUERY_FIELD_VALID;
  }
  else if (ecmdUserArgs.node == "all" || ecmdUserArgs.node.find(",") != std::string::npos) {
    i_target.nodeState = ECMD_TARGET_QUERY_WILDCARD;
  }

  //position
  if (!ecmdUserArgs.pos) {
    i_target.pos = 0x0;
    i_target.posState = ECMD_TARGET_QUERY_FIELD_VALID;
  }
  else if (ecmdUserArgs.pos == "all" || ecmdUserArgs.pos.find(",") != std::string::npos) {
    i_target.posState = ECMD_TARGET_QUERY_WILDCARD;
  }

  //core
  if (!ecmdUserArgs.core) {
    i_target.core = 0x0;
    i_target.coreState = ECMD_TARGET_QUERY_FIELD_VALID;
  }
  else if (ecmdUserArgs.core == "all" || ecmdUserArgs.core.find(",") != std::string::npos) {
    i_target.coreState = ECMD_TARGET_QUERY_WILDCARD;
  }

  //thread
  if (!ecmdUserArgs.thread) {
    i_target.thread = 0x0;
    i_target.threadState = ECMD_TARGET_QUERY_FIELD_VALID;
  }
  else if (ecmdUserArgs.thread == "all" || ecmdUserArgs.thread.find(",") != std::string::npos) {
    i_target.threadState = ECMD_TARGET_QUERY_WILDCARD;
  }

  return rc;
}

int dllCommonCommandArgs(int*  io_argc, char** io_argv[]) {
  int rc = ECMD_SUCCESS;

  /* We need to pull out the targeting options here, and
   store them away for future use */

  //cage - the "-k" was Larry's idea, I just liked it - jtw
  ecmdUserArgs.cage = ecmdParseOptionWithArgs(io_argc, io_argv, "-k");

  //node
  ecmdUserArgs.node = ecmdParseOptionWithArgs(io_argc, io_argv, "-n");

  //position
  ecmdUserArgs.pos = ecmdParseOptionWithArgs(io_argc, io_argv, "-p");

  //core
  ecmdUserArgs.core = ecmdParseOptionWithArgs(io_argc, io_argv, "-c");

  //thread
  ecmdUserArgs.thread = ecmdParseOptionWithArgs(io_argc, io_argv, "-t");

  /* Call the dllSpecificFunction */
  rc = dllSpecificCommandArgs(io_argc,io_argv);

  return rc;
}


// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
