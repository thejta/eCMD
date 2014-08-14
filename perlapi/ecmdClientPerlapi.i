%module ecmd
/* $Header$ */
/*********** Start Typemaps ***********/
%include typemaps.i
%include cpointer.i
%include ecmdString.i
%include ecmdVector.i
%include ecmdList.i
%include ecmdCommon.i
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

#include "ecmdPluginExtensionSupport.H"
#ifdef ECMD_AIP_EXTENSION_SUPPORT
  #include "aipClientPerlapi.H"
  #include "aipClientPerlapiFunc.H"
#endif
#ifdef ECMD_BML_EXTENSION_SUPPORT
  #include "bmlClientPerlapi.H"
  #include "bmlClientPerlapiFunc.H"
#endif
#ifdef ECMD_CIP_EXTENSION_SUPPORT
  #include "cipClientPerlapi.H"
  #include "cipClientPerlapiFunc.H"
#endif
#ifdef ECMD_CMD_EXTENSION_SUPPORT
  #include "cmdClientPerlapi.H"
  #include "cmdClientPerlapiFunc.H"
#endif
#ifdef ECMD_CRO_EXTENSION_SUPPORT
  #include "croClientPerlapi.H"
  #include "croClientPerlapiFunc.H"
#endif
#ifdef ECMD_EIP_EXTENSION_SUPPORT
  #include "eipClientPerlapi.H"
  #include "eipClientPerlapiFunc.H"
#endif
#ifdef ECMD_GIP_EXTENSION_SUPPORT
  #include "gipClientPerlapi.H"
  #include "gipClientPerlapiFunc.H"
#endif
#ifdef ECMD_MBO_EXTENSION_SUPPORT
  #include "mboClientPerlapi.H"
  #include "mboClientPerlapiFunc.H"
#endif
#ifdef ECMD_ZSE_EXTENSION_SUPPORT
  #include "zseStructs.H"
  #include "zseClientPerlapi.H"
  #include "zseClientPerlapiFunc.H"
  #include "zseClientPerlapiIterators.H"
#endif
#ifdef ECMD_FAPI_EXTENSION_SUPPORT
  #include "fapiClientPerlapi.H"
#endif
%}
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
%template(listEcmdI2CCmdEntry)       std::list<ecmdI2CCmdEntry>;
%template(listEcmdConnectionData)    std::list<ecmdConnectionData>;
%template(listEcmdScomEntry)         std::list<ecmdScomEntry>;
%template(listUint32_t)              std::list<uint32_t>;
/*********** End Templates ***********/

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

// The extensions
%include "ecmdPluginExtensionSupport.H"
#ifdef ECMD_AIP_EXTENSION_SUPPORT
  %include aipClientPerlapi.i
#endif
#ifdef ECMD_BML_EXTENSION_SUPPORT
  %include bmlClientPerlapi.i
#endif
#ifdef ECMD_CIP_EXTENSION_SUPPORT
  %include cipClientPerlapi.i
#endif
#ifdef ECMD_CMD_EXTENSION_SUPPORT
  %include cmdClientPerlapi.i
#endif
#ifdef ECMD_CRO_EXTENSION_SUPPORT
  %include croClientPerlapi.i
#endif
#ifdef ECMD_EIP_EXTENSION_SUPPORT
  %include eipClientPerlapi.i
#endif
#ifdef ECMD_GIP_EXTENSION_SUPPORT
  %include gipClientPerlapi.i
#endif
#ifdef ECMD_MBO_EXTENSION_SUPPORT
  %include mboClientPerlapi.i
#endif
#ifdef ECMD_ZSE_EXTENSION_SUPPORT
  %include zseClientPerlapi.i
#endif
#ifdef ECMD_FAPI_EXTENSION_SUPPORT
  %include fapiClientPerlapi.i
#endif
/*********** End Files to swigify ***********/

%pragma(perl5) include="ecmdClientPerlapi.pl"
