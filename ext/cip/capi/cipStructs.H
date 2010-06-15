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
#define CIP_PORE_UNKNOWN            0x00000000    ///< 
#define CIP_PORE_APPEND             0x00000001    ///< Append PORE instruction in image (add instruction)
#define CIP_PORE_REPLACE            0x00000002    ///< Replace PORE instruction in image (search and replace instruction)
#define CIP_PORE_NOOP               0x00000004    ///< No-op all existing scom operations if found
#define CIP_PORE_OVERLAY_OR         0x00000008    ///< Overlay PORE instruction onto existing by bitwise OR if found
#define CIP_PORE_OVERLAY_AND        0x00000010    ///< Overlay PORE instruction onto existing by bitwise AND if found
#define CIP_PORE_SCANINIT_SECTION   0x00000020    ///< Add PORE instruction to SCANINIT section of the image
#define CIP_PORE_SCOMINIT_SECTION   0x00000040    ///< Add PORE instruction to SCOMINIT section of the image

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
#endif /* cipStructs_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
