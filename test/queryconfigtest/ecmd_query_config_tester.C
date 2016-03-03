//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2003,2016 IBM International Business Machines Corp.
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



#include <list>
#include <string>
#include <stdio.h>

#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdSharedUtils.H>
#include <ecmdStructs.H>
#include <sys/time.h>                      //@SJ

  /***************************************************************************/
  /*  ---------    Function ProtoTypes and Defines  ---------------          */
  /***************************************************************************/
  
 
//@SJ  -begin

  timeval curTv;
  
  static double totaltime = 0;
  
  double start , stop; 
  
  static int query_config_TotalCnt;
  
//@SJ -end
  
std::string printTgtState(ecmdChipTargetState_t i_tgt_state)
{
    
  
  std::string answer;
  
  if (i_tgt_state == ECMD_TARGET_UNKNOWN_STATE)
    answer =  "ECMD_TARGET_UNKNOWN_STATE";
    
  else if (i_tgt_state == ECMD_TARGET_FIELD_VALID)
    answer =  "ECMD_TARGET_FIELD_VALID";
    
  else if (i_tgt_state == ECMD_TARGET_FIELD_UNUSED)
    answer =  "ECMD_TARGET_FIELD_UNUSED";
    
  else if (i_tgt_state == ECMD_TARGET_FIELD_WILDCARD)
    answer =  "ECMD_TARGET_FIELD_WILDCARD";
    
  else if (i_tgt_state == ECMD_TARGET_THREAD_ALIVE)
    answer =  "ECMD_TARGET_THREAD_ALIVE";
    
  else
    answer =  "!!! --- BAD TARGET STATE --- !!!";
  
 
  return answer;
};


void printTgt(ecmdChipTarget i_tgt)
{
   
    // Print struct data.
    printf("Target Input:\n");
    printf("  Cage: %d, Cage State: %s\n", i_tgt.cage, printTgtState(i_tgt.cageState).c_str());
    printf("  Node: %d, Node State: %s\n", i_tgt.node, printTgtState(i_tgt.nodeState).c_str());
    printf("  Slot: %d, Slot State: %s\n", i_tgt.slot, printTgtState(i_tgt.slotState).c_str());
    printf("  ChipType: %s, ChipType State: %s\n", i_tgt.chipType.c_str(), printTgtState(i_tgt.chipTypeState).c_str());
    printf("  Pos: %d, Pos State: %s\n", i_tgt.pos, printTgtState(i_tgt.posState).c_str());
    printf("  ChipUnitType: %s, ChipUnitTypeState: %s\n", i_tgt.chipUnitType.c_str(), printTgtState(i_tgt.chipUnitTypeState).c_str()); 
    printf("  ChipUnitNum: %d, ChipUnitNumState: %s\n", i_tgt.chipUnitNum, printTgtState(i_tgt.chipUnitNumState).c_str());
    printf("  Thread: %d, Thread State: %s\n", i_tgt.thread, printTgtState(i_tgt.threadState).c_str());
    printf("  UnitId: %d, UnitId State: %s\n", i_tgt.unitId, printTgtState(i_tgt.unitIdState).c_str());

   
};

