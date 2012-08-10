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
#include <errno.h>
#include <string.h>

#include <ecmdClientCapi.H>
#include <fapiClientCapi.H>
#include <fapiInterpreter.H>
#include <fapiSharedUtils.H>
#include <fapiTarget.H>
#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <ecmdSharedUtils.H>
#include <algorithm>
#include <map>
#include <fstream>

//----------------------------------------------------------------------
//  User Types
//----------------------------------------------------------------------
struct AttributeInfo {
    uint32_t attributeType;
    uint32_t numOfEntries;
    uint32_t numOfBytes;
    bool attributeEnum;
};

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

uint32_t fapiDumpAttributeUser(int argc, char * argv[])
{
    // fapidumpattr [chip[.chipunit]] [all | hwp | plat] [-f filename]
    // all is default
    uint32_t rc = ECMD_SUCCESS;

    ecmdChipTarget target;        ///< Current target
    std::string attributeName;    ///< Name of attribute to fetch
    bool validPosFound = false;   ///< Did we find something to actually execute on ?
    ecmdDataBuffer numData;       ///< Initialise data buffer with the numeric value
    std::string printed;          ///< Print Buffer
    ecmdLooperData looperData;    ///< Store internal Looper data
    char * dumpfilename = NULL;

    int CAGE = 1, NODE = 2, SLOT = 3, POS = 4, CHIPUNIT = 5;
    int depth = 0;                 ///< depth found from Command line parms

    uint32_t attributeSource = fapi::FAPI_ATTRIBUTE_SOURCE_PLAT | fapi::FAPI_ATTRIBUTE_SOURCE_HWP;

    if (ecmdParseOption(&argc, &argv, "-dk"))             depth = CAGE;
    else if (ecmdParseOption(&argc, &argv, "-dn"))        depth = NODE;
    else if (ecmdParseOption(&argc, &argv, "-ds"))        depth = SLOT;
    else if (ecmdParseOption(&argc, &argv, "-dp"))        depth = POS;
    else if (ecmdParseOption(&argc, &argv, "-dc"))        depth = CHIPUNIT;

    target.cageState = target.nodeState = target.slotState = target.chipTypeState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
    target.chipUnitTypeState = ECMD_TARGET_FIELD_WILDCARD;
    target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
    target.threadState = ECMD_TARGET_FIELD_UNUSED;

    dumpfilename = ecmdParseOptionWithArgs(&argc, &argv, "-f");

    /************************************************************************/
    /* Parse Common Cmdline Args                                            */
    /************************************************************************/
    rc = ecmdCommandArgs(&argc, &argv);
    if (rc) return rc;

    /************************************************************************/
    /* Parse Local ARGS here!                                               */
    /************************************************************************/
    //Setup the target that will be used to query the system config
    if (argc > 3)
    {
        ecmdOutputError("fapidumpattr - Too many arguments specified; you probably added an unsupported option.\n");
        ecmdOutputError("fapidumpattr - Type 'fapidumpattr -h' for usage.\n");
        return ECMD_INVALID_ARGS;
    }
    for (int argIndex = 0; argIndex < argc; argIndex++)
    {
        // need to parse chip and/or all|plat|hwp
        if (strcmp("all", argv[argIndex]) == 0)
        {
            attributeSource = fapi::FAPI_ATTRIBUTE_SOURCE_PLAT | fapi::FAPI_ATTRIBUTE_SOURCE_HWP;
        }
        else if (strcmp("plat", argv[argIndex]) == 0)
        {
            attributeSource = fapi::FAPI_ATTRIBUTE_SOURCE_PLAT;
        }
        else if(strcmp("hwp", argv[argIndex]) == 0)
        {
            attributeSource = fapi::FAPI_ATTRIBUTE_SOURCE_HWP;
        }
        else
        {
            std::string chipType, chipUnitType;
            rc = ecmdParseChipField(argv[argIndex], chipType, chipUnitType);
            if (rc)
            { 
                ecmdOutputError("fapidumpattr - Wildcard character detected however it is not supported by this command.\n");
                return rc;
            }

            /* Error check */
            if (depth)
            {
                    if (chipUnitType == "" && depth < POS)
                    {
                        ecmdOutputError("fapidumpattr - Invalid Depth parm specified when a chip was specified.  Try with -dp.\n");
                        return ECMD_INVALID_ARGS;
                    }

                    if (chipUnitType != "" && depth < CHIPUNIT)
                    {
                        ecmdOutputError("fapidumpattr - Invalid Depth parm specified when a chipUnit was specified.  Try with -dc.\n");
                        return ECMD_INVALID_ARGS;
                    }
            }
            else
            { /* No depth, set on for the code below */
                if (chipUnitType == "") {
                    depth = POS;
                } else {
                    depth = CHIPUNIT;
                }
            }
            target.chipType = chipType;
            target.chipTypeState = ECMD_TARGET_FIELD_VALID;
            target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
            if (chipUnitType != "")
            {
                target.chipUnitType = chipUnitType;
                target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
            }
        }
    }

    /* Now set our states based on depth */
    if (depth == POS)
    {
        target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;
        target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
    }
    else if (depth == SLOT)
    {
        target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
    }
    else if (depth == NODE)
    {
        target.slotState = ECMD_TARGET_FIELD_UNUSED;
    }
    else if (depth == CAGE)
    {
        target.nodeState = ECMD_TARGET_FIELD_UNUSED;
    }

    ecmdQueryData queryData;
    rc = ecmdQueryConfigSelected(target, queryData, ECMD_SELECTED_TARGETS_LOOP_DEFALL);
    if (rc) return rc;

    std::list<ecmdChipTarget> targetList;
    for (std::list<ecmdCageData>::iterator ecmdCurCage = queryData.cageData.begin(); ecmdCurCage != queryData.cageData.end(); ecmdCurCage++)
    {
        if (depth == 0 || depth == CAGE)
        {
            // print cage
            target.cage = ecmdCurCage->cageId;
            target.cageState = ECMD_TARGET_FIELD_VALID;
            target.nodeState = ECMD_TARGET_FIELD_UNUSED;
            target.slotState = ECMD_TARGET_FIELD_UNUSED;
            target.chipType = "";
            target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
            target.chipUnitType = "";
            target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
            target.threadState = ECMD_TARGET_FIELD_UNUSED;
            targetList.push_back(target);
        }
        for (std::list<ecmdNodeData>::iterator ecmdCurNode = ecmdCurCage->nodeData.begin(); ecmdCurNode != ecmdCurCage->nodeData.end(); ecmdCurNode++)
        { 
            for (std::list<ecmdSlotData>::iterator ecmdCurSlot = ecmdCurNode->slotData.begin(); ecmdCurSlot != ecmdCurNode->slotData.end(); ecmdCurSlot++)
            {
                for (std::list<ecmdChipData>::iterator ecmdCurChip = ecmdCurSlot->chipData.begin(); ecmdCurChip != ecmdCurSlot->chipData.end(); ecmdCurChip++)
                {
                    if (depth == 0 || depth == POS)
                    {
                        //print chip
                        target.cage = ecmdCurCage->cageId;
                        target.cageState = ECMD_TARGET_FIELD_VALID;
                        target.node = ecmdCurNode->nodeId;
                        target.nodeState = ECMD_TARGET_FIELD_VALID;
                        target.slot = ecmdCurSlot->slotId;
                        target.slotState = ECMD_TARGET_FIELD_VALID;
                        target.chipType = ecmdCurChip->chipType;
                        target.chipTypeState = ECMD_TARGET_FIELD_VALID;
                        target.pos = ecmdCurChip->pos;
                        target.posState = ECMD_TARGET_FIELD_VALID;
                        target.chipUnitType = "";
                        target.chipUnitTypeState = target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;
                        target.threadState = ECMD_TARGET_FIELD_UNUSED;
                        targetList.push_back(target);
                    }
                    for (std::list<ecmdChipUnitData>::iterator ecmdCurChipUnit = ecmdCurChip->chipUnitData.begin(); ecmdCurChipUnit != ecmdCurChip->chipUnitData.end(); ecmdCurChipUnit++)
                    {
                        if (depth == 0 || depth == CHIPUNIT)
                        {
                            // print chip unit
                            target.cage = ecmdCurCage->cageId;
                            target.cageState = ECMD_TARGET_FIELD_VALID;
                            target.node = ecmdCurNode->nodeId;
                            target.nodeState = ECMD_TARGET_FIELD_VALID;
                            target.slot = ecmdCurSlot->slotId;
                            target.slotState = ECMD_TARGET_FIELD_VALID;
                            target.chipType = ecmdCurChip->chipType;
                            target.chipTypeState = ECMD_TARGET_FIELD_VALID;
                            target.pos = ecmdCurChip->pos;
                            target.posState = ECMD_TARGET_FIELD_VALID;
                            target.chipUnitType = ecmdCurChipUnit->chipUnitType;
                            target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
                            target.chipUnitNum = ecmdCurChipUnit->chipUnitNum;
                            target.chipUnitNumState = ECMD_TARGET_FIELD_VALID;
                            target.threadState = ECMD_TARGET_FIELD_UNUSED;
                            targetList.push_back(target);
                        }
                    }
                }
            }
        }
    }

    // open file to write if needed
    std::ofstream dumpfile;
    if (dumpfilename != NULL)
    {
        errno = 0;
        dumpfile.open(dumpfilename);
        if (dumpfile.fail())
        {
            printed = "fapidumpattr - Unable to open file (";
            printed += dumpfilename;
            printed += ") to write. ";
            printed += strerror(errno);
            printed += ".\n";
            ecmdOutputError(printed.c_str());
            return ECMD_INVALID_ARGS;
        }
    }

    std::list<fapi::AttributeId> systemAttributeIds;
    std::list<fapi::AttributeId> dimmAttributeIds;
    std::list<fapi::AttributeId> procChipAttributeIds;
    std::list<fapi::AttributeId> membufChipAttributeIds;
    std::list<fapi::AttributeId> exChipletAttributeIds;
    std::list<fapi::AttributeId> mbaChipletAttributeIds;
    std::list<fapi::AttributeId> mcsChipletAttributeIds;
    std::list<fapi::AttributeId> xbusEndpointAttributeIds;
    std::list<fapi::AttributeId> abusEndpointAttributeIds;

    std::map<fapi::AttributeId, AttributeInfo> attributeInfoMap;

    fapi::Target fapiTarget;
    fapi::Target * fapiSystemTargetPtr = NULL;
    fapi::Target & fapiSystemTargetRef = *fapiSystemTargetPtr;
    fapi::TargetTypes_t targetType = fapi::TARGET_TYPE_NONE;
    std::list<fapi::AttributeId> * attributeListPtr = NULL;
    for (std::list<ecmdChipTarget>::iterator targetIter = targetList.begin(); targetIter != targetList.end(); targetIter++)
    {
        ecmdTargetToFapiTarget(*targetIter, fapiTarget);
        targetType = fapiTarget.getType();
        switch (targetType)
        {
            case fapi::TARGET_TYPE_SYSTEM:
                if (systemAttributeIds.empty())
                {
                    fapiGetAttributeIdsByType(targetType, attributeSource, systemAttributeIds);
                }
                attributeListPtr = &systemAttributeIds;
                break;
            case fapi::TARGET_TYPE_DIMM:
                if (dimmAttributeIds.empty())
                {
                    fapiGetAttributeIdsByType(targetType, attributeSource, dimmAttributeIds);
                }
                attributeListPtr = &dimmAttributeIds;
                break;
            case fapi::TARGET_TYPE_PROC_CHIP:
                if (procChipAttributeIds.empty())
                {
                    fapiGetAttributeIdsByType(targetType, attributeSource, procChipAttributeIds);
                }
                attributeListPtr = &procChipAttributeIds;
                break;
            case fapi::TARGET_TYPE_MEMBUF_CHIP:
                if (membufChipAttributeIds.empty())
                {
                    fapiGetAttributeIdsByType(targetType, attributeSource, membufChipAttributeIds);
                }
                attributeListPtr = &membufChipAttributeIds;
                break;
            case fapi::TARGET_TYPE_EX_CHIPLET:
                if (exChipletAttributeIds.empty())
                {
                    fapiGetAttributeIdsByType(targetType, attributeSource, exChipletAttributeIds);
                }
                attributeListPtr = &exChipletAttributeIds;
                break;
            case fapi::TARGET_TYPE_MBA_CHIPLET:
                if (mbaChipletAttributeIds.empty())
                {
                    fapiGetAttributeIdsByType(targetType, attributeSource, mbaChipletAttributeIds);
                }
                attributeListPtr = &mbaChipletAttributeIds;
                break;
            case fapi::TARGET_TYPE_MCS_CHIPLET:
                if (mbaChipletAttributeIds.empty())
                {
                    fapiGetAttributeIdsByType(targetType, attributeSource, mcsChipletAttributeIds);
                }
                attributeListPtr = &mcsChipletAttributeIds;
                break;
            case fapi::TARGET_TYPE_XBUS_ENDPOINT:
                if (xbusEndpointAttributeIds.empty())
                {
                    fapiGetAttributeIdsByType(targetType, attributeSource, xbusEndpointAttributeIds);
                }
                attributeListPtr = &xbusEndpointAttributeIds;
                break;
            case fapi::TARGET_TYPE_ABUS_ENDPOINT:
                if (abusEndpointAttributeIds.empty())
                {
                    fapiGetAttributeIdsByType(targetType, attributeSource, abusEndpointAttributeIds);
                }
                attributeListPtr = &abusEndpointAttributeIds;
                break;
            default:
                // ERROR
                //ecmdOutputError("fapidumpattr - Unknown target type\n");
                attributeListPtr = NULL;
        }

        if ((attributeListPtr != NULL) && (!attributeListPtr->empty()))
        {
            printed = "\ntarget = " + ecmdWriteTarget(*targetIter, ECMD_DISPLAY_TARGET_COMPRESSED) + "\n";
            if (dumpfilename != NULL)
                dumpfile << printed;
            else
                ecmdOutput(printed.c_str());
            for (std::list<fapi::AttributeId>::iterator curAttr = attributeListPtr->begin(); curAttr != attributeListPtr->end(); curAttr++)
            {
                std::map<fapi::AttributeId, AttributeInfo>::iterator attributeInfoIter = attributeInfoMap.find(*curAttr);
                if (attributeInfoIter == attributeInfoMap.end())
                {
                    AttributeInfo tempAttributeInfo = {0, 0, 0, false};
                    /* Determine attribute type and size */
                    rc = fapiGetAttrInfo(*curAttr, tempAttributeInfo.attributeType, tempAttributeInfo.numOfEntries, tempAttributeInfo.numOfBytes, tempAttributeInfo.attributeEnum);
                    if (rc) {
                        printed = "fapidumpattr - Unknown attribute type\n";
                        ecmdOutputError( printed.c_str() );
                    }
                    attributeInfoMap[*curAttr] = tempAttributeInfo;
                    attributeInfoIter = attributeInfoMap.find(*curAttr);
                }

                fapi::AttributeData attributeData;
                attributeData.faValidMask = attributeInfoIter->second.attributeType;
                attributeData.faEnumUsed = attributeInfoIter->second.attributeEnum;

                if (attributeInfoIter->second.attributeType == FAPI_ATTRIBUTE_TYPE_UINT8ARY)
                {
                    attributeData.faUint8ary = new uint8_t[attributeInfoIter->second.numOfEntries];
                }
                else if (attributeInfoIter->second.attributeType == FAPI_ATTRIBUTE_TYPE_UINT32ARY)
                {
                    attributeData.faUint32ary = new uint32_t[attributeInfoIter->second.numOfEntries];
                }
                else if (attributeInfoIter->second.attributeType == FAPI_ATTRIBUTE_TYPE_UINT64ARY)
                {
                    attributeData.faUint64ary = new uint64_t[attributeInfoIter->second.numOfEntries];
                }

                if (targetType == fapi::TARGET_TYPE_SYSTEM)
                    rc = fapiGetAttribute(fapiSystemTargetRef, *curAttr, attributeData);
                else
                    rc = fapiGetAttribute(fapiTarget, *curAttr, attributeData);
                if (rc) 
                {
                    printed = "fapidumpattr - Error occured performing fapiGetAttribute on ";
                    printed += ecmdWriteTarget(*targetIter) + "\n";
                    ecmdOutputError( printed.c_str() );
                    continue;
                }
                else
                {
                    validPosFound = true;
                }

                std::string attributeDataString;
                rc = fapiAttributeDataToString(*curAttr, attributeData, attributeDataString, true, "file");
                if (rc)
                {
                    printed = "fapidumpattr - Error occured performing fapiAttributeDataToString for ";
                    printed += attributeName + "\n";
                    ecmdOutputError( printed.c_str() );
                    continue;
                }
                printed = attributeDataString;
                if (dumpfilename != NULL)
                    dumpfile << printed;
                else
                    ecmdOutput(printed.c_str());

                if (attributeInfoIter->second.attributeType == FAPI_ATTRIBUTE_TYPE_UINT8ARY)
                {
                    delete [] attributeData.faUint8ary;
                }
                else if (attributeInfoIter->second.attributeType == FAPI_ATTRIBUTE_TYPE_UINT32ARY)
                {
                    delete [] attributeData.faUint32ary;
                }
                else if (attributeInfoIter->second.attributeType == FAPI_ATTRIBUTE_TYPE_UINT64ARY)
                {
                    delete [] attributeData.faUint64ary;
                }
            }
        }
    }
    if (dumpfilename != NULL)
        dumpfile.close();

    // This is an error common across all UI functions
    if (!validPosFound) {
        ecmdOutputError("fapidumpattr - Unable to find a valid chip to execute command on\n");
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
