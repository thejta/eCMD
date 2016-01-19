/*********** Start Files to swigify ***********/
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
