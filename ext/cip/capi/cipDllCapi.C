// Copyright ***********************************************************
//                                                                      
// File cipDllCapi.C                                   
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
// Description: CIP Extension common plugin code
//
// End Module Description **********************************************

/* $Header$ */

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <stdio.h>
#include <string>
#include <cipDllCapi.H>

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
#ifndef ECMD_STRIP_DEBUG
/* This is en ecmdDllCapi.C */
extern uint32_t ecmdGlobal_DllDebug;
#endif

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------

uint32_t dllCipInitExtension (const char * i_clientVersion) {

  /* First off let's check our version */
  std::string ext = "CIP";
  std::string DllVersion = ECMD_CIP_CAPI_VERSION;

  /* Let's found our '.' char because we only fail if the Major number changes */
  int majorlength = (int)(strchr(i_clientVersion, '.') - i_clientVersion);

  if (strncmp(i_clientVersion,DllVersion.c_str(),majorlength)) {
    fprintf(stderr,"**** FATAL : eCMD %s Extension and your client Major version numbers don't match, they are not compatible\n", ext.c_str());
    fprintf(stderr,"**** FATAL : Client Version : %s   : Extension Version : %s\n",i_clientVersion, DllVersion.c_str());

    if (atoi(i_clientVersion) < atoi(DllVersion.c_str())) {
      fprintf(stderr,"**** FATAL : Your client is older then the eCMD %s Extension Plugin you are running\n", ext.c_str());
      fprintf(stderr,"**** FATAL : You must grab the latest client libraries and rebuild your client to continue\n");
      fprintf(stderr,"**** FATAL : Information on where to obtain these files is at http://rhea.rchland.ibm.com/eCMD/\n");
    } else {
      fprintf(stderr,"**** FATAL : It appears your client is newer then the eCMD %s Extension Plugin you are running\n", ext.c_str());
      fprintf(stderr,"**** FATAL : Contact the eCMD team to have the Plugin rebuilt to match your client\n");
      fprintf(stderr,"**** FATAL : Or get ahold of down level client libraries and rebuild your client to match\n");
      fprintf(stderr,"**** FATAL : Contact information can be found at http://rhea.rchland.ibm.com/eCMD/\n");

    }

    exit(999);
  }


#ifndef ECMD_STRIP_DEBUG
  if (ecmdGlobal_DllDebug > 0) {
    printf("ECMD DEBUG : %s Client Version '%s'\n", ext.c_str(), i_clientVersion);
    printf("ECMD DEBUG : %s Plugin Version '%s'\n", ext.c_str(), DllVersion.c_str());
  }
#endif

  return 0;
}
