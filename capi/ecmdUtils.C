// Copyright **********************************************************
//                                                                      
// File ecmdDataBuffer.C                                               
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
// End Copyright ******************************************************

/* $Header$ */

/**
 * @file ecmdUtils.C
 * @brief Useful functions for use throughout the ecmd C API
 *
 */


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <inttypes.h>
#include <ecmdUtils.H>

#include <string.h>
#include <stdlib.h>

//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------

//--------------------------------------------------------------------
// Macros
//--------------------------------------------------------------------
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

//----------------------------------------------------------------------
//  User Types
//----------------------------------------------------------------------

uint32_t ecmdGenB32FromHex (uint32_t * o_numPtr, const char * i_hexChars, int startPos) {

  uint32_t tempB32 = 0;
  int	counter = 0;
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


int ecmdParseOption (int *argc, char **argv[], const char *option) {
  int counter = 0;
  int foundit = 0;

  for (counter = 0; counter < *argc ; counter++) {
    if (((*argv)[counter] != NULL) && (strncmp((*argv)[counter],option,strlen(option))==0)) {
      (*argv)[counter]=NULL;
      foundit = 1;
      counter = *argc;
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
    }
  }

  ecmdRemoveNullPointers(argc, argv);

  return returnValue;
}

