/**@file Sigo.cpp
*  @brief class MessageObject to warp MigoApp
*  @author Simon Chang
*
*/
#include "Sigo.h"
#include "BufferedLog.h"
//---------------------------------------------------------------------------
const int REQ_MAX = 200;
//----------------------------------------------------------------------------------------------------------------
AppExitThread::AppExitThread( int MS )
:UFC::PThread( NULL, TRUE )
,FMS( MS )
{
	Start( 4*1024 );
}
//----------------------------------------------------------------------------------------------------------------
void AppExitThread::Execute( void )
{
    UFC::SleepMS( FMS );
    UFC::BufferedLog::Printf( " ___________________ Terminated _____________________" );
    exit( 0 );
}
//----------------------------------------------------------------------------------------------------------------
MessageObject::MessageObject( UFC::AnsiString AppName )
:UFC::PThread( NULL, FALSE )
,FApp( NULL )
,FListener( &FEmptyListener )
,FAppName( AppName )
,FResponseListener( this )
{
    MApp::MAppProperties AppProps;               // declare MAPP properties
    AppProps.setAppName( AppName.c_str());       // set application name
    FApp = new MApp( &AppProps, MApp::MAPP_DEF_PORT, MApp::MSG_QUEUE_SIZE  );
    FApp->SetListener( this );
    AddListener( "MBUS", "all", this );
    FResponseSubject.Printf( "%s%s_%010u", UFC::Hostname, AppName.c_str(), UFC::GetTickCountMS() );
    AddListener( FResponseSubject, "all", &FResponseListener );
}
//----------------------------------------------------------------------------------------------------------------
MessageObject::MessageObject( UFC::AnsiString AppName,
                              UFC::AnsiString Version,
                              UFC::AnsiString Description,
                              UFCType::Int32 Port )
