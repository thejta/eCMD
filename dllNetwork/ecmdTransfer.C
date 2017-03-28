//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG

// Module Description **************************************************
//
// Description: 
//
// End Module Description **********************************************
/** @file ecmdTransfer.C
 *  @brief High level interface into the different ecmdTransfer protocols
*/
/** @class ecmdTransfer
 *  @brief High level interface into the different ecmdTransfer protocols
*/

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define ecmdTransfer_C

// This ifdef is super stupid, but the damn aix compiler won't work with just fstream included.
// Here is the error I got: "/afs/rchland.ibm.com/rs_aix53/lpp/vacpp.6008/usr/vacpp/include/iostream.h", line 86.33: 1540-0063 (S) The text "(" is unexpected.
// If you can fix it, I'll buy you a beer.  JTA 07/22/08
//#ifdef _AIX
//#include <fstream.h>
//#else
#include <fstream>
//#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include <ecmdSharedUtils.H>

#include <OutputLite.H>
extern OutputLite out;

#include <ControlInstruction.H>

#include <ecmdTransfer.H>

#include <eth_transfer1.h>


#undef ecmdTransfer_C



//----------------------------------------------------------------------
//  User Types
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Macros
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Internal Function Prototypes
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------


//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------

ecmdTransfer::ecmdTransfer(): initialized(0), tran(NULL) {
  drv_hw_info.type = SERVER_UNDEFINED;
}

ecmdTransfer::~ecmdTransfer() {
  if (tran != NULL) {
    delete tran;
  }
}

/* ------------------------------- */
/* INTERFACES TO TRANSFER PROTOCOL */
/* ------------------------------- */
int ecmdTransfer::initialize(const char * attach, int do_performance, int do_verify) {
  
  int rc = 0;


  tran = new eth_transfer();
  init_data = attach;


  tran->setPerformanceMonitorEnable(do_performance);
  return rc;

}

int ecmdTransfer::open() {

  int rc = 0;

  /* We are already done here */
  if (initialized)
    return 99;

  rc = tran->initialize(init_data.c_str());
  if (rc) return rc;


  /* After everything inits ok, now set the initialized flag */
  initialized = 1;


  /* Let's query the info on the other side */
  ControlInstruction * ci = new ControlInstruction(Instruction::INFO, 0);
  ecmdDataBuffer data;
  InstructionStatus is;

  std::list<Instruction *> instructionList;
  std::list<ecmdDataBuffer *> dataList;
  std::list<InstructionStatus *> statusList;

  instructionList.push_back(ci);
  dataList.push_back(&data);
  statusList.push_back(&is);

  send(instructionList, dataList, statusList);

  if (is.rc != SERVER_COMMAND_COMPLETE) {
    rc = out.error(is.rc, "ecmdTransfer::open", "Problem querying hw info rc : %d\n", is.rc);

  } else {
 
    rc = ci->populateTypeInfo(drv_hw_info, data);
    if (rc != 0) {
      out.error("ecmdTransfer::open","Unknown version of hw info\n");
    }

    /* check if authorization is needed */
    if ((drv_hw_info.flags & SERVER_INFO_AUTH_NEEDED) != 0) {
      /* get key from environment variable and send to server */
      char * keyString = getenv("ECMD_LOCK");
      if (keyString == NULL) {
        out.error("ecmdTransfer::open","Server authentication is enabled.\n");
        out.error("ecmdTransfer::open","ECMD_LOCK environment variable not set.\n");
        out.error("ecmdTransfer::open","Cannot attempt to authenticate.\n");
        rc = TRANSFER_DEVICE_DRIVER_INIT_FAIL;
      } else {
        uint32_t key = ecmdHashString32(keyString, 0);
        ci->setup(Instruction::AUTH, key, (uint32_t) 0);
        /* clear out old command info */
        data.clear();
        is.errorMessage.clear();
        is.rc = 0;

        send(instructionList, dataList, statusList);
        if (is.rc != SERVER_COMMAND_COMPLETE) {
          out.error(is.rc, "ecmdTransfer::open","Could not authenticate. %s\n", is.errorMessage.c_str());
          rc = is.rc;
        }
      }
    }

  }
  delete ci;

  /* Query the system on lock status */

  return rc;
}

