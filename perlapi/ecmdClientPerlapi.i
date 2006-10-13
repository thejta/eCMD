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
#include "ecmdClientPerlapi.H"
#include "ecmdClientPerlapiFunc.H"
#include "ecmdDataBuffer.H"
#include "ecmdBit64.H"
#include "ecmdStructs.H"
#include "ecmdClientPerlapiIterators.H"
#include "ecmdUtils.H"
#include "ecmdSharedUtils.H"

#include "ecmdPluginExtensionSupport.H"
#ifdef ECMD_CIP_EXTENSION_SUPPORT
  #include "cipClientPerlapi.H"
  #include "cipClientPerlapiFunc.H"
#endif
#ifdef ECMD_GIP_EXTENSION_SUPPORT
  #include "gipClientPerlapi.H"
  #include "gipClientPerlapiFunc.H"
#endif
#ifdef ECMD_CRO_EXTENSION_SUPPORT
  #include "croClientPerlapi.H"
  #include "croClientPerlapiFunc.H"
#endif
#ifdef ECMD_ZSE_EXTENSION_SUPPORT
  #include "zseClientPerlapi.H"
  #include "zseClientPerlapiFunc.H"
#endif
#ifdef ECMD_BML_EXTENSION_SUPPORT
  #include "bmlClientPerlapi.H"
  #include "bmlClientPerlapiFunc.H"
#endif
#ifdef ECMD_EIP_EXTENSION_SUPPORT
  #include "eipClientPerlapi.H"
  #include "eipClientPerlapiFunc.H"
#endif
#include "cmdClientPerlapi.H"
#include "cmdClientPerlapiFunc.H"

%}
/*********** End Insert Code ***********/

/*********** Start Templates ***********/
// Templates for vector support - one of these have to be created for every type needed
// From ecmdStructs.H
%template(listEcmdThreadData)        std::list<ecmdThreadData>;
%template(listEcmdCoreData)          std::list<ecmdCoreData>;
%template(listEcmdChipData)          std::list<ecmdChipData>;
%template(listEcmdSlotData)          std::list<ecmdSlotData>;
%template(listEcmdNodeData)          std::list<ecmdNodeData>;
%template(listEcmdCageData)          std::list<ecmdCageData>;
%template(listString)                std::list<std::string>;
%template(vectorEcmdDataBuffer)      std::vector<ecmdDataBuffer>;
%template(listEcmdMemoryEntry)       std::list<ecmdMemoryEntry>;
// From ecmdClientPerlapi.H
%template(listEcmdRingData)          std::list<ecmdRingData>;
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
/*********** End Templates ***********/

/*********** Start Renames ***********/
// This has to be done before the files to swigify get pulled in, otherwise they won't be applied
// This covers a bunch of standard operators
%rename(operatorIncrement)  operator++(int);
%rename(operatorDecrement)  operator--(int);
%rename(operatorLeftShift)  operator<<(int) const;
%rename(operatorRightShift) operator>>(int) const;
%rename(operatorNot)        operator!() const;
%rename(operatorBitNot)     operator~() const;

