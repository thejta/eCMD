#ifndef ecmdTransfer_h
#define ecmdTransfer_h
//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG
// Class Description *************************************************
//                                                                      
//  Name:  ecmdTransfer
//  Base class: 
//
//  Description: 
//  Usage:
//
// End Class Description *********************************************

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <transfer.h>

// x86 support was dropped with the move to gcc 3.4.6 - JTA 06/18/08
#define ETH_TRANSFER_SUPPORT




#include <eth_transfer1.h>

#include <ControlInstruction.H>


//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------
class ecmdTransfer
{
public:

  /** @brief Default Constructor */
  ecmdTransfer();

  /** @brief Default Destructor */
  virtual ~ecmdTransfer();


  /* ------------------------------- */
  /* INTERFACES TO TRANSFER PROTOCOL */
  /* ------------------------------- */
  /** @brief Initialize the appropraiate interface
   *  @param do_performance if 1 enable performance monitoring
   *  @param do_verify if 1 verify the connection after opening
   *  @param attach ip address to attach to
   *  @retval 0 on success - nonzero on failure
   */
  int initialize(const char * attach, int do_performance = 0, int do_verify = 1);
  /**
   @brief Open the interface if not already open
   */
  int open();  

  /** @brief Send the data and wait for response
   *  @param std::list of Instructions to send
   *  @param std::list of Result data
   *  @param std::list of Result Status
   */
  void send(std::list<Instruction *> & i_instruction, std::list<ecmdDataBuffer *> & o_resultData, std::list<InstructionStatus *> & o_resultStatus);

  /** @brief Close down Protocol
   *  @retval 0 on success - nonzero on failure
   */
  int close();
  /** @brief Reset protocol
   *  @retval 0 on success - nonzero on failure
   */
  int reset();

  /** @brief Retrieve hardware info struct based on this Transfer class
   * @param info Data retrieved from Target
   */
  int getHardwareInfo(server_type_info& info);


private:  // functions
  ecmdTransfer(ecmdTransfer &me);
  int operator=(ecmdTransfer &me);

private:  // Data
  int initialized;            ///< Has this class been initialized
  transfer* tran;             ///< Pointer to the transfer protocol
  server_type_info drv_hw_info;  ///< Hardware info of this transfer class
  std::string init_data;
};

#endif /* ecmdTransfer_h */
