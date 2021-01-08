#ifndef _SERVERXDMAINSTRUCTION_H
#define _SERVERXDMAINSTRUCTION_H
//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2003,2020 IBM International Business Machines Corp.
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

/**
 * @file ServerXDMAInstruction.H
 * @brief Provides a class for XDMA access on the bmc server
 *
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <XDMAInstruction.H>

class ServerXDMAInstruction : public XDMAInstruction
{
    protected:
    uint32_t xdma_open(Handle ** handle, InstructionStatus & o_status);
    void xdma_ffdc(Handle ** handle, InstructionStatus & o_status);
    void xdma_reset(Handle * handle, InstructionStatus & o_status);
    uint32_t xdma_close(Handle * handle);

    ssize_t xdma_command(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status);
};

#endif // _ServerXDMAInstruction_H
