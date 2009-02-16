#ifndef sedcDefines_h
#define sedcDefines_h
/* $Header$ */
// Copyright **********************************************************
//                                                                      
// File sedcDefines.H                                               
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

// Class Description *************************************************
//                                                                      
//  Name:  sedcDefines
//  Base class: 
//
//  Description: 
//  Usage:
//
// End Class Description *********************************************

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                     10/20/03 albertj  Initial Creation
//
// End Change Log *****************************************************

//--------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------
#ifdef ENABLE_MPATROL
 #include <mpatrol.h>
#endif

/*******************************/
/* All the runtimeFlag defines */
/*******************************/
#define RTF_NONE                        0x00000000      ///< No flags to pass in
#define RTF_RETAIN_LATCH_NAME           0x80000000      ///< Save away latchName on read of spydef.  On in the EDC, off in Cronus, etc..
#define RTF_RETAIN_COMMENTS             0x40000000      ///< Save away any pure comment lines.  On in the EDC, off in Cronus, etc..
#define RTF_PRESERVE_EXTRA_LATCH_INFO   0x20000000      ///< Save away the lhsNum, rhsNum and direction.  On in the edc, off in Cronus, etc..
#define RTF_ALL_SCOMS_AVAIL_FOR_SPIES   0x10000000      ///< A hack to tell the scomdef parser that every entry it reads in is available for spy inclusion 
/* Command line switches */
#define RTF_MODEL_BUILD_MODE            0x08000000      ///< The user is model build mode (-mb command line)
#define RTF_UPDATE_SCANDEF_MODE         0x04000000      ///< The user is update scandef mode (-us command line)
#define RTF_UPDATE_ARRAY_MODE           0x02000000      ///< The user is update array mode   (-ua command line)
#define RTF_UPDATE_SPYDEF_MODE          0x01000000      ///< The user is update spydef mode  (-ue command line)
#define RTF_SPYBIN_MODE                 0x00040000      ///< The EDC is being run to generate binary spy output files
#define RTF_VERIFY_MODE                 0x00020000      ///< The EDC is being run to verify the syntax of a spydef
#define RTF_EDC_MODE                    0x00010000      ///< A global define to tell the parser and other components we are running in the EDC, not in Cronus, etc..
#define RTF_BUILD_INVERSION_ALT         0x00008000      ///< Tells parse latch line to care about 1 or 0 and turn on invert bit.  Added for GFW
#define RTF_SBIN_MODE                   0x00000008      ///< Tells the EDC to run the scanring binary module.
#define RTF_ABIN_MODE                   0x00000080      ///< Tells the EDC to run the array binary module.
#define RTF_OVERRIDE_MODE               0x00000040      ///< Tells the EDC to run the init override file module.
/****************************/
/* All the spyState defines */
/****************************/
/* Global Indicators */
#define SPY_SECTION_DEFINE      0xC0000000      ///< Used to indicate either a start or stop, used for some types of checking.
#define SPY_SECTION_START       0x80000000      ///< Used to indicate the start of any section (alias/idial/ring/scom/group/etc.. )
#define SPY_SECTION_END         0x40000000      ///< Used to indicate the end of any section (alias/idial/ring/scom/group/etc.. )
#define SPY_HOLD_LINE           0x10000000      ///< Indicates insertion point for data once retreived from the builder
/* High level define types */
#define SPY_MAJOR_TYPES         0x0F800000      ///< Any of the major types, including the in{} and out{} sections of ecc
#define SPY_ALIAS               0x08000000      ///< Define for a spy of type alias 
#define SPY_MAJOR_TYPES_DIAL    0x06000000      ///< Define for a spy of type either idial or edial, used for some checking
#define SPY_IDIAL               0x04000000      ///< Define for a spy of type idial
#define SPY_EDIAL               0x02000000      ///< Define for a spy of type edial
#define SPY_MAJOR_TYPES_ECC     0x01800000      ///< Define for the in{} or out{} eccptylatches types
#define SPY_ECCIN               0x01000000      ///< Define for the in{} eccptylatches type
#define SPY_ECCOUT              0x00800000      ///< Define for the out{} eccptylatches type
/* Clock states */
#define SPY_CLOCK_ANY           0x000E0000      ///< Define for any of the clock states being on, used for checking
#define SPY_CLOCK_ON            0x00080000      ///< Define for the clocks in the on state define
#define SPY_CLOCK_OFF           0x00040000      ///< Define for the clocks in the off state define
#define SPY_CLOCK_IND           0x00020000      ///< Define for the clocks in the ind state define
/* Other states */
#define SPY_GROUP_BITS          0x00008000      ///< Define for a group (shadow copies)
#define SPY_MINOR_TYPES         0x00006000      ///< Any of the following latch types
#define SPY_RING                0x00004000      ///< Define for a scanring
#define SPY_SCOM                0x00002000      ///< Define for a scom
/* EDC only states */
#define SPY_COMMENT             0x00000080      
#define SPY_ENUM                0x00000040      ///< Defines the start of an enum{} section that contains the enum values for the spy
#define SPY_EPCHECKERS          0x00000020      ///< Defines the start of an ecc{} section that contains the names of ecc tagged to this spy
/* Special Operators */
#define SPY_INVERT              0x00000008      ///< Indicates the data on this line should be inverted
#define SPY_SIGNAL              0x00000004      ///< Indicates a signal ($) was given by figtree.
#define SPY_DEADBITS            0x00000002      ///< Indicates deadbits

