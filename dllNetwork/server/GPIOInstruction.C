//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <GPIOInstruction.H>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <iomanip>
#include <sstream>
#include <errno.h>

#include <OutputLite.H>
extern OutputLite out;

const char * gpioDioModeToString(gpioDioMode_t i_mode) {
  switch (i_mode) {
    case GPIO_DIO_NOT_USED:
      return "GPIO_DIO_NOT_USED";
    case GPIO_DIO_INPUT:
      return "GPIO_DIO_INPUT";
    case GPIO_DIO_OPEN_DRAIN:
      return "GPIO_DIO_OPEN_DRAIN";
    case GPIO_DIO_OPEN_SOURCE:
      return "GPIO_DIO_OPEN_SOURCE";
    case GPIO_DIO_PUSH_PULL:
      return "GPIO_DIO_PUSH_PULL";
    case GPIO_DIO_UNCONFIGURED:
      return "GPIO_DIO_UNCONFIGURED";
  }
  return NULL;
}

/*****************************************************************************/
/* GPIOInstruction Implementation ********************************************/
/*****************************************************************************/
GPIOInstruction::GPIOInstruction(void) : Instruction(){
  version = 0x3;
  type = GPIO;
}

GPIOInstruction::GPIOInstruction(InstructionCommand i_command, uint32_t i_cfamid, uint32_t i_linkid, uint32_t i_cmaster, uint32_t i_engineId, uint32_t i_pin, uint32_t i_mask, gpioDioMode_t i_mode, uint32_t i_data, uint32_t i_flags) : Instruction(),
cfamid(i_cfamid),
linkid(i_linkid),
cmaster(i_cmaster),
engineId(i_engineId),
pin(i_pin),
mask(i_mask),
mode(i_mode),
data(i_data)
{
  version = 0x2;
  type = GPIO;
  command = i_command;
  flags = i_flags;
  if (cmaster == 0x0) {
    version = 0x1;
  }
}


GPIOInstruction::~GPIOInstruction(void) {
}

uint32_t GPIOInstruction::setup(InstructionCommand i_command, uint32_t i_cfamid, uint32_t i_linkid, uint32_t i_cmaster, uint32_t i_engineId, uint32_t i_pin, uint32_t i_mask, gpioDioMode_t i_mode, uint32_t i_data, uint32_t i_flags) {
  cfamid = i_cfamid;
  linkid = i_linkid;
  cmaster = i_cmaster;
  engineId = i_engineId;
  pin = i_pin;
  mask = i_mask;
  mode = i_mode;
  data = i_data;
  command = i_command;
  flags = i_flags;
  if (cmaster == 0x0) {
    version = 0x1;
  } else {
    version = 0x2;
  }
  return 0;
}

uint32_t GPIOInstruction::setup(InstructionCommand i_command, std::string &i_deviceString, uint32_t i_engineId, uint32_t i_pin, uint32_t i_mask, gpioDioMode_t i_mode, uint32_t i_data, uint32_t i_flags) {
  deviceString = i_deviceString;
  engineId = i_engineId;
  pin = i_pin;
  mask = i_mask;
  mode = i_mode;
  data = i_data;
  command = i_command;
  flags = i_flags | INSTRUCTION_FLAG_DEVSTR;
  version = 0x3;
  return 0;
}

