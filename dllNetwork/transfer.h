#ifndef transfer_h
#define transfer_h
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
//  Name:  transfer
//  Base class: 
//
//  Description: This class is the base class for all CSP interface types 
//  Usage:
//
// End Class Description *********************************************

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <stdio.h>
#include <map>
#include <sys/time.h>

#include <Instruction.H>
#define CSP_DRV_FUNCTION_NOT_DEFINED TRANSFER_FUNCTION_NOT_DEFINED

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------

class transfer
{
  public:
  transfer() {
    record_performance = perf_num_commands = perf_num_writes = perf_num_reads = perf_read_len = perf_write_len = 0;
    perf_run_time = 0.0;
  }

  virtual ~transfer() {
    if (record_performance) {
      printf("**** PIPE PERFORMANCE DUMP ********\n");
      printf("Number of Commands Run        : %d\n",(int)perf_num_commands);
      printf("Number of Writes Performed    : %d\n",(int)perf_num_writes);
      printf("Number of Reads Performed     : %d\n",(int)perf_num_reads);
      printf("Number of Bytes Received      : %d\n",(int)perf_read_len);
      printf("Number of Bytes Sent          : %d\n",(int)perf_write_len);
      printf("Time Spent Executing Commands : %f\n", perf_run_time);
      std::map<Instruction::InstructionCommand, uint32_t>::iterator commandsIter;
      for (commandsIter = commands.begin(); commandsIter != commands.end(); commandsIter++) {
        printf("%s : %d\n", InstructionCommandToString(commandsIter->first).c_str(), commandsIter->second);
      }
    }
  }

  virtual int initialize(const char * opt = NULL) {return CSP_DRV_FUNCTION_NOT_DEFINED;};     /* Initialize the interface */

  virtual int close() {return CSP_DRV_FUNCTION_NOT_DEFINED;};          /* Close the interface */

  virtual int send(std::list<Instruction *> & i_instruction, std::list<ecmdDataBuffer *> & o_resultData, std::list<InstructionStatus *> & o_resultStatus) {return CSP_DRV_FUNCTION_NOT_DEFINED;};

  virtual int reset() {return CSP_DRV_FUNCTION_NOT_DEFINED;};          /* Performs a reset to the connection */
  
  void setPerformanceMonitorEnable(int newvalue) { record_performance = newvalue; }
  int  getPerfromanceMonitorEnable() { return record_performance; }

  void addPerfNumCommands(int num) { perf_num_commands += num; }
  void addPerfRunTime(uint32_t num) { perf_run_time += num; }
  void addPerfRunTime(const struct timeval & start, const struct timeval & end ) { perf_run_time += (end.tv_sec - start.tv_sec) + (0.000001 * (end.tv_usec - start.tv_usec)); }


  private:  // functions
     transfer(transfer &me);
     int operator=(transfer &me);

  protected:
     uint32_t record_performance;
     uint32_t perf_num_commands;
     uint32_t perf_num_writes;
     uint32_t perf_num_reads;
     double perf_run_time;
     uint32_t perf_read_len;
     uint32_t perf_write_len;
    std::map<Instruction::InstructionCommand, uint32_t> commands;

  private:  // Data

};

#endif /* transfer_h */
