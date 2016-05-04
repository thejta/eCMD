
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
/*********** End Typemaps ***********/

/*********** Start Applies ***********/
// These are used to map C types that swig doesn't understand to types swig does understand
/*********** End Applies ***********/

/********** Start swig directives ***********/
// This is required to build using swig > 3.0.2 because of this change:
//----
//2014-10-28: vadz
//            [Python] Patch #201 The generated .py file no longer uses *args for all Python parameters.
//            Instead, the parameters are named using the C++ parameter names.
//
//            "compactdefaultargs" feature can be enabled to restore the old behaviour.
//----
// Without this feature enabled, required enums are generated in the .py after
// they are required by function definitions, causing the module to not load.
%feature("compactdefaultargs");
/********** End swig directives ***********/

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
%include "ecmdClientCapi.H"
%include "ecmdClientPyapi.H"
%include "ecmdDataBufferBase.H"
%include "ecmdDataBuffer.H"
%include "ecmdStructs.H"
%include "ecmdUtils.H"
%include "ecmdSharedUtils.H"
%include "ecmdReturnCodes.H"

%include "ecmdExtPyIncludes.i"
/*********** End Files to swigify ***********/