uint32_t GPIOInstruction::execute(ecmdDataBuffer & o_data, InstructionStatus & o_status, Handle ** io_handle) {
  uint32_t rc = 0;

  /* set the version of this instruction in the status */
  o_status.instructionVersion = version;

  /* check for any previous errors to report back */
  if (error) {
    rc = o_status.rc = error;
    return rc;
  }

  if ( command != GPIO_CONFIGPIN
    && command != GPIO_READPIN
    && command != GPIO_READPINS
    && command != GPIO_READLATCH
    && command != GPIO_WRITELATCH
    && command != GPIO_WRITELATCHES
    && command != GPIO_READCONFIG
    && command != GPIO_WRITECONFIG
    && command != GPIO_WRITECNFGSET
    && command != GPIO_WRITECNFGCLR) {
    rc = o_status.rc = SERVER_COMMAND_NOT_SUPPORTED;
    return rc;
  }

  char errstr[200];

          /* Open the Handle */
          rc = gpio_open(io_handle, o_status);
          if (rc) {
            o_status.rc = rc;
            return rc;
          }


  /* First we'll hand the operations that need to do a config op */
  if (command == GPIO_CONFIGPIN || command == GPIO_READLATCH || command == GPIO_WRITELATCH || command == GPIO_WRITELATCHES) {

    /* do an unconfig first */
    errno = 0;

    if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
      snprintf(errstr, 200, "SERVER_DEBUG : gpio_set_mode() mask = 0x%08X mode = 0x%08X\n", mask, GPIO_DIO_UNCONFIGURED);
      o_status.errorMessage.append(errstr);
    }

    rc = gpio_set_mode(*io_handle, o_status, GPIO_DIO_UNCONFIGURED);

    if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
      snprintf(errstr, 200, "SERVER_DEBUG : gpio_set_mode() rc = %u\n", rc);
      o_status.errorMessage.append(errstr);
    }

    if (rc) {
      snprintf(errstr, 200, "GPIOInstruction::execute Problem during gpio_set_mode: errno %d\n", errno);
      o_status.errorMessage.append(errstr);
      rc = o_status.rc = SERVER_GPIO_CONFIG_FAIL;
      gpio_ffdc(io_handle, o_status);
      return rc;
    }

    errno = 0;

    if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
      snprintf(errstr, 200, "SERVER_DEBUG : gpio_set_mode() mask = 0x%08X, mode = 0x%08X\n", mask, mode);
      o_status.errorMessage.append(errstr);
    }

    rc = gpio_set_mode(*io_handle, o_status, mode);

    if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
      snprintf(errstr, 200, "SERVER_DEBUG : gpio_set_mode() rc = %u\n", rc);
      o_status.errorMessage.append(errstr);
    }

    if (rc) {
      snprintf(errstr, 200, "GPIOInstruction::execute Problem during gpio_set_mode: errno %d\n", errno);
      o_status.errorMessage.append(errstr);
      rc = o_status.rc = SERVER_GPIO_CONFIG_FAIL;
      gpio_ffdc(io_handle, o_status);
      return rc;
    } else if (command == GPIO_CONFIGPIN) {
      rc = o_status.rc = SERVER_COMMAND_COMPLETE;
    }
  }

  switch(command) {
    case GPIO_CONFIGPIN:
      /* Nothing further to do, just no-op */
      break;

    case GPIO_READPIN:
      errno = 0;
      o_data.setBitLength(32);

      if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        snprintf(errstr, 200, "SERVER_DEBUG : gpio_read_pin() pin = 0x%08X\n", pin);
        o_status.errorMessage.append(errstr);
      }

      rc = gpio_read_pin(*io_handle, o_data, o_status);

      if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        std::string words;
        genWords(o_data, words);
        snprintf(errstr, 200, "SERVER_DEBUG : gpio_read_pin() o_data = %s, rc = %u\n", words.c_str(), rc);
        o_status.errorMessage.append(errstr);
      }

      if (rc) {
        snprintf(errstr, 200, "GPIOInstruction::execute Problem during gpio_read_pin: errno %d\n", errno);
        o_status.errorMessage.append(errstr);
        rc = o_status.rc = SERVER_GPIO_READ_FAIL;
        gpio_ffdc(io_handle, o_status);
      } else {
        rc = o_status.rc = SERVER_COMMAND_COMPLETE;
      }
      break;

    case GPIO_READPINS:
      errno = 0;
      o_data.setBitLength(32);

      if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        snprintf(errstr, 200, "SERVER_DEBUG : gpio_read_input_pins() mask = 0x%08X\n", mask);
        o_status.errorMessage.append(errstr);
      }

      rc = gpio_read_input_pins(*io_handle, o_data, o_status);

      if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        std::string words;
        genWords(o_data, words);
        snprintf(errstr, 200, "SERVER_DEBUG : gpio_read_input_pins() o_data = %s, rc = %u\n", words.c_str(), rc);
        o_status.errorMessage.append(errstr);
      }

      if (rc) {
        snprintf(errstr, 200, "GPIOInstruction::execute Problem during gpio_read_input_pins: errno %d\n", errno);
        o_status.errorMessage.append(errstr);
        rc = o_status.rc = SERVER_GPIO_READ_FAIL;
        gpio_ffdc(io_handle, o_status);
      } else {
        rc = o_status.rc = SERVER_COMMAND_COMPLETE;
      }
      break;

    case GPIO_READLATCH:
      errno = 0;
      o_data.setBitLength(32);

      if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        snprintf(errstr, 200, "SERVER_DEBUG : gpio_read_latch() pin = 0x%08X\n", pin);
        o_status.errorMessage.append(errstr);
      }

      rc = gpio_read_latch(*io_handle, o_data, o_status);

      if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        std::string words;
        genWords(o_data, words);
        snprintf(errstr, 200, "SERVER_DEBUG : gpio_read_latch() o_data = %s, rc = %u\n", words.c_str(), rc);
        o_status.errorMessage.append(errstr);
      }

      if (rc) {
        snprintf(errstr, 200, "GPIOInstruction::execute Problem during gpio_read_latch: errno %d\n", errno);
        o_status.errorMessage.append(errstr);
        rc = o_status.rc = SERVER_GPIO_READ_FAIL;
        gpio_ffdc(io_handle, o_status);
      } else {
        rc = o_status.rc = SERVER_COMMAND_COMPLETE;
      }
      break;

    case GPIO_WRITELATCH:
      errno = 0;

      if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        snprintf(errstr, 200, "SERVER_DEBUG : gpio_write_pin() pin = 0x%08X, data = 0x%08X\n", pin, data);
        o_status.errorMessage.append(errstr);
      }

      rc = gpio_write_pin(*io_handle, o_status);

      if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        snprintf(errstr, 200, "SERVER_DEBUG : gpio_write_pin() rc = %u\n", rc);
        o_status.errorMessage.append(errstr);
      }

      if (rc) {
        snprintf(errstr, 200, "GPIOInstruction::execute Problem during gpio_write_pin: errno %d\n", errno);
        o_status.errorMessage.append(errstr);
        rc = o_status.rc = SERVER_GPIO_WRITE_FAIL;
        gpio_ffdc(io_handle, o_status);
      } else {
        rc = o_status.rc = SERVER_COMMAND_COMPLETE;
      }
      break;

    case GPIO_WRITELATCHES:
      errno = 0;

      if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        snprintf(errstr, 200, "SERVER_DEBUG : gpio_write_output_pins() mask = 0x%08X, data = 0x%08X\n", mask, data);
        o_status.errorMessage.append(errstr);
      }

      rc = gpio_write_output_pins(*io_handle, o_status);

      if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        snprintf(errstr, 200, "SERVER_DEBUG : gpio_write_output_pins() rc = %u\n", rc);
        o_status.errorMessage.append(errstr);
      }

      if (rc) {
        snprintf(errstr, 200, "GPIOInstruction::execute Problem during gpio_write_output_pins: errno %d\n", errno);
        o_status.errorMessage.append(errstr);
        rc = o_status.rc = SERVER_GPIO_WRITE_FAIL;
        gpio_ffdc(io_handle, o_status);
      } else {
        rc = o_status.rc = SERVER_COMMAND_COMPLETE;
      }
      break;

    case GPIO_READCONFIG:
      errno = 0;
      o_data.setBitLength(32);

      if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        snprintf(errstr, 200, "SERVER_DEBUG : gpio_read_config() pin = 0x%08X\n", pin);
        o_status.errorMessage.append(errstr);
      }

      rc = gpio_read_config(*io_handle, o_data, o_status);

      if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        std::string words;
        genWords(o_data, words);
        snprintf(errstr, 200, "SERVER_DEBUG : gpio_read_config() o_data = %s, rc = %u\n", words.c_str(), rc);
        o_status.errorMessage.append(errstr);
      }

      if (rc) {
        snprintf(errstr, 200, "GPIOInstruction::execute Problem during gpio_read_config: errno %d\n", errno);
        o_status.errorMessage.append(errstr);
        rc = o_status.rc = SERVER_GPIO_READ_FAIL;
        gpio_ffdc(io_handle, o_status);
      } else {
        rc = o_status.rc = SERVER_COMMAND_COMPLETE;
      }
      break;

    case GPIO_WRITECONFIG:
      errno = 0;

      if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        snprintf(errstr, 200, "SERVER_DEBUG : gpio_write_config() pin = 0x%08X, data = 0x%08X\n", pin, data);
        o_status.errorMessage.append(errstr);
      }

      rc = gpio_write_config(*io_handle, o_status);

      if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        snprintf(errstr, 200, "SERVER_DEBUG : gpio_write_config() rc = %u\n", rc);
        o_status.errorMessage.append(errstr);
      }

      if (rc) {
        snprintf(errstr, 200, "GPIOInstruction::execute Problem during gpio_write_config: errno %d\n", errno);
        o_status.errorMessage.append(errstr);
        rc = o_status.rc = SERVER_GPIO_WRITE_FAIL;
        gpio_ffdc(io_handle, o_status);
      } else {
        rc = o_status.rc = SERVER_COMMAND_COMPLETE;
      }
      break;

    case GPIO_WRITECNFGSET:
      errno = 0;

      if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        snprintf(errstr, 200, "SERVER_DEBUG : gpio_write_config_set_bit() ping = 0x%08X, data = 0x%08X\n", pin, data);
        o_status.errorMessage.append(errstr);
      }

      rc = gpio_write_config_set_bit(*io_handle, o_status);

      if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        snprintf(errstr, 200, "SERVER_DEBUG : gpio_write_config_set_bit() rc = %u\n", rc);
        o_status.errorMessage.append(errstr);
      }

      if (rc) {
        snprintf(errstr, 200, "GPIOInstruction::execute Problem during gpio_write_config_set_bit: errno %d\n", errno);
        o_status.errorMessage.append(errstr);
        rc = o_status.rc = SERVER_GPIO_WRITE_FAIL;
        gpio_ffdc(io_handle, o_status);
      } else {
        rc = o_status.rc = SERVER_COMMAND_COMPLETE;
      }
      break;

    case GPIO_WRITECNFGCLR:
      errno = 0;

      if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        snprintf(errstr, 200, "SERVER_DEBUG : gpio_write_config_clear_bit() pin = 0x%08X, data = 0x%08X\n", pin, data);
        o_status.errorMessage.append(errstr);
      }

      rc = gpio_write_config_clear_bit(*io_handle, o_status);

      if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
        snprintf(errstr, 200, "SERVER_DEBUG : gpio_write_config_clear_bit() rc = %u\n", rc);
        o_status.errorMessage.append(errstr);
      }

      if (rc) {
        snprintf(errstr, 200, "GPIOInstruction::execute Problem during gpio_write_config_clear_bit: errno %d\n", errno);
        o_status.errorMessage.append(errstr);
        rc = o_status.rc = SERVER_GPIO_WRITE_FAIL;
        gpio_ffdc(io_handle, o_status);
      } else {
        rc = o_status.rc = SERVER_COMMAND_COMPLETE;
      }
      break;

    default:
      break;
  }

  return rc;
}

