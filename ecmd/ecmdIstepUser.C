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
#include <string>
#include <algorithm>
#include <ctype.h>

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


  /* Now pull off the common args */
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

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

      size_t curOffset = 0;
      size_t nextOffset = 0;
      size_t tmpOffset = 0;

      while (curOffset < userArgs.length()) {

        nextOffset = userArgs.find(',',curOffset);
        if (nextOffset == std::string::npos) {
          nextOffset = userArgs.length();
        }

        curSubstr = userArgs.substr(curOffset, nextOffset - curOffset);
        
        if ((tmpOffset = curSubstr.find("..",0)) < curSubstr.length()) {

          if (!isdigit(curSubstr[0]) || !isdigit(curSubstr[tmpOffset+2])) {
            ecmdOutputError("istep - Non-Numeric character found in istep range (x..y)\n");
            return ECMD_INVALID_ARGS;
          }
            
          int lowerBound = atoi(curSubstr.substr(0,tmpOffset).c_str());
          int upperBound = atoi(curSubstr.substr(tmpOffset+2, curSubstr.length()).c_str());
          
          if (stepPtr != NULL)
            steps.setBit(lowerBound, upperBound - lowerBound + 1);
          else
            steps.clearBit(lowerBound, upperBound - lowerBound + 1);

        }
        else {
          if (!isdigit(curSubstr[0])) {
            ecmdOutputError("istep - Non-Numeric character found in istep steps (x,y)\n");
            return ECMD_INVALID_ARGS;
          }

          if (stepPtr != NULL)
            steps.setBit(atoi(curSubstr.c_str()));
          else
            steps.clearBit(atoi(curSubstr.c_str()));

        }
        
        curOffset = nextOffset+1;
        
      }
    }



    rc = iStepsByNumber(steps);

  } else {

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
   target.chipTypeState = ECMD_TARGET_FIELD_VALID;
   target.posState      = ECMD_TARGET_FIELD_WILDCARD;
  }
  else {
   target.chipTypeState = target.posState = ECMD_TARGET_FIELD_UNUSED;
  }

  target.cageState = target.nodeState = target.slotState = ECMD_TARGET_FIELD_WILDCARD;
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
   target.chipTypeState = ECMD_TARGET_FIELD_VALID;
   target.posState      = ECMD_TARGET_FIELD_WILDCARD;
  }
  else {
   target.chipTypeState = target.posState = ECMD_TARGET_FIELD_UNUSED;
  }

  target.cageState = target.nodeState = target.slotState = ECMD_TARGET_FIELD_WILDCARD;
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

