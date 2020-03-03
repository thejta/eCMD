#ifndef _ServerSPIInstruction_H
#define _ServerSPIInstruction_H
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
 * @file ServerSPIInstruction.H
 * @brief Provides a class for SPI instructions to run on the fsp server
 *
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <SPIInstruction.H>

class ServerSPIInstruction : public SPIInstruction
{
    protected:
    uint32_t spi_open(Handle ** handle, InstructionStatus & o_status);
    void spi_ffdc(Handle ** handle, InstructionStatus & o_status);
    uint32_t spi_close(Handle * handle);

    ssize_t spi_command(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status);
};

#endif // _ServerSPIInstruction_H
