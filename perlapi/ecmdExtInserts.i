/*********** Start Insert Code ***********/
// Insert C code into the file swig generates
%{
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
#ifdef ECMD_FAPI_EXTENSION_SUPPORT
  #include "fapiClientPerlapi.H"
#endif
#ifdef ECMD_GIP_EXTENSION_SUPPORT
  #include "gipClientPerlapi.H"
  #include "gipClientPerlapiFunc.H"
#endif
#ifdef ECMD_MBO_EXTENSION_SUPPORT
  #include "mboClientPerlapi.H"
  #include "mboClientPerlapiFunc.H"
#endif
