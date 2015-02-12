# $Header$
# Makefile for the ecmd Extensions

EXTENSION_NAME   := fapi2
DISABLE_AUTOGEN := @
DEFINES := ${DEFINES}  -DFAPIARCHIVE -DFAPI_SUPPORT_SPY_AS_STRING -DFAPI_2
#DEFINES := ${DEFINES}  -DFAPIARCHIVE -DFAPI_SUPPORT_SPY_AS_STRING -DFAPI_SUPPORT_MULTI_SCOM -DFAPI_2

# Base info and default build rules
SUBDIR     := ext/${EXTENSION_NAME}/capi/
include ../../../makefile.rules

# Additional Source files for the Client API
CAPI_SOURCE  := fapi2DllCapi.C fapi2PlatUtil.C plat_hw_access.C fapi2PlatHwpExecutor.C fapi2PlatSystemConfig.C
# fapiLiteHwAccess.C fapiLitePlatHwAccess.C fapiLiteUtil.C fapiLitePlatUtil.C fapi2HwAccess.C  fapiMultiScom.C
CAPI_SPECIAL_SOURCE = tls_storage.C

# Added for fapiTarget.H, etc
CAPI_INCLUDES     :=  target.H target_types.H target_states.H \
					 buffer_base.H buffer_parameters.H buffer_traits.H \
					 buffer.H variable_buffer.H error_scope.H \
                     return_code.H \
					 hw_access.H plat_hw_access.H hw_access_structs.H \
 				     fapi2SharedUtils.H fapi2PlatTrace.H \
                     fapi2DllCapi.H fapi2ClientEnums.H \
                     fapi2Util.H fapi2PlatUtil.H \
                     fapi2HwpExecutor.H fapi2PlatHwpExecutor.H \
					 fapi2AttributeService.H fapi2PlatAttributeService.H

					 # fapi2Macro.H fapi2PlatMacro.H fapi2SystemConfig.H
					 # fapiCollectRegFfdc.H fapiMBvpdAccess.H fapiMvpdAccess.H
                     # fapiErrorInfo.H fapiMultiScom.H fapiSetSbeError.H
                     # fapi2HwpReturnCodes.H fapi2HwpErrorInfo.H

					 #fapiLiteUtil.H fapiLitePlatUtil.H fapiLiteHwAccess.H fapiLitePlatHwAccess.H fapiTarget.H 
					 # fapiReturnCode.H fapiReturnCodeDataRef.H fapiReturnCodes.H fapi2HwAccess.H 

# These files are needed by the client code
CAPI_INCLUDES     := ${CAPI_INCLUDES} fapi2.H fapi2AttributeIds.H fapi2AttributeService.C fapi2ChipEcFeature.H fapi2ChipEcFeature.C \

# Additional Source files for the Shared Lib
# FIXME add fapi buffer source files
SLIB_SOURCE  := fapi2SharedUtils.C variable_buffer.C
# fapiPlatTarget.C fapiPlatReturnCodeDataRef.C fapiErrorInfo.C \
#fapiTarget.C fapiReturnCode.C fapiReturnCodeDataRef.C buffer_impl.C

# Include the common extension makefile to do all the work
# include ../../template/capi/makefile.capi

# For this extension we're including a local makefile since it needed to be modified to link to libecmd
 include makefile.capi

