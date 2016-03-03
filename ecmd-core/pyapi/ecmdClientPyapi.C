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
