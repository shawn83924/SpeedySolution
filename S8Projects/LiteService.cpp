//---------------------------------------------------------------------------
#include "FMTConfig.h"
#include "OrderStore.h"
#include "Login.h"
#include "PClientSocket.h"

//---------------------------------------------------------------------------
extern TOrderStore* gOrderStore;
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
	return true;
	UFC::AnsiString ServerIP( FIP.c_str() );	///< 將 VCL String 轉成 UFC::AnsiString
	const int        TestPort = 7788;

	try
	{
		UFC::PClientSocket TestSocket( ServerIP, TestPort, FALSE );	///< ThreadMode = FALSE，本函式內同步處理即可

		TestSocket.Connect( 5 );	///< 5 秒連線逾時

		UFC::AnsiString SendContent = "GetPosition Test\n";	///< 內容隨意，用 \n 作為訊息分界

		TestSocket.SendQueue( std::string( SendContent.c_str(), SendContent.Length() ) );
		TestSocket.ProcessQueue();	///< SendQueue 只是排隊，要呼叫 ProcessQueue 才會真正送出

		UFC::BufferedLog::Printf( " TLiteService::GetPosition 已送出測試字串到[%s:%d] Content=[%s]", ServerIP.c_str(), TestPort, SendContent.c_str() );

		TestSocket.Disconnect();
	}
	catch( UFC::SocketException& e )
	{
		UFC::BufferedLog::Printf( " TLiteService::GetPosition 送出測試字串失敗:%s", e.what() );
	}

	return true;
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
