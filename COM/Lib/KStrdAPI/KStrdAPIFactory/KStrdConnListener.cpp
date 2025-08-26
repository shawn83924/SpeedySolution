//---------------------------------------------------------------------------
#ifdef _CTP
//------------------------------------------------------------------------------------------------------------------------
#ifndef _KSFT
//------------------------------------------------------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//------------------------------------------------------------------------------------------------------------------------
#include "KStrdConnListener.h"
#include "TTaifexConnection.h"
//------------------------------------------------------------------------------------------------------------------------
typedef AnsiStringT<936> GB2312String;
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
// class KStrdConnListener
// Implement TKStrdTraderSpi callback functions
//
//------------------------------------------------------------------------------------------------------------------------
KStrdConnListener::KStrdConnListener( TTaifexConnection* SpeedyConnPtr )
: FSpeedyConnPtr( SpeedyConnPtr )
{
}
//------------------------------------------------------------------------------------------------------------------------
KStrdConnListener::~KStrdConnListener()
{
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdAPIConnected()
{
	// this function is non trigger by min UI thread
	// Don't modify any GDI object int this function
	FSpeedyConnPtr->FIsConnected = true;
	FSpeedyConnPtr->FLogonEvent.SetEvent();
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdAPIDisconnected( int /*nReason*/ )
{
	// this function is non trigger by min UI thread
	// Don't modify any GDI object int this function
	FSpeedyConnPtr->FIsConnected = false;
	FSpeedyConnPtr->FIsLogon = false;
	FSpeedyConnPtr->DetachShm();
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
void KStrdConnListener::OnKStrdAPIRecoverFinished( int Count )
{
	if ( FSpeedyConnPtr->FListener != NULL )
		FSpeedyConnPtr->FListener->OnRecoverFinished( Count );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdFrontConnected( )
{
	if ( FSpeedyConnPtr->FListener != NULL )
	{
		if ( FSpeedyConnPtr->FTriggerCTPOnConnected == true )
		{
			UFC::SleepMS( 330 );
			FSpeedyConnPtr->FTriggerCTPOnConnected = false;
			FSpeedyConnPtr->FListener->OnConnected();
		}
		else
		{
			TNewsMessage Msg;

			Msg.SetHeadline( "KSTRD" );
			Msg.SetID( CTP_OnConnected );
			FSpeedyConnPtr->FListener->OnNews( &Msg );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdFrontDisconnected( int /*nReason*/ )
{
	if ( FSpeedyConnPtr->FListener != NULL )
	{
		FSpeedyConnPtr->FListener->OnDisconnected();
		TNewsMessage Msg;

		Msg.SetHeadline( "KSTRD");
		Msg.SetID( CTP_OnDisconnectd );
		FSpeedyConnPtr->FListener->OnNews( &Msg );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRspUserLogin( KS_::CThostFtdcRspUserLoginField *pRspUserLogin, KS_::CThostFtdcRspInfoField *pRspInfo, int /*nRequestID*/, bool /*bIsLast*/ )
{
	LogonResult  Result = lrFailed;
	AnsiString   ReplyString( "" );

	FSpeedyConnPtr->FAdmin = 0;
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 ) ///< Logon failed.
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		UTF8String   ErrMsgUTF8( ErrMsg );

		ReplyString = ErrMsgUTF8.c_str();
		Result   = lrFailed;
		FSpeedyConnPtr->FIsLogon = false;
		FSpeedyConnPtr->FCurrentConnectionID = 0;
		UFC::BufferedLog::Printf( " OnKStrdRspUserLogin: Code[%d] Msg[%s] Logon failed."
								  , pRspInfo->ErrorID, ErrMsgUTF8.c_str() );
		if ( FSpeedyConnPtr->FListener != NULL )
			FSpeedyConnPtr->FListener->OnLogonReply( ReplyString.c_str(), Result, FSpeedyConnPtr->FCurrentConnectionID );
	}
	else ///< Logon OK
	{
		// Prevent 1st parameter null reference error
		if ( pRspUserLogin == NULL )
		{
			UFC::BufferedLog::Printf( " OnKStrdRspUserLogin: pRspUserLogin is NULL." );
			UFC::BufferedLog::FlushToFile();
			return;
		}

		//UFC::AnsiString FileName;
		TNewsMessage Msg, ReqMsg;
		UFC::NameValueMessage TimeMsg("^\n");

		FSpeedyConnPtr->FTradeingDate = pRspUserLogin->TradingDay;
		FSpeedyConnPtr->FIsRecover = false;
		FSpeedyConnPtr->FRecoverCount = 0;
		FSpeedyConnPtr->FLastRecover = UFC::GetTickCountMS();
		FSpeedyConnPtr->RemoveShm( "KSTRD" );
		FSpeedyConnPtr->AttachShm( "KSTRD", pRspUserLogin->TradingDay );
		FSpeedyConnPtr->FCTPFrontID   = pRspUserLogin->FrontID;
		FSpeedyConnPtr->FCTPSessionID = pRspUserLogin->SessionID;
		Result   = lrOk;
		FSpeedyConnPtr->FIsLogon = true;
		FSpeedyConnPtr->FUseNID64 = true;
		FSpeedyConnPtr->FCurrentConnectionID = pRspUserLogin->SessionID;
		UFC::CreateDir( "./log" );
		//FileName.Printf( "./log/KStrdLog_%s_%s.log", pRspUserLogin->UserID, pRspUserLogin->TradingDay );
		//CTPRecover( FileName );
		//if ( FCTPTransLog != NULL )
		//	delete FCTPTransLog;
		//FCTPTransLog = new UFC::FileStream64( FileName.c_str(), "a" );
		UFC::BufferedLog::Printf( " OnKStrdRspUserLogin: Account[%s] FrontID[%d] SessionID[%d] MaxOrderRef[%s] Logon OK!"
								  , pRspUserLogin->UserID, pRspUserLogin->FrontID, pRspUserLogin->SessionID, pRspUserLogin->MaxOrderRef );
		if ( FSpeedyConnPtr->FListener != NULL )
			FSpeedyConnPtr->FListener->OnLogonReply( ReplyString.c_str(), Result, FSpeedyConnPtr->FCurrentConnectionID );
		// Update Exchange clocks.
		Msg.SetHeadline( "KSTRD" );
		Msg.SetID( CTP_OnExchangeTime );
		TimeMsg.Append( "SHFE",  pRspUserLogin->SHFETime );
		TimeMsg.Append( "DCE",   pRspUserLogin->DCETime );
		TimeMsg.Append( "CZCE",  pRspUserLogin->CZCETime );
		TimeMsg.Append( "CFFEX", pRspUserLogin->FFEXTime );
		// ?? //TimeMsg.Append( "INE", pRspUserLogin->INETime );
		Msg.SetText( TimeMsg.ToString().c_str() );
		if ( FSpeedyConnPtr->FListener != NULL )
			FSpeedyConnPtr->FListener->OnNews( &Msg );
		// query SettlementInfo confirm.
		ReqMsg.SetHeadline( "KSTRD" );
		ReqMsg.SetID( CTP_QuerySettlementInfoConfirm );
		FSpeedyConnPtr->HandleKStrdMessage( &ReqMsg );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRspUserLogout( KS_::CThostFtdcUserLogoutField */*pUserLogout*/, KS_::CThostFtdcRspInfoField */*pRspInfo*/, int /*nRequestID*/, bool /*bIsLast*/ )
{
	FSpeedyConnPtr->FAdmin   = 0;
	FSpeedyConnPtr->FIsLogon = false; ///< Clear logon flag.
	FSpeedyConnPtr->FID      = "";
	FSpeedyConnPtr->FToken   = "";
	FSpeedyConnPtr->DetachShm();
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
void KStrdConnListener::OnKStrdRspUserPasswordUpdate( KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int /*nRequestID*/, bool /*bIsLast*/ )
{
	TNewsMessage Msg;

	Msg.SetHeadline( "KSTRD" );
	Msg.SetID( CTP_OnPasswordChanged );
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		UTF8String   UTF8Text( ErrMsg );

		Msg.SetText( UTF8Text.c_str() );
		UFC::BufferedLog::Printf( " OnKStrdRspUserPasswordUpdate: Change Password failed! Code[%d] Msg[%s]"
								  , pRspInfo->ErrorID, UTF8Text.c_str() );
	}
	else
	{
		// Prevent 1st parameter null reference error
		if ( pUserPasswordUpdate == NULL )
		{
			UFC::BufferedLog::Printf( " OnKStrdRspUserPasswordUpdate: pUserPasswordUpdate is NULL." );
			UFC::BufferedLog::FlushToFile();
			return;
		}

		Msg.SetText( "OK" );
		UFC::BufferedLog::Printf( " OnKStrdRspUserPasswordUpdate: UserID[%s] Change Password OK! ", pUserPasswordUpdate->UserID );
	}
	if ( FSpeedyConnPtr->FListener != NULL )
		FSpeedyConnPtr->FListener->OnNews( &Msg );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRspTradingAccountPasswordUpdate( KS_::CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int /*nRequestID*/, bool /*bIsLast*/ )
{
	TNewsMessage Msg;

	Msg.SetHeadline( "KSTRD" );
	Msg.SetID( CTP_OnPasswordChanged );
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		UTF8String   UTF8Text( ErrMsg );

		Msg.SetText( UTF8Text.c_str() );
		UFC::BufferedLog::Printf( " OnKStrdRspTradingAccountPasswordUpdate: Change Password failed! Code[%d] Msg[%s]"
								  , pRspInfo->ErrorID, UTF8Text.c_str() );
	}
	else
	{
		// Prevent 1st parameter null reference error
		if ( pTradingAccountPasswordUpdate == NULL )
		{
			UFC::BufferedLog::Printf( " OnKStrdRspTradingAccountPasswordUpdate: pTradingAccountPasswordUpdate is NULL." );
			UFC::BufferedLog::FlushToFile();
			return;
		}

		Msg.SetText( "OK" );
		UFC::BufferedLog::Printf( " OnKStrdRspTradingAccountPasswordUpdate: Account[%s] Change Password OK!", pTradingAccountPasswordUpdate->AccountID );
	}
	if ( FSpeedyConnPtr->FListener != NULL )
		FSpeedyConnPtr->FListener->OnNews( &Msg );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRspSettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int /*nRequestID*/, bool /*bIsLast*/ )
{
	if ( pRspInfo == NULL || pRspInfo->ErrorID == 0 )///< Confirm OK
	{   ///< User confirmed Settlement Info.
		// Prevent 1st parameter null reference error
		if ( pSettlementInfoConfirm == NULL )
		{
			UFC::BufferedLog::Printf( " OnKStrdRspSettlementInfoConfirm: pSettlementInfoConfirm is NULL." );
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
void KStrdConnListener::OnKStrdRspQryInvestorPosition( KS_::CThostFtdcInvestorPositionField */*pInvestorPosition*/, KS_::CThostFtdcRspInfoField */*pRspInfo*/, int /*nRequestID*/, bool /*bIsLast*/ )
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
		UFC::BufferedLog::Printf( " OnKStrdRspQryInvestorPosition:%s %s %s PosDate=%c Dir=%c YDPos=%d Pos=%d TradeDate=%s",
								  ansiBrokerId.c_str(), ansiInvestorId.c_str(), ansiInstrumentId.c_str(), positionDate, positionDirection, ydPosition, tdPosition, tradingDay.c_str() );
		UFC::BufferedLog::Printf( " OnKStrdRspQryInvestorPosition:Open[Vol=%d Amt=%15.6lf] Close[Vol=%d Amt=%15.6lf] Cost[%15.6lf] Fee[%15.6lf] CloseProfit[%15.6lf] PosProfit[%15.6lf] TodayPos[%d]",
								  openVolume, openAmount, closeVolume, closeAmount, positionCost, commission, closeProfit, positionProfit, todayPosition );
		UFC::BufferedLog::Printf( " OnKStrdRspQryInvestorPosition:preSettlementPx[%15.6lf] SettlementPx[%15.6lf]",
								  preSettlementPrice, settlementPrice );
		UFC::BufferedLog::Printf( " OnKStrdRspQryInvestorPosition:[%s]", ansiResponseStr.c_str() );
		UFC::BufferedLog::FlushToFile();
	}

	if ( FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSTRD" );
		newsMsg.SetID( CTP_OnInvestorPosition );
		newsMsg.SetText( ansiResponseStr.c_str() );
		FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( "KStrdConnListener::OnKStrdRspQryInvestorPosition() %s", ansiResponseStr.c_str() );
*/
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRspQryTradingAccount( KS_::CThostFtdcTradingAccountField *pTradingAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int /*nRequestID*/, bool bIsLast )
{
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		UFC::BufferedLog::Printf( " OnKStrdRspQryTradingAccount failed code:%d", pRspInfo->ErrorID );
		return;
	}
	if ( pTradingAccount != NULL && bIsLast )
	{
		TNewsMessage Msg;
		UFC::NameValueMessage BalanceMsg("^\n");

		Msg.SetHeadline( "KSTRD");
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

		FSpeedyConnPtr->FBankAccount = pTradingAccount->AccountID;
		Msg.SetText( BalanceMsg.ToString().c_str() );
		if ( FSpeedyConnPtr->FListener != NULL )
			FSpeedyConnPtr->FListener->OnNews( &Msg );
		UFC::BufferedLog::Printf( "KStrdConnListener::OnKStrdRspQryTradingAccount [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f]"
								  , pTradingAccount->PreBalance, pTradingAccount->Withdraw, pTradingAccount->Deposit, pTradingAccount->CloseProfit
								  , pTradingAccount->PositionProfit, pTradingAccount->Commission, pTradingAccount->CurrMargin, pTradingAccount->FrozenMargin
								  , pTradingAccount->FrozenCommission, pTradingAccount->Available, pTradingAccount->PreCredit, pTradingAccount->PreMortgage
								  , pTradingAccount->Mortgage, pTradingAccount->DeliveryMargin, pTradingAccount->Credit, pTradingAccount->WithdrawQuota );
		UFC::BufferedLog::Printf( "KStrdConnListener::OnKStrdRspQryTradingAccount [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f]"
								  , pTradingAccount->ReserveBalance , pTradingAccount->CurrencyID , pTradingAccount->PreFundMortgageIn , pTradingAccount->PreFundMortgageOut
								  , pTradingAccount->FundMortgageIn , pTradingAccount->FundMortgageOut , pTradingAccount->FundMortgageAvailable , pTradingAccount->MortgageableFund
								  , pTradingAccount->SpecProductMargin , pTradingAccount->SpecProductFrozenMargin );
		UFC::BufferedLog::Printf( "KStrdConnListener::OnKStrdRspQryTradingAccount [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f] [%15.6f]"
								  , pTradingAccount->SpecProductCommission , pTradingAccount->SpecProductFrozenCommission , pTradingAccount->SpecProductPositionProfit , pTradingAccount->SpecProductCloseProfit
								  , pTradingAccount->SpecProductPositionProfitByAlg , pTradingAccount->SpecProductExchangeMargin );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRspQryTradingCode( KS_::CThostFtdcTradingCodeField *pTradingCode, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	UFC::AnsiString ansiResponseStr = FSpeedyConnPtr->BuildKS_ResponseInformationStr( pRspInfo, "|" );
	FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, nRequestID, "|" );
	FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, bIsLast, "|" );
	if ( pTradingCode != NULL )
	{
		UFC::AnsiString ansiInvestorId = pTradingCode->InvestorID; //5.投資者代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiInvestorId, "|" );
		UFC::AnsiString ansiBrokerId = pTradingCode->BrokerID; //6.經紀公司代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerId, "|" );
		UFC::AnsiString ansiExchangeId = pTradingCode->ExchangeID; //7.交易所代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiExchangeId, "|" );
		UFC::AnsiString ansiClientId = pTradingCode->ClientID; //8.交易編碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiClientId, "|" );
		bool isActive; //9.是否活躍
		if ( pTradingCode->IsActive == 1 )
			isActive = true;
		else
			isActive = false;
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, isActive, "|" );
		char clientIDType = pTradingCode->ClientIDType; //10.交易編碼類型  '1':投機 '2':套利 '3':套保
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, clientIDType, "|" );
	}

	if ( FSpeedyConnPtr->FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSTRD" );
		newsMsg.SetID( CTP_OnRspQueryTradingCode );
		newsMsg.SetText( ansiResponseStr.c_str() );
		FSpeedyConnPtr->FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( " KStrdConnListener::OnKStrdRspQryTradingCode() %s", ansiResponseStr.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRspQryInstrumentCommissionRate( KS_::CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	UFC::AnsiString ansiResponseStr = FSpeedyConnPtr->BuildKS_ResponseInformationStr( pRspInfo, "|" );
	FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, nRequestID, "|" );
	FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, bIsLast, "|" );
	if ( pInstrumentCommissionRate != NULL )
	{
		UFC::AnsiString ansiInstrumentId = pInstrumentCommissionRate->InstrumentID;  // 5.合約代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiInstrumentId, "|" );
		char investorRange = pInstrumentCommissionRate->InvestorRange;  // 6.投資者範圍  '1':所有 '2':投資者組 '3':單一投資者
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, investorRange, "|" );
		UFC::AnsiString ansiBrokerId = pInstrumentCommissionRate->BrokerID;  // 7.經紀公司代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerId, "|" );
		UFC::AnsiString ansiInvestorId = pInstrumentCommissionRate->InvestorID;  // 8.投資者代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiInvestorId, "|" );
		double openRatioByMoney = pInstrumentCommissionRate->OpenRatioByMoney;  // 9.開倉手續費率
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, openRatioByMoney, "|" );
		double openRatioByVolume = pInstrumentCommissionRate->OpenRatioByVolume;  //10.開倉手續費
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, openRatioByVolume, "|" );
		double closeRatioByMoney = pInstrumentCommissionRate->CloseRatioByMoney;  //11.平倉手續費率
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, closeRatioByMoney, "|" );
		double closeRatioByVolume = pInstrumentCommissionRate->CloseRatioByVolume;  //12.平倉手續費
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, closeRatioByVolume, "|" );
		double closeTodayRatioByMoney = pInstrumentCommissionRate->CloseTodayRatioByMoney;  //13.平今手續費率
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, closeTodayRatioByMoney, "|" );
		double closeTodayRatioByVolume = pInstrumentCommissionRate->CloseTodayRatioByVolume;  //14.平今手續費
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, closeTodayRatioByVolume, "|" );
	}

	if ( FSpeedyConnPtr->FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSTRD" );
		newsMsg.SetID( CTP_OnRspQryInstrumentCommissionRate );
		newsMsg.SetText( ansiResponseStr.c_str() );
		FSpeedyConnPtr->FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( " KStrdConnListener::OnKStrdRspQryInstrumentCommissionRate() %s", ansiResponseStr.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRspQrySettlementInfo( KS_::CThostFtdcSettlementInfoField *pSettlementInfo, KS_::CThostFtdcRspInfoField *pRspInfo, int /*nRequestID*/, bool bIsLast )
{
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		TNewsMessage Msg;

		UTF8String UTF8Text( ErrMsg );
		Msg.SetHeadline( "KSTRD" );
		Msg.SetID( CTP_OnSettlementInfo );
		Msg.SetText( UTF8Text.c_str() );
		if( FSpeedyConnPtr->FListener != NULL )
			FSpeedyConnPtr->FListener->OnNews( &Msg );
		return;
	}
	if ( ( pSettlementInfo != NULL ) && ( std::strlen(pSettlementInfo->Content) > 0 ) )
		FSpeedyConnPtr->FSettlementInfo.Add( pSettlementInfo->Content );

	if ( bIsLast )
	{
		GB2312String Text;
		TNewsMessage Msg;

		Msg.SetHeadline( "KSTRD" );
		Msg.SetID( CTP_OnSettlementInfo );
		if ( FSpeedyConnPtr->FSettlementInfo.ItemCount() > 0 )
		{
			for ( int i =0; i < FSpeedyConnPtr->FSettlementInfo.ItemCount(); i ++ )
				Text += GB2312String( FSpeedyConnPtr->FSettlementInfo[i].c_str() );
			UTF8String UTF8Text( Text );
			Msg.SetText( UTF8Text.c_str() );
		}
		else
			Msg.SetText( "-----------------------------------------------------------------" );
		if ( FSpeedyConnPtr->FListener != NULL )
			FSpeedyConnPtr->FListener->OnNews( &Msg );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRspQryTransferBank( KS_::CThostFtdcTransferBankField *pTransferBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	UFC::AnsiString ansiResponseStr = FSpeedyConnPtr->BuildKS_ResponseInformationStr( pRspInfo, "|" );
	FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, nRequestID, "|" );
	FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, bIsLast, "|" );
	if ( pTransferBank != NULL )
	{
		UFC::AnsiString ansiBankId = pTransferBank->BankID;  //5.銀行代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBankId, "|" );
		UFC::AnsiString ansiBankBranchId = pTransferBank->BankBrchID;  //6.銀行分中心代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBankBranchId, "|" );
		GB2312String gb2312BankName( pTransferBank->BankName );  //7.銀行名稱
		UTF8String utf8BankName( gb2312BankName );
		UFC::AnsiString ansiBankName( utf8BankName.c_str() );
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBankName, "|" );
		int isActive = pTransferBank->IsActive;  //8.是否活躍
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, isActive, "|" );
	}

	if ( FSpeedyConnPtr->FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSTRD" );
		newsMsg.SetID( CTP_OnRspQueryTransferBank );
		newsMsg.SetText( ansiResponseStr.c_str() );
		FSpeedyConnPtr->FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( " KStrdConnListener::OnKStrdRspQryTransferBank() %s", ansiResponseStr.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRspQryInvestorPositionDetail( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int /*nRequestID*/, bool /*bIsLast*/ )
{
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		UTF8String UTF8Text( ErrMsg );
		UFC::BufferedLog::Printf( " OnKStrdRspQryInvestorPositionDetail: %d[%s]", pRspInfo->ErrorID, UTF8Text.c_str() );
		UFC::BufferedLog::FlushToFile();
		return;
	}

	// Prevent 1st parameter null reference error
	if ( pInvestorPositionDetail == NULL )
	{
		UFC::BufferedLog::Printf( " OnKStrdRspQryInvestorPositionDetail: pInvestorPositionDetail is NULL." );
		UFC::BufferedLog::FlushToFile();
		return;
	}
	UFC::BufferedLog::Printf( " OnKStrdRspQryInvestorPositionDetail: %s %s %s Direction=%c OpenDate=%s TradeDate=%s Vol=%d OpenPx=%15.6lf CloseVol=%d TradeType=%c",
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
	if ( FSpeedyConnPtr->FListener != NULL )
	{
		UFC::AnsiString newsText;
		newsText.Printf( "%s|%s|%s|%c|%s|%d|%15.6lf|%d|%s",
						 pInvestorPositionDetail->BrokerID, pInvestorPositionDetail->InvestorID, pInvestorPositionDetail->InstrumentID,
						 pInvestorPositionDetail->Direction, pInvestorPositionDetail->OpenDate,
						 pInvestorPositionDetail->Volume, pInvestorPositionDetail->OpenPrice,
						 pInvestorPositionDetail->CloseVolume,
						 pInvestorPositionDetail->TradingDay );
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSTRD" );
		newsMsg.SetID( CTP_OnInvestorPositionDetail );
		newsMsg.SetText( newsText.c_str() );
		FSpeedyConnPtr->FListener->OnNews( &newsMsg );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRspQrySettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int /*nRequestID*/, bool /*bIsLast*/ )
{
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		UFC::BufferedLog::Printf( " OnKStrdRspQrySettlementInfoConfirm failed code:%d", pRspInfo->ErrorID );
		UFC::BufferedLog::FlushToFile();
		return;
	}
	if ( pSettlementInfoConfirm != NULL ) ///< Already confirmed.
	{
		UFC::BufferedLog::Printf( " OnKStrdRspQrySettlementInfoConfirm: User confirm settlement Info at:%s", pSettlementInfoConfirm->ConfirmTime );
		UFC::BufferedLog::FlushToFile();
		return;
	}
	else ///< Request Settlement Info
	{
		UFC::BufferedLog::Printf( " OnKStrdRspQrySettlementInfoConfirm: User not confirm Settlement Info." );
		KS_::CThostFtdcQrySettlementInfoField QuerySettlementData;

		FSpeedyConnPtr->FSettlementInfo.Clear();
		memset( &QuerySettlementData, 0, sizeof(KS_::CThostFtdcQrySettlementInfoField) );
		strcpy( QuerySettlementData.BrokerID,   FSpeedyConnPtr->FCFFEXBrokerID.c_str() );
		strcpy( QuerySettlementData.InvestorID, FSpeedyConnPtr->FID.c_str() );
		while ( TRUE )
		{
			int Rtn = FSpeedyConnPtr->FKSTRDAPI->ReqQrySettlementInfo( &QuerySettlementData, FSpeedyConnPtr->CTPReqID() );
			if( Rtn == -2 || Rtn == -3 )
				UFC::SleepMS(300);
			else
				break;
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRspQryTransferSerial( KS_::CThostFtdcTransferSerialField *pTransferSerial, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	UFC::AnsiString ansiResponseStr = FSpeedyConnPtr->BuildKS_ResponseInformationStr( pRspInfo, "|" );
	FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, nRequestID, "|" );
	FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, bIsLast, "|" );
	if ( pTransferSerial != NULL )
	{
		int plateSerial = pTransferSerial->PlateSerial;  // 5.平台流水號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, plateSerial, "|" );
		UFC::AnsiString ansiTradeDate = pTransferSerial->TradeDate;  // 6.交易發起方日期
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiTradeDate, "|" );
		UFC::AnsiString ansiTradingDay =  pTransferSerial->TradingDay;  // 7.交易日期
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiTradingDay, "|" );
		UFC::AnsiString ansiTradeTime = pTransferSerial->TradeTime;  // 8.交易時間
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiTradeTime, "|" );
		UFC::AnsiString ansiTradeCode = pTransferSerial->TradeCode;  // 9.交易代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiTradeCode, "|" );
		int sessionID = pTransferSerial->SessionID;  //10.會話編號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, sessionID, "|" );
		UFC::AnsiString ansiBankID = pTransferSerial->BankID;  //11.銀行編碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBankID, "|" );
		UFC::AnsiString ansiBankBranchID = pTransferSerial->BankBranchID;  //12.銀行分支機構編碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBankBranchID, "|" );
		char bankAccType = pTransferSerial->BankAccType;  //13.銀行帳號類型  '1':存摺 '2':儲蓄卡 '3':信用卡
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, bankAccType, "|" );
		UFC::AnsiString ansiBankAccount = pTransferSerial->BankAccount;  //14.銀行帳號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBankAccount, "|" );
		UFC::AnsiString ansiBankSerial = pTransferSerial->BankSerial;  //15.銀行流水號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBankSerial, "|" );
		UFC::AnsiString ansiBrokerId = pTransferSerial->BrokerID;  //16.期貨公司編碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerId, "|" );
		UFC::AnsiString ansiBrokerBranchId = pTransferSerial->BrokerBranchID;  //17.期貨公司分支機構代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerBranchId, "|" );
		char futureAccType = pTransferSerial->FutureAccType;  //18.期貨公司帳號類型 '1':存摺 '2':儲蓄卡 '3':信用卡
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, futureAccType, "|" );
		UFC::AnsiString ansiAccountId = pTransferSerial->AccountID;  //19.投資者帳號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiAccountId, "|" );
		UFC::AnsiString ansiInvestorId = pTransferSerial->InvestorID;  //20.投資者代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiInvestorId, "|" );
		int futureSerial = pTransferSerial->FutureSerial;  //21.期貨公司流水號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, futureSerial, "|" );
		char idCardType = pTransferSerial->IdCardType;  //22.證件類型
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, idCardType, "|" );
		UFC::AnsiString ansiIdentifiedCardNo = pTransferSerial->IdentifiedCardNo;  //23.證件號碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiIdentifiedCardNo, "|" );
		UFC::AnsiString ansiCurrencyId = pTransferSerial->CurrencyID;  //24.幣種代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiCurrencyId, "|" );
		double tradeAmount = pTransferSerial->TradeAmount;  //25.交易金額
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, tradeAmount, "|" );
		double customFee = pTransferSerial->CustFee;  //26.應收客戶費用
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, customFee, "|" );
		double brokerFee = pTransferSerial->BrokerFee;  //27.應收期貨公司費用
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, brokerFee, "|" );
		char availabilityFlag = pTransferSerial->AvailabilityFlag;  //28.有效標誌 '0':未確認 '1':有效 '2':沖正
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, availabilityFlag, "|" );
		UFC::AnsiString ansiOperatorCode = pTransferSerial->OperatorCode;  //29.操作員
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiOperatorCode, "|" );
		UFC::AnsiString ansiBankNewAccount = pTransferSerial->BankNewAccount;  //30.新銀行帳號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBankNewAccount, "|" );
		int errorID = pTransferSerial->ErrorID;  //31.錯誤代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, errorID, "|" );
		GB2312String gb2312ErrorMsg( pTransferSerial->ErrorMsg );  //32.錯誤訊息
		UTF8String utf8ErrorMsg( gb2312ErrorMsg );
		UFC::AnsiString ansiErrorMsg( utf8ErrorMsg.c_str() );
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiErrorMsg, "|" );
	}

	if ( FSpeedyConnPtr->FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSTRD" );
		newsMsg.SetID( CTP_OnRspQryTransferSerial );
		newsMsg.SetText( ansiResponseStr.c_str() );
		FSpeedyConnPtr->FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( " KStrdConnListener::OnKStrdRspQryTransferSerial() %s", ansiResponseStr.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRspQryAccountregister( KS_::CThostFtdcAccountregisterField *pAccountregister, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	UFC::AnsiString ansiResponseStr = FSpeedyConnPtr->BuildKS_ResponseInformationStr( pRspInfo, "|" );
	FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, nRequestID, "|" );
	FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, bIsLast, "|" );
	if ( pAccountregister != NULL )
	{
		UFC::AnsiString ansiTradeDate = pAccountregister->TradeDay;  // 5.交易日期
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiTradeDate, "|" );
		UFC::AnsiString ansiBankId = pAccountregister->BankID;  // 6.銀行代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBankId, "|" );
		UFC::AnsiString ansiBankBranchId = pAccountregister->BankBranchID;  // 7.銀行分支機構代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBankBranchId, "|" );
		UFC::AnsiString ansiBankAccount = pAccountregister->BankAccount;  // 8.銀行帳號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBankAccount, "|" );
		UFC::AnsiString ansiBrokerId = pAccountregister->BrokerID;  // 9.期貨公司編碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerId, "|" );
		UFC::AnsiString ansiBrokerBranchId = pAccountregister->BrokerBranchID;  //10.期貨公司分支機構編碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerBranchId, "|" );
		UFC::AnsiString ansiAccountId = pAccountregister->AccountID;  //11.投資者帳號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiAccountId, "|" );
		char idCardType = pAccountregister->IdCardType;  //12.證件類型
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, idCardType, "|" );
		UFC::AnsiString ansiIdentifiedCardNo = pAccountregister->IdentifiedCardNo;  //13.證件號碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiIdentifiedCardNo, "|" );
		GB2312String gb2312CustomerName( pAccountregister->CustomerName );  //14.客戶姓名
		UTF8String utf8CustomerName( gb2312CustomerName );
		UFC::AnsiString ansiCustomerName( utf8CustomerName.c_str() );
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiCustomerName, "|" );
		UFC::AnsiString ansiCurrencyId = pAccountregister->CurrencyID;  //15.幣種
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiCurrencyId, "|" );
		char openOrDestroy = pAccountregister->OpenOrDestroy;  //16.開銷戶類別 '1':開戶 '0':銷戶
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, openOrDestroy, "|" );
		UFC::AnsiString ansiRegisterDate = pAccountregister->RegDate;  //17.簽約日期
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiRegisterDate, "|" );
		UFC::AnsiString ansiCancelDate = pAccountregister->OutDate;  //18.解約日期
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiCancelDate, "|" );
		int tradeId = pAccountregister->TID;  //19.交易ID
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, tradeId, "|" );
		char customType = pAccountregister->CustType;  //20.客戶類型 '0':自然人 '1':機構戶
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, customType, "|" );
		char bankAccountType = pAccountregister->BankAccType;  //21.銀行帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, bankAccountType, "|" );
	}

	if ( FSpeedyConnPtr->FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSTRD" );
		newsMsg.SetID( CTP_OnRspQueryAccountRegister );
		newsMsg.SetText( ansiResponseStr.c_str() );
		FSpeedyConnPtr->FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( " KStrdConnListener::OnKStrdRspQryAccountregister() %s", ansiResponseStr.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRtnInstrumentStatus( KS_::CThostFtdcInstrumentStatusField *pInstrumentStatus )
{
	TNewsMessage Msg;
	UFC::NameValueMessage StatusMsg("^\n");

	if ( pInstrumentStatus != NULL )
	{
		Msg.SetHeadline( "KSTRD" );
		Msg.SetID( CTP_OnExchangeStatus );
		StatusMsg.Append( "Exchange",  pInstrumentStatus->ExchangeID );
		StatusMsg.Append( "Status",  UFC::AnsiString(pInstrumentStatus->InstrumentStatus) );
		Msg.SetText( StatusMsg.ToString().c_str() );
		if ( FSpeedyConnPtr->FListener != NULL )
			FSpeedyConnPtr->FListener->OnNews( &Msg );
		UFC::BufferedLog::Printf( " OnKStrdRtnInstrumentStatus Exchange[%s] Time[%s] Status[%c] "
								  , pInstrumentStatus->ExchangeID, pInstrumentStatus->EnterTime, pInstrumentStatus->InstrumentStatus );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRspQryContractBank( KS_::CThostFtdcContractBankField *pContractBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	UFC::AnsiString ansiResponseStr = FSpeedyConnPtr->BuildKS_ResponseInformationStr( pRspInfo, "|" );
	FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, nRequestID, "|" );
	FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, bIsLast, "|" );
	if ( pContractBank != NULL )
	{
		UFC::AnsiString ansiBrokerId = pContractBank->BrokerID;  //5.經紀公司代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerId, "|" );
		UFC::AnsiString ansiBankId = pContractBank->BankID;  //6.銀行代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBankId, "|" );
		UFC::AnsiString ansiBankBranchId = pContractBank->BankBrchID;  //7.銀行分中心代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBankBranchId, "|" );
		GB2312String gb2312BankName( pContractBank->BankName );  //8.銀行名稱
		UTF8String utf8BankName( gb2312BankName );
		UFC::AnsiString ansiBankName( utf8BankName.c_str() );
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBankName, "|" );
	}

	if ( FSpeedyConnPtr->FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSTRD" );
		newsMsg.SetID( CTP_OnRspQueryContractBank );
		newsMsg.SetText( ansiResponseStr.c_str() );
		FSpeedyConnPtr->FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( " KStrdConnListener::OnKStrdRspQryContractBank() %s", ansiResponseStr.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRtnFromBankToFutureByFuture( KS_::CThostFtdcRspTransferField *pRspTransfer )
{
	// Prevent 1st parameter null reference error
	if ( pRspTransfer == NULL )
	{
		UFC::BufferedLog::Printf( " OnKStrdRtnFromBankToFutureByFuture: pRspTransfer is NULL." );
		UFC::BufferedLog::FlushToFile();
		return;
	}

	if ( FSpeedyConnPtr->FListener != NULL )
	{
		UFC::AnsiString newsText = FSpeedyConnPtr->BuildKS_TransferResponseStr( pRspTransfer );
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSTRD" );
		newsMsg.SetID( CTP_OnRtnFromBankToFutureByFuture );
		newsMsg.SetText( newsText.c_str() );
		FSpeedyConnPtr->FListener->OnNews( &newsMsg );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRtnFromFutureToBankByFuture( KS_::CThostFtdcRspTransferField *pRspTransfer )
{
	// Prevent 1st parameter null reference error
	if ( pRspTransfer == NULL )
	{
		UFC::BufferedLog::Printf(" OnKStrdRtnFromFutureToBankByFuture: pRspTransfer is NULL.");
		UFC::BufferedLog::FlushToFile();
		return;
	}

	if ( FSpeedyConnPtr->FListener != NULL )
	{
		UFC::AnsiString newsText = FSpeedyConnPtr->BuildKS_TransferResponseStr( pRspTransfer );
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSTRD" );
		newsMsg.SetID( CTP_OnRtnFromFutureToBankByFuture );
		newsMsg.SetText( newsText.c_str() );
		FSpeedyConnPtr->FListener->OnNews( &newsMsg );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRtnQueryBankBalanceByFuture( KS_::CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount )
{
	UFC::AnsiString ansiResultStr = "";
	if ( pNotifyQueryAccount != NULL )
	{
		int errorId = pNotifyQueryAccount->ErrorID;  //1.錯誤代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, errorId, "|" );
		GB2312String gb2312ErrorMsg( pNotifyQueryAccount->ErrorMsg );  //2.錯誤訊息
		UTF8String utf8ErrorMsg( gb2312ErrorMsg );
		UFC::AnsiString ansiErrorMsg( utf8ErrorMsg.c_str() );
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiErrorMsg, "|" );
		UFC::AnsiString ansiTradeCode = pNotifyQueryAccount->TradeCode;  // 3.業務功能碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiTradeCode, "|" );
		UFC::AnsiString ansiBankId = pNotifyQueryAccount->BankID;  // 4.銀行代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiBankId, "|" );
		UFC::AnsiString ansiBankBranchId = pNotifyQueryAccount->BankBranchID;  // 5.銀行分支機構代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiBankBranchId, "|" );
		UFC::AnsiString ansiBrokerId = pNotifyQueryAccount->BrokerID;  // 6.期商代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiBrokerId, "|" );
		UFC::AnsiString ansiBrokerBranchId = pNotifyQueryAccount->BrokerBranchID;  // 7.期商分支機構代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiBrokerBranchId, "|" );
		UFC::AnsiString ansiTradeDate =  pNotifyQueryAccount->TradeDate;  // 8.交易日期
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiTradeDate, "|" );
		UFC::AnsiString ansiTradeTime = pNotifyQueryAccount->TradeTime;  // 9.交易時間
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiTradeTime, "|" );
		UFC::AnsiString ansiBankSerial = pNotifyQueryAccount->BankSerial;  //10.銀行流水號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiBankSerial, "|" );
		UFC::AnsiString ansiSystemTradeDate = pNotifyQueryAccount->TradingDay;  //11.系統交易日期
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiSystemTradeDate, "|" );
		int plateSerial = pNotifyQueryAccount->PlateSerial;  //12.銀期平台消息流水號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, plateSerial, "|" );
		char lastFragment = pNotifyQueryAccount->LastFragment;  //13.最後分片標誌
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, lastFragment, "|" );
		int sessionId = pNotifyQueryAccount->SessionID;  //14.會話號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, sessionId, "|" );
		GB2312String gb2312CustomerName( pNotifyQueryAccount->CustomerName );  //15.客戶姓名
		UTF8String utf8CustomerName( gb2312CustomerName );
		UFC::AnsiString ansiCustomerName( utf8CustomerName.c_str() );
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiCustomerName, "|" );
		char idCardType = pNotifyQueryAccount->IdCardType;  //16.證件類型
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, idCardType, "|" );
		UFC::AnsiString ansiIdentifiedCardNo = pNotifyQueryAccount->IdentifiedCardNo;  //17.證件號碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiIdentifiedCardNo, "|" );
		char customType = pNotifyQueryAccount->CustType;  //18.客戶類型
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, customType, "|" );
		UFC::AnsiString ansiBankAccount = pNotifyQueryAccount->BankAccount;  //19.銀行帳號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiBankAccount, "|" );
		UFC::AnsiString ansiBankPassword = pNotifyQueryAccount->BankPassWord;  //20.銀行密碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiBankPassword, "|" );
		UFC::AnsiString ansiAccountId = pNotifyQueryAccount->AccountID;  //21.投資者帳號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiAccountId, "|" );
		UFC::AnsiString ansiAccountPassword = pNotifyQueryAccount->Password;  //22.期貨密碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiAccountPassword, "|" );
		int futureSerial = pNotifyQueryAccount->FutureSerial;  //23.期貨公司流水號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, futureSerial, "|" );
		int installID = pNotifyQueryAccount->InstallID;  //24.安裝編號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, installID, "|" );
		UFC::AnsiString ansiUserID = pNotifyQueryAccount->UserID;  //25.用戶標誌
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiUserID, "|" );
		char verifyCertNoFlag = pNotifyQueryAccount->VerifyCertNoFlag;  //26.驗證客戶證件號碼標誌 '0':是 '1':否
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, verifyCertNoFlag, "|" );
		UFC::AnsiString ansiCurrencyId = pNotifyQueryAccount->CurrencyID;  //27.幣種
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiCurrencyId, "|" );
		UFC::AnsiString ansiDigest = pNotifyQueryAccount->Digest;  //28.摘要
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiDigest, "|" );
		char bankAccountType = pNotifyQueryAccount->BankAccType;  //29.銀行帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, bankAccountType, "|" );
		UFC::AnsiString ansiDeviceId = pNotifyQueryAccount->DeviceID;  //30.渠道標誌
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiDeviceId, "|" );
		char bankSecurityAccountType = pNotifyQueryAccount->BankSecuAccType;  //31.期貨單位帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, bankSecurityAccountType, "|" );
		UFC::AnsiString ansiBrokerIDByBank = pNotifyQueryAccount->BrokerIDByBank;  //32.期貨公司銀行編碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiBrokerIDByBank, "|" );
		UFC::AnsiString ansiBankSecurityAccount = pNotifyQueryAccount->BankSecuAcc;  //33.期貨單位帳號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiBankSecurityAccount, "|" );
		char bankPasswordFlag = pNotifyQueryAccount->BankPwdFlag;  //34.銀行密碼標誌 '0':不核對  '1':明文核對  '2':密文核對
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, bankPasswordFlag, "|" );
		char securityPasswordFlag = pNotifyQueryAccount->SecuPwdFlag;  //35.期貨資金密碼核對標誌 '0':不核對  '1':明文核對  '2':密文核對
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, securityPasswordFlag, "|" );
		UFC::AnsiString ansiOperatorNo = pNotifyQueryAccount->OperNo;  //36.交易櫃員
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiOperatorNo, "|" );
		int requestId = pNotifyQueryAccount->RequestID;  //37.請求編號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, requestId, "|" );
		int tradeId = pNotifyQueryAccount->TID;  //38.交易ID
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, tradeId, "|" );
		double bankUseAmount = pNotifyQueryAccount->BankUseAmount;  //39.銀行可用金額
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, bankUseAmount, "|" );
		double bankFetchAmount = pNotifyQueryAccount->BankFetchAmount;  //40.銀行可取金額
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, bankFetchAmount, "|" );
	}

	if ( FSpeedyConnPtr->FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSTRD" );
		newsMsg.SetID( CTP_OnRtnQueryBankBalanceByFuture );
		newsMsg.SetText( ansiResultStr.c_str() );
		FSpeedyConnPtr->FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( " KStrdConnListener::OnKStrdRtnQueryBankBalanceByFuture() %s", ansiResultStr.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRspFromBankToFutureByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	UFC::AnsiString ansiNewsText = FSpeedyConnPtr->BuildKS_TransferRequestStr( pRspInfo, nRequestID, bIsLast, pReqTransfer );
	if ( FSpeedyConnPtr->FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSTRD" );
		newsMsg.SetID( CTP_OnRspFromBankToFutureByFuture );
		newsMsg.SetText( ansiNewsText.c_str() );
		FSpeedyConnPtr->FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( " KStrdConnListener::OnKStrdRspFromBankToFutureByFuture() %s", ansiNewsText.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRspFromFutureToBankByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	UFC::AnsiString ansiNewsText = FSpeedyConnPtr->BuildKS_TransferRequestStr( pRspInfo, nRequestID, bIsLast, pReqTransfer );
	if ( FSpeedyConnPtr->FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSTRD" );
		newsMsg.SetID( CTP_OnRspFromFutureToBankByFuture );
		newsMsg.SetText( ansiNewsText.c_str() );
		FSpeedyConnPtr->FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( " KStrdConnListener::OnKStrdRspFromFutureToBankByFuture() %s", ansiNewsText.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRspQueryBankAccountMoneyByFuture( KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	UFC::AnsiString ansiResponseStr = FSpeedyConnPtr->BuildKS_ResponseInformationStr( pRspInfo, "|" );
	FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, nRequestID, "|" );
	FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, bIsLast, "|" );
	if ( pReqQueryAccount != NULL )
	{
		UFC::AnsiString ansiTradeCode = pReqQueryAccount->TradeCode;  // 5.業務功能碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiTradeCode, "|" );
		UFC::AnsiString ansiBankId = pReqQueryAccount->BankID;  // 6.銀行代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBankId, "|" );
		UFC::AnsiString ansiBankBranchId = pReqQueryAccount->BankBranchID;  // 7.銀行分支機構代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBankBranchId, "|" );
		UFC::AnsiString ansiBrokerId = pReqQueryAccount->BrokerID;  // 8.期商代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerId, "|" );
		UFC::AnsiString ansiBrokerBranchId = pReqQueryAccount->BrokerBranchID;  // 9.期商分支機構代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerBranchId, "|" );
		UFC::AnsiString ansiTradeDate = pReqQueryAccount->TradeDate;  //10.交易日期
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiTradeDate, "|" );
		UFC::AnsiString ansiTradeTime = pReqQueryAccount->TradeTime;  //11.交易時間
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiTradeTime, "|" );
		UFC::AnsiString ansiBankSerial = pReqQueryAccount->BankSerial;  //12.銀行流水號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBankSerial, "|" );
		UFC::AnsiString ansiSystemTradeDate = pReqQueryAccount->TradingDay;  //13.系統交易日期
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiSystemTradeDate, "|" );
		int plateSerial = pReqQueryAccount->PlateSerial;  //14.銀期平台消息流水號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, plateSerial, "|" );
		char lastFragment = pReqQueryAccount->LastFragment;  //15.最後分片標誌
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, lastFragment, "|" );
		int sessionId = pReqQueryAccount->SessionID;  //16.會話號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, sessionId, "|" );
		GB2312String gb2312CustomerName( pReqQueryAccount->CustomerName );  //17.客戶姓名
		UTF8String utf8CustomerName( gb2312CustomerName );
		UFC::AnsiString ansiCustomerName( utf8CustomerName.c_str() );
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiCustomerName, "|" );
		char idCardType = pReqQueryAccount->IdCardType;  //18.證件類型
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, idCardType, "|" );
		UFC::AnsiString ansiIdentifiedCardNo = pReqQueryAccount->IdentifiedCardNo;  //19.證件號碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiIdentifiedCardNo, "|" );
		char customType = pReqQueryAccount->CustType;  //20.客戶類型
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, customType, "|" );
		UFC::AnsiString ansiBankAccount = pReqQueryAccount->BankAccount;  //21.銀行帳號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBankAccount, "|" );
		UFC::AnsiString ansiBankPassword = pReqQueryAccount->BankPassWord;  //22.銀行密碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBankPassword, "|" );
		UFC::AnsiString ansiAccountId = pReqQueryAccount->AccountID;  //23.投資者帳號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiAccountId, "|" );
		UFC::AnsiString ansiAccountPassword = pReqQueryAccount->Password;  //24.期貨密碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiAccountPassword, "|" );
		int installID = pReqQueryAccount->InstallID;  //25.安裝編號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, installID, "|" );
		UFC::AnsiString ansiUserID = pReqQueryAccount->UserID;  //26.用戶標誌
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiUserID, "|" );
		char verifyCertNoFlag = pReqQueryAccount->VerifyCertNoFlag;  //27.驗證客戶證件號碼標誌 '0':是 '1':否
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, verifyCertNoFlag, "|" );
		UFC::AnsiString ansiCurrencyId = pReqQueryAccount->CurrencyID;  //28.幣種
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiCurrencyId, "|" );
		UFC::AnsiString ansiDigest = pReqQueryAccount->Digest;  //29.摘要
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiDigest, "|" );
		char bankAccountType = pReqQueryAccount->BankAccType;  //30.銀行帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, bankAccountType, "|" );
		UFC::AnsiString ansiDeviceId = pReqQueryAccount->DeviceID;  //31.渠道標誌
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiDeviceId, "|" );
		char bankSecurityAccountType = pReqQueryAccount->BankSecuAccType;  //32.期貨單位帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, bankSecurityAccountType, "|" );
		UFC::AnsiString ansiBrokerIDByBank = pReqQueryAccount->BrokerIDByBank;  //33.期貨公司銀行編碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerIDByBank, "|" );
		UFC::AnsiString ansiBankSecurityAccount = pReqQueryAccount->BankSecuAcc;  //34.期貨單位帳號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBankSecurityAccount, "|" );
		char bankPasswordFlag = pReqQueryAccount->BankPwdFlag;  //35.銀行密碼標誌 '0':不核對  '1':明文核對  '2':密文核對
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, bankPasswordFlag, "|" );
		char securityPasswordFlag = pReqQueryAccount->SecuPwdFlag;  //36.期貨資金密碼核對標誌 '0':不核對  '1':明文核對  '2':密文核對
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, securityPasswordFlag, "|" );
		UFC::AnsiString ansiOperatorNo = pReqQueryAccount->OperNo;  //37.交易櫃員
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiOperatorNo, "|" );
		int requestId = pReqQueryAccount->RequestID;  //38.請求編號
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, requestId, "|" );
		int tradeId = pReqQueryAccount->TID;  //39.交易ID
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, tradeId, "|" );
	}

	if ( FSpeedyConnPtr->FListener != NULL )
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline( "KSTRD" );
		newsMsg.SetID( CTP_OnRspQueryBankAccountMoneyByFuture );
		newsMsg.SetText( ansiResponseStr.c_str() );
		FSpeedyConnPtr->FListener->OnNews( &newsMsg );
	}
	else
		UFC::BufferedLog::Printf( "KStrdConnListener::OnKStrdRspQueryBankAccountMoneyByFuture() %s", ansiResponseStr.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
// [KStrd] KingStar special Investor Position for Open
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRspQryInvestorOpenPosition( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		UFC::BufferedLog::Printf( " OnKStrdRspQryInvestorOpenPosition failed code:%d", pRspInfo->ErrorID );
		UFC::BufferedLog::FlushToFile();
		return;
	}

	UFC::AnsiString ansiResponseStr = FSpeedyConnPtr->BuildKS_ResponseInformationStr( pRspInfo, "|" );
	FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, nRequestID, "|" );
	FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, bIsLast, "|" );
	if ( pInvestorPositionDetail != NULL )
	{
		UFC::AnsiString ansiInstrumentId = pInvestorPositionDetail->InstrumentID;  // 5.合約代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiInstrumentId, "|" );
		UFC::AnsiString ansiBrokerId = pInvestorPositionDetail->BrokerID;  // 6.經紀公司代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBrokerId, "|" );
		UFC::AnsiString ansiInvestorId = pInvestorPositionDetail->InvestorID;  // 7.投資者代碼
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiInvestorId, "|" );
		char Direction = pInvestorPositionDetail->Direction;  // 8.買賣 '0':買  '1':賣
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, Direction, "|" );
		UFC::AnsiString ansiOpenDate = pInvestorPositionDetail->OpenDate;  // 9.開倉日期
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiOpenDate, "|" );
		int Volume = pInvestorPositionDetail->Volume;  // 10.數量
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, Volume, "|" );
		double openPrice = pInvestorPositionDetail->OpenPrice;  // 11.開倉價
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, openPrice, "|" );
		UFC::BufferedLog::Printf( " OnKStrdRspQryInvestorOpenPosition:%s %s %s Dir=%c OpenPrice=%15.6f Volume=%d OpenDate=%s IsLast=%s",
								  ansiBrokerId.c_str(), ansiInvestorId.c_str(), ansiInstrumentId.c_str(), Direction, openPrice, Volume, ansiOpenDate.c_str(), (bIsLast ? "true" : "false") );
		UFC::BufferedLog::FlushToFile();

		if ( FSpeedyConnPtr->FListener != NULL )
		{
			TNewsMessage newsMsg;
			newsMsg.SetHeadline( "KSTRD" );
			newsMsg.SetID( CTP_OnInvestorOpenPosition );
			newsMsg.SetText( ansiResponseStr.c_str() );
			FSpeedyConnPtr->FListener->OnNews( &newsMsg );
		}
		else
			UFC::BufferedLog::Printf( "KStrdConnListener::OnKStrdRspQryInvestorOpenPosition() FListener is NULL [%s]", ansiResponseStr.c_str() );
	}
}
//------------------------------------------------------------------------------------------------------------------------
// [KStrd] KingStar Reject execution
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRspOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int /*nRequestID*/, bool /*bIsLast*/ )
{
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 && pInputOrder != NULL )
	{
		GB2312String     ErrMsg( pRspInfo->ErrorMsg );
		UTF8String       ErrMsgUTF8 = ErrMsg;
		UFC::AnsiString  PEStr;
		UFC::AnsiString  CombHedgeFlagStr;
		TNewOrderMessage New;

		FSpeedyConnPtr->UpdateRecover();
		UFC::BufferedLog::Printf( " ------------------------ KStrd Reject ----------------------------" );
		UFC::BufferedLog::Printf( " OnKStrdRspOrderInsert: OrderRef[%s] ",  pInputOrder->OrderRef );
		New.SetAccount( pInputOrder->UserID );		///< Account
		New.SetBrokerID( pInputOrder->BrokerID );	///< BrokerID
		New.SetSymbol( pInputOrder->InstrumentID );	///< Symbol
		New.SetMarket( nsOrderMessageDefine::mCNFutures ); ///< Market // ??
		New.SetNID( COrderInfo::ToNID( FSpeedyConnPtr->FCTPFrontID, FSpeedyConnPtr->FCTPSessionID, atoi(pInputOrder->OrderRef) ) );
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
		// CombHedge Flag. ///< 組合投機套保標誌
		if ( pInputOrder->CombHedgeFlag[0] == THOST_FTDC_HF_Speculation )
		{
			New.SetCombHedgeFlag( nsOrderMessageDefine::hfSpeculation ); 	///< Speculation ///投機/非備兌 '1'
			CombHedgeFlagStr = "1:Speculation";
		}
		else if ( pInputOrder->CombHedgeFlag[0] == THOST_FTDC_HF_Arbitrage )
		{
			New.SetCombHedgeFlag( nsOrderMessageDefine::hfArbitrage ); 		///< Arbitrage ///套利			'2'
			CombHedgeFlagStr = "2:Arbitrage";
		}
		else if ( pInputOrder->CombHedgeFlag[0] == THOST_FTDC_HF_Hedge )
		{
			New.SetCombHedgeFlag( nsOrderMessageDefine::hfHedge ); 			///< Hedge ///套保				'3'
			CombHedgeFlagStr = "3:Hedge";
		}
		else if ( pInputOrder->CombHedgeFlag[0] == THOST_FTDC_HF_Covered )
		{
			New.SetCombHedgeFlag( nsOrderMessageDefine::hfCovered ); 		///< Covered ///備兌			'4'
			CombHedgeFlagStr = "4:Covered";
		}
		UFC::BufferedLog::Printf( " NID[%s] Symbol[%s] PositionEffect[%s] CombHedgeFlag[%s] ErrorMsg[%s]", pInputOrder->OrderRef, pInputOrder->InstrumentID, PEStr.c_str(), CombHedgeFlagStr.c_str(), ErrMsgUTF8.c_str() );
		FSpeedyConnPtr->Reject( nsOrderMessageDefine::crrNew, ErrMsgUTF8.c_str(), &New , New.GetTradingSessionID());
		//TradeLogKStrdConfirm( pInputOrder, pRspInfo->ErrorID );
		UFC::BufferedLog::Printf( " ------------------- Sent Reject Execution -----------------------" );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRspError( KS_::CThostFtdcRspInfoField *pRspInfo, int /*nRequestID*/, bool /*bIsLast*/ )
{
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		UTF8String   ErrMsgUTF8 = ErrMsg;
		TNewsMessage newsMsg;

		newsMsg.SetHeadline( "KSTRD" );
		newsMsg.SetID( CTP_OnRspError );
		newsMsg.SetText( ErrMsgUTF8.c_str() );
		if ( FSpeedyConnPtr->FListener != NULL )
			FSpeedyConnPtr->FListener->OnNews( &newsMsg );
		UFC::BufferedLog::Printf( " OnKStrdRspError: Code[%d] Msg[%s]", pRspInfo->ErrorID, ErrMsgUTF8.c_str() );
	}
}
//------------------------------------------------------------------------------------------------------------------------
// [KStrd] Exchange Order confirm
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRtnOrder( KS_::CThostFtdcOrderField *pOrder )
{
	if ( pOrder == NULL )
		return;

	///< KStrd Ack: ( FrontID + SessionID ) OrderRef to find Order record.
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
	UFC::BufferedLog::Printf( " OnKStrdRtnOrder: FrontID[%d] SessionID[%d] OrderRef[%s] CombHedgeFlag[%s].", pOrder->FrontID, pOrder->SessionID, pOrder->OrderRef, pOrder->CombHedgeFlag );
	UFC::BufferedLog::Printf( " OnKStrdRtnOrder: Exchange[%s] OrderSysID[%s] OrderLocalID[%s].", ExchangeID.c_str(), OrdSysID.c_str(), OrderLocalID.c_str() );
	UFC::BufferedLog::Printf( " OnKStrdRtnOrder: OrderStatus[%s] OrderSubmitStatus[%s]", Status.c_str(), SubmitStatus.c_str() );
	UFC::BufferedLog::FlushToFile();

	FSpeedyConnPtr->UpdateRecover();
	// ??
	if ( OrderLocalID.AnsiPos( '_' ) != -1 || OrdSysID.AnsiPos( '_' ) != -1 )
	{
		UFC::BufferedLog::Printf( " OnKStrdRtnOrder: ----- It's a KStrd server side Stop Order execution. Skip this execution! -----" );
		return;
	}
	///< Use FrontID + SessionID + OrderRef to find Order.
	if ( ( Record = FSpeedyConnPtr->FindOrderByOrderRef( pOrder->FrontID, pOrder->SessionID, OrdRef.ToInt() ) ) != NULL )
		UFC::BufferedLog::Printf( " OnKStrdRtnOrder: Use FrontID + SessionID + OrderRef found the original order." );
	///< Use ExchangeID + OrderSysID to find Order.
	else if ( ( ExchangeID.Length() > 0 && OrdSysID.Length() > 0 ) &&
			  ( Record = FSpeedyConnPtr->FindOrderByOrderSysID( ExchangeID, OrdSysID.ToInt64() ) ) != NULL )
		UFC::BufferedLog::Printf( " OnKStrdRtnOrder: Use ExchangeID + OrderSysID found the original order." );
	else ///< Order not found!
	{
		UFC::BufferedLog::Printf( " --------------------- Receive Execution from other KStrd clients -------------------------" );
		UFC::BufferedLog::Printf( " OnKStrdRtnOrder: Order Exchange[%s] OrderSysID[%s] OrderLocalID[%s] not found!", ExchangeID.c_str(), OrdSysID.c_str(), OrderLocalID.c_str());
		UFC::BufferedLog::Printf( " OnKStrdRtnOrder: Add Order record FrontID[%d] SessionID[%d] OrderRef[%s]", pOrder->FrontID, pOrder->SessionID, pOrder->OrderRef );
		UFC::BufferedLog::Printf( " ----------------------------------------------------------------------------------------" );
		Record = FSpeedyConnPtr->KStrdAddPendingOrder( pOrder );
	}
	///< Update Record fields
	if ( Record->GetStatus() == cosPending && OrderLocalID.Length() > 0 && ExchangeID.Length() > 0 )
	{
		if ( OrdSysID == "0" || OrdSysID == " " ) ///< From Pending New status to KStrd confirm status.( Update ExchangeID, OrderLocalID )
		{
			Record->CTPAccepted( ExchangeID, OrderLocalID );
			//AddOrderLocalIDToMap( ExchangeID, OrderLocalID.ToInt(), Record );
			FSpeedyConnPtr->KStrdPendingExecution( Record, pOrder ); ///< Send pending New execution.
			//TradeLogKStrdConfirm( pOrder );
		}
		else ///< From KStrd confirm status to Exchange confirm status.( Update ExchangeID, OrderSysID, OrderLocalID )
		{
			Record->EachangeAccepted( ExchangeID, OrdSysID, OrderLocalID );
			FSpeedyConnPtr->AddOrderSysIDToMap( ExchangeID, OrdSysID.ToInt64() , Record );
			FSpeedyConnPtr->KStrdConfirmExecution( Record, pOrder ); ///< Send confirm execution.
			//TradeLogExConfirm( pOrder );
		}
	}
	else if( Record->GetStatus() == cosConfirm && OrdSysID.Length() > 0 && ExchangeID.Length() > 0 )
	{   ///< From CTP confirm status to Exchange confirm status.( OrderSysID )
		Record->EachangeAccepted( OrdSysID );
		FSpeedyConnPtr->AddOrderSysIDToMap( ExchangeID, OrdSysID.ToInt64() , Record );
		FSpeedyConnPtr->KStrdConfirmExecution( Record, pOrder ); ///< Send confirm execution.
		//TradeLogExConfirm( pOrder );
	}

	if ( pOrder->OrderStatus == THOST_FTDC_OST_Canceled )
	{
		if ( pOrder->OrderSubmitStatus == THOST_FTDC_OSS_Accepted ||
			 pOrder->OrderSubmitStatus == THOST_FTDC_OSS_CancelSubmitted )
		{
			Record->CloseOrder();
			FSpeedyConnPtr->KStrdCanceledExecution( Record, pOrder );///< Send Canceled execution.
			//TradeLogCancel( pOrder );
		}
		else if ( pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected ||
				  pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted )
		{
			Record->CloseOrder();
			FSpeedyConnPtr->KStrdRejectExecution( Record, pOrder );///< Send Reject execution.
			//TradeLogCancel( pOrder );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdErrRtnOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String     ErrMsg( pRspInfo->ErrorMsg );
		UTF8String       ErrMsgUTF8 = ErrMsg;

		if ( pInputOrder != NULL )
		{
			FSpeedyConnPtr->UpdateRecover();
			UFC::BufferedLog::Printf( " OnKStrdErrRtnOrderInsert: Symbol[%s] OrderRef[%s] Msg[%s]",
									  pInputOrder->InstrumentID,
									  pInputOrder->OrderRef,
									  ErrMsgUTF8.c_str() );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdErrRtnOrderAction( KS_::CThostFtdcOrderActionField *pOrderAction, KS_::CThostFtdcRspInfoField */*pRspInfo*/ )
{
	if ( pOrderAction != NULL )
	{
		FSpeedyConnPtr->UpdateRecover();
		GB2312String            ErrMsg( pOrderAction->StatusMsg );
		UTF8String              ErrMsgUTF8( ErrMsg );
		TExecutionReportMessage ExecutionReport;
		UFC::AnsiString         ExecID;
		UFC::AnsiString         ExchangeID( pOrderAction->ExchangeID );
		UFC::AnsiString         OrderSysID( pOrderAction->OrderSysID );
		COrderInfo*             Ord;
		bool IsDup;

		UFC::BufferedLog::Printf( " OnKStrdErrRtnOrderAction: Symbol[%s] Exchange[%s] OrderSysID[%s] Msg[%s]"
								  , pOrderAction->InstrumentID, pOrderAction->ExchangeID, pOrderAction->OrderSysID, ErrMsgUTF8.c_str() );
		if ( ExchangeID.Length() == 0 || OrderSysID.Length() == 0 )
			return;
		if ( ( Ord = FSpeedyConnPtr->FindOrderByOrderSysID( ExchangeID, OrderSysID.ToInt64() ) ) != NULL )
		{
			OrderSysID.TrimLeft();
			UFC::BufferedLog::Printf( " ----------------------- KStrd Cancel Reject -------------------------" );
			ExecID.Printf( "CR_%s_%s_%d", ExchangeID.c_str(), OrderSysID.c_str(), atoi(pOrderAction->OrderRef) );
			FSpeedyConnPtr->CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Ord ); // ??
			ExecutionReport.SetOrderID( OrderSysID.c_str() );
			ExecutionReport.SetNID( Ord->GetOrderNID() );
			ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osRejected );
			ExecutionReport.SetExecType( nsOrderMessageDefine::etRejected );
			ExecutionReport.SetCxlRejResponseTo( nsOrderMessageDefine::crrCancel );
			ExecutionReport.SetAllUserData( Ord->GetCxlUserData() );
			ExecutionReport.SetExecID( ExecID.c_str() );
			ExecutionReport.SetLastQty( 0 );
			ExecutionReport.SetTransactTime( pOrderAction->ActionTime );
			IsDup = FSpeedyConnPtr->IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
			UFC::BufferedLog::Printf( " OrderRef[%s] ExchangeID[%s] Symbol[%s] OrderSysID[%s]"
									  , pOrderAction->OrderRef, ExchangeID.c_str(), pOrderAction->InstrumentID, OrderSysID.c_str() );
			if ( ErrMsg.Length() > 0 )
			{
				ExecutionReport.SetText( ErrMsgUTF8.c_str() );
				UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] Msg[%s]", ExecID.c_str(), IsDup ? "Yes" : "No", ErrMsgUTF8.c_str() );
			}
			else
				UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] ", ExecID.c_str(), IsDup ? "Yes" : "No" );
			FSpeedyConnPtr->TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
			UFC::BufferedLog::Printf( " ------------------------ Sent Cancel Reject ------------------------" );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
// [KStrd] Order fill execution
//------------------------------------------------------------------------------------------------------------------------
void KStrdConnListener::OnKStrdRtnTrade( KS_::CThostFtdcTradeField *pTrade )
{
	if ( pTrade == NULL )
		return;

	// ExchangeID + OrderSysIT to find Order record.
	if ( strlen( pTrade->ExchangeID ) > 0 && strlen( pTrade->OrderLocalID ) >0 && strlen( pTrade->OrderSysID ) >0 )
	{
		FSpeedyConnPtr->UpdateRecover();
		UFC::AnsiString ExchangeID( pTrade->ExchangeID );
		Int64           OrdSysIDInt64     = _atoi64( pTrade->OrderSysID );
		int             OrderLocalIDInt = atoi( pTrade->OrderLocalID );
		COrderInfo*     Ord;

		if ( ( Ord = FSpeedyConnPtr->FindOrderByOrderSysID( ExchangeID, OrdSysIDInt64 ) )!= NULL )
		{
			FSpeedyConnPtr->KStrdFillExecution( Ord, pTrade );
			//TradeLogFill( pTrade );
		}
		else
		{
			UFC::BufferedLog::Printf( " OnKStrdRtnTrade: Exchange[%s] OrderSysID[%lld] OrderLocalID[%d] not found! update fill failed."
									  , ExchangeID.c_str(), OrdSysIDInt64, OrderLocalIDInt );
		}
	}
	else
		UFC::BufferedLog::Printf( " OnKStrdRtnTrade: missing field OrderSysID,OrderLocalID or ExchangeID." );
}

//---------------------------------------------------------------------------
// Lock
//---------------------------------------------------------------------------
//ReqLockInsert 鎖定請求 (這個介面類似下單介面)
//成功回檔 OnRtnLock 鎖定通知
//只有錯誤情況下回檔 OnRspLockInsert 鎖定應答

///鎖定應答
/*
OnRspLockInsert 方法 <== 只有錯誤情況下回檔
鎖定應答。當用戶端發出過鎖定錄入指令後，
Kingstar 伺服器返回回應時，該方法會被調用。
*/
void KStrdConnListener::OnKStrdRspLockInsert( KS_::CThostFtdcInputLockField *pInputLock, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	UTF8String   UTF8Text = "";

	if ( pRspInfo != NULL )
	{
		if ( pRspInfo->ErrorID != 0 )
		{
			GB2312String ErrMsg( pRspInfo->ErrorMsg );
			UTF8Text = ErrMsg;
			UFC::BufferedLog::Printf( " OnKStrdRspLockInsert: ErrorMsg[%s]", UTF8Text.c_str() );
		}
	}

	if ( pInputLock != NULL )
	{
		AnsiString aLockType( (pInputLock->LockType == THOST_FTDC_LCKT_Lock) ? "Lock {Error}":"Unlock {Error}" );

		UFC::AnsiString ansiResponseStr = FSpeedyConnPtr->BuildKS_ResponseInformationStr( pRspInfo, "|" );
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, nRequestID, "|" );
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, bIsLast, "|" );

		UFC::AnsiString ansiLockRef = pInputLock->LockRef;
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiLockRef, "|" );
		UFC::AnsiString ansiBusinessUnit = pInputLock->BusinessUnit;
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBusinessUnit, "|" );

		UFC::AnsiString ansiInsertDate = "";
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiInsertDate, "|" );
		UFC::AnsiString ansiInsertTime = "";
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiInsertTime, "|" );
		int iRequestID = pInputLock->RequestID;
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, iRequestID, "|" );
		UFC::AnsiString ansiInstrumentID = pInputLock->InstrumentID;
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiInstrumentID, "|" );
		UFC::AnsiString ansiLockType = (pInputLock->LockType == THOST_FTDC_LCKT_Lock) ? "Lock":"Unlock";
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiLockType, "|" );
		int iVolume = pInputLock->Volume;
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, iVolume, "|" );
		UFC::AnsiString ansiStatusMsg = UTF8Text.c_str();
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiStatusMsg, "|" );
		UFC::AnsiString ansiClearingPartID = "";
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiClearingPartID, "|" );

		if ( FSpeedyConnPtr->FListener != NULL )
		{
			TNewsMessage newsMsg;
			newsMsg.SetHeadline( "KSTRD" );
			newsMsg.SetID( CTP_OnStockRspLockInsert );
			newsMsg.SetText( ansiResponseStr.c_str() );
			FSpeedyConnPtr->FListener->OnNews( &newsMsg );

			UFC::BufferedLog::Printf( " OnKStrdRspLockInsert: BrokerID[%s] UserID[%s] InvestorID[%s] ExchangeID[%s] InstrumentID[%s]", pInputLock->BrokerID, pInputLock->UserID, pInputLock->InvestorID, pInputLock->ExchangeID, pInputLock->InstrumentID );
			UFC::BufferedLog::Printf( " OnKStrdRspLockInsert: LockRef[%s] BusinessUnit[%s] LockType[%s](OnRspLockInsert) Volume[%d] RequestID[%d]", pInputLock->LockRef, pInputLock->BusinessUnit, aLockType.c_str(), pInputLock->Volume, pInputLock->RequestID );
		}
		else
			UFC::BufferedLog::Printf( " OnKStrdRspLockInsert: FSpeedyConnPtr->FListener NULL. [%s]", ansiResponseStr.c_str() );
	}
	else
	{
		UFC::BufferedLog::Printf( " OnKStrdRspLockInsert: NULL." );
	}
}
//---------------------------------------------------------------------------
///請求查詢鎖定回應
/*
OnRspQryLock 方法
請求查詢鎖定回應。當用戶端發出請求查詢鎖定指令後，
Kingstar 伺服器返回回應時，該方法會被調用。
*/
void KStrdConnListener::OnKStrdRspQryLock( KS_::CThostFtdcLockField *pLock, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( pRspInfo != NULL )
	{
		if ( pRspInfo->ErrorID != 0 )
		{
			GB2312String ErrMsg( pRspInfo->ErrorMsg );
			UTF8String   UTF8Text( ErrMsg );
			UFC::BufferedLog::Printf( " OnKStrdRspQryLock: ErrorMsg[%s]", UTF8Text.c_str() );
		}
	}

	if ( pLock != NULL )
	{
		AnsiString aLockType( (pLock->LockType == THOST_FTDC_LCKT_Lock) ? "Lock":"Unlock" );

		UFC::BufferedLog::Printf( " OnKStrdRspQryLock: BrokerID[%s] UserID[%s] InvestorID[%s] ExchangeID[%s] InstrumentID[%s]", pLock->BrokerID, pLock->UserID, pLock->InvestorID, pLock->ExchangeID, pLock->InstrumentID );
		UFC::BufferedLog::Printf( " OnKStrdRspQryLock: LockRef[%s] BusinessUnit[%s] LockType[%s] Volume[%d] RequestID[%d]", pLock->LockRef, pLock->BusinessUnit, aLockType.c_str(), pLock->Volume, pLock->RequestID );
	}
	else
	{
		UFC::BufferedLog::Printf( " OnKStrdRspQryLock: NULL." );
	}
}
//---------------------------------------------------------------------------
///請求查詢鎖定證券倉位回應
/*
OnRspQryLockPosition 方法
請求查詢鎖定證券倉位回應。當用戶端發出請求查詢鎖定證券倉位元指令後，
Kingstar 服務器返回回應時，該方法會被調用。
*/
void KStrdConnListener::OnKStrdRspQryLockPosition( KS_::CThostFtdcLockPositionField *pLockPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if ( pRspInfo != NULL )
	{
		if ( pRspInfo->ErrorID != 0 )
		{
			GB2312String ErrMsg( pRspInfo->ErrorMsg );
			UTF8String   UTF8Text( ErrMsg );
			UFC::BufferedLog::Printf( " OnKStrdRspQryLockPosition: ErrorMsg[%s]", UTF8Text.c_str() );
		}
	}

	if ( pLockPosition != NULL )
	{
		UFC::AnsiString Msg;
		AnsiString aVolume = Msg.Printf( "%d", pLockPosition->Volume );
		AnsiString aFrozenVolume = Msg.Printf( "%d", pLockPosition->FrozenVolume );

		UFC::BufferedLog::Printf( " OnKStrdRspQryLockPosition: %14s |%12s |%8s |%12s |%12s |%12s",
					pLockPosition->BrokerID,
					pLockPosition->InvestorID,
					pLockPosition->ExchangeID,
					pLockPosition->InstrumentID,
					aVolume.c_str(),
					aFrozenVolume.c_str() );
	}
	else
	{
		UFC::BufferedLog::Printf( " OnKStrdRspQryLockPosition: NULL." );
	}
}
//---------------------------------------------------------------------------
///鎖定通知
/*
OnRtnLock 方法 <== 成功回檔
鎖定通知。
*/
void KStrdConnListener::OnKStrdRtnLock( KS_::CThostFtdcLockField *pLock )
{
	UFC::AnsiString ansiResultStr = "";

	if ( pLock != NULL )
	{
		GB2312String StatusMsg( pLock->StatusMsg );			///狀態資訊
		UTF8String   UTF8Text( StatusMsg );

		AnsiString aLockType( (pLock->LockType == THOST_FTDC_LCKT_Lock) ? "Lock":"Unlock" );
		AnsiString aLockSysID( pLock->LockSysID );
		AnsiString aLockStatus;

		/*			///鎖定狀態
		///已經提交
		#define THOST_FTDC_OAS_Submitted 'a'
		///已經接受
		#define THOST_FTDC_OAS_Accepted 'b'
		///已經被拒絕
		#define THOST_FTDC_OAS_Rejected 'c'
		*/
		switch ( pLock->LockStatus ) ///鎖定狀態
		{
			case THOST_FTDC_OAS_Submitted: aLockStatus = "Submitted"; break;
			case THOST_FTDC_OAS_Accepted: aLockStatus = "Accepted"; break;
			case THOST_FTDC_OAS_Rejected: aLockStatus = "Rejected"; break;
			default: aLockStatus = "!! UnKnow LockStatus !!"; break;
		}

		String sLockSysID( pLock->LockSysID );
		if (sLockSysID.Trim() == "") ///鎖定編號
			aLockSysID = "Is NULL";

		String sStatusMsg( pLock->StatusMsg );
		if (sStatusMsg.Trim() == "")
			UTF8Text = aLockStatus;

		UFC::AnsiString ansiLockRef = pLock->LockRef;
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiLockRef, "|" );
		UFC::AnsiString ansiBusinessUnit = pLock->BusinessUnit;
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiBusinessUnit, "|" );

		UFC::AnsiString ansiInsertDate = pLock->InsertDate;
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiInsertDate, "|" );
		UFC::AnsiString ansiInsertTime = pLock->InsertTime;
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiInsertTime, "|" );
		int iRequestID = pLock->RequestID;
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, iRequestID, "|" );
		UFC::AnsiString ansiInstrumentID = pLock->InstrumentID;
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiInstrumentID, "|" );
		UFC::AnsiString ansiLockType = (pLock->LockType == THOST_FTDC_LCKT_Lock) ? "Lock":"Unlock";
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiLockType, "|" );
		int iVolume = pLock->Volume;
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, iVolume, "|" );
		UFC::AnsiString ansiStatusMsg = UTF8Text.c_str();
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiStatusMsg, "|" );
		UFC::AnsiString ansiClearingPartID = pLock->ClearingPartID;
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiClearingPartID, "|" );

		UFC::AnsiString ansiLockSysID = aLockSysID.c_str();
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResultStr, ansiLockSysID, "|" );

		if ( FSpeedyConnPtr->FListener != NULL )
		{
			TNewsMessage newsMsg;
			newsMsg.SetHeadline( "KSTRD" );
			newsMsg.SetID( CTP_OnStockRtnLock );
			newsMsg.SetText( ansiResultStr.c_str() );
			FSpeedyConnPtr->FListener->OnNews( &newsMsg );

			UFC::BufferedLog::Printf( " OnKStrdRtnLock: BrokerID[%s] UserID[%s] InvestorID[%s] ExchangeID[%s] InstrumentID[%s]", pLock->BrokerID, pLock->UserID, pLock->InvestorID, pLock->ExchangeID, pLock->InstrumentID );
			UFC::BufferedLog::Printf( " OnKStrdRtnLock: LockRef[%s] BusinessUnit[%s] LockType[%s] Volume[%d] RequestID[%d]", pLock->LockRef, pLock->BusinessUnit, aLockType.c_str(), pLock->Volume, pLock->RequestID );
			UFC::BufferedLog::Printf( " OnKStrdRtnLock: LockSysID[%s] DateTime[%s %s] CancelTime[%s] ClearingPartID[%s]", aLockSysID.c_str(), pLock->InsertDate, pLock->InsertTime, pLock->CancelTime, pLock->ClearingPartID );
			UFC::BufferedLog::Printf( " OnKStrdRtnLock: LockStatus[%s] StatusMsg[%s]", aLockStatus.c_str(), UTF8Text.c_str() );
		}
		else
			UFC::BufferedLog::Printf( " OnKStrdRtnLock: FSpeedyConnPtr->FListener NULL. [%s]", ansiResultStr.c_str() );

	}
	else
	{
		UFC::BufferedLog::Printf( " OnKStrdRtnLock: NULL." );
	}
}
//---------------------------------------------------------------------------
///鎖定錯誤通知
/*
OnErrRtnLockInsert 方法
鎖定錯誤通知。當用戶端發出鎖定錯誤指令後，
Kingstar 伺服器返回回應時，該方法會被調用。
*/
void KStrdConnListener::OnKStrdErrRtnLockInsert( KS_::CThostFtdcInputLockField *pInputLock, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	UTF8String   UTF8Text = "";

	if ( pRspInfo != NULL )
	{
		if ( pRspInfo->ErrorID != 0 )
		{
			GB2312String ErrMsg( pRspInfo->ErrorMsg );
			UTF8Text = ErrMsg;
			UFC::BufferedLog::Printf( " OnKStrdErrRtnLockInsert: ErrorMsg[%s]", UTF8Text.c_str() );
		}
	}

	if ( pInputLock != NULL )
	{
		AnsiString aLockType( (pInputLock->LockType == THOST_FTDC_LCKT_Lock) ? "Lock {Error}":"Unlock {Error}" );

		UFC::AnsiString ansiResponseStr = FSpeedyConnPtr->BuildKS_ResponseInformationStr( pRspInfo, "|" );

		UFC::AnsiString ansiLockRef = pInputLock->LockRef;
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiLockRef, "|" );
		UFC::AnsiString ansiBusinessUnit = pInputLock->BusinessUnit;
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiBusinessUnit, "|" );

		UFC::AnsiString ansiInsertDate = "";
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiInsertDate, "|" );
		UFC::AnsiString ansiInsertTime = "";
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiInsertTime, "|" );
		int iRequestID = pInputLock->RequestID;
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, iRequestID, "|" );
		UFC::AnsiString ansiInstrumentID = pInputLock->InstrumentID;
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiInstrumentID, "|" );
		UFC::AnsiString ansiLockType = (pInputLock->LockType == THOST_FTDC_LCKT_Lock) ? "Lock":"Unlock";
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiLockType, "|" );
		int iVolume = pInputLock->Volume;
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, iVolume, "|" );
		UFC::AnsiString ansiStatusMsg = UTF8Text.c_str();
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiStatusMsg, "|" );
		UFC::AnsiString ansiClearingPartID = "";
		FSpeedyConnPtr->AppendFieldToFieldsStr( ansiResponseStr, ansiClearingPartID, "|" );

		if ( FSpeedyConnPtr->FListener != NULL )
		{
			TNewsMessage newsMsg;
			newsMsg.SetHeadline( "KSTRD" );
			newsMsg.SetID( CTP_OnStockErrRtnLockInsert );
			newsMsg.SetText( ansiResponseStr.c_str() );
			FSpeedyConnPtr->FListener->OnNews( &newsMsg );

			UFC::BufferedLog::Printf( " OnKStrdErrRtnLockInsert: BrokerID[%s] UserID[%s] InvestorID[%s] ExchangeID[%s] InstrumentID[%s]", pInputLock->BrokerID, pInputLock->UserID, pInputLock->InvestorID, pInputLock->ExchangeID, pInputLock->InstrumentID );
			UFC::BufferedLog::Printf( " OnKStrdErrRtnLockInsert: LockRef[%s] BusinessUnit[%s] LockType[%s](錯誤!!OnErrRtnLockInsert通知) Volume[%d] RequestID[%d]", pInputLock->LockRef, pInputLock->BusinessUnit, aLockType.c_str(), pInputLock->Volume, pInputLock->RequestID );
		}
		else
			UFC::BufferedLog::Printf( " OnKStrdErrRtnLockInsert: FSpeedyConnPtr->FListener NULL. [%s]", ansiResponseStr.c_str() );
	}
	else
	{
		UFC::BufferedLog::Printf( " OnKStrdErrRtnLockInsert: NULL." );
	}
}

//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
