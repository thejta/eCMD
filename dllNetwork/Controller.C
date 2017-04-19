//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2003,2017 IBM International Business Machines Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
//IBM_PROLOG_END_TAG
// Module Description **************************************************
//
// Description: 
//
// End Module Description **********************************************
//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------

#define Controller_C

#include <Controller.H>
#include <OutputLite.H>
extern OutputLite out;

#undef Controller_C
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

Controller::Controller(const char * i_ip_address)
{
    ip_address = i_ip_address;
    isTransferValid = false;
    transferOpenAttempted = false;

    contid = "k0";

    pthread_mutex_init(&controllerMutex, NULL);
}

int Controller::initialize()
{
    int rc = 0;

    rc = transfer.initialize(ip_address.c_str());

    if (rc)
    {
        return out.error(rc, "Controller::intialize","Failure occurred while trying to initialize the transfer protocol: error code %d\n",rc);
    }
    return rc;
}

Controller::~Controller() {
  pthread_mutex_destroy(&controllerMutex);
}


int Controller::transfer_open() {
  int rc = 0;

  /* If we have already done this, let's not do it again */
  if (transferOpenAttempted) return rc;

  pthread_mutex_lock(&controllerMutex);

  /* check again after we have attained the lock */
  if (transferOpenAttempted) {
    pthread_mutex_unlock(&controllerMutex);
    return rc;
  }

  rc = transfer.open();

  /* an rc of 99 means it was already open */
  if (rc && rc != 99) {
    transferOpenAttempted = true;
    pthread_mutex_unlock(&controllerMutex);
    return out.error(rc, "Controller::transfer_open","Failure occurred while trying to open the transfer protocol: error code %d\n",rc);
  }

  if (rc == 99) rc = 0;

  transferOpenAttempted = true;

  isTransferValid = true;

  pthread_mutex_unlock(&controllerMutex);
  return rc;
}

int Controller::transfer_send(std::list<Instruction *> & i_instruction, std::list<ecmdDataBuffer *> & o_resultData, std::list<InstructionStatus *> & o_resultStatus) {

  int rc = 0;

  /* First we will make sure the transfer is open */
  rc = transfer_open();
  if ((rc == SERVER_AUTHORIZATION_INVALID) &&
      (i_instruction.begin() != i_instruction.end()) &&
      ((*(i_instruction.begin()))->getCommand() == Instruction::ADDAUTH)) {
    // clear out auth errors
    //dllFlushRegisteredErrorMsgs(SERVER_AUTHORIZATION_INVALID);
    rc = 0;
  }
  if (rc) return rc;

  transfer.send(i_instruction, o_resultData, o_resultStatus);

  return rc;
}


int Controller::transfer_close() {
  int rc = 0;

  if (isTransferValid) {

    rc =  transfer.close();
    if (rc) {
      out.error(rc, "Controller::transfer_close","Failure occurred while trying to close the transfer protocol\n");
    }

  }
  isTransferValid = false;
  return rc;
}

int Controller::getHardwareInfo(server_type_info& info) {
  int rc = 0;

  rc = transfer_open();
  if (rc) return rc;

  rc =  transfer.getHardwareInfo(info);
  if (rc) {
    out.error(rc, "Controller::getHardwareInfo","Failure occurred while trying to get hardware info\n");
  }

  return rc;
}

void Controller::extractError(InstructionStatus & i_status) {
    out.print("\n************ Start of FSP Errors ************\n");
    /* Start at word 1 because the first four characters are the F.M indicator used
     for server_store_error to know it has error strings already */
    out.print(i_status.errorMessage.c_str());
    out.print("************* End of FSP Errors *************\n\n");
}
