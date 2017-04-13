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
 *   Desc: IIC Access Device Abstraction Layer (a.k.a ADAL).
 */

#ifndef __ADAL_IIC_INTERFACES_H__
#define __ADAL_IIC_INTERFACES_H__

#include <adal_base_interfaces.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!@brief
 * The adal_iic_open() interface is used to open a device and associate it
 * with an adal_t.  It must be the first interface called to ensure that the
 * device has been opened. The device is closed when the application
 * terminates or when adal_iic_close() is called on the device.
 *
 * @post: When the device is opened it is configured with the default set of
 *       parameter values.  The adal_iic_config() can be used to modify the
 *       device's operating characteristics.
 *
 * @post: When the device is opened, sizeof(adal_t) bytes is
 *       allocated on the caller's heap.
 *
 * @param: 'device' NULL-terminated string of the target device
 *        (i.e. /dev/iic/<link>/<cfam>/<engine/<port>).
 *
 * @param: 'flags' Optional device specific open flags, use 0 for the 
 *        device default.  One of the required flags can be bitwise or'd
 *        with zero or more optional flags.
 *
 * @note:  The low-level device drivers will support only the flags
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
adal_t * adal_iic_open(const char * device, int flags);

/*!@brief
 * The adal_iic_close() interface is used to disassociate an adal_t from
 * its underlying device.  If this interface is not called before the process
 * exits, the device will be closed by the operating system.

 * @pre: The device was opened with a prior call to adal_iic_open().
 *
 * @note: Prior to closing the device, the adal_iic_close() interface will
 *       ensure any buffered data is written to the device by calling the
 *       adal_iic_flush() interface.
 *
 * @note: It's important to check the return code from the adal_iic_close()
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
int adal_iic_close(adal_t * adal);

/*!@brief
 * The adal_iic_reset() interface is used to force the underlying device,
 * associated with an adal_t, into a known state.
 * 
 * @pre: The device was opened with a prior call to adal_iic_open().
 * 
 * @note: The adal_iic_reset() interface can be used to ensure the device is
 *       in a "sane" state suitable for operation.
 * 
 * @param: 'adal' adal_t of the target device.
 * @param: 'type' Reset type. FULL type performs the same actions as LIGHT
 * reset, but also resets transfer operations to default values. All the
 * return codes can be set for either light or full reset.
 * 
 * @return: 0 on success.
 * @return: 1 success, stuck bus recovered
 * @return: -1 on failure to release stuck data line.
 * @return: < -1 on other failure with errno set as follows:
 * @return: EIO A low-level device driver or hardware I/O error occurred.
 * @return: EINVAL At least one of the input parameters is invalid.
 * @return: EBADF The file descriptor of the underlying device is not valid.
 * @return: EINTR The reset was interrupted.
 * @return: ENOTTY The ioctl command was invalid.
 * @return: ENODEV Lost local bus ownership.
 * @return: ENOLINK FSI link was severed.
 * @return: EDEADLK FFDC must be collected.
 * @return: ETIME Waiting for idle has timed out.
 * @return: ENOMEM There is insufficient memory for FFDC or there is I/O
 *	   failure.
 * @return: Other return codes may indicate engine state.
 */
int adal_iic_reset(adal_t * adal, int type);

/*!@brief
 * The adal_iic_config() interface is used to access the current configuration
 * parameters of the underlying device associated with an adal_t.
 * 
 * @pre: The device was opened with a prior call to adal_iic_open().
 * 
 * @param: 'adal' adal_t of the target device.
 * @param: 'type' The access type (i.e. read or write the config value).
 * @param: 'param' The target parameter.
 * @param: 'value' A multipurpose input/output variable defined by each
 *         specific config operation.
 * 
 * @return: >= 0 on success, -1 on failure with errno set as follows.
 * @return: EIO A low-level device driver or hardware I/O error occurred.
 * @return: EINVAL At least one of the input parameters is invalid.
 * @return: EFAULT There was an internal failure or 'data' is invalid
 * @return: EBADF The file descriptor of the underlying device is not valid.
 */
int adal_iic_config(adal_t * adal, int type, adal_iic_cfg_t cfg, void * value, size_t size);

