// Copyright ***********************************************************
//                                                                      
// File ecmdStructs.C                                   
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 2002                                        
//                                                                      
// The source code for this program is not published or otherwise       
// divested of its trade secrets, irrespective of what has been         
// deposited with the U.S. Copyright Office.                            
//                                                                      
// End Copyright *******************************************************

/* $Header$ */

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define ecmdStructs_C
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>
#include <netinet/in.h>

#include <ecmdReturnCodes.H>
#include <ecmdStructs.H>
#undef ecmdStructs_C

//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Forward declarations
//----------------------------------------------------------------------

//---------------------------------------------------------------------
//  Constructors
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//  Destructor
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//  Public Member Function Specifications
//---------------------------------------------------------------------


/** @brief Used to sort Cage entries in an ecmdCageData list. */
bool operator< (const ecmdCageData& lhs, const ecmdCageData& rhs) {

    if (lhs.cageId < rhs.cageId)
        return 1;
        
    return 0;
}

/** @brief Used to sort Node entries in an ecmdNodeData list. */
bool operator< (const ecmdNodeData& lhs, const ecmdNodeData& rhs) {

    if (lhs.nodeId < rhs.nodeId)
        return 1;
        
    return 0;
}

/** @brief Used to sort Slot entries in an ecmdSlotData list. */
bool operator< (const ecmdSlotData& lhs, const ecmdSlotData& rhs) {

    if (lhs.slotId < rhs.slotId)
        return 1;
        
    return 0;
}

/** @brief Used to sort Chip entries (based on Pos) in an ecmdChipData list. */
bool operator< (const ecmdChipData& lhs, const ecmdChipData& rhs) {

    if (lhs.chipType < rhs.chipType)
      return 1;
    else if ((lhs.chipType == rhs.chipType) && (lhs.pos < rhs.pos))
      return 1;
        
    return 0;
}

/** @brief Used to sort Core entries in an ecmdCoreData list. */
bool operator< (const ecmdCoreData& lhs, const ecmdCoreData& rhs) {

    if (lhs.coreId < rhs.coreId)
        return 1;
        
    return 0;

}

/** @brief Used to sort Thread entries in an ecmdThreadData list. */
bool operator< (const ecmdThreadData& lhs, const ecmdThreadData& rhs) {

    if (lhs.threadId < rhs.threadId)
        return 1;
        
    return 0;

}

std::string ecmdGetSharedLibVersion() {
  return ECMD_CAPI_VERSION;
}


/*
 * This function will flatten the ecmdChipTarget struct for transport accross
 * the user <-> cecserver interface.  
 */
uint32_t ecmdChipTarget::flatten(uint8_t *o_buf, uint32_t i_len) {

	uint32_t rc    = ECMD_SUCCESS;
	int      l_len = (int) i_len;
	uint8_t *l_ptr = (uint8_t *) o_buf;

	ecmdChipTarget tmpTarget;

	do {	// Single entry, single exit.

		// Check for buffer overflow conditions.
		if (this->flattenSize() <= i_len) {
			// Make sure buffer is clear.
			memset(o_buf, 0, i_len);
		} else {
			// Handle the error case for buffer overflow.
			ETRAC2("Buffer overflow occured in "
			       "ecmdChipTarget::flatten() "
			       "structure size = %d; input length = %d", 
			       this->flattenSize(), i_len);
			rc = ECMD_DATA_OVERFLOW;
			break;
		}

		// Copy everything into temporary struct in htonl format.
		tmpTarget.cage	  = htonl(cage);
		tmpTarget.node	  = htonl(node);
		tmpTarget.slot	  = htonl(slot);
		tmpTarget.pos	   = htonl(pos);
		tmpTarget.core	  = core;
		tmpTarget.thread	= thread;
		tmpTarget.unitId	= htonl(unitId);
		(uint32_t)tmpTarget.cageState     = htonl((uint32_t)cageState);
		(uint32_t)tmpTarget.nodeState     = htonl((uint32_t)nodeState);
		(uint32_t)tmpTarget.slotState     = htonl((uint32_t)slotState);
		(uint32_t)tmpTarget.chipTypeState = 
						htonl((uint32_t)chipTypeState);
		(uint32_t)tmpTarget.posState      = htonl((uint32_t)posState);
		(uint32_t)tmpTarget.coreState     = htonl((uint32_t)coreState);
		(uint32_t)tmpTarget.threadState   = 
						htonl((uint32_t)threadState);
		(uint32_t)tmpTarget.unitIdState   = 
						htonl((uint32_t)unitIdState);
		tmpTarget.chipType	= chipType;

		// Now copy everything to the output buffer.
		memcpy(l_ptr, &(tmpTarget.cage), sizeof(tmpTarget.cage));
		l_ptr += sizeof(tmpTarget.cage);
		l_len -= sizeof(tmpTarget.cage);

		memcpy(l_ptr, &(tmpTarget.node), sizeof(tmpTarget.node));
		l_ptr += sizeof(tmpTarget.node);
		l_len -= sizeof(tmpTarget.node);
	
		memcpy(l_ptr, &(tmpTarget.slot), sizeof(tmpTarget.slot));
		l_ptr += sizeof(tmpTarget.slot);
		l_len -= sizeof(tmpTarget.slot);

		memcpy(l_ptr, &(tmpTarget.pos), sizeof(tmpTarget.pos));
		l_ptr += sizeof(tmpTarget.pos);
		l_len -= sizeof(tmpTarget.pos);

		memcpy(l_ptr, &(tmpTarget.core), sizeof(tmpTarget.core));
		l_ptr += sizeof(tmpTarget.core);
		l_len -= sizeof(tmpTarget.core);

		memcpy(l_ptr, &(tmpTarget.thread), sizeof(tmpTarget.thread));
		l_ptr += sizeof(tmpTarget.thread);
		l_len -= sizeof(tmpTarget.thread);

		memcpy(l_ptr, &(tmpTarget.unitId), sizeof(tmpTarget.unitId));
		l_ptr += sizeof(tmpTarget.unitId);
		l_len -= sizeof(tmpTarget.unitId);

		memcpy(l_ptr, 
		       &(tmpTarget.cageState), 
		       sizeof(tmpTarget.cageState));
		l_ptr += sizeof(tmpTarget.cageState);
		l_len -= sizeof(tmpTarget.cageState);

		memcpy(l_ptr, 
		       &(tmpTarget.nodeState), 
		       sizeof(tmpTarget.nodeState));
		l_ptr += sizeof(tmpTarget.nodeState);
		l_len -= sizeof(tmpTarget.nodeState);

		memcpy(l_ptr, 
		       &(tmpTarget.slotState), 
		       sizeof(tmpTarget.slotState));
		l_ptr += sizeof(tmpTarget.slotState);
		l_len -= sizeof(tmpTarget.slotState);

		memcpy(l_ptr, 
		       &(tmpTarget.chipTypeState), 
		       sizeof(tmpTarget.chipTypeState));
		l_ptr += sizeof(tmpTarget.chipTypeState);
		l_len -= sizeof(tmpTarget.chipTypeState);

		memcpy(l_ptr, 
		       &(tmpTarget.posState), 
		       sizeof(tmpTarget.posState));
		l_ptr += sizeof(tmpTarget.posState);
		l_len -= sizeof(tmpTarget.posState);

		memcpy(l_ptr, 
		       &(tmpTarget.coreState), 
		       sizeof(tmpTarget.coreState));
		l_ptr += sizeof(tmpTarget.coreState);
		l_len -= sizeof(tmpTarget.coreState);

		memcpy(l_ptr, 
		       &(tmpTarget.threadState), 
		       sizeof(tmpTarget.threadState));
		l_ptr += sizeof(tmpTarget.threadState);
		l_len -= sizeof(tmpTarget.threadState);

		memcpy(l_ptr, 
		       &(tmpTarget.unitIdState), 
		       sizeof(tmpTarget.unitIdState));
		l_ptr += sizeof(tmpTarget.unitIdState);
		l_len -= sizeof(tmpTarget.unitIdState);

		memcpy(l_ptr, 
		       tmpTarget.chipType.c_str(),
		       tmpTarget.chipType.size() + 1);  // +1 for sizeof NULL.
		l_ptr += tmpTarget.chipType.size() + 1;
		l_len -= tmpTarget.chipType.size() +1;

	} while (0);	// Single exit point.

	return rc; 

}	// End ecmdChipTarget::flatten()

