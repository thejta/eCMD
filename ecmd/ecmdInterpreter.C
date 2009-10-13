/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File ecmdInterpreter.C                                   
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
// End Copyright *******************************************************

/* This file tells us what extensions are supported by the plugin env we are compiling in (default is all) */
#include <ecmdPluginExtensionSupport.H>


//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <inttypes.h>
#include <dlfcn.h>

#include <ecmdClientCapi.H>
#include <ecmdInterpreter.H>
#include <ecmdReturnCodes.H>
#include <ecmdCommandUtils.H>
#include <ecmdSharedUtils.H>

/* Extension Interpreters */
#ifdef ECMD_CIP_EXTENSION_SUPPORT
 #include <cipInterpreter.H>
 #include <cipClientCapi.H>
#endif
#ifdef ECMD_CRO_EXTENSION_SUPPORT
 #include <croClientCapi.H>
 #include <croInterpreter.H>
#endif
#ifdef ECMD_EIP_EXTENSION_SUPPORT
 #include <eipClientCapi.H>
 #include <eipInterpreter.H>
#endif
#ifdef ECMD_AIP_EXTENSION_SUPPORT
 #include <aipClientCapi.H>
 #include <aipInterpreter.H>
#endif
#ifdef ECMD_GIP_EXTENSION_SUPPORT
 #include <gipClientCapi.H>
 #include <gipInterpreter.H>
#endif
#ifdef ECMD_ZSE_EXTENSION_SUPPORT
 #include <zseClientCapi.H>
 #include <zseInterpreter.H>
#endif
#ifdef ECMD_BML_EXTENSION_SUPPORT
 #include <bmlClientCapi.H>
 #include <bmlInterpreter.H>
#endif
#ifdef ECMD_MBO_EXTENSION_SUPPORT
 #include <mboClientCapi.H>
 #include <mboInterpreter.H>
#endif

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

#ifdef ECMD_CIP_EXTENSION_SUPPORT
  /* Cronus/IP Extension */
  if ((rc == ECMD_INT_UNKNOWN_COMMAND) && (!strncmp("cip",argv[0],3))) {
    rc = cipInitExtension();
    if (rc == ECMD_SUCCESS) {
      rc = cipCommandInterpreter(argc, argv);
    }
  }
#endif

#ifdef ECMD_CRO_EXTENSION_SUPPORT
  /* Cronus Extension */
  if ((rc == ECMD_INT_UNKNOWN_COMMAND) && (!strncmp("cro",argv[0],3))) {
    rc = croInitExtension();
    if (rc == ECMD_SUCCESS) {
      rc = croCommandInterpreter(argc, argv);
    }
  }
#endif

#ifdef ECMD_EIP_EXTENSION_SUPPORT
  /* Eclipz IP Extension */
  if ((rc == ECMD_INT_UNKNOWN_COMMAND) && (!strncmp("eip",argv[0],3))) {
    rc = eipInitExtension();
    if (rc == ECMD_SUCCESS) {
      rc = eipCommandInterpreter(argc, argv);
    }
  }
#endif

#ifdef ECMD_AIP_EXTENSION_SUPPORT
  /* Apollo IP Extension */
  if ((rc == ECMD_INT_UNKNOWN_COMMAND) && (!strncmp("aip",argv[0],3))) {
    rc = aipInitExtension();
    if (rc == ECMD_SUCCESS) {
      rc = aipCommandInterpreter(argc, argv);
    }
  }
#endif

#ifdef ECMD_GIP_EXTENSION_SUPPORT
  /* GFW IP Extension */
  if ((rc == ECMD_INT_UNKNOWN_COMMAND) && (!strncmp("gip",argv[0],3))) {
    rc = gipInitExtension();
    if (rc == ECMD_SUCCESS) {
      rc = gipCommandInterpreter(argc, argv);
    }
  }
#endif

