// Copyright ***********************************************************
//                                                                      
// File ecmdQueryUser.C                                   
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 1996                                         
//                                                                      
// The source code for this program is not published or otherwise       
// divested of its trade secrets, irrespective of what has been         
// deposited with the U.S. Copyright Office.                            
//                                                                      
// End Copyright *******************************************************
/* $Header$ */

// Module Description **************************************************
//
// Description: 
//
// End Module Description **********************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define ecmdQueryUser_C
#include <stdio.h>

#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <ecmdSharedUtils.H>

#undef ecmdQueryUser_C
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


uint32_t ecmdQueryUser(int argc, char* argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  std::string printed;
  ecmdLooperData looperdata;            ///< Store internal Looper data


  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  if (argc == 0) {
    ecmdOutputError("ecmdquery - Too few arguments specified; you need at least a query mode.\n");
    ecmdOutputError("ecmdquery - Type 'ecmdquery -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }


  /* ---------- */
  /* rings      */
  /* ---------- */

  if (!strcmp(argv[0], "rings")) {

    char invmask = 'N';
    char chkable  = 'N';
    char broadmode = 'N';
    std::list<ecmdRingData> ringdata;
    std::list<ecmdRingData>::iterator ringit;
    std::list<std::string>::iterator strit;

    if (argc < 2) {
      ecmdOutputError("ecmdquery - Too few arguments specified for rings; you need at least a query rings <chipname>.\n");
      ecmdOutputError("ecmdquery - Type 'ecmdquery -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }

    ecmdChipData chipdata;

    //Setup the target that will be used to query the system config 
    ecmdChipTarget target;
    target.chipType = argv[1];
    target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_QUERY_WILDCARD;
    target.threadState = target.coreState = ECMD_TARGET_FIELD_UNUSED;

    /************************************************************************/
    /* Kickoff Looping Stuff                                                */
    /************************************************************************/

    bool validPosFound = false;
    rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
    if (rc) return rc;

    char buf[200];

    while ( ecmdConfigLooperNext(target, looperdata) ) {

      rc = ecmdQueryRing(target, ringdata,argv[2]);
      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
      } else if (rc) {
        printed = "ecmdquery - Error occured performing ring query on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }
      else {
        validPosFound = true;     
      }

      /* Let's look up other info about the chip, namely the ec level */
      rc = ecmdGetChipData (target, chipdata);
      if (rc) {
        printed = "ecmdquery - Unable to lookup ec information for chip ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }
        
      sprintf(buf,"\nAvailable rings for %s ec %d:\n", ecmdWriteTarget(target).c_str(), chipdata.chipEc); ecmdOutput(buf);
      printed = "Ring Names                           Address    Length   Mask Chkable BroadSide ClockDomain         ClockState\n"; ecmdOutput(printed.c_str());
      printed = "-----------------------------------  --------   ------   ---- ------- --------- ------------------- ----------\n"; ecmdOutput(printed.c_str());

      for (ringit = ringdata.begin(); ringit != ringdata.end(); ringit ++) {

        printed = "";
        /* The Ring Names */
        for (strit = ringit->ringNames.begin(); strit != ringit->ringNames.end(); strit ++) {
          if (strit != ringit->ringNames.begin()) printed += ", ";
          printed += (*strit);
        }
        for (int i = printed.length(); i <= 36; i++) { 
          printed += " ";
        }

        if(ringit->hasInversionMask) {
          invmask = 'Y';
        } else {
          invmask = 'N';
        }

        if (ringit->isCheckable) {
          chkable = 'Y';
        } else chkable = 'N';

        if (ringit->supportsBroadsideLoad) {
          broadmode = 'Y';
        } else broadmode = 'N';

        sprintf(buf,"0x%.6X\t%d\t  %c     %c         %c     %-20s", ringit->address, ringit->bitLength, invmask, chkable, broadmode,ringit->clockDomain.c_str());
        printed += buf;

        if (ringit->clockState == ECMD_CLOCKSTATE_UNKNOWN)
          printed += "UNKNOWN\n";
        else if (ringit->clockState == ECMD_CLOCKSTATE_ON)
          printed += "ON\n";
        else if (ringit->clockState == ECMD_CLOCKSTATE_OFF)
          printed += "OFF\n";
        else if (ringit->clockState == ECMD_CLOCKSTATE_NA)
          printed += "NA\n";

        ecmdOutput(printed.c_str());
      }
    }

    if (!validPosFound) {
      ecmdOutputError("ecmdquery - Unable to find a valid chip to execute command on\n");
      return ECMD_TARGET_NOT_CONFIGURED;
    }



    /* ---------- */
    /* version    */
    /* ---------- */
  } else if (!strcmp(argv[0],"version")) {

    rc = ecmdDisplayDllInfo();




    /* ---------- */
    /* configd    */
    /* ---------- */
  } else if (!strcmp(argv[0],"configd")) {

    if (argc < 2) {
      ecmdOutputError("ecmdquery - Too few arguments specified for configd; you need at least a query configd <chipname>.\n");
      ecmdOutputError("ecmdquery - Type 'ecmdquery -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }

    //Setup the target that will be used to query the system config 
    ecmdChipTarget target;
    target.chipType = argv[1];
    target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = target.posState = target.threadState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
    if (ecmdQueryTargetConfigured(target)) {
      printed = "ecmdquery - Target ";
      printed += ecmdWriteTarget(target);
      printed += " is configured!\n";
      ecmdOutput(printed.c_str());
    } else {
      printed = "ecmdquery - Target ";
      printed += ecmdWriteTarget(target);
      printed += " is not configured!\n";
      ecmdOutputError(printed.c_str());
      return ECMD_TARGET_NOT_CONFIGURED;
    }


    /* ---------- */
    /* chips      */
    /* ---------- */
  } else if (!strcmp(argv[0],"chips")) {

    //Setup the target that will be used to query the system config 
    ecmdQueryData queryData;            ///< Query data
    ecmdChipTarget target;              ///< Target to refine query
    std::list<ecmdCageData>::iterator ecmdCurCage;      ///< Iterators
    std::list<ecmdNodeData>::iterator ecmdCurNode;
    std::list<ecmdSlotData>::iterator ecmdCurSlot;
    std::list<ecmdChipData>::iterator ecmdCurChip;
    std::list<ecmdCoreData>::iterator ecmdCurCore;
    std::list<ecmdThreadData>::iterator ecmdCurThread;

    /* Do they want to run in easy parse mode ? */
    bool easyParse = ecmdParseOption (&argc, &argv, "-ep");
    


    target.chipTypeState = target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
    target.threadState = ECMD_TARGET_QUERY_IGNORE;

    rc = ecmdQueryConfig(target, queryData, ECMD_QUERY_DETAIL_HIGH);

    char buf[300];
    char buf2[10];
    std::string curchip, kbuf, nbuf, sbuf;

    for (ecmdCurCage = queryData.cageData.begin(); ecmdCurCage != queryData.cageData.end(); ecmdCurCage ++) {
      if (!easyParse) {
        sprintf(buf,"Cage %d\n",ecmdCurCage->cageId); ecmdOutput(buf);
      } else {
        sprintf(buf,"-k%d ",ecmdCurCage->cageId); kbuf = buf;
      }        
      for (ecmdCurNode = ecmdCurCage->nodeData.begin(); ecmdCurNode != ecmdCurCage->nodeData.end(); ecmdCurNode ++) {
        if (!easyParse) {
          sprintf(buf,"  Node %d\n",ecmdCurNode->nodeId); ecmdOutput(buf);
        } else {
          sprintf(buf,"-n%d ",ecmdCurNode->nodeId); nbuf = kbuf + buf;
        }

        for (ecmdCurSlot = ecmdCurNode->slotData.begin(); ecmdCurSlot != ecmdCurNode->slotData.end(); ecmdCurSlot ++) {
          if (!easyParse) {
            sprintf(buf,"    Slot %d\n",ecmdCurSlot->slotId); ecmdOutput(buf); buf[0] = '\0';
          } else {
            sprintf(buf,"-s%d ",ecmdCurSlot->slotId); sbuf = nbuf + buf;
          }

          curchip = "";
          for (ecmdCurChip = ecmdCurSlot->chipData.begin(); ecmdCurChip != ecmdCurSlot->chipData.end(); ecmdCurChip ++) {
            if (!easyParse) {
              if (curchip == "") {
                curchip = ecmdCurChip->chipType;
                sprintf(buf,"      %s",ecmdCurChip->chipType.c_str());
              } else if (curchip != ecmdCurChip->chipType) {
                strcat(buf, "\n"); ecmdOutput(buf);
                sprintf(buf,"      %s",ecmdCurChip->chipType.c_str());
              }
            }
            if ( (ecmdCurChip->numProcCores != 0) && ( !ecmdCurChip->coreData.empty() )) {
              for (ecmdCurCore = ecmdCurChip->coreData.begin(); ecmdCurCore != ecmdCurChip->coreData.end(); ecmdCurCore ++) {

                if ((ecmdCurCore->numProcThreads == 0) || ecmdCurCore->threadData.empty()) {
                  /* For non-threaded chips */
                  if (!easyParse) {
                    sprintf(buf2, " %d:%d,",ecmdCurChip->pos,ecmdCurCore->coreId);
                    strcat(buf, buf2);
                  } else {
                    sprintf(buf,"%s\t -p%02d -c%d\n", ecmdCurChip->chipType.c_str(), ecmdCurChip->pos, ecmdCurCore->coreId);
                    printed = sbuf + buf;
                    ecmdOutput(printed.c_str());
                  }
                } else {
                  for (ecmdCurThread = ecmdCurCore->threadData.begin(); ecmdCurThread != ecmdCurCore->threadData.end(); ecmdCurThread ++) {
                    if (!easyParse) {
                      sprintf(buf2, " %d:%d:%d,",ecmdCurChip->pos,ecmdCurCore->coreId,ecmdCurThread->threadId);
                      strcat(buf, buf2);
                    } else {
                      sprintf(buf,"%s\t -p%02d -c%d -t%d\n", ecmdCurChip->chipType.c_str(), ecmdCurChip->pos, ecmdCurCore->coreId, ecmdCurThread->threadId);
                      printed = sbuf + buf;
                      ecmdOutput(printed.c_str());
                    }

                  } /* curCoreIter */
                }

              } /* curCoreIter */
            }
	    else {
              /* For non-core chips OR For core-chips If core list is empty */
              if (!easyParse) {
                sprintf(buf2," %d,", ecmdCurChip->pos);
                strcat(buf, buf2);
              } else {
                sprintf(buf,"%s\t -p%02d\n", ecmdCurChip->chipType.c_str(), ecmdCurChip->pos);
                printed = sbuf + buf;
                ecmdOutput(printed.c_str());
              }
            } 

          } /* curChipIter */
          if (!easyParse && strlen(buf) > 0) {
              strcat(buf, "\n"); ecmdOutput(buf);
          }
        } /* curSlotIter */

      } /* curNodeIter */

    } /* curCageIter */


    /* ---------- */
    /* showconfig */
    /* ---------- */
  } else if (!strcmp(argv[0],"showconfig")) {

    //Setup the target that will be used to query the system config 
    ecmdQueryData queryData;            ///< Query data
    ecmdChipTarget target;              ///< Target to refine query
    std::list<ecmdCageData>::iterator ecmdCurCage;      ///< Iterators
    std::list<ecmdNodeData>::iterator ecmdCurNode;
    std::list<ecmdSlotData>::iterator ecmdCurSlot;
    std::list<ecmdChipData>::iterator ecmdCurChip;
    std::list<ecmdCoreData>::iterator ecmdCurCore;
    std::list<ecmdThreadData>::iterator ecmdCurThread;

   
    target.chipTypeState = target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_QUERY_WILDCARD;
    target.threadState = ECMD_TARGET_QUERY_IGNORE;

    rc = ecmdQueryConfig(target, queryData, ECMD_QUERY_DETAIL_HIGH);

    char buf[500];
    char buf2[100];
    
    for (ecmdCurCage = queryData.cageData.begin(); ecmdCurCage != queryData.cageData.end(); ecmdCurCage ++) {
      
      sprintf(buf,"Cage %d\n",ecmdCurCage->cageId); ecmdOutput(buf);
      sprintf(buf,"  Details: CageUid=%d\n",ecmdCurCage->unitId); ecmdOutput(buf);
            
      for (ecmdCurNode = ecmdCurCage->nodeData.begin(); ecmdCurNode != ecmdCurCage->nodeData.end(); ecmdCurNode ++) {
        
	sprintf(buf,"  Node %d\n",ecmdCurNode->nodeId ); ecmdOutput(buf);
        sprintf(buf,"    Details: NodeUid=%d\n",ecmdCurNode->unitId ); ecmdOutput(buf);

        for (ecmdCurSlot = ecmdCurNode->slotData.begin(); ecmdCurSlot != ecmdCurNode->slotData.end(); ecmdCurSlot ++) {
          
	  sprintf(buf,"    Slot %d\n",ecmdCurSlot->slotId); ecmdOutput(buf); 
          sprintf(buf,"      Details: SlotUid=%d\n",ecmdCurSlot->unitId); ecmdOutput(buf); buf[0] = '\0';

          for (ecmdCurChip = ecmdCurSlot->chipData.begin(); ecmdCurChip != ecmdCurSlot->chipData.end(); ecmdCurChip ++) {
	  
	    buf[0] = '\0';
	    //Common chip details
	    sprintf(buf2, "      %s %d\n",ecmdCurChip->chipType.c_str(), ecmdCurChip->pos);strcat(buf, buf2);
            sprintf(buf2, "        Details: PosUid=%d, Name=%s, Common Name=%s,\n",ecmdCurChip->unitId, ecmdCurChip->chipType.c_str(),ecmdCurChip->chipCommonType.c_str());
	    sprintf(buf2, "        Details: Name=%s, Common Name=%s,\n", ecmdCurChip->chipType.c_str(),ecmdCurChip->chipCommonType.c_str());
	    strcat(buf, buf2);
            sprintf(buf2, "                 Pos=%d, NumProcCores=%d, EC=%d, Model EC=%d,\n",ecmdCurChip->pos,ecmdCurChip->numProcCores, ecmdCurChip->chipEc,ecmdCurChip->simModelEc );  
	    strcat(buf, buf2);
	    if (ecmdCurChip->interfaceType == ECMD_INTERFACE_ACCESS)
              sprintf(buf2,"                 Interface=ACCESS, ");
            else if (ecmdCurChip->interfaceType == ECMD_INTERFACE_CFAM)
              sprintf(buf2,"                 Interface=CFAM, ");
            else
              sprintf(buf2,"                 Interface=UNKNOWN, ");  
	    strcat(buf, buf2);
            sprintf(buf2,  "Flags=0x%.08X\n", ecmdCurChip->chipFlags); strcat(buf, buf2);
	    ecmdOutput(buf);   buf[0] = '\0';
	    	  
            if ( (ecmdCurChip->numProcCores != 0) && ( !ecmdCurChip->coreData.empty() )) {
              for (ecmdCurCore = ecmdCurChip->coreData.begin(); ecmdCurCore != ecmdCurChip->coreData.end(); ecmdCurCore ++) {
                sprintf(buf, "        Core %d\n", ecmdCurCore->coreId ); ecmdOutput(buf); 
                sprintf(buf, "          Details: CoreUid=%d\n", ecmdCurCore->unitId ); ecmdOutput(buf); 
		if ((ecmdCurCore->numProcThreads != 0) || !ecmdCurCore->threadData.empty()) {
                  /* For threaded chips */
                  for (ecmdCurThread = ecmdCurCore->threadData.begin(); ecmdCurThread != ecmdCurCore->threadData.end(); ecmdCurThread ++) {
                    sprintf(buf, "          Thread %d\n", ecmdCurThread->threadId );ecmdOutput(buf); 
                    sprintf(buf, "            Details: ThreadUid=%d\n", ecmdCurThread->unitId );ecmdOutput(buf); 
                  } 
                }

              } /* curCoreIter */
            }

          } /* curChipIter */
              
        } /* curSlotIter */

      } /* curNodeIter */

    } /* curCageIter */


    /* ---------- */
    /* chipinfo   */
    /* ---------- */
  } else if (!strcmp(argv[0],"chipinfo")) {
    if (argc < 2) {
      ecmdOutputError("ecmdquery - Too few arguments specified for chipinfo; you need at least a query chipinfo <chipname>.\n");
      ecmdOutputError("ecmdquery - Type 'ecmdquery -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }

    ecmdChipData chipdata;

    //Setup the target that will be used to query the system config 
    ecmdChipTarget target;
    target.chipType = argv[1];
    target.chipTypeState = ECMD_TARGET_QUERY_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_QUERY_WILDCARD;
    target.threadState = target.coreState = ECMD_TARGET_FIELD_UNUSED;

    bool validPosFound = false;
    rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
    if (rc) return rc;

    char buf[200];

    while ( ecmdConfigLooperNext(target, looperdata) ) {

      /* Let's look up other info about the chip, namely the ec level */
      rc = ecmdGetChipData (target, chipdata);
      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
      } else if (rc) {
        printed = "ecmdquery - Error occured performing chipinfo query on ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }
      else {
        validPosFound = true;     
      }

      /* Ok, display what we got */
      ecmdOutput(   "*******************************************************\n");
      printed =     "Target           : " + ecmdWriteTarget(target) + "\n"; ecmdOutput(printed.c_str());
      printed =     "Chip Name        : " + chipdata.chipType + "\n"; ecmdOutput(printed.c_str());
      printed =     "Chip Common Name : " + chipdata.chipCommonType + "\n"; ecmdOutput(printed.c_str());
      sprintf(buf,  "Chip Position    : %d\n", chipdata.pos); ecmdOutput(buf);
      sprintf(buf,  "Num Proc Cores   : %d\n", chipdata.numProcCores); ecmdOutput(buf);
      sprintf(buf,  "Chip EC Level    : %d\n", chipdata.chipEc); ecmdOutput(buf);
      sprintf(buf,  "Chip Model EC    : %d\n", chipdata.simModelEc); ecmdOutput(buf);
      if (chipdata.interfaceType == ECMD_INTERFACE_ACCESS)
        sprintf(buf,"Chip Interface   : ACCESS\n");
      else if (chipdata.interfaceType == ECMD_INTERFACE_CFAM)
        sprintf(buf,"Chip Interface   : CFAM\n");
      else
        sprintf(buf,"Chip Interface   : UNKNOWN\n");
      ecmdOutput(buf);

      sprintf(buf,  "Chip Flags       : %.08X\n", chipdata.chipFlags); ecmdOutput(buf);
      ecmdOutput(   "*******************************************************\n");

    }

    /* ---------- */
    /* formats    */
    /* ---------- */
  } else if (!strcmp(argv[0],"formats")) {
    /* We will just print this from the format helpfile */
    return ecmdPrintHelp("format");



  } else {
    /* Invalid Query Mode */
    ecmdOutputError("ecmdquery - Invalid Query Mode.\n");
    ecmdOutputError("ecmdquery - Type 'ecmdquery -h' for usage.\n");
    return ECMD_INVALID_ARGS;
    

  }


  return rc;
}






// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//
// End Change Log *****************************************************
