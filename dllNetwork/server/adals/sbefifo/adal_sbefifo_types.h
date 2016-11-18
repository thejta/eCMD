/******************************************************************************
 *
 * IBM Confidential
 *
 * Licensed Internal Code Source Materials
 *
 * IBM Flexible Support Processor Licensed Internal Code
 *
 * (c) Copyright IBM Corp. 2015, 2015
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
#ifndef _ADAL_SBEFIFO_TYPES_H_
#define _ADAL_SBEFIFO_TYPES_H_

#include <stddef.h>
#include <adal_base.h>

struct adal_sbefifo_op
{
	void * data;     		/* data pointer - "in" for request , "out" for reply */
	size_t wordcount;  		/* "in" */
	unsigned long status;	/* "out" containing upstream fifo status for request, downstream for reply */
};


/*! @struct	ioctl_scan_register
 * use this struct to read or write register
 */
typedef struct ioctl_sbefifo_register
{
        unsigned long address;
        unsigned long value;
} ioctl_sbefifo_register_t;

typedef struct adal_sbefifo_op adal_sbefifo_request;
typedef struct adal_sbefifo_op adal_sbefifo_reply;

/* ioctl commands */
#define IOCTL_SUBMIT		0x1
#define IOCTL_REQUEST_RESET	0x2
#define IOCTL_READREG		0x3
#define IOCTL_WRITEREG		0x4
#define IOCTL_RESET		0x5


/* the registers of the SBEFIFO Engine */
#define ADAL_SBEFIFO_UPSTREAM_STATUS  0x01
#define ADAL_SBEFIFO_DOWNSTREAM_STATUS  0x11

#define ADAL_SBEFIFO_UPSTREAM_EOT  0x02
#define ADAL_SBEFIFO_DOWNSTREAM_EOT  0x12

#define ADAL_SBEFIFO_UPSTREAM_REQUEST_RESET  0x03
#define ADAL_SBEFIFO_DOWNSTREAM_REQUEST_RESET  0x13

#define ADAL_SBEFIFO_UPSTREAM_RESET  0x04
#define ADAL_SBEFIFO_DOWNSTREAM_RESET  0x14

#define ADAL_SBEFIFO_UPSTREAM_ACK_EOT  0x05
#define ADAL_SBEFIFO_DOWNSTREAM_ACK_EOT  0x15

#define ADAL_SBEFIFO_MAXMESSAGELEN  0x16  /* not yet defined in spec */

/* Bits in status register */
#define ADAL_SBEFIFO_REQUESTRESET_BIT 0x02000000   /* Bit 6 is the request reset bit */

#endif
