// Copyright ***********************************************************
//                                                                      
// File ecmdClientCapi.C                                   
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

// Module Description **************************************************
//
// Description: 
//
// End Module Description **********************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <dlfcn.h>

#include <ecmdClientCapi.H>
#include <ecmdDllCapi.H>

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

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************


typedef enum {
ECMD_QUERYCONFIG,
ECMD_QUERYRING,
ECMD_QUERYARRAY,
ECMD_QUERYSPY,
ECMD_QUERYFILELOCATION,
ECMD_GETRING,
ECMD_PUTRING,
ECMD_RINGREAD,
ECMD_RINGWRITE,
ECMD_GETSCOM,
ECMD_PUTSCOM,
ECMD_GETSPY,
ECMD_GETSPYENUM,
ECMD_PUTSPY,
ECMD_PUTSPYENUM,
ECMD_GETARRAY,
ECMD_PUTARRAY,
ECMD_FLUSHSYS,
ECMD_IPLSYS,
ECMD_REGISTERERRORMSG,
ECMD_NUMFUNCTIONS
} ecmdFunctionIndex_t;

void * dlHandle = NULL;
void * DllFnTable[ECMD_NUMFUNCTIONS];

int ecmdQueryConfig(ecmdChipTarget & target, vector<ecmdCageData> & queryData) {

  int rc;

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllQueryConfig(target, queryData);

#else

  if (DllFnTable[ECMD_QUERYCONFIG] == NULL) {
     DllFnTable[ECMD_QUERYCONFIG] = (void*)dlsym(dlHandle, "dllQueryConfig");
  }

  int (*Function)(ecmdChipTarget &,  vector<ecmdCageData> &) = 
      (int(*)(ecmdChipTarget &,  vector<ecmdCageData> &))DllFnTable[ECMD_QUERYCONFIG];

  rc = (*Function)(target, queryData);

#endif

  return rc;

}

int ecmdQueryRing(ecmdChipTarget & target, vector<ecmdRingData> & queryData, const char * ringName ) {

  int rc;

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllQueryRing(target, queryData, ringName);

#else

  if (DllFnTable[ECMD_QUERYRING] == NULL) {
     DllFnTable[ECMD_QUERYRING] = (void*)dlsym(dlHandle, "dllQueryRing");
  }

  int (*Function)(ecmdChipTarget &,  vector<ecmdRingData> &,  const char *) = 
      (int(*)(ecmdChipTarget &,  vector<ecmdRingData> &,  const char *))DllFnTable[ECMD_QUERYRING];

  rc = (*Function)(target, queryData, ringName);

#endif

  return rc;

}

int ecmdQueryArray(ecmdChipTarget & target, vector<ecmdArrayData> & queryData, const char * arrayName) {

  int rc;

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllQueryArray(target, queryData, arrayName);

#else

  if (DllFnTable[ECMD_QUERYARRAY] == NULL) {
     DllFnTable[ECMD_QUERYARRAY] = (void*)dlsym(dlHandle, "dllQueryArray");
  }

  int (*Function)(ecmdChipTarget &,  vector<ecmdArrayData> &,  const char *) = 
      (int(*)(ecmdChipTarget &,  vector<ecmdArrayData> &,  const char *))DllFnTable[ECMD_QUERYARRAY];

  rc = (*Function)(target, queryData, arrayName);

#endif

  return rc;

}

int ecmdQuerySpy(ecmdChipTarget & target, vector<ecmdSpyData> & queryData, const char * spyName) {

  int rc;

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllQuerySpy(target, queryData, spyName);

#else

  if (DllFnTable[ECMD_QUERYSPY] == NULL) {
     DllFnTable[ECMD_QUERYSPY] = (void*)dlsym(dlHandle, "dllQuerySpy");
  }

  int (*Function)(ecmdChipTarget &,  vector<ecmdSpyData> &,  const char *) = 
      (int(*)(ecmdChipTarget &,  vector<ecmdSpyData> &,  const char *))DllFnTable[ECMD_QUERYSPY];

  rc = (*Function)(target, queryData, spyName);

#endif

  return rc;

}

int ecmdQueryFileLocation(ecmdChipTarget & target, ecmdFileType_t fileType, string fileLocation) {

  int rc;

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllQueryFileLocation(target, fileType, fileLocation);

#else

  if (DllFnTable[ECMD_QUERYFILELOCATION] == NULL) {
     DllFnTable[ECMD_QUERYFILELOCATION] = (void*)dlsym(dlHandle, "dllQueryFileLocation");
  }

  int (*Function)(ecmdChipTarget &,  ecmdFileType_t,  string) = 
      (int(*)(ecmdChipTarget &,  ecmdFileType_t,  string))DllFnTable[ECMD_QUERYFILELOCATION];

  rc = (*Function)(target, fileType, fileLocation);

#endif

  return rc;

}

