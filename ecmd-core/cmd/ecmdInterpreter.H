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

#ifndef ecmdInterpreter_h
#define ecmdInterpreter_h


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
f*/
typedef enum {  
  ECMD_MEM_DMA,
  ECMD_MEM_MEMCTRL,
  ECMD_MEM_PROC,
  ECMD_GPR,
  ECMD_FPR,
  ECMD_SRAM
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
uint32_t ecmdGetSpyImageUser(int argc, char* argv[]);
uint32_t ecmdPutSpyImageUser(int argc, char* argv[]);

/* ecmdMemUser.C */
uint32_t ecmdCacheFlushUser(int argc, char* argv[]);
uint32_t ecmdGetMemUser(int argc, char* argv[], ECMD_DA_TYPE memMode);
uint32_t ecmdPutMemUser(int argc, char* argv[], ECMD_DA_TYPE memMode);
uint32_t ecmdGetMemPbaUser(int argc, char * argv[]);
uint32_t ecmdPutMemPbaUser(int argc, char * argv[]);

/* ecmdQueryUser.C */
uint32_t ecmdQueryUser(int argc, char* argv[]);

/* ecmdProcUser.C */
uint32_t ecmdGetSprUser(int argc, char * argv[]);
uint32_t ecmdPutSprUser(int argc, char * argv[]);
uint32_t ecmdGetGprFprUser(int argc, char * argv[], ECMD_DA_TYPE daType);
uint32_t ecmdPutGprFprUser(int argc, char * argv[], ECMD_DA_TYPE daType);


/* ecmdIstepUser.C */
uint32_t ecmdIstepUser(int argc, char * argv[]);
uint32_t ecmdInitChipFromFileUser(int argc, char * argv[]);
uint32_t ecmdSetClockSpeedUser(int argc, char * argv[]);
uint32_t ecmdGetClockSpeedUser(int argc, char * argv[]);
uint32_t ecmdStartClocksUser(int argc, char * argv[]);
uint32_t ecmdStopClocksUser(int argc, char * argv[]);

/* ecmdMiscUser.C */
uint32_t ecmdDeconfigUser(int argc, char * argv[]);
uint32_t ecmdReconfigUser(int argc, char * argv[]);
uint32_t ecmdGetConfigUser(int argc, char * argv[]);
uint32_t ecmdSetConfigUser(int argc, char * argv[]);
uint32_t ecmdGetCfamUser(int argc, char* argv[]);
uint32_t ecmdGetEcidUser(int argc, char * argv[]);
uint32_t ecmdMakeSPSystemCallUser(int argc, char* argv[]);
uint32_t ecmdPutCfamUser(int argc, char* argv[]);
uint32_t ecmdGetGpRegisterUser(int argc, char* argv[]);
uint32_t ecmdPutGpRegisterUser(int argc, char* argv[]);
uint32_t ecmdEchoUser(int argc, char* argv[]);
uint32_t ecmdDelayUser(int argc, char* argv[]);
uint32_t ecmdUnitIdUser(int argc, char* argv[]);  
uint32_t ecmdGetSensorUser(int argc, char* argv[]);
uint32_t ecmdSyncPluginStateUser(int argc, char* argv[]);
uint32_t ecmdSyncIplModeUser(int argc, char* argv[]);
uint32_t ecmdMpiplClearCheckstopUser(int argc, char* argv[]);
uint32_t ecmdMpiplForceWinkleUser(int argc, char* argv[]);
uint32_t ecmdChipCleanupUser(int argc, char * argv[]);

/* ecmdVpdUser.C */
uint32_t ecmdPutVpdKeywordUser(int argc, char* argv[]);
uint32_t ecmdGetVpdKeywordUser(int argc, char* argv[]);
uint32_t ecmdPutVpdImageUser(int argc, char* argv[]);
uint32_t ecmdGetVpdImageUser(int argc, char* argv[]);

/* ecmdI2cGpioUser.C */
uint32_t ecmdPutI2cUser(int argc, char* argv[]);
uint32_t ecmdGetI2cUser(int argc, char* argv[]);
uint32_t ecmdI2cResetUser(int argc, char* argv[]);
uint32_t ecmdI2cMultipleUser(int argc, char* argv[]);
uint32_t ecmdGetGpioPinUser(int argc, char* argv[]);
uint32_t ecmdGetGpioLatchUser(int argc, char* argv[]);
uint32_t ecmdPutGpioLatchUser(int argc, char* argv[]);
uint32_t ecmdGpioConfigUser(int argc, char* argv[]);
uint32_t ecmdGetGpioRegUser(int argc, char* argv[]);
uint32_t ecmdPutGpioRegUser(int argc, char* argv[]);

/* ecmdSpiUser.C */
uint32_t ecmdPutSpiUser(int argc, char* argv[]);
uint32_t ecmdGetSpiUser(int argc, char* argv[]);

/* ecmdPowerUser.C */
uint32_t ecmdSystemPowerUser(int argc, char * argv[]);
uint32_t ecmdFruPowerUser(int argc, char * argv[]);
uint32_t ecmdPowerModeUser(int argc, char * argv[]);
uint32_t ecmdBiasVoltageUser(int argc, char * argv[]);
uint32_t ecmdQueryBiasStateUser(int argc, char * argv[]);
//@}

/* ecmdAdalPsiUser.C */
uint32_t ecmdAdalPsiUser(int argc, char * argv[]);

/* ecmdPnorUser.C */
uint32_t ecmdGetPnorUser(int argc, char * argv[]);
uint32_t ecmdPutPnorUser(int argc, char * argv[]);

#endif /* ecmdInterpreter_h */
