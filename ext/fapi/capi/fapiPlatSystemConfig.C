
#include <stdio.h>

#include <ecmdClientCapi.H>
#include <ecmdSharedUtils.H>
#include <fapiClientCapi.H>
#include <fapiSystemConfig.H>
#include <fapiSharedUtils.H>
#ifndef ECMD_STATIC_FUNCTIONS
#include <fapiClientEnums.H>
#include <fapiTarget.H>
#endif

#include <ecmdUtils.H>

static const char ECMD_DLL_NOT_LOADED_ERROR[] = ": eCMD Function called before DLL has been loaded\n";
static const char ECMD_UNABLE_TO_FIND_FUNCTION_ERROR[] = ": Unable to find function, must be an invalid DLL - program aborting\n";

using namespace fapi;

#ifndef ECMD_STATIC_FUNCTIONS

 #include <dlfcn.h>

/* This is from ecmdClientCapiFunc.C */
 extern void * dlHandle;
 extern void * fapiDllFnTable[FAPI_NUMFUNCTIONS];

#else

 #include <fapiDllCapi.H>

#endif

extern bool fapiInitialized; 

#ifndef ECMD_STRIP_DEBUG
extern int ecmdClientDebug;
extern int fppCallCount;
extern bool ecmdDebugOutput;
#endif

fapi::ReturnCode fapiGetChildChiplets( const fapi::Target & i_chip, const fapi::TargetType i_chipletType, std::vector<fapi::Target> & o_chiplets, const fapi::TargetState i_state){

  ReturnCode rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiGetChildChiplets%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

  if (!fapiInitialized) {
    fprintf(stderr,"dllFapiGetChildChiplets: eCMD Extension not initialized before function called\n");
    fprintf(stderr,"dllFapiGetChildChiplets: OR eCMD fapi Extension not supported by plugin\n");
    exit(ECMD_DLL_INVALID);
  }

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_chip);
     args.push_back((void*) &i_chipletType);
     args.push_back((void*) &o_chiplets);
     args.push_back((void*) &i_state);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"ReturnCode fapiGetChildChiplets(const Target& i_chip, const TargetType & i_chipletType, std::vector<Target> & o_chiplets,fapi::TargetState i_state )",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapiGetChildChiplets");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapiGetChildChiplets(i_chip, i_chipletType, o_chiplets, i_state);
