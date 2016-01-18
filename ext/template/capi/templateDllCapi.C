//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG

/* NOTE : This file has been generated from the eCMD extension temp-late */
/* DO NOT EDIT THISFILE (unless you are editing the temp-late */







// Module Description **************************************************
//
// Description: TEMPLATE Extension common plugin code
//
// End Module Description **********************************************


//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <templateDllCapi.H>

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
#ifndef ECMD_STRIP_DEBUG
/* This is en ecmdDllCapi.C */
extern uint32_t ecmdGlobal_DllDebug;
#endif

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------

uint32_t dllTemplateInitExtension (const char * i_clientVersion) {

  /* First off let's check our version */
  std::string DllVersion = ECMD_TEMPLATE_CAPI_VERSION;

  /* Let's found our '.' char because we only fail if the Major number changes */
  int majorlength = (int)(strchr(i_clientVersion, '.') - i_clientVersion);

  if (strncmp(i_clientVersion,DllVersion.c_str(),majorlength)) {
    fprintf(stderr,"**** FATAL : eCMD TEMPLATE Extension and your client Major version numbers don't match, they are not compatible\n");
    fprintf(stderr,"**** FATAL : Client Version : %s   : Extension Version : %s\n",i_clientVersion, DllVersion.c_str());

    if (atoi(i_clientVersion) < atoi(DllVersion.c_str())) {
      fprintf(stderr,"**** FATAL : Your client is older then the eCMD TEMPLATE Extension Plugin you are running\n");
      fprintf(stderr,"**** FATAL : You must grab the latest client libraries and rebuild your client to continue\n");
    } else {
      fprintf(stderr,"**** FATAL : It appears your client is newer then the eCMD TEMPLATE Extension Plugin you are running\n");
      fprintf(stderr,"**** FATAL : Contact the eCMD team to have the Plugin rebuilt to match your client\n");
      fprintf(stderr,"**** FATAL : Or get ahold of down level client libraries and rebuild your client to match\n");

    }

    exit(999);
  }


#ifndef ECMD_STRIP_DEBUG
  if (ecmdGlobal_DllDebug >= 8) {
    printf("ECMD DEBUG : TEMPLATE Client Version '%s'\n", i_clientVersion);
    printf("ECMD DEBUG : TEMPLATE Plugin Version '%s'\n", DllVersion.c_str());
  }
#endif

  return dllTemplateInitExtensionInPlugin();
}
