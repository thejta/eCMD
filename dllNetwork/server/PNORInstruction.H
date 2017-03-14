#ifndef _PNORINSTRUCTION_H
#define _PNORINSTRUCTION_H
//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG

/**
 * @file PNORInstruction.H
 * @brief Provides a class for ffs instructions being passed to the fsp server
 *        to interact with the pnor image for the target
 *
*/


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <Instruction.H>
#include <arpa/inet.h>
#include <list>
#include <stdio.h>
#include <string.h>


/**
 @brief PNORInstruction class
*/
class PNORInstruction : public Instruction {
  public:
    /** @name PNORInstruction Constructors */
    //@{
    /**
     * @brief Default Constructor
     * @post Instruction() is called, version is set to 0x1, and type is set to PNOR
     */
    PNORInstruction(void);

    /**
     * @brief Constructor with all variables passed in
     * @post Instruction() is called, command, partitionOffset, blockSize, partitionEntry, deviceString, pnorFlags, flags, if i_data is not NULL it is copied to data
     */
    PNORInstruction(InstructionCommand i_command, uint32_t i_partitionOffset, uint32_t i_blockSize, std::string i_partitionEntry, std::string i_deviceString, uint32_t i_pnorFlags, uint32_t i_flags, ecmdDataBuffer * i_data = NULL);

    /**
     * @brief Default Destructor
     */
    ~PNORInstruction(void);
    //@}

    // Member Functions
    /** @name Setup Function */
    //@{
    /**
     * @brief Used as alternate way to set variables normally set on construction
     * @retval ECMD_SUCCESS on success
     * @retval nonzero on failure
     * @post command, partitionOffset, blockSize, partitionEntry, deviceString, pnorFlags, flags, if i_data is not NULL it is copied to data
     */
    uint32_t setup(InstructionCommand i_command, uint32_t i_parittionOffset, uint32_t i_blockSize, std::string i_partitionEntry, std::string i_deviceString, uint32_t i_pnorFlags, uint32_t i_flags, ecmdDataBuffer * i_data = NULL);
    //@}

    /** @name Execution Function */
    //@{
    /**
     * @brief Called by the fsp server or sim transfer class
     * @param o_data ecmdDataBuffer with output data
     * @param o_status InstructionStatus with error information
     * @param io_handle Handle * pointer for handle reuse between instructions
     * @retval ECMD_SUCCESS on success
     * @retval nonzero on failure
     * @post executes the appropriate command and sets o_data and o_status accordingly
     * Operates on the following InstructionCommand types: GETPNOR, GETPNORLIST, PUTPNOR
     */
    uint32_t execute(ecmdDataBuffer & o_data, InstructionStatus & o_status, Handle ** io_handle);
    //@}

    /** @name Transport Functions */
    //@{
    /**
     * @brief Flatten all the object data into a uint8_t buffer
     * @param o_data Byte buffer to write the flattened data to
     * @param i_len Number of bytes in the o_data buffer
     * @post o_data buffer has a flattened version of the PNORInstruction - must be pre-allocated
     * Data format (all in network byte order):
     * First Word:      version
     * Second Word:     command
     * Third Word:      type
     * Fourth Word:     flags
     * Fifth Word:      partitionOffset
     * Sixth Word:      blockSize
     * Seventh Word:    pnorFlags
     * Eighth Word:     data size
     * Ninth Word:      deviceString szie
     * Tenth Word:      partitionEntry size
     * Multiple Words:  data
     * Multiple Words:  deviceString   i.e. L03C0E03P00
     * Multiple Words:  partitionEntry i.e. HBB
     */
    uint32_t flatten(uint8_t * o_data, uint32_t i_len) const;

    /**
     * @brief Unflatten object data from a uint8_t buffer into this PSIInstruction
     * @param i_data Byte buffer to read the flattened data from
     * @param i_len Number of bytes in the i_data buffer
     * @post This PNORInstruction is allocated and initialized with the unflattened version of i_data
     * Data format (all in network byte order):
     * First Word:      version
     * Second Word:     command
     * Third Word:      type
     * Fourth Word:     flags
     * Fifth Word:      partitionOffset
     * Sixth Word:      blockSize
     * Seventh Word:    pnorFlags
     * Eighth Word:     data size
     * Ninth Word:      deviceString size
     * Tenth Word:      partitionEntry size
     * Multiple Words:  data
     * Multiple Words:  deviceString   i.e. L03C0E03P00
     * Multiple Words:  partitionEntry i.e. HBB
     */
    uint32_t unflatten(const uint8_t * i_data, uint32_t i_len);

