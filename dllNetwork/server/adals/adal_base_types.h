/******************************************************************************
 * IBM Confidential
 *
 * Licensed Internal Code Source Materials
 *
 * IBM Flexible Support Processor Licensed Internal Code
 *
 * (c) Copyright IBM Corp. 2007, 2008, 2009
 *
 * The source code is for this program is not published or otherwise divested
 * of its trade secrets, irrespective of what has been deposited with the
 * U.S. Copyright Office.
 *****************************************************************************/

#ifndef __ADAL_BASE_TYPES_H__
#define __ADAL_BASE_TYPES_H__

#include <fcntl.h>	/* open interface */
#include <sys/types.h>	/* size_t */
#include <stdint.h>	/* uintX_t */
#include <sys/uio.h>	/* struct iovec */
#include <sys/errno.h>	/* errno */

//#include <dd_ffdc.h>	/* DD_FFDC interfaces */

/*
 * Interface Overview:
 *
 * Data Type(s):
 * -------------
 *                adal_t - The ADAL type.
 *           adal_wait_t - ADAL wait event type.
 *
 */

#define ADAL_MAGIC_NUM       0xADA11ADA
#define ADAL_MAGIC_CHECK(p) (((p) && ((unsigned) (p)->magic == ADAL_MAGIC_NUM)) ? 1 : 0)

#define ADAL_EVENT_WRITE 0x00000004	/*!< in: block until a write will not block */
#define ADAL_EVENT_READ	 0x00000001	/*!< in: block until a read will not block */
#define ADAL_EVENT_ERROR 0x00000008	/*!< out: a device error has occurred */
#define ADAL_EVENT_NVAL  0x00000020	/*!< out: invalid event input specified */
#define ADAL_EVENT_HUP   0x00000010	/*!< out: hung up. (FIXME!! what does this mean?) */
#define ADAL_EVENT_USER  0x0000FFFF	/*!< DEPRECATED */
#define ADAL_EVENT_SCOM        (ADAL_EVENT_USER & 0x0001)
#define ADAL_EVENT_SCOM_CANCEL (ADAL_EVENT_USER & 0x0002)

#define ADAL_ERR	"ADAL_ERR"
#define TRACE_SZ	4096

/* would like to use standard UINT32_MAX, but it is not included by
 * default when using g++, no way to reliably ensure it defined */
#define MAX_UINT32 (4294967295U)
#define ADAL_USER_DEFINED (MAX_UINT32 / 4)

#define ADAL_FFDC_DISABLE   0	/*!< disable fd locking on error */
#define ADAL_FFDC_ENABLE    1	/*!< enable fd locking on error */

#define ADAL_FFDC_USER      ADAL_USER_DEFINED

#define ADAL_RESET_LIGHT	1	/*!< device reset; configuration unchanged */
#define ADAL_RESET_FULL		2	/*!< device reset; configuration reinitialized */
#define ADAL_RESET_USER		ADAL_USER_DEFINED

#define ADAL_CONFIG_READ	1	/*!< read a device config value */
#define ADAL_CONFIG_WRITE	2	/*!< change a device config value */
#define ADAL_CONFIG_USER	ADAL_USER_DEFINED

/* base config > 32 */
#define ADAL_CFG_TFR		0x00000080	/*!< modify TEMP_FAILURE_RETRY */
#define ADAL_CFG_USER		ADAL_USER_DEFINED

#define ADAL_LCTL_LOCK		1	/*!< request device exclusive lock */
#define ADAL_LCTL_UNLOCK	2	/*!< release device exclusive lock */
#define ADAL_LCTL_TEST		3	/*!< test device exclusive lock */
#define ADAL_LCTL_USER		ADAL_USER_DEFINED

#define ADAL_FFDC_DRIVER_FSI	0x01
#define ADAL_FFDC_DRIVER_PSI	0x02
#define ADAL_FFDC_DRIVER_DMA	0x03
#define ADAL_FFDC_DRIVER_HDMA	0x04
#define ADAL_FFDC_DRIVER_MDMA	0x05
#define ADAL_FFDC_DRIVER_IIC	0x06
#define ADAL_FFDC_DRIVER_UART	0x07
#define ADAL_FFDC_DRIVER_JTAG	0x08
#define ADAL_FFDC_DRIVER_MBOX	0x09
#define ADAL_FFDC_DRIVER_DIO	0x0A
#define ADAL_FFDC_DRIVER_SCAN	0x0B
#define ADAL_FFDC_DRIVER_SCOM	0x0C
#define ADAL_FFDC_DRIVER_TONE	0x0D
#define ADAL_FFDC_DRIVER_FAN	0x0E
#define ADAL_FFDC_DRIVER_RTC	0x0F
#define ADAL_FFDC_DRIVER_SPAD	0x10
#define ADAL_FFDC_DRIVER_IOMUX	0x11
#define ADAL_FFDC_DRIVER_DRA	0x12
#define ADAL_FFDC_DRIVER_USB	0x13

/*!@struct Abstract Data Type for ADAL devices */
struct adal_struct {	/*!< this definition is shared between all ADAL devices */
    int magic;
    int fd;		/*!< device file descriptor */
    uint32_t flags;	/*!< device config flags */
    void * priv;	/*!< device specific (private) data */
};
typedef struct adal_struct adal_t;

struct adal_wait {
    adal_t * adal;
    int events;
    int results;
};
typedef struct adal_wait adal_wait_t;

/*!@struct location error identifiers for FRU callouts */
enum adal_base_err_identifier
{
	ADAL_BASE_IDENT_NO_ERROR = 0,	/* no hardwere error detected */
	ADAL_BASE_IDENT_UNKNOWN = 1,	/* Undefined identifier */
	ADAL_BASE_IDENT_PRI_LINK = 2,	/* Main link off of FSP, can be */
					/*   PSI or FSI */
	ADAL_BASE_IDENT_PRI_CFAM = 3,   /* Primary FSI CFAM */
	ADAL_BASE_IDENT_HUB_LINK = 4,   /* Hub FSI link */
	ADAL_BASE_IDENT_HUB_CFAM = 5,   /* Hub FSI CFAM */
	ADAL_BASE_IDENT_SUB_LINK = 6,   /* Cascaded FSI link (sub) */
	ADAL_BASE_IDENT_SUB_CFAM = 7,   /* Cascaded FSI CFAM (sub) */
	ADAL_BASE_IDENT_ENG = 8,	/* Engine failure */
	ADAL_BASE_IDENT_PORT = 9,	/* Port in use on given engine */
	ADAL_BASE_IDENT_PROC = 10,	/* Host processor -PSI only */
	ADAL_BASE_IDENT_FSP = 11,	/* Error on FSP chip  */
	ADAL_BASE_IDENT_FSP_CARD = 12,	/* Error on FSP card */
	ADAL_BASE_IDENT_PREVIOUS = 13	/* Error likely located in component */
					/* one above current in ffdc chain */
};

#endif /* __ADAL_BASE_TYPES_H__ */
