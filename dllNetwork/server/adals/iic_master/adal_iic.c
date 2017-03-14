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

/*
 *   Desc: IIC Access Device Abstraction Layer (a.k.a ADAL).
 */

static const char copyright [] __attribute__((unused)) =
          "Licensed Internal Code - Property of IBM\n"
          "IBM Flexible Support Processor Licensed Internal Code\n"
          "(c) Copyright IBM Corp 2006 All Rights Reserved\n"
          "US Government Users Restricted Rights - Use, duplication\n"
          "or disclosure restricted by GSA ADP Schedule Contract\n"
          "with IBM Corp.";

#include <adal_iic.h>
#include <iic_dd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


/*
 * Secure IIC
 */
#define SIIC_CMD_COPY 0xE1      /* copy main to staging */
#define SIIC_CMD_STORE 0xD2     /* copy staging to main */
#define SIIC_CMD_CLRINT 0xB4    /* clear selected IRQ's */
#define SIIC_CMD_OFFSET 0x00FF  /* Command register offset */
#define SIIC_CMD_LEN    1       /* Size of a Secure IIC command */

#define SIIC_MK_OFFSET(t) (((t) << 8) | ((~(t)) & 0x000000ff))


/******************************************************************************
 */

adal_t *adal_iic_open(const char *device, int flags)
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

int adal_iic_close(adal_t * adal)
{
	int rc = 0;

	if (!adal)
		return 0;
  //free(adal->priv);
  //adal->priv = NULL;
	rc = close(adal->fd);

	free(adal);
	adal = NULL;

	return rc;
}

ssize_t adal_iic_read(adal_t * adal, void * buf, size_t count)
{
	int rc = -1;
	int total = 0;
	uint8_t * l_buf = (uint8_t *) buf;

	/* buf/count are validated in libc (sysdeps/generic/read.c) */

	do {
		rc = read(adal->fd, l_buf, count) ;
		if (rc == -1) return rc;
		if (rc == 0) break;

		count -= rc;

		l_buf += rc;
		total += rc;
	} while (count != 0);
	rc = total;

	return rc;
}

ssize_t adal_iic_write(adal_t * adal, const void * buf, size_t count)
{
	int rc = -1;
	int total = 0;
	uint8_t * l_buf = (uint8_t *) buf;

	/* buf/count are validated in libc (sysdeps/generic/write.c) */

	do {
		rc = write(adal->fd, l_buf, count);
		if (rc == -1) return rc;
		if (rc == 0) break;

		count -= rc;
		l_buf += rc;
		total += rc;

	} while (count != 0);
	rc = total;

	return rc;
}


ssize_t adal_iic_ffdc_extract(adal_t * adal, int scope, void ** buf)
{
	return 0;
}

int adal_iic_ffdc_unlock(adal_t * adal, int scope)
{
	return 0;
}



/******************************************************************************
 * size parameter is ignored by adal and device driver.
 */
int adal_iic_config(adal_t * adal, int type, adal_iic_cfg_t cfg, void * value,
		    size_t size)
{
    int rc = -1;
    errno = ENOSYS;
    int _ioc;

    /* things to consider: */
    /*   1) interrupted syscall (ie. errno == EINTR) */
    /*   2) unusual errno's: EAGAIN, EFBIG, ENOSPC, EIO */

    if(cfg > (adal_iic_cfg_t)IIC_IOC_MAXNR)
    {
	    /* Can't handle the cfg value
	     */
	    goto exit;
    }

    if(type == ADAL_CONFIG_WRITE)
    {
	    _ioc = _IOW(IIC_IOC_MAGIC, cfg, long);
    }
    else if(type == ADAL_CONFIG_READ)
    {
	    _ioc = _IOR(IIC_IOC_MAGIC, cfg, long);
    }
    else
    {
        errno = -EINVAL;
        goto exit;
    }
    do{
        rc = ioctl(adal->fd, _ioc, value);
    }while ((rc < 0) && (errno == EINTR));

exit:
    return rc;
}




