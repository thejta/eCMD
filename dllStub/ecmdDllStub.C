// Copyright **********************************************************
//                                                                      
// File ecmdDllStub.C                                               
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

//--------------------------------------------------------------------
//  Function Definitions                                               
//--------------------------------------------------------------------
/* For use by dllQueryConfig and dllQueryExist */
uint32_t queryConfigExist(ecmdChipTarget & target, ecmdQueryData & queryData, ecmdQueryDetail_t detail, bool allowDisabled);

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

uint32_t dllInitDll() {
  /* This is where we would init any local variables to the dll */
  return ECMD_SUCCESS;
}

uint32_t dllFreeDll() {
  return ECMD_SUCCESS;
}

uint32_t dllSpecificCommandArgs(int* argc, char** argv[]) {
  return ECMD_SUCCESS;
}

/* Dll Specific Return Codes */
std::string dllSpecificParseReturnCode(uint32_t i_returnCode) { return ""; }

uint32_t dllGetRing (ecmdChipTarget & target, const char * ringName, ecmdDataBuffer & data) { return ECMD_SUCCESS; }

uint32_t dllPutRing (ecmdChipTarget & target, const char * ringName, ecmdDataBuffer & data) { return ECMD_SUCCESS; }


uint32_t dllGetScom (ecmdChipTarget & target, uint64_t address, ecmdDataBuffer & data) {

  return ECMD_SUCCESS;
}

uint32_t dllPutScom (ecmdChipTarget & target, uint64_t address, ecmdDataBuffer & data) { return ECMD_SUCCESS; }


uint32_t dllGetArray (ecmdChipTarget & target, const char * arrayName, uint32_t * address, ecmdDataBuffer & data) { return ECMD_SUCCESS; }

uint32_t dllPutArray (ecmdChipTarget & target, const char * arrayName, uint32_t * address, ecmdDataBuffer & data) { return ECMD_SUCCESS; }

/* ##################################################################### */
/* Query Functions - Query Functions - Query Functions - Query Functions */
/* ##################################################################### */
uint32_t dllQueryConfig(ecmdChipTarget & i_target, ecmdQueryData & o_queryData, ecmdQueryDetail_t i_detail ) {
  return queryConfigExist(i_target, o_queryData, i_detail, false);
}

uint32_t dllQueryExist(ecmdChipTarget & i_target, ecmdQueryData & o_queryData, ecmdQueryDetail_t i_detail ) {
  return queryConfigExist(i_target, o_queryData, i_detail, true);
}

