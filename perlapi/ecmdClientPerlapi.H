#ifndef _ecmdClientPerlapi_H
#define _ecmdClientPerlapi_H

/* File ecmdClientPerlapi.H created by Joshua Wills on Mon Nov 24 2003. */

// Copyright **********************************************************
//
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

//for Linux compiles
#ifdef list
#undef list
#endif

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <string>
#include <ecmdStructs.H>

extern "C" int ecmdPerlInterfaceErrorCheck (int ec);

class ecmdClientPerlapi
{
  public:

    //constructor, destructor
    ecmdClientPerlapi ();
    ~ecmdClientPerlapi ();

    int initDll(const char * i_dllName = NULL, const char * i_options = NULL);

    char * getScom(char * i_target, int i_address);

    int putScom(char * i_target, int i_address, char * i_data);

  private:
    int setupTarget(char * i_targetStr, ecmdChipTarget & o_target);

    ecmdChipTarget myTarget;
    std::string myFormat;

};

#endif /* _ecmdClientPerlapi_H */
