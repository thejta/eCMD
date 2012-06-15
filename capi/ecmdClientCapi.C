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

// Get rid of lint errors that will just happen
/*lint -e611 Get rid of suspicious cast warning */

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <dlfcn.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fstream>
#include <netinet/in.h>
#include <ecmdSharedUtils.H>
#include <algorithm>

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
int fppCallCount = 0;
bool ecmdDebugOutput = false;
#endif

/* @brief we are going to store a copy of the args list for load/unload operations */
struct ecmdGlobalArgs {
  int argc;
  char* argv[ECMD_ARG_LIMIT+1];   ///< limit of 20 args, same limit in ecmdMain.C

  // Constructor
  ecmdGlobalArgs() {argc = 0; for (int x = 0; x < ECMD_ARG_LIMIT; x++) argv[x] = NULL; }
  // Destructor, deallocate saved args
  ~ecmdGlobalArgs() { for (int x = 0; x < argc; x ++) {delete[] argv[x];argv[x]=NULL;} }
};
ecmdGlobalArgs g_args;

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------


uint32_t ecmdLoadDll(std::string i_dllName) {

  uint32_t rc = ECMD_SUCCESS;
  std::string loadDllName = i_dllName;

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
  int myTcount = 0;
  std::vector< void * > args;
  args.push_back((void*) &loadDllName);
  if (ecmdClientDebug != 0) {
     fppCallCount++;
     myTcount = fppCallCount;

     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t ecmdLoadDll(std::string i_dllName)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_LOADDLL,"ecmdLoadDll");
     ecmdDebugOutput = true; /* Dll is being loaded now, can use ecmdOutput */
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
  if (loadDllName.size() == 0) {
    /* Let's try to get it from the env var */
    char * tempptr = getenv("ECMD_DLL_FILE");  /* is there a ECMD_DLL_FILE environment variable? */
    if (tempptr != NULL) {
      loadDllName = tempptr;
    } else {
      fprintf(stderr,"ecmdLoadDll: Unable to find DLL to load, you must set ECMD_DLL_FILE\n");
      return ECMD_INVALID_DLL_FILENAME;
    }
  }

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) 
    printf("loadDll: loading %s ...\n", loadDllName.c_str()); 
