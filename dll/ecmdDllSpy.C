// Copyright ***********************************************************
//                                                                      
// File ecmdClientSpy.C                                   
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
/* $Header$ */
// Module Description **************************************************
//
// Description: Functions to handle spies on top of eCMD
//
// End Module Description **********************************************

/* This source only gets included for ecmdDll's that don't want to implement spy handling */
#ifdef USE_ECMD_COMMON_SPY

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define ecmdClientSpy_C

#include <ecmdDllCapi.H>

/* Grab the includes for the engineering data compiler */
#include <sedcDataContainer.H>
#include <sedcStructs.H>
#include <sedcDefines.H>
#include <sedcParser.H>

#undef ecmdClientSpy_C
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

/**
  This function specification is the same as defined in ecmdClientCapi.H as ecmdQuerySpy
*/
int dllQuerySpy(ecmdChipTarget & i_target, ecmdSpyData & o_queryData, const char * i_spyName) {
  int rc = ECMD_SUCCESS;

  return rc;
}
/**
  This function specification is the same as defined in ecmdClientCapi.H as getSpy
*/
int dllGetSpy (ecmdChipTarget & i_target, const char * i_spyName, ecmdDataBuffer & o_data){
  int rc = ECMD_SUCCESS;

  return rc;
}

/**
  This function specification is the same as defined in ecmdClientCapi.H as GetSpyEnum
*/
int dllGetSpyEnum (ecmdChipTarget & i_target, const char * i_spyName, std::string & o_enumValue){
  int rc = ECMD_SUCCESS;

  return rc;
}


/**
  This function specification is the same as defined in ecmdClientCapi.H as GetSpyEccGrouping
*/
int dllGetSpyEccGrouping (ecmdChipTarget & i_target, const char * i_spyEccGroupName, ecmdDataBuffer & o_groupData, ecmdDataBuffer & o_eccData, ecmdDataBuffer & o_eccErrorMask){
  int rc = ECMD_SUCCESS;

  return rc;
}


/**
  This function specification is the same as defined in ecmdClientCapi.H as PutSpy
*/
int dllPutSpy (ecmdChipTarget & i_target, const char * i_spyName, ecmdDataBuffer & i_data){
  int rc = ECMD_SUCCESS;

  return rc;
}


/**
  This function specification is the same as defined in ecmdClientCapi.H as PutSpyEnum
*/
int dllPutSpyEnum (ecmdChipTarget & i_target, const char * i_spyName, const std::string i_enumValue){
  int rc = ECMD_SUCCESS;

  return rc;
}



#endif

