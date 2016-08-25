#ifndef eth_transfer_h
#define eth_transfer_h
//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG

// Class Description *************************************************
//                                                                      
//  Name:  eth_transfer
//  Base class: 
//
//  Description: 
//  Usage:
//
// End Class Description *********************************************

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <signal.h>
#include <netinet/in.h>
#include <list>
#include <pthread.h>

#include <Instruction.H>
#include "transfer.h"

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------

class eth_transfer : public transfer
{
  public:
  eth_transfer();

  virtual ~eth_transfer();

  int initialize(const char * opt = NULL);

  int close();

  int close(int socket);

  int send(std::list<Instruction *> & i_instruction, std::list<ecmdDataBuffer *> & o_resultData, std::list<InstructionStatus *> & o_resultStatus);
  
  int reset();          /* Performs a reset to the connection */

  private:  // functions

    eth_transfer(eth_transfer &me);
    int operator=(eth_transfer &me);

  private:  // Data
    std::list<int> socks;
    std::list<int> availableSocks;
    uint32_t  maxSocks;
    pthread_mutex_t socksMutex;
    pthread_cond_t socksCondition;

    uint32_t seedCounter;
    std::list<uint32_t *> seeds;
    pthread_mutex_t seedsMutex;

    uint32_t clientVersion;

    /* For timer signal handling */
    struct sigaction new_action;
    struct sigaction old_action;

};


#endif /* eth_transfer_h */

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder  Description                       
//  ---- -------- ---- -------- -----  -------------------------------   
//                              cengel Initial Creation
//
// End Change Log *****************************************************
