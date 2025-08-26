#ifndef OrderAdapterH
#define OrderAdapterH
#include <map>
#include <queue>
#include <SysUtils.hpp>
#include "UFC.h"
#include "OrderMessageDefine.h"
#include "TTaifexConnection.h"
#include "TExecutionUnit.h"

//---------------------------------------------------------------------------
class TOrderStore;
class ICACheckListener;
//---------------------------------------------------------------------------
const int NEWS_RTT_ACK_UID    = 999000000;
const int NEWS_SERVER_MSG_UID = 999000001;
const int NEWS_LOGOUT_MSG_UID = 999000002;
//---------------------------------------------------------------------------
class TStrategyReport
{
private:
	UFC::AnsiString FStrategyName;
	UFC::AnsiString FReportFieldStr;
	int             FNID;

public:
	TStrategyReport();
	TStrategyReport(const UFC::AnsiString& StrategyName, const UFC::AnsiString& ReportFieldStr, int NID);

	UFC::AnsiString GetStrategyName() {return FStrategyName;};
	UFC::AnsiString GetReportFieldStr() {return FReportFieldStr;};
	int             GetNID() {return FNID;};

	void SetStrategyName(UFC::AnsiString StrategyName) {FStrategyName = StrategyName;};
	void SetReportFieldStr(UFC::AnsiString ReportFieldStr) {FReportFieldStr = ReportFieldStr;};
	void SetNID(int NID) {FNID = NID;};
};  //TStrategyReport

