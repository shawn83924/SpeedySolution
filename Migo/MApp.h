#ifndef _MApp_h
#define _MApp_h
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "MDefinitions.h"
#include "MTree.h"
#include "MComponent.h"
#include "SubjectDB.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define EXECUTABLE_WIN32 "MBus.exe"
#define EXECUTABLE_UNIX  "MBus"
#define INTALL_PATH_ENV  "MLIB_PATH"
#define MAPP_LOCAL_IP    "127.0.0.1"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MSubscriber;
class MApp;
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
    psStopped = 0,
    psRunning = 1,
    psNotMonitoring = 2

} ProcessState;
//----------------------------------------------------------------------------------------------------------------------
class ProcessStateObj
{
private:
    UFC::AnsiString FHost;
	UFC::AnsiString FAppName;
    BOOL            FIsAlive;
    BOOL            FReply;
public:
    ProcessStateObj( const UFC::AnsiString& Host, const UFC::AnsiString& AppName )
    :FHost( Host ),FAppName( AppName ),FIsAlive( FALSE ),FReply( FALSE ){}
    BOOL                    IsAlive( void )    { return FIsAlive; }
    BOOL                    IsReplied( void )  { return FReply; }
    void                    Alive( BOOL Val )  { FIsAlive = Val; }
    void                    Replied( BOOL Val ){ FReply   = Val; }
    const UFC::AnsiString&  GetHost( void )    { return FHost; }
    const UFC::AnsiString&  GetAppName( void ) { return FAppName; }
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
    meSendConnectedAckFailed,
    meUnregistFailed,
    meReadDataFailed,
    meSendFailed,
    meSendFailedNotConnected,
    meConnectTimeout

}PMAppError;
//----------------------------------------------------------------------------------------------------------------------
// Class MAppListener
// Interface to handle MApp events.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MAppListener
{
public:
#ifndef _WIN32
    virtual ~MAppListener( void ) { }
#endif
    virtual void OnMAppConnected() = 0;
    virtual void OnMAppError( PMAppError Error ) = 0;
    virtual void OnMAppDisconnected() = 0;
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MTHandle
{
public:
    UFC::MemoryStream FWriteStream;
    UFCType::Int32    FNodeCount;
    UFCType::Int64    FMTreePos;
public:
    MTHandle()
    :FWriteStream( 1024 )
    ,FNodeCount(0)
    ,FMTreePos(0)
    {}
    char*          GetMTreeBuffer() { return ((char*)FWriteStream.GetBuffer()) + (int)FMTreePos ;}
    UFCType::Int64 GetMTreeSize()   { return FWriteStream.GetSize() - FMTreePos;}      
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef UFC::PtrList<MSubscriber> PSubscriberList;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MApp : public UFC::SocketClientListener
{
public:
    static int MSG_QUEUE_SIZE;
    static int MAPP_DEF_PORT;
    static void SetQueueSize( UFC::Int32 size ) { MSG_QUEUE_SIZE = size; }
public:
    class MAppProperties
    {
	private:
            MString m_rsAppName;
            MString m_rsAppVersion;
            MString m_rsAppInfo;
            MString m_rsConfigURL;
            long    m_iArgc;
            char**  m_aszArgv ;
            MString m_rsAdvSubject;
	public:
            MAppProperties(){}
            ~MAppProperties(){}
            void setAppName(const MString& rsAppName) { m_rsAppName = rsAppName; }
            void setAppVersion(const MString& rsAppVersion){ m_rsAppVersion = rsAppVersion; }
            void setAppInfo(const MString& rsAppInfo) { m_rsAppInfo = rsAppInfo; }
            void setConfigURL(const MString& rsConfigURL) { m_rsConfigURL = rsConfigURL; }
            void setCommandLine( long iArgc, char** aszArgv ) { m_iArgc = iArgc; m_aszArgv = aszArgv; }
            const MString& getAppName() const { return m_rsAppName; }
            const MString& getAppVersion() const { return m_rsAppVersion; }

    }; // class MAppProperties
protected:
    UFC::PClientSocket*	            FSockClient;
    UFC::PCriticalSection           FSendCS;
    UFC::PCriticalSection           FRegCS;
    UFC::PCriticalSection           FSubscriberListCS;
    UFC::PCriticalSection           FLedgerCS;
    UFC::PtrQueue<DataMessage>*     FMsgQueue;
    UFC::PEvent                     FMsgArrived;
    UFC::PEvent                     FStopEvent;    
    CSubjectDB<PSubscriberList>     FSubSubjectDB; ///< Use subject and key to find Subscriber.
    RegistMessage*                  FRegSubjects;  ///< Keep all regist information into a register.
    RegistMessage*                  FBatchRegSubjects; 
    BOOL                            FHandshake;
    BOOL                            FIsRunning;
    BOOL                            FNeedReconnect;
    BOOL                            FBatchAdding;
    MAppListener*                   FListener;
    MString                         FAppName;
    MString                         FLegFileName;
    int                             FRecvTimer;
    int                             FSendTimer;
    int                             FHEARTBEAT_TIMEOUT;
    int                             FHEARTBEAT_TIME;
    int                             FDelayRecv;
    UFC::UInt64			    FAcuRecvBytes;
    UFC::UInt64			    FAcuRecvMsgs;
    BOOL                            FSelfDisconnect;
    int                             FReconnectSec;    
protected:
    UFCType::Int32        FMAppPort;
    MString               FMAppAddress;
    MString               FLocalIPAddress;
protected:
    virtual void OnIdle( UFC::PClientSocket* Socket ){};
    virtual void OnConnect( UFC::PClientSocket* Socket );
    virtual void OnDisconnect( UFC::PClientSocket* Socket, BOOL NeedReconnect = FALSE ); // modify by joe
    virtual BOOL OnDataArrived( UFC::PClientSocket* Socket );
protected:
    void LockAndSend( MigoHeader* Header );    
    void RemoveRegRecord( MComponent* pComponent );
    void StartServer( void );
    void ReceiveHandshake( MigoHeader* Header );
    void RegistAll( void );
    void Regist( const MString& SubjectName, const MString& SubjectKey );
    void SendHeartbeat( void );
    void SendHeartbeatConfirm( void );
    BOOL CheckHeartBeat( int IncSec );
protected:
    virtual void TriggerOnMessage( DataMessage* MsgHeader );
    virtual void TriggerOnConnected( void );
    virtual void TriggerOnError( PMAppError Error );
    virtual void TriggerOnDisconnected( void );
public:
    void BeginSend( MTHandle& Handle, const UFC::AnsiString& Subject, const UFC::AnsiString& Key );
    void WriteString( MTHandle& Handle, const UFC::AnsiString& Name, const UFC::AnsiString& Value );
    void WriteZipString( MTHandle& Handle, const UFC::AnsiString& Name, const UFC::AnsiString& Value );
    void WriteInt32( MTHandle& Handle, const UFC::AnsiString& Name, UFCType::Int32 Value );
    void WriteInt64( MTHandle& Handle, const UFC::AnsiString& Name, UFCType::Int64 Value );    
    void EndWrite( MTHandle& Handle );
    BOOL EndSend( MTHandle& Handle );
public:
    MApp( const MAppProperties* pAppProperties, UFCType::Int32 Port = MAPP_DEF_PORT, UFCType::Int32 QueueSize = MSG_QUEUE_SIZE );
    MApp( UFCType::Int32 Port = MAPP_DEF_PORT, UFCType::Int32 QueueSize = MSG_QUEUE_SIZE );
    virtual    ~MApp( void );
    void        start( BOOL AutoConnect );
    void        stop( void );
    void        ConnectOnStart( bool Yes );
    void        SendSelf( const MString& SubjectName, const MString& SubjectKey, MTree& refMTree );
    BOOL        send_by_key( const MString& SubjectName, const MString& SubjectKey, MTree& refMTree, BOOL KeepUnsent );
    BOOL     	send_direct( UFC::MemoryStream* WriteStream );
    BOOL        WaitForConnected( UFCType::Int32 Sec );
public: ///< Functions to add/remove subscribers
    void              BeginAddSubscriber( void );
    void              EndAddSubscriber( void );
    UFCType::Int32    AddSubscriber( MSubscriber* pComponent );
    void              DeleteSubscriber( MSubscriber* pComponent );
    PSubscriberList*  FindExactSubscriber( const MString& SubjectName, const MString& SubjectKey );
    PSubscriberList*  FindSubscriber( const MString& SubjectName, const MString& SubjectKey  );
    void              SetRecvDelayMS( int DelayMS ) { FDelayRecv = DelayMS; }
public: ///< Functions to manipulate MBus connection.
    BOOL         Connect( UFCType::Int32 Timeout = 10, UFCType::Int32 WriteTimeoutms = 8000 );
    void         Disconnect( void );
    BOOL         IsConnected( void );
    void         SetListener( MAppListener* Listener ){ FListener = Listener; }
    void         SetAppName( const MString& NewName );
    void         SetPort( int Port )            { FMAppPort    = Port; }
    void         SetHost( const MString& Host ) { FMAppAddress = Host; }
    int          GetPort( void )                { return FMAppPort; }
    MString&     GetHost( void )                { return FMAppAddress; }
    MString&     GetAppName( void )             { return FAppName; }
    MString&	 GetLocalIPAddress( void )	{ return FLocalIPAddress; }
    int     	 GetSocketHandle( void )	{ return FSockClient->GetHandle(); }
    UFC::UInt64	 GetAcuRecvBytes( void )	{ return FAcuRecvBytes; }
    UFC::UInt64	 GetAcuRecvMsgs( void )	        { return FAcuRecvMsgs; }
    UFC::Int32   GetQueueSize( void )           { return FMsgQueue->Size(); }
}; // MApp
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif // MApp_h
