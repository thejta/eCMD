/* $Header$ */
// Copyright ***********************************************************
//
// File ecmdSharedUtils.C
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
//  @01  STGC7449      04/18/05 prahl     Fix up Beam messages.
//   
// End Change Log *****************************************************

//lint -e571 The casts are necessary in ecmdHashString - JTA
//lint -e744 We don't want there to be a default in the ecmdHashString case - JTA

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <string>
#include <vector>
#include <stdio.h>
#include <fstream>

#include <ecmdSharedUtils.H>
#include <ecmdStructs.H>
#include <ecmdReturnCodes.H>

// For Calling Hashing Algorithm in MCP: static inline u32 jhash(const void *key, u32 length, u32 initval)
#ifdef ECMD_USE_MCP
#include <stdio.h>

// Needed for jhash() call
#include <sys/types.h>
typedef u_int32_t u32 ;
typedef u_int8_t u8 ;

#define void u8
#include <linux/jhash.h>
#undef void
#endif // ECMD_USE_MCP


// This can be set by the user or plugin by calling ecmdSetTargetDisplayMode
ecmdTargetDisplayMode_t pluginDisplayMode = ECMD_DISPLAY_TARGET_DEFAULT;

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
/**
 * @brief Iterates over argv, removing null pointers and decrementing argc
 * @retval None
 * @param io_argc Pointer to number of elements in io_argv array
 * @param io_argv Array of strings passed in from command line

 - Utility function for ecmdParseOption and ecmdParseOptionWithArgs
 */
void ecmdRemoveNullPointers (int * io_argc, char ** io_argv[]);


//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------

void ecmdRemoveNullPointers (int *argc, char **argv[]) {
  int counter=0;
  int counter2=0;

  for (counter=0;counter<(*argc+1);counter++) {
    for (counter2=counter;counter2<*argc;counter2++) {
      if ((*argv)[counter]==NULL) {
        (*argv)[counter]=(*argv)[counter2];
        (*argv)[counter2]=NULL;
      }
    }
  }

  for (counter=0;counter<(*argc);counter++) {
    if ((*argv)[counter]==NULL) {
      *argc=counter;
      return;
    }
  }
}


bool ecmdParseOption (int *argc, char **argv[], const char *option) {
  int counter = 0;
  bool foundit = false;

  for (counter = 0; counter < *argc ; counter++) {
    if (((*argv)[counter] != NULL) && (strcmp((*argv)[counter],option)==0)) {
      (*argv)[counter]=NULL;
      foundit = true;
      break;
    }
  }

  ecmdRemoveNullPointers(argc, argv);
  return foundit;
}

/* ----------------------------------------------------------------- */
/* Function will parse for an option and eliminate it from a list    */
/* while returning a pointer to the option that is used.             */
/* If no option is available, the function will return NULL          */
/* ----------------------------------------------------------------- */
char * ecmdParseOptionWithArgs(int *argc, char **argv[], const char *option) {
  int counter = 0;
  char *returnValue=NULL;

  for (counter = 0; counter < *argc ; counter++) {
    if (((*argv)[counter] != NULL) && (strncmp((*argv)[counter],option,strlen(option))==0)) {
      if (strlen((*argv)[counter])>strlen(option)) {
        returnValue = &((*argv)[counter][strlen(option)]);
        (*argv)[counter]=NULL;
      } else {
        if ((counter+1)<*argc) {
          returnValue = (*argv)[counter+1];
          (*argv)[counter]=NULL;
          (*argv)[counter+1]=NULL;
        } else {
          returnValue = NULL;
        }
      }
      /* We found it , let's stop looping , we don't want to pull other args out if they are here, this fixes BZ#6 */
      break;

    }
  }

  ecmdRemoveNullPointers(argc, argv);

  return returnValue;
}

void ecmdParseTokens (std::string line, const char* seperators, std::vector<std::string> & tokens) {

  size_t curStart = 0, curEnd = 0;

  tokens.clear();

  while (1) {
    curStart = line.find_first_not_of(seperators, curEnd);
    if (curStart == std::string::npos) break;
    curEnd = line.find_first_of(seperators,curStart);
    tokens.push_back(line.substr(curStart, curEnd-curStart));
  }
}


