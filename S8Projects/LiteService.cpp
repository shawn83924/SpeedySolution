//---------------------------------------------------------------------------
#include "FMTConfig.h"
#include "OrderStore.h"
#include "Login.h"
#include "PClientSocket.h"
#include "MD5.h"
#include "TradingCommodities.h"
#include "TradingObjectPool.h"
#include "TradingPosition.h"
#include "UDateTime.h"

//---------------------------------------------------------------------------
extern TOrderStore* gOrderStore;
extern TCMarketDataStore* gMarketDataStore;
extern TUnifyUser gUser;
using namespace s888;
const String& LiteExTAIFEX = L"TAIFEX";
//---------------------------------------------------------------------------
void TBrokerUser::ClearAccounts( void )
{
	for( int i = 0; i < FFutAccounts.ItemCount(); ++i )
	{
		TAccountInfo* account = FFutAccounts.GetItem( i );
		delete account;
	}
	FFutAccounts.Clear();

	for( int i = 0; i < FStockAccounts.ItemCount(); ++i )
	{
		TAccountInfo* account = FStockAccounts.GetItem( i );
		delete account;
	}
	FStockAccounts.Clear();
}
//--------------------------------------------------------------------------
TLiteService::TLiteService()
{
}
//---------------------------------------------------------------------------
bool TLiteService::LoginBroker( const String& ID, const String& Password, String& Msg )
{
	LoadConfigSetting("LiteService.ini");
	RegisterOrderStore();

	FID = ID;
	FPassword = Password;

	gOrderStore->ID 			= FID;
	gOrderStore->TWSEAccount 	= FStockAccount;
	gOrderStore->Password 		= FPassword;
	gOrderStore->Account 		= FFutAccount;
	gOrderStore->IP 			= FIP;
	gOrderStore->Port 			= FPort;
	gOrderStore->TWSEBrokerID 	= FStockBrokerID;
	gOrderStore->BrokerID 		= FFutBrokerID;
	gOrderStore->ClearMemberID 	= FClearMemberID;
	gOrderStore->Version       	= LoginForm->Version;
	gOrderStore->TryVersion    	= FTryVersion;
	String LogFilePrefix;
	LogFilePrefix.printf(L"SU_%s", FID);
	gOrderStore->OrderLogFileNamePrefix = LogFilePrefix;
	LogLoginParameters();

    gOrderStore->Connect();

	FStopLogon = false;
	FWaitConnectDone = false;
	FWaitLogonDone = false;

	DWORD startTick = GetTickCount();
	WaitingForResponse(FWaitConnectDone, startTick, 1000);

	if (!FWaitConnectDone)
	{
		FStopLogon = true;
		Msg = L"連線逾時";
		return false;
	}

	startTick = GetTickCount();
	WaitingForResponse(FWaitLogonDone, startTick, 5000);

	if (!FWaitLogonDone)
	{
		FStopLogon = true;
		Msg = FWaitConnectMsg;
		return false;
	}

	Msg = FWaitConnectMsg;
	return FLoginOK;
}
//---------------------------------------------------------------------------
void TLiteService::WaitingForResponse(bool& flag, DWORD& startTick, const DWORD& timeoutMs)
{
	while (!flag)
	{
		MSG Message;
		while( PeekMessage( &Message, NULL, 0, 0, PM_REMOVE ) )
		{
			if( Message.message >= WM_MOUSEFIRST && Message.message <= WM_MOUSELAST )
				continue;
			if( Message.message >= WM_KEYFIRST && Message.message <= WM_KEYLAST )
				continue;
			if( Message.message == WM_SYSCOMMAND || Message.message == WM_CLOSE || Message.message == WM_QUIT )
				continue;
			TranslateMessage( &Message );
			DispatchMessage( &Message );
		}
		UFC::SleepMS( 10 );

		if (GetTickCount() - startTick >= timeoutMs)
			break;
	}
}
//---------------------------------------------------------------------------
void TLiteService::RegisterOrderStore(void)
{
	if(gOrderStore->OnConnect == NULL)
		gOrderStore->OnConnect = OrderStoreConnect;
	if(gOrderStore->OnLogonOK == NULL)
		gOrderStore->OnLogonOK = OrderStoreLogonOK;
	if(gOrderStore->OnLogonFailed == NULL)
		gOrderStore->OnLogonFailed = OrderStoreLogonFailed;
}
//---------------------------------------------------------------------------
void TLiteService::LogLoginParameters(void)
{
	UFC::AnsiString logID( FID.c_str() );
	UFC::AnsiString logFutAccount( FFutAccount.c_str() );
	UFC::AnsiString logStockAccount( FStockAccount.c_str() );
	UFC::AnsiString logIP( FIP.c_str() );
	UFC::AnsiString logFutBrokerID( FFutBrokerID.c_str() );
	UFC::AnsiString logStockBrokerID( FStockBrokerID.c_str() );
	UFC::AnsiString logClearMemberID( FClearMemberID.c_str() );
	UFC::BufferedLog::Printf( " TLiteService::LoginBroker ID[%s] FutAccount[%s] StockAccount[%s] IP[%s:%d] FutBrokerID[%s] StockBrokerID[%s] ClearMemberID[%s] Version[%d] TryVersion[%d] ProxyLogon[%d] PasswordLen[%d]",
		logID.c_str(), logFutAccount.c_str(), logStockAccount.c_str(), logIP.c_str(), FPort,
		logFutBrokerID.c_str(), logStockBrokerID.c_str(), logClearMemberID.c_str(),
		LoginForm->Version, (int)FTryVersion, (int)FProxyLogon, FPassword.Length() );
}
//---------------------------------------------------------------------------
bool TLiteService::GetPosition( bool IsTAIFEX, const String& Account, String& Msg )
{
	UFC::AnsiString ServerIP( FIP.c_str() );	///< 將 VCL String 轉成 UFC::AnsiString

	try
	{
		UFC::PClientSocket PositionSocket( ServerIP, FPositionPort, FALSE );	///< ThreadMode = FALSE，本函式內同步處理即可

		PositionSocket.Connect( 5 );	///< 5 秒連線逾時

		UFC::AnsiString ansiID( FID.c_str() );
		UFC::AnsiString ansiAccount( Account.c_str() );
		UFC::AnsiString rawPassword( FPassword.c_str() );
		UFC::MD5 passwordMD5( (const UFC::UInt8*)rawPassword.c_str(), rawPassword.Length() );
		UFC::AnsiString ansiPassword = passwordMD5.ToString();
		UFC::AnsiString SendContent;	///< ID、Account、Password、IsTAIFEX 用逗號分隔
		SendContent.Printf("%s,%s,%s,%d\n", ansiID.c_str(), ansiAccount.c_str(), ansiPassword.c_str(), (int)IsTAIFEX);

		PositionSocket.SendQueue( std::string( SendContent.c_str(), SendContent.Length() ) );
		PositionSocket.ProcessQueue();	///< SendQueue 只是排隊，要呼叫 ProcessQueue 才會真正送出

		UFC::BufferedLog::Printf( " TLiteService::GetPosition 已送出測試字串到[%s:%d] Content=[%s]", ServerIP.c_str(), FPositionPort, SendContent.c_str() );

		UFC::AnsiString ReplyContent;
		ReceiveSocketReply( PositionSocket, ReplyContent );

		PositionSocket.Disconnect();

		UFC::BufferedLog::Printf( " TLiteService::GetPosition 收到回應內容:[%s]", ReplyContent.c_str() );

		ProcessPositionReply( IsTAIFEX, Account, ReplyContent );
	}
	catch( UFC::SocketException& e )
	{
		UFC::BufferedLog::Printf( " TLiteService::GetPosition 送出測試字串失敗:%s", e.what() );
	}

	return true;
}
//---------------------------------------------------------------------------
bool TLiteService::ReceiveSocketReply( UFC::PClientSocket& Socket, UFC::AnsiString& Reply )
{
	const int RecvTimeoutSec = 5;
	const int BufferSize = 4096;
	const size_t MaxReplySize = 1024*1024;	///< 安全上限，避免對方一直傳資料造成無窮迴圈
	UFC::UInt8 buffer[ BufferSize ];
	std::string accumulated;

	try
	{
		while( accumulated.length() < MaxReplySize )
		{
			int received = Socket.RecvBufferWithTimeout( buffer, BufferSize, RecvTimeoutSec );
			if( received <= 0 )
				break;
			accumulated.append( (char*)buffer, received );
		}
	}
	catch( UFC::SocketException& e )
	{
		///< Server 主動關閉連線，或逾時沒有更多資料，都會在這裡跳出迴圈
		UFC::BufferedLog::DebugPrintf( " TLiteService::ReceiveSocketReply 停止接收:%s", e.what() );
	}

	Reply = UFC::AnsiString( accumulated.c_str(), (UFCType::Int32)accumulated.length() );
	return accumulated.length() > 0;
}
//---------------------------------------------------------------------------
void TLiteService::ProcessPositionReply( bool IsTAIFEX, const String& Account, const UFC::AnsiString& ReplyContent )
{
	if( ReplyContent.Length() <= 0 )
	{
		UFC::BufferedLog::Printf( " TLiteService::ProcessPositionReply 沒有回應內容" );
		return;
	}

	String ReplyStr( ReplyContent.c_str() );
	TJSONValue*  lpJson = TJSONObject::ParseJSONValue( ReplyStr );
	TJSONObject* lpRoot = dynamic_cast<TJSONObject*>( lpJson );

	if( lpRoot == NULL )
	{
		UFC::BufferedLog::Printf( " TLiteService::ProcessPositionReply 回應不是合法JSON格式" );
		delete lpJson;
		return;
	}

	String StatusStr = lpRoot->Values[L"status"]->Value();
	if( StatusStr != L"OK" )	///< 確認資料有效才繼續處理
	{
		UFC::AnsiString ansiStatus( StatusStr.c_str() );
		UFC::BufferedLog::Printf( " TLiteService::ProcessPositionReply status[%s] 不是 OK，不處理", ansiStatus.c_str() );
		delete lpJson;
		return;
	}

	String TypeStr = lpRoot->Values[L"type"]->Value();	///< 依 Server 回傳的 type 決定要走期貨還是證券的解析方式

	TJSONValue* lpRecords = lpRoot->GetValue( L"records" );
	TJSONArray* RecordsArray = dynamic_cast<TJSONArray*>( lpRecords );
	if( RecordsArray != NULL )
	{
		if( TypeStr == L"Futures" )
			AddPosition( Account, RecordsArray );
		else if( TypeStr == L"Securities" )
			AddStockPosition( Account, RecordsArray );
		else
		{
			UFC::AnsiString ansiType( TypeStr.c_str() );
			UFC::BufferedLog::Printf( " TLiteService::ProcessPositionReply 未知的 type[%s]，不處理", ansiType.c_str() );
		}
	}
	delete lpJson;
}
//---------------------------------------------------------------------------
void TLiteService::AddPosition( const String& Account, TJSONArray *PosArray )
{
	s888::CTradingAccount* AccountPtr = s888::gTradingObjsPool.FindAccountFromLogonIdIndex( gUser.LoginUserID, s888::rwLockForRead );
	if( AccountPtr == NULL )
	{
		UFC::AnsiString ansiLoginUserID( gUser.LoginUserID.c_str() );
		UFC::BufferedLog::Printf( " TLiteService::AddPosition LoginUserID[%s] 找不到對應的 CTradingAccount，跳過", ansiLoginUserID.c_str() );
		return;
	}

	for( int i = 0; i < PosArray->Count; i++ )
	{
		TJSONObject *PosItem = dynamic_cast<TJSONObject*>( PosArray->Items[i] );
		if( PosItem == NULL )
			continue;

		String SymbID        = PosItem->Values[L"SymbID"]->Value();
		String DealDate      = PosItem->Values[L"DealDate"]->Value();
		String OrderNo       = PosItem->Values[L"OrderNo"]->Value();
		String BSCode        = PosItem->Values[L"BSCode"]->Value();
		String DealPriceStr  = PosItem->Values[L"DealPrice"]->Value();
		int    DealQty       = PosItem->Values[L"DealQty"]->Value().ToInt();

		if( (DealPriceStr.Length() > 0) && (DealPriceStr[1] == L'+') )	///< VCL String 是 1-based，去掉開頭的 '+'
			DealPriceStr = DealPriceStr.SubString( 2, DealPriceStr.Length() - 1 );
		double MatchPrice = DealPriceStr.ToDouble() / 10000.0;	///< DealPrice 是定點數字串，實際數值要除以 10000 還原

		nsOrderMessageDefine::SideEnum Side = (BSCode == L"S") ? nsOrderMessageDefine::sSell : nsOrderMessageDefine::sBuy;

		UFC::AnsiString AnsiSymbID( SymbID.c_str() );	///< 依照 TAIFEX SymbID 格式解析，所有 SubString/[] 都用 UFC::AnsiString 的 0-based 處理
		nsOrderMessageDefine::MarketEnum Market;
		String ProdID, MaturityYearMonth;
		double StrikePrice = 0.0;
		CallPutCode CP = cpCall;
		bool ParsedOK = false;

		UFC::UDateTime Today;
		int BaseYear = (Today.getYear()/10)*10;
		int ThisYearDigi = Today.getYear()%10;

		if( AnsiSymbID.Length() == 5 )		///< 期貨代碼格式: Root(3) + MonthLetter(1,A-L=1~12月) + YearDigi(1)
		{
			Market = nsOrderMessageDefine::mTWFutures;
			ProdID = AnsiSymbID.SubString( 0, 3 ).c_str();
			char MonthChar = AnsiSymbID[3];
			char YearChar  = AnsiSymbID[4];
			if( (MonthChar >= 'A') && (MonthChar <= 'L') )
			{
				int Month = MonthChar - 'A' + 1;
				int YearDigi = YearChar - '0';
				int Year;
				if( (ThisYearDigi == 9 || ThisYearDigi == 8) && YearDigi == 0 )
					Year = BaseYear + YearDigi + 10;
				else if( (ThisYearDigi == 0) && (YearDigi == 9 || YearDigi == 8) )
					Year = BaseYear + YearDigi - 10;
				else
					Year = BaseYear + YearDigi;
				MaturityYearMonth.printf( L"%04d%02d", Year, Month );
				ParsedOK = true;
			}
		}
		else if( AnsiSymbID.Length() == 10 )	///< 選擇權代碼格式: Root(3) + Strike(5) + MonthCode(1,A-L=Call Jan-Dec,M-X=Put Jan-Dec) + YearDigi(1)
		{
			Market = nsOrderMessageDefine::mTWOptions;
			ProdID = AnsiSymbID.SubString( 0, 3 ).c_str();
			StrikePrice = AnsiSymbID.SubString( 3, 5 ).ToDouble();
			char MonthChar = AnsiSymbID[8];
			char YearChar  = AnsiSymbID[9];
			int Month = 0;
			if( (MonthChar >= 'A') && (MonthChar <= 'L') )
			{
				CP = cpCall;
				Month = MonthChar - 'A' + 1;
			}
			else if( (MonthChar >= 'M') && (MonthChar <= 'X') )
			{
				CP = cpPut;
				Month = MonthChar - 'M' + 1;
			}
			if( Month > 0 )
			{
				int YearDigi = YearChar - '0';
				int Year;
				if( (ThisYearDigi == 9 || ThisYearDigi == 8) && YearDigi == 0 )
					Year = BaseYear + YearDigi + 10;
				else if( (ThisYearDigi == 0) && (YearDigi == 9 || YearDigi == 8) )
					Year = BaseYear + YearDigi - 10;
				else
					Year = BaseYear + YearDigi;
				MaturityYearMonth.printf( L"%04d%02d", Year, Month );
				ParsedOK = true;
			}
		}

		if( !ParsedOK )
		{
			UFC::BufferedLog::Printf( " TLiteService::AddPosition SymbID[%s] 無法解析，跳過", AnsiSymbID.c_str() );
			continue;
		}

		BasicInformation* Info = (Market == nsOrderMessageDefine::mTWOptions) ?
			gMarketDataStore->GetOPTInfo( LiteExTAIFEX, ProdID, MaturityYearMonth, StrikePrice, CP ) :
			gMarketDataStore->GetFUTInfo( LiteExTAIFEX, ProdID, MaturityYearMonth );

		if( (DealQty <= 0) || (Info == NULL) )
		{
			UFC::BufferedLog::Printf( " TLiteService::AddPosition SymbID[%s] ProdID[%s] MaturityYearMonth[%s] 找不到對應商品資訊，跳過",
				AnsiSymbID.c_str(), UFC::AnsiString(ProdID.c_str()).c_str(), UFC::AnsiString(MaturityYearMonth.c_str()).c_str() );
			continue;
		}

		String Symbol = Info->GetSymbol().c_str();

		CHoldPositionData *HoldPositionPtr = new CHoldPositionData( LiteExTAIFEX, Symbol, Market, Side, DealQty, MatchPrice );
		HoldPositionPtr->SetTradeSymbol( Symbol, rwNotLock );
		HoldPositionPtr->SetOrderID( OrderNo, rwNotLock );
		HoldPositionPtr->SetTradeDate( DealDate, rwNotLock );
		AccountPtr->AppendTaifexHoldPositionToArray( HoldPositionPtr, rwLockForWrite );
	}
	ProcessHoldPositionData( AccountPtr );
}
//---------------------------------------------------------------------------
void TLiteService::ProcessHoldPositionData( s888::CTradingAccount* AccountPtr )
{
	int PositionCount = AccountPtr->GetTaifexHoldPositionCount( s888::rwNotLock );
	s888::CHoldPositionData *HoldPositionPtr;

	for( int i = 0; i < PositionCount; i++ )
	{
		HoldPositionPtr = AccountPtr->GetTaifexHoldPositionFromArray( i, s888::rwNotLock );
		HoldPositionPtr->LockForRead();
		String OrderId      = HoldPositionPtr->GetOrderID( s888::rwNotLock );
		String Symbol       = HoldPositionPtr->GetSymbol( s888::rwNotLock );
		int    HoldQuantity = HoldPositionPtr->GetHoldQuantity( s888::rwNotLock );
		double MatchPrice   = HoldPositionPtr->GetMatchPrice( s888::rwNotLock );
		nsOrderMessageDefine::SideEnum   Side   = HoldPositionPtr->GetSide( s888::rwNotLock );
		nsOrderMessageDefine::MarketEnum Market = HoldPositionPtr->GetMarket( s888::rwNotLock );
		HoldPositionPtr->UnlockForRead();

		s888::CPositionStatisticRecord *PositionStatisticRecPtr = gOrderStore->FindOrCreatePosStatisticRec( LiteExTAIFEX, Symbol );
		if( PositionStatisticRecPtr != NULL )
		{
			nsOrderMessageDefine::PositionEffectEnum PositionEffect = nsOrderMessageDefine::peOpen;
			PositionStatisticRecPtr->LockForWrite();
			PositionStatisticRecPtr->AddHoldPosition( OrderId, OrderId, Market, Side, PositionEffect, MatchPrice, HoldQuantity, s888::rwNotLock );
			PositionStatisticRecPtr->ReOffsetPosition( s888::rwNotLock );
			PositionStatisticRecPtr->DumpData( s888::constFlush, s888::rwNotLock );
			PositionStatisticRecPtr->UnlockForWrite();
			gOrderStore->SubscribeSymbol( LiteExTAIFEX, Symbol, true, true );
		}
	}
}
//---------------------------------------------------------------------------
void TLiteService::AddStockPosition( const String& Account, TJSONArray *PosArray )
{
	s888::CTradingAccount* AccountPtr = s888::gTradingObjsPool.FindAccountFromLogonIdIndex( gUser.LoginUserID, s888::rwLockForRead );
	if( AccountPtr == NULL )
	{
		UFC::AnsiString ansiLoginUserID( gUser.LoginUserID.c_str() );
		UFC::BufferedLog::Printf( " TLiteService::AddStockPosition LoginUserID[%s] 找不到對應的 CTradingAccount，跳過", ansiLoginUserID.c_str() );
		return;
	}

	for( int i = 0; i < PosArray->Count; i++ )
	{
		TJSONObject *PosItem = dynamic_cast<TJSONObject*>( PosArray->Items[i] );
		if( PosItem == NULL )
			continue;

		String OTCMarkStr = PosItem->Values[L"OTCMark"]->Value();
		String StockNo    = PosItem->Values[L"StockNo"]->Value();
		int    Quantity   = PosItem->Values[L"UnsoldStockNos"]->Value().ToInt();	///< 庫存股數(股)，不能除以1000換算成張，否則零股(<1000股)會被無條件捨去成 0 而遺漏

		if( Quantity <= 0 )
		{
			UFC::AnsiString ansiStockNo( StockNo.c_str() );
			UFC::BufferedLog::Printf( " TLiteService::AddStockPosition StockNo[%s] 庫存數量為 0，跳過", ansiStockNo.c_str() );
			continue;
		}

		bool IsOTC = (OTCMarkStr == L"1");	///< OTCMark: 0=上市(TWSE) 1=上櫃(OTC)
		String Exchange = IsOTC ? String(L"OTC") : String(L"TWSE");
		nsOrderMessageDefine::MarketEnum Market = IsOTC ? nsOrderMessageDefine::mOTC : nsOrderMessageDefine::mTSE;
		nsOrderMessageDefine::SideEnum   Side   = nsOrderMessageDefine::sBuy;	///< 這個回應格式只回報庫存部位，沒有 Buy/Sell 欄位，一律當多頭庫存
		double MatchPrice = 0.0;	///< 這個回應格式沒有提供成本價欄位，暫時填 0

		BasicInformation* Info = gMarketDataStore->GetBasicInformation( Exchange, StockNo, false );
		if( Info == NULL )
		{
			UFC::AnsiString ansiStockNo( StockNo.c_str() );
			UFC::BufferedLog::Printf( " TLiteService::AddStockPosition StockNo[%s] Exchange[%s] 找不到對應商品資訊，跳過",
				ansiStockNo.c_str(), UFC::AnsiString(Exchange.c_str()).c_str() );
			continue;
		}

		String Symbol = Info->GetSymbol().c_str();

		CHoldPositionData *HoldPositionPtr = new CHoldPositionData( Exchange, Symbol, Market, Side, Quantity, MatchPrice );
		HoldPositionPtr->SetTradeSymbol( Symbol, rwNotLock );
		HoldPositionPtr->SetOrderID( StockNo, rwNotLock );	///< 這個回應格式沒有委託書號，用股票代號當識別
		AccountPtr->AppendTWSEHoldPositionToArray( HoldPositionPtr, rwLockForWrite );
	}
	ProcessStockHoldPositionData( AccountPtr );
}
//---------------------------------------------------------------------------
void TLiteService::ProcessStockHoldPositionData( s888::CTradingAccount* AccountPtr )
{
	int PositionCount = AccountPtr->GetTWSEHoldPositionCount( s888::rwNotLock );
	s888::CHoldPositionData *HoldPositionPtr;

	for( int i = 0; i < PositionCount; i++ )
	{
		HoldPositionPtr = AccountPtr->GetTWSEHoldPositionFromArray( i, s888::rwNotLock );
		HoldPositionPtr->LockForRead();
		String OrderId      = HoldPositionPtr->GetOrderID( s888::rwNotLock );
		String Symbol       = HoldPositionPtr->GetSymbol( s888::rwNotLock );
		String Exchange     = HoldPositionPtr->GetExchangeCode( s888::rwNotLock );
		int    HoldQuantity = HoldPositionPtr->GetHoldQuantity( s888::rwNotLock );
		double MatchPrice   = HoldPositionPtr->GetMatchPrice( s888::rwNotLock );
		nsOrderMessageDefine::SideEnum   Side   = HoldPositionPtr->GetSide( s888::rwNotLock );
		nsOrderMessageDefine::MarketEnum Market = HoldPositionPtr->GetMarket( s888::rwNotLock );
		HoldPositionPtr->UnlockForRead();

		s888::CPositionStatisticRecord *PositionStatisticRecPtr = gOrderStore->FindOrCreatePosStatisticRec( Exchange, Symbol );
		if( PositionStatisticRecPtr != NULL )
		{
			nsOrderMessageDefine::PositionEffectEnum PositionEffect = nsOrderMessageDefine::peOpen;
			PositionStatisticRecPtr->LockForWrite();
			PositionStatisticRecPtr->AddHoldPosition( OrderId, OrderId, Market, Side, PositionEffect, MatchPrice, HoldQuantity, s888::rwNotLock );
			PositionStatisticRecPtr->ReOffsetPosition( s888::rwNotLock );
			PositionStatisticRecPtr->DumpData( s888::constFlush, s888::rwNotLock );
			PositionStatisticRecPtr->UnlockForWrite();
			gOrderStore->SubscribeSymbol( Exchange, Symbol, true, true );
		}
	}
}
//--------------------------------------------------------------------------
void TLiteService::ClearPosition( const String& ID )
{
}
//--------------------------------------------------------------------------
bool TLiteService::SignAgreememt( const String& Account, TCAChecker* CAChecker, String& Msg )
{
	(void)Account;
	(void)CAChecker;
	Msg = L"TLiteService::SignAgreememt 尚未實作";
	return false;
}
//--------------------------------------------------------------------------
TBrokerUser* TLiteService::GetAccount( void )
{
	return &FAccounts;
}
//--------------------------------------------------------------------------
void TLiteService::LoadConfigSetting(const char* FileName)
{
	if( UFC::FileExists( FileName ) == false )
	{
		UFC::BufferedLog::Printf( " 找不到設定檔" );
		return;
	}

	UFC::UiniFile   config( FileName, true );
	UFC::AnsiString value;

	if(config.GetValue( "Setting","FutBrokerID", value))
		FFutBrokerID = value.c_str();
	else
		UFC::BufferedLog::Printf( " 找不到 FutBrokerID" );

	if(config.GetValue( "Setting","FutAccount", value))
		FFutAccount = value.c_str();
	else
		UFC::BufferedLog::Printf( " 找不到 FutAccount" );

    if(config.GetValue( "Setting","StockAccount", value))
		FStockAccount = value.c_str();
	else
		UFC::BufferedLog::Printf( " 找不到 FutAccount" );

	if(config.GetValue( "Setting","StockBrokerID", value))
		FStockBrokerID = value.c_str();
	else
		UFC::BufferedLog::Printf( " 找不到 StockBrokerID" );

	if(config.GetValue( "Setting","IP", value))
		FIP = value.c_str();
	else
		UFC::BufferedLog::Printf( " 找不到 IP" );

	if(config.GetValue( "Setting","Port", value))
		FPort = StrToIntDef(String(value.c_str()), 0);
	else
		UFC::BufferedLog::Printf( " 找不到 Port" );

	if(config.GetValue( "Setting","ClearMemberID", value))
		FClearMemberID = value.c_str();
	else
		UFC::BufferedLog::Printf( " 找不到 ClearMemberID" );

	if(config.GetValue( "Setting","TryVersion", value))
		FTryVersion = (value == "False")? false : true;
	else
		UFC::BufferedLog::Printf( " 找不到 TryVersion" );

	if(config.GetValue( "Setting","ProxyLogon", value))
		FProxyLogon = (value == "False")? false : true;
	else
		UFC::BufferedLog::Printf( " 找不到 ProxyLogon" );

	if(config.GetValue( "Setting","PositionPort", value))
		FPositionPort = StrToIntDef(String(value.c_str()), 7788);
	else
		UFC::BufferedLog::Printf( " 找不到 PositionPort" );
}
//--------------------------------------------------------------------------
void __fastcall TLiteService::OrderStoreConnect(TObject *Sender)
{
	if(FStopLogon)
		return;
	FWaitConnectDone = true;
	if( FProxyLogon == true )
		gOrderStore->LogonProxy();
	else
		gOrderStore->LogonPropTrade();
}
//--------------------------------------------------------------------------
void __fastcall TLiteService::OrderStoreLogonOK(
		TObject *Sender,
		const UnicodeString &ReplyMessage,
		int CID)
{
	if(FStopLogon)
		return;
	FWaitLogonDone = true;
	FLoginOK = true;
	FWaitConnectMsg = L"登入成功";
}
//--------------------------------------------------------------------------
void __fastcall TLiteService::OrderStoreLogonFailed(
		TObject *Sender,
		const UnicodeString &ReplyMessage,
		int CID)
{
	if(FStopLogon)
		return;
	FWaitLogonDone = false;
	FWaitConnectMsg = L"登入失敗! 失敗原因:"+ ReplyMessage;
}
