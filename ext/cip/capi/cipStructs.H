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


#define ECMD_CIP_CAPI_VERSION "1.1d"          ///< eCMD CIP Extension version

/**
 @brief Used by setBreakpoint to specify what type of breakpoint to set
*/
typedef enum {
  ECMD_BREAKPOINT_IABR,         ///< Instruction Address Breakpoint
  ECMD_BREAKPOINT_DABR,         ///< Data Address Breakpoint
  ECMD_BREAKPOINT_CIABR         ///< ?? Breakpoint
} ecmdBreakpointType_t;



#endif /* cipStructs_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
