#ifndef ecmdStructs_H
#define ecmdStructs_H
// Copyright **********************************************************
//                                                                      
// File ecmdStructs.H                                               
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 2003                                         
//                                                                      
// The source code for this program is not published or otherwise       
// divested of its trade secrets, irrespective of what has been         
// deposited with the U.S. Copyright Office.                             
//                                                                      
// End Copyright ******************************************************

/* $Header$ */

/**
  @file ecmdStructs.H
  @brief All the Structures required for the eCMD Capi
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <inttypes.h>
#include <list>  /* For STL list */
#include <vector>
#include <string>

#include <ecmdDataBuffer.H>

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//  Defines                                                
//--------------------------------------------------------------------

#define ECMD_CAPI_VERSION "3.1"          ///< eCMD API Version


/**
 @brief This is used by ecmdQueryDllInfo to return who's dll you are actually running against 
*/

typedef enum {
  ECMD_DLL_UNKNOWN,             ///< This should never be encountered
  ECMD_DLL_STUB,                ///< This is a stub version of the dll for client testing
  ECMD_DLL_CRONUS,              ///< Running against the Cronus Dll
  ECMD_DLL_IPSERIES,            ///< Running against I/P Series HOM
  ECMD_DLL_ZSERIES,             ///< Running against Z Series HOM
  ECMD_DLL_SCAND                ///< Running against the ScanD dll owned by Meghna Paruthi

} ecmdDllType_t;

/**
 @brief This is used by ecmdQueryDllInfo to return what product the dll supports
*/
typedef enum {
  ECMD_DLL_PRODUCT_UNKNOWN,     ///< Unknown product
  ECMD_DLL_PRODUCT_ECLIPZ       ///< Eclipz
} ecmdDllProduct_t;

/**
 @brief This is used by ecmdQueryDllInfo to return what environment the dll is designed to run in (i.e Simulation vs Hardware)
*/
typedef enum {
  ECMD_DLL_ENV_HW,              ///< Hardware Environment
  ECMD_DLL_ENV_SIM              ///< Simulation Environment
} ecmdDllEnv_t;

/**
 @brief This is used by ecmdQueryDllInfo to return info to the client about what Dll instance they are actually running with
*/

struct ecmdDllInfo {
  ecmdDllType_t         dllType;        ///< Dll instance type running
  ecmdDllProduct_t      dllProduct;     ///< Dll product supported
  std::string           dllProductType; ///< Dll product type currently configured
  ecmdDllEnv_t          dllEnv;         ///< Dll environment (Simulation vs Hardware)
  std::string           dllBuildDate;   ///< Date the Dll was built
  std::string           dllCapiVersion; ///< should be set to ECMD_CAPI_VERSION
  std::string           dllBuildInfo;   ///< Any additional info the Dll/Plugin would like to pass
};


/**
 Predefined common chip names for ecmdChipData.chipCommonType
*/
#define ECMD_CHIPT_PROCESSOR    "pu"
#define ECMD_CHIPT_MEM_BUF      "memb"
#define ECMD_CHIPT_MEM_CNTRL    "memc"
#define ECMD_CHIPT_MEM_L2CACHE  "l2cache"
#define ECMD_CHIPT_MEM_L3CACHE  "l3cache"
#define ECMD_CHIPT_IOBDG        "iobdg"
#define ECMD_CHIPT_IOHUB        "iohub"


/**
 @brief Used by ecmdChipTarget to describe the value in the state fields
 - The ECMD_TARGET_FIELD_* states are used for functions to return applicable values
 - The ECMD_TARGET_QUERY_* states are used by the ecmdQueryConfig and ecmdQuerySelected functions to refine the query
*/

typedef enum {
  ECMD_TARGET_UNKNOWN_STATE,    ///< State field has not been initialized

  ECMD_TARGET_FIELD_VALID,      ///< Associated State Field is valid for this function
  ECMD_TARGET_FIELD_UNUSED,     ///< Associated State Field is unused for this function

  ECMD_TARGET_QUERY_FIELD_VALID,///< Associated State Field is valid for the query
  ECMD_TARGET_QUERY_WILDCARD,   ///< Associated State Field should be itterated on and all valid results returned
  ECMD_TARGET_QUERY_IGNORE,     ///< Query should be limited to data above this field, ignoring data 

  ECMD_TARGET_THREAD_ALIVE      ///< Used when calling thread dependent functions tell the function to check for the thread to be alive before running
} ecmdChipTargetState_t;


