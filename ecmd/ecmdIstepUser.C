// Copyright ***********************************************************
//                                                                      
// File ecmdIstepUser.C                                   
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

// Module Description **************************************************
//
// Description: 
//
// End Module Description **********************************************
/* $Header$ */
//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------

#include <stdio.h>
#include <list>

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

uint32_t ecmdIstepUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  ecmdDataBuffer steps(200);                    ///< Buffer to hold numbered steps (max of 200 steps)
  std::string userArgs;                         ///< Buffer for manipulating user args
  std::vector<std::string> tokens;              ///< Tokens from user args
  std::list< std::string > iSteps;              ///< Steps to pass into Multiple function
  bool foundRange;                              ///< Did the user specify a range of step names

  char * stepPtr = ecmdParseOptionWithArgs(&argc, &argv, "-s");
  char * skipStepPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if ((stepPtr != NULL) || (skipStepPtr != NULL) || (argc == 0)) {

    if ((stepPtr == NULL) && (skipStepPtr == NULL)) {
      /* They didn't specify a step, so we will run all */
      steps.flushTo1();

    } else {

      if ((stepPtr != NULL) && (skipStepPtr != NULL)) {
        ecmdOutputError("istep - You cannot specify -s and -i on the same command\n");
        return ECMD_INVALID_ARGS;
      } else if (argc != 0) {
        ecmdOutputError("istep - Too many arguments passed to istep\n");
        return ECMD_INVALID_ARGS;
      }

      std::string curSubstr;
      if (stepPtr != NULL)
        userArgs = stepPtr;
      else {
        userArgs = skipStepPtr;
        steps.flushTo1();
      }

      uint32_t curOffset = 0;
      uint32_t nextOffset = 0;
      uint32_t tmpOffset = 0;

      while (curOffset < userArgs.length()) {

        nextOffset = userArgs.find(',',curOffset);
        if (nextOffset == std::string::npos) {
          nextOffset = userArgs.length();
        }

        curSubstr = userArgs.substr(curOffset, nextOffset - curOffset);
        
        if ((tmpOffset = curSubstr.find("..",0)) < curSubstr.length()) {

          int lowerBound = atoi(curSubstr.substr(0,tmpOffset).c_str());
          int upperBound = atoi(curSubstr.substr(tmpOffset+2, curSubstr.length()).c_str());
          
          if (stepPtr != NULL)
            steps.setBit(lowerBound, upperBound - lowerBound + 1);
          else
            steps.clearBit(lowerBound, upperBound - lowerBound + 1);

        }
        else {
          if (stepPtr != NULL)
            steps.setBit(atoi(curSubstr.c_str()));
          else
            steps.clearBit(atoi(curSubstr.c_str()));

        }
        
        curOffset = nextOffset+1;
        
      }
    }


    rc = ecmdCommandArgs(&argc, &argv);
    if (rc) return rc;

    rc = iStepsByNumber(steps);

  } else {

    rc = ecmdCommandArgs(&argc, &argv);
    if (rc) return rc;

    /* Let's just push all the args into one string */
    userArgs = "";
    for (int arg = 0; arg < argc; arg ++) {
      userArgs += argv[arg];
    }

    /* Let's see if the user specified a range */
    foundRange = false;
    if (userArgs.find("..",0) != std::string::npos) foundRange = true;

    /* Now let's break up into our tokens */
    ecmdParseTokens(userArgs, ", .", tokens);

    if (foundRange) {
      if (tokens.size() != 2) {
        ecmdOutputError("istep - Too many arguments found when specifying a step name range '..'\n");
        rc = ECMD_INVALID_ARGS;
      } else {
        rc = iStepsByNameRange(tokens[0], tokens[1]);
      }

    } else {
      /* Let's move the steps from the vector to a list (kindof annoying) */
      for (uint32_t x = 0; x < tokens.size(); x ++) {
        iSteps.push_back(tokens[x]);
      }

      rc = iStepsByNameMultiple(iSteps);
    }   
  }
  if (rc) {
    ecmdOutputError("istep - Problems running iSteps\n");
  }

  return rc;

}


