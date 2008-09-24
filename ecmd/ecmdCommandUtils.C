/* $Header$ */
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


uint32_t ecmdCheckExpected (ecmdDataBuffer & i_data, ecmdDataBuffer & i_expected, uint32_t & o_mismatchBit) {

  uint32_t wordCounter = 0;
  uint32_t maxBits = 32;
  uint32_t numBits = i_data.getBitLength();
  uint32_t numToFetch = numBits < maxBits ? numBits : maxBits;
  uint32_t curData, curExpected;

  /* We are going to make sure they didn't expect more data then we had */
  /* We are going to allow for odd bits in a nibble if the user provided data in hex */
  /* We will just check below that all the extra data was zero's */
  if ((i_data.getBitLength()-1)/4 > (i_expected.getBitLength()-1)/4) {
    ecmdOutputError("ecmdCheckExpected - Not enough expect data provided\n");
    o_mismatchBit = ECMD_UNSET;
    return 0;
  } else if ((i_data.getBitLength()-1)/4 < (i_expected.getBitLength()-1)/4) {
    ecmdOutputError("ecmdCheckExpected - More expect data provided than data that was retrieved\n");
    o_mismatchBit = ECMD_UNSET;
    return 0;

    /* Now we are going to check to see if expect bits we specified in any odd nibble bits */
  } else if ((i_data.getBitLength() < i_expected.getBitLength()) &&
             (!i_expected.isBitClear(i_data.getBitLength(), i_expected.getBitLength() - i_data.getBitLength()))) {
    ecmdOutputError("ecmdCheckExpected - More non-zero expect data provided in odd bits of a nibble then data retrieved\n");
    o_mismatchBit = ECMD_UNSET;
    return 0;
  }

  while (numToFetch > 0) {

    curData = i_data.getWord(wordCounter);
    curExpected = i_expected.getWord(wordCounter);

    if (numToFetch == maxBits) {
      if (curData != curExpected) {
	if (o_mismatchBit != ECMD_UNSET) {
	  uint32_t mask = 0x80000000;
	  for (uint32_t i = 0; i < numToFetch; i++, mask >>= 1) {
	    if ( (curData & mask) != (curExpected & mask) ) {
	      o_mismatchBit = (wordCounter * 32) + i;
	      break;
	    }
	  }
	}
        return 0;
      }
    }
    else {
      uint32_t mask = 0x80000000;
      for (uint32_t i = 0; i < numToFetch; i++, mask >>= 1) {
        if ( (curData & mask) != (curExpected & mask) ) {
	  if (o_mismatchBit != ECMD_UNSET) 
	    o_mismatchBit = (wordCounter * 32) + i;
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

uint32_t ecmdApplyDataModifier(ecmdDataBuffer & io_data, ecmdDataBuffer & i_newData, uint32_t i_startbit, std::string i_modifier) {
  return ecmdApplyDataModifierHidden(io_data, i_newData, i_startbit, i_modifier);
}

uint32_t ecmdApplyDataModifierHidden(ecmdDataBuffer & io_data, ecmdDataBuffer & i_newData, uint32_t i_startBit, std::string i_modifier, ecmdEndianMode_t i_endianMode) {
  uint32_t rc = ECMD_SUCCESS;
  uint32_t length;

  if (i_endianMode == ECMD_LITTLE_ENDIAN) {
    if ((i_startBit - i_newData.getBitLength()) < 0) {
      length = io_data.getBitLength() - i_startBit;
      /* If there are bits on past the length, let's error.  Otherwise, let it through */
      if (i_newData.isBitSet((i_startBit + length), (i_newData.getBitLength() - (i_startBit+length)))) {
        char buf[200];	     
        sprintf(buf,"ecmdApplyDataModifier - There are bits set past on the input data(%d) past the length of the destination data(%d)!\n", i_newData.getBitLength(), io_data.getBitLength());
        ecmdOutput(buf);
        return ECMD_INVALID_ARGS;
      }
    } else {
      length = i_newData.getBitLength();
    }

    /* On a little endian insert, we need to mess with the data and our start offsets to put the LE data into a BE buffer */
    /* Here is how the math below works
     io_data.getBitLength() is the length of the entire data buffer, this gives us our conversion point from LE to BE
     If io_data is 64 bits long, then our two different views of the data are:
     Storage (BE)
     0                         64
     User View (LE)
     63                         0

     We need to convert their LE start bit into something that works with our BE buffers.
     When doing an insert at 12, for 4 bits, they expect the insert to go from 12 down to 9.

     At first thought, this is just getBitLength() - i_startBit.  However, you really want the end bit, 63, which is getBitLength() - 1.

     Once you have the start location, since the buffer is oriented like this:
        6         5         4          3         2         1         0
     32109876543210987654321098765432 10987654321098765432109876543210

     You can then just take the data and insert it starting at bit 12 and it will set 12,11,10,9

     Example:

     64 - 12 - 1 = 51 in our BE buffer

     Then insert 0b0101 at 51 for 4.  That sets bits 12, 11, 10 and 9 in our LE buffer.

     That sums things up pretty well I think.  Hopefully typing out this comment was worth something to someone in 2 years.  09/24/08 - JTA

     Addendum - we can only do this if they are messing with subranges of io_data.  If i_newData matches io_data in length, they have provided
     the entire image and just overlay that.
     */
    uint32_t leStartBit;

    if (io_data.getBitLength() == i_newData.getBitLength()) {
      leStartBit = i_startBit;
    } else {
      leStartBit = io_data.getBitLength() - 1 - i_startBit;  
    }

    if (i_modifier == "insert") {
      io_data.insert(i_newData, leStartBit, length);
    } else if (i_modifier == "and") {
      io_data.setAnd(i_newData, leStartBit, length);
    } else if (i_modifier == "or") {
      io_data.setOr(i_newData, leStartBit, length);
    } else {
      ecmdOutputError(("ecmdApplyDataModifier - Invalid Data Modifier specified with -b arg : " + i_modifier + "\n").c_str());
      return ECMD_INVALID_ARGS;
    }
  } else {
    /* We no longer just error out on length problems, let's try and be smart about it */
    /* STGC00108031 - JTA 01/31/07 */
    if ((i_startBit + i_newData.getBitLength()) > io_data.getBitLength()) {
      length = io_data.getBitLength() - i_startBit;
      /* If there are bits on past the length, let's error.  Otherwise, let it through */
      if (i_newData.isBitSet((i_startBit + length), (i_newData.getBitLength() - (i_startBit+length)))) {
        char buf[200];	     
        sprintf(buf,"ecmdApplyDataModifier - There are bits set past on the input data(%d) past the length of the destination data(%d)!\n", i_newData.getBitLength(), io_data.getBitLength());
        ecmdOutput(buf);
        return ECMD_INVALID_ARGS;
      }
    } else {
      length = i_newData.getBitLength();
    }

    if (i_modifier == "insert") {
      io_data.insert(i_newData, i_startBit, length);
    } else if (i_modifier == "and") {
      io_data.setAnd(i_newData, i_startBit, length);
    } else if (i_modifier == "or") {
      io_data.setOr(i_newData, i_startBit, length);
    } else {
      ecmdOutputError(("ecmdApplyDataModifier - Invalid Data Modifier specified with -b arg : "+i_modifier + "\n").c_str());
      return ECMD_INVALID_ARGS;
    }
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
  size_t len = strlen(str);
  for (size_t x = 0; x < len; x ++) {
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
  size_t len = strlen(str);
  for (size_t x = 0; x < len; x ++) {
    if (!isxdigit(str[x])) {
      ret = false;
      break;
    }
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

uint32_t ecmdParseTargetFields(int *argc, char ** argv[], char *targetField, ecmdChipTarget &target, uint8_t &targetFieldType, std::string &targetFieldList) {
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
  bool isThread =false;
  char arg[6] = ""; //@01a add init 
  
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
  else if (strcmp(targetField, "thread")==0) {
    isThread = true;
    strcpy(arg, "-t");
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
         target.cage = (uint32_t)atoi(targetFieldList.c_str());
       }
       else if(isNode) {
         target.node = (uint32_t)atoi(targetFieldList.c_str());
       }
       else if(isSlot) {
         target.slot = (uint32_t)atoi(targetFieldList.c_str());
       }
       else if(isPos) {
         target.pos = (uint32_t)atoi(targetFieldList.c_str());
       }
       else if(isCore) {
         target.core = (uint8_t)atoi(targetFieldList.c_str());
       }
       else if(isThread) {
         target.thread = (uint8_t)atoi(targetFieldList.c_str());
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
       else if(isThread) {
         target.thread = 0x0;
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
   else if(isThread) {
    target.cageState = ECMD_TARGET_FIELD_VALID;
    target.nodeState = ECMD_TARGET_FIELD_VALID;
    target.slotState = ECMD_TARGET_FIELD_VALID;
    target.posState = ECMD_TARGET_FIELD_VALID;
    target.coreState = ECMD_TARGET_FIELD_VALID;
    target.threadState = ECMD_TARGET_FIELD_VALID;
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
    else if(isThread) {
     target.thread = 0x0;
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

      uint32_t lowerBound = (uint32_t)atoi(curSubstr.substr(0,tmpOffset).c_str());
      uint32_t upperBound = (uint32_t)atoi(curSubstr.substr(tmpOffset+2, curSubstr.length()).c_str());
      
      uint32_t curPos = lowerBound;
      while (lowerBound <= curPos && curPos <= upperBound) {
        targetList.push_back(curPos);
	curPos++;
      }
    }
    else {

      uint32_t curValidPos = (uint32_t)atoi(curSubstr.c_str());
      targetList.push_back(curValidPos);
    }

    curOffset = nextOffset+1;

  }
}
