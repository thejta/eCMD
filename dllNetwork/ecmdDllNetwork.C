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



//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <inttypes.h>
#include <stdio.h>

#include <ecmdDllCapi.H>
#include <ecmdStructs.H>
#include <ecmdReturnCodes.H>
#include <ecmdDataBuffer.H>
#include <ecmdSharedUtils.H>

#include <FSIInstruction.H>
#include <OutputLite.H>
#include <Controller.H>

OutputLite out;
Controller * controller = NULL;
//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//  Function Definitions                                               
//--------------------------------------------------------------------
/* For use by dllQueryConfig and dllQueryExist */
uint32_t queryConfigExist(ecmdChipTarget & target, ecmdQueryData & queryData, ecmdQueryDetail_t detail, bool allowDisabled);

//--------------------------------------------------------------------
//  Function Definitions                                               
//--------------------------------------------------------------------
//
//   These are just stubs, used for testing the out the DLL

uint32_t dllInitDll() {
  // initialize controller
  controller = new Controller("127.0.0.1");
  uint32_t rc = controller->initialize();
  return rc;
}

uint32_t dllFreeDll() {
  // destroy controller
  if (controller != NULL)
  {
    delete controller;
  }
  controller = NULL;
  return ECMD_SUCCESS;
}

uint32_t dllSpecificCommandArgs(int* argc, char** argv[]) {
  return ECMD_SUCCESS;
}

/* Dll Specific Return Codes */
std::string dllSpecificParseReturnCode(uint32_t i_returnCode) { return ""; }

uint32_t dllGetScom(ecmdChipTarget & i_target, uint64_t i_address, ecmdDataBuffer & o_data)
{
    uint32_t rc = 0;
    FSIInstruction * scomoutInstruction = new FSIInstruction();
    uint32_t scomlen = 64;
    uint32_t flags = 0x0;

    std::string deviceString = "1";

    scomoutInstruction->setup(Instruction::SCOMOUT, deviceString, i_address, scomlen, flags);
    InstructionStatus resultStatus;

    std::list<Instruction *> instructionList;
    std::list<ecmdDataBuffer *> dataList;
    std::list<InstructionStatus *> statusList;

    statusList.push_back(&resultStatus);
    dataList.push_back(&o_data);
    instructionList.push_back(scomoutInstruction);

    /* --------------------------------------------------- */
    /* Call the server interface with the Instruction and  */
    /* result objects.                                     */
    /* --------------------------------------------------- */
    rc = controller->transfer_send(instructionList, dataList, statusList);
    //uint32_t status = resultStatus.data.getWord(0);
    delete scomoutInstruction;

    if (resultStatus.rc != SERVER_COMMAND_COMPLETE) {
        controller->extractError(resultStatus);
        return out.error(resultStatus.rc, "dllGetScom","Problem calling interface: rc = %d for %s\n", resultStatus.rc, ecmdWriteTarget(i_target,ECMD_DISPLAY_TARGET_HYBRID).c_str());
    }

    return rc;
}

uint32_t dllPutScom(ecmdChipTarget & i_target, uint64_t i_address, ecmdDataBuffer & i_data)
{
    uint32_t rc = 0;
    FSIInstruction * scominInstruction = new FSIInstruction();
    uint32_t scomlen = 64;
    uint32_t flags = 0x0;

    std::string deviceString = "1";

    scominInstruction->setup(Instruction::SCOMIN, deviceString, i_address, scomlen, flags, &i_data);
    InstructionStatus resultStatus;
    ecmdDataBuffer data;

    std::list<Instruction *> instructionList;
    std::list<ecmdDataBuffer *> dataList;
    std::list<InstructionStatus *> statusList;

    statusList.push_back(&resultStatus);
    dataList.push_back(&data);
    instructionList.push_back(scominInstruction);

    /* --------------------------------------------------- */
    /* Call the server interface with the Instruction and  */
    /* result objects.                                     */
    /* --------------------------------------------------- */
    rc = controller->transfer_send(instructionList, dataList, statusList);
    //uint32_t status = resultStatus.data.getWord(0);
    delete scominInstruction;

    if (resultStatus.rc != SERVER_COMMAND_COMPLETE) {
        controller->extractError(resultStatus);
        return out.error(resultStatus.rc, "dllPutScom","Problem calling interface: rc = %d for %s\n", resultStatus.rc, ecmdWriteTarget(i_target,ECMD_DISPLAY_TARGET_HYBRID).c_str());
    }

    return rc;
}

