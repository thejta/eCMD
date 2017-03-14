//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG

#include <ServerGPIOInstruction.H>
#include <OutputLite.H>
#include <stdio.h>
#include <errno.h>

//FIXME remove these
Handle * system_gpio_open(const char * device, int flags) { return NULL; }

uint32_t ServerGPIOInstruction::gpio_open(Handle ** handle, InstructionStatus & o_status)
{
  uint32_t rc = 0;

  char device[50];
  char errstr[200];

  /* already have a handle lets reuse it */
  if(*handle != NULL)
    return rc;

    /* Open the device */
    if (flags & INSTRUCTION_FLAG_DEVSTR) {
      snprintf(device, 50, "/dev/gpio/%sE%02dP00", deviceString.c_str(), engineId);
    } else {
      snprintf(device, 50, "/dev/gpio/%d/%d/%0*X/0", linkid, cfamid, (cmaster >> 28), ((cmaster << 8) | engineId));
    }
    errno = 0;

    if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
      snprintf(errstr, 200, "SERVER_DEBUG : system_gpio_open(%s, 0)\n", device);
      o_status.errorMessage.append(errstr);
    }

#ifdef TESTING
#ifndef TESTING_NO_DEBUG
    printf("*handle = system_gpio_open(%s, 0);\n",device);
#endif
    *handle = (Handle *) 0x1;
#else
    *handle = system_gpio_open(device, 0);
#endif

    if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
      snprintf(errstr, 200, "SERVER_DEBUG : system_gpio_open() *handle = %p\n", *handle);
      o_status.errorMessage.append(errstr);
    }

    if (*handle == NULL) {
      snprintf(errstr, 200, "ServerGPIOInstruction::gpio_open Problem opening GPIO device %s : errno %d\n", device, errno);
      o_status.errorMessage.append(errstr);
      rc = o_status.rc = SERVER_GPIO_OPEN_FAIL;
      return rc;
    }

    return rc;
}

void ServerGPIOInstruction::gpio_ffdc(Handle ** handle, InstructionStatus & o_status)
{
    // system_gpio_ffdc_extract
    //  append to o_status.errorMessage
    // system_gpio_ffdc_unlock

    uint32_t rc = gpio_close(*handle);
    *handle = NULL;
    if (rc) o_status.rc = SERVER_GPIO_CLOSE_FAIL;
}

uint32_t ServerGPIOInstruction::gpio_close(Handle * handle)
{
    // close gpio device
    printf("system_gpio_close()\n");
    return 0;
}

uint32_t ServerGPIOInstruction::gpio_set_mode(Handle * i_handle, InstructionStatus & o_status, gpioDioMode_t i_mode)
{
#if 0
    dioMode_t localMode;
    if (i_mode == GPIO_DIO_INPUT) {
      localMode = eInput;
    } else if (i_mode == GPIO_DIO_OPEN_DRAIN) {
      localMode = eOpenDrain;
    } else if (i_mode == GPIO_DIO_OPEN_SOURCE) {
      localMode = eOpenSource;
    } else if (i_mode == GPIO_DIO_PUSH_PULL) {
      localMode = ePushPull;
    } else if (i_mode == GPIO_DIO_UNCONFIGURED) {
      localMode = eUnconfigured;
    } else {
      snprintf(errstr, 200, "GPIOInstruction::execute Unknown config type received on server!\n");
      o_status.errorMessage.append(errstr);
      rc = o_status.rc = SERVER_GPIO_GENERAL_ERROR;
      return rc;
    }
#endif
    printf("system_gpio_set_mode()\n");
    return 0;
}

uint32_t ServerGPIOInstruction::gpio_read_pin(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status)
{
    // use pin variable
    printf("system_gpio_read_pin()\n");
    return 0;
}

uint32_t ServerGPIOInstruction::gpio_read_input_pins(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status)
{
    // use mask variable
    printf("system_gpio_read_input_pins()\n");
    return 0;
}

uint32_t ServerGPIOInstruction::gpio_read_latch(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status)
{
    // use pin variable
    printf("system_gpio_read_latch()\n");
    return 0;
}

uint32_t ServerGPIOInstruction::gpio_write_pin(Handle * i_handle, InstructionStatus & o_status)
{
    // use pin and data variables
    printf("system_gpio_write_pin()\n");
    return 0;
}

uint32_t ServerGPIOInstruction::gpio_write_output_pins(Handle * i_handle, InstructionStatus & o_status)
{
    // use mask and data variables
    printf("system_gpio_write_output_pins()\n");
    return 0;
}

uint32_t ServerGPIOInstruction::gpio_read_config(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status)
{
    // use pin variable
    printf("system_gpio_read_config()\n");
    return 0;
}

uint32_t ServerGPIOInstruction::gpio_write_config(Handle * i_handle, InstructionStatus & o_status)
{
    // use pin and data variables
    printf("system_gpio_write_config()\n");
    return 0;
}

uint32_t ServerGPIOInstruction::gpio_write_config_set_bit(Handle * i_handle, InstructionStatus & o_status)
{
    // use pin and data variables
    printf("system_gpio_write_config_set_bit()\n");
    return 0;
}

uint32_t ServerGPIOInstruction::gpio_write_config_clear_bit(Handle * i_handle, InstructionStatus & o_status)
{
    // use pin and data variables
    printf("system_gpio_write_config_clear_bit()\n");
    return 0;
}
