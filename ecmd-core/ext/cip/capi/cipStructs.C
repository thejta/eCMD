//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2003,2019 IBM International Business Machines Corp.
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

/*
 * @file cipStructs.C
 * @brief Converged IP Series eCMD Extension Structures
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <ecmdReturnCodes.H>
#include <ecmdStructs.H>
#include <ecmdSharedUtils.H> // for ecmdWriteTarget()
#include <cipStructs.H>
#include <math.h>
#include <netinet/in.h> //for htonl ntohl
#include <stdio.h>
#include <string.h>
 

//--------------------------------------------------------------------
//  Member Functions                                                
//--------------------------------------------------------------------


/*
 * The following methods for the cipSoftwareEvent_t struct will flatten,
 * unflatten & get the flattened size of the struct.
 */
uint32_t cipSoftwareEvent_t::flatten(uint8_t *o_buf, uint32_t &i_len)
{
  uint32_t tmpData32 = 0;
  uint32_t l_rc = ECMD_SUCCESS;

  uint32_t l_tgtSize = 0; // holder for ecmdChipTarget size
  int l_len = (int)i_len;   // use a local copy to decrement
  uint8_t *l_ptr8 = o_buf;  // pointer to the output buffer

  do      // Single entry ->
  {
    // Check for buffer overflow conditions.
    if (this->flattenSize() > i_len)
    {
      // Generate an error for buffer overflow conditions.
      ETRAC2("ECMD: Buffer overflow occurred in "
             "cipSoftwareEvent_t::flatten(), "
             "structure size = %d; input length = %d",
             this->flattenSize(), i_len);
      l_rc = ECMD_DATA_OVERFLOW;
      break;
    }

    // Flatten and store each data member in the ouput buffer

    // "event" (cipEventType_t, stored as uint32_t)
    tmpData32 = htonl( (uint32_t)event );
    memcpy( l_ptr8, &tmpData32, sizeof(tmpData32) );
    l_ptr8 += sizeof(tmpData32);
    l_len  -= sizeof(tmpData32);

    // "seqId" (uint32_t)
    tmpData32 = htonl( seqId );
    memcpy( l_ptr8, &tmpData32, sizeof(tmpData32) );
    l_ptr8 += sizeof(seqId);
    l_len  -= sizeof(seqId);

    // "rel_thread" (uint32_t)
    tmpData32 = htonl( rel_thread );
    memcpy( l_ptr8, &tmpData32, sizeof(tmpData32) );
    l_ptr8 += sizeof(rel_thread);
    l_len  -= sizeof(rel_thread);

    // "target" (ecmdChipTarget)
    //  1st save the size of the buffer
    l_tgtSize = target.flattenSize();
    tmpData32 = htonl( l_tgtSize );
    memcpy( l_ptr8, &tmpData32, sizeof(tmpData32) );
    l_ptr8 += sizeof( l_tgtSize );
    l_len  -= sizeof( l_tgtSize );

    //  2nd, flatten and save the buffer
    l_rc = target.flatten( l_ptr8, l_tgtSize );

    // If the flatten failed, exit now with an error
    if ( l_rc != ECMD_DBUF_SUCCESS )
    {
      ETRAC4("ECMD: Fail to flatten ecmdChipTarget in "
             "cipSoftwareEvent_t::flatten(), struct size= %d; "
             "input length= %d; remainder= %d, l_tgt_size=%d\n",
             this->flattenSize(), i_len, l_len, l_tgtSize);
      break;  // exit the do loop
    }
    else
    {
      l_rc = ECMD_SUCCESS;  // restore standard success value
    }
    l_ptr8 += l_tgtSize;
    l_len  -= l_tgtSize;

    // Final check: if the length isn't 0, something went wrong
    if (l_len != 0)
    {
      // Generate an error for buffer overflow conditions.
      ETRAC3("ECMD: Buffer overflow occurred in "
             "cipSoftwareEvent_t::flatten(), struct size= %d; "
             "input length= %d; remainder= %d\n",
             this->flattenSize(), i_len, l_len);
      l_rc = ECMD_DATA_OVERFLOW;
      break;
    }

  } while (false);   // <- single exit

  return l_rc;
}


