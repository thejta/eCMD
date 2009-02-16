/* $Header$ */
// Copyright ***********************************************************
//
// File sedcSpyParser.C
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
//                     01/20/05 albertj  Initial Creation
//
// End Change Log *****************************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define sedcSpyParser_C
#include <list>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <sedcSpyParser.H>
#include <sedcCommonParser.H>
#include <sedcDefines.H>
#include <ecmdSharedUtils.H>

#undef sedcSpyParser_C

sedcSpyContainer sedcSpyParser(std::ifstream &spyFile, std::vector<std::string> &errMsgs, uint32_t &runtimeFlags) {

  sedcSpyContainer returnSC;
  sedcAEIEntry returnAEI;
  sedcSynonymEntry returnSynonym;
  sedcEplatchesEntry returnEplatches;
  sedcEccfuncEntry returnEccfunc;
  int done = 0;
  long entryFilePos = spyFile.tellg();
  std::string line;
  sedcFileLine myLine;
  std::string tempstr;

  while (!done && !spyFile.eof()) {

    getline(spyFile,line,'\n'); /* Start each time with a new line read */
    sedcCreateSpyTokens(line,WHITESPACE,myLine);  // Cut each line into tokens seperated on white space

    /**************************************/
    /* Disregard blank lines and comments */
    /**************************************/
    if (myLine.tokens.size() == 0) {
      continue;
    }

    /*******************************************************/
    /* Look for alias/idial/edial lines and act from there */
    /*******************************************************/
    else if (myLine.tokens[0] == "alias" || myLine.tokens[0] == "idial" || myLine.tokens[0] == "edial") {
      returnSC.type = SC_AEI; /* Set the container type */
      spyFile.seekg(entryFilePos); /* Put it back to start */
      returnAEI = sedcAEIParser(spyFile, errMsgs, runtimeFlags);
      returnSC.valid = returnAEI.valid;
      returnSC.setName(returnAEI.name);
      returnSC.setAEIEntry(returnAEI);
      done = 1;
    } else if (myLine.tokens[0] == "synonym") {
      returnSC.type = SC_SYNONYM; /* Set the container type */
      spyFile.seekg(entryFilePos); /* Put it back to start */
      returnSynonym = sedcSynonymParser(spyFile, errMsgs, runtimeFlags);
      returnSC.valid = returnSynonym.valid;
      returnSC.setName(returnSynonym.name);
      returnSC.setSynonymEntry(returnSynonym);
      done = 1;
    } else if (myLine.tokens[0] == "eplatches") {
      returnSC.type = SC_EPLATCHES; /* Set the container type */
      spyFile.seekg(entryFilePos); /* Put it back to start */
      returnEplatches = sedcEplatchesParser(spyFile, errMsgs, runtimeFlags);
      returnSC.valid = returnEplatches.valid;
      returnSC.setName(returnEplatches.name);
      returnSC.setEplatchesEntry(returnEplatches);
      done = 1;
    } else if (myLine.tokens[0] == "eccfunc") {
      returnSC.type = SC_ECCFUNC; /* Set the container type */
      spyFile.seekg(entryFilePos); /* Put it back to start */
      returnEccfunc = sedcEccfuncParser(spyFile, errMsgs, runtimeFlags);
      returnSC.valid = returnEccfunc.valid;
      returnSC.setName(returnEccfunc.name);
      returnSC.setEccfuncEntry(returnEccfunc);
      done = 1;
    } else {
      returnSC.valid = 0;
      errMsgs.push_back("A type other than alias/idial/edial/eplatches/eccfunc was found when parsing file!");
      errMsgs.push_back("That type was: " + myLine.tokens[0]);
      break;
    }
  }

  return returnSC;
}

