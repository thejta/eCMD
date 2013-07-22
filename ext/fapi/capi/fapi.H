/**
 *  @file fapi.H
 *
 *  @brief Includes all the header files necessary for the FAPI interface.
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 *                          mjjones     04/13/2011  Created.
 *                          mjjones     06/07/2011  Added new includes.
 *                          mjjones     08/08/2011  Updated Executor include
 *                          mjjones     09/23/2011  Added fapiHwpErrorInfo.H
 *                          mjjones     11/10/2011  Use ecmdDataBufferBase
 *                          mjjones     04/16/2012  Added fapiChipEcFeature.H
 *                          mjjones     11/14/2012  Added fapiSetSbeError.H
 */

#ifndef FAPI_H_
#define FAPI_H_

#include <fapiTarget.H>
#include <fapiReturnCode.H>
#include <fapiUtil.H>
#include <fapiHwAccess.H>
#include <fapiSystemConfig.H>
#include <fapiPlatTrace.H>
#include <fapiHwpExecutor.H>
#include <fapiAttributeService.H>
#include <fapiChipEcFeature.H>
#include <fapiHwpReturnCodes.H> // Generated file
#include <fapiHwpErrorInfo.H>   // Generated file
#include <fapiAttributeIds.H>   // Generated file
#include <fapiSetSbeError.H>    // Generated file
#include <ecmdDataBufferBase.H>
#include <fapiMvpdAccess.H>

#endif // FAPI_H_
