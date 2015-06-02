/**
 *  @file fapiPlatAttributeService.H
 *
 *  @brief Defines the PLAT attribute access macros and defines which macro
 *         handles each attribute.
 *
 *  Note that platform code must provide the code.
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 *                          mjjones     06/27/2011  Created.
 */

#ifndef FAPI2PLATATTRIBUTESERVICE_H_
#define FAPI2PLATATTRIBUTESERVICE_H_
#include <stdint.h>
#include <stdio.h>
#include <return_code.H>
#include <attribute_ids.H>
#include <fapi2ClientCapi.H>
#include <fapi2Structs.H>
#include <fapi2SharedUtils.H>

#include <ecmdReturnCodes.H>
#include <ecmdStructs.H>

#define PLAT_GET_CHIP_EC_FEATURE_OVERRIDE(ID, TARGET, VAL) \
      _getEcFeatureOverride<ID##_Type>(ID, TARGET, VAL)

/* INSERT NEW ATTRIBUTES HERE */

#define ATTR_SIGNED_TEST_32_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SIGNED_TEST_64_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SIGNED_TEST_8_GETMACRO PLAT_ATTR_GET_GLOBAL_INT

#define ATTR_ALL_MCS_IN_INTERLEAVING_GROUP_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_BOOT_FREQ_MHZ_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_BOOT_FREQ_MHZ_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_BOOT_VOLTAGE_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_BOOT_VOLTAGE_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_DISABLE_I2C_ACCESS_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_EXECUTION_PLATFORM_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_EX_GARD_BITS_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_EX_GARD_BITS_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_IS_MPIPL_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_IS_MPIPL_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_IS_SIMULATION_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_MEMB_DDR_REFCLOCK_RCVR_TERM_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_MEMB_DMI_REFCLOCK_RCVR_TERM_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_MEM_FILTER_PLL_SOURCE_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_MFG_TRACE_ENABLE_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_MNFG_FLAGS_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_MULTI_SCOM_BUFFER_MAX_SIZE_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_NEST_FREQ_MHZ_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_NEST_FREQ_MHZ_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_PCI_REFCLOCK_RCVR_TERM_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_PIB_I2C_NEST_PLL_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_PIB_I2C_NEST_PLL_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_PIB_I2C_REFCLOCK_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_PIB_I2C_REFCLOCK_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_PM_HWP_ATTR_VERSION_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_PM_HWP_ATTR_VERSION_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_PROC_EPS_TABLE_TYPE_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_PROC_FABRIC_PUMP_MODE_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_PROC_REFCLOCK_RCVR_TERM_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_PROC_X_BUS_WIDTH_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_RECONFIGURE_LOOP_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_RECONFIGURE_LOOP_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_REDUNDANT_CLOCKS_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_RISK_LEVEL_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SBE_IMAGE_OFFSET_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SBE_IMAGE_OFFSET_SETMACRO PLAT_ATTR_SET_GLOBAL_INT