sedcAEIEntry sedcAEIParser(std::ifstream &spyFile, std::vector<std::string> &errMsgs, uint32_t &runtimeFlags) {

  sedcAEIEntry returnAEI;
  returnAEI.valid = 1; /* We'll start out assuming things are good and dis-prove if necessary */
  returnAEI.states = 0x0; /* Start with a clean slate */
  returnAEI.length = 0;
  sedcLatchLine curAEILine;
  sedcLatchLine holderAEILine;
  sedcAEIEnum curAEIEnum;
  std::string line;
  sedcFileLine myLine;
  std::string tempstr;
  unsigned int linePos;
  unsigned int whatsGoingOn = 0x0;
  std::list<unsigned int> braceOrder;
  unsigned int stateTemp;
  int done = 0;
  int prevFilePos;  /* This is needed to rewind the file in a couple error scenarios */
  int holderID = -1;
  int prevHolderID = -1;

  /* Let's try and keep track of how many bits we have from the user perspective */
  /* This should be fun! */
  int numGroupBits = 0;
  int numClockOnBits = 0;
  int numClockOffBits = 0;
  int numClockIndBits = 0;

  while (returnAEI.valid && !done) {

    prevFilePos = spyFile.tellg(); /* Save this before we do anything */

    getline(spyFile,line,'\n'); /* Start each time with a new line read */
    curAEILine.reset(); /* Clear out the temp structure from the previous loop */
    sedcCreateSpyTokens(line,WHITESPACE,myLine);  // Cut each line into tokens seperated on white space

    /******************************************************************************************/
    /* To start, we'll have a bunch of special cases before we start looking for token counts */
    /******************************************************************************************/

    /****************************************/
    /* Handle comment lines and blank lines */
    /****************************************/
    if (myLine.tokens.size() == 0) {
      if (myLine.comment.size() != 0 && (runtimeFlags & RTF_RETAIN_COMMENTS)) {
        curAEILine.state |= SPY_COMMENT;
        curAEILine.comment = myLine.comment;
        /* We need to check and see if CVS tags are buried in the comment.  If so, seperate them so they no longer update */
        /* $ Author$ */
        linePos = line.find("$Author");
        if (linePos != NOT_FOUND) {
          curAEILine.comment.replace(linePos, 7, "$ Author");
        }
        /* $ Date$ */
        linePos = line.find("$Date");
        if (linePos != NOT_FOUND) {
          curAEILine.comment.replace(linePos, 5, "$ Date");
        }
        /* $ Header$ */
        linePos = line.find("$Header");
        if (linePos != NOT_FOUND) {
          curAEILine.comment.replace(linePos, 7, "$ Header");
        }
        /* $ Id$ */
        linePos = line.find("$Id");
        if (linePos != NOT_FOUND) {
          curAEILine.comment.replace(linePos, 3, "$ Id");
        }
        /* $ Locker$ */
        linePos = line.find("$Locker");
        if (linePos != NOT_FOUND) {
          curAEILine.comment.replace(linePos, 7, "$ Locker");
        }
        /* $ Log$ */
        linePos = line.find("$Log");
        if (linePos != NOT_FOUND) {
          curAEILine.comment.replace(linePos, 4, "$ Log");
        }
        /* $ Name$ */
        linePos = line.find("$Name");
        if (linePos != NOT_FOUND) {
          curAEILine.comment.replace(linePos, 5, "$ Name");
        }
        /* $ RCSfile$ */
        linePos = line.find("$RCSfile");
        if (linePos != NOT_FOUND) {
          curAEILine.comment.replace(linePos, 8, "$ RCSfile");
        }
        /* $ Revision$ */
        linePos = line.find("$Revision");
        if (linePos != NOT_FOUND) {
          curAEILine.comment.replace(linePos, 9, "$ Revision");
        }
        /* $ Source$ */
        linePos = line.find("$Source");
        if (linePos != NOT_FOUND) {
          curAEILine.comment.replace(linePos, 7, "$ Source");
        }
        /* $ State$ */
        linePos = line.find("$State");
        if (linePos != NOT_FOUND) {
          curAEILine.comment.replace(linePos, 6, "$ State");
        }
      } else {
        continue;
      }
    }

    /*******************************************************/
    /* Look for alias/idial/edial lines and act from there */
    /*******************************************************/
    else if (myLine.tokens[0] == "alias" || myLine.tokens[0] == "idial" || myLine.tokens[0] == "edial" || myLine.tokens[0] == "in" || myLine.tokens[0] == "out") {

      /******************/
      /* Error Checking */
      /******************/
      /* If whatsGoingOn is set, that means we've already seen an alias line and have a brace mismatch*/
      if (whatsGoingOn) {
        spyFile.seekg(prevFilePos);  /* Rewind the file so the error handling doesn't skip this spy */
        returnAEI.valid = 0;
        errMsgs.push_back("Found the start of a new spy while not finished with the current one.  Check your braces!");
        break;
      }
      if (myLine.tokens.size() != 2 && myLine.tokens.size() != 3) {
        returnAEI.valid = 0;
        errMsgs.push_back("Found the wrong number of arguments when parsing this line:");
        errMsgs.push_back(myLine.realLine);
        break;
      }

      /*************/
      /* Real Work */
      /*************/
      if (myLine.tokens[0] == "alias")
        stateTemp = SPY_ALIAS;
      else if (myLine.tokens[0] == "idial")
        stateTemp = SPY_IDIAL;
      else if (myLine.tokens[0] == "edial")
        stateTemp = SPY_EDIAL;
      else if (myLine.tokens[0] == "in")
        stateTemp = SPY_ECCIN;
      else if (myLine.tokens[0] == "out")
        stateTemp = SPY_ECCOUT;
      else {
        returnAEI.valid = 0;
        errMsgs.push_back("(sedcAEIParser) :: a type other than alias/idial/edial found!");
        break;
      }

      if (!(stateTemp & SPY_MAJOR_TYPES_ECC)) {
        /* Check for proper starting brace */
        if (myLine.tokens.size() < 3 || myLine.tokens[2] != "{") {
          returnAEI.valid = 0;
          errMsgs.push_back("You did not have an open brace (\"{\") on your alias/idial/edial declaration line");
          errMsgs.push_back( myLine.realLine );
          break;
        }

        /* Force upper case, save away the name, error check */
        transform(myLine.tokens[1].begin(), myLine.tokens[1].end(), myLine.tokens[1].begin(), toupper);
        returnAEI.name = myLine.tokens[1];
        if (returnAEI.name.find_first_not_of(SPY_NAME_ALLOWED) != NOT_FOUND) {
          returnAEI.valid = 0;
          errMsgs.push_back("An invalid character was found in the spy name!");
          break;
        }
      } else { //SPY_MAJOR_TYPES_ECC
        /* Check for proper starting brace */
        if (myLine.tokens.size() < 2 || myLine.tokens[1] != "{") {
          returnAEI.valid = 0;
          errMsgs.push_back("You did not have an open brace (\"{\") on your in/out declaration line");
          break;
        }
      }

      returnAEI.states |= stateTemp;
      curAEILine.state |= (stateTemp | SPY_SECTION_START);
      whatsGoingOn |= stateTemp;

      /* In EDC mode we have to keep track of the holder ID */
      if (runtimeFlags & RTF_EDC_MODE) holderID++;

      braceOrder.push_back(stateTemp);
    }

    /*******************************************/
    /* Look for the ending } and count them up */
    /*******************************************/
    else if (myLine.tokens[0] == "}") { /* If we found a } to start, that is good.  Then lets see if there are others */
      for (unsigned int x = 0; x < myLine.tokens.size(); x++) {
        stateTemp = braceOrder.back();
        braceOrder.pop_back();
        /******************/
        /* Error Checking */
        /******************/
        if (!(stateTemp & whatsGoingOn)) {
          returnAEI.valid = 0;
          errMsgs.push_back("Mismatch between braceOrder state and whatsGoingOn!");
          break;
        }
        /* If whatsGoingOn is empty, we shouldn't see any more braces */
        if (!whatsGoingOn) {
          returnAEI.valid = 0;
          errMsgs.push_back("We emptied out whatsGoingOn and still found braces!");
          break;
        }
        /* Check for invalid data type */
        if (myLine.tokens[x] != "}") {
          returnAEI.valid = 0;
          errMsgs.push_back("Looking for closing braces and found data other than \"}\"!");
          break;
        }

        /*************/
        /* Real Work */
        /*************/
        whatsGoingOn ^= stateTemp;
        /* Conclusion to not wanting to include rings in our build */
        //if (runtimeFlags & RTF_EDC_MODE && stateTemp == SPY_RING) {
        if (runtimeFlags & RTF_EDC_MODE && stateTemp & SPY_MINOR_TYPES) {
          continue; // We don't want to create a curAEILine entry.  The builder will take care of that for us.
        }
        curAEILine.reset(); /* Make sure it's clear here since we are doing special case looping */
        curAEILine.state |= (stateTemp | SPY_SECTION_END);

        /* Push what we found out onto the stack */
        returnAEI.aeiLines.push_back(curAEILine);
      }

      /* We've gotten any of the braces that might be on the line and erased them.  If whatsGoingOn is now empty, we're done! */
      if (!whatsGoingOn) done = 1;

      continue; // We don't want to push a second copy of the returnAEI onto the stack, we handled it above
    }

    /******************************************************************/
    /* We think we're in an enum{} section, try and parse those lines */
    /******************************************************************/
    else if (whatsGoingOn & SPY_ENUM) {
      curAEIEnum = sedcParseEnumLine(myLine, returnAEI.valid, errMsgs, runtimeFlags);
      if (!returnAEI.valid) break;
      returnAEI.aeiEnums.push_back(curAEIEnum);
      continue; /* We don't want to push any sort of info back into the aeiLines */
    }

    /************************************************************************/
    /* We think we're in an epcheckers{} section, try and parse those lines */
    /************************************************************************/
    else if (whatsGoingOn & SPY_EPCHECKERS) {
      if (!(runtimeFlags & RTF_EDC_MODE)) {  // We don't want to save away epcheckers in EDC mode
        if (myLine.tokens.size() != 1) {
          returnAEI.valid = 0;
          errMsgs.push_back("Wrong number of items on the line when loading epcheckers!");
          break;
        }

        returnAEI.aeiEpcheckers.push_back(myLine.tokens[0]);
      }
      continue; /* We don't want to push any sort of info back into the aeiLines */
    }

    /***********************************************************************/
    /* Now we'll look for things based upon the number of tokens returned  */
    /* This should allow for some better error checking that we had before */
    /***********************************************************************/
    else if (myLine.tokens.size() == 3) {
      /*****************************/
      /* Looking ring or scom line */
      /*****************************/
      if (myLine.tokens[0] == "ring" && myLine.tokens[2] == "{") {
        /******************/
        /* Error Checking */
        /******************/
        if (whatsGoingOn & (SPY_RING | SPY_SCOM)) { /* We can't have rings or scoms buried in each other */
          returnAEI.valid = 0;
          errMsgs.push_back("Ring line nested in another ring or scom section!");
          break;
        }

        /*************/
        /* Real Work */
        /*************/
        whatsGoingOn |= SPY_RING;
        returnAEI.states |= SPY_RING;
        braceOrder.push_back(SPY_RING);
        /* EDC mode means we don't want to save away the ring lines, just setup the couple things above for checking */
        if (runtimeFlags & RTF_EDC_MODE) continue; // Don't push a curAEILine on

        /* Save the ringname and bunch of other stuff away */
        curAEILine.state |= (SPY_RING | SPY_SECTION_START);
        curAEILine.latchName = myLine.tokens[1];
      }

      else if (myLine.tokens[0] == "scom" && myLine.tokens[2] == "{") {
        /******************/
        /* Error Checking */
        /******************/
        if (whatsGoingOn & (SPY_RING | SPY_SCOM)) { /* We can't have rings or scoms buried in each other */
          returnAEI.valid = 0;
          errMsgs.push_back("Scom line nested in another ring or scom section!");
          break;
        }

        /*************/
        /* Real Work */
        /*************/
        /* Save the scom addr and bunch of other stuff away */
        returnAEI.states |= SPY_SCOM;
        whatsGoingOn |= SPY_SCOM;
        braceOrder.push_back(SPY_SCOM);
        /* EDC mode means we don't want to save away the scom lines, just setup the couple things above for checking */
        if (runtimeFlags & RTF_EDC_MODE) continue; // Don't push a curAEILine on

        curAEILine.state |= (SPY_SCOM | SPY_SECTION_START);
        curAEILine.latchName = myLine.tokens[1];
      }

      /***********************************************************/
      /* General purpose error for no match for number of tokens */
      /***********************************************************/
      else {
        returnAEI.valid = 0;
        errMsgs.push_back("Found the wrong number of arguments when parsing this line:");
        errMsgs.push_back(myLine.realLine);
        break;
      }
    }

    else if (myLine.tokens.size() == 2) {
      /************************************/
      /* Looking clock (on/off/ind) lines */
      /************************************/
      sedcClockState clkState = SEDC_CLK_INVALID;
      if ((clkState = sedcStringToClkState(myLine.tokens[0])) != SEDC_CLK_INVALID && myLine.tokens[1] == "{") {
        /******************/
        /* Error Checking */
        /******************/
        if (whatsGoingOn & (SPY_RING | SPY_SCOM)) {
          returnAEI.valid = 0;
          errMsgs.push_back("Clock section found inside a ring/scom.  A clock section needs to be above a ring or scom!");
          break;
        }

        unsigned int clkStateBitMap = sedcClkStateToLatchState(clkState);

        /*************/
        /* Real Work */
        /*************/
        if (whatsGoingOn & SPY_CLOCK_ANY) {
          returnAEI.valid = 0;
          errMsgs.push_back("Attempting to start a" + myLine.tokens[0] + "section while nested in another clock state!");
          break;
        }

        returnAEI.states |= clkStateBitMap;
        whatsGoingOn |= clkStateBitMap;
        braceOrder.push_back(clkStateBitMap);
        curAEILine.state |= (clkStateBitMap | SPY_SECTION_START);

        /* In EDC mode we have to keep track of the holder ID */
        if (runtimeFlags & RTF_EDC_MODE) holderID++;
      }

      /************************************/
      /* Looking for start of group lines */
      /************************************/
      else if (myLine.tokens[0] == "group" && myLine.tokens[1] == "{") {
        /******************/
        /* Error Checking */
        /******************/
        if (whatsGoingOn & SPY_GROUP_BITS) {
          returnAEI.valid = 0;
          errMsgs.push_back("Attempting to start a group section while nested in another group section!");
          break;
        }

        if (whatsGoingOn & (SPY_RING | SPY_SCOM)) {
          returnAEI.valid = 0;
          errMsgs.push_back("Group section found inside a ring/scom.  A group needs to be at the top level!");
          break;
        }

        /*************/
        /* Real Work */
        /*************/
        returnAEI.states |= SPY_GROUP_BITS;
        whatsGoingOn |= SPY_GROUP_BITS;
        braceOrder.push_back(SPY_GROUP_BITS);
        curAEILine.state |= (SPY_GROUP_BITS | SPY_SECTION_START);
        numGroupBits = 0; /* Reset group bits count */

        /* In EDC mode we have to keep track of the holder ID */
        if (runtimeFlags & RTF_EDC_MODE) holderID++;
      }

      /***********************************/
      /* Looking for start of enum lines */
      /***********************************/
      else if (myLine.tokens[0] == "enum" && myLine.tokens[1] == "{") {
        /******************/
        /* Error Checking */
        /******************/
        if (whatsGoingOn & (SPY_RING | SPY_SCOM)) {
          returnAEI.valid = 0;
          errMsgs.push_back("Enum section nest in a ring/scom section!");
          break;
        }

        /*************/
        /* Real Work */
        /*************/
        returnAEI.states |= SPY_ENUM;
        whatsGoingOn |= SPY_ENUM;
        braceOrder.push_back(SPY_ENUM);
        curAEILine.state |= (SPY_ENUM | SPY_SECTION_START);
      }

      /*****************************************/
      /* Looking for start of epcheckers lines */
      /*****************************************/
      else if (myLine.tokens[0] == "epcheckers" && myLine.tokens[1] == "{") {
        /******************/
        /* Error Checking */
        /******************/
        if (whatsGoingOn & (SPY_RING | SPY_SCOM)) {
          returnAEI.valid = 0;
          errMsgs.push_back("ecc section nested in a ring/scom section!");
          break;
        }

        /*************/
        /* Real Work */
        /*************/
        returnAEI.states |= SPY_EPCHECKERS;
        whatsGoingOn |= SPY_EPCHECKERS;
        braceOrder.push_back(SPY_EPCHECKERS);
        curAEILine.state |= (SPY_EPCHECKERS | SPY_SECTION_START);
      }

      /***********************************************************/
      /* General purpose error for no match for number of tokens */
      /***********************************************************/
      else {
        returnAEI.valid = 0;
        errMsgs.push_back("Found the wrong number of arguments when parsing this line:");
        errMsgs.push_back(myLine.realLine);
        break;
      }
    }

    else if (myLine.tokens.size() == 1 || myLine.tokens.size() == 5) {

      /* Parse the line, assuming it's a latch line */
      sedcParseLatchLine(curAEILine, myLine, runtimeFlags);

      /* If it's not EDC mode, do some of this length adding up */
      if (!(runtimeFlags & RTF_EDC_MODE)) {
        if (whatsGoingOn & (SPY_RING | SPY_SCOM)) {
          /* Setup the state info */
          curAEILine.state |= whatsGoingOn;
          /* More length gathering special cases */
          if (curAEILine.state & SPY_GROUP_BITS)
            numGroupBits += curAEILine.length;
          else if (curAEILine.state & SPY_CLOCK_ON)
            numClockOnBits += curAEILine.length;
          else if (curAEILine.state & SPY_CLOCK_IND)
            numClockIndBits += curAEILine.length;
          else if (curAEILine.state & SPY_CLOCK_OFF)
            numClockOffBits += curAEILine.length;
          else /* The default case */
            returnAEI.length += curAEILine.length;
        }
      } else { // EDC mode
        /* Setup the state info */
        curAEILine.state |= whatsGoingOn;
        curAEILine.holderID = holderID;
        //curAEILine.state |= SPY_RING; // Turn on the ring since it won't be on if no ring line was given

        if (whatsGoingOn & SPY_SCOM)
           curAEILine.state |= SPY_SCOM;
        else // If a scom {} section wasn't given, it's assumed to be a ring.  No more latch nonsense
           curAEILine.state |= SPY_RING;

          /* Finally push curAEILine onto the latch list and do a continue.  This makes us miss the normal push_back below */
        returnAEI.aeiLatches.push_back(curAEILine);
        continue;
      }
    }

    /****************************/
    /* Big 'ol Bucket of errors */
    /****************************/
    else {
      returnAEI.valid = 0;
      errMsgs.push_back("Found the wrong number of arguments when parsing this line:");
      errMsgs.push_back(myLine.realLine);
      break;
    }

    /* Finally push curAEILine into the list */
    returnAEI.aeiLines.push_back(curAEILine);

    /* But if in EDC mode, we also have to push on a line for the new holderID line */
    /* But don't do it on a comment line */
    if ((runtimeFlags & RTF_EDC_MODE) && (prevHolderID != holderID)) {
      prevHolderID = holderID;
      holderAEILine.reset();
      holderAEILine.holderID = holderID;
      holderAEILine.state |= SPY_HOLD_LINE;
      holderAEILine.state |= whatsGoingOn; // Tack on all the other states, we'll use it in the builder
      returnAEI.aeiLines.push_back(holderAEILine);
    }
  }

  /************************************************/
  /* Done looping through, do some final checking */
  /************************************************/

  /* Group Bits stuff (counted in non-EDC mode only) */
  if (numGroupBits != 0)
    returnAEI.length = numGroupBits;

  /* Check all the clock states stuff (counted in non-EDC mode only) */
  if (!(runtimeFlags & RTF_EDC_MODE) && returnAEI.states & SPY_CLOCK_ANY) {
    if (numClockOnBits && numClockOffBits) {
      if (numClockOnBits != numClockOffBits) {
        returnAEI.valid = 0;
        tempstr = "Mismatch in number of bits between clock on and clock off section!\n";
        errMsgs.push_back(tempstr);
        //break;
      } else {
        returnAEI.length += numClockOnBits; /* Since they are the same length, use clock on */
        if (numClockIndBits) /* Add any independent bits we might have come across */
          returnAEI.length += numClockIndBits;
      }
    } else { /* We don't have a clock on/off combo so we don't have to worry about a double add */
      if (numClockOnBits)
        returnAEI.length += numClockOnBits;
      if (numClockIndBits)
        returnAEI.length += numClockIndBits;
      if (numClockOffBits)
        returnAEI.length += numClockOffBits;
    }
  }

  return returnAEI;
}

