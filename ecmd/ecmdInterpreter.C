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
/* $Header$ */

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
#include <ecmdInterpreter.H>
#include <ecmdReturnCodes.H>
#include <ecmdCommandUtils.H>
#include <ecmdUtils.H>
#include <ecmdSharedUtils.H>

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


uint32_t ecmdCommandInterpreter(int argc, char* argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  if (argc >= 1) {

    /* Let's handle the '-h' arg right here */
    if (ecmdParseOption(&argc, &argv, "-h")) {
      ecmdPrintHelp(argv[0]);
      return rc;
    }

    switch (argv[0][0]) {

        /************************/
        /* The C's              */
        /************************/
      case 'c':

        if (!strcmp(argv[0], "checkrings")) {
          rc = ecmdCheckRingsUser(argc - 1, argv + 1);
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;

        /************************/
        /* The E's              */
        /************************/
      case 'e':

        if (!strcmp(argv[0], "ecmdquery")) {
          rc = ecmdQueryUser(argc - 1, argv + 1);
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;

        /************************/
        /* The G's              */
        /************************/
      case 'g':

        if (!strcmp(argv[0], "getarray")) {
          rc = ecmdGetArrayUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "getbits")) {
          rc = ecmdGetBitsUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "getfpr")) {
          rc = ecmdGetGprFprUser(argc - 1, argv + 1, ECMD_FPR);
        } else if (!strcmp(argv[0], "getgpr")) {
          rc = ecmdGetGprFprUser(argc - 1, argv + 1, ECMD_GPR);
        } else if (!strcmp(argv[0], "getlatch")) {
          rc = ecmdGetLatchUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "getmemdma")) {
          rc = ecmdGetMemUser(argc - 1, argv + 1, ECMD_MEM_DMA);
        } else if (!strcmp(argv[0], "getmemmemctrl")) {
          rc = ecmdGetMemUser(argc - 1, argv + 1, ECMD_MEM_MEMCTRL);
        } else if (!strcmp(argv[0], "getmemproc")) {
          rc = ecmdGetMemUser(argc - 1, argv + 1, ECMD_MEM_PROC);
        } else if (!strcmp(argv[0], "getringdump")) {
          rc = ecmdGetRingDumpUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "getscom")) {
          rc = ecmdGetScomUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "getspr")) {
          rc = ecmdGetSprUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "getspy")) {
          rc = ecmdGetSpyUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "gettracearray")) {
          rc = ecmdGetTraceArrayUser(argc - 1, argv + 1);
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;

        }
        break;



        /************************/
        /* The I's              */
        /************************/
      case 'i':

        if (!strcmp(argv[0], "istep")) {
          rc = ecmdIstepUser(argc - 1, argv + 1);
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;


        /************************/
        /* The P's              */
        /************************/
      case 'p':

        if (!strcmp(argv[0], "putarray")) {
          rc = ecmdPutArrayUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "putbits")) {
          rc = ecmdPutBitsUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "putfpr")) {
          rc = ecmdPutGprFprUser(argc - 1, argv + 1, ECMD_FPR);
        } else if (!strcmp(argv[0], "putgpr")) {
          rc = ecmdPutGprFprUser(argc - 1, argv + 1, ECMD_GPR);
        } else if (!strcmp(argv[0], "pollscom")) {
          rc = ecmdPollScomUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "putlatch")) {
          rc = ecmdPutLatchUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "putmemdma")) {
          rc = ecmdPutMemUser(argc - 1, argv + 1, ECMD_MEM_DMA);
        } else if (!strcmp(argv[0], "putmemmemctrl")) {
          rc = ecmdPutMemUser(argc - 1, argv + 1, ECMD_MEM_MEMCTRL);
        } else if (!strcmp(argv[0], "putmemproc")) {
          rc = ecmdPutMemUser(argc - 1, argv + 1, ECMD_MEM_PROC);
        } else if (!strcmp(argv[0], "putpattern")) {
          rc = ecmdPutPatternUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "putscom")) {
          rc = ecmdPutScomUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "putspr")) {
          rc = ecmdPutSprUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "putspy")) {
          rc = ecmdPutSpyUser(argc - 1, argv + 1);
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;


      case 's':

        if (!strcmp(argv[0], "sendcmd")) {
          rc = ecmdSendCmdUser(argc - 1, argv + 1);

#ifndef REMOVE_SIM
        } else if (!strcmp(argv[0], "simaet")) {
          rc = ecmdSimaetUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simcheckpoint")) {
          rc = ecmdSimcheckpointUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simclock")) {
          rc = ecmdSimclockUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simecho")) {
          rc = ecmdSimechoUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simexit")) {
          rc = ecmdSimexitUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simEXPECTFAC")) {
          rc = ecmdSimEXPECTFACUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simexpecttcfac")) {
          rc = ecmdSimexpecttcfacUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simGETFAC")) {
          rc = ecmdSimGETFACUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simGETFACX")) {
          rc = ecmdSimGETFACXUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simgettcfac")) {
          rc = ecmdSimgettcfacUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simgetcurrentcycle")) {
          rc = ecmdSimgetcurrentcycleUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "siminit")) {
          rc = ecmdSiminitUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simPUTFAC")) {
          rc = ecmdSimPUTFACUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simPUTFACX")) {
          rc = ecmdSimPUTFACXUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simputtcfac")) {
          rc = ecmdSimputtcfacUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simrestart")) {
          rc = ecmdSimrestartUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simSTKFAC")) {
          rc = ecmdSimSTKFACUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simstktcfac")) {
          rc = ecmdSimstktcfacUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simSUBCMD")) {
          rc = ecmdSimSUBCMDUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simtckinterval")) {
          rc = ecmdSimTckIntervalUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simUNSTICK")) {
          rc = ecmdSimUNSTICKUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simunsticktcfac")) {
          rc = ecmdSimunsticktcfacUser(argc - 1, argv + 1);
	} else if (!strcmp(argv[0], "simgethierarchy")) {
          rc = ecmdSimGetHierarchyUser(argc - 1, argv + 1);
#endif
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
