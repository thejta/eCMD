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
uint32_t cipInstructUser(int argc, char* argv[]);
uint32_t cipBreakpointUser(int argc, char* argv[]);
uint32_t cipGetVprUser(int argc, char * argv[]);
uint32_t cipPutVprUser(int argc, char * argv[]);

//@}

#endif /* cipInterpreter_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