sedcAEIEnum sedcParseEnumLine(sedcFileLine &myLine, bool &valid, std::vector<std::string> &errMsgs, uint32_t &runtimeFlags) {
  sedcAEIEnum returnAEIEnum;
  std::string tempstr;
  unsigned int tempValue;
  unsigned int strPos;
  unsigned int mask[] = {
    0x80000000, 0x40000000, 0x20000000, 0x10000000, 0x08000000, 0x04000000, 0x02000000, 0x01000000,
    0x00800000, 0x00400000, 0x00200000, 0x00100000, 0x00080000, 0x00040000, 0x00020000, 0x00010000,
    0x00008000, 0x00004000, 0x00002000, 0x00001000, 0x00000800, 0x00000400, 0x00000200, 0x00000100,
    0x00000080, 0x00000040, 0x00000020, 0x00000010, 0x00000008, 0x00000004, 0x00000002, 0x00000001 };

  /*   MY_ENUM = 0x01234 */
  if (myLine.tokens[1] != "=" || myLine.tokens.size() != 3) {
    valid = 0;
    errMsgs.push_back("Invalid format in enum section!");
    return returnAEIEnum;
  }

  /* Error check the name and then set it */
  if (myLine.tokens[0].find_first_not_of(ENUM_NAME_ALLOWED) != NOT_FOUND) {
    valid = 0;
    errMsgs.push_back("An invalid character was found in the enum name: " + myLine.tokens[0]);
    return returnAEIEnum;
  }
  /* Force upper case for matching */
  transform(myLine.tokens[0].begin(), myLine.tokens[0].end(), myLine.tokens[0].begin(), toupper);
  if ( myLine.tokens[0].length() > 2 && myLine.tokens[0][0] == '0' && ( myLine.tokens[0][1] == 'B' || myLine.tokens[0][1] == 'X' ) ) {
    valid = 0;
    errMsgs.push_back("An enum name started with 0x or 0b, that is just too confusing: " + myLine.tokens[0]);
    return returnAEIEnum;    
  }
  returnAEIEnum.enumName = myLine.tokens[0];

  /* Look for hex, then for binary */
  if (myLine.tokens[2][1] == 'x' || myLine.tokens[2][1] == 'X') {
    myLine.tokens[2].erase(0,2);
    returnAEIEnum.enumLength = myLine.tokens[2].length();
    /* Now start converting my data over to 32 bit chunks to pass in */
    while (myLine.tokens[2].length() >= 8) {
      tempstr = myLine.tokens[2].substr(0,8);
      sscanf(tempstr.c_str(), "%X", &tempValue);
      returnAEIEnum.enumValue.push_back(tempValue);
      myLine.tokens[2].erase(0,8);
    }
    /* Now see if we got any left overs to clean up */
    if (myLine.tokens[2].length()) {
      sscanf(myLine.tokens[2].c_str(), "%X", &tempValue);
      /* Now left align it */
      tempValue = tempValue << ((8 - myLine.tokens[2].length()) * 4);
      returnAEIEnum.enumValue.push_back(tempValue);
    }
  } else if (myLine.tokens[2][1] == 'b') {
    tempValue = 0x0;
    myLine.tokens[2].erase(0,2);
    returnAEIEnum.enumLength = 0;
    /* Convert binary over to hex */
    for (strPos = 0; strPos < myLine.tokens[2].length(); strPos++) {
      /* Keep a count of the enumLength in hex */
      if ((strPos % 4) == 0)
        returnAEIEnum.enumLength++;
      /* Convert binary to hex */
      if (myLine.tokens[2][strPos] == '1') {
        tempValue |= mask[strPos&0x1F];
      } else if (myLine.tokens[2][strPos] == '0') {
        tempValue &= ~mask[strPos&0x1F];
      } else {
        valid = 0;
        errMsgs.push_back("Binary input data need to be either 0 or 1!");
        break;
      }
      /* At the end of the word, we need to store tempValue away, tempValue needs to be zero'd out */
      if (((strPos+1) % 32) == 0) {
        returnAEIEnum.enumValue.push_back(tempValue);
        tempValue = 0x0;
      }
    }
    /* Now any remainder needs to be stored away */
    if (strPos % 32)
      returnAEIEnum.enumValue.push_back(tempValue);
  } else {
    valid = 0;
    errMsgs.push_back("You must specify either 0x, 0X or 0b for an enum value!");
    return returnAEIEnum;
  }

  return returnAEIEnum;
}

