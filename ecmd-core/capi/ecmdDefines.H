//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2003,2016 IBM International Business Machines Corp.
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

#ifndef ecmdDefines_H 
#define ecmdDefines_H

/**
 * @file ecmdDefines.H
 * @brief Provides a common place for all compiletime defines
 * NOTE : None of these defines are enabled by default
 * They are only intended to be used in embeded, etc.. environments to reduce size
*/


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

/* Removes Sensor interfaces from api and cmdline code */
//#define ECMD_REMOVE_SENSOR_FUNCTIONS



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
/*  These Compile Flags are used for Specific ECMD Extensions            */
/*************************************************************************/

#endif
