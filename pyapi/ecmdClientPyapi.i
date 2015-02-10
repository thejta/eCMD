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

#include "ecmdPluginExtensionSupport.H"
#ifdef ECMD_AIP_EXTENSION_SUPPORT
  #include "aipClientCapi.H"
  #include "aipClientPyapi.H"
#endif
#ifdef ECMD_BML_EXTENSION_SUPPORT
  #include "bmlClientCapi.H"
  #include "bmlClientPyapi.H"
#endif
#ifdef ECMD_CIP_EXTENSION_SUPPORT
  #include "cipClientCapi.H"
  #include "cipClientPyapi.H"
#endif
#ifdef ECMD_CMD_EXTENSION_SUPPORT
  #include "cmdClientCapi.H"
  #include "cmdClientPyapi.H"
#endif
#ifdef ECMD_CRO_EXTENSION_SUPPORT
  #include "croClientCapi.H"
  #include "croClientPyapi.H"
#endif
#ifdef ECMD_EIP_EXTENSION_SUPPORT
  #include "eipClientCapi.H"
  #include "eipClientPyapi.H"
#endif
#ifdef ECMD_GIP_EXTENSION_SUPPORT
  #include "gipClientCapi.H"
  #include "gipClientPyapi.H"
#endif
#ifdef ECMD_MBO_EXTENSION_SUPPORT
  #include "mboClientCapi.H"
  #include "mboClientPyapi.H"
#endif
#ifdef ECMD_ZSE_EXTENSION_SUPPORT
  #include "zseStructs.H"
  #include "zseClientCapi.H"
  #include "zseClientPyapi.H"
#endif
#ifdef ECMD_FAPI_EXTENSION_SUPPORT
  #include "fapiClientPyapi.H"
#endif
#ifdef ECMD_FAPI2_EXTENSION_SUPPORT
  #include "fapi2ClientPyapi.H"
#endif
%}
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

// The extensions
%include "ecmdPluginExtensionSupport.H"
#ifdef ECMD_AIP_EXTENSION_SUPPORT
  %include aipClientPyapi.i
#endif
#ifdef ECMD_BML_EXTENSION_SUPPORT
  %include bmlClientPyapi.i
#endif
#ifdef ECMD_CIP_EXTENSION_SUPPORT
  %include cipClientPyapi.i
#endif
#ifdef ECMD_CMD_EXTENSION_SUPPORT
  %include cmdClientPyapi.i
#endif
#ifdef ECMD_CRO_EXTENSION_SUPPORT
  %include croClientPyapi.i
#endif
#ifdef ECMD_EIP_EXTENSION_SUPPORT
  %include eipClientPyapi.i
#endif
#ifdef ECMD_GIP_EXTENSION_SUPPORT
  %include gipClientPyapi.i
#endif
#ifdef ECMD_MBO_EXTENSION_SUPPORT
  %include mboClientPyapi.i
#endif
#ifdef ECMD_ZSE_EXTENSION_SUPPORT
  %include zseClientPyapi.i
#endif
#ifdef ECMD_FAPI_EXTENSION_SUPPORT
  %include fapiClientPyapi.i
#endif
#ifdef ECMD_FAPI2_EXTENSION_SUPPORT
  %include fapi2ClientPyapi.i
#endif
/*********** End Files to swigify ***********/
