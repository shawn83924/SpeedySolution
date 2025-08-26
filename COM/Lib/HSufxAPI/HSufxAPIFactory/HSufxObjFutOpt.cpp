//---------------------------------------------------------------------------
#ifdef _CTP
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#include "HSufxObjFutOpt.h"
#include "TTaifexConnection.h"
#include "APIParams.h"
//---------------------------------------------------------------------------
#include <WideStrUtils.hpp>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
bool HSufxFuncSortQryOrder( HSufxFutOpt::dataQryOrderResult* elem1, HSufxFutOpt::dataQryOrderResult* elem2 )
{
	return ( _atoi64(elem1->confirm_id) < _atoi64(elem2->confirm_id) );
}
//---------------------------------------------------------------------------
bool HSufxFuncSortQryFill( HSufxFutOpt::dataQryFillResult* elem1, HSufxFutOpt::dataQryFillResult* elem2 )
{
	return ( _atoi64(elem1->business_id) < _atoi64(elem2->business_id) );
}
//---------------------------------------------------------------------------
// class HSufxObjFutOpt
//
// Sending data process implementation ( ---> )
//---------------------------------------------------------------------------
HSufxObjFutOpt::HSufxObjFutOpt( TTaifexConnection* SpeedyConnPtr )
: FSpeedyConnPtr( SpeedyConnPtr )
, FHSufxConfig( NULL )
, FHSufxConnection( NULL )
, FHSufxSysNodeID( 0 )
, FHSufxBranchNo( 0 )
, FHSufxSessionNo( 0 )
, FHSufxRequestNum( 500 )
{
	// Create message window
	if ( InitMessageWindow() == FALSE )
	{
		UFC::BufferedLog::DebugPrintf( " Register HSUFX_FutOpt window class failed." );
		throw UFC::Exception( " Register HSUFX_FutOpt window class failed." );
	}
	if ( CreateMessageWindow() == FALSE )
	{
		UFC::BufferedLog::DebugPrintf( " Can't create the HSUFX_FutOpt message window." );
		throw UFC::Exception( " Can't create the HSUFX_FutOpt message window." );
	}
	UFC::BufferedLog::DebugPrintf( " Create HSUFX_FutOpt message window:%p Thread:%d", (void*)FHwnd, GetCurrentThreadId() );
}
//---------------------------------------------------------------------------
HSufxObjFutOpt::~HSufxObjFutOpt()
{
	DestroyWindow( FHwnd );
	UnregisterClassW( MessageWindowsClassName, FInstance );
}
//---------------------------------------------------------------------------
//
// HSufx Receiving functions (<-)
//
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxOnConnect( void )
{
	// Trigger FSpeedyConnPtr->FListener->OnConnected() when HSufxConnect() return
	// no need call OnConnected() in this
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxOnClose( void )
{
	bool OrgIsConnected = FSpeedyConnPtr->FIsConnected;
	FSpeedyConnPtr->FIsConnected = false;
	FSpeedyConnPtr->FIsLogon = false;
	FSpeedyConnPtr->DetachShm();
	if ( FSpeedyConnPtr->FListener && OrgIsConnected == true )
		FSpeedyConnPtr->FListener->OnDisconnected();
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxReQueryCmd( HSufxFutOpt::dataReQueryCmd* Data )
{
	if ( Data->FunctionID == 338301 )		///< QryOrderConfirm
		HSufxSendQryOrderConfirm( Data->position_str );
	else if ( Data->FunctionID == 338302 )	///< QryOrderFill
		HSufxSendQryOrderFill( Data->position_str );
	else if ( Data->FunctionID == 338303 )	///< QryPosition
		HSufxSendQryPosition( Data->position_str );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxLoginResult( HSufxFutOpt::dataLoginResult* Data )
{
	AnsiString  ReplyString( "" );

	if ( Data->IsLoginOK == false ) ///< Login failed.
	{
		ReplyString = Data->ErrMsg;
		FSpeedyConnPtr->FIsLogon    = false;
		FSpeedyConnPtr->FCurrentConnectionID = 0;
		UFC::BufferedLog::Printf( " HSufxLoginResult() Msg[%s] Login failed.", Data->ErrMsg );
		if ( FSpeedyConnPtr->FListener != NULL )
			FSpeedyConnPtr->FListener->OnLogonReply( ReplyString.c_str(), lrFailed, FSpeedyConnPtr->FCurrentConnectionID );
	}
	else ///< Login OK
	{
		// 1. Set TTaifexConnection fields
		FSpeedyConnPtr->FTradeingDate = UFC::AnsiString( Data->init_date );
		FSpeedyConnPtr->FIsRecover = false;
		FSpeedyConnPtr->FRecoverCount = 0;
		FSpeedyConnPtr->RemoveShm( "HSUFX_FutOpt" );
		FSpeedyConnPtr->AttachShm( "HSUFX_FutOpt", (char*)FSpeedyConnPtr->FTradeingDate.c_str() );
		FSpeedyConnPtr->FCTPFrontID   = 0;
		FSpeedyConnPtr->FCTPSessionID = Data->session_no;
		// no set FIsLogon
		// delay set at Recover finished
		//FSpeedyConnPtr->FIsLogon = true;
		FSpeedyConnPtr->FUseNID64 = true;
		FSpeedyConnPtr->FCurrentConnectionID = Data->session_no;
		UFC::CreateDir( "./log" );
		UFC::BufferedLog::Printf( " HSufxLoginResult()  Login OK! Account[%s] ClientID[%s] UserToken[%s]"
								  , Data->fund_account, Data->client_id, Data->user_token );
		UFC::BufferedLog::Printf( " sysnode_id[%d] branch_no[%d] session_no[%d]"
								  , Data->sysnode_id, Data->branch_no, Data->session_no );

		// 2. Set HSufx only fields
		if ( strlen(FHSufxUserToken) <= 40 )
			strcpy( FHSufxUserToken, Data->user_token );
		else
		{
			memcpy( FHSufxUserToken, Data->user_token, 40 );
			FHSufxUserToken[40] = 0;
		}
		if ( strlen(FHSufxClientID) <= 18 )
			strcpy( FHSufxClientID, Data->client_id );
		else
		{
			memcpy( FHSufxClientID, Data->client_id, 18 );
			FHSufxClientID[18] = 0;
		}
		FHSufxSysNodeID  = Data->sysnode_id;
		FHSufxBranchNo   = Data->branch_no;
		FHSufxSessionNo  = Data->session_no;
		// no trigger FListener->OnLogonReply
		// delay trigger at Recover finished
		// ( Query Confirm & Query Fill )

		// 3. Send Subscribe Push
		HSufxSendSubscribePush( ); ///< Confirm & Fill Report
	}
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxLogoutResult( HSufxFutOpt::dataLogoutResult* /*Data*/ )
{
	// no implementation
	// 功能[331102]不存在
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxChangePwdResult( HSufxFutOpt::dataChangePwdResult* Data )
{
	TNewsMessage Msg;

	Msg.SetHeadline( "HSUFX_FutOpt" );
	Msg.SetID( CTP_OnPasswordChanged );
	if ( Data->IsChangePwdOK )
	{
		Msg.SetText( "OK" );
		UFC::BufferedLog::Printf( " HSufxChangePwdResult: UserID[%s] Change Password OK! ", FSpeedyConnPtr->FID.c_str() );
	}
	else
	{
		Msg.SetText( Data->ErrMsg );
		UFC::BufferedLog::Printf( " HSufxChangePwdResult: Change Password failed! Msg[%s]", Data->ErrMsg );
	}
	if ( FSpeedyConnPtr->FListener != NULL )
		FSpeedyConnPtr->FListener->OnNews( &Msg );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxSubscribeResult( HSufxFutOpt::dataSubscribeResult* Data )
{
	AnsiString ReplyString;
	if ( Data->IsSubscribeOK )
	{
		if ( FSpeedyConnPtr->FListener != NULL )
		{
			if ( Data->IssueType == 33101 )
			{
				// Send Query Order Confirm
				HSufxSendQryOrderConfirm( );
			}
		}
	}
	else
	{
		if ( FSpeedyConnPtr->FListener != NULL )
		{
			ReplyString.printf( "Subscribe Push(%d) reply error!!", Data->IssueType );
			FSpeedyConnPtr->FListener->OnLogonReply( ReplyString.c_str(), lrFailed, FSpeedyConnPtr->FCurrentConnectionID );
		}
	}
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxQryOrderResult( HSufxFutOpt::dataQryOrderResult* Data )
{
	UFC::AnsiString          OrderLoacalAndSysID( Data->entrust_no );
	UFC::AnsiString          ExecID;
	UFC::AnsiString          ExecID2;
	UFC::AnsiString          ExchangeCode = HSufxTo_ExchangeCode( Data->futu_exch_type );
	COrderInfo*              Order;
	UFC::AnsiString          StrHHMMSSmmm;
	TExecutionReportMessage* ExecutionReportPtr = new TExecutionReportMessage();
	TExecutionReportMessage& ExecutionReport = *ExecutionReportPtr;
	bool                     IsDup;

	// 1. Process Query Order Confirm Result
	// 1.1 Create COrderInfo -> HSufxAccepted
	Order = new COrderInfo( FHSufxBranchNo , FHSufxSessionNo, ExchangeCode, Data );
	Order->HSufxAccepted( OrderLoacalAndSysID );
	FSpeedyConnPtr->FOrderRecords.Add( Order );
	FSpeedyConnPtr->AddOrderLocalIDToMap( ExchangeCode, Data->entrust_no, Order );

	// 1.2 TrigerOnExecutionReport for PendingNew
	FSpeedyConnPtr->CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Order );
	UFC::BufferedLog::Printf( " -------------------- HSufx Pending (by Query) -------------------" );
	StrHHMMSSmmm.Printf( "%09d", Data->entrust_time*1000 );
	ExecID.Printf( "P_%d", Data->entrust_no );
	ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osPendingNew );
	ExecutionReport.SetExecType( nsOrderMessageDefine::etPendingNew );
	ExecutionReport.SetOrderID( OrderLoacalAndSysID.c_str() );
	ExecutionReport.SetNID( FSpeedyConnPtr->GenerateNID( nsOrderMessageDefine::mtNew ) );
	ExecutionReport.SetAllUserData( Order->GetUserData() );
	ExecutionReport.SetExecID( ExecID.c_str() );
	ExecutionReport.SetLastQty( 0 );
	ExecutionReport.SetTransactTime( StrHHMMSSmmm.c_str() );
	// TrigerOnExecutionReport
	IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
	UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] Symbol[%s] entrust_no[%s] ExecID[%s] Dup[%s]"
							  , ExecutionReport.GetNID(), ExchangeCode.c_str(), Order->GetSymbol(), OrderLoacalAndSysID.c_str()
							  , ExecID.c_str(), IsDup ? "Yes" : "No" );
	FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );

	// 1.3 EachangeAccepted
	//     TrigerOnExecutionReport for New
	if ( Data->entrust_status != '0' && ///< 未報
		 Data->entrust_status != '1' && ///< 待報
		 Data->entrust_status != '9' )  ///< 廢單
	{
		Order->EachangeAccepted( OrderLoacalAndSysID );
		StrHHMMSSmmm.Printf( "%09d", Data->report_time*1000 );
		UFC::BufferedLog::Printf( " -------------------- HSufx Confirm (by Query) -------------------" );
		ExecID.Printf( "N_%d", Data->entrust_no );
		ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osNew );
		ExecutionReport.SetExecType( nsOrderMessageDefine::etNew );
		ExecutionReport.SetExecID( ExecID.c_str() );
		ExecutionReport.SetTransactTime( StrHHMMSSmmm.c_str() );
		// TrigerOnExecutionReport
		IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
		UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] Symbol[%s] entrust_no[%s] ExecID[%s] Dup[%s]"
								  , ExecutionReport.GetNID(), ExchangeCode.c_str(), Order->GetSymbol(), OrderLoacalAndSysID.c_str()
								  , ExecID.c_str(), IsDup ? "Yes" : "No" );
		FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	}

	// 2. Process Query Order Execute Result ( Cancel , Reject )
	if ( Data->entrust_status == '5' ) ///< 部撤 (撤單成功)
	{
		UFC::BufferedLog::Printf( " -------------- HSufx Cancel after fill (by Query) ---------------" );
		ExecID2.Printf( "C_%d", Data->entrust_no );
		ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osCanceled );
		ExecutionReport.SetExecType( nsOrderMessageDefine::etCanceled );
		ExecutionReport.SetExecID( ExecID2.c_str() );
		UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] Symbol[%s] entrust_no[%s] ExecID[%s] Dup[%s]"
								  , ExecutionReport.GetNID(), ExchangeCode.c_str(), Order->GetSymbol(), OrderLoacalAndSysID.c_str()
								  , ExecID2.c_str() );
		// Add to cancel after fill queue
		FHSufxCancelAfterFillList.Add( ExecutionReportPtr );

		return; ///< don't delete ExecutionReportPtr
	}
	else if ( Data->entrust_status == '6' ) ///< 已撤 (撤單成功)
	{
		UFC::BufferedLog::Printf( " ------------------- HSufx Canceled (by Query) -------------------" );
		ExecID2.Printf( "C_%d", Data->entrust_no );
		ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osCanceled );
		ExecutionReport.SetExecType( nsOrderMessageDefine::etCanceled );
		ExecutionReport.SetExecID( ExecID2.c_str() );
		IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
		UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] Symbol[%s] entrust_no[%s] ExecID[%s] Dup[%s]"
								  , ExecutionReport.GetNID(), ExchangeCode.c_str(), Order->GetSymbol(), OrderLoacalAndSysID.c_str()
								  , ExecID2.c_str(), IsDup ? "Yes" : "No" );
		FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	}
	else if ( Data->entrust_status == '9' ) ///< 廢單 (委託失敗)
	{
		UTF8String ErrMsgUTF8( AnsiToUtf8Ex(Data->error_message, 936) );
		UFC::BufferedLog::Printf( " -------------------- HSufx Reject (by Query) --------------------" );
		ExecID2.Printf( "R_%d", Data->entrust_no );
		Order->CloseOrder();
		ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osRejected );
		ExecutionReport.SetExecType( nsOrderMessageDefine::etRejected );
		ExecutionReport.SetCxlRejResponseTo( nsOrderMessageDefine::crrNew );
		ExecutionReport.SetExecID( ExecID2.c_str() );
		ExecutionReport.SetText( ErrMsgUTF8.c_str() );
		IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
		UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] Symbol[%s] entrust_no[%d]"
								  , ExecutionReport.GetNID(), ExchangeCode.c_str(), Order->GetSymbol(), Order->GetOrderLocalID() );
		UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] Msg[%s]", ExecID2.c_str(), IsDup ? "Yes" : "No", ErrMsgUTF8.c_str() );
		FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	}

	delete ExecutionReportPtr;
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxQryFillResult( HSufxFutOpt::dataQryFillResult* Data )
{
	UFC::AnsiString ExchangeCode = HSufxTo_ExchangeCode( Data->futu_exch_type );

	// 1. Process Query Order Fill Result
	// 1.1 Use ExchangeID + OrderLocalID to find Order.
	COrderInfo*     Order = FSpeedyConnPtr->FindOrderByOrderLocalID( ExchangeCode, Data->entrust_no );
	if ( Order == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxQryFillResult() error!! ExchangeID[%s] futu_exch_type[%s] entrust_no[%d] <Reason: Order not found.>"
								  , ExchangeCode.c_str(), Data->futu_exch_type, Data->entrust_no );
		return;
	}

	// 1.2 Create and setting TExecutionReportMessage
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         OrderLocalID( Data->entrust_no );
	UFC::AnsiString         ExecID;
	bool                    IsDup;
	char                    strHHMMSSmmm[10];

	sprintf( strHHMMSSmmm, "%09d", Data->business_time*1000 );

	UFC::BufferedLog::Printf( " --------------------- HSufx Fill (by Query) ---------------------" );
	ExecID.Printf( "F_%d_%s", Data->entrust_no, Data->business_id );
	FSpeedyConnPtr->CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Order );
	IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReport.GetMarket(), ExecID );
	if ( IsDup == false )
		Order->Fill( (int)Data->business_amount );
	ExecutionReport.SetOrderID( OrderLocalID.c_str() );
	ExecutionReport.SetNID( FSpeedyConnPtr->GenerateNID( nsOrderMessageDefine::mtNew ) );
	if ( Order->GetLeavesQty() == 0 )
	{
		ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osFilled );
		ExecutionReport.SetExecType( nsOrderMessageDefine::etFilled );
	}
	else
	{
		ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osPartiallyFilled );
		ExecutionReport.SetExecType( nsOrderMessageDefine::etPartiallyFilled );
	}
	ExecutionReport.SetAllUserData( Order->GetUserData() );
	ExecutionReport.SetExecID( ExecID.c_str() );
	ExecutionReport.SetPrice( Data->futu_business_price );
	ExecutionReport.SetLastPx( Data->futu_business_price );
	ExecutionReport.SetOrderQty( (long)Data->business_amount );
	ExecutionReport.SetLastQty( (long)Data->business_amount );
	ExecutionReport.SetTransactTime( strHHMMSSmmm );

	// 2. TrigerOnExecutionReport
	UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] entrust_no[%s] FillSeq[%s] ExecID[%s] Dup[%s]"
							  , Order->GetOrderNID(), ExchangeCode.c_str(), OrderLocalID.c_str(), Data->business_id, ExecID.c_str(), IsDup ? "Yes" : "No" );
	UFC::BufferedLog::Printf( " Time[%s] Symbol[%s] Side[%c] LastPx[%.4f] LastQty[%d] CumQty[%d] LeavesQty[%d]"
							  , strHHMMSSmmm, Order->GetSymbol(), Data->entrust_bs == '1' ? 'B':'S', Data->futu_business_price
							  , (int)Data->business_amount, Order->GetCumQty(), Order->GetLeavesQty() );
	FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport,(ExecDup) IsDup );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxQryAssetResult( HSufxFutOpt::dataQryAssetResult* Data )
{
	// Process Query Asset Result
	UFC::AnsiString MoneyType( Data->money_type );
	MoneyType.TrimRight();
	if ( MoneyType == "0" ) ///< 人民幣
	{
		TNewsMessage Msg;
		UFC::NameValueMessage BalanceMsg("^\n");

		Msg.SetHeadline( "HSUFX_FutOpt" );
		Msg.SetID( CTP_OnTradingAccount );
		BalanceMsg.Append( "money_type",           MoneyType );
		BalanceMsg.Append( "current_balance",      Data->current_balance );
		BalanceMsg.Append( "enable_balance",       Data->enable_balance );
		BalanceMsg.Append( "fetch_balance",        Data->fetch_balance );
		BalanceMsg.Append( "frozen_balance",       Data->frozen_balance );
		BalanceMsg.Append( "pre_entrust_balance",  Data->pre_entrust_balance );
		BalanceMsg.Append( "entrust_balance",      Data->entrust_balance );
		BalanceMsg.Append( "hold_income",          Data->hold_income );
		BalanceMsg.Append( "hold_income_float",    Data->hold_income_float );
		BalanceMsg.Append( "begin_equity_balance", Data->begin_equity_balance );
		BalanceMsg.Append( "equity_balance",       Data->equity_balance );
		BalanceMsg.Append( "interest_balance",     Data->interest_balance );
		BalanceMsg.Append( "drop_income",          Data->drop_income );
		BalanceMsg.Append( "drop_income_float",    Data->drop_income_float );
		BalanceMsg.Append( "business_fare",        Data->business_fare );
		BalanceMsg.Append( "hold_margin",          Data->hold_margin );
		BalanceMsg.Append( "exch_hold_margin",     Data->exch_hold_margin );
		BalanceMsg.Append( "client_risk_rate",     Data->client_risk_rate );
		BalanceMsg.Append( "exch_risk_rate",       Data->exch_risk_rate );
		BalanceMsg.Append( "out_premium",          Data->out_premium );
		BalanceMsg.Append( "in_premium",           Data->in_premium );
		BalanceMsg.Append( "market_value",         Data->market_value );

		Msg.SetText( BalanceMsg.ToString().c_str() );
		if ( FSpeedyConnPtr->FListener != NULL )
			FSpeedyConnPtr->FListener->OnNews( &Msg );
	}
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxQryPositionResult( HSufxFutOpt::dataQryPositionResult* Data )
{
	// 1. Check if End notify
	if ( Data->IsEndNotify )
		return;

	// 2. Process Query Hold Position Result
	UFC::BufferedLog::Printf( " HSufxQryPositionResult() %s %s futures_account[%s] entrust_bs[%c] futu_product_type[%c] begin_amount[%d] enable_amount[%d] real_enable_amount[%d]"
							  , FSpeedyConnPtr->FID.c_str(), Data->futu_code, Data->futures_account, Data->entrust_bs, Data->futu_product_type
							  , Data->begin_amount, Data->enable_amount, Data->real_enable_amount );
	UFC::BufferedLog::Printf( " real_amount[%d] real_current_amount[%d] old_current_amount[%d] hold_income_float[%.2f] hold_income[%.2f] hold_margin[%.2f] average_price[%.6f]"
							  , Data->real_amount, Data->real_current_amount, Data->old_current_amount, Data->hold_income_float
							  , Data->hold_income, Data->hold_margin, Data->average_price );
	UFC::BufferedLog::FlushToFile();

	if ( FSpeedyConnPtr->FListener != NULL )
	{
		char ctpBS = ( Data->entrust_bs == '1' ? '0' : '1' );
		UFC::AnsiString NewsText;
		NewsText.Printf( "0|OK|0|1|%s|%d|%s|%c|00000000|%d|%15.6lf"
						 , Data->futu_code, FHSufxBranchNo, FSpeedyConnPtr->FID.c_str(), ctpBS
						 , Data->begin_amount, Data->average_price );
		TNewsMessage Msg;
		Msg.SetHeadline( "HSUFX_FutOpt" );
		Msg.SetID( CTP_OnInvestorOpenPosition );
		Msg.SetText( NewsText.c_str() );
		FSpeedyConnPtr->FListener->OnNews( &Msg );
	}
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxExchStatusResult( HSufxFutOpt::dataExchStatusResult* Data )
{
	const char* strCTPExchID = HSufxTo_ExchangeCode( Data->futu_exch_type );
	char charCTPStatus;
	// HSufx '0'-連接斷開 '1'-開盤前 '2'-集合報價 '3'-競價平衡 '4'-競價撮合 '5'-連續交易 '6'-暫停交易 '7'-閉市 'X'-未知
	// CTP   '0'-開盤前 '1'-非交易 '2'-連續交易 '3'-集合競價報單 '4'-集合競價價格平衡 '5'-集合競價撮合 '6'-收盤
	switch ( Data->exchange_status )
	{
		case '1': charCTPStatus = '0'; break;
		case '2': charCTPStatus = '3'; break;
		case '3': charCTPStatus = '4'; break;
		case '4': charCTPStatus = '5'; break;
		case '5': charCTPStatus = '2'; break;
		case '7': charCTPStatus = '6'; break;
		case 'X': return;
		default : charCTPStatus = '1'; ///< '0' '6'
	}

	TNewsMessage Msg;
	UFC::NameValueMessage StatusMsg("^\n");
	Msg.SetHeadline( "HSUFX_FutOpt" );
	Msg.SetID( CTP_OnExchangeStatus );
	StatusMsg.Append( "Exchange", strCTPExchID );
	StatusMsg.Append( "Status",  UFC::AnsiString(charCTPStatus) );
	Msg.SetText( StatusMsg.ToString().c_str() );
	if ( FSpeedyConnPtr->FListener != NULL )
		FSpeedyConnPtr->FListener->OnNews( &Msg );
	UFC::BufferedLog::Printf( " HSufxExchStatusResult Exchange[%s] Status[%c] ", strCTPExchID, charCTPStatus );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxExchTimeResult( HSufxFutOpt::dataExchTimeResult* Data )
{
	// Update Exchange clocks.
	UFC::AnsiString StrTime;
	TNewsMessage Msg;
	UFC::NameValueMessage TimeMsg("^\n");
	Msg.SetHeadline( "HSUFX_FutOpt");
	Msg.SetID( CTP_OnExchangeTime );
	HSufxTo_CTPExchTime( StrTime, Data->SHFE_curr_time );  TimeMsg.Append( "SHFE", StrTime );
	HSufxTo_CTPExchTime( StrTime, Data->DCE_curr_time );   TimeMsg.Append( "DCE", StrTime );
	HSufxTo_CTPExchTime( StrTime, Data->CZCE_curr_time );  TimeMsg.Append( "CZCE", StrTime );
	HSufxTo_CTPExchTime( StrTime, Data->CFFEX_curr_time ); TimeMsg.Append( "CFFEX", StrTime );
	Msg.SetText( TimeMsg.ToString().c_str() );
	if ( FSpeedyConnPtr->FListener != NULL )
		FSpeedyConnPtr->FListener->OnNews( &Msg );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxPendingExecution( HSufxFutOpt::dataPendingExecution* Data )
{
	UFC::AnsiString ExchangeCode;
	UFC::AnsiString OrderLocalID( Data->entrust_no );

	// 1. Use FrontID + SessionID + OrderRef to find Order.
	COrderInfo* Order = FSpeedyConnPtr->FindOrderByOrderRef( 0, FHSufxSessionNo, Data->hSend );
	if ( Order == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxPendingExecution() error!! FrontID[0] SessionID[%d] OrderRef[%d] entrust_no[%d] <Reason: Order not found.>"
								  , FHSufxSessionNo, Data->hSend, Data->entrust_no );
		return;
	}
	ExchangeCode = Order->GetExchangeID();
	Order->HSufxAccepted( OrderLocalID );
	FSpeedyConnPtr->AddOrderLocalIDToMap( ExchangeCode, Data->entrust_no, Order );

	// 2. Create and setting TExecutionReportMessage
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	bool                    IsDup;
	char                    strHHMMSSmmm[10];
	UFC::GetTimeString( strHHMMSSmmm, FALSE );

	UFC::BufferedLog::Printf( " -------------------------- HSufx Pending ------------------------" );
	ExecID.Printf( "P_%d", Data->entrust_no );
	FSpeedyConnPtr->CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Order );
	ExecutionReport.SetOrderID( OrderLocalID.c_str() );
	ExecutionReport.SetNID( Order->GetOrderNID() );
	ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osPendingNew );
	ExecutionReport.SetExecType( nsOrderMessageDefine::etPendingNew );
	ExecutionReport.SetAllUserData( Order->GetUserData() );
	ExecutionReport.SetExecID( ExecID.c_str() );
	ExecutionReport.SetLastQty( 0 );
	ExecutionReport.SetTransactTime( strHHMMSSmmm );

	// 3. TrigerOnExecutionReport
	IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
	UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] Symbol[%s] entrust_no[%s]"
							  , Order->GetOrderNID(), ExchangeCode.c_str(), Order->GetSymbol(), OrderLocalID.c_str() );
	UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] SessionID[%d] OrderRef[%d]", ExecID.c_str(), IsDup ? "Yes" : "No", FHSufxSessionNo, Data->hSend );
	FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	UFC::BufferedLog::Printf( " ---------------- Sent Pending New Execution ---------------------" );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxRejectExecution( HSufxFutOpt::dataRejectExecution* Data )
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	bool                    IsDup;
	char                    strHHMMSSmmm[10];
	COrderInfo*             Order = NULL;
	UFC::GetTimeString( strHHMMSSmmm, FALSE );

	// 1. Find Order
	if ( Data->ResponseTo == nsOrderMessageDefine::crrCancel )
	{
		// 1.1 Use FrontID + SessionID + CancelOrderRef to find OrderLocalID
		HSufxFutOpt::dataRefToOrderKey* OrigOrderKey;
		UFC::AnsiString OrderRefKey;
		OrderRefKey.Printf( "%d|%d|%d", 0, FHSufxSessionNo, Data->hSend );
		if ( FHSufxCancelRefToOrigOrderKey.GetObjectByKey( OrderRefKey, OrigOrderKey ) == FALSE )
		{
			UFC::BufferedLog::Printf( " HSufxRejectExecution() Cancel Reject error!! FrontID[0] SessionID[%d] CancelOrderRef[%d] <Reason: OrderLocalID not found.>"
									  , FHSufxSessionNo, Data->hSend );
			return;
		}
		// 1.2 Use ExchangeID + OrderLocalID to find Order
		Order = FSpeedyConnPtr->FindOrderByOrderLocalID( OrigOrderKey->ExchangeCode, OrigOrderKey->entrust_no );
		if ( Order == NULL )
		{
			UFC::BufferedLog::Printf( " HSufxRejectExecution() Cancel Reject error!! ExchangeID[%s] entrust_no[%d] <Reason: Order not found.>"
									  , OrigOrderKey->ExchangeCode.c_str(), OrigOrderKey->entrust_no );
			return;
		}

		UFC::BufferedLog::Printf( " ---------------------- HSufx Cancel Reject ----------------------" );
		ExecID.Printf( "CR_%d_%d", FHSufxSessionNo, Data->hSend );
	}
	else
	{
		// 1.1 Use FrontID + SessionID + OrderRef to find Order.
		Order = FSpeedyConnPtr->FindOrderByOrderRef( 0, FHSufxSessionNo, Data->hSend );
		if ( Order == NULL )
		{
			UFC::BufferedLog::Printf( " HSufxRejectExecution() New Reject error!! FrontID[0] SessionID[%d] OrderRef[%d] <Reason: Order not found.>"
									  , FHSufxSessionNo, Data->hSend );
			return;
		}
		Order->CloseOrder();

		UFC::BufferedLog::Printf( " ------------------------- HSufx Reject --------------------------" );
		ExecID.Printf( "R_%d_%d", FHSufxSessionNo, Data->hSend );
	}

	// 2. Setting TExecutionReportMessage
	FSpeedyConnPtr->CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Order );
	ExecutionReport.SetOrderID( Order->GetOrderLocalID() ); ///< ??
	ExecutionReport.SetNID( Order->GetOrderNID() );
	ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osRejected );
	ExecutionReport.SetExecType( nsOrderMessageDefine::etRejected );
	ExecutionReport.SetCxlRejResponseTo( Data->ResponseTo );
	ExecutionReport.SetAllUserData( Order->GetCxlUserData() );
	ExecutionReport.SetExecID( ExecID.c_str() );
	ExecutionReport.SetLastQty( 0 );
	ExecutionReport.SetText( Data->ErrMsg );
	ExecutionReport.SetTransactTime( strHHMMSSmmm );

	// 3. TrigerOnExecutionReport
	IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
	UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] Symbol[%s] SessionID[%d] OrderRef[%d]"
							  , Order->GetOrderNID(), Order->GetExchangeID(), Order->GetSymbol(), FHSufxSessionNo, Data->hSend );
	UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] Msg[%s]", ExecID.c_str(), IsDup ? "Yes" : "No", Data->ErrMsg );
	FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	if ( Data->ResponseTo == nsOrderMessageDefine::crrCancel )
		UFC::BufferedLog::Printf( " ---------------- Sent Cancel Reject Execution -------------------" );
	else
		UFC::BufferedLog::Printf( " ------------------- Sent Reject Execution -----------------------" );
}
//---------------------------------------------------------------------------
char* HSufxObjFutOpt::GetNextPushToken( char*& strPos )
{
	char* Token = strPos;

	if ( *strPos == 0 )
		return Token;

	while( true )
	{
		if ( *strPos == 0x01 ) ///< QH delimiter
		{
			*strPos = 0;
			strPos++;
			return Token;
		}
		else if ( *strPos == 0 ) ///< end of QH record
			return Token;
		strPos++;
	}
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxPushReport( HSufxFutOpt::dataPushReport* Data )
{
	if ( Data->LY == 'B' )
	{
		HSufxFutOpt::dataPushFill FillData;
		memset( &FillData, 0, sizeof(HSufxFutOpt::dataPushFill) );
		char *strPos = Data->QH;

		FillData.entrust_no            = GetNextPushToken( strPos );
		FillData.futures_account       = GetNextPushToken( strPos );
		FillData.futu_exch_type        = GetNextPushToken( strPos );
		FillData.business_no           = GetNextPushToken( strPos );
		FillData.futu_code             = GetNextPushToken( strPos );
		FillData.entrust_bs            = GetNextPushToken( strPos );
		FillData.futures_direction     = GetNextPushToken( strPos );
		FillData.futu_business_price   = GetNextPushToken( strPos );
		FillData.business_amount       = GetNextPushToken( strPos );
		FillData.hedge_type            = GetNextPushToken( strPos );
		FillData.fund_account          = GetNextPushToken( strPos );
		FillData.report_id             = GetNextPushToken( strPos );
		FillData.firm_id               = GetNextPushToken( strPos );
		FillData.operator_no           = GetNextPushToken( strPos );
		FillData.client_group          = GetNextPushToken( strPos );
		FillData.entrust_amount        = GetNextPushToken( strPos );
		FillData.total_business_amount = GetNextPushToken( strPos );
		FillData.withdraw_amount       = GetNextPushToken( strPos );
		FillData.futu_entrust_price    = GetNextPushToken( strPos );
		FillData.entrust_status        = GetNextPushToken( strPos );
		FillData.branch_no             = GetNextPushToken( strPos );
		FillData.batch_no              = GetNextPushToken( strPos );
		FillData.entrust_type          = GetNextPushToken( strPos );
		FillData.amount_per_hand       = GetNextPushToken( strPos );
		FillData.forcedrop_reason      = GetNextPushToken( strPos );
		FillData.init_date             = GetNextPushToken( strPos );
		FillData.business_time         = GetNextPushToken( strPos );
		FillData.confirm_id            = GetNextPushToken( strPos );
		FillData.entrust_occasion      = GetNextPushToken( strPos );
		FillData.futu_entrust_price2   = GetNextPushToken( strPos );
		FillData.entrust_prop          = GetNextPushToken( strPos );
		FillData.arbit_code            = GetNextPushToken( strPos );
		FillData.entrust_reference     = GetNextPushToken( strPos );
		FillData.position_str          = GetNextPushToken( strPos );
		FillData.session_no            = GetNextPushToken( strPos );

		HSufxFutOpt::dataReportFill ReportFillData( FillData );
		HSufxPushFill( ReportFillData, FALSE );
	}
	else ///< '0' '3' 'O'
	{
		HSufxFutOpt::dataPushConfirm ConfirmData;
		memset( &ConfirmData, 0, sizeof(HSufxFutOpt::dataPushConfirm) );
		char *strPos = Data->QH;

		ConfirmData.entrust_no            = GetNextPushToken( strPos );
		ConfirmData.futures_account       = GetNextPushToken( strPos );
		ConfirmData.futu_exch_type        = GetNextPushToken( strPos );
		ConfirmData.futu_code             = GetNextPushToken( strPos );
		ConfirmData.entrust_bs            = GetNextPushToken( strPos );
		ConfirmData.futures_direction     = GetNextPushToken( strPos );
		ConfirmData.hedge_type            = GetNextPushToken( strPos );
		ConfirmData.fund_account          = GetNextPushToken( strPos );
		ConfirmData.report_id             = GetNextPushToken( strPos );
		ConfirmData.firm_id               = GetNextPushToken( strPos );
		ConfirmData.operator_no           = GetNextPushToken( strPos );
		ConfirmData.client_group          = GetNextPushToken( strPos );
		ConfirmData.entrust_amount        = GetNextPushToken( strPos );
		ConfirmData.total_business_amount = GetNextPushToken( strPos );
		ConfirmData.withdraw_amount       = GetNextPushToken( strPos );
		ConfirmData.futu_entrust_price    = GetNextPushToken( strPos );
		ConfirmData.entrust_status        = GetNextPushToken( strPos );
		ConfirmData.branch_no             = GetNextPushToken( strPos );
		ConfirmData.batch_no              = GetNextPushToken( strPos );
		ConfirmData.entrust_type          = GetNextPushToken( strPos );
		ConfirmData.amount_per_hand       = GetNextPushToken( strPos );
		ConfirmData.forcedrop_reason      = GetNextPushToken( strPos );
		ConfirmData.init_date             = GetNextPushToken( strPos );
		ConfirmData.entrust_time          = GetNextPushToken( strPos );
		ConfirmData.confirm_id            = GetNextPushToken( strPos );
		ConfirmData.entrust_occasion      = GetNextPushToken( strPos );
		ConfirmData.futu_entrust_price2   = GetNextPushToken( strPos );
		ConfirmData.entrust_prop          = GetNextPushToken( strPos );
		ConfirmData.arbit_code            = GetNextPushToken( strPos );
		ConfirmData.entrust_reference     = GetNextPushToken( strPos );
		ConfirmData.error_message         = GetNextPushToken( strPos );
		ConfirmData.position_str          = GetNextPushToken( strPos );
		ConfirmData.session_no            = GetNextPushToken( strPos );
		ConfirmData.second_code           = GetNextPushToken( strPos );
		ConfirmData.weave_type            = GetNextPushToken( strPos );
		ConfirmData.delta_entrust_fare    = GetNextPushToken( strPos );
		ConfirmData.delta_entrust_margin  = GetNextPushToken( strPos );
		ConfirmData.delta_entrust_premium = GetNextPushToken( strPos );

		HSufxFutOpt::dataReportConfirm ReportConfirmData( ConfirmData );
		HSufxPushConfirm( ReportConfirmData, FALSE );
	}
}
//---------------------------------------------------------------------------
BOOL HSufxObjFutOpt::HSufxPushReportReDo( HSufxRemainReport* Data )
{
	if ( Data->Type == 'C' )
		return HSufxPushConfirm( *((HSufxFutOpt::dataReportConfirm*)Data->ReportData), TRUE );
	else if ( Data->Type == 'F' )
		return HSufxPushFill( *((HSufxFutOpt::dataReportFill*)Data->ReportData), TRUE );

	return TRUE;
}
//---------------------------------------------------------------------------
BOOL HSufxObjFutOpt::HSufxPushConfirm( HSufxFutOpt::dataReportConfirm& Data, BOOL IsCallByReDo )
{
	if ( Data.entrust_status == '2' ) ///< 已報 (委託回報)
		return HSufxConfirmExecution( Data );
	else if ( Data.entrust_status == '9' ) ///< 廢單 (委託失敗)
		return HSufxRejectByExh( Data, nsOrderMessageDefine::crrNew, IsCallByReDo );
	else if ( Data.entrust_status == '5' || Data.entrust_status == '6' ) ///< 部撤/已撤 (撤單成功)
		return HSufxCanceledExecution( Data );
	else if ( Data.entrust_status == 'D' ) ///< 撤廢 (撤單失敗)
		return HSufxRejectByExh( Data, nsOrderMessageDefine::crrCancel, IsCallByReDo );
	else
		UFC::BufferedLog::Printf( " HSufxPushConfirm() Unhandle entrust_status=%c error!!", Data.entrust_status );

	return TRUE;
}
//---------------------------------------------------------------------------
BOOL HSufxObjFutOpt::HSufxPushFill( HSufxFutOpt::dataReportFill& Data, BOOL IsCallByReDo )
{
	if ( Data.entrust_status == '7' || Data.entrust_status == '8' ) ///< 部成/已成 (成交回報)
		return HSufxFillExecution( Data, IsCallByReDo );
	else
		UFC::BufferedLog::Printf( " HSufxPushFill() Unhandle entrust_status=%c error!!", Data.entrust_status );

	return TRUE;
}
//---------------------------------------------------------------------------
BOOL HSufxObjFutOpt::HSufxConfirmExecution( HSufxFutOpt::dataReportConfirm& Data )
{
	UFC::AnsiString ExecID;
	UFC::AnsiString ExchangeCode = HSufxTo_ExchangeCode( Data.futu_exch_type );
	bool            IsDup;
	int             entrust_no = atoi(Data.entrust_no.c_str());
	char            strHHMMSSmmm[10];
	UFC::GetTimeString( strHHMMSSmmm, FALSE );

	TExecutionReportMessage ExecutionReport;

	// Use ExchangeID + OrderLocalID to find Order.
	COrderInfo* Order = FSpeedyConnPtr->FindOrderByOrderLocalID( ExchangeCode, entrust_no );
	if ( Order == NULL ) ///< Order not found, exection form other client
	{
		UFC::BufferedLog::Printf( " --------------------- Receive Execution from other HSufx clients -------------------------" );
		UFC::BufferedLog::Printf( " HSufxConfirmExecution: Order ExchangeID[%s] entrust_no[%d] not found! Add Order record.", ExchangeCode.c_str(), entrust_no );
		UFC::BufferedLog::Printf( " ----------------------------------------------------------------------------------------" );
		// Create COrderInfo
		Order = new COrderInfo( FHSufxBranchNo, FHSufxSessionNo, ExchangeCode, Data );
		// 1. HSufxAccepted
		Order->HSufxAccepted( Data.entrust_no );
		FSpeedyConnPtr->FOrderRecords.Add( Order );
		FSpeedyConnPtr->AddOrderLocalIDToMap( ExchangeCode, entrust_no, Order );
		// 2 TrigerOnExecutionReport for PendingNew
		UFC::BufferedLog::Printf( " -------------------------- HSufx Pending ------------------------" );
		FSpeedyConnPtr->CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Order );
		ExecID.Printf( "P_%d", entrust_no );
		ExecutionReport.SetOrderID( Data.entrust_no.c_str() );
		ExecutionReport.SetNID( FSpeedyConnPtr->GenerateNID( nsOrderMessageDefine::mtNew ) );
		ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osPendingNew );
		ExecutionReport.SetExecType( nsOrderMessageDefine::etPendingNew );
		ExecutionReport.SetAllUserData( Order->GetUserData() );
		ExecutionReport.SetExecID( ExecID.c_str() );
		ExecutionReport.SetLastQty( 0 );
		ExecutionReport.SetTransactTime( strHHMMSSmmm );
		IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
		UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] Symbol[%s] entrust_no[%d] ExecID[%s] Dup[%s]"
								  , ExecutionReport.GetNID(), ExchangeCode.c_str(), Order->GetSymbol(), entrust_no
								  , ExecID.c_str(), IsDup ? "Yes" : "No" );
		FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
		UFC::BufferedLog::Printf( " ---------------- Sent Pending New Execution ---------------------" );
		// 3. EachangeAccepted
		Order->EachangeAccepted( Data.entrust_no ); ///< OrderSysID same as OrderLocalID
		// 4. TrigerOnExecutionReport for New
		UFC::BufferedLog::Printf( " -------------------------- HSufx Confirm ------------------------" );
		ExecID.Printf( "N_%d", entrust_no );
		ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osNew );
		ExecutionReport.SetExecType( nsOrderMessageDefine::etNew );
		ExecutionReport.SetExecID( ExecID.c_str() );
		IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
		UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] Symbol[%s] entrust_no[%d] ExecID[%s] Dup[%s]"
								  , ExecutionReport.GetNID(), ExchangeCode.c_str(), Order->GetSymbol(), entrust_no
								  , ExecID.c_str(), IsDup ? "Yes" : "No" );
		FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
		UFC::BufferedLog::Printf( " ------------------ Sent Confirmed Execution ---------------------" );
	}
	else ///< exection by self
	{
		// 1. EachangeAccepted
		Order->EachangeAccepted( Data.entrust_no ); ///< OrderSysID same as OrderLocalID
		//FSpeedyConnPtr->AddOrderSysIDToMap( ExchangeCode, entrust_no, Order );
		// 2. TrigerOnExecutionReport for New
		UFC::BufferedLog::Printf( " -------------------------- HSufx Confirm ------------------------" );
		ExecID.Printf( "N_%d", entrust_no );
		FSpeedyConnPtr->CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Order );
		ExecutionReport.SetOrderID( Data.entrust_no.c_str() );
		ExecutionReport.SetNID( Order->GetOrderNID() );
		ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osNew );
		ExecutionReport.SetExecType( nsOrderMessageDefine::etNew );
		ExecutionReport.SetAllUserData( Order->GetUserData() );
		ExecutionReport.SetExecID( ExecID.c_str() );
		ExecutionReport.SetLastQty( 0 );
		ExecutionReport.SetTransactTime( strHHMMSSmmm );
		IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
		UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] Symbol[%s] entrust_no[%d] ExecID[%s] Dup[%s]"
								  , Order->GetOrderNID(), ExchangeCode.c_str(), Order->GetSymbol(), entrust_no
								  , ExecID.c_str(), IsDup ? "Yes" : "No" );
		FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
		UFC::BufferedLog::Printf( " ------------------ Sent Confirmed Execution ---------------------" );
	}

	return TRUE;
}
//---------------------------------------------------------------------------
BOOL HSufxObjFutOpt::HSufxRejectByExh( HSufxFutOpt::dataReportConfirm& Data, nsOrderMessageDefine::CxlRejResponseToEnum ResponseTo, BOOL IsCallByReDo )
{
	UFC::AnsiString ExchangeCode = HSufxTo_ExchangeCode( Data.futu_exch_type );
	int             entrust_no = atoi(Data.entrust_no.c_str());
	UTF8String      ErrMsgUTF8( AnsiToUtf8Ex(Data.error_message.c_str(), 936) );

	// 1. Use ExchangeID + OrderLocalID to find Order.
	COrderInfo* Order = FSpeedyConnPtr->FindOrderByOrderLocalID( ExchangeCode, entrust_no );
	if ( Order == NULL )
	{
		if ( IsCallByReDo )
		{
			UFC::BufferedLog::Printf( " HSufxRejectByExh() error!! ExchangeID[%s] futu_exch_type[%s] entrust_no[%d] <Reason: Order not found. (Called by ReDo)>"
									  , ExchangeCode.c_str(), Data.futu_exch_type.c_str(), entrust_no );
		}
		else
		{
			AddRemainReport( Data );
			UFC::BufferedLog::Printf( " HSufxRejectByExh() error!! ExchangeID[%s] futu_exch_type[%s] entrust_no[%d] <Reason: Order not found. add to reamin report queue wait to ReDo>"
									  , ExchangeCode.c_str(), Data.futu_exch_type.c_str(), entrust_no );
		}
		return FALSE;
	}
	if ( Order->GetStatus() != cosPending && Order->GetStatus() != cosConfirm )
	{
		UFC::BufferedLog::Printf( " HSufxRejectByExh() error!! entrust_no[%d] <Reason: Order status not equal Pending and Confirm.>", entrust_no );
		return TRUE;
	}
	Order->CloseOrder();

	// 2. Create and setting TExecutionReportMessage
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	bool                    IsDup;
	char                    strHHMMSSmmm[10];
	UFC::GetTimeString( strHHMMSSmmm, FALSE );

	if ( ResponseTo == nsOrderMessageDefine::crrCancel )
	{
		UFC::BufferedLog::Printf( " -------------------- HSufx Exh Cancel Reject --------------------" );
		ExecID.Printf( "CR_%d", entrust_no );
	}
	else
	{
		UFC::BufferedLog::Printf( " ----------------------- HSufx Exh Reject ------------------------" );
		ExecID.Printf( "R_%d", entrust_no );
	}
	FSpeedyConnPtr->CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Order );
	ExecutionReport.SetOrderID( Data.entrust_no.c_str() );
	ExecutionReport.SetNID( Order->GetOrderNID() );
	ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osRejected );
	ExecutionReport.SetExecType( nsOrderMessageDefine::etRejected );
	ExecutionReport.SetCxlRejResponseTo( ResponseTo );
	ExecutionReport.SetAllUserData( Order->GetCxlUserData() );
	ExecutionReport.SetExecID( ExecID.c_str() );
	ExecutionReport.SetLastQty( 0 );
	ExecutionReport.SetText( ErrMsgUTF8.c_str() );
	ExecutionReport.SetTransactTime( strHHMMSSmmm );

	// 3. TrigerOnExecutionReport
	IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
	UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] Symbol[%s] entrust_no[%d]"
							  , Order->GetOrderNID(), ExchangeCode.c_str(), Order->GetSymbol(), entrust_no );
	UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] Msg[%s]", ExecID.c_str(), IsDup ? "Yes" : "No", ErrMsgUTF8.c_str() );
	FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	if ( ResponseTo == nsOrderMessageDefine::crrCancel )
		UFC::BufferedLog::Printf( " -------------- Sent Exh Cancel Reject Execution -----------------" );
	else
		UFC::BufferedLog::Printf( " ----------------- Sent Exh Reject Execution ---------------------" );

	return TRUE;
}
//---------------------------------------------------------------------------
BOOL HSufxObjFutOpt::HSufxCanceledExecution( HSufxFutOpt::dataReportConfirm& Data )
{
	UFC::AnsiString ExchangeCode = HSufxTo_ExchangeCode( Data.futu_exch_type );
	int             entrust_no = atoi(Data.entrust_no.c_str());

	// 1. Use ExchangeID + OrderLocalID to find Order.
	COrderInfo* Order = FSpeedyConnPtr->FindOrderByOrderLocalID( ExchangeCode, entrust_no );
	if ( Order == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxCanceledExecution() error!! ExchangeID[%s] futu_exch_type[%s] entrust_no[%d] <Reason: Order not found.>"
								  , ExchangeCode.c_str(), Data.futu_exch_type.c_str(), entrust_no );
		return TRUE;
	}
	Order->CloseOrder();

	// 2. Create and setting TExecutionReportMessage
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	bool                    IsDup;
	char                    strHHMMSSmmm[10];
	UFC::GetTimeString( strHHMMSSmmm, FALSE );

	UFC::BufferedLog::Printf( " ------------------------- HSufx Canceled ------------------------" );
	ExecID.Printf( "C_%d", entrust_no );
	FSpeedyConnPtr->CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Order );
	ExecutionReport.SetOrderID( Data.entrust_no.c_str() );
	if ( Order->GetCancelRef() == 0 )
		ExecutionReport.SetNID( Order->GetOrderNID() );
	else
		ExecutionReport.SetNID( Order->GetCancelNID() );
	ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osCanceled );
	ExecutionReport.SetExecType( nsOrderMessageDefine::etCanceled );
	ExecutionReport.SetAllUserData( Order->GetCxlUserData() );
	ExecutionReport.SetExecID( ExecID.c_str() );
	ExecutionReport.SetLastQty( 0 );
	ExecutionReport.SetTransactTime( strHHMMSSmmm );

	// 3. TrigerOnExecutionReport
	IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
	UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] Symbol[%s] entrust_no[%d] ExecID[%s] Dup[%s]"
							  , Order->GetOrderNID(), ExchangeCode.c_str(), Order->GetSymbol(), entrust_no
							  , ExecID.c_str(), IsDup ? "Yes" : "No" );
	FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	UFC::BufferedLog::Printf( " ------------------- Sent Canceled Execution ---------------------" );

	return TRUE;
}
//---------------------------------------------------------------------------
BOOL HSufxObjFutOpt::HSufxFillExecution( HSufxFutOpt::dataReportFill& Data, BOOL IsCallByReDo )
{
	UFC::AnsiString ExchangeCode = HSufxTo_ExchangeCode( Data.futu_exch_type );
	int             entrust_no = atoi(Data.entrust_no.c_str());

	// 1. Use ExchangeID + OrderLocalID to find Order.
	COrderInfo* Order = FSpeedyConnPtr->FindOrderByOrderLocalID( ExchangeCode, entrust_no );
	if ( Order == NULL )
	{
		if ( IsCallByReDo )
		{
			UFC::BufferedLog::Printf( " HSufxFillExecution() error!! ExchangeID[%s] futu_exch_type[%s] entrust_no[%d] <Reason: Order not found. (Called by ReDo)>"
									  , ExchangeCode.c_str(), Data.futu_exch_type.c_str(), entrust_no );
		}
		else
		{
			AddRemainReport( Data );
			UFC::BufferedLog::Printf( " HSufxFillExecution() error!! ExchangeID[%s] futu_exch_type[%s] entrust_no[%d] <Reason: Order not found. add to reamin report queue wait to ReDo>"
									  , ExchangeCode.c_str(), Data.futu_exch_type.c_str(), entrust_no );
		}
		return FALSE;
	}

	// 2. Create and setting TExecutionReportMessage
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	bool                    IsDup;
	char                    strHHMMSSmmm[10];

	sprintf( strHHMMSSmmm, "%09d", Data.business_time*1000 );

	UFC::BufferedLog::Printf( " --------------------------- HSufx Fill --------------------------" );
	ExecID.Printf( "F_%d_%s", entrust_no, Data.business_no.c_str() );
	FSpeedyConnPtr->CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Order );
	IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReport.GetMarket(), ExecID );
	if ( IsDup == false )
		Order->Fill( Data.business_amount );
	ExecutionReport.SetOrderID( Data.entrust_no.c_str() );
	ExecutionReport.SetNID( Order->GetOrderNID() );
	if ( Order->GetLeavesQty() == 0 )
	{
		ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osFilled );
		ExecutionReport.SetExecType( nsOrderMessageDefine::etFilled );
	}
	else
	{
		ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osPartiallyFilled );
		ExecutionReport.SetExecType( nsOrderMessageDefine::etPartiallyFilled );
	}
	ExecutionReport.SetAllUserData( Order->GetUserData() );
	ExecutionReport.SetExecID( ExecID.c_str() );
	ExecutionReport.SetPrice( Data.futu_business_price );
	ExecutionReport.SetLastPx( Data.futu_business_price );
	ExecutionReport.SetOrderQty( (long)Data.business_amount );
	ExecutionReport.SetLastQty( (long)Data.business_amount );
	ExecutionReport.SetTransactTime( strHHMMSSmmm );

	// 3. TrigerOnExecutionReport
	UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] entrust_no[%d] business_no[%s] ExecID[%s] Dup[%s]"
							  , Order->GetOrderNID(), ExchangeCode.c_str(), entrust_no, Data.business_no.c_str(), ExecID.c_str(), IsDup ? "Yes" : "No" );
	UFC::BufferedLog::Printf( " Account[%s] Time[%s] Symbol[%s] Side[%c] "
							  , Data.fund_account.c_str(), strHHMMSSmmm, Order->GetSymbol(), Data.entrust_bs == '1' ? 'B':'S' );
	UFC::BufferedLog::Printf( " LastPx[%.4f] LastQty[%d] CumQty[%d] LeavesQty[%d]"
							  , Data.futu_business_price, Data.business_amount, Order->GetCumQty(), Order->GetLeavesQty() );
	FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport,(ExecDup)IsDup );
	UFC::BufferedLog::Printf( " -------------------- Sent Fill Execution ------------------------" );

	return TRUE;
}
//---------------------------------------------------------------------------
//
// HSufx Sending functions (->)
//
//---------------------------------------------------------------------------
const char* HSufxObjFutOpt::HSufxTo_ExchangeCode( const UFC::AnsiString& exchange_type )
{
	if ( exchange_type == "1" )
		return "SSE";
	else if ( exchange_type == "F1" )
		return "CZCE";
	else if ( exchange_type == "F2" )
		return "DCE";
	else if ( exchange_type == "F3" )
		return "SHFE";
	else if ( exchange_type == "F4" )
		return "CFFEX";
	else if ( exchange_type == "0" )
		return "UNKNOW";
	else
		return "UNDEFINE";
}
//---------------------------------------------------------------------------
const char* HSufxObjFutOpt::HSufxTo_exchange_type( const UFC::AnsiString& ExchangeCode )
{
	if ( ExchangeCode == "SSE" )
		return "1";
	else if ( ExchangeCode == "CZCE" )
		return "F1";
	else if ( ExchangeCode == "DCE" )
		return "F2";
	else if ( ExchangeCode == "SHFE" )
		return "F3";
	else if ( ExchangeCode == "CFFEX" )
		return "F4";
	else ///< "UNKNOW"
		return "0";
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxTo_CTPExchTime( UFC::AnsiString& StrTime, int curr_time /*HHMMSSmmm*/ )
{
	int  hh, mm, ss;
	hh = curr_time / 10000000;
	mm = (curr_time / 100000) % 100;
	ss = (curr_time / 1000) % 100;
	StrTime.Printf( "%02d:%02d:%02d", hh, mm, ss );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::ClearQryOrderVector( void )
{
	for ( int i = 0; i < FHSufxQryOrderVector.size(); i++ )
	{
		delete FHSufxQryOrderVector[i];
	}
	FHSufxQryOrderVector.clear();
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::ClearQryFillVector( void )
{
	for ( int i = 0; i < FHSufxQryFillVector.size(); i++ )
	{
		delete FHSufxQryFillVector[i];
	}
	FHSufxQryFillVector.clear();
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::ClearRemainReportList(void )
{
	for ( int i = 0; i < FHSufxRemainReportList.ItemCount(); i++ )
	{
		delete FHSufxRemainReportList[i];
	}
	FHSufxRemainReportList.Clear();
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::ClearCancelList( void )
{
	for ( int i = 0; i < FHSufxCancelAfterFillList.ItemCount(); i++ )
	{
		delete FHSufxCancelAfterFillList[i];
	}
	FHSufxCancelAfterFillList.Clear();
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::DoQryOrderVector( void )
{
	sort( FHSufxQryOrderVector.begin(), FHSufxQryOrderVector.end(), HSufxFuncSortQryOrder );
	for ( int i = 0; i < FHSufxQryOrderVector.size(); i++ )
	{
		HSufxQryOrderResult( FHSufxQryOrderVector[i] );
	}
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::DoQryFillVector( void )
{
	sort( FHSufxQryFillVector.begin(), FHSufxQryFillVector.end(), HSufxFuncSortQryFill );
	for ( int i = 0; i < FHSufxQryFillVector.size(); i++ )
	{
		HSufxQryFillResult( FHSufxQryFillVector[i] );
	}
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::DoCancelList( void )
{
	for ( int i = 0; i < FHSufxCancelAfterFillList.ItemCount(); i++ )
	{
		// TrigerOnExecutionReport
		TExecutionReportMessage* ExecutionReportPtr = FHSufxCancelAfterFillList[i];
		bool IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReportPtr->GetMarket(), ExecutionReportPtr->GetExecID() );
		FSpeedyConnPtr->TrigerOnExecutionReport( ExecutionReportPtr, (ExecDup)IsDup );
	}
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::QryOrderFinishProcess( void )
{
	// 1. Do HSufxQryOrderResult in FHSufxQryOrderVector
	DoQryOrderVector( );
	// 2. Clear FHSufxQryOrderVector
	ClearQryOrderVector( );
	// 3. Send QryOrderFill
	HSufxSendQryOrderFill( );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::QryFillFinishProcess( void )
{
	// 1. Do HSufxQryFillResult in FHSufxQryFillVector
	DoQryFillVector( );
	// 2. Clear FHSufxQryFillVector
	ClearQryFillVector( );
	// 3. Do Cancel after Fill List to TrigerOnExecutionReport
	DoCancelList( );
	// 4. Clear FHSufxCancelAfterFillList
	ClearCancelList( );
	// 5. Trigger IOrderConnectionEventListener
	if ( FSpeedyConnPtr->FListener != NULL )
	{
		// 1.2 Trigger IOrderConnectionEventListener::OnLogonReply()
		FSpeedyConnPtr->FIsLogon = true;
		FSpeedyConnPtr->FListener->OnLogonReply( "", lrOk, FSpeedyConnPtr->FCurrentConnectionID );
		// 1.3 Trigger IOrderConnectionEventListener::OnRecoverFinished()
		FSpeedyConnPtr->FListener->OnRecoverFinished( FSpeedyConnPtr->FRecoverCount );
	}
	// 6. Send QryExchStatus & QryExchTime & SubscribeExchStatus
	HSufxSendQryExchStatus( );
	HSufxSendQryExchTime( );
	HSufxSendSubscribeExchStatus( );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::AddRemainReport( HSufxFutOpt::dataReportConfirm& Data )
{
	HSufxRemainReport* RemainPtr = new HSufxRemainReport();
	RemainPtr->TickMs = UFC::GetTickCountMS( );
	RemainPtr->Type   = 'C'; ///< Confirm
	RemainPtr-> ReportData = (void*)(new HSufxFutOpt::dataReportConfirm( Data ));
	FHSufxRemainReportList.Add( RemainPtr );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::AddRemainReport( HSufxFutOpt::dataReportFill& Data )
{
	HSufxRemainReport* RemainPtr = new HSufxRemainReport();
	RemainPtr->TickMs = UFC::GetTickCountMS( );
	RemainPtr->Type   = 'F'; ///< Fill
	RemainPtr-> ReportData = (void*)(new HSufxFutOpt::dataReportFill( Data ));
	FHSufxRemainReportList.Add( RemainPtr );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::CheckRemainReport( void )
{
	for ( int i = 0; i < FHSufxCancelAfterFillList.ItemCount(); i++ )
	{
		HSufxRemainReport* RemainPtr = FHSufxRemainReportList[i];
		if ( HSufxPushReportReDo( RemainPtr ) == TRUE )
		{
			FHSufxRemainReportList.Delete( i );
			delete RemainPtr;
		}
		else if ( UFC::GetTickCountMS() - RemainPtr->TickMs > 60000 ) ///< delete remain data after 60 seconds even no redo
		{
			FHSufxRemainReportList.Delete( i );
			delete RemainPtr;
		}
	}
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxReplyHeartBeatMsg( HSufxFutOpt::dataReplyHeartBeatMsg* Data )
{
	IBizMessage* lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxReplyHeartBeatMsg() NewBizMessage failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetBuff( Data->Buffer, Data->BufferLen );
	int RtnVal = FHSufxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	if ( RtnVal < 0 )
	{
		UTF8String ErrMsgUTF8( AnsiToUtf8Ex(FHSufxConnection->GetErrorMsg(RtnVal), 936) );
		UFC::BufferedLog::Printf( " HSufxReplyHeartBeatMsg SendBizMsg(620000) UserID[%s] BranchNo[%d] return error, Code[%d] Msg[%s]"
								  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal, ErrMsgUTF8.c_str() );
		return;
	}
	UFC::BufferedLog::Printf( " HSufxReplyHeartBeatMsg SendBizMsg(620000) to HS UFX Server OK." );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxSendSubscribePush( void )
{
	int iIssueType = 33101;
	int RtnVal;

	// 1. New T2SDK BizMessage
	IBizMessage* lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendSubscribePush(%d) NewBizMessage failed!!", iIssueType );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 620001 );
	lpBizMessage->SetIssueType( iIssueType );
	lpBizMessage->SetSystemNo( FHSufxSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 2. New T2SDK Packet
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2);
	if ( pPacker == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendSubscribePush(%d) NewPacker failed!!", iIssueType );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();

	// 3. Add T2SDK Packet Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );	///< NonRequire
	pPacker->AddField( "op_entrust_way", 'C', 1 );	///< NonRequire
	pPacker->AddField( "op_station", 'S', 255 );	///< NonRequire
	pPacker->AddField( "branch_no", 'I', 5 );		///< Require
	pPacker->AddField( "password", 'S', 50 );		///< NonRequire
	pPacker->AddField( "user_token", 'S', 512 );	///< NonRequire
	pPacker->AddField( "issue_type", 'I', 8 );		///< Require
	pPacker->AddField( "fund_account", 'S', 18 );	///< Require

	// 4. Add T2SDK Packet Field Value
	pPacker->AddInt( FHSufxBranchNo );						///< op_branch_no
	pPacker->AddChar( '7' );								///< op_entrust_way		// ?? from config ??
	pPacker->AddStr( "1" );									///< op_station
	pPacker->AddInt( FHSufxBranchNo );						///< branch_no
	pPacker->AddStr( FSpeedyConnPtr->FPassword.c_str() );	///< password
	pPacker->AddStr( FHSufxUserToken );						///< user_token
	pPacker->AddInt( iIssueType );							///< issue_type
	pPacker->AddStr( FSpeedyConnPtr->FID.c_str() );			///< fund_account
	pPacker->EndPack();

	// 5. Send T2SDK Packet
	lpBizMessage->SetKeyInfo( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FHSufxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		UTF8String ErrMsgUTF8( AnsiToUtf8Ex(FHSufxConnection->GetErrorMsg(RtnVal), 936) );
		UFC::BufferedLog::Printf( " HSufxSendSubscribePush(%d) SendBizMsg(620001) UserID[%s] BranchNo[%d] return error, Code[%d] Msg[%s]"
								  , iIssueType, FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal, ErrMsgUTF8.c_str() );
		return;
	}
	UFC::BufferedLog::Printf( " HSufxSendSubscribePush(%d) SendBizMsg(620001) OK. UserID[%s] BranchNo[%d] RtnVal[%d]"
							  , iIssueType, FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxSendQryOrderConfirm( const char* position_str )
{
	int RtnVal;

	// 1. New T2SDK BizMessage
	IBizMessage* lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendQryOrderConfirm() NewBizMessage failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 338301 );
	lpBizMessage->SetSystemNo( FHSufxSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 2. New T2SDK Packet
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2);
	if ( pPacker == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendQryOrderConfirm() NewPacker failed!!" );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();

	// 3. Add T2SDK Packet Field Name
	pPacker->AddField( "user_token", 'S', 512 );	///< Require
	pPacker->AddField( "op_branch_no", 'I', 5 );	///< Require
	pPacker->AddField( "op_entrust_way", 'C', 1 );	///< Require
	pPacker->AddField( "op_station", 'S', 255 );	///< Require
	pPacker->AddField( "branch_no", 'I', 5 );		///< Require
	pPacker->AddField( "client_id", 'S', 18 );		///< Require
	pPacker->AddField( "fund_account", 'S', 18 );	///< Require
	pPacker->AddField( "password", 'S', 50 );		///< Require
	//pPacker->AddField( "futu_exch_type", 'S', 4 );	///< NonRequire
	pPacker->AddField( "query_direction", 'C', 1 );	///< NonRequire
	pPacker->AddField( "sort_direction", 'C', 1 );	///< NonRequire
	pPacker->AddField( "request_num", 'I', 10 );	///< NonRequire
	pPacker->AddField( "position_str", 'S', 100 );	///< NonRequire

	// 4. Add T2SDK Packet Field Value
	pPacker->AddStr( FHSufxUserToken );						///< user_token
	pPacker->AddInt( FHSufxBranchNo );						///< op_branch_no
	pPacker->AddChar( '7' );								///< op_entrust_way		// ?? from config ??
	pPacker->AddStr( "1" );									///< op_station
	pPacker->AddInt( FHSufxBranchNo );						///< branch_no
	pPacker->AddStr( FHSufxClientID );						///< client_id
	pPacker->AddStr( FSpeedyConnPtr->FID.c_str() );			///< fund_account
	pPacker->AddStr( FSpeedyConnPtr->FPassword.c_str() );	///< password
	pPacker->AddChar( '0' );								///< query_direction
	pPacker->AddChar( '0' );								///< sort_direction
	pPacker->AddInt( FHSufxRequestNum );					///< request_num
	///< position_str
	if ( position_str )
		pPacker->AddStr( position_str );
	else
		pPacker->AddStr( "" );
	pPacker->EndPack();

	// 5. Send T2SDK Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FHSufxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		UTF8String ErrMsgUTF8( AnsiToUtf8Ex(FHSufxConnection->GetErrorMsg(RtnVal), 936) );
		UFC::BufferedLog::Printf( " HSufxSendQryOrderConfirm() SendBizMsg(338301) UserID[%s] BranchNo[%d] return error, Code[%d] Msg[%s]"
								  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal, ErrMsgUTF8.c_str() );
		return;
	}
	UFC::BufferedLog::Printf( " HSufxSendQryOrderConfirm() SendBizMsg(338301) UserID[%s] BranchNo[%d] RtnVal[%d]"
							  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxSendQryOrderFill( const char* position_str )
{
	int RtnVal;

	// 1. New T2SDK BizMessage
	IBizMessage* lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendQryOrderFill() NewBizMessage failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 338302 );
	lpBizMessage->SetSystemNo( FHSufxSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 2. New T2SDK Packet
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2);
	if ( pPacker == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendQryOrderFill() NewPacker failed!!" );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();

	// 3. Add T2SDK Packet Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );	///< Require
	pPacker->AddField( "op_entrust_way", 'C', 1 );	///< Require
	pPacker->AddField( "op_station", 'S', 255 );	///< Require
	pPacker->AddField( "branch_no", 'I', 5 );		///< Require
	pPacker->AddField( "client_id", 'S', 18 );		///< Require
	pPacker->AddField( "fund_account", 'S', 18 );	///< Require
	pPacker->AddField( "password", 'S', 50 );		///< Require
	//pPacker->AddField( "futu_exch_type", 'S', 4 );	///< NonRequire
	pPacker->AddField( "query_mode", 'C', 1 );		///< NonRequire
	pPacker->AddField( "query_direction", 'C', 1 );	///< NonRequire
	pPacker->AddField( "sort_direction", 'C', 1 );	///< NonRequire
	pPacker->AddField( "request_num", 'I', 10 );	///< NonRequire
	pPacker->AddField( "position_str", 'S', 100 );	///< NonRequire

	// 4. Add T2SDK Packet Field Value
	pPacker->AddInt( FHSufxBranchNo );						///< op_branch_no
	pPacker->AddChar( '7' );								///< op_entrust_way		// ?? from config ??
	pPacker->AddStr( "1" );									///< op_station
	pPacker->AddInt( FHSufxBranchNo );						///< branch_no
	pPacker->AddStr( FHSufxClientID );						///< client_id
	pPacker->AddStr( FSpeedyConnPtr->FID.c_str() );			///< fund_account
	pPacker->AddStr( FSpeedyConnPtr->FPassword.c_str() );	///< password
	pPacker->AddChar( '0' );								///< query_mode //明細
	pPacker->AddChar( '0' );								///< query_direction
	pPacker->AddChar( '0' );								///< sort_direction
	pPacker->AddInt( FHSufxRequestNum );					///< request_num
	///< position_str
	if ( position_str )
		pPacker->AddStr( position_str );
	else
		pPacker->AddStr( "" );
	pPacker->EndPack();

	// 5. Send T2SDK Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FHSufxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		UTF8String ErrMsgUTF8( AnsiToUtf8Ex(FHSufxConnection->GetErrorMsg(RtnVal), 936) );
		UFC::BufferedLog::Printf( " HSufxSendQryOrderFill() SendBizMsg(338302) UserID[%s] BranchNo[%d] return error, Code[%d] Msg[%s]"
								  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal, ErrMsgUTF8.c_str() );
		return;
	}
	UFC::BufferedLog::Printf( " HSufxSendQryOrderFill() SendBizMsg(338302) UserID[%s] BranchNo[%d] RtnVal[%d]"
							  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxSendQryExchStatus( void )
{
	int RtnVal;

	// 1. New T2SDK BizMessage
	IBizMessage* lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendQryExchStatus() NewBizMessage failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 338313 );
	lpBizMessage->SetSystemNo( FHSufxSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 2. New T2SDK Packet
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2);
	if ( pPacker == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendQryExchStatus() NewPacker failed!!" );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();

	// 3. Add T2SDK Packet Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );	///< Require
	pPacker->AddField( "op_entrust_way", 'C', 1 );	///< Require
	pPacker->AddField( "op_station", 'S', 255 );	///< Require
	//pPacker->AddField( "futu_exch_type", 'S', 4 );	///< NonRequire

	// 4. Add T2SDK Packet Field Value
	pPacker->AddInt( FHSufxBranchNo );						///< op_branch_no
	pPacker->AddChar( '7' );								///< op_entrust_way		// ?? from config ??
	pPacker->AddStr( "1" );									///< op_station
	//pPacker->AddStr( "F4" );								///< futu_exch_type F1-CZCE F2-DCE F3-SHFE F4-CFFEX
	pPacker->EndPack();

	// 5. Send T2SDK Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FHSufxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		UTF8String ErrMsgUTF8( AnsiToUtf8Ex(FHSufxConnection->GetErrorMsg(RtnVal), 936) );
		UFC::BufferedLog::Printf( " HSufxSendQryExchStatus() SendBizMsg(338313) return error, Code[%d] Msg[%s]", RtnVal, ErrMsgUTF8.c_str() );
		return;
	}
	UFC::BufferedLog::Printf( " HSufxSendQryExchStatus() SendBizMsg(338313) RtnVal[%d]", RtnVal );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxSendQryExchTime( void )
{
	int RtnVal;

	// 1. New T2SDK BizMessage
	IBizMessage* lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendQryExchTime() NewBizMessage failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 330852 );
	lpBizMessage->SetSystemNo( FHSufxSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 2. New T2SDK Packet
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2);
	if ( pPacker == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendQryExchTime() NewPacker failed!!" );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();

	// 3. Add T2SDK Packet Field Name
	pPacker->AddField( "user_token", 'S', 40 );		///< Require
	pPacker->AddField( "op_branch_no", 'I', 5 );	///< Require
	pPacker->AddField( "op_entrust_way", 'C', 1 );	///< Require
	pPacker->AddField( "op_station", 'S', 255 );	///< Require
	pPacker->AddField( "branch_no", 'I', 5 );		///< Require
	pPacker->AddField( "client_id", 'S', 18 );		///< Require
	pPacker->AddField( "fund_account", 'S', 18 );	///< Require
	pPacker->AddField( "password", 'S', 50 );		///< Require

	// 4. Add T2SDK Packet Field Value
	pPacker->AddStr( FHSufxUserToken );						///< user_token
	pPacker->AddInt( FHSufxBranchNo );						///< op_branch_no
	pPacker->AddChar( '7' );								///< op_entrust_way		// ?? from config ??
	pPacker->AddStr( "1" );									///< op_station
	pPacker->AddInt( FHSufxBranchNo );						///< branch_no
	pPacker->AddStr( FHSufxClientID );						///< client_id
	pPacker->AddStr( FSpeedyConnPtr->FID.c_str() );			///< fund_account
	pPacker->AddStr( FSpeedyConnPtr->FPassword.c_str() );	///< password
	pPacker->EndPack();

	// 5. Send T2SDK Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FHSufxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		UTF8String ErrMsgUTF8( AnsiToUtf8Ex(FHSufxConnection->GetErrorMsg(RtnVal), 936) );
		UFC::BufferedLog::Printf( " HSufxSendQryExchTime() SendBizMsg(330852) return error, Code[%d] Msg[%s]", RtnVal, ErrMsgUTF8.c_str() );
		return;
	}
	UFC::BufferedLog::Printf( " HSufxSendQryExchTime() SendBizMsg(330852) RtnVal[%d]", RtnVal );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxSendSubscribeExchStatus( void )
{
	int iIssueType = 33105;
	int RtnVal;

	// 1. New T2SDK BizMessage
	IBizMessage* lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendSubscribeExchStatus(%d) NewBizMessage failed!!", iIssueType );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 620001 );
	lpBizMessage->SetIssueType( iIssueType );
	lpBizMessage->SetSystemNo( FHSufxSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 2. New T2SDK Packet
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2);
	if ( pPacker == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendSubscribeExchStatus(%d) NewPacker failed!!", iIssueType );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();

	// 3. Add T2SDK Packet Field Name
	//pPacker->AddField( "op_branch_no", 'I', 5 );	///< NonRequire
	//pPacker->AddField( "op_entrust_way", 'C', 1 );	///< NonRequire
	//pPacker->AddField( "op_station", 'S', 255 );	///< NonRequire
	//pPacker->AddField( "branch_no", 'I', 5 );		///< NonRequire
	//pPacker->AddField( "client_id", 'S', 18 );		///< NonRequire
	pPacker->AddField( "fund_account", 'S', 18 );	///< Require
	//pPacker->AddField( "password", 'S', 50 );		///< NonRequire
	//pPacker->AddField( "user_token", 'S', 512 );	///< NonRequire
	pPacker->AddField( "issue_type", 'I', 8 );		///< Require
	pPacker->AddField( "futu_exch_type", 'S', 4 );	///< Require

	// 4. Add T2SDK Packet Field Value
	pPacker->AddStr( FSpeedyConnPtr->FID.c_str() );			///< fund_account
	pPacker->AddInt( iIssueType );							///< issue_type
	pPacker->AddStr( "F1" );								///< futu_exch_type F1-CZCE F2-DCE F3-SHFE F4-CFFEX
	pPacker->AddStr( FSpeedyConnPtr->FID.c_str() );			///< fund_account
	pPacker->AddInt( iIssueType );							///< issue_type
	pPacker->AddStr( "F2" );								///< futu_exch_type F1-CZCE F2-DCE F3-SHFE F4-CFFEX
	pPacker->AddStr( FSpeedyConnPtr->FID.c_str() );			///< fund_account
	pPacker->AddInt( iIssueType );							///< issue_type
	pPacker->AddStr( "F3" );								///< futu_exch_type F1-CZCE F2-DCE F3-SHFE F4-CFFEX
	pPacker->AddStr( FSpeedyConnPtr->FID.c_str() );			///< fund_account
	pPacker->AddInt( iIssueType );							///< issue_type
	pPacker->AddStr( "F4" );								///< futu_exch_type F1-CZCE F2-DCE F3-SHFE F4-CFFEX
	pPacker->EndPack();

	// 5. Send T2SDK Packet
	lpBizMessage->SetKeyInfo( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FHSufxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		UTF8String ErrMsgUTF8( AnsiToUtf8Ex(FHSufxConnection->GetErrorMsg(RtnVal), 936) );
		UFC::BufferedLog::Printf( " HSufxSendSubscribeExchStatus(%d) SendBizMsg(620001) UserID[%s] return error, Code[%d] Msg[%s]"
								  , iIssueType, FSpeedyConnPtr->FID.c_str(), RtnVal, ErrMsgUTF8.c_str() );
		return;
	}
	UFC::BufferedLog::Printf( " HSufxSendSubscribeExchStatus(%d) SendBizMsg(620001) OK. UserID[%s] RtnVal[%d]"
							  , iIssueType, FSpeedyConnPtr->FID.c_str(), RtnVal );
}
//---------------------------------------------------------------------------
int HSufxObjFutOpt::HSufxSendChangePwd( TNewsMessage* Msg )
{
	int RtnVal;

	// 1. Get news parameters
	UFC::AnsiString        OldPassword;
	UFC::AnsiString        NewPassword;
	UFC::NameValueMessage  Params("^\n");
	Params.FromString( Msg->GetText() );
	if ( Params.Get( "OldPassword", OldPassword )== false || Params.Get( "NewPassword", NewPassword )== false )
	{
		UFC::BufferedLog::Printf( " HSufxSendChangePwd() Params.Get(OldPassword) or Params.Get(NewPassword) failed!! " );
		return -1;
	}

	// 2. New T2SDK BizMessage
	IBizMessage* lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendChangePwd() NewBizMessage failed!!" );
		return -1;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 331101 );
	lpBizMessage->SetSystemNo( FHSufxSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 3. New T2SDK Packet
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2);
	if ( pPacker == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendChangePwd() NewPacker failed!!" );
		lpBizMessage->Release();
		return -1;
	}
	pPacker->AddRef();
	pPacker->BeginPack();

	// 4. Add T2SDK Packet Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );	///< Require
	pPacker->AddField( "op_entrust_way", 'C', 1 );	///< Require
	pPacker->AddField( "op_station", 'S', 255 );	///< Require
	pPacker->AddField( "branch_no", 'I', 5 );		///< Require
	pPacker->AddField( "client_id", 'S', 18 );		///< Require
	pPacker->AddField( "fund_account", 'S', 18 );	///< Require
	pPacker->AddField( "password", 'S', 50 );		///< Require
	pPacker->AddField( "password_type", 'C', 1 );	///< NonRequire
	pPacker->AddField( "user_token", 'S', 512 );	///< NonRequire
	pPacker->AddField( "new_password", 'S', 10 );	///< Require

	// 5. Add T2SDK Packet Field Value
	pPacker->AddInt( FHSufxBranchNo );			///< op_branch_no
	pPacker->AddChar( '7' );					///< op_entrust_way		// ?? from config ??
	pPacker->AddStr( "1" );						///< op_station
	pPacker->AddInt( FHSufxBranchNo );			///< branch_no
	pPacker->AddStr( FHSufxClientID );			///< client_id
	pPacker->AddStr( FSpeedyConnPtr->FID.c_str() );///< fund_account
	pPacker->AddStr( OldPassword.c_str() );		///< password
	pPacker->AddChar( '2' );					///< password_type
	pPacker->AddStr( FHSufxUserToken );			///< user_token
	pPacker->AddStr( NewPassword.c_str() );		///< new_password
	pPacker->EndPack();

	// 5. Send T2SDK Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FHSufxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		UTF8String ErrMsgUTF8( AnsiToUtf8Ex(FHSufxConnection->GetErrorMsg(RtnVal), 936) );
		UFC::BufferedLog::Printf( " HSufxSendChangePwd() SendBizMsg(331101) UserID[%s] BranchNo[%d] return error, Code[%d] Msg[%s]"
								  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal, ErrMsgUTF8.c_str() );
		return -1;
	}
	UFC::BufferedLog::Printf( " HSufxSendChangePwd() SendBizMsg(331101) UserID[%s] BranchNo[%d] RtnVal[%d]"
							  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal );

	return 0;
}
//---------------------------------------------------------------------------
int HSufxObjFutOpt::HSufxSendQryPosition( const char* position_str )
{
	int RtnVal;

	// 1. Get news parameters
	// no parameter need to get

	// 2. New T2SDK BizMessage
	IBizMessage* lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendQryPosition() NewBizMessage failed!!" );
		return -1;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 338303 );
	lpBizMessage->SetSystemNo( FHSufxSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 3. New T2SDK Packet
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2);
	if ( pPacker == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendQryPosition() NewPacker failed!!" );
		lpBizMessage->Release();
		return -1;
	}
	pPacker->AddRef();
	pPacker->BeginPack();

	// 4. Add T2SDK Packet Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );	///< Require
	pPacker->AddField( "op_entrust_way", 'C', 1 );	///< Require
	pPacker->AddField( "op_station", 'S', 255 );	///< Require
	pPacker->AddField( "branch_no", 'I', 5 );		///< Require
	pPacker->AddField( "client_id", 'S', 18 );		///< Require
	pPacker->AddField( "fund_account", 'S', 18 );	///< Require
	pPacker->AddField( "password", 'S', 50 );		///< Require
	//pPacker->AddField( "futu_exch_type", 'S', 4 );	///< NonRequire
	pPacker->AddField( "query_mode", 'C', 1 );		///< NonRequire
	pPacker->AddField( "request_num", 'I', 10 );	///< NonRequire
	pPacker->AddField( "position_str", 'S', 100 );	///< NonRequire

	// 5. Add T2SDK Packet Field Value
	pPacker->AddInt( FHSufxBranchNo );						///< op_branch_no
	pPacker->AddChar( '7' );								///< op_entrust_way		// ?? from config ??
	pPacker->AddStr( "1" );									///< op_station
	pPacker->AddInt( FHSufxBranchNo );						///< branch_no
	pPacker->AddStr( FHSufxClientID );						///< client_id
	pPacker->AddStr( FSpeedyConnPtr->FID.c_str() );			///< fund_account
	pPacker->AddStr( FSpeedyConnPtr->FPassword.c_str() );	///< password
	pPacker->AddChar( '1' );								///< query_mode //不過濾持倉為0的數據
	pPacker->AddInt( FHSufxRequestNum );					///< request_num
	///< position_str
	if ( position_str )
		pPacker->AddStr( position_str );
	else
		pPacker->AddStr( "" );
	pPacker->EndPack();

	// 6. Send T2SDK Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FHSufxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		UTF8String ErrMsgUTF8( AnsiToUtf8Ex(FHSufxConnection->GetErrorMsg(RtnVal), 936) );
		UFC::BufferedLog::Printf( " HSufxSendQryPosition() SendBizMsg(338303) UserID[%s] BranchNo[%d] return error, Code[%d] Msg[%s]"
								  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal, ErrMsgUTF8.c_str() );
		return -1;
	}
	UFC::BufferedLog::Printf( " HSufxSendQryPosition() SendBizMsg(338303) UserID[%s] BranchNo[%d] RtnVal[%d]"
							  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal );

	return 0;
}
//---------------------------------------------------------------------------
int HSufxObjFutOpt::HSufxSendQryAssert( TNewsMessage* /*Msg*/ )
{
	int RtnVal;

	// 1. Get news parameters
	// no parameter need to get

	// 2. New T2SDK BizMessage
	IBizMessage* lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendQryAssert() NewBizMessage failed!!" );
		return -1;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 338300 );
	lpBizMessage->SetSystemNo( FHSufxSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 3. New T2SDK Packet
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2);
	if ( pPacker == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendQryAssert() NewPacker failed!!" );
		lpBizMessage->Release();
		return -1;
	}
	pPacker->AddRef();
	pPacker->BeginPack();

	// 4. Add T2SDK Packet Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );	///< Require
	pPacker->AddField( "op_entrust_way", 'C', 1 );	///< Require
	pPacker->AddField( "op_station", 'S', 255 );	///< Require
	pPacker->AddField( "branch_no", 'I', 5 );		///< Require
	pPacker->AddField( "client_id", 'S', 18 );		///< Require
	pPacker->AddField( "fund_account", 'S', 18 );	///< Require
	pPacker->AddField( "password", 'S', 50 );		///< Require
	pPacker->AddField( "user_token", 'S', 512 );	///< Require
	//pPacker->AddField( "money_type", 'S', 3 );	///< NonRequire

	// 5. Add T2SDK Packet Field Value
	pPacker->AddInt( FHSufxBranchNo );						///< op_branch_no
	pPacker->AddChar( '7' );								///< op_entrust_way		// ?? from config ??
	pPacker->AddStr( "1" );									///< op_station
	pPacker->AddInt( FHSufxBranchNo );						///< branch_no
	pPacker->AddStr( FHSufxClientID );						///< client_id
	pPacker->AddStr( FSpeedyConnPtr->FID.c_str() );			///< fund_account
	pPacker->AddStr( FSpeedyConnPtr->FPassword.c_str() );	///< password
	pPacker->AddStr( FHSufxUserToken );						///< user_token
	pPacker->EndPack();

	// 6. Send T2SDK Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FHSufxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		UTF8String ErrMsgUTF8( AnsiToUtf8Ex(FHSufxConnection->GetErrorMsg(RtnVal), 936) );
		UFC::BufferedLog::Printf( " HSufxSendQryAssert() SendBizMsg(338300) UserID[%s] BranchNo[%d] return error, Code[%d] Msg[%s]"
								  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal, ErrMsgUTF8.c_str() );
		return -1;
	}
	UFC::BufferedLog::Printf( " HSufxSendQryAssert() SendBizMsg(338300) UserID[%s] BranchNo[%d] RtnVal[%d]"
							  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal );

	return 0;
}
//---------------------------------------------------------------------------
COrderInfo* HSufxObjFutOpt::HSufxAddPendingOrder( int OrdRef, TNewOrderMessage* Ord, const UFC::AnsiString& UserDefine )
{
	COrderInfo* NewOrd = new COrderInfo( 0, FHSufxSessionNo, Ord, UserDefine );
	UFC::AnsiString OrderRefKey;

	OrderRefKey.Printf( "%d|%d|%d", 0, FHSufxSessionNo, OrdRef );
	FSpeedyConnPtr->FOrderRecords.Add( NewOrd );
	FSpeedyConnPtr->FOrderRefToRecord.Add( OrderRefKey, NewOrd );
	return NewOrd;
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxInitial( void )
{
	// 1. Create CConfigInterface
	//通過T2SDK的引出函數，來獲取一個新的CConfig對象
	//此對象在創建連接對象時被傳遞，用於配置所創建的連接對象的各種屬性（比如服務器IP地址、安全模式等）
	//值得注意的是，在向配置對象設置配置信息時，配置信息既可以從ini文件中載入，
	//也可以在程序代碼中設定，或者是2者的混合，如果對同一個配置項設不同的值，則以最近一次設置為準
	FHSufxConfig = gHSufxAPI.pNewConfig();
	UFC::BufferedLog::Printf( " Create HSufx CConfigInterface OK." );

	//通過T2SDK的引出函數NewXXXX返回的對象，需要調用對象的Release方法釋放，而不能直接用delete
	//因為t2sdk.dll和調用程序可能是由不同的編譯器、編譯模式生成，delete可能會導致異常
	//為了適合Delphi等使用（Delphi對接口自動調用AddRef方法），用C/C++開發的代碼，需要在NewXXXX之後調用一下AddRef
	//以保證引用計數正確
	FHSufxConfig->AddRef();
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxRelease( void )
{
	// 1. FHSufxConnection
	FHSufxConnection->Release();
	FHSufxConnection = NULL;
	// 3. FHSufxConfig
	FHSufxConfig->Release();
	FHSufxConfig = NULL;
}
//---------------------------------------------------------------------------
BOOL HSufxObjFutOpt::HSufxConnect( const char* strT2Servers, int TimeoutSec )
{
	int RtnVal;

	// 0. Clear all container for reConnect if any exception
	ClearCancelList( );
	ClearQryFillVector( );
	ClearQryOrderVector( );

	// 1. Set HSufx Config
	///可以在代碼中對lpConfig對像中的具體值進行設置
	//設置以最後一次為準
	//[t2sdk] servers指定需要連接的IP地址及端口，可配置多個，中間以“;”間隔//111.207.179.6:8066  18.8.1.171:8066
	FHSufxConfig->SetString( "t2sdk", "servers", strT2Servers );
	FHSufxConfig->SetString( "t2sdk", "send_queue_size", "100" );
	//FHSufxConfig->SetString( "t2sdk", "support_multi", "1" ); ///< support multi-thread
	FHSufxConfig->SetString( "t2sdk", "enable_nagle", "1" ); ///< disable nagle algorithm
	//FHSufxConfig->SetString( "t2sdk", "heartbeat_time", "5" ); ///< enable client side send heartbeat second
	//FHSufxConfig->SetString( "t2sdk", "event_count", "100" );
	////////////////////
	FHSufxConfig->SetString( "t2sdk", "license_file", "HSt2sdkFutLicense.dat" );
//	FHSufxConfig->SetString( "t2sdk", "license_pwd", "123456" );
	FHSufxConfig->SetString( "safe",  "safe_level", "none" ); ///< "none" "ssl"
//	FHSufxConfig->SetString( "safe",  "cert_file", "c20121011.pfx" );
//	FHSufxConfig->SetString( "safe",  "cert_pwd", "111111" );

	// 2. Crete CConnectionInterface
	if ( FHSufxConnection == NULL )
	{
		FHSufxConnection = gHSufxAPI.pNewConnection( FHSufxConfig );
		UFC::BufferedLog::Printf( " Create HSufx CConnectionInterface OK." );
		FHSufxConnection->AddRef();
		//RtnVal = FHSufxConnection->CreateEx( this );
		RtnVal = FHSufxConnection->Create2BizMsg( this );
		if ( RtnVal != 0 )
		{
			UTF8String ErrMsgUTF8( AnsiToUtf8Ex(FHSufxConnection->GetErrorMsg(RtnVal), 936) );
			UFC::BufferedLog::Printf( " FHSufxConnection->Create2BizMsg() failed. ErrorCode[%d] ErrorMsg[%s]", RtnVal, ErrMsgUTF8.c_str() );
			throw UFC::Exception( " FHSufxConnection->Create2BizMsg() failed." );
		}
		UFC::BufferedLog::Printf( " FHSufxConnection->Create2BizMsg() OK." );
	}

	// 3. Connect
	if ( FHSufxConnection->GetStatus() & CConnectionInterface::Connected )
	{
		UFC::BufferedLog::Printf( " FHSufxConnection->Connect() already connected OK. UserID[%s] RtnVal[%d]", FSpeedyConnPtr->FID.c_str(), RtnVal );
		if ( FSpeedyConnPtr->FListener )
			FSpeedyConnPtr->FListener->OnConnected();
		return TRUE;
	}
	else
	{
		RtnVal = FHSufxConnection->Connect( TimeoutSec*1000 );
		if ( RtnVal != 0 )
		{
			UTF8String ErrMsgUTF8( AnsiToUtf8Ex(FHSufxConnection->GetErrorMsg(RtnVal), 936) );
			UFC::BufferedLog::Printf( " FHSufxConnection->Connect() failed. ErrorCode[%d] ErrorMsg[%s]", RtnVal, ErrMsgUTF8.c_str() );
			if ( FSpeedyConnPtr->FListener )
			{
				FSpeedyConnPtr->FListener->OnLogonReply( ErrMsgUTF8.c_str(), lrFailed, FSpeedyConnPtr->FCurrentConnectionID );
				FSpeedyConnPtr->FListener->OnDisconnected();
			}
			return FALSE;
		}
		UFC::BufferedLog::Printf( " FHSufxConnection->Connect() OK. UserID[%s] RtnVal[%d]", FSpeedyConnPtr->FID.c_str(), RtnVal );
		FSpeedyConnPtr->FIsConnected = true;
		if ( FSpeedyConnPtr->FListener )
			FSpeedyConnPtr->FListener->OnConnected();
	}
	return TRUE;
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxSendLogin( void )
{
	int RtnVal;

	// 1. New T2SDK BizMessage
	IBizMessage* lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendLogin() NewBizMessage failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 331100 );
	//lpBizMessage->SetSystemNo( 0 );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 2. New T2SDK Packet
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendLogin() NewPacker failed!!" );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();

	// 3. Add T2SDK Packet Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );	///< Require
	pPacker->AddField( "op_entrust_way", 'C', 1 );	///< Require
	pPacker->AddField( "op_station", 'S', 255 );	///< Require
	pPacker->AddField( "branch_no", 'I', 5 );		///< NonRequire
	pPacker->AddField( "password", 'S', 50 );		///< Require
	pPacker->AddField( "password_type", 'C', 1 );	///< NonRequire
	pPacker->AddField( "input_content", 'C', 1 );	///< Require
	pPacker->AddField( "account_content", 'S', 30 );///< Require
	pPacker->AddField( "content_type", 'S', 6 );	///< Require
	// 4. Add T2SDK Packet Field Value
	pPacker->AddInt( 0 );									///< op_branch_no
	pPacker->AddChar( '7' );								///< op_entrust_way		// ?? from config ??
	pPacker->AddStr( "1" );									///< op_station
	pPacker->AddInt( 0 );									///< branch_no
	pPacker->AddStr( FSpeedyConnPtr->FPassword.c_str() );	///< password
	pPacker->AddChar( '2' );								///< password_type
	pPacker->AddChar( '1' );								///< input_content
	pPacker->AddStr( FSpeedyConnPtr->FID.c_str() );			///< account_content
	pPacker->AddStr( "0" );									///< content_type
	pPacker->EndPack();

	// 5. Send T2SDK Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FHSufxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		UTF8String ErrMsgUTF8( AnsiToUtf8Ex(FHSufxConnection->GetErrorMsg(RtnVal), 936) );
		UFC::BufferedLog::Printf( " HSufxSendLogin() SendBizMsg(331100) UserID[%s] BrokerID[%s] return error, Code[%d] Msg[%s]"
								  , FSpeedyConnPtr->FID.c_str(), TTaifexConnection::FCFFEXBrokerID.c_str(), RtnVal, ErrMsgUTF8.c_str() );
		return;
	}
	UFC::BufferedLog::Printf( " HSufxSendLogin() SendBizMsg(331100) OK. UserID[%s] BrokerID[%s] RtnVal[%d]"
							  , FSpeedyConnPtr->FID.c_str(), TTaifexConnection::FCFFEXBrokerID.c_str(), RtnVal );
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxSendLogout( void )
{
	// no implementation
	// 功能[331102]不存在
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxNewOrder( TNewOrderMessage* Msg, const UFC::AnsiString& UserDefine )
{
	UFC::AnsiString ExchangeType = HSufxTo_exchange_type( Msg->GetExchangeCode() );
	UFC::AnsiString ErrMsg;
	bool            CanSend = true;
	int             OrderRef;
	char            strNID[32+1];
	UFC::AnsiString StrOrderType;
	UFC::AnsiString StrTimeInForce;
	UFC::AnsiString StrEntrustProp;
	char charPE;

	UFC::BufferedLog::Printf( " -------------------- User Place New Order -----------------------" );
	sprintf( strNID, "%lld", Msg->GetNID() );

	// 1. New T2SDK BizMessage
	IBizMessage* lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxObjFutOpt::HSufxNewOrder() NewBizMessage failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 338202 );
	lpBizMessage->SetSystemNo( FHSufxSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 2. New T2SDK Packet
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2);
	if ( pPacker == NULL )
	{
		ErrMsg.Printf( "HSufxObjFutOpt::HSufxNewOrder() NewPacker failed!!" );
		FSpeedyConnPtr->Reject( nsOrderMessageDefine::crrNew, ErrMsg, Msg ,Msg->GetTradingSessionID());
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();

	// 3. Add T2SDK Packet Field Name
	pPacker->AddField( "user_token", 'S', 512 );			///< Require
	pPacker->AddField( "op_branch_no", 'I', 5 );			///< Require
	pPacker->AddField( "op_entrust_way", 'C', 1 );			///< Require
	pPacker->AddField( "op_station", 'S', 255 );			///< Require
	pPacker->AddField( "branch_no", 'I', 5 );				///< Require
	pPacker->AddField( "client_id", 'S', 18 );				///< Require
	pPacker->AddField( "fund_account", 'S', 18 );			///< Require
	pPacker->AddField( "password", 'S', 50 );				///< Require
	pPacker->AddField( "futu_exch_type", 'S', 4 );			///< Require
	pPacker->AddField( "futures_account", 'S', 12 );		///< Require
	pPacker->AddField( "futu_code", 'S', 30 );				///< Require
	pPacker->AddField( "entrust_bs", 'C', 1 );				///< Require
	pPacker->AddField( "futures_direction", 'C', 1 );		///< Require
	pPacker->AddField( "hedge_type", 'C', 1 );				///< Require
	pPacker->AddField( "entrust_amount", 'I' );				///< Require
	pPacker->AddField( "futu_entrust_price", 'F', 12, 6 );	///< Require
	pPacker->AddField( "entrust_prop", 'S', 3 );			///< NonRequire
	//pPacker->AddField( "entrust_occasion", 'S', 32 );		///< NonRequire
	pPacker->AddField( "entrust_reference", 'S', 32 );		///< NonRequire

	// 4. Add T2SDK Packet Field Value
	pPacker->AddStr( FHSufxUserToken );							///< user_token
	pPacker->AddInt( FHSufxBranchNo );							///< op_branch_no
	pPacker->AddChar( '7' );									///< op_entrust_way		// ?? from config ??
	pPacker->AddStr( "1" );										///< op_station
	pPacker->AddInt( FHSufxBranchNo );							///< branch_no
	pPacker->AddStr( FHSufxClientID );							///< client_id
	pPacker->AddStr( Msg->GetAccount() );						///< fund_account
	pPacker->AddStr( FSpeedyConnPtr->FPassword.c_str() );		///< password
	pPacker->AddStr( ExchangeType.c_str() );					///< futu_exch_type
	pPacker->AddStr( "" );										///< futures_account
	pPacker->AddStr( Msg->GetSymbol() );						///< futu_code
	// Side (B/S)  (entrust_bs)
	if ( Msg->GetSide() == nsOrderMessageDefine::sBuy )
		pPacker->AddChar( '1' );
	else
		pPacker->AddChar( '2' );
	// Position effect  (futures_direction) 1:Open 2:Close
	switch ( Msg->GetPositionEffect() )
	{
	case nsOrderMessageDefine::peOpen         : charPE = '1'; break;
	case nsOrderMessageDefine::peClose        : charPE = '2'; break;
	case nsOrderMessageDefine::peRolled       : charPE = '2'; break;
	case nsOrderMessageDefine::peDayTradeOpen : charPE = '1'; break;
	default : ///< peAuto peAutoToday peTMPMarketMaker
		charPE = '1'; break;
	}
	pPacker->AddChar( charPE );					///< futures_direction
	pPacker->AddChar( '0' );					///< hedge_type
	pPacker->AddInt( Msg->GetOrderQty() );		///< entrust_amount
	pPacker->AddDouble( Msg->GetPrice() );		///< futu_entrust_price
	// OrderType  (entrust_prop) F0:Limit F1:Market
	if ( Msg->GetOrderType() == nsOrderMessageDefine::otMarket )
	{
		StrOrderType    = "Market";
		StrTimeInForce  = "ROD";
		StrEntrustProp  = "F1";
	}
	else
	{
		StrOrderType    = "Limit";
		StrTimeInForce  = "ROD";
		StrEntrustProp  = "F0";
	}
	pPacker->AddStr( StrEntrustProp.c_str() );
	//pPacker->AddStr( "" );						///< entrust_occasion
	pPacker->AddStr( strNID );					///< entrust_reference
	pPacker->EndPack();
	///< Allow to place New order ?
	if ( FSpeedyConnPtr->FSendEventListener != NULL )
	{
		FSpeedyConnPtr->FSendEventListener->OnBeforeSend( Msg->GetMarket(), smtNew, Msg, CanSend, ErrMsg );
		if ( CanSend == false )
		{
			FSpeedyConnPtr->Reject( nsOrderMessageDefine::crrNew, ErrMsg, Msg ,Msg->GetTradingSessionID());
			goto EXIT;
		}
	}

	// 5. Send T2SDK Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	OrderRef = FHSufxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );

	UFC::BufferedLog::Printf( " OrderRef[%d] Account[%s] Symbol[%s] Side[%c] NID[%s]",
							  OrderRef, Msg->GetAccount(), Msg->GetSymbol(),
							  Msg->GetSide() == nsOrderMessageDefine::sBuy ? 'B':'S', strNID );
	UFC::BufferedLog::Printf( " Qty[%d] Px[%0.3f] TIF[%s] OT[%s] entrust_prod[%s] PE[%c]",
							  Msg->GetOrderQty(), Msg->GetPrice(), StrTimeInForce.c_str(),
							  StrOrderType.c_str(), StrEntrustProp.c_str() , charPE );
	if ( OrderRef < 0 )
	{
		UFC::BufferedLog::Printf( " ----------------------- Send failed:%d ------------------------", OrderRef );
		UTF8String ErrMsgUTF8( AnsiToUtf8Ex(FHSufxConnection->GetErrorMsg(OrderRef), 936) );
		ErrMsg.Printf( "Call SendBizMsg(338202) return error, Code[%d] Msg[%s]", OrderRef, ErrMsgUTF8.c_str() );
		FSpeedyConnPtr->Reject( nsOrderMessageDefine::crrNew, ErrMsg, Msg ,Msg->GetTradingSessionID());
	}
	else
	{
		UFC::BufferedLog::Printf( " ---------------------- Send New Order OK ------------------------" );
		HSufxAddPendingOrder( OrderRef, Msg, UserDefine );
	}

EXIT:
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
}
//---------------------------------------------------------------------------
void HSufxObjFutOpt::HSufxCancelOrder( TCancelOrderMessage* Msg, const UFC::AnsiString& UserDefine )
{
	UFC::AnsiString OrderID( Msg->GetOrderID() );
	UFC::AnsiString ExchangeCode( Msg->GetExchangeCode() );
	UFC::AnsiString ExchangeType = HSufxTo_exchange_type( ExchangeCode );
	UFC::AnsiString ErrMsg;
	bool            CanSend = true;
	COrderInfo*     DelOrd;
	int             CancelOrderRef;
	int             OrgOrderRef = COrderInfo::ToOrderRef( Msg->GetCNID() );
	int             OrderIDInt  = OrderID.ToInt();
	char            strNID[32+1];

	UFC::BufferedLog::Printf( " ------------------ User Place Cancel Order ----------------------" );
	sprintf( strNID, "%lld", Msg->GetNID() );

	// 1. SpeedyAPI Find COrderInfo* logic
	// Check cancel key.
	if( OrderID.Length() == 0 && OrgOrderRef == 0 )
	{
		ErrMsg.Printf( "Missing OrderID or OrderActionRef." );
		FSpeedyConnPtr->Reject( nsOrderMessageDefine::crrCancel, ErrMsg, Msg ,Msg->GetTradingSessionID());
		UFC::BufferedLog::Printf( " Missing OrderID or OrderActionRef." );
		UFC::BufferedLog::Printf( " --------------------- Reject Order Cancel -----------------------" );
		return;
	}
	// Find the original order.
	if( (DelOrd = FSpeedyConnPtr->FindOrderByOrderLocalID( Msg->GetExchangeCode(), OrderIDInt )) != NULL )
		UFC::BufferedLog::Printf( " Cancel by entrust_no[%s]", OrderID.c_str()  );
	else if( (DelOrd = FSpeedyConnPtr->FindOrderByOrderRef( 0, FHSufxSessionNo, OrgOrderRef )) != NULL )
		UFC::BufferedLog::Printf( " Cancel by FrontID[%d] SessionID[%d] OrderRef[%d]", 0, FHSufxSessionNo, OrgOrderRef );
	else
	{
		ErrMsg.Printf( "Order not found! entrust_no[%d]", OrderIDInt );
		FSpeedyConnPtr->Reject( nsOrderMessageDefine::crrCancel, ErrMsg, Msg ,Msg->GetTradingSessionID());
		UFC::BufferedLog::Printf( " Cancel Order not found! OrderID[%s] CNID_KEY[%d|%d|%d]"
								  , OrderID.c_str(), 0, FHSufxSessionNo, OrgOrderRef );
		UFC::BufferedLog::Printf( " --------------------- Reject Order Cancel -----------------------" );
		return;
	}

	// 2. New T2SDK BizMessage
	IBizMessage* lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxObjFutOpt::HSufxCancelOrder() NewBizMessage failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 338217 );
	lpBizMessage->SetSystemNo( FHSufxSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 3. New T2SDK Packet
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2);
	if ( pPacker == NULL )
	{
		ErrMsg.Printf( "HSufxObjFutOpt::HSufxCancelOrder() NewPacker failed!!" );
		FSpeedyConnPtr->Reject( nsOrderMessageDefine::crrCancel, ErrMsg, Msg ,Msg->GetTradingSessionID());
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();

	// 4. Add T2SDK Packet Field Name
	pPacker->AddField( "user_token", 'S', 512 );	///< Require
	pPacker->AddField( "op_branch_no", 'I', 5 );	///< Require
	pPacker->AddField( "op_entrust_way", 'C', 1 );	///< Require
	pPacker->AddField( "op_station", 'S', 255 );	///< Require
	pPacker->AddField( "branch_no", 'I', 5 );		///< Require
	pPacker->AddField( "client_id", 'S', 18 );		///< Require
	pPacker->AddField( "fund_account", 'S', 18 );	///< Require
	pPacker->AddField( "password", 'S', 50 );		///< Require
	pPacker->AddField( "futu_exch_type", 'S', 4 );	///< Require
	pPacker->AddField( "entrust_no", 'I', 8 );		///< NonRequire
	//pPacker->AddField( "confirm_id", 'S', 20 );		///< NonRequire
	//pPacker->AddField( "session_no", 'I', 8 );		///< NonRequire
	//pPacker->AddField( "entrust_occasion", 'S', 32 );		///< NonRequire
	pPacker->AddField( "entrust_reference", 'S', 32 );		///< NonRequire

	// 5. Add T2SDK Packet Field Value
	pPacker->AddStr( FHSufxUserToken );							///< user_token
	pPacker->AddInt( FHSufxBranchNo );							///< op_branch_no
	pPacker->AddChar( '7' );									///< op_entrust_way		// ?? from config ??
	pPacker->AddStr( "1" );										///< op_station
	pPacker->AddInt( FHSufxBranchNo );							///< branch_no
	pPacker->AddStr( FHSufxClientID );							///< client_id
	pPacker->AddStr( Msg->GetAccount() );						///< fund_account
	pPacker->AddStr( FSpeedyConnPtr->FPassword.c_str() );		///< password
	pPacker->AddStr( ExchangeType.c_str() );					///< futu_exch_type
	pPacker->AddInt( OrderIDInt );								///< entrust_no
	pPacker->AddStr( strNID );									///< entrust_reference
	pPacker->EndPack();

	///< Allow to place cancel order ?
	if ( FSpeedyConnPtr->FSendEventListener != NULL )
	{
		FSpeedyConnPtr->FSendEventListener->OnBeforeSend( Msg->GetMarket(), smtCancel, Msg, CanSend, ErrMsg );
		if ( CanSend == false )
		{
			FSpeedyConnPtr->Reject( nsOrderMessageDefine::crrCancel, ErrMsg, Msg ,Msg->GetTradingSessionID());
			goto EXIT;
		}
	}
	DelOrd->Cancel( UserDefine, CancelOrderRef );

	// 6. Send T2SDK Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	CancelOrderRef = FHSufxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );

	UFC::BufferedLog::Printf( " CancelOrderRef[%d] Account[%s] ExchangeID[%s] Symbol[%s] NID[%s]",
							  CancelOrderRef, Msg->GetAccount(), ExchangeCode.c_str(), Msg->GetSymbol(), strNID );
	UFC::BufferedLog::Printf( " FrontID[%d] SessionID[%d] OrderActionRef[%d] OrderSysID[%s]",
							  0, FHSufxSessionNo, CancelOrderRef, OrderID.c_str() );
	if ( CancelOrderRef < 0 )
	{
		UFC::BufferedLog::Printf( " ----------------------- Send failed:%d ------------------------", CancelOrderRef );
		UTF8String ErrMsgUTF8( AnsiToUtf8Ex(FHSufxConnection->GetErrorMsg(CancelOrderRef), 936) );
		ErrMsg.Printf( "Call SendBizMsg(338217) return error, Code[%d] Msg[%s]", CancelOrderRef, ErrMsgUTF8.c_str() );
		FSpeedyConnPtr->Reject( nsOrderMessageDefine::crrCancel, ErrMsg, Msg ,Msg->GetTradingSessionID());
	}
	else
	{
		UFC::BufferedLog::Printf( " -------------------- Send Cancel Order OK -----------------------" );
		// Add CancelOrderRef to OrigOrderKey hash map
		UFC::AnsiString OrderCancelRefKey;
		OrderCancelRefKey.Printf( "%d|%d|%d", 0, FHSufxSessionNo, CancelOrderRef );

		HSufxFutOpt::dataRefToOrderKey* OrigOrderKey = new HSufxFutOpt::dataRefToOrderKey;
		OrigOrderKey->entrust_no = OrderIDInt;
		OrigOrderKey->ExchangeCode = ExchangeCode;
		FHSufxCancelRefToOrigOrderKey.Add( OrderCancelRefKey, OrigOrderKey );
	}

EXIT:
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
}
//---------------------------------------------------------------------------
//
// Function HandleHSufxMessage
// Handle any request from User.( TTaifexConnection->SendNewsRequest )
//
// User : call TTaifexConnection->SendNewsRequest
// This func: call HS T2SDK -> SendBizMsg()
//
//---------------------------------------------------------------------------
int HSufxObjFutOpt::HandleHSufxMessage( TNewsMessage* Msg )
{
	int NewsID = Msg->GetID();

	switch ( NewsID )
	{
		case CTP_QuerySettlementInfoConfirm       : return 0; ///< no implementation temporarily
		case CTP_SettlementInfoConfirm            : return 0; ///< no implementation temporarily
		case CTP_QueryInvestorPosition            : return HSufxSendQryPosition( );
		case CTP_QueryInvestorPositionDetail      : return 0; ///< no implementation temporarily
		case CTP_ChangePassword                   : return HSufxSendChangePwd( Msg );
		case CTP_QueryTradingAccount              : return HSufxSendQryAssert( Msg );
		case CTP_QueryInstrumentCommissionRate    : return 0; ///< no implementation temporarily
		case CTP_QuerySettlementInfo              : return 0; ///< no implementation temporarily
		case CTP_QueryTransferSerial              : return 0; ///< no implementation temporarily
		case CTP_QueryTransferBank                : return 0; ///< no implementation temporarily
		case CTP_QueryContractBank                : return 0; ///< no implementation temporarily
		case CTP_QueryAccountRegister             : return 0; ///< no implementation temporarily
		case CTP_QueryTradingCode                 : return 0; ///< no implementation temporarily
		case CTP_QueryBankAccountMoneyByFuture    : return 0; ///< no implementation temporarily
		case CTP_TransferFromBankToFutureByFuture : return 0; ///< no implementation temporarily
		case CTP_TransferFromFutureToBankByFuture : return 0; ///< no implementation temporarily
	}

	return -1;
}
//---------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
