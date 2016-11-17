//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG

#include <ServerFSIInstruction.H>
#include <OutputLite.H>

#ifndef TESTING
#include <adal_scom.h>
#include <adal_scan.h>
#endif

//FIXME remove these
adal_t * adal_mbx_open(const char * device, int flags) { return NULL; }
adal_t * system_gp_reg_open(const char * device, int flags, int type) { return NULL; }
#define EDAL_GP_TYPE_MBX 0

uint32_t ServerFSIInstruction::scan_open(Handle ** handle, InstructionStatus & o_status) {
  uint32_t rc = 0;

  char device[50];
  char errstr[200];

  /* already have a handle lets reuse it */
  if(*handle != NULL)
    return rc;

  /* We need to open the device*/
  if (flags & INSTRUCTION_FLAG_DEVSTR) {
    snprintf(device, 50, "/dev/scan%s", deviceString.c_str());
  } else {
    //ERROR
    *handle = NULL;
    snprintf(errstr, 200, "ServerFSIInstruction::scan_open INSTRUCTION_FLAG_DEVSTR must be set\n");
    o_status.errorMessage.append(errstr);
    return SERVER_INVALID_FSI_DEVICE;
  }
  errno = 0;

  if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
    snprintf(errstr, 200, "SERVER_DEBUG : adal_scan_open(%s, O_RDWR | O_SYNC)\n", device);
    o_status.errorMessage.append(errstr);
  }

#ifdef TESTING
#ifndef TESTING_NO_DEBUG
  printf("*handle = adal_scan_open(%s, O_RDWR | O_SYNC);\n", device);
#endif
  *handle = (Handle *) 0x1;
#else
  *handle = (Handle *) adal_scan_open(device, O_RDWR | O_SYNC);
#endif

  if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
    snprintf(errstr, 200, "SERVER_DEBUG : adal_scan_open() *handle = %p\n", *handle);
    o_status.errorMessage.append(errstr);
  }

  if (*handle == NULL) {
    snprintf(errstr, 200, "ServerFSIInstruction::scan_open Problem opening FSI device %s : errno %d\n", device, errno);
    o_status.errorMessage.append(errstr);
    return SERVER_INVALID_FSI_DEVICE;
  }

  return rc;
}

uint32_t ServerFSIInstruction::scom_open(Handle** handle, InstructionStatus & o_status) {
  uint32_t rc = 0;

  char device[50];
  char errstr[200];

  /* already have a handle lets reuse it */
  if(*handle != NULL)
    return rc;

  /* We need to open the device*/
  if (flags & INSTRUCTION_FLAG_DEVSTR) {
    snprintf(device, 50, "/dev/scom%s", deviceString.c_str());
  } else {
    //ERROR
    *handle = NULL;
    snprintf(errstr, 200, "ServerFSIInstruction::scom_open INSTRUCTION_FLAG_DEVSTR must be set\n");
    o_status.errorMessage.append(errstr);
    return SERVER_INVALID_FSI_DEVICE;
  }
  errno = 0;

  if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
    snprintf(errstr, 200, "SERVER_DEBUG : adal_scom_open(%s, O_RDWR | O_SYNC)\n", device);
    o_status.errorMessage.append(errstr);
  }

#ifdef TESTING
#ifndef TESTING_NO_DEBUG
  printf("*handle = adal_scom_open(%s, O_RDWR | O_SYNC);\n", device);
#endif
  *handle = (Handle *) 0x1;
#else
  *handle = (Handle *) adal_scom_open(device, O_RDWR | O_SYNC);
#endif

  if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
    snprintf(errstr, 200, "SERVER_DEBUG : adal_scom_open() *handle = %p\n", *handle);
    o_status.errorMessage.append(errstr);
  }

  if (*handle == NULL) {
    snprintf(errstr, 200, "ServerFSIInstruction::scom_open Problem opening FSI device %s : errno %d\n", device, errno);
    o_status.errorMessage.append(errstr);
    return SERVER_INVALID_FSI_DEVICE;
  }

  return rc;
}

