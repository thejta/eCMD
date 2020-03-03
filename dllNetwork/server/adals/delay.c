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

#include <delay.h>
#include <time.h>
#include <errno.h>

uint32_t delay(uint64_t i_nanoSeconds)
{
    uint32_t rc = 0;
    if (i_nanoSeconds == 0ull) return rc;

    struct timespec requested;
    struct timespec remaining;
    remaining.tv_sec = 0;
    remaining.tv_nsec = 0;
    if (i_nanoSeconds >= 1000000000ull)
    {
        requested.tv_sec = (time_t) (i_nanoSeconds / 1000000000ull);
        requested.tv_nsec = (long) (i_nanoSeconds % 1000000000ull);
    }
    else
    {
        requested.tv_sec = 0;
        requested.tv_nsec = (long) i_nanoSeconds;
    }

    int sleep_rc = 0;
    do
    {
        errno = 0;
        sleep_rc = nanosleep(&requested, &remaining);
        requested = remaining;
    } while ((sleep_rc != 0) && (errno == EINTR));
    if (sleep_rc)
    {
        rc = 1;
    }
    return rc;
}
