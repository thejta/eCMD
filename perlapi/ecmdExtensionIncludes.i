/*********** Start Files to swigify ***********/
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
#ifdef ECMD_FAPI2_EXTENSION_SUPPORT
  %include fapi2ClientPerlapi.i
#endif
/*********** End Files to swigify ***********/
