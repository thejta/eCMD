//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2003,2018 IBM International Business Machines Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
//IBM_PROLOG_END_TAG
#include <ecmdSharedUtils.H>
#include <stdio.h>

// This can be set by the user or plugin by calling ecmdSetTargetDisplayMode
ecmdTargetDisplayMode_t pluginDisplayMode = ECMD_DISPLAY_TARGET_DEFAULT; // FIXME make static

//----------------------------------------------------------------------
//  Internal Function Prototypes
//----------------------------------------------------------------------
/**
 * @brief Converts a ecmdChipTargetState_t enum to a std::string
 * @retval The string of State enum
 * @param i_targetState State enum
 */
std::string ecmdWriteTargetState(ecmdChipTargetState_t i_targetState); // FIXME make static

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------
std::string ecmdWriteTargetState(ecmdChipTargetState_t i_targetState)
{
  std::string str;

  switch (i_targetState)
  {
    case ECMD_TARGET_UNKNOWN_STATE: {str="(UNK)"; break;}
    case ECMD_TARGET_FIELD_VALID: {str="(V)"; break;}
    case ECMD_TARGET_FIELD_UNUSED: {str="(U)"; break;}
    case ECMD_TARGET_FIELD_WILDCARD: {str="(WC)"; break;}
    case ECMD_TARGET_THREAD_ALIVE: {str="(TA)"; break;}

    default: {str="(ERROR)"; break;}
  }

  return str;
}

void ecmdSetTargetDisplayMode(ecmdTargetDisplayMode_t i_displayMode) {
  pluginDisplayMode = i_displayMode;
}

std::string ecmdWriteTarget(ecmdChipTarget & i_target, ecmdTargetDisplayMode_t i_displayMode)
{
    return ecmdWriteTarget((const ecmdChipTarget &) i_target, i_displayMode);
}

