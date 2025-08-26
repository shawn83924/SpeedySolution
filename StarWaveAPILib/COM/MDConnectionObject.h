//---------------------------------------------------------------------------
#ifndef MDConnectionObjectH
#define MDConnectionObjectH
//------------------------------------------------------------------------------
#include "StarWaveAPI_TLB.h"
#include "MarketDataConnection.h"
#include <system.hpp>
//------------------------------------------------------------------------------
class TStarWaveConnectionImpl;
//------------------------------------------------------------------------------
class MDConnection : public IMarketDataConnectionEventListener
{
private:
	MarketDataConnection* FConnection;
	TStarWaveConnectionImpl* FInstance;
public:
	MDConnection( TStarWaveConnectionImpl* Instance, const UFC::AnsiString& AppName );
	~MDConnection();
public:
	void Connect( const UFC::AnsiString& IP, int Port, int Timeout );
	void Disconnect( void );
	void Subscribe( UFC::AnsiString Exchange, UFC::AnsiString Symbol, EnumSubscribeType Type );
	void SubscribeX( UFC::AnsiString Exchange, UFC::AnsiString ProductID, UFC::AnsiString SettlementMonth,
		 EnumCallPut CallPut, double StrikePx, EnumSubscribeType Type );
	void Unsubscribe( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol );
	void UnsubscribeAll( void ) { FConnection->UnsubscribeAll(); }
	bool IsConnected( void ) { return FConnection->IsConnected(); }
	int ExchangeCount( void ) { return FConnection->ExchangeCount(); }
	int SymbolCount( const UFC::AnsiString& Exchange );
	UFC::AnsiString GetExchange( int Index ) { return FConnection->GetExchange( Index ); }
	BasicInformation* GetBasicInformation( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol )
								{ return FConnection->GetBasicInformation( Exchange, Symbol ); }
	BasicInformation* GetBasicInformation( const UFC::AnsiString& Exchange, UFC::Int32 Index )
								{ return FConnection->GetBasicInformation( Exchange, Index ); }
	UFC::AnsiString GetSymbol( const UFC::AnsiString& Exchange, int Index ) { return FConnection->GetSymbol( Exchange, Index ); }
	void DownloadContract( void ) { FConnection->DownloadContract(); }
	void Clear( void ) { FConnection->Clear(); }
	void SubscribeSystemMessage( const UFC::AnsiString& Exchange );
	void UnsubscribeSystemMessage( const UFC::AnsiString& Exchange );
	void SendNews( const UFC::AnsiString& Group, const UFC::AnsiString& Msg );
	void SubscribeNews( const UFC::AnsiString& Group );
	void UnsubscribeNews( const UFC::AnsiString& Group );
	void Recover( const UFC::AnsiString& Exchange );
private:
	void OnConnected( void );
	void OnDisconnected( void );
	void OnContractDownloadComplete( int ExchangeCount, int UseMS );
	void OnServerAck( int RTT ){}
	void OnNews(  const UFC::AnsiString& Group, const UFC::AnsiString& Msg );
	void OnReload( const UFC::AnsiString& Exchange, UFC::PStringList& IDList, const UFC::AnsiString& ReloadID ) {}
	void OnReloadComplete( const UFC::AnsiString& Exchange, UFC::PStringList& SymbolList ) {}
	void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MarketDataMessage* Msg );
	void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, BasicData* Msg ){}
	void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MatchInfo* Msg );
	void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, TotalMatch* Msg );
	void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, DayHighLowPrice* Msg );
	void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, OpeningInfo* Msg );
	void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, OrderBookData* Msg );
	void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, ClosingMarketData* Msg );
	void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, UnderlyingIndexInfo* Msg );
	void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, SumOfOrderInfo* Msg );
	void OnSystemMessage( const UFC::AnsiString& Exchange, SystemMessage* Message );
    void OnErrorMessage( const UFC::AnsiString& Exchange, ErrorMessage* Message );
	void OnRecoverFinished( const UFC::AnsiString& Exchange, int Count );
};
//---------------------------------------------------------------------------
#endif
