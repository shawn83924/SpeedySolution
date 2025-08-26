// ---------------------------------------------------------------------------
// MDTRANSPORTIMPL : Implementation of TMdTransportImpl (CoClass: MdTransport, Interface: IMdTransport)
// ---------------------------------------------------------------------------
#include <vcl.h>
#include <Registry.hpp>
#pragma hdrstop
#include "MdMessageImpl.H"
#include "MdTransportImpl.h"
#include "TLicenseKey.h"
#include "NetUtility.h"
//---------------------------------------------------------------------------
UFC::PHashedSet<UFC::AnsiString>  FSubjects;
bool                              FUnlock = true;
//---------------------------------------------------------------------------
void InitSubjectTable( const AnsiString& Vendor )
{
	TRegistry*   Reg = new TRegistry();
	TStringList* Subjects = new TStringList();
	AnsiString   RegKey;

	RegKey.printf( "Software\\MDBS\\MBusAPI\\%s", Vendor.c_str() );
	Reg->RootKey = HKEY_LOCAL_MACHINE;
	Reg->OpenKey( RegKey ,true );
	Reg->GetValueNames( Subjects );
	for( int i = 0; i< Subjects->Count; i ++ )
	{
		AnsiString Subject = Subjects->Strings[i].c_str();
		AnsiString Str,Result;
		UFCType::UInt8 Buffer[16];

		Str.printf( "[%s]-[%s]-[MBusAPI]", Vendor.UpperCase().c_str(), Subject.UpperCase().c_str() );
		UFC::MD5 KeyMD5( (UFCType::UInt8*)(Str.c_str()),(UFCType::Int32) Str.Length() );
		memcpy( Buffer, KeyMD5.GetCode(), 16 );

		UFCType::Int32 n1 = abs( UFC::NInt32::ToInt32( Buffer )      % 10000 );
		UFCType::Int32 n2 = abs( UFC::NInt32::ToInt32( Buffer + 4 )  % 10000 );
		UFCType::Int32 n3 = abs( UFC::NInt32::ToInt32( Buffer + 8 )  % 10000 );
		UFCType::Int32 n4 = abs( UFC::NInt32::ToInt32( Buffer + 12 ) % 10000 );
		Result.printf( "%04d-%04d-%04d-%04d", n1, n2, n3, n4 );

		if( Result.AnsiCompare( Reg->ReadString( Subject ) ) == 0 )
		{
			printf( "Vendor[%s] can use subject[%s].\n", Vendor.c_str(), Subject.c_str() );
			FSubjects.Add( Subject.c_str() );
		}
		else
			printf( "Vendor[%s] can't use subject[%s] bad key.\n", Vendor.c_str(), Subject.c_str() );

	}
	Reg->CloseKey();
	delete Subjects;
	delete Reg;
	if( FSubjects.Exists( "all" ) == TRUE )
		FUnlock = true;
}
//---------------------------------------------------------------------------
bool CanHandleSubject( const AnsiString& Subject )
{
	if( FUnlock == true )
		return true;
	if( FSubjects.Exists(Subject.c_str()) == TRUE )
		return true;
	return false;
}
// ---------------------------------------------------------------------------
// TMdTransportImpl
// ---------------------------------------------------------------------------
__fastcall TMdTransportImpl::TMdTransportImpl()
:FThread( NULL )
,FApp( NULL )
,FInstance( CastToVariant<MApp*>(NULL) )
,FRemoteIP( "" )
,FRemotePort( 0 )
,FAdminSubscriber( NULL )
{
	UFC::AnsiString HostName    = UFC::PSocket::GetLocalHostName();
	FLocalHostName              = WideString( HostName.c_str() );
	FAppName                    = WideString( "MBusCOMAPIApp" );
	UFC::BufferedLog::SetPrintToStdout( FALSE );
	UFC::BufferedLog::SetDebugMode( FALSE);
	FHandler = new EventHandler( this );
}
// ---------------------------------------------------------------------------
// TMdTransportImpl
// ---------------------------------------------------------------------------
__fastcall TMdTransportImpl::TMdTransportImpl(const System::_di_IInterface Controller)
:inherited(Controller)
,FThread( NULL )
,FApp( NULL )
,FInstance( CastToVariant<MApp*>(NULL) )
,FRemoteIP( "" )
,FRemotePort( 0 )
,FAdminSubscriber( NULL )
{
	UFC::AnsiString HostName    = UFC::PSocket::GetLocalHostName();
	FLocalHostName              = WideString( HostName.c_str() );
	FAppName                    = WideString( "MBusCOMAPIApp" );
	UFC::BufferedLog::SetPrintToStdout( FALSE );
	UFC::BufferedLog::SetDebugMode( FALSE);
	FHandler = new EventHandler( this );
}
// ---------------------------------------------------------------------------
// TMdTransportImpl
// ---------------------------------------------------------------------------
__fastcall TMdTransportImpl::TMdTransportImpl(Comobj::TComObjectFactory* Factory,
									  const System::_di_IInterface Controller)
