#ifndef _I2CINSTRUCTION_H
#define _I2CINSTRUCTION_H
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

/**
 * @file I2CInstruction.H
 * @brief Provides a class for I2C instructions being passed to the fsp server
 *
*/


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <Instruction.H>
#include <unistd.h>

/**
 @brief I2CInstruction class
*/
class I2CInstruction : public Instruction {
  public:
    /** @name I2CInstruction Constructors */
    //@{
    /**
     * @brief Default Constructor
     * @post Instruction() is called, version is set to 0x3, and type is set to I2C 
     */
    I2CInstruction(void);

    /**
     * @brief Constructor with all variables passed in
     * @post Instruction() is called, command, cfamid, linkid, engineId, port, slaveAddress, busSpeed, offset, offsetFieldSize, length, i2cFlags, and flags are set, version is set to 0x1, and type is set to I2C, if i_data is not NULL it is copied to data
     */
    I2CInstruction(InstructionCommand i_command, uint32_t i_cfamid, uint32_t i_linkid, uint32_t i_engineId, uint32_t i_port, uint32_t i_slaveAddress, uint32_t i_busSpeed, uint32_t i_offset, uint32_t i_offsetFieldSize, uint32_t i_length, uint32_t i_i2cFlags, uint32_t i_flags, ecmdDataBuffer * i_data = NULL);

    /**
     * @brief Default Destructor
     */
    ~I2CInstruction(void);
    //@}

    // Member Functions
    /** @name Setup Function */
    //@{
    /**
     * @brief Used as alternate way to set variables normally set on construction
     * @retval ECMD_SUCCESS on success
     * @retval nonzero on failure
     * @post command, cfamid, linkid, engineId, port, slaveAddress, busSpeed, offset, offsetFieldSize, length, i2cFlags, and flags are set, if i_data is not NULL it is copied to data
     * @post version is set to 0x2 if i_i2cFlags is not 0
     */
    uint32_t setup(InstructionCommand i_command, uint32_t i_cfamid, uint32_t i_linkid, uint32_t i_engineId, uint32_t i_port, uint32_t i_slaveAddress, uint32_t i_busSpeed, uint32_t i_offset, uint32_t i_offsetFieldSize, uint32_t i_length, uint32_t i_i2cFlags, uint32_t i_flags, ecmdDataBuffer * i_data = NULL);

    /**
     * @brief Used as alternate way to set variables normally set on construction
     * @retval ECMD_SUCCESS on success
     * @retval nonzero on failure
     * @post command, cfamid, linkid, engineId, port, slaveAddress, busSpeed, address, length, i2cFlags, and flags are set, if i_data is not NULL it is copied to data, if i_mask is not NULL it is copied to mask
     * @post version is set to 0x3
     */
    uint32_t setup(InstructionCommand i_command, uint32_t i_cfamid, uint32_t i_linkid, uint32_t i_engineId, uint32_t i_port, uint32_t i_slaveAddress, uint32_t i_busSpeed, uint64_t i_address, uint32_t i_length, uint32_t i_i2cFlags, uint32_t i_flags, ecmdDataBuffer * i_data = NULL, ecmdDataBuffer * i_mask = NULL);

    /**
     * @brief Used as alternate way to set variables normally set on construction
     * @retval ECMD_SUCCESS on success
     * @retval nonzero on failure
     * @post command, deviceString, engineId, port, slaveAddress, busSpeed, offset/address, offsetFieldSize, length, i2cFlags, and flags are set, if i_data is not NULL it is copied to data, i_iicAckMask select what acks to retry on, i_numRedos is number of ack redos, i_msDelay is the ms delay between redos
     * @post flag has INSTRUCTION_FLAG_DEVSTR set
     * @post version is set to 0x4
     */
    uint32_t setup(InstructionCommand i_command, std::string &i_deviceString, uint32_t i_engineId, uint32_t i_port, uint32_t i_slaveAddress, uint32_t i_busSpeed, uint64_t i_offset, uint32_t i_offsetFieldSize, uint32_t i_length, uint32_t i_i2cFlags, uint32_t i_flags, ecmdDataBuffer * i_data = NULL, uint32_t i_iicAckMask = 0, uint32_t i_numRedos = 0, uint32_t i_msDelay = 0, uint32_t i_serverI2cFlags = 0);
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
     * Operates on the following InstructionCommand types: I2CREAD and I2CWRITE
     */
    uint32_t execute(ecmdDataBuffer & o_data, InstructionStatus & o_status, Handle ** io_handle);
    //@}

