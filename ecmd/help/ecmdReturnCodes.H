#ifndef ecmdReturnCodes_h
#define ecmdReturnCodes_h
// Copyright **********************************************************
//                                                                      
// File ecmdReturnCodes.H                                               
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
 * @file ecmdReturnCodes.H
 * @brief All Return Codes for the eCmd Capi
*/


#define ECMD_SUCCESS                    0x0    ///< API Returned Successfully

  /* For now let's define all return codes in the 0x1000-0x1FFF range */

#define ECMD_INVALID_DLL_VERSION        0x1000 ///< Dll Version didn't match the Client version detected
#define ECMD_FAILURE                    0x1010 ///< General Failure occurred in eCMD
#define ECMD_POS_NOT_CONFIGURED         0x1011 ///< Chip target provided was not configured in the system



#endif /* ecmdReturnCodes_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
