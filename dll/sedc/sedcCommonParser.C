/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File sedcCommonParser.C                                   
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
#include <list>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <stdio.h>
#include <sedcCommonParser.H>
#include <sedcDefines.H>
#include <ecmdSharedUtils.H>

void sedcParseLatchLine(sedcLatchLine &returnLatchLine, sedcFileLine &myLine, unsigned int runtimeFlags) {

  std::string tempstr;
  std::string tempstr2;
  uint32_t length;
  size_t linePos;
  uint32_t latchAccessor = 0; // Default is the first entry, will get moved if full latch line

  /* Make sure the latchLine we are going to return is clean to start */
  returnLatchLine.reset();

  /* See if it's a full size line */
  if (myLine.tokens.size() == 5) {
    sscanf(myLine.tokens[0].c_str(), "%d", &returnLatchLine.length);
    sscanf(myLine.tokens[1].c_str(), "%d", &returnLatchLine.offsetFSI);
    sscanf(myLine.tokens[2].c_str(), "%d", &returnLatchLine.offsetJTAG);
    /* Special characters field */
    if (myLine.tokens[3] == "!") returnLatchLine.state |= SPY_INVERT;
    if (runtimeFlags & RTF_BUILD_INVERSION_ALT && myLine.tokens[3] == "1") returnLatchLine.state |= SPY_INVERT;
    latchAccessor = 4;
  }

  /* Save away just the latch name */
  length = myLine.tokens[latchAccessor].length();
  if (myLine.tokens[latchAccessor][length-1] == ')') { /* This means we have a () condition on the latch.  Process and delete */
    linePos = myLine.tokens[latchAccessor].find_last_of('(');
    returnLatchLine.latchExtras = myLine.tokens[latchAccessor].substr(linePos,length-2); /* Get the ()'s */
    myLine.tokens[latchAccessor].erase(linePos,length); /* Now Erase the ()'s of the latch name */
    /* In a RTF_PRESERVE_EXTRA_LATCH_INFO we want to figure out the lhsNum, rhsNum and direction to save away */
    if (runtimeFlags & RTF_PRESERVE_EXTRA_LATCH_INFO) {
      tempstr = returnLatchLine.latchExtras; /* We want to preserve the state of the parens string, assign it over to a temp to be worked on */
      tempstr.erase(0,1); /* Erase the ()'s off the tempstr */
      length = tempstr.length();
      tempstr.erase((length-1),1);
      linePos = tempstr.find(':');
      if (linePos == NOT_FOUND) { /* Must only be only 1 value */
        tempstr2 = tempstr.substr(0,length);
        sscanf(tempstr2.c_str(), "%d", &returnLatchLine.lhsNum);
        returnLatchLine.rhsNum = returnLatchLine.lhsNum;
        returnLatchLine.direction = 0;  /* Direction is always zero because it's a single bit */
        returnLatchLine.length = 1;
      } else { /* Is multi-value, split it up */
        tempstr2 = tempstr.substr(0,linePos);
        sscanf(tempstr2.c_str(), "%d", &returnLatchLine.lhsNum);
        tempstr2 = tempstr.substr((linePos+1),(length-linePos));
        sscanf(tempstr2.c_str(), "%d", &returnLatchLine.rhsNum);
        if (returnLatchLine.rhsNum > returnLatchLine.lhsNum) {
          returnLatchLine.direction = 1;
          returnLatchLine.length = (returnLatchLine.rhsNum - returnLatchLine.lhsNum) + 1; 
        } else {
          returnLatchLine.direction = -1;
          returnLatchLine.length = (returnLatchLine.lhsNum - returnLatchLine.rhsNum) + 1;
        } 
      }
    }
  } else { /* ()'s not found, this has to be a single bit case */
    returnLatchLine.direction = 0;
    returnLatchLine.lhsNum = 0;
    returnLatchLine.rhsNum = 0;
    returnLatchLine.length = 1;
  }

  /* We need to capture if it's deadbits here because the latch name may not be held onto for later use */
  if (myLine.tokens[latchAccessor] == "DEADBITS" || myLine.tokens[latchAccessor] == "deadbits" || myLine.tokens[latchAccessor] == "deadbit" || myLine.tokens[latchAccessor] == "DEADBIT") {
    returnLatchLine.state |= SPY_DEADBITS;
    /* Reset the offset to 0 to keep it consistent */
    returnLatchLine.offsetJTAG = 0;
    returnLatchLine.offsetFSI  = 0;
  }

  if (runtimeFlags & RTF_RETAIN_LATCH_NAME) {
    returnLatchLine.latchName = myLine.tokens[latchAccessor];
  }

  if (runtimeFlags & RTF_RETAIN_COMMENTS) {
    returnLatchLine.comment = myLine.comment;
  }

  /* Check for the special operators */
  if (myLine.tokens.size() == 1) {
    if (returnLatchLine.latchName[0] == '!') {
      returnLatchLine.state |= SPY_INVERT;
      returnLatchLine.latchName.erase(0,1);
    }
  }

  /* Finally, generate the hashKey for the latchName */
  returnLatchLine.hashKey = ecmdHashString32(returnLatchLine.latchName.c_str(), 0);
}