/**
 @brief Structure used to designate which cec object/chip you would like the function to operate on

  - The state bits are used by D/A functions to tell the calling function what level of granularity the function operates on
    Ex.  putmem/getmem display memory through the processor, they are only dependent on cage/node/pos because they do not use
          the cores to perform their function
         However put/getspr display architected registers from the processor, they will signify that cage/node/pos/core and depending
          on the particular spr referenced threads may be valid
  - The state bits are used slightly differently for the queryFunctions they are used there to signify
     what data coming in is valid to refine a query
*/
struct ecmdChipTarget {

  uint32_t    cage;             ///< cage that contains node with chip
  uint32_t    node;             ///< node that contains chip
  uint32_t    slot;             ///< Card Slot/Fru to target
  std::string chipType;         ///< name of chip to access , either actual or common name
  uint32_t    pos;              ///< position of chip within node
  uint8_t     core;             ///< which core on chip to access, if chip is multi-core
  uint8_t     thread;           ///< which thread on chip to access, if chip is multi-threaded

  ecmdChipTargetState_t cageState;            ///< cage field state
  ecmdChipTargetState_t nodeState;            ///< node field state
  ecmdChipTargetState_t slotState;            ///< slot field state
  ecmdChipTargetState_t chipTypeState;        ///< chipType field state
  ecmdChipTargetState_t posState;             ///< pos field state
  ecmdChipTargetState_t coreState;            ///< core field state
  ecmdChipTargetState_t threadState;          ///< thread field state

  uint32_t unitId;           ///< This is an optional field if unitid's are used to specify the target, the above info still needs to be filled in
  ecmdChipTargetState_t unitIdState;          ///< unitId field state
  
};






/**
 @brief Used in ecmdChipData to describe the interface macro used by the chip
*/
typedef enum {
  ECMD_INTERFACE_ACCESS,                ///< Standard Jtag Access Macro
  ECMD_INTERFACE_CFAM,                  ///< CommonFirmwareAccessMacro - Fsi interface
  ECMD_INTERFACE_UNKNOWN                ///< Unknown Interface
} ecmdChipInterfaceType_t;

/**
 @brief Used by ecmdQueryConfig to specify detail level of query
*/
typedef enum {
  ECMD_QUERY_DETAIL_LOW,             ///< Only config info is returned
  ECMD_QUERY_DETAIL_HIGH             ///< All info is returned
} ecmdQueryDetail_t;


/**
 @brief Used for the ecmdQueryConfig function to return thread data

 Operators Supported : <

*/
struct ecmdThreadData {
  uint8_t threadId;                             ///< (Detail: Low) Thread number of this entry
};

/**
 @brief Used for the ecmdQueryConfig function to return core data

 Operators Supported : <

*/
struct ecmdCoreData {
  uint8_t                       coreId;         ///< (Detail: Low) core number of this entry
  uint8_t                       numProcThreads; ///< (Detail: Low) Number of threads per core this entry supports - only valid for Processors
  std::list<ecmdThreadData>     threadData;     ///< (Detail: Low) List of all threads avaliable for this chip - only valid for Processor compute cores - in numerical order
};


/**
 Defines for the ecmdChipData chipFlags field
*/
/* The Bus type flags use the first two bits of the chipflags field */
#define ECMD_CHIPFLAG_BUSMASK   0xC0000000
#define ECMD_CHIPFLAG_RSVDBUS1  0x00000000      ///< This is reserved for later expansion (should not be used)
#define ECMD_CHIPFLAG_JTAG      0x40000000
#define ECMD_CHIPFLAG_FSI       0x80000000
#define ECMD_CHIPFLAG_RSVDBUS2  0xC0000000      ///< This is reserved for later expansion (should not be used) 

