// Copyright ***********************************************************
//                                                                      
// File ecmdInterpreter.C                                   
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
#define ecmdInterpreter_C
#include <inttypes.h>

#include <ecmdClientCapi.H>
#include "ecmdInterpreter.H"
#include "ecmdIntReturnCodes.H"

#undef ecmdInterpreter_C
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


int ecmdCommandInterpreter(int argc, char* argv[]) {

  int rc = ECMD_SUCCESS;

  if (argc >= 1) {

    switch (argv[0][0]) {

        /************************/
        /* The G's              */
        /************************/
      case 'g':

        if (!strcmp(argv[0], "getbits")) {
          rc = ecmdGetBitsUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "getring")) {
          rc = ecmdGetRingDumpUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "getscom")) {
          rc = ecmdGetScomUser(argc - 1, argv + 1);
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;

        }
        break;




        /************************/
        /* The P's              */
        /************************/
      case 'p':

        if (!strcmp(argv[0], "putbits")) {
          rc = ecmdPutBitsUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "pollscom")) {
          rc = ecmdPollScomUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "putscom")) {
          rc = ecmdPutScomUser(argc - 1, argv + 1);
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;


        /************************/
        /* The Unknown          */
        /************************/

      default:
        /* We don't understand this function, let's let the caller know */
        rc = ECMD_INT_UNKNOWN_COMMAND;
        break;
    }


    /* Let's process the errors returned by the command */
    if (rc && (rc != ECMD_INT_UNKNOWN_COMMAND)) {
      /* Do something here */
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
