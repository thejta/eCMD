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

/* This source only gets included for ecmdDll's that don't want to implement spy handling */
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

#include <ecmdDllCapi.H>

/* Grab the includes for the engineering data compiler */
#include <sedcDataContainer.H>
#include <sedcStructs.H>
#include <sedcDefines.H>
#include <sedcParser.H>

#undef ecmdClientSpy_C
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
/* Lookup Spy info from a spydef file */
int dllGetSpyInfo(ecmdChipTarget & i_target, const char* name, sedcDataContainer& returnSpy);
/* Search the spy file for our spy */
int dllLocateSpy(std::ifstream &spyFile, std::string spy_name);

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------

/**
  This function specification is the same as defined in ecmdClientCapi.H as ecmdQuerySpy
*/
int dllQuerySpy(ecmdChipTarget & i_target, ecmdSpyData & o_queryData, const char * i_spyName) {
  int rc = ECMD_SUCCESS;
  sedcDataContainer mySpy;
  std::list<sedcSpyEnum>::iterator enumit;
  char outstr[200];

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

  /* Special case thing here.. */
  if (mySpy.type == DC_SYNONYM) {
    /* We have a synonym we need to look up the spy behind it */
    sedcSynonymEntry syn = mySpy.getSynonymEntry();

    rc = dllGetSpyInfo(i_target, syn.realName.c_str(), mySpy);
    if (rc) {
      sprintf(outstr,"dllQuerySpy - Problems reading spy '%s' from file!\n", i_spyName);
      dllOutputError(outstr);
      return ECMD_INVALID_SPY;
    } else if (!mySpy.valid) {
      sprintf(outstr,"dllQuerySpy - Read of spy '%s' from file failed!\n", i_spyName);
      dllOutputError(outstr);
      return ECMD_INVALID_SPY;
    }

  }


  o_queryData.spyName = mySpy.name;
  o_queryData.bitLength = 0;
  o_queryData.isEccChecked = false;
  if (mySpy.type == DC_SPY) {
    sedcSpyEntry spyent = mySpy.getSpyEntry();
    o_queryData.bitLength = spyent.length;
    if (spyent.states & SPY_ALIAS)
      o_queryData.spyType = ECMD_SPYTYPE_ALIAS;
    else if (spyent.states & SPY_IDIAL)
      o_queryData.spyType = ECMD_SPYTYPE_IDIAL;
    else if (spyent.states & SPY_EDIAL)
      o_queryData.spyType = ECMD_SPYTYPE_EDIAL;
    else {
      dllOutputError("dllQuerySpy - Unknown spy type returned\n");
      return ECMD_INVALID_SPY;
    }

    /* Does it have ECC ? */
    if (spyent.states & SPY_ECC) {
      o_queryData.isEccChecked = true;
    }

    /* Let's walk through the enums */
    o_queryData.enums.clear();
    for (enumit = spyent.spyEnums.begin(); enumit != spyent.spyEnums.end(); enumit ++) {
      o_queryData.enums.push_back(enumit->enumName);
    }

    /* The eccGroups */
    o_queryData.eccGroups.clear();

  } else {
    dllOutputError("dllQuerySpy - Unknown spy type returned\n");
    return ECMD_INVALID_SPY;
  }
  o_queryData.clockState = ECMD_CLOCKSTATE_UNKNOWN;

  return rc;
}
/**
  This function specification is the same as defined in ecmdClientCapi.H as getSpy
*/
int dllGetSpy (ecmdChipTarget & i_target, const char * i_spyName, ecmdDataBuffer & o_data){
  int rc = ECMD_SUCCESS;

  return rc;
}

/**
  This function specification is the same as defined in ecmdClientCapi.H as GetSpyEnum
*/
int dllGetSpyEnum (ecmdChipTarget & i_target, const char * i_spyName, std::string & o_enumValue){
  int rc = ECMD_SUCCESS;

  return rc;
}