int getRing(ecmdChipTarget & target, const char * ringName, ecmdDataBuffer & data) {

  int rc;

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllGetRing(target, ringName, data);

#else

  if (DllFnTable[ECMD_GETRING] == NULL) {
     DllFnTable[ECMD_GETRING] = (void*)dlsym(dlHandle, "dllGetRing");
  }

  int (*Function)(ecmdChipTarget &,  const char *,  ecmdDataBuffer &) = 
      (int(*)(ecmdChipTarget &,  const char *,  ecmdDataBuffer &))DllFnTable[ECMD_GETRING];

  rc = (*Function)(target, ringName, data);

#endif

  return rc;

}

int putRing(ecmdChipTarget & target, const char * ringName, ecmdDataBuffer & data) {

  int rc;

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllPutRing(target, ringName, data);

#else

  if (DllFnTable[ECMD_PUTRING] == NULL) {
     DllFnTable[ECMD_PUTRING] = (void*)dlsym(dlHandle, "dllPutRing");
  }

  int (*Function)(ecmdChipTarget &,  const char *,  ecmdDataBuffer &) = 
      (int(*)(ecmdChipTarget &,  const char *,  ecmdDataBuffer &))DllFnTable[ECMD_PUTRING];

  rc = (*Function)(target, ringName, data);

#endif

  return rc;

}

int ringRead(ecmdChipTarget & target, const char * ringName, const char * fileName) {

  int rc;

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllRingRead(target, ringName, fileName);

#else

  if (DllFnTable[ECMD_RINGREAD] == NULL) {
     DllFnTable[ECMD_RINGREAD] = (void*)dlsym(dlHandle, "dllRingRead");
  }

  int (*Function)(ecmdChipTarget &,  const char *,  const char *) = 
      (int(*)(ecmdChipTarget &,  const char *,  const char *))DllFnTable[ECMD_RINGREAD];

  rc = (*Function)(target, ringName, fileName);

#endif

  return rc;

}

int ringWrite(ecmdChipTarget & target, const char * ringName, const char * fileName) {

  int rc;

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllRingWrite(target, ringName, fileName);

#else

  if (DllFnTable[ECMD_RINGWRITE] == NULL) {
     DllFnTable[ECMD_RINGWRITE] = (void*)dlsym(dlHandle, "dllRingWrite");
  }

  int (*Function)(ecmdChipTarget &,  const char *,  const char *) = 
      (int(*)(ecmdChipTarget &,  const char *,  const char *))DllFnTable[ECMD_RINGWRITE];

  rc = (*Function)(target, ringName, fileName);

#endif

  return rc;

}

int getScom(ecmdChipTarget & target, uint32_t address, ecmdDataBuffer & data) {

  int rc;

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllGetScom(target, address, data);

#else

  if (DllFnTable[ECMD_GETSCOM] == NULL) {
     DllFnTable[ECMD_GETSCOM] = (void*)dlsym(dlHandle, "dllGetScom");
  }

  int (*Function)(ecmdChipTarget &,  uint32_t,  ecmdDataBuffer &) = 
      (int(*)(ecmdChipTarget &,  uint32_t,  ecmdDataBuffer &))DllFnTable[ECMD_GETSCOM];

  rc = (*Function)(target, address, data);

#endif

  return rc;

}

int putScom(ecmdChipTarget & target, uint32_t address, ecmdDataBuffer & data) {

  int rc;

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllPutScom(target, address, data);

#else

  if (DllFnTable[ECMD_PUTSCOM] == NULL) {
     DllFnTable[ECMD_PUTSCOM] = (void*)dlsym(dlHandle, "dllPutScom");
  }

  int (*Function)(ecmdChipTarget &,  uint32_t,  ecmdDataBuffer &) = 
      (int(*)(ecmdChipTarget &,  uint32_t,  ecmdDataBuffer &))DllFnTable[ECMD_PUTSCOM];

  rc = (*Function)(target, address, data);

#endif

  return rc;

}

int getSpy(ecmdChipTarget & target, const char * spyName, ecmdDataBuffer & data) {

  int rc;

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllGetSpy(target, spyName, data);

#else

  if (DllFnTable[ECMD_GETSPY] == NULL) {
     DllFnTable[ECMD_GETSPY] = (void*)dlsym(dlHandle, "dllGetSpy");
  }

  int (*Function)(ecmdChipTarget &,  const char *,  ecmdDataBuffer &) = 
      (int(*)(ecmdChipTarget &,  const char *,  ecmdDataBuffer &))DllFnTable[ECMD_GETSPY];

  rc = (*Function)(target, spyName, data);

#endif

  return rc;

}

