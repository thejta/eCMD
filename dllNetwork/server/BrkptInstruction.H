#ifndef _BrkptInstruction_H
#define _BrkptInstruction_H
//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2019 IBM International Business Machines Corp.
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
 * @file BrkptInstruction.H
 * @brief Provides a class for BRKPT instructions being passed to the fsp server
 *
*/


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <Instruction.H>
#include <unistd.h>
#include <ecmdStructs.H>
#include <cipStructs.H>

/**
 @brief BrkptInstruction class
*/
class BrkptInstruction : public Instruction {
  public:
    /** @name BrkptInstruction Constructors */
    //@{
    /**
     * @brief Default Constructor
     * @post Instruction() is called, version is set to 0x1, and type is set to SBEFIFO 
     */
    BrkptInstruction(void);

    /**
     * @brief Default Destructor
     */
    ~BrkptInstruction(void);
    //@}

    // Member Functions
    /** @name Setup Function */
    //@{
    /**
     * @brief Used as alternate way to set variables normally set on construction
     * @retval ECMD_SUCCESS on success
     * @retval nonzero on failure
     * @post command, deviceString, target, address, xlateVars, and flags are set
     * @post flag has INSTRUCTION_FLAG_DEVSTR set
     */
    virtual uint32_t setup(InstructionCommand i_command, std::string &i_deviceString, const ecmdChipTarget & i_target, const ecmdDataBuffer & i_address, const cipXlateVariables i_xlateVars, uint32_t i_flags);
    /**
     * @brief Used as alternate way to set variables normally set on construction
     * @retval ECMD_SUCCESS on success
     * @retval nonzero on failure
     * @post command, timeout, and flags are set
     * @post flag has INSTRUCTION_FLAG_DEVSTR set
     */
    virtual uint32_t setup(InstructionCommand i_command, uint32_t i_timeout, uint32_t i_flags);
    /**
     * @brief Used as alternate way to set variables normally set on construction
     * @retval ECMD_SUCCESS on success
     * @retval nonzero on failure
     * @post command, timeout, and flags are set
     * @post flag has INSTRUCTION_FLAG_DEVSTR set
     */
    virtual uint32_t setup(InstructionCommand i_command, const ecmdChipTarget & i_target, uint32_t i_steps, uint32_t i_flags);
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
     * @post o_data buffer has a flattened version of the BrkptInstruction - must be pre-allocated
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
     * @brief Unflatten object data from a uint8_t buffer into this BrkptInstruction
     * @param i_data Byte buffer to read the flattened data from
     * @param i_len Number of bytes in the i_data buffer
     * @post This BrkptInstruction is allocated and initialized with the unflattened version of i_data
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

    /**
     * @brief Unpack return data from command executed
     */
    static void unpackReturnData(const ecmdDataBuffer & i_data, std::list<cipBrkptTableEntry> & o_brkptTableEntries, ecmdDataBuffer & o_virtualAddress);
    static void unpackReturnData(const ecmdDataBuffer & i_data, std::list<cipSoftwareEvent_t> & o_events);
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
    std::string deviceString;
    ecmdChipTarget target;
    ecmdDataBuffer address;
    cipXlateVariables xlateVars;
    uint32_t timeout;
    uint32_t steps;

    virtual uint32_t brkpt_general(Handle * i_handle,
                                   InstructionStatus & o_status, 
                                   std::list<cipBrkptTableEntry> & o_brkptTableEntries,
                                   ecmdDataBuffer & o_virtualAddress)
    { return 1; }
    virtual uint32_t brkpt_wait(Handle * i_handle,
                                InstructionStatus & o_status, 
                                std::list<cipSoftwareEvent_t> & o_events)
    { return 1; }
    virtual uint32_t brkpt_instr_general(Handle * i_handle,
                                         InstructionStatus & o_status)
    { return 1; }
};

#endif // _BrkptInstruction_H