uint32_t ecmdSetClockSpeedUser(int argc, char* argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  
  ecmdChipTarget target;                        ///< Current target being operated on
  bool validPosFound = false;                   ///< Did the looper find anything?
  ecmdLooperData looperdata;                    ///< Store internal Looper data
  std::string printed;                          ///< Output data
  std::string clocktype;                        ///< the clock type to change the speed on
  std::string clockspeed;                       ///< Speed - frequency or cycle time
  ecmdClockSpeedType_t speedType;               ///< Clock speed type - frequency or cycle time
  ecmdClockType_t clockType;                    ///< the clock type to change the speed on
  ecmdClockSetMode_t clockSetMode = ECMD_CLOCK_ONE_STEP; ///< do adjustment in one operation or to steer to new value
  ecmdClockRange_t clockRange = ECMD_CLOCK_RANGE_DEFAULT; ///< range to adjust clock steering procedure
  
  /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
  if (ecmdParseOption(&argc, &argv, "-steer")) {
    clockSetMode = ECMD_CLOCK_STEER;
  }
  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 2) {  //clocktype + speed
    ecmdOutputError("setclockspeed - Too few arguments specified; you need at least a clocktype and speed.\n");
    ecmdOutputError("setclockspeed - Type 'setclockspeed -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.cageState =  ECMD_TARGET_FIELD_WILDCARD;
  target.nodeState = target.slotState = target.posState = target.chipTypeState = target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //Get the clock type
  clocktype = argv[0];
  transform(clocktype.begin(), clocktype.end(), clocktype.begin(), (int(*)(int)) tolower);
  if (clocktype == "pu_refclock") {
    clockType = ECMD_PROC_REFCLOCK;
  } else if (clocktype == "memctrl_refclock") {
    clockType = ECMD_MEMCTRL_REFCLOCK;
  } else {
    ecmdOutputError("setclockspeed - Unrecognizable clock Type. Should be \"pu_refclock\" or \"memctrl_refclock\"\n");
    return ECMD_INVALID_ARGS;
  }
  
  //get clockspeed
  size_t strpos;
  clockspeed = argv[1];
  transform(clockspeed.begin(), clockspeed.end(), clockspeed.begin(), (int(*)(int)) tolower);
  
  if ((strpos = clockspeed.find("mhz")) != std::string::npos)  {
    speedType = ECMD_CLOCK_FREQUENCY_SPEC;
  } else if ((strpos = clockspeed.find("us")) != std::string::npos) {
    speedType = ECMD_CLOCK_CYCLETIME_SPEC;
  } else {
    ecmdOutputError("setclockspeed - keyword \"mhz\" or \"us\" not found in clock speed field\n");
    return ECMD_INVALID_ARGS;
  }
  
  clockspeed.erase(strpos, clockspeed.length()-strpos);
  
  if (!ecmdIsAllDecimal(clockspeed.c_str())) {
    ecmdOutputError("setclockspeed - Non-Decimal characters detected in speed field\n");
    return ECMD_INVALID_ARGS;
  } 
  
  uint32_t speed = atoi(clockspeed.c_str());

  if (argc > 2) {
    ecmdOutputError("setclockspeed - Too many arguments specified; you probably added an option that wasn't recognized.\n");
    ecmdOutputError("setclockspeed - Type 'setclockspeed -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  
  
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;


  while ( ecmdConfigLooperNext(target, looperdata) ) {
    rc = ecmdSetClockSpeed(target, clockType, speed, speedType, clockSetMode, clockRange);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "setclockspeed - Error occured performing setclockspeed on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }

    
    printed = ecmdWriteTarget(target);
    printed += "\n";
    ecmdOutput( printed.c_str() );
  }


  if (!validPosFound) {
    //this is an error common across all UI functions
    ecmdOutputError("setclockspeed - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  

  return rc;
}

uint32_t ecmdGetClockSpeedUser(int argc, char* argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  
  ecmdChipTarget target;                        ///< Current target being operated on
  bool validPosFound = false;                   ///< Did the looper find anything?
  ecmdLooperData looperdata;                    ///< Store internal Looper data
  std::string printed;                          ///< Output data
  ecmdClockSpeedType_t speedType;               ///< Clock speed type - frequency or cycle time
  ecmdClockType_t clockType;                    ///< the clock type to change the speed on
  std::string outputformat = "d";               ///< Output Format to display
  uint32_t speed;                               ///< The speed return value
  ecmdDataBuffer buffer(32);

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 2) {  //clocktype + speed
    ecmdOutputError("getclockspeed - Too few arguments specified; you need at least a clocktype and speed.\n");
    ecmdOutputError("getclockspeed - Type 'getclockspeed -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  if (argc > 2) {
    ecmdOutputError("getclockspeed - Too many arguments specified; you need only a clocktype and speed.\n");
    ecmdOutputError("getclockspeed - Type 'getclockspeed -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = ECMD_CHIPT_PROCESSOR;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  //Get the clock type
  std::string clocktype = argv[0];
  transform(clocktype.begin(), clocktype.end(), clocktype.begin(), (int(*)(int)) tolower);
  if (clocktype == "pu_refclock") {
    clockType = ECMD_PROC_REFCLOCK;
  } else if (clocktype == "memctrl_refclock") {
    clockType = ECMD_MEMCTRL_REFCLOCK;
  } else {
    ecmdOutputError("getclockspeed - Unrecognizable clock Type. Should be \"pu_refclock\" or \"memctrl_refclock\"\n");
    return ECMD_INVALID_ARGS;
  }
  
  //get clockspeed
  std::string clockspeed = argv[1];
  transform(clockspeed.begin(), clockspeed.end(), clockspeed.begin(), (int(*)(int)) tolower);
  
  if (clockspeed == "mhz")  {
    speedType = ECMD_CLOCK_FREQUENCY_SPEC;
  } else if (clockspeed == "us") {
    speedType = ECMD_CLOCK_CYCLETIME_SPEC;
  } else {
    ecmdOutputError("getclockspeed - keyword \"mhz\" or \"us\" not found in clock speed field\n");
    return ECMD_INVALID_ARGS;
  }
  
  
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;


  while ( ecmdConfigLooperNext(target, looperdata) ) {
    rc = ecmdGetClockSpeed(target, clockType, speedType, speed);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "getclockspeed - Error occured performing getclockspeed on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }

    buffer.setWord(0, speed);
    printed = ecmdWriteTarget(target);
    printed += ecmdWriteDataFormatted(buffer, outputformat);
    ecmdOutput( printed.c_str() );
  }


  if (!validPosFound) {
    //this is an error common across all UI functions
    ecmdOutputError("getclockspeed - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  

  return rc;
}

uint32_t ecmdSystemPowerUser(int argc, char * argv[]) {

  uint32_t rc = ECMD_SUCCESS;

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc < 1) {
    ecmdOutputError("systempower - At least one argument ('on', 'off') is required for systempower.\n");
    return ECMD_INVALID_ARGS;
  }
  else if (argc > 1) {
    ecmdOutputError("systempower - Too many arguments to systempower, you probably added a non-supported option.\n");
    return ECMD_INVALID_ARGS;
  }
  
  if (!strcmp(argv[0], "on")) {
    ecmdOutput("Powering on the system ...\n");
    rc = ecmdSystemPowerOn();
  } else if (!strcmp(argv[0], "off")) {
    ecmdOutput("Powering off the system ...\n");
    rc = ecmdSystemPowerOff();
  } else {
    ecmdOutputError("systempower - Invalid argument passed to systempower. Accepted arguments: ('on', 'off').\n");
    return ECMD_INVALID_ARGS;
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
