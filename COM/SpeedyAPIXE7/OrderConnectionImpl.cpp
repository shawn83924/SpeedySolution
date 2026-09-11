// ---------------------------------------------------------------------------
// ORDERCONNECTIONIMPL : Implementation of TOrderConnectionImpl (CoClass: OrderConnection, Interface: IOrderConnection)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "OrderConnectionImpl.h"
// ---------------------------------------------------------------------------
EventHandler::EventHandler( TOrderConnectionImpl* Owner )
:FOwner( Owner )
{

}
// ---------------------------------------------------------------------------
void EventHandler::OnConnected( void )
{
	FOwner->FireOnConnected();
}
// ---------------------------------------------------------------------------
void EventHandler::OnDisconnected( void )
{
	FOwner->FireOnDisconnected();
}
// ---------------------------------------------------------------------------
void EventHandler::OnLogonReply( const char* ReplyString, LogonResult Result, int CID )
{
	UTF8String UTF8Str( ReplyString );
	WideString WideReplyString( UTF8Str );

	FOwner->FireOnLogonReply( WideReplyString.c_bstr(), (LogonResultEnum)Result,(long) CID );
}
// ---------------------------------------------------------------------------
void EventHandler::OnRecoverFinished( int Count )
{
	FOwner->FireOnRecoverFinished( Count );
}
// ---------------------------------------------------------------------------
void EventHandler::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup )
{
	TExecutionReportMessageImpl* FExecReportMessage = new TExecutionReportMessageImpl( Msg );
	FOwner->FireOnExecutionReport( FExecReportMessage, (ExecDupEnum)PossDup );
	FExecReportMessage->DetachMessage();
}
// ---------------------------------------------------------------------------
void EventHandler::OnCancelWorking( CancelWorkingResult Result, int Count )
{
	FOwner->FireOnCancelWorking( (CancelWorkingResultEnum)Result, Count );
}
// ---------------------------------------------------------------------------
void EventHandler::OnChangePassword( ChangePwdResult Result )
{
	WideString ErrMsg;

	switch( Result)
	{
		case crModifyOk:       ErrMsg = L"成功!請重新登入"; break;
		case crFailed:         ErrMsg = L"失敗!請稍後再試"; break;
		case crLengthNeeds8:   ErrMsg = L"失敗!長度至少要八碼"; break;
		case crNeedsNumbers:   ErrMsg = L"失敗!需要包含數字"; break;
		case crNeedsSymbols:   ErrMsg = L"失敗!需要包含符號"; break;
		case crNotLogin:       ErrMsg = L"失敗!請先登入"; break;
		case crRequestTimeOut: ErrMsg = L"失敗!修改逾時"; break;
		case crSameAsOld:      ErrMsg = L"失敗!與舊密碼相同"; break;
		case crSameAsPrevious: ErrMsg = L"失敗!與前一次密碼相同"; break;
		case crUpperLowerCase: ErrMsg = L"失敗!需要包含大小寫英文"; break;
		case crWrongPassword:  ErrMsg = L"失敗!原密碼錯誤"; break;
		case crLockedTryAgain: ErrMsg = L"失敗!帳號被鎖定,請稍後再試"; break;
	}
	FOwner->FireOnChangePasswordReply( (ChangePasswordEnum) Result, ErrMsg.c_bstr() );
}
// ---------------------------------------------------------------------------
void EventHandler::OnNews( TNewsMessage* Msg )
{
	TNewsMessageImpl* FNewsMessage = new TNewsMessageImpl();
	FNewsMessage->AttachMessage( Msg );
	FOwner->FireOnNews( FNewsMessage );
}
// ---------------------------------------------------------------------------
// TOrderConnectionImpl
// ---------------------------------------------------------------------------
__fastcall TOrderConnectionImpl::TOrderConnectionImpl()
:FConnection( NULL )
,FEventHandler( new EventHandler(this) )
{
}
// ---------------------------------------------------------------------------
// TOrderConnectionImpl
// ---------------------------------------------------------------------------
__fastcall TOrderConnectionImpl::TOrderConnectionImpl(const System::_di_IInterface Controller)
:inherited(Controller)
,FConnection( NULL )
,FEventHandler( new EventHandler(this) )
{
}
// ---------------------------------------------------------------------------
// TOrderConnectionImpl
// ---------------------------------------------------------------------------
__fastcall TOrderConnectionImpl::TOrderConnectionImpl(Comobj::TComObjectFactory* Factory,
									  const System::_di_IInterface Controller)
