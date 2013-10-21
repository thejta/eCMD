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
#include <string>


//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------

#define ECMD_FAPI_CAPI_VERSION "1.0"          ///< eCMD FAPI Extension version



#ifndef ECMD_PERLAPI

namespace fapi
{

/**
 * @brief Enumeration of fapi file types
 */
typedef enum {
    FAPI_FILE_UNKNOWN,       ///< Default for not initialized
    FAPI_FILE_HWP
} FileType_t;


enum AttributeSource
{
    FAPI_ATTRIBUTE_SOURCE_UNKNOWN = 0x00000000,
    FAPI_ATTRIBUTE_SOURCE_PLAT = 0x00000001,
    FAPI_ATTRIBUTE_SOURCE_HWP = 0x00000002,
};


#define FAPI_ATTRIBUTE_TYPE_STRING      0x80000000
#define FAPI_ATTRIBUTE_TYPE_UINT8       0x40000000
#define FAPI_ATTRIBUTE_TYPE_UINT32      0x20000000
#define FAPI_ATTRIBUTE_TYPE_UINT64      0x10000000
#define FAPI_ATTRIBUTE_TYPE_UINT8ARY    0x04000000
#define FAPI_ATTRIBUTE_TYPE_UINT32ARY   0x02000000
#define FAPI_ATTRIBUTE_TYPE_UINT64ARY   0x01000000

#define FAPI_ATTRIBUTE_MODE_CONST       0x80000000
/**
 @brief Used by the get/set configuration functions to return the data
*/
struct AttributeData {
  // Constructor
  AttributeData();

  // Destructor 
  ~AttributeData();

  // Methods  
  void printStruct(void);

  // Members
  uint32_t              faValidMask;  ///< Indicates which of the fields in this structure are valid
  char *                faString;     ///< If the output data is a char *, will be populated
  uint8_t               faUint8;      ///< If the output data is a uint8_t, will be populated
  uint32_t              faUint32;     ///< If the output data is a uint32_t, will be populated
  uint64_t              faUint64;     ///< If the output data is a uint64_t, will be populated
  uint8_t *             faUint8ary;   ///< If the output data is a uint8_t array, will be populated
  uint32_t *            faUint32ary;  ///< If the output data is a uint32_t array, will be populated
  uint64_t *            faUint64ary;  ///< If the output data is a uint64_t array, will be populated
  std::string           faName;       ///< Name of attribute (use sparingly).    
  bool                  faEnumUsed;   ///< Indicates if an enum was used or not
  uint8_t               faNumOfArrayDim; //< Indicates how many dimensions the array is (4 max currently)
  uint8_t               faSizeOfArrayDimW; //< Indicates how large dimension W is
  uint8_t               faSizeOfArrayDimX; //< Indicates how large dimension X is
  uint8_t               faSizeOfArrayDimY; //< Indicates how large dimension Y is
  uint8_t               faSizeOfArrayDimZ; //< Indicates how large dimension Z is
  uint32_t              faMode; //< Indicates a specific mode this attribute is in
};

inline AttributeData::AttributeData():
// mask
faValidMask(0x0), 
// char * 
faString("blah"),   
// integers
faUint8(0x0),         faUint32(0x0),       faUint64(0x0),  
// arrays
faUint8ary(NULL),     faUint32ary(NULL),   faUint64ary(NULL),
// name of attribute in string form
faName("blah"),       
// enum 
faEnumUsed(false), 
// array information
faNumOfArrayDim(0),
faSizeOfArrayDimW(0), faSizeOfArrayDimX(0),
faSizeOfArrayDimY(0), faSizeOfArrayDimZ(0),
faMode(0x0)
{}

inline AttributeData::~AttributeData() {}

} //namespace
#endif // #ifndef ECMD_PERLAPI
#endif 





