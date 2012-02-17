// Copyright ***********************************************************
//                                                                      
// File fapiPlatHwpExecuter.C                                   
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

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <dlfcn.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <fapiStructs.H>
#include <fapiReturnCodes.H>
#include <fapiDllCapi.H> 
#include <fapiClientCapi.H>
#include <ecmdReturnCodes.H>

// these should be in the fapi namespace, right? -farrugia JFDEBUG
   
#define FAPI_ERR(_fmt_, _args_...) printf("FAPI ERR>: "_fmt_"\n", ##_args_)   //JFDEBUG local defined due to dll load errors

// dlopens a shared library and returns the handle
int openSharedLib(const std::string & i_libName, void * & o_pLibHandle)
{
    uint32_t rc = fapi::FAPI_RC_SUCCESS;
    std::string sharedLibPath;

#ifdef LINUX
    std::string tmp = (i_libName + "_x86.so");
#else
    std::string tmp = (i_libName + "_aix.so");
#endif 

#if defined(ECMD_STATIC_FUNCTIONS) || defined(FAPIARCHIVE)
    rc = fapiQueryFileLocation(fapi::FAPI_FILE_HWP, tmp, sharedLibPath, "default");
#else 
    rc = dllFapiQueryFileLocation(fapi::FAPI_FILE_HWP, tmp, sharedLibPath, "default");
#endif 
    if (rc)
    {
        FAPI_ERR("fapiQueryFileLocation failed with rc = 0x%x\n", rc);
        return rc;
    }
    
    o_pLibHandle = dlopen(sharedLibPath.c_str(), RTLD_LAZY);
    if (o_pLibHandle == NULL)
    {
        FAPI_ERR("dlopen error '%s'\n", dlerror());
        return ECMD_FAILURE;
    }

    return rc;
}

// Gets a function symbol address from a dlopened shared library
int getSymAddr(char * i_pFuncName, void * i_pLibHandle, void * & o_pSymAddr)
{
    o_pSymAddr = dlsym(i_pLibHandle, i_pFuncName);

    if (o_pSymAddr == NULL)
    {
        FAPI_ERR("dlsym error '%s'\n", dlerror());
        return ECMD_FAILURE;
    }

    return fapi::FAPI_RC_SUCCESS;
}

// dlcloses a shared library
void closeSharedLib(void * i_pLibHandle)
{
    int l_res = dlclose(i_pLibHandle);

    if (l_res)
    {
        FAPI_ERR("dlclose error '%s'\n", dlerror());
    }
}
