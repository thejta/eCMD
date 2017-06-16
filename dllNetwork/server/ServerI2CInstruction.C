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

#include <ServerI2CInstruction.H>
#include <OutputLite.H>
#include <stdio.h>
#include <errno.h>
#include <adal_iic.h>

// commented out because of conflict with iic_dd.h
//#include <linux/i2c-dev.h>
//#include <linux/i2c.h>
// copied from i2c-dev.h
#define I2C_SLAVE	0x0703	/* Use this slave address */
#define I2C_TENBIT	0x0704	/* 0 for 7 bit addrs, != 0 for 10 bit */
#define I2C_FUNCS	0x0705	/* Get the adapter functionality mask */
#define I2C_RDWR	0x0707	/* Combined R/W transfer (one STOP only) */
// copied from i2c.h
#define I2C_FUNC_I2C                    0x00000001
#define I2C_FUNC_10BIT_ADDR             0x00000002

#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#ifdef SERVER_PRINT_PERF
#include <sys/time.h>
#endif

/* For this code device string is used to build file paths of valid i2c devices */
/* this include fsi i2c and native i2c devices */
/* we'll determine a valid device by what we find in the filesystem */

struct system_iic
{
    int fd;
    uint32_t slave_address;
    uint32_t slave_address_mask;
    uint8_t offset_width;
    uint32_t offset;
    uint32_t funcs;
};

system_iic * system_iic_open(const char * device, int flags)
{
    int fd = 0;
    fd = open(device, flags);
    if (fd < 0)
    {
        return NULL;
    }

    system_iic * ret = new system_iic;
    ret->fd = fd;
    return ret;
}

void system_iic_close(system_iic * handle)
{
    if (handle != NULL)
    {
        close(handle->fd);
        delete handle;
    }
}

uint32_t ServerI2CInstruction::iic_open(Handle ** handle, InstructionStatus & o_status)
{
    uint32_t rc = 0;
    char device[50];
    char errstr[200];
    int fd = 0;

    if(*handle != NULL)
      return rc;

    /* Open the device */
    if ((flags & INSTRUCTION_FLAG_DEVSTR) == 0) {
        //ERROR
        *handle = NULL;
        snprintf(errstr, 200, "ServerI2CInstruction::iic_open INSTRUCTION_FLAG_DEVSTR must be set\n");
        o_status.errorMessage.append(errstr);
        return SERVER_I2C_OPEN_FAIL;
    }

    if (iv_devicePaths.size() > 0)
    {
        iv_devicePaths.clear();
    }

    uint32_t deviceId = std::stoul( deviceString );

    snprintf(device, 50, "/dev/i2c-%d", ((deviceId * 100) + port));
    iv_devicePaths.push_back(device);
    snprintf(device, 50, "/dev/i2cfsi%d.%02d", deviceId, port);
    iv_devicePaths.push_back(device);
    iv_adalFsiDevice = 1;

    for( iv_deviceIdx=0; iv_deviceIdx < iv_devicePaths.size(); iv_deviceIdx++ )
    {
        fd = open(iv_devicePaths[iv_deviceIdx].c_str(), O_RDONLY);
        if (fd != -1)
        {
            close(fd);
            break;
        }
        close(fd);
    }

    if ( iv_deviceIdx >= iv_devicePaths.size() )
    {
        *handle = NULL;
        for( iv_deviceIdx=0; iv_deviceIdx < iv_devicePaths.size(); iv_deviceIdx++ )
        {
            snprintf(errstr, 200, "ServerI2CInstruction::iic_open invalid device open attempt %s\n", iv_devicePaths[iv_deviceIdx].c_str());
            o_status.errorMessage.append(errstr);
        }
        return SERVER_I2C_OPEN_FAIL;
    }

    // iv_deviceIdx contains the index into iv_devicePaths that points to a file that exists and we can open
    errno = 0;

    if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        if ( iv_deviceIdx == iv_adalFsiDevice )
            snprintf(errstr, 200, "SERVER_DEBUG : adal_iic_open(%s, O_RDWR | O_SYNC)\n", iv_devicePaths[iv_deviceIdx].c_str());
        else
            snprintf(errstr, 200, "SERVER_DEBUG : system_iic_open(%s, O_RDWR | O_SYNC)\n", iv_devicePaths[iv_deviceIdx].c_str());
        o_status.errorMessage.append(errstr);
    }

