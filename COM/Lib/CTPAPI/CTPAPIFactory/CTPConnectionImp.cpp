//---------------------------------------------------------------------------
#ifdef _CTP
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#include "CTPConnectionImp.h"
//---------------------------------------------------------------------------
typedef AnsiStringT<936> GB2312String;
//---------------------------------------------------------------------------
///< To Do list
// < 1. Type for FOK not define
//---------------------------------------------------------------------------
//  ExecID Rule
//
//  Int32: FrontID, SessionID, OrderRef
//  Int64: OrderSysID
//
//  NewOrder ExecID: "N" + FrontID + SessionID + OrderRef
//  Fill 	 ExecID: "F" + ExchangeID + OrderSysID + SequenceNo
//  Fill 	 ExecID: "F" + ExchangeID + OrderSysID + TraderID <HSctp>
//  Canceled ExecID: "C" + FrontID + SessionID + OrderRef
//---------------------------------------------------------------------------
void OrderSubmitStatusString( TThostFtdcOrderSubmitStatusType osst, UFC::AnsiString& Str )
{
	switch( osst )
	{
		case '0': Str ="THOST_FTDC_OSS_InsertSubmitted"; break;
		case '1': Str ="THOST_FTDC_OSS_CancelSubmitted"; break;
		case '2': Str ="THOST_FTDC_OSS_ModifySubmitted"; break;
		case '3': Str ="THOST_FTDC_OSS_Accepted"; break;
		case '4': Str ="THOST_FTDC_OSS_InsertRejected"; break;
		case '5': Str ="THOST_FTDC_OSS_CancelRejected"; break;
		case '6': Str ="THOST_FTDC_OSS_ModifyRejected"; break;
		default : Str.Printf( "Value[%c] not define",osst );break;
	}
}
//---------------------------------------------------------------------------
void OrderStatusString( TThostFtdcOrderStatusType ost, UFC::AnsiString& Str )
{
	switch( ost )
	{
		case '0': Str ="THOST_FTDC_OST_AllTraded";break;
		case '1': Str ="THOST_FTDC_OST_PartTradedQueueing";break;
		case '2': Str ="THOST_FTDC_OST_PartTradedNotQueueing";break;
		case '3': Str ="THOST_FTDC_OST_NoTradeQueueing";break;
		case '4': Str ="THOST_FTDC_OST_NoTradeNotQueueing";break;
		case '5': Str ="THOST_FTDC_OST_Canceled";break;
		case 'a': Str ="THOST_FTDC_OST_Unknown";break;
		case 'b': Str ="THOST_FTDC_OST_NotTouched";break;
		case 'c': Str ="THOST_FTDC_OST_Touched";break;
		default : Str.Printf( "Value[%c] not define",ost );break;
	}
}
//---------------------------------------------------------------------------
//
// Implement TTaifexConnection
// for CTP/Femas/KSft/KStrd functions
//
//---------------------------------------------------------------------------
int TTaifexConnection::CTPReqID( void )
{
	int Seq = FReqSequence;
	FReqSequence += 1;
	if( FReqSequence.ToInt32() > REQ_ID_MAX )
		FReqSequence = 1;
	return Seq;
}
//---------------------------------------------------------------------------
void TTaifexConnection::RemoveShm( char* APIName )
{
	UFC::UDate Date;
	UFC::AnsiString DateString, ShmName;

	Date -= RM_CTP_SHM_DAYS; ///Back to RM_SHM_DAYS days ago.
	for( int i = 0; i < RM_CTP_SHM_DAYS - 1; i++ )
	{
		 try
		 {
			DateString.Printf( "%04d%02d%02d", Date.getYear(), Date.getMonth(), Date.getDate() );
			Date += 1; ///< Next day.

			ShmName.Printf( "%s.%s.REQ.Seq.%s", FID.c_str(), APIName, DateString.c_str());
			FReqSequence.Attach( ShmName );
			FReqSequence.Remove( );

			ShmName.Printf( "%s.%s.Ord.Seq.%s", FID.c_str(), APIName, DateString.c_str() );
			FOrderRef.Attach( ShmName );
			FOrderRef.Remove();
		 }
		 catch( UFC::Exception& e )
		 {
			UFC::BufferedLog::Printf( " *** Remove share memory for[%s] failed, skip it! ***", DateString.c_str() );
			UFC::BufferedLog::Printf( " Reason:%s", e.what() );
		 }
	}
}
//---------------------------------------------------------------------------
void TTaifexConnection::AttachShm( char* APIName, char* TradingDay  )
{
	UFC::AnsiString ShmName;
	int InitOrderRef;

	if(	FIsAttached == false )
	{
		ShmName.Printf( "%s.%s.REQ.Seq.%s", FID.c_str(), APIName, TradingDay );
		FReqSequence.Attach( ShmName );

		ShmName.Printf( "%s.%s.Ord.Seq.%s", FID.c_str(), APIName, TradingDay );
		FOrderRef.Attach( ShmName );

		InitOrderRef = FOrderRef.ToInt32();
		if( InitOrderRef < 1 || InitOrderRef > 16777216 )
			FOrderRef = 1;
		FIsAttached = true;
	}
}
//---------------------------------------------------------------------------
void TTaifexConnection::DetachShm( void )
{
	if(	FIsAttached == true )
	{
		FReqSequence.Detach();
		FOrderRef.Detach();
		FIsAttached = false;
	}
}
//---------------------------------------------------------------------------
//
// Implement TTaifexConnection
// CTP functions
//
//---------------------------------------------------------------------------
COrderInfo* TTaifexConnection::AddPendingOrder( int OrdRef, CThostFtdcInputOrderField& Ord, const UFC::AnsiString& UserDefine )
{
	COrderInfo* NewOrd = new COrderInfo( FCTPFrontID,FCTPSessionID, Ord, UserDefine );
	UFC::AnsiString OrderRefKey;

	OrderRefKey.Printf( "%d|%d|%d", FCTPFrontID, FCTPSessionID, OrdRef );
	FOrderRecords.Add( NewOrd );
	FOrderRefToRecord.Add( OrderRefKey, NewOrd );
	return NewOrd;
}
//---------------------------------------------------------------------------
COrderInfo* TTaifexConnection::AddPendingOrder( int OrdRef, CUstpFtdcInputOrderField& Ord, const UFC::AnsiString& UserDefine )
{
	COrderInfo* NewOrd = new COrderInfo( FCTPFrontID,FCTPSessionID, Ord, UserDefine );
	UFC::AnsiString OrderRefKey;

	OrderRefKey.Printf( "%d|%d|%d", FCTPFrontID, FCTPSessionID, OrdRef );
	FOrderRecords.Add( NewOrd );
	FOrderRefToRecord.Add( OrderRefKey, NewOrd );
	return NewOrd;
}
//---------------------------------------------------------------------------
COrderInfo* TTaifexConnection::AddPendingOrder( CThostFtdcOrderField* Ord )
{
	COrderInfo* NewOrd = new COrderInfo( Ord );
	UFC::AnsiString OrderRefKey;

	OrderRefKey.Printf( "%d|%d|%d", Ord->FrontID, Ord->SessionID, atoi( Ord->OrderRef ) );
	FOrderRecords.Add( NewOrd );
	FOrderRefToRecord.Add( OrderRefKey, NewOrd );
	return NewOrd;
}
//---------------------------------------------------------------------------
COrderInfo* TTaifexConnection::AddPendingOrder( CUstpFtdcOrderField* Ord )
{
	COrderInfo* NewOrd = new COrderInfo( FCTPFrontID, FCTPSessionID, Ord );
	UFC::AnsiString OrderRefKey;

	OrderRefKey.Printf( "%d|%d|%d", FCTPFrontID, FCTPSessionID );
	FOrderRecords.Add( NewOrd );
	FOrderRefToRecord.Add( OrderRefKey, NewOrd );
	return NewOrd;
}
//---------------------------------------------------------------------------
COrderInfo*  TTaifexConnection::FindOrderByOrderRef( int FID, int SID, int OrdRef )
{
	UFC::AnsiString OrderRefKey;
	COrderInfo*   FindOrd;

	OrderRefKey.Printf( "%d|%d|%d", FID, SID, OrdRef );
	if( FOrderRefToRecord.GetObjectByKey( OrderRefKey, FindOrd ) == TRUE )
		return FindOrd;
	return NULL;
}
//---------------------------------------------------------------------------
void TTaifexConnection::AddOrderLocalIDToMap( const UFC::AnsiString& ExchangeID, int OrdLocalID, COrderInfo* Ord )
{
	UFC::AnsiString OrderLocalIDKey;

	OrderLocalIDKey.Printf( "%s|%d", ExchangeID.c_str(), OrdLocalID );
	FOrdLocalIDToRecord.Add( OrderLocalIDKey , Ord );
}
//---------------------------------------------------------------------------
COrderInfo* TTaifexConnection::FindOrderByOrderLocalID( const UFC::AnsiString& ExchangeID, int OrdLocalID )
{
	UFC::AnsiString OrderLocalIDKey;
	COrderInfo*   FindOrd;

	OrderLocalIDKey.Printf( "%s|%d", ExchangeID.c_str(), OrdLocalID );
	if( FOrdLocalIDToRecord.GetObjectByKey( OrderLocalIDKey, FindOrd ) == TRUE )
		return FindOrd;
	return NULL;
}
//---------------------------------------------------------------------------
void TTaifexConnection::AddOrderSysIDToMap( const UFC::AnsiString& ExchangeID, Int64 OrdSysID, COrderInfo* Ord )
{
	UFC::AnsiString OrderSysIDKey;

	OrderSysIDKey.Printf( "%s|%lld", ExchangeID.c_str(), OrdSysID );
	FOrdSysIDToRecord.Add( OrderSysIDKey , Ord );
}
//---------------------------------------------------------------------------
COrderInfo*  TTaifexConnection::FindOrderByOrderSysID( const UFC::AnsiString& ExchangeID, Int64 OrdSysID )
{
	UFC::AnsiString OrderSysIDKey;
	COrderInfo*   FindOrd;

	OrderSysIDKey.Printf( "%s|%lld", ExchangeID.c_str(), OrdSysID );
	if( FOrdSysIDToRecord.GetObjectByKey( OrderSysIDKey, FindOrd ) == TRUE )
		return FindOrd;
	return NULL;
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
void TTaifexConnection::OnAPIConnected()
{
	FIsConnected = true;
	FLogonEvent.SetEvent();
}
//---------------------------------------------------------------------------
void TTaifexConnection::OnAPIDisconnected( int )
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
void TTaifexConnection::OnRecoverFinished(int Count )
{
	if( FListener != NULL )
		FListener->OnRecoverFinished( Count );
}
//---------------------------------------------------------------------------
// Trigger by Main UI thread.
//---------------------------------------------------------------------------
void TTaifexConnection::OnFrontConnected()
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

			Msg.SetHeadline( "CTP" );
			Msg.SetID( CTP_OnConnected );
			FListener->OnNews( &Msg );
		}
	}

}
//---------------------------------------------------------------------------
void TTaifexConnection::OnFrontDisconnected(int  )
{
	if( FListener != NULL )
	{
		FListener->OnDisconnected();
		TNewsMessage Msg;

		Msg.SetHeadline( "CTP");
		Msg.SetID( CTP_OnDisconnectd );
		FListener->OnNews( &Msg );
	}
}
//---------------------------------------------------------------------------
void TTaifexConnection::OnRspUserLogin( CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int , bool )
{
	LogonResult  Result = lrFailed;
	AnsiString   ReplyString( "Logon OK" );

	FAdmin = 0;
	if( pRspInfo != NULL && pRspInfo->ErrorID != 0) ///< Logon failed.
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		UTF8String   ErrMsgUTF8( ErrMsg );

		ReplyString = ErrMsgUTF8.c_str();
		Result   = lrFailed;
		FIsLogon = false;
		FCurrentConnectionID = 0;
		if ( pRspUserLogin == NULL ) ///< Prevent access violation when pRspUserLogin is null (HSctp)
			UFC::BufferedLog::Printf( " OnRspUserLogin: Code[%d] Msg[%s] Logon failed.", pRspInfo->ErrorID, ErrMsgUTF8.c_str() );
		else
			UFC::BufferedLog::Printf( " OnRspUserLogin: Account[%s] Code[%d] Msg[%s] Logon failed.", pRspUserLogin->UserID, pRspInfo->ErrorID, ErrMsgUTF8.c_str() );
		if( FListener != NULL )
			FListener->OnLogonReply( ErrMsgUTF8.c_str(), Result, FCurrentConnectionID );
	}
	else ///< Logon OK
	{
		UFC::AnsiString FileName;
		TNewsMessage Msg,ReqMsg;
		UFC::NameValueMessage TimeMsg("^\n");

		FTradeingDate = pRspUserLogin->TradingDay;
		FIsRecover = false;
		FRecoverCount = 0;
		FLastRecover = UFC::GetTickCountMS();
		RemoveShm( "CTP" );
		AttachShm( "CTP", pRspUserLogin->TradingDay );
		FCTPFrontID   = pRspUserLogin->FrontID;
		FCTPSessionID = pRspUserLogin->SessionID;
		Result   = lrOk;
		FIsLogon = true;
		FUseNID64 = true;
		FCurrentConnectionID = pRspUserLogin->SessionID;
		UFC::CreateDir( "./log" );
		FileName.Printf( "./log/TradeingLog_%s_%s.log", pRspUserLogin->UserID, pRspUserLogin->TradingDay);
		CTPRecover( FileName );
		if( FCTPTransLog != NULL )
			delete FCTPTransLog;
		FCTPTransLog = new UFC::FileStream64( FileName.c_str(), "a");
		UFC::BufferedLog::Printf( " OnRspUserLogin: Account[%s] FrontID[%d] SessionID[%d] Logon OK! ", pRspUserLogin->UserID, pRspUserLogin->FrontID, pRspUserLogin->SessionID );
		if( FListener != NULL )
			FListener->OnLogonReply( ReplyString.c_str(), Result, FCurrentConnectionID );
		///< Update Exchange clocks.
		Msg.SetHeadline( "CTP");
		Msg.SetID( CTP_OnExchangeTime );
		TimeMsg.Append( "SHFE",  pRspUserLogin->SHFETime );
		TimeMsg.Append( "DCE",   pRspUserLogin->DCETime );
		TimeMsg.Append( "CZCE",  pRspUserLogin->CZCETime );
		TimeMsg.Append( "CFFEX", pRspUserLogin->FFEXTime );
		Msg.SetText( TimeMsg.ToString().c_str() );
		if( FListener != NULL )
			FListener->OnNews( &Msg );
		///< query SettlementInfo confirm.
		ReqMsg.SetHeadline( "CTP");
		ReqMsg.SetID( CTP_QuerySettlementInfoConfirm );
		HandleCTPMessage( &ReqMsg );
	}
}
//---------------------------------------------------------------------------
void TTaifexConnection::UpdateRecover( void )
{
	FRecoverCount++;
	if( FIsRecover == true )
		FLastRecover = UFC::GetTickCountMS();
}
//---------------------------------------------------------------------------
void TTaifexConnection::OnRspUserLogout(CThostFtdcUserLogoutField *, CThostFtdcRspInfoField *, int , bool )
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
void TTaifexConnection::OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int, bool )
{
	TNewsMessage Msg;

	Msg.SetHeadline( "CTP");
	Msg.SetID( CTP_OnPasswordChanged );
	if( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		if ( pUserPasswordUpdate == NULL )
			UFC::BufferedLog::Printf( " OnRspUserPasswordUpdate: pUserPasswordUpdate is null" );
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		UTF8String   UTF8Text( ErrMsg );

		Msg.SetText( UTF8Text.c_str() );
		if ( pUserPasswordUpdate == NULL ) ///< Prevent access violation when pUserPasswordUpdate is null (KNctp)
			UFC::BufferedLog::Printf( " OnRspUserPasswordUpdate: Change Password failed! Code[%d] Msg[%s]", pRspInfo->ErrorID, UTF8Text.c_str() );
		else
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
void TTaifexConnection::OnRspTradingAccountPasswordUpdate(CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int, bool)
{
	TNewsMessage Msg;

	Msg.SetHeadline( "CTP");
	Msg.SetID( CTP_OnPasswordChanged );
	if( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		UTF8String   UTF8Text( ErrMsg );

		Msg.SetText( UTF8Text.c_str() );
		UFC::BufferedLog::Printf( " OnRspTradingAccountPasswordUpdate: Account[%s] Change Password failed! Code[%d] Msg[%s]", pTradingAccountPasswordUpdate->AccountID, pRspInfo->ErrorID, UTF8Text.c_str() );
	}
	else
	{
		Msg.SetText( "OK" );
		UFC::BufferedLog::Printf( " OnRspTradingAccountPasswordUpdate: Account[%s] Change Password OK!", pTradingAccountPasswordUpdate->AccountID );
	}
	if( FListener != NULL )
		FListener->OnNews( &Msg );
}
//---------------------------------------------------------------------------
void TTaifexConnection::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int, bool bIsLast)
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

		Msg.SetHeadline( "CTP");
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
		FBankAccount = pTradingAccount->AccountID;
		Msg.SetText( BalanceMsg.ToString().c_str() );
		if( FListener != NULL )
			FListener->OnNews( &Msg );
	}
}
//---------------------------------------------------------------------------
void TTaifexConnection::OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus )
{
	TNewsMessage Msg;
	UFC::NameValueMessage StatusMsg("^\n");

	if( pInstrumentStatus != NULL )
	{
		Msg.SetHeadline( "CTP");
		Msg.SetID( CTP_OnExchangeStatus );
		StatusMsg.Append( "Exchange",  pInstrumentStatus->ExchangeID );
		StatusMsg.Append( "Status",  UFC::AnsiString(pInstrumentStatus->InstrumentStatus) );
		Msg.SetText( StatusMsg.ToString().c_str() );
		if( FListener != NULL )
			FListener->OnNews( &Msg );
		UFC::BufferedLog::Printf( " OnRtnInstrumentStatus Exchange[%s] Time[%s] Status[%c] ", pInstrumentStatus->ExchangeID, pInstrumentStatus->EnterTime, pInstrumentStatus->InstrumentStatus );
	}
}
//---------------------------------------------------------------------------
void TTaifexConnection::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int , bool )
{
	if(  pRspInfo != NULL && pRspInfo->ErrorID != 0  )
	{
		 UFC::BufferedLog::Printf( " OnRspQrySettlementInfoConfirm failed code:%d", pRspInfo->ErrorID );
		 return;
	}
	if( pSettlementInfoConfirm != NULL  ) ///< Already confirmed.
	{
		UFC::BufferedLog::Printf( " OnRspQrySettlementInfoConfirm: User confirm settlement Info at:%s", pSettlementInfoConfirm->ConfirmTime );
	}
	else ///< Request Settlement Info
	{
		UFC::BufferedLog::Printf( " OnRspQrySettlementInfoConfirm: User not confirm Settlement Info." );
		CThostFtdcQrySettlementInfoField QuerySettlementData;

		FSettlementInfo.Clear();
		memset( &QuerySettlementData, 0, sizeof(QuerySettlementData));
		strcpy(QuerySettlementData.BrokerID,   FCFFEXBrokerID.c_str() );
		strcpy(QuerySettlementData.InvestorID, FID.c_str() );
		while( TRUE )
		{
		   int Rtn = FCTPAPI->ReqQrySettlementInfo( &QuerySettlementData, CTPReqID() );
		   if( Rtn == -2 || Rtn == -3 )
			   UFC::SleepMS(300);
		   else
			   break;
		}
	}
}
//---------------------------------------------------------------------------
void TTaifexConnection::OnRspQrySettlementInfo( CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int , bool bIsLast)
{
	if( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		TNewsMessage Msg;

		UTF8String UTF8Text( ErrMsg );
		Msg.SetHeadline( "CTP");
		Msg.SetID( CTP_OnSettlementInfo );
		Msg.SetText( UTF8Text.c_str() );
		if( FListener != NULL )
			FListener->OnNews( &Msg );
		return;
	}
	if ((pSettlementInfo != NULL ) && (std::strlen(pSettlementInfo->Content) > 0))
	{
		FSettlementInfo.Add( pSettlementInfo->Content );
	}
	if( bIsLast )
	{
		GB2312String Text;
		TNewsMessage Msg;

		Msg.SetHeadline( "CTP");
		Msg.SetID( CTP_OnSettlementInfo );
		if( FSettlementInfo.ItemCount() > 0 )
		{
			for( int i =0; i < FSettlementInfo.ItemCount(); i ++ )
				Text += GB2312String( FSettlementInfo[i].c_str() );
			UTF8String UTF8Text( Text );
			Msg.SetText( UTF8Text.c_str() );
		}
		else
			Msg.SetText( "-----------------------------------------------------------------" );
		if( FListener != NULL )
			FListener->OnNews( &Msg );
	}
}
//---------------------------------------------------------------------------
void TTaifexConnection::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* SettlementInfoConfirm, CThostFtdcRspInfoField* pRspInfo, int , bool )
{
	if( pRspInfo == NULL || pRspInfo->ErrorID == 0 )///< Confirm OK
	{   ///< User confirmed Settlement Info.
		if( SettlementInfoConfirm != NULL  )
		{
			UFC::BufferedLog::Printf( " User[%s] Confirm settlement info at[%s] on[%s]",
									  SettlementInfoConfirm->InvestorID,
									  SettlementInfoConfirm->ConfirmTime,
									  SettlementInfoConfirm->ConfirmDate );
		}
	}
}
//---------------------------------------------------------------------------
// To Do : Femas imp
void TTaifexConnection::OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int , bool )
{
	if( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		UTF8String UTF8Text( ErrMsg );
		UFC::BufferedLog::Printf(" OnRspQryInvestorPositionDetail: %d[%s]", pRspInfo->ErrorID, UTF8Text.c_str());
		UFC::BufferedLog::FlushToFile();
		return;
	}

	if (pInvestorPositionDetail == NULL)
	{
		UFC::BufferedLog::Printf(" OnRspQryInvestorPositionDetail: pInvestorPositionDetail is NULL.");
		UFC::BufferedLog::FlushToFile();
		return;
	}
	UFC::BufferedLog::Printf(" OnRspQryInvestorPositionDetail: %s %s %s Direction=%c OpenDate=%s TradeDate=%s Vol=%d OpenPx=%15.6lf CloseVol=%d TradeType=%c",
	pInvestorPositionDetail->BrokerID,
	pInvestorPositionDetail->InvestorID,
	pInvestorPositionDetail->InstrumentID,
	pInvestorPositionDetail->Direction,
	pInvestorPositionDetail->OpenDate,
	pInvestorPositionDetail->TradingDay,
	pInvestorPositionDetail->Volume,
	pInvestorPositionDetail->OpenPrice,
	pInvestorPositionDetail->CloseVolume,
	pInvestorPositionDetail->TradeType);
	UFC::BufferedLog::FlushToFile();
	if (FListener != NULL)
	{
		UFC::AnsiString newsText;
		newsText.Printf("%s|%s|%s|%c|%s|%d|%15.6lf|%d|%s",
						pInvestorPositionDetail->BrokerID, pInvestorPositionDetail->InvestorID, pInvestorPositionDetail->InstrumentID,
						pInvestorPositionDetail->Direction, pInvestorPositionDetail->OpenDate,
						pInvestorPositionDetail->Volume, pInvestorPositionDetail->OpenPrice,
						pInvestorPositionDetail->CloseVolume,
						pInvestorPositionDetail->TradingDay);
		TNewsMessage newsMsg;
		newsMsg.SetHeadline("CTP");
		newsMsg.SetID( CTP_OnInvestorPositionDetail );
		newsMsg.SetText(newsText.c_str());
		FListener->OnNews(&newsMsg);
	}
}  //TTaifexConnection::OnRspQryInvestorPositionDetail()
//---------------------------------------------------------------------------
void TTaifexConnection::AppendFieldToFieldsStr(UFC::AnsiString& FieldsStr, const UFC::AnsiString& FieldValue, const UFC::AnsiString& FieldDelimiter)
{
	if (FieldsStr.Length() > 0) FieldsStr = FieldsStr + FieldDelimiter;
	if (FieldValue.Length() > 0) FieldsStr = FieldsStr + FieldValue;
}  //TTaifexConnection::AppendFieldToFieldsStr()
//---------------------------------------------------------------------------
void TTaifexConnection::AppendFieldToFieldsStr(UFC::AnsiString& FieldsStr, char FieldValue, const UFC::AnsiString& FieldDelimiter)
{
	UFC::AnsiString fieldValueStr;
	fieldValueStr.Printf("%c", FieldValue);
	AppendFieldToFieldsStr(FieldsStr, fieldValueStr, FieldDelimiter);
}  //TTaifexConnection::AppendFieldToFieldsStr()
//---------------------------------------------------------------------------
void TTaifexConnection::AppendFieldToFieldsStr(UFC::AnsiString& FieldsStr, int FieldValue, const UFC::AnsiString& FieldDelimiter)
{
	UFC::AnsiString fieldValueStr;
	fieldValueStr.Printf("%d", FieldValue);
	AppendFieldToFieldsStr(FieldsStr, fieldValueStr, FieldDelimiter);
}  //TTaifexConnection::AppendFieldToFieldsStr()
//---------------------------------------------------------------------------
void TTaifexConnection::AppendFieldToFieldsStr(UFC::AnsiString& FieldsStr, double FieldValue, const UFC::AnsiString& FieldDelimiter)
{
	UFC::AnsiString fieldValueStr;
	fieldValueStr.Printf("%15.7lf", FieldValue);
	AppendFieldToFieldsStr(FieldsStr, fieldValueStr, FieldDelimiter);
}  //TTaifexConnection::AppendFieldToFieldsStr()
//---------------------------------------------------------------------------
void TTaifexConnection::AppendFieldToFieldsStr(UFC::AnsiString& FieldsStr, bool FieldValue, const UFC::AnsiString& FieldDelimiter)
{
	UFC::AnsiString fieldValueStr = "0";
	if (FieldValue == true) fieldValueStr = "1";
	AppendFieldToFieldsStr(FieldsStr, fieldValueStr, FieldDelimiter);
}  //TTaifexConnection::AppendFieldToFieldsStr()
//---------------------------------------------------------------------------
UFC::AnsiString TTaifexConnection::BuildResponseInformationStr(CThostFtdcRspInfoField *pRspInfo, const UFC::AnsiString& FieldDelimiter)
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
// To Do : Femas imp
void TTaifexConnection::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	UFC::AnsiString ansiResponseStr = BuildResponseInformationStr(pRspInfo, "|");
	AppendFieldToFieldsStr(ansiResponseStr, nRequestID, "|");
	AppendFieldToFieldsStr(ansiResponseStr, bIsLast, "|");
	if (pInstrumentCommissionRate != NULL)
	{
		UFC::BufferedLog::Printf( "TTaifexConnection::OnRspQryInstrumentCommissionRate(): OpenRatioByMoney[%15.7lf] CloseRatioByMoney[%15.7lf] CloseTodayRatioByMoney[%15.7lf].", pInstrumentCommissionRate->OpenRatioByMoney, pInstrumentCommissionRate->CloseRatioByMoney, pInstrumentCommissionRate->CloseTodayRatioByMoney );

		UFC::AnsiString ansiInstrumentId = pInstrumentCommissionRate->InstrumentID;  // 5.合約代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiInstrumentId, "|");
		char investorRange = pInstrumentCommissionRate->InvestorRange;  // 6.投資者範圍  '1':所有 '2':投資者組 '3':單一投資者
		AppendFieldToFieldsStr(ansiResponseStr, investorRange, "|");
		UFC::AnsiString ansiBrokerId = pInstrumentCommissionRate->BrokerID;  // 7.經紀公司代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBrokerId, "|");
		UFC::AnsiString ansiInvestorId = pInstrumentCommissionRate->InvestorID;  // 8.投資者代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiInvestorId, "|");
		double openRatioByMoney = pInstrumentCommissionRate->OpenRatioByMoney;  // 9.開倉手續費率
		AppendFieldToFieldsStr(ansiResponseStr, openRatioByMoney, "|");
		double openRatioByVolume = pInstrumentCommissionRate->OpenRatioByVolume;  //10.開倉手續費
		AppendFieldToFieldsStr(ansiResponseStr, openRatioByVolume, "|");
		double closeRatioByMoney = pInstrumentCommissionRate->CloseRatioByMoney;  //11.平倉手續費率
		AppendFieldToFieldsStr(ansiResponseStr, closeRatioByMoney, "|");
		double closeRatioByVolume = pInstrumentCommissionRate->CloseRatioByVolume;  //12.平倉手續費
		AppendFieldToFieldsStr(ansiResponseStr, closeRatioByVolume, "|");
		double closeTodayRatioByMoney = pInstrumentCommissionRate->CloseTodayRatioByMoney;  //13.平今手續費率
		AppendFieldToFieldsStr(ansiResponseStr, closeTodayRatioByMoney, "|");
		double closeTodayRatioByVolume = pInstrumentCommissionRate->CloseTodayRatioByVolume;  //14.平今手續費
		AppendFieldToFieldsStr(ansiResponseStr, closeTodayRatioByVolume, "|");
	}

	if (FListener != NULL)
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline("CTP");
		newsMsg.SetID(CTP_OnRspQryInstrumentCommissionRate);
		newsMsg.SetText(ansiResponseStr.c_str());
		FListener->OnNews(&newsMsg);
	}
	else
		UFC::BufferedLog::Printf("TTaifexConnection::OnRspQryInstrumentCommissionRate() %s", ansiResponseStr.c_str());
}  //TTaifexConnection::OnRspQryInstrumentCommissionRate()
//---------------------------------------------------------------------------
// To Do : Femas imp
void TTaifexConnection::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPositionInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	UFC::AnsiString ansiResponseStr = BuildResponseInformationStr(pRspInfo, "|");
	AppendFieldToFieldsStr(ansiResponseStr, nRequestID, "|");
	AppendFieldToFieldsStr(ansiResponseStr, bIsLast, "|");
	if (pInvestorPositionInfo != NULL)
	{
		UFC::AnsiString ansiInstrumentId = pInvestorPositionInfo->InstrumentID;  // 5.合約代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiInstrumentId, "|");
		UFC::AnsiString ansiBrokerId = pInvestorPositionInfo->BrokerID;  // 6.經紀公司代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBrokerId, "|");
		UFC::AnsiString ansiInvestorId = pInvestorPositionInfo->InvestorID;  // 7.投資者代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiInvestorId, "|");
		char positionDirection = pInvestorPositionInfo->PosiDirection;  // 8.持倉多空方向 '1':淨  '2':多  '3':空
		AppendFieldToFieldsStr(ansiResponseStr, positionDirection, "|");
		char hedgeFlag = pInvestorPositionInfo->HedgeFlag;  // 9.投機套保標誌 '1':投機  '2':套利  '3':套保
		AppendFieldToFieldsStr(ansiResponseStr, hedgeFlag, "|");
		char positionDate = pInvestorPositionInfo->PositionDate;  //10.持倉日期 '1':今日持倉  '2':歷史持倉
		AppendFieldToFieldsStr(ansiResponseStr, positionDate, "|");
		int ydPosition = pInvestorPositionInfo->YdPosition;  //11.上日持倉
		AppendFieldToFieldsStr(ansiResponseStr, ydPosition, "|");
		int tdPosition = pInvestorPositionInfo->Position;  //12.今日持倉
		AppendFieldToFieldsStr(ansiResponseStr, tdPosition, "|");
		int longFrozenPosition = pInvestorPositionInfo->LongFrozen;  //13.多頭凍結
		AppendFieldToFieldsStr(ansiResponseStr, longFrozenPosition, "|");
		int shortFrozenPosition = pInvestorPositionInfo->ShortFrozen;  //14.空頭凍結
		AppendFieldToFieldsStr(ansiResponseStr, shortFrozenPosition, "|");
		double longFrozenAmount = pInvestorPositionInfo->LongFrozenAmount;  //15.開倉凍結金額
		AppendFieldToFieldsStr(ansiResponseStr, longFrozenAmount, "|");
		double shortFrozenAmount = pInvestorPositionInfo->ShortFrozenAmount;  //16.平倉凍結金額
		AppendFieldToFieldsStr(ansiResponseStr, shortFrozenAmount, "|");
		int	openVolume = pInvestorPositionInfo->OpenVolume;  //17.開倉量
		AppendFieldToFieldsStr(ansiResponseStr, openVolume, "|");
		int	closeVolume = pInvestorPositionInfo->CloseVolume;  //18.平倉量
		AppendFieldToFieldsStr(ansiResponseStr, closeVolume, "|");
		double openAmount = pInvestorPositionInfo->OpenAmount;  //19.開倉金額
		AppendFieldToFieldsStr(ansiResponseStr, openAmount, "|");
		double closeAmount = pInvestorPositionInfo->CloseAmount;  //20.平倉金額
		AppendFieldToFieldsStr(ansiResponseStr, closeAmount, "|");
		double positionCost = pInvestorPositionInfo->PositionCost;  //21.持倉成本
		AppendFieldToFieldsStr(ansiResponseStr, positionCost, "|");
		double preMargin = pInvestorPositionInfo->PreMargin;  //22.上次佔用的保證金
		AppendFieldToFieldsStr(ansiResponseStr, preMargin, "|");
		double usedMargin = pInvestorPositionInfo->UseMargin;  //23.佔用的保證金
		AppendFieldToFieldsStr(ansiResponseStr, usedMargin, "|");
		double frozenMargin = pInvestorPositionInfo->FrozenMargin;  //24.凍結的保證金
		AppendFieldToFieldsStr(ansiResponseStr, frozenMargin, "|");
		double frozenCash = pInvestorPositionInfo->FrozenCash;  //25.凍結的資金
		AppendFieldToFieldsStr(ansiResponseStr, frozenCash, "|");
		double frozenCommission = pInvestorPositionInfo->FrozenCommission;  //26.凍結的手續費
		AppendFieldToFieldsStr(ansiResponseStr, frozenCommission, "|");
		double cashIn = pInvestorPositionInfo->CashIn;  //27.資金差額
		AppendFieldToFieldsStr(ansiResponseStr, cashIn, "|");
		double commission = pInvestorPositionInfo->Commission;  //28.手續費
		AppendFieldToFieldsStr(ansiResponseStr, commission, "|");
		double closeProfit = pInvestorPositionInfo->CloseProfit;  //29.平倉盈虧
		AppendFieldToFieldsStr(ansiResponseStr, closeProfit, "|");
		double positionProfit = pInvestorPositionInfo->PositionProfit;  //30.持倉盈虧
		AppendFieldToFieldsStr(ansiResponseStr, positionProfit, "|");
		double preSettlementPrice = pInvestorPositionInfo->PreSettlementPrice;  //31.上次結算價
		AppendFieldToFieldsStr(ansiResponseStr, preSettlementPrice, "|");
		double settlementPrice = pInvestorPositionInfo->SettlementPrice;  //32.本次結算價
		AppendFieldToFieldsStr(ansiResponseStr, settlementPrice, "|");
		UFC::AnsiString tradingDay = pInvestorPositionInfo->TradingDay;  //33.交易日
		AppendFieldToFieldsStr(ansiResponseStr, tradingDay, "|");
		int	settlementID = pInvestorPositionInfo->SettlementID;  //34.結算編號
		AppendFieldToFieldsStr(ansiResponseStr, settlementID, "|");
		double openCost = pInvestorPositionInfo->OpenCost;  //35.開倉成本
		AppendFieldToFieldsStr(ansiResponseStr, openCost, "|");
		double exchangeMargin = pInvestorPositionInfo->ExchangeMargin;  //36.交易所保證金
		AppendFieldToFieldsStr(ansiResponseStr, exchangeMargin, "|");
		int	combPosition = pInvestorPositionInfo->CombPosition;  //37.組合成交形成的持倉
		AppendFieldToFieldsStr(ansiResponseStr, combPosition, "|");
		int	combLongFrozen = pInvestorPositionInfo->CombLongFrozen;  //38.組合多頭凍結
		AppendFieldToFieldsStr(ansiResponseStr, combLongFrozen, "|");
		int	combShortFrozen = pInvestorPositionInfo->CombShortFrozen;  //39.組合空頭凍結
		AppendFieldToFieldsStr(ansiResponseStr, combShortFrozen, "|");
		int todayPosition = pInvestorPositionInfo->TodayPosition;  //40.今日持倉
		AppendFieldToFieldsStr(ansiResponseStr, todayPosition, "|");
		UFC::BufferedLog::Printf(" OnRspQryInvestorPosition:%s %s %s PosDate=%c Dir=%c YDPos=%d Pos=%d TradeDate=%s",
								 ansiBrokerId.c_str(), ansiInvestorId.c_str(), ansiInstrumentId.c_str(), positionDate, positionDirection, ydPosition, tdPosition, tradingDay.c_str());
		UFC::BufferedLog::Printf(" OnRspQryInvestorPosition:Open[Vol=%d Amt=%15.6lf] Close[Vol=%d Amt=%15.6lf] Cost[%15.6lf] Fee[%15.6lf] CloseProfit[%15.6lf] PosProfit[%15.6lf] TodayPos[%d]",
								 openVolume, openAmount, closeVolume, closeAmount, positionCost, commission, closeProfit, positionProfit, todayPosition);
		UFC::BufferedLog::Printf(" OnRspQryInvestorPosition:preSettlementPx[%15.6lf] SettlementPx[%15.6lf]",
								 preSettlementPrice, settlementPrice);
		UFC::BufferedLog::Printf(" OnRspQryInvestorPosition:[%s]", ansiResponseStr.c_str());
		UFC::BufferedLog::FlushToFile();
	}

	if (FListener != NULL)
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline("CTP");
		newsMsg.SetID(CTP_OnInvestorPosition);
		newsMsg.SetText(ansiResponseStr.c_str());
		FListener->OnNews(&newsMsg);
	}
	else
		UFC::BufferedLog::Printf("TTaifexConnection::OnRspQryInvestorPosition() %s", ansiResponseStr.c_str());
}  //TTaifexConnection::OnRspQryInvestorPosition()
//---------------------------------------------------------------------------
void TTaifexConnection::OnRspQryTransferSerial(CThostFtdcTransferSerialField *pTransferSerial, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	UFC::AnsiString ansiResponseStr = BuildResponseInformationStr(pRspInfo, "|");
	AppendFieldToFieldsStr(ansiResponseStr, nRequestID, "|");
	AppendFieldToFieldsStr(ansiResponseStr, bIsLast, "|");
	if (pTransferSerial != NULL)
	{
		int plateSerial = pTransferSerial->PlateSerial;  // 5.平台流水號
		AppendFieldToFieldsStr(ansiResponseStr, plateSerial, "|");
		UFC::AnsiString ansiTradeDate = pTransferSerial->TradeDate;  // 6.交易發起方日期
		AppendFieldToFieldsStr(ansiResponseStr, ansiTradeDate, "|");
		UFC::AnsiString ansiTradingDay =  pTransferSerial->TradingDay;  // 7.交易日期
		AppendFieldToFieldsStr(ansiResponseStr, ansiTradingDay, "|");
		UFC::AnsiString ansiTradeTime = pTransferSerial->TradeTime;  // 8.交易時間
		AppendFieldToFieldsStr(ansiResponseStr, ansiTradeTime, "|");
		UFC::AnsiString ansiTradeCode = pTransferSerial->TradeCode;  // 9.交易代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiTradeCode, "|");
		int sessionID = pTransferSerial->SessionID;  //10.會話編號
		AppendFieldToFieldsStr(ansiResponseStr, sessionID, "|");
		UFC::AnsiString ansiBankID = pTransferSerial->BankID;  //11.銀行編碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBankID, "|");
		UFC::AnsiString ansiBankBranchID = pTransferSerial->BankBranchID;  //12.銀行分支機構編碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBankBranchID, "|");
		char bankAccType = pTransferSerial->BankAccType;  //13.銀行帳號類型  '1':存摺 '2':儲蓄卡 '3':信用卡
		AppendFieldToFieldsStr(ansiResponseStr, bankAccType, "|");
		UFC::AnsiString ansiBankAccount = pTransferSerial->BankAccount;  //14.銀行帳號
		AppendFieldToFieldsStr(ansiResponseStr, ansiBankAccount, "|");
		UFC::AnsiString ansiBankSerial = pTransferSerial->BankSerial;  //15.銀行流水號
		AppendFieldToFieldsStr(ansiResponseStr, ansiBankSerial, "|");
		UFC::AnsiString ansiBrokerId = pTransferSerial->BrokerID;  //16.期貨公司編碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBrokerId, "|");
		UFC::AnsiString ansiBrokerBranchId = pTransferSerial->BrokerBranchID;  //17.期貨公司分支機構代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBrokerBranchId, "|");
		char futureAccType = pTransferSerial->FutureAccType;  //18.期貨公司帳號類型 '1':存摺 '2':儲蓄卡 '3':信用卡
		AppendFieldToFieldsStr(ansiResponseStr, futureAccType, "|");
		UFC::AnsiString ansiAccountId = pTransferSerial->AccountID;  //19.投資者帳號
		AppendFieldToFieldsStr(ansiResponseStr, ansiAccountId, "|");
		UFC::AnsiString ansiInvestorId = pTransferSerial->InvestorID;  //20.投資者代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiInvestorId, "|");
		int futureSerial = pTransferSerial->FutureSerial;  //21.期貨公司流水號
		AppendFieldToFieldsStr(ansiResponseStr, futureSerial, "|");
		char idCardType = pTransferSerial->IdCardType;  //22.證件類型
		AppendFieldToFieldsStr(ansiResponseStr, idCardType, "|");
		UFC::AnsiString ansiIdentifiedCardNo = pTransferSerial->IdentifiedCardNo;  //23.證件號碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiIdentifiedCardNo, "|");
		UFC::AnsiString ansiCurrencyId = pTransferSerial->CurrencyID;  //24.幣種代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiCurrencyId, "|");
		double tradeAmount = pTransferSerial->TradeAmount;  //25.交易金額
		AppendFieldToFieldsStr(ansiResponseStr, tradeAmount, "|");
		double customFee = pTransferSerial->CustFee;  //26.應收客戶費用
		AppendFieldToFieldsStr(ansiResponseStr, customFee, "|");
		double brokerFee = pTransferSerial->BrokerFee;  //27.應收期貨公司費用
		AppendFieldToFieldsStr(ansiResponseStr, brokerFee, "|");
		char availabilityFlag = pTransferSerial->AvailabilityFlag;  //28.有效標誌 '0':未確認 '1':有效 '2':沖正
		AppendFieldToFieldsStr(ansiResponseStr, availabilityFlag, "|");
		UFC::AnsiString ansiOperatorCode = pTransferSerial->OperatorCode;  //29.操作員
		AppendFieldToFieldsStr(ansiResponseStr, ansiOperatorCode, "|");
		UFC::AnsiString ansiBankNewAccount = pTransferSerial->BankNewAccount;  //30.新銀行帳號
		AppendFieldToFieldsStr(ansiResponseStr, ansiBankNewAccount, "|");
		int errorID = pTransferSerial->ErrorID;  //31.錯誤代碼
		AppendFieldToFieldsStr(ansiResponseStr, errorID, "|");
		GB2312String gb2312ErrorMsg(pTransferSerial->ErrorMsg);  //32.錯誤訊息
		UTF8String utf8ErrorMsg(gb2312ErrorMsg);
		UFC::AnsiString ansiErrorMsg(utf8ErrorMsg.c_str());
		AppendFieldToFieldsStr(ansiResponseStr, ansiErrorMsg, "|");
	}

	if (FListener != NULL)
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline("CTP");
		newsMsg.SetID(CTP_OnRspQryTransferSerial);
		newsMsg.SetText(ansiResponseStr.c_str());
		FListener->OnNews(&newsMsg);
	}
	else
		UFC::BufferedLog::Printf("TTaifexConnection::OnRspQryTransferSerial() %s", ansiResponseStr.c_str());
}  //TTaifexConnection::OnRspQryTransferSerial()
//---------------------------------------------------------------------------
void TTaifexConnection::OnRspQryTransferBank(CThostFtdcTransferBankField *pTransferBank, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	UFC::AnsiString ansiResponseStr = BuildResponseInformationStr(pRspInfo, "|");
	AppendFieldToFieldsStr(ansiResponseStr, nRequestID, "|");
	AppendFieldToFieldsStr(ansiResponseStr, bIsLast, "|");
	if (pTransferBank != NULL)
	{
		UFC::AnsiString ansiBankId = pTransferBank->BankID;  //5.銀行代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBankId, "|");
		UFC::AnsiString ansiBankBranchId = pTransferBank->BankBrchID;  //6.銀行分中心代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBankBranchId, "|");
		GB2312String gb2312BankName(pTransferBank->BankName);  //7.銀行名稱
		UTF8String utf8BankName(gb2312BankName);
		UFC::AnsiString ansiBankName(utf8BankName.c_str());
		AppendFieldToFieldsStr(ansiResponseStr, ansiBankName, "|");
		int isActive = pTransferBank->IsActive;  //8.是否活躍
		AppendFieldToFieldsStr(ansiResponseStr, isActive, "|");
	}

	if (FListener != NULL)
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline("CTP");
		newsMsg.SetID(CTP_OnRspQueryTransferBank);
		newsMsg.SetText(ansiResponseStr.c_str());
		FListener->OnNews(&newsMsg);
	}
	else
		UFC::BufferedLog::Printf("TTaifexConnection::OnRspQryTransferBank() %s", ansiResponseStr.c_str());
}  //TTaifexConnection::OnRspQryTransferBank()
//---------------------------------------------------------------------------
void TTaifexConnection::OnRspQryContractBank(CThostFtdcContractBankField *pContractBank, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	UFC::AnsiString ansiResponseStr = BuildResponseInformationStr(pRspInfo, "|");
	AppendFieldToFieldsStr(ansiResponseStr, nRequestID, "|");
	AppendFieldToFieldsStr(ansiResponseStr, bIsLast, "|");
	if (pContractBank != NULL)
	{
		UFC::AnsiString ansiBrokerId = pContractBank->BrokerID;  //5.經紀公司代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBrokerId, "|");
		UFC::AnsiString ansiBankId = pContractBank->BankID;  //6.銀行代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBankId, "|");
		UFC::AnsiString ansiBankBranchId = pContractBank->BankBrchID;  //7.銀行分中心代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBankBranchId, "|");
		GB2312String gb2312BankName(pContractBank->BankName);  //8.銀行名稱
		UTF8String utf8BankName(gb2312BankName);
		UFC::AnsiString ansiBankName(utf8BankName.c_str());
		AppendFieldToFieldsStr(ansiResponseStr, ansiBankName, "|");
	}

	if (FListener != NULL)
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline("CTP");
		newsMsg.SetID(CTP_OnRspQueryContractBank);
		newsMsg.SetText(ansiResponseStr.c_str());
		FListener->OnNews(&newsMsg);
	}
	else
		UFC::BufferedLog::Printf("TTaifexConnection::OnRspQryContractBank() %s", ansiResponseStr.c_str());
}  //TTaifexConnection::OnRspQryContractBank()
//---------------------------------------------------------------------------
void TTaifexConnection::OnRspQryAccountregister(CThostFtdcAccountregisterField *pAccountregister, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	UFC::AnsiString ansiResponseStr = BuildResponseInformationStr(pRspInfo, "|");
	AppendFieldToFieldsStr(ansiResponseStr, nRequestID, "|");
	AppendFieldToFieldsStr(ansiResponseStr, bIsLast, "|");
	if (pAccountregister != NULL)
	{
		UFC::AnsiString ansiTradeDate = pAccountregister->TradeDay;  // 5.交易日期
		AppendFieldToFieldsStr(ansiResponseStr, ansiTradeDate, "|");
		UFC::AnsiString ansiBankId = pAccountregister->BankID;  // 6.銀行代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBankId, "|");
		UFC::AnsiString ansiBankBranchId = pAccountregister->BankBranchID;  // 7.銀行分支機構代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBankBranchId, "|");
		UFC::AnsiString ansiBankAccount = pAccountregister->BankAccount;  // 8.銀行帳號
		AppendFieldToFieldsStr(ansiResponseStr, ansiBankAccount, "|");
		UFC::AnsiString ansiBrokerId = pAccountregister->BrokerID;  // 9.期貨公司編碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBrokerId, "|");
		UFC::AnsiString ansiBrokerBranchId = pAccountregister->BrokerBranchID;  //10.期貨公司分支機構編碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBrokerBranchId, "|");
		UFC::AnsiString ansiAccountId = pAccountregister->AccountID;  //11.投資者帳號
		AppendFieldToFieldsStr(ansiResponseStr, ansiAccountId, "|");
		char idCardType = pAccountregister->IdCardType;  //12.證件類型
		AppendFieldToFieldsStr(ansiResponseStr, idCardType, "|");
		UFC::AnsiString ansiIdentifiedCardNo = pAccountregister->IdentifiedCardNo;  //13.證件號碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiIdentifiedCardNo, "|");
		GB2312String gb2312CustomerName(pAccountregister->CustomerName);  //14.客戶姓名
		UTF8String utf8CustomerName(gb2312CustomerName);
		UFC::AnsiString ansiCustomerName(utf8CustomerName.c_str());
		AppendFieldToFieldsStr(ansiResponseStr, ansiCustomerName, "|");
		UFC::AnsiString ansiCurrencyId = pAccountregister->CurrencyID;  //15.幣種
		AppendFieldToFieldsStr(ansiResponseStr, ansiCurrencyId, "|");
		char openOrDestroy = pAccountregister->OpenOrDestroy;  //16.開銷戶類別 '1':開戶 '0':銷戶
		AppendFieldToFieldsStr(ansiResponseStr, openOrDestroy, "|");
		UFC::AnsiString ansiRegisterDate = pAccountregister->RegDate;  //17.簽約日期
		AppendFieldToFieldsStr(ansiResponseStr, ansiRegisterDate, "|");
		UFC::AnsiString ansiCancelDate = pAccountregister->OutDate;  //18.解約日期
		AppendFieldToFieldsStr(ansiResponseStr, ansiCancelDate, "|");
		int tradeId = pAccountregister->TID;  //19.交易ID
		AppendFieldToFieldsStr(ansiResponseStr, tradeId, "|");
		char customType = pAccountregister->CustType;  //20.客戶類型 '0':自然人 '1':機構戶
		AppendFieldToFieldsStr(ansiResponseStr, customType, "|");
		char bankAccountType = pAccountregister->BankAccType;  //21.銀行帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
		AppendFieldToFieldsStr(ansiResponseStr, bankAccountType, "|");
	}

	if (FListener != NULL)
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline("CTP");
		newsMsg.SetID(CTP_OnRspQueryAccountRegister);
		newsMsg.SetText(ansiResponseStr.c_str());
		FListener->OnNews(&newsMsg);
	}
	else
		UFC::BufferedLog::Printf("TTaifexConnection::OnRspQryAccountregister() %s", ansiResponseStr.c_str());
}  //TTaifexConnection::OnRspQryAccountregister()
//---------------------------------------------------------------------------
void TTaifexConnection::OnRspQryTradingCode(CThostFtdcTradingCodeField *pTradingCode, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	UFC::AnsiString ansiResponseStr = BuildResponseInformationStr(pRspInfo, "|");
	AppendFieldToFieldsStr(ansiResponseStr, nRequestID, "|");
	AppendFieldToFieldsStr(ansiResponseStr, bIsLast, "|");
	if (pTradingCode != NULL)
	{
		UFC::AnsiString ansiInvestorId = pTradingCode->InvestorID;  //5.投資者代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiInvestorId, "|");
		UFC::AnsiString ansiBrokerId = pTradingCode->BrokerID;  //6.經紀公司代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBrokerId, "|");
		UFC::AnsiString ansiExchangeId = pTradingCode->ExchangeID;  //7.交易所代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiExchangeId, "|");
		UFC::AnsiString ansiClientId = pTradingCode->ClientID;  //8.交易編碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiClientId, "|");
		bool isActive;  //9.是否活躍
		if (pTradingCode->IsActive == 1)
			isActive = true;
		else
			isActive = false;
		AppendFieldToFieldsStr(ansiResponseStr, isActive, "|");
		char clientIDType = pTradingCode->ClientIDType;  //10.交易編碼類型  '1':投機 '2':套利 '3':套保
		AppendFieldToFieldsStr(ansiResponseStr, clientIDType, "|");
	}

	if (FListener != NULL)
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline("CTP");
		newsMsg.SetID(CTP_OnRspQueryTradingCode);
		newsMsg.SetText(ansiResponseStr.c_str());
		FListener->OnNews(&newsMsg);
	}
	else
		UFC::BufferedLog::Printf("TTaifexConnection::OnRspQryTradingCode() %s", ansiResponseStr.c_str());
}  //TTaifexConnection::OnRspQryTradingCode()
//---------------------------------------------------------------------------
void TTaifexConnection::OnRspQueryBankAccountMoneyByFuture(CThostFtdcReqQueryAccountField *pReqQueryAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	UFC::AnsiString ansiResponseStr = BuildResponseInformationStr(pRspInfo, "|");
	AppendFieldToFieldsStr(ansiResponseStr, nRequestID, "|");
	AppendFieldToFieldsStr(ansiResponseStr, bIsLast, "|");
	if (pReqQueryAccount != NULL)
	{
		UFC::AnsiString ansiTradeCode = pReqQueryAccount->TradeCode;  // 5.業務功能碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiTradeCode, "|");
		UFC::AnsiString ansiBankId = pReqQueryAccount->BankID;  // 6.銀行代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBankId, "|");
		UFC::AnsiString ansiBankBranchId = pReqQueryAccount->BankBranchID;  // 7.銀行分支機構代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBankBranchId, "|");
		UFC::AnsiString ansiBrokerId = pReqQueryAccount->BrokerID;  // 8.期商代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBrokerId, "|");
		UFC::AnsiString ansiBrokerBranchId = pReqQueryAccount->BrokerBranchID;  // 9.期商分支機構代碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBrokerBranchId, "|");
		UFC::AnsiString ansiTradeDate = pReqQueryAccount->TradeDate;  //10.交易日期
		AppendFieldToFieldsStr(ansiResponseStr, ansiTradeDate, "|");
		UFC::AnsiString ansiTradeTime = pReqQueryAccount->TradeTime;  //11.交易時間
		AppendFieldToFieldsStr(ansiResponseStr, ansiTradeTime, "|");
		UFC::AnsiString ansiBankSerial = pReqQueryAccount->BankSerial;  //12.銀行流水號
		AppendFieldToFieldsStr(ansiResponseStr, ansiBankSerial, "|");
		UFC::AnsiString ansiSystemTradeDate = pReqQueryAccount->TradingDay;  //13.系統交易日期
		AppendFieldToFieldsStr(ansiResponseStr, ansiSystemTradeDate, "|");
		int plateSerial = pReqQueryAccount->PlateSerial;  //14.銀期平台消息流水號
		AppendFieldToFieldsStr(ansiResponseStr, plateSerial, "|");
		char lastFragment = pReqQueryAccount->LastFragment;  //15.最後分片標誌
		AppendFieldToFieldsStr(ansiResponseStr, lastFragment, "|");
		int sessionId = pReqQueryAccount->SessionID;  //16.會話號
		AppendFieldToFieldsStr(ansiResponseStr, sessionId, "|");
		GB2312String gb2312CustomerName(pReqQueryAccount->CustomerName);  //17.客戶姓名
		UTF8String utf8CustomerName(gb2312CustomerName);
		UFC::AnsiString ansiCustomerName(utf8CustomerName.c_str());
		AppendFieldToFieldsStr(ansiResponseStr, ansiCustomerName, "|");
		char idCardType = pReqQueryAccount->IdCardType;  //18.證件類型
		AppendFieldToFieldsStr(ansiResponseStr, idCardType, "|");
		UFC::AnsiString ansiIdentifiedCardNo = pReqQueryAccount->IdentifiedCardNo;  //19.證件號碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiIdentifiedCardNo, "|");
		char customType = pReqQueryAccount->CustType;  //20.客戶類型
		AppendFieldToFieldsStr(ansiResponseStr, customType, "|");
		UFC::AnsiString ansiBankAccount = pReqQueryAccount->BankAccount;  //21.銀行帳號
		AppendFieldToFieldsStr(ansiResponseStr, ansiBankAccount, "|");
		UFC::AnsiString ansiBankPassword = pReqQueryAccount->BankPassWord;  //22.銀行密碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBankPassword, "|");
		UFC::AnsiString ansiAccountId = pReqQueryAccount->AccountID;  //23.投資者帳號
		AppendFieldToFieldsStr(ansiResponseStr, ansiAccountId, "|");
		UFC::AnsiString ansiAccountPassword = pReqQueryAccount->Password;  //24.期貨密碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiAccountPassword, "|");
		int installID = pReqQueryAccount->InstallID;  //25.安裝編號
		AppendFieldToFieldsStr(ansiResponseStr, installID, "|");
		UFC::AnsiString ansiUserID = pReqQueryAccount->UserID;  //26.用戶標誌
		AppendFieldToFieldsStr(ansiResponseStr, ansiUserID, "|");
		char verifyCertNoFlag = pReqQueryAccount->VerifyCertNoFlag;  //27.驗證客戶證件號碼標誌 '0':是 '1':否
		AppendFieldToFieldsStr(ansiResponseStr, verifyCertNoFlag, "|");
		UFC::AnsiString ansiCurrencyId = pReqQueryAccount->CurrencyID;  //28.幣種
		AppendFieldToFieldsStr(ansiResponseStr, ansiCurrencyId, "|");
		UFC::AnsiString ansiDigest = pReqQueryAccount->Digest;  //29.摘要
		AppendFieldToFieldsStr(ansiResponseStr, ansiDigest, "|");
		char bankAccountType = pReqQueryAccount->BankAccType;  //30.銀行帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
		AppendFieldToFieldsStr(ansiResponseStr, bankAccountType, "|");
		UFC::AnsiString ansiDeviceId = pReqQueryAccount->DeviceID;  //31.渠道標誌
		AppendFieldToFieldsStr(ansiResponseStr, ansiDeviceId, "|");
		char bankSecurityAccountType = pReqQueryAccount->BankSecuAccType;  //32.期貨單位帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
		AppendFieldToFieldsStr(ansiResponseStr, bankSecurityAccountType, "|");
		UFC::AnsiString ansiBrokerIDByBank = pReqQueryAccount->BrokerIDByBank;  //33.期貨公司銀行編碼
		AppendFieldToFieldsStr(ansiResponseStr, ansiBrokerIDByBank, "|");
		UFC::AnsiString ansiBankSecurityAccount = pReqQueryAccount->BankSecuAcc;  //34.期貨單位帳號
		AppendFieldToFieldsStr(ansiResponseStr, ansiBankSecurityAccount, "|");
		char bankPasswordFlag = pReqQueryAccount->BankPwdFlag;  //35.銀行密碼標誌 '0':不核對  '1':明文核對  '2':密文核對
		AppendFieldToFieldsStr(ansiResponseStr, bankPasswordFlag, "|");
		char securityPasswordFlag = pReqQueryAccount->SecuPwdFlag;  //36.期貨資金密碼核對標誌 '0':不核對  '1':明文核對  '2':密文核對
		AppendFieldToFieldsStr(ansiResponseStr, securityPasswordFlag, "|");
		UFC::AnsiString ansiOperatorNo = pReqQueryAccount->OperNo;  //37.交易櫃員
		AppendFieldToFieldsStr(ansiResponseStr, ansiOperatorNo, "|");
		int requestId = pReqQueryAccount->RequestID;  //38.請求編號
		AppendFieldToFieldsStr(ansiResponseStr, requestId, "|");
		int tradeId = pReqQueryAccount->TID;  //39.交易ID
		AppendFieldToFieldsStr(ansiResponseStr, tradeId, "|");
	}

	if (FListener != NULL)
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline("CTP");
		newsMsg.SetID(CTP_OnRspQueryBankAccountMoneyByFuture);
		newsMsg.SetText(ansiResponseStr.c_str());
		FListener->OnNews(&newsMsg);
	}
	else
		UFC::BufferedLog::Printf("TTaifexConnection::OnRspQueryBankAccountMoneyByFuture() %s", ansiResponseStr.c_str());
}  //TTaifexConnection::OnRspQueryBankAccountMoneyByFuture()
//---------------------------------------------------------------------------
void TTaifexConnection::OnRtnQueryBankBalanceByFuture(CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount)
{
	UFC::AnsiString ansiResultStr = "";
	if (pNotifyQueryAccount != NULL)
	{
		int errorId = pNotifyQueryAccount->ErrorID;  //1.錯誤代碼
		AppendFieldToFieldsStr(ansiResultStr, errorId, "|");
		GB2312String gb2312ErrorMsg(pNotifyQueryAccount->ErrorMsg);  //2.錯誤訊息
		UTF8String utf8ErrorMsg(gb2312ErrorMsg);
		UFC::AnsiString ansiErrorMsg(utf8ErrorMsg.c_str());
		AppendFieldToFieldsStr(ansiResultStr, ansiErrorMsg, "|");
		UFC::AnsiString ansiTradeCode = pNotifyQueryAccount->TradeCode;  // 3.業務功能碼
		AppendFieldToFieldsStr(ansiResultStr, ansiTradeCode, "|");
		UFC::AnsiString ansiBankId = pNotifyQueryAccount->BankID;  // 4.銀行代碼
		AppendFieldToFieldsStr(ansiResultStr, ansiBankId, "|");
		UFC::AnsiString ansiBankBranchId = pNotifyQueryAccount->BankBranchID;  // 5.銀行分支機構代碼
		AppendFieldToFieldsStr(ansiResultStr, ansiBankBranchId, "|");
		UFC::AnsiString ansiBrokerId = pNotifyQueryAccount->BrokerID;  // 6.期商代碼
		AppendFieldToFieldsStr(ansiResultStr, ansiBrokerId, "|");
		UFC::AnsiString ansiBrokerBranchId = pNotifyQueryAccount->BrokerBranchID;  // 7.期商分支機構代碼
		AppendFieldToFieldsStr(ansiResultStr, ansiBrokerBranchId, "|");
		UFC::AnsiString ansiTradeDate =  pNotifyQueryAccount->TradeDate;  // 8.交易日期
		AppendFieldToFieldsStr(ansiResultStr, ansiTradeDate, "|");
		UFC::AnsiString ansiTradeTime = pNotifyQueryAccount->TradeTime;  // 9.交易時間
		AppendFieldToFieldsStr(ansiResultStr, ansiTradeTime, "|");
		UFC::AnsiString ansiBankSerial = pNotifyQueryAccount->BankSerial;  //10.銀行流水號
		AppendFieldToFieldsStr(ansiResultStr, ansiBankSerial, "|");
		UFC::AnsiString ansiSystemTradeDate = pNotifyQueryAccount->TradingDay;  //11.系統交易日期
		AppendFieldToFieldsStr(ansiResultStr, ansiSystemTradeDate, "|");
		int plateSerial = pNotifyQueryAccount->PlateSerial;  //12.銀期平台消息流水號
		AppendFieldToFieldsStr(ansiResultStr, plateSerial, "|");
		char lastFragment = pNotifyQueryAccount->LastFragment;  //13.最後分片標誌
		AppendFieldToFieldsStr(ansiResultStr, lastFragment, "|");
		int sessionId = pNotifyQueryAccount->SessionID;  //14.會話號
		AppendFieldToFieldsStr(ansiResultStr, sessionId, "|");
		GB2312String gb2312CustomerName(pNotifyQueryAccount->CustomerName);  //15.客戶姓名
		UTF8String utf8CustomerName(gb2312CustomerName);
		UFC::AnsiString ansiCustomerName(utf8CustomerName.c_str());
		AppendFieldToFieldsStr(ansiResultStr, ansiCustomerName, "|");
		char idCardType = pNotifyQueryAccount->IdCardType;  //16.證件類型
		AppendFieldToFieldsStr(ansiResultStr, idCardType, "|");
		UFC::AnsiString ansiIdentifiedCardNo = pNotifyQueryAccount->IdentifiedCardNo;  //17.證件號碼
		AppendFieldToFieldsStr(ansiResultStr, ansiIdentifiedCardNo, "|");
		char customType = pNotifyQueryAccount->CustType;  //18.客戶類型
		AppendFieldToFieldsStr(ansiResultStr, customType, "|");
		UFC::AnsiString ansiBankAccount = pNotifyQueryAccount->BankAccount;  //19.銀行帳號
		AppendFieldToFieldsStr(ansiResultStr, ansiBankAccount, "|");
		UFC::AnsiString ansiBankPassword = pNotifyQueryAccount->BankPassWord;  //20.銀行密碼
		AppendFieldToFieldsStr(ansiResultStr, ansiBankPassword, "|");
		UFC::AnsiString ansiAccountId = pNotifyQueryAccount->AccountID;  //21.投資者帳號
		AppendFieldToFieldsStr(ansiResultStr, ansiAccountId, "|");
		UFC::AnsiString ansiAccountPassword = pNotifyQueryAccount->Password;  //22.期貨密碼
		AppendFieldToFieldsStr(ansiResultStr, ansiAccountPassword, "|");
		int futureSerial = pNotifyQueryAccount->FutureSerial;  //23.期貨公司流水號
		AppendFieldToFieldsStr(ansiResultStr, futureSerial, "|");
		int installID = pNotifyQueryAccount->InstallID;  //24.安裝編號
		AppendFieldToFieldsStr(ansiResultStr, installID, "|");
		UFC::AnsiString ansiUserID = pNotifyQueryAccount->UserID;  //25.用戶標誌
		AppendFieldToFieldsStr(ansiResultStr, ansiUserID, "|");
		char verifyCertNoFlag = pNotifyQueryAccount->VerifyCertNoFlag;  //26.驗證客戶證件號碼標誌 '0':是 '1':否
		AppendFieldToFieldsStr(ansiResultStr, verifyCertNoFlag, "|");
		UFC::AnsiString ansiCurrencyId = pNotifyQueryAccount->CurrencyID;  //27.幣種
		AppendFieldToFieldsStr(ansiResultStr, ansiCurrencyId, "|");
		UFC::AnsiString ansiDigest = pNotifyQueryAccount->Digest;  //28.摘要
		AppendFieldToFieldsStr(ansiResultStr, ansiDigest, "|");
		char bankAccountType = pNotifyQueryAccount->BankAccType;  //29.銀行帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
		AppendFieldToFieldsStr(ansiResultStr, bankAccountType, "|");
		UFC::AnsiString ansiDeviceId = pNotifyQueryAccount->DeviceID;  //30.渠道標誌
		AppendFieldToFieldsStr(ansiResultStr, ansiDeviceId, "|");
		char bankSecurityAccountType = pNotifyQueryAccount->BankSecuAccType;  //31.期貨單位帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
		AppendFieldToFieldsStr(ansiResultStr, bankSecurityAccountType, "|");
		UFC::AnsiString ansiBrokerIDByBank = pNotifyQueryAccount->BrokerIDByBank;  //32.期貨公司銀行編碼
		AppendFieldToFieldsStr(ansiResultStr, ansiBrokerIDByBank, "|");
		UFC::AnsiString ansiBankSecurityAccount = pNotifyQueryAccount->BankSecuAcc;  //33.期貨單位帳號
		AppendFieldToFieldsStr(ansiResultStr, ansiBankSecurityAccount, "|");
		char bankPasswordFlag = pNotifyQueryAccount->BankPwdFlag;  //34.銀行密碼標誌 '0':不核對  '1':明文核對  '2':密文核對
		AppendFieldToFieldsStr(ansiResultStr, bankPasswordFlag, "|");
		char securityPasswordFlag = pNotifyQueryAccount->SecuPwdFlag;  //35.期貨資金密碼核對標誌 '0':不核對  '1':明文核對  '2':密文核對
		AppendFieldToFieldsStr(ansiResultStr, securityPasswordFlag, "|");
		UFC::AnsiString ansiOperatorNo = pNotifyQueryAccount->OperNo;  //36.交易櫃員
		AppendFieldToFieldsStr(ansiResultStr, ansiOperatorNo, "|");
		int requestId = pNotifyQueryAccount->RequestID;  //37.請求編號
		AppendFieldToFieldsStr(ansiResultStr, requestId, "|");
		int tradeId = pNotifyQueryAccount->TID;  //38.交易ID
		AppendFieldToFieldsStr(ansiResultStr, tradeId, "|");
		double bankUseAmount = pNotifyQueryAccount->BankUseAmount;  //39.銀行可用金額
		AppendFieldToFieldsStr(ansiResultStr, bankUseAmount, "|");
		double bankFetchAmount = pNotifyQueryAccount->BankFetchAmount;  //40.銀行可取金額
		AppendFieldToFieldsStr(ansiResultStr, bankFetchAmount, "|");
	}

	if (FListener != NULL)
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline("CTP");
		newsMsg.SetID(CTP_OnRtnQueryBankBalanceByFuture);
		newsMsg.SetText(ansiResultStr.c_str());
		FListener->OnNews(&newsMsg);
	}
	else
		UFC::BufferedLog::Printf("TTaifexConnection::OnRtnQueryBankBalanceByFuture() %s", ansiResultStr.c_str());
}  //TTaifexConnection::OnRtnQueryBankBalanceByFuture()
//---------------------------------------------------------------------------
UFC::AnsiString TTaifexConnection::BuildTransferRequestStr(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast, CThostFtdcReqTransferField *pReqTransfer)
{
	UFC::AnsiString ansiTransferRequestStr = BuildResponseInformationStr(pRspInfo, "|");
	AppendFieldToFieldsStr(ansiTransferRequestStr, nRequestID, "|");
	AppendFieldToFieldsStr(ansiTransferRequestStr, bIsLast, "|");
	if (pReqTransfer != NULL)
	{
		UFC::AnsiString ansiTradeCode = pReqTransfer->TradeCode;  // 5.業務功能碼
		AppendFieldToFieldsStr(ansiTransferRequestStr, ansiTradeCode, "|");
		UFC::AnsiString ansiBankId = pReqTransfer->BankID;  // 6.銀行代碼
		AppendFieldToFieldsStr(ansiTransferRequestStr, ansiBankId, "|");
		UFC::AnsiString ansiBankBranchId = pReqTransfer->BankBranchID;  // 7.銀行分支機構代碼
		AppendFieldToFieldsStr(ansiTransferRequestStr, ansiBankBranchId, "|");
		UFC::AnsiString ansiBrokerId = pReqTransfer->BrokerID;  // 8.期貨公司代碼
		AppendFieldToFieldsStr(ansiTransferRequestStr, ansiBrokerId, "|");
		UFC::AnsiString ansiTradeDate = pReqTransfer->TradeDate;  // 9.交易日期
		AppendFieldToFieldsStr(ansiTransferRequestStr, ansiTradeDate, "|");
		UFC::AnsiString ansiTradeTime = pReqTransfer->TradeTime;  //10.交易時間
		AppendFieldToFieldsStr(ansiTransferRequestStr, ansiTradeTime, "|");
		UFC::AnsiString ansiBankSerial = pReqTransfer->BankSerial;  //11.銀行流水號
		AppendFieldToFieldsStr(ansiTransferRequestStr, ansiBankSerial, "|");
		UFC::AnsiString ansiSystemTradeDate = pReqTransfer->TradingDay;  //12.系統交易日期
		AppendFieldToFieldsStr(ansiTransferRequestStr, ansiSystemTradeDate, "|");
		int plateSerial = pReqTransfer->PlateSerial;  //13.銀期平台消息流水號
		AppendFieldToFieldsStr(ansiTransferRequestStr, plateSerial, "|");
		char lastFragment = pReqTransfer->LastFragment;  //14.最後分片標誌
		AppendFieldToFieldsStr(ansiTransferRequestStr, lastFragment, "|");
		int sessionId = pReqTransfer->SessionID;  //15.會話號
		AppendFieldToFieldsStr(ansiTransferRequestStr, sessionId, "|");
		UFC::AnsiString ansiCustomerName = pReqTransfer->CustomerName;  //16.客戶姓名
		AppendFieldToFieldsStr(ansiTransferRequestStr, ansiCustomerName, "|");
		char idCardType = pReqTransfer->IdCardType;  //17.證件類型
		AppendFieldToFieldsStr(ansiTransferRequestStr, idCardType, "|");
		UFC::AnsiString ansiIdentifiedCardNo = pReqTransfer->IdentifiedCardNo;  //18.證件號碼
		AppendFieldToFieldsStr(ansiTransferRequestStr, ansiIdentifiedCardNo, "|");
		char customType = pReqTransfer->CustType;  //19.客戶類型
		AppendFieldToFieldsStr(ansiTransferRequestStr, customType, "|");
		UFC::AnsiString ansiBankAccount = pReqTransfer->BankAccount;  //20.銀行帳號
		AppendFieldToFieldsStr(ansiTransferRequestStr, ansiBankAccount, "|");
		UFC::AnsiString ansiBankPassword = pReqTransfer->BankPassWord;  //21.銀行密碼
		AppendFieldToFieldsStr(ansiTransferRequestStr, ansiBankPassword, "|");
		UFC::AnsiString ansiAccountId = pReqTransfer->AccountID;  //22.投資者帳號
		AppendFieldToFieldsStr(ansiTransferRequestStr, ansiAccountId, "|");
		UFC::AnsiString ansiAccountPassword = pReqTransfer->Password;  //23.期貨密碼
		AppendFieldToFieldsStr(ansiTransferRequestStr, ansiAccountPassword, "|");
		UFC::AnsiString ansiCurrencyId = pReqTransfer->CurrencyID;  //24.幣種
		AppendFieldToFieldsStr(ansiTransferRequestStr, ansiCurrencyId, "|");
		double tradeAmount = pReqTransfer->TradeAmount;  //25.轉帳金額
		AppendFieldToFieldsStr(ansiTransferRequestStr, tradeAmount, "|");
		double futureFetchAmount = pReqTransfer->FutureFetchAmount;  //26.期貨可取金額
		AppendFieldToFieldsStr(ansiTransferRequestStr, futureFetchAmount, "|");
		char feePaymentFlag = pReqTransfer->FeePayFlag;  //27.費用支付標誌 '0':由受益方支付  '1':由發送方支付  '2':由發送方支付發起的費用，由受益方支付接受的費用
		AppendFieldToFieldsStr(ansiTransferRequestStr, feePaymentFlag, "|");
		double customFee = pReqTransfer->CustFee;  //28.應收客戶費用
		AppendFieldToFieldsStr(ansiTransferRequestStr, customFee, "|");
		double brokerFee = pReqTransfer->BrokerFee;  //29.應收期貨公司費用
		AppendFieldToFieldsStr(ansiTransferRequestStr, brokerFee, "|");
		UFC::AnsiString ansiMessage = pReqTransfer->Message;  //30.發送方給接收方的消息
		AppendFieldToFieldsStr(ansiTransferRequestStr, ansiMessage, "|");
		UFC::AnsiString ansiDigest = pReqTransfer->Digest;  //31.摘要
		AppendFieldToFieldsStr(ansiTransferRequestStr, ansiDigest, "|");
		char bankAccountType = pReqTransfer->BankAccType;  //32.銀行帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
		AppendFieldToFieldsStr(ansiTransferRequestStr, bankAccountType, "|");
		UFC::AnsiString ansiDeviceId = pReqTransfer->DeviceID;  //33.渠道標誌
		AppendFieldToFieldsStr(ansiTransferRequestStr, ansiDeviceId, "|");
		char bankSecurityAccountType = pReqTransfer->BankSecuAccType;  //34.期貨單位帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
		AppendFieldToFieldsStr(ansiTransferRequestStr, bankSecurityAccountType, "|");
		UFC::AnsiString ansiBrokerIDByBank = pReqTransfer->BrokerIDByBank;  //35.期貨公司銀行編碼
		AppendFieldToFieldsStr(ansiTransferRequestStr, ansiBrokerIDByBank, "|");
		UFC::AnsiString ansiBankSecurityAccount = pReqTransfer->BankSecuAcc;  //34.期貨單位帳號
		AppendFieldToFieldsStr(ansiTransferRequestStr, ansiBankSecurityAccount, "|");
		char bankPasswordFlag = pReqTransfer->BankPwdFlag;  //36.銀行密碼標誌 '0':不核對  '1':明文核對  '2':密文核對
		AppendFieldToFieldsStr(ansiTransferRequestStr, bankPasswordFlag, "|");
		char securityPasswordFlag = pReqTransfer->SecuPwdFlag;  //37.期貨資金密碼核對標誌 '0':不核對  '1':明文核對  '2':密文核對
		AppendFieldToFieldsStr(ansiTransferRequestStr, securityPasswordFlag, "|");
		UFC::AnsiString ansiOperatorNo = pReqTransfer->OperNo;  //38.交易櫃員
		AppendFieldToFieldsStr(ansiTransferRequestStr, ansiOperatorNo, "|");
		int requestId = pReqTransfer->RequestID;  //39.請求編號
		AppendFieldToFieldsStr(ansiTransferRequestStr, requestId, "|");
		int tradeId = pReqTransfer->TID;  //40.交易ID
		AppendFieldToFieldsStr(ansiTransferRequestStr, tradeId, "|");
		char transferStatus = pReqTransfer->TransferStatus;  //41.轉帳交易狀態 '0':正常  '1':被沖正
		AppendFieldToFieldsStr(ansiTransferRequestStr, transferStatus, "|");
	}
	return ansiTransferRequestStr;
}  //TTaifexConnection::BuildTransferRequestStr()
//---------------------------------------------------------------------------
void TTaifexConnection::OnRspFromBankToFutureByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	UFC::AnsiString ansiNewsText = BuildTransferRequestStr(pRspInfo, nRequestID, bIsLast, pReqTransfer);
	if (FListener != NULL)
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline("CTP");
		newsMsg.SetID(CTP_OnRspFromBankToFutureByFuture);
		newsMsg.SetText(ansiNewsText.c_str());
		FListener->OnNews(&newsMsg);
	}
	else
		UFC::BufferedLog::Printf("TTaifexConnection::OnRspFromBankToFutureByFuture() %s", ansiNewsText.c_str());
}  //TTaifexConnection::OnRspFromBankToFutureByFuture()
//---------------------------------------------------------------------------
void TTaifexConnection::OnRspFromFutureToBankByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	UFC::AnsiString ansiNewsText = BuildTransferRequestStr(pRspInfo, nRequestID, bIsLast, pReqTransfer);
	if (FListener != NULL)
	{
		TNewsMessage newsMsg;
		newsMsg.SetHeadline("CTP");
		newsMsg.SetID(CTP_OnRspFromFutureToBankByFuture);
		newsMsg.SetText(ansiNewsText.c_str());
		FListener->OnNews(&newsMsg);
	}
	else
		UFC::BufferedLog::Printf("TTaifexConnection::OnRspFromFutureToBankByFuture() %s", ansiNewsText.c_str());
}  //TTaifexConnection::OnRspFromFutureToBankByFuture()
//---------------------------------------------------------------------------
UFC::AnsiString TTaifexConnection::BuildTransferResponseStr(CThostFtdcRspTransferField *pRspTransfer)
{
	UFC::AnsiString ansiTransferResponseStr = "";
	int errorId = pRspTransfer->ErrorID;  //1.錯誤代碼
	AppendFieldToFieldsStr(ansiTransferResponseStr, errorId, "|");
	GB2312String gb2312ErrorMsg(pRspTransfer->ErrorMsg);  //2.錯誤訊息
	UTF8String utf8ErrorMsg(gb2312ErrorMsg);
	UFC::AnsiString ansiErrorMsg(utf8ErrorMsg.c_str());
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiErrorMsg, "|");
	UFC::AnsiString ansiTradeCode = pRspTransfer->TradeCode;  //3.業務功能碼
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiTradeCode, "|");
	UFC::AnsiString ansiBankId = pRspTransfer->BankID;  //4.銀行代碼
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiBankId, "|");
	UFC::AnsiString ansiBankBranchId = pRspTransfer->BankBranchID;  //5.銀行分支機構代碼
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiBankBranchId, "|");
	UFC::AnsiString ansiBrokerId = pRspTransfer->BrokerID;  //6.期貨公司代碼
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiBrokerId, "|");
	UFC::AnsiString ansiTradeDate = pRspTransfer->TradeDate;  //7.交易日期
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiTradeDate, "|");
	UFC::AnsiString ansiTradeTime = pRspTransfer->TradeTime;  //8.交易時間
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiTradeTime, "|");
	UFC::AnsiString ansiBankSerial = pRspTransfer->BankSerial;  //9.銀行流水號
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiBankSerial, "|");
	UFC::AnsiString ansiSystemTradeDate = pRspTransfer->TradingDay;  //10.系統交易日期
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiSystemTradeDate, "|");
	int plateSerial = pRspTransfer->PlateSerial;  //11.銀期平台消息流水號
	AppendFieldToFieldsStr(ansiTransferResponseStr, plateSerial, "|");
	char lastFragment = pRspTransfer->LastFragment;  //12.最後分片標誌
	AppendFieldToFieldsStr(ansiTransferResponseStr, lastFragment, "|");
	int sessionId = pRspTransfer->SessionID;  //13.會話號
	AppendFieldToFieldsStr(ansiTransferResponseStr, sessionId, "|");
	GB2312String gb2312CustomerName(pRspTransfer->CustomerName);  //14.客戶姓名
	UTF8String utf8CustomerName(gb2312CustomerName);
	UFC::AnsiString ansiCustomerName(utf8CustomerName.c_str());
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiCustomerName, "|");
	char idCardType = pRspTransfer->IdCardType;  //15.證件類型
	AppendFieldToFieldsStr(ansiTransferResponseStr, idCardType, "|");
	UFC::AnsiString ansiIdentifiedCardNo = pRspTransfer->IdentifiedCardNo;  //16.證件號碼
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiIdentifiedCardNo, "|");
	char customType = pRspTransfer->CustType;  //17.客戶類型
	AppendFieldToFieldsStr(ansiTransferResponseStr, customType, "|");
	UFC::AnsiString ansiBankAccount = pRspTransfer->BankAccount;  //18.銀行帳號
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiBankAccount, "|");
	UFC::AnsiString ansiBankPassword = pRspTransfer->BankPassWord;  //19.銀行密碼
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiBankPassword, "|");
	UFC::AnsiString ansiAccountId = pRspTransfer->AccountID;  //20.投資者帳號
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiAccountId, "|");
	UFC::AnsiString ansiAccountPassword = pRspTransfer->Password;  //21.期貨密碼
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiAccountPassword, "|");
	int futureSerial = pRspTransfer->FutureSerial;  //22.期貨公司流水號
	AppendFieldToFieldsStr(ansiTransferResponseStr, futureSerial, "|");
	UFC::AnsiString ansiCurrencyId = pRspTransfer->CurrencyID;  //23.幣種
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiCurrencyId, "|");
	double tradeAmount = pRspTransfer->TradeAmount;  //24.轉帳金額
	AppendFieldToFieldsStr(ansiTransferResponseStr, tradeAmount, "|");
	double futureFetchAmount = pRspTransfer->FutureFetchAmount;  //25.期貨可取金額
	AppendFieldToFieldsStr(ansiTransferResponseStr, futureFetchAmount, "|");
	char feePaymentFlag = pRspTransfer->FeePayFlag;  //26.費用支付標誌 '0':由受益方支付  '1':由發送方支付  '2':由發送方支付發起的費用，由受益方支付接受的費用
	AppendFieldToFieldsStr(ansiTransferResponseStr, feePaymentFlag, "|");
	double customFee = pRspTransfer->CustFee;  //27.應收客戶費用
	AppendFieldToFieldsStr(ansiTransferResponseStr, customFee, "|");
	double brokerFee = pRspTransfer->BrokerFee;  //28.應收期貨公司費用
	AppendFieldToFieldsStr(ansiTransferResponseStr, brokerFee, "|");
	UFC::AnsiString ansiMessage = pRspTransfer->Message;  //29.發送方給接收方的消息
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiMessage, "|");
	UFC::AnsiString ansiDigest = pRspTransfer->Digest;  //30.摘要
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiDigest, "|");
	char bankAccountType = pRspTransfer->BankAccType;  //31.銀行帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
	AppendFieldToFieldsStr(ansiTransferResponseStr, bankAccountType, "|");
	UFC::AnsiString ansiDeviceId = pRspTransfer->DeviceID;  //32.渠道標誌
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiDeviceId, "|");
	char bankSecurityAccountType = pRspTransfer->BankSecuAccType;  //33.期貨單位帳號類型 '1':銀行存摺  '2':儲蓄卡  '3':信用卡
	AppendFieldToFieldsStr(ansiTransferResponseStr, bankSecurityAccountType, "|");
	UFC::AnsiString ansiBrokerIDByBank = pRspTransfer->BrokerIDByBank;  //34.期貨公司銀行編碼
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiBrokerIDByBank, "|");
	UFC::AnsiString ansiBankSecurityAccount = pRspTransfer->BankSecuAcc;  //35.期貨單位帳號
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiBankSecurityAccount, "|");
	char bankPasswordFlag = pRspTransfer->BankPwdFlag;  //36.銀行密碼標誌 '0':不核對  '1':明文核對  '2':密文核對
	AppendFieldToFieldsStr(ansiTransferResponseStr, bankPasswordFlag, "|");
	char securityPasswordFlag = pRspTransfer->SecuPwdFlag;  //37.期貨資金密碼核對標誌 '0':不核對  '1':明文核對  '2':密文核對
	AppendFieldToFieldsStr(ansiTransferResponseStr, securityPasswordFlag, "|");
	UFC::AnsiString ansiOperatorNo = pRspTransfer->OperNo;  //38.交易櫃員
	AppendFieldToFieldsStr(ansiTransferResponseStr, ansiOperatorNo, "|");
	int requestId = pRspTransfer->RequestID;  //39.請求編號
	AppendFieldToFieldsStr(ansiTransferResponseStr, requestId, "|");
	int tradeId = pRspTransfer->TID;  //40.交易ID
	AppendFieldToFieldsStr(ansiTransferResponseStr, tradeId, "|");
	char transferStatus = pRspTransfer->TransferStatus;  //41.轉帳交易狀態 '0':正常  '1':被沖正
	AppendFieldToFieldsStr(ansiTransferResponseStr, transferStatus, "|");
	return ansiTransferResponseStr;
}  //TTaifexConnection::BuildTransferResponseStr()
//---------------------------------------------------------------------------
void TTaifexConnection::OnRtnFromBankToFutureByFuture(CThostFtdcRspTransferField *pRspTransfer)
{
	if (pRspTransfer == NULL)
	{
		UFC::BufferedLog::Printf(" OnRtnFromBankToFutureByFuture: pRspTransfer is NULL.");
		UFC::BufferedLog::FlushToFile();
		return;
	}

	if (FListener != NULL)
	{
		UFC::AnsiString newsText = BuildTransferResponseStr(pRspTransfer);
		TNewsMessage newsMsg;
		newsMsg.SetHeadline("CTP");
		newsMsg.SetID(CTP_OnRtnFromBankToFutureByFuture);
		newsMsg.SetText(newsText.c_str());
		FListener->OnNews(&newsMsg);
	}
}  //TTaifexConnection::OnRtnFromBankToFutureByFuture()
//---------------------------------------------------------------------------
void TTaifexConnection::OnRtnFromFutureToBankByFuture(CThostFtdcRspTransferField *pRspTransfer)
{
	if (pRspTransfer == NULL)
	{
		UFC::BufferedLog::Printf(" OnRtnFromFutureToBankByFuture: pRspTransfer is NULL.");
		UFC::BufferedLog::FlushToFile();
		return;
	}

	if (FListener != NULL)
	{
		UFC::AnsiString newsText = BuildTransferResponseStr(pRspTransfer);
		TNewsMessage newsMsg;
		newsMsg.SetHeadline("CTP");
		newsMsg.SetID(CTP_OnRtnFromFutureToBankByFuture);
		newsMsg.SetText(newsText.c_str());
		FListener->OnNews(&newsMsg);
	}
}  //TTaifexConnection::OnRtnFromFutureToBankByFuture()
//---------------------------------------------------------------------------
void TTaifexConnection::OnRspError(CThostFtdcRspInfoField *pRspInfo, int ,bool )
{
	if( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		UTF8String   ErrMsgUTF8 = ErrMsg;
		TNewsMessage newsMsg;

		newsMsg.SetHeadline("CTP");
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
void TTaifexConnection::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField * )
{
	if( pOrderAction != NULL )
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

		UFC::BufferedLog::Printf( " OnErrRtnOrderAction: Symbol[%s] Exchange[%s] OrderSysID[%s] Msg[%s]",
								  pOrderAction->InstrumentID,
								  pOrderAction->ExchangeID,
								  pOrderAction->OrderSysID,
								  ErrMsgUTF8.c_str() );
		if( ExchangeID.Length() == 0 || OrderSysID.Length() == 0 )
			return;
		if( ( Ord = FindOrderByOrderSysID( ExchangeID, OrderSysID.ToInt64())) != NULL )
		{
			OrderSysID.TrimLeft();
			UFC::BufferedLog::Printf( " ----------------------- CTP Cancel Reject --------------------------" );
			ExecID.Printf( "CR_%s_%s_%d", ExchangeID.c_str(), OrderSysID.c_str(), atoi(pOrderAction->OrderRef) );
			CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Ord );
			ExecutionReport.SetOrderID( OrderSysID.c_str() );
			ExecutionReport.SetNID( Ord->GetOrderNID() );
			ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osRejected );
			ExecutionReport.SetExecType( nsOrderMessageDefine::etRejected );
			ExecutionReport.SetCxlRejResponseTo( nsOrderMessageDefine::crrCancel );
			ExecutionReport.SetAllUserData( Ord->GetCxlUserData() );
			ExecutionReport.SetExecID( ExecID.c_str());
			ExecutionReport.SetLastQty( 0 );
			ExecutionReport.SetTransactTime( pOrderAction->ActionTime );
			IsDup = IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
			UFC::BufferedLog::Printf( " OrderRef[%s] ExchangeID[%s] Symbol[%s] OrderSysID[%s]", pOrderAction->OrderRef, ExchangeID.c_str(), pOrderAction->InstrumentID, OrderSysID.c_str() );
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
// New Order error from Exchange.(Log only, not handle)
//---------------------------------------------------------------------------
void TTaifexConnection::OnErrRtnOrderInsert( CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
	if( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String     ErrMsg( pRspInfo->ErrorMsg );
		UTF8String       ErrMsgUTF8 = ErrMsg;

		if( pInputOrder != NULL )
		{
			UpdateRecover();
			UFC::BufferedLog::Printf( " OnErrRtnOrderInsert: Symbol[%s] OrderRef[%s] Msg[%s]",
									  pInputOrder->InstrumentID,
									  pInputOrder->OrderRef,
									  ErrMsgUTF8.c_str() );
		}
	}
}
//---------------------------------------------------------------------------
// CTP Reject NewOrder.
//---------------------------------------------------------------------------
void TTaifexConnection::OnRspOrderInsert( CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int , bool )
{
	if( pRspInfo != NULL && pRspInfo->ErrorID != 0 && pInputOrder != NULL )
	{
		GB2312String     ErrMsg( pRspInfo->ErrorMsg );
		UTF8String       ErrMsgUTF8 = ErrMsg;
		UFC::AnsiString  PEStr;
		TNewOrderMessage New;

		UpdateRecover();
		UFC::BufferedLog::Printf( " ------------------------- CTP Reject ----------------------------" );
		UFC::BufferedLog::Printf( " OnRspOrderInsert: OrderRef[%s] ",  pInputOrder->OrderRef );
		New.SetAccount( pInputOrder->UserID );
		New.SetBrokerID( pInputOrder->BrokerID );
		New.SetSymbol( pInputOrder->InstrumentID );
		New.SetMarket( nsOrderMessageDefine::mCNFutures );
		New.SetNID( COrderInfo::ToNID( FCTPFrontID, FCTPSessionID, atoi( pInputOrder->OrderRef )) );
		if(	pInputOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Open ) //Open
		{
			New.SetPositionEffect( nsOrderMessageDefine::peOpen );
			PEStr = "Open";
		}
		else if( pInputOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Close )//Close
		{
			New.SetPositionEffect( nsOrderMessageDefine::peClose );
			PEStr = "Close";
		}
		else if( pInputOrder->CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday )//Close Today
		{
			New.SetPositionEffect( nsOrderMessageDefine::peRolled );
			PEStr = "Close Today";
		}
		if(	pInputOrder->Direction == THOST_FTDC_D_Buy )
			New.SetSide( nsOrderMessageDefine::sBuy );
		else
			New.SetSide( nsOrderMessageDefine::sSell );
		if( pInputOrder->OrderPriceType == THOST_FTDC_OPT_AnyPrice )
		{
			New.SetOrderType( nsOrderMessageDefine::otMarket );
			New.SetPrice( 0.0 );
		}
		else
		{
			New.SetOrderType( nsOrderMessageDefine::otLimit );
			New.SetPrice( pInputOrder->LimitPrice );
		}
		if( pInputOrder->TimeCondition == THOST_FTDC_TC_IOC )
			New.SetTimeInForce( nsOrderMessageDefine::tifIOC );
		else
			New.SetTimeInForce( nsOrderMessageDefine::tifROD );
		New.SetOrderQty( pInputOrder->VolumeTotalOriginal );
		UFC::BufferedLog::Printf( " NID[%s] Symbol[%s] PositionEffect[%s]", pInputOrder->OrderRef, pInputOrder->InstrumentID, PEStr.c_str() );
		Reject( nsOrderMessageDefine::crrNew, ErrMsgUTF8.c_str(), &New, New.GetTradingSessionID() );
		TradeLogCTPConfirm( pInputOrder, pRspInfo->ErrorID );
		UFC::BufferedLog::Printf( " ------------------- Sent Reject Execution -----------------------" );
	}
}
//---------------------------------------------------------------------------
void TTaifexConnection::CTPFillExecBase( nsOrderMessageDefine::MarketEnum Market, TExecutionReportMessage& ExecutionReport, COrderInfo* Ord )
{
	ExecutionReport.SetMarket( Market );
	ExecutionReport.SetAE( Ord->GetUserID() );
	ExecutionReport.SetOrderType( Ord->GetOrderType() );
	ExecutionReport.SetSide( Ord->GetSide() );
	ExecutionReport.SetPositionEffect( Ord->GetPositionEffect() );
	ExecutionReport.SetTimeInForce( Ord->GetTimeInForce() );
	ExecutionReport.SetAccount( Ord->GetUserID() );
	ExecutionReport.SetExchangeCode( Ord->GetExchangeID() );
	ExecutionReport.SetSymbol( Ord->GetSymbol() );
	ExecutionReport.SetPrice( Ord->GetPrice() );
	///< Fields about qty
	ExecutionReport.SetOrderQty( Ord->GetOrderQty() );
	ExecutionReport.SetCumQty( Ord->GetCumQty() );
	ExecutionReport.SetLeavesQty( Ord->GetLeavesQty() );
}
//---------------------------------------------------------------------------
void TTaifexConnection::CTPPendingExecution( COrderInfo* Ord, CThostFtdcOrderField *pOrder)
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	GB2312String            ErrMsg( pOrder->StatusMsg );
	UFC::AnsiString         OrderLocalID( pOrder->OrderLocalID );
	Int64                   NID64 = Ord->GetOrderNID();
	bool IsDup;

	UFC::BufferedLog::Printf( " ---------------------------- CTP Pending ------------------------" );
	ExecID.Printf( "P_%d_%d_%d", Ord->GetFrontID(), Ord->GetSessionID(), Ord->GetOrderRef() );
	OrderLocalID.TrimLeft();
	CTPFillExecBase( nsOrderMessageDefine::mCNFutures, ExecutionReport, Ord );
	ExecutionReport.SetOrderID( OrderLocalID.c_str() );
	ExecutionReport.SetNID( NID64 );
	ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osPendingNew );
	ExecutionReport.SetExecType( nsOrderMessageDefine::etPendingNew );
	ExecutionReport.SetAllUserData( Ord->GetUserData() );
	ExecutionReport.SetExecID( ExecID.c_str());
	ExecutionReport.SetLastQty( 0 );
	ExecutionReport.SetTransactTime( pOrder->InsertTime );
	UFC::BufferedLog::Printf( " NID[%s] ExchangeID[%s] Symbol[%s] OrderSysID[%s] OrderLocalID[%s]", pOrder->OrderRef, pOrder->ExchangeID, pOrder->InstrumentID, pOrder->OrderSysID, pOrder->OrderLocalID );
	IsDup = IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
	if( ErrMsg.Length() > 0 )
	{
		UTF8String  ErrMsgUTF8( ErrMsg );
		ExecutionReport.SetText( ErrMsgUTF8.c_str());
		UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] Msg[%s] NID[%lld]", ExecID.c_str(), IsDup ? "Yes" : "No", ErrMsgUTF8.c_str(), ExecutionReport.GetNID());
	}
	else
		UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] ", ExecID.c_str(), IsDup ? "Yes" : "No" );
	TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() ));
	UFC::BufferedLog::Printf( " ---------------- Sent Pending New Execution ---------------------" );
}
//---------------------------------------------------------------------------
void TTaifexConnection::CTPConfirmExecution( COrderInfo* Ord, CThostFtdcOrderField *pOrder)
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	GB2312String            ErrMsg( pOrder->StatusMsg );
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
	UFC::BufferedLog::Printf( " NID[%s] ExchangeID[%s] Symbol[%s] OrderSysID[%s] OrderLocalID[%s]", pOrder->OrderRef, pOrder->ExchangeID, pOrder->InstrumentID, pOrder->OrderSysID, pOrder->OrderLocalID );
	IsDup = IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
	if( ErrMsg.Length() > 0 )
	{
		UTF8String   ErrMsgUTF8( ErrMsg );
		ExecutionReport.SetText( ErrMsgUTF8.c_str());
		UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] Msg[%s] NID[%lld]", ExecID.c_str(), IsDup ? "Yes" : "No", ErrMsgUTF8.c_str(), ExecutionReport.GetNID());
	}
	else
		UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] ", ExecID.c_str(), IsDup ? "Yes" : "No" );
	TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() ));
	UFC::BufferedLog::Printf( " ----------------- Sent Confirmed Execution ----------------------" );
}
//---------------------------------------------------------------------------
void TTaifexConnection::CTPCanceledExecution( COrderInfo* Ord, CThostFtdcOrderField *pOrder)
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	GB2312String            ErrMsg( pOrder->StatusMsg );
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
	UFC::BufferedLog::Printf( " OrdNID[%s] NID[%lld] ExchangeID[%s] Symbol[%s] OrderSysID[%s] OrderLocalID[%s]", pOrder->OrderRef, ExecutionReport.GetNID(), pOrder->ExchangeID, pOrder->InstrumentID, pOrder->OrderSysID, pOrder->OrderLocalID );
	IsDup = IsExecutionDup( ExecutionReport.GetMarket(), ExecutionReport.GetExecID() );
	if( ErrMsg.Length() > 0 )
	{
		UTF8String   ErrMsgUTF8( ErrMsg );
		ExecutionReport.SetText( ErrMsgUTF8.c_str());
		UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] Msg[%s] NID[%lld]", ExecID.c_str(), IsDup ? "Yes" : "No", ErrMsgUTF8.c_str(), ExecutionReport.GetNID());
	}
	else
		UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] ", ExecID.c_str(), IsDup ? "Yes" : "No" );
	TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	UFC::BufferedLog::Printf( " ------------------ Sent Canceled Execution ----------------------" );
}
//---------------------------------------------------------------------------
void TTaifexConnection::CTPRejectExecution( COrderInfo* Ord, CThostFtdcOrderField *pOrder )
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString         ExecID;
	GB2312String            ErrMsg( pOrder->StatusMsg );
	UFC::AnsiString         OrderLocalID( pOrder->OrderLocalID );
	UFC::AnsiString         OrderSysID( pOrder->OrderSysID );
	bool IsDup;

	UFC::BufferedLog::Printf( " ------------------------- CTP Reject ----------------------------" );
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
	UFC::BufferedLog::Printf( " NID[%s] ExchangeID[%s] Symbol[%s] OrderSysID[%s] OrderLocalID[%s]", pOrder->OrderRef, pOrder->ExchangeID, pOrder->InstrumentID, pOrder->OrderSysID, pOrder->OrderLocalID);
	if( ErrMsg.Length() > 0 )
	{
		UTF8String   ErrMsgUTF8( ErrMsg );
		ExecutionReport.SetText( ErrMsgUTF8.c_str());
		UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] Msg[%s] NID[%lld]", ExecID.c_str(), IsDup ? "Yes" : "No", ErrMsgUTF8.c_str(), ExecutionReport.GetNID());
	}
	else
		UFC::BufferedLog::Printf( " ExecID[%s] Dup[%s] ", ExecID.c_str(), IsDup ? "Yes" : "No" );
	TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsDup );
	UFC::BufferedLog::Printf( " ------------------- Sent Reject Execution -----------------------" );
}
//---------------------------------------------------------------------------
// CTP Order Confirmation.
//---------------------------------------------------------------------------
void TTaifexConnection::OnRtnOrder( CThostFtdcOrderField *pOrder)
{
	///< CTP Ack: (FrontID + SessionID ) OrderRef to find Order record.
	///<          Update OrderLocalID,ExchangeID
	///< Exchange Ack: ExchangeID + OrderLocalID  to find Order record.
	///<           Update OrderSys.
	UFC::AnsiString OrdRef( pOrder->OrderRef );
	UFC::AnsiString ExchangeID( pOrder->ExchangeID  );
	UFC::AnsiString OrderLocalID( pOrder->OrderLocalID  );
	UFC::AnsiString OrdSysID( pOrder->OrderSysID );
	UFC::AnsiString Status,SubmitStatus;
	COrderInfo*     Record;

	///< Print order info.
	OrderStatusString( pOrder->OrderStatus, Status );
	OrderSubmitStatusString( pOrder->OrderSubmitStatus, SubmitStatus );
	UFC::BufferedLog::Printf( " OnRtnOrder: FrontID[%d] SessionID[%d] OrderRef[%s].", pOrder->FrontID, pOrder->SessionID, pOrder->OrderRef );
	UFC::BufferedLog::Printf( " OnRtnOrder: Exchange[%s] OrderSysID[%s] OrderLocalID[%s].", ExchangeID.c_str(), OrdSysID.c_str(), OrderLocalID.c_str());
	UFC::BufferedLog::Printf( " OnRtnOrder: OrderStatus[%s] OrderSubmitStatus[%s]", Status.c_str(), SubmitStatus.c_str());

	UpdateRecover();
	if( OrderLocalID.AnsiPos( '_' ) != -1 || OrdSysID.AnsiPos( '_' ) != -1 )
	{
		UFC::BufferedLog::Printf( " OnRtnOrder: ----- It's a CTP server side Stop Order execution. Skip this execution! -----" );
		return;
	}
	///< Use FrontID + SessionID + OrderRef to find Order.
	if( (Record = FindOrderByOrderRef( pOrder->FrontID, pOrder->SessionID, OrdRef.ToInt())) != NULL )
		UFC::BufferedLog::Printf( " OnRtnOrder: Use FrontID + SessionID + OrderRef found the original order.");
	///< Use ExchangeID + OrderSysID to find Order.
	else if( ExchangeID.Length() > 0 && OrdSysID.Length() > 0 &&
			 (Record = FindOrderByOrderSysID( ExchangeID, OrdSysID.ToInt64())) != NULL )
		UFC::BufferedLog::Printf( " OnRtnOrder: Use ExchangeID + OrderSysID found the original order.");
	else ///< Order not found!
	{
		UFC::BufferedLog::Printf( " --------------------- Receive Execution from other CTP clients -------------------------" );
		UFC::BufferedLog::Printf( " OnRtnOrder: Order Exchange[%s] OrderSysID[%s] OrderLocalID[%s] not found!", ExchangeID.c_str(), OrdSysID.c_str(), OrderLocalID.c_str());
		UFC::BufferedLog::Printf( " OnRtnOrder: Add Order record FrontID[%d] SessionID[%d] OrderRef[%s]", pOrder->FrontID, pOrder->SessionID, pOrder->OrderRef );
		UFC::BufferedLog::Printf( " ----------------------------------------------------------------------------------------" );
		Record = AddPendingOrder( pOrder );
	}
	///< Update Record fields
	if ( Record->GetStatus() == cosPending && OrderLocalID.Length() > 0 && ExchangeID.Length() > 0 )
	{
		if ( FUseAPI == atHSctp && pOrder->OrderSubmitStatus == THOST_FTDC_OSS_Accepted )
		{
			///< From HSctp confirm status to Exchange confirm status.( Update ExchangeID, OrderSysID, OrderLocalID )
			Record->EachangeAccepted( ExchangeID, OrdSysID, OrderLocalID );
			AddOrderSysIDToMap( ExchangeID, OrdSysID.ToInt64() , Record );
			CTPConfirmExecution( Record, pOrder ); ///< Send confirm execution.
			TradeLogExConfirm( pOrder );
		}
		else
		{
			///< From Pending New status to CTP confirm status.( Update ExchangeID, OrderLocalID )
			Record->CTPAccepted( ExchangeID, OrderLocalID );
			//AddOrderLocalIDToMap( ExchangeID, OrderLocalID.ToInt(), Record );
			CTPPendingExecution( Record, pOrder ); ///< Send pending New execution.
			TradeLogCTPConfirm( pOrder );
		}
	}
	else if ( Record->GetStatus() == cosConfirm && OrdSysID.Length() > 0 && ExchangeID.Length() > 0 )
	{   ///< From CTP confirm status to Exchange confirm status.( OrderSysID )
		Record->EachangeAccepted( OrdSysID );
		AddOrderSysIDToMap( ExchangeID, OrdSysID.ToInt64() , Record );
		CTPConfirmExecution( Record, pOrder ); ///< Send confirm execution.
		TradeLogExConfirm( pOrder );
	}

	if( pOrder->OrderStatus == THOST_FTDC_OST_Canceled )
	{
		if( pOrder->OrderSubmitStatus == THOST_FTDC_OSS_Accepted )
		{
			Record->CloseOrder();
			CTPCanceledExecution( Record, pOrder );///< Send Canceled execution.
			TradeLogCancel( pOrder );
		}
		else if( pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected ||
				 pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted )
		{
			Record->CloseOrder();
			if ( Record->GetCumQty() > 0 && pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted )
				CTPCanceledExecution( Record, pOrder );///< Send Canceled execution.
			else
				CTPRejectExecution( Record, pOrder );///< Send Reject execution.
			TradeLogCancel( pOrder );
		}
	}
}
//---------------------------------------------------------------------------
void TTaifexConnection::CTPFillExecution( COrderInfo* Ord, CThostFtdcTradeField *pTrade )
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString OrdSysIDStr( pTrade->OrderSysID );
	UFC::AnsiString ExecID;
	bool IsDup;

	OrdSysIDStr.TrimLeft();
	UFC::BufferedLog::Printf( " -------------------------- CTP Fill -----------------------------" );
	if ( FUseAPI == atHSctp ) ///< SequenceNo always 0 in HSctp case, use TradeID replace
		ExecID.Printf( "F_%s_%lld_%s", pTrade->ExchangeID, OrdSysIDStr.ToInt64(), pTrade->TradeID );
	else
		ExecID.Printf( "F_%s_%lld_%d", pTrade->ExchangeID, OrdSysIDStr.ToInt64(), pTrade->SequenceNo );
	if( (IsDup = IsExecutionDup( nsOrderMessageDefine::mCNFutures, ExecID )) == false )
		Ord->Fill( pTrade->Volume );
	UFC::BufferedLog::Printf( " NID[%s] ExchangeID[%s] Symbol[%s] OrderSysID[%s] OrderLocalID[%s]", pTrade->OrderRef, pTrade->ExchangeID, pTrade->InstrumentID, pTrade->OrderSysID, pTrade->OrderLocalID );
	UFC::BufferedLog::Printf( " Account[%s] Time[%s] Side[%c] ", pTrade->UserID, pTrade->TradeTime, pTrade->Direction == THOST_FTDC_D_Buy? 'B':'S' );
	UFC::BufferedLog::Printf( " LastPx[%0.3f] LastQty[%d] CumQty[%d] LeavesQty[%d]", pTrade->Price, pTrade->Volume, Ord->GetCumQty(), Ord->GetLeavesQty() );
	UFC::BufferedLog::Printf( " ExecID[%s] TradeID[%s]  FillSeq[%d] Dup[%s] NID[%lld]", ExecID.c_str(), pTrade->TradeID, pTrade->SequenceNo, IsDup ? "Yes" : "No", Ord->GetOrderNID());
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
	ExecutionReport.SetPrice( pTrade->Price );
	ExecutionReport.SetLastPx( pTrade->Price );
	ExecutionReport.SetOrderQty( pTrade->Volume );
	ExecutionReport.SetLastQty( pTrade->Volume );
	ExecutionReport.SetTransactTime( pTrade->TradeTime );
	TrigerOnExecutionReport( &ExecutionReport,(ExecDup) IsDup );
	UFC::BufferedLog::Printf( " -------------------- Sent Fill Execution ------------------------" );
}
//---------------------------------------------------------------------------
// CTP Order Fill.
//---------------------------------------------------------------------------
void TTaifexConnection::OnRtnTrade( CThostFtdcTradeField *pTrade )
{
	///< ExchangeID + OrderSysID to find Order record.
	if( strlen( pTrade->ExchangeID ) > 0 && strlen( pTrade->OrderLocalID ) >0 && strlen( pTrade->OrderSysID ) >0 )
	{
		UpdateRecover();
		UFC::AnsiString ExchangeID( pTrade->ExchangeID  );
		Int64           OrdSysIDInt64     = _atoi64( pTrade->OrderSysID );
		int             OrderLocalIDInt = atoi( pTrade->OrderLocalID  );
		COrderInfo*     Ord;

		if( (Ord = FindOrderByOrderSysID( ExchangeID, OrdSysIDInt64 ) )!= NULL )
		{
			CTPFillExecution( Ord, pTrade );
			TradeLogFill( pTrade );
		}
		else
			UFC::BufferedLog::Printf( " OnRtnTrade: Exchange[%s] OrderSysID[%lld] OrderLocalID[%d] not found! update fill failed.", ExchangeID.c_str(), OrdSysIDInt64, OrderLocalIDInt );
	}
	else
		UFC::BufferedLog::Printf( " OnRtnTrade: missing field OrderSysID,OrderLocalID or ExchangeID." );
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
