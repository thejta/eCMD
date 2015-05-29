#ifndef cipStructs_h
#define cipStructs_h
// Copyright **********************************************************
//                                                                      
// File cipStructs.H                                               
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 1996                                         
//                                                                      
// The source code for this program is not published or otherwise       
// divested of its trade secrets, irrespective of what has been         
// deposited with the U.S. Copyright Office.                             
//                                                                      
// End Copyright ******************************************************


/**
 * @file cipStructs.H
 * @brief Cronus & IP eCMD Extension Structures

 * Extension Owner : Chris Engel
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------


//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------


#define ECMD_CIP_CAPI_VERSION "1.1"          ///< eCMD CIP Extension version


/* Defines for the various PORE puts */
#define CIP_PORE_OP_MASK            0x000000FF    ///< Mask off just the operation type
#define CIP_PORE_LOC_MASK           0x0000FF00    ///< Mask off just the image location

/* Commands for PORE puts */
#define CIP_PORE_UNKNOWN            0x00000000    ///< 
#define CIP_PORE_APPEND             0x00000001    ///< Append PORE instruction in image (add instruction)
#define CIP_PORE_REPLACE            0x00000002    ///< Replace PORE instruction in image (search and replace instruction)
#define CIP_PORE_NOOP               0x00000004    ///< No-op all existing scom operations if found
#define CIP_PORE_OVERLAY_OR         0x00000008    ///< Overlay PORE instruction onto existing by bitwise OR if found
#define CIP_PORE_OVERLAY_AND        0x00000010    ///< Overlay PORE instruction onto existing by bitwise AND if found

/* Options for PORE puts */
#define CIP_PORE_IMAGE_ONLY         0x00010000    ///< Update the image only, do not do operations to hardware

/* Locations for PORE puts */
#define CIP_PORE_UNKNOWN_SECTION            0x00000000 ///<
#define CIP_PORE_SCANINIT_SECTION           0x00000100 ///< Add PORE instruction to SCANINIT section of the image
#define CIP_PORE_SCANINIT_SCOM_EX_SECTION   0x00000100 ///< Alias for CIP_PORE_SCANINIT_SECTION
#define CIP_PORE_SCOMINIT_SECTION           0x00000200 ///< Add PORE instruction to SCOMINIT section of the image, software will chose proper scom section
#define CIP_PORE_SCOMINIT_SCOM_EX_SECTION   0x00000300 ///< Add PORE instruction to SCOMINIT SCOM EX section of the image
#define CIP_PORE_SCOMINIT_SCOM_CORE_SECTION 0x00000400 ///< Add PORE instruction to SCOMINIT SCOM CORE section of the image

#define CIP_PORE_FSP_SCOM_EX_SECTION        0x00000500 ///< Add PORE instruction to FSP SCOM EX section of the image
#define CIP_PORE_FSP_SCOM_CORE_SECTION      0x00000600 ///< Add PORE instruction to FSP SCOM CORE section of the image
#define CIP_PORE_FSP_SCOM_L2DEL_SECTION     0x00000700 ///< Add PORE instruction to FSP SCOM L2DEL section of the image
#define CIP_PORE_FSP_SCOM_L3DEL_SECTION     0x00000800 ///< Add PORE instruction to FSP SCOM L3DEL section of the image

#define CIP_PORE_PHYP_SCOM_EX_SECTION       0x00000900 ///< Add PORE instruction to PHYP SCOM EX section of the image
#define CIP_PORE_PHYP_SCOM_CORE_SECTION     0x00000A00 ///< Add PORE instruction to PHYP SCOM CORE section of the image

#define CIP_PORE_TPMD_SCOM_EX_SECTION       0x00000B00 ///< Add PORE instruction to TPMD SCOM EX section of the image
#define CIP_PORE_TPMD_SCOM_CORE_SECTION     0x00000C00 ///< Add PORE instruction to TPMD SCOM CORE section of the image

