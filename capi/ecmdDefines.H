#ifndef ecmdDefines_H 
#define ecmdDefines_H

// Copyright **********************************************************
//                                                                      
// File ecmdDefines.H                                               
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

/* $Header$ */

/**
 * @file ecmdDefines.H
 * @brief Provides a common place for all compiletime defines
 * NOTE : None of these defines are enabled in the CTE common environment, they are only intended to be
 *   used in a closed environment ZSE,FSP, etc
*/

/* Removes simulation interfaces from api and ecmdDataBuffer */
//#define REMOVE_SIM

/* Removes extra debug/trace information */
//#define ECMD_STRIP_DEBUG

/* Makes api call's static function calls instead of dll symbol lookups, used for building the client
   and plugin together in one application */
//#define ECMD_STATIC_FUNCTIONS

/* Defined when building the eCMD Perlapi, can be used to remove functions from the perl module */
//#define ECMD_PERLAPI

/* Defined when generating doxygen documentation, can be used to removed functions from 
   public documentation */
//#define DOCUMENTATION





/* Turned on only in a GIP FSP build environment */
//#define FIPSODE

/* Turned on only in ZSE build environment */
//#define ZSERIES_SWITCH

/* Turned on in a BML Build environment only */
//#define BML

/* Used to enable mpatrol memory debugging */
//#define ENABLE_MPATROL

#endif
