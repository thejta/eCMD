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


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <InstructionStatus.H>
#include <arpa/inet.h>
#include <stdio.h>
#include <cstring>

InstructionStatus::InstructionStatus(void): statusVersion(0x1), instructionVersion(0xFFFFFFFF), rc(0) {
}

InstructionStatus::~InstructionStatus(void) {
}

uint32_t InstructionStatus::flatten(uint8_t * o_data, uint32_t i_len) const {
  uint32_t local_rc = 0;
  uint32_t * o_ptr = (uint32_t *) o_data;
  
  if (i_len < flattenSize()) {
    printf("InstructionStatus:flatten: i_len %d bytes is too small to flatten\n", i_len);
    local_rc = 1;
  } else {
    // clear memory
    memset(o_data, 0, flattenSize());
    o_ptr[0] = htonl(statusVersion);
    o_ptr[1] = htonl(instructionVersion);
    o_ptr[2] = htonl(rc);
    uint32_t dataSize = data.flattenSize();
    o_ptr[3] = htonl(dataSize);
    data.flatten((uint8_t * ) (o_ptr + 4), dataSize);
    if (errorMessage.size() > 0) {
      strcpy(((char *)(o_ptr + 4)) + dataSize, errorMessage.c_str());
    } else {
      memset(((char *)(o_ptr + 4)) + dataSize, 0x0, 1);
    }
  }
  return local_rc;
}

uint32_t InstructionStatus::unflatten(const uint8_t * i_data, uint32_t i_len) {
  uint32_t local_rc = 0;
  uint32_t * i_ptr = (uint32_t *) i_data;
  statusVersion = ntohl(i_ptr[0]);
  if(statusVersion == 0x1) {
    instructionVersion = ntohl(i_ptr[1]);
    rc = ntohl(i_ptr[2]);
    uint32_t dataSize = ntohl(i_ptr[3]);
    local_rc = data.unflatten((uint8_t *) (i_ptr + 4), dataSize);
    errorMessage = ((char *)(i_ptr + 4)) + dataSize;
  } else {
    local_rc = 1;
  }
  return local_rc;
}

uint32_t InstructionStatus::flattenSize(void) const {
  return (4 * sizeof(uint32_t)) + data.flattenSize() + errorMessage.size() + 1;
}
