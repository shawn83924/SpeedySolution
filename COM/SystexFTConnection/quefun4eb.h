#ifndef __quefun4eb_h
#define __quefun4eb_h

#if R_6 || LINUX || BSD || SOLARIS || SCO || SUN || HP
#undef WIN
#else
#define WIN
#endif

#if   LINUX || BSD
#include        <sys/time.h>
#else
#include        <time.h>
#endif

#ifndef WIN
#define DLL_IMPORT_EXPORT 
#define CONNDLL_IMPORT_EXPORT
#else 
#include        <time.h>

#ifdef DLL_SOURCE_CODE
#define DLL_IMPORT_EXPORT __declspec(dllexport) __stdcall
#else
#define DLL_IMPORT_EXPORT __declspec(dllexport) __stdcall
#endif


#ifdef CONNDLL_SOURCE_CODE
#define CONNDLL_IMPORT_EXPORT __declspec(dllexport) __stdcall
#else
#define CONNDLL_IMPORT_EXPORT __declspec(dllexport) __stdcall
#endif

#endif

#ifdef __cplusplus
#define NoMangle extern "C"
#else
#define NoMangle
#endif

/*    type define     */
#ifndef __mytype_h
typedef int            Handle;
#endif

/* const define */
#define     MSGKEY_SIZE             11
#define     MSGKEY_BUFSIZE          (MSGKEY_SIZE+1)

   
/*      struct define       */
struct QueueAttr {
    /* local queue use only */
    unsigned int    initmsg;        /* initial messages on queue */
    unsigned int    extmsg;         /* extented messages erery turn */
    unsigned int    maxmsg;         /* max message in queue */
    unsigned int    maxmsgsize;     /* max message size */
    unsigned int    msgqnum;        /* current # of msg on que  */
    unsigned int    maxqbytes;

    time_t  msgwtime;
    time_t  msgrtime;

    /* remote queue use only */
    int     sync;
    int     timeout;        /* milliseconds */
    char    dqname[48];     /* dead letter queue name */
};
#define sQueueAttr    sizeof( struct QueueAttr )



/*****************************************************************************
**                 SQI API declaration                       
******************************************************************************/

#ifndef HP
/*****************************************************************************
** FUNCTION : SQCreate
** DESCRIPTION : create message queue if it is not existed
** Arguments :
**	[IN] ip : remote host IP, "xx.xx.xx.xx"; "0.0.0.0" or NULL for local
**	[IN] port : remote SYSQue system listen port
**	[IN] sqname : message queue name to create
**	[IN] initmsg : max message number of this message queue 
**	[IN] maxmsgsize : max message size per message
** RETURN :
**	>= 0 : message queue handle
**	SQE_SEM_CREATE
**	SQE_GET_SEMKEY
**	SQE_FILE_CREATE
**	SQE_MMAP_CREATE
**	SQE_NO_HANDLE
**	SQE_MSGSIZE_NOTMATCH
**	SQE_INVALID_PARA
**	SQE_CONN_FAIL
**	SQE_CONN_TIMEOUT
**	SQE_TRANSQUE_OPEN
**	SQE_HOST_NOTFIND
**	SQE_RUN_CHILD
**	SQE_PROG_DOWN
******************************************************************************/
NoMangle Handle DLL_IMPORT_EXPORT SQCreate( unsigned char *ip,int port,char *sqname,unsigned int initmsg,unsigned int maxmsgsize );

/*****************************************************************************
** FUNCTION : SQOpen
** DESCRIPTION : open an existed message queue
** Arguments :
**	[IN] ip : remote host IP, "xx.xx.xx.xx"; "0.0.0.0" or NULL for local
**	[IN] port : remote SysQue system listen port
**	[IN] sqname : message queue name to create
** RETURN :
**	>= 0 : message queue handle
**	SQE_SEM_CREATE
**	SQE_GET_SEMKEY
**	SQE_FILE_OPEN
**	SQE_QUE_NOTFIND
**	SQE_MMAP_CREATE
**	SQE_NO_HANDLE
**	SQE_INVALID_PARA
**	SQE_CONN_FAIL
**	SQE_CONN_TIMEOUT
**	SQE_TRANSQUE_OPEN
**	SQE_HOST_NOTFIND
**	SQE_RUN_CHILD
**	SQE_PROG_DOWN
******************************************************************************/
NoMangle Handle DLL_IMPORT_EXPORT SQOpen( unsigned char *ip,int port,char *sqname );

