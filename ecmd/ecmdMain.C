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
/* $Header$ */

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
#define ecmdMain_C

#include <stdio.h>
#include <string>

#include <ecmdClientCapi.H>
#include <ecmdInterpreter.H>
#include <ecmdReturnCodes.H>
#include <ecmdCommandUtils.H>
#include <ecmdSharedUtils.H>

#undef ecmdMain_C

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


  if (getenv ("ECMD_DLL_FILE") == NULL) {
    printf("ecmd - You must set ECMD_DLL_FILE in order to run the eCMD command line client\n");
    rc = ECMD_INVALID_DLL_FILENAME;
  } else {
    /* Load the one specified by ECMD_DLL_FILE */
    rc = ecmdLoadDll("");
  }

  if (!rc) {


    /* Check to see if we are using stdin to pass in multiple commands */
    if (ecmdParseOption(&argc, &argv, "-stdin")) {

      /* Grab any other args that may be there */
      rc = ecmdCommandArgs(&argc, &argv);
      if (rc) return rc;

      if (argc > 1) {
        ecmdOutputError("ecmd - Invalid args passed to ecmd in -stdin mode\n");
        rc = ECMD_INVALID_ARGS;

      } else {

        std::vector< std::string > commands;
        int   c_argc;
        char* c_argv[21];
        char* buffer = NULL;
        int   bufflen = 0;
        int   commlen;

        while ((rc = ecmdParseStdinCommands(commands)) != ECMD_SUCCESS) {

          rc = 0;
          for (std::vector< std::string >::iterator commit = commands.begin(); commit != commands.end(); commit ++) {

            c_argc = 0;

            commlen = commit->length();
            if ( commlen > bufflen) {
              if (buffer != NULL) delete[] buffer;
              buffer = new char[commlen + 20];
              bufflen = commlen + 19;
            }

            strcpy(buffer, commit->c_str());

            /* Now start carving this thing up */
            bool lookingForStart = true; /* Are we looking for the start of a word ? */
            for (int c = 0; c < commlen; c++) {
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
                ecmdOutputError("ecmd - Found a command with greater then 20 arguments, not supported\n");
                rc = ECMD_INVALID_ARGS;
                break;
              }
            }

            /* We now want to call the command interpreter to handle what the user provided us */
            if (!rc) rc = ecmdCallInterpreters(c_argc, c_argv);


            if (rc == ECMD_INT_UNKNOWN_COMMAND) {
              sprintf(buf,"ecmd -  Unknown Command specified '%s'\n", c_argv[0]);
              ecmdOutputError(buf);
            } else if (rc) {
              std::string parse = ecmdGetErrorMsg(rc, false);
              if (parse.length() > 0) {
                /* Display the registered message right away BZ#160 */
                ecmdOutput(parse.c_str());
              }
              parse = ecmdParseReturnCode(rc);
              sprintf(buf,"ecmd - '%s' returned with error code %X (%s)\n", c_argv[0], rc, parse.c_str());
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
        sprintf(buf,"ecmd -  Unknown Command specified '%s'\n", argv[1]);
        ecmdOutputError(buf);
      } else if (rc) {
        std::string parse = ecmdGetErrorMsg(rc, false);
        if (parse.length() > 0) {
          /* Display the registered message right away BZ#160 */
          ecmdOutput(parse.c_str());
        }
        parse = ecmdParseReturnCode(rc);
        sprintf(buf,"ecmd - '%s' returned with error code %X (%s)\n", argv[1], rc, parse.c_str());
        ecmdOutputError(buf);
      }
    }


    /* Move these outputs into the if !rc to fix BZ#224 - cje */
    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      ecmdOutput(cmdsave.c_str());
    }


    ecmdUnloadDll();

  }

  exit(rc);

}





// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
