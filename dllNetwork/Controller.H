#ifndef Controller_h
#define Controller_h
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

// Class Description *************************************************
//                                                                      
//  Name:  Controller
//  Base class: 
//
//  Description: 
//  Usage:
//
// End Class Description *********************************************

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
/* @file Controller.H
 * @brief Contains the Controller class which represents a Service Processor
*/
/* @class Controller
 * @brief The Controller class represents a Service Processor
*/
#include <list>

#include <ecmdTransfer.H>
#include <Instruction.H>
#include <InstructionStatus.H>
#include <ControlInstruction.H>

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------


class Controller
{
  public:
  Controller(const char * ip_address);

  ~Controller();


  /** @brief Intialize this controller - this also initializes the transfer class */
  int initialize();

  /** @brief Returns wether this Controller is Valid (meaning we have an active/open communication to it) */
   bool getTransferValid() { return isTransferValid; }
  

  /** @brief Get the name of this controller */
  std::string getControllerId() { return contid; }

  /* ------------------------------- */
  /* INTERFACES TO TRANSFER PROTOCOL */
  /* ------------------------------- */
  /**
   @brief Fetch the Transfer instance for this controller
   */

  /** @brief Open communications to the Controller */
  int transfer_open();
  /** @brief Send data to the Controller */
  int transfer_send(std::list<Instruction *> & i_instruction, std::list<ecmdDataBuffer *> & o_resultData, std::list<InstructionStatus *> & o_resultStatus);
  /** @brief Close communications to the Controller */
  int transfer_close();

  /** @brief Retrieve hardware info struct based on this Transfer class
    * @param info Data retrieved from Target
    */
  int getHardwareInfo(server_type_info& info);

  /** @brief Extract error messages out of InstructionStatus */
  void extractError(InstructionStatus & i_status);

  private:  // functions
     Controller(Controller &me);
    int operator=(Controller &me);

  private:  // Data

    ecmdTransfer transfer;             ///< Transfer class to access this controller
    std::string ip_address;
    std::string contid;                 ///< Name of this controller

    bool isTransferValid;                       ///< isValid is only set when the transfer protocol has actually been opened successfully
    bool transferOpenAttempted;         ///< This is set if we have attempted to open this interface already

    pthread_mutex_t controllerMutex;

};


#endif /* Controller_h */
