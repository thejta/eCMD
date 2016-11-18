/******************************************************************************
  *
  * IBM Confidential
  *
  * Licensed Internal Code Source Materials
  *
  * IBM Flexible Support Processor Licensed Internal Code
  *
  * (c) Copyright IBM Corp. 2003, 2012
  *
  * The source code is for this program is not published or otherwise divested
  * of its trade secrets, irrespective of what has been deposited with the
  * U.S. Copyright Office.
  *
  *****************************************************************************/

#include "adal_mbx.h"

#include <stdlib.h>             /* NULL, malloc, free */
#include <unistd.h>             /* close, read, write, fsync */
#include <string.h>             /* memset, memcpy */


/* GLOBAL VARIABLES ARE BAD IN SHARED LIBRARIES, DON'T USE THEM! */
#define ADAL_MBX_MAX_STR	128

static const char * const copyright __attribute__ ((unused)) =
	"Licensed Internal Code - Property of IBM\n"
	"IBM Flexible Support Processor Licensed Internal Code\n"
	"(c) Copyright IBM Corp 2008 All Rights Reserved\n"
	"US Government Users Restricted Rights - Use, duplication\n"
	"or disclosure restricted by GSA ADP Schedule Contract\n"
	"with IBM Corp.";


adal_t * adal_mbx_open(const char * device, int flags) {
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
	if (adal != NULL) {
		/* store away file string for /sys access later
		   use strndup for reasonable limit on path name (128 char) */
		adal->priv = strndup(device, ADAL_MBX_MAX_STR);
	}

	return adal;
}


int adal_mbx_close(adal_t * adal) {

	int rc = 0;

	if (!adal)
		return 0;
  free(adal->priv);
  adal->priv = NULL;
	rc = close(adal->fd);

	free(adal);
	adal = NULL;

	return rc;
}


int adal_mbx_reset(adal_t * adal, int type) {
  return 0;
}
ssize_t adal_mbx_ffdc_extract(adal_t * adal, int scope, void ** buf)
{
  *buf = NULL;
	return 0;
}

int adal_mbx_ffdc_unlock(adal_t * adal, int scope)
{
	return 0;
}


int adal_mbx_scratch(adal_t *adal,
					 adal_mbx_scratch_t  scratch,
					 adal_mbx_scratch_mode_t mode,
					 uint32_t * value) {
	int rc = -1;
  unsigned short reg_address;

	if( (NULL != value) ) {
		reg_address = (unsigned short) scratch;
    reg_address *=4;
    reg_address += 0x38;

    lseek(adal->fd, reg_address, SEEK_SET);
		if( ADAL_MBX_SPAD_READ == mode ) {
      /**
		  * Poison the read value in case we fail and user does not check return
		  * codes.
		  */
			*value = 0xA5A5A5A5;
      rc = read(adal->fd, value, sizeof(uint32_t));
		}
    else {
      rc  = write(adal->fd, value, sizeof(uint32_t));
    }

	} else {
		errno = EBADF;
	}
	return rc;
}


int adal_mbx_get_register(adal_t * adal, unsigned long reg,
		       unsigned long * value)
{
	int rc = -1;
  unsigned long reg_address;

	if( (NULL != value) ) {
    reg_address = reg;
    reg_address &= ~0xFFFFFE00;
    reg_address *=4;

		/**
		* Poison the read value in case we fail and user does not
		* check return codes.
		*/
		*value = 0xA5A5A5A5;

    lseek(adal->fd, reg_address, SEEK_SET);
    rc = read(adal->fd, value, sizeof(unsigned long));

	} else {
		errno = EBADF;
	}
	return rc;
}

int adal_mbx_set_register(adal_t *adal, unsigned long reg,
		       unsigned long value)
{
	int rc = -1;
  unsigned long reg_address;

	reg_address = reg;
  reg_address &= ~0xFFFFFE00;
  reg_address *=4;
  lseek(adal->fd, reg_address, SEEK_SET);
  rc = write(adal->fd, (const void*)value, sizeof(unsigned long));

	return rc;
}