uint32_t ecmdChipTarget::unflatten(const uint8_t *i_buf, uint32_t i_len) {

	uint32_t rc    = ECMD_SUCCESS;
	int      l_len = (int) i_len;
	uint8_t *l_ptr = (uint8_t *)i_buf;

	// Find offset to chipType string.
	uint8_t l_byteCount = sizeof(cage)
			      + sizeof(node)
			      + sizeof(slot)
			      + sizeof(pos)
			      + sizeof(core)
			      + sizeof(thread)
			      + sizeof(unitId)
			      + sizeof(cageState)
			      + sizeof(nodeState)
			      + sizeof(slotState)
			      + sizeof(chipTypeState)
			      + sizeof(posState)
			      + sizeof(coreState)
			      + sizeof(threadState)
			      + sizeof(unitIdState);

	const char *l_chipTypePtr = (char *)(l_ptr += l_byteCount);
	
	l_ptr = (uint8_t *)i_buf;
	// Finish calculating the size of the structure to be built.
	l_byteCount += (strlen(l_chipTypePtr) + 1);  // +1 for NULL terminator.

	do {	// Single entry ->

		if (l_byteCount > i_len) {
			// Generate an error for buffer overflow conditions.
			ETRAC2("Buffer overflow occured in "
                               "ecmdChipTarget::unflatten() "
                               "estimated structure size = %d; "
			       "input length = %d",
                               l_byteCount, i_len);
			rc = ECMD_DATA_OVERFLOW;
			break;
		}
		
		memcpy(&cage, l_ptr, sizeof(cage));
		cage = ntohl(cage);
		l_ptr += sizeof(cage);
		l_len -= sizeof(cage);

		memcpy(&node, l_ptr, sizeof(node));
		node = ntohl(node);
		l_ptr += sizeof(node);
		l_len -= sizeof(node);

		memcpy(&slot, l_ptr, sizeof(slot));
		slot = ntohl(slot);
		l_ptr += sizeof(slot);
		l_len -= sizeof(slot);

		memcpy(&pos, l_ptr, sizeof(pos));
		pos = ntohl(pos);
		l_ptr += sizeof(pos);
		l_len -= sizeof(pos);

		memcpy(&core, l_ptr, sizeof(core));
		l_ptr += sizeof(core);
		l_len -= sizeof(core);

		memcpy(&thread, l_ptr, sizeof(thread));
		l_ptr += sizeof(thread);
		l_len -= sizeof(thread);

		memcpy(&unitId, l_ptr, sizeof(unitId));
		unitId = ntohl(unitId);
		l_ptr += sizeof(unitId);
		l_len -= sizeof(unitId);

		memcpy(&cageState, l_ptr, sizeof(cageState));
		cageState = (ecmdChipTargetState_t) ntohl((uint32_t) cageState);
		l_ptr += sizeof((uint32_t) cageState);
		l_len -= sizeof((uint32_t) cageState);

		memcpy(&nodeState, l_ptr, sizeof(nodeState));
		nodeState = (ecmdChipTargetState_t) ntohl((uint32_t) nodeState);
		l_ptr += sizeof((uint32_t) nodeState);
		l_len -= sizeof((uint32_t) nodeState);

		memcpy(&slotState, l_ptr, sizeof(slotState));
		slotState = (ecmdChipTargetState_t) ntohl((uint32_t) slotState);
		l_ptr += sizeof((uint32_t) slotState);
		l_len -= sizeof((uint32_t) slotState);

		memcpy(&chipTypeState, l_ptr, sizeof(chipTypeState));
		chipTypeState = (ecmdChipTargetState_t) ntohl(
						      (uint32_t) chipTypeState);
		l_ptr += sizeof((uint32_t) chipTypeState);
		l_len -= sizeof((uint32_t) chipTypeState);

		memcpy(&posState, l_ptr, sizeof(posState));
		posState = (ecmdChipTargetState_t) ntohl((uint32_t) posState);
		l_ptr += sizeof((uint32_t) posState);
		l_len -= sizeof((uint32_t) posState);

		memcpy(&coreState, l_ptr, sizeof(coreState));
		coreState = (ecmdChipTargetState_t) ntohl((uint32_t) coreState);
		l_ptr += sizeof((uint32_t) coreState);
		l_len -= sizeof((uint32_t) coreState);

		memcpy(&threadState, l_ptr, sizeof(threadState));
		threadState = (ecmdChipTargetState_t) ntohl(
							(uint32_t) threadState);
		l_ptr += sizeof((uint32_t) threadState);
		l_len -= sizeof((uint32_t) threadState);

		memcpy(&unitIdState, l_ptr, sizeof(unitIdState));
		unitIdState = (ecmdChipTargetState_t) ntohl(
							(uint32_t) unitIdState);
		l_ptr += sizeof((uint32_t) unitIdState);
		l_len -= sizeof((uint32_t) unitIdState);

		chipType = (char *) l_ptr;
		l_len -= (chipType.size() + 1);

		// Check for buffer underflow conditions.
		if (l_len > 0) {
			ETRAC2("Buffer underflow occured in "
                               "ecmdChipTarget::unflatten() "
                               "estimated structure size = %d; "
                               "input length = %d",
                               l_byteCount, l_len);
			rc = ECMD_DATA_UNDERFLOW; 
			break;
		}

	} while(0);	// <- single exit.

	return rc;

}	// End ecmdChipTarget::unflatten()

uint32_t ecmdChipTarget::flattenSize() const {
	
	return (sizeof(cage)
		+ sizeof(node)
		+ sizeof(slot)
		+ sizeof(pos)
		+ sizeof(core)
		+ sizeof(thread)
		+ sizeof(unitId)
		+ sizeof(cageState)
		+ sizeof(nodeState)
		+ sizeof(slotState)
		+ sizeof(chipTypeState)
		+ sizeof(posState)
		+ sizeof(coreState)
		+ sizeof(threadState)
		+ sizeof(unitIdState)
		+ (chipType.size() + 1));
}

#ifndef REMOVE_SIM
void ecmdChipTarget::printStruct() const {

	// Print struct data.
	printf("\tCage: %d\n", cage);
	printf("\tNode: %d\n", node);
	printf("\tSlot: %d\n", slot);
	printf("\tPosition: %d\n", pos);
	printf("\tCore: %d\n", core);
	printf("\tThread: %d\n", thread);
	printf("\tUnitId: %d\n", unitId);
	printf("\tCage state: 0x%08x\n", (uint32_t) cageState);
	printf("\tNode state: 0x%08x\n", (uint32_t) nodeState);
	printf("\tSlot state: 0x%08x\n", (uint32_t) slotState);
	printf("\tChip Type state: 0x%08x\n", (uint32_t) chipTypeState);
	printf("\tPosition state: 0x%08x\n", (uint32_t) posState);
	printf("\tCore state: 0x%08x\n", (uint32_t) coreState);
	printf("\tThread state: 0x%08x\n", (uint32_t) threadState);
	printf("\tUnit ID state: 0x%08x\n", (uint32_t) unitIdState);
	printf("\tChip Type: %s\n", chipType.c_str());
}
#endif

/*
 * The following methods for the ecmdThreadData struct will flatten, unflatten &
 * get the flattened size of the struct.
 */
uint32_t ecmdThreadData::flatten(uint8_t *o_buf, uint32_t &i_len) {

	uint32_t tmpData32 = 0;
	uint32_t rc        = ECMD_SUCCESS;

	uint8_t *l_ptr = o_buf;

	do {	// Single entry ->

		// Check for buffer overflown conditions.
		if (this->flattenSize() > i_len) {
			// Generate an error for buffer overflow conditions.
			ETRAC2("Buffer overflow occured in "
                               "ecmdThreadData::flatten() "
                               "structure size = %d; "
                               "input length = %d",
                               this->flattenSize(), i_len);
			rc = ECMD_DATA_OVERFLOW;
			break;
		}

		// Insert magic header in the buffer.
		tmpData32 = htonl(THREAD_HDR_MAGIC);      
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(THREAD_HDR_MAGIC);
		i_len -= sizeof(THREAD_HDR_MAGIC);

		// Copy non-list data.
		memcpy(l_ptr, &threadId, sizeof(threadId));
		l_ptr += sizeof(threadId);
		i_len -= sizeof(threadId);

		memcpy(l_ptr, &unitId, sizeof(unitId));
		l_ptr += sizeof(unitId);
		i_len -= sizeof(unitId);

	} while (0);	// <- single exit.

	return rc;
}

uint32_t ecmdThreadData::unflatten(const uint8_t *i_buf, uint32_t &i_len) {

	uint8_t *l_ptr = (uint8_t *)i_buf;

	uint32_t hdrCheck = 0;
	uint32_t rc       = ECMD_SUCCESS;

	do {	// Single entry ->

		// Check for buffer overflown conditions.
		if (this->flattenSize() != i_len) {
			// Generate an error for buffer overflow conditions.
			ETRAC2("Buffer overflow occured in "
                               "ecmdThreadData::unflatten() "
                               "structure size = %d; "
                               "input length = %d",
                               this->flattenSize(), i_len);
			rc = ECMD_DATA_OVERFLOW;
			break;
		}

		// Get and verify the magic header.
		memcpy(&hdrCheck, l_ptr, sizeof(hdrCheck));
		hdrCheck = ntohl(hdrCheck);
		l_ptr += sizeof(hdrCheck);
		i_len -= sizeof(hdrCheck);

		if (THREAD_HDR_MAGIC != hdrCheck) {
			ETRAC2("Buffer header does not match struct header in "
			       "ecmdThreadData::unflatten(): "
			       "Struct header: 0x%08x; read from buffer as: "
			       "0x%08x", THREAD_HDR_MAGIC, hdrCheck);
			rc = ECMD_INVALID_ARRAY;
			break;
		}

		// Copy non-list data.
		memcpy(&threadId, l_ptr, sizeof(threadId));
		l_ptr += sizeof(threadId);
		i_len -= sizeof(threadId);

		memcpy(&unitId, l_ptr, sizeof(unitId));
		l_ptr += sizeof(unitId);
		i_len -= sizeof(unitId);

	} while (0);	// <- single exit.

	return rc;
}

uint32_t ecmdThreadData::flattenSize() {

	uint32_t flatSize = 0;

	/*
	 * Every struct entry shall place in the buffer a 32bit value to
	 * contain a magic header used to identify itself.  This will 
	 * be used to make sure the code is looking at the expected 
	 * struct.  So...
	 * 
	 * Add the size of magic header.
	 */
	flatSize += sizeof(THREAD_HDR_MAGIC);

	// Size of non-list member data.
	flatSize += sizeof(threadId);
	flatSize += sizeof(unitId);

	return flatSize;
}