//---------------------------------------------------------------------------
class TOrderAdapter : public IOrderConnectionEventListener, public ISendMessageListener
{
private:
	UnicodeString      FAppName;
	AnsiString         FBrokerID;
	AnsiString         FTWSEBrokerID;
	UnicodeString      FClearMemberID;
	UnicodeString      FIP;
	int                FPort;
	UnicodeString      FID;
	UnicodeString      FUserName;
	int 			   FCID;
	UnicodeString      FConnectionLogFileName;
	UnicodeString      FLocalIP;
	UFC::AnsiString    FActiveExecutive;
	UFC::AnsiString    FIBID;
	TTaifexConnection* FConnection;
	TOrderStore*       FOrderStore;
	bool               FIsEDS;
	bool               FIsRecovering;
	bool               FIsTry;
	ICACheckListener*  FCAListener;
//	std::map<UFCType::Int64, UFCType::Int64> FQueryNIDIndex;
	UFC::PHashMap<UFCType::Int64, UFCType::Int64> FQueryNIDMap;
	std::queue<TStrategyReport*> FStrategyReportQueue;
	TTimer*						 FStrategyReportTimer;
	UFC::PCriticalSection        FStrategyReportLocker;
private:
	char ToTWSEOrderType( nsOrderMessageDefine::MarketEnum Market, nsOrderMessageDefine::SideEnum Side, char TSEOrdTp );
	char ToOrderKind( nsOrderMessageDefine::MarketEnum Market, nsOrderMessageDefine::PositionEffectEnum PE, char TSEOrdTp );
	UFCType::Int64 FindOrderRootNIDByQueryNID( UFCType::Int64 QueryNID);
	bool InsertQueryNIDIndex( UFCType::Int64 QueryNID, UFCType::Int64 OrderRootNID);
	bool RemoveQueryNIDIndex( UFCType::Int64 QueryNID );
	void GetUserData( nsOrderMessageDefine::MarketEnum Market,  const UFC::AnsiString& Ex, UFC::AnsiString& UserData, UFC::AnsiString StrategyName, nsOrderMessageDefine::PositionEffectEnum PE, char TSEOrdTp = '0' );
	void ToHexIP( const UFC::AnsiString& IP, UFC::AnsiString& HexIP );
	void __fastcall OnStrategyReportTimer(TObject *Sender);
	void  __fastcall SetActiveExecutive( const String& AE );
	void  __fastcall SetIBID( const String& IB );
	DWORD GetAPVersion( void );
	String UTF8String( const char* CStr );
private:///< Implement ISendMessageListener
	virtual void OnBeforeSend( nsOrderMessageDefine::MarketEnum Market,
							   SendMessageType SType,
							   TBaseMessage* Msg,
							   bool& CanSend,
							   UFC::AnsiString& RejectMsg );
private:
	void GetLocalSystemTime( UnicodeString& LocalSystemTimeStr, bool UseDelimiter = true);
	TExecution *FindOrderRootByOrderId_NID(const UnicodeString& RptOrderId, UFCType::Int64 RptNID);
	void ProcessStrategyOrder(TExecutionReportMessage *RecvMsg, TExecution *OrderRootPtr);
	void ProcessPendingNewMsg( nsOrderMessageDefine::OrderStatusEnum MsgOrderStatus, TExecutionReportMessage *RecvMsg, TExecution *NewReportPtr);
	void ProcessNewMsg( nsOrderMessageDefine::OrderStatusEnum MsgOrderStatus, TExecutionReportMessage *RecvMsg, TExecution *NewReportPtr );
	void ProcessFilledMsg( nsOrderMessageDefine::OrderStatusEnum MsgOrderStatus, TExecutionReportMessage *RecvMsg, TExecution *NewReportPtr );
	void ProcessReplacedMsg( nsOrderMessageDefine::OrderStatusEnum MsgOrderStatus, TExecutionReportMessage *RecvMsg, TExecution *NewReportPtr );
	void ProcessCanceledMsg( nsOrderMessageDefine::OrderStatusEnum MsgOrderStatus,  TExecutionReportMessage *RecvMsg, TExecution *NewReportPtr );
	void ProcessRejectedMsg( nsOrderMessageDefine::OrderStatusEnum MsgOrderStatus, TExecutionReportMessage *RecvMsg, TExecution *NewReportPtr);
public:///< Implement IOrderConnectionEventListener
	virtual void OnConnected(void);
	virtual void OnDisconnected(void);
	virtual void OnLogonReply(const char* ReplyString, LogonResult Result, int CID);
	virtual void OnExecutionReport(TExecutionReportMessage* Msg, ExecDup PossDup);
	virtual void OnNews(TNewsMessage* Msg);
	virtual void OnRecoverFinished(int Count);
public:
	TOrderAdapter(TOrderStore* OrderStore,
				  const UnicodeString& AppName,
				  const UnicodeString& BrokerID,
				  const UnicodeString& ClearMemberID,
				  const UnicodeString& ConnectionLogFileName,
				  APIType WhichAPI );
	~TOrderAdapter();
	///< Get set properties
	void __fastcall SetUserName( const UnicodeString& NewName ) { FUserName = NewName; }
	void __fastcall SetBrokerID( const UnicodeString& BrokerID);
	void __fastcall SetTWSEBrokerID( const UnicodeString& BrokerID);
	void __fastcall SetClearMemberID( const UnicodeString& ClearMemberID);
	void __fastcall SetPort(int Port) {FPort = Port;};
	void __fastcall SetIsTry( bool IsTry );
	void __fastcall SetCAListener( ICACheckListener* Listener ) { FCAListener = Listener; }
	void __fastcall SetThroughput( int PerSec ) { FConnection->SetThroughput( PerSec ); }
	const String& __fastcall GetLocalIP( void );
	const String& __fastcall GetUserName( void ) {return FUserName; }
	const char* __fastcall GetBrokerID( nsOrderMessageDefine::MarketEnum Market );
	const char* __fastcall GetTradingDate( void );
	int  __fastcall GetConnectionID() {return FCID;};
	ICACheckListener* __fastcall GetCAListener( void ) { return FCAListener; }
	///< Functions
	void __fastcall Connect(const UnicodeString& IP, int Port);
	void __fastcall Disconnect();
	bool __fastcall IsRecovering();
	bool __fastcall IsConnected();
	bool __fastcall IsLogon();
	void __fastcall LogonProxy(const String& ID,  const UnicodeString& Accounts  );
	void __fastcall LogonPropTrade( const String& ID, const String& Password, const String& Account );
	void __fastcall Logoff();
	void __fastcall DoNetRecover();
	bool __fastcall IsConnectionReady( void );
	void __fastcall QueryMargins( const String& Query );
	void __fastcall SendMessageToServer( const String& Msg );
	///< Strategy Report functions
	void LockStrategyReport() {FStrategyReportLocker.Acquire();};
	void UnlockStrategyReport() {FStrategyReportLocker.Release();};
	void EnableStrategyReportTimer() {FStrategyReportTimer->Enabled = true;};
	void DisableStrategyReportTimer() {FStrategyReportTimer->Enabled = false;};
	void PushStrategyReport(TStrategyReport *StrategyReportPtr) {FStrategyReportQueue.push(StrategyReportPtr);};
private:
	TExecution *CreateOrderRootForNewOrder(nsOrderMessageDefine::MarketEnum Market,
										   const String& Exchange,
										   const String& Symbol,
										   nsOrderMessageDefine::SideEnum Side,
										   double Px,
										   int Qty,
										   nsOrderMessageDefine::OrderTypeEnum OrderType,
										   nsOrderMessageDefine::TimeInForceEnum TimeInForce,
										   nsOrderMessageDefine::PositionEffectEnum PositionEffect,
										   const String& Account,
										   double StopPx,
										   int TickCount,
										   const UFC::AnsiString& StrategyName,
										   UFC::AnsiString& AnsiTradeSymbol,
										   UFC::AnsiString& AnsiMaturityMonthYear,
										   char OrderKind );
	TExecution* CreateRootNewOrder( TNewOrderMessage& NewMsg, int TickCount, const UFC::AnsiString& StrategyName );
	UFCType::Int64 SendNewOrderToSpeedy(TNewOrderMessage& NewMsg);
	///< for CancelOrder
	bool GetMarketId(const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, nsOrderMessageDefine::MarketEnum& MarketId);
public:
	UFCType::Int64 UniqueNewNID( void );
	UFCType::Int64 GenerateNID( nsOrderMessageDefine::MessageTypeEnum MT = nsOrderMessageDefine::mtNew );
	UFCType::Int64 NewSingleOrder(nsOrderMessageDefine::MarketEnum Market,
								 const String& Exchange,
								 const String& Symbol,
								 nsOrderMessageDefine::SideEnum Side,
								 double Px,
								 int Qty,
								 nsOrderMessageDefine::OrderTypeEnum OrderType,  ///< Limit/Market
								 nsOrderMessageDefine::TimeInForceEnum TimeInForce, ///< FOK,IOC,ROD
								 nsOrderMessageDefine::PositionEffectEnum PositionEffect, ///< Open,Close,Auto,DayTrade
								 nsOrderMessageDefine::EventTypeEnum EventType, ///< Call/Put
								 double StrikePx,
								 const String& Account,
								 const String& AE,
								 const String& IB,
								 int TickCount, // 0   for Ping Pong
								 double StopPx, // 0.0
								 UFC::AnsiString StrategyName, // "     "
								 char TWSEOrdType ); ///< TWSE OrderType '0': Normal '3': Loan capital '4': Loan share  for Mega 'A': Day Trade Normal '9': Day Trade Loan
	UFCType::Int64 CancelOrder( nsOrderMessageDefine::MarketEnum Market,
								const String& Exchange,
								const String& OrderID,
								const String& AE,
								const String& IB,
								UFC::AnsiString StrategyName = "" );
	void ReplaceOrderPrice( nsOrderMessageDefine::MarketEnum Market,
							const String& Exchange,
							const String& OrderID,
							nsOrderMessageDefine::OrderTypeEnum OrderType,
							nsOrderMessageDefine::TimeInForceEnum TimeInForce,
							double NewPrice);
	void AmendOrder( nsOrderMessageDefine::MarketEnum Market,
					 const String& Exchange,
					 const String& OrderID,
					 int Qty);
	void RequestOrderStatus( UFCType::Int64 OrderNID,
							 nsOrderMessageDefine::MarketEnum Market,
							 const String& Exchange,
							 const String& OrderID,
							 const String& Symbol,
							 nsOrderMessageDefine::SideEnum Side,
							 double Px );
public: /// for Name/Value Order string.
	UFCType::Int64 NewOrder( nsOrderMessageDefine::MarketEnum Market,
							 UFCType::Int64 NewNID,
							 UFC::NameValueMessage& FieldData,
							 const UFC::AnsiString& ansiStrategyName,
							 const UFC::AnsiString& ansiAccount,
							 const String& AE,
							 const String& IB,
							 int TickCount = 0 );
	UFCType::Int64 CancelOrder( const UFC::AnsiString& FieldValueStr,
								const UFC::AnsiString& StrategyName,
								const String& AE,
								const String& IB);
	void RejectStrategyRequestDirectly(int RejectResponse, int StatusCode, const UFC::AnsiString& RejectMsg,
									   UFCType::Int64 NID, const UFC::AnsiString& StrategyName);
public:
	__property String ActiveExecutive = {write = SetActiveExecutive };
	__property String IB              = {write = SetIBID};
	__property bool   IsEDS           = {read = FIsEDS, write = FIsEDS };
};
//---------------------------------------------------------------------------
#endif