:UFC::PThread( NULL, FALSE )
,FApp( NULL )
,FListener( &FEmptyListener )
,FAppName( AppName )
,FResponseListener( this )
{
    MApp::MAppProperties AppProps;               // declare MAPP properties
    AppProps.setAppName( AppName.c_str());       // set application name
    AppProps.setAppVersion( Version.c_str() );   // set application version
    AppProps.setAppInfo( Description.c_str() );  // set banner description
    AppProps.setConfigURL( "Sigo" );
    FApp = new MApp( &AppProps, Port, MApp::MSG_QUEUE_SIZE );
    FApp->SetListener( this );    
    AddListener( "MBUS", "all", this );
    FResponseSubject.Printf( "%s%s_%010u", UFC::Hostname, AppName.c_str(), UFC::GetTickCountMS() );
    AddListener( FResponseSubject, "all", &FResponseListener );
}
//----------------------------------------------------------------------------------------------------------------
MessageObject::~MessageObject( )
{
    if( FApp != NULL )
    {
        FApp->stop();
        Terminate();
        WaitFor();
        delete FApp;
    }
}
//----------------------------------------------------------------------------------------------------------------
void MessageObject::SetHost( const UFC::AnsiString& HostIP, int Port )
{
    FApp->SetHost( HostIP );
    FApp->SetPort( Port );
}
//----------------------------------------------------------------------------------------------------------------
MSubscriber* MessageObject::GetSubscriber( const UFC::AnsiString& Subject, const UFC::AnsiString& Key )
{
    if( FApp != NULL )
    {
	PSubscriberList* Subscribers = FApp->FindSubscriber( Subject, Key );
        if( Subscribers != NULL )
            return Subscribers->GetItem(0);
    }
    return NULL;
}
//----------------------------------------------------------------------------------------------------------------
void MessageObject::Execute( )
{
    try
    {
       ///< Try to connect to MBus when Start() called.
       FApp->ConnectOnStart( TRUE ); 
       ///< Auto Reconnect when broken.
       FApp->start( TRUE );          
    }
    catch( UFC::Exception& e )
    {
       UFC::BufferedLog::Printf( " MBus client Exception:%s", e.what() );
    }    
}
//----------------------------------------------------------------------------------------------------------------
//  Public functions.
//----------------------------------------------------------------------------------------------------------------
void MessageObject::AddListener( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MessageListener* Listener  )
{
    if( FApp != NULL )
    {
        if( GetSubscriber( Subject, Key ) == NULL )
        {
            UFC::AnsiString  ObjectName = "Subscriber" + Subject + Key;
            MSubscriber*     NewSubscriber = new MSubscriber( ObjectName, Subject, Key );

            FApp->AddSubscriber( NewSubscriber );
            NewSubscriber->addListener( Listener );
        }
    }
}
//----------------------------------------------------------------------------------------------------------------
void MessageObject::AddListener( const UFC::AnsiString& Subject, RequestMessageListener* Listener )
{    
    if( FApp != NULL )
    {
        if( GetSubscriber( Subject, "all" ) == NULL )
        {
            UFC::AnsiString     ObjectName = "Subscriber" + Subject + "all";
            MSubscriber*        NewSubscriber  = new MSubscriber( ObjectName, Subject, "all" );
            SyncMessageHandler* SyncSubscriber = new SyncMessageHandler( this , Listener );

            FApp->AddSubscriber( NewSubscriber );
            NewSubscriber->addListener( SyncSubscriber );
        }
    }
}
//----------------------------------------------------------------------------------------------------------------
BOOL MessageObject::Send( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree& Data, BOOL KeepUnsent )
{
    if( FApp != NULL )
        return FApp->send_by_key( Subject, Key, Data, KeepUnsent );
    return FALSE;
}
//----------------------------------------------------------------------------------------------------------------
void MessageObject::WaitForConnected( int TimeoutSec )
{    
    FApp->WaitForConnected( TimeoutSec );
}
//---------------------------------------------------------------------------
int MessageObject::GetMaxRequest( const UFC::AnsiString& Subject )
{
    ThroughputCtrl* Info;
    
    if( FMaxRequest.GetObjectByKey( Subject, Info ) == false )   
    {
        Info = new ThroughputCtrl( REQ_MAX );
        FMaxRequest.Add( Subject, Info );
    }        
    return Info->GetMaxRequest();
}
//---------------------------------------------------------------------------
int MessageObject::SetMaxRequest( const UFC::AnsiString& Subject, int Max )
{   
    ThroughputCtrl* Info;
    int                OldReqMax;
    
    if( FMaxRequest.GetObjectByKey( Subject, Info ) == false )   
    {
        Info = new ThroughputCtrl( REQ_MAX );
        FMaxRequest.Add( Subject, Info );
    }        
    OldReqMax = Info->GetMaxRequest(); 
    Info->SetMaxRequest( Max );
    return OldReqMax;
}
//---------------------------------------------------------------------------
BOOL MessageObject::IncreaseRequestCount( const UFC::AnsiString& Subject )
{
    ThroughputCtrl* Info;
    
    if( FMaxRequest.GetObjectByKey( Subject, Info ) == false )   
    {
        Info = new ThroughputCtrl( REQ_MAX );
        FMaxRequest.Add( Subject, Info );
    }        
    return Info->Increase();
}
//---------------------------------------------------------------------------
void MessageObject::DecreaseRequestCount( const UFC::AnsiString& Subject )
{
    ThroughputCtrl* Info;
    
    if( FMaxRequest.GetObjectByKey( Subject, Info ) == false )   
    {
        Info = new ThroughputCtrl( REQ_MAX );
        FMaxRequest.Add( Subject, Info );
    }        
    Info->Decrease();    
}
//------------------------------------------------------------------------------
void MessageObject::UniqueReqID( UFC::AnsiString& ReqID  )
{
    UFC::PLockObject  Lock( FUIDLock );
    
    ReqID.Printf( "%s%s_%010u", UFC::Hostname, FAppName.c_str(), FReqSeq );
    FReqSeq++;
}
//------------------------------------------------------------------------------
void MessageObject::AddReqEvent( const UFC::AnsiString& ReqID, ResponseData* ResponseEvent )
{
    UFC::PLockObject  Lock( FMapLock );
       
    FRequestEvents.Add( ReqID, ResponseEvent );                    
}
//------------------------------------------------------------------------------
void MessageObject::DeleteReqEvent( const UFC::AnsiString& ReqID )
{    
    UFC::PLockObject  Lock( FMapLock );
    
    FRequestEvents.DeleteByKey( ReqID );    
}
//------------------------------------------------------------------------------
RequestResult MessageObject::SendRequest( const UFC::AnsiString& Subject,  MTree& Data, MTree& Response, int TimeoutMS )
{
    if( FApp != NULL && FApp->IsConnected() == TRUE  )
    {
        UFC::AnsiString  ReqID;
        BOOL             WaitResult;
        
        if( IncreaseRequestCount( Subject ) == FALSE )
            return rrMaxReq; ///< Reach the Max request
        UniqueReqID( ReqID );  ///< Generate unique request ID.   
        Data.append( "ReqID", ReqID );        
        
        ResponseData  ResponseEvent( Response );
        
        AddReqEvent( ReqID, &ResponseEvent );        ///< Create a response object.        
        FApp->send_by_key( Subject, FResponseSubject, Data, FALSE );  ///< Send request to MBus.       
        WaitResult = ResponseEvent.WaitFor( TimeoutMS );             ///< Wait for response message.       
        DeleteReqEvent( ReqID );                       ///< Delete the response object.                       
        
        DecreaseRequestCount( Subject );
        if( WaitResult == TRUE )
            return rrOK;
        return rrTimeout;            
    }
    return rrNotConnected;
}
//---------------------------------------------------------------------------
void SyncMessageHandler::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* Req )
{
    MTree           Response;
    UFC::AnsiString ReqID;
    
    if( FEvent->OnRequest( Subject, Req, &Response ) == true ) ///< Need Response    
    {
    	if( Req->get( "ReqID", ReqID ) == TRUE ) /// Allen modify at 20190614
        	FOwner->GetMApp()->send_by_key( Key, ReqID, Response, FALSE );
	}
}
//----------------------------------------------------------------------------------------------------------------
void MessageObject::HandleResponse( const UFC::AnsiString& ReqID, MTree* Data )
{
    ResponseData*     ResponseEvent;     
    UFC::PLockObject  Lock( FMapLock );

    if( FRequestEvents.GetObjectByKey( ReqID, ResponseEvent ) == TRUE ) /// Allen modify at 20190614
    {
        ResponseEvent->Data.Copy( *Data ); ///< Copy the response data
        ResponseEvent->SetEvent();         ///< Trigger response event.          
    }     
}
//----------------------------------------------------------------------------------------------------------------
void ResponseHandler::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* Data )
{     
     FOwner->HandleResponse( Key, Data );
}
//----------------------------------------------------------------------------------------------------------------
//  Functions for dependency processes monitoring
//
//----------------------------------------------------------------------------------------------------------------
void MessageObject::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* Data )
{
    UFCType::Int32 CMD;

    if( Data->get( "CMD", CMD ) == TRUE )
    {
        switch( CMD )
        {
            case ADMIN_REPLY_PROCESS: CheckStat( Data );break;
            case ADMIN_APP_OFFLINE:   OnStatChange( FALSE, Data );break;
            case ADMIN_APP_ONLINE:    OnStatChange( TRUE, Data );break;
            case ADMIN_APP_FIRST_ONE: FListener->OnProcessConnected( TRUE );break;
            case ADMIN_APP_EXISTS:    FListener->OnProcessConnected( FALSE );break;
            case ADMIN_KILL_PROCESS:  OnKill( Data );break;
        }
    }  
}
//---------------------------------------------------------------------------
void MessageObject::CheckStat( MTree* Data )
{
    int              Count;
    UFC::PStringList ProcessList;
    UFC::AnsiString  Host,App,KeyStr;
    ProcessStateObj* InfoObj; 

    if( Data->get( "COUNT", Count ) && Data->get( "HOST", Host ) )
    {        
        for( register int i = 0; i < Count; i++ )
        {
            UFC::AnsiString FieldName( i );
             
            if( Data->get( FieldName, App ) == TRUE )
            {
                ProcessList.Add( App );
                KeyStr = Host + App;
                if( ( InfoObj = FProcesss.GetObjectByKey( KeyStr )) != NULL )
                {
                    if( InfoObj->IsAlive() == FALSE )
                    {
                        InfoObj->Alive( TRUE );
                        ///< Trigger event.
                        FListener->OnProcessStartup( Host, App );
                    }
                }
            }
        }
        if( Host.AnsiCompare( UFC::Hostname ) == 0 )
            FListener->OnProcessList( ProcessList );
    }
}
//------------------------------------------------------------------------------
void MessageObject::OnKill( MTree* Data )
{
    UFC::AnsiString  App,Host,Killer;

    if( Data->get( "APP", App ) && Data->get( "HOST", Host ) && Data->get( "KILLER", Killer ) )
    {
        if( FApp->GetAppName() == App && Host.AnsiCompare( UFC::Hostname ) == 0 )
        {
            bool CanExit = true;

            UFC::BufferedLog::Printf( " [%s] want to stop this process.", Killer.c_str() );
            ///< Trigger event.
            FListener->OnAppExit( CanExit );
            if( CanExit == true )
            {
                UFC::BufferedLog::Printf( " Process %s is stopping.", App.c_str() );
                new AppExitThread( 50 );
            }
            else
                UFC::BufferedLog::Printf( " Process %s can't stop now.", App.c_str() );
        }
    }
}
//------------------------------------------------------------------------------
void MessageObject::OnStatChange( BOOL IsOnline, MTree* Data )
{
	UFC::AnsiString  App,Host,KeyStr,StopApp;
	ProcessStateObj* InfoObj;

	if( Data->get( "APP", App ) && Data->get( "HOST", Host ) )
	{
            if( App.AnsiCompare("MBus") == 0 ) ///< MBus State changed.
            {
                if( IsOnline == FALSE ) ///< A MBus offline.
                {
                    for( int i = 0; i < FProcesss.ItemCount();i++)
                    {
                        if( FProcesss.GetItem( i, KeyStr, InfoObj ) )
                        {   ///< Is this process in that MBus server?
                            if( KeyStr.AnsiNCompare( Host.c_str(), Host.Length() ) == 0 )
                            {   ///< Is this process alive ?                           
                                if( InfoObj->IsAlive() )
                                {
                                    StopApp = KeyStr.SubString( Host.Length(), KeyStr.Length() - Host.Length() );
                                    InfoObj->Alive( FALSE );  
                                    FListener->OnProcessStopped( Host, StopApp );///< Trigger OnProcessStopped event
                                }
                            }
                        }
                    }
                }
                else  ///< A MBus online, Ask MBus the processes state.
                {                
                    MTree AskData;
                    AskData.append( "CMD", ADMIN_ASK_PROCESS );
                    Send( "MBUS", Host, AskData, FALSE );
                }
            }
            else
            {
                KeyStr.Printf( "%s%s", Host.c_str(),  App.c_str() );
                if( ( InfoObj = FProcesss.GetObjectByKey( KeyStr )) != NULL )
		{
                    InfoObj->Alive( IsOnline );
                    if( IsOnline )
			FListener->OnProcessStartup( Host, App );
                    else
			FListener->OnProcessStopped( Host, App );
		}
            }
	}
}
//----------------------------------------------------------------------------------------------------------------
void MessageObject::SetMonitorListener( MonitorListener* Listener ) 
{
    if( Listener != NULL )
        FListener = Listener;
}
//----------------------------------------------------------------------------------------------------------------
void MessageObject::AddMonitoringProcess( const UFC::AnsiString& Host, const UFC::AnsiString& AppName )
{
    UFC::AnsiString KeyString;

    KeyString.Printf( "%s%s", Host.c_str(),  AppName.c_str() );
    FProcesss.Add( KeyString, new ProcessStateObj( Host, AppName) );
    ///< Ask to MBus
    MTree Data;
    Data.append( "CMD", ADMIN_ASK_PROCESS );
    Send( "MBUS", Host, Data, FALSE );
}
//----------------------------------------------------------------------------------------------------------------
ProcessState MessageObject::IsAlive( const UFC::AnsiString& Host, const UFC::AnsiString& AppName )
{
    UFC::AnsiString  KeyStr;
    ProcessStateObj* InfoObj; 
    
    KeyStr.Printf( "%s%s", Host.c_str(),  AppName.c_str() );
    if( ( InfoObj = FProcesss.GetObjectByKey( KeyStr )) != NULL )
    {
        if( InfoObj->IsAlive( ) == TRUE )
			return psRunning;
        else
            return psStopped;
    }
    return psNotMonitoring;
}
//----------------------------------------------------------------------------------------------------------------
void MessageObject::SendProcessListRequest( void )
{
    MTree	Data;
    
    Data.append( "CMD", ADMIN_ASK_PROCESS );
    Send( "MBUS", UFC::Hostname, Data ); 
}
//----------------------------------------------------------------------------------------------------------------
void MessageObject::OnMAppConnected()
{
    FListener->OnConnected();///< Trigger event.
}
//----------------------------------------------------------------------------------------------------------------
void MessageObject::OnMAppError( PMAppError Error )
{
    UFC::AnsiString ErrorText = "";
    switch (Error)
    {
        case meSendConnectedAckFailed: ErrorText = "Send Connected Ack. Failed"; break;
        case meUnregistFailed:         ErrorText = "Un-registered Failed";       break;
        case meReadDataFailed:         ErrorText = "Read Data Failed";           break;
        case meSendFailed:             ErrorText = "Send Failed";                break;
        case meSendFailedNotConnected: ErrorText = "Send Failed Not Connected";  break;
        case meConnectTimeout:         ErrorText = "Connect Timeout";            break;
        default:                       ErrorText = "Unknown Error";
        
    }
    UFC::BufferedLog::Printf(" MessageObject::OnMAppError(). Error:%d[%s]", Error, ErrorText.c_str());
    UFC::BufferedLog::FlushToFile();
    FListener->OnDisconnected();///< Trigger event.
}
//----------------------------------------------------------------------------------------------------------------
void MessageObject::OnMAppDisconnected()
{
    FListener->OnDisconnected();///< Trigger event.
}
//----------------------------------------------------------------------------------------------------------------
