#ifndef fapiSharedUtils_h
#define fapiSharedUtils_h
// Copyright **********************************************************
//
// File fapiSharedUtils.H
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
// End Copyright ******************************************************


/**
 * @file fapiSharedUtils.H
 * @brief Useful functions for use throughout the fapi C API and Plugin
 *
 */


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <string>
#include <vector>
#include <inttypes.h>

#include <ecmdDefines.H>
#include <ecmdDataBuffer.H>
#include <ecmdStructs.H>
#include <fapiTarget.H>


/** @name Misc Functions */
//@{

/**
 * @brief Turns a fapi target (fapi::Target) into an eCMD chip target (ecmdChipTarget)
 * @param i_fapiTarget fapi::Target to convert
 * @param o_ecmdTarget generated ecmdChipTarget 
*/
void fapiTargetToEcmdTarget(fapi::Target i_fapiTarget, ecmdChipTarget & o_ecmdTarget);

/**
 * @brief Turns an eCMD chip target (ecmdChipTarget) into a fapi target (fapi::Target)
 * @param i_ecmdTarget ecmdChipTarget to convert
 * @param o_fapiTarget generated fapi::Target
*/
void ecmdTargetToFapiTarget(ecmdChipTarget & i_ecmdTarget, fapi::Target & o_fapiTarget);

//@}


#endif /* fapiSharedUtils_h */
