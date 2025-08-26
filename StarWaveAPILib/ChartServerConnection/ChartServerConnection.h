//---------------------------------------------------------------------------
#ifndef ChartServerConnectionH
#define ChartServerConnectionH
//---------------------------------------------------------------------------
#include "MarketDataMessage.h"
#include "TMDThreadApp.h"
#include "NameValueMessage.h"
#include <math.h>

class BasicInformation;
//------------------------------------------------------------------------------
const UFC::Int32 CHART_SERVER_DEFAULT_PORT = 34569;
//------------------------------------------------------------------------------
///< Ask for prev. N days K Bars.
extern UFC::AnsiString CANDLE_HISTORY_REQUEST_SUBJECT;
extern UFC::AnsiString CANDLE_HISTORY_RESPONSE_SUBJECT;
///< Ask for today's K Bars.
extern UFC::AnsiString CANDLE_TODAY_REQUEST_SUBJECT;
extern UFC::AnsiString CANDLE_TODAY_RESPONSE_SUBJECT;
///< Ask for recover today's ticks.
extern UFC::AnsiString TICK_RECOVER_REQUEST;
extern UFC::AnsiString TICK_RECOVER_RESPONSE;
//---------------------------------------------------------------------------
//ID, Seq, Time, BuyPx, SellPx, LastPx, LastQty
//---------------------------------------------------------------------------
class TTickInfo
{
public:
	int ID;
	int Seq;
	int Time;
	int BuyIntPx;
	int SellIntPx;
	int FillIntPx;
	int FillQty;
public:
	double BuyPx;
	double SellPx;
	double FillPx;
	int    BuyPxIndex;
	int    SellPxIndex;
	int    FillPxIndex;
public:
	int    RefSec;///< Time, format:HHMMSSmm
	int    TenMS; ///< 10 ms
	bool   FShowed;
public:
#ifdef _WIN32
	__fastcall TTickInfo( void );
	__fastcall TTickInfo( const TTickInfo& Ref );
	__fastcall TTickInfo( int* RowData, int DecimalLocator );
	__fastcall TTickInfo( UFC::PStream* Stream );
#else
	TTickInfo( void );
	TTickInfo( const TTickInfo& Ref );
	TTickInfo( int* RowData, int DecimalLocator );
	TTickInfo( UFC::PStream* Stream );
#endif
	void Accumulate( const TTickInfo& Ref );
	void Average( int Count );
	void SaveToStream( UFC::PStream* Stream);
	void LoadFromStream( UFC::PStream* Stream);
};
//---------------------------------------------------------------------------
class TMinuteKInfo
{
public:
	 int   Hour;
	 int   Minutes;
	 int   Qty;

	 // 20140717 by Tim Lin
	 double MaxPx;
	 double MinPx;
	 double OpenPx;
	 double ClosePx;
	 /*
	 float MaxPx;
	 float MinPx;
	 float OpenPx;
	 float ClosePx;
	 */
public:
#ifdef _WIN32
	__fastcall TMinuteKInfo( void );
	__fastcall TMinuteKInfo( const TMinuteKInfo& Ref );
	__fastcall TMinuteKInfo( int* Array, int DecimalLocator );
#else
	TMinuteKInfo( void );
	TMinuteKInfo( const TMinuteKInfo& Ref );
	TMinuteKInfo( int* Array, int DecimalLocator );
#endif
};
//------------------------------------------------------------------------------
typedef UFC::List<TTickInfo>     TTickInfoList;
typedef UFC::List<TMinuteKInfo>  TMinuteKInfoList;
//------------------------------------------------------------------------------
class IChartServerConnectionEventListener
{
public:
	virtual void OnConnected( void ) = 0;
	virtual void OnDisconnected( void ) = 0;
	virtual void OnLastTick( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,TTickInfo& Tick ) = 0;
	virtual void OnLastKBar( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,TMinuteKInfo& KBar ) = 0;
	virtual void OnRecoverTicks( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,TTickInfoList& Ticks ) = 0;
	virtual void OnRecoverKBars( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,TMinuteKInfoList& KBars ) = 0;
	virtual void OnDayKBars( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, int Count, double MaxPx, double MinPx, UFC::PStringList& Date, UFC::List<TMinuteKInfoList*>& KBars ) = 0;
};
//------------------------------------------------------------------------------
class SubscriberInfo
{
private:
	UFC::PCriticalSection FCS;
	int                   FRef;
	bool                  FIsReg;
public:
	UFC::AnsiString Exchange;
	UFC::AnsiString Symbol;
	double          BullPx;
	double          BearPx;
	int             DecimalLocator;
	TMinuteKInfo    MinuteKBarInfo;
public:
	int IncreaseRef( void );
	int DecreaseRef( void );
	void Regist( bool );
	bool IsRegist( void );
	void Init( TTickInfo& Info );
	void Update( TTickInfo& Info );
	SubscriberInfo( void );
	SubscriberInfo( const UFC::AnsiString& Ex,
					const UFC::AnsiString& Sym,
					int decimallocator,
					double BullPrice,
					double BearPrice );
};
//------------------------------------------------------------------------------
class StarWaveCSConnection : /*public UFC::PThread,*/ public MessageListener, public MAppListener
{
typedef void (StarWaveCSConnection::*MDEventFunc)( const UFC::AnsiString& Key, MTree* Msg );
private:
	MApp* 						FApp;
	TMDThreadApp*   			FThread;
	HINSTANCE 					FInstance;
	UFC::AnsiString				FIPAddress;
	UFC::Int32					FPort;
	UFC::AnsiString				FAppName;
	UFC::AnsiString				FRecoverKey;
private:
	UFC::UInt64					FAcuRecvBytes;
	UFC::UInt64					FAcuRecvMsgs;
private:
	UFC::PCriticalSection	                           FCS;
	UFC::PHashedList<UFC::AnsiString, SubscriberInfo*> FSubscribeInfoTable;
	UFC::PHashedList<UFC::AnsiString, MSubscriber*>    FSubscribeTable;
private:
	IChartServerConnectionEventListener* FListener;
	MDEventFunc                                     FOnTickMsg;
	MDEventFunc                                     FOnTodayKMsg;
	MDEventFunc                                     FOnNDaysKMsg;
	UFC::PHashedList<UFC::AnsiString, MDEventFunc*> FCallbackFuncs;
private: ///< Impelment interface MAppListener
	virtual void                OnMAppConnected( void );
	virtual void                OnMAppDisconnected( void );
	virtual void                OnMAppError( PMAppError Error );
private:  ///< Impelment interface MessageListener
	virtual void                OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* pTree );
	void TodayTicks( MTree *Tree );
	void LastTick( MTree *Tree );
	void TickData(  const UFC::AnsiString& Key, MTree *Tree );
	void TodayCandle( const UFC::AnsiString& Key, MTree *Tree );
	void PrevNDaysCandle( const UFC::AnsiString& Key, MTree *Tree );
	void SubscribeSymbol( SubscriberInfo* RegInfo, const UFC::AnsiString& CurrSymbol );
	void UnsubscribeSymbol( SubscriberInfo* RegInfo, const UFC::AnsiString& CurrSymbol );
