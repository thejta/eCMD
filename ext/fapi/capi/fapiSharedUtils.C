/* $Header$ */
// Copyright ***********************************************************
//
// File ecmdSharedUtils.C
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
// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder     Description                       
//  ---- -------- ---- -------- -----     -----------------------------
//   
// End Change Log *****************************************************


//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <string>
#include <vector>
#include <stdio.h>
#include <fstream>

#include <fapiSharedUtils.H>
#include <ecmdStructs.H>
#include <ecmdReturnCodes.H>


void fapiTargetToEcmdTarget(fapi::Target i_fapiTarget, ecmdChipTarget & o_ecmdTarget){
  ecmdChipTarget * ecmdTargetPtr = reinterpret_cast<ecmdChipTarget *> (i_fapiTarget.get());
  o_ecmdTarget = (*ecmdTargetPtr);
}

void ecmdTargetToFapiTarget(ecmdChipTarget & i_ecmdTarget, fapi::Target & o_fapiTarget){
  o_fapiTarget.set(reinterpret_cast<void *> (&i_ecmdTarget));
}
