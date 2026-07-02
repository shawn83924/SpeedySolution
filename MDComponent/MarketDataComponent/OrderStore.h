//---------------------------------------------------------------------------
#ifndef OrderStoreH
#define OrderStoreH
//---------------------------------------------------------------------------
#include <queue>
#include <map>
#include <vector>
#include <list>
#include "OrderMessageDefine.h"
#include "TNewOrderMessage.h"
#include "TCancelOrderMessage.h"
#include "CTPObjects.h"
#include "MarketDataStore.h"
#include "TExecutionUnit.h"
#include "TradingUtility.h"
#include "TradingBasicObjects.h"
#include "TradingCommodities.h"
#include "TradingPosition.h"
#include "OrderAdapter.h"
#include "UFC.h"
#include <SysUtils.hpp>
#include <Classes.hpp>
#include "MDComponentStrings.hpp"

using namespace nsOrderMessageDefine;
//---------------------------------------------------------------------------
typedef enum
{
	osErrMessageExist      = 0,
	osErrNotCorrectMessage = 1,
	osErrOrderNotExist     = 2,

} OrderStoreErrorTypeEnum;
//---------------------------------------------------------------------------
typedef enum
{
	soNew    = 0,
	soRemove = 1,
	soSend   = 2

} StopOrderStatusEnum;
//---------------------------------------------------------------------------
class TFilledData
{
public:
	nsOrderMessageDefine::SideEnum Side;
	double                         FilledPx;
	int                            Qty;
	int                            hhmm;
public:
	TFilledData(nsOrderMessageDefine::SideEnum bs,double px,int qty,int time)
	:Side( bs )
	,FilledPx( px )
	,Qty( qty )
	,hhmm( time ){}

