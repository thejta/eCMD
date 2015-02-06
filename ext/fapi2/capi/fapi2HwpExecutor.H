// $Id$
// $Source$

/**
 *  @file fapi2HwpExecutor.H
 *
 *  @brief Defines the FAPI HWP Executor Macro.
 *
 *  The FAPI HWP Executor macro is called when a PLAT invoker function or a HWP
 *  wants to execute a HWP.
 */

/*
 * Change Log ******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- ----------------------------
 *                          mjjones     09/22/2011  Removed mandatory target
 *                                                  parameter from FAPI_EXEC_HWP
 */
#ifndef FAPI2HWPEXECUTOR_H_
#define FAPI2HWPEXECUTOR_H_

#include <fapi2PlatHwpExecutor.H>

/**
 * @brief HWP Executor macro
 *
 * This macro calls a PLAT macro which will do any platform specific work to
 * execute the HWP (e.g. dlopening a shared library)
 */
#define FAPI_EXEC_HWP(RC, FUNC, _args_...) \
    FAPI_PLAT_EXEC_HWP(RC, FUNC, ##_args_)

#endif // FAPI2HWPEXECUTOR_H_