/**
 @brief Used for the ecmdQueryConfig function to return chip data

 Operators Supported : <

*/
struct ecmdChipData {
  std::string                   chipType;           ///< (Detail: Low)  actual name of chip , ie. gr, ent (should be 3chars or less)
  std::string                   chipCommonType;     ///< (Detail: Low)  common name of chip, ie. pu, riohub
  uint32_t                      pos;                ///< (Detail: Low)  Position of this entry
  uint8_t                       numProcCores;       ///< (Detail: Low)  Number of cores this entry supports - only valid for Processor compute cores
  uint32_t                      chipEc;             ///< (Detail: High) EC level of this chip, usually 0-F (ec read from 'jtag' chip id or CFAM id)
  uint32_t                      simModelEc;         ///< (Detail: High) Model EC level of this chip
  ecmdChipInterfaceType_t       interfaceType;      ///< (Detail: High) Interface Macro used by the chip
  uint32_t                      chipFlags;          ///< (Detail: High) Various additional info about the chip - bitmask of defines
  std::list<ecmdCoreData>       coreData;           ///< (Detail: Low)  List of all cores requested in this chip - only valid for Processor compute cores - in numerical order by coreId
};

/**
 @brief Used for the ecmdQueryConfig function to return slot data

 Operators Supported : <

*/
struct ecmdSlotData {
  uint32_t slotId;                      ///< (Detail: Low) Slot number of this entry
  std::list<ecmdChipData> chipData;     ///< (Detail: Low) List of all chips requested in this slot - in order by chipType and pos
};

/**
 @brief Used for the ecmdQueryConfig function to return node data

 Operators Supported : <

*/
struct ecmdNodeData {
  uint32_t nodeId;                      ///< (Detail: Low) Node number of this entry
  std::list<ecmdSlotData> slotData;     ///< (Detail: Low) List of all slots requested in this node - in numerical order by slotId
};

/**
 @brief Used for the ecmdQueryConfig function to return cage data

 Operators Supported : <

*/
struct ecmdCageData {
  uint32_t cageId;                      ///< (Detail: Low) Cage number of this entry
  std::list<ecmdNodeData> nodeData;     ///< (Detail: Low) List of all nodes requested in this cage - in numerical order by nodeId
};


/**
 @brief Used by the ecmdQueryConfig function to return data
*/
struct ecmdQueryData {
  ecmdQueryDetail_t       detailLevel;  ///< (Detail: Low) This is set to the detail level of the data contained within
  std::list<ecmdCageData> cageData;     ///< (Detail: Low) List of all cages in the system - in nummerical order by cageId
};




/**
 @brief Used by Ring/Array/Spy Query functions to return a required clock state
*/
typedef enum {
  ECMD_CLOCKSTATE_UNKNOWN,              ///< Unable to determine a required clock state
  ECMD_CLOCKSTATE_ON,                   ///< Chip clocks must be on to access
  ECMD_CLOCKSTATE_OFF,                  ///< Chip clocks must be off to access
  ECMD_CLOCKSTATE_NA                    ///< Chip clocks can be in any state to access
} ecmdClockState_t;

/**
 @brief Used for the ecmdQueryRing function to return ring info
*/
struct ecmdRingData {
  std::list<std::string> ringNames;     ///< Names used to reference this ring
  uint32_t address;                     ///< Address modifier
  int      bitLength;                   ///< length of ring
  bool     hasInversionMask;            ///< Ring has an inversion mask applied before scanning
  bool     supportsBroadsideLoad;       ///< This ring supports broadside load in simulation
  bool     isCheckable;                 ///< This ring can be run through the check_rings command
  std::string           clockDomain;    ///< Clock domain this ring belongs to
  ecmdClockState_t      clockState;     ///< Required clock state to access this ring
};



/**
 @brief Used for the ecmdQueryArray function to return array info
*/
struct ecmdArrayData {
  std::string arrayName;                ///< Names used to reference this array
  int         addressLength;            ///< Bit length of address
  int         length;                   ///< Length of array (number of entries)
  int         width;                    ///< Bit width of array entry
  std::string           clockDomain;    ///< Clock domain this array belongs to
  ecmdClockState_t       clockState;    ///< Required clock state to access this array
};

/**
 @brief Used by the getArrayMultiple function to pass data
*/
struct ecmdArrayEntry {
  ecmdDataBuffer address;               ///< Array address/element to access
  ecmdDataBuffer buffer;                ///< Array data from address
  uint32_t       rc;                    ///< Error code in retrieving this entry
};

/**
 @brief Used by get/putspy function to create the return data from a group
*/
struct ecmdSpyGroupData {
  ecmdDataBuffer extractBuffer;         ///< The data read from the ring buffer
  ecmdDataBuffer deadbitsMask;          ///< A mask of the bits that were deadbits in that buffer
};


