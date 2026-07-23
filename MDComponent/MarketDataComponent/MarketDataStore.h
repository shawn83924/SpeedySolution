//---------------------------------------------------------------------------

#ifndef MarketDataStoreH
#define MarketDataStoreH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "MarketDataMessage.h"
#include "MarketDataConnection.h"
#include "TickList.h"
#include "iniFile.h"

//---------------------------------------------------------------------------
typedef enum
{
	ceTWD  = '1',
	ceUSD  = '2',
	ceEUR  = '3',
	ceJPY  = '4',
	ceGBP  = '5',
	ceAUD  = '6',
	ceHKD  = '7',
	ceCND  = '8',
	ceZAR  = 'A',
	ceKRW  = 'B',
	ceSGD  = 'C',
	ceCAD  = 'D',
	ceSEK  = 'E',
	ceCHF  = 'F',
	ceNZD  = 'G',
	ceTHB  = 'H',
	cePHP  = 'I',
	ceIDR  = 'J',
	ceMYR  = 'K',
	ceVND  = 'L',
	ceUnknown = 'X'

} TCurrencyEnum;
//---------------------------------------------------------------------------
typedef enum
{
	lTraditionalChinese = 0,
	lSimplifiedChinese = 1

} Language;
//---------------------------------------------------------------------------
typedef enum
{
	fptIndex        = 'I',
	fptInterestRate = 'R',
	fptBond         = 'B',
	fptCommodity    = 'C',
	fptStock        = 'S',
	fptFX           = 'E',
	fptUnknown      = 'X'

} TFutProdType;
//------------------------------------------------------------------------------
class TimerListener
{
private:
	HWND FHandle;
	UFC::AnsiString FExchange;
	UFC::AnsiString FSymbol;
	UFC::AnsiString FSubscribeKey;
	int FUpdateInterval;
	DWORD FLastUpdate;
	UFC::UInt32 FTimerID;
public:
	TimerListener( HWND Handle, const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, int TickPerSecond, UFC::UInt32 TimerID );
	~TimerListener();
	bool IsTimerTimer( void );
};
//------------------------------------------------------------------------------
class ProductInfo
{
public:
	double          ContractMultiplier;
	TFutProdType    ProductType;
	TCurrencyEnum   Currency;
	UFC::AnsiString StockID;
	Market          market;
};
//------------------------------------------------------------------------------
class ExchangeClock
{
public:
	String  Exchange;
	int     Hour;
	int     Minutes;
	int     Second;
public:
	ExchangeClock( const String& Ex ):Exchange( Ex ),Hour(0),Minutes(0),Second(0){}
	void __fastcall SetTime( int h,int m, int s ) { Hour = h; Minutes = m; Second = s; }
	int GetHHMM( void ) { return Hour*100 + Minutes; }
};
//---------------------------------------------------------------------------
class IMarketDataListener
{
public:
	virtual Classes::TStringList* __fastcall GetSymbols( void ) = 0;
	virtual void OnMarketDataUpdate( MarketDataMessage* Msg ) = 0;
	virtual void OnMarketDataUpdate( BasicData* Msg ) = 0;
	virtual void OnMarketDataUpdate( MatchInfo* Msg ) = 0;
	virtual void OnMarketDataUpdate( TotalMatch* Msg ) = 0;
	virtual void OnMarketDataUpdate( DayHighLowPrice* Msg ) = 0;
	virtual void OnMarketDataUpdate( OpeningInfo* Msg ) = 0;
	virtual void OnMarketDataUpdate( OrderBookData* Msg ) = 0;
	virtual void OnMarketDataUpdate( ClosingMarketData* Msg ) = 0;
	virtual void OnMarketDataUpdate( UnderlyingIndexInfo* Msg ) = 0;
	virtual void OnMarketDataUpdate( SumOfOrderInfo* Msg ) = 0;
};
//---------------------------------------------------------------------------
class SymbolCompare : public UFC::CompareInterface
{
public:
	int Compare( void* elem1, void* elem2 );
};
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TOnContractDownloadCompleteEvent)( int Count, int UseMS );
typedef void __fastcall (__closure *TOnContractReloadEvent)( const MString& Exchange );
typedef void __fastcall (__closure *TOnContractReloadCompleteEvent)( const MString& Exchange, MStringList& SymbolList );
typedef void __fastcall (__closure *TOnServerAckEvent)( int RTT );
typedef void __fastcall (__closure *TOnLeaderBoardEvent)( TStringList* Board );
//---------------------------------------------------------------------------
typedef UFC::PHashedList<UFC::AnsiString, UnicodeString*> StringHashedList;
typedef UFC::PHashedList<UFC::AnsiString, BasicInformation*> InfoTable;
//---------------------------------------------------------------------------
class PACKAGE TCMarketDataStore : public TCustomControl, public IMarketDataConnectionEventListener
{
private:
	MarketDataConnection* FConnection;
	SymbolCompare*        FCompareRule;
	TTimer*               FReloadTimer;
private:
	Language       FLanguage;
	UnicodeString  FIP;
	int            FPort;
	bool           FSupportOptions;
	UFC::PCriticalSection FCS;
private:
	///< Exchange Info
	UFC::PHashMap<UFC::AnsiString, UFC::PHashedList<UFC::AnsiString, ExchangeInfo*>*> FExchangeTable;
	///< Rewrite
	UFC::PHashedList<UFC::AnsiString, InfoTable*> FStoreTable;
	UFC::PHashedList<UFC::AnsiString, InfoTable*> FAliasTable;
	UFC::PHashedList<UFC::AnsiString, InfoTable*> FTradeSymbolTable;
	UFC::PHashedList<UFC::AnsiString, StringHashedList*> FChineseTable;
	UFC::PHashedList<UFC::AnsiString, ProductInfo*> FProductInfoTable;
	UFC::PHashedList<UFC::AnsiString, UFC::List<IMarketDataListener*>*>	FSubscribeTable;
	UFC::PHashedList<UFC::AnsiString, UFC::List<IMarketDataListener*>*>	FSnapShotTable;
	UFC::PHashedList<UFC::AnsiString, MarketDataMessage*> FTable;
	UFC::PHashedList<UFC::AnsiString, BasicInformation*> FQueryTable;
	UFC::PHashedList<UFC::AnsiString, UFC::List<BasicInformation*>*> FPrefixMap;
	UFC::PStringList FSymbolIndex;

