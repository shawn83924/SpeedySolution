//-------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <ComObj.hpp>
#include <ActiveX.hpp>
#include <axbase.h>
#include "StarWaveActiveXImpl.h"
#pragma package(smart_init)
//------------------------------------------------------------------------------
MDAXConnection::MDAXConnection( TStarWaveActiveXImpl* Instance, const UFC::AnsiString& AppName )
:FInstance( Instance )
,FIsResponse( false )
{

	FConnection = new MarketDataConnection( HInstance, // Instance For Win32 GUI Applications. Linux, Unix ignore this param.
											AppName,   // Gives this client application a name.
											((IMarketDataConnectionEventListener*)this), // Listener to handle marketdata events.
											TRUE,      // TRUE for Win32 GUI Application, FALSE for Linux, Unix, Win32 console application.
											FALSE,     // Is debug mode
											FALSE,     // Is debug Performance mode
											32768 );   // Size of the queue to store marketdata message.( default: 8192)
	UFC::SleepMS( 330 );
}
//------------------------------------------------------------------------------
MDAXConnection::~MDAXConnection()
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
void MDAXConnection::Connect( const UFC::AnsiString& IP, int Port, int Timeout )
{
	if( FConnection != NULL )
		FConnection->Connect( IP, Port, Timeout );
}
//------------------------------------------------------------------------------
void MDAXConnection::Disconnect( void )
{
	if( FConnection != NULL )
		FConnection->Disconnect();
}
//------------------------------------------------------------------------------
bool MDAXConnection::Subscribe( UFC::AnsiString Exchange, UFC::AnsiString Symbol, EnumSubscribeType Type )
{
	UFC::AnsiString ExStr( Exchange );
	UFC::AnsiString SymStr( Symbol );

	if( FConnection != NULL )
		return FConnection->Subscribe( ExStr, SymStr, (SubscribeType)Type );
	return false;
}
//------------------------------------------------------------------------------
void MDAXConnection::SubscribeX( UFC::AnsiString Exchange, UFC::AnsiString ProductID, UFC::AnsiString SettlementMonth,
	EnumCallPut CallPut, double StrikePx, EnumSubscribeType Type )
{
	UFC::AnsiString ExStr( Exchange );
	UFC::AnsiString ProductStr( ProductID );
	UFC::AnsiString SettleMonthStr( SettlementMonth );

	if( FConnection != NULL )
		FConnection->SubscribeX( ExStr, ProductStr, SettleMonthStr, (CallPutCode)CallPut, StrikePx, (SubscribeType)Type );
}
//------------------------------------------------------------------------------
bool MDAXConnection::Recover( const UFC::AnsiString& Exchange )
{
	UFC::AnsiString ExStr( Exchange );

	if( FConnection != NULL )
		return FConnection->Recover( ExStr );
	return false;
}
//------------------------------------------------------------------------------
void MDAXConnection::Unsubscribe( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol )
{
	if( FConnection != NULL )
		FConnection->Unsubscribe( Exchange, Symbol );
}
//------------------------------------------------------------------------------
void MDAXConnection::SubscribeSystemMessage( const UFC::AnsiString& Exchange )
{
	if( FConnection != NULL )
		FConnection->SubscribeSystemMessage( Exchange );
}
//------------------------------------------------------------------------------
void MDAXConnection::UnsubscribeSystemMessage( const UFC::AnsiString& Exchange )
{
	if( FConnection != NULL )
		FConnection->UnsubscribeSystemMessage( Exchange );
}
//------------------------------------------------------------------------------
void MDAXConnection::SendNews( const UFC::AnsiString& Group, const UFC::AnsiString& Msg )
{
	if( FConnection != NULL )
		FConnection->SendNews( Group , Msg );
}
//------------------------------------------------------------------------------
void MDAXConnection::SubscribeNews( const UFC::AnsiString& Group )
{
	if( FConnection != NULL )
		FConnection->SubscribeNews( Group );
}
//------------------------------------------------------------------------------
void MDAXConnection::UnsubscribeNews( const UFC::AnsiString& Group )
{
	if( FConnection != NULL )
		FConnection->UnsubscribeNews( Group );
}
//------------------------------------------------------------------------------
void MDAXConnection::SubscribeExchangeNews( const UFC::AnsiString& Exchange )
{
	if( FConnection != NULL )
		FConnection->SubscribeExchangeNews( Exchange );
}
//------------------------------------------------------------------------------
void MDAXConnection::UnsubscribeExchangeNews( const UFC::AnsiString& Exchange )
{
	if( FConnection != NULL )
		FConnection->UnsubscribeExchangeNews( Exchange );
}
//------------------------------------------------------------------------------
int MDAXConnection::SymbolCount( const UFC::AnsiString& Exchange )
{
	if( FConnection != NULL )
		return FConnection->SymbolCount( Exchange );
	return 0;
}
//------------------------------------------------------------------------------
void MDAXConnection:: OnConnected( void )
{
//	FInstance->Fire_OnConnected();
}
//------------------------------------------------------------------------------
void MDAXConnection::OnLogonReply( bool OK,const UFC::AnsiString& Msg )
{
	UTF8String UTF8Msg( Msg.c_str() );
	WideString MsgStr( UTF8Msg );

	FIsResponse = true;
	FInstance->Fire_OnLogonReply( OK, MsgStr.c_bstr() );
	printf( "OnLoginReply[%d:%s]\n", OK, Msg.c_str() );
}
//------------------------------------------------------------------------------
void MDAXConnection::OnDisconnected( void )
{
	FInstance->Fire_OnDisconnected();
}
//------------------------------------------------------------------------------
void MDAXConnection::OnContractDownloadComplete( int ExchangeCount, int UseMS )
{
	FInstance->Fire_OnContractDownloadCompleted( ExchangeCount, UseMS );
}
//------------------------------------------------------------------------------
void MDAXConnection::OnNews(  const UFC::AnsiString& Group, const UFC::AnsiString& Msg )
{
	TNewsImpl* NewsImpl = new TNewsImpl();

	NewsImpl->FromNews( Group, Msg );
	FInstance->Fire_OnNews( interface_cast<INews>(NewsImpl) );
	//NewsImpl->Release();
}
//------------------------------------------------------------------------------
void MDAXConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MatchInfo* Msg )
{
	TMatchImpl* MatchImpl = new TMatchImpl();

	MatchImpl->FromMatchInfo( Msg );
	FInstance->Fire_OnMatchInfo(interface_cast<IMatch>(MatchImpl) );
	//MatchImpl->Release();
}
//------------------------------------------------------------------------------
void MDAXConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, TotalMatch* Msg )
{
	TTotalVolumeImpl* TotalVolumeImpl = new TTotalVolumeImpl();

	TotalVolumeImpl->FromTotalMatch( Msg );
	FInstance->Fire_OnTotalMatch( interface_cast<ITotalVolume>(TotalVolumeImpl) );
	//TotalVolumeImpl->Release();
}
//------------------------------------------------------------------------------
void MDAXConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, DayHighLowPrice* Msg )
{
	TDayHighLowImpl* DayHighLowImpl = new TDayHighLowImpl();

	DayHighLowImpl->FromDayHighLow( Msg );
	FInstance->Fire_OnDayHighLow(interface_cast<IDayHighLow>(DayHighLowImpl));
	//DayHighLowImpl->Release();
}
//------------------------------------------------------------------------------
void MDAXConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, OpeningInfo* Msg )
{
	TOpenInfoImpl* OpenInfoImpl = new TOpenInfoImpl();

	OpenInfoImpl->FromOpeningInfo( Msg );
	FInstance->Fire_OnOpeningInfo(interface_cast<IOpenInfo>(OpenInfoImpl));
	//OpenInfoImpl->Release();
}
//------------------------------------------------------------------------------
void MDAXConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, OrderBookData* Msg )
{
	TOrderBookImpl* OrderBookImpl = new TOrderBookImpl();

	OrderBookImpl->FromOrderBookData( Msg );
	FInstance->Fire_OnOrderBookData(interface_cast<IOrderBook>(OrderBookImpl));
	//OrderBookImpl->Release();
}
//------------------------------------------------------------------------------
void MDAXConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, ClosingMarketData* Msg )
{
	TCloseMarketDataImpl* CloseMarketDataImpl = new TCloseMarketDataImpl( );

	if( Msg->GetMsgType() == mtClosingMarketData )
		CloseMarketDataImpl->FromI070( Msg );
	else if( Msg->GetMsgType() == mtClosingMarketDataWithSettlementPrice )
		CloseMarketDataImpl->FromI071( (ClosingMarketDataWithSettlementPrice*)Msg );
	else if( Msg->GetMsgType() == mtClosingMarketDataWithSettlementPriceAndOpenInterest )
		CloseMarketDataImpl->FromI072( (ClosingMarketDataWithSettlementPriceAndOpenInterest*)Msg );
	FInstance->Fire_OnClosingMarketData( interface_cast<ICloseMarketData>(CloseMarketDataImpl));
	//CloseMarketDataImpl->Release();
}
//------------------------------------------------------------------------------
void MDAXConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, UnderlyingIndexInfo* Msg )
{
	TUnderlyingIndexImpl* UnderlyingIndexImpl = new TUnderlyingIndexImpl();

	UnderlyingIndexImpl->FromIndexMessage( Msg );
	FInstance->Fire_OnUnderlyingIndex( interface_cast<IUnderlyingIndex>(UnderlyingIndexImpl));
	//UnderlyingIndexImpl->Release();
}
//------------------------------------------------------------------------------
void MDAXConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, SumOfOrderInfo* Msg )
{
	TSumOfOrderImpl* SumOfOrderImpl = new TSumOfOrderImpl();

	SumOfOrderImpl->FromSumOfOrderInfo( Msg );
	FInstance->Fire_OnSumOfOrder( interface_cast<ISumOfOrder>(SumOfOrderImpl) );
	//SumOfOrderImpl->Release();
}
//------------------------------------------------------------------------------
void MDAXConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MarketDataMessage* Msg )
{
	TRecoverImpl* RecoverImpl = new TRecoverImpl();

	RecoverImpl->FromMarketDataMessage( Msg );
	FInstance->Fire_OnRecover( interface_cast<IRecover>( RecoverImpl ));
	//RecoverImpl->Release();
}
//------------------------------------------------------------------------------
void MDAXConnection::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, HKBrokerQueue* Msg )
{
	TBrokerQueueImpl* BrokerQueueImpl = new TBrokerQueueImpl();

	BrokerQueueImpl->FromBrokerQueue( Msg );
	FInstance->Fire_OnBrokerQueue( interface_cast<IBrokerQueue>( BrokerQueueImpl ));
}
//------------------------------------------------------------------------------
void MDAXConnection::OnSystemMessage( const UFC::AnsiString& Exchange, SystemMessage* Message )
{
	TSystemMsgImpl* SystemMessageImpl = new TSystemMsgImpl();

	SystemMessageImpl->FromMarketDataMessage( Message );
	FInstance->Fire_OnSystemMessage( interface_cast<ISystemMsg>(SystemMessageImpl));
	//SystemMessageImpl->Release();
}
//------------------------------------------------------------------------------
void MDAXConnection::OnErrorMessage( const UFC::AnsiString& Exchange, ErrorMessage* Message )
{
//	TErrorMsgImpl* ErrorMessageImpl = new TErrorMsgImpl( );

//	ErrorMessageImpl->FromMarketDataMessage( Message );
//	FInstance->Fire_OnErrorMessage( interface_cast<IErrorMsg>(ErrorMessageImpl));
	//ErrorMessageImpl->Release();
}
//------------------------------------------------------------------------------
void MDAXConnection::OnRecoverFinished( const UFC::AnsiString& Exchange, int Count )
{
	WideString ExStr( Exchange.c_str() );

	FInstance->Fire_OnRecoverFinished( ExStr.Copy(), Count );
}
//------------------------------------------------------------------------------
void MDAXConnection::OnReload( const UFC::AnsiString& Exchange, UFC::PStringList& IDList, const UFC::AnsiString& ReloadID )
{
	printf( " 1. OnReload Exchange[%s] UID[%s]\n",Exchange.c_str(), ReloadID.c_str() );
	if( FUIDSet.Exists( ReloadID ) == false )
	{
		FUIDSet.Add( ReloadID );
		new ReloadThread( Exchange, IDList, FConnection );
	}
}
//------------------------------------------------------------------------------
void MDAXConnection::OnReloadComplete( const UFC::AnsiString& Exchange, UFC::PStringList& SymbolList )
{
	WideString ExStr( Exchange.c_str() );

	printf( " 3. OnReloadComplete Exchange[%s]\n",Exchange.c_str() );
	FInstance->Fire_OnContractReloaded(ExStr.Copy() );
}
//------------------------------------------------------------------------------
bool MDAXConnection::Logon( const UFC::AnsiString& ID, const UFC::AnsiString& Password, const UFC::AnsiString& Version, UFC::AnsiString& Msg )
{
	FIsResponse = false;
	if( FConnection != NULL )
	{
		FConnection->SetAppversion( Version );
		return FConnection->Logon( ID, Password, Msg );
	}
	return false;
}
// ---------------------------------------------------------------------------------
const UFC::AnsiString& MDAXConnection::GetIP( void )
{
	return FConnection->GetIP();
}
// ---------------------------------------------------------------------------------
void MDAXConnection::SetIP( const UFC::AnsiString& IP )
{
	FConnection->SetIP( IP );
}
// ---------------------------------------------------------------------------------
int MDAXConnection::GetPort( void )
{
	return FConnection->GetPort();
}
// ---------------------------------------------------------------------------------
void MDAXConnection::SetPort( int port )
{
	FConnection->SetPort( port );
}
// ---------------------------------------------------------------------------------