/**
  This function specification is the same as defined in ecmdClientCapi.H as GetSpyEccGrouping
*/
int dllGetSpyEccGrouping (ecmdChipTarget & i_target, const char * i_spyEccGroupName, ecmdDataBuffer & o_groupData, ecmdDataBuffer & o_eccData, ecmdDataBuffer & o_eccErrorMask){
  int rc = ECMD_SUCCESS;

  return rc;
}


/**
  This function specification is the same as defined in ecmdClientCapi.H as PutSpy
*/
int dllPutSpy (ecmdChipTarget & i_target, const char * i_spyName, ecmdDataBuffer & i_data){
  int rc = ECMD_SUCCESS;

  return rc;
}


/**
  This function specification is the same as defined in ecmdClientCapi.H as PutSpyEnum
*/
int dllPutSpyEnum (ecmdChipTarget & i_target, const char * i_spyName, const std::string i_enumValue){
  int rc = ECMD_SUCCESS;

  return rc;
}



int dllGetSpyInfo(ecmdChipTarget & i_target, const char* name, sedcDataContainer& returnSpy) {

  int rc = 0;
  std::ifstream spyFile;
  std::string spyFilePath;
  std::string spy_name;
  int foundSpy = 0;
  returnSpy.valid = 0;
  uint32_t buildflags = 0;
  char outstr[200];

  /* Convert to a STL string */
  spy_name.insert(0, name);
  transform(spy_name.begin(), spy_name.end(), spy_name.begin(), tolower);


  /* Now look for it in the hash file */
  rc = dllQueryFileLocation(i_target, ECMD_FILE_SPYDEF, spyFilePath);

  spyFile.open(spyFilePath.c_str());
  if (spyFile.fail()) {
    sprintf(outstr,"dllGetSpyInfo - Unable to open spy file : %s\n", spyFilePath.c_str());
    dllOutputError(outstr);
    returnSpy.valid = 0; return ECMD_INVALID_SPY;
  }

  foundSpy = dllLocateSpy(spyFile, spy_name);

  /* If we made it here, we got nothing.. */
  if (!foundSpy) {
    sprintf(outstr,"dllGetSpyInfo - Unable to find spy \"%s\"!\n", spy_name.c_str());
    dllOutputError(outstr);
    returnSpy.valid = 0; return ECMD_INVALID_SPY;
  }

  /* Now that we have our position in the file, call the parser and read it in */
  std::vector<std::string> errMsgs; /* This should be empty all the time */
  returnSpy = sedcParser(spyFile, errMsgs, buildflags);
  if (!errMsgs.empty()) {
    dllOutputError("dllGetSpyInfo - I have no clue why I'm here.. errMsgs should ALWAYS be clear!\n");
    returnSpy.valid = 0; return ECMD_INVALID_SPY;
  }
  spyFile.close();
  return 0;
}


int dllLocateSpy(std::ifstream &spyFile, std::string spy_name) {

  int found = 0;
  std::string line;
  long filepos;

  /* Get the file back to the beginning so we can search */
  spyFile.seekg(0, std::ios::beg);

  /* One to kick it off */
  filepos = spyFile.tellg();                   /* get end of file position     */
  getline(spyFile,line,'\n');
  while (!spyFile.eof() && !found) {

    if (((line[0] == 'a') && (line.substr(0,5) == "alias")) ||
        ((line[0] == 'i') && (line.substr(0,5) == "idial")) ||
        ((line[0] == 'e') && (line.substr(0,5) == "edial")) ||
        ((line[0] == 's') && (line.substr(0,7) == "synonym")) ) {

      /* We found something here let's see if it the alias we want */
      /* Strip the front off */
      std::string tmp = line.substr(line.find_first_not_of(" \t", line.find_first_of(" \t",0)), line.length());
      /* Strip the end off */
      tmp = tmp.erase(tmp.find_first_of(" \t",0), tmp.length());

      transform(tmp.begin(), tmp.end(), tmp.begin(), tolower);

      if (tmp == spy_name) {
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

