
####### LINUX STUFF
g++ -c -I. -I../capi/export -I../ecmd -g -ftemplate-depth-20 ecmdClientPerlapi.C -o ecmdClientPerlapi.o
g++ -c -I. -I../capi/export -I../ecmd -g -ftemplate-depth-20 ../ecmd/ecmdCommandUtils.C -o ecmdCommandUtils.o

/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/tools/swig/linux/swig/bin/swig -I/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/tools/swig/linux/swig/lib -I/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/tools/swig/linux/swig/lib/swig_lib/perl5 -c++ -perl5 -shadow -o ecmdClientPerlapi_wrap.c ecmdClientPerlapi.i

g++ -c -I. -I../capi/export -I../ecmd -I/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/common/contrib/linux/perl/lib/5.6.1/i386-linux/CORE -g -ftemplate-depth-20 ecmdClientPerlapi_wrap.c -o ecmdClientPerlapi_wrap.o

g++ -shared -e dll.exp -o ecmdClientPerlapi.so *.o ../capi/linux/*.a

##### END LINUX STUFF

###### AIX STUFF
xlC -+ -c -I. -I../capi/export -I../ecmd -g -qstaticinline ecmdClientPerlapi.C -o ecmdClientPerlapi.o
xlC -+ -c -I. -I../capi/export -I../ecmd -g -qstaticinline ../ecmd/ecmdCommandUtils.C -o ecmdCommandUtils.o

/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/tools/swig/aix/swig/bin/swig -I/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/tools/swig/aix/swig/lib -I/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/tools/swig/aix/swig/lib/swig_lib/perl5 -c++ -perl5 -shadow -o ecmdClientPerlapi_wrap.c ecmdClientPerlapi.i

xlC -+ -c -I. -I../capi/export -I../ecmd -g -qstaticinline -I/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/common/contrib/aix/perl/lib/5.6.1/aix/CORE -g ecmdClientPerlapi_wrap.c -o ecmdClientPerlapi_wrap.o

/usr/ibmcxx.3666/bin/makeC++SharedLib -e cronuspm.export.new -p 0 -lpthreads -bI:/afs/rchland.ibm.com/rel/common/bringup.tools/cronus/common/contrib/aix/perl/lib/5.6.1/aix/CORE/perl.exp -o ecmdPerlInterface.so *.o ../capi/aix/*.a

### END AIX STUFF
