// Copyright ***********************************************************
//                                                                      
// File cipMain.C                                   
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
// Description: CIP Extension main function
//
// End Module Description **********************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define cipMain_C
#include <stdio.h>

#include <ecmdClientCapi.H>
#include <ecmdReturnCodes.H>
#include <ecmdUtils.H>
#include <cipClientCapi.H>
#include <cipInterpreter.H>
#include <ecmdCommandUtils.H>

#undef cipMain_C
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


    /* Initialize the CIP extension */
    rc = cipInitExtension();
    if (rc) {
      sprintf(buf,"cipecmd - Unable to initialize the extension, CIP extension is not supported by current plugin\n");
      ecmdOutputError(buf);
      break;
    }

    /* We now want to call the command interpreter to handle what the user provided us */
    rc = cipCommandInterpreter(argc - 1, argv + 1);


    if (rc == ECMD_INT_UNKNOWN_COMMAND) {
      sprintf(buf,"cipecmd -  Unknown Command specified '%s'\n", argv[1]);
      ecmdOutputError(buf);
      break;
    } else if (rc) {
      std::string parse = ecmdGetErrorMsg(rc);
      if (parse.length() > 0) {
        /* Display the registered message right away BZ#160 */
        ecmdOutput(parse.c_str());
      }
      parse = ecmdParseReturnCode(rc);
      sprintf(buf,"cipecmd - '%s' returned with error code %X (%s)\n", argv[1], rc, parse.c_str());
      ecmdOutputError(buf);
      break;
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
