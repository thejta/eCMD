/* File ecmdLoadTest.C created by Joshua Wills on Fri Sep 19 2003. */

/* Change Activity: */
/* End Change Activity */

static const char ibmid[] = "Copyright IBM Corporation 2003 LICENSED MATERIAL - PROGRAM PROPERTY OF IBM";

#include <ecmdClientCapi.H>
#include <ecmdDataBuffer.H>

int main (int argc, char *argv[])
{
  ecmdDataBuffer vec(32);

#ifdef _AIX
  ecmdLoadDll("../dllStub/export/ecmdDllStub_aix.so");
#else
  ecmdLoadDll("../dllStub/export/ecmdDllStub_x86.so");
#endif

  ecmdUnloadDll();

}