std::string ecmdGenEbcdic(ecmdDataBuffer &i_data, int start, int bitLen) {
  std::string ret;
  std::string tempChar;
  // Conversion table
  char EBCDICtoASCIITable [256] = {
    0x2E,0x01,0x02,0x03,0x2E,0x09,0x2E,0x7f,
    0x2E,0x2E,0x2E,0x0b,0x0c,0x0d,0x0e,0x0f,
    0x10,0x11,0x12,0x13,0x2E,0x2E,0x08,0x2E,
    0x18,0x19,0x2E,0x2E,0x1c,0x1d,0x1e,0x1f,
    0x2E,0x2E,0x2E,0x2E,0x2E,0x0a,0x17,0x1b,
    0x2E,0x2E,0x2E,0x2E,0x2E,0x05,0x06,0x07,
    0x2E,0x2E,0x16,0x2E,0x2E,0x2E,0x2E,0x04,
    0x2E,0x2E,0x2E,0x2E,0x14,0x15,0x2E,0x1a,
    0x20,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,
    0x2E,0x2E,0x2E,0x2e,0x3c,0x28,0x2b,0x7c,
    0x26,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,
    0x2E,0x2E,0x21,0x24,0x2a,0x29,0x3b,0x5e,
    0x2d,0x2f,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,
    0x2E,0x2E,0x2E,0x2c,0x25,0x5f,0x3e,0x3f,
    0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,
    0x2E,0x60,0x3a,0x23,0x40,0x27,0x3d,0x22,
    0x2E,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
    0x68,0x69,0x2E,0x7b,0x2E,0x2E,0x2E,0x2E,
    0x2E,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,
    0x71,0x72,0x2E,0x7d,0x2E,0x2E,0x2E,0x2E,
    0x2E,0x7e,0x73,0x74,0x75,0x76,0x77,0x78,
    0x79,0x7a,0x2E,0x2E,0x2E,0x5b,0x2E,0x2E,
    0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,
    0x2E,0x2E,0x2E,0x2E,0x2E,0x5d,0x2E,0x2E,
    0x7b,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
    0x48,0x49,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,
    0x7d,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,
    0x51,0x52,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,
    0x5c,0x2E,0x53,0x54,0x55,0x56,0x57,0x58,
    0x59,0x5a,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x38,0x39,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,
};

  /* Loop through the dataBuffer and convert to EBCDIC */
  int startByte = start/8;
  int endByte = (start+bitLen)/8;
  for (int x = startByte; x < endByte; x++) {
    tempChar = EBCDICtoASCIITable[i_data.getByte(x)];
    ret.insert(ret.length(),tempChar);
  }

  return ret;
}

#define mix(a,b,c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}

uint32_t ecmdHashString32(const char *k, uint32_t c)
{
#ifdef ECMD_USE_MCP
// Call MCP's jhash() function directly 
return jhash((const u8*)k, strlen(k), c);
#endif // ECMD_USE_MCP

    uint32_t length;
    register uint32_t a,b,len;

    /* Set up the internal state */
    len = strlen(k);
    length = len;

    a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */

    /*---------------------------------------- handle most of the key */
    while (len >= 12)
    {
        a += (k[0] +((uint32_t)k[1]<<8) +((uint32_t)k[2]<<16) +((uint32_t)k[3]<<24));
        b += (k[4] +((uint32_t)k[5]<<8) +((uint32_t)k[6]<<16) +((uint32_t)k[7]<<24));
        c += (k[8] +((uint32_t)k[9]<<8) +((uint32_t)k[10]<<16)+((uint32_t)k[11]<<24));
        mix(a,b,c);
        k += 12; len -= 12;
    }
    /*------------------------------------- handle the last 11 bytes */
    c += length;
    // Add /*fall through*/ to tell Beam to ignore and not throw an error. @01a
    switch(len)  /* all the case statements fall through */
    {
    case 11: c+=((uint32_t)k[10]<<24); /*fall through*/
    case 10: c+=((uint32_t)k[9]<<16);  /*fall through*/
    case 9 : c+=((uint32_t)k[8]<<8);
        /* the first byte of c is reserved for the length */ /*fall through*/
    case 8 : b+=((uint32_t)k[7]<<24);  /*fall through*/
    case 7 : b+=((uint32_t)k[6]<<16);  /*fall through*/
    case 6 : b+=((uint32_t)k[5]<<8);   /*fall through*/
    case 5 : b+=k[4];                  /*fall through*/
    case 4 : a+=((uint32_t)k[3]<<24);  /*fall through*/
    case 3 : a+=((uint32_t)k[2]<<16);  /*fall through*/
    case 2 : a+=((uint32_t)k[1]<<8);   /*fall through*/
    case 1 : a+=k[0];
        /* case 0: nothing left to add */
    }
    mix(a,b,c);

    return(c);
}

