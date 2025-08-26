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
		FIndex = gUser.UserID.ToInt() % FIPs.ItemCount();
	FIndex = FIndex % FIPs.ItemCount();
	String IP = FIPs[ FIndex ].c_str();
	FIndex++;
	return IP;
}
//---------------------------------------------------------------------------
String TSpeedyConfig::GetVIPIP( void )
{
	if( FIndex == -1 )
		FIndex = gUser.UserID.ToInt() % FVIPIPs.ItemCount();
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
	if( Sect->GetValue( "BrokerID", Value ) == false )
		Value = "F002000";
	FBrokerID = Value.c_str();
	///< TWSE broker ID
	if( Sect->GetValue( "TWSEBrokerID", Value ) == false )
		Value = "7000";
	FTWSEBrokerID = Value.c_str();
	///< Clear member ID
	if( Sect->GetValue( "CMID", Value ) == false )
		FCMID = FBrokerID.SubString( 1, 4 );
	else
		FCMID = Value.c_str();
	if( Sect->GetValue( "ServiceURL", ServiceURL ) == false )
		ServiceURL = "http://210.202.76.87/futuresTreasureApi/";

	if( Sect->GetValue( "Count", Value ) == true )
		Count = Value.ToInt();
	for( int i = 1; i <= Count; i++)
	{
		UFC::AnsiString tag1,tag2,tag3,tag4,tag5,tag6,tag7,tagVIP2,tagVIP3;
		UFC::AnsiString Name,IP,Port,IsProxy,Broker,CM,VIPIP,VIPPort;

		tag1.Printf( "Name%d", i );
		tag2.Printf( "IP%d", i );
		tag3.Printf( "Port%d", i );
		tagVIP2.Printf( "VIPIP%d", i );
		tagVIP3.Printf( "VIPPort%d", i );
		tag4.Printf( "IsProxy%d", i );
		tag5.Printf( "BrokerID%d", i );
		tag6.Printf( "CMID%d", i );
		tag7.Printf( "TWSEBrokerID%d", i );
		if( Sect->GetValue( tag1, Name ) == true &&
			Sect->GetValue( tag2, IP ) == true &&
			Sect->GetValue( tag3, Port ) == true &&
			Sect->GetValue( tag4, IsProxy ) == true )
		{
			TSpeedyConfig* NewConfig = new TSpeedyConfig();
			NewConfig->Name    = Name.c_str();
			///< Set Order Server IP/port.
			NewConfig->SetIPS( IP.c_str() );
			NewConfig->Port    = Port.ToInt();
			///< Set VIP Order Server IP/port.
			if( Sect->GetValue( tagVIP2, VIPIP ) == true )
				NewConfig->SetVIPIPS( VIPIP.c_str() );
			else
				NewConfig->SetIPS( IP.c_str() );
			if( Sect->GetValue( tagVIP3, VIPPort ) == true )
				NewConfig->VIPPort = VIPPort.ToInt();
			else
				NewConfig->VIPPort = Port.ToInt();
            ///< Proxy or Gateway
			NewConfig->IsProxy = IsProxy.ToInt();
			if( Sect->GetValue( tag5, Broker ) == true )
				NewConfig->BrokerID = Broker.c_str();
			else
				NewConfig->BrokerID = FBrokerID;
			if( Sect->GetValue( tag6, CM ) == true )
				NewConfig->CMID = CM.c_str();
			else
				NewConfig->CMID = FCMID;
			if( Sect->GetValue( tag7, Broker ) == true )
				NewConfig->TWSEBrokerID = Broker.c_str();
			else
				NewConfig->TWSEBrokerID = FTWSEBrokerID;
			FConfigs.Add( NewConfig );
		}
	}
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