	UFC::PHashedList<UFC::AnsiString, TStringList*> FFUTPrefixTable;
	UFC::PHashedList<UFC::AnsiString, TStringList*> FOPTPrefixTable;
	UFC::PHashedList<UFC::AnsiString, TStringList*> FChineseFUTPrefixTable;
	UFC::PHashedList<UFC::AnsiString, TStringList*> FChineseOPTPrefixTable;
	UFC::PHashedList<UFC::AnsiString, TStringList*> FOPTYearMonthTable;
	UFC::PHashedList<UFC::AnsiString, UFC::List<BasicInformation*>*> FOPTStrikePriceTable;
	UFC::PHashedList<UFC::AnsiString, TTickList*>   FTickTable;
	UFC::PHashedList<UFC::AnsiString, TTickList*>   FTickTableEx;
	UFC::PHashedList<UFC::AnsiString, AnsiString*>  FSpotList;
	UFC::PHashedList<UFC::AnsiString, TStringList*> FBlackList;
private:
	UFC::PCriticalSection                              FEpdateCS;
    int                                                FUpdateInterval;
	TTimer*                                            FUpdateTimer;
	UFC::PHashMap<UFC::AnsiString, MarketDataMessage*> FFilledEventMap;
	UFC::PHashMap<UFC::AnsiString, MarketDataMessage*> FDepthEventMap;
	void __fastcall UpdateFilled( void );
	void __fastcall UpdateDepth( void );
	void __fastcall OnUpdateTimer( TObject* Sender );
	void __fastcall TriggerOnFilled( MatchInfo* MatchMsg );
	void __fastcall TriggerOnDepth( OrderBookData* OrderBookMsg );
private:
	UFC::PHashMap<UFC::AnsiString,ExchangeClock*> FExchangeClockMap;
	void __fastcall SetClock( const UFC::AnsiString& Ex, int h,int m,int s );
	void __fastcall UpdateTimer( const UFC::AnsiString& Exchange, const UFC::AnsiString& Time );
public:
	ExchangeClock* __fastcall GetClock( const String& Ex );
private:
	TNotifyEvent                     FOnConnected;
	TNotifyEvent                     FOnDisconnected;
	TOnServerAckEvent                FOnServerAck;
	TOnLeaderBoardEvent              FOnLeaderBoard;
	TOnContractDownloadCompleteEvent FOnContractDownloadComplete;
	TOnContractReloadCompleteEvent   FOnContractReloadComplete;
	TOnContractReloadEvent           FOnContractReload;
private:
	UFC::PHashedSet<UFC::AnsiString>  FReloadUIDSet;
	UFC::PHashedSet<UFC::AnsiString>  FChineExSet;
	UFC::PHashedSet<UFC::AnsiString>  FOptionsExSet;
	UFC::PHashedSet<UFC::AnsiString>  FFuturesExSet;
private:
	///< Implement Market Data Listener interface
	virtual void OnConnected( void );
	virtual void OnDisconnected( void );
	virtual void OnLogonReply( bool OK,const UFC::AnsiString& Msg ) {}
	virtual void OnContractDownloadComplete( int ExchangeCount, int UseMS );
	virtual void OnServerAck( int RTT );
	virtual void OnLeaderboard( UFC::PStringList& Board );
	virtual void OnServerTimeDiffUS( UFCType::Int64 TimeDiffUS, UFCType::Int64 RTTUS );
	virtual void OnNews( const UFC::AnsiString& , const UFC::AnsiString&  );
	virtual void OnReload( const UFC::AnsiString& Exchange, UFC::PStringList& IDList, const UFC::AnsiString& UID );
	virtual void OnReload( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID );
	virtual void OnReloadComplete( const UFC::AnsiString& Exchange, UFC::PStringList& SymbolList );
	virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MarketDataMessage* Msg );
	virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, BasicData* Msg );
	virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MatchInfo* Msg );
	virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, TotalMatch* Msg );
	virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, DayHighLowPrice* Msg );
	virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, OpeningInfo* Msg );
	virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, OrderBookData* Msg );
	virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, ClosingMarketData* Msg );
	virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, UnderlyingIndexInfo* Msg );
	virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, SumOfOrderInfo* Msg );
	virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, HKBrokerQueue* Msg ) {}
	virtual void OnSystemMessage( const UFC::AnsiString& Exchange, SystemMessage* Message );
	virtual void OnRecoverFinished( const UFC::AnsiString& Exchange, int Count );
	virtual void OnRecoverFinished( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID, int Count );
	virtual void OnErrorMessage( const UFC::AnsiString& Exchange, ErrorMessage* Message );
	///< Build Table
	void __fastcall BuildQueryTable( void );
	void __fastcall BuildAliasTable( void );
	void __fastcall SetDisplayName( void );
	void __fastcall AddTAIFEXChineseName( void );
	void __fastcall AddChineseName( UFC::UiniFile& Config, const UFC::AnsiString& Exchange );
	UFC::AnsiString ToSubscribeKey( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol );
	bool __fastcall IsChinaExchange( const UFC::AnsiString );
	bool __fastcall PriceEqual( double Px1, double Px2 );
	void __fastcall RemoveSubscriber( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,const UFC::AnsiString& AnsiString );
	void __fastcall SplitSubscribeKey( const UFC::AnsiString& SubscribeKey, UFC::AnsiString& Exchange, UFC::AnsiString& Symbol );
