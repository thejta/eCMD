
// Copyright **********************************************************
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
// End Copyright ******************************************************

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <ecmdClientCapi.H>
#include <ecmdDataBuffer.H>
#include <ecmdReturnCodes.H>
#include <ecmdUtils.H>
#include <cipClientPerlapi.H>
#include <cipClientCapi.H>
#include <ecmdSharedUtils.H>


//extern int myErrorCode = ECMD_SUCCESS;
//extern int safeMode = 1;

cipClientPerlapi::cipClientPerlapi () {
}

cipClientPerlapi::~cipClientPerlapi () {
}


int cipClientPerlapi::cipInitExtension() {
printf("I'm here\n");
int rc = ::cipInitExtension();
printf("RC : %d\n",rc);
printf("RC3 : %d\n",rc);
  return rc;
}