    /**
     * @brief Return number of bytes needed for a buffer to flatten the object
     * @retval Number of bytes needed
     */
    uint32_t flattenSize(void) const;

    /** @name Debug Function */
    //@{
    /**
     * @brief Creates a string with the instruction information in a -debug5.f format
     * @retval String with instruction information
     */
    std::string dumpInstruction(void) const;
    //@}

    /** @name Server Utility Function */
    //@{
    std::string getInstructionVars(const InstructionStatus & i_status) const;
    //@}

  protected:
    std::string deviceString;
    std::string partitionEntry;
    uint32_t partitionOffset;
    uint32_t blockSize;
    uint32_t pnorFlags;
    ecmdDataBuffer data;
};

class Pnor_ffs_entry {

  public:
    Pnor_ffs_entry():name(std::string()), base(0), size(0), pid(0), id(0), type(0), flags(0), actual(0), checksum(0)
    {
    }

    Pnor_ffs_entry( std::string i_name, uint32_t i_base, uint32_t i_size, uint32_t i_pid, uint32_t i_id, uint32_t i_type, uint32_t i_flags, uint32_t i_actual, uint32_t i_checksum ):
                    name(i_name), base(i_base), size(i_size), pid(i_pid), id(i_id), type(i_type), flags(i_flags), actual(i_actual), checksum(i_checksum)
    {
    }

    ~Pnor_ffs_entry()
    {
    }

    uint32_t flatten(uint8_t * o_data, uint32_t i_len) 
    {
        uint32_t rc = 0;
        uint32_t *o_ptr = (uint32_t *) o_data;
        if ( i_len < flattenSize() )
        {
        
        }
        else
        {
            memset( o_data, 0, flattenSize() );
            o_ptr[0] = htonl(base);
            o_ptr[1] = htonl(size);
            o_ptr[2] = htonl(pid);
            o_ptr[3] = htonl(id);
            o_ptr[4] = htonl(type);
            o_ptr[5] = htonl(flags);
            o_ptr[6] = htonl(actual);
            o_ptr[7] = htonl(checksum);
            o_ptr[8] = htonl(name.size());
            if ( name.size() > 0 )
            {
                strcpy( ((char *)(o_ptr + 9)), name.c_str() );
            }
        }

        return rc;
    }

    uint32_t unflatten(const uint8_t *i_data, uint32_t i_len)
    {
        uint32_t rc = 0;
        uint32_t *i_ptr = (uint32_t *) i_data;

        base = ntohl(i_ptr[0]);
        size = ntohl(i_ptr[1]);
        pid = ntohl(i_ptr[2]);
        id = ntohl(i_ptr[3]);
        type = ntohl(i_ptr[4]);
        flags = ntohl(i_ptr[5]);
        actual = ntohl(i_ptr[6]);
        checksum = ntohl(i_ptr[7]);
        name = ((char *)(i_ptr + 9));

        return rc;
    }

    uint32_t flattenSize()
    {
        return (8 * sizeof(uint32_t)) + (1 * sizeof(uint32_t)) + 1 + name.size();
    }

  public:
    std::string name;
    uint32_t base;
    uint32_t size;
    uint32_t pid;
    uint32_t id;
    uint32_t type;
    uint32_t flags;
    uint32_t actual;
    uint32_t checksum;
};

class Pnor_ffs_hdr {

  public:
    Pnor_ffs_hdr():magic(0), version(0), size(0), entry_size(0), entry_count(0), block_size(0), block_count(0), checksum(0)
    {
    }

    Pnor_ffs_hdr( uint32_t i_magic, uint32_t i_version, uint32_t i_size, uint32_t i_entry_size, uint32_t i_entry_count, uint32_t i_block_size, uint32_t i_block_count, uint32_t i_checksum, std::list<Pnor_ffs_entry*> i_entries):
                  magic(i_magic), version(i_version), size(i_size), entry_size(i_entry_size), entry_count(i_entry_count), block_size(i_block_size), block_count(i_block_count), checksum(i_checksum), entries(i_entries)
    {
    }

