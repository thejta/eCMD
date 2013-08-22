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
#include <unistd.h>
#include <unistd.h>
#include <termios.h>
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
int getch(void);
void setch(void);
void resetch(void);
struct termios oldt;

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------
#define MAX_NUM_ECMDS 250
#define MAX_CMD_LEN 80
#define MAX_CMD_HISTORY_LEN 10
char cmdHistory[MAX_CMD_HISTORY_LEN][MAX_CMD_LEN];
int  curIndex = 0; // points to the next write position
int  getIndex = 0; // points to the selected cmd
int  curHistorySize = 0; // counts the number of valid history entries

std::string allecmds[MAX_NUM_ECMDS];


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
  if ((i_data.getBitLength()-1)/4 > (i_expected.getBitLength()-1)/4) {
    ecmdOutputError("ecmdCheckExpected - Not enough expect data provided\n");
    o_mismatchBit = ECMD_UNSET;
    return 0;
  } else if ((i_data.getBitLength()-1)/4 < (i_expected.getBitLength()-1)/4) {
    ecmdOutputError("ecmdCheckExpected - More expect data provided than data that was retrieved\n");
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

uint32_t ecmdApplyDataModifier(ecmdDataBuffer & io_data, ecmdDataBuffer & i_newData, uint32_t i_startBit, std::string i_modifier, ecmdEndianMode_t i_endianMode) {
  uint32_t rc = ECMD_SUCCESS;
  uint32_t length;

  if (i_endianMode == ECMD_LITTLE_ENDIAN) {
    if (i_startBit < i_newData.getBitLength()) { 
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


/* STGC01276442 - MKL 03/04/10 */
uint32_t ecmdCreateDataMaskModifier(ecmdDataBuffer & o_data, ecmdDataBuffer & o_mask, ecmdDataBuffer & i_newData, uint32_t i_startBit, std::string i_modifier, ecmdEndianMode_t i_endianMode) {
  uint32_t rc = ECMD_SUCCESS;
  uint32_t length;

  if (i_endianMode == ECMD_LITTLE_ENDIAN) {
    if (i_startBit < i_newData.getBitLength()) { 
      length = o_data.getBitLength() - i_startBit;
      /* If there are bits on past the length, let's error.  Otherwise, let it through */
      if (i_newData.isBitSet((i_startBit + length), (i_newData.getBitLength() - (i_startBit+length)))) {
        char buf[200];	     
        sprintf(buf,"ecmdApplyDataModifier - There are bits set past on the input data(%d) past the length of the destination data(%d)!\n", i_newData.getBitLength(), o_data.getBitLength());
        ecmdOutput(buf);
        return ECMD_INVALID_ARGS;
      }
    } else {
      length = i_newData.getBitLength();
    }

    uint32_t leStartBit;

    if (o_data.getBitLength() == i_newData.getBitLength()) {
      leStartBit = i_startBit;
    } else {
      leStartBit = o_data.getBitLength() - 1 - i_startBit;  
    }

    if (i_modifier == "insert") {
      o_data.flushTo0();
      o_mask.flushTo0();
      o_data.insert(i_newData, leStartBit, length);
      o_mask.setBit(leStartBit, length);
    } else if (i_modifier == "and") {
      o_data.flushTo0();
      o_mask.flushTo0();
      o_mask.insert(i_newData, leStartBit, length);
      o_mask.flipBit(leStartBit, length);
    } else if (i_modifier == "or") {
      o_data.flushTo0();
      o_mask.flushTo0();
      o_data.insert(i_newData, leStartBit, length);
      o_mask.insert(i_newData, leStartBit, length);
    } else {
      ecmdOutputError(("ecmdApplyDataModifier - Invalid Data Modifier specified with -b arg : " + i_modifier + "\n").c_str());
      return ECMD_INVALID_ARGS;
    }
  } else {
    /* We no longer just error out on length problems, let's try and be smart about it */
    /* STGC00108031 - JTA 01/31/07 */
    if ((i_startBit + i_newData.getBitLength()) > o_data.getBitLength()) {
      length = o_data.getBitLength() - i_startBit;
      /* If there are bits on past the length, let's error.  Otherwise, let it through */
      if (i_newData.isBitSet((i_startBit + length), (i_newData.getBitLength() - (i_startBit+length)))) {
        char buf[200];	     
        sprintf(buf,"ecmdApplyDataModifier - There are bits set past on the input data(%d) past the length of the destination data(%d)!\n", i_newData.getBitLength(), o_data.getBitLength());
        ecmdOutput(buf);
        return ECMD_INVALID_ARGS;
      }
    } else {
      length = i_newData.getBitLength();
    }

    if (i_modifier == "insert") {
      o_data.flushTo0();
      o_mask.flushTo0();
      o_data.insert(i_newData, i_startBit, length);
      o_mask.setBit(i_startBit, length);
    } else if (i_modifier == "and") {
      o_data.flushTo0();
      o_mask.flushTo0();
      o_mask.insert(i_newData, i_startBit, length);
      o_mask.flipBit(i_startBit, length);
    } else if (i_modifier == "or") {
      o_data.flushTo0();
      o_mask.flushTo0();
      o_data.insert(i_newData, i_startBit, length);
      o_mask.insert(i_newData, i_startBit, length);
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
  std::string l_version = "default";

  /* Get the path to the help text files */
  rc = ecmdQueryFileLocation(target, ECMD_FILE_HELPTEXT, file, l_version);
  if (rc) return rc;

  file += i_command; file += ".htxt";

  /* Let's go open this guy */
  ins.open(file.c_str());
  if (ins.fail()) {
    ecmdOutputError(("Error occured opening help text file: " + file + "\n").c_str());
    return ECMD_UNKNOWN_HELP_FILE;  
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
uint32_t ecmdParseStdinCommands(std::vector< std::string > & o_commands)
{
  std::string buffer;

  o_commands.clear();

  if (!std::cin) {
    /* We have reached EOF */
    return ECMD_SUCCESS;
  } else {
    if(getline(std::cin, buffer))
    {
      /* Carve up what we have here */
      ecmdParseTokens(buffer,"\n;", o_commands);
    }
    else
    {
      /* We have reached EOF */
      return ECMD_SUCCESS;
    }
  }
  return 1;
}


uint32_t ecmdParseShellCommands(std::vector< std::string > & o_commands)
{
  std::string buffer;

  uint32_t x;

  int cp = 0; // cursorpos
  o_commands.clear();

  if (!std::cin)
  {
    /* We have reached EOF */
    return ECMD_SUCCESS;
  }
  else
  {
    setch();

    buffer.clear();
    x = getch();
    if (x==0xFFFFFFFF) {

      resetch();

      return ECMD_SUCCESS;

    }
    if (x=='$')    // test mode
    {

      printf(" Test Mode entered \n");
      do
      {
        x = getch();
        printf(".. got key %u \n",x);


      } while (x !='$');
      printf(" Test Mode exited \n");
    }
    else 
    {
      cp = 0; // cursorpos
      do
      {
        if (x==27)  //esc
        {
          x = getch(); // get next esc seq II
          x = getch(); // get next cmdr

          if (x==68)    // ****left**** 
          {
            if (cp>0)
            {
              cp=cp-1;
              printf("\b");  // backspace
              x = getch(); // get next cmdr
              continue;
            }

          }
          if (x==67)   // ****right**** 
          {
            if (cp<(int)buffer.size())
            {
              printf("%c",27);
              printf("%c",91);
              printf("%c",x);
              cp=cp+1;
              x = getch(); // get next cmdr
              continue;
            }
          }
          if (x==65)   // ****  up  **** 
          {


            if (getIndex == 0)
            {
              if (curHistorySize>0) getIndex=curHistorySize-1;
            }
            else
              getIndex = getIndex -1;

                // erase current buffer contents
            for (int s=0; s<cp; s++)
            {
              printf("\b \b");
            }
            printf("%s", cmdHistory[getIndex]);
            buffer = cmdHistory[getIndex];
            cp = buffer.size();

            x = getch(); // get next cmdr
            continue;

          }
          if (x==66)   // **** down **** 
          {


            getIndex = getIndex+1;
            if (getIndex >= curHistorySize)
            {
              getIndex=0;
            }

              // erase current input and buffer
            for (int s=0; s<cp; s++)
            {
              printf("\b \b");
            }
            printf("%s", cmdHistory[getIndex]);
            //cp = strlen(cmdHistory[getIndex]);
            buffer = cmdHistory[getIndex];
            cp = buffer.size();

            x = getch(); // get next cmdr
            continue;
          }
          if (x==70)   //**** end ****
          {
            for (int s= cp; s< (int)buffer.size();s++)
            {   // move cursor  right
              printf("%c",27);
              printf("%c",91);
              printf("%c",67);
            }
            cp=buffer.size();
            x = getch(); // get next cmdr
            continue;
          }
          if (x==72)   // ****pos 1**** 
          {
            printf("\r");  
            printf("%s",getEcmdPrompt().c_str());
            cp=0;
            x = getch(); // get next cmdr
            continue;
          }

          if (x==51) //  ****delete key****
          {
            x = getch(); // get next cmdr  126

            buffer.erase(cp,1);

            printf("\b ");   // delte last char
            printf("\r");   // pos 1
            printf("%s%s ",getEcmdPrompt().c_str(),buffer.c_str()); // write prompt + buffer
            if (buffer.size()> (unsigned)cp)   // delete in middle of string
            {
              for (int rp=0; rp<(int)buffer.size()-cp;rp++)
              {
                printf("\b");  // backspace
              }
            }
            printf("\b");  // backspace
          }
        }
        else if (x==127)  //   ***backspace***
        {
          if (cp>0)
          {
            cp=cp-1;
            buffer.erase(cp,1);

            printf("\b ");   // delte last char
            printf("\r");   // pos 1
            printf("%s%s ",getEcmdPrompt().c_str(),buffer.c_str()); // write prompt + buffer
            if (buffer.size()> (unsigned)cp)   // delete in middle of string
            {
              for (int rp=0; rp<(int)buffer.size()-cp;rp++)
              {
                printf("\b");  // backspace
              }
            }
            printf("\b");  // backspace
          }
        }
        else if (x==9)  //   ***tab***
        {
           
           buffer = getBestEcmd(buffer.substr(0,cp));
           printf("\r");   // pos 1
           printf("%s%s ",getEcmdPrompt().c_str(),buffer.c_str()); // write prompt + buffer
           if ((int)buffer.size()<cp)
           {
             for (int s=0; s<cp-(int)buffer.size(); s++)
             {
               printf("\b \b");
             }
           }
           else 
           {
             for (int s=cp; s<(int)buffer.size()+1; s++)
             {
               printf("\b");
             }
           }

           //cp=buffer.size();
          
        }
        else if (((x >= 48) && (x <= 57))||((x >= 97) && (x <= 122)) ||( (x>= 65) && (x<= 90))|| (x==32) || (x==124)|| (x==62)|| (x==42)||(x==47)||(x==35)||(x==34)|| (x==95) || (x==46)|| (x==45)) // alphanumeric values
        {
          if (buffer.size()>(unsigned)cp)   // always insert mode
          {

            printf("%c",x);  //print character
            printf("%s",buffer.substr(cp).c_str());  // print rest of string
            buffer.insert(cp,1,(char)x);

            for (int s= cp; s< ((int)buffer.size()-1) ;s++)
            {   // move cursor  left
              printf("\b");  // backspace
            }
          }
          else
          {
            buffer = buffer + (char *)&x;
            printf("%c",x);
          }

          cp=cp+1;
          // insert start
          //insert end
        }


        x = getch(); // get next char
      } while (x != 10);


      printf("\n");  // newline
       // first check whether previous cmd was the same
      if (curIndex>0)
      {
        if (strcmp(buffer.c_str(),cmdHistory[curIndex-1]))
        {  // not the smae .. so save it in history
          (void)memset(cmdHistory[curIndex],0x00,MAX_CMD_LEN);  
          (void)memcpy(cmdHistory[curIndex],buffer.c_str(),buffer.size());
          curIndex = curIndex+1;
          if (curHistorySize < MAX_CMD_HISTORY_LEN) curHistorySize=curHistorySize+1;
          else  curHistorySize = MAX_CMD_HISTORY_LEN;
          if (curIndex >= MAX_CMD_HISTORY_LEN)
          {
            curIndex=0;
          }
          getIndex=curIndex;
        }
      }
      else
      {
        if (strcmp(buffer.c_str(),cmdHistory[curHistorySize]))
        {  // not the smae .. so save it in history
          (void)memset(cmdHistory[curIndex],0x00,MAX_CMD_LEN);  
          (void)memcpy(cmdHistory[curIndex],buffer.c_str(),buffer.size());
          curIndex = curIndex+1;
          if (curHistorySize < MAX_CMD_HISTORY_LEN) curHistorySize=curHistorySize+1;
          else  curHistorySize = MAX_CMD_HISTORY_LEN;
          if (curIndex >= MAX_CMD_HISTORY_LEN)
          {
            curIndex=0;
          }
          getIndex=curIndex;
        }
      }
    }

    std::cout << buffer.c_str() ;
    ecmdParseTokens(buffer,"\n;", o_commands);

    resetch();
  }
  return 1;
}

uint32_t ecmdParseTargetFields(int *argc, char ** argv[],const char *targetField, ecmdChipTarget &target, uint8_t &targetFieldType, std::string &targetFieldList) {
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
   // Look for ':' in target field, which isn't supported for this function at this time
   else if (targetFieldList.find_first_of(":") < targetFieldList.length()) {
     printed = "ecmdParseTargetFields - target fields with ':' are not supported by the calling function\n";
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
         if (!targetFieldList.compare(0,2,"0x")) //input is hex
           target.cage = (uint32_t)strtol(targetFieldList.c_str(),NULL,16);
         else  // dec
           target.cage = (uint32_t)atoi(targetFieldList.c_str());
       }
       else if(isNode) {
         if (!targetFieldList.compare(0,2,"0x")) //input is hex
           target.node = (uint32_t)strtol(targetFieldList.c_str(),NULL,16);
         else  // dec
           target.node = (uint32_t)atoi(targetFieldList.c_str());
       }
       else if(isSlot) {
         if (!targetFieldList.compare(0,2,"0x")) //input is hex
           target.slot = (uint32_t)strtol(targetFieldList.c_str(),NULL,16);
         else  // dec
           target.slot = (uint32_t)atoi(targetFieldList.c_str());
       }
       else if(isPos) {
         if (!targetFieldList.compare(0,2,"0x")) //input is hex
           target.pos = (uint32_t)strtol(targetFieldList.c_str(),NULL,16);
         else  // dec
           target.pos = (uint32_t)atoi(targetFieldList.c_str());
       }
       else if(isCore) {
         if (!targetFieldList.compare(0,2,"0x")) //input is hex
           target.core = (uint32_t)strtol(targetFieldList.c_str(),NULL,16);
         else  // dec
           target.core = (uint8_t)atoi(targetFieldList.c_str());
       }
       else if(isThread) {

         if (!targetFieldList.compare(0,2,"0x")) //input is hex
           target.thread = (uint32_t)strtol(targetFieldList.c_str(),NULL,16);
         else  // dec
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
  if (str[0]=='0' && (str[1]=='x' ||str[1]=='X' )) // hex
  {
    for (uint32_t x = 0; x < str.length(); x ++) {
      if (isxdigit(str[x])) {
      } else if (str[x] == ',') {
      } else if (str[x] == 'x') {
      } else if (str[x] == '.' && str[x+1] == '.') {
        x++;
      } else {
        ret = false;
        break;
      }
    }
  }
  else // dec
  {
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

void setch(void)
{
 struct termios newt;

 tcgetattr(STDIN_FILENO, &oldt); /*store old settings */
 newt = oldt; /* copy old settings to new settings */
 newt.c_lflag &= ~(ICANON | ECHO); /* make one change to old settings in new settings */
 tcsetattr(STDIN_FILENO, TCSANOW, &newt); /*apply the new settings immediatly */
 return ;
}

void resetch(void)
{
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt); /*reapply the old settings */
  return;
}

int getch(void)
{

  int ch = 0;
  ch = getchar(); /* standard getchar call */
  return ch; /*return received char */
}


std::string getEcmdPrompt() {
  //char dirBuffer[80];
  //getcwd(dirBuffer,80);
  //prompt = dirBuffer;
  //prompt = prompt + ">>eCMD>";
  //return prompt;

  return "ecmd> ";
}

void setupEcmds(void)
{

  std::ifstream ecmdDefFile;
  ecmdDefFile.open("/console/data/ecmdDefFile.txt");
  int i=0;
  if (ecmdDefFile.is_open())
  {
    i=0;
    getline (ecmdDefFile,allecmds[i]);
    while (! ecmdDefFile.eof() )
    {
      allecmds[i].resize(allecmds[i].size()-1,00);
      i++;
      getline (ecmdDefFile,allecmds[i]);
    }
    ecmdDefFile.close();
  }
  else {
    ecmdOutput( "Unable to open ecmd definition file"); 

    allecmds[0]="ecmdquery -h";
    allecmds[1]="ecmdquery chips -all";
    allecmds[2]="ecmdquery chips -k1 -n0 -s0";
    allecmds[3]="ecmdquery showconfig";
    allecmds[4]="ecmdquery showexist";
    allecmds[5]="ecmdquery rings pu -all";
    allecmds[6]="ecmdquery rings pu NAME  -k1 -n0 -s0";
    allecmds[7]="ecmdquery scoms pu -all";
    allecmds[8]="ecmdquery scoms pu 24608 -k1 -n0 -s0";
    allecmds[9]="ecmdquery arrays pu NAME -k1 -n0 -s0";
    allecmds[10]="ecmdquery arrays pu -all";
    allecmds[11]="ecmdquery spys pu NAME  -k1 -n0 -s0";
    allecmds[12]="ecmdquery spys pu -all";
    allecmds[13]="ecmdquery version";
    allecmds[14]="ecmdquery formats";
    allecmds[15]="ecmdquery chipinfo pu -all";
    allecmds[16]="ecmdquery chipinfo pu -k1 -n0 -s0";
    allecmds[17]="ecmdquery configd pu -k1 -n0 -s0";
    allecmds[18]="ecmdquery configd -all";
    allecmds[19]="ecmdquery exist pu -k1 -n0 -s0";
    allecmds[20]="ecmdquery exist -all";
    i=21;
  }

  /*   prequery .. removed for now
  ----------------------------------------------
  std::list<ecmdRingData> ringDataList;
  ecmdRingData ringData;

  ecmdChipTarget queryTarget; 
  ecmdChipTarget chipTarget; 

  ecmdQueryData qData;

  ecmdCageData     cageData     ;
  ecmdNodeData     nodeData     ;
  ecmdSlotData     slotData     ;
  ecmdChipData     chipData     ;
  ecmdChipUnitData chipUnitData ;

  std::list<ecmdCageData>     cageList     ;
  std::list<ecmdNodeData>     nodeList     ;
  std::list<ecmdSlotData>     slotList     ;
  std::list<ecmdChipData>     chipList     ;
  std::list<ecmdChipUnitData> chipUnitList ;


  uint32_t src = 0;

  int cage = 0;
  int node = 0;
  int slot = 0;
  std::string chipType = "";
  int pos = 0;
  //int chipUnitNum = 0;
  std::string chipUnitType = "";



  chipTarget.cageState=ECMD_TARGET_FIELD_WILDCARD;
  chipTarget.nodeState=ECMD_TARGET_FIELD_WILDCARD;
  chipTarget.slotState=ECMD_TARGET_FIELD_WILDCARD;
  chipTarget.posState=ECMD_TARGET_FIELD_WILDCARD;
  chipTarget.chipTypeState=ECMD_TARGET_FIELD_VALID;
  chipTarget.chipUnitNumState=ECMD_TARGET_FIELD_UNUSED;
  chipTarget.chipUnitTypeState=ECMD_TARGET_FIELD_UNUSED;
  chipTarget.chipType = "pu";


  src=ecmdQueryConfig(chipTarget, qData, ECMD_QUERY_DETAIL_HIGH);
  //printf("qConfig done: src: %u \n",src );
  if (src==0)
  {
    cageList= qData.cageData;
    cageData = cageList.front();
    cage=cageData.cageId; 

    nodeList=  cageData.nodeData;
    nodeData=  nodeList.front();
    node= nodeData.nodeId; 

    slotList=  nodeData.slotData;
    slotData=  slotList.front();
    slot= slotData.slotId; 

    chipList=  slotData.chipData;
    chipData=  chipList.front();
    chipType= chipData.chipType; 
    pos= chipData.pos; 


    // now check for valid rings on first taqrget
    queryTarget.cageState=ECMD_TARGET_FIELD_VALID;
    queryTarget.nodeState=ECMD_TARGET_FIELD_VALID;
    queryTarget.slotState=ECMD_TARGET_FIELD_VALID;
    queryTarget.posState=ECMD_TARGET_FIELD_VALID;
    queryTarget.chipTypeState=ECMD_TARGET_FIELD_VALID;
    queryTarget.chipUnitTypeState=ECMD_TARGET_FIELD_UNUSED;
    queryTarget.chipUnitNumState=ECMD_TARGET_FIELD_UNUSED;

    queryTarget.cage = cage ;
    queryTarget.node = node ;
    queryTarget.slot = slot ;
    queryTarget.pos = pos ;

    queryTarget.chipType = chipType.c_str() ;

    src = ecmdQueryRing(queryTarget,ringDataList,NULL,ECMD_QUERY_DETAIL_HIGH);

    if (src==0)
    {
      char tempBuff[80];
      for (int s = 0;(s<(int)ringDataList.size()) && (s<100);s++)
      {
        ringData=ringDataList.front();
        ringDataList.pop_front();
        (void)memset(tempBuff,0x00,sizeof(tempBuff));
        //printf("ring name -%s-, ring type -%s- \n",ringData.ringNames.front().c_str(), ringData.relatedChipUnit.c_str());
        if (ringData.relatedChipUnit== (std::string)"")
        {

          sprintf(tempBuff,"getbits pu %s 0 %u -all",ringData.ringNames.front().c_str() , ringData.bitLength  );
        }
        else 
         sprintf(tempBuff,"getbits pu.%s %s 0 %u -all",ringData.relatedChipUnit.c_str(), ringData.ringNames.front().c_str(), ringData.bitLength   );
        allecmds[i+s]=(std::string)tempBuff;
      }
    }
    else 
      ecmdOutput( "Unable to get  ecmd ring informaton data"); 

  }
  else {  
    ecmdOutput( "Unable to get  ecmd configuration data"); 
  }
  -----------------------------------------------
   End prequery */
}


std::string  getBestEcmd(std::string i_ecmd)
{
  int i;
  for (i = 0; i< MAX_NUM_ECMDS; i++)
  {
    if (i_ecmd==allecmds[i].substr(0,i_ecmd.length()))
       return allecmds[i] ;
  }
  return i_ecmd ;
}
