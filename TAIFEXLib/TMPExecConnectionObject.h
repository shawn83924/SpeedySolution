#ifndef __TMP_EXEC_CONNECTION_OBJECT_H
#define __TMP_EXEC_CONNECTION_OBJECT_H
//-----------------------------------------------------------------------------------------------------------------------
#include "ConnectionObject.h"
#include "TMPMessage.h"
//-----------------------------------------------------------------------------------------------------------------------
class LinkSubSystemTAIFEX;
//-----------------------------------------------------------------------------------------------------------------------
class TMPExecConnectionObject : public ConnectionObjectBase, public UFC::PThread
{
private:
	BOOL                 FNeedLock;
	BOOL                 FNeedReset;
	BOOL                 FConnected;
	BOOL                 FBackup;
        Int32                FSendHeartbeat;
        Int32                FRecvHeartbeat;
	ConnectionListener*  FConnectionListener;
	LinkSubSystemTAIFEX* FConnection;
        LogManager*          FLog;
        UFC::AnsiString      FReportFileName;
        Int32                FBeginSeq;
        UFC::FileStream*     FReportFile;
        BOOL                 FIsTMP89Connection;
        Int32                FSessionPrefixIndex;
        Int32                FReconnectTimes;
private:       
	void             MessageLoop( void );
	void             LockLoop( void );
        void             WaitForConnected( void );        
	void             Execute( void );
	void             ConfirmsubSystemHandshake( BOOL ReceivedL010 = FALSE, UInt8 StatusCode = 0 );
        void             InitFileName( UInt16 CMIndex, UInt8 TargetID );
        void             CMInitialiseHandshake( void );
        Int32            GetReportCount( void );
        void             OpenReportFile( void );
        TMP::TMPMessage* CheckExecution( void );
public:
	TMPExecConnectionObject(  ConnectionParameter& info,
				  ConnectionListener*  Listener,
				  UFC::ThreadListener* TListener );
	virtual ~TMPExecConnectionObject( void );
        void SetLogManager( LogManager* LogObj );
        void SendExecution( TMP::TMPMessage* Report );
public:
	virtual void Close( void );
        virtual void Active( void )  { UFC::PThread::Start(); }
        virtual BOOL IsReady( void );
	virtual void OnCommand( int Command, void* Data );
	virtual void UpdateConnectionParameter( ConnectionParameter& );
        virtual BOOL Accept( int FD );                
};
//-----------------------------------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------------------------------
