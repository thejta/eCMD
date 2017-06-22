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
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <stdio.h>
#include <arpa/inet.h>
#include "adal_sbefifo.h"

static const uint32_t fsirawSize = 3;
static const char *fsiraw[3] = {"/sys/devices/platform/gpio-fsi/fsi0/slave@00:00/raw",   // newest
                                "/sys/devices/platform/fsi-master/slave@00:00/raw",   
                                "/sys/bus/platform/devices/fsi-master/slave@00:00/raw"}; // oldest

#define container_of(ptr, type, member) ({                      \
	const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
	(type *)( (char *)__mptr - offsetof(type,member) );})

#define SBEFIFO_ENGINE_OFFSET 0x2400
#define P1_SLAVE_OFFSET       0x100000

struct adal_sbefifo {
        adal_t adal;
        uint32_t offset;
};
typedef struct adal_sbefifo adal_sbefifo_t;

#define to_sbefifo_adal(x) container_of((x), struct adal_sbefifo, adal)

adal_t * adal_sbefifo_open(const char * device, int flags) {
        int idx = 1;
        char *str;
        char *prev = NULL;
        adal_sbefifo_t *sbefifo;

	sbefifo = (adal_sbefifo_t *)malloc(sizeof(*sbefifo));
        sbefifo->adal.fd = open(device, flags);
	if (sbefifo->adal.fd == -1)
        {   
                free(sbefifo);
                sbefifo = NULL;
		return NULL;
	}

        str = strstr((char *)device, "sbefifo");
        while (str)
        {
                prev = str;
                str = strstr(str+1, "sbefifo");
        }

        if (prev)
                idx = strtol(prev+7, NULL, 10);

        if (idx > 1)
                sbefifo->offset = P1_SLAVE_OFFSET;

	return &sbefifo->adal;
}

ssize_t adal_sbefifo_submit(adal_t * adal, adal_sbefifo_request * request,
		adal_sbefifo_reply * reply, unsigned long timeout_in_msec) {

	int rc = -1;

	struct pollfd pollfd;
	int ret = 0;

	pollfd.fd = adal->fd;
	pollfd.events = POLLOUT | POLLERR;

	if((ret = poll(&pollfd, 1, timeout_in_msec)) < 0) {
		perror("Waiting for fifo device failed");
		return -1;
	}

	if (pollfd.revents & POLLERR) {

		return -1;
	}

	uint8_t * buf = (uint8_t *) request->data;
	int size_bytes = request->wordcount << 2;
        if ( adal_is_byte_swap_needed() ) {
                uint32_t *tmpBuf = (uint32_t *) request->data;
                for ( uint32_t idx = 0; idx < request->wordcount; idx++ ){
                        tmpBuf[idx] = htonl(tmpBuf[idx]);
                }
        }
	ret = write(adal->fd, buf, size_bytes);
	if (ret < 0) {
		perror("Writing user input failed");
		return -1;
	} else if (ret != size_bytes) {
		fprintf(stderr, "Incorrect number of bytes written %d != %d\n", ret, size_bytes);
		return -1;
	}

	pollfd.fd = adal->fd;
	pollfd.events = POLLIN | POLLERR;

	if((ret = poll(&pollfd, 1, timeout_in_msec) < 0)) {
		perror("Waiting for fifo device failed");
		return -1;
	}

	if (pollfd.revents & POLLERR) {
		fprintf(stderr, "POLLERR while waiting for readable fifo\n");
		return -1;
	}

	buf = (uint8_t *) reply->data;
	int ret_bytes = 0;
	size_bytes = reply->wordcount << 2;
	while (size_bytes > 0) {
		if((ret = read(adal->fd, buf + ret_bytes, size_bytes)) < 0) {
			if (errno == EAGAIN)
				break;
			perror("Reading fifo device failed");
			return -1;
		}
		ret_bytes += ret;
		size_bytes -= ret;
	}


	reply->wordcount = ret_bytes >> 2;

        if ( adal_is_byte_swap_needed() ) {
                uint32_t *tmpBuf = (uint32_t *) reply->data;
                for ( uint32_t idx = 0; idx < reply->wordcount; idx++ ){
                        tmpBuf[idx] = ntohl(tmpBuf[idx]);
                }
        }

	rc = ret_bytes;

	return rc;

}

int adal_sbefifo_close(adal_t * adal) {
	int rc = -1;

        adal_sbefifo_t *sbefifo = to_sbefifo_adal(adal);

	if (!adal)
		return 0;

	rc = close(adal->fd);

	free(sbefifo);
	adal = NULL;

	return rc;
}

int adal_sbefifo_request_reset(adal_t * adal) 
{
        int rc = -1;
        rc = adal_sbefifo_set_register(adal, 0x03, 0xFFFFFFFF);
        return rc;
}


ssize_t adal_sbefifo_ffdc_extract(adal_t * adal, int scope, void ** buf) {
        *buf=NULL;
        return 0;
}

int adal_sbefifo_unlock(adal_t * adal, int scope) {

	return 0;

}

ssize_t adal_sbefifo_get_register(adal_t *adal, int registerNo,
			       unsigned long *data)
{

	int rc;
        int fd = 0;
        uint32_t fsiIdx = 0;
        for (fsiIdx=0; fsiIdx < fsirawSize; fsiIdx++ )
        {
                // try an open to find a valid file
                fd = open(fsiraw[fsiIdx], O_RDONLY);
                if (fd != -1)
                {
                        break;
                }
                close(fd);
        } 
	adal_sbefifo_t *sbefifo = to_sbefifo_adal(adal);

	if (fd == -1)
		return -ENODEV;

	lseek(fd, sbefifo->offset + SBEFIFO_ENGINE_OFFSET + ((registerNo & ~0xFFFFFF00) * 4), SEEK_SET);
	rc = read(fd, data, 4);
	
	close(fd);
	return rc;
}

ssize_t adal_sbefifo_set_register(adal_t *adal, int registerNo,
			       unsigned long data)
{
	int rc;
        int fd = 0;
        uint32_t fsiIdx = 0;
        for (fsiIdx=0; fsiIdx < fsirawSize; fsiIdx++ )
        {
                // try an open to find a valid file
                fd = open(fsiraw[fsiIdx], O_WRONLY);
                if (fd != -1)
                {
                        break;
                }
                close(fd);
        } 
	adal_sbefifo_t *sbefifo = to_sbefifo_adal(adal);

	if (fd == -1)
		return -ENODEV;

	lseek(fd, sbefifo->offset + SBEFIFO_ENGINE_OFFSET + ((registerNo & ~0xFFFFFF00) * 4), SEEK_SET);
	rc = write(fd, &data, 4);
	
	close(fd);
	return rc;
}
