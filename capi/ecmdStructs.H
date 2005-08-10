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


#define ECMD_CAPI_VERSION "5.3"          ///< eCMD API Version

// Magic headers for packing/unpacking the ecmdQueryData structure heirarchy.
#define QD_HDR_MAGIC     0xFFFFFFF1
#define CAGE_HDR_MAGIC   0xFFFFFF2F
#define NODE_HDR_MAGIC   0xFFFFF3FF
#define SLOT_HDR_MAGIC   0xFFFF4FFF
#define CHIP_HDR_MAGIC   0xFFF5FFFF
#define CORE_HDR_MAGIC   0xFF6FFFFF
#define THREAD_HDR_MAGIC 0xF7FFFFFF

/**
 Predefined common chip names for ecmdChipData.chipCommonType
*/
#define ECMD_CHIPT_PROCESSOR            "pu"
#define ECMD_CHIPT_MEM_BUF              "memb"
#define ECMD_CHIPT_MEM_CNTRL            "memc"
#define ECMD_CHIPT_MEM_L2CACHE          "l2cache"
#define ECMD_CHIPT_MEM_L3CACHE          "l3cache"
#define ECMD_CHIPT_IOBDG                "iobdg"
#define ECMD_CHIPT_IOHUB                "iohub"
#define ECMD_CHIPT_MUX                  "mux"
#define ECMD_CHIPT_SERVICE_PROCESSOR    "sp"






//--------------------------------------------------------------------
//  TypeDefs                                               
//--------------------------------------------------------------------

/**
 @brief Used by ecmdCacheFlush to specify which level of cache to flush
*/
typedef enum {
  ECMD_CACHE_UNKNOWN = 0,       ///< Unknown Cache Type
  ECMD_CACHE_LEVEL1D,           ///< L1 Data Cache 
  ECMD_CACHE_LEVEL1I,           ///< L1 Instruction Cache 
  ECMD_CACHE_LEVEL2,            ///< L2 Cache 
  ECMD_CACHE_LEVEL3,            ///< L3 Cache 
  ECMD_CACHE_LEVEL4,            ///< L4 Cache 
} ecmdCacheType_t;

/**
 @brief Used in ecmdChipData to describe the interface macro used by the chip
*/
typedef enum {
  ECMD_INTERFACE_ACCESS,                ///< Standard Jtag Access Macro
  ECMD_INTERFACE_CFAM,                  ///< CommonFirmwareAccessMacro 
  ECMD_INTERFACE_UNKNOWN                ///< Unknown Interface
} ecmdChipInterfaceType_t;

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
 @brief Used by SetClockSpeed interfaces to adjust clock steering procedure
 */
typedef enum {
  ECMD_CLOCK_RANGE_DEFAULT,
  ECMD_CLOCK_RANGE_LOWEST,
  ECMD_CLOCK_RANGE_LOW,
  ECMD_CLOCK_RANGE_MIDDLE,
  ECMD_CLOCK_RANGE_HIGH,
  ECMD_CLOCK_RANGE_HIGHEST
} ecmdClockRange_t;

/**
 @brief Used by SetClockSpeed interfaces to specify to do adjustment in one operation or to steer to new value
*/
typedef enum {
  ECMD_CLOCK_ONE_STEP,          ///< Change to new frequency in one operation
  ECMD_CLOCK_STEER              ///< Steer to new frequency
} ecmdClockSetMode_t;

/**
 @brief Used by SetClockSpeed interfaces to specify what notation the speed is provided in
 */
typedef enum{
  ECMD_CLOCK_FREQUENCY_SPEC,     ///< Clock speed is specified in Mhz
  ECMD_CLOCK_CYCLETIME_SPEC      ///< Clock speed is specified in cycle time
} ecmdClockSpeedType_t;

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
 @brief Used by SetClockSpeed interfaces to specify the clock to control
 */
typedef enum {
  ECMD_PROC_REFCLOCK,                   ///< Processor reference clock
  ECMD_MEMCTRL_REFCLOCK                 ///< Memory Controller reference clock
} ecmdClockType_t;

/**
 @brief Used by ecmdConfigLooperInit to enable/disable variable depth looping
*/
typedef enum {
  ECMD_VARIABLE_DEPTH_LOOP,             ///< Allow the plugin to change states to control loop (ie in the case of core vs non core scom looping)
  ECMD_STATIC_DEPTH_LOOP                ///< Use the state specified on init, don't allow plugin to change
} ecmdConfigLoopMode_t;

