#ifndef _ServerFSIInstruction_H
#define _ServerFSIInstruction_H
//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG

/**
 * @file ServerFSIInstruction.H
 * @brief Provides a class for FSI instructions to run on the fsp server
 *
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <FSIInstruction.H>

class ServerFSIInstruction : public FSIInstruction
{
    protected:
    uint32_t scan_open(Handle ** handle, InstructionStatus & o_status);
    uint32_t scom_open(Handle ** handle, InstructionStatus & o_status);
    uint32_t gp_reg_open(Handle ** handle, InstructionStatus & o_status);
    uint32_t mbx_open(Handle ** handle, InstructionStatus & o_status);

    void scan_ffdc_and_reset(Handle ** handle, InstructionStatus & o_status);
    void scom_ffdc_and_reset(Handle ** handle, InstructionStatus & o_status);
    void gp_reg_ffdc_and_reset(Handle ** handle, InstructionStatus & o_status);
    void mbx_ffdc_and_reset(Handle ** handle, InstructionStatus & o_status);

    uint32_t scan_close(Handle * handle);
    uint32_t scom_close(Handle * handle);
    uint32_t gp_reg_close(Handle * handle);
    uint32_t mbx_close(Handle * handle);

    ssize_t scan_write(Handle * i_handle, InstructionStatus & o_status);
    ssize_t scan_read(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status);
    ssize_t scom_write(Handle * i_handle, InstructionStatus & o_status, uint32_t i_index = 0);
    ssize_t scom_write_under_mask(Handle * i_handle, InstructionStatus & o_status);
    ssize_t scom_read(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status, uint32_t i_index = 0);

    ssize_t scan_get_register(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status);
    ssize_t scom_get_register(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status);
    ssize_t mbx_get_scratch_register(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status);
    ssize_t mbx_get_register(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status);
    ssize_t gp_reg_get(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status);

    ssize_t scan_set_register(Handle * i_handle, InstructionStatus & o_status);
    ssize_t scom_set_register(Handle * i_handle, InstructionStatus & o_status);
    ssize_t mbx_set_scratch_register(Handle * i_handle, InstructionStatus & o_status);
    ssize_t mbx_set_register(Handle * i_handle, InstructionStatus & o_status);
    ssize_t gp_reg_set(Handle * i_handle, InstructionStatus & o_status);
};

#endif // _ServerFSIInstruction_H
