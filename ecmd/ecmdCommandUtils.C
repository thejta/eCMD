// Copyright ***********************************************************
//                                                                      
// File ecmdCommandUtils.C                                   
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
// Description: 
//
// End Module Description **********************************************

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder     Description                       
//  ---- -------- ---- -------- -----     -----------------------------
//  @01  STG4466       03/10/05 Prahl     Fix up Beam errors
//   
// End Change Log *****************************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define ecmdCommandUtils_C
#include <list>
#include <iostream>
#include <fstream>
#include <inttypes.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdSharedUtils.H>

#undef ecmdCommandUtils_C
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


uint32_t ecmdCheckExpected (ecmdDataBuffer & i_data, ecmdDataBuffer & i_expected) {

  int wordCounter = 0;
  uint32_t maxBits = 32;
  uint32_t numBits = i_data.getBitLength();
  uint32_t numToFetch = numBits < maxBits ? numBits : maxBits;
  uint32_t curData, curExpected;

  /* We are going to make sure they didn't expect more data then we had */
  /* We are going to allow for odd bits in a nibble if the user provided data in hex */
  /* We will just check below that all the extra data was zero's */
  if ((i_data.getBitLength()-1)/4 > (i_expected.getBitLength()-1)/4) {
    ecmdOutputError("ecmdCheckExpected - Not enough expect data provided\n");
    return 0;
  } else if ((i_data.getBitLength()-1)/4 < (i_expected.getBitLength()-1)/4) {
    ecmdOutputError("ecmdCheckExpected - More expect data provided than data that was retrieved\n");
    return 0;

    /* Now we are going to check to see if expect bits we specified in any odd nibble bits */
  } else if ((i_data.getBitLength() < i_expected.getBitLength()) &&
             (!i_expected.isBitClear(i_data.getBitLength(), i_expected.getBitLength() - i_data.getBitLength()))) {
    ecmdOutputError("ecmdCheckExpected - More non-zero expect data provided in odd bits of a nibble then data retrieved\n");
    return 0;
  }

  while (numToFetch > 0) {

    curData = i_data.getWord(wordCounter);
    curExpected = i_expected.getWord(wordCounter);

    if (numToFetch == maxBits) {
      if (curData != curExpected) 
        return 0;
    }
    else {
      uint32_t mask = 0x80000000;
      for (uint32_t i = 0; i < numToFetch; i++, mask >>= 1) {
        if ( (curData & mask) != (curExpected & mask) ) {
          return 0;
        }
      }
    }

    numBits -= numToFetch;
    numToFetch = (numBits < maxBits) ? numBits : maxBits;
    wordCounter++;
  }


  return 1;
        
}

uint32_t ecmdApplyDataModifier (ecmdDataBuffer & io_data, ecmdDataBuffer & i_newData, int i_startbit, std::string i_modifier) {
  uint32_t rc = ECMD_SUCCESS;


  if ((i_startbit + i_newData.getBitLength()) > io_data.getBitLength()) {
    char buf[200];
    sprintf(buf,"ecmdApplyDataModifier - startbit + numbits (%d) > data length (%d), buffer overflow!\n",i_startbit + i_newData.getBitLength(), io_data.getBitLength());
    ecmdOutputError(buf);
    return ECMD_INVALID_ARGS;
  }

  if (i_modifier == "insert") {
    io_data.insert(i_newData, i_startbit, i_newData.getBitLength());
  } else if (i_modifier == "and") {
    io_data.setAnd(i_newData, i_startbit, i_newData.getBitLength());
  } else if (i_modifier == "or") {
    io_data.setOr(i_newData, i_startbit, i_newData.getBitLength());
  } else {
    ecmdOutputError(("ecmdApplyDataModifier - Invalid Data Modifier specified with -b arg : "+i_modifier + "\n").c_str());
    return ECMD_INVALID_ARGS;
  }


  return rc;
}

uint32_t ecmdPrintHelp(const char* i_command) {

  uint32_t rc = ECMD_SUCCESS;
  std::string file;
  ecmdChipTarget target;
  std::ifstream ins;
  std::string curLine;

  /* Get the path to the help text files */
  rc = ecmdQueryFileLocation(target, ECMD_FILE_HELPTEXT, file);
  if (rc) return rc;

  file += i_command; file += ".htxt";

  /* Let's go open this guy */
  ins.open(file.c_str());
  if (ins.fail()) {
    ecmdOutputError(("Error occured opening help text file: " + file + "\n").c_str());
    return ECMD_INVALID_ARGS;  //change this
  }

  while (getline(ins, curLine)) {
    curLine += '\n';
    ecmdOutput(curLine.c_str());
  }
  ins.close();

  return rc;

}
  

/* Returns true if all chars of str are decimal numbers */
bool ecmdIsAllDecimal(const char* str) {

  bool ret = true;
  int len = strlen(str);
  for (int x = 0; x < len; x ++) {
    if (!isdigit(str[x])) {
      ret = false;
      break;
    }
  }

  return ret;
}

