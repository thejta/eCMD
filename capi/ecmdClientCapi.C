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

/* $Header$ */

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <dlfcn.h>
#include <string>
#include <stdio.h>

#include <ecmdClientCapi.H>
#include <ecmdDllCapi.H>
#include <ecmdUtils.H>
#include <ecmdClientEnums.H>

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
int ecmdClientDebug = 0;
#endif

/* @brief This var stores the state of ring caching */
bool ecmdRingCacheEnabled = false;

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------


uint32_t ecmdLoadDll(std::string i_dllName) {


  const char* dlError;
  uint32_t rc = ECMD_SUCCESS;

#ifndef ECMD_STATIC_FUNCTIONS
  /* Only do this if it hasn't been done already */
  if (dlHandle != NULL) {
    return ECMD_SUCCESS;
  }
#endif

#ifndef ECMD_STRIP_DEBUG
    char* tmpptr = getenv("ECMD_DEBUG");
    if (tmpptr != NULL)
      ecmdClientDebug = atoi(tmpptr);
    else
      ecmdClientDebug = 0;
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

    dlHandle = dlopen(i_dllName.c_str(), RTLD_LAZY);
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

    /* Now we need to call loadDll on the dll itself so it can initialize */

    uint32_t (*Function)(const char *,uint32_t) = 
      (uint32_t(*)(const char *,uint32_t))(void*)dlsym(dlHandle, "dllLoadDll");
    if (!Function) {
      fprintf(stderr,"ecmdLoadDll: Unable to find LoadDll function, must be an invalid DLL\n");
      rc = ECMD_DLL_LOAD_FAILURE;
    } else {
      rc = (*Function)(ECMD_CAPI_VERSION, ecmdClientDebug);
    }

#else
    rc = dllLoadDll(ECMD_CAPI_VERSION, ecmdClientDebug);

#endif /* ECMD_STATIC_FUNCTIONS */


  return rc;
}

uint32_t ecmdUnloadDll() {

  uint32_t rc = ECMD_SUCCESS;
  uint32_t c_rc = ECMD_SUCCESS;

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

  return rc;
}

uint32_t ecmdCommandArgs(int* i_argc, char** i_argv[]) {

  uint32_t rc = ECMD_SUCCESS;


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

  return rc;
}

bool ecmdQueryTargetConfigured(ecmdChipTarget i_target, ecmdQueryData * i_queryData) {
  uint32_t rc = ECMD_SUCCESS;
  bool ret = false;
  bool myQuery = false;
  ecmdChipTarget queryTarget;

  std::list<ecmdCageData>::iterator ecmdCurCage;
  std::list<ecmdNodeData>::iterator ecmdCurNode;
  std::list<ecmdSlotData>::iterator ecmdCurSlot;
  std::list<ecmdChipData>::iterator ecmdCurChip;
  std::list<ecmdCoreData>::iterator ecmdCurCore;
  std::list<ecmdThreadData>::iterator ecmdCurThread;


  /* Do we need to do our own query ? */
  if (i_queryData == NULL) {
    i_queryData = new ecmdQueryData;
    queryTarget = i_target;
    myQuery = true;

    if (queryTarget.cageState == ECMD_TARGET_FIELD_UNUSED) {
      queryTarget.cageState = ECMD_TARGET_QUERY_IGNORE;
    } else
      queryTarget.cageState = ECMD_TARGET_QUERY_FIELD_VALID;


    if (queryTarget.nodeState == ECMD_TARGET_FIELD_UNUSED) {
      queryTarget.nodeState = ECMD_TARGET_QUERY_IGNORE;
    } else
      queryTarget.nodeState = ECMD_TARGET_QUERY_FIELD_VALID;

    if (queryTarget.slotState == ECMD_TARGET_FIELD_UNUSED) {
      queryTarget.slotState = ECMD_TARGET_QUERY_IGNORE;
    } else
      queryTarget.slotState = ECMD_TARGET_QUERY_FIELD_VALID;

    if (queryTarget.chipTypeState == ECMD_TARGET_FIELD_UNUSED) {
      queryTarget.chipTypeState = ECMD_TARGET_QUERY_IGNORE;
    } else
      queryTarget.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;

    if (queryTarget.posState == ECMD_TARGET_FIELD_UNUSED) {
      queryTarget.posState = ECMD_TARGET_QUERY_IGNORE;
    } else
      queryTarget.posState = ECMD_TARGET_QUERY_FIELD_VALID;

    if (queryTarget.coreState == ECMD_TARGET_FIELD_UNUSED) {
      queryTarget.coreState = ECMD_TARGET_QUERY_IGNORE;
    } else
      queryTarget.coreState = ECMD_TARGET_QUERY_FIELD_VALID;

    if (queryTarget.threadState == ECMD_TARGET_FIELD_UNUSED) {
      queryTarget.threadState = ECMD_TARGET_QUERY_IGNORE;
    } else
      queryTarget.threadState = ECMD_TARGET_QUERY_FIELD_VALID;


    rc = ecmdQueryConfig(queryTarget, *i_queryData, ECMD_QUERY_DETAIL_LOW);
    if (rc) {
      delete i_queryData;
      return ret;
    }
  }


  /* Now we have our data, let's start walking the data we have */
  for (ecmdCurCage = i_queryData->cageData.begin(); ecmdCurCage != i_queryData->cageData.end(); ecmdCurCage ++) {
    if (ecmdCurCage->cageId == i_target.cage) {
      if (i_target.nodeState == ECMD_TARGET_FIELD_UNUSED) {
        ret = true;
        break;
      }

      for (ecmdCurNode = ecmdCurCage->nodeData.begin(); ecmdCurNode != ecmdCurCage->nodeData.end(); ecmdCurNode ++) {
        if (ecmdCurNode->nodeId == i_target.node) {
          if (i_target.slotState == ECMD_TARGET_FIELD_UNUSED) {
            ret = true;
            break;
          }


          for (ecmdCurSlot = ecmdCurNode->slotData.begin(); ecmdCurSlot != ecmdCurNode->slotData.end(); ecmdCurSlot ++) {
            if (ecmdCurSlot->slotId == i_target.slot) {
              if (i_target.chipTypeState == ECMD_TARGET_FIELD_UNUSED || i_target.posState == ECMD_TARGET_FIELD_UNUSED) {
                ret = true;
                break;
              }

              for (ecmdCurChip = ecmdCurSlot->chipData.begin(); ecmdCurChip != ecmdCurSlot->chipData.end(); ecmdCurChip ++) {
                if (((ecmdCurChip->chipType == i_target.chipType) || (ecmdCurChip->chipCommonType == i_target.chipType)) &&
                    (ecmdCurChip->pos == i_target.pos)) {
                  if (i_target.coreState == ECMD_TARGET_FIELD_UNUSED) {
                    ret = true;
                    break;
                  }
                  
                  for (ecmdCurCore = ecmdCurChip->coreData.begin(); ecmdCurCore != ecmdCurChip->coreData.end(); ecmdCurCore ++) {
                    if (ecmdCurCore->coreId == i_target.core) {
                      if (i_target.threadState == ECMD_TARGET_FIELD_UNUSED) {
                        ret = true;
                        break;
                      }

                      for (ecmdCurThread = ecmdCurCore->threadData.begin(); ecmdCurThread != ecmdCurCore->threadData.end(); ecmdCurThread ++) {
                        if (ecmdCurThread->threadId == i_target.thread) {
                          ret = true;
                          break;
                        } /* curThreadId == tarThreadId */
                      } /* for ecmdCurThread */
                        
                      if (ret) break;
                    } /* curCoreId == tarCoreId */
                  } /* for ecmdCurCore */

                  if (ret) break;
                } /* curChipType == tarChipType && curChipPos == tarChipPos */
              } /* for ecmdCurChip */

              if (ret) break;
            } /* curSlotId == tarSlotId */
          } /* for ecmdCurSlot */

          if (ret) break;
        } /* curNodeId == tarNodeId */
      } /* for ecmdCurNode */

      if (ret) break;
    } /* curCageId == tarCageId */
  } /* for ecmdCurCage */


  if (myQuery) {
    delete i_queryData;
    i_queryData = NULL;
  }

  return ret;
}