#define ATTR_CHIP_UNIT_POS_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_DUMMY_SCRATCH_PLAT_INIT_UINT8_GETMACRO PLAT_ATTR_GET_UINT8_3D_ARRAY
#define ATTR_DUMMY_SCRATCH_PLAT_INIT_UINT8_SETMACRO PLAT_ATTR_SET_UINT8_3D_ARRAY
#define ATTR_FUNCTIONAL_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_POS_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SCRATCH_INT32_1_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SCRATCH_INT32_1_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_SCRATCH_INT32_2_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SCRATCH_INT32_2_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_SCRATCH_INT32_ARRAY_1_GETMACRO PLAT_ATTR_GET_INT32_1D_ARRAY
#define ATTR_SCRATCH_INT32_ARRAY_1_SETMACRO PLAT_ATTR_SET_INT32_1D_ARRAY
#define ATTR_SCRATCH_INT32_ARRAY_2_GETMACRO PLAT_ATTR_GET_INT32_2D_ARRAY
#define ATTR_SCRATCH_INT32_ARRAY_2_SETMACRO PLAT_ATTR_SET_INT32_2D_ARRAY
#define ATTR_SCRATCH_INT64_1_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SCRATCH_INT64_1_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_SCRATCH_INT64_2_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SCRATCH_INT64_2_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_SCRATCH_INT64_ARRAY_1_GETMACRO PLAT_ATTR_GET_INT64_1D_ARRAY
#define ATTR_SCRATCH_INT64_ARRAY_1_SETMACRO PLAT_ATTR_SET_INT64_1D_ARRAY
#define ATTR_SCRATCH_INT64_ARRAY_2_GETMACRO PLAT_ATTR_GET_INT64_2D_ARRAY
#define ATTR_SCRATCH_INT64_ARRAY_2_SETMACRO PLAT_ATTR_SET_INT64_2D_ARRAY
#define ATTR_SCRATCH_INT8_1_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SCRATCH_INT8_1_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_SCRATCH_INT8_2_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SCRATCH_INT8_2_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_SCRATCH_INT8_ARRAY_1_GETMACRO PLAT_ATTR_GET_INT8_1D_ARRAY
#define ATTR_SCRATCH_INT8_ARRAY_1_SETMACRO PLAT_ATTR_SET_INT8_1D_ARRAY
#define ATTR_SCRATCH_INT8_ARRAY_2_GETMACRO PLAT_ATTR_GET_INT8_3D_ARRAY
#define ATTR_SCRATCH_INT8_ARRAY_2_SETMACRO PLAT_ATTR_SET_INT8_3D_ARRAY
#define ATTR_SCRATCH_UINT32_1_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SCRATCH_UINT32_1_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_SCRATCH_UINT32_2_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SCRATCH_UINT32_2_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_SCRATCH_UINT32_ARRAY_1_GETMACRO PLAT_ATTR_GET_UINT32_1D_ARRAY
#define ATTR_SCRATCH_UINT32_ARRAY_1_SETMACRO PLAT_ATTR_SET_UINT32_1D_ARRAY
#define ATTR_SCRATCH_UINT32_ARRAY_2_GETMACRO PLAT_ATTR_GET_UINT32_2D_ARRAY
#define ATTR_SCRATCH_UINT32_ARRAY_2_SETMACRO PLAT_ATTR_SET_UINT32_2D_ARRAY
#define ATTR_SCRATCH_UINT64_1_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SCRATCH_UINT64_1_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_SCRATCH_UINT64_2_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SCRATCH_UINT64_2_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_SCRATCH_UINT64_ARRAY_1_GETMACRO PLAT_ATTR_GET_UINT64_1D_ARRAY
#define ATTR_SCRATCH_UINT64_ARRAY_1_SETMACRO PLAT_ATTR_SET_UINT64_1D_ARRAY
#define ATTR_SCRATCH_UINT64_ARRAY_2_GETMACRO PLAT_ATTR_GET_UINT64_2D_ARRAY
#define ATTR_SCRATCH_UINT64_ARRAY_2_SETMACRO PLAT_ATTR_SET_UINT64_2D_ARRAY
#define ATTR_SCRATCH_UINT8_1_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SCRATCH_UINT8_1_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_SCRATCH_UINT8_2_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_SCRATCH_UINT8_2_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_SCRATCH_UINT8_ARRAY_1_GETMACRO PLAT_ATTR_GET_UINT8_1D_ARRAY
#define ATTR_SCRATCH_UINT8_ARRAY_1_SETMACRO PLAT_ATTR_SET_UINT8_1D_ARRAY
#define ATTR_SCRATCH_UINT8_ARRAY_2_GETMACRO PLAT_ATTR_GET_UINT8_3D_ARRAY
#define ATTR_SCRATCH_UINT8_ARRAY_2_SETMACRO PLAT_ATTR_SET_UINT8_3D_ARRAY

#define ATTR_CHIP_HAS_SBE_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_CHIP_ID_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_CHIP_REGIONS_TO_ENABLE_GETMACRO PLAT_ATTR_GET_UINT64_1D_ARRAY
#define ATTR_EC_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_EX_L2_SINGLE_MEMBER_ENABLE_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_FABRIC_CHIP_ID_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_FABRIC_NODE_ID_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_FSI_GP_REG_SCOM_ACCESS_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_I2C_SLAVE_ADDRESS_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_NAME_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_OSCSWITCH_CTL0_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_OSCSWITCH_CTL1_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_OSCSWITCH_CTL2_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_PCI_OSCSWITCH_CONFIG_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_PROC_BOOT_VOLTAGE_VID_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_PROC_BOOT_VOLTAGE_VID_SETMACRO PLAT_ATTR_SET_GLOBAL_INT
#define ATTR_PROC_DCM_INSTALLED_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_TARGET_SCOMABLE_GETMACRO PLAT_ATTR_GET_GLOBAL_INT
#define ATTR_TARGET_SCOMABLE_SETMACRO PLAT_ATTR_SET_GLOBAL_INT


/******************************************************************************/
/*                          * Global macros *                                 */
/* These macros are called by the macros above to invoke the appropriate API. */
/* These macros do not need to change when a new attribute is introduced.     */
/******************************************************************************/

/* global get uint8_t 1D array macro */
#define PLAT_ATTR_GET_UINT8_1D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArrayShort(ID, TARGET, VAL)
/* global set uint8_t 1D array macro */
#define PLAT_ATTR_SET_UINT8_1D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArrayShort(ID, TARGET, VAL)

/* global get uint8_t 2D array macro */
#define PLAT_ATTR_GET_UINT8_2D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArrayShort(ID, TARGET, VAL[0])
/* global set uint8_t 2D array macro */
#define PLAT_ATTR_SET_UINT8_2D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArrayShort(ID, TARGET, VAL[0])

/* global get uint8_t 3D array macro */
#define PLAT_ATTR_GET_UINT8_3D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArrayShort(ID, TARGET, VAL[0][0])
/* global set uint8_t 3D array macro */
#define PLAT_ATTR_SET_UINT8_3D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArrayShort(ID, TARGET, VAL[0][0])

/* global get uint8_t 4D array macro */
#define PLAT_ATTR_GET_UINT8_4D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArrayShort(ID, TARGET, VAL[0][0][0])
/* global set uint8_t 4D array macro */
#define PLAT_ATTR_SET_UINT8_4D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArrayShort(ID, TARGET, VAL[0][0][0])




