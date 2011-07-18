#ifndef fapiStructs_h
#define fapiStructs_h
// Copyright **********************************************************
//                                                                      
// File fapiStructs.H                                               
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
 * @file fapiStructs.H
 * @brief fapi eCMD Extension Structures

 * Extension Owner : John Farrugia
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------


//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------

#define ECMD_FAPI_CAPI_VERSION "1.0"          ///< eCMD FAPI Extension version



namespace fapi
{

/**
 * @brief Enumeration of fapi file types
 */
typedef enum {
    FAPI_FILE_UNKNOWN,       ///< Default for not initialized
    FAPI_FILE_HWP
} FileType_t;



#define FAPI_ATTRIBUTE_TYPE_STRING      0x80000000
#define FAPI_ATTRIBUTE_TYPE_UINT8       0x40000000
#define FAPI_ATTRIBUTE_TYPE_UINT32      0x20000000
#define FAPI_ATTRIBUTE_TYPE_UINT64      0x10000000
#define FAPI_ATTRIBUTE_TYPE_UINT8ARAY   0x08000000
#define FAPI_ATTRIBUTE_TYPE_UINT32ARAY  0x04000000
#define FAPI_ATTRIBUTE_TYPE_UINT64ARAY  0x02000000
/**
 @brief Used by the get/set configuration functions to return the data
*/
struct AttributeData {
  uint32_t              faValidMask;  ///< Indicates which of the fields in this structure are valid
  char *                faString;     ///< If the output data is a char *, will be populated
  uint8_t               faUint8;      ///< If the output data is a uint8_t, will be populated
  uint32_t              faUint32;     ///< If the output data is a uint32_t, will be populated
  uint64_t              faUint64;     ///< If the output data is a uint64_t, will be populated
  uint8_t *             faUint8ary;   ///< If the output data is a uint8_t array, will be populated
  uint32_t *            faUint32ary;  ///< If the output data is a uint32_t array, will be populated
  uint64_t *            faUint64ary;  ///< If the output data is a uint64_t array, will be populated
};


} //namespace
#endif 