#else
  if (fapiDllFnTable[ECMD_FAPIGETCHILDCHIPLETS] == NULL) {
     fapiDllFnTable[ECMD_FAPIGETCHILDCHIPLETS] = (void*)dlsym(dlHandle, "dllFapiGetChildChiplets");
     if (fapiDllFnTable[ECMD_FAPIGETCHILDCHIPLETS] == NULL) {
       fprintf(stderr,"dllFapiGetChildChiplets%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const Target&,  const TargetType ,  std::vector<Target> &, const TargetState) = 
      (ReturnCode(*)(const Target&,  const TargetType,  std::vector<Target> &, const TargetState ))fapiDllFnTable[ECMD_FAPIGETCHILDCHIPLETS];
  rc =    (*Function)(i_chip, i_chipletType, o_chiplets, i_state);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiGetFunctionalChiplets");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"ReturnCode fapiGetFunctionalChiplets(const Target& i_chip, const TargetType & i_chiplet, std::vector<Target> & o_entries)",args);
   }
#endif

  return rc;
}

fapi::ReturnCode fapiGetOtherSideOfMemChannel(const fapi::Target & i_target, fapi::Target & o_target, const fapi::TargetState i_state)
{

  ReturnCode rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) 
  {
    fprintf(stderr,"dllFapiGetOtherSideOfMemChannel%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

  if (!fapiInitialized) 
  {
    fprintf(stderr,"dllFapiGetOtherSideOfMemChannel: eCMD Extension not initialized before function called\n");
    fprintf(stderr,"dllFapiGetOtherSideOfMemChannel: OR eCMD fapi Extension not supported by plugin\n");
    exit(ECMD_DLL_INVALID);
  }

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) 
  {
     args.push_back((void*) &i_target);
     args.push_back((void*) &o_target);
     args.push_back((void*) &i_state);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"ReturnCode fapiGetOtherSideOfMemChannel(const Target& i_target, Target & o_target, const fapi::TargetState i_state )",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapiGetOtherSideOfMemChannel");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapiGetOtherSideOfMemChannel(i_target, o_target, i_state);
#else
  if (fapiDllFnTable[ECMD_FAPIGETOTHERSIDEOFMEMCHANNEL] == NULL) 
  {
     fapiDllFnTable[ECMD_FAPIGETOTHERSIDEOFMEMCHANNEL] = (void*)dlsym(dlHandle, "dllFapiGetOtherSideOfMemChannel");
     if (fapiDllFnTable[ECMD_FAPIGETOTHERSIDEOFMEMCHANNEL] == NULL) {
       fprintf(stderr,"dllFapiGetOtherSideOfMemChannel%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const Target &, Target &, const TargetState) = 
      (ReturnCode(*)(const Target &, Target &, const TargetState ))fapiDllFnTable[ECMD_FAPIGETOTHERSIDEOFMEMCHANNEL];
  rc =    (*Function)(i_target, o_target, i_state);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) 
  {
      args.push_back((void*) &rc);
      ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiGetFunctionalChiplets");
      ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"ReturnCode fapiGetFunctionalChiplets(const Target& i_target, Target & o_target, const fapi::TargetState i_state)",args);
  }
#endif

  return rc;
}

#if 0
ReturnCode fapiGetExistingChiplets(const Target& i_handle, const TargetType  i_chiplet, std::vector<Target> &o_entries) {

  ReturnCode rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiGetExistingChiplets%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

  if (!fapiInitialized) {
    fprintf(stderr,"dllFapiGetExistingChiplets: eCMD Extension not initialized before function called\n");
    fprintf(stderr,"dllFapiGetExistingChiplets: OR eCMD fapi Extension not supported by plugin\n");
    exit(ECMD_DLL_INVALID);
  }

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_handle);
     args.push_back((void*) &i_chiplet);
     args.push_back((void*) &o_entries);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"ReturnCode fapiGetExistingChiplets(const Target& i_handle, const TargetType & i_chiplet, std::vector<Target> &o_entries)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapiGetExistingChiplets");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapiGetExistingChiplets(i_handle, i_chiplet, o_entries);
#else
  if (fapiDllFnTable[ECMD_FAPIGETEXISTINGCHIPLETS] == NULL) {
     fapiDllFnTable[ECMD_FAPIGETEXISTINGCHIPLETS] = (void*)dlsym(dlHandle, "dllFapiGetExistingChiplets");
     if (fapiDllFnTable[ECMD_FAPIGETEXISTINGCHIPLETS] == NULL) {
       fprintf(stderr,"dllFapiGetExistingChiplets%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const Target&,  const TargetType,  std::vector<Target> &) = 
      (ReturnCode(*)(const Target&,  const TargetType,  std::vector<Target> &))fapiDllFnTable[ECMD_FAPIGETEXISTINGCHIPLETS];
  rc =    (*Function)(i_handle, i_chiplet, o_entries);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiGetExistingChiplets");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"ReturnCode fapiGetExistingChiplets(const Target& i_handle, const TargetType & i_chiplet, std::vector<Target> &o_entries)",args);
   }
#endif

  return rc;
}

fapi::ReturnCode fapiGetFunctionalDimms(const fapi::Target & i_target, std::vector<fapi::Target> & o_dimms) {

  ReturnCode rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiGetFunctionalDimms%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

  if (!fapiInitialized) {
    fprintf(stderr,"dllFapiGetFunctionalDimms: eCMD Extension not initialized before function called\n");
    fprintf(stderr,"dllFapiGetFunctionalDimms: OR eCMD fapi Extension not supported by plugin\n");
    exit(ECMD_DLL_INVALID);
  }

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_target);
     args.push_back((void*) &o_dimms);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"ReturnCode fapiGetFunctionalDimms(const Target&  std::vector<Target> & o_dimms)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapiGetFunctionalDimms");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapiGetFunctionalDimms(i_target, o_dimms);
#else
  if (fapiDllFnTable[ECMD_FAPIGETFUNCTIONALDIMMS] == NULL) {
     fapiDllFnTable[ECMD_FAPIGETFUNCTIONALDIMMS] = (void*)dlsym(dlHandle, "dllFapiGetFunctionalDimms");
     if (fapiDllFnTable[ECMD_FAPIGETFUNCTIONALDIMMS] == NULL) {
       fprintf(stderr,"dllFapiGetFunctionalDimms%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const Target&,  std::vector<Target> &) = 
      (ReturnCode(*)(const Target&,  std::vector<Target> &))fapiDllFnTable[ECMD_FAPIGETFUNCTIONALDIMMS];
  rc =    (*Function)(i_target, o_dimms);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiGetFunctionalDimms");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"ReturnCode fapiGetFunctionalDimms(const Target& i_handle, const TargetType & i_dimms, std::vector<Target> & o_entries)",args);
   }
#endif

  return rc;
}
#endif 


fapi::ReturnCode fapiGetAssociatedDimms(const fapi::Target & i_target, std::vector<fapi::Target> & o_dimms, const fapi::TargetState i_state) {

  ReturnCode rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiGetAssociatedDimms%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

  if (!fapiInitialized) {
    fprintf(stderr,"dllFapiGetAssociatedDimms: eCMD Extension not initialized before function called\n");
    fprintf(stderr,"dllFapiGetAssociatedDimms: OR eCMD fapi Extension not supported by plugin\n");
    exit(ECMD_DLL_INVALID);
  }

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_target);
     args.push_back((void*) &o_dimms);
     args.push_back((void*) &i_state);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"ReturnCode fapiGetAssociatedDimms(const Target&,  std::vector<Target> & o_dimms, const fapi::TargetState i_state)",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapiGetExistingDimms");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapiGetAssociatedDimms(i_target, o_dimms,i_state);
#else
  if (fapiDllFnTable[ECMD_FAPIGETASSOCIATEDDIMMS] == NULL) {
     fapiDllFnTable[ECMD_FAPIGETASSOCIATEDDIMMS] = (void*)dlsym(dlHandle, "dllFapiGetAssociatedDimms");
     if (fapiDllFnTable[ECMD_FAPIGETASSOCIATEDDIMMS] == NULL) {
       fprintf(stderr,"dllFapiGetAssociatedDimms%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const Target&,  std::vector<Target> &, const fapi::TargetState) = 
      (ReturnCode(*)(const Target&,  std::vector<Target> &, const fapi::TargetState))fapiDllFnTable[ECMD_FAPIGETASSOCIATEDDIMMS];
  rc =    (*Function)(i_target, o_dimms, i_state);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiGetAssociatedDimms");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"ReturnCode fapiGetAssociatedDimms(const Target& i_handle, const TargetType & i_dimms, std::vector<Target> & o_entries, const fapi::TargetState i_state)",args);
   }
#endif

  return rc;
}

fapi::ReturnCode fapiGetParentChip( const fapi::Target & i_chiplet, fapi::Target & o_chip){

  ReturnCode rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapiGetParentChip%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

  if (!fapiInitialized) {
    fprintf(stderr,"dllFapiGetParentChip: eCMD Extension not initialized before function called\n");
    fprintf(stderr,"dllFapiGetParentChip: OR eCMD fapi Extension not supported by plugin\n");
    exit(ECMD_DLL_INVALID);
  }

#ifndef ECMD_STRIP_DEBUG
  int myTcount;
  std::vector< void * > args;
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &i_chiplet);
     args.push_back((void*) &o_chip);
     fppCallCount++;
     myTcount = fppCallCount;
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"ReturnCode fapiGetParentChip(const Target& i_chiplet, Target & o_chip )",args);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapiGetParentChip");
  }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
  rc = dllFapiGetParentChip(i_chiplet, o_chip);
#else
  if (fapiDllFnTable[ECMD_FAPIGETPARENTCHIP] == NULL) {
     fapiDllFnTable[ECMD_FAPIGETPARENTCHIP] = (void*)dlsym(dlHandle, "dllFapiGetParentChip");
     if (fapiDllFnTable[ECMD_FAPIGETPARENTCHIP] == NULL) {
       fprintf(stderr,"dllFapiGetParentChip%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const Target&,  Target& ) = 
      (ReturnCode(*)(const Target&,  Target&))fapiDllFnTable[ECMD_FAPIGETPARENTCHIP];
  rc =    (*Function)(i_chiplet, o_chip);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) {
     args.push_back((void*) &rc);
     ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiGetParentChip");
     ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"ReturnCode fapiGetParentChip(const Target& i_chiplet, Target & o_chip)",args);
   }
#endif

  return rc;
}

