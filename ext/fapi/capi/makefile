# $Header$
# Makefile for the ecmd Extensions

EXTENSION_NAME   := fapi
DISABLE_AUTOGEN := @
DEFINES := ${DEFINES}  -DFAPIARCHIVE -DFAPI_SUPPORT_SPY_AS_STRING

# Base info and default build rules
SUBDIR     := ext/${EXTENSION_NAME}/capi/
include ../../../makefile.rules

# Additional Source files for the Client API
CAPI_SOURCE  := fapiDllCapi.C fapiPlatUtil.C fapiHwAccess.C fapiPlatHwAccess.C fapiPlatHwpExecutor.C fapiPlatSystemConfig.C

# Added for fapiTarget.H, etc
CAPI_INCLUDES     :=  fapiSharedUtils.H fapiTarget.H fapiPlatTrace.H fapiReturnCode.H  fapiReturnCodeDataRef.H  \
                     fapiReturnCodes.H fapiDllCapi.H fapiClientEnums.H \
                     fapiUtil.H fapiPlatUtil.H fapiHwAccess.H fapiSystemConfig.H fapiPlatHwAccess.H \
                     fapiErrorInfo.H fapiPlatHwpExecutor.H  fapiHwpReturnCodes.H fapiHwpErrorInfo.H \
                     fapiHwpExecutor.H fapiAttributeService.H fapiPlatAttributeService.H \
		     fapiMBvpdAccess.H fapiMvpdAccess.H fapiCollectRegFfdc.H \

# These files are needed by the client code
CAPI_INCLUDES	  := ${CAPI_INCLUDES} fapi.H fapiAttributeIds.H fapiPlatAttributeService.C fapiAttributeService.C fapiChipEcFeature.H fapiChipEcFeature.C \

# Additional Source files for the Shared Lib
SLIB_SOURCE  := fapiErrorInfo.C fapiTarget.C fapiPlatTarget.C fapiPlatReturnCodeDataRef.C \
 		fapiReturnCode.C fapiReturnCodeDataRef.C fapiSharedUtils.C

# Include the common extension makefile to do all the work
# include ../../template/capi/makefile.capi

# For this extension we're including a local makefile since it needed to be modified to link to libecmd
 include makefile.capi