#ifndef REMOVE_SIM
void ecmdThreadData::printStruct() {

	printf("\n\t\t\t\t\t\t\tThread Data:\n");
	printf("\t\t\t\t\t\t\tThread ID: %d\n", threadId);
	printf("\t\t\t\t\t\t\tUnit ID: %d\n", unitId);
}
#endif


/*
 * The following methods for the ecmdChipData struct will flatten, unflatten &
 * get the flattened size of the struct.
 */
uint32_t ecmdCoreData::flatten(uint8_t *o_buf, uint32_t &i_len) {

	uint32_t tmpData32 = 0;
	uint32_t listSize  = 0;
	uint32_t rc        = ECMD_SUCCESS ;

	uint8_t *l_ptr = o_buf;

	std::list<ecmdThreadData>::iterator threaditor = threadData.begin();

	do {	// Single entry ->

		// Check for buffer overflow conditions.
		if (this->flattenSize() > i_len) {
			// Generate an error for buffer overflow conditions.
			ETRAC2("Buffer overflow occured in "
                               "ecmdCoreData::flatten() "
                               "structure size = %d; "
                               "input length = %d",
                               this->flattenSize(), i_len);
			rc = ECMD_DATA_OVERFLOW;
			break;
		}

		// Insert magic header in the buffer.
		tmpData32 = htonl(CORE_HDR_MAGIC);      
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(CORE_HDR_MAGIC);
		i_len -= sizeof(CORE_HDR_MAGIC);

		// Copy non-list data.
		memcpy(l_ptr, &coreId, sizeof(coreId));
		l_ptr += sizeof(coreId);
		i_len -= sizeof(coreId);

		memcpy(l_ptr, &numProcThreads, sizeof(numProcThreads));
		l_ptr += sizeof(numProcThreads);
		i_len -= sizeof(numProcThreads);

		memcpy(l_ptr, &unitId, sizeof(unitId));
		l_ptr += sizeof(unitId);
		i_len -= sizeof(unitId);

		/*
		 * Figure out how many threadData structs are in the list for 
		 * future unflattening.
		 */
		listSize = threadData.size();
		tmpData32 = htonl(listSize);
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(listSize);
		i_len -= sizeof(listSize);

		if (0 == listSize) {
			/*
			 * There are no threadData structs in this list. Don't 
			 * bother attempting to loop on cageData list.
			 */
			break;
		}

		// Copy list data.
		while (threaditor != threadData.end()) {
			threaditor->flatten(l_ptr, i_len);
			/*
                         * l_ptr is not passed by reference so now that we 
                         * have it populated increment by the actual size.
                         */
			l_ptr += threaditor->flattenSize();
			threaditor++;
		}

	} while (0);	// <- single exit.

	return rc;
}

uint32_t ecmdCoreData::unflatten(const uint8_t *i_buf, uint32_t &i_len) {

	uint8_t *l_ptr = (uint8_t *)i_buf;

	uint32_t hdrCheck = 0;
	uint32_t listSize = 0;
	uint32_t rc       = ECMD_SUCCESS;

	do {	// Single entry ->

		// Check for buffer overflow conditions.
		if (this->flattenSize() > i_len) {
			// Generate an error for buffer overflow conditions.
			ETRAC2("Buffer overflow occured in "
                               "ecmdCoreData::unflatten() "
                               "structure size = %d; "
                               "input length = %d",
                               this->flattenSize(), i_len);
			rc = ECMD_DATA_OVERFLOW;
			break;
		}

		// Get and verify the magic header.
		memcpy(&hdrCheck, l_ptr, sizeof(hdrCheck));
		hdrCheck = ntohl(hdrCheck);
		l_ptr += sizeof(hdrCheck);
		i_len -= sizeof(hdrCheck);

		if (CORE_HDR_MAGIC != hdrCheck) {
			ETRAC2("Buffer header does not match struct header in "
                               "ecmdCoreData::unflatten(): "
                               "Struct header: 0x%08x; read from buffer as: "
                               "0x%08x", CORE_HDR_MAGIC, hdrCheck);
			rc = ECMD_INVALID_ARRAY;
			break;
		}

		// Unflatten non-list data.
		memcpy(&coreId, l_ptr, sizeof(coreId));
		l_ptr += sizeof(coreId);
		i_len -= sizeof(coreId);

		memcpy(&numProcThreads, l_ptr, sizeof(numProcThreads));
		l_ptr += sizeof(numProcThreads);
		i_len -= sizeof(numProcThreads);

		memcpy(&unitId, l_ptr, sizeof(unitId));
		l_ptr += sizeof(unitId);
		i_len -= sizeof(unitId);

		// Get the number of thredData structs from the buffer.
		memcpy(&listSize, l_ptr, sizeof(listSize));
		listSize = ntohl(listSize);
		l_ptr += sizeof(listSize);
		i_len -= sizeof(listSize);

		// Check to see if the list is populated.
		if (0 == listSize) {
			// Nothing to create, just leave.
			break;
		}

		// Create any list entries.
		for (uint32_t i = 0; i < listSize; i++) {
			threadData.push_back(ecmdThreadData());
		}

		std::list<ecmdThreadData>::iterator threaditor = 
							threadData.begin();
		// Unflatten list data.
		while (threaditor != threadData.end()) {
			threaditor->unflatten(l_ptr, i_len);
			/*
                         * l_ptr is not passed by reference so now that we 
                         * have it populated increment by the actual size.
                         */
			l_ptr += threaditor->flattenSize();
			threaditor++;
		}

	} while (0);	// <- single exit.

	return rc;
}

uint32_t ecmdCoreData::flattenSize() {

	uint32_t flatSize = 0;
	std::list<ecmdThreadData>::iterator threaditor = threadData.begin();

	do {    // Single entry ->

		/*
		 * Every struct entry shall place in the buffer a 32bit value to		 * contain a magic header used to identify itself.  This will 
		 * be used to make sure the code is looking at the expected 
		 * struct.  So...
		 * 
		 * Add the size of magic header.
		 */
		flatSize += sizeof(CORE_HDR_MAGIC);

		// Size of non-list member data.
		flatSize += sizeof(coreId) + sizeof(numProcThreads) + sizeof(unitId);

		/* 
		 * Every struct entry which contains a list of other structs is
		 * required to put into the buffer a 32bit value describing the
		 * number of structures in its list.  So...
		 * 
		 * Add one for the threadData list counter.
		 */
		flatSize += sizeof(uint32_t);

		// If threadData list does not contain any structs break out.
		if (0 == threadData.size()) {
			break;
		}

		// Size of list member data.
		while (threaditor != threadData.end()) {
			flatSize += threaditor->flattenSize();
			threaditor++;
		}
	
	} while (0);    // <- single exit.

	return flatSize;
}

#ifndef REMOVE_SIM
void ecmdCoreData::printStruct() {

	std::list<ecmdThreadData>::iterator threaditor = threadData.begin();

	printf("\n\t\t\t\t\t\tCore Data:\n");

	// Print non-list data.
	printf("\t\t\t\t\t\tCore ID: %d\n", coreId);
	printf("\t\t\t\t\t\tNumber of proc threads: %d\n", numProcThreads);
	printf("\t\t\t\t\t\tUnit ID: %d\n", unitId);

	// Print list data.
	if (threadData.size() == 0) {
		printf("\t\t\t\t\t\tNo thread data.\n");
	}

	while (threaditor != threadData.end()) {
		threaditor->printStruct();
		threaditor++;
	}
}
#endif


/*
 * The following methods for the ecmdChipData struct will flatten, unflatten &
 * get the flattened size of the struct.
 */
uint32_t ecmdChipData::flatten(uint8_t *o_buf, uint32_t &i_len) {

	uint32_t tmpData32 = 0;
	uint32_t listSize  = 0;
	uint32_t rc	   = ECMD_SUCCESS;

	uint8_t *l_ptr = o_buf;

	std::list<ecmdCoreData>::iterator coreitor = coreData.begin();

	do {	// Single entry ->

		// Check for buffer overflow conditions.
		if (this->flattenSize() > i_len) {
			// Generate an error for buffer overflow conditions.
			ETRAC2("Buffer overflow occured in "
                               "ecmdChipData::flatten() "
                               "structure size = %d; "
                               "input length = %d",
                               this->flattenSize(), i_len);
			rc = ECMD_DATA_OVERFLOW;
			break;
		}

		// Insert magic header in the buffer.
		tmpData32 = htonl(CHIP_HDR_MAGIC);
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(CHIP_HDR_MAGIC);
		i_len -= sizeof(CHIP_HDR_MAGIC);

		// Copy non-list data.
		memcpy(l_ptr, chipType.c_str(), chipType.size() + 1);
		l_ptr += chipType.size() + 1;
		i_len -= chipType.size() + 1;
	
		memcpy(l_ptr, 
		       chipShortType.c_str(), 
		       chipShortType.size() + 1);
		l_ptr += chipShortType.size() + 1;
		i_len -= chipShortType.size() + 1;

		memcpy(l_ptr, 
		       chipCommonType.c_str(), 
		       chipCommonType.size() + 1);
		l_ptr += chipCommonType.size() + 1;
		i_len -= chipCommonType.size() + 1;
	
		tmpData32 = htonl(pos);
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(pos);
		i_len -= sizeof(pos);
	
		memcpy(l_ptr, &numProcCores, sizeof(numProcCores));
		l_ptr += sizeof(numProcCores);
		i_len -= sizeof(numProcCores);
	
		tmpData32 = htonl(chipEc);
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(chipEc);
		i_len -= sizeof(chipEc);

		tmpData32 = htonl(simModelEc);
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(simModelEc);
		i_len -= sizeof(simModelEc);
	
		tmpData32 = htonl((uint32_t) interfaceType);
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(interfaceType);
		i_len -= sizeof(interfaceType);

		tmpData32 = htonl(chipFlags);
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(chipFlags);
		i_len -= sizeof(chipFlags);

		/*
		 * Figure out how many coreData structs are in the list for 
		 * future unflattening.
		 */
		listSize = coreData.size();
		tmpData32 = htonl(listSize);
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(listSize);
		i_len -= sizeof(listSize);

		if (0 == listSize) {
			/*
			 * There are no coreData structs in this list. Don't 
			 * bother attempting to loop on cageData list.
			 */
			break;
		}

		// Copy list data.
		while (coreitor != coreData.end()) {
			coreitor->flatten(l_ptr, i_len);
			/*
                         * l_ptr is not passed by reference so now that we 
                         * have it populated increment by the actual size.
                         */
			l_ptr += coreitor->flattenSize();
			coreitor++;
		}

		

	} while (0);	// <- single exit.

	return rc;
}