#define CIP_PORE_SCOM_CPM_MODE0_SECTION     0x00001000 ///< Add PORE instruction to SCOM CPM MODE0 section of the image
#define CIP_PORE_SCOM_CPM_MODE1_SECTION     0x00001100 ///< Add PORE instruction to SCOM CPM MODE1 section of the image
#define CIP_PORE_SCOM_CPM_MODE2_SECTION     0x00001200 ///< Add PORE instruction to SCOM CPM MODE2 section of the image
#define CIP_PORE_SCOM_CPM_MODE3_SECTION     0x00001300 ///< Add PORE instruction to SCOM CPM MODE3 section of the image
#define CIP_PORE_SCOM_CPM_MODE4_SECTION     0x00001400 ///< Add PORE instruction to SCOM CPM MODE4 section of the image
#define CIP_PORE_SCOM_CPM_MODE5_SECTION     0x00001500 ///< Add PORE instruction to SCOM CPM MODE5 section of the image
#define CIP_PORE_SCOM_CPM_MODE6_SECTION     0x00001600 ///< Add PORE instruction to SCOM CPM MODE6 section of the image
#define CIP_PORE_SCOM_CPM_MODE7_SECTION     0x00001700 ///< Add PORE instruction to SCOM CPM MODE7 section of the image
#define CIP_PORE_SCOM_CPM_MODE8_SECTION     0x00001800 ///< Add PORE instruction to SCOM CPM MODE8 section of the image
#define CIP_PORE_SCOM_CPM_MODE9_SECTION     0x00001900 ///< Add PORE instruction to SCOM CPM MODE9 section of the image
#define CIP_PORE_SCOM_CPM_MODEA_SECTION     0x00001A00 ///< Add PORE instruction to SCOM CPM MODEA section of the image
#define CIP_PORE_SCOM_CPM_MODEB_SECTION     0x00001B00 ///< Add PORE instruction to SCOM CPM MODEB section of the image
#define CIP_PORE_SCOM_CPM_MODEC_SECTION     0x00001C00 ///< Add PORE instruction to SCOM CPM MODEC section of the image
#define CIP_PORE_SCOM_CPM_MODED_SECTION     0x00001D00 ///< Add PORE instruction to SCOM CPM MODED section of the image
#define CIP_PORE_SCOM_CPM_MODEE_SECTION     0x00001E00 ///< Add PORE instruction to SCOM CPM MODEE section of the image
#define CIP_PORE_SCOM_CPM_MODEF_SECTION     0x00001F00 ///< Add PORE instruction to SCOM CPM MODEF section of the image

#define CIP_PBA_MODE_LCO    0x00000001  ///< Use LCO mode with cipPutMemPba
#define CIP_PBA_MODE_INJECT 0x00000002  ///< Use Inject mode with cipPutMemPba
#define CIP_PBA_MODE_DMA    0x00000003  ///< Use DMA mode with cipPutMemPba

#define CIP_PMC_VOLTAGE_MODE_IGNORE  0x00000000
#define CIP_PMC_VOLTAGE_MODE_VID     0x00000001
#define CIP_PMC_VOLTAGE_MODE_VOLT    0x00000002

/**
 @brief Used by setBreakpoint to specify what type of breakpoint to set
*/
typedef enum {
  CIP_BREAKPOINT_UNKNOWN,      ///< Default
  CIP_BREAKPOINT_IABR,         ///< Instruction Address Breakpoint
  CIP_BREAKPOINT_DABR,         ///< Data Address Breakpoint
  CIP_BREAKPOINT_CIABR         ///< ?? Breakpoint
} ecmdBreakpointType_t;

/**
  @brief Used by sendMboxMsg function to specify message types
  */
typedef enum {
    CIP_MBOX_MSG_UNKNOWN,              ///< Unknown operation
    CIP_MBOX_MSG_STOP,                 ///< Stop message
    CIP_MBOX_MSG_RESUME                ///< Resume message
} mbox_message_t;



