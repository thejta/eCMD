// Copyright **********************************************************
//                                                                      
// File ecmdDllCapi.H                                               
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 2003                                         
//                                                                      
// The source code for this program is not published or otherwise       
// divested of its trade secrets, irrespective of what has been         
// deposited with the U.S. Copyright Office.                             
//                                                                      
// End Copyright ******************************************************


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <inttypes.h>
#include <stdio.h>

#include <ecmdDllCapi.H>
#include <ecmdStructs.H>
#include <ecmdReturnCodes.H>
#include <ecmdDataBuffer.H>

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------


// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              willsj Initial Creation
//
// End Change Log *****************************************************


//--------------------------------------------------------------------
//  Function Definitions                                               
//--------------------------------------------------------------------
//
//   These are just stubs, used for testing the out the DLL

int dllInitDll() {
  /* This is where we would init any local variables to the dll */
  printf("Stub dll has been initialized\n");
  return ECMD_SUCCESS;
}

int dllCommandArgs(int argc, char* argv[]) {
  return ECMD_SUCCESS;
}


int dllGetRing (ecmdChipTarget & target, const char * ringName, ecmdDataBuffer & data) { return ECMD_SUCCESS; }

int dllPutRing (ecmdChipTarget & target, const char * ringName, ecmdDataBuffer & data) { return ECMD_SUCCESS; }


int dllRingRead (ecmdChipTarget & target, const char * ringName, const char * fileName) { return ECMD_SUCCESS; }

int dllRingWrite (ecmdChipTarget & target, const char * ringName, const char * fileName) { return ECMD_SUCCESS; }


int dllGetScom (ecmdChipTarget & target, uint32_t address, ecmdDataBuffer & data) { return ECMD_SUCCESS; }

int dllPutScom (ecmdChipTarget & target, uint32_t address, ecmdDataBuffer & data) { return ECMD_SUCCESS; }


int dllGetSpy (ecmdChipTarget & target, const char * spyName, ecmdDataBuffer & data) { return ECMD_SUCCESS; }

int dllGetSpyEnum(ecmdChipTarget & target, const char * spyName, string enumValue){ return ECMD_SUCCESS; }

int dllPutSpy (ecmdChipTarget & target, const char * spyName, ecmdDataBuffer & data) { return ECMD_SUCCESS; }

int dllPutSpyEnum(ecmdChipTarget & target, const char * spyName, const string enumValue){ return ECMD_SUCCESS; } 

int dllGetArray (ecmdChipTarget & target, const char * arrayName, uint32_t * address, ecmdDataBuffer & data) { return ECMD_SUCCESS; }

int dllPutArray (ecmdChipTarget & target, const char * arrayName, uint32_t * address, ecmdDataBuffer & data) { return ECMD_SUCCESS; }

int dllQueryConfig(ecmdChipTarget & target, vector<ecmdCageData> & queryData){ return ECMD_SUCCESS; } 

int dllQueryRing(ecmdChipTarget & target, vector<ecmdRingData> & queryData, const char * ringName ){ return ECMD_SUCCESS; }

int dllQueryArray(ecmdChipTarget & target, vector<ecmdArrayData> & queryData, const char * arrayName){ return ECMD_SUCCESS; } 

int dllQuerySpy(ecmdChipTarget & target, vector<ecmdSpyData> & queryData, const char * spyName){ return ECMD_SUCCESS; } 

int dllQueryFileLocation(ecmdChipTarget & target, ecmdFileType_t fileType, string fileLocation){ return ECMD_SUCCESS; } 


int dllFlushSys () { return ECMD_SUCCESS; } 

int dllIplSys () { return ECMD_SUCCESS; }


void dllOutputError(const char* message) {
  printf("DLLSTUBERROR : %s\n",message);
}

void dllOutputWarning(const char* message) {
  printf("DLLSTUBWARNING : %s\n",message);
}

void dllOutput(const char* message) {
  printf(message);
}
