/* $Header$ */
// Copyright ***********************************************************
//                                                                      
// File ecmdArrayUser.C                                  
//                                                                      
// IBM Confidential                                                     
// OCO Source Materials                                                 
// 9400 Licensed Internal Code                                          
// (C) COPYRIGHT IBM CORP. 2003                                         
//                                                                      
// The source code for this program is not published or otherwise       
// divested of its trade secrets, irrespective of what has been         
// deposited with the U.S. Copyright Office.                            
//                                                                      
// End Copyright *******************************************************

// Module Description **************************************************
//
// Description: 
//
// End Module Description **********************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------

#include <ctype.h>
#include <algorithm>

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
#ifndef ECMD_REMOVE_ARRAY_FUNCTIONS

uint32_t ecmdGetArrayUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  uint32_t coeRc = ECMD_SUCCESS;
  ecmdChipTarget target;                ///< Current target
  ecmdChipTarget cuTarget;              ///< Current target being operated on for the chipUnits
  std::string arrayName;                ///< Name of array to access
  ecmdDataBuffer address;               ///< Buffer to store address
  ecmdDataBuffer address_copy;          ///< Copy of address to modify in entry loop
  uint32_t  numEntries = 1;             ///< Number of consecutive entries to fetch
  bool validPosFound = false;           ///< Did we find something to actually execute on ?
  std::string printed;                  ///< Print Buffer
  bool printedHeader;                   ///< Have we printed the array name and pos
  std::list<ecmdArrayEntry> entries;    ///< List of arrays to fetch, to use getArrayMultiple
  ecmdArrayEntry entry;                 ///< Array entry to fetch
  uint32_t* add_buffer = NULL;          ///< Buffer to do temp work with the address for incrementing
  ecmdArrayData arrayData;              ///< Query data about array
  std::list<ecmdArrayData> arrayDataList;      ///< Query data about array
  ecmdLooperData looperData;            ///< Store internal Looper data
  ecmdLooperData cuLooper;              ///< Store internal Looper data for the chipUnit loop
  std::string outputformat = "x";       ///< Output Format to display
  std::string inputformat = "x";        ///< Input format of data
  bool expectFlag = false;
  bool maskFlag = false;
  char* expectPtr = NULL;               ///< Pointer to expected data in arg list
  char* maskPtr = NULL;                 ///< Pointer to mask data in arg list
  ecmdDataBuffer expected;              ///< Buffer to store expected data
  ecmdDataBuffer mask;                  ///< Buffer for mask of expected data
  bool isChipUnitArray;                     ///< Is this a chipUnit array ?
  
  //expect and mask flags check
  if ((expectPtr = ecmdParseOptionWithArgs(&argc, &argv, "-exp")) != NULL) {
    expectFlag = true;

    if ((maskPtr = ecmdParseOptionWithArgs(&argc, &argv, "-mask")) != NULL) {
      maskFlag = true;
    }
  }
  
  /* get format flag, if it's there */
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr != NULL) {
    outputformat = formatPtr;
  }
  formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr != NULL) {
    inputformat = formatPtr;
  }


  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode
  
  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 3) {
    ecmdOutputError("getarray - Too few arguments specified; you need at least a chip, an array, and an address or ALL.\n");
    ecmdOutputError("getarray - Type 'getarray -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  arrayName = argv[1];

  /* Did the specify more then one entry ? */
  if( (argc > 3) && ((std::string)argv[2] == "ALL") ) {
    ecmdOutputError("getarray - Cannot specify NumEntries with the ALL option.\n");
    return ECMD_INVALID_ARGS;
  } else if (argc > 3) {
    if (!ecmdIsAllDecimal(argv[3])) {
      ecmdOutputError("getarray - Non-decimal numbers detected in numEntries field\n");
      return ECMD_INVALID_ARGS;
    }
    numEntries = (uint32_t)atoi(argv[3]);
  }

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;

   while (ecmdConfigLooperNext(target, looperData) && (!coeRc || coeMode)) {     //@02

    /* We need to find out info about this array */
    rc = ecmdQueryArray(target, arrayDataList , arrayName.c_str(), ECMD_QUERY_DETAIL_LOW);
    if (rc || arrayDataList.empty()) {
      printed = "getarray - Problems retrieving data about array '" + arrayName + "' on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;                                   //@02
      continue;                                     //@02
    }
    arrayData = *(arrayDataList.begin());

    /* We have to do the expact flag data read here so that we know the bit length in the right aligned data case - JTA 02/21/06 */
    if (expectFlag) {

      rc = ecmdReadDataFormatted(expected, expectPtr, inputformat, arrayData.width);
      if (rc) {
        ecmdOutputError("getarray - Problems occurred parsing expected data, must be an invalid format\n");
        coeRc = rc;                                 //@02        
        continue;                                   //@02
      }

      if (maskFlag) {
        rc = ecmdReadDataFormatted(mask, maskPtr, inputformat, arrayData.width);
        if (rc) {
          ecmdOutputError("getarray - Problems occurred parsing mask data, must be an invalid format\n");
        coeRc = rc;                                 //@02        
        continue;                                   //@02
        }

      }
    }

    if ((std::string)argv[2] == "ALL") {
      entries.clear();
    } else {

      /* Set the length  */
      address.setBitLength(arrayData.readAddressLength);
      rc = address.insertFromHexRight(argv[2], 0, arrayData.readAddressLength);
      if (rc) {
        ecmdOutputError("getarray - Invalid number format detected trying to parse address\n");
          coeRc = rc;                                //@02
          continue;                                  //

      }


      add_buffer = new uint32_t[address.getWordLength()];
      uint32_t idx;
      uint32_t add_inc = 1;	      ///< Address increment, this will increment data by 1 for left aligned buffer
      uint32_t add_mask = 0xFFFFFFFF;	  ///< Mask of valid bits in the last word of the address
      if (address.getBitLength() % 32) {
        add_inc = 1 << (32 - (address.getBitLength() % 32));
        add_mask <<= (32 - (address.getBitLength() % 32));
      }

      /* Extract the address into a buffer we can deal with */
      for (idx = 0; idx < address.getWordLength(); idx ++) {
        add_buffer[idx] = address.getWord(idx);
      }
      /* Setup the array entries we are going to fetch */
      for (idx = 0; idx < numEntries; idx ++) { 			   //@01d
        entry.address.setBitLength(address.getBitLength());
        entry.address.insert(add_buffer, 0, address.getBitLength());

        entries.push_back(entry);

        /* Increment for the next one, if we have more to go */
        if ((idx + 1) < numEntries) {
          // Can't hit initialized error for add_buffer that Beam calls out.  So
          // tell it to ignore that message with the comment on the following line.
          if (add_buffer[address.getWordLength()-1] == add_mask) {/*uninitialized*/
            /* We are going to rollover */
            if (address.getWordLength() == 1) {
              printed = "getarray - Address overflow on " + arrayName + " ";
              printed += ecmdWriteTarget(target) + "\n";
              ecmdOutputError( printed.c_str() );
              // Clean up allocated memory
              if (coeMode) {
                continue;
              } else {
                if (add_buffer) {
                  delete[] add_buffer;
                } 

                return ECMD_DATA_OVERFLOW;
              }
            }

            add_buffer[address.getWordLength()-1] = 0;
            for (int word = (int)address.getWordLength()-2; word >= 0; word --) {
              if (add_buffer[word] == 0xFFFFFFFF) {
                /* We are going to rollover */
                if (word == 0) {
                  printed = "getarray - Address overflow on " + arrayName + " ";
                  printed += ecmdWriteTarget(target) + "\n";
                  ecmdOutputError( printed.c_str() );
                  // Clean up allocated memory
                  if (coeMode) {       
                    continue;
                  } else {
                    if (add_buffer) {
                      delete[] add_buffer;
                    } 

                    return ECMD_DATA_OVERFLOW;
                  }
                }
                add_buffer[word] = 0;
              } else {
                add_buffer[word] ++;
                /* We took care of the carryover, let's get out of here */
                break;
              }

            }
          } else {
            // Beam doesn't pick up add_inc initialization above.  So tell it to
            // ignore uninitialized error message with comment in line below.
            add_buffer[address.getWordLength()-1] += add_inc; /*uninitialized*/
          }
        }
      }

    }

    printedHeader = false;

    isChipUnitArray = arrayData.isChipUnitRelated;

    /* Setup our chipUnit looper if needed */
    cuTarget = target;
    if (isChipUnitArray) {
      cuTarget.chipTypeState = cuTarget.cageState = cuTarget.nodeState = cuTarget.slotState = cuTarget.posState = ECMD_TARGET_FIELD_VALID;
      /* Error check the chipUnit returned */
      if (arrayData.relatedChipUnit != chipUnitType) {
        printed = "getarray - Provided chipUnit \"";
        printed += chipUnitType;
        printed += "\"doesn't match chipUnit returned by queryArray \"";
        printed += arrayData.relatedChipUnit + "\"\n";
        ecmdOutputError( printed.c_str() );
        rc = ECMD_INVALID_ARGS;
        break;
      }
      /* If we have a chipUnit, set the state fields properly */
      if (chipUnitType != "") {
        cuTarget.chipUnitType = chipUnitType;
        cuTarget.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
      }
      cuTarget.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
      cuTarget.threadState = ECMD_TARGET_FIELD_UNUSED;

      /* Init the chipUnit loop */
      rc = ecmdConfigLooperInit(cuTarget, ECMD_SELECTED_TARGETS_LOOP, cuLooper);
      if (rc) {
        printed = "getarray - Error returned from ecmdConfigLooperInit on chipUnit Looper ";
        printed += ecmdWriteTarget(cuTarget) + "\n";
        ecmdOutputError( printed.c_str() );
        // Clean up allocated memory
        if (add_buffer)
        {
          delete[] add_buffer;
        }
        coeRc = rc;
        continue;
      }
    } else { // !isChipUnitArray
      if (chipUnitType != "") {
        printed = "getarray - A chipUnit \"";
        printed += chipUnitType;
        printed += "\" was given on a non chipUnit array\n";
        ecmdOutputError(printed.c_str());
        rc = ECMD_INVALID_ARGS;
        break;
      }
    }

    /* Actually go fetch the data */
    while ((!isChipUnitArray || ecmdConfigLooperNext(cuTarget, cuLooper))&& (!coeRc || coeMode)) {
      rc = getArrayMultiple(cuTarget, arrayName.c_str(), entries);
      if (rc) {
        printed = "getarray - Error occured performing getArrayMultiple on ";
        printed += ecmdWriteTarget(cuTarget) + "\n";
        ecmdOutputError( printed.c_str() );
        // Clean up allocated memory
        if (add_buffer) {
          delete[] add_buffer;
        }
        coeRc = rc;
        continue;
      }
      else {
        validPosFound = true;     
      }

      for (std::list<ecmdArrayEntry>::iterator entit = entries.begin(); entit != entries.end(); entit ++) {
        if (expectFlag) {

          if (maskFlag) {
            entit->buffer.setAnd(mask, 0, entit->buffer.getBitLength());
          }

          uint32_t mismatchBit = 0;
          if (!ecmdCheckExpected(entit->buffer, expected, mismatchBit)) {

            //@ make this stuff sprintf'd

            if (!printedHeader) {
              printed = ecmdWriteTarget(cuTarget) + " " + arrayName + "\n";
              printedHeader = true;
            } 

            printed = "\ngetarray - Data miscompare occured at address: " + entit->address.genHexRightStr() + "\n";
            ecmdOutputError( printed.c_str() );

            if (mismatchBit != ECMD_UNSET) {
              char outstr[200];
              sprintf(outstr, "First bit mismatch found at bit %d\n",mismatchBit);
              ecmdOutputError( outstr );
            }


            printed = "getarray - Actual";
            if (maskFlag) {
              printed += " (with mask): ";
            }
            else {
              printed += "            : ";
            }

            printed += ecmdWriteDataFormatted(entit->buffer, outputformat);
            ecmdOutputError( printed.c_str() );

            printed = "getarray - Expected          : ";
            printed += ecmdWriteDataFormatted(expected, outputformat);
            ecmdOutputError( printed.c_str() );
          }

        }
        else {

          if (!printedHeader) {
            printed = ecmdWriteTarget(cuTarget) + " " + arrayName + "\n";
            ecmdOutput( printed.c_str() );
            printedHeader = true;
          }

          printed = entit->address.genHexRightStr() + "\t";
          printed += ecmdWriteDataFormatted(entit->buffer, outputformat);
          ecmdOutput( printed.c_str() );
        }

        // Clear ecmdDataBuffer and rc before next 'get'
        //  but save address info
        entit->rc=ECMD_SUCCESS;  // clear rc before next 'get'
        entit->buffer.clear(); // clear data before next 'get'

      } // end of for loop for output of all entries for a target

      // reset printedHeader for next target
      printedHeader = false;

      if (!isChipUnitArray) break;
    } /* End cuLooper */

    /* Now that we are done, clear the list for the next iteration - fixes BZ#49 */
    entries.clear();

  } /* End PosLooper */

  if (!validPosFound) {
    //this is an error common across all UI functions
    ecmdOutputError("getarray - Unable to find a valid chip to execute command on\n");
    // Clean up allocated memory
    if (add_buffer)                                                      //@01a
    {
        delete[] add_buffer;
    }
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  // Clean up allocated memory
  if (add_buffer)                                                        //@01a
  {
      delete[] add_buffer;
  }

  // Now check if our coeRc accumulated anything and return if it has
  if (coeRc) return coeRc;

  return rc;
}

uint32_t ecmdPutArrayUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  uint32_t coeRc = ECMD_SUCCESS;                    //@02
  ecmdChipTarget target;        ///< Current target
  ecmdChipTarget cuTarget;      ///< Current target being operated on for the chipUnits
  std::string arrayName;        ///< Name of array to access
  ecmdDataBuffer address;       ///< Buffer to store address
  ecmdDataBuffer buffer;        ///< Buffer to store data to write with
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  ecmdLooperData looperData;            ///< Store internal Looper data
  ecmdLooperData cuLooper;                    ///< Store internal Looper data for the chipUnit loop
  ecmdArrayData arrayData;      ///< Query data about array
  std::list<ecmdArrayData> arrayDataList;      ///< Query data about array
  bool isChipUnitArray;             ///< Is this a chipUnit array ?

  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-i");
  if (formatPtr == NULL) {
    format = "x";
  }
  else {
    format = formatPtr;
  }

  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/

  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode

  if (argc < 4) {
    ecmdOutputError("putarray - Too few arguments specified; you need at least a chip, an array, an address, and some data.\n");
    ecmdOutputError("putarray - Type 'putarray -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  arrayName = argv[1];

  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;

  while (ecmdConfigLooperNext(target, looperData) && (!coeRc || coeMode)) {


    /* We need to find out info about this array */
    rc = ecmdQueryArray(target, arrayDataList , arrayName.c_str(), ECMD_QUERY_DETAIL_LOW);
    if (rc || arrayDataList.empty()) {
      printed = "putarray - Problems retrieving data about array '" + arrayName + "' on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }
    arrayData = *(arrayDataList.begin());
    isChipUnitArray = arrayData.isChipUnitRelated;

    /* Set the length  */
    address.setBitLength(arrayData.writeAddressLength);
    rc = address.insertFromHexRight(argv[2], 0, arrayData.writeAddressLength);
    if (rc) {
      ecmdOutputError("putarray - Invalid number format detected trying to parse address\n");
      coeRc = rc;
      continue;
    }

    rc = ecmdReadDataFormatted(buffer, argv[3], format, arrayData.width);
    if (rc) {
      ecmdOutputError("putarray - Problems occurred parsing input data, must be an invalid format\n");
      coeRc = rc;                         //@02
      continue;                           //@02
    }

    /* Setup our chipUnit looper if needed */
    cuTarget = target;
    if (isChipUnitArray) {
      cuTarget.chipTypeState = cuTarget.cageState = cuTarget.nodeState = cuTarget.slotState = cuTarget.posState = ECMD_TARGET_FIELD_VALID;
      /* Error check the chipUnit returned */
      if (arrayData.relatedChipUnit != chipUnitType) {
        printed = "putarray - Provided chipUnit \"";
        printed += chipUnitType;
        printed += "\"doesn't match chipUnit returned by queryArray \"";
        printed += arrayData.relatedChipUnit + "\"\n";
        ecmdOutputError( printed.c_str() );
        rc = ECMD_INVALID_ARGS;
        break;
      }
      /* If we have a chipUnit, set the state fields properly */
      if (chipUnitType != "") {
        cuTarget.chipUnitType = chipUnitType;
        cuTarget.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
      }
      cuTarget.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
      cuTarget.threadState = ECMD_TARGET_FIELD_UNUSED;

      /* Init the chipUnit loop */
      rc = ecmdConfigLooperInit(cuTarget, ECMD_SELECTED_TARGETS_LOOP, cuLooper);
      if (rc) return rc;
    } else { // !isChipUnitArray
      if (chipUnitType != "") {
        printed = "putarray - A chipUnit \"";
        printed += chipUnitType;
        printed += "\" was given on a non chipUnit array\n";
        ecmdOutputError(printed.c_str());
        rc = ECMD_INVALID_ARGS;
        break;
      }
    }

    /* If this isn't a chipUnit ring we will fall into while loop and break at the end, if it is we will call run through configloopernext */
    while ((!isChipUnitArray || ecmdConfigLooperNext(cuTarget, cuLooper))&& (!coeRc || coeMode)){ //@02

      rc = putArray(cuTarget, arrayName.c_str(), address, buffer);
      if (rc) {
        printed = "putarray - Error occured performing putarray on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;                         //@02
        continue;                           //@02
      }
      else {
        validPosFound = true;     
      }

      if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
        printed = ecmdWriteTarget(target) + "\n";
        ecmdOutput(printed.c_str());
      }

      if (!isChipUnitArray) break;
    } /* End cuLooper */
  } /* End PosLooper */

  if (!validPosFound) {
    //this is an error common across all UI functions
    ecmdOutputError("putarray - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  // Now check if our coeRc accumulated anything and return if it has
  if (coeRc) return coeRc;

  return rc;
}
#endif // ECMD_REMOVE_ARRAY_FUNCTIONS


#ifndef ECMD_REMOVE_TRACEARRAY_FUNCTIONS
uint32_t ecmdGetTraceArrayUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;
  uint32_t coeRc = ECMD_SUCCESS;        //@02
  ecmdChipTarget target;                ///< Current target
  ecmdChipTarget cuTarget;              ///< Current target being operated on for the chipUnits
  bool validPosFound = false;           ///< Did we find something to actually execute on ?
  std::string printed;                  ///< Print Buffer
  bool printedHeader;                   ///< Have we printed the array name and pos
  ecmdLooperData looperData;            ///< Store internal Looper data
  ecmdLooperData cuLooper;            ///< Store internal Looper data for the chipUnit loop
  uint32_t loop; 			///<loop around the array data
  bool doStopStart = true;              ///< Do we StopStart trace arrays ?
  std::list<ecmdTraceArrayData> queryTraceData; ///< Trace Data
  std::list<ecmdTraceArrayData>::iterator queryIt; ///< Trace Data Ietrator
  std::list<ecmdNameVectorEntry> coreArrayList;        ///< Array data fetched
  std::list<ecmdNameVectorEntry> nestArrayList;        ///< Array data fetched
  ecmdNameVectorEntry entry;                       ///< Entry to populate the list
  
  /* get format flag, if it's there */
  std::string format;
  char * formatPtr = ecmdParseOptionWithArgs(&argc, &argv, "-o");
  if (formatPtr == NULL) {
    format = "x";
  }
  else {
    format = formatPtr;
  }

  doStopStart = !ecmdParseOption(&argc, &argv, "-nostopstart");


  /************************************************************************/
  /* Parse Common Cmdline Args                                            */
  /************************************************************************/
  rc = ecmdCommandArgs(&argc, &argv);
  if (rc) return rc;

  /* Global args have been parsed, we can read if -coe was given */
  bool coeMode = ecmdGetGlobalVar(ECMD_GLOBALVAR_COEMODE); ///< Are we in continue on error mode
  
  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 2) {
    ecmdOutputError("gettracearray - Too few arguments specified; you need at least a chip and an array.\n");
    ecmdOutputError("gettracearray - Type 'gettracearray -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  
  
  //Setup the looper to do gettracearray
  std::string chipType, chipUnitType;
  ecmdParseChipField(argv[0], chipType, chipUnitType);
  target.chipType = chipType;
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.chipUnitTypeState = target.chipUnitNumState = target.threadState = ECMD_TARGET_FIELD_UNUSED;
  
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperData);
  if (rc) return rc;
 
  while (ecmdConfigLooperNext(target, looperData) && (!coeRc || coeMode)) {


    //Get all the valid trace arrays
    rc = ecmdQueryTraceArray(target, queryTraceData, NULL, ECMD_QUERY_DETAIL_LOW);
    if (rc) {
      printed = "gettracearray - Error occurred performing querytracearray on ";
      printed += ecmdWriteTarget(target);
      printed += "\n";
      ecmdOutputError( printed.c_str() );
      coeRc = rc;
      continue;
    }

    if (queryTraceData.size() < 1) {
      ecmdOutputError("gettracearray - Too much/little tracearray information returned from the dll, unable to determine core vs non-core\n");
      return ECMD_DLL_INVALID;
    }

    nestArrayList.clear();
    coreArrayList.clear();

    for (int i = 1; i < argc; i++) {//Loop for the trace array names from the user

      bool tracearrayfound = false;
      std::string traceArrayName = argv[i];
      transform(traceArrayName.begin(), traceArrayName.end(), traceArrayName.begin(), (int(*)(int)) toupper);


      for (queryIt = queryTraceData.begin(); queryIt != queryTraceData.end(); queryIt++) {
        std::string qTrace = queryIt->traceArrayName;
        transform(qTrace.begin(), qTrace.end(), qTrace.begin(), (int(*)(int)) toupper);
        if (qTrace == traceArrayName) {
          tracearrayfound = true;
          entry.name = traceArrayName;

          if (queryIt->isChipUnitRelated) {
            coreArrayList.push_back(entry);
          } else {
            nestArrayList.push_back(entry);
          }
        }
      }

      if (!tracearrayfound) {
        printed = "gettracearray - TraceArray " + traceArrayName + " not found on\n";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return ECMD_INVALID_ARGS;
      }

    }

    if (nestArrayList.size() > 0) {

      rc = getTraceArrayMultiple(target,  doStopStart, nestArrayList);
      if (rc) {
        printed = "gettracearray - Error occured performing getTraceArrayMultiple on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        coeRc = rc;                                           //@02
        continue;                                             //@02
      } else {
        validPosFound = true;
      }

      for (std::list<ecmdNameVectorEntry>::iterator lit = nestArrayList.begin(); lit != nestArrayList.end(); lit++ ) {
        printedHeader = false;
        for(loop =0; loop < lit->buffer.size() ; loop++) {
          if (!printedHeader) {
            printed = ecmdWriteTarget(target) + " " + lit->name + "\n";
            ecmdOutput( printed.c_str() );
            printedHeader = true;
          }

          printed = ecmdWriteDataFormatted(lit->buffer[loop], format);

          ecmdOutput( printed.c_str() );
        }
      }
    }
    /* Setup our Core looper if needed */
    cuTarget = target;
    if (coreArrayList.size() > 0) {
      cuTarget.chipTypeState = cuTarget.cageState = cuTarget.nodeState = cuTarget.slotState = cuTarget.posState = ECMD_TARGET_FIELD_VALID;
      /* Error check the chipUnit returned */
      /* THIS CODE HAS TO BE UPDATED SO THAT YOU HAVE THE RETURNED CHIPUNIT QUERY INFORMATION.  STGC00315950 was opened about the fix
       JTA 09/05/2007
      if (queryRingData.begin()->relatedChipUnit != chipUnitType) {
        printed = "gettracearray - Provided chipUnit: \"";
        printed += chipUnitType;
        printed += "\"doesn't match chipUnit returned by queryTraceArray: \"";
        printed += queryRingData.begin()->relatedChipUnit + "\n";
        ecmdOutputError( printed.c_str() );
        rc = ECMD_INVALID_ARGS;
        break;
      } */
      /* If we have a chipUnit, set the state fields properly */
      if (chipUnitType != "") {
        ecmdOutputError("gettracearray - UPDATES NEEDED!!!! This code won't work for chipUnit\n");
        cuTarget.chipUnitType = chipUnitType;
        cuTarget.chipUnitTypeState = ECMD_TARGET_FIELD_VALID;
      }
      cuTarget.chipUnitNumState = ECMD_TARGET_FIELD_WILDCARD;
      cuTarget.threadState = ECMD_TARGET_FIELD_UNUSED;

      /* Init the core loop */
      rc = ecmdConfigLooperInit(cuTarget, ECMD_SELECTED_TARGETS_LOOP, cuLooper);
      if (rc) return rc;


      /* If this isn't a core ring we will fall into while loop and break at the end, if it is we will call run through configloopernext */
      while (ecmdConfigLooperNext(cuTarget, cuLooper)) {

        //Clear the core List
        for (std::list<ecmdNameVectorEntry>::iterator lit = coreArrayList.begin(); lit != coreArrayList.end(); lit++ ) {
          lit->buffer.clear();
        }

        rc = getTraceArrayMultiple(cuTarget,  doStopStart, coreArrayList);
        if (rc) {
          printed = "gettracearray - Error occured performing getTraceArray on ";
          printed += ecmdWriteTarget(cuTarget) + "\n";
          ecmdOutputError( printed.c_str() );
          coeRc = rc;                                           //@02
          continue;                                             //@02
        }
        else {
          validPosFound = true;
        }

        for (std::list<ecmdNameVectorEntry>::iterator lit = coreArrayList.begin(); lit != coreArrayList.end(); lit++ ) {
          printedHeader = false;
          for(loop =0; loop < lit->buffer.size() ; loop++) {
            if (!printedHeader) {
              printed = ecmdWriteTarget(cuTarget) + " " + lit->name + "\n";
              ecmdOutput( printed.c_str() );
              printedHeader = true;
            }

            printed = ecmdWriteDataFormatted(lit->buffer[loop], format);

            ecmdOutput( printed.c_str() );
          }
        }

      }/* End cuLooper */

    } /* If core trace */

  }/* End ChipLooper */

  if (!validPosFound) {
    //this is an error common across all UI functions
    ecmdOutputError("gettracearray - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  } 

  // Now check if our coeRc accumulated anything and return if it has
  if (coeRc) return coeRc;

  return rc;
}
#endif // ECMD_REMOVE_TRACEARRAY_FUNCTIONS

// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//  none STGC7449      04/18/05 prahl    Clean up Beam messages.
//  @01  STGC12283     05/25/05 prahl    Fix memory leak of add_buffer
//  @02  F620122       08/22/07 shashank Add support for "continue on error" for the command lines
// End Change Log *****************************************************
