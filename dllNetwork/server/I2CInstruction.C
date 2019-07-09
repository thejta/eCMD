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


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#ifndef __STDC_FORMAT_MACROS
  #define __STDC_FORMAT_MACROS 1
#endif
#include <I2CInstruction.H>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iomanip>
#include <errno.h>
#include <inttypes.h>

#include <OutputLite.H>
extern OutputLite out;

/*****************************************************************************/
/* I2CInstruction Implementation *********************************************/
/*****************************************************************************/
I2CInstruction::I2CInstruction(void) : Instruction(){
  version = 0x4;
  type = I2C;
}

I2CInstruction::I2CInstruction(InstructionCommand i_command, uint32_t i_cfamid, uint32_t i_linkid, uint32_t i_engineId, uint32_t i_port, uint32_t i_slaveAddress, uint32_t i_busSpeed, uint32_t i_offset, uint32_t i_offsetFieldSize, uint32_t i_length, uint32_t i_i2cFlags, uint32_t i_flags, ecmdDataBuffer * i_data) : Instruction(),
cfamid(i_cfamid),
linkid(i_linkid),
engineId(i_engineId),
port(i_port),
slaveAddress(i_slaveAddress),
busSpeed(i_busSpeed),
offset(i_offset),
offsetFieldSize(i_offsetFieldSize),
length(i_length),
i2cFlags(i_i2cFlags),
address(0),
iicAckMask(0),
numRedos(0),
msDelay(0)
{
  version = 0x2;
  type = I2C;
  command = i_command;
  flags = i_flags;
  if(i_data != NULL) {
    i_data->shareBuffer(&data);
  }
  /* use version one protocol if i2cFlags is zero */
  if (version == 0x2 && ((i2cFlags & INSTRUCTION_I2C_FLAG_MASK) == 0x0)) {
    version = 0x1;
  }
}

I2CInstruction::~I2CInstruction(void) {
}

uint32_t I2CInstruction::setup(InstructionCommand i_command, uint32_t i_cfamid, uint32_t i_linkid, uint32_t i_engineId, uint32_t i_port, uint32_t i_slaveAddress, uint32_t i_busSpeed, uint32_t i_offset, uint32_t i_offsetFieldSize, uint32_t i_length, uint32_t i_i2cFlags, uint32_t i_flags, ecmdDataBuffer * i_data) {
  cfamid = i_cfamid;
  linkid = i_linkid;
  engineId = i_engineId;
  port = i_port;
  slaveAddress = i_slaveAddress;
  busSpeed = i_busSpeed;
  offset = i_offset;
  offsetFieldSize = i_offsetFieldSize;
  length = i_length;
  i2cFlags = i_i2cFlags;
  command = i_command;
  flags = i_flags;
  address = 0;
  iicAckMask = 0;
  numRedos = 0;
  msDelay = 0;
  if(i_data != NULL) {
    i_data->shareBuffer(&data);
  }
  version = 0x2;
  /* use version one protocol if i2cFlags is zero */
  if (version == 0x2 && ((i2cFlags & INSTRUCTION_I2C_FLAG_MASK) == 0x0)) {
    version = 0x1;
  }
  return 0;
}

uint32_t I2CInstruction::setup(InstructionCommand i_command, uint32_t i_cfamid, uint32_t i_linkid, uint32_t i_engineId, uint32_t i_port, uint32_t i_slaveAddress, uint32_t i_busSpeed, uint64_t i_address, uint32_t i_length, uint32_t i_i2cFlags, uint32_t i_flags, ecmdDataBuffer * i_data, ecmdDataBuffer * i_mask) {
  cfamid = i_cfamid;
  linkid = i_linkid;
  engineId = i_engineId;
  port = i_port;
  slaveAddress = i_slaveAddress;
  busSpeed = i_busSpeed;
  offset = 0;
  offsetFieldSize = 0;
  length = i_length;
  i2cFlags = i_i2cFlags;
  command = i_command;
  flags = i_flags;
  address = i_address;
  iicAckMask = 0;
  numRedos = 0;
  msDelay = 0;
  if(i_data != NULL) {
    i_data->shareBuffer(&data);
  }
  if(i_mask != NULL) {
    i_mask->shareBuffer(&mask);
  }
  version = 0x3;
  return 0;
}