uint32_t GPIOInstruction::flatten(uint8_t * o_data, uint32_t i_len) const {
  uint32_t rc = 0;
  uint32_t * o_ptr = (uint32_t *) o_data;
  
  if (i_len < flattenSize()) {
    out.error("GPIOIInstruction::flatten", "i_len %d bytes is too small to flatten\n", i_len);
    rc = 1;
  } else {
    // clear memory
    memset(o_data, 0, flattenSize());
    o_ptr[0] = htonl(version);
    o_ptr[1] = htonl(command);
    o_ptr[2] = htonl(flags);
    uint32_t offset = 0;
    if ((flags & INSTRUCTION_FLAG_DEVSTR) == 0) {
      o_ptr[3] = htonl(cfamid);
      o_ptr[4] = htonl(linkid);
      offset = 2;
    }
    o_ptr[3 + offset] = htonl(engineId);
    o_ptr[4 + offset] = htonl(pin);
    o_ptr[5 + offset] = htonl(mask);
    o_ptr[6 + offset] = htonl(mode);
    o_ptr[7 + offset] = htonl(data);
    if ((flags & INSTRUCTION_FLAG_DEVSTR) == 0) {
      if (version >= 0x2) {
        o_ptr[8 + offset] = htonl(cmaster);
      }
    } else {
      uint32_t deviceStringSize = deviceString.size() + 1;
      if (deviceStringSize % sizeof(uint32_t)) {
        deviceStringSize += (sizeof(uint32_t) - (deviceStringSize % sizeof(uint32_t)));
      }
      o_ptr[8 + offset] = htonl(deviceStringSize);
      if (deviceString.size() > 0) {
        strcpy(((char *)(o_ptr + 9 + offset)), deviceString.c_str());
      }
    }
  }
  return rc;
}

