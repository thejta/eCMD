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

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include "adal_scom.h"

#define container_of(ptr, type, member) ({                      \
	const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
	(type *)( (char *)__mptr - offsetof(type,member) );})

struct adal_scom {
	adal_t adal;
	int statfd;
};
typedef struct adal_scom adal_scom_t;

#define to_scom_adal(x) container_of((x), struct adal_scom, adal)

adal_t * adal_scom_open(const char * device, int flags)
{
	long bus_num;
	char bus_num_str[3] = { 0 };
	char status_path[64] = { 0 };
	char *str;
	char *prev = NULL;
	adal_scom_t *scom;

	scom = (adal_scom_t *)malloc(sizeof(*scom));
	if (scom == NULL) {
		return NULL;
	}

	scom->adal.fd = open(device, flags);
	if (scom->adal.fd == -1) {
		free(scom);
		scom = NULL;
	}

	strncpy(status_path, "/sys/bus/fsi/drivers/scom/00:00:00:xx/status", 64);
	str = strstr((char *) device, "scom");
	while (str) {
		prev = str;
		str = strstr(str + 1, (char *)"scom");
	}

	if (prev) {
		bus_num = strtol(prev + 4, NULL, 10);
		snprintf(bus_num_str, 3, "%02lx", bus_num);

		status_path[35] = bus_num_str[0];
		status_path[36] = bus_num_str[1];
	}

	scom->statfd = open(status_path, O_RDONLY);

	return &scom->adal;
}


int adal_scom_close(adal_t * adal)
{
	int rc = -1;
	adal_scom_t *scom = to_scom_adal(adal);

	if (!adal)
		return 0;

	rc = close(adal->fd);
	if (scom->statfd != -1)
		close(scom->statfd);

	free(scom);
	adal = NULL;

	return rc;

}
ssize_t adal_scom_read(adal_t * adal, void * buf, uint64_t scom_address, unsigned long * status)
{
	adal_scom_t *scom = to_scom_adal(adal);
	int rc = 0, rc1 = 0;
	char stat[9] = { 0 };

  lseek64(adal->fd, scom_address, SEEK_SET);
  rc = read(adal->fd, buf, SCOM_DATA_LEN);

	if (scom->statfd != -1) {
		rc1 = read(scom->statfd, stat, 8);
		*status = strtoul(stat, NULL, 16);
	}
	else
 		*status = 0;

	return rc;
}
ssize_t adal_scom_write(adal_t * adal, void * buf, uint64_t scom_address,  unsigned long * status)
{
	adal_scom_t *scom = to_scom_adal(adal);
	int rc = 0, rc1 = 0;
	char stat[9] = { 0 };

  lseek64(adal->fd, scom_address, SEEK_SET);
  rc = write(adal->fd, buf, SCOM_DATA_LEN);

	if (scom->statfd != -1) {
		rc1 = read(scom->statfd, stat, 8);
		*status = strtoul(stat, NULL, 16);
	}
	else
		*status = 0;

	return rc;
}


int adal_scom_reset(adal_t * adal, scom_adal_reset_t type)
{
    return 0;
}

ssize_t adal_scom_write_under_mask(adal_t * adal, void * buf, uint64_t scom_address, void * mask, unsigned long * status)
{
	int rc = 0, rc1 = 0;
	adal_scom_t *scom = to_scom_adal(adal);
	char stat[9] = { 0 };

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

	if (scom->statfd != -1) {
		rc1 = read(scom->statfd, stat, 8);
		*status = strtoul(stat, NULL, 16);
	}
	else
		*status = 0;

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
