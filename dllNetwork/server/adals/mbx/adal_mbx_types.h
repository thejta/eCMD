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