uint32_t ServerFSIInstruction::gp_reg_open(Handle** handle, InstructionStatus & o_status) {
  uint32_t rc = 0;

  char device[50];
  char errstr[200];

  /* already have a handle lets reuse it */
  if(*handle != NULL)
    return rc;

  /* We need to open the device*/
  if (flags & INSTRUCTION_FLAG_DEVSTR) {
    snprintf(device, 50, "/dev/mbx%s", deviceString.c_str());
  } else {
    //ERROR
    *handle = NULL;
    snprintf(errstr, 200, "ServerFSIInstruction::gp_reg_open INSTRUCTION_FLAG_DEVSTR must be set\n");
    o_status.errorMessage.append(errstr);
    return SERVER_INVALID_FSI_DEVICE;
  }
  errno = 0;

  if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
    snprintf(errstr, 200, "SERVER_DEBUG : system_gp_reg_open(%s, O_RDWR | O_SYNC, EDAL_GP_TYPE_MBX)\n", device);
    o_status.errorMessage.append(errstr);
  }

#ifdef TESTING
#ifndef TESTING_NO_DEBUG
  printf("*handle = system_gp_reg_open(%s, O_RDWR | O_SYNC, EDAL_GP_TYPE_MBX);\n", device);
#endif
  *handle = (Handle *) 0x1;
#else
  *handle = (Handle *) system_gp_reg_open(device, O_RDWR | O_SYNC, EDAL_GP_TYPE_MBX);
#endif

  if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
    snprintf(errstr, 200, "SERVER_DEBUG : system_gp_reg_open() *handle = %p\n", *handle);
    o_status.errorMessage.append(errstr);
  }

  if (*handle == NULL) {
    snprintf(errstr, 200, "ServerFSIInstruction::gp_reg_open Problem opening FSI device %s : errno %d\n", device, errno);
    o_status.errorMessage.append(errstr);
    return SERVER_INVALID_FSI_DEVICE;
  }

  return rc;
}

uint32_t ServerFSIInstruction::mbx_open(Handle** handle, InstructionStatus & o_status) {
  uint32_t rc = 0;

  char device[50];
  char errstr[200];

  /* already have a handle lets reuse it */
  if(*handle != NULL)
    return rc;

  /* We need to open the device*/
  if (flags & INSTRUCTION_FLAG_DEVSTR) {
    snprintf(device, 50, "/dev/mbx%s", deviceString.c_str());
  } else {
    //ERROR
    *handle = NULL;
    snprintf(errstr, 200, "ServerFSIInstruction::mbx_open INSTRUCTION_FLAG_DEVSTR must be set\n");
    o_status.errorMessage.append(errstr);
    return SERVER_INVALID_FSI_DEVICE;
  }
  errno = 0;

  if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
    snprintf(errstr, 200, "SERVER_DEBUG : adal_mbx_open(%s, O_RDWR | O_SYNC)\n", device);
    o_status.errorMessage.append(errstr);
  }

#ifdef TESTING
#ifndef TESTING_NO_DEBUG
  printf("*handle = adal_mbx_open(%s, O_RDWR | O_SYNC);\n", device);
#endif
  *handle = (Handle *) 0x1;
#else
  *handle = (Handle *) adal_mbx_open(device, O_RDWR | O_SYNC);
#endif

  if (flags & INSTRUCTION_FLAG_SERVER_DEBUG) {
    snprintf(errstr, 200, "SERVER_DEBUG : adal_mbx_open() *handle = %p\n", *handle);
    o_status.errorMessage.append(errstr);
  }

  if (*handle == NULL) {
    snprintf(errstr, 200, "ServerFSIInstruction::mbx_open Problem opening FSI device %s : errno %d\n", device, errno);
    o_status.errorMessage.append(errstr);
    return SERVER_INVALID_FSI_DEVICE;
  }

  return rc;
}

void ServerFSIInstruction::scan_ffdc_and_reset(Handle ** handle, InstructionStatus & o_status) {

    // system_scan_ffdc_extract
    //  append to o_status.errorMessage
    // system_scan_ffdc_unlock
    // system_scan_reset

    uint32_t rc = scan_close(*handle);
    *handle = NULL;
    if (rc) o_status.rc = SERVER_SCAN_CLOSE_FAIL;
}

void ServerFSIInstruction::scom_ffdc_and_reset(Handle ** handle, InstructionStatus & o_status) {

    // system_scom_ffdc_extract
    //  append to o_status.errorMessage
    // system_scom_ffdc_unlock
    // system_scom_reset

    uint32_t rc = scom_close(*handle);
    *handle = NULL;
    if (rc) o_status.rc = SERVER_SCOM_CLOSE_FAIL;
}

void ServerFSIInstruction::gp_reg_ffdc_and_reset(Handle ** handle, InstructionStatus & o_status) {

    // system_gp_reg_ffdc_extract
    //  append to o_status.errorMessage
    // system_gp_reg_ffdc_unlock
    // system_gp_reg_reset -- may not be available

    uint32_t rc = gp_reg_close(*handle);
    *handle = NULL;
    if (rc) o_status.rc = SERVER_SCOM_CLOSE_FAIL;

}