#endif
  dlHandle = dlopen(loadDllName.c_str(), RTLD_LAZY | RTLD_GLOBAL);

  if (!dlHandle) {
    if ((dlError = dlerror()) != NULL) {
      printf("ERROR: loadDll: Problems loading '%s' : %s\n", loadDllName.c_str(), dlError);
      return ECMD_DLL_LOAD_FAILURE;
    }
#ifndef ECMD_STRIP_DEBUG
  } else if (ecmdClientDebug != 0) {
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
  if (ecmdClientDebug != 0) {
    args.push_back((void*) &rc);
    ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t ecmdLoadDll(std::string i_dllName)",args);
  }
#endif     // strip debug

  if (rc) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(rc, false, false, false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;
}

uint32_t ecmdUnloadDll() {

  uint32_t rc = ECMD_SUCCESS;
#ifndef ECMD_STATIC_FUNCTIONS
  int c_rc = ECMD_SUCCESS;
#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount=0;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
    fppCallCount++;
    myTcount = fppCallCount;
    ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t ecmdUnloadDll()",args);
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
  if (ecmdClientDebug != 0) {
    args.push_back((void*) &rc);
    ecmdDebugOutput = false;  /* Dll is unloaded, can't use ecmdOutput */
    ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t ecmdUnloadDll()",args);
    ecmdFunctionTimer(myTcount,ECMD_TMR_UNLOADDLL,"ecmdUnloadDll");
  }
#endif

  if (rc) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(rc, false, false, false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

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
      for (int i=0;i<*i_argc;i++) {
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
      sprintf(printbuffer, "ECMD DEBUG (ecmdFPP) : EXIT (%03d) : \t type : int* : variable : i_argc = %d\n",myTcount,*i_argc);
      ecmdOutput(printbuffer);
      for (int i=0;i<*i_argc;i++) {
        sprintf(printbuffer,"ECMD DEBUG (ecmdFPP) : EXIT (%03d) : \t type : char** : variable : i_argv %u:  . parameter: %s\n",myTcount, i , (*i_argv)[i]);
        ecmdOutput(printbuffer);
      }
      sprintf(printbuffer,"ECMD DEBUG (ecmdFPP) : EXIT (%03d) : \t type : RETURN CODE = d=%u 0x%.08X\n",myTcount,rc,rc);
      ecmdOutput(printbuffer);
    }
    sprintf(printbuffer,"ECMD DEBUG (ecmdFPP) : EXIT (%03d) : \t ***************************************\n",myTcount);
    ecmdOutput(printbuffer);
  }

#endif

  if (rc) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(rc, false, false, false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;
}

uint32_t ecmdSetup(const char* i_args) {
  uint32_t rc = 0;

  char command[200];
  std::string retstr;

  sprintf(command, "%s/tools/ecmd/%s/bin/ecmdsetup.pl ksh %s", getenv("CTEPATH"), getenv("ECMD_RELEASE"), i_args);

  bool finished = false;
  for (int trial = 1; trial <= 10; ++trial)
  {
    int charsBeforeError = -1;

    FILE* lFilePtr = popen(command, "r");

    // Handle a popen error
    if (ferror(lFilePtr))
    {
      //printf("ecmdClientCapi.C: popen call produced error %d: %s\n", errno, sys_errlist[errno]);
      clearerr(lFilePtr);
    }
    // Make sure we have a valid file pointer
    else if (lFilePtr)
    {
      // read the output from the command
      while (true)
      {
        int nextChar = getc(lFilePtr);

        // Check if there was an error.  Some of them we can handle.
        if (ferror(lFilePtr))
        {
          //printf("ecmdClientCapi.C: getc call after getting %d characters produced error %d: %s\n", retstr.length(), errno, sys_errlist[errno]);

          // If we're not looping on the same character then we may be able to continue the getc loop
          if (charsBeforeError < (int)retstr.length())
          {
            // We made some progress since the last error.
            charsBeforeError = retstr.length();

            // If we got an EINTR (interrupted system call) error then clear it and try the getc again
            if (errno == EINTR)
            {
              clearerr(lFilePtr);
              continue;
            }
          }

          // All other errors we can't handle so break out of the loop
          clearerr(lFilePtr);
          break;
        }

        // If we truly hit the end of file then we're done
        if (feof(lFilePtr))
        {
          finished = true;
          break;
        }

        // Else add this character to the string
        unsigned char lChar = nextChar;
        retstr += lChar;
      }  // while (TRUE)

    }  // if (lFilePtr)

    pclose(lFilePtr);

    if (finished)
    {
      //if (charsBeforeError != -1)
      //    printf("ecmdClientCapi.C: recovered from getc errors\n");
      //if (trial > 1)
      //    printf("ecmdClientCapi.C: popen trial %d succeeded\n", trial);
      // We got everything we need, so get out of here
      break;
    }
    else
    {
      // Reset the string because we are starting over
      retstr.clear();
    }
  }  // for (int trial = 1; trial <= 10; ++trial)

  if (!finished) {
    ecmdOutputError("ecmdSetup - execution of the ecmdsetup script failed!\n");
    ecmdOutputError("ecmdSetup - none of your environment variables have been updated!\n");
    return ECMD_FAILURE;
  }

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

    /* If a return value was provided, push that back as an rc */
    else if (tokens[idx].substr(0, 6) == "return") {
      sscanf(tokens[idx].substr(7, tokens[idx].length()).c_str(),"%d",&rc);
      return rc;
    }
  }

  return rc;
}

/* This function has to be here because the ecmdClientCapiFunc.C code generates a call to */
/* If it was autogenerated, it would keep calling itself - and that is bad */
uint32_t ecmdGetGlobalVar(ecmdGlobalVarType_t i_type) {

  uint32_t rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllGetGlobalVar%s",": eCMD Function called before DLL has been loaded\n");
    exit(ECMD_DLL_INVALID);
  }
#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_type);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t ecmdGetGlobalVar(ecmdGlobalVarType_t i_type)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"ecmdGetGlobalVar");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllGetGlobalVar(i_type);
#else
  if (DllFnTable[ECMD_GETGLOBALVAR] == NULL) {
     DllFnTable[ECMD_GETGLOBALVAR] = (void*)dlsym(dlHandle, "dllGetGlobalVar");
     if (DllFnTable[ECMD_GETGLOBALVAR] == NULL) {
       fprintf(stderr,"dllGetGlobalVar%s",": Unable to find function, must be an invalid DLL - program aborting\n"); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)(ecmdGlobalVarType_t) = 
      (uint32_t(*)(ecmdGlobalVarType_t))DllFnTable[ECMD_GETGLOBALVAR];
  rc =    (*Function)(i_type);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"ecmdGetGlobalVar");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t ecmdGetGlobalVar(ecmdGlobalVarType_t i_type)",args);
   }
