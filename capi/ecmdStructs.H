#ifndef ecmdStructs_H
#define ecmdStructs_H
/* $Header$ */
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
// Change Log *********************************************************
//                                                                      
//  Flag Reason   Date      Coder     Description                       
//  ---- -------- --------  --------  -------------------------------   
//  @01x CQ32245  11/08/05  prahl     Clean up Beam errors
//       FW038339 03/30/06  scottw    Change i_len parms to pass by value
//  @02  FW038451 03/30/06  scottw    Add std methods to ecmdI2CCmdEntry

//  @03                05/08/07 hjh       add of the continue on error option -coe
  
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

#include <ecmdDefines.H>
#include <ecmdDataBuffer.H>

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//  Defines                                                
//--------------------------------------------------------------------


#define ECMD_CAPI_VERSION "9.0"          ///< eCMD API Version

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
#define ECMD_CHIPT_IOU                  "iou"
#define ECMD_CHIPT_NOCHIP               "nochip"


#define ECMD_UNSET          0xFFFFFFFF ///< Used to signify unset or -1 for uint32_t types


#define ECMD_TARGETDEPTH_NA 0xFFFFFFFF ///< Used to signify a level of the ecmdChipTarget depth is not used (only valid for node/slot depths)

#define ECMD_ARG_LIMIT      20         ///< Used in a few spots set the limit of command line args

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
  ECMD_INTERFACE_UNKNOWN,               ///< Unknown Interface
  ECMD_INTERFACE_ACCESS,                ///< Standard Jtag Access Macro
  ECMD_INTERFACE_CFAM                   ///< CommonFirmwareAccessMacro 
} ecmdChipInterfaceType_t;

/**
 @brief Used by ecmdChipTarget to describe the value in the state fields
*/

typedef enum {
  ECMD_TARGET_UNKNOWN_STATE,    ///< State field has not been initialized
  ECMD_TARGET_FIELD_VALID,      ///< Associated State Field is set to a valid value
  ECMD_TARGET_FIELD_UNUSED,     ///< Associated State Field is unused and should be ignored
  ECMD_TARGET_FIELD_WILDCARD,   ///< Associated State Field is a wildcard and should be iterated on in query functions
  ECMD_TARGET_THREAD_ALIVE,     ///< Used when calling thread dependent functions tell the function to check for the thread to be alive before running
} ecmdChipTargetState_t;

/**
 @brief Used by SetClockSpeed interfaces to adjust clock steering procedure
 */
typedef enum {
  ECMD_CLOCK_RANGE_UNKNOWN,
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
  ECMD_CLOCK_UNKNOWN,           ///< Default for not initialized
  ECMD_CLOCK_ONE_STEP,          ///< Change to new frequency in one operation
  ECMD_CLOCK_STEER              ///< Steer to new frequency
} ecmdClockSetMode_t;

/**
 @brief Used by SetClockSpeed interfaces to specify what notation the speed is provided in
 */
typedef enum {
  ECMD_CLOCK_FREQUENCY_UNKNOWN,         ///< Default for not initialized
  ECMD_CLOCK_FREQUENCY_KHZ_SPEC,        ///< Clock speed is specified in Khz
  ECMD_CLOCK_FREQUENCY_MHZ_SPEC,        ///< Clock speed is specified in Mhz
  ECMD_CLOCK_CYCLETIME_US_SPEC,         ///< Clock speed is specified in microseconds cycle time
  ECMD_CLOCK_CYCLETIME_PS_SPEC,         ///< Clock speed is specified in picoseconds cycle time
  ECMD_CLOCK_FREQUENCY_SPEC = ECMD_CLOCK_FREQUENCY_MHZ_SPEC,    ///< Clock speed is specified in Mhz
  ECMD_CLOCK_CYCLETIME_SPEC = ECMD_CLOCK_CYCLETIME_PS_SPEC,     ///< Clock speed is specified in picoseconds cycle time
  ECMD_CLOCK_NOMINAL_PERCENT_UP,        ///< Clock speed specified in % above nominal
  ECMD_CLOCK_NOMINAL_PERCENT_DOWN,      ///< Clock speed specified in % below nominal
  ECMD_CLOCK_CURRENT_PERCENT_UP,        ///< Clock speed specified in % above current value
  ECMD_CLOCK_CURRENT_PERCENT_DOWN,      ///< Clock speed specified in % below current value
  ECMD_CLOCK_POWERSAVE_PERCENT_UP,      ///< Clock speed specified in % above powersave value
  ECMD_CLOCK_POWERSAVE_PERCENT_DOWN     ///< Clock speed specified in % below powersave value
} ecmdClockSpeedType_t;

/**
 @brief Used by ecmdBiasVoltage interfaces to specify what notation the voltage is provided in
 */