#ifdef TESTING
    if ( iv_deviceIdx == iv_adalFsiDevice )
        TEST_PRINT("*handle = adal_iic_open(%s, O_RDWR | O_SYNC);\n", iv_devicePaths[iv_deviceIdx].c_str());
    else
        TEST_PRINT("*handle = system_iic_open(%s, O_RDWR | O_SYNC);\n", iv_devicePaths[iv_deviceIdx].c_str());
    *handle = (Handle *) 0x1;
#else
    if ( iv_deviceIdx == iv_adalFsiDevice )
        *handle = (Handle *) adal_iic_open(iv_devicePaths[iv_deviceIdx].c_str(), O_RDWR | O_SYNC);
    else
        *handle = (Handle *) system_iic_open(iv_devicePaths[iv_deviceIdx].c_str(), O_RDWR | O_SYNC);
#endif
    if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        if ( iv_deviceIdx == iv_adalFsiDevice )
            snprintf(errstr, 200, "SERVER_DEBUG : adal_iic_open() *handle = %p\n", *handle);
        else
            snprintf(errstr, 200, "SERVER_DEBUG : system_iic_open() *handle = %p\n", *handle);
        o_status.errorMessage.append(errstr);
    }

    if ( iv_deviceIdx != iv_adalFsiDevice )
    {
        // Test if I2C_FUNC_I2C is available
        uint32_t funcs = 0;
        system_iic * l_handle = (system_iic *) *handle;
        rc = ioctl(l_handle->fd, I2C_FUNCS, &funcs);
        l_handle->funcs = funcs;
        if ((l_handle->funcs & I2C_FUNC_I2C) == 0)
        {
            snprintf(errstr, 200, "ServerI2CInstruction::iic_open I2C_FUNC_I2C not supported, funcs = %08X\n", l_handle->funcs);
            o_status.errorMessage.append(errstr);
            system_iic_close(l_handle);
            *handle = NULL;
            return SERVER_I2C_OPEN_FAIL;
        }
    }

    if (*handle == NULL) {
        snprintf(errstr, 200, "I2CInstruction::execute Problem opening I2C device %s : errno %d\n", iv_devicePaths[iv_deviceIdx].c_str(), errno);
        o_status.errorMessage.append(errstr);
        rc = o_status.rc = SERVER_I2C_OPEN_FAIL;
        return rc;
    }

    return rc;
}

void ServerI2CInstruction::iic_ffdc(Handle ** handle, InstructionStatus & o_status)
{
    // adal_iic_ffdc_extract
    //  append to o_status.errorMessage
    // adal_iic_ffdc_unlock

    uint32_t rc = iic_close(*handle);
    *handle = NULL;
    if (rc) o_status.rc = SERVER_I2C_CLOSE_FAIL;
}

uint32_t ServerI2CInstruction::iic_close(Handle * handle)
{
    uint32_t rc = 0;
    // close iic device
#ifdef TESTING
    if ( iv_deviceIdx == iv_adalFsiDevice )
        TEST_PRINT("adal_iic_close()\n");
    else
        TEST_PRINT("system_iic_close()\n");
#else
    if ( iv_deviceIdx == iv_adalFsiDevice )
        rc = adal_iic_close((adal_t *) handle);
    else
        system_iic_close((system_iic *) handle);
#endif
    return rc;
}

uint32_t ServerI2CInstruction::iic_config_speed(Handle * i_handle, InstructionStatus & o_status)
{
    uint32_t rc = 0;
#if 0
    int cfg_val = 0;
    char errstr[200];

    switch (busSpeed)
    {
        case 100:
            cfg_val = ADAL_IIC_VAL_100KHZ;
            break;
        case 400:
            cfg_val = ADAL_IIC_VAL_400KHZ;
            break;
        default:
            snprintf(errstr, 200, "ServerI2CInstruction::iic_config_speed Bus speed %d is not known\n", busSpeed);
            o_status.errorMessage.append(errstr);
            rc = SERVER_IIC_SPEED_INVALID;
            break;
    }

    if (rc == 0)
    {
#ifdef TESTING
        TESTPRINT("rc = adal_iic_config((adal_t *) i_handle, ADAL_CONFIG_WRITE, ADAL_IIC_CFG_SPEED, %x, 0);\n", cfg_val);
#else
        rc = adal_iic_config((adal_t *) i_handle, ADAL_CONFIG_WRITE, ADAL_IIC_CFG_SPEED, &cfg_val, 0);
#endif
    }
#endif
    return rc;
}

