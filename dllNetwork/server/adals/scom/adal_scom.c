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

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "adal_scom.h"

static const char *fsirawold =
	"/sys/bus/platform/devices/fsi-master/slave@00:00/raw";
static const char *fsiraw =
	"/sys/devices/platform/fsi-master/slave@00:00/raw";

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
#define P1_SLAVE_OFFSET		0x100000

#define DIRECT_SCOM_ADDR	0x7FFFFFFFULL
#define FORM1_SCOM		0x1000000000000000ULL
#define FORM1_SCOM_ADDR		0x00000FFF00000000ULL
#define FORM1_SCOM_SHIFT	20
#define FORM1_SCOM_DATA		0x000FFFFFFFFFFFFFULL
#define INDIRECT_SCOM_READ	0x8000000000000000ULL
#define INDIRECT_SCOM_ADDR	0x000FFFFF00000000ULL
#define INDIRECT_SCOM_READ_DATA	0xFFFFULL

struct adal_scom {
	adal_t adal;
	uint32_t offset;
};
typedef struct adal_scom adal_scom_t;

#define to_scom_adal(x) container_of((x), struct adal_scom, adal)

adal_t * adal_scom_open(const char * device, int flags)
{
	int idx = 1;
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
		idx = strtol(prev + 4, NULL, 10);

	if (idx > 1)
		scom->offset = P1_SLAVE_OFFSET;

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
	int rc = 0;

	if (scom_address & INDIRECT_SCOM_ADDR) {
		uint64_t read_data;
		uint64_t data = (scom_address & INDIRECT_SCOM_ADDR) | INDIRECT_SCOM_READ;

		lseek(adal->fd, (uint32_t)(scom_address & DIRECT_SCOM_ADDR), SEEK_SET);
		rc = write(adal->fd, &data, SCOM_DATA_LEN);

		lseek(adal->fd, (uint32_t)(scom_address & DIRECT_SCOM_ADDR), SEEK_SET);
		rc = read(adal->fd, &data, SCOM_DATA_LEN);

		read_data = data & INDIRECT_SCOM_READ_DATA;

		memcpy(buf, &read_data, SCOM_DATA_LEN);
	}
	else {
		lseek64(adal->fd, scom_address, SEEK_SET);
		rc = read(adal->fd, buf, SCOM_DATA_LEN);
	}

        // ignore rc as status is checked later
	adal_scom_get_register(adal, STATUS, status);

	return rc;
}

ssize_t adal_scom_write(adal_t * adal, void * buf, uint64_t scom_address,  unsigned long * status)
{
	int rc = 0;

	if (scom_address & INDIRECT_SCOM_ADDR) {
		uint64_t data;

		if (scom_address & FORM1_SCOM) {
			uint64_t form1_write_data;

			memcpy(&form1_write_data, buf, 8);

			data = (scom_address & FORM1_SCOM_ADDR) << FORM1_SCOM_SHIFT;
			data |= (form1_write_data & FORM1_SCOM_DATA);
		}
		else {
			uint64_t write_data;

			memcpy(&write_data, buf, SCOM_DATA_LEN);

			data = scom_address & INDIRECT_SCOM_ADDR;
			data |= write_data;
		}

		lseek(adal->fd, (uint32_t)(scom_address & DIRECT_SCOM_ADDR), SEEK_SET);
		rc = write(adal->fd, &data, SCOM_DATA_LEN);
	}
	else {
		lseek64(adal->fd, scom_address, SEEK_SET);
		rc = write(adal->fd, buf, SCOM_DATA_LEN);
	}

        // ignore rc as status is checked later
	adal_scom_get_register(adal, STATUS, status);

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
	{
		fd = open(fsirawold, O_RDONLY);
	}
	adal_scom_t *scom = to_scom_adal(adal);

	if (fd == -1)
		return -ENODEV;

	lseek(fd, scom->offset + SCOM_ENGINE_OFFSET + ((registerNo & ~0xFFFFFF00) * 4), SEEK_SET);
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
	{
		fd = open(fsirawold, O_RDONLY);
	}
	adal_scom_t *scom = to_scom_adal(adal);

	if (fd == -1)
		return -ENODEV;

	lseek(fd, scom->offset + SCOM_ENGINE_OFFSET + ((registerNo & ~0xFFFFFF00) * 4), SEEK_SET);
	rc = write(fd, &data, 4);
	
	close(fd);
	return rc;
}
