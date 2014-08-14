/* NOTE : This file has been generated from the eCMD extension temp-late */
/* DO NOT EDIT THISFILE (unless you are editing the temp-late */






// Copyright ***********************************************************
//                                                                      
// File templateClientPyapi.C                                   
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
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <ecmdDataBuffer.H>
#include <ecmdReturnCodes.H>
#include <ecmdUtils.H>
#include <ecmdSharedUtils.H>
#include <ecmdClientCapi.H>
#include <ecmdClientPyapi.H>
#include <templateClientCapi.H>
#include <templateClientPyapi.H>

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
int templateInitExtension(const char * i_clientVersion) {
  /* Check our Python Major Version */
  char capiVersion[10];
  strcpy(capiVersion,"ver");
  strcat(capiVersion,ECMD_TEMPLATE_CAPI_VERSION);
  int majorlength = (int)(strchr(capiVersion, '.') - capiVersion);
  /* Strip off the minor version */
  capiVersion[majorlength] = '\0';

  if (!strstr(i_clientVersion,capiVersion)) {
    fprintf(stderr,"**** FATAL : eCMD Python Module and your client major version numbers don't match, they are not compatible\n");
    fprintf(stderr,"**** FATAL : Client Version(s) : %s   : Python Module Version : %s\n",i_clientVersion, ECMD_TEMPLATE_CAPI_VERSION);

    fprintf(stderr,"(templateInitExtension) :: Python Module version mismatch - execution halted\n");
    exit(1);
  }
  return templateInitExtension();
}



// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