uint32_t ServerI2CInstruction::iic_config_slave_address(Handle * i_handle, InstructionStatus & o_status)
{
    uint32_t rc = 0;
    if ( iv_deviceIdx == iv_adalFsiDevice )
#ifdef TESTING
        TESTPRINT("rc = adal_iic_config((adal_t *) i_handle, ADAL_CONFIG_WRITE, ADAL_IIC_CFG_DEV_ADDR, %x, 0);\n", slaveAddress);
#else
        rc = adal_iic_config((adal_t *) i_handle, ADAL_CONFIG_WRITE, ADAL_IIC_CFG_DEV_ADDR, &slaveAddress, 0);
#endif
    else
    {
#ifdef TESTING
        TESTPRINT("ioctl(handle->fd, I2C_TENBIT, 1) if tenbit supported\n");
        TESTPRINT("ioctl(handle->fd, I2C_SLAVE, handle->slave_address)\n");
#else
        errno = 0;
        system_iic * handle = (system_iic *) i_handle;
        if ( (handle->funcs & I2C_FUNC_10BIT_ADDR) == I2C_FUNC_10BIT_ADDR )
        {
            rc = ioctl(handle->fd, I2C_TENBIT, 1);
            if ( rc ) return rc;
            handle->slave_address = slaveAddress;
            handle->slave_address_mask = 0x3FF;
        }
        else
        {
            handle->slave_address = slaveAddress >> 1; // right shift to a 7-bit address
            handle->slave_address_mask = 0x7F;
        }

        rc = ioctl(handle->fd, I2C_SLAVE, handle->slave_address & handle->slave_address_mask);
#endif
    } 
    return rc;
}

uint32_t ServerI2CInstruction::iic_config_device_width(Handle * i_handle, InstructionStatus & o_status)
{
    uint32_t rc = 0;
    int cfg_val = 0;
    char errstr[200];

    if ( iv_deviceIdx == iv_adalFsiDevice )
    {
        switch (offsetFieldSize)
        {
            case 0:
                cfg_val = ADAL_IIC_VAL_0BYTE;
                break;
            case 1:
                cfg_val = ADAL_IIC_VAL_1BYTE;
                break;
            case 2:
                cfg_val = ADAL_IIC_VAL_2BYTE;
                break;
            case 3:
                cfg_val = ADAL_IIC_VAL_3BYTE;
                break;
            case 4:
                cfg_val = ADAL_IIC_VAL_4BYTE;
                break;
            default:
                snprintf(errstr, 200, "ServerI2CInstruction::iic_config_device_width Invalid offsetFieldSize: %d is not supported\n", offsetFieldSize);
                o_status.errorMessage.append(errstr);
                rc = SERVER_IIC_INVALID_OFFSET_SIZE;
        }

        if (rc == 0)
        {
#ifdef TESTING
            TESTPRINT("rc = adal_iic_config((adal_t *) i_handle, ADAL_CONFIG_WRITE, ADAL_IIC_CFG_DEV_WIDTH, %x, 0);\n", cfg_val);
#else
            rc = adal_iic_config((adal_t *) i_handle, ADAL_CONFIG_WRITE, ADAL_IIC_CFG_DEV_WIDTH, &cfg_val, 0);
#endif
        }
    } 
    else
    {
        system_iic * handle = (system_iic *) i_handle;
        handle->offset_width = offsetFieldSize;
        if (handle->offset_width > 4)
        {
            char errstr[200];
            snprintf(errstr, 200, "ServerI2CInstruction::iic_config_device_width Invalid offsetFieldSize: %d is not supported\n", handle->offset_width);
            o_status.errorMessage.append(errstr);
            rc = SERVER_IIC_INVALID_OFFSET_SIZE;
        }
    }

    return rc;
}

