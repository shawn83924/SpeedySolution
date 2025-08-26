// ---------------------------------------------------------------------------
// StarWaveConnectionImpl1.h : Declaration of the TStarWaveConnectionImpl
// ---------------------------------------------------------------------------
#ifndef StarWaveConnectionImplH
#define StarWaveConnectionImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "StarWaveAPI_TLB.h"

#include "MarketDataConnection.h"
#include "SecurityDefinitionImpl.h"
#include "MatchImpl.h"
#include "TotalVolumeImpl.h"
#include "DayHighLowImpl.h"
#include "OpenInfoImpl.h"
#include "SumOfOrderImpl.h"
#include "NewsImpl.h"
#include "UnderlyingIndexImpl.h"
#include "CloseMarketDataImpl.h"
#include "OrderBookImpl.h"
#include "RecoverImpl.h"
#include "SystemMsgImpl.h"
#include "ErrorMsgImpl.h"


// ---------------------------------------------------------------------------
// TStarWaveConnectionImpl     Implements IStarWaveConnection, default interface of StarWaveConnection
// Events         : Implements IStarWaveConnectionEvents, the default source interface
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : TRUE
// Description    : StarWave MarketData connection object
// ---------------------------------------------------------------------------
class MDConnection;
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TStarWaveConnectionImpl : public TCppAutoObjectEvent<IStarWaveConnection>
{
	typedef _COM_CLASS inherited;
	typedef std::vector<IStarWaveConnectionEventsDisp*> TSWEventList;
public:
	TSWEventList  FEventList;
private:
	MDConnection*   FConnection;
	UFC::AnsiString FIP;
	UFC::AnsiString FAppName;
	int             FPort;
public:
  __fastcall TStarWaveConnectionImpl();
  __fastcall TStarWaveConnectionImpl(const System::_di_IInterface Controller);
  __fastcall TStarWaveConnectionImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  void __fastcall EventSinkChanged(const System::_di_IInterface EventSink);
  

  // IStarWaveConnection
protected:
  STDMETHOD(Connect(BSTR IP, long Port, long Timeout));
  STDMETHOD(Destroy());
  STDMETHOD(Disconnect());
  STDMETHOD(DownloadContract());
  STDMETHOD(get_AppName(BSTR* Value));
  STDMETHOD(get_IP(BSTR* Value));
  STDMETHOD(get_Port(long* Value));
  STDMETHOD(GetExchange(long Index, BSTR* Exchange));
  STDMETHOD(GetSecurityDefinition(BSTR Exchange, BSTR Symbol, ISecurityDefinition* SecurityDefinition,
          VARIANT_BOOL* IsExist));
  STDMETHOD(GetSecurityDefinitionByIndex(BSTR Exchange, long Index, ISecurityDefinition* SecurityDefinition,
          VARIANT_BOOL* IsExist));
  STDMETHOD(GetSymbol(BSTR Exchange, long Index, BSTR* Symbol));
  STDMETHOD(Init());
  STDMETHOD(Recover(BSTR Exchange));
  STDMETHOD(SendNews(BSTR Group, BSTR Message));
  STDMETHOD(Subscribe(BSTR Exchange, BSTR Symbol, EnumSubscribeType SubscribeType));
  STDMETHOD(SubscribeNews(BSTR Group));
  STDMETHOD(SubscribeSystemMessage(BSTR Exchange));
  STDMETHOD(SubscribeX(BSTR Exchange, BSTR ProductID, BSTR SettlementMonth, EnumCallPut CallPutCode,
          double StrikePx, EnumSubscribeType SubscribeType));
  STDMETHOD(SymbolCount(BSTR Exchange, long* Count));
  STDMETHOD(Unsubscribe(BSTR Exchange, BSTR Symbol));
  STDMETHOD(UnsubscribeAll());
  STDMETHOD(UnsubscribeNews(BSTR Group));
  STDMETHOD(UnsubscribeSystemMessage(BSTR Exchange));
};
//------------------------------------------------------------------------------
class MDConnection : public IMarketDataConnectionEventListener
{
private:
	MarketDataConnection*    FConnection;
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
	void OnServerTimeDiffUS( UFCType::Int64 TimeDiffUS, UFCType::Int64 RTTUS ){}
	void OnNews(  const UFC::AnsiString& Group, const UFC::AnsiString& Msg );
	void OnReload( const UFC::AnsiString& Exchange, UFC::PStringList& IDList, const UFC::AnsiString& ReloadID ) {}
	void OnReload( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID ) {}
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
	void OnRecoverFinished( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID, int Count ) {}
};
//------------------------------------------------------------------------------
#endif //StarWaveConnectionImpl1H
