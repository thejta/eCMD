/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File ecmdClientCapi.C                                   
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

// Get rid of lint errors that will just happen
/*lint -e611 Get rid of suspicious cast warning */

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <dlfcn.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#ifndef ECMD_STATIC_FUNCTIONS
# include <ecmdClientEnums.H>
#else
# include <ecmdDllCapi.H>
#endif

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

#ifndef ECMD_STATIC_FUNCTIONS
/* These are from ecmdClientCapiFunc.C */
extern void * dlHandle;
extern void * DllFnTable[];
#endif

#ifndef ECMD_STRIP_DEBUG
/* @brief This is used to output Debug messages on the Client side */
uint32_t ecmdClientDebug = 0;
int fppCallCount =0;
#endif

/* @brief we are going to store a copy of the args list for load/unload operations */
struct ecmdGlobalArgs {
  int argc;
  char* argv[ECMD_ARG_LIMIT+1];   ///< limit of 20 args, same limit in ecmdMain.C

  // Constructor
  ecmdGlobalArgs() {argc = 0; for (int x = 0; x < ECMD_ARG_LIMIT; x++) argv[x] = NULL; }
  // Destructor, deallocate saved args
  ~ecmdGlobalArgs() { for (int x = 0; x < argc; x ++) delete[] argv[x]; }
};
ecmdGlobalArgs g_args;

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------


uint32_t ecmdLoadDll(std::string i_dllName) {

  uint32_t rc = ECMD_SUCCESS;


#ifndef ECMD_STATIC_FUNCTIONS
  const char* dlError;
  /* Only do this if it hasn't been done already */
  if (dlHandle != NULL) {
    return ECMD_SUCCESS;
  }
#endif

#ifndef ECMD_STRIP_DEBUG
  char* tmpptr = getenv("ECMD_DEBUG");
  if (tmpptr != NULL)
    ecmdClientDebug = (uint32_t)atoi(tmpptr);
  else
    ecmdClientDebug = 0;
#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount=0;
  if (ecmdClientDebug >= 8) {
     fppCallCount++;
     myTcount = fppCallCount;

     printf("ECMD DEBUG (ecmdFPP) : ENTER(%03d) : uint32_t ecmdLoadDll(std::string i_dllName)\n",myTcount);
     if (ecmdClientDebug >= 9) {
       printf("ECMD DEBUG (ecmdFPP) : ENTER(%03d) : \t type : std::string \t varriable name : i_dllName = %s\n",myTcount,i_dllName.c_str());
     }
  }
#endif

#ifndef ECMD_STATIC_FUNCTIONS
#ifdef _AIX
    /* clean up the machine from previous tests */
  system("slibclean");
#endif

    /* --------------------- */
    /* load DLL              */
    /* --------------------- */
  if (i_dllName.size() == 0) {
    /* Let's try to get it from the env var */
    char * tempptr = getenv("ECMD_DLL_FILE");  /* is there a ECMD_DLL_FILE environment variable? */
    if (tempptr != NULL) {
      i_dllName = tempptr;
    } else {
      fprintf(stderr,"ecmdLoadDll: Unable to find DLL to load, you must set ECMD_DLL_FILE\n");
      return ECMD_INVALID_DLL_FILENAME;
    }
  }

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug > 1) 
    printf("loadDll: loading %s ...\n", i_dllName.c_str()); 
#endif
  dlHandle = dlopen(i_dllName.c_str(), RTLD_LAZY | RTLD_GLOBAL);

  if (!dlHandle) {
    if ((dlError = dlerror()) != NULL) {
      printf("ERROR: loadDll: Problems loading '%s' : %s\n", i_dllName.c_str(), dlError);
      return ECMD_DLL_LOAD_FAILURE;
    }
#ifndef ECMD_STRIP_DEBUG
  } else if (ecmdClientDebug > 1) {
    printf("loadDll: load successful\n");
#endif
  }

  /* Clear out the function table */
  for (int func = 0; func < ECMD_NUMFUNCTIONS; func ++) {
    DllFnTable[func] = NULL;
  }

  /* Now we need to call loadDll on the dll itself so it can initialize */

  uint32_t (*Function)(const char *,uint32_t) = 
    (uint32_t(*)(const char *,uint32_t))(void*)dlsym(dlHandle, "dllLoadDll");
  if (!Function) {
    fprintf(stderr,"ecmdLoadDll: Unable to find LoadDll function, must be an invalid DLL\n");
    rc = ECMD_DLL_LOAD_FAILURE;
  } else {
#ifndef ECMD_STRIP_DEBUG
    rc = (*Function)(ECMD_CAPI_VERSION, ecmdClientDebug);
#else
    rc = (*Function)(ECMD_CAPI_VERSION, 0);
#endif
  }

