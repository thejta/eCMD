// Copyright ***********************************************************
//                                                                      
// File cipInterpreter.C                                   
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
// Description: CIP Extension command interpreter
//
// End Module Description **********************************************
/* $Header$ */

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define cipInterpreter_C

#include <cipInterpreter.H>
#include <ecmdReturnCodes.H>
#include <ecmdUtils.H>
#include <ecmdSharedUtils.H>
#include <ecmdCommandUtils.H>

#undef cipInterpreter_C
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

uint32_t cipCommandInterpreter(int argc, char* argv[]) {

  uint32_t rc = ECMD_INT_UNKNOWN_COMMAND;


  if (argc >= 1) {

    /* Let's make sure it is ours */
    if (strncmp(argv[0],"cip",3)) {
      return ECMD_INT_UNKNOWN_COMMAND;
    }

    /* Let's handle the '-h' arg right here */
    if (ecmdParseOption(&argc, &argv, "-h")) {
      ecmdPrintHelp(argv[0]);
      return rc;
    }

    switch (argv[0][3]) {

        /************************/
        /* The I's              */
        /************************/
      case 'i':

        if (!strcmp(argv[0], "cipinstruct")) {
          rc = cipInstructUser(argc - 1, argv + 1);
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;
	/************************/
        /* The B's              */
        /************************/
      case 'b':

        if (!strcmp(argv[0], "cipbreakpoint")) {
          rc = cipBreakpointUser(argc - 1, argv + 1);
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;
      default:
        /* We don't understand this function, let's let the caller know */
        rc = ECMD_INT_UNKNOWN_COMMAND;
        break;
    }



  } /* End if (argc >= 1) */
  else {
    /* For now we will return invalid, for the future we may want a shell here */
    rc = ECMD_INT_UNKNOWN_COMMAND;

  }

  return rc;
}

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
