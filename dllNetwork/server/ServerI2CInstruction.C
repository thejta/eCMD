//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG

#include <ServerI2CInstruction.H>
#include <OutputLite.H>
#include <stdio.h>
#include <errno.h>

//FIXME remove these
Handle * system_iic_open(const char * device, int flags) { return NULL; }
#include <fcntl.h>

uint32_t ServerI2CInstruction::iic_open(Handle ** handle, InstructionStatus & o_status)
{
    uint32_t rc = 0;
    char device[50];
    char errstr[200];

    if(*handle != NULL)
      return rc;

    /* Open the device */
    if (flags & INSTRUCTION_FLAG_DEVSTR) {
        snprintf(device, 50, "/dev/iic/%sE%02dP%02d", deviceString.c_str(), engineId, port);
    } else {
        snprintf(device, 50, "/dev/iic/%d/%d/%d/%d", linkid, cfamid, engineId, port);
    }
    errno = 0;

    if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        snprintf(errstr, 200, "SERVER_DEBUG : system_iic_open(%s, O_RDWR | O_SYNC)\n", device);
        o_status.errorMessage.append(errstr);
    }

#ifdef TESTING
    TEST_PRINT("*handle = system_iic_open(%s, O_RDWR | O_SYNC);\n", device);
    *handle = (Handle *) 0x1;
#else
    *handle = system_iic_open(device, O_RDWR | O_SYNC);
#endif
    if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        snprintf(errstr, 200, "SERVER_DEBUG : system_iic_open() *handle = %p\n", *handle);
        o_status.errorMessage.append(errstr);
    }

    if (*handle == NULL) {
        snprintf(errstr, 200, "I2CInstruction::execute Problem opening I2C device %s : errno %d\n", device, errno);
        o_status.errorMessage.append(errstr);
        rc = o_status.rc = SERVER_I2C_OPEN_FAIL;
        return rc;
    }

    return rc;
}

void ServerI2CInstruction::iic_ffdc(Handle ** handle, InstructionStatus & o_status)
{
    // system_iic_ffdc_extract
    //  append to o_status.errorMessage
    // system_iic_ffdc_unlock

    uint32_t rc = iic_close(*handle);
    *handle = NULL;
    if (rc) o_status.rc = SERVER_I2C_CLOSE_FAIL;
}

uint32_t ServerI2CInstruction::iic_close(Handle * handle)
{
    // close iic device
    printf("system_iic_close()\n");
    return 0;
}

uint32_t ServerI2CInstruction::iic_config_speed(Handle * i_handle, InstructionStatus & o_status)
{
#if 0
        /* Config the device */
        /* Bus speed */
        if (busSpeed == 100) {
          cfg_val = ADAL_IIC_VAL_100KHZ;
        } else if (busSpeed == 400) {
          cfg_val = ADAL_IIC_VAL_400KHZ;
        } else {
          snprintf(errstr, 200, "I2CInstruction::execute Bus speed %d is not known\n", busSpeed);
          o_status.errorMessage.append(errstr);
          rc = o_status.rc = SERVER_IIC_SPEED_INVALID;
          iic_ffdc(io_handle, o_status);
          break;
        }
#endif

    return 0;
}

uint32_t ServerI2CInstruction::iic_config_slave_address(Handle * i_handle, InstructionStatus & o_status)
{
    return 0;
}

uint32_t ServerI2CInstruction::iic_config_device_width(Handle * i_handle, InstructionStatus & o_status)
{
#if 0
        /* Offset field size */
        if (offsetFieldSize == 0) {
          cfg_val = ADAL_IIC_VAL_0BYTE;
        } else if (offsetFieldSize == 1) {
          cfg_val = ADAL_IIC_VAL_1BYTE;
        } else if (offsetFieldSize == 2) {
          cfg_val = ADAL_IIC_VAL_2BYTE;
        } else if (offsetFieldSize == 3) {
          cfg_val = ADAL_IIC_VAL_3BYTE;
        } else if (offsetFieldSize == 4) {
          cfg_val = ADAL_IIC_VAL_4BYTE;
        } else {
          snprintf(errstr, 200, "I2CInstruction::execute Invalid offsetFieldSize: %d is not supported\n", offsetFieldSize);
          o_status.errorMessage.append(errstr);
          rc = o_status.rc = SERVER_IIC_INVALID_OFFSET_SIZE;
          iic_ffdc(io_handle, o_status);
          break;
        }
#endif

    return 0;
}

uint32_t ServerI2CInstruction::iic_config_device_offset(Handle * i_handle, InstructionStatus & o_status)
{
    return 0;
}

ssize_t ServerI2CInstruction::iic_read(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status)
{
    return length % 8 ? (length / 8) + 1 : length / 8;
}
ssize_t ServerI2CInstruction::iic_write(Handle * i_handle, InstructionStatus & o_status)
{
    return length % 8 ? (length / 8) + 1 : length / 8;
}
