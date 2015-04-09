/**
 * @file plat_utils.H
 * @brief Defines platform specific fapi2 utilities
 */

#ifndef FAPI2_PLAT_UTILS_H_
#define FAPI2_PLAT_UTILS_H_

namespace fapi2plat
{
    /**  @brief This function answers the question, is scan tracing turned on?
     *   The implementation of this function is specific to the platform.
     *
     *   @returns Boolean indication
     */
    bool isScanTraceEnabled();

    /**  @brief Alter the state of scand tracing.
     *   The implementation of this function is specific to the platform.
     *
     *   @param[in] i_enable True to enable or false to disable scan trace.
     *   @return  void
     */
    void setScanTrace(bool i_enable);
};

#endif // FAPI2_PLAT_UTILS_H_