/**
 @brief Used by ecmdConfigLooperInit function to specify what type of data to loop on
*/
typedef enum {
  ECMD_SELECTED_TARGETS_LOOP,           ///< Loop on only targets in the system the user specified with -p# -c# -n#, etc. if not specified default to 0
  ECMD_SELECTED_TARGETS_LOOP_DEFALL,    ///< Loop on only targets in the system the user specified with -p# -c# -n#, etc. if not specified default to all
  ECMD_SELECTED_TARGETS_LOOP_VD,        ///< Loop only on targets in the system to the depth user specified on command line (ie if use said only '-n0' then -s and below are unused) if not specified default to 0
  ECMD_SELECTED_TARGETS_LOOP_VD_DEFALL, ///< Loop only on targets in the system to the depth user specified on command line (ie if use said only '-n0' then -s and below are unused) if not specified default to all
  ECMD_ALL_TARGETS_LOOP                 ///< Loop on all valid targets in the system
} ecmdConfigLoopType_t;


/**
 @brief Used by the get/set configuration functions to specify what data is good
*/
typedef enum {
  ECMD_CONFIG_VALID_FIELD_NONE,         ///< No field is valid, must have been an error
  ECMD_CONFIG_VALID_FIELD_ALPHA,        ///< The string field contains valid data
  ECMD_CONFIG_VALID_FIELD_NUMERIC,      ///< The numeric field contains valid data
  ECMD_CONFIG_VALID_FIELD_BOTH          ///< Bothe the string and numeric fields contain valid data
} ecmdConfigValid_t;

/**
 @brief This is used by ecmdQueryDllInfo to return what environment the dll is designed to run in (i.e Simulation vs Hardware)
*/
typedef enum {
  ECMD_DLL_ENV_HW,              ///< Hardware Environment
  ECMD_DLL_ENV_SIM              ///< Simulation Environment
} ecmdDllEnv_t;

/**
 @brief This is used by ecmdQueryDllInfo to return what product the dll supports
*/
typedef enum {
  ECMD_DLL_PRODUCT_UNKNOWN,     ///< Unknown product
  ECMD_DLL_PRODUCT_ECLIPZ       ///< Eclipz
} ecmdDllProduct_t;

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
 @brief Used by simOutputFusionMessage function to specify message type
 Values copied from globals.h of SimDispatcher delivery 
*/
typedef enum {
  ECMD_SIM_MSG_EXCEPTION = 1,
  ECMD_SIM_MSG_TESTCASE = 2,
  ECMD_SIM_MSG_CMD_RS = 4,       // command receive / response send
  ECMD_SIM_MSG_CMD_EXE = 8,      // command execution details (e.g. subcommands)
  ECMD_SIM_MSG_DEBUG = 16,
  ECMD_SIM_MSG_BROADSIDE = 32,
  ECMD_SIM_MSG_END_SD_MSGS = 0x000000ff,
  ECMD_SIM_MSG_ALWAYS = 0xff
} ecmdFusionMessageType_t;

/**
 @brief Used by simOutputFusionMessage function to specify message severity
 Values copied from globals.h of SimDispatcher delivery 
*/
typedef enum {
      ECMD_SIM_ERROR = 0x1,
      ECMD_SIM_WARNING = 0x2,
      ECMD_SIM_INFO = 0x3,
      ECMD_SIM_PLAIN = ~0
} ecmdFusionSeverity_t;

/**
 @brief Used by ecmdGetGlobalVar to specify what variable you are looking for
*/
typedef enum {
  ECMD_GLOBALVAR_DEBUG,         ///< Retrieve the value of the ecmd debug flag set by ECMD_DEBUG env var
  ECMD_GLOBALVAR_QUIETMODE      ///< Retrieve the value of the quiet mode debug flag = set by -quiet default = 0
} ecmdGlobalVarType_t;


/**
 @brief Used by get/putLatch functions to specify what mode should be used to find latches in the scandef
*/
typedef enum {
  ECMD_LATCHMODE_FULL,          ///< Latch must match exactly
  ECMD_LATCHMODE_PARTIAL        ///< Latch can be a partial match
} ecmdLatchMode_t;


