// $Id$
// $Source$

/**
 *  @file fapiMultiScom.C
 *
 *  @brief Implements the fapi::MultiScom class
 */

/*
 * Change Log 
******************************************************************
 * Flag     Defect/Feature  User        Date        Description
 * ------   --------------  ----------  ----------- -------------------------
 *          F883863         atendolk    05/16/2013  Created intial draft
 *          F883863         atendolk    06/05/2013  Review rework
 *          D923348         whs         04/14/2014  Remove FAPI_RC_INVALID_PARAM
 */
#include <fapi.H>
#include <fapiUtil.H>
#include <fapiMultiScom.H>
#include <ecmdUtils.H>

#ifdef FAPI_SUPPORT_MULTI_SCOM
namespace fapi
{
static const uint32_t FAPI_DWORD_BIT_LEN = 64;

//******************************************************************************
// MultiScom::SingleScomInfo::SingleScomInfo Constructor
//******************************************************************************
MultiScom::SingleScomInfo::SingleScomInfo ( const ScomMode      i_mode,
                                            const uint64_t      i_addr,
                                            ecmdDataBufferBase& i_dataBuf,
                                            const uint64_t      i_mask ) :
                                            scomMode(i_mode),
                                            address(i_addr),
                                            pGetScomDataBuffer(NULL),
                                            putScomMask(i_mask)
{
    FAPI_DBG ("SingleScomInfo: mode %d i_addr 0x"UINT64_HEX16_PRESC_FORMAT" "
                       "mask 0x"UINT64_HEX16_PRESC_FORMAT, i_mode, i_addr, i_mask);

    switch (scomMode)
    {
        case SCOM_MODE_READ:
        case SCOM_MODE_BULK_READ:
            pGetScomDataBuffer = &i_dataBuf;
            break;

        case SCOM_MODE_WRITE_UNDER_MASK:
        case SCOM_MODE_WRITE:
        case SCOM_MODE_BULK_WRITE:
            putScomDataBuffer = i_dataBuf;
            break;

        default:
            FAPI_ERR ("Invalid Scom Mode: 0x%08x! asserting!", scomMode);
            fapiAssert(false);
            break;
    }   // end switch
}


//******************************************************************************
// MultiScom::~MultiScom    Destructor
//******************************************************************************
MultiScom::~MultiScom ()
{
    std::vector<SingleScomInfo*>::iterator l_itr = iv_ScomList.begin ();

    for (; l_itr != iv_ScomList.end (); ++l_itr)
    {   // free up the memory and mark the pointers NULL
        delete (*l_itr);
        *l_itr = NULL;
    }

    // Clear the list
    iv_ScomList.clear ();
}


//******************************************************************************
// MultiScom::addGetScom    method
//******************************************************************************
ReturnCode MultiScom::addGetScom ( const uint64_t            i_addr,
                                         ecmdDataBufferBase& o_data )
{
    ReturnCode l_rc;

    l_rc = sanityCheck (i_addr, SCOM_MODE_READ, FAPI_DWORD_BIT_LEN,
                        o_data);

    if (l_rc.ok())
    {
        SingleScomInfo* l_pScom = new SingleScomInfo (SCOM_MODE_READ,
                                                      i_addr, o_data);
        iv_ScomList.push_back (l_pScom);
    }

    else
    {
        FAPI_ERR ("MultiScom::addGetScom failed - i_addr: 0x"UINT64_HEX16_PRESC_FORMAT", o_data "
        "length: %d double words", i_addr, o_data.getDoubleWordLength());
    }

    return l_rc;
}


//******************************************************************************
// MultiScom::addPutScom    method
//******************************************************************************
ReturnCode MultiScom::addPutScom ( const uint64_t            i_addr,
                                   const ecmdDataBufferBase& i_data)
{
    ReturnCode l_rc;

    l_rc = sanityCheck (i_addr, SCOM_MODE_WRITE, FAPI_DWORD_BIT_LEN,
                        i_data);

    if (l_rc.ok())
    {
        SingleScomInfo* l_pScom = new SingleScomInfo (SCOM_MODE_WRITE, i_addr,
                                  const_cast<ecmdDataBufferBase&>(i_data));
        iv_ScomList.push_back (l_pScom);
    }

    else
    {
        FAPI_ERR ("MultiScom::addPutScom failed - i_addr: 0x"UINT64_HEX16_PRESC_FORMAT", i_data "
        "length: %d double words", i_addr, i_data.getDoubleWordLength());
    }

    return l_rc;
}


//******************************************************************************
// MultiScom::addPutScomUnderMask    method
//******************************************************************************
ReturnCode MultiScom::addPutScomUnderMask (const uint64_t i_addr,
                                    const ecmdDataBufferBase&   i_data,
                                    const ecmdDataBufferBase&   i_mask)
{
    ReturnCode l_rc;

    l_rc = sanityCheck (i_addr, SCOM_MODE_WRITE_UNDER_MASK,
                        FAPI_DWORD_BIT_LEN, i_data, &i_mask);

    if (l_rc.ok())
    {
        SingleScomInfo* l_pScom = new SingleScomInfo (
                                  SCOM_MODE_WRITE_UNDER_MASK, i_addr,
                                  const_cast<ecmdDataBufferBase&>(i_data),
                                  i_mask.getDoubleWord(0) );
        iv_ScomList.push_back (l_pScom);
    }

    else
    {
        FAPI_ERR ("MultiScom::addPutScomUnderMask failed - i_addr: 0x"UINT64_HEX16_PRESC_FORMAT", "
        "i_data length: %d double words i_mask length: %d double words",
        i_addr, i_data.getDoubleWordLength(), i_mask.getDoubleWordLength());
    }

    return l_rc;
}


//******************************************************************************
// MultiScom::addGetBulkScom    method
//******************************************************************************
ReturnCode MultiScom::addGetBulkScom ( const uint64_t i_addr,
                                       const size_t   i_lenInDoubleWords,
                                       ecmdDataBufferBase& o_data)
{
    ReturnCode l_rc;
    size_t l_lenInBits = i_lenInDoubleWords * FAPI_DWORD_BIT_LEN;

    l_rc = sanityCheck (i_addr, SCOM_MODE_BULK_READ, l_lenInBits,
                        o_data);

    if (l_rc.ok())
    {
        SingleScomInfo* l_pScom = new SingleScomInfo ( SCOM_MODE_BULK_READ,
                                                       i_addr, o_data );
        iv_ScomList.push_back (l_pScom);
    }

    else
    {
        FAPI_ERR ("MultiScom::addGetBulkScom failed - i_addr: 0x"UINT64_HEX16_PRESC_FORMAT", o_data"
        " length: %d bits, expected len: %zd bits, i_lenInDoubleWords: %zd",
        i_addr, o_data.getBitLength(), l_lenInBits, i_lenInDoubleWords);
    }

    return l_rc;
}


//******************************************************************************
// MultiScom::addPutBulkScom    method
//******************************************************************************
ReturnCode MultiScom::addPutBulkScom ( const uint64_t i_addr,
                                       const size_t   i_lenInDoubleWords,
                                       const ecmdDataBufferBase& i_data)
{
    ReturnCode l_rc;
    size_t l_lenInBits = i_lenInDoubleWords*FAPI_DWORD_BIT_LEN;

    l_rc = sanityCheck (i_addr, SCOM_MODE_BULK_WRITE, l_lenInBits,
                        i_data);

    if (l_rc.ok())
    {
        SingleScomInfo* l_pScom = new SingleScomInfo ( SCOM_MODE_BULK_WRITE,
                                  i_addr,
                                  const_cast<ecmdDataBufferBase&>(i_data) );
        iv_ScomList.push_back (l_pScom);
    }

    else
    {
        FAPI_ERR ("MultiScom::addPutBulkScom failed - i_addr: 0x"UINT64_HEX16_PRESC_FORMAT", i_data"
        " length: %d bits, expected len: %zd bits, i_lenInDoubleWords: %zd",
        i_addr, i_data.getBitLength(), l_lenInBits, i_lenInDoubleWords);
    }

    return l_rc;
}


//******************************************************************************
// MultiScom::sanityCheck    method
//******************************************************************************
ReturnCode MultiScom::sanityCheck (
                            const uint64_t                  i_addr,
                            const ScomMode                  i_scomMode,
                            const size_t                    i_lenInBits,
                            const ecmdDataBufferBase&       io_data,
                            const ecmdDataBufferBase* const i_pMask )
{
    ReturnCode l_rc;

    FAPI_INF ("MultiScom::sanityCheck - i_addr: 0x"UINT64_HEX16_PRESC_FORMAT" i_scomMode:"
             " 0x%.8X i_lenInBits: %zd io_data len: %d bits Mask len: %d bits",
              i_addr, i_scomMode, i_lenInBits, io_data.getBitLength(),
              ((NULL == i_pMask) ? (0) : (i_pMask->getBitLength())));

    do
    {
        // even a single scom operation has to be at least 64 bit wide
        if (FAPI_DWORD_BIT_LEN > i_lenInBits)
        {
            FAPI_ERR("MultiScom::sanityCheck - invalid input length %zd",
                     i_lenInBits);
            l_rc.setFapiError (FAPI_RC_INVALID_MULTISCOM_LENGTH);
            l_rc.addEIFfdc(0, &i_lenInBits, sizeof(i_lenInBits));
            break;
        }

        if (i_lenInBits != io_data.getBitLength ())
        {
            if ((SCOM_MODE_BULK_READ == i_scomMode) ||
                (SCOM_MODE_READ == i_scomMode))
            {   // for read operations, adjust buffer len if it does not match
                uint32_t l_ecmdRc =
                (const_cast<ecmdDataBufferBase&>(io_data)).setBitLength
                                                (i_lenInBits);

                FAPI_INF ("MultiScom::sanityCheck - adjusted len %d bits",
                          io_data.getBitLength ());

                if (ECMD_DBUF_SUCCESS != l_ecmdRc)
                {
                    FAPI_ERR("MultiScom::sanityCheck - error from "
                             "ecmdDataBuffer setBitLength() - len %zd rc 0x%.8X",
                             i_lenInBits, l_ecmdRc);
                    l_rc.setEcmdError(l_ecmdRc);
                    break;
                }
            }

            else
            {   // for all other SCOM write operations, buf len -must- match
                FAPI_ERR("MultiScom::sanityCheck: Input length %zd bits does not"
                         " match ecmdDataBuffer length %d bits! "
                         "Scom Mode: 0x%.8X",
                         i_lenInBits, io_data.getBitLength (),
                         i_scomMode);
                l_rc.setFapiError (FAPI_RC_INVALID_MULTISCOM_LENGTH);
                l_rc.addEIFfdc(0, &i_lenInBits, sizeof(i_lenInBits));
                uint32_t l_io_data_bitlen = io_data.getBitLength();
                l_rc.addEIFfdc(0, &l_io_data_bitlen, sizeof(l_io_data_bitlen));
                break;
            }
        }

        if (SCOM_MODE_WRITE_UNDER_MASK == i_scomMode)
        {
            if (NULL == i_pMask)
            {
                FAPI_ERR("MultiScom::sanityCheck - NULL mask buffer input "
                         "for Scom Mode: 0x%.8X! asserting ..", i_scomMode);
                fapiAssert (false);
                break;  // not needed, keeping it for now
            }

            if (i_lenInBits != i_pMask->getBitLength ())
            {
                FAPI_ERR("MultiScom::sanityCheck - Mask buffer length %d bits"
                         "does not match expected length %zd bits for "
                         "Scom Mode: 0x%.8X", i_pMask->getBitLength (),
                         i_lenInBits, i_scomMode);
                l_rc.setFapiError (FAPI_RC_INVALID_MULTISCOM_LENGTH);
                l_rc.addEIFfdc(0, &i_lenInBits, sizeof(i_lenInBits));
                uint32_t l_i_pMask_bitlen = i_pMask->getBitLength();
                l_rc.addEIFfdc(0, &l_i_pMask_bitlen, sizeof(l_i_pMask_bitlen));
                break;
            }
        }
    }   while (false);

    return l_rc;
}

}   // namespace fapi

#endif  // FAPI_SUPPORT_MULTI_SCOM
