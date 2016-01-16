#ifndef sedcSpyContainer_h
#define sedcSpyContainer_h

// Class Description *************************************************
//                                                                      
//  Name:  sedcSpyContainer
//  Base class: 
//
//  Description: 
//  Usage:
//
// End Class Description *********************************************

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <sedcAEIClasses.H>
#include <sedcSynonymClasses.H>
#include <sedcEccClasses.H>

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------

typedef enum { // SPY_CONTAINER_TYPES
  SC_NONE,
  SC_AEI,
  SC_SYNONYM,
  SC_EPLATCHES,
  SC_ECCFUNC
} SPY_CONTAINER_TYPES;

class sedcSpyContainer
{
   public:
     // Constructors
     inline sedcSpyContainer():
     valid(false),
     type(SC_NONE),
     hashKey(0x0),
     aeiEntry(NULL),
     synonymEntry(NULL),
     eplatchesEntry(NULL),
     eccfuncEntry(NULL)
     {};

     // Destructor
     virtual ~sedcSpyContainer();

     
     sedcAEIEntry getAEIEntry();  ///< Returns a derefrenced copy of the spy entry
     sedcAEIEntry& getAEIEntryRef();  ///< Returns a reference to the spy entry
     void         setAEIEntry(sedcAEIEntry newAEIEntry);  ///< Takes the data and copies it onto the spyEntry pointer

     sedcSynonymEntry getSynonymEntry();  ///< Returns a derefrenced copy of the synonym entry
     sedcSynonymEntry& getSynonymEntryRef();  ///< Returns a reference tothe synonym entry
     void         setSynonymEntry(sedcSynonymEntry newSynonymEntry);  ///< Takes the data and copies it onto the synonymEntry pointer

     sedcEplatchesEntry getEplatchesEntry();  ///< Returns a derefrenced copy of the eplatches entry
     sedcEplatchesEntry& getEplatchesEntryRef();  ///< Returns a reference to the eplatches entry
     void         setEplatchesEntry(sedcEplatchesEntry newEplatchesEntry);  ///< Takes the data and copies it onto the eplatchesEntry pointer

     sedcEccfuncEntry getEccfuncEntry();  ///< Returns a derefrenced copy of the EccFunc entry
     sedcEccfuncEntry& getEccfuncEntryRef();  ///< Returns a reference to the EccFunc entry
     void         setEccfuncEntry(sedcEccfuncEntry newEccfuncEntry);  ///< Takes the data and copies it onto the EccFuncEntry pointer
     
     void          setName(std::string newName); ///< Sets the name field equal to new name, and gens the hashKey

     // Data
     bool valid;  ///< Is this DC valid?
     SPY_CONTAINER_TYPES type; ///< The type of Data Container this is
     std::string name;          ///< The name associated with the Data Container
     uint64_t hashKey;      ///< A hashKey based off the name - used for lookup

     // Operators
     int operator=(const sedcSpyContainer &rhs);  ///< Used for copies
     sedcSpyContainer(const sedcSpyContainer &rhs);  ///< Used for copies
     int operator<(const sedcSpyContainer &rhs) const;  ///< Used for lower bound
//     int operator==(const sedcSpyContainer &rhs) const;  ///< Used for finds
     /* Inline this speed things up - JTA 09/11/05 */
     inline int operator==(const sedcSpyContainer &rhs) const {
       return (hashKey == rhs.hashKey && name==rhs.name);
     };

  private:  // functions
    sedcAEIEntry* aeiEntry;             ///< The actual pointer to the spy data
    sedcSynonymEntry* synonymEntry;     ///< The actual pointer to the synonym data
    sedcEplatchesEntry* eplatchesEntry; ///< The actual pointer to the eplatches data
    sedcEccfuncEntry* eccfuncEntry;     ///< The actual pointer to the eccfunc data

};
#endif /* sedcSpyContainer_h */