uint32_t I2CInstruction::setup(InstructionCommand i_command, std::string &i_deviceString, uint32_t i_engineId, uint32_t i_port, uint32_t i_slaveAddress, uint32_t i_busSpeed, uint64_t i_offset, uint32_t i_offsetFieldSize, uint32_t i_length, uint32_t i_i2cFlags, uint32_t i_flags, ecmdDataBuffer * i_data, uint32_t i_iicAckMask, uint32_t i_numRedos, uint32_t i_msDelay ) {
  deviceString = i_deviceString;
  engineId = i_engineId;
  port = i_port;
  slaveAddress = i_slaveAddress;
  busSpeed = i_busSpeed;
  offsetFieldSize = i_offsetFieldSize;
  length = i_length;
  i2cFlags = i_i2cFlags;
  command = i_command;
  flags = i_flags | INSTRUCTION_FLAG_DEVSTR;
  address = 0;
  iicAckMask = 0;
  numRedos = 0;
  msDelay = 0;
  if(i_data != NULL) {
    i_data->shareBuffer(&data);
  }
  if ( i_command == I2CRESETLIGHT || i_command == I2CRESETFULL )
  {
    version = 0x5;
  }
  else
  {
    version = 0x4;
  }

  if ((i_offsetFieldSize & 0x0FFFFFFF) <= 4)
  {
    offset = (uint32_t) i_offset;
  }
  else
  {
    address = i_offset;
    version = 0x6;
  }
  if ( i_iicAckMask != 0 || i_numRedos != 0 || i_msDelay != 0 ) {
    iicAckMask = i_iicAckMask;
    numRedos = i_numRedos;
    msDelay = i_msDelay;
    version = 0x7;
  }
  return 0;
}

