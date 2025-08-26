// ---------------------------------------------------------------------------
// STARWAVECONNECTIONIMPL1 : Implementation of TStarWaveConnectionImpl (CoClass: StarWaveConnection, Interface: IStarWaveConnection)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "StarWaveConnectionImpl.h"
//------------------------------------------------------------------------------
MDConnection::MDConnection( TStarWaveConnectionImpl* Instance, const UFC::AnsiString& AppName )
:FInstance( Instance )
{
	FConnection = new MarketDataConnection( HInstance, AppName, ((IMarketDataConnectionEventListener*)this), TRUE );
	UFC::SleepMS( 330 );
}
//------------------------------------------------------------------------------
MDConnection::~MDConnection()
{
	if( FConnection )
	{
		if( FConnection->IsConnected() )
			FConnection->Disconnect();
		delete FConnection;
		UFC::BufferedLog::Printf( " delete complete" );
	}
}
//------------------------------------------------------------------------------
void MDConnection::Connect( const UFC::AnsiString& IP, int Port, int Timeout )
{
	if( FConnection != NULL )
		FConnection->Connect( IP, Port, Timeout );
}
//------------------------------------------------------------------------------
void MDConnection::Disconnect( void )
{
	if( FConnection != NULL )
		FConnection->Disconnect();
}
//------------------------------------------------------------------------------
void MDConnection::Subscribe( UFC::AnsiString Exchange, UFC::AnsiString Symbol, EnumSubscribeType Type )
{
	UFC::AnsiString ExStr( Exchange );
	UFC::AnsiString SymStr( Symbol );

	if( FConnection != NULL )
		FConnection->Subscribe( ExStr, SymStr, (SubscribeType)Type );
}
//------------------------------------------------------------------------------
void MDConnection::SubscribeX( UFC::AnsiString Exchange, UFC::AnsiString ProductID, UFC::AnsiString SettlementMonth,
	EnumCallPut CallPut, double StrikePx, EnumSubscribeType Type )
{
	UFC::AnsiString ExStr( Exchange );
	UFC::AnsiString ProductStr( ProductID );
	UFC::AnsiString SettleMonthStr( SettlementMonth );

	if( FConnection != NULL )
		FConnection->SubscribeX( ExStr, ProductStr, SettleMonthStr, (CallPutCode)CallPut, StrikePx, (SubscribeType)Type );
}
//------------------------------------------------------------------------------
void MDConnection::Recover( const UFC::AnsiString& Exchange )
{
	UFC::AnsiString ExStr( Exchange );

	if( FConnection != NULL )
		FConnection->Recover( ExStr );
}
//------------------------------------------------------------------------------
void MDConnection::Unsubscribe( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol )
{
	if( FConnection != NULL )
		FConnection->Unsubscribe( Exchange, Symbol );
}
//------------------------------------------------------------------------------
void MDConnection::SubscribeSystemMessage( const UFC::AnsiString& Exchange )
{
	if( FConnection != NULL )
		FConnection->SubscribeSystemMessage( Exchange );
}
//------------------------------------------------------------------------------
void MDConnection::UnsubscribeSystemMessage( const UFC::AnsiString& Exchange )
{
	if( FConnection != NULL )
		FConnection->UnsubscribeSystemMessage( Exchange );
}
//------------------------------------------------------------------------------
void MDConnection::SendNews( const UFC::AnsiString& Group, const UFC::AnsiString& Msg )
{
	if( FConnection != NULL )
		FConnection->SendNews( Group , Msg );
}
//------------------------------------------------------------------------------
void MDConnection::SubscribeNews( const UFC::AnsiString& Group )
{
	if( FConnection != NULL )
		FConnection->SubscribeNews( Group );
}
//------------------------------------------------------------------------------
void MDConnection::UnsubscribeNews( const UFC::AnsiString& Group )
{
	if( FConnection != NULL )
		FConnection->UnsubscribeNews( Group );
}
//------------------------------------------------------------------------------
int MDConnection::SymbolCount( const UFC::AnsiString& Exchange )
{
	if( FConnection != NULL )
		return FConnection->SymbolCount( Exchange );
	return 0;
}
//------------------------------------------------------------------------------
void MDConnection:: OnConnected( void )
{
	for( register size_t i=0;i< FInstance->FEventList.size(); i ++ )
		 FInstance->FEventList[i]->OnConnected();
}
//------------------------------------------------------------------------------
void MDConnection::OnDisconnected( void )
{
	for( register size_t i=0;i< FInstance->FEventList.size(); i ++ )
		 FInstance->FEventList[i]->OnDisconnected();
}
//------------------------------------------------------------------------------
void MDConnection::OnContractDownloadComplete( int ExchangeCount, int UseMS )
{
	for( register size_t i=0;i< FInstance->FEventList.size(); i ++ )
		 FInstance->FEventList[i]->OnContractDownloadCompleted( ExchangeCount, UseMS );
}
//------------------------------------------------------------------------------
void MDConnection::OnNews(  const UFC::AnsiString& Group, const UFC::AnsiString& Msg )
{
	TNewsImpl* NewsImpl = new TNewsImpl();

	NewsImpl->FromNews( Group, Msg );
	for( register size_t i=0;i< FInstance->FEventList.size(); i ++ )
		 FInstance->FEventList[i]->OnNews( interface_cast<INews>(NewsImpl) );
	NewsImpl->Release();
}
//------------------------------------------------------------------------------
void MDConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MatchInfo* Msg )
{
	TMatchImpl* MatchImpl = new TMatchImpl();

	MatchImpl->FromMatchInfo( Msg );
	for( register size_t i=0;i< FInstance->FEventList.size(); i ++ )
		 FInstance->FEventList[i]->OnMatchInfo(interface_cast<IMatch>(MatchImpl) );
	MatchImpl->Release();
}
//------------------------------------------------------------------------------
void MDConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, TotalMatch* Msg )
{
	TTotalVolumeImpl* TotalVolumeImpl = new TTotalVolumeImpl();

	TotalVolumeImpl->FromTotalMatch( Msg );
	for( register size_t i=0;i< FInstance->FEventList.size(); i ++ )
		 FInstance->FEventList[i]->OnTotalMatch( interface_cast<ITotalVolume>(TotalVolumeImpl) );
	TotalVolumeImpl->Release();
}
//------------------------------------------------------------------------------
void MDConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, DayHighLowPrice* Msg )
{
	TDayHighLowImpl* DayHighLowImpl = new TDayHighLowImpl();

	DayHighLowImpl->FromDayHighLow( Msg );
	for( register size_t i=0;i< FInstance->FEventList.size(); i ++ )
		 FInstance->FEventList[i]->OnDayHighLow(interface_cast<IDayHighLow>(DayHighLowImpl));
	DayHighLowImpl->Release();
}
//------------------------------------------------------------------------------
void MDConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, OpeningInfo* Msg )
{
	TOpenInfoImpl* OpenInfoImpl = new TOpenInfoImpl();

	OpenInfoImpl->FromOpeningInfo( Msg );
	for( register size_t i=0;i< FInstance->FEventList.size(); i ++ )
		 FInstance->FEventList[i]->OnOpeningInfo(interface_cast<IOpenInfo>(OpenInfoImpl));
	OpenInfoImpl->Release();
}
//------------------------------------------------------------------------------
void MDConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, OrderBookData* Msg )
{
	TOrderBookImpl* OrderBookImpl = new TOrderBookImpl();

	OrderBookImpl->FromOrderBookData( Msg );
	for( register size_t i=0;i< FInstance->FEventList.size(); i ++ )
		 FInstance->FEventList[i]->OnOrderBookData(interface_cast<IOrderBook>(OrderBookImpl));
	OrderBookImpl->Release();
}
//------------------------------------------------------------------------------
void MDConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, ClosingMarketData* Msg )
{
	TCloseMarketDataImpl* CloseMarketDataImpl = new TCloseMarketDataImpl( );

	if( Msg->GetMsgType() == mtClosingMarketData )
		CloseMarketDataImpl->FromI070( Msg );
	else if( Msg->GetMsgType() == mtClosingMarketDataWithSettlementPrice )
		CloseMarketDataImpl->FromI071( (ClosingMarketDataWithSettlementPrice*)Msg );
	else if( Msg->GetMsgType() == mtClosingMarketDataWithSettlementPriceAndOpenInterest )
		CloseMarketDataImpl->FromI072( (ClosingMarketDataWithSettlementPriceAndOpenInterest*)Msg );
	for( register size_t i=0;i< FInstance->FEventList.size(); i ++ )
		 FInstance->FEventList[i]->OnClosingMarketData( interface_cast<ICloseMarketData>(CloseMarketDataImpl));
	CloseMarketDataImpl->Release();
}
//------------------------------------------------------------------------------
void MDConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, UnderlyingIndexInfo* Msg )
{
	TUnderlyingIndexImpl* UnderlyingIndexImpl = new TUnderlyingIndexImpl();

	UnderlyingIndexImpl->FromIndexMessage( Msg );
	for( register size_t i=0;i< FInstance->FEventList.size(); i ++ )
		 FInstance->FEventList[i]->OnUnderlyingIndex( interface_cast<IUnderlyingIndex>(UnderlyingIndexImpl));
	UnderlyingIndexImpl->Release();
}
//------------------------------------------------------------------------------
void MDConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, SumOfOrderInfo* Msg )
{
	TSumOfOrderImpl* SumOfOrderImpl = new TSumOfOrderImpl();

	SumOfOrderImpl->FromSumOfOrderInfo( Msg );
	for( register size_t i=0;i< FInstance->FEventList.size(); i ++ )
		 FInstance->FEventList[i]->OnSumOfOrder( interface_cast<ISumOfOrder>(SumOfOrderImpl) );
	SumOfOrderImpl->Release();
}
//------------------------------------------------------------------------------
void MDConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MarketDataMessage* Msg )
{
	TRecoverImpl* RecoverImpl = new TRecoverImpl();

	RecoverImpl->FromMarketDataMessage( Msg );
	for( register size_t i=0;i< FInstance->FEventList.size(); i ++ )
		 FInstance->FEventList[i]->OnRecover( interface_cast<IRecover>( RecoverImpl ));
	RecoverImpl->Release();
}
//------------------------------------------------------------------------------
void MDConnection::OnSystemMessage( const UFC::AnsiString& Exchange, SystemMessage* Message )
{
	TSystemMsgImpl* SystemMessageImpl = new TSystemMsgImpl();

	SystemMessageImpl->FromMarketDataMessage( Message );
	for( register size_t i=0;i< FInstance->FEventList.size(); i ++ )
		 FInstance->FEventList[i]->OnSystemMessage( interface_cast<ISystemMsg>(SystemMessageImpl));
	SystemMessageImpl->Release();
}
//------------------------------------------------------------------------------
void MDConnection::OnErrorMessage( const UFC::AnsiString& Exchange, ErrorMessage* Message )
{
	TErrorMsgImpl* ErrorMessageImpl = new TErrorMsgImpl( );

	ErrorMessageImpl->FromMarketDataMessage( Message );
	for( register size_t i=0;i< FInstance->FEventList.size(); i ++ )
		 FInstance->FEventList[i]->OnErrorMessage( interface_cast<IErrorMsg>(ErrorMessageImpl));
	ErrorMessageImpl->Release();
}
//------------------------------------------------------------------------------
void MDConnection::OnRecoverFinished( const UFC::AnsiString& Exchange, int Count )
{
	WideString ExStr( Exchange.c_str() );

	for( register size_t i=0;i< FInstance->FEventList.size(); i ++ )
		 FInstance->FEventList[i]->OnRecoverFinished( ExStr.Copy(), Count );
}
// ---------------------------------------------------------------------------
// TStarWaveConnectionImpl
// ---------------------------------------------------------------------------
__fastcall TStarWaveConnectionImpl::TStarWaveConnectionImpl()
:FConnection( NULL )
,FIP( "" )
,FPort( 34567 )
,FAppName( "" )
{
}
// ---------------------------------------------------------------------------
// TStarWaveConnectionImpl
// ---------------------------------------------------------------------------
__fastcall TStarWaveConnectionImpl::TStarWaveConnectionImpl(const System::_di_IInterface Controller)
: inherited(Controller)
,FConnection( NULL )
,FIP( "" )
,FPort( 34567 )
,FAppName( "" )
{
}
// ---------------------------------------------------------------------------
// TStarWaveConnectionImpl
// ---------------------------------------------------------------------------
__fastcall TStarWaveConnectionImpl::TStarWaveConnectionImpl(Comobj::TComObjectFactory* Factory,
                                      const System::_di_IInterface Controller)
