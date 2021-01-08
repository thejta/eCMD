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
#include <sys/ioctl.h>
#include "adal_scan.h"

#define bitToByte(a)  ( (a+7) /8 )
#define byteToWord(a) ( (a+3) /4 )

#define LENNAME 32
struct name_flags {
    char name[LENNAME];
    int flags;
};


adal_t * adal_scan_open(const char * device, int flags)
{
    adal_t * adal = NULL;
    struct name_flags * nf =NULL;

    adal = (adal_t *)malloc(sizeof(*adal));
    if (adal == NULL) {
        return NULL;
    }
    adal->fd = open(device, flags);
    if (adal->fd == -1) {
        free(adal);
        adal = NULL;
    }

    if (adal != NULL) {
        nf=(struct name_flags *)malloc(sizeof(struct name_flags));
        if (nf) {
            adal->priv=nf;
            memset(nf->name,0,LENNAME);
            strncpy(nf->name,device,LENNAME-1);  // handle -Wstringop-truncation warning and only copy size-1
            nf->flags=flags;
        }
        else {
            adal_scan_close(adal);
            errno = ENOMEM;
            return NULL;
        }
    }

    return adal;
}

int adal_scan_close(adal_t * adal)
{
	int rc = 0;

    if (adal != NULL) {
        free(adal->priv);
    }


    if (adal != NULL) {
        free(adal->priv);
    }

	if (!adal)
		return 0;

	rc = close(adal->fd);

	free(adal);
	adal = NULL;

	return rc;

}
int adal_scan_reset(adal_t * adal, int type)
{
    return 0;
}


ssize_t adal_scan_read(adal_t * adal, void * buf, int chainAddress, size_t bitlength, unsigned long options, unsigned long * status)
{
    int rc=0;
    unsigned long noBytes = bitToByte(bitlength);
    lseek(adal->fd, chainAddress, SEEK_SET);
    rc = read(adal->fd, buf, noBytes);
    *status=0;
    return rc;
}


ssize_t adal_scan_write(adal_t * adal,void * buf, int chainAddress, size_t bitlength, unsigned long options, unsigned long * status)
{
    int rc=0;
    unsigned long noBytes = bitToByte(bitlength);
    lseek(adal->fd, chainAddress, SEEK_SET);
    rc = write(adal->fd, buf, noBytes);
    *status=0;
    return rc;

}


ssize_t adal_scan_ffdc_extract(adal_t * adal, int scope, void ** buf)
{
    *buf = NULL;
    return 0;
}

int adal_scan_ffdc_unlock(adal_t * adal, int scope)
{
	return 0;
}



ssize_t adal_scan_get_register(adal_t * adal, int registerNo, unsigned long * value)
{
    ioctl_scan_register_t aregister;
    int rc=0;

    aregister.address=registerNo;
    rc=ioctl(adal->fd, IOCTL_READREG, &aregister);
    *value=aregister.value;
    return rc;
}

ssize_t adal_scan_set_register(adal_t * adal, int registerNo, unsigned long value)
{
    ioctl_scan_register_t aregister;

    aregister.address=registerNo;
    aregister.value=value;
    return(ioctl(adal->fd, IOCTL_WRITEREG, &aregister) ) ;
}

