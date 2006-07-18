#ifndef _ecmdClientPerlapi_H
#define _ecmdClientPerlapi_H


// Copyright **********************************************************
//
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
// End Copyright ******************************************************
/* $Header$ */


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <string>

#include <ecmdDefines.H>
#include <ecmdStructs.H>
#include <ecmdDataBuffer.H>

/**
 * @file ecmdClientPerlapi.H
 * @brief eCMD Perl API
 * Usage : 
   <pre>
 use ecmd;
   </pre>

*/

#ifndef DOCUMENTATION
namespace ECMDPERLAPI {

#endif

  /** @name Load/Unload Functions */
  //@{

  /**
   @brief Initialize the eCMD DLL
   @retval 0 if successful load  1 if unsuccessful
   @param i_dllName Full path and filename of the eCMD Dll to load
   @param i_clientVersion Comma seperated list of eCMD Perl api major numbers this script supports, see details
   @pre ecmdClientPerlapi constructor must have been called

   - initializes the eCMD Dll.<br>

   VERSIONS :<br>
   eCMD at times has to make changes to add/remove functionality and parameters to functions.  This could cause
   incompatability in your script if you used functions that have changed.  The i_clientVersion string is used
   to tell eCMD which major releases you support such that your script will not continue execution if it encounters
   a version that is either not known about or not supported.  This is similar to how the eCMD C-Api works except in
   Perl you can support multiple versions with one script as long as the changes that were made between the versions
   do not affect your script.

   USAGE :<br>
   if (ecmdLoadDll("","ver3,ver4")) { die "Fatal errors initializing DLL"; }<br>

   */
  int ecmdLoadDll(const char * i_dllName, const char * i_clientVersion);


  /**
   @brief Clean up the Perl Module - MUST BE CALLED JUST BEFORE SCRIPT EXIT
   */
  void ecmdUnloadDll();

  /**
   @brief Pass any unknown command line paramaters to the DLL for processing (ex. -p#, Cronus -debug)
   @retval ECMD_SUCCESS if successful
   @retval nonzero if unsuccessful
   @param io_argv Passed from Command line Arguments
   @pre initDll must have been called
   @post Global options (ex. -p#, -c#) will be removed from arg list
   @see initDll

   - argv gets passed to the eCMD DLL.<br>
   - Global options such as -p#, -c# will be parsed out.<br>
   - Target flags can be queried later with functions like ecmdQuerySelected<br>
   NOTE : This function does not affect ring caching<br>

   USAGE :<br>
   ecmdCommandArgs(\@ARGV);<br>

   */

  int ecmdCommandArgs(char** io_argv); 

  //@}



  /** @name Command Line Parsing Functions */
  //@{

  /**
   * @brief Iterates over argv, looking for given option string, removes it if found
   * @retval 1 if option found, 0 otherwise
   * @param io_argv Array of strings passed in from command line
   * @param i_option Option to look for
   * @see ecmdParseOptionWithArgs

   */
  bool ecmdParseOption (char ** io_argv, const char * i_option);

  /**
   * @brief Iterates over argv, looking for given option string, removes it if found
   * @retval Value of option arg if found, NULL otherwise
   * @param io_argv Array of strings passed in from command line
   * @param i_option Option to look for
   * @see ecmdParseOptionWithArgs

   */
  char * ecmdParseOptionWithArgs(char ** io_argv, const char * i_option);

  //@}


#ifndef DOCUMENTATION
}
#endif




/*** -------------------------------------- ***/
/*** The following is doxygen documentation ***/
/*** -------------------------------------- ***/