/********************************/
/* All the synonymState defines */
/********************************/
#define SYN_SECTION_START       0x80000000      ///< Used to indicate the start of any section
#define SYN_SECTION_END         0x40000000      ///< Used to indicate the end of any section
#define SYN_COMMENT             0x08000000      ///< A line that is a comment only
#define SYN_NAME                0x04000000      ///< Indicates this is the line that has the real spy we want to look up.

/****************************/
/* All the eplState defines */
/****************************/
#define EPL_SECTION_START       0x80000000      ///< Used to indicate either a start or stop, used for some types of checking.
#define EPL_SECTION_END         0x40000000      ///< Used to indicate the end of any section
#define EPL_COMMENT             0x08000000      ///< A line that is a comment only
#define EPL_FUNCTION            0x04000000      ///< The line that has the eccfunc to go look up 
#define EPL_INLATCHES           0x02000000      ///< Defines the location where the in{} latches where found and loaded into the inSpy
#define EPL_OUTLATCHES          0x01000000      ///< Defines the location where the out{} latches where found and loaded into the outSpy
#define EPL_BOTHLATCHES         0x03000000      ///< Used for some types of checking
#define EPL_ALL_FOUND           0x87000000      ///< Used to make sure all the elements we were looking for were found on a read

/****************************/
/* All the ecfState defines */
/****************************/
#define ECF_SECTION_START       0x80000000      ///< Used to indicate either a start or stop, used for some types of checking.
#define ECF_SECTION_END         0x40000000      ///< Used to indicate the end of any section
#define ECF_COMMENT             0x08000000      ///< A line that is a comment only
#define ECF_INBITS              0x04000000      ///< The line that had the number of inbits on it.  The was loaded into the inBits variable.
#define ECF_OUTBITS             0x02000000      ///< The line that had the number of outbits on it.  The was loaded into the outBits variable.
#define ECF_TABLE               0x01000000      ///< Defines a line of the ecc hamming table was found here
#define ECF_ALL_FOUND           0x87000000      ///< Used to make sure all the elements we were looking for were found on a read

/********************************/
/* All the scomdefState defines */
/********************************/
#define SCM_SECTION_START       0x80000000      ///< Used to indicate the start of any section
#define SCM_SECTION_END         0x40000000      ///< Used to indicate the end of any section
#define SCM_COMMENT             0x08000000      ///< A line that is a comment only
#define SCM_ADDRESS             0x04000000      ///< Says I found an address line
#define SCM_NAME                0x02000000      ///< Says I found the name line
#define SCM_CLKDOMAIN           0x01000000      ///< Says I found the clock domain line
#define SCM_STATE               0x00800000      ///< Says I found the state line
#define SCM_DESCRIPTION         0x00400000      ///< Says I found the description section
#define SCM_DEFINITION          0x00200000      ///< Says I found the definition section
#define SCM_LATCHES             0x00100000      ///< Says I found the latches
#define SCM_SCOM                0x00080000      ///< Says I found the scom section
#define SCM_INSPY               0x00040000      ///< Says if it should be included in spy builds
#define SCM_MASK                0x00020000      ///< Says I found a mask line

