#ifndef __sqerrno_h
#define __sqerrno_h

#define     SQE_RMTERR_OFFSET       1000    /* remote queue offset -8100-8999 */

/***********      error code define       ************/
#define     SQE_NO_ERROR            0       /* no error */
#define     SQE_SEM_CREATE          -9100   /* create semaphore fail */
#define     SQE_FILE_CREATE         -9101   /* create file fail */
#define     SQE_MMAP_CREATE         -9102   /* file map memory fail */
#define     SQE_NO_HANDLE           -9103   /* no free queue handle */
#define     SQE_MEM_ALLOC           -9104   /* memory allocate fail */
#define     SQE_CONN_FAIL           -9105   /* connect fail */
#define     SQE_DATA_TOOBIG         -9106   /* data size > queue max msg size */
#define     SQE_QUE_LOCK            -9107   /* queue user by other process */
#define     SQE_QUE_FULL            -9108   /* queue full */
#define     SQE_INVALID_HANDLE      -9109   /* invalid queue handle */

#define     SQE_MSG_TOOBIG          -9110   /* msg size > buffer size */
#define     SQE_MSG_NOTFIND         -9111   /* message not foudn */
#define     SQE_USED_BYOTHER        -9112   /* queue used by other process */
#define     SQE_FILE_OPEN           -9113   /* file open fail */
#define     SQE_NO_PERMISSION       -9114   /* no permission */
#define     SQE_MSGSIZE_NOTMATCH    -9115   /* create an existed queue and
                                               size not match */
#define     SQE_GET_SEMKEY          -9116   /* get semaphore key fail */
#define     SQE_TRANSQUE_OPEN       -9117   /* open transaction queue fail */
#define     SQE_CONN_TIMEOUT        -9118   /* remote connect timeout */
#define     SQE_HOST_NOTFIND        -9119   /* remote host name not find */
#define     SQE_NOT_SUPPORT         -9120   /* API not support */
#define     SQE_QUE_NOTFIND         -9121   /* queue not find */
#define     SQE_IP_DISCONNECT       -9122   /* IP is not in connectionip */
#define     SQE_SYSQUE_NOTRUN       -9123   /* sysque not run */
#define     SQE_PROG_DOWN           -9124   /* program down */
#define     SQE_WAIT_TIMEOUT        -9125
#define     SQE_RMTCRT_DISABLE      -9126   /* remote create queue disable */
#define     SQE_INVALID_PARA        -9127   /* invalid parameter */
#define     SQE_QU_REMOVE           -9128   /* remove queue fail */
#define     SQE_SYNCSVR_UNDEF       -9129   /* sync svr undefined(WIN) */

#define     SQE_RUN_CHILD           -9133   /* run child fail */
#define     SQE_IP_FULL             -9136   /* ip table full */
#define     SQE_LOAD_CFG            -9137   /* load config fail */
#define     SQE_DOWN_NOACK          -9140   /* not recv ack while down */

#endif