void ServerFSIInstruction::mbx_ffdc_and_reset(Handle ** handle, InstructionStatus & o_status) {

    // system_mbx_ffdc_extract
    //  append to o_status.errorMessage
    // system_mbx_ffdc_unlock
    // system_mbx_reset

    uint32_t rc = mbx_close(*handle);
    *handle = NULL;
    if (rc) o_status.rc = SERVER_SCOM_CLOSE_FAIL;

}

uint32_t ServerFSIInstruction::scan_close(Handle * handle)
{
    // close scan device
    printf("system_scan_close()\n");
    return 0;
}

uint32_t ServerFSIInstruction::scom_close(Handle * handle)
{
    // close scom device
    printf("system_scom_close()\n");
    return 0;
}

uint32_t ServerFSIInstruction::gp_reg_close(Handle * handle)
{
    // close gp_reg device
    printf("system_gp_reg_close()\n");
    return 0;
}

uint32_t ServerFSIInstruction::mbx_close(Handle * handle)
{
    // close mbx device
    printf("system_mbx_close()\n");
    return 0;
}

ssize_t ServerFSIInstruction::scan_write(Handle * i_handle, InstructionStatus & o_status)
{
/* 
 * handle these flags
        if (!(flags & INSTRUCTION_FLAG_FSI_SCANHEADERCHECK)) options |= SCANNOHEADERCHECK;
        if (flags & INSTRUCTION_FLAG_FSI_SCANSETPULSE) options |= SCANSETPULSE;
        if (flags & INSTRUCTION_FLAG_FSI_SCANEXTRABCLOCK) options |= SCANEXTRABCLOCK;
        if (flags & INSTRUCTION_FLAG_FSI_SCANVIAPIB) options |= SCANVIAPIB;
*/
        printf("ServerFSIInstruction::scan_write() address = %08X, length = %u\n", address, length);

        uint32_t status = 0; // fsi status register
        o_status.data.setBitLength(32);
        o_status.data.insert(status, 0, 32);

        return length % 8 ? (length / 8) + 1 : length / 8;
}

ssize_t ServerFSIInstruction::scan_read(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status)
{
/* 
 * handle these flags
        if (!(flags & INSTRUCTION_FLAG_FSI_SCANHEADERCHECK)) options |= SCANNOHEADERCHECK;
        if (flags & INSTRUCTION_FLAG_FSI_SCANSETPULSE) options |= SCANSETPULSE;
        if (flags & INSTRUCTION_FLAG_FSI_SCANEXTRABCLOCK) options |= SCANEXTRABCLOCK;
        if (flags & INSTRUCTION_FLAG_FSI_SCANVIAPIB) options |= SCANVIAPIB;
*/
        printf("ServerFSIInstruction::scan_read() address = %08X, length = %u\n", address, length);

        uint32_t status = 0; // fsi status register
        o_status.data.setBitLength(32);
        o_status.data.insert(status, 0, 32);

        return length % 8 ? (length / 8) + 1 : length / 8;
}

ssize_t ServerFSIInstruction::scom_write(Handle * i_handle, InstructionStatus & o_status, uint32_t i_index)
{
        if (flags & INSTRUCTION_FLAG_64BIT_ADDRESS) {
          printf("ServerFSIInstruction::scom_write() address64 = " UINT64_HEX_VARIN_FORMAT(%016) "\n", address64);
        } else {
          printf("ServerFSIInstruction::scom_write() address = %08X\n", address);
        }

        uint32_t status = 0x01000000;
        o_status.data.setBitLength(32);
        o_status.data.insert(status, 0, 32);

        return length % 8 ? (length / 8) + 1 : length / 8;
}

ssize_t ServerFSIInstruction::scom_write_under_mask(Handle * i_handle, InstructionStatus & o_status)
{
        if (flags & INSTRUCTION_FLAG_64BIT_ADDRESS) {
          printf("ServerFSIInstruction::scom_write_under_mask() address64 = " UINT64_HEX_VARIN_FORMAT(%016) "\n", address64);
        } else {
          printf("ServerFSIInstruction::scom_write_under_mask() address = %08X\n", address);
        }

        uint32_t status = 0x01000000;
        o_status.data.setBitLength(32);
        o_status.data.insert(status, 0, 32);

        return length % 8 ? (length / 8) + 1 : length / 8;
}