/**
 @brief Used by ecmdQueryConfig to specify detail level of query
*/
typedef enum {
  ECMD_QUERY_DETAIL_LOW,             ///< Only config info is returned
  ECMD_QUERY_DETAIL_HIGH             ///< All info is returned
} ecmdQueryDetail_t;

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
 @brief Used by ecmdSetTraceMode to specify which trace to control
*/
typedef enum {
 ECMD_TRACE_SCAN,               ///< Scan Trace
 ECMD_TRACE_PROCEDURE           ///< Procedure Trace
} ecmdTraceType_t;








//--------------------------------------------------------------------
//  Structures                                    
//--------------------------------------------------------------------

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

#ifndef DOCUMENTATION
  // Constructor
  ecmdChipTarget();

  // Destructor
  ~ecmdChipTarget();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t i_len);
  uint32_t flattenSize(void) const;
#ifndef REMOVE_SIM
  void     printStruct(void) const;
#endif
#endif
    
  // Members
  uint32_t    cage;             ///< cage that contains node with chip
  uint32_t    node;             ///< node that contains chip
  uint32_t    slot;             ///< Card Slot/Fru to target
  std::string chipType;         ///< name of chip to access , either actual or common name
  uint32_t    pos;              ///< position of chip within node
  uint8_t     core;             ///< which core on chip to access, if chip is multi-core
  uint8_t     thread;           ///< which thread on chip to access, if chip is multi-threaded
  uint32_t    unitId;		///< This is an optional field if unitid's are used to specify the target, the above info still needs to be filled in
  ecmdChipTargetState_t cageState;            ///< cage field state
  ecmdChipTargetState_t nodeState;            ///< node field state
  ecmdChipTargetState_t slotState;            ///< slot field state
  ecmdChipTargetState_t chipTypeState;        ///< chipType field state
  ecmdChipTargetState_t posState;             ///< pos field state
  ecmdChipTargetState_t coreState;            ///< core field state
  ecmdChipTargetState_t threadState;          ///< thread field state
  ecmdChipTargetState_t unitIdState;          ///< unitId field state
  
};

#ifndef DOCUMENTATION
// ecmdChipTarget Constructor
inline ecmdChipTarget::ecmdChipTarget():
cage(0),
node(0),
slot(0),
pos(0),
core(0),
thread(0),
unitId(0),
cageState(ECMD_TARGET_UNKNOWN_STATE),
nodeState(ECMD_TARGET_UNKNOWN_STATE),
slotState(ECMD_TARGET_UNKNOWN_STATE),
chipTypeState(ECMD_TARGET_UNKNOWN_STATE),
posState(ECMD_TARGET_UNKNOWN_STATE),
coreState(ECMD_TARGET_UNKNOWN_STATE),
threadState(ECMD_TARGET_UNKNOWN_STATE),
unitIdState(ECMD_TARGET_UNKNOWN_STATE)
{
}

// ecmdChipTarget Destructor
inline ecmdChipTarget::~ecmdChipTarget() { }
#endif


/**
 @brief Used for the ecmdQueryConfig function to return thread data

 Operators Supported : <

*/
struct ecmdThreadData {

#ifndef DOCUMENTATION
  // Constructor
  ecmdThreadData();

  // Destructor
  ~ecmdThreadData();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t &i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t &i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  uint8_t   threadId;		///< (Detail: Low) Thread number of this entry
  uint32_t  unitId;		///< (Detail: High) Unit Id of this entry
};

#ifndef DOCUMENTATION
// ecmdThreadData Constructor
inline ecmdThreadData::ecmdThreadData():
threadId(0),
unitId(0)
{
}

// ecmdThreadData Destructor
inline ecmdThreadData::~ecmdThreadData() { }
#endif


/**
 @brief Used for the ecmdQueryConfig function to return core data

 Operators Supported : <

*/
struct ecmdCoreData {

#ifndef DOCUMENTATION
  // Constructor
  ecmdCoreData();

  // Destructor
  ~ecmdCoreData();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t &i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t &i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
	void printStruct(void);
#endif
#endif

  // Members
  uint8_t                       coreId;         ///< (Detail: Low) core number of this entry
  uint8_t                       numProcThreads; ///< (Detail: Low) Number of threads per core this entry supports - only valid for Processors
  uint32_t                      unitId;         ///< (Detail: High) Unit Id of this entry
  std::list<ecmdThreadData>     threadData;     ///< (Detail: Low) List of all threads avaliable for this chip - only valid for Processor compute cores - in numerical order
};

