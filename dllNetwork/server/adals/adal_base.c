//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2003,2017 IBM International Business Machines Corp.
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

#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include "adal_base.h"

bool adal_is_byte_swap_needed( ) {
    struct utsname unameBuf;

    char *str, *token, *prvToken, *saveptr;

    int rc = uname(&unameBuf);
    if ( rc ) return false;

    // idx 1 < 4 return false, version is less than 4.10
    // idx 1 > 4 return true, version is greater than 4.10
    // idx 2 < 10 and idx 1 == 4 return false, version is less than 4.10
    // idx 2 >= 10 and idx 1 == 4 return true, version is greater than 4.10
    str = unameBuf.release;
    for( int idx=1; ; idx++, str=NULL )
    {
        token = strtok_r( str, ".", &saveptr );
        if ( token == NULL )
            break;
        if ( idx == 1 && atoi(token) < 4 )
            return false;
        else if ( idx == 1 && atoi(token) > 4 )
            return true;
        else if ( idx == 2 && atoi(token) < 10 && atoi(prvToken) == 4 )
            return false;
        else if ( idx == 2 && atoi(token) >= 10 && atoi(prvToken) == 4 )
            return true;
        else if ( idx > 2 )
            break;
        prvToken = token;
    }
    return false;
}

