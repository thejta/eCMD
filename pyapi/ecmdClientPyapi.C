/* $Header$ */
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
#include <ecmdReturnCodes.H>
#include <ecmdUtils.H>
#include <ecmdClientPyapi.H>
#include <ecmdSharedUtils.H>

uint32_t ecmdLoadDll(const char * i_dllName, const char * i_clientVersion) {

  uint32_t rc = ECMD_SUCCESS;
  std::string dllName = "";
  if (i_dllName != NULL) {
    dllName = i_dllName;
  }

  rc = ecmdLoadDll(dllName);


  /* Check our Python Major Version */
  char capiVersion[10];
  strcpy(capiVersion,"ver");
  strcat(capiVersion,ECMD_CAPI_VERSION);
  int majorlength = (int)(strchr(capiVersion, '.') - capiVersion);
  /* Strip off the minor version */
  capiVersion[majorlength] = '\0';

  if (!strstr(i_clientVersion,capiVersion)) {
    fprintf(stderr,"**** FATAL : eCMD Python Module and your client major version numbers don't match, they are not compatible\n");
    fprintf(stderr,"**** FATAL : Client Version(s) : %s   : Python Module Version : %s\n",i_clientVersion, ECMD_CAPI_VERSION);

    fprintf(stderr,"(ecmdLoadDll) :: Python Module version mismatch - execution halted\n");
    exit(1);
  }

  return rc;
}

uint32_t ecmdCommandArgs(char** io_argv) {
  uint32_t rc = 0;
  int looper = 0;

  while(io_argv[looper] != NULL) looper++;

  rc = ecmdCommandArgs(&looper, &io_argv);

  return rc;
}

bool ecmdParseOption(char ** io_argv, const char * i_option) {

  int looper = 0;

  while(io_argv[looper] != NULL) looper++;

  return ::ecmdParseOption(&looper, &io_argv,i_option);
}

char * ecmdParseOptionWithArgs(char ** io_argv, const char * i_option) {

  char * ret = NULL;
  int looper = 0;

  while(io_argv[looper] != NULL) looper++;

  ret = ::ecmdParseOptionWithArgs(&looper, &io_argv,i_option);

  return ret;
}