	TFilledData( const TFilledData& Ref )
	:Side( Ref.Side )
	,FilledPx( Ref.FilledPx )
	,Qty( Ref.Qty )
	,hhmm( Ref.hhmm ){}
	int GetCompareHHMM( void )
	{
		if( hhmm < 510 ) ///< Time < 5:10 should be nest day.
			return (hhmm + 2400);
		return hhmm;
    }
};
//---------------------------------------------------------------------------
class TFilledListener
{
public:
	virtual void OnRecoverFilled( const String& Ex,const String& Sym, UFC::List<TFilledData>& Filleds ) = 0;
	virtual void OnFilled( const String& Ex,const String& Sym, TFilledData& Filled ) = 0;
};
//---------------------------------------------------------------------------
class TOrderMessageListener
{
public:
	virtual void OrderMessageArrived( TExecution* OrderRootPtr) = 0;
	virtual void DeleteRejectedOrderRoot( TExecution* OrderRootPtr) = 0;
	virtual void RefreshDataNotice() = 0;
	virtual void StopOrderChanged(TExecution* OrderRootPtr, StopOrderStatusEnum StopOrderStatus) = 0;
};
//---------------------------------------------------------------------------
class TOrderStatusListener
{
public:
	virtual void OrderStatusChanged( nsOrderMessageDefine::MarketEnum Market,
									 const String& OrderID) = 0;
};
//---------------------------------------------------------------------------
class ICACheckListener
{
public:
	virtual void OnCACheck( nsOrderMessageDefine::MarketEnum Market,
							SendMessageType Type,
							TBaseMessage* Msg,
							bool& CanSend,
							UFC::AnsiString& RejectMsg) = 0;
};
//---------------------------------------------------------------------------
class TOrderQtyListener
{
public:
	virtual void OrderQtyChanged( nsOrderMessageDefine::MarketEnum Market,
								  const String& Symbol,
								  nsOrderMessageDefine::SideEnum Side,
								  double Px, int Qty) = 0;
	virtual void StopOrderQtyChanged( nsOrderMessageDefine::MarketEnum Market,
									  const String& Symbol, double StopPx,
									  nsOrderMessageDefine::SideEnum Side,
									  double Px, int Qty,
									  nsOrderMessageDefine::OrderTypeEnum OrderType) = 0;
};
//---------------------------------------------------------------------------
class TPositionChangeListener
{
public:
	virtual void OnFill(int Position, double BuyArvPx, double SellArvPx, int BuyQty, int SellQty,
						int BuyOpenInterestQty, int SellOpenInterestQty,
						DynamicArray<void*>& PosStatisticsArray, int NetPosition, double FloatingProfit) = 0;
	virtual void OnFill(nsOrderMessageDefine::SideEnum Side, double Px, int Qty, int TickCount) = 0;  //For Ping Pong
	virtual void OnProfit(double AveragePrice, double ProfitAmount) = 0;
};
//---------------------------------------------------------------------------
class IStrategyOrderListener
{
public:
	virtual void OnFill( const UFC::AnsiString& Symbol, double LastPx, int LastQty, int LeavesQty ) = 0;
};
//---------------------------------------------------------------------------
class OSConnectionNotExistException : public UFC::Exception
{
public:
	static UnicodeString OSExceptionMsg;
	OSConnectionNotExistException():UFC::Exception( OSExceptionMsg.c_str() ){}
};
//---------------------------------------------------------------------------
class OSConnectionNotConnectedException : public UFC::Exception
{
public:
	static UnicodeString OSExceptionMsg;
	OSConnectionNotConnectedException():UFC::Exception( OSExceptionMsg.c_str() ){}
};
//---------------------------------------------------------------------------
class OSConnectionNotLogonException : public UFC::Exception
{
public:
	static UnicodeString OSExceptionMsg;
	OSConnectionNotLogonException():UFC::Exception( OSExceptionMsg.c_str() ){}
};
//---------------------------------------------------------------------------
class OSNewOrderException : public UFC::Exception
{
public:
	static UnicodeString OSExceptionMsg;
	OSNewOrderException():UFC::Exception( OSExceptionMsg.c_str() ){}
};
//---------------------------------------------------------------------------
class OSCancelOrderException : public UFC::Exception
{
public:
	static UnicodeString OSExceptionMsg;
	OSCancelOrderException():UFC::Exception( OSExceptionMsg.c_str() ){}
};
//---------------------------------------------------------------------------
class OSAmendOrderException : public UFC::Exception
{
public:
	static UnicodeString OSExceptionMsg;
	OSAmendOrderException():UFC::Exception( OSExceptionMsg.c_str() ){}
};
//---------------------------------------------------------------------------
class OSReplaceOrderPriceException : public UFC::Exception
{
public:
	static UnicodeString OSExceptionMsg;
	OSReplaceOrderPriceException():UFC::Exception( OSExceptionMsg.c_str() ){}
};
//---------------------------------------------------------------------------
class OSRequestOrderStatusException : public UFC::Exception
{
public:
	static UnicodeString OSExceptionMsg;
	OSRequestOrderStatusException():UFC::Exception( L"『交易所連線物件』查詢錯誤 !" ){}
};
//---------------------------------------------------------------------------
UnicodeString OSConnectionNotExistException::OSExceptionMsg     = Mdcomponentstrings_MD_ORDERSTORE_CONNECTION_NOT_EXIST_EXCEPTION;
UnicodeString OSConnectionNotConnectedException::OSExceptionMsg = Mdcomponentstrings_MD_ORDERSTORE_CONNECTION_NOT_CONNECTED;
UnicodeString OSConnectionNotLogonException::OSExceptionMsg     = Mdcomponentstrings_MD_ORDERSTORE_CONNECTION_NOT_LOGON;
UnicodeString OSNewOrderException::OSExceptionMsg               = Mdcomponentstrings_MD_ORDERSTORE_NEW_ORDER_ERROR;
UnicodeString OSCancelOrderException::OSExceptionMsg            = Mdcomponentstrings_MD_ORDERSTORE_CANCEL_ERROR;
UnicodeString OSAmendOrderException::OSExceptionMsg             = Mdcomponentstrings_MD_ORDERSTORE_AMEND_ERROR;
UnicodeString OSReplaceOrderPriceException::OSExceptionMsg      = Mdcomponentstrings_MD_ORDERSTORE_REPLACE_PRICE_ERROR;
//---------------------------------------------------------------------------
typedef std::map<UnicodeString, int> CStringIntMap;
typedef std::map<int, TExecution*> CIntTExecutionMap;
typedef std::map<UFCType::Int64, TExecution*> CInt64TExecutionMap;
typedef std::map<UnicodeString, TExecution*> CStringTExecutionMap;
typedef std::multimap<UnicodeString, TExecution*> CStringTExecutionMultiMap;
typedef std::multimap<UnicodeString, TOrderQtyListener*> CStringTOrderQtyListenerMultiMap;
typedef std::multimap<UnicodeString, TPositionChangeListener*> CStringTPositionChangeListenerMultiMap;
//---------------------------------------------------------------------------
class TOrderAdapter;
//------------------------------------------------------------------------------
// Class TOrderStore
//------------------------------------------------------------------------------
class PACKAGE TOrderStore : public TComponent, public IMarketDataListener
{
typedef void __fastcall (__closure *TOnLogonOK)    (System::TObject* Sender, const String& ReplyMessage, int CID);
typedef void __fastcall (__closure *TOnLogonFailed)(System::TObject* Sender, const String& ReplyMessage, int CID);
typedef void __fastcall (__closure *TOnNewOrder)   (System::TObject* Sender, const String& Exchange, const String& Symbol,
													nsOrderMessageDefine::SideEnum Side, double Px, int Qty,
													nsOrderMessageDefine::OrderTypeEnum OrderType,
													nsOrderMessageDefine::TimeInForceEnum TimeInForce,
													nsOrderMessageDefine::PositionEffectEnum PositionEffect,
													bool& IsAccept);
typedef void __fastcall (__closure *TOnCancelByOrderID)(System::TObject* Sender, const String& OrderID, bool& IsAccept);
typedef void __fastcall (__closure *TOnCancelBySymbolPrice)(System::TObject* Sender, const String& Symbol,
															nsOrderMessageDefine::SideEnum Side, double Price, bool& IsAccept);
typedef void __fastcall (__closure *TOnCancelStatistic)(System::TObject* Sender, const String& ExchangeCode, const String& Symbol, int CancelQuantity);
typedef void __fastcall (__closure *TOnNewOrderReply)(System::TObject* Sender, const String& OrderID, const String& Symbol,
													  nsOrderMessageDefine::SideEnum Side, double Px, int Qty,
													  nsOrderMessageDefine::OrderTypeEnum OrderType,
													  nsOrderMessageDefine::TimeInForceEnum TimeInForce,
													  nsOrderMessageDefine::PositionEffectEnum PositionEffect);
typedef void __fastcall (__closure *TOnFilledReply)(System::TObject* Sender,
													const String& Exchange,
													const String& Symbol,
													const String& OrderID,
													nsOrderMessageDefine::SideEnum Side,
													double Price, int Qty,
													nsOrderMessageDefine::OrderStatusEnum OrderStatus);
typedef void __fastcall (__closure *TOnStopOrderChange)(System::TObject* Sender, const String& Exchange, const String& StopSymbol, double StopPrice,
														const String& Symbol, double Price, int Qty,
														nsOrderMessageDefine::TimeInForceEnum TimeInForce,
														nsOrderMessageDefine::SideEnum Side,
														StopOrderStatusEnum StopStatus, bool& IsAccept);
typedef void __fastcall (__closure *TOnStopOrderError)(System::TObject* Sender, const String& StopSymbol, double StopPrice,
													   const String& Symbol, double Price, int Qty,
													   nsOrderMessageDefine::TimeInForceEnum TimeInForce,
													   nsOrderMessageDefine::SideEnum Side,
													   const String& ErrorMessage);
typedef void __fastcall (__closure *TOnErrorMessage)(System::TObject* Sender, int MsgHour, int MsgMinute, int MsgSecond, int MsgMilliSecond,
													 OrderStoreErrorTypeEnum OrderStoreErrorType,
													 nsOrderMessageDefine::OrderStatusEnum OrderStatus,
													 const String& OrderID, UFCType::Int64 NID, const String& Symbol, nsOrderMessageDefine::SideEnum Side, double Px, int Qty);
typedef void __fastcall (__closure *TOnMarginsQueryReply)(System::TObject* Sender, String Reply );
typedef void __fastcall (__closure *TOnServerAckEvent)(System::TObject* Sender, int RTT );
typedef void __fastcall (__closure *TOnFloatingProfit)(System::TObject* Sender,
													   const String& ExchangeCode, const String& Symbol,
													   void *PositionStatisticRecordPtr);
typedef void __fastcall (__closure *TOnAdminMessage)(System::TObject* Sender, const String& Message, bool ForceLogout );
typedef void __fastcall (__closure *TOnStrategyReport)(const UFC::AnsiString& FieldValueStr, const UFC::AnsiString& StrategyName, int NID);

private:
	static bool                      FIsTestMode;
	static bool                      FIsTestMatchPrice;
	static bool                      FIsAutoPositionEffect;
	static bool                      FCanSeparateCloseTodayOrder;  //是否自動拆平今、平倉
	static bool                      FCanSeparateOpenOrder;        //是否自動拆新倉、平倉(若為true，設定自動拆平今、平倉)
	static bool                      FIsContractDownloadCompleted;
	UnicodeString                    FAppName;
	UnicodeString                    FIP;
	int                              FPort;
	UnicodeString                    FID;
	UnicodeString                    FPassword;
	UnicodeString                    FAccount;
	UnicodeString                    FTWSEAccount;
	UnicodeString                    FBrokerID;
	UnicodeString                    FTWSEBrokerID;
	UnicodeString                    FClearMemberID;
	UnicodeString	                 FExchange;
	UnicodeString	                 FActiveExecutive;
	UnicodeString	                 FIBID;
	APIType                          FUseAPI;