uint32_t ecmdChipData::unflatten(const uint8_t *i_buf, uint32_t &i_len) {

	uint8_t *l_ptr = (uint8_t *)i_buf;

	uint32_t hdrCheck = 0;
	uint32_t listSize = 0;
	uint32_t rc       = ECMD_SUCCESS;

	do {	// Single entry ->
		/*
		 * Because of the strings in this struct we must check the magic
		 * header early.  So...Get and verify the magic header.
		 */
		memcpy(&hdrCheck, l_ptr, sizeof(hdrCheck));
		hdrCheck = ntohl(hdrCheck);
		l_ptr += sizeof(hdrCheck);
		i_len -= sizeof(hdrCheck);

		if (CHIP_HDR_MAGIC != hdrCheck) {
			ETRAC2("Buffer header does not match struct header in "
                               "ecmdChipData::unflatten(): "
                               "Struct header: 0x%08x; read from buffer as: "
                               "0x%08x", CHIP_HDR_MAGIC, hdrCheck);
			rc = ECMD_INVALID_ARRAY;
			break;
		}

		std::string l_chipType = (const char *) l_ptr;
		l_ptr += l_chipType.size() + 1;

		std::string l_chipShortType = (const char *) l_ptr;
		l_ptr += l_chipShortType.size() + 1;

		std::string l_chipCommonType = (const char *) l_ptr;
		l_ptr += l_chipCommonType.size() + 1;

		uint32_t l_byteCount = sizeof(pos)
				       + sizeof(numProcCores)
				       + sizeof(chipEc)
				       + sizeof(simModelEc)
				       + sizeof(interfaceType)
				       + sizeof(chipFlags)
				       + l_chipType.size() + 1
		                       + l_chipShortType.size() + 1
				       + l_chipCommonType.size() + 1;

		if (l_byteCount > i_len) {
			// Generate an error for buffer overflow conditions.
			ETRAC2("Buffer overflow occured in "
                               "ecmdChipData::unflatten() "
                               "structure size = %d; "
                               "input length = %d",
                               this->flattenSize(), i_len);
			rc = ECMD_DATA_OVERFLOW;
			break;
		}

		// Unflatten non-list data.
		chipType = l_chipType;
		i_len -= l_chipType.size() + 1;

		chipShortType = l_chipShortType;
		i_len -= l_chipShortType.size() + 1;

		chipCommonType = l_chipCommonType;
		i_len -= l_chipCommonType.size() + 1;

		memcpy(&pos, l_ptr, sizeof(pos));
		pos = ntohl(pos);
		l_ptr += sizeof(pos);
		i_len -= sizeof(pos);

		memcpy(&numProcCores, l_ptr, sizeof(numProcCores));
		l_ptr += sizeof(numProcCores);
		i_len -= sizeof(numProcCores);

		memcpy(&chipEc, l_ptr, sizeof(chipEc));
		chipEc = ntohl(chipEc);
		l_ptr += sizeof(chipEc);
		i_len -= sizeof(chipEc);

		memcpy(&simModelEc, l_ptr, sizeof(simModelEc));
		simModelEc = ntohl(simModelEc);
		l_ptr += sizeof(simModelEc);
		i_len -= sizeof(simModelEc);

		memcpy(&interfaceType, l_ptr, sizeof(interfaceType));
		interfaceType = (ecmdChipInterfaceType_t) ntohl(
						      (uint32_t) interfaceType);
		l_ptr += sizeof(interfaceType);
		i_len -= sizeof(interfaceType);

		memcpy(&chipFlags, l_ptr, sizeof(chipFlags));
		chipFlags = ntohl(chipFlags);
		l_ptr += sizeof(chipFlags);
		i_len -= sizeof(chipFlags);

		// Get the number of cageData structs from the buffer.
		memcpy(&listSize, l_ptr, sizeof(listSize));
		listSize = ntohl(listSize);
		l_ptr += sizeof(listSize);
		i_len -= sizeof(listSize);

		// Check to see if the list is populated.
		if (0 == listSize) {
			// Nothing to create, just leave.
			break;
		}

		// Create any list entries.
		for (uint32_t i = 0; i < listSize; i++) {
			coreData.push_back(ecmdCoreData());
		}

		std::list<ecmdCoreData>::iterator coreitor = coreData.begin();
	
		// Unflatten list data.
		while (coreitor != coreData.end()) {
			coreitor->unflatten(l_ptr, i_len);
			/*
                         * l_ptr is not passed by reference so now that we 
                         * have it populated increment by the actual size.
                         */
			l_ptr += coreitor->flattenSize();
			coreitor++;
		}

	} while (0);	// <- single exit.

	return rc;
}

uint32_t ecmdChipData::flattenSize() {
	
	uint32_t flatSize = 0;
	std::list<ecmdCoreData>::iterator coreitor = coreData.begin();

	do {	// Single entry ->

		/*
		 * Every struct entry shall place in the buffer a 32bit value to		 * contain a magic header used to identify itself.  This will 
		 * be used to make sure the code is looking at the expected 
		 * struct.  So...
		 * 
		 * Add the size of magic header.
		 */
		flatSize += sizeof(CHIP_HDR_MAGIC);

		// Size of non-list member data.
		flatSize += (sizeof(pos) 
			     + sizeof(numProcCores) 
			     + sizeof(chipEc) 
			     + sizeof(simModelEc) 
			     + sizeof(interfaceType) 
			     + sizeof(chipFlags) 
			     + chipType.size() + 1
			     + chipShortType.size() + 1
			     + chipCommonType.size() + 1);

		/* 
		 * Every struct entry which contains a list of other structs is
		 * required to put into the buffer a 32bit value describing the
		 * number of structures in its list.  So...
		 * 
		 * Add one for the coreData list counter.
		 */
		flatSize += sizeof(uint32_t);

		// If the coreData list does not contain any structs break out.
		if (0 == coreData.size()) {
			break;
		}

		// Size of list member data.
		while (coreitor != coreData.end()) {
			flatSize += coreitor->flattenSize();
			coreitor++;
		}

	} while (0);	// <- single exit.

	return flatSize;
}

#ifndef REMOVE_SIM
void ecmdChipData::printStruct() {

	std::list<ecmdCoreData>::iterator coreitor = coreData.begin();

	printf("\n\t\t\t\t\tChip Data:\n");

	// Print non-list data.
	printf("\t\t\t\t\tChip type: %s\n", chipType.c_str());
	printf("\t\t\t\t\tChip short type: %s\n", chipShortType.c_str());
	printf("\t\t\t\t\tChip common type: %s\n", chipCommonType.c_str());
	printf("\t\t\t\t\tPosition: %d\n", pos);
	printf("\t\t\t\t\tNumber of proc cores: %d\n", numProcCores);
	printf("\t\t\t\t\tChip EC: %d\n", chipEc);
	printf("\t\t\t\t\tSim mode EC: %d\n", simModelEc);
	printf("\t\t\t\t\tInterface: 0x%08x\n", (uint32_t) interfaceType);
	printf("\t\t\t\t\tChip flags: 0x%08x\n", chipFlags);

	// Print list data.
	if (coreData.size() == 0) {
		printf("\t\t\t\t\tNo core data.\n");
	}
	while (coreitor != coreData.end()) {
		coreitor->printStruct();
		coreitor++;
	}
}
#endif


/*
 * The following methods for the ecmdSlotData struct will flatten, unflatten &
 * get the flattened size of the struct.
 */
uint32_t ecmdSlotData::flatten(uint8_t *o_buf, uint32_t &i_len) {

	uint32_t tmpData32 = 0;
	uint32_t listSize  = 0;
	uint32_t rc	= ECMD_SUCCESS;

	uint8_t *l_ptr = o_buf;

	std::list<ecmdChipData>::iterator chipitor = chipData.begin();

	do {	// Single entry -> 

		// Check for buffer overflow conditions.
		if (this->flattenSize() > i_len) {
			// Generate an error for buffer overflow conditions.
			ETRAC2("Buffer overflow occured in "
                               "ecmdSlotData::flatten() "
                               "structure size = %d; "
                               "input length = %d",
                               this->flattenSize(), i_len);
			rc = ECMD_DATA_OVERFLOW;
			break;
		}

		// Insert magic header in the buffer.
		tmpData32 = htonl(SLOT_HDR_MAGIC);		
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(SLOT_HDR_MAGIC);
		i_len -= sizeof(SLOT_HDR_MAGIC);

		// Copy non-list data.
		tmpData32 = htonl((uint32_t) slotId);
		memcpy(l_ptr, &tmpData32, sizeof(slotId));
		l_ptr += sizeof(slotId);
		i_len -= sizeof(slotId);

		tmpData32 = htonl((uint32_t) unitId);
		memcpy(l_ptr, &tmpData32, sizeof(unitId));
		l_ptr += sizeof(unitId);
		i_len -= sizeof(unitId);

		/* 
		 * Figure out how many chipData structs are in the list for 
		 * future unflattening.
		 */
		listSize = chipData.size();
		tmpData32 = htonl(listSize);
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(listSize);
		i_len -= sizeof(listSize);

		if (0 == listSize) {
			/*
			 * There are no chipData structs in this list. Don't 
			 * bother attempting to loop on cageData list.
			 */
			break;
		}

		// Copy list data.
		while (chipitor != chipData.end()) {
			chipitor->flatten(l_ptr, i_len);
			l_ptr += chipitor->flattenSize();
			chipitor++;
		}

	} while (0);	// <- single exit.

	return rc;
}

