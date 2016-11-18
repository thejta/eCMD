//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG

#include <ServerFSIInstruction.H>
#include <OutputLite.H>
#include <stdio.h>

#include <adal_scom.h>
#include <adal_scan.h>
#include <adal_mbx.h>

//FIXME remove these
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
#ifdef TESTING
    TEST_PRINT("adal_scan_close((adal_t *) handle);\n");
    return 0;
#else
    return adal_scan_close((adal_t *) handle);
#endif
}

uint32_t ServerFSIInstruction::scom_close(Handle * handle)
{
#ifdef TESTING
    TEST_PRINT("adal_scom_close((adal_t *) handle);\n");
    return 0;
#else
    return adal_scom_close((adal_t *) handle);
#endif
}

uint32_t ServerFSIInstruction::gp_reg_close(Handle * handle)
{
    // close gp_reg device
    printf("system_gp_reg_close()\n");
    return 0;
}

uint32_t ServerFSIInstruction::mbx_close(Handle * handle)
{
#ifdef TESTING
    TEST_PRINT("adal_mbx_close((adal_t *) handle);\n");
    return 0;
#else
    return adal_mbx_close((adal_t *) handle);
#endif
}

ssize_t ServerFSIInstruction::scan_write(Handle * i_handle, InstructionStatus & o_status)
{
        ssize_t rc = 0;
        unsigned long options = 0;
        unsigned long status = 0;

        if (!(flags & INSTRUCTION_FLAG_FSI_SCANHEADERCHECK)) options |= SCANNOHEADERCHECK;
        if (flags & INSTRUCTION_FLAG_FSI_SCANSETPULSE) options |= SCANSETPULSE;
        if (flags & INSTRUCTION_FLAG_FSI_SCANEXTRABCLOCK) options |= SCANEXTRABCLOCK;
        if (flags & INSTRUCTION_FLAG_FSI_SCANVIAPIB) options |= SCANVIAPIB;

#ifdef TESTING
        TEST_PRINT("adal_scan_write((adal_t *) i_handle, ecmdDataBufferImplementationHelper::getDataPtr(&data), %08X, %u, options, &status);\n", address, length);
        rc = length % 8 ? (length / 8) + 1 : length / 8;
#else
        rc = adal_scan_write((adal_t *) i_handle,
            ecmdDataBufferImplementationHelper::getDataPtr(&data),
            address, length, options, &status);
#endif

        o_status.data.setBitLength(32);
        o_status.data.setWord(0, status);

        return rc;
}

ssize_t ServerFSIInstruction::scan_read(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status)
{
        ssize_t rc = 0;
        unsigned long options = 0;
        unsigned long status = 0;

        if (!(flags & INSTRUCTION_FLAG_FSI_SCANHEADERCHECK)) options |= SCANNOHEADERCHECK;
        if (flags & INSTRUCTION_FLAG_FSI_SCANSETPULSE) options |= SCANSETPULSE;
        if (flags & INSTRUCTION_FLAG_FSI_SCANEXTRABCLOCK) options |= SCANEXTRABCLOCK;
        if (flags & INSTRUCTION_FLAG_FSI_SCANVIAPIB) options |= SCANVIAPIB;

#ifdef TESTING
        TEST_PRINT("adal_scan_read((adal_t *) i_handle, ecmdDataBufferImplementationHelper::getDataPtr(&o_data), %08X, %u, options, &status);\n", address, length);
        rc = length % 8 ? (length / 8) + 1 : length / 8;
#else
        rc = adal_scan_read((adal_t *) i_handle,
            ecmdDataBufferImplementationHelper::getDataPtr(&o_data),
            address, length, options, &status);
#endif

        o_status.data.setBitLength(32);
        o_status.data.setWord(0, status);

        return rc;
}

ssize_t ServerFSIInstruction::scom_write(Handle * i_handle, InstructionStatus & o_status, uint32_t i_index)
{
        ssize_t rc = 0;
        uint64_t l_address = address;
        if (flags & INSTRUCTION_FLAG_64BIT_ADDRESS) {
          l_address = address64;
        }
	uint64_t l_data = data.getDoubleWord(i_index);
	unsigned long l_status = 0;
#ifdef TESTING
        TEST_PRINT("adal_scom_write((adal_t *) i_handle, %016llX, %016llX, &status);\n", l_data, l_address);
        rc = 8;
#else
        rc = adal_scom_write((adal_t *) i_handle, &l_data, l_address, &l_status);
#endif

        // FIXME may need to check scom status from 1007 register
        o_status.data.setBitLength(32);
        o_status.data.setWord(0, l_status);

        return rc;
}

