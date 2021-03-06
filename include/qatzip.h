/***************************************************************************
 *
 *   BSD LICENSE
 *
 *   Copyright(c) 2007-2017 Intel Corporation. All rights reserved.
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***************************************************************************/

/**
 *****************************************************************************
 * @file qatzip.h
 *
 * @defgroup qatZip Data Compression API
 *
 * @description
 *      These functions specify the API for Data Compression operations.
 *
 * @remarks
 *
 *
 *****************************************************************************/

#ifndef _QATZIP_H
#define _QATZIP_H

#ifdef __cplusplus
extern"C" {
#endif

#include <string.h>
/**
 *****************************************************************************
 *
 *  This API provides access to underlying compression functions in QAT hardware
 *  The API supports an implementation that provides compression service in
 *  software if not all of the required resources are not available to execute
 *  the compression service in hardware.
 *
 *  The API supports threaded applications.
 *  Applications can create threads and each of these threads can invoke the
 *  API defined herein.
 *
 *  For simplicity, initializations and setup function calls are not
 *  required to obtain compression services. If the initialization and setup
 *  functions are not called before compression or decompression requests, then
 *  they will be called with default arguments from within the compression or
 *  decompression functions. This results in several legal calling scenarios,
 *  described below.
 *
 *  Scenario 1 - all functions explicitly invoked by caller, with all arguments provided
 *
 *  qzInit(&sess_c, sw_backup);
 *  qzSetupSession(&sess_c, &params);
 *  qzCompress(&sess, src, &src_len, dest, &dest_len, 1);
 *  qzDecompress(&sess, src, &src_len, dest, &dest_len);
 *  qzTeardownSession(&sess);
 *  qzClose(&sess);
 *
 *
 * Scenario 2 - initialization function called, setup function not invoked by caller.
 *               This scenario can be used to specify the sw_backup argument to
 *               qzInit.
 *
 *  qzInit(&sess, sw_backup);
 *  qzCompress(&sess, src, &src_len, dest, &dest_len, 1);
 *      calls  qzSetupSession(sess, NULL);
 *  qzTeardownSession(&sess);
 *  qzClose(&sess);
 *
 *
 * Scenario 3 - calling application simply invokes the actual qzCompress functions
 *
 *  qzCompress(&sess, src, &src_len, dest, &dest_len, 0);
 *      calls qzInit sess, 1);
 *      calls qzSetupSession(sess, NULL);
 *  qzCompress(&sess, src, &src_len, dest, &dest_len, 1);
 *
 *  Notes: Invoking qzSetupSession with NULL for params sets up a session with
 *  default session attributed, detailed in the function description below.
 *
 *  If an application terminates with out invoking tear down and close
 *  functions, process termination will invoke memory and hardware instance
 *  cleanup.
 *
 *  If a thread terminates without invoking tear down and close functions,
 *  memory and hardware are not cleanup until the application exits.
 *
 *****************************************************************************/
/**
 *****************************************************************************
 * @ingroup qatZip
 *      Supported Huffman Headers
 *
 * @description
 *      This enumerated list identifies the Huffman header types
 *    supported by QATZip
 *
 *****************************************************************************/
typedef enum QzHuffmanHdr_E {
    QZ_DYNAMIC_HDR = 0,
    /**< Full Dynamic Huffman Trees */
    QZ_STATIC_HDR
    /**< Static Huffman Trees */
} QzHuffmanHdr_T;

/**
 *****************************************************************************
 * @ingroup qatZip
 *      Compress or decompress setting
 *
 * @description
 *      This enumerated list identifies the session directions
 *    supported by QATZip. A session can be compress, decompress
 *    or both.
 *
 *****************************************************************************/
typedef enum QzDirection_E {
    QZ_DIR_COMPRESS = 0,
    /**< Session will be used for compression */
    QZ_DIR_DECOMPRESS,
    /**< Session will be used for decompression */
    QZ_DIR_BOTH
    /**< Session will be used for both compression and decompression */
} QzDirection_T;

/**
 *****************************************************************************
 * @ingroup qatZip
 *    QATZIP Session Status definitions and function return codes
 *
 * @description
 *      This list identifies valid values for session status and function
 *    return codes.
 *
 *****************************************************************************/
#define QZ_OK                   (0)
/**<Success */
#define  QZ_DUPLICATE           (1)
/**<Can not process function again. No failure. */
#define QZ_FORCE_SW             (2)
/**<using SW: Switch to software because of previous block*/
#define  QZ_PARAMS              (-1)
/**<invalid parameter in function call */
#define  QZ_FAIL                (-2)
/**<Unspecified error */
#define QZ_BUF_ERROR            (-3)
/**<Insufficient buffer error */
#define QZ_DATA_ERROR           (-4)
/**<Input data was corrupted */
#define QZ_NO_HW                (11)
/**<using SW: No QAT HW detected */
#define QZ_NO_MDRV              (12)
/**<using SW: No memory driver detected */
#define QZ_NO_INST_ATTACH       (13)
/**<using SW: Could not attach to an instance */
#define QZ_LOW_MEM              (14)
/**<using SW: Not enough pinned memory */
#define QZ_NOSW_NO_HW           (-101)
/**<not using SW: No QAT HW detected */
#define QZ_NOSW_NO_MDRV         (-102)
/**<not using SW: No memory driver detected */
#define QZ_NOSW_NO_INST_ATTACH  (-103)
/**<not using SW: Could not attach to instance */
#define QZ_NOSW_LOW_MEM         (-104)
/**<not using SW: not enough pinned memory */


#define QZ_MAX_ALGORITHMS  ((int)255)
#define QZ_DEFLATE         ((unsigned char)8)
#define QZ_SNAPPY          ((unsigned char)'S')
#define QZ_LZ4             ((unsigned char)'4')

#define MIN(a,b) (((a)<(b))?(a):(b))
#define QZ_MEMCPY(dest, src, dest_sz, src_sz) \
            memcpy((void *)(dest), (void *) (src), (size_t)MIN(dest_sz, src_sz))

/**
 *****************************************************************************
 * @ingroup qatZip
 *    QATZIP Session Initialization parameters
 *
 * @description
 *      This structure contains data for initializing a session
 *
 *****************************************************************************/
typedef struct QzSessionParams_S {
    QzHuffmanHdr_T  huffman_hdr;
    /**<Dynamic or Static Huffman headers */
    QzDirection_T  direction;
    /**<compress or decompress */
    unsigned int  comp_lvl;
    /**<Compression level 1..9 */
    unsigned char  comp_algorithm;
    /** <Compress/decompression algorithms */
    unsigned int  poll_sleep;
    /**<nanosleep between poll [0..1000] 0 means no sleep */
    unsigned int  max_forks;
    /**<maximum forks permitted in the current thread.  */
    /**<0 means no forking permitted */
    unsigned char sw_backup;
    /**<0 means no sw backup, 1 means sw backup */
    unsigned int hw_buff_sz;
    /**<default buffer size, Must be a power of 2*/
    /**< 4K,8K,16K,32K,64K,128K */
    unsigned int input_sz_thrshold;
    /**<default threshold of compression service's input size */
    /**<for sw failover, if the size of input request less */
    /**<than the threshold, QATzip will route the request */
    /**<to software */
    unsigned int req_cnt_thrshold;
    /**set between 1 and 4, default 4*/
} QzSessionParams_T;

#define QZ_HUFF_HDR_DEFAULT          QZ_DYNAMIC_HDR
#define QZ_DIRECTION_DEFAULT         QZ_DIR_BOTH
#define QZ_COMP_LEVEL_DEFAULT        1
#define QZ_COMP_ALGOL_DEFAULT        QZ_DEFLATE
#define QZ_POLL_SLEEP_DEFAULT        10
#define QZ_MAX_FORK_DEFAULT          3
#define QZ_SW_BACKUP_DEFAULT         1
#define QZ_HW_BUFF_SZ                (64*1024)
#define QZ_HW_BUFF_MIN_SZ            (1*1024)
#define QZ_HW_BUFF_MAX_SZ            (512*1024)
#define QZ_COMP_THRESHOLD_DEFAULT    1024
#define QZ_COMP_THRESHOLD_MINIMUM    128
#define QZ_REQ_THRESHOLD_MINIMUM     1
#define QZ_REQ_THRESHOLD_MAXINUM     4
#define QZ_REQ_THRESHOLD_DEFAULT     4
/**
 *****************************************************************************
 * @ingroup qatZip
 *    QATZIP Session opaque data storage
 *
 * @description
 *      This structure contains a pointer to a structure with
 *    session state
 *
 *****************************************************************************/
typedef struct QzSession_S {
    signed long int hw_session_stat;
    /**< filled in during initialization, session startup and decompression */
    int thd_sess_stat;
    /**< note process compression and decompression thread state*/
    void  *internal;
    /**<session data is opaque to outside world */
    unsigned long total_in;
    /**<Total processed input data length in this session */
    unsigned long total_out;
    /**<Total output data length in this session */
} QzSession_T;

/**
 *****************************************************************************
 * @ingroup qatZip
 *    QATZIP status structure
 *
 * @description
 *      This structure contains data relating to the stat usof QAT on the
 *    platform
 *
 *****************************************************************************/
typedef struct QzStatus_S {
    unsigned short int qat_hw_count;
    /**<from PCI scan */
    unsigned char qat_service_stated;
    /**<Check if the QAT service is properly running  on at least one device*/
    unsigned char qat_mem_drvr;
    /**<1 if /dev/qat_mem exists */
    /**<2 if /dev/qat_mem has been opened */
    /**<0 otherwise */
    unsigned char qat_instance_attach;
    /**<Is this thread/g_process properly attached to an Instance? */
    unsigned long int memory_alloced;
    /**<Amount of memory allocated by this thread/process */
    unsigned char using_huge_pages;
    /**<Are memory slabs coming from huge pages */
    signed long int hw_session_status;
    /**<One of QATZIP Session Status */
    unsigned char algo_sw[QZ_MAX_ALGORITHMS];
    /**<support software algorithms */
    unsigned char algo_hw[QZ_MAX_ALGORITHMS];
    /**<count of hw devices supporting algorithms */
} QzStatus_T;

/**
 *****************************************************************************
 * @ingroup qatZip
 *      Initialize QAT hardware
 *
 * @description
 *      This function initializes the QAT hardware.
 *    This function is optional in the function calling sequence.  If
 *    desired, this call can be made to avoid latency impact during the
 *    first call to qzDecompress or qzCompress, or to set the sw_backup
 *    parameter explicitly.
 *    The input parameter sw_backup specifies the behavior of the function
 *    and that of the functions called with the same session in the event
 *    there are insufficient resources to establish a QAT based compression
 *    or decompression session.
 *
 *    Required resources include access to the QAT hardware, contiguous
 *    pinned memory for mmaping the hardware rings, and contiguous
 *    pinned memory for buffers.
 *
 * @context
 *      This function shall not be called in an interrupt context.
 * @assumptions
 *      None
 * @sideEffects
 *    This function will:
 *      1) start the user space driver if necessary
 *      2) allocate all hw instances available
 * @blocking
 *      Yes
 * @reentrant
 *      No
 * @threadSafe
 *      Yes
 *
 * @param[in]       sess pointer to opaque instance and session data.
 * @param[in]       sw_backup 0 for no sw backup, 1 for sw backup
 *
 * @retval QZ_OK                   Function executed successfully. A hw
 *                                 or sw instance has been allocated to
 *                                 the calling process/thread.
 * @retval QZ_DUPLICATE            This process/thread already has a hw
 *                                 instance
 * @retval QZ_PARAMS               *sess is NULL
 * @retval QZ_NOSW_NO_HW           No hardware and no sw session being
 *                                 established
 * @retval QZ_NOSW_NO_MDRV         No memory driver. No software session
 *                                 established
 * @retval QZ_NOSW_NO_INST_ATTACH  No instance avail.
 *                                 No software session established
 * @retval QZ_NOSW_LOW_MEM         Not enough pinned memory available.
 *                                 No software session established
 *
 * @pre
 *      None
 * @post
 *      None
 * @note
 *      Only a synchronous version of this function is provided.
 *
 * @see
 *      None
 *
 *****************************************************************************/
int qzInit(QzSession_T *sess,  unsigned char sw_backup);

/**
 *****************************************************************************
 * @ingroup qatZip
 *      initialize a QATZip session
 *
 * @description
 *      This function establishes a QAT session.  This involves associating
 *    a hardware instance to the session, allocating buffers.  If all of these
 *    activities can not be completed successfully, then this function
 *    will set up a software based session of param->sw_backup is set to 1.
 *
 *    Before this function is called, the hardware must have been
 *    successfully started via qzInit.
 *
 *    If *sess includes an existing hardware or software session, then this
 *    session will be torn down before a new one is attempted.
 *
 * @context
 *      This function shall not be called in an interrupt context.
 * @assumptions
 *      None
 * @sideEffects
 *      None
 * @blocking
 *      Yes
 * @reentrant
 *      No
 * @threadSafe
 *      Yes
 *
 * @param[in]       sess         Session handle
 * @param[in]       params       Parameters for session
 *
 *
 * @retval QZ_OK                   Function executed successfully. A hw or sw
 *                                 based compression session has been created.
 * @retval QZ_PARAMS               *sess is NULL or member of params is invalid
 * @retval QZ_NOSW_NO_HW           No hardware and no sw session being
 *                                 established
 * @retval QZ_NOSW_NO_MDRV         No memory driver. No software session
 *                                 established
 * @retval QZ_NOSW_NO_INST_ATTACH  No instance avail.
 *                                 No software session established
 * @retval QZ_NO_LOW_MEM           Not enough pinned memory available.
 *                                 No software session established
 *
 * @pre
 *      None
 * @post
 *      None
 * @note
 *      Only a synchronous version of this function is provided.
 *
 * @see
 *      None
 *
 *****************************************************************************/
int qzSetupSession(QzSession_T *sess,  QzSessionParams_T *params);

/**
 *****************************************************************************
 * @ingroup qatZip
 *      compress a buffer
 *
 * @description
 *      This function will compress a buffer if either a hw based session
 *    or a software based session is available. If no session has been
 *    established - as indicated by the contents of *sess - then this app
 *    will attempt to set up a session using qzinit and qzSetupSession.
 *
 *    This function will place completed compression blocks in the output
 *    buffer.
 *
 *    The caller must check the updated src_len.  This value will be the
 *    number of consumed bytes on exit.  The calling API may have to
 *    process the destination  buffer and call again.
 *
 *    The parameter dest_len will be set to the number of bytes produced in
 *    the destination buffer.  This value may be zero if no data was produced
 *    which may occur if the consumed data is retained internally. A
 *    possible reason for this may be small amounts of data in the src
 *    buffer.
 *
 * @context
 *      This function shall not be called in an interrupt context.
 * @assumptions
 *      None
 * @sideEffects
 *      None
 * @blocking
 *      Yes
 * @reentrant
 *      No
 * @threadSafe
 *      Yes
 *
 * @param[in]       sess     Session handle
 * @param[in]       src      point to source buffer
 * @param[in,out]   src_len  length of source buffer. Modified to number
 *                           of bytes consumed
 * @param[in]       dest     point to destination buffer
 * @param[in,out]   dest_len length of destination buffer.  Modified
 *                           to length of compressed data when
 *                           function returns
 * @param[in]       last     1 for 'No more data to be compressed'
 *                           0 for 'More data to be compressed'
 *
 * @retval QZ_OK             Function executed successfully.
 * @retval QZ_FAIL           Function did not succeed.
 * @retval QZ_PARAMS         *sess is NULL or member of params is invalid
 * @pre
 *      None
 * @post
 *      None
 * @note
 *      Only a synchronous version of this function is provided.
 *
 * @see
 *      None
 *
 *****************************************************************************/
int qzCompress(QzSession_T *sess, const unsigned char *src,
               unsigned int *src_len, unsigned char *dest, unsigned int *dest_len,
               unsigned int last);

/**
 *****************************************************************************
 * @ingroup qatZip
 *      compress a buffer and return the CRC check sum
 *
 * @description
 *      This function will compress a buffer if either a hw based session
 *    or a software based session is available. If no session has been
 *    established - as indicated by the contents of *sess - then this app
 *    will attempt to set up a session using qzinit and qzSetupSession.
 *
 *    This function will place completed compression blocks in the output
 *    buffer and put CRC32 checksum for compressed input data in user provided
 *    bufer *crc.
 *
 *    The caller must check the updated src_len.  This value will be the
 *    number of consumed bytes on exit.  The calling API may have to
 *    process the destination  buffer and call again.
 *
 *    The parameter dest_len will be set to the number of bytes produced in
 *    the destination buffer.  This value may be zero if no data was produced
 *    which may occur if the consumed data is retained internally. A
 *    possible reason for this may be small amounts of data in the src
 *    buffer.
 *
 * @context
 *      This function shall not be called in an interrupt context.
 * @assumptions
 *      None
 * @sideEffects
 *      None
 * @blocking
 *      Yes
 * @reentrant
 *      No
 * @threadSafe
 *      Yes
 *
 * @param[in]       sess     Session handle
 * @param[in]       src      point to source buffer
 * @param[in,out]   src_len  length of source buffer. Modified to number
 *                           of bytes consumed
 * @param[in]       dest     point to destination buffer
 * @param[in,out]   dest_len length of destination buffer.  Modified
 *                           to length of compressed data when
 *                           function returns
 * @param[in]       last     1 for 'No more data to be compressed'
 *                           0 for 'More data to be compressed'
 * @param[in,out]   crc      point to CRC32 checksum buffer
 *
 * @retval QZ_OK             Function executed successfully.
 * @retval QZ_FAIL           Function did not succeed.
 * @retval QZ_PARAMS         *sess is NULL or member of params is invalid
 * @pre
 *      None
 * @post
 *      None
 * @note
 *      Only a synchronous version of this function is provided.
 *
 * @see
 *      None
 *
 *****************************************************************************/
int qzCompressCrc(QzSession_T *sess, const unsigned char *src,
                  unsigned int *src_len, unsigned char *dest, unsigned int *dest_len,
                  unsigned int last, unsigned long *crc);


/**
 *****************************************************************************
 * @ingroup qatZip
 *      decompress a buffer
 *
 * @description
 *      This function will decompress a buffer if either a hw based session
 *    or a software based session is available. If no session has been
 *    established - as indicated by the contents of *sess - then this app
 *    will attempt to set up a session using qzinit and qzSetupSession.
 *
 *      The resulting compressed block of data will be composed of one or more
 *      gzip blocks per RFC1952.
 *
 * @context
 *      This function shall not be called in an interrupt context.
 * @assumptions
 *      None
 * @sideEffects
 *      None
 * @blocking
 *      Yes
 * @reentrant
 *      No
 * @threadSafe
 *      Yes
 *
 * @param[in]     sess                      Session handle
 * @param[in]     src                       point to source buffer
 * @param[in]     src_len                   length of source buffer. Modified to
 *                                          length of processed compressed data
 *                                          when function returns
 * @param[in]      dest                     point to destination buffer
 * @param[in,out]  dest_len                 length of destination buffer. Modified
 *                                          to length of decompressed data when
 *                                          function returns
 *
 * @retval QZ_OK      Function executed successfully.
 * @retval QZ_FAIL    Function did not succeed.
 * @retval QZ_PARAMS  *sess is NULL or member of params is invalid
 * @pre
 *      None
 * @post
 *      None
 * @note
 *      Only a synchronous version of this function is provided.
 *
 * @see
 *      None
 *
 *****************************************************************************/
int qzDecompress(QzSession_T *sess, const unsigned char *src,
                 unsigned int *src_len, unsigned char *dest,
                 unsigned int *dest_len);

/**
 *****************************************************************************
 * @ingroup qatZip
 *      Deinitialize a QATZip session
 *
 * @description
 *      This function disconnects a session from a hardware instance and
 *    deallocates buffers. If no session has been initialized, then no
 *    action will take place.
 *
 * @context
 *      This function shall not be called in an interrupt context.
 * @assumptions
 *      None
 * @sideEffects
 *      None
 * @blocking
 *      Yes
 * @reentrant
 *      No
 * @threadSafe
 *      Yes
 *
 * @param[in]       sess  Session handle
 *
 * @retval QZ_OK          Function executed successfully.
 * @retval QZ_FAIL        Function did not succeed.
 * @retval QZ_PARAMS      *sess is NULL or member of params is invalid
 *
 * @pre
 *      None
 * @post
 *      None
 * @note
 *      Only a synchronous version of this function is provided.
 *
 * @see
 *      None
 *
 *****************************************************************************/
int qzTeardownSession(QzSession_T *sess);

/**
 *****************************************************************************
 * @ingroup qatZip
 *      terminates a QATZip session
 *
 * @description
 *      This function closes the connection with QAT
 *
 * @context
 *      This function shall not be called in an interrupt context.
 * @assumptions
 *      None
 * @sideEffects
 *      None
 * @blocking
 *      Yes
 * @reentrant
 *      No
 * @threadSafe
 *      Yes
 *
 * @param[in]       sess  pointer to session data
 *
 * @retval QZ_OK          Function executed successfully.
 * @retval QZ_FAIL        Function did not succeed.
 * @retval QZ_PARAMS      *sess is NULL or member of params is invalid
 *
 * @pre
 *      None
 * @post
 *      None
 * @note
 *      Only a synchronous version of this function is provided.
 *
 * @see
 *      None
 *
 *****************************************************************************/
int qzClose(QzSession_T *sess);


/**
 *****************************************************************************
 * @ingroup qatZip
 *      Get current QAT status
 *
 * @description
 *    This function retrieves the status of QAT in the platform.
 *    The status structure will be filled in as follows:
 *  qat_hw_count         number of discovered QAT devices on PCU bus
 *  qat_service_stated   1 if qzInit has been successfully run, 0 otherwise
 *  qat_mem_drvr         1 if the QAT memory driver is installed, 0 otherwise
 *  qat_instance_attach  1 if session has attached to a hw instance, 0 otherwise
 *  memory_alloced       amount of memory, in kilobytes, from kernel or huge
 *                       pages allocated  by this process/thread.
 *  using_huge_pages     1 if memory is being allocated from huge pages, 0 if
 *                       memory is being allocated from standard kernel memory
 *  hw_session_stat      Hw session status:  one of:
 *      QZ_OK
 *      QZ_FAIL
 *      QZ_NO_HW
 *      QZ_NO_MDRV
 *      QZ_NO_INST_ATTACH
 *      QZ_LOW_MEM
 *      QZ_NOSW_NO_HW
 *      QZ_NOSW_MDRV
 *      QZ_NOSW_NO_INST_ATTACH
 *      QZ_NOSW_LOW_MEM
 *
 * @context
 *      This function shall not be called in an interrupt context.
 * @assumptions
 *      None
 * @sideEffects
 *      None
 * @blocking
 *      Yes
 * @reentrant
 *      No
 * @threadSafe
 *      Yes
 *
 * @param[in]  dcInstance  Instance handle derived from discovery functions
 * @retval QZ_OK           Function executed successfully. A hw based
 *                         compression session has been created.
 * @retval QZ_PARAMS       *status is NULL
 *
 * @pre
 *      None
 * @post
 *      None
 * @note
 *      Only a synchronous version of this function is provided.
 *
 * @see
 *      None
 *
 *****************************************************************************/
int qzGetStatus(QzSession_T *sess, QzStatus_T *status);

/**
 *****************************************************************************
 * @ingroup qatZip
 *     Get the max compressed output length
 *
 * @description
 *     Get the max compressed output length
 *
 * @context
 *      This function shall not be called in an interrupt context.
 * @assumptions
 *      None
 * @sideEffects
 *      None
 * @blocking
 *      Yes
 * @reentrant
 *      No
 * @threadSafe
 *      Yes
 *
 * @param[in]
 *         src_sz     Input data length in byte.
 *
 * @retval dest_sz    Max compressed data output length in byte.
 *                    When src_sz equal to 0, the return value is 0.
 *
 * @pre
 *      None
 * @post
 *      None
 * @note
 *      Only a synchronous version of this function is provided.
 *
 * @see
 *      None
 *
 *****************************************************************************/
#define QZ_SKID_PAD_SZ 48
unsigned int qzMaxCompressedLength(unsigned int src_sz);

/**
 *****************************************************************************
 * @ingroup qatZip
 *     Set default QzSessionParams_T value
 *
 * @description
 *     Set default QzSessionParams_T value
 *
 * @context
 *      This function shall not be called in an interrupt context.
 * @assumptions
 *      None
 * @sideEffects
 *      None
 * @blocking
 *      Yes
 * @reentrant
 *      No
 * @threadSafe
 *      Yes
 *
 * @param[in]
 *         defaults   The pointer to value to be set as default.
 *
 * @retval      QZ_OK      Success on setting default value.
 * @retval      QZ_PARAM   Fail to set default value.
 *
 * @pre
 *      None
 * @post
 *      None
 * @note
 *      Only a synchronous version of this function is provided.
 *
 * @see
 *      None
 *
 *****************************************************************************/
int qzSetDefaults(QzSessionParams_T *defaults);

/**
 *****************************************************************************
 * @ingroup qatZip
 *     Get default QzSessionParams_T value
 *
 * @description
 *     Get default QzSessionParams_T value
 *
 * @context
 *      This function shall not be called in an interrupt context.
 * @assumptions
 *      None
 * @sideEffects
 *      None
 * @blocking
 *      Yes
 * @reentrant
 *      No
 * @threadSafe
 *      Yes
 *
 * @param[in]   defaults   The pointer to default value.
 *
 * @retval      QZ_OK      Success on getting default value.
 * @retval      QZ_PARAM   Fail to get default value.
 *
 * @pre
 *      None
 * @post
 *      None
 * @note
 *      Only a synchronous version of this function is provided.
 *
 * @see
 *      None
 *
 *****************************************************************************/
int qzGetDefaults(QzSessionParams_T *defaults);

/**
 *****************************************************************************
 * @ingroup qatZip
 *     Allocate different types of memory
 *
 * @description
 *     Allocate different types of memory
 *
 * @context
 *      This function shall not be called in an interrupt context.
 * @assumptions
 *      None
 * @sideEffects
 *      None
 * @blocking
 *      Yes
 * @reentrant
 *      No
 * @threadSafe
 *      Yes
 *
 * @param[in]       sz                  Memory size to be allocated.
 * @param[in]       numa                NUMA node from which to allocate memory
 * @param[in]       force_pinned        PINNED_MEM allocate continous memory
 *                                      COMMON_MEM allocate non-continous memory
 *
 * @retval          NULL                Fail to allocate memory
 * @retval          adress              The address to allocated memory
 *
 * @pre
 *      None
 * @post
 *      None
 * @note
 *      Only a synchronous version of this function is provided.
 *
 * @see
 *      None
 *
 *****************************************************************************/
void *qzMalloc(size_t sz, int numa, int force_pinned);

/**
 *****************************************************************************
 * @ingroup qatZip
 *     Free allocated memory
 *
 * @description
 *     Free allocated memory
 *
 * @context
 *      This function shall not be called in an interrupt context.
 * @assumptions
 *      None
 * @sideEffects
 *      None
 * @blocking
 *      Yes
 * @reentrant
 *      No
 * @threadSafe
 *      Yes
 *
 * @param[in]       m                   Memory address to be freed.
 *
 * @pre
 *      None
 * @post
 *      None
 * @note
 *      Only a synchronous version of this function is provided.
 *
 * @see
 *      None
 *
 *****************************************************************************/
void qzFree(void *m);

/**
 *****************************************************************************
 * @ingroup qatZip
 *     Check whether the address is available
 *
 * @description
 *     Check whether the address is available
 *
 * @context
 *      This function shall not be called in an interrupt context.
 * @assumptions
 *      None
 * @sideEffects
 *      None
 * @blocking
 *      Yes
 * @reentrant
 *      No
 * @threadSafe
 *      Yes
 *
 * @param[in]
 *              a       Address need to be checked
 *
 * @retval      1       The Address is available
 * @retval      0       The address is not available
 *
 * @pre
 *      None
 * @post
 *      None
 * @note
 *      Only a synchronous version of this function is provided.
 *
 * @see
 *      None
 *
 *****************************************************************************/
int qzMemFindAddr(unsigned char *a);

#ifdef __cplusplus
}
#endif

#endif
