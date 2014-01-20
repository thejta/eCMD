// $Id$
// $Source$

/**
 *  @file fapiMultiScom.H
 *
 *  @brief Defines the fapi::MultiScom class.
 *      This interface allows one to add multiple single SCOM operations like
 *      getScom, putScom, putScomUnderMask, getBulkScom and putBulkScom, to a
 *      list. The multiple SCOMs list can then be submitted to the platform code
 *      to execute all the single SCOM operations it contains, in a single
 *      invocation, in the order they were added to the list.
 *      In the back-end, the platform code may leverage advanced
 *      hardware capabilities (e.g. FSI Shift Engine in DMA mode) for
 *      optimal performance.
 */

/*
 * Change Log 
******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- -------------------------
 *                          atendolk    02/21/2013  Created intial draft
 *                          atendolk    05/06/2013  Review rework
 *          F883863         atendolk    05/14/2013  Review rework
 *          F883863         atendolk    06/05/2013  Review rework
 */

#ifndef FAPIMULTISCOM_H_
#define FAPIMULTISCOM_H_

#ifdef FAPI_SUPPORT_MULTI_SCOM

#include <stdint.h>
#include <ecmdDataBufferBase.H>
#include <fapiReturnCode.H>

namespace fapi
{

/**
 * @class MultiScom
 *
 *  This class provides an interface to add multiple single SCOM operations
 *  (specific to a chip) to be executed at once (in the order they were added
 *  to the MultiScom object) via the fapiMultiScom interface.
 */
class MultiScom
{
public:
    /**
     * @enum    ScomMode
     * @brief   Enumeration of types of SCOM operations supported in MultiScom
     */
    enum ScomMode
    {
        SCOM_MODE_NONE              = 0x00,
        SCOM_MODE_READ              = 0x01, // Single SCOM read (64 bit)
        SCOM_MODE_WRITE             = 0x02, // Single SCOM write (64 bit)
        SCOM_MODE_WRITE_UNDER_MASK  = 0x03, // Scom Write Under Mask (64 bit)
        SCOM_MODE_BULK_READ         = 0x04, // Large bulk of SCOM reads
        SCOM_MODE_BULK_WRITE        = 0x05, // Large bulk of SCOM writes
    };

    /**
     * @struct  SingleScomInfo
     * @brief   Data type consolidating information of a single SCOM operation
     */
    struct SingleScomInfo
    {
        /**
         *  @brief Constructor
         */
        SingleScomInfo ( const ScomMode      i_mode,
                         const uint64_t      i_addr,
                         ecmdDataBufferBase& i_dataBuf,
                         const uint64_t      i_mask = 0 );

        // Data members
        ScomMode            scomMode;           // Type of Scom operation
        uint64_t            address;            // Scom address for all modes
        ecmdDataBufferBase* pGetScomDataBuffer; // Pointer to the user's
                                                // ecmdDBB for scom read modes
        ecmdDataBufferBase  putScomDataBuffer;  // Copy of user's ecmdDBB
                                                // for scom write modes
        uint64_t            putScomMask;        // Mask for
                                                // SCOM_WRITE_UNDER_MASK_MODE

    private:
        // disabled copy constructor and assignment operator
        SingleScomInfo(const SingleScomInfo& i_rhval);
        SingleScomInfo& operator=(const SingleScomInfo& i_rhval);
    };

    /**
     *  @brief Default constructor
     */
    MultiScom() : iv_NumOfCompletes(0)
    {
    }

    /**
     *  @brief Destructor
     */
    ~MultiScom();

    ////////////////////////////////////////////////////////////////////////////
    // HWPs should use the following methods to setup a MultiScom and not
    // use iv_ScomList directly
    ////////////////////////////////////////////////////////////////////////////

    /**
     *  @brief Add a getScom (SCOM_MODE_READ) operation to the MultiScom object
     *
     *  @param[in]      i_addr  Scom address to read from
     *  @param[out]     o_data  Reference to user's ecmdDataBufferBase to hold
     *                  data read from i_addr.
     *  @return ReturnCode  Zero on success, else FAPI/ECMD generated error
     *  @note   The user's buffer at o_data will be modified to correct length,
     *          if needed. As the data read is copied directly to the user's
     *          buffer, the user must NOT re-use the buffer
     */
    ReturnCode addGetScom ( const uint64_t              i_addr,
                                  ecmdDataBufferBase&   o_data );

    /**
     *  @brief Add a putScom (SCOM_MODE_WRITE) operation to the MultiScom object
     *
     *  @param[in]  i_addr  Scom address to write to
     *  @param[in]  i_data  ecmdDataBufferBase object that holds data to write
     *                      to i_addr
     *  @return ReturnCode  Zero on success, else FAPI generated error
     *  @note   As a copy of i_data is taken, the user is free to re-use
     *          the input buffer. o_data size must match 64 bits
     */
    ReturnCode addPutScom ( const uint64_t              i_addr,
                            const ecmdDataBufferBase&   i_data);

