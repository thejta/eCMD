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
%apply unsigned int { uint32_t }
%apply unsigned char { uint8_t }
%apply unsigned int &REFERENCE { uint32_t & };
/*********** End Applies ***********/

/*********** Start Insert Code ***********/
// Insert C code into the file swig generates
%{
#include "ecmdClientPerlapi.H"
#include "ecmdClientPerlapiFunc.H"
#include "ecmdDataBuffer.H"
#include "ecmdStructs.H"
#include "ecmdClientPerlapiIterators.H"
#include "ecmdUtils.H"
#include "ecmdSharedUtils.H"

#include "ecmdPluginExtensionSupport.H"
#ifdef ECMD_CIP_EXTENSION_SUPPORT
  #include "cipClientPerlapi.H"
  #include "cipClientPerlapiFunc.H"
#endif

%}
/*********** End Insert Code ***********/

/*********** Start Templates ***********/
// Templates for vector support - one of these have to be created for every type needed
// From ecmdStructs.H
%template(listEcmdThreadData)        std::list<ecmdThreadData>;
%template(listEcmdCoreData)          std::list<ecmdCoreData>;
%template(listEcmdchipData)          std::list<ecmdChipData>;
%template(listEcmdSlotData)          std::list<ecmdSlotData>;
%template(listEcmdNodeData)          std::list<ecmdNodeData>;
%template(listEcmdCageData)          std::list<ecmdCageData>;
%template(listString)                std::list<std::string>;
%template(vectorEcmdDataBuffer)      std::vector<ecmdDataBuffer>;
// From ecmdClientPerlapi.H
%template(listEcmdRingData)          std::list<ecmdRingData>;
%template(listEcmdLatchEntry)        std::list<ecmdLatchEntry>;
%template(listEcmdSpyGroupData)      std::list<ecmdSpyGroupData>;
%template(listEcmdArrayEntry)        std::list<ecmdArrayEntry>;
%template(listEcmdNameEntry)         std::list<ecmdNameEntry>;
%template(listEcmdNameVectorEntry)   std::list<ecmdNameVectorEntry>;
%template(listEcmdIndexEntry)        std::list<ecmdIndexEntry>;
%template(listEcmdChipTarget)        std::list<ecmdChipTarget>;
/*********** End Templates ***********/

/*********** Start Renames ***********/
// This has to be done before the files to swigify get pulled in, otherwise they won't be applied
%rename(operatorEqualTo)    operator==(const ecmdDataBuffer& other) const;
%rename(operatorNotEqualTo) operator!=(const ecmdDataBuffer& other) const;
%rename(operatorAnd) operator&(const ecmdDataBuffer& other) const;
%rename(operatorOr) operator|(const ecmdDataBuffer& other) const;
// Pull in the auto generated renames
%include ecmdRename.i
/*********** End Renames ***********/

/*********** Start Files to swigify ***********/
%include ecmdClientPerlapi.H
%include ecmdClientPerlapiFunc.H
%include ecmdDataBuffer.H
%include ecmdStructs.H
%include ecmdClientPerlapiIterators.H
%include ecmdUtils.H
%include ecmdSharedUtils.H

// The extensions
%include ecmdPluginExtensionSupport.H
#ifdef ECMD_CIP_EXTENSION_SUPPORT
  %include cipClientPerlapi.H
  %include cipClientPerlapiFunc.H
#endif
/*********** End Files to swigify ***********/

%exception {
	$function
          if ((ECMDPERLAPI::ecmdPerlInterfaceErrorCheck(-1)) &&
	   (ECMDPERLAPI::ecmdQuerySafeMode() == 1)) {
		croak("ecmdClientPerlapi.i::Error occured in eCMD Perl module - execution halted\n");
	}
}

// This line inserts a straight copy of the file below into the beginning of the swig generated perl module
%pragma(perl5) include="ecmdClientPerlapi.pl"
// Pull in the generated operator code
%pragma(perl5) include="ecmdClientPerlapiIterators.pl"
