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
#define ECMD_MAX_DATA_BITS        800000          ///< Maximum number of bits to allow as input/output for reasonable bounds/error checking

/**
@brief An enum to tell us what path to call through in some command line functions
*/
typedef enum {  
  ECMD_MEM_DMA,
  ECMD_MEM_MEMCTRL,
  ECMD_MEM_PROC,
  ECMD_GPR,
  ECMD_FPR
} ECMD_DA_TYPE;


/**
 @brief Walk through all interpreters, including the extensions
 @param argc Command line arguments
 @param argv Command line arguments
 @retval ECMD_SUCCESS On successful command
 @retval ECMD_INT_UNKNOWN_COMMAND If command is not valid
 @retval nonzero On command failure
*/
uint32_t ecmdCallInterpreters(int argc, char* argv[]);

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
uint32_t ecmdRingCacheUser(int argc, char* argv[]);
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
uint32_t ecmdGetTraceArrayUser(int argc, char* argv[]);
uint32_t ecmdPutArrayUser(int argc, char* argv[]);

/* ecmdScomUser.C */
uint32_t ecmdGetScomUser(int argc, char* argv[]);
uint32_t ecmdPutScomUser(int argc, char* argv[]);
uint32_t ecmdPollScomUser(int argc, char* argv[]);

/* ecmdSpyUser.C */
uint32_t ecmdGetSpyUser(int argc, char* argv[]);
uint32_t ecmdPutSpyUser(int argc, char* argv[]);

/* ecmdMemUser.C */
uint32_t ecmdCacheFlushUser(int argc, char* argv[]);
uint32_t ecmdGetMemUser(int argc, char* argv[], ECMD_DA_TYPE memMode);
uint32_t ecmdPutMemUser(int argc, char* argv[], ECMD_DA_TYPE memMode);

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
uint32_t ecmdSimOutputFusionMessageUser(int argc, char * argv[]);
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
uint32_t ecmdSimGetHierarchyUser(int argc, char * argv[]);
#endif  //REMOVE_SIM

/* ecmdProcUser.C */
uint32_t ecmdGetSprUser(int argc, char * argv[]);
uint32_t ecmdPutSprUser(int argc, char * argv[]);
uint32_t ecmdGetGprFprUser(int argc, char * argv[], ECMD_DA_TYPE daType);
uint32_t ecmdPutGprFprUser(int argc, char * argv[], ECMD_DA_TYPE daType);


/* ecmdIstepUser.C */
uint32_t ecmdIstepUser(int argc, char * argv[]);
uint32_t ecmdSetClockSpeedUser(int argc, char * argv[]);
uint32_t ecmdGetClockSpeedUser(int argc, char * argv[]);
uint32_t ecmdStartClocksUser(int argc, char * argv[]);
uint32_t ecmdStopClocksUser(int argc, char * argv[]);
uint32_t ecmdSystemPowerUser(int argc, char * argv[]);

/* ecmdMiscUser.C */
uint32_t ecmdDeconfigUser(int argc, char * argv[]);
uint32_t ecmdReconfigUser(int argc, char * argv[]);
uint32_t ecmdGetConfigUser(int argc, char * argv[]);
uint32_t ecmdSetConfigUser(int argc, char * argv[]);
uint32_t ecmdGetCfamUser(int argc, char* argv[]);
uint32_t ecmdMakeSPSystemCallUser(int argc, char* argv[]);
uint32_t ecmdPutCfamUser(int argc, char* argv[]);
uint32_t ecmdGetGpRegisterUser(int argc, char* argv[]);
uint32_t ecmdPutGpRegisterUser(int argc, char* argv[]);

/* ecmdVpdUser.C */
uint32_t ecmdPutVpdKeywordUser(int argc, char* argv[]);
uint32_t ecmdGetVpdKeywordUser(int argc, char* argv[]);
uint32_t ecmdPutVpdImageUser(int argc, char* argv[]);
uint32_t ecmdGetVpdImageUser(int argc, char* argv[]);

/* ecmdI2cGpioUser.C */
uint32_t ecmdPutI2cUser(int argc, char* argv[]);
uint32_t ecmdGetI2cUser(int argc, char* argv[]);
uint32_t ecmdI2cResetUser(int argc, char* argv[]);
uint32_t ecmdGetGpioPinUser(int argc, char* argv[]);
uint32_t ecmdGetGpioLatchUser(int argc, char* argv[]);
uint32_t ecmdPutGpioLatchUser(int argc, char* argv[]);
uint32_t ecmdGpioConfigUser(int argc, char* argv[]);
uint32_t ecmdGetGpioRegUser(int argc, char* argv[]);
uint32_t ecmdPutGpioRegUser(int argc, char* argv[]);
//@}



#endif /* ecmdInterpreter_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
