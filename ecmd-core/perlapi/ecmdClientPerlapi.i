%module ecmd
/*********** Start Typemaps ***********/
%include typemaps.i
%include cpointer.i
%include ecmdString.i
%include ecmdVector.i
%include ecmdList.i
%include ecmdCommon.i
%include ecmdConst.i
%include std_map.i
/*********** End Typemaps ***********/

/*********** Start Applies ***********/
// These are used to map C types that swig doesn't understand to types swig does understand
%apply unsigned char { uint8_t };
%apply unsigned char &REFERENCE { uint8_t & };
%apply unsigned char *REFERENCE { uint8_t * };
%apply unsigned short { uint16_t };
%apply unsigned short &REFERENCE { uint16_t & };
%apply unsigned short *REFERENCE { uint16_t * };
%apply unsigned int { uint32_t };
%apply unsigned int &REFERENCE { uint32_t & };
%apply unsigned int *REFERENCE { uint32_t * };
%apply int { int32_t };
%apply int &REFERENCE { int32_t & };
%apply int *REFERENCE { int32_t * };
%apply ecmdBit64 { uint64_t };
%apply ecmdBit64 &REFERENCE { uint64_t & };
%apply ecmdBit64 *REFERENCE { uint64_t * };
%apply std::string &REFERENCE { std::string & };
%apply std::string *REFERENCE { std::string * };
%apply enum SWIGTYPE &REFERENCE { enum SWIGTYPE& };
/*********** End Applies ***********/

/*********** Start Insert Code ***********/
// Insert C code into the file swig generates
%{
#include "ecmdDefines.H"
#include "ecmdClientPerlapi.H"
#include "ecmdClientPerlapiFunc.H"
#include "ecmdDataBufferBase.H"
#include "ecmdDataBuffer.H"
#include "ecmdBit64.H"
#include "ecmdStructs.H"
#include "ecmdClientPerlapiIterators.H"
#include "ecmdUtils.H"
#include "ecmdSharedUtils.H"

%}
%include "ecmdExtPerlInserts.i"
/*********** End Insert Code ***********/

/*********** Start Templates ***********/
// Templates for vector support - one of these have to be created for every type needed
// From ecmdStructs.H
%template(listEcmdThreadData)        std::list<ecmdThreadData>;
%template(listEcmdChipUnitData)      std::list<ecmdChipUnitData>;
%template(listEcmdChipData)          std::list<ecmdChipData>;
%template(listEcmdSlotData)          std::list<ecmdSlotData>;
%template(listEcmdNodeData)          std::list<ecmdNodeData>;
%template(listEcmdCageData)          std::list<ecmdCageData>;
%template(listString)                std::list<std::string>;
%template(vectorEcmdDataBufferBase)  std::vector<ecmdDataBufferBase>;
%template(vectorEcmdDataBuffer)      std::vector<ecmdDataBuffer>;
%template(listEcmdMemoryEntry)       std::list<ecmdMemoryEntry>;
%template(listEcmdRingData)          std::list<ecmdRingData>;
%template(listEcmdLatchData)         std::list<ecmdLatchData>;
%template(listEcmdLatchEntry)        std::list<ecmdLatchEntry>;
%template(listEcmdSpyGroupData)      std::list<ecmdSpyGroupData>;
%template(listEcmdArrayEntry)        std::list<ecmdArrayEntry>;
%template(listEcmdNameEntry)         std::list<ecmdNameEntry>;
%template(listEcmdNameVectorEntry)   std::list<ecmdNameVectorEntry>;
%template(listEcmdIndexEntry)        std::list<ecmdIndexEntry>;
%template(listEcmdChipTarget)        std::list<ecmdChipTarget>;
%template(listEcmdArrayData)         std::list<ecmdArrayData>;
%template(listEcmdTraceArrayData)    std::list<ecmdTraceArrayData>;
%template(listEcmdSpyData)           std::list<ecmdSpyData>;
%template(listEcmdSpyLatchData)      std::list<ecmdSpyLatchData>;
%template(listEcmdScomData)          std::list<ecmdScomData>;
%template(listEcmdScomDataHidden)    std::list<ecmdScomDataHidden>;
%template(listEcmdFileLocation)      std::list<ecmdFileLocation>;
%template(listEcmdI2CCmdEntry)       std::list<ecmdI2CCmdEntry>;
%template(listEcmdI2CCmdEntryHidden) std::list<ecmdI2CCmdEntryHidden>;
%template(listEcmdConnectionData)    std::list<ecmdConnectionData>;
%template(listEcmdScomEntry)         std::list<ecmdScomEntry>;
%template(listUint32_t)              std::list<uint32_t>;
%template(vectorString)              std::vector<std::string>;
// Template for maps
%template(string_ecmdDataBufferMap)  std::map<std::string, ecmdDataBuffer>;
/*********** End Templates ***********/

/*********** Start Copy Constructors ***********/
%copyctor ecmdChipTarget;
%copyctor ecmdThreadData;
%copyctor ecmdChipUnitData;
%copyctor ecmdChipData;
%copyctor ecmdSlotData;
%copyctor ecmdNodeData;
%copyctor ecmdCageData;
%copyctor ecmdQueryData;
%copyctor ecmdRingData;
%copyctor ecmdArrayData;
%copyctor ecmdTraceArrayData;
%copyctor ecmdFastArrayData;
%copyctor ecmdScomData;
%copyctor ecmdScomDataHidden;
%copyctor ecmdLataData;
%copyctor ecmdScomEntry;
%copyctor ecmdArrayEntry;
%copyctor ecmdNameEntry;
%copyctor ecmdNameVectorEntry;
%copyctor ecmdIndexVectorEntry;
%copyctor ecmdIndexEntry;
%copyctor ecmdLatchEntry;
%copyctor ecmdLatchQueryData;
%copyctor ecmdLatchQueryDataHidden;
%copyctor ecmdProcRegisterInfo;
%copyctor ecmdCacheData;
%copyctor ecmdSpyData;
%copyctor ecmdI2CCmdEntry;
%copyctor ecmdI2CCmdEntryHidden;
%copyctor ecmdSimModelInfo;
%copyctor ecmdConnectionData;
%copyctor ecmdPnorListEntryData;
%copyctor ecmdPnorListData;
%copyctor ecmdLooperData;
/*********** End Copy Constructors ***********/

/*********** Start Files to swigify ***********/
%include "ecmdDefines.H"
%include "ecmdClientPerlapi.H"
%include "ecmdClientPerlapiFunc.H"
%include "ecmdDataBufferBase.H"
%include "ecmdDataBuffer.H"
%include "ecmdBit64.H"
%include "ecmdStructs.H"
%include "ecmdClientPerlapiIterators.H"
%include "ecmdUtils.H"
%include "ecmdSharedUtils.H"
%include "ecmdReturnCodes.H"

%include "ecmdExtPerlIncludes.i"
/*********** End Files to swigify ***********/

%pragma(perl5) include="ecmdClientPerlapi.pl"
