/* File ecmdLoadTest.C created by Joshua Wills on Fri Sep 19 2003. */

/* Change Activity: */
/* End Change Activity */

static const char ibmid[] = "Copyright IBM Corporation 2003 LICENSED MATERIAL - PROGRAM PROPERTY OF IBM";

#include <ecmdClientCapi.H>
#include <ecmdDataBuffer.H>

int main (int argc, char *argv[])
{
  ecmdDataBuffer vec(32);

  ecmdLoadDll("/afs/rchland.ibm.com/usr2/willsj/ecmd/capi/ecmdClientCapi_x86.a");

  ecmdUnloadDll();

}

/* Change Log
<@log@>

Fri Sep 19 2003  16:05:32  by Joshua Wills
<reason><version><Brief description and why change was made.>
*/