uint32_t ecmdStartClocksUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  char clockDomain[100]; 			///< Store the clock domain user specified
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdChipTarget target;                ///< Current target being operated on
  bool validPosFound = false;           ///< Did the looper find something to run on?

   /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
 
  //Check force option
  bool force = ecmdParseOption(&argc, &argv, "-force");

  strcpy(clockDomain, "ALL");
  
  //Convenience Clock Domains
  char * cDomain = ecmdParseOptionWithArgs(&argc, &argv, "-domain");
  if (cDomain != NULL) {
    strcpy(clockDomain, cDomain);
  }
  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  //Setup the target that will be used to query the system config
  if(argc >= 1) {
   target.chipType = argv[0];
   target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  }
  else {
   target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
  }

  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = target.coreState = ECMD_TARGET_FIELD_UNUSED;

  std::string printed;

  //Should be the case only if -domain wasn't specified
  if(argc == 2) {
    if(strcmp(clockDomain,"ALL")!=0) {
       printed = "startclocks - Convenience Domain '"+ (std::string)clockDomain + "' and Physical Domain '" + (std::string)argv[1] +"' both specified. Choosing Physical Domain.\n";
       ecmdOutputWarning( printed.c_str() );
    }
    strcpy(clockDomain, argv[1]);
  }
  
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP_DEFALL, looperdata);
  if (rc) return rc;

  ecmdOutput("Starting Clocks on Targets ...\n");

  while ( ecmdConfigLooperNext(target, looperdata) ) {

    rc = startClocks(target, clockDomain, force);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc == ECMD_INVALID_CLOCK_DOMAIN) {
        printed = "startclocks - An invalid clock domain " + (std::string)clockDomain+ " was specified for target ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else if (rc == ECMD_CLOCKS_ALREADY_ON) {
        printed = "startclocks - Clocks in the specified domain are already on for target ";
        printed += ecmdWriteTarget(target) + "\n";
        if(force) {
         ecmdOutput( printed.c_str() );
	 continue;
	} 
	else {
	 ecmdOutputError( printed.c_str() );
         return rc;
	}
    }
    else if (rc == ECMD_CLOCKS_IN_INVALID_STATE) {
        printed = "startclocks - The clock in the specified domain are in an unknown state (not all on/off) for target ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else if (rc == ECMD_RING_CACHE_ENABLED) {
        printed = "startclocks - Ring Cache enabled - must be disabled to use this function. Target ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }


    printed = ecmdWriteTarget(target) + "\n";
    ecmdOutput( printed.c_str() ); 
  }

  if (!validPosFound) {
    ecmdOutputError("Unable to find a valid target to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdStopClocksUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  char clockDomain[100]; 			///< Store the clock domain user specified
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdChipTarget target;                ///< Current target being operated on
  bool validPosFound = false;           ///< Did the looper find something to run on?

   /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
 
  //Check force option
  bool force = ecmdParseOption(&argc, &argv, "-force");

  strcpy(clockDomain, "ALL");
  
  //Convenience Clock Domains
  char * cDomain = ecmdParseOptionWithArgs(&argc, &argv, "-domain");
  if (cDomain != NULL) {
    strcpy(clockDomain, cDomain);
  }
  
  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  //Setup the target that will be used to query the system config
  if(argc >= 1) {
   target.chipType = argv[0];
   target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
  }
  else {
   target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
  }

  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_QUERY_WILDCARD;
  target.threadState = target.coreState = ECMD_TARGET_FIELD_UNUSED;

  std::string printed;

  if(argc == 2) {
     if(strcmp(clockDomain,"ALL")!=0) {
       printed = "stopclocks - Convenience Domain '"+ (std::string)clockDomain + "' and Physical Domain '" + (std::string)argv[1] +"' both specified. Choosing Physical Domain.\n";
       ecmdOutputWarning( printed.c_str() );
     }
     strcpy(clockDomain, argv[1]);
  }
  
  
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP_DEFALL, looperdata);
  if (rc) return rc;
  ecmdOutput("Stopping Clocks on Targets ...\n");

  while ( ecmdConfigLooperNext(target, looperdata) ) {

    rc = stopClocks(target, clockDomain, force);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc == ECMD_INVALID_CLOCK_DOMAIN) {
        printed = "stopclocks - An invalid clock domain " + (std::string)clockDomain+ " was specified for target ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else if (rc == ECMD_CLOCKS_ALREADY_OFF) {
        printed = "stopclocks - Clocks in the specified domain are already off for target ";
        printed += ecmdWriteTarget(target) + "\n";
	if(force) {
         ecmdOutput( printed.c_str() );
	 continue;
	} 
	else {
	 ecmdOutputError( printed.c_str() );
         return rc;
	}
    }
    else if (rc == ECMD_CLOCKS_IN_INVALID_STATE) {
        printed = "stopclocks - The clock in the specified domain are in an unknown state (not all on/off) for target ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else if (rc == ECMD_RING_CACHE_ENABLED) {
        printed = "stopclocks - Ring Cache enabled - must be disabled to use this function. Target ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }


    printed = ecmdWriteTarget(target) + "\n";
    ecmdOutput( printed.c_str() ); 
  }

  if (!validPosFound) {
    ecmdOutputError("Unable to find a valid target to execute command on\n");
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
