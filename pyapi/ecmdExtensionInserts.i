/*********** Start Insert Code ***********/
// Insert C code into the file swig generates
%{
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