uint32_t cipSoftwareEvent_t::unflatten(const uint8_t *i_buf, uint32_t &i_len)
{

  uint32_t l_rc = ECMD_SUCCESS;
  uint32_t tmpData32 = 0;

  uint32_t l_tgtSize = 0;       // holds size of ecmdChipTarget
  uint32_t l_len = (int)i_len; 
  const uint8_t *l_ptr8 = i_buf;  // pointer to the input buffer

  do    // Single entry ->
  {
    // Unflatten each data member from the input buffer

    // "event" (cipEventType_t, stored as uint32_t)
    memcpy( &tmpData32, l_ptr8, sizeof(tmpData32) );
    event = (cipEventType_t)ntohl( tmpData32 );
    l_ptr8 += sizeof(tmpData32);
    l_len  -= sizeof(tmpData32);

    // "seqId" (uint32_t)
    memcpy( &seqId, l_ptr8, sizeof(seqId) );
    seqId = ntohl( seqId );
    l_ptr8 += sizeof(seqId);
    l_len  -= sizeof(seqId);

    // "rel_thread" (uint32_t)
    memcpy( &rel_thread, l_ptr8, sizeof(rel_thread) );
    rel_thread = ntohl( rel_thread );
    l_ptr8 += sizeof(rel_thread);
    l_len  -= sizeof(rel_thread);

    // "target" (ecmdChipTarget)
    //  1st get the size of the ecmdChipTarget
    memcpy( &l_tgtSize, l_ptr8, sizeof(l_tgtSize) );
    l_tgtSize = ntohl( l_tgtSize );
    l_ptr8 += sizeof( l_tgtSize );
    l_len  -= sizeof( l_tgtSize );

    //  2nd, unflatten the ecmdChipTarget "target"
    l_rc = target.unflatten( l_ptr8, l_tgtSize );

    // If the unflatten failed, exit now with an error
    if ( l_rc != ECMD_DBUF_SUCCESS )
    {
      ETRAC4("ECMD: Fail to unflatten ecmdChipTarget in "
             "cipSoftwareEvent_t::unflatten(), struct size= %d; "
             "input length= %d; remainder= %d, l_tgt_size=%d\n",
             this->flattenSize(), i_len, l_len, l_tgtSize);
      break;
    }
    else
    {
      l_rc = ECMD_SUCCESS;  // restore standard success value
    }
    l_ptr8 += l_tgtSize;
    l_len  -= l_tgtSize;

    // Final check: if the length isn't 0, something went wrong
    if (l_len != 0)
    {
      // Generate an error for buffer overflow conditions.
      ETRAC3("ECMD: Buffer overflow occurred in "
             "cipSoftwareEvent_t::unflatten(), struct size= %d; "
             "input length= %d; remainder= %d\n",
             this->flattenSize(), i_len, l_len);
      l_rc = ECMD_DATA_OVERFLOW;
      break;
    }

  } while (false);   // <- single exit

  return l_rc;
}


uint32_t cipSoftwareEvent_t::flattenSize(void)
{

    uint32_t flatSize = 0;

    flatSize += sizeof(uint32_t); // Event Type enum stored as uint32_t
    flatSize += sizeof(seqId); // seqId
    flatSize += sizeof(rel_thread); // rel_thread
    flatSize += sizeof(uint32_t);   // Size of ecmdChipTarget stored in uint32_t
    flatSize += target.flattenSize(); // flattened ecmdChipTarget

    return flatSize;
}

void cipSoftwareEvent_t::printStruct(){

  printf("\ncipSoftwareEvent_t Struct\n");

  printf("\tevent = %d\n", event);
  printf("\tseqId = %d\n", seqId);
  printf("\trel_thread = %d\n", rel_thread);
  printf("\ttarget = %s\n", ecmdWriteTarget(target).c_str());

}

/*
 * The following methods for the cipBrkptTableEntry struct will flatten,
 * unflatten & get the flattened size of the struct.
 */
