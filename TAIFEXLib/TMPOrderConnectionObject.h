#ifndef __TMP_ORDER_CONNECTION_OBJECT_H
#define __TMP_ORDER_CONNECTION_OBJECT_H
//-----------------------------------------------------------------------------------------------------------------------
#include "ConnectionObject.h"
#include "TMPMessage.h"
#include "TMPTransactionObject.h"
#include "../TMP34/ExecutionContainer.h"
//-----------------------------------------------------------------------------------------------------------------------
class LinkSubSystemTAIFEX;
class TMPOrderConnectionObject;
//-----------------------------------------------------------------------------------------------------------------------
//   TAIFEX Side: Connection Object for AP code 4 ( Order )
//-----------------------------------------------------------------------------------------------------------------------
class ThroughputCum
{
private:    
    Int32* FThroughputs;
    Int32  FPos;
    Int32  FCount;
public:    
    ThroughputCum( Int32 Sec );
    ~ThroughputCum( void );
    void  Clear( void );
    void  PutThroughput( Int32 Orders );
    Int32 GetCumThroughput( void );
};
//-----------------------------------------------------------------------------------------------------------------------
class SendingThread : public  UFC::PThread
{
private:
    TMPOrderConnectionObject* FOdrConnection;
    ExecutionCotainer         FExecutions;
    BOOL                      IsRunning;
    BOOL                      FCompactMode;
    BOOL                      FMessagesPerSend;
    Int32                     FSessionPrefixIndex;
public:
    SendingThread( TMPOrderConnectionObject* Connection );
    BOOL  AddExecution( odm_Rpt* Exec, ExecutionSource Src );
    void  Execute( void );
    void  StartAt( Int32 StartPos, Int32 Throuthput, BOOL CompactExec, Int32 MessagesPerSend );
    void  SetSessionPrefixIndex( Int32 Index );
};
//-----------------------------------------------------------------------------------------------------------------------
class TMPOrderConnectionObject : public ConnectionObjectBase, public UFC::PThread
{
private:
	BOOL                 FNeedLock;
	BOOL                 FNeedReset;
	BOOL                 FConnected;
	BOOL                 FBackup;
        Int32                FSendHeartbeat;
        Int32                FRecvHeartbeat;
	TransactionObject*   FTransactionObj;
	ConnectionListener*  FConnectionListener;
	LinkSubSystemTAIFEX* FConnection;
        LogManager*          FLog;
        UInt32               FTick;
        UInt32               FThroughputCount;
        UInt32               FThroughputCountPrev;
        double               FThroughput;
        BOOL                 FCompactMode;
        Int32                FMessagesPerSend;
private:// Throughput control        
        Int32                FWarningThroughput;
        Int32                FAlarmThroughput;
        Int32                FDisconnectThroughput;
        Int32                FMaxCumThroughput;
        Int32                FThroughputOverCount;
        ThroughputCum        FThroughputCum;       
        SendingThread        FSendingThread;
        UInt32               FLastR12Tick;
        Int32                FSessionPrefixIndex;
        Int32                FReconnectTimes;
private:// Message list for offline R14
        UFC::List< TMP::TMPMessage*> FNotifies;
private:       
	TMP::TMPMessage* CheckReceiveMessage( void );    
	void             MessageLoop( void );
	void             LockLoop( void );
        void             WaitForConnected( void );        
	void             Execute( void );
	void             ConfirmsubSystemHandshake( BOOL ReceivedL010 = FALSE, UInt8 StatusCode = 0 );
        UInt8            CalculateThroughput( void );
        void             HandleThroughputExeedException( const UFC::AnsiString& Reason );
        void             ReplyHeartbeat( TMP::TMPMessage* InMsg, UInt8 ThroughputWarning );
        void             CheckExecutionDelay( TMP::TMPMessage* Report );
public:
	TMPOrderConnectionObject(  ConnectionParameter& info,
				   TransactionObject*   TObj,
				   ConnectionListener*  Listener,
				   UFC::ThreadListener*      TListener );
	virtual ~TMPOrderConnectionObject();
        void   SetLogManager( LogManager* LogObj );
        void   SendExecutions( UFC::List< TMP::TMPMessage*>& Reports );
        void   SendExecution( TMP::TMPMessage* Report, BOOL KeepIfBroken = FALSE );
        BOOL   AddExecution( odm_Rpt* Exec, ExecutionSource Src ) { return FSendingThread.AddExecution( Exec, Src ); }
        double GetThroughput( void ) { return FThroughput; }
        BOOL   IsCompactExec( void ) { return FCompactMode; }
        BOOL   CanProcessR12( void );
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