#ifndef DOCUMENTATION
// ecmdCoreData Constructor
inline ecmdCoreData::ecmdCoreData():
coreId(0),
numProcThreads(0),
unitId(0)
{
}

// ecmdCoreData Destructor
inline ecmdCoreData::~ecmdCoreData() { }
#endif

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

#ifndef DOCUMENTATION
  // Constructor
  ecmdChipData();

  // Destructor
  ~ecmdChipData();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t &i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t &i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  std::string                   chipType;           ///< (Detail: Low)  Full name of chip , ie. p6, enterprise, corona
  std::string                   chipShortType;      ///< (Detail: Low)  Short name of chip, ie. p6, ent, cor  (should be 3chars or less)
  std::string                   chipCommonType;     ///< (Detail: Low)  common name of chip, ie. pu, iohub, l3cache
  uint32_t                      pos;                ///< (Detail: Low)  Position of this entry
  uint32_t                      unitId;             ///< (Detail: High) Unit Id of this entry
  uint8_t                       numProcCores;       ///< (Detail: Low)  Number of cores this entry supports - only valid for Processor compute cores
  uint32_t                      chipEc;             ///< (Detail: High) EC level of this chip, (ec read from 'jtag' chip id or CFAM id)
  uint32_t                      simModelEc;         ///< (Detail: High) Model EC level of this chip
  ecmdChipInterfaceType_t       interfaceType;      ///< (Detail: High) Interface Macro used by the chip
  uint32_t                      chipFlags;          ///< (Detail: High) Various additional info about the chip - bitmask of defines
  std::list<ecmdCoreData>       coreData;           ///< (Detail: Low)  List of all cores requested in this chip - only valid for Processor compute cores - in numerical order by coreId
};

#ifndef DOCUMENTATION
// ecmdChipData Constructor
inline ecmdChipData::ecmdChipData():
pos(0),
unitId(0),
numProcCores(0),
chipEc(0),
simModelEc(0),
interfaceType(ECMD_INTERFACE_UNKNOWN),
chipFlags(0)
{
}

// ecmdChipData Destructor
inline ecmdChipData::~ecmdChipData() { }
#endif

/**
 @brief Used for the ecmdQueryConfig function to return slot data

 Operators Supported : <

*/
struct ecmdSlotData {

#ifndef DOCUMENTATION
  // Constructor
  ecmdSlotData();

  // Destructor
  ~ecmdSlotData();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t &i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t &i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  uint32_t slotId;                      ///< (Detail: Low) Slot number of this entry
  uint32_t unitId;                      ///< (Detail: High) Unit Id of this entry
  std::list<ecmdChipData> chipData;     ///< (Detail: Low) List of all chips requested in this slot - in order by chipType and pos
};

#ifndef DOCUMENTATION
// ecmdSlotData Constructor
inline ecmdSlotData::ecmdSlotData():
slotId(0),
unitId(0)
{
}

// ecmdSlotData Destructor
inline ecmdSlotData::~ecmdSlotData() { }
#endif

/**
 @brief Used for the ecmdQueryConfig function to return node data

 Operators Supported : <

*/
struct ecmdNodeData {

#ifndef DOCUMENTATION
  // Constructor
  ecmdNodeData();

  // Destructor
  ~ecmdNodeData();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t &i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t &i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  uint32_t nodeId;                      ///< (Detail: Low) Node number of this entry
  uint32_t unitId;                      ///< (Detail: High) Unit Id of this entry
  std::list<ecmdSlotData> slotData;     ///< (Detail: Low) List of all slots requested in this node - in numerical order by slotId
};

#ifndef DOCUMENTATION
// ecmdNodeData Constructor
inline ecmdNodeData::ecmdNodeData():
nodeId(0),
unitId(0)
{
}

// ecmdNodeData Destructor
inline ecmdNodeData::~ecmdNodeData() { }
#endif

/**
 @brief Used for the ecmdQueryConfig function to return cage data

 Operators Supported : <

*/
struct ecmdCageData {

#ifndef DOCUMENTATION
  // Constructor
  ecmdCageData();