/**
 * @mainpage

<br>

@section perlapi eCMD Perl API
 In addition to a C/C++ API, eCMD provides a Perl API.  To do this, Perl's DynaLoader module handles the loading of a special eCMD shared object and the Perl XS interface handles the function calls between eCMD C code and Perl.
<br><br>
 The API interface is implemented as a "ecmdClientPerlapi" object.  The ecmdClientPerlapi class has methods to initialize eCMD, make standard function calls. 
<br>
This Document Last Approved: March 22nd, 2006
 <br>
 <hr>
 @section files Related Information
  NOTE : Some of this documentation may look like C/C++ files/headers but that is a side effect of the tool we are using to generated the PerlAPI and documentation.  Treat as Perl functions and especially look at any usage information associated with each function description

 <ul>
 <li>ecmdClientPerlapi.H</li>
 <li>ecmdDataBuffer</li>
 <li>ecmdStructs.H</li>
 <li>ecmdUtils.H</li>
 <li>ecmdSharedUtils.H</li>
 </ul>

@subsection ext Api Extensions

 CIP (Cronus/IP) Extension<br>
 <ul>
 <li> cipClientPerlapi.H </li>
 </ul>

 CMD (Command Line) Extension<br>
 <ul>
 <li> cmdClientPerlapi.H </li>
 </ul>

 GIP (GFW IP Series) Extension<br>
 <ul>
 <li> gipClientPerlapi.H </li>
 </ul>

 CRO (Cronus) Extension<br>
 <ul>
 <li> croClientPerlapi.H </li>
 </ul>

 Z Series Extension<br>
 <ul>
 <li> zseClientPerlapi.H </li>
 </ul>

 BML Extension<br>
 <ul>
 <li> bmlClientPerlapi.H </li>
 </ul>


 <br><br>

 <hr>
 @section perlv Perl Version
 eCMD Scripts need Perl version 5.8.1 which is installed as part of the CTE package on all supported sites.  All scripts using the perl module should start with the following: 

In an IP Series LAB environment
<pre>
TBD ...
</pre>
<br>
In a CTE only environment
 <pre>
 #!/bin/ksh 
 #! -*- perl -*-

 eval '
 if [ "X$ECMDPERLBIN" = "X" ] ; then
  if [ "X$CTEPATH" = "X" ]; then echo "CTEPATH env var is not set."; exit 1; fi
  export ECMDPERLBIN=$CTEPATH/tools/perl/5.8.1/bin/perl;
  export CTEPERLPATH=$CTEPATH/tools/perl/5.8.1;
  export CTEPERLLIB=$CTEPERLPATH/lib/5.8.1:$CTEPATH/tools/ecmd/$ECMD_RELEASE/perlapi/`uname`/:$CTEPERLLIB;
 fi
 exec $ECMDPERLBIN -x -S $0 ${1+"$@"}
 '
 if 0;

 use strict;
 use ecmd;


 # Initialize the Plugin - has to be first thing done
 # Here we pass in that we know this script supports eCMD Releases 1.x if we know we support more we could pass like "ver1,ver2"
 if (ecmdLoadDll("", "ver1")) { die "Fatal errors initializing DLL"; }


 </pre>


 <br>
 <hr>
 @section extinit Using eCMD Perl Extensions
 If you need functionality from an eCMD Extension that provides the Perl Api, it is initialized like below.  This is a 'cip'
extension example but all extensions work the same way just replace the prefix with the extension of choice.
<br>
<pre>

 # Initialize the Plugin - has to be first thing done 
 if (ecmdLoadDll("", "ver1")) { die "Fatal errors initializing DLL"; }

 # Now initialize the extension as well - notice I use the $cp variable that points to my extension
 if (cipInitExtension("ver1")) { die "Fatal errors initializing CIP Extension"; }

</pre>

 <br>
 <hr>
 @section dform Data Passing
 @subsection ecmdDataBuffer ecmdDataBuffer's
 Data is passed between the client Perl script and the eCMD shared object in the form of the same ecmdDataBuffer that is part of the C-Api.  The function set is the identical.  For additional documentation on the ecmdDataBuffer see the C/C++ Api Documentation.
<br>
Here is an example usage of the ecmdDataBuffer:
<pre>

  # Create a pointer to an DataBuffer class
  my $data = new ecmd::ecmdDataBuffer();

  # Set the size of my buffer
  $data->setBitLength(32);

  # Set the first word of data in this class
  $data->setWord(0,0xFEEDBEEF);

  # Read data from the chip
  my $rc = 0;
  $rc = getRing($target, "idreg", $data);

  # What is in the first word
  printf("Data : %.08X",$data->getWord(0));

  #Change the value
  $data->setWord(0,0xAAAA5555);
  printf("Data : %.08X",$data->getWord(0));

  # Write my new value to the chip
  $rc = putRing($target, "idreg", $data);
</pre>

 <b>WARNING: </b> The '=' operator DOES NOT work between ecmdDataBuffer's or any other structure like you would expect it to in C. See 'Perl API Usage' below
<br>
 @subsection uint64_t uint64_t (64 bit data)
 Perl doesn't support 64 bit variables, so all 64 bit data is converted to a string.
 To specify hex numbers prefix the string with 0x otherwise it is treated as decimal
<pre>
  my $addr = "0x0012a33e884bb338";
  putMemProc($target, $addr, $data);
</pre>

 <br><br>
 <hr>
@section target Chip/Object targeting
Perl functions use the same ecmdChipTarget structure as the C-Api.

<pre>
  my $target = new ecmd::ecmdChipTarget();
  $target->{chipType} = "pu";
  $target->{cage} = 0;
  $target->{node} = 0;
  $target->{slot} = 0;
  $target->{pos} = 1;
  $target->{core} = 0;

  $rc = putRing($target, "idreg", $data);

</pre>
 <b>WARNING: </b> The '=' operator DOES NOT work between ecmdChipTarget's or any other structure like you would expect it to in C. See 'Perl API Usage' below

<br><br>
 <hr>
 @section listuse Using Lists
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


<br><br>
 <hr>
 @section vectoruse Using Vectors
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

Usage of a vector is identical to a list like above.

<br><br>
 <hr>
 @section perluse Perl API Usage

 The following should be observed when using the Perl API.

 <ol>
 <li> The ecmdLoadDll() function should ALWAYS be the first function called.</li>
 <li> The ecmdUnloadDll() function should be called just before script exit</li>
 <li> Perl should be picked up as shown in the example below to grab the eCMD supported version</li>
 </ol>

<br>
 @subsection operator The '=' operator
 <b>WARNING:</b> Because of the behavior of Perl the following statements results in two variables pointing to the same data:
<pre>
  my $data1 = new ecmd::ecmdDataBuffer();
  $data1.setBitLength(32);
  $data1.setWord(0,0xFEEDBEEF);

  # Use the = operator
  my $data2 = $data1;

  printf("Data 1 : %.08X", $data1.getWord(0));        # This will echo 'Data 1 : FEEDBEEF'
  printf("Data 2 : %.08X", $data2.getWord(0));        # This will echo 'Data 2 : FEEDBEEF'

  # Now just set Data 2
  $data2.setWord(0,0xAAAA5555);

  printf("Data 1 : %.08X", $data1.getWord(0));        # This will echo 'Data 1 : AAAA5555'
  printf("Data 2 : %.08X", $data2.getWord(0));        # This will echo 'Data 2 : AAAA5555'
</pre>  
 NOTE : Since both $data1 and $data2 point to the same memory space when one is modified they are both modified.  This behavior is the same for the ecmdDataBuffer and all eCMD structures found in ecmdStructs.H


 <br>
 <hr>
 @section perlscript Example Perl Script

 Please see the eCMD web page under 'Use eCMD' for an example Perl script at : http://rhea.rchland.ibm.com/eCMD/


*/


#endif /* _ecmdClientPerlapi_H */
