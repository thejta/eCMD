#ifndef FAPI2_HWACCESSSTRUCTS_H_
#define FAPI2_HWACCESSSTRUCTS_H_

#include <stdint.h>
typedef uint64_t spyId_t;
typedef uint32_t scanRingId_t;

namespace fapi2
{
    ///
    /// @enum fapi2::ChipOpModifyMode
    /// @brief Enumeration of modify modes used in HW access modify operations
    ///
    enum ChipOpModifyMode
    {
        CHIP_OP_MODIFY_MODE_OR = 1,  ///< Modify or mode
        CHIP_OP_MODIFY_MODE_AND = 2, ///< Modify and mode
        CHIP_OP_MODIFY_MODE_XOR = 3, ///< Modify xor mode
    };

    ///
    /// @enum fapi2::RingMode
    /// @brief Enumeration of Ring access operation modes
    ///        This is a bitmap to allow the user to specify multiple modes.
    ///
    enum RingMode
    {
        RING_MODE_SET_PULSE       = 0x00000001, ///< Set pulse
        RING_MODE_NO_HEADER_CHECK = 0x00000002, ///< Dont' check header
        // FUTURE_MODE            = 0x00000004,
        // FUTURE_MODE            = 0x00000008,
    };


}

#endif
