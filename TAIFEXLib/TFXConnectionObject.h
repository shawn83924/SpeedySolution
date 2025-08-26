#ifndef __CONNECTION_OBJECT_H
#define __CONNECTION_OBJECT_H
//-----------------------------------------------------------------------------------------------------------------------
#include "TFXConnection.h"
//-----------------------------------------------------------------------------------------
namespace TFX
{
//-----------------------------------------------------------------------------------------------------------------------
#define COMMAND_OPEN_MARKET    0
#define COMMAND_CLOSE_MARKET   1
#define COMMAND_UNLOCK_PVC     2
#define COMMAND_GET_STATE      3
#define COMMAND_GET_BANDWIDTH 20
#define COMMAND_LOCK_PVC       4
#define EVENT_PVC_READ         5
#define EVENT_PVC_WRITE        6
#define EVENT_TMP_NOTIFY       9
#define EVENT_TMP_EXECUTION   10
#define EVENT_TMP_GET_SEQ     11
#define EVENT_TMP_GET_MSG     12
#define EVENT_TMP_RECOVER_MSG 13
#define EVENT_TMP_SET_TIME    14
#define EVENT_TMP_RECONNECT   99
//-----------------------------------------------------------------------------------------------------------------------
#define CONNECTION_OBJECT_NORMAL_CLOSE   0
#define CONNECTION_OBJECT_X25ERROR      -1
#define CONNECTION_OBJECT_UNKNOWN_ERROR -2
#define CONNECTION_ORDER_REPORT         -3
#define CONNECTION_PVC_LOCKED           -4
#define CONNECTION_OBJECT_PVC_READY     -5
#define CONNECTION_OBJECT_PVC_BROKEN    -6
#define CONNECTION_CONFIRM_FINASHED     -7
#define CONNECTION_PVC_READ_WRITE       -8
#define CONNECTION_PVC_DISABLED         -14
#define CONNECTION_PVC_ENABLED          -15
#define CONNECTION_PVC_UPDATE_STATE     -16
#define CONNECTION_PVC_SLEEPING         -17
#define CONNECTION_PVC_WAIT_OPEN        -18
#define CONNECTION_PVC_TRY_OPEN         -19
//-----------------------------------------------------------------------------------------------------------------------
#define CONNECTION_TMP_MESSAGE_ARRIVE   -50
#define CONNECTION_TMP_SESSION_SEQ      -51
#define CONNECTION_TMP_RECOVER_MSG      -52
//-----------------------------------------------------------------------------------------------------------------------
#define CONNECTION_CCH_UPDATE_SEQ       -10
#define CONNECTION_CCH_LAST_MSG         -11
//-----------------------------------------------------------------------------------------------------------------------
#define CONNECTION_RECV_MSG_F050	-12
#define CONNECTION_RECV_MSG_F080	-13
//-----------------------------------------------------------------------------------------------------------------------
#define CONNECTION_IDLE         	-20
//-----------------------------------------------------------------------------------------------------------------------
#define FTP_STATUS_REPORT         	-30
//-----------------------------------------------------------------------------------------------------------------------
class ConnectionObjectBase;
//-----------------------------------------------------------------------------------------------------------------------
typedef struct
{
	int  OrderCount;
	long UseMS;

}TOrderReport;
//-----------------------------------------------------------------------------------------------------------------------
class ConnectionListener
{
public:
	virtual void OnConnectionNotify( ConnectionObjectBase* CObj, int Reason, void* Data = NULL ) = 0;
        virtual ~ConnectionListener( void ){}
};
//-----------------------------------------------------------------------------------------------------------------------
class ConnectionObjectBase
{
protected:
    ConnectionParameter  FParameters;
public:
    ConnectionObjectBase( ConnectionParameter& info ):FParameters( info ){}
    ConnectionParameter* GetConnectionParameter( void ){ return &FParameters; }
    virtual ~ConnectionObjectBase(){}
    virtual void Listen( BOOL /*IsListen*/  ) {}
    virtual void Enable( BOOL /*IsEnabled*/, BOOL COD ) {}
    virtual void SwitchBackup( int /*Backup index*/ ) = 0;
    virtual int  GetBackupIndex( void ) = 0;
    virtual void Active( void ) = 0;
    virtual void Close( void ) = 0;
    virtual void OnCommand( int Command, void* Data ) = 0;
    virtual void UpdateConnectionParameter( ConnectionParameter& NewParam ) = 0;
    virtual BOOL Accept( int /*FD*/ ) { return FALSE; }
    virtual BOOL IsReady( void ) = 0;
    virtual BOOL IsOffHour( void ) = 0;
};
//----------------------------------------------------------------------------------------------------------------------
const int                     SwitchTimes = 5; 
//-----------------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------------------------------
