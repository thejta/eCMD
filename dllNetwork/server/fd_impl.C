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
#include <fd_impl.H>
#include <errno.h>
#include <stdio.h>

ssize_t fd_write(int i_fd, const void *i_ptr, size_t i_len)
{
    ssize_t nleft = i_len;
    ssize_t nwritten, rc = 0;
    const char *ptr = (const char *)i_ptr;

    if (i_fd < 0) // bad file number
    {
        fprintf(stderr,
                "%s %d: fd_write to negative file descriptor\n"
                "Your socket may be closed or not connected\n",
                __FILE__, __LINE__);

        rc = -1;

        errno = EBADF;

    } // bad file number

    while ((nleft > 0) && (rc == 0))
    {
        nwritten = write(i_fd, ptr, nleft);

        if (nwritten <= 0)
        {
            if (errno == EINTR)
            {
                nwritten = 0; // call write again
            }
            else
            {
                rc = - 1;
            }
        }

        else
        {
            nleft -= nwritten;
            ptr   += nwritten;
        }

    } // while ((nleft > 0) && (rc == 0))

    if ((rc == 0) && (nleft == 0))
    {
        rc = i_len;
    }

    return(rc);

} // fd_write

//====================================================================
ssize_t fd_read(int i_fd, void *o_ptr, size_t i_len)
{
    ssize_t  nleft = i_len;
    ssize_t  nread, rc = 0;
    char    *ptr = (char *)o_ptr;

    if (i_fd < 0) // bad file number
    {

        fprintf(stderr,
                "%s %d: fd_read to negative file descriptor\n"
                "Your socket may be closed or not connected\n",
                __FILE__, __LINE__);

        rc = -1;

        errno = EBADF;
                     
    } // bad file number

    while ((nleft > 0) && (rc == 0)) 
    {
        nread = read(i_fd, ptr, nleft);

        if (nread < 0)
        {
            if (errno == EINTR)
            {
                nread = 0; // call read again
            }
            else
            {
                rc = - 1;
            }
        }

        else if (nread == 0) // fd closed
        {
            rc = 0;
            break; // EOF. In Socket case: Socket closed?

        }  // fd closed

        else // positive num byte read
        {
            nleft -= nread;
            ptr   += nread;

        } // positive nun byte read

    } // while ((nleft > 0) && (rc == 0))

    if ((rc == 0) && (nleft == 0))
    {
        rc = i_len;
    }

    return (rc);

} // fd_read
