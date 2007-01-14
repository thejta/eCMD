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

  std::string cmdsave;
  char buf[200];
  for (int i = 0; i < argc; i++) {
    cmdsave += argv[i];
    cmdsave += " ";
  }

  cmdsave += "\n";


#ifndef ECMD_STATIC_FUNCTIONS
// If building the function 'statically', it doesn't need ECMD_DLL_FILE set; it'll automatically load its DLL
  if (getenv ("ECMD_DLL_FILE") == NULL) {
    printf("ecmd - You must set ECMD_DLL_FILE in order to run the eCMD command line client\n");
    rc = ECMD_INVALID_DLL_FILENAME;
  } else {
    /* Load the one specified by ECMD_DLL_FILE */
    rc = ecmdLoadDll("");
  }
#else
  rc = ecmdLoadDll("");
#endif

  if (rc == ECMD_SUCCESS) {
    /* Check to see if we are using stdin to pass in multiple commands */
    if (ecmdParseOption(&argc, &argv, "-stdin")) {

      /* Grab any other args that may be there */
      rc = ecmdCommandArgs(&argc, &argv);
      if (rc) exit((int)rc);

      /* There shouldn't be any more args when doing a -stdin */
      if (argc > 1) {
        ecmdOutputError("ecmd - Invalid args passed to ecmd in -stdin mode\n");
        rc = ECMD_INVALID_ARGS;

      } else {

        std::vector< std::string > commands;
        int   c_argc;
        char* c_argv[ECMD_ARG_LIMIT + 1];       ///< A limit of 20 tokens(args) per command
        char* buffer = NULL;
        size_t   bufflen = 0;
        size_t   commlen;

        /* ecmdParseStdInCommands reads from stdin and returns a vector of strings */
        /*  each string contains one command (ie 'ecmdquery version')              */
        /* When Ctrl-D or EOF is reached this function will fail to break out of loop */
        while ((rc = ecmdParseStdinCommands(commands)) != ECMD_SUCCESS) {

          rc = 0;

          /* Walk through individual commands from ecmdParseStdInCommands */
          for (std::vector< std::string >::iterator commit = commands.begin(); commit != commands.end(); commit ++) {

            c_argc = 0;
	    c_argv[0] = NULL;

            /* Check for a comment or empty line, if so delete it */
            if ((*commit)[0] == '#') continue;
            else if (commit->length() == 0) continue;

            /* Create a char buffer to hold the whole command, we will use this to create pointers to each token in the command (like argc,argv) */
            commlen = commit->length();
            if ( commlen > bufflen) {
              if (buffer != NULL) delete[] buffer;
              buffer = new char[commlen + 20];
              bufflen = commlen + 19;
            }

            // Beam "error" of possible NULL 'buffer' value requires mutually
            // exclusive conditions (need an argument present to enter 
            // "commands" FOR loop but commit->length = 0 ie. no command).  So
            // tell beam to ignore NULL pointer message for 'buffer' parm via
            // comment on next line.
	    //lint -e(668) Ignore passing null, same as above for lint
            strcpy(buffer, commit->c_str()); /*passing null object*/

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
		sprintf(buf,"ecmd - Found a command with greater then %d arguments, not supported\n",ECMD_ARG_LIMIT);
                ecmdOutputError(buf);
                rc = ECMD_INVALID_ARGS;
                break;
              }
            }

            // ignore a line if it is only space or tabs - 
            // This prevents c_argv[o] being accessed below when still pointing to NULL
            if (c_argc == 0) continue;


            /* We now want to call the command interpreter to handle what the user provided us */
            if (!rc) rc = ecmdCallInterpreters(c_argc, c_argv);


            if (rc == ECMD_INT_UNKNOWN_COMMAND) {
              if (strlen(c_argv[0]) < 200)
                sprintf(buf,"ecmd -  Unknown Command specified '%s'\n", c_argv[0]);
              else
                sprintf(buf,"ecmd -  Unknown Command specified \n");
              ecmdOutputError(buf);
            } else if (rc) {
              std::string parse = ecmdGetErrorMsg(rc, false);
              if (parse.length() > 0) {
                /* Display the registered message right away BZ#160 */
                ecmdOutput(parse.c_str());
              }
              parse = ecmdParseReturnCode(rc);
              if (strlen(c_argv[0]) + parse.length() < 300)
                sprintf(buf,"ecmd - '%s' returned with error code %X (%s)\n", c_argv[0], rc, parse.c_str());
              else
                sprintf(buf,"ecmd - Command returned with error code %X (%s)\n", rc, parse.c_str());
              ecmdOutputError(buf);
              break;
            }

            if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
              ecmdOutput((*commit + "\n").c_str());
            }

          } /* tokens loop */
          if (rc) break;

        }
        if (buffer != NULL) delete[] buffer;

      } /* invalid args if */

    } else {
      /* Standard command line command */

      /* We now want to call the command interpreter to handle what the user provided us */
      rc = ecmdCallInterpreters(argc - 1, argv + 1);


      if (rc == ECMD_INT_UNKNOWN_COMMAND) {
        if (argv[1] == NULL)
          sprintf(buf,"ecmd - Must specify a command to execute. Run 'ecmd -h' for command list.\n");
        else if (strlen(argv[1]) < 200)
          sprintf(buf,"ecmd - Unknown Command specified '%s'\n", argv[1]);
        else
          sprintf(buf,"ecmd - Unknown Command specified \n");
        ecmdOutputError(buf);
      } else if (rc) {
        std::string parse = ecmdGetErrorMsg(rc, false);
        if (parse.length() > 0) {
          /* Display the registered message right away BZ#160 */
          ecmdOutput(parse.c_str());
        }
        parse = ecmdParseReturnCode(rc);
        if (strlen(argv[1]) + parse.length() < 300)
          sprintf(buf,"ecmd - '%s' returned with error code %X (%s)\n", argv[1], rc, parse.c_str());
        else
          sprintf(buf,"ecmd - Command returned with error code %X (%s)\n", rc, parse.c_str());
        ecmdOutputError(buf);
      }
    }


    /* Move these outputs into the if !rc to fix BZ#224 - cje */
    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      ecmdOutput(cmdsave.c_str());
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
