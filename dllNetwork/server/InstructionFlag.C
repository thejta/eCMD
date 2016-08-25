//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <InstructionFlag.H>

std::string InstructionFlagToString(uint32_t i_flag) {
  std::string returnString = "";
  if (INSTRUCTION_FLAG_DEBUG & i_flag)
    returnString += "INSTRUCTION_FLAG_DEBUG, ";
  if (INSTRUCTION_FLAG_NOEXECUTE & i_flag)
    returnString += "INSTRUCTION_FLAG_NOEXECUTE, ";
  if (INSTRUCTION_FLAG_SERVER_DEBUG & i_flag)
    returnString += "INSTRUCTION_FLAG_SERVER_DEBUG, ";
  if (INSTRUCTION_FLAG_PERFORMANCE & i_flag)
    returnString += "INSTRUCTION_FLAG_PERFORMANCE, ";
  if (INSTRUCTION_FLAG_CRC_ENABLE & i_flag)
    returnString += "INSTRUCTION_FLAG_CRC_ENABLE, ";
  if (INSTRUCTION_FLAG_32BIT_CRC & i_flag)
    returnString += "INSTRUCTION_FLAG_32BIT_CRC, ";
  if (INSTRUCTION_FLAG_16BIT_CRC & i_flag)
    returnString += "INSTRUCTION_FLAG_16BIT_CRC, ";
  if (INSTRUCTION_FLAG_PERSISTENT_DATA & i_flag)
    returnString += "INSTRUCTION_FLAG_PERSISTENT_DATA, ";
  if (INSTRUCTION_FLAG_FSI_SCANHEADERCHECK & i_flag)
    returnString += "INSTRUCTION_FLAG_FSI_SCANHEADERCHECK, ";
  if (INSTRUCTION_FLAG_FSI_SCANSETPULSE & i_flag)
    returnString += "INSTRUCTION_FLAG_FSI_SCANSETPULSE, ";
  if (INSTRUCTION_FLAG_FSI_USE_DRA & i_flag)
    returnString += "INSTRUCTION_FLAG_FSI_USE_DRA, ";
  if (INSTRUCTION_FLAG_FSI_SCANEXTRABCLOCK & i_flag)
    returnString += "INSTRUCTION_FLAG_FSI_SCANEXTRABCLOCK, ";
  if (INSTRUCTION_FLAG_FSI_SCANVIAPIB & i_flag)
    returnString += "INSTRUCTION_FLAG_FSI_SCANVIAPIB, ";
  if (INSTRUCTION_FLAG_FSI_CFAM2_0 & i_flag)
    returnString += "INSTRUCTION_FLAG_FSI_CFAM2_0, ";
  if (INSTRUCTION_FLAG_64BIT_ADDRESS & i_flag)
    returnString += "INSTRUCTION_FLAG_64BIT_ADDRESS, ";
  if (INSTRUCTION_FLAG_DEVSTR & i_flag)
    returnString += "INSTRUCTION_FLAG_DEVSTR, ";
  if (INSTRUCTION_FLAG_CFAM_MAILBOX & i_flag)
    returnString += "INSTRUCTION_FLAG_CFAM_MAILBOX, ";
  if (INSTRUCTION_FLAG_CACHE_INHIBITED & i_flag)
    returnString += "INSTRUCTION_FLAG_CACHE_INHIBITED, ";
  return returnString;
}

