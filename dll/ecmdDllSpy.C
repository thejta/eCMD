// Copyright ***********************************************************
//                                                                      
// File ecmdClientSpy.C                                   
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
/* $Header$ */
// Module Description **************************************************
//
// Description: Functions to handle spies on top of eCMD
//
// End Module Description **********************************************

/* This source only gets included for ecmdDll's that don't want to implement spy handling of their own */
#ifdef USE_ECMD_COMMON_SPY

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define ecmdClientSpy_C
#include <list>
#include <algorithm>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <ecmdSharedUtils.H>

#include <ecmdDllCapi.H>

/* Grab the includes for the engineering data compiler */
#include <sedcSpyContainer.H>
#include <sedcDefines.H>
#include <sedcSpyParser.H>
#include <sedcCommonParser.H>

#undef ecmdClientSpy_C
//----------------------------------------------------------------------
//  User Types
//----------------------------------------------------------------------
typedef enum {
  SPYDATA_ENUM,
  SPYDATA_DATA,
  SPYDATA_GROUPS
} dllSpyDataType_t;

/* @brief Used by the getspy/putspy to hold different data types */
struct dllSpyData {
  dllSpyDataType_t              dataType;       ///< Field below to use
  std::string                   enum_data;      ///< Enum data
  ecmdDataBuffer*               int_data;       ///< Integer data
  std::list< ecmdSpyGroupData > *  group_data;     ///< Group Data
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
/* Lookup Spy info from a spydef file */
uint32_t dllGetSpyInfo(ecmdChipTarget & i_target, const char* name, sedcSpyContainer& returnSpy);
uint32_t dllGetSpyClockDomain(ecmdChipTarget & i_target, sedcAEIEntry* spy_data, std::string & o_domain);
/* Search the spy file for our spy */
uint32_t dllLocateSpy(std::ifstream &spyFile, std::string spy_name);
int dllLocateSpyHash(std::ifstream &spyFile, std::ifstream &hashFile, uint32_t key, std::string spy_name);
uint32_t dllGetSpiesInfo(ecmdChipTarget & i_target, std::list<sedcSpyContainer>& returnSpyList);
int dllGetSpyListHash(std::ifstream &hashFile, std::list<sedcHashEntry> &spyKeysList);
uint32_t dllGetSpy (ecmdChipTarget & i_target, const char * i_spyName, dllSpyData & data);
uint32_t dllGetSpy(ecmdChipTarget & i_target, dllSpyData &data, sedcSpyContainer &spy);
uint32_t dllGetSpyEcc(ecmdChipTarget & i_target, std::string epcheckerName, ecmdDataBuffer& inLatches, ecmdDataBuffer& outLatches, ecmdDataBuffer& errorMask);
uint32_t dllGenSpyEcc(ecmdChipTarget & i_target, std::string eccfuncName, ecmdDataBuffer& inLatches, ecmdDataBuffer& goodECC);
uint32_t dllPutSpy (ecmdChipTarget & i_target, const char * i_spyName, dllSpyData & i_data);
uint32_t dllPutSpy(ecmdChipTarget & i_target, dllSpyData &data, sedcSpyContainer &spy);
uint32_t dllPutSpyEcc(ecmdChipTarget & i_target, std::string epcheckerName);

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------
/* Defined in ecmdDllCapi.C */
uint32_t dllGetChipData (ecmdChipTarget & i_target, ecmdChipData & o_data);

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------

/**
  This function specification is the same as defined in ecmdClientCapi.H as ecmdQuerySpy
*/
uint32_t dllQuerySpy(ecmdChipTarget & i_target, std::list<ecmdSpyData> & o_queryDataList, const char * i_spyName, ecmdQueryDetail_t i_detail) {
  uint32_t rc = ECMD_SUCCESS;
  sedcSpyContainer mySpy;
  std::list<sedcSpyContainer> mySpyList;
  std::list<sedcSpyContainer>::iterator spyIt;
  std::list<sedcAEIEnum>::iterator enumit;
  ecmdSpyData queryData;
  char outstr[200];
  
  if (i_spyName == NULL) {
    rc = dllGetSpiesInfo(i_target, mySpyList);
    if (rc) {
      printf(outstr,"dllQuerySpy - Problems reading spies from spydef/hash file!\n");
      dllOutputError(outstr);
      return ECMD_INVALID_SPY;
    }
  }
  else {
    /* Retrieve my spy either from the DB or the spydef file */
    rc = dllGetSpyInfo(i_target, i_spyName, mySpy);
    if (rc) {
      sprintf(outstr,"dllQuerySpy - Problems reading spy '%s' from file!\n", i_spyName);
      dllOutputError(outstr);
      dllOutputError(outstr);
      return ECMD_INVALID_SPY;
    } else if (!mySpy.valid) {
      sprintf(outstr,"dllQuerySpy - Read of spy '%s' from file failed!\n", i_spyName);
      dllOutputError(outstr);
      return ECMD_INVALID_SPY;
    }
    mySpyList.push_back(mySpy);
  }
  
  for (spyIt = mySpyList.begin(); spyIt != mySpyList.end(); spyIt++) {
    if (!spyIt->valid) {
       if (i_spyName != NULL) {
         sprintf(outstr,"dllQuerySpy - Read of spy '%s' from file failed!\n", spyIt->name.c_str());
         dllOutputError(outstr);
         return ECMD_INVALID_SPY;
       }
       else {
         continue;
       }
    }

    /* Special case thing here.. */
    if (spyIt->type == SC_SYNONYM) {
      /* We have a synonym we need to look up the spy behind it */
      sedcSynonymEntry syn = spyIt->getSynonymEntry();

      rc = dllGetSpyInfo(i_target, syn.realName.c_str(), mySpy);
      if (rc) {
    	sprintf(outstr,"dllQuerySpy - Problems reading spy '%s' from file!\n", spyIt->name.c_str());
    	dllOutputError(outstr);
    	return ECMD_INVALID_SPY;
      } else if (!spyIt->valid) {
    	sprintf(outstr,"dllQuerySpy - Read of spy '%s' from file failed!\n", spyIt->name.c_str());
    	dllOutputError(outstr);
    	return ECMD_INVALID_SPY;
      }

    }

    queryData.spyName = spyIt->name;
    queryData.bitLength = 0;
    queryData.isEccChecked = false;
    queryData.isEnumerated = false;
    if (spyIt->type == SC_AEI) {
      sedcAEIEntry spyent = spyIt->getAEIEntry();
      queryData.bitLength = spyent.length;
      if (spyent.states & SPY_ALIAS)
    	queryData.spyType = ECMD_SPYTYPE_ALIAS;
      else if (spyent.states & SPY_IDIAL)
    	queryData.spyType = ECMD_SPYTYPE_IDIAL;
      else if (spyent.states & SPY_EDIAL)
    	queryData.spyType = ECMD_SPYTYPE_EDIAL;
      else {
        if (i_spyName != NULL) {
          dllOutputError("dllQuerySpy - Unknown spy type returned\n");
	  return ECMD_INVALID_SPY;
	}
    	continue;
      }

      /* Does it have ECC ? */
      if (spyent.states & SPY_EPCHECKERS) {
    	queryData.isEccChecked = true;
      }

      /* Let's walk through the enums */
      if (spyent.states & SPY_ENUM)
    	queryData.isEnumerated = true;

      /* Let's walk through the enums */
      queryData.enums.clear();
      for (enumit = spyent.aeiEnums.begin(); enumit != spyent.aeiEnums.end(); enumit ++) {
    	queryData.enums.push_back(enumit->enumName);
      }

      /* The eccGroups */
      queryData.epCheckers = spyent.aeiEpcheckers;

    } else {
      if (i_spyName != NULL) {
        dllOutputError("dllQuerySpy - Unknown spy type returned\n");
        return ECMD_INVALID_SPY;
      }
      continue;
    }
    queryData.clockState = ECMD_CLOCKSTATE_UNKNOWN;
    o_queryDataList.push_back(queryData);
  }
  return rc;
}
/**
  This function specification is the same as defined in ecmdClientCapi.H as getSpy
*/
uint32_t dllGetSpy (ecmdChipTarget & i_target, const char * i_spyName, ecmdDataBuffer & o_data){
  uint32_t rc = ECMD_SUCCESS;

  dllSpyData fdata;
  fdata.dataType = SPYDATA_DATA;
  fdata.int_data = &o_data;

  rc = dllGetSpy(i_target,i_spyName,fdata);


  return rc;
}

/**
  This function specification is the same as defined in ecmdClientCapi.H as GetSpyEnum
*/
uint32_t dllGetSpyEnum (ecmdChipTarget & i_target, const char * i_spyName, std::string & o_enumValue){
  uint32_t rc = ECMD_SUCCESS;
  dllSpyData fdata;
  fdata.dataType = SPYDATA_ENUM;

  rc = dllGetSpy(i_target,i_spyName,fdata);
  if (!rc)
  o_enumValue = fdata.enum_data;
  
  return rc;
}

uint32_t dllGetSpyGroups(ecmdChipTarget & i_target, const char * i_spyName, std::list < ecmdSpyGroupData > & o_groups) {
  uint32_t rc = ECMD_SUCCESS;
  dllSpyData fdata;
  fdata.dataType = SPYDATA_GROUPS;
  fdata.group_data = &o_groups;

  rc = dllGetSpy(i_target,i_spyName,fdata);
  return rc;

}

uint32_t dllGetSpy (ecmdChipTarget & i_target, const char * i_spyName, dllSpyData & data){
  uint32_t rc = ECMD_SUCCESS;
  bool enabledCache = false;                    ///< This is turned on if we enabled the cache, so we can disable on exit
  sedcSpyContainer mySpy;
  char outstr[200];


  if (!dllIsRingCacheEnabled()) {
    enabledCache = true;
    dllEnableRingCache();
  }


  /* Retrieve my spy either from the DB or the spydef file */
  rc = dllGetSpyInfo(i_target, i_spyName, mySpy);
  if (rc) {
    sprintf(outstr,"dllGetSpy - Problems reading spy '%s' from file!\n", i_spyName);
    dllOutputError(outstr);
    return rc;
  } else if (!mySpy.valid) {
    sprintf(outstr,"dllGetSpy - Read of spy '%s' from file failed!\n", i_spyName);
    dllOutputError(outstr);
    return ECMD_UNKNOWN_FILE;
  } else if (mySpy.type != SC_AEI) {
    dllOutputError("dllGetSpy - A non alias, idial or edial was passed in!\n");
    return ECMD_INVALID_SPY;
  }


  rc = dllGetSpy(i_target, data, mySpy);


  /* Handle ECC here */
  if (!rc) {
    sedcAEIEntry myAIE = mySpy.getAEIEntry();
    if (!myAIE.aeiEpcheckers.empty()) {
      std::list<std::string>::iterator eccIter;
      ecmdDataBuffer inData, outData, errorMask; // Not used on the cronus interface, just place holders
      eccIter = myAIE.aeiEpcheckers.begin();
      uint32_t totalrc = 0;
      while (eccIter != myAIE.aeiEpcheckers.end()) {
        rc = dllGetSpyEcc(i_target, *eccIter, inData, outData, errorMask);
        if (rc) {
          totalrc = rc;
          sprintf(outstr,"dllGetSpy - ECC match fail for eplatches \"%s\"!\n",eccIter->c_str());
          dllOutputError(outstr);
        }
        eccIter++;
      }
      // If we had any errors, total rc will be set and we can return
      if (totalrc)
        return ECMD_SPY_FAILED_ECC_CHECK;

    }
  }
  else { return rc; }
  
  if (enabledCache) {
    rc = dllDisableRingCache();
  }

  return rc;
}

uint32_t dllGetSpy(ecmdChipTarget & i_target, dllSpyData &data, sedcSpyContainer &spy) {
  uint32_t rc = ECMD_SUCCESS;

  std::list<sedcLatchLine>::iterator lineit;
  std::list<sedcAEIEnum>::iterator enumit;
  ecmdDataBuffer scan;
  ecmdDataBuffer enumextract;
  uint32_t addr;
  ecmdDataBuffer* extractbuffer;
  ecmdDataBuffer tmpbuffer;
  ecmdDataBuffer deadbitsMask;
  char outstr[200];
  ecmdChipData chipData;                ///< Chip data to find out bus info
  uint32_t bustype;                             ///< Type of bus we are attached to JTAG vs FSI
  std::string spyDomain;                        ///< What domain does this chip belong to
  ecmdClockState_t curClockState;               ///< Current state of spy clock domain

  ecmdSpyGroupData groupTemp;
  std::list<ecmdSpyGroupData> groups;             ///< List to store all groups so that they can be compared in the end
  std::list<ecmdSpyGroupData>::iterator groupit;   ///< Iterator for group list

  int num;
  int curaliasbit = 0;

  uint32_t curstate = SPY_CLOCK_IND;       ///< Current state of what we will accept, we will start by allowing clock independent (or spy's without clock dependence) to run


  sedcSpyContainer mySpy = spy;

  /* We got here, we had better be an alias/idial or edial */
  if (mySpy.type != SC_AEI) {
    sprintf(outstr,"dllGetSpy - Spy '%s' is not of type alias, idial or edial\n", spy.name.c_str());
    dllOutputError(outstr);
    return ECMD_INVALID_SPY;
  }

  /* Let's find out if we are JTAG of FSI here */
  rc = dllGetChipData(i_target, chipData);
  if (rc) {
    sprintf(outstr,"Problems retrieving chip information on target\n");
    dllRegisterErrorMsg(rc, "dllGetSpy", outstr );
    return rc;
  }
  /* Now make sure the plugin gave us some bus info */
  if (!(chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK)) {
    dllRegisterErrorMsg(ECMD_DLL_INVALID, "dllGetSpy", "eCMD plugin did not implement ecmdChipData.chipFlags unable to determine if FSI or JTAG attached chip\n");
    return ECMD_DLL_INVALID;
  } else if (((chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK) != ECMD_CHIPFLAG_JTAG) &&
             ((chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK) != ECMD_CHIPFLAG_FSI) ) {
    dllRegisterErrorMsg(ECMD_DLL_INVALID, "dllGetSpy", "eCMD plugin returned an invalid bustype in ecmdChipData.chipFlags\n");
    return ECMD_DLL_INVALID;
  }
  /* Store our type */
  bustype = chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK;


  /* Now let's go grab our data */
  sedcAEIEntry spyent = mySpy.getAEIEntry();


  /* See if we need the clock domain */
  if (spyent.states & SPY_CLOCK_ANY)
    rc = dllGetSpyClockDomain(i_target, &spyent, spyDomain);
  if (rc) return rc;

  /* Do some error checking with what we have */
  if (data.dataType == SPYDATA_DATA) {
    /* We need to size our buffer here */
    data.int_data->setBitLength(spyent.length);
    extractbuffer = data.int_data;
  } else if (data.dataType == SPYDATA_ENUM) {

    if (!spyent.states & SPY_ENUM) {
      sprintf(outstr,"dllGetSpy - Tried to fetch an enum on a non-enumerated alias or idial for : %s\n",spy.name.c_str());
      dllOutputError(outstr);
      return ECMD_SPY_NOT_ENUMERATED;
    }
    /* This is an enum dial or enumerated alias, we are going to extract to a side buffer */
    enumextract.setBitLength(spyent.length);
    extractbuffer = &enumextract;
  } else if (data.dataType == SPYDATA_GROUPS) {
    /* We are just grabbing groups from here, but we need to create a buffer for it to work */
    enumextract.setBitLength(spyent.length);
    extractbuffer = &enumextract;
  }

  deadbitsMask.setBitLength(spyent.length);



  /* Ok, here goes the meat */
  for (lineit = spyent.aeiLines.begin(); lineit != spyent.aeiLines.end(); lineit ++) {

    /*---------------------*/
    /* MAIN SPY SECTION    */
    /*---------------------*/

    if ((lineit->state  & ~(SPY_SECTION_START | SPY_MAJOR_TYPES)) == 0) {
      /* this is the start of the spy , woohoo */
      /* Oh and we have nothing to do for it */

    } else if ((lineit->state  & ~(SPY_SECTION_END | SPY_MAJOR_TYPES)) == 0) {
      /* Do Nothing */



    /*---------------------*/
    /* RING     SECTION    */
    /*---------------------*/
    } else if (lineit->state == (SPY_SECTION_START | SPY_RING)) {
      /* This is a new ring */
      /* Check to see if we are in the right state */
      if (curstate & SPY_CLOCK_ANY) {
        rc = dllGetRing( i_target, lineit->latchName.c_str(), scan);
        if (rc) return rc;
      }

    } else if (lineit->state == (SPY_SECTION_END | SPY_RING)) {
      /* Do Nothing */



    /*---------------------*/
    /* SCOM     SECTION    */
    /*---------------------*/
    } else if (lineit->state == (SPY_SECTION_START | SPY_SCOM)) {
      /* This is a new scom */
      /* Check to see if we are in the right state */
      if (curstate & SPY_CLOCK_ANY) {
        num = sscanf(lineit->latchName.c_str(), "%x",&addr);
        if (num != 1) {
          sprintf(outstr, "dllGetSpy - Unable to determine scom address (%s) from spy definition (%s)\n", lineit->latchName.c_str(), spy.name.c_str());
          dllOutputError(outstr);
          return ECMD_INVALID_SPY;
        }
        rc = dllGetScom(i_target, addr, scan);
        if (rc) return rc;
      }

    } else if (lineit->state == (SPY_SECTION_END | SPY_SCOM)) {
      /* Do nothing */


    /*---------------------*/
    /* GROUP    SECTION    */
    /*---------------------*/
    } else if (lineit->state == (SPY_SECTION_START | SPY_GROUP_BITS)) {
      /* This is a new group */
      /* Reset out bit pointer */
      curaliasbit = 0;
      deadbitsMask.flushTo0();


    } else if (lineit->state == (SPY_SECTION_END | SPY_GROUP_BITS)) {
      /* We want to push the current buffer */
      groupTemp.extractBuffer = *extractbuffer;
      groupTemp.deadbitsMask = deadbitsMask;
      groups.push_back(groupTemp);


    /*---------------------*/
    /* CLOCK ON SECTION    */
    /*---------------------*/
    } else if (lineit->state == (SPY_SECTION_START | SPY_CLOCK_ON)) {
      /* We hit a clock on section - we need to figure out if the chip clocks are on */
      rc = dllQueryClockState (i_target, spyDomain.c_str(), curClockState);
      if (rc) return rc;
      if ((curClockState == ECMD_CLOCKSTATE_ON) || (curClockState == ECMD_CLOCKSTATE_NA)) {
        curstate &= ~(SPY_CLOCK_ANY);
        curstate |= SPY_CLOCK_ON;
      } else 
        curstate &= ~(SPY_CLOCK_ANY);
    } else if (lineit->state == (SPY_SECTION_END | SPY_CLOCK_ON)) {
      /* We are done with the clock dep section, lets set our state back to independent */
      curstate &= ~(SPY_CLOCK_ANY);
      curstate |= SPY_CLOCK_IND;


    /*---------------------*/
    /* CLOCK OFF SECTION   */
    /*---------------------*/
    } else if (lineit->state == (SPY_SECTION_START | SPY_CLOCK_OFF)) {
      /* We hit a clock off section - we need to figure out if the chip clocks are off */
      rc = dllQueryClockState (i_target, spyDomain.c_str(), curClockState);
      if (rc) return rc;
      if ((curClockState == ECMD_CLOCKSTATE_OFF) || (curClockState == ECMD_CLOCKSTATE_NA)  || (curClockState == ECMD_CLOCKSTATE_UNKNOWN)  ) {
        curstate &= ~(SPY_CLOCK_ANY);
        curstate |= SPY_CLOCK_ON;
      } else 
        curstate &= ~(SPY_CLOCK_ANY);

    } else if (lineit->state == (SPY_SECTION_END | SPY_CLOCK_OFF)) {
      /* We are done with the clock dep section, lets set our state back to independent */
      curstate &= ~(SPY_CLOCK_ANY);
      curstate |= SPY_CLOCK_IND;


    /*---------------------*/
    /* ENUM     SECTION    */
    /*---------------------*/
    } else if (lineit->state & SPY_ENUM) {
      /* We don't care about the enum lines */

    /*---------------------*/
    /* ECC     SECTION     */
    /*---------------------*/
    } else if (lineit->state & SPY_EPCHECKERS) {
      /* We don't care about the ecc lines */

    /*---------------------*/
    /* DATA     SECTION    */
    /*---------------------*/
      
    } else if (lineit->length == -1) {
      /* We shouldn't get here, the only thing left should be actual latchs */
      dllOutputError("dllGetSpy - Problems reading spy info found invalid data\n");
      return ECMD_FAILURE;

      /* If cur_state says we are in a section then we want to go do something */
    } else if (curstate & SPY_CLOCK_ANY) {
      /* This had better be valid latches so let's go extract */

      /* Handle deadbits by just clearing them */
      if (lineit->state & SPY_DEADBITS) {
        extractbuffer->clearBit(curaliasbit, lineit->length);
        if (lineit->state & SPY_GROUP_BITS)  // Needed for data compares later
          deadbitsMask.setBit(curaliasbit, lineit->length);
        curaliasbit += lineit->length;

      /* Inverted bits */
      } else if (lineit->state & SPY_INVERT) {

        if (lineit->length == 1) {
          if (bustype == ECMD_CHIPFLAG_FSI) {
            if (scan.isBitSet(lineit->offsetFSI))
              rc = extractbuffer->clearBit(curaliasbit++);
            else
              rc = extractbuffer->setBit(curaliasbit++);
          } else { //JTAG
            if (scan.isBitSet(lineit->offsetJTAG))
              rc = extractbuffer->clearBit(curaliasbit++);
            else
              rc = extractbuffer->setBit(curaliasbit++);
          }
          if (rc) return rc;
        } else {
          /* We need to grab more data */
          /* scom's are not reversed so we grab like FSI mode */
          if ((bustype == ECMD_CHIPFLAG_FSI) || (lineit->state & SPY_SCOM)) 
            rc = scan.extract(tmpbuffer, lineit->offsetFSI, lineit->length);
          else { // JTAG
            rc = scan.extract(tmpbuffer, (lineit->offsetJTAG - lineit->length + 1), lineit->length);
            tmpbuffer.reverse();
          }
          if (rc) return rc;
          tmpbuffer.invert();   /* Invert bits */
          rc = extractbuffer->insert(tmpbuffer, curaliasbit, lineit->length);
          if (rc) return rc;
          curaliasbit += lineit->length;
        }

        /* Standard latch bits */
      } else {
        if (lineit->length == 1) {
          if (bustype == ECMD_CHIPFLAG_FSI) {
            if (scan.isBitSet(lineit->offsetFSI))
              rc = extractbuffer->setBit(curaliasbit++);
            else
              rc = extractbuffer->clearBit(curaliasbit++);
          } else { //JTAG
            if (scan.isBitSet(lineit->offsetJTAG))
              rc = extractbuffer->setBit(curaliasbit++);
            else
              rc = extractbuffer->clearBit(curaliasbit++);
          }
          if (rc) return rc;
        } else {
          /* We need to grab more data */
          /* scom's are not reversed so we grab like FSI mode */
          if ((bustype == ECMD_CHIPFLAG_FSI) || (lineit->state & SPY_SCOM)) 
            rc = scan.extract(tmpbuffer, lineit->offsetFSI, lineit->length);
          else { // JTAG
            rc = scan.extract(tmpbuffer, (lineit->offsetJTAG - lineit->length + 1), lineit->length);
            tmpbuffer.reverse();
          }
          if (rc) return rc;
          rc = extractbuffer->insert(tmpbuffer, curaliasbit, lineit->length);
          if (rc) return rc;
          curaliasbit += lineit->length;
        }
      }

    }
  }     /* End of spy data loop */


  int foundit = 0;
  int wordlen = extractbuffer->getWordLength();
  uint32_t mask;

  /*--------------------------------*/
  /* GROUP VERIFICATION  SECTION    */
  /*--------------------------------*/
  /* If the user want's the groups we won't do any verification */
  if (data.dataType != SPYDATA_GROUPS) {
    /* Now that we are done we should have a list of groups we need to walk through them all and make sure they all match */
    ecmdDataBuffer cumlExtract;    ///< Cumulative copy of the extractbuffer
    ecmdDataBuffer cumlDead;       ///< Cumulative copy of the deadbits
    ecmdDataBuffer tempDead;      ///< Stores a temporary build of the deadbits mask
    int groupCount = 0;           ///< Keep track of which group entry this is
    for (groupit = groups.begin(); groupit != groups.end(); groupit ++) {
      groupCount++;
      if (groupit == groups.begin()) {
        cumlExtract = groupit->extractBuffer;
        cumlDead = groupit->deadbitsMask;
        tempDead.setBitLength(groupit->deadbitsMask.getBitLength());
      } else {

        /* Merge the newest deadbitsmask with the cumulative one, then invert */
        tempDead = cumlDead;
        tempDead.merge(groupit->deadbitsMask);
        tempDead.invert();

        /* Now compare the two extracts.  If they match, woohoo! */
        if ((groupit->extractBuffer & tempDead) == (cumlExtract & tempDead)) {
          cumlExtract = cumlExtract | groupit->extractBuffer;
          cumlDead = cumlDead & groupit->deadbitsMask;
        } else {  /* We found a mismatch in group bits */
          ecmdDataBuffer failData;
          /* Get some data to print to the user */
          failData = (groupit->extractBuffer & tempDead);
          tempDead = (cumlExtract & tempDead);
          failData.setXor(tempDead, 0, tempDead.getBitLength());
          sprintf(outstr,"dllGetSpy - Group mismatch occurred on entry %d when reading spy '%s'\n", groupCount, spy.name.c_str());
          dllOutputError(outstr);
          /* Lets log some extended data that the user will get in certain types of fails */
          sprintf(outstr,"\tCuml. Data: 0x%s\n\tFail Group: 0x%s\n\tFail Mask:  0x%s\n",cumlExtract.genHexLeftStr().c_str(),groupit->extractBuffer.genHexLeftStr().c_str(),tempDead.genHexLeftStr().c_str());
          dllRegisterErrorMsg(ECMD_SPY_GROUP_MISMATCH, "dllGetSpy", outstr);
          rc = ECMD_SPY_GROUP_MISMATCH;
          return rc;
        }
      }
    }
    /* If we the groups aren't empty, and we made it this far, then assign the cumlExtract to the extractbuffer */
    if (!groups.empty()) {
      *extractbuffer = cumlExtract;
    }
  }

  /* We had an enumerated spy, we need to lookup the enum to return, not the data */
  if (data.dataType == SPYDATA_ENUM) {
    foundit = 0;
    wordlen = extractbuffer->getWordLength();
    /* We are doing an enumerated fetch here, lets see what we can find */
    for (enumit = spyent.aeiEnums.begin(); enumit != spyent.aeiEnums.end(); enumit ++) {
      mask = 0xFFFFFFFF;
      for (int idx = 0; idx < wordlen; idx ++) {

        if (idx == wordlen - 1) {
          /* This is the last word we need to shift appropriately to compare only good data */
          mask <<= 32 - (extractbuffer->getBitLength() % 32);
          if ((enumit->enumValue[idx] & mask) == (extractbuffer->getWord(idx) & mask)) {
            /* This is it, woohooo */
            foundit = 1;
            data.enum_data = enumit->enumName;
            break;
          } else {
            /* Not it */
            break;
          }
        } else {
          if (enumit->enumValue[idx] != extractbuffer->getWord(idx))
            /* This isn't it */
            break;
        }
      }
      if (foundit) break;

    }
    if (!foundit) {
      /* We didn't find an enum that matched , this is bad */
      sprintf(outstr,"dllGetSpy - Didn't find a matching enum on read of : %s\n",spy.name.c_str());
      dllOutputError(outstr);
      return ECMD_INVALID_SPY_ENUM;
    }
    

  } else if (data.dataType == SPYDATA_GROUPS) {
    *(data.group_data) = groups;
  }

  return rc;
}

uint32_t dllGetSpyEcc(ecmdChipTarget & i_target, std::string epcheckerName, ecmdDataBuffer& inLatches, ecmdDataBuffer& outLatches, ecmdDataBuffer& errorMask) {
  uint32_t rc = ECMD_SUCCESS;

  sedcSpyContainer myDC;
  dllSpyData data;
  char outstr[200];

  /* Retrieve my spy either from the DB or the spydef file */
  rc = dllGetSpyInfo(i_target, epcheckerName.c_str(), myDC);
  if (rc) {
    sprintf(outstr,"dllGetSpyEcc - Problems reading spy '%s' from file!\n", epcheckerName.c_str());
    dllOutputError(outstr);
    return rc;
  } else if (!myDC.valid) {
    sprintf(outstr,"dllGetSpyEcc - Read of spy '%s' from file failed!\n", epcheckerName.c_str());
    dllOutputError(outstr);
    return ECMD_UNKNOWN_FILE;
  } else if (myDC.type != SC_EPLATCHES) {
    dllOutputError("dllGetSpyEcc - A type other than eplatches was found on the lookup!\n");
    return ECMD_INVALID_SPY;
  }

  /* We'll read out the in{} and out{} latches via getspy */
  sedcSpyContainer tempDC;
  tempDC.type = SC_AEI; // We need to set this to fake out the DA code
  sedcEplatchesEntry tempECC;
  ecmdDataBuffer goodECC;
  // in{} latches
  tempECC = myDC.getEplatchesEntry();
  tempDC.setAEIEntry(tempECC.inSpy); 
  data.dataType = SPYDATA_DATA;
  data.int_data = &inLatches;
  rc = dllGetSpy(i_target, data, tempDC);
  if (rc) return rc;

  // out{} latches
  tempECC = myDC.getEplatchesEntry();
  tempDC.setAEIEntry(tempECC.outSpy);
  data.int_data = &outLatches;
  rc = dllGetSpy(i_target, data, tempDC);
  if (rc) return rc;

  /* Now generate the proper ecc based on the in{} */
  rc = dllGenSpyEcc(i_target, tempECC.function, inLatches, goodECC);
  if (rc) return rc;

  if (!(outLatches == goodECC)) {
    errorMask = outLatches;
    errorMask.setXor(goodECC, 0, goodECC.getBitLength());
    return ECMD_SPY_FAILED_ECC_CHECK;   // The ECC didn't match up, return error.
  } else {
    errorMask.setBitLength(outLatches.getBitLength());
    errorMask.flushTo0();
    return 0;   // The ECC is good!
  }

  return rc;
}


uint32_t dllGenSpyEcc(ecmdChipTarget & i_target, std::string eccfuncName, ecmdDataBuffer& inLatches, ecmdDataBuffer& goodECC) {
  uint32_t rc = ECMD_SUCCESS;

  sedcSpyContainer myDC;
  sedcEccfuncEntry eccfuncEntry;
  std::list<sedcEccfuncLine>::iterator eccfuncIter;
  char outstr[200];

  if (eccfuncName == "ODDPARITY") {
    goodECC.setBitLength(1);
    goodECC.writeBit(0, inLatches.oddParity(0, inLatches.getBitLength()));
  } else if (eccfuncName == "EVENPARITY") {
    goodECC.setBitLength(1);
    goodECC.writeBit(0, inLatches.evenParity(0, inLatches.getBitLength()));
  } else {
    /* See if we can lookup the name of the function before doing anything else */
    rc = dllGetSpyInfo(i_target, eccfuncName.c_str(), myDC);
    if (rc) {
      sprintf(outstr,"dllGenSpyEcc - Problems reading spy '%s' from file!\n", eccfuncName.c_str());
    dllOutputError(outstr);
      return rc;
    } else if (!myDC.valid) {
      sprintf(outstr,"dllGenSpyEcc - Read of spy '%s' from file failed!\n", eccfuncName.c_str());
    dllOutputError(outstr);
      return ECMD_UNKNOWN_FILE;
    } else if (myDC.type != SC_ECCFUNC) {
      dllOutputError("dllGenSpyEcc - A type other than eccfunc was found on the lookup!\n");
      return ECMD_INVALID_SPY;
    }

    /* We found it, get it out of the DC and setup the ecmdDataBuffer before we start looping */
    eccfuncEntry = myDC.getEccfuncEntry();
    ecmdDataBuffer tempBuffer; tempBuffer.setBitLength(eccfuncEntry.inBits);
    goodECC.setBitLength(eccfuncEntry.outBits);
    int numWords = eccfuncEntry.inBits / 32;
    if (eccfuncEntry.inBits % 32) numWords++;
    int outbitOffset = 0;

    /* Now start our looping and working with the data */
    eccfuncIter = eccfuncEntry.eccfuncLines.begin();
    while (eccfuncIter != eccfuncEntry.eccfuncLines.end()) {

      if (eccfuncIter->state & ECF_TABLE) {
        for (int x = 0; x < numWords; x++) {
          tempBuffer.setWord(x, (inLatches.getWord(x) & eccfuncIter->tableValue[x]));
        }

        /* Now all loaded up with the data, gen the parity and insert it into the goodECC */
        if (eccfuncIter->parityType == "odd") {
          goodECC.writeBit(outbitOffset, tempBuffer.oddParity(0, tempBuffer.getBitLength()));
        } else {
          goodECC.writeBit(outbitOffset, tempBuffer.evenParity(0, tempBuffer.getBitLength()));
        }
        outbitOffset++; // Increment where we're sticking the gen'd ecc bits
      }
      eccfuncIter++;
    }
  }

  return rc;
}

/**
  This function specification is the same as defined in ecmdClientCapi.H as GetSpyEccGrouping
*/
uint32_t dllGetSpyEpCheckers(ecmdChipTarget & i_target, const char * i_spyEccGroupName, ecmdDataBuffer & o_inLatches, ecmdDataBuffer & o_outLatches, ecmdDataBuffer & o_eccErrorMask){
  uint32_t rc = ECMD_SUCCESS;


    rc = dllGetSpyEcc(i_target, i_spyEccGroupName, o_inLatches, o_outLatches, o_eccErrorMask);

  return rc;
}


/**
  This function specification is the same as defined in ecmdClientCapi.H as PutSpy
*/
uint32_t dllPutSpy (ecmdChipTarget & i_target, const char * i_spyName, ecmdDataBuffer & i_data){
  dllSpyData fdata;

  fdata.dataType = SPYDATA_DATA;
  fdata.int_data = &i_data;


  return dllPutSpy(i_target,i_spyName,fdata);
}


/**
  This function specification is the same as defined in ecmdClientCapi.H as PutSpyEnum
*/
uint32_t dllPutSpyEnum (ecmdChipTarget & i_target, const char * i_spyName, const std::string i_enumValue){

  dllSpyData fdata;

  fdata.dataType = SPYDATA_ENUM;
  fdata.enum_data = i_enumValue;
  fdata.int_data = NULL;

  return dllPutSpy(i_target,i_spyName,fdata);

}

uint32_t dllPutSpy (ecmdChipTarget & i_target, const char * i_spyName, dllSpyData & i_data){
  uint32_t rc = ECMD_SUCCESS;
  bool enabledCache = false;                    ///< This is turned on if we enabled the cache, so we can disable on exit
  sedcSpyContainer mySpy;
  char outstr[200];

  if (!dllIsRingCacheEnabled()) {
    enabledCache = true;
    dllEnableRingCache();
  }


  /* Retrieve my spy either from the DB or the spydef file */
  rc = dllGetSpyInfo(i_target, i_spyName, mySpy);
  if (rc) {
    sprintf(outstr,"dllPutSpy - Problems reading spy '%s' from file!\n", i_spyName);
    dllOutputError(outstr);
    return rc;
  } else if (!mySpy.valid) {
    sprintf(outstr,"dllPutSpy - Read of spy '%s' from file failed!\n", i_spyName);
    dllOutputError(outstr);
    return ECMD_UNKNOWN_FILE;
  } else if (mySpy.type != SC_AEI) {
    dllOutputError("dllPutSpy - A non alias, idial or edial was passed in!\n");
    return ECMD_INVALID_SPY;
  }


  rc = dllPutSpy(i_target, i_data, mySpy);


  /* Handle ECC here */
  if (!rc) {
    sedcAEIEntry myAIE = mySpy.getAEIEntry();
    if (!myAIE.aeiEpcheckers.empty()) {
      std::list<std::string>::iterator eccIter;
      eccIter = myAIE.aeiEpcheckers.begin();
      while (eccIter != myAIE.aeiEpcheckers.end()) {
        rc = dllPutSpyEcc(i_target, *eccIter);
        if (rc) return rc;
        eccIter++;
      }
    }
  } else { return rc;}

  if (enabledCache) {
    rc = dllFlushRingCache();
    if (rc) {
      dllOutputError("dllPutSpy - Problems flushing the ring cache\n");
    }
    rc = dllDisableRingCache();
  }

  return rc;

}

uint32_t dllPutSpy(ecmdChipTarget & i_target, dllSpyData &data, sedcSpyContainer &spy) {
  uint32_t rc = ECMD_SUCCESS;


  std::list<sedcLatchLine>::iterator lineit;
  std::list<sedcAEIEnum>::iterator enumit;
  ecmdDataBuffer scan;
  ecmdDataBuffer enuminsert;
  ecmdDataBuffer* insertbuffer = NULL;
  ecmdDataBuffer tmpbuffer;
  int num;
  int curaliasbit = 0;
  sedcSpyContainer mySpy = spy;
  uint32_t curstate = SPY_CLOCK_IND;       ///< Current state of what we will accept, we will start by allowing clock independent (or spy's without clock dependence) to run
  std::string spyDomain;                        ///< What domain does this chip belong to
  ecmdClockState_t curClockState;               ///< Current state of spy clock domain

  char outstr[200];
  ecmdChipData chipData;                ///< Chip data to find out bus info
  uint32_t bustype;                             ///< Type of bus we are attached to JTAG vs FSI
  std::string curRing;                          ///< Current ring being used
  uint32_t    curScomAddr;                      ///< Current scom address being used


  /* We got here, we had better be an alias/idial or edial */
  if (mySpy.type != SC_AEI) {
    sprintf(outstr,"dllPutSpy - Spy '%s' is not of type alias, idial or edial\n", mySpy.name.c_str());
    dllOutputError(outstr);
    return ECMD_INVALID_SPY;
  }

  /* Let's find out if we are JTAG of FSI here */
  rc = dllGetChipData(i_target, chipData);
  if (rc) {
    sprintf(outstr,"Problems retrieving chip information on target\n");
    dllRegisterErrorMsg(rc, "dllGetSpy", outstr );
    return rc;
  }
  /* Now make sure the plugin gave us some bus info */
  if (!(chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK)) {
    dllRegisterErrorMsg(ECMD_DLL_INVALID, "dllPutSpy", "eCMD plugin did not implement ecmdChipData.chipFlags unable to determine if FSI or JTAG attached chip\n");
    return ECMD_DLL_INVALID;
  } else if (((chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK) != ECMD_CHIPFLAG_JTAG) &&
             ((chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK) != ECMD_CHIPFLAG_FSI) ) {
    dllRegisterErrorMsg(ECMD_DLL_INVALID, "dllPutSpy", "eCMD plugin returned an invalid bustype in ecmdChipData.chipFlags\n");
    return ECMD_DLL_INVALID;
  }
  /* Store our type */
  bustype = chipData.chipFlags & ECMD_CHIPFLAG_BUSMASK;



  /* Now let's go grab our data */
  sedcAEIEntry spyent = mySpy.getAEIEntry();

  /* See if we need the clock domain */
  if (spyent.states & SPY_CLOCK_ANY)
    rc = dllGetSpyClockDomain(i_target, &spyent, spyDomain);
  if (rc) return rc;


  /* Do some error checking with what we have */
  if ((spyent.states & (SPY_IDIAL | SPY_MAJOR_TYPES_ECC)) || ((spyent.states & SPY_ALIAS) && (data.int_data != NULL))) {
    /* We need to size our buffer here */
    if (data.int_data == NULL) {
      sprintf(outstr,"dllPutSpy - Found an Idial or non-enumerated Alias without a buffer provided for : %s\n", spy.name.c_str());
    dllOutputError(outstr);
      return ECMD_SPY_NOT_ENUMERATED;
    }

    insertbuffer = data.int_data;
  } else {

    if (data.int_data != NULL) {
      sprintf(outstr,"dllPutSpy - Found an Edial or enumerated Alias and a buffer was provided for : %s\n",spy.name.c_str());
    dllOutputError(outstr);
      return ECMD_SPY_IS_EDIAL;
    } else if ((spyent.states & SPY_ALIAS) && !(spyent.states & SPY_ENUM)) {
      sprintf(outstr,"dllPutSpy - Tried to fetch an enum on a non-enumerated alias for : %s\n",spy.name.c_str());
    dllOutputError(outstr);
      return ECMD_SPY_NOT_ENUMERATED;
    }


    /* This is an enum dial or enumerated alias, we are going to find the enum they gave us and setup our data */
    enuminsert.setBitLength(spyent.length);
    insertbuffer = &enuminsert;


    int foundit = 0;
    std::string enumName = data.enum_data;

    /* Transform to upper case for case-insensitive comparisons */
    transform(enumName.begin(), enumName.end(), enumName.begin(), (int(*)(int)) toupper);

    int wordlen = enuminsert.getWordLength();
    for (enumit = spyent.aeiEnums.begin(); enumit != spyent.aeiEnums.end(); enumit ++) {
      if (enumit->enumName == enumName) {
        /* Here it is */
        foundit = 1;
        for (int idx = 0; idx < wordlen; idx ++) {
          enuminsert.setWord(idx, enumit->enumValue[idx]);
        }
        break;
      }
    }
    if (!foundit) {
      /* We didn't find an enum that matched , this is bad */
      sprintf(outstr,"dllPutSpy - Invalid enum name provided on write of : %s\n",spy.name.c_str());
    dllOutputError(outstr);
      return ECMD_INVALID_SPY_ENUM;
    }
    

  }
    

  /* Ok, here goes the meat */
  for (lineit = spyent.aeiLines.begin(); lineit != spyent.aeiLines.end(); lineit ++) {


    /*---------------------*/
    /* MAIN SPY SECTION    */
    /*---------------------*/

    if ((lineit->state  & ~(SPY_SECTION_START | SPY_MAJOR_TYPES)) == 0) {
      /* this is the start of the spy , woohoo */
      /* Oh and we have nothing to do for it */

    } else if ((lineit->state  & ~(SPY_SECTION_END | SPY_MAJOR_TYPES)) == 0) {
      /* Do Nothing */





    /*---------------------*/
    /* RING     SECTION    */
    /*---------------------*/
    } else if (lineit->state == (SPY_SECTION_START | SPY_RING)) {
      /* Check to see if we are in the right state */
      if (curstate & SPY_CLOCK_ANY) {
        /* This is a new ring */
        curRing = lineit->latchName;
        rc = dllGetRing(i_target, curRing.c_str(), scan);
        if (rc) return rc;
      }
    } else if (lineit->state == (SPY_SECTION_END | SPY_RING)) {
      /* Write the ring back in , hopefully cached */
      if (curstate & SPY_CLOCK_ANY) {

        rc = dllPutRing(i_target, curRing.c_str(), scan);
        if (rc) return rc;
      }

      



    /*---------------------*/
    /* SCOM     SECTION    */
    /*---------------------*/
    } else if (lineit->state == (SPY_SECTION_START | SPY_SCOM)) {
      /* Check to see if we are in the right state */
      if (curstate & SPY_CLOCK_ANY) {
        /* This is a new scom */
        num = sscanf(lineit->latchName.c_str(), "%x",&curScomAddr);
        if (num != 1) {
          sprintf(outstr, "dllPutSpy - Unable to determine scom address (%s) from spy definition (%s)\n", lineit->latchName.c_str(), spy.name.c_str());
          dllOutputError(outstr);
          return ECMD_INVALID_SPY;
        }
        rc = dllGetScom(i_target, curScomAddr, scan);
        if (rc) return rc;
      }
    } else if (lineit->state == (SPY_SECTION_END | SPY_SCOM)) {

      /* Write the scom back in  */

      if (curstate & SPY_CLOCK_ANY) {
        rc = dllPutScom(i_target, curScomAddr, scan);
        if (rc) return rc;
      }

    /*---------------------*/
    /* CLOCK ON SECTION    */
    /*---------------------*/
    } else if (lineit->state == (SPY_SECTION_START | SPY_CLOCK_ON)) {
      /* We hit a clock on section - we need to figure out if the chip clocks are on */
      rc = dllQueryClockState (i_target, spyDomain.c_str(), curClockState);
      if (rc) return rc;
      if ((curClockState == ECMD_CLOCKSTATE_ON) || (curClockState == ECMD_CLOCKSTATE_NA)) {
        curstate &= ~(SPY_CLOCK_ANY);
        curstate |= SPY_CLOCK_ON;
      } else 
        curstate &= ~(SPY_CLOCK_ANY);

    } else if (lineit->state == (SPY_SECTION_END | SPY_CLOCK_ON)) {
      /* We are done with the clock dep section, lets set our state back to independent */
      curstate &= ~(SPY_CLOCK_ANY);
      curstate |= SPY_CLOCK_IND;




    /*---------------------*/
    /* CLOCK OFF SECTION   */
    /*---------------------*/
    } else if (lineit->state == (SPY_SECTION_START | SPY_CLOCK_OFF)) {
      /* We hit a clock on section - we need to figure out if the chip clocks are on */
      rc = dllQueryClockState (i_target, spyDomain.c_str(), curClockState);
      if (rc) return rc;
      if ((curClockState == ECMD_CLOCKSTATE_OFF) || (curClockState == ECMD_CLOCKSTATE_NA)  || (curClockState == ECMD_CLOCKSTATE_UNKNOWN)  ) {
        curstate &= ~(SPY_CLOCK_ANY);
        curstate |= SPY_CLOCK_ON;
      } else 
        curstate &= ~(SPY_CLOCK_ANY);

    } else if (lineit->state == (SPY_SECTION_END | SPY_CLOCK_OFF)) {
      /* We are done with the clock dep section, lets set our state back to independent */
      curstate &= ~(SPY_CLOCK_ANY);
      curstate |= SPY_CLOCK_IND;





    /*---------------------*/
    /* ENUM     SECTION    */
    /*---------------------*/
    } else if (lineit->state & SPY_ENUM) {
      /* We don't care about the enum lines */

    /*---------------------*/
    /* ECC     SECTION     */
    /*---------------------*/
    } else if (lineit->state & SPY_EPCHECKERS) {
      /* We don't care about the ecc lines */

    /*---------------------*/
    /* GROUP    SECTION    */
    /*---------------------*/

    } else if (lineit->state == (SPY_SECTION_START | SPY_GROUP_BITS)) {
      /* This is a new group */
      /* Reset out bit pointer */
      curaliasbit = 0;


    } else if (lineit->state == (SPY_SECTION_END | SPY_GROUP_BITS)) {
      /* Do Nothing */




    /*---------------------*/
    /* DATA     SECTION    */
    /*---------------------*/
    } else if (lineit->length == -1) {
      /* We shouldn't get here, the only thing left should be actual latchs */
      dllOutputError("dllPutSpy - Problems reading spy info found invalid data\n");
      return ECMD_FAILURE;
    } else if (curstate & SPY_CLOCK_ANY) {
      /* This had better be valid latches so let's go extract */

      /* Handle deadbits */
      if (lineit->state & SPY_DEADBITS) {
        /* We ignore the data here */
        curaliasbit += lineit->length;

      /* Inverted bits */
      } else if (lineit->state & SPY_INVERT) {

        if (lineit->length == 1) {
          if (insertbuffer->isBitSet(curaliasbit++)) {
            if (bustype == ECMD_CHIPFLAG_FSI)
              scan.clearBit(lineit->offsetFSI);
            else // JTAG
              scan.clearBit(lineit->offsetJTAG);
          } else {
            if (bustype == ECMD_CHIPFLAG_FSI)
              scan.setBit(lineit->offsetFSI);
            else // JTAG
              scan.setBit(lineit->offsetJTAG);
          }
        } else {
          /* We need to grab more data */
          insertbuffer->extract(tmpbuffer, curaliasbit, lineit->length);
          tmpbuffer.invert();   /* Invert bits */
          /* scom's are not reversed so we grab like FSI mode */
          if ((bustype == ECMD_CHIPFLAG_FSI) || (lineit->state & SPY_SCOM)) 
            scan.insert(tmpbuffer, lineit->offsetFSI, lineit->length);
          else { // JTAG
            tmpbuffer.reverse();
            scan.insert(tmpbuffer, (lineit->offsetJTAG - lineit->length + 1), lineit->length);
          }
          curaliasbit += lineit->length;
        }

        /* Standard latch bits */
      } else {

          if (lineit->length == 1) {
            if (insertbuffer->isBitSet(curaliasbit++)) {
              if (bustype == ECMD_CHIPFLAG_FSI)
                scan.setBit(lineit->offsetFSI);
              else // JTAG
                scan.setBit(lineit->offsetJTAG);
            } else {
              if (bustype == ECMD_CHIPFLAG_FSI)
                scan.clearBit(lineit->offsetFSI);
              else // JTAG
                scan.clearBit(lineit->offsetJTAG);
            }
          } else {
            /* We need to grab more data */
            insertbuffer->extract(tmpbuffer, curaliasbit, lineit->length);
            /* scom's are not reversed so we grab like FSI mode */
            if ((bustype == ECMD_CHIPFLAG_FSI) || (lineit->state & SPY_SCOM)) 
              scan.insert(tmpbuffer, lineit->offsetFSI, lineit->length);
            else { // JTAG
              tmpbuffer.reverse();
              scan.insert(tmpbuffer, (lineit->offsetJTAG - lineit->length + 1), lineit->length);
            }
            curaliasbit += lineit->length;
          }
        
      }

    }
  }

  return rc;
}


uint32_t dllPutSpyEcc(ecmdChipTarget & i_target, std::string epcheckerName) {
  uint32_t rc = ECMD_SUCCESS;
  sedcSpyContainer myDC;
  dllSpyData data;
  char outstr[200];

  /* Retrieve my spy either from the DB or the spydef file */
  rc = dllGetSpyInfo(i_target, epcheckerName.c_str(), myDC);
  if (rc) {
    sprintf(outstr,"dllPutSpyEcc - Problems reading spy '%s' from file!\n", epcheckerName.c_str());
        dllOutputError(outstr);
    return rc;
  } else if (!myDC.valid) {
    sprintf(outstr,"dllPutSpyEcc - Read of spy '%s' from file failed!\n", epcheckerName.c_str());
        dllOutputError(outstr);
    return ECMD_UNKNOWN_FILE;
  } else if (myDC.type != SC_EPLATCHES) {
    dllOutputError("dllPutSpyEcc - A type other than eplatches was found on the lookup!\n");
    return ECMD_INVALID_SPY;
  }

  /* We'll read out the in{} and out{} latches via getspy */
  sedcSpyContainer tempDC;
  tempDC.type = SC_AEI; // We need to set this to fake out the DA code
  sedcEplatchesEntry tempECC;
  ecmdDataBuffer goodECC, inLatches;
  // in{} latches
  tempECC = myDC.getEplatchesEntry();
  tempDC.setAEIEntry(tempECC.inSpy); 

  data.dataType = SPYDATA_DATA;
  data.int_data = &inLatches;
  rc = dllGetSpy(i_target, data, tempDC);
  if (rc) return rc;

  /* Now generate the proper ecc based on the in{} */
  rc = dllGenSpyEcc(i_target, tempECC.function, inLatches, goodECC);
  if (rc) return rc;

  /* Now write out the goodECC to the out{} */
  tempECC = myDC.getEplatchesEntry();
  tempDC.setAEIEntry(tempECC.outSpy);
  data.dataType = SPYDATA_DATA;
  data.int_data = &goodECC;
  rc = dllPutSpy(i_target, data, tempDC);
  if (rc) return rc;


  return rc;
}

uint32_t dllGetSpiesInfo(ecmdChipTarget & i_target, std::list<sedcSpyContainer>& returnSpyList) {

  uint32_t rc = 0;

  std::ifstream spyFile, hashFile;
  std::string spyFilePath;
  std::string spyHashFilePath;
  std::list<sedcHashEntry> spyKeysList;
  std::list<sedcHashEntry>::iterator searchSpy;
  uint32_t buildflags = 0;
  char outstr[200];
  sedcSpyContainer returnSpy;
  
  /* ----------------------------------------------------------------- */
  /*  Try to find the spy position from the hash file		     */
  /* ----------------------------------------------------------------- */
  rc = dllQueryFileLocation(i_target, ECMD_FILE_SPYDEFHASH, spyHashFilePath);
  if (!rc) {
    hashFile.open(spyHashFilePath.c_str(),
  		  std::ios::ate | std::ios::in | std::ios::binary); /* go to end of file upon opening */

    /* If we have a hash file, look for it in there */
    if (!hashFile.fail()) {
      rc = dllGetSpyListHash(hashFile, spyKeysList);
      if (rc) {
       sprintf(outstr,"dllGetSpiesInfo - Problems in getting spylist from the spydefhash file!\n");
       dllOutputError(outstr);
       return rc;
      }
    } else {
       sprintf(outstr,"dllGetSpiesInfo - Unable to open spydefhash file: %s!\n", spyHashFilePath.c_str());
       dllOutputError(outstr);
       return ECMD_INVALID_SPY;
    }
  }
  else {
    return ECMD_UNKNOWN_FILE;
  }
  if (spyKeysList.empty()) {
     sprintf(outstr,"dllGetSpiesInfo - Unable to find any spies from the hashfile!\n");
     dllOutputError(outstr);
     return ECMD_INVALID_SPY;
  }
  //Not handling getting the list from the spydef file incase the hashfile method fails
  /* Couldn't find it in the hash file, try a straigh linear search */
  /*
  if (!foundSpy) {
    foundSpy = dllLocateSpy(spyFile, spy_name);
  }

  // If we made it here, we got nothing.. 
  if (!foundSpy) {
    sprintf(outstr,"dllGetSpyInfo - Unable to find spy \"%s\"!\n", spy_name.c_str());
    dllOutputError(outstr);
    returnSpy.valid = 0;
    return ECMD_INVALID_SPY;
  }
  */
  
  /* Let's get the path to the spydef */
  rc = dllQueryFileLocation(i_target, ECMD_FILE_SPYDEF, spyFilePath);
  if (rc) return rc;
  
  spyFile.open(spyFilePath.c_str());
  if (spyFile.fail()) {
    sprintf(outstr,"dllGetSpyInfo - Unable to open spy file : %s\n", spyFilePath.c_str());
    dllOutputError(outstr);
    returnSpy.valid = 0;
    return ECMD_INVALID_SPY;
  }
  for (searchSpy = spyKeysList.begin(); searchSpy != spyKeysList.end(); searchSpy++) {
    spyFile.seekg(searchSpy->filepos);
    
    /* Now that we have our position in the file, call the parser and read it in */
    std::vector<std::string> errMsgs; /* This should be empty all the time */
    returnSpy = sedcSpyParser(spyFile, errMsgs, buildflags);
    if (!errMsgs.empty()) {
      returnSpy.valid = 0;
    }
    else if (returnSpy.type != SC_SYNONYM){
      returnSpyList.push_back(returnSpy);
    }
  }
  spyFile.close();
  hashFile.close();
  return 0;
}


uint32_t dllGetSpyInfo(ecmdChipTarget & i_target, const char* name, sedcSpyContainer& returnSpy) {

  uint32_t rc = 0;

  std::ifstream spyFile, hashFile;
  std::string spyFilePath;
  std::string spyHashFilePath;
  uint32_t key;
//  std::list<sedcSpyContainer>::iterator searchSpy;
  std::string spy_name;
  int foundSpy = 0;
  returnSpy.valid = 0;
  uint32_t buildflags = 0;
  char outstr[200];

  /* Convert to a STL string */
  spy_name = name;
  transform(spy_name.begin(), spy_name.end(), spy_name.begin(), (int(*)(int)) toupper);

  /* Look in the DB to see if we've read this in already */
  returnSpy.setName(spy_name);

  do {
//    searchSpy = find(spies.begin(), spies.end(), returnSpy);

//    if (searchSpy != spies.end()) { /* Found! */

//      returnSpy = (*searchSpy);

//    } else { /* Not Found */

      /* Let's get the path to the spydef */
      rc = dllQueryFileLocation(i_target, ECMD_FILE_SPYDEF, spyFilePath);
      if (rc) return rc;
      
      spyFile.open(spyFilePath.c_str());
      if (spyFile.fail()) {
        sprintf(outstr,"dllGetSpyInfo - Unable to open spy file : %s\n", spyFilePath.c_str());
        dllOutputError(outstr);
        returnSpy.valid = 0;
        return ECMD_INVALID_SPY;
      }

      /* ----------------------------------------------------------------- */
      /*  Try to find the spy position from the hash file                */
      /* ----------------------------------------------------------------- */
      key = ecmdHashString32(returnSpy.name.c_str(),0);

      rc = dllQueryFileLocation(i_target, ECMD_FILE_SPYDEFHASH, spyHashFilePath);
      if (!rc) {
        hashFile.open(spyHashFilePath.c_str(),
                      std::ios::ate | std::ios::in | std::ios::binary); /* go to end of file upon opening */

        /* If we have a hash file, look for it in there */
        if (!hashFile.fail()) {
          foundSpy = dllLocateSpyHash(spyFile, hashFile, key, returnSpy.name);
        }
        hashFile.close();
      } else { return rc;}

      /* Couldn't find it in the hash file, try a straigh linear search */
      /*if (!foundSpy) {
        foundSpy = dllLocateSpy(spyFile, returnSpy.name);
      }*/

      /* If we made it here, we got nothing.. */
      if (!foundSpy) {
        sprintf(outstr,"dllGetSpyInfo - Unable to find spy \"%s\"!\n", returnSpy.name.c_str());
        dllOutputError(outstr);
        returnSpy.valid = 0;
        return ECMD_INVALID_SPY;
      }

      /* Now that we have our position in the file, call the parser and read it in */
      std::vector<std::string> errMsgs; /* This should be empty all the time */
      returnSpy = sedcSpyParser(spyFile, errMsgs, buildflags);
      if (!errMsgs.empty()) {
        sprintf(outstr,"dllGetSpyInfo - Error occured in the parsing of the spy : %s!\n",returnSpy.name.c_str());
        dllOutputError(outstr);
        returnSpy.valid = 0;
        return ECMD_INVALID_SPY;
      }
      /* Everything looks good, let's get out of here */
//      spies.push_front(returnSpy);
      spyFile.close();
//    }

    /* If we found a synonym, go back and look up what it is suppose to point to */
    if (returnSpy.type == SC_SYNONYM) {
      sedcSynonymEntry syn = returnSpy.getSynonymEntry();
      returnSpy.name = syn.realName;  /* Change the name to point to real name so when we do the look up, we find the spy/ecclatch/etc.. */
    }
  } while (returnSpy.type == SC_SYNONYM);

  return 0;
}

uint32_t dllGetSpyClockDomain(ecmdChipTarget & i_target, sedcAEIEntry* spy_data, std::string & o_domain) {
  uint32_t rc = 0;
  bool foundit = false;
  std::list<sedcLatchLine>::iterator lineit;
  char outstr[200];
  std::list<ecmdRingData> ringQueryData;
  ecmdQueryDetail_t detail = ECMD_QUERY_DETAIL_HIGH;
  
  /* Walk through the lines looking for our first scom/ring line */
  for (lineit = spy_data->aeiLines.begin(); lineit != spy_data->aeiLines.end(); lineit ++) {

    if (lineit->state == (SPY_SECTION_START | SPY_RING)) {
      /* Now we need to query the ring data */
      rc = dllQueryRing(i_target, ringQueryData, lineit->latchName.c_str(), detail);
      if (rc) return rc;
      if (!ringQueryData.empty())
        o_domain = ringQueryData.begin()->clockDomain;

      foundit = true;
      break;
    } else if (lineit->state == (SPY_SECTION_START | SPY_SCOM)) {
#if 0
      /* Right now there isn't an api to check scom clock domain 2/2/05 cje */

      BIT32 addr;
      int num = sscanf(lineit->latchName.c_str(), "%x",&addr);
      if (num != 1) {
        sprintf(outstr, "Unable to determine scom address (%s) from spy definition (%s)\n", lineit->latchName.c_str(), spy_name);
        out.error("Chip::getSpyClockDomain",outstr);
        return ECMD_INVALID_SPY;
      }
      rc = getScomClockDomain(addr, o_domain, 0); if(rc) return rc;
      foundit = true;
      break;
#endif
    }

  }

  if (!foundit) {
    sprintf(outstr,"dllGetSpyClockDomain - Unable to find a ring or scom definition in spy\n");
    dllOutputError(outstr);
    return ECMD_INVALID_SPY;
  }    

  return rc;

}

int dllGetSpyListHash(std::ifstream &hashFile, std::list<sedcHashEntry> &spyKeysList) {
  
  int entrysize = sizeof(struct sedcHashEntry); /* We need this to be able to traverse the binary hash file */
  sedcHashEntry curhash;
  long filepos;
  int numentries;
  int rc = ECMD_SUCCESS;
  
  /* first get the size of the hash file */
  filepos = hashFile.tellg();			/* get end of file position	*/
  hashFile.seekg(0, std::ios::beg);			/* go back to beginning of file */
  numentries = filepos / entrysize;
  
  if (filepos < entrysize) { return ECMD_FAILURE; }  /* we got problems */
 
  for (int i=0; i< numentries; i++) {
 
    hashFile.seekg(i * entrysize);	     /* position into file */
    hashFile.read((char*)&(curhash.key), 4); /* read 4-byte key */
    /* We need to byte swap this guy */
    curhash.key = htonl(curhash.key);
    hashFile.read((char*)&(curhash.filepos), sizeof(curhash.filepos));
    /* We need to byte swap this guy */
    curhash.filepos = htonl(curhash.filepos);
    spyKeysList.push_back(curhash);
  }

  if (spyKeysList.empty()) {
    rc = ECMD_FAILURE;
  }
  else {
    rc = ECMD_SUCCESS;
  }
  return rc;
}

int dllLocateSpyHash(std::ifstream &spyFile, std::ifstream &hashFile, uint32_t key, std::string spy_name) {

  int found = 0;
  sedcHashEntry curhash;
  int entrysize = sizeof(struct sedcHashEntry); /* We need this to be able to traverse the binary hash file */
  long filepos;
  std::string line;
  int numentries;

  /* first get the size of the hash file */
  filepos = hashFile.tellg();                   /* get end of file position     */
  hashFile.seekg(0, std::ios::beg);			/* go back to beginning of file */
  hashFile.seekg(0, std::ios::beg);			/* go back to beginning of file */
  numentries = filepos / entrysize;

  if (filepos < entrysize) { return 0; }  /* we got problems */

  /* Binary Search For It */
  int low = 0;
  int high = numentries - 1;
  int cur;

  while (low < high) {

    cur = (high + low) / 2;

    hashFile.seekg(cur * entrysize);         /* position into file */ 
    hashFile.read((char*)&(curhash.key), 4); /* read 4-byte key */
    /* We need to byte swap this guy */
    curhash.key = htonl(curhash.key);

    if (key > curhash.key)
      low = cur + 1;
    else   /* key < hash || key == hash */
      high = cur;
  }

  cur = low;
  hashFile.seekg(cur * entrysize);         /* position into file */

  do {  /* at least once */

    hashFile.read((char*)&(curhash.key), sizeof(curhash.key));
    /* We need to byte swap this guy */
    curhash.key = htonl(curhash.key);
    if (key != curhash.key) return 0;      /* we should have been at the right spot */
    hashFile.read((char*)&(curhash.filepos), sizeof(curhash.filepos));
    /* We need to byte swap this guy */
    curhash.filepos = htonl(curhash.filepos);

    spyFile.seekg(curhash.filepos);              /* go to that spot in the spy file */
    if (spyFile.fail()) {
      /* We must have seeked to a bad spot, this hash file is really messed up */
      spyFile.clear();
      return 0;
    }
    getline(spyFile,line,'\n');                 /* read the spy and then clean off the extras*/

    /* We found something here let's see if it the spy we want */
    /* Strip the front off */
    int strPos = line.find_first_not_of(WHITESPACE,line.find_first_of(WHITESPACE,0));
    if (strPos != NOT_FOUND) {
      std::string tmp = line.substr(strPos,line.length());
      /* Strip the end off */
      strPos = tmp.find_first_of(WHITESPACE,0);
      if (strPos != NOT_FOUND) {
        tmp = tmp.erase(strPos, tmp.length());

        transform(tmp.begin(), tmp.end(), tmp.begin(), (int(*)(int)) toupper);

        if (tmp == spy_name)
          found = 1;      /* found it! */
      }
    }
  } while (!found);

  if (found) {
    /* Now rewind the file to beginning of the spy so the parser will be able to read it properly */
    spyFile.seekg(curhash.filepos);
  }

  return found;
}

uint32_t dllLocateSpy(std::ifstream &spyFile, std::string spy_name) {

  int found = 0;
  std::string line;
  long filepos;
  sedcFileLine myLine;

  /* Get the file back to the beginning so we can search */
  spyFile.clear();
  spyFile.seekg(0, std::ios::beg);

  /* One to kick it off */
  filepos = spyFile.tellg();                   /* get end of file position     */
  getline(spyFile,line,'\n');
  while (!spyFile.eof() && !found) {

    if ((line.length() > 4) &&
        (((line[0] == 'a') && (line.substr(0,5) == "alias")) ||
         ((line[0] == 'i') && (line.substr(0,5) == "idial")) ||
         ((line[0] == 'e') && ((line.substr(0,5) == "edial") || (line.substr(0,7) == "eccfunc") || (line.substr(0,9) == "eplatches"))) ||
         ((line[0] == 's') && (line.substr(0,7) == "synonym"))) ) {

      /* We found something here let's see if it the spy we want */
      /* Strip the front off */
      std::string tmp = line.substr(line.find_first_not_of(WHITESPACE, line.find_first_of(WHITESPACE,0)), line.length());
      sedcCreateSpyTokens(line, WHITESPACE, myLine);
      transform(myLine.tokens[1].begin(), myLine.tokens[1].end(), myLine.tokens[1].begin(), toupper);


      if (myLine.tokens[1] == spy_name) {
        /* We found it */
        found = 1;
        spyFile.seekg(filepos);
        break;
      }

    }

    /* Must have not found anything, get a new line and keep looping */
    filepos = spyFile.tellg();                   /* get end of file position     */
    getline(spyFile,line,'\n');
  }
  return found;
}


#endif

