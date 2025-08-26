//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "OrderAdapter.h"
#include "MessageDataFormat.h"
#include "TNewOrderMessage.h"
#include "TCancelOrderMessage.h"
#include "TReplaceOrderMessage.h"
#include "TExecutionUnit.h"
#include "TradingObjects.h"
#include "TradingCommodities.h"
#include "TradingObjectPool.h"
#include "OrderStore.h"
#include <WideStrUtils.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//----------------------------- TStrategyReport -----------------------------
//---------------------------------------------------------------------------
TStrategyReport::TStrategyReport()
:FStrategyName(""),
 FReportFieldStr(""),
 FNID(0)
{
}  //TStrategyReport::TStrategyReport()
//---------------------------------------------------------------------------
TStrategyReport::TStrategyReport(const UFC::AnsiString& StrategyName, const UFC::AnsiString& ReportFieldStr, int NID)
:FStrategyName(StrategyName),
 FReportFieldStr(ReportFieldStr),
 FNID(NID)
{
}  //TStrategyReport::TStrategyReport()

//------------------------------ TOrderAdapter ------------------------------
//---------------------------------------------------------------------------
TOrderAdapter::TOrderAdapter(TOrderStore* OrderStore,
							 const UnicodeString& AppName,
							 const UnicodeString& BrokerID,
							 const UnicodeString& ClearMemberID,
							 const UnicodeString& ConnectionLogFileName,
							 APIType WhichAPI )