	UnicodeString                    FOrderLogPath;
	UnicodeString                    FOrderLogFileNamePrefix;
	UnicodeString                    FOrderMsgLogFileName;
	UnicodeString                    FStartTimeFileName;
	UnicodeString                    FStopOrderFileName;
	UnicodeString                    FConnectionLogFileName;
	TFileStream* 					 FOrderMsgFileStream;
	TFileStream* 					 FStopOrderFileStream;
	bool                             FIsFirstTime;
	UnicodeString                    FRecoverStartTime;
	bool							 FChannel;
	TOrderAdapter*                   FAdapter;
	TCMarketDataStore*               FMarketDataStore;
	UnicodeString                    FVersion;
	int                              FRecoverOverlapSecond;
private: ///< Events
	TNotifyEvent                     FOnConnect;
	TNotifyEvent                     FOnDisconnect;
	TOnLogonOK                       FOnLogonOK;
	TOnLogonFailed                   FOnLogonFailed;
	TOnNewOrder                      FOnNewOrder;
	TOnCancelByOrderID               FOnCancelByOrderID;
	TOnCancelBySymbolPrice           FOnCancelBySymbolPrice;
	TOnCancelStatistic               FOnCancelStatistic;
	TOnNewOrderReply                 FOnNewOrderReply;
	TOnFilledReply                   FOnFilledReply;
	TNotifyEvent                     FOnRejectedReply;
	TNotifyEvent                     FOnOrderStatusChange;
	TNotifyEvent                     FOnBeginRecover;
	TNotifyEvent                     FOnRecoverFinished;
	TOnStopOrderChange               FOnStopOrderChange;
	TOnStopOrderError                FOnStopOrderError;
	TOnErrorMessage                  FOnErrorMessage;
	TOnMarginsQueryReply			 FOnMarginsQueryReply;
	TOnServerAckEvent                FOnServerAck;
	TOnFloatingProfit                FOnFloatingProfit;
	TOnAdminMessage                  FOnAdminMessage;
	TOnStrategyReport                FOnStrategyReport;
private:
	TTimer*                          FMarketDataTimer;
	int                              FMarketDataTimerInterval;
	bool                             FIsPropTrade;
	void __fastcall                  OnMarketDataTimer(TObject *Sender);
	bool                                FIsEDS;
	std::queue<s888::CCTPBasicCommand*> FCTPCommandQueue;
	UFC::PCriticalSection               FCTPCommandQueueLock;
	///< Order Root, Execution maps
	UFC::PtrList<TExecution>         FOrderRootList;
	CInt64TExecutionMap              FOrderRootNIDIndex;
	CStringTExecutionMap             FOrderRootOrderIDIndex;
	///< For OrderListVile display
	CStringTExecutionMap             FOrderRootTimeOrderIDIndex;         ///< All order page
	CStringTExecutionMap             FFilledOrderRootTimeOrderIDIndex;   ///< Fill order page
	CStringTExecutionMap             FCanceledOrderRootTimeOrderIDIndex; ///< Cancel order page
	CStringTExecutionMap             FActiveOrderRootTimeOrderIDIndex;   ///< Active order page
	CStringTExecutionMultiMap        FOrderSymbolSidePriceIndex;
	///< For Stop Order
	///< Key: Time + NID
	CStringTExecutionMap             FStopOrderTimeNIDIndex;             ///< Stop order page
	//UFC::PHashMap<UFC::AnsiString, TExecution*> FStopOrderTimeNIDMap;
	///< Key: Exchange + Symbol + Stop Price
	CStringTExecutionMultiMap        FStopOrderConditionIndex;
	//Message
	UFC::PtrList<TExecution>         FOrderMessageList;
	CStringTExecutionMap             FOrderMessageExecIDIndex;
	CStringTExecutionMultiMap        FFillMessageSymbolIndex;
	//Position Statistic Record
	UFC::PCriticalSection        FPosStatisticRecCSLock;
	s888::CWStrTradingObjPtrHMap FPosStatisticRecTradeSymbolIndex;
	s888::CTradingObjPtrVector   FPosStatisticRecArray;
	s888::CTradingObjQueue       FPosStatisticRecWaitingQueue;
	//Listener
	CStringTOrderQtyListenerMultiMap       FOrderQtyListenerMap;
	UFC::PtrList<TOrderMessageListener>    FOrderMessageListenerList;
	CStringTPositionChangeListenerMultiMap FPositionChangeListenerMap;
	UFC::PHashedList<UFC::AnsiString, IStrategyOrderListener*> FStrategyListenerTable;

