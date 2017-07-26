
%module ecmd

// python helper to load _ecmd.so correctly for fapi
%include dlopenhelper.i

/*********** Start Typemaps ***********/
%include typemaps.i
%include std_string.i
%include std_list.i
%include std_vector.i
%include stdint.i
%include ecmdCommon.i
%include ecmdConst.i
%apply uint32_t &OUTPUT { uint32_t & };
//typemap for ecmdClockFreqMode_t input value passed by reference
%typemap(in) ecmdClockFreqMode_t & (ecmdClockFreqMode_t temp) { temp = (ecmdClockFreqMode_t) PyInt_AsLong($input); $1 = &temp; }
/*********** End Typemaps ***********/

/*********** Start Applies ***********/
// These are used to map C types that swig doesn't understand to types swig does understand
/*********** End Applies ***********/

/*********** Start Insert Code ***********/
// Insert C code into the file swig generates
%{
#define SWIG_FILE_WITH_INIT
#include "ecmdDefines.H"
#include "ecmdClientCapi.H"
#include "ecmdClientPyapi.H"
#include "ecmdDataBufferBase.H"
#include "ecmdDataBuffer.H"
#include "ecmdStructs.H"
#include "ecmdUtils.H"
#include "ecmdSharedUtils.H"
// Header file needed to compile with newer gcc
#include <stddef.h>

%}
%include "ecmdExtPyInserts.i"
/*********** End Insert Code ***********/

/*********** Start Templates ***********/
// Templates for list/vector support - one of these have to be created for every type needed
// From ecmdStructs.H
%template(ecmdThreadDataList)        std::list<ecmdThreadData>;
%template(ecmdChipUnitDataList)      std::list<ecmdChipUnitData>;
%template(ecmdChipDataList)          std::list<ecmdChipData>;
%template(ecmdSlotDataList)          std::list<ecmdSlotData>;
%template(ecmdNodeDataList)          std::list<ecmdNodeData>;
%template(ecmdCageDataList)          std::list<ecmdCageData>;
%template(stringList)                std::list<std::string>;
%template(ecmdDataBufferBaseVector)  std::vector<ecmdDataBufferBase>;
%template(ecmdDataBufferVector)      std::vector<ecmdDataBuffer>;
%template(ecmdMemoryEntryList)       std::list<ecmdMemoryEntry>;
%template(ecmdRingDataList)          std::list<ecmdRingData>;
%template(ecmdLatchDataList)         std::list<ecmdLatchData>;
%template(ecmdLatchEntryList)        std::list<ecmdLatchEntry>;
%template(ecmdSpyGroupDataList)      std::list<ecmdSpyGroupData>;
%template(ecmdArrayEntryList)        std::list<ecmdArrayEntry>;
%template(ecmdNameEntryList)         std::list<ecmdNameEntry>;
%template(ecmdNameVectorEntryList)   std::list<ecmdNameVectorEntry>;
%template(ecmdIndexEntryList)        std::list<ecmdIndexEntry>;
%template(ecmdChipTargetList)        std::list<ecmdChipTarget>;
%template(ecmdArrayDataList)         std::list<ecmdArrayData>;
%template(ecmdTraceArrayDataList)    std::list<ecmdTraceArrayData>;
%template(ecmdSpyDataList)           std::list<ecmdSpyData>;
%template(ecmdSpyLatchDataList)      std::list<ecmdSpyLatchData>;
%template(ecmdScomDataList)          std::list<ecmdScomData>;
%template(ecmdI2CCmdEntryList)       std::list<ecmdI2CCmdEntry>;
%template(ecmdConnectionDataList)    std::list<ecmdConnectionData>;
%template(ecmdScomEntryList)         std::list<ecmdScomEntry>;
%template(uint32_tList)              std::list<uint32_t>;
/*********** End Templates ***********/

/*********** Start Files to swigify ***********/
%include "ecmdDefines.H"
%include "ecmdStructs.H"
%include "ecmdUtils.H"
%include "ecmdSharedUtils.H"
%include "ecmdReturnCodes.H"
%include "ecmdDataBufferBase.H"
%include "ecmdDataBuffer.H"
%include "ecmdClientCapi.H"
%include "ecmdClientPyapi.H"

%include "ecmdExtPyIncludes.i"
/*********** End Files to swigify ***********/