typedef enum {
  ECMD_VOLTAGE_UNKNOWN,                 ///< Default
  ECMD_VOLTAGE_NOMINAL,                 ///< Set the voltaget to nominal
  ECMD_VOLTAGE_PERCENT_DOWN,            ///< Voltage in % below nominal
  ECMD_VOLTAGE_PERCENT_UP,              ///< Voltage in % above nominal value
  ECMD_VOLTAGE_POWERSAVE_PERCENT_DOWN,  ///< Voltage in % below powersave value
  ECMD_VOLTAGE_POWERSAVE_PERCENT_UP     ///< Voltage in % above powersave value
} ecmdVoltageType_t;

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
  ECMD_CLOCKTYPE_UNKNOWN,               ///< Default for not initialized
  ECMD_PROC_REFCLOCK,                   ///< Processor reference clock
  ECMD_MEMCTRL_REFCLOCK,                ///< Memory Controller reference clock
  ECMD_PROC_CORE_CLOCK,                 ///< Processor Core Clock (actual speed of the domain)
  ECMD_PROC_NEST_CLOCK,                 ///< Processor Nest Clock (actual speed of the domain)
  ECMD_MEMCTRL_CLOCK,                   ///< Memory Controller Clock (actual speed of the chip)
  ECMD_IO_REFCLOCK,                     ///< IO Chip reference clock
  ECMD_IO_CLOCK                         ///< IO Chip Clock (actual speed of the chip)
} ecmdClockType_t;

/**
 @brief Used by ecmdConfigLooperInit to enable/disable variable depth looping
*/
typedef enum {
  ECMD_LOOP_MODE_UNKNOWN,               ///< Default for not initialized
  ECMD_STATIC_DEPTH_LOOP                ///< Use the state specified on init, don't allow plugin to change
} ecmdConfigLoopMode_t;


