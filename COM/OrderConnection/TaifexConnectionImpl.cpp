// TAIFEXCONNECTIONIMPL : Implementation of TTaifexConnectionImpl (CoClass: TaifexConnection, Interface: ITaifexConnection)
#include <vcl.h>
#pragma hdrstop

#include "TAIFEXCONNECTIONIMPL.H"
#include "MessageDataFormat.h"
//------------------------------------------------------------------------------
bool IsNewVersion = false;
////////////////////////////////////////////////////////////////////////////////
// TTaifexConnectionImpl
//------------------------------------------------------------------------------
TTaifexConnectionImpl::TTaifexConnectionImpl()
:FConnection( NULL )
{

}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::Create(BSTR AppName)
{
	return Create2( AppName );
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::Create2(BSTR AppName)
{
	AnsiString AnsiAppName( AppName );

	IsNewVersion = true;
	if( SysStringLen( AppName ) <= 2 )
		AnsiAppName.printf( "MApp@%s.%d", UFC::Hostname, GetTickCount() );
	FConnection = new TTaifexConnection( _Module.m_hInst,
										 _Module.m_hInstResource,
										 AnsiAppName.c_str(),
										 this,
										 atSpeedy,
										 TRUE );///< Is GUI App
	FConnection->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
	FConnection->SetTAIFEXQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );
	FConnection->SetTSEDataFormatResourceID( IDR_TSE_T010,IDR_TSE_O010,IDR_TSE_P010, IDR_TSE_T020, IDR_TSE_O020, IDR_TSE_P020, IDR_TSE_R030, "INI" );
	FConnection->SetTSEDataFormatResourceID2( IDR_TSE_A010, IDR_TSE_V010,IDR_TSE_E010,IDR_TSE_Ex010, IDR_TSE_A020, IDR_TSE_V020, IDR_TSE_E020,IDR_TSE_Ex020, "INI" );
	FConnection->SetOTCDataFormatResourceID( IDR_OTC_T010,IDR_OTC_O010,IDR_OTC_P010, IDR_OTC_T020, IDR_OTC_O020, IDR_OTC_P020, IDR_OTC_R030, "INI" );
	FConnection->SetOTCDataFormatResourceID2( IDR_OTC_V010, IDR_OTC_E010, IDR_OTC_Ex010, IDR_OTC_V020, IDR_OTC_E020, IDR_OTC_Ex020, "INI" );
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::Create3(BSTR AppName, APIEnum UseAPI, BSTR* Message,
          VARIANT_BOOL* Result)

{
	if( UseAPI  == Orderconnection_tlb::APIEnum::aeCTP   ||
		UseAPI  == Orderconnection_tlb::APIEnum::aeFemas ||
		UseAPI  == Orderconnection_tlb::APIEnum::aeKSFt  ||
		UseAPI  == Orderconnection_tlb::APIEnum::aeKSOpt ||
		UseAPI  == Orderconnection_tlb::APIEnum::aeHSctp ||
		UseAPI  == Orderconnection_tlb::APIEnum::aeHSufx ||
		UseAPI  == Orderconnection_tlb::APIEnum::aeKNctp  )
	{
		if( TTaifexConnection::SupportAPI( (APIType) UseAPI ) == TRUE )
		{
			AnsiString AnsiAppName( AppName );

			if( SysStringLen( AppName ) <= 2 )
				AnsiAppName.printf( "MApp@%s.%d", UFC::Hostname, GetTickCount() );
			FConnection = new TTaifexConnection( _Module.m_hInst,
												 _Module.m_hInstResource,
												 AnsiAppName.c_str(),
												 this,
												 (APIType) UseAPI,
												 TRUE );///< Is GUI App
			WideString Msg( "Create object OK" );
			*Message = Msg.Copy();
			*Result  = VARIANT_TRUE;
			return S_OK;
		}
		else
		{
			WideString Msg( "API not supported" );
			*Message = Msg.Copy();
			*Result  = VARIANT_FALSE;
			return S_OK;
		}
	}
	else if( UseAPI  == Orderconnection_tlb::APIEnum::aeSpeedy )
	{
		WideString Msg( "Create Speedy API object OK" );
		*Message = Msg.Copy();
		*Result  = VARIANT_TRUE;
		return Create2( AppName );
	}
	else
	{
		WideString Msg( "Unknown API type" );
		*Message = Msg.Copy();
		*Result  = VARIANT_FALSE;
		return S_OK;
	}
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::Destroy()
{
	if( FConnection != NULL )
	{
		delete FConnection;
		FConnection = NULL;
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::Connect(BSTR RemoteIP, long RemotePort)
{
	if( FConnection != NULL )
	{
		AnsiString AnsiRemoteIP( RemoteIP );
		FConnection->Connect( AnsiRemoteIP.c_str(), RemotePort, 10  );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::Connect2(BSTR RemoteIP, long RemotePort, long TimeoutSec)










{
	if( FConnection != NULL )
	{
		AnsiString AnsiRemoteIP( RemoteIP );
		FConnection->Connect( AnsiRemoteIP.c_str(), RemotePort, TimeoutSec  );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::Disconnect()
{
	if( FConnection != NULL )
	{
		FConnection->Disconnect();
	}
	return S_OK;
}
//------------------------------------------------------------------------------
int TTaifexConnectionImpl::GetVersion( const char* Binary, bool IsProxy )
{
	int   COMVersion = 20000;
	DWORD n = GetFileVersionInfoSize( Binary, NULL);

	if( IsProxy == true )
		COMVersion = 2000000;

	if( n > 0 )
	{
		char *pBuf = new char[ n ];
		VS_FIXEDFILEINFO *vsinfo;
		UINT Len;

		GetFileVersionInfo( Binary, 0, n, pBuf );
		if( VerQueryValue( pBuf, "\\", (void **)&vsinfo, &Len))
		{
			AnsiString VersionString;
			int Version  = HIWORD(vsinfo->dwFileVersionMS);
			int MVersion = LOWORD(vsinfo->dwFileVersionMS);
			int Release  = HIWORD(vsinfo->dwFileVersionLS);
			int Build    = LOWORD(vsinfo->dwFileVersionLS);

			if( IsProxy == true ) ///< For Proxy Format: MM.mm.RR.BB
				COMVersion = Version*1000000 + (MVersion%100)*10000 + (Release%100)*100 + Build%100;
			else 				  ///< For Gateway Format: MM.mm.BB
				COMVersion = Version*10000 + (MVersion%100)*100 + Build%100;
		}
		delete [] pBuf;
	}
	return COMVersion;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::LogonProxy(BSTR ID, BSTR Password, BSTR Account,
          BSTR Token1, BSTR Token2)
{
	if( FConnection != NULL )
	{
		AnsiString AnsiID( ID );
		AnsiString AnsiPassword( Password );
		AnsiString AnsiACCOUNT( Account );
		AnsiString AnsiToken1( Token1 );
		AnsiString AnsiToken2( Token2 );
		int        COMVersion = GetVersion( "OrderConnection.dll", true );

		FConnection->SpeedyProxyLogon( AnsiID.c_str(),
									   AnsiPassword.c_str(),
									   AnsiACCOUNT.c_str(),
									   AnsiToken1.c_str(),
									   AnsiToken2.c_str(),
									   ctBoth,
									   COMVersion );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::Logon(BSTR ID, BSTR PASSWD, BSTR ACCOUNT, ConnectionTypeEnum Type)










{
	if( FConnection != NULL )
	{
		AnsiString AnsiID( ID );
		AnsiString AnsiPASSWD( PASSWD );
		AnsiString AnsiACCOUNT( ACCOUNT );
		int        COMVersion = GetVersion( "OrderConnection.dll", false );

		FConnection->Logon( AnsiID.c_str(),
							AnsiPASSWD.c_str(),
							AnsiACCOUNT.c_str(),
							(ConnectionType)Type,
							COMVersion );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::NewOrder(INewOrderMessage* Msg)
{
	try
	{
		if( FConnection != NULL )
		{
			VARIANT MessageInstance;
			Msg->get_Instance( &MessageInstance );
			TNewOrderMessage* pNewOrderMessage = (TNewOrderMessage*)(MessageInstance.byref);
			if( pNewOrderMessage != NULL )
				FConnection->NewOrder( pNewOrderMessage );
		}
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_ITaifexConnection);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::ReplaceOrder(IReplaceOrderMessage* Msg)
{
    if( FConnection != NULL )
    {
		VARIANT MessageInstance;
		Msg->get_Instance( &MessageInstance );
		TReplaceOrderMessage* pReplaceOrderMessage = (TReplaceOrderMessage*)(MessageInstance.byref);

		if( pReplaceOrderMessage != NULL )
			FConnection->ReplaceOrder( pReplaceOrderMessage );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::CancelOrder(ICancelOrderMessage* Msg)
{
	if( FConnection != NULL )
	{
		VARIANT MessageInstance;
		Msg->get_Instance( &MessageInstance );
		TCancelOrderMessage* pCancelOrderMessage = (TCancelOrderMessage*)(MessageInstance.byref);

		if( pCancelOrderMessage != NULL )
			FConnection->CancelOrder( pCancelOrderMessage );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::Quote(IQuoteMessage* Msg)
{
	if( FConnection != NULL )
	{
		VARIANT MessageInstance;
		Msg->get_Instance( &MessageInstance );
		TQuoteMessage* pQuoteMessage = (TQuoteMessage*)(MessageInstance.byref);

		if( pQuoteMessage != NULL )
			FConnection->Quote( pQuoteMessage );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::QuoteCancel(IQuoteCancelMessage* Msg)
{
	if( FConnection != NULL )
	{
		VARIANT MessageInstance;
		Msg->get_Instance( &MessageInstance );
		TQuoteCancelMessage* pQuoteCancelMessage = (TQuoteCancelMessage*)(MessageInstance.byref);

		if( pQuoteCancelMessage != NULL )
			FConnection->QuoteCancel( pQuoteCancelMessage );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::QuoteRequest(IQuoteRequestMessage* Msg)
{
	if( FConnection != NULL )
	{
		VARIANT MessageInstance;
		Msg->get_Instance( &MessageInstance );
		TQuoteRequestMessage* pQuoteRequestMessage = (TQuoteRequestMessage*)(MessageInstance.byref);

		if( pQuoteRequestMessage != NULL )
			FConnection->QuoteRequest( pQuoteRequestMessage );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
void TTaifexConnectionImpl::OnConnected( void )
{
	Fire_OnConnected();
}
//------------------------------------------------------------------------------
void TTaifexConnectionImpl::OnDisconnected( void )
{
	Fire_OnDisconnected();
}
//------------------------------------------------------------------------------
void TTaifexConnectionImpl::OnLogonReply( const char* ReplyString, LogonResult Result, int CID )
{
	WideString WideReplyString( ReplyString );

	Fire_OnLogonReply( WideReplyString.c_bstr(), (LogonResultEnum)Result,(long) CID );
}
//------------------------------------------------------------------------------
void TTaifexConnectionImpl::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup ExecPossDup )
{
	VARIANT                     MessageInstance;
	TCOMIExecutionReportMessage MessageToSend = CoExecutionReportMessage::Create();
	MessageToSend->get_Instance( &MessageInstance );
	TExecutionReportMessage* pExecutionReportMessage = (TExecutionReportMessage*)(MessageInstance.byref);
	pExecutionReportMessage->Attach( Msg->GetMTree() );

	Fire_OnExecutionReport( (IExecutionReportMessage*)MessageToSend, (ExecDupEnum)ExecPossDup );
}
//------------------------------------------------------------------------------
void TTaifexConnectionImpl::OnNews( TNewsMessage* Msg )
{
	VARIANT MessageInstance;
	TCOMINewsMessage MessageToTriggerEvent = CoNewsMessage::Create();
	MessageToTriggerEvent->get_Instance( &MessageInstance );
	TNewsMessage* pNewsMessage = (TNewsMessage*)(MessageInstance.byref);
	pNewsMessage->Attach( Msg->GetMTree() );

	Fire_OnNews( MessageToTriggerEvent );
}
//------------------------------------------------------------------------------
void TTaifexConnectionImpl::OnRecoverFinished( int Count )
{
	Fire_OnRecoverFinished( Count );
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::get_ClearMemberID(BSTR* Value)
{
	if( FConnection != NULL )
	{
		WideString CMID( FConnection->GetCMID().c_str() );
		*Value = CMID.Copy();
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::get_IsAdministrator(VARIANT_BOOL* Value)
{
	if( FConnection != NULL )
	{
		if( FConnection->IsAdmin() == true )
			*Value = VARIANT_TRUE;
		else
			*Value = VARIANT_FALSE;
	}
	else
		*Value = VARIANT_FALSE;
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::set_ClearMemberID(BSTR Value)
{
	if( FConnection != NULL )
	{
		AnsiString CM( Value );
		FConnection->SetCMID( CM.c_str() );
	}
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::SetLanguage(MessageLanguageEnum Lang)
{
	if( FConnection != NULL )
		FConnection->SetLanguage((MessageLanguage) Lang );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::GenerateUniqueID(MarketEnum Market, MessageTypeEnum Type,
          __int64* NID)
{
	if( FConnection != NULL )
		*NID = FConnection->GenerateNID( (nsOrderMessageDefine::MessageTypeEnum) Type );
	else
		*NID = 0;
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::GetBrokerID(MarketEnum Market, BSTR* BrokerID)

{
	if( FConnection != NULL )
	{
		WideString WideBrokerID( FConnection->GetBrokerID( (nsOrderMessageDefine::MarketEnum)Market ).c_str() );
		*BrokerID = WideBrokerID.Copy();
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::SetBrokerID(MarketEnum Market, BSTR Value)
{
	if( FConnection != NULL )
	{
		AnsiString BrokerID( Value );
		FConnection->SetBrokerID( (nsOrderMessageDefine::MarketEnum)Market, BrokerID.c_str());
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::get_BrokerID(BSTR* Value)
{
	if( FConnection != NULL )
	{
		WideString BrokerID( FConnection->GetBrokerID( nsOrderMessageDefine::mTWFutures ).c_str() );
		*Value = BrokerID.Copy();
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::set_BrokerID(BSTR Value)
{
	if( FConnection != NULL )
	{
		AnsiString BrokerID( Value );
		FConnection->SetBrokerID( nsOrderMessageDefine::mTWFutures, BrokerID.c_str() );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::get_UseNewFuturesSymbol(VARIANT_BOOL* Value)
{
	if( FConnection != NULL )
	{
		if( FConnection->GetUseNewFuturesSymbol() == true )
			*Value =  VARIANT_TRUE;
		else
			*Value =  VARIANT_FALSE;
	}
	else
		*Value =  VARIANT_FALSE;
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::set_UseNewFuturesSymbol(VARIANT_BOOL Value)
{
	if( FConnection != NULL )
	{
		if( Value ==  VARIANT_FALSE )
			FConnection->SetUseNewFuturesSymbol( false );
		else
			FConnection->SetUseNewFuturesSymbol( true );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::SetDebugLog(BSTR FileName)
{
	if( FConnection != NULL )
	{
		AnsiString LogFileName( FileName );

		FConnection->SetLogFile( LogFileName.c_str() );
		UFC::BufferedLog::Printf( " -------------------------------------------------" );
		UFC::BufferedLog::Printf( "    Speedy Order API, Win32 COM Version           " );
		UFC::BufferedLog::Printf( "    Copyright 2002-2012 by MDBS Software Inc.     " );
		UFC::BufferedLog::Printf( "    All right reserved.                           " );
		UFC::BufferedLog::Printf( "                                                  " );
		UFC::BufferedLog::Printf( "    OrderConnection Version[%d]", GetVersion( "OrderConnection.dll", true ) );
		if( FConnection != NULL )
			UFC::BufferedLog::Printf( "    Message Queue Size[%d]", FConnection->GetQueueSize() );
		UFC::BufferedLog::Printf( " -------------------------------------------------"  );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::OrderStatusRequest(IOrderStatusRequestMessage* Msg)










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
STDMETHODIMP TTaifexConnectionImpl::Recover(BSTR BeginTime, RecoverTypeEnum Type,
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
STDMETHODIMP TTaifexConnectionImpl::Recover2(BSTR BeginTime, BSTR EndTime, RecoverTypeEnum Type,
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
STDMETHODIMP TTaifexConnectionImpl::set_SendFillZeroQty(VARIANT_BOOL Param1)
{
	if( Param1 == VARIANT_TRUE )
		FConnection->SetFillZeroQtyToCancel( false );
	else
		FConnection->SetFillZeroQtyToCancel( true );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::NewsRequest(INewsMessage* NewsReq)
{
	if( FConnection != NULL )
	{
		VARIANT MessageInstance;
		NewsReq->get_Instance( &MessageInstance );
		TNewsMessage* pNewsRequest = (TNewsMessage*)(MessageInstance.byref);

		if( pNewsRequest != NULL )
			FConnection->SendNewsRequest( pNewsRequest );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::LockThisAE(VARIANT_BOOL Locked)
{
	if( FConnection != NULL )
	{
		if( Locked == VARIANT_FALSE )
			FConnection->Lock( false );
		else
			FConnection->Lock( true );
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::LockAE(BSTR AE, VARIANT_BOOL IsLock)
{
	if( FConnection != NULL )
	{
		AnsiString AnsiAE( AE );

		if( IsLock == VARIANT_FALSE )
			FConnection->Lock( AnsiAE.c_str(), false );
		else
			FConnection->Lock( AnsiAE.c_str(), true );
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::set_EnableFilledStatus(VARIANT_BOOL Value)
{
	if( FConnection != NULL )
	{
		if( Value == VARIANT_FALSE )
			FConnection->SetEnableFilledState( false );
		else
			FConnection->SetEnableFilledState( true );
	}
	return S_OK;
};
//-----------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::get_EnablePandingNewAck(VARIANT_BOOL* Value)
{
	if( FConnection != NULL )
	{
		if( FConnection->GetEnablePendingNewAck() == true  )
			*Value = VARIANT_TRUE;
		else
			*Value = VARIANT_FALSE;
		return S_OK;
	}
	*Value = VARIANT_FALSE;
	return S_OK;
};
//-----------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::set_EnablePandingNewAck(VARIANT_BOOL Value)
{
	if( FConnection != NULL )
	{
		if( Value == VARIANT_FALSE )
			FConnection->SetEnablePendingNewAck( false );
		else
			FConnection->SetEnablePendingNewAck( true );
	}
	return S_OK;
};
//-----------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::AddSellSide(SellSideTypeEnum SellSide)
{
	if( FConnection != NULL )
		FConnection->AddSellSide( (SellSideType) SellSide );
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::RemoveSellSide(SellSideTypeEnum SellSide)
{
	if( FConnection != NULL )
		FConnection->RemoveSellSide( (SellSideType) SellSide );
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::set_X25StylePxQty(VARIANT_BOOL Value)
{
	if( FConnection != NULL )
	{
		if( Value == VARIANT_FALSE )
			FConnection->SetX25StyleQtyPx( false );
		else
			FConnection->SetX25StyleQtyPx( true );
	}
	return S_OK;
};
//-----------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::get_X25StylePxQty(VARIANT_BOOL* Value)
{
	if( FConnection != NULL )
	{
		if( FConnection->GetX25StyleQtyPx() == true  )
			*Value = VARIANT_TRUE;
		else
			*Value = VARIANT_FALSE;
		return S_OK;
	}
	*Value = VARIANT_FALSE;
	return S_OK;
};
//-----------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::set_Stdout(VARIANT_BOOL Value)
{
	if( Value == VARIANT_FALSE )
		UFC::BufferedLog::SetPrintToStdout( FALSE );
	else
		UFC::BufferedLog::SetPrintToStdout( TRUE );
	return S_OK;
};
//-----------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::URLEncode(BSTR URL, BSTR* EncodeURL)
{
	AnsiString strMsg( URL );
	String     strEncode;

	for ( int i=0; i< strMsg.Length(); i++)
	{
	   unsigned char cChar = *(strMsg.c_str()+i);

	   if ( (cChar >= 'a' && cChar <= 'z') ||
			(cChar >= 'A' && cChar <= 'Z') ||
			(cChar >= '0' && cChar <= '9'))
	   {
		   strEncode += (char)cChar;
	   }
	   else
	   {
		   char caBuf[5] = {0,};

		   sprintf( caBuf, "%%%x", cChar);
		   strEncode += caBuf;
	   }
	}
	WideString WEncodeURL( strEncode );
	*EncodeURL = WEncodeURL.Copy();
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::get_Version(BSTR* Value)
{
	int COMVer = GetVersion( "OrderConnection.dll", true );
	WideString VerStr;

	VerStr.printf( L"%d.%02d.%02d.%02d",
				   COMVer/1000000,
				   (COMVer%1000000)/10000,
				   (COMVer%10000)/100,
					COMVer%100 );
	*Value = VerStr.Copy();
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::get_WhichAPI(APIEnum* Value)
{
	if( FConnection != NULL )
		*Value =  (APIEnum)FConnection->UseAPI();
	else
		*Value = Orderconnection_tlb::APIEnum::aeSpeedy;
	return S_OK;
};
//-----------------------------------------------------------------------------
STDMETHODIMP TTaifexConnectionImpl::set_DetectAPI(VARIANT_BOOL Value)
{
	if( Value == VARIANT_TRUE )
		TTaifexConnection::DetectAPI( true );
	else
		TTaifexConnection::DetectAPI( false );
  return S_OK;
};
//-----------------------------------------------------------------------------


