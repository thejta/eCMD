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


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#ifndef __STDC_FORMAT_MACROS
  #define __STDC_FORMAT_MACROS 1
#endif
#include <SPIInstruction.H>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iomanip>
#include <errno.h>
#include <inttypes.h>

#ifdef OTHER_USE
#include <arpa/inet.h>
#include <OutputLite.H>
extern OutputLite out;
#else
#include <CronusData.H>
#include <cronus_endian.h>
#endif

/*****************************************************************************/
/* SPIInstruction Implementation *********************************************/
/*****************************************************************************/
SPIInstruction::SPIInstruction(void) : Instruction(),
engineId(0),
select(0),
busSpeed(100000),
readLength(0),
msDelay(0),
data(NULL)
{
    version = 0x1;
    type = SPI;
}

SPIInstruction::~SPIInstruction(void)
{
    while (!mydata.empty())
    {
        delete mydata.back();
        mydata.pop_back();
    }
}

uint32_t SPIInstruction::setup(InstructionCommand i_command, std::string &i_deviceString, uint32_t i_engineId, uint32_t i_select, uint32_t i_busSpeed, uint32_t i_readLength, uint32_t i_msDelay, uint32_t i_flags, const ecmdDataBuffer * i_data)
{
    command = i_command;
    flags = i_flags;
    deviceString = i_deviceString;
    engineId = i_engineId;
    select = i_select;
    busSpeed = i_busSpeed;
    readLength = i_readLength;
    msDelay = i_msDelay;
    data = i_data;
    return 0;
}

uint32_t SPIInstruction::execute(ecmdDataBuffer & o_data, InstructionStatus & o_status, Handle ** io_handle)
{
    int rc = 0;

    /* set the version of this instruction in the status */
    o_status.instructionVersion = version;

    /* check for any previous errors to report back */
    if (error)
    {
        rc = o_status.rc = error;
        return rc;
    }

    switch(command)
    {
        case SPI_COMMAND:
            {
                char errstr[200];

                /* Open the Handle */
                rc = spi_open(io_handle, o_status);
                if (rc)
                {
                    o_status.rc = rc;
                    return rc;
                }

                errno = 0;

                o_data.setBitLength(readLength);
                ssize_t bytelen = readLength % 8 ? (readLength / 8) + 1 : readLength / 8;
                if ( bytelen == 0 )
                    bytelen = data->getByteLength() - commandLengthBytes;
                ssize_t len = 0;
                errno = 0;

                if (flags & INSTRUCTION_FLAG_SERVER_DEBUG)
                {
                    snprintf(errstr, 200, "SERVER_DEBUG : spi_command() readLength = %d, writeLength = %d\n", readLength, (data->getByteLength() - commandLengthBytes));
                    o_status.errorMessage.append(errstr);
                }

                len = spi_command(*io_handle, o_data, o_status);

                if (flags & INSTRUCTION_FLAG_SERVER_DEBUG)
                {
                    std::string words;
                    genWords(o_data, words);
                    snprintf(errstr, 200, "SERVER_DEBUG : spi_command() o_data = %s, rc = %zu)\n", words.c_str(), len);
                    o_status.errorMessage.append(errstr);
                }

                if (len != bytelen)
                {
                    snprintf(errstr, 200, "SPIInstruction::execute Problem using SPI device : errno %d, length exp (%zd) actual (%zd)\n", errno, bytelen, len);
                    o_status.errorMessage.append(errstr);
                    rc = o_status.rc = SERVER_SPI_COMMAND_FAIL;
                    spi_ffdc(io_handle, o_status);
                    break;
                }
                rc = o_status.rc = SERVER_COMMAND_COMPLETE;
            }
            break;

        default:
            rc = o_status.rc = SERVER_COMMAND_NOT_SUPPORTED;
            break;
      }

      return rc;
}

uint32_t SPIInstruction::flatten(uint8_t * o_data, uint32_t i_len) const
{
    uint32_t rc = 0;
    uint32_t * o_ptr = (uint32_t *) o_data;
  
    if (i_len < flattenSize())
    {
        out.error("SPIInstruction::flatten", "i_len %d bytes is too small to flatten\n", i_len);
        rc = 1;
    }
    else
    {
        // clear memory
        memset(o_data, 0, flattenSize());
        o_ptr[0] = htonl(version);
        o_ptr[1] = htonl(command);
        o_ptr[2] = htonl(flags);
        o_ptr[3] = htonl(engineId);
        o_ptr[4] = htonl(select);
        o_ptr[5] = htonl(busSpeed);
        o_ptr[6] = htonl(readLength);
        o_ptr[7] = htonl(msDelay);
        uint32_t deviceStringSize = deviceString.size() + 1;
        if (deviceStringSize % sizeof(uint32_t))
            deviceStringSize += (sizeof(uint32_t) - (deviceStringSize % sizeof(uint32_t)));
        o_ptr[8] = htonl(deviceStringSize);
        uint32_t dataSize = getFlattenSize(data);
        o_ptr[9] = htonl(dataSize);
        if (deviceString.size() > 0)
            strcpy(((char *)(o_ptr + 10)), deviceString.c_str());
        if (data)
            data->flatten((uint8_t *) (o_ptr + 10 + (deviceStringSize / sizeof(uint32_t))), dataSize);
    }

    return rc;
}