/**
 @brief Used by ecmdConfigLooperInit function to specify what type of data to loop on
*/
typedef enum {
  ECMD_LOOP_TYPE_UNKNOWN,               ///< Default for not initialized
  ECMD_SELECTED_TARGETS_LOOP,           ///< Loop on only targets in the system the user specified with -p# -c# -n#, etc. if not specified default to 0
  ECMD_SELECTED_TARGETS_LOOP_DEFALL,    ///< Loop on only targets in the system the user specified with -p# -c# -n#, etc. if not specified default to all
  ECMD_SELECTED_TARGETS_LOOP_VD,        ///< Loop only on targets in the system to the depth user specified on command line (ie if user said only '-n0' then -s and below are unused) if not specified default to 0
  ECMD_SELECTED_TARGETS_LOOP_VD_DEFALL, ///< Loop only on targets in the system to the depth user specified on command line (ie if user said only '-n0' then -s and below are unused) if not specified default to all
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
 @brief Used by the GPIO functions to specify the different modes for the GPIO pin

             |             High (1)            |             Low (0)             |
             | Output Enable | Output Register | Output Enable | Output Register |
 Push Pull   |     true      |       1         |     true      |       0         |
 Open Drain  |    false      |       0         |     true      |       0         |
 Open Source |     true      |       1         |    false      |       1         |
*/
typedef enum {
  ECMD_DIO_UNKNOWN,                     ///< Default for not initialized
  ECMD_DIO_INPUT,                       ///< Input pin
  ECMD_DIO_OPEN_DRAIN,                  ///< See detailed table
  ECMD_DIO_OPEN_SOURCE,                 ///< See detailed table
  ECMD_DIO_PUSH_PULL                    ///< See detailed table
} ecmdDioMode_t;


/**
 @brief This is used by ecmdQueryDllInfo to return what environment the dll is designed to run in (i.e Simulation vs Hardware)
*/
typedef enum {
  ECMD_DLL_ENV_UNKNOWN,         ///< Default for not initialized
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
  ECMD_DLL_SCAND,               ///< Running against the ScanD dll owned by Meghna Paruthi
  ECMD_DLL_BML,                 ///< Running against the BML plugin owned by Chris Engel
  ECMD_DLL_MAMBO                ///< Running against the Mambo plugin owned by Chris Engel

} ecmdDllType_t;

/**
 @brief Used for the ecmdQueryFileLocation function to specify the file type you are looking for
*/
typedef enum {
  ECMD_FILE_UNKNOWN,    ///< Default for not initialized
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
  ECMD_SIM_MSG_UNKNOWN,    ///< Default for not initialized
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
  ECMD_SIM_UNKNOWN,    ///< Default for not initialized
  ECMD_SIM_ERROR = 0x1,
  ECMD_SIM_WARNING = 0x2,
  ECMD_SIM_INFO = 0x3,
  ECMD_SIM_PLAIN = ~0
} ecmdFusionSeverity_t;

/**
 @brief Used by ecmdGetGlobalVar to specify what variable you are looking for
*/
typedef enum {
  ECMD_GLOBALVAR_UNKNOWN,    ///< Default for not initialized
  ECMD_GLOBALVAR_DEBUG,         ///< Retrieve the value of the ecmd debug flag set by ECMD_DEBUG env var
  ECMD_GLOBALVAR_QUIETMODE,     ///< Retrieve the value of the quiet mode debug flag = set by -quiet default = 0
  ECMD_GLOBALVAR_COEMODE        //< Retrieve the value of the continueOnError  mode  flag = set by -coe default = 0 (hjhcoe)
} ecmdGlobalVarType_t;

/**
 @brief Used by I2C functions to specify bus speed
*/
typedef enum {
  ECMD_I2C_BUSSPEED_UNKNOWN,    ///< Default for not initialized
  ECMD_I2C_BUSSPEED_50KHZ,      ///< Run I2c bus at 50Khz
  ECMD_I2C_BUSSPEED_100KHZ,     ///< Run I2c bus at 100Khz
  ECMD_I2C_BUSSPEED_400KHZ,     ///< Run I2c bus at 400Khz
} ecmdI2cBusSpeed_t;

/**
 @brief Used by ecmdGpioWriteConfigRegister to specify the type of operation to do
*/
typedef enum {
  ECMD_GPIO_UNKNOWN,            ///< Default for not initialized
  ECMD_GPIO_CONFIG_WRITE,       ///< Set the current value of i_configReg to i_value
  ECMD_GPIO_SET_BIT,            ///< Set each bit of  i_configReg that is true in i_value. 
  ECMD_GPIO_CLEAR_BIT           ///< Clear each bit of i_configReg that is set to true in i_value. 
} ecmdGpioWriteMode_t;

/**
 @brief Used by get/putLatch functions to specify what mode should be used to find latches in the scandef
*/
typedef enum {
  ECMD_LATCHMODE_UNKNOWN,       ///< Default for not initialized
  ECMD_LATCHMODE_FULL,          ///< Latch must match exactly
  ECMD_LATCHMODE_PARTIAL        ///< Latch can be a partial match
} ecmdLatchMode_t;

/**
 @brief Used by readScandef functions to specify what type of latch was found in the scandef
*/
typedef enum {
  ECMD_LATCHTYPE_UNKNOWN,       ///< Default for not initialized
  ECMD_LATCHTYPE_NOBIT,         ///< Latch was specified with out a position, 0 is implied
  ECMD_LATCHTYPE_SINGLEBIT,     ///< Latch was a single bit entry (4)
  ECMD_LATCHTYPE_MULTIBIT,      ///< Latch was multiple bits (3:5)
  ECMD_LATCHTYPE_ARRAY          ///< Latch was an Array Latch (8,28)
} ecmdLatchType_t;


/**
 @brief Used by ecmdQueryConfig to specify detail level of query
*/
typedef enum {
  ECMD_QUERY_DETAIL_UNKNOWN,         ///< Default for not initialized
  ECMD_QUERY_DETAIL_LOW,             ///< Only config info is returned
  ECMD_QUERY_DETAIL_HIGH             ///< All info is returned
} ecmdQueryDetail_t;

/**
 @brief Used for the ecmdQuerySpy function to specify which type of spy we have @see ecmdSpyData
*/
typedef enum {
  ECMD_SPYTYPE_UNKNOWN, ///< Default for not initialized
  ECMD_SPYTYPE_ALIAS,   ///< Spy is an alias
  ECMD_SPYTYPE_IDIAL,   ///< Spy is an iDial
  ECMD_SPYTYPE_EDIAL,   ///< Spy is an eDial
  ECMD_SPYTYPE_ECCGROUP ///< Spy is an eccGrouping
} ecmdSpyType_t;

/**
 @brief Used by ecmdSetTraceMode to specify which trace to control
*/
typedef enum {
  ECMD_TRACE_UNKNOWN,           ///< Default for not initialized
  ECMD_TRACE_SCAN,              ///< Scan Trace
  ECMD_TRACE_PROCEDURE          ///< Procedure Trace
} ecmdTraceType_t;

/**
 @brief Used by ecmdI2CCmdEntry to specify which commands to run
*/
typedef enum {
  ECMD_I2C_UNKNOWN,        ///< Default for not initialized
  ECMD_I2C_RESET,          ///< Call should go ecmdI2cReset
  ECMD_I2C_READ,           ///< Call should go ecmdI2cRead
  ECMD_I2C_READOFFSET,     ///< Call should go ecmdI2cReadOffset
  ECMD_I2C_WRITE,          ///< Call should go ecmdI2cWrite
  ECMD_I2C_WRITEOFFSET     ///< Call should go ecmdI2cWriteOffset
} ecmdI2CCmds_t;

/**
 @brief Used for the ecmdQueryArray function to specify which type of array we have @see ecmdArrayData
*/
typedef enum {
  ECMD_ARRAYTYPE_UNKNOWN,               ///< Default for not initialized
  ECMD_ARRAYTYPE_DIRECT_ACCESS,         ///< Array is a direct access array
  ECMD_ARRAYTYPE_SIMPLE,                ///< Array is a simple array
  ECMD_ARRAYTYPE_HARDWARE_ASSIST,       ///< Array is a hardware assist array
  ECMD_ARRAYTYPE_HARDWARE_ASSIST_2,     ///< Array is a hardware assist array
} ecmdArrayType_t;

/**
 @brief Used by ecmdWriteTarget to specify displayMode
*/
typedef enum {
  ECMD_DISPLAY_TARGET_UNKNOWN,          ///< The unknown state for this enum, not a valid output type
  ECMD_DISPLAY_TARGET_QUERY_PLUGIN,     ///< Query the plugin for which of the below states to use
  ECMD_DISPLAY_TARGET_DEFAULT,          ///< Default mode : 'pu k0:n0:s0:p01:c1' or 'k0:n0'
  ECMD_DISPLAY_TARGET_COMPRESSED,       ///< Compressed : 'k0:n0:s0:pu:p01:c1' or 'k0:n0'
  ECMD_DISPLAY_TARGET_HEX_DEFAULT,      ///< Default mode : '0x[pu k0:n0:s0:p01:c1]' or '0x[k0:n0]'
  ECMD_DISPLAY_TARGET_HEX_COMPRESSED    ///< Compressed : '0x[k0:n0:s0:pu:p01:c1]' or '0x[k0:n0]'
} ecmdTargetDisplayMode_t;

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
  uint32_t    slot;             ///< card slot/fru to target
  std::string chipType;         ///< name of chip to access , either actual or common name
  uint32_t    pos;              ///< position of chip within node
  std::string chipUnitType;     ///< name of the chip unit to access
  // In the perl module, the union construct isn't supported
  // We need to define these variables without the union so that the perl module interface is generated correctly
  // In the swig wrapper code the C union is supported, so when you set core, chipUnitNum is set and vice versa
#if !defined(ECMD_PERLAPI) && !defined(DOCUMENTATION)
  union {
#endif
    /**
     @brief which core on the chip to access, union'd with chipUnitNum.

     The core and chipUnitNum variables are joined together with a C union.  This means if you set core, you are setting chipUnitNum
     and vice versa.  If you are writing code for p6/z6 chips, you want to use core.  If you are writing code for chips that support chipUnits (p7/z7)
     you want to use chipUnitNum.  If you are using chipUnitNum, you also want to set the chipUnitType.
     */
    uint8_t     core;
    /**
     @brief which chip unit on a chip to access, union'd with core

     The core and chipUnitNum variables are joined together with a C union.  This means if you set core, you are setting chipUnitNum
     and vice versa.  If you are writing code for p6/z6 chips, you want to use core.  If you are writing code for chips that support chipUnits (p7/z7)
     you want to use chipUnitNum.  If you are using chipUnitNum, you also want to set the chipUnitType.
     */
    uint8_t     chipUnitNum;
#if !defined(ECMD_PERLAPI) && !defined(DOCUMENTATION)
  };
#endif
  uint8_t     thread;           ///< which thread on chip to access, if chip is multi-threaded
  uint32_t    unitId;		///< This is an optional field if unitid's are used to specify the target, the above info still needs to be filled in
  ecmdChipTargetState_t cageState;            ///< cage field state
  ecmdChipTargetState_t nodeState;            ///< node field state
  ecmdChipTargetState_t slotState;            ///< slot field state
  ecmdChipTargetState_t chipTypeState;        ///< chipType field state
  ecmdChipTargetState_t posState;             ///< pos field state
  ecmdChipTargetState_t chipUnitTypeState;    ///< chipUnitType field state
#ifndef ECMD_PERLAPI
  union {
#endif
    ecmdChipTargetState_t coreState;          ///< core field state
    ecmdChipTargetState_t chipUnitNumState;   ///< chipUnitNum field state
#ifndef ECMD_PERLAPI
  };
#endif
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
chipUnitTypeState(ECMD_TARGET_UNKNOWN_STATE),
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
  bool operator< (const ecmdThreadData& rhs); ///< Used to sort Thread entries in an ecmdThreadData list.
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  uint8_t   threadId;		///< (Detail: Low) Thread number of this entry
  uint32_t  unitId;		///< (Detail: High) Unit Id of this entry
  uint32_t  threadFlags;        ///< (Detail: High) Various additional info about the thread - bitmask of defines
};