uint32_t cipBrkptTableEntry::flatten(uint8_t *o_buf, uint32_t &i_len)
{
  uint32_t tmpData32 = 0;
  uint32_t l_rc = ECMD_SUCCESS;

  uint32_t l_tgtSize = 0; // holder for ecmdChipTarget size
  int l_len = (int)i_len;   // use a local copy to decrement
  uint8_t *l_ptr8 = o_buf;  // pointer to the output buffer

  do      // Single entry ->
  {
    // Check for buffer overflow conditions.
    if (this->flattenSize() > i_len)
    {
      // Generate an error for buffer overflow conditions.
      ETRAC2("ECMD: Buffer overflow occurred in "
             "cipBrkptTableEntry::flatten(), "
             "structure size = %d; input length = %d",
             this->flattenSize(), i_len);
      l_rc = ECMD_DATA_OVERFLOW;
      break;
    }

    // Flatten and store each data member in the ouput buffer
    
    // "Address" (ecmdDataBuffer)
    //  1st save the size of the buffer
    l_tgtSize = Address.flattenSize();
    tmpData32 = htonl( l_tgtSize );
    memcpy( l_ptr8, &tmpData32, sizeof(tmpData32) );
    l_ptr8 += sizeof( l_tgtSize );
    l_len  -= sizeof( l_tgtSize );

    //  2nd, flatten and save the buffer
    l_rc = Address.flatten( l_ptr8, l_tgtSize );

    // If the flatten failed, exit now with an error
    if ( l_rc != ECMD_DBUF_SUCCESS )
    {
      ETRAC4("ECMD: Fail to flatten ecmdDataBuffer Address in "
             "cipBrkptTableEntry::flatten(), struct size= %d; "
             "input length= %d; remainder= %d, l_tgt_size=%d\n",
             this->flattenSize(), i_len, l_len, l_tgtSize);
      break;  // exit the do loop
    }
    else
    {
      l_rc = ECMD_SUCCESS;  // restore standard success value
    }
    l_ptr8 += l_tgtSize;
    l_len  -= l_tgtSize;


    // "Original_Instruction" (ecmdDataBuffer)
    //  1st save the size of the buffer
    l_tgtSize = Original_Instruction.flattenSize();
    tmpData32 = htonl( l_tgtSize );
    memcpy( l_ptr8, &tmpData32, sizeof(tmpData32) );
    l_ptr8 += sizeof( l_tgtSize );
    l_len  -= sizeof( l_tgtSize );

    //  2nd, flatten and save the buffer
    l_rc = Original_Instruction.flatten( l_ptr8, l_tgtSize );

    // If the flatten failed, exit now with an error
    if ( l_rc != ECMD_DBUF_SUCCESS )
    {
      ETRAC4("ECMD: Fail to flatten ecmdDataBuffer Original_Instruction in "
             "cipBrkptTableEntry::flatten(), struct size= %d; "
             "input length= %d; remainder= %d, l_tgt_size=%d\n",
             this->flattenSize(), i_len, l_len, l_tgtSize);
      break;  // exit the do loop
    }
    else
    {
      l_rc = ECMD_SUCCESS;  // restore standard success value
    }
    l_ptr8 += l_tgtSize;
    l_len  -= l_tgtSize;


    // "Partition_Id"
    tmpData32 = htonl(Partition_Id);
    memcpy(l_ptr8, &tmpData32, sizeof(Partition_Id));
    l_ptr8 += sizeof(Partition_Id);
    l_len  -= sizeof(Partition_Id);

    // "Installed"
    memcpy(l_ptr8, &Installed, sizeof(Installed));
    l_ptr8 += sizeof(Installed);
    l_len  -= sizeof(Installed);

    // "TA"
    memcpy(l_ptr8, &TA, sizeof(TA));
    l_ptr8 += sizeof(TA);
    l_len  -= sizeof(TA);

    // "Addr_Mode"
    memcpy(l_ptr8, &Addr_Mode, sizeof(Addr_Mode));
    l_ptr8 += sizeof(Addr_Mode);
    l_len  -= sizeof(Addr_Mode);

    // "Addr_Type"
    memcpy(l_ptr8, &Addr_Type, sizeof(Addr_Type));
    l_ptr8 += sizeof(Addr_Type);
    l_len  -= sizeof(Addr_Type);

    // Final check: if the length isn't 0, something went wrong
    if (l_len != 0)
    {
      // Generate an error for buffer overflow conditions.
      ETRAC3("ECMD: Buffer overflow occurred in "
             "cipBrkptTableEntry::flatten(), struct size= %d; "
             "input length= %d; remainder= %d\n",
             this->flattenSize(), i_len, l_len);
      l_rc = ECMD_DATA_OVERFLOW;
      break;
    }

  } while (false);   // <- single exit

  return l_rc;
}