// ecmdDataBuffer
%rename(operatorEqualTo)    operator==(const ecmdDataBuffer& other) const;
%rename(operatorNotEqualTo) operator!=(const ecmdDataBuffer& other) const;
%rename(operatorAnd)        operator&(const ecmdDataBuffer& other) const;
%rename(operatorOr)         operator|(const ecmdDataBuffer& other) const;
%rename(operatorOr)         operator|(const uint32_t) const;
// ecmdBit64
%rename(operatorEqualTo)    operator==(const ecmdBit64& other) const;
%rename(operatorNotEqualTo) operator!=(const ecmdBit64& other) const;
%rename(operatorLessThan)   operator<(const ecmdBit64& other) const;
%rename(operatorLessEqualThan)    operator<=(const ecmdBit64& other) const;
%rename(operatorGreaterThan)      operator>(const ecmdBit64& other) const;
%rename(operatorGreaterEqualThan) operator>=(const ecmdBit64& other) const;
%rename(operatorPlus)       operator+(const ecmdBit64& other) const;
%rename(operatorMinus)      operator-(const ecmdBit64& other) const;
%rename(operatorMultiply)   operator*(const ecmdBit64& other) const;
%rename(operatorDivide)     operator/(const ecmdBit64& other) const;
%rename(operatorMod)        operator%(const ecmdBit64& other) const;
%rename(operatorAnd)        operator&(const ecmdBit64& other) const;
%rename(operatorOr)         operator|(const ecmdBit64& other) const;
%rename(operatorXor)        operator^(const ecmdBit64& other) const;
%rename(operatorPlus)       operator+(uint32_t) const;
%rename(operatorMinus)      operator-(uint32_t) const;
%rename(operatorMultiply)   operator*(uint32_t) const;
%rename(operatorDivide)     operator/(uint32_t) const;
%rename(operatorMod)        operator%(uint32_t) const;
%rename(operatorEqualTo)    operator==(uint32_t) const;
%rename(operatorNotEqualTo) operator!=(uint32_t) const;
%rename(operatorLessThan)   operator<(uint32_t) const;
%rename(operatorLessEqualThan)    operator<=(uint32_t) const;
%rename(operatorGreaterThan)      operator>(uint32_t) const;
%rename(operatorGreaterEqualThan) operator>=(uint32_t) const;

// Pull in the auto generated renames
%include ecmdRename.i
/*********** End Renames ***********/

/*********** Start Files to swigify ***********/
%include "ecmdClientPerlapi.H"
%include "ecmdClientPerlapiFunc.H"
%include "ecmdDataBuffer.H"
%include "ecmdBit64.H"
%include "ecmdStructs.H"
%include "ecmdClientPerlapiIterators.H"
%include "ecmdUtils.H"
%include "ecmdSharedUtils.H"
%include "ecmdReturnCodes.H"

// The extensions
%include "ecmdPluginExtensionSupport.H"
#ifdef ECMD_CIP_EXTENSION_SUPPORT
  %include "cipClientPerlapi.H"
  %include "cipClientPerlapiFunc.H"
  %include "cipStructs.H"
#endif
#ifdef ECMD_GIP_EXTENSION_SUPPORT
  %include "gipClientPerlapi.H"
  %include "gipClientPerlapiFunc.H"
  %include "gipStructs.H"
#endif
#ifdef ECMD_CRO_EXTENSION_SUPPORT
  %include "croClientPerlapi.H"
  %include "croClientPerlapiFunc.H"
  %include "croStructs.H"
#endif
#ifdef ECMD_ZSE_EXTENSION_SUPPORT
  %include "zseClientPerlapi.H"
  %include "zseClientPerlapiFunc.H"
  %include "zseStructs.H"
#endif
#ifdef ECMD_BML_EXTENSION_SUPPORT
  %include "bmlClientPerlapi.H"
  %include "bmlClientPerlapiFunc.H"
  %include "bmlStructs.H"
#endif
#ifdef ECMD_EIP_EXTENSION_SUPPORT
  %include "eipClientPerlapi.H"
  %include "eipClientPerlapiFunc.H"
  %include "eipStructs.H"
#endif
%include "cmdClientPerlapi.H"
%include "cmdClientPerlapiFunc.H"
/*********** End Files to swigify ***********/

/* Removing this from v6.1 on - JTA 12/08/05
%exception {
	$function
          if ((ECMDPERLAPI::ecmdPerlInterfaceErrorCheck(-1)) &&
	   (ECMDPERLAPI::ecmdQuerySafeMode() == true)) {
		croak("ecmdClientPerlapi.i::Error occured in eCMD Perl module - execution halted\n");
	}
}
*/

// This line inserts a straight copy of the file below into the beginning of the swig generated perl module
%pragma(perl5) include="ecmdClientPerlapi.pl"
// Pull in the generated operator code
%pragma(perl5) include="ecmdClientPerlapiIterators.pl"