#else

#ifndef ECMD_STRIP_DEBUG
  rc = dllLoadDll(ECMD_CAPI_VERSION, ecmdClientDebug);
#else
  rc = dllLoadDll(ECMD_CAPI_VERSION, 0);
#endif
	    

    
#endif /* ECMD_STATIC_FUNCTIONS */


  /* Ok, is this a recall of ecmdLoadDll and we have some saved away parms, if so, recall */
  if (!rc && g_args.argc != 0) {
    /* ecmdCommandArgs just clears pointers it doesn't free the memory, so make a new pointer list so we don't loose our args */
    char* l_argv[ECMD_ARG_LIMIT+1];
    int l_argc = g_args.argc;

    for (int idx = 0; idx < ECMD_ARG_LIMIT; idx++) {
      if (idx < l_argc) {
	l_argv[idx] = g_args.argv[idx];
      } else {
	l_argv[idx] = NULL;
      }
    }
   
    char** tmp = l_argv;

    rc = ecmdCommandArgs(&l_argc,&tmp);
  }


#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug >= 8) {
    printf("ECMD DEBUG (ecmdFPP) : EXIT (%03d) : uint32_t ecmdLoadDll(std::string i_dllName)\n",myTcount);
    if((ecmdClientDebug == 8) && (rc != ECMD_SUCCESS)) {
      printf("ECMD DEBUG (ecmdFPP) : EXIT (%03d) : \t type : uint32_t : variable name : RETURN CODE = d=%u 0x%.08X\n",myTcount,rc,rc);
    }


    if (ecmdClientDebug >= 9) {
      printf("ECMD DEBUG (ecmdFPP) : EXIT (%03d) : \t type : std::string : varriable name : i_dllName = %s\n",myTcount,i_dllName.c_str());
      printf("ECMD DEBUG (ecmdFPP) : EXIT (%03d) : \t type : uint32_t : variable name : RETURN CODE = d=%u 0x%.08X\n",myTcount,rc,rc);
    }
    printf("ECMD DEBUG (ecmdFPP) : EXIT (%03d) : \t ***************************************\n",myTcount);
  }
#endif     // strip debug

  return rc;
}

uint32_t ecmdUnloadDll() {

  uint32_t rc = ECMD_SUCCESS;
#ifndef ECMD_STATIC_FUNCTIONS
  int c_rc = ECMD_SUCCESS;
#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount=0;
  if (ecmdClientDebug >= 8) {
    fppCallCount++;
    myTcount = fppCallCount;

    printf("ECMD DEBUG (ecmdFPP) : ENTER(%03d) : \t uint32_t ecmdUnloadDll()\n",myTcount);
  }
#endif


#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllUnloadDll();

#else

  if (dlHandle) {
    /* call DLL unload */
    uint32_t (*Function)() =
      (uint32_t(*)())(void*)dlsym(dlHandle, "dllUnloadDll");
    if (!Function) {
      fprintf(stderr,"ecmdUnloadDll: Unable to find UnloadDll function, must be an invalid DLL\n");
      rc = ECMD_DLL_UNLOAD_FAILURE;
      return rc;
    }
    rc = (*Function)();


    /* release DLL */
    const char* dlError;

    c_rc = dlclose(dlHandle);
    if (c_rc) {
      if ((dlError = dlerror()) != NULL) {
        fprintf(stderr,"ERROR: ecmdUnloadDll: %s\n", dlError);
        return ECMD_DLL_UNLOAD_FAILURE;
      }
    }
  }
  dlHandle = NULL;


#endif

  /* Go reset all the extensions so they know we have been unloaded */
  ecmdResetExtensionInitState();

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug >= 8) {
    printf("ECMD DEBUG (ecmdFPP) : EXIT (%03d) : \t uint32_t ecmdUnloadDll()\n",myTcount);
    if((ecmdClientDebug == 8) && (rc !=ECMD_SUCCESS)) {
      printf("ECMD DEBUG (ecmdFPP) : EXIT (%03d) : \t type : RETURN CODE = d=%u 0x%.08X\n",myTcount,rc,rc);
    }
    if (ecmdClientDebug >= 9) {
      printf("ECMD DEBUG (ecmdFPP) : EXIT (%03d) : \t type : RETURN CODE = d=%u 0x%.08X\n",myTcount,rc,rc);
    }
    printf("ECMD DEBUG (ecmdFPP) : EXIT (%03d) : \t ***************************************\n",myTcount);
  }
