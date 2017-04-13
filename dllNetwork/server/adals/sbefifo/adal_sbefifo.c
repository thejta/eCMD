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
#include <sys/ioctl.h>
#include "adal_sbefifo.h"


struct adal_sbefifo_submit
{
	adal_sbefifo_request request;
	adal_sbefifo_reply reply;
	unsigned long timout_in_msec;
};
typedef struct adal_sbefifo_submit adal_sbefifo_submit_t;


adal_t * adal_sbefifo_open(const char * device, int flags) {
	adal_t * adal = NULL;

	adal = (adal_t *)malloc(sizeof(*adal));
	if (adal == NULL) {
		return NULL;
	}

	adal->fd = open(device, flags);
	if (adal->fd == -1) {
		free(adal);
		adal = NULL;
	}

	return adal;
}

ssize_t adal_sbefifo_submit(adal_t * adal, adal_sbefifo_request * request,
		adal_sbefifo_reply * reply, unsigned long timeout_in_msec) {

	int rc = -1;

	adal_sbefifo_submit_t submit;
	memcpy(&submit.request, request, sizeof(adal_sbefifo_request));
	memcpy(&submit.reply, reply, sizeof(adal_sbefifo_reply));
	submit.timout_in_msec = timeout_in_msec;

	rc = ioctl(adal->fd, IOCTL_SUBMIT, &submit);

	if (rc>0) {
		reply->wordcount=submit.reply.wordcount;
		rc = (reply->wordcount) * 4;
	}
	else
	{
		rc=-1;
	}

	return rc;

}

int adal_sbefifo_close(adal_t * adal) {
	int rc = 0;

	if (!adal)
		return 0;

	rc = close(adal->fd);

	free(adal);
	adal = NULL;

	return rc;
}

int adal_sbefifo_request_reset(adal_t * adal) {

	return 0;
}


ssize_t adal_sbefifo_ffdc_extract(adal_t * adal, int scope, void ** buf) {
  *buf=NULL;
  return 0;
}

int adal_sbefifo_unlock(adal_t * adal, int scope) {

	return 0;

}


