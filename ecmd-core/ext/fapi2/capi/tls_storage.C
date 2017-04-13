//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2017 IBM International Business Machines Corp.
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
