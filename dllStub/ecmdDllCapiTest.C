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

int dllGetRing (ecmdChipTarget & target, const char * ringName, ecmdDataBuffer & data) { return ECMD_SUCCESS; }

int dllPutRing (ecmdChipTarget & target, const char * ringName, ecmdDataBuffer & data) { return ECMD_SUCCESS; }


int dllRingRead (ecmdChipTarget & target, const char * ringName, const char * fileName) { return ECMD_SUCCESS; }

int dllRingWrite (ecmdChipTarget & target, const char * ringName, const char * fileName) { return ECMD_SUCCESS; }


int dllGetScom (ecmdChipTarget & target, uint32_t address, ecmdDataBuffer & data) { return ECMD_SUCCESS; }

int dllPutScom (ecmdChipTarget & target, uint32_t address, ecmdDataBuffer & data) { return ECMD_SUCCESS; }


int dllGetSpy (ecmdChipTarget & target, const char * spyName, ecmdDataBuffer & data) { return ECMD_SUCCESS; }

int dllGetSpyEnum (ecmdChipTarget & target, const char * spyName, char * enumValue) { return ECMD_SUCCESS; }

int dllPutSpy (ecmdChipTarget & target, const char * spyName, ecmdDataBuffer & data) { return ECMD_SUCCESS; }

int dllPutSpyEnum (ecmdChipTarget & target, const char * spyName, ecmdDataBuffer & data) { return ECMD_SUCCESS; }


int dllGetArray (ecmdChipTarget & target, const char * arrayName, uint32_t * address, ecmdDataBuffer & data) { return ECMD_SUCCESS; }

int dllPutArray (ecmdChipTarget & target, const char * arrayName, uint32_t * address, ecmdDataBuffer & data) { return ECMD_SUCCESS; }


int dllFlushSys () { return ECMD_SUCCESS; } 

int dllIplSys () { return ECMD_SUCCESS; }

