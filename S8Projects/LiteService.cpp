//---------------------------------------------------------------------------
#include "FMTConfig.h"
#include "OrderStore.h"

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
	FID = ID;
	FAccount = Account;
	FPassword = Password;
	FAccounts.ClearAccounts();
	FAccounts.FName = ID;
	FAccounts.FIDNO = Account;
	Msg = L"";
	return true;
}
//---------------------------------------------------------------------------
bool TLiteService::GetPosition( bool IsTAIFEX, const String& Account, String& Msg )
{
	Msg = L"TLiteService::GetPosition 尚未實作";
	return false;
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

	if(config.GetValue( "Setting","BrokerID", value))
		FBrokerID = value.c_str();
	else
		UFC::BufferedLog::Printf( " 找不到 BrokerID" );

	if(config.GetValue( "Setting","FutAccount", value))
		FFutAccount = value.c_str();
	else
		UFC::BufferedLog::Printf( " 找不到 FutAccount" );

	if(config.GetValue( "Setting","StockAccount", value))
		FStockAccount = value.c_str();
	else
		UFC::BufferedLog::Printf( " 找不到 StockAccount" );

	if(config.GetValue( "Setting","IP", value))
		FIP = value.c_str();
	else
		UFC::BufferedLog::Printf( " 找不到 IP" );

	if(config.GetValue( "Setting","Port", value))
		FPort = value.c_str();
	else
		UFC::BufferedLog::Printf( " 找不到 Port" );
}
