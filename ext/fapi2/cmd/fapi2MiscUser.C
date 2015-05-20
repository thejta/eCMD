/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File fapi2MiscUser.C                                   
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
#include <fapi2ClientCapi.H>
#include <fapi2Interpreter.H>
#include <fapi2SharedUtils.H>
#include <target_types.H>
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

uint32_t fapi2GetAttributeUser(int argc, char * argv[]) {
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
    ecmdOutputError("fapi2getattr - Too few arguments specified; you need at least an attribute.\n");
    ecmdOutputError("fapi2getattr - Type 'fapi2getattr -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }


  //Setup the target that will be used to query the system config
  if (argc > 2) {
    ecmdOutputError("fapi2getattr - Too many arguments specified; you probably added an unsupported option.\n");
    ecmdOutputError("fapi2getattr - Type 'fapi2getattr -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  } else if (argc == 2) {
    std::string chipType, chipUnitType;
    rc = ecmdParseChipField(argv[0], chipType, chipUnitType);
    if (rc) { 
      ecmdOutputError("fapi2getattr - Wildcard character detected however it is not supported by this command.\n");
      return rc;
    }

    /* Error check */
    if (depth) {
      if (chipUnitType == "" && depth < POS) {
        ecmdOutputError("fapi2getattr - Invalid Depth parm specified when a chip was specified.  Try with -dp.\n");
        return ECMD_INVALID_ARGS;
      }

      if (chipUnitType != "" && depth < CHIPUNIT) {
        ecmdOutputError("fapi2getattr - Invalid Depth parm specified when a chipUnit was specified.  Try with -dc.\n");
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
  AttributeId attributeId;
  rc = fapi2AttributeStringToId(attributeName, attributeId);
  if (rc) {
    printed = "fapi2getattr - Unknown attribute name (";
    printed += attributeName + ")\n";
    ecmdOutputError( printed.c_str() );
    return ECMD_INVALID_ARGS;
  }

  /* Determine attribute type and size */
  uint32_t attributeType = 0;
  uint32_t l_numOfEntries;
  uint32_t l_numOfBytes;
  bool attributeEnum = false;
  rc = fapi2GetAttrInfo(attributeId, attributeType, l_numOfEntries, l_numOfBytes, attributeEnum);
  if (rc) {
    printed = "fapi2getattr - Unknown attribute type (";
    printed += attributeName + ")\n";
    ecmdOutputError( printed.c_str() );
    return ECMD_INVALID_ARGS;
  }

  fapi2::AttributeData attributeData;
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
  else if (attributeType == FAPI_ATTRIBUTE_TYPE_INT8ARY)
  {
      attributeData.faInt8ary = new int8_t[l_numOfEntries];
  }
  else if (attributeType == FAPI_ATTRIBUTE_TYPE_INT32ARY)
  {
      attributeData.faInt32ary = new int32_t[l_numOfEntries];
  }
  else if (attributeType == FAPI_ATTRIBUTE_TYPE_INT64ARY)
  {
      attributeData.faInt64ary = new int64_t[l_numOfEntries];
  }

  /* Now set our states based on depth */
  target.cageState = target.nodeState = target.slotState = target.posState = target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;
  if (depth == POS) {
    target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;
    target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
  } else if (depth == SLOT) {
    target.posState = ECMD_TARGET_FIELD_UNUSED;
    target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
    target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;
    target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
  } else if (depth == NODE) {
    target.slotState = ECMD_TARGET_FIELD_UNUSED;
    target.posState = ECMD_TARGET_FIELD_UNUSED;
    target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
    target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;
    target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
  } else if (depth == CAGE) {
    target.nodeState = ECMD_TARGET_FIELD_UNUSED;
    target.slotState = ECMD_TARGET_FIELD_UNUSED;
    target.posState = ECMD_TARGET_FIELD_UNUSED;
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
    rc = fapi2GetAttribute(target, attributeId, attributeData);
    if (rc) {
      printed = "fapi2getattr - Error occured performing fapi2GetAttribute on ";
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
    rc = fapi2AttributeDataToString(attributeId, attributeData, attributeDataString, true, format.c_str());
    if (rc) {
      printed = "fapi2getattr - Error occured performing fapi2AttributeDataToString for ";
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
  else if (attributeType == FAPI_ATTRIBUTE_TYPE_INT8ARY)
  {
    delete [] attributeData.faInt8ary;
  }
  else if (attributeType == FAPI_ATTRIBUTE_TYPE_INT32ARY)
  {
    delete [] attributeData.faInt32ary;
  }
  else if (attributeType == FAPI_ATTRIBUTE_TYPE_INT64ARY)
  {
    delete [] attributeData.faInt64ary;
  }

  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  // This is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("fapi2getattr - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t fapi2DumpAttributeUser(int argc, char * argv[])
{
    // fapi2dumpattr [chip[.chipunit]] [all | hwp | plat] [-f filename]
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

    uint32_t attributeSource = fapi2::FAPI_ATTRIBUTE_SOURCE_PLAT | fapi2::FAPI_ATTRIBUTE_SOURCE_HWP;

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
        ecmdOutputError("fapi2dumpattr - Too many arguments specified; you probably added an unsupported option.\n");
        ecmdOutputError("fapi2dumpattr - Type 'fapi2dumpattr -h' for usage.\n");
        return ECMD_INVALID_ARGS;
    }
    for (int argIndex = 0; argIndex < argc; argIndex++)
    {
        // need to parse chip and/or all|plat|hwp
        if (strcmp("all", argv[argIndex]) == 0)
        {
            attributeSource = fapi2::FAPI_ATTRIBUTE_SOURCE_PLAT | fapi2::FAPI_ATTRIBUTE_SOURCE_HWP;
        }
        else if (strcmp("plat", argv[argIndex]) == 0)
        {
            attributeSource = fapi2::FAPI_ATTRIBUTE_SOURCE_PLAT;
        }
        else if(strcmp("hwp", argv[argIndex]) == 0)
        {
            attributeSource = fapi2::FAPI_ATTRIBUTE_SOURCE_HWP;
        }
        else
        {
            std::string chipType, chipUnitType;
            rc = ecmdParseChipField(argv[argIndex], chipType, chipUnitType);
            if (rc)
            { 
                ecmdOutputError("fapi2dumpattr - Wildcard character detected however it is not supported by this command.\n");
                return rc;
            }

            /* Error check */
            if (depth)
            {
                    if (chipUnitType == "" && depth < POS)
                    {
                        ecmdOutputError("fapi2dumpattr - Invalid Depth parm specified when a chip was specified.  Try with -dp.\n");
                        return ECMD_INVALID_ARGS;
                    }

                    if (chipUnitType != "" && depth < CHIPUNIT)
                    {
                        ecmdOutputError("fapi2dumpattr - Invalid Depth parm specified when a chipUnit was specified.  Try with -dc.\n");
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
            printed = "fapi2dumpattr - Unable to open file (";
            printed += dumpfilename;
            printed += ") to write. ";
            printed += strerror(errno);
            printed += ".\n";
            ecmdOutputError(printed.c_str());
            return ECMD_INVALID_ARGS;
        }
    }

    std::map<fapi2::TargetType, std::list<AttributeId> > attributeIds;
    std::list<AttributeId> systemAttributeIds;

    std::map<AttributeId, AttributeInfo> attributeInfoMap;

    fapi2::TargetType targetType = fapi2::TARGET_TYPE_NONE;
    for (std::list<ecmdChipTarget>::iterator targetIter = targetList.begin(); targetIter != targetList.end(); targetIter++)
    {
        rc = fapi2GetTargetType(*targetIter, targetType);
        if (rc)
        {
            // ERROR
            continue;
        }

        if (attributeIds.count(targetType) == 0)
        {
            fapi2GetAttributeIdsByType(targetType, attributeSource, attributeIds[targetType]);
        }

        if ((attributeIds.count(targetType) != 0) && (!attributeIds[targetType].empty()))
        {
            printed = "\ntarget = " + ecmdWriteTarget(*targetIter, ECMD_DISPLAY_TARGET_COMPRESSED) + "\n";
            if (dumpfilename != NULL)
                dumpfile << printed;
            else
                ecmdOutput(printed.c_str());
            for (std::list<AttributeId>::iterator curAttr = attributeIds[targetType].begin(); curAttr != attributeIds[targetType].end(); curAttr++)
            {
                std::map<AttributeId, AttributeInfo>::iterator attributeInfoIter = attributeInfoMap.find(*curAttr);
                if (attributeInfoIter == attributeInfoMap.end())
                {
                    AttributeInfo tempAttributeInfo = {0, 0, 0, false};
                    /* Determine attribute type and size */
                    rc = fapi2GetAttrInfo(*curAttr, tempAttributeInfo.attributeType, tempAttributeInfo.numOfEntries, tempAttributeInfo.numOfBytes, tempAttributeInfo.attributeEnum);
                    if (rc) {
                        printed = "fapi2dumpattr - Unknown attribute type\n";
                        ecmdOutputError( printed.c_str() );
                    }
                    attributeInfoMap[*curAttr] = tempAttributeInfo;
                    attributeInfoIter = attributeInfoMap.find(*curAttr);
                }

                fapi2::AttributeData attributeData;
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
                else if (attributeInfoIter->second.attributeType == FAPI_ATTRIBUTE_TYPE_INT8ARY)
                {
                    attributeData.faInt8ary = new int8_t[attributeInfoIter->second.numOfEntries];
                }
                else if (attributeInfoIter->second.attributeType == FAPI_ATTRIBUTE_TYPE_INT32ARY)
                {
                    attributeData.faInt32ary = new int32_t[attributeInfoIter->second.numOfEntries];
                }
                else if (attributeInfoIter->second.attributeType == FAPI_ATTRIBUTE_TYPE_INT64ARY)
                {
                    attributeData.faInt64ary = new int64_t[attributeInfoIter->second.numOfEntries];
                }

                rc = fapi2GetAttribute(*targetIter, *curAttr, attributeData);
                if (rc) 
                {
                    printed = "fapi2dumpattr - Error occured performing fapi2GetAttribute on ";
                    printed += ecmdWriteTarget(*targetIter) + "\n";
                    ecmdOutputError( printed.c_str() );
                    continue;
                }
                else
                {
                    validPosFound = true;
                }

                std::string attributeDataString;
                rc = fapi2AttributeDataToString(*curAttr, attributeData, attributeDataString, true, "file");
                if (rc)
                {
                    printed = "fapi2dumpattr - Error occured performing fapi2AttributeDataToString for ";
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
                else if (attributeInfoIter->second.attributeType == FAPI_ATTRIBUTE_TYPE_INT8ARY)
                {
                    delete [] attributeData.faInt8ary;
                }
                else if (attributeInfoIter->second.attributeType == FAPI_ATTRIBUTE_TYPE_INT32ARY)
                {
                    delete [] attributeData.faInt32ary;
                }
                else if (attributeInfoIter->second.attributeType == FAPI_ATTRIBUTE_TYPE_INT64ARY)
                {
                    delete [] attributeData.faInt64ary;
                }
            }
        }
    }
    if (dumpfilename != NULL)
        dumpfile.close();

    // This is an error common across all UI functions
    if (!validPosFound) {
        ecmdOutputError("fapi2dumpattr - Unable to find a valid chip to execute command on\n");
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
