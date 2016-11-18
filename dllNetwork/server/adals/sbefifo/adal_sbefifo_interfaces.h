/******************************************************************************
 *
 * IBM Confidential
 *
 * Licensed Internal Code Source Materials
 *
 * IBM Flexible Support Processor Licensed Internal Code
 *
 * (c) Copyright IBM Corp. 2015, 2015
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

#ifndef _ADAL_SBEFIFO_INTERFACES_H_
#define _ADAL_SBEFIFO_INTERFACES_H_

#include <adal_base.h>

/*
 * Interface Overview:
 *
 * Common Type(s):
 * ---------------
 *  adal_t - Abstract Data Type for ADAL devices.
 *
 * sbefifo Specific Type(s):
 * ------------------------
 *  adal_sbefifo_request - type used in adal_sbefifo_submuit
 *  adal_sbefifo_reply - type used in adal_sbefifo_submuit
 *
 *
 * Interface(s):
 * -------------
 *		adal_sbefifo_open - Request access to an ADAL device.
 *  	adal_sbefifo_close - Release access of an ADAL device.
 * 
 *      adal_sbefifo_submit - initiate a action with the sbe on the host side
 * 	
 *		adal_sbefifo_request_reset - request a reset of the fifo by the sbe engine
 *      
 *		adal_sbefifo_ffdc_extract - Retreive any pending FFDC information a device may have.
 * 		adal_sbefifo_ffdc_unlock - Remove from black list
 *
 */
 
 #ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief	This interface is used to open a device and associate it with an
 *		adal_t.  It must be the first interface called to ensure that
 *		the device has been opened. The device is closed when the
 *		application terminates or when adal_sbefifo_close() is called on
 *		the device.
 *
  * @post	When the device is opened, sizeof(adal_t) bytes is allocated on
 *		the caller's heap.
 *
 * @param	device	NULL-terminated string of the target device
 *			(i.e. /dev/sbefifo/<n>).
 *
 * @param	flags	Optional device specific open flags,  
 * 			One of the required flags can be bitwise
 *			or'd with zero or more optional flags.
 *
 * @note	The low-level device drivers will support only the flags which
 *			are appropriate for the underlying device.
 *
 * @note	[Required]
 * @note	O_RDWR	Open the device for read and write access.
 *
 * @note	[Optional]
 * @note	O_EXCL	Request exclusive access to the device
 *			(i.e. blocking single-threaded access).
 *
 * @note	See 'man 2 open' for a details about all open flags.
 *
 * @return	Pointer to an adal_t on success, NULL on failure with 'ERRNO'
 *		set as follows.
 * @return	ENXIO	The device in the /dev filesystem can not be found.
 * @return	EIO	A low-level device driver or hardware I/O error occurred.
 * @return	EINVAL	At least one of the input parameters is invalid.
 */
adal_t * adal_sbefifo_open(const char * device, int flags);


/*!
 * @brief	This interface is used to dissociate an adal_t from its
 *		underlying device. If this interface is not called before the
 *		process exits, the device will be closed by the operating system.

 * @pre		The device was opened with a prior call to adal_sbefifo_open().
 *
 * @note	Prior to closing the device, the adal_sbefifo_close() interface will
 *			ensure any buffered data is written to the device 
 *
 * @note	It's important to check the return code from the adal_sbefifo_close()
 *		interface because device operation errors are guaranteed to be
 *		returned as the return code from this interface. Not checking
 *		the return code can lead to silent data lossage.
 *
 * @param	adal	adal_t of the target device.
 *
 * @return	0 on success, -1 on failure with 'ERRNO' set as follows.
 * @return	EIO	A low-level device driver or hardware I/O error occurred.
 * @return	EINVAL	At least one of the input parameters is invalid.
 * @return	EBADF	The file descriptor of the underlying device is not valid.
 */
int adal_sbefifo_close(adal_t * adal);