uint32_t ecmdSlotData::unflatten(const uint8_t *i_buf, uint32_t &i_len) {

	uint8_t *l_ptr = (uint8_t *) i_buf;

	uint32_t hdrCheck = 0;
	uint32_t listSize = 0;
	uint32_t rc       = ECMD_SUCCESS;

	do {	// Single entry ->

		// Check for buffer overflow conditions.		
		if (this->flattenSize() > i_len) {
			// Generate an error for buffer overflow conditions.
			ETRAC2("Buffer overflow occured in "
                               "ecmdSlotData::unflatten() "
                               "structure size = %d; "
                               "input length = %d",
                               this->flattenSize(), i_len);
			rc = ECMD_DATA_OVERFLOW;
			break;
		}

		// Get and verify the magic header.
		memcpy(&hdrCheck, l_ptr, sizeof(hdrCheck));
		hdrCheck = ntohl(hdrCheck);
		l_ptr += sizeof(hdrCheck);
		i_len -= sizeof(hdrCheck);

		if (SLOT_HDR_MAGIC != hdrCheck) {
			ETRAC2("Buffer header does not match struct header in "
                               "ecmdSlotData::unflatten(): "
                               "Struct header: 0x%08x; read from buffer as: "
                               "0x%08x", SLOT_HDR_MAGIC, hdrCheck);
			rc = ECMD_INVALID_ARRAY;
			break;
		}

		// Unflatten non-list data.
		memcpy(&slotId, l_ptr, sizeof(slotId));
		slotId = ntohl(slotId);
		l_ptr += sizeof(slotId);
		i_len -= sizeof(slotId);

		memcpy(&slotId, l_ptr, sizeof(unitId));
		slotId = ntohl(unitId);
		l_ptr += sizeof(unitId);
		i_len -= sizeof(unitId);

		// Get the number of cageData structs from the buffer.
		memcpy(&listSize, l_ptr, sizeof(listSize));
		listSize = ntohl(listSize);
		l_ptr += sizeof(listSize);
		i_len -= sizeof(listSize);

		// Check to see if the list is populated.
		if (0 == listSize) {
			// Nothing to create, just leave.
			break;
		}

		// Create any list entries.
		for (uint32_t i = 0; i < listSize; i++) {
			chipData.push_back(ecmdChipData());
		}

		std::list<ecmdChipData>::iterator chipitor = chipData.begin();

		// Unflatten list data.
		while (chipitor != chipData.end()) {
			chipitor->unflatten(l_ptr, i_len);
			/*
			 * l_ptr is not passed by reference so now that we 
			 * have it populated increment by the actual size.
			 */
			l_ptr += chipitor->flattenSize();
			chipitor++;
		}
	
	} while (0);	// <- single exit.

	return rc;
}

uint32_t ecmdSlotData::flattenSize() {

	uint32_t flatSize = 0;
	std::list<ecmdChipData>::iterator chipitor = chipData.begin();

	do {	// Single entry ->

		/*
		 * Every struct entry shall place in the buffer a 32bit value to		 * contain a magic header used to identify itself.  This will 
		 * be used to make sure the code is looking at the expected 
		 * struct.  So...
		 * 
		 * Add the size of magic header.
		 */
		flatSize += sizeof(SLOT_HDR_MAGIC);

		// Size of non-list member data.
		flatSize += sizeof(slotId) + sizeof(unitId);

		/* 
		 * Every struct entry which contains a list of other structs is
		 * required to put into the buffer a 32bit value describing the
		 * number of structures in its list.  So...
		 * 
		 * Add one for the chipData list counter.
		 */
		flatSize += sizeof(uint32_t);

		// If the chipData list does not contain any structs break out.
		if (0 == chipData.size()) {
			break;
		}

		// Size of list member data.
		while (chipitor != chipData.end()) {
			flatSize += chipitor->flattenSize();
			chipitor++;
		}

	} while (0);	// <- single exit.

	return flatSize;
}

#ifndef REMOVE_SIM
void ecmdSlotData::printStruct() {

	std::list<ecmdChipData>::iterator chipitor = chipData.begin();

	printf("\n\t\t\t\tSlot Data:\n");

	// Print non-list data.
	printf("\t\t\t\tSlot ID: 0x%08x\n", slotId);
	printf("\t\t\t\tUnit ID: 0x%08x\n", unitId);

	// Print list data.
	if (chipData.size() == 0) {
		printf("\t\t\t\tNo chip data.\n");
	}

	while (chipitor != chipData.end()) {
		chipitor->printStruct();
		chipitor++;
	}
}
#endif


/*
 * The following methods for the ecmdNodeData struct will flatten, unflatten &
 * get the flattened size of the struct.
 */
uint32_t ecmdNodeData::flatten(uint8_t *o_buf, uint32_t &i_len) {

	uint32_t tmpData32 = 0;
	uint32_t listSize  = 0;
	uint32_t rc	= ECMD_SUCCESS;

	uint8_t *l_ptr = o_buf;

	std::list<ecmdSlotData>::iterator slotitor = slotData.begin();

	do {	// Single entry ->

		// Check for buffer overflow conditions.
		if (this->flattenSize() > i_len) {
			// Generate an error for buffer overflow conditions.
			ETRAC2("Buffer overflow occured in "
                               "ecmdNodeData::flatten() "
                               "structure size = %d; "
                               "input length = %d",
                               this->flattenSize(), i_len);
			rc = ECMD_DATA_OVERFLOW;
			break;
		}

		// Insert magic header in the buffer.
		tmpData32 = htonl(NODE_HDR_MAGIC);      
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(NODE_HDR_MAGIC);
		i_len -= sizeof(NODE_HDR_MAGIC);

		// Copy non-list data.
		tmpData32 = htonl((uint32_t) nodeId);
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(nodeId);
		i_len -= sizeof(nodeId);

		tmpData32 = htonl((uint32_t) unitId);
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(unitId);
		i_len -= sizeof(unitId);

		/*
		 * Figure out how many slotData structs are in the list for 
		 * future unflattening.
		 */
		listSize = slotData.size();
		tmpData32 = htonl(listSize);
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(listSize);
		i_len -= sizeof(listSize);

		if (0 == listSize) {
			/*
			 * There are no slotData structs in this list. Don't 
			 * bother attempting to loop on cageData list.
			 */
			break;
		}

		// Copy list data.
		while (slotitor != slotData.end()) {
			slotitor->flatten(l_ptr, i_len);
			/*
                         * l_ptr is not passed by reference so now that we 
                         * have it populated increment by the actual size.
                         */
			l_ptr += slotitor->flattenSize();
			slotitor++;
		}

	} while (0);	// <- single exit.

	return rc;
}

uint32_t ecmdNodeData::unflatten(const uint8_t *i_buf, uint32_t &i_len) {

	uint8_t *l_ptr = (uint8_t *) i_buf;

	uint32_t hdrCheck = 0;
	uint32_t listSize = 0;
	uint32_t rc       = ECMD_SUCCESS;

	do {	// Single entry ->

		// Check for buffer overflow conditions.
		if (this->flattenSize() > i_len) {
			// Generate an error for buffer overflow conditions.
			ETRAC2("Buffer overflow occured in "
                               "ecmdNodeData::unflatten() "
                               "structure size = %d; "
                               "input length = %d",
                               this->flattenSize(), i_len);
			rc = ECMD_DATA_OVERFLOW;
			break;
		}

		// Get and verify the magic header.
		memcpy(&hdrCheck, l_ptr, sizeof(hdrCheck));
		hdrCheck = ntohl(hdrCheck);
		l_ptr += sizeof(hdrCheck);
		i_len -= sizeof(hdrCheck);

		if (NODE_HDR_MAGIC != hdrCheck) {
			ETRAC2("Buffer header does not match struct header in "
                               "ecmdNodeData::unflatten(): "
                               "Struct header: 0x%08x; read from buffer as: "
                               "0x%08x", NODE_HDR_MAGIC, hdrCheck);
			rc = ECMD_INVALID_ARRAY;
			break;
		}

		// Unflatten non-list data.
		memcpy(&nodeId, l_ptr, sizeof(nodeId));
		nodeId = ntohl(nodeId);
		l_ptr += sizeof(nodeId);
		i_len -= sizeof(nodeId);

		memcpy(&nodeId, l_ptr, sizeof(unitId));
		nodeId = ntohl(unitId);
		l_ptr += sizeof(unitId);
		i_len -= sizeof(unitId);

		// Get the number of cageData structs from the buffer.
		memcpy(&listSize, l_ptr, sizeof(listSize));
		listSize = ntohl(listSize);
		l_ptr += sizeof(listSize);
		i_len -= sizeof(listSize);

		// Check to see if the list is populated.
		if (0 == listSize) {
			// Nothing to create, just leave.
			break;
		}

		// Create any list entries.
		for (uint32_t i = 0; i < listSize; i++) {
			slotData.push_back(ecmdSlotData());
		}

		std::list<ecmdSlotData>::iterator slotitor = slotData.begin();
	
		// Unflatten list data.
		while (slotitor != slotData.end()) {
			slotitor->unflatten(l_ptr, i_len);
			/*
                         * l_ptr is not passed by reference so now that we 
                         * have it populated increment by the actual size.
                         */
			l_ptr += slotitor->flattenSize();
			slotitor++;
		}

	} while (0);	// <- single exit.

	return rc;	
}

