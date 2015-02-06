#include <thread>
#include <return_code.H>

/*
/opt/rh/devtoolset-2/root/usr/bin/g++ -S -o tls_storage.S tls_storage.C -std=c++11

.globl  _ZTHN5fapi211current_errE
_ZTHN5fapi211current_errE = __tls_init
*/

namespace fapi2
{
    thread_local ReturnCode current_err;
}