uint32_t I2CInstruction::execute(ecmdDataBuffer & o_data, InstructionStatus & o_status, Handle ** io_handle) {
  int rc = 0;

  /* set the version of this instruction in the status */
  o_status.instructionVersion = version;

  /* check for any previous errors to report back */
  if (error) {
    rc = o_status.rc = error;
    return rc;
  }

  switch(command) {
    case I2CREAD:
    case I2CWRITE:
    case I2CRESETLIGHT:
    case I2CRESETFULL:
      {
          char errstr[200];

          /* Open the Handle */
          rc = iic_open(io_handle, o_status);
          if (rc) {
            o_status.rc = rc;
            return rc;
          }

        errno = 0;

        if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
          snprintf(errstr, 200, "SERVER_DEBUG : iic_config_speed() busSpeed = %d\n", busSpeed);
          o_status.errorMessage.append(errstr);
        }

        rc = iic_config_speed(*io_handle, o_status);

        if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
          snprintf(errstr, 200, "SERVER_DEBUG : iic_config_speed() rc = %u\n", rc);
          o_status.errorMessage.append(errstr);
        }

        if (rc < 0) {
          snprintf(errstr, 200, "I2CInstruction::execute Problem during I2C device speed config. Speed: %d, errno %d\n", busSpeed, errno);
          o_status.errorMessage.append(errstr);
          rc = o_status.rc = SERVER_I2C_CONFIG_FAIL;
          iic_ffdc(io_handle, o_status);
          break;
        }

        /* Slave address */
        errno = 0;

        if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
          snprintf(errstr, 200, "SERVER_DEBUG : iic_config_slave_address() slaveAddress = 0x%08X\n", slaveAddress);
          o_status.errorMessage.append(errstr);
        }

        rc = iic_config_slave_address(*io_handle, o_status);

        if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
          snprintf(errstr, 200, "SERVER_DEBUG : iic_config_slave_address() rc = %u\n", rc);
          o_status.errorMessage.append(errstr);
        }

        if (rc) {
          snprintf(errstr, 200, "I2CInstruction::execute Problem during I2C device slave config : errno %d\n", errno);
          o_status.errorMessage.append(errstr);
          rc = o_status.rc = SERVER_I2C_CONFIG_FAIL;
          iic_ffdc(io_handle, o_status);
          break;
        }

        errno = 0;

        if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
          snprintf(errstr, 200, "SERVER_DEBUG : iic_config_device_width() offsetFieldSize = %d\n", offsetFieldSize);
          o_status.errorMessage.append(errstr);
        }

        rc = iic_config_device_width(*io_handle, o_status);

        if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
          snprintf(errstr, 200, "SERVER_DEBUG : iic_config_device_width() rc = %u\n", rc);
          o_status.errorMessage.append(errstr);
        }

        if (rc) {
          snprintf(errstr, 200, "I2CInstruction::execute Problem during I2C device slave config : errno %d\n", errno);
          o_status.errorMessage.append(errstr);
          rc = o_status.rc = SERVER_I2C_CONFIG_FAIL;
          iic_ffdc(io_handle, o_status);
          break;
        }

        /* The offset */
        errno = 0;

        if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
          if (offsetFieldSize > 4)
          {
            snprintf(errstr, 200, "SERVER_DEBUG : iic_config_device_offset() address = 0x%0" PRIx64 "\n", address);
          }
          else
          {
            snprintf(errstr, 200, "SERVER_DEBUG : iic_config_device_offset() offset = 0x%08X\n", offset);
          }
          o_status.errorMessage.append(errstr);
        }

        rc = iic_config_device_offset(*io_handle, o_status);

        if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
          snprintf(errstr, 200, "SERVER_DEBUG : iic_config_device_offset() rc = %u\n", rc);
          o_status.errorMessage.append(errstr);
        }

        if (rc) {
          snprintf(errstr, 200, "I2CInstruction::execute Problem during I2C device slave config : errno %d\n", errno);
          o_status.errorMessage.append(errstr);
          rc = o_status.rc = SERVER_I2C_CONFIG_FAIL;
          iic_ffdc(io_handle, o_status);
          break;
        }

        /* enable retry on nacks */
        if ( (version > 0x1) )
        {
          // TODO implement policy
          errno = 0;

          // IF INSTRUCTION_I2C_FLAG_NACK_RETRY_100MS then override other parameters passed in
          if (INSTRUCTION_I2C_FLAG_NACK_RETRY_100MS & i2cFlags)
          {
            numRedos = 20; /* 20 redos for nacks */
            msDelay = 5; /* 5ms delay between redo */
          }
          else if ( msDelay == 0 && numRedos == 0 )
          {
            msDelay = 1000;
            numRedos = 10;
          }

          rc = iic_config_device_retries(*io_handle, o_status);

          if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
            snprintf(errstr, 200, "SERVER_DEBUG : iic_config_retries() retries: %d\n", numRedos);
            o_status.errorMessage.append(errstr);
          }
          if (rc) {
            snprintf(errstr, 200, "I2CInstruction::execute Problem during I2C device retry policy config : errno %d\n", errno);
            o_status.errorMessage.append(errstr);
            rc = o_status.rc = SERVER_I2C_CONFIG_FAIL;
            iic_ffdc(io_handle, o_status);
            break;
          }

          rc = iic_config_device_timeout(*io_handle, o_status);

          if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
            snprintf(errstr, 200, "SERVER_DEBUG : iic_config_timeout() msdelay: %d\n", msDelay);
            o_status.errorMessage.append(errstr);
          }
          if (rc) {
            snprintf(errstr, 200, "I2CInstruction::execute Problem during I2C device timeout config : errno %d\n", errno);
            o_status.errorMessage.append(errstr);
            rc = o_status.rc = SERVER_I2C_CONFIG_FAIL;
            iic_ffdc(io_handle, o_status);
            break;
          }
        }

        if (command == I2CRESETLIGHT){
          int rcReset = 0;
          errno = 0;

          rcReset = iic_reset(*io_handle, IIC_RESET_LIGHT);

          if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
            std::string words;
            genWords(o_data, words);
            snprintf(errstr, 200, "SERVER_DEBUG : iic_reset() SERVER_I2CRESETLIGHT errno=%d, rc = %d)\n", errno, rcReset);
            o_status.errorMessage.append(errstr);
          }
         
          if ( rcReset < -1 )
          {
            // in this case the errno value is set
            snprintf( errstr, 200, "I2CInstruction::execute light reset problem errno=%d rc=%d\n", errno, rcReset );
            o_status.errorMessage.append(errstr);
            rc = o_status.rc = SERVER_I2C_RESET_LIGHT_FAIL;
            iic_ffdc( io_handle, o_status );
            break;
          }
          else if ( rcReset == -1 )
          {
            snprintf( errstr, 200, "I2CInstruction::execute Failure to release stuck data line errno=%d, rc=%d\n", errno, rcReset );
            o_status.errorMessage.append(errstr);
            rc = o_status.rc = SERVER_I2C_RESET_LIGHT_FAIL;
            iic_ffdc( io_handle, o_status );
            break;
          }
          else if ( rcReset == 1 )
          {
            // set status here to RESET SUCCESS indicating to the caller that 
            // the stuck bus has been recovered
            rc = o_status.rc = SERVER_I2C_RESET_SUCCESS;
            break;
          } 
        } else if (command == I2CRESETFULL){
          int rcReset = 0;
          errno = 0;

          rcReset = iic_reset(*io_handle, IIC_RESET_FULL);

          if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
            std::string words;
            genWords(o_data, words);
            snprintf(errstr, 200, "SERVER_DEBUG : iic_reset() SERVER_I2CRESETFULL errno=%d, rc = %d)\n", errno, rcReset);
            o_status.errorMessage.append(errstr);
          }
         
          if ( rcReset < -1 )
          {
            // in this case the errno value is set
            snprintf( errstr, 200, "I2CInstruction::execute full reset problem errno=%d rc=%d\n", errno, rcReset );
            o_status.errorMessage.append(errstr);
            rc = o_status.rc = SERVER_I2C_RESET_FULL_FAIL;
            iic_ffdc( io_handle, o_status );
            break;
          }
          else if ( rcReset == -1 )
          {
            snprintf( errstr, 200, "I2CInstruction::execute Failure to release stuck data line errno=%d, rc=%d\n", errno, rcReset );
            o_status.errorMessage.append(errstr);
            rc = o_status.rc = SERVER_I2C_RESET_FULL_FAIL;
            iic_ffdc( io_handle, o_status );
            break;
          }
          else if ( rcReset == 1 )
          {
            // set status here to RESET SUCCESS indicating to the caller that 
            // the stuck bus has been recovered
            rc = o_status.rc = SERVER_I2C_RESET_SUCCESS;
            break;
          } 
        
        } else if (command == I2CREAD) {
          /* Perform the read */
          o_data.setBitLength(length);
          ssize_t bytelen = length % 8 ? (length / 8) + 1 : length / 8;
          ssize_t len = 0;
          errno = 0;

          if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
            snprintf(errstr, 200, "SERVER_DEBUG : iic_read() length = %d\n", length);
            o_status.errorMessage.append(errstr);
          }

          len = iic_read(*io_handle, o_data, o_status);

          if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
            std::string words;
            genWords(o_data, words);
            snprintf(errstr, 200, "SERVER_DEBUG : iic_read() o_data = %s, rc = %zu)\n", words.c_str(), len);
            o_status.errorMessage.append(errstr);
          }

          if (len != bytelen) {
            snprintf(errstr, 200, "I2CInstruction::execute Problem reading from I2C device : errno %d, length exp (%zd) actual (%zd)\n", errno, bytelen, len);
            o_status.errorMessage.append(errstr);
            rc = o_status.rc = SERVER_I2C_READ_FAIL;
            iic_ffdc(io_handle, o_status);
            break;
          }
        } else {
          /* Perform the actual data write */
          ssize_t bytelen = length % 8 ? (length / 8) + 1 : length / 8;
          ssize_t len = 0;
          errno = 0;

          if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
            std::string words;
            genWords(data, words);
            snprintf(errstr, 200, "SERVER_DEBUG : iic_write() data = %s length = %u\n", words.c_str(), length);
            o_status.errorMessage.append(errstr);
          }

          len = iic_write(*io_handle, o_status);

          if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
            snprintf(errstr, 200, "SERVER_DEBUG : iic_write() rc = %zu\n", len);
            o_status.errorMessage.append(errstr);
          }

          if (len != bytelen) {
            snprintf(errstr, 200, "I2CInstruction::execute Problem writing to I2C device : errno %d, length exp (%zd) actual (%zd)\n", errno, bytelen, len);
            o_status.errorMessage.append(errstr);
            rc = o_status.rc = SERVER_I2C_WRITE_FAIL;
            iic_ffdc(io_handle, o_status);
            break;
          }
        }
        rc = o_status.rc = SERVER_COMMAND_COMPLETE;
      }
      break;
    default:
      rc = o_status.rc = SERVER_COMMAND_NOT_SUPPORTED;
      break;
  }


  return rc;
}