: inherited(Factory, Controller)
,FConnection( NULL )
,FIP( "" )
,FPort( 34567 )
,FAppName( "" )
{
}
// ---------------------------------------------------------------------------
void __fastcall TStarWaveConnectionImpl::EventSinkChanged(const System::_di_IInterface EventSink)
{
	if( EventSink != NULL )
	{
		IStarWaveConnectionEventsDisp* NewEvents = new IStarWaveConnectionEventsDisp();

		FEventList.push_back( NewEvents );
		NewEvents->Bind( EventSink );
	}
}
// ---------------------------------------------------------------------------
// TStarWaveConnectionImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TStarWaveConnectionImpl>(Comserv::GetComServer(),
                           __classid(TStarWaveConnectionImpl),
                           CLSID_StarWaveConnection,
                           Comobj::ciMultiInstance,
                           Comobj::tmApartment);
}
#pragma startup createFactory 32
/////////////////////////////////////////////////////////////////////////////
// TStarWaveConnectionImpl
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::Connect(BSTR IP, long Port, long Timeout)
{
	try
	{
		if( FConnection != NULL )
		{
			AnsiString IPStr( IP );

			FIP   = IPStr.c_str();
			FPort = Port;
			FConnection->Connect( IPStr.c_str(), Port, Timeout );
		}
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::Disconnect()
{
	try
	{
		if( FConnection != NULL )
			FConnection->Disconnect();
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::DownloadContract()
{
	try
	{
		FConnection->Clear();
		if( FConnection != NULL )
			FConnection->DownloadContract();
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::get_AppName(BSTR* Value)
{
	try
	{
		WideString AppStr( FAppName.c_str() );
		*Value = AppStr.Copy();
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::get_IP(BSTR* Value)
{
	try
	{
		WideString IPStr( FIP.c_str() );
		*Value = IPStr.Copy();
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::get_Port(long* Value)
{
	*Value = FPort;
	 return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::GetExchange(long Index, BSTR* Exchange)
{
	try
	{
		if( FConnection != NULL )
		{
			if( Index >= 0 && Index < FConnection->ExchangeCount() )
			{
				WideString ExStr( FConnection->GetExchange( Index ).c_str() );
				*Exchange = ExStr.Copy();
			}
			else
				*Exchange = NULL;
		}
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::GetSecurityDefinition(BSTR Exchange, BSTR Symbol,
          ISecurityDefinition* SecurityDefinition, VARIANT_BOOL* IsExist)
{
  try
  {
		if( FConnection != NULL )
		{
			AnsiString ExStr( Exchange );
			AnsiString SymStr( Symbol );
			BasicInformation* Info = FConnection->GetBasicInformation( ExStr.c_str(), SymStr.c_str() );

			if( Info != NULL )
			{
				TSecurityDefinitionImpl* DefImpl = dynamic_cast<TSecurityDefinitionImpl*>(SecurityDefinition);
				DefImpl->FromBasicInfo(Info);
				*IsExist = VARIANT_TRUE;
			}
			else
			{
				*IsExist = VARIANT_FALSE;
			}
		}
  }
  catch(Exception &e)
  {
	AnsiString ExMsg( e.Message.c_str() );
	UFC::BufferedLog::Printf( " %s", ExMsg.c_str() );
	return Error(e.Message.c_str(), IID_IStarWaveConnection);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::Init()
{
  try
  {
	if( FConnection == NULL )
	{
		FAppName.Printf( "%s.%d", UFC::Hostname, UFC::GetTickCountMS() );
		FConnection = new MDConnection( this, FAppName );
	}
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveConnection);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::Subscribe(BSTR Exchange, BSTR Symbol, EnumSubscribeType SubscribeType)
{
  try
  {
	if( FConnection != NULL )
	{
		AnsiString ExStr( Exchange );
		AnsiString SymStr( Symbol );

		FConnection->Subscribe( ExStr.c_str(), SymStr.c_str(),SubscribeType );
	}
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveConnection);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::Unsubscribe(BSTR Exchange, BSTR Symbol)
{
  try
  {
	  if( FConnection != NULL )
	  {
		AnsiString ExStr( Exchange );
		AnsiString SymStr( Symbol );
		FConnection->Unsubscribe( ExStr.c_str(), SymStr.c_str() );
	  }
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveConnection);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::Destroy()
{
  try
  {
	if( FConnection != NULL )
	{
		FConnection->UnsubscribeAll();
		FConnection->Clear();
		delete FConnection;
		FConnection = NULL;
	}
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveConnection);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::SymbolCount(BSTR Exchange, long* Count)
{
  try
  {
	if( FConnection != NULL )
	{
		AnsiString ExStr( Exchange );
		*Count = FConnection->SymbolCount( ExStr.c_str() );
	}
	else
		*Count = 0;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveConnection);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::GetSymbol(BSTR Exchange, long Index, BSTR* Symbol)
{
  try
  {
	if( FConnection != NULL )
	{
		AnsiString ExStr( Exchange );
		UFC::AnsiString SymStr = FConnection->GetSymbol( ExStr.c_str(), Index );
		WideString Result( SymStr.c_str() );
		*Symbol = Result.Copy();
	}
	else
		*Symbol = NULL;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveConnection);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::UnsubscribeAll()
{
	try
	{
		if( FConnection != NULL )
		{
			FConnection->UnsubscribeAll();
		}
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::GetSecurityDefinitionByIndex(BSTR Exchange,
          long Index, ISecurityDefinition* SecurityDefinition, VARIANT_BOOL* IsExist)
{
	try
	{
		if( FConnection != NULL )
		{
			AnsiString ExStr( Exchange );
			BasicInformation* Info = FConnection->GetBasicInformation( ExStr.c_str(), Index );

			if( Info != NULL )
			{
				TSecurityDefinitionImpl* DefImpl = dynamic_cast<TSecurityDefinitionImpl*>(SecurityDefinition);
				DefImpl->FromBasicInfo(Info);
				*IsExist = VARIANT_TRUE;
			}
			else
			{
				*IsExist = VARIANT_FALSE;
			}
		}
	}
	catch(Exception &e)
	{
		AnsiString ExMsg( e.Message.c_str() );
		UFC::BufferedLog::Printf( " %s", ExMsg.c_str() );
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::SubscribeSystemMessage(BSTR Exchange)
{
	try
	{
		AnsiString ExStr( Exchange );

		if( FConnection != NULL )
			FConnection->SubscribeSystemMessage( ExStr.c_str() );
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::UnsubscribeSystemMessage(BSTR Exchange)
{
	try
	{
		AnsiString ExStr( Exchange );

		if( FConnection != NULL )
			FConnection->UnsubscribeSystemMessage( ExStr.c_str() );
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::SendNews(BSTR Group, BSTR Message)
{
	AnsiString GroupStr( Group );
	AnsiString MsgStr( Message );

	if( FConnection != NULL )
		FConnection->SendNews( GroupStr.c_str(), MsgStr.c_str() );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::SubscribeNews(BSTR Group)
{
	AnsiString GroupStr( Group );

	if( FConnection != NULL )
		FConnection->SubscribeNews( GroupStr.c_str() );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::UnsubscribeNews(BSTR Group)
{
	AnsiString GroupStr( Group );

	if( FConnection != NULL )
		FConnection->UnsubscribeNews( GroupStr.c_str() );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::Recover(BSTR Exchange)
{
	try
	{
		if( FConnection != NULL )
		{
			AnsiString ExStr( Exchange );
			FConnection->Recover( ExStr.c_str() );
		}
	}
	catch(Exception &e)
	{
		AnsiString ExMsg( e.Message.c_str() );
		UFC::BufferedLog::Printf( " %s", ExMsg.c_str() );
		return Error(e.Message.c_str(), IID_IStarWaveConnection);
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TStarWaveConnectionImpl::SubscribeX(BSTR Exchange, BSTR ProductID, BSTR SettlementMonth,
		  EnumCallPut CallPutCode, double StrikePx, EnumSubscribeType SubscribeType)
{
  try
  {
	if( FConnection != NULL )
	{
		AnsiString ExStr( Exchange );
		AnsiString ProductStr( ProductID );
		AnsiString SettleMonthStr( SettlementMonth );

		FConnection->SubscribeX( ExStr.c_str(), ProductStr.c_str(), SettleMonthStr.c_str(),
			CallPutCode, StrikePx, SubscribeType );
	}
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IStarWaveConnection);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