	UFC::PHashMap<UFC::AnsiString, UFC::PtrList<TFilledListener>*> FFilledListenerMap;
	int FCancelCommandCount;
	int FCancelReportCount;
	bool FIsTry;
	int  FCanUse;
    int  FMaxLots;
	int ProdIDMargin( const UFC::AnsiString& ProdID );
private: ///< Access property functions
	void __fastcall SetAppName(UnicodeString NewAppName);
	void __fastcall SetIP(UnicodeString NewIP);
	void __fastcall SetBrokerID(UnicodeString NewBrokerID);
	void __fastcall SetTWSEBrokerID(UnicodeString NewBrokerID);
	void __fastcall SetClearMemberID(UnicodeString NewClearMemberID);
	void __fastcall SetPort(int NewPort);
	void __fastcall SetID(UnicodeString NewID);
	void __fastcall SetPassword(UnicodeString NewPassword);
	void __fastcall SetAccount(UnicodeString NewAccount);
   	void __fastcall SetTWSEAccount(UnicodeString NewAccount);
	void __fastcall SetOrderLogPath(UnicodeString NewOrderLogPath);
	void __fastcall SetOrderLogFileNamePrefix( UnicodeString NewOrderLogFileNamePrefix );
	void __fastcall SetChannel( bool Channel );
	void __fastcall SetIsTry( bool IsTry );
	bool __fastcall GetChannel( void );
	void  __fastcall SetUserName( String Name );
	void  __fastcall SetActiveExecutive( String AE );
	void  __fastcall SetIBID( String IB );
	bool  __fastcall GetIsEDS( void );
	void  __fastcall SetIsEDS( bool IsEDS );
	String  __fastcall GetUserName( void );
	String __fastcall GetTradingDate( void );
	String __fastcall UTF8String( const char* CStr );
	String __fastcall UTF8String( const String& CStr );
	void __fastcall GetTempLogPath( String& TempOrderLogPath );
	void __fastcall UpdateOrderLogFileNamePrefix( void );
	bool __fastcall NeedClearOrderFiles(  const AnsiString& StartDateString  );
private:
	const String& __fastcall ConvertExCode( const String& ExCode );
	bool __fastcall IsActiveeOrder( nsOrderMessageDefine::OrderStatusEnum OrderRootStatus );
	void __fastcall CancelAndNewAsReplace(  nsOrderMessageDefine::MarketEnum Market,
											const String&                    Exchange,
											double                           NewPrice,
											TExecution*                      CurOrderRootPtr );
	void __fastcall CancelAndNewAsReplace(  nsOrderMessageDefine::MarketEnum      Market,
											const String&                         Exchange,
											nsOrderMessageDefine::OrderTypeEnum   OrderType,
											nsOrderMessageDefine::TimeInForceEnum TimeInForce,
											double                                NewPrice,
											TExecution*                           CurOrderRootPtr );
public:
	static bool IsTestMode();
	static void SetTestMode(bool IsTestMode);
	static bool IsTestMatchPrice();
	static void SetTestMatchPrice(bool IsTestMatchPrice);
	static bool IsAutoPositionEffect();
	static void SetIsAutoPositionEffect(bool IsAutoPositionEffect);
	static bool CanSeparateCloseTodayOrder();
	static void SetCanSeparateCloseTodayOrder(bool CanSeparateCloseTodayOrder);
	static bool CanSeparateOpenOrder();
	static void SetCanSeparateOpenOrder(bool CanSeparateOpenOrder);
	static bool IsContractDownloadCompleted();
	static void SetContractDownloadCompleted(bool IsContractDownloadCompleted);
public:
	__fastcall TOrderStore(TComponent* Owner);
	__fastcall ~TOrderStore();
	void __fastcall Loaded();
	void __fastcall InitialOrderAdapter( void );
	String __fastcall ReadOrderLogStr(TFileStream *OrderLogStmPtr);
	void __fastcall RecoverFromNet();
	void __fastcall RecoverOrderLogFileData();
	bool __fastcall RecoverFileExist( void );
	void __fastcall RemoveRecoverFile( void );
	void __fastcall CloseOrderLogFile();
	void __fastcall Connect();
	void __fastcall Disconnect(bool DoClearBussiness = true);
	bool __fastcall IsConnected();
	void __fastcall LogonProxy( void );
	void __fastcall LogonPropTrade( void );
	void __fastcall Logoff(bool DoClearBussiness = true);
	bool __fastcall IsLogon();
	bool __fastcall IsReady();
	bool __fastcall IsRecovering();
	const String& __fastcall GetLocalIP( void );
	UnicodeString GetConnectionLogFileName();

	void BuildExchangeSymbolIndexKey(const UnicodeString& ExchangeCode, const UnicodeString& SymbolCode, UnicodeString& IndexKey);
	void ParseExchangeSymbolIndexKey(const UnicodeString& IndexKey, UnicodeString& ExchangeCode, UnicodeString& SymbolCode);
	BasicInformation *GetCommodityBasicInformation(const String& ExchangeCode, const String& Symbol);
	String __fastcall GetTradeSymbol(const String& ExchangeCode, const String& OrderSymbol, bool CheckSymbolIndexFirst = true);
	String __fastcall GetOrderSymbol(const String& ExchangeCode, const String& OrigSymbol, bool CheckTradeSymbolIndexFirst = true);
	String __fastcall GetMarketAccountForOrder( nsOrderMessageDefine::MarketEnum Market );
public: ///< Order placement
	///< New Order
	bool NewOrder( nsOrderMessageDefine::MarketEnum Market,
				   const String& Exchange,
				   const String& Symbol,
				   nsOrderMessageDefine::SideEnum Side,
				   double Px,
				   int Qty,
				   nsOrderMessageDefine::OrderTypeEnum OrderType,
				   nsOrderMessageDefine::TimeInForceEnum TimeInForce,
				   nsOrderMessageDefine::PositionEffectEnum PositionEffect,
				   nsOrderMessageDefine::EventTypeEnum EventType,//= evtNone
				   double StrikePx, // = 0.0
				   int TickCount,
				   double StopPx,
				   char TWSEOrdType );
	bool NewOrder( nsOrderMessageDefine::MarketEnum Market,
				   const String& Exchange,
				   const String& Symbol,
				   nsOrderMessageDefine::SideEnum Side,
				   double Px,
				   int Qty,
				   nsOrderMessageDefine::OrderTypeEnum OrderType,
				   nsOrderMessageDefine::TimeInForceEnum TimeInForce,
				   nsOrderMessageDefine::PositionEffectEnum PositionEffect,
				   nsOrderMessageDefine::EventTypeEnum EventType,//= evtNone
				   double StrikePx, // = 0.0
				   int TickCount,
				   double StopPx,
				   const UFC::AnsiString& StrategyName,
				   char TWSEOrdType,
				   UFCType::Int64& NID );
	UFCType::Int64 NewOrder( const UFC::AnsiString& FieldValueStr,
							 const UFC::AnsiString& StrategyName,
							 int TickCount = 0);
	///< Cancel Order
	UFCType::Int64 CancelOrder( nsOrderMessageDefine::MarketEnum Market,
								const String& Exchange,
								const String& OrderID,
								const UFC::AnsiString& StrategyName = "" ); ///< By OrderID
	///< Cancel Order by side + price.( user click cancel from Depth view )
	int CancelOrder( nsOrderMessageDefine::MarketEnum Market,
					 const String& Exchange,
					 const String& SymbolCode,
					 nsOrderMessageDefine::SideEnum Side,
					 double Price,
					 bool SkipAsk = false ); ///< By side + price
	int CancelAllOrder( const String& Exchange,
						const String& Symbol ); ///< All Exchange + Symbol
	UFCType::Int64 CancelOrder( const UFC::AnsiString& FieldValueStr,
								const UFC::AnsiString& StrategyName); ///< Buy Strategy Name
	void CancelAllOrder( void );
	///< Replace order
	void AmendOrder( nsOrderMessageDefine::MarketEnum Market,
					 const String& Exchange,
					 const String& OrderID,
					 int Qty);
	void ReplaceOrderPrice( nsOrderMessageDefine::MarketEnum Market,
							const String& Exchange,
							const String& OrderID,
							nsOrderMessageDefine::OrderTypeEnum OrderType,
							nsOrderMessageDefine::TimeInForceEnum TimeInForce,
							double NewPrice,
							bool SupportRPX  );
	void ReplaceOrderPrice( nsOrderMessageDefine::MarketEnum Market,
							const String& Exchange,
							const String& SymbolCode,
							nsOrderMessageDefine::SideEnum Side,
							double Price,
							double NewPrice,
							bool SupportRPX );
	void RequestOrderStatus( int OrderNID,
							 nsOrderMessageDefine::MarketEnum Market,
							 const String& Exchange,
							 const String& OrderID,
							 const String& Symbol,
							 nsOrderMessageDefine::SideEnum Side,
							 double Px );
	UFC::AnsiString GetBalance( void );
	/// < New Stop Order
private:
	void NewStopOrder( TExecution* NewStopOrderPtr, bool UpQtyOnly = true);
	int  ToNewHHMM( int oldHHMM, const String& OrigTime );
	bool AddStopOrder( TExecution* NewStopOrderPtr );
	String NormalizeBalanceJSON( const String& raw );
public:
	void NewStopOrderRequest( const String& StopExchangeCode, const String& StopSymbol,
							  double StopPrice,
							  nsOrderMessageDefine::MarketEnum Market,
							  const String& ExchangeCode, const String& Symbol,
							  nsOrderMessageDefine::SideEnum Side,
							  double Px, int Qty,
							  nsOrderMessageDefine::OrderTypeEnum OrderType,
							  nsOrderMessageDefine::TimeInForceEnum TimeInForce,
							  nsOrderMessageDefine::PositionEffectEnum PositionEffect,
							  nsOrderMessageDefine::EventTypeEnum EventType,//= evtNone
							  double StrikePx, // = 0.0
							  bool MustTrigerOnClient = false,
							  bool UpQtyOnly = true );
	void CancelStopOrder( const UnicodeString& StopExchangeCode, const UnicodeString& StopSymbol, double StopPrice,
						  const UnicodeString& ExchangeCode, const UnicodeString& Symbol, double Price,
						  nsOrderMessageDefine::SideEnum Side);
	int CancelStopOrder( const UnicodeString& StopExchangeCode, const UnicodeString& StopSymbol,
						 double StopPrice, nsOrderMessageDefine::SideEnum Side);
	int CancelStopOrder(const UnicodeString& ExchangeCode, const UnicodeString& Symbol, nsOrderMessageDefine::SideEnum Side);
	int CancelStopOrder(const String& ExchangeCode, const String& Symbol);
    bool HasStopOrder();
	void CancelAllStopOrder();
	void CancelStopOrderQuantity(const String& StopExchangeCode, const String& StopSymbol, double StopPrice,
								 const String& ExchangeCode, const String& Symbol, double Price,
								 nsOrderMessageDefine::SideEnum Side, int CancelQuantity);
	bool CheckStopOrder( const AnsiString& Ex, const AnsiString& Sym, double MatchPrice, double StopPrice );
	bool ExecuteStopOrder( const String& MatchExchange, const String& MatchSymbol, double MatchPrice);
	void StopOrderToNewOrder( TExecution* CurStopOrderPtr );
	void ProcessStopOrder(const String& MatchExchange, const String& MatchSymbol, double MatchPrice);
	double GetNewestPrice(const String& ExchangeCode, const String& Symbol);
	bool CheckStopOrder(double CurrentMarketPrice, double StopPrice,
						nsOrderMessageDefine::SideEnum Side,
						nsOrderMessageDefine::OrderTypeEnum OrderType,
						double OrderPrice, String& ErrorMessage);
	void UpdateDisplayName( void );

