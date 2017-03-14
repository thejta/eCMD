#ifndef _ServerSBEFIFOInstruction_H
#define _ServerSBEFIFOInstruction_H
//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG

/**
 * @file ServerSBEFIFOInstruction.H
 * @brief Provides a class for SBEFIFO instructions to run on the fsp server
 *
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <SBEFIFOInstruction.H>

class ServerSBEFIFOInstruction : public SBEFIFOInstruction
{
    protected:
    uint32_t sbefifo_open(Handle ** handle, InstructionStatus & o_status);
    void sbefifo_ffdc_and_reset(Handle ** handle, InstructionStatus & o_status);
    uint32_t sbefifo_close(Handle * handle);
    ssize_t sbefifo_submit(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status, uint32_t & o_reply_wordcount);

};

#endif // _ServerFSIInstruction_H
