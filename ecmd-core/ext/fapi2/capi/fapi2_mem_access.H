/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: hwpf/fapi2/include/fapi2_mem_access.H $                       */
/*                                                                        */
/* IBM CONFIDENTIAL                                                       */
/*                                                                        */
/* EKB Project                                                            */
/*                                                                        */
/* COPYRIGHT 2019                                                         */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* The source code for this program is not published or otherwise         */
/* divested of its trade secrets, irrespective of what has been           */
/* deposited with the U.S. Copyright Office.                              */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
///
/// @file fapi2_mem_access.H
/// @brief ADU and PBA memory access definitions
///

#ifndef FAPI2_MEMACCESSDEF_H_
#define FAPI2_MEMACCESSDEF_H_

namespace fapi2
{

//------------------------------------------------------------------------------
// Constant Definitions
//------------------------------------------------------------------------------
enum sbeMemoryAccessFlags
{
    SBE_MEM_ACCESS_FLAGS_TARGET_PROC          = 0x00000001,
    SBE_MEM_ACCESS_FLAGS_TARGET_PBA           = 0x00000002,
    SBE_MEM_ACCESS_FLAGS_AUTO_INCR_ON         = 0x00000004,
    SBE_MEM_ACCESS_FLAGS_ECC_OVERRIDE         = 0x00000008,
    SBE_MEM_ACCESS_FLAGS_TAG                  = 0x00000010,
    SBE_MEM_ACCESS_FLAGS_FAST_MODE_ON         = 0x00000020,
    SBE_MEM_ACCESS_FLAGS_LCO_MODE             = 0x00000040,
    SBE_MEM_ACCESS_FLAGS_CACHE_INHIBITED_MODE = 0x00000080,
    SBE_MEM_ACCESS_FLAGS_HOST_PASS_THROUGH    = 0x00000100,
    SBE_MEM_ACCESS_FLAGS_CACHE_INJECT_MODE    = 0x00000200,
    SBE_MEM_ACCESS_FLAGS_PB_DIS_MODE          = 0x00000400,
    SBE_MEM_ACCESS_FLAGS_SWITCH_MODE          = 0x00000800,
    SBE_MEM_ACCESS_FLAGS_PB_INIT_MODE         = 0x00001000,
    SBE_MEM_ACCESS_FLAGS_PRE_SWITCH_CD_MODE   = 0x00002000,
    SBE_MEM_ACCESS_FLAGS_PRE_SWITCH_AB_MODE   = 0x00004000,
    SBE_MEM_ACCESS_FLAGS_POST_SWITCH_MODE     = 0x00008000,
    SBE_MEM_ACCESS_FLAGS_ATOMIC_MODE          = 0x00010000,
    SBE_MEM_ACCESS_FLAGS_ATOMIC_TYPE          = 0x00060000, // 0=ADD, 1=AND, 2=OR, 3=XOR
};

} // End fapi2 namespace

#endif // End FAPI2_MEMACCESSDEF_H_