#endif

  return rc;
}


//this function will read the hash file and find the groupName passed in and then read the groupscomdef and gather all the entries and scomdata, and return them
// if a version is passed in it will look for that version of the files
uint32_t ecmdQueryScomGroup(const ecmdChipTarget i_target, const std::string i_scomGroupName, ecmdScomData &o_queryData, std::list<ecmdScomEntry> &o_entries, std::string & io_scomGroupFileVersion) {
  uint32_t rc = 0;
  std::string upper_scomGroupName = i_scomGroupName;

  uint32_t groupNameKey;
  groupHashEntry groupNameLine;  //use this for the few 32bit 32bit lines to seperate the unique, total, addrs, last line groupings
  //addrHashEntry addrLine;  //use this for the few 32bit 32bit lines to seperate the unique, total, addrs, last line groupings
  sectionHashEntry sectionLine;  //use this for the few 32bit 32bit lines to seperate the unique, total, addrs, last line groupings

  int groupNameSize = sizeof(struct groupHashEntry); /* We need this to be able to traverse the binary hash file */
  //int adderSize = sizeof(struct addrHashEntry); /* We need this to be able to traverse the binary hash file */
  int group_section_start = 0;
  int total_num_groups = 0;

// we have the chip target and version number get the groupscomdef and the hash
  std::string scomgroup_filename;
  std::string scomgroupHash_filename;
  ecmdChipTarget target = i_target;
  std::string str_version = io_scomGroupFileVersion;
  rc = ecmdQueryFileLocationHidden(target, ECMD_FILE_GROUPSCOM, scomgroup_filename, str_version); if (rc) return rc;
  //if str_version was default, then it should have been changed here but the groupscomdef and hash should have the exact same number
  rc = ecmdQueryFileLocationHidden(target, ECMD_FILE_GROUPSCOMHASH, scomgroupHash_filename, str_version); if (rc) return rc;

  if ( io_scomGroupFileVersion == "default") {
    io_scomGroupFileVersion = str_version;
  }
  std::ifstream scomgroupHashFile;
  scomgroupHashFile.open(scomgroupHash_filename.c_str());
  if (scomgroupHashFile.fail()) {
    char buf[200];
    sprintf(buf,"ecmdQueryScomGroup - Unable to open groupscom.hash file: %s\n", scomgroupHash_filename.c_str());
    ecmdOutputError(buf);
    return ECMD_UNKNOWN_FILE;
  }

  //get the start section, and number of groupName entries in that section
  scomgroupHashFile.read((char*)&(sectionLine), sizeof(sectionLine));
  sectionLine.key = htonl(sectionLine.key);

  group_section_start = scomgroupHashFile.tellg();
  total_num_groups = sectionLine.key;

  //get hash of the groupName
  transform(upper_scomGroupName.begin(), upper_scomGroupName.end(), upper_scomGroupName.begin(), (int(*)(int)) toupper);

  groupNameKey = ecmdHashString32(upper_scomGroupName.c_str(), 0);


  //binary search for the groupNameKey
  int low = 0;
  int high = total_num_groups - 1;
  int cur;
  int num_loops = 0;

  while (low < high) {
    cur = (high + low) / 2;
    scomgroupHashFile.seekg( group_section_start + (cur * groupNameSize) ); /* position into file */ 
    scomgroupHashFile.read((char*)&(groupNameLine), sizeof(groupNameLine));

    /* We need to byte swap this guy */
    groupNameLine.key = htonl(groupNameLine.key);

    if (groupNameKey > groupNameLine.key) {
      low = cur + 1;
    } else {
      high = cur;
    }
    num_loops++;
  }// end while loop
  cur = low;

  //cur has the data we want, now go get it
  scomgroupHashFile.seekg( group_section_start + (cur * groupNameSize) ); /* position into file */ 
  scomgroupHashFile.read((char*)&(groupNameLine), sizeof(groupNameLine));
  /* We need to byte swap this guy */
  groupNameLine.key = htonl(groupNameLine.key);
  groupNameLine.filepos = htonl(groupNameLine.filepos);

  //we can close the hash file
  scomgroupHashFile.close();

  //make sure we have the right key
  if (groupNameKey != groupNameLine.key) {
    char buf[200];
    sprintf(buf,"ecmdQueryScomGroup - Did not find the groupName: %s in the hash file\n", i_scomGroupName.c_str());
    ecmdOutputError(buf);
    return ECMD_INVALID_ARGS;
  }

  //printf("filepos = 0x%X\n", groupNameLine.filepos);
  //call the parsing of groupscomdef, pass in the filepos, get back a std::list ecmdScomEntry
  std::list<scomGroupRecord_t> scomGroupRecord;
  rc = parse_groupscomdef_file(scomgroup_filename, scomGroupRecord, true, groupNameLine.filepos ); if (rc) return rc; //read only the group at filepos

  // now xform the scomGroupRecord into the std::list ecmdScomEntry
  ecmdScomEntry entry;
  std::list<scomGroupRecord_t>::iterator scomGroupIter;

  //print all the data we read to the screen
  //printf("asked for a specific group, total groups found:%d\n", scomGroupRecord.size() );
  for (scomGroupIter = scomGroupRecord.begin(); scomGroupIter != scomGroupRecord.end(); scomGroupIter++) {
    //printf("GroupName = %s, line: %d filepos %X, ChipUnit = %s \n", scomGroupIter->scomGroup_name.c_str(), scomGroupIter->lineNumofName, scomGroupIter->filepos, scomGroupIter->scomGroup_chipUnit.c_str() );
    if (scomGroupIter->scomGroup_chipUnit == "NONE") {
      o_queryData.isChipUnitRelated = false;
    } else {
      o_queryData.isChipUnitRelated = true;
      o_queryData.relatedChipUnit = scomGroupIter->scomGroup_chipUnit;
      o_queryData.relatedChipUnitShort = scomGroupIter->scomGroup_chipUnit;
    }
    for (uint32_t i = 0; i < scomGroupIter->scomGroup_listOfAddrs.size(); i++) {
      //printf("addr = 0x%016llX index in group:%X\n", scomGroupIter->scomGroup_listOfAddrs[i], scomGroupIter->scomGroup_indexOfAddr[i]);

      //create the list of scoms
      entry.operation = ECMD_GETSCOM_OP;
      entry.address = scomGroupIter->scomGroup_listOfAddrs[i];
      o_entries.push_back(entry);
    }
  }


  return rc;
}

