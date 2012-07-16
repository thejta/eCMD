/**
 *  @file platReturnCodeDataRef.C
 *
 *  @brief Implements the platform part of the ReturnCodeDataRef class.
 *
 *  Note that platform code must provide the implementation. FAPI has provided
 *  an example for platforms that do not attach ReturnCodeData to a ReturnCode.
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 *                          mjjones     04/13/2011  Created.
 *                          camvanng	05/31/2011	Added debug trace
 *                          mjjones     07/05/2011. Removed const from data
 *                          mjjones     08/11/2011  class name change
 *                          mjjones     09/26/2011  Another func name update
 */



#include <fapiReturnCodeDataRef.H>

/* *****************************************************************************/
/* Note:                                                                       */
/* I manually added these macros, added the stdio.h include, and commented out */
/* the fapiPlatTrace.H include because the macros that are defined in          */
/* fapiPlatTrace.H call the fapiOutput*() APIs.  These APIs are in the archive */
/* and this file is in the shared lib and therefore causes undefined symbols in*/
/* the shared lib (ie libfapi.so)                                              */
/* -farrugia 08.25.2011                                                        */
/* *****************************************************************************/
#include <stdio.h>
//#include <fapiPlatTrace.H>
#define FAPI_DBG(_fmt_, _args_...) printf("FAPI DBG>: "_fmt_"\n", ##_args_)

 
namespace fapi
{

//******************************************************************************
// deleteData function
//******************************************************************************
void ReturnCodeDataRef::deletePlatData()
{
    //JFDEBUG 	FAPI_DBG("ReturnCodeDataRef::deletePlatData");

    // If platform does not attach ReturnCodeData to a ReturnCode then it can
    // do nothing (this function will never be called), but if it does then it
    // must be deleted with the correct type.

   
    // In the eCMD fapi extention we'll do nothing for now - farrugia
    //    delete (reinterpret_cast<TYPE *>(const_cast<void *>(iv_pData)));
}

}
