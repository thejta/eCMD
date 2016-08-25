#ifndef _InstructionStatus_H
#define _InstructionStatus_H
//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG

/**
 * @file InstructionStatus.H
 * @brief Provides a class for instructions status being passed to the client
 *
 * Contains a uint32_t instructionVersion, a uint32_t rc, a std::string errorMessage, and a ecmdDataBuffer data that are public
 * 
*/


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <string>
#include <ecmdDataBuffer.H>

/**
 @brief InstructionStatus class
*/
class InstructionStatus{
  public:
    /** @name InstructionStatus Constructor */
    //@{
    /**
     * @brief Default Constructor
     * @post statusVersion is set to 0x1, instructionVersion is set to 0xFFFFFFFF, and rc is set to 0
     */
    InstructionStatus(void);

    /**
     * @brief Default Destructor
     */
    ~InstructionStatus(void);
    //@}

    /** @name Transport Functions */
    //@{
    /**
     * @brief Flatten all the object data into a uint8_t buffer
     * @param o_data Byte buffer to write the flattened data to
     * @param i_len Number of bytes in the o_data buffer
     * @post o_data buffer has a flattened version of the InstructionStatus - must be pre-allocated
     * Data format (all in network byte order):
     * First Word:      statusVersion
     * Second Word:     instructionVersion
     * Third Word:      rc
     * Fourth Word:     data size
     * Multiple Words:  data
     * Multiple Words:  errorMessage
     */
    uint32_t flatten(uint8_t * o_data, uint32_t i_len) const;

    /**
     * @brief Unflatten object data from a uint8_t buffer into this InstructionStatus
     * @param i_data Byte buffer to read the flattened data from
     * @param i_len Number of bytes in the i_data buffer
     * @post This InstructionStatus is allocated and initialized with the unflattened version of i_data
     * Data format (all in network byte order):
     * First Word:      statusVersion
     * Second Word:     instructionVersion
     * Third Word:      rc
     * Fourth Word:     data size
     * Multiple Words:  data
     * Multiple Words:  errorMessage
     */
    uint32_t unflatten(const uint8_t * i_data, uint32_t i_len);

    /**
     * @brief Return number of bytes needed for a buffer to flatten the object
     * @retval Number of bytes needed
     */
    uint32_t flattenSize(void) const;
    //@}

  protected:
    uint32_t statusVersion;

  public:
    uint32_t instructionVersion;
    uint32_t rc;
    std::string errorMessage;
    ecmdDataBuffer data;
    
};

#endif // _InstructionStatus_H
