#ifndef ecmdPluginExtensionSupport_h
#define ecmdPluginExtensionSupport_h
// Copyright **********************************************************
//                                                                      
// File ecmdPluginExtensionSupport.H                                               
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
/* $Header$ */

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------


//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------

/* These defines what extensions are supported by this plugin, they can only be disabled when building common code in an plugin specific environment like the FSP or SE */


/* The Cronus/IP GFW Extension */
#define ECMD_CIP_EXTENSION_SUPPORT

/* The Cronus Extension */
#define ECMD_CRO_EXTENSION_SUPPORT

/* The Eclipz IP/Cronus Extension */
#define ECMD_EIP_EXTENSION_SUPPORT

/* The GFW IP Extension */
#define ECMD_GIP_EXTENSION_SUPPORT

/* The Z Series Extension */
#define ECMD_ZSE_EXTENSION_SUPPORT

/* The BML Extension */
//#define ECMD_BML_EXTENSION_SUPPORT

#endif /* ecmdPluginExtensionSupport_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