void printQueryData(ecmdQueryData queryData)
{

    
// from ecmdQueryUser.C - showconfig
    //Setup the target that will be used to query the system config 
//    ecmdQueryData queryData;            ///< Query data
    ecmdChipTarget target;              ///< Target to refine query
    std::list<ecmdCageData>::iterator ecmdCurCage;      ///< Iterators
    std::list<ecmdNodeData>::iterator ecmdCurNode;
    std::list<ecmdSlotData>::iterator ecmdCurSlot;
    std::list<ecmdChipData>::iterator ecmdCurChip;
    std::list<ecmdChipUnitData>::iterator ecmdCurChipUnit;
    std::list<ecmdThreadData>::iterator ecmdCurThread;
    char buf[500];

    printf("Query Data Output:\n");
// CAGE
    for (ecmdCurCage = queryData.cageData.begin(); ecmdCurCage != queryData.cageData.end(); ecmdCurCage ++) {

      sprintf(buf,"  Cage %d\n",ecmdCurCage->cageId); ecmdOutput(buf);
//      sprintf(buf,"  Details: CageUid=%8.8X\n",ecmdCurCage->unitId); ecmdOutput(buf);

// NODE
      for (ecmdCurNode = ecmdCurCage->nodeData.begin(); ecmdCurNode != ecmdCurCage->nodeData.end(); ecmdCurNode ++) {

      if (ecmdCurNode->nodeId == ECMD_TARGETDEPTH_NA) {
        sprintf(buf,"    Node NA\n"); ecmdOutput(buf);
      } else {
        sprintf(buf,"    Node %d\n",ecmdCurNode->nodeId ); ecmdOutput(buf);
      }
//        sprintf(buf,"    Details: NodeUid=%8.8X\n",ecmdCurNode->unitId ); ecmdOutput(buf);

// SLOT
        for (ecmdCurSlot = ecmdCurNode->slotData.begin(); ecmdCurSlot != ecmdCurNode->slotData.end(); ecmdCurSlot ++) {

      if (ecmdCurSlot->slotId == ECMD_TARGETDEPTH_NA) {
        sprintf(buf,"      Slot NA\n"); ecmdOutput(buf);
      } else {
        sprintf(buf,"      Slot %d\n",ecmdCurSlot->slotId); ecmdOutput(buf);
      }
 //         sprintf(buf,"      Details: SlotUid=%8.8X\n",ecmdCurSlot->unitId); ecmdOutput(buf); buf[0] = '\0';

// CHIP/POS
          for (ecmdCurChip = ecmdCurSlot->chipData.begin(); ecmdCurChip != ecmdCurSlot->chipData.end(); ecmdCurChip ++) {

          sprintf(buf,"        ChipType=%s, ChipPos=%d\n",ecmdCurChip->chipType.c_str(), ecmdCurChip->pos ); ecmdOutput(buf);
//              sprintf(buf,"      Details: ChipUid=%8.8X\n",ecmdCurChip->unitId ); ecmdOutput(buf);

//CORE
            for (ecmdCurChipUnit = ecmdCurChip->chipUnitData.begin(); ecmdCurChipUnit != ecmdCurChip->chipUnitData.end(); ecmdCurChipUnit++) {
            sprintf(buf,"          ChipUnitType=%s\n",ecmdCurChipUnit->chipUnitType.c_str()); ecmdOutput(buf);
            sprintf(buf,"          ChipUnitNum %d\n",ecmdCurChipUnit->chipUnitNum ); ecmdOutput(buf);
//                sprintf(buf,"        Details: CoreUid=%8.8X\n",ecmdCurChipUnit->unitId ); ecmdOutput(buf);

// THREAD
              for (ecmdCurThread = ecmdCurChipUnit->threadData.begin(); ecmdCurThread != ecmdCurChipUnit->threadData.end(); ecmdCurThread ++) {

              sprintf(buf,"            Thread %d\n",ecmdCurThread->threadId ); ecmdOutput(buf);
//              sprintf(buf,"          Details: ThreadUid=%8.8X\n",ecmdCurThread->unitId ); ecmdOutput(buf);

              } /* curThreadIter */

            } /* curCoreIter */

          } /* curChipIter */

        } /* curSlotIter */

      } /* curNodeIter */

    } /* curCageIter */

   


}



  /***************************************************************************/
  /*  ---------    MAIN  ---------------                                     */
  /***************************************************************************/