uint32_t ecmdNodeData::flattenSize()  {

	uint32_t flatSize = 0;
	std::list<ecmdSlotData>::iterator slotitor = slotData.begin();

	do {    // Single entry ->

		/*
		 * Every struct entry shall place in the buffer a 32bit value to		 * contain a magic header used to identify itself.  This will 
		 * be used to make sure the code is looking at the expected 
		 * struct.  So...
		 * 
		 * Add the size of magic header.
		 */
		flatSize += sizeof(NODE_HDR_MAGIC);

		// Size of non-list member data.
		flatSize += sizeof(nodeId) + sizeof(unitId);

		/* 
		 * Every struct entry which contains a list of other structs is
		 * required to put into the buffer a 32bit value describing the
		 * number of structures in its list.  So...
		 * 
		 * Add one for the slotData list counter.
		 */
		flatSize += sizeof(uint32_t);

		// If the slotData list does not contain any structs break out.
		if (0 == slotData.size()) {
			break;
		}

		// Size of list member data.
		while (slotitor != slotData.end()) {
			flatSize += slotitor->flattenSize();
			slotitor++;
		}

	} while (0);    // <- single exit.

	return flatSize;
}

#ifndef REMOVE_SIM
void ecmdNodeData::printStruct() {

	std::list<ecmdSlotData>::iterator slotitor = slotData.begin();

	printf("\n\t\t\tNode Data:\n");

	// Print non-list data.
	printf("\t\t\tNode ID: 0x%08x\n", nodeId);
	printf("\t\t\tUnit ID: 0x%08x\n", unitId);

	// Print list data.
	if (slotData.size() == 0) {
		printf("\t\t\tNo slot data.\n");
	}

	while (slotitor != slotData.end()) {
		slotitor->printStruct();
		slotitor++;
	}
}
#endif


/*
 * The following methods for the ecmdCageData struct will flatten, unflatten &
 * get the flattened size of the struct.
 */
uint32_t ecmdCageData::flatten(uint8_t *o_buf, uint32_t &i_len) {

	uint32_t tmpData32 = 0;
	uint32_t listSize  = 0;
	uint32_t rc	= ECMD_SUCCESS;

	uint8_t *l_ptr = o_buf;

	std::list<ecmdNodeData>::iterator nodeitor = nodeData.begin();

	do {    // Single entry ->

		if (this->flattenSize() > i_len) {
			// Generate an error for buffer overflow conditions.
			ETRAC2("Buffer overflow occured in "
                               "ecmdCageData::flatten() "
                               "structure size = %d; "
                               "input length = %d",
                               this->flattenSize(), i_len);
			rc = ECMD_DATA_OVERFLOW;
			break;
		}

		// Insert magic header in the buffer.
		tmpData32 = htonl(CAGE_HDR_MAGIC);
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(CAGE_HDR_MAGIC);
		i_len -= sizeof(CAGE_HDR_MAGIC);

		// Copy non-list data.
		tmpData32 = htonl((uint32_t) cageId);
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(cageId);
		i_len -= sizeof(cageId);

		tmpData32 = htonl((uint32_t) unitId);
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(unitId);
		i_len -= sizeof(unitId);

		/*
		 * Figure out how many nodeData structs are in the list for 
		 * future unflattening.
		 */
		listSize = nodeData.size();
		tmpData32 = htonl(listSize);
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(listSize);
		i_len -= sizeof(listSize);

		if (0 == listSize) {
			/*
			 * There are no nodeData structs in this list. Don't 
			 * bother attempting to loop on cageData list.
			 */
			break;
		}

		// Copy list data.
		while (nodeitor != nodeData.end()) {
			nodeitor->flatten(l_ptr, i_len);
			/*
                         * l_ptr is not passed by reference so now that we 
                         * have it populated increment by the actual size.
                         */
			l_ptr += nodeitor->flattenSize();
			nodeitor++;
		}

	} while (0);    // <- single exit.

	return rc;
}

uint32_t ecmdCageData::unflatten(const uint8_t *i_buf, uint32_t &i_len) {

	uint8_t *l_ptr = (uint8_t *) i_buf;

	uint32_t hdrCheck = 0;
	uint32_t listSize = 0;
	uint32_t rc       = ECMD_SUCCESS;

	do {    // Single entry ->

		// Check for buffer overflow conditions.
		if (this->flattenSize() > i_len) {
			// Generate an error for buffer overflow conditions.
			ETRAC2("Buffer overflow occured in "
                               "ecmdCageData::unflatten() "
                               "structure size = %d; "
                               "input length = %d",
                               this->flattenSize(), i_len);
			rc = ECMD_DATA_OVERFLOW;
			break;
		}

		// Get and verify the magic header.
		memcpy(&hdrCheck, l_ptr, sizeof(hdrCheck));
		hdrCheck = ntohl(hdrCheck);
		l_ptr += sizeof(hdrCheck);
		i_len -= sizeof(hdrCheck);

		if (CAGE_HDR_MAGIC != hdrCheck) {
			ETRAC2("Buffer header does not match struct header in "
                               "ecmdCageData::unflatten(): "
                               "Struct header: 0x%08x; read from buffer as: "
                               "0x%08x", CAGE_HDR_MAGIC, hdrCheck);
			rc = ECMD_INVALID_ARRAY;
			break;
		}

		// Unflatten non-list data.
		memcpy(&cageId, l_ptr, sizeof(cageId));
		cageId = ntohl(cageId);
		l_ptr += sizeof(cageId);
		i_len -= sizeof(cageId);

		memcpy(&cageId, l_ptr, sizeof(unitId));
		cageId = ntohl(unitId);
		l_ptr += sizeof(unitId);
		i_len -= sizeof(unitId);

		// Get the number of nodeData structs from the buffer.
		memcpy(&listSize, l_ptr, sizeof(listSize));
		listSize = ntohl(listSize);
		l_ptr += sizeof(listSize);
		i_len -= sizeof(listSize);

		// Check to see if the list is populated.
		if (0 == listSize) {
			// Nothing to create, just leave.
			break;
		}

		// Create any list entries.
		for (uint32_t i = 0; i < listSize; i++) {
			nodeData.push_back(ecmdNodeData());
		}

		std::list<ecmdNodeData>::iterator nodeitor = nodeData.begin();

		// Unflatten list data.
		while (nodeitor != nodeData.end()) {
			nodeitor->unflatten(l_ptr, i_len);
			/*
                         * l_ptr is not passed by reference so now that we 
                         * have it populated increment by the actual size.
                         */
			l_ptr += nodeitor->flattenSize();
			nodeitor++;
		}

	} while (0);    // <- single exit.

	return rc;
}

uint32_t ecmdCageData::flattenSize() {

	uint32_t flatSize = 0;
	std::list<ecmdNodeData>::iterator nodeitor = nodeData.begin();

	do {    // Single entry ->

		/*
		 * Every struct entry shall place in the buffer a 32bit value to		 * contain a magic header used to identify itself.  This will 
		 * be used to make sure the code is looking at the expected 
		 * struct.  So...
		 * 
		 * Add the size of magic header.
		 */
		flatSize += sizeof(CAGE_HDR_MAGIC);

		// Size of non-list member data.
		flatSize += sizeof(cageId) + sizeof(unitId);

		/* 
		 * Every struct entry which contains a list of other structs is
		 * required to put into the buffer a 32bit value describing the
		 * number of structures in its list.  So...
		 * 
		 * Add one for the nodeData list counter.
		 */
		flatSize += sizeof(uint32_t);

		// If the nodeData list does not contain any structs break out.
		if (0 == nodeData.size()) {
			break;
		}

		// Size of list member data.
		while (nodeitor != nodeData.end()) {
			flatSize += nodeitor->flattenSize();
			nodeitor++;
		}

	} while (0);    // <- single exit.

	return flatSize;
}

#ifndef REMOVE_SIM
void ecmdCageData::printStruct() {

	std::list<ecmdNodeData>::iterator nodeitor = nodeData.begin();

	printf("\n\t\tCage Data:\n");

	// Print non-list data.
	printf("\t\tCage ID: 0x%08x\n", cageId);
	printf("\t\tUnit ID: 0x%08x\n", unitId);

	// Print list data.
	if (nodeData.size() == 0) {
		printf("\t\tNo node data.\n");
	}

	while (nodeitor != nodeData.end()) {
		nodeitor->printStruct();	
		nodeitor++;
	}
}
#endif


/*
 * The following methods for the ecmdQueryData struct will flatten, unflatten &
 * get the flattened size of the struct.
 */