/*!@brief
 * The adal_iic_read() interface is used to "read" data from the underlying
 * device associated with an adal_t.
 *
 * @pre: The device was opened with a prior call to adal_iic_open().
 *
 * @note: In general, the adal_iic_read() interface implements a blocking
 *       "read" interface.  However, if the device is opened with the
 *       O_NONBLOCK or O_NDELAY, neither the adal_iic_open() nor any
 *       subsequent operations on the file descriptor will cause the
 *       calling process to wait.  The O_NONBLOCK or O_NDELAY options
 *       will only be implemented for devices where appropiate, all
 *       other devices will return EINVAL if O_NONBLOCK or O_NDELAY are
 *       specified.
 *
 * @note: The maximun length for a data operation is determined by the
 *       capabilities of the target device hardware. For devices which
 *       have data size limitations, EFBIG will be returned for requests
 *       which exceed the device capabilities.
 * 
 * @param: 'adal' adal_t of the target device.
 * @param: 'buf' The destination buffer of the data read from the device.
 * @param: 'count' The maximum size of the buffer.
 *
 *         @note: The meaning of 'count' is defined by the underlying device.
 *               For most devices, 'count' will represent bytes but it could
 *               represent bits or 32-bit words or whatever is appropiate.
 *
 * @return: The number of bytes copied into 'buf' on success, -1 on failure
 *         with errno set as follows.
 * @return: EIO A low-level device driver or hardware I/O error occurred.
 * @return: EINVAL At least one of the input parameters is invalid.
 * @return: EFAULT There was an internal failure or 'buf' is invalid.
 * @return: EBADF The file descriptor of the underlying device is not valid.
 * @return: EFBIG If 'count' is larger then the maximum supported device
 *         operation length.
 * @return: EAGAIN If another thread is using this device.
 * @return: EDEADLK FFDC must be collected.
 * @return: ENOLCK Failure to create new thread lock.
 * @return: ENOENT Failure to create new thread lock.
 * @return: EINTR If process received an interrupt signal, no data transferred.
 * @return: ERESTARTSYS If wait event receives an interrupt.
 * @return: EACCESS If file access permission denied.
 * @return: ENOMEM Not enough memory.
 * @return: ENODEV Lost local bus ownership.
 * @return: ENOLINK FSI link was severed.
 */
ssize_t adal_iic_read(adal_t * adal, void * buf, size_t count);

/*!@brief
 * The adal_iic_write() interface is used to write data to the underlying
 * device associated with an adal_t.
 *
 * @pre: The device was opened with a prior call to adal_iic_open().
 *
 * @note: If the device is opened with the O_SYNC flag, the adal_iic_write()
 *       interface will block the caller until the device has completly
 *       processed the data.  All devices should support the O_SYNC flag,
 *       unless posted writes don't make sense for the device.
 *
 * @note: If the device is opened with the O_NONBLOCK or O_NDELAY, neither
 *       the adal_iic_open() nor any subsequent operations on the file
 *       descriptor will cause the calling process to wait.  The O_NONBLOCK
 *       or O_NDELAY options will only be implemented for devices where
 *       appropiate, all other devices will return EINVAL if O_NONBLOCK or
 *       O_NDELAY are specified.
 *
 * @note: The maximum length for a data operation is determined by the
 *       capabilities of the target device hardware. For devices which
 *       have data size limitations, EFBIG will be returned for requests
 *       which exceed the device capabilities.
 * 
 * @param: 'adal' adal_t of the target device.
 * @param: 'buf' The source buffer of the data to be written to the device.
 * @param: 'count' The actual size of the write data.
 *
 *         @note: The meaning of 'count' is defined by the underlying device.
 *               For most devices, 'count' will represent bytes but it could
 *               represent bits or 32-bit words or whatever is appropiate.
 *
 * @return: The number of bytes copied to the device on success, -1 on failure
 *         with errno set as follows.         
 * @return: EIO A low-level device driver or hardware I/O error occurred.
 * @return: EINVAL At least one of the input parameters is invalid.
 * @return: EFAULT There was an internal failure or 'buf' is invalid.
 * @return: EBADF The file descriptor of the underlying device is not valid.
 * @return: EFBIG If 'count' is larger then the maximum supported device
 *         operation length.
 * @return: EAGAIN If another thread is using this device.
 * @return: EDEADLK FFDC must be collected.
 * @return: ENOLCK Failure to create new thread lock.
 * @return: ENOENT Failure to create new thread lock.
 * @return: EINTR If process received an interrupt signal, no data transferred.
 * @return: ERESTARTSYS If wait event receives an interrupt.
 * @return: EACCESS If file access permission denied.
 * @return: ENOMEM Not enough memory.
 * @return: ENODEV Lost local bus ownership.
 * @return: ENOLINK FSI link was severed.
 */
ssize_t adal_iic_write(adal_t * adal, const void * buf, size_t count);

