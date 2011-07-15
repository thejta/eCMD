# $Header$
# Makefile for the ecmd Extensions

EXTENSION_NAME   := fapi
DISABLE_AUTOGEN := @

# Base info and default build rules
SUBDIR     := ext/${EXTENSION_NAME}/capi/
include ../../../makefile.rules

# Additional Source files for the Client API
CAPI_SOURCE  := fapiDllCapi.C fapiPlatHwAccess.C fapiPlatHwpExecutor.C fapiPlatSystemConfig.C

#  added for fapiTarget.H, etc
CAPI_INCLUDES     :=  fapiSharedUtils.H fapiTarget.H fapiPlatTrace.H fapiReturnCode.H  fapiReturnCodeDataRef.H  \
                     ../../../capi/ecmdDataBufferBase.H fapiReturnCodes.H fapiDllCapi.H fapiClientEnums.H \
                     fapiUtil.H fapiHwAccess.H fapiSystemConfig.H \
                     fapi.H fapiPlatHwpExecutor.H 

# Additional Source files for the Shared Lib
SLIB_SOURCE  := fapiTarget.C fapiPlatTarget.C fapiPlatUtil.C fapiPlatReturnCodeDataRef.C \
 		fapiReturnCode.C fapiReturnCodeDataRef.C fapiSharedUtils.C


# Include the common extension makefile to do all the work
 include ../../template/capi/makefile.capi