sedcSynonymEntry sedcSynonymParser(std::ifstream &spyFile, std::vector<std::string> &errMsgs, uint32_t &runtimeFlags) {

  sedcSynonymEntry returnSynonym; returnSynonym.valid = 1;  /* We'll assume valid until disproven */
  sedcSynonymLine  curSynonymLine;
  int done = 0;
  std::string line, tempstr;
  sedcFileLine myLine;
  int realNameFound = 0;
  unsigned int whatsGoingOn = 0x0;
  int prevFilePos;  /* This is needed to rewind the file in a couple error scenarios */

  while (returnSynonym.valid && !done) {

    prevFilePos = spyFile.tellg(); /* Save this before we do anything */

    getline(spyFile,line,'\n'); /* Start each time with a new line read */
    curSynonymLine.reset(); /* Clear out the temp structure from the previous loop */
    sedcCreateSpyTokens(line,WHITESPACE,myLine);  // Cut each line into tokens seperated on white space

    /****************************************/
    /* Handle comment lines and blank lines */
    /****************************************/
    if (myLine.tokens.size() == 0) {
      if (myLine.comment.size() != 0 && (runtimeFlags & RTF_RETAIN_COMMENTS)) {
        curSynonymLine.state |= SYN_COMMENT;
        curSynonymLine.lineExtras = myLine.comment;
      } else {
        continue;
      }
    }

    /*********************************************/
    /* Look for synonym lines and act from there */
    /*********************************************/
    else if (myLine.tokens[0] == "synonym" && myLine.tokens[2] == "{") {
      /******************/
      /* Error Checking */
      /******************/
      /* If whatsGoingOn is set, that means we've already seen an synonym line and have a brace mismatch*/
      if (whatsGoingOn) {
        spyFile.seekg(prevFilePos);  /* Rewind the file so the error handling doesn't skip this spy */
        returnSynonym.valid = 0;
        errMsgs.push_back("Found the start of a new spy while not finished with the current one.  Check your braces!");
        break;
      }

      /*************/
      /* Real Work */
      /*************/
      if (myLine.tokens[0] == "synonym") {
        whatsGoingOn |= SYN_SECTION_START;
        curSynonymLine.state |= SYN_SECTION_START;
      } else {
        returnSynonym.valid = 0;
        errMsgs.push_back("(sedcSynonymParser) :: a type other than synonym found!");
        break;
      }

      /* Force upper case, save away the name, error check */
      transform(myLine.tokens[1].begin(), myLine.tokens[1].end(), myLine.tokens[1].begin(), toupper);
      returnSynonym.name = myLine.tokens[1];
      if (returnSynonym.name.find_first_not_of(SPY_NAME_ALLOWED) != NOT_FOUND) {
        returnSynonym.valid = 0;
        errMsgs.push_back("An invalid character was found in the spy name!");
        break;
      }
    }

    /*************************/
    /* Look for the ending } */
    /*************************/
    else if (myLine.tokens[0] == "}") { /* If we don't find anything other than a }, this is good */
      if (!realNameFound) {
        returnSynonym.valid = 0;
        errMsgs.push_back("Found ending brace before the real name was found!");
        break;
      }

      /* We must have found a realName.  Let's finish this up and mark it done */
      curSynonymLine.state |= SYN_SECTION_END;
      done = 1;
    }

    /*************************/
    /* Gotta be the realname */
    /*************************/
    else if (myLine.tokens.size() == 1) {
      /******************/
      /* Error Checking */
      /******************/
      if (realNameFound) { /* We already have our name, this is no good */
        returnSynonym.valid = 0;
        errMsgs.push_back("Found multiple names in this synonym!");
        break;
      }

      /*************/
      /* Real Work */
      /*************/
      returnSynonym.realName = myLine.tokens[0];
      curSynonymLine.lineExtras = myLine.comment;

      /* We must have found a realName.  Let's finish this up and mark it done */
      curSynonymLine.state |= SYN_NAME;
      curSynonymLine.lineName = myLine.tokens[0];
      realNameFound = 1;
    }

    /****************************/
    /* Big 'ol Bucket of errors */
    /****************************/
    else {
      returnSynonym.valid = 0;
      errMsgs.push_back("Found the wrong number of arguments when parsing this line:");
      errMsgs.push_back(myLine.realLine);
      break;
    }

    /* Finally push curSynonymLine into the list */
    returnSynonym.synonymLines.push_back(curSynonymLine);
  }

  return returnSynonym;
}