uint32_t ecmdHexToUInt32(const char* str)
{
	const char* start = str + 2;
	return strtoull(start, NULL, 16);
}

uint32_t ecmdSetTargetDepth(ecmdChipTarget & io_target, ecmdTargetDepth_t i_depth)
{
  uint32_t rc = ECMD_SUCCESS;

  // Set Target Depth based on i_depth input
  switch (i_depth)
  {
    case ECMD_DEPTH_CAGE:
      io_target.cageState = ECMD_TARGET_FIELD_VALID;
      io_target.nodeState = io_target.slotState = io_target.chipTypeState = io_target.posState = io_target.chipUnitTypeState = io_target.chipUnitNumState = io_target.threadState = ECMD_TARGET_FIELD_UNUSED;
      break;

    case ECMD_DEPTH_NODE:
      io_target.cageState = io_target.nodeState = ECMD_TARGET_FIELD_VALID;
      io_target.slotState = io_target.chipTypeState = io_target.posState = io_target.chipUnitTypeState = io_target.chipUnitNumState = io_target.threadState = ECMD_TARGET_FIELD_UNUSED;
      break;

    case ECMD_DEPTH_SLOT:
      io_target.cageState = io_target.nodeState = io_target.slotState = ECMD_TARGET_FIELD_VALID;
      io_target.chipTypeState = io_target.posState = io_target.chipUnitTypeState = io_target.chipUnitNumState = io_target.threadState = ECMD_TARGET_FIELD_UNUSED;
      break;

    case ECMD_DEPTH_CHIP:
      io_target.cageState = io_target.nodeState = io_target.slotState = io_target.chipTypeState = io_target.posState = ECMD_TARGET_FIELD_VALID;
      io_target.chipUnitTypeState = io_target.chipUnitNumState = io_target.threadState = ECMD_TARGET_FIELD_UNUSED;
      break;

    case ECMD_DEPTH_CORE:
      io_target.cageState = io_target.nodeState = io_target.slotState = io_target.chipTypeState = io_target.posState =  io_target.chipUnitNumState = ECMD_TARGET_FIELD_VALID;
      io_target.chipUnitTypeState = io_target.threadState = ECMD_TARGET_FIELD_UNUSED;
      break;

    case ECMD_DEPTH_CHIPUNIT:
      io_target.cageState = io_target.nodeState = io_target.slotState = io_target.chipTypeState = io_target.posState = io_target.chipUnitTypeState = io_target.chipUnitNumState = ECMD_TARGET_FIELD_VALID;
      io_target.threadState = ECMD_TARGET_FIELD_UNUSED;
      break;

    case ECMD_DEPTH_THREAD:
      io_target.cageState = io_target.nodeState = io_target.slotState = io_target.chipTypeState = io_target.posState = io_target.chipUnitTypeState = io_target.chipUnitNumState = io_target.threadState = ECMD_TARGET_FIELD_VALID;
      break;


    default: // Used an unknown ecmdTargetDepth_t enum value
      rc = ECMD_INVALID_ARGS;
      break;
  }

  return rc;
}



