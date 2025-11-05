//---------------------------------------------------------------------------
#define INC_VCLDB_HEADERS
#include <vcl.h>
#pragma hdrstop
#include "Login.h"
#include "SimTFXMain.h"
#include "SearchSpeedyForm.h"
#include "LineEditForm.h"
#include "LinkFrame.h"
#include "SettingForm.h"
#include "NewUser.h"
#include "NewSession.h"
#include "SeqSettingForm.h"
#include "WarningDialog.h"
#include "..\UFC\MD5.h"
#include "..\UFC\iniFile.h"
#include "EventViewer.h"
#include "FormAbout.h"
#include "TagViewer.h"
#include "PrcoessSettingForm.h"
#include "AppInfo.h"
#include "CMDform.h"
#include "NewAccountForm.h"
#include "ProfileGraph.h"
#include "StarWaveFrame.h"
#include "SystemInfoForm.h"
#include "SCStrings.hpp"
#include "PATSFrame.h"
#include "MDIMain.h"
#include "ChangeSessionStatus.h"
#include "SelChannelForm.h"
#include "ExecptionAccount.h"
#include "ExceptionProd.h"
#include "MessageDialog.h"
#include "ThroughputLimit.h"
#include "APIForm.h"
#include <DateUtils.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "MessageObject"
#pragma link "Publisher"
#pragma link "Subscriber"
#pragma link "PerformanceGauge"
#pragma link "GraphMarquee"
#pragma link "SysTrayIcon"
#pragma link "GraphButton"
#pragma link "GraphTrackBar"
#pragma link "GraphPanel"
#include "ItemListForm.h"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
//typedef AnsiStringT<950> BIG5String;
//---------------------------------------------------------------------------
__fastcall TProxyInfo::TProxyInfo(  TSimTFXForm* Own,const UFC::AnsiString& Name )
:TObject()
,FOwner( Own )
,FName( Name )
,FCID( 0 )
,FCIDMAX( 0 )
,FLogon( 0 )
,FLogonMax( 0 )
,FNewOrder( 0 )
,FCancelOrder( 0 )
,FFillOrder( 0 )
,FTabIndex( 0 )
{
}
//---------------------------------------------------------------------------
__fastcall TProxyInfo::~TProxyInfo( )
{
	TStringList* DelItem;

	FHashedUsers.Clear();
	for( int i = 0; i < FUsers.ItemCount(); i ++ )
	{
	   if( (DelItem = FUsers[i]) != NULL )
		   delete DelItem;
	}
	FUsers.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TProxyInfo::UpdateTabIndex( void )
{
	FTabIndex = FOwner->ProxyTabSet->Tabs->IndexOf( FName.c_str() );
}
//---------------------------------------------------------------------------
void __fastcall TProxyInfo::Active( void )
{
   FOwner->CIDStaticText->Caption            = String(FCID);
   FOwner->CIDMaxStaticText->Caption         = String(FCIDMAX);
   FOwner->ConnectionStaticText->Caption     = String(FLogon);
   FOwner->ConnectionMaxStaticText->Caption  = String(FLogonMax);
   FOwner->NewOrdCountStaticText->Caption    = String(FNewOrder);
   FOwner->CancelOrdCountStaticText->Caption = String(FCancelOrder);
   FOwner->FillOrdStaticText->Caption        = String(FFillOrder);
   FOwner->ProxyUsersListView->Items->Count  = FUsers.ItemCount();
}
//---------------------------------------------------------------------------
void __fastcall TProxyInfo::Update( MTree* Tree )
{
   bool            IsActiveTab = false;
   int             UserCount;
   UFC::AnsiString Name,Value;

   if( FOwner->ProxyTabSet->TabIndex == FTabIndex )
		IsActiveTab = true;
   if( Tree->get( "CID", FCID ) && IsActiveTab )
	   FOwner->CIDStaticText->Caption = String(FCID);
   if( Tree->get( "CIDMAX", FCIDMAX )  && IsActiveTab )
	   FOwner->CIDMaxStaticText->Caption = String(FCIDMAX);
   if( Tree->get( "Logon", FLogon )  && IsActiveTab )
	   FOwner->ConnectionStaticText->Caption = String(FLogon);
   if( Tree->get( "Max", FLogonMax )  && IsActiveTab )
	   FOwner->ConnectionMaxStaticText->Caption = String(FLogonMax);
   if( Tree->get( "New", FNewOrder )  && IsActiveTab )
	   FOwner->NewOrdCountStaticText->Caption = String(FNewOrder);
   if( Tree->get( "Cancel", FCancelOrder )  && IsActiveTab )
	   FOwner->CancelOrdCountStaticText->Caption = String(FCancelOrder);
   if( Tree->get( "Fill", FFillOrder ) && IsActiveTab)
	   FOwner->FillOrdStaticText->Caption = String(FFillOrder);
   if( Tree->get( "Count", UserCount ) == true )
   {
	   for( int i = 0;i < UserCount; i++ )
	   {
		   Name.Printf("Data%d", i );
		   if( Tree->get( Name, Value ) == true )
			   UpdateProxyUserStat( Value, IsActiveTab );
	   }
	   FOwner->ProxyUsersListView->Invalidate();
   }
}
//---------------------------------------------------------------------------
// ID, LockState, LogonStat,LogonCount,APIVersion
//---------------------------------------------------------------------------
// ID: Logon Proxy ID
// LockState: O:Locked 1:OK
// LogonStat: O:broken 1:OK
// LogonCount: CID used count.
// APIVersion: API Version string
//---------------------------------------------------------------------------

void __fastcall TProxyInfo::UpdateProxyUserStat( const UFC::AnsiString& Status, bool IsActiveTab )
{
	TStringList* Strs = new TStringList();
	Strs->Delimiter = ',';
	Strs->DelimitedText = Status.c_str();
	if( Strs->Count >= 5 )
	{
		String       ID      = Strs->Strings[0];
		bool         IsLogon = Strs->Strings[2].ToInt();
		AnsiString   AnsiID( ID );
		TStringList* UpdateItem;

		if( FHashedUsers.GetObjectByKey( AnsiID.c_str(), UpdateItem ) == false )
		{
			UpdateItem = new TStringList();
			FHashedUsers.Add( AnsiID.c_str(), UpdateItem );
			FUsers.Add( UpdateItem );
			UpdateItem->Add( Strs->Strings[1] );    ///< Lock stat.Index 0
			UpdateItem->Add( Strs->Strings[3] );    ///< LogonCount.Index 1
			UpdateItem->Add( Strs->Strings[2] );    ///< Logon stat.Index 2
			if( IsLogon )
				UpdateItem->Add( Strs->Strings[4] );///< API Version.Index 3
			else
				UpdateItem->Add( L"----" );
			UpdateItem->Add( ID );                  ///< Account.Index 4
			if( IsActiveTab == true )
				FOwner->ProxyUsersListView->Items->Count  = FUsers.ItemCount();
		}
		else
		{
			UpdateItem->Strings[0] = Strs->Strings[1];// Lock stat
			UpdateItem->Strings[1] = Strs->Strings[3];// LogonCount
			UpdateItem->Strings[2] = Strs->Strings[2];// Logon stat
			if( IsLogon )
				UpdateItem->Strings[3] = Strs->Strings[4];// API Version
			else
				UpdateItem->Strings[3] = L"----";
			if( IsActiveTab == true )
				FOwner->ProxyUsersListView->Update();
		}
	}
	delete Strs;
}
//---------------------------------------------------------------------------
void __fastcall DrawGradientBar( HDC DC , TRect& dRect, TColor UpCol, TColor DownCol )
{
	TRIVERTEX        vert[2] ;
	GRADIENT_RECT    gRect;

	vert [0] .x      = dRect.Left;
	vert [0] .y      = dRect.Top;
	vert [0] .Red    = GetRValue( (DWORD)UpCol)<<8;
	vert [0] .Green  = GetGValue( (DWORD)UpCol)<<8;
	vert [0] .Blue   = GetBValue( (DWORD)UpCol)<<8;
	vert [0] .Alpha  = 0x0000;

	vert [1] .x      = dRect.Right;
	vert [1] .y      = dRect.Bottom;
	vert [1] .Red    = GetRValue( (DWORD)DownCol)<<8;;
	vert [1] .Green  = GetGValue( (DWORD)DownCol)<<8;;
	vert [1] .Blue   = GetBValue( (DWORD)DownCol)<<8;;
	vert [1] .Alpha  = 0x0000;

	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	GradientFill( DC, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V );
}
//---------------------------------------------------------------------------
const AnsiString FIX_NAME            = "imFIX";
const AnsiString SPEEDY_TW_NAME     = "SpeedyTW";
const AnsiString SPEEDY_TFX_NAME     = "SpeedyTAIFEX"; /// Not Use
const AnsiString SPEEDY_FUT_NAME     = "SpeedyFUT";
const AnsiString SPEEDY_OPT_NAME     = "SpeedyOPT";
const AnsiString SPEEDY_STOCK_NAME   = "SpeedyTSEC";
const AnsiString SPEEDY_OTC_NAME     = "SpeedyOTC";
const AnsiString SPEEDY_PATS_NAME    = "SpeedyPATS";
const AnsiString SPEEDY_TT_NAME      = "SpeedyTTGW";
const AnsiString STAR_WAVE_NAME      = "StarWave";
const AnsiString DTS_API_NAME        = "DTSAdapter";
//---------------------------------------------------------------------------
const AnsiString SPEEDY_TOP_NAME     = "SpeedyTop";
const AnsiString SPEEDY_GATEWAY_NAME = "SpeedyGateway";
const AnsiString SPEEDY_PROFILE_NAME = "SpeedyProfile";
const AnsiString SPEEDY_PROXY_NAME   = "SpeedyProxy";
//---------------------------------------------------------------------------
const AnsiString SPEEDY_REPORY_HISTORY_NAME = "ReportHistory";
const char*      SPEEDY_PROCESS_CFG         = "Speedy.cfg";
const char*      SPEEDY_USERS_CFG           = "SpeedyGatewayUsers.cfg";
const char*      SPEEDY_PROFILE_CFG         = "SpeedyProfile.cfg";
const char*      SPEEDY_CLUSTER_CFG         = "SpeedyCluster.cfg";
const char*      SPEEDY_ROUTE_EX_CFG        = "Route/Exchange.ini";
const char*      SPEEDY_ROUTE_CH_CFG        = "Route/Channel.ini";
const char*      SPEEDY_ROUTE_ACC_CFG       = "Route/Account.ini";
const char*      SPEEDY_ROUTE_R1_CFG        = "Route/Rule1.ini";
const char*      SPEEDY_ROUTE_R2_CFG        = "Route/Rule2.ini";
const char*      SPEEDY_ROUTE_R3_CFG        = "Route/Rule3.ini";
const char*      SPEEDY_ROUTE_R4_CFG        = "Route/Rule4.ini";
const char*      SPEEDY_ROUTE_R5_CFG        = "Route/Rule5.ini";
//---------------------------------------------------------------------------
extern AnsiString CurrentDir;
extern bool       gAutoLogon;
//---------------------------------------------------------------------------
TSimTFXForm* SimTFXForm;
AnsiString   TSimTFXForm::FLastUser;
AnsiString   TSimTFXForm::FLastPasswordMD5;
//---------------------------------------------------------------------------
__fastcall TSimTFXForm::TSimTFXForm(TComponent* Owner)
: TForm(Owner),
  FShowPassword( false),
  FSpeedyExists(false),
  FFirstTimeDetect( true ),
  FOptMaxTP(0),
  FFutMaxTP(0),
  FStockMaxTP(0),
  FOTCMaxTP(0),
  FReceiveHBTime(0),
  FIsLogon( false ),
  FLogonResponse( false ),
  FHeartbeatResponse( false ),
  FMonitorFIX( false ),
  FMonitorSession( NULL ),
  FSpeedyRunningState( 0 ),
  FimFIXRunningState( 0 ),
  FFtpPublisher( NULL ),
  FMonitorProcess( 0 ),
  FMonitorOtherProcess( 0 ),
  FFIXRecvTick( 0 ),
  FModifyType( mtNone ),
  FOPTTick( 0 ),
  FFUTTick( 0 ),
  FTSETick( 0 ),
  FOTCTick( 0 ),
  FOPTQueueCount( 0 ),
  FFUTQueueCount( 0 ),
  FTSEQueueCount( 0 ),
  FOTCQueueCount( 0 ),
  FEmomeOrdCount( 0 ),
  FEmomeOrdTick( 0 ),
  FEmomeThroughputMax( 0 ),
  FCPUCount( 0 ),
  FMemCount( 0 ),
  FDiskCount( 0 ),
  FDiffCount( 0 ),
  FDiffMax( 0 ),
  FSpeedyHost( "" ),
  FSpeedyHostIP( "127.0.0.1" ),
  FTPPageVisible( true ),
  AEPageVisible( true ),
  ProcessControl( true ),
  ProcessSetting( true ),
  ConnectionControl( true ),
  ConnectionSetting( true ),
  FIXControl( true ),
  FIXSetting( true ),
  FAutoLogon( false ),
  FServerVersion( 1000 ),
  FOnLogon( NULL ),
  FOnLogoff( NULL ),
  FOnMaxinum( NULL ),
  FOnMDIClosed( NULL ),
  FOnMDIActive( NULL ),
  FOSIndex( 0 ),
  FLoginIDSortDir( 1 )
{
	FLinkPVCToPanel   = new THashedStringList();
	FFIXSessions      = new THashedStringList();
	FDispToAll        = new THashedStringList();
	FSWPanels         = new THashedStringList();
	FProxys           = new THashedStringList();
	FUIUpdateCS       = new TCriticalSection();
	FOptList          = new TStringList();
	FFutList          = new TStringList();
	FStockList        = new TStringList();
	FOTCList          = new TStringList();
	FProcessList      = new TStringList();
	FOtherProcessList = new TStringList();
	FUserList         = new TStringList();
	FUserLogList      = new TStringList();
	FFIXLogList       = new TStringList();
	FPVCLogList       = new TStringList();
	FFIXDicts         = new TStringList();
	FCheckers         = new TStringList();
	FGroupNames       = new TStringList();
	FRuleExchangeList = new TStringList();
	FRuleChannelList  = new TStringList();
	FFIXLogCS         = new TCriticalSection();
	FPVCLogCS         = new TCriticalSection();
	FListBuffer       = new Graphics::TBitmap();
	FGraphPanels      = new TObjectList();

	FLinkPVCToPanel->CaseSensitive = true;
	FFIXSessions->CaseSensitive = true;
	FDispToAll->CaseSensitive = true;
	FSWPanels->CaseSensitive = true;
	FProxys->CaseSensitive = true;

	for( int i = 0; i < MAX_PROCESS; i++ )
	{
		 FProcessState[ i ]         = -1;
		 FProcessStartTime[ i ]     = "22:00"; ///< 6:00 am in Taiwan
		 FProcessStopTime[ i ]      = "21:59"; ///< 5:59 am in Taiwan
		 FProcessStartSec[ i ]      = 22*3600; ///< 6:00 am in Taiwan
		 FProcessStopSec[ i ]       = 21*3600+59*60; ///< 5:59 am in Taiwan
		 FProcessConfig[ i ]        = "";
		 FIsStandby[ i ] = false;
		 FIsChecker[ i ] = false;
		 FIsRemote[ i ] = false;
		 FIsExchange[ i ] = false;
		 FOtherProcessState[ i ]     = -1;
		 FOtherProcessStartTime[ i ] = "22:00"; ///< 6:00 am in Taiwan
		 FOtherProcessStopTime[ i ]  = "21:59"; ///< 5:59 am in Taiwan
	}

	FIXPageControl->Height = 0;
	FIXMsgListBox->Canvas->Font->Name = "Arial";
	FIXMsgListBox->Canvas->Font->Size = 11;

	FListBuffer->Canvas->Font = FIXMsgListBox->Canvas->Font;
	FListBuffer->Canvas->Pen->Color   = clYellow;
	FListBuffer->Canvas->Brush->Color = clBlack;
	FGraphPanels->OwnsObjects = true;
	FSWPanels->OwnsObjects = true;
	FProxys->OwnsObjects = true;
	AnsiString AppUniqueID;

	AppUniqueID.printf( "SpeedyCenter@%s.%d",UFC::Hostname,GetTickCount());
	MessageObject->AppName = AppUniqueID;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FormCreate(TObject *Sender)
{
	ProcessListView->DoubleBuffered     = true;
	OtherProcessListView->DoubleBuffered= true;
	LineListBox->DoubleBuffered         = true;
	FIXSessionsListView->DoubleBuffered = true;
	FIXMsgListBox->DoubleBuffered       = true;
	LogListBox->DoubleBuffered          = true;
	FileListView->DoubleBuffered        = true;
	LogListBox->Color = clBlack;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::LogonDialog()
{
	DWORD InfoSize  = GetFileVersionInfoSizeA( PROCESS_NAME, NULL);
	int   SCVersion = 0;

	if( InfoSize > 0 )
	{
		char *pBuf = new char[ InfoSize ];
		VS_FIXEDFILEINFO *vsinfo;
		UINT Len;

		GetFileVersionInfoA( PROCESS_NAME, 0, InfoSize, pBuf );
		if( VerQueryValueA( pBuf, "\\", (void **)&vsinfo, &Len))
		{
			int Version  = HIWORD(vsinfo->dwFileVersionMS);
			int MVersion = LOWORD(vsinfo->dwFileVersionMS);
			int Build    = LOWORD(vsinfo->dwFileVersionLS);

			SCVersion =  Version*10000 + MVersion*100 + Build;
		}
		delete [] pBuf;
	}
	if( FAutoLogon == true )
	{
		MTree      Data;

		Data.append( "COMMAND", CMD_LOGIN );
		Data.append( "ID",      FLastUser.c_str());
		Data.append( "PWD",     FLastPasswordMD5.c_str() );
		Data.append( "VER",     SCVersion );
		AdminPublisher->SendData( &Data );
		FLogonResponse = false;
		ResponseTimer->Enabled = true;
		FAutoLogon = false;
	}
	else
	{
		LoginForm = new TLoginForm( this );
		if( LoginForm->ShowModal() == mrOk )
		{
			FUser     = LoginForm->IDEdit->Text;
			FPassword = LoginForm->PwdEdit->Text;

			UFC::MD5   md5( (const UFCType::UInt8*) FPassword.c_str(), FPassword.Length() );
			AnsiString PasswdMD5( md5.ToString().c_str() );
			MTree      Data;

			///< Keep last logon ID/Password.
			FLastUser        = FUser;
			FLastPasswordMD5 = PasswdMD5;
			Data.append( "COMMAND", CMD_LOGIN );
			Data.append( "ID",      FUser.c_str());
			Data.append( "PWD",     PasswdMD5.c_str() );
			Data.append( "VER",     SCVersion );
			AdminPublisher->SendData( &Data );
			FLogonResponse = false;
			ResponseTimer->Enabled = true;
		}
		else
		{
			 MBusDisconnect( Scstrings_MAIN_LOGON_FAILED_DISCONNECTED );//"取消登入,通訊模組已斷線" );
		}
		delete LoginForm;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::MessageObjectConnected(TObject *Sender)
{
	if( MessageObject->IsConnected() )
	{
		Caption = Scstrings_MAIN_CPATION;//"Speedy 控制台";
		Screen->Cursor = crDefault;
		StatusBar->Panels->Items[ 0 ]->Text = Scstrings_MAIN_CONNECTED;//"通訊模組已連線";
		PostMessage( Handle, WM_LOGON, 0,0 );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::MessageObjectError(TObject *Sender)
{
	if( gAutoLogon == true )
		gAutoLogon = false;
	Caption = Scstrings_MAIN_CPATION;//"Speedy 控制台";
	Screen->Cursor = crDefault;
	if( FAutoLogon == true && FOnLogonFalied != NULL )
		FOnLogonFalied( this );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::MessageObjectDisconnected(TObject *Sender)
{
	if( gAutoLogon == true )
		gAutoLogon = false;
	Caption = Scstrings_MAIN_CPATION;//"Speedy 控制台";
	Screen->Cursor = crDefault;
	if( FIsLogon )
		LogoutButtonClick( NULL );
	if( FAutoLogon == true && FOnLogonFalied != NULL )
		FOnLogonFalied( this );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::Logon( const AnsiString& IP )
{
	FSpeedyHostIP  = IP;
	FAutoLogon     = false;
	LogonButtonClick( NULL );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::Logon( const AnsiString& SpeedyHost, const AnsiString& IP )
{
	FSpeedyHostIP  = IP;
	FSpeedyHost    = SpeedyHost;
	FUser          = FLastUser;
	FAutoLogon     = true;
	LogonButtonClick( NULL );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::LogonButtonClick(TObject *Sender)
{
	SpeedyAgentPublisher->Key   = UFC::Hostname;
	SpeedyAgentSubscriber->Key  = UFC::Hostname;
	AdminPublisher->Key         = UFC::Hostname;
	AdminSubscriber->Key        = UFC::Hostname;
	MessageObject->Host         = FSpeedyHostIP;
	Screen->Cursor = crHourGlass;
	MessageObject->Connect();
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::LogoutButtonClick(TObject *Sender)
{
	ProcessStateRequestTimer->Enabled = false;
	if( MessageObject->IsConnected())
	{
		MTree Data;

		Data.append( "COMMAND", CMD_LOGOUT );
		Data.append( "ID", FUser.c_str());
		AdminPublisher->SendData( &Data );
	}
	FUser = "";
	FIsLogon = false;
	UserListView->Clear();  ///< clear user page.
	FSpeedyRunningState = 0;
	for( int i = 0; i < FMonitorProcess; i++ ) ///< clear Process status page.
		 FProcessState[ i ] = -1;//PROCESS_STOPED;
	for( int i = 0; i < FMonitorOtherProcess; i++ ) ///< clear Process status page.
		 FOtherProcessState[ i ] = -1;
	ClearAllLines();        ///< clear X.25 line page.
	ClearFIXState( true );  ///< Clear FIX page and Stop session monitoring.
	ClearFIXSessions();     ///< Clear FIX SESSIONS.
	FFIXDicts->Clear();
	AdminPageControl->Visible = false;
	OPPanel->Visible          = false;
	MBusDisconnect( Scstrings_MAIN_LOGOUT );// "登出" );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::MBusDisconnect(  const String& State  )
{
	Caption                     = L"SpeedyCenter";
	FOSIndex                    = 0;
	StatusBar->Panels->Items[0]->Text = State;
	if( FOnLogoff != NULL )
		FOnLogoff( this );
	if( MessageObject->IsConnected())
		PostMessage( Handle, WM_DISCONNECT_MBUS, 0,0 );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::DisconnectMBus( void )
{
	TNotifyEvent OrigDisconnect = MessageObject->OnDisconnected;
	TNotifyEvent OrigOnError    = MessageObject->OnError;

	MessageObject->OnDisconnected = NULL;
	MessageObject->OnError = NULL;
	MessageObject->Disconnect();
	MessageObject->OnDisconnected = OrigDisconnect;
	MessageObject->OnError = OrigOnError;
}
//---------------------------------------------------------------------------
// TWSE TMP Format
// Link,PVC,APCode,BrokerID,Password,PVCID,IP,Local Port,Group,Enabled
//
// TWSE FIX Format
// Link,99,APCode,BrokerID,Password,PVCID,IP,Local Port,Group,Enabled,Throughput
//
// TAIFEX TMP Format:
// Port,SessionID,APCode,BrokerID,Password,CM( CM not required ),URL
// Port,SessionID,APCode,BrokerID,Password,URL,Group,Enabled,OffHour
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::AddToListView( TStringList* SettingList, TMarket Market )
{
	int        PVC,APCode,Link,LocalPort,Group,Throughput;
	AnsiString FCMNo,Pwd,CMorIP,PVCIDorURL,LineKey;
	bool       Enable,OffHour;

	for( int i = 0; i< SettingList->Count; i++)
	{
		TStringList* Strs = new TStringList();

		Strs->Delimiter = ',';
		Strs->DelimitedText = SettingList->Strings[i];
		if( Strs->Count < 6 )
		{
			ShowMessage( "Invalid Session setting:" + SettingList->Strings[i] );
			delete Strs;
			continue;
		}
		Link   = Strs->Strings[ 0 ].ToInt();
		PVC    = Strs->Strings[ 1 ].ToInt();
		if( Strs->Strings[ 2 ] == L"C" )
			APCode = 10;
		else
			APCode = Strs->Strings[ 2 ].ToInt();
		FCMNo  = Strs->Strings[ 3 ];
		Pwd    = Strs->Strings[ 4 ];
		PVCIDorURL  = Strs->Strings[ 5 ];
		Group       = -1;
		OffHour     = false;
		Enable      = true;
		Throughput  = 16;
		if( Market == mtStock || Market == mtOTC ) ///< TSEC
		{
			if( Strs->Count >= 8 )  ///< Is a TCP/IP connection
			{
				CMorIP = Strs->Strings[ 6 ];
				LocalPort = Strs->Strings[ 7 ].ToInt();
				if( Strs->Count >= 9 )
					Group = Strs->Strings[ 8 ].ToInt();
				if( Strs->Count >= 10 )
					Enable = (bool)Strs->Strings[ 9 ].ToInt();
				if( Strs->Count >= 11 )
					Throughput = Strs->Strings[ 10 ].ToInt();
			}
			///<BrokerID,Link,PVCID,Market
			LineKey.printf("%s:%d:%s:%d",FCMNo.c_str(), Link, PVCIDorURL.c_str(),(int)Market );
		}
		else ///< TAIFEX
		{
			if( Strs->Count >= 6 )///< Have CM or URL field.
			{
				CMorIP = "";
				if( Strs->Count >= 7 )
					Group = Strs->Strings[ 6 ].ToInt();
				if( Strs->Count >= 8 )
					Enable = (bool)Strs->Strings[ 7 ].ToInt();
				if( Strs->Count >= 9 )
					OffHour = Strs->Strings[ 8 ].ToInt();
			}
			LineKey.printf("%d:%d:%d:%d", Link, PVC,(int)Market,(int)OffHour );
		}
		if( FLinkPVCToPanel->IndexOf( LineKey ) == -1 ) ///< This PVC line not exists.
		{
			TLineInfo* Line  = new TLineInfo( FCMNo, Pwd, Link, PVC, APCode, Market, Group, CMorIP, LocalPort, PVCIDorURL, Enable, Throughput );
			Line->SetOffHour( OffHour );
			if( BrokerComboBox->Items->IndexOf( FCMNo ) == -1 )
				BrokerComboBox->Items->Add( FCMNo );
			Line->SetState( PVC_STATUS_INIT );
			FLinkPVCToPanel->AddObject( LineKey, Line );
		}
		else
			StatusBar->Panels->Items[2]->Text = "Line Dup:" +LineKey;
		delete Strs;
    }
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::AddSettingToListView()
{
	String LineString;

	SystemComboBox->Clear();
	BrokerComboBox->Clear();
	SystemComboBox->Items->Add( Scstrings_MAIN_ALL );//"全部" );
	BrokerComboBox->Items->Add( Scstrings_MAIN_ALL );//"全部" );
	FUIUpdateCS->Enter();
	if( FOptList->Count > 0 )
	{
		SystemComboBox->Items->AddObject( Scstrings_MAIN_OPTIONS, (TObject*)mtOption );
		AddToListView( FOptList, mtOption );
	}
	if( FFutList->Count > 0 )
	{
		SystemComboBox->Items->AddObject( Scstrings_MAIN_FUTURES, (TObject*)mtFutures );
		AddToListView( FFutList, mtFutures );
	}
	if( FStockList->Count > 0 )
	{
		SystemComboBox->Items->AddObject( Scstrings_MAIN_TSEC, (TObject*)mtStock );
		AddToListView( FStockList, mtStock );
	}
	if( FOTCList->Count > 0 )
	{
		SystemComboBox->Items->AddObject( Scstrings_MAIN_OTC, (TObject*)mtOTC );
		AddToListView( FOTCList, mtOTC );
	}
	FOptList->Clear();
	FFutList->Clear();
	FStockList->Clear();
	FOTCList->Clear();
	FDispToAll->Clear();
	LineComboBox->Items->BeginUpdate();
	LineComboBox->Items->Clear();
	for( int i = 0;i < FLinkPVCToPanel->Count; i++ )
	{
		 TLineInfo* Line  = (TLineInfo*)FLinkPVCToPanel->Objects[ i ];
		 switch( Line->GetSystem())
		 {
			case mtFutures: if( Line->IsTCP( ) == true )
								LineString.printf( Scstrings_MAIN_FUT_TCPIP.c_str(), String(Line->GetBrokerID()), Line->GetPVC() );
							else
								LineString.printf( Scstrings_MAIN_FUT_X25.c_str(), Line->GetLink(), Line->GetPVC(), String(Line->GetBrokerID()) );
							break;
			case mtOption:  if( Line->IsTCP( ) == true )
								LineString.printf( Scstrings_MAIN_OPT_TCPIP.c_str(), String(Line->GetBrokerID()), Line->GetPVC());
							else
								LineString.printf( Scstrings_MAIN_OPT_X25.c_str(), Line->GetLink(), Line->GetPVC(), String(Line->GetBrokerID()) );break;
			case mtStock:   if( Line->IsTCP() )
								LineString.printf( Scstrings_MAIN_TSE_PVC.c_str(), Line->GetLocalPort(), String(Line->GetPVCID().c_str()), String(Line->GetBrokerID().c_str()) );
							else
								LineString.printf( Scstrings_MAIN_TSE_PVC.c_str(), Line->GetLink(), String(Line->GetPVCID().c_str()), String(Line->GetBrokerID().c_str()) );
							break;
			case mtOTC:     if( Line->IsTCP() )
								LineString.printf( Scstrings_MAIN_OTC_PVC.c_str(), Line->GetLocalPort(), String(Line->GetPVCID().c_str()), String(Line->GetBrokerID().c_str()) );
							else
								LineString.printf( Scstrings_MAIN_OTC_PVC.c_str(), Line->GetLink(), String(Line->GetPVCID().c_str()), String(Line->GetBrokerID().c_str()) );
							break;
		 }
		 LineComboBox->Items->Add( LineString );
		 FDispToAll->Add( IntToStr( i ));
	}
	LineComboBox->Items->EndUpdate();
	FUIUpdateCS->Leave();
	LineListBox->Count = FLinkPVCToPanel->Count;
	LineComboBox->ItemIndex = 0;
	SystemComboBox->ItemIndex = 0;
	BrokerComboBox->ItemIndex = 0;
	AppComboBox->ItemIndex = 0;
	StateButtonClick( NULL );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::AddSettings( MTree *Tree )
{
	int     CountOpt;
	int     CountFut;
	int		CountStock;
	int		CountOTC;
	MString SettingStr,Field;

	FUIUpdateCS->Enter();
	FOptList->Clear();
	FFutList->Clear();
	FStockList->Clear();
	FOTCList->Clear();
	if( FMarketSet.Contains( mtOption ) &&  Tree->get( "COUNT_OPT", CountOpt ))
	{
		for( int i = 1; i <= CountOpt; i++ )
		{
			Field.Printf("PVC_OPT%d",i);
			if( Tree->get( Field, SettingStr ) )
				FOptList->Add( SettingStr.c_str());
		}
	}
	if( FMarketSet.Contains( mtFutures ) && Tree->get( "COUNT_FUT", CountFut ))
	{
		for( int i = 1; i <= CountFut; i++ )
		{
			Field.Printf("PVC_FUT%d",i);
			if( Tree->get( Field, SettingStr ) )
				FFutList->Add( SettingStr.c_str());
		}
	}
	if( FMarketSet.Contains( mtStock ) && Tree->get( "COUNT_TSE", CountStock ))
	{
		for( int i = 1; i <= CountStock; i++ )
		{
			Field.Printf("PVC_TSE%d",i);
			if( Tree->get( Field, SettingStr ) )
				FStockList->Add( SettingStr.c_str());
		}
	}
	if( FMarketSet.Contains( mtOTC ) && Tree->get( "COUNT_OTC", CountOTC ) )
	{
		for( int i = 1; i <= CountOTC; i++ )
		{
			Field.Printf("PVC_OTC%d",i);
			if( Tree->get( Field, SettingStr ) )
				FOTCList->Add( SettingStr.c_str());
		}
	}
	FUIUpdateCS->Leave();
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ClearAllLines()
{
	FUIUpdateCS->Enter();
	LineListBox->Count = 0;
	for( int i = FLinkPVCToPanel->Count-1; i >= 0; i-- )
		 delete FLinkPVCToPanel->Objects[ i ];
	FLinkPVCToPanel->Clear();
	FUIUpdateCS->Leave();
}
//---------------------------------------------------------------------------
TConnectionFrame* __fastcall TSimTFXForm::GetConnectionPanel( const UFC::AnsiString& CName )
{
	TConnectionFrame* ConnectionPanel;
	if( FConnectionPanelMap.GetObjectByKey( CName, ConnectionPanel ) == false )
	{
		ConnectionPanel = new TConnectionFrame( ConnectionScrollBox );
		ConnectionPanel->Name = CName.c_str();
		ConnectionPanel->Parent = ConnectionScrollBox;
		ConnectionPanel->TitleStaticText->Caption = CName.c_str();
		ConnectionPanel->Top = 3999;
		FConnectionPanelMap.Add( CName, ConnectionPanel );
	}
	return ConnectionPanel;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::AgentSubscriberMessage(const MString &Subject,
	  const MString &Key, MTree *Tree)
{
	int ReplyVal;

	if( Tree->get( "COMMAND", ReplyVal )  )
	{
		if( ReplyVal == AGENT_REPLY )
		{
			FReceiveHBTime = GetTickCount();
		}
		else if( ReplyVal == AGENT_AE_STATUS_REPORT )
		{
			UpdateAEStatus( Tree );
		}		
		else if( ReplyVal == AGENT_REPLY_STATE )///< Speedy reply PVC status
		{
			int  Index, Link, PVC, State, Market;
			int  IsOffHour = 0;
			UFC::AnsiString BrokerID,PVCID;

			if( Tree->get( "LINK", Link ) &&
				Tree->get( "PVC_STATE", State ) &&
				Tree->get( "SYS", Market ))
			{
				AnsiString LineKey;
				int InSeq = 1;
				int OutSeq = 1;
				int BKIndex = 0;

				if( FHeartbeatResponse == false )
					FHeartbeatResponse = true;
				Tree->get( "OFFHOUR", IsOffHour );
				if(  Tree->get( "PVC", PVC ) )
					LineKey.printf("%d:%d:%d:%d", Link, PVC, Market, IsOffHour );
				else if( Tree->get( "PVCID", PVCID ) && Tree->get( "BROKERID", BrokerID ) )
					LineKey.printf("%s:%d:%s:%d",BrokerID.c_str(), Link, PVCID.c_str(), Market );
				else
					return;
				Tree->get( "IN", InSeq );   ///< FIX Inbound seq for TWSE/OTC
				Tree->get( "OUT", OutSeq ); ///< FIX Outbound seq for TWSE/OTC
				Tree->get( "BAK", BKIndex );///< TAIFEX backup session
				if( (Index = FLinkPVCToPanel->IndexOf( LineKey )) != -1 )
					UpdateLineState( Index, State, InSeq, OutSeq, BKIndex );
			}
		}
		else if( ReplyVal == AGENT_PERFORMANCE )///< Speedy reply performance
		{
			int Market,Count,MsgSec,Time,QueueCount = -1;
			int OHCount = 0;
			int OHQueueCount = 0;
			UFCType::UInt32 SessionStatus = 0xFFFFFFFF;

			if( Tree->get( "TRADE_SYS", Market ) &&
				Tree->get( "COUNT", Count ) &&
				Tree->get( "MSG_SEC", MsgSec ))
			{
				Tree->get( "QUEUE", QueueCount );
				Tree->get( "STATUS", SessionStatus );

				Tree->get( "OHCOUNT", OHCount );
				Tree->get( "OHQUEUE", OHQueueCount );
				switch( Market )
				{
					case 0: UpdateFUTPerformance( MsgSec, Count, QueueCount, OHCount, OHQueueCount, SessionStatus ); break;
					case 1: UpdateOPTPerformance( MsgSec, Count, QueueCount, OHCount, OHQueueCount,SessionStatus ); break;
					case 2: UpdateStockPerformance( MsgSec, Count, QueueCount, SessionStatus ); break;
					case 3: UpdateOTCPerformance( MsgSec, Count, QueueCount, SessionStatus ); break;
				}
			}
		}
		else if( ReplyVal == AGENT_PATS_PERFORMANCE )
		{
			int ECount,OCount,Stat;
			UFC::AnsiString Name;
			UFCType::UInt32 Tick;

			if( Tree->get( "ECount", ECount ) &&
				Tree->get( "OCount", OCount ) &&
				Tree->get( "State", Stat )&&
				Tree->get( "Tick", Tick ) &&
				Tree->get( "Name", Name ))
			{
				TConnectionFrame* ConnectionPanel = GetConnectionPanel( Name );
				if( ConnectionPanel->UpdateInfo( ECount , OCount, Stat, Tick ) == true ) ///< broken
				{
					AnsiString Message;

					Message.printf( "PATS Connection %s broken!", Name.c_str() );
					WarningForm->ShowWarning( FSpeedyHost, Message, FormSetting->GetLineErrorSound() );
					WarningForm->LogError( FSpeedyHost, FSpeedyHostIP,"E12",Message );
				}
			}
		}
		else if( ReplyVal == AGENT_PVC_DATA )
		{
            UFC::AnsiString Data;
			if( PageControl->ActivePage == LogSheet && Tree->get( "IO_DATA", Data ))
			{
				FPVCLogCS->Enter();
				FPVCLogList->Add( Data.c_str() );
				FPVCLogCS->Leave();
			}
        }
		else if( ReplyVal == AGENT_EVENT )
        {
			UFC::AnsiString Data,App,Host;

            if( Tree->get( "MSG", Data ))
            {
                if( Tree->get( "APP", App ) == true && Tree->get( "HOST", Host ) == true && Host.AnsiCompare( FSpeedyHost.c_str()) == 0 )
				{
                    int index = FProcessList->IndexOf( App.c_str() );
                    if( index >= 0 )
                    {
						FIsStandby[ index ] = false;
                        TCustomDrawState DrawState;
						bool             DefaultDraw;
                        TListItem*       Item = ProcessListView->Items->Item[ index ];
                        if( Item != NULL )
                        {
							ProcessListViewCustomDrawSubItem( ProcessListView,Item,1,DrawState,DefaultDraw );
                            ProcessListViewCustomDrawSubItem( ProcessListView,Item,5,DrawState,DefaultDraw );
                            ProcessListViewCustomDrawSubItem( ProcessListView,Item,6,DrawState,DefaultDraw );
                        }
                    }
                }
				AddEventMessage( Data.c_str() );
			}
		}
		else if( ReplyVal == AGENT_KILLSWITCH_LIST )
		{
			AppInfoForm->UpdateKillSwitch( this, Tree );
		}
    }
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::RequestPVCSettingList( void )
{
    MTree Data;

    Data.append( "COMMAND", SPEEDY_GET_LIST );
    Data.append( "HOST", FSpeedyHost.c_str());
	SpeedyAgentPublisher->SendData( &Data );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::RequestFIXSessionList( )
{
	MTree Data;

	Data.append( "COMMAND", SPEEDY_GET_FIX_SETTING );
	Data.append( "HOST", FSpeedyHost.c_str());
    SpeedyAgentPublisher->SendData( &Data );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::RequestFIXSessionsState( void )
{
    MTree Data;

    Data.append( "COMMAND", IMFIX_ASK_SESSIONS_STATE );
    imFIXPublisher->SendData( &Data );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::UpdateProcessState( int State, int mult32 )
{
	register int Mask = 1;
	int UpdateProcess = FMonitorProcess - (32*mult32);

	if( UpdateProcess > 32 )
		UpdateProcess = 32;
	for( register int i = 0; i < UpdateProcess; i++ )
	{
		if( State & Mask )
			ChangeProcessState( i + mult32*32 , PROCESS_RUNNING );
		else
			ChangeProcessState( i + mult32*32 , PROCESS_STOPED );
		Mask <<= 1;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::UpdateOtherProcessState( int State, int mult32 )
{
	register int Mask = 1;
	int UpdateProcess = FMonitorOtherProcess - (32*mult32);

	if( UpdateProcess > 32 )
		UpdateProcess = 32;
	for( register int i = 0; i < UpdateProcess; i++ )
	{
		if( State & Mask )
			ChangeOtherProcessState( i + mult32*32 , PROCESS_RUNNING );
		else
			ChangeOtherProcessState( i + mult32*32 , PROCESS_STOPED );
        Mask <<= 1;
    }
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::AdminLogon( int Days, bool Force )
{
	if(	Force == false && FIsLogon == true )
    	return;
	///< Update all UI states
	IsAdmin = true;
	StatusBar->Panels->Items[0]->Text = Scstrings_MAIN_ADMIN_LOGON_OK;//"管理者登入成功";
	Caption = FUser;
	OPPanel->Visible = false;
	AdminPageControl->ActivePageIndex = 0;
	AdminPageControl->Align = alClient;
	AdminPageControl->Visible    = true;
	BeginDateTimePicker->Date    = TDateTime::CurrentDate() - 31;
	BeginDateTimePicker->MaxDate = TDateTime::CurrentDate();
	EndDateTimePicker->Date      = TDateTime::CurrentDate() - 1;
	EndDateTimePicker->MaxDate   = TDateTime::CurrentDate();
	///< Enable heartbeat timer and set logon flag
	if(	Force == false )
		ProcessStateRequestTimer->Enabled = true;
	FIsLogon = true;
	///< Send the get user list request
	MTree Data;
	Data.append( "COMMAND", CMD_GET_LIST );
	AdminPublisher->SendData( &Data );
	if(	Force == false )
	{
		if( Days > 0 )
		{
			String Msg;
			Msg.printf( Scstrings_MAIN_PASSWD_EXP_WARNING.c_str(), Days );
			WarningForm->ShowWarning( FSpeedyHost, Msg, FormSetting->GetWarningSound() );
			WarningForm->LogError( FSpeedyHost, FSpeedyHostIP,"E09", Msg );
		}
		if( FOnAdminLogon != NULL )
			FOnAdminLogon( this );
		if( FPasswordExp == true )
			ChangePasswdButtonClick( this );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::RequestConfigFile( const char* FileName )
{
	MTree AgentData; ///< Request for the config file.

	AgentData.append( "COMMAND",  SPEEDY_GET_CONFIG_FILE );
	AgentData.append( "FILENAME", FileName );
	AgentData.append( "HOST",     FSpeedyHost.c_str());
	AgentData.append( "ZIP",     1 );
	SpeedyAgentPublisher->SendData( &AgentData );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::RequestUserConfigFile( void )
{
	RequestConfigFile( SPEEDY_USERS_CFG );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::RequestProfileConfigFile( void )
{
	RequestConfigFile( SPEEDY_PROFILE_CFG );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::RequestClusterConfigFile( void )
{
	RequestConfigFile( SPEEDY_CLUSTER_CFG );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::GroupAttrib(  MTree *Tree )
{
	UFCType::Int32 Value;

	if( Tree->get( "FTPPage", Value )) ///< Can see the FTP Page ?
		FTPPageVisible = (bool)Value;
	if( Tree->get( "AEPage", Value ))  ///< Can see the AE Page ?
		AEPageVisible = (bool)Value;
	if( Tree->get( "ProcessControl", Value ))
		ProcessControl = (bool)Value;
	if( Tree->get( "ProcessSetting", Value ) )
		ProcessSetting = (bool)Value;
	if( Tree->get( "ConnectionControl", Value ))
		ConnectionControl = (bool)Value;
	if( Tree->get( "ConnectionSetting", Value ))
		ConnectionSetting = (bool)Value;
	if( Tree->get( "FIXControl", Value ))
		FIXControl = (bool)Value;
	if( Tree->get( "FIXSetting", Value ))
		FIXSetting = (bool)Value;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::GroupNames(  MTree *Tree )
{
	UFC::AnsiString Value,Name;
	FGroupNames->Clear();
	for( int i=0;;i++ )
	{
		Name.Printf("%d", i );
		if( Tree->get( Name, Value ) )
			FGroupNames->Add( Value.c_str() );
		else
			break;
	}
	if( FGroupNames->Count < 3 )
	{
		FGroupNames->Clear();
		FGroupNames->Add( Scstrings_MAIN_ADMIN );//"管理者");
		FGroupNames->Add( Scstrings_MAIN_USER );//"使用者");
		FGroupNames->Add( Scstrings_MAIN_OP );//"監控人員");
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::OPLogon( int Days , int Group, MTree *Tree )
{
	if( FIsLogon == false )
	{
		UFCType::Int32 IPInt;

		if( Tree->get("_IP", IPInt ) == TRUE )
		{
			String CaptionText;
			UFC::AnsiString SelHost, IPAddr = UFC::IntToIP( IPInt );

			GroupNames( Tree );
			GroupAttrib( Tree );
			if( Days > 0 )
			{
				String Msg;
				Msg.printf( Scstrings_MAIN_PASSWD_EXP_WARNING.c_str(), Days );
				WarningForm->ShowWarning( FSpeedyHost, Msg, FormSetting->GetWarningSound() );
				WarningForm->LogError( FSpeedyHost, FSpeedyHostIP,"E09", Msg );
			}
			if( Group < this->FGroupNames->Count )
			{
				String GroupName = FGroupNames->Strings[Group];

				CaptionText.printf( Scstrings_MAIN_LOGON_OK.c_str(), GroupName );
			}
			else
				CaptionText.printf( Scstrings_MAIN_GROUP_LOGON_OK.c_str(), Group );
			StatusBar->Panels->Items[0]->Text = CaptionText;
			Tree->get( "HOST", SelHost );
			SearchForm->WaitForServer( this, SelHost.c_str() );
			return;
		}
	}
	MBusDisconnect( FStatusText );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::LogonOK( const AnsiString& HostName, int Ver )
{
	FServerVersion = Ver;
	FSpeedyHost = "";
	MonitorServer( HostName );
	if( FOnLogon != NULL )
		FOnLogon( this );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::RestoreLogonState( )
{
	if( FIsLogon == true )
	{
		if( IsAdmin == false )
			MonitorServer( FSpeedyHost, true  );
		else
			AdminLogon( 0, true );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::MonitorServer( AnsiString Host, bool Force  )
{
	if( FSpeedyHost != Host || Force == true  )
	{
		AnsiString UniqueKey;

		UniqueKey.printf( "%s@%s_%d", FUser.c_str(), UFC::Hostname, GetTickCount() );
		///< Update all UI states
		IsAdmin = false;
		FSpeedyHost = Host;
		Caption = FSpeedyHost;
		AdminPageControl->Visible = false;
		OPPanel->Visible = true;
		PageControl->ActivePage = ProcessSheet;

		SystemInfoPublisher->Key   = FSpeedyHost;
		SystemInfoSubscriber->Key  = UniqueKey;
		SpeedyAgentSubscriber->Key = UniqueKey;
		SpeedyAgentPublisher->Key  = UniqueKey;
		if( AgentPublisher->Key  != FSpeedyHost )
			AgentPublisher->Key   = FSpeedyHost;
		if( AgentSubscriber->Key != FSpeedyHost )
			AgentSubscriber->Key  = FSpeedyHost;
		if( imFIXPublisher->Key  != FSpeedyHost )
			imFIXPublisher->Key   = FSpeedyHost;
		if( imFIXSubscriber->Key != FSpeedyHost )
			imFIXSubscriber->Key  = FSpeedyHost;
		if( SpeedyFutFtpPublisher->Key  != FSpeedyHost )
			SpeedyFutFtpPublisher->Key   = FSpeedyHost;
		if( SpeedyFutFtpSubscriber->Key != FSpeedyHost )
			SpeedyFutFtpSubscriber->Key  = FSpeedyHost;
		if( SpeedyOptFtpSubscriber->Key != FSpeedyHost )
			SpeedyOptFtpSubscriber->Key  = FSpeedyHost;
		if( SpeedyOptFtpPublisher->Key  != FSpeedyHost )
			SpeedyOptFtpPublisher->Key   = FSpeedyHost;
		if( SpeedyFLEXOPTSubscriber->Key != FSpeedyHost )
			SpeedyFLEXOPTSubscriber->Key  = FSpeedyHost;
		if( SpeedyFLEXFUTSubscriber->Key != FSpeedyHost )
			SpeedyFLEXFUTSubscriber->Key  = FSpeedyHost;

		if( TOPSubscriber->Key != FSpeedyHost )
			TOPSubscriber->Key =  FSpeedyHost;
		if( ProfileSubscriber->Key != FSpeedyHost )
			ProfileSubscriber->Key  = FSpeedyHost;
		if( ProxySubscriber->Key != FSpeedyHost )
			ProxySubscriber->Key = FSpeedyHost;
		if( StarWaveSubscriber->Key != "all" )
			StarWaveSubscriber->Key = "all";

		ServerResourcePanel->Width = 1;
		LogListBox->Clear(); ///< Clear PVC log
		FSpeedyRunningState = 0; ///< Clear Speedy Running state.
		DiskGroupBox->Height = 1;
		CPUAlarmButton->Selected  =!FormSetting->AlarmCPU( );
		MemAlarmButton->Selected  =!FormSetting->AlarmMemory( );
		DiskAlarmButton->Selected  =!FormSetting->AlarmDisk( );
		TimeAlarmButton->Selected =!FormSetting->AlarmTimeDiff( );
		for( int i =  ServersToolBar->ButtonCount -1; i >= 0; i--)
			delete ServersToolBar->Buttons[i];

		TToolButton* ServerBtn = new TToolButton(ServersToolBar);
		ServerBtn->Parent      = ServersToolBar;
		ServerBtn->ImageIndex  = 7;
		ServerBtn->Caption     = FSpeedyHost;
		ServerBtn->Grouped     = true;
		ServerBtn->Down        = true;
		ServerBtn->AllowAllUp  = false;
		ServerBtn->Style       = tbsCheck;
		ServerBtn->OnClick     = ToolButtonClick;
		///< Enable heartbeat timer and set logon flag
		ProcessStateRequestTimer->Enabled = true;
		FIsLogon = true;
		FSWPanels->Clear();
		StopMonitorFIXSession();
		ClearFIXSessions();
		ClearPerformanceGauges();
		ClearProxyPage( );
		MonitorOtherProcess( false );
		///< Send the get process list request
		ReloadMBusProcess();
		FModifyType = mtNone;
		AccountsTabSheet->TabVisible = false;
		if( FPasswordExp == true )
			ChangePasswdButtonClick( this );
		OnChangeToPVCPage();
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::UpdateLineState( int Index, int NewState, int InSeq, int OutSeq, int Backup )
{
	TLineInfo*  Line;

	if( Index >= 0 && Index < FLinkPVCToPanel->Count )
	{
		if( (Line= dynamic_cast<TLineInfo*>(FLinkPVCToPanel->Objects[Index])) != NULL )
		{
			int OldState = Line->GetStateCode();

			if( Line->IsFIX() == true )
			{
				Line->SetInboundSeq( InSeq );
				Line->SetOutboundSeq( OutSeq );
			}
			Line->SetBackupIndex( Backup );
			if( Line->SetState( NewState ) == false )
				OnPVCBroken( Line );
			if( OldState != NewState )
				UpdateItem( Index );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::OnPVCBroken( TLineInfo*  Line  )
{
	String Message;
	int    APCode = Line->GetAPCode();

	if( APCode == 0 || APCode == 4 || APCode == 9  || APCode == 10 )
	{
		if( Line->GetSystem() == mtStock || Line->GetSystem() == mtOTC )
            Message.printf( Scstrings_MAIN_TWSE_ORDER_LINE_BROKEN.c_str(), Line->GetLocalPort(), String( Line->GetPVCID().c_str() ) );
		else
			Message.printf( Scstrings_MAIN_ORDER_LINE_BROKEN.c_str(), Line->GetLink(), Line->GetPVC() );
		WarningForm->ShowWarning( FSpeedyHost, Message, FormSetting->GetLineErrorSound() );
		WarningForm->LogError( FSpeedyHost, FSpeedyHostIP,"E03",Message );
	}
	else if( APCode == 3 )
    {
		Message.printf( Scstrings_MAIN_FILLED_LINE_BROKEN.c_str(), Line->GetLink(), Line->GetPVC() );
		WarningForm->ShowWarning( FSpeedyHost, Message, FormSetting->GetLineErrorSound() );
		WarningForm->LogError( FSpeedyHost, FSpeedyHostIP,"E03",Message );
    }
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ClearPerformanceGauges( void )
{
   StockSpeedLabel->Caption   = " ";
   StockMaxTimeLabel->Caption = " ";
   StockTotalLabel->Caption   = " ";
   PerformanceGaugeSTOCK->Clear();
   OTCSpeedLabel->Caption   = " ";
   OTCMaxTimeLabel->Caption = " ";
   OTCTotalLabel->Caption   = " ";
   PerformanceGaugeOTC->Clear();
   SpeedLabel->Caption   = " ";
   MaxTimeLabel->Caption = " ";
   TotalLabel->Caption   = " ";
   OffHourTotalLabel->Caption   = " ";
   PerformanceGaugeOPT->Clear();
   FSpeedLabel->Caption   = " ";
   FMaxTimeLabel->Caption = " ";
   FTotalLabel->Caption   = " ";
   FOffHourTotalLabel->Caption   = " ";
   PerformanceGaugeFUT->Clear();
   CPUPerformanceGauge->Clear();
   MEMPerformanceGauge->Clear();
   DiskPerformanceGauge->Clear();
   FStockMaxTP = 0;
   FOTCMaxTP   = 0;
   FOptMaxTP   = 0;
   FFutMaxTP   = 0;
   FOPTTick = 0;
   FFUTTick = 0;
   FTSETick = 0;
   FOTCTick = 0;
   FLastAlarmCID = 0;
   FProxyStatEmpty = true;
   FCIDMax = 0;
   FCurrentCID = 0;
   FProxyConnectionMax = 0;
   FProxyConnectionCount = 0;
   for(	int i = 0;i < FConnectionPanelMap.ItemCount(); i ++ )
   {
		TConnectionFrame* DelFrame;
		if( FConnectionPanelMap.GetItem( i, DelFrame ) == true )
			delete DelFrame;
   }
	FConnectionPanelMap.Clear();
}
//---------------------------------------------------------------------------
String __fastcall TSimTFXForm::GetStatusString( UFCType::UInt32 Status )
{
	switch( Status )
	{
		case 0:  return Scstrings_MAIN_SESSION_BEFORE_OPEN;//L"盤前";
		case 1:  return Scstrings_MAIN_SESSION_TRY_OPEN;   //L"試單";
		case 2:  return Scstrings_MAIN_SESSION_MKT_OPENED; //L"盤中";
		default: return Scstrings_MAIN_SESSION_MKT_CLOSED; //L"收盤";
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::SetSessionStatusString( bool IsTSE, UFCType::UInt32 Status )
{
	if( Status == 0xFFFFFFFF )
		return;
	UFCType::UInt32 Normal = (Status&0x000000FF);
	UFCType::UInt32 Odd    = (Status&0x0000FF00)>>8;
	UFCType::UInt32 Post   = (Status&0x00FF0000)>>16;

	if( IsTSE == true )
	{
	   TSENormalText->Caption = GetStatusString(Normal);
	   TSEOddText->Caption    = GetStatusString(Odd);
	   TSEPostText->Caption   = GetStatusString(Post);
	}
	else
	{
	   OTCNormalText->Caption = GetStatusString(Normal);
	   OTCOddText->Caption    = GetStatusString(Odd);
	   OTCPostText->Caption   = GetStatusString(Post);
    }
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::UpdateStockPerformance( int OrderSec, int Total, int qcount, UFCType::UInt32 Status )
{
   int 	   CheckInterval;
   String  MsgSecStr;

   MsgSecStr.printf( Scstrings_MAIN_ORDER_PER_SEC.c_str(),((float)OrderSec)/100.0 );
   StockSpeedLabel->Caption   = MsgSecStr;
   if( OrderSec > FStockMaxTP )
   {
	   FStockMaxTP = OrderSec;
	   StockMaxTimeLabel->Caption = MsgSecStr;
   }
   if( qcount >= 0 )
   {
	   if( qcount >= FormSetting->GetQueueLimit() && FTSEQueueCount < qcount )
	   {
			MsgSecStr.printf( Scstrings_MAIN_QUEUE_WARNING.c_str(),Scstrings_MAIN_TSEC, qcount );
			WarningForm->ShowWarning( FSpeedyHost, MsgSecStr, FormSetting->GetWarningSound() );
			WarningForm->LogError( FSpeedyHost, FSpeedyHostIP,"E02",MsgSecStr );
	   }
	   FTSEQueueCount = qcount;
	   MsgSecStr.printf( Scstrings_MAIN_ORDER_QUEUE_COUNT.c_str(),Total,qcount );
   }
   else
	   MsgSecStr.printf( Scstrings_MAIN_ORDER_COUNT.c_str(),Total );
   StockTotalLabel->Caption = MsgSecStr;
   PerformanceGaugeSTOCK->Add( OrderSec );
   SetSessionStatusString( true ,Status );
   if( FormSetting->IsTSECheckingTime( CheckInterval ) && FTSETick >= CheckInterval && TSEAlarmButton->Selected == false )
   {
		if( PerformanceGaugeSTOCK->DataExists( CheckInterval ) == false )
			PlaySoundA( FormSetting->GetNoTradeSound().c_str(), NULL, SND_ASYNC|SND_FILENAME );
		FTSETick = 0;
   }
   FTSETick++;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::UpdateOTCPerformance( int OrderSec, int Total, int qcount, UFCType::UInt32 Status )
{
   int    CheckInterval;
   String MsgSecStr;

   MsgSecStr.printf( Scstrings_MAIN_ORDER_PER_SEC.c_str(),((float)OrderSec)/100.0 );
   OTCSpeedLabel->Caption   = MsgSecStr;
   if( OrderSec > FOTCMaxTP )
   {
	   FOTCMaxTP = OrderSec;
	   OTCMaxTimeLabel->Caption = MsgSecStr;
   }
   if( qcount >= 0 )
   {
	   if( qcount >= FormSetting->GetQueueLimit() && FOTCQueueCount < qcount )
	   {
			MsgSecStr.printf( Scstrings_MAIN_QUEUE_WARNING.c_str(),Scstrings_MAIN_OTC, qcount );
			WarningForm->ShowWarning( FSpeedyHost, MsgSecStr, FormSetting->GetWarningSound() );
			WarningForm->LogError( FSpeedyHost, FSpeedyHostIP,"E02",MsgSecStr );
	   }
	   FOTCQueueCount = qcount;
	   MsgSecStr.printf( Scstrings_MAIN_ORDER_QUEUE_COUNT.c_str(),Total,qcount );
   }
   else
	   MsgSecStr.printf( Scstrings_MAIN_ORDER_COUNT.c_str(),Total );
   OTCTotalLabel->Caption = MsgSecStr;
   PerformanceGaugeOTC->Add( OrderSec );
   SetSessionStatusString( false,Status );
   if( FormSetting->IsOTCCheckingTime( CheckInterval ) && FOTCTick >= CheckInterval && OTCAlarmButton->Selected == false )
   {
		if( PerformanceGaugeOTC->DataExists( CheckInterval ) == false )
			PlaySoundA( FormSetting->GetNoTradeSound().c_str(), NULL, SND_ASYNC|SND_FILENAME );
		FOTCTick = 0;
   }
   FOTCTick++;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::UpdateOPTPerformance( int OrderSec, int Total, int qcount, int OHTotal, int OHqcount, UFCType::UInt32 Status )
{
   int    CheckInterval;
   String MsgSecStr;

   MsgSecStr.printf( Scstrings_MAIN_ORDER_PER_SEC.c_str(),((float)OrderSec)/100.0 );
   SpeedLabel->Caption   = MsgSecStr;
   if( OrderSec > FOptMaxTP )
   {
	   FOptMaxTP = OrderSec;
	   MaxTimeLabel->Caption = MsgSecStr;
   }
   if( qcount >= 0 )
   {
	   if( qcount >= FormSetting->GetQueueLimit() && FOPTQueueCount < qcount )
	   {
			MsgSecStr.printf( Scstrings_MAIN_QUEUE_WARNING.c_str(),Scstrings_MAIN_OPTIONS, qcount );
			WarningForm->ShowWarning( FSpeedyHost, MsgSecStr, FormSetting->GetWarningSound() );
			WarningForm->LogError( FSpeedyHost, FSpeedyHostIP,"E02",MsgSecStr );
	   }
	   FOPTQueueCount = qcount;
	   MsgSecStr.printf( Scstrings_MAIN_ORDER_QUEUE_COUNT.c_str(),Total-OHTotal,qcount );
   }
   else
	   MsgSecStr.printf( Scstrings_MAIN_ORDER_COUNT.c_str(),Total-OHTotal );
   TotalLabel->Caption = MsgSecStr;

   if( OHqcount >= 0 )
	   MsgSecStr.printf( Scstrings_MAIN_ORDER_QUEUE_COUNT.c_str(),OHTotal,OHqcount );
   else
	   MsgSecStr.printf( Scstrings_MAIN_ORDER_COUNT.c_str(),OHTotal );
   OffHourTotalLabel->Caption = MsgSecStr;
   PerformanceGaugeOPT->Add( OrderSec );
   if( OPTAlarmButton->Selected == false && FormSetting->IsOPTCheckingTime( CheckInterval ) && FOPTTick >= CheckInterval  )
   {
		if( PerformanceGaugeOPT->DataExists( CheckInterval ) == false )
			PlaySoundA( FormSetting->GetNoTradeSound().c_str(), NULL, SND_ASYNC|SND_FILENAME );
		FOPTTick = 0;
   }
   FOPTTick++;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::UpdateFUTPerformance( int OrderSec, int Total, int qcount, int OHTotal, int OHqcount, UFCType::UInt32 Status )
{
   int    CheckInterval;
   String MsgSecStr;

   MsgSecStr.printf( Scstrings_MAIN_ORDER_PER_SEC.c_str(),((float)OrderSec)/100.0 );
   FSpeedLabel->Caption   = MsgSecStr;
   if( OrderSec > FFutMaxTP )
   {
	   FFutMaxTP = OrderSec;
	   FMaxTimeLabel->Caption = MsgSecStr;
   }
   if( qcount >= 0 )
   {
	   if( qcount >= FormSetting->GetQueueLimit() && FFUTQueueCount < qcount )
	   {
			MsgSecStr.printf( Scstrings_MAIN_QUEUE_WARNING.c_str(),Scstrings_MAIN_FUTURES, qcount );
			WarningForm->ShowWarning( FSpeedyHost, MsgSecStr, FormSetting->GetWarningSound() );
			WarningForm->LogError( FSpeedyHost, FSpeedyHostIP,"E02",MsgSecStr );
	   }
	   FFUTQueueCount = qcount;
	   MsgSecStr.printf( Scstrings_MAIN_ORDER_QUEUE_COUNT.c_str(),Total-OHTotal,qcount );
   }
   else
	   MsgSecStr.printf( Scstrings_MAIN_ORDER_COUNT.c_str(),Total-OHTotal );
   FTotalLabel->Caption = MsgSecStr;

   if( OHqcount >= 0 )
	   MsgSecStr.printf( Scstrings_MAIN_ORDER_QUEUE_COUNT.c_str(),OHTotal,OHqcount );
   else
	   MsgSecStr.printf( Scstrings_MAIN_ORDER_COUNT.c_str(),OHTotal );
   FOffHourTotalLabel->Caption = MsgSecStr;

   PerformanceGaugeFUT->Add( OrderSec );
   if( FormSetting->IsFUTCheckingTime( CheckInterval ) && FFUTTick >= CheckInterval && FUTAlarmButton->Selected == false )
   {
		if( PerformanceGaugeFUT->DataExists( CheckInterval ) == false )
			PlaySoundA( FormSetting->GetNoTradeSound().c_str(), NULL, SND_ASYNC|SND_FILENAME );
		FFUTTick = 0;
   }
   FFUTTick++;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::UpdatePVCIOLog( TMemoryStream* Stream )
{
	LogListBox->Items->BeginUpdate( );
		TStringList* strs = new TStringList();
		strs->LoadFromStream( Stream );
		delete Stream;
		for( int i = 0 ; i < strs->Count; i++ )
		{
			AnsiString Str = strs->Strings[ i ];
			for( register int j = 1 ; j <= Str.Length(); j++ )
				 if( Str[ j ] == '|' )	 Str[ j ] = ' ';
			LogListBox->Items->Add( Str );
		}
		delete strs;
	LogListBox->Items->EndUpdate();
	if( LogListBox->Count > 6 )
		LogListBox->TopIndex = LogListBox->Count - 6;

	MTree      Data;
	TLineInfo* Line = (TLineInfo*)FLinkPVCToPanel->Objects[ LineComboBox->ItemIndex ];

	Data.append( "COMMAND", AGENT_LISTEN_PVC );
	Data.append( "LINK",    Line->GetLink() );
	Data.append( "PVC",     Line->GetPVC() );
	Data.append( "PVCID",   Line->GetPVCID().c_str() );
	Data.append( "BROKERID",Line->GetBrokerID().c_str() );
	Data.append( "SYS",     (UFCType::Int32)Line->GetSystem() );
	Data.append( "OFFHOUR", (UFCType::Int32)Line->GetOffHour() );
	AgentPublisher->SendData( &Data );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::WndProc( TMessage& Msg )
{
	switch( Msg.Msg )
	{
		  case WM_RESTORE_WIN:     ShowWindow( Application->Handle, SW_SHOW );
								   Application->Restore();
								   break;
		  case WM_CONFIG_FILE:     ConfigProcess();
								   break;
		  case WM_LOGON:           LogonDialog();
								   break;
		  case WM_DISCONNECT_MBUS: DisconnectMBus();
								   break;
		  case WM_SYSCOMMAND:      if( Msg.WParam == SC_MAXIMIZE && FOnMaxinum != NULL )
									   FOnMaxinum( this );
								   break;
	}
	TForm::WndProc( Msg );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::AddEventMessage( const String& Msg )
{
	WriteUserLog( "inf", Msg );
	if( FormSetting->OnlyErrorAlarmTimeDiff() == false )
		WarningForm->ShowWarning( FSpeedyHost, Msg, FormSetting->GetWarningSound() );
	if( Msg.Pos( L"Throughput exceed" ) != 0 )
		WarningForm->LogError( FSpeedyHost, FSpeedyHostIP, "E04", Msg ); ///< exceed 80,90% exchange throughput.
	else if( Msg.Pos( L"can place less then" ) != 0 )
		WarningForm->LogError( FSpeedyHost, FSpeedyHostIP, "E07", Msg ); ///< OrderID less then XXX number.
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::AddFIXMessage( bool IsSend, const AnsiString& FIXMsg )
{
    FFIXLogCS->Enter();
	if( IsSend )
        FFIXLogList->Add( "Receive\n" + FIXMsg );
    else
        FFIXLogList->Add( "Send\n" + FIXMsg );
    FFIXLogCS->Leave();
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::UpdateFIXStat( BOOL broken, int Index )
{
    if( broken == TRUE)
    {
		TFIXSession* session = (TFIXSession*)FFIXSessions->Objects[ Index ];
		OnFIXSessionBroken( session );
	}
    else
        FIXSessionsListView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::UpdateFIXPerformance( int In,int Out )
{
	String inSpeed,outSpeed;

    FIXInPerformanceGauge->Add( In );
    FIXOutPerformanceGauge->Add( Out );
	inSpeed.printf( Scstrings_MAIN_ORDER_PER_SEC.c_str(), (float)In/100.0 );
	outSpeed.printf( Scstrings_MAIN_ORDER_PER_SEC.c_str(), (float)Out/100.0 );
	FIXOutSpeedLabel->Caption = outSpeed;
	FIXInSpeedLabel->Caption  = inSpeed;
	if( In  > FMaxInVol )
	{
		FMaxInVol = In;
		FIXInVolLabel->Caption = inSpeed;
	}
	if( Out > FMaxOutVol )
	{
		FMaxOutVol = Out;
		FIXOutVolLabel->Caption = outSpeed;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::CheckMarketExist( void )
{
	OPTGroupBox->Visible = false;
	FUTGroupBox->Visible = false;
	StockGroupBox->Visible = false;
	OTCGroupBox->Visible = false;
	if( FProcessList->IndexOf( SPEEDY_OPT_NAME )!=-1 || FProcessList->IndexOf( SPEEDY_TFX_NAME )!=-1 || FProcessList->IndexOf( SPEEDY_TW_NAME )!=-1 )
	{
		FMarketSet << mtOption;
		OPTGroupBox->Top = 1000;
		OPTGroupBox->Visible = true;
		PerformanceGaugeOPT->Clear();
		OPTAlarmButton->Selected =	!FormSetting->AlarmOPT( );
	}
	if( FProcessList->IndexOf( SPEEDY_FUT_NAME )!=-1 || FProcessList->IndexOf( SPEEDY_TFX_NAME )!=-1 || FProcessList->IndexOf( SPEEDY_TW_NAME )!=-1 )
	{
		FMarketSet << mtFutures;
		FUTGroupBox->Top = 1000;
		FUTGroupBox->Visible = true;
		PerformanceGaugeFUT->Clear();
		FUTAlarmButton->Selected =	!FormSetting->AlarmFUT( );
	}
	if( FProcessList->IndexOf( SPEEDY_STOCK_NAME )!=-1 || FProcessList->IndexOf( SPEEDY_TW_NAME )!=-1 )
	{
		FMarketSet << mtStock;
		StockGroupBox->Top = 1000;
		StockGroupBox->Visible = true;
		PerformanceGaugeSTOCK->Clear();
		TSEAlarmButton->Selected =	!FormSetting->AlarmTSE( );
	}
	if( FProcessList->IndexOf( SPEEDY_OTC_NAME )!=-1 || FProcessList->IndexOf( SPEEDY_TW_NAME )!=-1 )
	{
		FMarketSet << mtOTC;
		OTCGroupBox->Top = 1000;
		OTCGroupBox->Visible = true;
		PerformanceGaugeOTC->Clear();
		OTCAlarmButton->Selected =	!FormSetting->AlarmOTC( );
	}
	if( !FMarketSet.Empty() )
	{
		if( FMarketSet.Contains( mtFutures ) || FMarketSet.Contains( mtOption ))
			FTTabSheet->TabVisible = FTPPageVisible;
		else
			FTTabSheet->TabVisible = false;
		LineSheet->TabVisible     = true;
		LogSheet->TabVisible      = ConnectionControl;
	}
	else
	{
		LineSheet->TabVisible  = false;
		FTTabSheet->TabVisible = false;
		LogSheet->TabVisible   = false;
	}
	if( FProcessList->IndexOf( SPEEDY_GATEWAY_NAME )!=-1 ||
		FProcessList->IndexOf( SPEEDY_TFX_NAME )!=-1 ||
		FProcessList->IndexOf( SPEEDY_TW_NAME )!=-1 )
		AccountsTabSheet->TabVisible = AEPageVisible;
	else
		AccountsTabSheet->TabVisible = false;
	if( FProcessList->IndexOf( STAR_WAVE_NAME )!=-1 )
		StarWaveTabSheet->TabVisible = true;
	else
		StarWaveTabSheet->TabVisible = false;
	if( FProcessList->IndexOf( DTS_API_NAME )!=-1 )
		APITabSheet->TabVisible = true;
	else
		APITabSheet->TabVisible = false;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::RequestRouteFiles( void )
{
	RequestConfigFile( SPEEDY_ROUTE_EX_CFG );
	RequestConfigFile( SPEEDY_ROUTE_CH_CFG );
	UFC::SleepMS( 20 );
	RequestConfigFile( SPEEDY_ROUTE_R1_CFG );
	RequestConfigFile( SPEEDY_ROUTE_R2_CFG );
	RequestConfigFile( SPEEDY_ROUTE_R3_CFG );
	RequestConfigFile( SPEEDY_ROUTE_R4_CFG );
	RequestConfigFile( SPEEDY_ROUTE_R5_CFG );
	UFC::SleepMS( 20 );
	RequestConfigFile( SPEEDY_ROUTE_ACC_CFG );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::AddProcess( void )
{
	AnsiString  FileName = CurrentDir +"\\" + FSpeedyHost + "-Speedy.ini";
	UFC::UiniFile    ini( FileName.c_str() );
	UFC::Section*    iniSection;
	TListItem*       NewListItem;
	int              ProcessCount;

	FMarketSet.Clear();
	FProcessList->Clear();
	ProcessListView->Clear();
	FCheckers->Clear();
	FCheckers->Add( Scstrings_MAIN_RM_NOT_EXISTS );
	FCheckers->Add( "DynamicRiskManager" );
	ProcessListView->Items->BeginUpdate();
	FMonitorProcess = ini.SectionCount();
	ConnectionSheet->TabVisible = false;
	ProcessCount = ini.SectionCount();
	if( ProcessCount > MAX_PROCESS )
	{
		ProcessCount = MAX_PROCESS;
		ShowMessage( "Support upto 64 procrss");
	}
	for( int i = 0; i < ProcessCount; i++ )
	{
		UFC::AnsiString  DisplayName,StartTime,StopTime,AutoStart,ConfigFile,Param,Value;

		iniSection = ini.GetSection( i );
		FProcessList->Add( iniSection->GetSectionName().c_str());
		NewListItem = ProcessListView->Items->Add();
		NewListItem->ImageIndex = -1;

		if( iniSection->GetValue( "Display",DisplayName )== TRUE )
			NewListItem->Caption = String( DisplayName.c_str());
		else
			NewListItem->Caption = String( iniSection->GetSectionName().c_str() );
		if( iniSection->GetValue( "Auto", AutoStart )== TRUE )
			FProcessAutoStart[i] = AutoStart.ToInt();
		else
			FProcessAutoStart[i] = true;
		if( iniSection->GetValue( "Start", StartTime )== TRUE )
		{
			FProcessStartTime[i] = StartTime.c_str();
			FProcessStartSec[i] = StartTime.SubString(0,2).ToInt()*3600 + StartTime.SubString(3,2).ToInt()*60;
		}
		if( iniSection->GetValue( "Stop", StopTime )== TRUE )
		{
			FProcessStopTime[i]  = StopTime.c_str();
			FProcessStopSec[i]   = StopTime.SubString(0,2).ToInt()*3600 + StopTime.SubString(3,2).ToInt()*60;
		}
		if( iniSection->GetValue( "Config", ConfigFile )== TRUE )
			FProcessConfig[i] = ConfigFile.c_str();
		else
			FProcessConfig[i] = "";
		if( iniSection->GetValue( "Param", Param )== TRUE && Param.AnsiPos("-S") >= 0)
			FIsStandby[ i ] = true;
		else
			FIsStandby[ i ] = false;
		if( iniSection->GetValue( "Checker", Value )== TRUE && Value.ToInt() == 1)
		{
			FIsChecker[ i ] = true;
			FCheckers->Add( iniSection->GetSectionName().c_str() );
		}
		else
			FIsChecker[ i ] = false;
		if( iniSection->GetValue( "Remote", Value )== TRUE && Value.ToInt() == 1)
			FIsRemote[ i ] = true;
		else
			FIsRemote[ i ] = false;
		if( iniSection->GetValue( "WatchDog", Value )== TRUE && Value.ToInt() == 1)
			FEnableWatchDog[ i ] = true;
		else
			FEnableWatchDog[ i ] = false;
		if( iniSection->GetValue( "WatchDogSeconds", Value )== TRUE && Value.ToInt() > 0 )
			FWatchDogSeconds[ i ] = Value.ToInt();
		else
			FWatchDogSeconds[ i ] = 10;
		if( iniSection->GetValue( "WatchDogTimes", Value )== TRUE && Value.ToInt() > 1 )
			FWatchDogTimes[ i ] = Value.ToInt();
		else
			FWatchDogTimes[ i ] = 3;
		if(	iniSection->GetSectionName().AnsiCompare( SPEEDY_PATS_NAME.c_str() ) == 0 ||
			iniSection->GetSectionName().AnsiCompare( SPEEDY_TT_NAME.c_str() ) == 0 )
		{
			FIsExchange[ i ] = true;
			GetConnectionPanel( iniSection->GetSectionName().c_str() )->UpdateLineStat( 0 );
			ConnectionSheet->TabVisible = true;
			RequestRouteFiles( );
		}
		else if( iniSection->GetValue( "Exchange", Value )== TRUE && Value.ToInt() == 1)
		{
			FIsExchange[ i ] = true;
			GetConnectionPanel( iniSection->GetSectionName().c_str() )->UpdateLineStat( 0 );
			ConnectionSheet->TabVisible = true;
			RequestRouteFiles( );
		}
		else
			FIsExchange[ i ] = false;
		FProcessState[ i ] = -1;
	}
	ProcessListView->Items->EndUpdate();
	RequestClusterConfigFile();
	if( AEPageVisible == true &&
		(FProcessList->IndexOf( SPEEDY_GATEWAY_NAME )!=-1 ||
		 FProcessList->IndexOf( SPEEDY_TFX_NAME )!=-1 ||
		 FProcessList->IndexOf( SPEEDY_TW_NAME )!=-1) )
	{
		RequestUserConfigFile();
		AccountsTabSheet->TabVisible = true;
	}
	else
		AccountsTabSheet->TabVisible = false;
	if( FProcessList->IndexOf( SPEEDY_PROFILE_NAME )!=-1 )
	{
		RequestProfileConfigFile();
		ProfileTabSheet->TabVisible = true;
	}
	else
		ProfileTabSheet->TabVisible = false;
	if( FProcessList->IndexOf( SPEEDY_PROXY_NAME )!=-1 )
		ProxySheet->TabVisible = true;
	else
		ProxySheet->TabVisible = false;
	CheckMarketExist();
	if( FProcessList->IndexOf( FIX_NAME )!= -1 )
        FIXSheet->TabVisible = true;
    else
        FIXSheet->TabVisible = false;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::UpdateItem( int Index )
{
	int DspIndex = AllIndexToDisplayIndex( Index );
	if( DspIndex != -1 )
	{
		TOwnerDrawState State;
		TRect Rect = LineListBox->ItemRect( DspIndex );

		State.Clear();
		if( LineListBox->Selected[ DspIndex ] )
			State << odSelected;
		LineListBoxDrawItem( LineListBox, DspIndex, Rect, State );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::LineListBoxDrawItem(TWinControl *Control,
	  int Index, TRect &Rect, TOwnerDrawState State)
{
	TLineInfo* Info;

	if( (Info = (TLineInfo*)LineListBox->Items->Objects[ Index ] ) != NULL )
	{
		bool IsSelected = State.Contains( odSelected );
		Graphics::TBitmap* DrawBuffer = new Graphics::TBitmap();
		TRect BufferRect( 0,0,Rect.Width(),Rect.Height());
		TRect PaintRect;
		TTextFormat  Formats;
		String       OutStr;

		DrawBuffer->Width  = Rect.Width();
		DrawBuffer->Height = Rect.Height();
		DrawBuffer->Canvas->Font = LineListBox->Font;
		DrawBuffer->Canvas->Brush->Color = clWhite;
		DrawBuffer->Canvas->Pen->Color   = clWhite;
		///< Draw gradient background.
		if( ! IsSelected )
		{
			if( Index % 2 == 0 )
				DrawBuffer->Canvas->FillRect( BufferRect );
			else
				DrawGradientBar( DrawBuffer->Canvas->Handle, BufferRect, clWhite, clSilver );
		}
		else
			DrawGradientBar( DrawBuffer->Canvas->Handle, BufferRect, clBlue, clNavy);
		///< Draw system icon.
		DrawBuffer->Canvas->Brush->Style = bsClear;
		switch( Info->GetSystem() )
		{
			case mtFutures: ImageList->Draw( DrawBuffer->Canvas, 5, 2, 1, true );break;
			case mtOption:  ImageList->Draw( DrawBuffer->Canvas, 5, 2, 0, true );break;
			case mtStock:   ImageList->Draw( DrawBuffer->Canvas, 5, 2, 5, true );break;
			case mtOTC:     ImageList->Draw( DrawBuffer->Canvas, 5, 2, 9, true );break;
		}
		///< Draw Broker ID and Password
		if( IsSelected )
			DrawBuffer->Canvas->Font->Color = clWhite;
		else
			DrawBuffer->Canvas->Font->Color = clBlack;
		if( Info->IsTCP() )
		{
			AnsiString DispStr;

			switch( Info->GetSystem() )
			{
				case mtFutures:
				case mtOption:  DispStr.printf( "%s", Info->GetIP().c_str());
								break;
				case mtStock:
				case mtOTC:     if( Info->IsFIX() == true ) ///< FIX Session
								{
									DispStr.printf( "%s:%d In[%d] Out[%d] %dX", Info->GetIP().c_str(),
																				Info->GetLink(),
																				Info->GetInboundSeq(), Info->GetOutboundSeq(),
																				Info->GetThroughput()/20 );
								}
								else
									DispStr.printf( "%s:%d", Info->GetIP().c_str(), Info->GetLink());
								break;
			}
			if( Info->GetBackupIndex() > 0 )
				DrawBuffer->Canvas->Font->Color = clRed;
			if( Info->IsFIX() == true )
				ImageList->Draw( DrawBuffer->Canvas, LINE_TYPE_POS, 2, 13, true );
			else
				ImageList->Draw( DrawBuffer->Canvas, LINE_TYPE_POS, 2, 7, true );
			DrawBuffer->Canvas->TextOut( LINE_IP_POS, TEXT_Y_OFFSET, DispStr );
			if(	DrawBuffer->Canvas->Font->Color == clRed )
			{
				if( IsSelected )
					DrawBuffer->Canvas->Font->Color = clWhite;
				else
					DrawBuffer->Canvas->Font->Color = clBlack;
			}
		}
		else
		{
			ImageList->Draw( DrawBuffer->Canvas, LINE_TYPE_POS, 2, 6, true );
			DrawBuffer->Canvas->TextOut( LINE_IP_POS, TEXT_Y_OFFSET, "------------" );
		}
		///< Draw Broker ID and Password
		Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
		OutStr = Info->GetBrokerID();
		PaintRect = TRect( BROKERID_POS, TEXT_Y_OFFSET, PASSWORD_POS, PVCLabelPanel->Height - 2 );
		DrawBuffer->Canvas->TextRect( PaintRect, OutStr, Formats );
		if( FShowPassword == true )
			OutStr = Info->GetPassword();
		else
			OutStr = L"****";
		PaintRect = TRect( PASSWORD_POS, TEXT_Y_OFFSET, LINE_STAT_POS, PVCLabelPanel->Height - 2 );
		DrawBuffer->Canvas->TextRect( PaintRect, OutStr, Formats );
		///< Draw line state
		Formats.Clear();
		Formats <<tfSingleLine<<tfLeft<<tfVerticalCenter<<tfEndEllipsis;
		OutStr = Info->GetState();
		PaintRect = TRect( LINE_STAT_POS, TEXT_Y_OFFSET, LINE_PORT_POS, PVCLabelPanel->Height - 2 );
		DrawBuffer->Canvas->Font->Color = Info->GetColor( IsSelected );
		DrawBuffer->Canvas->TextRect( PaintRect, OutStr, Formats );
		///< Draw line link number and PVC number
		DrawBuffer->Canvas->Font->Color = clGreen;
		if( Info->GetSystem() == mtFutures || Info->GetSystem() == mtOption )
			DrawBuffer->Canvas->TextOut( LINE_PORT_POS, TEXT_Y_OFFSET, IntToStr(Info->GetLink()) );
		else
			DrawBuffer->Canvas->TextOut( LINE_PORT_POS, TEXT_Y_OFFSET, IntToStr(Info->GetLocalPort()) );
		if( Info->GetSystem() == mtFutures || Info->GetSystem() == mtOption )
			DrawBuffer->Canvas->TextOut( LINE_PVC_POS, TEXT_Y_OFFSET, IntToStr(Info->GetPVC()) );
		else
			DrawBuffer->Canvas->TextOut( LINE_PVC_POS, TEXT_Y_OFFSET, Info->GetPVCID() );
		if( Info->GetGroup() != -1 && Info->IsOrderLine() )
		{
			AnsiString Group;

			if( Info->GetGroup() == 99 )
				Group = "all";
			else
				Group = AnsiString( Info->GetGroup() );
			DrawBuffer->Canvas->TextOut( LINE_GROUP_POS, TEXT_Y_OFFSET, Group );
		}
		else
			DrawBuffer->Canvas->TextOut( LINE_GROUP_POS, TEXT_Y_OFFSET, "--" );
		///< Draw line application code.
		if( IsSelected )
			DrawBuffer->Canvas->Font->Color = clWhite;
		else
			DrawBuffer->Canvas->Font->Color = clNavy;
		OutStr = Info->GetApplication();
		PaintRect = TRect( LINE_APC_POS, TEXT_Y_OFFSET, LINE_MSG_POS, PVCLabelPanel->Height - 2 );
		DrawBuffer->Canvas->TextRect( PaintRect, OutStr, Formats );
		///< Draw message of this line.
		OutStr = Info->GetMessage();
		PaintRect = TRect( LINE_MSG_POS, TEXT_Y_OFFSET, LINE_IP_POS, PVCLabelPanel->Height - 2 );
		DrawBuffer->Canvas->Font->Color = Info->GetColor( IsSelected );
		DrawBuffer->Canvas->TextRect( PaintRect, OutStr, Formats );
		///< Draw buffer to CD
		LineListBox->Canvas->Draw( Rect.left, Rect.top, DrawBuffer );
		delete DrawBuffer;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::SendStatusRequest( const char* BrokerID, int Link, int PVC, const char* PVCID, TMarket Market, BOOL IsOffHour )
{
	MTree Data;

	Data.append( "COMMAND", AGENT_REQUEST_STATE );
	Data.append( "TRADE_SYS", (int)Market );
	if( Link != -1 && PVC != -1 )
	{
		Data.append( "LINK", Link );
		Data.append( "PVC",  PVC );
	}
	Data.append( "OFFHOUR", (int)IsOffHour );
	Data.append( "BROKERID", BrokerID );
	Data.append( "PVCID",  PVCID );
	MessageObject->Send( "SPEEDY.COMMAND",FSpeedyHost.c_str(), &Data );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::StateButtonClick(TObject *Sender)
{
	if( StateButton->Enabled == true )
	{
		TLineInfo* Line;

		for( int i = 0; i < FLinkPVCToPanel->Count; i++ )
		{
			Line = static_cast< TLineInfo* >( FLinkPVCToPanel->Objects[ i ]);
			Line->ClearState();
		}
		LineListBox->Invalidate();
		for( int i = 0; i < FLinkPVCToPanel->Count; i++ )
		{
			if( ( Line = static_cast<TLineInfo*>(FLinkPVCToPanel->Objects[i]))!= NULL )
			{
				SendStatusRequest( Line->GetBrokerID().c_str() ,Line->GetLink(),Line->GetPVC(), Line->GetPVCID().c_str(),Line->GetSystem(),Line->GetOffHour() );
				Application->ProcessMessages();
			}
		}
		StateButton->Enabled = false;
		StateBtnEnableTimer->Enabled = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::StateBtnEnableTimerTimer(TObject *Sender)
{
	StateBtnEnableTimer->Enabled = false;
	StateButton->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FetchUserlist( MTree* Tree )
{
	int Count;
	UFC::AnsiString Host;

    FUserList->Clear();
	if( Tree->get( "HOST", Host ) )
        FSpeedyHost = Host.c_str();
	if( Tree->get( "COUNT", Count ) )
    {
        for( int i = 1; i <= Count; i++ )
		{
            UFC::AnsiString Field = "USER" + UFC::AnsiString( i );
            UFC::AnsiString UserInfo;

            if( Tree->get( Field, UserInfo ))
                FUserList->Add( UserInfo.c_str() );
		}
    }
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::BuildUserlist( void )
{
     UserListView->Clear();
     AccountComboBox->Clear();
     UserListView->Items->BeginUpdate();
     for( int i = 0; i < FUserList->Count; i++ )
     {
		  TStringList* Strs = new TStringList();
		  Strs->Delimiter = ',';
		  Strs->DelimitedText = FUserList->Strings[i];

          TListItem* NewUser = UserListView->Items->Add();
          AccountComboBox->Items->Add( Strs->Strings[ 0 ] );
		  NewUser->Caption = Strs->Strings[ 0 ];
		  if( Strs->Strings[2].ToInt( ) == 0 )
			  NewUser->ImageIndex = 8;		  
		  else
			  NewUser->ImageIndex = 3;
          NewUser->SubItems->Add( Strs->Strings[1] );
          NewUser->SubItems->Add( Strs->Strings[2] );
          NewUser->SubItems->Add( Strs->Strings[3] );
          NewUser->SubItems->Add( Strs->Strings[4] );
          delete Strs;
    }
    UserListView->Items->EndUpdate();
    FUserList->Clear();
    AccountComboBox->ItemIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::AdminSubscriberMessage(const MString &Subject,
	  const MString &Key, MTree *Tree)
{
	int ReplyVal,Result,GP,ExpDays = 0;

	if( Tree->get( "COMMAND", ReplyVal )  )
    {
        if( ReplyVal == CMD_LOGIN_RESULT )
        {
			if( Tree->get( "RESULT", Result ) && FIsLogon == FALSE )
			{
				Tree->get( "WARNING", ExpDays );
				FLogonResponse = true;
				ResponseTimer->Enabled = false;
				switch( Result )
				{
					case 0:  FPasswordExp = false;
							 GroupNames( Tree );
							 AdminLogon( ExpDays );
							 return;
					case -1: FStatusText = Scstrings_MAIN_LOGON_FAILED_ACCOUNT_NOT_EXISTS;
							 if( FOnLogonFalied != NULL )
								 FOnLogonFalied( this );
							 break;
					case -2: FStatusText = Scstrings_MAIN_LOGON_FAILED_PASSWORD_ERROR;
							 if( FOnLogonFalied != NULL )
								 FOnLogonFalied( this );
							 break;
					case -3: if( Tree->get( "GP", GP ) == true )///< NewVersion
							 {
								 FStatusText = Scstrings_MAIN_LOGON_FAILED_PASSWORD_EXPIRED;
								 FPasswordExp = true;
								 if( GP == 0 )
									 AdminLogon( 0 );
								 else
									 OPLogon( 0, GP, Tree );
								 StatusBar->Panels->Items[0]->Text = FStatusText;
								 return;
							 }
							 else
							 {
								FStatusText = Scstrings_MAIN_LOGON_FAILED_PASSWORD_EXPIRED;
								break;
							 }
					case -4: FStatusText = Scstrings_MAIN_LOGON_FAILED_ACCOUNT_LOCKED;
							 if( FOnLogonFalied != NULL )
								 FOnLogonFalied( this );
							 break;
					case -5: FStatusText = Scstrings_MAIN_LOGON_FAILED_ADMIN_LOGONED;
							 if( FOnLogonFalied != NULL )
								 FOnLogonFalied( this );
							 break;
					case -6: FStatusText = Scstrings_MAIN_LOGON_FAILED_NOT_ALLOW_ADMIN;
							 if( FOnLogonFalied != NULL )
								 FOnLogonFalied( this );
							 break;
					case -7: FStatusText = Scstrings_MAIN_LOGON_FAILED_VERSION_TOO_OLD;
							 if( FOnLogonFalied != NULL )
								 FOnLogonFalied( this );
							 break;
					default: FPasswordExp = false;
							 OPLogon( ExpDays, Result, Tree );
							 return;
				}
				MBusDisconnect( FStatusText );
			}
		}
        else if( ReplyVal == CMD_USERS_LIST)
        {
            FLogonResponse = true;
            ResponseTimer->Enabled = false;
            FetchUserlist( Tree );
            BuildUserlist();
        }
        else if( ReplyVal == CMD_ADD_RESULT )
        {
            if( Tree->get( "RESULT", Result ))
            {
                FLogonResponse = true;
                ResponseTimer->Enabled = false;
                switch( Result )
                {
					case 0:  FStatusText = Scstrings_MAIN_ADD_FAILED_UPTO_LIMIT;break;
					case -1: FStatusText = Scstrings_MAIN_ADD_FAILED_ALREADY_EXISTS;break;
					case -2: FStatusText = Scstrings_MAIN_ADD_ACCOUNT_FAILED;break;
					case -4: FStatusText = Scstrings_MAIN_ADD_FAILED_NO_RIGHTS;break;
					default: FStatusText = Scstrings_MAIN_ADD_ACCOUNT_OK;
							 FetchUserlist( Tree );
							 BuildUserlist();
                             break;
                }
                StatusBar->Panels->Items[0]->Text = FStatusText;
            }
        }
        else if( ReplyVal == CMD_DELETE_RESULT )
        {
            FLogonResponse = true;
            ResponseTimer->Enabled = false;
            if( Tree->get( "RESULT", Result ))
            {
                switch( Result )
                {
					case 0:  FStatusText = Scstrings_MAIN_ACCOUNT_NOT_EXISTS;break;
					case -1: FStatusText = Scstrings_MAIN_DEL_FAILED_CANT_DEL_ALL_ADMIN;break;
					case -2: FStatusText = Scstrings_MAIN_DEL_FAILED_CANT_DEL_ALL_ACCOUNT;break;
					case -4: FStatusText = Scstrings_MAIN_DEL_FAILED_NO_RIGHTS;break;
					default: FStatusText = Scstrings_MAIN_DEL_ACCOUNT_OK;
							 FetchUserlist( Tree );
							 BuildUserlist();
							 break;
                }
				StatusBar->Panels->Items[0]->Text = FStatusText;
            }
        }
        else if(  ReplyVal == CMD_MODIFY_RESULT )
        {
            FLogonResponse = true;
            ResponseTimer->Enabled = false;
            if( Tree->get( "RESULT", Result ))
			{
                switch( Result )
                {
                    case 0:
					case -1: FStatusText = Scstrings_MAIN_ACCOUNT_NOT_EXISTS;break;
					case -4: FStatusText = Scstrings_MAIN_MODIFY_FAILED_NO_RIGHTS;break;
					default: FStatusText = Scstrings_MAIN_MODIFY_ACCOUNT_OK;
							 FetchUserlist( Tree );
							 BuildUserlist();
							 break;
				}
				StatusBar->Panels->Items[0]->Text = FStatusText;
			}
		}
		else if(  ReplyVal == CMD_MODIFY_PWD_RESULT )
		{
			FLogonResponse = true;
			ResponseTimer->Enabled = false;
			if( Tree->get( "RESULT", Result ))
			{
				switch( Result )
				{
					case 0:
					case -1: FStatusText = Scstrings_MAIN_ACCOUNT_NOT_EXISTS;break;
					case -4: FStatusText = Scstrings_MAIN_MODIFY_PASSWORD_NO_RIGHTS;break;
					default: FStatusText = Scstrings_MAIN_MODIFY_PASSWORD_OK;
                             FetchUserlist( Tree );
                             BuildUserlist();
                             break;
                }
				StatusBar->Panels->Items[0]->Text = FStatusText;
            }
        }
		else if( ReplyVal == CMD_REPLY_LOG )
        {
            char* Buffer;
            int   Size;

            FLogonResponse = true;
            ResponseTimer->Enabled = false;
            if( Tree->get("LOG",Buffer,Size))
            {
                TMemoryStream* UserLogStream = new TMemoryStream();
                UserLogStream->WriteBuffer( Buffer, Size );
                UserLogStream->Seek( 0, soFromBeginning );
                FUserLogList->LoadFromStream( UserLogStream );
                delete UserLogStream;
				LoadUserLog();
            }
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::LoadUserLog()
{
     UserLogListView->Clear();
     UserLogListView->Items->BeginUpdate();
     for( int i = 0; i < FUserLogList->Count; i++ )
     {
        TStringList* Strs = new TStringList();
        Strs->Delimiter = ',';
		Strs->DelimitedText = FUserLogList->Strings[i];
		if( Strs->Count >= 5 ) ///<
        {
            TListItem* NewLog = UserLogListView->Items->Add();
            AnsiString Type = Strs->Strings[ 3 ];

            if( Type == "inf" )
                NewLog->ImageIndex = 0; ///< information message
            else if( Type == "war" )
                NewLog->ImageIndex = 1; ///< warning message
			else if( Type == "err" )
                NewLog->ImageIndex = 2; ///< Error message
            NewLog->SubItems->Add( Strs->Strings[0] );///< Date
            NewLog->SubItems->Add( Strs->Strings[1] );///< Time
            AnsiString Message;
            for( int i = 4; i < Strs->Count; i++ )
            {
                 Message += Strs->Strings[i];
                 Message += " ";
            }
            NewLog->SubItems->Add( Message ); ///< Message
        }
        delete Strs;
    }
	UserLogListView->Items->EndUpdate();
    FUserLogList->Clear();
    QueryButton->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ResponseTimerTimer(TObject *Sender)
{
    ResponseTimer->Enabled = false;
    if( FLogonResponse == false )
    {
        LogoutButtonClick( NULL );
		ShowMessage( Scstrings_MAIN_NO_RESPONSE_CHECK_SPEEDYAGENT );
    }
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
	TIniFile* iniFile = new TIniFile( CurrentDir +"\\SpeedyClientCfg.ini");
	FormSetting->WriteIniInteger( iniFile, "Monitoring", "CPU", CPUTrackBar->Position  );
	FormSetting->WriteIniInteger( iniFile, "Monitoring", "Memory", MemTrackBar->Position  );
	FormSetting->WriteIniInteger( iniFile, "Monitoring", "Disk", DiskTrackBar->Position  );
	FormSetting->WriteIniInteger( iniFile, "Monitoring", "TimeDiff", TimeDiffTrackBar->Position  );
	delete iniFile;
	if( FOnMDIClosed != NULL )
		FOnMDIClosed( this );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	TCloseAction Action;

	if( FIsLogon )
	{
		String Msg;

		if( PageControl->ActivePage == ProxySheet )
			PageControl->ActivePageIndex = 0;
		Msg.printf( Scstrings_MAIN_ASK_LOGOUT_BEFORE_EXIT.c_str(), String( FSpeedyHost.c_str()) );
		if( FormSetting->CloseConfirm( Msg ) == true )
		{
			LogoutButtonClick( NULL );
			FormClose( this, Action );
			CanClose = true;
		}
		else
		{
			MDIForm->StopCloseAll();
			CanClose = false;
		}
	}
	else
	{
		FormClose( this, Action );
		CanClose = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FormShow(TObject *Sender)
{
	TIniFile* iniFile = new TIniFile( CurrentDir+"\\SpeedyClientCfg.ini");

	CPUTrackBar->Position      = iniFile->ReadInteger( "Monitoring", "CPU", 80  );
	MemTrackBar->Position      = iniFile->ReadInteger( "Monitoring", "Memory", 80  );
	DiskTrackBar->Position     = iniFile->ReadInteger( "Monitoring", "Disk", 80  );
	TimeDiffTrackBar->Position = iniFile->ReadInteger( "Monitoring", "TimeDiff", 500  );
	delete iniFile;

	CPUTrackBarPositionChange( NULL );
	MemTrackBarPositionChange( NULL );
	DiskTrackBarPositionChange( NULL );
	TimeDiffTrackBarPositionChange( NULL );

	Constraints->MaxWidth = Screen->Width*2;
	Constraints->MaxHeight = Screen->Height*2;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::NewUserButtonClick(TObject *Sender)
{
	NewUserForm = new TNewUserForm( );
	NewUserForm->GroupComboBox->Items->Clear();
	NewUserForm->GroupComboBox->Items->AddStrings( this->FGroupNames );
	NewUserForm->GroupComboBox->ItemIndex = 0;
	if( NewUserForm->ShowModal() == mrOk )
    {
		MTree        Data;
		AnsiString   ID = NewUserForm->IDEdit->Text;
		AnsiString   Password = NewUserForm->PwdEdit->Text;
		UFC::MD5 md5( (const UFCType::UInt8*)Password.c_str(),Password.Length());

		int Group = NewUserForm->GroupComboBox->ItemIndex;
		Data.append( "COMMAND", CMD_ADD_USER );
		Data.append( "ID", ID.c_str());
		Data.append( "PWD", md5.ToString().c_str() );
		Data.append( "GROUP", Group );
		AdminPublisher->SendData( &Data );
		FLogonResponse = false;
		ResponseTimer->Enabled = true;
	}
    delete NewUserForm;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::DelUserButtonClick(TObject *Sender)
{
    TListItem* Del = UserListView->Selected;

    if( Del != NULL )
    {
		if( MessageDlg( Scstrings_MAIN_ASK_DELETE_AE + Del->Caption+ L"?", mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0 ) == mrOk )
        {
			MTree      Data;
			AnsiString DelID( Del->Caption );

			Data.append( "COMMAND", CMD_DELETE_USER );
			Data.append( "ID", DelID.c_str());
			AdminPublisher->SendData( &Data );
			FLogonResponse = false;
			ResponseTimer->Enabled = true;
        }
    }
	else
		ShowMessage( Scstrings_MAIN_ASK_SELECT_DEL_AE );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::AttribButtonClick(TObject *Sender)
{
	TListItem* Modify = UserListView->Selected;
	if( Modify != NULL )
	{
		NewUserForm = new TNewUserForm( Modify );
		NewUserForm->GroupComboBox->Items->Clear();
		NewUserForm->GroupComboBox->Items->AddStrings( this->FGroupNames );
		NewUserForm->GroupComboBox->ItemIndex = Modify->SubItems->Strings[1].ToInt();
		if( NewUserForm->ShowModal() == mrOk )
		{
			MTree      Data;
			int        Group = NewUserForm->GroupComboBox->ItemIndex;
			int        State = NewUserForm->StateComboBox->ItemIndex;
			AnsiString ID    = NewUserForm->IDEdit->Text;

			Data.append( "COMMAND", CMD_MODIFY_USER );
			Data.append( "ID", ID.c_str());
			Data.append( "GROUP", Group );
			Data.append( "STATE", State );
			AdminPublisher->SendData( &Data );
			FLogonResponse = false;
			ResponseTimer->Enabled = true;
		}
		delete NewUserForm;
	}
	else
		ShowMessage( Scstrings_MAIN_ASK_SELECT_MODIFY_AE );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::PasswdButtonClick(TObject *Sender)
{
	TListItem* Modify = UserListView->Selected;

	if( Modify != NULL )
	{
		NewUserForm = new TNewUserForm( Modify, false );
		if( NewUserForm->ShowModal() == mrOk )
		{
			MTree        Data;
			AnsiString   ID = NewUserForm->IDEdit->Text;
			AnsiString   Password = NewUserForm->PwdEdit->Text;
			UFC::MD5 md5( (const UFCType::UInt8*)Password.c_str(), Password.Length());

			Data.append( "COMMAND", CMD_MODIFY_PWD );
			Data.append( "ID", ID.c_str());
			Data.append( "PWD", md5.ToString() );
			AdminPublisher->SendData( &Data );
			FLogonResponse = false;
			ResponseTimer->Enabled = true;
		}
		delete NewUserForm;
	}
	else
		ShowMessage( Scstrings_MAIN_ASK_SELECT_MODIFY_PASSWORD_AE );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::UserListViewCustomDrawSubItem(
	  TCustomListView *Sender, TListItem *Item, int SubItem,
	  TCustomDrawState State, bool &DefaultDraw)
{
	TRect  ItemRect;
	TColor TextColor = clBlack;
	String Text = Item->SubItems->Strings[ SubItem-1 ];

	DefaultDraw = false;
	ListView_GetSubItemRect( UserListView->Handle, Item->Index, SubItem,LVIR_BOUNDS,&ItemRect);
	if( SubItem == 1 )
	{
		Text = L"********";
	}
	else if( SubItem == 2 )
	{
		int Group = Text.ToInt();
		if( Group < this->FGroupNames->Count )
			Text = FGroupNames->Strings[ Group ];
		else
			Text = Scstrings_MAIN_LOGON_ID_STATUS + IntToStr( Group );
	}
	else if( SubItem == 3 )
	{
		switch( Text.ToInt() )
		{
			case 0: Text = Scstrings_MAIN_LOGON_ID_STATUS_NORMAL; TextColor = clBlue; break;
			case 1: Text = Scstrings_MAIN_LOGON_ID_STATUS_LOCKED; TextColor = clRed;  break;
			case 2:	if( Item->Caption == L"Admin" )
					{
						Text = Scstrings_MAIN_LOGON_ID_STATUS_NORMAL;
						TextColor = clBlue;
					}
					else
					{
						Text = Scstrings_MAIN_LOGON_ID_STATUS_EXPIRED;
						TextColor = clRed;
					}
					break;
		}
	}
	else if( SubItem == 4 )
	{
		if( Item->Caption == L"Admin" )
			Text = L"----/--/--";
	}
	if( State.Contains( cdsSelected ))
		SetTextColor( UserListView->Canvas->Handle,(COLORREF)clNavy );
	else
		SetTextColor( UserListView->Canvas->Handle,(COLORREF)TextColor );
	TTextFormat Formats;

	Formats<<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
	Sender->Canvas->TextRect( ItemRect, Text, Formats );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::MonitorOtherProcess( bool Monitor )
{
	if( Monitor == true )
	{
		ProcessListView->Align = alTop;
		ProcessListView->Height = ProcessSheet->Height - 286;
		ProcessSplitter->Align = alTop;
		ProcessSplitter->Enabled = true;
		OtherProcessListView->Align  = alClient;

		AnsiString  FileName = CurrentDir +"\\"+ FSpeedyHost+"-Others.ini";
		UFC::UiniFile    ini( FileName.c_str() );
		UFC::Section*    iniSection;
		TListItem*       NewListItem;

		OtherProcessListView->Clear();
		OtherProcessListView->Items->BeginUpdate();
		FMonitorOtherProcess = ini.SectionCount();

		for( int i = 0; i < ini.SectionCount(); i++ )
		{
			UFC::AnsiString  DisplayName,StartTime,StopTime,AutoStart,ConfigFile,Param,Value;

			iniSection = ini.GetSection( i );
			FOtherProcessList->Add( iniSection->GetSectionName().c_str());
			NewListItem = OtherProcessListView->Items->Add();
			NewListItem->ImageIndex = 4;
			if( iniSection->GetValue( "Display",DisplayName )== TRUE )
				NewListItem->Caption = String( DisplayName.c_str() );
			else
				NewListItem->Caption = String( iniSection->GetSectionName().c_str() );
			if( iniSection->GetValue( "Auto", AutoStart )== TRUE )
				FOtherProcessAutoStart[i] = AutoStart.ToInt();
			else
				FOtherProcessAutoStart[i] = true;
			if( iniSection->GetValue( "Start", StartTime )== TRUE )
				FOtherProcessStartTime[i] = StartTime.c_str();
			if( iniSection->GetValue( "Stop", StopTime )== TRUE )
				FOtherProcessStopTime[i]  = StopTime.c_str();
			FOtherProcessState[ i ] = -1;
		}
		OtherProcessListView->Items->EndUpdate();
	}
	else
	{
		for( int i = 0; i < MAX_PROCESS; i++ )
			 FOtherProcessState[ i ] = PROCESS_STOPED;
		FOtherProcessList->Clear();
		OtherProcessListView->Clear();
		OtherProcessListView->Align = alBottom;
		OtherProcessListView->Height = 0;
		ProcessSplitter->Align = alBottom;
		ProcessSplitter->Enabled = false;
		ProcessListView->Height = ProcessSheet->Height;
		ProcessListView->Align = alClient;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::SaveAndLoadRule( int index, char* Buffer, int Size  )
{
	AnsiString       ConfigFileName;

	FRuleFile[index].printf("%s-Rule%d.ini", FSpeedyHost.c_str(), index + 1);
	ConfigFileName = CurrentDir + "\\" + FRuleFile[index];
	SaveToFile( FRuleFile[index], Buffer, Size );
	FRulesIni[index].Load( ConfigFileName.c_str(), true );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::SpeedyAgentSubscriberMessage(
      const MString &Subject, const MString &Key, MTree *Tree)
{
	int ReplyVal,Result;
	UFC::AnsiString Host,ExpDate;

	if( Tree->get( "COMMAND", ReplyVal )  )
	{
		if( ReplyVal == SPEEDY_AGENT_REPLY )
		{
			if( Tree->get( "HOST", Host ) && SearchForm->Visible && FIsLogon == false )
			{
				int ServerType = 0;
				int Version = 1000;
				AnsiString* PassString = new AnsiString();

				Tree->get( "TYPE", ServerType );
				Tree->get( "VER", Version );
				if( Tree->get( "Date", ExpDate ) == true )
				{
					int DateInt = ExpDate.ToInt();
					int dd   = DateInt%100;
					int mm   = (DateInt%10000)/100;
					int yyyy = DateInt/10000;

					PassString->printf("%s,%04d/%02d/%02d", Host.c_str(),yyyy,mm,dd );

					UFC::UDate Today;
					UFC::UDate ExpDay( dd, mm, yyyy );
					Today.setCurrent();
					if( yyyy - Today.getYear() <= 2)
					{
						int Days =  ExpDay - Today;
						if( Days < 60 )
						{
							AnsiString Msg;

							Msg.printf( "License expired after %d days.", Days );
							WarningForm->ShowWarning( Host.c_str(), Msg, FormSetting->GetLineErrorSound() );
							WarningForm->LogError( Host.c_str(), FSpeedyHostIP,"E09",Msg );
						}
					}

				}
				else
					PassString->printf("%s",Host.c_str() );
				HeartbeatResponseTimer->Enabled = false;
				FHeartbeatResponse = true;
				PostMessage( SearchForm->Handle, WM_ADD_HOSTS,(long)PassString, Version*10 + ServerType );
				ProcessStateRequestTimer->Enabled = true;
			}
        }
		else if( ReplyVal == SPEEDY_REPLY_LIST )
		{
			AddSettings( Tree );
			AddSettingToListView();
		}
        else if( ReplyVal == SPEEDY_REPLY_PROCESS ) ///< SpeedyAgent Report all processes status.
        {   ///< Turn off heartbeat timeout checking timer.
			HeartbeatResponseTimer->Enabled = false;
			FHeartbeatResponse = true;
			if( Tree->get( "PROCESS", Result ))
				UpdateProcessState( Result, 0 );
			if( Tree->get( "PROCESS1", Result ))
				UpdateProcessState( Result, 1 );
			if( Tree->get( "OTHERS", Result ))
				UpdateOtherProcessState( Result, 0 );
			if( Tree->get( "OTHERS1", Result ))
				UpdateOtherProcessState( Result, 1 );
			///< Turn on process state request timer
			ProcessStateRequestTimer->Enabled = true;
		}
        else if( ReplyVal == SPEEDY_REPLY_FIX_SETTING )
        {
			char* Buffer;
            int   Size;
            if( Tree->get("FILE",Buffer,Size))
            {
                TFileStream* FIXSettingfile = new TFileStream( CurrentDir +"\\"+FSpeedyHost+"-FIXSetting.ini",fmCreate);
                FIXSettingfile->WriteBuffer( Buffer, Size );
                FIXSettingfile->Size = Size;
                delete FIXSettingfile;
                if( Tree->get( "COUNT",Size ) )
                {
                    UFC::AnsiString Name,Value;
                    FFIXDicts->Clear();
                    for( int i = 0; i < Size; i++ )
                    {
						Name.Printf( "%d",i );
                        if( Tree->get( Name, Value ) )
                            FFIXDicts->Add( Value.c_str() );
                    }
                }
				GetSessionsFromSettingFile();
				if(	IsProcessRunning( FIX_NAME ) == true )
					RequestFIXSessionsState( );
			}
        }
		else if( ReplyVal == SPEEDY_REPLY_PROCESS_LIST )
        {
            char* Buffer;
            int   Size;
			if( Tree->get("FILE",Buffer,Size))
			{
				TFileStream* ProcessSettingfile = new TFileStream( CurrentDir +"\\"+FSpeedyHost+"-Speedy.ini",fmCreate);
				ProcessSettingfile->WriteBuffer( Buffer, Size );
				ProcessSettingfile->Size = Size;
				delete ProcessSettingfile;
				AddProcess();
			}
			if( Tree->get( "OTHER",Buffer,Size ))
			{
				TFileStream* ProcessSettingfile = new TFileStream( CurrentDir +"\\"+FSpeedyHost+"-Others.ini",fmCreate);
                ProcessSettingfile->WriteBuffer( Buffer, Size );
				ProcessSettingfile->Size = Size;
                delete ProcessSettingfile;
                MonitorOtherProcess( true );
			}
            else
                MonitorOtherProcess( false );
        }
		else if( ReplyVal == SPEEDY_REPLY_PVC_LOG )
        {
            char* Buffer;
            int   Size;
			if( PageControl->ActivePage == LogSheet )
			{
				if( Tree->get("LOGDATA",Buffer,Size))
				{
					int begin = GetTickCount();
					String Msg;

					FPVCLogCS->Enter();
					FPVCLogList->Clear();
					FPVCLogCS->Leave();
					TMemoryStream* Stream = new TMemoryStream();
					Stream->WriteBuffer( Buffer, Size );
					Stream->Seek( 0, soFromBeginning );
					UpdatePVCIOLog( Stream );
					Msg.printf( L"Log size:%d bytes,Parse log use:%dms", Size, GetTickCount()-begin );
					StatusBar->Panels->Items[2]->Text = Msg;
					PVCLogTimer->Enabled = true;
				}
				MonitorButton->Enabled = true;
			}
		}
        else if( ReplyVal == SPEEDY_REPLY_CONFIG_FILE )
        {
			char* Buffer;
            int   Size;
            UFC::AnsiString FileName;

            if( Tree->get("FILE",Buffer,Size) && Tree->get("FILENAME",FileName ) )
            {
				if( FileName.AnsiCompare( SPEEDY_USERS_CFG ) == 0 )
				{
					if( FModifyType == mtNone )
					{
						AnsiString  UsersSettingFile = GetUserConfigFileName( false );
						SaveToFile( UsersSettingFile.c_str(),Buffer,Size );
						ReadUsers();
					}
					else
						UpdateUserConfigFile();
				}
				else if( FileName.AnsiCompare( SPEEDY_CLUSTER_CFG ) == 0 )
				{
					AnsiString  ClusterSettingFile;

					ClusterSettingFile.printf("%s-SpeedyCluster.cfg", FSpeedyHost.c_str());
					SaveToFile( ClusterSettingFile, Buffer, Size );
					CreateCluster( ClusterSettingFile );
				}
				else if( FileName.AnsiCompare( SPEEDY_PROFILE_CFG ) == 0 )
				{
					AnsiString  ProfileSettingFile = GetProfileConfigFileName( false );
					SaveToFile( ProfileSettingFile, Buffer, Size );
					CreateProfilePanels();
				}
				else if( FileName.AnsiNCompare( "Route", 5 ) == 0 )
				{
					if( FileName.AnsiCompare( SPEEDY_ROUTE_EX_CFG ) == 0 )
					{
						UFC::AnsiString  Name,Value;
						AnsiString       ConfigFileName;

						FExSettingFile.printf("%s-Exchange.ini", FSpeedyHost.c_str());
						ConfigFileName = CurrentDir + "\\" + FExSettingFile;
						SaveToFile( FExSettingFile, Buffer, Size );
						FRuleExchangeIni.Load( ConfigFileName.c_str(), true );
						FRuleExchangeList->Clear();
						for( int i = 1;; i++ )
						{
							Name.Printf("ex%d", i );
							if(	FRuleExchangeIni.GetValue( "Exchange", Name,Value ) == true )
								FRuleExchangeList->Add( Value.c_str() );
							else
								break;
						}
					}
					else if( FileName.AnsiCompare( SPEEDY_ROUTE_CH_CFG ) == 0 )
					{
						UFC::AnsiString  Name,Value;
						AnsiString       ConfigFileName;

						FChSettingFile.printf("%s-Channel.ini", FSpeedyHost.c_str());
						ConfigFileName = CurrentDir + "\\" + FChSettingFile;
						SaveToFile( FChSettingFile, Buffer, Size );
						FRuleChannelIni.Load( ConfigFileName.c_str(), true );
						FRuleChannelList->Clear();
						for( int i = 1;; i++ )
						{
							Name.Printf("Ch%d", i );
							if(	FRuleChannelIni.GetValue( "Setting", Name,Value ) == true )
								FRuleChannelList->Add( Value.c_str() );
							else
								break;
						}
					}
					else if( FileName.AnsiCompare( SPEEDY_ROUTE_R1_CFG ) == 0 )
					{
						FRemoteRuleFile[ 0 ] = SPEEDY_ROUTE_R1_CFG;
						SaveAndLoadRule( 0, Buffer, Size );
					}
					else if( FileName.AnsiCompare( SPEEDY_ROUTE_R2_CFG ) == 0 )
					{
						FRemoteRuleFile[ 1 ] = SPEEDY_ROUTE_R2_CFG;
						SaveAndLoadRule( 1, Buffer, Size );
					}
					else if( FileName.AnsiCompare( SPEEDY_ROUTE_R3_CFG ) == 0 )
					{
						FRemoteRuleFile[ 2 ] = SPEEDY_ROUTE_R3_CFG;
						SaveAndLoadRule( 2, Buffer, Size );
					}
					else if( FileName.AnsiCompare( SPEEDY_ROUTE_R4_CFG ) == 0 )
					{
						FRemoteRuleFile[ 3 ] = SPEEDY_ROUTE_R4_CFG;
						SaveAndLoadRule( 3, Buffer, Size );
					}
					else if( FileName.AnsiCompare( SPEEDY_ROUTE_R5_CFG ) == 0 )
					{
						FRemoteRuleFile[ 4 ] = SPEEDY_ROUTE_R5_CFG;
						SaveAndLoadRule( 4, Buffer, Size );
					}
					else if( FileName.AnsiCompare( SPEEDY_ROUTE_ACC_CFG ) == 0 )
					{
						UFC::AnsiString  Value;
						int              UseRule = 0;
						AnsiString       ConfigFileName;

						FAccSettingFile.printf("%s-Account.ini", FSpeedyHost.c_str());
						ConfigFileName = CurrentDir + "\\" + FAccSettingFile;
						SaveToFile( FAccSettingFile, Buffer, Size );
						FRuleAccountIni.Load( ConfigFileName.c_str(), true );
						if(	FRuleAccountIni.GetValue( "Account", "Default", Value ) == true  )
						{
							if( Value.LastChar() == '1' )
								UseRule = 0;
							else if( Value.LastChar() == '2' )
								UseRule = 1;
							else if( Value.LastChar() == '3' )
								UseRule = 2;
							else if( Value.LastChar() == '4' )
								UseRule = 3;
							else if( Value.LastChar() == '5' )
								UseRule = 4;
						}
						RuleComboBox->ItemIndex = UseRule;
					}
				}
				else
				{
					SaveToFile( "Config.ini",Buffer,Size );
					FConfigFileName = FileName.c_str();
					PostMessage( Handle,WM_CONFIG_FILE,0,0 );
				}
			}
		}
	}
}
//------------------------------------------------------------------------------
TColor __fastcall TSimTFXForm::StringToColor( const char* ColStr )
{
	TStringList* Strs = new TStringList();

	Strs->Delimiter = ',';
	Strs->DelimitedText = ColStr;
	if( Strs->Count == 3 )
	{
		int R = Strs->Strings[0].ToInt();
		int G = Strs->Strings[1].ToInt();
		int B = Strs->Strings[1].ToInt();
		delete Strs;
		return (TColor)RGB( R,G,B );
	}
	delete Strs;
	return clBlack;
}
//------------------------------------------------------------------------------
void __fastcall TSimTFXForm::CreateProfilePanels( void )
{
	FGraphPanels->Clear();
	AnsiString  ProfileSettingFile = GetProfileConfigFileName( true );
	UFC::UiniFile    ini( ProfileSettingFile.c_str() );
	UFC::AnsiString  SectionName,GraphName,Value;
	TProfileFrame*   ProfilePanel;

	for( int i = 0; i < ini.SectionCount(); i++ )
	{
		SectionName.Printf( "Graph%d", i+1 );
		if( ini.SectionExists( SectionName ) == FALSE )
			return;
		if( ini.GetValue( SectionName,"Display", GraphName ) == TRUE )
		{
			ProfilePanel = new TProfileFrame( ScrollBox,i, GraphName.c_str() );
			if( ini.GetValue( SectionName,"BKColor", Value ) == TRUE )
				ProfilePanel->SetBackgroundColor( StringToColor(Value.c_str()));
			if( ini.GetValue( SectionName,"GridColor", Value ) == TRUE )
				ProfilePanel->SetGridColor( StringToColor(Value.c_str()));
			if( ini.GetValue( SectionName,"LineColor", Value ) == TRUE )
				ProfilePanel->SetLineColor( StringToColor(Value.c_str()));
			FGraphPanels->Add( ProfilePanel );
		}
	}
}
//------------------------------------------------------------------------------
bool __fastcall TSimTFXForm::ClusterButtonExists( const AnsiString& Name )
{
	String ClusterServerName( Name );

	for( int i = 0; i < ServersToolBar->ButtonCount; i++ )
	{
	   if( ServersToolBar->Buttons[i]->Caption == ClusterServerName )
		   return true;
	}
	return false;
}
//------------------------------------------------------------------------------
void __fastcall TSimTFXForm::CreateCluster( const AnsiString& SettingFile )
{
	AnsiString       ConfigFileName = CurrentDir + "\\" + SettingFile;
	UFC::UiniFile    ini( ConfigFileName.c_str() );
	UFC::AnsiString  SectionName( FSpeedyHost.c_str() );
	UFC::AnsiString  Name,Value;

	if( ini.SectionExists( FSpeedyHost.c_str() ) == FALSE )
		return;
	for( int i = 1; ;i++)
	{
		Name = UFC::AnsiString( i );
		if( ini.GetValue( SectionName, Name, Value ) == FALSE )
			break;
		if( Value.AnsiCompare( SectionName ) != 0 &&
			ClusterButtonExists( Value.c_str() ) == false )
		{
			TToolButton* ClusterServeBtn = new TToolButton( ServersToolBar );
			ClusterServeBtn->Parent     = ServersToolBar;
			ClusterServeBtn->ImageIndex = 7;
			ClusterServeBtn->Caption    = Value.c_str();
			ClusterServeBtn->Grouped    = true;
			ClusterServeBtn->Down       = false;
			ClusterServeBtn->Enabled    = false;
			ClusterServeBtn->AllowAllUp = false;
			ClusterServeBtn->Style      = tbsCheck;
			ClusterServeBtn->OnClick    = ToolButtonClick;
		}
	}
	MessageObject->RequestMBusList();
}
//------------------------------------------------------------------------------
void __fastcall TSimTFXForm::ReloadMBusProcess( void )
{
	///< Reload process list
	MTree Data;

	Data.append( "COMMAND", SPEEDY_GET_PROCESS_LIST );
	Data.append( "HOST", FSpeedyHost.c_str());
	SpeedyAgentPublisher->SendData( &Data );
}
//------------------------------------------------------------------------------
void __fastcall TSimTFXForm::ConfigProcess( void )
{
	if( AppInfoForm->Show( this, FConfigFileName.c_str() ) == mrOk )
	{
		AnsiString   ConfigFileName = CurrentDir + "\\Config.ini";
		SendFile( ConfigFileName, "FILE", SPEEDY_SET_CONFIG_FILE, FConfigFileName.c_str() );

		if( FConfigFileName.AnsiCompare( SPEEDY_PROCESS_CFG ) == 0 )
		{
			for( int i=0;i<50;i++)
			{
				UFC::SleepMS( 10 );
				Application->ProcessMessages();
			}
			ReloadMBusProcess();
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TSimTFXForm::SaveToFile( const AnsiString& FileName, char *Buffer, int Size )
{
	AnsiString   ConfigFileName = CurrentDir + "\\" + FileName;

	TFileStream* Configfile = new TFileStream( ConfigFileName, fmCreate);
	Configfile->WriteBuffer( Buffer, Size );
	Configfile->Size = Size;
	delete Configfile;
}
//------------------------------------------------------------------------------
void __fastcall TSimTFXForm::ReadUsers( void )
{
	AccountsListView->Clear();
	AccountsListView->Items->BeginUpdate();

	AnsiString       ConfigFileName = GetUserConfigFileName();
	UFC::UiniFile    ini( ConfigFileName.c_str() );
	UFC::Section*    iniSection;
	TListItem*       NewListItem;

	for( int i = 0; i < ini.SectionCount(); i++ )
	{
		UFC::AnsiString  Type,IP,Accounts,Begin,End,Terms,Source = "0";

		iniSection = ini.GetSection( i );
		if( iniSection->GetSectionName() == "Speedy" || iniSection->GetSectionName() == "SpeedyOffHour" )
			continue;
		NewListItem = AccountsListView->Items->Add();
		NewListItem->Caption = iniSection->GetSectionName().c_str();
		NewListItem->Data = NULL;

		iniSection->GetValue( "Type",Type );
		iniSection->GetValue( "IP",IP );
		iniSection->GetValue( "Source",Source );
		if( Type == "Admin" )
		{
			NewListItem->ImageIndex = 8;
			NewListItem->SubItems->Add( Scstrings_MAIN_ADMIN );//"管理者");
		}
		else if( Type == "Channel" )
		{
			NewListItem->ImageIndex = 10;
			NewListItem->SubItems->Add( Scstrings_MAIN_AE_ORDER_CHANNEL );//"下單管道");
		}
		else
		{
			NewListItem->ImageIndex =3;
			NewListItem->SubItems->Add( Scstrings_MAIN_AE_ACCOUNT_EXECUTIVE );//"營業員");
		}
		NewListItem->SubItems->Add( FormSetting->Sources->Strings[Source.ToInt()] );
		if( IP == "All" )
			NewListItem->SubItems->Add( Scstrings_MAIN_AE_IP_NOT_FIXED );//"不指定");
        else
			NewListItem->SubItems->Add( IP.c_str() );
		for( int j = 0; j < iniSection->ItemCount(); j++ )
		{
			UFC::AnsiString Name,Value;
			iniSection->GetNameValue( j, Name,Value );
			if( (Name.Length() == 7 || Name.Length() == 10) && Value.Length() == 1 )
			{
				UFC::AnsiString Acc;
				if( Name.Length() == 7 )
					Acc = Name;
				else if( Name.Length() == 10 )
					Acc = Name.c_str() + 3;

				if( Accounts.Length() > 0 )
					Accounts = Accounts + "," + Acc;
				else
					Accounts += Acc;
			}
		}
		NewListItem->SubItems->Add( "0" ); ///< Connections
		NewListItem->SubItems->Add( Accounts.c_str() );
		if( iniSection->GetValue( "Begin",Begin ) && iniSection->GetValue( "End",End ) )
		{
			NewListItem->SubItems->Add( Begin.c_str() );
			NewListItem->SubItems->Add( End.c_str() );
		}
		else if( iniSection->GetValue( "Terminals", Terms ))
		{
			TStringList* TermList = new TStringList();
			TermList->Delimiter = ';';
			TermList->DelimitedText = Terms.c_str();
			if( TermList->Count > 0 )
			{
				if( TermList->Strings[0].Length( ) == 2 )
				{
					NewListItem->SubItems->Add( TermList->Strings[0]+"001" );
					NewListItem->SubItems->Add( TermList->Strings[TermList->Count-1]+"999" );
				}
				else if( TermList->Strings[0].Length( ) == 3 )
				{
					NewListItem->SubItems->Add( TermList->Strings[0]+"01" );
					NewListItem->SubItems->Add( TermList->Strings[TermList->Count-1]+"99" );
				}
			}
			else
			{
				NewListItem->SubItems->Add( "-" );
				NewListItem->SubItems->Add( "-" );
			}
			delete TermList;
		}
		else
		{
			NewListItem->SubItems->Add( "-" );
			NewListItem->SubItems->Add( "-" );
		}
	}
	AccountsListView->Items->EndUpdate();
}
//------------------------------------------------------------------------------
//  FIX setting functions
//------------------------------------------------------------------------------
void __fastcall TSimTFXForm::ClearFIXSessions()
{
	for( int i = 0; i < FFIXSessions->Count; i++ )
		 delete FFIXSessions->Objects[ i ];
    FFIXSessions->Clear();
    FIXSessionsListView->Clear();
}
//------------------------------------------------------------------------------
void __fastcall TSimTFXForm::SendFile( const AnsiString& File,const AnsiString& Name,int CMD,const char* FileName )
{
	unsigned char* Buffer;
	int            Size;
	MTree          Data;

	try
	{
		UFC::FileStream SendFile( File.c_str(), O_RDONLY );

		Size   = SendFile.GetSize( );
		Buffer = new unsigned char[ Size ];
		Size   = SendFile.Read( Buffer, Size );
		Data.append( "COMMAND", CMD );
		Data.append( "HOST", FSpeedyHost.c_str());
		if( FServerVersion > 2000 )///< Ver 2.0.01 later, support compress node.
			Data.append( Name.c_str(), Buffer, Size, TRUE );
		else
			Data.append( Name.c_str(), Buffer, Size );
		if( FileName != NULL )
			Data.append( "FILENAME", FileName );
		SpeedyAgentPublisher->SendData( &Data );
		delete [] Buffer;
	}
	catch( UFC::FileException& )
	{
		String Msg;

		Msg.printf( Scstrings_MAIN_READ_FILE_FAILED.c_str(), String(File) );
		ShowMessage( Msg );
	}
}
//------------------------------------------------------------------------------
void __fastcall TSimTFXForm::SetFIXDefaultIniSection( UFC::UiniFile* ini )
{
	AnsiString LogPath,StorePath;
	UFC::AnsiString  SocketAcceptPort( TAcceptor::GetAcceptorPort());

	TIniFile* iniFile = new TIniFile( CurrentDir +"\\SpeedyClientCfg.ini");
	LogPath   = iniFile->ReadString( "FIX Setting", "LogPath","../FIXLogs");
	StorePath = iniFile->ReadString( "FIX Setting", "StorePath","../FIXStore");
	delete iniFile;

	ini->SetValue("DEFAULT","FileStorePath", StorePath.c_str() );
	ini->SetValue("DEFAULT","FileLogPath", LogPath.c_str() );
	ini->SetValue("DEFAULT","CheckLatency","N");
	ini->SetValue("DEFAULT","SocketAcceptPort",SocketAcceptPort );
	ini->SetValue("DEFAULT","SocketNodelay","Y");
	ini->SetValue("DEFAULT","SocketReuseAddress","Y");
}
//------------------------------------------------------------------------------
void __fastcall TSimTFXForm::SaveSessionsToSettingFile()
{
	AnsiString       FIXSettingFile = CurrentDir +"\\"+FSpeedyHost+"-FIXSetting.ini";
	UFC::UiniFile    ini( FIXSettingFile.c_str() );

	ini.Clear();
	SetFIXDefaultIniSection( &ini );
	for( int i = 0; i < FFIXSessions->Count; i++ )
	{
		TFIXSession* FIXSession = dynamic_cast<TFIXSession*>(FFIXSessions->Objects[ i ]);
		if( FIXSession )
			ini.AddSection( FIXSession->CreateConfigSession());
	}
	ini.Save();
	GetSessionsFromSettingFile();
	SendFile( FIXSettingFile, "FIX", SPEEDY_CHANGE_FIX_SETTING );
}
//------------------------------------------------------------------------------
void __fastcall TSimTFXForm::GetSessionsFromSettingFile()
{
	AnsiString       FIXSettingFile = CurrentDir + "\\"+ FSpeedyHost + "-FIXSetting.ini";
	UFC::UiniFile    ini( FIXSettingFile.c_str() );
	UFC::Section*    iniSection;
	UFC::AnsiString  Type,Port;
	TListItem*       NewListItem;
	int SocketPort = 5000;

	ClearFIXSessions();
	if( ini.GetValue( "DEFAULT", "SocketAcceptPort", Port ) )
		SocketPort = Port.ToInt();
	TAcceptor::SetAcceptorPort( SocketPort );
	FIXSessionsListView->Items->BeginUpdate();
	for( int i = 0; i < ini.SectionCount(); i++ )
	{
		iniSection = ini.GetSection( i );
		if( iniSection->GetSectionName() == "SESSION" )
		{
			if( iniSection->GetValue("ConnectionType",Type ) && Type == "acceptor")
			{
				TAcceptor* Acceptor = new TAcceptor( iniSection );
				FFIXSessions->AddObject( Acceptor->GetSessionID(),Acceptor);
				NewListItem = FIXSessionsListView->Items->Add();
				NewListItem->Caption = "Acceptor";
				NewListItem->ImageIndex = 6;
			}
			else
			{
				TInitiator* Initiator = new TInitiator( iniSection );
				FFIXSessions->AddObject( Initiator->GetSessionID(),Initiator );
				NewListItem = FIXSessionsListView->Items->Add();
				NewListItem->Caption = "Initiator";
				NewListItem->ImageIndex = 7;
			}
		}
	}
	FIXSessionsListView->Items->EndUpdate();
}
//---------------------------------------------------------------------------
bool __fastcall TSimTFXForm::IsErrorProcess( int ProcessIndex )
{
	if( FProcessState[ ProcessIndex ] == PROCESS_STOPED &&
		FIsRemote[ ProcessIndex ] == FALSE &&
		FProcessAutoStart[ ProcessIndex ] == TRUE )
	{
		int TimeNow = UFC::GetSecondsToday();

		if( FProcessStartSec[ ProcessIndex ] < FProcessStopSec[ ProcessIndex ] )
		{
			if( TimeNow > FProcessStartSec[ ProcessIndex ] && TimeNow < FProcessStopSec[ ProcessIndex ]  )
				return true; ///< This process should be running.
		}
		else ////< Start time > Stop Time, Over on day case. (exp: 22:00 start --> 08:00 stop
		{
			if( TimeNow > FProcessStartSec[ ProcessIndex ] || TimeNow < FProcessStopSec[ ProcessIndex ] )
				return true; ///< This process should be running.
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TSimTFXForm::IsRunningTime( int ProcessIndex )
{
	if( FIsRemote[ ProcessIndex ] == TRUE ||
		FProcessAutoStart[ ProcessIndex ] == TRUE )
	{
		int TimeNow = UFC::GetSecondsToday();

		if( FProcessStartSec[ ProcessIndex ] < FProcessStopSec[ ProcessIndex ] )
		{
			if( TimeNow > FProcessStartSec[ ProcessIndex ] && TimeNow < FProcessStopSec[ ProcessIndex ]  )
				return true; ///< This process should be running.
		}
		else ////< Start time > Stop Time, Over on day case. (exp: 22:00 start --> 08:00 stop
		{
			if( TimeNow > FProcessStartSec[ ProcessIndex ] || TimeNow < FProcessStopSec[ ProcessIndex ] )
				return true; ///< This process should be running.
		}
	}
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ProcessListViewCustomDrawSubItem(
	  TCustomListView *Sender, TListItem *Item, int SubItem,
	  TCustomDrawState State, bool &DefaultDraw)
{
	TRect ItemRect;
	bool  ErrorProcess = IsErrorProcess( Item->Index );

	DefaultDraw = false;
	ListView_GetSubItemRect( ProcessListView->Handle, Item->Index, SubItem,LVIR_BOUNDS,&ItemRect);
	if( Item->Selected )
	{
		Sender->Canvas->Brush->Style = bsClear;
		if( ErrorProcess == true )
			DrawGradientBar( Sender->Canvas->Handle , ItemRect, clFuchsia, clRed );
		else
			DrawGradientBar( Sender->Canvas->Handle , ItemRect, clWhite, clSilver );
	}
	else
	{
		Sender->Canvas->Brush->Style = bsSolid;
		if( ErrorProcess == true )
			Sender->Canvas->Brush->Color = clFuchsia;
		else
			Sender->Canvas->Brush->Color = ProcessListView->Color;
		Sender->Canvas->FillRect( (TRect)ItemRect );
	}
	if( SubItem == 1 )
	{
		String Text;
		int    ImgIndex;
		TColor Color;
		TTextFormat Formats;

		switch( FProcessState[ Item->Index ]  )
		{
			case PROCESS_STOPED:    Text = Scstrings_MAIN_PROCESS_STAT_STOPED;   ImgIndex = 4;
									if( ErrorProcess == true )
										Color = clYellow;
									else
										Color = clRed;
									break;
			case PROCESS_STARTING:  Text = Scstrings_MAIN_PROCESS_STAT_SATRTING; ImgIndex = 5; Color = clNavy;break;
			case PROCESS_STOPING:   Text = Scstrings_MAIN_PROCESS_STAT_STOPING; ImgIndex = 5; Color  = clNavy;break;
			case PROCESS_RESTARTING:Text = Scstrings_MAIN_PROCESS_STAT_RESTART; ImgIndex = 5; Color  = clNavy;break;
			case PROCESS_RUNNING:   if( FIsChecker[ Item->Index ] == true )
										ImgIndex = 14;
									else if( FIsExchange[ Item->Index ] == true )
										ImgIndex = 16;
									else
										ImgIndex = 3;
									Text  = Scstrings_MAIN_PROCESS_STAT_RUNNING;
									Color = clGreen;
									break;
			default:                Text = L"---";
									ImgIndex = 5;
									Color = clBlack;
									break;
		}
		StateImageList->Draw( Sender->Canvas, ItemRect.left + 5, ItemRect.top+1, ImgIndex, true);
		SetTextColor( Sender->Canvas->Handle,(COLORREF)Color );
		Formats<<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
		Sender->Canvas->TextRect( ItemRect, Text, Formats );
	}
	else if( SubItem == 2 )
	{
		String Text;
		TTextFormat Formats;

		if( FIsRemote[ Item->Index ] == false )
		{
			if( FProcessAutoStart[ Item->Index ] )
				Text = FProcessStartTime[ Item->Index ];
			else
				Text = L"--:--";
		}
		else
			Text = L"     ";
		SetTextColor( Sender->Canvas->Handle,(COLORREF)clNavy );
		Formats<<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
		Sender->Canvas->TextRect( ItemRect, Text, Formats );
	}
	else if( SubItem == 3 )
	{
		String Text;
		TTextFormat Formats;

		if( FIsRemote[ Item->Index ] == false )
		{
			if( FProcessAutoStart[ Item->Index ] )
				Text = FProcessStopTime[ Item->Index ];
			else
				Text = L"--:--";
		}
		else
			Text = L"     ";
		SetTextColor( Sender->Canvas->Handle,(COLORREF)clNavy );
		Formats<<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
		Sender->Canvas->TextRect( ItemRect, Text, Formats );
	}
	else if( SubItem == 4 ) ///< Setting.
	{
		if( FIsRemote[ Item->Index ] == false )
		{
			int Offset = ItemRect.left + (ItemRect.right -ItemRect.left- StateImageList->Width )/2;
			StateImageList->Draw( Sender->Canvas, Offset, ItemRect.top+1, 13, true);
		}
	}
	else if( SubItem == 5 ) ///<Start
	{
		int Offset = ItemRect.left + (ItemRect.right -ItemRect.left- StateImageList->Width )/2;
		if( FIsRemote[ Item->Index ] == false )
		{
			if( FProcessState[ Item->Index ] != PROCESS_RUNNING )
				StateImageList->Draw( Sender->Canvas, Offset, ItemRect.top+1, 0, true);
			else
				StateImageList->Draw( Sender->Canvas, Offset, ItemRect.top+1, 10, true);
		}
	}
	else if( SubItem == 6 ) ///< Stop
	{
		int Offset = ItemRect.left + (ItemRect.right -ItemRect.left- StateImageList->Width )/2;
		if( FIsRemote[ Item->Index ] == false )
		{
			if( FProcessState[ Item->Index ] == PROCESS_RUNNING )
				StateImageList->Draw( Sender->Canvas, Offset, ItemRect.top+1, 1, true);
			else
				StateImageList->Draw( Sender->Canvas, Offset, ItemRect.top+1, 11, true);
		}
	}
	else if( SubItem == 7 )
	{
		int Offset = ItemRect.left + (ItemRect.right -ItemRect.left- StateImageList->Width )/2;
		if( FIsRemote[ Item->Index ] == false )
			StateImageList->Draw( Sender->Canvas, Offset, ItemRect.top+1, 2, true);
	}
	ProcessListView->Canvas->Brush->Style = bsSolid;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::OnChangeToFIXPage( void )
{
	bool Running = IsProcessRunning( FIX_NAME );

	NewSessionButton->Enabled       = !Running;
	SessionSettingButton->Enabled   = !Running;
	DeleteSessionButton->Enabled    = !Running;
	MonitorFIXButton->Enabled       = Running;
	FIXStateButton->Enabled         = Running;
	FIXModifyMenu->Enabled          = !Running;
	FIXDlelteMenu->Enabled          = !Running;
	FIXBrokenSessionMenu->Enabled   = Running;
	FIXSetSessionSeqMenu->Enabled   = Running;
	FIXSessionEndOfDayMenu->Enabled = Running;
	if( FIXSessionsListView->Items->Count == 0 )
	{
		FIXSessionsListView->Clear();
		RequestFIXSessionList( );
	}
	else
		FIXSessionsListView->ItemIndex = 0;
	if( FimFIXRunningState != Running )
		FimFIXRunningState = Running;
	if( Running == true )
		RequestFIXSessionsState( );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::OnChangeToPVCPage( void )
{
	int   StateNow = ((IsProcessRunning(SPEEDY_OTC_NAME)<<3)|
					  (IsProcessRunning(SPEEDY_TW_NAME )<<3)|
					  (IsProcessRunning(SPEEDY_STOCK_NAME)<<2)|
					  (IsProcessRunning(SPEEDY_TW_NAME )<<2)|
					  (IsProcessRunning(SPEEDY_FUT_NAME)<<1)|
					  (IsProcessRunning(SPEEDY_TFX_NAME)<<1)|
					  (IsProcessRunning(SPEEDY_TW_NAME )<<1)|
					   IsProcessRunning(SPEEDY_OPT_NAME)|
					   IsProcessRunning(SPEEDY_TFX_NAME)|
					   IsProcessRunning(SPEEDY_TW_NAME ));
	bool  Running = true;

    if( StateNow == 0 )
        Running = false;
	NewPVCButton->Enabled       = !Running;
	ModifyPVCButton->Enabled    = !Running;
	DeletePVCButton->Enabled    = !Running;
	MenuItemModifyPVC->Enabled  = !Running;
	MenuItemDeletePVC->Enabled  = !Running;
	CopySettingMenuItem->Enabled= !Running;
	DisablePVCMenuItem->Enabled = Running;
	CODPVCMenuItem->Enabled = Running;
	EnablePVCMenuItem->Enabled  = Running;
	StateButton->Enabled        = Running;
	MonitorButton->Enabled      = Running;
	PVCMonitorButton->Enabled   = Running;

	BROKERID_POS   = StaticText4->Left;
	PASSWORD_POS   = StaticText5->Left;
	LINE_STAT_POS  = StaticText6->Left;
	LINE_TYPE_POS  = StaticText8->Left;
	LINE_PORT_POS  = StaticText3->Left;
	LINE_PVC_POS   = StaticText9->Left;
	LINE_APC_POS   = StaticText10->Left;
	LINE_GROUP_POS = StaticText11->Left;
	LINE_MSG_POS   = StaticText7->Left;
	LINE_IP_POS    = StaticText1->Left;
	TEXT_Y_OFFSET  = 3;
    if( StateNow != FSpeedyRunningState || StateNow == 0 )
    {
        FSpeedyRunningState = StateNow;
        ClearAllLines();
        RequestPVCSettingList();
    }
    else
    {
        StateButtonClick( NULL );
    }
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::OnChangeToFileTransferPage( void )
{
	AnsiString Section[]={"Futures","Options"};
	String SysName[]={Scstrings_MAIN_FUTURES,Scstrings_MAIN_OPTIONS};
	String FileCode,Name,FileName;
	int        Count;

	FileName = CurrentDir +"\\Data\\FTDefTMP.ini";
	SysComboBoxChange( NULL );
	FileListView->Clear();
	if( FileExists( FileName ) == true )
	{
		TIniFile* iniFile = new TIniFile( FileName );
		for( int Sys = 0; Sys <= 1; Sys++ )
		{
			 Count = iniFile->ReadInteger( Section[Sys], L"Count" ,0);
			 for( int i = 0; i < Count; i++ )
			 {
				  FileCode = iniFile->ReadString( Section[Sys], L"FileCode" + IntToStr( i+1 ),L"000");
				  Name     = iniFile->ReadString( Section[Sys], FileCode, L"000");
				  TListItem* NewItem = FileListView->Items->Add();
				  NewItem->Caption = FileCode;
				  NewItem->SubItems->Add( SysName[Sys] );
				  NewItem->SubItems->Add( Name );
				  NewItem->SubItems->Add( L"---" );
			 }
		}
		delete iniFile;
	}
	ResendPageControl->ActivePageIndex = 0;
	FLEXSysComboBoxChange( NULL );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::PageControlChange(TObject *Sender)
{
	if( PageControl->ActivePage == FTTabSheet )
		OnChangeToFileTransferPage();
    else if( PageControl->ActivePage == LineSheet || PageControl->ActivePage == LogSheet)
		OnChangeToPVCPage();
	else if( PageControl->ActivePage == FIXSheet )
		OnChangeToFIXPage();
	else if( PageControl->ActivePage == AccountsTabSheet )
	{
		MTree Data;

		Data.append( "COMMAND", AGENT_ASK_AE_STATUS );
		AgentPublisher->SendData( &Data );
	}
	else if( PageControl->ActivePage == ProxySheet )
	{
		if( ProxyTabSet->Tabs->Count >= 1 )
			ProxyTabSet->TabIndex = 0;
		if( FProxyStatEmpty == true )
		{
			FProxyStatEmpty = false;
			MTree Data;

			Data.append( "CMD", CMD_PROXY_STATUS_REQ );
			MessageObject->Send( "SPEEDYCENTER.CMD",FSpeedyHost.c_str(),&Data );
		}
	}
	else if( PageControl->ActivePage == ConnectionSheet )
	{
		//RuleTabSet->TabIndex = 0;
		LoadRule( RuleTabSet->TabIndex );
	}
	else if( PageControl->ActivePage == APITabSheet )
	{
		if( APISettingForm == NULL )
			APISettingForm = new TAPISettingForm(this);
		APIStateButtonClick( NULL );
		QueryAPISetting();
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::SysComboBoxChange(TObject *Sender)
{
	AnsiString Section;
	AnsiString Name;
	AnsiString FileName;
	int Count;

	FCodeComboBox->Clear();
	if( this->ResendPageControl->ActivePage == this->X25TabSheet )
		FileName = CurrentDir +"\\Data\\FTDef.ini";
	else
		FileName = CurrentDir +"\\Data\\FTDefTMP.ini";
	if( FileExists( FileName ) == true )
	{
		TIniFile* iniFile = new TIniFile( FileName );
		if( SysComboBox->ItemIndex == 0 )
			Section = "Futures";
		else
			Section = "Options";
		Count = iniFile->ReadInteger( Section,"Count" ,0);
		for( int i = 0;i < Count; i++ )
		{
			Name = "FileCode"+IntToStr(i+1);
			FCodeComboBox->Items->Add( iniFile->ReadString( Section, Name,"000"));
		}
		FCodeComboBox->ItemIndex = 0;
		delete iniFile;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ProcessStateRequestTimerTimer(TObject *Sender)
{
	///< Turn off request timer
	ProcessStateRequestTimer->Enabled = false;
	///< Turn off HeartbeatResponse timer
	HeartbeatResponseTimer->Enabled = false;
    ///< Send out the process state request
    MTree Data;

    Data.append( "COMMAND", SPEEDY_GET_PROCESS );
    Data.append( "HOST", FSpeedyHost.c_str());
    SpeedyAgentPublisher->SendData( &Data );
	FHeartbeatResponse = false;
    ///< Turn on Heartbeat timeout timer
    HeartbeatResponseTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::SendShellCMD( int CMD, int ProcessIndex, bool IsMBusApp )
{
	MTree Data;

	Data.append( "COMMAND", CMD );
	Data.append( "INDEX", ProcessIndex );
	Data.append( "HOST", FSpeedyHost.c_str());
	if( IsMBusApp == false )
		Data.append( "MBusApp", 0 );
	else
		Data.append( "MBusApp", 1 );
	SpeedyAgentPublisher->SendData( &Data );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ProcessListViewMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
	TListItem*  Item = ProcessListView->GetItemAt( X, Y);
	if( Item )
	{
		RECT ItemRect;

		for( int i = 2; i <= 7; i++ )
		{
			ListView_GetSubItemRect( ProcessListView->Handle, Item->Index, i, LVIR_BOUNDS,&ItemRect);
			if( X > ItemRect.left && X < ItemRect.right )
			{
				ProcessListView->Cursor = crHandPoint;
				return;
			}
		}
	}
    ProcessListView->Cursor = crDefault;
}
//---------------------------------------------------------------------------
bool __fastcall TSimTFXForm::IsProcessRunning( const AnsiString& ProcessName )
{
    int Index = FProcessList->IndexOf( ProcessName );
    if( Index == -1 )
        return false;
    return (FProcessState[ Index ] == PROCESS_RUNNING);
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::OnProcessCrashed( int Index )
{
	AnsiString ProcessName = FProcessList->Strings[ Index ];

	if( ProcessName.AnsiCompare( FIX_NAME )== 0 )
		ProcessName = "SpiderNet";
	else if( FConnectionPanelMap.IsExists( ProcessName.c_str() ) == true )
		GetConnectionPanel( ProcessName.c_str() )->UpdateLineStat( 0 );
	if( IsRunningTime( Index ) == true )
	{
		String Message;
		AnsiString Msg;

		Msg.printf("Process:%s crashed!", ProcessName.c_str() );
		Message.printf( Scstrings_MAIN_PROCESS_CRASHED.c_str(), String(ProcessName.c_str()) );
		WriteUserLog( "err", Msg );
		WarningForm->ShowWarning( FSpeedyHost, Message, FormSetting->GetProcessErrorSound() );
		WarningForm->LogError( FSpeedyHost, FSpeedyHostIP,"E01",Message );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::OnOtherProcessCrashed( int Index )
{
	String Message;
	String Msg;
	String     ProcessName = FOtherProcessList->Strings[ Index ];

	Msg.printf( L"Process:%s crashed!", ProcessName );
	WriteUserLog( "err", Msg );
	Message.printf( Scstrings_MAIN_PROCESS_CRASHED.c_str(), ProcessName );
	WarningForm->ShowWarning( FSpeedyHost, Message, FormSetting->GetProcessErrorSound() );
	WarningForm->LogError( FSpeedyHost, FSpeedyHostIP,"E01",Message );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ChangeProcessState( int Index, int State )
{
	if( FProcessState[ Index ] != State )
	{
		TCustomDrawState DrawState;
		bool             DefaultDraw;
		bool             FullUpdate = false;
		TListItem*       Item = ProcessListView->Items->Item[ Index ];

		if( Item != NULL )
		{
			if( FProcessState[ Index ] == -1 ) ///< First time pdate.
				FullUpdate = true;
			if( State == PROCESS_RUNNING ) ///< Procress running.
				FullUpdate = true;
			if( (FProcessState[ Index ] == PROCESS_RUNNING || FProcessState[ Index ] == PROCESS_STOPING )
				&& State == PROCESS_STOPED ) ///< Procress crashed.
			{
				FullUpdate = true;
				OnProcessCrashed( Index );
			}
			FProcessState[ Index ] = State;
			if( FullUpdate == false )
			{
				ProcessListViewCustomDrawSubItem( ProcessListView,Item,1,DrawState,DefaultDraw );
				ProcessListViewCustomDrawSubItem( ProcessListView,Item,5,DrawState,DefaultDraw );
				ProcessListViewCustomDrawSubItem( ProcessListView,Item,6,DrawState,DefaultDraw );
			}
			else
			{
				TOwnerDrawState State;
				TRect           ItemRect;

				if( Item->Selected == true )
					State << odSelected;
				ListView_GetItemRect( ProcessListView->Handle, Item->Index, (RECT*) &ItemRect, LVIR_LABEL );
				ProcessListViewDrawItem(ProcessListView,Item, ItemRect, State );
			}
			OnProcessStateChanged( Index, State );
		}
    }
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ChangeOtherProcessState( int Index, int State )
{
    if( FOtherProcessState[ Index ] != State )
    {
        TCustomDrawState DrawState;
        bool             DefaultDraw;
        TListItem*       Item = OtherProcessListView->Items->Item[ Index ];

        if( Item != NULL )
        {
            if( (FOtherProcessState[ Index ] == PROCESS_RUNNING || FOtherProcessState[ Index ] == PROCESS_STOPING )
                && State == PROCESS_STOPED )
				OnOtherProcessCrashed( Index );
            FOtherProcessState[ Index ] = State;
			OtherProcessListViewCustomDrawSubItem( OtherProcessListView,Item,1,DrawState,DefaultDraw );
			OtherProcessListViewCustomDrawSubItem( ProcessListView,Item,4,DrawState,DefaultDraw );
			OtherProcessListViewCustomDrawSubItem( ProcessListView,Item,5,DrawState,DefaultDraw );
		}
    }
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::OnProcessStateChanged  ( int Index, int State )
{
    if( FProcessList->Strings[ Index ] == FIX_NAME )
    {
		if( State != PROCESS_RUNNING )
			ClearFIXState( true );
		if( PageControl->ActivePage == FIXSheet )
		   PageControlChange( NULL );
	}
	else if( FProcessList->Strings[ Index ] == SPEEDY_FUT_NAME ||
			 FProcessList->Strings[ Index ] == SPEEDY_OPT_NAME ||
			 FProcessList->Strings[ Index ] == SPEEDY_TFX_NAME ||
			 FProcessList->Strings[ Index ] == SPEEDY_TW_NAME  ||
			 FProcessList->Strings[ Index ] == SPEEDY_STOCK_NAME ||
			 FProcessList->Strings[ Index ] == SPEEDY_OTC_NAME )
	{
		if( PageControl->ActivePage == LineSheet )
			PageControlChange( NULL );
	}
	else if( FProcessList->Strings[ Index ] == SPEEDY_TOP_NAME )
	{
		if( State == PROCESS_RUNNING )
			ServerResourcePanel->Width = 185;
		else
			ServerResourcePanel->Width = 1;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::UpdateProcess( TListItem* ModifyItem )
{
	AnsiString  FileName = CurrentDir +"\\"+FSpeedyHost+"-Speedy.ini";
	UFC::UiniFile    ini( FileName.c_str() );
	UFC::Section*    iniSection = ini.GetSection( ModifyItem->Index );

	if( iniSection )
	{
		UFC::AnsiString StartTime = FProcessStartTime[ ModifyItem->Index ].c_str();
		UFC::AnsiString StopTime  = FProcessStopTime[ ModifyItem->Index ].c_str();

		iniSection->SetValue( "Start", StartTime );
		iniSection->SetValue( "Stop",  StopTime );
		FProcessStartSec[ ModifyItem->Index ] = StartTime.SubString(0,2).ToInt()*3600 + StartTime.SubString(3,2).ToInt()*60;
		FProcessStopSec[ ModifyItem->Index ] = StopTime.SubString(0,2).ToInt()*3600 + StopTime.SubString(3,2).ToInt()*60;
		iniSection->SetValue( "Auto",  UFC::AnsiString((int)FProcessAutoStart[ ModifyItem->Index ]));

		iniSection->SetValue( "WatchDog",  UFC::AnsiString((int)FEnableWatchDog[ ModifyItem->Index ]));
		iniSection->SetValue( "WatchDogSeconds",  UFC::AnsiString((int)FWatchDogSeconds[ ModifyItem->Index ]));
		iniSection->SetValue( "WatchDogTimes",  UFC::AnsiString((int)FWatchDogTimes[ ModifyItem->Index ]));
		ini.Save();
		SendFile( FileName, "FILE", SPEEDY_MODIFY_PROCESS );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::UpdateOtherProcess( TListItem* ModifyItem )
{
	AnsiString  FileName = CurrentDir +"\\"+FSpeedyHost+"-Others.ini";
	UFC::UiniFile    ini( FileName.c_str() );
	UFC::Section*    iniSection = ini.GetSection( ModifyItem->Index );

	if( iniSection )
	{
		iniSection->SetValue( "Start", FOtherProcessStartTime[ ModifyItem->Index ].c_str() );
		iniSection->SetValue( "Stop",  FOtherProcessStopTime[ ModifyItem->Index ].c_str() );
		iniSection->SetValue( "Auto",  UFC::AnsiString((int)FOtherProcessAutoStart[ ModifyItem->Index ]));
		ini.Save();
		SendFile( FileName, "FILE", SPEEDY_SET_CONFIG_FILE, "OtherProcess.cfg" );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ProcessListViewMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	TListItem* SelItem = ProcessListView->GetItemAt( 10, Y );

	if( SelItem )
	{
		if( FIsRemote[ SelItem->Index ] == true )
			return; ///<  Can't control remote process.
		RECT   ItemRect,ItemRect2;
		String Msg;
		TPoint PT( X, Y );

		ListView_GetSubItemRect( ProcessListView->Handle, SelItem->Index, 2,LVIR_BOUNDS,&ItemRect );
		ListView_GetSubItemRect( ProcessListView->Handle, SelItem->Index, 3,LVIR_BOUNDS,&ItemRect2 );
		///< Setting process Start/Stop time
		if( PtInRect( ItemRect, PT ) || PtInRect( ItemRect2, PT ) )
		{
			if( ProcessSetting == false )
			{
				StatusBar->Panels->Items[0]->Text = Scstrings_MAIN_PROCESS_NO_RIGHTS_CHANGE_TIME;//"無權限設定程式啟動停止時間";
				return;
			}
			ProcessForm = new TProcessForm( SelItem->Caption, FProcessStartTime[ SelItem->Index ],FProcessStopTime[ SelItem->Index ],FProcessAutoStart[ SelItem->Index ],
											FEnableWatchDog[ SelItem->Index ], FWatchDogSeconds[ SelItem->Index ] , FWatchDogTimes[ SelItem->Index ]);
			if( ProcessForm->ShowModal() == mrOk )
			{
				FProcessStartTime[ SelItem->Index ] = ProcessForm->GetStartTime();
				FProcessStopTime[ SelItem->Index ]  = ProcessForm->GetStopTime();
				FProcessAutoStart[ SelItem->Index ] = ProcessForm->GetAutoStart();
				FEnableWatchDog[ SelItem->Index ]   = ProcessForm->WatchDogCheckBox->Checked;
				FWatchDogSeconds[ SelItem->Index ]  = ProcessForm->WatchSecondsSpinEdit->Value;
				FWatchDogTimes[ SelItem->Index ]    = ProcessForm->RestartTimesSpinEdit->Value;
				ProcessListView->Invalidate();
				UpdateProcess( SelItem );
			}
			delete ProcessForm;
			return;
		}
		///< Setting App info
		ListView_GetSubItemRect( ProcessListView->Handle, SelItem->Index, 4,LVIR_BOUNDS,&ItemRect);
		if( PtInRect( ItemRect, PT ))
		{
			if( ProcessSetting == false )
			{
				this->StatusBar->Panels->Items[0]->Text = Scstrings_MAIN_PROCESS_NO_RIGHTS_CHANGE_SETTING;//"無權限設定程式";
				return;
			}
			if( FProcessList->Strings[ SelItem->Index ] == SPEEDY_FUT_NAME )
				AppInfoForm->ShowSpeedy( this, mtFutures );
			else if( FProcessList->Strings[ SelItem->Index ] == SPEEDY_OPT_NAME )
				AppInfoForm->ShowSpeedy( this, mtOption );
			else if( FProcessList->Strings[ SelItem->Index ] == SPEEDY_STOCK_NAME )
				AppInfoForm->ShowSpeedy( this, mtStock );
			else if( FProcessList->Strings[ SelItem->Index ] == SPEEDY_OTC_NAME )
				AppInfoForm->ShowSpeedy( this, mtOTC );
			else if( FProcessList->Strings[ SelItem->Index ] == SPEEDY_TW_NAME ||
					 FProcessList->Strings[ SelItem->Index ] == SPEEDY_TFX_NAME )
				AppInfoForm->ShowSpeedy( this, mtTW );
			else if( FProcessList->Strings[ SelItem->Index ] == FIX_NAME )
			{
				if( AppInfoForm->ShowSpiderNet( this, SelItem->Index, FSpeedyHost ) == mrOk )
				{
					AnsiString  FileName = CurrentDir +"\\"+FSpeedyHost+"-Speedy.ini";
					SendFile( FileName, "FILE", SPEEDY_MODIFY_PROCESS );
				}
			}
			else
			{
				if( FProcessConfig[ SelItem->Index ].Length() > 0 )
				{
					MTree Data; ///< Request for the config file.

					Data.append( "COMMAND", SPEEDY_GET_CONFIG_FILE );
					Data.append( "FILENAME", FProcessConfig[ SelItem->Index ].c_str());
					Data.append( "HOST", FSpeedyHost.c_str());
					Data.append( "ZIP", 1 );
					SpeedyAgentPublisher->SendData( &Data );
				}
				else
					ShowMessage( Scstrings_MAIN_PROCESS_NO_CONFIG_FILE );//"這個程式沒有相對的設定檔案.");
			}
			return;
		}
		if( ProcessControl == false )
		{
			StatusBar->Panels->Items[0]->Text = Scstrings_MAIN_PROCESS_NO_RIGHTS_STOP_START_PROCESS;//"無權限啟動或停止程式";
			return;
		}
		///< Start process
		ListView_GetSubItemRect( ProcessListView->Handle, SelItem->Index, 5,LVIR_BOUNDS,&ItemRect);
		if( PtInRect( ItemRect, PT ))
		{
			Msg.printf( Scstrings_MAIN_PROCESS_START_ASK.c_str(),FProcessList->Strings[ SelItem->Index ]);
			if( FormSetting->Confirm( Msg ) == true )
			{
				if( FProcessState[ SelItem->Index ] == PROCESS_RUNNING )
				{
					Msg.printf( Scstrings_MAIN_PROCESS_RUNNING_STOP_RESTERT_FIRST.c_str(),FProcessList->Strings[ SelItem->Index ].c_str());
					ShowMessage( Msg );
					return;
				}
				else
				{
					ChangeProcessState( SelItem->Index, PROCESS_STARTING );
					SendShellCMD( SPEEDY_RUN_PROCESS, SelItem->Index,true );
					Msg.printf( L"Start Process:%s",FProcessList->Strings[ SelItem->Index ].c_str());
					WriteUserLog( "war", Msg);
					return;
				}
			}
		}
		ListView_GetSubItemRect( ProcessListView->Handle, SelItem->Index, 6,LVIR_BOUNDS,&ItemRect);
		if( PtInRect( ItemRect, PT ))
		{
			Msg.printf( Scstrings_MAIN_PROCESS_STOP_ASK.c_str(),FProcessList->Strings[ SelItem->Index ].c_str());
			if( FormSetting->Confirm( Msg ) == true )
			{
				if( FProcessState[ SelItem->Index ] == PROCESS_STOPED )
				{
					Msg.printf( Scstrings_MAIN_PROCESS_ALREADY_STOPED.c_str(),FProcessList->Strings[ SelItem->Index ].c_str());
					ShowMessage( Msg );
					return;
				}
				else
				{
					ChangeProcessState( SelItem->Index, PROCESS_STOPING );
					SendShellCMD( SPEEDY_KILL_PROCESS, SelItem->Index,true );
					Msg.printf( L"Stop Process:%s", FProcessList->Strings[ SelItem->Index ].c_str());
					WriteUserLog("war",Msg);
					return;
				}
			}
        }
        ListView_GetSubItemRect( ProcessListView->Handle, SelItem->Index, 7,LVIR_BOUNDS,&ItemRect);
        if( PtInRect( ItemRect, PT ))
		{
			Msg.printf( Scstrings_MAIN_PROCESS_START_ASK.c_str(), FProcessList->Strings[ SelItem->Index ].c_str());
			if( FormSetting->Confirm( Msg ) == true )
			{
				ChangeProcessState( SelItem->Index, PROCESS_RESTARTING );
				SendShellCMD( SPEEDY_RESTART_PROCESS, SelItem->Index,true );
				Msg.printf(L"Restart Process:%s",FProcessList->Strings[ SelItem->Index ].c_str());
				WriteUserLog("war",Msg);
				return;
			}
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::OnFIXSessionBroken( TFIXSession* session )
{
    AnsiString Message;
    AnsiString Msg;

	Msg.printf("FIX session:%s broken!",session->GetSessionID().c_str());
	WriteUserLog("err",Msg);
	Message = "FIX Session: " +
              session->GetFIXVersion()   + " "    +
              session->GetSenderCompID() + " -> " +
			  session->GetTargetCompID() + " broken";
	WarningForm->ShowWarning( FSpeedyHost, Message, FormSetting->GetLineErrorSound() );
	WarningForm->LogError( FSpeedyHost, FSpeedyHostIP,"E05",Message );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ParseFIXStatus( TStringList* Status, UFCType::UInt32 TickNow )
{
    int Index;
    int TickDiff;
    int Outseq  = Status->Strings[ 3 ].ToInt();
	int inseq   = Status->Strings[ 4 ].ToInt();

	if( (Index = FFIXSessions->IndexOf( Status->Strings[ 0 ] ) )!= -1 )
    {
        TFIXSession* session = (TFIXSession*)FFIXSessions->Objects[ Index ];
        if( Status->Strings[ 1 ].ToInt() == 1 ) ///< FIX Session Enable/Disable.
            session->SetEnabled( true );
        else
            session->SetEnabled( false );
        if( Status->Strings[ 2 ].ToInt() == 1 ) ///< FIX Session Connected/Broken.
            session->SetState( SESSION_STATUS_ONLINE );
        else
        {
            if( session->GetState() == SESSION_STATUS_ONLINE )
                UpdateFIXStat( 1, Index );
            session->SetState( SESSION_STATUS_BROKEN );
        }
        session->SetOutboundSeq( Outseq );
        session->SetInboundSeq( inseq );
        if( FMonitorSession == session   && FMonitorFIX )
        {
            TickDiff = TickNow - FFIXRecvTick;
            if( FFIXRecvTick > 0 && TickDiff > 0 )
            {
                int inrate  = ( inseq - FInSeq  )*100000 / ( TickDiff );
                int outrate = ( Outseq- FOutSeq )*100000 / ( TickDiff );
                UpdateFIXPerformance( inrate, outrate );
            }
            FInSeq       = inseq;
            FOutSeq      = Outseq;
            FFIXRecvTick = TickNow;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::imFIXSubscriberMessage(const MString &Subject,
      const MString &Key, MTree *Tree)
{
	int ReplyVal;
    int Count;

    if( Tree->get( "COMMAND", ReplyVal )  )
    {
        if( ReplyVal == IMFIX_REPLY_SESSION_STATE )
        {
            if( Tree->get( "COUNT", Count ) )
            {
                MString StatusStr;
				UFCType::UInt32  Tick;

                if( Tree->get( "TICK", Tick ) == FALSE )
                    Tick = GetTickCount();
                if( Count == 0 )
                    UpdateFIXPerformance( 0, 0 );
                else
                {
                    for( int i = 1; i < Count; i++ )
                    {
                        MString Field( i );
                        if( Tree->get( Field, StatusStr ) )
						{
                            TStringList* Strs = new TStringList();
                            Strs->Delimiter = ',';
                            Strs->DelimitedText = StatusStr.c_str();
                            ParseFIXStatus( Strs, Tick );
                            delete Strs;
                        }
                    }
                    UpdateFIXStat( 0, 0 );
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FIXSessionsListViewCustomDrawSubItem(
      TCustomListView *Sender, TListItem *Item, int SubItem,
      TCustomDrawState State, bool &DefaultDraw)
{
	TRect        ItemRect;
	TColor       TextColor = clBlack;
	TFIXSession* Session = (TFIXSession*)FFIXSessions->Objects[ Item->Index ];
	int          StartTime = Session->GetStartTime();
	int          EndTime = Session->GetEndTime();
	String       DrawText,DayStr;

	DefaultDraw = false;
	ListView_GetSubItemRect( FIXSessionsListView->Handle, Item->Index, SubItem,LVIR_BOUNDS,&ItemRect);
	switch( SubItem )
	{
		case 1: DrawText = Session->GetFIXVersion();  TextColor = clBlack; break;
		case 2: DrawText = Session->GetSenderCompID();TextColor = clNavy;  break;
		case 3: DrawText = Session->GetTargetCompID();TextColor = clNavy;  break;
		case 4: if( Session->GetState() == SESSION_STATUS_UNKNOWN )
				{
					DrawText = L"---";
					TextColor = clGreen;
				}
				else
				{
					if( Session->IsEnabled() )
					{
						DrawText  = Scstrings_MAIN_SESSION_ENABLE;
						TextColor = clGreen;
					}
					else
					{
						DrawText  = Scstrings_MAIN_SESSION_DISABLE;
						TextColor = clRed;
					}
				}
				break;

		case 5: switch( Session->GetState() )
				{
					case SESSION_STATUS_ONLINE: DrawText = Scstrings_MAIN_SESSION_STAT_CONNECTED; TextColor = clGreen; break;
					case SESSION_STATUS_BROKEN: DrawText = Scstrings_MAIN_SESSION_STAT_BROKEN;    TextColor = clRed;   break;
					case SESSION_STATUS_LISTEN: DrawText = Scstrings_MAIN_SESSION_STAT_WAITING;   TextColor = clNavy;  break;
					case SESSION_STATUS_STOP:   DrawText = Scstrings_MAIN_SESSION_STAT_STOPED;    TextColor = clNavy;  break;
					case SESSION_STATUS_UNKNOWN:DrawText = L"---";      TextColor = clGreen; break;
				}
				break;
		case 6: DrawText = IntToStr(Session->GetOutboundSeq());break;
		case 7: DrawText = IntToStr(Session->GetInboundSeq());break;
		case 8: if( Session->GetStartDay() =="everyday" )
					DrawText.printf( L"%02d:%02d:%02d", StartTime/10000,(StartTime%10000)/100, StartTime%100 );
				else
				{
					DayStr =  Session->GetStartDay().SubString(1,3).UpperCase();
					DrawText.printf( L"%s:%02d:%02d:%02d", DayStr,StartTime/10000,(StartTime%10000)/100, StartTime%100 );
				}
				break;
		case 9: if( Session->GetEndDay() =="everyday" )
					DrawText.printf( L"%02d:%02d:%02d", EndTime/10000,(EndTime%10000)/100, EndTime%100 );
				else
				{
					DayStr =  Session->GetEndDay().SubString(1,3).UpperCase();
					DrawText.printf( L"%s:%02d:%02d:%02d", DayStr,EndTime/10000,(EndTime%10000)/100, EndTime%100 );
				}
				break;
	}
	if( Item->Selected )
	{
		Sender->Canvas->Brush->Color = clNavy;
		SetTextColor( FIXSessionsListView->Canvas->Handle,(COLORREF)clWhite );
	}
	else
	{
		if( Item->Index %2 )
			Sender->Canvas->Brush->Color = clSilver;
		else
			Sender->Canvas->Brush->Color = FIXSessionsListView->Color;
		SetTextColor( FIXSessionsListView->Canvas->Handle,(COLORREF)TextColor );
	}
	Sender->Canvas->FillRect( ItemRect );
	TTextFormat Formats;

	Formats<<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
	Sender->Canvas->TextRect( ItemRect, DrawText, Formats );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FIXSetSessionSeqMenuClick(TObject *Sender)
{
	TListItem* Item = FIXSessionsListView->Selected;
	if( Item != NULL )
	{
		TFIXSession*  Session = (TFIXSession*)FFIXSessions->Objects[Item->Index];
		SequenceForm = new TSequenceForm( this, Session->GetInboundSeq(),Session->GetOutboundSeq());
		if( SequenceForm->ShowModal() == mrOk)
		{
			MTree      Data;
			AnsiString SID = Session->GetSessionID();

			Data.append( "COMMAND", IMFIX_SET_SEQUENCE_NO );
			Data.append( "SID",     SID.c_str() );
			Data.append( "SSEQ",    SequenceForm->GetOutboundSeq() );
			Data.append( "TSEQ",    SequenceForm->GetInboundSeq());
			imFIXPublisher->SendData( &Data );
			AnsiString Msg;
			Msg.printf("Set FIX session:%s Inbound Seq:%d Outbound Seq:%d",SID.c_str(),SequenceForm->GetInboundSeq(),SequenceForm->GetOutboundSeq());
			WriteUserLog("war",Msg);
		}
		delete SequenceForm;
	}
    else
		ShowMessage( Scstrings_MAIN_FIX_SELECT_SET_SEQ_SESSION );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FIXSessionEndOfDayMenuClick(TObject *Sender)
{
	TListItem* Item = FIXSessionsListView->Selected;
	if( Item != NULL )
	{
		TFIXSession*  Session = (TFIXSession*)FFIXSessions->Objects[Item->Index];
		AnsiString SID =  Session->GetSessionID();
		if( MessageDlg( Scstrings_MAIN_FIX_ASK_END_OF_DAY + SID + L"?", mtConfirmation, TMsgDlgButtons() << mbOK<<mbCancel, 0) == mrOk )
		{
			MTree Data;
			AnsiString Msg;

			Data.append( "COMMAND", IMFIX_SESSION_EOD );
			Data.append( "SID",     SID.c_str() );
			imFIXPublisher->SendData( &Data );
			Msg.printf("FIX session:%s perform EOD.",SID.c_str());
			WriteUserLog("war",Msg);
		}
	}
	else
		ShowMessage( Scstrings_MAIN_FIX_SELECT_EOD_SESSION );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::NextConnectionMenuItemClick(TObject *Sender)
{
	TListItem* Item = FIXSessionsListView->Selected;
	if( Item != NULL )
	{
		TFIXSession*  Session = (TFIXSession*)FFIXSessions->Objects[Item->Index];
		AnsiString SID =  Session->GetSessionID();
		MTree Data;
		AnsiString Msg;

		Data.append( "COMMAND", IMFIX_RECONNECT_SESSION );
		Data.append( "SID",     SID.c_str() );
		imFIXPublisher->SendData( &Data );
		Msg.printf("FIX session:%s re-connect.",SID.c_str());
		WriteUserLog("war",Msg);
	}
	else
		ShowMessage( Scstrings_MAIN_FIX_SELECT_EOD_SESSION );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FIXBrokenSessionMenuClick(TObject *Sender)
{
	TListItem* Item = FIXSessionsListView->Selected;
	if( Item != NULL )
	{
		TFIXSession* Session = (TFIXSession*)FFIXSessions->Objects[Item->Index];
		AnsiString   SID     = Session->GetSessionID();
		MTree        Data;
		AnsiString   Msg;

		Data.append( "COMMAND", IMFIX_DISCONNECT_SESSION );
		Data.append( "SID",     SID.c_str() );
		imFIXPublisher->SendData( &Data );
		Msg.printf( "Force disconnect FIX session:%s",SID.c_str());
		WriteUserLog("war",Msg);
	}
	else
		ShowMessage( Scstrings_MAIN_FIX_SELECT_STOP_SESSION );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::N8Click(TObject *Sender)
{
	TListItem* Item = FIXSessionsListView->Selected;
	if( Item != NULL )
	{
		TFIXSession* Session = (TFIXSession*)FFIXSessions->Objects[Item->Index];
		AnsiString   SID     = Session->GetSessionID();
		MTree        Data;
		AnsiString   Msg;

        Data.append( "COMMAND", IMFIX_ENABLE_SESSION );
		Data.append( "SID",     SID.c_str() );
		imFIXPublisher->SendData( &Data );
		Msg.printf("Force disconnect FIX session:%s",SID.c_str());
		WriteUserLog("war",Msg);
    }
    else
		ShowMessage( Scstrings_MAIN_FIX_SELECT_ACTIVATE_SESSION );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::NewSessionButtonClick(TObject *Sender)
{
	if( FIXSetting == false )
	{
		MessageDlg( Scstrings_MAIN_FIX_NO_RIGHTS_ADD_SESSION, mtWarning, TMsgDlgButtons() << mbOK,0);
		return;
	}
	if( IsProcessRunning( FIX_NAME ) )
	{
		ShowMessage( Scstrings_MAIN_FIX_RUNNING_CAN_NOT_ADD_SESSION );
		return;
	}
	SessionForm = new TSessionForm( FFIXDicts );
	if( SessionForm->ShowModal() == mrOk )
	{
		bool IsAcceptor;
		TFIXSession* NewSession = SessionForm->CreateSession( IsAcceptor );
		if( FFIXSessions->IndexOf( NewSession->GetSessionID() ) == -1 )
		{
			FFIXSessions->AddObject( NewSession->GetSessionID(), NewSession );
			SaveSessionsToSettingFile();
			AnsiString Msg;
			Msg.printf("Add FIX session:%s",NewSession->GetSessionID().c_str());
			WriteUserLog("war",Msg);
		}
		else
		{
			String   Msg;

			Msg.printf( Scstrings_MAIN_FIX_ADD_FAILED_ALREADY_EXISTS.c_str(),String(NewSession->GetSessionID()) );
			ShowMessage( Msg );
			delete NewSession;
		}
	}
	delete SessionForm;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::SessionSettingButtonClick(TObject *Sender)
{
	if( ConnectionSetting == false )
	{
		MessageDlg( Scstrings_MAIN_FIX_NO_RIGHTS_ADD_SESSION, mtWarning, TMsgDlgButtons() << mbOK,0);
		return;
	}
	if( IsProcessRunning( FIX_NAME ) )
    {
		ShowMessage( Scstrings_MAIN_FIX_RUNNING_CAN_NOT_MODIFY_SESSION);
		return;
	}
	if( FIXSessionsListView->Selected != NULL )
	{
		TFIXSession* Modify = (TFIXSession*)FFIXSessions->Objects[ FIXSessionsListView->Selected->Index ];
		SessionForm = new TSessionForm( Modify, FFIXDicts );
		if( SessionForm->ShowModal() == mrOk )
		{
			SessionForm->UpdateSession();
			SaveSessionsToSettingFile();
			AnsiString Msg;
			Msg.printf("Modify FIX session:%s",Modify->GetSessionID().c_str());
			WriteUserLog("war",Msg);
		}
		delete SessionForm;
	}
	else
		MessageDlg( Scstrings_MAIN_FIX_SELECT_MODIFY_SESSION, mtWarning, TMsgDlgButtons() << mbOK,0);

}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::DeleteSessionButtonClick(TObject *Sender)
{
	if( ConnectionSetting == false )
	{
		MessageDlg( Scstrings_MAIN_FIX_NO_RIGHTS_ADD_SESSION, mtWarning, TMsgDlgButtons() << mbOK,0);
		return;
	}
	if( IsProcessRunning( FIX_NAME ) )
    {
		ShowMessage( Scstrings_MAIN_FIX_RUNNING_CAN_NOT_DEL_SESSION );
		return;
	}
	if( FIXSessionsListView->Selected != NULL )
	{
		int          ListViewIndex = FIXSessionsListView->Selected->Index;
		TFIXSession* del = (TFIXSession*)FFIXSessions->Objects[ ListViewIndex ];
		String       Msg;

		Msg.printf( Scstrings_MAIN_FIX_ASK_DELETE_SESSION.c_str(), String( del->GetSessionID() ));
		if( MessageDlg( Msg , mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0 ) == mrOk )
		{
			FIXSessionsListView->DeleteSelected();
			FFIXSessions->Delete( ListViewIndex );
			AnsiString Msg;
			Msg.printf("Delete FIX session:%s",del->GetSessionID().c_str());
			WriteUserLog("war",Msg);
			delete del;
			SaveSessionsToSettingFile();
		}
	}
	else
		MessageDlg( Scstrings_MAIN_FIX_SELECT_DEL_SESSION, mtWarning, TMsgDlgButtons() << mbOK,0);
}
//---------------------------------------------------------------------------
//
// Functions to Add/Modify/Delete PVC lines.
//
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::MakeLineKey( TPVCSettingForm* Setting, AnsiString& Key )
{
	TMarket Market = Setting->GetMarket();

	if( Market == mtOTC || Market == mtStock )
		Key.printf("%s:%d:%s:%d",Setting->GetFCM(), Setting->GetLink(), Setting->GetPVCID(),(int)Market );
	else
		Key.printf("%d:%d:%d:%d",Setting->GetLink(), Setting->GetPVC(), (int)Market, (int)Setting->GetIsOffHour() );
}
//---------------------------------------------------------------------------
bool __fastcall TSimTFXForm::LineExists( TPVCSettingForm* Setting )
{
	AnsiString FindKey;

	MakeLineKey( Setting, FindKey );
	if( FLinkPVCToPanel->IndexOf( FindKey ) != -1 )
		return true;
	return false;
}
//---------------------------------------------------------------------------
/*bool __fastcall TSimTFXForm::LineExists( TMarket Market, int Link, int PVC )
{
	AnsiString FindName = "Link" +IntToStr( Link ) + "PVC" + IntToStr( PVC )+"Market" + IntToStr( Market );

	if( FLinkPVCToPanel->IndexOf( FindName ) != -1 )
		return true;
	return false;
} */
//---------------------------------------------------------------------------
int __fastcall TSimTFXForm::GetInsertPos( TMarket Market )
{
    TLineInfo* line;

	if( Market == mtOTC )
	{
		///< Insert OTC line at the end of the list.
		return FLinkPVCToPanel->Count;
	}
	else if( Market == mtStock )
	{
		///< Insert Stock line at the begin of the OTC list.
		for( int i = FLinkPVCToPanel->Count-1; i >= 0; i-- )
		{
			 line = (TLineInfo*)FLinkPVCToPanel->Objects[i];
			 if( line->GetSystem() != mtOTC )
				 return i + 1;
		}
	}
	else if( Market == mtOption )
	{
		///< Insert Option line at the end of the Options lines.
		for( int i = 0; i < FLinkPVCToPanel->Count;i++ )
		{
			 line = (TLineInfo*)FLinkPVCToPanel->Objects[i];
			 if( line->GetSystem() != mtOption )
				 return i;
		}
	}
	else if( Market == mtFutures )
	{
		 ///< Insert Futures line at the end of the  lines.
		for( int i = FLinkPVCToPanel->Count-1; i >=0; i-- )
		{
			 line = (TLineInfo*)FLinkPVCToPanel->Objects[i];
			 if( line->GetSystem() == mtFutures )
				 return i+1;
		}
		return FLinkPVCToPanel->Count;
	}
	return 0;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::NewPVCButtonClick(TObject *Sender)
{
	if( IsProcessRunning( SPEEDY_FUT_NAME ) || IsProcessRunning( SPEEDY_OPT_NAME ) || IsProcessRunning( SPEEDY_TFX_NAME ) ||
		IsProcessRunning( SPEEDY_OTC_NAME ) || IsProcessRunning( SPEEDY_STOCK_NAME ) || IsProcessRunning( SPEEDY_TW_NAME ) )
	{
		MessageDlg( Scstrings_MAIN_SPEEDY_RUNNING_CANT_ADD_SESSION, mtWarning, TMsgDlgButtons() << mbOK,0);
		return;
	}
	if( ConnectionSetting == false )
	{
		MessageDlg( Scstrings_MAIN_SPEEDY_NO_RIGHTS_ADD_SESSION, mtWarning, TMsgDlgButtons() << mbOK,0);
		return;
	}
	PVCSettingForm = new TPVCSettingForm( this, FMarketSet );
	if( PVCSettingForm->ShowModal() == mrOk )
	{
		if( LineExists( PVCSettingForm ) == true )
		{
			MessageDlg( Scstrings_MAIN_SPEEDY_ADD_FAILED_ALREADY_EXISTS, mtWarning, TMsgDlgButtons() << mbOK,0);
		}
		else
		{
			int        Pos = GetInsertPos( PVCSettingForm->GetMarket());
			AnsiString LineName;

			MakeLineKey( PVCSettingForm, LineName );
			TLineInfo* Line  = new TLineInfo( PVCSettingForm->GetFCM(),
											  PVCSettingForm->GetPWD(),
											  PVCSettingForm->GetLink(),
											  PVCSettingForm->GetPVC(),
											  PVCSettingForm->GetAPCode(),
											  PVCSettingForm->GetMarket(),
											  PVCSettingForm->GetGroup(),
											  PVCSettingForm->GetCM(),
											  PVCSettingForm->GetLocalPort(),
											  PVCSettingForm->GetPVCID(),
											  PVCSettingForm->GetIsEnabled());
			Line->SetThroughput( PVCSettingForm->GetThreougput()  );											  										  ;
			Line->SetOffHour( PVCSettingForm->GetIsOffHour() );
			FLinkPVCToPanel->InsertObject( Pos, LineName, Line );
			SystemComboBoxChange( NULL ); ///< Filter PVC
			AnsiString Msg;
			Msg.printf("Add Link:%d PVC:%d APCode:%d",PVCSettingForm->GetLink(),PVCSettingForm->GetPVC(),PVCSettingForm->GetAPCode());
			WriteUserLog("war",Msg);
			RequestUpdatePVCList( PVCSettingForm->GetMarket());
			RequestPVCSettingList();
		}
	}
	delete PVCSettingForm;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ModifyPVCButtonClick(TObject *Sender)
{
	if( IsProcessRunning( SPEEDY_FUT_NAME ) || IsProcessRunning( SPEEDY_OPT_NAME ) || IsProcessRunning( SPEEDY_TFX_NAME ) ||
		IsProcessRunning( SPEEDY_OTC_NAME ) || IsProcessRunning( SPEEDY_STOCK_NAME ) || IsProcessRunning( SPEEDY_TW_NAME ) )
	{
		MessageDlg( Scstrings_MAIN_SPEEDY_RUNNING_CANT_ADD_SESSION, mtWarning, TMsgDlgButtons() << mbOK,0);
		return;
	}
	if( ConnectionSetting == false )
	{
		MessageDlg( Scstrings_MAIN_SPEEDY_NO_RIGHTS_MODIFY_SESSION, mtWarning, TMsgDlgButtons() << mbOK,0);
		return;
	}
	if( LineListBox->ItemIndex != -1 )
	{
		TLineInfo* Modify = (TLineInfo*)LineListBox->Items->Objects[ LineListBox->ItemIndex ];
		PVCSettingForm = new TPVCSettingForm( this, Modify,FMarketSet );

		if( PVCSettingForm->ShowModal() == mrOk && PVCSettingForm->IsModified() )
		{
			TMarket Market = Modify->GetSystem();
			int     Port = PVCSettingForm->GetLink();
			int     PVC  = PVCSettingForm->GetPVC();

			if( PVCSettingForm->IsConnectionModified() && LineExists( PVCSettingForm ) )
			{
				MessageDlg( Scstrings_MAIN_SPEEDY_MODIFY_FAILED_ALREADY_EXISTS, mtWarning, TMsgDlgButtons() << mbOK,0);
			}
			else
			{
				AnsiString NewName;

				MakeLineKey( PVCSettingForm, NewName );
				FLinkPVCToPanel->Strings[ DisplayIndexToAllIndex( LineListBox->ItemIndex ) ] = NewName;
				Modify->SetBrokerID( PVCSettingForm->GetFCM() );
				Modify->SetPassword( PVCSettingForm->GetPWD() );
				Modify->SetLink( Port );
				Modify->SetPVC( PVC );
				Modify->SetGroup( PVCSettingForm->GetGroup() );
				Modify->SetIsEnabled( PVCSettingForm->GetIsEnabled());
				if( Modify->GetSystem() == mtStock || Modify->GetSystem() == mtOTC ) ///< Stock system support TCP/IP
				{
					Modify->SetIP( PVCSettingForm->GetCM() );
					Modify->SetLocalPort( PVCSettingForm->GetLocalPort());
					Modify->SetPVCID( PVCSettingForm->GetPVCID() );
					Modify->SetThroughput( PVCSettingForm->GetThreougput()  );
				}
				else
				{
					Modify->SetCMID( PVCSettingForm->GetCM() );
					Modify->SetIP( PVCSettingForm->GetPVCID() );
					Modify->SetOffHour( PVCSettingForm->GetIsOffHour() );
				}
				AnsiString Msg;
				Msg.printf("Modify Link:%d PVC:%d APCode:%d",PVCSettingForm->GetLink(),PVCSettingForm->GetPVC(),PVCSettingForm->GetAPCode());
				WriteUserLog("war",Msg);
				RequestUpdatePVCList( Modify->GetSystem() );
				RequestPVCSettingList();
            }
		}
        delete PVCSettingForm;
	}
	else
		MessageDlg( Scstrings_MAIN_SPEEDY_SELECT_MODIFY_SESSION, mtWarning, TMsgDlgButtons() << mbOK,0);
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::DeletePVCButtonClick(TObject *Sender)
{
	if( IsProcessRunning( SPEEDY_FUT_NAME ) || IsProcessRunning( SPEEDY_OPT_NAME ) || IsProcessRunning( SPEEDY_TFX_NAME ) ||
		IsProcessRunning( SPEEDY_OTC_NAME ) || IsProcessRunning( SPEEDY_STOCK_NAME ) || IsProcessRunning( SPEEDY_TW_NAME ) )
	{
		MessageDlg( Scstrings_MAIN_SPEEDY_RUNNING_CANT_ADD_SESSION, mtWarning, TMsgDlgButtons() << mbOK,0);
		return;
	}
	if( ConnectionSetting == false )
	{
		MessageDlg( Scstrings_MAIN_SPEEDY_NO_RIGHTS_DEL_SESSION, mtWarning, TMsgDlgButtons() << mbOK,0);
		return;
	}
	int DelIndex = LineListBox->ItemIndex;
	if( DelIndex != -1 )
	{
		TLineInfo* Delete = (TLineInfo*)LineListBox->Items->Objects[ DelIndex ];
		if( MessageDlg( Scstrings_MAIN_SPEEDY_ASK_DELETE_SESSION +IntToStr(Delete->GetLink())+L" PVC:"+IntToStr(Delete->GetPVC())+L"?", mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0 ) == mrOk )
		{
			TMarket    Market = Delete->GetSystem();
			AnsiString Msg;

			FLinkPVCToPanel->Delete( DisplayIndexToAllIndex( DelIndex ) );
			SystemComboBoxChange( NULL ); ///< Filter PVC
			Msg.printf( "Modify Port:%d PVC:%d APCode:%d",Delete->GetLink(),Delete->GetPVC(),Delete->GetAPCode());
			WriteUserLog( "war", Msg );
			delete Delete;
			RequestUpdatePVCList( Market );
			RequestPVCSettingList();
		}
	}
	else
		MessageDlg( Scstrings_MAIN_SPEEDY_SELECT_DEL_SESSION, mtWarning, TMsgDlgButtons() << mbOK,0);
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::CopySettingMenuItemClick(TObject *Sender)
{
	if( IsProcessRunning( SPEEDY_FUT_NAME ) || IsProcessRunning( SPEEDY_OPT_NAME ) || IsProcessRunning( SPEEDY_TFX_NAME ) ||
		IsProcessRunning( SPEEDY_OTC_NAME ) || IsProcessRunning( SPEEDY_STOCK_NAME ) || IsProcessRunning( SPEEDY_TW_NAME ) )
	{
		MessageDlg( Scstrings_MAIN_SPEEDY_RUNNING_CANT_ADD_SESSION, mtWarning, TMsgDlgButtons() << mbOK,0);
		return;
	}
	if( LineListBox->ItemIndex != -1 )
	{
		TLineInfo* Modify = (TLineInfo*)LineListBox->Items->Objects[ LineListBox->ItemIndex ];
		PVCSettingForm = new TPVCSettingForm( this, Modify,FMarketSet, true );
		if( PVCSettingForm->ShowModal() == mrOk )
		{
			if( LineExists( PVCSettingForm ) )
			{
				MessageDlg( Scstrings_MAIN_SPEEDY_ADD_FAILED_ALREADY_EXISTS, mtWarning, TMsgDlgButtons() << mbOK,0);
			}
			else
			{
				int        Pos = GetInsertPos( PVCSettingForm->GetMarket());
				AnsiString LineName;

				MakeLineKey( PVCSettingForm, LineName );
				TLineInfo* Line  = new TLineInfo( PVCSettingForm->GetFCM(),
												  PVCSettingForm->GetPWD(),
												  PVCSettingForm->GetLink(),
												  PVCSettingForm->GetPVC(),
												  PVCSettingForm->GetAPCode(),
												  PVCSettingForm->GetMarket(),
												  0,
												  PVCSettingForm->GetCM(),
												  PVCSettingForm->GetLocalPort(),
												  PVCSettingForm->GetPVCID() );
				Line->SetThroughput( PVCSettingForm->GetThreougput()  );											  										  ;
				FLinkPVCToPanel->InsertObject( Pos, LineName, Line );
				SystemComboBoxChange( NULL ); ///< Filter PVC
				AnsiString Msg;
				Msg.printf("Add Link:%d PVC:%d APCode:%d",PVCSettingForm->GetLink(),PVCSettingForm->GetPVC(),PVCSettingForm->GetAPCode());
				WriteUserLog("war",Msg);
				RequestUpdatePVCList( PVCSettingForm->GetMarket());
				RequestPVCSettingList();
			}
		}
		delete PVCSettingForm;
	}
	else
		MessageDlg( Scstrings_MAIN_SPEEDY_SELECT_COPY_SESSION, mtWarning, TMsgDlgButtons() << mbOK,0);
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::SaveCFGFile( TMarket Market, TStream* Stream )
{
	TStringList* CFGStrings = new TStringList();
	TLineInfo*   lineInfo;

	for( int i = 0; i < FLinkPVCToPanel->Count;i++ )
	{
		 AnsiString lineCfg;

		 lineInfo = (TLineInfo*) FLinkPVCToPanel->Objects[i];
		 if( lineInfo->GetSystem() == Market )
		 {
			 if( Market == mtStock || Market == mtOTC )
			 {
				 if( lineInfo->GetPVC() == 99 ) ///< FIX Session
				 {
					 lineCfg.printf("%d,%d,%d,%s,%s,%s,%s,%d,%d,%d,%d", lineInfo->GetLink(),
																		lineInfo->GetPVC(),
																		lineInfo->GetAPCode(),
																		lineInfo->GetBrokerID().c_str(),
																		lineInfo->GetPassword().c_str(),
																		lineInfo->GetPVCID().c_str(),
																		lineInfo->GetIP().c_str(),
																		lineInfo->GetLocalPort(),
																		lineInfo->GetGroup(),    ///< Group
																		lineInfo->IsEnabled(),    ///< Enabled
																		lineInfo->GetThroughput() ); ///< Throughput
				 }
				 else  ///< TMP Session
				 {
					 if( lineInfo->GetAPCode() == 10 )
						 lineCfg.printf("%d,%d,C,%s,%s,%s,%s,%d,%d,%d",    lineInfo->GetLink(),
																		   lineInfo->GetPVC(),
																		   lineInfo->GetBrokerID().c_str(),
																		   lineInfo->GetPassword().c_str(),
																		   lineInfo->GetPVCID().c_str(),
																		   lineInfo->GetIP().c_str(),
																		   lineInfo->GetLocalPort(),
																		   lineInfo->GetGroup(),    ///< Group
																		   lineInfo->IsEnabled());   ///< Enabled
					 else
						 lineCfg.printf("%d,%d,%d,%s,%s,%s,%s,%d,%d,%d",   lineInfo->GetLink(),
																		   lineInfo->GetPVC(),
																		   lineInfo->GetAPCode(),
																		   lineInfo->GetBrokerID().c_str(),
																		   lineInfo->GetPassword().c_str(),
																		   lineInfo->GetPVCID().c_str(),
																		   lineInfo->GetIP().c_str(),
																		   lineInfo->GetLocalPort(),
																		   lineInfo->GetGroup(),    ///< Group
																		   lineInfo->IsEnabled());   ///< Enabled
				 }
			 }
			 else
			 {
				lineCfg.printf("%d,%03d,%d,%s,%s,%s,%d,%d,%d\n",lineInfo->GetLink(),
																lineInfo->GetPVC(),
																lineInfo->GetAPCode(),
																lineInfo->GetBrokerID().c_str(),
																lineInfo->GetPassword().c_str(),
																lineInfo->GetIP().c_str(),
																lineInfo->GetGroup(),    ///< Group
																lineInfo->IsEnabled(),   ///< Enabled
																lineInfo->GetOffHour()); ///< OffHour session
			 }
			 CFGStrings->Add( lineCfg );
		 }
	}
	CFGStrings->SaveToStream( Stream );
	delete CFGStrings;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::RequestUpdatePVCList( TMarket Market )
{
	MTree Data;
	TMemoryStream* MemCgf = new TMemoryStream();

	SaveCFGFile( Market, MemCgf );
	ClearAllLines();
	Data.append( "COMMAND", SPEEDY_MODIFY_PVC );
	switch( Market )
	{
		case mtFutures: Data.append( "FUT",(unsigned char*)MemCgf->Memory, MemCgf->Size );break;
		case mtOption:  Data.append( "OPT",(unsigned char*)MemCgf->Memory, MemCgf->Size );break;
		case mtStock:   Data.append( "TSE",(unsigned char*)MemCgf->Memory, MemCgf->Size );break;
		case mtOTC:     Data.append( "OTC",(unsigned char*)MemCgf->Memory, MemCgf->Size );break;
	}
	Data.append( "HOST", FSpeedyHost.c_str());
	SpeedyAgentPublisher->SendData( &Data );
    delete MemCgf;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::MonitorButtonClick(TObject *Sender)
{
	MTree           Data;
	TLineInfo*      Line = (TLineInfo*)FLinkPVCToPanel->Objects[ LineComboBox->ItemIndex ];
	AnsiString      LineTxt = LineComboBox->Text;
	UFC::AnsiString PVCStr( LineTxt.c_str(),16 );

	MonitorButton->Enabled = false;
	LogListBox->Items->Clear();
	Data.append( "COMMAND", SPEEDY_MONITER_PVC );
	Data.append( "CMPSTR",  PVCStr );
	Data.append( "PVC",     Line->GetPVC() );
	Data.append( "PVCID",   Line->GetPVCID().c_str() );
	Data.append( "BROKERID",Line->GetBrokerID().c_str() );
	Data.append( "LINK", Line->GetLink() );
	Data.append( "SYS",     (UFCType::Int32)Line->GetSystem() );
	Data.append( "AP",      Line->GetAPCode() );
	Data.append( "HOST", FSpeedyHost.c_str());
	Data.append( "OFFHOUR", (UFCType::Int32)Line->GetOffHour());
	SpeedyAgentPublisher->SendData( &Data );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::LogListBoxDrawItem(TWinControl *Control,
      int Index, TRect &Rect, TOwnerDrawState State)
{
	AnsiString OutText = LogListBox->Items->Strings[ Index ];
	FListBuffer->Width  = Rect.Width();
	FListBuffer->Height = Rect.Height();
	RECT TextRect = { 1,1, FIX_MSG_WIDTH-2, FListBuffer->Height-2};

	FListBuffer->Canvas->Pen->Color = clBlack;
	FListBuffer->Canvas->Rectangle( 0, 0, FListBuffer->Width, FListBuffer->Height );
	FListBuffer->Canvas->Pen->Color = clGray;
	FListBuffer->Canvas->MoveTo( 0, FListBuffer->Height-2 );
	FListBuffer->Canvas->LineTo( FListBuffer->Width, FListBuffer->Height-2 );
	if( OutText.AnsiPos( "Recv" ) )
		FListBuffer->Canvas->Font->Color  = clLime;  ///< Receive
	else
		FListBuffer->Canvas->Font->Color  = clWhite; ///< Send
	DrawTextA( FListBuffer->Canvas->Handle,
			   OutText.c_str(),
			   OutText.Length(),
			   &TextRect, DT_LEFT|DT_WORDBREAK );
	LogListBox->Canvas->Draw( Rect.Left, Rect.Top, FListBuffer );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::SaveLogButtonClick(TObject *Sender)
{
    if( SaveDialog->Execute() )
		LogListBox->Items->SaveToFile( SaveDialog->FileName );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FTRequestButtonClick(TObject *Sender)
{
	LockDownloadControls( true );
	try
    {
        MTree Data;
		CreateFtpMTreeData( &Data );
	}
    catch( ... )
    {
		LockDownloadControls( false );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FileListViewSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
    if( Selected )
    {
		if( Item->SubItems->Strings[ 0 ] == Scstrings_MAIN_FUTURES )
        {
            if( SysComboBox->ItemIndex != 0 )
            {
                SysComboBox->ItemIndex = 0;
                SysComboBoxChange( this );
            }
        }
        else
        {
            if( SysComboBox->ItemIndex != 1 )
            {
                SysComboBox->ItemIndex = 1;
                SysComboBoxChange( this );
            }
        }
        FCodeComboBox->ItemIndex = FCodeComboBox->Items->IndexOf( Item->Caption );
    }
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::CreateFtpMTreeData( MTree* pData )
{
    if( pData )
	{
		if( this->ResendPageControl->ActivePage == this->X25TabSheet )
		{
			AnsiString FileCode = FCodeComboBox->Text;

			pData->append( "FileCode", FileCode.c_str() );
			pData->append( "StartSeq", 0 );
			pData->append( "EndSeq",   0 );
			if( SysComboBox->ItemIndex == 0 )
				SpeedyFutFtpPublisher->SendData( pData );
			else
				SpeedyOptFtpPublisher->SendData( pData );
		}
		else
		{
			AnsiString FileCode = FCodeComboBox->Text;

			pData->append( "Body", FileCode.c_str() );
			pData->append( "ReqID", (UFCType::Int32)GetTickCount() );
			if( FileCode.Length() == 4 )
				pData->append( "FLEX", 1 );
			if( SessionComboBox->ItemIndex == 0 )
				pData->append( "TradeKind", 0 );
			else
				pData->append( "TradeKind", 1 );
			if( SysComboBox->ItemIndex == 0 )
			{
				ChangeFileListViewStatus( FCodeComboBox->Text, FTP_WAITING, false );
				SpeedyFutFtpPublisher->SendData( pData );
			}
			else
			{
				ChangeFileListViewStatus( FCodeComboBox->Text, FTP_WAITING, true );
				SpeedyOptFtpPublisher->SendData( pData );
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::LockDownloadControls( bool Value )
{
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::SpeedyFutFtpSubscriberMessage(
	  const MString &Subject, const MString &Key, MTree *Tree)
{
	MString FileCode;
	int     StatusCode;

	if( Tree->get( "FileCode", FileCode ) && Tree->get( "StatusCode", StatusCode ) )
		ChangeFileListViewStatus( FileCode.c_str(), StatusCode, false );

	if( Tree->get( "FileCode", FileCode ) && Tree->get( "ErrorCode", StatusCode ) )
		ChangeFileListViewStatus( FileCode.c_str(), StatusCode, false );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::SpeedyOptFtpSubscriberMessage(
	  const MString &Subject, const MString &Key, MTree *Tree)
{
	MString FileCode;
	int     StatusCode;

	if( Tree->get( "FileCode", FileCode ) && Tree->get( "StatusCode", StatusCode ) )
		ChangeFileListViewStatus( FileCode.c_str(), StatusCode, true );

	if( Tree->get( "FileCode", FileCode ) && Tree->get( "ErrorCode", StatusCode ) )
		ChangeFileListViewStatus( FileCode.c_str(), StatusCode, true );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ChangeFileListViewStatus( const AnsiString& FileCode, int StatusCode, bool IsOpt )
{
	TListItem* pItem;

	if( IsOpt == true )
	{
		for( int i = FileListView->Items->Count - 1; i >= 0 ; i-- )
		{
			pItem = FileListView->Items->Item[ i ];
			if( pItem->Caption.Compare( FileCode ) == 0 )
			{
				pItem->SubItems->Strings[2] = GetStatusStringByStatusCode( StatusCode );
				return;
			}
		}
	}
	else
	{
		for( int i = 0; i < FileListView->Items->Count; i++ )
		{
			pItem = FileListView->Items->Item[ i ];
			if( pItem->Caption.Compare( FileCode ) == 0 )
			{
				pItem->SubItems->Strings[2] = GetStatusStringByStatusCode( StatusCode );
				return;
			}
		}
	}
	if( FileCode == "B30" || FileCode == "XB30" )
		B30StatusLabel->Caption = Scstrings_MAIN_STATUS + GetStatusStringByStatusCode( StatusCode );
}
//---------------------------------------------------------------------------
String __fastcall TSimTFXForm::GetStatusStringByStatusCode( int StatusCode )
{
	if( StatusCode > FTP_NORMAL_RESPONSE )
		LockDownloadControls( false );

	switch( StatusCode )
	{
		//DocDef
		case FTP_NORMAL_RESPONSE:           return Scstrings_MAIN_FTP_NORMAL_RESPONSE;//"正常回應";//"normal response";
		case SFTP_ILLEGAL_FILE_CODE:
		case FTP_ILLEGAL_FILE_CODE:         return Scstrings_MAIN_FTP_ILLEGAL_FILE_CODE;//"錯誤的File Code";//"illegal file code";
		case FTP_ILLEGAL_EOF_VALUE:         return Scstrings_MAIN_FTP_ILLEGAL_EOF_VALUE;//"錯誤的EOF值";//"illegal EOF value";
		case FTP_ILLEGAL_FILE_SIZE:         return Scstrings_MAIN_FTP_ILLEGAL_FILE_SIZE;//"錯誤的檔案大小";//"illegal file size";
		case FTP_TIMING_ERROR:              return Scstrings_MAIN_FTP_TIMING_ERROR;//"錯誤的傳輸時機";//"timing error";
		case SFTP_FILE_IS_NOT_READY:
		case FTP_FILE_IS_NOT_READY:         return Scstrings_MAIN_FTP_FILE_IS_NOT_READY;//"檔案尚未準備好";//"file is not ready";
		case SFTP_FILE_IS_READY_BUT_EMPTY:
		case FTP_FILE_IS_READY_BUT_EMPTY:   return Scstrings_MAIN_FTP_FILE_IS_READY_BUT_EMPTY;//"檔案已準備好但是是空檔";//"file is ready but empty";
		case FTP_ABORT_BY_INITIATOR:        return Scstrings_MAIN_FTP_ABORT_BY_INITIATOR;//"被使用者中斷";//"abort by initiator";
		case FTP_DUPLICATE_ACCESS_REQUEST:  return Scstrings_MAIN_FTP_DUPLICATE_ACCESS_REQUEST;//"重複的下載請求";//"duplicate access request";
		case FTP_ILLEGAL_SUBSYSTEM_NAME:    return Scstrings_MAIN_FTP_ILLEGAL_SUBSYSTEM_NAME;//"錯誤的系統別";//"illegal subsystem name";
		case FTP_ILLEGAL_FUNCTION_CODE:     return Scstrings_MAIN_FTP_ILLEGAL_FUNCTION_CODE;//"錯誤的 Function Code";//"illegal function code";
		case FTP_ILLEGAL_MESSAGE_TYPE:      return Scstrings_MAIN_FTP_ILLEGAL_MESSAGE_TYPE;//"錯誤的 Message Type";//"illegal message type";
		case FTP_ILLEGAL_MESSAGE_TIME:      return Scstrings_MAIN_FTP_ILLEGAL_MESSAGE_TIME;//"錯誤的 Message Time";//"illegal message time";
		case FTP_ILLEGAL_STATUS_CODE:       return Scstrings_MAIN_FTP_ILLEGAL_STATUS_CODE;//"錯誤的狀態碼";//"illegal status code";
		case FTP_ILLEGAL_SOURCE_ID:         return Scstrings_MAIN_FTP_ILLEGAL_SOURCE_ID;//"錯誤的Source ID";//"illegal source id";
		case FTP_ILLEGAL_OBJECT_ID:         return Scstrings_MAIN_FTP_ILLEGAL_OBJECT_ID;//"錯誤的ID";//"illegal object id";
		case FTP_ILLEGAL_BODY_LENGTH:       return Scstrings_MAIN_FTP_ILLEGAL_BODY_LENGTH;//"錯誤的資料長度";//"illegal body length";
		case FTP_INTERNAL_ERROR:            return Scstrings_MAIN_FTP_INTERNAL_ERROR;//"期交所內部錯誤";//"internal error";
		case FTP_CALL_COMPUTER_CENTER:      return Scstrings_MAIN_FTP_CALL_COMPUTER_CENTER;//"重大錯誤!連絡期交所";//"CALL COMPUTER CENTER";
		case FTP_TMP_TOO_MANEY_REQ:	 		return Scstrings_MAIN_FTP_TMP_TOO_MANEY_REQ;//"檔案請求太頻繁";


		case SFTP_ILLEGAL_SEQ_VALUE:        return L"要求檔案序號範圍錯誤";
		case SFTP_FILE_BROKER_NOT_READY:    return L"該期貨商之檔案不存在";
		//CustDef
		case FTP_WAITING:                   return Scstrings_MAIN_FTP_WAITING;//"等待中";//"waiting";
		case FTP_JOB_EXISTS:                return Scstrings_MAIN_FTP_JOB_EXISTS;//"下載請求已存在";
		case FTP_DOWNLOAD_FINISHED:         return Scstrings_MAIN_FTP_DOWNLOAD_FINISHED;//"完成";//"finished";
		case TMP_FILE_NOT_EXISTS:           return Scstrings_MAIN_TMP_FILE_NOT_EXISTS;//"檔案不存在";
		case TMP_READY_NOT_EXISTS:          return Scstrings_MAIN_TMP_READY_NOT_EXISTS;//".ready檔不存在";
	}
	return Scstrings_MAIN_FTP_UNKNOWN_ERROR;//"不明錯誤";//"unknown error";
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::MonitorMenuItemClick(TObject *Sender)
{
	if( LineListBox->ItemIndex != -1 )
    {
        PageControl->ActivePage = LogSheet;
		LineComboBox->ItemIndex = DisplayIndexToAllIndex( LineListBox->ItemIndex );
		MonitorButtonClick( this );
	}
	else
		MessageDlg( Scstrings_MAIN_SPEEDY_SELECT_MONITOR_SESSION, mtWarning, TMsgDlgButtons() << mbOK,0);
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::EnablePVC( TLineInfo* Line, bool Enabled, bool COD )
{
	MTree Data;

	Data.append( "COMMAND", AGENT_PVC_ENABLED );
	Data.append( "SYS",     (int)Line->GetSystem() );
	Data.append( "LINK",    Line->GetLink() );
	Data.append( "PVC",     Line->GetPVC() );
	Data.append( "PVCID",   Line->GetPVCID().c_str() );
	Data.append( "BROKERID",Line->GetBrokerID().c_str() );
	Data.append( "ENABLE", (int)Enabled );
	Data.append( "COD", (int)COD );
	Data.append( "OFFHOUR",(int)Line->GetOffHour());
	AgentPublisher->SendData( &Data );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::DisablePVCMenuItemClick(TObject *Sender)
{
	if( LineListBox->ItemIndex != -1 )
	{
		bool COD = false;

		if( Sender == CODPVCMenuItem )
			COD = true;
		if( LineListBox->SelCount == 1 )
		{
			if( FormSetting->Confirm( Scstrings_MAIN_SPEEDY_ASK_STOP_SESSION ) == true )
			{
				TLineInfo* Line = (TLineInfo*)LineListBox->Items->Objects[ LineListBox->ItemIndex ];
				int Code = Line->GetStateCode()%100;
				if( Code == PVC_STATUS_DISABLE || Code == PVC_STATUS_DISABLEING )
					MessageDlg( Scstrings_MAIN_SPEEDY_SESSION_ALREADY_STOPED, mtWarning, TMsgDlgButtons() << mbOK,0);
				else
					EnablePVC( Line, false, COD );
			}
		}
		else
		{
			if( FormSetting->Confirm( Scstrings_MAIN_SPEEDY_ASK_STOP_SEL_SESSIONS ) == true )
			{
				for( int i = 0; i < LineListBox->Count; i++ )
				{
					if( LineListBox->Selected[ i ] == true )
					{
						TLineInfo* Line = (TLineInfo*)LineListBox->Items->Objects[i];
						int Code = Line->GetStateCode()%100;
						if( Code != PVC_STATUS_DISABLE && Code != PVC_STATUS_DISABLEING )
							EnablePVC( Line, false, COD );
					}
				}
			}
		}
	}
	else
		MessageDlg( Scstrings_MAIN_SPEEDY_SELECT_STOP_SESSION, mtWarning, TMsgDlgButtons() << mbOK,0);
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::EnablePVCMenuItemClick(TObject *Sender)
{
	if( LineListBox->ItemIndex != -1 )
	{
		if( LineListBox->SelCount == 1 )
		{
			if( FormSetting->Confirm( Scstrings_MAIN_SPEEDY_ASK_ACTIVATE_SESSION ) == true )
			{
				TLineInfo* Line = (TLineInfo*)LineListBox->Items->Objects[ LineListBox->ItemIndex ];
				int Code = Line->GetStateCode()%100;
				if( Code != PVC_STATUS_DISABLE )
					MessageDlg( Scstrings_MAIN_SPEEDY_SESSION_NOT_STOPED, mtWarning, TMsgDlgButtons() << mbOK,0);
				else
					EnablePVC( Line, true, false );
			}
		}
		else
		{
			if( FormSetting->Confirm( Scstrings_MAIN_SPEEDY_ASK_ACTIVATE_SEL_SESSIONS ) == true )
			{
				for( int i = 0; i < LineListBox->Count; i++ )
				{
					if( LineListBox->Selected[ i ] == true )
					{
						TLineInfo* Line = (TLineInfo*)LineListBox->Items->Objects[ i ];
						int Code = Line->GetStateCode()%100;
						if( Code == PVC_STATUS_DISABLE )
							EnablePVC( Line, true, false );
					}
				}
			}
		}
	}
	else
		MessageDlg( Scstrings_MAIN_SPEEDY_SELECT_ACTIVATE_SESSION, mtWarning, TMsgDlgButtons() << mbOK, 0 );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FIXStateButtonClick(TObject *Sender)
{
	if( !IsProcessRunning( FIX_NAME ) )
	{
		ShowMessage( Scstrings_MAIN_FIX_NO_SESSION_STATUS_NOT_RUNNING);
	}
    else
    {
        MTree Data;

        ClearFIXState();
        Data.append( "COMMAND", IMFIX_ASK_SESSIONS_STATE );
        imFIXPublisher->SendData( &Data );
    }
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ClearFIXState(  bool StopMonitoring  )
{
    TFIXSession*  Session;

    for( int i = 0; i < FFIXSessions->Count; i++ )
    {
         Session = (TFIXSession*)FFIXSessions->Objects[ i ];
         Session->SetState( SESSION_STATUS_UNKNOWN );
         Session->SetInboundSeq( 0 );
         Session->SetOutboundSeq( 0 );
    }
    FIXSessionsListView->Invalidate();
    if( StopMonitoring == true )
        StopMonitorFIXSession();
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FIXSessionsListViewSelectItem(TObject *Sender,
	  TListItem *Item, bool Selected)
{
	if( Selected && FMonitorFIX )
	{
		TFIXSession*  Session = (TFIXSession*)FFIXSessions->Objects[ Item->Index ];
		AnsiString info;
		if( Session )
		{
			TAcceptor*  Accept    = dynamic_cast<TAcceptor*>(Session);
			TInitiator* Initiator = dynamic_cast<TInitiator*>(Session);
			TStringList* InfoStrings = new TStringList();
			if( Accept )
				InfoStrings->Add( L"Sell Side FIX session  ");
			if( Initiator )
				InfoStrings->Add( L"Buy Side FIX session  ");
			InfoStrings->Add( L"Version: "+ Session->GetFIXVersion() + "    " );
			InfoStrings->Add( L"Sender: " + Session->GetSenderCompID()+ "    ");
			InfoStrings->Add( L"Target :" + Session->GetTargetCompID()+ "    ");
			if( Accept )
			{
				info.printf( "Port: %d", Accept->GetAcceptorPort() );
				InfoStrings->Add( info );
			}
			if( Initiator )
			{
				info.printf( "Heartbeat: %d sec", Initiator->GetHeartbeatInterval() );
				InfoStrings->Add( info );
				for( int i = 0; i < Initiator->HostCount(); i++ )
				{
					info.printf( "Host: %s", Initiator->GetHostIP(i).c_str() );
					InfoStrings->Add( info );
					info.printf( "Port: %d", Initiator->GetHostPort(i));
					InfoStrings->Add( info );
				}
			}
			this->FIXSessionInfoLabel->Caption = InfoStrings->Text;
			delete InfoStrings;
			///< Reset the in/out Performance Gauge.
			FFIXRecvTick = 0;
			FMaxInVol    = 0;
			FMaxOutVol   = 0;
			FIXInPerformanceGauge->Clear();
			FIXOutPerformanceGauge->Clear();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::EraseBKGnd( TListView* ListView  )
{
	RECT UpdateRect = {0,0,ListView->Width,ListView->Height};
	RECT ItemRect;
	HRGN ClipRGN,ItemRgn;

	ClipRGN = CreateRectRgnIndirect( &UpdateRect );
	for( int i = 0; i < ListView->Items->Count;i++)
	{
		 ListView_GetItemRect( ListView->Handle, i, &ItemRect, LVIR_BOUNDS );
		 ItemRect.left += 24; ///< Erase the icon background.
		 ItemRgn = CreateRectRgnIndirect( &ItemRect );
		 CombineRgn( ClipRGN, ClipRGN, ItemRgn, RGN_DIFF );
		 DeleteObject( ItemRgn );
	}
	SelectClipRgn( ListView->Canvas->Handle, ClipRGN );
	ListView->Canvas->Brush->Color = ListView->Color;
	ListView->Canvas->FillRect( UpdateRect );
	SelectClipRgn( ListView->Canvas->Handle, NULL );
	DeleteObject( ClipRGN );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::QueryButtonClick(TObject *Sender)
{
	unsigned short y,m,d;
	MTree          Data;
	AnsiString     Acc = AccountComboBox->Text;

	if( (int)BeginDateTimePicker->Date > (int)EndDateTimePicker->Date )
	{
		ShowMessage( Scstrings_MAIN_ADMIN_QUERY_DATE_ERR );
		return;
	}
	QueryButton->Enabled = false;
	Data.append( "COMMAND", CMD_QUERY_LOG );
	BeginDateTimePicker->Date.DecodeDate(&y,&m,&d);
	Data.append( "BY", (UFCType::Int32)y );
	Data.append( "BM", (UFCType::Int32)m );
	Data.append( "BD", (UFCType::Int32)d );
	EndDateTimePicker->Date.DecodeDate(&y,&m,&d);
	Data.append( "EY", (UFCType::Int32)y );
	Data.append( "EM", (UFCType::Int32)m );
	Data.append( "ED", (UFCType::Int32)d );
	Data.append( "ID", Acc.c_str() );
	AdminPublisher->SendData( &Data );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ClearButtonClick(TObject *Sender)
{
    UserLogListView->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::WriteUserLog( const AnsiString& Type, const AnsiString& Msg )
{
	if( FIsLogon == true )
	{
		MTree Data;

		Data.append( "COMMAND", CMD_WRITE_LOG );
		Data.append( "ID", FUser.c_str() );
		Data.append( "TYPE", Type.c_str() );
		Data.append( "MSG", Msg.c_str() );
		AdminPublisher->SendData( &Data );
		EventForm->AddEvent( FSpeedyHost, Type, Msg );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::SaveUserLogButtonClick(TObject *Sender)
{
	if( SaveDialog->Execute() )
	{
		TStringList* Log = new TStringList();
		for( int i = 0; i < UserLogListView->Items->Count; i++ )
		{
			TListItem* Item = UserLogListView->Items->Item[ i ];
			AnsiString LogStr;
            LogStr = Item->SubItems->Strings[0]+","+Item->SubItems->Strings[1]+Item->SubItems->Strings[2];
            Log->Add( LogStr );
		 }
		 Log->SaveToFile( SaveDialog->FileName );
         delete Log;
    }
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::HeartbeatResponseTimerTimer(TObject *Sender)
{
	HeartbeatResponseTimer->Enabled = false;
	if( FHeartbeatResponse == false )
    {
		LogoutButtonClick( NULL );
		ShowMessage( Scstrings_MAIN_NO_RESPONSE_CHECK_AGENT_MANAGER );
    }
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::StopMonitorFIXSession( void )
{
	FFIXRecvTick = 0;
	FMonitorFIX = false;
	FIXPageControl->Align = alBottom;
	FIXPageControl->Height = 0;
	Splitter->Align = alBottom;
	Splitter->Enabled = false;
	FIXSessionsListView->Align = alClient;
	MonitorFIXButton->Caption = Scstrings_MAIN_MONITORING_SESSION;
	FMonitorSession = NULL;
	for( int i = 0; i < FIXSessionsListView->Items->Count; i++ )
	{
		 TListItem* Item = FIXSessionsListView->Items->Item[ i ];
		 Item->StateIndex = -1;
	}
	FIXSessionsListView->Invalidate();
	FIXInSubscriber->Key  = L"none";
	FIXOutSubscriber->Key = L"none";
	FIXMsgListBox->Clear();
	FIXLogTimer->Enabled = false;
	if( FIXControl )
	{
	   ShowMsgCheckBox->Enabled = true;
	   ShowMsgCheckBoxClick( NULL );
	}
	else
	{
	   ShowMsgCheckBox->Enabled = false;
	   ShowMsgCheckBox->Checked = false;
	   ShowMsgCheckBoxClick( NULL );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::MonitorFIXSession( void )
{
	MonitorFIXButton->Caption = Scstrings_MAIN_STOP_MONITORING_SESSION;
	FIXSessionsListView->Align = alTop;
	FIXSessionsListView->Height = FIXViewPanel->Height - 286;
	Splitter->Align = alTop;
	Splitter->Enabled = true;
	FIXPageControl->Align  = alClient;
	FMonitorSession = (TFIXSession*)FFIXSessions->Objects[ FIXSessionsListView->Selected->Index ];
	FIXSessionsListView->Selected->StateIndex = 8;
	FIXSessionsListViewSelectItem( this, FIXSessionsListView->Selected, true );
	FIXInSubscriber->Key  = FMonitorSession->GetSessionID();
	FIXOutSubscriber->Key = L"all";
	FIXMsgListBox->ScrollWidth = FIX_MSG_WIDTH;
	FIXLogTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::MonitorFIXButtonClick(TObject *Sender)
{
	if( FMonitorFIX == false && FIXSessionsListView->Selected == NULL )
	{
		MessageDlg( Scstrings_MAIN_FIX_SELECT_MOTORING_SESSION, mtWarning, TMsgDlgButtons() << mbOK,0);
		return;
	}
	FMonitorFIX = !FMonitorFIX;
	if( FMonitorFIX == true ) /// Monitor a FIX Session
		MonitorFIXSession();
	else ///< Already monitor a FIX session
		StopMonitorFIXSession();
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ReplaceChar( UFC::AnsiString& FIXMsg )
{
	for( register int i = 0;i < FIXMsg.Length(); i++ )
	{
		switch( FIXMsg[i] )
		{
			case '|':  FIXMsg[i] = ' ';break; ///< Replace SOH char
			case 0x01: FIXMsg[i] = ' ';break; ///< Replace SOH char
			case ' ':  FIXMsg[i] = '_';break; ///< Replace space char
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FIXOutSubscriberMessage(
      const MString &Subject, const MString &Key, MTree *Tree)
{
    if( MessageTabSheet->TabVisible == true )
    {
        UFC::AnsiString FIXMsg,SID;

		if( FMonitorFIX && FMonitorSession && Tree->get("SID", SID ) && Tree->get("FIXMSG", FIXMsg ))
        {
            if( FMonitorSession->IsSame( SID.c_str()) )
            {
                ReplaceChar( FIXMsg );
                AddFIXMessage( 0, FIXMsg.c_str() );
            }
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FIXInSubscriberMessage(const MString &Subject,
      const MString &Key, MTree *Tree)
{
	if( MessageTabSheet->TabVisible == true )
    {
        UFC::AnsiString FIXMsg;

		if( FMonitorFIX && Tree->get( "FIXMSG", FIXMsg ))
		{
			ReplaceChar( FIXMsg );
			AddFIXMessage( 1, FIXMsg.c_str() );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FIXMsgListBoxDrawItem(TWinControl *Control,
	  int Index, TRect &Rect, TOwnerDrawState State)
{
	FListBuffer->Width  = Rect.Width();
	FListBuffer->Height = Rect.Height();
	RECT TextRect = { 1,1, FIX_MSG_WIDTH-2, FListBuffer->Height-2};

	FListBuffer->Canvas->Pen->Color = clBlack;
	FListBuffer->Canvas->Rectangle( 0, 0, FListBuffer->Width, FListBuffer->Height );
	FListBuffer->Canvas->Pen->Color = clYellow;
	FListBuffer->Canvas->MoveTo( 0, FListBuffer->Height-2 );
	FListBuffer->Canvas->LineTo( FListBuffer->Width, FListBuffer->Height-2 );
	if( FIXMsgListBox->Items->Strings[ Index ][1] == 'R' )
		FListBuffer->Canvas->Font->Color  = clLime;  ///< Receive
	else
		FListBuffer->Canvas->Font->Color  = clWhite; ///< Send
	AnsiString Text = FIXMsgListBox->Items->Strings[ Index ];
	DrawTextA( FListBuffer->Canvas->Handle,
			   Text.c_str(),
			   Text.Length(),
			   &TextRect, DT_LEFT|DT_WORDBREAK );
	FIXMsgListBox->Canvas->Draw( Rect.Left, Rect.Top, FListBuffer );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FIXMsgListBoxMeasureItem(TWinControl *Control,
      int Index, int &Height)
{
	RECT Rect;
	Rect.left   = 0;
	Rect.right  = FIX_MSG_WIDTH-2;
	Rect.top    = 0;
	Rect.bottom = 0;

	AnsiString Text = FIXMsgListBox->Items->Strings[ Index ];

	DrawTextA( FListBuffer->Canvas->Handle,
			   Text.c_str(), Text.Length(),
			   &Rect, DT_CALCRECT|DT_LEFT|DT_WORDBREAK  );//DT_NOFULLWIDTHCHARBREAK );
	Height = (Rect.bottom - Rect.top )+ 2;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FIXMsgListBoxClick(TObject *Sender)
{
	int Index = FIXMsgListBox->ItemIndex;
	if( Index != -1 )
	{
		TagForm->SetFIXMessage( FIXMsgListBox->Items->Strings[Index] );
		TagForm->Show();
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FIXLogTimerTimer(TObject *Sender)
{
	if( FFIXLogList->Count > 0 )
	{
		FIXMsgListBox->Items->BeginUpdate();
			FFIXLogCS->Enter();
				FIXMsgListBox->Items->AddStrings( FFIXLogList );
				FFIXLogList->Clear();
			FFIXLogCS->Leave();
		FIXMsgListBox->Items->EndUpdate();
		FIXMsgListBox->TopIndex = FIXMsgListBox->Count - 3; ///< Scroll to the end
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::Clear1Click(TObject *Sender)
{
    FIXMsgListBox->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ShowMsgCheckBoxClick(TObject *Sender)
{
	if( ShowMsgCheckBox->Checked == true )
        MessageTabSheet->TabVisible = true;
    else
        MessageTabSheet->TabVisible = false;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::OtherProcessListViewCustomDrawSubItem(
	  TCustomListView *Sender, TListItem *Item, int SubItem,
	  TCustomDrawState State, bool &DefaultDraw)
{
	TRect ItemRect;

	DefaultDraw = false;
	ListView_GetSubItemRect( OtherProcessListView->Handle, Item->Index, SubItem,LVIR_BOUNDS,&ItemRect);
	if( Item->Selected )
	{
		Sender->Canvas->Brush->Style = bsClear;
		DrawGradientBar( OtherProcessListView->Canvas->Handle , ItemRect, clWhite, clSilver );
	}
	else
	{
		OtherProcessListView->Canvas->Brush->Style = bsSolid;
		OtherProcessListView->Canvas->Brush->Color = UserListView->Color;
		OtherProcessListView->Canvas->FillRect( ItemRect );
	}
	if( SubItem == 1 )
	{
		String      Text;
		int         ImgIndex;
		TColor      Color;
		TTextFormat Formats;

		switch( FOtherProcessState[ Item->Index ]  )
		{
			case PROCESS_STOPED:    Text = Scstrings_MAIN_PROCESS_STAT_STOPED;  ImgIndex = 4; Color = clRed; break;
			case PROCESS_STARTING:  Text = Scstrings_MAIN_PROCESS_STAT_SATRTING;ImgIndex = 5; Color = clNavy; break;
			case PROCESS_STOPING:   Text = Scstrings_MAIN_PROCESS_STAT_STOPING; ImgIndex = 5; Color = clNavy; break;
			case PROCESS_RESTARTING:Text = Scstrings_MAIN_PROCESS_STAT_RESTART; ImgIndex = 5; Color = clNavy; break;
			case PROCESS_RUNNING:   Text = Scstrings_MAIN_PROCESS_STAT_RUNNING;	ImgIndex = 3; Color = clGreen;break;
			default:                Text = L"---";                              ImgIndex = 5; Color = clBlack;break;
		}
		StateImageList->Draw( OtherProcessListView->Canvas, ItemRect.left + 5, ItemRect.top+1, ImgIndex, true);
		SetTextColor( OtherProcessListView->Canvas->Handle,(COLORREF)Color );
		Formats<<tfSingleLine<<tfCenter<<tfVerticalCenter;
		OtherProcessListView->Canvas->TextRect( ItemRect, Text, Formats );
	}
	else if( SubItem == 2 )
	{
		String      Text;
		TTextFormat Formats;

		if( FOtherProcessAutoStart[ Item->Index ] )
			Text = FOtherProcessStartTime[ Item->Index ];
		else
			Text = L"--:--";
		SetTextColor( OtherProcessListView->Canvas->Handle,(COLORREF)clNavy );
		Formats<<tfSingleLine<<tfCenter<<tfVerticalCenter;
		OtherProcessListView->Canvas->TextRect( ItemRect, Text, Formats );
	}
	else if( SubItem == 3 )
	{
		String      Text;
		TTextFormat Formats;

		if( FOtherProcessAutoStart[ Item->Index ] )
			Text = FOtherProcessStopTime[ Item->Index ];
		else
			Text = L"--:--";
		SetTextColor( OtherProcessListView->Canvas->Handle,(COLORREF)clNavy );
		Formats<<tfSingleLine<<tfLeft<<tfVerticalCenter<<tfEndEllipsis;
		OtherProcessListView->Canvas->TextRect( ItemRect, Text, Formats );
	}
	else if( SubItem == 4 ) ///<Start
	{
		int Offset = ItemRect.left + (ItemRect.right -ItemRect.left- StateImageList->Width )/2;

		if( FOtherProcessState[ Item->Index ] != PROCESS_RUNNING )
			StateImageList->Draw( OtherProcessListView->Canvas, Offset, ItemRect.top+1, 0, true);
		else
			StateImageList->Draw( OtherProcessListView->Canvas, Offset, ItemRect.top+1, 10, true);
	}
	else if( SubItem == 5 ) ///< Stop
	{
		int Offset = ItemRect.left + (ItemRect.right -ItemRect.left- StateImageList->Width )/2;

		if( FOtherProcessState[ Item->Index ] == PROCESS_RUNNING )
			StateImageList->Draw( OtherProcessListView->Canvas, Offset, ItemRect.top+1, 1, true);
		else
			StateImageList->Draw( OtherProcessListView->Canvas, Offset, ItemRect.top+1, 11, true);
	}
	else if( SubItem == 6 )
	{
		int Offset = ItemRect.left + (ItemRect.right -ItemRect.left- StateImageList->Width )/2;
		StateImageList->Draw( OtherProcessListView->Canvas, Offset, ItemRect.top+1, 2, true);
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::UpdateUserConfigFile( void )
{
	AnsiString       ConfigFileName = GetUserConfigFileName();
	UFC::UiniFile    ini( ConfigFileName.c_str() );

	LockUserUI( false );
	switch( FModifyType )
	{
		case mtNewUser:AccountForm->UpdateNewAccount( &ini );
					   break;                     
		case mtModifyUser: AccountForm->UpdateModifyAccount( &ini );
					   break;
		case mtChangePassword: AccountForm->UpdateChangePasswd( &ini );
					   break;
		default:       return;
	}
	ini.Save( );
	SendFile( ConfigFileName, "FILE", SPEEDY_SET_CONFIG_FILE, SPEEDY_USERS_CFG );
	ReadUsers();
	if( AccountForm != NULL )
		delete AccountForm;
	AccountForm = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::LockUserUI( bool Lock )
{
	NewAccountButton->Enabled          = !Lock;
	ModifyAccountButton->Enabled       = !Lock;
	ChangeAccountPwdButton->Enabled    = !Lock;
	DelAccountButton->Enabled 		   = !Lock;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::NewAccountButtonClick(TObject *Sender)
{
	if( AccountForm != NULL )
		delete AccountForm;
	AccountForm = new TAccountForm( this );

	if( FormSetting->ForceRM() == true )
	{
		if( FCheckers->Strings[ 0 ] == Scstrings_MAIN_RM_NOT_EXISTS )
			FCheckers->Delete( 0 );
	}
	else
	{
		if( FCheckers->Strings[ 0 ] != Scstrings_MAIN_RM_NOT_EXISTS )
			FCheckers->Insert( 0, Scstrings_MAIN_RM_NOT_EXISTS);
	}
	AccountForm->SourceComboBox->Items->Assign( FormSetting->Sources );
	AccountForm->SourceComboBox->ItemIndex = 0;
	AccountForm->CheckerComboBox->Items->Assign( FCheckers );
	AccountForm->CheckerComboBox->ItemIndex = 0;
	AccountForm->TSECheckerComboBox->Items->Assign( FCheckers );
	AccountForm->TSECheckerComboBox->ItemIndex = 0;
	if( AccountForm->ShowModal() == mrOk )
	{
		LockUserUI( true );
		FModifyType = mtNewUser; ///< It's an add user action.
		RequestUserConfigFile(); ///< Get the most update config file.
	}
	else
	{
		delete AccountForm;
		AccountForm = NULL;
	}
}
//---------------------------------------------------------------------------
AnsiString __fastcall TSimTFXForm::GetUserConfigFileName( bool FullPath )
{
	AnsiString ConfigFileName;
	if( FullPath == true )
		ConfigFileName.printf("%s\\%s-%s", CurrentDir.c_str(), FSpeedyHost.c_str(), SPEEDY_USERS_CFG );
	else
		ConfigFileName.printf("%s-%s", FSpeedyHost.c_str(), SPEEDY_USERS_CFG );
	return ConfigFileName;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TSimTFXForm::GetProfileConfigFileName( bool FullPath )
{
	AnsiString ConfigFileName;
	if( FullPath == true )
		ConfigFileName.printf("%s\\%s-%s", CurrentDir.c_str(), FSpeedyHost.c_str(), SPEEDY_PROFILE_CFG );
	else
		ConfigFileName.printf("%s-%s", FSpeedyHost.c_str(), SPEEDY_PROFILE_CFG );
	return ConfigFileName;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ModifyAccountButtonClick(TObject *Sender)
{
	TListItem* Modify = AccountsListView->Selected;
	if( Modify != NULL )
	{
		AnsiString       ModifyName = Modify->Caption;
		AnsiString       ConfigFileName = GetUserConfigFileName();
		UFC::UiniFile    ini( ConfigFileName.c_str() );
		UFC::Section*    iniSection = ini.GetSection( ModifyName.c_str());

		if( iniSection != NULL )
		{
			if( AccountForm != NULL )
				delete AccountForm;
			if( FormSetting->ForceRM() == true )
			{
				if( FCheckers->Strings[ 0 ] == Scstrings_MAIN_RM_NOT_EXISTS )
					FCheckers->Delete( 0 );
			}
			else
			{
				if( FCheckers->Strings[ 0 ] != Scstrings_MAIN_RM_NOT_EXISTS )
					FCheckers->Insert( 0, Scstrings_MAIN_RM_NOT_EXISTS);
			}
			AccountForm = new TAccountForm( this, iniSection, FCheckers,FormSetting->Sources, true );
			if( AccountForm->ShowModal() == mrOk )
			{
				LockUserUI( true );
				FModifyType = mtModifyUser;
				RequestUserConfigFile();
			}
			else
			{
				delete AccountForm;
				AccountForm = NULL;
			}
		}
	}
	else
		ShowMessage( Scstrings_MAIN_AE_SEL_MODIFY_AE );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ChangeAccountPwdButtonClick(TObject *Sender)
{
	TListItem* Modify = AccountsListView->Selected;
	if( Modify != NULL )
	{
		AnsiString       ModifyName = Modify->Caption;
		AnsiString       ConfigFileName = GetUserConfigFileName();
		UFC::UiniFile    ini( ConfigFileName.c_str() );
		UFC::Section*    iniSection = ini.GetSection( ModifyName.c_str());

		if( iniSection != NULL )
		{
			if( AccountForm != NULL )
				delete AccountForm;
			if( FormSetting->ForceRM() == true )
			{
				if( FCheckers->Strings[ 0 ] == Scstrings_MAIN_RM_NOT_EXISTS )
					FCheckers->Delete( 0 );
			}
			else
			{
				if( FCheckers->Strings[ 0 ] != Scstrings_MAIN_RM_NOT_EXISTS )
					FCheckers->Insert( 0, Scstrings_MAIN_RM_NOT_EXISTS);
			}
			AccountForm = new TAccountForm( this, iniSection,FCheckers,FormSetting->Sources, false );
			if( AccountForm->ShowModal() == mrOk )
			{
				LockUserUI( true );
				FModifyType = mtChangePassword;
				RequestUserConfigFile();
			}
			else
			{
				delete AccountForm;
				AccountForm = NULL;
			}
		}
	}
	else
		ShowMessage( Scstrings_MAIN_AE_SEL_MODIFY_PASSWORD_AE);
}
//---------------------------------------------------------------------------

void __fastcall TSimTFXForm::DelAccountButtonClick(TObject *Sender)
{
	TListItem* Del = AccountsListView->Selected;

	if( Del != NULL )
	{
		String WarningMsg = Scstrings_MAIN_ASK_DELETE_AE;

		WarningMsg += Del->Caption;
		WarningMsg += L" ?\n";
		WarningMsg += Scstrings_MAIN_AE_DELETE_AE_WARNING;
		if( MessageDlg( WarningMsg, mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0 ) == mrOk )
		{
			AnsiString       DelName = Del->Caption;
			AnsiString       ConfigFileName = GetUserConfigFileName();
			UFC::UiniFile    ini( ConfigFileName.c_str() );

			ini.DeleteSection( DelName.c_str() );
			ini.Save( );
			SendFile( ConfigFileName, "FILE", SPEEDY_SET_CONFIG_FILE, SPEEDY_USERS_CFG );
			ReadUsers();
		}
	}
	else
		ShowMessage( Scstrings_MAIN_AE_SEL_DELETE_AE );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::AccountsListViewDblClick(TObject *Sender)
{
	ModifyAccountButtonClick( Sender );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ChangeAPCode( TLineInfo* Line, char APCODE )
{
	MTree Data;
	UFC::AnsiString AP( APCODE );

	Line->SetAPCode( AP.ToInt() );
	Data.append( "COMMAND", AGENT_CHANGE_APCODE );
	Data.append( "SYS",     (int)Line->GetSystem() );
	Data.append( "LINK",    Line->GetLink() );
	Data.append( "PVCID",   Line->GetPVCID().c_str() );
	Data.append( "BROKERID",Line->GetBrokerID().c_str() );
	Data.append( "APCODE",  AP );
	AgentPublisher->SendData( &Data );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ChangeBackup( TLineInfo* Line, int Index )
{
	MTree Data;

	Data.append( "COMMAND", AGENT_PVC_BACKUPSWITCH );
	Data.append( "SYS",     (int)Line->GetSystem() );
	Data.append( "LINK",    Line->GetLink() );
	Data.append( "PVC",     Line->GetPVC() );
	Data.append( "PVCID",   Line->GetPVCID().c_str() );
	Data.append( "BROKERID",Line->GetBrokerID().c_str() );
	Data.append( "INDEX",   Index );
	Data.append( "OFFHOUR", (int)Line->GetOffHour() );
	AgentPublisher->SendData( &Data );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::N15Click(TObject *Sender)
{
	if( LineListBox->ItemIndex != -1 )
	{
		TLineInfo* Line = (TLineInfo*)LineListBox->Items->Objects[ LineListBox->ItemIndex ];
		ChangeAPCode( Line, '5' );
	}
	else
		MessageDlg( Scstrings_MAIN_TSE_SEL_LEND_PVC, mtWarning, TMsgDlgButtons() << mbOK,0);
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::TenderMenuItemClick(TObject *Sender)
{
	if( LineListBox->ItemIndex != -1 )
	{
		TLineInfo* Line = (TLineInfo*)LineListBox->Items->Objects[ LineListBox->ItemIndex ];
		ChangeAPCode( Line, '8' );
	}
	else
		MessageDlg( Scstrings_MAIN_TSE_SEL_TENDER_PVC, mtWarning, TMsgDlgButtons() << mbOK,0);
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::AuctionMenuItemClick(TObject *Sender)
{
	if( LineListBox->ItemIndex != -1 )
	{
		TLineInfo* Line = (TLineInfo*)LineListBox->Items->Objects[ LineListBox->ItemIndex ];
		ChangeAPCode( Line, '6' );
	}
	else
		MessageDlg( Scstrings_MAIN_TSE_SEL_AUCTION_PVC, mtWarning, TMsgDlgButtons() << mbOK,0);
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::TenderExMenuItemClick(TObject *Sender)
{
	if( LineListBox->ItemIndex != -1 )
	{
		TLineInfo* Line = (TLineInfo*)LineListBox->Items->Objects[ LineListBox->ItemIndex ];
		ChangeAPCode( Line, '7' );
	}
	else
		MessageDlg( Scstrings_MAIN_TSE_SEL_TENDER_EX_PVC, mtWarning, TMsgDlgButtons() << mbOK,0);
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::N16Click(TObject *Sender)
{
	if( LineListBox->ItemIndex != -1 )
	{
		TLineInfo* Line = (TLineInfo*)LineListBox->Items->Objects[ LineListBox->ItemIndex ];
		ChangeAPCode( Line, '0' );
	}
	else
		MessageDlg( Scstrings_MAIN_TSE_SEL_NORMAL_PVC, mtWarning, TMsgDlgButtons() << mbOK,0);
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::N17Click(TObject *Sender)
{
	if( LineListBox->ItemIndex != -1 )
	{
		TLineInfo* Line = (TLineInfo*)LineListBox->Items->Objects[ LineListBox->ItemIndex ];
		ChangeAPCode( Line, '4' );
	}
	else
		MessageDlg( Scstrings_MAIN_TSE_SEL_ODD_PVC, mtWarning, TMsgDlgButtons() << mbOK,0);
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::N18Click(TObject *Sender)
{
	if( LineListBox->ItemIndex != -1 )
	{
		TLineInfo* Line = (TLineInfo*)LineListBox->Items->Objects[ LineListBox->ItemIndex ];
		ChangeAPCode( Line, '9' );
	}
	else
		MessageDlg( Scstrings_MAIN_TSE_SEL_POST_PVC, mtWarning, TMsgDlgButtons() << mbOK,0);
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::BackupLineClick(TObject *Sender)
{
	if( LineListBox->ItemIndex != -1 )
	{
		TLineInfo* Line  = (TLineInfo*)LineListBox->Items->Objects[ LineListBox->ItemIndex ];
		TMenuItem* NewIP = (TMenuItem*)Sender;
		if( NewIP->Checked == false )
			ChangeBackup( Line, NewIP->Tag );
		else
			MessageDlg( L"正在使用此線路,不需要切換", mtWarning, TMsgDlgButtons() << mbOK,0);
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::LineListBoxContextPopup(TObject *Sender,
	  TPoint &MousePos, bool &Handled)
{
	int CurserIndex;

	if( ConnectionControl == false )
	{
		Handled = true;
		return;
	}
	if( LineListBox->ItemIndex != -1 &&
		(CurserIndex = LineListBox->ItemAtPos( MousePos,true)) != -1 &&
		LineListBox->Selected[ CurserIndex ] )
	{
		if( LineListBox->SelCount <= 1 )
		{
			TLineInfo* Line = (TLineInfo*)LineListBox->Items->Objects[ CurserIndex ];
			bool EnableFIXFunc;

			if( Line->GetSystem() == mtStock )///< TWSE
			{
				CODPVCMenuItem->Enabled = false;
				APCodeMenuItem->Enabled = true;
				TFXBackItem->Enabled = false;
				AuctionMenuItem->Caption = Scstrings_MAIN_AUCTION;//"拍賣";
				EnableFIXFunc = Line->IsFIX();
			}
			else if(  Line->GetSystem() == mtOTC ) ///< OTC
			{
				CODPVCMenuItem->Enabled = false;
				APCodeMenuItem->Enabled = true;
				TFXBackItem->Enabled = false;
				AuctionMenuItem->Caption = Scstrings_MAIN_NEGOTIATE_PX;//"議價";
				if( Line->GetBrokerID()[4] == 'T' ) ///< It's dealer branch.
					AuctionMenuItem->Enabled = true;///< Can switch to Negotiate Px session.
				else
					AuctionMenuItem->Enabled = false;///< Not support Negotiate Px session.
				EnableFIXFunc = Line->IsFIX();//&& IsProcessRunning(SPEEDY_OTC_NAME) )
			}
			else ///< Futures/Option only have normal trade session.
			{
				if( DisablePVCMenuItem->Enabled == true )
					CODPVCMenuItem->Enabled = true;
				else
					CODPVCMenuItem->Enabled = false;
				APCodeMenuItem->Enabled = false;
				if( Line->GetIPCount() > 1 )
				{
					TFXBackItem->Enabled = true;
					TFXBackItem->Clear();
					for( int i = 0; i < Line->GetIPCount(); i ++ )
					{
						TMenuItem* NewIP = new TMenuItem( TFXBackItem );
						if( Line->GetBackupIndex() == i )
							NewIP->Checked = true;
						else
							NewIP->Checked = false;
						NewIP->Caption = Line->GetIP( i );
						NewIP->OnClick = BackupLineClick;
						NewIP->Tag = i;
						TFXBackItem->Add( NewIP );
					}
				}
				else
					TFXBackItem->Enabled = false;
				EnableFIXFunc = false;
			}
			EODItem->Enabled    = EnableFIXFunc;
			FIXSeqItem->Enabled = EnableFIXFunc;
			MonitorMenuItem->Enabled = true;
			Handled = false;
		}
		else
		{
			EODItem->Enabled = false;
			FIXSeqItem->Enabled = false;
			MonitorMenuItem->Enabled = false;
			APCodeMenuItem->Enabled = false;
			Handled = false;
		}
	}
	else
		Handled = true;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::GetSystemInfo( void )
{
	MTree      Tree;
	AnsiString ReplyKey = SystemInfoSubscriber->Key;

	SystemForm->Caption = Scstrings_MAIN_SYSTEM_INFO + SystemInfoPublisher->Key;
	SystemForm->InfoListBox->Items->Clear();
	Tree.append( "ReplyKey", ReplyKey.c_str() );
	SystemInfoPublisher->SendData( &Tree );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::SystemInfoSubscriberMessage(const MString &Subject,
	  const MString &Key, MTree *Tree)
{
	UFC::AnsiString Value;
	int             Count;
	if( Tree->get( "Name", Value ) == TRUE && Tree->get( "COUNT", Count ) == TRUE  )
	{
		SystemForm->InfoListBox->Items->Add( Value.c_str() );
		for( int i = 0; i < Count; i++ )
		{
			UFC::AnsiString Name( i );
			if( Tree->get( Name, Value ) == TRUE )
				SystemForm->InfoListBox->Items->Add( Value.c_str() );
		}
		SystemForm->Show();
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::AlarmButtonClick(TObject *Sender)
{
	TGraphButton* ClickButton = dynamic_cast<TGraphButton*>(Sender);
	if( ClickButton != NULL )
	{
		ClickButton->Selected = !ClickButton->Selected;
		if( Sender == OPTAlarmButton )
			FormSetting->SetAlarmOPT( !ClickButton->Selected );
		else if( Sender == FUTAlarmButton )
			FormSetting->SetAlarmFUT( !ClickButton->Selected );
		else if( Sender == TSEAlarmButton )
			FormSetting->SetAlarmTSE( !ClickButton->Selected );
		else if( Sender == OTCAlarmButton )
			FormSetting->SetAlarmOTC( !ClickButton->Selected );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FIXSessionsListViewDblClick(TObject *Sender)
{
	if( SessionSettingButton->Enabled == true )
		SessionSettingButtonClick( NULL );
	else
		MonitorFIXButtonClick( NULL );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::LineListBoxDataObject(TWinControl *Control,
	  int Index, TObject *&DataObject)
{
	int AllIndex = DisplayIndexToAllIndex( Index );
	DataObject  = FLinkPVCToPanel->Objects[ AllIndex ];
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::AccountsListViewAdvancedCustomDrawItem(
	  TCustomListView *Sender, TListItem *Item, TCustomDrawState State,
	  TCustomDrawStage Stage, bool &DefaultDraw)
{
	if( Item->Index%2 )
		AccountsListView->Canvas->Brush->Color = clSilver;
	else
		AccountsListView->Canvas->Brush->Color = clWhite;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::StaticText5DblClick(TObject *Sender)
{
	FShowPassword = !FShowPassword;
	LineListBox->Invalidate();
}
//---------------------------------------------------------------------------
const int CPU_WARNING_SEC = 5;
const int MEM_WARNING_SEC = 5;
const int TIMEDIFF_WARNING_SEC = 5;
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::TOPSubscriberMessage(const MString &Subject,
	  const MString &Key, MTree *Tree)
{
	int CPU,Total,TimeDiff,Free,Process,Active,Time;
	String Msg;

	if( Tree->get( "CPU", CPU ) )
	{
		Msg.printf( L"%0.1f\%",((float)CPU)/10.0 );
		CPUPerformanceGauge->Add( CPU*10 );
		CPULabel->Caption = Msg;
		if( CPU*10 > CPUTrackBar->Position*100 )
		{
			FCPUCount++;
			if( FCPUCount > CPU_WARNING_SEC )
			{
				if( CPUAlarmButton->Selected == false )
					PlaySoundA( FormSetting->GetCPUWarningSound().c_str(), NULL, SND_ASYNC|SND_FILENAME );
				FCPUCount = 0;
			}
		}
		else
			FCPUCount = 0;
	}
	if( Tree->get( "TimeDiff", TimeDiff ) )
	{
		if( TimeDiff >= FDiffMax  )
			FDiffMax = TimeDiff;
		Msg.printf( L"%dms Current:%dms ", FDiffMax, TimeDiff );
		MaxDiffLabel->Caption = Msg;
		TimeDiffPerformanceGauge->Add( TimeDiff*100 );
		if( TimeDiff > TimeDiffTrackBar->Position )
		{
			FDiffCount++;
			if( FDiffCount > TIMEDIFF_WARNING_SEC )
			{
				if( TimeAlarmButton->Selected == false )
					PlaySoundA( FormSetting->GetTimeDiffWarningSound().c_str(), NULL, SND_ASYNC|SND_FILENAME );

				AnsiString Message;

				Message.printf( "%dms time difference.", TimeDiff );
				WarningForm->LogError( FSpeedyHost, FSpeedyHostIP,"E08",Message );
				FDiffCount = 0;
			}
		}
		else
			FDiffCount = 0;
	}
	if( Tree->get( "MEM_TOTAL", Total ) && Tree->get( "MEM_FREE", Free ) )
	{
		if( Total > 0 )
		{
			int Usage = (100*(Total-Free)) / Total;

			Msg.printf( L"%dM", Total>>10 );
			MEMLabel->Caption = Msg;
			Msg.printf( L"%dM(%d\%%)", Free>>10, 100 - Usage );
			MEMFreeLabel->Caption = Msg;
			MEMPerformanceGauge->Add( 100*Usage );
			if( Usage > MemTrackBar->Position )
			{
				MEMFreeLabel->Font->Color = clRed;
				FMemCount++;
				if( FMemCount > MEM_WARNING_SEC )
				{
					if( MemAlarmButton->Selected == false )
						PlaySoundA( FormSetting->GetMemWarningSound().c_str(), NULL, SND_ASYNC|SND_FILENAME );
					FMemCount = 0;
				}
			}
			else
			{
				FMemCount = 0;
				MEMFreeLabel->Font->Color = clBlack;
			}
		}
	}
	if( Tree->get( "TotalM", Total ) && Tree->get( "AvailM", Free ) )
	{
		if( DiskGroupBox->Height == 1 )
			DiskGroupBox->Height = 133;
		if( Total > 0 )
		{
			int Usage = (100*(Total-Free)) / Total;

			Msg.printf( L"%dM", Total );
			DiskLabel->Caption = Msg;
			Msg.printf( L"%dM(%d\%%)", Free, 100 - Usage );
			DiskFreeLabel->Caption = Msg;
			DiskPerformanceGauge->Add( 100*Usage );
			if( Usage > DiskTrackBar->Position )
			{
				DiskFreeLabel->Font->Color = clRed;
				FDiskCount++;
				if( FDiskCount > MEM_WARNING_SEC )
				{
					if( DiskAlarmButton->Selected == false )
						PlaySoundA( FormSetting->GetDiskWarningSound().c_str(), NULL, SND_ASYNC|SND_FILENAME );
					FDiskCount = 0;
				}
			}
			else
			{
				FDiskCount = 0;
				DiskFreeLabel->Font->Color = clBlack;
			}
		}
	}
	if( Tree->get( "PROCESS", Process ) && Tree->get( "ACTIVE", Active ) )
	{
		ProcessCountLabel->Caption = IntToStr( Process );
		ActiveProcessCountLabel->Caption = IntToStr( Active );
	}
	if( Tree->get( "TIME", Time )  )
	{
		FTimeStr.printf( "%2d:%02d:%02d", Time/10000, (Time%10000)/100, Time%100 );
		if( FOnMDITime != NULL )
			FOnMDITime( this );
	}
}
//---------------------------------------------------------------------------
int __fastcall TSimTFXForm::DisplayIndexToAllIndex( int DispIndex )
{
	try
	{
		return FDispToAll->Strings[ DispIndex ].ToInt();
	}
	catch( Exception &e )
	{
		ShowMessage( "Exception:" + e.Message + " in function DisplayIndexToAllIndex.");
		return 0;
	}
}
//---------------------------------------------------------------------------
int __fastcall TSimTFXForm::AllIndexToDisplayIndex( int AllIndex )
{
	return FDispToAll->IndexOf( IntToStr( AllIndex ) );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::SystemComboBoxChange(TObject *Sender)
{
	TMarket    MarketFilter;
	AnsiString BrokerIDFilter;
	bool    FilterMarket   = true;
	bool    FilterBrokerID = true;
	bool    ErrorOnly;

	FUIUpdateCS->Enter();
	FDispToAll->Clear();
	if( SystemComboBox->ItemIndex > 0 )
		MarketFilter = (TMarket)SystemComboBox->Items->Objects[ SystemComboBox->ItemIndex ];
	else
		FilterMarket = false;
	if( BrokerComboBox->ItemIndex > 0 )
		BrokerIDFilter = BrokerComboBox->Text;
	else
		FilterBrokerID = false;
	if( LineStatusComboBox->ItemIndex == 0 )
	{
		ErrorOnly = false;
		FilterTimer->Enabled = false;
	}
	else  ///< Show broken lines only.
	{
		ErrorOnly = true;
		FilterTimer->Enabled = true;
	}
	for( int i = 0;i < FLinkPVCToPanel->Count; i++ )
	{
		 TLineInfo* Line  = (TLineInfo*)FLinkPVCToPanel->Objects[ i ];
		 if( ErrorOnly == true && Line->IsConnected() == true ) ///< List broken lines only
			 continue;///< Skip connected lines.
		 if( !FilterMarket || Line->GetSystem() == MarketFilter )
		 {
			 if( !FilterBrokerID || BrokerIDFilter.AnsiCompare( Line->GetBrokerID()) == 0 )
			 {
				switch( AppComboBox->ItemIndex )
				{
					case 0: FDispToAll->Add( IntToStr( i ));break;
					case 1: if( Line->IsOrderLine())
								FDispToAll->Add( IntToStr( i ));
							break;
					case 2: if( Line->IsConfirmLine())
								FDispToAll->Add( IntToStr( i ));
							break;
					case 3: if( Line->IsFTLine())
								FDispToAll->Add( IntToStr( i ));
							break;
				}
			 }
		 }
	}
	LineListBox->Count = FDispToAll->Count;
	FUIUpdateCS->Leave();
	LineListBox->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::CPUTrackBarPositionChange(TObject *Sender)
{
	CPUWarningLabel->Caption = IntToStr( CPUTrackBar->Position ) + "%";
	CPUPerformanceGauge->WarningValue = CPUTrackBar->Position *100;
	CPUPerformanceGauge->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::MemTrackBarPositionChange(TObject *Sender)
{
	MemWarningLabel->Caption = IntToStr( MemTrackBar->Position ) + "%";
	MEMPerformanceGauge->WarningValue = MemTrackBar->Position *100;
	MEMPerformanceGauge->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::DiskTrackBarPositionChange(TObject *Sender)
{
	DiskWarningLabel->Caption = IntToStr( DiskTrackBar->Position ) + "%";
	DiskPerformanceGauge->WarningValue = DiskTrackBar->Position *100;
	DiskPerformanceGauge->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::TimeDiffTrackBarPositionChange(TObject *Sender)
{
	TimeDiffLabel->Caption = IntToStr( TimeDiffTrackBar->Position ) + "ms";
	TimeDiffPerformanceGauge->WarningValue = TimeDiffTrackBar->Position *100;
	TimeDiffPerformanceGauge->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::CPUAlarmButtonClick(TObject *Sender)
{
	TGraphButton* ClickButton = dynamic_cast<TGraphButton*>(Sender);
	if( ClickButton != NULL )
	{
		ClickButton->Selected = !ClickButton->Selected;
		if( Sender == CPUAlarmButton )
			FormSetting->SetAlarmCPU( !ClickButton->Selected );
		else if( Sender == MemAlarmButton )
			FormSetting->SetAlarmMemory( !ClickButton->Selected );
		else if( Sender == TimeAlarmButton )
			FormSetting->SetAlarmTimeDiff( !ClickButton->Selected );
		else if( Sender == DiskAlarmButton )
			FormSetting->SetAlarmDisk( !ClickButton->Selected );
    }
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::LineListBoxKeyUp(TObject *Sender, WORD &Key,
	  TShiftState Shift)
{
	if( (Key == 'A'||Key == 'a') && Shift.Contains( ssCtrl ) )
		 LineListBox->SelectAll();
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FilterTimerTimer(TObject *Sender)
{
	FilterTimer->Enabled = false;
	SystemComboBoxChange( NULL );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ChangePasswdButtonClick(TObject *Sender)
{
	NewUserForm = new TNewUserForm( FUser );
	if( NewUserForm->ShowModal() == mrOk )
	{
		MTree      Data;
		AnsiString Password = NewUserForm->PwdEdit->Text;
		UFC::MD5   md5( (const UFCType::UInt8*)Password.c_str(),Password.Length());

		Data.append( "COMMAND", CMD_MODIFY_PWD );
		Data.append( "ID", FUser.c_str());
		Data.append( "USER", FUser.c_str());
		Data.append( "PWD", md5.ToString() );
		AdminPublisher->SendData( &Data );
		FLogonResponse = false;
		ResponseTimer->Enabled = true;
		FPasswordExp = false;
	}
	else
	{
		if( FPasswordExp == true )
		{
			Logoff();
			StatusBar->Panels->Items[0]->Text = Scstrings_MAIN_LOGON_FAILED_PASSWORD_EXPIRED;
			if( FOnLogoff != NULL )
				FOnLogoff( this );
		}
	}
	delete NewUserForm;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::OPTShrinkButtonClick(TObject *Sender)
{
	TGraphButton* ClickButton = dynamic_cast<TGraphButton*>(Sender);
	TGroupBox* GroupBox = dynamic_cast<TGroupBox*>(ClickButton->Parent);
	if( ClickButton != NULL )
	{
		ClickButton->Selected = !ClickButton->Selected;
		if( GroupBox->Height == 213 )
			GroupBox->Height = 77;
		else
			GroupBox->Height = 213;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ProcessListViewDrawItem(TCustomListView *Sender,
	  TListItem *Item, TRect &Rect, TOwnerDrawState State)
{
	try
	{
		TCustomDrawState CDState;
		bool             DefaultDraw;
		String           DrawString = Item->Caption;
		TRect            ItemRect;
		bool             ErrorProcess = IsErrorProcess( Item->Index );

		ListView_GetItemRect( ProcessListView->Handle, Item->Index, (RECT*)&ItemRect, LVIR_LABEL );
		ItemRect.left = 0;
		if( State.Contains( odSelected  ))
		{
			CDState << cdsSelected;
			Sender->Canvas->Brush->Style = bsClear;
			if( ErrorProcess == true )
				DrawGradientBar( Sender->Canvas->Handle , ItemRect, clFuchsia, clRed );
			else
				DrawGradientBar( Sender->Canvas->Handle , ItemRect, clWhite, clSilver );
		}
		else
		{
			Sender->Canvas->Brush->Style = bsSolid;
			if( ErrorProcess == true )
				Sender->Canvas->Brush->Color = clFuchsia;
			else
				Sender->Canvas->Brush->Color = ProcessListView->Color;
			Sender->Canvas->FillRect( ItemRect );
		}
		SetTextColor( Sender->Canvas->Handle,(COLORREF)clNavy );
		ImageList->Draw( Sender->Canvas, ItemRect.left, ItemRect.top + 1, 4, true);
		ItemRect.left += ImageList->Width + 2;
		TTextFormat Formats;
		Formats <<tfSingleLine<<tfLeft<<tfVerticalCenter<<tfEndEllipsis;
		Sender->Canvas->TextRect( ItemRect, DrawString, Formats );
		for( int i = 1; i <= 7; i ++ )
			 ProcessListViewCustomDrawSubItem( Sender, Item, i, CDState, ErrorProcess );
	}
	catch(...)
	{
		ShowMessage( "Exception ProcessListViewDrawItem" );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::OtherProcessListViewMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	TListItem* SelItem = OtherProcessListView->GetItemAt( 10, Y );

	if( SelItem )
	{
		TRect  ItemRect,ItemRect2;
		String Msg;
		TPoint PT( X, Y );

		ListView_GetSubItemRect( OtherProcessListView->Handle, SelItem->Index, 2,LVIR_BOUNDS,&ItemRect );
		ListView_GetSubItemRect( OtherProcessListView->Handle, SelItem->Index, 3,LVIR_BOUNDS,&ItemRect2 );
		///< Setting process Start/Stop time
		if( PtInRect( ItemRect, PT ) || PtInRect( ItemRect2, PT ) )
		{
			if( ProcessSetting == false )
			{
				StatusBar->Panels->Items[0]->Text = Scstrings_MAIN_PROCESS_NO_RIGHTS_CHANGE_STOP_START;
				return;
			}
			ProcessForm = new TProcessForm( SelItem->Caption, FOtherProcessStartTime[ SelItem->Index ],FOtherProcessStopTime[ SelItem->Index ],FOtherProcessAutoStart[ SelItem->Index ]);
			if( ProcessForm->ShowModal() == mrOk )
			{
				FOtherProcessStartTime[ SelItem->Index ] = ProcessForm->GetStartTime();
				FOtherProcessStopTime[ SelItem->Index ]  = ProcessForm->GetStopTime();
				FOtherProcessAutoStart[ SelItem->Index ] = ProcessForm->GetAutoStart();
				OtherProcessListView->Invalidate();
				UpdateOtherProcess( SelItem );
			}
			delete ProcessForm;
			return;
		}
		if( ProcessControl == false )
		{
			this->StatusBar->Panels->Items[0]->Text = Scstrings_MAIN_PROCESS_NO_RIGHTS_STOP_PROCESS;
			return;
		}
		///< Start process
		ListView_GetSubItemRect( OtherProcessListView->Handle, SelItem->Index, 4,LVIR_BOUNDS,&ItemRect);
		if( PtInRect( ItemRect, PT ))
		{
			Msg.printf( Scstrings_MAIN_PROCESS_START_ASK.c_str(), FOtherProcessList->Strings[ SelItem->Index ] );
			if( FormSetting->Confirm( Msg ) == true )
			{
				if( FOtherProcessState[ SelItem->Index ] == PROCESS_RUNNING )
				{
					Msg.printf( Scstrings_MAIN_PROCESS_RUNNING_STOP_RESTERT_FIRST.c_str() ,FOtherProcessList->Strings[ SelItem->Index ].c_str());
					ShowMessage( Msg );
					return;
				}
				else
				{
					ChangeOtherProcessState( SelItem->Index, PROCESS_STARTING );
					SendShellCMD( SPEEDY_RUN_PROCESS, SelItem->Index, false );
					Msg.printf(L"Start Process:%s",FOtherProcessList->Strings[ SelItem->Index ] );
					WriteUserLog("war",Msg);
					return;
				}
			}
		}
		ListView_GetSubItemRect( OtherProcessListView->Handle, SelItem->Index, 5,LVIR_BOUNDS,&ItemRect);
		if( PtInRect( ItemRect, PT ))
		{
			Msg.printf( Scstrings_MAIN_PROCESS_STOP_ASK.c_str(),FOtherProcessList->Strings[ SelItem->Index ] );
			if( FormSetting->Confirm( Msg ) == true )
			{
				if( FOtherProcessState[ SelItem->Index ] == PROCESS_STOPED )
				{
					Msg.printf( Scstrings_MAIN_PROCESS_ALREADY_STOPED.c_str(),FOtherProcessList->Strings[ SelItem->Index ] );
					ShowMessage( Msg );
					return;
				}
				else
				{
					ChangeOtherProcessState( SelItem->Index, PROCESS_STOPING );
					SendShellCMD( SPEEDY_KILL_PROCESS, SelItem->Index , false);
					Msg.printf(L"Stop Process:%s",FOtherProcessList->Strings[ SelItem->Index ] );
					WriteUserLog("war",Msg);
					return;
				}
			}
		}
		ListView_GetSubItemRect( OtherProcessListView->Handle, SelItem->Index, 6,LVIR_BOUNDS,&ItemRect);
		if( PtInRect( ItemRect, PT ))
		{
			Msg.printf( Scstrings_MAIN_PROCESS_RESTART_ASK.c_str(),FOtherProcessList->Strings[ SelItem->Index ].c_str());
			if( FormSetting->Confirm( Msg ) == true )
			{
				ChangeOtherProcessState( SelItem->Index, PROCESS_RESTARTING );
				SendShellCMD( SPEEDY_RESTART_PROCESS, SelItem->Index, false );
				Msg.printf( L"Restart Process:%s",FOtherProcessList->Strings[ SelItem->Index ] );
				WriteUserLog("war",Msg);
				return;
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::OtherProcessListViewMouseMove(TObject *Sender,
	  TShiftState Shift, int X, int Y)
{
	TListItem*  Item = OtherProcessListView->GetItemAt( X, Y);
	if( Item )
	{
		RECT ItemRect;

		for( int i = 2; i <= 5; i++ )
		{
			ListView_GetSubItemRect( OtherProcessListView->Handle, Item->Index, i, LVIR_BOUNDS,&ItemRect);
			if( X > ItemRect.left && X < ItemRect.right )
			{
				OtherProcessListView->Cursor = crHandPoint;
				return;
			}
		}
	}
	OtherProcessListView->Cursor = crDefault;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::OtherProcessListViewDrawItem(
	  TCustomListView *Sender, TListItem *Item, TRect &Rect,
	  TOwnerDrawState State)
{
	try
	{
		TCustomDrawState CDState;
		bool             DefaultDraw;
		String           DrawString = Item->Caption;

		if( State.Contains( odSelected   ))
		{
			CDState << cdsSelected;
			Sender->Canvas->Brush->Style = bsClear;
			DrawGradientBar( Sender->Canvas->Handle , Rect, clWhite, clSilver );
		}
		else
		{
			Sender->Canvas->Brush->Style = bsSolid;
			Sender->Canvas->Brush->Color = UserListView->Color;
			Sender->Canvas->FillRect( Rect );
		}
		SetTextColor( Sender->Canvas->Handle,(COLORREF)clNavy );
		ImageList->Draw( Sender->Canvas, Rect.left, Rect.top + 1, 4, true);
		Rect.Left += (ImageList->Width + 2);
		DrawTextExW( Sender->Canvas->Handle, DrawString.c_str(), DrawString.Length(),&Rect,DT_SINGLELINE|DT_VCENTER|DT_LEFT,NULL);
		for( int i = 1; i <= 6; i ++ )
			 OtherProcessListViewCustomDrawSubItem( Sender,Item,i,CDState, DefaultDraw);
	}
	catch(...)
	{
		ShowMessage( "Exception OtherProcessListViewDrawItem" );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::StatusBarDrawPanel(TStatusBar *StatusBar,
	  TStatusPanel *Panel, const TRect &Rect)
{
	TTextFormat Formats;
	String TextOut = Panel->Text;

	Formats<<tfSingleLine<<tfLeft<<tfVerticalCenter;
	StatusBar->Canvas->Font->Color = clNavy;
	StatusBar->Canvas->TextRect( (TRect&)Rect , TextOut ,Formats );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FIXSessionsListViewDrawItem(
	  TCustomListView *Sender, TListItem *Item, TRect &Rect,
	  TOwnerDrawState State)
{
	try
	{
		TCustomDrawState CDState;
		bool             DefaultDraw;
		String           DrawString = Item->Caption;

		if( State.Contains( odSelected ))
		{
			FIXSessionsListView->Canvas->Brush->Color = clNavy;
			SetTextColor( Sender->Canvas->Handle,(COLORREF)clWhite );
		}
		else
		{
			if( Item->Index%2 )
				FIXSessionsListView->Canvas->Brush->Color = clSilver;
			else
				FIXSessionsListView->Canvas->Brush->Color = clWhite;
			SetTextColor( Sender->Canvas->Handle,(COLORREF)clNavy );
		}
		Sender->Canvas->FillRect( Rect );
		if( Item->StateIndex > 0 )
			FIXSessionsListView->StateImages->Draw(  Sender->Canvas, Rect.left, Rect.top + 1, Item->StateIndex, true );
		Rect.Left += (FIXSessionsListView->StateImages->Width +2);
		ImageList->Draw( Sender->Canvas, Rect.left , Rect.top + 1, 4, true);
		Rect.Left += (ImageList->Width+2);
		DrawTextExW( Sender->Canvas->Handle, DrawString.c_str(), DrawString.Length(),&Rect,DT_SINGLELINE|DT_VCENTER|DT_LEFT,NULL);
		for( int i = 1; i <= 9; i ++ )
			 FIXSessionsListViewCustomDrawSubItem( Sender,Item,i,CDState, DefaultDraw);
	}
	catch(...)
	{
		ShowMessage( "Exception FIXSessionsListViewDrawItem" );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::UpdateAEStatus( MTree* Tree )
{
	UFCType::Int32 ItemCount, TotalCount, ConnectedCount;

	if( Tree->get( "Count", ItemCount ) )
	{
		UFC::AnsiString  Fields;	
		TListItem*       Item;
		String           AEName, Connections;
		bool             Locked;

		for( int i = 0; i < ItemCount; i ++ )
		{
			UFC::AnsiString FieldName( i );
			if( Tree->get( FieldName, Fields ) )
			{
				TStringList* Strs = new TStringList();
				Strs->Delimiter = ',';
				Strs->DelimitedText = Fields.c_str();
				if( Strs->Count == 3 )
				{
					AEName      = Strs->Strings[ 0 ];
					Locked      = (bool)Strs->Strings[ 1 ].ToInt();
					Connections = Strs->Strings[ 2 ].ToInt();
					for( register int j= 0; j < AccountsListView->Items->Count; j++ )
					{
						Item = AccountsListView->Items->Item[j];
						if( Item->Caption == AEName )
						{
							int BeforConnection = Item->SubItems->Strings[ 3 ].ToInt();
							if( Locked == true )
								Item->Data = (void*)1;
							else
								Item->Data = NULL;
							Item->SubItems->Strings[ 3 ] = Connections;
							Item->Update();
							if( Connections <  BeforConnection )
							{
								String Msg;

								Msg.printf( L"API User[%s] disconnected.", AEName );
								WarningForm->LogError( FSpeedyHost, FSpeedyHostIP,"E06",Msg );
							}
							break;
						}
					}					
				}
				delete Strs; 
			}
		}
	}
	if( Tree->get( "Total", TotalCount ) && Tree->get( "Connected", ConnectedCount ) )
	{
		String DispText;
		UFCType::Int32 LeavesCID;

		if( Tree->get( "LCID", LeavesCID ) )
		{
			if( LeavesCID <= FormSetting->GetCIDLimit() )
			{
				if( abs( FLastAlarmCID - LeavesCID ) >= 10 )
				{
					FLastAlarmCID = LeavesCID;
					DispText.printf( Scstrings_MAIN_CID_WARNING.c_str(), LeavesCID );
					WarningForm->ShowWarning( FSpeedyHost, DispText, FormSetting->GetWarningSound() );
					WarningForm->LogError( FSpeedyHost, FSpeedyHostIP,"E06",DispText );
				}
			}
			DispText.printf( Scstrings_MAIN_AE_CURRENT_MAX_CONNECTION_CID.c_str(),ConnectedCount, TotalCount, LeavesCID );
		}
		else
			DispText.printf( Scstrings_MAIN_AE_CURRENT_MAX_CONNECTION.c_str(),ConnectedCount, TotalCount );
		ConnectionLabel->Caption = DispText;
    }
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::AccountsListViewCustomDrawSubItem(
	  TCustomListView *Sender, TListItem *Item, int SubItem,
	  TCustomDrawState State, bool &DefaultDraw)
{
	DWORD   Style;
	RECT    ItemRect;
	TColor  TextColor = clNavy;
	String  DrawText = Item->SubItems->Strings[ SubItem -1];

	DefaultDraw = false;
	ListView_GetSubItemRect( Sender->Handle, Item->Index, SubItem,LVIR_BOUNDS,&ItemRect);
	switch( SubItem )
	{
		case 1: Style = DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_END_ELLIPSIS;
				TextColor = clBlack;
				break;  ///< Type
		case 2: Style = DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_END_ELLIPSIS;
				TextColor = clGreen;
				break;///< Source
		case 3: Style = DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS;break;///< Accepted IP
		case 4: Style = DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS;
				if( DrawText.ToInt() >= 3 )
					TextColor = clRed;
				else
					TextColor = clGreen;
				break;///< Connections
		case 5: Style = DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_END_ELLIPSIS;break;  ///< Accounts
		case 6: Style = DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_END_ELLIPSIS;break;  ///< Begin OrderID
		case 7: Style = DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_END_ELLIPSIS;break;  ///< End OrderID
	}
	if( Item->Selected )
	{
		Sender->Canvas->Brush->Color = clNavy;
		SetTextColor( Sender->Canvas->Handle,(COLORREF)clWhite );
	}
	else
	{
		if( Item->Index %2 )
			Sender->Canvas->Brush->Color = clSilver;
		else
			Sender->Canvas->Brush->Color = FIXSessionsListView->Color;
		SetTextColor( Sender->Canvas->Handle,(COLORREF)TextColor );
	}
	Sender->Canvas->FillRect( ItemRect );
	if( SubItem == 1 )
	{
		if( Item->Data == NULL )
			ImageList->Draw( Sender->Canvas, ItemRect.left, ItemRect.top, 11, true );
		else
			ImageList->Draw( Sender->Canvas, ItemRect.left, ItemRect.top, 12, true );
		ItemRect.left += 24;
	}
	DrawTextExW( Sender->Canvas->Handle, DrawText.c_str(), DrawText.Length(), &ItemRect, Style, NULL );
}
//---------------------------------------------------------------------------

void __fastcall TSimTFXForm::AccountsListViewDrawItem(TCustomListView *Sender,
	  TListItem *Item, TRect &Rect, TOwnerDrawState State)
{
	TCustomDrawState CDState;
	bool             DefaultDraw;
	String           DrawString = Item->Caption;

	if( State.Contains( odSelected ))
	{
		Sender->Canvas->Brush->Color = clNavy;
		SetTextColor( Sender->Canvas->Handle,(COLORREF)clWhite );
	}
	else
	{
		if( Item->Index%2 )
			Sender->Canvas->Brush->Color = clSilver;
		else
			Sender->Canvas->Brush->Color = clWhite;
		SetTextColor( Sender->Canvas->Handle,(COLORREF)clBlack );
	}
	Sender->Canvas->FillRect( Rect );
	AccountsListView->SmallImages->Draw( Sender->Canvas, Rect.left , Rect.top + 1, Item->ImageIndex, true);
	Rect.Left += (AccountsListView->SmallImages->Width+2);
	DrawTextExW( Sender->Canvas->Handle, DrawString.c_str(), DrawString.Length(),&Rect,DT_SINGLELINE|DT_VCENTER|DT_LEFT,NULL);
	for( int i = 1; i <= 7; i ++ )
		 AccountsListViewCustomDrawSubItem( Sender,Item,i,CDState, DefaultDraw);
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::LogListBoxMeasureItem(TWinControl *Control,
	  int Index, int &Height)
{
	RECT       Rect;
	AnsiString Text = LogListBox->Items->Strings[ Index ];

	Rect.left   = 0;
	Rect.right  = FIX_MSG_WIDTH-2;
	Rect.top    = 0;
	Rect.bottom = 0;
	DrawTextA( FListBuffer->Canvas->Handle,
			   Text.c_str(),
			   Text.Length(),
			   &Rect, DT_CALCRECT|DT_LEFT|DT_WORDBREAK  );//DT_NOFULLWIDTHCHARBREAK );
	Height = (Rect.bottom - Rect.top )+ 4;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::LogListBoxClick(TObject *Sender)
{
	TLineInfo* Line = (TLineInfo*)FLinkPVCToPanel->Objects[ LineComboBox->ItemIndex ];
	if( Line->IsTCP( ) == true )
	{
		int Index = LogListBox->ItemIndex;
		if( Index != -1 )
		{
			if( Line->GetSystem( ) == mtFutures || Line->GetSystem( ) == mtOption )
			{
				TagForm->SetTMPMessage( LogListBox->Items->Strings[Index] );
				TagForm->Show();
			}
			else if( Line->IsFIX() == true )
			{
				TagForm->SetTWSEFIXMessage( LogListBox->Items->Strings[Index] );
				TagForm->Show();
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::LogClearMenuItemClick(TObject *Sender)
{
	LogListBox->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::PVCLogTimerTimer(TObject *Sender)
{
	if( FPVCLogList->Count > 0 )
	{
		LogListBox->Items->BeginUpdate();
			FPVCLogCS->Enter();
				for( int j=0;j< FPVCLogList->Count; j ++ )
				{
					AnsiString Data = FPVCLogList->Strings[ j ];
					for( register int i =1 ; i <= Data.Length(); i++ )
					 if( Data[ i ] == '|' )
						 Data[ i ] = ' ';
					LogListBox->Items->Add( Data );
				}
				FPVCLogList->Clear();
			FPVCLogCS->Leave();
		LogListBox->Items->EndUpdate();
		LogListBox->TopIndex = LogListBox->Count - 6; ///< Scroll to the end
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ResendPageControlChange(TObject *Sender)
{
	if( FLEXSysComboBox->ItemIndex == 0 )
		OPTPanel->Visible = false;
	else
		OPTPanel->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ResendB30ButtonClick(TObject *Sender)
{
	AnsiString ReqStr,FileCode;
	MTree Data;

	if( SessionComboBox->ItemIndex == 0 )
		Data.append( "TradeKind", 0 );
	else
		Data.append( "TradeKind", 1 );
	if( PartIDComboBox->ItemIndex == 0 )
	{
		FileCode = "B30";
		ReqStr.printf("B30%05d%05d%010d%010d", B30FCMEdit->Text.ToInt(),
											   B30SessionEdit->Text.ToInt(),
											   B30BeginEdit->Text.ToInt(),
											   B30EndEdit->Text.ToInt());
		Data.append( "FLEX", (UFCType::Int32)0 );
	}
	else
	{
		FileCode = "XB30";
		ReqStr.printf("XB30%03d%05d%05d%010d%010d",PartIDComboBox->Text.ToInt(), ///< PartID
												   B30FCMEdit->Text.ToInt(),     ///< fcm_id
												   B30SessionEdit->Text.ToInt(), ///< session_id
												   B30BeginEdit->Text.ToInt(),   ///< start_seq
												   B30EndEdit->Text.ToInt());    ///< end_seq
		Data.append( "FLEX", (UFCType::Int32)1 );
	}
	Data.append( "Body", ReqStr.c_str() );
	Data.append( "ReqID", (UFCType::Int32)GetTickCount() );

	if( B30SysComboBox->ItemIndex == 0 )
	{
		ChangeFileListViewStatus( FileCode, FTP_WAITING, false );
		SpeedyFutFtpPublisher->SendData( &Data );
	}
	else
	{
		ChangeFileListViewStatus( FileCode, FTP_WAITING, true );
		SpeedyOptFtpPublisher->SendData( &Data );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::UserListViewDrawItem(TCustomListView *Sender,
	  TListItem *Item, TRect &Rect, TOwnerDrawState State)
{
	TCustomDrawState CDState;
	bool             DefaultDraw;
	String           DrawString = Item->Caption;

	if( State.Contains( odSelected   ))
	{
		CDState << cdsSelected;
		Sender->Canvas->Brush->Color = clSilver;
	}
	else
		Sender->Canvas->Brush->Color = UserListView->Color;
	Sender->Canvas->FillRect( Rect );
	SetTextColor( Sender->Canvas->Handle,(COLORREF)clNavy );
	ImageList->Draw( Sender->Canvas, Rect.left, Rect.top + 1, Item->ImageIndex, true);
	Rect.Left += (ImageList->Width + 2);
	DrawTextExW( Sender->Canvas->Handle, DrawString.c_str(), DrawString.Length(),&Rect,DT_SINGLELINE|DT_VCENTER|DT_LEFT,NULL);
	for( int i = 1; i <= 4; i ++ )
		 UserListViewCustomDrawSubItem( Sender,Item,i,CDState, DefaultDraw);
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FIXSessionsListViewContextPopup(TObject *Sender,
	  TPoint &MousePos, bool &Handled)
{
	if( FIXControl == false )
		Handled = true;
	else
	{
		if( FIXSessionsListView->ItemIndex != -1 )
		{
			TFIXSession* Session = (TFIXSession*)FFIXSessions->Objects[FIXSessionsListView->ItemIndex];
			TInitiator*  Initiator = dynamic_cast<TInitiator*>(Session);

			if( Initiator != NULL && Initiator->HostCount() > 1 )
				NextConnectionMenuItem->Visible = true;
			else
				NextConnectionMenuItem->Visible = false;
		}
		else
			NextConnectionMenuItem->Visible = false;
		Handled = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ProfileSubscriberMessage(const MString &Subject,
	  const MString &Key, MTree *Tree)
{
	TProfileFrame*  ProfilePanel;
	UFCType::UInt32 TickNow;
	UFCType::Int32  Count;

	if( Tree->get( "Tick", TickNow ) && Tree->get("Count", Count ) )
	{
		for( int i = 0; i < Count; i ++ )
		{
			UFC::AnsiString ValName( i+1 );
			UFC::AnsiString Value;

			if( Tree->get( ValName, Value ) && i < FGraphPanels->Count )
			{
				if( (ProfilePanel = dynamic_cast<TProfileFrame*>(FGraphPanels->Items[i])) != NULL )
					ProfilePanel->SetData( TickNow, Value.c_str() );
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FormActivate(TObject *Sender)
{
	if( FOnMDIActive != NULL )
		FOnMDIActive( this );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::AESettingButtonClick(TObject *Sender)
{
	AnsiString       ConfigFileName = GetUserConfigFileName();
	UFC::UiniFile    ini( ConfigFileName.c_str() );
	UFC::Section*    iniSection = ini.GetSection( "Speedy");
	UFC::Section*    OHiniSection = ini.GetSection( "SpeedyOffHour");

	if( iniSection == NULL || OHiniSection == NULL )
	{
		if( iniSection == NULL )
		{
			ini.SetValue("Speedy","Type","Admin");
			ini.SetValue("Speedy","Password","1234rewq");
			ini.SetValue("Speedy","Source","0");
			ini.SetValue("Speedy","Checker","null");
			ini.SetValue("Speedy","DefCheckAction","0");
			ini.SetValue("Speedy","IP","127.0.0.1");
			iniSection = ini.GetSection( "Speedy");
		}
		if( OHiniSection == NULL )
		{
			ini.SetValue("SpeedyOffHour","Type","Admin");
			ini.SetValue("SpeedyOffHour","Password","1234rewq");
			ini.SetValue("SpeedyOffHour","Source","0");
			ini.SetValue("SpeedyOffHour","Checker","null");
			ini.SetValue("SpeedyOffHour","DefCheckAction","0");
			ini.SetValue("SpeedyOffHour","IP","127.0.0.1");
			OHiniSection = ini.GetSection( "SpeedyOffHour");
		}
		FModifyType = mtNewUser;
	}
	else
		FModifyType = mtModifyUser;
	if( AccountForm != NULL )
		delete AccountForm;
	if( FormSetting->ForceRM() == true )
	{
		if( FCheckers->Strings[ 0 ] == Scstrings_MAIN_RM_NOT_EXISTS )
			FCheckers->Delete( 0 );
	}
	else
	{
		if( FCheckers->Strings[ 0 ] != Scstrings_MAIN_RM_NOT_EXISTS )
			FCheckers->Insert( 0, Scstrings_MAIN_RM_NOT_EXISTS);
	}
	AccountForm = new TAccountForm( this, iniSection, OHiniSection, FCheckers,FormSetting->Sources );
	if( AccountForm->ShowModal() == mrOk )
	{
		LockUserUI( true );
		RequestUserConfigFile();
	}
	else
	{
		delete AccountForm;
		AccountForm = NULL;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::MessageObjectMBusStartup(const MString &Hostname)
{
	String StartupHost( Hostname.c_str());

	for( int i =  ServersToolBar->ButtonCount -1; i >= 0; i--)
	{
		TToolButton* ServerBtn = ServersToolBar->Buttons[i];
		if( ServerBtn->Caption == StartupHost )
		{
			ServerBtn->Enabled = true;
			ServersToolBar->Repaint();
			return;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::MessageObjectMBusStopped(const MString &Hostname)
{
	String StartupHost( Hostname.c_str());

	for( int i =  ServersToolBar->ButtonCount -1; i >= 0; i--)
	{
		TToolButton* ServerBtn = ServersToolBar->Buttons[i];
		if( ServerBtn->Caption == StartupHost )
		{
			ServerBtn->Enabled = false;
			ServersToolBar->Repaint();
			return;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::MessageObjectMBusList(const MString &Hostname, TStringList *List)
{
	for( int j = 0;  j < List->Count; j ++ )
	{
		String StartupHost = List->Strings[j];

		for( int i =  ServersToolBar->ButtonCount -1; i >= 0; i--)
		{
			TToolButton* ServerBtn = ServersToolBar->Buttons[i];
			if( ServerBtn->Caption == StartupHost )
				ServerBtn->Enabled = true;
		}
	}
	ServersToolBar->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ToolButtonClick(TObject *Sender)
{
	TToolButton* ServerBtn = dynamic_cast<TToolButton*>(Sender);
	String       CurrentHost( FSpeedyHost.c_str());

	if( ServerBtn != NULL )
	{
		if( CurrentHost != ServerBtn->Caption )
		{
			String Msg;

			Msg.printf( Scstrings_MAIN_MONITOR_ANOTHER_SVR_ASK.c_str(), ServerBtn->Caption );
			if( MessageDlg( Msg, mtConfirmation, TMsgDlgButtons() << mbOK<<mbCancel, 0) == mrOk )
			{
				String NewHost = ServerBtn->Caption;
				MonitorServer( NewHost );
			}
		}
	}
}
//---------------------------------------------------------------------------
bool __fastcall TSimTFXForm::RequestCancelAll( bool IsOptions, bool IsOffHour, String Password, String Account  )
{
	AnsiString AnsiPassword = Password;
	AnsiString AnsiAccount  = Account;

	if( AnsiPassword.AnsiCompare( FPassword ) == 0 ) ///< Pass Password check.
	{
		MTree  Data;
		String Msg;

		if( IsOptions == true )
		{
			Data.append( "COMMAND", SPEEDY_CANCEL_OPT_ORDERS );
			if( AnsiAccount.Length() > 0 ) ///< Cancel by Account ?
			{
				Data.append( "Account", AnsiAccount.c_str() );
				Msg.printf( Scstrings_MAIN_CALCEL_OPT_ALL_BY_ACCOUNT.c_str(),String(FUser),String(UFC::Hostname), String(AnsiAccount) );
			}
			else
				Msg.printf( Scstrings_MAIN_CALCEL_ALL_OPT_ORDERS.c_str(),String(FUser),String(UFC::Hostname) );
		}
		else
		{
			Data.append( "COMMAND", SPEEDY_CANCEL_FUT_ORDERS );
			if( AnsiAccount.Length() > 0 ) ///< Cancel by Account ?
			{
				Data.append( "Account", AnsiAccount.c_str() );
				Msg.printf( Scstrings_MAIN_CALCEL_FUT_ALL_BY_ACCOUNT.c_str(),String(FUser),String(UFC::Hostname),String(AnsiAccount));
			}
			else
				Msg.printf( Scstrings_MAIN_CALCEL_ALL_FUT_ORDERS.c_str(),String(FUser),String(UFC::Hostname) );
		}
		Data.append( "HOST", FSpeedyHost.c_str() );
		Data.append( "OFFHOUR", (int)IsOffHour );
		SpeedyAgentPublisher->SendData( &Data );
		AddEventMessage( Msg );
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::RequestKillSwitchList( bool IsOptions, bool IsOffHour )
{
	MTree Data;

	Data.append( "COMMAND", AGENT_REQ_KILLSWITCH );
	Data.append( "SYS", (int)IsOptions );
	Data.append( "OFFHOUR", (int)IsOffHour );
	MessageObject->Send( "SPEEDY.COMMAND",FSpeedyHost.c_str(), &Data );
}
//---------------------------------------------------------------------------
bool __fastcall TSimTFXForm::RequestKillSwitch( bool IsOptions, bool IsOffHour, bool Enable, String Password, String FCM  )
{
	AnsiString AnsiPassword = Password;

	if( AnsiPassword.AnsiCompare( FPassword ) == 0 ) ///< Pass Password check.
	{
		MTree Data;
		AnsiString FCMID( FCM.c_str());

		Data.append( "COMMAND", AGENT_PVC_KILLSWITCH );
		Data.append( "SYS", (int)IsOptions );
		Data.append( "FCM", FCMID.c_str() );
		Data.append( "ENABLE", (int)Enable );
		Data.append( "OFFHOUR", (int)IsOffHour );
		MessageObject->Send( "SPEEDY.COMMAND",FSpeedyHost.c_str(), &Data );
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TSimTFXForm::RequestKillSwitch( bool IsOptions, bool IsOffHour, bool Enable, String Password, String FCM, String SFCM, String SID  )
{
	AnsiString AnsiPassword = Password;

	if( AnsiPassword.AnsiCompare( FPassword ) == 0 ) ///< Pass Password check.
	{
		MTree Data;
		AnsiString FCMID( FCM.c_str());
		AnsiString SFCMID( SFCM.c_str());
		AnsiString SessionID( SID.c_str());

		Data.append( "COMMAND", AGENT_PVC_KILLSWITCH );
		Data.append( "SYS", (int)IsOptions );
		Data.append( "FCM", FCMID.c_str() );
		Data.append( "SFCM", SFCMID.c_str() );
		Data.append( "SID", SessionID.c_str() );
		Data.append( "ENABLE", (int)Enable );
		Data.append( "OFFHOUR", (int)IsOffHour );
		MessageObject->Send( "SPEEDY.COMMAND",FSpeedyHost.c_str(), &Data );
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::UpdatProxyInfo( const MString& ProxyName, MTree *Tree )
{
	TProxyInfo* UpdateProxy;
	int         ProxyIndex = FProxys->IndexOf( ProxyName.c_str() );

	if( ProxyIndex == -1 )
	{
		UpdateProxy = new TProxyInfo( this, ProxyName );
		FProxys->AddObject( ProxyName.c_str(),UpdateProxy );
		ProxyTabSet->Tabs->AddObject( ProxyName.c_str(), UpdateProxy );
		ProxyTabSet->Invalidate();
		UpdateProxy->UpdateTabIndex();
		UpdateProxy->Update( Tree );
		if( ProxyTabSet->Tabs->Count == 1 )
			UpdateProxy->Active();
	}
	else
	{
		UpdateProxy = (TProxyInfo*)FProxys->Objects[ ProxyIndex ];
		UpdateProxy->Update( Tree );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ModifyThroughput( const MString& ProxyName, MTree *Tree )
{
	MTree SendData;

	ThroughputForm->Load( Tree );
	if( ThroughputForm->ShowModal() == mrOk )
	{
		ThroughputForm->Save( &SendData );
		SendData.append( "CMD", CMD_PROXY_THROUGHPUT_UPDATE );
		MessageObject->Send( "SPEEDYCENTER.CMD",ProxyName.c_str(),&SendData );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ProxySubscriberMessage(const MString &Subject, const MString &Key,
		  MTree *Tree)
{
   UFC::AnsiString ProxyName( "SpeedyProxy" );
   int DefTP;

   Tree->get( "NAME", ProxyName );
   if( Tree->get( "Default", DefTP ) == false )
	   UpdatProxyInfo( ProxyName, Tree );
   else
	   ModifyThroughput( ProxyName, Tree );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ClearProxyPage( void )
{
   MTree Tree;

   CIDStaticText->Caption = L"-----";
   CIDMaxStaticText->Caption = L"-----";
   ConnectionStaticText->Caption = L"-----";
   ConnectionMaxStaticText->Caption = L"-----";
   NewOrdCountStaticText->Caption = L"-----";
   CancelOrdCountStaticText->Caption = L"-----";
   FillOrdStaticText->Caption = L"-----";
   ProxyUsersListView->Items->Count = 0;
   ProxyUsersListView->Clear();
   FProxys->Clear();
   ProxyTabSet->Tabs->Clear();
   UpdatProxyInfo( "SpeedyProxy", &Tree  );
}
//---------------------------------------------------------------------------

void __fastcall TSimTFXForm::ProxyUsersListViewAdvancedCustomDrawItem(TCustomListView *Sender,
          TListItem *Item, TCustomDrawState State, TCustomDrawStage Stage,
          bool &DefaultDraw)
{
	if( Item->Index%2 )
		ProxyUsersListView->Canvas->Brush->Color = clSilver;
	else
		ProxyUsersListView->Canvas->Brush->Color = clWhite;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ProxyUsersListViewCustomDrawSubItem(TCustomListView *Sender,
          TListItem *Item, int SubItem, TCustomDrawState State, bool &DefaultDraw)

{
	DWORD   Style;
	RECT    ItemRect;
	TColor  TextColor = clNavy;
	String  DrawText = Item->SubItems->Strings[ SubItem -1];
	bool    Locked;

	DefaultDraw = false;
	ListView_GetSubItemRect( Sender->Handle, Item->Index, SubItem,LVIR_BOUNDS,&ItemRect);
	switch( SubItem )
	{
		case 1: Style = DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_END_ELLIPSIS;
				Locked = DrawText.ToInt();
				if( Locked == true  )
				{
					DrawText  = Scstrings_MAIN_LOGON_ID_STATUS_LOCKED;
					TextColor = clRed;
				}
				else
				{
					DrawText  = Scstrings_MAIN_LOGON_ID_STATUS_NORMAL;
					TextColor = clBlack;
				}
				break;  ///< Locked
		case 2: Style = DT_SINGLELINE|DT_VCENTER|DT_RIGHT|DT_END_ELLIPSIS;
				if( DrawText.ToInt() >= 5 )
					TextColor = clRed;
				else
					TextColor = clGreen;
				break;///< Logon Count
		case 3: Style = DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS;
				if( DrawText.ToInt() == 1 )
				{
					DrawText  = Scstrings_MAIN_SESSION_STAT_CONNECTED;///L"已連線";
					TextColor = clGreen;
				}
				else
				{
					DrawText  = Scstrings_MAIN_SESSION_STAT_BROKEN;///L"斷線";
					TextColor = clNavy;
				}
				break; ///< Connected/disconnected.
		case 4: Style = DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_END_ELLIPSIS;
				TextColor = clNavy;
				break; ///< Version
	}
	if( Item->Selected )
	{
		Sender->Canvas->Brush->Color = clNavy;
		SetTextColor( Sender->Canvas->Handle,(COLORREF)clWhite );
	}
	else
	{
		if( Item->Index %2 )
			Sender->Canvas->Brush->Color = clSilver;
		else
			Sender->Canvas->Brush->Color = ProxyUsersListView->Color;
		SetTextColor( Sender->Canvas->Handle,(COLORREF)TextColor );
	}
	Sender->Canvas->FillRect( ItemRect );
	if( SubItem == 1 )
	{
		if( Locked == false )
			ImageList->Draw( Sender->Canvas, ItemRect.left, ItemRect.top, 11, true ); ///< Unlock
		else
			ImageList->Draw( Sender->Canvas, ItemRect.left, ItemRect.top, 12, true ); ///< Locked
		ItemRect.left += 24;
	}
	DrawTextExW( Sender->Canvas->Handle, DrawText.c_str(), DrawText.Length(), &ItemRect, Style, NULL );
}
//---------------------------------------------------------------------------

void __fastcall TSimTFXForm::ProxyUsersListViewDrawItem(TCustomListView *Sender, TListItem *Item,
          TRect &Rect, TOwnerDrawState State)
{
	TCustomDrawState CDState;
	bool             DefaultDraw;
	String           DrawString = Item->Caption;

	if( State.Contains( odSelected ))
	{
		Sender->Canvas->Brush->Color = clNavy;
		SetTextColor( Sender->Canvas->Handle,(COLORREF)clWhite );
	}
	else
	{
		if( Item->Index%2 )
			Sender->Canvas->Brush->Color = clSilver;
		else
			Sender->Canvas->Brush->Color = clWhite;
		SetTextColor( Sender->Canvas->Handle,(COLORREF)clBlack );
	}
	Sender->Canvas->FillRect( Rect );
	ProxyUsersListView->SmallImages->Draw( Sender->Canvas, Rect.left , Rect.top + 1, 3, true);
	Rect.Left += (ProxyUsersListView->SmallImages->Width+2);
	DrawTextExW( Sender->Canvas->Handle, DrawString.c_str(), DrawString.Length(),&Rect,DT_SINGLELINE|DT_VCENTER|DT_LEFT,NULL);
	for( int i = 1; i <= 4; i ++ )
		 ProxyUsersListViewCustomDrawSubItem( Sender,Item,i,CDState, DefaultDraw);
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ProxyUsersListViewMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y)
{
	if( Button == mbRight )
	{
		TPoint     ClientMousePos( X, Y );
		TListItem* SelItem = ProxyUsersListView->GetItemAt( 10, Y );

		if( SelItem != NULL )
		{
			TPoint ScreenPos = ProxyUsersListView->ClientToScreen( ClientMousePos );
			bool   IsLock    = SelItem->SubItems->Strings[0].ToInt();

			if( IsLock == true )
			{
				UnlockUserMenuItem->Visible = true;
				LockUserMenuItem->Visible   = false;
			}
			else
			{
				UnlockUserMenuItem->Visible = false;
				LockUserMenuItem->Visible   = true;
			}
			ProxyPopupMenu->Tag = SelItem->Index;
			ProxyPopupMenu->Popup( ScreenPos.x , ScreenPos.y );
		}
		else
			ProxyPopupMenu->Tag = -1;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::LockUserMenuItemClick(TObject *Sender)
{
	if( ProxyPopupMenu->Tag != -1 )
	{
		TListItem* SelItem = ProxyUsersListView->Items->Item[ ProxyPopupMenu->Tag ];
		AnsiString UserID( SelItem->Caption );
		MTree      Data;

		Data.append( "CMD", CMD_PROXY_LOCK_USER );
		Data.append( "ID", UserID.c_str() );
		MessageObject->Send( "SPEEDYCENTER.CMD",FSpeedyHost.c_str(),&Data );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::UnlockUserMenuItemClick(TObject *Sender)
{
	if( ProxyPopupMenu->Tag != -1 )
	{
		TListItem* SelItem = ProxyUsersListView->Items->Item[ ProxyPopupMenu->Tag ];
		AnsiString UserID( SelItem->Caption );
		MTree      Data;

		Data.append( "CMD", CMD_PROXY_UNLOCK_USER );
		Data.append( "ID", UserID.c_str() );
		MessageObject->Send( "SPEEDYCENTER.CMD",FSpeedyHost.c_str(),&Data );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::BrokenUserMenuItemClick(TObject *Sender)
{
	if( ProxyPopupMenu->Tag != -1 )
	{
		TListItem* SelItem = ProxyUsersListView->Items->Item[ ProxyPopupMenu->Tag ];
		AnsiString UserID( SelItem->Caption );
		MTree      Data;

		Data.append( "CMD", CMD_PROXY_BROKEN_USER );
		Data.append( "ID", UserID.c_str() );
		MessageObject->Send( "SPEEDYCENTER.CMD",FSpeedyHost.c_str(),&Data );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::StarWaveSubscriberMessage(const MString &Subject, const MString &Key,
		  MTree *Tree)
{
	int PanelIndex = FSWPanels->IndexOf( Key.c_str() );
	TSWFrame* UpdatePanel;

	if( PanelIndex == -1 )
	{
		UpdatePanel = new TSWFrame( SWScrollBox, Key.c_str() );
		FSWPanels->AddObject( Key.c_str(),UpdatePanel);
	}
	else
	{
		UpdatePanel = (TSWFrame*)FSWPanels->Objects[ PanelIndex ];
	}
	if( UpdatePanel != NULL )
		UpdatePanel->UpdateData( Tree );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ProxyUsersListViewData(TObject *Sender, TListItem *Item)
{
	TProxyInfo* ActiveProxy = (TProxyInfo*)ProxyTabSet->Tabs->Objects[ ProxyTabSet->TabIndex ];
	if( ActiveProxy != NULL )
	{
		UFC::List<TStringList*>* Users = ActiveProxy->GetUsers();
		TStringList* Update = Users->GetItem( Item->Index  );
		Item->Caption = Update->Strings[ 4 ];       ///< Account
		Item->SubItems->Add( Update->Strings[ 0 ] );///< Lock stat.  Index 0
		Item->SubItems->Add( Update->Strings[ 1 ] );///< LogonCount. Index 1
		Item->SubItems->Add( Update->Strings[ 2 ] );///< Logon stat. Index 2
		Item->SubItems->Add( Update->Strings[ 3 ] );///< API Version.Index 3
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ProxyTabSetChange(TObject *Sender, int NewTab, bool &AllowChange)
{
	AllowChange = true;
	TProxyInfo* ActiveProxy = (TProxyInfo*)ProxyTabSet->Tabs->Objects[ NewTab ];
	if( ActiveProxy != NULL )
		ActiveProxy->Active();
}
//---------------------------------------------------------------------------

void __fastcall TSimTFXForm::EmomeSubscriberMessage(const MString &Subject, const MString &Key,
          MTree *Tree)
{
	UFC::AnsiString SecStr;
	UFCType::UInt32 TickNow;
	String          MsgSecStr;
	int             Count;

	if( Tree->get( "seconds", SecStr ) && Tree->get( "count", Count ) )
	{
		TickNow = SecStr.ToInt();
		if( FEmomeOrdTick == 0 )
		{
			FEmomeOrdTick = TickNow;
			FEmomeOrdCount = Count;
		}
		else
		{
			int UseMS    = TickNow - FEmomeOrdTick;
			int OrdCount = Count - FEmomeOrdCount;
			double OrderSec = (double)OrdCount / ( (double)UseMS / 1000 );

			EmomePerformanceGauge->Add( OrderSec * 100 );
			MsgSecStr.printf( L"處理速度:%0.1f 筆/秒",OrderSec );
			EmomeSpeedLabel->Caption = MsgSecStr;
			if( OrderSec > FEmomeThroughputMax )
			{
				FEmomeThroughputMax = OrderSec;
				MsgSecStr.printf( L"最大流量:%0.1f 筆/秒", FEmomeThroughputMax );
				EmomeMaxLabel->Caption = MsgSecStr;
			}
			FEmomeOrdTick = TickNow;
			FEmomeOrdCount = Count;
		}
		MsgSecStr.printf( L"總比數:%d 筆", Count );
		EmomeCountLabel->Caption = MsgSecStr;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::EODItemClick(TObject *Sender)
{
	if( LineListBox->ItemIndex != -1 )
	{
		TLineInfo* Line = (TLineInfo*)LineListBox->Items->Objects[ LineListBox->ItemIndex ];
		String Msg;

		Msg.printf( L"%s %s ?", Scstrings_MAIN_FIX_ASK_END_OF_DAY, String(Line->GetPVCID().c_str()) );
		if( MessageDlg( Msg, mtConfirmation, TMsgDlgButtons() << mbOK<<mbCancel, 0) == mrOk )
		{
			MTree Data;

			Data.append( "COMMAND", AGENT_SET_SEQUENCE );
			Data.append( "SYS",     (int)Line->GetSystem() );
			Data.append( "LINK",    Line->GetLink() );
			Data.append( "PVC",     Line->GetPVC() );
			Data.append( "PVCID",   Line->GetPVCID().c_str() );
			Data.append( "BROKERID",Line->GetBrokerID().c_str() );
			AgentPublisher->SendData( &Data );
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TSimTFXForm::FIXSeqItemClick(TObject *Sender)
{
	if( LineListBox->ItemIndex != -1 )
	{
		TLineInfo* Line = (TLineInfo*)LineListBox->Items->Objects[ LineListBox->ItemIndex ];

		SequenceForm = new TSequenceForm( this, Line->GetInboundSeq(), Line->GetOutboundSeq() );
		if( SequenceForm->ShowModal() == mrOk)
		{
			MTree      Data;

			Data.append( "COMMAND",  AGENT_SET_SEQUENCE );
			Data.append( "SYS",      (int)Line->GetSystem() );
			Data.append( "LINK",     Line->GetLink() );
			Data.append( "PVC",      Line->GetPVC() );
			Data.append( "PVCID",    Line->GetPVCID().c_str() );
			Data.append( "BROKERID", Line->GetBrokerID().c_str() );
			Data.append( "OUTBOUND", SequenceForm->GetOutboundSeq() );
			Data.append( "INBOUND",  SequenceForm->GetInboundSeq());
			AgentPublisher->SendData( &Data );
		}
		delete SequenceForm;
	}
}
//---------------------------------------------------------------------------

void __fastcall TSimTFXForm::OTCRecjectButtonClick(TObject *Sender)
{
	String Msg;

	Msg.printf( Scstrings_MAIN_REJACT_ALL_ORDER_IN_Q.c_str(), L"OTC" );
	if( MessageDlg( Msg, mtConfirmation, TMsgDlgButtons() << mbOK<<mbCancel, 0) == mrOk )
	{
		MTree Data;

		Data.append( "COMMAND", AGENT_REJECT_ALL_ORD );
		Data.append( "SYS",     (int)mtOTC );
		AgentPublisher->SendData( &Data );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::TSERejectButtonClick(TObject *Sender)
{
	String Msg;

	Msg.printf( Scstrings_MAIN_REJACT_ALL_ORDER_IN_Q.c_str(), L"TWSE" );
	if( MessageDlg( Msg, mtConfirmation, TMsgDlgButtons() << mbOK<<mbCancel, 0) == mrOk )
	{
		MTree Data;

		Data.append( "COMMAND", AGENT_REJECT_ALL_ORD );
		Data.append( "SYS",     (int)mtStock );
		AgentPublisher->SendData( &Data );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::TSENormalTextClick(TObject *Sender)
{
	TStaticText* TradeSession = dynamic_cast<TStaticText*>(Sender);
	if( TradeSession != NULL )
	{
		UFC::AnsiString APCode;
		String Market,Session;
		String CurrStatus = TradeSession->Caption;

		ChangeSessionStatusForm = new TChangeSessionStatusForm( this );
		int Sel = ChangeSessionStatusForm->StatusGroupBox->Items->IndexOf( CurrStatus );
		if( Sel != -1 )
		{
			ChangeSessionStatusForm->StatusGroupBox->ItemIndex = Sel;
			if( TradeSession == TSENormalText || TradeSession == TSEOddText || TradeSession == TSEPostText)
				Market = Scstrings_MAIN_TSEC;
			else
				Market = Scstrings_MAIN_OTC;
			if( TradeSession == TSENormalText || TradeSession == OTCNormalText)
			{
				Session = Scstrings_MAIN_NORMAL;
				APCode  = "0";
			}
			else if( TradeSession == TSEOddText || TradeSession == OTCOddText)
			{
				Session = Scstrings_MAIN_ODD;
				APCode  = "4";
			}
			else if( TradeSession == TSEPostText || TradeSession == OTCPostText )
			{
				Session = Scstrings_MAIN_POST;
				APCode  = "9";
			}
			ChangeSessionStatusForm->Caption = Market + L"[" + Session + L"]";
			if( ChangeSessionStatusForm->ShowModal() == mrOk )
			{
				int index = ChangeSessionStatusForm->StatusGroupBox->ItemIndex;
				String NewStatus = ChangeSessionStatusForm->StatusGroupBox->Items->Strings[ index ];

				if( index != Sel )
				{
					String Msg;

					Msg.printf( L"是否要切換%s%%s盤別狀態成%s?", Market, Session, NewStatus );
					if( MessageDlg( Msg, mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0 ) == mrOk )
					{
						MTree Data;

						Data.append( "COMMAND", AGENT_SESSION_STATUS );
						if( Market == Scstrings_MAIN_TSEC )
							Data.append( "SYS",     (int)mtStock );
						else
							Data.append( "SYS",     (int)mtOTC );
						Data.append( "APCODE", APCode );
						Data.append( "STATUS", index ); ///< 0:wait 1:try 2:Opened 3:Closed
						AgentPublisher->SendData( &Data );
					}
				}
			}
		}
		delete ChangeSessionStatusForm;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ProcessListViewColumnClick(TObject *Sender, TListColumn *Column)
{
	if( Column->Index == 4 )///< Click Setting column
	{
		if( ProcessSetting == true )
		{
			MTree Data; ///< Request for the config file.

			Data.append( "COMMAND", SPEEDY_GET_CONFIG_FILE );
			Data.append( "FILENAME", "Speedy.cfg" );
			Data.append( "HOST", FSpeedyHost.c_str());
			Data.append( "ZIP", 1 );
			SpeedyAgentPublisher->SendData( &Data );
		}
		else
			this->StatusBar->Panels->Items[0]->Text = Scstrings_MAIN_PROCESS_NO_RIGHTS_CHANGE_SETTING;//"無權限設定程式";
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::AccountsListViewCompare(TObject *Sender, TListItem *Item1,
		  TListItem *Item2, int Data, int &Compare)
{
	Compare = FLoginIDSortDir*Item1->Caption.Compare( Item2->Caption );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::AccountsListViewColumnClick(TObject *Sender, TListColumn *Column)
{
   if( Column->Index == 0 )
   {
	   FLoginIDSortDir *= -1;
	   AccountsListView->AlphaSort();
   }
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::RuleTabSetChange(TObject *Sender, int NewTab, bool &AllowChange)
{
	 AllowChange = true;
	 LoadRule( NewTab );
}
//---------------------------------------------------------------------------
const char* MarketName( const char* Mkt )
{
	AnsiString MK( Mkt );
	AnsiString MKUp = MK.UpperCase();

	if( MKUp == "FUT" )
		return "期貨";
	else if( MKUp == "OPT" )
		return "選擇權";
	else if( MKUp == "STK" )
		return "證券";
	return Mkt;
}
//---------------------------------------------------------------------------
const char* MarketSymbol( const String& MName )
{
	if( MName == L"期貨" )
		return "FUT";
	else if( MName == L"選擇權" )
		return "OPT";
	else if( MName == L"證券" )
		return "STK";
	return "---";
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::LoadRule( int index )
{
	char Market[3][5] = {{"FUT"},{"OPT"},{"STK"}};
	AnsiString       Name;
	UFC::AnsiString  UName,Value;
	UFC::UiniFile&   ini = FRulesIni[index];
	UFC::Section*    ExpSection = ini.GetSection("Exception");

	RuleListView->Clear();
	for( int i = 0; i < FRuleExchangeList->Count; i ++ )
	{
		AnsiString Ex = FRuleExchangeList->Strings[ i ];
		for( int j = 0; j < 3; j++ )
		{
			TListItem* NewItem = RuleListView->Items->Add();

			NewItem->SubItems->Add( Ex );
			NewItem->SubItems->Add( MarketName( Market[j]) );
			Name.printf( "%s.%s",Ex,Market[j] );
			if( ini.GetValue( "Exchange", Name.c_str(), Value) == true )
				NewItem->SubItems->Add( Value.c_str());
			else
				NewItem->SubItems->Add( "---" );
		}
	}
	RuleExListView->Clear();
	if( ExpSection != NULL)
	{
		for( int i = 0; i < ExpSection->ItemCount(); i++ )
		{
			UFC::PStringList Strs;

			ExpSection->GetNameValue( i, UName, Value );
			Strs.SetStrings( UName, ".\n" );
			if( Strs.ItemCount() >= 3 &&
				FRuleExchangeList->IndexOf( Strs[0].c_str()) != -1 &&
				FRuleChannelList->IndexOf( Value.c_str()) != -1 )
			{
				TListItem* NewItem = RuleExListView->Items->Add();
				NewItem->SubItems->Add( Strs[0].c_str() );
				NewItem->SubItems->Add( MarketName( Strs[1].c_str()) );
				NewItem->SubItems->Add( Strs[2].c_str() );
				NewItem->SubItems->Add( Value.c_str() );
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::RuleListViewClick(TObject *Sender)
{
	TListItem* Item;
	if( (Item = RuleListView->Selected) != NULL )
	{
		ChannelForm->Init( Item->SubItems->Strings[0],Item->SubItems->Strings[1],Item->SubItems->Strings[2],FRuleChannelList);
		if( ChannelForm->ShowModal()== mrOk  )
		{
			if( Item->SubItems->Strings[2] != ChannelForm->ChComboBox->Text )
			{
				Item->SubItems->Strings[2] = ChannelForm->ChComboBox->Text;
				int              Index = RuleTabSet->TabIndex;
				UFC::UiniFile&   ini = FRulesIni[Index];
				UFC::Section*    ExSec = ini.GetSection( "Exchange" );
				if( ExSec != NULL )
				{
					AnsiString       ConfigFileName = CurrentDir + "\\" + FRuleFile[Index];
					AnsiString       Exchange( Item->SubItems->Strings[0] );
					AnsiString       Market( MarketSymbol( Item->SubItems->Strings[1] ) );
					AnsiString       Channel( Item->SubItems->Strings[2] );
					UFC::AnsiString  Key;

					Key.Printf( "%s.%s", Exchange.c_str(),Market.c_str() );
					if( Channel == "---" )
						ExSec->DeleteValue( Key );
					else
						ExSec->SetValue( Key, Channel.c_str() );
					ini.Save();
					SendFile( ConfigFileName, "FILE", SPEEDY_SET_CONFIG_FILE, FRemoteRuleFile[Index].c_str() );
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FExButtonClick(TObject *Sender)
{
	if( ItemsForm->ShowDialog( L"設定交易所", FRuleExchangeList ) == mrOk )
	{
		UFC::AnsiString  Name;
		AnsiString		 Value;
		AnsiString       ConfigFileName = CurrentDir + "\\" + FExSettingFile;

		FRuleExchangeIni.Clear();
		for( int i = 0; i < FRuleExchangeList->Count; i++ )
		{
			Value = FRuleExchangeList->Strings[ i ];
			Name.Printf( "ex%d", i + 1 );
			FRuleExchangeIni.SetValue( "Exchange", Name, Value.c_str() );
		}
		FRuleExchangeIni.Save();
		SendFile( ConfigFileName, "FILE", SPEEDY_SET_CONFIG_FILE, SPEEDY_ROUTE_EX_CFG );
		LoadRule( RuleTabSet->TabIndex );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::CounterPartyButtonClick(TObject *Sender)
{
	if( ItemsForm->ShowDialog( L"設定上手", FRuleChannelList ) == mrOk )
	{
		UFC::AnsiString  Name;
		AnsiString		 Value;
		AnsiString       ConfigFileName = CurrentDir + "\\" + FChSettingFile;

		FRuleChannelIni.Clear();
		for( int i = 0; i < FRuleChannelList->Count; i++ )
		{
			Value = FRuleChannelList->Strings[ i ];
			Name.Printf( "Ch%d", i + 1 );
			FRuleChannelIni.SetValue( "Setting", Name, Value.c_str() );
		}
		FRuleChannelIni.Save();
		SendFile( ConfigFileName, "FILE", SPEEDY_SET_CONFIG_FILE, SPEEDY_ROUTE_CH_CFG );
		LoadRule( RuleTabSet->TabIndex );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ExAccButtonClick(TObject *Sender)
{
	ExceptionAccForm->Init( &FRuleAccountIni );
	if( ExceptionAccForm->ShowModal() == mrOk )
	{
		AnsiString       ConfigFileName = CurrentDir + "\\" + FAccSettingFile;

		ExceptionAccForm->GetItems( &FRuleAccountIni );
		FRuleAccountIni.Save();
		SendFile( ConfigFileName, "FILE", SPEEDY_SET_CONFIG_FILE, SPEEDY_ROUTE_ACC_CFG );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::RuleComboBoxChange(TObject *Sender)
{
	UFC::AnsiString  Value;
	AnsiString       ConfigFileName = CurrentDir + "\\" + FAccSettingFile;

	Value.Printf( "Rule%d", RuleComboBox->ItemIndex + 1 );

	FRuleAccountIni.SetValue( "Account", "Default", Value );
	FRuleAccountIni.Save();
	SendFile( ConfigFileName, "FILE", SPEEDY_SET_CONFIG_FILE, SPEEDY_ROUTE_ACC_CFG );
}
//---------------------------------------------------------------------------
bool __fastcall TSimTFXForm::CheckExceptionProdID( void )
{
	if( ExceptionProdForm->ProdIDEdit->Text.Length() <= 0 )
	{
		MessageDlg( L"商品不能為空白", mtWarning, TMsgDlgButtons() << mbOK , 0 );
		return false;
	}
	TListItemsEnumerator* Enumerator = RuleExListView->Items->GetEnumerator();

	while( Enumerator->MoveNext() )
	{
		TListItem* SelItem = Enumerator->Current;
		if( SelItem != NULL)
		{
			if( SelItem->SubItems->Strings[0] == ExceptionProdForm->ExComboBox->Text &&
				SelItem->SubItems->Strings[1] == ExceptionProdForm->MarketComboBox->Text &&
				SelItem->SubItems->Strings[2] == ExceptionProdForm->ProdIDEdit->Text )
			{
				String Msg;
				Msg.printf( L"[%s][%s][%s]已經存在",ExceptionProdForm->ExComboBox->Text,ExceptionProdForm->MarketComboBox->Text,ExceptionProdForm->ProdIDEdit->Text );
				MessageDlg( Msg, mtWarning, TMsgDlgButtons() << mbOK , 0 );
				return false;
			}
		}
	}
	return true;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::UpdateExceptionRule( void )
{
	int                   Index = RuleTabSet->TabIndex;
	UFC::UiniFile&        ini   = FRulesIni[Index];
	AnsiString            ConfigFileName = CurrentDir + "\\" + FRuleFile[Index];
	TListItemsEnumerator* Enumerator = RuleExListView->Items->GetEnumerator();

	ini.DeleteSection( "Exception" );
	while( Enumerator->MoveNext() )
	{
		TListItem* SelItem = Enumerator->Current;
		if( SelItem != NULL)
		{
			AnsiString       Exchange( SelItem->SubItems->Strings[0] );
			AnsiString       Market( MarketSymbol( SelItem->SubItems->Strings[1] ) );
			AnsiString       ProdID( SelItem->SubItems->Strings[2] );
			AnsiString       Channel( SelItem->SubItems->Strings[3] );
			UFC::AnsiString  Name;

			Name.Printf( "%s.%s.%s", Exchange.c_str(),Market.c_str(),ProdID.c_str()  );
			ini.SetValue( "Exception",Name,Channel.c_str() );
		}
	}
	ini.Save();
	SendFile( ConfigFileName, "FILE", SPEEDY_SET_CONFIG_FILE, FRemoteRuleFile[Index].c_str() );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::NewExRuleButtonClick(TObject *Sender)
{
	ExceptionProdForm->Init( FRuleExchangeList, FRuleChannelList );
	if( ExceptionProdForm->ShowModal() == mrOk )
	{
		if( CheckExceptionProdID()==true )
		{
			TListItem* NewItem = RuleExListView->Items->Add();
			NewItem->SubItems->Add( ExceptionProdForm->ExComboBox->Text );
			NewItem->SubItems->Add( ExceptionProdForm->MarketComboBox->Text );
			NewItem->SubItems->Add( ExceptionProdForm->ProdIDEdit->Text );
			NewItem->SubItems->Add( ExceptionProdForm->ChComboBox->Text );
			UpdateExceptionRule();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::DelExRuleButtonClick(TObject *Sender)
{

	TListItem* SelItem  = RuleExListView->Selected;

	if( SelItem != NULL )
	{
		String Msg;
		Msg.printf( L"確定要刪除[%s][%s][%s]?",	SelItem->SubItems->Strings[0],
												SelItem->SubItems->Strings[1],
												SelItem->SubItems->Strings[2]);
		if( MessageDlg( Msg, mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0 ) == mrOk )
		{
			RuleExListView->Items->Delete( SelItem->Index );
			UpdateExceptionRule();
		}
	}
	else
		MessageDlg( L"請選擇欲刪除的項目", mtWarning, TMsgDlgButtons() << mbOK , 0 );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ModExRuleButtonClick(TObject *Sender)
{
	TListItem* ModifyItem  = RuleExListView->Selected;

	if( ModifyItem != NULL )
	{
		ExceptionProdForm->Init( FRuleExchangeList, FRuleChannelList, ModifyItem->SubItems );
		if( ExceptionProdForm->ShowModal() == mrOk )
		{
			String OldCh = ModifyItem->SubItems->Strings[3];
			String NewCh = ExceptionProdForm->ChComboBox->Text;

			if( OldCh != NewCh )
			{
				if( ExceptionProdForm->ApplyAllChCheckBox->Checked == true )
				{
					TListItemsEnumerator* Enumerator = RuleExListView->Items->GetEnumerator();
					while( Enumerator->MoveNext() )
					{
						TListItem* SelItem = Enumerator->Current;
						if( SelItem != NULL)
						{
							if( SelItem->SubItems->Strings[3] == OldCh )
								SelItem->SubItems->Strings[3] = NewCh;
						}
					}
				}
				else
					ModifyItem->SubItems->Strings[3] = NewCh;
				UpdateExceptionRule();
			}
		}
	}
	else
		MessageDlg( L"請選擇欲修改的項目", mtWarning, TMsgDlgButtons() << mbOK , 0 );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::MessageButtonClick(TObject *Sender)
{
	TListItemsEnumerator* Enumerator = AccountsListView->Items->GetEnumerator();

	MessageForm->UserComboBox->Items->Clear();
	MessageForm->UserComboBox->Items->Add( L"所有用戶" );
	while( Enumerator->MoveNext() )
	{
		TListItem* SelItem = Enumerator->Current;
		if( SelItem != NULL )
			MessageForm->UserComboBox->Items->Add( SelItem->Caption );
	}
	if( AccountsListView->Selected == NULL )
		MessageForm->UserComboBox->ItemIndex = 0;
	else
		MessageForm->UserComboBox->ItemIndex = MessageForm->UserComboBox->Items->IndexOf( AccountsListView->Selected->Caption );
	if( MessageForm->ShowModal() == mrOk )
	{
		UTF8String Header( MessageForm->HeaderEdit->Text );
		UTF8String Message( MessageForm->TextEdit->Text );

		for( int i = 0; i < MessageForm->Users->Count; i++ )
		{
			AnsiString AE( MessageForm->Users->Strings[i] );
			MTree Data;

			Data.append( "UID", 0 );
			Data.append( "USER", AE.c_str() );
			Data.append( "FUNC", Header.c_str() );
			Data.append( "DATA", Message.c_str() );
			MessageObject->Send( "NEWS.RESPONSE",FSpeedyHost.c_str(), &Data );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::ThroughputButtonClick(TObject *Sender)
{
	MTree      Data;
	int        index =	ProxyTabSet->TabIndex;
	AnsiString ProxyName = ProxyTabSet->Tabs->Strings[ index ];

	Data.append( "CMD", CMD_PROXY_THROUGHPUT_REQ );
	MessageObject->Send( "SPEEDYCENTER.CMD",ProxyName.c_str(),&Data );
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::B30SysComboBoxChange(TObject *Sender)
{
	if( B30SysComboBox->ItemIndex == 0 )
	{
		PartIDComboBox->Items->Clear();
		PartIDComboBox->Items->Add( "0" );
		PartIDComboBox->Items->Add( "202" );
	}
	else
	{
		PartIDComboBox->Items->Clear();
		PartIDComboBox->Items->Add( "0" );
		PartIDComboBox->Items->Add( "102" );
	}
	PartIDComboBox->ItemIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::FLEXSysComboBoxChange(TObject *Sender)
{
	if( FLEXSysComboBox->ItemIndex == 0 )
		OPTPanel->Visible = false;
	else
		OPTPanel->Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TSimTFXForm::NewFLEXButtonClick(TObject *Sender)
{
	int AccountInt,StrikePxInt;
	if( RootSymbolEdit->Text.Length() != 3 )
	{
		MessageDlg( L"請輸入三碼的契約根代碼", mtWarning, TMsgDlgButtons() << mbOK , 0 );
		return;
	}
	if( BIDEdit->Text.Length() != 7 )
	{
		MessageDlg( L"請輸入七碼的期貨商代號", mtWarning, TMsgDlgButtons() << mbOK , 0 );
		return;
	}
	if( AccEdit->Text.Length() != 7 )
	{
		MessageDlg( L"請輸入七碼的交易人帳號", mtWarning, TMsgDlgButtons() << mbOK , 0 );
		return;
	}
	try
	{
		AccountInt = AccEdit->Text.ToInt();
	}
	catch(...)
	{
		MessageDlg( L"請輸入七碼數字的交易人帳號", mtWarning, TMsgDlgButtons() << mbOK , 0 );
		return;
	}
	if( ContractDateEdit->Text.Length() != 8 )
	{
		MessageDlg( L"請輸入八碼的契約日期(YYYYMMDD)", mtWarning, TMsgDlgButtons() << mbOK , 0 );
		return;
	}
	if( FLEXSysComboBox->ItemIndex == 1 )
	{
		try
		{
			StrikePxInt = StrikePxEdit->Text.ToInt();
		}
		catch(...)
		{
			MessageDlg( L"請輸入商品履約價", mtWarning, TMsgDlgButtons() << mbOK , 0 );
			return;
		}
	}
	AnsiString RootSym = RootSymbolEdit->Text;
	AnsiString BrokerID = BIDEdit->Text;
	AnsiString ContractDate = ContractDateEdit->Text;
	MTree Data;

	if( FLEXTypeComboBox->ItemIndex == 0 )
		Data.append( "SecurityRequestType", "1" );
	else
		Data.append( "SecurityRequestType", "3" );
	Data.append( "SecurityReqID", (UFCType::Int32)GetTickCount() );
	if(	FLEXSysComboBox->ItemIndex == 0 )
		Data.append( "type", "F" );
	else
		Data.append( "type", "O" );
	Data.append( "root_symbol", RootSym.c_str() );
	Data.append( "BrokerID", BrokerID.c_str() );
	Data.append( "investor_acno", AccountInt );
	if(	AppTypeComboBox->ItemIndex == 0 )
		Data.append( "application_type", "1" );
	else if( AppTypeComboBox->ItemIndex == 1 )
		Data.append( "application_type", "2" );
	else if( AppTypeComboBox->ItemIndex == 2 )
		Data.append( "application_type", "3" );
	Data.append( "contract_date", ContractDate.c_str() );

	FLEXStatusLabel->Caption = "送出請求...";
	NewFLEXButton->Enabled = false;
	for( int i = 0; i < 20; i++ )
	{
		Application->ProcessMessages();
		UFC::SleepMS( 20 );
	}
	if(	FLEXSysComboBox->ItemIndex == 1 )
	{
		Data.append( "strike_price", StrikePxInt );
		if( CallPutComboBox->ItemIndex == 0 )
			Data.append( "call_put_code", "C" );
		else
			Data.append( "call_put_code", "P" );
		MessageObject->Send( "FLEX.REQUEST.OPT",FSpeedyHost.c_str(), &Data );
	}
	else
		MessageObject->Send( "FLEX.REQUEST.FUT",FSpeedyHost.c_str(), &Data );
	NewFLEXButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSimTFXForm::SpeedyFLEXOPTSubscriberMessage(const MString &Subject,
          const MString &Key, MTree *Tree)
{
	MString Type;
	int     StatusCode;

	if( Tree->get( "SecurityRequestType", Type ) && Tree->get( "status_code", StatusCode ) )
	{
		String ReqType,Msg;

		if( Type == "1" )
			ReqType = L"新增結果:";
		else
			ReqType = L"查詢結果:";
		switch( StatusCode )
		{
			case 0: Msg = L"OK";break;
			case 1: Msg = L"[1]非客製化契約申請時間.";break;
			case 12: Msg = L"[12]期貨商代號錯誤.";break;
			case 20: Msg = L"[20]商品代號錯誤.";break;
			case 82: Msg = L"沒有FLEX線路.";break;
			case 99: Msg = L"Speedy沒開啟FLEX支援.";break;
			case 300: Msg = L"[300]客製化契約已重複申請.";break;
			case 301: Msg = L"[301]客製化契約因本日 OI 尚未確定，請待 OI 確認後再提出申請.";break;
			case 302: Msg = L"[3002客製化契約已掛牌交易.";break;
			case 303: Msg = L"[303]客製化契約到期日錯誤.";break;
			case 304: Msg = L"[304]客製化契約申請識別碼重複.";break;
			case 306: Msg = L"[306]查無申請成功紀錄.";break;
			case 310: Msg = L"[310]非客製化契約申請時間.";break;
			case 311: Msg = L"[311]此自營商交易帳號尚未核准申請客製化契約.";break;
			case 312: Msg = L"[312]此期貨商設定帳號尚未核准申請客製化契約.";break;
			case 313: Msg = L"[313]客製化契約涵蓋幅度錯誤.";break;
			case 314: Msg = L"[314]客製化契約履約價格錯誤.";break;
			case 315: Msg = L"[315]客製化契約執行類型錯誤,檢查RX19訊息中執行類型欄位並更正.";break;
			case 316: Msg = L"[316]客製化契約申請型別錯誤,檢查RX19訊息中申請型別欄位並更正.";break;
			case 317: Msg = L"[317]契約版號錯誤 檢查並更正契約版號.";break;
			case 318: Msg = L"[318]契約型別錯誤 檢查並更正契約型別.";break;
			case 319: Msg = L"[319]買賣權別錯.";break;
			default: Msg.printf( L"交易所錯誤碼[%d]", StatusCode );
		}
		FLEXStatusLabel->Caption = ReqType + Msg;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::APIStateButtonClick(TObject *Sender)
{
	MTree Data;

	APILockedListView->Clear();
	Data.append("text", "query_locked_account" );
	APIStatePublisher->Key = "query_locked_account";
	APIStatePublisher->SendData( &Data );
	APILockedListView->Items->BeginUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TSimTFXForm::QueryAPISetting( void )
{
	MTree Data;

	Data.append("text", "query_order_threshold" );
	APIStatePublisher->Key = "query_order_threshold";
	APIStatePublisher->SendData( &Data );
}
//---------------------------------------------------------------------------

void __fastcall TSimTFXForm::APIStateSubscriberMessage(const MString &Subject, const MString &Key,
		  MTree *Tree)
{
	UFC::AnsiString BrokerID,Account,Reason,Time;
	UFC::Int32 Total, Cur;
	if( Tree->get( "broker_id", BrokerID ) == true &&
		Tree->get( "account", Account ) == true &&
		Tree->get( "reason", Reason ) == true &&
		Tree->get( "timestamp", Time ) == true &&
		Tree->get( "total", Total ) == true &&
		Tree->get( "n_th", Cur ) == true )
	{
		TListItem*       NewListItem;
		UTF8String UniReason( Reason.c_str() );

		if( BrokerID.Length() >= 4 && Account.Length() >= 5 )
		{
			NewListItem = APILockedListView->Items->Add();
			NewListItem->Caption = Account.c_str();
			NewListItem->SubItems->Add( BrokerID.c_str());
			NewListItem->SubItems->Add( Time.c_str());
			NewListItem->SubItems->Add( UniReason );
			NewListItem->ImageIndex = 3;
		}
		if( Total == Cur )
			APILockedListView->Items->EndUpdate();
	}

}
//---------------------------------------------------------------------------


void __fastcall TSimTFXForm::APISettingSubscriberMessage(const MString &Subject, const MString &Key,
          MTree *Tree)
{
	UFC::AnsiString Editor,Time;
	UFC::Int32 DayLimit, Duration, Limit;

	if( Tree->get( "day_threshold", DayLimit ) == true &&
		Tree->get( "time_duration", Duration ) == true &&
		Tree->get( "duration_threshold", Limit ) == true &&
		Tree->get( "editor", Editor ) == true &&
		Tree->get( "last_modified", Time ) == true )
	{
		String ThroughputCtrl;

		ThroughputCtrl.printf( L"每%d秒最多下%d筆  ", Duration, Limit );
		DayLimitText->Caption = String( DayLimit );
		ModifyTimeText->Caption = Time.c_str();
		EditorText->Caption = Editor.c_str();
		ThroughputCtrlLabel->Caption = ThroughputCtrl;

		APISettingForm->DayLimitEdit->Value = DayLimit;
		APISettingForm->SecSpinEdit->Value = Duration;
		APISettingForm->OrdLimitEdit->Value = Limit;

	}
}
//---------------------------------------------------------------------------

void __fastcall TSimTFXForm::APISettingButtonClick(TObject *Sender)
{
	String Title;

	Title.printf( L"API 編輯者:%s", FUser );
	APISettingForm->Caption = Title;
	if( APISettingForm->ShowModal() == mrOk )
	{
		AnsiString ID( FUser );
		MTree Data;

		Data.append("day_threshold", (UFCType::Int32)APISettingForm->DayLimitEdit->Value );
		Data.append("time_duration", (UFCType::Int32)APISettingForm->SecSpinEdit->Value );
		Data.append("duration_threshold", (UFCType::Int32)APISettingForm->OrdLimitEdit->Value );
		Data.append("editor", ID.c_str() );
		APIStatePublisher->Key = "modify_order_threshold";
		APIStatePublisher->SendData( &Data );
		for( int i = 0; i < 10; i++ )
		{
			UFC::SleepMS( 10 );
			Application->ProcessMessages();
		}
		QueryAPISetting();
	}

}
//---------------------------------------------------------------------------

void __fastcall TSimTFXForm::MenuItem1Click(TObject *Sender)
{
	TListItem* Item;

	for( register int j= 0; j < APILockedListView->Items->Count; j++ )
	{
		Item = APILockedListView->Items->Item[j];
		if( Item->Checked == true )
		{
			AnsiString Acc( Item->Caption );
			AnsiString Broker(Item->SubItems->Strings[0]);
			MTree Data;

			Data.append("broker_id", Broker.c_str() );
			Data.append("account", Acc.c_str() );
			APIStatePublisher->Key = "remove_locked_account";
			APIStatePublisher->SendData( &Data );
			UFC::SleepMS( 20 );
			Application->ProcessMessages();
		}
	}
	APIStateButtonClick( NULL );
}
//---------------------------------------------------------------------------