uint32_t ecmdQueryData::flatten(uint8_t *o_buf, uint32_t &i_len) {

	uint32_t tmpData32 = 0;
	uint32_t listSize  = 0;
	uint32_t rc	= ECMD_SUCCESS;

	uint8_t *l_ptr = o_buf;

	std::list<ecmdCageData>::iterator cageitor = cageData.begin();

	do {    // Single entry ->

		if (this->flattenSize() > i_len) {
			// Generate an error for buffer overflow conditions.
			ETRAC2("Buffer overflow occured in "
                               "ecmdQueryData::flatten() "
                               "structure size = %d; "
                               "input length = %d",
                               this->flattenSize(), i_len);
			rc = ECMD_DATA_OVERFLOW;
			break;
		}

		// Insert magic header in the buffer.
		tmpData32 = htonl(QD_HDR_MAGIC);		
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(QD_HDR_MAGIC);
		i_len -= sizeof(QD_HDR_MAGIC);

		// Copy non-list data.
		tmpData32 = htonl((uint32_t)detailLevel);
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(detailLevel);
		i_len -= sizeof(detailLevel);

		/*
		 * Figure out how many cageData structs are in the list for 
		 * future unflattening.
		 */
		listSize = cageData.size();
		tmpData32 = htonl(listSize);
		memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
		l_ptr += sizeof(listSize);
		i_len -= sizeof(listSize);

		if (0 == listSize) {
			/*
			 * There are no cageData structs in this list. Don't 
			 * bother attempting to loop on cageData list.
			 */
			break;
		}

		// Copy list data.
		while (cageitor != cageData.end()) {
			cageitor->flatten(l_ptr, i_len);
			/*
                         * l_ptr is not passed by reference so now that we 
                         * have it populated increment by the actual size.
                         */
			l_ptr += cageitor->flattenSize();
			cageitor++;
		}

	} while (0);    // <- single exit.

	return rc;
}

uint32_t ecmdQueryData::unflatten(const uint8_t *i_buf, uint32_t &i_len) {

	uint8_t *l_ptr = (uint8_t *) i_buf;

	uint32_t hdrCheck = 0;
	uint32_t listSize = 0;
	uint32_t rc       = ECMD_SUCCESS;

	do {    // Single entry ->

		if (this->flattenSize() > i_len) {
			// Generate an error for buffer overflow conditions.
			ETRAC2("Buffer overflow occured in "
                               "ecmdQueryData::unflatten() "
                               "structure size = %d; "
                               "input length = %d",
                               this->flattenSize(), i_len);
			rc = ECMD_DATA_OVERFLOW;
			break;
		}

		// Get and verify the magic header.
		memcpy(&hdrCheck, l_ptr, sizeof(hdrCheck));
		hdrCheck = ntohl(hdrCheck);
		l_ptr += sizeof(hdrCheck);
		i_len -= sizeof(hdrCheck);

		if (QD_HDR_MAGIC != hdrCheck) {
			ETRAC2("Buffer header does not match struct header in "
                               "ecmdQueryDataData::unflatten(): "
                               "Struct header: 0x%08x; read from buffer as: "
                               "0x%08x", QD_HDR_MAGIC, hdrCheck);
			rc = ECMD_INVALID_ARRAY;
			break;
		}

		// Unflatten non-list data.
		memcpy(&detailLevel, l_ptr, sizeof(detailLevel));
		detailLevel = (ecmdQueryDetail_t) ntohl((uint32_t) detailLevel);
		l_ptr += sizeof(detailLevel);
		i_len -= sizeof(detailLevel);

		// Get the number of cageData structs from the buffer.
		memcpy(&listSize, l_ptr, sizeof(listSize));
		listSize = ntohl(listSize);
		l_ptr += sizeof(listSize);
		i_len -= sizeof(listSize);

		// Check to see if the list is populated.
		if (0 == listSize) {
			// Nothing to create, just leave.
			break;
		}

		// Create any list entries.
		for (uint32_t i = 0; i < listSize; i++) {
			cageData.push_back(ecmdCageData());
		}

		std::list<ecmdCageData>::iterator cageitor = cageData.begin();

		// Unflatten list data.
		while (cageitor != cageData.end()) {
			cageitor->unflatten(l_ptr, i_len);
			/*
                         * l_ptr is not passed by reference so now that we 
                         * have it populated increment by the actual size.
                         */
			l_ptr += cageitor->flattenSize();
			cageitor++;
		}

	} while (0);    // <- single exit.

	return rc;
}

uint32_t ecmdQueryData::flattenSize() {

	uint32_t flatSize = 0;
	std::list<ecmdCageData>::iterator cageitor = cageData.begin();

	do {    // Single entry ->

		/*
		 * Every struct entry shall place in the buffer a 32bit value to		 * contain a magic header used to identify itself.  This will 
		 * be used to make sure the code is looking at the expected 
		 * struct.  So...
		 * 
		 * Add the size of magic header.
		 */
		flatSize += sizeof(QD_HDR_MAGIC);

		// Size of non-list member data.
		flatSize += sizeof(detailLevel);

		/* 
		 * Every struct entry which contains a list of other structs is
		 * required to put into the buffer a 32bit value describing the
		 * number of structures in its list.  So...
		 * 
		 * Add one for the cageData list counter.
		 */
		flatSize += sizeof(uint32_t);

		// If the cageData list does not contain any structs break out.
		if (0 == cageData.size()) {
			break;
		}

		// Size of list member data.
		while (cageitor != cageData.end()) {
			flatSize += cageitor->flattenSize();
			cageitor++;
		}

	} while (0);    // <- single exit.

	return flatSize;
}

#ifndef REMOVE_SIM
void  ecmdQueryData::printStruct() {

	std::list<ecmdCageData>::iterator cageitor = cageData.begin();

	printf("\n\tQuery Data:\n");

	// Print non-list data.
	printf("\tDetail level: 0x%08x\n", (uint32_t) detailLevel);

	// Print list data.
	if (cageData.size() == 0) {
		printf("\tNo cage data.\n");
	}

	while (cageitor != cageData.end()) {
		cageitor->printStruct();
		cageitor++;
	}
}
#endif




/*
 * The following methods for the ecmdSpyyData struct will flatten, unflatten &
 * get the flattened size of the struct.
 */
uint32_t ecmdSpyData::flatten(uint8_t *o_buf, uint32_t &i_len) {

        uint32_t tmpData32 = 0;
        uint32_t enumsListSize  = 0;
        uint32_t epCheckersListSize  = 0;
        uint32_t rc     = ECMD_SUCCESS;

        uint8_t *l_ptr = o_buf;

        std::list<std::string>:: iterator listStringIter;


        do {    // Single entry ->

	    if (this->flattenSize() > i_len) {
		// Generate an error for buffer overflow conditions.
		ETRAC2("Buffer overflow occured in "
		       "ecmdSpyData::flatten() "
		       "structure size = %d; "
		       "input length = %d",
		       this->flattenSize(), i_len);
		rc = ECMD_DATA_OVERFLOW;
		break;
	    }

	    // Copy non-list data.
	    // spyName  - first copy in str Length
	    tmpData32 = htonl((uint32_t)(spyName.size()));
	    memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
	    l_ptr += sizeof(spyType);
	    i_len -= sizeof(spyType);

	    if (spyName.size() != 0)  // if there is a spyName, copy it in
	    {
		memcpy(l_ptr, spyName.c_str(), spyName.size() + 1);
		l_ptr += spyName.size() + 1;
		i_len -= spyName.size() + 1;
	    }

	    // bitLength
	    tmpData32 = htonl((uint32_t)bitLength);
	    memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
	    l_ptr += sizeof(bitLength);
	    i_len -= sizeof(bitLength);

	    // spyType
	    tmpData32 = htonl((uint32_t)spyType);
	    memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
	    l_ptr += sizeof(spyType);
	    i_len -= sizeof(spyType);

	    // isEccChecked
	    memcpy(l_ptr, &isEccChecked, sizeof(isEccChecked)); 
	    l_ptr += sizeof(isEccChecked);
	    i_len -= sizeof(isEccChecked);

	    // isEnumerated
	    memcpy(l_ptr, &isEnumerated, sizeof(isEnumerated));
	    l_ptr += sizeof(isEnumerated);
	    i_len -= sizeof(isEnumerated);

	    // isCoreRelated
	    memcpy(l_ptr, &isCoreRelated, sizeof(isCoreRelated)); 
	    l_ptr += sizeof(isCoreRelated);
	    i_len -= sizeof(isCoreRelated);

	    // clockDomain
	    memcpy(l_ptr, clockDomain.c_str(), clockDomain.size() + 1);
	    l_ptr += clockDomain.size() + 1;
	    i_len -= clockDomain.size() + 1;

	    // clockState
	    tmpData32 = htonl((uint32_t)clockState);
	    memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
	    l_ptr += sizeof(clockState);
	    i_len -= sizeof(clockState);


	    /*
	     * Figure out how many enum strings are in the list for
	     * future unflattening.
	     */
	    enumsListSize = enums.size();

	    // add enumsListSize
	    tmpData32 = htonl(enumsListSize);
	    memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
	    l_ptr += sizeof(enumsListSize);
	    i_len -= sizeof(enumsListSize);

	    if (enumsListSize != 0)   // don't bother grabbing list of enums if there aren't any
	    {
		// Copy enum List of strings
		for (listStringIter = enums.begin(); listStringIter != enums.end(); ++listStringIter)
		{
		    memcpy(l_ptr, (*listStringIter).c_str(), (*listStringIter).size() + 1);
		    l_ptr += (*listStringIter).size() + 1;
		    i_len -= (*listStringIter).size() + 1;
		}

	    }

	    /*
	     * Figure out how many epChecker strings are in the list for
	     * future unflattening.
	     */
	    epCheckersListSize = epCheckers.size();

	    // add epCheckersListSize
	    tmpData32 = htonl(epCheckersListSize);
	    memcpy(l_ptr, &tmpData32, sizeof(tmpData32));
	    l_ptr += sizeof(epCheckersListSize);
	    i_len -= sizeof(epCheckersListSize);

	    if (0 != epCheckersListSize) // don't bother grabbing list of enums if there aren't any
	    {
		// Copy epCheckers List of strings
		for (listStringIter = epCheckers.begin(); listStringIter != epCheckers.end(); ++listStringIter)
		{
		    memcpy(l_ptr, (*listStringIter).c_str(), (*listStringIter).size() + 1);
		    l_ptr += (*listStringIter).size() + 1;
		    i_len -= (*listStringIter).size() + 1;
		}
	    }

	    // Do final check
	    if (i_len != 0)
	    {
		ETRAC2("Buffer size mismacth occured in "
		       "ecmdSpyData::flatten() "
		       "structure size = %d; "
		       "leftover length = %d",
		       this->flattenSize(), i_len);
		rc = ECMD_DATA_OVERFLOW;
		break;
	    }

        } while (0);    // <- single exit.

        return rc;
}