sedcEplatchesEntry sedcEplatchesParser(std::ifstream &spyFile, std::vector<std::string> &errMsgs, uint32_t &runtimeFlags) {

  sedcEplatchesEntry returnEplatches;
  returnEplatches.valid = 1; /* We'll start out assuming things are good and dis-prove if necessary */
  sedcEplatchesLine  curEplatchesLine;
  sedcAEIEntry returnAEI;
  std::string line;
  sedcFileLine myLine;
  std::string tempstr;
  int done = 0;
  unsigned int whatsGoingOn = 0x0;
  long prevFilePos;

  /* We only need to loop until we find the eplatches and function name, then make a couple calls to the parse to read the in and out */
  while (returnEplatches.valid && !done) {

    /* Save away the file pos each time because a couple conditions require a rewind */
    prevFilePos = spyFile.tellg();
    getline(spyFile,line,'\n'); /* Start each time with a new line read */
    sedcCreateSpyTokens(line,WHITESPACE,myLine);  // Cut each line into tokens seperated on white space
    curEplatchesLine.reset();

    /****************************************/
    /* Handle comment lines and blank lines */
    /****************************************/
    if (myLine.tokens.size() == 0) {
      if (myLine.comment.size() != 0 && (runtimeFlags & RTF_RETAIN_COMMENTS)) {
        curEplatchesLine.state |= EPL_COMMENT;
        curEplatchesLine.comment = myLine.comment;
      } else {
        continue;
      }
    }

    /**********************************************/
    /* Look for eplatches line and act from there */
    /**********************************************/
    else if (myLine.tokens[0] == "eplatches" && myLine.tokens[2] == "{") {
      /******************/
      /* Error Checking */
      /******************/
      /* If whatsGoingOn is set, that means we've already seen an eplatches line and have a brace mismatch*/
      if (whatsGoingOn) {
        spyFile.seekg(prevFilePos);  /* Rewind the file so the error handling doesn't skip this spy */
        returnEplatches.valid = 0;
        errMsgs.push_back("Found the start of a new spy while not finished with the current one.  Check your braces!");
        break;
      }

      /*************/
      /* Real Work */
      /*************/
      if (myLine.tokens[0] == "eplatches") {
        whatsGoingOn |= EPL_SECTION_START;
        curEplatchesLine.state |= EPL_SECTION_START;
      } else {
        returnEplatches.valid = 0;
        errMsgs.push_back("(sedcEplatchesParser) :: a type other than eplatches found!");
        break;
      }

      /* Force upper case, save away the name, error check */
      transform(myLine.tokens[1].begin(), myLine.tokens[1].end(), myLine.tokens[1].begin(), toupper);
      returnEplatches.name = myLine.tokens[1];
      if (returnEplatches.name.find_first_not_of(SPY_NAME_ALLOWED) != NOT_FOUND) {
        returnEplatches.valid = 0;
        errMsgs.push_back("An invalid character was found in the spy name!");
        break;
      }
    }

    /*********************************/
    /* Looking for the function line */
    /*********************************/
    else if (myLine.tokens[0] == "function" && myLine.tokens[1] == "=") {
      transform(myLine.tokens[2].begin(), myLine.tokens[2].end(), myLine.tokens[2].begin(), toupper);  /* Force function to be upper case */
      returnEplatches.function = myLine.tokens[2];
      curEplatchesLine.state |= EPL_FUNCTION;
      whatsGoingOn |= EPL_FUNCTION;
    }

    /**********************************/
    /* Looking for the in latch lines */
    /**********************************/
    else if (myLine.tokens[0] == "in" && myLine.tokens[1] == "{") {
      spyFile.seekg(prevFilePos); /* Rewind to undo the read of the in line */
      returnAEI = sedcAEIParser(spyFile, errMsgs, runtimeFlags);
      returnAEI.name = returnEplatches.name; // Make the name of the spy the same as the name of eplatches.  Needed in the verifier later
      if (!returnAEI.valid) {
        returnEplatches.valid = 0;
        errMsgs.push_back("Error parsing the in latches of the eccLatch group!");
        break;
      } else {
        returnEplatches.inSpy = returnAEI;
      }
      curEplatchesLine.state |= EPL_INLATCHES;
      whatsGoingOn |= EPL_INLATCHES;
    }

    /***********************************/
    /* Looking for the out latch lines */
    /***********************************/
    else if (myLine.tokens[0] == "out" && myLine.tokens[1] == "{") {
      spyFile.seekg(prevFilePos); /* Rewind to undo the read of the in line */
      returnAEI = sedcAEIParser(spyFile, errMsgs, runtimeFlags);
      returnAEI.name = returnEplatches.name; // Make the name of the spy the same as the name of eplatches.  Needed in the verifier later
      if (!returnAEI.valid) {
        returnEplatches.valid = 0;
        errMsgs.push_back("Error parsing the out latches of the eplatches group!");
        break;
      } else {
        returnEplatches.outSpy = returnAEI;
      }
      curEplatchesLine.state |= EPL_OUTLATCHES;
      whatsGoingOn |= EPL_OUTLATCHES;
    }

    /*************************/
    /* Look for the ending } */
    /*************************/
    else if (myLine.tokens[0] == "}") { /* If we don't find anything other than a }, this is good */
      if (myLine.tokens.size() > 1) {
        returnEplatches.valid = 0;
        errMsgs.push_back("Too many closing braces found in eplatches!");
        break;
      }

      if (!(whatsGoingOn == EPL_ALL_FOUND)) {
        returnEplatches.valid = 0;
        errMsgs.push_back("Found ending brace before function, in and out latches were found!");
        break;
      }

      curEplatchesLine.state |= EPL_SECTION_END;
      done = 1;
    }

    /****************************/
    /* Big 'ol Bucket of errors */
    /****************************/
    else {
      returnEplatches.valid = 0;
      errMsgs.push_back("Found the wrong number of arguments when parsing this line:");
      errMsgs.push_back(myLine.realLine);
      break;
    }

    /* Finally push curEplatchesLine into the list */
    returnEplatches.eplatchesLines.push_back(curEplatchesLine);
  }

  return returnEplatches;
}