uint32_t I2CInstruction::flatten(uint8_t * o_data, uint32_t i_len) const {
  uint32_t rc = 0;
  uint32_t * o_ptr = (uint32_t *) o_data;
  
  if (i_len < flattenSize()) {
    out.error("I2CInstruction::flatten", "i_len %d bytes is too small to flatten\n", i_len);
    rc = 1;
  } else {
    // clear memory
    memset(o_data, 0, flattenSize());
    o_ptr[0] = htonl(version);
    o_ptr[1] = htonl(command);
    o_ptr[2] = htonl(flags);
    if (flags & INSTRUCTION_FLAG_DEVSTR) {
      if (command == SCOMIN || command == SCOMOUT || command == SCOMIN_MASK || command == LONGIN || command == LONGOUT) {
        // error not supported
        out.error("I2CInstruction::flatten", "command %s not supported\n", InstructionCommandToString(command).c_str());
        return 1;
      }
      o_ptr[3] = htonl(engineId);
      o_ptr[4] = htonl(port);
      o_ptr[5] = htonl(slaveAddress);
      o_ptr[6] = htonl(busSpeed);
      o_ptr[7] = htonl(offset);
      o_ptr[8] = htonl(offsetFieldSize);
      o_ptr[9] = htonl(length);
      o_ptr[10] = htonl(i2cFlags);
      uint32_t l_offset = 0;
      uint32_t deviceStringSize = deviceString.size() + 1;
      if (deviceStringSize % sizeof(uint32_t)) {
        deviceStringSize += (sizeof(uint32_t) - (deviceStringSize % sizeof(uint32_t)));
      }
      o_ptr[11] = htonl(deviceStringSize);
      if ((offsetFieldSize & 0x0FFFFFFF) > 4)
      {
        o_ptr[12] = htonl((uint32_t) (address >> 32));
        o_ptr[13] = htonl((uint32_t) (address & 0xFFFFFFFF));
        l_offset += 2;
      }
      if ( version >= 0x7 ) {
        o_ptr[12+l_offset] = htonl(iicAckMask);
        o_ptr[13+l_offset] = htonl(numRedos);
        o_ptr[14+l_offset] = htonl(msDelay);
        l_offset += 3;
      }
      if (command == I2CREAD || command == I2CRESETLIGHT || command == I2CRESETFULL) {
        // no data to flatten
      } else {
        uint32_t dataSize = data.flattenSize();
        o_ptr[12 + l_offset] = htonl(dataSize);
        data.flatten((uint8_t *) (o_ptr + 13 + l_offset + (deviceStringSize / sizeof(uint32_t))), dataSize);
        l_offset++;
      }
      if (deviceString.size() > 0) {
        strcpy(((char *)(o_ptr + 12 + l_offset)), deviceString.c_str());
      }
    } else {
      o_ptr[3] = htonl(cfamid);
      o_ptr[4] = htonl(linkid);
      o_ptr[5] = htonl(engineId);
      o_ptr[6] = htonl(port);
      o_ptr[7] = htonl(slaveAddress);
      o_ptr[8] = htonl(busSpeed);
      if (version >= 0x3 && (command == SCOMIN || command == SCOMOUT || command == SCOMIN_MASK || command == LONGIN || command == LONGOUT)) {
        o_ptr[9] = htonl((uint32_t) (address >> 32));
        o_ptr[10] = htonl((uint32_t) (address & 0xFFFFFFFF));
      } else {
        o_ptr[9] = htonl(offset);
        o_ptr[10] = htonl(offsetFieldSize);
      }
      o_ptr[11] = htonl(length);
      uint32_t dataSize = data.flattenSize();
      if(version == 0x1) {
        o_ptr[12] = htonl(dataSize);
        data.flatten((uint8_t *) (o_ptr + 13), dataSize);
      } else if(version >= 0x2) {
        o_ptr[12] = htonl(i2cFlags);
        if (version >= 0x3 && command == SCOMIN_MASK) {
          o_ptr[13] = htonl(dataSize);
          uint32_t maskSize = mask.flattenSize();
          o_ptr[14] = htonl(maskSize);
          data.flatten((uint8_t *) (o_ptr + 15), dataSize);
          mask.flatten((uint8_t *) (o_ptr + 15 + (dataSize / sizeof(uint32_t))), maskSize);
        } else if (version >= 0x3 && (command == SCOMOUT || command == LONGOUT || command == I2CREAD)) {
          // no data to flatten
        } else {
          o_ptr[13] = htonl(dataSize);
          data.flatten((uint8_t *) (o_ptr + 14), dataSize);
        }
      }
    }
  }
  return rc;
}

