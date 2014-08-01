#ifdef DOCUMENTATION
/**
 * @file ecmdPerlApiTypes.H
 * @brief Definitions of the various eCMD Perl API types
*/


/**
 @brief A scalar is a variable that starts with a dollar sign in perl.
 <p>
 Example:
 my $variable;
 somefunction($variable);

 or

 somefunction("some string");

 or

 somefunction(0xFEEDB0B0);
 </p> 
*/
typedef scalar scalar;

/**
 @brief A scalar that is passed by reference in perl.
 <p>
 Example:
 somefunction(\\$variable);
 </p>
*/
typedef scalarref scalarref;

/**
 @brief An array that is passed by reference in perl.
 <p>
 Example:
 somefunction(\\@variable);
 </p>
*/
typedef arrayref arrayref;

/**
 @brief How to use a list in perl
 <hr>
 Some eCMD functions return data to Perl in the form of a list of elements.  This is the same way this data is handled on the C-Api.<br>

 Functions available on a Perl list :
 <table>
 <tr><td>$list->size()</td><td>Returns the number of entries in the list</td></tr>
 <tr><td>$list->empty()</td><td>Returns True/False as to wether the list is empty</td></tr>
 <tr><td>$list->clear()</td><td>Empty the list</td></tr>
 <tr><td>$list->push_back($entry)</td><td>Push a new entry onto the back of the list</td></tr>
 <tr><td>$list->push_front($entry)</td><td>Push a new entry onto the front of the list</td></tr>
 <tr><td>$entry = $list->pop_back()</td><td>Pop an entry from the back of the list and return</td></tr>
 <tr><td>$entry = $list->pop_front()</td><td>Pop an entry from the front of the list and return</td></tr>
 <tr><td>$list->begin()</td><td>Returns an iterator that points to the beginning of list</td></tr>
 <tr><td>$list->end()</td><td>Returns an iterator that points to end of list</td></tr>
 </table>

 Iterators are pointers to a position in the list, they can be used to walk a list and view all elements.<br>

 Functions available on a Perl list iterator :
 <table>
 <tr><td>$entry = $listIter->getValue()</td><td>Return the entry that this iterator points to</td></tr>
 <tr><td>$listIter->setValue($entry)</td><td>Overwrite the value at this position with the new entry</td></tr>
 <tr><td>$listIter++</td><td>Increment the iterator to next entry</td></tr>
 <tr><td>$listIter--</td><td>Decrement the iterator to previous entry</td></tr>
 <tr><td>$listIter == $listIter2</td><td>Check if two iterators point to the same entry</td></tr>
 <tr><td>$listIter != $listIter2</td><td>Check if two iterators point to different entries</td></tr>
 <tr><td>$listIter->setIter($list->begin())</td><td>Set the iterator pointer to an position in the list</td></tr>
 <tr><td>$listIter2->setIter($listIter->getIter())</td><td>Retrieve the iterator pointer to assign to another iterator</td></tr>

 </table>
 <br>

 Loading data into a list :
 <pre>
   my $isteps = new ecmd::listString();
   my $step = "proc_cfaminit";
   $isteps->push_back($step);
   $step = "proc_runiap";
   $isteps->push_back($step);
   $step = "asic_cfaminit";
   $isteps->push_back($step);
   iStepsByNameMultiple($isteps);
 </pre>

 Displaying data on a list :
 <pre>
   my $istepIter = new ecmd::listStringIterator();
   $istepIter->setIter($isteps->begin());
   while ($istepIter != $isteps->end()) {
     printf(" name: %s", $istepIter->getValue());
     $istepIter++;
   }
 </pre>
*/
typedef List List;

