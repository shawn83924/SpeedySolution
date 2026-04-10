#include <Registry.hpp>
#include <IniFiles.hpp>
#include <stdio.h>
#include "FMTConfig.h"
#include "TradingObjects.h"
#include "TradingObjectPool.h"
#include "OrderStore.h"
#include <WideStrUtils.hpp>
#include <psapi.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "IdHTTP"
//------------------------------------------------------------------------------
bool   GEnableAllFunc;
extern TCMarketDataStore* gMarketDataStore;
extern bool               GIsTestingMode;
extern bool               GSimMatch;
extern TUnifyUser         gUser;
//---------------------------------------------------------------------------
Config g_Config;
//------------------------------------------------------------------------------
//
//  FMTSetting.ini    Basic setting.(Install path)
//  FMTConfig.ini     Deskup display settings.
//  FMTMD.ini         StarWave Setting, and MDBS web services URLs.
//  FMTBroker.ini     Speedy Setting by Broker.
//
//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
Config::Config()
:m_CancelUpdate( false )
,m_StarWaveIPIndex( -1 )
,m_ChartIPsIndex( -1 )
,m_VIPStarWaveIPIndex( -1 )
,m_VIPChartIPsIndex( -1 )
,m_AutoUpdate( true )
,m_ConfigFile( "FileList.ini" )
,m_ini( NULL )
{
	m_SSLIOHandler       = new TIdSSLIOHandlerSocketOpenSSL( NULL );
	m_StarWaveIPs        = new TStringList();
	m_ChartIPs           = new TStringList();
	m_VIPStarWaveIPs        = new TStringList();
	m_VIPChartIPs           = new TStringList();
	m_SimOrderIPs        = new TStringList();
	m_ChNames            = new TStringList();
	m_ChMaxLots          = new TStringList();
	m_pHTTP              = new TIdHTTP( NULL );
	m_pHTTP->HandleRedirects = true;
	m_pHTTP->OnWork      = Work;
	m_pHTTP->OnWorkEnd   = WorkEnd;
	m_pHTTP->OnWorkBegin = WorkBegin;
	LoadAllIni( );
}
//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
Config::~Config()
{
	delete m_pHTTP;
}
//------------------------------------------------------------------------------
void  Config::LoadStarWaveSettingIni( void )
{
	LoadStarWaveSetting( "FMTMD.ini" );   ///< Load market data serversetting
}
//------------------------------------------------------------------------------
void  Config::LoadAllIni( void )
{
	LoadBasicSetting( "FMTSetting.ini" ); ///< Load basic setting
	LoadStarWaveSetting( "FMTMD.ini" );   ///< Load market data serversetting
	LoadSpeedySetting( "FMTBroker.ini" );
	LoadUpdateSetting( "FMTMD.ini" );
	if( m_ini != NULL )
		delete m_ini;
	m_ini = new UFC::UiniFile( "FMTConfig.ini", true );
	AnsiString InstallDir( m_CurrentDir );

	UFC::BufferedLog::Printf( " [Config] Install dir[%s].", InstallDir.c_str() );
}
//---------------------------------------------------------------------------
bool  Config::Save( void )
{
	AnsiString InstallDir( m_CurrentDir );
	SetCurrentDir( m_CurrentDir );
	UFC::BufferedLog::Printf( " [Config::Save] Save desktop config to[%s].", InstallDir.c_str() );
	return m_ini->Save();
}
//------------------------------------------------------------------------------
void Config::SetFunctionSet( const String& FuncStr )
{
	AnsiString CpyStr = FuncStr;
	UFC::PStringList FuncStrs;

	m_FuncSet.Clear();
	FuncStrs.SetStrings( CpyStr.c_str(), ";" );
	for( int i = 0; i < FuncStrs.ItemCount(); i++ )
	{
		UFC::PStringList FuncPair;

		FuncPair.SetStrings( FuncStrs[i],"|" );
		if( FuncPair.ItemCount( ) == 2 && FuncPair[1] == "8" )
			m_FuncSet.Add( FuncPair[0] );
	}
}
//------------------------------------------------------------------------------
bool Config::IsSupportFunction( const char* FuncStr )
{
	UFC::AnsiString FuncCode( FuncStr );

	if( FuncCode == FUNC_MULTI_ACCOUNT )
	{
		if( m_FuncSet.Exists( FuncCode ) )
			return true;
		return false;
	}
	else if( FuncCode == FUNC_PINGPONG )
		return true;
	if( GEnableAllFunc == true )
		return true;
	return m_FuncSet.Exists( FuncCode );
}
//------------------------------------------------------------------------------
bool Config::IsSupportFunction( const String& FuncStr  )
{
	AnsiString FuncCode( FuncStr );

	return IsSupportFunction( FuncCode.c_str() );
}
//------------------------------------------------------------------------------
void Config::ReleaseResource( void )
{
	delete m_StarWaveIPs;
	delete m_ChartIPs;
	delete m_VIPStarWaveIPs;
	delete m_VIPChartIPs;
}
//------------------------------------------------------------------------------
void Config::GetProcessDir( void )
{
	HANDLE processHandle;
	TCHAR filename[MAX_PATH];

	processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId() );
	if( processHandle != NULL)
	{
		if( GetModuleFileNameEx(processHandle, NULL, filename, MAX_PATH) != 0)
			m_CurrentDir = ExtractFileDir( filename ) + L"\\";
		CloseHandle(processHandle);
	}
}
//------------------------------------------------------------------------------
// Load settings
//------------------------------------------------------------------------------
void Config::LoadBasicSetting( const char* FileName )  ///< Speedy Unify
{
	if( UFC::FileExists( FileName ) == false ) ///< First time running
	{
		GetProcessDir();
		SetCurrentDir( m_CurrentDir );

		AnsiString    ProcessDir( m_CurrentDir );
		UFC::UiniFile NewCfg( FileName, true );

		NewCfg.SetValue( "Setting","InstallPath", ProcessDir.c_str() );
		NewCfg.SetValue( "Setting","IsBackup", "0" );
		NewCfg.SetValue( "Setting","IsInternet", "1" );
		NewCfg.Save();
		m_UseBackup = false;
		m_InternetConfig = 0;
	}
	else
	{
		UFC::UiniFile   Cfg( FileName, true );
		UFC::AnsiString Value;

		if( Cfg.GetValue( "Setting","InstallPath", Value ) ) ///< get the install path.
			m_CurrentDir = Value.c_str();
		else
			m_CurrentDir = ::GetCurrentDir() + "\\";
		SetCurrentDir( m_CurrentDir );
		if( Cfg.GetValue( "Setting","IsBackup", Value) ) ///< Connect to backup servers?
			m_UseBackup = Value.ToInt();
		else
			m_UseBackup = false;
	}
}
//------------------------------------------------------------------------------
void  Config::SplitIPPort( const UFC::AnsiString& Str, UFC::AnsiString& IP, int& Port, int DefPort )
{
	UFC::PStringList Strs;

	Strs.SetStrings( Str, ":" );
	IP = Strs[0];
	if( Strs.ItemCount( ) > 1 )
	{
		Port = Strs[1].ToInt();
		if( Port == 0 )
			Port = DefPort;
	}
	else
		Port = DefPort;
}
//------------------------------------------------------------------------------
void Config::ClearBrokers( void )  ///< Speedy Unify
{
	for( int i = 0; i <  m_Brokers.ItemCount(); i++ )
	{
		delete m_Brokers[i];
		m_Brokers.Clear();
	}
}
//------------------------------------------------------------------------------
void Config::LoadSpeedySetting( const char* FileName )  ///< Speedy Unify
{
	UFC::UiniFile   SettingsFile( FileName, true );

	ClearBrokers();
	for( int i = 0; i  < SettingsFile.SectionCount(); i ++ )
		m_Brokers.Add( new TBrokerConfig( SettingsFile.GetSection(i)) );
    #ifdef _DEBUG
	UFC::Section* capitalSection = new UFC::Section( "群益期貨" );
	capitalSection->SetValue( "BrokerID",  "F020000" );
	capitalSection->SetValue( "TWSEBrokerID",  "2210" );
	capitalSection->SetValue( "CMID",  "F020" );
	capitalSection->SetValue( "Count",  "1" );
	capitalSection->SetValue( "Name1",  "網際網路" );
	capitalSection->SetValue( "IsProxy1", "1" );
	capitalSection->SetValue( "IP1", "192.168.0.97" );
	capitalSection->SetValue( "Port1", "45678" );
	capitalSection->SetValue( "VIPIP1", "192.168.0.97" );
	capitalSection->SetValue( "VIPPort1", "45678" );
	m_Brokers.Add( new TBrokerConfig(capitalSection) );
	#endif
	m_OrderStoreRecoverOverlapSecond = 90;
}
//------------------------------------------------------------------------------
void Config::LoadCHSetting( UFC::UiniFile& SettingsFile, const UFC::AnsiString& Session, const UFC::AnsiString& ValName, const UFC::AnsiString& DefVal, TStringList* mList )
{
	UFC::AnsiString Value, Name;

	if( SettingsFile.GetValue( Session, ValName, Value ) == TRUE )
	{
		mList->Add( Value.c_str() );
		for( int i = 1; ; i++ )
		{
			Name.Printf( "%s%d", ValName.c_str(), i );
			if( SettingsFile.GetValue( Session, Name, Value ) == FALSE )
				break;
			mList->Add( Value.c_str() );
		}
	}
	else
		mList->Add( DefVal.c_str() );
}
//------------------------------------------------------------------------------
void Config::LoadParameter( UFC::UiniFile& SettingsFile )
{
	UFC::AnsiString Value, Name;
	UFC::Section* Params = SettingsFile.GetSection( "Parameter" );

	m_Params.Clear();
	if( Params != NULL  )
	{
		for( int i = 0; i < Params->ItemCount(); i++ )
		{
			Params->GetNameValue( i , Name, Value );
			if( Name.Length() > 0 && Value.Length() > 0 )
				m_Params.Add( Name, new UFC::AnsiString(Value));
		}
	}
}
//------------------------------------------------------------------------------
void Config::GetSignal( int h, int m, UFC::AnsiString& Signal )
{
	int Now = h*100 + m;

	if( Now < m_SignalTime[0] )
	{
		Signal ="00000";
		return;
	}
	for( int i = 0; i < m_SignalTime.ItemCount()-1; i++ )
	{
		int Begin = m_SignalTime[i];
		int End   = m_SignalTime[i+1];

		if( Now >= Begin && Now < End )
		{
			Signal = m_Signal[i];
			return;
		}
	}
	Signal = m_Signal[ m_Signal.ItemCount() -1 ];
}
//------------------------------------------------------------------------------
void Config::LoadSignal( UFC::UiniFile& SettingsFile )
{
	UFC::AnsiString Value, Name;
	UFC::Section* Signal = SettingsFile.GetSection( "Signal" );

	m_Signal.Clear();
	m_SignalTime.Clear();
	if( Signal != NULL  )
	{
		for( int i = 0; i < Signal->ItemCount(); i++ )
		{
			Signal->GetNameValue( i , Name, Value );
			if( Name.Length() > 0 && Value.Length() > 0 )
			{
				m_SignalTime.Add( Name.ToInt() );
				m_Signal.Add( Value );
			}
		}
	}
}
//------------------------------------------------------------------------------
void Config::LoadThroughput( UFC::UiniFile& SettingsFile )
{
	UFC::AnsiString Value, Name;
	UFC::Section* Throughput = SettingsFile.GetSection( "Throughput" );

	m_Throughput.Clear();
	m_ThroughputAccount.Clear();
	if( Throughput != NULL  )
	{
		for( int i = 0; i < Throughput->ItemCount(); i++ )
		{
			Throughput->GetNameValue( i , Name, Value );
			if( Name.Length() > 0 && Value.Length() > 0 )
			{
				m_ThroughputAccount.Add( Name );
				m_Throughput.Add( Value.ToInt() );
			}
		}
	}
}
//------------------------------------------------------------------------------
int Config::GetThroughput( const UFC::AnsiString& Account )
{
	int index = m_ThroughputAccount.IndexOf( Account );
	if( index != -1 )
		return m_Throughput[ index ];
	else
		return 0;
}
//------------------------------------------------------------------------------
int Config::GetParamInt( const UFC::AnsiString& Name )
{
	UFC::AnsiString* Value;
	if( m_Params.GetObjectByKey( Name, Value ) == true )
		return Value->ToInt();
	return 0;
}
//------------------------------------------------------------------------------
double Config::GetParamDouble( const UFC::AnsiString& Name )
{
	UFC::AnsiString* Value;
	if( m_Params.GetObjectByKey( Name, Value ) == true )
		return Value->ToDouble();
	return 0.0;
}
//------------------------------------------------------------------------------
void Config::LoadStarWaveSetting( const char* FileName ) ///< Speedy Unify
{
	UFC::UiniFile   SettingsFile( FileName, true );
	UFC::AnsiString Value, Session, Name, Sect;

	m_ChartIPs->Clear();
	m_StarWaveIPs->Clear();
	m_VIPChartIPs->Clear();
	m_VIPStarWaveIPs->Clear();
	m_SimOrderIPs->Clear();
	m_ChNames->Clear();
	m_ChMaxLots->Clear();
	if( GSimMatch == false )
	{
		if( m_UseBackup == false )
			Session = "Host"; ///< Production
		else
			Session = "BackupHost";
	}
	else
		Session = "SimHost"; ///< Simulator
	///< Load StraWave Server connection information
	LoadCHSetting( SettingsFile, Session, "MD", "60.250.82.135", m_StarWaveIPs );
	///< Load Chart Server connection information
	LoadCHSetting( SettingsFile, Session, "Chart", "60.250.82.135", m_ChartIPs );
	///< Load VIP StraWave Server connection information
	LoadCHSetting( SettingsFile, "VIPServer", "MD", "60.250.82.135", m_VIPStarWaveIPs );
	///< Load VIP　Chart Server connection information
	LoadCHSetting( SettingsFile, "VIPServer", "Chart", "60.250.82.135", m_VIPChartIPs );
	///< Load Simulator Order Server connection information
	LoadCHSetting( SettingsFile, Session, "Order", "60.250.82.135", m_SimOrderIPs );
	///< Load MD/Chart Server connection name.
	LoadCHSetting( SettingsFile, Session, "Name", "正式環境", m_ChNames );
	///< Load Max Lots setting
	LoadCHSetting( SettingsFile, Session, "MaxLots", "10", m_ChMaxLots );
	///<  Load URLs for Speedy Unify web services.
	if( GIsTestingMode == true )
		Sect = "Test";
	else
		Sect = "Prod";
	if( SettingsFile.GetValue( Sect, "RegisterURL", Value) ) ///< New User (Login Page)
		m_RegisterURL = Value.c_str();
	else
		m_RegisterURL = L"http://tomcat85-4.mdevelop.com/Unify/reg.do";
	if( SettingsFile.GetValue( Sect, "ForgetPassword", Value) ) ///< Forget Password (Login Page)
		m_ForgetPassword = Value.c_str();
	else
		m_ForgetPassword = L"http://tomcat85-4.mdevelop.com/Unify/forgetPwd.do";
	if( SettingsFile.GetValue( Sect, "LogonURL", Value) ) ///< Unify login (Login Page)
		m_LogonURL = Value.c_str();
	else
		m_LogonURL = L"https://tomcat85-4.mdevelop.com/Unify/dataTrans.do";
	if( SettingsFile.GetValue( Sect, "EditURL", Value) ) ///< Edit user frofile.( Main form )
		m_EditURL = Value.c_str();
	else
		m_EditURL = L"http://tomcat85-4.mdevelop.com/Unify/memberInfoEdit.do";
	if( SettingsFile.GetValue( Sect, "XFlashURL", Value) ) ///< Subscribe Unify.( Main form )
		m_XFlashURL = Value.c_str();
	else
		m_XFlashURL = L"http://tomcat85-4.mdevelop.com/Unify/makePurchase.do";
	if( SettingsFile.GetValue( Sect, "PurchaseList", Value) ) ///< Purchase List.( Main form )
		m_PurchaseListURL = Value.c_str();
	else
		m_PurchaseListURL = L"https://tomcat85-4.mdevelop.com/AOEApi/orderByMember.do";

	if( SettingsFile.GetValue( Sect, "Report", Value) ) ///< See report Unify.( Main form )
		m_ReportURL = Value.c_str();
	else
		m_ReportURL = L"http://tomcat85-4.mdevelop.com/Unify/calendar/table.do";
	if( SettingsFile.GetValue( Sect, "MegaCAURL", Value) ) ///< Subscribe Unify.( Main form )
		m_MegaCAURL = Value.c_str();
	else
		m_MegaCAURL = L"http://ekeytest.emega.com.tw:8080/VA/StockP1VerifySign.jsp";

	if( SettingsFile.GetValue( Sect, "MegaBackendURL", Value) ) ///< Subscribe Unify.( Main form )
		m_MegaBackendURL = Value.c_str();
	else
		m_MegaBackendURL = L"https://rayin.emega.com.tw/query/indexf.jsp";

	m_LicenseIPSet.Clear();
	for( int i = 1; ; i++ )
	{
		Name.Printf( "IP%d", i );
		if( SettingsFile.GetValue( "License", Name, Value ) == FALSE )
			break;
		m_LicenseIPSet.Add( Value.c_str() );
	}
	m_TFTLicenseIPSet.Clear();
	for( int i = 1; ; i++ )
	{
		Name.Printf( "IP%d", i );
		if( SettingsFile.GetValue( "TFTLicense", Name, Value ) == FALSE )
			break;
		m_TFTLicenseIPSet.Add( Value.c_str() );
	}
	m_VIPSet.Clear();
	for( int i = 1; ; i++ )
	{
		Name.Printf( "VIP%d", i );
		if( SettingsFile.GetValue( "VIPServer", Name, Value ) == FALSE )
			break;
		m_VIPSet.Add( Value.c_str() );
	}
	LoadParameter( SettingsFile );
	LoadSignal( SettingsFile );
	LoadThroughput( SettingsFile );
}
//------------------------------------------------------------------------------
bool Config::IamVIP( const String& ID )
{
	AnsiString AnsiID( ID );

	return m_VIPSet.Exists( AnsiID.c_str() );
}
//------------------------------------------------------------------------------
bool Config::IPLicense( const String& IP )
{
	AnsiString AnsiIP( IP );

	return m_LicenseIPSet.Exists( AnsiIP.c_str() );
}
//------------------------------------------------------------------------------
bool Config::TFTLicense( const String& IP )
{
	AnsiString AnsiIP( IP );

	return m_TFTLicenseIPSet.Exists( AnsiIP.c_str() );
}
//------------------------------------------------------------------------------
void Config::LoadOpenCloseTime( const char* FileName )
{
	UFC::UiniFile   SettingsFile( FileName, true );
	UFC::AnsiString Value;
	int OpenHr   = 5;
	int OpenMin  =50;
	int CloseHr  =5;
	int CloseMin =15;

	if( SettingsFile.GetValue( "Host", "OpenHour", Value ) == TRUE )
		OpenHr = Value.ToInt();
	if( SettingsFile.GetValue( "Host", "OpenMinutes", Value ) == TRUE )
		OpenMin = Value.ToInt();
	if( SettingsFile.GetValue( "Host", "CloseHour", Value ) == TRUE )
		CloseHr = Value.ToInt();
	if( SettingsFile.GetValue( "Host", "CloseMinutes", Value ) == TRUE )
		CloseMin = Value.ToInt();
	UFC::SetTradeingHour( OpenHr, OpenMin, CloseHr, CloseMin );
}
//------------------------------------------------------------------------------
int Config::GetMaxLots(  void  ) ///< Speedy Unify
{
	AnsiString  MaxLotsStr;
	int         Value;

	MaxLotsStr = m_ChMaxLots->Strings[ m_InternetConfig  ];
	Value = MaxLotsStr.ToInt();
	if( Value == 0 )
		return 10;
	return Value;
}
//------------------------------------------------------------------------------
String Config::GetStarWaveIP(  int& Port, int DefPort  ) ///< Speedy Unify
{
	AnsiString       IPPortStr;
	UFC::AnsiString  IP;
	UFC::PStringList IPs;

	IPPortStr = m_StarWaveIPs->Strings[ m_InternetConfig  ];
	IPs.SetStrings( IPPortStr.c_str(), "," );
	if( m_StarWaveIPIndex == -1 )
		m_StarWaveIPIndex = gUser.UserID.ToInt() % IPs.ItemCount();
	m_StarWaveIPIndex = m_StarWaveIPIndex%IPs.ItemCount();
	SplitIPPort( IPs[ m_StarWaveIPIndex ], IP, Port, DefPort );
	m_StarWaveIPIndex++;
	return IP.c_str();
}
//------------------------------------------------------------------------------
String  Config::GetChartServerIP(  int& Port, int DefPort  ) ///< Speedy Unify
{
	AnsiString      IPPortStr;
	UFC::AnsiString IP;
	UFC::PStringList IPs;

	IPPortStr = m_ChartIPs->Strings[ m_InternetConfig ];
	IPs.SetStrings( IPPortStr.c_str(), "," );
	if( m_ChartIPsIndex == -1 )
		m_ChartIPsIndex = gUser.UserID.ToInt() % IPs.ItemCount();
	m_ChartIPsIndex = m_ChartIPsIndex%IPs.ItemCount();
	SplitIPPort( IPs[ m_ChartIPsIndex ], IP, Port, DefPort );
	m_ChartIPsIndex++;
	return IP.c_str();
}
//------------------------------------------------------------------------------
String Config::GetVIPStarWaveIP(  int& Port, int DefPort  ) ///< Speedy Unify
{
	AnsiString       IPPortStr;
	UFC::AnsiString  IP;
	UFC::PStringList IPs;

	IPPortStr = m_VIPStarWaveIPs->Strings[ 0  ];
	IPs.SetStrings( IPPortStr.c_str(), "," );
	if( m_VIPStarWaveIPIndex == -1 )
		m_VIPStarWaveIPIndex = gUser.UserID.ToInt() % IPs.ItemCount();
	m_VIPStarWaveIPIndex = m_VIPStarWaveIPIndex%IPs.ItemCount();
	SplitIPPort( IPs[ m_VIPStarWaveIPIndex ], IP, Port, DefPort );
	m_VIPStarWaveIPIndex++;
	return IP.c_str();
}
//------------------------------------------------------------------------------
String  Config::GetVIPChartServerIP(  int& Port, int DefPort  ) ///< Speedy Unify
{
	AnsiString      IPPortStr;
	UFC::AnsiString IP;
	UFC::PStringList IPs;

	IPPortStr = m_VIPChartIPs->Strings[ 0 ];
	IPs.SetStrings( IPPortStr.c_str(), "," );
	if( m_VIPChartIPsIndex == -1 )
		m_VIPChartIPsIndex = gUser.UserID.ToInt() % IPs.ItemCount();
	m_VIPChartIPsIndex = m_VIPChartIPsIndex%IPs.ItemCount();
	SplitIPPort( IPs[ m_VIPChartIPsIndex ], IP, Port, DefPort );
	m_VIPChartIPsIndex++;
	return IP.c_str();
}
//------------------------------------------------------------------------------
String  Config::GetSimOrderServerIP(  int& Port,  const String& DefIP, int DefPort  ) ///< Speedy Unify
{
	AnsiString      IPPortStr;
	UFC::AnsiString IP;
	UFC::PStringList IPs;
	int IPIndex;

	if( m_SimOrderIPs->Count > m_InternetConfig )
	{
		IPPortStr = m_SimOrderIPs->Strings[ m_InternetConfig ];
		IPs.SetStrings( IPPortStr.c_str(), "," ); ///< IP1:Port1,IP2:Port2,IP3:Port3...
		if( IPs.ItemCount() > 0 )
		{
			IPIndex = m_StarWaveIPIndex%IPs.ItemCount();
			SplitIPPort( IPs[ IPIndex ], IP, Port, DefPort );
			return IP.c_str();
		}
	}
	Port = DefPort;
	return DefIP;
}
//------------------------------------------------------------------------------
bool  Config::SupportExchange( const String& Exchange )
{
	if( Exchange == L"TAIFEX" ||
		Exchange == L"TWSE" ||
		Exchange == L"OTC" )
		return true;
	return false;
}
//---------------------------------------------------------------------------
bool  Config::FuturesExchange( const String& Exchange )
{
	if( Exchange == L"TAIFEX" )
		return true;
	return false;
}
//---------------------------------------------------------------------------
int Config::OrderStoreCount( void )
{
	return m_Users.ItemCount( );
}
//---------------------------------------------------------------------------
const String& Config::GetUserKey( int index )
{
	if( index == -1 )
		index = 0;
	return m_Users[ index ]->UniqueKey;
}
//---------------------------------------------------------------------------
void  Config::SetUserKey( int index, const String& Key )
{
	if( index == -1 )
		index = 0;
	m_Users[ index ]->UniqueKey = Key;
}
//---------------------------------------------------------------------------
String Config::GetLogonID( int index )
{
	if( index == -1 )
		index = 0;
	return m_Users[ index ]->LogonID;
}
//---------------------------------------------------------------------------
void  Config::SetLogonID( int index, const String& ID )
{
	if( index == -1 )
		index = 0;
	m_Users[ index ]->LogonID        = ID;
}
//---------------------------------------------------------------------------
String Config::GetAccount( int index )
{
	if( index == -1 )
		index = 0;
	return m_Users[ index ]->Account;
}
//---------------------------------------------------------------------------
void  Config::SetAccount( int index, const String& Account )
{
	if( index < -1 )
		index = 0;
	m_Users[ index ]->Account = Account;
}
//---------------------------------------------------------------------------
String Config::GetPassword( int index )
{
	if( index == -1 )
		index = 0;
	return m_Users[ index ]->Password;
}
//---------------------------------------------------------------------------
void  Config::SetPassword( int index, const String& Password )
{
	if( index == -1 )
		index = 0;
	m_Users[ index ]->Password = Password;
}
//---------------------------------------------------------------------------
String Config::GetBrokerID( int index )
{
	if( index == -1 )
		index = 0;
	return m_Users[ index ]->BrokerID;
}
//---------------------------------------------------------------------------
void Config::SetBrokerID( int index, const String& Company )
{
	if( index == -1 )
		index = 0;
	String Broker( Company );

	Broker.SetLength( 7 );
	m_Users[ index ]->BrokerID = Broker;
}
//---------------------------------------------------------------------------
String Config::GetCitizenID( int index )
{
	if( index == -1 )
		index = 0;
	return m_Users[ index ]->UserCitizenID;
}
//---------------------------------------------------------------------------
void Config::SetCitizenID( int index, const String& CitizenID )
{
	if( index == -1 )
		index = 0;
	m_Users[ index ]->UserCitizenID = CitizenID;
}
//---------------------------------------------------------------------------
String Config::GetActiveExecutive( int index )
{
	if( index == -1 )
		index = 0;
	return m_Users[ index ]->ActiveExecutive;
}
//---------------------------------------------------------------------------
void Config::SetActiveExecutive( int index, const String& AE )
{
	if( index == -1 )
		index = 0;
	m_Users[ index ]->OrderStore->ActiveExecutive = AE;
	m_Users[ index ]->ActiveExecutive = AE;
}
//---------------------------------------------------------------------------
String Config::GetIB( int index )
{
	if( index == -1 )
		index = 0;
	return m_Users[ index ]->OrderStore->IB;
}
//---------------------------------------------------------------------------
void Config::SetIB( int index, const String& IB )
{
	if( index == -1 )
		index = 0;
	m_Users[ index ]->OrderStore->IB = IB;
}
//---------------------------------------------------------------------------
const String Config::GetName( int index )
{
	if( index == -1 )
		index = 0;
	return m_Users[ index ]->UserName;
}
//---------------------------------------------------------------------------
void Config::SetName( int index, const String& Name )
{
	if( index == -1 )
		index = 0;
	m_Users[ index ]->UserName = Name;
}
//---------------------------------------------------------------------------
TOrderStore*  Config::GetOrderStore( int index )
{
	if( index == -1 )
		index = 0;
	return m_Users[ index ]->OrderStore;
}
//---------------------------------------------------------------------------
int Config::GetOrderStoreIndex( TObject* OrderStore )
{
	for( int i =0; i < m_Users.ItemCount( ); i++ )
	{
		if( m_Users[i]->OrderStore == OrderStore )
			return i;
	}
	return -1;
}
//---------------------------------------------------------------------------
int Config::GetAccountIndex( const String& Account )
{
	for( register int i =0; i < m_Users.ItemCount( ); i++ )
	{
		if( m_Users[i]->Account == Account )
			return i;
	}
	return -1;
}
//---------------------------------------------------------------------------
void Config::GetCurrentDir( char *cBuf )
{
	AnsiString ansiDir = m_CurrentDir;
	strcpy( cBuf, ansiDir.c_str() );
}
//---------------------------------------------------------------------------
TSpeedyConfig*  Config::SpeedyConfig( int BrokerIndex, int Config )
{
	TBrokerConfig* Broker =  BrokerConfig( BrokerIndex );
	if( Broker != NULL)
		return Broker->GetConfig( Config );
	return NULL;
}
//---------------------------------------------------------------------------