uint32_t I2CInstruction::unflatten(const uint8_t * i_data, uint32_t i_len) {
  uint32_t rc = 0;
  uint32_t * i_ptr = (uint32_t *) i_data;

  version = ntohl(i_ptr[0]);
  if((version >= 0x1) && (version <= 0x7)) {
    command = (InstructionCommand) ntohl(i_ptr[1]);
    flags = ntohl(i_ptr[2]);
    if ((version >= 0x4) && (flags & INSTRUCTION_FLAG_DEVSTR)) {
      engineId = ntohl(i_ptr[3]);
      port = ntohl(i_ptr[4]);
      slaveAddress = ntohl(i_ptr[5]);
      busSpeed = ntohl(i_ptr[6]);
      offset = ntohl(i_ptr[7]);
      offsetFieldSize = ntohl(i_ptr[8]);
      length = ntohl(i_ptr[9]);
      i2cFlags = ntohl(i_ptr[10]);
      uint32_t l_offset = 0;
      uint32_t deviceStringSize = ntohl(i_ptr[11]);
      if ((offsetFieldSize & 0x0FFFFFFF) > 4) {
        address = ((uint64_t) ntohl(i_ptr[12])) << 32;
        address |= ((uint64_t) ntohl(i_ptr[13]));
        l_offset += 2;
      }
      if ( version >= 0x7 ) {
        iicAckMask = ntohl(i_ptr[12+l_offset]);
        numRedos = ntohl(i_ptr[13+l_offset]);
        msDelay = ntohl(i_ptr[14+l_offset]);
        l_offset += 3;
      }
      if (command == I2CREAD || command == I2CRESETLIGHT || command == I2CRESETFULL) {
        // no data to unflatten
      } else {
        uint32_t dataSize = ntohl(i_ptr[12 + l_offset]);
        rc = data.unflatten((uint8_t *) (i_ptr + 13 + l_offset + (deviceStringSize / sizeof(uint32_t))), dataSize);
        l_offset++;
      }
      if (deviceStringSize > 0) {
        deviceString = ((char *)(i_ptr + 12 + l_offset));
      }
    } else {
      cfamid = ntohl(i_ptr[3]);
      linkid = ntohl(i_ptr[4]);
      engineId = ntohl(i_ptr[5]);
      port = ntohl(i_ptr[6]);
      slaveAddress = ntohl(i_ptr[7]);
      busSpeed = ntohl(i_ptr[8]);
      if (version >= 0x3 && (command == SCOMIN || command == SCOMOUT || command == SCOMIN_MASK || command == LONGIN || command == LONGOUT)) {
        address = ((uint64_t) ntohl(i_ptr[9])) << 32;
        address |= ((uint64_t) ntohl(i_ptr[10]));
      } else {
        offset = ntohl(i_ptr[9]);
        offsetFieldSize = ntohl(i_ptr[10]);
      }
      length = ntohl(i_ptr[11]);
      if(version == 0x1) {
        i2cFlags = 0x0;
        uint32_t dataSize = ntohl(i_ptr[12]);
        rc = data.unflatten((uint8_t *) (i_ptr + 13), dataSize);
      } else if(version >= 0x2) {
        i2cFlags = ntohl(i_ptr[12]);
        if (version >= 0x3 && command == SCOMIN_MASK) {
          uint32_t dataSize = ntohl(i_ptr[13]);
          uint32_t maskSize = ntohl(i_ptr[14]);
          rc = data.unflatten((uint8_t *) (i_ptr + 15), dataSize);
          if (rc) { error = rc; }
          rc = mask.unflatten((uint8_t *) (i_ptr + 15 + (dataSize / sizeof(uint32_t))), maskSize);
        } else if (version >= 0x3 && (command == SCOMOUT || command == LONGOUT || command == I2CREAD)) {
          // no data to unflatten
        } else {
          uint32_t dataSize = ntohl(i_ptr[13]);
          rc = data.unflatten((uint8_t *) (i_ptr + 14), dataSize);
        }
      }
    }
    if (rc) { error = rc; }
  } else {
    error = rc = SERVER_UNKNOWN_INSTRUCTION_VERSION;
  }
  return rc;
}