sedcEccfuncEntry sedcEccfuncParser(std::ifstream &spyFile, std::vector<std::string> &errMsgs, uint32_t &runtimeFlags) {

  sedcEccfuncEntry returnEccfunc;
  returnEccfunc.valid = 1; /* We'll start out assuming things are good and dis-prove if necessary */
  sedcEccfuncLine  curEccfuncLine;
  std::string line;
  sedcFileLine myLine, tempLine;
  std::string tempstr;
  int done = 0;
  unsigned int whatsGoingOn = 0x0;
  long prevFilePos;
  unsigned int strPos;
  unsigned int tempValue;
  unsigned int mask[] = {
    0x80000000, 0x40000000, 0x20000000, 0x10000000, 0x08000000, 0x04000000, 0x02000000, 0x01000000,
    0x00800000, 0x00400000, 0x00200000, 0x00100000, 0x00080000, 0x00040000, 0x00020000, 0x00010000,
    0x00008000, 0x00004000, 0x00002000, 0x00001000, 0x00000800, 0x00000400, 0x00000200, 0x00000100,
    0x00000080, 0x00000040, 0x00000020, 0x00000010, 0x00000008, 0x00000004, 0x00000002, 0x00000001 };

  /* We only need to loop until we find the eplatches and function name, then make a couple calls to the parse to read the in and out */
  while (returnEccfunc.valid && !done) {

    /* Save away the file pos each time because a couple conditions require a rewind */
    prevFilePos = spyFile.tellg();
    getline(spyFile,line,'\n'); /* Start each time with a new line read */
    sedcCreateSpyTokens(line,WHITESPACE,myLine);  // Cut each line into tokens seperated on white space
    curEccfuncLine.reset();

    /****************************************/
    /* Handle comment lines and blank lines */
    /****************************************/
    if (myLine.tokens.size() == 0) {
      if (myLine.comment.size() != 0 && (runtimeFlags & RTF_RETAIN_COMMENTS)) {
        curEccfuncLine.state |= ECF_COMMENT;
        curEccfuncLine.comment = myLine.comment;
      } else {
        continue;
      }
    }

    /**********************************************/
    /* Look for eplatches line and act from there */
    /**********************************************/
    else if (myLine.tokens[0] == "eccfunc" && myLine.tokens[2] == "{") {
      /******************/
      /* Error Checking */
      /******************/
      /* If whatsGoingOn is set, that means we've already seen an eplatches line and have a brace mismatch*/
      if (whatsGoingOn) {
        spyFile.seekg(prevFilePos);  /* Rewind the file so the error handling doesn't skip this spy */
        returnEccfunc.valid = 0;
        errMsgs.push_back("Found the start of a new spy while not finished with the current one.  Check your braces!");
        break;
      }

      /*************/
      /* Real Work */
      /*************/
      if (myLine.tokens[0] == "eccfunc") {
        curEccfuncLine.state |= ECF_SECTION_START;
        whatsGoingOn |= ECF_SECTION_START;
      } else {
        returnEccfunc.valid = 0;
        errMsgs.push_back("(sedcEccFuncParser) :: a type other than Eccfunc found!");
        break;
      }

      /* Force upper case, save away the name, error check */
      transform(myLine.tokens[1].begin(), myLine.tokens[1].end(), myLine.tokens[1].begin(), toupper);
      returnEccfunc.name = myLine.tokens[1];
      if (returnEccfunc.name.find_first_not_of(SPY_NAME_ALLOWED) != NOT_FOUND) {
        returnEccfunc.valid = 0;
        errMsgs.push_back("An invalid character was found in the spy name!");
        break;
      }
    }

    /*******************************/
    /* Looking for the inbits line */
    /*******************************/
    else if (myLine.tokens[0] == "inbits" && myLine.tokens[1] == "=") {
      sscanf(myLine.tokens[2].c_str(),"%d",&returnEccfunc.inBits); // Convert the number of inbits to a number
      curEccfuncLine.state |= ECF_INBITS;
      whatsGoingOn |= ECF_INBITS;
    }

    /********************************/
    /* Looking for the outbits line */
    /********************************/
    else if (myLine.tokens[0] == "outbits" && myLine.tokens[1] == "=") {
      sscanf(myLine.tokens[2].c_str(),"%d",&returnEccfunc.outBits); // Convert the number of outbits to a number
      curEccfuncLine.state |= ECF_OUTBITS;
      whatsGoingOn |= ECF_OUTBITS;
    }

    /*******************************/
    /* Looking for the table lines */
    /*******************************/
    /* Since an hamming table entry has to have (), we'll use that as an indicator. */
    else if (myLine.tokens[0].find_first_of("()") != NOT_FOUND) {
      /* First, bust the line up via the parens */
      sedcCreateSpyTokens(myLine.tokens[0],"()",tempLine);  // Cut each line into tokens seperated on white space
      /******************/
      /* Error Checking */
      /******************/
      if (tempLine.tokens.size() != 2) {
        returnEccfunc.valid = 0;
        errMsgs.push_back("Found the wrong number of arguments when parsing this line:");
        errMsgs.push_back(myLine.realLine);
        break;
      }

      /*************/
      /* Real Work */
      /*************/
      curEccfuncLine.parityType = tempLine.tokens[0];
      /* Look for hex, then for binary */
      if (tempLine.tokens[1][1] == 'x' || tempLine.tokens[1][1] == 'X') {
        tempLine.tokens[1].erase(0,2);
        curEccfuncLine.tableLength = tempLine.tokens[1].length();
        /* Now start converting my data over to 32 bit chunks to pass in */
        while (tempLine.tokens[1].length() >= 8) {
          tempstr = tempLine.tokens[1].substr(0,8);
          sscanf(tempstr.c_str(), "%X", &tempValue);
          curEccfuncLine.tableValue.push_back(tempValue);
          tempLine.tokens[1].erase(0,8);
        }
        /* Now see if we got any left overs to clean up */
        if (tempLine.tokens[1].length()) {
          sscanf(tempLine.tokens[1].c_str(), "%X", &tempValue);
          /* Now left align it */
          tempValue = tempValue << ((8 - tempLine.tokens[1].length()) * 4);
          curEccfuncLine.tableValue.push_back(tempValue);
        }
      } else if (tempLine.tokens[1][1] == 'b') {
        tempLine.tokens[1].erase(0,2);
        /* Convert binary over to hex */
        for (strPos = 0; strPos < tempLine.tokens[1].length(); strPos++) {
          /* Keep a count of the maskLength in nibbles */
          if ((strPos % 4) == 0)
            curEccfuncLine.tableLength++;
          /* Convert binary to hex */
          if (tempLine.tokens[1][strPos] == '1') {
            tempValue |= mask[strPos&0x1F];
          } else if (tempLine.tokens[1][strPos] == '0') {
            tempValue &= ~mask[strPos&0x1F];
          } else {
            returnEccfunc.valid = 0;
            errMsgs.push_back("Binary input data need to be either 0 or 1!");
            break;
          }
          /* At the end of the word, we need to store tempValue away, tempValue needs to be zero'd out */
          if (((strPos+1) % 32) == 0) {
            curEccfuncLine.tableValue.push_back(tempValue);
            tempValue = 0x0;
          }
        }
        /* Now any remainder needs to be stored away */
        if (strPos % 32)
          curEccfuncLine.tableValue.push_back(tempValue);
      } else {
        returnEccfunc.valid = 0;
        errMsgs.push_back("You must specify either 0x, 0X or 0b for an enum value!");
        break;
      }

      curEccfuncLine.state |= ECF_TABLE;
      whatsGoingOn |= ECF_TABLE;
    }

    /*************************/
    /* Look for the ending } */
    /*************************/
    else if (myLine.tokens[0] == "}") { /* If we don't find anything other than a }, this is good */
      /******************/
      /* Error Checking */
      /******************/
      if (myLine.tokens.size() > 1) {
        returnEccfunc.valid = 0;
        errMsgs.push_back("Too many closing braces found in eplatches!");
        break;
      }

      /*************/
      /* Real Work */
      /*************/
      if (!(whatsGoingOn == ECF_ALL_FOUND)) {
        returnEccfunc.valid = 0;
        errMsgs.push_back("Found ending brace before inbits, outbits or table were found!");
        break;
      }

      curEccfuncLine.state |= ECF_SECTION_END;
      done = 1;
    }

    /****************************/
    /* Big 'ol Bucket of errors */
    /****************************/
    else {
      returnEccfunc.valid = 0;
      errMsgs.push_back("Found the wrong number of arguments when parsing this line:");
      errMsgs.push_back(myLine.realLine);
      break;
    }

    /* Finally push curEplatchesLine into the list */
    returnEccfunc.eccfuncLines.push_back(curEccfuncLine);
  }

  return returnEccfunc;
}


