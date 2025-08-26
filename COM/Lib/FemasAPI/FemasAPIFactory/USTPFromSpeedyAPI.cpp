//---------------------------------------------------------------------------
#ifdef _CTP
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "CTPConnectionImp.h"
//------------------------------------------------------------------------------
//
// Function FemasNewOrder
// call by Speedy API NewOrder function.( TTaifexConnection->NewOrder )
//
//------------------------------------------------------------------------------
void TTaifexConnection::FemasNewOrder( TNewOrderMessage* Msg, const UFC::AnsiString& UserDefine )
{
	CUstpFtdcInputOrderField ord;
	UFC::AnsiString ErrMsg;
	bool            CanSend = true;
	int             OrderRef = COrderInfo::ToOrderRef( Msg->GetNID() );
	int             Rtn;

	UFC::BufferedLog::Printf( " -------------------- Femas Place New Order -----------------------" );
	memset(&ord, 0, sizeof(ord));
	strcpy(ord.BrokerID,     FCFFEXBrokerID.c_str() );///< Broker ID
	strcpy(ord.InvestorID,   FInvestorID.c_str() );///< Account
	strcpy(ord.UserID,       Msg->GetAccount() ); ///< Account. could be AE.
	strcpy(ord.InstrumentID, Msg->GetSymbol() ); ///< Symbol
	strcpy(ord.ExchangeID,   Msg->GetExchangeCode() );
	if( UserDefine.Length() > 65 )
		memcpy( ord.UserCustom, UserDefine.c_str(), 65 );
	else
		memcpy( ord.UserCustom, UserDefine.c_str(), UserDefine.Length()  );
	///< Side (B/S)
	if( Msg->GetSide() == nsOrderMessageDefine::sBuy )
		ord.Direction = USTP_FTDC_D_Buy;
	else
		ord.Direction = USTP_FTDC_D_Sell;
	///< Order Type (Market or Limit)
	if( Msg->GetOrderType() == nsOrderMessageDefine::otMarket )
	{
		ord.OrderPriceType = USTP_FTDC_OPT_AnyPrice;
		ord.LimitPrice     = 0.0;
	}
	else
	{
		ord.OrderPriceType = USTP_FTDC_OPT_LimitPrice;
		ord.LimitPrice     = Msg->GetPrice();
	}
	ord.Volume = Msg->GetOrderQty(); ///< Order Qty
	switch( Msg->GetTimeInForce() ) ///< Time in force.
	{
		case nsOrderMessageDefine::tifROD:  ord.TimeCondition = USTP_FTDC_TC_GFD; break;
		case nsOrderMessageDefine::tifIOC:  ord.TimeCondition = USTP_FTDC_TC_IOC; break;
		case nsOrderMessageDefine::tifFOK:  ord.TimeCondition = USTP_FTDC_TC_GFD;; break;
		default: ord.TimeCondition = USTP_FTDC_TC_GFD; break;
	}
	///< Position effect.
	if( Msg->GetPositionEffect() == nsOrderMessageDefine::peOpen )
		ord.OffsetFlag = USTP_FTDC_OF_Open;//Open
	else if( Msg->GetPositionEffect() == nsOrderMessageDefine::peClose )
		ord.OffsetFlag = USTP_FTDC_OF_Close;//Close
	else if( Msg->GetPositionEffect() == nsOrderMessageDefine::peRolled )
		ord.OffsetFlag = USTP_FTDC_OF_CloseToday;//Close Today
	else if( Msg->GetPositionEffect() == nsOrderMessageDefine::peAutoToday )
		ord.OffsetFlag = USTP_FTDC_OF_Open; //Auto Today
	else
		ord.OffsetFlag = USTP_FTDC_OF_Open;//Auto
	///< Fixed value field
	ord.HedgeFlag = USTP_FTDC_CHF_Speculation; // 组合投机套保标志
	strcpy(ord.GTDDate, "");        // GTD日期
	ord.VolumeCondition = USTP_FTDC_VC_AV; // 成交量类型
	ord.MinVolume = 1; // 最小成交量
	ord.StopPrice = 0; // 止损价
	ord.ForceCloseReason = USTP_FTDC_FCR_NotForceClose; // 强平原因
	ord.IsAutoSuspend = 0; // 自动挂起标志
	///< Order Sequence.
	sprintf( ord.UserOrderLocalID,"%012d", OrderRef ); ///报单引用
	///< Allow to place New order ?
	if( FSendEventListener != NULL )
	{
		FSendEventListener->OnBeforeSend( Msg->GetMarket(), smtNew, Msg, CanSend, ErrMsg );
		if( CanSend == false )
		{
			Reject( nsOrderMessageDefine::crrNew, ErrMsg, Msg,Msg->GetTradingSessionID() );
			return;
		}
	}
	UFC::AnsiString PE;

	if( ord.OffsetFlag == USTP_FTDC_OF_Open )
		PE = "Open";
	else if( ord.OffsetFlag == USTP_FTDC_OF_Close )
		PE = "Close";
	else if( ord.OffsetFlag == USTP_FTDC_OF_CloseToday )
		PE = "CloseToday";
	UFC::BufferedLog::Printf( " UserOrderLocalID[%d] Account[%s] Symbol[%s] Side[%c]",
							  OrderRef, ord.UserID, ord.InstrumentID,
							  ord.Direction == USTP_FTDC_D_Buy? 'B':'S' );
	UFC::BufferedLog::Printf( " Qty[%d] Px[%0.3f] TIF[%s] OT[%s] PE[%s]",
							  ord.Volume, ord.LimitPrice,
							  ord.TimeCondition == USTP_FTDC_TC_GFD? "ROD":"IOC",
							  ord.OrderPriceType == USTP_FTDC_OPT_AnyPrice? "Market" : "Limit", PE.c_str());
	if( (Rtn = FFEMASAPI->ReqOrderInsert( &ord, CTPReqID() )) != 0 )
	{
		UFC::BufferedLog::Printf( " ----------------------- Send failed:%d ------------------------", Rtn );
		ErrMsg.Printf( "Call ReqOrderInsert return error, Code[%d]", Rtn );
		Reject( nsOrderMessageDefine::crrNew, ErrMsg, Msg,Msg->GetTradingSessionID() );
	}
	else ///< OK.
	{
		UFC::BufferedLog::Printf( " ---------------------- Send New Order OK ------------------------" );
		AddPendingOrder( OrderRef, ord, UserDefine );
	}
}
//------------------------------------------------------------------------------
//
// Function FemasCancelOrder
// call by Speedy API CancelOrder function.( TTaifexConnection->CancelOrder )
//
//------------------------------------------------------------------------------
void TTaifexConnection::FemasCancelOrder( TCancelOrderMessage* Msg, const UFC::AnsiString& UserDefine )
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

	UFC::BufferedLog::Printf( " ------------------ Femas Place Cancel Order ----------------------" );
	///< Check cancel key.
	if( (OrderID.Length() == 0 || ExchangeID.Length() == 0 ) && OrgOrderRef  == 0 )
	{
		ErrMsg.Printf( "Missing OrderID, ExchangeID or OrderActionRef." );
		Reject( nsOrderMessageDefine::crrCancel, ErrMsg, Msg,Msg->GetTradingSessionID() );
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
		ErrMsg.Printf( "Order not found! Exchange[%s] OrderID[%s]", ExchangeID.c_str(), OrderID.c_str() );
		Reject( nsOrderMessageDefine::crrCancel, ErrMsg, Msg,Msg->GetTradingSessionID() );
		UFC::BufferedLog::Printf( " Cancel Order not found! Exchange[%s] OrderID[%s] CNID[%d]", ExchangeID.c_str(), OrderID.c_str(), OrgOrderRef );
		UFC::BufferedLog::Printf( " --------------------- Reject Order Cancel -----------------------" );
		return;
	}
	///< Pass all check, send Order Cancel to CTP.
	CUstpFtdcOrderActionField       CancelOrder;
	UFC::AnsiString                 OrderSysID( DelOrd->GetOrderSysID() );

	if( OrderSysID.Length() < 12 )            ///< CZCE OrderSysID Format: 9(16)
		OrderSysID.PadThis( 12, ' ', false ); ///< CTP OrderSysID Format: space(6) + 9(6)

	DelOrd->Cancel( UserDefine, CancelOrderRef );
	memset(&CancelOrder, 0, sizeof(CancelOrder));
	CancelOrder.LimitPrice     = 0.0;
	CancelOrder.VolumeChange   = 0;
	CancelOrder.ActionFlag     = USTP_FTDC_AF_Delete;
	strcpy( CancelOrder.UserID,        FID.c_str() );
	strcpy( CancelOrder.InvestorID,    FInvestorID.c_str() );
	strcpy( CancelOrder.BrokerID,      DelOrd->GetBrokerID());
	strcpy( CancelOrder.ExchangeID,    DelOrd->GetExchangeID());
	strcpy( CancelOrder.OrderSysID,    OrderSysID.c_str() );
	///< Order Sequence.
	sprintf( CancelOrder.UserOrderActionLocalID, "%012d", CancelOrderRef );
	sprintf( CancelOrder.UserOrderLocalID,       "%012d", OrgOrderRef );
	///< Allow to place cancel order ?
	if( FSendEventListener != NULL )
	{
		FSendEventListener->OnBeforeSend( Msg->GetMarket(), smtCancel, Msg, CanSend, ErrMsg );
		if( CanSend == false )
		{
			Reject( nsOrderMessageDefine::crrCancel, ErrMsg, Msg,Msg->GetTradingSessionID() );
			return;
		}
	}
	UFC::BufferedLog::Printf( " CancelOrderRef[%d] UserID[%s] InvestorID[%s] ExchangeID[%s]", CancelOrderRef, CancelOrder.UserID, FInvestorID.c_str(), CancelOrder.ExchangeID );
	if( (Rtn = FFEMASAPI->ReqOrderAction(  &CancelOrder, CTPReqID() )) != 0 )
	{
		UFC::BufferedLog::Printf( " ----------------------- Send failed:%d ------------------------", Rtn );
		ErrMsg.Printf( "Call ReqOrderAction return error, Code[%d]", Rtn );
		Reject( nsOrderMessageDefine::crrCancel, ErrMsg, Msg,Msg->GetTradingSessionID() );
	}
	else
	{
		UFC::BufferedLog::Printf( " -------------------- Send Cancel Order OK -----------------------" );
	}
}
//---------------------------------------------------------------------------
// This function will be called  When user call SendNewsRequest.
//---------------------------------------------------------------------------
int TTaifexConnection::HandleFemasMessage( TNewsMessage* Msg )
{
	int ctpRequestId = CTPReqID();

	if( Msg->GetID() == CTP_ChangePassword ) ///< Change Password.
	{
		UFC::NameValueMessage Params("^\n");
		UFC::AnsiString OldPassword, NewPassword, Type;

		Params.FromString( Msg->GetText() );
		if( Params.Get( "OldPassword", OldPassword )== true  &&
			Params.Get( "NewPassword", NewPassword )== true  )
		{
			CUstpFtdcUserPasswordUpdateField UserPasswordUpdate;

			memset(&UserPasswordUpdate, 0, sizeof(UserPasswordUpdate));
			strcpy(UserPasswordUpdate.BrokerID,    FCFFEXBrokerID.c_str());
			strcpy(UserPasswordUpdate.UserID,      FID.c_str());
			strcpy(UserPasswordUpdate.OldPassword, OldPassword.c_str());
			strcpy(UserPasswordUpdate.NewPassword, NewPassword.c_str());
			UFC::BufferedLog::Printf(" Call ReqUserPasswordUpdate: RequestID[%d] UserID[%d] BrokerID[%s]", ctpRequestId, FID.c_str(), FCFFEXBrokerID.c_str() );
			return FFEMASAPI->ReqUserPasswordUpdate( &UserPasswordUpdate, ctpRequestId );
		}
	}
	else if( Msg->GetID() == CTP_QueryTradingAccount ) ///< Query Trading Account
	{
		CUstpFtdcQryInvestorAccountField QryTradingAccount;
		UFC::NameValueMessage parameters("^\n");
		UFC::AnsiString ansiBrokerId, ansiInvestorId;

		parameters.FromString(Msg->GetText());
		if ((parameters.Get("BrokerId", ansiBrokerId) == FALSE) || (ansiBrokerId.Length() <= 0))
			ansiBrokerId = FCFFEXBrokerID;
		if ((parameters.Get("InvestorId", ansiInvestorId) == FALSE) || (ansiInvestorId.Length() <= 0))
			ansiInvestorId = FInvestorID;
		memset( &QryTradingAccount, 0, sizeof(CThostFtdcQryTradingAccountField));
		strcpy( QryTradingAccount.BrokerID,   ansiBrokerId.c_str());
		strcpy( QryTradingAccount.InvestorID, ansiInvestorId.c_str());
		UFC::BufferedLog::Printf(" Call ReqQryTradingAccount: RequestID[%d] UID[%d] BrokerID[%s] InvestorID[%s]", ctpRequestId, Msg->GetID(), ansiBrokerId.c_str(), ansiInvestorId.c_str());
		UFC::BufferedLog::FlushToFile();
		return FFEMASAPI->ReqQryInvestorAccount( &QryTradingAccount, ctpRequestId);
	}
	else if( Msg->GetID() == CTP_QueryInvestorPosition ) ///< InvestorPositionData.
	{
		CUstpFtdcQryInvestorPositionField qryInvestorPositionData;
		memset(&qryInvestorPositionData, 0, sizeof(CUstpFtdcQryInvestorPositionField));
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

		UFC::BufferedLog::Printf(" Call Femas ReqQryInvestorPosition: RequestID[%d] UID[%d] BrokerID[%s] InvestorID[%s] InstrumentID[%s]", ctpRequestId, Msg->GetID(), ansiBrokerId.c_str(), ansiInvestorId.c_str(), ansiInstrumentId.c_str());
		return FFEMASAPI->ReqQryInvestorPosition(&qryInvestorPositionData, ctpRequestId);
	}
	return -1;
}
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