uint32_t I2CInstruction::flattenSize(void) const {
  uint32_t size = 0;
  if ((version >= 0x4) && (flags & INSTRUCTION_FLAG_DEVSTR)) {
    size = 12 * sizeof(uint32_t); // version, command, flags, engineId, port, slaveAddress, busSpeed, offset, offsetFieldSize, length, i2cFlags, deviceStringSize
    uint32_t deviceStringSize = deviceString.size() + 1;
    if (deviceStringSize % sizeof(uint32_t)) {
      deviceStringSize += (sizeof(uint32_t) - (deviceStringSize % sizeof(uint32_t))); 
    } 
    size += deviceStringSize; // deviceString
    if (command == I2CWRITE) {
      size += sizeof(uint32_t); // dataSize
      size += data.flattenSize(); // data
    }
    if ((offsetFieldSize & 0x0FFFFFFF) > 4) {
      size += sizeof(uint32_t) * 2; // address
    }
    if ( version >= 0x7 ) {
      size += sizeof(uint32_t) * 3; // iicAckMask, numRedos, msDelay
    }
  } else if(version == 0x1) {
    size = (13 * sizeof(uint32_t)) + data.flattenSize();
  } else if(version >= 0x2) {
    if (version >= 0x3 && command == SCOMIN_MASK) {
      size = (15 * sizeof(uint32_t)) + data.flattenSize() + mask.flattenSize();
    } else if (version >= 0x3 && (command == SCOMOUT || command == LONGOUT || command == I2CREAD)) {
      size = (13 * sizeof(uint32_t));
    } else {
      size = (14 * sizeof(uint32_t)) + data.flattenSize();
    }
  }
  return size;
}

