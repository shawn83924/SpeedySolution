// ---------------------------------------------------------------------------------
// ActiveX Control
// Declaration of TStarWaveActiveXImpl
// ---------------------------------------------------------------------------------
#ifndef StarWaveActiveXImplH
#define StarWaveActiveXImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "StarWaveAPI_TLB.h"
#include <Vcl.Controls.hpp>

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
#include "BrokerQueueImpl.h"
// ---------------------------------------------------------------------------------
// TStarWaveActiveXImpl - Exposes TCustomControl as an ActiveX Control
// Description: ActiveX
// ---------------------------------------------------------------------------------
class MDAXConnection;
// ---------------------------------------------------------------------------------
class DAX_COM_CLASS TStarWaveActiveXImpl: public TCppActiveXControl<TCustomControl,
							   IStarWaveActiveX,
							   IStarWaveActiveX_EventsDispatcher<TStarWaveActiveXImpl> >
{
private:
	MDAXConnection* FConnection;
	TTimer*         FLogonTimer;
	UFC::AnsiString FAppName;
private:
	int  __fastcall GetVersion( void );
	void __fastcall OnLogonTimer(TObject*Sender );
public:
  __fastcall TStarWaveActiveXImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  void __fastcall InitializeControl() { }
public:// IStarWaveActiveX
  STDMETHOD(DisposeOf());
  STDMETHOD(DrawTextBiDiModeFlagsReadingOnly(long* Value));
  STDMETHOD(get_AlignDisabled(VARIANT_BOOL* Value));
  STDMETHOD(get_AlignWithMargins(VARIANT_BOOL* Value));
  STDMETHOD(get_DockSite(VARIANT_BOOL* Value));
  STDMETHOD(get_DoubleBuffered(VARIANT_BOOL* Value));
  STDMETHOD(get_Enabled(VARIANT_BOOL* Value));
  STDMETHOD(get_ExplicitHeight(long* Value));
  STDMETHOD(get_ExplicitLeft(long* Value));
  STDMETHOD(get_ExplicitTop(long* Value));
  STDMETHOD(get_ExplicitWidth(long* Value));
  STDMETHOD(get_MouseInClient(VARIANT_BOOL* Value));
  STDMETHOD(get_ParentCustomHint(VARIANT_BOOL* Value));
  STDMETHOD(get_ParentDoubleBuffered(VARIANT_BOOL* Value));
  STDMETHOD(get_UseDockManager(VARIANT_BOOL* Value));
  STDMETHOD(get_Visible(VARIANT_BOOL* Value));
  STDMETHOD(get_VisibleDockClientCount(long* Value));
  STDMETHOD(InitiateAction());
  STDMETHOD(IsRightToLeft(VARIANT_BOOL* Value));
  STDMETHOD(QualifiedClassName(BSTR* Value));
  STDMETHOD(set_AlignWithMargins(VARIANT_BOOL Value));
  STDMETHOD(set_DockSite(VARIANT_BOOL Value));
  STDMETHOD(set_DoubleBuffered(VARIANT_BOOL Value));
  STDMETHOD(set_Enabled(VARIANT_BOOL Value));
  STDMETHOD(set_ParentCustomHint(VARIANT_BOOL Value));
  STDMETHOD(set_ParentDoubleBuffered(VARIANT_BOOL Value));
  STDMETHOD(set_UseDockManager(VARIANT_BOOL Value));
  STDMETHOD(set_Visible(VARIANT_BOOL Value));
  STDMETHOD(SetDesignVisible(VARIANT_BOOL Value));
  STDMETHOD(SetSubComponent(VARIANT_BOOL IsSubComponent));
  STDMETHOD(UnitScope(BSTR* Value));
  STDMETHOD(UseRightToLeftReading(VARIANT_BOOL* Value));
  STDMETHOD(UseRightToLeftScrollBar(VARIANT_BOOL* Value));
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
  STDMETHOD(Subscribe(BSTR Exchange, BSTR Symbol, EnumSubscribeType SubscribeType, VARIANT_BOOL* Value));








  STDMETHOD(SubscribeNews(BSTR Group));
  STDMETHOD(SubscribeSystemMessage(BSTR Exchange));
  STDMETHOD(SubscribeX(BSTR Exchange, BSTR ProductID, BSTR SettlementMonth, EnumCallPut CallPutCode,
          double StrikePx, EnumSubscribeType SubscribeType));
  STDMETHOD(SymbolCount(BSTR Exchange, long* Count));
  STDMETHOD(Unsubscribe(BSTR Exchange, BSTR Symbol));
  STDMETHOD(UnsubscribeAll());
  STDMETHOD(UnsubscribeNews(BSTR Group));
  STDMETHOD(UnsubscribeSystemMessage(BSTR Exchange));
  STDMETHOD(SetDebugLog(BSTR FileName));
  STDMETHOD(get_BuildDate(BSTR* Value));
  STDMETHOD(get_Version(BSTR* Value));
  STDMETHOD(Logon(BSTR ID, BSTR Password, BSTR* Msg, VARIANT_BOOL* Value));
  STDMETHOD(set_IP(BSTR Value));
  STDMETHOD(set_Port(long Value));
  STDMETHOD(SubscribeExchangeNews(BSTR Exchange));
  STDMETHOD(UnsubscribeExchangeNews(BSTR Exchange));
};
//------------------------------------------------------------------------------
class ReloadThread : public UFC::PThread
{
private:
	UFC::AnsiString       FExchange;
	UFC::PStringList      FIDList;
	MarketDataConnection* FConnection;
public:
   ReloadThread( const UFC::AnsiString& Exchange, UFC::PStringList& IDList, MarketDataConnection* Connection )
   :UFC::PThread( NULL )
   ,FExchange( Exchange )
   ,FConnection( Connection )
   {
		FIDList.CopyFrom( IDList );
		Start();
   }
protected:
	virtual void Execute( void )
	{
		UFC::SleepMS( rand()%6000 + 100 );
		printf( " 2. ReloadExchangeContract Exchange[%s]\n",FExchange.c_str() );
		FConnection->ReloadExchangeContract( FExchange, FIDList );
	}
};
//------------------------------------------------------------------------------
class MDAXConnection : public IMarketDataConnectionEventListener
{
private:
	MarketDataConnection* FConnection;
	TStarWaveActiveXImpl* FInstance;
	UFC::PHashedSet<UFC::AnsiString> FUIDSet;
	bool FIsResponse;
public:
	MDAXConnection( TStarWaveActiveXImpl* Instance, const UFC::AnsiString& AppName );
	~MDAXConnection();
public:
	const UFC::AnsiString& GetIP( void );
	void                   SetIP( const UFC::AnsiString& IP );
	int                    GetPort( void );
	void                   SetPort( int port );
public:
	bool IsResponse( void ) {return FIsResponse; }
	bool Logon( const UFC::AnsiString& ID, const UFC::AnsiString& Password, const UFC::AnsiString& Version, UFC::AnsiString& Msg );
	void Connect( const UFC::AnsiString& IP, int Port, int Timeout );
	void Disconnect( void );
	bool Subscribe( UFC::AnsiString Exchange, UFC::AnsiString Symbol, EnumSubscribeType Type );
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
	void SubscribeExchangeNews( const UFC::AnsiString& Ex );
	void UnsubscribeExchangeNews( const UFC::AnsiString& Ex );
	bool Recover( const UFC::AnsiString& Exchange );
	void SetLogFile( const char* FName ) { FConnection->SetLogFile( FName ); }
private:
	void OnConnected( void );
	void OnLogonReply( bool OK,const UFC::AnsiString& Msg );
	void OnDisconnected( void );
	void OnContractDownloadComplete( int ExchangeCount, int UseMS );
	void OnServerAck( int RTT ){}
	void OnLeaderboard( UFC::PStringList& Board ) {}
	void OnServerTimeDiffUS( UFCType::Int64 TimeDiffUS, UFCType::Int64 RTTUS ){}
	void OnNews(  const UFC::AnsiString& Group, const UFC::AnsiString& Msg );
	void OnReload( const UFC::AnsiString& Exchange, UFC::PStringList& IDList, const UFC::AnsiString& ReloadID );
	void OnReload( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID ) {}
	void OnReloadComplete( const UFC::AnsiString& Exchange, UFC::PStringList& SymbolList );
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
	void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, HKBrokerQueue* Msg );
	void OnSystemMessage( const UFC::AnsiString& Exchange, SystemMessage* Message );
	void OnErrorMessage( const UFC::AnsiString& Exchange, ErrorMessage* Message );
	void OnRecoverFinished( const UFC::AnsiString& Exchange, int Count );
	void OnRecoverFinished( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID, int Count ) {}
};
//------------------------------------------------------------------------------
#endif //StarWaveActiveXImplH