// does the queryScomgroup and the doScomMultiple all in one
uint32_t getScomGroup(const ecmdChipTarget i_target, const std::string i_scomGroupName, std::list<ecmdScomEntry> & io_groupScomEntries, std::string & io_scomGroupFileVersion) {
  uint32_t rc = ECMD_SUCCESS;
  std::string scomGroupName;
  ecmdScomData queryData;
  std::string printed;
  ecmdChipTarget target = i_target;


  rc = ecmdQueryScomGroup(target, i_scomGroupName, queryData, io_groupScomEntries, io_scomGroupFileVersion);
  if (rc) {
    printed = "getScomGroup - Error occurred performing queryscom on ";
    printed += ecmdWriteTarget(target) + "\n";
    ecmdOutputError( printed.c_str() );
    return ECMD_FAILURE;
  }

  if (io_groupScomEntries.size() == 0) {
    printed = "getScomGroup - Did not find any scoms for this scomGroupName:";
    printed += i_scomGroupName;
    printed += "\n";
    ecmdOutputError(printed.c_str());
    return ECMD_INVALID_ARGS;
  }

  if (queryData.isChipUnitRelated) {
    if (!queryData.isChipUnitMatch(target.chipUnitType)) {
      printed = "getScomGroup - Provided chipUnit \"";
      printed += target.chipUnitType;
      printed += "\" doesn't match chipUnit returned by queryScomgroup \"";
      printed += queryData.relatedChipUnit + "\"\n";
      ecmdOutputError(printed.c_str());
      return ECMD_INVALID_ARGS;
    }
  } else { // !scomData->isChipUnitRelated
    if ( (target.chipUnitType != "") && (target.chipUnitTypeState !=ECMD_TARGET_FIELD_UNUSED)) {
      printed = "getScomGroup - A chipUnit \"";
      printed += target.chipUnitType;
      printed += "\" was given on a non chipUnit scom address.\n";
      ecmdOutputError(printed.c_str());
      return ECMD_INVALID_ARGS;
    }
  }

  rc = doScomMultiple(target, io_groupScomEntries); if (rc) return rc;
  if (rc) {
    printed = "getScomGroup - Error occured performing doScomMultiple on ";
    printed += ecmdWriteTarget(target);
    printed += "\n";
    ecmdOutputError( printed.c_str() );
    return rc;
  }

  return rc;
}