/*******************************/
/* All the Array State defines */
/*******************************/
#define ARR_NAME        0x80000000
#define ARR_WIDTH       0x40000000
#define ARR_ENTRIES     0x20000000
#define ARR_TYPE        0x10000000
#define ARR_CLKDOMAIN   0x08000000
#define ARR_R_ADDR      0x04000000
#define ARR_W_ADDR      0x02000000
#define ARR_R_DATA      0x01000000
#define ARR_W_DATA      0x00800000
#define ARR_R_CONTROL   0x00400000
#define ARR_W_CONTROL   0x00200000
#define ARR_R_DONE      0x00004000
#define ARR_W_DONE      0x00002000
#define ARR_R_CLOCK     0x00100000
#define ARR_W_CLOCK     0x00080000
#define ARR_TRANSLATION 0x00040000
#define ARR_A_CONTROL   0x00020000
#define ARR_END         0x00010000
#define ARR_COMMENT     0x00008000
#define ARR_REQUIRED (ARR_NAME | ARR_WIDTH | ARR_ENTRIES | ARR_TYPE | ARR_CLKDOMAIN | ARR_A_CONTROL | ARR_END)
#define ARR_READ_REQUIRED (ARR_R_ADDR | ARR_R_DATA | ARR_R_CONTROL | ARR_R_CLOCK)
#define ARR_WRITE_REQUIRED (ARR_W_ADDR | ARR_W_DATA | ARR_W_CONTROL | ARR_W_CLOCK)
#define ARR_SCOM_ONE_READ_REQUIRED (ARR_R_ADDR | ARR_R_DATA | ARR_R_DONE)
#define ARR_SCOM_ONE_WRITE_REQUIRED (ARR_W_ADDR | ARR_W_DATA | ARR_W_DONE)

/******************************/
/* command line input defines */
/******************************/
#define CMDLINE_L2SCANDEF      0x80000000
#define CMDLINE_SCOMDEF        0x40000000
#define CMDLINE_EXTSPY         0x20000000
#define CMDLINE_FIGSPY         0x10000000
#define CMDLINE_FIGDB          0x08000000
#define CMDLINE_ARRAY          0x04000000
#define CMDLINE_NAME           0x02000000
#define CMDLINE_ECLEVEL        0x01000000
#define CMDLINE_OUTPUTPATH     0x00800000
#define CMDLINE_JTAG           0x00400000
#define CMDLINE_COREREDUCE     0x00200000
#define CMDLINE_INVMASK        0x00100000
#define CMDLINE_LIST           0x00080000
#define CMDLINE_KEY            0x00040000
#define CMDLINE_RINGINFO       0x00020000
#define CMDLINE_SELSPYLIST     0x00010000
#define CMDLINE_SELARRLIST     0x00008000
#define CMDLINE_L1SCANDEF      0x00004000
#define CMDLINE_OVERRIDE       0x00002000


/****************/
/* Misc Defines */
/****************/
#define NOT_FOUND std::string::npos    ///< For STL finds
#define WHITESPACE " \t" ///< For STL finds as well
#define SPY_NAME_ALLOWED  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.#$"
#define ENUM_NAME_ALLOWED "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_./:#"
#define ARRAY_TOKENS " \t,{}=:" ///< All the characters to tokenize on for arrays


/****************/
/* Return Codes */
/****************/
/* For now let's define all Interpreter return codes in the 0x2000-0x2FFF range */
#define SEDC_SUCESS                    0x0      ///< Command completed successfully
#define SEDC_COMMAND_LINE_ERROR        0x00002000   ///< Command line input errors
#define SEDC_SCANDEF_FORMAT_ERROR      0x00040000


#endif /* sedcDefines_h */
