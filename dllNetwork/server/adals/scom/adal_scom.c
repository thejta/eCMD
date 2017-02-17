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

#define SCOM_DEVPATH_LENGTH	39
#define SCOM_BUSNUM_OFFSET	35

struct adal_scom {
	adal_t adal;
	int statfd;
	char devpath[SCOM_DEVPATH_LENGTH];
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
		return NULL;
	}			       /* 1	    2	      3 */
			     /* 012345678901234567890123456789012345678 */
	strncpy(scom->devpath, "/sys/bus/fsi/drivers/scom/00:00:00:xx/",
		SCOM_DEVPATH_LENGTH);
	str = strstr((char *) device, "scom");
	while (str) {
		prev = str;
		str = strstr(str + 1, "scom");
	}

	if (prev) {
		bus_num = strtol(prev + 4, NULL, 10);
		snprintf(bus_num_str, 3, "%02lx", bus_num);

		scom->devpath[SCOM_BUSNUM_OFFSET] = bus_num_str[0];
		scom->devpath[SCOM_BUSNUM_OFFSET + 1] = bus_num_str[1];
	}

	strncpy(status_path, scom->devpath, SCOM_DEVPATH_LENGTH);
	strncpy(&status_path[SCOM_DEVPATH_LENGTH - 1], "status", 7);
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

int adal_scom_get_chipid(uint32_t *id)
{
	int rc;
	int fd = open(fsiraw, O_RDONLY);

	if (fd == -1)
		return -ENODEV;

	lseek(fd, 0x1028, SEEK_SET);
	rc = read(fd, id, 4);
	if (rc >= 0)
		rc = 0;
	
	close(fd);
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
		rc1 = lseek64(scom->statfd, 0, SEEK_SET);
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
		rc1 = lseek64(scom->statfd, 0, SEEK_SET);
		rc1 = read(scom->statfd, stat, 8);
		*status = strtoul(stat, NULL, 16);
	}
	else
		*status = 0;

	return rc;
}


int adal_scom_reset(adal_t * adal, scom_adal_reset_t type)
{
	int rc = 0;
	int fd;
	adal_scom_t *scom = to_scom_adal(adal);
	char reset_path[64] = { 0 };

	strncpy(reset_path, scom->devpath, SCOM_DEVPATH_LENGTH);
	strncpy(&reset_path[SCOM_DEVPATH_LENGTH - 1], "reset", 6);

	fd = open(reset_path, O_WRONLY);
	if (fd == -1)
		return -ENODEV;

	if (type == SCOMRESETENGINE)
		rc = write(fd, "1", 1);
	else if (type == SCOMRESETFULL)
		rc = write(fd, "full", 4);
	else
		rc = -EINVAL;

	close(fd);

	if (rc > 0)
		rc = 0;

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
        if (rc) return rc;

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

	ssize_t rc=-1;
/*stub out. Still need to figure out how to do this */
        if ((adal != NULL) && (registerNo == 0x100A))
        {
            uint32_t l_chipid = 0;
            rc = adal_scom_get_chipid(&l_chipid);
            if (rc >= 0)
            {
                rc = 4;
                *data = l_chipid;
            }
        }
	else if (registerNo == 0x1007)
	{
		adal_scom_t *scom = to_scom_adal(adal);
		int rc1 = 0;
		char stat[9] = { 0 };
		printf("statfd = %d\n", scom->statfd);
		if (scom->statfd != -1) {
			rc1 = lseek64(scom->statfd, 0, SEEK_SET);
			printf("lseek64 rc = %d\n", rc1);
			rc1 = read(scom->statfd, stat, 8);
			printf("read rc = %d\n", rc1);
			if (rc1 >= 0)
			{
				*data = strtoul(stat, NULL, 16);
				rc = 4;
			}
		}
	}
	return rc;
}

ssize_t adal_scom_set_register(adal_t * adal, int registerNo, unsigned long data)
{

/*stub out. Still need to figure out how to do this */

    return -1;
}
