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

#ifndef cipInterpreter_h
#define cipInterpreter_h


/**
 @file cipInterpreter.H
 @brief eCMD CIP Extension Command Interpreter function prototypes
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <inttypes.h>


#define ECMD_MAX_DATA_BITS        800000 
//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------
/**
 @brief Process user args and execute command requested
 @param argc Command line arguments
 @param argv Command line arguments
 @retval ECMD_SUCCESS On successful command
 @retval ECMD_INT_UNKNOWN_COMMAND If command is not valid
 @retval nonzero On command failure
 @pre Dll must be loaded - if applicable

 NOTE - should only be called for cip extension functions
*/
 
uint32_t cipCommandInterpreter(int argc, char* argv[]);



/** @name User Functions */
//@{

/* cipProcUser.C */
#ifndef CIP_REMOVE_INSTRUCTION_FUNCTIONS
uint32_t cipInstructUser(int argc, char* argv[]);
#endif // CIP_REMOVE_INSTRUCTION_FUNCTIONS

#ifndef CIP_REMOVE_BREAKPOINT_FUNCTIONS
uint32_t cipBreakpointUser(int argc, char* argv[]);
#endif // CIP_REMOVE_BREAKPOINT_FUNCTIONS

#ifndef CIP_REMOVE_VR_FUNCTIONS
uint32_t cipGetVrUser(int argc, char * argv[]);
uint32_t cipPutVrUser(int argc, char * argv[]);
#endif // CIP_REMOVE_VR_FUNCTIONS

#ifndef CIP_REMOVE_MEMORY_FUNCTIONS
uint32_t ecmdCipGetMemProcUser(int argc, char * argv[]);
uint32_t ecmdCipPutMemProcUser(int argc, char * argv[]);
uint32_t cipGetMemProcVarUser(int argc, char * argv[]);
uint32_t cipPutMemProcVarUser(int argc, char * argv[]);
#endif // CIP_REMOVE_MEMORY_FUNCTIONS

#ifndef CIP_REMOVE_VSR_FUNCTIONS
uint32_t cipGetVsrUser(int argc, char * argv[]);
uint32_t cipPutVsrUser(int argc, char * argv[]);
#endif // CIP_REMOVE_VSR_FUNCTIONS

/* cipPoreUser.C */
#ifndef CIP_REMOVE_PORE_FUNCTIONS
uint32_t cipPorePutScomUser(int argc, char * argv[]);
uint32_t cipPorePutSpyUser(int argc, char * argv[]);
uint32_t cipPorePutSprUser(int argc, char * argv[]);
uint32_t cipPoreQueryImageUser(int argc, char * argv[]);
uint32_t cipPoreLoadImageUser(int argc, char * argv[]);
#endif // CIP_REMOVE_PORE_FUNCTIONS

/* cipProcUser.C */
#ifndef CIP_REMOVE_RW_FUNCTIONS
uint32_t cipRWReadCacheUser(int argc, char * argv[]);
uint32_t cipRWReadTLBUser(int argc, char * argv[]);
uint32_t cipRWReadMemUser(int argc, char * argv[]);
uint32_t cipRWWriteMemUser(int argc, char * argv[]);
uint32_t cipRWGetDcrUser(int argc, char * argv[]);
uint32_t cipRWPutDcrUser(int argc, char * argv[]);
uint32_t cipRWProcStatusUser(int argc, char * argv[]);
#endif // CIP_REMOVE_RW_FUNCTIONS

/* cipProcUser.C */
#ifndef CIP_REMOVE_PMC_VOLTAGE_FUNCTIONS
uint32_t cipGetPmcVoltageUser(int argc, char * argv[]);
uint32_t cipPutPmcVoltageUser(int argc, char * argv[]);
#endif // CIP_REMOVE_PMC_VOLTAGE_FUNCTIONS

//@}

#endif /* cipInterpreter_h */