/**
 @brief How to use a vector in perl
 <hr>
 Some eCMD functions return data to Perl in the form of a vector(array) of elements.  This is the same way this data is handled on the C-Api.<br>

 Functions available on a Perl vector :
 <table>
 <tr><td>$vector->size()</td><td>Returns the number of entries in the vector</td></tr>
 <tr><td>$vector->empty()</td><td>Returns True/False as to wether the vector is empty</td></tr>
 <tr><td>$vector->clear()</td><td>Empty the vector</td></tr>
 <tr><td>$vector->push_back($entry)</td><td>Push a new entry onto the back of the vector</td></tr>
 <tr><td>$entry = $vector->pop_back()</td><td>Pop an entry from the back of the vector and return</td></tr>
 <tr><td>$entry = $vector->get($index)</td><td>Retrieve entry at specified index</td></tr>
 <tr><td>$vector->set($index, $entry)</td><td>Overwrite entry at specified index</td></tr>
 <tr><td>$vector->begin()</td><td>Returns an iterator that points to the beginning of vector</td></tr>
 <tr><td>$vector->end()</td><td>Returns an iterator that points to end of vector</td></tr>
 </table>

 Iterators are pointers to a position in the vector, they can be used to walk a vector and view all elements.<br>

 Functions available on a Perl vector iterator :
 <table>
 <tr><td>$entry = $vectorIter->getValue()</td><td>Return the entry that this iterator points to</td></tr>
 <tr><td>$vectorIter->setValue($entry)</td><td>Overwrite the value at this position with the new entry</td></tr>
 <tr><td>$vectorIter++</td><td>Increment the iterator to next entry</td></tr>
 <tr><td>$vectorIter--</td><td>Decrement the iterator to previous entry</td></tr>
 <tr><td>$vectorIter == $vectorIter2</td><td>Check if two iterators point to the same entry</td></tr>
 <tr><td>$vectorIter != $vectorIter2</td><td>Check if two iterators point to different entries</td></tr>
 </table>
 <br>

 Usage of a vector is identical to a list as shown above.
*/
typedef Vector Vector;

/**
 @brief The perl version of the ecmdRingData C API std::list
 @see List
*/
typedef ecmdRingDataList ecmdRingDataList;
/**
 @brief The perl version of the ecmdLatchData C API std::list
 @see List
*/
typedef ecmdLatchDataList ecmdLatchDataList;
/**
 @brief The perl version of the ecmdArrayData C API std::list
 @see List
*/
typedef ecmdArrayDataList ecmdArrayDataList;
/**
 @brief The perl version of the ecmdSpyData C API std::list
 @see List
*/
typedef ecmdSpyDataList ecmdSpyDataList;
/**
 @brief The perl version of the ecmdScomData C API std::list
 @see List
*/
typedef ecmdScomDataList ecmdScomDataList;
/**
 @brief The perl version of the ecmdTraceArrayData C API std::list
 @see List
*/
typedef ecmdTraceArrayDataList ecmdTraceArrayDataList;
/**
 @brief The perl version of the ecmdConnectionData C API std::list
 @see List
*/
typedef ecmdConnectionDataList ecmdConnectionDataList;
/**
 @brief The perl version of the ecmdLatchEntry C API std::list
 @see List
*/
typedef ecmdLatchEntryList ecmdLatchEntryList;
/**
 @brief The perl version of the ecmdScomEntry C API std::list
 @see List
*/
typedef ecmdScomEntryList ecmdScomEntryList;
/**
 @brief The perl version of the ecmdArrayEntry C API std::list
 @see List
*/
typedef ecmdArrayEntryList ecmdArrayEntryList;
/**
 @brief The perl version of the ecmdSpyGroupData C API std::list
 @see List
*/
typedef ecmdSpyGroupDataList ecmdSpyGroupDataList;
/**
 @brief The perl version of the ecmdNameEntry C API std::list
 @see List
*/
typedef ecmdNameEntryList ecmdNameEntryList;
/**
 @brief The perl version of the ecmdIndexEntry C API std::list
 @see List
*/
typedef ecmdIndexEntryList ecmdIndexEntryList;
/**
 @brief The perl version of the ecmdNameVectorEntry C API std::list
 @see List
*/
typedef ecmdNameVectorEntryList ecmdNameVectorEntryList;
/**
 @brief The perl version of the ecmdChipTarget C API std::list
 @see List
*/
typedef ecmdChipTargetList ecmdChipTargetList;
/**
 @brief The perl version of the ecmdI2CCmdEntry C API std::list
 @see List
*/
typedef ecmdI2CCmdEntryList ecmdI2CCmdEntryList;
/**
 @brief The perl version of the ecmdDataBuffer Vector
 @see Vector
*/
typedef ecmdDataBufferVector ecmdDataBufferVector;
#endif