int getSpyEnum(ecmdChipTarget & target, const char * spyName, string enumValue) {

  int rc;

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllGetSpyEnum(target, spyName, enumValue);

#else

  if (DllFnTable[ECMD_GETSPYENUM] == NULL) {
     DllFnTable[ECMD_GETSPYENUM] = (void*)dlsym(dlHandle, "dllGetSpyEnum");
  }

  int (*Function)(ecmdChipTarget &,  const char *,  string) = 
      (int(*)(ecmdChipTarget &,  const char *,  string))DllFnTable[ECMD_GETSPYENUM];

  rc = (*Function)(target, spyName, enumValue);

#endif

  return rc;

}

int putSpy(ecmdChipTarget & target, const char * spyName, ecmdDataBuffer & data) {

  int rc;

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllPutSpy(target, spyName, data);

#else

  if (DllFnTable[ECMD_PUTSPY] == NULL) {
     DllFnTable[ECMD_PUTSPY] = (void*)dlsym(dlHandle, "dllPutSpy");
  }

  int (*Function)(ecmdChipTarget &,  const char *,  ecmdDataBuffer &) = 
      (int(*)(ecmdChipTarget &,  const char *,  ecmdDataBuffer &))DllFnTable[ECMD_PUTSPY];

  rc = (*Function)(target, spyName, data);

#endif

  return rc;

}

int putSpyEnum(ecmdChipTarget & target, const char * spyName, const string enumValue) {

  int rc;

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllPutSpyEnum(target, spyName, enumValue);

#else

  if (DllFnTable[ECMD_PUTSPYENUM] == NULL) {
     DllFnTable[ECMD_PUTSPYENUM] = (void*)dlsym(dlHandle, "dllPutSpyEnum");
  }

  int (*Function)(ecmdChipTarget &,  const char *,  const string) = 
      (int(*)(ecmdChipTarget &,  const char *,  const string))DllFnTable[ECMD_PUTSPYENUM];

  rc = (*Function)(target, spyName, enumValue);

#endif

  return rc;

}

int getArray(ecmdChipTarget & target, const char * arrayName, uint32_t * address, ecmdDataBuffer & data) {

  int rc;

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllGetArray(target, arrayName, address, data);

#else

  if (DllFnTable[ECMD_GETARRAY] == NULL) {
     DllFnTable[ECMD_GETARRAY] = (void*)dlsym(dlHandle, "dllGetArray");
  }

  int (*Function)(ecmdChipTarget &,  const char *,  uint32_t *,  ecmdDataBuffer &) = 
      (int(*)(ecmdChipTarget &,  const char *,  uint32_t *,  ecmdDataBuffer &))DllFnTable[ECMD_GETARRAY];

  rc = (*Function)(target, arrayName, address, data);

#endif

  return rc;

}

int putArray(ecmdChipTarget & target, const char * arrayName, uint32_t * address, ecmdDataBuffer & data) {

  int rc;

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllPutArray(target, arrayName, address, data);

#else

  if (DllFnTable[ECMD_PUTARRAY] == NULL) {
     DllFnTable[ECMD_PUTARRAY] = (void*)dlsym(dlHandle, "dllPutArray");
  }

  int (*Function)(ecmdChipTarget &,  const char *,  uint32_t *,  ecmdDataBuffer &) = 
      (int(*)(ecmdChipTarget &,  const char *,  uint32_t *,  ecmdDataBuffer &))DllFnTable[ECMD_PUTARRAY];

  rc = (*Function)(target, arrayName, address, data);

#endif

  return rc;

}

int flushSys() {

  int rc;

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllFlushSys();

#else

  if (DllFnTable[ECMD_FLUSHSYS] == NULL) {
     DllFnTable[ECMD_FLUSHSYS] = (void*)dlsym(dlHandle, "dllFlushSys");
  }

  int (*Function)() = 
      (int(*)())DllFnTable[ECMD_FLUSHSYS];

  rc = (*Function)();

#endif

  return rc;

}

int iplSys() {

  int rc;

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllIplSys();

#else

  if (DllFnTable[ECMD_IPLSYS] == NULL) {
     DllFnTable[ECMD_IPLSYS] = (void*)dlsym(dlHandle, "dllIplSys");
  }

  int (*Function)() = 
      (int(*)())DllFnTable[ECMD_IPLSYS];

  rc = (*Function)();

#endif

  return rc;

}

int ecmdRegisterErrorMsg(int errorCode, const char* whom, const char* message) {

  int rc;

#ifdef ECMD_STATIC_FUNCTIONS

  rc = dllRegisterErrorMsg(errorCode, whom, message);

#else

  if (DllFnTable[ECMD_REGISTERERRORMSG] == NULL) {
     DllFnTable[ECMD_REGISTERERRORMSG] = (void*)dlsym(dlHandle, "dllRegisterErrorMsg");
  }

  int (*Function)(int,  const char*,  const char*) = 
      (int(*)(int,  const char*,  const char*))DllFnTable[ECMD_REGISTERERRORMSG];

  rc = (*Function)(errorCode, whom, message);

#endif

  return rc;

}