#ifndef DOCUMENTATION
// ecmdThreadData Constructor
inline ecmdThreadData::ecmdThreadData():
threadId(0),
unitId(0),
threadFlags(0x0)
{
}

// ecmdThreadData Destructor
inline ecmdThreadData::~ecmdThreadData() { }
#endif

#if 0
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
  bool operator< (const ecmdCoreData& rhs);  ///< Used to sort Core entries in an ecmdCoreData list.
#ifndef REMOVE_SIM
	void printStruct(void);
#endif
#endif

  // Members
  uint8_t                       coreId;         ///< (Detail: Low) core number of this entry
  uint8_t                       numProcThreads; ///< (Detail: Low) Number of threads per core this entry supports - only valid for Processors
  uint32_t                      unitId;         ///< (Detail: High) Unit Id of this entry
  uint32_t                      coreFlags;      ///< (Detail: High) Various additional info about the core - bitmask of defines
  std::list<ecmdThreadData>     threadData;     ///< (Detail: Low) List of all threads avaliable for this chip - only valid for Processor compute cores - in numerical order
};

#ifndef DOCUMENTATION
// ecmdCoreData Constructor
inline ecmdCoreData::ecmdCoreData():
coreId(0),
numProcThreads(0),
unitId(0),
coreFlags(0x0)
{
}

