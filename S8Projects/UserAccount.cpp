//---------------------------------------------------------------------------
#include "OrderStore.h"
#pragma hdrstop
#include "FMTConfig.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
extern TUnifyUser         gUser;
//---------------------------------------------------------------------------
void TBrokerUser::ClearAccounts( void )
{
	for( int i = 0; i< FStockAccounts.ItemCount();i++)
		 delete FStockAccounts[i];
	for( int i = 0; i< FFutAccounts.ItemCount();i++)
		 delete FFutAccounts[i];
	FStockAccounts.Clear();
	FFutAccounts.Clear();
}
//---------------------------------------------------------------------------
//
//  class TUserAccount
//
//---------------------------------------------------------------------------
__fastcall TUserAccount::TUserAccount( TComponent* Owner )
:TComponent( Owner )
,FOrderStore( new TOrderStore( this ) )
{
}
//---------------------------------------------------------------------------
__fastcall TUserAccount::~TUserAccount( void )
{
	if( FOrderStore->IsLogon() )
		FOrderStore->Logoff();
	delete FOrderStore;
}
//---------------------------------------------------------------------------
String __fastcall TUserAccount::GetUserName( void )
{
	if( FName.IsEmpty( ) )
		return FOrderStore->UserName;
	return FName;
}
//---------------------------------------------------------------------------
void   __fastcall TUserAccount::SetUserName( String NewName )
{
	FOrderStore->UserName = NewName;
	FName = NewName;
}
//---------------------------------------------------------------------------
String __fastcall TUserAccount::GetLogonID( void )
{
	return FOrderStore->ID;
}
//---------------------------------------------------------------------------
void   __fastcall TUserAccount::SetLogonID( String NewID )
{
	FOrderStore->ID = NewID;
}
//---------------------------------------------------------------------------
String __fastcall TUserAccount::GetLogonPassword( void )
{
	return FOrderStore->Password;
}
//---------------------------------------------------------------------------
void   __fastcall TUserAccount::SetLogonPassword( String NewPassword )
{
	FOrderStore->Password = NewPassword;
}
//---------------------------------------------------------------------------
String __fastcall TUserAccount::GetBrokerID( void )
{
	return FOrderStore->BrokerID;
}
//---------------------------------------------------------------------------
void   __fastcall TUserAccount::SetBrokerID( String NewID )
{
	FOrderStore->BrokerID = NewID;
}
//---------------------------------------------------------------------------
String __fastcall TUserAccount::GetAccount( void )
{
	return FOrderStore->Account;
}
//---------------------------------------------------------------------------
void   __fastcall TUserAccount::SetAccount( String Account )
{
   FOrderStore->Account = Account;
}
//---------------------------------------------------------------------------
//
//  class TSpeedyConfig
//
//---------------------------------------------------------------------------
TSpeedyConfig::TSpeedyConfig()
:FIndex( -1 )
{
}
//---------------------------------------------------------------------------
TSpeedyConfig::~TSpeedyConfig()
{

}
//---------------------------------------------------------------------------
void TSpeedyConfig::SetIPS( const String& IPStr )
{
	AnsiString AnsiIPs( IPStr );

	FIPs.Clear();
	FIPs.SetStrings( AnsiIPs.c_str(), "," );
}
//---------------------------------------------------------------------------
void TSpeedyConfig::SetVIPIPS( const String& IPStr )
{
	AnsiString AnsiIPs( IPStr );

	FVIPIPs.Clear();
	FVIPIPs.SetStrings( AnsiIPs.c_str(), "," );
}
//---------------------------------------------------------------------------
String TSpeedyConfig::GetIP( void )
{
	if( FIndex == -1 )
		FIndex = 0;
	FIndex = FIndex % FIPs.ItemCount();
	String IP = FIPs[ FIndex ].c_str();
	FIndex++;
	return IP;
}
//---------------------------------------------------------------------------
String TSpeedyConfig::GetVIPIP( void )
{
	if( FIndex == -1 )
		FIndex = 0;
	FIndex = FIndex % FVIPIPs.ItemCount();
	String IP = FVIPIPs[ FIndex ].c_str();
	FIndex++;
	return IP;
}
//---------------------------------------------------------------------------
//
//  class TBrokerConfig
//
//---------------------------------------------------------------------------
TBrokerConfig::TBrokerConfig( UFC::Section* Sect )
:FService( NULL )
{
	UFC::AnsiString Value,ServiceURL;
	int Count = 0;

	FBrokerName = Sect->GetSectionName().c_str();
	///< TAIFEX broker ID
	if( Sect->GetValue( "BrokerID", Value ) == true )
		UFC::BufferedLog::Printf( "%s: Can not get BrokerID", FBrokerName.c_str() );
	FBrokerID = Value.c_str();
	///< TWSE broker ID
	if( Sect->GetValue( "TWSEBrokerID", Value ) == false )
		UFC::BufferedLog::Printf( "%s: Can not get TWSEBrokerID", FBrokerName.c_str() );
	FTWSEBrokerID = Value.c_str();
	///< Clear member ID
	if( Sect->GetValue( "CMID", Value ) == false )
		FCMID = FBrokerID.SubString( 1, 4 );
	else
		FCMID = Value.c_str();
	if( Sect->GetValue( "ServiceURL", ServiceURL ) == false )
		UFC::BufferedLog::Printf( "%s: Can not get ServiceURL", FBrokerName.c_str() );

	if( Sect->GetValue( "Count", Value ) == true )
		Count = Value.ToInt();
	for( int i = 1; i <= Count; ++i )
	{
		UFC::AnsiString nameTag, ipTag, portTag, isProxyTag, brokerTag, cmTag, twseBrokerTag, vipIpTag, vipPortTag;
		UFC::AnsiString name, ip, port, isProxy, broker, cm, vipIp, vipPort;

		nameTag.Printf( "Name%d", i );
		ipTag.Printf( "IP%d", i );
		portTag.Printf( "Port%d", i );
		vipIpTag.Printf( "VIPIP%d", i );
		vipPortTag.Printf( "VIPPort%d", i );
		isProxyTag.Printf( "IsProxy%d", i );
		brokerTag.Printf( "BrokerID%d", i );
		cmTag.Printf( "CMID%d", i );
		twseBrokerTag.Printf( "TWSEBrokerID%d", i );

		const bool hasRequiredFields =
			Sect->GetValue( nameTag, name ) == true &&
			Sect->GetValue( ipTag, ip ) == true &&
			Sect->GetValue( portTag, port ) == true &&
			Sect->GetValue( isProxyTag, isProxy ) == true;

		if( !hasRequiredFields )
			continue;

		TSpeedyConfig* newConfig = new TSpeedyConfig();
		newConfig->Name = name.c_str();

		///< Set Order Server IP/port.
		newConfig->SetIPS( ip.c_str() );
		newConfig->Port = port.ToInt();

		///< Set VIP Order Server IP/port.
		if( Sect->GetValue( vipIpTag, vipIp ) == true )
			newConfig->SetVIPIPS( vipIp.c_str() );
		else
			newConfig->SetIPS( ip.c_str() ); // keep original behavior

		if( Sect->GetValue( vipPortTag, vipPort ) == true )
			newConfig->VIPPort = vipPort.ToInt();
		else
			newConfig->VIPPort = port.ToInt();

		///< Proxy or Gateway
		newConfig->IsProxy = isProxy.ToInt();

		if( Sect->GetValue( brokerTag, broker ) == true )
			newConfig->BrokerID = broker.c_str();
		else
			newConfig->BrokerID = FBrokerID;

		if( Sect->GetValue( cmTag, cm ) == true )
			newConfig->CMID = cm.c_str();
		else
			newConfig->CMID = FCMID;

		if( Sect->GetValue( twseBrokerTag, broker ) == true )
			newConfig->TWSEBrokerID = broker.c_str();
		else
			newConfig->TWSEBrokerID = FTWSEBrokerID;

		FConfigs.Add( newConfig );
	}
	/*
	if( FBrokerID == L"F002000" ) ///< SinoPac
	{
		FIconIndex = 1;
		FService = NULL;
	}
	else if( FBrokerID == L"F030000" ) ///< Mega Futures
	{
		FIconIndex = 0;
		FService = new TMegaService( ServiceURL.c_str() );
	}
	else
	{
		FIconIndex = 0;
		FService = NULL;
	}
	*/
	FService = new TLiteService();
}
//---------------------------------------------------------------------------
TBrokerConfig::~TBrokerConfig( void )
{
	if(	FService != NULL )
		delete  FService;
}
//---------------------------------------------------------------------------
TSpeedyConfig* TBrokerConfig::GetConfig( int i )
{
	if( i < 0 || i >= FConfigs.ItemCount() )
	{
		if( FConfigs.ItemCount() > 0 )
			return FConfigs.GetItem( 0 );
		else
			return NULL;
	}
	return FConfigs.GetItem( i );
}
//---------------------------------------------------------------------------