/**
 @brief Used by get/putSprMultiple function to pass data
*/
struct ecmdNameEntry {
  std::string    name;                  ///< Name of entry 
  ecmdDataBuffer buffer;                ///< Data to/from entry
  uint32_t       rc;                    ///< Error code in retrieving this entry
};

/**
 @brief Used by getTraceArrayMultiple function to pass data
*/
struct ecmdNameVectorEntry {
  std::string    name;                  ///< Name of entry 
  std::vector<ecmdDataBuffer> buffer;   ///< Vector of data to/from entry
  uint32_t       rc;                    ///< Error code in retrieving this entry
};

/**
 @brief Used by get/put Gpr/Fpr Multiple function to pass data
*/
struct ecmdIndexEntry {
  int            index;                 ///< Index of entry
  ecmdDataBuffer buffer;                ///< Data to/from entry
  uint32_t       rc;                    ///< Error code in retrieving this entry
}; 

/**
 @brief Used by getlatch function to return data
*/
struct ecmdLatchEntry {
  std::string    latchName;             ///< Latch name of entry
  std::string    ringName;              ///< Ring that latch came from
  ecmdDataBuffer buffer;                ///< Latch data
  int            latchStartBit;         ///< Start bit of data inside latch
  int            latchEndBit;           ///< End bit of data inside latch
  uint32_t       rc;                    ///< Error code in retrieving this entry
};


/**
 @brief Used by ecmdQueryProcRegisterInfo function to return data about a Architected register
*/
struct ecmdProcRegisterInfo {
  int bitLength;                ///< Bit length of each entry
  int totalEntries;             ///< Total number of entries available
  bool threadReplicated;        ///< Register is replicated for each thread
};


/**
 @brief Used for the ecmdQuerySpy function to specify which type of spy we have @see ecmdSpyData
*/
typedef enum {
  ECMD_SPYTYPE_ALIAS,   ///< Spy is an alias
  ECMD_SPYTYPE_IDIAL,   ///< Spy is an iDial
  ECMD_SPYTYPE_EDIAL,   ///< Spy is an eDial
  ECMD_SPYTYPE_ECCGROUP ///< Spy is an eccGrouping
} ecmdSpyType_t;

/**
 @brief Used for the ecmdQuerySpy function to return spy info
*/
struct ecmdSpyData {
  std::string   spyName;                ///< Names used to reference this spy
  int           bitLength;              ///< length of spy
  ecmdSpyType_t spyType;                ///< Type of spy
  bool          isEccChecked;           ///< This spy affects some ECC groupings
  bool          isEnumerated;           ///< This spy has enumerated values

  std::string           clockDomain;    ///< Clock domain this spy belongs to
  ecmdClockState_t       clockState;    ///< Required clock state to access this spy
  
  std::list<std::string> enums;         ///< Possible enum values for Spy - I/P Can only provide this on a client, not on the FSP
  std::list<std::string> epCheckers;     ///< Possible epChecker names affected by this Spy
};


/**
 @brief Used for the ecmdQueryFileLocation function to specify the file type you are looking for
*/
typedef enum {
  ECMD_FILE_SCANDEF,    ///< Scandef file type
  ECMD_FILE_SPYDEF,     ///< Spy Definition file
  ECMD_FILE_ARRAYDEF,   ///< Array Definition file
  ECMD_FILE_HELPTEXT,   ///< eCMD Help Text file - target field of ecmdQueryFileLocation is not used for this and just a path is returned
  ECMD_FILE_SCOMDATA,   ///< eCMD ScanComm Parse data files, used by getscom - target field of ecmdQueryFileLocation is not used for this and just a path is returned
  ECMD_FILE_SPYDEFHASH,    ///< Hash file for spy definition
  ECMD_FILE_SCANDEFHASH ///< Hash file for the scandef
} ecmdFileType_t; 


/**
 @brief Used by ecmdConfigLooperInit function to specify what type of data to loop on
*/
typedef enum {
  ECMD_SELECTED_TARGETS_LOOP,   ///< Loop on only targets in the system the user specified with -p# -c# -n#, etc
  ECMD_ALL_TARGETS_LOOP         ///< Loop on all valid targets in the system
} ecmdConfigLoopType_t;

/**
 @brief Used internally by ecmdConfigLooper to store looping state information
*/
struct ecmdLooperData {
  bool             ecmdLooperInitFlag;                  ///< Is fresh ?
  bool             ecmdUseUnitid;                       ///< This looper is looping on unitid targets not config data

