// Copyright **********************************************************
//                                                                      
// File ecmdDllCapi.H                                               
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 2003                                         
//                                                                      
// The source code for this program is not published or otherwise       
// divested of its trade secrets, irrespective of what has been         
// deposited with the U.S. Copyright Office.                             
//                                                                      
// End Copyright ******************************************************


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <inttypes.h>
#include <stdio.h>

#include <ecmdDllCapi.H>
#include <ecmdStructs.H>
#include <ecmdReturnCodes.H>
#include <ecmdDataBuffer.H>

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------


// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              willsj Initial Creation
//
// End Change Log *****************************************************


//--------------------------------------------------------------------
//  Function Definitions                                               
//--------------------------------------------------------------------
//
//   These are just stubs, used for testing the out the DLL

int dllInitDll() {
  /* This is where we would init any local variables to the dll */
  return ECMD_SUCCESS;
}

int dllFreeDll() {
  return ECMD_SUCCESS;
}

int dllSpecificCommandArgs(int* argc, char** argv[]) {
  return ECMD_SUCCESS;
}


int dllGetRing (ecmdChipTarget & target, const char * ringName, ecmdDataBuffer & data) { return ECMD_SUCCESS; }

int dllPutRing (ecmdChipTarget & target, const char * ringName, ecmdDataBuffer & data) { return ECMD_SUCCESS; }


int dllGetScom (ecmdChipTarget & target, uint32_t address, ecmdDataBuffer & data) {

  return ECMD_SUCCESS;
}

int dllPutScom (ecmdChipTarget & target, uint32_t address, ecmdDataBuffer & data) { return ECMD_SUCCESS; }


int dllGetArray (ecmdChipTarget & target, const char * arrayName, uint32_t * address, ecmdDataBuffer & data) { return ECMD_SUCCESS; }

int dllPutArray (ecmdChipTarget & target, const char * arrayName, uint32_t * address, ecmdDataBuffer & data) { return ECMD_SUCCESS; }

int dllQueryDllInfo(ecmdDllInfo & o_dllInfo) {
  char tmp[100];
  o_dllInfo.dllType = ECMD_DLL_STUB;
  o_dllInfo.dllProduct = ECMD_DLL_PRODUCT_UNKNOWN;
  o_dllInfo.dllEnv = ECMD_DLL_ENV_SIM;  
  sprintf(tmp,"%s %s",__DATE__,__TIME__);
  o_dllInfo.dllBuildDate = tmp;
  o_dllInfo.dllCapiVersion = ECMD_CAPI_VERSION;
  return ECMD_SUCCESS;
}

int dllQueryConfig(ecmdChipTarget & target,ecmdQueryData & queryData, ecmdQueryDetail_t i_detail) {

  ecmdCoreData coreData;
  ecmdChipData chipData;
  ecmdNodeData nodeData;
  ecmdSlotData slotData;
  ecmdCageData cageData;
  ecmdThreadData threadData;

  threadData.threadId = 0;

  /* Let's return some dummy info , we will return a proc with cores and threads */
  coreData.coreId = 0;
  coreData.numProcThreads = 2;
  coreData.threadData.push_front(threadData);

  chipData.coreData.push_front(coreData);
  chipData.chipType = "gr";
  chipData.pos = 0;
  slotData.chipData.push_front(chipData);

  ecmdChipData cd2;
  cd2.coreData.clear();
  cd2.chipType = "gr";
  cd2.pos = 1;
  slotData.chipData.push_back(cd2);

  cd2.pos = 2;
  slotData.chipData.push_back(cd2);

  cd2.pos = 3;
  slotData.chipData.push_back(cd2);

  slotData.slotId = 0;

  nodeData.slotData.push_front(slotData);

  nodeData.nodeId = 0;

  cageData.cageId = 0;
  cageData.nodeData.push_front(nodeData);

  queryData.cageData.push_front(cageData);
  
  return ECMD_SUCCESS;

} 

int dllQueryRing(ecmdChipTarget & target, std::list<ecmdRingData> & queryData, const char * ringName ){ return ECMD_SUCCESS; }

int dllQueryArray(ecmdChipTarget & target, ecmdArrayData & queryData, const char * arrayName){ return ECMD_SUCCESS; } 

int dllQueryFileLocation(ecmdChipTarget & target, ecmdFileType_t fileType, std::string & fileLocation){ return ECMD_SUCCESS; } 


int dllFlushSys () { return ECMD_SUCCESS; } 

int dllIplSys () { return ECMD_SUCCESS; }


void dllOutputError(const char* message) {
  printf("DLLSTUBERROR : %s\n",message);
}

void dllOutputWarning(const char* message) {
  printf("DLLSTUBWARNING : %s\n",message);
}

void dllOutput(const char* message) {
  printf(message);
}

void dllEnableRingCache() { return ; }

int dllDisableRingCache() { return ECMD_SUCCESS; }

int dllFlushRingCache() { return ECMD_SUCCESS; }

int dllGetArray(ecmdChipTarget & i_target, const char * i_arrayName, ecmdDataBuffer & i_address, ecmdDataBuffer & o_data) { return ECMD_SUCCESS; }

int dllPutArray(ecmdChipTarget & i_target, const char * i_arrayName, ecmdDataBuffer & i_address, ecmdDataBuffer & i_data) { return ECMD_SUCCESS; }



