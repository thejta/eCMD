//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG


/* This file tells us what extensions are supported by the plugin env we are compiling in (default is all) */
#include <ecmdPluginExtensionSupport.H>


//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <inttypes.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

#include <ecmdClientCapi.H>
#include <ecmdExtInterpreter.H>
#include <ecmdReturnCodes.H>
#include <ecmdCommandUtils.H>
#include <ecmdSharedUtils.H>

/* Extension Interpreters */
#ifdef ECMD_FAPI_EXTENSION_SUPPORT
#include <fapiClientCapi.H>
#include <fapiInterpreter.H>
#endif

#ifdef ECMD_FAPI2_EXTENSION_SUPPORT
#include <fapi2ClientCapi.H>
#include <fapi2Interpreter.H>
#endif

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

uint32_t ecmdCallExtInterpreters(int argc, char* argv[], uint32_t & io_rc) {
  uint32_t rc = ECMD_SUCCESS;

#ifdef ECMD_CIP_EXTENSION_SUPPORT
  /* Cronus/IP Extension */
  if ((io_rc == ECMD_INT_UNKNOWN_COMMAND) && (!strncmp("cip",argv[0],3))) {
    io_rc = cipInitExtension();
    if (io_rc == ECMD_SUCCESS) {
      io_rc = cipCommandInterpreter(argc, argv);
    }
  }
#endif

#ifdef ECMD_CRO_EXTENSION_SUPPORT
  /* Cronus Extension */
  if ((io_rc == ECMD_INT_UNKNOWN_COMMAND) && (!strncmp("cro",argv[0],3))) {
    io_rc = croInitExtension();
    if (io_rc == ECMD_SUCCESS) {
      io_rc = croCommandInterpreter(argc, argv);
    }
  }
#endif

#ifdef ECMD_EIP_EXTENSION_SUPPORT
  /* Eclipz IP Extension */
  if ((io_rc == ECMD_INT_UNKNOWN_COMMAND) && (!strncmp("eip",argv[0],3))) {
    io_rc = eipInitExtension();
    if (io_rc == ECMD_SUCCESS) {
      io_rc = eipCommandInterpreter(argc, argv);
    }
  }
#endif

#ifdef ECMD_AIP_EXTENSION_SUPPORT
  /* Apollo IP Extension */
  if ((io_rc == ECMD_INT_UNKNOWN_COMMAND) && (!strncmp("aip",argv[0],3))) {
    io_rc = aipInitExtension();
    if (io_rc == ECMD_SUCCESS) {
      io_rc = aipCommandInterpreter(argc, argv);
    }
  }
#endif

#ifdef ECMD_GIP_EXTENSION_SUPPORT
  /* GFW IP Extension */
  if ((io_rc == ECMD_INT_UNKNOWN_COMMAND) && (!strncmp("gip",argv[0],3))) {
    io_rc = gipInitExtension();
    if (io_rc == ECMD_SUCCESS) {
      io_rc = gipCommandInterpreter(argc, argv);
    }
  }
#endif

#ifdef ECMD_ZSE_EXTENSION_SUPPORT
  /* Z Series Extension */
  if ((io_rc == ECMD_INT_UNKNOWN_COMMAND) && (!strncmp("zse",argv[0],3))) {
    //    io_rc = zseInitExtension();  moved to zseCommandInterpreter hjh 07/2009
    // --------------------------------------------------------------
    // call   io_rc = zseCommandInterpreter(argc, argv)  dynamically 
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
        return ECMD_DLL_LOAD_FAILURE;          /* file leak */                //done by unloadDll
      }
    }
           
    zseInterpreterFunction = (void *)dlsym(soHandle, "zseCommandInterpreter");
    if (zseInterpreterFunction == NULL)
    {
      if ((soError = dlerror()) != NULL) {
        fprintf(stderr,"ERROR: ecmdLoad Function zseCommandInterpreter error:  : %s\n",  soError);
	dlclose(soHandle);
        return ECMD_DLL_LOAD_FAILURE;          /* file leak */       //done by unloadDll

      }
  
    }
     // fprintf(stderr,"NoERROR: pointer %u \n",  (uint32_t)zseInterpreterFunction);
     //uint32_t (*function)(int,  char*[]) = (uint32_t(*)(int,  char*[]))zseInterpreterFunction;
     uint32_t (*function)(int,  char*[]) = (uint32_t(*)(int,  char*[]))zseInterpreterFunction;  
     io_rc =  (*function)(argc, argv);   /* null function */
  
     //   io_rc = zseCommandInterpreter(argc, argv);
    if(soHandle != NULL)
	dlclose(soHandle);
  }
#endif    /* file leak */                                           

#ifdef ECMD_BML_EXTENSION_SUPPORT
  /* BML Extension */
  if ((io_rc == ECMD_INT_UNKNOWN_COMMAND) && (!strncmp("bml",argv[0],3))) {
    io_rc = bmlInitExtension();
    if (io_rc == ECMD_SUCCESS) {
      io_rc = bmlCommandInterpreter(argc, argv);
    }
  }
#endif

#ifdef ECMD_MBO_EXTENSION_SUPPORT
  /* Mambo Extension */
  if ((io_rc == ECMD_INT_UNKNOWN_COMMAND) && (!strncmp("mbo",argv[0],3))) {
    io_rc = mboInitExtension();
    if (io_rc == ECMD_SUCCESS) {
      io_rc = mboCommandInterpreter(argc, argv);
    }
  }
#endif

#ifdef ECMD_FAPI_EXTENSION_SUPPORT
  /* Fapi Extension */
  if ((io_rc == ECMD_INT_UNKNOWN_COMMAND) && (!strncmp("fapi",argv[0],4)) && (strncmp("fapi2",argv[0],5))) {
    io_rc = fapiInitExtension();
    if (io_rc == ECMD_SUCCESS) {
      io_rc = fapiCommandInterpreter(argc, argv);
    }
  }
#endif

#ifdef ECMD_FAPI2_EXTENSION_SUPPORT
  /* Fapi2 Extension */
  if ((io_rc == ECMD_INT_UNKNOWN_COMMAND) && (!strncmp("fapi2",argv[0],5))) {
    io_rc = fapi2InitExtension();
    if (io_rc == ECMD_SUCCESS) {
      io_rc = fapi2CommandInterpreter(argc, argv);
    }
  }
#endif
  
  return rc;
}
