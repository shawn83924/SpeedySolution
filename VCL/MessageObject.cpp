//---------------------------------------------------------------------------
#include <basepch.h>
#pragma hdrstop
#include "MessageObject.h"
#include "MWinApp.h"
#include "Publisher.h"
#include "Subscriber.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//---------------------------------------------------------------------------
//extern HINSTANCE DLLInstance;
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TMessageObject *)
{
    new TMessageObject(NULL);
}
//---------------------------------------------------------------------------
namespace Messageobject
{
    void __fastcall PACKAGE Register()
    {
         TComponentClass classes[1] = { __classid(TMessageObject) };
         RegisterComponents( "MBus", classes, 0 );
	}
}
//---------------------------------------------------------------------------
//
//  Class PThreadMApp
//  Wrap the MApp as a thread.
//
//---------------------------------------------------------------------------
__fastcall PThreadMApp::PThreadMApp( MApp* App )
:TThread( false )
,FApp( App )
{
}
//---------------------------------------------------------------------------
__fastcall PThreadMApp::~PThreadMApp( void )
{
   	Terminate();  ///< Try to terminate this thread.
    FApp->stop(); ///< Stop the MApp Object.
    WaitFor();    ///< Wait for this thread terminated.
}
//---------------------------------------------------------------------------
void __fastcall PThreadMApp::Execute( void )
{
    while( Terminated == FALSE )
    {
		FApp->start( FALSE );
    }
}
//---------------------------------------------------------------------------
void DoAdminListener::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* refMEvent )
{
	FMessageObject->OnAdminMessage( Subject,Key, refMEvent );
}
//---------------------------------------------------------------------------
//
//  Class TMessageObject
//  Wrap the MApp as a C++ Builder component.
//
//---------------------------------------------------------------------------
__fastcall TMessageObject::TMessageObject( TComponent* Owner )
:TComponent( Owner )
,FErrorMessage( "" )
,FHost( MAPP_LOCAL_IP )
,FConnectedHost( "" )
,FPort( MApp::MAPP_DEF_PORT )
,FIsConnected( false )
,FApp( NULL )
,FThreadMApp( NULL )
,FOnConnected( NULL )
,FOnError( NULL )
,FOnDisconnected( NULL )
{
	if( InitializeWinSock() == false )
		throw( UFC::SocketException( "Initialize Winsock2 failed!" ));
	if( !ComponentState.Contains( csDesigning ) )
	{
		UFC::AnsiString COMName;

		FApp = new MWinApp( HInstance );//DLLInstance );
		FApp->SetListener( this );
		FThreadMApp = new PThreadMApp( FApp ); ///< Create a new MApp thread
		COMName.Printf( "__MBUS_%010d", GetTickCount() );
		FDoAdminListener.SetMessageObject( this );
		FSubscriber = new MSubscriber( COMName, "MBUS", "all" );
		FSubscriber->addListener( &FDoAdminListener );
	}
}
//---------------------------------------------------------------------------
__fastcall TMessageObject::~TMessageObject( void )
{
	FIsConnected = false;
	if( FApp != NULL )
		FApp->SetListener( NULL );
	if( FThreadMApp != NULL )
	{
		PThreadMApp* TempDelete = FThreadMApp;
		FThreadMApp = NULL;
		delete TempDelete;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMessageObject::Loaded( void )
{
	SetHost( FHost );
	SetAppName( FAppName );
}
//---------------------------------------------------------------------------
bool __fastcall TMessageObject::InitializeWinSock( void )
{
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD( 2, 0 );

	if( WSAStartup( wVersionRequested, &wsaData ) == 0 )
	{
		if( LOBYTE( wsaData.wVersion ) != 2 ||HIBYTE( wsaData.wVersion ) != 0 )
		{
			WSACleanup( );
			return false;
		}
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TMessageObject::Connect( int Timeout )
{
	try
	{
		if( FApp != NULL )
		{
			FApp->AddSubscriber( FSubscriber );
			FApp->Connect( Timeout );
        }
	}
	catch( UFC::SocketException &e )
	{
		FIsConnected  = false;
		FErrorMessage = e.what();
		if( FOnError != NULL )
			FOnError( this );
	}
	catch( ... )
	{
		FIsConnected = false;
		FErrorMessage = "Unknown error";
		if( FOnError != NULL )
			FOnError( this );
	}
}
//---------------------------------------------------------------------------
bool  __fastcall TMessageObject::Send( const UFC::AnsiString& Subject,const UFC::AnsiString& Key,MTree* Data, bool KeepUnsend )
{
	if( FApp != NULL )
		return FApp->send_by_key( Subject, Key, *Data, KeepUnsend );
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TMessageObject::Disconnect(void)
{
	if( FApp != NULL )
		FApp->Disconnect();
}
//---------------------------------------------------------------------------
bool  __fastcall TMessageObject::IsConnected( void )
{
	return FIsConnected;
}
//---------------------------------------------------------------------------
unsigned long long __fastcall  TMessageObject::GetAcuRecvBytes( void )
{
	if( FApp != NULL )
		return FApp->GetAcuRecvBytes();
	return 0;
}
//---------------------------------------------------------------------------
unsigned long long __fastcall  TMessageObject::GetAcuRecvMsgs( void )
{
	if( FApp != NULL )
		return FApp->GetAcuRecvMsgs();
	return 0;
}
//---------------------------------------------------------------------------
int __fastcall TMessageObject::AddSubscriber( MSubscriber* Subscriber )
{
	if( FApp != NULL )
		return FApp->AddSubscriber( Subscriber );
	return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMessageObject::DeleteSubscriber( MSubscriber* Subscriber )
{
	if( FApp != NULL )
		FApp->DeleteSubscriber( Subscriber );
}
//---------------------------------------------------------------------------
void __fastcall TMessageObject::SetHost( const System::UnicodeString Host )
{
	FHost = Host;
	if( FApp != NULL )
	{
		AnsiString AnsiHost = FHost;
		FApp->SetHost( AnsiHost.c_str() );
	}
}
//---------------------------------------------------------------------------
void __fastcall TMessageObject::SetAppName( const System::UnicodeString AppName )
{
	FAppName = AppName;
	if( FApp != NULL )
	{
		AnsiString AnsiAppName = FAppName;
		FApp->SetAppName( AnsiAppName.c_str() );
	}
}
//---------------------------------------------------------------------------
void __fastcall TMessageObject::SetMBusPort( int Port )
{
	FPort = Port;
	if( FApp != NULL )
		FApp->SetPort( FPort );
}
//---------------------------------------------------------------------------
void TMessageObject::OnMAppConnected()
{
	if( FIsConnected == false )
	{
		FIsConnected = true;
		if( FOnConnected != NULL )
			OnConnected( this );
	}
}
//---------------------------------------------------------------------------
void TMessageObject::OnMAppDisconnected()
{
	FIsConnected = false;
	ClearProcessState();
	if( FOnDisconnected != NULL )
		OnDisconnected( this );
}
//---------------------------------------------------------------------------
void TMessageObject::OnMAppError( PMAppError Error )
{
	FIsConnected = false;
	ClearProcessState();
	switch( Error )
	{
		case meSendConnectedAckFailed: FErrorMessage = "Send connected ack failed.";break;
		case meUnregistFailed:         FErrorMessage = "Unregister subject failed.";break;
		case meReadDataFailed:         FErrorMessage = "Receive message failed.";break;
		case meSendFailed:             FErrorMessage = "Send message failed.";break;
		case meSendFailedNotConnected: FErrorMessage = "Not connected.";break;
	}
	if( FOnError != NULL )
		OnError( this );
}
//---------------------------------------------------------------------------
void __fastcall  TMessageObject::ClearProcessState( void )
{
	FMonioringSet.Clear();

	for( int i = 0; i < FProcesss.ItemCount(); i++ )
	{
		ProcessStateObj* State = FProcesss.GetItem( i );
		delete State;
	}
	FProcesss.Clear();
}
//---------------------------------------------------------------------------
//
// Functions fo HA ability.
//
//---------------------------------------------------------------------------
void __fastcall TMessageObject::OnAdminMessage(const MString&, const MString&,MTree *Data)
{
	UFCType::Int32 CMD;
	UFC::AnsiString Host;

	if( Data->get( "CMD", CMD ) == TRUE )
	{
		switch( CMD )
		{
			///< Global message. all MApp connect to the MBus
			///< will received this message.
			case ADMIN_APP_OFFLINE:   OnStatChange( FALSE, Data );
									  break;
			case ADMIN_APP_ONLINE:    OnStatChange( TRUE, Data );
									  break;
			///< Request/Response message
			///< Only the client send the request will received the message.
			case ADMIN_REPLY_PROCESS: OnProcessListReceived( Data );
									  break;
			case ADMIN_REPLY_MBUS:    TriggerMBusListEvent( Data );
									  break;
			case ADMIN_APP_FIRST_ONE: if( Data->get("HOST",Host ) == TRUE )
									  {
										  FConnectedHost = Host;
										  if( FOnMBusConnected != NULL  )
											  FOnMBusConnected( Host, TRUE );
									  }
									  break;
			case ADMIN_APP_EXISTS:    if( Data->get("HOST",Host ) == TRUE )
									  {
										  FConnectedHost = Host;
										  if( FOnMBusConnected != NULL )
											  FOnMBusConnected( Host, FALSE );
									  }
									  break;
		}
	}
}
//----------------------------------------------------------------------------------------------------------------
void __fastcall TMessageObject::TriggerOnStartup( const UFC::AnsiString& Host, const UFC::AnsiString& App )
{
	if( FOnProcessStartup != NULL )
		FOnProcessStartup( Host, App );
}
//----------------------------------------------------------------------------------------------------------------
void __fastcall TMessageObject::TriggerOnStopped( const UFC::AnsiString& Host, const UFC::AnsiString& App )
{
	if( FOnProcessStopped != NULL )
		FOnProcessStopped( Host, App );
}
//----------------------------------------------------------------------------------------------------------------
void __fastcall TMessageObject::MakeKeyString( UFC::AnsiString& Key, const UFC::AnsiString& Host, const UFC::AnsiString& App )
{
	Key.Printf( "%s%s", Host.c_str(), App.c_str() );
}
//----------------------------------------------------------------------------------------------------------------
ProcessState __fastcall TMessageObject::IsAlive( const UFC::AnsiString& Host, const UFC::AnsiString& AppName )
{
	UFC::AnsiString  KeyStr;
	ProcessStateObj* InfoObj;

	MakeKeyString( KeyStr, Host, AppName );///< Make the process key string.
	if( FMonioringSet.Exists( KeyStr ) == TRUE )
	{
		if( ( InfoObj = FProcesss.GetObjectByKey( KeyStr )) != NULL )
		{
			if( InfoObj->IsAlive( ) == TRUE )
				return psRunning;
			else
				return psStopped;
		}
	}
	return psNotMonitoring;///< Not monitoring.
}
//----------------------------------------------------------------------------------------------------------------
void __fastcall TMessageObject::RequestMBusList( void )
{
	MTree Data;

	Data.append( "CMD", ADMIN_ASK_MBUS );
	Send( "MBUS", FConnectedHost, &Data, false );
}
//----------------------------------------------------------------------------------------------------------------
void __fastcall TMessageObject::RequestProcessList( const UFC::AnsiString& Hostname )
{
	MTree Data;

	Data.append( "CMD", ADMIN_ASK_PROCESS );
	Send( "MBUS", Hostname, &Data, false );
}
//----------------------------------------------------------------------------------------------------------------
void __fastcall TMessageObject::MonitoringProcess( const UFC::AnsiString& Host, const UFC::AnsiString& AppName )
{
	UFC::AnsiString KeyStr;
	ProcessStateObj* InfoObj;

	MakeKeyString( KeyStr, Host, AppName );
	if( FMonioringSet.Exists( KeyStr ) == FALSE ) ///< Not monitoring.
	{
		FMonioringSet.Add( KeyStr );
		///< Add to monitoring applications map.
		if( (InfoObj = FProcesss.GetObjectByKey( KeyStr )) == NULL )
		{
			MTree Data;

			FProcesss.Add( KeyStr, new ProcessStateObj( Host, AppName ) );
			///< Ask to MBus
			Data.append( "CMD", ADMIN_ASK_PROCESS );
			Send( "MBUS", Host, &Data, FALSE );
		}
		else
		{   ///< Process stat info already exists, trigger Startup/Stoped event.
			InfoObj->Replied( TRUE );
			if( InfoObj->IsAlive() )
				TriggerOnStartup( Host, AppName );
			else
				TriggerOnStopped( Host, AppName );
		}
	}
	else ///< Already monitoring.
	{
		if( (InfoObj = FProcesss.GetObjectByKey( KeyStr )) != NULL )
		{   ///< Process stat info already exists, trigger Startup/Stoped event.
			InfoObj->Replied( TRUE );
			if( IsConnected( ) == TRUE )
			{
				if( InfoObj->IsAlive() )
					TriggerOnStartup( Host, AppName );
				else
					TriggerOnStopped( Host, AppName );
			}
			else
			   	TriggerOnStopped( Host, AppName );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMessageObject::TriggerMBusListEvent( MTree*Data )
{
	TStringList*     MBusList = new TStringList();
	UFC::AnsiString  Host,MBusHost;
	int              Count;
	///< Get active MBus list.
	if( Data->get( "COUNT", Count ) && Data->get( "HOST", Host ) )
	{
		for( register int i = 0; i < Count; i++ )
		{
			 UFC::AnsiString FieldName( i );

			 if( Data->get( FieldName, MBusHost ) == TRUE )
				 MBusList->Add( MBusHost.c_str() ); ///< Add process name to process list.
		}
		///< Trigger OnProcessList event.
		if( FOnMBusList != NULL )
			FOnMBusList( Host, MBusList );
	}
	delete MBusList;
}
//------------------------------------------------------------------------------
void __fastcall TMessageObject::CheckUnrepliedProcess( const UFC::AnsiString& Host )
{
	UFC::AnsiString  KeyStr,StopApp;
	ProcessStateObj* InfoObj;

	for( int i = 0; i < FProcesss.ItemCount();i++)
	{
		if( FProcesss.GetItem( i, KeyStr, InfoObj ) )
		{   ///< Is this process in that MBus server?
			if( KeyStr.AnsiNCompare( Host.c_str(), Host.Length() ) == 0 )
			{   ///< Is this process alive ?
				if( InfoObj->IsReplied( ) == FALSE )
				{
				   StopApp = KeyStr.SubString( Host.Length(), KeyStr.Length() - Host.Length() );
				   InfoObj->Replied( TRUE );
				   TriggerOnStopped( Host, StopApp );///< Trigger OnProcessStopped event
				}
			}
		}
	 }
}
//---------------------------------------------------------------------------
void __fastcall TMessageObject::OnProcessListReceived( MTree* Data )
{
	int              Count;
	TStringList*     ProcessList = new TStringList();
	UFC::AnsiString  Host,App,KeyStr;
	ProcessStateObj* InfoObj;

	///< Get active process list.
	if( Data->get( "COUNT", Count ) && Data->get( "HOST", Host ) )
	{
		for( register int i = 0; i < Count; i++ )
		{
			 UFC::AnsiString FieldName( i );

			 if( Data->get( FieldName, App ) == TRUE )
			 {
				 ProcessList->Add( App.c_str() ); ///< Add process name to process list.
				 MakeKeyString( KeyStr, Host, App );
				 ///< Process information exists?
				 if( ( InfoObj = FProcesss.GetObjectByKey( KeyStr )) != NULL )
				 {
					 InfoObj->Replied( TRUE );
					 if( InfoObj->IsAlive() == FALSE )///< Process not active?
					 {
						 InfoObj->Alive( TRUE ); ///< Set it's stat to alive.
						 if( FMonioringSet.Exists( KeyStr ) == TRUE  )
							 TriggerOnStartup( Host, App ); ///< Trigger process startup event.
					 }
				 }
				 else ///< Process information not exists
				 {   ///< Add process info.
					 InfoObj = new ProcessStateObj( Host, App );
					 FProcesss.Add( KeyStr, InfoObj );
					 InfoObj->Alive( TRUE );
					 InfoObj->Replied( TRUE );
				 }
			 }
		}
		 CheckUnrepliedProcess( Host );
		///< Trigger OnProcessList event.
		if( FOnProcessList != NULL )
			FOnProcessList( Host, ProcessList );
	}
	delete ProcessList;
}
//------------------------------------------------------------------------------
void __fastcall TMessageObject::OnMBusOffline( const UFC::AnsiString& Host )
{
	UFC::AnsiString  KeyStr,StopApp;
	ProcessStateObj* InfoObj;

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
				   TriggerOnStopped( Host, StopApp );///< Trigger OnProcessStopped event
				}
			}
		 }
	}
	if( FOnMBusStopped != NULL )
		FOnMBusStopped( Host );
}
//------------------------------------------------------------------------------
void __fastcall TMessageObject::OnMBusOnline( const UFC::AnsiString& Host )
{
	UFC::AnsiString  KeyStr;
	ProcessStateObj* InfoObj;

	for( int i = 0; i < FProcesss.ItemCount();i++)
	{
		 if( FProcesss.GetItem( i, KeyStr, InfoObj ) )
		 {   ///< Is this process in that MBus server?
			if( KeyStr.AnsiNCompare( Host.c_str(), Host.Length() ) == 0 )
			{   ///< Monitoring process on this MBus server, Ask MBus the processes state.
				MTree Data;

				Data.append( "CMD", ADMIN_ASK_PROCESS );
				Send( "MBUS", Host, &Data, FALSE );
				break;
			}
		 }
	}
	if( FOnMBusStartup != NULL )
		FOnMBusStartup( Host );
}
//------------------------------------------------------------------------------
void __fastcall TMessageObject::OnAppChanged( BOOL IsOnline, const UFC::AnsiString& Host, const UFC::AnsiString& App )
{
	UFC::AnsiString  KeyStr;
	ProcessStateObj* InfoObj;

	MakeKeyString( KeyStr, Host, App );
	if( ( InfoObj = FProcesss.GetObjectByKey( KeyStr )) != NULL )
	{
		InfoObj->Alive( IsOnline );
		if( FMonioringSet.Exists( KeyStr ) == TRUE )///< Monitoring it ?
		{
			if( IsOnline == TRUE )///< Trigger event.
				TriggerOnStartup( Host, App );
			else
				TriggerOnStopped( Host, App );
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TMessageObject::OnStatChange( BOOL IsOnline, MTree* Data )
{
	UFC::AnsiString  App,Host;

	if( Data->get( "APP", App ) && Data->get( "HOST", Host ) )
	{
		if( App.AnsiCompare("MBus") == 0 ) ///< MBus State changed.
		{
			if( IsOnline == FALSE ) ///< A MBus offline.
				OnMBusOffline( Host );
			else                    ///< A MBus online.
				OnMBusOnline( Host );
		}
		else ///< Process state changed.
			OnAppChanged( IsOnline, Host, App );
	}
}
//---------------------------------------------------------------------------
const String& __fastcall TMessageObject::GetErrorMessage( void )
{
	return FErrorMessage;
}
//---------------------------------------------------------------------------
void  __fastcall TMessageObject::SetRecvDelay( int ms )
{
	FApp->SetRecvDelayMS( ms );
}
//---------------------------------------------------------------------------

