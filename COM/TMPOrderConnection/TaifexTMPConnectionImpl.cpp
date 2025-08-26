// TAIFEXCONNECTIONIMPL : Implementation of TTaifexConnectionImpl (CoClass: TaifexConnection, Interface: ITaifexConnection)
#include <vcl.h>
#pragma hdrstop

#include "TaifexTMPConnectionImpl.h"
#include "MessageDataFormat.h"
/////////////////////////////////////////////////////////////////////////////
// TTaifexConnectionImpl
//------------------------------------------------------------------------------
TTaifexTMPConnectionImpl::TTaifexTMPConnectionImpl()
:FConnection( NULL )
{

}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::Create(BSTR AppName)
{
	AnsiString AnsiAppName( AppName );

	if( SysStringLen( AppName ) <= 2 )
		AnsiAppName.printf( "MApp@%s.%d", UFC::Hostname, GetTickCount() );
	FConnection = new TTaifexTMPConnection( _Module.m_hInst, _Module.m_hInstResource, AnsiAppName.c_str(), this, TRUE );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::Destroy()
{
	if( FConnection != NULL )
	{
		delete FConnection;
		FConnection = NULL;
	}
    return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::Connect(BSTR RemoteIP, long RemotePort)
{
	if( FConnection != NULL )
	{
		AnsiString AnsiRemoteIP( RemoteIP );
		FConnection->Connect( AnsiRemoteIP.c_str(), RemotePort  );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::Disconnect()
{
	if( FConnection != NULL )
	{
		FConnection->Disconnect();
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::Logon(BSTR ID, BSTR PASSWD, BSTR ACCOUNT, ConnectionTypeEnum Type)


{
	if( FConnection != NULL )
	{
		AnsiString AnsiID( ID );
		AnsiString AnsiPASSWD( PASSWD );
		AnsiString AnsiACCOUNT( ACCOUNT );
		int        COMVersion = 50000;
		DWORD n = GetFileVersionInfoSize( "TMPOrderConnection.dll", NULL);

		if( n > 0 )
		{
			char *pBuf = new char[ n ];
			VS_FIXEDFILEINFO *vsinfo;
			UINT Len;

			GetFileVersionInfo( "TMPOrderConnection.dll", 0, n, pBuf );
			if( VerQueryValue( pBuf, "\\", (void **)&vsinfo, &Len))
			{
				AnsiString VersionString;
				int Version  = HIWORD(vsinfo->dwFileVersionMS);
				int MVersion = LOWORD(vsinfo->dwFileVersionMS);
				int Build    = LOWORD(vsinfo->dwFileVersionLS);
				COMVersion = Version*10000 + (MVersion%100)*100 + Build%100;
			}
			delete [] pBuf;
		}
		FConnection->Logon( COMVersion, AnsiID.c_str(), AnsiPASSWD.c_str(), AnsiACCOUNT.c_str(),(ConnectionType)Type );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::NewOrder(INewOrderMessage* Msg)
{
	try
	{
		if( FConnection != NULL )
		{
			VARIANT MessageInstance;
			Msg->get_Instance( &MessageInstance );
			TNewOrderMessage* pNewOrderMessage = (TNewOrderMessage*)(MessageInstance.byref);
			if( pNewOrderMessage != NULL )
			{
				AnsiString Symbol( pNewOrderMessage->GetSymbol() );
				if( Symbol.Length() < 3 )
					return Error("Missing field Symbol", IID_ITaifexTMPConnection);
				FConnection->NewOrder( pNewOrderMessage );
			}
		}

	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_ITaifexTMPConnection);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::ReplaceOrder(IReplaceOrderMessage* Msg)
{
	if( FConnection != NULL )
	{
		VARIANT MessageInstance;
		Msg->get_Instance( &MessageInstance );
		TReplaceOrderMessage* pReplaceOrderMessage = (TReplaceOrderMessage*)(MessageInstance.byref);
		if( pReplaceOrderMessage != NULL )
		{
			FConnection->ReplaceOrder( pReplaceOrderMessage );
		}
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::CancelOrder(ICancelOrderMessage* Msg)
{
	if( FConnection != NULL )
	{
		VARIANT MessageInstance;
		Msg->get_Instance( &MessageInstance );
		TCancelOrderMessage* pCancelOrderMessage = (TCancelOrderMessage*)(MessageInstance.byref);
		if( pCancelOrderMessage != NULL )
		{
			FConnection->CancelOrder( pCancelOrderMessage );
		}
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::Quote(IQuoteMessage* Msg)
{
	if( FConnection != NULL )
	{
		VARIANT MessageInstance;
		Msg->get_Instance( &MessageInstance );
		TQuoteMessage* pQuoteMessage = (TQuoteMessage*)(MessageInstance.byref);
		if( pQuoteMessage != NULL )
		{
			FConnection->Quote( pQuoteMessage );
		}
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::QuoteCancel(IQuoteCancelMessage* Msg)
{
	if( FConnection != NULL )
	{
		VARIANT MessageInstance;
		Msg->get_Instance( &MessageInstance );
		TQuoteCancelMessage* pQuoteCancelMessage = (TQuoteCancelMessage*)(MessageInstance.byref);
		if( pQuoteCancelMessage != NULL )
		{
			FConnection->QuoteCancel( pQuoteCancelMessage );
		}
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::QuoteRequest(IQuoteRequestMessage* Msg)
{
	if( FConnection != NULL )
	{
		VARIANT MessageInstance;
		Msg->get_Instance( &MessageInstance );
		TQuoteRequestMessage* pQuoteRequestMessage = (TQuoteRequestMessage*)(MessageInstance.byref);
		if( pQuoteRequestMessage != NULL )
		{
			FConnection->QuoteRequest( pQuoteRequestMessage );
		}
	}
	return S_OK;
}
//------------------------------------------------------------------------------
void TTaifexTMPConnectionImpl::OnConnected( void )
{
	Fire_OnConnected();
}
//------------------------------------------------------------------------------
void TTaifexTMPConnectionImpl::OnDisconnected( void )
{
	Fire_OnDisconnected();
}
//------------------------------------------------------------------------------
void TTaifexTMPConnectionImpl::OnLogonReply( const char* ReplyString, LogonResult Result, int CID )
{
	WideString WideReplyString( ReplyString );

	Fire_OnLogonReply( WideReplyString.c_bstr(), (LogonResultEnum)Result, CID );
}
//------------------------------------------------------------------------------
void TTaifexTMPConnectionImpl::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup )
{
	VARIANT MessageInstance;
	TCOMIExecutionReportMessage MessageToSend = CoExecutionReportMessage::Create();
	MessageToSend->get_Instance( &MessageInstance );
	TExecutionReportMessage* pExecutionReportMessage = (TExecutionReportMessage*)(MessageInstance.byref);
	pExecutionReportMessage->Attach( Msg->GetMTree() );

	Fire_OnExecutionReport( MessageToSend, (ExecDupEnum)PossDup );
}
//------------------------------------------------------------------------------
void TTaifexTMPConnectionImpl::OnRecoverFinished( int Count )
{
	Fire_OnRecoverFinished( Count );
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::Recover(BSTR BeginTime, RecoverTypeEnum Type,
          RecoverMarketEnum Market)
{
	if( FConnection != NULL )
	{
		AnsiString AnsiBeginTime( BeginTime );
		///< Recover from Begin time
		FConnection->Recover( AnsiBeginTime.c_str(), (RecoverDataType) Type, (RecoverMarket) Market );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::get_ClearMemberID(BSTR* Value)
{
	if( FConnection != NULL )
	{
		WideString CMID( FConnection->GetCMID().c_str() );
		*Value = CMID.Copy();
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::get_IsAdministrator(VARIANT_BOOL* Value)
{
	if( FConnection != NULL )
		*Value = FConnection->IsAdmin();
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::set_ClearMemberID(BSTR Value)
{
	if( FConnection != NULL )
	{
		AnsiString CM( Value );
		FConnection->SetCMID( CM.c_str() );
	}
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::SetLanguage(MessageLanguageEnum Lang)
{
	if( FConnection != NULL )
		FConnection->SetLanguage((MessageLanguage) Lang );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::GenerateUniqueID(MessageTypeEnum Type, long* NID)

{
	if( FConnection != NULL )
		*NID = FConnection->GenerateNID( (nsOrderMessageDefine::MessageTypeEnum) Type );
	else
		*NID = 0;
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::get_BrokerID(BSTR* Value)
{
	if( FConnection != NULL )
	{
		WideString BrokerID( FConnection->GetBrokerID(  ).c_str() );
		*Value = BrokerID.Copy();
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::set_BrokerID(BSTR Value)
{
	if( FConnection != NULL )
	{
		AnsiString BrokerID( Value );
		FConnection->SetBrokerID( BrokerID.c_str() );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::SetDebugLog(BSTR FileName)
{
	if( FConnection != NULL )
	{
		AnsiString LogFileName( FileName );
		FConnection->SetLogFile( LogFileName.c_str() );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::OrderStatusRequest(IOrderStatusRequestMessage* Msg)

{
	if( FConnection != NULL )
	{
		VARIANT MessageInstance;
		Msg->get_Instance( &MessageInstance );
		TOrderStatusRequest* pOrderStatusRequest = (TOrderStatusRequest*)(MessageInstance.byref);
		if( pOrderStatusRequest != NULL )
			FConnection->OrderStatusRequest( pOrderStatusRequest );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexTMPConnectionImpl::Recover2(BSTR BeginTime, BSTR EndTime, RecoverTypeEnum Type,
          RecoverMarketEnum Market)
{
	if( FConnection != NULL )
	{
		AnsiString AnsiBeginTime( BeginTime );
		AnsiString AnsiEndTime( EndTime );
		///< Recover from Begin time to End time.
		FConnection->Recover( AnsiBeginTime.c_str(),AnsiEndTime.c_str(),(RecoverDataType) Type, (RecoverMarket) Market );
	}
	return S_OK;
}
//------------------------------------------------------------------------------

