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
#include <stdio.h>
#include "adal_scom.h"

static const char *fsiraw =
	"/sys/bus/platform/devices/fsi-master/slave@00:00/raw";

#define container_of(ptr, type, member) ({                      \
	const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
	(type *)( (char *)__mptr - offsetof(type,member) );})

enum SCOM_REGS {
	DATA0 = 0,
	DATA1,
	CMD,
	FSI2PIB_RESET = 6,
	STATUS,
	RESET = STATUS,
	JTAG_EMU = 9,
	CHIPID,
	IRQ,
	COMP_MASK,
	TRUE_MASK
};

#define SCOM_ENGINE_OFFSET	0x1000

struct adal_scom {
	adal_t adal;
	int idx;
};
typedef struct adal_scom adal_scom_t;

#define to_scom_adal(x) container_of((x), struct adal_scom, adal)

adal_t * adal_scom_open(const char * device, int flags)
{
	char *str;
	char *prev = NULL;
	adal_scom_t *scom;

	scom = (adal_scom_t *)malloc(sizeof(*scom));
	if (scom == NULL) {
		return NULL;
	}

	memset(scom, 0, sizeof(*scom));

	scom->adal.fd = open(device, flags);
	if (scom->adal.fd == -1) {
		free(scom);
		scom = NULL;
		return NULL;
	}

	str = strstr((char *)device, "scom");
	while (str) {
		prev = str;
		str = strstr(str + 1, "scom");
	}

	if (prev)
		scom->idx = strtol(prev + 4, NULL, 10);

	return &scom->adal;
}

int adal_scom_close(adal_t * adal)
{
	int rc = -1;
	adal_scom_t *scom = to_scom_adal(adal);

	if (!adal)
		return 0;

	rc = close(adal->fd);

	free(scom);
	adal = NULL;

	return rc;

}

ssize_t adal_scom_read(adal_t * adal, void * buf, uint64_t scom_address, unsigned long * status)
{
	int rc = 0, rc1 = 0;

	lseek64(adal->fd, scom_address, SEEK_SET);
	rc = read(adal->fd, buf, SCOM_DATA_LEN);

	rc1 = adal_scom_get_register(adal, STATUS, status);

	return rc;
}

ssize_t adal_scom_write(adal_t * adal, void * buf, uint64_t scom_address,  unsigned long * status)
{
	int rc = 0, rc1 = 0;

	lseek64(adal->fd, scom_address, SEEK_SET);
	rc = write(adal->fd, buf, SCOM_DATA_LEN);

	rc1 = adal_scom_get_register(adal, STATUS, status);

	return rc;
}

int adal_scom_reset(adal_t * adal, scom_adal_reset_t type)
{
	int rc;

	rc = adal_scom_set_register(adal, RESET, 0xFFFFFFFF);

	rc = adal_scom_set_register(adal, FSI2PIB_RESET, 0xFFFFFFFF);
	
	return rc;
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
	if (rc < 0) return rc;

	new_data[0] = old_data[0] & ~(usermask[0]);
	new_data[0] = new_data[0] | ((userbuffer[0]) & (usermask[0]));
	new_data[1] = old_data[1] & ~(usermask[1]);
	new_data[1] = new_data[1] | ((userbuffer[1]) & (usermask[1]));

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


ssize_t adal_scom_get_register(adal_t *adal, int registerNo,
			       unsigned long *data)
{

	int rc;
	int fd = open(fsiraw, O_RDONLY);

	if (fd == -1)
		return -ENODEV;

	lseek(fd, SCOM_ENGINE_OFFSET + ((registerNo & ~0xFFFFFF00) * 4), SEEK_SET);
	rc = read(fd, data, 4);
	
	close(fd);
	return rc;
}

ssize_t adal_scom_set_register(adal_t *adal, int registerNo,
			       unsigned long data)
{
	int rc;
	int fd = open(fsiraw, O_WRONLY);

	if (fd == -1)
		return -ENODEV;

	lseek(fd, SCOM_ENGINE_OFFSET + ((registerNo & ~0xFFFFFF00) * 4), SEEK_SET);
	rc = write(fd, &data, 4);
	
	close(fd);
	return rc;
}
