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

// Module Description **************************************************
//
// Description:
//
// End Module Description **********************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define ecmdSharedUtils_C
#include <string>
#include <vector>
#include <stdio.h>

#include <ecmdSharedUtils.H>


#undef ecmdSharedUtils_C
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
  int foundit = 0;
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

  int curStart = 0, curEnd = 0;

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
    switch(len)  /* all the case statements fall through */
    {
    case 11: c+=((uint32_t)k[10]<<24);
    case 10: c+=((uint32_t)k[9]<<16);
    case 9 : c+=((uint32_t)k[8]<<8);
        /* the first byte of c is reserved for the length */
    case 8 : b+=((uint32_t)k[7]<<24);
    case 7 : b+=((uint32_t)k[6]<<16);
    case 6 : b+=((uint32_t)k[5]<<8);
    case 5 : b+=k[4];
    case 4 : a+=((uint32_t)k[3]<<24);
    case 3 : a+=((uint32_t)k[2]<<16);
    case 2 : a+=((uint32_t)k[1]<<8);
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

// Change Log *********************************************************
//
//  Flag Reason   Vers Date     Coder    Description
//  ---- -------- ---- -------- -------- ------------------------------
//                              CENGEL   Initial Creation
//                     10/07/04 kloss    added the hashfunction ecmdHexToUInt32 and ecmdHashString32
//
// End Change Log *****************************************************
