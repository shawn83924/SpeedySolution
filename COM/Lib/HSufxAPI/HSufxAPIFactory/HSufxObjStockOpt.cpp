//---------------------------------------------------------------------------
#ifdef _CTP
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#include "HSufxObjStockOpt.h"
#include "TTaifexConnection.h"
#include "APIParams.h"
//---------------------------------------------------------------------------
#include <WideStrUtils.hpp>
//---------------------------------------------------------------------------
// class HSufxObjStockOpt
//
// Sending data process implementation ( ---> )
//---------------------------------------------------------------------------
HSufxObjStockOpt::HSufxObjStockOpt( TTaifexConnection* SpeedyConnPtr )
: FSpeedyConnPtr( SpeedyConnPtr )
, FHSufxConfig( NULL )
, FHSufxConnection( NULL )
, FHSufxSysNodeID( 0 )
, FHSufxBranchNo( 0 )
, FHSufxLoginTimes( 0 )
, FHSufxSubscribeMask( 0 )
{
	// Create message window
	if ( InitMessageWindow() == FALSE )
	{
		UFC::BufferedLog::DebugPrintf( " Register HSUFX_StockOpt window class failed." );
		throw UFC::Exception( " Register HSUFX_StockOpt window class failed." );
	}
	if ( CreateMessageWindow() == FALSE )
	{
		UFC::BufferedLog::DebugPrintf( " Can't create the HSUFX_StockOpt message window." );
		throw UFC::Exception( " Can't create the HSUFX_StockOpt message window." );
	}
	UFC::BufferedLog::DebugPrintf( " Create HSUFX_StockOpt message window:%p Thread:%d", (void*)FHwnd, GetCurrentThreadId() );
}
//---------------------------------------------------------------------------
HSufxObjStockOpt::~HSufxObjStockOpt()
{
	DestroyWindow( FHwnd );
	UnregisterClassW( MessageWindowsClassName, FInstance );
}
//---------------------------------------------------------------------------
//
// HSufx Receiving functions (<-)
//
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxOnConnect( void )
{
/*
	FSpeedyConnPtr->FIsConnected = true;
	if ( FSpeedyConnPtr->FListener )
		FSpeedyConnPtr->FListener->OnConnected();
*/
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxOnClose( void )
{
	bool OrgIsConnected = FSpeedyConnPtr->FIsConnected;
	FSpeedyConnPtr->FIsConnected = false;
	FSpeedyConnPtr->FIsLogon = false;
	FSpeedyConnPtr->DetachShm();
	if ( FSpeedyConnPtr->FListener && OrgIsConnected == true )
		FSpeedyConnPtr->FListener->OnDisconnected();
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxLoginResult( HSufxStockOpt::dataLoginResult* Data )
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
		FSpeedyConnPtr->RemoveShm( "HSUFX_StockOpt" );
		FSpeedyConnPtr->AttachShm( "HSUFX_StockOpt", (char*)FSpeedyConnPtr->FTradeingDate.c_str() );
		FSpeedyConnPtr->FCTPFrontID   = 0;
		FSpeedyConnPtr->FCTPSessionID = Data->login_times;
		// no set FIsLogon
		// delay set at Recover finished
		//FSpeedyConnPtr->FIsLogon = true;
		FSpeedyConnPtr->FUseNID64 = true;
		FSpeedyConnPtr->FCurrentConnectionID = Data->login_times;
		UFC::CreateDir( "./log" );
		UFC::BufferedLog::Printf( " HSufxLoginResult()  Login OK! Account[%s] ClientID[%s] UserToken[%s]"
								  , Data->fund_account, Data->client_id, Data->user_token );
		UFC::BufferedLog::Printf( " sysnode_id[%d] branch_no[%d] login_times[%d]"
								  , Data->sysnode_id, Data->branch_no, Data->login_times );

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
		FHSufxLoginTimes = Data->login_times;
		// no trigger FListener->OnLogonReply
		// delay trigger at Recover finished
		// ( Query Confirm & Query Fill )

		// 3. Send Subscribe Push
		HSufxSendSubscribePush( 33012 ); ///< Confirm Report
		HSufxSendSubscribePush( 33011 ); ///< Fill Report
	}
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxLogoutResult( HSufxStockOpt::dataLogoutResult* /*Data*/ )
{
	FSpeedyConnPtr->FAdmin   = 0;
	FSpeedyConnPtr->FIsLogon = false; ///< Clear logon flag.
	FSpeedyConnPtr->FID      = "";
	FSpeedyConnPtr->FToken   = "";
	FSpeedyConnPtr->DetachShm();
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxChangePwdResult( HSufxStockOpt::dataChangePwdResult* Data )
{
	TNewsMessage Msg;

	Msg.SetHeadline( "HSUFX" );
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
void HSufxObjStockOpt::HSufxSubscribeResult( HSufxStockOpt::dataSubscribeResult* Data )
{
	AnsiString ReplyString;
	if ( Data->IsSubscribeOK )
	{
		if ( Data->IssueType == 33011 ) ///< Fill
			FHSufxSubscribeMask |= 0x02;
		else if ( Data->IssueType == 33012 ) ///< Confirm
			FHSufxSubscribeMask |= 0x01;

		if ( FSpeedyConnPtr->FListener != NULL )
		{
			if ( (FHSufxSubscribeMask & 0x01) && (FHSufxSubscribeMask & 0x02) )
			{
				// 1. Clear Cancel after Fill List
				HSufxClearCancelList( );
				// 2. Send Query Order Confirm
				HSufxSendQryOrderConfirm( );
			}
		}
	}
	else
	{
		if ( FSpeedyConnPtr->FListener != NULL )
		{
			ReplyString.printf( "Subscribe %s Push reply error!!", Data->IssueType == 33011 ? "Fill" : "Confirm" );
			FSpeedyConnPtr->FListener->OnLogonReply( ReplyString.c_str(), lrFailed, FSpeedyConnPtr->FCurrentConnectionID );
		}
	}
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxQryOrderResult( HSufxStockOpt::dataQryOrderResult* Data )
{
	// 1. Check if End notify then
	//    Send Query Order Fill
	if ( Data->IsEndNotify )
	{
		HSufxSendQryOrderFill( );
		return;
	}

	UFC::AnsiString          OrderLoacalAndSysID( Data->entrust_no );
	UFC::AnsiString          ExchangeCode = HSufxTo_ExchangeCode( Data->exchange_type );
	UFC::AnsiString          ExecID;
	UFC::AnsiString          ExecID2;
	COrderInfo*              Order;
	UFC::AnsiString          StrHHMMSSmmm;
	TExecutionReportMessage* ExecutionReportPtr = new TExecutionReportMessage();
	TExecutionReportMessage& ExecutionReport = *ExecutionReportPtr;
	bool                     IsDup;

	// 2. Process Query Order Confirm Result
	// 2.1 Create COrderInfo -> CTPAccepted -> EachangeAccepted
	Order = new COrderInfo( FHSufxBranchNo , FHSufxLoginTimes, Data );
	Order->CTPAccepted( ExchangeCode, OrderLoacalAndSysID );
	FSpeedyConnPtr->AddOrderLocalIDToMap( "0", Data->entrust_no, Order ); ///< use 0 replace ExchangeID

	// 2.2 TrigerOnExecutionReport for PendingNew
	FSpeedyConnPtr->CTPFillExecBase( nsOrderMessageDefine::mCNOptions, ExecutionReport, Order );
	UFC::BufferedLog::Printf( " -------------------- HSufx Pending (by Query) -------------------" );
	StrHHMMSSmmm = Data->entrust_time;
	StrHHMMSSmmm.PadThis( 9, '0', true );
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
	UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] Symbol[%s] OrderLocalID[%s] ExecID[%s] Dup[%s]"
							  , Order->GetOrderNID(), Order->GetExchangeID(), Order->GetSymbol(), OrderLoacalAndSysID.c_str()
							  , ExecID.c_str(), IsDup ? "Yes" : "No" );
	FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );

	// 2.3 TrigerOnExecutionReport for New
	if ( Data->entrust_status != '0' && ///< 未報
		 Data->entrust_status != '1' && ///< 待報
		 Data->entrust_status != '9' )  ///< 廢單
	{
		Order->EachangeAccepted( OrderLoacalAndSysID );
		StrHHMMSSmmm = Data->report_time;
		StrHHMMSSmmm.PadThis( 9, '0', true );
		UFC::BufferedLog::Printf( " -------------------- HSufx Confirm (by Query) -------------------" );
		ExecID.Printf( "N_%d", Data->entrust_no );
		ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osNew );
		ExecutionReport.SetExecType( nsOrderMessageDefine::etNew );
		ExecutionReport.SetExecID( ExecID.c_str() );
		ExecutionReport.SetTransactTime( StrHHMMSSmmm.c_str() );
		// TrigerOnExecutionReport
		IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
		UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] Symbol[%s] OrderLocalID[%s] ExecID[%s] Dup[%s]"
								  , Order->GetOrderNID(), Order->GetExchangeID(), Order->GetSymbol(), OrderLoacalAndSysID.c_str()
								  , ExecID.c_str(), IsDup ? "Yes" : "No" );
		FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	}

	// 3. Process Query Order Execute Result ( Cancel , Reject )
	if ( Data->entrust_status == '5' ) ///< 部撤 (部成部撤)
	{
		UFC::BufferedLog::Printf( " -------------- HSufx Cancel after fill (by Query) ---------------" );
		ExecID2.Printf( "C_%d", Data->entrust_no );
		ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osCanceled );
		ExecutionReport.SetExecType( nsOrderMessageDefine::etCanceled );
		ExecutionReport.SetExecID( ExecID2.c_str() );
		UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] Symbol[%s] OrderLocalID[%s] ExecID[%s] Dup[%s]"
								  , Order->GetOrderNID(), Order->GetExchangeID(), Order->GetSymbol(), OrderLoacalAndSysID.c_str()
								  , ExecID2.c_str() );
		// Add to cancel after fill queue
		FHSufxCancelAfterFillList.Add( ExecutionReportPtr );

		return; ///< don't delete ExecutionReportPtr
	}
	else if ( Data->entrust_status == '6' ) ///< 已撤
	{
		UFC::BufferedLog::Printf( " ------------------- HSufx Canceled (by Query) -------------------" );
		ExecID2.Printf( "C_%d", Data->entrust_no );
		ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osCanceled );
		ExecutionReport.SetExecType( nsOrderMessageDefine::etCanceled );
		ExecutionReport.SetExecID( ExecID2.c_str() );
		IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
		UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] Symbol[%s] OrderLocalID[%s] ExecID[%s] Dup[%s]"
								  , Order->GetOrderNID(), Order->GetExchangeID(), Order->GetSymbol(), OrderLoacalAndSysID.c_str()
								  , ExecID2.c_str(), IsDup ? "Yes" : "No" );
		FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	}
	else if ( Data->entrust_status == '9' ) ///< 廢單
	{
		UTF8String ErrMsgUTF8( AnsiToUtf8Ex(Data->cancel_info, 936) );
		UFC::BufferedLog::Printf( " -------------------- HSufx Reject (by Query) --------------------" );
		ExecID2.Printf( "R_%d", Data->entrust_no );
		Order->CloseOrder();
		ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osRejected );
		ExecutionReport.SetExecType( nsOrderMessageDefine::etRejected );
		ExecutionReport.SetCxlRejResponseTo( nsOrderMessageDefine::crrNew );
		ExecutionReport.SetExecID( ExecID2.c_str() );
		ExecutionReport.SetText( ErrMsgUTF8.c_str() );
		IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
		UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] Symbol[%s] OrderLocalID[%d]"
								  , Order->GetOrderNID(), Order->GetExchangeID(), Order->GetSymbol(), Order->GetOrderLocalID() );
		UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] Msg[%s]", ExecID2.c_str(), IsDup ? "Yes" : "No", ErrMsgUTF8.c_str() );
		FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	}

	delete ExecutionReportPtr;
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxQryFillResult( HSufxStockOpt::dataQryFillResult* Data )
{
	// 1. Check if End notify
	if ( Data->IsEndNotify )
	{
		if ( FSpeedyConnPtr->FListener != NULL )
		{
			// 1.1 Do Cancel after Fill List to TrigerOnExecutionReport
			HSufxDoCancelList( );
			// 1.2 Trigger IOrderConnectionEventListener::OnLogonReply()
			FSpeedyConnPtr->FIsLogon = true;
			FSpeedyConnPtr->FListener->OnLogonReply( "", lrOk, FSpeedyConnPtr->FCurrentConnectionID );
			// 1.3 Trigger IOrderConnectionEventListener::OnRecoverFinished()
			FSpeedyConnPtr->FListener->OnRecoverFinished( FSpeedyConnPtr->FRecoverCount );
		}
		return;
	}

	// 2. Process Query Order Fill Result
	// 2.1 Use ExchangeID + OrderLocalID to find Order.
	COrderInfo*     Order = FSpeedyConnPtr->FindOrderByOrderLocalID( "0", Data->entrust_no ); ///< use 0 replace ExchangeID
	if ( Order == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxQryFillResult() error!! entrust_no[%d] <Reason: Order not found.>", Data->entrust_no );
		return;
	}

	// 2.2 Create and setting TExecutionReportMessage
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         OrderLocalID( Data->entrust_no );
	UFC::AnsiString         ExecID;
	bool                    IsDup;
	char                    strHHMMSSmmm[10];

	sprintf( strHHMMSSmmm, "%d", Data->business_time*1000 );

	UFC::BufferedLog::Printf( " --------------------- HSufx Fill (by Query) ---------------------" );
	ExecID.Printf( "F_%d_%s", Data->entrust_no, Data->business_id );
	FSpeedyConnPtr->CTPFillExecBase( nsOrderMessageDefine::mCNOptions, ExecutionReport, Order );
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
	ExecutionReport.SetPrice( Data->opt_business_price );
	ExecutionReport.SetLastPx( Data->opt_business_price );
	ExecutionReport.SetOrderQty( (long)Data->business_amount );
	ExecutionReport.SetLastQty( (long)Data->business_amount );
	ExecutionReport.SetTransactTime( strHHMMSSmmm );

	// 3. TrigerOnExecutionReport
	UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] OrderLocalID[%s] FillSeq[%s] ExecID[%s] Dup[%s]"
							  , Order->GetOrderNID(), Order->GetExchangeID(), OrderLocalID.c_str(), Data->business_id, ExecID.c_str(), IsDup ? "Yes" : "No" );
	UFC::BufferedLog::Printf( " Time[%s] Symbol[%s] Side[%c] LastPx[%.4f] LastQty[%d] CumQty[%d] LeavesQty[%d]"
							  , strHHMMSSmmm, Order->GetSymbol(), Data->entrust_bs == '1' ? 'B':'S', Data->opt_business_price
							  , (int)Data->business_amount, Order->GetCumQty(), Order->GetLeavesQty() );
	FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport,(ExecDup) IsDup );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxQryAssetResult( HSufxStockOpt::dataQryAssetResult* Data )
{
	// Process Query Asset Result
	TNewsMessage Msg;
	UFC::NameValueMessage BalanceMsg("^\n");

	Msg.SetHeadline( "HSUFX" );
	Msg.SetID( CTP_OnTradingAccount );
	BalanceMsg.Append( "total_asset",         Data->total_asset );
	BalanceMsg.Append( "fund_asset",          Data->fund_asset );
	BalanceMsg.Append( "current_balance",     Data->current_balance );
	BalanceMsg.Append( "enable_balance",      Data->enable_balance );
	BalanceMsg.Append( "enable_bail_balance", Data->enable_bail_balance );
	BalanceMsg.Append( "used_bail_balance",   Data->used_bail_balance );
	BalanceMsg.Append( "used_pur_balance",    Data->used_pur_balance );
	BalanceMsg.Append( "enable_pur_balance",  Data->enable_pur_balance );
	BalanceMsg.Append( "pur_quota",           Data->pur_quota );
	BalanceMsg.Append( "income_balance",      Data->income_balance );
	//BalanceMsg.Append( "frozen_balance",      Data->frozen_balance ); ///<文件沒有
	BalanceMsg.Append( "risk_degree",         Data->risk_degree );
	BalanceMsg.Append( "real_risk_degree",    Data->real_risk_degree );
	BalanceMsg.Append( "dyna_market_value",   Data->dyna_market_value );
	BalanceMsg.Append( "real_used_bail",      Data->real_used_bail );
	UFC::AnsiString OptRiskType( Data->optrisk_type );
	BalanceMsg.Append( "optrisk_type",        OptRiskType );

	Msg.SetText( BalanceMsg.ToString().c_str() );
	if ( FSpeedyConnPtr->FListener != NULL )
		FSpeedyConnPtr->FListener->OnNews( &Msg );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxQryPositionResult( HSufxStockOpt::dataQryPositionResult* Data )
{
	// 1. Check if End notify
	if ( Data->IsEndNotify )
		return;

	// 2. Process Query Hold Position Result
	UFC::BufferedLog::Printf( " HSufxQryPositionResult() %s %s opthold_type[%c] current_amount[%.0f] enable_amount[%.0f] real_open_amount[%.0f] real_drop_amount[%.0f]"
							  , Data->fund_account, Data->option_code, Data->opthold_type, Data->current_amount
							  , Data->enable_amount, Data->real_open_amount, Data->real_drop_amount );
	UFC::BufferedLog::Printf( " entrust_drop_amount[%.0f] hold_amount[%.0f] opt_cost_price[%.3f] opt_last_price[%.3f] amount_per_hand[%d]"
							  , Data->entrust_drop_amount, Data->hold_amount, Data->opt_cost_price, Data->opt_last_price, Data->amount_per_hand );
	UFC::BufferedLog::FlushToFile();

	if ( FSpeedyConnPtr->FListener != NULL )
	{
		UFC::AnsiString NewsText;
		NewsText.Printf( "0|OK|0|1|%s|%d|%s|%c|00000000|%d|%15.4lf"
						 , Data->option_code, FHSufxBranchNo, Data->fund_account, Data->opthold_type
						 , (int)Data->current_amount, Data->opt_cost_price );
		TNewsMessage Msg;
		Msg.SetHeadline("HSUFX" );
		Msg.SetID( CTP_OnInvestorOpenPosition );
		Msg.SetText( NewsText.c_str() );
		FSpeedyConnPtr->FListener->OnNews( &Msg );
	}
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxPendingExecution( HSufxStockOpt::dataPendingExecution* Data )
{
	UFC::AnsiString OrderLocalID( Data->entrust_no );

	// 1. Use FrontID + SessionID + OrderRef to find Order.
	COrderInfo* Order = FSpeedyConnPtr->FindOrderByOrderRef( 0, FHSufxLoginTimes, Data->hSend );
	if ( Order == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxPendingExecution() error!! FrontID[0] SessionID[%d] OrderRef[%d] entrust_no[%d] <Reason: Order not found.>"
								  , FHSufxLoginTimes, Data->hSend, Data->entrust_no );
		return;
	}
	Order->CTPAccepted( Order->GetExchangeID(), OrderLocalID );
	FSpeedyConnPtr->AddOrderLocalIDToMap( "0", Data->entrust_no, Order ); ///< use 0 replace ExchangeID

	// 2. Create and setting TExecutionReportMessage
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	bool                    IsDup;
	char                    strHHMMSSmmm[10];
	UFC::GetTimeString( strHHMMSSmmm, FALSE );

	UFC::BufferedLog::Printf( " -------------------------- HSufx Pending ------------------------" );
	ExecID.Printf( "P_%d", Data->entrust_no );
	FSpeedyConnPtr->CTPFillExecBase( nsOrderMessageDefine::mCNOptions, ExecutionReport, Order );
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
	UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] Symbol[%s] OrderLocalID[%s]"
							  , Order->GetOrderNID(), Order->GetExchangeID(), Order->GetSymbol(), OrderLocalID.c_str() );
	UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] SessionID[%d] OrderRef[%d]", ExecID.c_str(), IsDup ? "Yes" : "No", FHSufxLoginTimes, Data->hSend );
	FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	UFC::BufferedLog::Printf( " ---------------- Sent Pending New Execution ---------------------" );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxConfirmExecution( HSufxStockOpt::dataConfirmExecution* Data )
{
	UFC::AnsiString OrderLocalID( Data->entrust_no );
	if ( Data->entrust_status != '2' ) ///< entrust_status=2 (已報)
	{
		UFC::BufferedLog::Printf( " HSufxConfirmExecution() error!! entrust_no[%d] entrust_status[%c] <Reason: entrust_status != '2'>", Data->entrust_no, Data->entrust_status );
		return;
	}

	// 1. Use ExchangeID + OrderLocalID to find Order.
	COrderInfo* Order = FSpeedyConnPtr->FindOrderByOrderLocalID( "0", Data->entrust_no ); ///< use 0 replace ExchangeID
	if ( Order == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxConfirmExecution() error!! entrust_no[%d] <Reason: Order not found.>", Data->entrust_no );
		return;
	}
	Order->EachangeAccepted( OrderLocalID ); ///< OrderSysID same as OrderLocalID
	//FSpeedyConnPtr->AddOrderSysIDToMap( "0", Data->entrust_no, Order ); ///< use 0 replace ExchangeID

	// 2. Create and setting TExecutionReportMessage
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	bool                    IsDup;
	char                    strHHMMSSmmm[10];
	UFC::GetTimeString( strHHMMSSmmm, FALSE );

	UFC::BufferedLog::Printf( " -------------------------- HSufx Confirm ------------------------" );
	ExecID.Printf( "N_%d", Data->entrust_no );
	FSpeedyConnPtr->CTPFillExecBase( nsOrderMessageDefine::mCNOptions, ExecutionReport, Order );
	ExecutionReport.SetOrderID( OrderLocalID.c_str() );
	ExecutionReport.SetNID( Order->GetOrderNID() );
	ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osNew );
	ExecutionReport.SetExecType( nsOrderMessageDefine::etNew );
	ExecutionReport.SetAllUserData( Order->GetUserData() );
	ExecutionReport.SetExecID( ExecID.c_str() );
	ExecutionReport.SetLastQty( 0 );
	ExecutionReport.SetTransactTime( strHHMMSSmmm );

	// 3. TrigerOnExecutionReport
	IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
	UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] Symbol[%s] OrderLocalID[%s] ExecID[%s] Dup[%s]"
							  , Order->GetOrderNID(), Order->GetExchangeID(), Order->GetSymbol(), OrderLocalID.c_str()
							  , ExecID.c_str(), IsDup ? "Yes" : "No" );
	FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	UFC::BufferedLog::Printf( " ------------------ Sent Confirmed Execution ---------------------" );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxRejectExecution( HSufxStockOpt::dataRejectExecution* Data )
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
		int             OrderIDInt;
		UFC::AnsiString OrderRefKey;
		OrderRefKey.Printf( "%d|%d|%d", 0, FHSufxLoginTimes, Data->hSend );
		if ( FHSufxCancelRefToOrdLocalID.GetObjectByKey( OrderRefKey, OrderIDInt ) == FALSE )
		{
			UFC::BufferedLog::Printf( " HSufxRejectExecution() Cancel Reject error!! FrontID[0] SessionID[%d] CancelOrderRef[%d] <Reason: OrderLocalID not found.>"
									  , FHSufxLoginTimes, Data->hSend );
			return;
		}
		// 1.2 Use ExchangeID + OrderLocalID to find Order
		Order = FSpeedyConnPtr->FindOrderByOrderLocalID( "0", OrderIDInt ); ///< use 0 replace ExchangeID
		if ( Order == NULL )
		{
			UFC::BufferedLog::Printf( " HSufxRejectExecution() Cancel Reject error!! entrust_no[%d] <Reason: Order not found.>", OrderIDInt );
			return;
		}

		UFC::BufferedLog::Printf( " ---------------------- HSufx Cancel Reject ----------------------" );
		ExecID.Printf( "CR_%d_%d", FHSufxLoginTimes, Data->hSend );
	}
	else
	{
		// 1.1 Use FrontID + SessionID + OrderRef to find Order.
		Order = FSpeedyConnPtr->FindOrderByOrderRef( 0, FHSufxLoginTimes, Data->hSend );
		if ( Order == NULL )
		{
			UFC::BufferedLog::Printf( " HSufxRejectExecution() New Reject error!! FrontID[0] SessionID[%d] OrderRef[%d] <Reason: Order not found.>"
									  , FHSufxLoginTimes, Data->hSend );
			return;
		}
		Order->CloseOrder();

		UFC::BufferedLog::Printf( " ------------------------- HSufx Reject --------------------------" );
		ExecID.Printf( "R_%d_%d", FHSufxLoginTimes, Data->hSend );
	}

	// 2. Setting TExecutionReportMessage
	FSpeedyConnPtr->CTPFillExecBase( nsOrderMessageDefine::mCNOptions, ExecutionReport, Order );
	//ExecutionReport.SetOrderID( OrderLocalID.c_str() ); ///< ??
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
							  , Order->GetOrderNID(), Order->GetExchangeID(), Order->GetSymbol(), FHSufxLoginTimes, Data->hSend );
	UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] Msg[%s]", ExecID.c_str(), IsDup ? "Yes" : "No", Data->ErrMsg );
	FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	if ( Data->ResponseTo == nsOrderMessageDefine::crrCancel )
		UFC::BufferedLog::Printf( " ---------------- Sent Cancel Reject Execution -------------------" );
	else
		UFC::BufferedLog::Printf( " ------------------- Sent Reject Execution -----------------------" );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxRejectByExh( HSufxStockOpt::dataRejectByExh* Data )
{
	// 1. Use ExchangeID + OrderLocalID to find Order.
	COrderInfo* Order = FSpeedyConnPtr->FindOrderByOrderLocalID( "0", Data->entrust_no ); ///< use 0 replace ExchangeID
	if ( Order == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxRejectByExh() error!! entrust_no[%d] <Reason: Order not found.>", Data->entrust_no );
		return;
	}
	if ( Order->GetStatus() != cosPending && Order->GetStatus() != cosConfirm )
	{
		UFC::BufferedLog::Printf( " HSufxRejectByExh() error!! entrust_no[%d] <Reason: Order status not equal Pending and Confirm.>", Data->entrust_no );
		return;
	}
	Order->CloseOrder();

	// 2. Create and setting TExecutionReportMessage
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString OrderLocalID( Data->entrust_no );
	UFC::AnsiString         ExecID;
	bool                    IsDup;
	UFC::AnsiString         ErrMsg;
	char                    strHHMMSSmmm[10];

	ErrMsg.Printf( "Exchange Reject:%s", Data->extern_code );
	sprintf( strHHMMSSmmm, "%d", Data->business_time*1000 );

	if ( Data->ResponseTo == nsOrderMessageDefine::crrCancel )
	{
		UFC::BufferedLog::Printf( " -------------------- HSufx Exh Cancel Reject --------------------" );
		ExecID.Printf( "CR_%d", Data->entrust_no );
	}
	else
	{
		UFC::BufferedLog::Printf( " ----------------------- HSufx Exh Reject ------------------------" );
		ExecID.Printf( "R_%d", Data->entrust_no );
	}
	FSpeedyConnPtr->CTPFillExecBase( nsOrderMessageDefine::mCNOptions, ExecutionReport, Order );
	ExecutionReport.SetOrderID( OrderLocalID.c_str() );
	ExecutionReport.SetNID( Order->GetOrderNID() );
	ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osRejected );
	ExecutionReport.SetExecType( nsOrderMessageDefine::etRejected );
	ExecutionReport.SetCxlRejResponseTo( Data->ResponseTo );
	ExecutionReport.SetAllUserData( Order->GetCxlUserData() );
	ExecutionReport.SetExecID( ExecID.c_str() );
	ExecutionReport.SetLastQty( 0 );
	ExecutionReport.SetText( ErrMsg.c_str() );
	ExecutionReport.SetTransactTime( strHHMMSSmmm );

	// 3. TrigerOnExecutionReport
	IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
	UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] Symbol[%s] OrderLocalID[%d]"
							  , Order->GetOrderNID(), Order->GetExchangeID(), Order->GetSymbol(), Order->GetOrderLocalID() );
	UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] Msg[%s]", ExecID.c_str(), IsDup ? "Yes" : "No", ErrMsg.c_str() );
	FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	if ( Data->ResponseTo == nsOrderMessageDefine::crrCancel )
		UFC::BufferedLog::Printf( " -------------- Sent Exh Cancel Reject Execution -----------------" );
	else
		UFC::BufferedLog::Printf( " ----------------- Sent Exh Reject Execution ---------------------" );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxCanceledExecution( HSufxStockOpt::dataCanceledExecution* Data )
{
	// 1. Use ExchangeID + OrderLocalID to find Order.
	COrderInfo* Order = FSpeedyConnPtr->FindOrderByOrderLocalID( "0", Data->entrust_no ); ///< use 0 replace ExchangeID
	if ( Order == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxCanceledExecution() error!! entrust_no[%d] <Reason: Order not found.>", Data->entrust_no );
		return;
	}
	Order->CloseOrder();

	// 2. Create and setting TExecutionReportMessage
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString OrderLocalID( Data->entrust_no );
	UFC::AnsiString         ExecID;
	bool                    IsDup;
	char                    strHHMMSSmmm[10];

	sprintf( strHHMMSSmmm, "%d", Data->business_time*1000 );

	UFC::BufferedLog::Printf( " ------------------------- HSufx Canceled ------------------------" );
	ExecID.Printf( "C_%d", Data->entrust_no );
	FSpeedyConnPtr->CTPFillExecBase( nsOrderMessageDefine::mCNOptions, ExecutionReport, Order );
	ExecutionReport.SetOrderID( OrderLocalID.c_str() );
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
	UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] Symbol[%s] OrderLocalID[%s] ExecID[%s] Dup[%s]"
							  , Order->GetOrderNID(), Order->GetExchangeID(), Order->GetSymbol(), OrderLocalID.c_str()
							  , ExecID.c_str(), IsDup ? "Yes" : "No" );
	FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	UFC::BufferedLog::Printf( " ------------------- Sent Canceled Execution ---------------------" );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxFillExecution( HSufxStockOpt::dataFillExecution* Data )
{
	// 1. Use ExchangeID + OrderLocalID to find Order.
	COrderInfo* Order = FSpeedyConnPtr->FindOrderByOrderLocalID( "0", Data->entrust_no ); ///< use 0 replace ExchangeID
	if ( Order == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxFillExecution() error!! entrust_no[%d] <Reason: Order not found.>", Data->entrust_no );
		return;
	}

	// 2. Create and setting TExecutionReportMessage
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString OrderLocalID( Data->entrust_no );
	UFC::AnsiString         ExecID;
	bool                    IsDup;
	char                    strHHMMSSmmm[10];

	sprintf( strHHMMSSmmm, "%d", Data->business_time*1000 );

	UFC::BufferedLog::Printf( " --------------------------- HSufx Fill --------------------------" );
	ExecID.Printf( "F_%d_%s", Data->entrust_no, Data->business_id );
	FSpeedyConnPtr->CTPFillExecBase( nsOrderMessageDefine::mCNOptions, ExecutionReport, Order );
	IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReport.GetMarket(), ExecID );
	if ( IsDup == false )
		Order->Fill( (int)Data->business_amount );
	ExecutionReport.SetOrderID( OrderLocalID.c_str() );
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
	ExecutionReport.SetPrice( Data->opt_business_price );
	ExecutionReport.SetLastPx( Data->opt_business_price );
	ExecutionReport.SetOrderQty( (long)Data->business_amount );
	ExecutionReport.SetLastQty( (long)Data->business_amount );
	ExecutionReport.SetTransactTime( strHHMMSSmmm );

	// 3. TrigerOnExecutionReport
	UFC::BufferedLog::Printf( " NID[%lld] ExchangeID[%s] OrderLocalID[%s] FillSeq[%s] ExecID[%s] Dup[%s]"
							  , Order->GetOrderNID(), Order->GetExchangeID(), OrderLocalID.c_str(), Data->business_id, ExecID.c_str(), IsDup ? "Yes" : "No" );
	UFC::BufferedLog::Printf( " Account[%s] Time[%s] Symbol[%s] Side[%c] ", Data->fund_account, strHHMMSSmmm, Order->GetSymbol(), Data->entrust_bs == '1' ? 'B':'S' );
	UFC::BufferedLog::Printf( " LastPx[%.4f] LastQty[%d] CumQty[%d] LeavesQty[%d]"
							  , Data->opt_business_price, (int)Data->business_amount, Order->GetCumQty(), Order->GetLeavesQty() );
	FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport,(ExecDup)IsDup );
	UFC::BufferedLog::Printf( " -------------------- Sent Fill Execution ------------------------" );
}
//---------------------------------------------------------------------------
//
// HSufx Sending functions (->)
//
//---------------------------------------------------------------------------
const char* HSufxObjStockOpt::HSufxTo_ExchangeCode( const UFC::AnsiString& exchange_type )
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
const char* HSufxObjStockOpt::HSufxTo_exchange_type( const UFC::AnsiString& ExchangeCode )
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
void HSufxObjStockOpt::HSufxClearCancelList( void )
{
	for ( int i = 0; i < FHSufxCancelAfterFillList.ItemCount(); i++ )
	{
		delete FHSufxCancelAfterFillList[i];
	}
	FHSufxCancelAfterFillList.Clear();
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxDoCancelList( void )
{
	for ( int i = 0; i < FHSufxCancelAfterFillList.ItemCount(); i++ )
	{
		// TrigerOnExecutionReport
		TExecutionReportMessage* ExecutionReportPtr = FHSufxCancelAfterFillList[i];
		bool IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReportPtr->GetMarket(), ExecutionReportPtr->GetExecID() );
		FSpeedyConnPtr->TrigerOnExecutionReport( ExecutionReportPtr, (ExecDup)IsDup );
		delete ExecutionReportPtr;
	}
	FHSufxCancelAfterFillList.Clear();
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxReplyHeartBeat( HSufxStockOpt::dataReplyHeartBeat* Data )
{
	int RtnVal = FHSufxConnection->SendBizEx( 620000, NULL, NULL, HSufx_ASyncSend, FHSufxSysNodeID, 0, FHSufxBranchNo, &Data->ReqData );
	if ( RtnVal < 0 )
	{
		UTF8String ErrMsgUTF8( AnsiToUtf8Ex(FHSufxConnection->GetErrorMsg(RtnVal), 936) );
		UFC::BufferedLog::Printf( " HSufxReplyHeartBeat SendBizEx(620000) UserID[%s] BranchNo[%d] return error, Code[%d] Msg[%s]"
								  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal, ErrMsgUTF8.c_str() );
		return;
	}
	UFC::BufferedLog::Printf( " HSufxReplyHeartBeat SendBizEx(620000) to HS UFX Server OK." );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxReplyHeartBeatMsg( HSufxStockOpt::dataReplyHeartBeatMsg* Data )
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
void HSufxObjStockOpt::HSufxSendSubscribePush( int IssueType )
{
	int RtnVal;

	// 1. New T2SDK BizMessage
	IBizMessage* lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendSubscribePush(%d) NewBizMessage failed!!", IssueType );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 620001 );
	lpBizMessage->SetIssueType( IssueType );
	lpBizMessage->SetSystemNo( FHSufxSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 2. New T2SDK Packet
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2);
	if ( pPacker == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendSubscribePush(%d) NewPacker failed!!", IssueType );
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
	pPacker->AddField( "password", 'S', 10 );		///< NonRequire
	pPacker->AddField( "user_token", 'S', 40 );		///< NonRequire
	pPacker->AddField( "issue_type", 'I', 8 );		///< Require
	pPacker->AddField( "fund_account", 'S', 18 );	///< Require

	// 4. Add T2SDK Packet Field Value
	pPacker->AddInt( FHSufxBranchNo );						///< op_branch_no
	pPacker->AddChar( '7' );								///< op_entrust_way		// ?? from config ??
	pPacker->AddStr( "1" );									///< op_station
	pPacker->AddInt( FHSufxBranchNo );						///< branch_no
	pPacker->AddStr( FSpeedyConnPtr->FPassword.c_str() );	///< password
	pPacker->AddStr( FHSufxUserToken );						///< user_token
	pPacker->AddInt( IssueType );							///< issue_type
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
								  , IssueType, FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal, ErrMsgUTF8.c_str() );
		return;
	}
	UFC::BufferedLog::Printf( " HSufxSendSubscribePush(%d) SendBizMsg(620001) OK. UserID[%s] BranchNo[%d] RtnVal[%d]"
							  , IssueType, FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxSendQryOrderConfirm( void )
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
	lpBizMessage->SetFunction( 338020 );
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
	pPacker->AddField( "op_branch_no", 'I', 5 );	///< Require
	pPacker->AddField( "op_entrust_way", 'C', 1 );	///< Require
	pPacker->AddField( "op_station", 'S', 255 );	///< Require
	pPacker->AddField( "branch_no", 'I', 5 );		///< Require
	pPacker->AddField( "client_id", 'S', 18 );		///< Require
	pPacker->AddField( "fund_account", 'S', 18 );	///< Require
	pPacker->AddField( "password", 'S', 10 );		///< Require
	pPacker->AddField( "password_type", 'C', 1 );	///< NonRequire
	pPacker->AddField( "user_token", 'S', 40 );		///< NonRequire
	pPacker->AddField( "asset_prop", 'C', 1 );		///< Require
	pPacker->AddField( "query_type", 'C', 1 );		///< NonRequire

	// 4. Add T2SDK Packet Field Value
	pPacker->AddInt( FHSufxBranchNo );						///< op_branch_no
	pPacker->AddChar( '7' );								///< op_entrust_way		// ?? from config ??
	pPacker->AddStr( "1" );									///< op_station
	pPacker->AddInt( FHSufxBranchNo );						///< branch_no
	pPacker->AddStr( FHSufxClientID );						///< client_id
	pPacker->AddStr( FSpeedyConnPtr->FID.c_str() );			///< fund_account
	pPacker->AddStr( FSpeedyConnPtr->FPassword.c_str() );	///< password
	pPacker->AddChar( '2' );								///< password_type
	pPacker->AddStr( FHSufxUserToken );						///< user_token
	pPacker->AddChar( 'B' );								///< asset_prop
	pPacker->AddChar( '1' );								///< query_type
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
		UFC::BufferedLog::Printf( " HSufxSendQryOrderConfirm() SendBizMsg(338020) UserID[%s] BranchNo[%d] return error, Code[%d] Msg[%s]"
								  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal, ErrMsgUTF8.c_str() );
		return;
	}
	UFC::BufferedLog::Printf( " HSufxSendQryOrderConfirm() SendBizMsg(338020) UserID[%s] BranchNo[%d] RtnVal[%d]"
							  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxSendQryOrderFill( void )
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
	lpBizMessage->SetFunction( 338021 );
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
	pPacker->AddField( "password", 'S', 10 );		///< Require
	pPacker->AddField( "password_type", 'C', 1 );	///< NonRequire
	pPacker->AddField( "user_token", 'S', 40 );		///< NonRequire
	pPacker->AddField( "asset_prop", 'C', 1 );		///< Require

	// 4. Add T2SDK Packet Field Value
	pPacker->AddInt( FHSufxBranchNo );						///< op_branch_no
	pPacker->AddChar( '7' );								///< op_entrust_way		// ?? from config ??
	pPacker->AddStr( "1" );									///< op_station
	pPacker->AddInt( FHSufxBranchNo );						///< branch_no
	pPacker->AddStr( FHSufxClientID );						///< client_id
	pPacker->AddStr( FSpeedyConnPtr->FID.c_str() );			///< fund_account
	pPacker->AddStr( FSpeedyConnPtr->FPassword.c_str() );	///< password
	pPacker->AddChar( '2' );								///< password_type
	pPacker->AddStr( FHSufxUserToken );						///< user_token
	pPacker->AddChar( 'B' );								///< asset_prop
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
		UFC::BufferedLog::Printf( " HSufxSendQryOrderFill() SendBizMsg(338021) UserID[%s] BranchNo[%d] return error, Code[%d] Msg[%s]"
								  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal, ErrMsgUTF8.c_str() );
		return;
	}
	UFC::BufferedLog::Printf( " HSufxSendQryOrderFill() SendBizMsg(338021) UserID[%s] BranchNo[%d] RtnVal[%d]"
							  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal );
}
//---------------------------------------------------------------------------
int HSufxObjStockOpt::HSufxSendChangePwd( TNewsMessage* Msg )
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
	pPacker->AddField( "password", 'S', 10 );		///< Require
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
int HSufxObjStockOpt::HSufxSendQryPosition( TNewsMessage* /*Msg*/ )
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
	lpBizMessage->SetFunction( 338023 );
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
	pPacker->AddField( "password", 'S', 10 );		///< Require
	pPacker->AddField( "password_type", 'C', 1 );	///< NonRequire
	pPacker->AddField( "user_token", 'S', 40 );		///< NonRequire
	pPacker->AddField( "asset_prop", 'C', 1 );		///< Require

	// 5. Add T2SDK Packet Field Value
	pPacker->AddInt( FHSufxBranchNo );						///< op_branch_no
	pPacker->AddChar( '7' );								///< op_entrust_way		// ?? from config ??
	pPacker->AddStr( "1" );									///< op_station
	pPacker->AddInt( FHSufxBranchNo );						///< branch_no
	pPacker->AddStr( FHSufxClientID );						///< client_id
	pPacker->AddStr( FSpeedyConnPtr->FID.c_str() );			///< fund_account
	pPacker->AddStr( FSpeedyConnPtr->FPassword.c_str() );	///< password
	pPacker->AddChar( '2' );								///< password_type
	pPacker->AddStr( FHSufxUserToken );						///< user_token
	pPacker->AddChar( 'B' );								///< asset_prop
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
		UFC::BufferedLog::Printf( " HSufxSendQryPosition() SendBizMsg(338023) UserID[%s] BranchNo[%d] return error, Code[%d] Msg[%s]"
								  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal, ErrMsgUTF8.c_str() );
		return -1;
	}
	UFC::BufferedLog::Printf( " HSufxSendQryPosition() SendBizMsg(338023) UserID[%s] BranchNo[%d] RtnVal[%d]"
							  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal );

	return 0;
}
//---------------------------------------------------------------------------
int HSufxObjStockOpt::HSufxSendQryAssert( TNewsMessage* /*Msg*/ )
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
	lpBizMessage->SetFunction( 338022 );
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
	pPacker->AddField( "password", 'S', 10 );		///< Require
	pPacker->AddField( "password_type", 'C', 1 );	///< NonRequire
	pPacker->AddField( "user_token", 'S', 40 );		///< NonRequire
	pPacker->AddField( "asset_prop", 'C', 1 );		///< Require
	// "money_type" ///< ??							///< Require

	// 5. Add T2SDK Packet Field Value
	pPacker->AddInt( FHSufxBranchNo );						///< op_branch_no
	pPacker->AddChar( '7' );								///< op_entrust_way		// ?? from config ??
	pPacker->AddStr( "1" );									///< op_station
	pPacker->AddInt( FHSufxBranchNo );						///< branch_no
	pPacker->AddStr( FHSufxClientID );						///< client_id
	pPacker->AddStr( FSpeedyConnPtr->FID.c_str() );			///< fund_account
	pPacker->AddStr( FSpeedyConnPtr->FPassword.c_str() );	///< password
	pPacker->AddChar( '2' );								///< password_type
	pPacker->AddStr( FHSufxUserToken );						///< user_token
	pPacker->AddChar( 'B' );								///< asset_prop
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
		UFC::BufferedLog::Printf( " HSufxSendQryAssert() SendBizMsg(338022) UserID[%s] BranchNo[%d] return error, Code[%d] Msg[%s]"
								  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal, ErrMsgUTF8.c_str() );
		return -1;
	}
	UFC::BufferedLog::Printf( " HSufxSendQryAssert() SendBizMsg(338022) UserID[%s] BranchNo[%d] RtnVal[%d]"
							  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal );

	return 0;
}
//---------------------------------------------------------------------------
COrderInfo* HSufxObjStockOpt::HSufxAddPendingOrder( int OrdRef, TNewOrderMessage* Ord, const UFC::AnsiString& UserDefine )
{
	COrderInfo* NewOrd = new COrderInfo( 0, FHSufxLoginTimes, Ord, UserDefine );
	UFC::AnsiString OrderRefKey;

	OrderRefKey.Printf( "%d|%d|%d", 0, FHSufxLoginTimes, OrdRef );
	FSpeedyConnPtr->FOrderRecords.Add( NewOrd );
	FSpeedyConnPtr->FOrderRefToRecord.Add( OrderRefKey, NewOrd );
	return NewOrd;
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxInitial( void )
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
void HSufxObjStockOpt::HSufxRelease( void )
{
	// 1. FHSufxConnection
	FHSufxConnection->Release();
	FHSufxConnection = NULL;
	// 3. FHSufxConfig
	FHSufxConfig->Release();
	FHSufxConfig = NULL;
}
//---------------------------------------------------------------------------
BOOL HSufxObjStockOpt::HSufxConnect( const char* strT2Servers, int TimeoutSec )
{
	int RtnVal;

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
	FHSufxConfig->SetString( "t2sdk", "license_file", "HSt2sdkLicense.dat" );
	FHSufxConfig->SetString( "t2sdk", "license_pwd", "123456" );
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
void HSufxObjStockOpt::HSufxSendLogin( void )
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
	pPacker->AddField( "password", 'S', 10 );		///< Require
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
void HSufxObjStockOpt::HSufxSendLogout( void )
{
	int RtnVal;

	// 1. New T2SDK BizMessage
	IBizMessage* lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendLogout() NewBizMessage failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 331102 );
	lpBizMessage->SetSystemNo( FHSufxSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 2. New T2SDK Packet
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxSendLogout() NewPacker failed!!" );
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
	pPacker->AddField( "client_id", 'S', 18 );		///< Require
	pPacker->AddField( "fund_account", 'S', 18 );	///< Require
	pPacker->AddField( "password", 'S', 10 );		///< Require
	pPacker->AddField( "password_type", 'C', 1 );	///< NonRequire
	pPacker->AddField( "user_token", 'S', 40 );		///< NonRequire

	// 4. Add T2SDK Packet Field Value
	pPacker->AddInt( 0 );									///< op_branch_no
	pPacker->AddChar( '7' );								///< op_entrust_way		// ?? from config ??
	pPacker->AddStr( "1" );									///< op_station
	pPacker->AddInt( 0 );									///< branch_no
	pPacker->AddStr( FHSufxClientID );						///< client_id
	pPacker->AddStr( FSpeedyConnPtr->FID.c_str() );			///< fund_account
	pPacker->AddStr( FSpeedyConnPtr->FPassword.c_str() );	///< password
	pPacker->AddChar( '2' );								///< password_type
	pPacker->AddStr( FHSufxUserToken );						///< user_token
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
		UFC::BufferedLog::Printf( " HSufxSendLogout() SendBizMsg(331102) UserID[%s] BranchNo[%d] return error, Code[%d] Msg[%s]"
								  , FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal, ErrMsgUTF8.c_str() );
		return;
	}
	UFC::BufferedLog::Printf( " HSufxSendLogout() SendBizMsg(331102) OK. UserID[%s] BranchNo[%d] RtnVal[%d]", FSpeedyConnPtr->FID.c_str(), FHSufxBranchNo, RtnVal );
}
//---------------------------------------------------------------------------
void HSufxObjStockOpt::HSufxNewOrder( TNewOrderMessage* Msg, const UFC::AnsiString& UserDefine )
{
	UFC::AnsiString ExchangeCode( Msg->GetExchangeCode() );
	UFC::AnsiString ErrMsg;
	bool            CanSend = true;
	int             OrderRef;
	UFC::AnsiString StrOrderType;
	UFC::AnsiString StrTimeInForce;
	UFC::AnsiString StrEnstrustProp;
	char charPE;

	UFC::BufferedLog::Printf( " -------------------- User Place New Order -----------------------" );

	// 1. New T2SDK BizMessage
	IBizMessage* lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxObjStockOpt::HSufxNewOrder() NewBizMessage failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 338011 );
	lpBizMessage->SetSystemNo( FHSufxSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 2. New T2SDK Packet
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2);
	if ( pPacker == NULL )
	{
		ErrMsg.Printf( "HSufxObjStockOpt::HSufxNewOrder() NewPacker failed!!" );
		FSpeedyConnPtr->Reject( nsOrderMessageDefine::crrNew, ErrMsg, Msg ,Msg->GetTradingSessionID());
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();

	// 3. Add T2SDK Packet Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );			///< Require
	pPacker->AddField( "op_entrust_way", 'C', 1 );			///< Require
	pPacker->AddField( "op_station", 'S', 255 );			///< Require
	pPacker->AddField( "branch_no", 'I', 5 );				///< Require
	pPacker->AddField( "client_id", 'S', 18 );				///< Require
	pPacker->AddField( "fund_account", 'S', 18 );			///< Require
	pPacker->AddField( "password", 'S', 10 );				///< Require
	pPacker->AddField( "password_type", 'C', 1 );			///< Require
	pPacker->AddField( "user_token", 'S', 40 );				///< NonRequire
	pPacker->AddField( "asset_prop", 'C', 1 );				///< Require
	pPacker->AddField( "exchange_type", 'S', 4 );			///< Require
	pPacker->AddField( "option_code", 'S', 8 );				///< Require
	pPacker->AddField( "entrust_amount", 'F', 16, 2 );		///< Require
	pPacker->AddField( "opt_entrust_price", 'F', 11, 4 );	///< Require
	pPacker->AddField( "entrust_bs", 'C', 1 );				///< Require
	pPacker->AddField( "entrust_oc", 'C', 1 );				///< Require
	pPacker->AddField( "covered_flag", 'C', 1 );			///< Require
	pPacker->AddField( "entrust_prop", 'S', 3 );			///< Require

	// 4. Add T2SDK Packet Field Value
	pPacker->AddInt( FHSufxBranchNo );							///< op_branch_no
	pPacker->AddChar( '7' );									///< op_entrust_way		// ?? from config ??
	pPacker->AddStr( "1" );										///< op_station
	pPacker->AddInt( FHSufxBranchNo );							///< branch_no
	pPacker->AddStr( FHSufxClientID );							///< client_id
	pPacker->AddStr( Msg->GetAccount() );						///< fund_account
	pPacker->AddStr( FSpeedyConnPtr->FPassword.c_str() );		///< password
	pPacker->AddChar( '2' );									///< password_type
	pPacker->AddStr( FHSufxUserToken );							///< user_token
	pPacker->AddChar( 'B' );									///< asset_prop
	pPacker->AddStr( HSufxTo_exchange_type( ExchangeCode ) );	///< exchange_type
	pPacker->AddStr( Msg->GetSymbol() );						///< option_code
	pPacker->AddDouble( Msg->GetOrderQty() );					///< entrust_amount
	pPacker->AddDouble( Msg->GetPrice() );						///< opt_entrust_price
	// Side (B/S)  (entrust_bs)
	if ( Msg->GetSide() == nsOrderMessageDefine::sBuy )
		pPacker->AddChar( '1' );
	else
		pPacker->AddChar( '2' );
	// Position effect  (entrust_oc)
	switch ( Msg->GetPositionEffect() )
	{
	case nsOrderMessageDefine::peOpen         : charPE = 'O'; break;
	case nsOrderMessageDefine::peClose        : charPE = 'C'; break;
	case nsOrderMessageDefine::peRolled       : charPE = 'C'; break;
	case nsOrderMessageDefine::peDayTradeOpen : charPE = 'O'; break;
	default : ///< peAuto peAutoToday peTMPMarketMaker
		charPE = 'O'; break;
	}
	pPacker->AddChar( charPE );					///< entrust_oc
	pPacker->AddChar( ' ' );					///< covered_flag
	// OrderType + TimeInForce  (entrust_prop)
	if ( Msg->GetOrderType() == nsOrderMessageDefine::otMLtoLimit )
	{
		StrOrderType = "MLtoLimit";
		switch ( Msg->GetTimeInForce() )
		{
			case nsOrderMessageDefine::tifROD :
				StrTimeInForce  = "ROD";
				StrEnstrustProp = "OPD";
				break;
			case nsOrderMessageDefine::tifIOC :
				ErrMsg.Printf( "HSufxObjStockOpt::HSufxNewOrder() No support [MLtoLimit] [IOC] Order " );
				FSpeedyConnPtr->Reject( nsOrderMessageDefine::crrNew, ErrMsg, Msg ,Msg->GetTradingSessionID());
				goto EXIT;
			case nsOrderMessageDefine::tifFOK :
				ErrMsg.Printf( "HSufxObjStockOpt::HSufxNewOrder() No support [MLtoLimit] [FOK] Order " );
				FSpeedyConnPtr->Reject( nsOrderMessageDefine::crrNew, ErrMsg, Msg ,Msg->GetTradingSessionID());
				goto EXIT;
			default:
				ErrMsg.Printf( "HSufxObjStockOpt::HSufxNewOrder() No support [MLtoLimit] [TimeInForce] Order " );
				FSpeedyConnPtr->Reject( nsOrderMessageDefine::crrNew, ErrMsg, Msg ,Msg->GetTradingSessionID());
				goto EXIT;
		}
	}
	else if ( Msg->GetOrderType() == nsOrderMessageDefine::otMarket )
	{
		StrOrderType = "Market";
		switch ( Msg->GetTimeInForce() )
		{
			case nsOrderMessageDefine::tifROD :
				ErrMsg.Printf( "HSufxObjStockOpt::HSufxNewOrder() No support [Market] [ROD] Order " );
				FSpeedyConnPtr->Reject( nsOrderMessageDefine::crrNew, ErrMsg, Msg ,Msg->GetTradingSessionID());
				goto EXIT;
			case nsOrderMessageDefine::tifIOC :
				StrTimeInForce  = "IOC";
				StrEnstrustProp = "OPB";
				break;
			case nsOrderMessageDefine::tifFOK :
				StrTimeInForce  = "FOK";
				StrEnstrustProp = "OPC";
				break;
			default:
				ErrMsg.Printf( "HSufxObjStockOpt::HSufxNewOrder() No support [Market] [TimeInForce] Order " );
				FSpeedyConnPtr->Reject( nsOrderMessageDefine::crrNew, ErrMsg, Msg ,Msg->GetTradingSessionID());
				goto EXIT;
		}
	}
	else
	{
		StrOrderType = "Limit";
		switch ( Msg->GetTimeInForce() )
		{
			case nsOrderMessageDefine::tifROD :
				StrTimeInForce  = "ROD";
				StrEnstrustProp = "0";
				break;
			case nsOrderMessageDefine::tifIOC :
				ErrMsg.Printf( "HSufxObjStockOpt::HSufxNewOrder() No support [Limit] [IOC] Order " );
				FSpeedyConnPtr->Reject( nsOrderMessageDefine::crrNew, ErrMsg, Msg ,Msg->GetTradingSessionID());
				goto EXIT;
			case nsOrderMessageDefine::tifFOK :
				StrTimeInForce  = "FOK";
				StrEnstrustProp = "OPA";
				break;
			default:
				StrTimeInForce  = "ROD";
				StrEnstrustProp = "0";
				break;
		}
	}
	pPacker->AddStr( StrEnstrustProp.c_str() );
	pPacker->EndPack();

	///< Allow to place New order ?
	if ( FSpeedyConnPtr->FSendEventListener != NULL )
	{
		FSpeedyConnPtr->FSendEventListener->OnBeforeSend( Msg->GetMarket(), smtNew, Msg, CanSend, ErrMsg );
		if ( CanSend == false )
		{
			FSpeedyConnPtr->Reject( nsOrderMessageDefine::crrNew, ErrMsg, Msg,Msg->GetTradingSessionID() );
			goto EXIT;
		}
	}

	// 5. Send T2SDK Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	OrderRef = FHSufxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	// NID has set by OrderAdapter, no need to set NID
	//Msg->SetNID( OrderRef );

	UFC::BufferedLog::Printf( " OrderRef[%d] Account[%s] Symbol[%s] Side[%c] NID[%lld]",
							  OrderRef, Msg->GetAccount(), Msg->GetSymbol(),
							  Msg->GetSide() == nsOrderMessageDefine::sBuy ? 'B':'S', Msg->GetNID() );
	UFC::BufferedLog::Printf( " Qty[%d] Px[%0.3f] TIF[%s] OT[%s] entrust_prod[%s] PE[%c]",
							  Msg->GetOrderQty(), Msg->GetPrice(), StrTimeInForce.c_str(),
							  StrOrderType.c_str(), StrEnstrustProp.c_str() , charPE );
	if ( OrderRef < 0 )
	{
		UFC::BufferedLog::Printf( " ----------------------- Send failed:%d ------------------------", OrderRef );
		UTF8String ErrMsgUTF8( AnsiToUtf8Ex(FHSufxConnection->GetErrorMsg(OrderRef), 936) );
		ErrMsg.Printf( "Call SendBizMsg(338011) return error, Code[%d] Msg[%s]", OrderRef, ErrMsgUTF8.c_str() );
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
void HSufxObjStockOpt::HSufxCancelOrder( TCancelOrderMessage* Msg, const UFC::AnsiString& /*UserDefine*/ )
{
	UFC::AnsiString OrderID( Msg->GetOrderID() );
	UFC::AnsiString ExchangeCode( Msg->GetExchangeCode() );
	UFC::AnsiString ErrMsg;
	bool            CanSend = true;
	COrderInfo*     DelOrd;
	int             CancelOrderRef;
	int             OrgOrderRef = COrderInfo::ToOrderRef( Msg->GetCNID() );
	int             OrderIDInt  = OrderID.ToInt();

	UFC::BufferedLog::Printf( " ------------------ User Place Cancel Order ----------------------" );

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
	if( (DelOrd = FSpeedyConnPtr->FindOrderByOrderLocalID( "0", OrderIDInt )) != NULL ) ///< use 0 replace ExchangeID
		UFC::BufferedLog::Printf( " Cancel by OrderLocalID[%s]", OrderID.c_str()  );
	else if( (DelOrd = FSpeedyConnPtr->FindOrderByOrderRef( 0, FHSufxLoginTimes, OrgOrderRef )) != NULL )
		UFC::BufferedLog::Printf( " Cancel by FrontID[%d] SessionID[%d] OrderRef[%d]", 0, FHSufxLoginTimes, OrgOrderRef );
	else
	{
		ErrMsg.Printf( "Order not found! OrderLocalID[%d]", OrderIDInt );
		FSpeedyConnPtr->Reject( nsOrderMessageDefine::crrCancel, ErrMsg, Msg ,Msg->GetTradingSessionID());
		UFC::BufferedLog::Printf( " Cancel Order not found! OrderID[%s] CNID_KEY[%d|%d|%d]"
								  , OrderID.c_str(), 0, FHSufxLoginTimes, OrgOrderRef );
		UFC::BufferedLog::Printf( " --------------------- Reject Order Cancel -----------------------" );
		return;
	}

	// 2. New T2SDK BizMessage
	IBizMessage* lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		UFC::BufferedLog::Printf( " HSufxObjStockOpt::HSufxCancelOrder() NewBizMessage failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 338012 );
	lpBizMessage->SetSystemNo( FHSufxSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 3. New T2SDK Packet
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2);
	if ( pPacker == NULL )
	{
		ErrMsg.Printf( "HSufxObjStockOpt::HSufxCancelOrder() NewPacker failed!!" );
		FSpeedyConnPtr->Reject( nsOrderMessageDefine::crrCancel, ErrMsg, Msg ,Msg->GetTradingSessionID());
		lpBizMessage->Release();
		return;
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
	pPacker->AddField( "password", 'S', 10 );		///< Require
	pPacker->AddField( "password_type", 'C', 1 );	///< NonRequire
	pPacker->AddField( "user_token", 'S', 40 );		///< NonRequire
	pPacker->AddField( "asset_prop", 'C', 1 );		///< Require
	pPacker->AddField( "exchange_type", 'S', 4 );	///< NonRequire
	pPacker->AddField( "entrust_no", 'I', 8 );		///< Require

	// 5. Add T2SDK Packet Field Value
	pPacker->AddInt( FHSufxBranchNo );							///< op_branch_no
	pPacker->AddChar( '7' );									///< op_entrust_way		// ?? from config ??
	pPacker->AddStr( "1" );										///< op_station
	pPacker->AddInt( FHSufxBranchNo );							///< branch_no
	pPacker->AddStr( FHSufxClientID );							///< client_id
	pPacker->AddStr( Msg->GetAccount() );						///< fund_account
	pPacker->AddStr( FSpeedyConnPtr->FPassword.c_str() );		///< password
	pPacker->AddChar( '2' );									///< password_type
	pPacker->AddStr( FHSufxUserToken );							///< user_token
	pPacker->AddChar( 'B' );									///< asset_prop
	pPacker->AddStr( HSufxTo_exchange_type( ExchangeCode ) );	///< exchange_type
	pPacker->AddInt( OrderIDInt );								///< entrust_no
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

	// 6. Send T2SDK Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	CancelOrderRef = FHSufxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	// NID has set by OrderAdapter, no need to set NID
	//Msg->SetNID( CancelOrderRef );

	UFC::BufferedLog::Printf( " CancelOrderRef[%d] Account[%s] ExchangeID[%s] Symbol[%s] NID[%lld]",
							  CancelOrderRef, Msg->GetAccount(), DelOrd->GetExchangeID(), Msg->GetSymbol(), Msg->GetNID() );
	UFC::BufferedLog::Printf( " FrontID[%d] SessionID[%d] OrderActionRef[%d] OrderSysID[%s]",
							  0, FHSufxLoginTimes, CancelOrderRef, OrderID.c_str() );
	if ( CancelOrderRef < 0 )
	{
		UFC::BufferedLog::Printf( " ----------------------- Send failed:%d ------------------------", CancelOrderRef );
		UTF8String ErrMsgUTF8( AnsiToUtf8Ex(FHSufxConnection->GetErrorMsg(CancelOrderRef), 936) );
		ErrMsg.Printf( "Call SendBizMsg(338012) return error, Code[%d] Msg[%s]", CancelOrderRef, ErrMsgUTF8.c_str() );
		FSpeedyConnPtr->Reject( nsOrderMessageDefine::crrCancel, ErrMsg, Msg ,Msg->GetTradingSessionID());
	}
	else
	{
		UFC::BufferedLog::Printf( " -------------------- Send Cancel Order OK -----------------------" );
		// Add CancelOrderRef to OrderLocalID hash map
		UFC::AnsiString OrderCancelRefKey;
		OrderCancelRefKey.Printf( "%d|%d|%d", 0, FHSufxLoginTimes, CancelOrderRef );
		FHSufxCancelRefToOrdLocalID.Add( OrderCancelRefKey, OrderIDInt );
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
int HSufxObjStockOpt::HandleHSufxMessage( TNewsMessage* Msg )
{
	int NewsID = Msg->GetID();

	switch ( NewsID )
	{
		case CTP_QuerySettlementInfoConfirm       : return 0; ///< no implementation temporarily
		case CTP_SettlementInfoConfirm            : return 0; ///< no implementation temporarily
		case CTP_QueryInvestorPosition            : return HSufxSendQryPosition( Msg );
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