uint32_t queryConfigExist(ecmdChipTarget & target, ecmdQueryData & queryData, ecmdQueryDetail_t detail, bool allowDisabled) {
  ecmdChipUnitData chipUnitData;
  ecmdChipData chipData;
  ecmdNodeData nodeData;
  ecmdSlotData slotData;
  ecmdCageData cageData;
  ecmdThreadData threadData;

  threadData.threadId = 0;

  /* Let's return some dummy info , we will return a proc with cores and threads */
  chipUnitData.chipUnitType = "core";
  chipUnitData.chipUnitNum = 0;
  chipUnitData.numThreads = 2;
  chipUnitData.threadData.push_front(threadData);

  chipData.chipUnitData.push_front(chipUnitData);
  chipData.chipType = "pu";
  chipData.pos = 0;
  slotData.chipData.push_front(chipData);

  ecmdChipData cd2;
  cd2.chipUnitData.clear();
  cd2.chipType = "pu";
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

uint32_t dllQueryRing(ecmdChipTarget & i_target, std::list<ecmdRingData> & o_queryData, const char * i_ringName, ecmdQueryDetail_t i_detail) {

  ecmdRingData ringData;

  ringData.ringNames.push_back("ring1");
  ringData.address = 0x80000001;
  ringData.bitLength = 100;
  ringData.isCheckable = false;
  ringData.clockState = ECMD_CLOCKSTATE_ON;

  o_queryData.push_back(ringData);

  ringData.ringNames.clear();
  ringData.ringNames.push_back("ring2");
  ringData.ringNames.push_back("ring2long");
  ringData.address = 0x80000002;
  ringData.bitLength = 2000;
  ringData.isCheckable = true;
  ringData.clockState = ECMD_CLOCKSTATE_NA;

  o_queryData.push_back(ringData);

  return ECMD_SUCCESS;
}

uint32_t dllQueryArray(ecmdChipTarget & target, ecmdArrayData & queryData, const char * arrayName){ return ECMD_SUCCESS; } 

uint32_t dllQueryFileLocation(ecmdChipTarget & i_target, ecmdFileType_t i_fileType, std::string & o_fileLocation, std::string & io_version){ return ECMD_SUCCESS; } 


uint32_t dllFlushSys () { return ECMD_SUCCESS; } 

uint32_t dllIplSys () { return ECMD_SUCCESS; }


void dllOutputError(const char* message) {
  printf("DLLSTUBERROR : %s\n",message);
}

void dllOutputWarning(const char* message) {
  printf("DLLSTUBWARNING : %s\n",message);
}

void dllOutput(const char* message) {
  printf("%s", message);
}

uint32_t dllGetChipData(ecmdChipTarget & i_target, ecmdChipData & o_data) { return ECMD_SUCCESS; }

uint32_t dllEnableRingCache(ecmdChipTarget & i_target) { return ECMD_SUCCESS; }

uint32_t dllDisableRingCache(ecmdChipTarget & i_target) { return ECMD_SUCCESS; }

uint32_t dllFlushRingCache(ecmdChipTarget & i_target) { return ECMD_SUCCESS; }

bool dllIsRingCacheEnabled(ecmdChipTarget & i_target) { return false; }

uint32_t dllGetArray(ecmdChipTarget & i_target, const char * i_arrayName, ecmdDataBuffer & i_address, ecmdDataBuffer & o_data) { return ECMD_SUCCESS; }

uint32_t dllPutArray(ecmdChipTarget & i_target, const char * i_arrayName, ecmdDataBuffer & i_address, ecmdDataBuffer & i_data) { return ECMD_SUCCESS; }

/* ################################################################# */
/* Misc Functions - Misc Functions - Misc Functions - Misc Functions */
/* ################################################################# */
uint32_t dllGetScandefOrder(ecmdChipTarget & i_target, uint32_t & o_mode) {
  uint32_t rc = ECMD_SUCCESS;
  ecmdChipData chipData;

  /* Just get the chip data, we get extra data but this has never been a performance problem for us */
  rc = dllGetChipData(i_target, chipData);
  if (rc) return rc;

  o_mode = chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK;

  return rc;
}

void dllSetTraceMode(ecmdTraceType_t i_type, bool i_enable) {
  dllOutputError("Not supported: dllSetTraceMode");
}

bool dllQueryTraceMode(ecmdTraceType_t i_type) {
  return false;
}

uint32_t dllTargetTranslateNormalToFused(ecmdChipTarget & i_target, uint32_t & o_core, uint32_t & o_thread) {
  uint32_t rc = ECMD_SUCCESS;
  return rc;
}

/* ######################################################################### */
/* UnitID Functions - UnitID Functions - UnitID Functions - UnitID Functions */
/* ######################################################################### */
/* Added to fix symbol errors - JTA 06/28/14 */
uint32_t dllTargetToUnitId(ecmdChipTarget & io_target) {
  uint32_t rc = ECMD_SUCCESS;
  return rc;
}

uint32_t dllUnitIdStringToTarget(std::string i_unitId, std::list<ecmdChipTarget> & o_target) {
  uint32_t rc = ECMD_SUCCESS;
  return rc;
}

uint32_t dllUnitIdToTarget(uint32_t i_unitId, std::list<ecmdChipTarget> & o_target) {
  uint32_t rc = ECMD_SUCCESS;
  return rc;
}

uint32_t dllUnitIdToString(uint32_t i_unitId, std::string & o_unitIdStr) {
  uint32_t rc = ECMD_SUCCESS;
  return rc;
}

uint32_t dllSequenceIdToTarget(uint32_t i_core_seq_num, ecmdChipTarget & io_target, uint32_t i_thread_seq_num) {
  uint32_t rc = ECMD_SUCCESS;
  return rc;
}

uint32_t dllTargetToSequenceId(ecmdChipTarget i_target, uint32_t & o_core_seq_num, uint32_t & o_thread_seq_num) {
  uint32_t rc = ECMD_SUCCESS;
  return rc;
}

uint32_t dllGetUnitIdVersion(uint32_t & o_unitIdVersion) {
  uint32_t rc = ECMD_SUCCESS;
  return rc;
}