:FOrderStore(OrderStore),
 FAppName(AppName),
 FBrokerID(BrokerID),
 FClearMemberID(ClearMemberID),
 FConnectionLogFileName(ConnectionLogFileName),
 FIsEDS( false ),
 FCID( 0 ),
 FIsRecovering( false ),
 FIsTry( true )
{
	AnsiString AnsiAppName( FAppName );
	AnsiString AnsiClearMemberID( FClearMemberID );
	AnsiString AnsiLogName( FConnectionLogFileName );

	FConnection = new TTaifexConnection( AnsiAppName.c_str(), this, WhichAPI, true);
	FConnection->SetUseNewFuturesSymbol( true );
	FConnection->SetLanguage( mlChinese );
	FConnection->AddSellSide( ssForeignExchange );
	FConnection->SetCMID( AnsiClearMemberID.c_str() );
	FConnection->SetEnableFilledState( true );
	FConnection->SetEnablePendingNewAck( false );
	FConnection->SetBeforeSendListener( this );
	FConnection->SetLogFile( AnsiLogName.c_str() );
	FConnection->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020,IDR_OPT_C030, IDR_OPT_C030M,IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI");
	FConnection->SetTSEDataFormatResourceID( IDR_TSE_T010,IDR_TSE_O010,IDR_TSE_P010, IDR_TSE_T020,IDR_TSE_O020, IDR_TSE_P020, IDR_TSE_R030, "INI" );
	FConnection->SetTSEDataFormatResourceID2( IDR_TSE_A010, IDR_TSE_V010,IDR_TSE_E010,IDR_TSE_Ex010,IDR_TSE_A020, IDR_TSE_V020,IDR_TSE_E020,IDR_TSE_Ex020, "INI" );
	FConnection->SetOTCDataFormatResourceID( IDR_OTC_T010,IDR_OTC_O010,IDR_OTC_P010, IDR_OTC_T020,IDR_OTC_O020, IDR_OTC_P020, IDR_OTC_R030, "INI" );
	FConnection->SetOTCDataFormatResourceID2( IDR_OTC_V010, IDR_OTC_E010,IDR_OTC_Ex010, IDR_OTC_V020, IDR_OTC_E020, IDR_OTC_Ex020, "INI" );

	///< Strategy Report Timer
	FStrategyReportTimer = new TTimer( 0 );
	FStrategyReportTimer->Enabled  = false;
	FStrategyReportTimer->Interval = 100;
	FStrategyReportTimer->OnTimer  = OnStrategyReportTimer;
}
//---------------------------------------------------------------------------
TOrderAdapter::~TOrderAdapter()
{
	delete FStrategyReportTimer;
	delete FConnection;
}
//------------------------------------------------------------------------------
DWORD TOrderAdapter::GetAPVersion( void )
{
	DWORD Version,n;
	AnsiString CheckFile( Application->ExeName );

	if( FileExists( CheckFile ) == false )
		return 1000000;///< 1.00.00.00

	if( (n = GetFileVersionInfoSizeA( CheckFile.c_str(), NULL)) > 0 ) ///< File contains version information.
	{
		char *pBuf = new char[ n ];
		VS_FIXEDFILEINFO *vsinfo;
		UINT Len;

		GetFileVersionInfoA( CheckFile.c_str(), 0, n, pBuf);
		if( VerQueryValueA( pBuf, "\\", (void **)&vsinfo, &Len))
		{
			AnsiString VersionString;
			int MajorVersion = HIWORD(vsinfo->dwFileVersionMS)%100;
			int MinorVersion = LOWORD(vsinfo->dwFileVersionMS)%100;
			int Release      = HIWORD(vsinfo->dwFileVersionLS)%100;
			int Build        = LOWORD(vsinfo->dwFileVersionLS)%100;
			if( FIsTry == false )
				MajorVersion += 2;
			Version = MajorVersion*1000000 + MinorVersion*10000 + Release*100 + Build;
		}
		delete [] pBuf;
	}
	return Version;
}
//---------------------------------------------------------------------------
void __fastcall TOrderAdapter::Connect(const UnicodeString& IP, int Port)
{
	AnsiString AnsiIP = IP;

	FIP = IP;
	FPort = Port;
	FConnection->Connect( AnsiIP.c_str(), FPort, 10);
}
//---------------------------------------------------------------------------
void __fastcall TOrderAdapter::Disconnect()
{
	if( FConnection->IsLogon( ) ) FConnection->Logoff();
	if( FConnection->IsConnected( ) ) FConnection->Disconnect();
}
//---------------------------------------------------------------------------
void __fastcall TOrderAdapter::LogonPropTrade( const UnicodeString& ID, const UnicodeString& Password, const UnicodeString& Account )
{
	AnsiString tempID      = ID;
	AnsiString tempPasswd  = Password;
	AnsiString tempAccount = Account;

	FID       = ID;
	FConnection->Logon( tempID.c_str(), tempPasswd.c_str(), tempAccount.c_str(), ctBoth, 30150 );
}
//---------------------------------------------------------------------------
void __fastcall TOrderAdapter::LogonProxy( const UnicodeString& ID, const UnicodeString& Accounts  )
{
	AnsiString tempID    = ID;
	AnsiString tempAccs  = Accounts;
	DWORD      Version = GetAPVersion( );

	FID       = ID;
	if( TOrderStore::IsTestMode() == true )
		FConnection->SpeedyProxyLogon( tempID.c_str(), "Password", tempAccs.c_str(), "SpeedyProxy", "601008", ctRecvExecution, Version );
	else
		FConnection->SpeedyProxyLogon( tempID.c_str(), "Password", tempAccs.c_str(), "SpeedyProxy", "601008", ctBoth, Version );
}
//---------------------------------------------------------------------------
void __fastcall TOrderAdapter::Logoff()
{
	FConnection->Logoff();
}
//---------------------------------------------------------------------------
void __fastcall TOrderAdapter::DoNetRecover()
{
	if( FOrderStore != NULL ) ///< Try to recover
	{
		UFC::AnsiString ansiRecoverStartTime = s888::ConvertUnicodeToUFCAnsiStr(FOrderStore->GetRecoverStartTime());
		UFC::AnsiString ansiHourStr, ansiMinuteStr, ansiSecondStr;

		if( ansiRecoverStartTime.AnsiPos(":") >= 0 )///< Format: HH:MM:SS
		{
			ansiHourStr   = ansiRecoverStartTime.SubString(0, 2);
			ansiMinuteStr = ansiRecoverStartTime.SubString(3, 2);
			ansiSecondStr = ansiRecoverStartTime.SubString(6, 2);
		}
		else  ///< Format: HHMMSS
		{
			ansiHourStr   = ansiRecoverStartTime.SubString(0, 2);
			ansiMinuteStr = ansiRecoverStartTime.SubString(2, 2);
			ansiSecondStr = ansiRecoverStartTime.SubString(4, 2);
		}

		UFC::AnsiString ansiRecoverBeginTimeStr = ansiHourStr + ansiMinuteStr + ansiSecondStr;
		UFC::AnsiString ansiMinRecoverBeginTimeStr;

		ansiMinRecoverBeginTimeStr.Printf("%06d", FOrderStore->RecoverOverlapSecond);
		if( ansiMinRecoverBeginTimeStr < ansiRecoverBeginTimeStr )
		{
			UFC::UDateTime recoverTime(ansiHourStr.ToInt(), ansiMinuteStr.ToInt(), ansiSecondStr.ToInt(), 0);

			recoverTime -= FOrderStore->RecoverOverlapSecond;
			ansiRecoverBeginTimeStr.Printf( "%02d%02d%02d", recoverTime.getHour(), recoverTime.getMinute(), recoverTime.getSecond());
		}
		else
			ansiRecoverBeginTimeStr = "060000";
		if( (FOrderStore != NULL ) && (FOrderStore->OnBeginRecover != NULL ))
			FOrderStore->OnBeginRecover( FOrderStore );
		UFC::BufferedLog::Printf(" OrderAdapter Recover Begin Time [%s](%s:%s:%s)", ansiRecoverBeginTimeStr.c_str(), ansiHourStr.c_str(), ansiMinuteStr.c_str(), ansiSecondStr.c_str());
		FIsRecovering = true;
		FConnection->Recover(ansiRecoverBeginTimeStr.c_str(), rdBoth, rmAll);
	}
}
//---------------------------------------------------------------------------
String TOrderAdapter::UTF8String( const char* CStr )
{
	if( IsUTF8String( CStr ) == true )
		return UTF8ToString( CStr );
	return String( CStr );
}
//---------------------------------------------------------------------------
UFCType::Int64 TOrderAdapter::FindOrderRootNIDByQueryNID( UFCType::Int64 QueryNID )
{
	UFCType::Int64 OrderRootNID = 0L;

/*	if( QueryNID > 0L )
	{
		std::map<UFCType::Int64, UFCType::Int64>::iterator it = FQueryNIDIndex.find( QueryNID );
		if( it != FQueryNIDIndex.end() )
			OrderRootNID = it->second;
		return OrderRootNID;
	}
	return 0L;*/

	if( FQueryNIDMap.GetObjectByKey( QueryNID, OrderRootNID ) == true )
		return OrderRootNID;
	return 0L;
}
//---------------------------------------------------------------------------
bool TOrderAdapter::InsertQueryNIDIndex( UFCType::Int64 QueryNID, UFCType::Int64 OrderRootNID )
{
	if( (QueryNID > 0L) && (OrderRootNID > 0L) )
	{
/*		if( FindOrderRootNIDByQueryNID(QueryNID) == 0L ) ///< Query NID Not found
		{
			FQueryNIDIndex.insert(std::map<UFCType::Int64, UFCType::Int64>::value_type(QueryNID, OrderRootNID));
			return true;
		}*/
		if( FQueryNIDMap.IsExists( QueryNID ) == false )
		{
			FQueryNIDMap.Add( QueryNID, OrderRootNID );
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bool TOrderAdapter::RemoveQueryNIDIndex( UFCType::Int64 QueryNID)
{
	if( QueryNID > 0L )
	{
/*		std::map<UFCType::Int64, UFCType::Int64>::iterator it = FQueryNIDIndex.find( QueryNID );
		if( it != FQueryNIDIndex.end() )
		{
			FQueryNIDIndex.erase( it );
			return true;
		}*/
		if( FQueryNIDMap.DeleteByKey( QueryNID ) >= 0 )
			return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void TOrderAdapter::GetLocalSystemTime( UnicodeString& LocalSystemTimeStr, bool UseDelimiter)
{
	SYSTEMTIME SysTime;

	GetLocalTime( &SysTime );
	if (UseDelimiter)
		LocalSystemTimeStr.printf(L"%02d:%02d:%02d.%03d", SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds);
	else
		LocalSystemTimeStr.printf(L"%02d%02d%02d%03d", SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds);
}
//---------------------------------------------------------------------------
void TOrderAdapter::ProcessStrategyOrder(TExecutionReportMessage *RecvMsg, TExecution *OrderRootPtr)
{
	if( FOrderStore->OnStrategyReport != NULL )
	{
		UFC::AnsiString  AnsiStrategyName;

		if( OrderRootPtr->GetStrategyName( AnsiStrategyName, RecvMsg ) == true )
		{
			String NameValueStr = OrderRootPtr->EncodeNameValueString( RecvMsg );
			char   NameValueBuffer[ 1024 ];

			int BufferLength = UnicodeToUtf8( NameValueBuffer, 1000, NameValueStr.c_str(), NameValueStr.Length() );
			NameValueBuffer[ BufferLength ] = 0;
			UFC::AnsiString AnsiNameValueStr = NameValueBuffer;

			FOrderStore->OnStrategyReport( AnsiNameValueStr, AnsiStrategyName, OrderRootPtr->GetNID());
		}
	}
}
//---------------------------------------------------------------------------
bool __fastcall TOrderAdapter::IsConnectionReady( void )
{
	if (FConnection == NULL ) throw OSConnectionNotExistException();
	if (!FConnection->IsConnected()) throw OSConnectionNotConnectedException();
	if (!FConnection->IsLogon()) throw OSConnectionNotLogonException();
	return true;
}
//---------------------------------------------------------------------------
void TOrderAdapter::OnBeforeSend(nsOrderMessageDefine::MarketEnum Market,
								 SendMessageType SType,
								 TBaseMessage* Msg,
								 bool& CanSend,
								 UFC::AnsiString& RejectMsg)
{
	if( FCAListener != NULL )
		FCAListener->OnCACheck( Market, SType, Msg, CanSend, RejectMsg );
}
//---------------------------------------------------------------------------
void  __fastcall TOrderAdapter::QueryMargins(  const String& Query )
{
	if( IsConnectionReady( ) == true )
	{
		TNewsMessage    QueryMessage;
		UFC::AnsiString QueryStr( Query.c_str() );

		QueryMessage.SetHeadline( "32" );
		QueryMessage.SetText( QueryStr );
		FConnection->SendNewsRequest( &QueryMessage );
	}
}
//---------------------------------------------------------------------------
void  __fastcall TOrderAdapter::SendMessageToServer( const String& Msg )
{
	if( IsConnectionReady( ) == true )
	{
		TNewsMessage    Message;
		UFC::AnsiString MsgStr( Msg.c_str() );
		long            Tick = UFC::GetTickCountMS()%100000;

		Message.SetID( 999000000 );///< ID for Ack
		Message.SetHeadline( MsgStr );
		Message.SetText( "Ack" );
		Message.SetMsgNum( Tick );
		FConnection->SendNewsRequest( &Message );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderAdapter::SetIsTry( bool IsTry )
{
	FIsTry = IsTry;
}
//---------------------------------------------------------------------------
void __fastcall TOrderAdapter::SetBrokerID( const UnicodeString& BrokerID)
{
	FBrokerID = BrokerID;
}
//---------------------------------------------------------------------------
void __fastcall TOrderAdapter::SetTWSEBrokerID( const UnicodeString& BrokerID)
{
	FTWSEBrokerID = BrokerID;
}
//---------------------------------------------------------------------------
const char* __fastcall TOrderAdapter::GetBrokerID( nsOrderMessageDefine::MarketEnum Market )
{
	if( Market == nsOrderMessageDefine::mTSE || Market == nsOrderMessageDefine::mOTC)
		return  FTWSEBrokerID.c_str();
	return  FBrokerID.c_str();
}
//---------------------------------------------------------------------------
void __fastcall TOrderAdapter::SetClearMemberID( const UnicodeString& ClearMemberID)
{
	FClearMemberID = ClearMemberID;
	if( FConnection != NULL )
	{
		AnsiString tempClearMemberID = FClearMemberID;

		FConnection->SetCMID( tempClearMemberID.c_str());
	}
}
//---------------------------------------------------------------------------
void  __fastcall TOrderAdapter::SetActiveExecutive( const String& AE )
{
	AnsiString AnsiAE( AE );
	FActiveExecutive = AnsiAE.c_str();
}
//---------------------------------------------------------------------------
void  __fastcall TOrderAdapter::SetIBID( const String& IB )
{
	AnsiString AnsiIB( IB );
	FIBID = AnsiIB.c_str();
}
//---------------------------------------------------------------------------
const String& __fastcall TOrderAdapter::GetLocalIP( void )
{
	FLocalIP = FConnection->GetLocalIP().c_str();
	return  FLocalIP;
}
//---------------------------------------------------------------------------
bool __fastcall TOrderAdapter::IsRecovering( void )
{
	return FIsRecovering;
}
//---------------------------------------------------------------------------
bool __fastcall TOrderAdapter::IsConnected( void )
{
	return FConnection->IsConnected();
}
//---------------------------------------------------------------------------
bool __fastcall TOrderAdapter::IsLogon( void )
{
	return FConnection->IsLogon();
}
//---------------------------------------------------------------------------
const char* __fastcall TOrderAdapter::GetTradingDate( void )
{
	return FConnection->GetTradingDate();
}
//---------------------------------------------------------------------------
void __fastcall TOrderAdapter::OnStrategyReportTimer(TObject *Sender)
{
	FStrategyReportLocker.Acquire();
	FStrategyReportTimer->Enabled = false;
	while(FStrategyReportQueue.size() > 0)
	{
		TStrategyReport *CurRptPtr = FStrategyReportQueue.front();
		FStrategyReportQueue.pop();
		UFC::AnsiString StrategyName = CurRptPtr->GetStrategyName();
		UFC::AnsiString ReportFieldStr = CurRptPtr->GetReportFieldStr();
		int             NID = CurRptPtr->GetNID();
		delete CurRptPtr;
		FOrderStore->OnStrategyReport(ReportFieldStr, StrategyName, NID);
	}
	FStrategyReportLocker.Release();
}
//---------------------------------------------------------------------------
//
//  Implement interface IOrderConnectionEventListener
//  to handle Object TTaifexConnection events.
//
//---------------------------------------------------------------------------
void TOrderAdapter::OnConnected(void)
{
	if ((FOrderStore != 0) && (FOrderStore->OnConnect != 0))
		FOrderStore->OnConnect(FOrderStore);
}
//---------------------------------------------------------------------------
void TOrderAdapter::OnDisconnected()
{
	if (FOrderStore != 0)
		FOrderStore->ExecuteOnDisconnect();
}
//---------------------------------------------------------------------------
void TOrderAdapter::OnLogonReply(const char* ReplyString, LogonResult Result, int CID)
{
	String Msg = UTF8String( ReplyString );
	if( Result == lrOk )
	{
		FUserName = UTF8String( FConnection->GetUserName().c_str() ); ///< Login ID
		FCID      = CID; ///< Connection ID
		if( FOrderStore != NULL )
		{
			FOrderStore->RecoverOrderLogFileData();  ///< Recover Orders from log
			if( FOrderStore->OnLogonOK != NULL )
				FOrderStore->OnLogonOK( FOrderStore, Msg, FCID);
		}
	}
	else
	{
		FCID = 0;
		if( FOrderStore != NULL && FOrderStore->OnLogonFailed != NULL )
			FOrderStore->OnLogonFailed( FOrderStore, Msg, FCID );
		return;
	}
	DoNetRecover();
}
//---------------------------------------------------------------------------
void TOrderAdapter::OnExecutionReport(TExecutionReportMessage* Msg, ExecDup PossDup)
{
	if( FOrderStore !=  NULL )
	{
		TExecution*                           NewMsgPtr = new TExecution( Msg );

		String                                RptExecId( NewMsgPtr->GetExecID());
		String                                TransactTime( NewMsgPtr->GetTransactTime());
		nsOrderMessageDefine::OrderStatusEnum MsgOrderStatus = NewMsgPtr->GetOrderStatus();
		nsOrderMessageDefine::ExecTypeEnum    RptExecType = NewMsgPtr->GetExecType();
		UFCType::Int64                        RptNID = NewMsgPtr->GetNID();
		String                                RptOrderId( NewMsgPtr->GetOrderID());
		String                                LocalMessageTimeStr;

		///< Fill TransactTime
		GetLocalSystemTime( LocalMessageTimeStr, true );
		if( TransactTime.Length() <= 0 ) ///< Set local time as TransactTime
			NewMsgPtr->SetTransactTime( LocalMessageTimeStr );
		///< OrderStatus execution report.
		if ((RptExecType != nsOrderMessageDefine::etOrderStatus) &&
			(RptExecType != nsOrderMessageDefine::etCanceled) &&
			(RptExecType != nsOrderMessageDefine::etReplaced) &&
			(FOrderStore->FindOrderMessageByExecID( RptExecId ) != NULL ))
		{
			if ((!FIsRecovering) && (FOrderStore->OnErrorMessage != 0))
			{
				String     tradeSymbol( Msg->GetSymbol() );
				SYSTEMTIME SysTime;
				GetLocalTime( &SysTime );
				FOrderStore->OnErrorMessage( FOrderStore, SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds,
											 osErrMessageExist, MsgOrderStatus,
											 RptOrderId, RptNID, tradeSymbol, NewMsgPtr->GetSide(),
											 NewMsgPtr->GetPrice(), NewMsgPtr->GetOrderQty());
			}
			delete NewMsgPtr;
			return;
		}

		switch( MsgOrderStatus )
		{
			case nsOrderMessageDefine::osPendingNew:      ProcessPendingNewMsg( MsgOrderStatus, Msg, NewMsgPtr );return;
			case nsOrderMessageDefine::osNew:             ProcessNewMsg(MsgOrderStatus, Msg, NewMsgPtr); return;
			case nsOrderMessageDefine::osPartiallyFilled:
			case nsOrderMessageDefine::osFilled: 	      ProcessFilledMsg(MsgOrderStatus, Msg, NewMsgPtr); return;
			case nsOrderMessageDefine::osReplaced:		  ProcessReplacedMsg(MsgOrderStatus, Msg, NewMsgPtr );return;
			case nsOrderMessageDefine::osCanceled:        ProcessCanceledMsg(MsgOrderStatus, Msg, NewMsgPtr );return;
			case nsOrderMessageDefine::osRejected: 	      ProcessRejectedMsg(MsgOrderStatus, Msg, NewMsgPtr );return;
			case nsOrderMessageDefine::osPendingReplace:
			case nsOrderMessageDefine::osPendingCancel:
			default: return;
		}
	}
	else
		UFC::BufferedLog::Printf(" OnExecutionReport Error! OrderStore = NULL");
}
//---------------------------------------------------------------------------
void TOrderAdapter::OnRecoverFinished(int Count)
{
	UFC::BufferedLog::Printf(" Order Adapter Recover Finished. %d Message Recovered", Count);
	FOrderStore->DeleteRejectedOrderRoot(5);
	FOrderStore->ExecuteRefreshData();
	FIsRecovering = false;
	FOrderStore->QueryPendingNewOrderRoot();
	if (FOrderStore->OnRecoverFinished != 0)
		FOrderStore->OnRecoverFinished(FOrderStore);
}
//---------------------------------------------------------------------------
void TOrderAdapter::OnNews( TNewsMessage* Msg )
{
	long   ID = Msg->GetID();

	if( ID >= NEWS_RTT_ACK_UID ) ///< Server Ack
	{
		if( ID == NEWS_RTT_ACK_UID )
		{
			long SendTick = Msg->GetMsgNum();
			int  RTT      = UFC::GetTickCountMS()%100000 - SendTick;
			FOrderStore->OnServerReply( RTT );
		}
		else if( ID == NEWS_SERVER_MSG_UID )
		{
			String MessageStr = UTF8ToUnicodeString( Msg->GetText() );
			if( FOrderStore->OnAdminMessage != NULL )
				FOrderStore->OnAdminMessage( FOrderStore, MessageStr, false );
		}
		else if( ID == NEWS_LOGOUT_MSG_UID )
		{
			String MessageStr = UTF8ToUnicodeString( Msg->GetText() );
			if( FOrderStore->OnAdminMessage != NULL )
				FOrderStore->OnAdminMessage( FOrderStore, MessageStr, true );
		}
	}
	else ///< Query Margins
	{
		String Text = Msg->GetText();
		FOrderStore->OnMarginsReply( Text );
	}
}
//---------------------------------------------------------------------------
//
// Functions for User Define
//
//---------------------------------------------------------------------------
void TOrderAdapter::ToHexIP( const UFC::AnsiString& IP, UFC::AnsiString& HexIP )
{
	UFC::PStringList Nums;

	Nums.SetStrings( IP, "./n" );
	if( Nums.ItemCount() == 4 )
		HexIP.Printf( "%02X%02X%02X%02X",Nums[0].ToInt(),Nums[1].ToInt(),Nums[2].ToInt(),Nums[3].ToInt() );
	else
		HexIP = "7F000000";
}
//---------------------------------------------------------------------------
char TOrderAdapter::ToTWSEOrderType( nsOrderMessageDefine::MarketEnum Market, nsOrderMessageDefine::SideEnum Side, char TSEOrdTp )
{
	switch( Market )
	{
		case nsOrderMessageDefine::mTWFutures:
		case nsOrderMessageDefine::mTWOptions: return '0';
		case nsOrderMessageDefine::mTSE:
		case nsOrderMessageDefine::mOTC:       switch( TSEOrdTp )
											   {
													case '0': return '0';  ///< Normal = 0
													case '3': return '3';  ///< Trade Margin = 3
													case '4': return '4';  ///< Sell short = 4
													case '9': if( Side == nsOrderMessageDefine::sBuy )
																  return '3'; ///< Trade Margin + Day Trade = 9
															  else
																  return '4'; ///< Sell short  + Day Trade = 9
													case 'A': return '0';///< Day Trade + Normal = A
											   }
											   break;
	}
	return '0';
}
//---------------------------------------------------------------------------
char TOrderAdapter::ToOrderKind( nsOrderMessageDefine::MarketEnum Market, nsOrderMessageDefine::PositionEffectEnum PE, char TSEOrdTp )
{
	bool DayTrade;

	if( PE == nsOrderMessageDefine::peDayTradeOpen || PE == nsOrderMessageDefine::peDayTrade )
		DayTrade = true;
	else
		DayTrade = false;
	switch( Market )
	{
		case nsOrderMessageDefine::mTWFutures:
		case nsOrderMessageDefine::mTWOptions: 	if( DayTrade == true )
													return 'Y';
												return 'N';
		case nsOrderMessageDefine::mTSE:
		case nsOrderMessageDefine::mOTC:       switch( TSEOrdTp )
											   {
													case '0': if( DayTrade == true ) ///< Normal + Day trade = A
																  return 'A';
															  return '0';
													case '3': if( DayTrade == true ) ///< Trade Margin + Day trade = 9
																  return '9';
															  return '3';            ///< Trade Margin = 3
													case '4': if( DayTrade == true ) ///< Sell short + Day trade = 9
																  return '9';
															  return '4';            ///< Sell short = 4
													case 'N':
													case 'Y':
													case '9':
													case 'A': return TSEOrdTp;
											   }
                                               break;
	}
	return '0';
}
//---------------------------------------------------------------------------
//  Values in User Define
//
//  KT: kbar time HHMM.
//  T:  Send message time.
//  IP: Client IP.
//  DT: DayTrade
//  ST: Strategy Name.
//
//  HexIP:            "FFFFFFFF" -> 255.255.255.255  length 8
//  DayTrade:         'Y'/'N'|'A''9'/'0','3','4'     length 1
//  KBar time HHMM:   "1259"     -> 12:59            length 4
//  Strategy Name:    "ST00001"                      length 7
//---------------------------------------------------------------------------
void TOrderAdapter::GetUserData( nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& Ex, UFC::AnsiString& UserData, UFC::AnsiString StrategyName, nsOrderMessageDefine::PositionEffectEnum PE, char TSEOrdTp )
{
	int             KHHMM   = FOrderStore->MarketDataStore->GetClock( Ex.c_str() )->GetHHMM();
	char            OrdKind = ToOrderKind( Market, PE, TSEOrdTp );
	UFC::AnsiString HexIP;

	ToHexIP( FConnection->GetLocalIP(), HexIP );
	StrategyName.PadThis( 7, ' ');
	UserData.Printf( "%s%c%04d%s", HexIP.c_str(), OrdKind, KHHMM, StrategyName.c_str() );
}
//---------------------------------------------------------------------------