uint32_t GPIOInstruction::unflatten(const uint8_t * i_data, uint32_t i_len) {
  uint32_t rc = 0;
  uint32_t * i_ptr = (uint32_t *) i_data;

  version = ntohl(i_ptr[0]);
  if((version >= 0x1) && (version <= 0x3)) {
    command = (InstructionCommand) ntohl(i_ptr[1]);
    flags = ntohl(i_ptr[2]);
    uint32_t offset = 0;
    if ((flags & INSTRUCTION_FLAG_DEVSTR) == 0) {
      cfamid = ntohl(i_ptr[3]);
      linkid = ntohl(i_ptr[4]);
      offset = 2;
    }
    engineId = ntohl(i_ptr[3 + offset]);
    pin = ntohl(i_ptr[4 + offset]);
    mask = ntohl(i_ptr[5 + offset]);
    mode = (gpioDioMode_t) ntohl(i_ptr[6 + offset]);
    data = ntohl(i_ptr[7 + offset]);
    if ((flags & INSTRUCTION_FLAG_DEVSTR) == 0) {
      if (version >= 0x2) {
        cmaster = ntohl(i_ptr[8 + offset]);
      } else {
        cmaster = 0x0;
      }
    } else {
      uint32_t deviceStringSize = ntohl(i_ptr[8 + offset]);
      if (deviceStringSize > 0) {
        deviceString = ((char *)(i_ptr + 9 + offset));
      }
    }
  } else {
    error = rc = SERVER_UNKNOWN_INSTRUCTION_VERSION;
  }
  return rc;
}

