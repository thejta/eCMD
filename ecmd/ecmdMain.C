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
#define ecmdMain_C

#include <ecmdClientCapi.H>
#include <ecmdInterpreter.H>
#include <ecmdReturnCodes.H>

#include <stdio.h>
#include <string>

#undef ecmdMain_C

int main (int argc, char *argv[])
{
  int rc = 0;

  std::string cmdsave;
  for (int i = 0; i < argc; i++) {
    cmdsave += argv[i];
    cmdsave += " ";
  }

  cmdsave += "\n";
  if (getenv ("ECMD_DLL_FILE") == NULL) {
#ifdef _AIX
    rc = ecmdLoadDll("../dllStub/export/ecmdDllStub_aix.so");
#else
    rc = ecmdLoadDll("../dllStub/export/ecmdDllStub_x86.so");
#endif
  } else {
    /* Load the one specified by ECMD_DLL_FILE */
    rc = ecmdLoadDll("");
  }

  if (!rc) {

    /* We now want to call the command interpreter to handle what the user provided us */
    rc = ecmdCommandInterpreter(argc - 1, argv + 1);


    if (rc == ECMD_INT_UNKNOWN_COMMAND) {
      char buf[100];
      sprintf(buf,"**** ERROR (eCMD::main) : Unknown Command specified\n");
      ecmdOutputError(buf);
    }


  }

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