std::string I2CInstruction::dumpInstruction(void) const {
  std::ostringstream oss;
  oss << "I2CInstruction" << std::endl;
  oss << "version       : " << version << std::endl;
  oss << "command       : " << InstructionCommandToString(command) << std::endl;
  oss << "type          : " << InstructionTypeToString(type) << std::endl;
  oss << "flags         : " << InstructionFlagToString(flags) << std::endl;
  if (flags & INSTRUCTION_FLAG_DEVSTR) {
    oss << "deviceString  : " << deviceString << std::endl;
  } else {
    oss << "cfamid        : " << std::hex << std::setw(8) << std::setfill('0') << cfamid << std::dec << std::endl;
    oss << "linkid        : " << std::hex << std::setw(8) << std::setfill('0') << linkid << std::dec << std::endl;
  }
  oss << "engineId      : " << std::hex << std::setw(8) << std::setfill('0') << engineId << std::dec << std::endl;
  oss << "port          : " << std::hex << std::setw(8) << std::setfill('0') << port << std::dec << std::endl;
  oss << "slaveAddress  : " << std::hex << std::setw(8) << std::setfill('0') << slaveAddress << std::dec << std::endl;
  oss << "busSpeed      : " << busSpeed << std::endl;
  if (command == SCOMIN || command == SCOMOUT || command == SCOMIN_MASK || command == LONGIN || command == LONGOUT) {
    oss << "address       : " << std::hex << std::setw(16) << std::setfill('0') << address << std::dec << std::endl;
  } else {
    if ((offsetFieldSize & 0x0FFFFFFF) > 4) {
      oss << "address       : " << std::hex << std::setw(16) << std::setfill('0') << address << std::dec << std::endl;
    } else {
      oss << "offset        : " << std::hex << std::setw(8) << std::setfill('0') << offset << std::dec << std::endl;
    }
    oss << "offsetFieldSize : " << std::hex << std::setw(8) << std::setfill('0') << offsetFieldSize << std::dec << std::endl;
  }
  oss << "length        : " << std::dec << length << std::endl;
  if (version > 0x1) {
    oss << "i2cFlags      : " << InstructionI2CFlagToString(i2cFlags) << std::endl;
  }
  oss << "i2cAckMask    : " << iicAckMask << std::endl;
  oss << "i2cAck Redos  : " << numRedos << std::endl;
  oss << "i2cAck MsDelay: " << msDelay << std::endl;
  oss << "data length   : " << data.getBitLength() << std::endl;
  oss << "data          : ";
  for(uint32_t j = 0; j < data.getWordLength(); j++) {
    oss << std::hex << std::setw(8) << std::setfill('0') << data.getWord(j) << " ";
    if (!((j+1) % 5)) oss << "\n\t\t";
  }
  oss << std::dec << std::endl;

  return oss.str();
}

