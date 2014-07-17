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
  uint32_t ecmdLoadDll(const char * i_dllName, const char * i_clientVersion);


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

  uint32_t ecmdCommandArgs(char** io_argv); 

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

<hr>
@section ext eCMD Extensions
These are extensions to the core eCMD interface, not all eCMD Plugins support these extensions.<br>

\if ECMD_CMD_EXTENSION_SUPPORT
@subsection cmd CMD Command line Extension
This extension provides interfaces to call command line functions and have formatted data displayed to stdout or returned to the caller.  It supports command from the core command line and also all extensions.<br>

Include files :
<ul>
<li> cmdClientPerlapi.H</li>
<li> cmdStructs.H</li>
</ul>
\endif

\if ECMD_CIP_EXTENSION_SUPPORT
@subsection cip CIP (Cronus/IP) Extension
This extension provides interfaces to start/stop processor instructions and breakpoint handling.<br>

Include files :
<ul>
<li> cipClientPerlapi.H </li>
<li> cipStructs.H</li>
</ul>
\endif

\if ECMD_GIP_EXTENSION_SUPPORT
@subsection gip GIP GFW IP-Series Extension
This extension provides IP Series GFW only interfaces.<br>

Include files :
<ul>
<li> gipClientPerlapi.H </li>
<li> gipStructs.H</li>
</ul>
\endif

\if ECMD_CRO_EXTENSION_SUPPORT
@subsection cronus Cronus Extension
This extension provides Cronus only interfaces.<br>

Include files :
<ul>
<li> croClientPerlapi.H </li>
<li> croStructs.H</li>
</ul>
\endif

\if ECMD_ZSE_EXTENSION_SUPPORT
@subsection zse Z Series Extension
This extension provides Z-Series only interfaces.<br>

Include files :
<ul>
<li> zseClientPerlapi.H </li>
<li> zseStructs.H</li>
</ul>
\endif

\if ECMD_EIP_EXTENSION_SUPPORT
@subsection eip EIP (Eclipz I/P) Extension
This extension provides Eclipz only interfaces.<br>

Include files :
<ul>
<li> eipClientPerlapi.H </li>
<li> eipStructs.H</li>
</ul>
\endif

\if ECMD_AIP_EXTENSION_SUPPORT
@subsection aip AIP (Apollo I/P) Extension
This extension provides Apollo only interfaces.<br>

Include files :
<ul>
<li> aipClientPerlapi.H </li>
<li> aipStructs.H</li>
</ul>
\endif

\if ECMD_SCAND_EXTENSION_SUPPORT
@subsection scand ScanD Extension
This extension provides ScanD only interfaces.<br>

Include files :
<ul>
<li> scandClientPerlapi.H</li>
<li> scandStructs.H</li>
</ul>
\endif

\if ECMD_BML_EXTENSION_SUPPORT
@subsection bml BML Extension
This extension provides BML only interfaces and is only available running eCMD in a BML environment.<br>

Include files :
<ul>
<li> bmlClientPerlapi.H </li>
<li> bmlStructs.H</li>
</ul>
\endif

\if ECMD_MBO_EXTENSION_SUPPORT
@subsection mbo MBO (Mambo) Extension
This extension provides Mambo Simulator only interfaces.<br>

Include files :
<ul>
<li> mboClientPerlapi.H </li>
<li> mboStructs.H</li>
</ul>
\endif

<br><br>

 <hr>
 @section perlv Perl Version
 eCMD Scripts need Perl version 5.8.1 which is installed as part of the CTE package on all supported sites.  All scripts using the perl module should start with the following: 

 <pre>
 #!/bin/ksh 
 #! -*- perl -*-

 eval '
 if [ "X$ECMDPERLBIN" = "X" ] ; then
  if [ "X$CTEPATH" = "X" ]; then echo "CTEPATH env var is not set."; exit 1; fi
  if [[ "$ECMD_ARCH" =~ "64" ]]; then
   export ECMDPERLBIN=$CTEPATH/tools/perl/5.8.1/bin64/perl;
   export CTEPERLPATH=$CTEPATH/tools/perl/5.8.1;
   export CTEPERLLIB=$CTEPERLPATH/lib64/5.8.1:$CTEPATH/tools/ecmd/$ECMD_RELEASE/$ECMD_ARCH/perl/:$CTEPERLLIB;
  else
   export ECMDPERLBIN=$CTEPATH/tools/perl/5.8.1/bin/perl;
   export CTEPERLPATH=$CTEPATH/tools/perl/5.8.1;
   export CTEPERLLIB=$CTEPERLPATH/lib/5.8.1:$CTEPATH/tools/ecmd/$ECMD_RELEASE/$ECMD_ARCH/perl:$CTEPERLLIB;
  fi 
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

  # Change the value
  $data->setWord(0,0xAAAA5555);
  printf("Data : %.08X",$data->getWord(0));

  # Write my new value to the chip
  $rc = putRing($target, "idreg", $data);
</pre>

 <b>WARNING: </b> The '=' operator DOES NOT work between ecmdDataBuffer's or any other structure like you would expect it to in C. See 'Perl API Usage' below
<br>
 @subsection uint64_t uint64_t (64 bit data)
 Perl doesn't support 64 bit variables, so eCMD provides an ecmdBit64 class to try to make up for this.
 The following operators have been overloaded with this class :
<ul>
<li> + - * / mod
<li> == != &lt; &lt;= &gt; &gt;=
<li> & | ^ ! ~
<li> ++ --
<li> &lt;&lt; &gt;&gt;
</ul>
This will allow you to do things like the following :
<pre>
  my $addr = new ecmd::ecmdBit64("0x00100FFE00000000");
  $addr |= 0x100200;
  $addr += 100;
  putMemProc($target, $addr, $data);

  $addr->setValue(1000);  # Set to decimal value 1000
  $addr->setValue(0x1000); # Set to hex value 0x1000

  if ($addr != 0x1000) { die ("failed") }
  
  my $addr2 = new ecmd::ecmdBit64();
  $addr->copy($addr2);                # Make addr2 == addr , you CANNOT do a '$addr2 = $addr;' see warning below
  # Or you could do this
  my $addr2 = new ecmd::ecmdBit64($addr);

</pre>
 <b>WARNING: </b> The '=' operator DOES NOT work between ecmdBit64's or any other structure like you would expect it to in C. See 'Perl API Usage' below

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

 To work around this problem you need to copy the contents of a structure manually :
 <ul>
 <li> For ecmdDataBuffer/ecmdBit64 : $data1->copy($data2)
 <li> For Structures : $target2->{cage} = $target->{cage}
 </ul>

 <br>
 <hr>
 @section perlscript Example Perl Script

 Please see the eCMD web page under 'Use eCMD' for an example Perl script at : http://rhea.rch.stglabs.ibm.com/eCMD/


*/


#endif /* _ecmdClientPerlapi_H */