:inherited(Factory, Controller)
,FThread( NULL )
,FApp( NULL )
,FInstance( CastToVariant<MApp*>(NULL) )
,FRemoteIP( "" )
,FRemotePort( 0 )
,FAdminSubscriber( NULL )
{
	UFC::AnsiString HostName    = UFC::PSocket::GetLocalHostName();
	FLocalHostName              = WideString( HostName.c_str() );
	FAppName                    = WideString( "MBusCOMAPIApp" );
	UFC::BufferedLog::SetPrintToStdout( FALSE );
	UFC::BufferedLog::SetDebugMode( FALSE);
	FHandler = new EventHandler( this );

}
// ---------------------------------------------------------------------------
void __fastcall TMdTransportImpl::EventSinkChanged(const System::_di_IInterface EventSink)
{
	if( EventSink != NULL )
	{
		IMdTransportEventsDisp* NewEvents = new IMdTransportEventsDisp();
		FEventList.push_back( NewEvents );
		NewEvents->Bind( EventSink );
	}
}
// ---------------------------------------------------------------------------
// TMdTransportImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TMdTransportImpl>(Comserv::GetComServer(),
						   __classid(TMdTransportImpl),
						   CLSID_MdTransport,
						   Comobj::ciMultiInstance,
						   Comobj::tmApartment);
}
#pragma startup createFactory 32
//---------------------------------------------------------------------------
void TMdTransportImpl::StartMAppThread( long TimeoutSec )
{
	StopMAppThread(); ///< If MApp thread already exists, stop and delete it.
	FThread = new TThreadApp( FApp ); ///< Create a new MApp thread

	FThread->Wait( TimeoutSec*1000 );
	for( int i =0;i <10; i++ )
		UFC::SleepMS( 10 );
	if( FApp->Connect( TimeoutSec ) == FALSE )
		throw( UFC::SocketException( "Connect failed!" ));
}
//---------------------------------------------------------------------------
void TMdTransportImpl::StopMAppThread()
{
	if( FThread != NULL )
	{
        TThreadApp* TempDelete = FThread;
		FThread = NULL;
        delete TempDelete;
	}
}
//---------------------------------------------------------------------------
// Implement  MAppListener
//---------------------------------------------------------------------------
void TMdTransportImpl::OnMAppConnected()
{
	FireOnConnected( this );
}
//---------------------------------------------------------------------------
void TMdTransportImpl::OnMAppDisconnected()
{
	ProcessStateObj* Del = FProcesss.First();
	while( Del != NULL )
	{
		delete Del;
		Del = FProcesss.Next();
	};
	FProcesss.Clear();
	FireOnDisconnected( this );
}
//---------------------------------------------------------------------------
void TMdTransportImpl::OnMAppError(PMAppError Error)
{
    WideString ErrorMessage;

	switch( Error )
    {
  	    case meSendConnectedAckFailed: ErrorMessage = "Send connected ack failed.";break;
        case meUnregistFailed:         ErrorMessage = "Unregister subject failed.";break;
        case meReadDataFailed:         ErrorMessage = "Receive message failed.";break;
        case meSendFailed:             ErrorMessage = "Send message failed.";break;
        case meSendFailedNotConnected: ErrorMessage = "Not connected.";break;
		default :                      ErrorMessage = "Unknown socket error.";break;
    }
	FireOnError( this, ErrorMessage.c_bstr() );
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdTransportImpl::Create(BSTR Vendor, BSTR Name, BSTR IP, long Port,
          VARIANT_BOOL IsGUIApp)
{
	HRESULT hResult;
	try
	{
		printf( "Create MApp.\n" );
		if( FApp == NULL )
		{
			///< Already initialize WinSock2 library in UFC
			AnsiString Host( IP );
			AnsiString AppName( Name );
			AnsiString VendorID( Vendor );

			if( IsGUIApp == VARIANT_TRUE )
				FApp = new MWinApp( HInstance );
			else
				FApp = new MApp( );
			FApp->SetHost( Host.c_str() );
			FApp->SetPort( Port );
			FApp->SetAppName( AppName.c_str() );
			FApp->SetListener( FHandler );
			FInstance   = CastToVariant<MApp*>(FApp);
			FAppName    = WideString( Name );
			FRemoteIP   = WideString( IP );
			FRemotePort = Port;

			FAdminSubscriber = new MSubscriber( "Subscriber_MBUS_ALL", "MBUS", "all" ); ///< Create a new Subscriber.
			FAdminSubscriber->addListener( FHandler );                       ///< Tell MApp to add this Subscriber.
			FApp->AddSubscriber( FAdminSubscriber );
			printf( "MApp created.\n" );
			InitSubjectTable( VendorID );
		}
		hResult = S_OK;
	}
	catch( UFC::SocketException &e )
	{
		FireOnError( this, WideString( e.what() ).c_bstr() );
		hResult = S_FALSE;
	}
	catch(...)
	{
		FireOnError( this, WideString( "Unknown error" ).c_bstr() );
		hResult = S_FALSE;
	}
	return hResult;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdTransportImpl::Destroy()
{
	if( FApp != NULL )
	{
		if( FAdminSubscriber != NULL )
		{
			FApp->DeleteSubscriber( FAdminSubscriber ); ///< Tell MApp to remove this Subscriber.
			delete FAdminSubscriber;                    ///< Delete the Subscriber object;
			FAdminSubscriber = NULL;
		}
        FApp->SetListener( NULL );
		StopMAppThread();
        delete FApp;
    }
    FApp        = NULL;
    FInstance   = CastToVariant<MApp*>(NULL);
	FAppName    = WideString( "MBusCOMAPIApp" );
    FRemoteIP   = WideString( "" );
    FRemotePort = 0;
  	return S_OK;
}
//---------------------------------------------------------------------------
void TMdTransportImpl::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* Data )
{
	UFCType::Int32 CMD;

	if( Data->get( "CMD", CMD ) == TRUE )
	{
		switch( CMD )
		{
			case ADMIN_REPLY_PROCESS: CheckStat( Data ); break;
			case ADMIN_APP_OFFLINE:   OnStatChange( FALSE, Data ); break;
			case ADMIN_APP_ONLINE:    OnStatChange( TRUE, Data );  break;
			case ADMIN_APP_FIRST_ONE: FireOnProcessConnected( this, Mbusapi_tlb::ProcessStatusEnum::psFirstTime ); break;
			case ADMIN_APP_EXISTS:    FireOnProcessConnected( this, Mbusapi_tlb::ProcessStatusEnum::psAlreadyExists ); break;
		}
	}
}
//---------------------------------------------------------------------------
void TMdTransportImpl::CheckStat( MTree* Data )
{
	int              Count;
	UFC::AnsiString  Host,App,KeyStr;
	ProcessStateObj* InfoObj;

	if( Data->get( "COUNT", Count ) && Data->get( "HOST", Host ) )
	{
		for( register int i = 0; i < Count; i++ )
		{
			 UFC::AnsiString FieldName( i );

			 if( Data->get( FieldName, App ) == TRUE )
			 {
				 KeyStr.Printf( "%s%s", Host.c_str(),  App.c_str() );
				 if( ( InfoObj = FProcesss.GetObjectByKey( KeyStr )) != NULL )
				 {
					 if( InfoObj->IsAlive() == FALSE )
					 {
						 InfoObj->Alive( TRUE );
						 FireOnProcessStartup( this,WideString(Host.c_str()).c_bstr(), WideString(App.c_str()).c_bstr());
					 }
				 }
			 }
		}
    }
}
//------------------------------------------------------------------------------
void TMdTransportImpl::OnStatChange( BOOL IsOnline, MTree* Data )
{
	UFC::AnsiString  App,Host,KeyStr,StopApp;
	ProcessStateObj* InfoObj;

	if( Data->get( "APP", App ) && Data->get( "HOST", Host ) )
	{
		if( App.AnsiCompare("MBus") == 0 ) ///< MBus State changed.
		{
			if( IsOnline == FALSE ) ///< A MBus offline.
			{
				///< All MApp client connect to this MBus should be offline.
				for( int i = 0; i < FProcesss.ItemCount();i++)
				{
				   if( FProcesss.GetItem( i, KeyStr, InfoObj ) )
				   {   ///< Is this process connect to that MBus server and is alive?
					   if( KeyStr.AnsiNCompare( Host.c_str(), Host.Length() ) == 0 && InfoObj->IsAlive())
					   {
						   StopApp = KeyStr.SubString( Host.Length(), KeyStr.Length() - Host.Length() );
						   InfoObj->Alive( FALSE );
						   FireOnProcessStopped( this,WideString(Host.c_str()).c_bstr(), WideString(StopApp.c_str()).c_bstr() );
					   }
				   }
				}
			}
			else  ///< A MBus online, Ask MBus the processes state.
			{
				MTree Data;
				Data.append( "CMD", ADMIN_ASK_PROCESS );
				FApp->send_by_key( "MBUS", Host.c_str(), Data, FALSE );
			}
		}
		else
		{
			KeyStr.Printf( "%s%s", Host.c_str(),  App.c_str() );
			if( ( InfoObj = FProcesss.GetObjectByKey( KeyStr )) != NULL )
			{
				InfoObj->Alive( IsOnline );
				if( IsOnline )	///< Trigger event.
					FireOnProcessStartup( this,WideString(Host.c_str()).c_bstr(), WideString(App.c_str()).c_bstr());
				else
					FireOnProcessStopped( this,WideString(Host.c_str()).c_bstr(), WideString(App.c_str()).c_bstr());
			}
		}
	}
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdTransportImpl::Connect(long Timeout)
{
	HRESULT hResult;
	try
    {
		StartMAppThread( Timeout ); ///< Create a new MApp thread and run.
	    hResult = S_OK;
    }
    catch( UFC::SocketException &e )
    {
		FireOnError( this, WideString( e.what() ).c_bstr() );
		StopMAppThread();
		hResult = S_FALSE;
	}
	catch(...)
	{
		FireOnError( this, WideString( "Unknown error" ).c_bstr() );
        StopMAppThread();
        hResult = S_FALSE;
    }
  	return hResult;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdTransportImpl::Disconnect()
{
    StopMAppThread();
    return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdTransportImpl::get_AppName(BSTR* Value)
{
    *Value = FAppName.c_bstr();
	return S_OK;
};
//---------------------------------------------------------------------------
STDMETHODIMP TMdTransportImpl::get_Host(BSTR* Value)
{
	*Value = FRemoteIP.c_bstr();
	return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdTransportImpl::get_Port(long* Value)
{
    *Value = FRemotePort;
    return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdTransportImpl::IsConnected(VARIANT_BOOL* Value)
{
    if( FApp != NULL )
        *Value = FApp->IsConnected();
    else
        *Value = false;
	return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdTransportImpl::IsValid(VARIANT_BOOL* Value)
{
  	*Value = ( FApp != NULL );
	return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdTransportImpl::Send(BSTR Subject, BSTR Key, IMdMessage* Msg)
{
  	if( (FApp != NULL) && (Msg != NULL) )
   	{
		if( FApp->IsConnected() && Msg->IsValid()  )
		{
			AnsiString AnsiSubject( Subject );
			AnsiString AnsiKey( Key );
			VARIANT    MessageInstance;

			if( CanHandleSubject( AnsiSubject ) == false )
				return Error("No right to use this subject", IID_IMdTransport );
			Msg->GetMessageInstance( &MessageInstance );
			MTree* pTree = CastToObject<MTree*>( &MessageInstance );
			if( FApp->send_by_key( AnsiSubject.c_str(), AnsiKey.c_str(), *pTree, FALSE ) == TRUE )
				return S_OK;
		}
  	}
  	return S_FALSE;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdTransportImpl::GetTransportInstance(VARIANT* Value)
{
	*Value = FInstance;
	return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdTransportImpl::get_LocalHostname(BSTR* Value)
{
    *Value = FLocalHostName.c_bstr();
    return S_OK;
};
//---------------------------------------------------------------------------
STDMETHODIMP TMdTransportImpl::get_SocketFD(long* Value)
{
    *Value = FApp->GetSocketHandle();
    return S_OK;
};
//---------------------------------------------------------------------------
STDMETHODIMP TMdTransportImpl::get_LocalIPAddress(BSTR* Value)
{
    UFC::AnsiString IP = FApp->GetLocalIPAddress();
    FLocalIPAddress = WideString( IP.c_str() );
    *Value = FLocalIPAddress.c_bstr();
    return S_OK;
};
//---------------------------------------------------------------------------
STDMETHODIMP TMdTransportImpl::AddMonitoringProcess(BSTR Host, BSTR AppName)
{
	AnsiString Hostname( Host );
	AnsiString ApplicationName( AppName );
	UFC::AnsiString KeyString;

	KeyString.Printf( "%s%s", Hostname.c_str(),  ApplicationName.c_str() );
	FProcesss.Add( KeyString, new ProcessStateObj( Hostname.c_str(), ApplicationName.c_str()) );
	///< Ask to MBus
	MTree Data;
	Data.append( "CMD", ADMIN_ASK_PROCESS );
	FApp->send_by_key( "MBUS", Hostname.c_str(), Data, FALSE );
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT TMdTransportImpl::FireOnConnected(Mbusapi_tlb::IMdTransport* Transport)
{
	for( register size_t i=0;i<  FEventList.size(); i ++ )
		 FEventList[i]->OnConnected( Transport );
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT TMdTransportImpl::FireOnDisconnected(Mbusapi_tlb::IMdTransport* Transport)
{
	for( register size_t i=0;i<  FEventList.size(); i ++ )
		FEventList[i]->OnDisconnected( Transport );
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT TMdTransportImpl::FireOnError(Mbusapi_tlb::IMdTransport* Transport, BSTR ErrorMessage)
{
	for( register size_t i=0;i<  FEventList.size(); i ++ )
		FEventList[i]->OnError( Transport, ErrorMessage );
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT TMdTransportImpl::FireOnProcessConnected(Mbusapi_tlb::IMdTransport* Transport, Mbusapi_tlb::ProcessStatusEnum IsTheFirstOne)
{
	for( register size_t i=0;i<  FEventList.size(); i ++ )
		FEventList[i]->OnProcessConnected( Transport, IsTheFirstOne );
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT TMdTransportImpl::FireOnProcessStartup(Mbusapi_tlb::IMdTransport* Transport, BSTR Host,	BSTR AppName)
{
	for( register size_t i=0;i<  FEventList.size(); i ++ )
		FEventList[i]->OnProcessStartup( Transport, Host, AppName );
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT TMdTransportImpl::FireOnProcessStopped(Mbusapi_tlb::IMdTransport* Transport, BSTR Host,	BSTR AppName)
{
	for( register size_t i=0;i<  FEventList.size(); i ++ )
		FEventList[i]->OnProcessStopped( Transport, Host, AppName );
	return S_OK;
}
//---------------------------------------------------------------------------