/* ------------------------------------------------------------------------------------ */
/* Below are the functions that pass straigh through to the Dll                         */
/* Some have been moved here from ecmdClientCapiFunc.C to add additional debug messages */
/* ------------------------------------------------------------------------------------ */


void ecmdEnableRingCache() {

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug > 1) {
    std::string printed = "ECMD DEBUG (ecmdEnableRingCache) : Entering\n"; ecmdOutput(printed.c_str());
  }
#endif
  /* Set our local variable so we know caching is enabled */
  ecmdRingCacheEnabled = true;

#ifdef ECMD_STATIC_FUNCTIONS

  dllEnableRingCache();

#else

  if (dlHandle == NULL) {
    fprintf(stderr,"ecmdEnableRingCache: eCMD Function called before DLL has been loaded\n");
    exit(ECMD_DLL_INVALID);
  }

  if (DllFnTable[ECMD_ENABLERINGCACHE] == NULL) {
     DllFnTable[ECMD_ENABLERINGCACHE] = (void*)dlsym(dlHandle, "dllEnableRingCache");
     if (DllFnTable[ECMD_ENABLERINGCACHE] == NULL) {
       fprintf(stderr,"dllEnableRingCache : Unable to find dllEnableRingCache function, must be an invalid DLL - program aborting\n"); 
       exit(ECMD_DLL_INVALID);
     }
  }

  void (*Function)() = 
      (void(*)())DllFnTable[ECMD_ENABLERINGCACHE];

   (*Function)();

#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug > 1) {
    std::string printed = "ECMD DEBUG (ecmdEnableRingCache) : Exiting\n"; ecmdOutput(printed.c_str());
  }
#endif

}

uint32_t ecmdDisableRingCache() {

  uint32_t rc;

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug > 1) {
    std::string printed = "ECMD DEBUG (ecmdDisableRingCache) : Entering\n"; ecmdOutput(printed.c_str());
  }
#endif

  /* Set our local variable so we know caching is enabled */
  ecmdRingCacheEnabled = false;

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllDisableRingCache();

#else

  if (dlHandle == NULL) {
    fprintf(stderr,"ecmdDisableRingCache: eCMD Function called before DLL has been loaded\n");
    exit(ECMD_DLL_INVALID);
  }

  if (DllFnTable[ECMD_DISABLERINGCACHE] == NULL) {
     DllFnTable[ECMD_DISABLERINGCACHE] = (void*)dlsym(dlHandle, "dllDisableRingCache");
     if (DllFnTable[ECMD_DISABLERINGCACHE] == NULL) {
       fprintf(stderr,"dllDisableRingCache : Unable to find dllDisableRingCache function, must be an invalid DLL - program aborting\n"); 
       exit(ECMD_DLL_INVALID);
     }
  }

  uint32_t (*Function)() = 
      (uint32_t(*)())DllFnTable[ECMD_DISABLERINGCACHE];

  rc =    (*Function)();

#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug > 1) {
    std::string printed = "ECMD DEBUG (ecmdDisableRingCache) : Exiting\n"; ecmdOutput(printed.c_str());
  }
#endif

  return rc;

}
