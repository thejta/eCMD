/* $Header$ */
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

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <stdio.h>

#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <ecmdSharedUtils.H>

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
  if(argv[0] != NULL && strcmp(argv[0], "configd")) {
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
    std::string isChipUnit = "N";
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

    while (ecmdConfigLooperNext(target, looperdata)) {

      rc = ecmdQueryRing(target, ringdata,argv[2]);
      if (rc) {
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
      rc = ecmdGetChipData(target, chipdata);
      if (rc) {
        printed = "ecmdquery - Unable to lookup ec information for chip ";
        printed += ecmdWriteTarget(target);
        printed += "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
      }
        
      sprintf(buf,"\nAvailable rings for %s ec %X:\n", ecmdWriteTarget(target).c_str(), chipdata.chipEc); ecmdOutput(buf);
      printed = "Ring Names                          Address     Length ChipUnit Mask Check Broad ClockDomain       ClockState\n"; ecmdOutput(printed.c_str());
      printed = "----------------------------------- ----------  ------ -------- ---- ----- ----- ----------------- ----------\n"; ecmdOutput(printed.c_str());
      for (ringit = ringdata.begin(); ringit != ringdata.end(); ringit ++) {

        printed = "";
        /* The Ring Names */
        for (strit = ringit->ringNames.begin(); strit != ringit->ringNames.end(); strit ++) {
          if (strit != ringit->ringNames.begin()) printed += ", ";
          printed += (*strit);
        }
        for (size_t i = printed.length(); i <= 35; i++) { 
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

        if (ringit->isChipUnitRelated) {
          if (ringit->relatedChipUnit != "") {
            isChipUnit = ringit->relatedChipUnit;
          } else {
            isChipUnit = "Y";
          }
        } else {
          isChipUnit = "N";
        }

        if (ringit->supportsBroadsideLoad) {
          broadmode = 'Y';
        } else broadmode = 'N';

        sprintf(buf,"0x%.8X  %6d %8s %4c %5c %5c %-18s", ringit->address, ringit->bitLength, isChipUnit.c_str(), invmask, chkable, broadmode,ringit->clockDomain.c_str());
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
#ifndef ECMD_REMOVE_SPY_FUNCTIONS
  } else if (!strcmp(argv[0], "spys")) {

    char eccChk = 'N';
    char isEnum  = 'N';
    std::string isChipUnit = "N";
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

    while (ecmdConfigLooperNext(target, looperdata)) {

      rc = ecmdQuerySpy(target, spydata,argv[2]);
      if (rc) {
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
      printed = "SpyType   BitLength EccChked Enum ChipUnit ClockDomain         ClockState\n"; ecmdOutput(printed.c_str());
      printed = "-------   --------- -------- ---- -------- ------------------- ----------\n"; ecmdOutput(printed.c_str());

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
        for (size_t i = printed.length(); i <= 9; i++) { 
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

        if (spyit->isChipUnitRelated) {
          if (spyit->relatedChipUnit != "") {
            isChipUnit = spyit->relatedChipUnit;
          } else {
            isChipUnit = "Y";
          }
        } else {
          isChipUnit = "N";
        }

        sprintf(buf,"%-9d %8c %4c %8s %-20s", spyit->bitLength, eccChk, isEnum, isChipUnit.c_str(),spyit->clockDomain.c_str());
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
#endif // ECMD_REMOVE_SPY_FUNCTIONS


    /* ----------- */
    /* tracearrays */
    /* ----------- */
#ifndef ECMD_REMOVE_TRACEARRAY_FUNCTIONS
  } else if (!strcmp(argv[0], "tracearrays")) {

    std::string isChipUnit = "N";
    
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

    while (ecmdConfigLooperNext(target, looperdata)) {

      rc = ecmdQueryTraceArray(target, tracearraydata,argv[2]);
      if (rc) {
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
      printed = "TraceArray Names         Length   Width ChipUnit ClockDomain         ClockState\n"; ecmdOutput(printed.c_str());
      printed = "------------------------ -------- ----- -------- ------------------- ----------\n"; ecmdOutput(printed.c_str());

      for (traceit = tracearraydata.begin(); traceit != tracearraydata.end(); traceit ++) {

        printed = "";
        printed += traceit->traceArrayName;
        
	for (size_t i = printed.length(); i <= 24; i++) { 
          printed += " ";
        }

        
        if (traceit->isChipUnitRelated) {
          if (traceit->relatedChipUnit != "") {
            isChipUnit = traceit->relatedChipUnit;
          } else {
            isChipUnit = "Y";
          }
        } else {
          isChipUnit = "N";
        }

        sprintf(buf,"%-8d %-5d %8s %-20s", traceit->length, traceit->width, isChipUnit.c_str(), traceit->clockDomain.c_str());
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
#endif // ECMD_REMOVE_TRACE_ARRAY_FUNCTIONS

    /* ----------- */
    /* scoms       */
    /* ----------- */
  } else if (!strcmp(argv[0], "scoms")) {

    std::string isChipUnit = "N";
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
    while (ecmdConfigLooperNext(target, looperdata)) {

      rc = ecmdQueryScom(target, scomdata, address);
      if (rc) {
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
      printed = "Scom Address  ChipUnit  Length  ClockDomain          ClockState\n"; ecmdOutput(printed.c_str());
      printed = "------------  --------  ------  -------------------  ----------\n"; ecmdOutput(printed.c_str());

      for (scomit = scomdata.begin(); scomit != scomdata.end(); scomit ++) {

        printed = "";
        
        sprintf(addrStr, "%8.8X", scomit->address);
	printed += addrStr;
	
	for (size_t i = printed.length(); i <= 13; i++) { 
          printed += " ";
        }

        if (scomit->isChipUnitRelated) {
          if (scomit->relatedChipUnit != "") {
            isChipUnit = scomit->relatedChipUnit;
          } else {
            isChipUnit = "Y";
          }
        } else {
          isChipUnit = "N";
        }

        sprintf(buf,"%8s  %-7d %-21s", isChipUnit.c_str() ,scomit->length, scomit->clockDomain.c_str());
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
#ifndef ECMD_REMOVE_ARRAY_FUNCTIONS
  } else if (!strcmp(argv[0], "arrays")) {

    std::string isChipUnit = "N";
    
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
    
    while (ecmdConfigLooperNext(target, looperdata)) {

      rc = ecmdQueryArray(target, arraydata, argv[2]);
      if (rc) {
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
      
      printed = "Array Names                    ArrayType          RdAddrLen  WrtAddrLen Length           Width ChipUnit ClockDomain         ClockState\n"; ecmdOutput(printed.c_str());
      printed = "------------------------------ ------------------ ---------- ---------- ---------------- ----- -------- ------------------- ----------\n"; ecmdOutput(printed.c_str());

      for (arrayit = arraydata.begin(); arrayit != arraydata.end(); arrayit ++) {

        printed = "";
        printed += arrayit->arrayName;
	for (size_t i = printed.length(); i <= 30; i++) { 
          printed += " ";
        }

        if (arrayit->arrayType == ECMD_ARRAYTYPE_UNKNOWN)
          printed += "UNKNOWN            ";
        else if (arrayit->arrayType == ECMD_ARRAYTYPE_DIRECT_ACCESS)
          printed += "DIRECT_ACCESS      ";
        else if (arrayit->arrayType == ECMD_ARRAYTYPE_SIMPLE)
          printed += "SIMPLE             ";
        else if (arrayit->arrayType == ECMD_ARRAYTYPE_HARDWARE_ASSIST)
          printed += "HARDWARE_ASSIST    ";
        else if (arrayit->arrayType == ECMD_ARRAYTYPE_HARDWARE_ASSIST_2)
          printed += "HARDWARE_ASSIST_2  ";
        else 
          printed += "** ERROR **        ";


        if(arrayit->isChipUnitRelated) {
          if (arrayit->relatedChipUnit != "") {
            isChipUnit = arrayit->relatedChipUnit;
          } else {
            isChipUnit = "Y";
          }
        } else {
          isChipUnit = "N";
        }

        sprintf(buf,"%-10d %-10d %-16lld %-5d %-8s %-20s", arrayit->readAddressLength, arrayit->writeAddressLength, (unsigned long long)arrayit->length, arrayit->width, isChipUnit.c_str(),arrayit->clockDomain.c_str());
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
#endif // ECMD_REMOVE_ARRAY_FUNCTIONS


    /* ---------- */
    /* version    */
    /* ---------- */
  } else if (!strcmp(argv[0],"version")) {

    rc = ecmdDisplayDllInfo();




    /* ---------- */
    /* configd    */
    /* ---------- */
  } else if (!strcmp(argv[0],"configd") || !strcmp(argv[0],"exist")) {

    // Note: ecmdConfigLooperInit/Next is not used here because they only return targets in the config, 
    //  but this cmd requires keeping track of each target the user requests to be checked

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
    std::list<uint32_t>              chipUnitList;
    std::list<uint32_t>::iterator    chipUnitListIter;
  
    /************************************************************************/
    /* Parse Common Cmdline Args                                            */
    /************************************************************************/
    
    if(ecmdParseOption(&argc, &argv, "-all")) {
       printed = "ecmdquery - 'all' option for targets not supported for configd option.\n";
       ecmdOutputError( printed.c_str() );
       return ECMD_INVALID_ARGS;
    }


    //Set all the states Unused to begin with. Then set them to valid based on the args
    target.cageState            = ECMD_TARGET_FIELD_UNUSED;
    target.nodeState            = ECMD_TARGET_FIELD_UNUSED;
    target.slotState            = ECMD_TARGET_FIELD_UNUSED;
    target.chipTypeState        = ECMD_TARGET_FIELD_UNUSED;
    target.posState             = ECMD_TARGET_FIELD_UNUSED;
    target.chipUnitTypeState	= ECMD_TARGET_FIELD_UNUSED;
    target.chipUnitNumState	= ECMD_TARGET_FIELD_UNUSED;
    target.threadState          = ECMD_TARGET_FIELD_UNUSED;
  
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
      ecmdOutputError("ecmdquery - Too few arguments specified for configd; you need at least 'ecmdquery configd <chipname>'.\n");
      ecmdOutputError("ecmdquery - Type 'ecmdquery -h' for usage.\n");
      return ECMD_INVALID_ARGS;
    }
    else {
      std::string chipType, chipUnitType;
      ecmdParseChipField(argv[1], chipType, chipUnitType);
      target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_VALID;
      target.chipType = chipType;
      target.chipTypeState = ECMD_TARGET_FIELD_VALID;
      if (chipUnitType != "") {
        target.chipUnitType = chipUnitType;
        target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
        target.chipUnitNumState = ECMD_TARGET_FIELD_VALID;
      }
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
	   
	    chipUnitList.clear();

	    if (coreType == ONE) {
              chipUnitList.push_back(target.chipUnitNum);
            }
            else if (coreType == MANY) {
              getTargetList(core, chipUnitList);
            }
	    
	    /* Ok, walk the cores */
            for (chipUnitListIter = chipUnitList.begin(); chipUnitListIter != chipUnitList.end(); chipUnitListIter ++) {
	      target.chipUnitNum = ((uint8_t)*chipUnitListIter);

              if (!strcmp(argv[0],"configd")) {
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
              } else {
                if (ecmdQueryTargetExist(target)) {
                  printed = "ecmdquery - Target ";
                  printed += ecmdWriteTarget(target);
                  printed += " does exist!\n";
                  ecmdOutput(printed.c_str());
                }  else {
                  printed = "ecmdquery - Target ";
                  printed += ecmdWriteTarget(target);
                  printed += " does not exist!\n";
                  ecmdOutput(printed.c_str());
                  targetNotFound = true;
                }
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
    ecmdQueryData chipUnitQueryData;            ///< Query data for -dall
    ecmdQueryData threadQueryData;            ///< Query data for -dall
    std::list<ecmdChipUnitData>::iterator ecmdBeginChipUnit;
    std::list<ecmdChipUnitData>::iterator ecmdEndChipUnit;
    std::list<ecmdThreadData>::iterator ecmdBeginThread;
    std::list<ecmdThreadData>::iterator ecmdEndThread;

    ecmdChipTarget target;              ///< Target to refine query
    std::list<ecmdCageData>::iterator ecmdCurCage;      ///< Iterators
    std::list<ecmdNodeData>::iterator ecmdCurNode;
    std::list<ecmdSlotData>::iterator ecmdCurSlot;
    std::list<ecmdChipData>::iterator ecmdCurChip;
    std::list<ecmdChipUnitData>::iterator ecmdCurChipUnit;
    std::list<ecmdThreadData>::iterator ecmdCurThread;
    bool doAll = false;

    /* Do they want to run in easy parse mode ? */
    bool easyParse = ecmdParseOption (&argc, &argv, "-ep");

    /* Figure out the depth they want */
    target.cageState = target.nodeState = target.slotState = target.chipTypeState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
    /* Default is -dp, so set to unknown */
    target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;
    /* Now see if the user wants a different level */
    if (ecmdParseOption (&argc, &argv, "-dall")) {
      doAll = true;
    } else if (ecmdParseOption (&argc, &argv, "-dt")) {
      target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_WILDCARD;
    } else if (ecmdParseOption (&argc, &argv, "-dc")) {
      target.chipUnitTypeState = target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
      target.threadState = ECMD_TARGET_FIELD_UNUSED;
    }

    rc = ecmdQueryConfigSelected(target, queryData, ECMD_SELECTED_TARGETS_LOOP_DEFALL);

    /* Use as many STL buffers as possible so we don't keep blowing char buffers and core dumping */
    char buf[300];
    std::string kbuf, nbuf, sbuf, cbuf;
    uint8_t DEPTH_NONE = 0;
    uint8_t DEPTH_CHIP = 1;
    uint8_t DEPTH_POS = 2;
    uint8_t DEPTH_CUTYPE = 3;
    uint8_t DEPTH_THREAD = 4;
    uint8_t curDepth = DEPTH_NONE;
    std::string prevChip, prevChipUnit;

    for (ecmdCurCage = queryData.cageData.begin(); ecmdCurCage != queryData.cageData.end(); ecmdCurCage ++) {
      if (!easyParse) {
        sprintf(buf,"cage %d\n",ecmdCurCage->cageId); ecmdOutput(buf);
      } else {
        sprintf(buf,"-k%d ",ecmdCurCage->cageId); kbuf = buf;
      }        
      for (ecmdCurNode = ecmdCurCage->nodeData.begin(); ecmdCurNode != ecmdCurCage->nodeData.end(); ecmdCurNode ++) {
        if (!easyParse) {
          if (ecmdCurNode->nodeId == ECMD_TARGETDEPTH_NA) {
            sprintf(buf,"  node NA\n"); ecmdOutput(buf);
          } else {
            sprintf(buf,"  node %d\n",ecmdCurNode->nodeId); ecmdOutput(buf);
          }
        } else {
          if (ecmdCurNode->nodeId == ECMD_TARGETDEPTH_NA) {
            sprintf(buf,"-n- "); nbuf = kbuf + buf;
          } else {
            sprintf(buf,"-n%d ",ecmdCurNode->nodeId); nbuf = kbuf + buf;
          }
        }

        for (ecmdCurSlot = ecmdCurNode->slotData.begin(); ecmdCurSlot != ecmdCurNode->slotData.end(); ecmdCurSlot ++) {
          if (!easyParse) {
            if (ecmdCurSlot->slotId == ECMD_TARGETDEPTH_NA) {
              sprintf(buf,"    slot NA\n"); ecmdOutput(buf);
            } else {
              sprintf(buf,"    slot %d\n",ecmdCurSlot->slotId); ecmdOutput(buf);
            }
          } else {
            if (ecmdCurSlot->slotId == ECMD_TARGETDEPTH_NA) {
              sprintf(buf,"-s%-4s ", "-"); sbuf = nbuf + buf;
            } else {
              sprintf(buf,"-s%-4d ",ecmdCurSlot->slotId); sbuf = nbuf + buf;
            }
          }

          prevChip = "";
          for (ecmdCurChip = ecmdCurSlot->chipData.begin(); ecmdCurChip != ecmdCurSlot->chipData.end(); ecmdCurChip++) {
            if (!easyParse) {
              /* Print out the chip name if we are switching chips */
              if (prevChip != ecmdCurChip->chipType) {
                if (prevChip != "") {
                  // Pull off a comma out there and then close it up
                  cbuf.erase((cbuf.length() - 1), 1);
                  cbuf += "]\n";
                  ecmdOutput(cbuf.c_str());
                  cbuf.clear(); // make sure we reset
                }
                prevChip = ecmdCurChip->chipType;
                sprintf(buf,"      %s\n",ecmdCurChip->chipType.c_str());
                ecmdOutput(buf);
                curDepth = DEPTH_CHIP;
              }
            }
            if (doAll || !ecmdCurChip->chipUnitData.empty()) {
              /* In doAll mode, we are attempting to tunnel all the way down so we need to re-run the query at the chipUnit level to see if we have chipUnits */
              if (doAll) {
                target.cage = ecmdCurCage->cageId;
                target.cageState = ECMD_TARGET_FIELD_VALID;
                target.node = ecmdCurNode->nodeId;
                target.nodeState = ECMD_TARGET_FIELD_VALID;
                target.slot = ecmdCurSlot->slotId;
                target.slotState = ECMD_TARGET_FIELD_VALID;
                target.chipType = ecmdCurChip->chipType;
                target.chipTypeState = ECMD_TARGET_FIELD_VALID;
                target.pos = ecmdCurChip->pos;
                target.posState = ECMD_TARGET_FIELD_VALID;
                target.chipUnitTypeState = target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
                target.threadState = ECMD_TARGET_FIELD_UNUSED;

                rc = ecmdQueryConfigSelected(target, chipUnitQueryData, ECMD_SELECTED_TARGETS_LOOP_DEFALL);
                /* If it's empty list, we don't have chipUnits and need to return back */
                if (chipUnitQueryData.cageData.empty()) {
                  /* For non-chipUnit chips OR For chipUnit-chips If chipUnit list is empty */
                  if (!easyParse) {
                    if (curDepth != DEPTH_POS) {
                      if (curDepth > DEPTH_POS) {
                        // Pull off a comma out there and then close it up
                        cbuf.erase((cbuf.length() - 1), 1);
                        cbuf += "]\n";
                      }
                      /* Start the new line */
                      cbuf += "        p[";
                    }
                    sprintf(buf,"%d,", ecmdCurChip->pos);
                    cbuf += buf;
                    curDepth = DEPTH_POS;
                  } else {
                    sprintf(buf,"%-15s -p%02d\n", ecmdCurChip->chipType.c_str(), ecmdCurChip->pos);
                    printed = sbuf + buf;
                    ecmdOutput(printed.c_str());
                  }
                  continue;
                } else {
                  ecmdBeginChipUnit = chipUnitQueryData.cageData.begin()->nodeData.begin()->slotData.begin()->chipData.begin()->chipUnitData.begin();
                  ecmdEndChipUnit = chipUnitQueryData.cageData.begin()->nodeData.begin()->slotData.begin()->chipData.begin()->chipUnitData.end();
                }
              } else {
                ecmdBeginChipUnit = ecmdCurChip->chipUnitData.begin();
                ecmdEndChipUnit = ecmdCurChip->chipUnitData.end();
              }

              if (!easyParse && (ecmdBeginChipUnit != ecmdEndChipUnit)) {
                if (curDepth > DEPTH_CHIP) {
                  // Pull off a comma out there and then close it up
                  cbuf.erase((cbuf.length() - 1), 1);
                  cbuf += "]\n";
                }
                sprintf(buf,"        p[%d]", ecmdCurChip->pos);
                cbuf += buf;
                curDepth = DEPTH_POS;
              }

              prevChipUnit = "jta"; // Can't be initialized to "" because that is valid for P6/Z6, use my initials
              for (ecmdCurChipUnit = ecmdBeginChipUnit; ecmdCurChipUnit != ecmdEndChipUnit; ecmdCurChipUnit++) {

                if (!easyParse) {
                  if (prevChipUnit != ecmdCurChipUnit->chipUnitType || (curDepth > DEPTH_CUTYPE)) {
                    // Pull off a comma out there and then close it up
                    cbuf.erase((cbuf.length() - 1), 1);
                    cbuf += "]\n";
                    prevChipUnit = ecmdCurChipUnit->chipUnitType;
                    sprintf(buf,"          %s[",((ecmdCurChipUnit->chipUnitType == "") ? "c" : ecmdCurChipUnit->chipUnitType.c_str()));
                    cbuf += buf;
                    curDepth = DEPTH_CUTYPE;
                  }
                }

                if (doAll || !ecmdCurChipUnit->threadData.empty()) {
                  /* In doAll mode, we are attempting to tunnel all the way down so we need to re-run the query at the thread level to see if we have threads */
                  if (doAll) {
                    target.cage = ecmdCurCage->cageId;
                    target.cageState = ECMD_TARGET_FIELD_VALID;
                    target.node = ecmdCurNode->nodeId;
                    target.nodeState = ECMD_TARGET_FIELD_VALID;
                    target.slot = ecmdCurSlot->slotId;
                    target.slotState = ECMD_TARGET_FIELD_VALID;
                    target.chipType = ecmdCurChip->chipType;
                    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
                    target.pos = ecmdCurChip->pos;
                    target.posState = ECMD_TARGET_FIELD_VALID;
                    if (ecmdCurChipUnit->chipUnitType != "") {
                      target.chipUnitType = ecmdCurChipUnit->chipUnitType;
                      target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
                    } else {
                      target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
                    }
                    target.chipUnitNum = ecmdCurChipUnit->chipUnitNum;
                    target.chipUnitNumState = ECMD_TARGET_FIELD_VALID;
                    target.threadState = ECMD_TARGET_FIELD_WILDCARD;

                    rc = ecmdQueryConfigSelected(target, threadQueryData, ECMD_SELECTED_TARGETS_LOOP_DEFALL);
                    /* If it's empty list, we don't have threads and need to return back */
                    if (threadQueryData.cageData.empty()) {
                      /* For non-threaded chips */
                      if (!easyParse) {
                        sprintf(buf, "%d,", ecmdCurChipUnit->chipUnitNum);
                        cbuf += buf;
                      } else {
                        /* We need to figure out if we should add on a chipUnit */
                        std::string fullChipType = ecmdCurChip->chipType.c_str();
                        if (ecmdCurChipUnit->chipUnitType != "") {
                          fullChipType += "." + ecmdCurChipUnit->chipUnitType;
                        }
                        sprintf(buf,"%-15s -p%02d -c%d\n", fullChipType.c_str(), ecmdCurChip->pos, ecmdCurChipUnit->chipUnitNum);
                        printed = sbuf + buf;
                        ecmdOutput(printed.c_str());
                      }
                      continue;
                    } else {
                      ecmdBeginThread = threadQueryData.cageData.begin()->nodeData.begin()->slotData.begin()->chipData.begin()->chipUnitData.begin()->threadData.begin();
                      ecmdEndThread = threadQueryData.cageData.begin()->nodeData.begin()->slotData.begin()->chipData.begin()->chipUnitData.begin()->threadData.end();
                    }
                  } else {
                    ecmdBeginThread = ecmdCurChipUnit->threadData.begin();
                    ecmdEndThread = ecmdCurChipUnit->threadData.end();
                  }

                  if (!easyParse && (ecmdBeginThread != ecmdEndThread)) {
                    sprintf(buf,"%d]->t[", ecmdCurChipUnit->chipUnitNum);
                    cbuf += buf;
                  }

                  for (ecmdCurThread = ecmdBeginThread; ecmdCurThread != ecmdEndThread; ecmdCurThread++) {

                    if (!easyParse) {
                      sprintf(buf, "%d,", ecmdCurThread->threadId);
                      cbuf += buf;
                      curDepth = DEPTH_THREAD;
                    } else {
                      /* We need to figure out if we should add on a chipUnit */
                      std::string fullChipType = ecmdCurChip->chipType.c_str();
                      if (ecmdCurChipUnit->chipUnitType != "") {
                        fullChipType += "." + ecmdCurChipUnit->chipUnitType;
                      }
                      sprintf(buf,"%-15s -p%02d -c%d -t%d\n", fullChipType.c_str(), ecmdCurChip->pos, ecmdCurChipUnit->chipUnitNum, ecmdCurThread->threadId);
                      printed = sbuf + buf;
                      ecmdOutput(printed.c_str());
                    }
                  } /* threadCoreIter */

                } else {
                  /* For non-threaded chips */
                  if (!easyParse) {
                    sprintf(buf, "%d,", ecmdCurChipUnit->chipUnitNum);
                    cbuf += buf;
                  } else {
                    /* We need to figure out if we should add on a chipUnit */
                    std::string fullChipType = ecmdCurChip->chipType.c_str();
                    if (ecmdCurChipUnit->chipUnitType != "") {
                      fullChipType += "." + ecmdCurChipUnit->chipUnitType;
                    }
                    sprintf(buf,"%-15s -p%02d -c%d\n", fullChipType.c_str(), ecmdCurChip->pos, ecmdCurChipUnit->chipUnitNum);
                    printed = sbuf + buf;
                    ecmdOutput(printed.c_str());
                  }
                }
              } /* curChipUnitIter */
            }
            else {
              /* For non-chipunit chips OR For chipunit chips If chipunit list is empty */
              if (!easyParse) {
                if (curDepth != DEPTH_POS) {
                  if (curDepth > DEPTH_POS) {
                    // Pull off a comma out there and then close it up
                    cbuf.erase((cbuf.length() - 1), 1);
                    cbuf += "]\n";
                  }
                  /* Start the new line */
                  cbuf += "        p[";
                }
                sprintf(buf,"%d,", ecmdCurChip->pos);
                cbuf += buf;
                curDepth = DEPTH_POS;
              } else {
                sprintf(buf,"%-15s -p%02d\n", ecmdCurChip->chipType.c_str(), ecmdCurChip->pos);
                printed = sbuf + buf;
                ecmdOutput(printed.c_str());
              }
            } 
          } /* curChipIter */
          /* We have to print our buffer after our last loop is done */
          if (!easyParse) {
            // Pull off a comma out there and then close it up
            cbuf.erase((cbuf.length() - 1), 1);
            cbuf += "]\n";
            ecmdOutput(cbuf.c_str());
            cbuf.clear(); // make sure we reset
          }
        } /* curSlotIter */
      } /* curNodeIter */
    } /* curCageIter */


    /* ---------- */
    /* showconfig */
    /* ---------- */
  } else if (!strcmp(argv[0],"showconfig") || !strcmp(argv[0],"showexist")) {

    //Setup the target that will be used to query the system config 
    ecmdChipTarget target;              ///< Target to refine query
    std::list<ecmdCageData>::iterator ecmdCurCage;      ///< Iterators
    std::list<ecmdNodeData>::iterator ecmdCurNode;
    std::list<ecmdNodeData>::iterator ecmdBeginNode;
    std::list<ecmdNodeData>::iterator ecmdEndNode;
    std::list<ecmdSlotData>::iterator ecmdCurSlot;
    std::list<ecmdSlotData>::iterator ecmdBeginSlot;
    std::list<ecmdSlotData>::iterator ecmdEndSlot;
    std::list<ecmdChipData>::iterator ecmdCurChip;
    std::list<ecmdChipData>::iterator ecmdBeginChip;
    std::list<ecmdChipData>::iterator ecmdEndChip;
    std::list<ecmdChipUnitData>::iterator ecmdCurChipUnit;
    std::list<ecmdChipUnitData>::iterator ecmdBeginChipUnit;
    std::list<ecmdChipUnitData>::iterator ecmdEndChipUnit;
    std::list<ecmdThreadData>::iterator ecmdCurThread;
    std::list<ecmdThreadData>::iterator ecmdBeginThread;
    std::list<ecmdThreadData>::iterator ecmdEndThread;

    /* query info structures for each level */
    ecmdQueryData cageQueryData;            
    ecmdQueryData nodeQueryData;            
    ecmdQueryData slotQueryData;            
    ecmdQueryData chipQueryData;            
    ecmdQueryData chipUnitQueryData;            
    ecmdQueryData threadQueryData;            

    char buf[500];
    char buf2[100];
    

    /* Start looking at the cage level */
    target.cageState = ECMD_TARGET_FIELD_WILDCARD;
    target.nodeState = target.slotState = target.chipTypeState = target.posState = target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

    if (!strcmp(argv[0],"showconfig")) {
      rc = ecmdQueryConfig(target, cageQueryData, ECMD_QUERY_DETAIL_HIGH);
    } else { // exist
      rc = ecmdQueryExist(target, cageQueryData, ECMD_QUERY_DETAIL_HIGH);
    }

    for (ecmdCurCage = cageQueryData.cageData.begin(); ecmdCurCage != cageQueryData.cageData.end(); ecmdCurCage++) {
      
      sprintf(buf,"Cage %d\n",ecmdCurCage->cageId); ecmdOutput(buf);
      sprintf(buf,"  Details: CageUid=%8.8X, Flags=0x%.08X\n",ecmdCurCage->unitId, ecmdCurCage->cageFlags); ecmdOutput(buf);


      /* Now get our node level info */
      target.cage = ecmdCurCage->cageId;
      target.cageState = ECMD_TARGET_FIELD_VALID;
      target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
      target.slotState = target.chipTypeState = target.posState = target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

      if (!strcmp(argv[0],"showconfig")) {
        rc = ecmdQueryConfig(target, nodeQueryData, ECMD_QUERY_DETAIL_HIGH);
      } else { // exist
        rc = ecmdQueryExist(target, nodeQueryData, ECMD_QUERY_DETAIL_HIGH);
      }
      /* If we get back an empty query, we should just continue and try the next item in the list */
      if (nodeQueryData.cageData.empty()) {
        continue;
      }
      /* We have good data, so assign our Begin and End iterators so the for loop below is easily readable */
      ecmdBeginNode = nodeQueryData.cageData.begin()->nodeData.begin();
      ecmdEndNode = nodeQueryData.cageData.begin()->nodeData.end();

      for (ecmdCurNode = ecmdBeginNode; ecmdCurNode != ecmdEndNode; ecmdCurNode++) {

        if (ecmdCurNode->nodeId == ECMD_TARGETDEPTH_NA) {
          sprintf(buf,"  Node NA\n"); ecmdOutput(buf);
        } else {
          sprintf(buf,"  Node %d\n",ecmdCurNode->nodeId ); ecmdOutput(buf);
        }
        sprintf(buf,"    Details: NodeUid=%8.8X, Flags=0x%.08X\n",ecmdCurNode->unitId, ecmdCurNode->nodeFlags); ecmdOutput(buf);

        /* Now get our slot level info */
        target.cage = ecmdCurCage->cageId;
        target.cageState = ECMD_TARGET_FIELD_VALID;
        target.node = ecmdCurNode->nodeId;
        target.nodeState = ECMD_TARGET_FIELD_VALID;
        target.slotState = ECMD_TARGET_FIELD_WILDCARD;
        target.chipTypeState = target.posState = target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

        if (!strcmp(argv[0],"showconfig")) {
          rc = ecmdQueryConfig(target, slotQueryData, ECMD_QUERY_DETAIL_HIGH);
        } else { // exist
          rc = ecmdQueryExist(target, slotQueryData, ECMD_QUERY_DETAIL_HIGH);
        }
        /* If we get back an empty query, we should just continue and try the next item in the list */
        if (slotQueryData.cageData.empty()) {
          continue;
        }
        /* We have good data, so assign our Begin and End iterators so the for loop below is easily readable */
        ecmdBeginSlot = slotQueryData.cageData.begin()->nodeData.begin()->slotData.begin();
        ecmdEndSlot = slotQueryData.cageData.begin()->nodeData.begin()->slotData.end();

        for (ecmdCurSlot = ecmdBeginSlot; ecmdCurSlot != ecmdEndSlot; ecmdCurSlot++) {

          if (ecmdCurSlot->slotId == ECMD_TARGETDEPTH_NA) {
            sprintf(buf,"    Slot NA\n"); ecmdOutput(buf); 
          } else {
            sprintf(buf,"    Slot %d\n",ecmdCurSlot->slotId); ecmdOutput(buf); 
          }
          sprintf(buf,"      Details: SlotUid=%8.8X, Flags=0x%.08X\n",ecmdCurSlot->unitId, ecmdCurSlot->slotFlags); ecmdOutput(buf); buf[0] = '\0';

          /* Now get our chip level info */
          target.cage = ecmdCurCage->cageId;
          target.cageState = ECMD_TARGET_FIELD_VALID;
          target.node = ecmdCurNode->nodeId;
          target.nodeState = ECMD_TARGET_FIELD_VALID;
          target.slot = ecmdCurSlot->slotId;
          target.slotState = ECMD_TARGET_FIELD_VALID;
          target.chipTypeState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
          target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

          if (!strcmp(argv[0],"showconfig")) {
            rc = ecmdQueryConfig(target, chipQueryData, ECMD_QUERY_DETAIL_HIGH);
          } else { // exist
            rc = ecmdQueryExist(target, chipQueryData, ECMD_QUERY_DETAIL_HIGH);
          }
          /* If we get back an empty query, we should just continue and try the next item in the list */
          if (chipQueryData.cageData.empty()) {
            continue;
          }
          /* We have good data, so assign our Begin and End iterators so the for loop below is easily readable */
          ecmdBeginChip = chipQueryData.cageData.begin()->nodeData.begin()->slotData.begin()->chipData.begin();
          ecmdEndChip = chipQueryData.cageData.begin()->nodeData.begin()->slotData.begin()->chipData.end();

          for (ecmdCurChip = ecmdBeginChip; ecmdCurChip != ecmdEndChip; ecmdCurChip++) {

            buf[0] = '\0';
            //Common chip details
            sprintf(buf2, "      %s %d\n",ecmdCurChip->chipType.c_str(), ecmdCurChip->pos);strcat(buf, buf2);
            sprintf(buf2, "        Details: PosUid=%8.8X, Name=%s, Short Name=%s, Common Name=%s,\n",ecmdCurChip->unitId, ecmdCurChip->chipType.c_str(),ecmdCurChip->chipShortType.c_str(),ecmdCurChip->chipCommonType.c_str());
            strcat(buf, buf2);
            sprintf(buf2, "                 Pos=%d, EC=%X, Model EC=%X,\n",ecmdCurChip->pos, ecmdCurChip->chipEc,ecmdCurChip->simModelEc );  
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

            /* Now get our chipUnit level info */
            target.cage = ecmdCurCage->cageId;
            target.cageState = ECMD_TARGET_FIELD_VALID;
            target.node = ecmdCurNode->nodeId;
            target.nodeState = ECMD_TARGET_FIELD_VALID;
            target.slot = ecmdCurSlot->slotId;
            target.slotState = ECMD_TARGET_FIELD_VALID;
            target.chipType = ecmdCurChip->chipType;
            target.chipTypeState = ECMD_TARGET_FIELD_VALID;
            target.pos = ecmdCurChip->pos;
            target.posState = ECMD_TARGET_FIELD_VALID;
            target.chipUnitTypeState = target.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
            target.threadState = ECMD_TARGET_FIELD_UNUSED;

            if (!strcmp(argv[0],"showconfig")) {
              rc = ecmdQueryConfig(target, chipUnitQueryData, ECMD_QUERY_DETAIL_HIGH);
            } else { // exist
              rc = ecmdQueryExist(target, chipUnitQueryData, ECMD_QUERY_DETAIL_HIGH);
            }
            /* If we get back an empty query, we should just continue and try the next item in the list */
            if (chipUnitQueryData.cageData.empty()) {
              continue;
            }
            /* We have good data, so assign our Begin and End iterators so the for loop below is easily readable */
            ecmdBeginChipUnit = chipUnitQueryData.cageData.begin()->nodeData.begin()->slotData.begin()->chipData.begin()->chipUnitData.begin();
            ecmdEndChipUnit = chipUnitQueryData.cageData.begin()->nodeData.begin()->slotData.begin()->chipData.begin()->chipUnitData.end();

            for (ecmdCurChipUnit = ecmdBeginChipUnit; ecmdCurChipUnit != ecmdEndChipUnit; ecmdCurChipUnit++) {
              sprintf(buf, "        %s %d\n", (ecmdCurChipUnit->chipUnitType == "" ? "core" : ecmdCurChipUnit->chipUnitType.c_str()), ecmdCurChipUnit->chipUnitNum ); ecmdOutput(buf); 
              sprintf(buf, "          Details: ChipUnitUid=%8.8X, Flags=0x%.08X\n", ecmdCurChipUnit->unitId, ecmdCurChipUnit->chipUnitFlags); ecmdOutput(buf);

              /* Now get our thread level info */
              target.cage = ecmdCurCage->cageId;
              target.cageState = ECMD_TARGET_FIELD_VALID;
              target.node = ecmdCurNode->nodeId;
              target.nodeState = ECMD_TARGET_FIELD_VALID;
              target.slot = ecmdCurSlot->slotId;
              target.slotState = ECMD_TARGET_FIELD_VALID;
              target.chipType = ecmdCurChip->chipType;
              target.chipTypeState = ECMD_TARGET_FIELD_VALID;
              target.pos = ecmdCurChip->pos;
              target.posState = ECMD_TARGET_FIELD_VALID;
              if (ecmdCurChipUnit->chipUnitType != "") {
                target.chipUnitType = ecmdCurChipUnit->chipUnitType;
                target.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
              } else {
                target.chipUnitTypeState = ECMD_TARGET_FIELD_UNUSED;
              }
              target.chipUnitNum = ecmdCurChipUnit->chipUnitNum;
              target.chipUnitNumState = ECMD_TARGET_FIELD_VALID;
              target.threadState = ECMD_TARGET_FIELD_WILDCARD;

              if (!strcmp(argv[0],"showconfig")) {
                rc = ecmdQueryConfig(target, threadQueryData, ECMD_QUERY_DETAIL_HIGH);
              } else { // exist
                rc = ecmdQueryExist(target, threadQueryData, ECMD_QUERY_DETAIL_HIGH);
              }
              /* If we get back an empty query, we should just continue and try the next item in the list */
              if (threadQueryData.cageData.empty()) {
                continue;
              }
              /* We have good data, so assign our Begin and End iterators so the for loop below is easily readable */
              ecmdBeginThread = threadQueryData.cageData.begin()->nodeData.begin()->slotData.begin()->chipData.begin()->chipUnitData.begin()->threadData.begin();
              ecmdEndThread = threadQueryData.cageData.begin()->nodeData.begin()->slotData.begin()->chipData.begin()->chipUnitData.begin()->threadData.end();

              for (ecmdCurThread = ecmdBeginThread; ecmdCurThread != ecmdEndThread; ecmdCurThread++) {
                sprintf(buf, "          Thread %d\n", ecmdCurThread->threadId );ecmdOutput(buf); 
                sprintf(buf, "            Details: ThreadUid=%8.8X, Flags=0x%.08X\n", ecmdCurThread->unitId , ecmdCurThread->threadFlags);ecmdOutput(buf); 
              }

            } /* curChipUnitIter */

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
    target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

    bool validPosFound = false;
    rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
    if (rc) return rc;

    char buf[200];

    while (ecmdConfigLooperNext(target, looperdata)) {

      /* Let's look up other info about the chip, namely the ec level */
      rc = ecmdGetChipData (target, chipdata);
      if (rc) {
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

    if (!validPosFound) {
      ecmdOutputError("ecmdquery - Unable to find a valid chip to execute command on\n");
      return ECMD_TARGET_NOT_CONFIGURED;
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