uint32_t ServerI2CInstruction::iic_config_device_offset(Handle * i_handle, InstructionStatus & o_status)
{
    uint32_t rc = 0;
    if ( iv_deviceIdx == iv_adalFsiDevice )
    {
#ifdef TESTING
        TESTPRINT("rc = adal_iic_config((adal_t *) i_handle, ADAL_CONFIG_WRITE, ADAL_IIC_CFG_OFFSET, %x, 0);\n", offset);
#else
        rc = adal_iic_config((adal_t *) i_handle, ADAL_CONFIG_WRITE, ADAL_IIC_CFG_OFFSET, &offset, 0);
#endif
    }
    else
    {
        system_iic * handle = (system_iic *) i_handle;
        handle->offset = offset;
    }
    return rc;
}

ssize_t ServerI2CInstruction::iic_read(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status)
{
#ifdef SERVER_PRINT_PERF
    struct timeval start_time;
    struct timeval end_time;
#endif

    if ( iv_deviceIdx == iv_adalFsiDevice )
    {
        ssize_t len = length % 8 ? (length / 8) + 1 : length / 8;

        uint8_t * buffer = new uint8_t[len + 4];

        // adjust timeout value based on data size
        int timeout = len * 160 * 2 + 1000;
#ifdef SERVER_PRINT_PERF
        printf("bytes: %d, timeout: %d\n", len, timeout);
#endif
#ifdef TESTING
        TESTPRINT("rc = adal_iic_config((adal_t *) i_handle, ADAL_CONFIG_WRITE, ADAL_IIC_CFG_TIMEOUT, %x, 0);\n", timeout);
#else
        ssize_t rc = adal_iic_config((adal_t *) i_handle, ADAL_CONFIG_WRITE, ADAL_IIC_CFG_TIMEOUT, &timeout, 0);
        if (rc)
        {
            delete [] buffer;
	    return rc;
        }
#endif

#ifdef SERVER_PRINT_PERF
        gettimeofday(&start_time, NULL);
#endif
#ifdef TESTING
        TESTPRINT("len = adal_iic_read((adal_t *) i_handle, buffer, len);\n");
#else
        len = adal_iic_read((adal_t *) i_handle, buffer, len);
#endif
#ifdef SERVER_PRINT_PERF
        gettimeofday(&end_time, NULL);
        int perf_run_time = (end_time.tv_sec - start_time.tv_sec) * 1000 + ((end_time.tv_usec - start_time.tv_usec) / 1000);
        printf("time: %d, rc %d\n", perf_run_time, len);
#endif

#if 0
	for (int i = 0; i < (len + 4); i++)
	{
		printf("%02X ", (uint32_t) buffer[i]);
	}
	printf ("\n");
#endif

        uint32_t rc_ecmd = o_data.insert(buffer, 0, length);
        if (rc_ecmd)
        {
            printf("insert error %d\n", rc_ecmd);
        }
        delete [] buffer;

        return len;
    }
    else
    {
        system_iic * handle = (system_iic *) i_handle;
        uint32_t bytes = length % 8 ? (length / 8) + 1 : length / 8;

        uint32_t messages = 1;
        // check if we need to write address to the bus before the write
        if (handle->offset_width > 0)
        {
            messages = 2;
        }

        i2c_rdwr_ioctl_data * rdwr_data = new i2c_rdwr_ioctl_data;
        rdwr_data->msgs = new i2c_msg[messages];
        rdwr_data->nmsgs = messages;

        uint32_t msg_offset = 0;

        if (messages > 1)
        {
            // set up write message
            rdwr_data->msgs[msg_offset].addr = handle->slave_address;
            rdwr_data->msgs[msg_offset].flags = 0;
            rdwr_data->msgs[msg_offset].len = handle->offset_width;
            rdwr_data->msgs[msg_offset].buf = new uint8_t[rdwr_data->msgs[msg_offset].len];
            // copy in offset data in bytes
            for (uint8_t buf_offset = 0; buf_offset < handle->offset_width; buf_offset++)
            {
                rdwr_data->msgs[msg_offset].buf[buf_offset] =
		    (handle->offset >> ((handle->offset_width - buf_offset - 1) * 8)) & 0xFF;
            }

            msg_offset++;
        }

        // set up read message
        rdwr_data->msgs[msg_offset].addr = handle->slave_address;
        rdwr_data->msgs[msg_offset].flags = I2C_M_RD;
        rdwr_data->msgs[msg_offset].len = bytes;
        rdwr_data->msgs[msg_offset].buf = new uint8_t[bytes];

        errno = 0;
        int rc = 0;
#ifdef TESTING
        TESTPRINT("rc = ioctl(handle->fd, I2C_RDWR, rdwr_data);\n");
#else
        rc = ioctl(handle->fd, I2C_RDWR, rdwr_data);
#endif

        if (rc >= 0)
        {
           // copy out data
           o_data.insert(rdwr_data->msgs[msg_offset].buf, 0, length);
           rc = bytes;
        }
        else
        {
            perror("i2c read");
        }

        for (msg_offset = 0; msg_offset < messages; msg_offset++)
        {
            delete [] rdwr_data->msgs[msg_offset].buf;
        }
        delete [] rdwr_data->msgs;
        delete rdwr_data;

        return rc;
    }
}