int main (int argc, char *argv[])
{
    
 
  printf(">>> Start main\n");
  uint32_t rc = ECMD_SUCCESS;
  ecmdChipTarget target;
  ecmdQueryData o_queryData;
  std::vector< ecmdChipTarget > tgt_vector;
  std::vector< std::string > msg_vector;
  std::string tmp_str;
  uint32_t num_tests=0;

  printf(">>> Load Dll ..");
  // Load and initialize the eCMD Dll 
  // Which DLL to load is determined by the ECMD_DLL_FILE environment variable 
  rc = ecmdLoadDll("");
  printf(".. rc = %d\n", rc);
  if (rc) {
    printf("**** ERROR : Problems loading eCMD Dll!\n");
    return rc;
  }


  // Pass your arguments to the Dll so it can parse out any common args 
  // Common args like -p# -c# will be removed from arg list upon return 
  printf(">>> Parse Commands\n");
  rc = ecmdCommandArgs(&argc, &argv); 
  if (rc) return rc;


  /***************************************************************************/
  /*  ---------    User's Testcase Section  ---------------                  */
  /***************************************************************************/

  printf(">>> Start of Testcase Section ...\n");

  /***************************************************************************/
  /*  ---------    Create Testcases  ---------------                         */
  /***************************************************************************/

/*****************************************************************************/
  tmp_str= "All WildCard - Down to Thread";
  msg_vector.push_back(tmp_str);

  target.cage = 0;
  target.cageState = ECMD_TARGET_FIELD_WILDCARD;
  target.node = 0;
  target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
  target.slot = 0;
  target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipType = "";
  target.chipTypeState = ECMD_TARGET_FIELD_WILDCARD;
  target.pos = 0;
  target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitType = "";
  target.chipUnitTypeState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitNum = 0;
  target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
  target.thread = 0;
  target.threadState = ECMD_TARGET_FIELD_WILDCARD;

  tgt_vector.push_back(target);
  num_tests++;

/*****************************************************************************/
  tmp_str= "All WildCard - Down to ChipUnit";
  msg_vector.push_back(tmp_str);

  target.cage = 0;
  target.cageState = ECMD_TARGET_FIELD_WILDCARD;
  target.node = 0;
  target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
  target.slot = 0;
  target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipType = "";
  target.chipTypeState = ECMD_TARGET_FIELD_WILDCARD;
  target.pos = 0;
  target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitType = "";
  target.chipUnitTypeState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitNum = 0;
  target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
  target.thread = 0;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  tgt_vector.push_back(target);
  num_tests++;

/*****************************************************************************/
  tmp_str= "Only Want pu Pos1, Core1";
  msg_vector.push_back(tmp_str);
  
  target.cage = 0;
  target.cageState = ECMD_TARGET_FIELD_WILDCARD;
  target.node = 0;
  target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
  target.slot = 0;
  target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipType = "pu";
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.pos = 1;
  target.posState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitType = "core";
  target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitNum = 1;
  target.chipUnitNumState = ECMD_TARGET_FIELD_VALID;
  target.thread = 1;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;
  
  tgt_vector.push_back(target);
  num_tests++; 
  
/*****************************************************************************/
  tmp_str= "Only Want pu Pos1, Core1, Thread0";
  msg_vector.push_back(tmp_str);
  
  target.cage = 0;
  target.cageState = ECMD_TARGET_FIELD_WILDCARD;
  target.node = 0;
  target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
  target.slot = 0;
  target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipType = "pu";
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.pos = 1;
  target.posState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitType = "core";
  target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitNum = 1;
  target.chipUnitNumState = ECMD_TARGET_FIELD_VALID;
  target.thread = 0;
  target.threadState = ECMD_TARGET_FIELD_VALID;

  tgt_vector.push_back(target);
  num_tests++;

/*****************************************************************************/
  tmp_str= "Only Want pu Pos1, Core1, Thread1";
  msg_vector.push_back(tmp_str);
  
  target.cage = 0;
  target.cageState = ECMD_TARGET_FIELD_WILDCARD;
  target.node = 0;
  target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
  target.slot = 0;
  target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipType = "pu";
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.pos = 1;
  target.posState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitType = "core";
  target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitNum = 1;
  target.chipUnitNumState = ECMD_TARGET_FIELD_VALID;
  target.thread = 1;
  target.threadState = ECMD_TARGET_FIELD_VALID;

  tgt_vector.push_back(target);
  num_tests++;

/*****************************************************************************/
  tmp_str= "Only Want pu Pos2, Core0, Thread3";
  msg_vector.push_back(tmp_str);
  
  target.cage = 0;
  target.cageState = ECMD_TARGET_FIELD_WILDCARD;
  target.node = 0;
  target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
  target.slot = 0;
  target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipType = "pu";
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.pos = 2;
  target.posState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitType = "core";
  target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitNum = 0;
  target.chipUnitNumState = ECMD_TARGET_FIELD_VALID;
  target.thread = 3;
  target.threadState = ECMD_TARGET_FIELD_VALID;

  tgt_vector.push_back(target);
  num_tests++;

/*****************************************************************************/
  tmp_str= "Only Want pu Pos2, Core5, Thread2";
  msg_vector.push_back(tmp_str);
  
  target.cage = 0;
  target.cageState = ECMD_TARGET_FIELD_WILDCARD;
  target.node = 0;
  target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
  target.slot = 0;
  target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipType = "pu";
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.pos = 2;
  target.posState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitType = "core";
  target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitNum = 5;
  target.chipUnitNumState = ECMD_TARGET_FIELD_VALID;
  target.thread = 2;
  target.threadState = ECMD_TARGET_FIELD_VALID;

  tgt_vector.push_back(target);
  num_tests++;


/*****************************************************************************/
  tmp_str= "Running mc Test on pu - want all mc0s";
  msg_vector.push_back(tmp_str);

  target.cage = 0;
  target.cageState = ECMD_TARGET_FIELD_WILDCARD;
  target.node = 0;
  target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
  target.slot = 0;
  target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipType = "pu";
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.pos = 0;
  target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitType = "mc";
  target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitNum = 0;
  target.chipUnitNumState = ECMD_TARGET_FIELD_VALID;
  target.thread = 0;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  tgt_vector.push_back(target);
  num_tests++;

/*****************************************************************************/
  tmp_str= "Running mc Test on pu - want pos1 mc1";
  msg_vector.push_back(tmp_str);

  target.cage = 0;
  target.cageState = ECMD_TARGET_FIELD_WILDCARD;
  target.node = 0;
  target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
  target.slot = 0;
  target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipType = "pu";
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.pos = 1;
  target.posState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitType = "mc";
  target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitNum = 1;
  target.chipUnitNumState = ECMD_TARGET_FIELD_VALID;
  target.thread = 0;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  tgt_vector.push_back(target);
  num_tests++;

/*****************************************************************************/
  tmp_str= "Running mc 3 Test on pu";
  msg_vector.push_back(tmp_str);

  target.cage = 0;
  target.cageState = ECMD_TARGET_FIELD_WILDCARD;
  target.node = 0;
  target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
  target.slot = 0;
  target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipType = "pu";
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.pos = 0;
  target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitType = "mc";
  target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitNum = 3;
  target.chipUnitNumState = ECMD_TARGET_FIELD_VALID;
  target.thread = 0;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  tgt_vector.push_back(target);
  num_tests++;


/*****************************************************************************/

  tmp_str= "Running Core Test on pu";
  msg_vector.push_back(tmp_str);

  target.cage = 0;
  target.cageState = ECMD_TARGET_FIELD_WILDCARD;
  target.node = 0;
  target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
  target.slot = 0;
  target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipType = "pu";
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.pos = 0;
  target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitType = "core";
  target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitNum = 0;
  target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
  target.thread = 0;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  tgt_vector.push_back(target);
  num_tests++;

/*****************************************************************************/
  tmp_str= "Running Core 9 Test on pu";
  msg_vector.push_back(tmp_str);

  target.cage = 0;
  target.cageState = ECMD_TARGET_FIELD_WILDCARD;
  target.node = 0;
  target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
  target.slot = 0;
  target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipType = "pu";
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.pos = 0;
  target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitType = "core";
  target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitNum = 9;
  target.chipUnitNumState = ECMD_TARGET_FIELD_VALID;
  target.thread = 0;
  target.threadState = ECMD_TARGET_FIELD_WILDCARD;

  tgt_vector.push_back(target);
  num_tests++;

/*****************************************************************************/
  tmp_str= "Running Core Test on nova";
  msg_vector.push_back(tmp_str);

  target.cage = 0;
  target.cageState = ECMD_TARGET_FIELD_WILDCARD;
  target.node = 0;
  target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
  target.slot = 0;
  target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipType = "nova";
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.pos = 0;
  target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitType = "core";
  target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitNum = 0;
  target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
  target.thread = 0;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  tgt_vector.push_back(target);
  num_tests++;


/*****************************************************************************/
  tmp_str= "Running Pos Test on 'fred' Chip";
  msg_vector.push_back(tmp_str);

  target.cage = 0;
  target.cageState = ECMD_TARGET_FIELD_WILDCARD;
  target.node = 0;
  target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
  target.slot = 0;
  target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipType = "fred";
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.pos = 0;
  target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitType = "";
  target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
  target.chipUnitNum = 0;
  target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;
  target.thread = 0;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  tgt_vector.push_back(target);
  num_tests++;

/*****************************************************************************/
  tmp_str= "Running 'fred' chipUnit Test";
  msg_vector.push_back(tmp_str);

  target.cage = 0;
  target.cageState = ECMD_TARGET_FIELD_WILDCARD;
  target.node = 0;
  target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
  target.slot = 0;
  target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipType = "pu";
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.pos = 0;
  target.posState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitType = "fred";
  target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
  target.chipUnitNum = 0;
  target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
  target.thread = 0;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  tgt_vector.push_back(target);
  num_tests++;


/*****************************************************************************/
  tmp_str= "Running Slot Test on 4";
  msg_vector.push_back(tmp_str);
  
  target.cage = 0;
  target.cageState = ECMD_TARGET_FIELD_WILDCARD;
  target.node = 0;
  target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
  target.slot = 4;
  target.slotState = ECMD_TARGET_FIELD_VALID;
  target.chipType = "";
  target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
  target.pos = 0;
  target.posState = ECMD_TARGET_FIELD_UNUSED;
  target.chipUnitType = "";
  target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
  target.chipUnitNum = 0;
  target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;
  target.thread = 0;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  tgt_vector.push_back(target);
  num_tests++;


/*****************************************************************************/
  tmp_str= "Running Node Test on 0";
  msg_vector.push_back(tmp_str);

  target.cage = 0;
  target.cageState = ECMD_TARGET_FIELD_WILDCARD;
  target.node = 0;
  target.nodeState = ECMD_TARGET_FIELD_VALID;
  target.slot = 0;
  target.slotState = ECMD_TARGET_FIELD_UNUSED;
  target.chipType = "";
  target.chipTypeState = ECMD_TARGET_FIELD_WILDCARD;
  target.pos = 0;
  target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitType = "";
  target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
  target.chipUnitNum = 0;
  target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;
  target.thread = 0;
  target.threadState = ECMD_TARGET_FIELD_WILDCARD;

  tgt_vector.push_back(target);
  num_tests++;


/*****************************************************************************/
  tmp_str= "Running Node Test on 10";

  msg_vector.push_back(tmp_str);
  target.cage = 0;
  target.cageState = ECMD_TARGET_FIELD_WILDCARD;
  target.node = 10;
  target.nodeState = ECMD_TARGET_FIELD_VALID;
  target.slot = 0;
  target.slotState = ECMD_TARGET_FIELD_UNUSED;
  target.chipType = "";
  target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
  target.pos = 0;
  target.posState = ECMD_TARGET_FIELD_UNUSED;
  target.chipUnitType = "";
  target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
  target.chipUnitNum = 0;
  target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;
  target.thread = 0;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  tgt_vector.push_back(target);
  num_tests++;

/*****************************************************************************/
  tmp_str= "Running NA node test - only valid if your config has one";

  msg_vector.push_back(tmp_str);
  target.cage = 0;
  target.cageState = ECMD_TARGET_FIELD_WILDCARD;
  target.node = ECMD_TARGETDEPTH_NA;
  target.nodeState = ECMD_TARGET_FIELD_VALID;
  target.slot = 0;
  target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipType = "";
  target.chipTypeState = ECMD_TARGET_FIELD_WILDCARD;
  target.pos = 0;
  target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitType = "";
  target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
  target.chipUnitNum = 0;
  target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;
  target.thread = 0;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  tgt_vector.push_back(target);
  num_tests++;

/*****************************************************************************/
  tmp_str= "Running FSP test";

  msg_vector.push_back(tmp_str);
  target.cage = 0;
  target.cageState = ECMD_TARGET_FIELD_WILDCARD;
  target.node = 0;
  target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
  target.slot = 0;
  target.slotState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipType = "fsp";
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.pos = 0;
  target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitType = "";
  target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
  target.chipUnitNum = 0;
  target.chipUnitNumState = ECMD_TARGET_FIELD_UNUSED;
  target.thread = 0;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  tgt_vector.push_back(target);
  num_tests++;


  /***************************************************************************/
  /*  ---------    Loop Through Testcases  ---------------                   */
  /***************************************************************************/
//  printf(">>> Start of ecmdQuery Config Test Loop ...\n");
  uint32_t i = 0; 
  for(i=0 ; i < num_tests ; i++)
  {
//    (o_queryData.cageData).clear(); // clear output
//    o_queryData.cageData->clear(); // clear output
//   o_queryData->cageData->clear(); // clear output
//    o_queryData->cageData.clear(); // clear output
    printf("\n-------------------------------------------------\n");
    printf("eQC Test %d: %s\n", i, msg_vector[i].c_str());
    printf("-------------------------------------------------\n");
    
    
    gettimeofday(&curTv, NULL);             //@SJ
    
    start = curTv.tv_sec*1000000 + (curTv.tv_usec);         //@SJ
    
    rc = ecmdQueryConfig(tgt_vector[i], o_queryData);
    
    gettimeofday(&curTv, NULL);         //@SJ
 
    stop = curTv.tv_sec*1000000 + (curTv.tv_usec);      //@SJ
    
    
    query_config_TotalCnt = query_config_TotalCnt + 1;    //@SJ                      
     
    printf("Time taken in ecmdQueryConfig for iteration no: %d -> %lf usecs\n",query_config_TotalCnt,(stop-start)); //@SJ
    
    totaltime = totaltime+ + (stop-start);      //@SJ
    
    
 
    if (rc != ECMD_SUCCESS)
    {
      printf("\n\nFAILURE!!! rc=0x%x\n\n", rc);
      printTgt(tgt_vector[i]);
    }
    else 
    {
      printf("\nrc=0x%x\n", rc);
      printTgt(tgt_vector[i]);
      printQueryData(o_queryData);
    }
    printf("\n-------------------------------------------------\n");
    printf("-------------------------------------------------\n\n");

  }
  printf("\n>>> ... End of ecmdQuery Config Test Loop.\n");



  /************************************************************************/
  /*  ---------     End of Testcase             ---------------           */
  /************************************************************************/

  ecmdUnloadDll();
  
  //@SJ
  printf("TotalTime and TotalCount of function ecmdQueryConfig = %lf usecs and %d\n",totaltime,query_config_TotalCnt);
  

  return rc;

}

