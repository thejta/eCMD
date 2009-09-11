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
#include "ecmdDataBuffer.H"
#include "ecmdBit64.H"
#include "ecmdStructs.H"
#include "zseStructs.H"
#include "ecmdClientPerlapiIterators.H"
#include "ecmdUtils.H"
#include "ecmdSharedUtils.H"

#include "ecmdPluginExtensionSupport.H"
#ifdef ECMD_ZSE_EXTENSION_SUPPORT
  #include "zseClientPerlapi.H"
  #include "zseClientPerlapiFunc.H"
#endif
%}
/*********** End Insert Code ***********/

/*********** Start Templates ***********/
// Templates for vector support - one of these have to be created for every type needed
// From ecmdStructs.H
%template(listZseI390trcRangeData)   std::list<zseI390trcRangeData>;
%template(listUint32_t)              std::list<uint32_t>;
%template(listEcmdPchidInfo)         std::list<ecmdPchidInfo>;
/*********** End Templates ***********/

/*********** Start Files to swigify ***********/
%include "ecmdDefines.H"
%include "ecmdClientPerlapi.H"
%include "ecmdClientPerlapiFunc.H"
%include "ecmdDataBuffer.H"
%include "ecmdBit64.H"
%include "ecmdStructs.H"
%include "zseStructs.H"
%include "ecmdClientPerlapiIterators.H"
%include "zseClientPerlapiIterators.H"
%include "ecmdUtils.H"
%include "ecmdSharedUtils.H"
%include "ecmdReturnCodes.H"

// The extensions
%include "ecmdPluginExtensionSupport.H"
#ifdef ECMD_ZSE_EXTENSION_SUPPORT
  %include "zseClientPerlapi.H"
  %include "zseClientPerlapiFunc.H"
  %include "zseStructs.H"
  %include "zseReturnCodes.H"
#endif
/*********** End Files to swigify ***********/