ssize_t ServerI2CInstruction::iic_write(Handle * i_handle, InstructionStatus & o_status)
{
#ifdef SERVER_PRINT_PERF
    struct timeval start_time;
    struct timeval end_time;
#endif

    if ( iv_deviceIdx == iv_adalFsiDevice )
    {
        ssize_t len = length % 8 ? (length / 8) + 1 : length / 8;

        uint8_t * buffer = new uint8_t[len + 4];
        memset(buffer, 0x00, len + 4);
        data.extract(buffer, 0, length);

        // adjust timeout value based on data size
        int timeout = len * 160 * 2 + 1000;
#ifdef SERVER_PRINT_PERF
        printf("bytes: %d, timeout: %d\n", len, timeout);
#endif
#ifdef TESTING
        TESTPRINT("rc = adal_iic_config((adal_t *) i_handle, ADAL_CONFIG_WRITE, ADAL_IIC_CFG_TIMEOUT, %x, 0);\n", timeout);
#else
        ssize_t rc = adal_iic_config((adal_t *) i_handle, ADAL_CONFIG_WRITE, ADAL_IIC_CFG_TIMEOUT, &timeout, 0);
        if (rc)
        {
            delete [] buffer;
	    return rc;
        }
#endif

#ifdef SERVER_PRINT_PERF
        gettimeofday(&start_time, NULL);
#endif
#ifdef TESTING
        TESTPRINT("len = adal_iic_write((adal_t *) i_handle, buffer), len);\n");
#else
        len = adal_iic_write((adal_t *) i_handle, buffer, len);
#endif
#ifdef SERVER_PRINT_PERF
        gettimeofday(&end_time, NULL);
        int perf_run_time = (end_time.tv_sec - start_time.tv_sec) * 1000 + ((end_time.tv_usec - start_time.tv_usec) / 1000);
        printf("time: %d, rc %d\n", perf_run_time, len);
#endif
        delete [] buffer;

        return len;
    }
    else
    {
        system_iic * handle = (system_iic *) i_handle;
        uint32_t bytes = length % 8 ? (length / 8) + 1 : length / 8;

        uint32_t messages = 1;

        i2c_rdwr_ioctl_data * rdwr_data = new i2c_rdwr_ioctl_data;
        rdwr_data->msgs = new i2c_msg[messages];
        rdwr_data->nmsgs = messages;

        uint32_t msg_offset = 0;

        // set up write message
        rdwr_data->msgs[msg_offset].addr = handle->slave_address;
        rdwr_data->msgs[msg_offset].flags = 0;
        rdwr_data->msgs[msg_offset].len = handle->offset_width + bytes;
        rdwr_data->msgs[msg_offset].buf = new uint8_t[rdwr_data->msgs[msg_offset].len];
        // copy in offset data in bytes
        for (uint8_t buf_offset = 0; buf_offset < handle->offset_width; buf_offset++)
        {
            rdwr_data->msgs[msg_offset].buf[buf_offset] =
                (handle->offset >> ((handle->offset_width - buf_offset - 1) * 8)) & 0xFF;
        }
        // copy in message data
        data.extract(rdwr_data->msgs[msg_offset].buf + handle->offset_width, 0, length);

        int rc = 0;
#ifdef TESTING
        TESTPRINT("rc = ioctl(handle->fd, I2C_RDWR, rdwr_data);\n");
#else
        rc = ioctl(handle->fd, I2C_RDWR, rdwr_data);
#endif

        if (rc >= 0)
        {
           rc = bytes;
        }

        for (msg_offset = 0; msg_offset < messages; msg_offset++)
        {
            delete [] rdwr_data->msgs[msg_offset].buf;
        }
        delete [] rdwr_data->msgs;
        delete rdwr_data;

        return rc;
    } 
}