	//Order Root
	void AddOrderRoot(TExecution* OrderRootPtr);
	int QueryPendingNewOrderRoot();
	int DeleteRejectedOrderRoot(int PreserveCount);

	TExecution* FindOrderRootByNID( UFCType::Int64 NIDValue);
	bool InsertOrderRootNIDIndex(TExecution* OrderRootPtr, bool NeedCheckExist = false);

	TExecution* FindOrderRootByOrderID(UnicodeString OrderID);
	bool InsertOrderRootOrderIDIndex(TExecution* OrderRootPtr, bool NeedCheckExist = false);

	TExecution* FindOrderRootByTimeOrderID(UnicodeString TimeOrderIDKey);
	bool InsertOrderRootTimeOrderIDIndex(TExecution* OrderRootPtr, bool NeedCheckExist = false);
	bool RemoveOrderRootByTimeOrderID(UnicodeString TimeOrderIDKey);

	TExecution* FindFilledOrderRootByTimeOrderID(UnicodeString TimeOrderIDKey);
	bool InsertFilledOrderRootTimeOrderIDIndex(TExecution* OrderRootPtr, bool NeedCheckExist = false);
	bool RemoveFilledOrderRootByTimeOrderID(UnicodeString TimeOrderIDKey);

	TExecution* FindCanceledOrderRootByTimeOrderID(UnicodeString TimeOrderIDKey);
	bool InsertCanceledOrderRootTimeOrderIDIndex(TExecution* OrderRootPtr, bool NeedCheckExist = false);
	bool RemoveCanceledOrderRootByTimeOrderID(UnicodeString TimeOrderIDKey);

	TExecution* FindActiveOrderRootByTimeOrderID(UnicodeString TimeOrderIDKey);
	bool InsertActiveOrderRootTimeOrderIDIndex(TExecution* OrderRootPtr, bool NeedCheckExist = false);
	bool RemoveActiveOrderRootByTimeOrderID(UnicodeString TimeOrderIDKey);

	bool InsertOrderRootSymbolSidePriceIndex(TExecution* OrderRootPtr);
	bool RemoveOrderRootSymbolSidePriceIndex(TExecution* OrderRootPtr);
	int SumSymbolSidePriceLeavesQuantity(String OrderSymbol, nsOrderMessageDefine::SideEnum Side, double Price);

	void ClearOrderRootData();
	void QueryMargins( const String& Query );
	void SendMessageToServer( const String& Msg );
	void OnServerReply( int RTT );
	void OnMarginsReply( String Text );

	//Stop Order
	//MarketDataStore Listener
	virtual Classes::TStringList* __fastcall GetSymbols(void);
	virtual void OnMarketDataUpdate(MarketDataMessage* Msg);  //For Recover
	virtual void OnMarketDataUpdate(BasicData* Msg);
	virtual void OnMarketDataUpdate(MatchInfo* Msg);  //For Fill
	virtual void OnMarketDataUpdate(TotalMatch* Msg);
	virtual void OnMarketDataUpdate(DayHighLowPrice* Msg);
	virtual void OnMarketDataUpdate(OpeningInfo* Msg);
	virtual void OnMarketDataUpdate(OrderBookData* Msg);
	virtual void OnMarketDataUpdate(ClosingMarketData* Msg);
	virtual void OnMarketDataUpdate(UnderlyingIndexInfo* Msg);
	virtual void OnMarketDataUpdate(SumOfOrderInfo* Msg);
    ///< Sim Match
	void OnDepthSimMatch( const UFC::AnsiString& Ex, const UFC::AnsiString& Sym, double buyPrice1, int buyQty1, double sellPrice1, int sellQty1 );