    ~Pnor_ffs_hdr()
    {
        if ( entries.size() != 0 )
        {
            std::list<Pnor_ffs_entry*>::iterator myIt = entries.begin();
            for ( ; myIt != entries.end(); myIt++ )
            {
                delete (*myIt);
                myIt = entries.erase(myIt);
            }
        }
    }

    uint32_t flatten( uint8_t * o_data, uint32_t i_len )
    {
        uint32_t rc = 0;
        uint32_t *o_ptr = (uint32_t *) o_data;

        if ( i_len < flattenSize() )
        {

        }
        else
        {
            memset( o_data, 0, flattenSize() );
            o_ptr[0] = htonl(magic);
            o_ptr[1] = htonl(version);
            o_ptr[2] = htonl(size);
            o_ptr[3] = htonl(entry_size);
            o_ptr[4] = htonl(entry_count);
            o_ptr[5] = htonl(block_size);
            o_ptr[6] = htonl(block_count);
            o_ptr[7] = htonl(checksum);
            o_ptr[8] = htonl(entries.size());
            uint32_t tmpOffset = 0;
            std::list<Pnor_ffs_entry*>::iterator myIt;
            for ( myIt = entries.begin(); myIt != entries.end(); myIt++ )
            {
                uint32_t entryFlattenSize = (*myIt)->flattenSize();
                o_ptr[9+tmpOffset] = htonl(entryFlattenSize);
                uint8_t *tmpData = new uint8_t[entryFlattenSize];
                (*myIt)->flatten(tmpData, entryFlattenSize);
                tmpOffset += 1;
                memcpy( (o_ptr + 9 + tmpOffset), tmpData, entryFlattenSize );
                tmpOffset += entryFlattenSize;
            }
        }

        return rc;
    }
    
    uint32_t unflatten( uint8_t * i_data, uint32_t i_len )
    {
        uint32_t rc = 0;
        uint32_t * i_ptr = (uint32_t *) i_data;

        magic = ntohl(i_ptr[0]);
        version = ntohl(i_ptr[1]);
        size = ntohl(i_ptr[2]);
        entry_size = ntohl(i_ptr[3]);
        entry_count = ntohl(i_ptr[4]);
        block_size = ntohl(i_ptr[5]);
        block_count = ntohl(i_ptr[6]);
        checksum = ntohl(i_ptr[7]);
        uint32_t countEntries = ntohl(i_ptr[8]);
        uint32_t tmpOffset = 0;
        for ( uint32_t idx=0; idx < countEntries; idx++ )
        {
            uint32_t entrySize = ntohl(i_ptr[9+tmpOffset]);
            tmpOffset += 1;
            Pnor_ffs_entry *tmpEntry = new Pnor_ffs_entry();
            tmpEntry->unflatten( (uint8_t *) (i_ptr + 9 + tmpOffset), entrySize );
            tmpOffset += entrySize;
            entries.push_back( tmpEntry );
        }

        return rc;
    }

    uint32_t flattenSize( )
    {
        uint32_t size = 0;
        size += (8 * sizeof(uint32_t));
        size += 1;
        std::list<Pnor_ffs_entry*>::iterator myIt;
        for ( myIt = entries.begin(); myIt != entries.end(); myIt++ )
        {
            size += 1 + (*myIt)->flattenSize();
        }
        return size;
    }

  public:
    uint32_t magic;
    uint32_t version;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_count;
    uint32_t block_size;
    uint32_t block_count;
    uint32_t checksum;
    std::list<Pnor_ffs_entry*> entries;
};

class Pnor_ffs {

  public:
    Pnor_ffs():hdr(NULL), path(std::string()), offset(0), count(0), buf(NULL), buf_count(0)
    {
    }

    Pnor_ffs(Pnor_ffs_hdr * i_hdr, std::string i_path, off_t i_count, void *i_buf, uint32_t i_buf_count):
             hdr(i_hdr), path(i_path), count(i_count), buf(i_buf), buf_count(i_buf_count)
    {
    }

    ~Pnor_ffs()
    {
        if ( hdr != NULL )
        {
            delete hdr;
        }
    }

  public:
    Pnor_ffs_hdr *hdr;

    std::string path;
    off_t offset;

    uint32_t count;

    void * buf;
    uint32_t buf_count;
};

#endif   // _PNORINSTRUCTION_H
