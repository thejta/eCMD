#ifndef cipInterpreter_h
#define cipInterpreter_h
// Copyright **********************************************************
//                                                                      
// File cipInterpreter.H                                               
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

/* $Header$ */

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
#endif // CIP_REMOVE_RW_FUNCTIONS

//@}

#endif /* cipInterpreter_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
