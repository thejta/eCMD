#ifndef _GPIOINSTRUCTION_H
#define _GPIOINSTRUCTION_H
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

/**
 * @file GPIOInstruction.H
 * @brief Provides a class for GPIO instructions being passed to the fsp server
 *
*/


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <Instruction.H>

/**
 @brief enum used to set gpio mode
*/
typedef enum {
  GPIO_DIO_NOT_USED,
  GPIO_DIO_INPUT,
  GPIO_DIO_OPEN_DRAIN,
  GPIO_DIO_OPEN_SOURCE,
  GPIO_DIO_PUSH_PULL,
  GPIO_DIO_UNCONFIGURED,
} gpioDioMode_t;

/**
 @brief function to create string from gpio mode
*/
const char * gpioDioModeToString(gpioDioMode_t i_mode);

/**
 @brief GPIOInstruction class
*/
class GPIOInstruction : public Instruction {
  public:
    /** @name GPIOInstruction Constructors */
    //@{
    /**
     * @brief Default Constructor
     * @post Instruction() is called, version is set to 0x1, and type is set to GPIO 
     */
    GPIOInstruction(void);

    /**
     * @brief Constructor with all variables passed in
     * @post Instruction() is called, command, cfamid, linkid, cmaster, engineId, pin, mask, mode, data, and flags are set, version is set to 0x1, and type is set to GPIO
     */
    GPIOInstruction(InstructionCommand i_command, uint32_t i_cfamid, uint32_t i_linkid, uint32_t i_cmaster, uint32_t i_engineId, uint32_t i_pin, uint32_t i_mask, gpioDioMode_t i_mode, uint32_t i_data, uint32_t i_flags);

    /**
     * @brief Default Destructor
     */
    ~GPIOInstruction(void);
    //@}

    // Member Functions
    /** @name Setup Function */
    //@{
    /**
     * @brief Used as alternate way to set variables normally set on construction
     * @retval ECMD_SUCCESS on success
     * @retval nonzero on failure
     * @post command, cfamid, linkid, cmaster, engineId, pin, mask, mode, data, and flags are set
     */
    uint32_t setup(InstructionCommand i_command, uint32_t i_cfamid, uint32_t i_linkid, uint32_t i_cmaster, uint32_t i_engineId, uint32_t i_pin, uint32_t i_mask, gpioDioMode_t i_mode, uint32_t i_data, uint32_t i_flags);

    /**
     * @brief Used as alternate way to set variables normally set on construction
     * @retval ECMD_SUCCESS on success
     * @retval nonzero on failure
     * @post command, deviceString, engineId, pin, mask, mode, data, and flags are set
     * @post flag has INSTRUCTION_FLAG_DEVSTR set
     * @post version is set to 0x3
     */
    uint32_t setup(InstructionCommand i_command, std::string &i_deviceString, uint32_t i_engineId, uint32_t i_pin, uint32_t i_mask, gpioDioMode_t i_mode, uint32_t i_data, uint32_t i_flags);
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
     * Operates on the following InstructionCommand types: GPIO_CONFIGPIN, GPIO_READPIN, GPIO_READPINS, GPIO_READLATCH, GPIO_WRITELATCH,
     * GPIO_WRITELATCHES, GPIO_READCONFIG, GPIO_WRITECONFIG, GPIO_WRITECNFGSET, GPIO_WRITECNFGCLR
     */
    uint32_t execute(ecmdDataBuffer & o_data, InstructionStatus & o_status, Handle ** io_handle);
    //@}

    /** @name Transport Functions */
    //@{
    /**
     * @brief Flatten all the object data into a uint8_t buffer
     * @param o_data Byte buffer to write the flattened data to
     * @param i_len Number of bytes in the o_data buffer
     * @post o_data buffer has a flattened version of the GPIOInstruction - must be pre-allocated
     * Data format (all in network byte order):
     * First Word:      version
     * Second Word:     command
     * Third Word:      flags
     * Fourth Word:     cfamid
     * Fifth Word:      linkid
     * Sixth Word:      engineId
     * Seventh Word:    pin
     * Eighth Word:     mask
     * Ninth Word:      mode
     * Tenth Word:      data
     * Eleventh Word:   cmaster     VERSION >= 2
     * =========== Device String Format (flag & INSTRUCTION_FLAG_DEVSTR)
     * First Word:      version
     * Second Word:     command
     * Third Word:      flags
     * Fourth Word:     engineId
     * Fifth Word:      pin
     * Sixth Word:      mask
     * Seventh Word:    mode
     * Eighth Word:     data
     * Ninth Word:      deviceString size
     * Multiple Words:  deviceString
     */
    uint32_t flatten(uint8_t * o_data, uint32_t i_len) const;

    /**
     * @brief Unflatten object data from a uint8_t buffer into this GPIOInstruction
     * @param i_data Byte buffer to read the flattened data from
     * @param i_len Number of bytes in the i_data buffer
     * @post This GPIOInstruction is allocated and initialized with the unflattened version of i_data
     * Data format (all in network byte order):
     * First Word:      version
     * Second Word:     command
     * Third Word:      flags
     * Fourth Word:     cfamid
     * Fifth Word:      linkid
     * Sixth Word:      engineId
     * Seventh Word:    pin
     * Eighth Word:     mask
     * Ninth Word:      mode
     * Tenth Word:      data
     * Eleventh Word:   cmaster     VERSION 2
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
     * <pre>
type,     cfamid(28:31), linkid(24:31), cmaster(24:31), engineId(24:31)
bits 0:3  bits 4:7,      bits 8:15,     bits 16:23,     bits 24:31
     */
    uint64_t getHash(void) const;

    /**
     * @brief closes Handle handle based upon the command type
     */
    uint32_t closeHandle(Handle ** i_handle);

    std::string getInstructionVars(const InstructionStatus & i_status) const;
    //@}

  protected:
    uint32_t cfamid;
    uint32_t linkid;
    uint32_t cmaster;
    uint32_t engineId;
    uint32_t pin;
    uint32_t mask;
    gpioDioMode_t mode;
    uint32_t data;
    std::string deviceString;

    virtual uint32_t gpio_open(Handle ** handle, InstructionStatus & o_status) { return -1; }
    virtual void gpio_ffdc(Handle ** handle, InstructionStatus & o_status) { }
    virtual uint32_t gpio_close(Handle * handle) { return -1; }

    virtual uint32_t gpio_set_mode(Handle * i_handle, InstructionStatus & o_status, gpioDioMode_t i_mode) { return -1; }
    virtual uint32_t gpio_read_pin(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status) { return -1; }
    virtual uint32_t gpio_read_input_pins(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status) { return -1; }
    virtual uint32_t gpio_read_latch(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status) { return -1; }
    virtual uint32_t gpio_write_pin(Handle * i_handle, InstructionStatus & o_status) { return -1; }
    virtual uint32_t gpio_write_output_pins(Handle * i_handle, InstructionStatus & o_status) { return -1; }
    virtual uint32_t gpio_read_config(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status) { return -1; }
    virtual uint32_t gpio_write_config(Handle * i_handle, InstructionStatus & o_status) { return -1; }
    virtual uint32_t gpio_write_config_set_bit(Handle * i_handle, InstructionStatus & o_status) { return -1; }
    virtual uint32_t gpio_write_config_clear_bit(Handle * i_handle, InstructionStatus & o_status) { return -1; }
};

#endif // _GPIOINSTRUCTION_H
