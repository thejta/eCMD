#include <plat_vpd_access.H>

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
    fapi2::ReturnCode getVPD(const ecmdChipTarget& i_target,
                             const uint32_t i_version,
                             void * io_vpd_info,
                             uint8_t* o_blob)
    {


        fapi2::ReturnCode rc;
        uint32_t l_ecmdRc;
  

#ifndef ECMD_STATIC_FUNCTIONS
        if (dlHandle == NULL)
        {
            fprintf(stderr,"dllFapi2GetVpdBlob%s",ECMD_DLL_NOT_LOADED_ERROR);
            exit(ECMD_DLL_INVALID);
        }
#endif
        if (!fapi2Initialized)
        {
            fprintf(stderr,"dllFapi2GetVpdBlob: eCMD Extension not initialized before function called\n");
            fprintf(stderr,"dllFapi2GetVpdBlob: OR eCMD fapi Extension not supported by plugin\n");
            exit(ECMD_DLL_INVALID);
        }

#ifndef ECMD_STRIP_DEBUG
        int myTcount;
        std::vector< void * > args;
        if (ecmdClientDebug != 0)
        {
            args.push_back((void*) &i_target);
            args.push_back((void*) &i_version);
            args.push_back((void*) &io_vpd_info);
            args.push_back((void*) &o_blob);
            fppCallCount++;
            myTcount = fppCallCount;
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONIN,"uint32_t dllFapi2GetVpdBlob(const ecmdChipTarget & i_target, const uint32_t i_version, void * io_vpd_info, const uint8_t * const o_blob) )",args);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONIN,"dllFapi2GetVpdBlob");
        }
#endif

#ifdef ECMD_STATIC_FUNCTIONS
        l_ecmdRc = dllFapi2GetVpdBlob(i_target, i_version, io_vpd_info, o_blob);
#else
        if (fapi2DllFnTable[ECMD_FAPI2GETVPDBLOB] == NULL)
        {
            fapi2DllFnTable[ECMD_FAPI2GETVPDBLOB] = (void*)dlsym(dlHandle, "dllFapi2GetVpdBlob");
            if (fapi2DllFnTable[ECMD_FAPI2GETVPDBLOB] == NULL)
            {
                fprintf(stderr,"dllFapi2GetVpdBlob%s",ECMD_UNABLE_TO_FIND_FUNCTION_ERROR); 
                ecmdDisplayDllInfo();
                exit(ECMD_DLL_INVALID);
            }
        }

        uint32_t (*Function)(const ecmdChipTarget &, const uint32_t, void *, uint8_t *) = 
            (uint32_t(*)(const ecmdChipTarget &, const uint32_t, void *, uint8_t *))fapi2DllFnTable[ECMD_FAPI2GETVPDBLOB];
        l_ecmdRc = (*Function)(i_target, i_version, io_vpd_info, o_blob);
#endif
        if (l_ecmdRc)
        {
            rc = (fapi2::ReturnCodes) l_ecmdRc; 
        }

#ifndef ECMD_STRIP_DEBUG
        if (ecmdClientDebug != 0) {
            args.push_back((void*) &l_ecmdRc);
            ecmdFunctionTimer(myTcount,ECMD_TMR_FUNCTIONOUT,"dllFapi2GetVpdBlob");
            ecmdFunctionParmPrinter(myTcount,ECMD_FPP_FUNCTIONOUT,"uint32_t dllFapi2GetVpdBlob(const ecmdChipTarget & i_target, const uint32_t i_version, void * io_vpd_info, uint8_t * o_blob)",args);
        }
#endif

        return rc;
    }

    fapi2::ReturnCode getVPD(const fapi2::Target<fapi2::TARGET_TYPE_MCS>& i_target,
                             fapi2::VPDInfo<fapi2::TARGET_TYPE_MCS>& io_vpd_info,
                             uint8_t* o_blob)
    {
        ecmdChipTarget ecmdTarget;
        fapiTargetToEcmdTarget(i_target, ecmdTarget);
	    uint32_t l_version = 1;
#if 0
///
/// version 1 of MCS VPDInfo
///
template<>
class VPDInfo<TARGET_TYPE_MCS>
{
    public:
        // @brief VPDInfo constructor
        VPDInfo( const fapi2::MemVpdData& i_vpd_type)
            : iv_vpd_type(i_vpd_type),
              iv_size(0), iv_freq(0), iv_rank_count_dimm_0(0),
              iv_rank_count_dimm_1(0), iv_dimm_drop_per_port(0)
        {};

        // type of vpd field to return
        fapi2::MemVpdData_t iv_vpd_type;

        // size of the vpd data
        size_t   iv_size;
        uint64_t iv_freq;
        uint64_t iv_rank_count_dimm_0;
        uint64_t iv_rank_count_dimm_1;
        uint64_t iv_dimm_drop_per_port;
};
#endif
        return fapi2plat::getVPD(ecmdTarget, l_version, static_cast<void *>(&io_vpd_info), o_blob);
    }
}; // namespace fapi2plat
