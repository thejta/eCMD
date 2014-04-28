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

#include <fapiTarget.H>
#include <fapiSharedUtils.H>
#include <ecmdSharedUtils.H>
#include <ecmdStructs.H>
#include <ecmdReturnCodes.H>


void fapiTargetToEcmdTarget(fapi::Target i_fapiTarget, ecmdChipTarget & o_ecmdTarget){
  ecmdChipTarget * ecmdTargetPtr = reinterpret_cast<ecmdChipTarget *> (i_fapiTarget.get());
  o_ecmdTarget = (*ecmdTargetPtr);
}

void ecmdTargetToFapiTarget(ecmdChipTarget & i_ecmdTarget, fapi::Target & o_fapiTarget){
  o_fapiTarget.set(reinterpret_cast<void *> (&i_ecmdTarget));
  o_fapiTarget.setType(fapi::TARGET_TYPE_NONE);

  /* ChipUnit is the lowest level so lets check it first */
  if (i_ecmdTarget.chipUnitTypeState == ECMD_TARGET_FIELD_VALID)
  {
      if (i_ecmdTarget.chipUnitType == "ex")
      {
         o_fapiTarget.setType(fapi::TARGET_TYPE_EX_CHIPLET);
      } 
      else if (i_ecmdTarget.chipUnitType == "mcs") 
      {
         o_fapiTarget.setType(fapi::TARGET_TYPE_MCS_CHIPLET);
      } 
      else if (i_ecmdTarget.chipUnitType == "mba") 
      {
         o_fapiTarget.setType(fapi::TARGET_TYPE_MBA_CHIPLET);
      } 
      else if (i_ecmdTarget.chipUnitType == "xbus") 
      {
         o_fapiTarget.setType(fapi::TARGET_TYPE_XBUS_ENDPOINT);
      } 
      else if (i_ecmdTarget.chipUnitType == "abus") 
      {
         o_fapiTarget.setType(fapi::TARGET_TYPE_ABUS_ENDPOINT);
      } 
      else if (i_ecmdTarget.chipUnitType == "l4") 
      {
         o_fapiTarget.setType(fapi::TARGET_TYPE_L4);
      } 
      /*
      else if (i_ecmdTarget.chipUnitType == "occ") 
      {
         o_fapiTarget.setType(fapi::TARGET_TYPE_MBS_CHIPLET);
      } 
      */
      else 
      {
         // do nothing for now.  Maybe we should error out?
      }
  }
  else if (i_ecmdTarget.chipTypeState == ECMD_TARGET_FIELD_VALID)
  {
      if (i_ecmdTarget.chipType == "p8") 
      {
         o_fapiTarget.setType(fapi::TARGET_TYPE_PROC_CHIP);
      } 
      else if (i_ecmdTarget.chipType == "s1") 
      {
         o_fapiTarget.setType(fapi::TARGET_TYPE_PROC_CHIP);
      } 
      else if (i_ecmdTarget.chipType == "n1") 
      {
         o_fapiTarget.setType(fapi::TARGET_TYPE_PROC_CHIP);
      } 
      else if (i_ecmdTarget.chipType == "centaur") 
      {
         o_fapiTarget.setType(fapi::TARGET_TYPE_MEMBUF_CHIP);
      } 
  }
  else if (i_ecmdTarget.cageState == ECMD_TARGET_FIELD_VALID)
  {
      o_fapiTarget.setType(fapi::TARGET_TYPE_SYSTEM);
  }
}