std::string InstructionTapToString(uint32_t i_tap) {
  std::string returnString = "";
  if (INSTRUCTION_TAP_RESETTAP & i_tap)
    returnString += "INSTRUCTION_TAP_RESETTAP, ";
  if (INSTRUCTION_TAP_RUNTESTIDLE & i_tap)
    returnString += "INSTRUCTION_TAP_RUNTESTIDLE, ";
  if (INSTRUCTION_TAP_SHIFTDR & i_tap)
    returnString += "INSTRUCTION_TAP_SHIFTDR, ";
  if (INSTRUCTION_TAP_PAUSEDR & i_tap)
    returnString += "INSTRUCTION_TAP_PAUSEDR, ";
  if (INSTRUCTION_TAP_SHIFTIR & i_tap)
    returnString += "INSTRUCTION_TAP_SHIFTIR, ";
  if (INSTRUCTION_TAP_PAUSEIR & i_tap)
    returnString += "INSTRUCTION_TAP_PAUSEIR, ";
  if (INSTRUCTION_TAP_NO_TAP_STATE & i_tap)
    returnString += "INSTRUCTION_TAP_NO_TAP_STATE, ";
  if (INSTRUCTION_TAP_UPDATEDR & i_tap)
    returnString += "INSTRUCTION_TAP_UPDATEDR, ";
  if (INSTRUCTION_TAP_UPDATEIR & i_tap)
    returnString += "INSTRUCTION_TAP_UPDATEIR, ";
  if (INSTRUCTION_TAP_TAP_STATE_HACK & i_tap)
    returnString += "INSTRUCTION_TAP_TAP_STATE_HACK, ";
  if (INSTRUCTION_TAP_NO_TEST_LOGIC_RESET & i_tap)
    returnString += "INSTRUCTION_TAP_NO_TEST_LOGIC_RESET, ";
  if (INSTRUCTION_TAP_NO_JTAG_SETUP & i_tap)
    returnString += "INSTRUCTION_TAP_NO_JTAG_SETUP, ";
  if (INSTRUCTION_TAP_READ_NO_DATA & i_tap)
    returnString += "INSTRUCTION_TAP_READ_NO_DATA, ";
  switch (INSTRUCTION_TAP_DIAMOND_MASK & i_tap)
  {
    case INSTRUCTION_TAP_DIAMOND_1:
      returnString += "INSTRUCTION_TAP_DIAMOND_1, ";
      break;
    case INSTRUCTION_TAP_DIAMOND_2:
      returnString += "INSTRUCTION_TAP_DIAMOND_2, ";
      break;
    case INSTRUCTION_TAP_DIAMOND_3:
      returnString += "INSTRUCTION_TAP_DIAMOND_3, ";
      break;
    case INSTRUCTION_TAP_DIAMOND_4:
      returnString += "INSTRUCTION_TAP_DIAMOND_4, ";
      break;
    case INSTRUCTION_TAP_DIAMOND_5:
      returnString += "INSTRUCTION_TAP_DIAMOND_5, ";
      break;
    case INSTRUCTION_TAP_DIAMOND_6:
      returnString += "INSTRUCTION_TAP_DIAMOND_6, ";
      break;
    case INSTRUCTION_TAP_DIAMOND_7:
      returnString += "INSTRUCTION_TAP_DIAMOND_7, ";
      break;
    case INSTRUCTION_TAP_DIAMOND_8:
      returnString += "INSTRUCTION_TAP_DIAMOND_8, ";
      break;
    default:
      break;
  }
  return returnString;
}

std::string InstructionI2CFlagToString(uint32_t i_i2cFlag) {
  std::string returnString = "";
  if (INSTRUCTION_I2C_FLAG_NACK_RETRY_100MS & i_i2cFlag)
    returnString += "INSTRUCTION_I2C_FLAG_NACK_RETRY_100MS, ";
  return returnString;
}

std::string InstructionPnorFlagToString(uint32_t i_pnorFlag)
{
    std::string returnString = "";
    if ( INSTRUCTION_PNOR_FLAG_ERASE_PARTITION & i_pnorFlag )
        returnString += "INSTRUCTION_PNOR_FLAG_ERASE_PARTITION, ";
    if ( INSTRUCTION_PNOR_FLAG_WRITE_PARTITION & i_pnorFlag )
        returnString += "INSTRUCTION_PNOR_FLAG_WRITE_PARTITION, ";
    return returnString;
}
