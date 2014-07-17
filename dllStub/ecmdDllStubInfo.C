// Copyright ***********************************************************
//                                                                      
// File ecmdDllStubInfo.C                                   
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
#define ecmdDllStubInfo_C
#include <stdio.h>

#include <ecmdDllCapi.H>
#include <ecmdStructs.H>
#include <ecmdReturnCodes.H>
#undef ecmdDllStubInfo_C
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

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------


uint32_t dllQueryDllInfo(ecmdDllInfo & o_dllInfo) {
  char tmp[200];

  o_dllInfo.dllBuildInfo = "Test eCMD Plugin";

  o_dllInfo.dllType = ECMD_DLL_STUB;
  o_dllInfo.dllProduct = ECMD_DLL_PRODUCT_UNKNOWN;
  o_dllInfo.dllProductType = "Stub DLL";
  o_dllInfo.dllEnv = ECMD_DLL_ENV_HW;  

  sprintf(tmp,"%s %s CST",__DATE__,__TIME__);
  o_dllInfo.dllBuildDate = tmp;
  o_dllInfo.dllCapiVersion = ECMD_CAPI_VERSION;

  return 0;
}
