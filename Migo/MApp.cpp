#include "MApp.h"
#include "MSubscriber.h"
#include "MigoHeader.h"
#include "../UFC/UFC.h"
#include "../UFC/zlibutility.h"
//------------------------------------------------------------------------------
int MApp::MSG_QUEUE_SIZE = 8192;
int MApp::MAPP_DEF_PORT  = 12345;
//------------------------------------------------------------------------------
//
// Implement Public methods.
//
//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------
MApp::MApp( UFCType::Int32 Port, UFCType::Int32 QSize )
:FSockClient( NULL )
,FRegSubjects( new RegistMessage( 0 ))
,FBatchRegSubjects( new RegistMessage( 0 ))
,FHandshake( FALSE )
,FIsRunning( FALSE )
,FNeedReconnect( TRUE )
,FBatchAdding( FALSE )
,FListener( NULL )
,FRecvTimer( 0 )
,FSendTimer( 0 )
,FHEARTBEAT_TIMEOUT( 45 )
,FHEARTBEAT_TIME( 40 )
,FDelayRecv( 0 )
,FAcuRecvBytes( 0 )
,FAcuRecvMsgs( 0 )
,FSelfDisconnect( TRUE )
,FReconnectSec( 9 )
,FMAppPort( Port )
,FMAppAddress( MAPP_LOCAL_IP )
,FLocalIPAddress( MAPP_LOCAL_IP )
{
    StartServer();
	FMsgQueue   = new UFC::PtrQueue<DataMessage>( QSize );
    FSockClient = new UFC::PClientSocket( FMAppAddress, FMAppPort );
    FSockClient->SetListener( this );
}
//------------------------------------------------------------------------------
MApp::MApp( const MAppProperties* pAppProperties, UFCType::Int32 Port, UFCType::Int32 QSize  )
:FSockClient( NULL )
//,FLedgerFile( NULL )
,FRegSubjects( new RegistMessage( 0 ))
,FBatchRegSubjects( new RegistMessage( 0 ))
,FHandshake( FALSE )
,FIsRunning( FALSE )
,FNeedReconnect( TRUE )
,FBatchAdding( FALSE )
,FListener( NULL )
,FAppName( pAppProperties->getAppName() )
,FRecvTimer( 0 )
,FSendTimer( 0 )
,FHEARTBEAT_TIMEOUT( 45 )
,FHEARTBEAT_TIME( 40 )
,FDelayRecv( 0 )
,FAcuRecvBytes( 0 )
,FAcuRecvMsgs( 0 )
,FSelfDisconnect( TRUE )
,FReconnectSec( 9 )
,FMAppPort( Port )
,FMAppAddress( MAPP_LOCAL_IP )
{
    StartServer();
    FMsgQueue   = new UFC::PtrQueue<DataMessage>( QSize );
    FSockClient = new UFC::PClientSocket( FMAppAddress, FMAppPort );
    FSockClient->SetListener( this );
    FLegFileName = UFC::GetCurrentDir( ) + "/" + FAppName + ".que";
}
//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
MApp::~MApp( void )
{
    if( FIsRunning == TRUE )
		stop();
    delete FRegSubjects;
    delete FBatchRegSubjects;
    delete FMsgQueue;

    if( FSockClient != NULL )
    {
        delete FSockClient;
        FSockClient = NULL;
    }
}
//------------------------------------------------------------------------------
BOOL MApp::CheckHeartBeat( int IncSec )
{    
    if( FSockClient->IsConnect() == TRUE )
    {	        
        if( FRecvTimer < FHEARTBEAT_TIME && FRecvTimer + IncSec >= FHEARTBEAT_TIME ) ///< Read time is up, trigger heartbeat.
        {
            if( FAppName.Length() > 0 )
                UFC::BufferedLog::Printf( " %s::%s() Send Heartbeat. Read timer[%d sec(%d + %d)], Heartbeat time[%d]", FAppName.c_str(), __func__, FRecvTimer + IncSec, FRecvTimer, IncSec, FHEARTBEAT_TIME );
            else
                UFC::BufferedLog::Printf( " Send Heartbeat. Read timer[%d sec]", FRecvTimer + IncSec );                
            FRecvTimer = FHEARTBEAT_TIME;
            SendHeartbeat();
            return TRUE;
        }
        else if( FSendTimer < FHEARTBEAT_TIME && FSendTimer + IncSec >= FHEARTBEAT_TIME ) ///< Write time is up, trigger heartbeat.
        {
            if( FAppName.Length() > 0 )
                UFC::BufferedLog::Printf( " %s::%s() Send Heartbeat. Send timer[%d sec(%d + %d)], Heartbeat time[%d]", FAppName.c_str(), __func__, FSendTimer + IncSec, FSendTimer, IncSec, FHEARTBEAT_TIME );
            else                
                UFC::BufferedLog::Printf( " Send Heartbeat. Write timer[%d sec]", FSendTimer + IncSec );
            FRecvTimer += IncSec;
            SendHeartbeat();
            return TRUE;
        }
        else if( FRecvTimer + IncSec >= FHEARTBEAT_TIMEOUT ) ///< Read time out, Close connection.
        {
            if( FAppName.Length() > 0 )
                UFC::BufferedLog::Printf( " %s::%s() Read Heartbeat time out[%d sec(%d + %d)] >= Heartbeat timeout[%d], close the MBus connection.",
                                          FAppName.c_str(), __func__, FRecvTimer + IncSec, FRecvTimer, IncSec, FHEARTBEAT_TIMEOUT );
            else
                UFC::BufferedLog::Printf( " Read Heartbeat time out[%d sec], close the MBus connection.", FRecvTimer + IncSec  );
            FRecvTimer = 0;
	    FSendTimer = 0;
            FSockClient->Disconnect( TRUE );///<  TRUE for trigger close event.
            FNeedReconnect = TRUE;
            FHandshake     = FALSE;
        }
        else if( FSendTimer + IncSec >= FHEARTBEAT_TIMEOUT ) ///< Write time out, Close connection.
        {
            if( FAppName.Length() > 0 )
                UFC::BufferedLog::Printf( " %s::%s() Write Heartbeat time out[%d sec(%d + %d)] >= Heartbeat timeout[%d], close the MBus connection.",
                                          FAppName.c_str(), __func__, FSendTimer + IncSec, FSendTimer, IncSec, FHEARTBEAT_TIMEOUT );
            else
                UFC::BufferedLog::Printf( " Write Heartbeat time out[%d sec], close the MBus connection.", FSendTimer + IncSec  );
            FRecvTimer = 0;
            FSendTimer = 0;
            FSockClient->Disconnect( TRUE );///<  TRUE for trigger close event.
            FNeedReconnect = TRUE;
            FHandshake     = FALSE;
        }
        else  ///< Increase Read/Write counter.
        {
            FRecvTimer += IncSec;
            FSendTimer += IncSec;
//            if( FAppName.Length() > 0 )
//                UFC::BufferedLog::Printf( " %s::%s() Receive Timer[%d sec], Send Timer[%d sec]", FAppName.c_str(), __func__, FRecvTimer, FSendTimer );
//            else
//                UFC::BufferedLog::Printf( " Receive Timer[%d sec], Send Timer[%d sec]", FRecvTimer, FSendTimer );
        }
    }
    return FALSE;
}
//------------------------------------------------------------------------------
void MApp::ConnectOnStart( bool Yes )
{
    FSelfDisconnect = !Yes;
    FNeedReconnect = Yes;
}
//------------------------------------------------------------------------------
// Start the message service.
//------------------------------------------------------------------------------
void MApp::start( BOOL AutoReconnect )
{
    DataMessage* Msg;
    UInt32 BeginTick, UseMS;
    
    FIsRunning = TRUE;
	UFC::BufferedLog::Printf( " %s::%s() start...", FAppName.c_str(), __func__ );
	int loopCount = 0;
	while( FIsRunning == TRUE )
	{
		loopCount++;
		if( AutoReconnect   == TRUE  &&  ///< Auto reconnect
			FSelfDisconnect == FALSE &&  ///< Not disconnect by caller
			FNeedReconnect  == TRUE  )   ///< Socket was broken
		{
			if( Connect( ) == TRUE )
				FNeedReconnect = FALSE;
			else
				UFC::SleepMS( FReconnectSec * 1000 );
		}
		BeginTick = UFC::GetTickCountMS();
		///< Deque message from local queue
		if( ( Msg = FMsgQueue->Dequeue( 1 ) ) != NULL )
		{   ///< Process until no message in queue.
			do
			{
				TriggerOnMessage( Msg );
				///< Process Messages use more than 1000ms, increase heartbeat countor.

				UInt32 afterTick = UFC::GetTickCountMS();
				UseMS = afterTick - BeginTick;
				if( UseMS > 1000 )
				{
					CheckHeartBeat( (int)UseMS / 1000 );
					BeginTick = UFC::GetTickCountMS();
				}
			} while( (Msg = FMsgQueue->DequeueNoWait( ) ) != NULL );
		}
		else ///< No Message, increase heartbeat countor.
		{
			UInt32 afterTick = UFC::GetTickCountMS();
			UseMS = afterTick - BeginTick;
			if( ( loopCount % 600 ) == 0 )
				UFC::BufferedLog::Printf( " %s::%s() %010d UseMS[%u] = afterTick[%u] - BeginTick[%u]", FAppName.c_str(), __func__, loopCount, UseMS, afterTick, BeginTick );
			if( UseMS > 1000  )
				CheckHeartBeat( (int)UseMS / 1000 );
			else
				CheckHeartBeat( 1 );
		}
	}
	///< Empty all messages
	while( ( Msg = FMsgQueue->DequeueNoWait() ) != NULL )
	{
		delete Msg;
	}
}
//------------------------------------------------------------------------------
// Stop the message service.
//------------------------------------------------------------------------------
void MApp::stop()
{
    if( FSockClient->IsConnect() == TRUE )
        FSockClient->Disconnect( TRUE );
    FIsRunning     = FALSE;
    FNeedReconnect = TRUE;
    FHandshake     = FALSE;
}
//------------------------------------------------------------------------------
// Set the application name
//------------------------------------------------------------------------------
void MApp::SetAppName(  const MString& NewName )
{
    FAppName = NewName;
    FLegFileName.Printf( "%s/%s.que", UFC::GetCurrentDir( ).c_str(), FAppName.c_str() );
}
//------------------------------------------------------------------------------
// Serialize the message to the socket.
// and sync the I/O use Critical Section
//------------------------------------------------------------------------------
inline void MApp::LockAndSend( MigoHeader* Header  )
{
    FSendTimer = 0;
    Header->SerializeToSocket( FSockClient );
}
//------------------------------------------------------------------------------
//  Send a MBus Message.
//------------------------------------------------------------------------------
BOOL MApp::send_by_key( const MString& SubjectName, const MString& SubjectKey, MTree& refMTree,BOOL KeepUnsent )
{
    if( FSockClient->IsConnect() != FALSE && FHandshake == TRUE )
    {
        try
        {
            if( SubjectName == ADMIN_SUBJECT )
            {
                AdminMessage AdminMsg( 0, SubjectKey, refMTree );
                LockAndSend( &AdminMsg );
            }
            else
                DataMessage::MessageToSocket( 0, SubjectName, SubjectKey, refMTree, FSockClient );
            return TRUE;
        }
        catch( UFC::SocketException & )
        {
            if( FAppName.Length() > 0 )
                UFC::BufferedLog::Printf( " App[%s] Send message failed, try to reconnect server.", FAppName.c_str() );
            else
                UFC::BufferedLog::Printf( " Send message failed, try to reconnect server." );
            FNeedReconnect = TRUE;
            FHandshake     = FALSE;
            FSockClient->Disconnect( FALSE );
            TriggerOnError( meSendFailed );
            return FALSE;
        }
    }
    else
    {
        FNeedReconnect = TRUE;
        FHandshake     = FALSE;
        TriggerOnError( meSendFailedNotConnected );
        return FALSE;
    }
}
//------------------------------------------------------------------------------
BOOL MApp::send_direct( UFC::MemoryStream* WriteStream )
{
    if( FSockClient->IsConnect() != FALSE && FHandshake == TRUE )
    {
        try
        {
            UFC::PLockObject Lock( FSockClient->IOLock );

            FSockClient->BlockSend( (char*)WriteStream->GetBuffer(), WriteStream->GetSize() );
            return TRUE;
        }
        catch( UFC::SocketException & )
        {
            if( FAppName.Length() > 0 )
                UFC::BufferedLog::Printf( " App[%s] Send message failed, try to reconnect server.", FAppName.c_str());
            else
                UFC::BufferedLog::Printf( " Send message failed, try to reconnect server.");
            FNeedReconnect = TRUE;
            FHandshake     = FALSE;
            FSockClient->Disconnect( FALSE );
            TriggerOnError( meSendFailed );
            return FALSE;
        }
    }
    else
    {
        FNeedReconnect = TRUE;
        FHandshake     = FALSE;
        TriggerOnError( meSendFailedNotConnected );
        return FALSE;
    }
}
//------------------------------------------------------------------------------
void MApp::SendSelf( const MString& SubjectName, const MString& SubjectKey, MTree& refMTree )
{
    DataMessage* MsgHeader = new DataMessage( 0,SubjectName, SubjectKey, refMTree );
    TriggerOnMessage( MsgHeader );
}
//------------------------------------------------------------------------------
// Wait for connect to MBus server.
//------------------------------------------------------------------------------
BOOL MApp::WaitForConnected( UFCType::Int32 Sec )
{
    int CheckTime = 20 * Sec; ///< check connection 20 times per secound.(every 50 ms)
    
    for( int i = 0; i < CheckTime; i++ )
    {
        UFC::SleepMS( 50 ); 
        if( IsConnected() == TRUE )
        {
            UFC::SleepMS( 500 ); ///< Wait for startup handshake.
            return TRUE;///< OK! Connected.
        }
    }
    return FALSE; ///< Connect failed.
}
//------------------------------------------------------------------------------
// Begin Batch add subscribers
//------------------------------------------------------------------------------
void MApp::BeginAddSubscriber( void )
{
    FBatchRegSubjects->Clear();
    FBatchAdding = TRUE;
}
//------------------------------------------------------------------------------
//  Batch add subscribers finished. 
//------------------------------------------------------------------------------
void MApp::EndAddSubscriber( void )
{
    FBatchAdding = FALSE;
    if( FHandshake == TRUE ) ///< Is MApp online ?
        LockAndSend( FBatchRegSubjects ); ///< Send regist message to server.
}
//------------------------------------------------------------------------------
// Add a Subscriber to MApp.
//------------------------------------------------------------------------------
int MApp::AddSubscriber( MSubscriber* NewSubscriber )
{
    if( NewSubscriber != NULL )
    {
        const MString&   Subject     = NewSubscriber->getSubjectName();
        const MString&   SubjectKey  = NewSubscriber->getSubjectKey();
        PSubscriberList* Subscribers;

        NewSubscriber->setMApp( this );
        UFC::PLockObject CSObj( FSubscriberListCS );

        if( (Subscribers = FindExactSubscriber( Subject, SubjectKey )) == NULL )///< New subject/key, create a new subscriber list.
        {
            Subscribers = new PSubscriberList(); ///< Create a new subscriber list.
            FSubSubjectDB.Add( Subject, SubjectKey, Subscribers );
            Regist( Subject, SubjectKey );
        }
        Subscribers->Add( NewSubscriber );        
        return 1;
    }
    return 0;
}
//------------------------------------------------------------------------------
// Delete a Subscriber from MApp.
//------------------------------------------------------------------------------
void MApp::DeleteSubscriber( MSubscriber* Subscriber )
{

    PSubscriberList* Subscribers = FindExactSubscriber( Subscriber->getSubjectName(), Subscriber->getSubjectKey() );

    if( Subscribers != NULL )
    {
        UFC::PLockObject CSObj( FSubscriberListCS );

        if( Subscribers->Remove( Subscriber ) != -1 ) ///< Subcriber exists
        {
            if( Subscribers->ItemCount( ) == 0 )///< No subscriber subscribe this subject.
            {
                UnregistMessage UnregMsg( 0, Subscriber->getSubjectName(), Subscriber->getSubjectKey() );

                FSubSubjectDB.Delete( Subscribers );   ///< Remove the "Subject to Subscriber" map.
                RemoveRegRecord( Subscriber );        ///< Remove the regist record in local list.
                delete Subscribers;
                if( FHandshake == TRUE && FSockClient->IsConnect() == TRUE )
                {
                    try ///< Unregist listener from the server.
                    {
                        LockAndSend( &UnregMsg );
                    }
                    catch( UFC::SocketException & )
                    {
                        if( FAppName.Length() > 0 )
                            UFC::BufferedLog::Printf( " App[%s] Unregister from the server failed.", FAppName.c_str());
                        else
                            UFC::BufferedLog::Printf( " Unregister from the server failed.");
                        TriggerOnError( meUnregistFailed );
                    }
                }
            }
        }
    }
}
//------------------------------------------------------------------------------
// Find a Subscriber from MApp using Subject and Key.
//------------------------------------------------------------------------------
PSubscriberList* MApp::FindSubscriber( const MString& SubjectName, const MString& SubjectKey )
{
    CSubjectRec<PSubscriberList>* Subscribers = FSubSubjectDB.Search( SubjectName, SubjectKey, NULL );

    if( Subscribers != NULL )
        return Subscribers->FObject;
    return NULL;
}
//------------------------------------------------------------------------------
PSubscriberList* MApp::FindExactSubscriber( const MString& SubjectName, const MString& SubjectKey )
{
    CSubjectRec<PSubscriberList>* Subscribers = FSubSubjectDB.ExactSearch( SubjectName, SubjectKey );

    if( Subscribers != NULL )
        return Subscribers->FObject;
    return NULL;
}
//------------------------------------------------------------------------------
//
//  Implement Private functions.
//
//------------------------------------------------------------------------------
// Startup the local Message server.
//------------------------------------------------------------------------------
void MApp::StartServer( void )
{
    MString ExecDir;
    char*   EnvPath = getenv( INTALL_PATH_ENV );

    if( EnvPath == NULL )
        ExecDir = ".";
    else
        ExecDir = MString( EnvPath );
}
//------------------------------------------------------------------------------
// Connect to the local Message Server
//------------------------------------------------------------------------------
BOOL MApp::Connect( UFCType::Int32 Time, UFCType::Int32 WriteTimeoutms )
{
    if( FSockClient->IsConnect() == FALSE /*&& FIsRunning == TRUE*/ ) ///< Try to connect to server until succeed.
    {
        try
        {
            if( FSelfDisconnect == TRUE )
                FSelfDisconnect = FALSE;
            if( FNeedReconnect == TRUE )
                FNeedReconnect = FALSE;
            FHandshake = FALSE;
            FSockClient->Connect( FMAppAddress, FMAppPort, Time );
            FSockClient->SetTCPNoDelay( TRUE );                    ///< Turn on TCP NO DELAY
            FSockClient->SetBlockMode( FALSE );                    ///< Blocking mode
            FSockClient->SetWriteTimeout( WriteTimeoutms );        ///< Socket write time out
            FSockClient->SetBufferSize( SO_RCVBUF, MBUS_MSG_MAX ); ///< 1024K Solaris default
            FSockClient->SetBufferSize( SO_SNDBUF, MBUS_MSG_MAX ); ///<   64k AIX default
                        
            if( FAppName.Length() > 0 )
                UFC::BufferedLog::Printf( " App[%s] MBus connected FD:%d", FAppName.c_str(), FSockClient->GetHandle() );
            else
                UFC::BufferedLog::Printf( " MBus connected FD:%d", FSockClient->GetHandle() );
            return TRUE;
        }
        catch( UFC::SocketException& se )
        {
            FNeedReconnect = TRUE;
            if( FAppName.Length() > 0 )
                UFC::BufferedLog::Printf( " App[%s] MBus connection failed:%s", FAppName.c_str(), se.what());
            else
                UFC::BufferedLog::Printf( " MBus connection failed:%s", se.what());
            TriggerOnError( meConnectTimeout );
        }
    }
    return FALSE;
}
//------------------------------------------------------------------------------
void MApp::Disconnect( void )
{
    if( FAppName.Length() > 0 )
        UFC::BufferedLog::Printf( " App[%s] Close MBus connection FD:%d", FAppName.c_str(), FSockClient->GetHandle() );
    else
        UFC::BufferedLog::Printf( " Close MBus connection FD:%d", FSockClient->GetHandle() );
    FSockClient->Disconnect( TRUE );
    FSelfDisconnect = TRUE;
}
//------------------------------------------------------------------------------
// Register a subject/key to server
//------------------------------------------------------------------------------
void MApp::Regist( const MString& SubjectName, const MString& SubjectKey )
{
    UFC::PLockObject CSObj( FRegCS );

    FRegSubjects->Add( SubjectName, SubjectKey ); ///< Add to registed list
    if( FHandshake == TRUE &&  FSockClient->IsConnect() == TRUE ) ///< Is MApp online ?
    {
        if( FBatchAdding == FALSE  )
        {
            RegistMessage RegMsg( 0, SubjectName, SubjectKey );
            LockAndSend( &RegMsg ); ///< Send regist message to server.
        }
        else
            FBatchRegSubjects->Add( SubjectName, SubjectKey ); ///< Add to batch list
    }
}
//------------------------------------------------------------------------------
// Register all subjects/keys to server
//------------------------------------------------------------------------------
void MApp::RegistAll( void )
{
    UFC::PLockObject CSObj( FRegCS );

    LockAndSend( FRegSubjects );///< Send regist message to server.
}
//------------------------------------------------------------------------------
// Virtual functions to fire events
//------------------------------------------------------------------------------
void MApp::TriggerOnMessage( DataMessage* MsgHeader )
{
    MSubscriber*     Subscriber;
    PSubscriberList* Subscribers;
    MessageListener* EventListener;
    const MString&   Subject = MsgHeader->getSubject();
    const MString&   Key     = MsgHeader->getKey();
    MTree            TreeData;

    if( MsgHeader != NULL )
    {
        MsgHeader->getData( TreeData );
        if( MsgHeader->GetSenderID() != 0 )
            TreeData.append( "_IP", MsgHeader->GetSenderID() );

        UFC::PLockObject CSObj( FSubscriberListCS );        
        
        if( (Subscribers = FindSubscriber( Subject, Key ))!= NULL)
        {
            for( register int i = 0; i < Subscribers->ItemCount(); i++ )
            {
                try
                {
                    if( (Subscriber = Subscribers->GetItem( i )) != NULL )
                    {
                        if( ( EventListener = Subscriber->getListener() ) != NULL )
                            EventListener->OnMigoMessage( Subject, Key, &TreeData );
                    }
                }
                catch( UFC::Exception& e )
                {
                    UFC::BufferedLog::Printf(" OnMessage(%s,%s). Exception[%s].", Subject.c_str(), Key.c_str(), e.what() );
                }
                catch( ... )
                {
                    UFC::BufferedLog::Printf(" OnMessage(%s,%s). Unknown Exception.", Subject.c_str(), Key.c_str() );
                }
            }
        }
        else
            UFC::BufferedLog::DebugPrintf( " Can't find the subscriber[%s.%s].", Subject.c_str(), Key.c_str() );
        delete MsgHeader;
    }
}
//------------------------------------------------------------------------------
void MApp::TriggerOnConnected( void )
{
    if( FListener != NULL )
        FListener->OnMAppConnected();
}
//------------------------------------------------------------------------------
void MApp::TriggerOnError( PMAppError Error )
{
    if( FListener != NULL )
        FListener->OnMAppError( Error );
}
//------------------------------------------------------------------------------
void MApp::TriggerOnDisconnected( void )
{
    FHandshake     = FALSE;
    if( FListener != NULL )
        FListener->OnMAppDisconnected();
}
//------------------------------------------------------------------------------
// Received handshake message.
// Means connection in confirmed by server.
//------------------------------------------------------------------------------
void MApp::ReceiveHandshake( MigoHeader* Header )
{
    if( Header->GetACK() == ACK_CONNECT_CONFIRM )
    {
        int HeartbeatTimeout = Header->GetMessageID();

        ///< HeartbeatTimeout value given by server
        if( HeartbeatTimeout > 10 && HeartbeatTimeout < 300 )
        {
            FHEARTBEAT_TIMEOUT = HeartbeatTimeout + 5;
            FHEARTBEAT_TIME    = HeartbeatTimeout;
        }
        else ///< Use default
        {
            FHEARTBEAT_TIMEOUT = 45;
            FHEARTBEAT_TIME    = 40;
        }
        if( FAppName.Length() > 0 )
            UFC::BufferedLog::Printf( " App[%s] Server Heartbeat interval:%d sec timeout:%d sec ", FAppName.c_str(), FHEARTBEAT_TIME, FHEARTBEAT_TIMEOUT );
        else
            UFC::BufferedLog::Printf( " Server Heartbeat interval:%d sec timeout:%d sec ", FHEARTBEAT_TIME, FHEARTBEAT_TIMEOUT );
        FHandshake = TRUE;
        if( FRegSubjects->ItemCount() > 0 )
            RegistAll(); ///< regist all subjects,keys to server.
        TriggerOnConnected();
    }
}
//------------------------------------------------------------------------------
void MApp::RemoveRegRecord( MComponent* pComponent )
{
    UFC::PLockObject CSObj( FRegCS );
    FRegSubjects->Remove( pComponent->getSubjectName(), pComponent->getSubjectKey() );
}
//------------------------------------------------------------------------------
//
//  Implement SocketClientListener Interface
//
//------------------------------------------------------------------------------
//  Occur when connected to server.
//------------------------------------------------------------------------------
void MApp::OnConnect( UFC::PClientSocket * Socket )
{
    try
    {
        FRecvTimer      = 0;
        FSendTimer      = 0;
        FHandshake      = FALSE;
        FLocalIPAddress = Socket->GetSocketIPAddress();
        ConnectMessage ConnectedMessage( 0, FAppName );
        LockAndSend( &ConnectedMessage );  ///< Send a connected acknowledgement.
    }
    catch( UFC::SocketException & )
    {
        if( FAppName.Length() > 0 )
            UFC::BufferedLog::Printf( " App[%s] Send connected ack failed, try to reconnect server.", FAppName.c_str());
        else
            UFC::BufferedLog::Printf( " Send connected ack failed, try to reconnect server.");
        FNeedReconnect = TRUE;
        TriggerOnError( meSendConnectedAckFailed );
        FSockClient->Disconnect( TRUE );
    }
}
//------------------------------------------------------------------------------
//  Occur when the client try to disconnect.
//------------------------------------------------------------------------------
//void MApp::OnDisconnect( UFC::PClientSocket *  )
void MApp::OnDisconnect( UFC::PClientSocket *, BOOL NeedReconnect ) // modify by joe
{
    try
    {
        MigoHeader Header( ACK_DISCONNECT, 0, 0 );

        FLocalIPAddress = MAPP_LOCAL_IP;
        FHandshake      = FALSE;
        if( NeedReconnect )
            FNeedReconnect = TRUE;
        LockAndSend( &Header ); ///< Send a disconnect acknowledgement.
    }
    catch( UFC::SocketException & )
    {
        if( FAppName.Length() > 0 )
            UFC::BufferedLog::Printf( " App[%s] Send a disconnect acknowledge failed.", FAppName.c_str());
        else
            UFC::BufferedLog::Printf( " Send a disconnect acknowledge failed.");
    }
    TriggerOnDisconnected();
}
//------------------------------------------------------------------------------
//  Occur when the server send data to client.
//------------------------------------------------------------------------------
BOOL MApp::OnDataArrived( UFC::PClientSocket * Socket )
{
    MigoHeader*     Header = NULL;
    DataMessage*    DataMsg;
    UFC::AnsiString ErrMsg;
    UInt32          RecvSize = 0;
    BOOL            RetInqueue;

    try
    {
        if( (Header = MigoHeader::CreateFromSocket( Socket, RecvSize )) != NULL )
        {
            FAcuRecvBytes += RecvSize; ///< Add receive bytes.
            FAcuRecvMsgs ++;           ///< Increase incoming message count.
            FRecvTimer = 0;            ///< Reset heartbeat read direction counter.
            if( FHandshake == FALSE )  ///< Handshake finished ?
            {
                ReceiveHandshake( Header );  ///< Try to get the handshake message.
                delete Header;
            }
            else
            {
                switch( Header->GetACK() )
                {
                    case ACK_MESSAGE: ///< Data Message							
                                    DataMsg = static_cast<DataMessage*>( Header ) ;
                                    if( DataMsg->getSubject() == ADMIN_SUBJECT )///< It's a Admin reply message,High priority!
                                        RetInqueue = FMsgQueue->InqueueFront( DataMsg ); ///< Add message in the front of the queue.
                                    else
                                        RetInqueue = FMsgQueue->Inqueue( DataMsg ); /// Adds an element to the back of the queue.
                                    if( FMsgQueue->ItemCount() == FMsgQueue->Size() && RetInqueue == TRUE )
                                    {
                                        if( FAppName.Length() > 0 )
                                            UFC::BufferedLog::Printf( " App[%s] In-queue message full, MessageID:[%d], Count:[%d], Size:[%d]", FAppName.c_str(), DataMsg->GetMessageID(), FMsgQueue->ItemCount(), FMsgQueue->Size() );
                                        else
                                            UFC::BufferedLog::Printf( " In-queue message full, MessageID:[%d], Count:[%d], Size:[%d]", DataMsg->GetMessageID(), FMsgQueue->ItemCount(), FMsgQueue->Size() );
                                    }
                                    else if( RetInqueue != TRUE )
                                    {
                                        if( FAppName.Length() > 0 )
                                            UFC::BufferedLog::Printf( " App[%s] Error: In-queue message failed. MessageID:[%d], Count:[%d], Size:[%d]", FAppName.c_str(), DataMsg->GetMessageID(), FMsgQueue->ItemCount(), FMsgQueue->Size() );                            
                                        else
                                            UFC::BufferedLog::Printf( " Error: In-queue message failed. MessageID:[%d], Count:[%d], Size:[%d]", DataMsg->GetMessageID(), FMsgQueue->ItemCount(), FMsgQueue->Size() );                            
                                    }
                                    if( FDelayRecv > 0 )
                                        UFC::SleepMS( FDelayRecv );
                                    break;
                    case ACK_HEARTBEAT: ///< Server heartbeat
                                    SendHeartbeatConfirm(); ///< Reply server heartbeat.
                                    delete Header;
                                    break;
                    case ACK_HEARTBEAT_CONFIRM: ///< Receive heatbeat confirm from server.
                                    delete Header;
                                    break;
                    default: ///< Unknown message
                                    ErrMsg.Printf( "Receive un-handled message type:%d", Header->GetACK() );
                                    throw( UFC::SocketException( ErrMsg.c_str() ));
                }
            }
        }
        return TRUE;
    }
    catch( UFC::SocketException & e )
    {
        if( Header != NULL )
            delete Header;
        if( FAppName.Length() > 0 )
            UFC::BufferedLog::Printf( " App[%s] MBus received data failed:%s", FAppName.c_str(), e.what( ) );
        else
            UFC::BufferedLog::Printf( " MBus received data failed:%s", e.what( ) );
        FNeedReconnect = TRUE;
        FHandshake     = FALSE;
        if( FAppName.Length() > 0 )
            UFC::BufferedLog::Printf( " App[%s] Trigger OnDisconnected event.", FAppName.c_str() );
        else
            UFC::BufferedLog::Printf( " Trigger OnDisconnected event." );
        TriggerOnDisconnected();
        FSockClient->Disconnect( FALSE );
    }
    return FALSE;
}
//------------------------------------------------------------------------------
void MApp::SendHeartbeat( void )
{
    MigoHeader  ClientHeartbeat( ACK_HEARTBEAT, 0, 0  );

    LockAndSend( &ClientHeartbeat ); ///< LockAndSend will reset FSendTimer.
}
//------------------------------------------------------------------------------
void MApp::SendHeartbeatConfirm( void )
{
    MigoHeader  HeartBeatConfirm( ACK_HEARTBEAT_CONFIRM );

    LockAndSend( &HeartBeatConfirm ); ///< LockAndSend will reset FSendTimer.
}
//---------------------------------------------------------------------------
BOOL  MApp::IsConnected( void )
{
    BOOL Rtn = FSockClient->IsConnect();
    return Rtn;
}
//---------------------------------------------------------------------------
//
//  Functions fast write MBus DataMessage to socket  
//
//---------------------------------------------------------------------------
void MApp::BeginSend( MTHandle& Handle, const UFC::AnsiString& Subject, const UFC::AnsiString& Key )
{
    UFC::NInt32 NetInt( 0 );

    Handle.FWriteStream.Clear();
    Handle.FNodeCount = 0;
    ///< Write MIGO_HEADER_SIZE bytes to Skip the message header.
    Handle.FWriteStream.Write( MIGO_HEADER_SIZE );
    ///< Save message body.
    Subject.SaveToStream( &Handle.FWriteStream );        ///< Save Subject to straeam.
    Key.SaveToStream( &Handle.FWriteStream );            ///< Save Key to straeam.
    Handle.FMTreePos = Handle.FWriteStream.GetPosition();///< keep node count position.
    NetInt.SaveToStream( &Handle.FWriteStream );         ///< Save node count.
}
//---------------------------------------------------------------------------
void MApp::WriteString( MTHandle& Handle, const UFC::AnsiString& Name, const UFC::AnsiString& Value )
{
    UFC::NInt32  NetInt;
    int          StrSize = Value.Length() + 1;

    Name.SaveToStream( &Handle.FWriteStream );
    NetInt = StrSize;
    NetInt.SaveToStream( &Handle.FWriteStream );
    NetInt = M_STRING;
    NetInt.SaveToStream( &Handle.FWriteStream );
    Handle.FWriteStream.Write( Value.c_str() , StrSize );
    Handle.FNodeCount++;
}
//---------------------------------------------------------------------------
void MApp::WriteZipString( MTHandle& Handle, const UFC::AnsiString& Name, const UFC::AnsiString& Value )
{

    UFC::CompressedData* cData;
    int                  StrSize = Value.Length() + 1;

    if( (cData = UFC::Compress( (const UInt8*)Value.c_str(), StrSize )) != NULL ) ///< Compress it!!
    {
        if( cData->GetSize() > (UInt32)StrSize  ) ///< No needs compress!
        {
            UFC::NInt32  DataType( (UFCType::Int32)M_ROWDATA );
            UFC::NInt32  DataSize( StrSize );

            Name.SaveToStream( &Handle.FWriteStream ); ///< Name
            DataSize.SaveToStream( &Handle.FWriteStream );///< Size
            DataType.SaveToStream( &Handle.FWriteStream );///< Type
            Handle.FWriteStream.Write( Value.c_str() , StrSize );///< Data
        }
        else ///< compressed data
        {
            UFC::NInt32  DataType( (UFCType::Int32)M_COMPRESSEDDATA );
            UFC::NInt32  DataSize( cData->GetSize() );

            Name.SaveToStream( &Handle.FWriteStream ); ///< Name
            DataSize.SaveToStream( &Handle.FWriteStream ); ///< Size
            DataType.SaveToStream( &Handle.FWriteStream ); ///< Type
            Handle.FWriteStream.Write(  cData->GetData(), cData->GetSize() );//Data
        }
        Handle.FNodeCount++;
        delete cData;
    }
}
//---------------------------------------------------------------------------
void MApp::WriteInt32( MTHandle& Handle, const UFC::AnsiString& Name, UFCType::Int32 Value )
{
    UFC::NInt32  NetInt;

    Name.SaveToStream( &Handle.FWriteStream );
    NetInt = sizeof(int);
    NetInt.SaveToStream( &Handle.FWriteStream );
    NetInt = M_INT;
    NetInt.SaveToStream( &Handle.FWriteStream );
    NetInt = Value;
    NetInt.SaveToStream( &Handle.FWriteStream );
    Handle.FNodeCount++;
}
//---------------------------------------------------------------------------
void MApp::WriteInt64( MTHandle& Handle, const UFC::AnsiString& Name, UFCType::Int64 Value )
{
    UFC::NInt32  NetInt;
    UFC::NInt64  NetInt64;

    Name.SaveToStream( &Handle.FWriteStream );   ///< Save name
    NetInt = sizeof(int);
    NetInt.SaveToStream( &Handle.FWriteStream ); ///< Save size
    NetInt = M_INT64;
    NetInt.SaveToStream( &Handle.FWriteStream ); ///< Save data type
    NetInt64 = Value;
    NetInt64.SaveToStream( &Handle.FWriteStream ); ///< Save binary data
    Handle.FNodeCount++;
}
//---------------------------------------------------------------------------
void MApp::EndWrite( MTHandle& Handle )
{
    UFC::NInt32  NetInt;
    Int32        Size;

    Handle.FWriteStream.Seek( 0, UFC::soFromBeginning );         ///< Seek to the begining.
    NetInt = ACK_MESSAGE;
    NetInt.SaveToStream( &Handle.FWriteStream );                 ///< Save action code.
    NetInt = 0;
    NetInt.SaveToStream( &Handle.FWriteStream );                 ///< Save Sender ID.
    NetInt.SaveToStream( &Handle.FWriteStream );                 ///< Save Message ID.
    Size = ( Handle.FWriteStream.GetSize() - MIGO_HEADER_SIZE ); ///< Data size not include Header size.
    NetInt = Size;
    NetInt.SaveToStream( &Handle.FWriteStream );                 ///< Save the size to stream.
    NetInt = MigoHeader::HeaderChecksum( ACK_MESSAGE, Size, 0 ); ///< Calculate the checksum.
    NetInt.SaveToStream( &Handle.FWriteStream );                 ///< Save the checksum to stream.

    Handle.FWriteStream.Seek( Handle.FMTreePos, UFC::soFromBeginning );///< Seek to the Node Count position.
    NetInt = Handle.FNodeCount;
    NetInt.SaveToStream( &Handle.FWriteStream );                       ///< Save node count.    
}
//---------------------------------------------------------------------------
BOOL MApp::EndSend( MTHandle& Handle )
{
    EndWrite( Handle );
    return send_direct( &Handle.FWriteStream  );
}
//---------------------------------------------------------------------------
