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
/* $Header$ */

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
 @brief eCMD Command Interpreter function prototypes
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
 
uint32_t ecmdCommandInterpreter(int argc, char* argv[]);



/** @name User Functions */
//@{

/* ecmdRingUser.C */
uint32_t ecmdCheckRingsUser(int argc, char* argv[]);
uint32_t ecmdGetBitsUser(int argc, char* argv[]);
uint32_t ecmdGetLatchUser(int argc, char* argv[]);
uint32_t ecmdGetRingDumpUser(int argc, char* argv[]);
uint32_t ecmdPutBitsUser(int argc, char* argv[]);
uint32_t ecmdPutLatchUser(int argc, char* argv[]);
uint32_t ecmdPutPatternUser(int argc, char* argv[]);

/* ecmdJtagUser.C */
uint32_t ecmdSendCmdUser(int argc, char* argv[]);

/* ecmdArrayUser.C */
uint32_t ecmdGetArrayUser(int argc, char* argv[]);
uint32_t ecmdPutArrayUser(int argc, char* argv[]);

/* ecmdScomUser.C */
uint32_t ecmdGetScomUser(int argc, char* argv[]);
uint32_t ecmdPutScomUser(int argc, char* argv[]);
uint32_t ecmdPollScomUser(int argc, char* argv[]);

/* ecmdSpyUser.C */
uint32_t ecmdGetSpyUser(int argc, char* argv[]);
uint32_t ecmdPutSpyUser(int argc, char* argv[]);

/* ecmdQueryUser.C */
uint32_t ecmdQueryUser(int argc, char* argv[]);

/* ecmdSimUser.C */
#ifndef REMOVE_SIM
uint32_t ecmdSimaetUser(int argc, char * argv[]);
uint32_t ecmdSimcheckpointUser(int argc, char * argv[]);
uint32_t ecmdSimclockUser(int argc, char * argv[]);
uint32_t ecmdSimechoUser(int argc, char * argv[]);
uint32_t ecmdSimexitUser(int argc, char * argv[]);
uint32_t ecmdSimEXPECTFACUser(int argc, char * argv[]);
uint32_t ecmdSimexpecttcfacUser(int argc, char * argv[]);
uint32_t ecmdSimGETFACUser(int argc, char * argv[]);
uint32_t ecmdSimGETFACXUser(int argc, char * argv[]);
uint32_t ecmdSimgettcfacUser(int argc, char * argv[]);
uint32_t ecmdSimgetcurrentcycleUser(int argc, char * argv[]);
uint32_t ecmdSiminitUser(int argc, char * argv[]);
uint32_t ecmdSimPUTFACUser(int argc, char * argv[]);
uint32_t ecmdSimPUTFACXUser(int argc, char * argv[]);
uint32_t ecmdSimputtcfacUser(int argc, char * argv[]);
uint32_t ecmdSimrestartUser(int argc, char * argv[]);
uint32_t ecmdSimSTKFACUser(int argc, char * argv[]);
uint32_t ecmdSimstktcfacUser(int argc, char * argv[]);
uint32_t ecmdSimSUBCMDUser(int argc, char * argv[]);
uint32_t ecmdSimTckIntervalUser(int argc, char * argv[]);
uint32_t ecmdSimUNSTICKUser(int argc, char * argv[]);
uint32_t ecmdSimunsticktcfacUser(int argc, char * argv[]);
#endif  //REMOVE_SIM

//@}



#endif /* ecmdInterpreter_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