  ecmdQueryData    ecmdSystemConfigData;                ///< Config data queried from the system
  std::list<ecmdCageData>::iterator ecmdCurCage;        ///< Pointer to current Cage
  std::list<ecmdNodeData>::iterator ecmdCurNode;        ///< Pointer to current Node
  std::list<ecmdSlotData>::iterator ecmdCurSlot;        ///< Pointer to current Slot
  std::list<ecmdChipData>::iterator ecmdCurChip;        ///< Pointer to current Chip
  std::list<ecmdCoreData>::iterator ecmdCurCore;        ///< Pointer to current Core
  std::list<ecmdThreadData>::iterator ecmdCurThread;    ///< Pointer to current Thread
  ecmdChipTarget prevTarget;                            ///< Pointer to previous target


  std::list<ecmdChipTarget>     unitIdTargets;          ///< List of targets if looping on a unitid
  std::list<ecmdChipTarget>::iterator curUnitIdTarget;  ///< Pointer to current unitid target
};


/**
 @brief Used by ecmdGetGlobalVar to specify what variable you are looking for
*/
typedef enum {
  ECMD_GLOBALVAR_DEBUG,         ///< Retrieve the value of the ecmd debug flag set by ECMD_DEBUG env var
  ECMD_GLOBALVAR_QUIETMODE      ///< Retrieve the value of the quiet mode debug flag = set by -quiet default = 0
} ecmdGlobalVarType_t;



/**
 @brief Used by ecmdSetTraceMode to specify which trace to control
*/
typedef enum {
 ECMD_TRACE_SCAN,               ///< Scan Trace
 ECMD_TRACE_PROCEDURE           ///< Procedure Trace
} ecmdTraceType_t;


/**
 @brief Used by get/putLatch functions to specify what mode should be used to find latches in the scandef
*/
typedef enum {
  ECMD_LATCHMODE_FULL,          ///< Latch must match exactly
  ECMD_LATCHMODE_PARTIAL        ///< Latch can be a partial match
} ecmdLatchMode_t;


/**
 @brief Used by the eCMD Function parm trace printer
*/

typedef enum {
  ECMD_FPP_RETOUT,                      ///< Designates the failing return code out of the api function
  ECMD_FPP_FUNCTIONIN,                  ///< Designates the call in of the api function
  ECMD_FPP_FUNCTIONOUT,                 ///< Designatest the call out of the api function
  ECMD_FPP_JUSTIN,                      ///< Designatest the call in of the api function in debug 8 mode
  ECMD_FPP_JUSTOUT                      ///< Designatest the call out of the api function in debug 8 mode and rc==0
} efppInOut_t;

/**
 @brief Used by the get/set configuration functions to specify what data is good
*/
typedef enum {
  ECMD_CONFIG_VALID_FIELD_NONE,         ///< No field is valid, must have been an error
  ECMD_CONFIG_VALID_FIELD_ALPHA,        ///< The string field contains valid data
  ECMD_CONFIG_VALID_FIELD_NUMERIC,      ///< The numeric field contains valid data
  ECMD_CONFIG_VALID_FIELD_BOTH          ///< Bothe the string and numeric fields contain valid data
} ecmdConfigValid_t;


/* @name Operators */
//@{

/** @brief Used to sort Cage entries in an ecmdCageData list. */
bool operator< (const ecmdCageData& lhs, const ecmdCageData& rhs);

/** @brief Used to sort Node entries in an ecmdNodeData list. */
bool operator< (const ecmdNodeData& lhs, const ecmdNodeData& rhs);

/** @brief Used to sort Slot entries in an ecmdSlotData list. */
bool operator< (const ecmdSlotData& lhs, const ecmdSlotData& rhs);

/** @brief Used to sort Chip entries (based on Pos) in an ecmdChipData list. */
bool operator< (const ecmdChipData& lhs, const ecmdChipData& rhs);

/** @brief Used to sort Core entries in an ecmdCoreData list. */
bool operator< (const ecmdCoreData& lhs, const ecmdCoreData& rhs);

/** @brief Used to sort Thread entries in an ecmdThreadData list. */
bool operator< (const ecmdThreadData& lhs, const ecmdThreadData& rhs);



//@}


/* @name Misc functions */
//@{

/** @brief Returns the version of the shared lib so it can be compared with the other versions */
std::string ecmdGetSharedLibVersion();

//@}

#endif
