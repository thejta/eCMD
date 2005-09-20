
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

// Include the Swig Perl headers for Croak
#include "EXTERN.h"    
#include "perl.h"
#include "XSUB.h"


#include <ecmdClientCapi.H>
#include <ecmdDataBuffer.H>
#include <ecmdReturnCodes.H>
#include <ecmdUtils.H>
#include <ecmdClientPerlapi.H>
#include <cmdClientPerlapi.H>
#include <cmdClientCapi.H>
#include <ecmdSharedUtils.H>


int CMDPERLAPI::cmdInitExtension(const char * i_clientVersion) {
  /* Check our Perl Major Version */
  char capiVersion[10];
  strcpy(capiVersion,"ver");
  strcat(capiVersion,ECMD_CMD_CAPI_VERSION);
  int majorlength = (int)(strchr(capiVersion, '.') - capiVersion);
  /* Strip off the minor version */
  capiVersion[majorlength] = '\0';

  if (!strstr(i_clientVersion,capiVersion)) {
    fprintf(stderr,"**** FATAL : eCMD Perl Module and your client major version numbers don't match, they are not compatible\n");
    fprintf(stderr,"**** FATAL : Client Version(s) : %s   : Perl Module Version : %s\n",i_clientVersion, ECMD_CMD_CAPI_VERSION);

    croak("(zseInitExtension) :: Perl Module version mismatch - execution halted\n");
  }
  return ::cmdInitExtension();
}

