/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File sedcScomdefParser.C                                   
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

// Module Description **************************************************
//
// Description: 
//
// End Module Description **********************************************

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                     09/23/03 albertj  Initial Creation
//
// End Change Log *****************************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <list>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <stdio.h>
#include <sedcScomdefParser.H>
#include <sedcCommonParser.H>
#include <sedcDefines.H>



void sedcScomdefParser(sedcScomdefEntry &returnScom, std::ifstream &scomdefFile, std::vector<std::string> &errMsgs, uint32_t runtimeFlags) {

  returnScom.valid = 1; /* We'll start out assuming things are good and dis-prove if necessary */
  std::string line;
  sedcFileLine myLine;
  std::string tempstr;
  size_t linePos;
  uint32_t whatsGoingOn = 0x0;
  std::list<uint32_t> braceOrder;
  uint32_t stateTemp;
  int done = 0;
  sedcLatchLine curLatchLine;
  sedcScomdefDefLine curDefLine;
  int seenBits = 0; // A hack for in the definition section, so I know if I should save away the entry

  /* Make sure the scomdef we are going to return is clean to start */
  returnScom.reset();

  while (returnScom.valid && !done && !scomdefFile.eof()) {

    getline(scomdefFile,line,'\n'); /* Start each time with a new line read */
    sedcCreateScomdefTokens(line,WHITESPACE,myLine);  // Cut each line into tokens seperated on white space

    /******************************************************************************************/
    /* To start, we'll have a bunch of special cases before we start looking for token counts */
    /******************************************************************************************/

    /****************************************/
    /* Handle comment lines and blank lines */
    /****************************************/
    if (myLine.tokens.size() == 0) {
      continue;  // We don't have to do anything, we don't need to write the scomdef back out.  Just read it in.
    }

    /***********************************************/
    /* Look for BEGIN XXXX line and act from there */
    /***********************************************/
    else if (myLine.tokens[0] == "BEGIN") {
      /******************/
      /* Error Checking */
      /******************/
      if (myLine.tokens.size() != 2) {
        returnScom.valid = 0;
        errMsgs.push_back("Found the wrong number of arguments when parsing this line:");
        errMsgs.push_back(myLine.realLine);
        break;
      }

      /*************/
      /* Real Work */
      /*************/
      if (myLine.tokens[1] == "Scom") {
        stateTemp = SCM_SCOM;
        /* If it's original format, then it'll always be in a spy */
        if ((runtimeFlags & RTF_ALL_SCOMS_AVAIL_FOR_SPIES) == RTF_ALL_SCOMS_AVAIL_FOR_SPIES) {
          returnScom.states |= SCM_INSPY;
        }
      } else if (myLine.tokens[1] == "Description")
        stateTemp = SCM_DESCRIPTION;
      else if (myLine.tokens[1] == "Latches")
        stateTemp = SCM_LATCHES;
      else if (myLine.tokens[1] == "Definition")
        stateTemp = SCM_DEFINITION;
      else {
        returnScom.valid = 0;
        errMsgs.push_back("(sedcScomdefParser) :: a BEGIN other than Scom/Description/Latches/Definition found!");
        break;
      }

      returnScom.states |= stateTemp;
      whatsGoingOn |= stateTemp;
    }

    /***********************************************/
    /* Look for END XXXX line and act from there */
    /***********************************************/
    else if (myLine.tokens[0] == "END") {
      /******************/
      /* Error Checking */
      /******************/
      if (myLine.tokens.size() != 2) {
        returnScom.valid = 0;
        errMsgs.push_back("Found the wrong number of arguments when parsing this line:");
        errMsgs.push_back(myLine.realLine);
        break;
      }

      /*************/
      /* Real Work */
      /*************/
      if (myLine.tokens[1] == "Scom")
        stateTemp = SCM_SCOM;
      else if (myLine.tokens[1] == "Description")
        stateTemp = SCM_DESCRIPTION;
      else if (myLine.tokens[1] == "Latches")
        stateTemp = SCM_LATCHES;
      else if (myLine.tokens[1] == "Definition")
        stateTemp = SCM_DEFINITION;
      else {
        returnScom.valid = 0;
        errMsgs.push_back("(sedcScomdefParser) :: a END other than Scom/Description/Latches/Definition found!");
        break;
      }

      /***********************/
      /* More Error Checking */
      /***********************/
      if (!(whatsGoingOn & stateTemp)) {
        returnScom.valid = 0;
        errMsgs.push_back("(sedcScomdefParser) :: Found an END section for a block I wasn't in!");
        break;
      }

      /* Turn off the state we are ending in whatsGoingOn */
      whatsGoingOn ^= stateTemp;

      /* We're at the end, lets do something stuff to clean up and make sure we found everything */
      if (stateTemp == SCM_SCOM) {
        /* First, push onto the list the last curDefLine we read in if we ever had a def section (which we should always have) */
        if (seenBits)
          returnScom.definition.push_back(curDefLine);
        /* I could make sure I have seen all the states I need, but I'm lazy for now and will just mark it done */
        done = 1;
      }
    }

    /************************************************************/
    /* Look for single line keywords like state, address, etc.. */
    /************************************************************/
    else if (!(whatsGoingOn & (SCM_DESCRIPTION | SCM_DEFINITION))) {
       /* Address */
       if (myLine.tokens[0] == "Address") {
         /* Pull off the braces */
         tempstr = myLine.tokens[2];
         tempstr.erase(0,1);
         tempstr.erase((tempstr.length()-1), 1);
         /* Now tokenize on comas and shove onto the list */
         sedcFileLine mySubLine;
         sedcCreateScomdefTokens(tempstr,",",mySubLine);  // Cut each line into tokens seperated on comma
         uint64_t tempAddress;
         for (uint32_t x = 0; x < mySubLine.tokens.size(); x++) {
           sscanf(mySubLine.tokens[x].c_str(), "%llX", &tempAddress);
           returnScom.addresses.push_back(tempAddress);
         }
         returnScom.states |= SCM_ADDRESS;
       }

       /* Name */
       else if (myLine.tokens[0] == "Name") {
         returnScom.name = myLine.tokens[2];
         returnScom.states |= SCM_NAME;
       }

       /* Mask */
       else if (myLine.tokens[0] == "Mask") {
         returnScom.mask = strtoull(myLine.tokens[2].c_str(), NULL, 16);
         returnScom.states |= SCM_MASK;
       }

       /* ClockDomain */
       else if (myLine.tokens[0] == "ClockDomain") {
         if (myLine.tokens.size() > 2) {  // Fix for missing clock domain info currently 
           returnScom.clkdomain = myLine.tokens[2];
         }
         returnScom.states |= SCM_CLKDOMAIN;
       }

       /* State */
       else if (myLine.tokens[0] == "State") {
         returnScom.clkstate = sedcStringToClkState(myLine.tokens[2]);
         returnScom.states |= SCM_STATE;
       }

       /* Include in spy */
       else if (myLine.tokens[0] == "Spy") {
         if (myLine.tokens[2] == "YES") {
          returnScom.states |= SCM_INSPY;
         }
       }

       else if (whatsGoingOn & SCM_LATCHES) {
         curLatchLine.reset(); // Clean it out from last time
         sedcParseLatchLine(curLatchLine, myLine, runtimeFlags);
         returnScom.latches.push_back(curLatchLine);
       }
    }

    /*******************************************************************/
    /* Now handle the sections like description/latches and definition */
    /*******************************************************************/
    else if (whatsGoingOn & SCM_DESCRIPTION) {
      /* This is the simplest one.  We'll string the white space off the front of the line and push it onto the list */
      tempstr = line;
      linePos = tempstr.find_first_not_of(WHITESPACE,0);
      tempstr.erase(0, linePos);
      returnScom.description.push_back(tempstr);
    }

    else if (whatsGoingOn & SCM_DEFINITION) {
      if (myLine.tokens[0] == "bits:") {
        /* Save away */
        if (seenBits)
          returnScom.definition.push_back(curDefLine);
        seenBits = 1;
        /* Reset my definition line */
        curDefLine.reset();
        /* Now setup the lhs, rhs and length fields */
        linePos = myLine.tokens[1].find(":");
        if (linePos != NOT_FOUND) {
          /* lhsNum */
          tempstr = myLine.tokens[1].substr(0, linePos);
          curDefLine.lhsNum = atoi(tempstr.c_str());
          /* rhsNum */
          tempstr = myLine.tokens[1].substr((linePos+1), myLine.tokens[1].length());
          curDefLine.rhsNum = atoi(tempstr.c_str());
          /* length */
          curDefLine.length = (curDefLine.rhsNum - curDefLine.lhsNum) + 1;

        } else { // Single bit
          curDefLine.lhsNum = atoi(myLine.tokens[1].c_str());
          curDefLine.length = 1;
        }
        /* Now save away the dialname */
        curDefLine.dialName = myLine.tokens[2];
      } else {
        /* Must be more descriptive text lines, push those onto the list */
        /* We'll strip the white space off the front of the line and push it onto the list */
        tempstr = line;
        linePos = tempstr.find_first_not_of(WHITESPACE,0);
        tempstr.erase(0, linePos);
        curDefLine.detail.push_back(tempstr);
      }
    }
  }
}

void sedcCreateScomdefTokens(std::string line, const char* seperators, sedcFileLine &myLine) {

  size_t curStart = 0, curEnd = 0;
  std::string token;

  myLine.realLine = line;
  myLine.tokens.clear();
  myLine.comment = "";

  curStart = line.find_first_not_of(seperators, curEnd);
  while (curStart != NOT_FOUND) {
    if (line[curStart] == '*') {  // Comment found - save away rest of line and bail
      myLine.comment = line.substr(curStart,line.length());
      break;
    } 
    curEnd = line.find_first_of(seperators,curStart);
    if (curEnd != NOT_FOUND)
      token = line.substr(curStart, curEnd-curStart);
    else // No More white space, but still a token
      token = line.substr(curStart,line.length());
    myLine.tokens.push_back(token);
    curStart = line.find_first_not_of(seperators, curEnd);
  }
}