  // Destructor
  ~ecmdCageData();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t &i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t &i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  uint32_t cageId;                      ///< (Detail: Low) Cage number of this entry
  uint32_t unitId;                      ///< (Detail: High) Unit Id of this entry
  std::list<ecmdNodeData> nodeData;     ///< (Detail: Low) List of all nodes requested in this cage - in numerical order by nodeId
};

#ifndef DOCUMENTATION
// ecmdCageData Constructor
inline ecmdCageData::ecmdCageData():
cageId(0),
unitId(0)
{
}

// ecmdCageData Destructor
inline ecmdCageData::~ecmdCageData() { }
#endif

/**
 @brief Used by the ecmdQueryConfig function to return data
*/
struct ecmdQueryData {

#ifndef DOCUMENTATION
  // Constructor
  ecmdQueryData();

  // Destructor
  ~ecmdQueryData();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t &i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t &i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  ecmdQueryDetail_t       detailLevel;  ///< (Detail: Low) This is set to the detail level of the data contained within
  std::list<ecmdCageData> cageData;     ///< (Detail: Low) List of all cages in the system - in nummerical order by cageId
};

#ifndef DOCUMENTATION
// ecmdQueryData Constructor
inline ecmdQueryData::ecmdQueryData():
detailLevel((ecmdQueryDetail_t)0x0)
{
}

// ecmdQueryData Destructor
inline ecmdQueryData::~ecmdQueryData() { }
#endif

/**
 @brief Used for the ecmdQueryRing function to return ring info
*/
struct ecmdRingData {
#ifndef DOCUMENTATION
  // Constructor
  ecmdRingData();

  // Destructor
  ~ecmdRingData();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t &i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t &i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  std::list<std::string> ringNames;                     ///< Names used to reference this ring
  uint32_t               address;                       ///< Address modifier
  int                    bitLength;                     ///< length of ring
  bool                   hasInversionMask;              ///< Ring has an inversion mask applied before scanning
  bool                   supportsBroadsideLoad;         ///< This ring supports broadside load in simulation
  bool                   isCheckable;                   ///< This ring can be run through the check_rings command
  bool                   isCoreRelated;                 ///< This scom is related to the core level of a chip
  std::string            clockDomain;                   ///< Clock domain this ring belongs to
  ecmdClockState_t       clockState;                    ///< Required clock state to access this ring
};

#ifndef DOCUMENTATION
// ecmdRingData Constructor
inline ecmdRingData::ecmdRingData():
address(0),
bitLength(0),
hasInversionMask(false),
supportsBroadsideLoad(false),
isCheckable(false),
clockState((ecmdClockState_t) ECMD_CLOCKSTATE_UNKNOWN)
{
}

// ecmdRingData Destructor
inline ecmdRingData::~ecmdRingData() { }
#endif


/**
 @brief Used for the ecmdQueryArray function to return array info
*/
struct ecmdArrayData {
#ifndef DOCUMENTATION
  // Constructor
  ecmdArrayData();

  // Destructor
  ~ecmdArrayData();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t &i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t &i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  std::string arrayName;                ///< Names used to reference this array
  int         readAddressLength;        ///< Bit length of read address
  int         writeAddressLength;       ///< Bit length of write address
  int         length;                   ///< Length of array (number of entries)
  int         width;                    ///< Bit width of array entry
  bool             isCoreRelated;       ///< This scom is related to the core level of a chip
  std::string           clockDomain;    ///< Clock domain this array belongs to
  ecmdClockState_t       clockState;    ///< Required clock state to access this array
};

#ifndef DOCUMENTATION
// ecmdArrayData Constructor
inline ecmdArrayData::ecmdArrayData():
readAddressLength(0),
writeAddressLength(0),
length(0),
width(0),
clockState((ecmdClockState_t) ECMD_CLOCKSTATE_UNKNOWN)
{
}

// ecmdArrayData Destructor
inline ecmdArrayData::~ecmdArrayData() { }
#endif


/**
 @brief Used for the ecmdQueryTraceArray function to return trace array info
*/
struct ecmdTraceArrayData {
#ifndef DOCUMENTATION
  // Constructor
  ecmdTraceArrayData();