/**
 @brief Used by cipPoreQueryImage functions to return image info
*/
struct cipPoreImageInfo {
    uint32_t version;       ///< Version number of image layout - 0xAAbb, AA is major version and bb is minor version
    uint32_t build_date;    ///< Image build date - YYYYMMDD, e.g. Jan 16, 2011 = 20110116 
    uint32_t build_time;    ///< Image build time - 00HHMMSS, e.g. 15:43:17 DST = 00154317
    std::string builder;    ///< Image builder name
    uint32_t load_date;     ///< Image load date - YYYYMMDD, e.g. Jan 16, 2011 = 20110116
    uint32_t load_time;     ///< Image load time - 00HHMMSS, e.g. 15:43:17 DST = 00154317
    uint64_t base_address;  ///< Memory location where image is loaded or going to be loaded
};

/**
 @brief Used by cipRWReadCache function to return cache record data
*/
struct cipRWCacheRec
{
   uint32_t tag;      // data/instr tag
   uint32_t data[32]; // data/instr data
   uint8_t  valid;    // data/instr valid bit
   uint8_t  lru;      // data/instr LRU bit
   uint8_t  dirty;    // data dirty bits
};

/**
 @brief Used by cipRWReadTLB function to return TLB record data
*/
struct cipRWTLBRec
{
   uint32_t epn;      // Effective Page Number
   uint32_t rpn;      // Real Page Number
   uint8_t  size;     // page size
   uint8_t  ebit;     // Endian bit
   uint8_t  kbit;     // Compressed bit
   uint8_t  valid;    // TLB valid bit
   uint8_t  tid;      // TLB id #
   uint8_t  par[2];   // parity data for each word read
   uint8_t  ex;       // EXecute bit
   uint8_t  wr;       // WRite bit
   uint8_t  zsel;     // Zone Select
   uint8_t  wimg;     // WIMG storage attributes bits
};

/**
 @brief Used by cipRWProcStatus to return status
*/
typedef enum {
  CIP_RW_PROC_STATUS_UNKNOWN,
  CIP_RW_PROC_STATUS_RUNNING,
  CIP_RW_PROC_STATUS_STOPPED,
  CIP_RW_PROC_STATUS_HALTED,
  CIP_RW_PROC_STATUS_CHECKSTOPPED,
  CIP_RW_PROC_STATUS_POWEROFF,
} cipRWProcStatus_t;

enum CIP_PROCESSOR_FAMILY
{
  CIP_PROCESSOR_FAMILY_P8 = 0x05, 
  CIP_PROCESSOR_FAMILY_P9 = 0x06, 
  CIP_PROCESSOR_FAMILY_RESERVED = 0x0F,
  CIP_PROCESSOR_FAMILY_INVALID = 0xFF,     // Uninitialized
};


struct cipSysInfo_t
{
//   uint32_t flatten(uint8_t *o_buf, uint32_t &i_len);
//   uint32_t unflatten(const uint8_t *i_buf, uint32_t &i_len);
//   uint32_t flattenSize(void);
//   void printStruct();

   // CEC info
   uint32_t max_cores_per_chip; ///< Maximum Number of Cores per Processor Chip
   uint32_t max_threads_per_core; ///< Maximum Number of Threads per Core
//   GIP_SYSINFO_BOX_TYPE   boxType; ///< System/Box Type
//   ecmdChipTarget primaryThread; ///< Primary Thread of Chip On the System
   CIP_PROCESSOR_FAMILY  procFamily; ///< Type of Processor In The System
//   GIP_CLK_STATE procClkState; ///< Clock Domain States of Processors
//   GIP_CLK_STATE asicClkState; ///< Clock Domain States of Asic Chips
//   GIP_MACHINE_SERIES_TYPE seriesType;  ///< Indicates i vs p-series
//   bool hasOlcRcvd;           ///< Has OLC been received?
//   uint32_t attnErrorState; // Value set to attn/AhTerm registry variable
//   char ecmd_capi_version[16]; ///< ECMD_CAPI_VERSION In "Major.Minor" format (ex "10.4")
//   char gip_capi_version[16]; ///< ECMD_GIP_CAPI_VERSION In "Major.Minor" format (ex "1.0")
//   uint32_t gip_server_client_version; ///< ECMD_GIP_SERVER_CLIENT_VERSION in 0xMMmm format
};

#endif /* cipStructs_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