/* global get uint32_t 1D array macro */
#define PLAT_ATTR_GET_UINT32_1D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArrayWord(ID, TARGET, VAL)
/* global set uint32_t 1D array macro */
#define PLAT_ATTR_SET_UINT32_1D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArrayWord(ID, TARGET, VAL)

/* global get uint32_t 2D array macro */
#define PLAT_ATTR_GET_UINT32_2D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArrayWord(ID, TARGET, VAL[0])
/* global set uint32_t 2D array macro */
#define PLAT_ATTR_SET_UINT32_2D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArrayWord(ID, TARGET, VAL[0])

/* global get uint32_t 3D array macro */
#define PLAT_ATTR_GET_UINT32_3D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArrayWord(ID, TARGET, VAL[0][0])
/* global set uint32_t 3D array macro */
#define PLAT_ATTR_SET_UINT32_3D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArrayWord(ID, TARGET, VAL[0][0])

/* global get uint32_t 4D array macro */
#define PLAT_ATTR_GET_UINT32_4D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArrayWord(ID, TARGET, VAL[0][0][0])
/* global set uint32_t 4D array macro */
#define PLAT_ATTR_SET_UINT32_4D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArrayWord(ID, TARGET, VAL[0][0][0])


/* global get uint64_t 1D array macro */
#define PLAT_ATTR_GET_UINT64_1D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArrayDoubleWord(ID, TARGET, VAL)
/* global set uint64_t 1D array macro */
#define PLAT_ATTR_SET_UINT64_1D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArrayDoubleWord(ID, TARGET, VAL)

/* global get uint64_t 2D array macro */
#define PLAT_ATTR_GET_UINT64_2D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArrayDoubleWord(ID, TARGET, VAL[0])
/* global set uint64_t 2D array macro */
#define PLAT_ATTR_SET_UINT64_2D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArrayDoubleWord(ID, TARGET, VAL[0])

/* global get uint64_t 3D array macro */
#define PLAT_ATTR_GET_UINT64_3D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArrayDoubleWord(ID, TARGET, VAL[0][0])
/* global set uint64_t 3D array macro */
#define PLAT_ATTR_SET_UINT64_3D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArrayDoubleWord(ID, TARGET, VAL[0][0])

/* global get uint64_t 4D array macro */
#define PLAT_ATTR_GET_UINT64_4D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArrayDoubleWord(ID, TARGET, VAL[0][0][0])
/* global set uint64_t 4D array macro */
#define PLAT_ATTR_SET_UINT64_4D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArrayDoubleWord(ID, TARGET, VAL[0][0][0])

/* global get int macro (uint8_t, 32 and 64) */
#define PLAT_ATTR_GET_GLOBAL_INT(ID, TARGET, VAL) \
     _get<ID##_Type>(ID, TARGET, VAL)

/* global set int macro (uint8_t, 32 and 64) */
#define PLAT_ATTR_SET_GLOBAL_INT(ID, TARGET, VAL) \
     _set<ID##_Type>(ID, TARGET, VAL)

/******************************************************************************/
// Get Override Macros
/******************************************************************************/
/* global get override uint8_t 1D array macro */
#define PLAT_ATTR_GET_OVERRIDE_UINT8_1D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArrayShort(ID, TARGET, VAL)
/* global get override uint8_t 2D array macro */
#define PLAT_ATTR_GET_OVERRIDE_UINT8_2D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArrayShort(ID, TARGET, VAL[0])
/* global get override uint8_t 3D array macro */
#define PLAT_ATTR_GET_OVERRIDE_UINT8_3D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArrayShort(ID, TARGET, VAL[0][0])
/* global get override uint8_t 4D array macro */
#define PLAT_ATTR_GET_OVERRIDE_UINT8_4D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArrayShort(ID, TARGET, VAL[0][0][0])


/* global get override uint32_t 1D array macro */
#define PLAT_ATTR_GET_OVERRIDE_UINT32_1D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArrayWord(ID, TARGET, VAL)
/* global get override uint32_t 2D array macro */
#define PLAT_ATTR_GET_OVERRIDE_UINT32_2D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArrayWord(ID, TARGET, VAL[0])
/* global get override uint32_t 3D array macro */
#define PLAT_ATTR_GET_OVERRIDE_UINT32_3D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArrayWord(ID, TARGET, VAL[0][0])
/* global get override uint32_t 4D array macro */
#define PLAT_ATTR_GET_OVERRIDE_UINT32_4D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArrayWord(ID, TARGET, VAL[0][0][0])


/* global get override uint64_t 1D array macro */
#define PLAT_ATTR_GET_OVERRIDE_UINT64_1D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArrayDoubleWord(ID, TARGET, VAL)
/* global get override uint64_t 2D array macro */
#define PLAT_ATTR_GET_OVERRIDE_UINT64_2D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArrayDoubleWord(ID, TARGET, VAL[0])
/* global get override uint64_t 3D array macro */
#define PLAT_ATTR_GET_OVERRIDE_UINT64_3D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArrayDoubleWord(ID, TARGET, VAL[0][0])
/* global get override uint64_t 4D array macro */
#define PLAT_ATTR_GET_OVERRIDE_UINT64_4D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArrayDoubleWord(ID, TARGET, VAL[0][0][0])