std::string ecmdWriteTarget(const ecmdChipTarget & i_target, ecmdTargetDisplayMode_t i_displayMode) {

  std::string printed;
  char util[20];
  bool hexMode = false;
  bool stateMode = false;
  std::string subPrinted;

  if (i_displayMode == ECMD_DISPLAY_TARGET_PLUGIN_MODE) {
    i_displayMode = pluginDisplayMode;    
  }

  if (i_displayMode == ECMD_DISPLAY_TARGET_HEX_DEFAULT || i_displayMode == ECMD_DISPLAY_TARGET_HEX_COMPRESSED || i_displayMode == ECMD_DISPLAY_TARGET_HEX_HYBRID || i_displayMode == ECMD_DISPLAY_TARGET_STATES_HEX) {
    hexMode = true;
  }

  if (i_displayMode == ECMD_DISPLAY_TARGET_STATES_DECIMAL || i_displayMode == ECMD_DISPLAY_TARGET_STATES_HEX) {
    stateMode = true;
  }

  if ((i_displayMode == ECMD_DISPLAY_TARGET_DEFAULT ||
       i_displayMode == ECMD_DISPLAY_TARGET_HEX_DEFAULT ||
       i_displayMode == ECMD_DISPLAY_TARGET_HYBRID ||
       i_displayMode == ECMD_DISPLAY_TARGET_HEX_HYBRID ||
       i_displayMode == ECMD_DISPLAY_TARGET_COMMANDLINE) && (i_target.chipTypeState == ECMD_TARGET_FIELD_VALID)) {
    printed += i_target.chipType;
    if (i_target.chipUnitTypeState == ECMD_TARGET_FIELD_VALID) {
      printed += ".";
      printed += i_target.chipUnitType;
    }
    if (i_displayMode == ECMD_DISPLAY_TARGET_DEFAULT || i_displayMode == ECMD_DISPLAY_TARGET_HEX_DEFAULT) {
      printed += "\t";
    } else if (i_displayMode == ECMD_DISPLAY_TARGET_HYBRID || i_displayMode == ECMD_DISPLAY_TARGET_HEX_HYBRID) {
      printed += ":";
    }
  }
  else if (stateMode)
  {
        printed += i_target.chipType;
        printed += ecmdWriteTargetState(i_target.chipTypeState); 
        printed += ".";
        printed += i_target.chipUnitType;
        printed += ecmdWriteTargetState(i_target.chipUnitTypeState);
        printed += " ";
  }


  /* Put the hex prefix onto the output */
  if (hexMode) {
    subPrinted += "0x[";
  }

  if (i_target.cageState == ECMD_TARGET_FIELD_VALID || stateMode) {
    if (i_displayMode == ECMD_DISPLAY_TARGET_COMMANDLINE) {
      subPrinted += " -";
    } else {
      /* Nothing for cage */
    }
    if (hexMode) {
      sprintf(util, "k%X", i_target.cage);
    } else {
      sprintf(util, "k%d", i_target.cage);
    }
    subPrinted += util;
    if (stateMode) subPrinted += ecmdWriteTargetState(i_target.cageState);

    if (i_target.nodeState == ECMD_TARGET_FIELD_VALID || stateMode) {
      if (i_displayMode == ECMD_DISPLAY_TARGET_COMMANDLINE) {
        subPrinted += " -";
      } else {
        subPrinted += ":";
      }

      if (i_target.node == ECMD_TARGETDEPTH_NA) {
        sprintf(util, "n-");
        subPrinted += util;
      } else {
        if (hexMode) {
          sprintf(util, "n%X", i_target.node);
        } else {
          sprintf(util, "n%d", i_target.node);
        }
        subPrinted += util;
      }
      if (stateMode) subPrinted += ecmdWriteTargetState(i_target.nodeState);

      if (i_target.slotState == ECMD_TARGET_FIELD_VALID || stateMode) {
        if (i_displayMode == ECMD_DISPLAY_TARGET_COMMANDLINE) {
          subPrinted += " -";
        } else {
          subPrinted += ":";
        }

        if (i_target.slot == ECMD_TARGETDEPTH_NA) {
          sprintf(util, "s-");
          subPrinted += util;
        } else {
          if (hexMode) {
            sprintf(util, "s%X", i_target.slot);
          } else {
            sprintf(util, "s%d", i_target.slot);
          }
          subPrinted += util;
        }
        if (stateMode) subPrinted += ecmdWriteTargetState(i_target.slotState);

        if (((i_target.posState == ECMD_TARGET_FIELD_VALID) && (i_target.chipTypeState == ECMD_TARGET_FIELD_VALID))
             || stateMode)  {

          if (i_displayMode == ECMD_DISPLAY_TARGET_COMPRESSED || i_displayMode == ECMD_DISPLAY_TARGET_HEX_COMPRESSED) {
            subPrinted += ":";
            subPrinted += i_target.chipType;
            if (i_target.chipUnitTypeState == ECMD_TARGET_FIELD_VALID) {
              subPrinted += ".";
              subPrinted += i_target.chipUnitType;
            }
          }

          if (i_displayMode == ECMD_DISPLAY_TARGET_COMMANDLINE) {
            subPrinted += " -";
          } else {
            subPrinted += ":";
          }

          if (hexMode) {
            sprintf(util, "p%X", i_target.pos);
          } else {
            sprintf(util, "p%02d", i_target.pos);
          }
          subPrinted += util;
          if (stateMode) subPrinted += ecmdWriteTargetState(i_target.posState);
 
          if (i_target.chipUnitNumState == ECMD_TARGET_FIELD_VALID || stateMode) {
            if (i_displayMode == ECMD_DISPLAY_TARGET_COMMANDLINE) {
              subPrinted += " -";
            } else {
              subPrinted += ":";
            }

            if (hexMode) {
              sprintf(util, "c%X", i_target.chipUnitNum);
            } else {
              sprintf(util, "c%d", i_target.chipUnitNum);
            }
            subPrinted += util;
            if (stateMode) subPrinted += ecmdWriteTargetState(i_target.chipUnitNumState);

            if (i_target.threadState == ECMD_TARGET_FIELD_VALID || stateMode) {
              if (i_displayMode == ECMD_DISPLAY_TARGET_COMMANDLINE) {
                subPrinted += " -";
              } else {
                subPrinted += ":";
              }

              if (hexMode) {
                sprintf(util, "t%X", i_target.thread);
              } else {
                sprintf(util, "t%d", i_target.thread);
              }
              subPrinted += util;
              if (stateMode) subPrinted += ecmdWriteTargetState(i_target.threadState);
            }
          } //chipUnitNum
        } //pos
      } //slot
    } //node
  } //cage

  /* The closing bracket for hex mode */
  if (hexMode) {
    subPrinted += "]";
  }

  /* Add the unitId and state */
  if (stateMode) {
    subPrinted += " (uid 0x";
    sprintf(util, "%X", i_target.unitId);
    subPrinted += util;
    subPrinted += ecmdWriteTargetState(i_target.unitIdState);
    subPrinted += ")";
  }


  /* Now put the subPrinted stuff onto the printed string so we've got the full thing */
  printed += subPrinted;

  /* For the default display modes, there are a couple extra things we want to do */
  if (i_displayMode == ECMD_DISPLAY_TARGET_DEFAULT || i_displayMode == ECMD_DISPLAY_TARGET_HEX_DEFAULT) {
    /* If the generated string is was shorter than 18 characters, pad output with whitespace */
    int subPrintedLength = subPrinted.length();
    if (subPrintedLength < 18) {
      sprintf(util, "%*s", (18 - subPrintedLength) , "");
      printed += util;
    }

    //ensure there is a space between the target info and the data
    printed += " "; 
  }

  return printed;
}