void ecmdTransfer::send(std::list<Instruction *> & i_instruction, std::list<ecmdDataBuffer *> & o_resultData, std::list<InstructionStatus *> & o_resultStatus) {

  uint32_t rc = 0;

  std::list<Instruction *>::iterator instructionIterator;
  std::list<ecmdDataBuffer *>::iterator resultDataIterator;
  std::list<InstructionStatus *>::iterator resultStatusIterator;

  /* loop through instructions for debug flags to handle */
  instructionIterator = i_instruction.begin();
  while (instructionIterator != i_instruction.end()) {
    if((*instructionIterator)->getFlags() & INSTRUCTION_FLAG_DEBUG) {
      out.print("/*********************************************/\n");
      out.print("/* Dump of Instruction sent to the FSP       */\n");
      out.print("/*********************************************/\n");
      out.print("%s", (*instructionIterator)->dumpInstruction().c_str());
      out.print("/*********************************************/\n");
      out.print("/* End of Instruction Dump                   */\n");
      out.print("/*********************************************/\n");
    }
    instructionIterator++;
  }

  bool execute = true;
  bool failureSeen = false;
  instructionIterator = i_instruction.begin();
  resultStatusIterator = o_resultStatus.begin();
  while (instructionIterator != i_instruction.end()) {
    if ((*instructionIterator)->getFlags() & INSTRUCTION_FLAG_NOEXECUTE) {
      (*resultStatusIterator)->rc = SERVER_COMMAND_COMPLETE;
      /* If this is an FSI instruction put empty data for status in the InstructionStatus data field */
      if ((*instructionIterator)->getType() == Instruction::FSI || (*instructionIterator)->getType() == Instruction::I2C || (*instructionIterator)->getType() == Instruction::GSD2PIB)  {
        (*resultStatusIterator)->data.setBitLength(32);
      }
      execute = false;
    } else {

      /* Check to make sure that if crc generation is enable there is a mode selected */
      if (((*instructionIterator)->getFlags() & INSTRUCTION_FLAG_CRC_ENABLE) && !(((*instructionIterator)->getFlags() & INSTRUCTION_FLAG_16BIT_CRC) || ((*instructionIterator)->getFlags() & INSTRUCTION_FLAG_32BIT_CRC))) {
        (*resultStatusIterator)->rc = TRANSFER_INVALID_CRC_FLAGS;
        failureSeen = true;
      }

      /* Check to make sure that the device driver has been initialized */
      if (!initialized && !((*instructionIterator)->getFlags() & INSTRUCTION_FLAG_NOEXECUTE)) {
        (*resultStatusIterator)->rc = TRANSFER_UNINITIALIZED_DRIVER;
        failureSeen = true;
      }
    }
    instructionIterator++;
    resultStatusIterator++;
  }

  if (execute && !failureSeen) {

    struct timeval start_time;
    struct timeval end_time;

    if (tran->getPerfromanceMonitorEnable()) {
      gettimeofday(&start_time, NULL);
      tran->addPerfNumCommands(1);
    }

    rc = tran->send(i_instruction, o_resultData, o_resultStatus);

    if (tran->getPerfromanceMonitorEnable()) {
      gettimeofday(&end_time, NULL);
      tran->addPerfRunTime(start_time, end_time);
    }

    /* We are going to try this again if it was a pipe failure */
    if ((rc == TRANSFER_ACK_TIMEOUT) || (rc == TRANSFER_PIPE_SEND_FAIL) || (rc == TRANSFER_RECEIVE_FAIL) || (rc == TRANSFER_RECEIVE_TIMEOUT)) {
      out.note("ecmdTransfer::send","Timed out - trying a reset and then doing it again\n");
      reset();

      sleep(1);

      rc = tran->send(i_instruction, o_resultData, o_resultStatus);
    }

    if (rc) {
      resultStatusIterator = o_resultStatus.begin();
      (*resultStatusIterator)->rc = rc;
      return;
    }
  }

  /* loop through instructions for debug flags to handle and print results */
  instructionIterator = i_instruction.begin();
  resultDataIterator = o_resultData.begin();
  resultStatusIterator = o_resultStatus.begin();
  while (instructionIterator != i_instruction.end()) {
    if((*instructionIterator)->getFlags() & INSTRUCTION_FLAG_DEBUG) {
      out.print("/*********************************************/\n");
      out.print("/* Dump of Instruction Result                */\n");
      out.print("/*********************************************/\n");
      out.print("rc            : %08X\n", (*resultStatusIterator)->rc);
      out.print("executedVer   : %u\n", (*resultStatusIterator)->instructionVersion);
      out.print("status length : %u\n", (*resultStatusIterator)->data.getBitLength());
      out.print("status data   : ");
      for(uint32_t j = 0; j < (*resultStatusIterator)->data.getWordLength(); j++) {
        out.print("%.8X ", (*resultStatusIterator)->data.getWord(j));
        if (!((j+1) % 5)) out.print("\n\t\t");
      }
      out.print("\n");

      size_t start_position = 0;
      size_t end_position = 0;
       // search for newline
       while (std::string::npos != (end_position = (*resultStatusIterator)->errorMessage.find("\n", start_position))) {
         // see SERVER_DEBUG is not at begining of substr
         if (std::string::npos == ((*resultStatusIterator)->errorMessage.substr(start_position, end_position - start_position).find("SERVER_DEBUG"))) {
           // output found string
           out.print("errorMessage  : %s\n", (*resultStatusIterator)->errorMessage.substr(start_position, end_position - start_position).c_str());
         }
         start_position = end_position + 1;
         // if we are at the end
         if (end_position == (*resultStatusIterator)->errorMessage.length() - 1) {
           break;
         }
       }

      out.print("result length : %u\n", (*resultDataIterator)->getBitLength());
      out.print("result data   : ");
      for(uint32_t j = 0; j < (*resultDataIterator)->getWordLength(); j++) {
        out.print("%.8X ", (*resultDataIterator)->getWord(j));
        if (!((j+1) % 5)) out.print("\n\t\t");
      }
      out.print("\n");

      if (((*instructionIterator)->getFlags() & INSTRUCTION_FLAG_SERVER_DEBUG)) {
        out.print("/** Server Debug Output **********************/\n");
        /* dump all of ther server trace output */
        /* strip out messages without SERVER_DEBUG */
        size_t start_position = 0;
        size_t end_position = 0;
        // search for SERVER_DEBUG
        while (std::string::npos != (start_position = (*resultStatusIterator)->errorMessage.find("SERVER_DEBUG", end_position))) {
          // search for newline
          if (std::string::npos != (end_position = (*resultStatusIterator)->errorMessage.find("\n", start_position))) {
          } else {
            end_position = (*resultStatusIterator)->errorMessage.length() - 1;
          }
          // output found string
          out.print("%s\n", (*resultStatusIterator)->errorMessage.substr(start_position, end_position - start_position).c_str());
        }
        out.print("/** End of Server Debug Output ***************/\n");
      }
      out.print("/*********************************************/\n");
      out.print("/* End of Instruction Result Dump            */\n");
      out.print("/*********************************************/\n");
    }
    instructionIterator++;
    resultDataIterator++;
    resultStatusIterator++;
  }

  return;
}

int ecmdTransfer::close() {
  int rc = 0;

  if (tran) {
    if (initialized
        ) {
      rc = tran->close();
      if (rc && (rc != CSP_DRV_FUNCTION_NOT_DEFINED)) {
        out.error("ecmdTransfer::close","Problems occurred closing down interface\n");
      }
    }

    delete tran;
    tran = NULL;
    initialized = 0;
  }

  return 0;
}

int ecmdTransfer::reset() {

  return tran->reset();
}

int ecmdTransfer::getHardwareInfo(server_type_info& info) {
  info = drv_hw_info;
  return 0;
}
