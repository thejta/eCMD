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
#include <string>

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//  Defines                                                
//--------------------------------------------------------------------

#define ECMD_CAPI_VERSION ".1"          ///< eCMD API Version

/**
 @brief This is used by ecmdQueryDllInfo to return who's dll you are actually running against 
*/

typedef enum {
  ECMD_DLL_UNKNOWN,             ///< This should never be encountered
  ECMD_DLL_STUB,                ///< This is a stub version of the dll for client testing
  ECMD_DLL_CRONUS,              ///< Running against the Cronus Dll
  ECMD_DLL_IPSERIES,            ///< Running against I/P Series HOM
  ECMD_DLL_ZSERIES              ///< Running against Z Series HOM

} ecmdDllType_t;

/**
 @brief This is used by ecmdQueryDllInfo to return info to the client about what Dll instance they are actually running with
*/

struct ecmdDllInfo {
  ecmdDllType_t         dllType;        ///< Dll instance type running
};

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
  ECMD_TARGET_QUERY_IGNORE      ///< Query should be limited to data above this field, ignoring data 
} ecmdChipTargetState_t;


/**
 @brief Structure used to designate which cec object/chip you would like the function to operate on

  - The valid bits are used by D/A functions to tell the calling function what level of granularity the function operates on
    Ex.  putmem/getmem display memory through the processor, they are only dependent on cage/node/pos because they do not use
          the cores to perform their function
         However put/getspr display architected registers from the processor, they will signify that cage/node/pos/core and depending
          on the particular spr referenced threads may be valid
  - The valid bits are used slightly differently for the queryFunctions they are used there to signify the
     what data coming in is valid to refine a query
*/
struct ecmdChipTarget {


  uint32_t cage;                ///< cage that contains node with chip
  uint32_t node;                ///< node that contains chip
  std::string chipType;         ///< name of chip to access
  uint32_t pos;                 ///< position of chip within node
  uint32_t core;                ///< which core on chip to access, if chip is multi-core
  uint32_t thread;              ///< which thread on chip to access, if chip is multi-threaded

  ecmdChipTargetState_t cageState;            ///< cage field state
  ecmdChipTargetState_t nodeState;            ///< node field state
  ecmdChipTargetState_t chipTypeState;        ///< chipType field state
  ecmdChipTargetState_t posState;             ///< pos field state
  ecmdChipTargetState_t coreState;            ///< core field state
  ecmdChipTargetState_t threadState;          ///< thread field state
};



/**
 @brief Used in ecmdChipData to describe the interface macro used by the chip
*/
typedef enum {
  ECMD_INTERFACE_ACCESS,                ///< Standard Jtag Rochester Access Macro
  ECMD_INTERFACE_CFAM                   ///< CommonFirmwareAccessMacro - Fsi interface
} ecmdChipInterfaceType_t;

/**
 @brief Used for the ecmdQueryConfig function to return core data
*/
struct ecmdCoreData {
  uint32_t coreId;                      ///< core number of this entry
  uint8_t  numProcThreads;              ///< Number of threads per core this entry supports - only valid for Processors
};

/**
 @brief Used for the ecmdQueryConfig function to return chip data
*/
struct ecmdChipData {
  std::string                   chipType;           ///< name of chip
  uint32_t                      pos;                ///< Position of this entry
  uint8_t                       numProcCores;       ///< Number of cores this entry supports - only valid for Processor compute cores
  uint32_t                      chipEc;             ///< EC level of this chip, usually 0-F (ec read from 'jtag' chip id or CFAM id)
  ecmdChipInterfaceType_t       interfaceType;      ///< Interface Macro used by the chip
  uint32_t                      chipFlags;          ///< Various additional info about the chip - bitmask of defines
  std::list<ecmdCoreData>     coreData;           ///< Vector of all cores requested in this chip - only valid for Processor compute cores - in numerical order by coreId
};

/**
 @brief Used for the ecmdQueryConfig function to return node data
*/
struct ecmdNodeData {
  uint32_t nodeId;                      ///< Node number of this entry
  std::list<ecmdChipData> chipData;   ///< Vector of all chips requested in this node - in numerical order per chip by pos
};

/**
 @brief Used for the ecmdQueryConfig function to return cage data
*/
struct ecmdCageData {
  uint32_t cageId;                      ///< Cage number of this entry
  std::list<ecmdNodeData> nodeData;   ///< Vector of all nodes requested in this cage - in numerical order by nodeId
};


/**
 @brief Used for the ecmdQueryRing function to return ring info
*/
struct ecmdRingData {
  std::list<std::string> ringNames;   ///< Names used to reference this ring
  uint32_t address;                     ///< Address modifier
  int      bitLength;                   ///< length of ring
  bool     hasInversionMask;            ///< Ring has an inversion mask applied before scanning
  bool     supportsBroadsideLoad;       ///< This ring supports broadside load in simulation
  bool     isCheckable;                 ///< This ring can be run through the check_rings command
};

/**
 @brief Used for the ecmdQueryArray function to return array info
*/
struct ecmdArrayData {
  std::string arrayName; ///< Names used to reference this ring
  int length;            ///< length of array
  int width;             ///< width of array
};


/**
 @brief Used for the ecmdQuerySpy function to specify which type of spy we have @see ecmdSpyData
*/
typedef enum {
  ECMD_SPYTYPE_ALIAS,   ///< Spy is an alias
  ECMD_SPYTYPE_IDIAL,   ///< Spy is an iDial
  ECMD_SPYTYPE_EDIAL,   ///< Spy is an eDial
  ECMD_SPYTYPE_ECC      ///< Spy is an eccdef - do we need this ?
} ecmdSpyType_t;

/**
 @brief Used for the ecmdQuerySpy function to return spy info
*/
struct ecmdSpyData {
  std::string spyName;                  ///< Names used to reference this spy
  int bitLength;                        ///< length of spy
  ecmdSpyType_t spyType;                ///< Type of spy
  
  std::list<std::string> enums;       ///< Possible enum values for Spy - I/P Can only provide this on a client, not on the FSP
};

/**
 * @brief Used for storing error messages internally to dll
 */
struct ecmdError {
  int errorCode;        ///< Numeric error code- see ecmdReturnCodes.H
  std::string whom;     ///< Function that registered error
  std::string message;  ///< Message about the error
};


/**
 * @brief Used to store position-selection related info a cmdline user passed in
 */
struct ecmdUserSelectInfo {
  std::string cage;     ///< String containing cage info user passed in with -k option
  std::string node;     ///< String containing node info user passed in with -n option
  std::string pos;      ///< String containing position info user passed in -p option
  std::string core;     ///< String containing core info user passed in with -c option
  std::string thread;   ///< String containing thread info user passed in with -t option
};

/**
 @brief Used for the ecmdQueryFileLocation function to specify the file type you are looking for
*/
typedef enum {
  ECMD_FILE_SCANDEF,    ///< Scandef file type
  ECMD_FILE_SPYDEF,     ///< Spy Definition file
  ECMD_FILE_ARRAYDEF,   ///< Array Definition file
  ECMD_FILE_HELPTEXT    ///< eCMD Help Text file
} ecmdFileType_t;




#endif