/*****************************************************************************
** FUNCTION : SQClose
** DESCRIPTION : close message queue
** Arguments :
**	[IN] sqhandle : message queue handle
** RETURN :
**	= 0 : ok
**	SQE_INVALID_HANDLE
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQClose( Handle sqhandle );

/*****************************************************************************
** FUNCTION : SQDrop
** DESCRIPTION : delete local message queue file
** Arguments :
**	[IN] sqname : message queue name
** RETURN :
**	= 0 : ok
**	SQE_INVALID_PARA
**	SQE_QU_REMOVE
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQDrop( char *sqname );

/*****************************************************************************
** FUNCTION : SQRead
** DESCRIPTION : read one message from message queue
** Arguments :
**	[IN] sqhandle : message queue handle
**	[OUT] data : storage location for data
**	[IN] len : maximum number of characters to read
** RETURN :
**	>=0 : read message bytes
**	SQE_MSG_TOOBIG
**	SQE_QUE_LOCK
**	SQE_INVALID_HANDLE
**	SQE_INVALID_PARA
**	SQE_MMAP_CREATE
**	SQE_CONN_FAIL
**	SQE_CONN_TIMEOUT
**	SQE_RUN_CHILD
**	SQE_PROG_DOWN
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQRead( Handle sqhandle,char *data,int len );

/*****************************************************************************
** FUNCTION : SQReadWithKey
** DESCRIPTION : read one message and message key from message queue
** Arguments :
**	[IN] sqhandle : message queue handle
**	[OUT] data : storage location for data
**	[IN] len : maximum number of characters to read
**	[OUT] keybuf : storage location for message key
** RETURN :
**	>=0 : read message bytes
**	SQE_MSG_TOOBIG
**	SQE_QUE_LOCK
**	SQE_INVALID_HANDLE
**	SQE_INVALID_PARA
**	SQE_MMAP_CREATE
**	SQE_CONN_FAIL
**	SQE_CONN_TIMEOUT
**	SQE_RUN_CHILD
**	SQE_PROG_DOWN
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQReadWithKey( int sqhandle,char *data,int len, char *keyBuf);

/*****************************************************************************
** FUNCTION : SQPeekWithKey
** DESCRIPTION : peek one message and message key from message queue
** Arguments :
**	[IN] hQu : message queue handle
**	[IN] iSeq : message sequence(0 to N)
**	[OUT] chDataBuf : storage location for data
**	[IN] iDataBufSize : maximum number of characters to read
**	[OUT] chKeyBuf : storage location for message key
** RETURN :
**	>=0 : read message bytes
**	SQE_MSG_TOOBIG
**	SQE_QUE_LOCK
**	SQE_MSG_NOTFIND
**	SQE_INVALID_HANDLE
**	SQE_INVALID_PARA
**	SQE_MMAP_CREATE
**	SQE_CONN_FAIL
**	SQE_CONN_TIMEOUT
**	SQE_RUN_CHILD
**	SQE_PROG_DOWN
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQPeekWithKey( Handle hQu, int iSeq,
        char *chDataBuf, unsigned int iDataBufSize, char *chKeyBuf );

/*****************************************************************************
** FUNCTION : SQWrite
** DESCRIPTION : write one message to message queue
** Arguments :
**	[IN] sqhandle : message queue handle
**	[IN] data : pointer to data to be written
**	[IN] len : data size in bytes
**	[IN][OUT] keybuf : pointer to message/storage location for message key
** RETURN :
**	>=0 : write message bytes
**	SQE_DATA_TOOBIG
**	SQE_QUE_LOCK
**	SQE_QUE_FULL
**	SQE_INVALID_HANDLE
**	SQE_INVALID_PARA
**	SQE_NO_PERMISSION
**	SQE_MMAP_CREATE
**	SQE_CONN_FAIL
**	SQE_CONN_TIMEOUT
**	SQE_RUN_CHILD
**	SQE_PROG_DOWN
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQWrite( Handle sqhandle,char *data,int len,char *key );

/*****************************************************************************
** FUNCTION : SQDelete
** DESCRIPTION : delete one message from message queue by message key
** Arguments :
**	[IN] sqhandle : message queue handle
**	[IN] msgkey : message key
** RETURN :
**	=0 : ok
**	SQE_QUE_LOCK
**	SQE_INVALID_HANDLE
**	SQE_INVALID_PARA
**	SQE_MSG_NOTFIND
**	SQE_MMAP_CREATE
**	SQE_CONN_FAIL
**	SQE_CONN_TIMEOUT
**	SQE_RUN_CHILD
**	SQE_PROG_DOWN
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQDelete( Handle sqhandle,char *msgkey );

/*****************************************************************************
** FUNCTION : SQQuery
** DESCRIPTION : query one message from message queue by message key
** Arguments :
**	[IN] sqhandle : message queue handle
**	[IN] msgkey : message key
**	[OUT] data : storage location for data
**	[IN] len : maximum number of characters to read
** RETURN :
**	>=0 : message bytes
**	SQE_MSG_TOOBIG
**	SQE_QUE_LOCK
**	SQE_INVALID_HANDLE
**	SQE_INVALID_PARA
**	SQE_MSG_NOTFIND
**	SQE_MMAP_CREATE
**	SQE_CONN_FAIL
**	SQE_CONN_TIMEOUT
**	SQE_RUN_CHILD
**	SQE_PROG_DOWN
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQQuery( Handle sqhandle,char *msgkey,char *data,int len );

/*****************************************************************************
** FUNCTION : SQReadByKey
** DESCRIPTION : read one message from message queue by message key
** Arguments :
**	[IN] sqhandle : message queue handle
**	[IN] msgkey : message key
**	[OUT] data : storage location for data
**	[IN] len : maximum number of characters to read
** RETURN :
**	>=0 : message bytes
**	SQE_MSG_TOOBIG
**	SQE_QUE_LOCK
**	SQE_INVALID_HANDLE
**	SQE_INVALID_PARA
**	SQE_MSG_NOTFIND
**	SQE_MMAP_CREATE
**	SQE_CONN_FAIL
**	SQE_CONN_TIMEOUT
**	SQE_RUN_CHILD
**	SQE_PROG_DOWN
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQReadByKey( Handle sqhandle,char *msgkey,char *data,int len );

/*****************************************************************************
** FUNCTION : SQUpdate
** DESCRIPTION : update one message to message queue by message key
** Arguments :
**	[IN] sqhandle : message queue handle
**	[IN] msgkey : message key
**	[IN] data : pointer to new data to update
**	[IN] len : data size in bytes
** RETURN :
**	>=0 : message bytes
**	SQE_DATA_TOOBIG
**	SQE_QUE_LOCK
**	SQE_INVALID_HANDLE
**	SQE_INVALID_PARA
**	SQE_NO_PERMISSION
**	SQE_MSG_NOTFIND
**	SQE_CONN_FAIL
**	SQE_CONN_TIMEOUT
**	SQE_RUN_CHILD
**	SQE_PROG_DOWN
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQUpdate( Handle sqhandle,char *msgkey,char *data,int len );

/*****************************************************************************
** FUNCTION : SQMsgLeft
** DESCRIPTION : ask message number left in message queue
** Arguments :
**	[IN] sqhandle : message queue handle
** RETURN :
**	>=0 : message number
**	SQE_QUE_LOCK
**	SQE_INVALID_HANDLE
**	SQE_CONN_FAIL
**	SQE_CONN_TIMEOUT
**	SQE_MMAP_CREATE
**	SQE_RUN_CHILD
**	SQE_PROG_DOWN
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQMsgLeft( Handle sqhandle );

/*****************************************************************************
** FUNCTION : SQMsgClrAll
** DESCRIPTION : clear message in message queue
** Arguments :
**	[IN] sqhandle : message queue handle
** RETURN :
**	=0 : ok
**	SQE_QUE_LOCK
**	SQE_INVALID_HANDLE
**	SQE_CONN_FAIL
**	SQE_CONN_TIMEOUT
**	SQE_MMAP_CREATE
**	SQE_RUN_CHILD
**	SQE_PROG_DOWN
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQMsgClrAll( Handle sqhandle );

/*****************************************************************************
** FUNCTION : SQQueueFull
** DESCRIPTION : check message queue full or not
** Arguments :
**	[IN] sqhandle : message queue handle
** RETURN :
**	=0 : message queue not full
**	=1 : message queue is full
**	SQE_QUE_LOCK
**	SQE_INVALID_HANDLE
**	SQE_CONN_FAIL
**	SQE_CONN_TIMEOUT
**	SQE_RUN_CHILD
**	SQE_PROG_DOWN
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQQueueFull( Handle sqhandle );

/*****************************************************************************
** FUNCTION : SQGetQueAttr
** DESCRIPTION : get message queue attribute
** Arguments :
**	[IN] sqhandle : message queue handle
**	[OUT] queattr : storage location for message queue attribute
** RETURN :
**	=0 : ok
**	SQE_QUE_LOCK
**	SQE_INVALID_HANDLE
**	SQE_INVALID_PARA
**	SQE_CONN_FAIL
**	SQE_CONN_TIMEOUT
**	SQE_RUN_CHILD
**	SQE_PROG_DOWN
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQGetQueAttr( Handle sqhandle,struct QueueAttr *queattr );

/*****************************************************************************
** FUNCTION : SQSetQueAttr
** DESCRIPTION : set message queue attribute
** Arguments :
**	[IN] sqhandle : message queue handle
**	[IN] queattr : pointer to attribute to be written
** RETURN :
**	=0 : ok
**	SQE_QUE_LOCK
**	SQE_INVALID_HANDLE
**	SQE_INVALID_PARA
**	SQE_NO_PERMISSION
**	SQE_CONN_FAIL
**	SQE_CONN_TIMEOUT
**	SQE_RUN_CHILD
**	SQE_PROG_DOWN
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQSetQueAttr( Handle sqhandle,struct QueueAttr *queattr );

/*****************************************************************************
** FUNCTION : SQGetLastError
** DESCRIPTION : get last system error
** Arguments :
**	none
** RETURN :
**	last system error code
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQGetLastError();

/*****************************************************************************
** FUNCTION : SQSetLocalQueuePath
** DESCRIPTION : set local queue path
** Arguments :
**	[IN] lpath : local queue path
** RETURN :
**	0 : ok
**	SQE_INVALID_PARA
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQSetLocalQueuePath( char *lpath );

/*****************************************************************************
** FUNCTION : SQPeek
** DESCRIPTION : peek one message from local message queue
** Arguments :
**	[IN] sqhandle : message queue handle
**	[IN] n : sequence number (from 0 to x-1, x = total message in queue)
**	[OUT] data : storage location for data
**	[IN] len : maximum number of characters to read
** RETURN :
**	>=0 : peek message bytes
**	SQE_MSG_TOOBIG
**	SQE_QUE_LOCK
**	SQE_INVALID_HANDLE
**	SQE_INVALID_PARA
**	SQE_MSG_NOTFIND
**	SQE_NOT_SUPPORT
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQPeek(Handle sqhandle,int n,char *data,unsigned int len );

/*****************************************************************************
** FUNCTION : GetIP
** DESCRIPTION : get host IP
** Arguments :
**	[IN] ip : storage location for IP
** RETURN :
**	=0 : ok
**	-1 : fail
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT GetIP( char *ip );

/*****************************************************************************
** FUNCTION : SQConnSts
** DESCRIPTION : check if the communication program is running
** Arguments :
**	[IN] ip : which IP to check
**	[IN] porft : port number
** RETURN :
**	=0 : ok
**	SQE_IP_DISCONNECT
**	SQE_PROG_DOWN
******************************************************************************/
NoMangle int CONNDLL_IMPORT_EXPORT SQConnSts( unsigned char *ip,int port );