private:
	UFC::AnsiString  FReloadExchange;
	UFC::PStringList FReloadIDList;
	void __fastcall OnReloadTimer( TObject* Sender );
protected:
	virtual void __fastcall Loaded(void);
	virtual void __fastcall Paint(void);
public:
	__fastcall TCMarketDataStore(TComponent* Owner);
	__fastcall ~TCMarketDataStore();
	void __fastcall Clear( void );
	void __fastcall Connect( int Timeout );
	void __fastcall Disconnect( void );
	void __fastcall Subscribe( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, IMarketDataListener* Listener, SubscribeType Type = stSnapshotWithUpdate );
	void __fastcall Unsubscribe( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, IMarketDataListener* Listener );
	void __fastcall Unsubscribe( IMarketDataListener* Listener );
	void __fastcall DownloadContract( void );
	int __fastcall ExchangeCount( void );
	int __fastcall SymbolCount( const UFC::AnsiString& Exchange );
	int __fastcall SymbolCount( String Exchange );
	UFC::AnsiString __fastcall GetExchange( UFC::Int32 Index ); ///< Zero based.
	UFC::AnsiString __fastcall GetSymbol( const UFC::AnsiString& Exchange, UFC::Int32 Index );

	BasicInformation* __fastcall GetBasicInformation( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, bool ThrowException = true );
	BasicInformation* __fastcall GetBasicInformation( String Exchange, String Symbol, bool ThrowException = true );
	BasicInformation* __fastcall GetBasicInformation( String Exchange, UFC::Int32 Index, bool ThrowException = true );
	BasicInformation* __fastcall GetBasicInformation( const UFC::AnsiString& Exchange, UFC::Int32 Index, bool ThrowException = true );
	UFC::PHashedList<UFC::AnsiString, BasicInformation*>* GetSymbolsByExchange( const UFC::AnsiString& Exchange );
	bool __fastcall IsConnected( void );
	int __fastcall GetDigit( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol );
	int __fastcall GetDigit( UnicodeString Exchange, UnicodeString Symbol );
	static int __fastcall GetStrikePriceDigit( const UFC::AnsiString& Exchange, const UFC::AnsiString& SeriesOrSymbol );
	static int __fastcall GetStrikePriceDigit( UnicodeString Exchange, UnicodeString SeriesOrSymbol );
	BasicInformation* __fastcall GetFUTInfo( const String& Exchange, const String& SymbolPrefix, const String& MaturityDate );
	BasicInformation* __fastcall GetFUTInfo( const String& Exchange, const String& SymbolPrefix, const String& MaturityDate1, const String& MaturityDate2 );
	BasicInformation* __fastcall GetOPTInfo( const String& Exchange, const String& SymbolPrefix, const String& MaturityDate, double StrikePrice, CallPutCode CP );
	const UFC::List<BasicInformation*>* __fastcall GetMDSeries( const String& Exchange, const String& SymbolPrefix );
	const TStringList* GetOPTMonthYear( const String& Exchange, const String& SymbolPrefix );
	const UFC::List<BasicInformation*>* __fastcall GetOPTMDSeriesWithYearMonth( const String& Exchange, const String& SymbolPrefix, const String& MonthYear );
	const TStringList* __fastcall GetFUTMDPrefix( const String& Exchange );
	const TStringList* __fastcall GetChineseFUTMDPrefix( const String& Exchange );
	const TStringList* __fastcall GetOPTMDPrefix( const String& Exchange );
	const TStringList* __fastcall GetChineseOPTMDPrefix( const String& Exchange );
	double __fastcall GetTick( const String& Exchange, const String& Symbol );
	int __fastcall GetDecimalLocator( const String& Exchange, const String& Symbol );
	void GetChinesePrefix( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID, String& ChinesePrefix );
	UnicodeString GetChinesePrefix( const String& Exchange, const String& ProductID );
	UnicodeString GetChinesePrefix( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID );
	bool __fastcall IsVisible( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID );
	double GetMaxStrikePx( const String& Exchange, const String& SymbolPrefix, const String& MonthYear );
	double GetMinStrikePx( const String& Exchange, const String& SymbolPrefix, const String& MonthYear );
	///< TickList Method
	const TTickList* __fastcall GetTickList( const String& Exchange, const String& Symbol, BasicInformation* Info );
	const TTickList* __fastcall GetTickList( const String& Exchange, const String& Symbol );
	const TTickList* __fastcall GetTickListEx( const String& Exchange, const String& Symbol, double MaxPx, double MinPx );

	BasicInformation* _fastcall FindBasicInformation( const String& Exchange, const String& Symbol );
	void __fastcall SendMessageToServer( const String& Msg, int Func = 0 );
	void __fastcall SendMessageToServer( const char* Msg, int Func = 0 );
	void __fastcall LoadChineseName( void );
	String GetExchangeDisplayName( const String& Exchange );
	bool __fastcall IsTAIFEXFutures( const String& Exchange, const String& Symbol );
	TFutProdType  TAIFEXProductType( const String& ProductID );
	TFutProdType  TAIFEXProductType( UFC::AnsiString& ProductID );
	double        ProductContractMultiplier( const String& Exchange, const String& ProductID );
	bool          TAIFEXStockProduct( UFC::AnsiString& ProductID, UFC::AnsiString& StockID );
	ExchangeInfo* GetExchangeInfo( const String& ExchangeName );
	ExchangeInfo* GetExchangeInfo( const String& ExchangeName, const String& ProductID );
	ExchangeInfo* GetExchangeInfo( const UFC::AnsiString& ExchangeName );
	ExchangeInfo* GetExchangeInfo( const UFC::AnsiString& ExchangeName, const UFC::AnsiString& ProductID );
	void SetLanguage( Language lang );
	bool __fastcall IsOpen( const UFC::AnsiString& ExchangeName, const UFC::AnsiString& ProductID );
	bool __fastcall ExchangeSupportFutures( const UFC::AnsiString& Exchange );
	bool __fastcall ExchangeSupportOptions( const UFC::AnsiString& Exchange );
	void __fastcall SetBlackList( const UFC::AnsiString& Exchange, const String& Symbol );