/*!
 * @brief	This interface is used to send a request to the SBE and
 * 			then receive the reply
 *
 * @pre		The device was opened with a prior call to adal_sbefifio_open().
 *
 * @note	In general, the adal_sbefifo_submit() interface implements a blocking
 *			"read" interface.  However, if the device is opened with the
 *			O_NONBLOCK or O_NDELAY, and there is no free space in the sbe
 * 			upstream fifo, the functions will return immediatly with -1 and
 * 			errno set to EBUSY
 *
 * @param	adal		adal_t of the target device.
 * @param	adal_sbefifo_request		- containing:
 * 				void * data     		- in: data pointer containing the request 
 *				size_t wordcount		- in/out: in: sizeo of request out: number of words written to sbe 
 *				unsigned long status	- status register of the upstream fifo
 *
 * @param	adal_sbefifo_reply
 * 				void * data     		- out: data pointer, sbe reply to get stored here 
 *				size_t wordcount		- in/out: in: sizeo of databuffer in words: out: number of words received
 *				unsigned long status	- status register of the downstream fifo
 * @param	timeout_in_msec				- if timeout happens rc=-1 and errno ETIMEDOUT will get set, reset will get called 
 *
 * @return	The number of bytes copied into 'data' on success, -1 on failure
 *			with 'ERRNO' set as follows.
 * @return	EIO	A low-level device driver or hardware I/O error occurred.
 * @return EBUSY open in O_NONBLOCK, but device is currently busy
 * @return ETIMEDOUT function did not return in time
 * @return	EINVAL	At least one of the input parameters is invalid.
 * @return	EFAULT	There was an internal failure or 'data' is invalid.
 * @return	EBADF	The file descriptor of the underlying device is not valid.
 * @return	EFBIG	SBE did return to much data. Data did not fit into "data" buffer. Data in "data" is valid, but is not complete
 * @return EDEADLK FFDC pending, process is blacklisted
 * 
 * @note
 * * PSEUDO code for submit 
 * 1 check if both fifos are empty
 * 2 setup register to indicate maximum size of reply buffer ( not yet in spec from 12/11/2014 !!
 * 3 write data to upstream fifo 
 * 4 set the EOT flag in upstream fifo
 * 5 wait for data to come back in downstream fifo
 * 6 read until EOT  
 * 		- EOT -> all data reveived
 * 		- EOT -> data buffer full now, HW did autogenerate the EOT because of max len reached (see 2)
 * 						- this is flagged in a but in status register
 * 						- read into internal buffer in junks until "real" EOT is received (bit in status register not set any longer)
 * 						- the extra data is being discarted 
 */
ssize_t adal_sbefifo_submit(adal_t * adal, adal_sbefifo_request * request, adal_sbefifo_reply * reply, unsigned long timeout_in_msec);



/*!
 * @brief	requesting the SBE engine to reset the sbefifo device.
 *		Call will not wait until reset is done, it will return immediatly
 *		user needs to check status to see if reset was executed
 *
 * @pre		The device was opened with a prior call to adal_sbefifo_open().
 *
 * @note	The adal_sbefifo_request_reset() requests a reset to the sbefifo
 * 			by the SBE engine on host side
 * 			may be needed on the startup of the service processor
 * 
 *
 * @param	adal	adal_t of the target device.
 *
 * @return	0 on success, -1 on failure with 'ERRNO' set as follows.
 * @return	EIO	A low-level device driver or hardware I/O error occurred.
 * @return	EINVAL	At least one of the input parameters is invalid.
 * @return	EBADF	The file descriptor of the underlying device is not valid.
 * @return	EDEADLK FFDC pending, process is blacklisted
 */
int adal_sbefifo_request_reset(adal_t * adal);   

/*!
 * @brief	call will reset the sbefifo up and downstream
 *		by writing to register 0x14 of sbefifo
 *
 * @pre		The device was opened with a prior call to adal_sbefifo_open().
 *
 * @param	adal	adal_t of the target device.
 *
 * @return	0 on success, -1 on failure with 'ERRNO' set as follows.
 * @return	EIO	A low-level device driver or hardware I/O error occurred.
 * @return	EINVAL	At least one of the input parameters is invalid.
 * @return	EBADF	The file descriptor of the underlying device is not valid.
 * @return	EDEADLK FFDC pending, process is blacklisted
 */
