#ifndef sedcCommonParser_H
#define sedcCommonParser_H
/* $Header$ */
// Copyright **********************************************************
//                                                                      
// File sedcCommonParser.H                                               
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
//  Name:  sedcCommonParser
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
//                              albertj  Initial Creation
//
// End Change Log *****************************************************

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <sedcCommonClasses.H>

/**
 * @brief Carves up a line in the latch format.  Could be from spydef, scandef or scomdef 
 * @param sedcLatchLine Returns a class with the carved up pieces
 * @param myLine The tokenized line from the spy file to be parsed
 * @param runtimeFlags Possible directives on how to parse
 */
void sedcParseLatchLine(sedcLatchLine &returnLatchLine, sedcFileLine &myLine, unsigned int runtimeFlags);

/**
 * @brief converts a parsed clock state string into the sedcClockState enumeration type   
 * @param clkStateString Parsed clock state string ("Clock-Stop", "Clock-Running", ...)
 * @retval sedcClockState Returns the converted clock state
 */
sedcClockState sedcStringToClkState(const std::string& clkStateString);

/**
 * @brief converts a given clock state into the corresponding string for spy  
 * @param sedcClockState clock state taken from the scandef/scomdef container           
 * @retval string Returns the converted clock state string
 */
std::string sedcClkStateToSpyString(sedcClockState);

/**
 * @brief converts a given clock state into the corresponding string for scandef  
 * @param sedcClockState clock state taken from the scandef/scomdef container           
 * @retval string Returns the converted clock state string
 */
std::string sedcClkStateToScandefString(sedcClockState);

/**
 * @brief converts a given clock state into latch state                
 * @param sedcClockState clock state taken from the scandef/scomdef container           
 * @retval string Returns the clock state converted to a latch state bit mask
 */
unsigned int sedcClkStateToLatchState(sedcClockState);

/**
 * @brief converts a parsed clock state bit map into the sedcClockState enumeration type   
 * @param latchState clock state bitmap as found in latch definitions         
 * @retval sedcClockState Returns the converted clock state
 */
sedcClockState sedcLatchStateToClkState(unsigned int latchState);
#endif /* sedcCommonParser_H */
