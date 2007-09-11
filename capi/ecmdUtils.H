#ifndef ecmdUtils_H
#define ecmdUtils_H
/* $Header$ */
// Copyright **********************************************************
//                                                                      
// File ecmdUtils.H                                               
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
 * @file ecmdUtils.H
 * @brief Useful functions for use throughout the ecmd C API
 *
 */


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <inttypes.h>
#include <string>
#include <vector>

#include <ecmdDefines.H>
#include <ecmdClientCapi.H>
#ifdef FIPSODE
#include <tracinterface.H>
#endif

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------
#ifdef FIPSODE
extern tracDesc_t g_ptrc; /** Procedure Trace Descriptor **/
#endif

//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------

//--------------------------------------------------------------------
// Macros
//--------------------------------------------------------------------

#ifndef ECMD_PERLAPI

#ifdef FIPSODE
#define PTRAC0(fmt) \
  TRACFCOMP0(g_ptrc, fmt);
#define PTRAC1(fmt, arg1) \
  TRACFCOMP1(g_ptrc, fmt, arg1)
#define PTRAC2(fmt, arg1, arg2) \
  TRACFCOMP2(g_ptrc, fmt, arg1, arg2)
#define PTRAC3(fmt, arg1, arg2, arg3) \
  TRACFCOMP3(g_ptrc, fmt, arg1, arg2, arg3)
#define PTRAC4(fmt, arg1, arg2, arg3, arg4) \
  TRACFCOMP4(g_ptrc, fmt, arg1, arg2, arg3, arg4)
#define PTRAC5(fmt, arg1, arg2, arg3, arg4, arg5) \
  TRACFCOMP5(g_ptrc, fmt, arg1, arg2, arg3, arg4, arg5)
#define PTRAC6(fmt, arg1, arg2, arg3, arg4, arg5, arg6) \
  TRACFCOMP6(g_ptrc, fmt, arg1, arg2, arg3, arg4, arg5, arg6)
#define PTRAC7(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
  TRACFCOMP7(g_ptrc, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
  arg7)
#define PTRAC8(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
  TRACFCOMP8(g_ptrc, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
  arg7, arg8)
#define PTRAC9(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
  TRACFCOMP9(g_ptrc, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
  arg7, arg8, arg9)
#else
#define PTRAC0(fmt) \
  {char buffer [255]; \
  snprintf( buffer, 255, "%s> PTRC: "fmt, __FUNCTION__); \
  ecmdOutput(buffer);}
#define PTRAC1(fmt, arg1) \
  {char buffer [255]; \
  snprintf( buffer, 255, "%s> PTRC: "fmt, __FUNCTION__, arg1); \
  ecmdOutput(buffer);}
#define PTRAC2(fmt, arg1, arg2) \
  {char buffer [255]; \
  snprintf( buffer, 255, "%s> PTRC: "fmt, __FUNCTION__, arg1, arg2); \
  ecmdOutput(buffer);}
#define PTRAC3(fmt, arg1, arg2, arg3) \
  {char buffer [255]; \
  snprintf( buffer, 255, "%s> PTRC: "fmt, __FUNCTION__, arg1, arg2, arg3); \
  ecmdOutput(buffer);}
#define PTRAC4(fmt, arg1, arg2, arg3, arg4) \
  {char buffer [255]; \
  snprintf( buffer, 255, "%s> PTRC: "fmt, __FUNCTION__, arg1, arg2, arg3, \
  arg4); \
  ecmdOutput(buffer);}
#define PTRAC5(fmt, arg1, arg2, arg3, arg4, arg5) \
  {char buffer [255]; \
  snprintf( buffer, 255, "%s> PTRC: "fmt, __FUNCTION__, arg1, arg2, arg3, arg4, \
  arg5); \
  ecmdOutput(buffer);}
#define PTRAC6(fmt, arg1, arg2, arg3, arg4, arg5, arg6) \
  {char buffer [255]; \
  snprintf( buffer, 255, "%s> PTRC: "fmt, __FUNCTION__, arg1, arg2, arg3, arg4, \
  arg5, arg6); \
  ecmdOutput(buffer);}
#define PTRAC7(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
  {char buffer [255]; \
  snprintf( buffer, 255, "%s> PTRC: "fmt, __FUNCTION__, arg1, arg2, arg3, arg4, \
  arg5, arg6, arg7); \
  ecmdOutput(buffer);}
#define PTRAC8(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
  {char buffer [255]; \
  snprintf( buffer, 255, "%s> PTRC: "fmt, __FUNCTION__, arg1, arg2, arg3, arg4, \
  arg5, arg6, arg7, arg8); \
  ecmdOutput(buffer);}