uint32_t cipBrkptTableEntry::unflatten(const uint8_t *i_buf, uint32_t &i_len)
{

  uint32_t l_rc = ECMD_SUCCESS;

  uint32_t l_tgtSize = 0;       // holds size of ecmdChipTarget
  uint32_t l_len = (int)i_len;
  const uint8_t *l_ptr8 = i_buf;  // pointer to the input buffer

  do    // Single entry ->
  {
    // Unflatten each data member from the input buffer

    // "Address" (ecmdDataBuffer)
    //  1st get the size of the ecmdDataBuffer
    memcpy( &l_tgtSize, l_ptr8, sizeof(l_tgtSize) );
    l_tgtSize = ntohl( l_tgtSize );
    l_ptr8 += sizeof( l_tgtSize );
    l_len  -= sizeof( l_tgtSize );
    
    //  2nd, unflatten the ecmdDataBuffer "Address"
    l_rc = Address.unflatten( l_ptr8, l_tgtSize );

    // If the unflatten failed, exit now with an error
    if ( l_rc != ECMD_DBUF_SUCCESS )
    {
      ETRAC4("ECMD: Fail to unflatten ecmdDataBuffer Address in "
             "cipBrkptTableEntry::unflatten(), struct size= %d; "
             "input length= %d; remainder= %d, l_tgt_size=%d\n",
             this->flattenSize(), i_len, l_len, l_tgtSize);
      break; 
    }
    else
    {
      l_rc = ECMD_SUCCESS;  // restore standard success value
    }
    l_ptr8 += l_tgtSize;
    l_len  -= l_tgtSize;

    // "Original_Instruction" (ecmdDataBuffer)
    //  1st get the size of the ecmdDataBuffer
    memcpy( &l_tgtSize, l_ptr8, sizeof(l_tgtSize) );
    l_tgtSize = ntohl( l_tgtSize );
    l_ptr8 += sizeof( l_tgtSize );
    l_len  -= sizeof( l_tgtSize );
    
    //  2nd, unflatten the ecmdDataBuffer "Address"
    l_rc = Original_Instruction.unflatten( l_ptr8, l_tgtSize );
    
    // If the unflatten failed, exit now with an error
    if ( l_rc != ECMD_DBUF_SUCCESS )
    {
      ETRAC4("ECMD: Fail to unflatten ecmdDataBuffer Original_Instruction in "
             "cipBrkptTableEntry::unflatten(), struct size= %d; "
             "input length= %d; remainder= %d, l_tgt_size=%d\n",
             this->flattenSize(), i_len, l_len, l_tgtSize);
      break;
    }
    else
    {
      l_rc = ECMD_SUCCESS;  // restore standard success value
    }
    l_ptr8 += l_tgtSize;
    l_len  -= l_tgtSize;

    // "Partition_Id"
    memcpy(&Partition_Id, l_ptr8, sizeof(Partition_Id));
    Partition_Id = ntohl(Partition_Id);
    l_ptr8 += sizeof(Partition_Id);
    l_len  -= sizeof(Partition_Id);

    // "Installed"
    memcpy(&Installed, l_ptr8, sizeof(Installed));
    l_ptr8 += sizeof(Installed);
    l_len  -= sizeof(Installed);

    // "TA"
    memcpy(&TA, l_ptr8, sizeof(TA));
    l_ptr8 += sizeof(TA);
    l_len  -= sizeof(TA);

    // "Addr_Mode"
    memcpy(&Addr_Mode, l_ptr8, sizeof(Addr_Mode));
    l_ptr8 += sizeof(Addr_Mode);
    l_len  -= sizeof(Addr_Mode);

    // "Addr_Type"
    memcpy(&Addr_Type, l_ptr8, sizeof(Addr_Type));
    l_ptr8 += sizeof(Addr_Type);
    l_len  -= sizeof(Addr_Type);

    // Final check: if the length isn't 0, something went wrong
    if (l_len != 0)
    {
      // Generate an error for buffer overflow conditions.
      ETRAC3("ECMD: Buffer overflow occurred in "
             "cipBrkptTableEntry::unflatten(), struct size= %d; "
             "input length= %d; remainder= %d\n",
             this->flattenSize(), i_len, l_len);
      l_rc = ECMD_DATA_OVERFLOW;
      break;
    }
  
  } while (false);   // <- single exit
    
  return l_rc;
}
    
    
uint32_t cipBrkptTableEntry::flattenSize(void)
{   

    uint32_t flatSize = 0;
    
    flatSize += sizeof(uint32_t);      // Size of ecmdDataBuffer 'Address' stored in uint32_t
    flatSize += Address.flattenSize(); // flattened ecmdDataBuffer
    flatSize += sizeof(uint32_t);      // @06 Size of ecmdDataBuffer 'Original_Instruction' stored in uint32_t
    flatSize += Original_Instruction.flattenSize(); // flattened ecmdDataBuffer
    flatSize += sizeof(Partition_Id);  // Partition_Id
    flatSize += sizeof(Installed);     // Installed
    flatSize += sizeof(TA);            // TA
    flatSize += sizeof(Addr_Mode);     // Addr_Mode
    flatSize += sizeof(Addr_Type);     // Addr_Type

    return flatSize;
}

void cipBrkptTableEntry::printStruct(){

  printf("\ncipBrkptTableEntry Struct\n");

  printf("\tAddress = %s\n", Address.genHexLeftStr().c_str());
  printf("\tOriginal_Instruction = %s\n", Original_Instruction.genHexLeftStr().c_str());
  printf("\tPartition_Id = %d\n", Partition_Id);
  printf("\tInstalled = %d\n", Installed);
  printf("\tTA = %d\n", TA);
  printf("\tAddr_Mode = %d\n", Addr_Mode);
  printf("\tAddr_Type = %d\n", Addr_Type);
 
}


 