uint32_t ecmdReadTarget(std::string i_targetStr, ecmdChipTarget & o_target) {

  uint32_t rc = ECMD_SUCCESS;
  std::vector<std::string> tokens;
  bool allFound;
  bool naFound;
  bool numFound;
  int num=0;        //fix beam error

  /* Tokenize my string on colon */
  ecmdParseTokens(i_targetStr, ":", tokens);

  for (uint32_t x = 0; x < tokens.size(); x++) {
    allFound = false;
    naFound = false;
    numFound = false;
    if (tokens[x].substr(1,tokens[x].length()) == "all") {
      allFound = true;
    } else if (tokens[x].substr(1,tokens[x].length()) == "-") {
      naFound = true;
    }
    // If it is all numbers after the first letter, assume it is a k/c/s/p/c/t arg and not a chip name
    // We will have problems with something like p7, but if they use pu we will be okay.
    // This will fix p5ioc2, which was broke under the previous sscanf only method - JTA 04/28/08
    else if (tokens[x].find_first_not_of("0123456789",1) == std::string::npos) {
      sscanf(tokens[x].substr(1,tokens[x].length()).c_str(), "%d", &num);
      numFound = true;
    }
    if (!numFound && !allFound && !naFound) {
      /* I didn't get a number, - or all, must be a chip */
      ecmdParseChipField(tokens[x], o_target.chipType, o_target.chipUnitType);
      if (o_target.chipType == "chipall"){
        o_target.chipType = ""; /* Blank it for clarity */
        o_target.chipTypeState = ECMD_TARGET_FIELD_WILDCARD;
      } else {
        o_target.chipTypeState = ECMD_TARGET_FIELD_VALID;
        if (o_target.chipUnitType != "") {
          o_target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
        } else {
          o_target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
        }
      }
    } else {    
      if (tokens[x].substr(0, 1) == "k") {
        if (allFound) {
          o_target.cageState = ECMD_TARGET_FIELD_WILDCARD;
        } else {
          o_target.cage = num;
          o_target.cageState = ECMD_TARGET_FIELD_VALID;
        } 
      } else if (tokens[x].substr(0, 1) == "n") {
        if (allFound) {
          o_target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
        } else if (naFound) {
          o_target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
        } else {
          if (naFound) {
            o_target.node = ECMD_TARGETDEPTH_NA;
          } else {
            o_target.node = num;
          }
          o_target.nodeState = ECMD_TARGET_FIELD_VALID;
        } 
      } else if (tokens[x].substr(0, 1) == "s") {
        if (allFound) {
          o_target.slotState = ECMD_TARGET_FIELD_WILDCARD;
        } else {
          if (naFound) {
            o_target.slot = ECMD_TARGETDEPTH_NA;
          } else {
            o_target.slot = num;
          }
          o_target.slotState = ECMD_TARGET_FIELD_VALID;
        } 
      } else if (tokens[x].substr(0, 1) == "p") {
        if (allFound) {
          o_target.posState = ECMD_TARGET_FIELD_WILDCARD;
        } else {
          o_target.pos = num;
          o_target.posState = ECMD_TARGET_FIELD_VALID;
        } 
      } else if (tokens[x].substr(0, 1) == "c") {
        if (allFound) {
          o_target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
        } else {
          o_target.chipUnitNum = num;
          o_target.chipUnitNumState = ECMD_TARGET_FIELD_VALID;
        } 
      } else if (tokens[x].substr(0, 1) == "t") {
        if (allFound) {
          o_target.threadState = ECMD_TARGET_FIELD_WILDCARD;
        } else {
          o_target.thread = num;
          o_target.threadState = ECMD_TARGET_FIELD_VALID;
        } 
      } else {
        return ECMD_INVALID_ARGS;
      }
    }
  }

  return rc;
}


