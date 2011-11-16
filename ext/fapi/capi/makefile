# $Header$
# Makefile for the ecmd Extensions

EXTENSION_NAME   := fapi
DISABLE_AUTOGEN := @
DEFINES := ${DEFINES}  -DFAPIARCHIVE

# Base info and default build rules
SUBDIR     := ext/${EXTENSION_NAME}/capi/
include ../../../makefile.rules

# Additional Source files for the Client API
CAPI_SOURCE  := fapiDllCapi.C fapiPlatUtil.C fapiHwAccess.C fapiPlatHwAccess.C fapiPlatHwpExecutor.C fapiPlatSystemConfig.C

# Added for fapiTarget.H, etc
CAPI_INCLUDES     :=  fapiSharedUtils.H fapiTarget.H fapiPlatTrace.H fapiReturnCode.H  fapiReturnCodeDataRef.H  \
                     ../../../capi/ecmdDataBufferBase.H fapiReturnCodes.H fapiDllCapi.H fapiClientEnums.H \
                     fapiUtil.H fapiHwAccess.H fapiSystemConfig.H fapiPlatHwAccess.H \
                     fapiErrorInfo.H fapiPlatHwpExecutor.H  fapiHwpReturnCodes.H \
                     fapiHwpExecutor.H fapiAttributeService.H fapiPlatAttributeService.H \

# These files are needed by the client code
CAPI_INCLUDES	  := ${CAPI_INCLUDES} fapi.H fapiAttributeIds.H fapiPlatAttributeService.C   fapiAttributeService.C \

# Additional Source files for the Shared Lib
SLIB_SOURCE  := fapiErrorInfo.C fapiTarget.C fapiPlatTarget.C fapiPlatReturnCodeDataRef.C \
 		fapiReturnCode.C fapiReturnCodeDataRef.C fapiSharedUtils.C


# Include the common extension makefile to do all the work
 include ../../template/capi/makefile.capi

