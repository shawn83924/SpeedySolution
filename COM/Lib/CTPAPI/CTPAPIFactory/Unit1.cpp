//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "Unit1.h"

#include "ThostFtdcUserApiStruct.h"
#include "ThostFtdcUserApiDataType.h"
#include "CTPAPIDynamic.h"
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
TTradingForm *TradingForm;
//---------------------------------------------------------------------------
AnsiString gBrokerID;
AnsiString gUserID;
AnsiString gPassword;
AnsiString gServerAddress( "tcp://" );
//---------------------------------------------------------------------------
/*
AnsiString gBrokerID( "1111" );
AnsiString gUserID( "565" );
AnsiString gPassword( "565" );
AnsiString gServerAddress( "tcp://115.238.108.173:41207" );
*/
/*
AnsiString gBrokerID( "6000" );
AnsiString gUserID( "00205287" );
AnsiString gPassword( "123456" );
AnsiString gServerAddress( "tcp://222.68.181.138:41205" );
*/
//---------------------------------------------------------------------------
// 永安期貨 - 恒生仿 CTP
//---------------------------------------------------------------------------
/*
AnsiString gBrokerID( "1111" );
AnsiString gUserID( "101300117" );
AnsiString gPassword( "147963" );
AnsiString gServerAddress( "tcp://115.238.53.140:8882" );
*/
//---------------------------------------------------------------------------
int gCurRequestId = 0;
int gOrderRef = 1;
//---------------------------------------------------------------------------
__fastcall TTradingForm::TTradingForm(TComponent* Owner)
	: TForm(Owner)
	, FListener( NULL )
	, FCTPAPI( NULL )
{
	FStrings = new TStringList();
	// Set Screen
	GetGlobalVariant( );
	SetButtons( false, false );

	/*
	FListener = new CTPListener();
	FCTPAPI = new TThostFtdcTraderApi( dynamic_cast<TThostFtdcTraderSpi*>(FListener));
	FCTPAPI->SubscribePrivateTopic( THOST_TERT_RESUME );
	FCTPAPI->SubscribePublicTopic( THOST_TERT_RESUME );
	FCTPAPI->RegisterFront( gServerAddress.c_str() );
	FCTPAPI->Init( );
	*/
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::Button1Click(TObject *Sender)
{
	CThostFtdcInputOrderField ord;

	memset(&ord, 0, sizeof(ord));
	///< From User basic info.
	strcpy(ord.BrokerID, gBrokerID.c_str() );//经纪公司代码
	strcpy(ord.InvestorID, gUserID.c_str() );//投资者代码
	strcpy(ord.UserID, gUserID.c_str() ); // 用户代码
	///< From UI
	strcpy(ord.InstrumentID, "IO1403-P-2150"); // 合约代码
//	strcpy(ord.InstrumentID, "IF1404"); // 合约代码
	ord.Direction = THOST_FTDC_D_Buy; // 买卖方向
	ord.OrderPriceType = THOST_FTDC_OPT_LimitPrice; // 报单价格条件
	ord.LimitPrice = 2578.2; // 价格
	ord.VolumeTotalOriginal = 10; // 数量
	ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open; // 组合开平标志
	strcpy(ord.CombHedgeFlag, "1"); // 组合投机套保标志
	ord.TimeCondition = THOST_FTDC_TC_GFD;	// 有效期类型
	///< Fixed value field
	strcpy(ord.GTDDate, ""); // GTD日期
	ord.VolumeCondition = THOST_FTDC_VC_AV; // 成交量类型
	ord.MinVolume = 1; // 最小成交量
	ord.ContingentCondition = THOST_FTDC_CC_Immediately; // 触发条件
	ord.StopPrice = 0; // 止损价
	ord.ForceCloseReason = THOST_FTDC_FCC_NotForceClose; // 强平原因
	ord.IsAutoSuspend = 0; // 自动挂起标志
	///< Sequence.
	sprintf( ord.OrderRef,"%012ld", gOrderRef ); ///报单引用
	gOrderRef ++;
	gCurRequestId++;
	int Rtn = FCTPAPI->ReqOrderInsert(  &ord, gCurRequestId );
	if( Rtn == 0 )
		Memo1->Lines->Add( L" NewOrder ReqID:" + String( gCurRequestId ) );
	else
		Memo1->Lines->Add( L" NewOrder ErrCode:" + String( Rtn ) );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::Button2Click(TObject *Sender)
{

	CThostFtdcQrySettlementInfoConfirmField QrySettlementInfoConfirm;

	memset( &QrySettlementInfoConfirm, 0, sizeof(QrySettlementInfoConfirm));
	strcpy(QrySettlementInfoConfirm.BrokerID, gBrokerID.c_str() );
	strcpy(QrySettlementInfoConfirm.InvestorID, gUserID.c_str() );

	gCurRequestId++;
	Memo1->Lines->Add( " ========== Begin ReqQrySettlementInfoConfirm ==========");
	int rt = FCTPAPI->ReqQrySettlementInfoConfirm( &QrySettlementInfoConfirm,gCurRequestId);
	Memo1->Lines->Add( " Return:" + String(rt));
}
//---------------------------------------------------------------------------
void CTPListener::OnFrontConnected()
{
	TradingForm->Memo1->Lines->Add( "********************************************" );
	TradingForm->Memo1->Lines->Add( L"Connected!" );
	CThostFtdcReqUserLoginField reqUserLogin;
	memset( &reqUserLogin, 0, sizeof(reqUserLogin) );

	strcpy(reqUserLogin. BrokerID, gBrokerID.c_str() );
	strcpy(reqUserLogin.UserID, gUserID.c_str() );
	strcpy( reqUserLogin.Password, gPassword.c_str());

	TradingForm->FCTPAPI->ReqUserLogin( &reqUserLogin, 0 );
}
//---------------------------------------------------------------------------
void CTPListener::OnFrontDisconnected(int nReason)
{
	TradingForm->Memo1->Lines->Add( L"Disconnected! Reason:" + String( nReason ));
	// Release API
	TradingForm->ReleaseAPI();
}
//---------------------------------------------------------------------------
void CTPListener::OnRspUserLogin( CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	String Msg;

	if (pRspInfo->ErrorID != 0)
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		String ErrMsgUnicode( ErrMsg );

		Msg.printf( L"Failed to login, Errorcode[%d] msg[%s] requestid[%d] chain[%d]",
					  pRspInfo->ErrorID, ErrMsgUnicode, nRequestID, bIsLast);
		TradingForm->Memo1->Lines->Add( Msg );
	}
	else
	{
		AnsiString MaxRef( pRspUserLogin->MaxOrderRef );
		AnsiString OutMsg;

		gOrderRef = MaxRef.ToInt();
		OutMsg.printf( "Login OK! MaxOrderRef[%d]", pRspUserLogin->MaxOrderRef );
		TradingForm->Memo1->Lines->Add(  OutMsg );
		OutMsg.printf( "TradingDay[%s]", pRspUserLogin->TradingDay );
		TradingForm->Memo1->Lines->Add(  OutMsg );
		OutMsg.printf( "FrontID[%d]", pRspUserLogin->FrontID );
		TradingForm->Memo1->Lines->Add(  OutMsg );
		OutMsg.printf( "SessionID[%d]", pRspUserLogin->SessionID );
		TradingForm->Memo1->Lines->Add(  OutMsg );
	}
	TradingForm->Caption = L"User[" + gUserID + L"] ";

	TradingForm->Memo1->Lines->Add( "********************************************" );
	// Set Buttons
	TradingForm->SetButtons( true, true );
}
//---------------------------------------------------------------------------
void CTPListener::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(  pRspInfo != NULL && pRspInfo->ErrorID != 0  )
	{
		 TradingForm->Memo1->Lines->Add( " OnRspQrySettlementInfoConfirm failed code:" +  String(pRspInfo->ErrorID) );
		 return;
	}
	if( pSettlementInfoConfirm != NULL  )
	{
		String Msg( pSettlementInfoConfirm->ConfirmTime );

		TradingForm->Memo1->Lines->Add( L" User confirm settlement Info at:" + Msg );
		TradingForm->Memo1->Lines->Add( " ========== End OnRspQrySettlementInfoConfirm ==========");
	}
	else
	{
		CThostFtdcQrySettlementInfoField QuerySettlementData;

		TradingForm->Memo1->Lines->Add( L"User not confirm Settlement Info. " );
		memset( &QuerySettlementData, 0, sizeof(QuerySettlementData));
		strcpy(QuerySettlementData.BrokerID, gBrokerID.c_str() );
		strcpy(QuerySettlementData.InvestorID, gUserID.c_str() );
		gCurRequestId++;
		TradingForm->Memo1->Lines->Add( " ========== Begin ReqQrySettlementInfo ==========");
		TradingForm->FCTPAPI->ReqQrySettlementInfo( &QuerySettlementData, gCurRequestId);
	}
}
//---------------------------------------------------------------------------
void CTPListener::OnRspQrySettlementInfo( CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if ((pSettlementInfo != NULL ) && (std::strlen(pSettlementInfo->Content) > 0))
	{
		GB2312String Msg( pSettlementInfo->Content );

		TradingForm->Memo1->Lines->Add( Msg );
	}
	if( bIsLast )
	{
		TradingForm->Memo1->Lines->Add( " ========== Filished OnRspQrySettlementInfo ==========");
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
		strcpy(SettlementConfirmData.BrokerID, gBrokerID.c_str());
		strcpy(SettlementConfirmData.InvestorID, gUserID.c_str());
		strcpy(SettlementConfirmData.ConfirmDate, CurDateStr.c_str());
		strcpy(SettlementConfirmData.ConfirmTime, CurTimeStr.c_str());
		gCurRequestId++;
		TradingForm->FCTPAPI->ReqSettlementInfoConfirm( &SettlementConfirmData, gCurRequestId);
	}
}
//---------------------------------------------------------------------------
void CTPListener::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( bIsLast )
		TradingForm->Memo1->Lines->Add( " ========== Filished OnRspSettlementInfoConfirm ==========");
}
//---------------------------------------------------------------------------
// CTP Reject NewOrder.
//---------------------------------------------------------------------------
void CTPListener::OnRspOrderInsert( CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( pRspInfo->ErrorID != 0 && pRspInfo != NULL )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		String ErrMsgUnicode( ErrMsg );
		String Msg;

		Msg.printf( L"Failed palce new order, Errorcode[%d] msg[%s] requestid[%d] chain[%d]",
					  pRspInfo->ErrorID, ErrMsgUnicode, nRequestID, bIsLast);
		TradingForm->Memo1->Lines->Add(  Msg );
	}
	else
		TradingForm->Memo1->Lines->Add(  "Ack for New Order OK!" );
}
//---------------------------------------------------------------------------
// CTP Order Confirmation.
//---------------------------------------------------------------------------
void CTPListener::OnRtnOrder( CThostFtdcOrderField *pOrder)
{
	String Msg,Status;
	String OrdSysID( pOrder->OrderSysID );
	String OrdRef( pOrder->OrderRef );
	String BrokerID( pOrder->BrokerID );
	String ExchangeID( pOrder->ExchangeID );
	String TraderID( pOrder->TraderID );
	String OrderLocalID( pOrder->OrderLocalID );

	switch( pOrder->OrderStatus )
	{
		case '0': Status = "THOST_FTDC_OST_AllTraded";break; ///< done
		case '1': Status = "THOST_FTDC_OST_PartTradedQueueing";break;
		case '2': Status = "THOST_FTDC_OST_PartTradedNotQueueing";break; ///< done
		case '3': Status = "THOST_FTDC_OST_NoTradeQueueing";break;
		case '4': Status = "THOST_FTDC_OST_NoTradeNotQueueing";break;///< done
		case '5': Status = "THOST_FTDC_OST_Canceled";break; ///< done
		case 'a': Status = "THOST_FTDC_OST_Unknown";break;
		case 'b': Status = "THOST_FTDC_OST_NotTouched";break;
		case 'c': Status = "THOST_FTDC_OST_Touched";break;
	}
	Msg.printf( L" -------------------- Status[%s] -------------------------",  Status  );
	TradingForm->Memo1->Lines->Add(  Msg );
	Msg.printf( L"OrderRef[%s] OrderSysID[%s]", OrdRef,OrdSysID );
	TradingForm->Memo1->Lines->Add(  Msg );
	Msg.printf( L"BrokerID[%s] ExchangeID[%s] TraderID[%s] OrderLocalID[%s]", BrokerID,ExchangeID, TraderID,  OrderLocalID );
	TradingForm->Memo1->Lines->Add(  Msg );
	Msg.printf( L"SessionID[%d] FrontID[%d] TraderID[%s]", pOrder->SessionID ,pOrder->FrontID, TraderID );
	TradingForm->Memo1->Lines->Add(  Msg );
	if( pOrder->OrderStatus == '5' &&
		(pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected ||
		 pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted) )
	{
		GB2312String ErrMsg( pOrder->StatusMsg );

		Msg.printf( L"Reject[%s]", String( ErrMsg ) );
		TradingForm->Memo1->Lines->Add(  Msg );
    }
}
//---------------------------------------------------------------------------
void CTPListener::OnRtnTrade(CThostFtdcTradeField *pTrade )
{
	String Msg,Status;
	String OrdSysID( pTrade->OrderSysID );
	String OrdRef( pTrade->OrderRef );
	String BrokerID( pTrade->BrokerID );
	String ExchangeID( pTrade->ExchangeID );
	String OrderLocalID( pTrade->OrderLocalID );

	Msg.printf( L" -------------------- Trade  -------------------------"  );
	TradingForm->Memo1->Lines->Add(  Msg );
	Msg.printf( L"BrokerID[%s] ExchangeID[%s] OrderLocalID[%s] OrderSysID[%s]", BrokerID,ExchangeID, OrderLocalID, OrdSysID );
	TradingForm->Memo1->Lines->Add(  Msg );
	Msg.printf( L"Price[%f] Qty[%d]", pTrade->Price, pTrade->Volume );
	TradingForm->Memo1->Lines->Add(  Msg );
}
//---------------------------------------------------------------------------
void CTPListener::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( pRspInfo->ErrorID != 0 && pRspInfo != NULL )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		String ErrMsgUnicode( ErrMsg );
	}
}
//---------------------------------------------------------------------------
void CTPListener::OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( pRspInfo != NULL && pRspInfo->ErrorID != 0  )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		String ErrMsgUnicode( ErrMsg );
	}
}
//---------------------------------------------------------------------------
void CTPListener::OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( pRspInfo != NULL && pRspInfo->ErrorID != 0  )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		String ErrMsgUnicode( ErrMsg );
		TradingForm->Memo1->Lines->Add( L"Change Password failed:" + ErrMsgUnicode );
	}
	else
		TradingForm->Memo1->Lines->Add( L"Change Password OK!" );
}
//---------------------------------------------------------------------------
void CTPListener::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( pRspInfo != NULL && pRspInfo->ErrorID != 0  )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		String ErrMsgUnicode( ErrMsg );
		TradingForm->Memo1->Lines->Add(  ErrMsgUnicode );
	}
	if( pInvestorPosition != NULL )
	{
		AnsiString Msg, BS, PosDay;

		if( pInvestorPosition->PosiDirection == THOST_FTDC_PD_Long )
			BS = "多";
		else if( pInvestorPosition->PosiDirection == THOST_FTDC_PD_Net )
			BS = "淨";
		else
			BS = "空";
		if( pInvestorPosition->PositionDate == THOST_FTDC_PSD_History )
			PosDay = "昨倉";
		else
			PosDay = "今倉";


		Msg.printf("%10s|%5s|%5s|%8d|%8d|%0.2f",
					pInvestorPosition->InstrumentID,
					BS.c_str(),
					PosDay,
					pInvestorPosition->Position,
					pInvestorPosition->YdPosition,
					pInvestorPosition->PositionProfit );
		TradingForm->FStrings->Add( Msg );
		if( bIsLast )
		{
			TradingForm->Memo1->Lines->Add( "     商品 | 買賣| 今昨|    今倉|    昨倉|持倉盈虧" );
			TradingForm->Memo1->Lines->Add( "------------------------------------------------------" );
			for( int i =0;i< TradingForm->FStrings->Count; i++ )
				 TradingForm->Memo1->Lines->Add(  TradingForm->FStrings->Strings[i] );
			TradingForm->Memo1->Lines->Add( "------------------------------------------------------" );
		}
	}
	else
		TradingForm->Memo1->Lines->Add( " 無留倉部位." );
}
//---------------------------------------------------------------------------
void CTPListener::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if( pRspInfo != NULL && pRspInfo->ErrorID != 0  )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		String ErrMsgUnicode( ErrMsg );
		TradingForm->Memo1->Lines->Add(  ErrMsgUnicode );
	}
	if( pTradingAccount != NULL && bIsLast )
	{
		AnsiString Msg;
		double staticBalance = 	pTradingAccount->PreBalance -pTradingAccount->Withdraw + pTradingAccount->Deposit;
		double dynamicBalance = staticBalance +
								pTradingAccount->CloseProfit + pTradingAccount->PositionProfit - pTradingAccount->Commission;
		TradingForm->Memo1->Lines->Add( " 靜態權益 | 平倉盈虧| 持倉盈虧| 動態權益|  占用保證金| 下單動結| 可用資金| 風險度" );
		Msg.printf("%10.2f|%10.2f|%10.2f|%10.2f|%10.2f|%10.2f|%10.2f|%10.2f",
					staticBalance,
					pTradingAccount->CloseProfit,
					pTradingAccount->PositionProfit ,
					dynamicBalance,
					pTradingAccount->CurrMargin,
					pTradingAccount->FrozenMargin + pTradingAccount->FrozenCommission,
					pTradingAccount->Available,
					pTradingAccount->CurrMargin/dynamicBalance*100 );
		TradingForm->Memo1->Lines->Add( Msg );
		TradingForm->Memo1->Lines->Add( " ========== End OnRspQryTradingAccount ==========");
	}
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::Button3Click(TObject *Sender)
{
	CThostFtdcQrySettlementInfoField QuerySettlementData;

	memset( &QuerySettlementData, 0, sizeof(QuerySettlementData));
	strcpy(QuerySettlementData.BrokerID, gBrokerID.c_str() );
	strcpy(QuerySettlementData.InvestorID, gUserID.c_str() );
	gCurRequestId++;
	TradingForm->Memo1->Lines->Add( " ========== Begin ReqQrySettlementInfo ==========");
	TradingForm->FCTPAPI->ReqQrySettlementInfo( &QuerySettlementData, gCurRequestId);
}
//---------------------------------------------------------------------------