ssize_t ServerFSIInstruction::scom_read(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status, uint32_t i_index)
{
        if (flags & INSTRUCTION_FLAG_64BIT_ADDRESS) {
          printf("ServerFSIInstruction::scom_read() address64 = " UINT64_HEX_VARIN_FORMAT(%016) "\n", address64);
        } else {
          printf("ServerFSIInstruction::scom_read() address = %08X\n", address);
        }

        uint32_t status = 0x01000000;
        o_status.data.setBitLength(32);
        o_status.data.insert(status, 0, 32);

        return length % 8 ? (length / 8) + 1 : length / 8;
}

ssize_t ServerFSIInstruction::scan_get_register(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status)
{
        printf("ServerFSIInstruction::scan_get_register() address = %08X\n", address);
        return 4;
}

ssize_t ServerFSIInstruction::scom_get_register(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status)
{
        printf("ServerFSIInstruction::scom_get_register() address = %08X\n", address);
        return 4;
}

ssize_t ServerFSIInstruction::mbx_get_scratch_register(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status)
{
#if 0
          mbx_scratch_t scratch = MBX_SCRATCH_0;
          switch (address & 0x0FFFFFFF)
          {
            case 0x2838:
              scratch = MBX_SCRATCH_0;
              break;
            case 0x2839:
              scratch = MBX_SCRATCH_1;
              break;
            case 0x283A:
              scratch = MBX_SCRATCH_2;
              break;
            case 0x283B:
              scratch = MBX_SCRATCH_3;
              break;
            case 0x283C:
              scratch = MBX_SCRATCH_4;
              break;
            case 0x283D:
              scratch = MBX_SCRATCH_5;
              break;
            case 0x283E:
              scratch = MBX_SCRATCH_6;
              break;
            case 0x283F:
              scratch = MBX_SCRATCH_7;
              break;

            default:
              rc = o_status.rc = SERVER_FSI_SCOM_READ_FAIL;
              break;

          }
#endif
        printf("ServerFSIInstruction::mbx_get_scratch_register() address = %08X\n", address);
        return 0;
}

ssize_t ServerFSIInstruction::mbx_get_register(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status)
{
        printf("ServerFSIInstruction::mbx_get_register() address = %08X\n", address);
        return 4;
}

ssize_t ServerFSIInstruction::gp_reg_get(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status)
{
        printf("ServerFSIInstruction::gp_reg_get() address = %08X\n", address);
        return 0;
}

ssize_t ServerFSIInstruction::scan_set_register(Handle * i_handle, InstructionStatus & o_status)
{
        printf("ServerFSIInstruction::scan_set_register() address = %08X\n", address);
        return 4;
}

ssize_t ServerFSIInstruction::scom_set_register(Handle * i_handle, InstructionStatus & o_status)
{
        printf("ServerFSIInstruction::scom_set_register() address = %08X\n", address);
        return 4;
}

ssize_t ServerFSIInstruction::mbx_set_scratch_register(Handle * i_handle, InstructionStatus & o_status)
{
#if 0
          mbx_scratch_t scratch = MBX_SCRATCH_0;
          switch (address & 0x0FFFFFFF)
          {
            case 0x2838:
              scratch = MBX_SCRATCH_0;
              break;
            case 0x2839:
              scratch = MBX_SCRATCH_1;
              break;
            case 0x283A:
              scratch = MBX_SCRATCH_2;
              break;
            case 0x283B:
              scratch = MBX_SCRATCH_3;
              break;
            case 0x283C:
              scratch = MBX_SCRATCH_4;
              break;
            case 0x283D:
              scratch = MBX_SCRATCH_5;
              break;
            case 0x283E:
              scratch = MBX_SCRATCH_6;
              break;
            case 0x283F:
              scratch = MBX_SCRATCH_7;
              break;

            default:
              rc = o_status.rc = SERVER_FSI_SCOM_WRITE_FAIL;
              break;

          }
#endif
        printf("ServerFSIInstruction::mbx_set_scratch_register() address = %08X\n", address);
        return 0;
}

ssize_t ServerFSIInstruction::mbx_set_register(Handle * i_handle, InstructionStatus & o_status)
{
        printf("ServerFSIInstruction::mbx_set_register() address = %08X\n", address);
        return 4;
}

ssize_t ServerFSIInstruction::gp_reg_set(Handle * i_handle, InstructionStatus & o_status)
{
        printf("ServerFSIInstruction::gp_reg_set() address = %08X\n", address);
        return 0;
}

