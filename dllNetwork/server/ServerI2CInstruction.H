#ifndef _ServerI2CInstruction_H
#define _ServerI2CInstruction_H
//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG

/**
 * @file ServerI2CInstruction.H
 * @brief Provides a class for I2C instructions to run on the fsp server
 *
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <I2CInstruction.H>

class ServerI2CInstruction : public I2CInstruction
{
    protected:
    uint32_t iic_open(Handle ** handle, InstructionStatus & o_status);
    void iic_ffdc(Handle ** handle, InstructionStatus & o_status);
    uint32_t iic_close(Handle * handle);

    uint32_t iic_config_speed(Handle * i_handle, InstructionStatus & o_status);
    uint32_t iic_config_slave_address(Handle * i_handle, InstructionStatus & o_status);
    uint32_t iic_config_device_width(Handle * i_handle, InstructionStatus & o_status);
    uint32_t iic_config_device_offset(Handle * i_handle, InstructionStatus & o_status);
    ssize_t iic_read(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status);
    ssize_t iic_write(Handle * i_handle, InstructionStatus & o_status);
};

#endif // _ServerI2CInstruction_H
