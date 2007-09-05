/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File ecmdJtagUser.C                                  
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 2003                                         
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
#include <stdio.h>

#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <ecmdSharedUtils.H>

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


uint32_t ecmdSendCmdUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  uint32_t coeRc = ECMD_SUCCESS;                                    //@01
  /*  bool expectFlag = false; */
  /*  bool xstateFlag = false; */
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdChipTarget target;                ///< Current target being operated on
  uint32_t instruction;                 ///< Instruction to send to chip
  uint32_t modifier;                    ///< Modifier to send to chip
  ecmdDataBuffer statusBuffer;          ///< Buffer to hold return status data
  bool validPosFound = false;           ///< Did the looper find something to run on?

  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr == NULL) {
    format = "x";
  }
  else {
    format = formatPtr;
  }
  //Check verbose option
  bool verbose = ecmdParseOption(&argc, &argv, "-v");

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

//@01
  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 3) {  //chip + instruction + modifier
    ecmdOutputError("sendcmd - Too few arguments specified; you need at least a chip, instruction, and modifier.\n");
    ecmdOutputError("sendcmd - Type 'sendcmd -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.threadState = target.coreState = ECMD_TARGET_FIELD_UNUSED;

  // we need the instruction and modifier

  if (strlen(argv[1]) > 2) {
    ecmdOutputError("sendcmd - The instruction has to be <= 8 bits\n");
    return ECMD_INVALID_ARGS;
  } else if (!ecmdIsAllHex(argv[1])) {
    ecmdOutputError("sendcmd - Instruction data contained some non-hex characters\n");
    return ECMD_INVALID_ARGS;
  }
  ecmdGenB32FromHexRight(&instruction, argv[1], 32);

  if (strlen(argv[2]) > 6) {
    ecmdOutputError("sendcmd - The modifier has to be <= 24 bits\n");
    return ECMD_INVALID_ARGS;
  } else if (!ecmdIsAllHex(argv[2])) {
    ecmdOutputError("sendcmd - Modifier data contained some non-hex characters\n");
    return ECMD_INVALID_ARGS;
  }
  ecmdGenB32FromHexRight(&modifier, argv[2], 32);


  if (argc > 3) {
    ecmdOutputError("sendcmd - Too many arguments specified; you probably added an option that wasn't recognized.\n");
    ecmdOutputError("sendcmd - Type 'sendcmd -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
  /*  char outstr[30]; */
  std::string printed;

  while (ecmdConfigLooperNext(target, looperdata) && (!coeRc || coeMode)) {    //@01

    rc = sendCmd(target, instruction, modifier, statusBuffer);
    if (rc) {
      printed = "sendcmd - Error occured performing sendcmd on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;                                   //@01                       
      continue;                                     //@01 
    }
    else {
      validPosFound = true;     
    }


    printed = ecmdWriteTarget(target) + "  ";
    std::string dataStr = ecmdWriteDataFormatted(statusBuffer, format);
    printed += dataStr;
    ecmdOutput( printed.c_str() ); 

    if ( verbose ) {

      ecmdChipData chipdata;
      rc = ecmdGetChipData (target, chipdata);

      if ( (!rc) && (chipdata.interfaceType == ECMD_INTERFACE_CFAM) ) {
        printed = "\n\t\tInstruction Status Register\n";
        printed += "\t\t---------------------------\n";
        printed += "\t\t " + statusBuffer.genHexRightStr(0, 1) + " Attention Active" + "\n";
        printed += "\t\t " + statusBuffer.genHexRightStr(1, 1) + " Checkstop" + "\n";
        printed += "\t\t " + statusBuffer.genHexRightStr(2, 1) + " Special Attention" + "\n";
        printed += "\t\t " + statusBuffer.genHexRightStr(3, 1) + " Recoverable Error" + "\n";
        printed += "\t\t " + statusBuffer.genHexRightStr(4, 1) + " SCOM Attention" + "\n";
        printed += "\t\t " + statusBuffer.genHexRightStr(5, 1) + " CRC Miscompare on previous data scan-in" + "\n";
        printed += "\t\t " + statusBuffer.genHexRightStr(6, 1) + " Invalid Instruction" + "\n";
        printed += "\t\t " + statusBuffer.genHexRightStr(7, 1) + " PGOOD Indicator(set to '1' by flush, set to '0' by first JTAG instruction)" + "\n";
        printed += "\t\t " + statusBuffer.genHexLeftStr(8, 4) + " JTAG Instruction count(Incremented following Shift-IR) Bits(8:11). (Hex Left)" + "\n";

        printed += "\t\t " + statusBuffer.genHexRightStr(12, 1) + " Data scan occured after the last instruction" + "\n";
        printed += "\t\t " + statusBuffer.genHexLeftStr(13, 3) + " Reserved Bits(13:15). (Hex Left)" + "\n";

        printed += "\t      " + statusBuffer.genHexLeftStr(16,14) + " Clock States(1 = running) Bits(16:29). (Hex Left)" + "\n";

        printed += "\t\t " + statusBuffer.genHexRightStr(30, 1) + " IEEE defined 0"  + "\n";
        printed += "\t\t " + statusBuffer.genHexRightStr(31, 1) + " IEEE defined 1"  + "\n";
        ecmdOutput(printed.c_str());
      }
      else if (rc) {
        printed = "sendcmd - Error occured performing chipinfo query on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;                                   //@01                       
        continue;                                     //@01
      }

    }
  }

  if (!validPosFound) {
    ecmdOutputError("Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  //begin -@01
  if(coeRc) 
    return coeRc;
  else
    return rc;  
  //end -@01
}