:inherited(Factory, Controller)
,FConnection( NULL )
,FEventHandler( new EventHandler(this) )
{
}
// ---------------------------------------------------------------------------
void __fastcall TOrderConnectionImpl::EventSinkChanged(const System::_di_IInterface EventSink)
{
	if( EventSink != NULL )
	{
		IOrderConnectionEventsDisp* NewEvents = new IOrderConnectionEventsDisp();

		FEventList.push_back( NewEvents );
		NewEvents->Bind( EventSink );
	}
}
// ---------------------------------------------------------------------------
// TOrderConnectionImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TOrderConnectionImpl>(Comserv::GetComServer(),
						   __classid(TOrderConnectionImpl),
						   CLSID_OrderConnection,
						   Comobj::ciMultiInstance,
#if defined(USING_MTA)
						   Comobj::tmFree );
#else
						   Comobj::tmApartment );
#endif
}
#pragma startup createFactory 32
//------------------------------------------------------------------------------
int TOrderConnectionImpl::GetVersion( bool IsProxy )
{
	int   COMVersion = 20000;
	char  Binary[MAX_PATH];
	DWORD n;

	if( IsProxy == true )
		COMVersion = 2000000;
	if( GetModuleFileNameA( HInstance, Binary, MAX_PATH ) > 0 )
	{
		if((n = GetFileVersionInfoSizeA( Binary, NULL ) ) > 0 )
		{
			VS_FIXEDFILEINFO* vsinfo;
			char*             pBuf = new char[ n ];
			UINT              Len;

			GetFileVersionInfoA( Binary, 0, n, pBuf );
			if( VerQueryValueA( pBuf, "\\", (void **)&vsinfo, &Len))
			{
				AnsiString  VersionString;
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
	}
	return COMVersion;
}
//------------------------------------------------------------------------------
HRESULT __fastcall TOrderConnectionImpl::FireOnConnected()
{
	for( register size_t i=0;i< FEventList.size(); i ++ )
		FEventList[i]->OnConnected( );
	return S_OK;
}
//------------------------------------------------------------------------------
HRESULT __fastcall TOrderConnectionImpl::FireOnDisconnected()
{
	for( register size_t i=0;i< FEventList.size(); i ++ )
		FEventList[i]->OnDisconnected( );
	return S_OK;
}
//------------------------------------------------------------------------------
HRESULT __fastcall TOrderConnectionImpl::FireOnLogonReply(BSTR Message,Speedyapi_tlb::LogonResultEnum Result, long ConnectionID)
{
	for( register size_t i=0;i< FEventList.size(); i ++ )
		FEventList[i]->OnLogonReply( Message, Result, ConnectionID );
	return S_OK;
}
//------------------------------------------------------------------------------
HRESULT __fastcall TOrderConnectionImpl::FireOnRecoverFinished( long Count )
{
	for( register size_t i=0;i<  FEventList.size(); i ++ )
		FEventList[i]->OnRecoverFinished( Count );
	return S_OK;
}
//------------------------------------------------------------------------------
HRESULT __fastcall TOrderConnectionImpl::FireOnCancelWorking( Speedyapi_tlb::CancelWorkingResultEnum  Result, long Count )
{
	for( register size_t i=0;i<  FEventList.size(); i ++ )
		FEventList[i]->OnCancelWorking( Result, Count );
	return S_OK;
}
//------------------------------------------------------------------------------
HRESULT __fastcall TOrderConnectionImpl::FireOnChangePasswordReply( Speedyapi_tlb::ChangePasswordEnum Result, BSTR Message )
{
	for( register size_t i=0;i<  FEventList.size(); i ++ )
		 FEventList[i]->OnChangePasswordReply( Result, Message );
	return S_OK;
}
//------------------------------------------------------------------------------
HRESULT __fastcall TOrderConnectionImpl::FireOnExecutionReport(IExecutionReportMessage* Msg ,  Speedyapi_tlb::ExecDupEnum ExecPossDup )
{
	for( register size_t i=0;i< FEventList.size(); i ++ )
		FEventList[i]->OnExecutionReport( Msg, ExecPossDup );
	return S_OK;
}
//------------------------------------------------------------------------------
HRESULT __fastcall TOrderConnectionImpl::FireOnNews(INewsMessage* News )
{
	for( register size_t i=0;i< FEventList.size(); i ++ )
		FEventList[i]->OnNews( News );
	return S_OK;
}

//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::Create(BSTR AppName)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	return Create2( AppName );
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::Create2(BSTR AppName)
{
	AnsiString AnsiAppName( AppName );

	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( SysStringLen( AppName ) <= 2 )
		AnsiAppName.printf( "MApp@%s.%d", UFC::Hostname, GetTickCount() );
	FConnection = new TTaifexConnection( HInstance,
										 HInstance,
										 AnsiAppName.c_str(),
										 FEventHandler,
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
STDMETHODIMP TOrderConnectionImpl::Create3(BSTR AppName, long QueueSize)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	MApp::SetQueueSize( QueueSize );
	return Create2( AppName );
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::CreateMT(BSTR AppName)
{
	AnsiString AnsiAppName( AppName );

	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( SysStringLen( AppName ) <= 2 )
		AnsiAppName.printf( "MApp@%s.%d", UFC::Hostname, GetTickCount() );
	FConnection = new TTaifexConnection( HInstance,
										 HInstance,
										 AnsiAppName.c_str(),
										 FEventHandler,
										 atSpeedy,
										 FALSE );///< Is NOT GUI App
	FConnection->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
	FConnection->SetTAIFEXQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );
	FConnection->SetTSEDataFormatResourceID( IDR_TSE_T010,IDR_TSE_O010,IDR_TSE_P010, IDR_TSE_T020, IDR_TSE_O020, IDR_TSE_P020, IDR_TSE_R030, "INI" );
	FConnection->SetTSEDataFormatResourceID2( IDR_TSE_A010, IDR_TSE_V010,IDR_TSE_E010,IDR_TSE_Ex010, IDR_TSE_A020, IDR_TSE_V020, IDR_TSE_E020,IDR_TSE_Ex020, "INI" );
	FConnection->SetOTCDataFormatResourceID( IDR_OTC_T010,IDR_OTC_O010,IDR_OTC_P010, IDR_OTC_T020, IDR_OTC_O020, IDR_OTC_P020, IDR_OTC_R030, "INI" );
	FConnection->SetOTCDataFormatResourceID2( IDR_OTC_V010, IDR_OTC_E010, IDR_OTC_Ex010, IDR_OTC_V020, IDR_OTC_E020, IDR_OTC_Ex020, "INI" );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::Destroy()
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
	{
		delete FConnection;
		FConnection = NULL;
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::Connect(BSTR RemoteIP, long RemotePort)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
	{
		AnsiString AnsiRemoteIP( RemoteIP );
		FConnection->Connect( AnsiRemoteIP.c_str(), RemotePort, 10  );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::Connect2(BSTR RemoteIP, long RemotePort, long TimeoutSec)

{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
	{
		AnsiString AnsiRemoteIP( RemoteIP );
		FConnection->Connect( AnsiRemoteIP.c_str(), RemotePort, TimeoutSec  );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::Disconnect()
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
	{
		FConnection->DeleteCAObject();
		FConnection->Disconnect();
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::LogonProxy(BSTR ID, BSTR Password, BSTR Account,
          BSTR Token1, BSTR Token2)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
	{
		AnsiString AnsiID( ID );
		AnsiString AnsiPassword( Password );
		AnsiString AnsiACCOUNT( Account );
		AnsiString AnsiToken1( Token1 );
		AnsiString AnsiToken2( Token2 );
		int        COMVersion = GetVersion( true );

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
STDMETHODIMP TOrderConnectionImpl::Logon(BSTR ID, BSTR PASSWD, BSTR ACCOUNT, ConnectionTypeEnum Type)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
	{
		AnsiString AnsiID( ID );
		AnsiString AnsiPASSWD( PASSWD );
		AnsiString AnsiACCOUNT( ACCOUNT );
		int        COMVersion = GetVersion( false );

		FConnection->Logon( AnsiID.c_str(),
							AnsiPASSWD.c_str(),
							AnsiACCOUNT.c_str(),
							(ConnectionType)Type,
							COMVersion );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::NewOrder(INewOrderMessage* Msg)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
	{
		VARIANT MessageInstance;

		Msg->get_Instance( &MessageInstance );
		if( MessageInstance.byref != NULL )
			FConnection->NewOrder( static_cast<TNewOrderMessage*>(MessageInstance.byref) );
		UFC::BufferedLog::Printf( " *** [%s] sent to socket buffer *** ",__FUNCTION__  );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::ReplaceOrder(IReplaceOrderMessage* Msg)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
	{
		VARIANT MessageInstance;

		Msg->get_Instance( &MessageInstance );
		if( MessageInstance.byref != NULL )
			FConnection->ReplaceOrder( static_cast<TReplaceOrderMessage*>(MessageInstance.byref) );
		UFC::BufferedLog::Printf( " *** [%s] sent to socket buffer *** ",__FUNCTION__  );
	}
	return S_OK;
}
//------------------------------------------------------------------------------



STDMETHODIMP TOrderConnectionImpl::CancelOrder(ICancelOrderMessage* Msg)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
	{
		VARIANT MessageInstance;

		Msg->get_Instance( &MessageInstance );
		if( MessageInstance.byref != NULL )
			FConnection->CancelOrder( static_cast<TCancelOrderMessage*>(MessageInstance.byref) );
		UFC::BufferedLog::Printf( " *** [%s] sent to socket buffer *** ",__FUNCTION__  );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::Quote(IQuoteMessage* Msg)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
	{
		VARIANT MessageInstance;

		Msg->get_Instance( &MessageInstance );
		if( MessageInstance.byref != NULL )
			FConnection->Quote( static_cast<TQuoteMessage*>(MessageInstance.byref) );
		UFC::BufferedLog::Printf( " *** [%s] sent to socket buffer *** ",__FUNCTION__  );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::QuoteCancel(IQuoteCancelMessage* Msg)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
	{
		VARIANT MessageInstance;

		Msg->get_Instance( &MessageInstance );
		if( MessageInstance.byref != NULL )
			FConnection->QuoteCancel( static_cast<TQuoteCancelMessage*>(MessageInstance.byref) );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::QuoteRequest(IQuoteRequestMessage* Msg)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
	{
		VARIANT MessageInstance;

		Msg->get_Instance( &MessageInstance );
		if( MessageInstance.byref != NULL )
			FConnection->QuoteRequest( static_cast<TQuoteRequestMessage*>(MessageInstance.byref) );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::OrderStatusRequest(IOrderStatusRequestMessage* Msg)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
	{
		VARIANT MessageInstance;

		Msg->get_Instance( &MessageInstance );
		if( MessageInstance.byref != NULL )
			FConnection->OrderStatusRequest( static_cast<TOrderStatusRequest*>(MessageInstance.byref) );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::get_ClearMemberID(BSTR* Value)
{
	if( FConnection != NULL )
	{
		WideString CMID( FConnection->GetCMID().c_str() );
		*Value = CMID.Copy();
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::get_IsAdministrator(VARIANT_BOOL* Value)
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
STDMETHODIMP TOrderConnectionImpl::set_ClearMemberID(BSTR Value)
{
	if( FConnection != NULL )
	{
		AnsiString CM( Value );
		FConnection->SetCMID( CM.c_str() );
	}
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::SetLanguage(MessageLanguageEnum Lang)
{
	if( FConnection != NULL )
		FConnection->SetLanguage((MessageLanguage) Lang );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::GenerateUniqueID(MarketEnum Market, MessageTypeEnum Type,
          __int64* NID)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
		*NID = FConnection->GenerateNID( (nsOrderMessageDefine::MessageTypeEnum) Type );
	else
		*NID = 0;
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::GetBrokerID(MarketEnum Market, BSTR* BrokerID)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
	{
		UFC::AnsiString AnsiBrokerID = FConnection->GetBrokerID( (nsOrderMessageDefine::MarketEnum)Market );
		WideString      WideBrokerID( AnsiBrokerID.c_str() );

		*BrokerID = WideBrokerID.Copy();
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::SetBrokerID(MarketEnum Market, BSTR Value)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
	{
		AnsiString BrokerID( Value );
		FConnection->SetBrokerID( (nsOrderMessageDefine::MarketEnum)Market, BrokerID.c_str());
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::get_BrokerID(BSTR* Value)
{
	if( FConnection != NULL )
	{
		UFC::AnsiString AnsiBrokerID = FConnection->GetBrokerID( nsOrderMessageDefine::mTWFutures );
		WideString      WideBrokerID( AnsiBrokerID.c_str() );

		*Value = WideBrokerID.Copy();
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::set_BrokerID(BSTR Value)
{
	if( FConnection != NULL )
	{
		AnsiString BrokerID( Value );
		FConnection->SetBrokerID( nsOrderMessageDefine::mTWFutures, BrokerID.c_str() );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::get_UseNewFuturesSymbol(VARIANT_BOOL* Value)
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
STDMETHODIMP TOrderConnectionImpl::set_UseNewFuturesSymbol(VARIANT_BOOL Value)
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
void TOrderConnectionImpl::PrintCaption( bool detail )
{
	Glog->fprintf( " -------------------------------------------------" );
	Glog->fprintf( "    Speedy Order API, Win%d COM Version           ", sizeof(void*)*8 );
	Glog->fprintf( "    Copyright 2002-%d by MDBS Software Inc.       ", UFC::ThisYear() );
	Glog->fprintf( "    All right reserved.                           " );
	Glog->fprintf( "                                                  " );
	Glog->fprintf( "    SpeedyAPI Version[%d]                   ", GetVersion( true ) );
		if( detail == true )
		Glog->fprintf( "    *** Enable Detail log ***" );
	if( FConnection != NULL )
		Glog->fprintf( "    Message Queue Size[%d]", FConnection->GetQueueSize() );
	Glog->fprintf( " -------------------------------------------------"  );
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::SetDebugLog(BSTR FileName)
{
	if( FConnection != NULL )
	{
		AnsiString LogFileName( FileName );

		FConnection->SetLogFile( LogFileName.c_str() );
		PrintCaption( false );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::SetDebugLogEx(BSTR FileName)
{
	if( FConnection != NULL )
	{
		AnsiString LogFileName( FileName );

		FConnection->SetLogFileEx( LogFileName.c_str() );
		PrintCaption( true );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::Recover(BSTR BeginTime, RecoverTypeEnum Type, RecoverMarketEnum Market)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
	{
		///< Recover from Begin time
		AnsiString AnsiBeginTime( BeginTime );

		FConnection->Recover( AnsiBeginTime.c_str(), (RecoverDataType) Type, (RecoverMarket) Market );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::Recover2(BSTR BeginTime, BSTR EndTime, RecoverTypeEnum Type,
          RecoverMarketEnum Market)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
	{
		///< Recover from Begin time to End time.
		AnsiString AnsiBeginTime( BeginTime );
		AnsiString AnsiEndTime( EndTime );

		FConnection->Recover( AnsiBeginTime.c_str(),AnsiEndTime.c_str(),(RecoverDataType) Type, (RecoverMarket) Market );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::Recover3(BSTR Date, BSTR BeginTime, RecoverTypeEnum Type,
          RecoverMarketEnum Market, RecoverSessionEnum Session)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
	{
		///< Recover from Begin time
		AnsiString AnsiDate( Date );
		AnsiString AnsiBeginTime( BeginTime );

		FConnection->Recover( AnsiBeginTime.c_str(), (RecoverDataType) Type, (RecoverMarket) Market, (RecoverSession)Session, AnsiDate.c_str() );
	}
    return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::set_SendFillZeroQty(VARIANT_BOOL Param1)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( Param1 == VARIANT_TRUE )
		FConnection->SetFillZeroQtyToCancel( true );
	else
		FConnection->SetFillZeroQtyToCancel( false );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::NewsRequest(INewsMessage* NewsReq)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
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
STDMETHODIMP TOrderConnectionImpl::LockThisAE(VARIANT_BOOL Locked)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
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
STDMETHODIMP TOrderConnectionImpl::LockAE(BSTR AE, VARIANT_BOOL IsLock)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
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
STDMETHODIMP TOrderConnectionImpl::set_EnableFilledStatus(VARIANT_BOOL Value)
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
STDMETHODIMP TOrderConnectionImpl::get_EnablePandingNewAck(VARIANT_BOOL* Value)
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
STDMETHODIMP TOrderConnectionImpl::set_EnablePandingNewAck(VARIANT_BOOL Value)
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
STDMETHODIMP TOrderConnectionImpl::AddSellSide(SellSideTypeEnum SellSide)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
		FConnection->AddSellSide( (SellSideType) SellSide );
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::RemoveSellSide(SellSideTypeEnum SellSide)
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
		FConnection->RemoveSellSide( (SellSideType) SellSide );
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::set_X25StylePxQty(VARIANT_BOOL Value)
{
	if( FConnection != NULL )
	{
		if( Value == VARIANT_FALSE )
			FConnection->SetX25StyleQtyPx( false );
		else
			FConnection->SetX25StyleQtyPx( true );
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::get_X25StylePxQty(VARIANT_BOOL* Value)
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
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::set_Stdout(VARIANT_BOOL Value)
{
	if( FConnection != NULL )
	{
		if( Value == VARIANT_FALSE )
			FConnection->Stdout( FALSE );
		else
			FConnection->Stdout( TRUE );
	}
	return S_OK;
};
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::URLEncode(BSTR URL, BSTR* EncodeURL)
{
	AnsiString strMsg( URL );
	String     strEncode;

	for( register int i=0; i< strMsg.Length(); i++)
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
STDMETHODIMP TOrderConnectionImpl::get_Version(BSTR* Value)
{
	WideString VerStr;
	int        COMVer = GetVersion( true );

#if defined(USING_MTA)
	VerStr.printf( L"%d.%02d.%02d.%02d %d-bit MTA",
				   COMVer/1000000, (COMVer%1000000)/10000,
				  (COMVer%10000)/100, COMVer%100, sizeof(void*)*8 );
#else
	VerStr.printf( L"%d.%02d.%02d.%02d %d-bit STA",
				   COMVer/1000000, (COMVer%1000000)/10000,
				  (COMVer%10000)/100, COMVer%100, sizeof(void*)*8 );
#endif

	*Value = VerStr.Copy();
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::SupportAPI(APIEnum Value, VARIANT_BOOL* Rtn)
{
	*Rtn = TTaifexConnection::SupportAPI((APIType) Value );
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::set_DetectAPI(VARIANT_BOOL Value)
{
	if( Value == VARIANT_TRUE )
		TTaifexConnection::DetectAPI( true );
	else
		TTaifexConnection::DetectAPI( false );
	return S_OK;
}
//-----------------------------------------------------------------------------

STDMETHODIMP TOrderConnectionImpl::get_AlwaysGenerateNID(VARIANT_BOOL* Value)
{
	if( FConnection != NULL )
	{
		if( FConnection->GetAlwaysGenerateNID() == TRUE )
			*Value = VARIANT_TRUE;
		else
			*Value = VARIANT_FALSE;
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::set_AlwaysGenerateNID(VARIANT_BOOL Value)
{
	if( FConnection != NULL )
	{
		if( Value == VARIANT_TRUE )
			FConnection->SetAlwaysGenerateNID( TRUE );
		else
			FConnection->SetAlwaysGenerateNID( FALSE );
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::get_BuildDate(BSTR* Value)
{
	AnsiString DateStr( __DATE__ );
	WideString wStr( DateStr );

	*Value = wStr.Copy();
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::IsTAIFEXFutures(BSTR Symbol, VARIANT_BOOL* Result)
{
	if( FConnection != NULL )
	{
		AnsiString AnsiSymbol( Symbol );
		if( FConnection->IsTWFutures( AnsiSymbol.c_str()) == true )
			*Result = VARIANT_TRUE;
		else
			*Result = VARIANT_FALSE;
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::IsTAIFEXOptions(BSTR Symbol, VARIANT_BOOL* Result)
{
	if( FConnection != NULL )
	{
		AnsiString AnsiSymbol( Symbol );
		if( FConnection->IsTWOptions( AnsiSymbol.c_str()) == true )
			*Result = VARIANT_TRUE;
		else
			*Result = VARIANT_FALSE;
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::CancelWorking(CancelMarketEnum Market, BSTR AE,
          BSTR Account, BSTR UserData, BSTR ProdID)
{
	if( FConnection != NULL )
	{
		AnsiString AnsiAE( AE );
		AnsiString AnsiAccount( Account );
		AnsiString AnsiUserData( UserData );
		AnsiString AnsiProdID( ProdID );

		FConnection->CancelWorking( (CancelMarket)Market,
									AnsiAE.c_str(),
									AnsiAccount.c_str(),
									AnsiUserData.c_str(),
									AnsiProdID.c_str(),
									"" );
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::get_CADLLFileName(BSTR* Value)
{
	if( FConnection != NULL )
	{
		WideString dllFileName( FConnection->GetCADLLFileName().c_str() );
		*Value = dllFileName.Copy();
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::get_CAPassword(BSTR* Value)
{
	if( FConnection != NULL )
	{
		WideString caPassword( FConnection->GetCAPassword().c_str() );
		*Value = caPassword.Copy();
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::get_CAPFXFilePathName(BSTR* Value)
{
	if( FConnection != NULL )
	{
		WideString caPFXFilePathName( FConnection->GetCAPFXFilePathName().c_str() );
		*Value = caPFXFilePathName.Copy();
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::set_CADLLFileName(BSTR Value)
{
	if( FConnection != NULL )
	{
		AnsiString dllFileName( Value );
		FConnection->SetCADLLFileName(dllFileName.c_str());
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::set_CAPassword(BSTR Value)
{
	if( FConnection != NULL )
	{
		AnsiString caPassword( Value );
		FConnection->SetCAPassword(caPassword.c_str());
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::set_CAPFXFilePathName(BSTR Value)
{
	if( FConnection != NULL )
	{
		AnsiString caPFXFilePathName( Value );
		FConnection->SetCAPFXFilePathName(caPFXFilePathName.c_str());
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::CreateCAObject()
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
	{
		FConnection->CreateCAObject();
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::get_CAOrganizationalUnit(BSTR* Value)
{
	if( FConnection != NULL )
	{
		WideString caOrganizationalUnit( FConnection->GetCAOrganizationalUnit().c_str() );
		*Value = caOrganizationalUnit.Copy();
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::set_CAOrganizationalUnit(BSTR Value)
{
	if( FConnection != NULL )
	{
		AnsiString caOrganizationalUnit( Value );
		FConnection->SetCAOrganizationalUnit(caOrganizationalUnit.c_str());
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::get_CACommonName(BSTR* Value)
{
	if( FConnection != NULL )
	{
		WideString caCommonName( FConnection->GetCACommonName().c_str() );
		*Value = caCommonName.Copy();
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::set_CACommonName(BSTR Value)
{
	if( FConnection != NULL )
	{
		AnsiString caCommonName( Value );
		FConnection->SetCACommonName(caCommonName.c_str());
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::DeleteCAObject()
{
	if( FConnection != NULL )
	{
		FConnection->DeleteCAObject();
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::NewOrderDirect(MarketEnum Market, __int64 NID,
          BSTR Order, BSTR Data, long Group)
{
	AnsiString AnsiOrder( Order );
	AnsiString AnsiData( Data );

	UFC::BufferedLog::Printf( " *** Call[%s] *** ",__FUNCTION__ );
	if( FConnection != NULL )
	{
		FConnection->SendDirect( NID,(nsOrderMessageDefine::MarketEnum)Market,
								 AnsiOrder.c_str(), AnsiData.c_str() ,Group  );
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::get_LocalIP(BSTR* Value)
{
	if( FConnection != NULL )
	{
		WideString LocalIP( FConnection->GetLocalIP().c_str() );
		*Value = LocalIP.Copy();
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::ChangePassword(BSTR Password, BSTR NewPassword)

{
	if( FConnection != NULL )
	{
		AnsiString AnsiPwd( Password );
		AnsiString AnsiNewPwd( NewPassword );
		FConnection->ChangePassword( AnsiPwd.c_str(), AnsiNewPwd.c_str() );
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::GetMarginPosition(BSTR BrokerID, BSTR Account,
          BSTR* Value, VARIANT_BOOL* SUCCEED)
{
	if( FConnection != NULL )
	{
		AnsiString AnsiBrokerID( BrokerID );
		AnsiString AnsiAccount( Account );
		UFC::AnsiString ResultStr;
		bool Rtn = FConnection->MarginPositionRequest( AnsiBrokerID.c_str(), AnsiAccount.c_str(),ResultStr );
		WideString WResult( ResultStr.c_str() );

		*Value = WResult.Copy();
		if( Rtn == true )
			*SUCCEED = VARIANT_TRUE;
		else
			*SUCCEED = VARIANT_FALSE;

	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::TouchOrderControl(ITouchOrderCommand* TouchOrderCmd,
          VARIANT_BOOL* Result)
{
	if( FConnection == NULL )
	{
		*Result = VARIANT_FALSE;
		return S_OK;
	}

	VARIANT CommandInstance;
	TouchOrderCmd->get_Instance(&CommandInstance);
	if(CommandInstance.byref != NULL)
		*Result = FConnection->TouchOrderControl( static_cast<TTouchOrderCommand*>(CommandInstance.byref) );
	else
		*Result = VARIANT_FALSE;

	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::CreateTWCAObject()
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ", __FUNCTION__ );
	if( FConnection != NULL )
	{
		FConnection->CreateMLTWCAObject();
	}
/*
	if( FConnection != NULL )
	{
		FConnection->CreateTWCAObject();
	}
*/
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::CreateMLTWCAObject()
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ", __FUNCTION__ );
	if( FConnection != NULL )
	{
		FConnection->CreateMLTWCAObject();
	}
	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::CreateTSCGCCAObject()
{
	try
	{
		UFC::BufferedLog::Printf( " *** Call[%s] *** ", __FUNCTION__ );
		if( FConnection != NULL )
		{
			FConnection->CreateTSCGCCAObject();
		}
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IOrderConnection);
	}
    return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP TOrderConnectionImpl::CreateUniFSCAObject()
{
	UFC::BufferedLog::Printf( " *** Call[%s] *** ", __FUNCTION__ );
	if( FConnection != NULL )
	{
		FConnection->CreateUniFSCAObject();
	}
	return S_OK;
}