  // Destructor
  ~ecmdTraceArrayData();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t &i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t &i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  std::string      traceArrayName;      ///< Name of Trace array
  int              length;              ///< Length of trace array (number of entries)
  int              width;               ///< Bit width of trace array entry
  bool             isCoreRelated;       ///< This scom is related to the core level of a chip
  std::string      clockDomain;         ///< Clock domain this array belongs to
  ecmdClockState_t clockState;          ///< Required clock state to access this array
};

#ifndef DOCUMENTATION
// ecmdTraceArrayData Constructor
inline ecmdTraceArrayData::ecmdTraceArrayData():
length(0),
width(0),
isCoreRelated(false),
clockState((ecmdClockState_t) ECMD_CLOCKSTATE_UNKNOWN)
{
}

// ecmdTraceArrayData Destructor
inline ecmdTraceArrayData::~ecmdTraceArrayData() { }
#endif

/**
 @brief Used for the ecmdQueryScom function to return scom info
*/
struct ecmdScomData {
#ifndef DOCUMENTATION
  // Constructor
  ecmdScomData();

  // Destructor
  ~ecmdScomData();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t &i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t &i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  uint32_t         address;             ///< Scom Address
  bool             isCoreRelated;       ///< This scom is related to the core level of a chip
  std::string      clockDomain;         ///< Clock domain this array belongs to
  ecmdClockState_t clockState;          ///< Required clock state to access this scom
};

#ifndef DOCUMENTATION
// ecmdScomData Constructor
inline ecmdScomData::ecmdScomData():
address(0),
isCoreRelated(false),
clockState((ecmdClockState_t) ECMD_CLOCKSTATE_UNKNOWN)
{
}

// ecmdScomData Destructor
inline ecmdScomData::~ecmdScomData() { }
#endif

/**
 @brief Used by the getArrayMultiple function to pass data
*/
struct ecmdArrayEntry {
#ifndef DOCUMENTATION
  // Constructor
  ecmdArrayEntry();

  // Destructor
  ~ecmdArrayEntry();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t i_len);
  uint32_t flattenSize(void) const;
#ifndef REMOVE_SIM
  void printStruct(void) const;
#endif
#endif

  // Members
  ecmdDataBuffer address;               ///< Array address/element to access
  ecmdDataBuffer buffer;                ///< Array data from address
  uint32_t       rc;                    ///< Error code in retrieving this entry
};

#ifndef DOCUMENTATION
// ecmdArrayEntry Constructor
inline ecmdArrayEntry::ecmdArrayEntry():
rc(0)
{
}

// ecmdArrayEntry Destructor
inline ecmdArrayEntry::~ecmdArrayEntry() { }
#endif


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
#ifndef DOCUMENTATION
  // Constructor
  ecmdNameEntry();

  // Destructor
  ~ecmdNameEntry();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t &i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t &i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  std::string    name;                  ///< Name of entry 
  ecmdDataBuffer buffer;                ///< Data to/from entry
  uint32_t       rc;                    ///< Error code in retrieving this entry
};

#ifndef DOCUMENTATION
// ecmdNameEntry Constructor
inline ecmdNameEntry::ecmdNameEntry():
rc(0)
{
}

// ecmdNameEntry Destructor
inline ecmdNameEntry::~ecmdNameEntry() { }
#endif

/**
 @brief Used by getTraceArrayMultiple function to pass data
*/
struct ecmdNameVectorEntry {
#ifndef DOCUMENTATION
  // Constructor
  ecmdNameVectorEntry();

  // Destructor
  ~ecmdNameVectorEntry();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t &i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t &i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  std::string    name;                  ///< Name of entry 
  std::vector<ecmdDataBuffer> buffer;   ///< Vector of data to/from entry
  uint32_t       rc;                    ///< Error code in retrieving this entry
};

#ifndef DOCUMENTATION
// ecmdNameVectorEntry Constructor
inline ecmdNameVectorEntry::ecmdNameVectorEntry():
rc(0)
{
}

// ecmdNameVectorEntry Destructor
inline ecmdNameVectorEntry::~ecmdNameVectorEntry() { }
#endif

/**
 @brief Used by ???? function to pass data
*/
struct ecmdIndexVectorEntry {
  // Members
  int            index;                 ///< Index of entry
  std::vector<ecmdDataBuffer> buffer;   ///< Vector of data to/from entry
  uint32_t       rc;                    ///< Error code in retrieving this entry
};