uint64_t I2CInstruction::getHash(void) const {
  if (flags & INSTRUCTION_FLAG_DEVSTR) {
    uint32_t devstrhash = 0x0;
    uint32_t rc = devicestring_genhash(deviceString, devstrhash);
    uint64_t hash64 = 0x0ull;
    hash64 |= ((0x0000000Full & type)     << 60);
    hash64 |= ((0x000000FFull & engineId) << 52);
    hash64 |= ((0x000000FFull & port)     << 46);
    if (rc == 0) {
        hash64 |= ((uint64_t) devstrhash);
    }
    return hash64;
  }
  //type,     cfamid(28:31), linkid(24:31), engineId(24:31), port(24:31)
  //bits 0:3  bits 4:7,      bits 8:15,     bits 16:23,      bits 24:31
  uint32_t hash = 0x0;
  hash |= ((0x0000000F & type)     << 28);
  hash |= ((0x0000000F & cfamid)   << 24);
  hash |= ((0x000000FF & linkid)   << 16);
  hash |= ((0x000000FF & engineId) << 8);
  hash |= ((0x000000FF & port)     << 0);
  return hash;
}

uint32_t I2CInstruction::closeHandle(Handle ** i_handle) {
  int rc = 0;

  /* Close the device */
  errno = 0;
  switch (command) {
    default:
    rc = iic_close(*i_handle);
    break;
  }
  *i_handle = NULL;
  if (rc) rc = SERVER_I2C_CLOSE_FAIL;

  return rc;
}

std::string I2CInstruction::getInstructionVars(const InstructionStatus & i_status) const {
  std::ostringstream oss;

  oss << std::hex << std::setfill('0');
  oss << "rc: " << std::setw(8) << i_status.rc;
  if (flags & INSTRUCTION_FLAG_DEVSTR) {
    oss << " devstr: " << deviceString;
  } else {
    oss << " cfamid: " << std::setw(8) << cfamid;
    oss << " linkid: " << std::setw(8) << linkid;
  }
  oss << " engineId: " << std::setw(8) << engineId;
  oss << " port: " << std::setw(8) << port;
  oss << " slaveAddress: " << std::setw(8) << slaveAddress;
  oss << " offset: " << std::setw(8) << offset;

  return oss.str();
}