/* global get override int macro (uint8_t, 32 and 64) */
#define PLAT_ATTR_GET_OVERRIDE_GLOBAL_INT(ID, TARGET, VAL) \
     _getOverride<ID##_Type>(ID, TARGET, VAL)

/* global get uint8_t 1D array macro */
#define PLAT_ATTR_GET_INT8_1D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArraySignedShort(ID, TARGET, VAL)
/* global set uint8_t 1D array macro */
#define PLAT_ATTR_SET_INT8_1D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArraySignedShort(ID, TARGET, VAL)

/* global get uint8_t 2D array macro */
#define PLAT_ATTR_GET_INT8_2D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArraySignedShort(ID, TARGET, VAL[0])
/* global set uint8_t 2D array macro */
#define PLAT_ATTR_SET_INT8_2D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArraySignedShort(ID, TARGET, VAL[0])

/* global get uint8_t 3D array macro */
#define PLAT_ATTR_GET_INT8_3D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArraySignedShort(ID, TARGET, VAL[0][0])
/* global set uint8_t 3D array macro */
#define PLAT_ATTR_SET_INT8_3D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArraySignedShort(ID, TARGET, VAL[0][0])

/* global get uint8_t 4D array macro */
#define PLAT_ATTR_GET_INT8_4D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArraySignedShort(ID, TARGET, VAL[0][0][0])
/* global set uint8_t 4D array macro */
#define PLAT_ATTR_SET_INT8_4D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArraySignedShort(ID, TARGET, VAL[0][0][0])




/* global get uint32_t 1D array macro */
#define PLAT_ATTR_GET_INT32_1D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArraySignedWord(ID, TARGET, VAL)
/* global set uint32_t 1D array macro */
#define PLAT_ATTR_SET_INT32_1D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArraySignedWord(ID, TARGET, VAL)

/* global get uint32_t 2D array macro */
#define PLAT_ATTR_GET_INT32_2D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArraySignedWord(ID, TARGET, VAL[0])
/* global set uint32_t 2D array macro */
#define PLAT_ATTR_SET_INT32_2D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArraySignedWord(ID, TARGET, VAL[0])

/* global get uint32_t 3D array macro */
#define PLAT_ATTR_GET_INT32_3D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArraySignedWord(ID, TARGET, VAL[0][0])
/* global set uint32_t 3D array macro */
#define PLAT_ATTR_SET_INT32_3D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArraySignedWord(ID, TARGET, VAL[0][0])

/* global get uint32_t 4D array macro */
#define PLAT_ATTR_GET_INT32_4D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArraySignedWord(ID, TARGET, VAL[0][0][0])
/* global set uint32_t 4D array macro */
#define PLAT_ATTR_SET_INT32_4D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArraySignedWord(ID, TARGET, VAL[0][0][0])


/* global get uint64_t 1D array macro */
#define PLAT_ATTR_GET_INT64_1D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArraySignedDoubleWord(ID, TARGET, VAL)
/* global set uint64_t 1D array macro */
#define PLAT_ATTR_SET_INT64_1D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArraySignedDoubleWord(ID, TARGET, VAL)

/* global get uint64_t 2D array macro */
#define PLAT_ATTR_GET_INT64_2D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArraySignedDoubleWord(ID, TARGET, VAL[0])
/* global set uint64_t 2D array macro */
#define PLAT_ATTR_SET_INT64_2D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArraySignedDoubleWord(ID, TARGET, VAL[0])

/* global get uint64_t 3D array macro */
#define PLAT_ATTR_GET_INT64_3D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArraySignedDoubleWord(ID, TARGET, VAL[0][0])
/* global set uint64_t 3D array macro */
#define PLAT_ATTR_SET_INT64_3D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArraySignedDoubleWord(ID, TARGET, VAL[0][0])

/* global get uint64_t 4D array macro */
#define PLAT_ATTR_GET_INT64_4D_ARRAY(ID, TARGET, VAL) \
     _getAttributeArraySignedDoubleWord(ID, TARGET, VAL[0][0][0])
/* global set uint64_t 4D array macro */
#define PLAT_ATTR_SET_INT64_4D_ARRAY(ID, TARGET, VAL) \
     _setAttributeArraySignedDoubleWord(ID, TARGET, VAL[0][0][0])

/* global get int macro (uint8_t, 32 and 64) */
#define PLAT_ATTR_GET_GLOBAL_INT(ID, TARGET, VAL) \
     _get<ID##_Type>(ID, TARGET, VAL)

/* global set int macro (uint8_t, 32 and 64) */
#define PLAT_ATTR_SET_GLOBAL_INT(ID, TARGET, VAL) \
     _set<ID##_Type>(ID, TARGET, VAL)

