#ifndef _ServerGPIOInstruction_H
#define _ServerGPIOInstruction_H
//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG

/**
 * @file ServerGPIOInstruction.H
 * @brief Provides a class for GPIO instructions to run on the fsp server
 *
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <GPIOInstruction.H>

class ServerGPIOInstruction : public GPIOInstruction
{
    protected:
    uint32_t gpio_open(Handle ** handle, InstructionStatus & o_status);
    void gpio_ffdc(Handle ** handle, InstructionStatus & o_status);
    uint32_t gpio_close(Handle * handle);

    uint32_t gpio_set_mode(Handle * i_handle, InstructionStatus & o_status, gpioDioMode_t i_mode);
    uint32_t gpio_read_pin(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status);
    uint32_t gpio_read_input_pins(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status);
    uint32_t gpio_read_latch(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status);
    uint32_t gpio_write_pin(Handle * i_handle, InstructionStatus & o_status);
    uint32_t gpio_write_output_pins(Handle * i_handle, InstructionStatus & o_status);
    uint32_t gpio_read_config(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status);
    uint32_t gpio_write_config(Handle * i_handle, InstructionStatus & o_status);
    uint32_t gpio_write_config_set_bit(Handle * i_handle, InstructionStatus & o_status);
    uint32_t gpio_write_config_clear_bit(Handle * i_handle, InstructionStatus & o_status);
};

#endif // _ServerGPIOInstruction_H
