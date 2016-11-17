/******************************************************************************
 *
 * IBM Confidential
 *
 * Licensed Internal Code Source Materials
 *
 * IBM Flexible Support Processor Licensed Internal Code
 *
 * (c) Copyright IBM Corp. 2006
 *
 * The source code is for this program is not published or otherwise divested
 * of its trade secrets, irrespective of what has been deposited with the
 * U.S. Copyright Office.
 *
 ******************************************************************************
 *
 *
 *
 *****************************************************************************/

static const char copyright [] __attribute__((unused)) =
"Licensed Internal Code - Property of IBM\n"
"IBM Flexible Support Processor Licensed Internal Code\n"
"(c) Copyright IBM Corp 2006 All Rights Reserved\n"
"US Government Users Restricted Rights - Use, duplication\n"
"or disclosure restricted by GSA ADP Schedule Contract\n"
"with IBM Corp.";


#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include "adal_scom.h"

adal_t * adal_scom_open(const char * device, int flags)
{
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


int adal_scom_close(adal_t * adal)
{
	int rc = -1;

	if (!adal)
		return 0;

	rc = close(adal->fd);

	free(adal);
	adal = NULL;

	return rc;

}
ssize_t adal_scom_read(adal_t * adal, void * buf, uint64_t scom_address, unsigned long * status)
{
	int rc = 0;

  lseek64(adal->fd, scom_address, SEEK_SET);
  rc = read(adal->fd, buf, SCOM_DATA_LEN);
  *status = 0;

	return rc;
}
ssize_t adal_scom_write(adal_t * adal, void * buf, uint64_t scom_address,  unsigned long * status)
{
	int rc = 0;

  lseek64(adal->fd, scom_address, SEEK_SET);
  rc = write(adal->fd, buf, SCOM_DATA_LEN);
  *status = 0;

	return rc;


}


int adal_scom_reset(adal_t * adal, scom_adal_reset_t type)
{
    return 0;
}

ssize_t adal_scom_write_under_mask(adal_t * adal, void * buf, uint64_t scom_address, void * mask, unsigned long * status)
{
	int rc = 0;

	unsigned long old_data[SCOM_DATA_WORDS];
  unsigned long new_data[SCOM_DATA_WORDS];
	unsigned long userbuffer[SCOM_DATA_WORDS];
	unsigned long usermask[SCOM_DATA_WORDS];
	memcpy(userbuffer, buf, SCOM_DATA_LEN);
	memcpy(usermask, mask, SCOM_DATA_LEN);

  rc = adal_scom_read(adal, &old_data, scom_address, status);

	new_data[0] = old_data[0] & ~(usermask[0]);
	new_data[0] =	new_data[0] | ((userbuffer[0]) & (usermask[0]));
	new_data[1] = old_data[1] & ~(usermask[1]);
	new_data[1] =	new_data[1] | ((userbuffer[1]) & (usermask[1]));

  rc = adal_scom_write(adal, &new_data, scom_address, status);

	return rc;
}


ssize_t adal_scom_ffdc_extract(adal_t * adal, int scope, void ** buf)
{
    *buf = NULL;
    return 0;
}

int adal_scom_ffdc_unlock(adal_t * adal, int scope)
{
	return 0;
}

ssize_t adal_scom_get_register(adal_t * adal, int registerNo, unsigned long * data)
{
	ioctl_scom_register_t aregister;
	int rc=0;

	aregister.address=registerNo;
	rc=ioctl(adal->fd, IOCTL_READREG, &aregister);
	*data=aregister.value;
	return rc;
}

ssize_t adal_scom_set_register(adal_t * adal, int registerNo, unsigned long data)
{
	ioctl_scom_register_t aregister;

	aregister.address=registerNo;
	aregister.value=data;
	return(ioctl(adal->fd, IOCTL_WRITEREG, &aregister) ) ;
}