#endif

  return rc;
}



uint32_t ecmdCommandArgs(int* i_argc, char** i_argv[]) {

  uint32_t rc = ECMD_SUCCESS;


#ifndef ECMD_STRIP_DEBUG
  int myTcount=0;
  if (ecmdClientDebug >= 8) {
    fppCallCount++;
    myTcount = fppCallCount;

    char printbuffer[256];

    sprintf(printbuffer,"ECMD DEBUG (ecmdFPP) : ENTER(%03d) : \t uint32_t ecmdCommandArgs(int* i_argc, char** i_argv[])\n",myTcount);
    ecmdOutput(printbuffer);
    if (ecmdClientDebug >= 9) {
      sprintf(printbuffer,"ECMD DEBUG (ecmdFPP) : ENTER(%03d) : \t type : int* : variable : i_argc = %d\n",myTcount,*i_argc);
      ecmdOutput(printbuffer);
      for (int i=0;i<*i_argc;i++)
      {

         sprintf(printbuffer,"ECMD DEBUG (ecmdFPP) : ENTER(%03d) : \t type : char** : variable : i_argv %u:  . parameter: %s\n",myTcount, i , (*i_argv)[i]);
         ecmdOutput(printbuffer);
      }
    }
  }
#endif


  /* We need to save away what the user provided for the load/unload dll case */

  g_args.argc = *i_argc;

  char* tmp;
  for (int idx = 0; idx < ECMD_ARG_LIMIT; idx++) {
    if (idx < *i_argc) {
      tmp = g_args.argv[idx];
      g_args.argv[idx] = new char[strlen((*i_argv)[idx])+1];
      strcpy(g_args.argv[idx],(*i_argv)[idx]);
      if (tmp != NULL) delete[] tmp;
    } else {
      if (g_args.argv[idx] != NULL) delete[] g_args.argv[idx];
      g_args.argv[idx] = NULL;
    }
  }



#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllCommonCommandArgs(i_argc, i_argv);

#else

  if (dlHandle == NULL) {
    fprintf(stderr,"ecmdCommandArgs: eCMD Function called before DLL has been loaded\n");
    exit(ECMD_DLL_INVALID);
  }

  /* call DLL common command args */
  uint32_t (*Function)(int*, char***) =
    (uint32_t(*)(int*, char***))(void*)dlsym(dlHandle, "dllCommonCommandArgs");
  if (!Function) {
    fprintf(stderr,"ecmdCommandArgs: Unable to find dllCommonCommandArgs function, must be an invalid DLL\n");
    exit(ECMD_DLL_INVALID);
  }
  rc = (*Function)(i_argc, i_argv);
  
#endif

#ifndef ECMD_STRIP_DEBUG

  char printbuffer[256];
  if (ecmdClientDebug >= 8) {
    sprintf(printbuffer,"ECMD DEBUG (ecmdFPP) : EXIT (%03d) : \t uint32_t ecmdCommandArgs(int* i_argc, char** i_argv[])\n",myTcount);
    ecmdOutput(printbuffer);
    if((ecmdClientDebug == 8) && (rc !=ECMD_SUCCESS)) {
      sprintf(printbuffer,"ECMD DEBUG (ecmdFPP) : EXIT (%03d) : \t type : RETURN CODE = d=%u 0x%.08X\n",myTcount,rc,rc);
      ecmdOutput(printbuffer);
    }
    if (ecmdClientDebug >= 9) {
      sprintf(printbuffer, "ECMD DEBUG (ecmdFPP) : EXIT (%03d) : \t type : int* : variable name : i_argc = %d\n",myTcount,*i_argc);
      ecmdOutput(printbuffer);
      sprintf(printbuffer,"ECMD DEBUG (ecmdFPP) : EXIT (%03d) : \t type : char** : variable name : i_argv = **not implemented yet**\n",myTcount);
      ecmdOutput(printbuffer);
      sprintf(printbuffer,"ECMD DEBUG (ecmdFPP) : EXIT (%03d) : \t type : RETURN CODE = d=%u 0x%.08X\n",myTcount,rc,rc);
      ecmdOutput(printbuffer);
    }
    sprintf(printbuffer,"ECMD DEBUG (ecmdFPP) : EXIT (%03d) : \t ***************************************\n",myTcount);
    ecmdOutput(printbuffer);
  }

#endif

  return rc;
}