void __fastcall TTradingForm::Button4Click(TObject *Sender)
{
	CThostFtdcQryTradingAccountField QryTradingAccount;

	memset(&QryTradingAccount, 0, sizeof(QryTradingAccount));
	strcpy(QryTradingAccount.BrokerID,  gBrokerID.c_str());
	strcpy(QryTradingAccount.InvestorID, gUserID.c_str());
	gCurRequestId++;
	Memo1->Lines->Add( " ========== Begin ReqQryTradingAccount ==========");
	FCTPAPI->ReqQryTradingAccount( &QryTradingAccount,  gCurRequestId );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::Button5Click(TObject *Sender)
{
	CThostFtdcUserPasswordUpdateField UserPasswordUpdate;

	memset(&UserPasswordUpdate, 0, sizeof(UserPasswordUpdate));
	strcpy(UserPasswordUpdate.BrokerID,    gBrokerID.c_str());
	strcpy(UserPasswordUpdate.UserID,      gUserID.c_str());
	strcpy(UserPasswordUpdate.OldPassword, "112234");
	strcpy(UserPasswordUpdate.NewPassword, "123456");
	gCurRequestId++;
	FCTPAPI->ReqUserPasswordUpdate(	&UserPasswordUpdate, gCurRequestId );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnLoadDllClick(TObject *Sender)
{
	if ( FListener == NULL )
		FListener = new CTPListener();
	if ( FCTPAPI == NULL )
	{
		if ( RadioHSctp->Checked == true )
			FCTPAPI = new TThostFtdcTraderApi( dynamic_cast<TThostFtdcTraderSpi*>(FListener), "HSctp" );
		else if ( RadioKNctp->Checked == true )
			FCTPAPI = new TThostFtdcTraderApi( dynamic_cast<TThostFtdcTraderSpi*>(FListener), "KNctp" );
		else
			FCTPAPI = new TThostFtdcTraderApi( dynamic_cast<TThostFtdcTraderSpi*>(FListener), "CTP" );
	}

	if ( FCTPAPI == NULL )
		SetButtons( false, false );
	else
		SetButtons( true, false );

	Memo1->Lines->Add( FCTPAPI->LoadCTPAPIResult() );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnConn_LoginClick(TObject *Sender)
{
	SetGlobalVariant();

	FCTPAPI->SubscribePrivateTopic( THOST_TERT_RESUME );
	FCTPAPI->SubscribePublicTopic( THOST_TERT_RESUME );
	FCTPAPI->RegisterFront( gServerAddress.c_str() );
	FCTPAPI->Init( );
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
void TTradingForm::SetButtons( bool IsLoadDll, bool IsConnected )
{
	if ( IsLoadDll == false )
	{
		BtnLoadDll->Enabled    = true;
		BtnConn_Login->Enabled = false;
		Button1->Enabled       = false;
		Button2->Enabled       = false;
		Button3->Enabled       = false;
		Button4->Enabled       = false;
		Button5->Enabled       = false;
		BtnDisconnect->Enabled = false;
	}
	else
	{
		BtnLoadDll->Enabled    = false;
		if ( IsConnected )
		{
			BtnConn_Login->Enabled = false;
			Button1->Enabled       = false;
			Button2->Enabled       = true;
			Button3->Enabled       = true;
			Button4->Enabled       = true;
			Button5->Enabled       = false;
			BtnDisconnect->Enabled = true;
		}
		else
		{
			BtnConn_Login->Enabled = true;
			Button1->Enabled       = false;
			Button2->Enabled       = false;
			Button3->Enabled       = false;
			Button4->Enabled       = false;
			Button5->Enabled       = false;
			BtnDisconnect->Enabled = false;
		}
	}
}
//---------------------------------------------------------------------------
void TTradingForm::ReleaseAPI( void )
{
	// Disable Buttons
	SetButtons( false, false );

	// Release API
	if ( FCTPAPI != NULL )
	{
		delete FCTPAPI;
		FCTPAPI = NULL;
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