ssize_t ServerFSIInstruction::scom_write_under_mask(Handle * i_handle, InstructionStatus & o_status)
{
        ssize_t rc = 0;
        uint64_t l_address = address;
        if (flags & INSTRUCTION_FLAG_64BIT_ADDRESS) {
          l_address = address64;
        }
	uint64_t l_data = data.getDoubleWord(0);
	uint64_t l_mask = mask.getDoubleWord(0);
	unsigned long l_status = 0;
#ifdef TESTING
        TEST_PRINT("adal_scom_write_under_mask((adal_t *) i_handle, %016llX, %016llX, %016llX, &status);\n", l_data, l_address, l_mask);
        rc = 8;
#else
        rc = adal_scom_write_under_mask((adal_t *) i_handle, &l_data, l_address, &l_mask, &l_status);
#endif

        // FIXME may need to check scom status from 1007 register
        o_status.data.setBitLength(32);
        o_status.data.setWord(0, l_status);

        return rc;
}

ssize_t ServerFSIInstruction::scom_read(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status, uint32_t i_index)
{
        ssize_t rc = 0;
        uint64_t l_address = address;
        if (flags & INSTRUCTION_FLAG_64BIT_ADDRESS) {
          l_address = address64;
        }
	uint64_t l_data = 0;
	unsigned long l_status = 0;
#ifdef TESTING
        TEST_PRINT("adal_scom_read((adal_t *) i_handle, &l_data, %016llX, &status);\n", l_address);
        rc = 8;
#else
        rc = adal_scom_read((adal_t *) i_handle, &l_data, l_address, &l_status);
#endif

        // FIXME may need to check scom status from 1007 register
        o_status.data.setBitLength(32);
        o_status.data.setWord(0, l_status);

	o_data.setDoubleWord(i_index, l_data);

        return rc;
}

ssize_t ServerFSIInstruction::scan_get_register(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status)
{
        ssize_t rc = 0;
        unsigned long l_data = 0;
#ifdef TESTING
        TEST_PRINT("adal_scan_get_register((adal_t *) i_handle, %08X, &l_data);\n", address);
        rc = 4;
#else
        rc = adal_scan_get_register((adal_t *) i_handle, address, &l_data);
#endif
        o_data.setWord(0, l_data);
        return rc;
}

ssize_t ServerFSIInstruction::scom_get_register(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status)
{
        ssize_t rc = 0;
        unsigned long l_data = 0;
#ifdef TESTING
        TEST_PRINT("adal_scom_get_register((adal_t *) i_handle, %08X, &l_data);\n", address);
        rc = 4;
#else
        rc = adal_scom_get_register((adal_t *) i_handle, address, &l_data);
#endif
        o_data.setWord(0, l_data);
        return rc;
}

ssize_t ServerFSIInstruction::mbx_get_scratch_register(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status)
{
          ssize_t rc = 0;

          adal_mbx_scratch_t scratch = ADAL_MBX_SCRATCH_0;
          switch (address & 0x0FFFFFFF)
          {
            case 0x2838:
              scratch = ADAL_MBX_SCRATCH_0;
              break;
            case 0x2839:
              scratch = ADAL_MBX_SCRATCH_1;
              break;
            case 0x283A:
              scratch = ADAL_MBX_SCRATCH_2;
              break;
            case 0x283B:
              scratch = ADAL_MBX_SCRATCH_3;
              break;
            case 0x283C:
              scratch = ADAL_MBX_SCRATCH_4;
              break;
            case 0x283D:
              scratch = ADAL_MBX_SCRATCH_5;
              break;
            case 0x283E:
              scratch = ADAL_MBX_SCRATCH_6;
              break;
            case 0x283F:
              scratch = ADAL_MBX_SCRATCH_7;
              break;

            default:
              rc = o_status.rc = SERVER_FSI_SCOM_READ_FAIL;
              break;
          }

	  if (!rc)
          {
#ifdef TESTING
              TEST_PRINT("adal_mbx_scratch((adal_t *) i_handle, %d, ADAL_MBX_SPAD_READ, ecmdDataBufferImplementationHelper::getDataPtr(&o_data));\n", scratch);
              rc = 0;
#else
              rc = adal_mbx_scratch((adal_t *) i_handle, scratch, ADAL_MBX_SPAD_READ, ecmdDataBufferImplementationHelper::getDataPtr(&o_data));
#endif
          }
          return rc;
}

