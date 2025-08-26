//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "Unit1.h"

#include "KStrdAPIDynamic.h"
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
TTradingForm *TradingForm;
//---------------------------------------------------------------------------
// 光大證券
//---------------------------------------------------------------------------
/*
AnsiString gBrokerID;
AnsiString gUserID( "03000435" );
AnsiString gPassword( "123456" );
AnsiString gServerAddress( "tcp://116.236.247.173:17996" );
*/
//---------------------------------------------------------------------------
// 南華期貨
//---------------------------------------------------------------------------
AnsiString gBrokerID( "3748FD77" );

//AnsiString gUserID( "90086903" );
//AnsiString gPassword( "123456" );
/*
證券0000000147	密碼180846
期權90088683	密碼666666
有1手 備兌開倉的中國平安
*/
AnsiString gUserID( "90088683" );
/*AnsiString gUserStockID( "0000000147" );*/
AnsiString gPassword( "666666" );

AnsiString gServerAddress( "tcp://115.238.106.253:18993" );
//

AnsiString gExchangeID( "SSE" ); // 上海證券交易所
											// 合約編碼	交易代碼            合約簡稱
//AnsiString gInstrumentID( "20000135" ); 	// 20000135 600104C1504M01900	上汽集團購4月1900
//AnsiString gInstrumentID( "20000186" ); 	// 20000186	600104C1506M02250	上汽集團購6月2250
AnsiString gInstrumentID( "20000073" ); 	// 20000073	601318C1506M07500	中國平安保險集團購6月7500
//AnsiString gInstrumentID( "20000077" ); 	// 20000077	601318P1506M07000	中國平安保險集團沽6月7000

//AnsiString gInstrumentStockID( "600104" ); 	// 上汽集團(上海汽車集團) 		股票代碼600104
AnsiString gInstrumentStockID( "601318" ); 		// 中國平安(中國平安保險集團) 	股票代碼601318

AnsiString gQty( "1" );
AnsiString gSide( "Sell" );

//AnsiString gPx( "12.0" );				// 20000135
//AnsiString gPx( "17.65" );			// 20000186
AnsiString gPx( "21.23" ); //41.23 //31.23	// 20000073

//
AnsiString gFrontID( "" );
AnsiString gSessionID( "" );
//
/*
AnsiString gBrokerID;
AnsiString gUserID( "0000000064" );
//AnsiString gUserID( "0000000008" );
AnsiString gPassword( "123456" );
AnsiString gServerAddress( "tcp://115.238.106.253:18993" );
*/
//---------------------------------------------------------------------------
int gCurRequestId = 0;
int gOrderRef = 0;
//---------------------------------------------------------------------------
int iOrderRef = 0;	// iOrderRef: Keep用
int iLockRef = 0;	// iLockRef: Keep用
//---------------------------------------------------------------------------
char cTemp[256]; // 轉換用 Char Temp
//---------------------------------------------------------------------------

/* TStockLock */ // StockLock資料
TStockLock::TStockLock()
{
	DataClear();
}

void TStockLock::DataClear()
{
	sInsertDate = "", sInsertTime = "", sRequestID = "";
	sInstrumentID = "", sLockType = "", iVolume = 0;
	sStatusMsg = "", sClearingPartID = "";
}

