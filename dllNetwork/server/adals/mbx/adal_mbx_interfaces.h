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

#ifndef __ADAL_MBX_INTERFACES_H__
#define __ADAL_MBX_INTERFACES_H__

#include <adal_base.h>

#include <stdint.h>               /* uintX_t */

#include <sys/uio.h>              /* struct iovec */


/*
 * Interface Overview:
 *
 * Interface(s):
 * -------------
 *        adal_mbx_open - Request access to a mailbox device.
 *       adal_mbx_close - Release access of a mailbox device.
 *       adal_mbx_reset - Stop activity and force a mailbox device into a "known"
 *                        state.
 *
 *adal_mbx_ffdc_extract - Retreive any pending FFDC information a
 *                         a device may have.
 * adal_mbx_ffdc_unlock - Remove from black list
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/*!@brief
 * The adal_mbx_open() interface is used to open a mailbox device and associate it
 * with an adal_t.  It must be the first interface called to ensure that the
 * device has been opened. The device is closed when the application
 * terminates or when adal_mbx_close() is called on the device.
 *
 * @post: When the device is opened it is configured with the default set of
 *       parameter values.  The adal_mbx_config() can be used to modify the
 *       device's operating characteristics.
 *
 * @post: When the device is opened, sizeof(adal_t) bytes is
 *       allocated on the caller's heap.
 *
 * @param: 'device' NULL-terminated string of the target device
 *        (i.e. /dev/mbx/<n>).
 *
 * @param: 'flags' Optional device specific open flags, use 0 for the
 *        device default.  One of the required flags can be bitwise or'd
 *        with zero or more optional flags.
 *
 * @note: The low-level device drivers will support only the flags
 *        which are appropiate for the underlying device.
 *
 * @note   [Required]
 * @note    O_RDONLY    Open the device for read-only access.
 * @note    O_WRONLY    Open the device for write-only access.
 * @note    O_RDWR      Open the device for read and write access.
 *
 * @note   [Optional]
 * @note    O_EXCL      Request exclusive access to the device
 *                      (i.e. blocking single-threaded access).
 * @note    O_SYNC      Any write()'s on the device will block the caller until
 *                      the data has been physically written to the underlying
 *                      device.
 * @note    O_ASYNC     Generate a signal (SIGIO by default) when input or
 *                      output becomes possible on this device.
 * @note    O_NONBLOCK,
 * @note    O_NDELAY    The device is opened in non-blocking mode.  Neither
 *                      the open nor any subsequent operations on the device
 *                      will cause the caller to wait.
 *
 * @note    See 'man 2 open' for a details about all open flags.
 *
 * @return: Pointer to an adal_t on success, NULL on failure with errno set as
 *         follows.
 * @return: ENXIO The device in the /dev filesystem can not be found.
 * @return: EIO A low-level device driver or hardware I/O error occurred.
 * @return: EINVAL At least one of the input parameters is invalid.
 * @return: EBUSY The request would block waiting for the device,
 *         if O_NONBLOCK was specified in the 'flags' parameter.
 */
adal_t * adal_mbx_open(const char * device, int flags);

/*!@brief
 * The adal_mbx_close() interface is used to disassociate an adal_t from
 * its underlying device.  If this interface is not called before the process
 * exits, the device will be closed by the operating system.

 * @pre: The device was opened with a prior call to adal_mbx_open().
 *
 * @note: Prior to closing the device, the adal_mbx_close() interface will
 *       ensure any buffered data is written to the device by calling the
 *       adal_mbx_flush() interface.
 *
 * @note: It's important to check the return code from the adal_mbx_close()
 *       interface because device operation errors are guaranteed to be
 *       returned as the return code from this interface.  Not checking
 *       the return code can lead to silent data lossage.
 *
 * @param: 'adal' adal_t of the target device.
 *
 * @return: 0 on success, -1 on failure with errno set as follows.
 * @return: EIO A low-level device driver or hardware I/O error occurred.
 * @return: EINVAL At least one of the input parameters is invalid.
 * @return: EBADF The file descriptor of the underlying device is not valid.
 */
int adal_mbx_close(adal_t * adal);

/*!@brief
 * The adal_mbx_reset() interface is used to force the underlying device,
 * associated with an adal_t, into a known state.
 *
 * @pre: The device was opened with a prior call to adal_mbx_open().
 *
 * @note: The adal_mbx_reset() interface can be used to ensure the device is
 *       in a "sane" state suitable for operation.
 *
 * @param: 'adal' adal_t of the target device.
 * @param: 'type' Reset type.
 *
 * @return: 0 on success, -1 on failure with errno set as follows.
 * @return: EIO A low-level device driver or hardware I/O error occurred.
 * @return: EINVAL At least one of the input parameters is invalid.
 * @return: EBADF The file descriptor of the underlying device is not valid.
 */
int adal_mbx_reset(adal_t * adal, int type);