#ifdef ECMD_ZSE_EXTENSION_SUPPORT
  /* Z Series Extension */
  if ((rc == ECMD_INT_UNKNOWN_COMMAND) && (!strncmp("zse",argv[0],3))) {
    //    rc = zseInitExtension();  moved to zseCommandInterpreter hjh 07/2009
    // --------------------------------------------------------------
    // call   rc = zseCommandInterpreter(argc, argv)  dynamically 
    // --------------------------------------------------------------
    void * zseInterpreterFunction = NULL;
    void * soHandle = NULL;
    const char* soError;
    char* tmpptr = getenv("ECMD_ZSE_DLL_FILE");
    std::string zseDllFile = tmpptr;

    soHandle = dlopen(zseDllFile.c_str(), RTLD_LAZY);
    if (!soHandle) {
      if ((soError = dlerror()) != NULL) {
        fprintf(stderr,"ERROR loading zse DLL:  : %s\n",  soError);
        return ECMD_DLL_LOAD_FAILURE;          /* file leak */
      }
    }
           
    zseInterpreterFunction = (void *)dlsym(soHandle, "zseCommandInterpreter");
    if (zseInterpreterFunction == NULL)
    {
      if ((soError = dlerror()) != NULL) {
        fprintf(stderr,"ERROR: ecmdLoad Function zseCommandInterpreter error:  : %s\n",  soError);
        return ECMD_DLL_LOAD_FAILURE;          /* file leak */
      }
  
    }
     // fprintf(stderr,"NoERROR: pointer %u \n",  (uint32_t)zseInterpreterFunction);
     //uint32_t (*function)(int,  char*[]) = (uint32_t(*)(int,  char*[]))zseInterpreterFunction;
     uint32_t (*function)(int,  char*[]) = (uint32_t(*)(int,  char*[]))zseInterpreterFunction;  
     rc =  (*function)(argc, argv);   /* null function */
  
     //   rc = zseCommandInterpreter(argc, argv);
  }
#endif    /* file leak */
#ifdef ECMD_BML_EXTENSION_SUPPORT
  /* BML Extension */
  if ((rc == ECMD_INT_UNKNOWN_COMMAND) && (!strncmp("bml",argv[0],3))) {
    rc = bmlInitExtension();
    if (rc == ECMD_SUCCESS) {
      rc = bmlCommandInterpreter(argc, argv);
    }
  }
#endif

#ifdef ECMD_MBO_EXTENSION_SUPPORT
  /* Mambo Extension */
  if ((rc == ECMD_INT_UNKNOWN_COMMAND) && (!strncmp("mbo",argv[0],3))) {
    rc = mboInitExtension();
    if (rc == ECMD_SUCCESS) {
      rc = mboCommandInterpreter(argc, argv);
    }
  }