/*!@brief
 * The adal_iic_repeated_start() interface is used to issue the repeated start 
 * I/O to the device associated with an adal_t.
 *
 * @pre: The device was opened with a prior call to adal_iic_open().
 *
 * @note: If the device is opened with the O_SYNC flag, the adal_iic_write()
 *       interface will block the caller until the device has completly
 *       processed the data.  All devices should support the O_SYNC flag,
 *       unless posted writes don't make sense for the device.
 *
 * @note: If the device is opened with the O_NONBLOCK or O_NDELAY, neither
 *       the adal_iic_open() nor any subsequent operations on the file
 *       descriptor will cause the calling process to wait.  The O_NONBLOCK
 *       or O_NDELAY options will only be implemented for devices where
 *       appropiate, all other devices will return EINVAL if O_NONBLOCK or
 *       O_NDELAY are specified.
 *
 * @note: The maximum length for a data operation is determined by the
 *       capabilities of the target device hardware. For devices which
 *       have data size limitations, EFBIG will be returned for requests
 *       which exceed the device capabilities.
 * 
 * @param: 'adal' adal_t of the target device.
 * @param: 'msgset' The pointer of the structure that contains multiple 
 * I2C requests.
 * @return: < 0 on failure with errno set as follows.         
 * @return: EIO A low-level device driver or hardware I/O error occurred.
 * @return: EINVAL At least one of the input parameters is invalid.
 * @return: EFAULT There was an internal failure or 'buf' is invalid.
 * @return: EBADF The file descriptor of the underlying device is not valid.
 * @return: EFBIG If 'count' is larger then the maximum supported device
 *         operation length.
 * @return: EINTR The command was interrupted.
 * @return: ENOTTY The ioctl command was invalid.
 * @return: ENODEV Lost local bus ownership.
 * @return: ENOLINK FSI link was severed.
 * @return: EDEADLK FFDC must be collected.
 */
int adal_iic_repeated_start(adal_t * adal, struct i2c_rdwr_ioctl_data *msgset);

/*!@brief
 * The adal_iic_seek() interface is used to reposition the read/write
 * offset of the underlying device associated with an adal_t.
 *
 * @note: Typically, only devices that support random access implement the
 *       lseek() system call which can be used to reposition the file
 *       position.  Some devices may require one of more ioctl() calls
 *       to change the read/write position.
 * 
 * @param: 'adal' adal_t of the target device.
 * @param: 'offset' New read/write file offset.
 * @param: 'whence' Offset reference point:
 * @note   [Required]
 * @note    SEEK_SET	The read/write offset is set relative to the
 *                      beginning of the device.
 * @note    SEEK_CUR	The read/write offset is set relative to the
 *                      current offset.
 * @note    SEEK_END	The read/write offset is set to relative to the
 *                      end of the device. 
 *
 * @return: The new read/write offset relative to the beginning of the device,
 *         -1 on failure with errno set as follows.
 * @return: EIO A low-level device driver or hardware I/O error occurred.
 * @return: EINVAL At least one of the input parameters is invalid.
 * @return: EBADF The file descriptor of the underlying device is not valid.
 */
int adal_iic_seek(adal_t * adal, off_t offset, int whence);

/*!@brief
 * The adal_iic_flush() interface is used to block the caller until all
 * pending operations are processed by the underlying device.  For some
 * devices, that means waiting until posted/queued writes are written to
 * the device medium.
 *
 * @pre: The device was opened with a prior call to adal_iic_open().
 *
 * @param: 'adal' adal_t of the target device.
 *
 * @return: 0 on success, -1 on failure with errno set as follows.
 * @return: EIO A low-level device driver or hardware I/O error occurred.
 * @return: EINVAL At least one of the input parameters is invalid.
 * @return: EBADF The file descriptor of the underlying device is not valid.
 */
int adal_iic_flush(adal_t * adal);

/*!@brief
 * The adal_iic_ffdc_extract() interface is used to access the device's FFDC
 * information.
 * 
 * @pre: The device was opened with a prior call to adal_iic_open().
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
 *         between each calls to adal_iic_ffdc_extract.
 * 
 * @return: The number of bytes copied into 'buf' on success, -1 on failure
 *         with errno set as follows. A value of 0 means that there is no
 *         FFDC information available.
 * @return: EIO A low-level device driver or hardware I/O error occurred.
 * @return: EINVAL At least one of the input parameters is invalid.
 * @return: EFAULT There was an internal failure or 'buf' is invalid.
 * @return: EBADF The file descriptor of the underlying device is not valid.
 */
ssize_t adal_iic_ffdc_extract(adal_t * adal, int scope, void ** buf);

