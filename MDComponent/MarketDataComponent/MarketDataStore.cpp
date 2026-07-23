//---------------------------------------------------------------------------
#include <vcl.h>
#include <float.h>
#include <math.h>
#include <WideStrUtils.hpp>
#pragma hdrstop
#include "MDComponentStrings.hpp"
#include "MarketDataStore.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
//---------------------------------------------------------------------------
//** Steps to add new Exchange **
//** 1. Add new exchange constant string
//---------------------------------------------------------------------------
// TAIWAN Exchange
//---------------------------------------------------------------------------
const char TAIFEX[] = "TAIFEX";
const char TWSE[] = "TWSE";
const char OTC[] = "OTC";
//---------------------------------------------------------------------------
// China Exchange
//---------------------------------------------------------------------------
const char SHFE[] = "SHFE";
const char CFFEX[] = "CFFEX";
const char DCE[] = "DCE";
const char CZCE[] = "CZCE";
//---------------------------------------------------------------------------
// Foreign Exchange
//---------------------------------------------------------------------------
const char SGX[] = "SGX";
const char CME[] = "CME";
const char CBOT[] = "CBOT";
const char NYMEX[] = "NYMEX";
const char NYBOT[] = "NYBOT";
const char HKEx[] = "HKEx";
const char TOCOM[] = "TOCOM";
const char OSE[] = "OSE" ;
const char TGE[] = "TGE" ;
const char LIF[] = "LIF" ;
const char LME[] = "LME" ;
const char EUX[] = "EUX" ;
const char KRX[] = "KRX" ;
const char TFX[] = "TFX" ;
const char SSE[] = "SSE" ;
const char BMD[] = "BMD" ;
//---------------------------------------------------------------------------
int SymbolCompare::Compare( void* elem1, void* elem2 )
{
	BasicInformation* Info1 = (BasicInformation*)elem1;
	BasicInformation* Info2 = (BasicInformation*)elem2;

	int Date1 = Info1->GetEndDate().ToInt();
	int	Date2 = Info2->GetEndDate().ToInt();
	if( Date1 > Date2 )
		return 1;
	else if ( Date1 < Date2 )
		return -1;
	else
		return 0;
}
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TCMarketDataStore *)
{
	new TCMarketDataStore(NULL);
}
//---------------------------------------------------------------------------
namespace Marketdatastore
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TCMarketDataStore)};
		RegisterComponents(L"Speedy", classes, 0);
	}
}
//---------------------------------------------------------------------------
//
// Class TCMarketDataStore
//
//---------------------------------------------------------------------------
__fastcall TCMarketDataStore::TCMarketDataStore(TComponent* Owner)
:TCustomControl(Owner)
,FConnection( NULL )
,FCompareRule( NULL )
,FReloadTimer( NULL )
,FIP( L"10.6.76.104" )
,FPort(34567)
,FSupportOptions( false )
,FLanguage( lSimplifiedChinese )
,FUpdateInterval( 50 )///< 20 FPS
{
	if( !ComponentState.Contains( csDesigning ) )
	{
		FCompareRule = new SymbolCompare();
		FReloadTimer = new TTimer( this );
		FReloadTimer->Enabled = false;
		FReloadTimer->OnTimer = OnReloadTimer;

		FUpdateTimer = new TTimer( this );
		FUpdateTimer->Enabled  = false;
		FUpdateTimer->Interval = 20; ///< upto 50 FPS
		FUpdateTimer->OnTimer  = OnUpdateTimer;
	}
	FChineExSet.Add( CFFEX );
	FChineExSet.Add( SHFE );
	FChineExSet.Add( DCE );
	FChineExSet.Add( CZCE );
	FChineExSet.Add( SSE );
}
//---------------------------------------------------------------------------
__fastcall TCMarketDataStore::~TCMarketDataStore()
{
	if( FReloadTimer != NULL )
		delete FReloadTimer;
	if( FUpdateTimer != NULL )
		delete FUpdateTimer;
	if( FCompareRule != NULL )
		delete FCompareRule;
	if( FConnection != NULL )
		delete FConnection;
}
//---------------------------------------------------------------------------
bool __fastcall TCMarketDataStore::IsChinaExchange( const UFC::AnsiString Exchange )
{
	return FChineExSet.Exists( Exchange );
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::Loaded(void)
{
	if( !ComponentState.Contains( csDesigning ) )
		FConnection = new MarketDataConnection( (HINSTANCE)Application->Handle, "TCMarketDataStore", this, TRUE );
	Width  = Canvas->TextWidth( Name ) + 6;
	Height = Canvas->TextHeight( Name ) + 4;
	Constraints->MaxWidth = Width;
	Constraints->MinWidth = Width;
	Constraints->MaxHeight = Height;
	Constraints->MinHeight = Height;
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::Paint(void)
{
	TTextFormat  Formats;
	String OutText( Name );

	TRect PRect(0,0,Width,Height);
	Canvas->Rectangle(0,0,Width,Height);
	Frame3D( Canvas, PRect, clSilver, clGray, 2 );
	Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter;
	Canvas->TextRect( PRect, OutText, Formats );
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::Connect( int Timeout )
{
	if( FConnection != NULL )
	{
		UFC::AnsiString IPStr( FIP.c_str());
		FConnection->Connect( IPStr, FPort, Timeout );
	}
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::Disconnect( void )
{
	if( FConnection != NULL )
		FConnection->Disconnect();
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::DownloadContract( void )
{
	if( FConnection != NULL && FConnection->IsConnected() )
	{
		UFC::AnsiString Exchange;
		UFC::PHashedList<UFC::AnsiString, ExchangeInfo*>* ExInfos;

		LoadExchangeTable(); ///< Build Exchange,Product table from Exchange.ini
		FConnection->ClearSupportExchange();
		for( int i = 0; i < FExchangeTable.ItemCount(); i++ )
		{
			if( FExchangeTable.GetItem( i, Exchange, ExInfos ) == true )
				FConnection->AddSupportExchange( Exchange );
		}
		FConnection->DownloadContract();
	}
}
//---------------------------------------------------------------------------
void TCMarketDataStore::SetLanguage( Language lang )
{
	FLanguage = lang;
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::Clear( void )
{
	///< Clear the Subscribe Table.
	for( register int i = 0; i < FSubscribeTable.ItemCount(); i++ )
	{
		UFC::List<IMarketDataListener*>* ListPtr = FSubscribeTable.GetItem(i);
		ListPtr->Clear();
		delete ListPtr;
	}
	FSubscribeTable.Clear();
	///< Clear the Market Data Table.
	for( register int i = 0; i < FTable.ItemCount(); i++ )
	{
		MarketDataMessage* Msg = FTable.GetItem(i);
		delete Msg;
	}
	FTable.Clear();
	FOptionsExSet.Clear();
	FFuturesExSet.Clear();

	for( register int i = 0; i < FPrefixMap.ItemCount(); i++ )
	{
		UFC::List<BasicInformation*>* pList = FPrefixMap.GetItem(i);
		pList->Clear();
		delete pList;
	}
	FPrefixMap.Clear();
	FQueryTable.Clear();

	for( register int i = 0; i < FStoreTable.ItemCount(); i++ )
	{
		InfoTable* DelTable = FStoreTable.GetItem( i );
		DelTable->Clear();
		delete DelTable;
	}
	FStoreTable.Clear();

	for( register int i = 0; i < FTradeSymbolTable.ItemCount(); i++ )
	{
		InfoTable* DelTable = FTradeSymbolTable.GetItem( i );
		DelTable->Clear();
		delete DelTable;
	}
	FTradeSymbolTable.Clear();

	FSymbolIndex.Clear();
	for( register int i = 0; i < FFUTPrefixTable.ItemCount(); i++ )
	{
		TStringList* stringList = FFUTPrefixTable.GetItem(i);
		stringList->Clear();
		delete stringList;
	}
	FFUTPrefixTable.Clear();

	for( register int i = 0; i < FOPTPrefixTable.ItemCount(); i++ )
	{
		TStringList* stringList = FOPTPrefixTable.GetItem(i);
		stringList->Clear();
		delete stringList;
	}
	FOPTPrefixTable.Clear();

	for( register int i = 0; i < FChineseFUTPrefixTable.ItemCount(); i++ )
	{
		TStringList* stringList = FChineseFUTPrefixTable.GetItem(i);
		stringList->Clear();
		delete stringList;
	}
	FChineseFUTPrefixTable.Clear();

	for( register int i = 0; i < FChineseOPTPrefixTable.ItemCount(); i++ )
	{
		TStringList* stringList = FChineseOPTPrefixTable.GetItem(i);
		stringList->Clear();
		delete stringList;
	}
	FChineseOPTPrefixTable.Clear();

	for( register int i = 0; i < FOPTYearMonthTable.ItemCount(); i++ )
	{
		TStringList* stringList = FOPTYearMonthTable.GetItem(i);
		stringList->Clear();
		delete stringList;
	}
	FOPTYearMonthTable.Clear();

	for( register int i = 0; i < FOPTStrikePriceTable.ItemCount(); i++ )
	{
		UFC::List<BasicInformation*>* pList = FOPTStrikePriceTable.GetItem(i);
		pList->Clear();
		delete pList;
	}
	FOPTStrikePriceTable.Clear();

	for( register int i = 0; i < FChineseTable.ItemCount(); i++ )
	{
		StringHashedList* DelList = FChineseTable.GetItem( i );
		{
			for( register int j = 0; j < DelList->ItemCount(); j++ )
			{
				UnicodeString* Name = DelList->GetItem(i);
				delete Name;
			}
		}
		DelList->Clear();
		delete DelList;
	}
	FChineseTable.Clear();
	///< Clear Alias Table
	for( register int i = 0; i < FAliasTable.ItemCount(); i++ )
	{
		InfoTable* DelList = FAliasTable.GetItem( i );
		DelList->Clear();
		delete DelList;
	}
	FAliasTable.Clear();

	FTickTable.Clear();
	FConnection->UnsubscribeAll();
	FConnection->Clear();

	UFC::PLockObject UL( FEpdateCS );
	FFilledEventMap.Clear();
	FDepthEventMap.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::Subscribe( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, IMarketDataListener* Listener, SubscribeType Type )
{
	UFC::AnsiString                  SubscribeKey( ToSubscribeKey( Exchange, Symbol ) );
	UFC::List<IMarketDataListener*>* ListenerSet;

	UFC::PLockObject Lock( FCS );
	if( Type == stSubscribe || Type == stSnapshotWithUpdate )
	{
		if( ( ListenerSet = FSubscribeTable.GetObjectByKey( SubscribeKey ) ) == NULL )
		{   ///< Subscribe an new contract.
			ListenerSet = new UFC::List<IMarketDataListener*>();
			FSubscribeTable.Add( SubscribeKey, ListenerSet );
			FConnection->Subscribe( Exchange, Symbol, Type );
		}
		else
		{
			MarketDataMessage* pMsg = FTable.GetObjectByKey( SubscribeKey );
			if( pMsg != NULL )
				Listener->OnMarketDataUpdate( pMsg );
		}
	}
	else ///< SnapShot only
	{
		MarketDataMessage* pMsg = FTable.GetObjectByKey( SubscribeKey );
		if( pMsg != NULL ) ///< Market Data object of this contract already exists.
		{
			Listener->OnMarketDataUpdate( pMsg );
			return;
		}
		if( ( ListenerSet = FSnapShotTable.GetObjectByKey( SubscribeKey ) ) == NULL )
		{   ///< Add listener to SnapShot Table
			ListenerSet = new UFC::List<IMarketDataListener*>();
			FSnapShotTable.Add( SubscribeKey, ListenerSet );
			FConnection->Subscribe( Exchange, Symbol, Type );
		}
	}
	if( ListenerSet->IndexOf( Listener ) == -1 )
		ListenerSet->Add( Listener ); ///< Add if not exist
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::Unsubscribe( IMarketDataListener* Listener )
{
	UFC::List<IMarketDataListener*>* ListenerSet;

	UFC::PLockObject   Lock( FCS );

	ListenerSet = FSubscribeTable.First();
	while( ListenerSet != NULL )
	{
		int DelIndex = ListenerSet->IndexOf( Listener ); ///< Listener exists ?
		if( DelIndex != -1 ) ///< Yes!
			ListenerSet->Delete( DelIndex );
		ListenerSet = FSubscribeTable.Next();
	}
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::Unsubscribe( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, IMarketDataListener* Listener )
{
	UFC::AnsiString                  SubscribeKey( ToSubscribeKey( Exchange, Symbol ) );
	UFC::List<IMarketDataListener*>* ListenerSet;

	UFC::PLockObject   Lock( FCS );
	///< Any listener subscribe this symbol ?
	if( ( ListenerSet = FSubscribeTable.GetObjectByKey( SubscribeKey ) ) != NULL )
	{
		int DelIndex = ListenerSet->IndexOf( Listener ); ///< Listener exists ?
		if( DelIndex != -1 ) ///< Yes!
		{
			ListenerSet->Delete( DelIndex );
			if( ListenerSet->ItemCount() == 0 ) ///< No one subscribe this symbol.
				RemoveSubscriber( Exchange, Symbol, SubscribeKey );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::RemoveSubscriber( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,const UFC::AnsiString& SubscribeKey )
{
	MarketDataMessage* pMsg;

	FConnection->Unsubscribe( Exchange, Symbol ); ///< Ask StarWave API to Unsubscribe it.
	FSubscribeTable.DeleteByKey( SubscribeKey );  ///< Remove from subscribe hashmap
	if( ( pMsg = FTable.GetObjectByKey( SubscribeKey ) ) != NULL  ) ///< Find in Snapshot object hashmap.
	{
		FTable.DeleteByKey( SubscribeKey );
		delete pMsg;
	}
}
//---------------------------------------------------------------------------
UFC::AnsiString TCMarketDataStore::ToSubscribeKey( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol )
{
	UFC::AnsiString SubscribeKey;
	SubscribeKey.Printf( "%s.%s", Exchange.c_str(), Symbol.c_str() );
	return SubscribeKey;
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::SplitSubscribeKey(  const UFC::AnsiString& SubscribeKey, UFC::AnsiString& Exchange, UFC::AnsiString& Symbol )
{
	int Pos = SubscribeKey.AnsiPos( '.' );

	if( Pos != -1 )
	{
		Exchange = SubscribeKey.SubString( 0, Pos );
		Symbol = SubscribeKey.SubString( Pos + 1, SubscribeKey.Length() - Pos );
	}
	else
	{
		Exchange = SubscribeKey;
		Symbol = SubscribeKey;
	}
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::OnReloadTimer( TObject* Sender )
{
	FReloadTimer->Enabled = false;
	FConnection->ReloadExchangeContract( FReloadExchange, FReloadIDList );
	UFC::BufferedLog::Printf( " Ask StarWave to reload %s contracts.", FReloadExchange.c_str( ) );
}
//---------------------------------------------------------------------------
//
//   Implement interface IMarketDataConnectionEventListener
//
//---------------------------------------------------------------------------
void TCMarketDataStore::OnConnected( void )
{
	FUpdateTimer->Enabled = true;
	if( FOnConnected != NULL )
		FOnConnected(this);
}
//---------------------------------------------------------------------------
void TCMarketDataStore::OnDisconnected( void )
{
	FUpdateTimer->Enabled = false;
	if( FOnDisconnected != NULL )
		FOnDisconnected(this);
}
//---------------------------------------------------------------------------
void TCMarketDataStore::OnServerAck( int RTT )
{
	if( FOnServerAck != NULL )
		FOnServerAck( RTT );
}
//---------------------------------------------------------------------------
void TCMarketDataStore::OnLeaderboard( UFC::PStringList& Board )
{
	if( FOnLeaderBoard != NULL )
	{
		TStringList* Leaders = new TStringList();
		for( int i = 0; i < Board.ItemCount(); i++ )
			 Leaders->Add( Board[i].c_str() );
		FOnLeaderBoard( Leaders );
        delete Leaders;
	}
}
//------------------------------------------------------------------------------
void TCMarketDataStore::OnServerTimeDiffUS( UFCType::Int64 TimeDiffUS, UFCType::Int64 RTTUS ) {}
//------------------------------------------------------------------------------
void TCMarketDataStore::OnNews( const UFC::AnsiString& , const UFC::AnsiString&  ){}
//------------------------------------------------------------------------------
void TCMarketDataStore::OnSystemMessage( const UFC::AnsiString& Exchange, SystemMessage* Message ) {}
//------------------------------------------------------------------------------
void TCMarketDataStore::OnReload( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID ){}
//------------------------------------------------------------------------------
void TCMarketDataStore::OnRecoverFinished( const UFC::AnsiString& Exchange, int Count ) {}
//------------------------------------------------------------------------------
void TCMarketDataStore::OnRecoverFinished( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID, int Count ) {}
//------------------------------------------------------------------------------
void TCMarketDataStore::OnErrorMessage( const UFC::AnsiString& Exchange, ErrorMessage* Message ) {}
//------------------------------------------------------------------------------
void TCMarketDataStore::OnContractDownloadComplete( int ExchangeCount, int UseMS )
{
	UFCType::UInt32 BeginTime = UFC::GetTickCountMS( );

	SetDisplayName();
	BuildQueryTable();
	BuildAliasTable();

	if( FOnContractDownloadComplete != NULL )
		FOnContractDownloadComplete( ExchangeCount, UFC::GetTickCountMS( ) - BeginTime + UseMS  );
}
//---------------------------------------------------------------------------
void TCMarketDataStore::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MarketDataMessage* Msg )
{
	UFC::AnsiString                  SubscribeKey( ToSubscribeKey( Exchange, Symbol ) );
	MarketDataMessage*               pMsg;
	UFC::List<IMarketDataListener*>* ListenerSet;

	UFC::PLockObject Lock( FCS );
	///< For Subscriber
	if( ( ListenerSet = FSubscribeTable.GetObjectByKey( SubscribeKey ) ) != NULL )
	{
		register IMarketDataListener* Listener;

		for( register int i = 0; i < ListenerSet->ItemCount(); i++ )
		{
			if( ( Listener = ListenerSet->GetItem(i) ) != NULL )
				Listener->OnMarketDataUpdate( Msg );
		}
	}
	///< For SnapShot
	if( ( ListenerSet = FSnapShotTable.GetObjectByKey( SubscribeKey ) ) != NULL )
	{
		for( register int i = 0; i < ListenerSet->ItemCount(); i++ )
		{
			register IMarketDataListener* Listener;

			if( (Listener = ListenerSet->GetItem(i) ) != NULL )
				Listener->OnMarketDataUpdate( Msg );
		}
		ListenerSet->Clear();
		FSnapShotTable.DeleteByKey( SubscribeKey );
		delete ListenerSet;
	}
	///< Add MarketData object to MarketData Table.
	if( ( pMsg = FTable.GetObjectByKey( SubscribeKey ) ) != NULL )
	{
		FTable.DeleteByKey( SubscribeKey );
		delete pMsg;
	}
	pMsg = new MarketDataMessage( *Msg );
	FTable.Add( SubscribeKey, pMsg );
	UpdateTimer( Exchange, Msg->GetTradeTime() );
}
//---------------------------------------------------------------------------
void TCMarketDataStore::OnReload( const UFC::AnsiString& Exchange, UFC::PStringList& IDList,const UFC::AnsiString& UID )
{
	if( FReloadUIDSet.Exists( UID ) == false )
	{
		FReloadUIDSet.Add( UID );
		FReloadExchange = Exchange;
		FReloadIDList.Clear();
		FReloadIDList.CopyFrom( IDList );
		FReloadTimer->Interval = rand()%5000 + 3000;
		if( FOnContractReload != NULL )
			FOnContractReload( Exchange );
		FReloadTimer->Enabled = true;
		UFC::BufferedLog::Printf( " Reload %s contract after %d ms.", Exchange.c_str( ), FReloadTimer->Interval );
	}
}
//---------------------------------------------------------------------------
void TCMarketDataStore::OnReloadComplete( const UFC::AnsiString& Exchange, UFC::PStringList& SymbolList )
{
	UFC::BufferedLog::Printf( " Reload %s %d contracts completed.", Exchange.c_str( ), SymbolList.ItemCount() );
	if( FOnContractReloadComplete != NULL )
		FOnContractReloadComplete( Exchange, SymbolList );
}
//---------------------------------------------------------------------------
void TCMarketDataStore::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, BasicData* Msg )
{
	UFC::AnsiString                  SubscribeKey( ToSubscribeKey( Exchange, Symbol ) );
	MarketDataMessage*               pMsg;
	UFC::List<IMarketDataListener*>* ListenerSet;

	UFC::PLockObject Lock( FCS );
	if( ( ListenerSet = FSubscribeTable.GetObjectByKey( SubscribeKey ) ) != NULL )
	{
		register IMarketDataListener* Listener;

		for( register int i = 0; i < ListenerSet->ItemCount(); i++ )
		{
			if( (Listener = ListenerSet->GetItem(i)) != NULL )
				Listener->OnMarketDataUpdate( Msg );
		}
	}
}
//------------------------------------------------------------------------------
// Total matched message
//------------------------------------------------------------------------------
void TCMarketDataStore::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, TotalMatch* Msg )
{
	MarketDataMessage*               pMsg;
	UFC::AnsiString                  SubscribeKey( ToSubscribeKey( Exchange, Symbol ) );
	UFC::List<IMarketDataListener*>* ListenerSet;

	UFC::PLockObject Lock( FCS );
	if( ( ListenerSet = FSubscribeTable.GetObjectByKey( SubscribeKey ) ) != NULL )
	{
		register IMarketDataListener* Listener;

		for( register int i = 0; i < ListenerSet->ItemCount(); i++ )
		{
			if(( Listener = ListenerSet->GetItem(i)) != NULL )
				Listener->OnMarketDataUpdate( Msg );
		}
	}
	///< Update Marketdata object.
	if( ( pMsg = FTable.GetObjectByKey( SubscribeKey ) ) != NULL )
		pMsg->SetTradeVolume( Msg->GetTotalMatchQty() );
}
//---------------------------------------------------------------------------
void TCMarketDataStore::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, DayHighLowPrice* Msg )
{
	MarketDataMessage*               pMsg;
	UFC::AnsiString                  SubscribeKey( ToSubscribeKey( Exchange, Symbol ) );
	UFC::List<IMarketDataListener*>* ListenerSet;

	UFC::PLockObject Lock( FCS );
	if( ( ListenerSet = FSubscribeTable.GetObjectByKey( SubscribeKey ) ) != NULL )
	{
		register IMarketDataListener* Listener;

		for( register int i = 0; i < ListenerSet->ItemCount(); i++ )
		{
			if(( Listener = ListenerSet->GetItem(i)) != NULL )
				Listener->OnMarketDataUpdate( Msg );
		}
	}
	///< Update Marketdata object.
	if( ( pMsg = FTable.GetObjectByKey( SubscribeKey ) ) != NULL )
	{
		pMsg->SetDayHighPrice( Msg->GetDayHighPrice() );
		pMsg->SetDayLowPrice( Msg->GetDayLowPrice() );
	}
}
//---------------------------------------------------------------------------
void TCMarketDataStore::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, OpeningInfo* Msg )
{
	MarketDataMessage*               pMsg;
	UFC::AnsiString                  SubscribeKey( ToSubscribeKey( Exchange, Symbol ) );
	UFC::List<IMarketDataListener*>* ListenerSet;

	UFC::PLockObject Lock( FCS );
	if( ( ListenerSet = FSubscribeTable.GetObjectByKey( SubscribeKey ) ) != NULL )
	{
		register IMarketDataListener* Listener;

		for( register int i = 0; i < ListenerSet->ItemCount(); i++ )
		{
			if(( Listener = ListenerSet->GetItem(i)) != NULL )
				Listener->OnMarketDataUpdate( Msg );
		}
	}
	///< Update Marketdata object.
	if( ( pMsg = FTable.GetObjectByKey( SubscribeKey ) ) != NULL )
		pMsg->SetOpeningPrice( Msg->GetOpeningPrice() );
}
//------------------------------------------------------------------------------
void __fastcall TCMarketDataStore::TriggerOnFilled( MatchInfo* MatchMsg )
{
	UFC::AnsiString SubscribeKey( ToSubscribeKey( MatchMsg->GetExchange(), MatchMsg->GetSymbol() ) );
	UFC::List<IMarketDataListener*>* ListenerSet;

	if( ( ListenerSet = FSubscribeTable.GetObjectByKey( SubscribeKey )) != NULL )
	{
		register IMarketDataListener* Listener;

		for( register int i = 0; i < ListenerSet->ItemCount(); i++ )
		{
			if(( Listener = ListenerSet->GetItem(i)) != NULL )
				Listener->OnMarketDataUpdate( MatchMsg );
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TCMarketDataStore::TriggerOnDepth( OrderBookData* OrderBookMsg )
{
	UFC::AnsiString SubscribeKey( ToSubscribeKey( OrderBookMsg->GetExchange(), OrderBookMsg->GetSymbol() ) );
	UFC::List<IMarketDataListener*>* ListenerSet;

	if( ( ListenerSet = FSubscribeTable.GetObjectByKey( SubscribeKey )) != NULL )
	{
		register IMarketDataListener* Listener;
		for( register int i = 0; i < ListenerSet->ItemCount(); i++ )
		{
			if(( Listener = ListenerSet->GetItem(i)) != NULL )
				Listener->OnMarketDataUpdate( OrderBookMsg );
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TCMarketDataStore::UpdateFilled( void )
{
	MarketDataMessage*               pMsg;
	UFCType::UInt32                  Now = UFC::GetTickCountMS();
	UFC::List<MarketDataMessage*>    RemoveList;
	UFC::PLockObject                 UL( FEpdateCS );

	pMsg = FFilledEventMap.First();
	while( pMsg != NULL )
	{
		int Diff = Now - pMsg->GetFilledUpdateTick();

		if( Diff > FUpdateInterval || Now < 0 )
		{
			MatchInfo MatchMsg;

			RemoveList.Add( pMsg );
			pMsg->CopyFilled( MatchMsg );
			pMsg->SetFilledUpdateTick( Now );
			UFC::PLockObject Lock( FCS );
			TriggerOnFilled( &MatchMsg );
		}
		pMsg = FFilledEventMap.Next();
	};
	for( int i = 0; i < RemoveList.ItemCount(); i++ )
		 FFilledEventMap.DeleteByKey( RemoveList[i]->GetSymbol() );
}
//------------------------------------------------------------------------------
void __fastcall TCMarketDataStore::UpdateDepth( void )
{
	MarketDataMessage*               pMsg;
	UFCType::UInt32                  Now = UFC::GetTickCountMS();
	UFC::PStringList                 RemoveList;
	UFC::PLockObject                 UL( FEpdateCS );

	pMsg = FDepthEventMap.First();
	while( pMsg != NULL )
	{
		int Diff = Now - pMsg->GetDepthUpdateTick();

		if( Diff > FUpdateInterval || Now < 0 )
		{
			OrderBookData OrderBookMsg;

			RemoveList.Add( pMsg->GetSymbol() );
			pMsg->CopyDepth( OrderBookMsg );
			pMsg->SetDepthUpdateTick( Now );
			UFC::PLockObject Lock( FCS );
			TriggerOnDepth( &OrderBookMsg );
		}
		pMsg = FDepthEventMap.Next();
	};
	for( int i = 0; i < RemoveList.ItemCount(); i++ )
	{
		if( RemoveList[i].Length() > 3 )
			FDepthEventMap.DeleteByKey( RemoveList[i] );
	}
}
//------------------------------------------------------------------------------
void __fastcall TCMarketDataStore::OnUpdateTimer( TObject* Sender )
{
	 UpdateFilled( );
	 UpdateDepth( );
}
//------------------------------------------------------------------------------
//   Filled message
//------------------------------------------------------------------------------
void TCMarketDataStore::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MatchInfo* Msg )
{
	MarketDataMessage*  pMsg;
	UFCType::UInt32     Now = UFC::GetTickCountMS();
	UFC::PLockObject    Lock( FCS );

	if( ( pMsg = FTable.GetObjectByKey( ToSubscribeKey( Exchange, Symbol ) ) ) != NULL )
	{
		int Diff = Now - pMsg->GetFilledUpdateTick();

		///< Update Marketdata object.
		pMsg->SetTradePrice( Msg->GetMatchPrice() );
		pMsg->SetTradeQty( Msg->GetMatchQty() );
		pMsg->SetTradeTime( Msg->GetMatchTime() );
		///< Trigger events
		if( Diff > FUpdateInterval || Diff < 0 )
		{
			pMsg->SetFilledUpdateTick( Now );
			TriggerOnFilled( Msg );
		}
		else ///< Add to buffer
		{
			UFC::PLockObject UL( FEpdateCS );
			FFilledEventMap.Add( Symbol, pMsg );
		}
	}
}
//------------------------------------------------------------------------------
//   OrderBook message
//---------------------------------------------------------------------------
void TCMarketDataStore::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, OrderBookData* Msg )
{
	MarketDataMessage* pMsg;
	UFCType::UInt32    Now = UFC::GetTickCountMS();
	UFC::PLockObject   Lock( FCS );

	if( ( pMsg = FTable.GetObjectByKey( ToSubscribeKey( Exchange, Symbol ) ) ) != NULL )
	{
		int Diff = Now - pMsg->GetDepthUpdateTick();

		///< Update Marketdata object.
		for( register int i = 1; i <= MAX_DEPTH; i++ )
		{
			double BidPx, AskPx;
			int BidQty, AskQty;

			if( Msg->GetBuyDepth( i, BidPx, BidQty ) )
				pMsg->SetBuyDepth( i, BidPx, BidQty );
			if( Msg->GetSellDepth( i, AskPx, AskQty ) )
				pMsg->SetSellDepth( i, AskPx, AskQty );
			Msg->SetBuyDepth( i, pMsg->GetBuyPx( i ), pMsg->GetBuyQty( i ) );
			Msg->SetSellDepth( i, pMsg->GetSellPx( i ), pMsg->GetSellQty( i ) );
		}
		pMsg->SetDerivedFlag( Msg->GetDerivedFlag() );
		if( Msg->GetDerivedFlag() )
		{
			pMsg->SetDerivedBuyPrice( Msg->GetDerivedBuyPrice() );
			pMsg->SetDerivedBuyQty( Msg->GetDerivedBuyQty() );
			pMsg->SetDerivedSellPrice( Msg->GetDerivedSellPrice() );
			pMsg->SetDerivedSellQty( Msg->GetDerivedSellQty() );
		}
		///< Trigger events
		if( Diff > FUpdateInterval || Diff < 0 )
		{
			UpdateTimer( Exchange, Msg->GetMessageTime() );
			pMsg->SetDepthUpdateTick( Now );
			TriggerOnDepth( Msg );
		}
		else
		{
			UFC::PLockObject UL( FEpdateCS );
			FDepthEventMap.Add( Symbol, pMsg );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::UpdateTimer( const UFC::AnsiString& Exchange, const UFC::AnsiString& Time )
{
	int hh,mm,ss;

	if( Time.Length() >= 6 )
	{
		hh = UFC::AnsiString::StrToInt32( Time.c_str()    , 2 );
		if( Time[2] == ':' ) ///< Format HH:MM:SS
		{
			mm = UFC::AnsiString::StrToInt32( Time.c_str() + 3, 2 );
			ss = UFC::AnsiString::StrToInt32( Time.c_str() + 6, 2 );
		}
		else ///< Format HHMMSS
		{
			mm = UFC::AnsiString::StrToInt32( Time.c_str() + 2, 2 );
			ss = UFC::AnsiString::StrToInt32( Time.c_str() + 4, 2 );
		}
		if( hh > 23 || mm > 59 || ss > 59 )
			return;
		SetClock( Exchange, hh, mm, ss );
	}
}
//---------------------------------------------------------------------------
void TCMarketDataStore::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, ClosingMarketData* Msg )
{
	MarketDataMessage*               pMsg;
	UFC::AnsiString                  SubscribeKey( ToSubscribeKey( Exchange, Symbol ) );
	UFC::List<IMarketDataListener*>* ListenerSet;

	UFC::PLockObject Lock( FCS );
	if( ( ListenerSet = FSubscribeTable.GetObjectByKey( SubscribeKey ) ) != NULL )
	{
		register IMarketDataListener* Listener;

		for( register int i = 0; i < ListenerSet->ItemCount(); i++ )
		{
			if( ( Listener = ListenerSet->GetItem(i)) != NULL )
				Listener->OnMarketDataUpdate( Msg );
		}
	}
	///< Update Marketdata object.
	if( ( pMsg = FTable.GetObjectByKey( SubscribeKey ) ) != NULL )
	{
		pMsg->SetClosingPrice( Msg->GetClosePrice() );
		if( pMsg->GetMsgType() == mtClosingMarketDataWithSettlementPrice || Msg->GetMsgType() == mtClosingMarketDataWithSettlementPriceAndOpenInterest )
			pMsg->SetSettlementPrice( ( ( ClosingMarketDataWithSettlementPrice*)Msg )->GetSettlementPrice() );
		if( Msg->GetMsgType() == mtClosingMarketDataWithSettlementPriceAndOpenInterest )
			pMsg->SetOpenInterest( ( ( ClosingMarketDataWithSettlementPriceAndOpenInterest*)Msg)->GetOpenInterest() );
	}
}
//---------------------------------------------------------------------------
void TCMarketDataStore::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, UnderlyingIndexInfo* Msg )
{
	MarketDataMessage*               pMsg;
	UFC::AnsiString                  SubscribeKey( ToSubscribeKey( Exchange, Symbol ) );
	UFC::List<IMarketDataListener*>* ListenerSet;

	UFC::PLockObject Lock( FCS );
	if( ( ListenerSet = FSubscribeTable.GetObjectByKey( SubscribeKey ) ) != NULL )
	{
		for( register int i = 0; i < ListenerSet->ItemCount(); i++ )
		{
			IMarketDataListener* Listener = ListenerSet->GetItem(i);
			if( Listener != NULL )
				Listener->OnMarketDataUpdate( Msg );
		}
	}
	///< Update Marketdata object.
	if( ( pMsg = FTable.GetObjectByKey( SubscribeKey ) ) != NULL )
	{
		pMsg->SetBuyDepth( 1, Msg->GetIndexValue() , 1 );
		pMsg->SetSellDepth( 1, Msg->GetIndexValue() , 1 );
		pMsg->SetTradePrice( Msg->GetIndexValue() );
	}
}
//---------------------------------------------------------------------------
void TCMarketDataStore::OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, SumOfOrderInfo* Msg )
{
	MarketDataMessage*               pMsg;
	UFC::AnsiString                  SubscribeKey( ToSubscribeKey( Exchange, Symbol ) );
	UFC::List<IMarketDataListener*>* ListenerSet;

	UFC::PLockObject Lock( FCS );
	if( ( ListenerSet = FSubscribeTable.GetObjectByKey( SubscribeKey ) ) != NULL )
	{
		for( register int i = 0; i < ListenerSet->ItemCount(); i++ )
		{
			IMarketDataListener* Listener = ListenerSet->GetItem(i);
			if( Listener != NULL )
				Listener->OnMarketDataUpdate( Msg );
		}
	}
	///< Update Marketdata object.
	if( ( pMsg = FTable.GetObjectByKey( SubscribeKey ) ) != NULL )
	{
		pMsg->SetBuyTotalQty( Msg->GetBuyQty() );
		pMsg->SetSellTotalQty( Msg->GetSellQty() );
	}
}
//------------------------------------------------------------------------------
//   End Implement interface IMarketDataConnectionEventListener
//------------------------------------------------------------------------------
bool __fastcall TCMarketDataStore::PriceEqual( double Px1, double Px2 )
{
	double diff = fabs( Px1 -Px2 );
	if( diff > 0.001 )
		return false;
	return true;
}
//---------------------------------------------------------------------------
int __fastcall TCMarketDataStore::ExchangeCount( void )
{
	return FConnection->ExchangeCount();
}
//---------------------------------------------------------------------------
int __fastcall TCMarketDataStore::SymbolCount( const UFC::AnsiString& Exchange )
{
	InfoTable* infoTable = FStoreTable.GetObjectByKey( Exchange );
	if( infoTable )
		return infoTable->ItemCount();
	return 0;
}
//---------------------------------------------------------------------------
int __fastcall TCMarketDataStore::SymbolCount( String Exchange )
{
	UFC::AnsiString Ex( Exchange.c_str());
	return SymbolCount( Ex ) ;
}
//---------------------------------------------------------------------------
UFC::AnsiString __fastcall TCMarketDataStore::GetExchange( UFC::Int32 Index )
{
	return FConnection->GetExchange( Index );
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::AddToStoreTable( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, BasicInformation* Info )
{
	InfoTable* infoTable      = FStoreTable.GetObjectByKey( Exchange );
	InfoTable* TradeInfoTable = FTradeSymbolTable.GetObjectByKey( Exchange );

	if( infoTable == NULL )
		FStoreTable.Add( Exchange, infoTable = new InfoTable() );

	if( TradeInfoTable == NULL )
		FTradeSymbolTable.Add( Exchange, TradeInfoTable = new InfoTable() );

	infoTable->Add( Symbol, Info );
	if( Info->GetTradeFlag() )
		TradeInfoTable->Add( Info->GetTradeSymbol(), Info );
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::AddToAliasTable( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, BasicInformation* Info )
{
	InfoTable* infoTable = FAliasTable.GetObjectByKey( Exchange );
	if( infoTable == NULL )
	{
		infoTable = new InfoTable();
		FAliasTable.Add( Exchange, infoTable );
	}
	infoTable->Add( Symbol, Info );
}
//---------------------------------------------------------------------------
UFC::AnsiString __fastcall TCMarketDataStore::GetSymbol( const UFC::AnsiString& Exchange, UFC::Int32 Index )
{
	InfoTable* infoTable = FStoreTable.GetObjectByKey( Exchange );
	BasicInformation* Info = NULL;

	if( infoTable != NULL )
	{
		Info = infoTable->GetItem( Index );
		if( Info == NULL )
			throw UFC::Exception( "Symbol not found!" );
	}
	else
		throw UFC::Exception( "Exchange not found!" );

	return Info->GetSymbol();
}
//---------------------------------------------------------------------------
BasicInformation* __fastcall TCMarketDataStore::GetBasicInformation( String Exchange, String Symbol, bool ThrowException )
{
	UFC::AnsiString Ex( Exchange.c_str() );
	UFC::AnsiString Sym( Symbol.c_str() );

	return GetBasicInformation( Ex, Sym, ThrowException );
}
//---------------------------------------------------------------------------
BasicInformation* __fastcall TCMarketDataStore::GetBasicInformation( String Exchange, UFC::Int32 Index, bool ThrowException )
{
	UFC::AnsiString Ex( Exchange.c_str() );

	return GetBasicInformation( Ex, Index, ThrowException );
}
//---------------------------------------------------------------------------
BasicInformation* __fastcall TCMarketDataStore::GetBasicInformation( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, bool ThrowException )
{
	InfoTable*        infoTable = FStoreTable.GetObjectByKey( Exchange );
	BasicInformation* Info = NULL;
	if( infoTable != NULL )
	{
		Info = infoTable->GetObjectByKey( Symbol );

		if( Info == NULL )
		{
			infoTable = FAliasTable.GetObjectByKey( Exchange );
			if( infoTable )
			{
				Info = infoTable->GetObjectByKey( Symbol );
				if( Info == NULL )
				{
					infoTable = FTradeSymbolTable.GetObjectByKey( Exchange );
					if( infoTable )
					{
						Info = infoTable->GetObjectByKey( Symbol );
						if( Info == NULL && ThrowException == true )
							throw UFC::Exception( "Symbol not found!" );
					}
				}
			}
			else
			{
				if( ThrowException == true )
					throw UFC::Exception( "Exchange not found!" );
			}
		}
	}
	else
	{
		if( ThrowException == true )
			throw UFC::Exception( "Exchange not found!" );
	}
	return Info;
}
//---------------------------------------------------------------------------
BasicInformation* __fastcall TCMarketDataStore::GetBasicInformation( const UFC::AnsiString& Exchange, UFC::Int32 Index, bool ThrowException )
{
	InfoTable*        infoTable = FStoreTable.GetObjectByKey( Exchange );
	BasicInformation* Info = NULL;

	if( infoTable != NULL )
	{
		Info = infoTable->GetItem( Index );
		if( Info == NULL && ThrowException == true )
			throw UFC::Exception( "Symbol not found!" );
	}
	else
	{
		if( ThrowException == true )
			throw UFC::Exception( "Exchange not found!" );
	}
	return Info;
}
//---------------------------------------------------------------------------
bool __fastcall TCMarketDataStore::IsConnected( void )
{
	return FConnection->IsConnected();
}
//---------------------------------------------------------------------------
int __fastcall TCMarketDataStore::GetDigit( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol )
{
	BasicInformation* Info = GetBasicInformation( Exchange, Symbol, false );

	if( Info != NULL )
		return Info->GetDigit();
	return -1;
}
//---------------------------------------------------------------------------
int __fastcall TCMarketDataStore::GetDigit( UnicodeString Exchange, UnicodeString Symbol )
{
	UFC::AnsiString Ex( Exchange.c_str());
	UFC::AnsiString Sym( Symbol.c_str());

	return GetDigit( Ex, Sym );
}
//---------------------------------------------------------------------------
int __fastcall TCMarketDataStore::GetStrikePriceDigit( const UFC::AnsiString& Exchange, const UFC::AnsiString& SeriesOrSymbol )
{
	if( Exchange == TAIFEX || Exchange == TFX )
	{
		if( SeriesOrSymbol[2] == 'O')
		{
			UFC::AnsiString Prefix( SeriesOrSymbol.SubString( 0, 2 ) );
			if( Prefix == "TX" || Prefix == "TF" || Prefix == "TG" || Prefix == "XI" )
				return 0;
		}
		else
		{
			UFC::AnsiString Prod( SeriesOrSymbol.SubString( 0, 3 ) );
			if( Prod == "TX1" || Prod == "TX2" || Prod == "TX4" || Prod == "TX5" )
				return 0;
		}
		return 1;
	}
	else if( Exchange == KRX )
	{
		if( SeriesOrSymbol.AnsiPos( "KS" ) >= 0 ) // KOSPI 200 Options
			return 1;
	}
	else if( Exchange == SSE )                   /// <- SSE Exchange
	{
		return 2;
	}
	return 1;
}
//---------------------------------------------------------------------------
int __fastcall TCMarketDataStore::GetStrikePriceDigit( UnicodeString Exchange, UnicodeString SeriesOrSymbol )
{
	UFC::AnsiString Ex( Exchange.c_str() );
	UFC::AnsiString Sym( SeriesOrSymbol.c_str() );
	return GetStrikePriceDigit( Ex, Sym );
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::BuildQueryTable( void )
{
	FSupportOptions = false;

	for( register int i = 0; i < ExchangeCount(); i++ )
	{
		UFC::AnsiString Exchange( GetExchange(i) );
		UFC::PHashedList<UFC::AnsiString, BasicInformation*>* ExSymbols;

		if( (ExSymbols = FConnection->GetSymbolsByExchange( Exchange )) != NULL )
		{
			BasicInformation* Info = ExSymbols->First();
			while( Info != NULL )
			{
				UFC::AnsiString Symbol = Info->GetSymbol();

				if( Info->GetTradeFlag() ) ///< It's a tradanble contract.
				{
					UnicodeString Chinese = GetChinesePrefix( Exchange, Info->GetProductID() );
					UFC::AnsiString Key, Key2, Key3;
					 /// Futures
					if( Info->GetMarket() == mTWFutures || Info->GetMarket() == mCNFutures || Info->GetMarket() == mPATSFutures )
					{
						FFuturesExSet.Add( Exchange );
						if( Symbol.Length() == 8 && ( Exchange == TAIFEX || Exchange == TFX ) )
							Key.Printf( "%s.%s.%s.%s", Exchange.c_str(), Info->GetProductID().c_str(), Info->GetMaturityDate().c_str(), Info->GetMaturityDate2().c_str() );
						else
							Key.Printf( "%s.%s.%s", Exchange.c_str(), Info->GetProductID().c_str(), Info->GetMaturityDate().c_str() );

						Key2.Printf( "%s.%s", Exchange.c_str(), Info->GetProductID().c_str() );

						TStringList* PrefixList;
						TStringList* ChinesePrefixList;

						if( ( PrefixList = FFUTPrefixTable.GetObjectByKey( Exchange ) ) == NULL )
						{
							PrefixList = new TStringList();
							ChinesePrefixList = new TStringList();
							FFUTPrefixTable.Add( Exchange, PrefixList );
							FChineseFUTPrefixTable.Add( Exchange, ChinesePrefixList );
						}

						if( PrefixList->IndexOf( Info->GetProductID().c_str() ) < 0 )
						{
							PrefixList->Add( String( Info->GetProductID().c_str() ) );
							ChinesePrefixList->Add( String( Info->GetProductID().c_str() ) );
						}

						FQueryTable.Add( Key, Info );
						UFC::List<BasicInformation*>* List = FPrefixMap.GetObjectByKey( Key2 );
						if( List == NULL )
						{
							List = new UFC::List<BasicInformation*>;
							FPrefixMap.Add( Key2, List );
						}
						List->Add( Info );
					} ///< Options
					else if( Info->GetMarket() == mTWOptions || Info->GetMarket() == mCNOptions || Info->GetMarket() == mPATSOptions )
					{
						FOptionsExSet.Add( Exchange );
						FSupportOptions = true;
						Key.Printf( "%s.%s.%s.%d.%c", Exchange.c_str(), Info->GetProductID().c_str(), Info->GetMaturityDate().c_str(), UFC::DoubleToInt( Info->GetStrikePrice(), 4 ), (Info->GetCallPut() == cpCall)?'C':'P' );
						Key2.Printf( "%s.%s", Exchange.c_str(), Info->GetProductID().c_str() );
						Key3.Printf( "%s.%s.%s", Exchange.c_str(), Info->GetProductID().c_str(), Info->GetMaturityDate().c_str() );

						TStringList* PrefixList;
						TStringList* ChinesePrefixList;
						if( ( PrefixList = FOPTPrefixTable.GetObjectByKey( Exchange ) ) == NULL )
						{
							PrefixList = new TStringList();
							ChinesePrefixList = new TStringList();
							FOPTPrefixTable.Add( Exchange, PrefixList );
							FChineseOPTPrefixTable.Add( Exchange, ChinesePrefixList );
						}

						if( PrefixList->IndexOf( Info->GetProductID().c_str() ) < 0 )
						{
							PrefixList->Add( String( Info->GetProductID().c_str() ) );
							ChinesePrefixList->Add( String( Info->GetProductID().c_str() ) );
						}

						FQueryTable.Add( Key, Info );
						///< TAIFEX.TXO ==> 200911, 200912, 201001, etc.
						TStringList* DateList;
						if( (DateList = FOPTYearMonthTable.GetObjectByKey( Key2 ) )== NULL )
						{
							DateList = new TStringList();
							FOPTYearMonthTable.Add( Key2, DateList );
						}

						if( DateList->IndexOf( Info->GetMaturityDate().c_str() ) < 0 )
						{
							DateList->Add( Info->GetMaturityDate().c_str() );
							DateList->Sort();
						}
						///< TAIFEX.TXO.200911 ==> TXO06600K9, TXO06600W9, TXO06700K9, TXO06700W9, etc.
						UFC::List<BasicInformation*>* pList = FOPTStrikePriceTable.GetObjectByKey( Key3 );
						if( pList == NULL )
						{
							pList = new UFC::List<BasicInformation*>();
							FOPTStrikePriceTable.Add( Key3, pList );
						}
						pList->Add( Info );

						UFC::List<BasicInformation*>* List = FPrefixMap.GetObjectByKey( Key2 );
						if( List == NULL )
						{
							List = new UFC::List<BasicInformation*>();
							FPrefixMap.Add( Key2, List );
						}

						List->Add( Info );
					}
				}
				else
				{
					for( register int index = 0; index < Info->GetSpotList().ItemCount(); index++ )
					{
						UFC::AnsiString key;
						key.Printf( "%s.%s", Info->GetExchange().c_str(), Info->GetSpotList().GetItem(index).c_str() );
						UFC::List<BasicInformation*>* List = FPrefixMap.GetObjectByKey( key );

						if( List == NULL )
						{
							List = new UFC::List<BasicInformation*>();
							FPrefixMap.Add( key, List );
						}
						List->Add( Info );
					}
				}
				Info = ExSymbols->Next();
			}
		}
	}
}
//---------------------------------------------------------------------------
BasicInformation* __fastcall TCMarketDataStore::GetFUTInfo( const String& Exchange, const String& SymbolPrefix, const String& MaturityDate )
{
	String Key;

	Key.printf( L"%s.%s.%s", Exchange, SymbolPrefix, MaturityDate );
	return FQueryTable.GetObjectByKey( UFC::AnsiString( Key.c_str()) );
}
//---------------------------------------------------------------------------
BasicInformation* __fastcall TCMarketDataStore::GetFUTInfo( const String& Exchange, const String& SymbolPrefix, const String& MaturityDate1, const String& MaturityDate2 )
{
	String Key;

	Key.printf( L"%s.%s.%s.%s", Exchange, SymbolPrefix, MaturityDate1, MaturityDate2 );
	return FQueryTable.GetObjectByKey( UFC::AnsiString( Key.c_str() ) );
}
//---------------------------------------------------------------------------
BasicInformation* __fastcall TCMarketDataStore::GetOPTInfo( const String& Exchange, const String& SymbolPrefix, const String& MaturityDate, double StrikePrice, CallPutCode CP )
{
	String Key;

	Key.printf( L"%s.%s.%s.%d.%c", Exchange, SymbolPrefix, MaturityDate, UFC::DoubleToInt( StrikePrice, 4 ), CP == cpCall?'C':'P' );
	return FQueryTable.GetObjectByKey( UFC::AnsiString( Key.c_str() ) );
}
//---------------------------------------------------------------------------
const UFC::List<BasicInformation*>* __fastcall TCMarketDataStore::GetMDSeries( const String& Exchange, const String& SymbolPrefix )
{
	String Key;

	Key.printf( L"%s.%s", Exchange, SymbolPrefix );
	return FPrefixMap.GetObjectByKey( UFC::AnsiString(Key.c_str() )  );
}
//---------------------------------------------------------------------------
const TStringList* __fastcall TCMarketDataStore::GetFUTMDPrefix( const String& Exchange )
{
	UFC::AnsiString Ex( Exchange.c_str() );
	return FFUTPrefixTable.GetObjectByKey( Ex );
}
//---------------------------------------------------------------------------
const TStringList* __fastcall TCMarketDataStore::GetOPTMDPrefix( const String& Exchange )
{
	UFC::AnsiString Ex( Exchange.c_str() );
	return FOPTPrefixTable.GetObjectByKey( Ex );
}
//---------------------------------------------------------------------------
const TStringList* __fastcall TCMarketDataStore::GetChineseFUTMDPrefix( const String& Exchange )
{
	UFC::AnsiString Ex( Exchange.c_str() );
	return FChineseFUTPrefixTable.GetObjectByKey( Ex );
}
//---------------------------------------------------------------------------
const TStringList* __fastcall TCMarketDataStore::GetChineseOPTMDPrefix( const String& Exchange )
{
	UFC::AnsiString Ex( Exchange.c_str() );
	return FChineseOPTPrefixTable.GetObjectByKey( Ex );
}
//---------------------------------------------------------------------------
const TStringList* TCMarketDataStore::GetOPTMonthYear( const String& Exchange, const String& SymbolPrefix )
{
	String Key;

	Key.printf( L"%s.%s", Exchange, SymbolPrefix );
	return FOPTYearMonthTable.GetObjectByKey( UFC::AnsiString(Key.c_str() ) );
}
//---------------------------------------------------------------------------
const UFC::List<BasicInformation*>* __fastcall TCMarketDataStore::GetOPTMDSeriesWithYearMonth( const String& Exchange, const String& SymbolPrefix, const String& MonthYear )
{
	String Key;

	Key.printf( L"%s.%s.%s", Exchange, SymbolPrefix, MonthYear );
	return FOPTStrikePriceTable.GetObjectByKey( UFC::AnsiString(Key.c_str() ) );
}
//---------------------------------------------------------------------------
double __fastcall TCMarketDataStore::GetTick( const String& Exchange, const String& Symbol )
{
	double Tick = 0;
	if( Exchange == "TAIFEX" || Exchange == "TFX" )
	{
		UnicodeString Prefix( Symbol.SubString( 1, 2 ) );
		if( Symbol.Length() == 5 || Symbol.Length() == 8 ) /// Futures
		{
			if( Prefix == "TX" || Prefix == "MX" || Prefix == "T5" || Prefix == "XI"|| Prefix == "TM" )
				Tick = 1;
			else if( Prefix == "EX" || Prefix == "GT" )
				Tick = 0.05;
			else if( Prefix == "FX" )
				Tick = 0.2;
			else if( Prefix == "GB" || Prefix == "CP" )
				Tick = 0.005;
			else if( Prefix == "MS" || Prefix == "GD" )
				Tick = 0.1;
			else if( Prefix == "TG" )
				Tick = 0.5;
		}
	}
	else
	{
		BasicInformation* Info = GetBasicInformation( Exchange, Symbol, false );
		if( Info != NULL )
			Tick = Info->GetTick();
	}

	return Tick;
}
//---------------------------------------------------------------------------
int __fastcall TCMarketDataStore::GetDecimalLocator( const String& Exchange, const String& Symbol )
{

	BasicInformation* Info = GetBasicInformation( Exchange, Symbol, false );
	if( Info != NULL )
		return Info->GetDecimalLocator();
	return -1;
}
//---------------------------------------------------------------------------
const TTickList* __fastcall TCMarketDataStore::GetTickList( const String& Exchange, const String& Symbol )
{
	if( Exchange.Length( ) == 0 || Symbol.Length( ) == 0  )
		return NULL;
	AnsiString exchange = Exchange;
	AnsiString symbol = Symbol;

	TTickList* List = FTickTable.GetObjectByKey( symbol.c_str() );
	if( List == NULL ) ////< not in the table, add one.
	{
		BasicInformation* Info = GetBasicInformation( exchange.c_str(), symbol.c_str(), false );
		if( Info != NULL )
		{
			List = new TTickList( Info );
			FTickTable.Add( symbol.c_str(), List );
		}
	}
	return List;
}
//---------------------------------------------------------------------------
const TTickList* __fastcall TCMarketDataStore::GetTickList( const String& Exchange, const String& Symbol, BasicInformation* Info )
{
	UFC::AnsiString symbol( Symbol.c_str() );

	TTickList* List = FTickTable.GetObjectByKey( symbol );
	if( List == NULL ) ////< not in the table, add one.
	{
		List = new TTickList( Info );
		FTickTable.Add( symbol, List );
	}
	return List;
}
//---------------------------------------------------------------------------
const TTickList* __fastcall TCMarketDataStore::GetTickListEx( const String& Exchange, const String& Symbol, double MaxPx, double MinPx )
{
	UFC::AnsiString exchange( Exchange.c_str() );
	UFC::AnsiString symbol( Symbol.c_str() );

	TTickList* List = FTickTableEx.GetObjectByKey( symbol );

	if( List == NULL ) ////< not in the table, add one.
	{
		BasicInformation* Info = GetBasicInformation( exchange, symbol, false );
		if( Info != NULL )
		{
			double BullPx = Info->GetBullPrice();
			double BearPx = Info->GetBearPrice();
			if( MaxPx < BullPx )
				MaxPx = BullPx;
			if( MinPx > BearPx )
				MinPx = BearPx;
			List = new TTickList( Info, MaxPx, MinPx );
			FTickTableEx.Add( symbol, List );
		}
	}
	return List;
}
//---------------------------------------------------------------------------
//** Steps to add new Exchange **
//**2. Add Exchange constant string to Chinese name hash map
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::LoadChineseName( void )
{
	try
	{
		AddTAIFEXChineseName( );

/*		UFC::AnsiString ConfigFile( "ContractMapping.ini" );

		if( FLanguage != lTraditionalChinese )
			ConfigFile.Printf( "ContractMapping%d.ini", FLanguage );

		UFC::UiniFile Config( ConfigFile );

		///< Add String HashedList into table
		AddChineseName( Config, SHFE );
		AddChineseName( Config, CFFEX );
		AddChineseName( Config, DCE );
		AddChineseName( Config, CZCE );
		AddChineseName( Config, CME );
		AddChineseName( Config, CBOT );
		AddChineseName( Config, SGX );
		AddChineseName( Config, NYMEX );
		AddChineseName( Config, NYBOT );
		AddChineseName( Config, HKEx );
		AddChineseName( Config, TOCOM );
		AddChineseName( Config, OSE );
		AddChineseName( Config, TGE );
		AddChineseName( Config, LIF );
		AddChineseName( Config, LME );
		AddChineseName( Config, EUX );
		AddChineseName( Config, KRX );
		AddChineseName( Config, TFX );
		AddChineseName( Config, SSE );
		AddChineseName( Config, BMD );*/
	}
	catch( UFC::Exception ex )
	{
		UFC::BufferedLog::Printf( " Load Chinese name failed." );
	}
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::AddTAIFEXChineseName( void )
{
	try
	{
		UFC::UiniFile     Config( "ProductInfo.cfg" );
		StringHashedList* pList = new StringHashedList();

		for( register int i = 0; i < Config.SectionCount(); i++ )
		{
			UFC::Section* pSection = Config.GetSection( i );

			if( pSection != NULL )
			{
				UFC::AnsiString Name = pSection->GetSectionName();
				UFC::AnsiString Value,curr,type, cm, stock, market;

				if( pSection->GetValue( "name" , Value ) == TRUE &&
					pSection->GetValue( "currency" , curr  ) == TRUE &&
					pSection->GetValue( "type" , type ) == TRUE &&
					pSection->GetValue( "cm" , cm ) == TRUE &&
					pSection->GetValue( "market" , market ) == TRUE )
				{
					if( type.Length() >= 1 && curr.Length() >= 1 )
					{
						if( Name == "TXO" )
							pList->Add( Name, new String(  L"쪃�鰥�" ));
						else if( Name == "TX1" )
							pList->Add( Name, new String(  L"쪃�鰥�W1" ));
						else if( Name == "TX2" )
							pList->Add( Name, new String(  L"쪃�鰥�W2" ));
						else if( Name == "TX3" )
							pList->Add( Name, new String(  L"쪃�鰥�W3" ));
						else if( Name == "TX4" )
							pList->Add( Name, new String(  L"쪃�鰥�W4" ));
						else if( Name == "TX5" )
							pList->Add( Name, new String(  L"쪃�鰥�W5" ));
						else
						{
							if( IsUTF8String( Value.c_str() ) == true )
							{
								String  OutStr = UTF8ToString( Value.c_str() );
								pList->Add( Name, new String( OutStr ) );
							}
							else
								pList->Add( Name, new String( Value.c_str() ) );
						}
						ProductInfo* pinfo = new ProductInfo();
						if( market == "Options" )
							pinfo->market = mTWOptions;
						else
							pinfo->market = mTWFutures;
						pinfo->ContractMultiplier = cm.ToDouble();
						pinfo->ProductType = (TFutProdType)type[0];
						if( pinfo->ProductType == fptStock )
						{
							pSection->GetValue( "stock" , stock );
							stock.TrimRight();
							pinfo->StockID = stock.c_str();
						}
						pinfo->Currency = (TCurrencyEnum)curr[0];
						FProductInfoTable.Add( Name, pinfo );
					}
				}
			}

		}
		FChineseTable.Add( "TAIFEX", pList );
	}
	catch( UFC::Exception ex )
	{
		UFC::BufferedLog::Printf( " Load TAIFEX Chinese name failed." );
	}
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::AddChineseName( UFC::UiniFile& Config, const UFC::AnsiString& Exchange )
{
	UFC::Section* pSection = Config.GetSection( Exchange );

	if( pSection != NULL )
	{
		StringHashedList* pList = new StringHashedList();

		for( register int i = 0; i < pSection->ItemCount(); i++ )
		{
			UFC::AnsiString Name, Value;

			pSection->GetNameValue( i, Name, Value );
			if( IsUTF8String( Value.c_str() ) == true )
			{
				String  OutStr = UTF8ToString( Value.c_str() );
				pList->Add( Name, new String( OutStr ) );
			}
			else
				pList->Add( Name, new String( Value.c_str() ) );
		}
		FChineseTable.Add( Exchange, pList );
	}
}
//---------------------------------------------------------------------------
bool TCMarketDataStore::TAIFEXStockProduct( UFC::AnsiString& ProductID, UFC::AnsiString& StockID )
{
	ProductInfo* pinfo;

	if( (pinfo = FProductInfoTable.GetObjectByKey( ProductID ))  != NULL)
	{
		StockID = pinfo->StockID;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
TFutProdType TCMarketDataStore::TAIFEXProductType( UFC::AnsiString& ProductID )
{
	ProductInfo* pinfo;

	if( (pinfo = FProductInfoTable.GetObjectByKey( ProductID ))  != NULL)
		return pinfo->ProductType;
	return fptUnknown;
}
//---------------------------------------------------------------------------
double TCMarketDataStore::ProductContractMultiplier( const String& Exchange, const String& ProductID )
{
	if( Exchange == L"TWSE" || Exchange == L"OTC" )
        return 1000;
	AnsiString   PID( ProductID );
	ProductInfo* pinfo;

	if( (pinfo = FProductInfoTable.GetObjectByKey( PID.c_str() ))  != NULL)
		return pinfo->ContractMultiplier;
	return 1.0;
}
//---------------------------------------------------------------------------
TFutProdType TCMarketDataStore::TAIFEXProductType( const String& ProductID )
{
	AnsiString Key( ProductID );
	return TAIFEXProductType( Key );
}
//---------------------------------------------------------------------------
//** Steps to add new Exchange **
//** 3. Add new exchange name case into SetDisplayName function.
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::SetDisplayName( void )
{
	UFC::AnsiString SubscribeKey;
	String          ChineseName, Chinese,MDate;

	LoadChineseName();
	for( int i = 0; i < FConnection->ExchangeCount(); i++ )
	{
		UFC::AnsiString Exchange( GetExchange(i) );
		UFC::PHashedList<UFC::AnsiString, BasicInformation*>* ExSymbols;

		if( (ExSymbols = FConnection->GetSymbolsByExchange( Exchange )) != NULL )
		{
			BasicInformation* Info = ExSymbols->First();
			if( Exchange == "TAIFEX" || Exchange == "TFX" )
			{
				while( Info != NULL )
				{
					TStringList* symbolList = FBlackList.GetObjectByKey(Exchange);
					// check product is blacklist or not
					if( symbolList != NULL && symbolList->IndexOf( Info->GetProductID().c_str() ) != -1 )
					{
						Info = ExSymbols->Next();
						continue;
					}

					if( Info->GetSymbol().Length() >= 5 && IsVisible( Exchange,  Info->GetProductID() ) )  ///< Skip Index
					{
						GetChinesePrefix( Exchange, Info->GetProductID(), Chinese );
						MDate   = Info->GetMaturityDate().c_str();
						if( Info->GetMarket() == mTWFutures || Info->GetMarket() == mPATSFutures )
						{
							if( Info->IsMultileg() == false )
								ChineseName.printf( L"%s %s", Chinese.c_str(), MDate.c_str() );
							else
								ChineseName.printf( L"%s%s/%s", Chinese.c_str(), MDate.c_str(), Info->GetMaturityDate2().c_str() );
						}
						else ///< Options
						{
							char CP = 'P'
							;
							if( Info->GetCallPut() == cpCall )
								CP = 'C';
							if( Info->GetProductID() == "TXO" )
								ChineseName.printf( L"%.*f-%c %s 쪃�鰥�", Info->GetDigit(), Info->GetStrikePrice(), CP, MDate.c_str() );
							else if( Info->GetProductID() == "TX1" )
								ChineseName.printf( L"%.*f-%c W1 %s 쪃�鰥�", Info->GetDigit(), Info->GetStrikePrice(), CP, MDate.c_str() );
							else if( Info->GetProductID() == "TX2" )
								ChineseName.printf( L"%.*f-%c W2 %s 쪃�鰥�", Info->GetDigit(), Info->GetStrikePrice(), CP, MDate.c_str() );
							else if( Info->GetProductID() == "TX3" )
								ChineseName.printf( L"%.*f-%c W3 %s 쪃�鰥�", Info->GetDigit(), Info->GetStrikePrice(), CP, MDate.c_str() );
							else if( Info->GetProductID() == "TX4" )
								ChineseName.printf( L"%.*f-%c W4 %s 쪃�鰥�", Info->GetDigit(), Info->GetStrikePrice(), CP, MDate.c_str() );
							else if( Info->GetProductID() == "TX5" )
								ChineseName.printf( L"%.*f-%c W5 %s 쪃�鰥�", Info->GetDigit(), Info->GetStrikePrice(), CP, MDate.c_str() );
							else
								ChineseName.printf( L"%.*f-%c %s %s", Info->GetDigit(), Info->GetStrikePrice(), CP, MDate.c_str(),Chinese.c_str() );
							//if( Info->GetCallPut() == cpCall )
							//	ChineseName.printf( L"%s %s %.*f%s", Chinese.c_str(), MDate.c_str(), Info->GetDigit(), Info->GetStrikePrice(),L"-C" );
							//else
							//	ChineseName.printf( L"%s %s %.*f%s", Chinese.c_str(), MDate.c_str(), Info->GetDigit(), Info->GetStrikePrice(),L"-P" );
							//if( Info->GetCallPut() == cpCall )
							//	ChineseName.printf( L"%s %s%s%.*f", Chinese.c_str(), MDate.c_str(),L" call ", Info->GetDigit(), Info->GetStrikePrice() );
							//else
							//	ChineseName.printf( L"%s %s%s%.*f", Chinese.c_str(), MDate.c_str(),L" put ", Info->GetDigit(), Info->GetStrikePrice() );
						}
						Info->SetDisplayName( ChineseName.c_str() );
						AddToStoreTable( Exchange, Info->GetSymbol(), Info );
						SubscribeKey.Printf( "%s.%s", Exchange.c_str(), Info->GetSymbol().c_str() );
						FSymbolIndex.Add( SubscribeKey );
					}
					Info = ExSymbols->Next();
				}
			}
			else if( Exchange == "SHFE" || Exchange == "CZCE" || Exchange == "CFFEX" || Exchange == "DCE" ||  Exchange == "SSE" )
			{
				while( Info != NULL )
				{
					if( Info->GetTradeFlag() == true )
					{
						GetChinesePrefix( Exchange, Info->GetProductID(), Chinese );
						MDate   = Info->GetMaturityDate().c_str();
						if( Info->GetMarket() == mCNOptions )///< Options
						{
							if( Info->GetCallPut() == cpCall )
								ChineseName.printf( L"%s %s%s%.*f", Chinese.c_str(), MDate.c_str(),L" call ", Info->GetDigit(), Info->GetStrikePrice() );
							else
								ChineseName.printf( L"%s %s%s%.*f", Chinese.c_str(), MDate.c_str(),L" put ", Info->GetDigit(), Info->GetStrikePrice() );
						}
						else
							ChineseName.printf( L"%s %s", Chinese.c_str(), MDate.c_str() );


						Info->SetDisplayName( ChineseName.c_str() );
						AddToStoreTable( Exchange, Info->GetSymbol(), Info );
						SubscribeKey.Printf( "%s.%s", Exchange.c_str(), Info->GetSymbol().c_str() );
						FSymbolIndex.Add( SubscribeKey );
					}
					Info = ExSymbols->Next();
				}
			}
			else if(   Exchange == "CME"   || Exchange == "CBOT"  || Exchange == "SGX"
					|| Exchange == "NYMEX" || Exchange == "NYBOT" || Exchange == "HKEx"
					|| Exchange == "TOCOM" || Exchange == "OSE"   || Exchange == "TGE"
					|| Exchange == "LIF"   || Exchange == "LME"   || Exchange == "EUX"
					|| Exchange == "BMD"  )
			{
				while( Info != NULL )
				{
					if( Info->GetTradeFlag() == true )
					{
						GetChinesePrefix( Exchange, Info->GetProductID(), Chinese );
						MDate   = Info->GetMaturityDate().c_str();
						ChineseName.printf( L"%s %s", Chinese.c_str(), MDate.c_str() );
					}
					else if( Info->GetSymbol().AnsiPos( "=" ) >= 0 ) ///<  FX Spot
					{
						GetChinesePrefix( Exchange, Info->GetSymbol().SubString( 0, 3 ), Chinese );
						ChineseName.printf( L"%s", Chinese.c_str() );
					}
					else
					{
						GetChinesePrefix( Exchange, Info->GetProductID(), Chinese );
						ChineseName.printf( L"%s", Chinese.c_str() );
					}
					Info->SetDisplayName( ChineseName.c_str() );
					AddToStoreTable( Exchange, Info->GetSymbol(), Info );
					SubscribeKey.Printf( "%s.%s", Exchange.c_str(), Info->GetSymbol().c_str() );
					FSymbolIndex.Add( SubscribeKey );
					Info = ExSymbols->Next();
				}
			}
			else if( Exchange == KRX )
			{
            	while( Info != NULL )
				{
					if( Info->GetTradeFlag() == true )
					{
						GetChinesePrefix( Exchange, Info->GetProductID(), Chinese );
						MDate   = Info->GetMaturityDate().c_str();

						if( Info->GetMarket() == mPATSFutures )
						{
							ChineseName.printf( L"%s %s", Chinese.c_str(), MDate.c_str() );
						}
						else if( Info->GetMarket() == mPATSOptions )// KS 200 Options
						{
							double StrikePx = Info->GetStrikePrice();
							int Digit = this->GetStrikePriceDigit( Exchange, Info->GetSymbol() );

							if( Info->GetCallPut() == cpCall )
								ChineseName.printf( L"%s C %s %.*f", Chinese.c_str(), MDate.c_str(), Digit, StrikePx );
							else
								ChineseName.printf( L"%s P %s %.*f", Chinese.c_str(), MDate.c_str(), Digit, StrikePx );
						}
						else
						{
      						GetChinesePrefix( Exchange, Info->GetProductID(), Chinese );
							ChineseName.printf( L"%s", Chinese.c_str() );
						}
					}
					else // KOSPI 200 Index
					{
						GetChinesePrefix( Exchange, Info->GetProductID(), Chinese );
						ChineseName.printf( L"%s", Chinese.c_str() );
					}
					Info->SetDisplayName( ChineseName.c_str() );
					AddToStoreTable( Exchange, Info->GetSymbol(), Info );
					SubscribeKey.Printf( "%s.%s", Exchange.c_str(), Info->GetSymbol().c_str() );
					FSymbolIndex.Add( SubscribeKey );
					Info = ExSymbols->Next();
				}
			}
			else if( Exchange == "TWSE" || Exchange == "OTC"  )
			{
				UFC::AnsiString ConfigFile( "EquitySectors.ini" );
				UFC::AnsiString ChineseSector;

				if( FLanguage != lTraditionalChinese )
					ConfigFile.Printf( "EquitySectors%d.ini", FLanguage );
				try
				{
					UFC::UiniFile Config( ConfigFile );
					while( Info != NULL )
					{
						if( IsUTF8String( Info->GetChineseName().c_str() ) == true )
							ChineseName = UTF8ToString( Info->GetChineseName().c_str() );
						else
							ChineseName = String( Info->GetChineseName().c_str());
						Info->SetDisplayName( ChineseName.c_str() );
						if( Config.GetValue( Exchange, Info->GetProductID(), ChineseSector ) == true )
							Info->SetProductID( ChineseSector );
						AddToStoreTable( Exchange, Info->GetSymbol(), Info );
						SubscribeKey.Printf( "%s.%s", Exchange.c_str(), Info->GetSymbol().c_str() );
						FSymbolIndex.Add( SubscribeKey );
						Info = ExSymbols->Next();
					}
				}
				catch( UFC::Exception ex )
				{
					UFC::BufferedLog::Printf( " Load %s failed.", ConfigFile.c_str() );
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::BuildAliasTable( void )
{
	UFC::PHashedList<UFC::AnsiString, UFC::PList*> Table;
	UFC::AnsiString Alias;
	TStringList*    List;
	int             ExCount = ExchangeCount();

	for( int i = 0; i < ExCount; i++ )
	{
		UFC::AnsiString Exchange( GetExchange(i) );
		UFC::PHashedList<UFC::AnsiString, BasicInformation*>* ExSymbols;

		if( (ExSymbols = FConnection->GetSymbolsByExchange( Exchange )) != NULL )
		{
			BasicInformation* Info = ExSymbols->First();
			while( Info != NULL )
			{
				if( ( Info->GetMarket() == mTWFutures || Info->GetMarket() == mCNFutures || Info->GetMarket() == mPATSFutures )
					&& Info->IsMultileg() == false
					&& Info->GetTradeFlag() )
				{
					UFC::AnsiString ProdID = Info->GetProductID();
					UFC::PList* PrefixList = Table.GetObjectByKey( ProdID );

					if( PrefixList == NULL ) ///< PrefixList not exists.
					{
						PrefixList = new UFC::PList( FCompareRule );
						Table.Add( ProdID, PrefixList );
						PrefixList->Add( Info );
					}
					else
						PrefixList->Add( Info );
				}
				Info = ExSymbols->Next();
			}
			if( ( List = (TStringList*)GetFUTMDPrefix( Exchange.c_str())) != NULL )
			{
				for( register int j = 0; j < List->Count; j++ )
				{
					AnsiString PrefixStr = List->Strings[j];

					if( PrefixStr.IsDelimiter( ".=", 1 ) == false )//  .Pos( "." ) == 0 && PrefixStr.Pos( "=" ) == 0 )
					{
						UFC::PList* PrefixList = Table.GetObjectByKey( PrefixStr.c_str() );
						if( PrefixList != NULL )
						{
							for( register int k = 0; k < PrefixList->ItemCount(); k++ )
							{
								BasicInformation* Info = (BasicInformation*)PrefixList->GetItem( k );
								if( k == 0 )
									Alias.Printf( "%sFront1", Info->GetProductID().c_str() );
								else if( k == 1 )
									Alias.Printf( "%sFront2", Info->GetProductID().c_str() );
								else
									Alias.Printf( "%sFar%d", Info->GetProductID().c_str(), k - 1 );
								Info->SetAliasName( Alias );
								AddToAliasTable( Exchange, Alias, Info );
							}
						}
					}
				}
			}
			for( register int i = 0; i < Table.ItemCount(); i++ )
				 Table.GetItem( i )->Clear();
			Table.Clear();
		}
	}
}
//---------------------------------------------------------------------------
UnicodeString TCMarketDataStore::GetChinesePrefix( const String& Exchange, const String& ProductID )
{
	UFC::AnsiString Ex( Exchange.c_str() );
	UFC::AnsiString Sym( ProductID.c_str() );
	return GetChinesePrefix( Ex, Sym );
}
//---------------------------------------------------------------------------
UnicodeString TCMarketDataStore::GetChinesePrefix( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID )
{
	StringHashedList* pList = FChineseTable.GetObjectByKey( Exchange );

	if( pList != NULL )
	{
		UnicodeString* chinesePrefix = pList->GetObjectByKey( ProductID );
		if( chinesePrefix != NULL )
			return *chinesePrefix;
		else
			return UnicodeString( ProductID.c_str() );
	}
	else
		return UnicodeString( ProductID.c_str() );
}
//---------------------------------------------------------------------------
void  TCMarketDataStore::GetChinesePrefix( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID, String& ChinesePrefix )
{
	StringHashedList* pList = FChineseTable.GetObjectByKey( Exchange );

	if( pList != NULL )
	{
		String* chinesePrefix = pList->GetObjectByKey( ProductID );
		if( chinesePrefix != NULL )
		{
			ChinesePrefix = *chinesePrefix;
			return;
		}
	}
	ChinesePrefix = ProductID.c_str();
}
//---------------------------------------------------------------------------
bool __fastcall TCMarketDataStore::IsVisible( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID )
{
	StringHashedList* pList = FChineseTable.GetObjectByKey( Exchange );

	if( pList != NULL )
		return pList->IsExists( ProductID );
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::SendMessageToServer( const String& Msg, int Func )
{
	UFC::AnsiString AnsiMsg( Msg.c_str() );
	FConnection->SendMessageToServer( AnsiMsg ,Func );
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::SendMessageToServer( const char* Msg, int Func )
{
	FConnection->SendMessageToServer( Msg ,Func );
}
//---------------------------------------------------------------------------
double TCMarketDataStore::GetMaxStrikePx( const String& Exchange, const String& SymbolPrefix, const String& MonthYear )
{
	double MaxStrikePx = DBL_MIN;
	BasicInformation* Info;

	UFC::List<BasicInformation*>* InfoList = (UFC::List<BasicInformation*>*)GetOPTMDSeriesWithYearMonth( Exchange, SymbolPrefix, MonthYear );
	if( InfoList )
	{
		for( register int i = 0; i < InfoList->ItemCount(); i++ )
		{
			Info = InfoList->GetItem(i);
			if( Info != NULL )
			{
				if( Info->GetStrikePrice() > MaxStrikePx )
					MaxStrikePx = Info->GetStrikePrice();
			}
		}
		return MaxStrikePx;
	}
	return 0;
}
//---------------------------------------------------------------------------
double TCMarketDataStore::GetMinStrikePx( const String& Exchange, const String& SymbolPrefix, const String& MonthYear )
{
	double MinStrikePx = DBL_MAX;
	BasicInformation* Info;

	UFC::List<BasicInformation*>* InfoList = (UFC::List<BasicInformation*>*)GetOPTMDSeriesWithYearMonth( Exchange, SymbolPrefix, MonthYear );
	if( InfoList )
	{
		for( register int i = 0; i < InfoList->ItemCount(); i++ )
		{
			Info = InfoList->GetItem(i);
			if( Info != NULL )
			{
				if( Info->GetStrikePrice() < MinStrikePx )
					MinStrikePx = Info->GetStrikePrice();
			}
		}
		return MinStrikePx;
	}
	return 0;
}
//---------------------------------------------------------------------------
BasicInformation* _fastcall TCMarketDataStore::FindBasicInformation( const String& Exchange, const String& Symbol )
{
	BasicInformation* Info = NULL;

	Info = GetBasicInformation( Exchange, Symbol, false );

	if( Info == NULL )
	{
		if( ( Exchange.Compare( "TAIFEX" ) == 0 || Exchange.Compare( "TFX" ) == 0 )
			&& IsTAIFEXFutures( Exchange, Symbol ) == false ) ///< TAIFEX Options
		{
			TStringList* ProductIDList = (TStringList*)GetOPTMDPrefix( Exchange );

			for( register int i = 0; i < ProductIDList->Count; i++ )
			{
				if( Symbol.Pos( ProductIDList->Strings[i] ) > 0 )
				{
					TStringList* MaturityDateList = (TStringList*)GetOPTMonthYear( Exchange, ProductIDList->Strings[i] );
					if( MaturityDateList->Count > 0 )
					{
						String MaturityDate = MaturityDateList->Strings[0];
						UFC::List<BasicInformation*>* InfoList =
								(UFC::List<BasicInformation*>*)GetOPTMDSeriesWithYearMonth( Exchange, ProductIDList->Strings[i], MaturityDate );

						if( InfoList )
						{
							Info = InfoList->GetItem( 0 );
							break;
						}
					}
				}
			}
		}
		else  ///< Futures
		{
			TStringList* ProductIDList = (TStringList*)GetFUTMDPrefix( Exchange );
			if( ProductIDList )
			{
				for( register int i = 0; i < ProductIDList->Count; i++ )
				{
					if( Symbol.Pos( ProductIDList->Strings[i] ) > 0 )
					{
						UFC::List<BasicInformation*>* InfoList =
								(UFC::List<BasicInformation*>*)GetMDSeries( Exchange, ProductIDList->Strings[i] );

						if( InfoList )
						{
							for( register int j = 0; j < InfoList->ItemCount(); j ++ )
							{
								Info = InfoList->GetItem( j );

								if( Info->IsMultileg() )
									continue;
								else
									return Info;
							}
						}
					}
				}
			}

		}
	}
	return Info;
}
//---------------------------------------------------------------------------
//** Steps to add new Exchange **
//** 4. Translate Exchange constant string to Chinese dislay name.
//---------------------------------------------------------------------------
String TCMarketDataStore::GetExchangeDisplayName( const String& Exchange )
{
	if( Exchange.Compare( "TAIFEX" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHANGE_TAIFEX;
	else if( Exchange.Compare( "TWSE" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHANGE_TWSE;
	else if( Exchange.Compare( "OTC" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHANGE_OTC;
	else if( Exchange.Compare( "SHFE" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHANGE_SHFE;
	else if( Exchange.Compare( "CFFEX" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHANGE_CFFEX;
	else if( Exchange.Compare( "CZCE" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHANGE_CZCE;
	else if( Exchange.Compare( "DCE" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHANGE_DCE;
	else if( Exchange.Compare( "SGX" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHANGE_SGX;
	else if( Exchange.Compare( "CME" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHANGE_CME;
	else if( Exchange.Compare( "CBOT" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHANGE_CBOT;
	else if( Exchange.Compare( "NYMEX" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHANGE_NYMEX;
	else if( Exchange.Compare( "NYBOT" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHANGE_NYBOT;
	else if( Exchange.Compare( "TOCOM" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHANGE_TOCOM;
	else if( Exchange.Compare( "OSE" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHANGE_OSE;
	else if( Exchange.Compare( "TGE" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHANGE_TGE;
	else if( Exchange.Compare( "HKEx" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHANGE_HKEX;
	else if( Exchange.Compare( "LIF" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHANGE_LIF;
	else if( Exchange.Compare( "LME" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHANGE_LME;
	else if( Exchange.Compare( "EUX" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHANGE_EUX;
	else if( Exchange.Compare( "KRX" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHNAGE_KRX;
	else if( Exchange.Compare( "TFX" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHNAGE_TFX;
	else if( Exchange.Compare( "SSE" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHANGE_SSE;
	else if( Exchange.Compare( "BMD" ) == 0 )
		return Mdcomponentstrings_MD_STORE_EXCHANGE_BMD;
	else
		return Exchange;
}
//---------------------------------------------------------------------------
bool __fastcall TCMarketDataStore::IsTAIFEXFutures( const String& Exchange, const String& Symbol )
{
	if( Exchange.Compare( "TAIFEX" ) == 0 || Exchange.Compare( "TFX" ) == 0 )
	{
		if( Symbol[3] == 'F' || isdigit( Symbol[3] ) )
			return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::LoadExchangeTable( void )
{
	try
	{
		UFC::UiniFile ExchangeConfig( "ExchangeInfo.ini" );

		for( register int i = 0; i < ExchangeConfig.SectionCount(); i++ )
		{
			UFC::Section* pSection = ExchangeConfig.GetSection( i );
			if( pSection != NULL )
			{
				UFC::PHashedList<UFC::AnsiString, ExchangeInfo*>* ExchangeList = NULL;
				UFC::PStringList StringList;
				ExchangeInfo*    Info;
				UFC::PStringList ProductList;
				UFC::AnsiString  Value;

				StringList.SetStrings( pSection->GetSectionName(), "." ); ///< Example: SGX.STW
				pSection->GetValue( "ProductID", Value ); ///< Example: ProductID=STW,.TAMSCI
				ProductList.SetStrings( Value, "," );

				if( !FExchangeTable.IsExists( StringList[0] ) ) ///< Exchange exists?
					FExchangeTable.Add( StringList[0], ExchangeList = new UFC::PHashedList<UFC::AnsiString, ExchangeInfo*>() );
				else
					FExchangeTable.GetObjectByKey( StringList[0], ExchangeList );
				Info = new ExchangeInfo( pSection->GetSectionName(), ExchangeConfig );
				for( register int j = 0; j < ProductList.ItemCount(); j++ )
				{
					if( !ExchangeList->IsExists( ProductList[j] ) )
						ExchangeList->Add( ProductList[j], Info );
				}
			}
		}
	}
	catch( UFC::Exception ex )
	{
		UFC::BufferedLog::Printf( " Load Exchange table fail." );
	}
}
//---------------------------------------------------------------------------
ExchangeInfo* TCMarketDataStore::GetExchangeInfo( const String& ExchangeName )
{
	UFC::AnsiString AnsiExchange( ExchangeName.c_str() );

	return GetExchangeInfo( AnsiExchange );
}
//---------------------------------------------------------------------------
ExchangeInfo* TCMarketDataStore::GetExchangeInfo( const String& ExchangeName, const String& ProductID )
{
	UFC::AnsiString AnsiEx( ExchangeName.c_str() );
	UFC::AnsiString AnsiProductID( ProductID.c_str() );

	return GetExchangeInfo( AnsiEx, AnsiProductID );
}
//---------------------------------------------------------------------------
ExchangeInfo* TCMarketDataStore::GetExchangeInfo( const UFC::AnsiString& ExchangeName )
{
	UFC::PHashedList<UFC::AnsiString, ExchangeInfo*>* List;

	if( FExchangeTable.GetObjectByKey( ExchangeName, List ) == TRUE && List->ItemCount() > 0 )
		return List->GetItem( 0 );
	return NULL;
}
//---------------------------------------------------------------------------
ExchangeInfo* TCMarketDataStore::GetExchangeInfo( const UFC::AnsiString& ExchangeName, const UFC::AnsiString& ProductID )
{
	ExchangeInfo* ExInfo = NULL;
	UFC::PHashedList<UFC::AnsiString, ExchangeInfo*>* List;

	if( FExchangeTable.GetObjectByKey( ExchangeName, List ) == TRUE && List->ItemCount() > 0 )
	{
		ExInfo = List->GetObjectByKey( ProductID );
		if( ExInfo == NULL )
			ExInfo = List->GetItem( 0 );
	}
	return ExInfo;
}
//---------------------------------------------------------------------------
unsigned long long __fastcall TCMarketDataStore::GetAcuRecvBytes( void )
{
	if( FConnection != NULL )
		return FConnection->GetAcuRecvBytes();
	return 0;
}
//---------------------------------------------------------------------------
unsigned long long __fastcall TCMarketDataStore::GetAcuRecvMsgs( void )
{
	if( FConnection != NULL )
		return FConnection->GetAcuRecvMsgs();
	return 0;
}
//---------------------------------------------------------------------------
bool __fastcall TCMarketDataStore::IsOpen( const UFC::AnsiString& ExchangeName, const UFC::AnsiString& ProductID )
{
	ExchangeInfo* Info = GetExchangeInfo( ExchangeName, ProductID );

	if( Info )
	{
    	TIME_ZONE_INFORMATION pTimeZoneInformation;
		UFC::UDateTime ExchangeLocalTime;
		int Now, Start, End;

		ExchangeLocalTime.setCurrent();						// machine local time
		GetTimeZoneInformation( &pTimeZoneInformation );	// http://msdn.microsoft.com/en-us/library/ms724421%28v=vs.85%29.aspx
		ExchangeLocalTime += pTimeZoneInformation.Bias * 60;// UTC time
		ExchangeLocalTime += Info->TimeDiff() * 3600;

		Now = ExchangeLocalTime.getHour() * 10000
			+ ExchangeLocalTime.getMinute() * 100
			+ ExchangeLocalTime.getSecond();

		Start = Info->GetSessionStartDateTime( 0 ).getHour() * 10000
				+ Info->GetSessionStartDateTime( 0 ).getMinute() * 100
				+ Info->GetSessionStartDateTime( 0 ).getSecond();

		End = Info->GetSessionEndDateTime( Info->GetSessionCount() - 1 ).getHour() * 10000
				+ Info->GetSessionEndDateTime( Info->GetSessionCount() - 1 ).getMinute() * 100
				+ Info->GetSessionEndDateTime( Info->GetSessionCount() - 1 ).getSecond();

		if( End < Start ) // two days
		{
			if( ( Now >= Start && Now < 240000 ) || Now < End )
				return true;
		}
		else // one day
		{
			if( Now >= Start && Now < End )
				return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TCMarketDataStore::ExchangeSupportFutures( const UFC::AnsiString& Exchange )
{
	return FFuturesExSet.Exists( Exchange );
}
//---------------------------------------------------------------------------
bool __fastcall TCMarketDataStore::ExchangeSupportOptions( const UFC::AnsiString& Exchange )
{
	if( Exchange == "TFX" ) return false;

	return FOptionsExSet.Exists( Exchange );
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::SetBlackList( const UFC::AnsiString& Exchange, const String& Symbol )
{
	TStringList* BlackListList;
	BlackListList = FBlackList.GetObjectByKey( Exchange );
	if( BlackListList == NULL )
	{
		BlackListList = new TStringList();
		FBlackList.Add( Exchange, BlackListList );
	}
	BlackListList->Add( Symbol );
}
//---------------------------------------------------------------------------
UFC::PHashedList<UFC::AnsiString, BasicInformation*>* TCMarketDataStore::GetSymbolsByExchange( const UFC::AnsiString& Exchange )
{
	return FStoreTable.GetObjectByKey( Exchange );
}
//---------------------------------------------------------------------------
ExchangeClock* __fastcall TCMarketDataStore::GetClock( const String& Ex )
{
	AnsiString     AnsiEx( Ex );
	ExchangeClock* Clock;

	if( FExchangeClockMap.GetObjectByKey( AnsiEx.c_str(), Clock ) == false )
	{
		Clock = new ExchangeClock( Ex );
		FExchangeClockMap.Add( AnsiEx.c_str(), Clock );
	}
	return Clock;
}
//---------------------------------------------------------------------------
void __fastcall TCMarketDataStore::SetClock( const UFC::AnsiString& Ex, int h,int m,int s )
{
	ExchangeClock* Clock;

	if( FExchangeClockMap.GetObjectByKey( Ex, Clock ) == false )
	{
		Clock = new ExchangeClock( Ex.c_str() );
		FExchangeClockMap.Add( Ex, Clock );
	}
	Clock->SetTime( h,m,s );
}
//---------------------------------------------------------------------------