/******************************************************************************/
// Get Override Macros
/******************************************************************************/
/* global get override uint8_t 1D array macro */
#define PLAT_ATTR_GET_OVERRIDE_INT8_1D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArraySignedShort(ID, TARGET, VAL)
/* global get override uint8_t 2D array macro */
#define PLAT_ATTR_GET_OVERRIDE_INT8_2D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArraySignedShort(ID, TARGET, VAL[0])
/* global get override uint8_t 3D array macro */
#define PLAT_ATTR_GET_OVERRIDE_INT8_3D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArraySignedShort(ID, TARGET, VAL[0][0])
/* global get override uint8_t 4D array macro */
#define PLAT_ATTR_GET_OVERRIDE_INT8_4D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArraySignedShort(ID, TARGET, VAL[0][0][0])


/* global get override uint32_t 1D array macro */
#define PLAT_ATTR_GET_OVERRIDE_INT32_1D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArraySignedWord(ID, TARGET, VAL)
/* global get override uint32_t 2D array macro */
#define PLAT_ATTR_GET_OVERRIDE_INT32_2D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArraySignedWord(ID, TARGET, VAL[0])
/* global get override uint32_t 3D array macro */
#define PLAT_ATTR_GET_OVERRIDE_INT32_3D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArraySignedWord(ID, TARGET, VAL[0][0])
/* global get override uint32_t 4D array macro */
#define PLAT_ATTR_GET_OVERRIDE_INT32_4D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArraySignedWord(ID, TARGET, VAL[0][0][0])


/* global get override uint64_t 1D array macro */
#define PLAT_ATTR_GET_OVERRIDE_INT64_1D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArraySignedDoubleWord(ID, TARGET, VAL)
/* global get override uint64_t 2D array macro */
#define PLAT_ATTR_GET_OVERRIDE_INT64_2D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArraySignedDoubleWord(ID, TARGET, VAL[0])
/* global get override uint64_t 3D array macro */
#define PLAT_ATTR_GET_OVERRIDE_INT64_3D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArraySignedDoubleWord(ID, TARGET, VAL[0][0])
/* global get override uint64_t 4D array macro */
#define PLAT_ATTR_GET_OVERRIDE_INT64_4D_ARRAY(ID, TARGET, VAL) \
     _getAttributeOverrideArraySignedDoubleWord(ID, TARGET, VAL[0][0][0])

