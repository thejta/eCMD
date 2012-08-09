/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File fapiMiscUser.C                                   
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
// End Copyright *******************************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <stdio.h>
#include <ctype.h>

#include <ecmdClientCapi.H>
#include <fapiClientCapi.H>
#include <fapiInterpreter.H>
#include <fapiSharedUtils.H>
#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <ecmdSharedUtils.H>
#include <algorithm>

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

uint32_t fapiGetAttributeUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  std::string attributeName;    ///< Name of attribute to fetch
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  ecmdDataBuffer numData;       ///< Initialise data buffer with the numeric value
  std::string printed;          ///< Print Buffer
  ecmdLooperData looperData;    ///< Store internal Looper data

  int CAGE = 1, NODE = 2, SLOT = 3, POS = 4, CHIPUNIT = 5;
  int depth = 0;                 ///< depth found from Command line parms

  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr == NULL) {
    format = "x";
  } else {
    format = formatPtr;
  }

  if (ecmdParseOption(&argc, &argv, "-dk"))             depth = CAGE;
  else if (ecmdParseOption(&argc, &argv, "-dn"))        depth = NODE;
  else if (ecmdParseOption(&argc, &argv, "-ds"))        depth = SLOT;
  else if (ecmdParseOption(&argc, &argv, "-dp"))        depth = POS;
  else if (ecmdParseOption(&argc, &argv, "-dc"))        depth = CHIPUNIT;

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
  if (argc < 1) {
    ecmdOutputError("fapigetattr - Too few arguments specified; you need at least an attribute.\n");
    ecmdOutputError("fapigetattr - Type 'fapigetattr -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }


  //Setup the target that will be used to query the system config
  if (argc > 2) {
    ecmdOutputError("fapigetattr - Too many arguments specified; you probably added an unsupported option.\n");
    ecmdOutputError("fapigetattr - Type 'fapigetattr -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if (argc == 2) {
    std::string chipType, chipUnitType;
    rc = ecmdParseChipField(argv[0], chipType, chipUnitType);
    if (rc) { 
      ecmdOutputError("fapigetattr - Wildcard character detected however it is not supported by this command.\n");
      return rc;
    }

    /* Error check */
    if (depth) {
      if (chipUnitType == "" && depth < POS) {
        ecmdOutputError("fapigetattr - Invalid Depth parm specified when a chip was specified.  Try with -dp.\n");
        return ECMD_INVALID_ARGS;
      }

      if (chipUnitType != "" && depth < CHIPUNIT) {
        ecmdOutputError("fapigetattr - Invalid Depth parm specified when a chipUnit was specified.  Try with -dc.\n");
        return ECMD_INVALID_ARGS;
      }
    } else { /* No depth, set on for the code below */
      if (chipUnitType == "") {
        depth = POS;
      } else {
        depth = CHIPUNIT;
      }
    }
    target.chipType = chipType;
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
    if (chipUnitType != "") {
      target.chipUnitType = chipUnitType;
      target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
    }
    attributeName = argv[1];
  } else {
    if (depth == 0) depth = CAGE;
    attributeName = argv[0];
  }

  /* Check that attribute name is valid */
  fapi::AttributeId attributeId;
  rc = fapiAttributeStringToId(attributeName, attributeId);
  if (rc) {
    printed = "fapigetattr - Unknown attribute name (";
    printed += attributeName + ")\n";
    ecmdOutputError( printed.c_str() );
    return ECMD_INVALID_ARGS;
  }

  /* Determine attribute type and size */
  uint32_t attributeType = 0;
  uint32_t l_numOfEntries;
  uint32_t l_numOfBytes;
  bool attributeEnum = false;
  rc = fapiGetAttrInfo(attributeId, attributeType, l_numOfEntries, l_numOfBytes, attributeEnum);
  if (rc) {
    printed = "fapigetattr - Unknown attribute type (";
    printed += attributeName + ")\n";
    ecmdOutputError( printed.c_str() );
    return ECMD_INVALID_ARGS;
  }

  fapi::AttributeData attributeData;
  attributeData.faValidMask = attributeType;
  attributeData.faEnumUsed = attributeEnum;

  if (attributeType == FAPI_ATTRIBUTE_TYPE_UINT8ARY)
  {
      attributeData.faUint8ary = new uint8_t[l_numOfEntries];
  }
  else if (attributeType == FAPI_ATTRIBUTE_TYPE_UINT32ARY)
  {
      attributeData.faUint32ary = new uint32_t[l_numOfEntries];
  }
  else if (attributeType == FAPI_ATTRIBUTE_TYPE_UINT64ARY)
  {
      attributeData.faUint64ary = new uint64_t[l_numOfEntries];
  }

  /* Now set our states based on depth */
  target.cageState = target.nodeState = target.slotState = target.posState = target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;
  if (depth == POS) {
    target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;
    target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
  } else if (depth == SLOT) {
    target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
    target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;
    target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
  } else if (depth == NODE) {
    target.slotState = ECMD_TARGET_FIELD_UNUSED;
    target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
    target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;
    target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
  } else if (depth == CAGE) {
    target.nodeState = ECMD_TARGET_FIELD_UNUSED;
    target.slotState = ECMD_TARGET_FIELD_UNUSED;
    target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
    target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;
    target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
  }

  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/
  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {

    /* Actually go fetch the data */
    fapi::Target fapiTarget;
    ecmdTargetToFapiTarget(target, fapiTarget);

    if (fapiTarget.getType() == fapi::TARGET_TYPE_SYSTEM) {
      fapi::Target * fapiSystemTargetPtr = NULL;
      fapi::Target & fapiSystemTargetRef = *fapiSystemTargetPtr;
      rc = fapiGetAttribute(fapiSystemTargetRef, attributeId, attributeData);
    } else {
      rc = fapiGetAttribute(fapiTarget, attributeId, attributeData);
    }
    if (rc) {
      printed = "fapigetattr - Error occured performing fapiGetAttribute on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }
    else {
      validPosFound = true;
    }

    printed = ecmdWriteTarget(target) + "\n";

    std::string attributeDataString;
    rc = fapiAttributeDataToString(attributeId, attributeData, attributeDataString, true, format.c_str());
    if (rc) {
      printed = "fapigetattr - Error occured performing fapiAttributeDataToString for ";
      printed += attributeName + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }
    printed += attributeName + " = " + attributeDataString + "\n";
    ecmdOutput(printed.c_str());

  }

  if (attributeType == FAPI_ATTRIBUTE_TYPE_UINT8ARY)
  {
    delete [] attributeData.faUint8ary;
  }
  else if (attributeType == FAPI_ATTRIBUTE_TYPE_UINT32ARY)
  {
    delete [] attributeData.faUint32ary;
  }
  else if (attributeType == FAPI_ATTRIBUTE_TYPE_UINT64ARY)
  {
    delete [] attributeData.faUint64ary;
  }

  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("fapigetattr - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}


// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