private: /// Rewrite
	void __fastcall LoadExchangeTable( void );
	void __fastcall AddToStoreTable( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, BasicInformation* Info );
	void __fastcall AddToAliasTable( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, BasicInformation* Info );
	unsigned long long __fastcall GetAcuRecvBytes( void );
	unsigned long long __fastcall GetAcuRecvMsgs( void );
__published:
	__property UnicodeString IP = { read = FIP, write = FIP };
	__property int Port = { read = FPort, write = FPort, default = 34567 };
	__property TNotifyEvent                     OnAppConnected              = { read = FOnConnected, write = FOnConnected };
	__property TNotifyEvent                     OnAppDisconnected           = { read = FOnDisconnected, write = FOnDisconnected };
	__property TOnContractDownloadCompleteEvent OnContractDownloadCompleted = { read = FOnContractDownloadComplete, write = FOnContractDownloadComplete };
	__property TOnContractReloadEvent           OnContractReload            = { read = FOnContractReload, write = FOnContractReload };
	__property TOnContractReloadCompleteEvent   OnContractReloadComplete    = { read = FOnContractReloadComplete, write = FOnContractReloadComplete };
	__property TOnServerAckEvent                OnMarketDataServerAck       = { read = FOnServerAck, write = FOnServerAck };
	__property TOnLeaderBoardEvent              OnLeaderBoard               = { read = FOnLeaderBoard, write = FOnLeaderBoard };
	__property unsigned long long AcmRecvBytes = { read = GetAcuRecvBytes };
	__property unsigned long long AcmRecvMsgs  = { read = GetAcuRecvMsgs };
	__property bool SupportOptions = { read = FSupportOptions };
};
//---------------------------------------------------------------------------
#endif
