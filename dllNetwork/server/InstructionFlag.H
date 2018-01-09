#ifndef _InstructionFlag_H
#define _InstructionFlag_H
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

#include <string>
#include <stdint.h>

/**
 * @file Instruction.H
 * @brief Provides a base class for instructions being passed to the fsp server
 *
 */

/* ------------------------------------------------------------------ */
/*  These are the Instruction Flag Defines                            */
/* ------------------------------------------------------------------ */
#define INSTRUCTION_FLAG_DEBUG              0x80000000
#define INSTRUCTION_FLAG_NOEXECUTE          0x40000000
#define INSTRUCTION_FLAG_SERVER_DEBUG       0x20000000
#define INSTRUCTION_FLAG_PERFORMANCE        0x10000000

/* CRC Flags */
#define INSTRUCTION_FLAG_CRC_ENABLE         0x08000000
#define INSTRUCTION_FLAG_32BIT_CRC          0x04000000
#define INSTRUCTION_FLAG_16BIT_CRC          0x02000000

#define INSTRUCTION_FLAG_PERSISTENT_DATA    0x01000000

/* FSI Flags */
#define INSTRUCTION_FLAG_FSI_SCANHEADERCHECK    0x00800000
#define INSTRUCTION_FLAG_FSI_SCANSETPULSE       0x00400000
#define INSTRUCTION_FLAG_FSI_USE_DRA            0x00200000
#define INSTRUCTION_FLAG_FSI_SCANEXTRABCLOCK    0x00100000
#define INSTRUCTION_FLAG_FSI_SCANVIAPIB         0x00080000
#define INSTRUCTION_FLAG_FSI_CFAM2_0            0x00040000

#define INSTRUCTION_FLAG_64BIT_ADDRESS          0x00008000
#define INSTRUCTION_FLAG_DEVSTR                 0x00004000
#define INSTRUCTION_FLAG_CFAM_MAILBOX           0x00002000

#define INSTRUCTION_FLAG_CACHE_INHIBITED        0x00001000
#define INSTRUCTION_FLAG_SBEFIFO_RESET_ENABLE   0x00000800
#define INSTRUCTION_FLAG_POWR_DDR4              0x00000400
#define INSTRUCTION_FLAG_NO_PIB_RESET           0x00000200

/**
 @brief function to create string from instruction flag
*/
std::string InstructionFlagToString(uint32_t i_flag);

/* ------------------------------------------------------------------ */
/*  These are the Instruction Tap State Defines                       */
/* ------------------------------------------------------------------ */
#define INSTRUCTION_TAP_RESETTAP            0x80000000
#define INSTRUCTION_TAP_RUNTESTIDLE         0x40000000
#define INSTRUCTION_TAP_SHIFTDR             0x20000000
#define INSTRUCTION_TAP_PAUSEDR             0x10000000

#define INSTRUCTION_TAP_SHIFTIR             0x08000000
#define INSTRUCTION_TAP_PAUSEIR             0x04000000
#define INSTRUCTION_TAP_NO_TAP_STATE        0x02000000
#define INSTRUCTION_TAP_UPDATEDR            0x01000000

#define INSTRUCTION_TAP_UPDATEIR            0x00800000

#define INSTRUCTION_TAP_DIAMOND_1           0x00001000
#define INSTRUCTION_TAP_DIAMOND_2           0x00002000
#define INSTRUCTION_TAP_DIAMOND_3           0x00003000
#define INSTRUCTION_TAP_DIAMOND_4           0x00004000
#define INSTRUCTION_TAP_DIAMOND_5           0x00005000
#define INSTRUCTION_TAP_DIAMOND_6           0x00006000
#define INSTRUCTION_TAP_DIAMOND_7           0x00007000
#define INSTRUCTION_TAP_DIAMOND_8           0x00008000
#define INSTRUCTION_TAP_DIAMOND_MASK        0x0000F000
#define INSTRUCTION_TAP_DIAMOND_SHIFT       12

#define INSTRUCTION_TAP_TAP_STATE_HACK      0x00000800
// This define is for System X and chips that can't go through Test Logic Reset - JTA 01/25/09
#define INSTRUCTION_TAP_NO_TEST_LOGIC_RESET 0x00000400

#define INSTRUCTION_TAP_NO_JTAG_SETUP       0x00000002
#define INSTRUCTION_TAP_READ_NO_DATA        0x00000001

/**
 @brief function to create string from instruction tap
*/
std::string InstructionTapToString(uint32_t i_tap);

/* ------------------------------------------------------------------ */
/*  These are the Instruction Flag Defines                            */
/* ------------------------------------------------------------------ */
#define INSTRUCTION_I2C_FLAG_NACK_RETRY_100MS   0x80000000
#define INSTRUCTION_I2C_FLAG_MASK               0xFF000000
#define INSTRUCTION_I2C_FLAG_RETRY_MASK         0x00FFA000  // specifies number of bytes written per jiffy, a jiffy is 10ms            
#define INSTRUCTION_I2C_FLAG_RETRY_SHIFT        14
#define INSTRUCTION_I2C_FLAG_MSG_SIZE_MASK      0x00003FFF  // max value of 8192 accepted
#define INSTRUCITON_I2C_FLAG_MSG_SIZE_SHIFT     0

/**
 @brief function to create string from instruction I2C flag
*/
std::string InstructionI2CFlagToString(uint32_t i_i2cFlag);

/* ------------------------------------------------------------------ */
/*  These are the PNOR Instruction Flag Defines                       */
/* ------------------------------------------------------------------ */
#define INSTRUCTION_PNOR_FLAG_ERASE_PARTITION   0x80000000
#define INSTRUCTION_PNOR_FLAG_WRITE_PARTITION   0x40000000

/**
 @brief function to create string from instruction PNOR flag
*/
std::string InstructionPnorFlagToString(uint32_t i_pnorFlag);


/* The SERVER Info Flags */
#define SERVER_INFO_HW_CRC_SUPPORT          0x80000000      /* This Hardware supports CRC Generation */
#define SERVER_INFO_MULTI_CLIENT            0x40000000      /* This Hardware allows multiple clients to connect and run */
#define SERVER_INFO_FSI_SUPPORT             0x20000000      /* This Hardware supports FSI bus protocols */
#define SERVER_INFO_JTAG_ADAL_SUPPORT       0x10000000      /* This Simulation supports JTAG ADAL */
#define SERVER_INFO_AUTH_NEEDED             0x08000000      /* This Server needs authentication before proceeding */
#define SERVER_INFO_THREADAUTH_NEEDED       0x04000000      /* This Server needs authentication before proceeding */

/* ------------------------------------------------------------------ */
/*  This is the SERVER SPMEM Select Defines                           */
/* ------------------------------------------------------------------ */
#define SERVER_SPMEM_SPMEM        0x80000000
#define SERVER_SPMEM_TITAN        0x40000000
#define SERVER_SPMEM_I2C          0x20000000
#define SERVER_SPMEM_FSIREG       0x10000000

#endif // _InstructionFlag_H
