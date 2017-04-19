#include <return_code.H>
#include <plat_spd_access.H>

#include <dlfcn.h>

#include <fapi2DllCapi.H>
#include <fapi2ClientEnums.H>

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------

static const char ECMD_DLL_NOT_LOADED_ERROR[] = ": eCMD Function called before DLL has been loaded\n";
static const char ECMD_UNABLE_TO_FIND_FUNCTION_ERROR[] = ": Unable to find function, must be an invalid DLL - program aborting\n";


#ifndef ECMD_STATIC_FUNCTIONS
/* This is from ecmdClientCapiFunc.C */
extern void * dlHandle;
/* These are from fapiClientCapiFunc.C */
extern void * fapi2DllFnTable[FAPI2_NUMFUNCTIONS];
#endif

extern bool fapi2Initialized;

#ifndef ECMD_STRIP_DEBUG
extern int ecmdClientDebug;
extern int fppCallCount;
extern bool ecmdDebugOutput;
#endif

namespace fapi2plat
{
    fapi2::ReturnCode getSPD( const ecmdChipTarget& i_target,
                              uint8_t* o_blob,
                              size_t& s)
    {


        fapi2::ReturnCode rc;
        uint32_t l_ecmdRc;
  

#ifndef ECMD_STATIC_FUNCTIONS
        if (dlHandle == NULL)
        {
            fprintf(stderr,"dllFapi2GetSpdBlob%s",ECMD_DLL_NOT_LOADED_ERROR);
            exit(ECMD_DLL_INVALID);
        }
#endif
        if (!fapi2Initialized)
        {
            fprintf(stderr,"dllFapi2GetSpdBlob: eCMD Extension not initialized before function called\n");
            fprintf(stderr,"dllFapi2GetSpdBlob: OR eCMD fapi Extension not supported by plugin\n");
            exit(ECMD_DLL_INVALID);
        }

#ifndef ECMD_STRIP_DEBUG
        int myTcount;
        std::vector< void * > args;
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &i_target);
            args.push_back((void*) &o_blob);
            args.push_back((void*) &s);
            fppCallCount++;
            myTcount = fppCallCount;
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t dllFapi2GetSpdBlob(const ecmdChipTarget & i_target, uint8_t* o_blob, size_t& s) )",args);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"dllFapi2GetSpdBlob");
        }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
        l_ecmdRc = dllFapi2GetSpdBlob(i_target, o_blob, s);
#else
        if (fapi2DllFnTable[ECMD_FAPI2GETSPDBLOB] == NULL)
        {
            fapi2DllFnTable[ECMD_FAPI2GETSPDBLOB] = (void*)dlsym(dlHandle, "dllFapi2GetSpdBlob");
            if (fapi2DllFnTable[ECMD_FAPI2GETSPDBLOB] == NULL)
            {
                fprintf(stderr,"dllFapi2GetSpdBlob%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
                ecmdDisplayDllInfo();
                exit(ECMD_DLL_INVALID);
            }
        }

        uint32_t (*Function)(const ecmdChipTarget &, uint8_t *, size_t &) = 
            (uint32_t(*)(const ecmdChipTarget &, uint8_t *, size_t &))fapi2DllFnTable[ECMD_FAPI2GETSPDBLOB];
        l_ecmdRc = (*Function)(i_target, o_blob, s);
#endif
        if (l_ecmdRc)
        {
            rc = (fapi2::ReturnCodes) l_ecmdRc; 
        }

#ifndef ECMD_STRIP_DEBUG
        if (ecmdClientDebug != 0) {
            args.push_back((void*) &l_ecmdRc);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"dllFapi2GetSpdBlob");
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t dllFapi2GetSpdBlob(const ecmdChipTarget & i_target, uint8_t * o_blob, size_t& s)",args);
        }
#endif

        return rc;
    }

    fapi2::ReturnCode getSPD( const fapi2::Target<fapi2::TARGET_TYPE_DIMM>& i_target,
                              uint8_t* o_blob,
                              size_t& s)
    {
        ecmdChipTarget ecmdTarget;
        fapiTargetToEcmdTarget(i_target, ecmdTarget);
        return fapi2plat::getSPD(ecmdTarget, o_blob, s);
    }
}; // namespace fapi2plat