/*!@brief
 * The adal_iic_ffdc_extract_identifier() interface is used to access the
 * device's FFDC information with location data.
 *
 * @pre: The device was opened with a prior call to adal_iic_open().
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
 *         between each calls to adal_iic_ffdc_extract.
 * @param: 'ident' The output value specifying the error location identifier
 *         code associated with the returned FFDC.
 *
 * @return: The number of bytes copied into 'buf' on success, -1 on failure
 *         with errno set as follows. A value of 0 means that there is no
 *         FFDC information available.
 * @return: EIO A low-level device driver or hardware I/O error occurred.
 * @return: EINVAL At least one of the input parameters is invalid.
 * @return: EFAULT There was an internal failure or 'buf' is invalid.
 * @return: EBADF The file descriptor of the underlying device is not valid.
 */
ssize_t adal_iic_ffdc_extract_identifier(adal_t * adal, int scope, void ** buf,
					enum adal_base_err_identifier * ident);

/*!@brief
 * The adal_iic_ffdc_unlock() interface is used to remove a process/thread
 * from a device's "blacklist".
 *
 * @pre: The device was opened with a prior call to adal_iic_open().
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
int adal_iic_ffdc_unlock(adal_t * adal, int scope);

/*!@brief
 * The adal_iic_init() interface is used to initialize the ADAL shared
 * library runtime.  This function gets called after a shared library is
 * is loaded and mapped into a process.
 *
 * @note: Obsolete symbols _init and _fini (See 'man 3 dlopen')
 *
 *   The linker recognizes special symbols _init and _fini.   If  a  dynamic
 *   library exports a routine named _init, then that code is executed after
 *   the loading, before dlopen() returns. If the dynamic library exports  a
 *   routine  named  _fini,  then  that  routine  is  called just before the
 *   library is unloaded.  In case you  need to  avoid  linking against  the
 *   system  startup  files,  this  can be done by giving gcc the "-nostart-
 *   files" parameter on the command line.
 *
 *   Using these routines, or the gcc -nostartupfiles or -nostdlib  options,
 *   is  _NOT_  recommended. Their use may result in undesired behavior, since
 *   the constructor/destructor routines will not be executed  (unless  spe-
 *   cial measures are taken).
 *
 *   Instead, libraries should export routines using the __attribute__((con-
 *   structor)) and __attribute__((destructor))  function  attributes.   See
 *   the  gcc info pages for information on these.  Constructor routines are
 *   executed before dlopen returns, and destructor  routines  are  executed
 *   before dlclose returns.
 *
 * @note: Since this function is called during shared library initialization,
 *       it runs on behalf of the calling process.  Any resources allocated
 *       are scoped to the address space of the calling process (except
 *       shared objects such as SysV IPC objects).  These resource should
 *       be cleaned up to prevent resource leaks when the ADAL is dynamically
 *       loaded.
 *
 * @note: The ADAL should support being dynamically loaded using dlopen
 *       from libdl.so.  Additionally, adal_iic_init gets called
 *       each time dlopen is called or when loaded by ld.so.
 *       
 */
void adal_iic_init(void) __attribute__ ((constructor));

/*!@brief
 * The adal_iic_cleanup() interface is used to "clean up" the ADAL shared
 * library runtime.  This function gets called before a shared library is
 * is unloaded and unmapped from a process.
 *
 * @note: Obsolete symbols _init and _fini (See 'man 3 dlopen')
 *
 *   The linker recognizes special symbols _init and _fini.   If  a  dynamic
 *   library exports a routine named _init, then that code is executed after
 *   the loading, before dlopen() returns. If the dynamic library exports  a
 *   routine  named  _fini,  then  that  routine  is  called just before the
 *   library is unloaded.  In case you  need to  avoid  linking against  the
 *   system  startup  files,  this  can be done by giving gcc the "-nostart-
 *   files" parameter on the command line.
 *
 *   Using these routines, or the gcc -nostartupfiles or -nostdlib  options,
 *   is  _NOT_  recommended. Their use may result in undesired behavior, since
 *   the constructor/destructor routines will not be executed  (unless  spe-
 *   cial measures are taken).
 *
 *   Instead, libraries should export routines using the __attribute__((con-
 *   structor)) and __attribute__((destructor))  function  attributes.   See
 *   the  gcc info pages for information on these.  Constructor routines are
 *   executed before dlopen returns, and destructor  routines  are  executed
 *   before dlclose returns.
 *
 * @note: Since this function is called during shared library termination,
 *       it runs on behalf of the calling process.  Any resource allocated
 *       in adal_iic_init should be unallocated to prevent resource leaks
 *       when the ADAL is dynamically loaded.
 *
 * @note: The ADAL should support being dynamically loaded using dlclose
 *       from libdl.so. Additionally, adal_iic_cleanup gets called
 *       each time dlclose is called or when unloaded by ld.so.
 *       
 */