	int SubscribeSymbol(const String& ExchangeCode, const String& Symbol, bool SubscribeAnyway);
	int SubscribeSymbol(const String& ExchangeCode, const String& Symbol, bool needIncreaseCount, bool SubscribeAnyway);
	int SubscribeAllPosStatisticRecSymbol(bool needIncreaseCount, bool SubscribeAnyway);
	int UnsubscribeSymbol(const String& ExchangeCode, const String& Symbol, bool UnsubscribeAnyway);

	TExecution* FindStopOrderByTimeNID(UnicodeString TimeNIDKey);
	bool FindStopOrder( const String& StopOrderKey,
						const String& Exchange,
						const String& Symbol,
						double Price,
						nsOrderMessageDefine::SideEnum Side,
						CStringTExecutionMultiMap::iterator& );
	bool FindStopOrder( const String& StopOrderKey,
						nsOrderMessageDefine::SideEnum Side,
						CStringTExecutionMultiMap::iterator& OrdIt );
//	bool InsertStopOrderTimeNIDIndex(TExecution* StopOrderPtr);
//	bool RemoveStopOrderByTimeNID(UnicodeString TimeNIDKey);
	bool RemoveStopOrderTimeNIDIndex(TExecution* StopOrderPtr);
	void WriteStopOrderToFile();
	TExecution* FindStopOrderConditionIndex(const UnicodeString& StopExchangeCode,
											const UnicodeString& StopSymbol,
											double StopPrice,
											const UnicodeString& ExchangeCode,
											const UnicodeString& Symbol,
											double Price,
											nsOrderMessageDefine::SideEnum Side);
	int RemoveStopOrderConditionIndex(const UnicodeString& StopExchangeCode,
									  const UnicodeString& StopSymbol,
									  double StopPrice,
									  const UnicodeString& ExchangeCode,
									  const UnicodeString& Symbol,
									  double Price,
									  nsOrderMessageDefine::SideEnum Side);
	bool RemoveStopOrder( const String& StopExchangeCode,
						  const String& StopSymbol,
						  double StopPrice,
						  const String& ExchangeCode,
						  const String& Symbol,
						  double Price,
						  nsOrderMessageDefine::SideEnum Side);
	void InsertStopOrderConditionIndex(const UnicodeString& StopExchangeCode,
									   const UnicodeString& StopSymbol, double StopPrice,
									   TExecution* StopOrderPtr);
	void ClearStopOrderData();
	bool IsStopOrderExist();
	bool IsStopOrderExist(const UnicodeString& StopExchangeCode, const UnicodeString& StopSymbol);

	//Message
	void AddOrderMessage(TExecution* OrderMessagePtr);
	TExecution* FindOrderMessageByExecID(UnicodeString ExecID);
	bool InsertOrderMessageExecIDIndex(TExecution* OrderMessagePtr, bool NeedCheckExist = false);
	bool InsertFillMessageSymbolIndex(TExecution* FillMessagePtr);

	CStringTExecutionMap* GetOrderRootTimeOrderIDIndex();
	CStringTExecutionMap* GetFilledOrderRootTimeOrderIDIndex();
	CStringTExecutionMap* GetCanceledOrderRootTimeOrderIDIndex();
	CStringTExecutionMap* GetActiveOrderRootTimeOrderIDIndex();
	CStringTExecutionMap* GetStopOrderTimeNIDIndex();

	void GetTaxAndFeeRate(const String& ExchangeCode,
						  const String& Symbol,
						  const String& Account,
						  nsOrderMessageDefine::PositionEffectEnum PositionEffect,
						  double& TaxRate, double& FeeRateByVolume, double& FeeRateByPrice);

	//Position Statistic Record
	int GetPosStatisticRecTradeSymbolIndexCount(s888::ReadWrietLockTypeEnum RWLockType = s888::rwNotLock);
	s888::CPositionStatisticRecord *FindPosStatisticRecByTradeSymbol(const String& ExchangeCode, const String& TradeSymbol, s888::ReadWrietLockTypeEnum RWLockType = s888::rwNotLock);
	bool InsertPosStatisticRecToTradeSymbolIndex(const String& ExchangeCode, const String& TradeSymbol,
												 s888::CPositionStatisticRecord *PosStatisticRecPtr,
												 bool DoCheckExist = s888::constNotCheckExist,
												 s888::ReadWrietLockTypeEnum RWLockType = s888::rwNotLock);
	int ClearPosStatisticRecTradeSymbolIndex(bool DoDelete = s888::constNotDelete, s888::ReadWrietLockTypeEnum RWLockType = s888::rwNotLock);
	int GetPosStatisticRecArrayCount(s888::ReadWrietLockTypeEnum RWLockType = s888::rwNotLock);
	s888::CPositionStatisticRecord *GetPosStatisticRecFromArray(int ArrayIndex, s888::ReadWrietLockTypeEnum RWLockType = s888::rwNotLock);
	bool AppendPosStatisticRecToArray(s888::CPositionStatisticRecord *PosStatisticRecPtr, s888::ReadWrietLockTypeEnum RWLockType = s888::rwNotLock);
	int ClearPosStatisticRecArray(bool DoDelete = s888::constNotDelete, s888::ReadWrietLockTypeEnum RWLockType = s888::rwNotLock);
	int GetPosStatisticRecWaitingQueueCount(s888::ReadWrietLockTypeEnum RWLockType = s888::rwNotLock);
	s888::CPositionStatisticRecord *GetPosStatisticRecFromWaitingQueue(int QueueIndex, s888::ReadWrietLockTypeEnum RWLockType = s888::rwNotLock);
	void PushPosStatisticRecToWaitingQueue(s888::CPositionStatisticRecord *PosStatisticRecPtr, s888::ReadWrietLockTypeEnum RWLockType = s888::rwNotLock);
	s888::CPositionStatisticRecord *GetFirstPosStatisticRecOfWaitingQueue(s888::ReadWrietLockTypeEnum RWLockType = s888::rwNotLock);
	void PopPosStatisticRecFromWaitingQueue(s888::ReadWrietLockTypeEnum RWLockType = s888::rwNotLock);
	int ClearPosStatisticRecWaitingQueue(s888::ReadWrietLockTypeEnum RWLockType = s888::rwNotLock);
	int SetPosStatisticRecFeeObj(s888::CFeeObject *FeeObjPtr);
	s888::CPositionStatisticRecord *FindOrCreatePosStatisticRec(const String& ExchangeCode, const String& Symbol);
	s888::CPositionStatisticRecord *AddPosStatisticRecFillQty(const String& OrderId, const String& ExecId,
															  const String& ExchangeCode, const String& Symbol,
															  nsOrderMessageDefine::MarketEnum Market,
															  nsOrderMessageDefine::SideEnum Side,
															  nsOrderMessageDefine::PositionEffectEnum PositionEffect,
															  double MatchPrice, int MatchQuantity);
	void AddPositionRecFillQty(TExecution *OrderMessagePtr);
	int ClearPosStatisticRecHoldPosition();
	void ClearOrderMessageData();