void sedcCreateSpyTokens(std::string line, const char* seperators, sedcFileLine &myLine) {

  unsigned int curStart = 0, curEnd = 0, subEnd = 0;
  std::string token;

  myLine.realLine = line;
  myLine.tokens.clear();
  myLine.comment = "";

  curStart = line.find_first_not_of(seperators, curEnd);
  while (curStart != NOT_FOUND) {
    if ( line[curStart] == '#' && ( curStart == 0 || line[curStart-1] == ' ' ) ) {  // Comment found - save away rest of line and bail
      myLine.comment = line.substr(curStart,line.length());
      break;
    }
    curEnd = line.find_first_of(seperators,curStart);
    if (curEnd != NOT_FOUND)
      token = line.substr(curStart, curEnd-curStart);
    else // No More white space, but still a token
      token = line.substr(curStart,line.length());
    // Now search the token and look for braces and handle appropriately
    subEnd = token.find_first_of("{}",0);
    while (subEnd != NOT_FOUND) {
      // If it's just a brace by itself, bail out - it'll get pushed onto the list at the end
      if (token == "{" || token == "}") break;
      // More than just a brace found, push the part without the brace onto the list, then erase
      myLine.tokens.push_back(token.substr(0, subEnd));
      token.erase(0, subEnd);
      // Now go through and push the brace onto the list, one at a time
      while (token[0] == '{' || token[0] == '}') {
        myLine.tokens.push_back(token.substr(0, 1));
        token.erase(0, 1);
      }
      subEnd = token.find_first_of("{}",0); // Now look again
    }
    /* Since there is some erasing going on above, need to make the empty check */
    if (token != "") myLine.tokens.push_back(token);
    curStart = line.find_first_not_of(seperators, curEnd);
  }

}