#endif

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
        } else if (!strcmp(argv[0], "fwsync")) {
          rc = ecmdFwSyncUser(argc - 1, argv + 1);
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
        } else if (!strcmp(argv[0], "getcfam")) {
          rc = ecmdGetCfamUser(argc - 1, argv + 1);
#ifndef ECMD_REMOVE_REFCLOCK_FUNCTIONS
        } else if (!strcmp(argv[0], "getclockspeed")) {
          rc = ecmdGetClockSpeedUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_REFCLOCK_FUNCTIONS
        } else if (!strcmp(argv[0], "getconfig")) {
          rc = ecmdGetConfigUser(argc - 1, argv + 1);
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
        } else if (!strcmp(argv[0], "getgpreg")) {
          rc = ecmdGetGpRegisterUser(argc - 1, argv + 1);
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
        } else if (!strcmp(argv[0], "getmemproc")) {
          rc = ecmdGetMemUser(argc - 1, argv + 1, ECMD_MEM_PROC);
#endif // ECMD_REMOVE_MEMORY_FUNCTIONS
#ifndef ECMD_REMOVE_LATCH_FUNCTIONS
        } else if (!strcmp(argv[0], "getringdump")) {
          rc = ecmdGetRingDumpUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_LATCH_FUNCTIONS
        } else if (!strcmp(argv[0], "getscom")) {
          rc = ecmdGetScomUser(argc - 1, argv + 1);
#ifndef ECMD_REMOVE_PROCESSOR_FUNCTIONS
        } else if (!strcmp(argv[0], "getspr")) {
          rc = ecmdGetSprUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_PROCESSOR_FUNCTIONS
#ifndef ECMD_REMOVE_SPY_FUNCTIONS
        } else if (!strcmp(argv[0], "getspy")) {
          rc = ecmdGetSpyUser(argc - 1, argv + 1);
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

        if (!strcmp(argv[0], "initchipfromfile")) {
          rc = ecmdInitChipFromFileUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "istep")) {
          rc = ecmdIstepUser(argc - 1, argv + 1);
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

        if (!strcmp(argv[0], "makespsystemcall")) {
          rc = ecmdMakeSPSystemCallUser(argc - 1, argv + 1);
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
        } else if (!strcmp(argv[0], "putcfam")) {
          rc = ecmdPutCfamUser(argc - 1, argv + 1);
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
        } else if (!strcmp(argv[0], "putgpreg")) {
          rc = ecmdPutGpRegisterUser(argc - 1, argv + 1);
#ifndef ECMD_REMOVE_I2C_FUNCTIONS
        } else if (!strcmp(argv[0], "puti2c")) {
          rc = ecmdPutI2cUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_I2C_FUNCTIONS
        } else if (!strcmp(argv[0], "pollscom")) {
          rc = ecmdPollScomUser(argc - 1, argv + 1);
#ifndef ECMD_REMOVE_LATCH_FUNCTIONS
        } else if (!strcmp(argv[0], "putlatch")) {
          rc = ecmdPutLatchUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_LATCH_FUNCTIONS
#ifndef ECMD_REMOVE_MEMORY_FUNCTIONS
        } else if (!strcmp(argv[0], "putmemdma")) {
          rc = ecmdPutMemUser(argc - 1, argv + 1, ECMD_MEM_DMA);
        } else if (!strcmp(argv[0], "putmemmemctrl")) {
          rc = ecmdPutMemUser(argc - 1, argv + 1, ECMD_MEM_MEMCTRL);
        } else if (!strcmp(argv[0], "putmemproc")) {
          rc = ecmdPutMemUser(argc - 1, argv + 1, ECMD_MEM_PROC);
#endif // ECMD_REMOVE_MEMORY_FUNCTIONS
#ifndef ECMD_REMOVE_RING_FUNCTIONS
        } else if (!strcmp(argv[0], "putpattern")) {
          rc = ecmdPutPatternUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_RING_FUNCTIONS
        } else if (!strcmp(argv[0], "putscom")) {
          rc = ecmdPutScomUser(argc - 1, argv + 1);
#ifndef ECMD_REMOVE_PROCESSOR_FUNCTIONS
        } else if (!strcmp(argv[0], "putspr")) {
          rc = ecmdPutSprUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_PROCESSOR_FUNCTIONS
#ifndef ECMD_REMOVE_SPY_FUNCTIONS
        } else if (!strcmp(argv[0], "putspy")) {
          rc = ecmdPutSpyUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_SPY_FUNCTIONS
#ifndef ECMD_REMOVE_VPD_FUNCTIONS
        } else if (!strcmp(argv[0], "putvpdkeyword")) {
          rc = ecmdPutVpdKeywordUser(argc - 1, argv + 1);
        } else if (!strcmp(argv[0], "putvpdimage")) {
          rc = ecmdPutVpdImageUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_VPD_FUNCTIONS
#ifndef ECMD_REMOVE_POWER_FUNCTIONS
        } else if (!strcmp(argv[0], "powermode")) {
          rc = ecmdFruPowerUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_POWER_FUNCTIONS
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

        if (!strcmp(argv[0], "sendcmd")) {
          rc = ecmdSendCmdUser(argc - 1, argv + 1);
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
#endif // REMOVE_SIM
#ifndef ECMD_REMOVE_CLOCK_FUNCTIONS
	} else if (!strcmp(argv[0], "startclocks")) {
          rc = ecmdStartClocksUser(argc - 1, argv + 1);
	} else if (!strcmp(argv[0], "stopclocks")) {
          rc = ecmdStopClocksUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_CLOCK_FUNCTIONS
#ifndef ECMD_REMOVE_POWER_FUNCTIONS
        } else if (!strcmp(argv[0], "systempower")) {
          rc = ecmdSystemPowerUser(argc - 1, argv + 1);
#endif // ECMD_REMOVE_POWER_FUNCTIONS
        } else {
          /* We don't understand this function, let's let the caller know */
          rc = ECMD_INT_UNKNOWN_COMMAND;
        }
        break;

        /************************/
        /* case : u             */
        /************************/
        case 'u':
        if (!strcmp(argv[0], "unitid")) {            
          rc = ecmdUnitIdUser(argc - 1, argv + 1);
        }
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


// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