int adal_sbefifo_reset(adal_t * adal);

/*!
 * @brief	This interface is used to read sbe up or downstream status
 *              The status is also needed to check if a reset was executed
 *
 * @pre		The device was opened with a prior call to adal_sbefifo_open().
 *
 * @param	adal	adal_t of the target device.
 * @param	status  will contain status
 *
 * @return	0 on success, -1 on failure with errno set as follows.
 * @return	EIO	A low-level device driver or hardware I/O error occurred.
  * @return	EINVAL	At least one of the input parameters is invalid.
 * @return	EBADF	The file descriptor of the underlying device is not valid.
 * @return	ENODEV	The device is currently unreachable and/or unavailable.
 */
ssize_t adal_sbefifo_status_upstream(adal_t * adal, unsigned long * status);
ssize_t adal_sbefifo_status_downstream(adal_t * adal, unsigned long * status);


/*!
 * @brief	This interface is used to access the device's FFDC information.
 *
 * @pre		The device was opened with a prior call to adal_sbefifo_open().
 *
 * @param	adal	adal_t of the target device.
 * @param	scope	Specifies the FFDC search mode.
 *	@note	ADAL_FFDC_THREAD is used to extract the queued FFDC data only
*			for the calling thread.
 *	@note	ADAL_FFDC_PROCESS is used to extract the queued FFDC data only
 *		for the calling process.
 * @param	buf	The output destination buffer of the FFDC data read from
 *			the device. This buffer is allocated on the callers heap.
 *			To avoid memory leaks, the caller should be sure to
 *			free(3) 'buf' between each calls to adal_sbe_fifo_ffdc_extract.
 *
 * @note   pretty print functions can be used to format the received buffer 
 * 
 * @return	The number of bytes copied into 'buf' on success, -1 on failure
 *			with 'ERRNO' set as follows. A value of 0 means that there is no
 *			FFDC information available.
 * @return	EIO	A low-level device driver or hardware I/O error occurred.
 * @return	EINVAL	At least one of the input parameters is invalid.
 * @return	EFAULT	There was an internal failure or 'buf' is invalid.
 * @return	EBADF	The file descriptor of the underlying device is not valid.
 */
ssize_t adal_sbefifo_ffdc_extract(adal_t * adal, int scope, void ** buf);

/*!
 * @brief	This interface is used to remove a process/thread from a
 *			device's "blacklist".
 *
 * @pre		The device was opened with a prior call to adal_sbefifo_open().
 *
 * @param	adal	adal_t of the target device.
 * @param	scope	Specifies the FFDC search mode.
 *	@note	ADAL_FFDC_THREAD does remove only calling thread from blacklist
 *	@note	ADAL_FFDC_PROCESS does remove only calling process from blacklist
 *
 * @return	0 on success, -1 on failure with errno set as follows.
 * @return	EIO	A low-level device driver or hardware I/O error occurred.
 * @return	EAGAIN	There is more FFDC data pending.
 * @return	EINVAL	At least one of the input parameters is invalid.
 * @return	EBADF	The file descriptor of the underlying device is not valid.
 * @return	ENODEV	The device is currently unreachable and/or unavailable.
 */
int adal_sbefifo_unlock(adal_t * adal, int scope);



/********************************************************************************************************/
/* Debug functions only 																				*/
/* following fuunctions should not be used in production code , only used for test and debug      		*/
/* they are subject of being removed from code 															*/
/********************************************************************************************************/

/* access to any register */
/* see adal_sbefifo_types.h for matching defines */
ssize_t adal_sbefifo_get_register(adal_t * adal, int registerNo, unsigned long * data);
ssize_t adal_sbefifo_set_register(adal_t * adal, int registerNo, unsigned long data);

#ifdef __cplusplus
}
#endif

#endif    /* _ADAL_SBEFIFO_INTERFACES_H_*/
