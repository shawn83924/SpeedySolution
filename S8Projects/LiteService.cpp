//---------------------------------------------------------------------------
#include "FMTConfig.h"
#include "OrderStore.h"
#include "Login.h"

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
	LoadConfigSetting("LiteService.ini");
}
//---------------------------------------------------------------------------
bool TLiteService::LoginBroker( const String& ID, const String& Password, String& Msg )
{
	return false;
}
//---------------------------------------------------------------------------
bool TLiteService::LoginBroker(
	const String& ID,
	const String& Account,
	const String& Password,
	String& Msg )
{
	gOrderStore->OnConnect = OrderStoreConnect;
	gOrderStore->OnLogonOK = OrderStoreLogonOK;
	gOrderStore->OnLogonFailed = OrderStoreLogonFailed;

	FID = ID;
	FStockAccount = Account;
	FPassword = Password;

	gOrderStore->ID = FID;
	gOrderStore->TWSEAccount = FStockAccount;
	gOrderStore->Password = FPassword;
	gOrderStore->IP = FIP;
	gOrderStore->Port = FPort;
	gOrderStore->TWSEBrokerID = FStockBrokerID;
	gOrderStore->BrokerID = FFutBrokerID;
	gOrderStore->ClearMemberID = FClearMemberID;
	gOrderStore->Version       = LoginForm->Version;
	gOrderStore->TryVersion    = FTryVersion;
    String LogFilePrefix;
	LogFilePrefix.printf(L"SU_%s", FID);
	gOrderStore->OrderLogFileNamePrefix = LogFilePrefix;

	gOrderStore->Connect();
	const DWORD startTick = GetTickCount();
    const DWORD timeoutMs = 5000;

	while (!FWaitConnectDone)
	{
		// 重要：讓訊息循環跑，callback 才有機會被派送
		Application->ProcessMessages();
		Sleep(10);

		if (GetTickCount() - startTick >= timeoutMs)
			break;
	}

	if (!FWaitConnectDone)
	{
		Msg = L"連線逾時";
		return false;
	}

    Msg = FWaitConnectMsg;
	return FWaitConnectOK;
}
//---------------------------------------------------------------------------
bool TLiteService::GetPosition( bool IsTAIFEX, const String& Account, String& Msg )
{
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
		FTryVersion = (value.c_str() == "0")? false : true;
	else
		UFC::BufferedLog::Printf( " 找不到 TryVersion" );

	if(config.GetValue( "Setting","ProxyLogon", value))
		FProxyLogon = (value.c_str() == "0")? false : true;
	else
		UFC::BufferedLog::Printf( " 找不到 ProxyLogon" );
}
//--------------------------------------------------------------------------
void __fastcall TLiteService::OrderStoreConnect(TObject *Sender)
{
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
	FWaitConnectDone = true;
	FWaitConnectOK = true;
	FWaitConnectMsg = L"登入成功";
}
//--------------------------------------------------------------------------
void __fastcall TLiteService::OrderStoreLogonFailed(
		TObject *Sender,
		const UnicodeString &ReplyMessage,
		int CID)
{
	FWaitConnectDone = true;
	FWaitConnectMsg = L"登入失敗! 失敗原因:"+ ReplyMessage;
}
