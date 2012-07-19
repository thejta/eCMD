#ifndef fapiInterpreter_h
#define fapiInterpreter_h
/* $Header$ */
// Copyright **********************************************************
//                                                                      
// File fapiInterpreter.H                                               
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
 @file fapiInterpreter.H
 @brief eCMD Fapi Extension Command Interpreter function prototypes
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

 NOTE - should only be called for Fapi extension functions
*/
 
uint32_t fapiCommandInterpreter(int argc, char* argv[]);



/** @name User Functions */
//@{

/* fapiMiscUser.C */
uint32_t fapiGetAttributeUser(int argc, char * argv[]);


//@}

#endif /* fapiInterpreter_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
