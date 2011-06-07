#ifndef ecmdDataBufferBase_H 
#define ecmdDataBufferBase_H
// Copyright **********************************************************
//                                                                      
// File ecmdDataBufferBase.H                                               
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

/**
 * @file ecmdDataBufferBase.H
 * @brief Provides a means to handle data from the eCMD C API. This is exactly like an ecmdDataBuffer, just using a different name.
 *
 * DataBufferBases handle and store data in a Big Endian fashion with Bit 0 being the MSB
*/


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------

#include <ecmdDataBuffer.H>

typedef ecmdDataBuffer ecmdDataBufferBase;


#endif  /* ecmdDataBufferBase_H */