std::string ecmdWriteTarget(ecmdChipTarget & i_target, ecmdTargetDisplayMode_t i_displayMode) {

  std::string printed;
  char util[10];
  bool hexMode = false;
  std::string subPrinted;

  if (i_displayMode == ECMD_DISPLAY_TARGET_PLUGIN_MODE) {
    i_displayMode = pluginDisplayMode;    
  }

  if (i_displayMode == ECMD_DISPLAY_TARGET_HEX_DEFAULT || i_displayMode == ECMD_DISPLAY_TARGET_HEX_COMPRESSED || i_displayMode == ECMD_DISPLAY_TARGET_HEX_HYBRID) {
    hexMode = true;
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

  /* Put the hex prefix onto the output */
  if (hexMode) {
    subPrinted += "0x[";
  }

  if (i_target.cageState == ECMD_TARGET_FIELD_VALID) {
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

    if (i_target.nodeState == ECMD_TARGET_FIELD_VALID) {
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

      if (i_target.slotState == ECMD_TARGET_FIELD_VALID) {
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

        if ((i_target.posState == ECMD_TARGET_FIELD_VALID) && (i_target.chipTypeState == ECMD_TARGET_FIELD_VALID)) {

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

          if (i_target.chipUnitNumState == ECMD_TARGET_FIELD_VALID) {
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

            if (i_target.threadState == ECMD_TARGET_FIELD_VALID) {
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

  /* Now put the subPrinted stuff onto the printed string so we've got the full thing */
  printed += subPrinted;

  /* For the default display modes, there are a couple extra things we want to do */
  if (i_displayMode == ECMD_DISPLAY_TARGET_DEFAULT || i_displayMode == ECMD_DISPLAY_TARGET_HEX_DEFAULT) {
    /* If the generated string is was shorter than 18 characters, pad output with whitespace */
    if (subPrinted.length() < 18) {
      sprintf(util, "%*s", (18 - subPrinted.length()) , "");
      printed += util;
    }

    //ensure there is a space between the target info and the data
    printed += " "; 
  }

  return printed;
}

uint32_t ecmdParseChipField(std::string i_chipField, std::string &o_chipType, std::string &o_chipUnitType) {
  uint32_t rc = ECMD_SUCCESS;

  /* See if the chipUnit separator (the period) is found.  If it is, then break up the input field.
   if it is not, then just return the chipType */
  uint32_t linePos = i_chipField.find(".");

  if (linePos == std::string::npos) {
    o_chipType = i_chipField;
    o_chipUnitType = "";
  } else {
    o_chipType = i_chipField.substr(0, linePos);
    o_chipUnitType = i_chipField.substr((linePos+1), i_chipField.length());
  }

  return rc;
}


uint32_t ecmdReadDcard(const char *i_filename, std::list<ecmdMemoryEntry> &o_data) {
  std::ifstream ins;
  std::string line;
  std::vector<std::string> splitArgs;
  int rc = 0;
  uint32_t databitlength=0, tagbitlength=0, bitlength=0;
  uint64_t curaddress, nextaddress=0;
  uint32_t tagsBitOffset =0, startOffset =0;
  bool isFirstTimeInLoop = true, setTag = true;
  std::list<ecmdMemoryEntry> memdata;        //ListElements corresponding to every line in file
  std::list<ecmdMemoryEntry>::iterator memdataIter;
  std::list<ecmdMemoryEntry>::iterator o_dataIter;
  ecmdMemoryEntry dcardEntry;
  
  ins.open(i_filename);
  
  if (ins.fail()) {
    return ECMD_DBUF_FOPEN_FAIL;  
  }
  
  //Loop to ealk thru the file and set the Data databuffers bitlengths
  while (!ins.eof()) {
   ecmdMemoryEntry dcardEntryForEveryLine;

   getline(ins, line, '\n');
   ecmdParseTokens(line, " \t\n", splitArgs);
 
   if (splitArgs.size() == 0) continue;
   if ((splitArgs.size() < 3) || (splitArgs.size() > 4)) {
     continue;
   }
   
   //Setup the dcardEntryForEveryLine 
   curaddress = strtoull(splitArgs[1].c_str(), NULL, 16);
   dcardEntryForEveryLine.address = curaddress;

   bitlength = splitArgs[2].length() * 4;
   dcardEntryForEveryLine.data.setBitLength(bitlength);
   rc = dcardEntryForEveryLine.data.insertFromHexLeft(splitArgs[2].c_str(), 0, bitlength);
   if (rc) return rc;

   if (splitArgs.size() == 4) {
    dcardEntryForEveryLine.tags.setBitLength(1);
    if (splitArgs[3] == "1") {
      dcardEntryForEveryLine.tags.setBit(0);
    } else if (splitArgs[3] == "0") {
      dcardEntryForEveryLine.tags.clearBit(0);
    } else {
      return ECMD_INVALID_ARGS; //tag neither 0 nor 1
    }
   } else { dcardEntryForEveryLine.tags.setBitLength(0); }
   
   memdata.push_back(dcardEntryForEveryLine);

   //Combine the lines if contiguous address
   if (isFirstTimeInLoop) {
     isFirstTimeInLoop = false;
     dcardEntry.address = curaddress;
   } else {
     if (curaddress != nextaddress) {  
       //found a hole, so setup the buffer with bitlength so far 
       dcardEntry.data.setBitLength(databitlength);
       dcardEntry.tags.setBitLength(tagbitlength);
       o_data.push_back(dcardEntry);
       
       //Set it up for the next data buffer
       dcardEntry.address = curaddress;
       databitlength = 0;
       tagbitlength = 0;
       setTag = true;
     } 
   }
   //If data bit length is not 64 OR tags are not present in the line then
   //set the tag buffer bitlength to 0 and ignore the tags for that buffer
   //For every 64 bits of data there's 1 tag bit
   if ((bitlength != 64) || (dcardEntryForEveryLine.tags.getBitLength() == 0) ) {
     tagbitlength = 0;
     setTag = false;
   }
   databitlength += bitlength;
   if (setTag) tagbitlength++;
   
   nextaddress = dcardEntry.address + databitlength/8;
  }
  //Set the last one from the loop
  dcardEntry.data.setBitLength(databitlength);
  dcardEntry.tags.setBitLength(tagbitlength);
  o_data.push_back(dcardEntry);
  
  ins.close();

  //Loop to set the buffers in the list
  o_dataIter = o_data.begin();
  for (memdataIter = memdata.begin(); memdataIter != memdata.end(); memdataIter++) {
   
   if (startOffset >= o_dataIter->data.getBitLength()) {
    startOffset = 0;
    tagsBitOffset = 0;
    if (o_dataIter != o_data.end()) {
      o_dataIter++;
    } else { break; }
   }
   memdataIter->data.extractPreserve(o_dataIter->data, 0,  memdataIter->data.getBitLength(), startOffset);
   startOffset += memdataIter->data.getBitLength();
   
   if (o_dataIter->tags.getBitLength() != 0) {
     memdataIter->tags.extractPreserve(o_dataIter->tags, 0,  memdataIter->tags.getBitLength(), tagsBitOffset);
     tagsBitOffset++;
   }
   
  }
  
  
  return rc;
}

uint32_t ecmdGenB32FromHex (uint32_t * o_numPtr, const char * i_hexChars, int startPos) {

  uint32_t tempB32 = 0;
  uint32_t counter = 0;
  char	twoChars[2] = "0";

  if ((o_numPtr == NULL) || (i_hexChars == NULL) || (strlen(i_hexChars) == 0))
    return 0xFFFFFFFF;

  o_numPtr[0] = 0x0;
  o_numPtr[startPos>>3]= 0x0;

  for (counter = 0; counter < strlen(i_hexChars); counter++) {
    if (((counter + startPos) & 0xFFF8) == (counter + startPos))
      o_numPtr[(counter+startPos)>>3] = 0x0;
    twoChars[0] = i_hexChars[counter];
    tempB32 = strtoul((char *) & twoChars[0], NULL, 16);
    o_numPtr[(counter+startPos)>>3] |= (tempB32 << (28 - (4 * ((counter + startPos) & 0x07))));
  }

  return o_numPtr[0];

}


uint32_t ecmdGenB32FromHexLeft (uint32_t * o_numPtr, const char * i_hexChars) {

  if ((i_hexChars == NULL) || (o_numPtr == NULL))
    return 0xFFFFFFFF;

  return ecmdGenB32FromHex(o_numPtr, i_hexChars, 0);

}

uint32_t ecmdGenB32FromHexRight (uint32_t * o_numPtr, const char * i_hexChars, int expectBits) {

  int stringSize;

  if ((i_hexChars == NULL) || (o_numPtr == NULL))
    return 0xFFFFFFFF;

  /* ----------------------------------------------------------------- */	
  /* Telling us we are expecting few bits than the user input          */	
  /* ----------------------------------------------------------------- */	
  stringSize = strlen(i_hexChars);

  if (expectBits==0) 
    expectBits = ((stringSize << 2) + 31) & 0xFFFE0;

  /* ----------------------------------------------------------------- */
  /* Can't figure out if this case is bad...                           */
  /* ----------------------------------------------------------------- */
  if (expectBits<(stringSize*4)) 
    expectBits = (4-(expectBits & 0x3))+expectBits+(stringSize*4-expectBits&0xFFFC);

  expectBits = (expectBits - stringSize*4)>>2; 

  return ecmdGenB32FromHex(o_numPtr, i_hexChars, expectBits );


}