uint32_t dllPutScomUnderMask(ecmdChipTarget & i_target, uint64_t i_address, ecmdDataBuffer & i_data, ecmdDataBuffer & i_mask)
{
    uint32_t rc = 0;
    FSIInstruction * scominInstruction = new FSIInstruction();
    uint32_t scomlen = 64;
    uint32_t flags = 0x0;

    std::string deviceString = "1";

    scominInstruction->setup(Instruction::SCOMIN_MASK, deviceString, i_address, scomlen, flags, &i_data, &i_mask);
    InstructionStatus resultStatus;
    ecmdDataBuffer data;

    std::list<Instruction *> instructionList;
    std::list<ecmdDataBuffer *> dataList;
    std::list<InstructionStatus *> statusList;

    statusList.push_back(&resultStatus);
    dataList.push_back(&data);
    instructionList.push_back(scominInstruction);

    /* --------------------------------------------------- */
    /* Call the server interface with the Instruction and  */
    /* result objects.                                     */
    /* --------------------------------------------------- */
    rc = controller->transfer_send(instructionList, dataList, statusList);
    //uint32_t status = resultStatus.data.getWord(0);
    delete scominInstruction;

    if (resultStatus.rc != SERVER_COMMAND_COMPLETE) {
        controller->extractError(resultStatus);
        return out.error(resultStatus.rc, "dllPutScomUnderMask","Problem calling interface: rc = %d for %s\n", resultStatus.rc, ecmdWriteTarget(i_target,ECMD_DISPLAY_TARGET_HYBRID).c_str());
    }

    return rc;
}

uint32_t dllGetRingSparse(ecmdChipTarget & i_target, const char * i_ringName, ecmdDataBuffer & o_data, ecmdDataBuffer & i_mask, uint32_t i_flags) 
{ 
    return ECMD_SUCCESS; 
} 

uint32_t dllPutRingSparse(ecmdChipTarget & i_target, const char * i_ringName, ecmdDataBuffer & i_data, ecmdDataBuffer & i_mask, uint32_t i_flags) 
{ 
    return ECMD_SUCCESS; 
} 

uint32_t dllGetRing(ecmdChipTarget & i_target, const char * i_ringName, ecmdDataBuffer & o_data, uint32_t i_mode)
{ 
    uint32_t rc = 0;

    if (rc) return rc;
    return rc;
} 

uint32_t dllPutRing(ecmdChipTarget & i_target, const char * i_ringName, ecmdDataBuffer & i_data, uint32_t i_mode) 
{ 
    return ECMD_SUCCESS; 
} 


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

  o_queryData.clear();
  if ((i_ringName == NULL) || (std::string("ring1") == i_ringName))
  {
    ringData.ringNames.push_back("ring1");
    ringData.address = 0x80000001;
    ringData.bitLength = 100;
    ringData.isCheckable = false;
    ringData.clockState = ECMD_CLOCKSTATE_ON;

    o_queryData.push_back(ringData);
  }

  if ((i_ringName == NULL) || (std::string("ring2") == i_ringName))
  {
    ringData.ringNames.clear();
    ringData.ringNames.push_back("ring2");
    ringData.ringNames.push_back("ring2long");
    ringData.address = 0x80000002;
    ringData.bitLength = 2000;
    ringData.isCheckable = true;
    ringData.clockState = ECMD_CLOCKSTATE_NA;

    o_queryData.push_back(ringData);
  }

  return ECMD_SUCCESS;
}

uint32_t dllQueryArray(ecmdChipTarget & target, ecmdArrayData & queryData, const char * arrayName){ return ECMD_SUCCESS; } 

uint32_t dllQueryFileLocation(ecmdChipTarget & i_target, ecmdFileType_t i_fileType, std::string & o_fileLocation, std::string & io_version){ return ECMD_SUCCESS; } 

uint32_t dllQueryScom(ecmdChipTarget & i_target, std::list<ecmdScomData> & o_queryData, uint64_t i_address, ecmdQueryDetail_t i_detail )
{
  uint32_t rc = ECMD_SUCCESS;
  o_queryData.clear();
  ecmdScomData ret;
  o_queryData.push_back(ret);
  o_queryData.back().isChipUnitRelated = false;
  o_queryData.back().endianMode = ECMD_BIG_ENDIAN;
  o_queryData.back().length = 64;

  return rc;
}



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