// ecmdCoreData Destructor
inline ecmdCoreData::~ecmdCoreData() { }
#endif
#endif

/**
 @brief Used for the ecmdQueryConfig function to return chipUnit data

 Operators Supported : <

*/
struct ecmdChipUnitData {

#ifndef DOCUMENTATION
  // Constructor
  ecmdChipUnitData();

  // Destructor
  ~ecmdChipUnitData();

  // Methods
#if 0
  uint32_t flatten(uint8_t *o_buf, uint32_t &i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t &i_len);
  uint32_t flattenSize(void);
#endif
  bool operator< (const ecmdChipUnitData& rhs);  ///< Used to sort Core entries in an ecmdChipUnitData list.
#ifndef REMOVE_SIM
#if 0
  void printStruct(void);
#endif
#endif
#endif

  // Members
  std::string                   chipUnitType;   ///< (Detail: Low)  Full name of chipUnit.. i.e. core, mc, etc..
  uint8_t                       chipUnitNum;    ///< (Detail: Low) core number of this entry
  uint8_t                       numThreads;     ///< (Detail: Low) Number of threads this entry supports
  uint32_t                      unitId;         ///< (Detail: High) Unit Id of this entry
  uint32_t                      chipUnitFlags;  ///< (Detail: High) Various additional info about the core - bitmask of defines
  std::list<ecmdThreadData>     threadData;     ///< (Detail: Low) List of all threads avaliable for this chip - only valid for Processor compute cores - in numerical order
};

#ifndef DOCUMENTATION
// ecmdChipUnitData Constructor
inline ecmdChipUnitData::ecmdChipUnitData():
chipUnitNum(0),
numThreads(0),
unitId(0),
chipUnitFlags(0x0)
{
}

// ecmdChipUnitData Destructor
inline ecmdChipUnitData::~ecmdChipUnitData() { }
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

#define ECMD_FLAG_STATEMASK         0x0000001F
#define ECMD_FLAG_CONFIGED_USEABLE  0x00000001
#define ECMD_FLAG_DELAYED_FENCED    0x00000002
#define ECMD_FLAG_FENCED            0x00000004
#define ECMD_FLAG_SPARE             0x00000008
#define ECMD_FLAG_DEFECTIVE         0x00000010

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
  bool operator< (const ecmdChipData& rhs); ///< Used to sort Chip entries (based on Pos) in an ecmdChipData list.
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
  uint32_t                      chipEc;             ///< (Detail: High) EC level of this chip, (ec read from 'jtag' chip id or CFAM id)
  uint32_t                      simModelEc;         ///< (Detail: High) Model EC level of this chip
  ecmdChipInterfaceType_t       interfaceType;      ///< (Detail: High) Interface Macro used by the chip
  uint32_t                      chipFlags;          ///< (Detail: High) Various additional info about the chip - bitmask of defines
  std::list<ecmdChipUnitData>   chipUnitData;       ///< (Detail: Low)  List of all chipUnits requested in this chip - sorted by chipUnitType and chipUnitNum
};

#ifndef DOCUMENTATION
// ecmdChipData Constructor
inline ecmdChipData::ecmdChipData():
pos(0),
unitId(0),
chipEc(0),
simModelEc(0),
interfaceType(ECMD_INTERFACE_UNKNOWN),
chipFlags(0x0)
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
  bool operator< (const ecmdSlotData& rhs); ///< Used to sort Slot entries in an ecmdSlotData list. 
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  uint32_t slotId;                      ///< (Detail: Low) Slot number of this entry
  std::string slotName;                 ///< (Detail: High) The name assigned to this slot
  uint32_t unitId;                      ///< (Detail: High) Unit Id of this entry
  uint32_t slotFlags;                   ///< (Detail: High) Various additional info about the slot - bitmask of defines
  std::list<ecmdChipData> chipData;     ///< (Detail: Low) List of all chips requested in this slot - in order by chipType and pos
};

#ifndef DOCUMENTATION
// ecmdSlotData Constructor
inline ecmdSlotData::ecmdSlotData():
slotId(0),
unitId(0),
slotFlags(0x0)
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
  bool operator< (const ecmdNodeData& rhs); ///< Used to sort Node entries in an ecmdNodeData list. 
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  uint32_t nodeId;                      ///< (Detail: Low) Node number of this entry
  std::string nodeName;                 ///< (Detail: High) The name assigned to this node
  uint32_t unitId;                      ///< (Detail: High) Unit Id of this entry
  uint32_t nodeFlags;                   ///< (Detail: High) Various additional info about the node - bitmask of defines
  std::list<ecmdSlotData> slotData;     ///< (Detail: Low) List of all slots requested in this node - in numerical order by slotId
};