void adal_iic_cleanup(void) __attribute__ ((destructor));


/**************************************************************************
 *
 *  Secure IIC (SIIC) interfaces
 *
 *************************************************************************/

/*!@brief
 * Read data from a secure IIC device.
 *
 * Parameters:
 * @param     adal:  The previously opened adal corresponding to the desired I2C bus.
 * @param     dev_addr:  The 7 bit device address shifted 1 bit to the left.
 * @param     offset: The beginning offset from where to start reading.
 * @param     data: a pointer to the buffer where read data should be stored.
 * @param     len: The number of bytes to read from the device and the minimum
 *           size of the buffer pointed to by data.
 *
 * @return:
 *      Upon success, 0 will be returned and 'len' bytes of data will be stored
 *      in the buffer pointed to by 'data'.  Otherwise, a negative value will
 *      be returned and errno will be set.  Possible errno's follow:
 *
 * @return     EBADF           'adal' is invalid
 * @return     EINVAL          At least one invalid parameter
 * @return     ETIME           a transfer timed out
 * @return     EALREADY        Lost arbitration
 * @return     ECOMM           Data miscompare
 * @return     ENXIO           Address not acknowledged
 * @return     ENODATA         Data not acknowledged
 * @return     EIO             low-level IO error
 * @return     ENODEV          Lost local bus ownership
 * @return     ENOLINK         FSI link was severed
 * @return     EDEADLK         FFDC must be collected
 */
int adal_siic_read(adal_t* adal, uint8_t dev_addr, uint8_t offset, uint8_t* data, uint8_t len);

/*!@brief
 * Write data to a secure IIC device 
 *
 * Parameters:
 * @param     adal:  The previously opened adal corresponding to the desired I2C bus.
 * @param     dev_addr:  The 7 bit device address shifted 1 bit to the left.
 * @param     offset: The beginning offset from where to start writing.
 * @param     data: a pointer to the buffer containing data to be written.
 * @param     len: The number of bytes to write to the device and the minimum
 *           size of the buffer pointed to by data.
 *
 * @return:
 *      Upon success, 0 will be returned and 'len' bytes of data will be written
 *      to the device.  Otherwise, a negative value will
 *      be returned and errno will be set.  Possible errno's follow:
 *
 * @return     EBADF           'adal' is invalid
 * @return     EINVAL          At least one invalid parameter
 * @return     ETIME           A transfer timed out
 * @return     EALREADY        Lost arbitration
 * @return     ECOMM           Data miscompare
 * @return     ENXIO           Address not acknowledged
 * @return     ENODATA         Data not acknowledged
 * @return     EIO             low-level IO error
 * @return     ENODEV          Lost local bus ownership
 * @return     ENOLINK         FSI link was severed
 * @return     EDEADLK         FFDC must be collected
 */
int adal_siic_write(adal_t* adal, uint8_t dev_addr, uint8_t offset, uint8_t* data, uint8_t len);

/*!@brief
 * Reset IRQ flags of a secure IIC device
 *
 * Parameters:
 * @param     adal:  The previously opened adal corresponding to the desired I2C bus.
 * @param     dev_addr:  The 7 bit device address shifted 1 bit to the left.
 * @param     offset: The beginning offset where IRQ's are stored.
 * @param     flags: a pointer to the buffer specifying IRQ bits to be cleared. 
 * @param     len: The number of bytes contained in the flags buffer.
 *
 * @return:
 *      Upon success, 0 will be returned and the specified IRQ's will be
 *      cleared.  Otherwise, a negative value will be returned and errno will 
 *      be set.  Possible errno's follow:
 *
 * @return     EBADF           'adal' is invalid
 * @return     EINVAL          At least one invalid parameter
 * @return     ETIME           A transfer timed out
 * @return     EALREADY        Lost arbitration
 * @return     ECOMM           Data miscompare
 * @return     ENXIO           Address not acknowledged
 * @return     ENODATA         Data not acknowledged
 * @return     EIO             low-level IO error
 * @return     ENODEV          Lost local bus ownership
 * @return     ENOLINK         FSI link was severed
 * @return     EDEADLK         FFDC must be collected
 */
int adal_siic_reset_irq(adal_t* adal, uint8_t dev_addr, uint8_t offset, uint8_t* flags, uint8_t len);


#ifdef __cplusplus
}
#endif

#endif /* __ADAL_IIC_INTERFACES_H__ */
