#ifndef ecmdDefines_H 
#define ecmdDefines_H
/* $Header$ */
// Copyright **********************************************************
//                                                                      
// File ecmdDefines.H                                               
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

/**
 * @file ecmdDefines.H
 * @brief Provides a common place for all compiletime defines
 * NOTE : None of these defines are enabled in the CTE common environment, they are only intended to be
 *   used in a closed environment ZSE,FSP, etc
*/


/* Include this file to determine which extensions are defined */
/* This WILL NOT be done in the common code */
//#include <ecmdPluginExtensionSupport.H>


/*************************************************************************/
/*  These are the Common Code Standard Compile Flags                     */
/*************************************************************************/

/* Removes extra debug/trace information */
//#define ECMD_STRIP_DEBUG

/* Makes api call's static function calls instead of dll symbol lookups, used for building the client
   and plugin together in one application */
//#define ECMD_STATIC_FUNCTIONS

/* Defined when building the eCMD Perlapi, can be used to remove functions from the perl module */
//#define ECMD_PERLAPI

/* Defined when generating doxygen documentation, can be used to removed functions from 
   public documentation */
//#define DOCUMENTATION

/* Used to enable mpatrol memory debugging */
//#define ENABLE_MPATROL

/* Used to enable calling MCP functions directly */
//#define ECMD_USE_MCP

/*************************************************************************/
/*  These Compile Flags are used to remove functions from Common Code    */
/*   CAPI & cmdline                                                      */
/*************************************************************************/

/* Removes simulation interfaces from api and ecmdDataBuffer */
//#define REMOVE_SIM

/* Removes SEDC functions and files from api and cmdline code */
//#define ECMD_REMOVE_SEDC_SUPPORT

/* Removes Ring interfaces from api and cmdline code */
//#define ECMD_REMOVE_RING_FUNCTIONS

/* Removes Latch interfaces from api and cmdline code */
//#define ECMD_REMOVE_LATCH_FUNCTIONS

/* Removes Spy  interfaces from api and cmdline code */
//#define ECMD_REMOVE_SPY_FUNCTIONS

/* Removes Array interfaces from api and cmdline code */
//#define ECMD_REMOVE_ARRAY_FUNCTIONS

/* Removes Clock interfaces from api and cmdline code */
//#define ECMD_REMOVE_CLOCK_FUNCTIONS

/* Removes Processor interfaces from api and cmdline code */
//#define ECMD_REMOVE_PROCESSOR_FUNCTIONS

/* Removes RefClock interfaces from api and cmdline code */
//#define ECMD_REMOVE_REFCLOCK_FUNCTIONS

/* Removes Trace Array interfaces from api and cmdline code */
//#define ECMD_REMOVE_TRACEARRAY_FUNCTIONS

/* Removes Memory interfaces from api and cmdline code */
//#define ECMD_REMOVE_MEMORY_FUNCTIONS

/* Removes VPD interfaces from api and cmdline code */
//#define ECMD_REMOVE_VPD_FUNCTIONS

/* Removes I2C interfaces from api and cmdline code */
//#define ECMD_REMOVE_I2C_FUNCTIONS

/* Removes GPIO interfaces from api and cmdline code */
//#define ECMD_REMOVE_GPIO_FUNCTIONS

/* Removes Power interfaces from api and cmdline code */
//#define ECMD_REMOVE_POWER_FUNCTIONS

/* Removes Adal interfaces from api and cmdline code */
//#define ECMD_REMOVE_ADAL_FUNCTIONS



/*************************************************************************/
/*  These Compile Flags are used to remove functions from CIP Extension  */
/*   CAPI & cmdline                                                      */
/*************************************************************************/
/* The CIP flags should only be used if ECMD_CIP_EXTENSION SUPPORT it active */
//#ifdef ECMD_CIP_EXTENSION_SUPPORT

/* Removes Instruction interfaces from CIP api and cmdline code */
//#define CIP_REMOVE_INSTRUCTION_FUNCTIONS

/* Removes Breakpoint interfaces from CIP api and cmdline code */
//#define CIP_REMOVE_BREAKPOINT_FUNCTIONS

/* Removes VR interfaces from CIP api and cmdline code */
//#define CIP_REMOVE_VR_FUNCTIONS

/* Removes Memory interfaces from CIP api and cmdline code */
//#define CIP_REMOVE_MEMORY_FUNCTIONS

//#endif // ECMD_CIP_EXTENSION_SUPPORT


/*************************************************************************/
/*  These Compile Flags are used to remove functions from GIP Extension  */
/*   CAPI & cmdline                                                      */
/*************************************************************************/
/* The GIP flags should only be used if ECMD_GIP_EXTENSION SUPPORT it active */
//#ifdef ECMD_GIP_EXTENSION_SUPPORT

/* Removes Memory interfaces from GIP api and cmdline code */
//#define GIP_REMOVE_MEMORY_FUNCTIONS

/* Removes Breakpoint interfaces from GIP api and cmdline code */
//#define GIP_REMOVE_BREAKPOINT_FUNCTIONS

//#endif  // ECMD_GIP_EXTENSION_SUPPORT

/*************************************************************************/
/*  These Compile Flags are used for Specific ECMD Extensions            */
/*************************************************************************/


/* Turned on only in a GIP FSP build environment */
//#define FIPSODE

/* Turned on only in ZSE build environment */
//#define ZSERIES_SWITCH

/* Turned on in a BML Build environment only */
//#define BML

#endif
