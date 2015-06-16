/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source$                                  */
/*                                                                        */
/* IBM CONFIDENTIAL                                                       */
/*                                                                        */
/* EKB Project                                                            */
/*                                                                        */
/* COPYRIGHT 2015                                                         */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* The source code for this program is not published or otherwise         */
/* divested of its trade secrets, irrespective of what has been           */
/* deposited with the U.S. Copyright Office.                              */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
///
/// @file fapi2.H
/// @brief top level header for fapi2
///

#ifndef __FAPI2_TOP_LEVEL__
#define __FAPI2_TOP_LEVEL__

#include <target.H>
#include <return_code.H>
#include <buffer.H>
#include <hw_access.H>
#include <utils.H>
#include <plat_trace.H>

// In turn includes the needed generated headers (hwp_ffd_classes, etc.)
#include <error_scope.H>
#include <set_sbe_error.H>      // Generated file

#include <fapi2_hwp_executor.H>

#include <mvpd_access.H>

#include <fapi2_attribute_service.H>

// Block of headers not currently in fapi2
#ifdef FAPI2_MISSING_HEADERS
    //#include <mbvpd_access.H>
#endif

// Block of auto generated headers not presently in fapi2
#ifdef FAPI2_AUTOGEN_HEADERS
  #include <attribute_ids.H>  // Generated file
  //#include <fapi2_chip_ec_feature.H> // Generated file
#endif

#endif // __FAPI2_TOP_LEVEL__
