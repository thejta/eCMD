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
  if(strcmp(argv[0], "configd")) {
   rc = ecmdCommandArgs(&argc, &argv);
   if (rc) return rc;
  }
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
    char isCore = 'N';
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
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
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
        
      sprintf(buf,"\nAvailable rings for %s ec %X:\n", ecmdWriteTarget(target).c_str(), chipdata.chipEc); ecmdOutput(buf);
      printed = "Ring Names                           Address    Length Core Mask Chkable BroadSide ClockDomain         ClockState\n"; ecmdOutput(printed.c_str());
      printed = "-----------------------------------  --------   ------ ---- ---- ------- --------- ------------------- ----------\n"; ecmdOutput(printed.c_str());
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

        if (ringit->isCoreRelated) {
          isCore = 'Y';
        } else isCore = 'N';

        if (ringit->supportsBroadsideLoad) {
          broadmode = 'Y';
        } else broadmode = 'N';

        sprintf(buf,"0x%.6X\t%d\t %c   %c     %c         %c     %-20s", ringit->address, ringit->bitLength, isCore, invmask, chkable, broadmode,ringit->clockDomain.c_str());
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



    /* ----------- */
    /* spys        */
    /* ----------- */
  } else if (!strcmp(argv[0], "spys")) {

    char eccChk = 'N';
    char isEnum  = 'N';
    char isCore = 'N';
    std::list<ecmdSpyData> spydata;
    std::list<ecmdSpyData>::iterator spyit;
    std::list<std::string>::iterator strit;
    
    if (argc < 2) {
      ecmdOutputError("ecmdquery - Too few arguments specified for spys; you need at least a query spys <chipname>.\n");
      ecmdOutputError("ecmdquery - Type 'ecmdquery -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }

    ecmdChipData chipdata;

    //Setup the target that will be used to query the system config 
    ecmdChipTarget target;
    target.chipType = argv[1];
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
    target.threadState = target.coreState = ECMD_TARGET_FIELD_UNUSED;

    /************************************************************************/
    /* Kickoff Looping Stuff                                                */
    /************************************************************************/

    bool validPosFound = false;
    rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
    if (rc) return rc;

    char buf[200];

    while ( ecmdConfigLooperNext(target, looperdata) ) {

      rc = ecmdQuerySpy(target, spydata,argv[2]);
      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
      } else if (rc) {
        printed = "ecmdquery - Error occured performing spy query on ";
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
        
      sprintf(buf,"\nAvailable spys for %s ec %X:\n", ecmdWriteTarget(target).c_str(), chipdata.chipEc); ecmdOutput(buf);
      printed = "SpyType   BitLength EccChked	Enum  Core ClockDomain	      ClockState\n"; ecmdOutput(printed.c_str());
      printed = "-------   --------- ---------   ----- ---- ------------------- ----------\n"; ecmdOutput(printed.c_str());

      for (spyit = spydata.begin(); spyit != spydata.end(); spyit ++) {

        printed = "SpyName = {";
        printed += spyit->spyName; printed += "}\n";
        ecmdOutput(printed.c_str());
	
	printed = "";
        
        if(spyit->spyType == ECMD_SPYTYPE_ALIAS) {
          printed = "ALIAS";
        } else if(spyit->spyType == ECMD_SPYTYPE_IDIAL) {
          printed = "IDIAL";
        } else if(spyit->spyType == ECMD_SPYTYPE_EDIAL) {
          printed = "EDIAL";
        } else if(spyit->spyType == ECMD_SPYTYPE_ECCGROUP) {
          printed = "ECCGROUP";
        } 
        for (int i = printed.length(); i <= 10; i++) { 
          printed += " ";
        }

        if(spyit->isEccChecked) {
          eccChk = 'Y';
        } else {
          eccChk = 'N';
        }

        if (spyit->isEnumerated) {
          isEnum = 'Y';
        } else isEnum = 'N';

        if (spyit->isCoreRelated) {
          isCore = 'Y';
        } else isCore = 'N';

        sprintf(buf,"%-10d  %c          %c    %c   %-20s", spyit->bitLength, eccChk, isEnum, isCore,spyit->clockDomain.c_str());
        printed += buf;

        if (spyit->clockState == ECMD_CLOCKSTATE_UNKNOWN)
          printed += "UNKNOWN\n";
        else if (spyit->clockState == ECMD_CLOCKSTATE_ON)
          printed += "ON\n";
        else if (spyit->clockState == ECMD_CLOCKSTATE_OFF)
          printed += "OFF\n";
        else if (spyit->clockState == ECMD_CLOCKSTATE_NA)
          printed += "NA\n";
	  
        printed += "EpCheckers = {";
        for (strit = spyit->epCheckers.begin(); strit != spyit->epCheckers.end(); strit ++) {
          if (strit != spyit->epCheckers.begin()) printed += ", ";
         printed += (*strit);
        }
        printed += "}\n";
        
        #ifndef FIPSODE
         printed += "Enums = {";
         for (strit = spyit->enums.begin(); strit != spyit->enums.end(); strit ++) {
          if (strit != spyit->enums.begin()) printed += ", ";
          printed += (*strit);
         }
         printed += "}\n";
        #endif
        
	if (argv[2] == NULL) {
          printed += "\n";
        }
        ecmdOutput(printed.c_str());
      }
    }

    if (!validPosFound) {
      ecmdOutputError("ecmdquery - Unable to find a valid chip to execute command on\n");
      return ECMD_TARGET_NOT_CONFIGURED;
    }



    /* ----------- */
    /* tracearrays */
    /* ----------- */
  } else if (!strcmp(argv[0], "tracearrays")) {

    char isCore='N';
    
    std::list<ecmdTraceArrayData> tracearraydata;
    std::list<ecmdTraceArrayData>::iterator traceit;
    
    if (argc < 2) {
      ecmdOutputError("ecmdquery - Too few arguments specified for tracearrays; you need at least a query tracearrays <chipname>.\n");
      ecmdOutputError("ecmdquery - Type 'ecmdquery -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }

    ecmdChipData chipdata;

    //Setup the target that will be used to query the system config 
    ecmdChipTarget target;
    target.chipType = argv[1];
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
    target.threadState = target.coreState = ECMD_TARGET_FIELD_UNUSED;

    /************************************************************************/
    /* Kickoff Looping Stuff                                                */
    /************************************************************************/

    bool validPosFound = false;
    rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
    if (rc) return rc;

    char buf[200];

    while ( ecmdConfigLooperNext(target, looperdata) ) {

      rc = ecmdQueryTraceArray(target, tracearraydata,argv[2]);
      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
      } else if (rc) {
        printed = "ecmdquery - Error occured performing tracearray query on ";
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
        
      sprintf(buf,"\nAvailable tracearrays for %s ec %X:\n", ecmdWriteTarget(target).c_str(), chipdata.chipEc); ecmdOutput(buf);
      printed = "TraceArray Names          Length     Width   Core    ClockDomain         ClockState\n"; ecmdOutput(printed.c_str());
      printed = "------------------------  --------   ------  ------  ------------------- ----------\n"; ecmdOutput(printed.c_str());

      for (traceit = tracearraydata.begin(); traceit != tracearraydata.end(); traceit ++) {

        printed = "";
        printed += traceit->traceArrayName;
        
	for (int i = printed.length(); i <= 25; i++) { 
          printed += " ";
        }

        
        if(traceit->isCoreRelated) {
          isCore = 'Y';
        } else {
          isCore = 'N';
        }

        sprintf(buf,"%-11d%-8d  %c     %-20s", traceit->length, traceit->width, isCore, traceit->clockDomain.c_str());
        printed += buf;
	

        if (traceit->clockState == ECMD_CLOCKSTATE_UNKNOWN)
          printed += "UNKNOWN\n";
        else if (traceit->clockState == ECMD_CLOCKSTATE_ON)
          printed += "ON\n";
        else if (traceit->clockState == ECMD_CLOCKSTATE_OFF)
          printed += "OFF\n";
        else if (traceit->clockState == ECMD_CLOCKSTATE_NA)
          printed += "NA\n";

        ecmdOutput(printed.c_str());
      }
    }

    if (!validPosFound) {
      ecmdOutputError("ecmdquery - Unable to find a valid chip to execute command on\n");
      return ECMD_TARGET_NOT_CONFIGURED;
    }


    /* ----------- */
    /* scoms       */
    /* ----------- */
  } else if (!strcmp(argv[0], "scoms")) {

    char isCore = 'N';
    uint32_t address =0xFFFFFFFF;
    char addrStr[20];
    
    std::list<ecmdScomData> scomdata;
    std::list<ecmdScomData>::iterator scomit;

    if (argc < 2) {
      ecmdOutputError("ecmdquery - Too few arguments specified for scoms; you need at least a query scoms <chipname>.\n");
      ecmdOutputError("ecmdquery - Type 'ecmdquery -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }

    ecmdChipData chipdata;

    //Setup the target that will be used to query the system config 
    ecmdChipTarget target;
    target.chipType = argv[1];
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
    target.threadState = target.coreState = ECMD_TARGET_FIELD_UNUSED;

    /************************************************************************/
    /* Kickoff Looping Stuff                                                */
    /************************************************************************/

    bool validPosFound = false;
    rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
    if (rc) return rc;

    char buf[200];
    if (argv[2] != NULL) {
      address = strtoul(argv[2], NULL, 16);
    }
    while ( ecmdConfigLooperNext(target, looperdata) ) {

      rc = ecmdQueryScom(target, scomdata, address);
      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
      } else if (rc) {
        printed = "ecmdquery - Error occured performing scom query on ";
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
        
      sprintf(buf,"\nAvailable scoms for %s ec %X:\n", ecmdWriteTarget(target).c_str(), chipdata.chipEc); ecmdOutput(buf);
      printed = "Scom Address  Core    ClockDomain          ClockState\n"; ecmdOutput(printed.c_str());
      printed = "------------  ------  -------------------  ----------\n"; ecmdOutput(printed.c_str());

      for (scomit = scomdata.begin(); scomit != scomdata.end(); scomit ++) {

        printed = "";
        
        sprintf(addrStr, "%8.8X", scomit->address);
	printed += addrStr;
	
	for (int i = printed.length(); i <= 14; i++) { 
          printed += " ";
        }

        if(scomit->isCoreRelated) {
          isCore = 'Y';
        } else {
          isCore = 'N';
        }

        sprintf(buf," %c     %-21s", isCore,scomit->clockDomain.c_str());
        printed += (std::string)buf;
	

        if (scomit->clockState == ECMD_CLOCKSTATE_UNKNOWN)
          printed += "UNKNOWN\n";
        else if (scomit->clockState == ECMD_CLOCKSTATE_ON)
          printed += "ON\n";
        else if (scomit->clockState == ECMD_CLOCKSTATE_OFF)
          printed += "OFF\n";
        else if (scomit->clockState == ECMD_CLOCKSTATE_NA)
          printed += "NA\n";

        ecmdOutput(printed.c_str());
      }
    }

    if (!validPosFound) {
      ecmdOutputError("ecmdquery - Unable to find a valid chip to execute command on\n");
      return ECMD_TARGET_NOT_CONFIGURED;
    }



    /* ----------- */
    /* arrays      */
    /* ----------- */
  } else if (!strcmp(argv[0], "arrays")) {

    char isCore = 'N';
    
    std::list<ecmdArrayData> arraydata;
    std::list<ecmdArrayData>::iterator arrayit;

    if (argc < 2) {
      ecmdOutputError("ecmdquery - Too few arguments specified for arrays; you need at least a query arrays <chipname>.\n");
      ecmdOutputError("ecmdquery - Type 'ecmdquery -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }

    ecmdChipData chipdata;

    //Setup the target that will be used to query the system config 
    ecmdChipTarget target;
    target.chipType = argv[1];
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
    target.threadState = target.coreState = ECMD_TARGET_FIELD_UNUSED;

    /************************************************************************/
    /* Kickoff Looping Stuff                                                */
    /************************************************************************/

    bool validPosFound = false;
    rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
    if (rc) return rc;

    char buf[200];
    
    while ( ecmdConfigLooperNext(target, looperdata) ) {

      rc = ecmdQueryArray(target, arraydata, argv[2]);
      if (rc == ECMD_TARGET_NOT_CONFIGURED) {
        continue;
      } else if (rc) {
        printed = "ecmdquery - Error occured performing array query on ";
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
        
      sprintf(buf,"\nAvailable arrays for %s ec %X:\n", ecmdWriteTarget(target).c_str(), chipdata.chipEc); ecmdOutput(buf);
      
      printed = "Array Names                    RdAddrLen  WrtAddrLen Length Width Core ClockDomain         ClockState\n"; ecmdOutput(printed.c_str());
      printed = "------------------------------ ---------- ---------- ------ ----- ---- ------------------- ----------\n"; ecmdOutput(printed.c_str());

      for (arrayit = arraydata.begin(); arrayit != arraydata.end(); arrayit ++) {

        printed = "";
        printed += arrayit->arrayName;
	for (int i = printed.length(); i <= 30; i++) { 
          printed += " ";
        }

        if(arrayit->isCoreRelated) {
          isCore = 'Y';
        } else {
          isCore = 'N';
        }

        sprintf(buf,"%-10d %-10d %-6d %-5d  %c   %-20s", arrayit->readAddressLength, arrayit->writeAddressLength, arrayit->length, arrayit->width, isCore,arrayit->clockDomain.c_str());
        printed += (std::string)buf;
	

        if (arrayit->clockState == ECMD_CLOCKSTATE_UNKNOWN)
          printed += "UNKNOWN\n";
        else if (arrayit->clockState == ECMD_CLOCKSTATE_ON)
          printed += "ON\n";
        else if (arrayit->clockState == ECMD_CLOCKSTATE_OFF)
          printed += "OFF\n";
        else if (arrayit->clockState == ECMD_CLOCKSTATE_NA)
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

    

    //Setup the target that will be used to query the system config 
    ecmdChipTarget target;
    
    uint8_t ONE = 0;
    uint8_t MANY = 1;

    std::string cage;	
    std::string node;	
    std::string slot;	
    std::string pos;	
    std::string core;	
    
    uint8_t cageType;
    uint8_t nodeType;
    uint8_t slotType;
    uint8_t posType;
    uint8_t coreType;
  
    bool targetNotFound = false;
  
    std::list<uint32_t> 	     cageList;
    std::list<uint32_t>::iterator    cageListIter;
    std::list<uint32_t> 	     nodeList;
    std::list<uint32_t>::iterator    nodeListIter;
    std::list<uint32_t> 	     slotList;
    std::list<uint32_t>::iterator    slotListIter;
    std::list<uint32_t> 	     posList;
    std::list<uint32_t>::iterator    posListIter;
    std::list<uint32_t> 	     coreList;
    std::list<uint32_t>::iterator    coreListIter;
  
    /************************************************************************/
    /* Parse Common Cmdline Args                                            */
    /************************************************************************/
    
    if(ecmdParseOption(&argc, &argv, "-all")) {
       printed = "ecmdquery - 'all' option for targets not supported for configd option.\n";
       ecmdOutputError( printed.c_str() );
       return ECMD_INVALID_ARGS;
    }


    //Set all the states Unused to begin with. Then set them to valid based on the args
    target.cageState	 = ECMD_TARGET_FIELD_UNUSED;
    target.nodeState	 = ECMD_TARGET_FIELD_UNUSED;
    target.slotState	 = ECMD_TARGET_FIELD_UNUSED;
    target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
    target.posState	 = ECMD_TARGET_FIELD_UNUSED;
    target.coreState	 = ECMD_TARGET_FIELD_UNUSED;
    target.threadState   = ECMD_TARGET_FIELD_UNUSED;
  
    rc = ecmdParseTargetFields(&argc, &argv, "cage", target, cageType, cage);
    if(rc) return rc;
    rc = ecmdParseTargetFields(&argc, &argv, "node", target, nodeType, node);
    if(rc) return rc;
    rc = ecmdParseTargetFields(&argc, &argv, "slot", target, slotType, slot);
    if(rc) return rc;
    rc = ecmdParseTargetFields(&argc, &argv, "pos", target, posType, pos);
    if(rc) return rc;
    rc = ecmdParseTargetFields(&argc, &argv, "core", target, coreType, core);
    if(rc) return rc;
    
    //Thread
    char *targetPtr = ecmdParseOptionWithArgs(&argc, &argv, "-t");
    if(targetPtr != NULL) {
      ecmdOutputError("ecmdquery - Thread (-t#) argument not supported\n");
      return ECMD_INVALID_ARGS;
    }
    
    //Chiptype
    if (argc < 2) {
      ecmdOutputError("ecmdquery - Too few arguments specified for configd; you need at least a query configd <chipname>.\n");
      ecmdOutputError("ecmdquery - Type 'ecmdquery -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }
    else {
     target.chipType = argv[1];
     target.chipTypeState = ECMD_TARGET_FIELD_VALID;
     target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_FIELD_VALID;
    }
    
    //Go through each of the targets and check if they are configured  
    if (cageType == ONE) {
      cageList.push_back(target.cage);
    }
    else if (cageType == MANY) {
      getTargetList(cage, cageList);
    }
    if (cageList.empty()) {
      ecmdOutputError("ecmdquery - no cage input found.\n");
      return ECMD_INVALID_CONFIG;
    }
    /* STart walking the cages */
    for (cageListIter = cageList.begin(); cageListIter != cageList.end(); cageListIter++) {
       target.cage = *cageListIter;
 
     nodeList.clear();

     if (nodeType == ONE) {
       nodeList.push_back(target.node);
     }
     else if (nodeType == MANY) {
       getTargetList(node, nodeList);
     }
	
     for (nodeListIter = nodeList.begin(); nodeListIter != nodeList.end(); nodeListIter ++) {
     	target.node = *nodeListIter;
       
     	slotList.clear();
     	
     	if (slotType == ONE) {
     	   slotList.push_back(target.slot);
     	}
     	else if (slotType == MANY) {
     	   getTargetList(slot, slotList);
     	}

	 /* Walk the slots */
        for (slotListIter = slotList.begin(); slotListIter != slotList.end(); slotListIter ++) {
          target.slot = *slotListIter;
	  
	 /* Walk the slots */
         if (target.chipTypeState != ECMD_TARGET_FIELD_UNUSED) {
           
	  posList.clear();
          //ChipType has been previously set
	  if (posType == ONE) {
           posList.push_back(target.pos);
          }
          else if (posType == MANY) {
           getTargetList(pos, posList);
          }

	  /* Now start walking chip pos's */
	  for (posListIter = posList.begin(); posListIter != posList.end(); posListIter ++) {
	    target.pos = *posListIter;
	   
	    coreList.clear();

	    if (coreType == ONE) {
              coreList.push_back(target.core);
            }
            else if (coreType == MANY) {
              getTargetList(core, coreList);
            }
	    
	    /* Ok, walk the cores */
            for (coreListIter = coreList.begin(); coreListIter != coreList.end(); coreListIter ++) {
	      target.core = *coreListIter;

	      if (ecmdQueryTargetConfigured(target)) {
                 printed = "ecmdquery - Target ";
                 printed += ecmdWriteTarget(target);
                 printed += " is configured!\n";
                 ecmdOutput(printed.c_str());
	      }  else {
                 printed = "ecmdquery - Target ";
                 printed += ecmdWriteTarget(target);
                 printed += " is not configured!\n";
                 ecmdOutput(printed.c_str());
                 targetNotFound = true;
              }
	
	    }//Loop cores
	       
	  }//Loop pos 

	 }// end if chiptype Used
	 
	}// Loop slots 
	    
      } //Loop nodes 
      
    }// Loop cages
    if (targetNotFound) {
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
    std::string decode;                 ///< Number decode ie "[p]" or "[p:c]"

    /* Do they want to run in easy parse mode ? */
    bool easyParse = ecmdParseOption (&argc, &argv, "-ep");
    
    /* Figure out the depth they want */
    target.chipTypeState = target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
    if (ecmdParseOption (&argc, &argv, "-dt")) {
      target.coreState = target.threadState = ECMD_TARGET_FIELD_WILDCARD;
      decode = "[p:c:t]";
    } else if (ecmdParseOption (&argc, &argv, "-dc")) {
      target.coreState = ECMD_TARGET_FIELD_WILDCARD;
      target.threadState = ECMD_TARGET_FIELD_UNUSED;
      decode = "[p:c]";
    } else { /* -dp as well */
      target.threadState = target.coreState = ECMD_TARGET_FIELD_UNUSED;
      decode = "[p]";
    }

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
                if (ecmdCurChip->numProcCores != 0)
                  sprintf(buf,"      %s %s",ecmdCurChip->chipType.c_str(), decode.c_str());
                else
                  sprintf(buf,"      %s %s",ecmdCurChip->chipType.c_str(), "[p]");
              } else if (curchip != ecmdCurChip->chipType) {
                curchip = ecmdCurChip->chipType;
                strcat(buf, "\n"); ecmdOutput(buf);
                if (ecmdCurChip->numProcCores != 0)
                  sprintf(buf,"      %s %s",ecmdCurChip->chipType.c_str(), decode.c_str());
                else
                  sprintf(buf,"      %s %s",ecmdCurChip->chipType.c_str(), "[p]");
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

   
    target.chipTypeState = target.cageState = target.nodeState = target.slotState = target.posState = target.coreState =
    target.threadState = ECMD_TARGET_FIELD_WILDCARD;
    
    rc = ecmdQueryConfig(target, queryData, ECMD_QUERY_DETAIL_HIGH);

    char buf[500];
    char buf2[100];
    
    for (ecmdCurCage = queryData.cageData.begin(); ecmdCurCage != queryData.cageData.end(); ecmdCurCage ++) {
      
      sprintf(buf,"Cage %d\n",ecmdCurCage->cageId); ecmdOutput(buf);
      sprintf(buf,"  Details: CageUid=%8.8X\n",ecmdCurCage->unitId); ecmdOutput(buf);
            
      for (ecmdCurNode = ecmdCurCage->nodeData.begin(); ecmdCurNode != ecmdCurCage->nodeData.end(); ecmdCurNode ++) {
        
	sprintf(buf,"  Node %d\n",ecmdCurNode->nodeId ); ecmdOutput(buf);
        sprintf(buf,"    Details: NodeUid=%8.8X\n",ecmdCurNode->unitId ); ecmdOutput(buf);

        for (ecmdCurSlot = ecmdCurNode->slotData.begin(); ecmdCurSlot != ecmdCurNode->slotData.end(); ecmdCurSlot ++) {
          
	  sprintf(buf,"    Slot %d\n",ecmdCurSlot->slotId); ecmdOutput(buf); 
          sprintf(buf,"      Details: SlotUid=%8.8X\n",ecmdCurSlot->unitId); ecmdOutput(buf); buf[0] = '\0';

          for (ecmdCurChip = ecmdCurSlot->chipData.begin(); ecmdCurChip != ecmdCurSlot->chipData.end(); ecmdCurChip ++) {
	  
	    buf[0] = '\0';
	    //Common chip details
	    sprintf(buf2, "      %s %d\n",ecmdCurChip->chipType.c_str(), ecmdCurChip->pos);strcat(buf, buf2);
            sprintf(buf2, "        Details: PosUid=%8.8X, Name=%s, Short Name=%s, Common Name=%s,\n",ecmdCurChip->unitId, ecmdCurChip->chipType.c_str(),ecmdCurChip->chipShortType.c_str(),ecmdCurChip->chipCommonType.c_str());
	    strcat(buf, buf2);
            sprintf(buf2, "                 Pos=%d, NumProcCores=%d, EC=%X, Model EC=%X,\n",ecmdCurChip->pos,ecmdCurChip->numProcCores, ecmdCurChip->chipEc,ecmdCurChip->simModelEc );  
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
                sprintf(buf, "          Details: CoreUid=%8.8X\n", ecmdCurCore->unitId ); ecmdOutput(buf); 
		if ((ecmdCurCore->numProcThreads != 0) || !ecmdCurCore->threadData.empty()) {
                  /* For threaded chips */
                  for (ecmdCurThread = ecmdCurCore->threadData.begin(); ecmdCurThread != ecmdCurCore->threadData.end(); ecmdCurThread ++) {
                    sprintf(buf, "          Thread %d\n", ecmdCurThread->threadId );ecmdOutput(buf); 
                    sprintf(buf, "            Details: ThreadUid=%8.8X\n", ecmdCurThread->unitId );ecmdOutput(buf); 
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
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
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
      printed =     "Chip Short Name  : " + chipdata.chipShortType + "\n"; ecmdOutput(printed.c_str());
      printed =     "Chip Common Name : " + chipdata.chipCommonType + "\n"; ecmdOutput(printed.c_str());
      sprintf(buf,  "Chip Position    : %d\n", chipdata.pos); ecmdOutput(buf);
      sprintf(buf,  "Num Proc Cores   : %d\n", chipdata.numProcCores); ecmdOutput(buf);
      sprintf(buf,  "Chip EC Level    : %X\n", chipdata.chipEc); ecmdOutput(buf);
      sprintf(buf,  "Chip Model EC    : %X\n", chipdata.simModelEc); ecmdOutput(buf);
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
