#ifndef ecmdInterpreter_h
#define ecmdInterpreter_h
// Copyright **********************************************************
//                                                                      
// File ecmdInterpreter.H                                               
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

// Class Description *************************************************
//                                                                      
//  Name:  ecmdInterpreter
//  Base class: 
//
//  Description: 
//  Usage:
//
// End Class Description *********************************************

/**
 @file ecmdInterpreter.H
 @brief eCMD Command Interpreter
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------

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
*/
 
int ecmdCommandInterpreter(int argc, char* argv[]);



/** @name User Functions */
//@{

int ecmdGetScomUser(int argc, char* argv[]);

int ecmdGetBitsUser(int argc, char* argv[]);

int ecmdGetLatchUser(int argc, char* argv[]);

int ecmdGetRingDumpUser(int argc, char* argv[]);

int ecmdPutBitsUser(int argc, char* argv[]);

int ecmdPutLatchUser(int argc, char* argv[]);

int ecmdPutScomUser(int argc, char* argv[]);

int ecmdPollScomUser(int argc, char* argv[]);

int ecmdQueryUser(int argc, char* argv[]);

//@}



#endif /* ecmdInterpreter_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