uint32_t ecmdSpyData::unflatten(const uint8_t *i_buf, uint32_t &i_len) {

        uint8_t *l_ptr = (uint8_t *) i_buf;
        uint32_t rc       = ECMD_SUCCESS;

        uint32_t enumsListSize  = 0;
        uint32_t epCheckersListSize  = 0;
	uint32_t loop = 0;
	int l_left = (int) i_len;
	uint32_t l_spyName_size = 0;


        do {    // Single entry ->

	    // Unflatten non-list data.
	    // spyName - get spyName Size first
	    memcpy(&l_spyName_size, l_ptr, sizeof(l_spyName_size));
	    l_spyName_size = ntohl(l_spyName_size);
	    l_ptr += sizeof(l_spyName_size);
	    l_left -= sizeof(l_spyName_size);
		
	    // if there is a l_spyName_size != 0, then set spyName
	    if (l_spyName_size != 0)
	    {
		spyName = (const char *) l_ptr;
		l_ptr += l_spyName_size + 1;
		l_left -= l_spyName_size + 1;
	    }

	    // bitLength
	    memcpy(&bitLength, l_ptr, sizeof(bitLength));
	    bitLength = ntohl(bitLength);
	    l_ptr += sizeof(bitLength);
	    l_left -= sizeof(bitLength);

	    // spyType
	    memcpy(&spyType, l_ptr, sizeof(spyType));
	    spyType = (ecmdSpyType_t) ntohl((uint32_t)spyType);
	    l_ptr += sizeof(spyType);
	    l_left -= sizeof(spyType);

	    // isEccChecked
	    memcpy(&isEccChecked, l_ptr, sizeof(isEccChecked));
	    l_ptr += sizeof(isEccChecked);
	    l_left -= sizeof(isEccChecked);

	    // isEnumerated
	    memcpy(&isEnumerated, l_ptr, sizeof(isEnumerated));
	    l_ptr += sizeof(isEnumerated);
	    l_left -= sizeof(isEnumerated);

	    // isCoreRelated
	    memcpy(&isCoreRelated, l_ptr, sizeof(isCoreRelated));
	    l_ptr += sizeof(isCoreRelated);
	    l_left -= sizeof(isCoreRelated);

	    // clockDomain
	    std::string l_clockDomain = (const char *) l_ptr;
	    clockDomain = l_clockDomain;
	    l_ptr += l_clockDomain.size() + 1;
	    l_left -= l_clockDomain.size() + 1;

	    // clockState
	    memcpy(&clockState, l_ptr, sizeof(clockState));
	    clockState = (ecmdClockState_t) ntohl((uint32_t)clockState);
	    l_ptr += sizeof(clockState);
	    l_left -= sizeof(clockState);

	    // Figure out how many enum strings are in the list and then unflatten
	    memcpy(&enumsListSize, l_ptr, sizeof(enumsListSize));
	    enumsListSize = ntohl(enumsListSize);
	    l_ptr += sizeof(enumsListSize);
	    l_left -= sizeof(enumsListSize);

	    // create list of string of enums (nothing happens if enumsListSize=0)
	    for (loop = 0; loop < enumsListSize; loop++)
	    {
		std::string l_str_enum = (char *)l_ptr; 
		enums.push_back(l_str_enum);
		l_ptr += (strlen((char *)l_ptr) + 1);
		l_left -= (strlen((char *)l_ptr) +1);
	    }

            // Figure out how many epChecker strings are in the list and then unflatten
	    memcpy(&epCheckersListSize, l_ptr, sizeof(epCheckersListSize));
	    epCheckersListSize = ntohl(epCheckersListSize);
	    l_ptr += sizeof(epCheckersListSize);
	    l_left -= sizeof(epCheckersListSize);

	    // create list of string of enums (nothing happens if enumsListSize=0)
	    for (loop = 0; loop < epCheckersListSize; loop++)
	    {
		std::string l_str_epCheck = (char *)l_ptr; 
		epCheckers.push_back(l_str_epCheck);
		l_ptr += (strlen((char *)l_ptr) + 1);
		l_left -= (strlen((char *)l_ptr) +1);
	    }

	    if (l_left <= 0)
	    {	
		// Generate an error for buffer overflow conditions.
		ETRAC3("Buffer overflow occured in "
		       "ecmdSpyData::unflatten() "
		       "structure size = %d; "
		       "input length = %x; "
		       "remainder = %d\n",
		       this->flattenSize(), i_len, l_left);
		rc = ECMD_DATA_OVERFLOW;
		break;
	    }
	    if (l_left >= 0)
	    {	
		// Generate an error for buffer underflow conditions.
		ETRAC3("Buffer underflow occured in "
		       "ecmdSpyData::unflatten() "
		       "structure size = %d; "
		       "input length = %x; "
		       "remainder = %d\n",
		       this->flattenSize(), i_len, l_left);
		rc = ECMD_DATA_UNDERFLOW;
		break;
	    }

        } while (0);    // <- single exit.

        return rc;
}


uint32_t ecmdSpyData::flattenSize() {

        uint32_t flatSize = 0;
        uint32_t enumsListSize  = 0;
        uint32_t epCheckersListSize  = 0;
        std::list<std::string>:: iterator listStringIter;


        do {    // Single entry ->


                // Size of non-list member data.
                flatSize += (spyName.size() + 1
                             + sizeof(bitLength)
                             + sizeof(spyType)
                             + sizeof(isEccChecked)
                             + sizeof(isEnumerated)
			     + sizeof(isCoreRelated)
                             + clockDomain.size() + 1
                             + sizeof(clockState));

                /*
                 * Figure out how many enum strings are in the list for
                 * future unflattening.
                 */
                enumsListSize = enums.size();

                // add enumsListSize
                flatSize += sizeof(enumsListSize);

                if (0 != enumsListSize) {  // don't bother grabbing list of enums if there aren't any
                    // add length of each string
                    for (listStringIter = enums.begin(); listStringIter != enums.end(); ++listStringIter)
                    {
                        flatSize += (*listStringIter).size() + 1;
                    }

                }

                epCheckersListSize = epCheckers.size();

                // add epCheckersListSize
                flatSize += sizeof(epCheckersListSize);

                if (0 != epCheckersListSize) {  // don't bother grabbing list of epCheckers if there aren't any
                    // add length of each epCheckers string
                    for (listStringIter = epCheckers.begin(); listStringIter != epCheckers.end(); ++listStringIter)
                    {
                        flatSize += (*listStringIter).size() + 1;
                    }

                }


        } while (0);    // <- single exit.

        return flatSize;
}

#ifndef REMOVE_SIM
void  ecmdSpyData::printStruct() {

        uint32_t enumsListSize  = enums.size(); ;
        uint32_t epCheckersListSize  = epCheckers.size();

        std::list<std::string>:: iterator listStringIter;


        printf("\n\t--- Spy Data Structure ---\n");

        // Print non-list data.
        printf("\tSpy Name:  %s\n", spyName.c_str());
        printf("\tBit Length: 0x%08x\n", (uint32_t) bitLength);
        printf("\tSpy Type: 0x%08x\n", (uint32_t) spyType);
        printf("\tisEccChecked: 0x%08x\n", (uint32_t) isEccChecked);
        printf("\tisEnumerated: 0x%08x\n", (uint32_t) isEnumerated);
        printf("\tisCoreRelated: 0x%08x\n", (uint32_t) isCoreRelated);
        printf("\tSpy Name:  %s\n", clockDomain.c_str());
        printf("\tClock State: 0x%08x\n", (uint32_t) clockState);

        // print enums List Data
        if (0 == enumsListSize) {
            printf("\tNo entries in enums list\n");
        }
        else {
            // display each enum string
            printf("\tList of Enum Strings: \n");
            for (listStringIter = enums.begin(); listStringIter != enums.end(); ++listStringIter)
            {
                printf("\t\t%s\n", (*listStringIter).c_str());
            }
        }

        // print epCheckers List Data
        if (0 == epCheckersListSize) {
            printf("\tNo entries in epCheckers List\n");
        }
        else {
            // display each enum string
            printf("\tList of epCheckers Strings: \n");
            for (listStringIter = epCheckers.begin(); listStringIter != epCheckers.end(); ++listStringIter)
            {
                printf("\t\t%s\n", (*listStringIter).c_str());
            }
        }

}
#endif


// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              prahl    Initial Creation
//
// End Change Log *****************************************************