	//OrderStatusListener

	//OrderQtyListener
	void WorkingQty( const String& Exchange, const String& Symbol, int& BQty, int& SQty );
	void AddOrderQtyListener(nsOrderMessageDefine::MarketEnum Market, String ExchangeCode, String Symbol, TOrderQtyListener* QtyListener);
	void ExecuteOrderQtyListener( nsOrderMessageDefine::MarketEnum Market, String ExchangeCode, String Symbol,
								  nsOrderMessageDefine::SideEnum Side, double Px);
	void ExecuteOrderQtyListenerForStopOrder( nsOrderMessageDefine::MarketEnum Market, String ExchangeCode, String Symbol,
											  double StopPx, nsOrderMessageDefine::SideEnum Side,
											  double Px, int Qty,
											  nsOrderMessageDefine::OrderTypeEnum OrderType);
	void RemoveOrderQtyListener( TOrderQtyListener* QtyListener);
	///< PositionChangeListener
	void AddPositionChangeListener( nsOrderMessageDefine::MarketEnum Market, String ExchangeCode, String Symbol,
									TPositionChangeListener* PositionChangeListener);
	void ExecutePositionChangeListener( nsOrderMessageDefine::MarketEnum Market, String ExchangeCode, String OrderSymbol,
										nsOrderMessageDefine::SideEnum ExeRptSide = nsOrderMessageDefine::sBuy,
										double ExeRptPx = 0.0, int ExeRptQty = 0, int ExeRptTickCount = 0);
	void RemovePositionChangeListener( TPositionChangeListener* PositionChangeListener);
	///< StrategyOrderListener
	bool IsStrategyExists( const UFC::AnsiString& Symbol, SideEnum Side );
	void AddStrategyOrderListener( const UFC::AnsiString& Symbol, SideEnum Side, IStrategyOrderListener* Listener );
	void ExecuteStrategyOrderListener( const UFC::AnsiString& Symbol, SideEnum Side, double LastPx, int LastQty, int LeavesQty );
	void RemoveStrategyOrderListener( const UFC::AnsiString& Symbol, SideEnum Side );
	///< OrderMessageListener
	void AddOrderMessageListener(TOrderMessageListener* MessageListener);
	void ExecuteOrderMessageListener(TExecution* OrderRootPtr);
	void RemoveOrderMessageListener(TOrderMessageListener* MessageListener);

	///< Filled Listener
	void AddFilledListener( const String& Exchange, const String& Symbol, TFilledListener* Listener );
	void ExecuteOnFilled(  const String& Exchange, const String& Symbol, const String& OrderID, double Price, int Qty, nsOrderMessageDefine::SideEnum Side, nsOrderMessageDefine::OrderStatusEnum OrderStatus, int HHMM );
	void RemoveFilledListener( const String& Exchange, const String& Symbol, TFilledListener* Listener);
	void GetFilledList(  const String& Exchange, const String& Symbol, UFC::List<TFilledData>& Filleds );

	void ExecuteRefreshData();
	void ExecuteStopOrderChanged(TExecution* OrderRootPtr, StopOrderStatusEnum StopOrderStatus);

	TFileStream* GetOrderMsgFileStream();
	bool CheckReportExecIDExist(TExecution *NewRptPtr);
	bool ProcessPendingNewMsg(TExecution *NewRptPtr,
							  nsOrderMessageDefine::OrderStatusEnum RptOrderStatus,
							  UFCType::Int64 RptNID, const UnicodeString& RptOrderId,
							  const UnicodeString& LocalMessageTimeStr);
	bool ProcessNewMsg(TExecution *NewRptPtr,
					   nsOrderMessageDefine::OrderStatusEnum RptOrderStatus,
					   UFCType::Int64 RptNID, const UnicodeString& RptOrderId,
					   const UnicodeString& LocalMessageTimeStr);
	TExecution *FindOrderRootByOrderId_NID(const UnicodeString& RptOrderId, UFCType::Int64 RptNID, int OrderRootNID);
	bool ProcessReplacedMsg(TExecution *NewRptPtr,
							nsOrderMessageDefine::OrderStatusEnum RptOrderStatus,
							UFCType::Int64 RptNID, const UnicodeString& RptOrderId,
							const UnicodeString& LocalMessageTimeStr);
	bool ProcessCanceledMsg(TExecution *NewRptPtr,
							nsOrderMessageDefine::OrderStatusEnum RptOrderStatus,
							UFCType::Int64 RptNID, const UnicodeString& RptOrderId,
							const UnicodeString& LocalMessageTimeStr);
	bool ProcessFilledMsg(TExecution *NewRptPtr,
						  nsOrderMessageDefine::OrderStatusEnum RptOrderStatus,
						  UFCType::Int64 RptNID, const UnicodeString& RptOrderId,
						  const UnicodeString& LocalMessageTimeStr);
	bool ProcessRejectedMsg(TExecution *NewRptPtr,
							nsOrderMessageDefine::OrderStatusEnum RptOrderStatus,
							UFCType::Int64 RptNID, const UnicodeString& RptOrderId,
							const UnicodeString& LocalMessageTimeStr);
	bool InsertOrderMessage(TExecution* OrderMessagePtr);
	bool IsFirstTime();
	UnicodeString     GetRecoverStartTime();
	void              SetCAListener( ICACheckListener* Listener );
	ICACheckListener* GetCAListener( void );
	int               GetConnectionID();
	void              ExecuteOnDisconnect();
	int               GetBuyHoldQuantity(String ExchangeCode, String SymbolCode);
	int               GetSellHoldQuantity(String ExchangeCode, String SymbolCode);
	void              ClearBusinessData();
	int               RecoverActiveStrategyReport(const UFC::AnsiString& StrategyName);
	///< for Simulate order
	static int FTestOrderIdSeq;
	static int FTestExecIdSeq;
	wchar_t ConvertNumberToChar(int Value);
	String GenerateNewIDPrefix();
	String GenerateNewOrderID();
	String GenerateNewExecID();
	String GenerateTestOrderTime();
	std::list<TExecutionReportMessage*> FTestExecRptList; ///< Sim order book
	void ProcessTestExecRptForMatch(MatchInfo *MatchMsgPtr);
	bool MatchOnFillChange( TExecutionReportMessage *CurRptPtr,
							int MatchQuantity, double MatchPrice );
	bool MatchOnDepthChange(  TExecutionReportMessage *CurRptPtr,
							  int BuyQuantity1, double BuyPrice1,
							  int SellQuantity1, double SellPrice1 );

