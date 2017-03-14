/******************************************************************************
 *
 * IBM Confidential
 *
 * Licensed Internal Code Source Materials
 *
 * IBM Flexible Support Processor Licensed Internal Code
 *
 * (c) Copyright IBM Corp. 2006
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

#ifndef _ADAL_SCOM_TYPES_H_
#define _ADAL_SCOM_TYPES_H_

#define __SCOM_INTDEV_STRING "_i"

#include <unistd.h>
#include <adal_base.h>

/* ioctl commands */
#define IOCTL_WRITEREG          0x0005
#define IOCTL_READREG           0x0006
#define IOCTL_RESET             0x0007
#define IOCTL_GETTRACE          0x0008
#define IOCTL_FLUSHZERO_START   0x0009
#define IOCTL_FLUSHZERO_STOP    0x000A
#define IOCTL_WRITESCOM         0x000C
#define IOCTL_READSCOM          0x000D
#define IOCTL_SET_MASKS_IN_MEM  0x000E
#define IOCTL_SCOMV		0x0010
#define IOCTL_WRITESCOMUNDERMASK 0x0011
#define IOCTL_WRITEPROTECT       0x0012
#define IOCTL_SETGPREG           0x0013
#define IOCTL_GETGPREG           0x0014
#define IOCTL_SETERRORMASK       0x0015

/* all registers within SCAN ENGINE */
#define SCOM_DATA0_REGISTER 0x00
#define SCOM_DATA1_REGISTER 0x01
#define SCOM_COMMAND_REGISTER 0x02
#define SCOM_RESET_PSCOM_REGISTER 0x06
#define SCOM_RESET_REGISTER 0x07
#define SCOM_STATUS_REGISTER 0x07
#define SCOM_ARBITER_REGISTER 0x08
#define SCOM_TOAD_REGISTER 0x09
#define SCOM_CHIP_ID_REGISTER 0x0A
#define SCOM_IRQ_REGISTER 0x0B      /*!< only supported in Z7/P7   */
#define SCOM_COMP_MASK_REGISTER 0x0C
#define SCOM_TRUE_MASK_REGISTER 0x0D
#define SCOM_GP1_REGISTER 0x10
#define SCOM_GP2_REGISTER 0x11
#define SCOM_GP3_REGISTER 0x12
#define SCOM_GP4_REGISTER 0x13
#define SCOM_GP5_REGISTER 0x14
#define SCOM_GP6_REGISTER 0x15
#define SCOM_GP7_REGISTER 0x16
#define SCOM_PERV_GP3_REGISTER 0x1B

#define SCOM_WRITEPROTECT_REGISTER_ZIOC_SN 0x12	/*!< only on ZIOC/SN */
#define SCOM_WRITEPROTECT_REGISTER_Z7 0x18	/*!< only on Z7 */

#define SCOM_SNS1_REGISTER 0x19     /*!< z7 only */
#define SCOM_SNS2_REGISTER 0x1A     /*!< z7 only */

#define SCOM_DATA_LEN 8 /*!< we have 8 Bytes SCOM */
#define SCOM_DATA_WORDS 2

#define SCOMREAD  0x1
#define SCOMWRITE 0x2
#define SCOMWRITEUNDERMASK 0x4
#define SCOMVOPFAILED 0x0000FFF;

#define SCOMWRITEPROTECTGET    0x00000001
#define SCOMWRITEPROTECTSET    0x00000002

#define SCOMWRITEPROTECTENABLE 0xDEADBEEF
#define SCOMWRITEPROTECTDISABLE 0x4453FFFF

#define SCOMWRITEPROTECTGP1 0x44530001
#define SCOMWRITEPROTECTGP2 0x44530002
#define SCOMWRITEPROTECTGP3 0x44530003
#define SCOMWRITEPROTECTGP4 0x44530004
#define SCOMWRITEPROTECTGP5 0x44530005
#define SCOMWRITEPROTECTGP6 0x44530006

/*!< @enum	scom_adal_reset_t
 * use this struct to read or write register
 */
typedef enum {
    SCOMRESETENGINE=1,	/*!< reset the fsi scom engine */
    SCOMRESETFULL=2	/*!< reset also the pscom engine */
} scom_adal_reset_t;

typedef enum {
    SCOMATTENTION	/*!< reset the device and reinitialize all configuration */
} scom_adal_event_t;

#define ADAL_EVENT_ATTN 0x00000001

typedef struct ioctl_scom_register
{
     unsigned long address;
     unsigned long value;
} ioctl_scom_register_t;

typedef struct ioctl_scom_writeprotect
{
     unsigned long cmd;
     unsigned long data;
} ioctl_scom_writeprotect_t;

typedef struct scom_masks
{
     unsigned long trueMask;
     unsigned long compMask;
} scom_masks_t;

typedef struct adal_scom_event
{
  unsigned long true_mask;
  unsigned long comp_mask;
  unsigned long status;
  unsigned long irq;
} adal_scom_event_t ;


typedef struct ioctl_scom_readwrite
{
	uint64_t scomAddress;
	unsigned long userbuffer[SCOM_DATA_WORDS];
	unsigned long usermask[SCOM_DATA_WORDS];
	unsigned long status;
	uint16_t mode;
	int16_t returnCode;
} ioctl_scom_readwrite_t, scom_op;

typedef struct scom_v_op
{
	size_t nr_ops;
	unsigned long flags;
	scom_op *ops;
} scom_v_op;


#endif
