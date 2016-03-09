// Copyright ***********************************************************
//                                                                      
// File ecmdPnorUser.C                                  
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
// End Copyright *******************************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <fstream>

#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <ecmdInterpreter.H>
#include <ecmdSharedUtils.H>

//----------------------------------------------------------------------
//  User Types
//----------------------------------------------------------------------


//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Macros
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Internal Function Prototypes
//----------------------------------------------------------------------


//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------
#ifndef ECMD_REMOVE_PNOR_FUNCTIONS
uint32_t ecmdGetPnorUser(int argc, char * argv[]) {

    uint32_t rc = ECMD_SUCCESS , coeRc = ECMD_SUCCESS;
    ecmdChipTarget l_target;
    ecmdLooperData looperdata;            ///< Store internal Looper data
    std::string outputformat = "xl";      ///< Output Format to display
    ecmdDataBuffer data;                  ///< Data from the module vpd record
    bool validPosFound = false;           ///< Did the looper find anything to execute on
    
    /************************************************************************/
    /* Parse Local FLAGS here!                                              */
    /************************************************************************/

    /* Get the filename if -fb is specified */
    char * filename = ecmdParseOptionWithArgs(&argc, &argv, "-fb");
    std::string printed;

    bool l_listPnor = ecmdParseOption( &argc, &argv, "--list" );

    if( (filename != NULL) && (l_listPnor) ) 
    {
        printed = "getvpdkeyword - Options '-fb' cannot be specified with '--list' option.\n";
        ecmdOutputError(printed.c_str());
        return ECMD_INVALID_ARGS;
    } 

    /************************************************************************/
    /* Parse Common Cmdline Args                                            */
    /************************************************************************/
    rc = ecmdCommandArgs(&argc, &argv);
    if (rc) return rc;

    /* Global args have been parsed, we can read if -coe was given */
    bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

    /************************************************************************/
    /* Parse Local ARGS here!                                               */
    /************************************************************************/
    if ( !l_listPnor && argc < 5) 
    {  
        ecmdOutputError("getvpdkeyword - Too few arguments specified; you need at least a chip/rid, vpdtype, recordname, keyword and numbytes.\n");
        ecmdOutputError("getvpdkeyword - Type 'getvpdkeyword -h' for usage.\n");
        return ECMD_INVALID_ARGS;
    } 
    else if (argc > 5) 
    {
        ecmdOutputError("getvpdkeyword - Too many arguments specified; you only need chip/rid, vpdtype, recordname, keyword and numbytes.\n");
        ecmdOutputError("getvpdkeyword - Type 'getvpdkeyword -h' for usage.\n");
        return ECMD_INVALID_ARGS;
    }

    l_target.cageState = l_target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
    l_target.posState = l_target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
    l_target.slotState = l_target.chipUnitTypeState = l_target.chipUnitNumState = l_target.threadState = ECMD_TARGET_FIELD_UNUSED;

    rc = ecmdLooperInit(l_target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
    if (rc) return rc;
              
    if ( l_listPnor )
    {
        while ( ecmdLooperNext(l_target, looperdata) ) 
        {
            validPosFound = true;
            ecmdPnorListData myPnorData;

            rc = ecmdGetPnorList( l_target, "*", 0, myPnorData );
            if ( rc ) break;
        }
    }

    // This is an error common across all UI functions
    if (!validPosFound) {
        ecmdOutputError("getpnor - Unable to find a valid target to execute command on\n");
        return ECMD_TARGET_NOT_CONFIGURED;
    }

    return rc;
}

uint32_t ecmdPutPnorUser(int argc, char * argv[]) {

    uint32_t rc = ECMD_SUCCESS , coeRc = ECMD_SUCCESS;
    ecmdLooperData looperdata;            ///< Store internal Looper data
    ecmdChipTarget target;                ///< Current target operating on
    ecmdDataBuffer vpdImage;              ///< buffer to load a VPD image into, if given

    /************************************************************************/
    /* Parse Local FLAGS here!                                              */
    /************************************************************************/
    char * fbFileName = ecmdParseOptionWithArgs(&argc, &argv, "-fb");
    
    /************************************************************************/
    /* Parse Common Cmdline Args                                            */
    /************************************************************************/
    rc = ecmdCommandArgs(&argc, &argv);
    if (rc) return rc;

    std::string chipType, chipUnitType;
    ecmdParseChipField(argv[0], chipType, chipUnitType);
    if ( chipType == "nochip" )
    {
        target.chipTypeState = target.posState = ECMD_TARGET_FIELD_UNUSED;
    }
    else
    {
        target.chipType = chipType;
        target.chipTypeState = ECMD_TARGET_FIELD_VALID;
        target.posState = ECMD_TARGET_FIELD_WILDCARD;
    }
    target.cageState = target.nodeState = target.slotState = ECMD_TARGET_FIELD_WILDCARD;
    target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

    rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
    if ( rc ) return rc;

    while( ecmdLooperNext(target, looperdata) )
    {
        //ecmdPutPnor(target, data);
    }

    return rc;
}
#endif // ECMD_REMOVE_PNOR_FUNCTIONS

