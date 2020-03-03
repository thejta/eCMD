//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2003,2018 IBM International Business Machines Corp.
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

#include <ServerSPIInstruction.H>
#include <OutputLite.H>
#include <stdio.h>
#include <errno.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <linux/spi/spidev.h>
#include <delay.h>

extern bool global_server_debug;

uint32_t ServerSPIInstruction::spi_open(Handle ** handle, InstructionStatus & o_status)
{
    uint32_t rc = 0;
    char device[50];
    char errstr[200];

    if(*handle != NULL)
      return rc;

    /* Open the device */

    snprintf(device, 50, "/dev/spidev%d.%d", engineId, select);

    errno = 0;

    if (flags & INSTRUCTION_FLAG_SERVER_DEBUG)
    {
        snprintf(errstr, 200, "SERVER_DEBUG : open(%s, O_RDWR | O_SYNC)\n", device);
        o_status.errorMessage.append(errstr);
    }

#ifdef TESTING
    TEST_PRINT("*handle = open(%s, O_RDWR | O_SYNC);\n", device);
    *handle = (Handle *) 55;
#else
    *handle = (Handle *) open(device, O_RDWR | O_SYNC);
#endif
    if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        snprintf(errstr, 200, "SERVER_DEBUG : open() *handle = %p\n", *handle);
        o_status.errorMessage.append(errstr);
    }

    if (*handle < 0) {
        snprintf(errstr, 200, "ServerSPIInstruction::spi_open Problem opening SPI device %s : errno %d\n", device, errno);
        o_status.errorMessage.append(errstr);
        rc = o_status.rc = SERVER_SPI_OPEN_FAIL;
        return rc;
    }

    return rc;
}

void ServerSPIInstruction::spi_ffdc(Handle ** handle, InstructionStatus & o_status)
{
    // adal_iic_ffdc_extract
    //  append to o_status.errorMessage
    // adal_iic_ffdc_unlock

    uint32_t rc = spi_close(*handle);
    *handle = NULL;
    if (rc) o_status.rc = SERVER_SPI_CLOSE_FAIL;
}

uint32_t ServerSPIInstruction::spi_close(Handle * handle)
{
    uint32_t rc = 0;
    // close iic device
#ifdef TESTING
    TEST_PRINT("close()\n");
#else
    close((int) handle);
#endif
    return rc;
}

ssize_t ServerSPIInstruction::spi_command(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status)
{
    ssize_t rc = 0;
    char errstr[200];

    int fd = (int) i_handle;
    uint32_t readBytes = readLength % 8 ? (readLength / 8) + 1 : readLength / 8;
    struct spi_ioc_transfer * xfer = NULL;
    uint32_t l_messages = 0;

    do
    {
        if (flags & INSTRUCTION_FLAG_SPI_FULL_DUPLEX)
        {
            if ((data == NULL) || (data->getBitLength() == 0))
            {
                snprintf(errstr, 200, "ServerSPIInstruction::spi_command no data for full duplex message\n");
                o_status.errorMessage.append(errstr);
                rc = -1;
                break;
            }

            if (data->getBitLength() != readLength)
            {
                snprintf(errstr, 200, "ServerSPIInstruction::spi_command full duplex message not symmetric\n");
                o_status.errorMessage.append(errstr);
                rc = -1;
                break;
            }

            l_messages = 1;
        }
        else
        {
            if ((data != NULL) && data->getBitLength())
                l_messages++;
            if (readLength)
                l_messages++;
        }

        if (l_messages == 0)
        {
            snprintf(errstr, 200, "ServerSPIInstruction::spi_command no messages to send\n");
            o_status.errorMessage.append(errstr);
            rc = -1;
            break;
        }

        xfer = new spi_ioc_transfer[l_messages];

        if (xfer == NULL)
        {
            snprintf(errstr, 200, "ServerSPIInstruction::spi_command could not allocate xfer\n");
            o_status.errorMessage.append(errstr);
            rc = -1;
            break;
        }

        memset(xfer, 0, sizeof(spi_ioc_transfer) * l_messages);

        if ((data != NULL) && data->getBitLength())
        {
            xfer[0].tx_buf = (unsigned long) new uint8_t[data->getByteLength()];
            if (xfer[0].tx_buf == 0)
            {
                snprintf(errstr, 200, "ServerSPIInstruction::spi_command could not allocate tx_buf\n");
                o_status.errorMessage.append(errstr);
                rc = -1;
                break;
            }

            uint32_t rc_ecmd = data->extract((uint8_t *) xfer[0].tx_buf, 0, data->getBitLength());
            if (rc_ecmd)
            {
                snprintf(errstr, 200, "ServerSPIInstruction::spi_command error during extract: %08X\n", rc_ecmd);
                o_status.errorMessage.append(errstr);
                rc = -1;
                break;
            }
            xfer[0].len = data->getByteLength();

        }

        if (readLength)
        {
            uint32_t l_msg_offset = 0;
            if (l_messages > 1)
                l_msg_offset = 1;

            xfer[l_msg_offset].rx_buf = (unsigned long) new uint8_t[readBytes];
            if (xfer[l_msg_offset].rx_buf == 0)
            {
                snprintf(errstr, 200, "ServerSPIInstruction::spi_command could not allocate rx_buf\n");
                o_status.errorMessage.append(errstr);
                rc = -1;
                break;
            }
            xfer[l_msg_offset].len = readBytes;
        }

        rc = ioctl(fd, SPI_IOC_MESSAGE(l_messages), xfer);

        if (rc < 0)
        {
            snprintf(errstr, 200, "ServerSPIInstruction::spi_command error during ioctl: errno: %d\n", errno);
            o_status.errorMessage.append(errstr);
            break;
        }

        if (readLength)
        {
            uint32_t l_msg_offset = 0;
            if (l_messages > 1)
                l_msg_offset = 1;

            uint32_t rc_ecmd = o_data.insert((uint8_t *) xfer[l_msg_offset].rx_buf, 0, readLength);
            if (rc_ecmd)
            {
                snprintf(errstr, 200, "ServerSPIInstruction::spi_command error during insert: %08X\n", rc_ecmd);
                o_status.errorMessage.append(errstr);
                rc = -1;
                break;
            }
        }

        if (msDelay)
        {
            uint64_t l_nsDelay = 1000000 * msDelay;
            delay(l_nsDelay);
        }

        rc = readBytes;
    } while (0);

    for (uint32_t l_msg_offset = 0; l_msg_offset < l_messages; l_msg_offset++)
    {
        if (xfer[l_msg_offset].tx_buf)
            delete [] (uint8_t *) xfer[l_msg_offset].tx_buf;
        if (xfer[l_msg_offset].rx_buf)
            delete [] (uint8_t *) xfer[l_msg_offset].rx_buf;
    }
    if (xfer)
        delete [] xfer;

    return rc;
}
