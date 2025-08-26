//---------------------------------------------------------------------------
#ifdef _CTP
//------------------------------------------------------------------------------------------------------------------------
#ifndef _KSFT
//------------------------------------------------------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//------------------------------------------------------------------------------------------------------------------------
#include "KStrdFromSpeedyAPI.h"
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
//  Fill       ExecID: "F" + ExchangeID + OrderSysID + SequenceNo (KStrd only support V8, use TradeID replace)
//  Fill       ExecID: "F" + ExchangeID + OrderSysID + TradeID
//  Canceled   ExecID: "C" + FrontID + SessionID + OrderRef
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//
// Function KStrdNewOrder
// call by Speedy API NewOrder function. ( TTaifexConnection->NewOrder )
//
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::KStrdNewOrder( TNewOrderMessage* Msg, const UFC::AnsiString& UserDefine )
{
	KS_::CThostFtdcInputOrderField ord;
	UFC::AnsiString ErrMsg;
	UFC::AnsiString  CombHedgeFlagStr;
	bool            CanSend = true;
	int             OrderRef = COrderInfo::ToOrderRef( Msg->GetNID() );
	int             Rtn;

	if (OrderRef <= 1)
	{
		// KSFTAPI 2.3.50416 要先加1
		// MaxOrderRef[1]
		// 從2開始
		OrderRef = COrderInfo::ToOrderRef( GenerateNID( Msg->GetMessageType() ) );
	}

	UFC::BufferedLog::Printf( " --------------------- KStrd Place New Order ----------------------" );
	memset( &ord, 0, sizeof( ord ) );
	strcpy( ord.BrokerID,     FCFFEXBrokerID.c_str() );	///< Broker ID
	strcpy( ord.InvestorID,   Msg->GetAccount() );		///< Account
	strcpy( ord.UserID,       Msg->GetAccount() );		///< Account. could be AE.
	strcpy( ord.InstrumentID, Msg->GetSymbol() );		///< Symbol
	strcpy( ord.ExchangeID,	  Msg->GetExchangeCode() ); /// 交易所代碼
	// Side (B/S)
	if ( Msg->GetSide() == nsOrderMessageDefine::sBuy )
		ord.Direction = THOST_FTDC_D_Buy;
	else
		ord.Direction = THOST_FTDC_D_Sell;
	// Order Type (Market or Limit)
	if ( Msg->GetOrderType() == nsOrderMessageDefine::otMarket )
	{
		ord.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
		ord.LimitPrice     = 0.0;
	}
	else
	{
		ord.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		ord.LimitPrice     = Msg->GetPrice();
	}
	// Order Qty
	ord.VolumeTotalOriginal = Msg->GetOrderQty();
	// Time in force.
	switch ( Msg->GetTimeInForce() )
	{
		case nsOrderMessageDefine::tifROD:  ord.TimeCondition = THOST_FTDC_TC_GFD; break;
		case nsOrderMessageDefine::tifIOC:  ord.TimeCondition = THOST_FTDC_TC_IOC; break;
		case nsOrderMessageDefine::tifFOK:  ord.TimeCondition = THOST_FTDC_TC_GFD; break;
		default: ord.TimeCondition = THOST_FTDC_TC_GFD; break;
	}
	// Position effect.
	if ( Msg->GetPositionEffect() == nsOrderMessageDefine::peOpen )
		ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open;		///< Open
	else if( Msg->GetPositionEffect() == nsOrderMessageDefine::peClose )
		ord.CombOffsetFlag[0] = THOST_FTDC_OF_Close;		///< Close
	else if( Msg->GetPositionEffect() == nsOrderMessageDefine::peRolled )
		ord.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;	///< Close Today
	else if( Msg->GetPositionEffect() == nsOrderMessageDefine::peAutoToday )
		ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open;		///< Auto Today
	else
		ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open;		///< Auto
	// CombHedge Flag. ///< 組合投機套保標誌
	if ( Msg->GetCombHedgeFlag() == nsOrderMessageDefine::hfSpeculation )
	{
		ord.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;		///< Speculation ///投機/非備兌
		CombHedgeFlagStr = "1:Speculation";
	}
	else if( Msg->GetCombHedgeFlag() == nsOrderMessageDefine::hfArbitrage )
	{
		ord.CombHedgeFlag[0] = THOST_FTDC_HF_Arbitrage;		///< Arbitrage ///套利
		CombHedgeFlagStr = "2:Arbitrage";
	}
	else if( Msg->GetCombHedgeFlag() == nsOrderMessageDefine::hfHedge )
	{
		ord.CombHedgeFlag[0] = THOST_FTDC_HF_Hedge;		///< Hedge ///套保
		CombHedgeFlagStr = "3:Hedge";
	}
	else if( Msg->GetCombHedgeFlag() == nsOrderMessageDefine::hfCovered )
	{
		ord.CombHedgeFlag[0] = THOST_FTDC_HF_Covered;		///< Covered ///備兌
		CombHedgeFlagStr = "4:Covered";
	}
	else
	{
		strcpy( ord.CombHedgeFlag, "1" );	///投機/非備兌
		CombHedgeFlagStr = "1:Speculation";
	}
	// Fixed value fields
	strcpy( ord.GTDDate, "" );			///< GTD Date
	ord.VolumeCondition     = THOST_FTDC_VC_AV; ///< Volumn Condition
	ord.MinVolume           = 0; ///< 最小成交量
	ord.ContingentCondition = THOST_FTDC_CC_Immediately; ///< 觸發條件
	ord.StopPrice = 0; ///< 止損價
	ord.ForceCloseReason = THOST_FTDC_FCC_NotForceClose; ///< 強平原因
	ord.IsAutoSuspend = 0; ///< 自動掛起標誌
	// Order Sequence
	sprintf( ord.OrderRef, "%012d", OrderRef ); ///< 報單引用
	// Allow to place New order ?
	if( FSendEventListener != NULL )
	{
		FSendEventListener->OnBeforeSend( Msg->GetMarket(), smtNew, Msg, CanSend, ErrMsg );
		if( CanSend == false )
		{
			Reject( nsOrderMessageDefine::crrNew, ErrMsg, Msg ,Msg->GetTradingSessionID());
			return;
		}
	}
	// Prepare PE string
	UFC::AnsiString PE;
	if ( ord.CombOffsetFlag[0] == THOST_FTDC_OF_Open )
		PE = "Open";
	else if ( ord.CombOffsetFlag[0] == THOST_FTDC_OF_Close )
		PE = "Close";
	else if ( ord.CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday )
		PE = "CloseToday";
	UFC::BufferedLog::Printf( " KStrdNewOrder: OrderRef[%d] Account[%s] Symbol[%s] Side[%c]",
							  OrderRef, ord.UserID, ord.InstrumentID,
							  ord.Direction == THOST_FTDC_D_Buy? 'B':'S' );
	UFC::BufferedLog::Printf( " KStrdNewOrder: Qty[%d] Px[%0.3f] TIF[%s] OT[%s] PE[%s] CombHedgeFlag[%s]",
							  ord.VolumeTotalOriginal, ord.LimitPrice,
							  ord.TimeCondition == THOST_FTDC_TC_GFD? "ROD":"IOC",
							  ord.OrderPriceType == THOST_FTDC_OPT_AnyPrice? "Market" : "Limit", PE.c_str(), CombHedgeFlagStr.c_str() );
	if ( ( Rtn = FKSTRDAPI->ReqOrderInsert( &ord, CTPReqID() ) ) != 0 )
	{
		UFC::BufferedLog::Printf( " ----------------------- Send KStrdNewOrder failed:%d ------------------------", Rtn );
		ErrMsg.Printf( "Call ReqOrderInsert return error, Code[%d]", Rtn );
		Reject( nsOrderMessageDefine::crrNew, ErrMsg, Msg ,Msg->GetTradingSessionID());
	}
	else ///< OK.
	{
		UFC::BufferedLog::Printf( " ---------------------- Send KStrdNewOrder OK ------------------------" );
		KStrdAddPendingOrder( OrderRef, ord, UserDefine );
		//TradeLogNew( FCTPFrontID, FCTPSessionID, ord, UserDefine );
	}
}
//------------------------------------------------------------------------------------------------------------------------
//
// Function KStrdCancelOrder
// call by Speedy API CancelOrder function. ( TTaifexConnection->CancelOrder )
//
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::KStrdCancelOrder( TCancelOrderMessage* Msg, const UFC::AnsiString& UserDefine )
{
	UFC::AnsiString OrderID( Msg->GetOrderID() );
	UFC::AnsiString ExchangeID( Msg->GetExchangeCode() );
	UFC::AnsiString ErrMsg;
	COrderInfo*     DelOrd;
	bool            CanSend = true;
	int             CancelOrderRef = COrderInfo::ToOrderRef( Msg->GetNID() );
	int             OrgOrderRef    = COrderInfo::ToOrderRef( Msg->GetCNID() );
	Int64           OrderIDInt64   = OrderID.ToInt64();
	int             Rtn;

	if (CancelOrderRef <= 1)
	{
		// KSFTAPI 2.3.50416 要先加1
		// MaxOrderRef[1]
		// 從2開始
		CancelOrderRef = COrderInfo::ToOrderRef( GenerateNID( Msg->GetMessageType() ) );
	}

	UFC::BufferedLog::Printf( " ------------------ User Place Cancel Order ----------------------" );
	// Check cancel key.
	if ( ( OrderID.Length() == 0 || ExchangeID.Length() == 0 ) && OrgOrderRef  == 0 )
	{
		ErrMsg.Printf( "Missing OrderID, ExchangeID or OrderActionRef." );
		Reject( nsOrderMessageDefine::crrCancel, ErrMsg, Msg ,Msg->GetTradingSessionID());
		UFC::BufferedLog::Printf( " Missing OrderID, ExchangeID or OrderActionRef." );
		UFC::BufferedLog::Printf( " --------------------- Reject Order Cancel -----------------------" );
		return;
	}
	// Find the original order.
	if ( ( DelOrd = FindOrderByOrderSysID( ExchangeID, OrderIDInt64 ) ) != NULL )
		UFC::BufferedLog::Printf( " Cancel by OrderSysID[%s]", OrderID.c_str() );
	else if ( ( DelOrd = FindOrderByOrderLocalID( ExchangeID, OrderIDInt64 ) ) != NULL )
		UFC::BufferedLog::Printf( " Cancel by OrderLocalID[%s]", OrderID.c_str()  );
	else if ( ( DelOrd = FindOrderByOrderRef( FCTPFrontID, FCTPSessionID, OrgOrderRef ) ) != NULL )
		UFC::BufferedLog::Printf( " Cancel by FrontID[%d] SessionID[%d] OrderRef[%d]", FCTPFrontID, FCTPSessionID, OrgOrderRef );
	else
	{
		ErrMsg.Printf( "Order not found! Exchange[%s] OrderLocalID[%d]", ExchangeID.c_str(), atoi( Msg->GetOrderID() ) );
		Reject( nsOrderMessageDefine::crrCancel, ErrMsg, Msg ,Msg->GetTradingSessionID());
		UFC::BufferedLog::Printf( " Cancel Order not found! Exchange[%s] OrderID[%s] CNID[%d]", ExchangeID.c_str(), OrderID.c_str(), OrgOrderRef );
		UFC::BufferedLog::Printf( " --------------------- Reject Order Cancel -----------------------" );
		return;
	}

	// Pass all check, send Order Cancel to KStrd.
	KS_::CThostFtdcInputOrderActionField	CancelOrder;
	UFC::AnsiString							OrderSysID( DelOrd->GetOrderSysID() );

	//if ( OrderSysID.Length() < 12 )           ///< CZCE OrderSysID Format: 9(16)
	//	OrderSysID.PadThis( 12, ' ', false ); ///< CTP OrderSysID Format: space(6) + 9(6)

	DelOrd->Cancel( UserDefine, CancelOrderRef );
	memset( &CancelOrder, 0, sizeof(CancelOrder) );
	CancelOrder.LimitPrice     = 0.0;
	CancelOrder.VolumeChange   = 0;
	CancelOrder.ActionFlag     = THOST_FTDC_AF_Delete;
	CancelOrder.FrontID        = DelOrd->GetFrontID();
	CancelOrder.SessionID      = DelOrd->GetSessionID();
	CancelOrder.OrderActionRef = DelOrd->GetOrderRef();
	strcpy( CancelOrder.InvestorID,    Msg->GetAccount() );
	strcpy( CancelOrder.InstrumentID,  Msg->GetSymbol() );
	strcpy( CancelOrder.BrokerID,      DelOrd->GetBrokerID() );
	strcpy( CancelOrder.UserID,        DelOrd->GetUserID() );
	strcpy( CancelOrder.ExchangeID,    DelOrd->GetExchangeID() );
	strcpy( CancelOrder.OrderSysID,    OrderSysID.c_str() );

	/*
	KS刘艳云-API开发:
	新版也可以用exchangeid+ordersysid撤单的，
	优先使用FrontID SessionID OrderRef撤单，
	如果这几个值有没填的，就用exchangeid+ordersysid撤单
	*/
	// Order Sequence.
	/* CancelOrderRef => 會變動 ???
	sprintf( CancelOrder.OrderRef, "%012d", CancelOrderRef );
	*/
	sprintf( CancelOrder.OrderRef, "%012d", DelOrd->GetOrderRef() );
	UFC::BufferedLog::Printf( " Cancel by FrontID[%d] SessionID[%d] OrderRef[%d]", FCTPFrontID, FCTPSessionID, DelOrd->GetOrderRef() );

	// Allow to place cancel order ?
	if ( FSendEventListener != NULL )
	{
		FSendEventListener->OnBeforeSend( Msg->GetMarket(), smtCancel, Msg, CanSend, ErrMsg );
		if ( CanSend == false )
		{
			Reject( nsOrderMessageDefine::crrCancel, ErrMsg, Msg ,Msg->GetTradingSessionID());
			return;
		}
	}
	UFC::BufferedLog::Printf( " KStrdCancelOrder: CancelOrderRef[%d] Account[%s] ExchangeID[%s] Symbol[%s]",
							  CancelOrderRef, CancelOrder.InvestorID, CancelOrder.ExchangeID, CancelOrder.InstrumentID );
	UFC::BufferedLog::Printf( " KStrdCancelOrder: FrontID[%d] SessionID[%d] OrderActionRef[%d] OrderSysID[%s]",
							  CancelOrder.FrontID, CancelOrder.SessionID, CancelOrder.OrderActionRef, CancelOrder.OrderSysID );
	if ( ( Rtn = FKSTRDAPI->ReqOrderAction( &CancelOrder, CTPReqID() ) ) != 0 )
	{
		UFC::BufferedLog::Printf( " ----------------------- Send KStrdCancelOrder failed:%d ------------------------", Rtn );
		ErrMsg.Printf( "Call ReqOrderAction return error, Code[%d]", Rtn );
		Reject( nsOrderMessageDefine::crrCancel, ErrMsg, Msg ,Msg->GetTradingSessionID());
	}
	else
	{
		UFC::BufferedLog::Printf( " -------------------- Send KStrdCancelOrder OK -----------------------" );
		//TradeLogCancel( OrderID.ToInt(), CancelOrder );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::KStrdSetTransferField( int KStrdRequestId, TNewsMessage* Msg, KS_::CThostFtdcReqTransferField& TransferFields )
{
	memset( &TransferFields, 0, sizeof(KS_::CThostFtdcReqTransferField) );
	UFC::NameValueMessage Params("^\n");
	Params.FromString( Msg->GetText() );

	UFC::AnsiString ansiTradeCode = ""; //業務功能碼
	if ( ( Params.Get("TradeCode", ansiTradeCode) == TRUE ) && ( ansiTradeCode.Length() > 0 ) )
		strcpy( TransferFields.TradeCode, ansiTradeCode.c_str() );

	UFC::AnsiString ansiBankId = ""; //銀行代碼
	if ( ( Params.Get("BankId", ansiBankId) == TRUE ) && ( ansiBankId.Length() > 0 ) )
		strcpy( TransferFields.BankID, ansiBankId.c_str() );

	UFC::AnsiString ansiBankBranchId = ""; //銀行分支機構代碼
	if ( Params.Get("BankBranchId", ansiBankBranchId) == TRUE )
	{
		if ( ansiBankBranchId.Length() <= 0 )
			ansiBankBranchId = "0000";
	}
	strcpy( TransferFields.BankBranchID, ansiBankBranchId.c_str() );

	UFC::AnsiString ansiBrokerId = ""; //期貨公司代碼
	if ( ( Params.Get("BrokerId", ansiBrokerId) == FALSE ) || ( ansiBrokerId.Length() <= 0 ) )
		ansiBrokerId = FCFFEXBrokerID;
	strcpy( TransferFields.BrokerID, ansiBrokerId.c_str() );

	UFC::AnsiString ansiBankPassword = ""; //銀行帳戶密碼
	if ( ( Params.Get("BankPassword", ansiBankPassword) == TRUE ) && ( ansiBankPassword.Length() > 0 ) )
		strcpy( TransferFields.BankPassWord, ansiBankPassword.c_str() );

	UFC::AnsiString ansiAccountId = ""; //投資者帳號
	if ( ( Params.Get("AccountId", ansiAccountId) == TRUE ) && ( ansiAccountId.Length() > 0 ) )
		strcpy( TransferFields.AccountID, ansiAccountId.c_str() );

	UFC::AnsiString ansiAccountPassword = ""; //資金帳戶密碼
	if ( ( Params.Get("AccountPassword", ansiAccountPassword) == TRUE ) && ( ansiAccountPassword.Length() > 0 ) )
		strcpy( TransferFields.Password, ansiAccountPassword.c_str() );

	TransferFields.SecuPwdFlag = '1';
	UFC::AnsiString ansiSecurityPasswordFlag; //資金帳戶密碼核對標誌
	if ( ( Params.Get("SecurityPasswordFlag", ansiSecurityPasswordFlag) == TRUE ) && ( ansiSecurityPasswordFlag.Length() > 0 ) )
	{
		if ( ansiSecurityPasswordFlag == "0" )
			TransferFields.SecuPwdFlag = '0'; //THOST_FTDC_BPWDF_NoCheck 不核對
		else if ( ansiSecurityPasswordFlag == "1" )
			TransferFields.SecuPwdFlag = '1'; //THOST_FTDC_BPWDF_BlankCheck 明文核對
		else if ( ansiSecurityPasswordFlag == "2" )
			TransferFields.SecuPwdFlag = '2'; //THOST_FTDC_BPWDF_EncryptCheck 密文核對
		else
			TransferFields.SecuPwdFlag = THOST_FTDC_BPWDF_BlankCheck; //'1' 明文核對
	}

	TransferFields.BankPwdFlag = THOST_FTDC_BPWDF_NoCheck; //銀行密碼標誌 '0':不核對 '1':明文核對 '2':密文核對
	TransferFields.VerifyCertNoFlag = THOST_FTDC_YNI_No; //'1'驗證客戶證件號碼標誌 '0':是 '1':否
	UFC::AnsiString ansiCurrencyId = ""; //幣種
	if ( ( Params.Get("CurrencyId", ansiCurrencyId) == TRUE ) && ( ansiCurrencyId.Length() > 0 ) )
		strcpy( TransferFields.CurrencyID, ansiCurrencyId.c_str() );

	double tradeAmount = 0.0; //轉帳金額
	UFC::AnsiString ansiTradeAmountStr = L"";
	if ( ( Params.Get("TradeAmount", ansiTradeAmountStr) == TRUE ) && ( ansiTradeAmountStr.Length() > 0 ) )
	{
		tradeAmount = ansiTradeAmountStr.ToDouble();
		TransferFields.TradeAmount = tradeAmount;
	}

	UFC::AnsiString ansiFunctionName = "";
	if ( ansiTradeCode == "202001" )
		ansiFunctionName = "ReqFromBankToFutureByFuture";
	else if ( ansiTradeCode == "202002" )
		ansiFunctionName = "ReqFromFutureToBankByFuture";

	UFC::BufferedLog::Printf( " Call %s: RequestID[%d] TradeCode[%s] BrokerID[%s] BankID[%s] InvestorID[%s] Amount[%13.2lf]%s"
							  , ansiFunctionName.c_str(), KStrdRequestId, ansiTradeCode.c_str(), ansiBrokerId.c_str(), ansiBankId.c_str(), ansiAccountId.c_str(), tradeAmount, ansiCurrencyId.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
COrderInfo* TTaifexConnection::KStrdAddPendingOrder( int OrdRef, KS_::CThostFtdcInputOrderField& Ord, const UFC::AnsiString& UserDefine )
{
	COrderInfo* NewOrd = new COrderInfo( FCTPFrontID, FCTPSessionID, Ord, UserDefine );
	UFC::AnsiString OrderRefKey;

	OrderRefKey.Printf( "%d|%d|%d", FCTPFrontID, FCTPSessionID, OrdRef );
	FOrderRecords.Add( NewOrd );
	FOrderRefToRecord.Add( OrderRefKey, NewOrd );
	return NewOrd;
}
//------------------------------------------------------------------------------------------------------------------------
COrderInfo* TTaifexConnection::KStrdAddPendingOrder( KS_::CThostFtdcOrderField* Ord )
{
	COrderInfo* NewOrd = new COrderInfo( Ord );
	UFC::AnsiString OrderRefKey;

	OrderRefKey.Printf( "%d|%d|%d", Ord->FrontID, Ord->SessionID, atoi( Ord->OrderRef ) );
	FOrderRecords.Add( NewOrd );
	FOrderRefToRecord.Add( OrderRefKey, NewOrd );
	return NewOrd;
}
//------------------------------------------------------------------------------------------------------------------------
void TTaifexConnection::KStrdPendingExecution( COrderInfo* Ord, KS_::CThostFtdcOrderField *pOrder )
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	GB2312String            ErrMsg( pOrder->StatusMsg );
	UFC::AnsiString         OrderLocalID( pOrder->OrderLocalID );
	Int64                   NID64 = Ord->GetOrderNID();
	bool IsDup;

	UFC::BufferedLog::Printf( " --------------------------- KStrd Pending ------------------------" );
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
void TTaifexConnection::KStrdConfirmExecution( COrderInfo* Ord, KS_::CThostFtdcOrderField *pOrder )
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	GB2312String            ErrMsg( pOrder->StatusMsg );
	UFC::AnsiString         OrderSysID( pOrder->OrderSysID );
	bool IsDup;

	UFC::BufferedLog::Printf( " --------------------------- KStrd Confirm ------------------------" );
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
void TTaifexConnection::KStrdRejectExecution( COrderInfo* Ord, KS_::CThostFtdcOrderField *pOrder )
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	GB2312String            ErrMsg( pOrder->StatusMsg );
	UFC::AnsiString         OrderLocalID( pOrder->OrderLocalID );
	UFC::AnsiString         OrderSysID( pOrder->OrderSysID );
	bool IsDup;

	UFC::BufferedLog::Printf( " ------------------------- KStrd Reject ---------------------------" );
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
void TTaifexConnection::KStrdCanceledExecution( COrderInfo* Ord, KS_::CThostFtdcOrderField *pOrder )
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	GB2312String            ErrMsg( pOrder->StatusMsg );
	UFC::AnsiString         OrderLocalID( pOrder->OrderLocalID );
	UFC::AnsiString         OrderSysID( pOrder->OrderSysID );
	bool IsDup;

	UFC::BufferedLog::Printf( " --------------------------- KStrd Cancled ------------------------" );
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
void TTaifexConnection::KStrdFillExecution( COrderInfo* Ord, KS_::CThostFtdcTradeField *pTrade )
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString OrdSysIDStr( pTrade->OrderSysID );
	UFC::AnsiString ExecID;
	bool IsDup;

	OrdSysIDStr.TrimLeft();
	UFC::BufferedLog::Printf( " ------------------------- KStrd Fill -----------------------------" );
	// SequenceNo KStrd only support V8, use TradeID replace
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
// Function HandleKStrdMessage
// Handle any request from User.( TTaifexConnection->SendNewsRequest )
//
// User : call TTaifexConnection->SendNewsRequest
// This func: call KStrd API -> ReqXXXX
//
//------------------------------------------------------------------------------------------------------------------------
int TTaifexConnection::HandleKStrdMessage( TNewsMessage* Msg )
{
	int  KStrdRequestId = CTPReqID();
	long NewsID = Msg->GetID();

	// KStrd no ReqXXXX implementation list
	// -- ReqXXXX

	if ( NewsID == CTP_QuerySettlementInfoConfirm ) ///< User confirm Settlement Info.
	{
		KS_::CThostFtdcQrySettlementInfoConfirmField QrySettlementInfoConfirm;
		///< Check  Settlement Info Confirm.
		memset( &QrySettlementInfoConfirm, 0, sizeof(KS_::CThostFtdcQrySettlementInfoConfirmField) );
		strcpy( QrySettlementInfoConfirm.BrokerID,   FCFFEXBrokerID.c_str() );
		strcpy( QrySettlementInfoConfirm.InvestorID, FID.c_str() );
		UFC::BufferedLog::Printf( " Call ReqQrySettlementInfoConfirm: RequestID[%d] BrokerID[%s] InvestorID[%s]"
								  , KStrdRequestId, FCFFEXBrokerID.c_str(), FID.c_str() );
		return FKSTRDAPI->ReqQrySettlementInfoConfirm( &QrySettlementInfoConfirm, KStrdRequestId );
	}
	else if ( NewsID == CTP_SettlementInfoConfirm ) ///< User confirm Settlement Info.
	{
		TDateTime Now = TDateTime::CurrentDateTime();
		unsigned short YY,MM,DD,hh,mm,ss,ms;
		AnsiString CurDateStr;
		AnsiString CurTimeStr;

		Now.DecodeDate( &YY,&MM,&DD );
		Now.DecodeTime( &hh,&mm,&ss,&ms );
		CurDateStr.printf( "%04d%02d%02d",YY,MM,DD );
		CurTimeStr.printf( "%02d:%02d:%02d", hh,mm,ss );

		KS_::CThostFtdcSettlementInfoConfirmField SettlementConfirmData;
		memset( &SettlementConfirmData, 0, sizeof(KS_::CThostFtdcSettlementInfoConfirmField) );
		strcpy( SettlementConfirmData.BrokerID,    FCFFEXBrokerID.c_str() );
		strcpy( SettlementConfirmData.InvestorID,  FID.c_str() );
		strcpy( SettlementConfirmData.ConfirmDate, CurDateStr.c_str() );
		strcpy( SettlementConfirmData.ConfirmTime, CurTimeStr.c_str() );
		UFC::BufferedLog::Printf( " Call ReqSettlementInfoConfirm: RequestID[%d] BrokerID[%s] InvestorID[%s] Date[%s][%s]"
								  , KStrdRequestId, FCFFEXBrokerID.c_str(), FID.c_str(), CurDateStr.c_str(), CurTimeStr.c_str() );
		return FKSTRDAPI->ReqSettlementInfoConfirm( &SettlementConfirmData, KStrdRequestId );
	}
	else if ( NewsID == CTP_QueryInvestorPosition ) ///< InvestorPositionData.
	{
		// Use <ReqQueryInvestorOpenPosition> replace <ReqQryInvestorPosition>

		//KS_::CThostFtdcQryInvestorPositionField qryInvestorPositionData;
		KS_::CThostFtdcQryInvestorPositionDetailField qryInvestorPositionData;
		memset( &qryInvestorPositionData, 0, sizeof(KS_::CThostFtdcQryInvestorPositionDetailField) );
		UFC::NameValueMessage Params("^\n");
		Params.FromString( Msg->GetText() );
		UFC::AnsiString ansiBrokerId = ""; //經紀公司代碼
		if ( ( Params.Get( "BrokerId", ansiBrokerId ) == FALSE ) || ( ansiBrokerId.Length() <= 0 ) )
			ansiBrokerId = FCFFEXBrokerID;
		strcpy( qryInvestorPositionData.BrokerID, ansiBrokerId.c_str() );

		UFC::AnsiString ansiInvestorId = ""; //投資者代碼
		if ( ( Params.Get( "InvestorId", ansiInvestorId ) == FALSE ) || ( ansiInvestorId.Length() <= 0 ) )
			ansiInvestorId = FID;
		strcpy( qryInvestorPositionData.InvestorID, ansiInvestorId.c_str() );

		UFC::AnsiString ansiInstrumentId = ""; //合約代碼
		if ( ( Params.Get( "InstrumentId", ansiInstrumentId ) == TRUE ) && ( ansiInstrumentId.Length() > 0 ) )
			strcpy( qryInvestorPositionData.InstrumentID, ansiInstrumentId.c_str() );

		//UFC::BufferedLog::Printf( " Call ReqQryInvestorPosition: RequestID[%d] UID[%d] BrokerID[%s] InvestorID[%s] InstrumentID[%s]"
		//						  , KStrdRequestId, Msg->GetID(), ansiBrokerId.c_str(), ansiInvestorId.c_str(), ansiInstrumentId.c_str() );
		//return FKSTRDAPI->ReqQryInvestorPosition( &qryInvestorPositionData, KStrdRequestId );
		UFC::BufferedLog::Printf( " Call ReqQueryInvestorOpenPosition: RequestID[%d] UID[%d] BrokerID[%s] InvestorID[%s] InstrumentID[%s]"
								  , KStrdRequestId, Msg->GetID(), ansiBrokerId.c_str(), ansiInvestorId.c_str(), ansiInstrumentId.c_str() );
		return FKSTRDAPI->ReqQueryInvestorOpenPosition( &qryInvestorPositionData, KStrdRequestId );
	}
	else if ( NewsID == CTP_QueryInvestorPositionDetail ) ///< InvestorPositionDetailData.
	{
		KS_::CThostFtdcQryInvestorPositionDetailField qryInvestorPositionDetailData;
		memset( &qryInvestorPositionDetailData, 0, sizeof(KS_::CThostFtdcQryInvestorPositionDetailField) );
		strcpy( qryInvestorPositionDetailData.BrokerID, FCFFEXBrokerID.c_str() );
		strcpy( qryInvestorPositionDetailData.InvestorID, FID.c_str() );
		UFC::BufferedLog::Printf( " Call ReqQryInvestorPositionDetail: RequestID[%d] UID[%d] BrokerID[%s] InvestorID[%s]"
								  , KStrdRequestId, Msg->GetID(), qryInvestorPositionDetailData.BrokerID, qryInvestorPositionDetailData.InvestorID );
		return FKSTRDAPI->ReqQryInvestorPositionDetail( &qryInvestorPositionDetailData, KStrdRequestId );
	}
	else if ( NewsID == CTP_ChangePassword ) ///< Change Password.
	{
		UFC::NameValueMessage Params("^\n");
		UFC::AnsiString OldPassword, NewPassword, Type;

		Params.FromString( Msg->GetText() );
		if ( Params.Get( "OldPassword", OldPassword )== true  &&
			 Params.Get( "NewPassword", NewPassword )== true  &&
			 Params.Get( "Type", Type )== true )
		{
			if ( Type == "Trade" )
			{
				KS_::CThostFtdcUserPasswordUpdateField UserPasswordUpdate;
				memset( &UserPasswordUpdate, 0, sizeof(KS_::CThostFtdcUserPasswordUpdateField) );
				strcpy( UserPasswordUpdate.BrokerID,    FCFFEXBrokerID.c_str() );
				strcpy( UserPasswordUpdate.UserID,      FID.c_str() );
				strcpy( UserPasswordUpdate.OldPassword, OldPassword.c_str() );
				strcpy( UserPasswordUpdate.NewPassword, NewPassword.c_str() );
				UFC::BufferedLog::Printf( " Call ReqUserPasswordUpdate: RequestID[%d] UserID[%s] BrokerID[%s]"
										  , KStrdRequestId, FID.c_str(), FCFFEXBrokerID.c_str() );
				return FKSTRDAPI->ReqUserPasswordUpdate( &UserPasswordUpdate, KStrdRequestId );
			}
			else if ( Type == "Bank" && FBankAccount.Length() > 0 )
			{
				KS_::CThostFtdcTradingAccountPasswordUpdateField TradingAccountPasswordUpdate;
				memset( &TradingAccountPasswordUpdate, 0, sizeof(KS_::CThostFtdcTradingAccountPasswordUpdateField) );
				strcpy( TradingAccountPasswordUpdate.BrokerID,    FCFFEXBrokerID.c_str() );
				strcpy( TradingAccountPasswordUpdate.AccountID,   FBankAccount.c_str() );
				strcpy( TradingAccountPasswordUpdate.OldPassword, OldPassword.c_str() );
				strcpy( TradingAccountPasswordUpdate.NewPassword, NewPassword.c_str() );
				// TradingAccountPasswordUpdate.CurrencyID not require field
				UFC::BufferedLog::Printf( " Call ReqTradingAccountPasswordUpdate: RequestID[%d] UserID[%s] BrokerID[%s]"
										  , KStrdRequestId, FID.c_str(), FCFFEXBrokerID.c_str() );
				return FKSTRDAPI->ReqTradingAccountPasswordUpdate( &TradingAccountPasswordUpdate, KStrdRequestId );
			}
		}
	}
	else if ( NewsID == CTP_QueryTradingAccount ) ///< Query Trading Account
	{
		KS_::CThostFtdcQryTradingAccountField QryTradingAccount;
		memset( &QryTradingAccount, 0, sizeof(KS_::CThostFtdcQryTradingAccountField) );
		UFC::NameValueMessage Params("^\n");
		UFC::AnsiString ansiBrokerId, ansiInvestorId;

		Params.FromString( Msg->GetText() );
		if ( ( Params.Get("BrokerId", ansiBrokerId) == FALSE ) || ( ansiBrokerId.Length() <= 0 ) )
			ansiBrokerId = FCFFEXBrokerID;
		if ( ( Params.Get("InvestorId", ansiInvestorId) == FALSE ) || ( ansiInvestorId.Length() <= 0 ) )
			ansiInvestorId = FID;
		strcpy( QryTradingAccount.BrokerID,   ansiBrokerId.c_str() );
		strcpy( QryTradingAccount.InvestorID, ansiInvestorId.c_str() );
		// QryTradingAccount.CurrencyID not require field
		UFC::BufferedLog::Printf( " Call ReqQryTradingAccount: RequestID[%d] UID[%d] BrokerID[%s] InvestorID[%s]"
								  , KStrdRequestId, Msg->GetID(), ansiBrokerId.c_str(), ansiInvestorId.c_str() );
		return FKSTRDAPI->ReqQryTradingAccount( &QryTradingAccount, KStrdRequestId );
	}
	else if ( NewsID == CTP_QueryInstrumentCommissionRate ) ///< InstrumentCommissionRate.
	{
		KS_::CThostFtdcQryInstrumentCommissionRateField qryField;
		memset( &qryField, 0, sizeof(KS_::CThostFtdcQryInstrumentCommissionRateField) );
		UFC::NameValueMessage Params("^\n");
		Params.FromString( Msg->GetText() );
		UFC::AnsiString ansiBrokerId = ""; //經紀公司代碼
		if ( ( Params.Get("BrokerId", ansiBrokerId) == FALSE ) || ( ansiBrokerId.Length() <= 0 ) )
			ansiBrokerId = FCFFEXBrokerID;
		strcpy( qryField.BrokerID, ansiBrokerId.c_str() );

		UFC::AnsiString ansiInvestorId = ""; //投資者代碼
		if ( ( Params.Get("InvestorId", ansiInvestorId) == FALSE ) || ( ansiInvestorId.Length() <= 0 ) )
			ansiInvestorId = FID;
		strcpy( qryField.InvestorID, ansiInvestorId.c_str() );

		UFC::AnsiString ansiInstrumentId = ""; //合約代碼
		if ( ( Params.Get("InstrumentId", ansiInstrumentId) == TRUE ) && ( ansiInstrumentId.Length() > 0 ) )
			strcpy( qryField.InstrumentID, ansiInstrumentId.c_str() );

		UFC::BufferedLog::Printf( " Call ReqQryInstrumentCommissionRate: RequestID[%d] UID[%d] BrokerID[%s] InvestorID[%s] Instrument[%s]"
								  , KStrdRequestId, Msg->GetID(), qryField.BrokerID, qryField.InvestorID, qryField.InstrumentID );
		return FKSTRDAPI->ReqQryInstrumentCommissionRate( &qryField, KStrdRequestId );
	}
	else if ( NewsID == CTP_QuerySettlementInfo )
	{
		UFC::NameValueMessage Params("^\n");
		UFC::AnsiString TradingDate("");

		Params.FromString( Msg->GetText() );
		Params.Get( "TradingDay", TradingDate );
		KS_::CThostFtdcQrySettlementInfoField QuerySettlementData;
		memset( &QuerySettlementData, 0, sizeof(KS_::CThostFtdcQrySettlementInfoField) );
		strcpy( QuerySettlementData.TradingDay, TradingDate.c_str() );
		strcpy( QuerySettlementData.BrokerID,   FCFFEXBrokerID.c_str() );
		strcpy( QuerySettlementData.InvestorID, FID.c_str() );
		UFC::BufferedLog::Printf( " Call ReqQrySettlementInfo: RequestID[%d] UID[%d] BrokerID[%s] InvestorID[%s]"
								  , KStrdRequestId, Msg->GetID(), QuerySettlementData.BrokerID, QuerySettlementData.InvestorID );
		return FKSTRDAPI->ReqQrySettlementInfo( &QuerySettlementData, KStrdRequestId );
	}
	else if ( NewsID == CTP_QueryTransferSerial )
	{
		KS_::CThostFtdcQryTransferSerialField queryParameter;
		memset( &queryParameter, 0, sizeof(KS_::CThostFtdcQryTransferSerialField) );
		UFC::NameValueMessage Params("^\n");
		Params.FromString(Msg->GetText());
		UFC::AnsiString ansiBrokerId = ""; //經紀公司代碼
		if ( ( Params.Get("BrokerId", ansiBrokerId) == FALSE ) || ( ansiBrokerId.Length() <= 0 ) )
			ansiBrokerId = FCFFEXBrokerID;
		strcpy( queryParameter.BrokerID, ansiBrokerId.c_str() );
		UFC::AnsiString ansiAccountId = ""; //投資者帳號
		if ( ( Params.Get("AccountId", ansiAccountId) == TRUE ) && ( ansiAccountId.Length() > 0 ) )
		{
			strcpy( queryParameter.AccountID, ansiAccountId.c_str() );
			UFC::AnsiString ansiBankId = ""; //銀行編碼
			if ( ( Params.Get("BankId", ansiBankId) == TRUE ) && ( ansiBankId.Length() > 0 ) )
				strcpy( queryParameter.BankID, ansiBankId.c_str() );
			// queryParameter.CurrencyID not require field
			UFC::BufferedLog::Printf( " Call ReqQryTransferSerial: RequestID[%d] UID[%d] BrokerID[%s] AccountID[%s] BankID[%s]"
									  , KStrdRequestId, Msg->GetID(), ansiBrokerId.c_str(), ansiAccountId.c_str(), ansiBankId.c_str() );
			return FKSTRDAPI->ReqQryTransferSerial( &queryParameter, KStrdRequestId );
		}
	}
	else if ( NewsID == CTP_QueryTransferBank )
	{
	// Temp Mark
	/*
		KS_::CThostFtdcQryTransferBankField queryField;
		memset( &queryField, 0, sizeof(KS_::CThostFtdcQryTransferBankField) );
		UFC::NameValueMessage Params("^\n");
		Params.FromString( Msg->GetText() );
		UFC::AnsiString ansiBankId = ""; //銀行編碼
		UFC::AnsiString ansiBankBranchId = ""; //銀行分中心代碼
		if ( ( Params.Get("BankId", ansiBankId) == TRUE ) && ( ansiBankId.Length() > 0 ) )
		{
			strcpy( queryField.BankID, ansiBankId.c_str() );
			if ( ( Params.Get("BankBranchId", ansiBankBranchId) == TRUE ) && ( ansiBankBranchId.Length() > 0 ) )
				strcpy( queryField.BankBrchID, ansiBankBranchId.c_str() );
		}
		UFC::BufferedLog::Printf( " Call ReqQryTransferBank: RequestID[%d] UID[%d] BankID[%s] BankBranchID[%s]"
								  , KStrdRequestId, Msg->GetID(), ansiBankId.c_str(), ansiBankBranchId.c_str() );
		return FKSTRDAPI->ReqQryTransferBank( &queryField, KStrdRequestId );
	*/
		return 0;
	}
	else if ( NewsID == CTP_QueryContractBank )
	{
		KS_::CThostFtdcQryContractBankField queryField;
		memset( &queryField, 0, sizeof(KS_::CThostFtdcQryContractBankField) );
		UFC::NameValueMessage Params("^\n");
		Params.FromString( Msg->GetText() );
		UFC::AnsiString ansiBrokerId = ""; //經紀公司代碼
		if ( ( Params.Get("BrokerId", ansiBrokerId) == FALSE ) || ( ansiBrokerId.Length() <= 0 ) )
			ansiBrokerId = FCFFEXBrokerID;
		strcpy( queryField.BrokerID, ansiBrokerId.c_str() );
		UFC::AnsiString ansiBankId = ""; //銀行編碼
		UFC::AnsiString ansiBankBranchId = ""; //銀行分中心代碼
		if ( ( Params.Get("BankId", ansiBankId) == TRUE ) && ( ansiBankId.Length() > 0 ) )
		{
			strcpy( queryField.BankID, ansiBankId.c_str() );
			if ( ( Params.Get("BankBranchId", ansiBankBranchId) == TRUE ) && ( ansiBankBranchId.Length() > 0 ) )
				strcpy( queryField.BankBrchID, ansiBankBranchId.c_str() );
		}
		UFC::BufferedLog::Printf( " Call ReqQryContractBank: RequestID[%d] UID[%d] BrokerID[%s] BankID[%s] BankBranchID[%s]"
								  , KStrdRequestId, Msg->GetID(), ansiBrokerId.c_str(), ansiBankId.c_str(), ansiBankBranchId.c_str());
		return FKSTRDAPI->ReqQryContractBank( &queryField, KStrdRequestId );
	}
	else if ( NewsID == CTP_QueryAccountRegister )
	{
		KS_::CThostFtdcQryAccountregisterField queryField;
		memset( &queryField, 0, sizeof(KS_::CThostFtdcQryAccountregisterField) );
		UFC::NameValueMessage Params("^\n");
		Params.FromString( Msg->GetText() );
		UFC::AnsiString ansiBrokerId = ""; //經紀公司代碼
		if ( ( Params.Get("BrokerId", ansiBrokerId) == FALSE ) || ( ansiBrokerId.Length() <= 0 ) )
			ansiBrokerId = FCFFEXBrokerID;
		strcpy( queryField.BrokerID, ansiBrokerId.c_str() );
		UFC::AnsiString ansiInvestorId = ""; //投資者代碼
		if ( ( Params.Get("InvestorId", ansiInvestorId) == TRUE ) && ( ansiInvestorId.Length() > 0 ) )
			strcpy( queryField.AccountID, ansiInvestorId.c_str() );
		UFC::AnsiString ansiBankId = ""; //銀行代碼
		if ( ( Params.Get("BankId", ansiBankId) == TRUE ) && ( ansiBankId.Length() > 0 ) )
			strcpy( queryField.BankID, ansiBankId.c_str() );
		UFC::BufferedLog::Printf( " Call ReqQryAccountregister: RequestID[%d] UID[%d] BrokerID[%s] InvestorID[%s] BankID[%s]"
								  , KStrdRequestId, Msg->GetID(), ansiBrokerId.c_str(), ansiInvestorId.c_str(), ansiBankId.c_str());
		return FKSTRDAPI->ReqQryAccountregister( &queryField, KStrdRequestId );
	}
	else if ( NewsID == CTP_QueryTradingCode )
	{
		KS_::CThostFtdcQryTradingCodeField queryField;
		memset( &queryField, 0, sizeof(KS_::CThostFtdcQryTradingCodeField) );
		UFC::NameValueMessage Params("^\n");
		Params.FromString( Msg->GetText() );
		UFC::AnsiString ansiBrokerId = ""; //經紀公司代碼
		if ( ( Params.Get("BrokerId", ansiBrokerId) == FALSE ) || ( ansiBrokerId.Length() <= 0 ) )
			ansiBrokerId = FCFFEXBrokerID;
		strcpy( queryField.BrokerID, ansiBrokerId.c_str() );
		UFC::AnsiString ansiInvestorId = ""; //投資者代碼
		if ( ( Params.Get("InvestorId", ansiInvestorId) == TRUE ) && ( ansiInvestorId.Length() > 0 ) )
			strcpy( queryField.InvestorID, ansiInvestorId.c_str() );
		UFC::AnsiString ansiExchangeId = ""; //交易所代碼
		if ( ( Params.Get("ExchangeId", ansiExchangeId) == TRUE ) && ( ansiExchangeId.Length() > 0 ) )
			strcpy( queryField.ExchangeID, ansiExchangeId.c_str() );
		UFC::AnsiString ansiClientId = ""; //交易編碼
		if ( ( Params.Get("ClientId", ansiClientId) == TRUE ) && ( ansiClientId.Length() > 0 ) )
			strcpy( queryField.ClientID, ansiClientId.c_str() );
		UFC::BufferedLog::Printf( " Call ReqQryTradingCode: RequestID[%d] UID[%d] BrokerID[%s] InvestorID[%s] ExchangeId[%s] ClientId[%s]"
								  , KStrdRequestId, Msg->GetID(), ansiBrokerId.c_str(), ansiInvestorId.c_str(), ansiExchangeId.c_str(), ansiClientId.c_str() );
		return FKSTRDAPI->ReqQryTradingCode( &queryField, KStrdRequestId );
	}
	else if ( NewsID == CTP_QueryBankAccountMoneyByFuture )
	{
		KS_::CThostFtdcReqQueryAccountField queryField;
		memset( &queryField, 0, sizeof(KS_::CThostFtdcReqQueryAccountField) );
		UFC::NameValueMessage Params("^\n");
		Params.FromString( Msg->GetText() );
		UFC::AnsiString ansiTradeCode = ""; //業務功能代碼
		if ( ( Params.Get("TradeCode", ansiTradeCode) == TRUE ) && (ansiTradeCode.Length() > 0 ) )
			strcpy( queryField.TradeCode, ansiTradeCode.c_str() );
		UFC::AnsiString ansiBrokerId = ""; //經紀公司代碼
		if ( ( Params.Get("BrokerId", ansiBrokerId) == FALSE ) || ( ansiBrokerId.Length() <= 0 ) )
			ansiBrokerId = FCFFEXBrokerID;
		strcpy( queryField.BrokerID, ansiBrokerId.c_str() );
		UFC::AnsiString ansiBankId = ""; //銀行代碼
		if ( ( Params.Get("BankId", ansiBankId) == TRUE ) && ( ansiBankId.Length() > 0 ) )
			strcpy( queryField.BankID, ansiBankId.c_str() );
		UFC::AnsiString ansiBankPassword = ""; //銀行帳戶密碼
		if ( ( Params.Get("BankPassword", ansiBankPassword) == TRUE ) && ( ansiBankPassword.Length() > 0 ) )
			strcpy( queryField.BankPassWord, ansiBankPassword.c_str() );
		UFC::AnsiString ansiAccountId = ""; //投資者帳號
		if ( ( Params.Get("AccountId", ansiAccountId) == TRUE ) && ( ansiAccountId.Length() > 0 ) )
			strcpy( queryField.AccountID, ansiAccountId.c_str() );
		UFC::AnsiString ansiAccountPassword = ""; //資金帳戶密碼
		if ( ( Params.Get("AccountPassword", ansiAccountPassword) == TRUE ) && ( ansiAccountPassword.Length() > 0 ) )
			strcpy( queryField.Password, ansiAccountPassword.c_str() );
		UFC::AnsiString ansiBankBranchId = ""; //銀行分支機構代碼
		if ( Params.Get("BankBranchId", ansiBankBranchId) == FALSE )
			ansiBankBranchId = "0000";
		if ( ansiBankBranchId.Length() <= 0 )
			ansiBankBranchId = "0000";
		strcpy( queryField.BankBranchID, ansiBankBranchId.c_str() );
		UFC::AnsiString ansiSecurityPasswordFlag; //資金帳戶密碼核對標誌
		if ( ( Params.Get("SecurityPasswordFlag", ansiSecurityPasswordFlag) == TRUE ) && ( ansiSecurityPasswordFlag.Length() > 0 ) )
		{
			if ( ansiSecurityPasswordFlag == "0" )
				queryField.SecuPwdFlag = '0'; // THOST_FTDC_BPWDF_NoCheck 不核對
			else if ( ansiSecurityPasswordFlag == "1" )
				queryField.SecuPwdFlag = '1'; // THOST_FTDC_BPWDF_BlankCheck 明文核對
			else if ( ansiSecurityPasswordFlag == "2" )
				queryField.SecuPwdFlag = '2'; // THOST_FTDC_BPWDF_EncryptCheck 密文核對
			else
				queryField.SecuPwdFlag = THOST_FTDC_BPWDF_BlankCheck; //'1' 明文核對
		}
		else
			queryField.SecuPwdFlag = THOST_FTDC_BPWDF_BlankCheck; //'1' 明文核對
		queryField.BankPwdFlag = THOST_FTDC_BPWDF_NoCheck; //'0' 銀行密碼標誌 '0':不核對 '1':明文核對 '2':密文核對
		queryField.VerifyCertNoFlag = THOST_FTDC_YNI_No; //'1'驗證客戶證件號碼標誌 '0':是 '1':否
		UFC::BufferedLog::Printf( " Call ReqQueryBankAccountMoneyByFuture: RequestID[%d] UID[%d] TradeCode[%s] ansiBankId[%s] BrokerID[%s] AccountId[%s]"
								  , KStrdRequestId, Msg->GetID(), ansiTradeCode.c_str(), ansiBankId.c_str(), ansiBrokerId.c_str(), ansiAccountId.c_str());
		return FKSTRDAPI->ReqQueryBankAccountMoneyByFuture( &queryField, KStrdRequestId );
	}
	else if ( NewsID == CTP_TransferFromBankToFutureByFuture )
	{
		KS_::CThostFtdcReqTransferField transferFields;
		KStrdSetTransferField( KStrdRequestId, Msg, transferFields );
		return FKSTRDAPI->ReqFromBankToFutureByFuture( &transferFields, KStrdRequestId );
	}
	else if ( NewsID == CTP_TransferFromFutureToBankByFuture )
	{
		KS_::CThostFtdcReqTransferField transferFields;
		KStrdSetTransferField( KStrdRequestId, Msg, transferFields );
		return FKSTRDAPI->ReqFromFutureToBankByFuture( &transferFields, KStrdRequestId );
	}
	else if ( NewsID == CTP_StockLockUnLockInsert )
	{
		KS_::CThostFtdcInputLockField InputLock;
		UFC::NameValueMessage Params("^\n");
		UFC::AnsiString ansiExchangeID = "";
		UFC::AnsiString ansiInstrumentId = "";
		UFC::AnsiString ansiVolume = "";
		UFC::AnsiString ansiLockType = "";
		AnsiString 		aLockType = "";
		UFC::AnsiString ansiOrderRef = "";
		int             Rtn = -1;
		char cTemp[256]; // 轉換用 Char Temp
		String sTemp;

		memset( &InputLock, 0, sizeof(InputLock) );
		Params.FromString( Msg->GetText() );

		// From User basic info.
		strcpy( InputLock.BrokerID,   FCFFEXBrokerID.c_str() ); 		// 經紀公司代碼
		strcpy( InputLock.InvestorID, FID.c_str() ); 					// 投資者代碼
		strcpy( InputLock.UserID,     FID.c_str() ); 					// 用戶代碼
		// From UI
		if ( ( Params.Get( "ExchangeID", ansiExchangeID ) == true ) && ( ansiExchangeID.Length() > 0 ) )
			strcpy( InputLock.ExchangeID, ansiExchangeID.c_str() ); 	// 交易所代碼
		if ( ( Params.Get( "InstrumentId", ansiInstrumentId ) == true ) && ( ansiInstrumentId.Length() > 0 ) )
			strcpy( InputLock.InstrumentID, ansiInstrumentId.c_str() ); // 合約代碼
		if ( ( Params.Get( "Volume", ansiVolume ) == true ) && ( ansiVolume.Length() > 0 ) )
		{
			strcpy( cTemp, ansiVolume.c_str() );   						// 數量
			sTemp = cTemp;
			InputLock.Volume = sTemp.ToInt();
		}
		if ( ( Params.Get( "LockType", ansiLockType ) == true ) && ( ansiLockType.Length() > 0 ) )
		{
			// 鎖定類型
			if ( ansiLockType == "1" )
			{
				InputLock.LockType = '1';								// 鎖定:THOST_FTDC_LCKT_Lock '1'
				aLockType = "Lock";
			}
			else if ( ansiLockType == "2" )
			{
				InputLock.LockType = '2';								// 解鎖:THOST_FTDC_LCKT_Unlock '2'
				aLockType = "UnLock";
			}
			else
			{
				aLockType = "Error LockType";
			}
		}
		if ( ( Params.Get( "OrderRef", ansiOrderRef ) == true ) && ( ansiOrderRef.Length() > 0 ) )
		{
			strcpy( cTemp, ansiOrderRef.c_str() ); 						// 鎖定引用
			sTemp = cTemp;
			int iOrderRef = sTemp.ToInt();
			sprintf( InputLock.LockRef, "%012d", iOrderRef );
		}

		InputLock.RequestID = KStrdRequestId;							// 請求編號
		//InputLock.BusinessUnit;										// 業務單元

		UFC::BufferedLog::Printf( " NewsID = CTP_StockLockUnLockInsert: BrokerID[%s] InvestorID[%s] UserID[%s]",
								  InputLock.BrokerID, InputLock.InvestorID, InputLock.UserID );
		UFC::BufferedLog::Printf( " NewsID = CTP_StockLockUnLockInsert: ExchangeID[%s] Symbol[%s] Volume[%d] LockRef[%s] LockType[%s]",
								  InputLock.ExchangeID, InputLock.InstrumentID, InputLock.Volume, InputLock.LockRef, aLockType.c_str() );

		if ( ( Rtn = FKSTRDAPI->ReqLockInsert( &InputLock, KStrdRequestId ) ) != 0 )
		{
			UFC::BufferedLog::Printf( " ----------------------- Send FKSTRDAPI->ReqLockInsert[%s] failed:%d ----------------------- ", aLockType.c_str(), Rtn );
			UFC::BufferedLog::Printf( "Call FKSTRDAPI->ReqLockInsert return error, Code[%d]", Rtn );
		}
		else
		{
			UFC::BufferedLog::Printf( " ----------------------- Send FKSTRDAPI->ReqLockInsert[%s] OK ----------------------- ", aLockType.c_str() );
		}

		return Rtn;
	}

	return -1;
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
