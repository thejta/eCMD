//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2003,2017 IBM International Business Machines Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
//IBM_PROLOG_END_TAG

#ifndef IIC_H
#define IIC_H
#include <linux/ioctl.h>

typedef struct iic_rec_pol
{
	unsigned long redo_pol;
#define IIC_VAL_ADDR_NOACK	0x00010000
#define IIC_VAL_DATA_NOACK	0x00020000
#define IIC_VAL_TIMEOUT		0x00040000
#define IIC_VAL_LOST_ARB	0x00080000
#define IIC_VAL_BUS_ERR		0x00100000
#define IIC_VAL_ALL_ERRS	0xffff0000
	unsigned long rsvd;
	unsigned long redo_delay;
} iic_rec_pol_t;

#define IIC_VAL_100KHZ  100
#define IIC_VAL_400KHZ	400
typedef struct iic_xfr_opts
{
	unsigned short rsvd;
	unsigned short dev_addr;	// address of end device
	unsigned short dev_width;	// number of bytes for offset (1-4)
	unsigned long inc_addr;		// mask of address bits to increment
					// for devices that span multiple
					// addresses.
	unsigned long timeout;		// operation timeout (msec)
	unsigned short wdelay;		// delay between write xfrs (msec)
	unsigned short rdelay;		// delay between read xfrs (msec)
	unsigned short wsplit;		// splits writes into smaller chunks
	unsigned short rsplit;		// splits reads into smaller chunks
	unsigned long offset;		// offset from beginning of device
	unsigned long flags;		// flags defined below
} iic_xfr_opts_t;


enum {
	IIC_FORCE_DMA = 0x01,		// use dma regardless of xfr size
	IIC_NO_DMA = 0x02,		// disallow dma
	IIC_SPECIAL_RD = 0x04,		// workaround for PLL/CRC chips
};

typedef struct iic_opts
{
	iic_xfr_opts_t xfr_opts;
	iic_rec_pol_t recovery;
} iic_opts_t;

typedef struct iic_lock
{
	unsigned short mask;
	unsigned short addr;
	unsigned long timeout;
} iic_lock_t;

typedef struct iicslv_opts
{
	unsigned long addr;
	unsigned long timeout;
} iicslv_opts_t;

#define IICSLV_ZBUF_MAX_SZ	256
typedef struct iicslv_ioctl_args
{	
	unsigned long slave_addr;
	unsigned long mode;
	unsigned char * buffer;
	unsigned long len;

} iicslv_ioctl_args_t;

/* Slave IOCTL Ordinal Numbers */
#define IICSLV_IOC_MAGIC	0x08
enum
{
	IICSLV_IOC_RESET_LIGHT,
	IICSLV_IOC_RESET_FULL,

	IICSLV_IOC_0_BYTES = IICSLV_IOC_RESET_FULL,

	IICSLV_IOC_ADDR,
	IICSLV_IOC_TIMEOUT,

	IICSLV_IOC_4_BYTES = IICSLV_IOC_TIMEOUT,

	IICSLV_IOC_ALL,
	IICSLV_IOC_DISPLAY_REGS,
	IICSLV_IOC_ZBUF_INIT,
	IICSLV_IOC_MAXNR = IICSLV_IOC_ZBUF_INIT,
};

/* Master IOCTL Ordinal Numbers */
#define IIC_IOC_MAGIC 		0x07
enum
{
	/* 0 bytes */
	IIC_IOC_RESET_LIGHT,
	IIC_IOC_RESET_FULL,

	IIC_IOC_0_BYTES = IIC_IOC_RESET_FULL,

	/* 4 bytes */
	IIC_IOC_SPEED,
	IIC_IOC_DEV_ADDR,
	IIC_IOC_DEV_WIDTH,
	IIC_IOC_OFFSET,
	IIC_IOC_INC_ADDR,
	IIC_IOC_TIMEOUT,
	IIC_IOC_RDELAY,
	IIC_IOC_WDELAY,
	IIC_IOC_RSPLIT,
	IIC_IOC_WSPLIT,
	IIC_IOC_REDO_POL,
	IIC_IOC_SPD_POL,
	IIC_IOC_REDO_DELAY,
	IIC_IOC_BUS_STATE,
#define IIC_VAL_BOTH_LO 0x00
#define IIC_VAL_SDA_LO  0x01
#define IIC_VAL_SCL_LO  0x02
#define IIC_VAL_BOTH_HI 0x03
	IIC_IOC_FLAGS,

	IIC_IOC_4_BYTES = IIC_IOC_FLAGS,

	/* Objects */
	IIC_IOC_LCK_ADDR,
	IIC_IOC_ULCK_ADDR,
	IIC_IOC_LCK_ENG,
	IIC_IOC_ULCK_ENG,
	IIC_IOC_ALL,
	IIC_IOC_DISPLAY_REGS,
        IIC_IOC_REPEATED_IO,
	IIC_IOC_MAXNR = IIC_IOC_REPEATED_IO,
};

//--------------------------------------------------------------------
// Specific header for repeated I/O
//--------------------------------------------------------------------
/*
 * I2C Message - used for pure i2c transaction, also from /dev interface
 */
struct i2c_msg {
        unsigned short addr;     /* slave address                        */
        unsigned short flags;
#define I2C_M_TEN       0x10    /* we have a ten bit chip address       */
#define I2C_M_RD        0x01
#define I2C_M_NOSTART   0x4000
#define I2C_M_REV_DIR_ADDR      0x2000
#define I2C_M_IGNORE_NAK        0x1000
#define I2C_M_NO_RD_ACK         0x0800
        unsigned short len;              /* msg length                           */
        unsigned char *buf;              /* pointer to msg data                  */
};
/* This is the structure as used in the I2C_RDWR ioctl call */
struct i2c_rdwr_ioctl_data {
        struct i2c_msg  *msgs;    /* pointers to i2c_msgs */
        unsigned int nmsgs;                    /* number of i2c_msgs */
};

#endif