	void AppendTestExecRptToList(TExecutionReportMessage *NewRptPtr);
	void AppendTestExecRptToList(TCancelOrderMessage& CancelMsg);
	void AppendTestExecRptToList(TReplaceOrderMessage& ReplaceMsg );

	void AppendTestExecRptToListDirectly(TExecutionReportMessage *RptPtr);
	int  GetCancelCommandCount();
	int  GetCancelReportCount();
	void SetCancelCommandCount(int CancelCommandCount);
	void SetCancelReportCount(int CancelReportCount);
	void increaseCancelCommandCount();
	void increaseCancelReportCount();
	bool AddCancelStatistic(const String& ExchangeCode, const String& Symbol, int CancelQty);
	void RecalculateCommodityCancelStatistic();
	void GetContractCancelStatistic(const String& ExchangeCode, const String& Symbol, int& CancelQty, int& CancelRptCount);
	void GetCommodityCancelStatistic(const String& ExchangeCode, const String& ContractCode, int& CancelQty, int& CancelRptCount);
	bool LotsControl( const UFC::AnsiString& ProdID, double Profit, int BFilled, int SFilled, int WBuy, int WSell  );
	void AutoPositionEffect( TExecutionReportMessage *NewRptPtr, s888::CPositionStatisticRecord *posStatisticRecPtr );
	void SetThroughput( int PerSec ) { FAdapter->SetThroughput( PerSec ); }
__published:
	///< properties
	__property TCMarketDataStore* MarketDataStore = {read = FMarketDataStore, write = FMarketDataStore}; ///< OK
	__property String AppName = {read = FAppName, write = SetAppName}; ///< OK
	__property String IP = {read = FIP, write = SetIP};                ///< OK
	__property String ID = {read = FID, write = SetID};                ///< OK
	__property String Password = {read = FPassword, write = SetPassword};///< OK
	__property String Account  = {read = FAccount, write = SetAccount};  ///< OK
	__property String TWSEAccount  = {read = FTWSEAccount, write = SetTWSEAccount};  ///< OK
	__property String UserName = {read = GetUserName, write = SetUserName};///< OK
	__property String BrokerID = {read = FBrokerID, write = SetBrokerID};  ///< OK
	__property String TWSEBrokerID = {read = FTWSEBrokerID, write = SetTWSEBrokerID};  ///< OK
	__property String ActiveExecutive = {read = FActiveExecutive, write = SetActiveExecutive };///< OK
	__property String IB              = {read = FIBID, write = SetIBID}; ///< OK
	__property APIType UseAPI = { read = FUseAPI, write = FUseAPI };///< OK
	__property String TradingDate = {read = GetTradingDate }; ///< OK
	__property String ClearMemberID = {read = FClearMemberID, write = SetClearMemberID};///< OK
	__property String Exchange = {read = FExchange, write = FExchange};///< OK
	__property String OrderLogPath = {read = FOrderLogPath, write = SetOrderLogPath};///< OK
	__property String OrderLogFileNamePrefix = {read = FOrderLogFileNamePrefix, write = SetOrderLogFileNamePrefix};///< OK
	__property String Version = {read = FVersion, write = FVersion}; ///< OK
	__property String LocalIP = {read = GetLocalIP }; ///< OK
	__property int    Port = {read = FPort, write = SetPort, default = 0};///< OK
	__property int    MarketDataTimerInterval = {read = FMarketDataTimerInterval, write = FMarketDataTimerInterval, default = 250};///< OK
	__property int    RecoverOverlapSecond = {read = FRecoverOverlapSecond, write = FRecoverOverlapSecond, default = 60};///< OK
	__property bool   Channel = { read = FChannel, write = SetChannel };///< OK
	__property bool   IsPropTrade = { read = FIsPropTrade, write = FIsPropTrade };
	__property bool   IsEDS = { read = GetIsEDS, write = SetIsEDS };
	__property int    CancelReportCount = { read = FCancelReportCount };
	__property bool   TryVersion = { read = FIsTry, write = SetIsTry };///< OK
	__property int    MaxLots = { read = FMaxLots, write = FMaxLots };
	///< Events
	__property TNotifyEvent           OnConnect = {read = FOnConnect, write = FOnConnect};
	__property TNotifyEvent           OnDisconnect = {read = FOnDisconnect, write = FOnDisconnect};
	__property TOnLogonOK             OnLogonOK = {read = FOnLogonOK, write = FOnLogonOK};
	__property TOnLogonFailed         OnLogonFailed = {read = FOnLogonFailed, write = FOnLogonFailed};
	__property TOnNewOrder            OnNewOrder = {read = FOnNewOrder, write = FOnNewOrder};
	__property TOnCancelByOrderID     OnCancelByOrderID = {read = FOnCancelByOrderID, write = FOnCancelByOrderID};
	__property TOnCancelBySymbolPrice OnCancelBySymbolPrice = {read = FOnCancelBySymbolPrice, write = FOnCancelBySymbolPrice};
	__property TOnCancelStatistic     OnCancelStatistic = {read = FOnCancelStatistic, write = FOnCancelStatistic};
	__property TOnNewOrderReply       OnNewOrderReply = {read = FOnNewOrderReply, write = FOnNewOrderReply};
	__property TOnFilledReply         OnFilledReply = {read = FOnFilledReply, write = FOnFilledReply};
	__property TNotifyEvent           OnRejectedReply = {read = FOnRejectedReply, write = FOnRejectedReply};
	__property TNotifyEvent           OnOrderStatusChange = {read = FOnOrderStatusChange, write = FOnOrderStatusChange};
	__property TNotifyEvent           OnBeginRecover = {read = FOnBeginRecover, write = FOnBeginRecover};
	__property TNotifyEvent           OnRecoverFinished = {read = FOnRecoverFinished, write = FOnRecoverFinished};
	__property TOnStopOrderChange     OnStopOrderChange = {read = FOnStopOrderChange, write = FOnStopOrderChange};
	__property TOnStopOrderError      OnStopOrderError = {read = FOnStopOrderError, write = FOnStopOrderError};
	__property TOnErrorMessage        OnErrorMessage = {read = FOnErrorMessage, write = FOnErrorMessage};
	__property TOnMarginsQueryReply   OnMarginsQueryReply = { read = FOnMarginsQueryReply, write = FOnMarginsQueryReply };
	__property TOnServerAckEvent      OnServerAck = { read = FOnServerAck, write = FOnServerAck };
	__property TOnFloatingProfit      OnFloatingProfit = { read = FOnFloatingProfit, write = FOnFloatingProfit };
	__property TOnAdminMessage        OnAdminMessage = { read = FOnAdminMessage, write = FOnAdminMessage };
	__property TOnStrategyReport      OnStrategyReport = { read = FOnStrategyReport, write = FOnStrategyReport };
};
//---------------------------------------------------------------------------
#endif