uint32_t GPIOInstruction::flattenSize(void) const {
  if ((flags & INSTRUCTION_FLAG_DEVSTR) == 0) {
    if (version >= 0x2) {
      return (11 * sizeof(uint32_t));
    }
    return (10 * sizeof(uint32_t));
  } else {
    uint32_t deviceStringSize = deviceString.size() + 1;
    if (deviceStringSize % sizeof(uint32_t)) {
      deviceStringSize += (sizeof(uint32_t) - (deviceStringSize % sizeof(uint32_t)));
    }
    return (9 * sizeof(uint32_t)) + deviceStringSize;
  }
}

std::string GPIOInstruction::dumpInstruction(void) const {
  std::ostringstream oss;
  oss << "GPIOInstruction" << std::endl;
  oss << "version       : " << version << std::endl;
  oss << "command       : " << InstructionCommandToString(command) << std::endl;
  oss << "type          : " << InstructionTypeToString(type) << std::endl;
  oss << "flags         : " << InstructionFlagToString(flags) << std::endl;
  if (flags & INSTRUCTION_FLAG_DEVSTR) {
    oss << "deviceString  : " << deviceString << std::endl;
  } else {
    oss << "cfamid        : " << std::hex << std::setw(8) << std::setfill('0') << cfamid << std::dec << std::endl;
    oss << "linkid        : " << std::hex << std::setw(8) << std::setfill('0') << linkid << std::dec << std::endl;
    oss << "cmaster       : " << std::hex << std::setw(8) << std::setfill('0') << cmaster << std::dec << std::endl;
  }
  oss << "engineId      : " << std::hex << std::setw(8) << std::setfill('0') << engineId << std::dec << std::endl;
  oss << "pin           : " << std::hex << std::setw(8) << std::setfill('0') << pin << std::dec << std::endl;
  oss << "mask          : " << std::hex << std::setw(8) << std::setfill('0') << mask << std::dec << std::endl;
  oss << "mode          : " << gpioDioModeToString(mode) << std::endl;
  oss << "data          : " << std::hex << std::setw(8) << std::setfill('0') << data << std::dec << std::endl;

  return oss.str();
}

uint64_t GPIOInstruction::getHash(void) const {
  if (flags & INSTRUCTION_FLAG_DEVSTR) {
    uint32_t devstrhash = 0x0;
    uint32_t rc = devicestring_genhash(deviceString, devstrhash);
    uint64_t hash64 = 0x0ull;
    hash64 |= ((0x0000000Full & type)     << 60);
    if (rc == 0) {
        hash64 |= ((uint64_t) devstrhash);
    }
    return hash64;
  }
  //type,     cfamid(28:31), linkid(24:31), cmaster(24:31), engineId(24:31)
  //bits 0:3  bits 4:7,      bits 8:15,     bits 16:23,     bits 24:31
  uint32_t hash = 0x0;
  hash |= ((0x0000000F & type)     << 28);
  hash |= ((0x0000000F & cfamid)   << 24);
  hash |= ((0x000000FF & linkid)   << 16);
  hash |= ((0x000000FF & cmaster)  << 8);
  hash |= ((0x000000FF & engineId) << 0);
  return hash;
}

uint32_t GPIOInstruction::closeHandle(Handle ** i_handle) {
  int rc = 0;
  errno = 0;
  rc = gpio_close(*i_handle);
  *i_handle = NULL;
  if (rc) rc = SERVER_GPIO_CLOSE_FAIL;
  return rc;
}

std::string GPIOInstruction::getInstructionVars(const InstructionStatus & i_status) const {
  std::ostringstream oss;

  oss << std::hex << std::setfill('0');
  oss << "rc: " << std::setw(8) << i_status.rc;
  if (flags & INSTRUCTION_FLAG_DEVSTR) {
    oss << " devstr: " << deviceString;
  } else {
    oss << " cfamid: " << std::setw(8) << cfamid;
    oss << " linkid: " << std::setw(8) << linkid;
    oss << " cmaster: " << std::setw(8) << cmaster;
  }
  oss << " engineId: " << std::setw(8) << engineId;
  oss << " pin: " << std::setw(8) << pin;

  return oss.str();
}
