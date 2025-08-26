//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "Unit1.h"

#include "KSUserApiStructEx.h"
#include "KSUserApiDataTypeEx.h"
#include "KSftAPIDynamic.h"
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
TTradingForm *TradingForm;
//---------------------------------------------------------------------------
/*
AnsiString gBrokerID;
AnsiString gUserID;
AnsiString gPassword;
AnsiString gServerAddress ( "tcp://" );
*/
//---------------------------------------------------------------------------
// 金仕達
//---------------------------------------------------------------------------
/*
AnsiString gBrokerID( "31000853" );
AnsiString gUserID( "201315" );
AnsiString gUserID( "201316" );
AnsiString gPassword( "123" );
//AnsiString gServerAddress( "tcp://210.5.154.195:13163" ); ///< V8T
AnsiString gServerAddress( "tcp://210.5.154.195:13153" ); ///< V6
*/
//---------------------------------------------------------------------------
// 光大證券
//---------------------------------------------------------------------------
AnsiString gBrokerID( "2E32CB35" );
AnsiString gUserID( "45090006" );
AnsiString gPassword( "888888" );
AnsiString gServerAddress( "tcp://10.0.29.188:17996" );
//---------------------------------------------------------------------------
// 大陸期貨
//---------------------------------------------------------------------------
/*
AnsiString gBrokerID( "31000853" );
AnsiString gUserID( "1766" );
AnsiString gPassword( "123456" );
//AnsiString gServerAddress( "tcp://61.151.253.198:14159" ); ///< Address1
AnsiString gServerAddress( "tcp://61.151.244.150:14159" ); ///< Address2
//AnsiString gServerAddress( "tcp://210.51.25.87:14159" ); ///< Address3
*/
//---------------------------------------------------------------------------
// 安信期貨
//---------------------------------------------------------------------------
/*
AnsiString gBrokerID( "6C2D786C" );
AnsiString gUserID( "8000500268" );
AnsiString gPassword( "654321" );
AnsiString gServerAddress( "tcp://219.143.242.159:13159" ); ///< Address1
*/
//---------------------------------------------------------------------------
int gCurRequestId = 0;
int gOrderRef = 1;
//---------------------------------------------------------------------------
void KSftListener::OnKSftFrontConnected( )
{
	TradingForm->Memo1->Lines->Add( "********************************************" );
	TradingForm->Memo1->Lines->Add( L"Connected!" );
	KS_::CThostFtdcReqUserLoginField reqUserLogin;
	memset( &reqUserLogin, 0, sizeof(reqUserLogin) );

	strcpy( reqUserLogin.BrokerID, gBrokerID.c_str() );
	strcpy( reqUserLogin.UserID,   gUserID.c_str() );
	strcpy( reqUserLogin.Password, gPassword.c_str() );
	strcpy( reqUserLogin.UserProductInfo, "S8 2020335" );

	TradingForm->FKSFTAPI->ReqUserLogin( &reqUserLogin, 1 );
}
//---------------------------------------------------------------------------
void KSftListener::OnKSftFrontDisconnected( int nReason )
{
	TradingForm->Memo1->Lines->Add( L"Disconnected! Reason:" + String(nReason) );
	// Release API
	TradingForm->ReleaseAPI();
}
//---------------------------------------------------------------------------
void KSftListener::OnKSftRspUserLogin( KS_::CThostFtdcRspUserLoginField *pRspUserLogin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	String Msg;

	if ( pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		String ErrMsgUnicode( ErrMsg );

		Msg.printf( L"Failed to login, Errorcode[%d] msg[%s] requestid[%d] chain[%d]"
					, pRspInfo->ErrorID, ErrMsgUnicode, nRequestID, bIsLast );
		TradingForm->Memo1->Lines->Add( Msg );
	}
	else
	{
		AnsiString MaxRef( pRspUserLogin->MaxOrderRef );
		AnsiString OutMsg;

		gOrderRef = MaxRef.ToInt();
		OutMsg.printf( "Login OK! MaxOrderRef[%d]", pRspUserLogin->MaxOrderRef );
		TradingForm->Memo1->Lines->Add( OutMsg );
		OutMsg.printf( "TradingDay[%s]", pRspUserLogin->TradingDay );
		TradingForm->Memo1->Lines->Add( OutMsg );
		OutMsg.printf( "FrontID[%d]", pRspUserLogin->FrontID );
		TradingForm->Memo1->Lines->Add( OutMsg );
		OutMsg.printf( "SessionID[%d]", pRspUserLogin->SessionID );
		TradingForm->Memo1->Lines->Add( OutMsg );
	}
	TradingForm->Caption = L"User[" + gUserID + "]";

	TradingForm->Memo1->Lines->Add( "********************************************" );
	// Set Buttons
	TradingForm->SetButtons( true );
}
//---------------------------------------------------------------------------
void KSftListener::OnKSftRspUserPasswordUpdate( KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		String ErrMsgUnicode( ErrMsg );
		TradingForm->Memo1->Lines->Add( L"Change Password failed:" + ErrMsgUnicode );
	}
	else
		TradingForm->Memo1->Lines->Add( L"Change Password OK!" );
}
//---------------------------------------------------------------------------
void KSftListener::OnKSftRspQryTradingAccount( KS_::CThostFtdcTradingAccountField *pTradingAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		String ErrMsgUnicode( ErrMsg );
		TradingForm->Memo1->Lines->Add( ErrMsgUnicode );
	}
	if ( pTradingAccount != NULL && bIsLast )
	{
		AnsiString Msg;
		double staticBalance  = pTradingAccount->PreBalance -pTradingAccount->Withdraw + pTradingAccount->Deposit;
		double dynamicBalance = staticBalance +
								pTradingAccount->CloseProfit + pTradingAccount->PositionProfit - pTradingAccount->Commission;
		TradingForm->Memo1->Lines->Add( L" 靜態權益 | 平倉盈虧| 持倉盈虧| 動態權益|  占用保證金| 下單動結| 可用資金| 風險度" );
		Msg.printf( "%10.2f|%10.2f|%10.2f|%10.2f|%10.2f|%10.2f|%10.2f|%10.2f",
					staticBalance,
					pTradingAccount->CloseProfit,
					pTradingAccount->PositionProfit ,
					dynamicBalance,
					pTradingAccount->CurrMargin,
					pTradingAccount->FrozenMargin + pTradingAccount->FrozenCommission,
					pTradingAccount->Available,
					pTradingAccount->CurrMargin/(dynamicBalance==0?1:dynamicBalance)*100 );
		TradingForm->Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void KSftListener::OnKSftRspQrySettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		TradingForm->Memo1->Lines->Add( " OnRspQrySettlementInfoConfirm failed code:" + String(pRspInfo->ErrorID) );
		return;
	}
	if ( pSettlementInfoConfirm != NULL )
	{
		String Msg( pSettlementInfoConfirm->ConfirmTime );
		TradingForm->Memo1->Lines->Add( L"User confirm settlement Info at:" + Msg );
	}
	else
	{
		KS_::CThostFtdcQrySettlementInfoField QuerySettlementData;

		TradingForm->Memo1->Lines->Add( L"User not confirm Settlement Info. " );
		memset( &QuerySettlementData, 0, sizeof(QuerySettlementData) );
		strcpy( QuerySettlementData.BrokerID,   gBrokerID.c_str() );
		strcpy( QuerySettlementData.InvestorID, gUserID.c_str() );
		gCurRequestId++;
		TradingForm->Memo1->Lines->Add( " ========== Begin ReqQrySettlementInfo ==========");
		TradingForm->FKSFTAPI->ReqQrySettlementInfo( &QuerySettlementData, gCurRequestId );
	}
}
//---------------------------------------------------------------------------
void KSftListener::OnKSftRspQrySettlementInfo( KS_::CThostFtdcSettlementInfoField *pSettlementInfo, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( ( pSettlementInfo != NULL ) && ( std::strlen(pSettlementInfo->Content) > 0 ) )
	{
		GB2312String Msg( pSettlementInfo->Content );
		TradingForm->Memo1->Lines->Add( Msg );
	}
	if ( bIsLast )
	{
		TradingForm->Memo1->Lines->Add( " ========== Filished OnRspQrySettlementInfo ==========");
		TDateTime Now = TDateTime::CurrentDateTime();
		unsigned short YY,MM,DD,hh,mm,ss,ms;
		AnsiString CurDateStr;
		AnsiString CurTimeStr;

		Now.DecodeDate( &YY,&MM,&DD );
		Now.DecodeTime( &hh,&mm,&ss,&ms );
		CurDateStr.printf( "%04d%02d%02d",YY,MM,DD );
		CurTimeStr.printf( "%02d:%02d:%02d", hh,mm,ss );
		KS_::CThostFtdcSettlementInfoConfirmField SettlementConfirmData;

		memset( &SettlementConfirmData, 0, sizeof(SettlementConfirmData) );
		strcpy( SettlementConfirmData.BrokerID,    gBrokerID.c_str() );
		strcpy( SettlementConfirmData.InvestorID,  gUserID.c_str() );
		strcpy( SettlementConfirmData.ConfirmDate, CurDateStr.c_str() );
		strcpy( SettlementConfirmData.ConfirmTime, CurTimeStr.c_str() );
		gCurRequestId++;
		TradingForm->FKSFTAPI->ReqSettlementInfoConfirm( &SettlementConfirmData, gCurRequestId );
	}
}
//---------------------------------------------------------------------------
void KSftListener::OnKSftRspSettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( bIsLast )
		TradingForm->Memo1->Lines->Add( " ========== Filished OnRspSettlementInfoConfirm ==========");
}
//---------------------------------------------------------------------------
void KSftListener::OnKSftRspQryInvestorPosition( KS_::CThostFtdcInvestorPositionField *pInvestorPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		String ErrMsgUnicode( ErrMsg );
		TradingForm->Memo1->Lines->Add(  ErrMsgUnicode );
	}
	if ( pInvestorPosition != NULL )
	{
		AnsiString Msg, BS, PosDay;

		if ( pInvestorPosition->PosiDirection == THOST_FTDC_PD_Long )
			BS = "多";
		else if ( pInvestorPosition->PosiDirection == THOST_FTDC_PD_Net )
			BS = "淨";
		else
			BS = "空";
		if ( pInvestorPosition->PositionDate == THOST_FTDC_PSD_History )
			PosDay = "昨倉";
		else
			PosDay = "今倉";

		////執行凍結
		//TThostFtdcVolumeType	StrikeFrozen;
		///執行凍結金額
		//TThostFtdcMoneyType	StrikeFrozenAmount;
		///放棄執行凍結
		//TThostFtdcVolumeType	AbandonFrozen;
		///期權市值
		//TThostFtdcMoneyType	OptionValue;

		Msg.printf( "%10s|%5s|%5s|%8d|%8d|%0.2f|%0.2f|%0.2f",
					pInvestorPosition->InstrumentID,
					BS.c_str(),
					PosDay,
					pInvestorPosition->Position,
					pInvestorPosition->YdPosition,
					pInvestorPosition->PositionCost,
					pInvestorPosition->UseMargin,
					pInvestorPosition->PositionProfit );
		TradingForm->FStrings->Add( Msg );
		if ( bIsLast )
		{
			TradingForm->Memo1->Lines->Add( L"     商品 | 買賣| 今昨|    今倉|    昨倉|  持倉成本|占用保證金|持倉盈虧" );
			TradingForm->Memo1->Lines->Add( "------------------------------------------------------" );
			for( int i =0;i< TradingForm->FStrings->Count; i++ )
				 TradingForm->Memo1->Lines->Add( TradingForm->FStrings->Strings[i] );
			TradingForm->Memo1->Lines->Add( "------------------------------------------------------" );
		}
	}
	else
		TradingForm->Memo1->Lines->Add( L" 無留倉部位." );
}
//---------------------------------------------------------------------------
void KSftListener::OnKSftRspQryInvestorPositionDetail( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( pRspInfo != NULL && pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		String ErrMsgUnicode( ErrMsg );
		TradingForm->Memo1->Lines->Add( ErrMsgUnicode );
	}
}
//---------------------------------------------------------------------------
void KSftListener::OnKSftRspOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( pRspInfo->ErrorID != 0 && pRspInfo != NULL )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		String ErrMsgUnicode( ErrMsg );
		String Msg;

		Msg.printf( L"Failed palce new order, Errorcode[%d] msg[%s] requestid[%d] chain[%d]",
					  pRspInfo->ErrorID, ErrMsgUnicode, nRequestID, bIsLast );
		TradingForm->Memo1->Lines->Add( Msg );
	}
	else
		TradingForm->Memo1->Lines->Add( "Ack for New Order OK!" );
}
//---------------------------------------------------------------------------
void KSftListener::OnKSftRtnOrder( KS_::CThostFtdcOrderField *pOrder )
{
	String Msg, Status, SubmitStatus;
	String OrdSysID( pOrder->OrderSysID );
	String OrdRef( pOrder->OrderRef );
	String BrokerID( pOrder->BrokerID );
	String InvestorID( pOrder->InvestorID );
	String InstrumentID( pOrder->InstrumentID );
	String Side( (pOrder->Direction==THOST_FTDC_D_Sell)?"Sell":"Buy" );
	GB2312String StatusMsg( pOrder->StatusMsg );
	String ExchangeID( pOrder->ExchangeID );
	String TraderID( pOrder->TraderID );
	String OrderLocalID( pOrder->OrderLocalID );

	switch ( pOrder->OrderStatus )
	{
		case '0': Status = "THOST_FTDC_OST_AllTraded"; break; ///< done
		case '1': Status = "THOST_FTDC_OST_PartTradedQueueing"; break;
		case '2': Status = "THOST_FTDC_OST_PartTradedNotQueueing"; break; ///< done
		case '3': Status = "THOST_FTDC_OST_NoTradeQueueing"; break;
		case '4': Status = "THOST_FTDC_OST_NoTradeNotQueueing"; break;///< done
		case '5': Status = "THOST_FTDC_OST_Canceled"; break; ///< done
		case 'a': Status = "THOST_FTDC_OST_Unknown"; break;
		case 'b': Status = "THOST_FTDC_OST_NotTouched"; break;
		case 'c': Status = "THOST_FTDC_OST_Touched"; break;
	}
	switch ( pOrder->OrderSubmitStatus )
	{
		case THOST_FTDC_OSS_InsertSubmitted: SubmitStatus ="THOST_FTDC_OSS_InsertSubmitted"; break;
		case THOST_FTDC_OSS_CancelSubmitted: SubmitStatus ="THOST_FTDC_OSS_CancelSubmitted"; break;
		case THOST_FTDC_OSS_ModifySubmitted: SubmitStatus ="THOST_FTDC_OSS_ModifySubmitted"; break;
		case THOST_FTDC_OSS_Accepted       : SubmitStatus ="THOST_FTDC_OSS_Accepted"; break;
		case THOST_FTDC_OSS_InsertRejected : SubmitStatus ="THOST_FTDC_OSS_InsertRejected"; break;
		case THOST_FTDC_OSS_CancelRejected : SubmitStatus ="THOST_FTDC_OSS_CancelRejected"; break;
		case THOST_FTDC_OSS_ModifyRejected : SubmitStatus ="THOST_FTDC_OSS_ModifyRejected"; break;
	}
	Msg.printf( L" -------------------- Status[%s] -------------------------", Status );
	TradingForm->Memo1->Lines->Add( Msg );
	Msg.printf( L"SubmitStatus[%s]", SubmitStatus );
	TradingForm->Memo1->Lines->Add( Msg );
	Msg.printf( L"OrderRef[%s] OrderSysID[%s]", OrdRef,OrdSysID );
	TradingForm->Memo1->Lines->Add( Msg );
	Msg.printf( L"BrokerID[%s] ExchangeID[%s] TraderID[%s] OrderLocalID[%s]", BrokerID,ExchangeID, TraderID, OrderLocalID );
	TradingForm->Memo1->Lines->Add( Msg );
	Msg.printf( L"SessionID[%d] FrontID[%d] TraderID[%s]", pOrder->SessionID , pOrder->FrontID, TraderID );
	TradingForm->Memo1->Lines->Add( Msg );
	Msg.printf( L"InvestorID[%s] InstrumentID[%s] Side[%s]", InvestorID , InstrumentID, Side );
	TradingForm->Memo1->Lines->Add( Msg );
	Msg.printf( L"StatusMsg[%s]", String(StatusMsg) );
	TradingForm->Memo1->Lines->Add( Msg );
}
//---------------------------------------------------------------------------
void KSftListener::OnKSftRtnTrade( KS_::CThostFtdcTradeField *pTrade )
{
	String Msg, Status;
	String OrdSysID( pTrade->OrderSysID );
	String OrdRef( pTrade->OrderRef );
	String BrokerID( pTrade->BrokerID );
	String ExchangeID( pTrade->ExchangeID );
	String OrderLocalID( pTrade->OrderLocalID );

	Msg.printf( L" -------------------- Trade  -------------------------"  );
	TradingForm->Memo1->Lines->Add( Msg );
	Msg.printf( L"BrokerID[%s] ExchangeID[%s] OrderLocalID[%s] OrderSysID[%s]", BrokerID,ExchangeID, OrderLocalID, OrdSysID );
	TradingForm->Memo1->Lines->Add( Msg );
	Msg.printf( L"Price[%f] Qty[%d]", pTrade->Price, pTrade->Volume );
	TradingForm->Memo1->Lines->Add( Msg );
}
//---------------------------------------------------------------------------
void KSftListener::OnKSftRspError( KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( pRspInfo->ErrorID != 0 && pRspInfo != NULL )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		String ErrMsgUnicode( ErrMsg );
		TradingForm->Memo1->Lines->Add( ErrMsgUnicode );
	}
}
//---------------------------------------------------------------------------
//
// TTradingForm implementation
//
//---------------------------------------------------------------------------
__fastcall TTradingForm::TTradingForm(TComponent* Owner)
	: TForm(Owner)
{
	FStrings = new TStringList();
	Memo1->Lines->Add( TKSftTraderApi::LoadKSftAPIResult() );

	// Set Screen
	GetGlobalVariant( );
	SetButtons( false );

	/*
	FListener = new KSftListener();
	FKSFTAPI = new TKSftTraderApi( dynamic_cast<TKSftTraderSpi*>(FListener) );
	FKSFTAPI->SubscribePrivateTopic( KS_::THOST_TERT_RESTART );
	FKSFTAPI->SubscribePublicTopic( KS_::THOST_TERT_RESUME );
	FKSFTAPI->RegisterFront( gServerAddress.c_str() );
	FKSFTAPI->Init( );
	String Msg;
	Msg.printf( L"Connect...[%s]", String(gServerAddress.c_str()).c_str() );
	Memo1->Lines->Add( Msg );
	*/
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::Button1Click(TObject *Sender)
{
	KS_::CThostFtdcInputOrderField ord;

	memset( &ord, 0, sizeof(ord) );
	// From User basic info.
	strcpy( ord.BrokerID,   gBrokerID.c_str() ); // 經紀公司代碼
	strcpy( ord.InvestorID, gUserID.c_str() ); // 投資者代碼
	strcpy( ord.UserID,     gUserID.c_str() ); // 用戶代碼
	// From UI
	strcpy( ord.InstrumentID, "10000118" ); // 合約代碼
	ord.Direction = THOST_FTDC_D_Buy; // 買賣方向
	ord.OrderPriceType = THOST_FTDC_OPT_LimitPrice; // 報單價格條件
	ord.LimitPrice = 0.05; // 價格
	ord.VolumeTotalOriginal = 1; // 數量
	ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open; // 組合開平標誌
	strcpy( ord.CombHedgeFlag, "0" ); // 備兌標誌類型 : 非備兌
	//strcpy( ord.CombHedgeFlag, "1" ); // 組合投機套保標誌
	ord.TimeCondition = THOST_FTDC_TC_GFD;	// 有效期類型
	// Fixed value field
	strcpy( ord.GTDDate, "" ); // GTD日期
	ord.VolumeCondition = THOST_FTDC_VC_AV; // 成交量類型
	ord.MinVolume = 1; // 最小成交量
	ord.ContingentCondition = THOST_FTDC_CC_Immediately; // 觸發條件
	ord.StopPrice = 0; // 止損價
	ord.ForceCloseReason = THOST_FTDC_FCC_NotForceClose; // 強平原因
	ord.IsAutoSuspend = 0; // 自動掛起標誌
	// Sequence.
	sprintf( ord.OrderRef,"%012ld", gOrderRef ); // 報單引用
	gOrderRef ++;
	gCurRequestId++;
	int Rtn = FKSFTAPI->ReqOrderInsert(  &ord, gCurRequestId );
	if ( Rtn == 0 )
		Memo1->Lines->Add( L" NewOrder ReqID:" + String( gCurRequestId ) );
	else
		Memo1->Lines->Add( L" NewOrder ErrCode:" + String( Rtn ) );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::Button2Click(TObject *Sender)
{
	KS_::CThostFtdcQrySettlementInfoConfirmField QrySettlementInfoConfirm;

	memset( &QrySettlementInfoConfirm, 0, sizeof(QrySettlementInfoConfirm) );
	strcpy( QrySettlementInfoConfirm.BrokerID,   gBrokerID.c_str() );
	strcpy( QrySettlementInfoConfirm.InvestorID, gUserID.c_str() );

	gCurRequestId++;
	Memo1->Lines->Add( " ========== Begin ReqQrySettlementInfoConfirm ==========");
	int rt = FKSFTAPI->ReqQrySettlementInfoConfirm( &QrySettlementInfoConfirm,gCurRequestId );
	Memo1->Lines->Add( " Return:" + String(rt) );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::Button3Click(TObject *Sender)
{
	int Rtn;
/*
	KS_::CThostFtdcQryInvestorPositionDetailField QryInvestorPositionDetail;

	memset( &QryInvestorPositionDetail, 0, sizeof(QryInvestorPositionDetail) );
	strcpy( QryInvestorPositionDetail.BrokerID,   gBrokerID.c_str() );
	strcpy( QryInvestorPositionDetail.InvestorID, gUserID.c_str() );
//	strcpy(QryInvestorPositionDetail.InstrumentID, "IF1006");
	gCurRequestId++;
	Rtn = FKSFTAPI->ReqQryInvestorPositionDetail( &QryInvestorPositionDetail, gCurRequestId );
*/

	KS_::CThostFtdcQryInvestorPositionField QryInvestorPosition;

	memset( &QryInvestorPosition, 0 , sizeof(QryInvestorPosition) );
	strcpy(QryInvestorPosition.BrokerID,   gBrokerID.c_str());
	strcpy(QryInvestorPosition.InvestorID, gUserID.c_str());
//	strcpy(QryInvestorPosition.InstrumentID, "IF1306");
	gCurRequestId++;
	FStrings->Clear();
	Rtn = FKSFTAPI->ReqQryInvestorPosition( &QryInvestorPosition,  gCurRequestId );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::Button4Click(TObject *Sender)
{
	KS_::CThostFtdcQryTradingAccountField QryTradingAccount;

	memset( &QryTradingAccount, 0, sizeof(QryTradingAccount) );
	strcpy( QryTradingAccount.BrokerID,   gBrokerID.c_str() );
	strcpy( QryTradingAccount.InvestorID, gUserID.c_str() );
	////幣種代碼
	//TThostFtdcCurrencyIDType	CurrencyID;

	gCurRequestId++;
	FKSFTAPI->ReqQryTradingAccount( &QryTradingAccount, gCurRequestId );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::Button5Click(TObject *Sender)
{
	KS_::CThostFtdcUserPasswordUpdateField UserPasswordUpdate;

	memset( &UserPasswordUpdate, 0, sizeof(UserPasswordUpdate) );
	strcpy( UserPasswordUpdate.BrokerID,    gBrokerID.c_str() );
	strcpy( UserPasswordUpdate.UserID,      gUserID.c_str() );
	strcpy( UserPasswordUpdate.OldPassword, gPassword.c_str() );
	strcpy( UserPasswordUpdate.NewPassword, "888888" );
	gCurRequestId++;
	FKSFTAPI->ReqUserPasswordUpdate( &UserPasswordUpdate, gCurRequestId );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnConn_LoginClick(TObject *Sender)
{
	SetGlobalVariant( );

	if ( FListener == NULL )
		FListener = new KSftListener();
	if ( FKSFTAPI == NULL )
		FKSFTAPI = new TKSftTraderApi( dynamic_cast<TKSftTraderSpi*>(FListener) );
	FKSFTAPI->SubscribePrivateTopic( KS_::THOST_TERT_RESTART );
	FKSFTAPI->SubscribePublicTopic( KS_::THOST_TERT_RESUME );
	FKSFTAPI->RegisterFront( gServerAddress.c_str() );
	FKSFTAPI->Init( );
	String Msg;
	Msg.printf( L"Connect...[%s]", String( gServerAddress.c_str()).c_str() );
	Memo1->Lines->Add( Msg );
}
//---------------------------------------------------------------------------
void TTradingForm::GetGlobalVariant( void )
{
	TxtBrokerID->Text      = gBrokerID.c_str();
	TxtUserID->Text        = gUserID.c_str();
	TxtPassword->Text      = gPassword.c_str();
	TxtServerAddress->Text = gServerAddress.c_str();
}
//---------------------------------------------------------------------------
void TTradingForm::SetGlobalVariant( void )
{
	gBrokerID      = TxtBrokerID->Text.Trim();
	gUserID        = TxtUserID->Text.Trim();
	gPassword      = TxtPassword->Text.Trim();
	gServerAddress = TxtServerAddress->Text.Trim();
}
//---------------------------------------------------------------------------
void TTradingForm::SetButtons( bool IsConnected )
{
	if ( IsConnected )
	{
		Button1->Enabled       = true;
		Button2->Enabled       = true;
		Button3->Enabled       = true;
		Button4->Enabled       = true;
		Button5->Enabled       = false;
		BtnDisconnect->Enabled = true;
		BtnConn_Login->Enabled = false;
	}
	else
	{
		Button1->Enabled       = false;
		Button2->Enabled       = false;
		Button3->Enabled       = false;
		Button4->Enabled       = false;
		Button5->Enabled       = false;
		BtnDisconnect->Enabled = false;
		BtnConn_Login->Enabled = true;
	}
}
//---------------------------------------------------------------------------
void TTradingForm::ReleaseAPI( void )
{
	// Disable Buttons
	SetButtons( false );

	// Release API
	if ( FKSFTAPI != NULL )
	{
		delete FKSFTAPI;
		FKSFTAPI = NULL;
	}
	Sleep( 1000 );
	if ( FListener != NULL )
	{
		delete FListener;
		FListener = NULL;
	}
	Memo1->Lines->Add( "********************************************" );
	Memo1->Lines->Add( "Client Disconnect and Release API!!" );
	Memo1->Lines->Add( "********************************************" );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnDisconnectClick(TObject *Sender)
{
	ReleaseAPI();
}
//---------------------------------------------------------------------------

