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
#include <ecmdDataBuffer.H>
#include <ecmdInterpreter.H>

#undef ecmdMain_C

int main (int argc, char *argv[])
{
  int rc = 0;

  rc = ecmdLoadDll("../dllStub/export/ecmdDllStub_x86.so");
  if (!rc) {

    /* We now want to call the command interpreter to handle what the user provided us */
    rc = ecmdCommandInterpreter(argc - 1, argv + 1);


  }


  ecmdUnloadDll();

}




// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
