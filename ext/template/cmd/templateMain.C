/* NOTE : This file has been generated from the eCMD extension temp-late */
/* DO NOT EDIT THISFILE (unless you are editing the temp-late */






// Copyright ***********************************************************
//                                                                      
// File templateMain.C                                   
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
// Description: TEMPLATE Extension main function
//
// End Module Description **********************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define templateMain_C
#include <stdio.h>

#include <ecmdClientCapi.H>
#include <ecmdReturnCodes.H>
#include <ecmdUtils.H>
#include <templateClientCapi.H>
#include <templateInterpreter.H>
#include <ecmdCommandUtils.H>
#include <ecmdSharedUtils.H>

#undef templateMain_C
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

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------

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

  /* Single exit point */
  while (1) {


    /* Load the one specified by ECMD_DLL_FILE */
    rc = ecmdLoadDll("");
    if (rc) break;


    /* Initialize the TEMPLATE extension */
    rc = templateInitExtension();
    if (rc) {
      sprintf(buf,"templateecmd - Unable to initialize the extension, TEMPLATE extension is not supported by current plugin\n");
      ecmdOutputError(buf);
      break;
    }


    /* Check to see if we are using stdin to pass in multiple commands */
    if (ecmdParseOption(&argc, &argv, "-stdin")) {

      /* Grab any other args that may be there */
      rc = ecmdCommandArgs(&argc, &argv);
      if (rc) return rc;

      if (argc > 1) {
        ecmdOutputError("templateecmd - Invalid args passed to ecmd in -stdin mode\n");
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
                ecmdOutputError("templateecmd - Found a command with greater then 20 arguments, not supported\n");
                rc = ECMD_INVALID_ARGS;
                break;
              }
            }

            /* We now want to call the command interpreter to handle what the user provided us */
            if (!rc) rc = templateCommandInterpreter(c_argc, c_argv);


            if (rc == ECMD_INT_UNKNOWN_COMMAND) {
              sprintf(buf,"templateecmd -  Unknown Command specified '%s'\n", c_argv[0]);
              ecmdOutputError(buf);
            } else if (rc) {
              std::string parse = ecmdGetErrorMsg(rc, false);
              if (parse.length() > 0) {
                /* Display the registered message right away BZ#160 */
                ecmdOutput(parse.c_str());
              }
              parse = ecmdParseReturnCode(rc);
              sprintf(buf,"templateecmd - '%s' returned with error code %X (%s)\n", c_argv[0], rc, parse.c_str());
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
      /* Standard command line command */    /* We now want to call the command interpreter to handle what the user provided us */

      rc = templateCommandInterpreter(argc - 1, argv + 1);


      if (rc == ECMD_INT_UNKNOWN_COMMAND) {
        sprintf(buf,"templateecmd -  Unknown Command specified '%s'\n", argv[1]);
        ecmdOutputError(buf);
        break;
      } else if (rc) {
        std::string parse = ecmdGetErrorMsg(rc, false);
        if (parse.length() > 0) {
          /* Display the registered message right away BZ#160 */
          ecmdOutput(parse.c_str());
        }
        parse = ecmdParseReturnCode(rc);
        sprintf(buf,"templateecmd - '%s' returned with error code %X (%s)\n", argv[1], rc, parse.c_str());
        ecmdOutputError(buf);
        break;
      }
    }
    break;
  } /* single exit point */

  ecmdOutput(cmdsave.c_str());

  ecmdUnloadDll();

  exit(rc);

}

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
