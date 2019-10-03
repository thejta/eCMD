//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2003,2016 IBM International Business Machines Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
//IBM_PROLOG_END_TAG


//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define cipInterpreter_C
#include <string.h>

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
        /* The B's              */
        /************************/
      case 'b':

        if (!strcmp(argv[0], "cipb")) { // stub to get 'if' at the start
          rc = ECMD_INT_UNKNOWN_COMMAND;
#ifndef CIP_REMOVE_BREAKPOINT_FUNCTIONS
        } else if (!strcmp(argv[0], "cipbreakpoint")) {
          rc = cipBreakpointUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "cipbrkpt")) {
          rc = cipBrkptUser(argc - 1, argv + 1);
#endif // CIP_REMOVE_BREAKPOINT_FUNCTIONS
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;



        /************************/
        /* The G's              */
        /************************/
      case 'g':

        if (!strcmp(argv[0], "cipg")) { // stub to get 'if' at the start
          rc = ECMD_INT_UNKNOWN_COMMAND;
#ifndef CIP_REMOVE_MEMORY_FUNCTIONS
        } else if (!strcmp(argv[0], "cipgetmemproc")) {
          rc = ecmdCipGetMemProcUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "cipgetmemprocvar")) {
          rc = cipGetMemProcVarUser(argc - 1, argv + 1);
#endif // CIP_REMOVE_VR_FUNCTIONS
#ifndef CIP_REMOVE_VR_FUNCTIONS
        } else if (!strcmp(argv[0], "cipgetvr")) {
          rc = cipGetVrUser(argc - 1, argv + 1);
#endif // CIP_REMOVE_VR_FUNCTIONS
#ifndef CIP_REMOVE_VSR_FUNCTIONS
        } else if (!strcmp(argv[0], "cipgetvsr")) {
          rc = cipGetVsrUser(argc - 1, argv + 1);
#endif // CIP_REMOVE_VSR_FUNCTIONS
#ifndef CIP_REMOVE_PMC_VOLTAGE_FUNCTIONS
        } else if (!strcmp(argv[0], "cipgetpmcvoltage")) {
          rc = cipGetPmcVoltageUser(argc - 1, argv + 1);
#endif // CIP_REMOVE_PMC_VOLTAGE_FUNCTIONS
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;



        /************************/
        /* The I's              */
        /************************/
      case 'i':

        if (!strcmp(argv[0], "cipi")) { // stub to get 'if' at the start
          rc = ECMD_INT_UNKNOWN_COMMAND;
#ifndef CIP_REMOVE_INSTRUCTION_FUNCTIONS
        } else if (!strcmp(argv[0], "cipinstruct")) {
          rc = cipInstructUser(argc - 1, argv + 1);
#endif // CIP_REMOVE_INSTRUCTION_FUNCTIONS
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;



        /************************/
        /* The P's              */
        /************************/
      case 'p':

        if (!strcmp(argv[0], "cipp")) { // stub to get 'if' at the start
          rc = ECMD_INT_UNKNOWN_COMMAND;
#ifndef CIP_REMOVE_MEMORY_FUNCTIONS
        } else if (!strcmp(argv[0], "cipputmemproc")) {
          rc = ecmdCipPutMemProcUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "cipputmemprocvar")) {
          rc = cipPutMemProcVarUser(argc - 1, argv + 1);
#endif // CIP_REMOVE_VR_FUNCTIONS
#ifndef CIP_REMOVE_VR_FUNCTIONS
        } else if (!strcmp(argv[0], "cipputvr")) {
          rc = cipPutVrUser(argc - 1, argv + 1);
#endif // CIP_REMOVE_VR_FUNCTIONS
#ifndef CIP_REMOVE_VSR_FUNCTIONS
        } else if (!strcmp(argv[0], "cipputvsr")) {
          rc = cipPutVsrUser(argc - 1, argv + 1);
#endif // CIP_REMOVE_VSR_FUNCTIONS
#ifndef CIP_REMOVE_PORE_FUNCTIONS
        } else if (!strcmp(argv[0], "cipporeputscom")) {
          rc = cipPorePutScomUser(argc - 1, argv + 1);

#ifndef ECMD_REMOVE_SPY_FUNCTIONS
        } else if (!strcmp(argv[0], "cipporeputspy")) {
          rc = cipPorePutSpyUser(argc - 1, argv + 1);
#endif 
        } else if (!strcmp(argv[0], "cipporeputspr")) {
          rc = cipPorePutSprUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "cipporequeryimage")) {
          rc = cipPoreQueryImageUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "cipporeloadimage")) {
          rc = cipPoreLoadImageUser(argc - 1, argv + 1);
#endif // CIP_REMOVE_PORE_FUNCTIONS
#ifndef CIP_REMOVE_PMC_VOLTAGE_FUNCTIONS
        } else if (!strcmp(argv[0], "cipputpmcvoltage")) {
          rc = cipPutPmcVoltageUser(argc - 1, argv + 1);
#endif // CIP_REMOVE_PMC_VOLTAGE_FUNCTIONS
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;


        /************************/
        /* The R's              */
        /************************/
      case 'r':

#ifndef CIP_REMOVE_RW_FUNCTIONS
        if (!strcmp(argv[0], "ciprwreadcache")) {
          rc = cipRWReadCacheUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "ciprwreadtlb")) {
          rc = cipRWReadTLBUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "ciprwreadmem")) {
          rc = cipRWReadMemUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "ciprwwritemem")) {
          rc = cipRWWriteMemUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "ciprwgetdcr")) {
          rc = cipRWGetDcrUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "ciprwputdcr")) {
          rc = cipRWPutDcrUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "ciprwprocstatus")) {
          rc = cipRWProcStatusUser(argc - 1, argv + 1);
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
#else
        rc = ECMD_INT_UNKNOWN_COMMAND;
#endif // CIP_REMOVE_RW_FUNCTIONS
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
