
#include <stdio.h>

#include <ecmdClientCapi.H>
#include <ecmdSharedUtils.H>
#include <fapi2ClientCapi.H>
#include <fapi2SystemConfig.H>
#include <fapi2SharedUtils.H>
#ifndef ECMD_STATIC_FUNCTIONS
#include <fapi2ClientEnums.H>
#include <target.H>
#endif

#include <ecmdUtils.H>

static const char ECMD_DLL_NOT_LOADED_ERROR[] = ": eCMD Function called before DLL has been loaded\n";
static const char ECMD_UNABLE_TO_FIND_FUNCTION_ERROR[] = ": Unable to find function, must be an invalid DLL - program aborting\n";

#ifndef ECMD_STATIC_FUNCTIONS

 #include <dlfcn.h>

/* This is from ecmdClientCapiFunc.C */
 extern void * dlHandle;
 extern void * fapi2DllFnTable[FAPI2_NUMFUNCTIONS];

#else

 #include <fapi2DllCapi.H>

#endif

extern bool fapi2Initialized; 

#ifndef ECMD_STRIP_DEBUG
extern int ecmdClientDebug;
extern int fppCallCount;
extern bool ecmdDebugOutput;
#endif

uint32_t fapi2GetAssociatedTargets( const ecmdChipTarget & i_target, const fapi2::TargetType i_associatedTargetType, std::list<const ecmdChipTarget *> & o_targets, const fapi2::TargetState i_state)
{
    uint32_t l_rc;

#ifndef ECMD_STATIC_FUNCTIONS
    if (dlHandle == NULL)
    {
        fprintf(stderr,"dllFapi2GetAssociatedTargets%s",ECMD_DLL_NOT_LOADED_ERROR);
        exit(ECMD_DLL_INVALID);
    }
#endif

    if (!fapi2Initialized)
    {
        fprintf(stderr,"dllFapi2GetAssociatedTargets: eCMD Extension not initialized before function called\n");
        fprintf(stderr,"dllFapi2GetAssociatedTargets: OR eCMD fapi2 Extension not supported by plugin\n");
        exit(ECMD_DLL_INVALID);
    }

#ifndef ECMD_STRIP_DEBUG
    int myTcount;
    std::vector< void * > args;
    if (ecmdClientDebug != 0)
    {
        args.push_back((void*) &i_target);
        args.push_back((void*) &i_associatedTargetType);
        args.push_back((void*) &o_targets);
        args.push_back((void*) &i_state);
        fppCallCount++;
        myTcount = fppCallCount;
        ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t fapi2GetAssociatedTargets(const ecmdChipTarget & i_target, const fapi2::TargetType i_associatedTargetType, std::list<const ecmdChipTarget *> & o_targets, const fapi2::TargetState i_state)",args);
        ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapi2GetAssociatedTargets");
    }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
    l_rc = dllFapi2GetAssocaitedTargets(i_target, i_associatedTargetType, o_targets, i_state);
#else
    if (fapi2DllFnTable[ECMD_FAPI2GETASSOCIATEDTARGETS] == NULL)
    {
        fapi2DllFnTable[ECMD_FAPI2GETASSOCIATEDTARGETS] = (void*)dlsym(dlHandle, "dllFapi2GetAssociatedTargets");
        if (fapi2DllFnTable[ECMD_FAPI2GETASSOCIATEDTARGETS] == NULL)
        {
            fprintf(stderr,"dllFapi2GetAssociatedTargets%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
            ecmdDisplayDllInfo();
            exit(ECMD_DLL_INVALID);
        }
    }

    uint32_t (*Function)(const ecmdChipTarget &, const fapi2::TargetType, std::list<const ecmdChipTarget *> &, const fapi2::TargetState) = 
        (uint32_t(*)(const ecmdChipTarget &, const fapi2::TargetType, std::list<const ecmdChipTarget *> &, const fapi2::TargetState ))fapi2DllFnTable[ECMD_FAPI2GETASSOCIATEDTARGETS];
    l_rc = (*Function)(i_target, i_associatedTargetType, o_targets, i_state);
#endif

#ifndef ECMD_STRIP_DEBUG
    if (ecmdClientDebug != 0)
    {
        args.push_back((void*) &l_rc);
        ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapi2GetAssociatedTargets");
        ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t fapi2GetAssociatedTargets(const ecmdChipTarget & i_target, const fapi2::TargetType i_associatedTargetType, std::list<const ecmdChipTarget *> & o_targets, const fapi2::TargetState i_state)",args);
    }
#endif

    return l_rc;
}
uint32_t fapi2GetTargetType( const ecmdChipTarget & i_target, fapi2::TargetType & o_targetType)
{
    uint32_t l_rc;

#ifndef ECMD_STATIC_FUNCTIONS
    if (dlHandle == NULL)
    {
        fprintf(stderr,"dllFapi2GetTargetType%s",ECMD_DLL_NOT_LOADED_ERROR);
        exit(ECMD_DLL_INVALID);
    }
#endif

    if (!fapi2Initialized)
    {
        fprintf(stderr,"dllFapi2GetTargetType: eCMD Extension not initialized before function called\n");
        fprintf(stderr,"dllFapi2GetTargetType: OR eCMD fapi2 Extension not supported by plugin\n");
        exit(ECMD_DLL_INVALID);
    }

#ifndef ECMD_STRIP_DEBUG
    int myTcount;
    std::vector< void * > args;
    if (ecmdClientDebug != 0)
    {
        args.push_back((void*) &i_target);
        args.push_back((void*) &o_targetType);
        fppCallCount++;
        myTcount = fppCallCount;
        ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t fapi2GetTargetType(const ecmdChipTarget & i_target, fapi2::TargetType & o_targetType)",args);
        ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"fapi2GetTargetType");
    }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
    l_rc = dllFapi2GetTargetType(i_target, o_targetType);
#else
    if (fapi2DllFnTable[ECMD_FAPI2GETTARGETTYPE] == NULL)
    {
        fapi2DllFnTable[ECMD_FAPI2GETTARGETTYPE] = (void*)dlsym(dlHandle, "dllFapi2GetTargetType");
        if (fapi2DllFnTable[ECMD_FAPI2GETTARGETTYPE] == NULL)
        {
            fprintf(stderr,"dllFapi2GetTargetType%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
            ecmdDisplayDllInfo();
            exit(ECMD_DLL_INVALID);
        }
    }

    uint32_t (*Function)(const ecmdChipTarget &, fapi2::TargetType &) =
        (uint32_t(*)(const ecmdChipTarget &, fapi2::TargetType &))fapi2DllFnTable[ECMD_FAPI2GETTARGETTYPE];
    l_rc = (*Function)(i_target, o_targetType);
#endif

#ifndef ECMD_STRIP_DEBUG
    if (ecmdClientDebug != 0)
    {
        args.push_back((void*) &l_rc);
        ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapi2GetTargetType");
        ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t fapi2GetTargetType(const ecmdChipTarget & i_target, fapi2::TargetType & o_targetType)",args);
    }
#endif

    return l_rc;
}

#if 0
fapi::ReturnCode fapiGetChildChiplets( const fapi::Target & i_chip, const fapi::TargetType i_chipletType, std::vector<fapi::Target> & o_chiplets, const fapi::TargetState i_state){

  ReturnCode rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapi2GetChildChiplets%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

  if (!fapi2Initialized) {
    fprintf(stderr,"dllFapi2GetChildChiplets: eCMD Extension not initialized before function called\n");
    fprintf(stderr,"dllFapi2GetChildChiplets: OR eCMD fapi Extension not supported by plugin\n");
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
  rc = dllFapi2GetChildChiplets(i_chip, i_chipletType, o_chiplets, i_state);
#else
  if (fapi2DllFnTable[ECMD_FAPI2GETCHILDCHIPLETS] == NULL) {
     fapi2DllFnTable[ECMD_FAPI2GETCHILDCHIPLETS] = (void*)dlsym(dlHandle, "dllFapi2GetChildChiplets");
     if (fapi2DllFnTable[ECMD_FAPI2GETCHILDCHIPLETS] == NULL) {
       fprintf(stderr,"dllFapi2GetChildChiplets%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const Target&,  const TargetType ,  std::vector<Target> &, const TargetState) = 
      (ReturnCode(*)(const Target&,  const TargetType,  std::vector<Target> &, const TargetState ))fapi2DllFnTable[ECMD_FAPI2GETCHILDCHIPLETS];
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
    fprintf(stderr,"dllFapi2GetOtherSideOfMemChannel%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

  if (!fapi2Initialized) 
  {
    fprintf(stderr,"dllFapi2GetOtherSideOfMemChannel: eCMD Extension not initialized before function called\n");
    fprintf(stderr,"dllFapi2GetOtherSideOfMemChannel: OR eCMD fapi Extension not supported by plugin\n");
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
  rc = dllFapi2GetOtherSideOfMemChannel(i_target, o_target, i_state);
#else
  if (fapi2DllFnTable[ECMD_FAPI2GETOTHERSIDEOFMEMCHANNEL] == NULL) 
  {
     fapi2DllFnTable[ECMD_FAPI2GETOTHERSIDEOFMEMCHANNEL] = (void*)dlsym(dlHandle, "dllFapi2GetOtherSideOfMemChannel");
     if (fapi2DllFnTable[ECMD_FAPI2GETOTHERSIDEOFMEMCHANNEL] == NULL) {
       fprintf(stderr,"dllFapi2GetOtherSideOfMemChannel%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const Target &, Target &, const TargetState) = 
      (ReturnCode(*)(const Target &, Target &, const TargetState ))fapi2DllFnTable[ECMD_FAPI2GETOTHERSIDEOFMEMCHANNEL];
  rc =    (*Function)(i_target, o_target, i_state);
#endif

#ifndef ECMD_STRIP_DEBUG
  if (ecmdClientDebug != 0) 
  {
      args.push_back((void*) &rc);
      ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"fapiGetOtherSideOfMemChannel");
      ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"ReturnCode fapiGetOtherSideOfMemChannel(const Target& i_target, Target & o_target, const fapi::TargetState i_state)",args);
  }
#endif

  return rc;
}

fapi::ReturnCode fapiGetAssociatedDimms(const fapi::Target & i_target, std::vector<fapi::Target> & o_dimms, const fapi::TargetState i_state) {

  ReturnCode rc;

#ifndef ECMD_STATIC_FUNCTIONS
  if (dlHandle == NULL) {
    fprintf(stderr,"dllFapi2GetAssociatedDimms%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

  if (!fapi2Initialized) {
    fprintf(stderr,"dllFapi2GetAssociatedDimms: eCMD Extension not initialized before function called\n");
    fprintf(stderr,"dllFapi2GetAssociatedDimms: OR eCMD fapi Extension not supported by plugin\n");
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
  rc = dllFapi2GetAssociatedDimms(i_target, o_dimms,i_state);
#else
  if (fapi2DllFnTable[ECMD_FAPI2GETASSOCIATEDDIMMS] == NULL) {
     fapi2DllFnTable[ECMD_FAPI2GETASSOCIATEDDIMMS] = (void*)dlsym(dlHandle, "dllFapi2GetAssociatedDimms");
     if (fapi2DllFnTable[ECMD_FAPI2GETASSOCIATEDDIMMS] == NULL) {
       fprintf(stderr,"dllFapi2GetAssociatedDimms%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const Target&,  std::vector<Target> &, const fapi::TargetState) = 
      (ReturnCode(*)(const Target&,  std::vector<Target> &, const fapi::TargetState))fapi2DllFnTable[ECMD_FAPI2GETASSOCIATEDDIMMS];
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
    fprintf(stderr,"dllFapi2GetParentChip%s",ECMD_DLL_NOT_LOADED_ERROR);
    exit(ECMD_DLL_INVALID);
  }
#endif

  if (!fapi2Initialized) {
    fprintf(stderr,"dllFapi2GetParentChip: eCMD Extension not initialized before function called\n");
    fprintf(stderr,"dllFapi2GetParentChip: OR eCMD fapi Extension not supported by plugin\n");
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
  rc = dllFapi2GetParentChip(i_chiplet, o_chip);
#else
  if (fapi2DllFnTable[ECMD_FAPI2GETPARENTCHIP] == NULL) {
     fapi2DllFnTable[ECMD_FAPI2GETPARENTCHIP] = (void*)dlsym(dlHandle, "dllFapi2GetParentChip");
     if (fapi2DllFnTable[ECMD_FAPI2GETPARENTCHIP] == NULL) {
       fprintf(stderr,"dllFapi2GetParentChip%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
       ecmdDisplayDllInfo();
       exit(ECMD_DLL_INVALID);
     }
  }

  ReturnCode (*Function)(const Target&,  Target& ) = 
      (ReturnCode(*)(const Target&,  Target&))fapi2DllFnTable[ECMD_FAPI2GETPARENTCHIP];
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

#endif