#ifndef DOCUMENTATION
// ecmdNodeData Constructor
inline ecmdNodeData::ecmdNodeData():
nodeId(0),
unitId(0),
nodeFlags(0x0)
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
  bool operator< (const ecmdCageData& rhs);  ///< Used to sort Cage entries in an ecmdCageData list.
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  uint32_t cageId;                      ///< (Detail: Low) Cage number of this entry
  std::string cageName;                 ///< (Detail: High) The name assigned to this cage
  uint32_t unitId;                      ///< (Detail: High) Unit Id of this entry
  uint32_t cageFlags;                   ///< (Detail: High) Various additional info about the cage - bitmask of defines
  std::list<ecmdNodeData> nodeData;     ///< (Detail: Low) List of all nodes requested in this cage - in numerical order by nodeId
};

#ifndef DOCUMENTATION
// ecmdCageData Constructor
inline ecmdCageData::ecmdCageData():
cageId(0),
unitId(0),
cageFlags(0x0)
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
  uint32_t flatten(uint8_t *o_buf, uint32_t i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  std::list<std::string> ringNames;                     ///< (Detail: Low)  Names used to reference this ring
  std::list<uint32_t>    ringIds;                       ///< (Detail: Low)  Hashed ring IDs
  uint32_t               address;                       ///< (Detail: Low)  Address modifier
  int                    bitLength;                     ///< (Detail: Low)  length of ring
  bool                   hasInversionMask;              ///< (Detail: High) Ring has an inversion mask applied before scanning
  bool                   supportsBroadsideLoad;         ///< (Detail: High) This ring supports broadside load in simulation
  bool                   isCheckable;                   ///< (Detail: High) This ring can be run through the check_rings command
  bool                   isChipUnitRelated;             ///< (Detail: Low)  This ring is related to the chipUnit level of a chip
  std::string            relatedChipUnit;               ///< (Detail: Low)  If the ring is chipUnit related, this is the name of the chipUnit
  std::string            clockDomain;                   ///< (Detail: High) Clock domain this ring belongs to
  ecmdClockState_t       clockState;                    ///< (Detail: High) Required clock state to access this ring
};

#ifndef DOCUMENTATION
// ecmdRingData Constructor
inline ecmdRingData::ecmdRingData():
address(0),
bitLength(0),
hasInversionMask(false),
supportsBroadsideLoad(false),
isCheckable(false),
isChipUnitRelated(false),
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
  uint32_t flatten(uint8_t *o_buf, uint32_t i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  std::string           arrayName;              ///< (Detail: Low)  Names used to reference this array
  uint32_t              arrayId;                ///< (Detail: Low)  Hashed array ID
  ecmdArrayType_t       arrayType;              ///< (Detail: Low)  The type of array this is
  uint32_t              readAddressLength;      ///< (Detail: Low)  Bit length of read address
  uint32_t              writeAddressLength;     ///< (Detail: Low)  Bit length of write address
  uint64_t              length;                 ///< (Detail: Low)  Length of array (number of entries)
  uint32_t              width;                  ///< (Detail: Low)  Bit width of array entry
  bool                  isChipUnitRelated;      ///< (Detail: Low)  This array is related to the chipUnit level of a chip
  std::string           relatedChipUnit;        ///< (Detail: Low)  If the array is chipUnit related, this is the name of the chipUnit
  std::string           clockDomain;            ///< (Detail: High) Clock domain this array belongs to
  ecmdClockState_t      clockState;             ///< (Detail: High) Required clock state to access this array
};

#ifndef DOCUMENTATION
// ecmdArrayData Constructor
inline ecmdArrayData::ecmdArrayData():
arrayId(0),
arrayType(ECMD_ARRAYTYPE_UNKNOWN),
readAddressLength(0),
writeAddressLength(0),
length(0),
width(0),
isChipUnitRelated(false),
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
  uint32_t flatten(uint8_t *o_buf, uint32_t i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  std::string      traceArrayName;      ///< (Detail: Low)  Name of Trace array
  uint32_t         traceArrayId;        ///< (Detail: Low)  Hashed trace array ID
  int              length;              ///< (Detail: Low)  Length of trace array (number of entries)
  int              width;               ///< (Detail: Low)  Bit width of trace array entry
  bool             isChipUnitRelated;   ///< (Detail: Low)  This trace array is related to the chipUnit level of a chip
  std::string      relatedChipUnit;     ///< (Detail: Low)  If the trace array is chipUnit related, this is the name of the chipUnit
  std::string      clockDomain;         ///< (Detail: High) Clock domain this array belongs to
  ecmdClockState_t clockState;          ///< (Detail: High) Required clock state to access this array
};