//---------------------------------------------------------------------------
void KStrdListener::OnKStrdFrontConnected( )
{
	TradingForm->TimerMsg->Enabled = true;

	TradingForm->AddMemo( "1", L"********************************************" );
	TradingForm->AddMemo( "1", L"Connected!" );
	KS_::CThostFtdcReqUserLoginField reqUserLogin;
	memset( &reqUserLogin, 0, sizeof(reqUserLogin) );

	strcpy( reqUserLogin.BrokerID, gBrokerID.c_str() );
	strcpy( reqUserLogin.UserID,   gUserID.c_str() );
	strcpy( reqUserLogin.Password, gPassword.c_str() );
	strcpy( reqUserLogin.UserProductInfo, "S8 2020335" );

	TradingForm->FKSTRDAPI->ReqUserLogin( &reqUserLogin, 1 );
}
//---------------------------------------------------------------------------
void KStrdListener::OnKStrdFrontDisconnected( int nReason )
{
	TradingForm->TimerMsg->Enabled = true;

	TradingForm->AddMemo( "1", L"Disconnected! Reason: " + String(nReason));
	// Release API
	TradingForm->ReleaseAPI();
}
//---------------------------------------------------------------------------
void KStrdListener::OnKStrdRspUserLogin( KS_::CThostFtdcRspUserLoginField *pRspUserLogin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TradingForm->TimerMsg->Enabled = true;

	String Msg;

	if ( pRspInfo->ErrorID != 0 )
	{
		GB2312String ErrMsg( pRspInfo->ErrorMsg );
		String ErrMsgUnicode( ErrMsg );

		Msg.printf( L"Failed to login, Errorcode[%d] msg[%s] requestid[%d] chain[%d]"
					, pRspInfo->ErrorID, ErrMsgUnicode, nRequestID, bIsLast );
		TradingForm->AddMemo( "1", Msg );
	}
	else
	{
		AnsiString MaxRef( pRspUserLogin->MaxOrderRef );
		AnsiString OutMsg;

		gOrderRef = MaxRef.ToInt();
		gOrderRef++; // ???????? // KSFTAPI 2.3.50416 要先加1 // 20150410 by Tim Lin
		OutMsg.printf( "Login OK! MaxOrderRef[%s]", pRspUserLogin->MaxOrderRef );
		TradingForm->AddMemo( "1", OutMsg );
		// UserLogin
		sprintf( cTemp, "%012d", gOrderRef );
		TradingForm->TxtMaxOrderRef->Text = cTemp;
		//
		OutMsg.printf( "TradingDay[%s]", pRspUserLogin->TradingDay );
		TradingForm->AddMemo( "1", OutMsg );
		OutMsg.printf( "FrontID[%d]", pRspUserLogin->FrontID );
		TradingForm->TxtFrontID->Text = pRspUserLogin->FrontID;
		TradingForm->AddMemo( "1", OutMsg );
		OutMsg.printf( "SessionID[%d]", pRspUserLogin->SessionID );
		TradingForm->TxtSessionID->Text = pRspUserLogin->SessionID;
		TradingForm->AddMemo( "1", OutMsg );
	}
	TradingForm->Caption = L"User[" + gUserID + "]";

	TradingForm->AddMemo( "1", L"********************************************" );

	// Set Buttons
	TradingForm->SetButtons( true );
}
//---------------------------------------------------------------------------
void KStrdListener::OnKStrdRspUserPasswordUpdate( KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TradingForm->TimerMsg->Enabled = true;

	if ( pRspInfo != NULL )
	{
		if ( pRspInfo->ErrorID != 0 )
		{
			GB2312String ErrMsg( pRspInfo->ErrorMsg );
			String ErrMsgUnicode( ErrMsg );
			TradingForm->AddMemo( "1", L"Change Password failed: " + ErrMsgUnicode );
		}
	}
	else
	{
		TradingForm->AddMemo( "1", L"Change Password OK!" );
	}
}
//---------------------------------------------------------------------------
void KStrdListener::OnKStrdRspQryTradingAccount( KS_::CThostFtdcTradingAccountField *pTradingAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TradingForm->TimerMsg->Enabled = true;

	if ( pRspInfo != NULL )
	{
		if ( pRspInfo->ErrorID != 0 )
		{
			GB2312String ErrMsg( pRspInfo->ErrorMsg );
			String ErrMsgUnicode( ErrMsg );
			TradingForm->AddMemo( "1", ErrMsgUnicode );
		}
	}
	//
	if ( pTradingAccount != NULL )
	{
		if ( bIsLast )
		{
			AnsiString Msg;
			double staticBalance  = pTradingAccount->PreBalance -pTradingAccount->Withdraw + pTradingAccount->Deposit;
			double dynamicBalance = staticBalance +
									pTradingAccount->CloseProfit + pTradingAccount->PositionProfit - pTradingAccount->Commission;
			TradingForm->AddMemo( "1", L" 靜態權益 | 平倉盈虧| 持倉盈虧| 動態權益|  占用保證金| 下單動結| 可用資金| 風險度" );
			Msg.printf( "%10.2f|%10.2f|%10.2f|%10.2f|%10.2f|%10.2f|%10.2f|%10.2f",
						staticBalance,
						pTradingAccount->CloseProfit,
						pTradingAccount->PositionProfit ,
						dynamicBalance,
						pTradingAccount->CurrMargin,
						pTradingAccount->FrozenMargin + pTradingAccount->FrozenCommission,
						pTradingAccount->Available,
						pTradingAccount->CurrMargin/(dynamicBalance==0?1:dynamicBalance)*100 );
			TradingForm->AddMemo( "1", Msg );
		}
	}
}
//---------------------------------------------------------------------------
void KStrdListener::OnKStrdRspQrySettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TradingForm->TimerMsg->Enabled = true;

	if ( pRspInfo != NULL )
	{
		if ( pRspInfo->ErrorID != 0 )
		{
			GB2312String ErrMsg( pRspInfo->ErrorMsg );
			String ErrMsgUnicode( ErrMsg );

			TradingForm->AddMemo( "1", L" OnRspQrySettlementInfoConfirm failed code[" + String(pRspInfo->ErrorID) + "]: " + ErrMsgUnicode );
			return;
		}
	}
	//
	if ( pSettlementInfoConfirm != NULL )
	{
		String Msg;
		String Date(pSettlementInfoConfirm->ConfirmDate);
		String Time(pSettlementInfoConfirm->ConfirmTime);
		Msg.printf( L"Date[%s] Time[%s]", Date.c_str(), Time.c_str());
		TradingForm->AddMemo( "1", L"User confirm settlement Info at:" + Msg );
	}
	else
	{
		KS_::CThostFtdcQrySettlementInfoField QuerySettlementData;

		TradingForm->AddMemo( "1", L"User not confirm Settlement Info. " );
		memset( &QuerySettlementData, 0, sizeof(QuerySettlementData) );
		strcpy( QuerySettlementData.BrokerID,   gBrokerID.c_str() );
		strcpy( QuerySettlementData.InvestorID, gUserID.c_str() );
		gCurRequestId++;
		TradingForm->AddMemo( "1", L" ========== Begin ReqQrySettlementInfo ==========" );
		TradingForm->FKSTRDAPI->ReqQrySettlementInfo( &QuerySettlementData, gCurRequestId );
	}
}
//---------------------------------------------------------------------------
void KStrdListener::OnKStrdRspQrySettlementInfo( KS_::CThostFtdcSettlementInfoField *pSettlementInfo, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TradingForm->TimerMsg->Enabled = true;

	if ( pSettlementInfo != NULL )
	{
		if ( std::strlen(pSettlementInfo->Content) > 0 )
		{
			GB2312String Msg( pSettlementInfo->Content );
			TradingForm->AddMemo( "1", Msg );
		}
	}
	//
	if ( bIsLast )
	{
		TradingForm->AddMemo( "1", L" ========== Filished OnRspQrySettlementInfo ==========" );
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
		TradingForm->FKSTRDAPI->ReqSettlementInfoConfirm( &SettlementConfirmData, gCurRequestId );
	}
}
//---------------------------------------------------------------------------
void KStrdListener::OnKStrdRspSettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TradingForm->TimerMsg->Enabled = true;

	if ( bIsLast )
	{
		TradingForm->AddMemo( "1", L" ========== Filished OnRspSettlementInfoConfirm ==========" );
	}
}
//---------------------------------------------------------------------------
void KStrdListener::OnKStrdRspQryInvestorPosition( KS_::CThostFtdcInvestorPositionField *pInvestorPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TradingForm->TimerMsg->Enabled = true;

	if ( pRspInfo != NULL )
	{
		if ( pRspInfo->ErrorID != 0 )
		{
			GB2312String ErrMsg( pRspInfo->ErrorMsg );
			String ErrMsgUnicode( ErrMsg );
			TradingForm->AddMemo( "1", ErrMsgUnicode );
		}
	}
	//
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
			TradingForm->AddMemo( "1", L"     商品 | 買賣| 今昨|    今倉|    昨倉|  持倉成本|占用保證金|持倉盈虧" );
			TradingForm->AddMemo( "1", L"------------------------------------------------------" );
			for( int i =0;i< TradingForm->FStrings->Count; i++ )
			{
				 TradingForm->AddMemo( "1", TradingForm->FStrings->Strings[i] );
			}
			TradingForm->AddMemo( "1", L"------------------------------------------------------" );
		}
	}
	else
	{
		TradingForm->AddMemo( "1", L" 無留倉部位." );
	}
}
//---------------------------------------------------------------------------
void KStrdListener::OnKStrdRspQryInvestorPositionDetail( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TradingForm->TimerMsg->Enabled = true;

	if ( pRspInfo != NULL )
	{
		if ( pRspInfo->ErrorID != 0 )
		{
			GB2312String ErrMsg( pRspInfo->ErrorMsg );
			String ErrMsgUnicode( ErrMsg );
			TradingForm->AddMemo( "1", ErrMsgUnicode );
		}
	}
}
//---------------------------------------------------------------------------
void KStrdListener::OnKStrdRspQryInvestorOpenPosition( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TradingForm->TimerMsg->Enabled = true;

	if ( pRspInfo != NULL )
	{
		if ( pRspInfo->ErrorID != 0 )
		{
			GB2312String ErrMsg( pRspInfo->ErrorMsg );
			String ErrMsgUnicode( ErrMsg );
			TradingForm->AddMemo( "1", ErrMsgUnicode );
		}
	}
	//
	if ( pInvestorPositionDetail != NULL )
	{
		AnsiString Msg, BS;

		if ( pInvestorPositionDetail->Direction == THOST_FTDC_D_Buy )
			BS = "買";
		else if ( pInvestorPositionDetail->Direction == THOST_FTDC_D_Sell )
			BS = "賣";
		else
			BS = "Unknow";

		Msg.printf( "%10s|%5s|%9s|%d|%d|%0.2f|%s",
					pInvestorPositionDetail->InstrumentID,
					BS.c_str(),
					pInvestorPositionDetail->OpenDate,
					pInvestorPositionDetail->Volume,
					pInvestorPositionDetail->CloseVolume,
					pInvestorPositionDetail->OpenPrice,
					pInvestorPositionDetail->TradeID );
		TradingForm->FStrings->Add( Msg );

		if ( bIsLast )
		{
			Msg = L"     商品 | 買賣| 開倉日期|   數量| 平倉量|  開倉價|成交編號";
			TradingForm->AddMemo( "1", Msg );
			Msg = L"------------------------------------------------------";
			TradingForm->AddMemo( "1", Msg );
			for( int i =0;i< TradingForm->FStrings->Count; i++ )
			{
				Msg = TradingForm->FStrings->Strings[i];
				TradingForm->AddMemo( "1", Msg );
			}
			Msg = L"------------------------------------------------------";
			TradingForm->AddMemo( "1", Msg );
		}
	}
	else
	{
		TradingForm->AddMemo( "1", L" 無留倉部位." );
	}
}
//---------------------------------------------------------------------------
void KStrdListener::OnKStrdRspOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TradingForm->TimerMsg->Enabled = true;

	if ( pRspInfo != NULL )
	{
		if ( pRspInfo->ErrorID != 0 )
		{
			GB2312String ErrMsg( pRspInfo->ErrorMsg );
			String ErrMsgUnicode( ErrMsg );
			String Msg;

			Msg.printf( L"Failed palce new order, Errorcode[%d] msg[%s] requestid[%d] chain[%d]",
						  pRspInfo->ErrorID, ErrMsgUnicode, nRequestID, bIsLast );
			TradingForm->AddMemo( "1", Msg );
		}
	}
	else
	{
		TradingForm->AddMemo( "1", "Ack for New Order OK!" );
	}
}
//---------------------------------------------------------------------------
void KStrdListener::OnKStrdRtnOrder( KS_::CThostFtdcOrderField *pOrder )
{
	TradingForm->TimerMsg->Enabled = true;

	String Msg, Status;
	String SubmitStatus, CombHedge;
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
	String InsertDate( pOrder->InsertDate );
	String InsertTime( pOrder->InsertTime );
	String Qty( pOrder->VolumeTotalOriginal );
	String Px( pOrder->LimitPrice );

/*
	///投機
	#define THOST_FTDC_HF_Speculation '1'
	///套利
	#define THOST_FTDC_HF_Arbitrage '2'
	///套保
	#define THOST_FTDC_HF_Hedge '3'
	///備兌
	#define THOST_FTDC_HF_Covered '4'

	typedef char TThostFtdcHedgeFlagType;
*/
	switch ( pOrder->CombHedgeFlag[0] )
	{
		case THOST_FTDC_HF_Speculation: CombHedge = "Speculation 投機/非備兌"; break; ///< done
		case THOST_FTDC_HF_Arbitrage: CombHedge = "Arbitrage 套利"; break;
		case THOST_FTDC_HF_Hedge: CombHedge = "Hedge 套保"; break; ///< done
		case THOST_FTDC_HF_Covered: CombHedge = "Covered 備兌"; break;
		default: CombHedge = "!! UnKnow CombHedge !!"; break;
	}
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

	if (SubmitStatus == "THOST_FTDC_OSS_Accepted") // 已經報入
	{
		if (gExchangeID == ExchangeID && gInstrumentID == InstrumentID && gSide == Side && iOrderRef == OrdRef.ToInt())
		{
			TradingForm->TxtOrderSysID->Text = OrdSysID;
			sprintf( cTemp, "%012d", iOrderRef );
			if (cTemp == OrdRef)
			{
				TradingForm->TxtOrderRef->Text = OrdRef;
			}
			else
			{
				TradingForm->TxtOrderRef->Text = "轉換錯誤!";
			}
			TradingForm->TxtOrderLocalID->Text = OrderLocalID;
		}
	}

	Msg.printf( L" -------------------- Status[%s] -------------------------", Status );
	TradingForm->AddMemo( "1", Msg );
	Msg.printf( L"SubmitStatus[%s]", SubmitStatus );
	TradingForm->AddMemo( "1", Msg );
	Msg.printf( L"OrderRef[%s] OrderSysID[%s] SessionID[%d] FrontID[%d] ", OrdRef, OrdSysID, pOrder->SessionID , pOrder->FrontID );
	TradingForm->AddMemo( "1", Msg );
	Msg.printf( L"BrokerID[%s] TraderID[%s] InvestorID[%s] OrderLocalID[%s]", BrokerID, TraderID, InvestorID, OrderLocalID );
	TradingForm->AddMemo( "1", Msg );
	Msg.printf( L"ExchangeID[%s] InstrumentID[%s] CombHedge[%s]", ExchangeID, InstrumentID, CombHedge);
	TradingForm->AddMemo( "1", Msg );
	Msg.printf( L"StatusMsg[%s] Side[%s] Qty[%s] Px[%s] DateTime[%s %s]", String(StatusMsg), Side, Qty, Px, String(InsertDate), String(InsertTime) );
	TradingForm->AddMemo( "1", Msg );
}
//---------------------------------------------------------------------------
void KStrdListener::OnKStrdRtnTrade( KS_::CThostFtdcTradeField *pTrade )
{
	TradingForm->TimerMsg->Enabled = true;

	String Msg;
	String CombHedge;
	String OrdSysID( pTrade->OrderSysID );
	String OrdRef( pTrade->OrderRef );
	String BrokerID( pTrade->BrokerID );
	String InvestorID( pTrade->InvestorID );
	String InstrumentID( pTrade->InstrumentID );
	String Side( (pTrade->Direction==THOST_FTDC_D_Sell)?"Sell":"Buy" );
	String ExchangeID( pTrade->ExchangeID );
	String TraderID( pTrade->TraderID );
	String OrderLocalID( pTrade->OrderLocalID );
	String TradeDate( pTrade->TradeDate );
	String TradeTime( pTrade->TradeTime );
	String Qty( pTrade->Volume );
	String Px( pTrade->Price );

/*
	///投機
	#define THOST_FTDC_HF_Speculation '1'
	///套利
	#define THOST_FTDC_HF_Arbitrage '2'
	///套保
	#define THOST_FTDC_HF_Hedge '3'
	///備兌
	#define THOST_FTDC_HF_Covered '4'

	typedef char TThostFtdcHedgeFlagType;
*/
	switch ( pTrade->HedgeFlag )
	{
		case THOST_FTDC_HF_Speculation: CombHedge = "Speculation 投機/非備兌"; break; ///< done
		case THOST_FTDC_HF_Arbitrage: CombHedge = "Arbitrage 套利"; break;
		case THOST_FTDC_HF_Hedge: CombHedge = "Hedge 套保"; break; ///< done
		case THOST_FTDC_HF_Covered: CombHedge = "Covered 備兌"; break;
		default: CombHedge = "!! UnKnow CombHedge !!"; break;
	}

	Msg.printf( L" --------------------【 Trade 】-------------------------"  );
	TradingForm->AddMemo( "1", Msg );
	Msg.printf( L"BrokerID[%s] TraderID[%s] InvestorID[%s] OrderLocalID[%s] OrderSysID[%s]", BrokerID, TraderID, InvestorID, OrderLocalID, OrdSysID );
	TradingForm->AddMemo( "1", Msg );
	Msg.printf( L"ExchangeID[%s] InstrumentID[%s] CombHedge[%s]", ExchangeID, InstrumentID, CombHedge);
	TradingForm->AddMemo( "1", Msg );
	Msg.printf( L"Side[%s] Qty[%s] Px[%s] DateTime[%s %s]", Side, Qty, Px, String(TradeDate), String(TradeTime) );
	TradingForm->AddMemo( "1", Msg );
}
//---------------------------------------------------------------------------
void KStrdListener::OnKStrdRspError( KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TradingForm->TimerMsg->Enabled = true;

	if ( pRspInfo != NULL )
	{
		if ( pRspInfo->ErrorID != 0 )
		{
			GB2312String ErrMsg( pRspInfo->ErrorMsg );
			String ErrMsgUnicode( ErrMsg );
			TradingForm->AddMemo( "1", ErrMsgUnicode );
		}
	}
}
//---------------------------------------------------------------------------
void KStrdListener::OnKStrdRspOrderAction( KS_::CThostFtdcInputOrderActionField *pInputOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TradingForm->TimerMsg->Enabled = true;

	/*
	/////////////////////////////////////////////////////////////////////////
	///TFtdcActionFlagType是一個操作標誌類型
	/////////////////////////////////////////////////////////////////////////
	///刪除
	#define THOST_FTDC_AF_Delete '0'
	///修改
	#define THOST_FTDC_AF_Modify '3'

	typedef char TThostFtdcActionFlagType;
	*/

	if ( pRspInfo != NULL )
	{
		if ( pRspInfo->ErrorID != 0 )
		{
			GB2312String ErrMsg( pRspInfo->ErrorMsg );
			String ErrMsgUnicode( ErrMsg );

			String ActionFlag, Msg;
			String OrdSysID( pInputOrderAction->OrderSysID );
			String OrdRef( pInputOrderAction->OrderRef );
			String BrokerID( pInputOrderAction->BrokerID );
			String InvestorID( pInputOrderAction->InvestorID );
			String InstrumentID( pInputOrderAction->InstrumentID );
			String TraderID(  pInputOrderAction->UserID );
			String ExchangeID( pInputOrderAction->ExchangeID );

			switch ( pInputOrderAction->ActionFlag )
			{
				case THOST_FTDC_AF_Delete: ActionFlag = "Delete 刪除"; break;
				case THOST_FTDC_AF_Modify: ActionFlag = "Modify 修改"; break;
				default: ActionFlag = "!! UnKnow ActionFlag !!"; break;
			}

			Msg.printf( L" --------------------﹝ %s %s ﹞-------------------------", ActionFlag, ErrMsgUnicode );
			TradingForm->AddMemo( "1", Msg );
			Msg.printf( L"OrderRef[%s] OrderSysID[%s] SessionID[%d] FrontID[%d] ", OrdRef, OrdSysID, pInputOrderAction->SessionID , pInputOrderAction->FrontID );
			TradingForm->AddMemo( "1", Msg );
			Msg.printf( L"BrokerID[%s] TraderID[%s] InvestorID[%s]", BrokerID, TraderID, InvestorID );
			TradingForm->AddMemo( "1", Msg );
			Msg.printf( L"ExchangeID[%s] InstrumentID[%s] ActionFlag[%s]", ExchangeID, InstrumentID, ActionFlag);
			TradingForm->AddMemo( "1", Msg );
		}
	}
}
//---------------------------------------------------------------------------
void KStrdListener::OnKStrdErrRtnOrderAction( KS_::CThostFtdcOrderActionField *pOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo)
{
	TradingForm->TimerMsg->Enabled = true;

	if ( pRspInfo != NULL && pOrderAction != NULL)
	{
		if ( pRspInfo->ErrorID != 0 )
		{
			String ActionFlag, Msg;
			String OrdSysID( pOrderAction->OrderSysID );
			String OrdRef( pOrderAction->OrderRef );
			String BrokerID( pOrderAction->BrokerID );
			String InvestorID( pOrderAction->InvestorID );
			String InstrumentID( pOrderAction->InstrumentID );
			String TraderID(  pOrderAction->UserID );
			String ExchangeID( pOrderAction->ExchangeID );

			switch ( pOrderAction->ActionFlag )
			{
				case THOST_FTDC_AF_Delete: ActionFlag = "Delete 刪除"; break;
				case THOST_FTDC_AF_Modify: ActionFlag = "Modify 修改"; break;
				default: ActionFlag = "!! UnKnow ActionFlag !!"; break;
			}

			GB2312String ErrMsg( pRspInfo->ErrorMsg );
			String ErrMsgUnicode( ErrMsg );

			Msg.printf( L" --------------------﹝ %s Err ﹞-------------------------", ActionFlag );
			TradingForm->AddMemo( "1", Msg );
			Msg.printf( L"OrderRef[%s] OrderSysID[%s] SessionID[%d] FrontID[%d] ", OrdRef, OrdSysID, pOrderAction->SessionID , pOrderAction->FrontID );
			TradingForm->AddMemo( "1", Msg );
			Msg.printf( L"BrokerID[%s] TraderID[%s] InvestorID[%s]", BrokerID, TraderID, InvestorID );
			TradingForm->AddMemo( "1", Msg );
			Msg.printf( L"ExchangeID[%s] InstrumentID[%s] ActionFlag[%s]", ExchangeID, InstrumentID, ActionFlag);
			TradingForm->AddMemo( "1", Msg );
			TradingForm->AddMemo( "1", ErrMsgUnicode );
		}
	}
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
void KStrdListener::OnKStrdRspLockInsert( KS_::CThostFtdcInputLockField *pInputLock, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TradingForm->TimerMsg->Enabled = true;

	String ErrMsgUnicode = "";

	if ( pRspInfo != NULL )
	{
		if ( pRspInfo->ErrorID != 0 )
		{
			GB2312String ErrMsg( pRspInfo->ErrorMsg );
			ErrMsgUnicode =  ErrMsg;
			TradingForm->AddMemo( "1", ErrMsgUnicode + "ErrorMsg: (OnRspLockInsert應答)" );
		}
	}

	//
	if ( pInputLock != NULL )
	{
		String Msg;

		String BrokerID( pInputLock->BrokerID );
		String InvestorID( pInputLock->InvestorID );
		String UserID( pInputLock->UserID );
		String ExchangeID( pInputLock->ExchangeID );
		String InstrumentID( pInputLock->InstrumentID );
		//
		String LockRef( pInputLock->LockRef );
		String BusinessUnit( pInputLock->BusinessUnit );
		String LockType( (pInputLock->LockType == THOST_FTDC_LCKT_Lock) ? "Lock 錯誤":"Unlock 錯誤" );
		String Volume( pInputLock->Volume );
		String RequestID( pInputLock->RequestID );

		Msg.printf( L"BrokerID[%s] UserID[%s] InvestorID[%s] ExchangeID[%s] InstrumentID[%s]", BrokerID, UserID, InvestorID, ExchangeID, InstrumentID );
		TradingForm->AddMemo( "1", Msg );
		Msg.printf( L"LockRef[%s] BusinessUnit[%s] LockType[%s](OnRspLockInsert應答) Volume[%s] RequestID[%s]", LockRef, BusinessUnit, LockType, Volume, RequestID );
		TradingForm->AddMemo( "1", Msg );
		TradingForm->AddMemo( "1", L"------------------------------------------------------" );

		int iVolume = 0;
		if (Volume.Trim() != "")
			iVolume = StrToInt(Volume);
		if (ErrMsgUnicode == "")
			ErrMsgUnicode = LockRef;
		TradingForm->StockLock.DataClear();
		TradingForm->StockLock.sInsertDate = "";
		TradingForm->StockLock.sInsertTime = "";
		TradingForm->StockLock.sRequestID = RequestID;
		TradingForm->StockLock.sInstrumentID = InstrumentID;
		TradingForm->StockLock.sLockType = LockType;
		TradingForm->StockLock.iVolume = iVolume;
		TradingForm->StockLock.sStatusMsg = ErrMsgUnicode;
		TradingForm->StockLock.sClearingPartID = "";
		TradingForm->DoStockLock(TradingForm->StockLock);
		TradingForm->TimerStockLock->Enabled = true;
	}
	else
	{
		TradingForm->TxtLockSysID->Text = "";
		TradingForm->AddMemo( "1", L" 無鎖定.(OnRspLockInsert應答)" );
	}
}
//---------------------------------------------------------------------------
///請求查詢鎖定回應
/*
OnRspQryLock 方法
請求查詢鎖定回應。當用戶端發出請求查詢鎖定指令後，
Kingstar 伺服器返回回應時，該方法會被調用。
*/
void KStrdListener::OnKStrdRspQryLock( KS_::CThostFtdcLockField *pLock, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TradingForm->TimerMsg->Enabled = true;

	if ( pRspInfo != NULL )
	{
		if ( pRspInfo->ErrorID != 0 )
		{
			GB2312String ErrMsg( pRspInfo->ErrorMsg );
			String ErrMsgUnicode( ErrMsg );
			TradingForm->AddMemo( "1", ErrMsgUnicode + "ErrorMsg: (OnRspQryLock回應)" );
		}
	}

	//
	if ( pLock != NULL )
	{
		String Msg;
		/*
		v pLock->BrokerID;
		v pLock->InvestorID;
		v pLock->InstrumentID;
		v pLock->LockRef;
		v pLock->UserID;
		v pLock->Volume;
		v pLock->RequestID;
		v pLock->BusinessUnit;
		v pLock->LockType;
		pLock->LockLocalID;
		v pLock->ExchangeID;
		pLock->ParticipantID;
		pLock->ClientID;
		pLock->ExchangeInstID;
		pLock->TraderID;
		pLock->InstallID;
		pLock->OrderSubmitStatus;
		pLock->NotifySequence;
		pLock->TradingDay;
		pLock->SettlementID;
		pLock->LockSysID;
		pLock->InsertDate;
		pLock->InsertTime;
		pLock->CancelTime;
		pLock->ExecResult;
		pLock->ClearingPartID;
		pLock->SequenceNo;
		pLock->FrontID;
		pLock->SessionID;
		pLock->UserProductInfo;
		pLock->StatusMsg;
		pLock->ActiveUserID;
		pLock->BrokerLockSeq;
		pLock->BranchID;
		*/

		String BrokerID( pLock->BrokerID );
		String InvestorID( pLock->InvestorID );
		String UserID( pLock->UserID );
		String ExchangeID( pLock->ExchangeID );
		String InstrumentID( pLock->InstrumentID );
		//
		String LockRef( pLock->LockRef );
		String BusinessUnit( pLock->BusinessUnit );
		String LockType( (pLock->LockType == THOST_FTDC_LCKT_Lock) ? "Lock 鎖定":"Unlock 解鎖" );
		String Volume( pLock->Volume );
		String RequestID( pLock->RequestID );

		Msg.printf( L"BrokerID[%s] UserID[%s] InvestorID[%s] ExchangeID[%s] InstrumentID[%s]", BrokerID, UserID, InvestorID, ExchangeID, InstrumentID );
		TradingForm->AddMemo( "1", Msg );
		Msg.printf( L"LockRef[%s] BusinessUnit[%s] LockType[%s](OnRspQryLock回應) Volume[%s] RequestID[%s]", LockRef, BusinessUnit, LockType, Volume, RequestID );
		TradingForm->AddMemo( "1", Msg );
		TradingForm->AddMemo( "1", L"------------------------------------------------------" );

	}
	else
	{
		TradingForm->AddMemo( "1", L" 無查詢鎖定.(OnRspQryLock回應)" );
	}
}
//---------------------------------------------------------------------------
///請求查詢鎖定證券倉位回應
/*
OnRspQryLockPosition 方法
請求查詢鎖定證券倉位回應。當用戶端發出請求查詢鎖定證券倉位元指令後，
Kingstar 服務器返回回應時，該方法會被調用。
*/
void KStrdListener::OnKStrdRspQryLockPosition( KS_::CThostFtdcLockPositionField *pLockPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TradingForm->TimerMsg->Enabled = true;

	if ( pRspInfo != NULL )
	{
		if ( pRspInfo->ErrorID != 0 )
		{
			GB2312String ErrMsg( pRspInfo->ErrorMsg );
			String ErrMsgUnicode( ErrMsg );
			TradingForm->AddMemo( "1", ErrMsgUnicode + "ErrorMsg: (OnRspQryLockPosition回應)" );
		}
	}

	//
	if ( pLockPosition != NULL )
	{
		AnsiString Msg;
		AnsiString Volume = Msg.printf( "%d", pLockPosition->Volume );
		AnsiString FrozenVolume = Msg.printf( "%d", pLockPosition->FrozenVolume );
		//
		String ExchangeID( pLockPosition->ExchangeID );
		ExchangeID = ExchangeID.Trim();
		String InstrumentID( pLockPosition->InstrumentID );
		InstrumentID = InstrumentID.Trim();

		Msg.printf( "%14s |%12s |%8s |%12s |%12s |%12s",
					pLockPosition->BrokerID,
					pLockPosition->InvestorID,
					pLockPosition->ExchangeID,
					pLockPosition->InstrumentID,
					Volume,
					FrozenVolume );

		TradingForm->FStrings->Add( Msg );

		if ( bIsLast )
		{
			TradingForm->AddMemo( "1", L"經紀公司代碼  |投資者代碼  |交易所  |      商品  |      數量  |  凍結數量  " );
			TradingForm->AddMemo( "1", L"------------------------------------------------------" );
			for( int i =0;i< TradingForm->FStrings->Count; i++ )
			{
				 TradingForm->AddMemo( "1", TradingForm->FStrings->Strings[i] );
			}
			TradingForm->AddMemo( "1", L"--------------(OnRspQryLockPosition回應)--------------" );
		}
		else
		{
			int i = 0;
		}
	}
	else
	{
		TradingForm->AddMemo( "1", L" 無鎖定證券倉位.(OnRspQryLockPosition回應)" );
	}
}
//---------------------------------------------------------------------------
///鎖定通知
/*
OnRtnLock 方法 <== 成功回檔
鎖定通知。
*/
void KStrdListener::OnKStrdRtnLock( KS_::CThostFtdcLockField *pLock )
{
	TradingForm->TimerMsg->Enabled = true;

	if ( pLock != NULL )
	{
		String Msg;
		/*
		v pLock->BrokerID;			///經紀公司代碼
		v pLock->InvestorID;		///投資者代碼
		v pLock->InstrumentID;      ///合約代碼
		v pLock->LockRef;			///鎖定引用
		v pLock->UserID;			///用戶代碼
		v pLock->Volume;			///數量
		v pLock->RequestID;			///請求編號
		v pLock->BusinessUnit;		///業務單元
		v pLock->LockType;			///鎖定類型
		pLock->LockLocalID;			///本地鎖定編號
		v pLock->ExchangeID;		///交易所代碼
		pLock->ParticipantID;		///會員代碼
		pLock->ClientID;			///客戶代碼
		pLock->ExchangeInstID;		///合約在交易所的代碼
		pLock->TraderID;			///交易所交易員代碼
		pLock->InstallID;			///安裝編號
		pLock->OrderSubmitStatus;	///執行宣告提交狀態
		pLock->NotifySequence;		///報單提示序號
		pLock->TradingDay;			///交易日
		pLock->SettlementID;		///結算編號
		v pLock->LockSysID;			///鎖定編號
		v pLock->InsertDate;			///報單日期
		v pLock->InsertTime;			///插入時間
		v pLock->CancelTime;			///撤銷時間
		v pLock->LockStatus;			///鎖定狀態
		pLock->ClearingPartID;		///結算會員編號
		pLock->SequenceNo;			///序號
		pLock->FrontID;				///前置編號
		pLock->SessionID;			///會話編號
		pLock->UserProductInfo;		///使用者端產品資訊
		v pLock->StatusMsg;			///狀態資訊
		pLock->ActiveUserID;		///操作用戶代碼
		pLock->BrokerLockSeq;		///經紀公司報單編號
		pLock->BranchID;			///營業部編號
		*/

		String BrokerID( pLock->BrokerID );
		String InvestorID( pLock->InvestorID );
		String UserID( pLock->UserID );
		String ExchangeID( pLock->ExchangeID );
		String InstrumentID( pLock->InstrumentID );
		//
		String LockRef( pLock->LockRef );
		String BusinessUnit( pLock->BusinessUnit );
		String LockType( (pLock->LockType == THOST_FTDC_LCKT_Lock) ? "Lock":"Unlock" );
		String Volume( pLock->Volume );
		String RequestID( pLock->RequestID );
		//
		String LockSysID( pLock->LockSysID );
		String InsertDate( pLock->InsertDate );			///報單日期
		String InsertTime( pLock->InsertTime );			///插入時間
		String CancelTime( pLock->CancelTime );			///撤銷時間
		String LockStatus( pLock->LockStatus );			///鎖定狀態
		GB2312String StatusMsg( pLock->StatusMsg );		///狀態資訊
		String StatusMsgUnicode( StatusMsg );
		StatusMsgUnicode = StatusMsgUnicode.Trim();
		String ClearingPartID( pLock->ClearingPartID );	///結算會員編號

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
			case THOST_FTDC_OAS_Submitted: LockStatus = "Submitted 提交"; break;
			case THOST_FTDC_OAS_Accepted: LockStatus = "Accepted 接受"; break;
			case THOST_FTDC_OAS_Rejected: LockStatus = "Rejected 拒絕"; break;
			default: LockStatus = "!! UnKnow LockStatus !!"; break;
		}

		if (LockSysID.Trim() == "") ///鎖定編號
		{
			LockSysID = "Error !! Is NULL !!";
		}
		else
		{
			if (pLock->LockType == THOST_FTDC_LCKT_Lock) // Lock 鎖定
			{
				if (gExchangeID == ExchangeID && gInstrumentStockID == InstrumentID && iLockRef == LockRef.ToInt())
				{
					/*
					//TradingForm->TxtOrderRef->Text = LockRef;
					*/
					//
					TradingForm->TxtLockSysID->Text = LockSysID;
				}
			}
		}

		Msg.printf( L"BrokerID[%s] UserID[%s] InvestorID[%s] ExchangeID[%s] InstrumentID[%s]", BrokerID, UserID, InvestorID, ExchangeID, InstrumentID );
		TradingForm->AddMemo( "1", Msg );
		Msg.printf( L"LockRef[%s] BusinessUnit[%s] LockType[%s](OnRtnLock通知) Volume[%s] RequestID[%s]", LockRef, BusinessUnit, LockType, Volume, RequestID );
		TradingForm->AddMemo( "1", Msg );
		Msg.printf( L"委託編號LockSysID[%s] 委託時間DateTime[%s %s] CancelTime[%s] ClearingPartID[%s]", LockSysID, String(InsertDate), String(InsertTime), CancelTime, ClearingPartID );
		TradingForm->AddMemo( "1", Msg );
		Msg.printf( L"LockStatus[%s] StatusMsg[%s]", LockStatus, StatusMsgUnicode );
		TradingForm->AddMemo( "1", Msg );
		TradingForm->AddMemo( "1", L"-------------------(OnRtnLock通知)--------------------" );

		int iVolume = 0;
		if (Volume.Trim() != "")
			iVolume = StrToInt(Volume);
		if (StatusMsgUnicode == "")
			StatusMsgUnicode = LockStatus;
		TradingForm->StockLock.DataClear();
		TradingForm->StockLock.sInsertDate = InsertDate;
		TradingForm->StockLock.sInsertTime = InsertTime;
		TradingForm->StockLock.sRequestID = RequestID;
		TradingForm->StockLock.sInstrumentID = InstrumentID;
		TradingForm->StockLock.sLockType = LockType;
		TradingForm->StockLock.iVolume = iVolume;
		TradingForm->StockLock.sStatusMsg = StatusMsgUnicode;
		TradingForm->StockLock.sClearingPartID = ClearingPartID;
		TradingForm->DoStockLock(TradingForm->StockLock);
		TradingForm->TimerStockLock->Enabled = true;
	}
	else
	{
		TradingForm->AddMemo( "1", L" 無鎖定.(OnRtnLock通知)" );
	}
}
//---------------------------------------------------------------------------
///鎖定錯誤通知
/*
OnErrRtnLockInsert 方法
鎖定錯誤通知。當用戶端發出鎖定錯誤指令後，
Kingstar 伺服器返回回應時，該方法會被調用。
*/
void KStrdListener::OnKStrdErrRtnLockInsert( KS_::CThostFtdcInputLockField *pInputLock, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TradingForm->TimerMsg->Enabled = true;

	if ( pRspInfo != NULL )
	{
		if ( pRspInfo->ErrorID != 0 )
		{
			GB2312String ErrMsg( pRspInfo->ErrorMsg );
			String ErrMsgUnicode( ErrMsg );
			TradingForm->AddMemo( "1", ErrMsgUnicode + "(ErrorMsg: OnErrRtnLockInsert通知)" );
		}
	}

	if ( pInputLock != NULL )
	{
			String Msg;

			String BrokerID( pInputLock->BrokerID );
			String InvestorID( pInputLock->InvestorID );
			String UserID( pInputLock->UserID );
			String ExchangeID( pInputLock->ExchangeID );
			String InstrumentID( pInputLock->InstrumentID );
			//
			String LockRef( pInputLock->LockRef );
			String BusinessUnit( pInputLock->BusinessUnit );
			String LockType( (pInputLock->LockType == THOST_FTDC_LCKT_Lock) ? "Lock 錯誤":"Unlock 錯誤" );
			String Volume( pInputLock->Volume );
			String RequestID( pInputLock->RequestID );

			Msg.printf( L"BrokerID[%s] UserID[%s] InvestorID[%s] ExchangeID[%s] InstrumentID[%s]", BrokerID, UserID, InvestorID, ExchangeID, InstrumentID );
			TradingForm->AddMemo( "1", Msg );
			Msg.printf( L"LockRef[%s] BusinessUnit[%s] LockType[%s](錯誤!!OnErrRtnLockInsert通知) Volume[%s] RequestID[%s]", LockRef, BusinessUnit, LockType, Volume, RequestID );
			TradingForm->AddMemo( "1", Msg );
			TradingForm->AddMemo( "1", L"------------(錯誤!!OnErrRtnLockInsert通知)----------------" );
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
	//mapMemoData.clear();
	//pMD = mapMemoData.begin();
	//
	TimerMsg = new TTimer(this);
	TimerMsg->Enabled = false;
	TimerMsg->Interval =500;
	TimerMsg->OnTimer = OnTimerMsg;
	TimerMsg->Enabled = true;
	//
	TimerStockLock = new TTimer(this);
	TimerStockLock->Enabled = false;
	TimerStockLock->Interval = 500;
	TimerStockLock->OnTimer = OnTimerStockLock;

	PageControl1->ActivePageIndex = 0;
	PageControl1Change( NULL );

	FStrings = new TStringList();
	TradingForm->AddMemo( "1", TKStrdTraderApi::LoadKStrdAPIResult() );

	// 初始化 int gOrderRef = 0;
	TradingForm->TxtMaxOrderRef->Text = IntToStr(gOrderRef);

	// Set Screen
	GetGlobalVariant( );
	SetButtons( false );
}
//---------------------------------------------------------------------------
__fastcall TTradingForm::~TTradingForm( )
{
	delete TimerMsg;
	TimerMsg = NULL;
	delete FStrings;
	FStrings = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::OnTimerMsg(TObject *Sender)
{
	TimerMsg->Enabled = false;
	try
	{
		for (int i= 1; i<=100; i++)
		{
			Sleep(5);
			Application->ProcessMessages();
		}
	}
	catch (...)
	{
		TimerMsg->Enabled = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::OnTimerStockLock(TObject *Sender)
{
	TimerStockLock->Enabled = false;
	try
	{
		StringGrid_DataClear(StringGrid_StockLock, mapStockLock.size());
		TStockLock aStockLock;
		int iRowIdx = StringGrid_StockLock->FixedRows;
		for (p_StockLock = mapStockLock.begin(); p_StockLock != mapStockLock.end(); p_StockLock++)
		{
			aStockLock = p_StockLock->second;
			StringGrid_StockLock->Cells[0][iRowIdx] = aStockLock.sInsertTime;
			StringGrid_StockLock->Cells[1][iRowIdx] = aStockLock.sInstrumentID;
			StringGrid_StockLock->Cells[2][iRowIdx] = aStockLock.sLockType;
			StringGrid_StockLock->Cells[3][iRowIdx] = IntToStr(aStockLock.iVolume);
			StringGrid_StockLock->Cells[4][iRowIdx] = aStockLock.sStatusMsg;
			StringGrid_StockLock->Cells[5][iRowIdx] = aStockLock.sRequestID;
			StringGrid_StockLock->Cells[6][iRowIdx] = aStockLock.sClearingPartID;
			//
			iRowIdx++;
		}
	}
	catch (...)
	{
		TimerStockLock->Enabled = false;
	}
	StringGrid_StockLock->Row = StringGrid_StockLock->RowCount - 1;
	StringGrid_StockLock->Refresh();
}
//---------------------------------------------------------------------------
void TTradingForm::AddMemo(String Kind, String Text)
{
	//TMemoData oMD;
	//oMD.sKind = Kind;
	//oMD.sText = Text;

	//String sDateTime = Now().FormatString("yyyy-MM-dd hh:mm:ss.zzz");
	String sDateTime = Now().FormatString("hh:mm:ss.zzz");

	//mapMemoData.insert(pair<String, TMemoData>(sDateTime, oMD));
	if (Kind == "1")
	{
		TradingForm->Memo1->Lines->Add( sDateTime + " " + Text );
	}
	else if (Kind == "2")
	{
		//TradingForm->Memo2->Lines->Add( sDateTime + " " + Text );
	}
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::Button1Click(TObject *Sender)
{
	// New Order

	SetGlobalVariant( );

	KS_::CThostFtdcInputOrderField ord;

	memset( &ord, 0, sizeof(ord) );
	// From User basic info.
	strcpy( ord.BrokerID,   gBrokerID.c_str() ); // 經紀公司代碼
	strcpy( ord.InvestorID, gUserID.c_str() ); // 投資者代碼
	strcpy( ord.UserID,     gUserID.c_str() ); // 用戶代碼
	// From UI
	strcpy( ord.ExchangeID, gExchangeID.c_str() ); // 交易所代碼 // v2.3.50212 // 20150410 by Tim Lin
	strcpy( ord.InstrumentID, gInstrumentID.c_str() ); // 合約代碼
	//strcpy( ord.InstrumentID, "11000072" ); // 合約代碼
	// Side (B/S)
	//ord.Direction = THOST_FTDC_D_Buy; // 買賣方向
	//ord.Direction = THOST_FTDC_D_Sell; // 買賣方向
	if (gSide == "Buy")
	{ // Buy
		ord.Direction = THOST_FTDC_D_Buy; // 買賣方向
	}
	else
	{ // Sell
		ord.Direction = THOST_FTDC_D_Sell; // 買賣方向
	}

	// Order Type (Market or Limit)
	// Limit
	ord.OrderPriceType = THOST_FTDC_OPT_LimitPrice; // 報單價格條件
	ord.LimitPrice = gPx.ToDouble(); // 價格
	//ord.LimitPrice = 0.66; // 價格
	/*
	// Market
	ord.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
	ord.LimitPrice     = 0.0;
	*/

	// Order Qty
	ord.VolumeTotalOriginal = gQty.ToInt(); // 數量
	// Time in force
	ord.TimeCondition = THOST_FTDC_TC_GFD;	// 有效期類型
	// Position effect.
	ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open; // 組合開平標誌

	/*
	// CombHedge Flag. ///< 組合投機套保標誌
	if ( Msg->GetCombHedgeFlag() == nsOrderMessageDefine::hfSpeculation )
		ord.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;		///< Speculation ///投機/非備兌
	else if( Msg->GetCombHedgeFlag() == nsOrderMessageDefine::hfArbitrage )
		ord.CombHedgeFlag[0] = THOST_FTDC_HF_Arbitrage;		///< Arbitrage ///套利
	else if( Msg->GetCombHedgeFlag() == nsOrderMessageDefine::hfHedge )
		ord.CombHedgeFlag[0] = THOST_FTDC_HF_Hedge;		///< Hedge ///套保
	else if( Msg->GetCombHedgeFlag() == nsOrderMessageDefine::hfCovered )
		ord.CombHedgeFlag[0] = THOST_FTDC_HF_Covered;		///< Covered ///備兌
	else strcpy( ord.CombHedgeFlag, "1" );	///投機/非備兌
	*/
	////strcpy( ord.CombHedgeFlag, "0" ); // ??? // 備兌標誌類型 : 非備兌
	////strcpy( ord.CombHedgeFlag, "1" ); // 組合投機套保標誌

	if (RadioGroup_CombHedge->ItemIndex == 0)
	{
		ord.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;		///< Speculation ///投機/非備兌
	}
	else
	{
		ord.CombHedgeFlag[0] = THOST_FTDC_HF_Covered;			///< Covered ///備兌
	}

	// Fixed value field
	strcpy( ord.GTDDate, "" ); // GTD日期
	ord.VolumeCondition = THOST_FTDC_VC_AV; // 成交量類型
	ord.MinVolume = 0; // 最小成交量
	ord.ContingentCondition = THOST_FTDC_CC_Immediately; // 觸發條件
	ord.StopPrice = 0; // 止損價
	ord.ForceCloseReason = THOST_FTDC_FCC_NotForceClose; // 強平原因
	ord.IsAutoSuspend = 0; // 自動掛起標誌

	// Order Sequence
	iOrderRef = gOrderRef; // iOrderRef: Keep用
	sprintf( cTemp, "%012d", gOrderRef );
	TradingForm->TxtOrderRef->Text = cTemp;
	sprintf( ord.OrderRef, "%012d", gOrderRef ); // 報單引用
	gOrderRef++;

	gCurRequestId++;
	int Rtn = FKSTRDAPI->ReqOrderInsert( &ord, gCurRequestId );
	if ( Rtn == 0 )
	{
		TradingForm->AddMemo( "1", L" NewOrder ReqID: " + String( gCurRequestId ) );
	}
	else
	{
		TradingForm->AddMemo( "1", L" NewOrder ErrCode: " + String( Rtn ) );
	}
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::Button7Click(TObject *Sender)
{
	// Cancel Order

	SetGlobalVariant( );

	KS_::CThostFtdcInputOrderActionField	CancelOrder;

	memset( &CancelOrder, 0, sizeof(CancelOrder) );
	CancelOrder.LimitPrice     = 0.0;
	CancelOrder.VolumeChange   = 0;
	CancelOrder.ActionFlag     = THOST_FTDC_AF_Delete;
	CancelOrder.FrontID        = gFrontID.ToInt();
	CancelOrder.SessionID      = gSessionID.ToInt();
	////CancelOrder.OrderActionRef;//int
	strcpy( CancelOrder.BrokerID,      gBrokerID.c_str() ); // 經紀公司代碼
	strcpy( CancelOrder.InvestorID,    gUserID.c_str() ); // 投資者代碼
	////strcpy( CancelOrder.UserID,        gUserID.c_str() ); // 用戶代碼

	// From UI
	strcpy( CancelOrder.ExchangeID,    gExchangeID.c_str() ); // 交易所代碼
	strcpy( CancelOrder.InstrumentID,  gInstrumentID.c_str() ); // 合約代碼

	/*
	KS刘艳云-API开发:
	新版也可以用exchangeid+ordersysid撤单的，
	优先使用FrontID SessionID OrderRef撤单，
	如果这几个值有没填的，就用exchangeid+ordersysid撤单
	*/

	/*
	//AnsiString sTxtOrderSysID = TxtOrderSysID->Text;
	//strcpy( CancelOrder.OrderSysID,    sTxtOrderSysID.c_str() );
	sprintf( CancelOrder.OrderSysID, "%020d", StrToInt(TxtOrderSysID->Text) );
	*/

	if (TxtOrderRef->Text == "")
	{
		MessageBox(Application->Handle, String("OrderRef is NULL !").c_str(), String("Cancel Order [Error]").c_str(),
			MB_ICONERROR + MB_OK + MB_TOPMOST);
		TxtOrderRef->SetFocus();
		return;
	}

	// Order Sequence
	sprintf( CancelOrder.OrderRef, "%012d", StrToInt(TxtOrderRef->Text) );

	gCurRequestId++;
	int Rtn = FKSTRDAPI->ReqOrderAction( &CancelOrder, gCurRequestId );
	if ( Rtn == 0 )
	{
		TradingForm->AddMemo( "1", L"------------------------------------------------------" );
		TradingForm->AddMemo( "1", L" CancelOrder ReqID: " + String( gCurRequestId ) + " ; OrderRef[" + CancelOrder.OrderRef + "]");
	}
	else
	{
		TradingForm->AddMemo( "1", L"------------------------------------------------------" );
		TradingForm->AddMemo( "1", L" CancelOrder ErrCode: " + String( Rtn ) + " ; OrderRef[" + CancelOrder.OrderRef + "]" );
	}
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::Button2Click(TObject *Sender)
{
	KS_::CThostFtdcQrySettlementInfoConfirmField QrySettlementInfoConfirm;

	memset( &QrySettlementInfoConfirm, 0, sizeof(QrySettlementInfoConfirm) );
	strcpy( QrySettlementInfoConfirm.BrokerID,   gBrokerID.c_str() );
	strcpy( QrySettlementInfoConfirm.InvestorID, gUserID.c_str() );

	gCurRequestId++;
	TradingForm->AddMemo( "1", L" ========== Begin ReqQrySettlementInfoConfirm ==========" );
	int rt = FKSTRDAPI->ReqQrySettlementInfoConfirm( &QrySettlementInfoConfirm, gCurRequestId );
	TradingForm->AddMemo( "1", L" Return:" + String(rt) );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::Button3Click(TObject *Sender)
{
	int Rtn;

	KS_::CThostFtdcQryInvestorPositionField QryInvestorPosition;

	memset( &QryInvestorPosition, 0 , sizeof(QryInvestorPosition) );
	strcpy(QryInvestorPosition.BrokerID,   gBrokerID.c_str());
	strcpy(QryInvestorPosition.InvestorID, gUserID.c_str());
	gCurRequestId++;
	FStrings->Clear();
	Rtn = FKSTRDAPI->ReqQryInvestorPosition( &QryInvestorPosition, gCurRequestId );
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
	FKSTRDAPI->ReqQryTradingAccount( &QryTradingAccount, gCurRequestId );
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
	FKSTRDAPI->ReqUserPasswordUpdate( &UserPasswordUpdate, gCurRequestId );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::Button6Click(TObject *Sender)
{
	int Rtn;

	KS_::CThostFtdcQryInvestorPositionDetailField QryInvestorPositionDetail;

	memset( &QryInvestorPositionDetail, 0 , sizeof(QryInvestorPositionDetail) );
	strcpy(QryInvestorPositionDetail.BrokerID,   gBrokerID.c_str());
	strcpy(QryInvestorPositionDetail.InvestorID, gUserID.c_str());
	gCurRequestId++;
	FStrings->Clear();
	Rtn = FKSTRDAPI->ReqQueryInvestorOpenPosition( &QryInvestorPositionDetail, gCurRequestId );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnConn_LoginClick(TObject *Sender)
{
	SetGlobalVariant( );

	if ( FListener == NULL )
		FListener = new KStrdListener();
	if ( FKSTRDAPI == NULL )
		FKSTRDAPI = new TKStrdTraderApi( dynamic_cast<TKStrdTraderSpi*>(FListener) );
	FKSTRDAPI->SubscribePrivateTopic( KS_::THOST_TERT_RESTART );
	FKSTRDAPI->SubscribePublicTopic( KS_::THOST_TERT_RESUME );
	FKSTRDAPI->RegisterFront( gServerAddress.c_str() );
	FKSTRDAPI->Init( );
	String Msg;
	Msg.printf( L"Connect...[%s]", String( gServerAddress.c_str()).c_str() );
	TradingForm->AddMemo( "1", Msg );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnDisconnectClick(TObject *Sender)
{
	ReleaseAPI();
}
//---------------------------------------------------------------------------
void TTradingForm::GetGlobalVariant( void )
{
	TxtBrokerID->Text      		= gBrokerID.c_str();
	TxtUserID->Text        		= gUserID.c_str();
	TxtPassword->Text      		= gPassword.c_str();
	TxtServerAddress->Text 		= gServerAddress.c_str();
	//
	TxtExchangeID->Text    		= gExchangeID.c_str();
	TxtInstrumentID->Text  		= gInstrumentID.c_str();
	TxtInstrumentStockID->Text  = gInstrumentStockID.c_str();
	TxtQty->Text    	   		= gQty.c_str();
	TxtPx->Text    		   		= gPx.c_str();
	if (gSide == "Buy")
	{ // Buy
		RadioGroup_BS->ItemIndex == 0; // 買賣方向
	}
	else
	{ // Sell
		RadioGroup_BS->ItemIndex == 1; // 買賣方向
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SetGlobalVariant( void )
{
	gBrokerID      		= TxtBrokerID->Text.Trim();
	gUserID        		= TxtUserID->Text.Trim();
	gPassword      		= TxtPassword->Text.Trim();
	gServerAddress 		= TxtServerAddress->Text.Trim();
	//
	gExchangeID	   		= TxtExchangeID->Text.Trim();
	gInstrumentID  		= TxtInstrumentID->Text.Trim();
	gInstrumentStockID	= TxtInstrumentStockID->Text.Trim();
	gQty		   		= TxtQty->Text.Trim();
	gPx			   		= TxtPx->Text.Trim();
	if (RadioGroup_BS->ItemIndex == 0)
	{ // Buy
		gSide = "Buy"; // 買賣方向
	}
	else
	{ // Sell
		gSide = "Sell"; // 買賣方向
	}
	//
	if (TxtFrontID->Text.Trim() != "")
	{
		gFrontID       = TxtFrontID->Text.Trim();
	}
	if (TxtSessionID->Text.Trim() != "")
	{
		gSessionID     = TxtSessionID->Text.Trim();
	}
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
		Button6->Enabled       = true;
		Button7->Enabled       = true;
		// Lock
		Button8->Enabled       = true;
		Button9->Enabled       = true;
		Button10->Enabled      = true;
		//
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
		Button6->Enabled       = false;
		Button7->Enabled       = false;
		// Lock
		Button8->Enabled       = false;
		Button9->Enabled       = false;
		Button10->Enabled      = false;
		//
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
	if ( FKSTRDAPI != NULL )
	{
		delete FKSTRDAPI;
		FKSTRDAPI = NULL;
	}
	Sleep( 1000 );
	if ( FListener != NULL )
	{
		delete FListener;
		FListener = NULL;
	}
	TradingForm->AddMemo( "1", L"********************************************" );
	TradingForm->AddMemo( "1", L"Client Disconnect and Release API!!" );
	TradingForm->AddMemo( "1", L"********************************************" );
}
//---------------------------------------------------------------------------

void __fastcall TTradingForm::Button8Click(TObject *Sender)
{
	// Lock Insert ///用戶端發出鎖定請求

	SetGlobalVariant( );

	KS_::CThostFtdcInputLockField InputLock;

	memset( &InputLock, 0, sizeof(InputLock) );

	/*
	v InputLock.BrokerID;
	v InputLock.InvestorID;
	v InputLock.UserID;
	//
	v InputLock.ExchangeID;
	v InputLock.InstrumentID;
	//
	InputLock.LockRef;
	InputLock.Volume; // int
	InputLock.RequestID; // int
	InputLock.BusinessUnit;
	InputLock.LockType;
	*/
	// From User basic info.
	strcpy( InputLock.BrokerID,   gBrokerID.c_str() ); // 經紀公司代碼
	strcpy( InputLock.InvestorID, gUserID.c_str() ); // 投資者代碼
	strcpy( InputLock.UserID,     gUserID.c_str() ); // 用戶代碼
	// From UI
	strcpy( InputLock.ExchangeID, gExchangeID.c_str() ); // 交易所代碼
	strcpy( InputLock.InstrumentID, gInstrumentStockID.c_str() ); // 合約代碼
	//
	gCurRequestId++;
	//
	/*
	/////////////////////////////////////////////////////////////////////////
	///TFtdcLockTypeType是一個鎖定方向類型
	/////////////////////////////////////////////////////////////////////////
	///鎖定
	#define THOST_FTDC_LCKT_Lock '1'
	///解鎖
	#define THOST_FTDC_LCKT_Unlock '2'

	typedef char TThostFtdcLockTypeType;
	*/
	//

	//InputLock.Volume = 1;///數量
	if (TxtLockVolume->Text == "")
	{
		MessageBox(Application->Handle, String("LockVolume is NULL !").c_str(), String("LockInsert [Error]").c_str(),
			MB_ICONERROR + MB_OK + MB_TOPMOST);
		TxtLockVolume->SetFocus();
		return;
	}
	InputLock.Volume = TxtLockVolume->Text.ToInt();///數量

	InputLock.RequestID = gCurRequestId;///請求編號
	//InputLock.BusinessUnit;///業務單元

	// Order Sequence
	//iLockRef = gOrderRef; // iLockRef: Keep用
	sprintf( cTemp, "%012d", gOrderRef );
	TradingForm->TxtOrderRef->Text = cTemp;

	if (RadioGroup_Lock->ItemIndex == 0)
	{
		///鎖定
		InputLock.LockType = THOST_FTDC_LCKT_Lock;///鎖定類型
	}
	else
	{
		///解鎖
		InputLock.LockType = THOST_FTDC_LCKT_Unlock;///鎖定類型
	}
	//

	sprintf( InputLock.LockRef, "%012d", gOrderRef );///鎖定引用
	gOrderRef++;

	int Rtn = FKSTRDAPI->ReqLockInsert( &InputLock, gCurRequestId );
	if ( Rtn == 0 )
	{
		TradingForm->AddMemo( "1", L"------------------------------------------------------" );
		TradingForm->AddMemo( "1", L" LockInsert ReqID: " + String( gCurRequestId ) + " ; InstrumentID[" + InputLock.InstrumentID + "]");
	}
	else
	{
		TradingForm->AddMemo( "1", L"------------------------------------------------------" );
		TradingForm->AddMemo( "1", L" LockInsert ErrCode: " + String( Rtn ) + " ; InstrumentID[" + InputLock.InstrumentID + "]" );
	}
}
//---------------------------------------------------------------------------

void __fastcall TTradingForm::Button9Click(TObject *Sender)
{
	// Qry Lock ///用戶端發出查詢鎖定請求

	SetGlobalVariant( );

	KS_::CThostFtdcQryLockField QryLock;

	memset( &QryLock, 0, sizeof(QryLock) );

	/*
	v QryLock.BrokerID;
	v QryLock.InvestorID;
	v QryLock.InstrumentID;
	v QryLock.ExchangeID;
	QryLock.LockSysID;
	QryLock.InsertTimeStart;
	QryLock.InsertTimeEnd;
	*/
	// From User basic info.
	strcpy( QryLock.BrokerID,   gBrokerID.c_str() ); // 經紀公司代碼
	strcpy( QryLock.InvestorID, /*gUserStockID.c_str()*/gUserID.c_str() ); // 投資者代碼
	// From UI
	strcpy( QryLock.ExchangeID, gExchangeID.c_str() ); // 交易所代碼
	strcpy( QryLock.InstrumentID, gInstrumentStockID.c_str() ); // 合約代碼
	//
	if (TxtLockSysID->Text == "")
	{
		MessageBox(Application->Handle, String("LockSysID is NULL !").c_str(), String("Qry Lock [Error]").c_str(),
			MB_ICONERROR + MB_OK + MB_TOPMOST);
		TxtLockSysID->SetFocus();
		return;
	}
	//
	sprintf( QryLock.LockSysID, "%020d", /*TradingForm->TxtOrderRef->Text.ToInt()*/ TradingForm->TxtLockSysID->Text.ToInt() );///鎖定編號
	strcpy( QryLock.InsertTimeStart, "09:35:00" );///開始時間 hh:mm:ss
	strcpy( QryLock.InsertTimeEnd, "14:55:00" );///結束時間 hh:mm:ss
	//
	gCurRequestId++;
	//
	int Rtn = FKSTRDAPI->ReqQryLock( &QryLock, gCurRequestId );
	if ( Rtn == 0 )
	{
		TradingForm->AddMemo( "1", L"------------------------------------------------------" );
		TradingForm->AddMemo( "1", L" QryLock ReqID: " + String( gCurRequestId ) + " ; InstrumentID[" + QryLock.InstrumentID + "]");
	}
	else
	{
		TradingForm->AddMemo( "1", L"------------------------------------------------------" );
		TradingForm->AddMemo( "1", L" QryLock ErrCode: " + String( Rtn ) + " ; InstrumentID[" + QryLock.InstrumentID + "]" );
	}
}
//---------------------------------------------------------------------------

void __fastcall TTradingForm::Button10Click(TObject *Sender)
{
	SetGlobalVariant( );

	// Qry LockPosition ///用戶端發出查詢鎖定證券倉位請求
	KS_::CThostFtdcQryLockPositionField QryLockPosition;

	memset( &QryLockPosition, 0, sizeof(QryLockPosition) );

	// From User basic info.
	strcpy( QryLockPosition.BrokerID,   gBrokerID.c_str() ); // 經紀公司代碼
	strcpy( QryLockPosition.InvestorID, gUserID.c_str() ); // 投資者代碼
	// From UI
	strcpy( QryLockPosition.ExchangeID, gExchangeID.c_str() ); // 交易所代碼
	strcpy( QryLockPosition.InstrumentID, gInstrumentStockID.c_str() ); // 合約代碼

	//
	gCurRequestId++;
	FStrings->Clear();
	int Rtn = FKSTRDAPI->ReqQryLockPosition( &QryLockPosition, gCurRequestId );
	if ( Rtn == 0 )
	{
		TradingForm->AddMemo( "1", L"------------------------------------------------------" );
		TradingForm->AddMemo( "1", L" QryLockPosition ReqID: " + String( gCurRequestId ) + " ; InstrumentID[" + QryLockPosition.InstrumentID + "]");
	}
	else
	{
		TradingForm->AddMemo( "1", L"------------------------------------------------------" );
		TradingForm->AddMemo( "1", L" QryLockPosition ErrCode: " + String( Rtn ) + " ; InstrumentID[" + QryLockPosition.InstrumentID + "]" );
	}
}
//---------------------------------------------------------------------------

void __fastcall TTradingForm::PageControl1Change(TObject *Sender)
{
	switch ( PageControl1->ActivePageIndex )
	{
		case 0:
		case 1:
		case 2:
			{
				Panel_Ref->Parent = PageControl1->ActivePage;
				Panel_Ref->SetBounds(5, 5, Panel_Ref->Width, Panel_Ref->Height);
			}
			break;
	}
}
//---------------------------------------------------------------------------

void __fastcall TTradingForm::FormCreate(TObject *Sender)
{
	m_StockQry_BgColor = (TColor)0x00FBF4E3; // 持倉查詢-背景顏色 //淺藍
	m_StockLock_BgColor = (TColor)0x00D8FAFA; // 鎖定委託-背景顏色 //淺黃
	//
	Label_LockRtn_StockQry_Tit->Transparent = false;
	Label_LockRtn_StockQry_Tit->Color = m_StockQry_BgColor;
	Label_LockRtn_StockQry_Tit->WordWrap = true;
	Label_LockRtn_StockQry_Tit->Alignment = taCenter;
	Label_LockRtn_StockQry_Tit->Layout = tlCenter;
	Label_LockRtn_StockQry_Tit->Font->Color = clBlack;
	//
	Label_LockRtn_StockLock_Tit->Transparent = false;
	Label_LockRtn_StockLock_Tit->Color = m_StockLock_BgColor;
	Label_LockRtn_StockLock_Tit->WordWrap = true;
	Label_LockRtn_StockLock_Tit->Alignment = taCenter;
	Label_LockRtn_StockLock_Tit->Layout = tlCenter;
	Label_LockRtn_StockLock_Tit->Font->Color = clBlack;
	// Initial StringGrid_StockQry
	StringGrid_StockQry->DefaultDrawing = false;
	StringGrid_StockQry->Canvas->Pen->Color = clGray;
	StringGrid_StockQry->Canvas->Pen->Width = 1;
	StringGrid_StockQry->Align = alTop;
	StringGrid_StockQry->BringToFront();
	StringGrid_StockQry->Color = clWhite;
	StringGrid_StockQry->DefaultRowHeight = 20;
	StringGrid_StockQry->ScrollBars = ssVertical;
	StringGrid_StockQry->FixedCols = 0;
	StringGrid_StockQry->FixedRows = 1;
	StringGrid_StockQry->FixedColor = m_StockQry_BgColor;
	StringGrid_StockQry->Font->Color = clBlack;
	StringGrid_StockQry->Font->Size = 10;
	StringGrid_StockQry->GridLineWidth = 1;
	StringGrid_StockQry->Ctl3D = false;
	StringGrid_StockQry->ColCount = 4;
	StringGrid_StockQry->RowCount = StringGrid_StockQry->FixedRows + 1; // FixedRows 要扣掉
	StringGrid_StockQry->ColWidths[0] = 100; // 證券代碼
	StringGrid_StockQry->ColWidths[1] = 100; // 證券名稱
	StringGrid_StockQry->ColWidths[2] = 90; // 可鎖定數量
	StringGrid_StockQry->ColWidths[3] = 90; // 已鎖定數量
	StringGrid_StockQry->Cells[0][0] = L"證券代碼"; // 證券代碼
	StringGrid_StockQry->Cells[1][0] = L"證券名稱"; // 成交價
	StringGrid_StockQry->Cells[2][0] = L"可鎖定數量"; // 可鎖定數量
	StringGrid_StockQry->Cells[3][0] = L"已鎖定數量"; // 已鎖定數量
	StringGrid_StockQry->Options = StringGrid_StockQry->Options >> goColSizing >> goRangeSelect << goRowSelect >> goAlwaysShowEditor; // >>: false, <<: true;
	// Initial StringGrid_StockLock
	StringGrid_StockLock->DefaultDrawing = false;
	StringGrid_StockLock->Canvas->Pen->Color = clGray;
	StringGrid_StockLock->Canvas->Pen->Width = 1;
	StringGrid_StockLock->Align = alClient;
	StringGrid_StockLock->BringToFront();
	StringGrid_StockLock->Color = clWhite;
	StringGrid_StockLock->DefaultRowHeight = 20;
	StringGrid_StockLock->ScrollBars = ssVertical;
	StringGrid_StockLock->FixedCols = 0;
	StringGrid_StockLock->FixedRows = 1;
	StringGrid_StockLock->FixedColor = m_StockLock_BgColor;
	StringGrid_StockLock->Font->Color = clBlack;
	StringGrid_StockLock->Font->Size = 10;
	StringGrid_StockLock->GridLineWidth = 1;
	StringGrid_StockLock->Ctl3D = false;
	StringGrid_StockLock->ColCount = 7;
	StringGrid_StockLock->RowCount = StringGrid_StockLock->FixedRows + 1; // FixedRows 要扣掉
	StringGrid_StockLock->ColWidths[0] = 90; // 委託時間
	StringGrid_StockLock->ColWidths[1] = 90; // 合約代碼
	StringGrid_StockLock->ColWidths[2] = 100; // 申請類別
	StringGrid_StockLock->ColWidths[3] = 70; // 委託數量
	StringGrid_StockLock->ColWidths[4] = 280; // 備註
	StringGrid_StockLock->ColWidths[5] = 70; // 委託編號
	StringGrid_StockLock->ColWidths[6] = 70; // 席位號
	StringGrid_StockLock->Cells[0][0] = L"委託時間"; // 委託時間
	StringGrid_StockLock->Cells[1][0] = L"合約代碼"; // 合約代碼
	StringGrid_StockLock->Cells[2][0] = L"申請類別"; // 申請類別
	StringGrid_StockLock->Cells[3][0] = L"委託數量"; // 委託數量
	StringGrid_StockLock->Cells[4][0] = L"備註"; // 備註
	StringGrid_StockLock->Cells[5][0] = L"委託編號"; // 委託編號
	StringGrid_StockLock->Cells[6][0] = L"席位號"; // 席位號
	StringGrid_StockLock->Options = StringGrid_StockLock->Options >> goColSizing >> goRangeSelect << goRowSelect >> goAlwaysShowEditor; // >>: false, <<: true;
}
//---------------------------------------------------------------------------
	/*
	券=(刀部)用作憑據的票契,發音ㄑㄩㄢˋ
	劵=(力部)身體疲勞,發音ㄐㄩㄢˋ
	*/
void __fastcall TTradingForm::FormShow(TObject *Sender)
{
	// RequestID 型態 8碼
	// 11:37:59+01
	// "11375901" ==> Key一共8碼
	String sDateTime = Now().FormatString("hhmmss");
	gCurRequestId = sDateTime.ToInt()*100;
	//
	Label_LockRtn_StockQry_Tit->Caption = L"備\r\n兌\r\n證\r\n券\r\n持\r\n倉";
	Label_LockRtn_StockLock_Tit->Caption = L"鎖\r\n定\r\n委\r\n託";
	TxtLockVolume->Text = "1000";
	//
	mapStockLock.clear();
	StockLock.DataClear();
}
//---------------------------------------------------------------------------

void __fastcall TTradingForm::StringGrid_StockQryDrawCell(TObject *Sender, int ACol,
		  int ARow, TRect &Rect, TGridDrawState State)
{
	((TStringGrid *)Sender)->Cols[ACol]->BeginUpdate();
	((TStringGrid *)Sender)->Rows[ARow]->BeginUpdate();

	if (ARow  >= 1) // 去除預設Cell的變色
	{
		((TStringGrid *)Sender)->Canvas->Brush->Color = ((TStringGrid *)Sender)->Color; // clWhite;
		((TStringGrid *)Sender)->Canvas->Font->Color = ((TStringGrid *)Sender)->Font->Color; // clBlack;
		((TStringGrid *)Sender)->Canvas->FillRect(((TStringGrid *)Sender)->CellRect(ACol, ARow));
		((TStringGrid *)Sender)->Canvas->TextOut(Rect.Left, Rect.Top, ((TStringGrid *)Sender)->Cells[ACol][ARow]);
	}

	WideString strContent = ((TStringGrid *)Sender)->Cells[ACol][ARow];

	//字的對齊
	int nWidth = ((TStringGrid *)Sender)->Canvas->TextWidth(strContent);
	int nHeight = ((TStringGrid *)Sender)->Canvas->TextHeight(strContent);

	((TStringGrid *)Sender)->Canvas->Font->Size = StringGrid_StockQry->Font->Size;
	((TStringGrid *)Sender)->Canvas->Font->Color = clBlack;

	if (ARow == 0)
	{
		((TStringGrid *)Sender)->Canvas->Brush->Color = m_StockQry_BgColor;

		// Align Center
		((TStringGrid *)Sender)->Canvas->TextRect(
			Rect,
			(Rect.Left+Rect.Right-nWidth)/2,
			(Rect.Top+Rect.Bottom-nHeight)/2,
			strContent
		);
	}
	else
	{
		((TStringGrid *)Sender)->Canvas->Brush->Color = clWhite;

		switch ( ACol)
		{
			case 0:
			case 1:
				// Align Left
				((TStringGrid *)Sender)->Canvas->TextRect(
					Rect,
					Rect.Left+2,
					(Rect.Top+Rect.Bottom-nHeight)/2,
					strContent
				);
				break;

			case 2:
			case 3:
				// Align Right
				((TStringGrid *)Sender)->Canvas->TextRect(
					Rect,
					Rect.Right-nWidth-2,
					(Rect.Top+Rect.Bottom-nHeight)/2,
					strContent
				);
				break;

			default:
				// Align Right
				((TStringGrid *)Sender)->Canvas->TextRect(
					Rect,
					Rect.Right-nWidth-2,
					(Rect.Top+Rect.Bottom-nHeight)/2,
					strContent
				);
				break;
		}
	}

	((TStringGrid *)Sender)->Canvas->Pen->Color = clGray;
	((TStringGrid *)Sender)->Canvas->MoveTo( Rect.Right-1, Rect.Top-1 );
	((TStringGrid *)Sender)->Canvas->LineTo( Rect.Right-1, Rect.Bottom-1 );
	((TStringGrid *)Sender)->Canvas->MoveTo( Rect.Left, Rect.Bottom-1  );
	((TStringGrid *)Sender)->Canvas->LineTo( Rect.Right-1, Rect.Bottom-1  );

	((TStringGrid *)Sender)->Cols[ACol]->EndUpdate();
	((TStringGrid *)Sender)->Rows[ARow]->EndUpdate();
}
//---------------------------------------------------------------------------

void __fastcall TTradingForm::StringGrid_StockLockDrawCell(TObject *Sender, int ACol,
		  int ARow, TRect &Rect, TGridDrawState State)
{
	((TStringGrid *)Sender)->Cols[ACol]->BeginUpdate();
	((TStringGrid *)Sender)->Rows[ARow]->BeginUpdate();

	if (ARow  >= 1) // 去除預設Cell的變色
	{
		((TStringGrid *)Sender)->Canvas->Brush->Color = ((TStringGrid *)Sender)->Color; // clWhite;
		((TStringGrid *)Sender)->Canvas->Font->Color = ((TStringGrid *)Sender)->Font->Color; // clBlack;
		((TStringGrid *)Sender)->Canvas->FillRect(((TStringGrid *)Sender)->CellRect(ACol, ARow));
		((TStringGrid *)Sender)->Canvas->TextOut(Rect.Left, Rect.Top, ((TStringGrid *)Sender)->Cells[ACol][ARow]);
	}

	WideString strContent = ((TStringGrid *)Sender)->Cells[ACol][ARow];

	//字的對齊
	int nWidth = ((TStringGrid *)Sender)->Canvas->TextWidth(strContent);
	int nHeight = ((TStringGrid *)Sender)->Canvas->TextHeight(strContent);

	((TStringGrid *)Sender)->Canvas->Font->Size = StringGrid_StockLock->Font->Size;
	((TStringGrid *)Sender)->Canvas->Font->Color = clBlack;

	if (ARow == 0)
	{
		((TStringGrid *)Sender)->Canvas->Brush->Color = m_StockLock_BgColor;

		// Align Center
		((TStringGrid *)Sender)->Canvas->TextRect(
			Rect,
			(Rect.Left+Rect.Right-nWidth)/2,
			(Rect.Top+Rect.Bottom-nHeight)/2,
			strContent
		);
	}
	else
	{
		((TStringGrid *)Sender)->Canvas->Brush->Color = clWhite;

		switch ( ACol)
		{
			case 0:
			case 1:
			case 4:
			case 5:
			case 6:
				// Align Left
				((TStringGrid *)Sender)->Canvas->TextRect(
					Rect,
					Rect.Left+2,
					(Rect.Top+Rect.Bottom-nHeight)/2,
					strContent
				);
				break;

			case 2:
				// Align Center
				((TStringGrid *)Sender)->Canvas->TextRect(
					Rect,
					(Rect.Left+Rect.Right-nWidth)/2,
					(Rect.Top+Rect.Bottom-nHeight)/2,
					strContent
				);
				break;

			case 3:
				// Align Right
				((TStringGrid *)Sender)->Canvas->TextRect(
					Rect,
					Rect.Right-nWidth-2,
					(Rect.Top+Rect.Bottom-nHeight)/2,
					strContent
				);
				break;

			default:
				// Align Right
				((TStringGrid *)Sender)->Canvas->TextRect(
					Rect,
					Rect.Right-nWidth-2,
					(Rect.Top+Rect.Bottom-nHeight)/2,
					strContent
				);
				break;
		}
	}

	((TStringGrid *)Sender)->Canvas->Pen->Color = clGray;
	((TStringGrid *)Sender)->Canvas->MoveTo( Rect.Right-1, Rect.Top-1 );
	((TStringGrid *)Sender)->Canvas->LineTo( Rect.Right-1, Rect.Bottom-1 );
	((TStringGrid *)Sender)->Canvas->MoveTo( Rect.Left, Rect.Bottom-1  );
	((TStringGrid *)Sender)->Canvas->LineTo( Rect.Right-1, Rect.Bottom-1  );

	((TStringGrid *)Sender)->Cols[ACol]->EndUpdate();
	((TStringGrid *)Sender)->Rows[ARow]->EndUpdate();
}
//---------------------------------------------------------------------------
void TTradingForm::DoStockLock(TStockLock aStockLock)
{
	// RequestID 型態 8碼
	// 11:37:59+01
	// "11375901" ==> Key一共8碼
	String sKey = "";
	sKey.printf(L"%010d", aStockLock.sRequestID.ToInt());
	p_StockLock = mapStockLock.begin();
	p_StockLock = mapStockLock.find(sKey);
	if (p_StockLock == mapStockLock.end())
	{
		mapStockLock.insert(pair<String, TStockLock>(sKey, aStockLock));
	}
	else
	{
		p_StockLock->second = aStockLock;
    }
}
//---------------------------------------------------------------------------
void TTradingForm::StringGrid_DataClear(TStringGrid * Grid, int iCount)
{
	int iFixedRows = Grid->FixedRows;
	int iCols = Grid->ColCount;
	for (int i=iFixedRows; i<Grid->RowCount; i++)
		for (int j=0; j<iCols; j++)
			Grid->Cells[j][i] = ""; // IntToStr(i) + "," + IntToStr(j);

	int iRowCount = iCount;
	if (iRowCount == 0)
		iRowCount = 1;
	Grid->RowCount = iRowCount + iFixedRows;
	//if (!m_bTimer)
	{
		Grid->Refresh();
	}
}
//---------------------------------------------------------------------------
