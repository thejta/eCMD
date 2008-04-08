/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File ecmdMain.C                                   
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
/**
 @file ecmdMain.C
 @brief Main Program entry point for ecmdDllClient Application
*/


//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------


#include <stdio.h>
#include <string>

#include <ecmdClientCapi.H>
#include <ecmdInterpreter.H>
#include <ecmdReturnCodes.H>
#include <ecmdCommandUtils.H>
#include <ecmdSharedUtils.H>


int main (int argc, char *argv[])
{
  uint32_t rc = 0;

  std::string cmdSave;
  char errorbuf[200];
  for (int i = 0; i < argc; i++) {
    cmdSave += argv[i];
    cmdSave += " ";
  }
  cmdSave += "\n";

  rc = ecmdLoadDll("");
  if (rc) {
    ecmdLoadDllRecovery(cmdSave, rc);
  }

  if (rc == ECMD_SUCCESS) {
    /* Check to see if we are using stdin to pass in multiple commands */
    bool shellMode = ecmdParseOption(&argc, &argv, "-shell");
    bool stdinMode = ecmdParseOption(&argc, &argv, "-stdin");
    if (stdinMode || shellMode) {

      /* Grab any other args that may be there */
      rc = ecmdCommandArgs(&argc, &argv);
      if (rc) exit((int)rc);

      /* There shouldn't be any more args when doing a -stdin/-shell */
      if (argc > 1) {
        ecmdOutputError("ecmd - Invalid args passed to ecmd in -stdin/-shell mode\n");
        exit(ECMD_INVALID_ARGS);
      }

      /* Let's get things going */
      std::vector<std::string> commands;
      int   c_argc;
      char* c_argv[ECMD_ARG_LIMIT + 1];       ///< A limit of 20 tokens(args) per command
      char* buffer = NULL;
      size_t   bufflen = 0;
      size_t   commlen;
      bool shellAlive = true;

      if (shellMode) {
        ecmdOutput("ecmd> "); fflush(0);
      }

      /* ecmdParseStdInCommands reads from stdin and returns a vector of strings */
      /*  each string contains one command (ie 'ecmdquery version')              */
      /* When Ctrl-D or EOF is reached this function will fail to break out of loop */
      while (shellAlive && (rc = ecmdParseStdinCommands(commands))) {

        rc = 0;

        /* Walk through individual commands from ecmdParseStdInCommands */
        for (std::vector< std::string >::iterator commandIter = commands.begin(); commandIter != commands.end(); commandIter++) {

          c_argc = 0;
          c_argv[0] = NULL;

          /* Check for a comment or empty line, if so delete it */
          if ((*commandIter)[0] == '#') {
            continue;
          } else if (commandIter->length() == 0) {
            continue;
          }
          if (shellMode) {
            if ((*commandIter) == "quit" || (*commandIter) == "exit") {
              ecmdOutput("Leaving ecmd shell at users request... \n");
              shellAlive = false;
              break;
            }
          }

          /* Create a char buffer to hold the whole command, we will use this to create pointers to each token in the command (like argc,argv) */
          commlen = commandIter->length();
          if ( commlen > bufflen) {
            if (buffer != NULL) delete[] buffer;
            buffer = new char[commlen + 20];
            bufflen = commlen + 19;
          }

          // Beam "error" of possible NULL 'buffer' value requires mutually
          // exclusive conditions (need an argument present to enter 
          // "commands" FOR loop but commandIter->length = 0 ie. no command).  So
          // tell beam to ignore NULL pointer message for 'buffer' parm via
          // comment on next line.
          //lint -e(668) Ignore passing null, same as above for lint
          strcpy(buffer, commandIter->c_str()); /*passing null object*/

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
            if (c_argc > ECMD_ARG_LIMIT) {
              sprintf(errorbuf,"ecmd - Found a command with greater then %d arguments, not supported\n",ECMD_ARG_LIMIT);
              ecmdOutputError(errorbuf);
              rc = ECMD_INVALID_ARGS;
              break;
            }
          }

          // ignore a line if it is only space or tabs - 
          // This prevents c_argv[o] being accessed below when still pointing to NULL
          if (c_argc == 0) continue;

          // Before Executing the cmd save it on the Dll side 
          ecmdSetCurrentCmdline(c_argc, c_argv);

          /* We now want to call the command interpreter to handle what the user provided us */
          if (!rc) rc = ecmdCallInterpreters(c_argc, c_argv);


          if (rc == ECMD_INT_UNKNOWN_COMMAND) {
            if (strlen(c_argv[0]) < 200)
              sprintf(errorbuf,"ecmd -  Unknown Command specified '%s'\n", c_argv[0]);
            else
              sprintf(errorbuf,"ecmd -  Unknown Command specified \n");
            ecmdOutputError(errorbuf);
          } else if (rc) {
            std::string parse = ecmdGetErrorMsg(rc, false);
            if (parse.length() > 0) {
              /* Display the registered message right away BZ#160 */
              ecmdOutput(parse.c_str());
            }
            parse = ecmdParseReturnCode(rc);
            if (strlen(c_argv[0]) + parse.length() < 300)
              sprintf(errorbuf,"ecmd - '%s' returned with error code %X (%s)\n", c_argv[0], rc, parse.c_str());
            else
              sprintf(errorbuf,"ecmd - Command returned with error code %X (%s)\n", rc, parse.c_str());
            ecmdOutputError(errorbuf);
            break;
          }

          if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
            ecmdOutput((*commandIter + "\n").c_str());
          }
        } /* tokens loop */
        if (rc) break;

        /* Print the prompt again */
        if (shellMode && shellAlive) {
          ecmdOutput("ecmd> "); fflush(0);
        }
      }
      if (buffer != NULL) delete[] buffer;

    } else {
      /* Standard command line command */

      // Before Executing the cmd save it on the Dll side 
      ecmdSetCurrentCmdline(argc-1, argv+1);

      /* We now want to call the command interpreter to handle what the user provided us */
      rc = ecmdCallInterpreters(argc - 1, argv + 1);


      if (rc == ECMD_INT_UNKNOWN_COMMAND) {
        if (argv[1] == NULL)
          sprintf(errorbuf,"ecmd - Must specify a command to execute. Run 'ecmd -h' for command list.\n");
        else if (strlen(argv[1]) < 200)
          sprintf(errorbuf,"ecmd - Unknown Command specified '%s'\n", argv[1]);
        else
          sprintf(errorbuf,"ecmd - Unknown Command specified \n");
        ecmdOutputError(errorbuf);
      } else if (rc) {
        std::string parse = ecmdGetErrorMsg(rc, false);
        if (parse.length() > 0) {
          /* Display the registered message right away BZ#160 */
          ecmdOutput(parse.c_str());
        }
        parse = ecmdParseReturnCode(rc);
        if (strlen(argv[1]) + parse.length() < 300)
          sprintf(errorbuf,"ecmd - '%s' returned with error code %X (%s)\n", argv[1], rc, parse.c_str());
        else
          sprintf(errorbuf,"ecmd - Command returned with error code %X (%s)\n", rc, parse.c_str());
        ecmdOutputError(errorbuf);
      }
    }


    /* Move these outputs into the if !rc to fix BZ#224 - cje */
    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      ecmdOutput(cmdSave.c_str());
    }


    ecmdUnloadDll();
  }

  exit((int)rc);
}





// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//  none STGC7449      04/18/05 prahl    Clean up Beam messages.
//
// End Change Log *****************************************************