sedcClockState sedcStringToClkState(const std::string& clkStateString)
{
   sedcClockState rc = SEDC_CLK_UNKNOWN;

   // copy input and convert to lower case

   if (clkStateString.length() > 0)
   {
      std::string temp = clkStateString;
      transform(temp.begin(),temp.end(), temp.begin(), (int(*)(int)) tolower);

      if ((temp == "clock-stop") || (temp == "clock-off"))
         rc = SEDC_CLK_STOP;
      else if ((temp == "clock-running") || (temp == "clock-on") || (temp == "i390-running"))
         rc = SEDC_CLK_RUNNING;
      else if (temp == "clock-independent")
         rc = SEDC_CLK_INDEPENDENT;
      else
         rc = SEDC_CLK_INVALID;
   } /* endif */

   return rc;
}

std::string sedcClkStateToSpyString(sedcClockState clkState)
{
   std::string rc = "Invalid";

   // copy input and convert to lower case

   switch (clkState)
   {
      case SEDC_CLK_UNKNOWN:
         rc = "Unknown";
         break;
      case SEDC_CLK_STOP:
         rc = "clock-off";
         break;
      case SEDC_CLK_RUNNING:
         rc = "clock-on";
         break;
      case SEDC_CLK_INDEPENDENT:
         rc = "clock-ind";
         break;
      case SEDC_CLK_INVALID:
         rc = "Invalid";
         break;
   } /* endswitch */

   return rc;
}

std::string sedcClkStateToScandefString(sedcClockState clkState)
{
   std::string rc = "Invalid";

   // copy input and convert to lower case

   switch (clkState)
   {
      case SEDC_CLK_UNKNOWN:
         rc = "Unknown";
         break;
      case SEDC_CLK_STOP:
         rc = "Clock-Stop";
         break;
      case SEDC_CLK_RUNNING:
         rc = "Clock-Running";
         break;
      case SEDC_CLK_INDEPENDENT:
         rc = "Clock-Independent";
         break;
      case SEDC_CLK_INVALID:
         rc = "Invalid";
         break;
   } /* endswitch */

   return rc;
}

unsigned int sedcClkStateToLatchState(sedcClockState clkState)
{
   unsigned int rc = 0;

   switch (clkState)
   {
      case SEDC_CLK_STOP:
         rc = SPY_CLOCK_OFF;
         break;
      case SEDC_CLK_RUNNING:
         rc = SPY_CLOCK_ON;
         break;
      case SEDC_CLK_INDEPENDENT:
         rc = SPY_CLOCK_IND;
         break;

      case SEDC_CLK_UNKNOWN:
      case SEDC_CLK_INVALID:
         break;
   } /* endswitch */

   return rc;
}

sedcClockState sedcLatchStateToClkState(unsigned int latchState)
{
   sedcClockState rc = SEDC_CLK_INVALID;

   switch (latchState & SPY_CLOCK_ANY)
   {
      case 0 :
         rc = SEDC_CLK_UNKNOWN;
         break;
      case SPY_CLOCK_OFF:
         rc = SEDC_CLK_STOP;
         break;
      case SPY_CLOCK_ON:
         rc = SEDC_CLK_RUNNING;
         break;
      case SPY_CLOCK_IND:
         rc = SEDC_CLK_INDEPENDENT;
         break;
      default:
         rc = SEDC_CLK_INVALID;
         break;
   } /* endswitch */

   return rc;
}
