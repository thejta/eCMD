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

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <ecmdCommandUtils.H>
#include <ecmdReturnCodes.H>
#include <ecmdClientCapi.H>
#include <ecmdUtils.H>
#include <ecmdDataBuffer.H>
#include <ecmdInterpreter.H>
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
#ifndef ECMD_REMOVE_PNOR_FUNCTIONS
uint32_t ecmdGetPnorUser(int argc, char * argv[]) {

    uint32_t rc = ECMD_SUCCESS;
    ecmdChipTarget l_target;
    ecmdLooperData looperdata;            ///< Store internal Looper data
    ecmdDataBuffer data;                  ///< Data from the pnor api call
    bool validPosFound = false;           ///< Did the looper find anything to execute on
    std::ostringstream oss;
    uint32_t match = 1;
    
    /************************************************************************/
    /* Parse Local FLAGS here!                                              */
    /************************************************************************/

    /* Get the filename if -fb is specified */
    char * filename = ecmdParseOptionWithArgs(&argc, &argv, "-fb");

    bool l_listPnor = ecmdParseOption( &argc, &argv, "-list" );

    if( (filename != NULL) && (l_listPnor) ) 
    {
        oss << "getpnor - Options '-fb' cannot be specified with '--list' option." << std::endl;
        ecmdOutputError(oss.str().c_str());
        return ECMD_INVALID_ARGS;
    } 

    char *partName = ecmdParseOptionWithArgs(&argc, &argv, "-name"); 

    if ( (!l_listPnor) && (filename == NULL || partName == NULL) )
    {
        oss << "getpnor usage incorrect, -fb and -name are required when not using -list" << std::endl;
        oss << "Use 'getpnor -h' for usage" << std::endl;
        ecmdOutputError(oss.str().c_str());
        return ECMD_INVALID_ARGS;
    }

    std::string partitionName;
    if ( partName == NULL )
    {
        partitionName = std::string();
    }
    else
    {
        partitionName = partName;
    }

    char *charOffset = ecmdParseOptionWithArgs(&argc, &argv, "-offset");
    uint32_t offset = 0;
    if ( charOffset != NULL )
    {
        if ( (strchr( charOffset, 'x' )) == NULL && ecmdIsAllDecimal( charOffset ) )
        {
            // no occurance of x in string assuming decimal
            match = sscanf( charOffset, "%d", &offset );
        } 
        else if ( (strchr( charOffset, 'x' )) != NULL || ecmdIsAllHex( charOffset ) )
        {
            char *newCharOffset = strchr( charOffset, 'x' );
            if ( newCharOffset != NULL )
            {
                // move past the x in the string
                newCharOffset++;
            }
            else
            {
                newCharOffset = charOffset;
            }
            // occurrance of x in string, assume hex
            match = sscanf( newCharOffset, "%x", &offset );
        }
        else
        {
            oss << "getpnor - Option '-offset' unable to process value " << charOffset << std::endl;
            ecmdOutputError(oss.str().c_str());
            return ECMD_INVALID_ARGS;
        }
        if ( match != 1 )
        {
            oss << "getpnor - Option '-offset' Error occurred processing value " << charOffset << std::endl;
            ecmdOutputError(oss.str().c_str());
            return ECMD_INVALID_ARGS;
        }
    }

    char* charBlockSize = ecmdParseOptionWithArgs(&argc, &argv, "-block-size");
    uint32_t blockSize = 0;
    if ( charBlockSize != NULL )
    {
        if ( (strchr( charBlockSize, 'x' )) == NULL && ecmdIsAllDecimal( charBlockSize ) )
        {
            // no occurance of x in string assuming decimal
            match = sscanf( charBlockSize, "%d", &blockSize );
        } 
        else if ( (strchr( charBlockSize, 'x' )) != NULL || ecmdIsAllHex( charBlockSize ) )
        {
            char *newCharblockSize = strchr( charBlockSize, 'x' );
            if ( newCharblockSize != NULL )
            {
                // move past the x in the string
                newCharblockSize++;
            }
            else
            {
                newCharblockSize = charBlockSize;
            }
            // occurrance of x in string, assume hex
            match = sscanf( newCharblockSize, "%x", &blockSize );
        }
        else
        {
            oss << "getpnor - Option '-block-size' unable to process value " << charBlockSize << std::endl;
            ecmdOutputError(oss.str().c_str());
            return ECMD_INVALID_ARGS;
        }
        if ( match != 1 )
        {
            oss << "getpnor - Option '-block-size' Error occurred processing value " << charBlockSize << std::endl;
            ecmdOutputError(oss.str().c_str());
            return ECMD_INVALID_ARGS;
        }
    }

    /************************************************************************/
    /* Parse Common Cmdline Args                                            */
    /************************************************************************/
    rc = ecmdCommandArgs(&argc, &argv);
    if (rc) return rc;

    l_target.cageState = l_target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
    l_target.posState = l_target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
    l_target.slotState = l_target.chipUnitTypeState = l_target.chipUnitNumState = l_target.threadState = ECMD_TARGET_FIELD_UNUSED;

    rc = ecmdLooperInit(l_target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
    if (rc) return rc;
              
    if ( l_listPnor )
    {
        while ( ecmdLooperNext(l_target, looperdata) ) 
        {
            validPosFound = true;
            ecmdPnorListData myPnorData;

            rc = ecmdGetPnorList( l_target, partitionName, offset, myPnorData );
            if ( rc ) break;

            std::ostringstream ossPnor;
            ossPnor << "==========================" << "[ PARTITION TABLE 0x";
            ossPnor << std::hex << offset << " ]" << "==========================";
            ossPnor << std::endl;
            ossPnor << "vers:" << std::setfill('0') << std::setw(4) << std::dec << myPnorData.hdrVersion;
            ossPnor << " size:" << std::setfill('0') << std::setw(4) << std::dec << myPnorData.hdrSize;
            ossPnor << " * blk:" << std::setfill('0') << std::setw(6) << std::hex << myPnorData.hdrBlockSize;
            ossPnor << " blk(s):" << std::setfill('0') << std::setw(6) << std::hex << myPnorData.hdrBlockCount;
            ossPnor << " * entsz:" << std::setfill('0') << std::setw(6) << std::hex << myPnorData.hdrEntrySize;
            ossPnor << " ent(s):" << std::setfill('0') << std::setw(6) << std::hex << myPnorData.hdrEntryCount;
            ossPnor << std::endl;
            ossPnor << "---------------------------------------------------------------------------" << std::endl;
            
            std::list<ecmdPnorListEntryData>::iterator myIt;
            for ( myIt = (myPnorData.hdrEntries).begin(); myIt != (myPnorData.hdrEntries).end(); myIt++ )
            {
                if ( partitionName.size() != 0 && ((myIt)->entryName != partitionName) )
                {
                    continue;
                }
                uint32_t baseAddr = myPnorData.hdrBlockSize * (myIt)->entryBase;
                uint32_t sizeAddr = myPnorData.hdrBlockSize * (myIt)->entrySize;
                ossPnor << std::setfill(' ') << std::setw(3) << std::dec << (myIt)->entryId;
                ossPnor << " [" << std::setfill('0') << std::setw(8) << std::hex << baseAddr;
                ossPnor << "-" << std::setfill('0') << std::setw(8) << std::hex << baseAddr+sizeAddr-1;
                ossPnor << "] [" << std::setfill(' ') << std::setw(8) << std::hex << sizeAddr;
                ossPnor << ":" << std::setfill(' ') << std::setw(8) << std::hex << (myIt)->entryActual;
                ossPnor << "] [";
                if ( (myIt)->entryType == ECMD_PNOR_TYPE_LOGICAL )
                {
                    ossPnor << "l";
                }
                else if ( (myIt)->entryType == ECMD_PNOR_TYPE_DATA )
                {
                    ossPnor << "d";
                }
                else if ( (myIt)->entryType == ECMD_PNOR_TYPE_PARTITION )
                {
                    ossPnor << "p";
                }
                else
                {
                    ossPnor << "-";
                }
                ossPnor << "---";
                if ( (myIt)->entryFlags & ECMD_PNOR_FLAGS_U_BOOT_ENV )
                {
                    ossPnor << " ";
                }
                else
                {
                    ossPnor << "-";
                }
                if ( (myIt)->entryFlags & ECMD_PNOR_FLAGS_PROTECTED )
                {
                    ossPnor << "p";
                }
                else
                {
                    ossPnor << "-";
                }
                ossPnor << "] " << (myIt)->entryName;
                ossPnor << std::endl;
            }

            ecmdOutput( ossPnor.str().c_str() );
        }
    }
    else
    {
        while ( ecmdLooperNext(l_target, looperdata) ) 
        {
            validPosFound = true;

            rc = ecmdGetPnor( l_target, partitionName, offset, blockSize, data );
            if ( rc ) break;
            
            rc = data.writeFile( filename, ECMD_SAVE_FORMAT_BINARY_DATA );
        }
    }

    // This is an error common across all UI functions
    if (!validPosFound) {
        ecmdOutputError("getpnor - Unable to find a valid target to execute command on\n");
        return ECMD_TARGET_NOT_CONFIGURED;
    }

    return rc;
}

uint32_t ecmdPutPnorUser(int argc, char * argv[]) {

    uint32_t rc = ECMD_SUCCESS;
    ecmdLooperData looperdata;            ///< Store internal Looper data
    ecmdChipTarget l_target;              ///< Current target operating on
    ecmdDataBuffer data;                  ///< buffer to load a VPD image into, if given
    std::ostringstream oss;
    uint32_t match = 1;

    /************************************************************************/
    /* Parse Local FLAGS here!                                              */
    /************************************************************************/
    char *filename = ecmdParseOptionWithArgs(&argc, &argv, "-fb");
    char *partitionName = ecmdParseOptionWithArgs(&argc, &argv, "-name"); 
    bool erasePartition = ecmdParseOption(&argc, &argv, "-erase");
    char *charEraseValue = ecmdParseOptionWithArgs(&argc, &argv, "-value");   
 
    if( (filename != NULL) && (erasePartition) ) 
    {
        oss << "putpnor usage incorrect, '-fb' and '-erase' cannot be specified together." << std::endl;
        oss << "Use 'putpnor -h' for usage" << std::endl;
        ecmdOutputError(oss.str().c_str());
        return ECMD_INVALID_ARGS;
    } 


    if ( partitionName == NULL )
    {
        oss << "putpnor usage incorrect, '-name' must be specified." << std::endl;
        oss << "Use 'putpnor -h' for usage" << std::endl;
        ecmdOutputError(oss.str().c_str());
        return ECMD_INVALID_ARGS;
    }

    char *charOffset = ecmdParseOptionWithArgs(&argc, &argv, "-offset");
    uint32_t offset = 0;
    if ( charOffset != NULL )
    {
        if ( (strchr( charOffset, 'x' )) == NULL && ecmdIsAllDecimal( charOffset ) )
        {
            // no occurance of x in string assuming decimal
            match = sscanf( charOffset, "%d", &offset );
        } 
        else if ( (strchr( charOffset, 'x' )) != NULL || ecmdIsAllHex( charOffset ) )
        {
            char *newCharOffset = strchr( charOffset, 'x' );
            if ( newCharOffset != NULL )
            {
                // move past the x in the string
                newCharOffset++;
            }
            else
            {
                newCharOffset = charOffset;
            }
            // occurrance of x in string, assume hex
            match = sscanf( newCharOffset, "%x", &offset );
        }
        else
        {
            oss << "putpnor - Option '-offset' unable to process value " << charOffset << std::endl;
            ecmdOutputError(oss.str().c_str());
            return ECMD_INVALID_ARGS;
        }
        if ( match != 1 )
        {
            oss << "putpnor - Option '-offset' Error occurred processing value " << charOffset << std::endl;
            ecmdOutputError(oss.str().c_str());
            return ECMD_INVALID_ARGS;
        }
    }

    char* charBlockSize = ecmdParseOptionWithArgs(&argc, &argv, "-block-size");
    uint32_t blockSize = 0;
    if ( charBlockSize != NULL )
    {
        if ( (strchr( charBlockSize, 'x' )) == NULL && ecmdIsAllDecimal( charBlockSize ) )
        {
            // no occurance of x in string assuming decimal
            match = sscanf( charBlockSize, "%d", &blockSize );
        } 
        else if ( (strchr( charBlockSize, 'x' )) != NULL || ecmdIsAllHex( charBlockSize ) )
        {
            char *newCharblockSize = strchr( charBlockSize, 'x' );
            if ( newCharblockSize != NULL )
            {
                // move past the x in the string
                newCharblockSize++;
            }
            else
            {
                newCharblockSize = charBlockSize;
            }
            // occurrance of x in string, assume hex
            match = sscanf( newCharblockSize, "%x", &blockSize );
        }
        else
        {
            oss << "putpnor - Option '-block-size' unable to process value " << charBlockSize << std::endl;
            ecmdOutputError(oss.str().c_str());
            return ECMD_INVALID_ARGS;
        }
        if ( match != 1 )
        {
            oss << "putpnor - Option '-block-size' Error occurred processing value " << charBlockSize << std::endl;
            ecmdOutputError(oss.str().c_str());
            return ECMD_INVALID_ARGS;
        }
    }

    uint32_t eraseValue = 0;
    if ( charEraseValue != NULL )
    {
        if ( (strchr( charEraseValue, 'x' )) == NULL && ecmdIsAllDecimal( charEraseValue ) )
        {
            // no occurance of x in string assuming decimal
            match = sscanf( charEraseValue, "%d", &eraseValue );
        } 
        else if ( (strchr( charEraseValue, 'x' )) != NULL || ecmdIsAllHex( charEraseValue ) )
        {
            char *newChareraseValue = strchr( charEraseValue, 'x' );
            if ( newChareraseValue != NULL )
            {
                // move past the x in the string
                newChareraseValue++;
            }
            else
            {
                newChareraseValue = charEraseValue;
            }
            // occurrance of x in string, assume hex
            match = sscanf( newChareraseValue, "%x", &eraseValue );
        }
        else
        {
            oss << "putpnor - Option '-value' unable to process value " << charEraseValue << std::endl;
            ecmdOutputError(oss.str().c_str());
            return ECMD_INVALID_ARGS;
        }
        if ( match != 1 )
        {
            oss << "putpnor - Option '-value' Error occurred processing value " << charEraseValue << std::endl;
            ecmdOutputError(oss.str().c_str());
            return ECMD_INVALID_ARGS;
        }
    }

    /************************************************************************/
    /* Parse Common Cmdline Args                                            */
    /************************************************************************/
    rc = ecmdCommandArgs(&argc, &argv);
    if (rc) return rc;

    l_target.cageState = l_target.nodeState = ECMD_TARGET_FIELD_WILDCARD;
    l_target.posState = l_target.chipTypeState = ECMD_TARGET_FIELD_UNUSED;
    l_target.slotState = l_target.chipUnitTypeState = l_target.chipUnitNumState = l_target.threadState = ECMD_TARGET_FIELD_UNUSED;

    rc = ecmdLooperInit(l_target, ECMD_SELECTED_TARGETS_LOOP, looperdata);
    if ( rc ) return rc;

    if ( filename != NULL )
    {
        rc = data.readFile( filename, ECMD_SAVE_FORMAT_BINARY_DATA );
        if ( rc ) return rc;
    }

    while( ecmdLooperNext(l_target, looperdata) )
    {
        if ( erasePartition )
        {
            data.setByteLength( 1 );
            data.setByte( 0, eraseValue );
            rc = ecmdPutPnor( l_target, partitionName, offset, blockSize, data, ECMD_PNOR_MODE_ERASE );
        }
        else
        {
            rc = ecmdPutPnor( l_target, partitionName, offset, blockSize, data, ECMD_PNOR_MODE_DEFAULT );
        }
    }

    return rc;
}
#endif // ECMD_REMOVE_PNOR_FUNCTIONS