/* Returns true if all chars of str are hex numbers */
bool ecmdIsAllHex(const char* str) {

  bool ret = true;
  int len = strlen(str);
  for (int x = 0; x < len; x ++) {
    if (!isxdigit(str[x])) {
      ret = false;
      break;
    }
  }

  return ret;
}


std::string ecmdParseReturnCode(uint32_t i_returnCode) {
  std::string ret = "";

  ecmdChipTarget dummy;
  std::string filePath;
  uint32_t rc = ecmdQueryFileLocation(dummy, ECMD_FILE_HELPTEXT, filePath); 

  if (rc || (filePath.length()==0)) {
    ret = "ERROR FINDING DECODE FILE";
    return ret;
  }

  filePath += "ecmdReturnCodes.H";


  std::string line;
  std::vector< std::string > tokens;
  std::string source, retdefine;
  int found = 0;
  uint32_t comprc;


  std::ifstream ins(filePath.c_str());

  if (ins.fail()) {
    ret = "ERROR OPENING DECODE FILE";
    return ret;
  }

  /* This is what I am trying to parse from ecmdReturnCodes.H */

  /* #define ECMD_ERR_UNKNOWN                        0x00000000 ///< This error code wasn't flagged to which plugin it came from        */
  /* #define ECMD_ERR_ECMD                           0x01000000 ///< Error came from eCMD                                               */
  /* #define ECMD_ERR_CRONUS                         0x02000000 ///< Error came from Cronus                                             */
  /* #define ECMD_ERR_IP                             0x04000000 ///< Error came from IP GFW                                             */
  /* #define ECMD_ERR_Z                              0x08000000 ///< Error came from Z GFW                                              */
  /* #define ECMD_INVALID_DLL_VERSION                (ECMD_ERR_ECMD | 0x1000) ///< Dll Version                                          */


  while (!ins.eof()) { /*  && (strlen(str) != 0) */
    getline(ins,line,'\n');
    /* Let's strip off any comments */
    line = line.substr(0, line.find_first_of("/"));
    ecmdParseTokens(line, " \n()|", tokens);

    /* Didn't find anything */
    if (line.size() < 2) continue;

    if (tokens[0] == "#define") {
      /* Let's see if we have one of they return code source defines */
      if ((tokens.size() == 3) && (tokens[1] != "ECMD_SUCCESS") && (tokens[1] != "ECMD_DBUF_SUCCESS")) {
        sscanf(tokens[2].c_str(),"0x%x",&comprc);
        if ((i_returnCode & 0xFF000000) == comprc) {
          /* This came from this source, we will save that as we may use it later */
          source = tokens[1];
        }
      } else if ((i_returnCode & 0xFF000000) != ECMD_ERR_ECMD) {
        /* We aren't going to find this return code in here since it didn't come from us */
      } else if (tokens.size() >= 4) {
        /* This is a standard return code define */
        sscanf(tokens[3].c_str(),"0x%x",&comprc);
        if ((i_returnCode & 0x00FFFFFF) == comprc) {
          /* This came from this source, we will save that as we may use it later */
          retdefine = tokens[1];
          found = 1;
          break;
        }
      }        
    }
  }

  ins.close();

  if (!found && source.length() == 0) {
    ret = "UNDEFINED";
  } else if (!found) {
    ret = "UNDEFINED FROM " + source;
  } else {
    ret = retdefine;
  }
  return ret;
}

uint32_t ecmdParseStdinCommands(std::vector< std::string > & o_commands) {
  std::string buffer;

  o_commands.clear();

  if (!std::cin) {
    /* We have reached EOF */
    return ECMD_SUCCESS;
  } else {
    getline(std::cin, buffer);

    /* Carve up what we have here */
    ecmdParseTokens(buffer,"\n;", o_commands);

  }
  return 1;
}