namespace fapi2 {

//******************************************************************************
// Get uint8_t 
//******************************************************************************
template<typename T, TargetType K>
ReturnCode _get(const AttributeId i_id,
                const Target<K>& i_target,
                uint8_t& o_value)
{
    static_assert(std::is_same<T, uint8_t>::value, "Attribute type mismatch");

    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT8;
    l_ecmd_rc = fapi2GetAttribute(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    else 
    {
        o_value = o_data.faUint8;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Get uint32_t 
//******************************************************************************
template<typename T, TargetType K>
ReturnCode _get(const AttributeId i_id,
                const Target<K>& i_target,
                uint32_t& o_value)
{
    static_assert(std::is_same<T, uint32_t>::value, "Attribute type mismatch");

    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS;

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT32;
    l_ecmd_rc = fapi2GetAttribute(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    { 
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    } 
    else 
    {
        o_value = o_data.faUint32;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Get uint64_t 
//******************************************************************************
template<typename T, TargetType K>
ReturnCode _get(const AttributeId i_id,
                const Target<K>& i_target,
                uint64_t& o_value)
{
    static_assert(std::is_same<T, uint64_t>::value, "Attribute type mismatch");

    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS;

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT64;
    l_ecmd_rc = fapi2GetAttribute(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    else
    {
        o_value = o_data.faUint64;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Get Override uint8_t 
//******************************************************************************
template<typename T, TargetType K>
ReturnCode _getOverride(const AttributeId i_id,
                        const Target<K>& i_target,
                        uint8_t& o_value)
{
    static_assert(std::is_same<T, uint8_t>::value, "Attribute type mismatch");

    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT8;
    l_ecmd_rc = fapi2GetAttributeOverride(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    else 
    {
        o_value = o_data.faUint8;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Get Override uint32_t 
//******************************************************************************
template<typename T, TargetType K>
ReturnCode _getOverride(const AttributeId i_id,
                        const Target<K>& i_target,
                        uint32_t& o_value)
{
    static_assert(std::is_same<T, uint32_t>::value, "Attribute type mismatch");

    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS;

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT32;
    l_ecmd_rc = fapi2GetAttributeOverride(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    { 
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    } 
    else 
    {
        o_value = o_data.faUint32;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Get Override uint64_t 
//******************************************************************************
template<typename T, TargetType K>
ReturnCode _getOverride(const AttributeId i_id,
                        const Target<K>& i_target,
                        uint64_t& o_value)
{
    static_assert(std::is_same<T, uint64_t>::value, "Attribute type mismatch");

    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS;

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT64;
    l_ecmd_rc = fapi2GetAttributeOverride(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    else
    {
        o_value = o_data.faUint64;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Get override EC Feature (uint8_t)
//******************************************************************************
template<typename T, TargetType K>
ReturnCode _getEcFeatureOverride(const AttributeId i_id,
                                 const Target<K>& i_target,
                                 uint8_t& o_value)
{
    static_assert(std::is_same<T, uint8_t>::value, "Attribute type mismatch");

    // The way this is implemented, we want to return a non-zero return code if we found an override.  
    // Return 0 if there was an error.
    // This is how it's implemented:
    // PLAT_GET_CHIP_EC_FEATURE_OVERRIDE(ID, TARGET, VAL) ? fapi::FAPI_RC_SUCCESS : fapi::fapiQueryChipEcFeature(fapi::ID, TARGET, VAL)

    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT8;
    l_ecmd_rc = fapi2GetAttributeOverride(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    {    
        l_fapi_rc = (ReturnCodes) ECMD_SUCCESS;
        // Should then call fapiQueryChipEcFeature to get value in calling macros
    }
    else 
    {
        o_value = o_data.faUint8;
        l_fapi_rc = (ReturnCodes) ECMD_INVALID_CONFIG_NAME; // translates to FAPI_RC_SUCCESS in calling macros
    }
    return l_fapi_rc;
}


//******************************************************************************
// Get uint8_t array
//******************************************************************************
template<TargetType K>
ReturnCode _getAttributeArrayShort(const AttributeId i_id,
                                   const Target<K>& i_target,
                                   uint8_t * o_pValues)
{
    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS;

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT8ARY;
    o_data.faUint8ary = o_pValues;

    l_ecmd_rc = fapi2GetAttribute(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}

//******************************************************************************
// Set uint8_t array
//******************************************************************************
template<TargetType K>
ReturnCode _setAttributeArrayShort(const AttributeId i_id,
                                   const Target<K>& i_target,
                                   uint8_t * i_pValues)
{
    fapi2::AttributeData i_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    i_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT8ARY;
    i_data.faUint8ary = i_pValues;

    l_ecmd_rc = fapi2SetAttribute(l_ecmd_target, i_id, i_data);
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}

//******************************************************************************
// Get uint32_t array
//******************************************************************************
template<TargetType K>
ReturnCode _getAttributeArrayWord(const AttributeId i_id,
                                  const Target<K>& i_target,
                                  uint32_t * o_pValues)
{
    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT32ARY;
    o_data.faUint32ary = o_pValues;

    l_ecmd_rc = fapi2GetAttribute(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}

//******************************************************************************
// Set uint32_t array
//******************************************************************************
template<TargetType K>
ReturnCode _setAttributeArrayWord(const AttributeId i_id,
                                  const Target<K>& i_target,
                                  uint32_t * i_pValues)
{
    fapi2::AttributeData i_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    i_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT32ARY;
    i_data.faUint32ary = i_pValues;

    l_ecmd_rc = fapi2SetAttribute(l_ecmd_target, i_id, i_data);
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}

//******************************************************************************
// Get uint64_t array
//******************************************************************************
template<TargetType K>
ReturnCode _getAttributeArrayDoubleWord(const AttributeId i_id,
                                        const Target<K>& i_target,
                                        uint64_t * o_pValues)
{
    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT64ARY;
    o_data.faUint64ary = o_pValues;

    l_ecmd_rc = fapi2GetAttribute(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}

//******************************************************************************
// Set uint64_t array
//******************************************************************************
template<TargetType K>
ReturnCode _setAttributeArrayDoubleWord(const AttributeId i_id,
                                        const Target<K>& i_target,
                                        uint64_t * i_pValues)
{
    fapi2::AttributeData i_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    i_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT64ARY;
    i_data.faUint64ary = i_pValues;

    l_ecmd_rc = fapi2SetAttribute(l_ecmd_target, i_id, i_data);
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}

//******************************************************************************
// Get Override uint8_t array
//******************************************************************************
template<TargetType K>
ReturnCode _getAttributeOverrideArrayShort(const AttributeId i_id,
                                           const Target<K>& i_target,
                                           uint8_t * o_pValues)
{
    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS;

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT8ARY;
    o_data.faUint8ary = o_pValues;

    l_ecmd_rc = fapi2GetAttributeOverride(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}

//******************************************************************************
// Get Override uint32_t array
//******************************************************************************
template<TargetType K>
ReturnCode _getAttributeOverrideArrayWord(const AttributeId i_id,
                                          const Target<K>& i_target,
                                          uint32_t * o_pValues)
{
    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT32ARY;
    o_data.faUint32ary = o_pValues;

    l_ecmd_rc = fapi2GetAttributeOverride(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}

//******************************************************************************
// Get Override uint64_t array
//******************************************************************************
template<TargetType K>
ReturnCode _getAttributeOverrideArrayDoubleWord(const AttributeId i_id,
                                                const Target<K>& i_target,
                                                uint64_t * o_pValues)
{
    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT64ARY;
    o_data.faUint64ary = o_pValues;

    l_ecmd_rc = fapi2GetAttributeOverride(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}

//******************************************************************************
// Set uint8_t 
//******************************************************************************
template<typename T, TargetType K>
ReturnCode _set(const AttributeId i_id,
                const Target<K>& i_target,
                uint8_t& i_value)
{
    static_assert(std::is_same<T, uint8_t>::value, "Attribute type mismatch"); // May need to remove

    fapi2::AttributeData i_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    i_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT8;
    i_data.faUint8 = i_value;

    l_ecmd_rc = fapi2SetAttribute(l_ecmd_target, i_id, i_data);
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Set uint32_t 
//******************************************************************************
template<typename T, TargetType K>
ReturnCode _set(const AttributeId i_id,
                const Target<K>& i_target,
                uint32_t& i_value)
{
    static_assert(std::is_same<T, uint32_t>::value, "Attribute type mismatch"); // May need to remove

    fapi2::AttributeData i_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    i_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT32;
    i_data.faUint32 = i_value;

    l_ecmd_rc = fapi2SetAttribute(l_ecmd_target, i_id, i_data);
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Set uint64_t 
//******************************************************************************
template<typename T, TargetType K>
ReturnCode _set(const AttributeId i_id,
                const Target<K>& i_target,
                const uint64_t & i_value)
{
    static_assert(std::is_same<T, uint64_t>::value, "Attribute type mismatch"); // May need to remove

    fapi2::AttributeData i_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    i_data.faValidMask = FAPI_ATTRIBUTE_TYPE_UINT64;
    i_data.faUint64 = i_value;

    l_ecmd_rc = fapi2SetAttribute(l_ecmd_target, i_id, i_data);
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Get int8_t 
//******************************************************************************
template<typename T, TargetType K>
ReturnCode _get(const AttributeId i_id,
                const Target<K>& i_target,
                int8_t& o_value)
{
    static_assert(std::is_same<T, int8_t>::value, "Attribute type mismatch");

    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_INT8;
    l_ecmd_rc = fapi2GetAttribute(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    else 
    {
        o_value = o_data.faInt8;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Get int32_t 
//******************************************************************************
template<typename T, TargetType K>
ReturnCode _get(const AttributeId i_id,
                const Target<K>& i_target,
                int32_t& o_value)
{
    static_assert(std::is_same<T, int32_t>::value, "Attribute type mismatch");

    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS;

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_INT32;
    l_ecmd_rc = fapi2GetAttribute(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    { 
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    } 
    else 
    {
        o_value = o_data.faInt32;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Get int64_t 
//******************************************************************************
template<typename T, TargetType K>
ReturnCode _get(const AttributeId i_id,
                const Target<K>& i_target,
                int64_t& o_value)
{
    static_assert(std::is_same<T, int64_t>::value, "Attribute type mismatch");

    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS;

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_INT64;
    l_ecmd_rc = fapi2GetAttribute(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    else
    {
        o_value = o_data.faInt64;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Get Override int8_t 
//******************************************************************************
template<typename T, TargetType K>
ReturnCode _getOverride(const AttributeId i_id,
                        const Target<K>& i_target,
                        int8_t& o_value)
{
    static_assert(std::is_same<T, int8_t>::value, "Attribute type mismatch");

    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_INT8;
    l_ecmd_rc = fapi2GetAttributeOverride(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    else 
    {
        o_value = o_data.faInt8;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Get Override int32_t 
//******************************************************************************
template<typename T, TargetType K>
ReturnCode _getOverride(const AttributeId i_id,
                        const Target<K>& i_target,
                        int32_t& o_value)
{
    static_assert(std::is_same<T, int32_t>::value, "Attribute type mismatch");

    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS;

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_INT32;
    l_ecmd_rc = fapi2GetAttributeOverride(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    { 
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    } 
    else 
    {
        o_value = o_data.faInt32;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Get Override int64_t 
//******************************************************************************
template<typename T, TargetType K>
ReturnCode _getOverride(const AttributeId i_id,
                        const Target<K>& i_target,
                        int64_t& o_value)
{
    static_assert(std::is_same<T, int64_t>::value, "Attribute type mismatch");

    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS;

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_INT64;
    l_ecmd_rc = fapi2GetAttributeOverride(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    else
    {
        o_value = o_data.faInt64;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Get int8_t array
//******************************************************************************
template<TargetType K>
ReturnCode _getAttributeArraySignedShort(const AttributeId i_id,
                                         const Target<K>& i_target,
                                         int8_t * o_pValues)
{
    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS;

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_INT8ARY;
    o_data.faInt8ary = o_pValues;

    l_ecmd_rc = fapi2GetAttribute(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Set int8_t array
//******************************************************************************
template<TargetType K>
ReturnCode _setAttributeArraySignedShort(const AttributeId i_id,
                                         const Target<K>& i_target,
                                         int8_t * i_pValues)
{
    fapi2::AttributeData i_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    i_data.faValidMask = FAPI_ATTRIBUTE_TYPE_INT8ARY;
    i_data.faInt8ary = i_pValues;

    l_ecmd_rc = fapi2SetAttribute(l_ecmd_target, i_id, i_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Get int32_t array
//******************************************************************************
template<TargetType K>
ReturnCode _getAttributeArraySignedWord(const AttributeId i_id,
                                        const Target<K>& i_target,
                                        int32_t * o_pValues)
{
    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_INT32ARY;
    o_data.faInt32ary = o_pValues;

    l_ecmd_rc = fapi2GetAttribute(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Set int32_t array
//******************************************************************************
template<TargetType K>
ReturnCode _setAttributeArraySignedWord(const AttributeId i_id,
                                        const Target<K>& i_target,
                                        int32_t * i_pValues)
{
    fapi2::AttributeData i_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    i_data.faValidMask = FAPI_ATTRIBUTE_TYPE_INT32ARY;
    i_data.faInt32ary = i_pValues;

    l_ecmd_rc = fapi2SetAttribute(l_ecmd_target, i_id, i_data);
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Get int64_t array
//******************************************************************************
template<TargetType K>
ReturnCode _getAttributeArraySignedDoubleWord(const AttributeId i_id,
                                              const Target<K>& i_target,
                                              int64_t * o_pValues)
{
    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_INT64ARY;
    o_data.faInt64ary = o_pValues;

    l_ecmd_rc = fapi2GetAttribute(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}

//******************************************************************************
// Set int64_t array
//******************************************************************************
template<TargetType K>
ReturnCode _setAttributeArraySignedDoubleWord(const AttributeId i_id,
                                              const Target<K>& i_target,
                                              int64_t * i_pValues)
{
    fapi2::AttributeData i_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    i_data.faValidMask = FAPI_ATTRIBUTE_TYPE_INT64ARY;
    i_data.faInt64ary = i_pValues;

    l_ecmd_rc = fapi2SetAttribute(l_ecmd_target, i_id, i_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}

//******************************************************************************
// Get Override int8_t array
//******************************************************************************
template<TargetType K>
ReturnCode _getAttributeOverrideArraySignedShort(const AttributeId i_id,
                                                 const Target<K>& i_target,
                                                 int8_t * o_pValues)
{
    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS;

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_INT8ARY;
    o_data.faInt8ary = o_pValues;

    l_ecmd_rc = fapi2GetAttributeOverride(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Get Override int32_t array
//******************************************************************************
template<TargetType K>
ReturnCode _getAttributeOverrideArraySignedWord(const AttributeId i_id,
                                                const Target<K>& i_target,
                                                int32_t * o_pValues)
{
    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_INT32ARY;
    o_data.faInt32ary = o_pValues;

    l_ecmd_rc = fapi2GetAttributeOverride(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Get Override int64_t array
//******************************************************************************
template<TargetType K>
ReturnCode _getAttributeOverrideArraySignedDoubleWord(const AttributeId i_id,
                                                      const Target<K>& i_target,
                                                      int64_t * o_pValues)
{
    fapi2::AttributeData o_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    o_data.faValidMask = FAPI_ATTRIBUTE_TYPE_INT64ARY;
    o_data.faInt64ary = o_pValues;

    l_ecmd_rc = fapi2GetAttributeOverride(l_ecmd_target, i_id, o_data); 
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Set int8_t 
//******************************************************************************
template<typename T, TargetType K>
ReturnCode _set(const AttributeId i_id,
                const Target<K>& i_target,
                int8_t& i_value)
{
    static_assert(std::is_same<T, int8_t>::value, "Attribute type mismatch"); // May need to remove

    fapi2::AttributeData i_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    i_data.faValidMask = FAPI_ATTRIBUTE_TYPE_INT8;
    i_data.faInt8 = i_value;

    l_ecmd_rc = fapi2SetAttribute(l_ecmd_target, i_id, i_data);
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Set int32_t 
//******************************************************************************
template<typename T, TargetType K>
ReturnCode _set(const AttributeId i_id,
                const Target<K>& i_target,
                int32_t& i_value)
{
    static_assert(std::is_same<T, int32_t>::value, "Attribute type mismatch"); // May need to remove

    fapi2::AttributeData i_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    i_data.faValidMask = FAPI_ATTRIBUTE_TYPE_INT32;
    i_data.faInt32 = i_value;

    l_ecmd_rc = fapi2SetAttribute(l_ecmd_target, i_id, i_data);
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}


//******************************************************************************
// Set int64_t 
//******************************************************************************
template<typename T, TargetType K>
ReturnCode _set(const AttributeId i_id,
                const Target<K>& i_target,
                const int64_t & i_value)
{
    static_assert(std::is_same<T, int64_t>::value, "Attribute type mismatch"); // May need to remove

    fapi2::AttributeData i_data;
    fapi2::ReturnCode l_fapi_rc(FAPI2_RC_SUCCESS); 
    uint32_t l_ecmd_rc = ECMD_SUCCESS; 

    ecmdChipTarget l_ecmd_target;
    fapiTargetToEcmdTarget(i_target, l_ecmd_target);

    i_data.faValidMask = FAPI_ATTRIBUTE_TYPE_INT64;
    i_data.faInt64 = i_value;

    l_ecmd_rc = fapi2SetAttribute(l_ecmd_target, i_id, i_data);
    if (l_ecmd_rc)
    {
        l_fapi_rc = (ReturnCodes) l_ecmd_rc;
    }
    return l_fapi_rc;
}


} // namespace fapi2
#endif // FAPI2PLATATTRIBUTESERVICE_H
