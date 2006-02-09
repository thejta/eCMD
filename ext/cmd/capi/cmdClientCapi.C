// Copyright ***********************************************************
//                                                                      
// File cmdClientCapi.C                                   
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
/* $Header$ */
// Module Description **************************************************
//
// Description: 
//
// End Module Description **********************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <string>
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>

#include <ecmdClientCapi.H>
#include <ecmdReturnCodes.H>
#include <cmdClientCapi.H>
#include <ecmdInterpreter.H>
#include <ecmdCommandUtils.H>

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
#ifndef ECMD_STATIC_FUNCTIONS
/* This is from ecmdClientCapiFunc.C */
//extern void * dlHandle;
/* These are from cmdClientCapiFunc.C */
//extern void * cmdDllFnTable[];
#endif
bool cmdInitialized;

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------
uint32_t cmdInitExtension() {

  uint32_t rc = ECMD_SUCCESS;
  /* Nothing to see here, we don't need any plugin functions, this extension is all on the client side */
  /* We don't have to worry about initialization because their client won't compile unless the extension is supported */
  cmdInitialized = true;

  return rc;

}


uint32_t cmdRunCommand(std::string i_command) {
  int c_argc = 0;
  uint32_t rc = ECMD_SUCCESS;

  char* c_argv[21];
  char* buffer = NULL;
  size_t   commlen;
  char buf[200];

  c_argc = 0;
  c_argv[0] = NULL;

  commlen = i_command.length();
  buffer = new char[commlen + 20];

  strcpy(buffer, i_command.c_str());

  /* Now start carving this thing up */
  bool lookingForStart = true; /* Are we looking for the start of a word ? */
  for (size_t c = 0; c < commlen; c++) {
    if (lookingForStart) {
      if (buffer[c] != ' ' && buffer[c] != '\t') {
        c_argv[c_argc++] = &buffer[c];
        lookingForStart = false;
      }
    } else {
      /* Looking for the end */
      if (buffer[c] == ' ' || buffer[c] == '\t') {
        buffer[c] = '\0';
        lookingForStart = true;
      }
    }
    if (c_argc > 20) {
      ecmdOutputError("cmdRunCommand - Found a command with greater then 20 arguments, not supported\n");
      rc = ECMD_INVALID_ARGS;
      break;
    }
  }

  /* We need to push the current state of the target args so that when the user functions call ecmdCommandArgs again, the current state doesn't get lost - CQ #5146 */
  ecmdPushCommandArgs();

  /* We now want to call the command interpreter to handle what the user provided us */
  if (!rc) rc = ecmdCallInterpreters(c_argc, c_argv);

  /* Restore the state of the target args */
  ecmdPopCommandArgs();

  if (rc == ECMD_INT_UNKNOWN_COMMAND) {
    sprintf(buf,"cmdRunCommand -  Unknown Command specified '%s'\n", c_argv[0]);
    ecmdOutputError(buf);
  } else if (rc) {
    std::string parse = ecmdGetErrorMsg(rc, false);
    if (parse.length() > 0) {
      /* Display the registered message right away BZ#160 */
      ecmdOutput(parse.c_str());
    }
    parse = ecmdParseReturnCode(rc);
    sprintf(buf,"cmdRunCommand - '%s' returned with error code %X (%s)\n", c_argv[0], rc, parse.c_str());
    ecmdOutputError(buf);
  }

  if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
    ecmdOutput((i_command + "\n").c_str());
  }

  /* Flush any registered errors so if the client calls in again we won't concat all their past errors on as well */
  ecmdFlushRegisteredErrorMsgs();

  return rc;
}

uint32_t cmdRunCommandCaptureOutput(std::string i_command, std::string & o_output) {
  uint32_t rc = ECMD_SUCCESS;
  int  end = EOF;
  FILE* tempfile;
  char buf[1010];
  int idx;
  int mychar;

  o_output = "";



  /* Create a tmpfile to pipe stdout through */
  tempfile = tmpfile();

  if (tempfile == NULL) {
    ecmdOutputError("cmdRunCommandCaptureOutput - Unable to allocate tempfile to capture output\n");
    return ECMD_FAILURE;
  }

  int stdoutsave = dup(1);

  int newstdout = fileno(tempfile);

  /* Redirect stdout */
  dup2(newstdout, 1);


  /* Ok, we can call it now */
  rc = cmdRunCommand(i_command);

  /* Flush all data */
  fflush(tempfile); 

  /* reset stream position */
  rewind(tempfile);

  /* Let's pop 1000 chars at a time */
  idx = 0;
  while(end != (mychar = getc(tempfile))) {
    buf[idx++] = (char)mychar;
    if (idx == 1000) {
      buf[idx] = '\0';
      o_output += buf;
      idx = 0;
    }
  }
  if (idx > 0) {
    buf[idx] = '\0';
    o_output += buf;
  }    


  /* Restore stdout */
  dup2(stdoutsave, 1);

  close(stdoutsave);
  fclose(tempfile);

  return rc;
}

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