ssize_t ServerFSIInstruction::mbx_get_register(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status)
{
        ssize_t rc = 0;
        unsigned long l_data = 0;
#ifdef TESTING
        TEST_PRINT("adal_mbx_get_register((adal_t *) i_handle, %08X, &l_data);\n", address);
        rc = 4;
#else
        rc = adal_mbx_get_register((adal_t *) i_handle, address, &l_data);
#endif
        o_data.setWord(0, l_data);
        return rc;
}

ssize_t ServerFSIInstruction::gp_reg_get(Handle * i_handle, ecmdDataBufferBase & o_data, InstructionStatus & o_status)
{
        printf("ServerFSIInstruction::gp_reg_get() address = %08X\n", address);
        return 0;
}

ssize_t ServerFSIInstruction::scan_set_register(Handle * i_handle, InstructionStatus & o_status)
{
        ssize_t rc = 0;
        unsigned long l_data = data.getWord(0);
#ifdef TESTING
        TEST_PRINT("adal_scan_set_register((adal_t *) i_handle, %08X, %08lX);\n", address, l_data);
        rc = 4;
#else
        rc = adal_scan_set_register((adal_t *) i_handle, address, &l_data);
#endif
        return rc;
}

ssize_t ServerFSIInstruction::scom_set_register(Handle * i_handle, InstructionStatus & o_status)
{
        ssize_t rc = 0;
        unsigned long l_data = data.getWord(0);
#ifdef TESTING
        TEST_PRINT("adal_scom_set_register((adal_t *) i_handle, %08X, %08lX);\n", address, l_data);
        rc = 4;
#else
        rc = adal_scom_set_register((adal_t *) i_handle, address, &l_data);
#endif
        return rc;
}

ssize_t ServerFSIInstruction::mbx_set_scratch_register(Handle * i_handle, InstructionStatus & o_status)
{
          ssize_t rc = 0;

          adal_mbx_scratch_t scratch = ADAL_MBX_SCRATCH_0;
          switch (address & 0x0FFFFFFF)
          {
            case 0x2838:
              scratch = ADAL_MBX_SCRATCH_0;
              break;
            case 0x2839:
              scratch = ADAL_MBX_SCRATCH_1;
              break;
            case 0x283A:
              scratch = ADAL_MBX_SCRATCH_2;
              break;
            case 0x283B:
              scratch = ADAL_MBX_SCRATCH_3;
              break;
            case 0x283C:
              scratch = ADAL_MBX_SCRATCH_4;
              break;
            case 0x283D:
              scratch = ADAL_MBX_SCRATCH_5;
              break;
            case 0x283E:
              scratch = ADAL_MBX_SCRATCH_6;
              break;
            case 0x283F:
              scratch = ADAL_MBX_SCRATCH_7;
              break;

            default:
              rc = o_status.rc = SERVER_FSI_SCOM_WRITE_FAIL;
              break;
          }

	  if (!rc)
          {
#ifdef TESTING
              TEST_PRINT("adal_mbx_scratch((adal_t *) i_handle, %d, ADAL_MBX_SPAD_WRITE, %08X);\n", scratch, data.getWord(0));
              rc = 0;
#else
              rc = adal_mbx_scratch((adal_t *) i_handle, scratch, ADAL_MBX_SPAD_WRITE, ecmdDataBufferImplementationHelper::getDataPtr(&data));
#endif
          }
          return rc;
}

ssize_t ServerFSIInstruction::mbx_set_register(Handle * i_handle, InstructionStatus & o_status)
{
        ssize_t rc = 0;
        unsigned long l_data = data.getWord(0);
#ifdef TESTING
        TEST_PRINT("adal_mbx_set_register((adal_t *) i_handle, %08X, %08lX);\n", address, l_data);
        rc = 4;
#else
        rc = adal_mbx_set_register((adal_t *) i_handle, address, &l_data);
#endif
        return rc;
}

ssize_t ServerFSIInstruction::gp_reg_set(Handle * i_handle, InstructionStatus & o_status)
{
        printf("ServerFSIInstruction::gp_reg_set() address = %08X\n", address);
        return 0;
}

