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