    /**
     *  @brief Add a putScomUnderMask (SCOM_MODE_WRITE_UNDER_MASK) operation to
     *          the MultiScom object
     *
     *  @param[in]  i_addr  SCOM address to write to
     *  @param[in]  i_data  ecmdDataBufferBase object that holds data to write
     *                      to i_addr
     *  @param[in]  i_mask  ecmdDataBufferBase object that holds the mask to use
     *                      for the putScomUnderMask operation
     *  @return ReturnCode  Zero on success, else FAPI generated error
     *  @note   As copies of i_data and i_mask are taken, the user is free to
     *          re-use these buffers. The buffer sizes must both match 64 bits
     *  @note   Data written at i_addr conforms to the below formula:
     *          [(i_data & i_mask) | (val\@i_addr & ~i_mask)]
     *  @note   This operation is currently -not- supported on processor chips
     *          and its chiplets. A MultiScom with this operation on a processor
     *          chip/chiplet will return an error
     */
    ReturnCode addPutScomUnderMask (const uint64_t               i_addr,
                                    const ecmdDataBufferBase&    i_data,
                                    const ecmdDataBufferBase&    i_mask);

    /**
     *  @brief Add a getBulkScom (SCOM_MODE_BULK_READ) operation to the
     *          MultiScom object
     *
     *  @param[in]  i_addr              Base/Start address of Bulk SCOM
     *                                  operation
     *  @param[in]  i_lenInDoubleWords  Length of the bulk data to read in
     *                                  multiples of 8B (64 bit)
     *  @param[out] o_data              Reference to the user's
     *                                  ecmdDataBufferBase to hold data read
     *                                  from SCOM addresses starting at
     *                                  i_addr
     *  @return ReturnCode              Zero on success, else FAPI/ECMD
     *                                  generated error
     *  @note   The user's buffer at o_data will be modified to correct length,
     *          if needed. As the data read is copied directly to the user's
     *          buffer, the user must NOT re-use the buffer
     *  @note   This operation is supported only on processor chips and its
     *          chiplets, that support auto-increment. A MultiScom with this
     *          operation on non-processor targets or on processor targets that
     *          do not support auto-increment, will return an error
     */
    ReturnCode addGetBulkScom ( const uint64_t            i_addr,
                                const size_t              i_lenInDoubleWords,
                                      ecmdDataBufferBase& o_data);

    /**
     *  @brief Add a putBulkScom (SCOM_MODE_BULK_WRITE) operation to the
     *          MultiScom object
     *
     *  @param[in]  i_addr              Base/Start address of Bulk SCOM
     *                                  operation
     *  @param[in]  i_lenInDoubleWords  Length of the bulk data to write in
     *                                  multiples of 8B (64 bit)
     *  @param[in]  i_data              ecmdDataBufferBase object that holds
     *                                  data to write to SCOM addresses starting
     *                                  at i_addr. It is expected that the
     *                                  length of this buffer matches
     *                                  i_lenInDoubleWords
     *  @return ReturnCode              Zero on success, else FAPI generated
     *                                  error
     *  @note   As a copy of i_data is taken, the user is free to re-use
     *          the input buffer. Buffer length must be multiple of 64 bits
     *  @note   This operation is supported only on processor chips and its
     *          chiplets, that support auto-increment. A MultiScom with this
     *          operation on non-processor targets or on processor targets that
     *          do not support auto-increment, will return an error
     */
    ReturnCode addPutBulkScom ( const uint64_t              i_addr,
                                const size_t                i_lenInDoubleWords,
                                const ecmdDataBufferBase&   i_data);

    // public data members

    // List of pointers to SCOM operations added to this MultiScom object.
    // PLAT code should use the iv_ScomList vector directly to process the
    // MultiScom structure
    std::vector<SingleScomInfo*> iv_ScomList;

    // PLAT code sets this up to indicate how many SCOMs completed successfully.
    // On error, can be read by HWP to get details of the failing SCOM from
    // iv_ScomList
    uint32_t                    iv_NumOfCompletes;

private:
    // Disabled copy constructor and assignment operator
    MultiScom(const MultiScom& i_rhval);
    MultiScom& operator=(const MultiScom& i_rhval);

    /**
     *  @brief Performs sanity checks on a SingleScomInfo entry, before adding
     *    it to the MultiScom object
     *
     *  Validates contents of the input single SCOM data.
     *  Returns error if SingleScomInfo indicates any type of write
     *  operation and data length is less than size to be written.
     *  For read operation, if needed, it will adjust the buffer length to
     *  the expected size and return modified buffer to the caller
     *
     *  @param i_addr             Scom address
     *  @param i_scomMode         Scom mode
     *  @param i_lenInDoubleWords Length of the data buffer in multiples of 8B
     *  @param io_data            Data buffer
     *  @param i_pMask            Mask buffer, used for addPutScomUnderMask
     *
     *  @return ReturnCode. Zero on success, else FAPI generated error
     */
    ReturnCode sanityCheck (const uint64_t                  i_addr,
                            const ScomMode                  i_scomMode,
                            const size_t                    i_lenInDoubleWords,
                            const ecmdDataBufferBase&       io_data,
                            const ecmdDataBufferBase* const i_pMask = NULL);
}; // class MultiScom

}   // namespace fapi

#endif  // FAPI_SUPPORT_MULTI_SCOM
#endif  // FAPIMULTISCOM_H_