uint32_t ecmdParseTargetFields(int *argc, char ** argv[], char *targetField, ecmdChipTarget &target, uint8_t &targetFieldType, std::string
&targetFieldList) {
  uint32_t rc = ECMD_SUCCESS;
  
  uint8_t ONE = 0;
  uint8_t MANY = 1;
  
  std::string printed;           ///< Print Buffer
  std::string patterns = ".,"; 

  char *targetPtr;
  bool isCage = false;
  bool isNode = false;
  bool isSlot = false;
  bool isPos = false;
  bool isCore = false;
  char arg[5] = ""; //@01a add init 
  
  if (strcmp(targetField, "cage")==0) {
    isCage = true;
    strcpy(arg, "-k");
  }
  else if (strcmp(targetField, "node")==0) {
    isNode = true;
    strcpy(arg, "-n");
  }
  else if (strcmp(targetField, "slot")==0) {
    isSlot = true;
    strcpy(arg, "-s");
  }
  else if (strcmp(targetField, "pos")==0) {
    isPos = true;
    strcpy(arg, "-p");
  }
  else if (strcmp(targetField, "core")==0) {
    isCore = true;
    strcpy(arg, "-c");
  }
  
  targetPtr = ecmdParseOptionWithArgs(argc, argv, arg);
  
  
  if(targetPtr != NULL) {
   targetFieldList = targetPtr;
   if(targetFieldList == "all") {
     printed = "'all' for targets not supported.\n";
     ecmdOutputError( printed.c_str() );
     return ECMD_INVALID_ARGS;
   }
   else if (targetFieldList.find_first_of(patterns) < targetFieldList.length()) {
     if (!isTargetStringValid(targetFieldList)) {
       printed = (std::string)arg + " argument contained invalid characters\n";
       ecmdOutputError(printed.c_str());
       return ECMD_INVALID_ARGS;
     }
     targetFieldType = MANY;
   }
   else {
     if (targetFieldList.length() != 0) {
       if (!isTargetStringValid(targetFieldList)) {
         printed = (std::string)arg + " argument contained invalid characters\n";
   	 ecmdOutputError(printed.c_str());
   	 return ECMD_INVALID_ARGS;
       }
       if(isCage) {
         target.cage = atoi(targetFieldList.c_str());
       }
       else if(isNode) {
         target.node = atoi(targetFieldList.c_str());
       }
       else if(isSlot) {
         target.slot = atoi(targetFieldList.c_str());
       }
       else if(isPos) {
         target.pos = atoi(targetFieldList.c_str());
       }
       else if(isCore) {
         target.core = atoi(targetFieldList.c_str());
       }
     }
     else {
       if(isCage) {
         target.cage = 0x0;
       }
       else if(isNode) {
         target.node = 0x0;
       }
       else if(isSlot) {
         target.slot = 0x0;
       }
       else if(isPos) {
         target.pos = 0x0;
       }
       else if(isCore) {
         target.core = 0x0;
       }
     }
     targetFieldType = ONE;
   }
   if(isCage) {
    target.cageState = ECMD_TARGET_FIELD_VALID;
   }
   else if(isNode) {
    target.cageState = ECMD_TARGET_FIELD_VALID;
    target.nodeState = ECMD_TARGET_FIELD_VALID;
   } 
   else if(isSlot) {
    target.cageState = ECMD_TARGET_FIELD_VALID;
    target.nodeState = ECMD_TARGET_FIELD_VALID;
    target.slotState = ECMD_TARGET_FIELD_VALID;
   }
   else if(isPos) {
    target.cageState = ECMD_TARGET_FIELD_VALID;
    target.nodeState = ECMD_TARGET_FIELD_VALID;
    target.slotState = ECMD_TARGET_FIELD_VALID;
    target.posState = ECMD_TARGET_FIELD_VALID;
   }
   else if(isCore) {
    target.cageState = ECMD_TARGET_FIELD_VALID;
    target.nodeState = ECMD_TARGET_FIELD_VALID;
    target.slotState = ECMD_TARGET_FIELD_VALID;
    target.posState = ECMD_TARGET_FIELD_VALID;
    target.coreState = ECMD_TARGET_FIELD_VALID;
   }    
  }
  else {
    if(isCage) {
     target.cage = 0x0;
    }
    else if(isNode) {
     target.node = 0x0;
    }
    else if(isSlot) {
     target.slot = 0x0;
    }
    else if(isPos) {
     target.pos = 0x0;
    }
    else if(isCore) {
     target.core = 0x0;
    }
    targetFieldType = ONE;
  }
  return rc;
}

/* Returns true if all chars of str are decimal numbers */
bool isTargetStringValid(std::string str) {

  bool ret = true;
  for (uint32_t x = 0; x < str.length(); x ++) {
    if (isdigit(str[x])) {
    } else if (str[x] == ',') {
    } else if (str[x] == '.' && str[x+1] == '.') {
      x++;
    } else {
      ret = false;
      break;
    }
  }

  return ret;
}

void getTargetList (std::string userArgs, std::list<uint32_t> &targetList) {
  
  std::string curSubstr;
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

      int lowerBound = atoi(curSubstr.substr(0,tmpOffset).c_str());
      int upperBound = atoi(curSubstr.substr(tmpOffset+2, curSubstr.length()).c_str());
      
      int curPos = lowerBound;
      while (lowerBound <= curPos && curPos <= upperBound) {
        targetList.push_back(curPos);
	curPos++;
      }
    }
    else {

      int curValidPos = atoi(curSubstr.c_str());
      targetList.push_back(curValidPos);
    }

    curOffset = nextOffset+1;

  }

}

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//                     10/8/04  CENGEL   Added ecmdParseStdinCommands BZ#261
//
// End Change Log *****************************************************