/*****************************************************************************
** FUNCTION : SQSYSQueSts
** DESCRIPTION : check if the sysque gateway is running
** Arguments :
**	none
** RETURN :
**	=0 : ok
**	SQE_SYSQUE_NOTRUN
**	SQE_PROG_DOWN
******************************************************************************/
NoMangle int CONNDLL_IMPORT_EXPORT SQSysQueSts();

/*****************************************************************************
** FUNCTION : SQGetPlatform
** DESCRIPTION : get platform string
** Arguments :
**	[OUT] buf : storage location for platform
**	[IN] len : maximum number of characters to write to buffer
** RETURN :
**	=0 : ok
**	SQE_INVALID_PARA
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQGetPlatform(char *buf, int size);

/*****************************************************************************
** FUNCTION : SQGetVersion
** DESCRIPTION : get system version string
** Arguments :
**	[OUT] buf : storage location for system version
**	[IN] len : maximum number of characters to write to buffer
** RETURN :
**	>0 : ok
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQGetVersion(char *buf, int len);

/*****************************************************************************
** FUNCTION : SQGetLibVersion
** DESCRIPTION : get library version string
** Arguments :
**	[OUT] buf : storage location for library version
**	[IN] len : maximum number of characters to write to buffer
** RETURN :
**	>0 : ok
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQGetLibVersion(char *buf, int len);

/*****************************************************************************
** FUNCTION : SQGetReleaseDate
** DESCRIPTION : get release date string
** Arguments :
**	[OUT] buf : storage location for release date
**	[IN] len : maximum number of characters to write to buffer
** RETURN :
**	=0 : ok
**	SQE_INVALID_PARA
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQGetReleaseDate(char *buf, int len);

/*****************************************************************************
** FUNCTION : SendNormal
** DESCRIPTION : write one message to system receive queue(FBrcvqueue.map)
** Arguments :
**	[IN] ip : remote host IP, "xx.xx.xx.xx"; "0.0.0.0" or NULL for local
**	[IN] port : remote SysQue system listen port
**	[IN] discard : reserve (not use)
**	[IN] data : pointer to data to be written
**	[IN] len : data size in bytes
**	[IN][OUT] keybuf : pointer to message/storage location for message key
** RETURN :
**	>=0 : write message bytes
**	SQE_DATA_TOOBIG
**	SQE_QUE_LOCK
**	SQE_QUE_FULL
**	SQE_INVALID_HANDLE
**	SQE_INVALID_PARA
**	SQE_MMAP_CREATE
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SendNormal( unsigned char *ip,int port,char discard,char *data,int len,char *key );

/*****************************************************************************
** FUNCTION : Receive
** DESCRIPTION : read one message from system receive queue (FBrcvqueue.map)
** Arguments :
**	[OUT] data : storage location for data
**	[IN] len : maximum number of characters to read
** RETURN :
**	>=0 : read message bytes
**	SQE_MSG_TOOBIG
**	SQE_QUE_LOCK
**	SQE_INVALID_HANDLE
**	SQE_INVALID_PARA
**	SQE_MMAP_CREATE
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT Receive( char *data,int len );

/*****************************************************************************
** FUNCTION : ReceiveErrLog
** FUNCTION : SQReceiveErrLog
** DESCRIPTION : read one message from system error log queue (FBelogqueue.map)
** Arguments :
**	[OUT] data : storage location for error data
**	[IN] len : maximum number of characters to read
**
**	[OUT] piErrCode : storage location for error code of sendnormal 
** RETURN :
**	>=0 : read message bytes
**	SQE_MSG_TOOBIG
**	SQE_QUE_LOCK
**	SQE_INVALID_HANDLE
**	SQE_INVALID_PARA
**	SQE_MMAP_CREATE
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT ReceiveErrLog( char *data, int len );
NoMangle int DLL_IMPORT_EXPORT SQReceiveErrLog( char *data, int len,
         int *piErrCode );
NoMangle int DLL_IMPORT_EXPORT SQReadSendNormalErr
         (char *data, int len, int *piErrCode, char *pIPBuf, int iIPBufSize,
          unsigned short *pushPort);

/*****************************************************************************
** FUNCTION : SQSetErrorCodeVer
** DESCRIPTION : set error code version
** Arguments :
**	[IN] pVer : pointer to version string
**	            "2.00" (default) means use old error code (only -91xx)
**	            "2.06" means use new error code (-91xx for local error and
**                         -81xx for remote error)
** RETURN :
**	=0 : read message bytes
**	SQE_INVALID_PARA
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQSetErrorCodeVer( char *pVer );

/*****************************************************************************
** FUNCTION : SQEchoTest
** DESCRIPTION : connection test
** Arguments :
**	[IN] ip : remote host IP, "xx.xx.xx.xx"; "0.0.0.0" or NULL for local
**	[IN] port : remote SysQue system listen port
** RETURN :
**	=0 : ok
**	SQE_SYSQUE_NOTRUN
**	SQE_PROG_DOWN
**	SQE_CONN_FAIL
**	SQE_CONN_TIMEOUT
**	SQE_TRANSQUE_OPEN
**	SQE_HOST_NOTFIND
**	SQE_RUN_CHILD
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQEchoTest( char *pIP, int iPort );

/*****************************************************************************
** FUNCTION : SQSetDefSQITimeout
** DESCRIPTION : set default SQI timeout value
** Arguments :
**	[IN] iTimeout : timeout value
** RETURN :
**	=0 : ok
**	SQE_INVALID_PARA
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQSetDefSQITimeout(int iTimeout);

/*****************************************************************************
** FUNCTION : SQGetDefSQITimeout
** DESCRIPTION : set default SQI timeout value
** Arguments :
**	none
** RETURN :
**	>0 : default timeout value
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQGetDefSQITimeout();


/*****************************************************************************
** FUNCTION : SQSend
** DESCRIPTION : write one message to message queue with asynchronous method
** Arguments :
**	[IN] sqhandle : message queue handle
**	[IN] data : pointer to data to be written
**	[IN] len : data size in bytes
**	[IN][OUT] keybuf : pointer to message/storage location for message key
** RETURN :
**	>=0 : write message bytes
**	SQE_DATA_TOOBIG
**	SQE_QUE_LOCK
**	SQE_QUE_FULL
**	SQE_INVALID_HANDLE
**	SQE_INVALID_PARA
**	SQE_NO_PERMISSION
**	SQE_MMAP_CREATE
**	SQE_PROG_DOWN
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQSend( int sqhandle, char *data, int len, char *key );


/*****************************************************************************
** FUNCTION : SQGetSendErr
** DESCRIPTION : get error result of SQSend()
** Arguments :
**	[IN] sqhandle : message queue handle
**	[OUT] piErrCode : storage location for error code of sendnormal 
**	[OUT] data : storage location for error data
**	[IN] len : maximum number of characters to read
**
** RETURN :
**	>=0 : read message bytes
**	SQE_MSG_TOOBIG
**	SQE_QUE_LOCK
**	SQE_INVALID_HANDLE
**	SQE_INVALID_PARA
**	SQE_MMAP_CREATE
******************************************************************************/
NoMangle int DLL_IMPORT_EXPORT SQGetSendErr( int sqhandle, int *piErrCode, char *data, int iBufSize );


#endif
#endif