#define PTRAC9(fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
  {char buffer [255]; \
  snprintf( buffer, 255, "%s> PTRC: "fmt, __FUNCTION__, arg1, arg2, arg3, arg4, \
  arg5, arg6, arg7, arg8, arg9); \
  ecmdOutput(buffer);}
#endif

#endif // ifndef ECMD_PERLAPI
//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  User Types
//----------------------------------------------------------------------
/**
 @brief Used internally by ecmdConfigLooper to store looping state information
*/
struct ecmdLooperData {
  bool             ecmdLooperInitFlag;                          ///< Is fresh ?
  bool             ecmdUseUnitid;                               ///< This looper is looping on unitid targets not config data
  ecmdConfigLoopMode_t ecmdLoopMode;                            ///< Is this a variable depth or static loop?

  ecmdQueryData    ecmdSystemConfigData;                        ///< Config data queried from the system
  std::list<ecmdCageData>::iterator ecmdCurCage;                ///< Pointer to current Cage
  std::list<ecmdNodeData>::iterator ecmdCurNode;                ///< Pointer to current Node
  std::list<ecmdSlotData>::iterator ecmdCurSlot;                ///< Pointer to current Slot
  std::list<ecmdChipData>::iterator ecmdCurChip;                ///< Pointer to current Chip
  std::list<ecmdChipUnitData>::iterator ecmdCurChipUnit;        ///< Pointer to current Core
  std::list<ecmdThreadData>::iterator ecmdCurThread;            ///< Pointer to current Thread
  ecmdChipTarget prevTarget;                                    ///< Pointer to previous target


  std::list<ecmdChipTarget>     unitIdTargets;                  ///< List of targets if looping on a unitid
  std::list<ecmdChipTarget>::iterator curUnitIdTarget;          ///< Pointer to current unitid target
};

#ifndef DOCUMENTATION
#if !defined(ECMD_STRIP_DEBUG) && !defined(ECMD_PERLAPI)
/**
 @brief Used by the eCMD Function parm trace printer
*/
typedef enum {
  ECMD_FPP_RETOUT,                      ///< Designates the failing return code out of the api function
  ECMD_FPP_FUNCTIONIN,                  ///< Designates the call in of the api function
  ECMD_FPP_FUNCTIONOUT,                 ///< Designatest the call out of the api function
  ECMD_FPP_JUSTIN,                      ///< Designatest the call in of the api function in debug 8 mode
  ECMD_FPP_JUSTOUT                      ///< Designatest the call out of the api function in debug 8 mode and rc==0
} efppInOut_t;

/**
 @brief Used by the eCMD Function Timer
*/
typedef enum {
  ECMD_TMR_FUNCTIONIN,                  ///< Designates the call in of the api function
  ECMD_TMR_FUNCTIONOUT,                 ///< Designates the call out of the api function
  ECMD_TMR_LOADDLL,                     ///< Load of the DLL
  ECMD_TMR_UNLOADDLL                    ///< Unload of the DLL
} etmrInOut_t;
#endif
#endif



/** @name eCMD Utility Functions */
//@{

/**
 @brief Initializes data structures and code to loop over configured and selected elements of the system
 @param io_target Initial ecmdChipTarget that may contain information used in building the struct to loop over
 @param i_looptype Specify type of all, all chips in system or all chips selected by user
 @param i_mode Specify if you want to allow the plugin to change the depth of your loop or not
 @param io_state Used internally by ConfigLooper to keep track of state, unique instance must be passed into each loop and must be passed to ecmdConfigLooperNext
 @retval ECMD_SUCCESS if initialization succeeded, error code if otherwise
 @see ecmdConfigLooperNext

 

 TARGET DEPTH  : Thread
 TARGET STATES : Must Be Initialized
 */
uint32_t ecmdConfigLooperInit (ecmdChipTarget & io_target, ecmdConfigLoopType_t i_looptype, ecmdLooperData& io_state, ecmdConfigLoopMode_t i_mode = ECMD_STATIC_DEPTH_LOOP);

/**
 @brief Loops over configured and selected elements of the system, updating target to point to them
 @param io_target ecmdChipTarget that contains info about next target to process
 @param io_state Used internally to keep track of state, must be passed from output of ecmdConfigLooperInit
 @retval 1 if io_target is valid, 0 if it is not
 @see ecmdConfigLooperInit

 TARGET DEPTH  : Thread
 TARGET STATES : Must be Initialized (from ecmdConfigLooperInit)
 */
uint32_t ecmdConfigLooperNext (ecmdChipTarget & io_target, ecmdLooperData& io_state);