#ifndef DOCUMENTATION
// ecmdTraceArrayData Constructor
inline ecmdTraceArrayData::ecmdTraceArrayData():
traceArrayId(0),
length(0),
width(0),
isChipUnitRelated(false),
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
  uint32_t flatten(uint8_t *o_buf, uint32_t i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  uint32_t         address;             ///< (Detail: Low)  Scom Address
  uint32_t         length;              ///< (Detail: Low)  The length of the scom in bits
  bool             isChipUnitRelated;   ///< (Detail: Low)  This scom is related to the chipUnit level of a chip
  std::string      relatedChipUnit;     ///< (Detail: Low)  If the scom is chipUnit related, this is the name of the chipUnit
  std::string      clockDomain;         ///< (Detail: High) Clock domain this scom belongs to
  ecmdClockState_t clockState;          ///< (Detail: High) Required clock state to access this scom
};

#ifndef DOCUMENTATION
// ecmdScomData Constructor
inline ecmdScomData::ecmdScomData():
address(0),
length(0),
isChipUnitRelated(false),
clockState((ecmdClockState_t) ECMD_CLOCKSTATE_UNKNOWN)
{
}

// ecmdScomData Destructor
inline ecmdScomData::~ecmdScomData() { }
#endif



/**
 @brief Used for the ecmdQueryLatch function to return latch info
*/
struct ecmdLatchData {
#ifndef DOCUMENTATION
  // Constructor
  ecmdLatchData();

  // Destructor
  ~ecmdLatchData();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t i_len);
  uint32_t flattenSize(void) const;
#ifndef REMOVE_SIM
  void printStruct(void) const;
#endif
#endif


  // Members
  std::string      latchName;           ///< (Detail: Low)  Latch Name
  std::string      ringName;            ///< (Detail: Low)  Ring that this latch belongs to
  uint32_t         latchId;             ///< (Detail: Low)  Hashed latch ID
  uint32_t         ringId;              ///< (Detail: Low)  Hashed ring ID
  int              bitLength;           ///< (Detail: Low)  length of latch, sum of all the parts
  bool             isChipUnitRelated;   ///< (Detail: Low)  This latch is related to the chipUnit level of a chip
  std::string      relatedChipUnit;     ///< (Detail: Low)  If the latch is chipUnit related, this is the name of the chipUnit
  std::string      clockDomain;         ///< (Detail: High) Clock domain this latch belongs to
  ecmdClockState_t clockState;          ///< (Detail: High) Required clock state to access this latch
};

#ifndef DOCUMENTATION
// ecmdLatchData Constructor
inline ecmdLatchData::ecmdLatchData():
latchId(0),
ringId(0),
bitLength(0),
isChipUnitRelated(false),
clockState((ecmdClockState_t) ECMD_CLOCKSTATE_UNKNOWN)
{
}

// ecmdLatchData Destructor
inline ecmdLatchData::~ecmdLatchData() { }
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
  uint32_t flatten(uint8_t *o_buf, uint32_t i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t i_len);
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
  uint32_t flatten(uint8_t *o_buf, uint32_t i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t i_len);
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
#ifndef DOCUMENTATION
  // Constructor
  ecmdIndexVectorEntry();

  // Destructor
  ~ecmdIndexVectorEntry();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  int            index;                 ///< Index of entry
  std::vector<ecmdDataBuffer> buffer;   ///< Vector of data to/from entry
  uint32_t       rc;                    ///< Error code in retrieving this entry
};

#ifndef DOCUMENTATION
// ecmdIndexEntry Constructor
inline ecmdIndexVectorEntry::ecmdIndexVectorEntry():
index(0),
rc(0)
{
}

// ecmdIndexEntry Destructor
inline ecmdIndexVectorEntry::~ecmdIndexVectorEntry() { }
#endif


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
  uint32_t flatten(uint8_t *o_buf, uint32_t i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t i_len);
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
  uint32_t flatten(uint8_t *o_buf, uint32_t i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  std::string      latchName;           ///< Latch name of entry
  std::string      ringName;            ///< Ring that latch came from
  ecmdDataBuffer   buffer;              ///< Latch data
  ecmdLatchType_t  latchType;           ///< Specifies what type of latch this is (single bit, multibit, etc..)
  int              latchStartBit;       ///< Start bit of data inside latch
  int              latchEndBit;         ///< End bit of data inside latch
  uint32_t         rc;                  ///< Error code in retrieving this entry
};

#ifndef DOCUMENTATION
// ecmdLatchEntry Constructor
inline ecmdLatchEntry::ecmdLatchEntry():
latchType(ECMD_LATCHTYPE_UNKNOWN),
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
  uint32_t flatten(uint8_t *o_buf, uint32_t i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t i_len);
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
 @brief Used by ecmdSpyData when returning the latches that are part of a spy
*/
struct ecmdSpyLatchData {

