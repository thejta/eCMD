/* $Header$ */
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
// Module Description **************************************************
//
// Description: 
//
// End Module Description **********************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdio.h>
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
uint32_t parseCommandFile(std::string i_string, std::vector<std::string> & o_commands);

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

#define ERRORBUF_SIZE 200

uint32_t cmdRunCommand(std::string i_command) {
  int c_argc = 0;
  uint32_t rc = ECMD_SUCCESS;
  char errorbuf[ERRORBUF_SIZE];

  char* c_argv[21];
  char* buffer = NULL;
  size_t   commlen;

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
  if (!rc) {
    rc = ecmdCallInterpreters(c_argc, c_argv);
  }

  /* Restore the state of the target args */
  ecmdPopCommandArgs();

  if (rc == ECMD_INT_UNKNOWN_COMMAND) {
    if (c_argv[0] == NULL)
      sprintf(errorbuf,"ecmd - Must specify a command to execute. Run 'ecmd -h' for command list.\n");
    else if (strlen(c_argv[0]) < (ERRORBUF_SIZE - 50))
      sprintf(errorbuf,"ecmd - Unknown Command specified '%s'\n", c_argv[0]);
    else
      sprintf(errorbuf,"ecmd - Unknown Command specified \n");
    ecmdOutputError(errorbuf);
  }

  // See if any errors are left, regardless of if we have a return code.  This will prevent error messages from being lost
  std::string parse = ecmdGetErrorMsg(ECMD_GET_ALL_REMAINING_ERRORS, false);
  /* Display the registered message right away BZ#160 */
  if (parse.length() > 0) {
    ecmdOutput(parse.c_str());
  }

  // If we did get a return code, parse that and print it to the screen
  if (rc) {
    parse = ecmdParseReturnCode(rc);
    if (strlen(c_argv[0]) + parse.length() < (ERRORBUF_SIZE - 50))
      sprintf(errorbuf,"ecmd - '%s' returned with error code 0x%X (%s)\n", c_argv[0], rc, parse.c_str());
    else
      sprintf(errorbuf,"ecmd - Command returned with error code 0x%X (%s)\n", rc, parse.c_str());
    ecmdOutputError(errorbuf);
  }

  /* Only print to the screen if quietmode isn't on */
  if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
    ecmdOutput(i_command.c_str());
  }

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

uint32_t cmdRunCommandFile(std::string i_file) {
  uint32_t rc = ECMD_SUCCESS;
  std::vector<std::string> commands;

  // Get my commands in an easy to use format
  rc = parseCommandFile(i_file, commands);
  if (rc) return rc;

  /* Loop through all our commands and execute them */
  for (uint32_t idx = 0; idx < commands.size(); idx++) {
    rc = cmdRunCommand(commands[idx]);
    if (rc) return rc;
  }

  return rc;
}

uint32_t cmdRunCommandFileCaptureOutput(std::string i_file, std::string & o_output) {
  uint32_t rc = ECMD_SUCCESS;
  std::vector<std::string> commands;

  // The single command interface resets the input var each time, so don't just pass in o_output
  std::string partialOutput;

  o_output = "";

  // Get my commands in an easy to use format
  rc = parseCommandFile(i_file, commands);
  if (rc) return rc;

  /* Loop through all our commands and execute them */
  for (uint32_t idx = 0; idx < commands.size(); idx++) {
    rc = cmdRunCommandCaptureOutput(commands[idx], partialOutput);
    if (rc) return rc;
    /* Accumulate our output */
    o_output += partialOutput;
  }

  return rc;
}

uint32_t parseCommandFile(std::string i_file, std::vector<std::string> & o_commands) {
  uint32_t rc = ECMD_SUCCESS;
  std::string line;

  /* Open it */
  std::ifstream file(i_file.c_str());
  if (file.fail()) {
    ecmdOutputError("Unable to open input file\n");
    return ECMD_INVALID_ARGS;
  }

  /* Loop through it until end of file */
  while (!file.eof()) {
    getline(file, line, '\n');
    o_commands.push_back(line);
  }

  /* Close it */
  file.close();

  return rc;
}
