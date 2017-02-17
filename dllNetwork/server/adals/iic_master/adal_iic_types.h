/******************************************************************************
 *
 * IBM Confidential
 *
 * Licensed Internal Code Source Materials
 *
 * IBM Flexible Support Processor Licensed Internal Code
 *
 * (c) Copyright IBM Corp. 2006, 2012
 *
 * The source code is for this program is not published or otherwise divested
 * of its trade secrets, irrespective of what has been deposited with the
 * U.S. Copyright Office.
 *
 ******************************************************************************
 *
 *
 *
 *****************************************************************************/


/*
 *   Desc: IIC Access Device Abstraction Layer (a.k.a ADAL).
 */

#ifndef __ADAL_IIC_TYPES_H__
#define __ADAL_IIC_TYPES_H__

#include <adal_base_types.h>
#include <iic_dd.h>

/*! @enum adal_iic_cfg
* linkage between device driver ioctl values and adal config values
*/
typedef enum {
	ADAL_IIC_CFG_SPEED = IIC_IOC_SPEED,
	ADAL_IIC_CFG_DEV_ADDR = IIC_IOC_DEV_ADDR,
	ADAL_IIC_CFG_DEV_WIDTH = IIC_IOC_DEV_WIDTH,
	ADAL_IIC_CFG_OFFSET = IIC_IOC_OFFSET,
	ADAL_IIC_CFG_INC_ADDR = IIC_IOC_INC_ADDR,
	ADAL_IIC_CFG_TIMEOUT = IIC_IOC_TIMEOUT,
	ADAL_IIC_CFG_RDELAY = IIC_IOC_RDELAY,
	ADAL_IIC_CFG_WDELAY = IIC_IOC_WDELAY,
	ADAL_IIC_CFG_RSPLIT = IIC_IOC_RSPLIT,
	ADAL_IIC_CFG_WSPLIT = IIC_IOC_WSPLIT,
	ADAL_IIC_CFG_REDO_POL = IIC_IOC_REDO_POL,
	ADAL_IIC_CFG_SPD_POL = IIC_IOC_SPD_POL,
	ADAL_IIC_CFG_REDO_DELAY = IIC_IOC_REDO_DELAY,
	ADAL_IIC_CFG_LCK_ADDR = IIC_IOC_LCK_ADDR,
	ADAL_IIC_CFG_ULCK_ADDR = IIC_IOC_ULCK_ADDR,
	ADAL_IIC_CFG_LCK_ENG = IIC_IOC_LCK_ENG,
	ADAL_IIC_CFG_ULCK_ENG = IIC_IOC_ULCK_ENG,
	ADAL_IIC_CFG_ALL = IIC_IOC_ALL,
	ADAL_IIC_CFG_BUS_STATE = IIC_IOC_BUS_STATE,
	ADAL_IIC_CFG_FLAGS = IIC_IOC_FLAGS,
} adal_iic_cfg_t;

/*! @enum adal_iic_val
* IIC device driver specific constants
*/
enum
{
	ADAL_IIC_VAL_100KHZ = 100,
	ADAL_IIC_VAL_400KHZ = 400,
	ADAL_IIC_VAL_0BYTE = 0,
	ADAL_IIC_VAL_1BYTE = 1,
	ADAL_IIC_VAL_2BYTE = 2,
	ADAL_IIC_VAL_3BYTE = 3,
	ADAL_IIC_VAL_4BYTE = 4,
	ADAL_IIC_VAL_0BIT = 0,
	ADAL_IIC_VAL_1BIT = 2,
	ADAL_IIC_VAL_2BIT = 6,
	ADAL_IIC_VAL_3BIT = 14,
	ADAL_IIC_VAL_LCK_ALL = 0x7ff,
	ADAL_IIC_VAL_LCK_ENG = 0xffff,
	ADAL_IIC_VAL_ADDR_NOACK = IIC_VAL_ADDR_NOACK,
	ADAL_IIC_VAL_DATA_NOACK = IIC_VAL_DATA_NOACK,
	ADAL_IIC_VAL_TIMEOUT = IIC_VAL_TIMEOUT,
	ADAL_IIC_VAL_LOST_ARB = IIC_VAL_LOST_ARB,
	ADAL_IIC_VAL_BUS_ERR = IIC_VAL_BUS_ERR,
	ADAL_IIC_VAL_ALL_ERRS = IIC_VAL_ALL_ERRS,
	ADAL_IIC_VAL_BOTH_LO = IIC_VAL_BOTH_LO,
	ADAL_IIC_VAL_SDA_LO = IIC_VAL_SDA_LO,
	ADAL_IIC_VAL_SCL_LO = IIC_VAL_SCL_LO,
	ADAL_IIC_VAL_BOTH_HI = IIC_VAL_BOTH_HI,
	ADAL_IIC_FLAG_NO_DMA = IIC_NO_DMA,
	ADAL_IIC_FLAG_FORCE_DMA = IIC_FORCE_DMA,
	ADAL_IIC_FLAG_SPECIAL_RD = IIC_SPECIAL_RD,
};

typedef iic_opts_t adal_iic_opts_t;
typedef iic_xfr_opts_t adal_iic_xfr_opts_t;
typedef iic_rec_pol_t adal_iic_rec_pol_t;
typedef iic_lock_t adal_iic_lock_t;

#include <iic_dev_configs.h>

#define ADAL_IIC_DDR4_NACK_REDO_POL 0x0002DD40

#endif /* __ADAL_IIC_TYPES_H__ */