  std::string   latchName;   ///< The latch name, including the ()'s
  uint32_t hashKey;          ///< The hashKey for the latchName
  int offset;                ///< The starting bit offset of this latch in the spy
  int length;                ///< The length in bits of this latch
  int lhsNum;                ///< left hand side Number - Value x of an (x:y) entry
  int rhsNum;                ///< right hand side Number - Value y of an (x:y) entry
};


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
  uint32_t flatten(uint8_t *o_buf, uint32_t i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  // Members
  std::string   spyName;                   ///< Names used to reference this spy
  uint32_t      spyId;                     ///< (Detail: Low)  Hashed spy ID
  int           bitLength;                 ///< length of spy
  ecmdSpyType_t spyType;                   ///< Type of spy
  bool          isEccChecked;              ///< This spy affects some ECC groupings
  bool          isEnumerated;              ///< This spy has enumerated values
  bool          isChipUnitRelated;         ///< (Detail: Low)  This spy is related to the chipUnit level of a chip
  std::string   relatedChipUnit;           ///< (Detail: Low)  If the spy is chipUnit related, this is the name of the chipUnit

  std::string           clockDomain;       ///< Clock domain this spy belongs to
  ecmdClockState_t       clockState;       ///< Required clock state to access this spy
  
  std::list<std::string> enums;            ///< Possible enum values for Spy - I/P Can only provide this on a client, not on the FSP
  std::list<std::string> epCheckers;       ///< Possible epChecker names affected by this Spy

  std::list<ecmdSpyLatchData> spyLatches;  ///< The latches that are associated with a spy, not valid with group spies
};

#ifndef DOCUMENTATION
// ecmdSpyData Constructor
inline ecmdSpyData::ecmdSpyData():
spyId(0),
bitLength(0),
isEccChecked(false),
isEnumerated(false),
isChipUnitRelated(false)
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

/**
 @brief Used by ecmdI2CMultipleCmds API
*/
struct ecmdI2CCmdEntry {

#ifndef DOCUMENTATION
  // Constructor
  ecmdI2CCmdEntry();

  // Destructor
  ~ecmdI2CCmdEntry();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t i_len);
  uint32_t flattenSize(void) const;
#ifndef REMOVE_SIM
  void printStruct(void) const;
#endif
#endif

  // Members
  ecmdI2CCmds_t ecmdI2CCmd;     ///< The eCMD API to target with this command
  uint32_t engineId;            ///< The i2c engine to use
  uint32_t port;                ///< The i2c port to use
  uint32_t slaveaddress;        ///< The i2c slave device address to use
  ecmdI2cBusSpeed_t busSpeed;   ///< The i2c bus speed to use
  uint32_t byteOffset;          ///< Byte offset in the device (needed for offset commands only)
  uint32_t offsetFieldSize;     ///< Specifies the field size used in the i2c protocal of the slave device (needed for offset commands only)
  uint32_t readByteLength;      ///< Byte length to read (only needed on reads)
  ecmdDataBuffer data;          ///< The data returned from the i2c operation
};

#ifndef DOCUMENTATION
// ecmdI2CCmdEntry Constructor
inline ecmdI2CCmdEntry::ecmdI2CCmdEntry():
ecmdI2CCmd(ECMD_I2C_UNKNOWN),                //fix beam error
engineId(0),
port(0),
slaveaddress(0),
busSpeed(ECMD_I2C_BUSSPEED_UNKNOWN),         //fix beam error
byteOffset(0),
offsetFieldSize(0),
readByteLength(0)                            //fix beam error
{
}

// ecmdI2CCmdEntry Destructor
inline ecmdI2CCmdEntry::~ecmdI2CCmdEntry() { }
#endif

/**
 @brief Used by simGetModelInfo
*/
struct ecmdSimModelInfo {
#ifndef DOCUMENTATION
  // Constructor
  ecmdSimModelInfo();

  // Destructor
  ~ecmdSimModelInfo();

  // Methods
  uint32_t flatten(uint8_t *o_buf, uint32_t i_len);
  uint32_t unflatten(const uint8_t *i_buf, uint32_t i_len);
  uint32_t flattenSize(void);
#ifndef REMOVE_SIM
  void printStruct(void);
#endif
#endif

  char modelname[255];
  char modeldate[255];
  char modeltime[255];
  char multivalue;  ///<   !=0  multivalue, ==0  2-state
};

//@01a Tell beam to ignore error w/uninitialized memember comment.
#ifndef DOCUMENTATION
// ecmdSimModelInfo Constructor
inline ecmdSimModelInfo::ecmdSimModelInfo()
{
} /*uninitialized member*/

// ecmdSimModelInfo Destructor
inline ecmdSimModelInfo::~ecmdSimModelInfo() { }
#endif

/* @name Misc functions */
//@{

/** @brief Returns the version of the shared lib so it can be compared with the other versions */
std::string ecmdGetSharedLibVersion();

//@}

#endif
