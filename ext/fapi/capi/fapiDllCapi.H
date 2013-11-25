#ifndef fapiDllCapi_H
#define fapiDllCapi_H

#include <inttypes.h>
#include <vector>
#include <string>

#include <ecmdDefines.H>
#include <ecmdStructs.H>
#include <ecmdReturnCodes.H>
#include <ecmdDataBufferBase.H>

#include <fapiReturnCode.H>
#include <fapiTarget.H>
#include <fapiStructs.H>
#include <fapiAttributeIds.H>
#include <fapiMvpdAccess.H>
#include <fapiMBvpdAccess.H>
#include <fapiMultiScom.H>

extern "C" {

/* Extension initialization function - verifies version */
uint32_t dllFapiInitExtension(const char * i_version);

/* Extension Specific load function - used by Cronus/GFW to see if extension is supported */
uint32_t dllFapiInitExtensionInPlugin();

fapi::ReturnCode dllFapiGetRing(
	const fapi::Target& i_handle, 
	const uint32_t i_address, 
	ecmdDataBufferBase & o_data, 
	const uint32_t i_ringMode); 

fapi::ReturnCode dllFapiPutRing(
	const fapi::Target& i_handle, 
	const uint32_t i_address, 
	ecmdDataBufferBase & i_data, 
	const uint32_t i_ringMode); 

fapi::ReturnCode dllFapiGetChildChiplets(
	const fapi::Target& i_target, 
	const fapi::TargetType i_chipletType, 
	std::vector<fapi::Target> & o_chiplets, 
	const fapi::TargetState i_state); 

fapi::ReturnCode dllFapiGetParentChip(
	const fapi::Target& i_chiplet, 
	fapi::Target & o_chip); 

fapi::ReturnCode dllFapiGetExistingChiplets(
	const fapi::Target& i_target, 
	const fapi::TargetType i_chipletType, 
	std::vector<fapi::Target> &o_chiplets); 

uint32_t dllHwpInvoker(
	ecmdChipTarget & i_target, 
	const std::string & i_sharedObjectName, 
	const std::string & i_sharedObjectEntryPoint, 
	std::list<uint64_t> &i_sharedObjectArgs);

uint32_t dllFapiQueryFileLocation(
	fapi::FileType_t i_fileType, 
	std::string & i_fileName, 
	std::string & o_fileLocation, 
	std::string i_version);

fapi::ReturnCode dllFapiGetAssociatedDimms(
	const fapi::Target& i_target, 
	std::vector<fapi::Target> & o_dimms, 
	const fapi::TargetState i_state);

uint32_t dllFapiGetAttributeOverride(
	const fapi::Target & i_target, 
	const uint32_t i_id, 
	fapi::AttributeData & o_data);

uint32_t dllFapiGetAttribute(
	const fapi::Target & i_target, 
	const uint32_t i_id, 
	fapi::AttributeData & o_data);

uint32_t dllFapiSetAttribute(
	const fapi::Target & i_target, 
	const uint32_t i_id, 
	fapi::AttributeData & i_data);

uint32_t dllFapiAttributeDataToString(
	fapi::AttributeId i_attrId, 
	const fapi::AttributeData & i_attrData, 
	std::string & o_attrDataString, 
	bool i_fullData, 
	const char * i_format);

uint32_t dllFapiAttributeStringToId(
	std::string i_attrString, 
	fapi::AttributeId & o_attrId);

uint32_t dllFapiGetAttrInfo(
	fapi::AttributeId i_attrId, 
	uint32_t & o_attrType, 
	uint32_t & o_numOfEntries, 
	uint32_t & o_numOfBytes, 
	bool & o_attrEnum);

uint32_t dllFapiGetAttributeIdsByType(
	uint32_t i_targetTypes, 
	uint32_t i_attributeSources, 
	std::list<fapi::AttributeId> & o_attributeIds);

void dllFapiOutputError(const char* i_message); 

void dllFapiOutputInfo(const char* i_message); 

void dllFapiOutputImportant(const char* i_message); 

void dllFapiOutputDebug(const char* i_message); 

void dllFapiOutputScanTrace(const char* i_message); 

uint32_t dllFapiDelay(uint64_t i_nanoSeconds, uint64_t i_simCycles); 

bool dllPlatIsScanTraceEnabled(); 

void dllPlatSetScanTrace(bool i_enable);

void dllFapiLogError(fapi::ReturnCode & io_rc);

fapi::ReturnCode dllFapiLoadInitFile(
	const fapi::Target &, 
	const char * i_file, 
	const char *& o_addr, 
	size_t & o_size);

fapi::ReturnCode dllFapiUnloadInitFile(
	const char * i_file, 
	const char *& io_addr, 
	size_t & io_size);

fapi::ReturnCode dllFapiGetMvpdField(
	const fapi::MvpdRecord i_record,
	const fapi::MvpdKeyword i_keyword,
	const fapi::Target &i_procTarget,
	uint8_t * const i_pBuffer,
	uint32_t &io_fieldSize);

fapi::ReturnCode dllFapiSetMvpdField(
	const fapi::MvpdRecord i_record,
	const fapi::MvpdKeyword i_keyword,
	const fapi::Target &i_procTarget,
	const uint8_t * const i_pBuffer,
	const uint32_t i_fieldSize);

fapi::ReturnCode dllFapiSpecialWakeup(
	const fapi::Target & i_target, 
	const bool i_enable);

fapi::ReturnCode dllFapiGetOtherSideOfMemChannel(
	const fapi::Target& i_target, 
	fapi::Target & o_target, 
	const fapi::TargetState i_state);

fapi::ReturnCode dllFapiGetMBvpdField(
	const fapi::MBvpdRecord i_record,
	const fapi::MBvpdKeyword i_keyword,
	const fapi::Target &i_memBufTarget,
	uint8_t * const i_pBuffer,
	uint32_t &io_fieldSize);

fapi::ReturnCode dllFapiSetMBvpdField(
	const fapi::MBvpdRecord i_record,
	const fapi::MBvpdKeyword i_keyword,
	const fapi::Target &i_memBufTarget,
	const uint8_t * const i_pBuffer,
	const uint32_t i_fieldSize);

#ifdef FAPI_SUPPORT_MULTI_SCOM
fapi::ReturnCode dllFapiMultiScom(const fapi::Target & i_target, fapi::MultiScom & io_multiScomList);
#endif

} //extern C

#endif
