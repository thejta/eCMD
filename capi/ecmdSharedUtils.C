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

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
