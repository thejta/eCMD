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

typedef enum {

  ECMD_SUCCESS = 0x0

  /* For now let's define all return codes in the 0x1000-0x1FFF range */
  /* ex ECMD_FAILURE = 0x1010; */

} ecmdrc_t;


#endif /* ecmdReturnCodes_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
