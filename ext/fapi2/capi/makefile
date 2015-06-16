# $Header$
# Makefile for the ecmd Extensions

EXTENSION_NAME   := fapi2
DISABLE_AUTOGEN := @
DEFINES := ${DEFINES}  -DFAPIARCHIVE -DFAPI_SUPPORT_SPY_AS_STRING -DFAPI_2
#DEFINES := ${DEFINES}  -DFAPIARCHIVE -DFAPI_SUPPORT_SPY_AS_STRING -DFAPI_SUPPORT_MULTI_SCOM -DFAPI_2

# Base info and default build rules
SUBDIR := ext/${EXTENSION_NAME}/capi/
include ../../../makefile.rules

# Additional Source files for the Client API
CAPI_SOURCE := fapi2DllCapi.C
CAPI_SOURCE += plat_utils.C
CAPI_SOURCE += plat_hw_access.C
CAPI_SOURCE += hwp_executor.C
CAPI_SOURCE += fapi2PlatSystemConfig.C
CAPI_SPECIAL_SOURCE = tls_storage.C

# Added for fapi2 source files
CAPI_INCLUDES := fapi2_target.H
CAPI_INCLUDES += plat_target.H
CAPI_INCLUDES += target.H
CAPI_INCLUDES += target_types.H
CAPI_INCLUDES += target_states.H
CAPI_INCLUDES += buffer_parameters.H
CAPI_INCLUDES += buffer_traits.H
CAPI_INCLUDES += buffer.H
CAPI_INCLUDES += variable_buffer.H
CAPI_INCLUDES += fapi2_error_scope.H
CAPI_INCLUDES += plat_error_scope.H
CAPI_INCLUDES += error_scope.H
CAPI_INCLUDES += plat_trace.H
CAPI_INCLUDES += fapi2_hw_access.H
CAPI_INCLUDES += hw_access.H
CAPI_INCLUDES += plat_hw_access.H
CAPI_INCLUDES += hw_access_def.H
CAPI_INCLUDES += mvpd_access.H
CAPI_INCLUDES += ffdc.H
CAPI_INCLUDES += error_info.H
CAPI_INCLUDES += error_info_defs.H
CAPI_INCLUDES += collect_reg_ffdc.H
CAPI_INCLUDES += return_code.H
CAPI_INCLUDES += return_code_defs.H
CAPI_INCLUDES += hwp_return_codes.H
CAPI_INCLUDES += hwp_error_info.H
CAPI_INCLUDES += hwp_ffdc_classes.H
CAPI_INCLUDES += set_sbe_error.H
CAPI_INCLUDES += utils.H
CAPI_INCLUDES += plat_utils.H
CAPI_INCLUDES += array.H
CAPI_INCLUDES += fapi2SharedUtils.H
CAPI_INCLUDES += fapi2DllCapi.H
CAPI_INCLUDES += fapi2ClientEnums.H
CAPI_INCLUDES += fapi2_hwp_executor.H
CAPI_INCLUDES += hwp_executor.H
CAPI_INCLUDES += fapi2_attribute_service.H
CAPI_INCLUDES += plat_attribute_service.H

# fapiMBvpdAccess.H fapiMvpdAccess.H
# fapiMultiScom.H

# These files are needed by the client code
CAPI_INCLUDES += fapi2.H
CAPI_INCLUDES += attribute_ids.H
CAPI_INCLUDES += fapi2_attribute_service.C
CAPI_INCLUDES += fapi2_chip_ec_feature.H
CAPI_INCLUDES += fapi2_chip_ec_feature.C

# Additional Source files for the Shared Lib
SLIB_SOURCE := ffdc.C
SLIB_SOURCE += error_info.C
SLIB_SOURCE += array.C

# Include the common extension makefile to do all the work
# include ../../template/capi/makefile.capi

# For this extension we're including a local makefile since it needed to be modified to link to libecmd
 include makefile.capi

