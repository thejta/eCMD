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
/* $Header$ */

// Module Description **************************************************
//
// Description: 
//
// End Module Description **********************************************

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define ecmdArrayUser_C

#include <ctype.h>
#include <algorithm>

#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <ecmdSharedUtils.H>

#undef ecmdArrayUser_C
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

uint32_t ecmdGetArrayUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  std::string arrayName;        ///< Name of array to access
  ecmdDataBuffer address;       ///< Buffer to store address
  ecmdDataBuffer address_copy;  ///< Copy of address to modify in entry loop
  uint32_t  numEntries = 1;     ///< Number of consecutive entries to fetch
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  bool printedHeader;           ///< Have we printed the array name and pos
  std::list<ecmdArrayEntry> entries;    ///< List of arrays to fetch, to use getArrayMultiple
  ecmdArrayEntry entry;         ///< Array entry to fetch
  uint32_t* add_buffer = NULL;  ///< Buffer to do temp work with the address for incrementing
  ecmdArrayData arrayData;      ///< Query data about array
  std::list<ecmdArrayData> arrayDataList;      ///< Query data about array
  ecmdLooperData looperdata;            ///< Store internal Looper data
  std::string outputformat = "x";  ///< Output Format to display
  std::string inputformat = "x";   ///< Input format of data
  bool expectFlag = false;
  bool maskFlag = false;
  char* expectPtr = NULL;          ///< Pointer to expected data in arg list
  char* maskPtr = NULL;            ///< Pointer to mask data in arg list
  ecmdDataBuffer expected;         ///< Buffer to store expected data
  ecmdDataBuffer mask;             ///< Buffer for mask of expected data
  
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


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 3) {
    ecmdOutputError("getarray - Too few arguments specified; you need at least a chip, an array, and an address.\n");
    ecmdOutputError("getarray - Type 'getarray -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_FIELD_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  arrayName = argv[1];

  

  if (expectFlag) {

    rc = ecmdReadDataFormatted(expected, expectPtr, inputformat);
    if (rc) {
      ecmdOutputError("getarray - Problems occurred parsing expected data, must be an invalid format\n");
      return rc;
    }

    if (maskFlag) {
      rc = ecmdReadDataFormatted(mask, maskPtr, inputformat);
      if (rc) {
        ecmdOutputError("getarray - Problems occurred parsing mask data, must be an invalid format\n");
        return rc;
      }

    }


  }
  /* Did the specify more then one entry ? */
  if( argc > 3 ) {
    numEntries = atoi(argv[3]);
  } 
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while ( ecmdConfigLooperNext(target, looperdata) ) {

    /* We need to find out info about this array */
    rc = ecmdQueryArray(target, arrayDataList , arrayName.c_str(), ECMD_QUERY_DETAIL_LOW);
    if (rc || arrayDataList.empty()) {
      printed = "getarray - Problems retrieving data about array '" + arrayName + "' on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    arrayData = *(arrayDataList.begin());


    /* Set the length  */
    address.setBitLength(arrayData.readAddressLength);
    rc = address.insertFromHexRight(argv[2], 0, arrayData.readAddressLength);
    if (rc) {
      ecmdOutputError("getarray - Invalid number format detected trying to parse address\n");
      return rc;
    }


    add_buffer = new uint32_t[address.getWordLength()];
    uint32_t idx;
    uint32_t add_inc = 1;           ///< Address increment, this will increment data by 1 for left aligned buffer
    uint32_t add_mask = 0xFFFFFFFF;     ///< Mask of valid bits in the last word of the address
    if (address.getBitLength() % 32) {
      add_inc = 1 << (32 - (address.getBitLength() % 32));
      add_mask <<= (32 - (address.getBitLength() % 32));
    }

    /* Extract the address into a buffer we can deal with */
    for (idx = 0; idx < address.getWordLength(); idx ++) {
      add_buffer[idx] = address.getWord(idx);
    }
    /* Setup the array entries we are going to fetch */
    for (idx = 0; idx < numEntries; idx ++) {                            //@01d
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
            if (add_buffer)                                                //@01a
            {
              delete[] add_buffer;
            }
            return ECMD_DATA_OVERFLOW;
          }

          add_buffer[address.getWordLength()-1] = 0;
          for (int word = address.getWordLength()-2; word >= 0; word --) {
            if (add_buffer[word] == 0xFFFFFFFF) {
              /* We are going to rollover */
              if (word == 0) {
                printed = "getarray - Address overflow on " + arrayName + " ";
                printed += ecmdWriteTarget(target) + "\n";
                ecmdOutputError( printed.c_str() );
                // Clean up allocated memory
                if (add_buffer)                                            //@01a
                {
                  delete[] add_buffer;
                }
                return ECMD_DATA_OVERFLOW;
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
  


    printedHeader = false;

    /* Actually go fetch the data */
    rc = getArrayMultiple(target, arrayName.c_str(), entries);
    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
      printed = "getarray - Error occured performing getArrayMultiple on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      // Clean up allocated memory
      if (add_buffer)                                                    //@01a
      {
          delete[] add_buffer;
      }
      return rc;
    }
    else {
      validPosFound = true;     
    }

    for (std::list<ecmdArrayEntry>::iterator entit = entries.begin(); entit != entries.end(); entit ++) {
      if (expectFlag) {

       if (maskFlag) {
         entit->buffer.setAnd(mask, 0, entit->buffer.getBitLength());
       }

       if (!ecmdCheckExpected(entit->buffer, expected)) {

         //@ make this stuff sprintf'd
         
	 if (!printedHeader) {
           printed = ecmdWriteTarget(target) + " " + arrayName + "\n";
           printedHeader = true;
         } 
       
         printed = "\ngetarray - Data miscompare occured at address: " + entit->address.genHexRightStr() + "\n";
         ecmdOutputError( printed.c_str() );


         printed = "getarray - Actual";
         if (maskFlag) {
           printed += " (with mask): ";
         }
         else {
           printed += " 	   : ";
         }

         printed += ecmdWriteDataFormatted(entit->buffer, outputformat);
         ecmdOutputError( printed.c_str() );

         printed = "getscom - Expected  	: ";
         printed += ecmdWriteDataFormatted(expected, outputformat);
         ecmdOutputError( printed.c_str() );
       }

      }
      else {
 
       if (!printedHeader) {
         printed = ecmdWriteTarget(target) + " " + arrayName + "\n" + entit->address.genHexRightStr() + "\t";
         printedHeader = true;
       } else {
         printed = entit->address.genHexRightStr() + "\t";
       }

       printed += ecmdWriteDataFormatted(entit->buffer, outputformat);

       ecmdOutput( printed.c_str() );
      }
    }

    /* Now that we are done, clear the list for the next iteration - fixes BZ#49 */
    entries.clear();

  }

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
  return rc;
}

uint32_t ecmdPutArrayUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdChipTarget target;        ///< Current target
  std::string arrayName;        ///< Name of array to access
  ecmdDataBuffer address;       ///< Buffer to store address
  ecmdDataBuffer buffer;        ///< Buffer to store data to write with
  bool validPosFound = false;   ///< Did we find something to actually execute on ?
  std::string printed;          ///< Print Buffer
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdArrayData arrayData;      ///< Query data about array
  std::list<ecmdArrayData> arrayDataList;      ///< Query data about array

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


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 4) {
    ecmdOutputError("putarray - Too few arguments specified; you need at least a chip, an array, an address, and some data.\n");
    ecmdOutputError("putarray - Type 'putarray -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }

  //Setup the target that will be used to query the system config 
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = target.coreState = ECMD_TARGET_FIELD_WILDCARD;
  target.threadState = ECMD_TARGET_FIELD_UNUSED;

  arrayName = argv[1];


  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;

  while ( ecmdConfigLooperNext(target, looperdata) ) {


    /* We need to find out info about this array */
    rc = ecmdQueryArray(target, arrayDataList , arrayName.c_str(), ECMD_QUERY_DETAIL_LOW);
    if (rc || arrayDataList.empty()) {
      printed = "putarray - Problems retrieving data about array '" + arrayName + "' on ";
      printed += ecmdWriteTarget(target) + "\n";
      ecmdOutputError( printed.c_str() );
      return rc;
    }
    arrayData = *(arrayDataList.begin());

    /* Set the length  */
    address.setBitLength(arrayData.writeAddressLength);
    rc = address.insertFromHexRight(argv[2], 0, arrayData.writeAddressLength);
    if (rc) {
      ecmdOutputError("putarray - Invalid number format detected trying to parse address\n");
      return rc;
    }

    rc = ecmdReadDataFormatted(buffer, argv[3], format, arrayData.width);
    if (rc) {
      ecmdOutputError("putarray - Problems occurred parsing input data, must be an invalid format\n");
      return rc;
    }




    rc = putArray(target, arrayName.c_str(), address, buffer);

    if (rc == ECMD_TARGET_NOT_CONFIGURED) {
      continue;
    }
    else if (rc) {
        printed = "putarray - Error occured performing putarray on ";
        printed += ecmdWriteTarget(target) + "\n";
        ecmdOutputError( printed.c_str() );
        return rc;
    }
    else {
      validPosFound = true;     
    }

    if (!ecmdGetGlobalVar(ECMD_GLOBALVAR_QUIETMODE)) {
      printed = ecmdWriteTarget(target) + "\n";
      ecmdOutput(printed.c_str());
    }

  }

  if (!validPosFound) {
    //this is an error common across all UI functions
    ecmdOutputError("putarray - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  }

  return rc;
}


uint32_t ecmdGetTraceArrayUser(int argc, char * argv[]) {
  uint32_t rc = ECMD_SUCCESS;

  ecmdChipTarget target;                ///< Current target
  ecmdChipTarget coretarget;            ///< Current target being operated on for the cores
  bool validPosFound = false;           ///< Did we find something to actually execute on ?
  std::string printed;                  ///< Print Buffer
  bool printedHeader;                   ///< Have we printed the array name and pos
  ecmdLooperData looperdata;            ///< Store internal Looper data
  ecmdLooperData corelooper;            ///< Store internal Looper data for the core loop
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


  /************************************************************************/
  /* Parse Local ARGS here!                                               */
  /************************************************************************/
  if (argc < 2) {
    ecmdOutputError("gettracearray - Too few arguments specified; you need at least a chip and an array.\n");
    ecmdOutputError("gettracearray - Type 'gettracearray -h' for usage.\n");
    return ECMD_INVALID_ARGS;
  }
  
  
  //Setup the looper to do gettracearray
  target.chipType = argv[0];
  target.chipTypeState = ECMD_TARGET_FIELD_VALID;
  target.cageState = target.nodeState = target.slotState = target.posState = ECMD_TARGET_FIELD_WILDCARD;
  target.coreState = target.threadState = ECMD_TARGET_FIELD_UNUSED;

  
  rc = ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
  if (rc) return rc;
 
  while ( ecmdConfigLooperNext(target, looperdata) ) {
     
     
     //Get all the valid trace arrays
     rc = ecmdQueryTraceArray(target, queryTraceData, NULL, ECMD_QUERY_DETAIL_LOW);
     if (rc == ECMD_TARGET_NOT_CONFIGURED) {
       continue;
     }
     else if (rc) {
       printed = "gettracearray - Error occurred performing querytracearray on ";
       printed += ecmdWriteTarget(target);
       printed += "\n";
       ecmdOutputError( printed.c_str() );
       return rc;
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
        if (queryIt->traceArrayName == traceArrayName) {
          tracearrayfound = true;
          entry.name = traceArrayName;
	  
          if (queryIt->isCoreRelated)	{
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
       
       if (rc == ECMD_TARGET_NOT_CONFIGURED) {
  	continue;
       }
       else if (rc) {
  	printed = "gettracearray - Error occured performing getTraceArrayMultiple on ";
  	printed += ecmdWriteTarget(target) + "\n";
  	ecmdOutputError( printed.c_str() );
  	return rc;
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
     coretarget = target;
     if (coreArrayList.size() > 0) {
       coretarget.chipTypeState = coretarget.cageState = coretarget.nodeState = coretarget.slotState = coretarget.posState = ECMD_TARGET_FIELD_VALID;
       coretarget.coreState = ECMD_TARGET_FIELD_WILDCARD;
       coretarget.threadState = ECMD_TARGET_FIELD_UNUSED;

       /* Init the core loop */
       rc = ecmdConfigLooperInit(coretarget, ECMD_SELECTED_TARGETS_LOOP, corelooper);
       if (rc) return rc;
     

       /* If this isn't a core ring we will fall into while loop and break at the end, if it is we will call run through configloopernext */
       while (ecmdConfigLooperNext(coretarget, corelooper)) {

        //Clear the core List
        for (std::list<ecmdNameVectorEntry>::iterator lit = coreArrayList.begin(); lit != coreArrayList.end(); lit++ ) {
          lit->buffer.clear();
	}
	
        rc = getTraceArrayMultiple(coretarget,  doStopStart, coreArrayList);

  	if (rc == ECMD_TARGET_NOT_CONFIGURED) {
  	  continue;
  	}
  	else if (rc) {
  	  printed = "gettracearray - Error occured performing getTraceArray on ";
  	  printed += ecmdWriteTarget(coretarget) + "\n";
  	  ecmdOutputError( printed.c_str() );
  	  return rc;
  	}
  	else {
          validPosFound = true;
        }

  	for (std::list<ecmdNameVectorEntry>::iterator lit = coreArrayList.begin(); lit != coreArrayList.end(); lit++ ) {
  	  printedHeader = false;
          for(loop =0; loop < lit->buffer.size() ; loop++) {
  	    if (!printedHeader) {
  	      printed = ecmdWriteTarget(coretarget) + " " + lit->name + "\n";
  	      ecmdOutput( printed.c_str() );
  	      printedHeader = true;
  	    }

  	    printed = ecmdWriteDataFormatted(lit->buffer[loop], format);

  	    ecmdOutput( printed.c_str() );
  	  }
  	 }
 
       }/* End CoreLooper */
     
     } /* If core trace */
    
  }/* End ChipLooper */

  if (!validPosFound) {
    //this is an error common across all UI functions
    ecmdOutputError("gettracearray - Unable to find a valid chip to execute command on\n");
    return ECMD_TARGET_NOT_CONFIGURED;
  } 

  return rc;
}


// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder    Description                       
//  ---- -------- ---- -------- -------- ------------------------------   
//                              CENGEL   Initial Creation
//  none STGC7449      04/18/05 prahl    Clean up Beam messages.
//  @01  STGC12283     05/25/05 prahl    Fix memory leak of add_buffer
//
// End Change Log *****************************************************