private:
	BOOL Send( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Msg );
	void AddListener( const UFC::AnsiString& Subject, const UFC::AnsiString& Key );
	void DelListener( const UFC::AnsiString& Subject, const UFC::AnsiString& Key );
	void RemoveAllListener( void );
public:
    /**
	 *  Constractor for using C++ library.
	 *  @Param Instance HInstance of this application.(Win32 Only, Linux Ignore this param.)
	 *  @Param AppName given this client application a name.
	 *  @Param IChartServerConnectionEventListener Listener to handle marketdata and connection events.
	 *  @Param IsWin32GUIApp TRUE for Win32 GUI Application, FALSE for Linux, Unix, Win32 console application.1
	 */
	StarWaveCSConnection( HINSTANCE Instance,
    			      const UFC::AnsiString& AppName,
		 	      IChartServerConnectionEventListener* pListener,
			      BOOL IsWin32GUIApp );
	/**
	 * Destructor.
	 */
	~StarWaveCSConnection( void );
	/**
	 *  Connect to the Chart server.
	 *  @Param IPAddress: IP of the ChartServer.
	 *  @Param Port:      Service port of the ChartServer.
	 *  @Param Timeout:   Connection timeout.(sec)
	 */
	void Connect( const UFC::AnsiString& IPAddress, UFC::Int32 Port = CHART_SERVER_DEFAULT_PORT, int Timeout = 10 );
	/**
	 * Try to close this ChartServer connection.
	 */
	void Disconnect( void );
	/**
	 * Is connected ?
	 */
	bool IsConnected( void );

	/**
	 * To subscribe:
	 * - Prev. N days K Bars.
	 * - Today's K Bars.
	 * - Realtime tick.
	 *
	 * @Param Contract:       The contruct object which you want to subscribe .
	 *
	 * @Param Exchange:       Exchange code of this contract.(TAIFEX,CME,SGX...)
	 * @Param Symbol:         Symbol of this contract.(TXFC3,ESZ2,STWV2...)
	 * @Param DecimalLocator: Decimal locator of the price.
	 * @Param BullPx:         Bull price of this contract.
	 * @Param BearPx:         Bear price of this contract.
	 */
	void Subscribe( BasicInformation* Contract );
	void Subscribe( const UFC::AnsiString& Exchange,const UFC::AnsiString& Symbol,int DecimalLocator,double BullPx,double BearPx );
	/**
	 * Unsubscribe K bar & tick data.
	 */
	void Unsubscribe( BasicInformation* Contract );
	void Unsubscribe( const UFC::AnsiString& Exchange,const UFC::AnsiString& Symbol );
	/**
	 * Recover ticks data from server.
	 * @Param Contract: The contruct object which you want to subscribe .
	 * @Param Begin:    The first tick sequence you want to recover.
	 * @Param End:      The last tick sequence.
	 */
	void RequestTicks( BasicInformation* Contract, int Begin, int End );
	void RequestTicks( const UFC::AnsiString& Exchange,const UFC::AnsiString& Symbol, int Begin, int End );
	void SetEventListener( IChartServerConnectionEventListener* Listener ) { FListener = Listener; }
public:
	const UFC::AnsiString& GetIP( void )           { return FIPAddress; }
	int                    GetPort( void )         { return FPort; }
	UFC::UInt64            GetAcuRecvBytes( void ) { return FAcuRecvBytes; }
	UFC::UInt64            GetAcuRecvMsgs( void )  { return FAcuRecvMsgs; }
};
//------------------------------------------------------------------------------
#endif
