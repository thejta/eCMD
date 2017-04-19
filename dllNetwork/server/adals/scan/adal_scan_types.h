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

#ifndef _ADAL_SCAN_TYPES_H_
#define _ADAL_SCAN_TYPES_H_

#include <unistd.h>
#include <stdint.h>
/* ioctl commands */
#define IOCTL_GETOFFSET		0x0002
#define IOCTL_WRITEREG		0x0005
#define IOCTL_READREG		0x0006
#define IOCTL_RESET		0x0007
#define IOCTL_GETTRACE		0x0008
#define IOCTL_WRITESCAN		0x0009
#define IOCTL_READSCAN		0x000A
#define IOCTL_SETERRORMASK_SCAN	0x000B
#define IOCTL_SHIFT_DMA		0x000C
#define IOCTL_CCR0		0x000D

/* types of shift operation */
#define ADAL_SHIFT_SCOM		0x0
#define ADAL_SHIFT_SCAN		0x1

/* types of scan operation */
#define ADAL_SHIFT_SCAN_READ	0x0
#define ADAL_SHIFT_SCAN_WRITE	0x1
#define ADAL_SHIFT_SCAN_MOD	0x2

/* types of scom operation */
#define ADAL_SHIFT_SCOM_READ		0x3
#define ADAL_SHIFT_SCOM_WRITE		0x4
#define ADAL_SHIFT_SCOM_BULKREAD	0x5
#define ADAL_SHIFT_SCOM_BULKWRITE	0x6
#define ADAL_SHIFT_SCOM_MOD_OR		0x7
#define ADAL_SHIFT_SCOM_MOD_AND		0x8

/* all registers within SCAN ENGINE */
#define SCAN_FIFO_REGISTER 0x00
#define SCAN_COMMAND_REGISTER 0x01
#define SCAN_FRONT_END_LENGTH_REGISTER 0x02
#define SCAN_RESET_REGISTER 0x06
#define SCAN_STATUS_REGISTER 0x07
#define SCAN_EXTSTATUS_REGISTER 0x08
#define SCAN_CHIP_ID_REGISTER 0x09
#define SCAN_COMP_MASK_REGISTER 0x0C
#define SCAN_TRUE_MASK_REGISTER 0x0D
#define SCAN_SHIFTCRTL_REGISTER 0x10
#define SCAN_PIBERR1_REGISTER 0x11
#define SCAN_PIBERR2_REGISTER 0x12

#define SCAN_ERRP_REGISTER	0x21
#define SCAN_RS_REGISTER	0x1C
#define SCAN_CMDOP_REGISTER	0x1B
#define SCAN_CMPMSK_REGISTER	0x1A

#define SCANRESETFULL 0x02
#define SCANRESETLIGHT 0x01

/* valid OPTIONS */
/* default 0x00000000 is: */
/* HEADERCHECK */
/* NO SETPULSE */
/* NO SCAN via PIB engine */

#define SCANHEADERCHECK   0x00000000
#define SCANNOHEADERCHECK 0x00000001
#define SCANNOSETPULSE    0x00000000
#define SCANSETPULSE      0x00000002
#define SCANNOEXTRAACLOCK 0x00000000
#define SCANEXTRAACLOCK   0x00000004
#define SCANNOEXTRABCLOCK 0x00000000
#define SCANEXTRABCLOCK   0x00000008
#define NOSCANVIAPIB      0x00000000
#define SCANVIAPIB        0x00000010

/*! @struct	ioctl_scan_register
 * use this struct to read or write register
 */
typedef struct ioctl_scan_register
{
        unsigned long address;
        unsigned long value;
} ioctl_scan_register_t;

/* changed with CR092 */
typedef struct ioctl_scan_readwrite
{
        char * userbuffer;
	int chainAddress;
        size_t bitlength;
        unsigned long options;
        unsigned long status;
        unsigned long aiostatus;
        unsigned long isAio;
} ioctl_scan_readwrite_t;

#define BITMAP_ACTION_MASK	0x0FF	/*!< action(s) to take on series */
#define BITMAP_ACTION_COPY	0x000	/*!< action: copy bits */
#define BITMAP_ACTION_INVERT	0x001	/*!< action: invert bits */
#define BITMAP_ACTION_FILL	0x002	/*!< action: fill with dummy bit */

#define BITMAP_DIRECTION_MASK	0x100	/*!< series processing direction */
#define BITMAP_DIRECTION_FWD	0x000	/*!< series is processed forward */
#define BITMAP_DIRECTION_BKWD	0x100	/*!< series is processed backward */

#define BITMAP_DUMMY_BIT_MASK	0x200	/*!< filler bit to use in filler action */
#define BITMAP_DUMMY_BIT_ZERO	0x000	/*!< fill with zero */
#define BITMAP_DUMMY_BIT_ONE	0x200	/*!< fill with one */

typedef struct bitmap {
	unsigned long offset;	/*!< offset into scanned bits */
	unsigned long length;	/*!< length of the series to process */
	unsigned long flags;	/*!< how the bits should be processed */
} bitmap_t;

typedef struct scan_op {
	uint8_t type;		/*!< type of operation : READ, WRITE, MOD */
	uint32_t chain_address;	/*!< chain address */
	uint32_t bit_length;	/*!< front end length */
} scan_op_t;

/* @struct	scom_oper
 * oper because of conflict of names in types.h in adal_scom
 */
typedef struct scom_oper {
	uint8_t type;		/*!< type of operation : READ, WRITE, BULKREAD,
					BULKWRITE */
	uint64_t address;	/*!< address */
	uint32_t size;		/*!< size of data in words :
					if we want 1 read/write - size = 2,
					 m = n * 2, when 'm' size, 'n' - count of
					 read/write. */
} scom_op_t;

typedef struct shift_op {
	uint8_t type; /*!< type of shift operation : SCAN, SCOM*/
	uint32_t flags;
	uint32_t *data;
	uint32_t gid;
	union {
		scom_op_t scom_op;
		scan_op_t scan_op;
	} op;
} shift_op_t;

/*! @struct	shift_stream
 * Singly linked list with shift_op and with gid of operation
 * for better searching.
 */
typedef struct shift_stream {
	shift_op_t *shift_op;		/*!< operation */
	uint32_t *gid;			/*!< &(op->gid) */
	struct shift_stream *next;	/*!< next element of stream */
} shift_stream_t;

#endif
