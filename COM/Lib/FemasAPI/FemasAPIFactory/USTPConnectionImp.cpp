//---------------------------------------------------------------------------
#ifdef _CTP
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "COrderInfo.h"
#include "TTaifexConnection.h"
//---------------------------------------------------------------------------
typedef AnsiStringT<936> GB2312String;
//---------------------------------------------------------------------------
//  ExecID Rule
//
//	Int32: FrontID,   <--- Map to Femas:DataCenterID
//		   SessionID, <--- Map to Femas:UserID
//		   OrderRef   <--- Map to Femas:UserOrderLocalID
//  Int64: OrderSysID
//
// 	NewOrder ExecID: "N" + FrontID + SessionID + OrderRef
//	Fill 	 ExecID: "F" + ExchangeID + OrderSysID + SequenceNo
//	Canceled ExecID: "C" + FrontID + SessionID + OrderRef
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void OrderStatusString( TUstpFtdcOrderStatusType ost, UFC::AnsiString& Str )
{
	switch( ost )
	{
		case '0': Str ="USTP_FTDC_OS_AllTraded";break;
		case '1': Str ="USTP_FTDC_OS_PartTradedQueueing";break;
		case '2': Str ="USTP_FTDC_OS_PartTradedNotQueueing";break;
		case '3': Str ="USTP_FTDC_OS_NoTradeQueueing";break;
		case '4': Str ="USTP_FTDC_OS_NoTradeNotQueueing";break;
		case '5': Str ="USTP_FTDC_OS_Canceled";break;
		case '6': Str ="USTP_FTDC_OS_AcceptNoReply";break;
		default : Str.Printf( "Value[%c] not define",ost );break;
	}
}
//---------------------------------------------------------------------------
//
// Implement interface TThostFtdcTraderSpi
// CTP SPI callback functions
//
//---------------------------------------------------------------------------
//
// OnAPIConnected, OnAPIDisconnected Trigger by CTP API thread.
// Not the same thread as the main UI thread!
// Don't modify any GDI object in this function
//
//---------------------------------------------------------------------------
void TTaifexConnection::OnFemasAPIConnected()
{
	FIsConnected = true;
	FLogonEvent.SetEvent();
}
//---------------------------------------------------------------------------
void TTaifexConnection::OnFemasAPIDisconnected( int )
{
	FIsConnected = false;
	FIsLogon = false;
	DetachShm();
	if( FCTPTransLog != NULL )
	{
		FCTPTransLog->Flush();
		delete FCTPTransLog;
		FCTPTransLog = NULL;
	}
}
//---------------------------------------------------------------------------
void TTaifexConnection::OnFemasRecoverFinished(int Count )
{
	if( FListener != NULL )
		FListener->OnRecoverFinished( Count );
}
//---------------------------------------------------------------------------
// Trigger by Main UI thread.
//---------------------------------------------------------------------------
void TTaifexConnection::OnFemasFrontConnected()
{
	if( FListener != NULL  )
	{
		if(  FTriggerCTPOnConnected == true )
		{
			UFC::SleepMS( 330 );
			FTriggerCTPOnConnected = false;
			FListener->OnConnected();
		}
		else
		{
			TNewsMessage Msg;

			Msg.SetHeadline( "FEMAS" );
			Msg.SetID( CTP_OnConnected );
			FListener->OnNews( &Msg );
		}
	}
}
//---------------------------------------------------------------------------
void TTaifexConnection::OnFemasFrontDisconnected(int  )
{
	if( FListener != NULL )
	{
		FListener->OnDisconnected();
		TNewsMessage Msg;

		Msg.SetHeadline( "FEMAS");
		Msg.SetID( CTP_OnDisconnectd );
		FListener->OnNews( &Msg );
	}
}
//---------------------------------------------------------------------------
UFC::AnsiString TTaifexConnection::BuildResponseInformationStr(CUstpFtdcRspInfoField *pRspInfo, const UFC::AnsiString& FieldDelimiter)
{
	UFC::AnsiString ansiResponseStr = "";
	if (pRspInfo == NULL)
		ansiResponseStr.Printf("0%s", FieldDelimiter.c_str());
	else
	{
		int errorId = pRspInfo->ErrorID;  //1.錯誤代碼
		AppendFieldToFieldsStr(ansiResponseStr, errorId, FieldDelimiter);
		GB2312String gb2312ErrorMsg(pRspInfo->ErrorMsg);  //2.錯誤訊息
		UTF8String utf8ErrorMsg(gb2312ErrorMsg);
		UFC::AnsiString ansiErrorMsg(utf8ErrorMsg.c_str());
		AppendFieldToFieldsStr(ansiResponseStr, ansiErrorMsg, FieldDelimiter);
	}
	return ansiResponseStr;
}  //TTaifexConnection::BuildResponseInformationStr()
//---------------------------------------------------------------------------
void TTaifexConnection::OnFemasRspQryUserInvestor(CUstpFtdcRspUserInvestorField *pRspUserInvestor, CUstpFtdcRspInfoField *, int , bool)
{
	FInvestorID = pRspUserInvestor->InvestorID;
	UFC::BufferedLog::Printf( " OnRspQryUserInvestor: UserID[%s] InvestorID[%s]", pRspUserInvestor->UserID, pRspUserInvestor->InvestorID );
}
//---------------------------------------------------------------------------
void TTaifexConnection::OnFemasRspUserLogin( CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int, bool )
{
	LogonResult  Result = lrFailed;
	AnsiString   ReplyString( "" );

	FAdmin = 0;
	if( pRspInfo != NULL && pRspInfo->ErrorID != 0) ///< Logon failed.
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		UTF8String   ErrMsgUTF8( ErrMsg );

		ReplyString = ErrMsgUTF8.c_str();
		Result   = lrFailed;
		FIsLogon = false;
		FCurrentConnectionID = 0;
		UFC::BufferedLog::Printf( " OnRspUserLogin: Account[%s] Code[%d] Msg[%s] Logon failed.", pRspUserLogin->UserID, pRspInfo->ErrorID, ErrMsgUTF8.c_str() );
		if( FListener != NULL )
			FListener->OnLogonReply( ReplyString.c_str(), Result, FCurrentConnectionID );
	}
	else ///< Logon OK
	{
		UFC::AnsiString FileName,MaxOLID( pRspUserLogin->MaxOrderLocalID );
		TNewsMessage Msg,ReqMsg;
		UFC::NameValueMessage TimeMsg("^\n");

		FTradeingDate = pRspUserLogin->TradingDay;
		FIsRecover = false;
		FRecoverCount = 0;
		FLastRecover = UFC::GetTickCountMS();
		RemoveShm( "FEMAS" );
		AttachShm( "FEMAS", pRspUserLogin->TradingDay );
		FCTPFrontID   = pRspUserLogin->DataCenterID;
		FCTPSessionID = atoi( pRspUserLogin->UserID );
		FCurrentConnectionID = FCTPSessionID;
		Result   = lrOk;
		FIsLogon = true;
		FUseNID64 = true;
		if( MaxOLID.Length() <= 0 )
			MaxOLID = "0";
		UFC::BufferedLog::Printf( " OnRspUserLogin: Account[%s] TradingDay[%s] MaxOrderLocalID[%s] TradingSystemName[%s] Logon OK! ",
								  pRspUserLogin->UserID,
								  pRspUserLogin->TradingDay,
								  MaxOLID.c_str(),
								  pRspUserLogin->TradingSystemName );
		FOrderRef = MaxOLID.ToInt() + 1;
		if( FListener != NULL )
			FListener->OnLogonReply( ReplyString.c_str(), Result, FCurrentConnectionID );
		CUstpFtdcQryUserInvestorField QryUserInvestor;

		strcpy(QryUserInvestor.BrokerID,    FCFFEXBrokerID.c_str());
		strcpy(QryUserInvestor.UserID,      FID.c_str());
		FFEMASAPI->ReqQryUserInvestor( &QryUserInvestor, CTPReqID() );
	}
	FFEMASAPI->SetEvent();
}
//---------------------------------------------------------------------------
void TTaifexConnection::OnFemasRspUserLogout(CUstpFtdcRspUserLogoutField *, CUstpFtdcRspInfoField *, int , bool )
{
	FAdmin   = 0;
	FIsLogon = false; ///< Clear logon flag.
	FID      = "";
	FToken   = "";
	DetachShm();
	if( FCTPTransLog != NULL )
	{
		FCTPTransLog->Flush();
		delete FCTPTransLog;
		FCTPTransLog = NULL;
	}
}
//---------------------------------------------------------------------------
void TTaifexConnection::OnFemasRspUserPasswordUpdate(CUstpFtdcUserPasswordUpdateField *pUserPasswordUpdate, CUstpFtdcRspInfoField *pRspInfo, int , bool )
{
	TNewsMessage Msg;

	Msg.SetHeadline( "FEMATS");
	Msg.SetID( CTP_OnPasswordChanged );
	if( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		UTF8String   UTF8Text( ErrMsg );

		Msg.SetText( UTF8Text.c_str() );
		UFC::BufferedLog::Printf( " OnRspUserPasswordUpdate: UserID[%s] Change Password failed! Code[%d] Msg[%s]", pUserPasswordUpdate->UserID, pRspInfo->ErrorID, UTF8Text.c_str() );
	}
	else
	{
		Msg.SetText( "OK" );
		UFC::BufferedLog::Printf( " OnRspUserPasswordUpdate: UserID[%s] Change Password OK! ", pUserPasswordUpdate->UserID );
	}
	if( FListener != NULL )
		FListener->OnNews( &Msg );
}
//---------------------------------------------------------------------------
void TTaifexConnection::OnFemasRspQryInvestorAccount(CUstpFtdcRspInvestorAccountField *pTradingAccount, CUstpFtdcRspInfoField *pRspInfo, int , bool bIsLast)
{
	if( pRspInfo != NULL && pRspInfo->ErrorID != 0  )
	{
		UFC::BufferedLog::Printf( " OnRspQryTradingAccount failed code:%d", pRspInfo->ErrorID );
		return;
	}
	if( pTradingAccount != NULL && bIsLast )
	{
		TNewsMessage Msg;
		UFC::NameValueMessage BalanceMsg("^\n");

		Msg.SetHeadline( "FEMAS");
		Msg.SetID( CTP_OnTradingAccount );
		BalanceMsg.Append( "PreBalance",       pTradingAccount->PreBalance );     ///< 上次結算準備金
		BalanceMsg.Append( "Withdraw",         pTradingAccount->Withdraw );       ///< 出金金額
		BalanceMsg.Append( "Deposit",          pTradingAccount->Deposit );        ///< 入金金額
		BalanceMsg.Append( "CloseProfit",      pTradingAccount->CloseProfit );    ///< 平倉盈虧
		BalanceMsg.Append( "PositionProfit",   pTradingAccount->PositionProfit ); ///< 持倉盈虧
		BalanceMsg.Append( "Commission",       pTradingAccount->Fee );            ///< 手續費
		BalanceMsg.Append( "CurrMargin",       pTradingAccount->Margin ); /// 當前保證金總額  *To Do:
		BalanceMsg.Append( "FrozenMargin",     pTradingAccount->FrozenMargin );   ///< 凍結的保證金
		BalanceMsg.Append( "FrozenCommission", pTradingAccount->FrozenFee );      ///< 凍結手續費
		BalanceMsg.Append( "Available",        pTradingAccount->Available );      ///< 可用資金

		BalanceMsg.Append( "PreCredit",        "0" ); // 上次信用額度  *To Do:
		BalanceMsg.Append( "PreMortgage",      "0" ); // 上次質押金額  *To Do:
		BalanceMsg.Append( "Mortgage",         "0" ); // 質押金額  *To Do:
		BalanceMsg.Append( "DeliveryMargin",   "0" ); // 交割保證金  *To Do:
		BalanceMsg.Append( "Credit",           "0" ); // 信用金額  *To Do:
		BalanceMsg.Append( "WithdrawQuota",    "0" ); // 可取資金  *To Do:

		BalanceMsg.Append( "DynamicRights",    pTradingAccount->DynamicRights ); //< 動態權益
		BalanceMsg.Append( "Risk",             pTradingAccount->Risk ); //< 風險度
		FBankAccount = pTradingAccount->AccountID;
		Msg.SetText( BalanceMsg.ToString().c_str() );
		if( FListener != NULL )
			FListener->OnNews( &Msg );
	}
}
//---------------------------------------------------------------------------
void TTaifexConnection::OnFemasRspQryInvestorPosition(CUstpFtdcRspInvestorPositionField *pRspInvestorPosition, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	UFC::AnsiString ansiResponseStr = BuildResponseInformationStr(pRspInfo, "|");
	AppendFieldToFieldsStr(ansiResponseStr, nRequestID, "|");
	AppendFieldToFieldsStr(ansiResponseStr, bIsLast, "|");
	if (pRspInvestorPosition != NULL)
	{
		UFC::AnsiString ansiInvestorId = pRspInvestorPosition->InvestorID;  // 5.投資者編號
		AppendFieldToFieldsStr(ansiResponseStr, ansiInvestorId, "|");
		UFC::AnsiString ansiBrokerId = pRspInvestorPosition->BrokerID;  // 6.經紀公司編號
		AppendFieldToFieldsStr(ansiResponseStr, ansiBrokerId, "|");
		UFC::AnsiString ansiExchangeCode = pRspInvestorPosition->ExchangeID;  // 7.交易所代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiExchangeCode, "|");
		UFC::AnsiString ansiClientId = pRspInvestorPosition->ClientID;  // 8.客戶代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiClientId, "|");
		UFC::AnsiString ansiInstrumentId = pRspInvestorPosition->InstrumentID;  // 9.合約代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiInstrumentId, "|");
		char direction = pRspInvestorPosition->Direction;  //10.買賣方向 '0':買  '1':賣
		AppendFieldToFieldsStr(ansiResponseStr, direction, "|");
		char hedgeFlag = pRspInvestorPosition->HedgeFlag;  //11.投機套保標誌 '1':投機  '2':套利  '3':套保
		AppendFieldToFieldsStr(ansiResponseStr, hedgeFlag, "|");
		double usedMargin = pRspInvestorPosition->UsedMargin;  //12.佔用的保證金
		AppendFieldToFieldsStr(ansiResponseStr, usedMargin, "|");
		int tdPosition = pRspInvestorPosition->Position;  //13.今日持倉量
		AppendFieldToFieldsStr(ansiResponseStr, tdPosition, "|");
		double positionCost = pRspInvestorPosition->PositionCost;  //14.今日持倉成本
		AppendFieldToFieldsStr(ansiResponseStr, positionCost, "|");
		int ydPosition = pRspInvestorPosition->YdPosition;  //15.上日持倉
		AppendFieldToFieldsStr(ansiResponseStr, ydPosition, "|");
		double ydPositionCost = pRspInvestorPosition->YdPositionCost;  //16.上日持倉成本
		AppendFieldToFieldsStr(ansiResponseStr, ydPositionCost, "|");
		double frozenMargin = pRspInvestorPosition->FrozenMargin;  //17.凍結的保證金
		AppendFieldToFieldsStr(ansiResponseStr, frozenMargin, "|");
		int frozenPosition = pRspInvestorPosition->FrozenPosition;  //18.開倉凍結持倉
		AppendFieldToFieldsStr(ansiResponseStr, frozenPosition, "|");
		int frozenClosing = pRspInvestorPosition->FrozenClosing;  //19.平倉凍結持倉
		AppendFieldToFieldsStr(ansiResponseStr, frozenClosing, "|");
		int frozenPremium = pRspInvestorPosition->FrozenPremium;  //20.凍結的權利金
		AppendFieldToFieldsStr(ansiResponseStr, frozenPremium, "|");
		UFC::AnsiString ansiLastTradeId = pRspInvestorPosition->LastTradeID;  //21.最後一筆成交編號
		AppendFieldToFieldsStr(ansiResponseStr, ansiLastTradeId, "|");
		UFC::AnsiString ansiLastOrderLocalId = pRspInvestorPosition->LastOrderLocalID;  //22.最後一筆本地報單編號
		AppendFieldToFieldsStr(ansiResponseStr, ansiLastOrderLocalId, "|");
		UFC::AnsiString ansiCurrency = pRspInvestorPosition->Currency;  //23.幣種
		AppendFieldToFieldsStr(ansiResponseStr, ansiCurrency, "|");
		UFC::BufferedLog::Printf(" OnFemasRspQryInvestorPosition:%s %s [%s-%s] Dir=%c YD[Pos=%d cost=%15.6lf] TD[Pos=%d  cost=%15.6lf]",
								 ansiBrokerId.c_str(), ansiInvestorId.c_str(), ansiExchangeCode.c_str(), ansiInstrumentId.c_str(), direction, ydPosition, ydPositionCost, tdPosition, positionCost);
		UFC::BufferedLog::Printf(" OnFemasRspQryInvestorPosition:[%s]", ansiResponseStr.c_str());
		UFC::BufferedLog::FlushToFile();
	}  //if (pRspInvestorPosition != NULL)

	if (FListener != NULL)
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline("FEMAS");
		newsMsg.SetID(CTP_OnInvestorPosition);
		newsMsg.SetText(ansiResponseStr.c_str());
		FListener->OnNews(&newsMsg);
	}
	else
		UFC::BufferedLog::Printf("TTaifexConnection::OnFemasRspQryInvestorPosition() %s", ansiResponseStr.c_str());
}  //TTaifexConnection::OnFemasRspQryInvestorPosition()
//---------------------------------------------------------------------------
void TTaifexConnection::OnFemasRtnInstrumentStatus(CUstpFtdcInstrumentStatusField *pInstrumentStatus)
{
	TNewsMessage Msg;
	UFC::NameValueMessage StatusMsg("^\n");

	if( pInstrumentStatus != NULL )
	{
		Msg.SetHeadline( "FEMATS");
		Msg.SetID( CTP_OnExchangeStatus );
		StatusMsg.Append( "Exchange",  pInstrumentStatus->ExchangeID );
		StatusMsg.Append( "Status",  UFC::AnsiString(pInstrumentStatus->InstrumentStatus) );
		Msg.SetText( StatusMsg.ToString().c_str() );
		if( FListener != NULL )
			FListener->OnNews( &Msg );
		UFC::BufferedLog::Printf( " OnRtnInstrumentStatus Exchange[%s] Status[%c] ",
									pInstrumentStatus->ExchangeID,
									pInstrumentStatus->InstrumentStatus );
	}
}
//---------------------------------------------------------------------------
void TTaifexConnection::OnFemasRspError(CUstpFtdcRspInfoField *pRspInfo, int , bool )
{
	if( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		UTF8String   ErrMsgUTF8 = ErrMsg;
		TNewsMessage newsMsg;

		newsMsg.SetHeadline("FEMAS");
		newsMsg.SetID( CTP_OnRspError );
		newsMsg.SetText( ErrMsgUTF8.c_str());
		if( FListener != NULL )
			FListener->OnNews(&newsMsg);
		UFC::BufferedLog::Printf( " OnRspError: Code[%d] Msg[%s]", pRspInfo->ErrorID, ErrMsgUTF8.c_str() );
	}
}
//---------------------------------------------------------------------------
//
// Implement CTP SPI callback functions
// about Order executions.( Reject,Confirm,Fill,Panding...)
//
//---------------------------------------------------------------------------
// Cancel Order error from Exchange.
//---------------------------------------------------------------------------
void TTaifexConnection::OnFemasErrRtnOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo)
{
	if( pOrderAction != NULL )
	{
		UpdateRecover();
		GB2312String            ErrMsg( pRspInfo->ErrorMsg );
		UTF8String              ErrMsgUTF8( ErrMsg );
		TExecutionReportMessage ExecutionReport;
		UFC::AnsiString         ExecID;
		UFC::AnsiString         ExchangeID( pOrderAction->ExchangeID );
		UFC::AnsiString         OrderSysID( pOrderAction->OrderSysID );
		COrderInfo*             Ord;
		bool IsDup;

		UFC::BufferedLog::Printf( " OnErrRtnOrderAction: Exchange[%s] OrderSysID[%s]",
								  pOrderAction->ExchangeID,
								  pOrderAction->OrderSysID );
		if( ( Ord = FindOrderByOrderSysID( ExchangeID, OrderSysID.ToInt64())) != NULL )
		{
			OrderSysID.TrimLeft();
			UFC::BufferedLog::Printf( " ----------------------- Femas Cancel Reject --------------------------" );
			ExecID.Printf( "CR_%s_%s_%d", ExchangeID.c_str(), OrderSysID.c_str(), atoi(pOrderAction->UserOrderActionLocalID ) );
			CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Ord );
			ExecutionReport.SetOrderID( OrderSysID.c_str() );
			ExecutionReport.SetNID( Ord->GetOrderNID() );
			ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osRejected );
			ExecutionReport.SetExecType( nsOrderMessageDefine::etRejected );
			ExecutionReport.SetCxlRejResponseTo( nsOrderMessageDefine::crrCancel );
			ExecutionReport.SetAllUserData( Ord->GetCxlUserData() );
			ExecutionReport.SetExecID( ExecID.c_str());
			ExecutionReport.SetLastQty( 0 );
			IsDup = IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
			UFC::BufferedLog::Printf( " OrderRef[%s] ExchangeID[%s] OrderSysID[%s]", pOrderAction->UserOrderActionLocalID, ExchangeID.c_str(), OrderSysID.c_str() );
			if( ErrMsg.Length() > 0 )
			{
				ExecutionReport.SetText( ErrMsgUTF8.c_str());
				UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] Msg[%s]", ExecID.c_str(), IsDup ? "Yes" : "No", ErrMsgUTF8.c_str() );
			}
			else
				UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] ", ExecID.c_str(), IsDup ? "Yes" : "No" );
			TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
			UFC::BufferedLog::Printf( " ------------------------ Sent Cancel Reject ------------------------" );
		}
	}
}
//---------------------------------------------------------------------------
// Reject New Order from Femas.
//---------------------------------------------------------------------------
void TTaifexConnection::OnFemasErrRtnOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo)
{
	if( pRspInfo != NULL && pRspInfo->ErrorID != 0 && pInputOrder != NULL )
	{
		GB2312String     ErrMsg( pRspInfo->ErrorMsg );
		UTF8String       ErrMsgUTF8 = ErrMsg;
		UFC::AnsiString  PEStr;
		TNewOrderMessage New;

		UpdateRecover();
		UFC::BufferedLog::Printf( " ------------------------- Femas Reject ----------------------------" );
		UFC::BufferedLog::Printf( " OnErrRtnOrderInsert: OrderRef[%s] ",  pInputOrder->UserOrderLocalID );
		New.SetAccount( pInputOrder->UserID );
		New.SetBrokerID( pInputOrder->BrokerID );
		New.SetSymbol( pInputOrder->InstrumentID );
		New.SetMarket( nsOrderMessageDefine::mCNFutures );
		New.SetNID( COrderInfo::ToNID( FCTPFrontID, FCTPSessionID, atoi( pInputOrder->UserOrderLocalID )) );
		if(	pInputOrder->OffsetFlag == USTP_FTDC_OF_Open ) //Open
		{
			New.SetPositionEffect( nsOrderMessageDefine::peOpen );
			PEStr = "Open";
		}
		else if( pInputOrder->OffsetFlag == USTP_FTDC_OF_Close )//Close
		{
			New.SetPositionEffect( nsOrderMessageDefine::peClose );
			PEStr = "Close";
		}
		else if( pInputOrder->OffsetFlag== USTP_FTDC_OF_CloseToday )//Close Today
		{
			New.SetPositionEffect( nsOrderMessageDefine::peRolled );
			PEStr = "Close Today";
		}
		if(	pInputOrder->Direction == USTP_FTDC_D_Buy )
			New.SetSide( nsOrderMessageDefine::sBuy );
		else
			New.SetSide( nsOrderMessageDefine::sSell );
		if( pInputOrder->OrderPriceType == USTP_FTDC_OPT_AnyPrice )
		{
			New.SetOrderType( nsOrderMessageDefine::otMarket );
			New.SetPrice( 0.0 );
		}
		else
		{
			New.SetOrderType( nsOrderMessageDefine::otLimit );
			New.SetPrice( pInputOrder->LimitPrice );
		}
		if( pInputOrder->TimeCondition == USTP_FTDC_TC_IOC )
			New.SetTimeInForce( nsOrderMessageDefine::tifIOC );
		else
			New.SetTimeInForce( nsOrderMessageDefine::tifROD );
		New.SetOrderQty( pInputOrder->Volume );
		UFC::BufferedLog::Printf( " NID[%s] Symbol[%s] PositionEffect[%s]", pInputOrder->UserOrderLocalID, pInputOrder->InstrumentID, PEStr.c_str() );
		Reject( nsOrderMessageDefine::crrNew, ErrMsgUTF8.c_str(), &New, New.GetTradingSessionID() );
		UFC::BufferedLog::Printf( " ------------------- Sent Reject Execution -----------------------" );
	}
}
//---------------------------------------------------------------------------
// Femas NewOrder.
//---------------------------------------------------------------------------
void TTaifexConnection::OnFemasRspOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *, int , bool )
{
	UFC::AnsiString OrdRef( pInputOrder->UserOrderLocalID );
	UFC::AnsiString ExchangeID( pInputOrder->ExchangeID  );
	UFC::AnsiString OrderSysID( pInputOrder->OrderSysID  );
	COrderInfo*     Record;

	///< Print order info.
	UFC::BufferedLog::Printf( " OnRtnOrder: FrontID[%d] SessionID[%d] OrderRef[%s].", FCTPFrontID, FCTPSessionID, pInputOrder->UserOrderLocalID );
	UpdateRecover();
	///< Use FrontID + SessionID + OrderRef to find Order.
	if( (Record = FindOrderByOrderRef( FCTPFrontID, FCTPSessionID, OrdRef.ToInt())) != NULL )
	{
		UFC::BufferedLog::Printf( " OnRtnOrder: Use FrontID + SessionID + OrderRef found the original order.");
		if( Record->GetStatus() == cosPending  )
			FemasPendingExecution( Record, pInputOrder ); ///< Send pending New execution.
	}
}
//---------------------------------------------------------------------------
void TTaifexConnection::FemasPendingExecution( COrderInfo* Ord,CUstpFtdcOrderField *pOrder)
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	UFC::AnsiString         OrderLocalID( pOrder->OrderLocalID );
	Int64                   NID64 = Ord->GetOrderNID();
	bool IsDup;

	UFC::BufferedLog::Printf( " ---------------------------- Femas Pending ------------------------" );
	ExecID.Printf( "P_%d_%d_%d", Ord->GetFrontID(), Ord->GetSessionID(), Ord->GetOrderRef() );
	CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Ord );
	ExecutionReport.SetOrderID( OrderLocalID.c_str() );
	ExecutionReport.SetNID( NID64 );
	ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osPendingNew );
	ExecutionReport.SetExecType( nsOrderMessageDefine::etPendingNew );
	ExecutionReport.SetAllUserData( Ord->GetUserData() );
	ExecutionReport.SetExecID( ExecID.c_str());
	ExecutionReport.SetLastQty( 0 );
	ExecutionReport.SetTransactTime( pOrder->InsertTime );
	UFC::BufferedLog::Printf( " NID[%s] ExchangeID[%s] Symbol[%s] OrderSysID[%s] OrderLocalID[%s]", pOrder->UserOrderLocalID, pOrder->ExchangeID, pOrder->InstrumentID, pOrder->OrderSysID, pOrder->OrderLocalID );
	IsDup = IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
	UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] ", ExecID.c_str(), IsDup ? "Yes" : "No" );
	TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() ));
	UFC::BufferedLog::Printf( " ---------------- Sent Pending New Execution ---------------------" );
}
//---------------------------------------------------------------------------
void TTaifexConnection::FemasPendingExecution( COrderInfo* Ord, CUstpFtdcInputOrderField *)
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	UFC::AnsiString         TimeNow;
	Int64                   NID64 = Ord->GetOrderNID();
	bool IsDup;

	UFC::BufferedLog::Printf( " ---------------------------- Femas Pending ------------------------" );
	ExecID.Printf( "P_%d_%d_%d", Ord->GetFrontID(), Ord->GetSessionID(), Ord->GetOrderRef() );
	CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Ord );
	ExecutionReport.SetNID( NID64 );
	ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osPendingNew );
	ExecutionReport.SetExecType( nsOrderMessageDefine::etPendingNew );
	ExecutionReport.SetAllUserData( Ord->GetUserData() );
	ExecutionReport.SetExecID( ExecID.c_str());
	ExecutionReport.SetLastQty( 0 );
	GetTimeString( TimeNow , false );
	ExecutionReport.SetTransactTime( TimeNow.c_str() );
	IsDup = IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
	UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] ", ExecID.c_str(), IsDup ? "Yes" : "No" );
	TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() ));
	UFC::BufferedLog::Printf( " ---------------- Sent Pending New Execution ---------------------" );
}
//---------------------------------------------------------------------------
void TTaifexConnection::FemasConfirmExecution( COrderInfo* Ord, CUstpFtdcOrderField *pOrder)
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	UFC::AnsiString         OrderSysID( pOrder->OrderSysID );
	bool IsDup;

	UFC::BufferedLog::Printf( " ---------------------------- CTP Confirm ------------------------" );
	ExecID.Printf( "N_%d_%d_%d", Ord->GetFrontID(), Ord->GetSessionID() ,Ord->GetOrderRef() );
	OrderSysID.TrimLeft();
	CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Ord );
	ExecutionReport.SetOrderID( OrderSysID.c_str() );
	ExecutionReport.SetNID( Ord->GetOrderNID() );
	ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osNew );
	ExecutionReport.SetExecType( nsOrderMessageDefine::etNew );
	ExecutionReport.SetAllUserData( Ord->GetUserData() );
	ExecutionReport.SetExecID( ExecID.c_str());
	ExecutionReport.SetLastQty( 0 );
	ExecutionReport.SetTransactTime( pOrder->InsertTime );
	UFC::BufferedLog::Printf( " NID[%s] ExchangeID[%s] Symbol[%s] OrderSysID[%s] OrderLocalID[%s]", pOrder->UserOrderLocalID, pOrder->ExchangeID, pOrder->InstrumentID, pOrder->OrderSysID, pOrder->OrderLocalID );
	IsDup = IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
	UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] ", ExecID.c_str(), IsDup ? "Yes" : "No" );
	TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() ));
	UFC::BufferedLog::Printf( " ----------------- Sent Confirmed Execution ----------------------" );
}
//---------------------------------------------------------------------------
void TTaifexConnection::FemasCanceledExecution( COrderInfo* Ord, CUstpFtdcOrderField *pOrder)
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	UFC::AnsiString         OrderLocalID( pOrder->OrderLocalID );
	UFC::AnsiString         OrderSysID( pOrder->OrderSysID );
	bool IsDup;

	UFC::BufferedLog::Printf( " ---------------------------- CTP Cancled ------------------------" );
	ExecID.Printf( "C_%d_%d_%d", Ord->GetFrontID(), Ord->GetSessionID() ,Ord->GetOrderRef() );

	OrderLocalID.TrimLeft();
	OrderSysID.TrimLeft();
	CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Ord );
	if( OrderSysID.Length() == 0 )
		ExecutionReport.SetOrderID( OrderLocalID.c_str() );
	else
		ExecutionReport.SetOrderID( OrderSysID.c_str() );
	if( Ord->GetCancelRef() == 0 )
		ExecutionReport.SetNID( Ord->GetOrderNID() );
	else
		ExecutionReport.SetNID( Ord->GetCancelNID() );
	ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osCanceled );
	ExecutionReport.SetExecType( nsOrderMessageDefine::etCanceled );
	ExecutionReport.SetAllUserData( Ord->GetCxlUserData() );
	ExecutionReport.SetExecID( ExecID.c_str());
	ExecutionReport.SetLastQty( 0 );
	ExecutionReport.SetTransactTime( pOrder->CancelTime );
	UFC::BufferedLog::Printf( " OrdNID[%s] NID[%lld] ExchangeID[%s] Symbol[%s] OrderSysID[%s] OrderLocalID[%s]", pOrder->UserOrderLocalID, ExecutionReport.GetNID(), pOrder->ExchangeID, pOrder->InstrumentID, pOrder->OrderSysID, pOrder->OrderLocalID );
	IsDup = IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
	UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] ", ExecID.c_str(), IsDup ? "Yes" : "No" );
	TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	UFC::BufferedLog::Printf( " ------------------ Sent Canceled Execution ----------------------" );
}
//---------------------------------------------------------------------------
void TTaifexConnection::FemasRejectExecution( COrderInfo* Ord, CUstpFtdcOrderField *pOrder )
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	UFC::AnsiString         OrderLocalID( pOrder->OrderLocalID );
	UFC::AnsiString         OrderSysID( pOrder->OrderSysID );
	bool IsDup;

	UFC::BufferedLog::Printf( " ------------------------- Femas Reject ----------------------------" );
	ExecID.Printf( "R_%d_%d_%d", Ord->GetFrontID(), Ord->GetSessionID() ,Ord->GetOrderRef() );
	OrderLocalID.TrimLeft();
	OrderSysID.TrimLeft();
	CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Ord );
	if( OrderSysID.Length() == 0 )
		ExecutionReport.SetOrderID( OrderLocalID.c_str() );
	else
		ExecutionReport.SetOrderID( OrderSysID.c_str() );
	ExecutionReport.SetNID( Ord->GetOrderNID() );
	ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osRejected );
	ExecutionReport.SetExecType( nsOrderMessageDefine::etRejected );
	ExecutionReport.SetCxlRejResponseTo( nsOrderMessageDefine::crrNew );
	ExecutionReport.SetAllUserData( Ord->GetCxlUserData() );
	ExecutionReport.SetExecID( ExecID.c_str());
	ExecutionReport.SetLastQty( 0 );
	ExecutionReport.SetTransactTime( pOrder->CancelTime );
	IsDup = IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
	UFC::BufferedLog::Printf( " NID[%s] ExchangeID[%s] Symbol[%s] OrderSysID[%s] OrderLocalID[%s]", pOrder->UserOrderLocalID, pOrder->ExchangeID, pOrder->InstrumentID, pOrder->OrderSysID, pOrder->OrderLocalID);
	UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] ", ExecID.c_str(), IsDup ? "Yes" : "No" );
	TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	UFC::BufferedLog::Printf( " ------------------- Sent Reject Execution -----------------------" );
}
//---------------------------------------------------------------------------
// CTP Order Confirmation.
//---------------------------------------------------------------------------
void TTaifexConnection::OnFemasRtnOrder( CUstpFtdcOrderField *pOrder )
{
	///< CTP Ack: (FrontID + SessionID ) OrderRef to find Order record.
	///<          Update OrderLocalID,ExchangeID
	///< Exchange Ack: ExchangeID + OrderLocalID  to find Order record.
	///<           Update OrderSys.
	if( pOrder == NULL )
		return;
	UFC::AnsiString OrdRef( pOrder->UserOrderLocalID );
	UFC::AnsiString ExchangeID( pOrder->ExchangeID  );
	UFC::AnsiString OrderLocalID( pOrder->OrderLocalID  );
	UFC::AnsiString OrdSysID( pOrder->OrderSysID );
	UFC::AnsiString Status;
	COrderInfo*     Record;

	///< Print order info.
	OrderStatusString( pOrder->OrderStatus, Status );
	UFC::BufferedLog::Printf( " OnRtnOrder: FrontID[%d] SessionID[%d] OrderRef[%s].", FCTPFrontID, FCTPSessionID, pOrder->UserOrderLocalID );
	UFC::BufferedLog::Printf( " OnRtnOrder: Exchange[%s] OrderSysID[%s] OrderLocalID[%s].", ExchangeID.c_str(), OrdSysID.c_str(), OrderLocalID.c_str());
	UFC::BufferedLog::Printf( " OnRtnOrder: OrderStatus[%s]", Status.c_str() );

	UpdateRecover();
	///< Use FrontID + SessionID + OrderRef to find Order.
	if( OrdRef.Length() > 0 && (Record = FindOrderByOrderRef( FCTPFrontID, FCTPSessionID, OrdRef.ToInt())) != NULL )
		UFC::BufferedLog::Printf( " OnRtnOrder: Use FrontID + SessionID + OrderRef found the original order.");
	///< Use ExchangeID + OrderSysID to find Order.
	else if( ExchangeID.Length() > 0 && OrdSysID.Length() > 0 &&
			 (Record = FindOrderByOrderSysID( ExchangeID, OrdSysID.ToInt64())) != NULL )
		UFC::BufferedLog::Printf( " OnRtnOrder: Use ExchangeID + OrderSysID found the original order.");
	else ///< Order not found!
	{
		UFC::BufferedLog::Printf( " --------------------- Receive Execution from other Femas clients -------------------------" );
		UFC::BufferedLog::Printf( " OnRtnOrder: Order Exchange[%s] OrderSysID[%s] OrderLocalID[%s] not found!", ExchangeID.c_str(), OrdSysID.c_str(), OrderLocalID.c_str());
		UFC::BufferedLog::Printf( " OnRtnOrder: Add Order record FrontID[%d] SessionID[%d] OrderRef[%s]", FCTPFrontID, FCTPSessionID, pOrder->UserOrderLocalID );
		UFC::BufferedLog::Printf( " ----------------------------------------------------------------------------------------" );
		Record = AddPendingOrder( pOrder );
	}
	///< Update Record fields
	if( Record->GetStatus() == cosPending && OrdSysID.Length() > 0 && ExchangeID.Length() > 0 )
	{
		Record->FemasAccepted( ExchangeID, OrdSysID, OrderLocalID );
		AddOrderSysIDToMap( ExchangeID, OrdSysID.ToInt64() , Record );
		FemasConfirmExecution( Record, pOrder ); ///< Send confirm execution.
	}

	if( pOrder->OrderStatus == USTP_FTDC_OS_Canceled )
	{
		Record->CloseOrder();
		FemasCanceledExecution( Record, pOrder );///< Send Canceled execution.
	}
}
//---------------------------------------------------------------------------
void TTaifexConnection::FemasFillExecution( COrderInfo* Ord, CUstpFtdcTradeField *pTrade )
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString OrdSysIDStr( pTrade->OrderSysID );
	UFC::AnsiString ExecID;
	int TradeSeg = UFC::AnsiString( pTrade->TradeID ).ToInt();
	bool IsDup;

	OrdSysIDStr.TrimLeft();
	UFC::BufferedLog::Printf( " -------------------------- Femas Fill -----------------------------" );
	ExecID.Printf( "F_%s_%c_%lld_%d", pTrade->ExchangeID, pTrade->Direction, OrdSysIDStr.ToInt64(),  TradeSeg );
	if( (IsDup = IsExecutionDup( nsOrderMessageDefine::mCNFutures, ExecID )) == false )
		Ord->Fill( pTrade->TradeVolume );
	UFC::BufferedLog::Printf( " NID[%s] ExchangeID[%s] Symbol[%s] OrderSysID[%s]", pTrade->UserOrderLocalID, pTrade->ExchangeID, pTrade->InstrumentID, pTrade->OrderSysID );
	UFC::BufferedLog::Printf( " Account[%s] Time[%s] Side[%c] ", pTrade->UserID, pTrade->TradeTime, pTrade->Direction == USTP_FTDC_D_Buy? 'B':'S' );
	UFC::BufferedLog::Printf( " LastPx[%0.3f] LastQty[%d] CumQty[%d] LeavesQty[%d]", pTrade->TradePrice, pTrade->TradeVolume, Ord->GetCumQty(), Ord->GetLeavesQty() );
	UFC::BufferedLog::Printf( " ExecID[%s] TradeID[%d] Dup[%s] NID[%lld]", ExecID.c_str(), TradeSeg, IsDup ? "Yes" : "No", Ord->GetOrderNID());
	CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Ord );
	ExecutionReport.SetOrderID( OrdSysIDStr.c_str() );
	ExecutionReport.SetNID( Ord->GetOrderNID() );
	if( Ord->GetLeavesQty() == 0 )
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
	ExecutionReport.SetPrice( pTrade->TradePrice );
	ExecutionReport.SetLastPx( pTrade->TradePrice );
	ExecutionReport.SetOrderQty( pTrade->TradeVolume );
	ExecutionReport.SetLastQty( pTrade->TradeVolume );
	ExecutionReport.SetTransactTime( pTrade->TradeTime );
	TrigerOnExecutionReport( &ExecutionReport,(ExecDup) IsDup );
	UFC::BufferedLog::Printf( " -------------------- Sent Fill Execution ------------------------" );
}
//---------------------------------------------------------------------------
// Femas Order Fill.
//---------------------------------------------------------------------------
void TTaifexConnection::OnFemasRtnTrade( CUstpFtdcTradeField *pTrade )
{
	if( pTrade == NULL )
		return;
	///< Use ExchangeID + OrderSysID to find Order record.
	if( strlen( pTrade->ExchangeID ) > 0 && strlen( pTrade->OrderSysID ) >0 )
	{
		UpdateRecover();
		UFC::AnsiString ExchangeID( pTrade->ExchangeID  );
		Int64           OrdSysIDInt64 = _atoi64( pTrade->OrderSysID );
		COrderInfo*     Ord;

		if( (Ord = FindOrderByOrderSysID( ExchangeID, OrdSysIDInt64 ) )!= NULL )
			FemasFillExecution( Ord, pTrade );
		else
			UFC::BufferedLog::Printf( " OnRtnTrade: Exchange[%s] OrderSysID[%lld] not found! update fill failed.", ExchangeID.c_str(), OrdSysIDInt64);
	}
	else
		UFC::BufferedLog::Printf( " OnRtnTrade: missing field OrderSysID,OrderLocalID or ExchangeID." );
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
