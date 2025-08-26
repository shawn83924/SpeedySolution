//---------------------------------------------------------------------------
#ifdef _CTP
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#include "CTPConnectionImp.h"
//------------------------------------------------------------------------------
//
// Function CTPNewOrder
// call by Speedy API NewOrder function.( TTaifexConnection->NewOrder )
//
//------------------------------------------------------------------------------
void TTaifexConnection::CTPNewOrder( TNewOrderMessage* Msg, const UFC::AnsiString& UserDefine )
{
	CThostFtdcInputOrderField ord;
	UFC::AnsiString ErrMsg;
	bool            CanSend = true;
	int             OrderRef = COrderInfo::ToOrderRef( Msg->GetNID() );
	int             Rtn;

	UFC::BufferedLog::Printf( " -------------------- User Place New Order -----------------------" );
	memset(&ord, 0, sizeof(ord));
	strcpy(ord.BrokerID,     FCFFEXBrokerID.c_str() );///< Broker ID
	strcpy(ord.InvestorID,   Msg->GetAccount() );///< Account
	strcpy(ord.UserID,       Msg->GetAccount() ); ///< Account. could be AE.
	strcpy(ord.InstrumentID, Msg->GetSymbol() ); ///< Symbol
	///< Side (B/S)
	if( Msg->GetSide() == nsOrderMessageDefine::sBuy )
		ord.Direction = THOST_FTDC_D_Buy;
	else
		ord.Direction = THOST_FTDC_D_Sell;
	///< Order Type (Market or Limit)
	if( Msg->GetOrderType() == nsOrderMessageDefine::otMarket )
	{
		ord.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
		ord.LimitPrice     = 0.0;
	}
	else
	{
		ord.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		ord.LimitPrice     = Msg->GetPrice();
	}
	ord.VolumeTotalOriginal = Msg->GetOrderQty(); ///< Order Qty
	switch( Msg->GetTimeInForce() ) ///< Time in force.
	{
		case nsOrderMessageDefine::tifROD:  ord.TimeCondition = THOST_FTDC_TC_GFD; break;
		case nsOrderMessageDefine::tifIOC:  ord.TimeCondition = THOST_FTDC_TC_IOC; break;
		case nsOrderMessageDefine::tifFOK:  ord.TimeCondition = THOST_FTDC_TC_GFD;; break;
		default: ord.TimeCondition = THOST_FTDC_TC_GFD; break;
	}
	///< Position effect.
	if( Msg->GetPositionEffect() == nsOrderMessageDefine::peOpen )
		ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open;//Open
	else if( Msg->GetPositionEffect() == nsOrderMessageDefine::peClose )
		ord.CombOffsetFlag[0] = THOST_FTDC_OF_Close;//Close
	else if( Msg->GetPositionEffect() == nsOrderMessageDefine::peRolled )
		ord.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;//Close Today
	else if( Msg->GetPositionEffect() == nsOrderMessageDefine::peAutoToday )
		ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open; //Auto Today
	else
		ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open;//Auto
	///< Fixed value field
	strcpy(ord.CombHedgeFlag, "1"); // 组合投机套保标志
	strcpy(ord.GTDDate, "");        // GTD日期
	ord.VolumeCondition = THOST_FTDC_VC_AV; // 成交量类型
	ord.MinVolume = 1; // 最小成交量
	ord.ContingentCondition = THOST_FTDC_CC_Immediately; // 触发条件
	ord.StopPrice = 0; // 止损价
	ord.ForceCloseReason = THOST_FTDC_FCC_NotForceClose; // 强平原因
	ord.IsAutoSuspend = 0; // 自动挂起标志
	///< Order Sequence.
	sprintf( ord.OrderRef,"%012d", OrderRef ); ///报单引用
	///< Allow to place New order ?
	if( FSendEventListener != NULL )
	{
		FSendEventListener->OnBeforeSend( Msg->GetMarket(), smtNew, Msg, CanSend, ErrMsg );
		if( CanSend == false )
		{
			Reject( nsOrderMessageDefine::crrNew, ErrMsg, Msg, Msg->GetTradingSessionID() );
			return;
		}
	}
	UFC::AnsiString PE;

	if( ord.CombOffsetFlag[0] == THOST_FTDC_OF_Open )
		PE = "Open";
	else if( ord.CombOffsetFlag[0] == THOST_FTDC_OF_Close )
		PE = "Close";
	else if( ord.CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday )
		PE = "CloseToday";
	UFC::BufferedLog::Printf( " OrderRef[%d] Account[%s] Symbol[%s] Side[%c]",
							  OrderRef, ord.UserID, ord.InstrumentID,
							  ord.Direction == THOST_FTDC_D_Buy? 'B':'S' );
	UFC::BufferedLog::Printf( " Qty[%d] Px[%0.3f] TIF[%s] OT[%s] PE[%s]",
							  ord.VolumeTotalOriginal, ord.LimitPrice,
							  ord.TimeCondition == THOST_FTDC_TC_GFD? "ROD":"IOC",
							  ord.OrderPriceType == THOST_FTDC_OPT_AnyPrice? "Market" : "Limit", PE.c_str());
	if( (Rtn = FCTPAPI->ReqOrderInsert(  &ord, CTPReqID() )) != 0 )
	{
		UFC::BufferedLog::Printf( " ----------------------- Send failed:%d ------------------------", Rtn );
		ErrMsg.Printf( "Call ReqOrderInsert return error, Code[%d]", Rtn );
		Reject( nsOrderMessageDefine::crrNew, ErrMsg, Msg ,Msg->GetTradingSessionID() );
	}
	else ///< OK.
	{
		UFC::BufferedLog::Printf( " ---------------------- Send New Order OK ------------------------" );
		AddPendingOrder( OrderRef, ord, UserDefine );
		TradeLogNew( FCTPFrontID, FCTPSessionID, ord, UserDefine );
	}
}
//------------------------------------------------------------------------------
//
// Function CTPCancelOrder
// call by Speedy API CancelOrder function.( TTaifexConnection->CancelOrder )
//
//------------------------------------------------------------------------------
void TTaifexConnection::CTPCancelOrder( TCancelOrderMessage* Msg, const UFC::AnsiString& UserDefine )
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

	UFC::BufferedLog::Printf( " ------------------ User Place Cancel Order ----------------------" );
	///< Check cancel key.
	if( (OrderID.Length() == 0 || ExchangeID.Length() == 0 ) && OrgOrderRef  == 0 )
	{
		ErrMsg.Printf( "Missing OrderID, ExchangeID or OrderActionRef." );
		Reject( nsOrderMessageDefine::crrCancel, ErrMsg, Msg ,Msg->GetTradingSessionID());
		UFC::BufferedLog::Printf( " Missing OrderID, ExchangeID or OrderActionRef." );
		UFC::BufferedLog::Printf( " --------------------- Reject Order Cancel -----------------------" );
		return;
	}
	///< Find the original order.
	if( (DelOrd = FindOrderByOrderSysID( ExchangeID, OrderIDInt64 )) != NULL )
		UFC::BufferedLog::Printf( " Cancel by OrderSysID[%s]", OrderID.c_str()  );
	else if( (DelOrd = FindOrderByOrderLocalID( ExchangeID, OrderIDInt64 )) != NULL )
		UFC::BufferedLog::Printf( " Cancel by OrderLocalID[%s]", OrderID.c_str()  );
	else if( (DelOrd = FindOrderByOrderRef( FCTPFrontID, FCTPSessionID, OrgOrderRef )) != NULL )
		UFC::BufferedLog::Printf( " Cancel by FrontID[%d] SessionID[%d] OrderRef[%d]", FCTPFrontID, FCTPSessionID,OrgOrderRef );
	else
	{
		ErrMsg.Printf( "Order not found! Exchange[%s] OrderLocalID[%d]", ExchangeID.c_str(), atoi( Msg->GetOrderID()) );
		Reject( nsOrderMessageDefine::crrCancel, ErrMsg, Msg ,Msg->GetTradingSessionID());
		UFC::BufferedLog::Printf( " Cancel Order not found! Exchange[%s] OrderID[%s] CNID[%d]", ExchangeID.c_str(), OrderID.c_str(), OrgOrderRef );
		UFC::BufferedLog::Printf( " --------------------- Reject Order Cancel -----------------------" );
		return;
	}
	///< Pass all check, send Order Cancel to CTP.
	CThostFtdcInputOrderActionField CancelOrder;
	UFC::AnsiString                 OrderSysID( DelOrd->GetOrderSysID() );

	if( OrderSysID.Length() < 12 )            ///< CZCE OrderSysID Format: 9(16)
		OrderSysID.PadThis( 12, ' ', false ); ///< CTP OrderSysID Format: space(6) + 9(6)

	DelOrd->Cancel( UserDefine, CancelOrderRef );
	memset(&CancelOrder, 0, sizeof(CancelOrder));
	CancelOrder.LimitPrice     = 0.0;
	CancelOrder.VolumeChange   = 0;
	CancelOrder.ActionFlag     = THOST_FTDC_AF_Delete;
	CancelOrder.FrontID        = DelOrd->GetFrontID();
	CancelOrder.SessionID      = DelOrd->GetSessionID();
	CancelOrder.OrderActionRef = DelOrd->GetOrderRef();
	strcpy( CancelOrder.InvestorID,    Msg->GetAccount());
	strcpy( CancelOrder.InstrumentID,  Msg->GetSymbol());
	strcpy( CancelOrder.BrokerID,      DelOrd->GetBrokerID());
	strcpy( CancelOrder.UserID,        DelOrd->GetUserID());
	strcpy( CancelOrder.ExchangeID,    DelOrd->GetExchangeID());
	strcpy( CancelOrder.OrderSysID,    OrderSysID.c_str() );
	//sprintf( CancelOrder.OrderRef,     "%012d", CancelOrderRef ); ///< for KingNew

	///< Allow to place cancel order ?
	if( FSendEventListener != NULL )
	{
		FSendEventListener->OnBeforeSend( Msg->GetMarket(), smtCancel, Msg, CanSend, ErrMsg );
		if( CanSend == false )
		{
			Reject( nsOrderMessageDefine::crrCancel, ErrMsg, Msg ,Msg->GetTradingSessionID());
			return;
		}
	}
	UFC::BufferedLog::Printf( " CancelOrderRef[%d] Account[%s] ExchangeID[%s] Symbol[%s]",
							  CancelOrderRef, CancelOrder.InvestorID, CancelOrder.ExchangeID, CancelOrder.InstrumentID );
	UFC::BufferedLog::Printf( " FrontID[%d] SessionID[%d] OrderActionRef[%d] OrderSysID[%s]",
							  CancelOrder.FrontID,CancelOrder.SessionID,CancelOrder.OrderActionRef,CancelOrder.OrderSysID );
	if( (Rtn = FCTPAPI->ReqOrderAction(  &CancelOrder, CTPReqID() )) != 0 )
	{
		UFC::BufferedLog::Printf( " ----------------------- Send failed:%d ------------------------", Rtn );
		ErrMsg.Printf( "Call ReqOrderAction return error, Code[%d]", Rtn );
		Reject( nsOrderMessageDefine::crrCancel, ErrMsg, Msg ,Msg->GetTradingSessionID());
	}
	else
	{
		UFC::BufferedLog::Printf( " -------------------- Send Cancel Order OK -----------------------" );
		TradeLogCancel( OrderID.ToInt(), CancelOrder );
	}
}
//---------------------------------------------------------------------------
void TTaifexConnection::CTPFillTransferField(int CTPRequestId, TNewsMessage* Msg, CThostFtdcReqTransferField& TransferFields)
{
	memset(&TransferFields, 0, sizeof(CThostFtdcReqTransferField));
	UFC::NameValueMessage parameters("^\n");
	parameters.FromString(Msg->GetText());

	UFC::AnsiString ansiTradeCode = "";  //業務功能碼
	if ((parameters.Get("TradeCode", ansiTradeCode) == TRUE) && (ansiTradeCode.Length() > 0))
		strcpy(TransferFields.TradeCode, ansiTradeCode.c_str());

	UFC::AnsiString ansiBankId = "";  //銀行代碼
	if ((parameters.Get("BankId", ansiBankId) == TRUE) && (ansiBankId.Length() > 0))
		strcpy(TransferFields.BankID, ansiBankId.c_str());

	UFC::AnsiString ansiBankBranchId = "";  //銀行分支機構代碼
	if (parameters.Get("BankBranchId", ansiBankBranchId) == TRUE)
		if (ansiBankBranchId.Length() <= 0) ansiBankBranchId = "0000";
	strcpy(TransferFields.BankBranchID, ansiBankBranchId.c_str());

	UFC::AnsiString ansiBrokerId = "";  //期貨公司代碼
	if ((parameters.Get("BrokerId", ansiBrokerId) == FALSE) || (ansiBrokerId.Length() <= 0))
		ansiBrokerId = FCFFEXBrokerID;
	strcpy(TransferFields.BrokerID, ansiBrokerId.c_str());

	UFC::AnsiString ansiBankPassword = "";  //銀行帳戶密碼
	if ((parameters.Get("BankPassword", ansiBankPassword) == TRUE) && (ansiBankPassword.Length() > 0))
		strcpy(TransferFields.BankPassWord, ansiBankPassword.c_str());

	UFC::AnsiString ansiAccountId = "";  //投資者帳號
	if ((parameters.Get("AccountId", ansiAccountId) == TRUE) && (ansiAccountId.Length() > 0))
		strcpy(TransferFields.AccountID, ansiAccountId.c_str());

	UFC::AnsiString ansiAccountPassword = "";//資金帳戶密碼
	if ((parameters.Get("AccountPassword", ansiAccountPassword) == TRUE) && (ansiAccountPassword.Length() > 0))
		strcpy(TransferFields.Password, ansiAccountPassword.c_str());

	TransferFields.SecuPwdFlag = '1';
	UFC::AnsiString ansiSecurityPasswordFlag;  //資金帳戶密碼核對標誌
	if ((parameters.Get("SecurityPasswordFlag", ansiSecurityPasswordFlag) == TRUE) && (ansiSecurityPasswordFlag.Length() > 0))
	{
		if (ansiSecurityPasswordFlag == "0")
			TransferFields.SecuPwdFlag = '0';  //THOST_FTDC_BPWDF_NoCheck 不核對
		else if (ansiSecurityPasswordFlag == "1")
			TransferFields.SecuPwdFlag = '1';  //THOST_FTDC_BPWDF_BlankCheck 明文核對
		else if (ansiSecurityPasswordFlag == "2")
			TransferFields.SecuPwdFlag = '2';  //THOST_FTDC_BPWDF_EncryptCheck 密文核對
		else
			TransferFields.SecuPwdFlag = '1';
	}

	TransferFields.BankPwdFlag = '0';  //銀行密碼標誌 '0':不核對 '1':明文核對 '2':密文核對
	TransferFields.VerifyCertNoFlag = '1';  //驗證客戶證件號碼標誌 '0':THOST_FTDC_YNI_Yes是 '1':THOST_FTDC_YNI_No否
	UFC::AnsiString ansiCurrencyId = "";  //幣種
	if ((parameters.Get("CurrencyId", ansiCurrencyId) == TRUE) && (ansiCurrencyId.Length() > 0))
		strcpy(TransferFields.CurrencyID, ansiCurrencyId.c_str());

	double tradeAmount = 0.0;  //轉帳金額
	UFC::AnsiString ansiTradeAmountStr = L"";
	if ((parameters.Get("TradeAmount", ansiTradeAmountStr) == TRUE) && (ansiTradeAmountStr.Length() > 0))
	{
		tradeAmount = ansiTradeAmountStr.ToDouble();
		TransferFields.TradeAmount = tradeAmount;
	}

	UFC::AnsiString ansiFunctionName = "";
	if (ansiTradeCode == "202001")
		ansiFunctionName = "ReqFromBankToFutureByFuture";
	else if (ansiTradeCode == "202002")
		ansiFunctionName = "ReqFromFutureToBankByFuture";

	UFC::BufferedLog::Printf( " Call %s: RequestID[%d] TradeCode[%s] BrokerID[%s] BankID[%s] InvestorID[%s] Amount[%13.2lf]%s", ansiFunctionName.c_str(), CTPRequestId, ansiTradeCode.c_str(), ansiBrokerId.c_str(), ansiBankId.c_str(), ansiAccountId.c_str(), tradeAmount, ansiCurrencyId.c_str());
}  //TTaifexConnection::CTPFillTransferField()
//---------------------------------------------------------------------------
//
// Function HandleCTPMessage
// Handle any request from User.( TTaifexConnection->SendNewsRequest )
//
// User : call TTaifexConnection->SendNewsRequest
// This func: call CTP API -> ReqXXXX
//
//---------------------------------------------------------------------------
int TTaifexConnection::HandleCTPMessage( TNewsMessage* Msg )
{
	int ctpRequestId = CTPReqID();
	if( Msg->GetID() == CTP_QuerySettlementInfoConfirm ) ///< User confirm Settlement Info.
	{
		CThostFtdcQrySettlementInfoConfirmField QrySettlementInfoConfirm;
		///< Check  Settlement Info Confirm.
		memset( &QrySettlementInfoConfirm, 0, sizeof(QrySettlementInfoConfirm));
		strcpy(QrySettlementInfoConfirm.BrokerID,   FCFFEXBrokerID.c_str() );
		strcpy(QrySettlementInfoConfirm.InvestorID, FID.c_str() );
		UFC::BufferedLog::Printf( " Call ReqQrySettlementInfoConfirm: RequestID[%d] BrokerID[%s] InvestorID[%s]", ctpRequestId, FCFFEXBrokerID.c_str(), FID.c_str() );
		return FCTPAPI->ReqQrySettlementInfoConfirm( &QrySettlementInfoConfirm, ctpRequestId);
	}
	else if( Msg->GetID() == CTP_SettlementInfoConfirm ) ///< User confirm Settlement Info.
	{
		TDateTime Now = TDateTime::CurrentDateTime();
		unsigned short YY,MM,DD,hh,mm,ss,ms;
		AnsiString CurDateStr;
		AnsiString CurTimeStr;

		Now.DecodeDate( &YY,&MM,&DD );
		Now.DecodeTime( &hh,&mm,&ss,&ms  );
		CurDateStr.printf("%04d%02d%02d",YY,MM,DD );
		CurTimeStr.printf("%02d:%02d:%02d", hh,mm,ss );
		CThostFtdcSettlementInfoConfirmField SettlementConfirmData;

		memset(&SettlementConfirmData, 0, sizeof(SettlementConfirmData));
		strcpy(SettlementConfirmData.BrokerID,    FCFFEXBrokerID.c_str());
		strcpy(SettlementConfirmData.InvestorID,  FID.c_str());
		strcpy(SettlementConfirmData.ConfirmDate, CurDateStr.c_str());
		strcpy(SettlementConfirmData.ConfirmTime, CurTimeStr.c_str());
		UFC::BufferedLog::Printf( " Call ReqSettlementInfoConfirm: RequestID[%d] BrokerID[%s] InvestorID[%s] Date[%s][%s]", ctpRequestId, FCFFEXBrokerID.c_str(), FID.c_str(), CurDateStr.c_str(),CurTimeStr.c_str() );
		return FCTPAPI->ReqSettlementInfoConfirm( &SettlementConfirmData, ctpRequestId );
	}
	else if( Msg->GetID() == CTP_QueryInvestorPosition ) ///< InvestorPositionData.
	{
		CThostFtdcQryInvestorPositionField qryInvestorPositionData;
		memset(&qryInvestorPositionData, 0, sizeof(CThostFtdcQryInvestorPositionField));
		UFC::NameValueMessage parameters("^\n");
		parameters.FromString(Msg->GetText());
		UFC::AnsiString ansiBrokerId = "";  //經紀公司代碼
		if ((parameters.Get("BrokerId", ansiBrokerId) == FALSE) || (ansiBrokerId.Length() <= 0))
			ansiBrokerId = FCFFEXBrokerID;
		strcpy(qryInvestorPositionData.BrokerID, ansiBrokerId.c_str());

		UFC::AnsiString ansiInvestorId = "";  //投資者代碼
		if ((parameters.Get("InvestorId", ansiInvestorId) == FALSE) || (ansiInvestorId.Length() <= 0))
			ansiInvestorId = FID;
		strcpy(qryInvestorPositionData.InvestorID, ansiInvestorId.c_str());

		UFC::AnsiString ansiInstrumentId = "";  //合約代碼
		if ((parameters.Get("InstrumentId", ansiInstrumentId) == TRUE) && (ansiInstrumentId.Length() > 0))
			strcpy(qryInvestorPositionData.InstrumentID, ansiInstrumentId.c_str());

		UFC::BufferedLog::Printf(" Call ReqQryInvestorPosition: RequestID[%d] UID[%d] BrokerID[%s] InvestorID[%s] InstrumentID[%s]", ctpRequestId, Msg->GetID(), ansiBrokerId.c_str(), ansiInvestorId.c_str(), ansiInstrumentId.c_str());
		return FCTPAPI->ReqQryInvestorPosition(&qryInvestorPositionData, ctpRequestId);
	}
	else if( Msg->GetID() == CTP_QueryInvestorPositionDetail ) ///< InvestorPositionDetailData.
	{
		CThostFtdcQryInvestorPositionDetailField qryInvestorPositionDetailData;
		memset(&qryInvestorPositionDetailData, 0, sizeof(CThostFtdcQryInvestorPositionDetailField));
		strcpy(qryInvestorPositionDetailData.BrokerID, FCFFEXBrokerID.c_str());
		strcpy(qryInvestorPositionDetailData.InvestorID, FID.c_str());
		UFC::BufferedLog::Printf(" Call ReqQryInvestorPositionDetail: RequestID[%d] UID[%d] BrokerID[%s] InvestorID[%s]", ctpRequestId, Msg->GetID(), qryInvestorPositionDetailData.BrokerID, qryInvestorPositionDetailData.InvestorID);
		return FCTPAPI->ReqQryInvestorPositionDetail(&qryInvestorPositionDetailData, ctpRequestId);
	}
	else if( Msg->GetID() == CTP_ChangePassword ) ///< Change Password.
	{
		UFC::NameValueMessage Params("^\n");
		UFC::AnsiString OldPassword, NewPassword, Type;

		Params.FromString( Msg->GetText() );
		if( Params.Get( "OldPassword", OldPassword )== true  &&
			Params.Get( "NewPassword", NewPassword )== true  &&
			Params.Get( "Type", Type )== true )
		{
			if( Type == "Trade" )
			{
				CThostFtdcUserPasswordUpdateField UserPasswordUpdate;

				memset(&UserPasswordUpdate, 0, sizeof(UserPasswordUpdate));
				strcpy(UserPasswordUpdate.BrokerID,    FCFFEXBrokerID.c_str());
				strcpy(UserPasswordUpdate.UserID,      FID.c_str());
				strcpy(UserPasswordUpdate.OldPassword, OldPassword.c_str());
				strcpy(UserPasswordUpdate.NewPassword, NewPassword.c_str());
				UFC::BufferedLog::Printf(" Call ReqUserPasswordUpdate: RequestID[%d] UserID[%d] BrokerID[%s]", ctpRequestId, FID.c_str(), FCFFEXBrokerID.c_str() );
				return FCTPAPI->ReqUserPasswordUpdate( &UserPasswordUpdate, ctpRequestId);
			}
			else if( Type == "Bank" && FBankAccount.Length() > 0  )
			{
				CThostFtdcTradingAccountPasswordUpdateField TradingAccountPasswordUpdate;

				memset(&TradingAccountPasswordUpdate, 0, sizeof(TradingAccountPasswordUpdate));
				strcpy(TradingAccountPasswordUpdate.BrokerID,    FCFFEXBrokerID.c_str());
				strcpy(TradingAccountPasswordUpdate.AccountID,   FBankAccount.c_str());
				strcpy(TradingAccountPasswordUpdate.OldPassword, OldPassword.c_str());
				strcpy(TradingAccountPasswordUpdate.NewPassword, NewPassword.c_str());
				UFC::BufferedLog::Printf(" Call ReqTradingAccountPasswordUpdate: RequestID[%d] UserID[%d] BrokerID[%s]", ctpRequestId, FID.c_str(), FCFFEXBrokerID.c_str() );
				return FCTPAPI->ReqTradingAccountPasswordUpdate( &TradingAccountPasswordUpdate, ctpRequestId );
			}
		}
	}
	else if( Msg->GetID() == CTP_QueryTradingAccount ) ///< Query Trading Account
	{
		CThostFtdcQryTradingAccountField QryTradingAccount;
		UFC::NameValueMessage parameters("^\n");
		UFC::AnsiString ansiBrokerId, ansiInvestorId;

		parameters.FromString(Msg->GetText());
		if ((parameters.Get("BrokerId", ansiBrokerId) == FALSE) || (ansiBrokerId.Length() <= 0))
			ansiBrokerId = FCFFEXBrokerID;
		if ((parameters.Get("InvestorId", ansiInvestorId) == FALSE) || (ansiInvestorId.Length() <= 0))
			ansiInvestorId = FID;
		memset( &QryTradingAccount, 0, sizeof(CThostFtdcQryTradingAccountField));
		strcpy( QryTradingAccount.BrokerID,   ansiBrokerId.c_str());
		strcpy( QryTradingAccount.InvestorID, ansiInvestorId.c_str());
		UFC::BufferedLog::Printf(" Call ReqQryTradingAccount: RequestID[%d] UID[%d] BrokerID[%s] InvestorID[%s]", ctpRequestId, Msg->GetID(), ansiBrokerId.c_str(), ansiInvestorId.c_str());
		UFC::BufferedLog::FlushToFile();
		return FCTPAPI->ReqQryTradingAccount( &QryTradingAccount, ctpRequestId);
	}
	else if( Msg->GetID() == CTP_QueryInstrumentCommissionRate ) ///< InstrumentCommissionRate.
	{
		CThostFtdcQryInstrumentCommissionRateField qryField;
		memset(&qryField, 0, sizeof(CThostFtdcQryInstrumentCommissionRateField));
		UFC::NameValueMessage parameters("^\n");
		parameters.FromString(Msg->GetText());
		UFC::AnsiString ansiBrokerId = "";  //經紀公司代碼
		if ((parameters.Get("BrokerId", ansiBrokerId) == FALSE) || (ansiBrokerId.Length() <= 0))
			ansiBrokerId = FCFFEXBrokerID;
		strcpy(qryField.BrokerID, ansiBrokerId.c_str());

		UFC::AnsiString ansiInvestorId = "";  //投資者代碼
		if ((parameters.Get("InvestorId", ansiInvestorId) == FALSE) || (ansiInvestorId.Length() <= 0))
			ansiInvestorId = FID;
		strcpy(qryField.InvestorID, ansiInvestorId.c_str());

		UFC::AnsiString ansiInstrumentId = "";  //合約代碼
		if ((parameters.Get("InstrumentId", ansiInstrumentId) == TRUE) && (ansiInstrumentId.Length() > 0))
			strcpy(qryField.InstrumentID, ansiInstrumentId.c_str());

		UFC::BufferedLog::Printf(" Call ReqQryInstrumentCommissionRate: RequestID[%d] UID[%d] BrokerID[%s] InvestorID[%s] Instrument[%s]", ctpRequestId, Msg->GetID(), qryField.BrokerID, qryField.InvestorID, qryField.InstrumentID);
		UFC::BufferedLog::FlushToFile();
		return FCTPAPI->ReqQryInstrumentCommissionRate(&qryField, ctpRequestId);
	}
	else if( Msg->GetID() == CTP_QuerySettlementInfo )
	{
		UFC::NameValueMessage Params("^\n");
		UFC::AnsiString TradingDate("");

		Params.FromString( Msg->GetText() );
		Params.Get( "TradingDay", TradingDate );
		CThostFtdcQrySettlementInfoField QuerySettlementData;

		memset( &QuerySettlementData, 0, sizeof(QuerySettlementData));
		strcpy(	QuerySettlementData.TradingDay, TradingDate.c_str());
		strcpy( QuerySettlementData.BrokerID,   FCFFEXBrokerID.c_str() );
		strcpy( QuerySettlementData.InvestorID, FID.c_str() );
		return FCTPAPI->ReqQrySettlementInfo( &QuerySettlementData, ctpRequestId );
	}
	else if( Msg->GetID() == CTP_QueryTransferSerial )
	{
		CThostFtdcQryTransferSerialField queryParameter;
		memset(&queryParameter, 0, sizeof(CThostFtdcQryTransferSerialField));
		UFC::NameValueMessage parameters("^\n");
		parameters.FromString(Msg->GetText());
		UFC::AnsiString ansiBrokerId = "";  //經紀公司代碼
		if ((parameters.Get("BrokerId", ansiBrokerId) == FALSE) || (ansiBrokerId.Length() <= 0))
			ansiBrokerId = FCFFEXBrokerID;
		strcpy(queryParameter.BrokerID, ansiBrokerId.c_str());
		UFC::AnsiString ansiAccountId = "";  //投資者帳號
		if (parameters.Get("AccountId", ansiAccountId) == TRUE)
		{
			if (ansiAccountId.Length() > 0)
			{
				strcpy(queryParameter.AccountID, ansiAccountId.c_str());
				UFC::AnsiString ansiBankId = "";  //銀行編碼
				if (parameters.Get("BankId", ansiBankId) == TRUE)
				{
					if (ansiBankId.Length() > 0) strcpy(queryParameter.BankID, ansiBankId.c_str());
				}
				UFC::BufferedLog::Printf(" Call ReqQryTransferSerial: RequestID[%d] UID[%d] BrokerID[%s] AccountID[%s] BankID[%s]", ctpRequestId, Msg->GetID(), ansiBrokerId.c_str(), ansiAccountId.c_str(), ansiBankId.c_str());
				UFC::BufferedLog::FlushToFile();
				return FCTPAPI->ReqQryTransferSerial(&queryParameter, ctpRequestId);
			}  //if (ansiAccountId.Length() > 0)
		}  //if (parameters.Get("AccountId", ansiAccountId) == TRUE)
	}
	else if( Msg->GetID() == CTP_QueryTransferBank )
	{
		CThostFtdcQryTransferBankField queryField;
		memset(&queryField, 0, sizeof(CThostFtdcQryTransferBankField));
		UFC::NameValueMessage parameters("^\n");
		parameters.FromString(Msg->GetText());
		UFC::AnsiString ansiBankId = "";  //銀行編碼
		UFC::AnsiString ansiBankBranchId = "";  //銀行分中心代碼
		if ((parameters.Get("BankId", ansiBankId) == TRUE) && (ansiBankId.Length() > 0))
		{
			strcpy(queryField.BankID, ansiBankId.c_str());
			if ((parameters.Get("BankBranchId", ansiBankBranchId) == TRUE) && (ansiBankBranchId.Length() > 0))
				strcpy(queryField.BankBrchID, ansiBankBranchId.c_str());
		}
		UFC::BufferedLog::Printf(" Call ReqQryTransferBank: RequestID[%d] UID[%d] BankID[%s] BankBranchID[%s]", ctpRequestId, Msg->GetID(), ansiBankId.c_str(), ansiBankBranchId.c_str());
		UFC::BufferedLog::FlushToFile();
		return FCTPAPI->ReqQryTransferBank(&queryField, ctpRequestId);
	}
	else if( Msg->GetID() == CTP_QueryContractBank )
	{
		CThostFtdcQryContractBankField queryField;
		memset(&queryField, 0, sizeof(CThostFtdcQryContractBankField));
		UFC::NameValueMessage parameters("^\n");
		parameters.FromString(Msg->GetText());
		UFC::AnsiString ansiBrokerId = "";  //經紀公司代碼
		if ((parameters.Get("BrokerId", ansiBrokerId) == FALSE) || (ansiBrokerId.Length() <= 0))
			ansiBrokerId = FCFFEXBrokerID;
		strcpy(queryField.BrokerID, ansiBrokerId.c_str());
		UFC::AnsiString ansiBankId = "";  //銀行編碼
		UFC::AnsiString ansiBankBranchId = "";  //銀行分中心代碼
		if ((parameters.Get("BankId", ansiBankId) == TRUE) && (ansiBankId.Length() > 0))
		{
			strcpy(queryField.BankID, ansiBankId.c_str());
			if ((parameters.Get("BankBranchId", ansiBankBranchId) == TRUE) && (ansiBankBranchId.Length() > 0))
				strcpy(queryField.BankBrchID, ansiBankBranchId.c_str());
		}
		UFC::BufferedLog::Printf(" Call ReqQryContractBank: RequestID[%d] UID[%d] BrokerID[%s] BankID[%s] BankBranchID[%s]", ctpRequestId, Msg->GetID(), ansiBrokerId.c_str(), ansiBankId.c_str(), ansiBankBranchId.c_str());
		UFC::BufferedLog::FlushToFile();
		return FCTPAPI->ReqQryContractBank(&queryField, ctpRequestId);
	}
	else if( Msg->GetID() == CTP_QueryAccountRegister )
	{
		CThostFtdcQryAccountregisterField queryField;
		memset(&queryField, 0, sizeof(CThostFtdcQryAccountregisterField));
		UFC::NameValueMessage parameters("^\n");
		parameters.FromString(Msg->GetText());
		UFC::AnsiString ansiBrokerId = "";  //經紀公司代碼
		if ((parameters.Get("BrokerId", ansiBrokerId) == FALSE) || (ansiBrokerId.Length() <= 0))
			ansiBrokerId = FCFFEXBrokerID;
		strcpy(queryField.BrokerID, ansiBrokerId.c_str());
		UFC::AnsiString ansiInvestorId = "";  //投資者代碼
		if ((parameters.Get("InvestorId", ansiInvestorId) == TRUE) && (ansiInvestorId.Length() > 0))
			strcpy(queryField.AccountID, ansiInvestorId.c_str());
		UFC::AnsiString ansiBankId = "";  //銀行代碼
		if ((parameters.Get("BankId", ansiBankId) == TRUE) && (ansiBankId.Length() > 0))
			strcpy(queryField.BankID, ansiBankId.c_str());
		UFC::BufferedLog::Printf(" Call ReqQryAccountregister: RequestID[%d] UID[%d] BrokerID[%s] InvestorID[%s] BankID[%s]", ctpRequestId, Msg->GetID(), ansiBrokerId.c_str(), ansiInvestorId.c_str(), ansiBankId.c_str());
		UFC::BufferedLog::FlushToFile();
		return FCTPAPI->ReqQryAccountregister(&queryField, ctpRequestId);
	}
	else if( Msg->GetID() == CTP_QueryTradingCode )
	{
		CThostFtdcQryTradingCodeField queryField;
		memset(&queryField, 0, sizeof(CThostFtdcQryTradingCodeField));
		UFC::NameValueMessage parameters("^\n");
		parameters.FromString(Msg->GetText());
		UFC::AnsiString ansiBrokerId = "";  //經紀公司代碼
		if ((parameters.Get("BrokerId", ansiBrokerId) == FALSE) || (ansiBrokerId.Length() <= 0))
			ansiBrokerId = FCFFEXBrokerID;
		strcpy(queryField.BrokerID, ansiBrokerId.c_str());
		UFC::AnsiString ansiInvestorId = "";  //投資者代碼
		if ((parameters.Get("InvestorId", ansiInvestorId) == TRUE) && (ansiInvestorId.Length() > 0))
			strcpy(queryField.InvestorID, ansiInvestorId.c_str());
		UFC::AnsiString ansiExchangeId = "";  //交易所代碼
		if ((parameters.Get("ExchangeId", ansiExchangeId) == TRUE) && (ansiExchangeId.Length() > 0))
			strcpy(queryField.ExchangeID, ansiExchangeId.c_str());
		UFC::AnsiString ansiClientId = "";  //交易編碼
		if ((parameters.Get("ClientId", ansiClientId) == TRUE) && (ansiClientId.Length() > 0))
			strcpy(queryField.ClientID, ansiClientId.c_str());
		UFC::BufferedLog::Printf(" Call ReqQryTradingCode: RequestID[%d] UID[%d] BrokerID[%s] InvestorID[%s] ExchangeId[%s] ClientId[%s]", ctpRequestId, Msg->GetID(), ansiBrokerId.c_str(), ansiInvestorId.c_str(), ansiExchangeId.c_str(), ansiClientId.c_str());
		UFC::BufferedLog::FlushToFile();
		return FCTPAPI->ReqQryTradingCode(&queryField, ctpRequestId);
	}
	else if( Msg->GetID() == CTP_QueryBankAccountMoneyByFuture )
	{
		CThostFtdcReqQueryAccountField queryField;
		memset(&queryField, 0, sizeof(CThostFtdcReqQueryAccountField));
		UFC::NameValueMessage parameters("^\n");
		parameters.FromString(Msg->GetText());
		UFC::AnsiString ansiTradeCode = "";  //業務功能代碼
		if ((parameters.Get("TradeCode", ansiTradeCode) == TRUE) && (ansiTradeCode.Length() > 0))
			strcpy(queryField.TradeCode, ansiTradeCode.c_str());
		UFC::AnsiString ansiBrokerId = "";  //經紀公司代碼
		if ((parameters.Get("BrokerId", ansiBrokerId) == FALSE) || (ansiBrokerId.Length() <= 0))
			ansiBrokerId = FCFFEXBrokerID;
		strcpy(queryField.BrokerID, ansiBrokerId.c_str());
		UFC::AnsiString ansiBankId = "";  //銀行代碼
		if ((parameters.Get("BankId", ansiBankId) == TRUE) && (ansiBankId.Length() > 0))
			strcpy(queryField.BankID, ansiBankId.c_str());
		UFC::AnsiString ansiBankPassword = "";  //銀行帳戶密碼
		if ((parameters.Get("BankPassword", ansiBankPassword) == TRUE) && (ansiBankPassword.Length() > 0))
			strcpy(queryField.BankPassWord, ansiBankPassword.c_str());
		UFC::AnsiString ansiAccountId = "";  //投資者帳號
		if ((parameters.Get("AccountId", ansiAccountId) == TRUE) && (ansiAccountId.Length() > 0))
			strcpy(queryField.AccountID, ansiAccountId.c_str());
		UFC::AnsiString ansiAccountPassword = "";  //資金帳戶密碼
		if ((parameters.Get("AccountPassword", ansiAccountPassword) == TRUE) && (ansiAccountPassword.Length() > 0))
			strcpy(queryField.Password, ansiAccountPassword.c_str());
		UFC::AnsiString ansiCurrencyId = "";  //幣種
		if ((parameters.Get("CurrencyId", ansiCurrencyId) == TRUE) && (ansiCurrencyId.Length() > 0))
			strcpy(queryField.CurrencyID, ansiCurrencyId.c_str());
		UFC::AnsiString ansiBankBranchId = "";  //銀行分支機構代碼
		if (parameters.Get("BankBranchId", ansiBankBranchId) == FALSE) ansiBankBranchId = "0000";
		if (ansiBankBranchId.Length() <= 0) ansiBankBranchId = "0000";
		strcpy(queryField.BankBranchID, ansiBankBranchId.c_str());
		UFC::AnsiString ansiSecurityPasswordFlag;  //資金帳戶密碼核對標誌
		if ((parameters.Get("SecurityPasswordFlag", ansiSecurityPasswordFlag) == TRUE) && (ansiSecurityPasswordFlag.Length() > 0))
		{
			if (ansiSecurityPasswordFlag == "0")
				queryField.SecuPwdFlag = '0';  //THOST_FTDC_BPWDF_NoCheck 不核對
			else if (ansiSecurityPasswordFlag == "1")
				queryField.SecuPwdFlag = '1';  //THOST_FTDC_BPWDF_BlankCheck 明文核對
			else if (ansiSecurityPasswordFlag == "2")
				queryField.SecuPwdFlag = '2';  //THOST_FTDC_BPWDF_EncryptCheck 密文核對
			else
				queryField.SecuPwdFlag = '1';
		}
		else
			queryField.SecuPwdFlag = '1';
		queryField.BankPwdFlag = '0';  //銀行密碼標誌 '0':不核對 '1':明文核對 '2':密文核對
		queryField.VerifyCertNoFlag = '1';  //驗證客戶證件號碼標誌 '0':THOST_FTDC_YNI_Yes是 '1':THOST_FTDC_YNI_No否
		UFC::BufferedLog::Printf(" Call ReqQueryBankAccountMoneyByFuture: RequestID[%d] UID[%d] TradeCode[%s] ansiBankId[%s] BrokerID[%s] AccountId[%s], Currency[%s]", ctpRequestId, Msg->GetID(), ansiTradeCode.c_str(), ansiBankId.c_str(), ansiBrokerId.c_str(), ansiAccountId.c_str(), ansiCurrencyId.c_str());
		UFC::BufferedLog::FlushToFile();
		return FCTPAPI->ReqQueryBankAccountMoneyByFuture(&queryField, ctpRequestId);
	}
	else if( Msg->GetID() == CTP_TransferFromBankToFutureByFuture )
	{
		CThostFtdcReqTransferField transferFields;
		CTPFillTransferField(ctpRequestId, Msg, transferFields);
		return FCTPAPI->ReqFromBankToFutureByFuture(&transferFields, ctpRequestId);
	}
	else if( Msg->GetID() == CTP_TransferFromFutureToBankByFuture )
	{
		CThostFtdcReqTransferField transferFields;
		CTPFillTransferField(ctpRequestId, Msg, transferFields);
		return FCTPAPI->ReqFromFutureToBankByFuture(&transferFields, ctpRequestId);
	}
	return -1;
}
//------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