    /** @name Transport Functions */
    //@{
    /**
     * @brief Flatten all the object data into a uint8_t buffer
     * @param o_data Byte buffer to write the flattened data to
     * @param i_len Number of bytes in the o_data buffer
     * @post o_data buffer has a flattened version of the I2CInstruction - must be pre-allocated
     * Data format (all in network byte order):
     * First Word:      version
     * Second Word:     command
     * Third Word:      flags
     * Fourth Word:     cfamid
     * Fifth Word:      linkid
     * Sixth Word:      engineId
     * Seventh Word:    port
     * Eighth Word:     slaveAddress
     * Ninth Word:      busSpeed
     * version < 0x3 || command == I2CREAD || command == I2CWRITE
     * Tenth Word:      offset
     * Eleventh Word:   offsetFieldSize
     * version >= 0x3 && command == SCOM* || command == LONG*
     * Tenth Word:      address upper 32 bits
     * Eleventh Word:   address lower 32 bits
     * Twelfth Word:    length
     * version == 0x1
     * Thirteenth Word: data size
     * Multiple Words:  data
     * version == 0x2
     * Thirteenth Word: i2cFlags
     * Fourteenth Word: data size
     * Multiple Words:  data
     * version == 0x3
     * command == SCOMIN || command == LONGIN || command == I2CWRITE
     * Thirteenth Word: i2cFlags
     * Fourteenth Word: data size
     * Multiple Words:  data
     * command == SCOMIN_MASK
     * Thirteenth Word: i2cFlags
     * Fourteenth Word: data size
     * Fifteenth  Word: mask size
     * Multiple Words:  data
     * Multiple Words:  mask
     * command == SCOMOUT || command == LONGOUT || command == I2CREAD
     * nothing
     */
    uint32_t flatten(uint8_t * o_data, uint32_t i_len) const;

    /**
     * @brief Unflatten object data from a uint8_t buffer into this I2CInstruction
     * @param i_data Byte buffer to read the flattened data from
     * @param i_len Number of bytes in the i_data buffer
     * @post This I2CInstruction is allocated and initialized with the unflattened version of i_data
     * Data format (all in network byte order):
     * First Word:      version
     * Second Word:     command
     * Third Word:      flags
     * Fourth Word:     cfamid
     * Fifth Word:      linkid
     * Sixth Word:      engineId
     * Seventh Word:    port
     * Eighth Word:     slaveAddress
     * Ninth Word:      busSpeed
     * version < 0x3 || command == I2CREAD || command == I2CWRITE
     * Tenth Word:      offset
     * Eleventh Word:   offsetFieldSize
     * version >= 0x3 && command == SCOM* || command == LONG*
     * Tenth Word:      address upper 32 bits
     * Eleventh Word:   address lower 32 bits
     * Twelfth Word:    length
     * version == 0x1
     * Thirteenth Word: data size
     * Multiple Words:  data
     * version == 0x2
     * Thirteenth Word: i2cFlags
     * Fourteenth Word: data size
     * Multiple Words:  data
     * version == 0x3
     * command == SCOMIN || command == LONGIN || command == I2CWRITE
     * Thirteenth Word: i2cFlags
     * Fourteenth Word: data size
     * Multiple Words:  data
     * command == SCOMIN_MASK
     * Thirteenth Word: i2cFlags
     * Fourteenth Word: data size
     * Fifteenth  Word: mask size
     * Multiple Words:  data
     * Multiple Words:  mask
     * command == SCOMOUT || command == LONGOUT || command == I2CREAD
     * nothing
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
type,     cfamid(28:31), linkid(24:31), engineId(24:31), port(24:31)
bits 0:3  bits 4:7,      bits 8:15,     bits 16:23,      bits 24:31 </pre>
     */
    uint64_t getHash(void) const;

    /**
     * @brief closes Handle handle with adal_iic_close()
     */
    uint32_t closeHandle(Handle ** i_handle);

    std::string getInstructionVars(const InstructionStatus & i_status) const;
    //@}

    enum ResetType
    {
        IIC_RESET_LIGHT = 1,
        IIC_RESET_FULL = 2,
    };

  protected:
    uint32_t cfamid;
    uint32_t linkid;
    uint32_t engineId;
    uint32_t port;
    uint32_t slaveAddress;
    uint32_t busSpeed;
    uint32_t offset;
    uint32_t offsetFieldSize;
    uint32_t length;
    uint32_t i2cFlags;
    uint64_t address;
    ecmdDataBuffer data;
    ecmdDataBuffer mask;
    std::string deviceString;

    //ack stuff
    uint32_t iicAckMask;
    uint32_t numRedos;
    uint32_t msDelay;

    // server I2c flags
    uint32_t serverI2cFlags;
    
    virtual uint32_t iic_open(Handle ** handle, InstructionStatus & o_status) { return -1; }
    virtual void iic_ffdc(Handle ** handle, InstructionStatus & o_status) { }
    virtual uint32_t iic_close(Handle * handle) { return -1; }

    virtual uint32_t iic_config_speed(Handle * i_handle, InstructionStatus & o_status) { return -1; }
    virtual uint32_t iic_config_slave_address(Handle * i_handle, InstructionStatus & o_status) { return -1; }
    virtual uint32_t iic_config_device_width(Handle * i_handle, InstructionStatus & o_status) { return -1; }
    virtual uint32_t iic_config_device_offset(Handle * i_handle, InstructionStatus & o_status) { return -1; }
    virtual uint32_t iic_config_device_retries(Handle * i_handle, InstructionStatus & o_status) { return -1; }
    virtual uint32_t iic_config_device_timeout(Handle * i_handle, InstructionStatus & o_status) { return -1; }
    virtual ssize_t iic_read(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status) { return -1; }
    virtual ssize_t iic_write(Handle * i_handle, InstructionStatus & o_status) { return -1; }
    virtual uint32_t iic_reset(Handle * i_handle, ResetType i_type) { return -1; }

};

#endif // _I2CINSTRUCTION_H