/*!@brief
 * The adal_mbx_ffdc_extract() interface is used to access the device's FFDC
 * information.
 *
 * @pre: The device was opened with a prior call to adal_mbx_open().
 *
 * @param: 'adal' adal_t of the target device.
 * @param: 'scope' Specifies the FFDC search mode.
 *         ADAL_FFDC_THREAD is used to extract the queued FFDC
 *         data only for the calling thread.
 *         ADAL_FFDC_PROCESS is used to extract the queued FFDC
 *         data only for the calling process.
 * @param: 'buf' The output destination buffer of the FFDC data read from
 *         the device.  This buffer is allocated on the callers heap.  To
 *         avoid memory leaks, the caller should be sure to free(3) 'buf'
 *         between each calls to adal_mbx_ffdc_extract.
 *
 * @return: The number of bytes copied into 'buf' on success, -1 on failure
 *         with errno set as follows. A value of 0 means that there is no
 *         FFDC information available.
 * @return: EIO A low-level device driver or hardware I/O error occurred.
 * @return: EINVAL At least one of the input parameters is invalid.
 * @return: EFAULT There was an internal failure or 'buf' is invalid.
 * @return: EBADF The file descriptor of the underlying device is not valid.
 */
ssize_t adal_mbx_ffdc_extract(adal_t * adal, int scope, void ** buf);


/*!@brief
 * The adal_mbx_ffdc_unlock() interface is used to remove a process/thread
 * from a device's "blacklist".
 *
 * @pre: The device was opened with a prior call to adal_mbx_open().
 *
 * @param: 'adal' adal_t of the target device.
 * @param: 'scope' Specifies the FFDC search mode.
 *         ADAL_FFDC_THREAD is used to extract the queued FFDC
 *         data only for the calling thread.
 *         ADAL_FFDC_PROCESS is used to extract the queued FFDC
 *         data only for the calling process.
 *
 * @return: 0 on success, -1 on failure with errno set as follows.
 * @return: EIO A low-level device driver or hardware I/O error occurred.
 * @return: EAGAIN There is more FFDC data pending.
 * @return: EINVAL At least one of the input parameters is invalid.
 * @return: EBADF The file descriptor of the underlying device is not valid.
 * @return: ENODEV The device is currently unreachable and/or unavailable.
 */
int adal_mbx_ffdc_unlock(adal_t * adal, int scope);


/*!@brief
 * The adal_mbx_scratch interface is used to read and write the mailbox
 * scratch pad registers.
 *
 * @param: 'adal'    adal_t of the target device.
 * @param: scratch   enumerated type which specified which register to read or
 *                  write.
 * @param: mode      enumerated type which specifies if you want to read or
 *                  write the register.
 * @param: value     The storage for the value that is to be written from
 *                  or read to.
 *
 * @return: 0 on success. -1 on failure with errno set as follows.
 * @return: EIO A low-level device driver or hardware I/O error occurred.
 * @return: EAGAIN There is more FFDC data pending.
 * @return: EINVAL At least one of the input parameters is invalid.
 * @return: EBADF The file descriptor of the underlying device is not valid.
 * @return: ENODEV The device is currently unreachable and/or unavailable.
 */
int adal_mbx_scratch(adal_t * adal,
					 adal_mbx_scratch_t  scratch,
					 adal_mbx_gpreg_mode_t mode,
					 uint32_t * value);

/*!@brief
 * The adal_mbx_get_register interface is used to read the mailbox
 * registers.
 *
 * @param: 'adal'    adal_t of the target device.
 * @param: reg       Address of the register.
 * @param: value     The storage for the value that is to be read to.
 *
 * @return: 0 on success. -1 on failure with errno set as follows.
 * @return: EIO A low-level device driver or hardware I/O error occurred.
 * @return: EAGAIN There is more FFDC data pending.
 * @return: EINVAL At least one of the input parameters is invalid.
 * @return: EBADF The file descriptor of the underlying device is not valid.
 * @return: ENODEV The device is currently unreachable and/or unavailable.
 */
int adal_mbx_get_register(adal_t * adal, unsigned long reg,
		       uint32_t * value);

/*!@brief
 * The adal_mbx_set_register interface is used to write the mailbox
 * registers.
 *
 * @param: 'adal'    adal_t of the target device.
 * @param: reg       Address of the register.
 * @param: value     The storage for the value that is to be write from.
 *
 * @return: 0 on success. -1 on failure with errno set as follows.
 * @return: EIO A low-level device driver or hardware I/O error occurred.
 * @return: EAGAIN There is more FFDC data pending.
 * @return: EINVAL At least one of the input parameters is invalid.
 * @return: EBADF The file descriptor of the underlying device is not valid.
 * @return: ENODEV The device is currently unreachable and/or unavailable.
 */
int adal_mbx_set_register(adal_t * adal, unsigned long reg,
		       uint32_t value);

#ifdef __cplusplus
}
#endif

#endif /* __ADAL_MBX_INTERFACES_H__ */