uint32_t ecmdSetup(char* i_args) {

  char command[200];
  std::string retstr;
  FILE * lFilePtr ;
  unsigned char lChar;

  sprintf(command, "%s/tools/ecmd/%s/bin/ecmdsetup.pl ksh %s", getenv("CTEPATH"), getenv("ECMD_RELEASE"), i_args);
  lFilePtr = popen(command, "r") ;
  // read the output from the command
  while (255 != (lChar = getc(lFilePtr)))
  {
    retstr += lChar;
  }
  pclose(lFilePtr);

  /* Print what we got back for debug */
  //printf("output: %s\n", retstr.c_str());

  std::vector<std::string> tokens;
  /* Can't call ecmdParseTokens from here, so do it manually */
  size_t curStart = 0, curEnd = 0;

  while (1) {
    curStart = retstr.find_first_not_of(";", curEnd);
    if (curStart == std::string::npos) break;
    curEnd = retstr.find_first_of(";",curStart);
    tokens.push_back(retstr.substr(curStart, curEnd-curStart));
  }
  /* End ecmdParseTokens yank */

  int linePos;
  for (uint32_t idx = 0; idx < tokens.size(); idx++) {
    /* Take an export command to the shell and turn it into a setenv command */
    if (tokens[idx].substr(0, 6) == "export") {
      linePos = tokens[idx].find("=");
      std::string envVariable = tokens[idx].substr(7,(linePos - 7));
      /* linePos + 2 gets past the =" in the output.  length() - (linePos + 3) pulls the quote off the end */
      std::string envData = tokens[idx].substr(linePos+2, (tokens[idx].length() - (linePos+3)));
      setenv(envVariable.c_str(), envData.c_str(), 1 /* Overwrite */);
    }

    /* If they are trying to echo it to the shell, print it here */
    else if (tokens[idx].substr(0, 4) == "echo") {
      printf("%s\n", tokens[idx].substr(5, tokens[idx].length()).c_str());
    }

    /* If they are trying to unset a variable, do it here */
    /* Beautiful, aix doesn't support it - shouldn't be a show stopper, so pulling.  JTA 07/26/06 */
#ifndef _AIX
    else if (tokens[idx].substr(0, 5) == "unset") {
      unsetenv(tokens[idx].substr(6, tokens[idx].length()).c_str());
    }
#endif
  }

  return 0;
}


uint32_t ecmdDllFunctionTimer(timeval &i_startTv, timeval &i_endTv) {
  uint32_t msTime;

  msTime = (i_endTv.tv_sec - i_startTv.tv_sec) * 1000;
  msTime += ((i_endTv.tv_usec - i_startTv.tv_usec) / 1000);

  return msTime;
};

/* ------------------------------------------------------------------------------------ */
/* Below are the functions that pass straigh through to the Dll                         */
/* Some have been moved here from ecmdClientCapiFunc.C to add additional debug messages */
/* ------------------------------------------------------------------------------------ */

/* enableRingCache and disableRingCache removed after v8.0 because they can now be generate - JTA 10/17/06 */
