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

/*
 *   Desc: Access Device Abstraction Layer (a.k.a ADAL) for the scan device.
 *
 * Author: International Business Machines, Inc. (c) Copyright IBM Corporation 2003.
 *         Shaun Wetzstein <shaun@us.ibm.com>
 *
 * Change: 10/23/03 <shaun@us.ibm.com>
 *         Initial creation.
 *         12/12/03 xxpetri@de.ibm.com
 *         adaption for scan
 */

#ifndef __ADAL_SCAN_INTERFACES_H__
#define __ADAL_SCAN_INTERFACES_H__

#include <unistd.h>

#include <adal_base.h>
//#include <libaio.h>
//#include <adal_aio.h>

/*
 * Interface Overview:
 *
 * Common Type(s):
 * ---------------
 *              adal_t - Abstract Data Type for ADAL devices.
 *       adal_config_t - Get or Set configuration specifier.
 *        adal_reset_t - Reset type.
 *        adal_event_t - Wait even type.
 *
 * scan Specific Type(s):
 * ------------------------
 *  adal_scan_config_t - scan Configuration parameters.
 *
 *
 * Interface(s):
 * -------------
 *        adal_scan_open - Request access to an ADAL device.
 *       adal_scan_close - Release access of an ADAL device.
 *       adal_scan_reset - Stop activity and force device into a "known" state.
 *
 *        adal_scan_read - Read data from a device.
 *       adal_scan_write - Write data to a device.
 *      adal_scan_offset - Change the read/write position of a device.
 *       adal_scan_flush - Suspend activity on a device until all pending
 *                         or queued operations are complete.
 *
 *        adal_scan_wait - Wait and/or check for events to occur on a device.
 *      adal_scan_config - Change a device's operating parameters.
 *adal_scan_ffdc_extract - Retreive any pending FFDC information a
 *                         a device may have.
 *adal_scan_ffdc_extract_identifier - Retreive any pending FFDC information a
 *                         a device may have with its location.
 * adal_scan_ffdc_unlock - Remove from black list
 *
 */


