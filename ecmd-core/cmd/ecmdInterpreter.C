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


//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <inttypes.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

#include <ecmdClientCapi.H>
#include <ecmdInterpreter.H>
#include <ecmdExtInterpreter.H>
#include <ecmdReturnCodes.H>
#include <ecmdCommandUtils.H>
#include <ecmdSharedUtils.H>

//----------------------------------------------------------------------
//  User Types
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Macros
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Internal Function Prototypes
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------

uint32_t ecmdCallInterpreters(int argc, char* argv[]) {
  uint32_t rc = ECMD_SUCCESS;


  if (argc == 0) {
    /* How did we get here ? */
    rc = ECMD_INT_UNKNOWN_COMMAND;
    return rc;
  }

  /* Core interpreter */
  rc = ecmdCommandInterpreter(argc, argv);

  /* Call the extension command interpreters */
  /* The functions handles calling to the enabled extensions */
  uint32_t ext_rc = ecmdCallExtInterpreters(argc, argv, rc);
  if (ext_rc) return ext_rc;

  return rc;
}

uint32_t ecmdCommandInterpreter(int argc, char* argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  if (argc >= 1) {

    /* Let's handle the '-h' arg right here */
    if (ecmdParseOption(&argc, &argv, "-h")) {
      if (argc == 0)
        rc=ecmdPrintHelp("ecmd");
      else
        rc=ecmdPrintHelp(argv[0]);
      return rc;
    }

    switch (argv[0][0]) {

        /************************/
        /* The B's              */
        /************************/
      case 'b':

        if (!strcmp(argv[0], "b")) { // stub to get 'if' at the start
          rc = ECMD_INT_UNKNOWN_COMMAND;
#ifndef ECMD_REMOVE_POWER_FUNCTIONS
        } else if (!strcmp(argv[0], "biasvoltage")) {
          rc = ecmdBiasVoltageUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_POWER_FUNCTIONS
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;

        /************************/
        /* The C's              */
        /************************/
      case 'c':

        if (!strcmp(argv[0], "c")) { // stub to get 'if' at the start
          rc = ECMD_INT_UNKNOWN_COMMAND;
#ifndef ECMD_REMOVE_MEMORY_FUNCTIONS
        } else if (!strcmp(argv[0], "cacheflush")) {
          rc = ecmdCacheFlushUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_MEMORY_FUNCTIONS
#ifndef ECMD_REMOVE_RING_FUNCTIONS
        } else if (!strcmp(argv[0], "checkrings")) {
          rc = ecmdCheckRingsUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_RING_FUNCTIONS
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;

        /************************/
        /* The D's              */
        /************************/
      case 'd':

        if (!strcmp(argv[0], "deconfig")) {
          rc = ecmdDeconfigUser(argc - 1, argv + 1);
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;

        /************************/
        /* The E's              */
        /************************/
      case 'e':

        if (!strcmp(argv[0], "ecmdecho")) {
          rc = ecmdEchoUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "ecmdquery")) {
          rc = ecmdQueryUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "ecmddelay")) {
          rc = ecmdDelayUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "ecmdchipcleanup")) {
          rc = ecmdChipCleanupUser(argc - 1, argv + 1);
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;

        /************************/
        /* The F's              */
        /************************/
      case 'f':

        if (!strcmp(argv[0], "f")) { // stub to get 'if' at the start
          rc = ECMD_INT_UNKNOWN_COMMAND;
#ifndef ECMD_REMOVE_POWER_FUNCTIONS
        } else if (!strcmp(argv[0], "frupower")) {
          rc = ecmdFruPowerUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_POWER_FUNCTIONS
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;

        /************************/
        /* The G's              */
        /************************/
      case 'g':

        if (!strcmp(argv[0], "g")) { // stub to get 'if' at the start
          rc = ECMD_INT_UNKNOWN_COMMAND;
#ifndef ECMD_REMOVE_ARRAY_FUNCTIONS
        } else if (!strcmp(argv[0], "getarray")) {
          rc = ecmdGetArrayUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_ARRAY_FUNCTIONS
#ifndef ECMD_REMOVE_RING_FUNCTIONS
        } else if (!strcmp(argv[0], "getbits")) {
          rc = ecmdGetBitsUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_RING_FUNCTIONS
#ifndef ECMD_REMOVE_FSI_FUNCTIONS
        } else if (!strcmp(argv[0], "getcfam")) {
          rc = ecmdGetCfamUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_FSI_FUNCTIONS
#ifndef ECMD_REMOVE_REFCLOCK_FUNCTIONS
        } else if (!strcmp(argv[0], "getclockspeed")) {
          rc = ecmdGetClockSpeedUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_REFCLOCK_FUNCTIONS
        } else if (!strcmp(argv[0], "getconfig")) {
          rc = ecmdGetConfigUser(argc - 1, argv + 1);
#ifndef ECMD_REMOVE_FSI_FUNCTIONS
        } else if (!strcmp(argv[0], "getecid")) {
          rc = ecmdGetEcidUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_FSI_FUNCTIONS
#ifndef ECMD_REMOVE_PROCESSOR_FUNCTIONS
        } else if (!strcmp(argv[0], "getfpr")) {
          rc = ecmdGetGprFprUser(argc - 1, argv + 1, ECMD_FPR);
#endif // ECMD_REMOVE_PROCESSOR_FUNCTIONS
#ifndef ECMD_REMOVE_GPIO_FUNCTIONS
        } else if (!strcmp(argv[0], "getgpiopin")) {
          rc = ecmdGetGpioPinUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "getgpiolatch")) {
          rc = ecmdGetGpioLatchUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "getgpioreg")) {
          rc = ecmdGetGpioRegUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_GPIO_FUNCTIONS
#ifndef ECMD_REMOVE_PROCESSOR_FUNCTIONS
        } else if (!strcmp(argv[0], "getgpr")) {
          rc = ecmdGetGprFprUser(argc - 1, argv + 1, ECMD_GPR);
#endif // ECMD_REMOVE_PROCESSOR_FUNCTIONS
#ifndef ECMD_REMOVE_FSI_FUNCTIONS
        } else if (!strcmp(argv[0], "getgpreg")) {
          rc = ecmdGetGpRegisterUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_FSI_FUNCTIONS
#ifndef ECMD_REMOVE_I2C_FUNCTIONS
        } else if (!strcmp(argv[0], "geti2c")) {
          rc = ecmdGetI2cUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_I2C_FUNCTIONS
#ifndef ECMD_REMOVE_LATCH_FUNCTIONS
        } else if (!strcmp(argv[0], "getlatch")) {
          rc = ecmdGetLatchUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_LATCH_FUNCTIONS
#ifndef ECMD_REMOVE_MEMORY_FUNCTIONS
        } else if (!strcmp(argv[0], "getmemdma")) {
          rc = ecmdGetMemUser(argc - 1, argv + 1, ECMD_MEM_DMA);
        } else if (!strcmp(argv[0], "getmemmemctrl")) {
          rc = ecmdGetMemUser(argc - 1, argv + 1, ECMD_MEM_MEMCTRL);
        } else if (!strcmp(argv[0], "getmempba")) {
          rc = ecmdGetMemPbaUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "getmemproc")) {
          rc = ecmdGetMemUser(argc - 1, argv + 1, ECMD_MEM_PROC);
        } else if (!strcmp(argv[0], "getsram")) {
          rc = ecmdGetMemUser(argc - 1, argv + 1, ECMD_SRAM);
#endif // ECMD_REMOVE_MEMORY_FUNCTIONS
#if !defined(ECMD_REMOVE_LATCH_FUNCTIONS) && !defined(ECMD_REMOVE_RING_FUNCTIONS)
        } else if (!strcmp(argv[0], "getringdump")) {
          rc = ecmdGetRingDumpUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_LATCH_FUNCTIONS && ECMD_REMOVE_RING_FUNCTIONS
#ifndef ECMD_REMOVE_SCOM_FUNCTIONS
        } else if (!strcmp(argv[0], "getscom")) {
          rc = ecmdGetScomUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_SCOM_FUNCTIONS
#ifndef ECMD_REMOVE_PROCESSOR_FUNCTIONS
        } else if (!strcmp(argv[0], "getspr")) {
          rc = ecmdGetSprUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_PROCESSOR_FUNCTIONS
#ifndef ECMD_REMOVE_SPY_FUNCTIONS
        } else if (!strcmp(argv[0], "getspy")) {
          rc = ecmdGetSpyUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "getspyimage")) {
          rc = ecmdGetSpyImageUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_SPY_FUNCTIONS
#ifndef ECMD_REMOVE_TRACEARRAY_FUNCTIONS
        } else if (!strcmp(argv[0], "gettracearray")) {
          rc = ecmdGetTraceArrayUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_TRACEARRAY_FUNCTIONS
#ifndef ECMD_REMOVE_VPD_FUNCTIONS
        } else if (!strcmp(argv[0], "getvpdkeyword")) {
          rc = ecmdGetVpdKeywordUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "getvpdimage")) {
          rc = ecmdGetVpdImageUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_VPD_FUNCTIONS
#ifndef ECMD_REMOVE_GPIO_FUNCTIONS
        } else if (!strcmp(argv[0], "gpioconfig")) {
          rc = ecmdGpioConfigUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_GPIO_FUNCTIONS        
#ifndef ECMD_REMOVE_SENSOR_FUNCTIONS
        } else if (!strcmp(argv[0], "getsensor")) {
          rc = ecmdGetSensorUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_SENSOR_FUNCTIONS
#ifndef ECMD_REMOVE_PNOR_FUNCTIONS
        } else if (!strcmp(argv[0], "getpnor")) {
          rc = ecmdGetPnorUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_PNOR_FUNCTIONS
#ifndef ECMD_REMOVE_SPI_FUNCTIONS
        } else if (!strcmp(argv[0], "getspi")) {
          rc = ecmdGetSpiUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_SPI_FUNCTIONS
        } 
        else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;



        /************************/
        /* The I's              */
        /************************/
      case 'i':

        if (0) {
          // Empty if to always start things in case the below gets compiled out
#ifndef ECMD_REMOVE_INIT_FUNCTIONS
        } else if (!strcmp(argv[0], "initchipfromfile")) {
          rc = ecmdInitChipFromFileUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "istep")) {
          rc = ecmdIstepUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_INIT_FUNCTIONS
#ifndef ECMD_REMOVE_I2C_FUNCTIONS
        } else if (!strcmp(argv[0], "i2cmultiple")) {
          rc = ecmdI2cMultipleUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "i2creset")) {
          rc = ecmdI2cResetUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_I2C_FUNCTIONS
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;


        /************************/
        /* The M's              */
        /************************/
      case 'm':

	if (0) {
          // Empty if to always start things in case the below gets compiled out
#ifndef ECMD_REMOVE_SP_FUNCTIONS
        } else if (!strcmp(argv[0], "makespsystemcall")) {
          rc = ecmdMakeSPSystemCallUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_SP_FUNCTIONS
#ifndef ECMD_REMOVE_MPIPL_FUNCTIONS
	} else if (!strcmp(argv[0], "mpiplclearcheckstop")) {
          rc = ecmdMpiplClearCheckstopUser(argc - 1, argv + 1);
	} else if (!strcmp(argv[0], "mpiplforcewinkle")) {
          rc = ecmdMpiplForceWinkleUser(argc - 1, argv + 1);
#endif //ECMD_REMOVE_MPIPL_FUNCTIONS
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;


        /************************/
        /* The P's              */
        /************************/
      case 'p':

        if (!strcmp(argv[0], "p")) { // stub to get 'if' at the start
          rc = ECMD_INT_UNKNOWN_COMMAND;
        } else if (!strcmp(argv[0], "psi")) { 
#ifndef ECMD_REMOVE_ADAL_FUNCTIONS
	  rc = ecmdAdalPsiUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_ADAL_FUNCTIONS
#ifndef ECMD_REMOVE_ARRAY_FUNCTIONS
        } else if (!strcmp(argv[0], "putarray")) {
          rc = ecmdPutArrayUser(argc - 1, argv + 1);
#endif //  ECMD_REMOVE_ARRAY_FUNCTIONS
#ifndef ECMD_REMOVE_RING_FUNCTIONS
        } else if (!strcmp(argv[0], "putbits")) {
          rc = ecmdPutBitsUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_RING_FUNCTIONS
#ifndef ECMD_REMOVE_FSI_FUNCTIONS
        } else if (!strcmp(argv[0], "putcfam")) {
          rc = ecmdPutCfamUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_FSI_FUNCTIONS
#ifndef ECMD_REMOVE_PROCESSOR_FUNCTIONS
        } else if (!strcmp(argv[0], "putfpr")) {
          rc = ecmdPutGprFprUser(argc - 1, argv + 1, ECMD_FPR);
#endif // ECMD_REMOVE_PROCESSOR_FUNCTIONS
#ifndef ECMD_REMOVE_GPIO_FUNCTIONS
        } else if (!strcmp(argv[0], "putgpiolatch")) {
          rc = ecmdPutGpioLatchUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "putgpioreg")) {
          rc = ecmdPutGpioRegUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_GPIO_FUNCTIONS
#ifndef ECMD_REMOVE_PROCESSOR_FUNCTIONS
        } else if (!strcmp(argv[0], "putgpr")) {
          rc = ecmdPutGprFprUser(argc - 1, argv + 1, ECMD_GPR);
#endif // ECMD_REMOVE_PROCESSOR_FUNCTIONS
#ifndef ECMD_REMOVE_FSI_FUNCTIONS
        } else if (!strcmp(argv[0], "putgpreg")) {
          rc = ecmdPutGpRegisterUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_FSI_FUNCTIONS
#ifndef ECMD_REMOVE_I2C_FUNCTIONS
        } else if (!strcmp(argv[0], "puti2c")) {
          rc = ecmdPutI2cUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_I2C_FUNCTIONS
#ifndef ECMD_REMOVE_SCOM_FUNCTIONS
        } else if (!strcmp(argv[0], "pollscom")) {
          rc = ecmdPollScomUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_SCOM_FUNCTIONS
#ifndef ECMD_REMOVE_LATCH_FUNCTIONS
        } else if (!strcmp(argv[0], "putlatch")) {
          rc = ecmdPutLatchUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_LATCH_FUNCTIONS
#ifndef ECMD_REMOVE_MEMORY_FUNCTIONS
        } else if (!strcmp(argv[0], "putmemdma")) {
          rc = ecmdPutMemUser(argc - 1, argv + 1, ECMD_MEM_DMA);
        } else if (!strcmp(argv[0], "putmemmemctrl")) {
          rc = ecmdPutMemUser(argc - 1, argv + 1, ECMD_MEM_MEMCTRL);
        } else if (!strcmp(argv[0], "putmempba")) {
          rc = ecmdPutMemPbaUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "putmemproc")) {
          rc = ecmdPutMemUser(argc - 1, argv + 1, ECMD_MEM_PROC);
        } else if (!strcmp(argv[0], "putsram")) {
          rc = ecmdPutMemUser(argc - 1, argv + 1, ECMD_SRAM);
#endif // ECMD_REMOVE_MEMORY_FUNCTIONS
#ifndef ECMD_REMOVE_RING_FUNCTIONS
        } else if (!strcmp(argv[0], "putpattern")) {
          rc = ecmdPutPatternUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_RING_FUNCTIONS
#ifndef ECMD_REMOVE_SCOM_FUNCTIONS
        } else if (!strcmp(argv[0], "putscom")) {
          rc = ecmdPutScomUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_SCOM_FUNCTIONS
#ifndef ECMD_REMOVE_PROCESSOR_FUNCTIONS
        } else if (!strcmp(argv[0], "putspr")) {
          rc = ecmdPutSprUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_PROCESSOR_FUNCTIONS
#ifndef ECMD_REMOVE_SPY_FUNCTIONS
        } else if (!strcmp(argv[0], "putspy")) {
          rc = ecmdPutSpyUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "putspyimage")) {
          rc = ecmdPutSpyImageUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_SPY_FUNCTIONS
#ifndef ECMD_REMOVE_VPD_FUNCTIONS
        } else if (!strcmp(argv[0], "putvpdkeyword")) {
          rc = ecmdPutVpdKeywordUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "putvpdimage")) {
          rc = ecmdPutVpdImageUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_VPD_FUNCTIONS
#ifndef ECMD_REMOVE_POWER_FUNCTIONS
        } else if (!strcmp(argv[0], "powermode")) {
          rc = ecmdPowerModeUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_POWER_FUNCTIONS
#ifndef ECMD_REMOVE_PNOR_FUNCTIONS
        } else if (!strcmp(argv[0], "putpnor")) {
          rc = ecmdPutPnorUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_PNOR_FUNCTIONS
#ifndef ECMD_REMOVE_SPI_FUNCTIONS
        } else if (!strcmp(argv[0], "putspi")) {
          rc = ecmdPutSpiUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_SPI_FUNCTIONS
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;


        /************************/
        /* The Q's              */
        /************************/
      case 'q':

        if (!strcmp(argv[0], "q")) { // stub to get 'if' at the start
          rc = ECMD_INT_UNKNOWN_COMMAND; 
#ifndef ECMD_REMOVE_POWER_FUNCTIONS
        } else if (!strcmp(argv[0], "querybiasstate")) {
          rc = ecmdQueryBiasStateUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_POWER_FUNCTIONS
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;


        /************************/
        /* The R's              */
        /************************/
      case 'r':

        if (!strcmp(argv[0], "reconfig")) {
          rc = ecmdReconfigUser(argc - 1, argv + 1);
#ifndef ECMD_REMOVE_RING_FUNCTIONS
        } else if (!strcmp(argv[0], "ringcache")) {
          rc = ecmdRingCacheUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_RING_FUNCTIONS
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;


      case 's':

        if (0) {
          // empty in case sendcmd gets compiled out
#ifndef ECMD_REMOVE_JTAG_FUNCTIONS
        } else if (!strcmp(argv[0], "sendcmd")) {
          rc = ecmdSendCmdUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_JTAG_FUNCTIONS
#ifndef ECMD_REMOVE_REFCLOCK_FUNCTIONS
        } else if (!strcmp(argv[0], "setclockspeed")) {
          rc = ecmdSetClockSpeedUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_REFCLOCK_FUNCTIONS
        } else if (!strcmp(argv[0], "setconfig")) {
          rc = ecmdSetConfigUser(argc - 1, argv + 1);
#ifndef REMOVE_SIM
        } else if (!strcmp(argv[0], "simaet")) {
          rc = ecmdSimaetUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simcallfusioncommand")) {
          rc = ecmdSimCallFusionCommandUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simcheckpoint")) {
          rc = ecmdSimcheckpointUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simclock")) {
          rc = ecmdSimclockUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simecho")) {
          rc = ecmdSimechoUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simexit")) {
          rc = ecmdSimexitUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simEXPECTFAC")) {
          rc = ecmdSimEXPECTFACUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simexpecttcfac")) {
          rc = ecmdSimexpecttcfacUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simGETFAC")) {
          rc = ecmdSimGETFACUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simGETFACX")) {
          rc = ecmdSimGETFACXUser(argc - 1, argv + 1);
	} else if (!strcmp(argv[0], "simgetfullfacname")) {
          rc = ecmdSimGetFullFacNameUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simgettcfac")) {
          rc = ecmdSimgettcfacUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simgetcurrentcycle")) {
          rc = ecmdSimgetcurrentcycleUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "siminit")) {
          rc = ecmdSiminitUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simoutputfusionmessage")) {
          rc = ecmdSimOutputFusionMessageUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simPUTFAC")) {
          rc = ecmdSimPUTFACUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simPUTFACX")) {
          rc = ecmdSimPUTFACXUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simputtcfac")) {
          rc = ecmdSimputtcfacUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simrestart")) {
          rc = ecmdSimrestartUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simSTKFAC")) {
          rc = ecmdSimSTKFACUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simSTKFACX")) {
          rc = ecmdSimSTKFACXUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simstktcfac")) {
          rc = ecmdSimstktcfacUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simSUBCMD")) {
          rc = ecmdSimSUBCMDUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simtckinterval")) {
          rc = ecmdSimTckIntervalUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simUNSTICK")) {
          rc = ecmdSimUNSTICKUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "simunsticktcfac")) {
          rc = ecmdSimunsticktcfacUser(argc - 1, argv + 1);
	} else if (!strcmp(argv[0], "simgethierarchy")) {
          rc = ecmdSimGetHierarchyUser(argc - 1, argv + 1);
	} else if (!strcmp(argv[0], "simgetdial")) {
          rc = ecmdSimGetDialUser(argc - 1, argv + 1);
	} else if (!strcmp(argv[0], "simputdial")) {
          rc = ecmdSimPutDialUser(argc - 1, argv + 1);
	} else if (!strcmp(argv[0], "simruntestcase")) {
          rc = ecmdSimRunTestcase(argc - 1, argv + 1);
#endif // REMOVE_SIM
#ifndef ECMD_REMOVE_CLOCK_FUNCTIONS
	} else if (!strcmp(argv[0], "startclocks")) {
          rc = ecmdStartClocksUser(argc - 1, argv + 1);
	} else if (!strcmp(argv[0], "stopclocks")) {
          rc = ecmdStopClocksUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_CLOCK_FUNCTIONS
        } else if (!strcmp(argv[0], "syncpluginstate")) {
          rc = ecmdSyncPluginStateUser(argc - 1, argv + 1);
#ifndef ECMD_REMOVE_POWER_FUNCTIONS
        } else if (!strcmp(argv[0], "systempower")) {
          rc = ecmdSystemPowerUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_POWER_FUNCTIONS
#ifndef ECMD_REMOVE_INIT_FUNCTIONS
        } else if (!strcmp(argv[0], "synciplmode")) {
          rc = ecmdSyncIplModeUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_INIT_FUNCTIONS
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;

        /************************/
        /* case : u             */
        /************************/
        case 'u':
#ifndef ECMD_REMOVE_UNITID_FUNCTIONS
          if (!strcmp(argv[0], "unitid")) {            
            rc = ecmdUnitIdUser(argc - 1, argv + 1);
          }
#endif // ECMD_REMOVE_UNITID_FUNCTIONS
          break;
        
        /************************/
        /* The Unknown          */
        /************************/
      default:
        /* We don't understand this function, let's let the caller know */
        rc = ECMD_INT_UNKNOWN_COMMAND;
        break;
    }



  } /* End if (argc >= 1) */
  else {
    /* For now we will return invalid, for the future we may want a shell here */
    rc = ECMD_INT_UNKNOWN_COMMAND;

  }

  return rc;
}
