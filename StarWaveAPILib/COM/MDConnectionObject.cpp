//---------------------------------------------------------------------------
#pragma hdrstop
#include "MDConnectionObject.h"
#include "StarWaveConnection.h"
//------------------------------------------------------------------------------
MDConnection::MDConnection( TStarWaveConnectionImpl* Instance, const UFC::AnsiString& AppName )
:FInstance( Instance )
{
	FConnection = new MarketDataConnection( (HINSTANCE)FInstance, AppName, ((IMarketDataConnectionEventListener*)this), TRUE );
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
	FInstance->Fire_OnConnected();
}
//------------------------------------------------------------------------------
void MDConnection::OnDisconnected( void )
{
	FInstance->Fire_OnDisconnected();
}
//------------------------------------------------------------------------------
void MDConnection::OnContractDownloadComplete( int ExchangeCount, int UseMS )
{
	FInstance->Fire_OnContractDownloadCompleted( ExchangeCount, UseMS );
}
//------------------------------------------------------------------------------
void MDConnection::OnNews(  const UFC::AnsiString& Group, const UFC::AnsiString& Msg )
{
	TCOMINews News = CoNews::Create();

	TNewsImpl* NewsImpl = dynamic_cast<TNewsImpl*>((INews*)News);
	NewsImpl->FromNews( Group, Msg );
	FInstance->Fire_OnNews( (INews*)News );
}
//------------------------------------------------------------------------------
void MDConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MatchInfo* Msg )
{
	TCOMIMatch match = CoMatch::Create();
	TMatchImpl* MatchImpl = dynamic_cast<TMatchImpl*>((IMatch*)match);
	MatchImpl->FromMatchInfo( Msg );
	FInstance->Fire_OnMatchInfo((IMatch*)match );
}
//------------------------------------------------------------------------------
void MDConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, TotalMatch* Msg )
{
	TCOMITotalVolume match = CoTotalVolume::Create();
	TTotalVolumeImpl* MatchImpl = dynamic_cast<TTotalVolumeImpl*>((ITotalVolume*)match);
	MatchImpl->FromTotalMatch( Msg );
	FInstance->Fire_OnTotalMatch( (ITotalVolume*)match );
}
//------------------------------------------------------------------------------
void MDConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, DayHighLowPrice* Msg )
{
	TCOMIDayHighLow HighLow = CoDayHighLow::Create();
	TDayHighLowImpl* DayHighLowImpl = dynamic_cast<TDayHighLowImpl*>((IDayHighLow*)HighLow);
	DayHighLowImpl->FromDayHighLow( Msg );
	FInstance->Fire_OnDayHighLow((IDayHighLow*)HighLow);
}
//------------------------------------------------------------------------------
void MDConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, OpeningInfo* Msg )
{
	TCOMIOpenInfo OpenInfo = CoOpenInfo::Create();
	TOpenInfoImpl* OpenInfoImpl = dynamic_cast<TOpenInfoImpl*>((IOpenInfo*)OpenInfo);
	OpenInfoImpl->FromOpeningInfo(Msg);
	FInstance->Fire_OnOpeningInfo((IOpenInfo*)OpenInfo);
}
//------------------------------------------------------------------------------
void MDConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, OrderBookData* Msg )
{
	TCOMIOrderBook OrderBook = CoOrderBook::Create();
	TOrderBookImpl* OrderBookImpl = dynamic_cast<TOrderBookImpl*>((IOrderBook*)OrderBook);
	OrderBookImpl->FromOrderBookData( Msg );
	FInstance->Fire_OnOrderBookData((IOrderBook*)OrderBook);
}
//------------------------------------------------------------------------------
void MDConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, ClosingMarketData* Msg )
{
	TCOMICloseMarketData CloseMarketData = CoCloseMarketData::Create();
	TCloseMarketDataImpl* CloseMarketDataImpl = dynamic_cast<TCloseMarketDataImpl*>((ICloseMarketData*)CloseMarketData);

	if( Msg->GetMsgType() == mtClosingMarketData )
		CloseMarketDataImpl->FromI070( Msg );
	else if( Msg->GetMsgType() == mtClosingMarketDataWithSettlementPrice )
		CloseMarketDataImpl->FromI071( (ClosingMarketDataWithSettlementPrice*)Msg );
	else if( Msg->GetMsgType() == mtClosingMarketDataWithSettlementPriceAndOpenInterest )
		CloseMarketDataImpl->FromI072( (ClosingMarketDataWithSettlementPriceAndOpenInterest*)Msg );

	FInstance->Fire_OnClosingMarketData((ICloseMarketData*)CloseMarketData);
}
//------------------------------------------------------------------------------
void MDConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, UnderlyingIndexInfo* Msg )
{
	TCOMIUnderlyingIndex Index = CoUnderlyingIndex::Create();
	TUnderlyingIndexImpl* UnderlyingIndexImpl = dynamic_cast<TUnderlyingIndexImpl*>((IUnderlyingIndex*)Index);
	UnderlyingIndexImpl->FromIndexMessage( Msg );
	FInstance->Fire_OnUnderlyingIndex((IUnderlyingIndex*)Index);
}
//------------------------------------------------------------------------------
void MDConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, SumOfOrderInfo* Msg )
{
	TCOMISumOfOrder Sum = CoSumOfOrder::Create();
	TSumOfOrderImpl* SumOfOrderImpl = dynamic_cast<TSumOfOrderImpl*>((ISumOfOrder*)Sum);
	SumOfOrderImpl->FromSumOfOrderInfo( Msg );
	FInstance->Fire_OnSumOfOrder( (ISumOfOrder*)Sum );
}
//------------------------------------------------------------------------------
void MDConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MarketDataMessage* Msg )
{
	TCOMIRecover Recover = CoRecover::Create();
	TRecoverImpl* RecoverImpl = dynamic_cast<TRecoverImpl*>((IRecover*)Recover);
	RecoverImpl->FromMarketDataMessage( Msg );
	FInstance->Fire_OnRecover((IRecover*)Recover);
}
//------------------------------------------------------------------------------
void MDConnection::OnSystemMessage( const UFC::AnsiString& Exchange, SystemMessage* Message )
{
	TCOMISystemMsg SysMsg = CoSystemMsg::Create();
	TSystemMsgImpl* SystemMessageImpl = dynamic_cast<TSystemMsgImpl*>((ISystemMsg*)SysMsg);
	SystemMessageImpl->FromMarketDataMessage( Message );
	FInstance->Fire_OnSystemMessage((ISystemMsg*)SysMsg);
}
//------------------------------------------------------------------------------
void MDConnection::OnErrorMessage( const UFC::AnsiString& Exchange, ErrorMessage* Message )
{
	TCOMIErrorMsg ErrorMsg = CoErrorMsg::Create();
	TErrorMsgImpl* ErrorMessageImpl = dynamic_cast<TErrorMsgImpl*>((IErrorMsg*)ErrorMsg);
	ErrorMessageImpl->FromMarketDataMessage( Message );
	FInstance->Fire_OnErrorMessage((IErrorMsg*)ErrorMsg);
}
//------------------------------------------------------------------------------
void MDConnection::OnRecoverFinished( const UFC::AnsiString& Exchange, int Count )
{
	WideString ExStr( Exchange.c_str() );

	FInstance->Fire_OnRecoverFinished( ExStr.Copy(), Count );
}