#ifdef __cplusplus
extern "C" {
#endif

/*!
 *
 * @brief	These interfaces are used to enable/disable the workaround for
 *		ppc476 erratum #48.  The workaround consists disabling the ppc476
 *		support for un-aligned loads and stores.  With the
 *		workaround enabled, any un-aligned access will trigger an
 *		exception and the kernel handler will emulate.  The API is
 *		provided so that applications that wish to both:
 *		1 - run code with un-aligned acceses -and-
 *		2 - maintain performace with FLSTA = 0
 *		may run exposed to erratum #48.
 *
 */
int32_t adal_scan_enable_align_wa(adal_t *dev);
int32_t adal_scan_disable_align_wa(adal_t *dev);

/*!
 * @brief	This interface is used to open a device and associate it with an
 *		adal_t.  It must be the first interface called to ensure that
 *		the device has been opened. The device is closed when the
 *		application terminates or when adal_scan_close() is called on
 *		the device.
 *
 * @post	When the device is opened it is configured with the default set
 *		of parameter values. The adal_scan_config() can be used to modify
 *		the device's operating characteristics.
 *
 * @post	When the device is opened, sizeof(adal_t) bytes is allocated on
 *		the caller's heap.
 *
 * @param	device	NULL-terminated string of the target device
 *			(i.e. /dev/scan/<n>).
 *
 * @param	flags	Optional device specific open flags, use 0 for the
 *			device default.  One of the required flags can be bitwise
 *			or'd with zero or more optional flags.
 *
 * @note	The low-level device drivers will support only the flags which
 *		are appropriate for the underlying device.
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
 * @return	EBUSY	The request would block waiting for the device,
 *			if O_NONBLOCK was specified in the 'flags' parameter.
 */
adal_t * adal_scan_open(const char * device, int flags);

/*!
 * @brief	This interface is used to dissociate an adal_t from its
 *		underlying device. If this interface is not called before the
 *		process exits, the device will be closed by the operating system.

 * @pre		The device was opened with a prior call to adal_scan_open().
 *
 * @note	Prior to closing the device, the adal_scan_close() interface will
 *		ensure any buffered data is written to the device by calling the
 *		adal_scan_flush() interface.
 *
 * @note	It's important to check the return code from the adal_scan_close()
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
int adal_scan_close(adal_t * adal);

/*!
 * @brief	This interface is used to force the underdying device, associated
 *		with an adal_t, into a known state.
 *
 * @pre		The device was opened with a prior call to adal_scan_open().
 *
 * @note	The adal_scan_reset() interface can be used to ensure the device
 *		is in a "sane" state suitable for operation.
 *
 * @param	adal	adal_t of the target device.
 * @param	type	Reset type.
 *
 * @return	0 on success, -1 on failure with 'ERRNO' set as follows.
 * @return	EIO	A low-level device driver or hardware I/O error occurred.
 * @return	EINVAL	At least one of the input parameters is invalid.
 * @return	EBADF	The file descriptor of the underlying device is not valid.
 */
int adal_scan_reset(adal_t * adal, int type);

/*!
 * @brief	This interface is used to read data from the underlying device
 *		associated with an adal_t.
 *
 * @pre		The device was opened with a prior call to adal_scan_open().
 *
 * @note	In general, the adal_scan_read() interface implements a blocking
 *		"read" interface.  However, if the device is opened with the
 *		O_NONBLOCK or O_NDELAY, neither the adal_scan_open() nor any
 *		subsequent operations on the file descriptor will cause the
 *		calling process to wait.  The O_NONBLOCK or O_NDELAY options
 *		will only be implemented for devices where appropiate, all
 *		other devices will return EINVAL if O_NONBLOCK or O_NDELAY are
 *		specified.
 *
 * @note	The maximum length for a data operation is determined by the
 *		capabilities of the target device hardware. For devices which
 *		have data size limitations, EFBIG will be returned for requests
 *		which exceed the device capabilities.
 *
 * @param	adal		adal_t of the target device.
 * @param	buf		The destination buffer of the data read from the
 *				device.
 * @param	chainAddress
 * @param	bitlength
 * @param	options		Options to be used for scan operations.
 * @param	status		Status of last scan operation.
 *
 * @return	The number of bytes copied into 'buf' on success, -1 on failure
 *		with 'ERRNO' set as follows.
 * @return	EIO	A low-level device driver or hardware I/O error occurred.
 * @return	EINVAL	At least one of the input parameters is invalid.
 * @return	EFAULT	There was an internal failure or 'buf' is invalid.
 * @return	EBADF	The file descriptor of the underlying device is not valid.
 * @return	EFBIG	If 'count' is larger then the maximum supported device
 *		operation length.
 */
ssize_t adal_scan_read(adal_t * adal, void * buf, int chainAddress,
		size_t bitlength, unsigned long options, unsigned long * status);

/*!
 * @brief	This interface is used to read and process data from the
 *		underlying device associated with an adal_t. The bits are first
 *		read from the device, second altered according to bitmaps and
 *		then put into the target buffer.
 *
 * @pre		The device was opened with a prior call to adal_scan_open().
 *
 * @note	In general, the adal_scan_read_bitmap() interface implements a
 *		blocking "read" interface. However, if the device is opened
 *		with the O_NONBLOCK or O_NDELAY, neither the adal_scan_open()
 *		nor any subsequent operations on the file descriptor will cause
 *		the calling process to wait. The O_NONBLOCK or O_NDELAY options
 *		will only be implemented for devices where appropriate, all
 *		other devices will return EINVAL if O_NONBLOCK or O_NDELAY are
 *		specified.
 *
 * @note	The maximum length for a data operation is determined by the
 *		capabilities of the target device hardware. For devices which
 *		have data size limitations, EFBIG will be returned for requests
 *		which exceed the device capabilities.
 *
 * @param	adal		adal_t of the target device.
 * @param	buf		The destination buffer of the data read from
 *				the device.
 * @param	bufLength	The length of buffer.
 * @param	maps		Array of bitmap descriptors
 * @param	count		Number of entries in the array of bitmap
 *				descriptors.
 * @param	chain		Address of scan chain.
 * @param	bitLength	Length of scan chain in bits.
 * @param	options		Options to be used for scan operations.
 * @param	status		Status of last scan operation.
 *
 * @return	The number of bytes copied into 'buf' on success, -1 on failure
 *		with 'ERRNO' set as follows.
 * @return	EIO	A low-level device driver or hardware I/O error occurred.
 * @return	EINVAL	At least one of the input parameters is invalid.
 * @return	EFAULT	There was an internal failure or 'buf' is invalid.
 * @return	EBADF	The file descriptor of the underlying device is not valid.
 */
ssize_t adal_scan_read_bitmap(adal_t * adal, void * buf, size_t bufLength,
		int chain, size_t bitLength, unsigned long options,
		const bitmap_t * maps, size_t count, unsigned long * status);

/*!
 * @brief	This interface is used to read-modify-writes data from/to the
 *		underlying device associated with an adal_t. The bits are first
 *		read from the device, second altered according to bitmaps and
 *		then written back to the device.
 *
 * @pre		The device was opened with a prior call to adal_scan_open().
 *
 * @note	The read-modify-write cycle is not atomic! Even if the device
 *		was opened with appropriate flag(s) (i.e. O_SYNC) a concurrent
 *		process might interfere and read out or even update the chain
 *		in between.
 *
 * @note	This behaviour is going to change once the bitmap facility is
 *		implemented in the hardware. As of now, it is an extra on top of
 *		'adal_scan_read' and 'adal_scan_write' subsequent calls.
 *
 * @note	As a consequence, handling of O_NONBLOCK, O_NDELAY, O_SYNC (and
 *		other) file descriptor flags is a mixture of 'adal_scan_read'
 *		and 'adal_scan_write' behaviours. Please, refer to these
 *		interfaces for the descriptions.
 *
 * @param	adal		adal_t of the target device.
 * @param	buf		The source buffer of the bits to be applied to
 *				the data read.
 * @param	bufLength	The length of 'buf'.
 * @param	maps		Array of bitmap descriptors
 * @param	count		Number of entries in the array of bitmap
 *				descriptors.
 * @param	chain		Address of scan chain.
 * @param	bitLength	Length of scan chain in bits.
 * @param	options		Options to be used for scan operations.
 * @param	status		Status of last scan operation.
 *
 * @return	The number of bytes copied into 'buf' on success, -1 on failure
 *		with 'ERRNO' set as follows.
 * @return	EIO	A low-level device driver or hardware I/O error occurred.
 * @return	EINVAL	At least one of the input parameters is invalid.
 * @return	EFAULT	There was an internal failure or 'buf' is invalid.
 * @return	EBADF	The file descriptor of the underlying device is not valid.
 */
ssize_t adal_scan_write_bitmap(adal_t * adal, const void * buf, size_t bufLength,
		int chain, size_t bitLength, unsigned long options,
		const bitmap_t * maps, size_t count, unsigned long * status);

/*!
 * @brief	This interface is used to write data to the underlying device
 *		associated with an adal_t.
 *
 * @pre		The device was opened with a prior call to adal_scan_open().
 *
 * @note	If the device is opened with the O_SYNC flag, the adal_scan_write()
 *		interface will block the caller until the device has completely
 *		processed the data.  All devices should support the O_SYNC flag,
 *		unless posted writes don't make sense for the device.
 *
 * @note	If the device is opened with the O_NONBLOCK or O_NDELAY, neither
 *		the adal_scan_open() nor any subsequent operations on the file
 *		descriptor will cause the calling process to wait.  The O_NONBLOCK
 *		or O_NDELAY options will only be implemented for devices where
 *		appropriate, all other devices will return EINVAL if O_NONBLOCK or
 *		O_NDELAY are specified.
 *
 * @note	The maximum length for a data operation is determined by the
 *		capabilities of the target device hardware. For devices which
 *		have data size limitations, EFBIG will be returned for requests
 *		which exceed the device capabilities.
 *
 * @param	adal	adal_t of the target device.
 * @param	buf	The source buffer of the data to be written to the device.
 * @param	chainAddress
 * @param	bitlength
 * @param	options		Options to be used for scan operations.
 * @param	status		Status of last scan operation.
 *
 * @return	The number of bytes copied to the device on success, -1 on failure
 *		with 'ERRNO' set as follows.
 * @return	EIO	A low-level device driver or hardware I/O error occurred.
 * @return	EINVAL	At least one of the input parameters is invalid.
 * @return	EFAULT	There was an internal failure or 'buf' is invalid.
 * @return	EBADF	The file descriptor of the underlying device is not valid.
 * @return	EFBIG	If 'count' is larger then the maximum supported device
 *			operation length.
 */
ssize_t adal_scan_write(adal_t * adal, void * buf, int chainAddress,
		size_t bitlength, unsigned long options, unsigned long * status);

/* get the FFDC Data */
/*!
 * @brief	This interface is used to access the device's FFDC information.
 *
 * @pre		The device was opened with a prior call to adal_scan_open().
 *
 * @param	adal	adal_t of the target device.
 * @param	scope	Specifies the FFDC search mode.
 *	@note	ADAL_FFDC_THREAD is used to extract the queued FFDC data only
 *		for the calling thread.
 *	@note	ADAL_FFDC_PROCESS is used to extract the queued FFDC data only
 *		for the calling process.
 * @param	buf	The output destination buffer of the FFDC data read from
 *			the device. This buffer is allocated on the callers heap.
 *			To avoid memory leaks, the caller should be sure to
 *			free(3) 'buf' between each calls to adal_scan_ffdc_extract.
 *
 * @return	The number of bytes copied into 'buf' on success, -1 on failure
 *		with 'ERRNO' set as follows. A value of 0 means that there is no
 *		FFDC information available.
 * @return	EIO	A low-level device driver or hardware I/O error occurred.
 * @return	EINVAL	At least one of the input parameters is invalid.
 * @return	EFAULT	There was an internal failure or 'buf' is invalid.
 * @return	EBADF	The file descriptor of the underlying device is not valid.
 */
ssize_t adal_scan_ffdc_extract(adal_t * adal, int scope, void ** buf);

/*!
 * @brief	This interface is used to access the device's FFDC information
 *		including location data.
 *
 * @pre		The device was opened with a prior call to adal_scan_open().
 *
 * @param	adal	adal_t of the target device.
 * @param	scope	Specifies the FFDC search mode.
 *	@note	ADAL_FFDC_THREAD is used to extract the queued FFDC data only
 *		for the calling thread.
 *	@note	ADAL_FFDC_PROCESS is used to extract the queued FFDC data only
 *		for the calling process.
 * @param	buf	The output destination buffer of the FFDC data read from
 *			the device. This buffer is allocated on the callers heap.
 *			To avoid memory leaks, the caller should be sure to
 *			free(3) 'buf' between each calls to
 *			adal_scan_ffdc_extract.
 * @param	ident	The output specifying the error location identifier
 *			code associated with the returned FFDC.
 *
 * @return	The number of bytes copied into 'buf' on success, -1 on failure
 *		with 'ERRNO' set as follows. A value of 0 means that there is no
 *		FFDC information available.
 * @return	EIO	A low-level device driver or hardware I/O error occurred.
 * @return	EINVAL	At least one of the input parameters is invalid.
 * @return	EFAULT	There was an internal failure or 'buf' is invalid.
 * @return	EBADF	The file descriptor of the underlying device is not valid.
 */
ssize_t adal_scan_ffdc_extract_identifier(adal_t * adal, int scope, void ** buf,
					enum adal_base_err_identifier * ident);

/*!
 * @brief	This interface is used to remove a process/thread from a
 *		device's "blacklist".
 *
 * @pre		The device was opened with a prior call to adal_scan_open().
 *
 * @param	adal	adal_t of the target device.
 * @param	scope	Specifies the FFDC search mode.
 *	@note	ADAL_FFDC_THREAD is used to extract the queued FFDC data only
 *		for the calling thread.
 *	@note	ADAL_FFDC_PROCESS is used to extract the queued FFDC data only
 *		for the calling process.
 *
 * @return	0 on success, -1 on failure with errno set as follows.
 * @return	EIO	A low-level device driver or hardware I/O error occurred.
 * @return	EAGAIN	There is more FFDC data pending.
 * @return	EINVAL	At least one of the input parameters is invalid.
 * @return	EBADF	The file descriptor of the underlying device is not valid.
 * @return	ENODEV	The device is currently unreachable and/or unavailable.
 */

int adal_scan_ffdc_unlock(adal_t * adal, int scope);

/*!
 * @brief	This interface is only a wrapper to adal_base_flush, code will
 *		try execute all pending operations, for more details see
 *		adal_base_flush.
 *
 * @param	adal	adal_t of the target device.
 *
 * @return	0 on success, -1 on failure.
 */
int adal_scan_flush(adal_t * adal) ;

/*!
 * @brief	The get function will read according register in the CFAM.
 *
 * @pre		The device was opened with a prior call to adal_scan_open().
 *
 *
 * @param	adal	adal_t of the target device.
 * @param	data	Data read out of the register.
 *
 * @return	number of Bytes read (4), negative on error.
 */
ssize_t adal_scan_get_setpulselength(adal_t * adal, unsigned long * data);

/*!
 * @brief	The set function will write the according register in the CFAM.
 *
 * @pre		The device was opened with a prior call to adal_scan_open().
 *
 *
 * @param	adal	adal_t of the target device.
 * @param	data	Data written into the register.
 *
 * @return	number of Bytes read (4), negative on error.
 */
ssize_t adal_scan_set_setpulselength(adal_t * adal, unsigned long data);

ssize_t adal_scan_get_true_mask(adal_t * adal, unsigned long * data);
ssize_t adal_scan_set_true_mask(adal_t * adal, unsigned long data);

ssize_t adal_scan_get_comp_mask(adal_t * adal, unsigned long * data);
ssize_t adal_scan_set_comp_mask(adal_t * adal, unsigned long data);

ssize_t adal_scan_get_status(adal_t * adal, unsigned long * data);
ssize_t adal_scan_get_extended_status(adal_t * adal, unsigned long * data);
ssize_t adal_scan_get_chipid(adal_t * adal, unsigned long * data);

/*!
 * @brief	Read a single register in SCAN Engine.
 *		This is for debug only.
 *
 * @pre		The device was opened with a prior call to adal_scan_open().
 *
 *
 * @param	adal		adal_t of the target device.
 * @param	registerNo	Address of the register.
 * @param	data		Data read out of the register.
 *
 * @return	'>=0' on success, negative on error.
 */
ssize_t adal_scan_get_register(adal_t * adal, int registerNo, unsigned long * data);

/*!
 * @brief	Write a single register in SCAN Engine.
 *		This is for debug only.
 *
 * @pre		The device was opened with a prior call to adal_scan_open().
 *
 *
 * @param	adal		adal_t of the target device.
 * @param	registerNo	Address of the register.
 * @param	data		Data written to the register.
 *
 * @return	'>=0' on success, negative on error..
 */
ssize_t adal_scan_set_register(adal_t * adal, int registerNo, unsigned long data);

/* changed with CR092 */

#if 0
/*!
 * @brief	The interface adal_scan_aiocb_init will allocate and setup a
 *		scan aio iocb with the passed in data.
 *
 * @note	adal_aiocb_init will be called with the newly generated control
 *		block.
 *
 * @pre		The adal device needs to be opened before.
 *
 * @param	key		Key to indetify cb - not used in kernel, only
 *				for use in application.
 * @param	adal		Pointer to the underlying device.
 * @param	cmd		The aio command, e.g ADAL_AIO_READ or ADAL_AIO_WRITE
 * @param	buf		Buffer for scan data.
 * @param	bitlength	Length of chain in bits.
 * @param	chainAddress	Address of scan chain.
 * @param	options		Options to be used for scan like not using
 *				header check.
 * @param	ioctl_cb	Will get allocated in the function.
 *
 * @returns	Pointer to control block on success, NULL on failure.
 */
adal_aiocb_t * adal_scan_aiocb_init(uint32_t key, adal_t * adal, adal_aio_cmd_t cmd,
		void * buf, size_t bitlength, unsigned long chainAddress,
		unsigned long options, ioctl_scan_readwrite_t ** ioctl_cb);

/*!
 * @brief	The Adal_scan_init interface can be used to initialise a scan
 *		control block. The control block can then be passed to
 *		adal_aiocb_init.
 *
 * @param	buf		buffer for scan data
 * @param	bitlength	length of chain in bits
 * @param	chainAddress	address of scan chain
 * @param	options		Options to be used for scan like not using
 *				header check.
 * @param	ioctl_cb	pointer to the allready allocated cb that will
 *				get initalised now.
 *
 * returns: 0 on success, -1 on error
 */
int adal_scan_init(void * buf, size_t bitlength, unsigned long chainAddress,
		unsigned long options, ioctl_scan_readwrite_t * ioctl_cb );

/* next two are new to CR092 */

/*!
 * @brief	The interface adal_scan_cb_get_aiostatus is returning the status
 *		from status register from a allready complete aio call.
 *
 * @param	cb	already complete aio cb.
 *
 * @return	Status of scan status register at the time cb got completed.
 */
unsigned long adal_scan_cb_get_aiostatus(adal_aiocb_t * cb);

/*!
 * @brief	The interface adal_scan_cb_get_aiostatus is returning the
 *		pointer to the scan data from a allready complete aio call.
 *
 * @param	cb	Already complete aio cb
 *
 * @return	Pointer to scan data.
 */
void * adal_scan_cb_get_buffer(adal_aiocb_t * cb);
#endif

/*! @brief	Not used currently. */
ssize_t adal_scan_extra_a_clock(adal_t * adal, unsigned long chainAddress);
/*! @brief	Not used currently. */
ssize_t adal_scan_extra_b_clock(adal_t * adal, unsigned long chainAddress);

/*!
 * @brief	This call we get mapped to adal_base_config only ...
 *		see adal_base_config to details.
 */
int adal_scan_config(adal_t * adal, int type, int config, void * value, size_t sz);


/* @brief	This interface will set the 'errormask' used by the device
 *		driver to check for errors.
 *
 * @pre		The device was opened with a prior call to adal_scan_open().
 *
 * @param	adal		adal_t of the target device.
 * @param	errormask	New 'errormask' to be used.
 *
 * @return	len of mask (4) on success, -1 on error with 'ERRNO' set.
 * @return	EINVAL, EIO.
 */
ssize_t adal_scan_set_errormask(adal_t * adal, unsigned long errormask);


/* to be deleted */
/* adal_aio_ctx_t * adal_scan_aio_ctx_create(int num_of_ops); */
/* int adal_scan_aio_ctx_delete(adal_aio_ctx_t * adal_context,uint32_t flag,ioctl_scan_readwrite_t ** if_pointer); */
/* int32_t adal_scan_aiocb_delete(adal_aiocb_t * aiocb, uint32_t flags); */
/* ssize_t adal_scan_aiocb_submit(adal_aio_ctx_t * adal_context,uint32_t no, adal_aiocb_t ** aiocb); */
/* ssize_t	adal_scan_aiocb_cancel(adal_aio_ctx_t * adal_context,uint32_t * nr, adal_aiocb_t ** retArray); */
/* ssize_t	adal_scan_ctx_wait (adal_aio_ctx_t * ctx,uint32_t min_no, uint32_t max_no, struct timespec * t, uint32_t * nr, adal_aiocb_t ** aiocb); */
/* int adal_scan_aio_flush(uint32_t key, adal_t * adal, ioctl_scan_readwrite_t ** if_pointer);  */

void __attribute__ ((constructor)) adal_scan_constructor(void);
void __attribute__ ((destructor)) adal_scan_cleanup(void);

#define SCOM_READ_OP(shift_op, _address, _data, _gid, _flags) \
	shift_op->op.scom_op.address = _address; \
	shift_op->op.scom_op.size = 0x2; \
	shift_op->op.scom_op.type = ADAL_SHIFT_SCOM_READ; \
	shift_op->flags = _flags; \
	shift_op->data = _data; \
	shift_op->type = ADAL_SHIFT_SCOM; \
	shift_op->gid = _gid; \

#define SCOM_WRITE_OP(shift_op, _address, _data, _gid, _flags) \
	shift_op->op.scom_op.address = _address; \
	shift_op->op.scom_op.size = 0x2; \
	shift_op->op.scom_op.type = ADAL_SHIFT_SCOM_WRITE; \
	shift_op->flags = _flags; \
	shift_op->data = _data; \
	shift_op->type = ADAL_SHIFT_SCOM; \
	shift_op->gid = _gid; \

#define SCOM_MOD_OR_OP(shift_op, _address, _data, _gid, _flags) \
	shift_op->op.scom_op.address = _address; \
	shift_op->op.scom_op.size = 0x2; \
	shift_op->op.scom_op.type = ADAL_SHIFT_SCOM_MOD_OR; \
	shift_op->flags = _flags; \
	shift_op->data = _data; \
	shift_op->type = ADAL_SHIFT_SCOM; \
	shift_op->gid = _gid; \

#define SCOM_MOD_AND_OP(shift_op, _address, _data, _gid, _flags) \
	shift_op->op.scom_op.address = _address; \
	shift_op->op.scom_op.size = 0x2; \
	shift_op->op.scom_op.type = ADAL_SHIFT_SCOM_MOD_AND; \
	shift_op->flags = _flags; \
	shift_op->data = _data; \
	shift_op->type = ADAL_SHIFT_SCOM; \
	shift_op->gid = _gid; \

#define SCOM_BULKREAD_OP(shift_op, _address, _data, _size, _gid, _flags) \
	shift_op->op.scom_op.address = _address; \
	shift_op->op.scom_op.size = _size; \
	shift_op->op.scom_op.type = ADAL_SHIFT_SCOM_BULKREAD; \
	shift_op->flags = _flags; \
	shift_op->data = _data; \
	shift_op->type = ADAL_SHIFT_SCOM; \
	shift_op->gid = _gid; \

#define SCOM_BULKWRITE_OP(shift_op, _address, _data, _size, _gid, _flags) \
	shift_op->op.scom_op.address = _address; \
	shift_op->op.scom_op.size = _size; \
	shift_op->op.scom_op.type = ADAL_SHIFT_SCOM_BULKWRITE; \
	shift_op->flags = _flags; \
	shift_op->data = _data; \
	shift_op->type = ADAL_SHIFT_SCOM; \
	shift_op->gid = _gid; \

/*
* Add the shift operation to the end of stream.
*
* Param : 'stream', pointer to the stream.
* Param : 'shift_op', pointer of the scan operation.
* Return : >= 0 on success,
*          < 0 on failure with errno
* Retval : EINVAL At least one of the input parameters is invalid
* Retval : ENOMEM Not enough memory for next stream element.
*/
int32_t adal_shift_add_op(shift_stream_t **stream, shift_op_t *shift_op);

/*!
 * @brief	Delete the first available shift operation from the stream,
 *		begins from start.
 *
 * @param	stream		Pointer to the stream.
 * @param	shift_op	Pointer of the scan operation
 *
 * @return	>= 0 on success, < 0 on failure with 'ERRNO'
 * @return	EINVAL	At least one of the input parameters is invalid or
 *			scan_op not found in the stream.
 */
int32_t adal_shift_del_op(shift_stream_t **stream, shift_op_t *shift_op);

/*!
 * @brief	Processing shift operations and wait when they finish executing.
 *
 * @param	adal	adal_t of the target device.
 * @param	stream	Pointer to the stream
 *
 * @return	>= 0 on success, < 0 on failure with 'ERRNO' set.
 * @return	EIO A low-level device driver or hardware I/O error occurred.
 */
int32_t adal_shift_submit(adal_t *adal, shift_stream_t *stream);

/*!
 * @brief	Get size of the stream.
 *
 * @param	stream	Pointer to the stream.
 *
 * @return	Size of stream.
 */
uint32_t adal_shift_get_count(shift_stream_t *stream);

/*!
 * @brief	Get part of elements from the stream on 'gid' of the scan
 *		operation. Make a copy from current 'stream'. Must be freed
 *		with adal_scan_free.
 *
 * @param	stream	pointer to the stream.
 * @param	gid	Unique identifier of the group operations.
 *
 * @return	NULL/0 - not found in the stream or error occurred,
 * @return	>0 - pointer to part of the stream with start of scan operation
 *		with 'gid'
 * @return	ENOMEM, Not enough memory for stream element.
 */
shift_stream_t * adal_shift_get_elements(shift_stream_t *stream, uint32_t gid);

/*!
 * @brief	Append copy of 'from_stream' to the end of 'to_stream'.
 *
 * @param	from_stream	pointer of the stream from which we copy.
 * @param	to_stream	pointer of the stream to which we copy.
 * Return : >= 0 on success,
 *          <0 on failer with errno
 * Retval : EINVAL 'from_stream' or 'to_stream' is NULL or incorrect references.
 * Retval : ENOMEM Not enough memory for stream element.
 */
int32_t adal_shift_add_stream(shift_stream_t *from_stream, shift_stream_t *to_stream);

/*
* Free the stream, delete all stream elements.
*
* Param : 'stream', pointer to the stream
*/
void adal_shift_free(shift_stream_t *stream);

/*
 * @brief	Update the scom address in the shift operation.
 *
 * @param	op	pointer to the shift operation.
 * @param	address	address.
 *
 * @return	>= 0 on success, < 0 on failure with 'ERRNO' set.
 * @return	EINVAL	incorrect shift operation.
 */
int32_t adal_shift_set_address(shift_op_t *op, uint64_t address);

/*!
 * @brief	Update the scan chain address in the shift operation.
 *
 * @param	op		pointer to the operation
 * @param	chain_address	chain address
 *
 * @return	>= 0 on success, <0 on failure with 'ERRNO' set.
 * @return	EINVAL	incorrect shift operation.
 */
int32_t adal_shift_set_chain_address(shift_op_t *op, uint32_t chain_address);

/*!
 * @brief	Get address from shift/scom operation.
 *
 * @param	op		pointer to the operation.
 * @param	address		pointer to the address.
 *
 * @return	>= 0 on success, <0 on failure with 'ERRNO' set.
 * @return	EINVAL	incorrect shift operation.
 */
int32_t adal_shift_get_address(shift_op_t *op, uint64_t *address);

/*!
 * @brief	Get size from scom operation.
 *
 * @param	op		pointer to the operation.
 * @param	size		pointer to the size.
 *
 * @return	>= 0 on success, <0 on failure with 'ERRNO' set.
 * @return	EINVAL	incorrect shift operation.
 */
int32_t adal_shift_get_size(shift_op_t *op, uint32_t *size);

/*!
 * @brief	Get chain address from the shift/scan operation.
 *
 * @param	op		pointer to the shift operation.
 * @param	chain_address	pointer to chain_addres in which we write.
 *
 * @return	>= 0 on success, <0 on failer with 'ERRNO'.
 * @return	EINVAL	incorrect shift operation.
 */
int32_t adal_shift_get_chain_address(shift_op_t *op, uint32_t *chain_address);

/*!
 * @brief	Get the data from the shift operation.
 *
 * @param	op	pointer to the shift operation
 *
 * @return	pointer of the data operation.
 */
inline uint32_t *adal_shift_get_data(shift_op_t *op);

/*!
 * @brief	Set the data for the shift operation.
 *
 * @param	op	pointer to the operation.
 * @param	data	data for setting.
 *
 * @return	>= 0 on success, <0 on failure with 'ERRNO' set.
 * @return	EINVAL	input data == NULL.
 *
 */
int32_t adal_shift_set_data(shift_op_t *op, uint32_t *data);

/*!
 * @brief	Get remaining size which we need to write/read for
 *		the ending of bulk operation.
 *
 * @param	adal	adal_t of the target device.
 * @param	data	remaining size.
 *
 * @return	>=0 on success, <0 on failure with 'ERRNO' set.
 *
 */
ssize_t adal_shift_get_remaining_size(adal_t *adal, unsigned long *data);

/*!
 * @brief	Get number of operation in bulk stream on which we failed.
 *
 * @param	adal	adal_t of the target device.
 * @param	data	error pointer on operation which we failed.
 *
 * @return	>=0 on success, <0 on failure with 'ERRNO' set.
 *
 */
ssize_t adal_shift_get_error_pointer(adal_t *adal, unsigned long *data);

/*!
 * @brief	Deprecated call, same as adal_shift_get_error_pointer, with
 *		same behavior.
 */
ssize_t __attribute__ ((deprecated)) adal_scan_get_error_pointer(adal_t *adal,
			unsigned long *data);

/*!
 * @brief	Deprecated call, same as adal_shift_get_remaining_size, with
 *		same behavior.
 */
ssize_t __attribute__ ((deprecated)) adal_scan_get_remaining_size(adal_t *adal,
			unsigned long *data);
/*!
 * @brief	Set dma compare mask for evaluating of failing operations in
 *		stream.
 *
 * @param	adal	adal_t of the target device.
 * @param	data	compare mask.
 *
 * @return	>=0 on success, <0 on failure with 'ERRNO' set.
 *
 */
ssize_t adal_shift_set_compare_mask(adal_t *adal, unsigned long data);

/*!
 * @brief	Get order number of first failed operation in stream.
 *
 * @param	adal	adal_t of the target device.
 * @param	stream	pointer to the executed stream.
 *
 * @return	>=0 on success, <0 with EINVAL.
 *
 */
ssize_t adal_shift_get_errcmd_order_number(adal_t *adal,
						shift_stream_t *stream);
/*!
 * @brief	Get pointer shift operation of first failed operation
 *		in stream.
 *
 * @param	adal	adal_t of the target device.
 * @param	stream	pointer to the executed stream.
 *
 * @return	pointer on success, 0 with EINVAL.
 *
 */
shift_stream_t *adal_shift_get_errcmd_stream(adal_t *adal,
						 shift_stream_t *stream);

#ifdef __cplusplus
}
#endif

#endif /* __ADAL_SCAN_H__ */
