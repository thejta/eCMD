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
  printf("Stub dll has been initialized\n");
  return ECMD_SUCCESS;
}

int dllFreeDll() {
  printf("Stub dll has been freed\n");
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


int dllGetSpy (ecmdChipTarget & target, const char * spyName, ecmdDataBuffer & data) { return ECMD_SUCCESS; }

int dllGetSpyEnum(ecmdChipTarget & target, const char * spyName, std::string & enumValue){ return ECMD_SUCCESS; }

int getSpyEccGrouping (ecmdChipTarget & i_target, const char * i_spyEccGroupName, ecmdDataBuffer & o_groupData, ecmdDataBuffer & o_eccData, ecmdDataBuffer & o_eccErrorMask) { return ECMD_SUCCESS; }

int dllPutSpy (ecmdChipTarget & target, const char * spyName, ecmdDataBuffer & data) { return ECMD_SUCCESS; }

int dllPutSpyEnum(ecmdChipTarget & target, const char * spyName, const std::string enumValue){ return ECMD_SUCCESS; } 

int dllGetArray (ecmdChipTarget & target, const char * arrayName, uint32_t * address, ecmdDataBuffer & data) { return ECMD_SUCCESS; }

int dllPutArray (ecmdChipTarget & target, const char * arrayName, uint32_t * address, ecmdDataBuffer & data) { return ECMD_SUCCESS; }

int dllQueryDllInfo(ecmdDllInfo & o_dllInfo) {
  o_dllInfo.dllType = ECMD_DLL_STUB;
  return ECMD_SUCCESS;
}

int dllQueryConfig(ecmdChipTarget & target,ecmdQueryData & queryData, ecmdQueryDetail_t i_detail) {

  ecmdCoreData coreData;
  ecmdChipData chipData;
  ecmdNodeData nodeData;
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
  nodeData.chipData.push_front(chipData);

  ecmdChipData cd2;
  cd2.coreData.clear();
  cd2.chipType = "gr";
  cd2.pos = 1;
  nodeData.chipData.push_back(cd2);

  cd2.pos = 2;
  nodeData.chipData.push_back(cd2);

  cd2.pos = 3;
  nodeData.chipData.push_back(cd2);

  nodeData.nodeId = 0;

  cageData.cageId = 0;
  cageData.nodeData.push_front(nodeData);

  queryData.cageData.push_front(cageData);
  
  return ECMD_SUCCESS;

} 

int dllQueryRing(ecmdChipTarget & target, std::list<ecmdRingData> & queryData, const char * ringName ){ return ECMD_SUCCESS; }

int dllQueryArray(ecmdChipTarget & target, std::list<ecmdArrayData> & queryData, const char * arrayName){ return ECMD_SUCCESS; } 

int dllQuerySpy(ecmdChipTarget & target, std::list<ecmdSpyData> & queryData, const char * spyName){ return ECMD_SUCCESS; } 

int dllQueryFileLocation(ecmdChipTarget & target, ecmdFileType_t fileType, std::string fileLocation){ return ECMD_SUCCESS; } 


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