/**
 @brief Reads data from data string into data buffer based on a format type
 @retval ECMD_SUCCESS if data is well-formatted, non-zero otherwise
 @param o_data ecmdDataBuffer where data from data string is placed.
 @param i_dataStr string of characters containing data
 @param i_format Flag that tells how to parse the data string, e.g., "b" = binary, "x" = hex left
 @param i_expectedLength If length of data is known before hand , should be passed is necessary for right aligned data that is not byte aligned lengths
 */
uint32_t ecmdReadDataFormatted (ecmdDataBuffer & o_data, const char * i_dataStr, std::string i_format, int i_expectedLength = 0);

/**
 @brief Converts decimal string to uint32_t
 @retval uint32_t value of converted input string
 @param i_decstr string of characters containing data
 */
uint32_t decToUInt32(const char *i_decstr);

/**
 @brief Formats data from data buffer into a string according to format flag and returns the string
 @return String of formatted data
 @param i_data ecmdDataBuffer where data to format is stored
 @param i_format Flag that tells how to parse the data into a string, e.g., "b" = binary, "x" = hex left
 @param i_address A base address value that can be used in formating certain data- i.e., data from memory
 */
std::string ecmdWriteDataFormatted (ecmdDataBuffer & i_data, std::string i_format, uint64_t i_address = 0);

/**
 @brief Print the bits header used in the output formats
 @param i_initCharOffset char offset on screen to start printing
 @param i_blockSize Binary block size (ie. column char size)
 @param i_numCols Number of columns to display
 @param i_maxBitWidth Maximum number of bits to display - this is actual data valid so we don't display more columns then we need
 @return String of formatted data
*/
std::string ecmdBitsHeader (int i_initCharOffset, int i_blockSize, int i_numCols, int i_maxBitWidth);

/**
 @brief Function calls ecmdQueryDllInfo and displays the output to stdout
 @retval ECMD_SUCCESS if successful
 @retval nonzero on failure
*/
uint32_t ecmdDisplayDllInfo();

/**
 @brief Returns a formatted string containing the data in the given ecmdChipTarget
 @return String with formatted target data
 @param i_target ecmdChipTarget containing data to format into string
 @param i_displayMode Mode to format data

 TARGET DEPTH  : cage, node, slot, pos, core, thread
 TARGET STATES : Must be Initialized
 */
std::string ecmdWriteTarget(ecmdChipTarget & i_target, ecmdTargetDisplayMode_t i_displayMode = ECMD_DISPLAY_TARGET_QUERY_PLUGIN);


/**
 @brief Reads in a target string, returns a filled in target
 @return String with formatted target data
 @param i_targetStr String in a "k0:nall:s0:pu:p0" format to be turned into a target
 @param o_target ecmdChipTarget initialized based upon i_targetStr
 */
uint32_t ecmdReadTarget(std::string i_targetStr, ecmdChipTarget & o_target);

#ifndef FIPSODE      
/**
 @brief Display the Scom data with the bit descriptions.
 @retval ECMD_SUCCESS if scom lookup and display was successful, non-zero otherwise
 @param i_target target for which scom data needs to be displayed.
 @param i_address Scom Address for which the details are required
 @param i_data buffer that holds the scom data
 @param i_format posible values -v, -vs0, -vs1 for the information that needs to be displayed
 @param o_strData If passed in, then we load o_strData with what is printed to the screen
 */
uint32_t ecmdDisplayScomData(ecmdChipTarget & i_target, uint32_t i_address, ecmdDataBuffer & i_data, const char* i_format, std::string *o_strData = NULL);
#endif

//@}

/***** INTERNAL eCMD UTILITY FUNCTIONS NOT FOR CLIENT USE ******/


#ifndef DOCUMENTATION
#if !defined(ECMD_STRIP_DEBUG) && !defined(ECMD_PERLAPI)
/**
 @brief Print the parameters pass to a given function.
 @param tCount is the trace count.
 @param inOut Tell Parm Printer if we are going into or out of the function
 @param fprototypeStr function prototype
 @param args function argument vector 
 */
void ecmdFunctionParmPrinter(int tCount, efppInOut_t inOut, const char * fprototypeStr, std::vector < void * > args);

/**
 @brief Time the spent inside of a dll call
 @param i_myTcount What step we are on
 @param i_timerState Going in or out
 @param i_funcName Name of the function
*/
void ecmdFunctionTimer(int32_t &i_myTcount, etmrInOut_t i_timerState, const char * i_funcName);
#endif

/**
 @brief Registers an extensions initstate pointer defect #18081
 @param i_initState Pointer to initState static so it can be reset later
 */
void ecmdRegisterExtensionInitState(bool* i_initState);

/**
 @brief Reset Extension initstate pointer to uninitialized
*/
void ecmdResetExtensionInitState();


#endif

/***** END INTERNAL eCMD UTILITY FUNCTIONS NOT FOR CLIENT USE ******/


#endif
