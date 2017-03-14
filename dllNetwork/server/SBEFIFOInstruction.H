#ifndef _SBEFIFOInstruction_H
#define _SBEFIFOInstruction_H
/* $Header$ */
// Copyright **********************************************************
//                                                                      
// File SBEFIFOInstruction.H                                               
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 2015
//                                                                      
// The source code for this program is not published or otherwise       
// divested of its trade secrets, irrespective of what has been         
// deposited with the U.S. Copyright Office.                             
//                                                                      
// End Copyright ******************************************************

/**
 * @file SBEFIFOInstruction.H
 * @brief Provides a class for SBEFIFO instructions being passed to the fsp server
 *
*/


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <Instruction.H>
#include <unistd.h>

/**
 @brief SBEFIFOInstruction class
*/
class SBEFIFOInstruction : public Instruction {
  public:
    /** @name SBEFIFOInstruction Constructors */
    //@{
    /**
     * @brief Default Constructor
     * @post Instruction() is called, version is set to 0x1, and type is set to SBEFIFO 
     */
    SBEFIFOInstruction(void);

    /**
     * @brief Default Destructor
     */
    ~SBEFIFOInstruction(void);
    //@}

    // Member Functions
    /** @name Setup Function */
    //@{
    /**
     * @brief Used as alternate way to set variables normally set on construction
     * @retval ECMD_SUCCESS on success
     * @retval nonzero on failure
     * @post command, deviceString, timeout, replyLength and flags are set, if i_data is not NULL it is copied to data
     * @post flag has INSTRUCTION_FLAG_DEVSTR set
     */
    virtual uint32_t setup(InstructionCommand i_command, std::string &i_deviceString, uint32_t i_timeout, uint32_t i_replyLength, uint32_t i_flags, ecmdDataBuffer * i_data = NULL);
    //@}

    /** @name Execution Function */
    //@{
    /**
     * @brief Called by the fsp server or sim transfer class
     * @param o_data ecmdDataBuffer with output data
     * @param o_status InstructionStatus with error information
     * @param io_handle Handle * pointer for handle reuse between instructions
     * @retval ECMD_SUCCESS on success
     * @retval nonzero on failure
     * @post executes the appropriate command and sets o_data and o_status accordingly
     * Operates on the following InstructionCommand types: SUBMIT, REQUEST_RESET
     */
    uint32_t execute(ecmdDataBuffer & o_data, InstructionStatus & o_status, Handle ** io_handle);
    //@}

    /** @name Transport Functions */
    //@{
    /**
     * @brief Flatten all the object data into a uint8_t buffer
     * @param o_data Byte buffer to write the flattened data to
     * @param i_len Number of bytes in the o_data buffer
     * @post o_data buffer has a flattened version of the SBEFIFOInstruction - must be pre-allocated
     * Data format (all in network byte order):
     * First Word:      version
     * Second Word:     command
     * Third Word:      flags
     * Fourth Word:     timeout
     * Fifth Word:      deviceString size
     * Sixth Word:      data size
     * Multiple Words:  deviceString
     * Multiple Words:  data
     * ======= // SIM only
     * n - 2 Word:      cage
     * n - 1 Word:      node
     * n Word:          slot
     */
    uint32_t flatten(uint8_t * o_data, uint32_t i_len) const;

    /**
     * @brief Unflatten object data from a uint8_t buffer into this SBEFIFOInstruction
     * @param i_data Byte buffer to read the flattened data from
     * @param i_len Number of bytes in the i_data buffer
     * @post This SBEFIFOInstruction is allocated and initialized with the unflattened version of i_data
     * Data format (all in network byte order):
     * First Word:      version
     * Second Word:     command
     * Third Word:      flags
     * Fourth Word:     timeout
     * Fifth Word:      deviceString size
     * Sixth Word:      data size
     * Multiple Words:  deviceString
     * Multiple Words:  data
     * ======= // SIM only
     * n - 2 Word:      cage
     * n - 1 Word:      node
     * n Word:          slot
     */
    uint32_t unflatten(const uint8_t * i_data, uint32_t i_len);

    /**
     * @brief Return number of bytes needed for a buffer to flatten the object
     * @retval Number of bytes needed
     */
    uint32_t flattenSize(void) const;
    //@}

    /** @name Debug Function */
    //@{
    /**
     * @brief Creates a string with the instruction information in a -debug5.f format
     * @retval String with instruction information
     */
    std::string dumpInstruction(void) const;
    //@}

    /** @name Server Utility Function */
    //@{
    /**
     * @brief Creates a hash based upon the instruction type
     */
    uint64_t getHash(void) const;

    /**
     * @brief closes Handle handle based upon the command type
     */
    uint32_t closeHandle(Handle ** i_handle);

    std::string getInstructionVars(const InstructionStatus & i_status) const;
    //@}

  protected:
    ecmdDataBuffer data;
    std::string deviceString;
    uint32_t timeout;
    uint32_t replyLength;

    virtual uint32_t sbefifo_open(Handle ** handle, InstructionStatus & o_status) { return -1; }
    virtual void sbefifo_ffdc_and_reset(Handle ** handle, InstructionStatus & o_status) { }
    virtual uint32_t sbefifo_close(Handle * handle) { return -1; }
    virtual ssize_t sbefifo_submit(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status, uint32_t & o_reply_wordcount) { return -1; }

};

#endif // _SBEFIFOInstruction_H
