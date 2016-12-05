/******************************************************************************
  *
  * IBM Confidential
  *
  * Licensed Internal Code Source Materials
  *
  * IBM Flexible Support Processor Licensed Internal Code
  *
  * (c) Copyright IBM Corp. 2003, 2012
  *
  * The source code is for this program is not published or otherwise divested
  * of its trade secrets, irrespective of what has been deposited with the
  * U.S. Copyright Office.
  *
  ******************************************************************************
  * Desc: Access Device Abstraction Layer (a.k.a. ADAL) for mailbox.
  *
  * Authors: Shaun Wetzstein <shaun@us.ibm.com>
  *          Christopher Bostic <cbostic@us.ibm.com>
  *****************************************************************************/
#ifndef __ADAL_MBX_TYPES_H__
#define __ADAL_MBX_TYPES_H__

#include <stdint.h>               /* uintX_t */

#include <sys/types.h>            /* size_t */
#include <sys/uio.h>              /* struct iovec */

/*!@brief
 * Mailbox message buffer format:
 *
 *    <--------- Total Size ----------> <- Header Len -> <-- Data Len -->
 *                2 bytes                    1 byte           1 byte
 *   +----------------+----------------+----------------+----------------+
 *   |                |                |                |                |
 *   +----------------+----------------+----------------+----------------+
 *    offset:        0                1                2                3
 *
 *    <-------- Header Data -------> <---------- Message Data ---------->
 *        variable [0..12] bytes           variable [0..64] bytes
 *   +--------------((--------------+-----------------((-----------------+
 *   |              \\              |                 \\                 |
 *   +--------------))--------------+-----------------))-----------------+
 *    offset:   variable        variable
 *
 */
typedef enum {
	ADAL_MBX_SCRATCH_0 = 0,
	ADAL_MBX_SCRATCH_1 = 1,
	ADAL_MBX_SCRATCH_2 = 2,
	ADAL_MBX_SCRATCH_3 = 3,
	ADAL_MBX_SCRATCH_4 = 4,
	ADAL_MBX_SCRATCH_5 = 5,
	ADAL_MBX_SCRATCH_6 = 6,
	ADAL_MBX_SCRATCH_7 = 7,
	ADAL_MBX_SCRATCH_INVALID = 255
} adal_mbx_scratch_t;

typedef enum {
   ADAL_MBX_SPAD_READ = 1,
   ADAL_MBX_SPAD_WRITE = 2,
} adal_mbx_scratch_mode_t;

typedef enum {
   MBX_IOCTL_READ_REG = 1,
   MBX_IOCTL_WRITE_REG= 2,
} adal_mbx_gpreg_mode_t;


#endif /* __ADAL_MBX_TYPES_H__ */