uint32_t SPIInstruction::unflatten(const uint8_t * i_data, uint32_t i_len)
{
    uint32_t rc = 0;
    uint32_t * i_ptr = (uint32_t *) i_data;

    version = ntohl(i_ptr[0]);
    if (version == 0x1)
    {
        command = (InstructionCommand) ntohl(i_ptr[1]);
        flags = ntohl(i_ptr[2]);
        engineId = ntohl(i_ptr[3]);
        select = ntohl(i_ptr[4]);
        busSpeed = ntohl(i_ptr[5]);
        readLength = ntohl(i_ptr[6]);
        msDelay = ntohl(i_ptr[7]);
        uint32_t deviceStringSize = ntohl(i_ptr[8]);
        uint32_t dataSize = ntohl(i_ptr[9]);
        ecmdDataBuffer * tempdata = new ecmdDataBuffer();
        mydata.push_back(tempdata);
        data = tempdata;
        rc = tempdata->unflatten((uint8_t *) (i_ptr + 10 + (deviceStringSize / sizeof(uint32_t))), dataSize);
        if (rc) { error = rc; }
        if (deviceStringSize > 0)
            deviceString = ((char *)(i_ptr + 10));
    }
    else
    {
        error = rc = SERVER_UNKNOWN_INSTRUCTION_VERSION;
    }
    return rc;
}

uint32_t SPIInstruction::flattenSize(void) const
{
    uint32_t size = 0;
    if (version >= 0x1)
    {
        size = 10 * sizeof(uint32_t); // version, command, flags, engineId, select, busSpeed, readLength, msDelay, dataSize, deviceStringSize
        uint32_t deviceStringSize = deviceString.size() + 1;
        if (deviceStringSize % sizeof(uint32_t))
            deviceStringSize += (sizeof(uint32_t) - (deviceStringSize % sizeof(uint32_t))); 
        size += deviceStringSize; // deviceString

        size += getFlattenSize(data); // data
    }
    return size;
}

std::string SPIInstruction::dumpInstruction(void) const
{
    std::ostringstream oss;
    oss << "SPIInstruction" << std::endl;
    oss << "version       : " << version << std::endl;
    oss << "command       : " << InstructionCommandToString(command) << std::endl;
    oss << "type          : " << InstructionTypeToString(type) << std::endl;
    oss << "flags         : " << InstructionFlagToString(flags) << std::endl;
    oss << "deviceString  : " << deviceString << std::endl;
    oss << "engineId      : " << std::hex << std::setw(8) << std::setfill('0') << engineId << std::dec << std::endl;
    oss << "select        : " << std::hex << std::setw(8) << std::setfill('0') << select << std::dec << std::endl;
    oss << "busSpeed      : " << busSpeed << std::endl;
    oss << "readLength    : " << std::dec << readLength << std::endl;
    oss << "msDelay       : " << std::dec << msDelay << std::endl;
    if (data)
    {
        oss << "data length   : " << data->getBitLength() << std::endl;
        oss << "data          : ";
        for(uint32_t j = 0; j < data->getWordLength(); j++)
        {
            oss << std::hex << std::setw(8) << std::setfill('0') << data->getWord(j) << " ";
            if (!((j+1) % 5)) oss << "\n\t\t";
        }
        oss << std::dec << std::endl;
    }

    return oss.str();
}

uint64_t SPIInstruction::getHash(void) const
{
    uint32_t devstrhash = 0x0;
    uint32_t rc = devicestring_genhash(deviceString, devstrhash);
    uint64_t hash64 = 0x0ull;
    hash64 |= ((0x0000000Full & type)     << 60);
    hash64 |= ((0x000000FFull & engineId) << 52);
    hash64 |= ((0x000000FFull & select)   << 46);
    if (rc == 0)
        hash64 |= ((uint64_t) devstrhash);
    return hash64;
}

uint32_t SPIInstruction::closeHandle(Handle ** i_handle)
{
    int rc = 0;

    /* Close the device */
    errno = 0;
    switch (command)
    {
        default:
        rc = spi_close(*i_handle);
        break;
    }
    *i_handle = NULL;
    if (rc) rc = SERVER_SPI_CLOSE_FAIL;

    return rc;
}

std::string SPIInstruction::getInstructionVars(const InstructionStatus & i_status) const
{
    std::ostringstream oss;

    oss << std::hex << std::setfill('0');
    oss << "rc: " << std::setw(8) << i_status.rc;
    oss << " devstr: " << deviceString;
    oss << " engineId: " << std::setw(8) << engineId;
    oss << " select: " << std::setw(8) << select;

    return oss.str();
}