/**
 @brief Used by get/put Gpr/Fpr Multiple function to pass data
*/
struct ecmdIndexEntry {
#ifndef DOCUMENTATION
  // Constructor
  ecmdIndexEntry();

  // Destructor
  ~ecmdIndexEntry();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t &i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t &i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  int            index;                 ///< Index of entry
  ecmdDataBuffer buffer;                ///< Data to/from entry
  uint32_t       rc;                    ///< Error code in retrieving this entry
}; 

#ifndef DOCUMENTATION
// ecmdIndexEntry Constructor
inline ecmdIndexEntry::ecmdIndexEntry():
index(0),
rc(0)
{
}

// ecmdIndexEntry Destructor
inline ecmdIndexEntry::~ecmdIndexEntry() { }
#endif

/**
 @brief Used by getlatch function to return data
*/
struct ecmdLatchEntry {
#ifndef DOCUMENTATION
  // Constructor
  ecmdLatchEntry();

  // Destructor
  ~ecmdLatchEntry();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t &i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t &i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  std::string    latchName;             ///< Latch name of entry
  std::string    ringName;              ///< Ring that latch came from
  ecmdDataBuffer buffer;                ///< Latch data
  int            latchStartBit;         ///< Start bit of data inside latch
  int            latchEndBit;           ///< End bit of data inside latch
  uint32_t       rc;                    ///< Error code in retrieving this entry
};

#ifndef DOCUMENTATION
// ecmdLatchEntry Constructor
inline ecmdLatchEntry::ecmdLatchEntry():
latchStartBit(0),
latchEndBit(0),
rc(0)
{
}

// ecmdLatchEntry Destructor
inline ecmdLatchEntry::~ecmdLatchEntry() { }
#endif

/**
 @brief Used by ecmdQueryProcRegisterInfo function to return data about a Architected register
*/
struct ecmdProcRegisterInfo {
#ifndef DOCUMENTATION
  // Constructor
  ecmdProcRegisterInfo();

  // Destructor
  ~ecmdProcRegisterInfo();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t &i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t &i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  int bitLength;                ///< Bit length of each entry
  int totalEntries;             ///< Total number of entries available
  bool threadReplicated;        ///< Register is replicated for each thread
};

#ifndef DOCUMENTATION
// ecmdProcRegisterInfo Constructor
inline ecmdProcRegisterInfo::ecmdProcRegisterInfo():
bitLength(0),
totalEntries(0),
threadReplicated(false)
{
}

// ecmdProcRegisterInfo Destructor
inline ecmdProcRegisterInfo::~ecmdProcRegisterInfo() { }
#endif



/**
 @brief Used for the ecmdQuerySpy function to return spy info
*/
struct ecmdSpyData {

#ifndef DOCUMENTATION
  // Constructor
  ecmdSpyData();

  // Destructor
  ~ecmdSpyData();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t &i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t &i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  std::string   spyName;                ///< Names used to reference this spy
  int           bitLength;              ///< length of spy
  ecmdSpyType_t spyType;                ///< Type of spy
  bool          isEccChecked;           ///< This spy affects some ECC groupings
  bool          isEnumerated;           ///< This spy has enumerated values
  bool          isCoreRelated;          ///< This spy is related to the core level of a chip

  std::string           clockDomain;    ///< Clock domain this spy belongs to
  ecmdClockState_t       clockState;    ///< Required clock state to access this spy
  
  std::list<std::string> enums;         ///< Possible enum values for Spy - I/P Can only provide this on a client, not on the FSP
  std::list<std::string> epCheckers;     ///< Possible epChecker names affected by this Spy
};

#ifndef DOCUMENTATION
// ecmdSpyData Constructor
inline ecmdSpyData::ecmdSpyData():
bitLength(0),
isEccChecked(false),
isEnumerated(false),
isCoreRelated(false)
{
}

// ecmdSpyData Destructor
inline ecmdSpyData::~ecmdSpyData() { }
#endif

/**
 @brief Used by ecmdReadDcard
*/
struct ecmdMemoryEntry {
  uint64_t address;
  ecmdDataBuffer data;
  ecmdDataBuffer tags;
};


#ifndef ECMD_PERLAPI
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
#endif


/* @name Misc functions */
//@{

/** @brief Returns the version of the shared lib so it can be compared with the other versions */
std::string ecmdGetSharedLibVersion();

//@}

#endif
