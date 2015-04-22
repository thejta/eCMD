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
CAPI_SOURCE  := fapi2DllCapi.C plat_utils.C plat_hw_access.C fapi2PlatHwpExecutor.C fapi2PlatSystemConfig.C
CAPI_SPECIAL_SOURCE = tls_storage.C

# Added for fapiTarget.H, etc
CAPI_INCLUDES     :=  fapi2_target.H plat_target.H target.H target_types.H target_states.H \
					 buffer_parameters.H buffer_traits.H \
					 buffer.H variable_buffer.H \
					 fapi2_error_scope.H plat_error_scope.H error_scope.H \
                     return_code.H return_code_defs.H plat_trace.H \
					 fapi2_hw_access.H hw_access.H plat_hw_access.H hw_access_def.H \
					 ffdc.H error_info.H error_info_defs.H collect_reg_ffdc.H \
					 hwp_return_codes.H hwp_error_info.H hwp_ffdc_classes.H set_sbe_error.H \
					 utils.H plat_utils.H array.H \
 				     fapi2SharedUtils.H \
                     fapi2DllCapi.H fapi2ClientEnums.H \
                     fapi2HwpExecutor.H fapi2PlatHwpExecutor.H \
					 fapi2AttributeService.H fapi2PlatAttributeService.H

					 # fapiMBvpdAccess.H fapiMvpdAccess.H
                     # fapiMultiScom.H

# These files are needed by the client code
CAPI_INCLUDES     := ${CAPI_INCLUDES} fapi2.H fapi2AttributeIds.H fapi2AttributeService.C fapi2ChipEcFeature.H fapi2ChipEcFeature.C

# Additional Source files for the Shared Lib
# FIXME add fapi buffer source files
SLIB_SOURCE  := ffdc.C error_info.C array.C

# Include the common extension makefile to do all the work
# include ../../template/capi/makefile.capi

# For this extension we're including a local makefile since it needed to be modified to link to libecmd
 include makefile.capi

