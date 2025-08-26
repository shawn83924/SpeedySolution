//---------------------------------------------------------------------------
#ifdef _CTP
//------------------------------------------------------------------------------------------------------------------------
#ifdef _KSFT
//------------------------------------------------------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//------------------------------------------------------------------------------------------------------------------------
#include "KSftConnectionImp.h"
//------------------------------------------------------------------------------------------------------------------------
typedef AnsiStringT<936> GB2312String;
//------------------------------------------------------------------------------------------------------------------------
//  ExecID Rule
//
//  Int32: FrontID, SessionID, OrderRef
//  Int64: OrderSysID
//
//  PendingNew ExecID: "P" + FrontID + SessionID + OrderRef
//  Reject     ExecID: "R" + FrontID + SessionID + OrderRef
//  NewOrder   ExecID: "N" + FrontID + SessionID + OrderRef
//  Fill       ExecID: "F" + ExchangeID + OrderSysID + SequenceNo (KSft only support V8, use TradeID replace)
//  Fill       ExecID: "F" + ExchangeID + OrderSysID + TradeID
//  Canceled   ExecID: "C" + FrontID + SessionID + OrderRef
//------------------------------------------------------------------------------------------------------------------------
void OrderSubmitStatusString( KS_::TThostFtdcOrderSubmitStatusType osst, UFC::AnsiString& Str )
{
	switch( osst )
	{
		case THOST_FTDC_OSS_InsertSubmitted: Str ="THOST_FTDC_OSS_InsertSubmitted"; break;
		case THOST_FTDC_OSS_CancelSubmitted: Str ="THOST_FTDC_OSS_CancelSubmitted"; break;
		case THOST_FTDC_OSS_ModifySubmitted: Str ="THOST_FTDC_OSS_ModifySubmitted"; break;
		case THOST_FTDC_OSS_Accepted       : Str ="THOST_FTDC_OSS_Accepted"; break;
		case THOST_FTDC_OSS_InsertRejected : Str ="THOST_FTDC_OSS_InsertRejected"; break;
		case THOST_FTDC_OSS_CancelRejected : Str ="THOST_FTDC_OSS_CancelRejected"; break;
		case THOST_FTDC_OSS_ModifyRejected : Str ="THOST_FTDC_OSS_ModifyRejected"; break;
		default : Str.Printf( "Value[%c] not define",osst ); break;
	}
}
//------------------------------------------------------------------------------------------------------------------------
void OrderStatusString( KS_::TThostFtdcOrderStatusType ost, UFC::AnsiString& Str )
{
	switch( ost )
	{
		case THOST_FTDC_OST_AllTraded            : Str ="THOST_FTDC_OST_AllTraded"; break;
		case THOST_FTDC_OST_PartTradedQueueing   : Str ="THOST_FTDC_OST_PartTradedQueueing"; break;
		case THOST_FTDC_OST_PartTradedNotQueueing: Str ="THOST_FTDC_OST_PartTradedNotQueueing"; break;
		case THOST_FTDC_OST_NoTradeQueueing      : Str ="THOST_FTDC_OST_NoTradeQueueing"; break;
		case THOST_FTDC_OST_NoTradeNotQueueing   : Str ="THOST_FTDC_OST_NoTradeNotQueueing"; break;
		case THOST_FTDC_OST_Canceled             : Str ="THOST_FTDC_OST_Canceled"; break;
		case THOST_FTDC_OST_Unknown              : Str ="THOST_FTDC_OST_Unknown"; break;
		case THOST_FTDC_OST_NotTouched           : Str ="THOST_FTDC_OST_NotTouched"; break;
		case THOST_FTDC_OST_Touched              : Str ="THOST_FTDC_OST_Touched"; break;
		default : Str.Printf( "Value[%c] not define",ost ); break;
	}
}
//------------------------------------------------------------------------------------------------------------------------
//
// Implement TTaifexConnection
// KSft functions
//
//------------------------------------------------------------------------------------------------------------------------
COrderInfo* TTaifexConnection::AddPendingOrder( int OrdRef, KS_::CThostFtdcInputOrderField& Ord, const UFC::AnsiString& UserDefine )
{
	COrderInfo* NewOrd = new COrderInfo( FCTPFrontID, FCTPSessionID, Ord, UserDefine );
	UFC::AnsiString OrderRefKey;

	OrderRefKey.Printf( "%d|%d|%d", FCTPFrontID, FCTPSessionID, OrdRef );
	FOrderRecords.Add( NewOrd );
	FOrderRefToRecord.Add( OrderRefKey, NewOrd );
	return NewOrd;
}
//------------------------------------------------------------------------------------------------------------------------
COrderInfo* TTaifexConnection::AddPendingOrder( KS_::CThostFtdcOrderField* Ord )
{
	COrderInfo* NewOrd = new COrderInfo( Ord );
	UFC::AnsiString OrderRefKey;

	OrderRefKey.Printf( "%d|%d|%d", Ord->FrontID, Ord->SessionID, atoi( Ord->OrderRef ) );
	FOrderRecords.Add( NewOrd );
	FOrderRefToRecord.Add( OrderRefKey, NewOrd );
	return NewOrd;
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::KSftPendingExecution( COrderInfo* Ord, KS_::CThostFtdcOrderField *pOrder )
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	GB2312String            ErrMsg( pOrder->StatusMsg );
	UFC::AnsiString         OrderLocalID( pOrder->OrderLocalID );
	Int64                   NID64 = Ord->GetOrderNID();
	bool IsDup;

	UFC::BufferedLog::Printf( " --------------------------- KSft Pending ------------------------" );
	ExecID.Printf( "P_%d_%d_%d", Ord->GetFrontID(), Ord->GetSessionID(), Ord->GetOrderRef() );
	OrderLocalID.TrimLeft();
	CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Ord ); // ??
	ExecutionReport.SetOrderID( OrderLocalID.c_str() );
	ExecutionReport.SetNID( NID64 );
	ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osPendingNew );
	ExecutionReport.SetExecType( nsOrderMessageDefine::etPendingNew );
	ExecutionReport.SetAllUserData( Ord->GetUserData() );
	ExecutionReport.SetExecID( ExecID.c_str() );
	ExecutionReport.SetLastQty( 0 );
	ExecutionReport.SetTransactTime( pOrder->InsertTime );
	UFC::BufferedLog::Printf( " NID[%s] ExchangeID[%s] Symbol[%s] OrderSysID[%s] OrderLocalID[%s]"
							  , pOrder->OrderRef, pOrder->ExchangeID, pOrder->InstrumentID, pOrder->OrderSysID, pOrder->OrderLocalID );
	IsDup = IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
	if ( ErrMsg.Length() > 0 )
	{
		UTF8String ErrMsgUTF8( ErrMsg );
		ExecutionReport.SetText( ErrMsgUTF8.c_str() );
		UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] Msg[%s] NID[%lld]", ExecID.c_str(), IsDup ? "Yes" : "No", ErrMsgUTF8.c_str(), ExecutionReport.GetNID() );
	}
	else
		UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] ", ExecID.c_str(), IsDup ? "Yes" : "No" );
	TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() ) );
	UFC::BufferedLog::Printf( " ---------------- Sent Pending New Execution ---------------------" );
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::KSftConfirmExecution( COrderInfo* Ord, KS_::CThostFtdcOrderField *pOrder )
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	GB2312String            ErrMsg( pOrder->StatusMsg );
	UFC::AnsiString         OrderSysID( pOrder->OrderSysID );
	bool IsDup;

	UFC::BufferedLog::Printf( " --------------------------- KSft Confirm ------------------------" );
	ExecID.Printf( "N_%d_%d_%d", Ord->GetFrontID(), Ord->GetSessionID(), Ord->GetOrderRef() );
	OrderSysID.TrimLeft();
	CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Ord ); // ??
	ExecutionReport.SetOrderID( OrderSysID.c_str() );
	ExecutionReport.SetNID( Ord->GetOrderNID() );
	ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osNew );
	ExecutionReport.SetExecType( nsOrderMessageDefine::etNew );
	ExecutionReport.SetAllUserData( Ord->GetUserData() );
	ExecutionReport.SetExecID( ExecID.c_str() );
	ExecutionReport.SetLastQty( 0 );
	ExecutionReport.SetTransactTime( pOrder->InsertTime );
	UFC::BufferedLog::Printf( " NID[%s] ExchangeID[%s] Symbol[%s] OrderSysID[%s] OrderLocalID[%s]"
							  , pOrder->OrderRef, pOrder->ExchangeID, pOrder->InstrumentID, pOrder->OrderSysID, pOrder->OrderLocalID );
	IsDup = IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
	if ( ErrMsg.Length() > 0 )
	{
		UTF8String   ErrMsgUTF8( ErrMsg );
		ExecutionReport.SetText( ErrMsgUTF8.c_str() );
		UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] Msg[%s] NID[%lld]", ExecID.c_str(), IsDup ? "Yes" : "No", ErrMsgUTF8.c_str(), ExecutionReport.GetNID() );
	}
	else
		UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] ", ExecID.c_str(), IsDup ? "Yes" : "No" );
	TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	UFC::BufferedLog::Printf( " ----------------- Sent Confirmed Execution ----------------------" );
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::KSftRejectExecution( COrderInfo* Ord, KS_::CThostFtdcOrderField *pOrder )
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	GB2312String            ErrMsg( pOrder->StatusMsg );
	UFC::AnsiString         OrderLocalID( pOrder->OrderLocalID );
	UFC::AnsiString         OrderSysID( pOrder->OrderSysID );
	bool IsDup;

	UFC::BufferedLog::Printf( " ------------------------- KSft Reject ---------------------------" );
	ExecID.Printf( "R_%d_%d_%d", Ord->GetFrontID(), Ord->GetSessionID(), Ord->GetOrderRef() );
	OrderLocalID.TrimLeft();
	OrderSysID.TrimLeft();
	CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Ord ); // ??
	if ( OrderSysID.Length() == 0 )
		ExecutionReport.SetOrderID( OrderLocalID.c_str() );
	else
		ExecutionReport.SetOrderID( OrderSysID.c_str() );
	ExecutionReport.SetNID( Ord->GetOrderNID() );
	ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osRejected );
	ExecutionReport.SetExecType( nsOrderMessageDefine::etRejected );
	ExecutionReport.SetCxlRejResponseTo( nsOrderMessageDefine::crrNew );
	ExecutionReport.SetAllUserData( Ord->GetCxlUserData() );
	ExecutionReport.SetExecID( ExecID.c_str() );
	ExecutionReport.SetLastQty( 0 );
	ExecutionReport.SetTransactTime( pOrder->CancelTime );
	UFC::BufferedLog::Printf( " NID[%s] ExchangeID[%s] Symbol[%s] OrderSysID[%s] OrderLocalID[%s]"
							  , pOrder->OrderRef, pOrder->ExchangeID, pOrder->InstrumentID, pOrder->OrderSysID, pOrder->OrderLocalID );
	IsDup = IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
	if ( ErrMsg.Length() > 0 )
	{
		UTF8String   ErrMsgUTF8( ErrMsg );
		ExecutionReport.SetText( ErrMsgUTF8.c_str() );
		UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] Msg[%s] NID[%lld]", ExecID.c_str(), IsDup ? "Yes" : "No", ErrMsgUTF8.c_str(), ExecutionReport.GetNID() );
	}
	else
		UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] ", ExecID.c_str(), IsDup ? "Yes" : "No" );
	TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	UFC::BufferedLog::Printf( " ------------------- Sent Reject Execution -----------------------" );
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::KSftCanceledExecution( COrderInfo* Ord, KS_::CThostFtdcOrderField *pOrder )
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	GB2312String            ErrMsg( pOrder->StatusMsg );
	UFC::AnsiString         OrderLocalID( pOrder->OrderLocalID );
	UFC::AnsiString         OrderSysID( pOrder->OrderSysID );
	bool IsDup;

	UFC::BufferedLog::Printf( " --------------------------- KSft Cancled ------------------------" );
	ExecID.Printf( "C_%d_%d_%d", Ord->GetFrontID(), Ord->GetSessionID(), Ord->GetOrderRef() );
	OrderLocalID.TrimLeft();
	OrderSysID.TrimLeft();
	CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Ord ); // ??
	if ( OrderSysID.Length() == 0 )
		ExecutionReport.SetOrderID( OrderLocalID.c_str() );
	else
		ExecutionReport.SetOrderID( OrderSysID.c_str() );
	if ( Ord->GetCancelRef() == 0 )
		ExecutionReport.SetNID( Ord->GetOrderNID() );
	else
		ExecutionReport.SetNID( Ord->GetCancelNID() );
	ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osCanceled );
	ExecutionReport.SetExecType( nsOrderMessageDefine::etCanceled );
	ExecutionReport.SetAllUserData( Ord->GetCxlUserData() );
	ExecutionReport.SetExecID( ExecID.c_str() );
	ExecutionReport.SetLastQty( 0 );
	// !! AuoDe Code
	if ( strcmp( pOrder->CancelTime, "" ) == 0 || strcmp( pOrder->CancelTime, " " ) == 0 )
		ExecutionReport.SetTransactTime( pOrder->InsertTime );
	else
		ExecutionReport.SetTransactTime( pOrder->CancelTime );
	// !!

	UFC::BufferedLog::Printf( " OrdNID[%s] NID[%lld] ExchangeID[%s] Symbol[%s] OrderSysID[%s] OrderLocalID[%s]"
							  , pOrder->OrderRef, ExecutionReport.GetNID(), pOrder->ExchangeID, pOrder->InstrumentID, pOrder->OrderSysID, pOrder->OrderLocalID );
	IsDup = IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
	if ( ErrMsg.Length() > 0 )
	{
		UTF8String   ErrMsgUTF8( ErrMsg );
		ExecutionReport.SetText( ErrMsgUTF8.c_str() );
		UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] Msg[%s] NID[%lld]", ExecID.c_str(), IsDup ? "Yes" : "No", ErrMsgUTF8.c_str(), ExecutionReport.GetNID() );
	}
	else
		UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] ", ExecID.c_str(), IsDup ? "Yes" : "No" );
	TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	UFC::BufferedLog::Printf( " ------------------ Sent Canceled Execution ----------------------" );
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::KSftFillExecution( COrderInfo* Ord, KS_::CThostFtdcTradeField *pTrade )
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString OrdSysIDStr( pTrade->OrderSysID );
	UFC::AnsiString ExecID;
	bool IsDup;

	OrdSysIDStr.TrimLeft();
	UFC::BufferedLog::Printf( " ------------------------- KSft Fill -----------------------------" );
	// SequenceNo KSft only support V8, use TradeID replace
	//ExecID.Printf( "F_%s_%lld_%d", pTrade->ExchangeID, OrdSysIDStr.ToInt64(), pTrade->SequenceNo );
	ExecID.Printf( "F_%s_%lld_%s", pTrade->ExchangeID, OrdSysIDStr.ToInt64(), pTrade->TradeID );
	if ( ( IsDup = IsExecutionDup( nsOrderMessageDefine::mCNFutures, ExecID ) ) == false ) // ??
		Ord->Fill( pTrade->Volume );
	UFC::BufferedLog::Printf( " NID[%s] ExchangeID[%s] Symbol[%s] OrderSysID[%s] OrderLocalID[%s]"
							  , pTrade->OrderRef, pTrade->ExchangeID, pTrade->InstrumentID, pTrade->OrderSysID, pTrade->OrderLocalID );
	UFC::BufferedLog::Printf( " Account[%s] Time[%s] Side[%c] "
							  , pTrade->UserID, pTrade->TradeTime, pTrade->Direction == THOST_FTDC_D_Buy? 'B':'S' );
	UFC::BufferedLog::Printf( " LastPx[%0.3f] LastQty[%d] CumQty[%d] LeavesQty[%d]"
							  , pTrade->Price, pTrade->Volume, Ord->GetCumQty(), Ord->GetLeavesQty() );
	UFC::BufferedLog::Printf( " ExecID[%s] TradeID[%s] FillSeq[%d] Dup[%s] NID[%lld]"
							  , ExecID.c_str(), pTrade->TradeID , pTrade->SequenceNo, IsDup ? "Yes" : "No", Ord->GetOrderNID());
	CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Ord ); // ??
	ExecutionReport.SetOrderID( OrdSysIDStr.c_str() );
	ExecutionReport.SetNID( Ord->GetOrderNID() );
	if ( Ord->GetLeavesQty() == 0 )
	{
		ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osFilled );
		ExecutionReport.SetExecType( nsOrderMessageDefine::etFilled );
	}
	else
	{
		ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osPartiallyFilled );
		ExecutionReport.SetExecType( nsOrderMessageDefine::etPartiallyFilled );
	}
	ExecutionReport.SetAllUserData( Ord->GetUserData() );
	ExecutionReport.SetExecID( ExecID.c_str() );
	ExecutionReport.SetPrice( pTrade->Price );
	ExecutionReport.SetLastPx( pTrade->Price );
	ExecutionReport.SetOrderQty( pTrade->Volume );
	ExecutionReport.SetLastQty( pTrade->Volume );
	ExecutionReport.SetTransactTime( pTrade->TradeTime );
	TrigerOnExecutionReport( &ExecutionReport,(ExecDup) IsDup );
	UFC::BufferedLog::Printf( " -------------------- Sent Fill Execution ------------------------" );
}
//------------------------------------------------------------------------------------------------------------------------
//
// Implement KSft SPI callback functions
//
// OnKSftAPIConnected, OnKSftAPIDisconnected Trigger by KSft API thread.
// Not the same thread as the main UI thread!
// Don't modify any GDI object in this function
//
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftAPIConnected()
{
	// this function is non trigger by min UI thread
	// Don't modify any GDI object int this function
	FIsConnected = true;
	FLogonEvent.SetEvent();
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftAPIDisconnected( int /*nReason*/ )
{
	// this function is non trigger by min UI thread
	// Don't modify any GDI object int this function
	FIsConnected = false;
	FIsLogon = false;
	DetachShm();
/*
	if ( FCTPTransLog != NULL )
	{
		FCTPTransLog->Flush();
		delete FCTPTransLog;
		FCTPTransLog = NULL;
	}
*/
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftAPIRecoverFinished( int Count )
{
	if ( FListener != NULL )
		FListener->OnRecoverFinished( Count );
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftFrontConnected( )
{
	if ( FListener != NULL )
	{
		if ( FTriggerCTPOnConnected == true )
		{
			UFC::SleepMS( 330 );
			FTriggerCTPOnConnected = false;
			FListener->OnConnected();
		}
		else
		{
			TNewsMessage Msg;

			Msg.SetHeadline( "KSFT" );
			Msg.SetID( CTP_OnConnected );
			FListener->OnNews( &Msg );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftFrontDisconnected( int /*nReason*/ )
{
	if ( FListener != NULL )
	{
		FListener->OnDisconnected();
		TNewsMessage Msg;

		Msg.SetHeadline( "KSFT");
		Msg.SetID( CTP_OnDisconnectd );
		FListener->OnNews( &Msg );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspUserLogin( KS_::CThostFtdcRspUserLoginField *pRspUserLogin, KS_::CThostFtdcRspInfoField *pRspInfo, int /*nRequestID*/, bool /*bIsLast*/ )
{
	LogonResult  Result = lrFailed;
	AnsiString   ReplyString( "" );

	FAdmin = 0;
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 ) ///< Logon failed.
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		UTF8String   ErrMsgUTF8( ErrMsg );

		ReplyString = ErrMsgUTF8.c_str();
		Result   = lrFailed;
		FIsLogon = false;
		FCurrentConnectionID = 0;
		UFC::BufferedLog::Printf( " OnKSftRspUserLogin: Code[%d] Msg[%s] Logon failed."
								  , pRspInfo->ErrorID, ErrMsgUTF8.c_str() );
		if ( FListener != NULL )
			FListener->OnLogonReply( ReplyString.c_str(), Result, FCurrentConnectionID );
	}
	else ///< Logon OK
	{
		// Prevent 1st parameter null reference error
		if ( pRspUserLogin == NULL )
		{
			UFC::BufferedLog::Printf( " OnKSftRspUserLogin: pRspUserLogin is NULL." );
			UFC::BufferedLog::FlushToFile();
			return;
		}

		//UFC::AnsiString FileName;
		TNewsMessage Msg, ReqMsg;
		UFC::NameValueMessage TimeMsg("^\n");

		FTradeingDate = pRspUserLogin->TradingDay;
		FIsRecover = false;
		FRecoverCount = 0;
		FLastRecover = UFC::GetTickCountMS();
		RemoveShm( "KSFT" );
		AttachShm( "KSFT", pRspUserLogin->TradingDay );
		FCTPFrontID   = pRspUserLogin->FrontID;
		FCTPSessionID = pRspUserLogin->SessionID;
		Result   = lrOk;
		FIsLogon = true;
		FUseNID64 = true;
		FCurrentConnectionID = pRspUserLogin->SessionID;
		UFC::CreateDir( "./log" );
		//FileName.Printf( "./log/KSftLog_%s_%s.log", pRspUserLogin->UserID, pRspUserLogin->TradingDay );
		//CTPRecover( FileName );
		//if ( FCTPTransLog != NULL )
		//	delete FCTPTransLog;
		//FCTPTransLog = new UFC::FileStream64( FileName.c_str(), "a" );
		UFC::BufferedLog::Printf( " OnKSftRspUserLogin: Account[%s] FrontID[%d] SessionID[%d] Logon OK!"
								  , pRspUserLogin->UserID, pRspUserLogin->FrontID, pRspUserLogin->SessionID );
		if ( FListener != NULL )
			FListener->OnLogonReply( ReplyString.c_str(), Result, FCurrentConnectionID );
		// Update Exchange clocks.
		Msg.SetHeadline( "KSFT" );
		Msg.SetID( CTP_OnExchangeTime );
		TimeMsg.Append( "SHFE",  pRspUserLogin->SHFETime );
		TimeMsg.Append( "DCE",   pRspUserLogin->DCETime );
		TimeMsg.Append( "CZCE",  pRspUserLogin->CZCETime );
		TimeMsg.Append( "CFFEX", pRspUserLogin->FFEXTime );
		// ?? //TimeMsg.Append( "INE", pRspUserLogin->INETime );
		Msg.SetText( TimeMsg.ToString().c_str() );
		if ( FListener != NULL )
			FListener->OnNews( &Msg );
		// query SettlementInfo confirm.
		ReqMsg.SetHeadline( "KSFT" );
		ReqMsg.SetID( CTP_QuerySettlementInfoConfirm );
		HandleKSftMessage( &ReqMsg );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspUserLogout( KS_::CThostFtdcUserLogoutField */*pUserLogout*/, KS_::CThostFtdcRspInfoField */*pRspInfo*/, int /*nRequestID*/, bool /*bIsLast*/ )
{
	FAdmin   = 0;
	FIsLogon = false; ///< Clear logon flag.
	FID      = "";
	FToken   = "";
	DetachShm();
/*
	if ( FCTPTransLog != NULL )
	{
		FCTPTransLog->Flush();
		delete FCTPTransLog;
		FCTPTransLog = NULL;
	}
*/
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspUserPasswordUpdate( KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int /*nRequestID*/, bool /*bIsLast*/ )
{
	TNewsMessage Msg;

	Msg.SetHeadline( "KSFT" );
	Msg.SetID( CTP_OnPasswordChanged );
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		UTF8String   UTF8Text( ErrMsg );

		Msg.SetText( UTF8Text.c_str() );
		UFC::BufferedLog::Printf( " OnKSftRspUserPasswordUpdate: Change Password failed! Code[%d] Msg[%s]"
								  , pRspInfo->ErrorID, UTF8Text.c_str() );
	}
	else
	{
		// Prevent 1st parameter null reference error
		if ( pUserPasswordUpdate == NULL )
		{
			UFC::BufferedLog::Printf( " OnKSftRspUserPasswordUpdate: pUserPasswordUpdate is NULL." );
			UFC::BufferedLog::FlushToFile();
			return;
		}

		Msg.SetText( "OK" );
		UFC::BufferedLog::Printf( " OnKSftRspUserPasswordUpdate: UserID[%s] Change Password OK! ", pUserPasswordUpdate->UserID );
	}
	if ( FListener != NULL )
		FListener->OnNews( &Msg );
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspTradingAccountPasswordUpdate( KS_::CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int /*nRequestID*/, bool /*bIsLast*/ )
{
	TNewsMessage Msg;

	Msg.SetHeadline( "KSFT" );
	Msg.SetID( CTP_OnPasswordChanged );
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		UTF8String   UTF8Text( ErrMsg );

		Msg.SetText( UTF8Text.c_str() );
		UFC::BufferedLog::Printf( " OnKSftRspTradingAccountPasswordUpdate: Change Password failed! Code[%d] Msg[%s]"
								  , pRspInfo->ErrorID, UTF8Text.c_str() );
	}
	else
	{
		// Prevent 1st parameter null reference error
		if ( pTradingAccountPasswordUpdate == NULL )
		{
			UFC::BufferedLog::Printf( " OnKSftRspTradingAccountPasswordUpdate: pTradingAccountPasswordUpdate is NULL." );
			UFC::BufferedLog::FlushToFile();
			return;
		}

		Msg.SetText( "OK" );
		UFC::BufferedLog::Printf( " OnKSftRspTradingAccountPasswordUpdate: Account[%s] Change Password OK!", pTradingAccountPasswordUpdate->AccountID );
	}
	if ( FListener != NULL )
		FListener->OnNews( &Msg );
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspSettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int /*nRequestID*/, bool /*bIsLast*/ )
{
	if ( pRspInfo == NULL || pRspInfo->ErrorID == 0 )///< Confirm OK
	{   ///< User confirmed Settlement Info.
		// Prevent 1st parameter null reference error
		if ( pSettlementInfoConfirm == NULL )
		{
			UFC::BufferedLog::Printf( " OnKSftRspSettlementInfoConfirm: pSettlementInfoConfirm is NULL." );
			UFC::BufferedLog::FlushToFile();
			return;
		}

		UFC::BufferedLog::Printf( " User[%s] Confirm settlement info at[%s] on[%s]",
								  pSettlementInfoConfirm->InvestorID,
								  pSettlementInfoConfirm->ConfirmTime,
								  pSettlementInfoConfirm->ConfirmDate );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspQryInvestorPosition( KS_::CThostFtdcInvestorPositionField */*pInvestorPosition*/, KS_::CThostFtdcRspInfoField */*pRspInfo*/, int /*nRequestID*/, bool /*bIsLast*/ )
{
// do nothing

/*
	UFC::AnsiString ansiResponseStr = BuildKS_ResponseInformationStr( pRspInfo, "|" );
	AppendFieldToFieldsStr( ansiResponseStr, nRequestID, "|" );
	AppendFieldToFieldsStr( ansiResponseStr, bIsLast, "|" );
	if ( pInvestorPosition != NULL )
	{
		UFC::AnsiString ansiInstrumentId = pInvestorPosition->InstrumentID;  // 5.合約代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiInstrumentId, "|" );
		UFC::AnsiString ansiBrokerId = pInvestorPosition->BrokerID;  // 6.經紀公司代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerId, "|" );
		UFC::AnsiString ansiInvestorId = pInvestorPosition->InvestorID;  // 7.投資者代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiInvestorId, "|" );
		char positionDirection = pInvestorPosition->PosiDirection;  // 8.持倉多空方向 '1':淨  '2':多  '3':空
		AppendFieldToFieldsStr( ansiResponseStr, positionDirection, "|" );
		char hedgeFlag = pInvestorPosition->HedgeFlag;  // 9.投機套保標誌 '1':投機  '2':套利  '3':套保
		AppendFieldToFieldsStr( ansiResponseStr, hedgeFlag, "|" );
		char positionDate = pInvestorPosition->PositionDate;  //10.持倉日期 '1':今日持倉  '2':歷史持倉
		AppendFieldToFieldsStr( ansiResponseStr, positionDate, "|" );
		int ydPosition = pInvestorPosition->YdPosition;  //11.上日持倉
		AppendFieldToFieldsStr( ansiResponseStr, ydPosition, "|" );
		int tdPosition = pInvestorPosition->Position;  //12.今日持倉
		AppendFieldToFieldsStr( ansiResponseStr, tdPosition, "|" );
		int longFrozenPosition = pInvestorPosition->LongFrozen;  //13.多頭凍結
		AppendFieldToFieldsStr( ansiResponseStr, longFrozenPosition, "|" );
		int shortFrozenPosition = pInvestorPosition->ShortFrozen;  //14.空頭凍結
		AppendFieldToFieldsStr( ansiResponseStr, shortFrozenPosition, "|" );
		double longFrozenAmount = pInvestorPosition->LongFrozenAmount;  //15.開倉凍結金額
		AppendFieldToFieldsStr( ansiResponseStr, longFrozenAmount, "|" );
		double shortFrozenAmount = pInvestorPosition->ShortFrozenAmount;  //16.平倉凍結金額
		AppendFieldToFieldsStr( ansiResponseStr, shortFrozenAmount, "|" );
		int openVolume = pInvestorPosition->OpenVolume;  //17.開倉量
		AppendFieldToFieldsStr( ansiResponseStr, openVolume, "|" );
		int closeVolume = pInvestorPosition->CloseVolume;  //18.平倉量
		AppendFieldToFieldsStr( ansiResponseStr, closeVolume, "|" );
		double openAmount = pInvestorPosition->OpenAmount;  //19.開倉金額
		AppendFieldToFieldsStr( ansiResponseStr, openAmount, "|" );
		double closeAmount = pInvestorPosition->CloseAmount;  //20.平倉金額
		AppendFieldToFieldsStr( ansiResponseStr, closeAmount, "|" );
		double positionCost = pInvestorPosition->PositionCost;  //21.持倉成本
		AppendFieldToFieldsStr( ansiResponseStr, positionCost, "|" );
		double preMargin = pInvestorPosition->PreMargin;  //22.上次佔用的保證金
		AppendFieldToFieldsStr( ansiResponseStr, preMargin, "|" );
		double usedMargin = pInvestorPosition->UseMargin;  //23.佔用的保證金
		AppendFieldToFieldsStr( ansiResponseStr, usedMargin, "|" );
		double frozenMargin = pInvestorPosition->FrozenMargin;  //24.凍結的保證金
		AppendFieldToFieldsStr( ansiResponseStr, frozenMargin, "|" );
		double frozenCash = pInvestorPosition->FrozenCash;  //25.凍結的資金
		AppendFieldToFieldsStr( ansiResponseStr, frozenCash, "|" );
		double frozenCommission = pInvestorPosition->FrozenCommission;  //26.凍結的手續費
		AppendFieldToFieldsStr( ansiResponseStr, frozenCommission, "|" );
		double cashIn = pInvestorPosition->CashIn;  //27.資金差額
		AppendFieldToFieldsStr( ansiResponseStr, cashIn, "|" );
		double commission = pInvestorPosition->Commission;  //28.手續費
		AppendFieldToFieldsStr( ansiResponseStr, commission, "|" );
		double closeProfit = pInvestorPosition->CloseProfit;  //29.平倉盈虧
		AppendFieldToFieldsStr( ansiResponseStr, closeProfit, "|" );
		double positionProfit = pInvestorPosition->PositionProfit;  //30.持倉盈虧
		AppendFieldToFieldsStr( ansiResponseStr, positionProfit, "|" );
		double preSettlementPrice = pInvestorPosition->PreSettlementPrice;  //31.上次結算價
		AppendFieldToFieldsStr( ansiResponseStr, preSettlementPrice, "|" );
		double settlementPrice = pInvestorPosition->SettlementPrice;  //32.本次結算價
		AppendFieldToFieldsStr( ansiResponseStr, settlementPrice, "|" );
		UFC::AnsiString tradingDay = pInvestorPosition->TradingDay;  //33.交易日
		AppendFieldToFieldsStr( ansiResponseStr, tradingDay, "|" );
		int settlementID = pInvestorPosition->SettlementID;  //34.結算編號
		AppendFieldToFieldsStr( ansiResponseStr, settlementID, "|" );
		double openCost = pInvestorPosition->OpenCost;  //35.開倉成本
		AppendFieldToFieldsStr( ansiResponseStr, openCost, "|" );
		double exchangeMargin = pInvestorPosition->ExchangeMargin;  //36.交易所保證金
		AppendFieldToFieldsStr( ansiResponseStr, exchangeMargin, "|" );
		int combPosition = pInvestorPosition->CombPosition;  //37.組合成交形成的持倉
		AppendFieldToFieldsStr( ansiResponseStr, combPosition, "|" );
		int combLongFrozen = pInvestorPosition->CombLongFrozen;  //38.組合多頭凍結
		AppendFieldToFieldsStr( ansiResponseStr, combLongFrozen, "|" );
		int combShortFrozen = pInvestorPosition->CombShortFrozen;  //39.組合空頭凍結
		AppendFieldToFieldsStr( ansiResponseStr, combShortFrozen, "|" );
		int todayPosition = pInvestorPosition->TodayPosition;  //40.今日持倉
		AppendFieldToFieldsStr( ansiResponseStr, todayPosition, "|" );
		// ??
		//int strikeFrozen = pInvestorPosition->StrikeFrozen; //41.執行凍結
		//AppendFieldToFieldsStr( ansiResponseStr, strikeFrozen, "|" );
		//double strikeFrozenAmount = pInvestorPosition->StrikeFrozenAmount; //42.執行凍結金額
		//AppendFieldToFieldsStr( ansiResponseStr, strikeFrozenAmount, "|" );
		//int abandonFrozen = pInvestorPosition->AbandonFrozen; //43.放棄執行凍結
		//AppendFieldToFieldsStr( ansiResponseStr, abandonFrozen, "|" );
		//double optionValue = pInvestorPosition->OptionValue; //44.期權市值
		//AppendFieldToFieldsStr( ansiResponseStr, optionValue, "|" );
		UFC::BufferedLog::Printf( " OnKSftRspQryInvestorPosition:%s %s %s PosDate=%c Dir=%c YDPos=%d Pos=%d TradeDate=%s",
								  ansiBrokerId.c_str(), ansiInvestorId.c_str(), ansiInstrumentId.c_str(), positionDate, positionDirection, ydPosition, tdPosition, tradingDay.c_str() );
		UFC::BufferedLog::Printf( " OnKSftRspQryInvestorPosition:Open[Vol=%d Amt=%15.6lf] Close[Vol=%d Amt=%15.6lf] Cost[%15.6lf] Fee[%15.6lf] CloseProfit[%15.6lf] PosProfit[%15.6lf] TodayPos[%d]",
								  openVolume, openAmount, closeVolume, closeAmount, positionCost, commission, closeProfit, positionProfit, todayPosition );
		UFC::BufferedLog::Printf( " OnKSftRspQryInvestorPosition:preSettlementPx[%15.6lf] SettlementPx[%15.6lf]",
								  preSettlementPrice, settlementPrice );
		UFC::BufferedLog::Printf( " OnKSftRspQryInvestorPosition:[%s]", ansiResponseStr.c_str() );
		UFC::BufferedLog::FlushToFile();
	}

	if ( FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSFT" );
		newsMsg.SetID( CTP_OnInvestorPosition );
		newsMsg.SetText( ansiResponseStr.c_str() );
		FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( "TTaifexConnection::OnKSftRspQryInvestorPosition() %s", ansiResponseStr.c_str() );
*/
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspQryTradingAccount( KS_::CThostFtdcTradingAccountField *pTradingAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int /*nRequestID*/, bool bIsLast )
{
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		UFC::BufferedLog::Printf( " OnKSftRspQryTradingAccount failed code:%d", pRspInfo->ErrorID );
		return;
	}
	if ( pTradingAccount != NULL && bIsLast )
	{
		TNewsMessage Msg;
		UFC::NameValueMessage BalanceMsg("^\n");

		Msg.SetHeadline( "KSFT");
		Msg.SetID( CTP_OnTradingAccount );
		BalanceMsg.Append( "PreBalance",       pTradingAccount->PreBalance );
		BalanceMsg.Append( "Withdraw",         pTradingAccount->Withdraw );
		BalanceMsg.Append( "Deposit",          pTradingAccount->Deposit );
		BalanceMsg.Append( "CloseProfit",      pTradingAccount->CloseProfit );
		BalanceMsg.Append( "PositionProfit",   pTradingAccount->PositionProfit );
		BalanceMsg.Append( "Commission",       pTradingAccount->Commission );
		BalanceMsg.Append( "CurrMargin",       pTradingAccount->CurrMargin );
		BalanceMsg.Append( "FrozenMargin",     pTradingAccount->FrozenMargin );
		BalanceMsg.Append( "FrozenCommission", pTradingAccount->FrozenCommission );
		BalanceMsg.Append( "Available",        pTradingAccount->Available );

		BalanceMsg.Append( "PreCredit",        pTradingAccount->PreCredit );
		BalanceMsg.Append( "PreMortgage",      pTradingAccount->PreMortgage );
		BalanceMsg.Append( "Mortgage",         pTradingAccount->Mortgage );
		BalanceMsg.Append( "DeliveryMargin",   pTradingAccount->DeliveryMargin );
		BalanceMsg.Append( "Credit",           pTradingAccount->Credit );
		BalanceMsg.Append( "WithdrawQuota",    pTradingAccount->WithdrawQuota );
		// ??
		// 保底期貨結算準備金		ReserveBalance
		// 幣種代碼					CurrencyID
		// 上次貨幣質入金額			PreFundMortgageIn
		// 上次貨幣質出金額			PreFundMortgageOut
		// 貨幣質入金額				FundMortgageIn
		// 貨幣質出金額				FundMortgageOut
		// 貨幣質押余額				FundMortgageAvailable
		// 可質押貨幣金額			MortgageableFund
		// 特殊產品占用保證金		SpecProductMargin
		// 特殊產品凍結保證金		SpecProductFrozenMargin
		// 特殊產品手續費			SpecProductCommission
		// 特殊產品凍結手續費		SpecProductFrozenCommission
		// 特殊產品持倉盈虧			SpecProductPositionProfit
		// 特殊產品平倉盈虧			SpecProductCloseProfit
		// 根據持倉盈虧算法計算的特殊產品持倉盈虧	SpecProductPositionProfitByAlg
		// 特殊產品交易所保證金		SpecProductExchangeMargin
		// 期權平倉盈虧				OptionCloseProfit
		// 期權市值					OptionValue

		FBankAccount = pTradingAccount->AccountID;
		Msg.SetText( BalanceMsg.ToString().c_str() );
		if ( FListener != NULL )
			FListener->OnNews( &Msg );
		UFC::BufferedLog::Printf( "TTaifexConnection::OnKSftRspQryTradingAccount [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f]"
								  , pTradingAccount->PreBalance, pTradingAccount->Withdraw, pTradingAccount->Deposit, pTradingAccount->CloseProfit
								  , pTradingAccount->PositionProfit, pTradingAccount->Commission, pTradingAccount->CurrMargin, pTradingAccount->FrozenMargin
								  , pTradingAccount->FrozenCommission, pTradingAccount->Available, pTradingAccount->PreCredit, pTradingAccount->PreMortgage
								  , pTradingAccount->Mortgage, pTradingAccount->DeliveryMargin, pTradingAccount->Credit, pTradingAccount->WithdrawQuota );
		UFC::BufferedLog::Printf( "TTaifexConnection::OnKSftRspQryTradingAccount [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f]"
								  , pTradingAccount->ReserveBalance , pTradingAccount->CurrencyID , pTradingAccount->PreFundMortgageIn , pTradingAccount->PreFundMortgageOut
								  , pTradingAccount->FundMortgageIn , pTradingAccount->FundMortgageOut , pTradingAccount->FundMortgageAvailable , pTradingAccount->MortgageableFund
								  , pTradingAccount->SpecProductMargin , pTradingAccount->SpecProductFrozenMargin );
		UFC::BufferedLog::Printf( "TTaifexConnection::OnKSftRspQryTradingAccount [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f]"
								  , pTradingAccount->SpecProductCommission , pTradingAccount->SpecProductFrozenCommission , pTradingAccount->SpecProductPositionProfit , pTradingAccount->SpecProductCloseProfit
								  , pTradingAccount->SpecProductPositionProfitByAlg , pTradingAccount->SpecProductExchangeMargin , pTradingAccount->OptionCloseProfit , pTradingAccount->OptionValue );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspQryTradingCode( KS_::CThostFtdcTradingCodeField *pTradingCode, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	UFC::AnsiString ansiResponseStr = BuildKS_ResponseInformationStr( pRspInfo, "|" );
	AppendFieldToFieldsStr( ansiResponseStr, nRequestID, "|" );
	AppendFieldToFieldsStr( ansiResponseStr, bIsLast, "|" );
	if ( pTradingCode != NULL )
	{
		UFC::AnsiString ansiInvestorId = pTradingCode->InvestorID; //5.投資者代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiInvestorId, "|" );
		UFC::AnsiString ansiBrokerId = pTradingCode->BrokerID; //6.經紀公司代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerId, "|" );
		UFC::AnsiString ansiExchangeId = pTradingCode->ExchangeID; //7.交易所代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiExchangeId, "|" );
		UFC::AnsiString ansiClientId = pTradingCode->ClientID; //8.交易編碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiClientId, "|" );
		bool isActive; //9.是否活躍
		if ( pTradingCode->IsActive == 1 )
			isActive = true;
		else
			isActive = false;
		AppendFieldToFieldsStr( ansiResponseStr, isActive, "|" );
		char clientIDType = pTradingCode->ClientIDType; //10.交易編碼類型  '1':投機 '2':套利 '3':套保
		AppendFieldToFieldsStr( ansiResponseStr, clientIDType, "|" );
	}

	if ( FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSFT" );
		newsMsg.SetID( CTP_OnRspQueryTradingCode );
		newsMsg.SetText( ansiResponseStr.c_str() );
		FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( " TTaifexConnection::OnKSftRspQryTradingCode() %s", ansiResponseStr.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspQryInstrumentCommissionRate( KS_::CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	UFC::AnsiString ansiResponseStr = BuildKS_ResponseInformationStr( pRspInfo, "|" );
	AppendFieldToFieldsStr( ansiResponseStr, nRequestID, "|" );
	AppendFieldToFieldsStr( ansiResponseStr, bIsLast, "|" );
	if ( pInstrumentCommissionRate != NULL )
	{
		UFC::AnsiString ansiInstrumentId = pInstrumentCommissionRate->InstrumentID;  // 5.合約代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiInstrumentId, "|" );
		char investorRange = pInstrumentCommissionRate->InvestorRange;  // 6.投資者範圍  '1':所有 '2':投資者組 '3':單一投資者
		AppendFieldToFieldsStr( ansiResponseStr, investorRange, "|" );
		UFC::AnsiString ansiBrokerId = pInstrumentCommissionRate->BrokerID;  // 7.經紀公司代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerId, "|" );
		UFC::AnsiString ansiInvestorId = pInstrumentCommissionRate->InvestorID;  // 8.投資者代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiInvestorId, "|" );
		double openRatioByMoney = pInstrumentCommissionRate->OpenRatioByMoney;  // 9.開倉手續費率
		AppendFieldToFieldsStr( ansiResponseStr, openRatioByMoney, "|" );
		double openRatioByVolume = pInstrumentCommissionRate->OpenRatioByVolume;  //10.開倉手續費
		AppendFieldToFieldsStr( ansiResponseStr, openRatioByVolume, "|" );
		double closeRatioByMoney = pInstrumentCommissionRate->CloseRatioByMoney;  //11.平倉手續費率
		AppendFieldToFieldsStr( ansiResponseStr, closeRatioByMoney, "|" );
		double closeRatioByVolume = pInstrumentCommissionRate->CloseRatioByVolume;  //12.平倉手續費
		AppendFieldToFieldsStr( ansiResponseStr, closeRatioByVolume, "|" );
		double closeTodayRatioByMoney = pInstrumentCommissionRate->CloseTodayRatioByMoney;  //13.平今手續費率
		AppendFieldToFieldsStr( ansiResponseStr, closeTodayRatioByMoney, "|" );
		double closeTodayRatioByVolume = pInstrumentCommissionRate->CloseTodayRatioByVolume;  //14.平今手續費
		AppendFieldToFieldsStr( ansiResponseStr, closeTodayRatioByVolume, "|" );
	}

	if ( FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSFT" );
		newsMsg.SetID( CTP_OnRspQryInstrumentCommissionRate );
		newsMsg.SetText( ansiResponseStr.c_str() );
		FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( " TTaifexConnection::OnKSftRspQryInstrumentCommissionRate() %s", ansiResponseStr.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspQrySettlementInfo( KS_::CThostFtdcSettlementInfoField *pSettlementInfo, KS_::CThostFtdcRspInfoField *pRspInfo, int /*nRequestID*/, bool bIsLast )
{
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		TNewsMessage Msg;

		UTF8String UTF8Text( ErrMsg );
		Msg.SetHeadline( "KSFT" );
		Msg.SetID( CTP_OnSettlementInfo );
		Msg.SetText( UTF8Text.c_str() );
		if( FListener != NULL )
			FListener->OnNews( &Msg );
		return;
	}
	if ( ( pSettlementInfo != NULL ) && ( std::strlen(pSettlementInfo->Content) > 0 ) )
		FSettlementInfo.Add( pSettlementInfo->Content );

	if ( bIsLast )
	{
		GB2312String Text;
		TNewsMessage Msg;

		Msg.SetHeadline( "KSFT" );
		Msg.SetID( CTP_OnSettlementInfo );
		if ( FSettlementInfo.ItemCount() > 0 )
		{
			for ( int i =0; i < FSettlementInfo.ItemCount(); i ++ )
				Text += GB2312String( FSettlementInfo[i].c_str() );
			UTF8String UTF8Text( Text );
			Msg.SetText( UTF8Text.c_str() );
		}
		else
			Msg.SetText( "-----------------------------------------------------------------" );
		if ( FListener != NULL )
			FListener->OnNews( &Msg );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspQryTransferBank( KS_::CThostFtdcTransferBankField *pTransferBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	UFC::AnsiString ansiResponseStr = BuildKS_ResponseInformationStr( pRspInfo, "|" );
	AppendFieldToFieldsStr( ansiResponseStr, nRequestID, "|" );
	AppendFieldToFieldsStr( ansiResponseStr, bIsLast, "|" );
	if ( pTransferBank != NULL )
	{
		UFC::AnsiString ansiBankId = pTransferBank->BankID;  //5.銀行代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBankId, "|" );
		UFC::AnsiString ansiBankBranchId = pTransferBank->BankBrchID;  //6.銀行分中心代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBankBranchId, "|" );
		GB2312String gb2312BankName( pTransferBank->BankName );  //7.銀行名稱
		UTF8String utf8BankName( gb2312BankName );
		UFC::AnsiString ansiBankName( utf8BankName.c_str() );
		AppendFieldToFieldsStr( ansiResponseStr, ansiBankName, "|" );
		int isActive = pTransferBank->IsActive;  //8.是否活躍
		AppendFieldToFieldsStr( ansiResponseStr, isActive, "|" );
	}

	if ( FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSFT" );
		newsMsg.SetID( CTP_OnRspQueryTransferBank );
		newsMsg.SetText( ansiResponseStr.c_str() );
		FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( " TTaifexConnection::OnKSftRspQryTransferBank() %s", ansiResponseStr.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspQryInvestorPositionDetail( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int /*nRequestID*/, bool /*bIsLast*/ )
{
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		UTF8String UTF8Text( ErrMsg );
		UFC::BufferedLog::Printf( " OnKSftRspQryInvestorPositionDetail: %d[%s]", pRspInfo->ErrorID, UTF8Text.c_str() );
		UFC::BufferedLog::FlushToFile();
		return;
	}

	// Prevent 1st parameter null reference error
	if ( pInvestorPositionDetail == NULL )
	{
		UFC::BufferedLog::Printf( " OnKSftRspQryInvestorPositionDetail: pInvestorPositionDetail is NULL." );
		UFC::BufferedLog::FlushToFile();
		return;
	}
	UFC::BufferedLog::Printf( " OnKSftRspQryInvestorPositionDetail: %s %s %s Direction=%c OpenDate=%s TradeDate=%s Vol=%d OpenPx=%15.6lf CloseVol=%d TradeType=%c",
							  pInvestorPositionDetail->BrokerID,
							  pInvestorPositionDetail->InvestorID,
							  pInvestorPositionDetail->InstrumentID,
							  pInvestorPositionDetail->Direction,
							  pInvestorPositionDetail->OpenDate,
							  pInvestorPositionDetail->TradingDay,
							  pInvestorPositionDetail->Volume,
							  pInvestorPositionDetail->OpenPrice,
							  pInvestorPositionDetail->CloseVolume,
							  pInvestorPositionDetail->TradeType );
	UFC::BufferedLog::FlushToFile();
	if ( FListener != NULL )
	{
		UFC::AnsiString newsText;
		newsText.Printf( "%s|%s|%s|%c|%s|%d|%15.6lf|%d|%s",
						 pInvestorPositionDetail->BrokerID, pInvestorPositionDetail->InvestorID, pInvestorPositionDetail->InstrumentID,
						 pInvestorPositionDetail->Direction, pInvestorPositionDetail->OpenDate,
						 pInvestorPositionDetail->Volume, pInvestorPositionDetail->OpenPrice,
						 pInvestorPositionDetail->CloseVolume,
						 pInvestorPositionDetail->TradingDay );
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSFT" );
		newsMsg.SetID( CTP_OnInvestorPositionDetail );
		newsMsg.SetText( newsText.c_str() );
		FListener->OnNews( &newsMsg );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspQrySettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int /*nRequestID*/, bool /*bIsLast*/ )
{
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		UFC::BufferedLog::Printf( " OnKSftRspQrySettlementInfoConfirm failed code:%d", pRspInfo->ErrorID );
		UFC::BufferedLog::FlushToFile();
		return;
	}
	if ( pSettlementInfoConfirm != NULL ) ///< Already confirmed.
	{
		UFC::BufferedLog::Printf( " OnKSftRspQrySettlementInfoConfirm: User confirm settlement Info at:%s", pSettlementInfoConfirm->ConfirmTime );
		UFC::BufferedLog::FlushToFile();
		return;
	}
	else ///< Request Settlement Info
	{
		UFC::BufferedLog::Printf( " OnKSftRspQrySettlementInfoConfirm: User not confirm Settlement Info." );
		KS_::CThostFtdcQrySettlementInfoField QuerySettlementData;

		FSettlementInfo.Clear();
		memset( &QuerySettlementData, 0, sizeof(KS_::CThostFtdcQrySettlementInfoField) );
		strcpy( QuerySettlementData.BrokerID,   FCFFEXBrokerID.c_str() );
		strcpy( QuerySettlementData.InvestorID, FID.c_str() );
		while ( TRUE )
		{
			int Rtn = FKSFTAPI->ReqQrySettlementInfo( &QuerySettlementData, CTPReqID() );
			if( Rtn == -2 || Rtn == -3 )
				UFC::SleepMS(300);
			else
				break;
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspQryTransferSerial( KS_::CThostFtdcTransferSerialField *pTransferSerial, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	UFC::AnsiString ansiResponseStr = BuildKS_ResponseInformationStr( pRspInfo, "|" );
	AppendFieldToFieldsStr( ansiResponseStr, nRequestID, "|" );
	AppendFieldToFieldsStr( ansiResponseStr, bIsLast, "|" );
	if ( pTransferSerial != NULL )
	{
		int plateSerial = pTransferSerial->PlateSerial;  // 5.平台流水號
		AppendFieldToFieldsStr( ansiResponseStr, plateSerial, "|" );
		UFC::AnsiString ansiTradeDate = pTransferSerial->TradeDate;  // 6.交易發起方日期
		AppendFieldToFieldsStr( ansiResponseStr, ansiTradeDate, "|" );
		UFC::AnsiString ansiTradingDay =  pTransferSerial->TradingDay;  // 7.交易日期
		AppendFieldToFieldsStr( ansiResponseStr, ansiTradingDay, "|" );
		UFC::AnsiString ansiTradeTime = pTransferSerial->TradeTime;  // 8.交易時間
		AppendFieldToFieldsStr( ansiResponseStr, ansiTradeTime, "|" );
		UFC::AnsiString ansiTradeCode = pTransferSerial->TradeCode;  // 9.交易代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiTradeCode, "|" );
		int sessionID = pTransferSerial->SessionID;  //10.會話編號
		AppendFieldToFieldsStr( ansiResponseStr, sessionID, "|" );
		UFC::AnsiString ansiBankID = pTransferSerial->BankID;  //11.銀行編碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBankID, "|" );
		UFC::AnsiString ansiBankBranchID = pTransferSerial->BankBranchID;  //12.銀行分支機構編碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBankBranchID, "|" );
		char bankAccType = pTransferSerial->BankAccType;  //13.銀行帳號類型  '1':存摺 '2':儲蓄卡 '3':信用卡
		AppendFieldToFieldsStr( ansiResponseStr, bankAccType, "|" );
		UFC::AnsiString ansiBankAccount = pTransferSerial->BankAccount;  //14.銀行帳號
		AppendFieldToFieldsStr( ansiResponseStr, ansiBankAccount, "|" );
		UFC::AnsiString ansiBankSerial = pTransferSerial->BankSerial;  //15.銀行流水號
		AppendFieldToFieldsStr( ansiResponseStr, ansiBankSerial, "|" );
		UFC::AnsiString ansiBrokerId = pTransferSerial->BrokerID;  //16.期貨公司編碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerId, "|" );
		UFC::AnsiString ansiBrokerBranchId = pTransferSerial->BrokerBranchID;  //17.期貨公司分支機構代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerBranchId, "|" );
		char futureAccType = pTransferSerial->FutureAccType;  //18.期貨公司帳號類型 '1':存摺 '2':儲蓄卡 '3':信用卡
		AppendFieldToFieldsStr( ansiResponseStr, futureAccType, "|" );
		UFC::AnsiString ansiAccountId = pTransferSerial->AccountID;  //19.投資者帳號
		AppendFieldToFieldsStr( ansiResponseStr, ansiAccountId, "|" );
		UFC::AnsiString ansiInvestorId = pTransferSerial->InvestorID;  //20.投資者代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiInvestorId, "|" );
		int futureSerial = pTransferSerial->FutureSerial;  //21.期貨公司流水號
		AppendFieldToFieldsStr( ansiResponseStr, futureSerial, "|" );
		char idCardType = pTransferSerial->IdCardType;  //22.證件類型
		AppendFieldToFieldsStr( ansiResponseStr, idCardType, "|" );
		UFC::AnsiString ansiIdentifiedCardNo = pTransferSerial->IdentifiedCardNo;  //23.證件號碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiIdentifiedCardNo, "|" );
		UFC::AnsiString ansiCurrencyId = pTransferSerial->CurrencyID;  //24.幣種代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiCurrencyId, "|" );
		double tradeAmount = pTransferSerial->TradeAmount;  //25.交易金額
		AppendFieldToFieldsStr( ansiResponseStr, tradeAmount, "|" );
		double customFee = pTransferSerial->CustFee;  //26.應收客戶費用
		AppendFieldToFieldsStr( ansiResponseStr, customFee, "|" );
		double brokerFee = pTransferSerial->BrokerFee;  //27.應收期貨公司費用
		AppendFieldToFieldsStr( ansiResponseStr, brokerFee, "|" );
		char availabilityFlag = pTransferSerial->AvailabilityFlag;  //28.有效標誌 '0':未確認 '1':有效 '2':沖正
		AppendFieldToFieldsStr( ansiResponseStr, availabilityFlag, "|" );
		UFC::AnsiString ansiOperatorCode = pTransferSerial->OperatorCode;  //29.操作員
		AppendFieldToFieldsStr( ansiResponseStr, ansiOperatorCode, "|" );
		UFC::AnsiString ansiBankNewAccount = pTransferSerial->BankNewAccount;  //30.新銀行帳號
		AppendFieldToFieldsStr( ansiResponseStr, ansiBankNewAccount, "|" );
		int errorID = pTransferSerial->ErrorID;  //31.錯誤代碼
		AppendFieldToFieldsStr( ansiResponseStr, errorID, "|" );
		GB2312String gb2312ErrorMsg( pTransferSerial->ErrorMsg );  //32.錯誤訊息
		UTF8String utf8ErrorMsg( gb2312ErrorMsg );
		UFC::AnsiString ansiErrorMsg( utf8ErrorMsg.c_str() );
		AppendFieldToFieldsStr( ansiResponseStr, ansiErrorMsg, "|" );
	}

	if ( FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSFT" );
		newsMsg.SetID( CTP_OnRspQryTransferSerial );
		newsMsg.SetText( ansiResponseStr.c_str() );
		FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( " TTaifexConnection::OnKSftRspQryTransferSerial() %s", ansiResponseStr.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspQryAccountregister( KS_::CThostFtdcAccountregisterField *pAccountregister, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	UFC::AnsiString ansiResponseStr = BuildKS_ResponseInformationStr( pRspInfo, "|" );
	AppendFieldToFieldsStr( ansiResponseStr, nRequestID, "|" );
	AppendFieldToFieldsStr( ansiResponseStr, bIsLast, "|" );
	if ( pAccountregister != NULL )
	{
		UFC::AnsiString ansiTradeDate = pAccountregister->TradeDay;  // 5.交易日期
		AppendFieldToFieldsStr( ansiResponseStr, ansiTradeDate, "|" );
		UFC::AnsiString ansiBankId = pAccountregister->BankID;  // 6.銀行代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBankId, "|" );
		UFC::AnsiString ansiBankBranchId = pAccountregister->BankBranchID;  // 7.銀行分支機構代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBankBranchId, "|" );
		UFC::AnsiString ansiBankAccount = pAccountregister->BankAccount;  // 8.銀行帳號
		AppendFieldToFieldsStr( ansiResponseStr, ansiBankAccount, "|" );
		UFC::AnsiString ansiBrokerId = pAccountregister->BrokerID;  // 9.期貨公司編碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerId, "|" );
		UFC::AnsiString ansiBrokerBranchId = pAccountregister->BrokerBranchID;  //10.期貨公司分支機構編碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerBranchId, "|" );
		UFC::AnsiString ansiAccountId = pAccountregister->AccountID;  //11.投資者帳號
		AppendFieldToFieldsStr( ansiResponseStr, ansiAccountId, "|" );
		char idCardType = pAccountregister->IdCardType;  //12.證件類型
		AppendFieldToFieldsStr( ansiResponseStr, idCardType, "|" );
		UFC::AnsiString ansiIdentifiedCardNo = pAccountregister->IdentifiedCardNo;  //13.證件號碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiIdentifiedCardNo, "|" );
		GB2312String gb2312CustomerName( pAccountregister->CustomerName );  //14.客戶姓名
		UTF8String utf8CustomerName( gb2312CustomerName );
		UFC::AnsiString ansiCustomerName( utf8CustomerName.c_str() );
		AppendFieldToFieldsStr( ansiResponseStr, ansiCustomerName, "|" );
		UFC::AnsiString ansiCurrencyId = pAccountregister->CurrencyID;  //15.幣種
		AppendFieldToFieldsStr( ansiResponseStr, ansiCurrencyId, "|" );
		char openOrDestroy = pAccountregister->OpenOrDestroy;  //16.開銷戶類別 '1':開戶 '0':銷戶
		AppendFieldToFieldsStr( ansiResponseStr, openOrDestroy, "|" );
		UFC::AnsiString ansiRegisterDate = pAccountregister->RegDate;  //17.簽約日期
		AppendFieldToFieldsStr( ansiResponseStr, ansiRegisterDate, "|" );
		UFC::AnsiString ansiCancelDate = pAccountregister->OutDate;  //18.解約日期
		AppendFieldToFieldsStr( ansiResponseStr, ansiCancelDate, "|" );
		int tradeId = pAccountregister->TID;  //19.交易ID
		AppendFieldToFieldsStr( ansiResponseStr, tradeId, "|" );
		char customType = pAccountregister->CustType;  //20.客戶類型 '0':自然人 '1':機構戶
		AppendFieldToFieldsStr( ansiResponseStr, customType, "|" );
		char bankAccountType = pAccountregister->BankAccType;  //21.銀行帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
		AppendFieldToFieldsStr( ansiResponseStr, bankAccountType, "|" );
	}

	if ( FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSFT" );
		newsMsg.SetID( CTP_OnRspQueryAccountRegister );
		newsMsg.SetText( ansiResponseStr.c_str() );
		FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( " TTaifexConnection::OnKSftRspQryAccountregister() %s", ansiResponseStr.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRtnInstrumentStatus( KS_::CThostFtdcInstrumentStatusField *pInstrumentStatus )
{
	TNewsMessage Msg;
	UFC::NameValueMessage StatusMsg("^\n");

	if ( pInstrumentStatus != NULL )
	{
		Msg.SetHeadline( "KSFT" );
		Msg.SetID( CTP_OnExchangeStatus );
		StatusMsg.Append( "Exchange",  pInstrumentStatus->ExchangeID );
		StatusMsg.Append( "Status",  UFC::AnsiString(pInstrumentStatus->InstrumentStatus) );
		Msg.SetText( StatusMsg.ToString().c_str() );
		if ( FListener != NULL )
			FListener->OnNews( &Msg );
		UFC::BufferedLog::Printf( " OnKSftRtnInstrumentStatus Exchange[%s] Time[%s] Status[%c] "
								  , pInstrumentStatus->ExchangeID, pInstrumentStatus->EnterTime, pInstrumentStatus->InstrumentStatus );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspQryContractBank( KS_::CThostFtdcContractBankField *pContractBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	UFC::AnsiString ansiResponseStr = BuildKS_ResponseInformationStr( pRspInfo, "|" );
	AppendFieldToFieldsStr( ansiResponseStr, nRequestID, "|" );
	AppendFieldToFieldsStr( ansiResponseStr, bIsLast, "|" );
	if ( pContractBank != NULL )
	{
		UFC::AnsiString ansiBrokerId = pContractBank->BrokerID;  //5.經紀公司代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerId, "|" );
		UFC::AnsiString ansiBankId = pContractBank->BankID;  //6.銀行代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBankId, "|" );
		UFC::AnsiString ansiBankBranchId = pContractBank->BankBrchID;  //7.銀行分中心代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBankBranchId, "|" );
		GB2312String gb2312BankName( pContractBank->BankName );  //8.銀行名稱
		UTF8String utf8BankName( gb2312BankName );
		UFC::AnsiString ansiBankName( utf8BankName.c_str() );
		AppendFieldToFieldsStr( ansiResponseStr, ansiBankName, "|" );
	}

	if ( FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSFT" );
		newsMsg.SetID( CTP_OnRspQueryContractBank );
		newsMsg.SetText( ansiResponseStr.c_str() );
		FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( " TTaifexConnection::OnKSftRspQryContractBank() %s", ansiResponseStr.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRtnFromBankToFutureByFuture( KS_::CThostFtdcRspTransferField *pRspTransfer )
{
	// Prevent 1st parameter null reference error
	if ( pRspTransfer == NULL )
	{
		UFC::BufferedLog::Printf( " OnKSftRtnFromBankToFutureByFuture: pRspTransfer is NULL." );
		UFC::BufferedLog::FlushToFile();
		return;
	}

	if ( FListener != NULL )
	{
		UFC::AnsiString newsText = BuildKS_TransferResponseStr( pRspTransfer );
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSFT" );
		newsMsg.SetID( CTP_OnRtnFromBankToFutureByFuture );
		newsMsg.SetText( newsText.c_str() );
		FListener->OnNews( &newsMsg );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRtnFromFutureToBankByFuture( KS_::CThostFtdcRspTransferField *pRspTransfer )
{
	// Prevent 1st parameter null reference error
	if ( pRspTransfer == NULL )
	{
		UFC::BufferedLog::Printf(" OnKSftRtnFromFutureToBankByFuture: pRspTransfer is NULL.");
		UFC::BufferedLog::FlushToFile();
		return;
	}

	if ( FListener != NULL )
	{
		UFC::AnsiString newsText = BuildKS_TransferResponseStr( pRspTransfer );
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSFT" );
		newsMsg.SetID( CTP_OnRtnFromFutureToBankByFuture );
		newsMsg.SetText( newsText.c_str() );
		FListener->OnNews( &newsMsg );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRtnQueryBankBalanceByFuture( KS_::CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount )
{
	UFC::AnsiString ansiResultStr = "";
	if ( pNotifyQueryAccount != NULL )
	{
		int errorId = pNotifyQueryAccount->ErrorID;  //1.錯誤代碼
		AppendFieldToFieldsStr( ansiResultStr, errorId, "|" );
		GB2312String gb2312ErrorMsg( pNotifyQueryAccount->ErrorMsg );  //2.錯誤訊息
		UTF8String utf8ErrorMsg( gb2312ErrorMsg );
		UFC::AnsiString ansiErrorMsg( utf8ErrorMsg.c_str() );
		AppendFieldToFieldsStr( ansiResultStr, ansiErrorMsg, "|" );
		UFC::AnsiString ansiTradeCode = pNotifyQueryAccount->TradeCode;  // 3.業務功能碼
		AppendFieldToFieldsStr( ansiResultStr, ansiTradeCode, "|" );
		UFC::AnsiString ansiBankId = pNotifyQueryAccount->BankID;  // 4.銀行代碼
		AppendFieldToFieldsStr( ansiResultStr, ansiBankId, "|" );
		UFC::AnsiString ansiBankBranchId = pNotifyQueryAccount->BankBranchID;  // 5.銀行分支機構代碼
		AppendFieldToFieldsStr( ansiResultStr, ansiBankBranchId, "|" );
		UFC::AnsiString ansiBrokerId = pNotifyQueryAccount->BrokerID;  // 6.期商代碼
		AppendFieldToFieldsStr( ansiResultStr, ansiBrokerId, "|" );
		UFC::AnsiString ansiBrokerBranchId = pNotifyQueryAccount->BrokerBranchID;  // 7.期商分支機構代碼
		AppendFieldToFieldsStr( ansiResultStr, ansiBrokerBranchId, "|" );
		UFC::AnsiString ansiTradeDate =  pNotifyQueryAccount->TradeDate;  // 8.交易日期
		AppendFieldToFieldsStr( ansiResultStr, ansiTradeDate, "|" );
		UFC::AnsiString ansiTradeTime = pNotifyQueryAccount->TradeTime;  // 9.交易時間
		AppendFieldToFieldsStr( ansiResultStr, ansiTradeTime, "|" );
		UFC::AnsiString ansiBankSerial = pNotifyQueryAccount->BankSerial;  //10.銀行流水號
		AppendFieldToFieldsStr( ansiResultStr, ansiBankSerial, "|" );
		UFC::AnsiString ansiSystemTradeDate = pNotifyQueryAccount->TradingDay;  //11.系統交易日期
		AppendFieldToFieldsStr( ansiResultStr, ansiSystemTradeDate, "|" );
		int plateSerial = pNotifyQueryAccount->PlateSerial;  //12.銀期平台消息流水號
		AppendFieldToFieldsStr( ansiResultStr, plateSerial, "|" );
		char lastFragment = pNotifyQueryAccount->LastFragment;  //13.最後分片標誌
		AppendFieldToFieldsStr( ansiResultStr, lastFragment, "|" );
		int sessionId = pNotifyQueryAccount->SessionID;  //14.會話號
		AppendFieldToFieldsStr( ansiResultStr, sessionId, "|" );
		GB2312String gb2312CustomerName( pNotifyQueryAccount->CustomerName );  //15.客戶姓名
		UTF8String utf8CustomerName( gb2312CustomerName );
		UFC::AnsiString ansiCustomerName( utf8CustomerName.c_str() );
		AppendFieldToFieldsStr( ansiResultStr, ansiCustomerName, "|" );
		char idCardType = pNotifyQueryAccount->IdCardType;  //16.證件類型
		AppendFieldToFieldsStr( ansiResultStr, idCardType, "|" );
		UFC::AnsiString ansiIdentifiedCardNo = pNotifyQueryAccount->IdentifiedCardNo;  //17.證件號碼
		AppendFieldToFieldsStr( ansiResultStr, ansiIdentifiedCardNo, "|" );
		char customType = pNotifyQueryAccount->CustType;  //18.客戶類型
		AppendFieldToFieldsStr( ansiResultStr, customType, "|" );
		UFC::AnsiString ansiBankAccount = pNotifyQueryAccount->BankAccount;  //19.銀行帳號
		AppendFieldToFieldsStr( ansiResultStr, ansiBankAccount, "|" );
		UFC::AnsiString ansiBankPassword = pNotifyQueryAccount->BankPassWord;  //20.銀行密碼
		AppendFieldToFieldsStr( ansiResultStr, ansiBankPassword, "|" );
		UFC::AnsiString ansiAccountId = pNotifyQueryAccount->AccountID;  //21.投資者帳號
		AppendFieldToFieldsStr( ansiResultStr, ansiAccountId, "|" );
		UFC::AnsiString ansiAccountPassword = pNotifyQueryAccount->Password;  //22.期貨密碼
		AppendFieldToFieldsStr( ansiResultStr, ansiAccountPassword, "|" );
		int futureSerial = pNotifyQueryAccount->FutureSerial;  //23.期貨公司流水號
		AppendFieldToFieldsStr( ansiResultStr, futureSerial, "|" );
		int installID = pNotifyQueryAccount->InstallID;  //24.安裝編號
		AppendFieldToFieldsStr( ansiResultStr, installID, "|" );
		UFC::AnsiString ansiUserID = pNotifyQueryAccount->UserID;  //25.用戶標誌
		AppendFieldToFieldsStr( ansiResultStr, ansiUserID, "|" );
		char verifyCertNoFlag = pNotifyQueryAccount->VerifyCertNoFlag;  //26.驗證客戶證件號碼標誌 '0':是 '1':否
		AppendFieldToFieldsStr( ansiResultStr, verifyCertNoFlag, "|" );
		UFC::AnsiString ansiCurrencyId = pNotifyQueryAccount->CurrencyID;  //27.幣種
		AppendFieldToFieldsStr( ansiResultStr, ansiCurrencyId, "|" );
		UFC::AnsiString ansiDigest = pNotifyQueryAccount->Digest;  //28.摘要
		AppendFieldToFieldsStr( ansiResultStr, ansiDigest, "|" );
		char bankAccountType = pNotifyQueryAccount->BankAccType;  //29.銀行帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
		AppendFieldToFieldsStr( ansiResultStr, bankAccountType, "|" );
		UFC::AnsiString ansiDeviceId = pNotifyQueryAccount->DeviceID;  //30.渠道標誌
		AppendFieldToFieldsStr( ansiResultStr, ansiDeviceId, "|" );
		char bankSecurityAccountType = pNotifyQueryAccount->BankSecuAccType;  //31.期貨單位帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
		AppendFieldToFieldsStr( ansiResultStr, bankSecurityAccountType, "|" );
		UFC::AnsiString ansiBrokerIDByBank = pNotifyQueryAccount->BrokerIDByBank;  //32.期貨公司銀行編碼
		AppendFieldToFieldsStr( ansiResultStr, ansiBrokerIDByBank, "|" );
		UFC::AnsiString ansiBankSecurityAccount = pNotifyQueryAccount->BankSecuAcc;  //33.期貨單位帳號
		AppendFieldToFieldsStr( ansiResultStr, ansiBankSecurityAccount, "|" );
		char bankPasswordFlag = pNotifyQueryAccount->BankPwdFlag;  //34.銀行密碼標誌 '0':不核對  '1':明文核對  '2':密文核對
		AppendFieldToFieldsStr( ansiResultStr, bankPasswordFlag, "|" );
		char securityPasswordFlag = pNotifyQueryAccount->SecuPwdFlag;  //35.期貨資金密碼核對標誌 '0':不核對  '1':明文核對  '2':密文核對
		AppendFieldToFieldsStr( ansiResultStr, securityPasswordFlag, "|" );
		UFC::AnsiString ansiOperatorNo = pNotifyQueryAccount->OperNo;  //36.交易櫃員
		AppendFieldToFieldsStr( ansiResultStr, ansiOperatorNo, "|" );
		int requestId = pNotifyQueryAccount->RequestID;  //37.請求編號
		AppendFieldToFieldsStr( ansiResultStr, requestId, "|" );
		int tradeId = pNotifyQueryAccount->TID;  //38.交易ID
		AppendFieldToFieldsStr( ansiResultStr, tradeId, "|" );
		double bankUseAmount = pNotifyQueryAccount->BankUseAmount;  //39.銀行可用金額
		AppendFieldToFieldsStr( ansiResultStr, bankUseAmount, "|" );
		double bankFetchAmount = pNotifyQueryAccount->BankFetchAmount;  //40.銀行可取金額
		AppendFieldToFieldsStr( ansiResultStr, bankFetchAmount, "|" );
	}

	if ( FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSFT" );
		newsMsg.SetID( CTP_OnRtnQueryBankBalanceByFuture );
		newsMsg.SetText( ansiResultStr.c_str() );
		FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( " TTaifexConnection::OnKSftRtnQueryBankBalanceByFuture() %s", ansiResultStr.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspFromBankToFutureByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	UFC::AnsiString ansiNewsText = BuildKS_TransferRequestStr( pRspInfo, nRequestID, bIsLast, pReqTransfer );
	if ( FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSFT" );
		newsMsg.SetID( CTP_OnRspFromBankToFutureByFuture );
		newsMsg.SetText( ansiNewsText.c_str() );
		FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( " TTaifexConnection::OnKSftRspFromBankToFutureByFuture() %s", ansiNewsText.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspFromFutureToBankByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	UFC::AnsiString ansiNewsText = BuildKS_TransferRequestStr( pRspInfo, nRequestID, bIsLast, pReqTransfer );
	if ( FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSFT" );
		newsMsg.SetID( CTP_OnRspFromFutureToBankByFuture );
		newsMsg.SetText( ansiNewsText.c_str() );
		FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( " TTaifexConnection::OnKSftRspFromFutureToBankByFuture() %s", ansiNewsText.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspQueryBankAccountMoneyByFuture( KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	UFC::AnsiString ansiResponseStr = BuildKS_ResponseInformationStr( pRspInfo, "|" );
	AppendFieldToFieldsStr( ansiResponseStr, nRequestID, "|" );
	AppendFieldToFieldsStr( ansiResponseStr, bIsLast, "|" );
	if ( pReqQueryAccount != NULL )
	{
		UFC::AnsiString ansiTradeCode = pReqQueryAccount->TradeCode;  // 5.業務功能碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiTradeCode, "|" );
		UFC::AnsiString ansiBankId = pReqQueryAccount->BankID;  // 6.銀行代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBankId, "|" );
		UFC::AnsiString ansiBankBranchId = pReqQueryAccount->BankBranchID;  // 7.銀行分支機構代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBankBranchId, "|" );
		UFC::AnsiString ansiBrokerId = pReqQueryAccount->BrokerID;  // 8.期商代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerId, "|" );
		UFC::AnsiString ansiBrokerBranchId = pReqQueryAccount->BrokerBranchID;  // 9.期商分支機構代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerBranchId, "|" );
		UFC::AnsiString ansiTradeDate = pReqQueryAccount->TradeDate;  //10.交易日期
		AppendFieldToFieldsStr( ansiResponseStr, ansiTradeDate, "|" );
		UFC::AnsiString ansiTradeTime = pReqQueryAccount->TradeTime;  //11.交易時間
		AppendFieldToFieldsStr( ansiResponseStr, ansiTradeTime, "|" );
		UFC::AnsiString ansiBankSerial = pReqQueryAccount->BankSerial;  //12.銀行流水號
		AppendFieldToFieldsStr( ansiResponseStr, ansiBankSerial, "|" );
		UFC::AnsiString ansiSystemTradeDate = pReqQueryAccount->TradingDay;  //13.系統交易日期
		AppendFieldToFieldsStr( ansiResponseStr, ansiSystemTradeDate, "|" );
		int plateSerial = pReqQueryAccount->PlateSerial;  //14.銀期平台消息流水號
		AppendFieldToFieldsStr( ansiResponseStr, plateSerial, "|" );
		char lastFragment = pReqQueryAccount->LastFragment;  //15.最後分片標誌
		AppendFieldToFieldsStr( ansiResponseStr, lastFragment, "|" );
		int sessionId = pReqQueryAccount->SessionID;  //16.會話號
		AppendFieldToFieldsStr( ansiResponseStr, sessionId, "|" );
		GB2312String gb2312CustomerName( pReqQueryAccount->CustomerName );  //17.客戶姓名
		UTF8String utf8CustomerName( gb2312CustomerName );
		UFC::AnsiString ansiCustomerName( utf8CustomerName.c_str() );
		AppendFieldToFieldsStr( ansiResponseStr, ansiCustomerName, "|" );
		char idCardType = pReqQueryAccount->IdCardType;  //18.證件類型
		AppendFieldToFieldsStr( ansiResponseStr, idCardType, "|" );
		UFC::AnsiString ansiIdentifiedCardNo = pReqQueryAccount->IdentifiedCardNo;  //19.證件號碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiIdentifiedCardNo, "|" );
		char customType = pReqQueryAccount->CustType;  //20.客戶類型
		AppendFieldToFieldsStr( ansiResponseStr, customType, "|" );
		UFC::AnsiString ansiBankAccount = pReqQueryAccount->BankAccount;  //21.銀行帳號
		AppendFieldToFieldsStr( ansiResponseStr, ansiBankAccount, "|" );
		UFC::AnsiString ansiBankPassword = pReqQueryAccount->BankPassWord;  //22.銀行密碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBankPassword, "|" );
		UFC::AnsiString ansiAccountId = pReqQueryAccount->AccountID;  //23.投資者帳號
		AppendFieldToFieldsStr( ansiResponseStr, ansiAccountId, "|" );
		UFC::AnsiString ansiAccountPassword = pReqQueryAccount->Password;  //24.期貨密碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiAccountPassword, "|" );
		int installID = pReqQueryAccount->InstallID;  //25.安裝編號
		AppendFieldToFieldsStr( ansiResponseStr, installID, "|" );
		UFC::AnsiString ansiUserID = pReqQueryAccount->UserID;  //26.用戶標誌
		AppendFieldToFieldsStr( ansiResponseStr, ansiUserID, "|" );
		char verifyCertNoFlag = pReqQueryAccount->VerifyCertNoFlag;  //27.驗證客戶證件號碼標誌 '0':是 '1':否
		AppendFieldToFieldsStr( ansiResponseStr, verifyCertNoFlag, "|" );
		UFC::AnsiString ansiCurrencyId = pReqQueryAccount->CurrencyID;  //28.幣種
		AppendFieldToFieldsStr( ansiResponseStr, ansiCurrencyId, "|" );
		UFC::AnsiString ansiDigest = pReqQueryAccount->Digest;  //29.摘要
		AppendFieldToFieldsStr( ansiResponseStr, ansiDigest, "|" );
		char bankAccountType = pReqQueryAccount->BankAccType;  //30.銀行帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
		AppendFieldToFieldsStr( ansiResponseStr, bankAccountType, "|" );
		UFC::AnsiString ansiDeviceId = pReqQueryAccount->DeviceID;  //31.渠道標誌
		AppendFieldToFieldsStr( ansiResponseStr, ansiDeviceId, "|" );
		char bankSecurityAccountType = pReqQueryAccount->BankSecuAccType;  //32.期貨單位帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
		AppendFieldToFieldsStr( ansiResponseStr, bankSecurityAccountType, "|" );
		UFC::AnsiString ansiBrokerIDByBank = pReqQueryAccount->BrokerIDByBank;  //33.期貨公司銀行編碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerIDByBank, "|" );
		UFC::AnsiString ansiBankSecurityAccount = pReqQueryAccount->BankSecuAcc;  //34.期貨單位帳號
		AppendFieldToFieldsStr( ansiResponseStr, ansiBankSecurityAccount, "|" );
		char bankPasswordFlag = pReqQueryAccount->BankPwdFlag;  //35.銀行密碼標誌 '0':不核對  '1':明文核對  '2':密文核對
		AppendFieldToFieldsStr( ansiResponseStr, bankPasswordFlag, "|" );
		char securityPasswordFlag = pReqQueryAccount->SecuPwdFlag;  //36.期貨資金密碼核對標誌 '0':不核對  '1':明文核對  '2':密文核對
		AppendFieldToFieldsStr( ansiResponseStr, securityPasswordFlag, "|" );
		UFC::AnsiString ansiOperatorNo = pReqQueryAccount->OperNo;  //37.交易櫃員
		AppendFieldToFieldsStr( ansiResponseStr, ansiOperatorNo, "|" );
		int requestId = pReqQueryAccount->RequestID;  //38.請求編號
		AppendFieldToFieldsStr( ansiResponseStr, requestId, "|" );
		int tradeId = pReqQueryAccount->TID;  //39.交易ID
		AppendFieldToFieldsStr( ansiResponseStr, tradeId, "|" );
	}

	if ( FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSFT" );
		newsMsg.SetID( CTP_OnRspQueryBankAccountMoneyByFuture );
		newsMsg.SetText( ansiResponseStr.c_str() );
		FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( "TTaifexConnection::OnKSftRspQueryBankAccountMoneyByFuture() %s", ansiResponseStr.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
// [KSft] KingStar special Investor Position for Open
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspQryInvestorOpenPosition( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		UFC::BufferedLog::Printf( " OnKSftRspQryInvestorOpenPosition failed code:%d", pRspInfo->ErrorID );
		UFC::BufferedLog::FlushToFile();
		return;
	}

	UFC::AnsiString ansiResponseStr = BuildKS_ResponseInformationStr( pRspInfo, "|" );
	AppendFieldToFieldsStr( ansiResponseStr, nRequestID, "|" );
	AppendFieldToFieldsStr( ansiResponseStr, bIsLast, "|" );
	if ( pInvestorPositionDetail != NULL )
	{
		UFC::AnsiString ansiInstrumentId = pInvestorPositionDetail->InstrumentID;  // 5.合約代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiInstrumentId, "|" );
		UFC::AnsiString ansiBrokerId = pInvestorPositionDetail->BrokerID;  // 6.經紀公司代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerId, "|" );
		UFC::AnsiString ansiInvestorId = pInvestorPositionDetail->InvestorID;  // 7.投資者代碼
		AppendFieldToFieldsStr( ansiResponseStr, ansiInvestorId, "|" );
		char Direction = pInvestorPositionDetail->Direction;  // 8.買賣 '0':買  '1':賣
		AppendFieldToFieldsStr( ansiResponseStr, Direction, "|" );
		UFC::AnsiString ansiOpenDate = pInvestorPositionDetail->OpenDate;  // 9.開倉日期
		AppendFieldToFieldsStr( ansiResponseStr, ansiOpenDate, "|" );
		int Volume = pInvestorPositionDetail->Volume;  // 10.數量
		AppendFieldToFieldsStr( ansiResponseStr, Volume, "|" );
		double openPrice = pInvestorPositionDetail->OpenPrice;  // 11.開倉價
		AppendFieldToFieldsStr( ansiResponseStr, openPrice, "|" );
		UFC::BufferedLog::Printf( " OnKSftRspQryInvestorOpenPosition:%s %s %s Dir=%c OpenPrice=%15.6f Volume=%d OpenDate=%s IsLast=%s",
								  ansiBrokerId.c_str(), ansiInvestorId.c_str(), ansiInstrumentId.c_str(), Direction, openPrice, Volume, ansiOpenDate.c_str(), (bIsLast ? "true" : "false") );
		UFC::BufferedLog::FlushToFile();

		if ( FListener != NULL )
		{
			TNewsMessage newsMsg;
			newsMsg.SetHeadline( "KSFT" );
			newsMsg.SetID( CTP_OnInvestorOpenPosition );
			newsMsg.SetText( ansiResponseStr.c_str() );
			FListener->OnNews( &newsMsg );
		}
		else
			UFC::BufferedLog::Printf( "TTaifexConnection::OnKSftRspQryInvestorOpenPosition() FListener is NULL [%s]", ansiResponseStr.c_str() );
	}
}
//------------------------------------------------------------------------------------------------------------------------
// [KSft] KingStar Reject execution
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int /*nRequestID*/, bool /*bIsLast*/ )
{
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 && pInputOrder != NULL )
	{
		GB2312String     ErrMsg( pRspInfo->ErrorMsg );
		UTF8String       ErrMsgUTF8 = ErrMsg;
		UFC::AnsiString  PEStr;
		TNewOrderMessage New;

		UpdateRecover();
		UFC::BufferedLog::Printf( " ------------------------ KSft Reject ----------------------------" );
		UFC::BufferedLog::Printf( " OnKSftRspOrderInsert: OrderRef[%s] ",  pInputOrder->OrderRef );
		New.SetAccount( pInputOrder->UserID );		///< Account
		New.SetBrokerID( pInputOrder->BrokerID );	///< BrokerID
		New.SetSymbol( pInputOrder->InstrumentID );	///< Symbol
		New.SetMarket( nsOrderMessageDefine::mCNFutures ); ///< Market // ??
		New.SetNID( COrderInfo::ToNID( FCTPFrontID, FCTPSessionID, atoi(pInputOrder->OrderRef) ) );
		// PositionEffect
		if ( pInputOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Open ) ///< Open
		{
			New.SetPositionEffect( nsOrderMessageDefine::peOpen );
			PEStr = "Open";
		}
		else if ( pInputOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Close ) ///< Close
		{
			New.SetPositionEffect( nsOrderMessageDefine::peClose );
			PEStr = "Close";
		}
		else if ( pInputOrder->CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday ) ///< Close Today
		{
			New.SetPositionEffect( nsOrderMessageDefine::peRolled );
			PEStr = "Close Today";
		}
		// Side
		if ( pInputOrder->Direction == THOST_FTDC_D_Buy )
			New.SetSide( nsOrderMessageDefine::sBuy );
		else
			New.SetSide( nsOrderMessageDefine::sSell );
		// Price
		if ( pInputOrder->OrderPriceType == THOST_FTDC_OPT_AnyPrice )
		{
			New.SetOrderType( nsOrderMessageDefine::otMarket );
			New.SetPrice( 0.0 );
		}
		else
		{
			New.SetOrderType( nsOrderMessageDefine::otLimit );
			New.SetPrice( pInputOrder->LimitPrice );
		}
		// TimeInForce
		if ( pInputOrder->TimeCondition == THOST_FTDC_TC_IOC )
			New.SetTimeInForce( nsOrderMessageDefine::tifIOC );
		else
			New.SetTimeInForce( nsOrderMessageDefine::tifROD );
		New.SetOrderQty( pInputOrder->VolumeTotalOriginal );
		UFC::BufferedLog::Printf( " NID[%s] Symbol[%s] PositionEffect[%s]", pInputOrder->OrderRef, pInputOrder->InstrumentID, PEStr.c_str() );
		Reject( nsOrderMessageDefine::crrNew, ErrMsgUTF8.c_str(), &New );
		//TradeLogKSftConfirm( pInputOrder, pRspInfo->ErrorID );
		UFC::BufferedLog::Printf( " ------------------- Sent Reject Execution -----------------------" );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRspError( KS_::CThostFtdcRspInfoField *pRspInfo, int /*nRequestID*/, bool /*bIsLast*/ )
{
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		UTF8String   ErrMsgUTF8 = ErrMsg;
		TNewsMessage newsMsg;

		newsMsg.SetHeadline( "KSFT" );
		newsMsg.SetID( CTP_OnRspError );
		newsMsg.SetText( ErrMsgUTF8.c_str() );
		if ( FListener != NULL )
			FListener->OnNews( &newsMsg );
		UFC::BufferedLog::Printf( " OnKSftRspError: Code[%d] Msg[%s]", pRspInfo->ErrorID, ErrMsgUTF8.c_str() );
	}
}
//------------------------------------------------------------------------------------------------------------------------
// [KSft] Exchange Order confirm
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRtnOrder( KS_::CThostFtdcOrderField *pOrder )
{
	if ( pOrder == NULL )
		return;

	///< KSft Ack: ( FrontID + SessionID ) OrderRef to find Order record.
	///<           Update OrderLocalID, ExchangeID
	///< Exchange Ack: ExchangeID + OrderLocalID  to find Order record.
	///<           Update OrderSysID
	UFC::AnsiString OrdRef( pOrder->OrderRef );
	UFC::AnsiString ExchangeID( pOrder->ExchangeID );
	UFC::AnsiString OrderLocalID( pOrder->OrderLocalID );
	UFC::AnsiString OrdSysID( pOrder->OrderSysID );
	UFC::AnsiString Status,SubmitStatus;
	COrderInfo*     Record;

	///< Print order info.
	OrderStatusString( pOrder->OrderStatus, Status );
	OrderSubmitStatusString( pOrder->OrderSubmitStatus, SubmitStatus );
	UFC::BufferedLog::Printf( " OnKSftRtnOrder: FrontID[%d] SessionID[%d] OrderRef[%s].", pOrder->FrontID, pOrder->SessionID, pOrder->OrderRef );
	UFC::BufferedLog::Printf( " OnKSftRtnOrder: Exchange[%s] OrderSysID[%s] OrderLocalID[%s].", ExchangeID.c_str(), OrdSysID.c_str(), OrderLocalID.c_str() );
	UFC::BufferedLog::Printf( " OnKSftRtnOrder: OrderStatus[%s] OrderSubmitStatus[%s]", Status.c_str(), SubmitStatus.c_str() );
	UFC::BufferedLog::FlushToFile();

	UpdateRecover();
	// ??
	if ( OrderLocalID.AnsiPos( '_' ) != -1 || OrdSysID.AnsiPos( '_' ) != -1 )
	{
		UFC::BufferedLog::Printf( " OnKSftRtnOrder: ----- It's a KSft server side Stop Order execution. Skip this execution! -----" );
		return;
	}
	///< Use FrontID + SessionID + OrderRef to find Order.
	if ( ( Record = FindOrderByOrderRef( pOrder->FrontID, pOrder->SessionID, OrdRef.ToInt() ) ) != NULL )
		UFC::BufferedLog::Printf( " OnKSftRtnOrder: Use FrontID + SessionID + OrderRef found the original order." );
	///< Use ExchangeID + OrderSysID to find Order.
	else if ( ( ExchangeID.Length() > 0 && OrdSysID.Length() > 0 ) &&
			  ( Record = FindOrderByOrderSysID( ExchangeID, OrdSysID.ToInt64() ) ) != NULL )
		UFC::BufferedLog::Printf( " OnKSftRtnOrder: Use ExchangeID + OrderSysID found the original order." );
	else ///< Order not found!
	{
		UFC::BufferedLog::Printf( " --------------------- Receive Execution from other KSft clients -------------------------" );
		UFC::BufferedLog::Printf( " OnKSftRtnOrder: Order Exchange[%s] OrderSysID[%s] OrderLocalID[%s] not found!", ExchangeID.c_str(), OrdSysID.c_str(), OrderLocalID.c_str());
		UFC::BufferedLog::Printf( " OnKSftRtnOrder: Add Order record FrontID[%d] SessionID[%d] OrderRef[%s]", pOrder->FrontID, pOrder->SessionID, pOrder->OrderRef );
		UFC::BufferedLog::Printf( " ----------------------------------------------------------------------------------------" );
		Record = AddPendingOrder( pOrder );
	}
	///< Update Record fields
	if ( Record->GetStatus() == cosPending && OrderLocalID.Length() > 0 && ExchangeID.Length() > 0 )
	{
		if ( OrdSysID == "0" || OrdSysID == " " ) ///< From Pending New status to KSft confirm status.( Update ExchangeID, OrderLocalID )
		{
			Record->CTPAccepted( ExchangeID, OrderLocalID );
			//AddOrderLocalIDToMap( ExchangeID, OrderLocalID.ToInt(), Record );
			KSftPendingExecution( Record, pOrder ); ///< Send pending New execution.
			//TradeLogKSftConfirm( pOrder );
		}
		else ///< From KSft confirm status to Exchange confirm status.( Update ExchangeID, OrderSysID, OrderLocalID )
		{
			Record->EachangeAccepted( ExchangeID, OrdSysID, OrderLocalID );
			AddOrderSysIDToMap( ExchangeID, OrdSysID.ToInt64() , Record );
			KSftConfirmExecution( Record, pOrder ); ///< Send confirm execution.
			//TradeLogExConfirm( pOrder );
		}
	}
	else if( Record->GetStatus() == cosConfirm && OrdSysID.Length() > 0 && ExchangeID.Length() > 0 )
	{   ///< From CTP confirm status to Exchange confirm status.( OrderSysID )
		Record->EachangeAccepted( OrdSysID );
		AddOrderSysIDToMap( ExchangeID, OrdSysID.ToInt64() , Record );
		KSftConfirmExecution( Record, pOrder ); ///< Send confirm execution.
		//TradeLogExConfirm( pOrder );
	}

	if ( pOrder->OrderStatus == THOST_FTDC_OST_Canceled )
	{
		if ( pOrder->OrderSubmitStatus == THOST_FTDC_OSS_Accepted ||
			 pOrder->OrderSubmitStatus == THOST_FTDC_OSS_CancelSubmitted )
		{
			Record->CloseOrder();
			KSftCanceledExecution( Record, pOrder );///< Send Canceled execution.
			//TradeLogCancel( pOrder );
		}
		else if ( pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected ||
				  pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted )
		{
			Record->CloseOrder();
			KSftRejectExecution( Record, pOrder );///< Send Reject execution.
			//TradeLogCancel( pOrder );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftErrRtnOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String     ErrMsg( pRspInfo->ErrorMsg );
		UTF8String       ErrMsgUTF8 = ErrMsg;

		if ( pInputOrder != NULL )
		{
			UpdateRecover();
			UFC::BufferedLog::Printf( " OnKSftErrRtnOrderInsert: Symbol[%s] OrderRef[%s] Msg[%s]",
									  pInputOrder->InstrumentID,
									  pInputOrder->OrderRef,
									  ErrMsgUTF8.c_str() );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftErrRtnOrderAction( KS_::CThostFtdcOrderActionField *pOrderAction, KS_::CThostFtdcRspInfoField */*pRspInfo*/ )
{
	if ( pOrderAction != NULL )
	{
		UpdateRecover();
		GB2312String            ErrMsg( pOrderAction->StatusMsg );
		UTF8String              ErrMsgUTF8( ErrMsg );
		TExecutionReportMessage ExecutionReport;
		UFC::AnsiString         ExecID;
		UFC::AnsiString         ExchangeID( pOrderAction->ExchangeID );
		UFC::AnsiString         OrderSysID( pOrderAction->OrderSysID );
		COrderInfo*             Ord;
		bool IsDup;

		UFC::BufferedLog::Printf( " OnKSftErrRtnOrderAction: Symbol[%s] Exchange[%s] OrderSysID[%s] Msg[%s]"
								  , pOrderAction->InstrumentID, pOrderAction->ExchangeID, pOrderAction->OrderSysID, ErrMsgUTF8.c_str() );
		if ( ExchangeID.Length() == 0 || OrderSysID.Length() == 0 )
			return;
		if ( ( Ord = FindOrderByOrderSysID( ExchangeID, OrderSysID.ToInt64() ) ) != NULL )
		{
			OrderSysID.TrimLeft();
			UFC::BufferedLog::Printf( " ----------------------- KSft Cancel Reject -------------------------" );
			ExecID.Printf( "CR_%s_%s_%d", ExchangeID.c_str(), OrderSysID.c_str(), atoi(pOrderAction->OrderRef) );
			CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Ord ); // ??
			ExecutionReport.SetOrderID( OrderSysID.c_str() );
			ExecutionReport.SetNID( Ord->GetOrderNID() );
			ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osRejected );
			ExecutionReport.SetExecType( nsOrderMessageDefine::etRejected );
			ExecutionReport.SetCxlRejResponseTo( nsOrderMessageDefine::crrCancel );
			ExecutionReport.SetAllUserData( Ord->GetCxlUserData() );
			ExecutionReport.SetExecID( ExecID.c_str() );
			ExecutionReport.SetLastQty( 0 );
			ExecutionReport.SetTransactTime( pOrderAction->ActionTime );
			IsDup = IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
			UFC::BufferedLog::Printf( " OrderRef[%s] ExchangeID[%s] Symbol[%s] OrderSysID[%s]"
									  , pOrderAction->OrderRef, ExchangeID.c_str(), pOrderAction->InstrumentID, OrderSysID.c_str() );
			if ( ErrMsg.Length() > 0 )
			{
				ExecutionReport.SetText( ErrMsgUTF8.c_str() );
				UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] Msg[%s]", ExecID.c_str(), IsDup ? "Yes" : "No", ErrMsgUTF8.c_str() );
			}
			else
				UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] ", ExecID.c_str(), IsDup ? "Yes" : "No" );
			TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
			UFC::BufferedLog::Printf( " ------------------------ Sent Cancel Reject ------------------------" );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
// [KSft] Order fill execution
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::OnKSftRtnTrade( KS_::CThostFtdcTradeField *pTrade )
{
	if ( pTrade == NULL )
		return;

	// ExchangeID + OrderSysIT to find Order record.
	if ( strlen( pTrade->ExchangeID ) > 0 && strlen( pTrade->OrderLocalID ) >0 && strlen( pTrade->OrderSysID ) >0 )
	{
		UpdateRecover();
		UFC::AnsiString ExchangeID( pTrade->ExchangeID );
		Int64           OrdSysIDInt64     = _atoi64( pTrade->OrderSysID );
		int             OrderLocalIDInt = atoi( pTrade->OrderLocalID );
		COrderInfo*     Ord;

		if ( ( Ord = FindOrderByOrderSysID( ExchangeID, OrdSysIDInt64 ) )!= NULL )
		{
			KSftFillExecution( Ord, pTrade );
			//TradeLogFill( pTrade );
		}
		else
		{
			UFC::BufferedLog::Printf( " OnKSftRtnTrade: Exchange[%s] OrderSysID[%lld] OrderLocalID[%d] not found! update fill failed."
									  , ExchangeID.c_str(), OrdSysIDInt64, OrderLocalIDInt );
		}
	}
	else
		UFC::BufferedLog::Printf( " OnKSftRtnTrade: missing field OrderSysID,OrderLocalID or ExchangeID." );
}
//------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString TTaifexConnection::BuildKS_ResponseInformationStr( KS_::CThostFtdcRspInfoField *pRspInfo, const UFC::AnsiString& FieldDelimiter )
{
	UFC::AnsiString ansiResponseStr = "";
	if ( pRspInfo == NULL )
		ansiResponseStr.Printf( "0%s", FieldDelimiter.c_str() );
	else
	{
		int errorId = pRspInfo->ErrorID;  //1.錯誤代碼
		AppendFieldToFieldsStr( ansiResponseStr, errorId, FieldDelimiter );
		GB2312String gb2312ErrorMsg( pRspInfo->ErrorMsg );  //2.錯誤訊息
		UTF8String utf8ErrorMsg( gb2312ErrorMsg );
		UFC::AnsiString ansiErrorMsg( utf8ErrorMsg.c_str() );
		AppendFieldToFieldsStr( ansiResponseStr, ansiErrorMsg, FieldDelimiter );
	}
	return ansiResponseStr;
}
//------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString TTaifexConnection::BuildKS_TransferRequestStr( KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, KS_::CThostFtdcReqTransferField *pReqTransfer )
{
	UFC::AnsiString ansiTransferRequestStr = BuildKS_ResponseInformationStr( pRspInfo, "|" );
	AppendFieldToFieldsStr( ansiTransferRequestStr, nRequestID, "|" );
	AppendFieldToFieldsStr( ansiTransferRequestStr, bIsLast, "|" );
	if ( pReqTransfer != NULL )
	{
		UFC::AnsiString ansiTradeCode = pReqTransfer->TradeCode;  // 5.業務功能碼
		AppendFieldToFieldsStr( ansiTransferRequestStr, ansiTradeCode, "|" );
		UFC::AnsiString ansiBankId = pReqTransfer->BankID;  // 6.銀行代碼
		AppendFieldToFieldsStr( ansiTransferRequestStr, ansiBankId, "|" );
		UFC::AnsiString ansiBankBranchId = pReqTransfer->BankBranchID;  // 7.銀行分支機構代碼
		AppendFieldToFieldsStr( ansiTransferRequestStr, ansiBankBranchId, "|" );
		UFC::AnsiString ansiBrokerId = pReqTransfer->BrokerID;  // 8.期貨公司代碼
		AppendFieldToFieldsStr( ansiTransferRequestStr, ansiBrokerId, "|" );
		UFC::AnsiString ansiTradeDate = pReqTransfer->TradeDate;  // 9.交易日期
		AppendFieldToFieldsStr( ansiTransferRequestStr, ansiTradeDate, "|" );
		UFC::AnsiString ansiTradeTime = pReqTransfer->TradeTime;  //10.交易時間
		AppendFieldToFieldsStr( ansiTransferRequestStr, ansiTradeTime, "|" );
		UFC::AnsiString ansiBankSerial = pReqTransfer->BankSerial;  //11.銀行流水號
		AppendFieldToFieldsStr( ansiTransferRequestStr, ansiBankSerial, "|" );
		UFC::AnsiString ansiSystemTradeDate = pReqTransfer->TradingDay;  //12.系統交易日期
		AppendFieldToFieldsStr( ansiTransferRequestStr, ansiSystemTradeDate, "|" );
		int plateSerial = pReqTransfer->PlateSerial;  //13.銀期平台消息流水號
		AppendFieldToFieldsStr( ansiTransferRequestStr, plateSerial, "|" );
		char lastFragment = pReqTransfer->LastFragment;  //14.最後分片標誌
		AppendFieldToFieldsStr( ansiTransferRequestStr, lastFragment, "|" );
		int sessionId = pReqTransfer->SessionID;  //15.會話號
		AppendFieldToFieldsStr( ansiTransferRequestStr, sessionId, "|" );
		UFC::AnsiString ansiCustomerName = pReqTransfer->CustomerName;  //16.客戶姓名
		AppendFieldToFieldsStr( ansiTransferRequestStr, ansiCustomerName, "|" );
		char idCardType = pReqTransfer->IdCardType;  //17.證件類型
		AppendFieldToFieldsStr( ansiTransferRequestStr, idCardType, "|" );
		UFC::AnsiString ansiIdentifiedCardNo = pReqTransfer->IdentifiedCardNo;  //18.證件號碼
		AppendFieldToFieldsStr( ansiTransferRequestStr, ansiIdentifiedCardNo, "|" );
		char customType = pReqTransfer->CustType;  //19.客戶類型
		AppendFieldToFieldsStr( ansiTransferRequestStr, customType, "|" );
		UFC::AnsiString ansiBankAccount = pReqTransfer->BankAccount;  //20.銀行帳號
		AppendFieldToFieldsStr( ansiTransferRequestStr, ansiBankAccount, "|" );
		UFC::AnsiString ansiBankPassword = pReqTransfer->BankPassWord;  //21.銀行密碼
		AppendFieldToFieldsStr( ansiTransferRequestStr, ansiBankPassword, "|" );
		UFC::AnsiString ansiAccountId = pReqTransfer->AccountID;  //22.投資者帳號
		AppendFieldToFieldsStr( ansiTransferRequestStr, ansiAccountId, "|" );
		UFC::AnsiString ansiAccountPassword = pReqTransfer->Password;  //23.期貨密碼
		AppendFieldToFieldsStr( ansiTransferRequestStr, ansiAccountPassword, "|" );
		UFC::AnsiString ansiCurrencyId = pReqTransfer->CurrencyID;  //24.幣種
		AppendFieldToFieldsStr( ansiTransferRequestStr, ansiCurrencyId, "|" );
		double tradeAmount = pReqTransfer->TradeAmount;  //25.轉帳金額
		AppendFieldToFieldsStr( ansiTransferRequestStr, tradeAmount, "|" );
		double futureFetchAmount = pReqTransfer->FutureFetchAmount;  //26.期貨可取金額
		AppendFieldToFieldsStr( ansiTransferRequestStr, futureFetchAmount, "|" );
		char feePaymentFlag = pReqTransfer->FeePayFlag;  //27.費用支付標誌 '0':由受益方支付  '1':由發送方支付  '2':由發送方支付發起的費用，由受益方支付接受的費用
		AppendFieldToFieldsStr( ansiTransferRequestStr, feePaymentFlag, "|" );
		double customFee = pReqTransfer->CustFee;  //28.應收客戶費用
		AppendFieldToFieldsStr( ansiTransferRequestStr, customFee, "|" );
		double brokerFee = pReqTransfer->BrokerFee;  //29.應收期貨公司費用
		AppendFieldToFieldsStr( ansiTransferRequestStr, brokerFee, "|" );
		UFC::AnsiString ansiMessage = pReqTransfer->Message;  //30.發送方給接收方的消息
		AppendFieldToFieldsStr( ansiTransferRequestStr, ansiMessage, "|" );
		UFC::AnsiString ansiDigest = pReqTransfer->Digest;  //31.摘要
		AppendFieldToFieldsStr( ansiTransferRequestStr, ansiDigest, "|" );
		char bankAccountType = pReqTransfer->BankAccType;  //32.銀行帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
		AppendFieldToFieldsStr( ansiTransferRequestStr, bankAccountType, "|" );
		UFC::AnsiString ansiDeviceId = pReqTransfer->DeviceID;  //33.渠道標誌
		AppendFieldToFieldsStr( ansiTransferRequestStr, ansiDeviceId, "|" );
		char bankSecurityAccountType = pReqTransfer->BankSecuAccType;  //34.期貨單位帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
		AppendFieldToFieldsStr( ansiTransferRequestStr, bankSecurityAccountType, "|" );
		UFC::AnsiString ansiBrokerIDByBank = pReqTransfer->BrokerIDByBank;  //35.期貨公司銀行編碼
		AppendFieldToFieldsStr( ansiTransferRequestStr, ansiBrokerIDByBank, "|" );
		UFC::AnsiString ansiBankSecurityAccount = pReqTransfer->BankSecuAcc;  //34.期貨單位帳號
		AppendFieldToFieldsStr( ansiTransferRequestStr, ansiBankSecurityAccount, "|" );
		char bankPasswordFlag = pReqTransfer->BankPwdFlag;  //36.銀行密碼標誌 '0':不核對  '1':明文核對  '2':密文核對
		AppendFieldToFieldsStr( ansiTransferRequestStr, bankPasswordFlag, "|" );
		char securityPasswordFlag = pReqTransfer->SecuPwdFlag;  //37.期貨資金密碼核對標誌 '0':不核對  '1':明文核對  '2':密文核對
		AppendFieldToFieldsStr( ansiTransferRequestStr, securityPasswordFlag, "|" );
		UFC::AnsiString ansiOperatorNo = pReqTransfer->OperNo;  //38.交易櫃員
		AppendFieldToFieldsStr( ansiTransferRequestStr, ansiOperatorNo, "|" );
		int requestId = pReqTransfer->RequestID;  //39.請求編號
		AppendFieldToFieldsStr( ansiTransferRequestStr, requestId, "|" );
		int tradeId = pReqTransfer->TID;  //40.交易ID
		AppendFieldToFieldsStr( ansiTransferRequestStr, tradeId, "|" );
		char transferStatus = pReqTransfer->TransferStatus;  //41.轉帳交易狀態 '0':正常  '1':被沖正
		AppendFieldToFieldsStr( ansiTransferRequestStr, transferStatus, "|" );
	}
	return ansiTransferRequestStr;
}
//------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString TTaifexConnection::BuildKS_TransferResponseStr( KS_::CThostFtdcRspTransferField *pRspTransfer )
{
	UFC::AnsiString ansiTransferResponseStr = "";
	int errorId = pRspTransfer->ErrorID;  //1.錯誤代碼
	AppendFieldToFieldsStr( ansiTransferResponseStr, errorId, "|" );
	GB2312String gb2312ErrorMsg( pRspTransfer->ErrorMsg );  //2.錯誤訊息
	UTF8String utf8ErrorMsg( gb2312ErrorMsg );
	UFC::AnsiString ansiErrorMsg( utf8ErrorMsg.c_str() );
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiErrorMsg, "|" );
	UFC::AnsiString ansiTradeCode = pRspTransfer->TradeCode;  //3.業務功能碼
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiTradeCode, "|" );
	UFC::AnsiString ansiBankId = pRspTransfer->BankID;  //4.銀行代碼
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiBankId, "|" );
	UFC::AnsiString ansiBankBranchId = pRspTransfer->BankBranchID;  //5.銀行分支機構代碼
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiBankBranchId, "|" );
	UFC::AnsiString ansiBrokerId = pRspTransfer->BrokerID;  //6.期貨公司代碼
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiBrokerId, "|" );
	UFC::AnsiString ansiTradeDate = pRspTransfer->TradeDate;  //7.交易日期
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiTradeDate, "|" );
	UFC::AnsiString ansiTradeTime = pRspTransfer->TradeTime;  //8.交易時間
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiTradeTime, "|" );
	UFC::AnsiString ansiBankSerial = pRspTransfer->BankSerial;  //9.銀行流水號
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiBankSerial, "|" );
	UFC::AnsiString ansiSystemTradeDate = pRspTransfer->TradingDay;  //10.系統交易日期
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiSystemTradeDate, "|" );
	int plateSerial = pRspTransfer->PlateSerial;  //11.銀期平台消息流水號
	AppendFieldToFieldsStr( ansiTransferResponseStr, plateSerial, "|" );
	char lastFragment = pRspTransfer->LastFragment;  //12.最後分片標誌
	AppendFieldToFieldsStr( ansiTransferResponseStr, lastFragment, "|" );
	int sessionId = pRspTransfer->SessionID;  //13.會話號
	AppendFieldToFieldsStr( ansiTransferResponseStr, sessionId, "|" );
	GB2312String gb2312CustomerName( pRspTransfer->CustomerName );  //14.客戶姓名
	UTF8String utf8CustomerName( gb2312CustomerName );
	UFC::AnsiString ansiCustomerName( utf8CustomerName.c_str() );
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiCustomerName, "|" );
	char idCardType = pRspTransfer->IdCardType;  //15.證件類型
	AppendFieldToFieldsStr( ansiTransferResponseStr, idCardType, "|" );
	UFC::AnsiString ansiIdentifiedCardNo = pRspTransfer->IdentifiedCardNo;  //16.證件號碼
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiIdentifiedCardNo, "|" );
	char customType = pRspTransfer->CustType;  //17.客戶類型
	AppendFieldToFieldsStr( ansiTransferResponseStr, customType, "|" );
	UFC::AnsiString ansiBankAccount = pRspTransfer->BankAccount;  //18.銀行帳號
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiBankAccount, "|" );
	UFC::AnsiString ansiBankPassword = pRspTransfer->BankPassWord;  //19.銀行密碼
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiBankPassword, "|" );
	UFC::AnsiString ansiAccountId = pRspTransfer->AccountID;  //20.投資者帳號
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiAccountId, "|" );
	UFC::AnsiString ansiAccountPassword = pRspTransfer->Password;  //21.期貨密碼
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiAccountPassword, "|" );
	int futureSerial = pRspTransfer->FutureSerial;  //22.期貨公司流水號
	AppendFieldToFieldsStr( ansiTransferResponseStr, futureSerial, "|" );
	UFC::AnsiString ansiCurrencyId = pRspTransfer->CurrencyID;  //23.幣種
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiCurrencyId, "|" );
	double tradeAmount = pRspTransfer->TradeAmount;  //24.轉帳金額
	AppendFieldToFieldsStr( ansiTransferResponseStr, tradeAmount, "|" );
	double futureFetchAmount = pRspTransfer->FutureFetchAmount;  //25.期貨可取金額
	AppendFieldToFieldsStr( ansiTransferResponseStr, futureFetchAmount, "|" );
	char feePaymentFlag = pRspTransfer->FeePayFlag;  //26.費用支付標誌 '0':由受益方支付  '1':由發送方支付  '2':由發送方支付發起的費用，由受益方支付接受的費用
	AppendFieldToFieldsStr( ansiTransferResponseStr, feePaymentFlag, "|" );
	double customFee = pRspTransfer->CustFee;  //27.應收客戶費用
	AppendFieldToFieldsStr( ansiTransferResponseStr, customFee, "|" );
	double brokerFee = pRspTransfer->BrokerFee;  //28.應收期貨公司費用
	AppendFieldToFieldsStr( ansiTransferResponseStr, brokerFee, "|" );
	UFC::AnsiString ansiMessage = pRspTransfer->Message;  //29.發送方給接收方的消息
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiMessage, "|" );
	UFC::AnsiString ansiDigest = pRspTransfer->Digest;  //30.摘要
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiDigest, "|" );
	char bankAccountType = pRspTransfer->BankAccType;  //31.銀行帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
	AppendFieldToFieldsStr( ansiTransferResponseStr, bankAccountType, "|" );
	UFC::AnsiString ansiDeviceId = pRspTransfer->DeviceID;  //32.渠道標誌
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiDeviceId, "|" );
	char bankSecurityAccountType = pRspTransfer->BankSecuAccType;  //33.期貨單位帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
	AppendFieldToFieldsStr( ansiTransferResponseStr, bankSecurityAccountType, "|" );
	UFC::AnsiString ansiBrokerIDByBank = pRspTransfer->BrokerIDByBank;  //34.期貨公司銀行編碼
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiBrokerIDByBank, "|" );
	UFC::AnsiString ansiBankSecurityAccount = pRspTransfer->BankSecuAcc;  //35.期貨單位帳號
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiBankSecurityAccount, "|" );
	char bankPasswordFlag = pRspTransfer->BankPwdFlag;  //36.銀行密碼標誌 '0':不核對  '1':明文核對  '2':密文核對
	AppendFieldToFieldsStr( ansiTransferResponseStr, bankPasswordFlag, "|" );
	char securityPasswordFlag = pRspTransfer->SecuPwdFlag;  //37.期貨資金密碼核對標誌 '0':不核對  '1':明文核對  '2':密文核對
	AppendFieldToFieldsStr( ansiTransferResponseStr, securityPasswordFlag, "|" );
	UFC::AnsiString ansiOperatorNo = pRspTransfer->OperNo;  //38.交易櫃員
	AppendFieldToFieldsStr( ansiTransferResponseStr, ansiOperatorNo, "|" );
	int requestId = pRspTransfer->RequestID;  //39.請求編號
	AppendFieldToFieldsStr( ansiTransferResponseStr, requestId, "|" );
	int tradeId = pRspTransfer->TID;  //40.交易ID
	AppendFieldToFieldsStr( ansiTransferResponseStr, tradeId, "|" );
	char transferStatus = pRspTransfer->TransferStatus;  //41.轉帳交易狀態 '0':正常  '1':被沖正
	AppendFieldToFieldsStr( ansiTransferResponseStr, transferStatus, "|" );
	return ansiTransferResponseStr;
}
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
