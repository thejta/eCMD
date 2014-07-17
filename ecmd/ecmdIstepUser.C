/* $Header$ */
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
//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
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
            
          uint32_t lowerBound = (uint32_t)atoi(curSubstr.substr(0,tmpOffset).c_str());
          uint32_t upperBound = (uint32_t)atoi(curSubstr.substr(tmpOffset+2, curSubstr.length()).c_str());
          
          if (lowerBound > upperBound) {
            ecmdOutputError("istep - Lower Bound istep # > Upper Bound istep #\n");
            return ECMD_INVALID_ARGS;
          }

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
            steps.setBit((uint32_t)atoi(curSubstr.c_str()));
          else
            steps.clearBit((uint32_t)atoi(curSubstr.c_str()));

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

uint32_t ecmdInitChipFromFileUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS, coeRc = ECMD_SUCCESS;
  ecmdChipTarget target;        ///< Current target
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;           ///< Print Buffer
  ecmdLooperData looperData;     ///< Store internal Looper data

  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  char* file = ecmdParseOptionWithArgs(&argc, &argv, "-file");
  char* name = ecmdParseOptionWithArgs(&argc, &argv, "-name");
  char* mode = ecmdParseOptionWithArgs(&argc, &argv, "-mode");

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  if (argc != 1) {
    ecmdOutputError("initchipfromfile - Incorrect number of args.\n");
    ecmdOutputError("initchipfromfile - Type 'runinitfromfile -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;

  while (ecmdLooperNext(target, looperData) && (!coeRc || coeMode)) {
    rc = initChipFromFile(target, file, name, mode);
    if (rc) {
      printed = "initchipfromfile - Error occured performing initchipfromfile on ";
      printed += ecmdWriteTarget(target);
      printed += "\n";
      ecmdOutputError(printed.c_str());
      coeRc = rc;
      continue;
    } else {
      validPosFound = true;
    }
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  //this is an error common across all UI functions
  if (!validPosFound) {
    ecmdOutputError("initchipfromfile - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}


#ifndef ECMD_REMOVE_CLOCK_FUNCTIONS
uint32_t ecmdStartClocksUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  uint32_t coeRc = ECMD_SUCCESS;            //@02
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

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode
  
  //Setup the target that will be used to query the system config
  target.chipUnitTypeState = target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;
  if (argc >= 1) {
    std::string chipType, chipUnitType;
    ecmdParseChipField(argv[0], chipType, chipUnitType);
    if (chipUnitType != "") {
     target.chipUnitType = chipUnitType;
     target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
     target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
    }
    target.chipType = chipType;
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    target.posState      = ECMD_TARGET_FIELD_WILDCARD;
    
  }
  else {
   target.chipTypeState = target.posState = ECMD_TARGET_FIELD_UNUSED;
  }

  target.cageState = target.nodeState = target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

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

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP_DEFALL, looperdata);
  if (rc) return rc;

  ecmdOutput("Starting Clocks on Targets ...\n");

  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {     //@02

    rc = startClocks(target, clockDomain, force);
    if (rc == ECMD_INVALID_CLOCK_DOMAIN) {
      printed = "startclocks - An invalid clock domain " + (std::string)clockDomain+ " was specified for target ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;                                   //@02                                      
      continue;                                     //@02
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
        coeRc = rc;                                   //@02                                         
        continue;                                     //@02
      }
    }
    else if (rc == ECMD_CLOCKS_IN_INVALID_STATE) {
      printed = "startclocks - The clock in the specified domain are in an unknown state (not all on/off) for target ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;                                   //@02                                         
      continue;                                     //@02
    }
    else if (rc == ECMD_RING_CACHE_ENABLED) {
      printed = "startclocks - Ring Cache enabled - must be disabled to use this function. Target ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;                                   //@02                                         
      continue;                                     //@02
    }
    else {
      validPosFound = true;     
    }


    printed = ecmdWriteTarget(target) + "\n";
    ecmdOutput( printed.c_str() ); 
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  if (!validPosFound) {
    ecmdOutputError("Unable to find a valid target to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdStopClocksUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  uint32_t coeRc = ECMD_SUCCESS;            //@02
  char clockDomain[100];                ///< Store the clock domain user specified
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdChipTarget target;                ///< Current target being operated on
  bool validPosFound = false;           ///< Did the looper find something to run on?
  uint32_t mode = 0x0;                  ///< Extra options to pass to stopClocks

   /************************************************************************/
  /* Parse Local FLAGS here!                                              */
  /************************************************************************/
 
  //Check force option
  bool force = ecmdParseOption(&argc, &argv, "-force");

  //Check skip_iovalid option
  bool skipIovalid = ecmdParseOption(&argc, &argv, "-skip_iovalid");
  if (skipIovalid) {
    mode |= ECMD_STOP_CLOCK_MODE_SKIP_IOVALID;
  }

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

  //@02
  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  //Setup the target that will be used to query the system config
  target.chipUnitTypeState = target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;
  if (argc >= 1) {
    std::string chipType, chipUnitType;
    ecmdParseChipField(argv[0], chipType, chipUnitType);
    if (chipUnitType != "") {
      target.chipUnitType = chipUnitType;
      target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
      target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
    }
    target.chipType = chipType;
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    target.posState      = ECMD_TARGET_FIELD_WILDCARD;
  }
  else {
   target.chipTypeState = target.posState = ECMD_TARGET_FIELD_UNUSED;
  }

  target.cageState = target.nodeState = target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

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

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP_DEFALL, looperdata);
  if (rc) return rc;
  ecmdOutput("Stopping Clocks on Targets ...\n");

  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {     //@02

    rc = stopClocks(target, clockDomain, force, mode);
    if (rc == ECMD_INVALID_CLOCK_DOMAIN) {
      printed = "stopclocks - An invalid clock domain " + (std::string)clockDomain+ " was specified for target ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;                                   //@02
      continue;                                     //@02
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
        coeRc = rc;                                   //@02
        continue;                                     //@02
      }
    }
    else if (rc == ECMD_CLOCKS_IN_INVALID_STATE) {
      printed = "stopclocks - The clock in the specified domain are in an unknown state (not all on/off) for target ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;                                   //@02
      continue;                                     //@02
    }
    else if (rc == ECMD_RING_CACHE_ENABLED) {
      printed = "stopclocks - Ring Cache enabled - must be disabled to use this function. Target ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;                                   //@02
      continue;                                     //@02
    }
    else {
      validPosFound = true;     
    }

    printed = ecmdWriteTarget(target) + "\n";
    ecmdOutput( printed.c_str() ); 
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  if (!validPosFound) {
    ecmdOutputError("Unable to find a valid target to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}  
#endif // ECMD_REMOVE_CLOCK_FUNCTIONS

#ifndef ECMD_REMOVE_REFCLOCK_FUNCTIONS
uint32_t ecmdSetClockSpeedUser(int argc, char* argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  uint32_t coeRc = ECMD_SUCCESS;
  ecmdChipTarget target;                        ///< Current target being operated on
  bool validPosFound = false;                   ///< Did the looper find anything?
  ecmdLooperData looperdata;                    ///< Store internal Looper data
  std::string printed;                          ///< Output data
  std::string chipType;                         ///< The chiptype the user specified (optional)
  std::string chipUnitType;                     ///< Purely for error checking
  std::string clocktype;                        ///< the clock type to change the speed on
  std::string clockspeed;                       ///< Speed - frequency or cycle time
  ecmdClockSpeedType_t speedType = ECMD_CLOCK_FREQUENCY_SPEC; ///< Clock speed type - frequency or cycle time
  ecmdClockType_t clockType = ECMD_CLOCKTYPE_UNKNOWN;  ///< the clock type to change the speed on
  ecmdClockSetMode_t clockSetMode = ECMD_CLOCK_ONE_STEP; ///< do adjustment in one operation or to steer to new value
  ecmdClockRange_t clockRange = ECMD_CLOCK_RANGE_DEFAULT; ///< range to adjust clock steering procedure
  uint32_t iv_mult=0;                              ///< Multiplier value, if present 
  uint32_t iv_div=0;                           ///< Divider value, if present
  uint32_t iv_fminmhz=0;                        ///< Frequency Minimum value, if present 
  uint32_t iv_fmaxmhz=0;                        ///< Frequency Maximum value, if present
  int32_t endOffet = 0;                         ///< The location where the required args in the arg list end
  ecmdClockFreqMode_t freqmode = ECMD_CLOCK_SINGLE_FREQ_MODE; ///<Frequency mode computed based on the paremeters

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

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode
  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 2) {  //clocktype + speed
    ecmdOutputError("setclockspeed - Too few arguments specified; you need at least a clocktype and speed.\n");
    ecmdOutputError("setclockspeed - Type 'setclockspeed -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  /* We need to figure out if the user gave us a proc chip or not */
  std::string temp = argv[0];
  transform(temp.begin(), temp.end(), temp.begin(), (int(*)(int)) tolower);
  if (
	  (temp.find("clock") == std::string::npos) &&
	  (temp.find("freq") == std::string::npos) 
     )
     {
    /* No 'clock' or 'freq' in the very first arg, assume it is a chip */
    ecmdParseChipField(argv[0], chipType, chipUnitType);
    //check if clocktype and speed were provided before reading them
    if (argc < 3) {  //chip + clocktype + speed
      ecmdOutputError("setclockspeed - Too few arguments specified; you need at least a clocktype and speed.\n");
      ecmdOutputError("setclockspeed - Type 'setclockspeed -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }
    clocktype = argv[1];
    clockspeed = argv[2];
    endOffet = 2;
  } else {
    /* We had a clock in the very first arg, assume no chip was given */
    clocktype = argv[0];
    clockspeed = argv[1];
    endOffet = 1;
  }

  //Setup the target that will be used to query the system config
  if (chipType != "") {
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
    target.chipType = chipType;
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;

    if (chipUnitType != "") {
      target.chipUnitType = chipUnitType;
      target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
      target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
      target.threadState = ECMD_TARGET_FIELD_UNUSED;
    } else {
      target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;
    }

  } else {
    target.cageState =  ECMD_TARGET_FIELD_WILDCARD;
    target.nodeState = target.slotState = target.posState = target.chipTypeState = target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;
  }

  //Check the clock type
  transform(clocktype.begin(), clocktype.end(), clocktype.begin(), (int(*)(int)) tolower);
  if (clocktype == "pu_refclock") {
    clockType = ECMD_PROC_REFCLOCK;
  } else if (clocktype == "memctrl_refclock") {
    clockType = ECMD_MEMCTRL_REFCLOCK;
  } else if (clocktype == "pu_coreclock") {
    clockType = ECMD_PROC_CORE_CLOCK;
  } else if (clocktype == "pu_nestclock") {
    clockType = ECMD_PROC_NEST_CLOCK;
  } else if (clocktype == "memctrl_clock") {
    clockType = ECMD_MEMCTRL_CLOCK;
  } else if (clocktype == "io_refclock") {
    clockType = ECMD_IO_REFCLOCK;
  } else if (clocktype == "io_clock") {
    clockType = ECMD_IO_CLOCK;
  } else if (clocktype == "gx_refclock") {
    clockType = ECMD_GX_REFCLOCK;
  } else if (clocktype == "dram_freq") {
    clockType = ECMD_DRAM_FREQ;
  } else {
    ecmdOutputError("setclockspeed - Unrecognized clocktype! 'setclockspeed -h' for recognized types.\n");
    return ECMD_INVALID_ARGS;
  }
  
  //get clockspeed
  size_t strpos;
  transform(clockspeed.begin(), clockspeed.end(), clockspeed.begin(), (int(*)(int)) tolower);
  
  if ((strpos = clockspeed.find("mhz")) != std::string::npos)  {
    speedType = ECMD_CLOCK_FREQUENCY_MHZ_SPEC;
    clockspeed.erase(strpos, clockspeed.length()-strpos);
  } else if ((strpos = clockspeed.find("khz")) != std::string::npos)  {
    speedType = ECMD_CLOCK_FREQUENCY_KHZ_SPEC;
    clockspeed.erase(strpos, clockspeed.length()-strpos);
  } else if ((strpos = clockspeed.find("npu")) != std::string::npos) {
    speedType = ECMD_CLOCK_NOMINAL_PERCENT_UP;
    clockspeed.erase(strpos, clockspeed.length()-strpos);
  } else if ((strpos = clockspeed.find("npd")) != std::string::npos) {
    speedType = ECMD_CLOCK_NOMINAL_PERCENT_DOWN;
    clockspeed.erase(strpos, clockspeed.length()-strpos);
  } else if ((strpos = clockspeed.find("cpu")) != std::string::npos) {
    speedType = ECMD_CLOCK_CURRENT_PERCENT_UP;
    clockspeed.erase(strpos, clockspeed.length()-strpos);
  } else if ((strpos = clockspeed.find("cpd")) != std::string::npos) {
    speedType = ECMD_CLOCK_CURRENT_PERCENT_DOWN;
    clockspeed.erase(strpos, clockspeed.length()-strpos);
  } else if ((strpos = clockspeed.find("pspu")) != std::string::npos) {
    speedType = ECMD_CLOCK_POWERSAVE_PERCENT_UP;
    clockspeed.erase(strpos, clockspeed.length()-strpos);
  } else if ((strpos = clockspeed.find("pspd")) != std::string::npos) {
    speedType = ECMD_CLOCK_POWERSAVE_PERCENT_DOWN;
    clockspeed.erase(strpos, clockspeed.length()-strpos);
  } else if ((strpos = clockspeed.find("stpu")) != std::string::npos) {
    speedType = ECMD_CLOCK_SUPER_TURBO_PERCENT_UP;
    clockspeed.erase(strpos, clockspeed.length()-strpos);
  } else if ((strpos = clockspeed.find("stpd")) != std::string::npos) {
    speedType = ECMD_CLOCK_SUPER_TURBO_PERCENT_DOWN;
    clockspeed.erase(strpos, clockspeed.length()-strpos);
  } else if ((strpos = clockspeed.find("tpu")) != std::string::npos) {
    speedType = ECMD_CLOCK_TURBO_PERCENT_UP;
    clockspeed.erase(strpos, clockspeed.length()-strpos);
  } else if ((strpos = clockspeed.find("tpd")) != std::string::npos) {
    speedType = ECMD_CLOCK_TURBO_PERCENT_DOWN;
    clockspeed.erase(strpos, clockspeed.length()-strpos);
  } // this needs to be after pspu and pspd so that those are found
    else if ((strpos = clockspeed.find("ps")) != std::string::npos) {
    speedType = ECMD_CLOCK_CYCLETIME_PS_SPEC;
    clockspeed.erase(strpos, clockspeed.length()-strpos);
  } else if ((strpos = clockspeed.find("mult")) != std::string::npos) {

    // get mult and divider value from cmdline
    clockspeed.erase(strpos, clockspeed.length()-strpos);

    if (!ecmdIsAllDecimal(clockspeed.c_str())) {
      ecmdOutputError("setclockspeed - Non-Decimal characters detected in speed field with 'mult' parm\n");
      return ECMD_INVALID_ARGS;
    }
    iv_mult = (uint32_t)atoi(clockspeed.c_str());
    if (iv_mult==0) {
      ecmdOutputError("setclockspeed - 'mult' value cannot be 0\n");
      return ECMD_INVALID_ARGS;
    }
    
    // divider is the next parm
    clockspeed = argv[endOffet + 1];
    transform(clockspeed.begin(), clockspeed.end(), clockspeed.begin(), (int(*)(int)) tolower);

    if ((strpos = clockspeed.find("div")) != std::string::npos) {
      clockspeed.erase(strpos, clockspeed.length()-strpos);

      if (!ecmdIsAllDecimal(clockspeed.c_str())) {
        ecmdOutputError("setclockspeed - Non-Decimal characters detected in speed field with 'div' parm\n");
        return ECMD_INVALID_ARGS;
      }
      iv_div = (uint32_t)atoi(clockspeed.c_str());
      if (iv_div==0) {
        ecmdOutputError("setclockspeed - 'div' value cannot be 0\n");
        return ECMD_INVALID_ARGS;
      }
      endOffet = endOffet + 1;//add for div also
      clockspeed = "0"; //clockspeed of zero in case of mult div

    } else {
      // this is supposed to be 'div'
      ecmdOutputError("setclockspeed - 'div' parm needs to come after 'mult' parm\n");
      ecmdOutputError("setclockspeed - Type 'setclockspeed -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }
  }else if ((strpos = clockspeed.find("fmin")) != std::string::npos) {
    freqmode = ECMD_CLOCK_MINMAX_FREQ_MODE;
    // get fmin and fmax value from cmdline
    clockspeed.erase(strpos, clockspeed.length()-strpos);

    if (!ecmdIsAllDecimal(clockspeed.c_str())) {
      ecmdOutputError("setclockspeed - Non-Decimal characters detected in speed field with 'fmin' parm\n");
      return ECMD_INVALID_ARGS;
    }
    iv_fminmhz = (uint32_t)atoi(clockspeed.c_str());
    if (iv_fminmhz==0) {
      ecmdOutputError("setclockspeed - 'fmin' value cannot be 0\n");
      return ECMD_INVALID_ARGS;
    }

    // fmax is the next parameter
    clockspeed = argv[endOffet + 1];
    transform(clockspeed.begin(), clockspeed.end(), clockspeed.begin(), (int(*)(int)) tolower);

    if ((strpos = clockspeed.find("fmax")) != std::string::npos) {
      clockspeed.erase(strpos, clockspeed.length()-strpos);

      if (!ecmdIsAllDecimal(clockspeed.c_str())) {
        ecmdOutputError("setclockspeed - Non-Decimal characters detected in speed field with 'fmax' parm\n");
        return ECMD_INVALID_ARGS;
      }
      iv_fmaxmhz = (uint32_t)atoi(clockspeed.c_str());
      if (iv_fmaxmhz==0) {
        ecmdOutputError("setclockspeed - 'fmax' value cannot be 0\n");
        return ECMD_INVALID_ARGS;
      }
      if (iv_fmaxmhz < iv_fminmhz) {
        ecmdOutputError("setclockspeed - 'fmax' value cannot less than 'fmin' value\n");
        return ECMD_INVALID_ARGS;
      }
      endOffet = endOffet + 1;//add for fmax also
      clockspeed = "0"; //clockspeed of zero in case of fmin and fmax
    } else {
      // this is supposed to be 'fmax'
      ecmdOutputError("setclockspeed - 'fmax' parm needs to come after 'fmin' parm\n");
      ecmdOutputError("setclockspeed - Type 'setclockspeed -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }

    //throw error of min-max mode is used for clocktypes other than core clock
    if (clocktype != "pu_coreclock") {
      ecmdOutputError("setclockspeed - Min-Max mode is supported only for core clocks\n");
    }
  } else {
    ecmdOutputError("setclockspeed - Speed Keyword not found in clock speed field\n");
    ecmdOutputError("setclockspeed - Type 'setclockspeed -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  
  if (!ecmdIsAllDecimal(clockspeed.c_str())) {
    ecmdOutputError("setclockspeed - Non-Decimal characters detected in speed field\n");
    return ECMD_INVALID_ARGS;
  } 
  
  uint32_t speed = (uint32_t)atoi(clockspeed.c_str());

  if (((argc > (endOffet+1)) && (iv_mult==0)) || ((argc > (endOffet+1)) && (iv_mult!=0)))  {
    ecmdOutputError("setclockspeed - Too many arguments specified; you probably added an option that wasn't recognized.\n");
    ecmdOutputError("setclockspeed - Type 'setclockspeed -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;


  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {
    //if not mult/div type
    if (iv_mult==0) {
      if (freqmode == ECMD_CLOCK_SINGLE_FREQ_MODE) {
        //In single frequency mode don not set min, max values
        rc = ecmdSetClockSpeed(target, clockType, speed, speedType, clockSetMode, clockRange, freqmode, 0, 0);
    } else { 
        //if the freq is min/max we dont set the speed
        rc = ecmdSetClockSpeed(target, clockType, 0, speedType, clockSetMode, clockRange, freqmode, iv_fminmhz, iv_fmaxmhz);
      }
    } else { 
      rc = ecmdSetClockMultDiv(target, clockType, iv_mult, iv_div);
    }
    if (rc) {
      printed = "setclockspeed - Error occured performing setclockspeed on ";
      printed += ecmdWriteTarget(target);
      printed += "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }
    else {
      validPosFound = true;     
    }

    printed = ecmdWriteTarget(target);
    printed += "\n";
    ecmdOutput( printed.c_str() );
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  if (!validPosFound) {
    //this is an error common across all UI functions
    ecmdOutputError("setclockspeed - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}

uint32_t ecmdGetClockSpeedUser(int argc, char* argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  uint32_t coeRc = ECMD_SUCCESS;            //@02  
  ecmdChipTarget target;                        ///< Current target being operated on
  bool validPosFound = false;                   ///< Did the looper find anything?
  ecmdLooperData looperdata;                    ///< Store internal Looper data
  std::string printed;                          ///< Output data
  ecmdClockSpeedType_t speedType;               ///< Clock speed type - frequency or cycle time
  ecmdClockType_t clockType;                    ///< the clock type to change the speed on
  std::string outputformat = "d";               ///< Output Format to display
  uint32_t speed;                               ///< The speed return value
  ecmdDataBuffer buffer(32);
  std::string chipType;                         ///< The chiptype the user specified (optional)
  std::string chipUnitType;                     ///< Purely for error checking
  std::string clocktype;                        ///< the clock type to change the speed on
  std::string clockspeed;                       ///< Speed - frequency or cycle time
  //Commenting endOffet out to avoid compiler warnings.  Nothing was being done with the values.
  //  uint32_t endOffet = 0;                         ///< The location where the required args in the arg list end
  ecmdClockFreqMode_t freqmode = ECMD_CLOCK_SINGLE_FREQ_MODE;///< Frequency mode set on the specified target
  uint32_t freqMin= 0;                           ///< Minimum frequency value set for the target 
  uint32_t freqMax= 0;                           ///< Maximum frequency value set for the target

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;
  
  //@02
  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 2) {  //clocktype + speed
    ecmdOutputError("getclockspeed - Too few arguments specified; you need at least a clocktype and speed.\n");
    ecmdOutputError("getclockspeed - Type 'getclockspeed -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  /* We need to figure out if the user gave us a proc chip or not */
  std::string temp = argv[0];
  transform(temp.begin(), temp.end(), temp.begin(), (int(*)(int)) tolower);
  if (temp.find("clock") == std::string::npos) {
    /* No clock in the very first arg, assume it is a chip */
    if (argc < 3) {  //target + clocktype + speedtype
      ecmdOutputError("getclockspeed - Too few arguments specified; you need at least a clocktype and speed.\n");
      ecmdOutputError("getclockspeed - Type 'getclockspeed -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }
    ecmdParseChipField(argv[0], chipType, chipUnitType);
    clocktype = argv[1];
    clockspeed = argv[2];
    //endOffet = 2;
  } else {
    /* We had a clock in the very first arg, assume no chip was given */
    clocktype = argv[0];
    clockspeed = argv[1];
    //endOffet = 1;
  }

  //Setup the target that will be used to query the system config
  if (chipType != "") {
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
    target.chipType = chipType;
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;

    if (chipUnitType != "") {
      target.chipUnitType = chipUnitType;
      target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
      target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
      target.threadState = ECMD_TARGET_FIELD_UNUSED;
    } else {
      target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;
    }

  } else {
    // No chipType.chipUnitType give, so use the processor defaults for the target
    //Setup the target that will be used to query the system config 
    target.chipType = ECMD_CHIPT_PROCESSOR;
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
    target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;
  }


  //Convert the clock type to the enum
  transform(clocktype.begin(), clocktype.end(), clocktype.begin(), (int(*)(int)) tolower);
  if (clocktype == "pu_refclock") {
    clockType = ECMD_PROC_REFCLOCK;
  } else if (clocktype == "memctrl_refclock") {
    clockType = ECMD_MEMCTRL_REFCLOCK;
  } else if (clocktype == "pu_coreclock") {
    clockType = ECMD_PROC_CORE_CLOCK;
  } else if (clocktype == "pu_nestclock") {
    clockType = ECMD_PROC_NEST_CLOCK;
  } else if (clocktype == "memctrl_clock") {
    clockType = ECMD_MEMCTRL_CLOCK;
  } else if (clocktype == "io_refclock") {
    clockType = ECMD_IO_REFCLOCK;
  } else if (clocktype == "io_clock") {
    clockType = ECMD_IO_CLOCK;
  } else if (clocktype == "gx_refclock") {
    clockType = ECMD_GX_REFCLOCK;
  } else {
    ecmdOutputError("getclockspeed - Unrecognized clocktype! 'getclockspeed -h' for recognized types.\n");
    return ECMD_INVALID_ARGS;
  }
  
  //Convert the clockspeed to the enum
  transform(clockspeed.begin(), clockspeed.end(), clockspeed.begin(), (int(*)(int)) tolower);
  
  if (clockspeed == "mhz")  {
    speedType = ECMD_CLOCK_FREQUENCY_MHZ_SPEC;
  } else if (clockspeed == "khz")  {
    speedType = ECMD_CLOCK_FREQUENCY_KHZ_SPEC;
  } else if (clockspeed == "ps") {
    speedType = ECMD_CLOCK_CYCLETIME_PS_SPEC;
  } else {
    ecmdOutputError("getclockspeed - keyword \"mhz\", \"khz\", \"ps\" or not found in clock speed field\n");
    return ECMD_INVALID_ARGS;
  }
  
  
  /************************************************************************/
  /* Kickoff Looping Stuff                                                */
  /************************************************************************/

  rc = ecmdLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;


  while (ecmdLooperNext(target, looperdata) && (!coeRc || coeMode)) {

    rc = ecmdGetClockSpeed(target, clockType, speedType, speed, freqmode, freqMin, freqMax);
    if (rc) {
      printed = "getclockspeed - Error occured performing getclockspeed on ";
      printed += ecmdWriteTarget(target);
      printed += "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;                   
      continue;
    } else {
      validPosFound = true;     
    }
    if( freqmode == ECMD_CLOCK_MINMAX_FREQ_MODE ){
      printed = ecmdWriteTarget(target);
      printed += "  speed:";
      buffer.setWord(0, speed);
      std::string formatstr = ecmdWriteDataFormatted(buffer, outputformat);
      formatstr.erase(std::remove(formatstr.begin(), formatstr.end(), '\n'), formatstr.end());
      printed += formatstr;
      buffer.setWord(0, freqMin);
      formatstr = ecmdWriteDataFormatted(buffer, outputformat);
      formatstr.erase(std::remove(formatstr.begin(), formatstr.end(), '\n'), formatstr.end());
      printed += "  fmin:";
      printed += formatstr;
      buffer.setWord(0, freqMax);
      printed += "  fmax:";
      printed += ecmdWriteDataFormatted(buffer, outputformat);
      ecmdOutput( printed.c_str() );
    } else {
    buffer.setWord(0, speed);
    printed = ecmdWriteTarget(target);
    printed += ecmdWriteDataFormatted(buffer, outputformat);
    ecmdOutput( printed.c_str() );
    }
  }
  // coeRc will be the return code from in the loop, coe mode or not.
  if (coeRc) return coeRc;

  if (!validPosFound) {
    //this is an error common across all UI functions
    ecmdOutputError("getclockspeed - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}
#endif // ECMD_REMOVE_REFCLOCK_FUNCTIONS