/* ------------------------------------------------------------------------------------ */
/* Below are the functions that pass straigh through to the Dll                         */
/* Some have been moved here from ecmdClientCapiFunc.C to add additional debug messages */
/* ------------------------------------------------------------------------------------ */

/* enableRingCache and disableRingCache removed after v8.0 because they can now be generated - JTA 10/17/06 */
/* disableRingCache got moved back into here so we could add the common cache flush.  This is a snapshot
 of the generated code as of 9/4/2007 - JTA */
uint32_t ecmdDisableRingCache(ecmdChipTarget & i_target) {

  uint32_t rc;

#ifndef ECMD_STATIC_FUNCTIONS

  if (dlHandle == NULL) {
    fprintf(stderr,"dllDisableRingCache%s",": eCMD Function called before DLL has been loaded\n");
    exit(ECMD_DLL_INVALID);
  }

#endif

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_target);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t ecmdDisableRingCache(ecmdChipTarget & i_target)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"ecmdDisableRingCache");
  }
#endif

  /* This block of code was added from what was autogenerated to have the common flush if enabled */
  /* STGC00203320 - 09/04/2007 */
  if (ecmdIsRingCacheEnabled(i_target)) {
#ifdef ECMD_STATIC_FUNCTIONS
    rc = dllFlushRingCache(i_target);
#else
    if (DllFnTable[ECMD_FLUSHRINGCACHE] == NULL) {
      DllFnTable[ECMD_FLUSHRINGCACHE] = (void*)dlsym(dlHandle, "dllFlushRingCache");
      if (DllFnTable[ECMD_FLUSHRINGCACHE] == NULL) {
        fprintf(stderr,"dllFlushRingCache%s",": Unable to find function, must be an invalid DLL - program aborting\n"); 
        ecmdDisplayDllInfo();
        exit(ECMD_DLL_INVALID);
      }
    }

    uint32_t (*Function)(ecmdChipTarget &) = 
      (uint32_t(*)(ecmdChipTarget &))DllFnTable[ECMD_FLUSHRINGCACHE];
    rc =    (*Function)(i_target);
#endif
    if (rc) return rc;
  }

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllDisableRingCache(i_target);

#else

  if (DllFnTable[ECMD_DISABLERINGCACHE] == NULL) {
     DllFnTable[ECMD_DISABLERINGCACHE] = (void*)dlsym(dlHandle, "dllDisableRingCache");
     if (DllFnTable[ECMD_DISABLERINGCACHE] == NULL) {
       fprintf(stderr,"dllDisableRingCache%s",": Unable to find function, must be an invalid DLL - program aborting\n"); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)(ecmdChipTarget &) = 
      (uint32_t(*)(ecmdChipTarget &))DllFnTable[ECMD_DISABLERINGCACHE];

  rc =    (*Function)(i_target);

#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"ecmdDisableRingCache");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t ecmdDisableRingCache(ecmdChipTarget & i_target)",args);
   }
#endif

  if (rc) {
    std::string errorString;
    errorString = ecmdGetErrorMsg(rc, false, false, false);
    if (errorString.size()) ecmdOutput(errorString.c_str());
  }

  return rc;
}
