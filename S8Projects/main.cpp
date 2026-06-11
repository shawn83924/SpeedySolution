//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "main.h"
#include "DepthTrade.h"
#include "Login.h"
#include "ContractViewer.h"
#include "ContractForm.h"
#include "ContractList.h"
#include "TBarForm.h"
#include "ChartForm.h"
#include "ExecutionViewerForm.h"
#include "TradingQueries.h"
#include "TradingObjectPool.h"
#include "TradingObjects.h"
#include "TradingCommodities.h"
#include "TradingObjectPool.h"
#include "HoldOpenInterest.h"
#include "FMTConfig.h"
#include <windowsx.h>
#include "dsaudio.h"
#include "UnifyDlg.h"
#include "MD5.h"
#include "CASetting.h"
#include "LifeUpdateForm.h"
#include "AskForUpdateForm.h"
#include "UnifyUtility.h"
#include "AggrementUnit.h"
#include "SearchUnit.h"
#include "TrainingDlg.h"
#include <System.NetEncoding.hpp>
#include <System.Win.Registry.hpp>
#include <System.IOUtils.hpp>
#include "LeaderBoard.h"
#include "WebBrowserForm.h"
#include "SelAccount.h"
#include "AskRoomi.h"
#include "AskCloseAll.h"
#include "CancelWorkingTimeForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "MarketDataStore"
#pragma link "MarketDataList"
#pragma link "ChartsStore"
#pragma link "OptionsStrikePriceView"
#pragma link "OrderStore"
#pragma link "OrderBookList"
#pragma link "RoundFormEx"
#pragma link "GraphButton"
#pragma link "RoundFormExRes"
#pragma link "GraphPanel"
#pragma link "SHDocVw_OCX"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
TMainForm *MainForm;
//---------------------------------------------------------------------------
TCMarketDataStore* gMarketDataStore = NULL;
TOrderStore*       gOrderStore = NULL;
DSAudio            gDSAudio;
TUnifyUser         gUser;
extern bool        GSimMatch;
extern bool        GIsTestingMode;
extern HINSTANCE   GHinstance;
const int CAPTION_H = 50;
const int BOTTOM_H  = 32;
//---------------------------------------------------------------------------
bool               gCheckCA   = true;  ///< Needs CA
bool               gNuclear   = true;  ///< AOE Nuclear class
bool               gTFT       = false; ///< Enable TFT functions. ( Turn on 創富 tab )
//---------------------------------------------------------------------------
bool               GVIPServer = false; ///< Use AOE production server.
//---------------------------------------------------------------------------
// For Debug
// Set GCheckLicense = false  to bypass license cjecking.
//---------------------------------------------------------------------------
bool GCheckLicense = true;
//---------------------------------------------------------------------------
UnifyProductKey UnifyKey( L"20200416141620158894", L"3cecd9c0010000060b3d" ); ///< Unify AP閃電下單版
UnifyProductKey ProdUnifyKey( L"20200505150934193405", L"3d4ed23401000002f060" ); ///< Unify AP閃電下單版
UnifyProductKey AOEPackKey( L"20200511170257752518", L"3d6e20efd8000006fb7c" ); ///< 啟蒙包
UnifyProductKey Nuclear1Key(L"20200430135113794927",L"3d34cd63150000028726");//核爆班第一現場
UnifyProductKey Nuclear2Key(L"20200430135113794927",L"3d34d31527000002fb6c");//核爆班第二現場
UnifyProductKey Nuclear3Key(L"20200430135113794927",L"3d34d732bb0000067ab5");//核爆數位班
//---------------------------------------------------------------------------
// For Debug
// Set GUseFixIP = true force Unify connect to the given IP address.
//---------------------------------------------------------------------------
//#define __TO_TEST
//#define __FIX_IP
//---------------------------------------------------------------------------
#ifdef __FIX_IP
	bool  GUseFixIP                = true;
#else
	bool  GUseFixIP                = false;
#endif
//---------------------------------------------------------------------------
// For Testing, use fixed IP
// GUseFixIP  = true;
//---------------------------------------------------------------------------
const String MD_SERVER_IP      = L"61.220.47.67"; //L"60.250.82.135"; //L"203.75.198.98";//
const int    MD_SERVER_PORT    = 34567;
const String CHART_SERVER_IP   = L"10.1.8.82";//L"203.75.198.98";//L"61.220.47.67"; //L"60.250.82.135";
const int    CHART_SERVER_PORT = 34569;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
: TForm(Owner)
,FSelectedFuncText( NULL )
,FVoice( NULL)
,FRecovering( false )
,FLoginBroker( NULL )
,FSpeedyCfg( NULL )
,FUILoaded( false )
,FForceClose( false )
,FRequestWeb( false )
,FNetBalance( 0 )
,FLoginUserStr( L"交易帳號:尚未登入" )
,FCopyRights( L"Copyright© 2020 MDBS, All Rights Reserved.  " )
,FFutAccIndex( 0 )
,FTseAccIndex( 0 )
,FLoadPositionOK( false )
{
	this->DefaultMonitor = dmDesktop;
	CMarketDataStore->SetLanguage( lTraditionalChinese );
	CMarketDataStore->LoadChineseName( );
	gMarketDataStore = CMarketDataStore;
	TExecution::FMarketDataStorePtr = CMarketDataStore;

	s888::gQueryObjsPool.LoadExchangeInformation( L"ExchangeInfo.ini" );

	///< Create ChartsStore
	ChartsStore = new TChartsStore( this );
	ChartsStore->Parent = this;
	ChartsStore->Left = -100;
	ChartsStore->Top = -100;
	ChartsStore->OnAppConnected = ChartsStoreXAppConnected;
	ChartsStore->OnAppDisconnected = ChartsStoreXAppDisconnected;
	///< Create OrderStore
	OrderStore  = new TOrderStore( this );
	gOrderStore = OrderStore;
	OrderStore->MarketDataStore   = CMarketDataStore;
	OrderStore->OnDisconnect      = OrderStoreDisconnect;
	OrderStore->OnNewOrder        = OrderStoreNewOrder;
	OrderStore->OnStopOrderChange =	StopOrderChange;
	OrderStore->OnCancelByOrderID = OrderStoreCancelByOrderID;
	OrderStore->OnRejectedReply   = OrderStoreRejectedReply;
	OrderStore->OnNewOrderReply   = OrderStoreNewOrderReply;
	OrderStore->OnFilledReply     = OrderStoreFilledReply;
	OrderStore->OnBeginRecover    = OrderStoreBeginRecover;
	OrderStore->OnRecoverFinished = OrderStoreRecoverFinished;
	OrderStore->OnFloatingProfit  =	OrderStoreFloatingProfit;
	OrderStore->OrderLogPath      = g_Config.GetCurrentDir( ) + "log";
	OrderStore->MarketDataTimerInterval = 250;

	FCAChecker = new TCAChecker( this );
	FCAChecker->OnCACheckFail =	CACheckFail;

	FHotkeys.Add(SellText);
	FHotkeys.Add(BuyText);
	FHotkeys.Add(BuyMarketText);
	FHotkeys.Add(SellMarketText);
	FHotkeys.Add(IncBetterText);
	FHotkeys.Add(DecBetterText);
	FHotkeys.Add(CancelAllText);
	FHotkeys.Add(CancelAllStopText);
	FHotkeys.Add(CancelAllandCloseText);
	FHotkeys.Add(CenterPxStaticText);
	FHotkeys.Add(NuclearBuyText);
	FHotkeys.Add(NuclearSellText);

	FTabs.Add( Tab1Button );
	FTabs.Add( Tab2Button );
	FTabs.Add( Tab3Button );
	FTabs.Add( Tab4Button );
	FTabs.Add( Tab5Button );

	SelectContractForm = new TContractInfoForm( this, true );
	FMDRTTDataList     = new TList();
	FMDPolygonPoints   = new TPoint[ 512 ];
	FMDRTTDataList->Add( (void*)0 );

	HRESULT hresult = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&FVoice);
	if( ! SUCCEEDED(hresult) )
		FVoice = NULL;
	FBKBuffer = new Graphics::TBitmap();
	FStatusBuffer = new Graphics::TBitmap();
	FBKBuffer->Height = CAPTION_H;
	FBKBuffer->Width = ClientWidth;
	FStatusBuffer->Height = BOTTOM_H;
	FStatusBuffer->Width = ClientWidth;
	FUserImg = new Graphics::TPicture();

	UFC::AnsiString FileName;
	UFC::AnsiString Today;
	char CurrDir[256];

	g_Config.GetCurrentDir( CurrDir );
	UFC::GetTradeYYYYMMDD( Today );
	FileName.Printf( "%slog\\SpeedyUnify_AP_Log%s.txt", CurrDir,Today.c_str() );
	UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( FileName, 10240, true, true ) );
	PrintLogHeader( );

   	TrayIcon->Visible = false;
	if( GSimMatch == true )
	{
		TOrderStore::SetTestMode( true );
		TOrderStore::SetTestMatchPrice( true );
		Caption = Mdcomponentstrings_MD_SpeedyUnify_AppName+L" Sim";
	}
	else
		Caption = Mdcomponentstrings_MD_SpeedyUnify_AppName;
}
//---------------------------------------------------------------------------
__fastcall TMainForm::~TMainForm( void )
{
	delete CMarketDataStore;
	delete ChartsStore;
	delete OrderStore;

	delete FCAChecker;
	delete SelectContractForm;
	delete FMDRTTDataList;
	delete FMDPolygonPoints;
	delete FBKBuffer;
	delete FStatusBuffer;
	if( FUserImg != NULL)
		delete FUserImg;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OSNotSupport( const AnsiString& OSStr )
{
	 String ErrMsg;
	 String OSVer( OSStr.c_str() );

	 ErrMsg.printf( L"您的作業系統[%s]版本太舊,無法執行本程式!", OSVer );
	 TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, ErrMsg );
	 Application->Terminate();
}
//---------------------------------------------------------------------------
float __fastcall TMainForm::ProcSpeedCalc()
{
	wchar_t Buffer[_MAX_PATH];
	DWORD BufSize = _MAX_PATH;
	DWORD dwMHz = _MAX_PATH;
	HKEY hKey;

	// open the key where the proc speed is hidden:
	long lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
						L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
						0,
						KEY_READ,
						&hKey);

	if( lError != ERROR_SUCCESS )
		return 1.0;
	// query the key:
	RegQueryValueEx(hKey, L"~MHz", NULL, NULL, (LPBYTE) &dwMHz, &BufSize);
	return (double)dwMHz/1000.0;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::PrintLogHeader( void )
{
	OSVERSIONINFOEX osvi;
	MEMORYSTATUSEX  statex;
	SYSTEM_INFO     si;
	String          ErrMsg;
	LARGE_INTEGER   PinTime;
	double          GHz = ProcSpeedCalc();
	int             AppVer = g_Config.GetVersion();

	UFC::BufferedLog::Printf( "  _______________________________________________________________");
	UFC::BufferedLog::Printf( "                                                                 ");
	UFC::BufferedLog::Printf( (AnsiString("    ") + Mdcomponentstrings_MD_SpeedyUnify_AppName + " Version:%d.%d.%d.%d ").c_str(),AppVer/1000000,	(AppVer%1000000)/10000, (AppVer%10000)/100, AppVer%100);
	UFC::BufferedLog::Printf( "                                                                 ");
	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if( GetVersionEx((OSVERSIONINFO *)&osvi) != 0 )
	{
			AnsiString OSStr;

			if(	osvi.dwMajorVersion == 4 ) ///< 4.X Windows NT
			{
				OSStr = "Windows NT";
				OSNotSupport( OSStr );
			}
			else if( osvi.dwMajorVersion == 5 )  ///< 5.X Windows XP/2000/2003
			{
				switch( osvi.dwMinorVersion )
				{
					case 0: OSStr = "Windows 2000";break;
					case 1: OSStr = "Windows XP";break;
					case 2: if( GetSystemMetrics(SM_SERVERR2) != 0 )
								OSStr = "Windows Server 2003 R2";
							else
								OSStr = "Windows Server 2003";
							break;
				}
				OSNotSupport( OSStr );
			}
			else if( osvi.dwMajorVersion == 6 )  ///< 6.X  Vista/Windows 7/8/Windows Server 2008/2012
			{
				if( osvi.dwMinorVersion == 0 )
				switch( osvi.dwMinorVersion )
				{
					case 0:	if( osvi.wProductType == VER_NT_WORKSTATION )
								OSStr = L"Windows Vista";
							else
								OSStr = L"Windows Server 2008";
							break;
					case 1:	if( osvi.wProductType == VER_NT_WORKSTATION )
								OSStr = L"Windows 7";
							else
								OSStr = L"Windows Server 2008 R2";
							break;
					case 2: if( osvi.wProductType == VER_NT_WORKSTATION )
								OSStr = L"Windows 8";
							else
								OSStr = L"Windows Server 2012";
							break;
					case 3: if( osvi.wProductType == VER_NT_WORKSTATION )
								OSStr = L"Windows 8.1";
							else
								OSStr = L"Windows Server 2012 R2";
							break;
				}
			}
			else if( osvi.dwMajorVersion == 10 )  ///< 10.X Windows 10/Windows Server 2016
			{
				if( osvi.dwMinorVersion == 0 )
				switch( osvi.dwMinorVersion )
				{
					case 0:	if( osvi.wProductType == VER_NT_WORKSTATION )
								OSStr = L"Windows 10";
							else
								OSStr = L"Windows Server 2016";
							break;
				}
			}
			UFC::BufferedLog::Printf( "    Windows version %d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion );
			UFC::BufferedLog::Printf( "    %s (Service Pack:%d)", OSStr.c_str(), osvi.wServicePackMajor );
	}
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	int TotalMem = (int)((__int64)statex.ullTotalPhys/((__int64)1024*1024));
	int FreeMem  = (int)((__int64)statex.ullAvailPhys/((__int64)1024*1024));

	if( FreeMem < 2048 ) ///< less than 2G memory
	{
		 if( TotalMem < 4096*2 )
			 ErrMsg.printf( L"注意!您的記憶體只剩[%dMB]可能會影響交易速度!建議增加記憶體.", FreeMem );
		 else
			 ErrMsg.printf( L"注意!您的記憶體只剩[%dMB]可能會影響交易速度!建議關閉不必要程式.", FreeMem );
		 TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, ErrMsg );
	}
	if( GHz < 3.0 ) ///< less than 2.5 GHz memory
	{
		 ErrMsg.printf( L"注意!您的CPU只有[%0.2fGHz]會導致交易速度不佳!建議升級CPU.", GHz );
		 TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, ErrMsg );
	}
	UFC::BufferedLog::Printf( "                                                                 ");
	UFC::BufferedLog::Printf( "    主機記憶體: %d MB", TotalMem );
	UFC::BufferedLog::Printf( "    可用記憶體: %d MB", FreeMem );
	UFC::BufferedLog::Printf( "       CPU時脈: %0.2f GHz", GHz );
	UFC::BufferedLog::Printf( "  _______________________________________________________________");
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SetIE11( void )
{
	UnicodeString sKey = ExtractFileName(ParamStr(0));
	int cIE11 = 11001;

	UnicodeString cFeatureBrowserEmulation = L"Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION\\";
	TRegistry *reg =  new TRegistry( KEY_WRITE );
	reg->RootKey = HKEY_CURRENT_USER;

	try
	{
		if( reg->OpenKey(cFeatureBrowserEmulation, true ) )
			reg->WriteInteger( sKey, cIE11);
	}
	__finally
	{
		delete reg;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject *Sender)
{
	Scaled = true;
	gDSAudio.InitDevice( Handle );
	SetIE11( );
	Font->Size = 11;
	SettingPageControl->Font->Size = 10;
	SearchEditBox->Font->Size = 11;
	catMenuItems->Font->Size = 11;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::NewOrdSoundComboBoxChange(TObject *Sender)
{
	LoadWaves();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CACheckFail( TCAChecker* Sender, UnicodeString& Reason )
{
	CAButton->Enabled = true;
	CAButton->Invalidate();

	AnsiString ReasonStr( Reason );
	UFC::BufferedLog::Printf( " [TMainForm::CACheckFail] failed Reason[%s]", ReasonStr.c_str() );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LoadWaves( void )
{
	AnsiString NewOrdSound( FSoundDir + NewOrdSoundComboBox->Text );
	AnsiString OrdFillSound( FSoundDir + FillSoundComboBox->Text );
	AnsiString OrdRejectSound( FSoundDir + RejectSoundComboBox->Text );
	AnsiString StopOrdSound( FSoundDir + StopSoundComboBox->Text );
	AnsiString PxAlarmSound( FSoundDir + PxAlarmSoundComboBox->Text );
	AnsiString LowPxAlarmSound( FSoundDir + LowPxAlarmSoundComboBox->Text );

	gDSAudio.ClearAll();
	if( FileExists( NewOrdSound ) )
		gDSAudio.LoadBaseWave( "New", NewOrdSound.c_str() );
	if( FileExists( OrdFillSound ) )
		gDSAudio.LoadBaseWave( "Fill", OrdFillSound.c_str() );
	if( FileExists( OrdRejectSound ) )
		gDSAudio.LoadBaseWave( "Reject", OrdRejectSound.c_str() );
	if( FileExists( StopOrdSound ) )
		gDSAudio.LoadBaseWave( "Stop", StopOrdSound.c_str() );
	if( FileExists( PxAlarmSound ) )
		gDSAudio.LoadBaseWave( "PxAlarm", PxAlarmSound.c_str() );
	if( FileExists( LowPxAlarmSound ) )
		gDSAudio.LoadBaseWave( "LowPxAlarm", LowPxAlarmSound.c_str() );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LoadWavesSetting( void )
{
	String CurrentDir = g_Config.GetCurrentDir( );
	String SoundFile;
	int index;

	LoadingProgress( 5, L"讀取音效設定..." );
	Application->ProcessMessages();
	FSoundDir = CurrentDir + "\\Sound\\";
	if( DirectoryExists( FSoundDir ) == false )
		ForceDirectories( FSoundDir );
	TFileListBox *SoundListBox;
	SoundListBox = new TFileListBox( this );
	SoundListBox->BoundsRect = TRect( -10,-10,0,0);
	SoundListBox->Visible = false;
	SoundListBox->Parent = this;
	SoundListBox->Directory = FSoundDir;
	SoundListBox->Mask = "*.wav";//;*.mp3;*.ogg";
	///< Clear ComboBox
	FillSoundComboBox->Items->Clear();
	NewOrdSoundComboBox->Items->Clear();
	RejectSoundComboBox->Items->Clear();
	StopSoundComboBox->Items->Clear();
	PxAlarmSoundComboBox->Items->Clear();
	LowPxAlarmSoundComboBox->Items->Clear();
	///< Add sounds to ComboBox
	FillSoundComboBox->Items->AddStrings( SoundListBox->Items );
	NewOrdSoundComboBox->Items->AddStrings( SoundListBox->Items );
	RejectSoundComboBox->Items->AddStrings( SoundListBox->Items );
	StopSoundComboBox->Items->AddStrings( SoundListBox->Items );
	PxAlarmSoundComboBox->Items->AddStrings( SoundListBox->Items );
	LowPxAlarmSoundComboBox->Items->AddStrings( SoundListBox->Items );
	FillSoundComboBox->Items->Add( L"語音" );

	SoundFile = g_Config.GetStringProperty("Setting","FillSound","收銀機.wav" );
	if( (index = FillSoundComboBox->Items->IndexOf( SoundFile ))!= -1 )
		FillSoundComboBox->ItemIndex = index;
	else
		FillSoundComboBox->ItemIndex = 0;
	SoundFile = g_Config.GetStringProperty("Setting","NewSound","貓叫.wav" );
	if( (index = NewOrdSoundComboBox->Items->IndexOf( SoundFile ))!= -1 )
		NewOrdSoundComboBox->ItemIndex = index;
	else
		NewOrdSoundComboBox->ItemIndex = 0;
	SoundFile = g_Config.GetStringProperty("Setting","RejectSound","烏鴉.wav" );
	if( (index = RejectSoundComboBox->Items->IndexOf( SoundFile ))!= -1 )
		RejectSoundComboBox->ItemIndex = index;
	else
		RejectSoundComboBox->ItemIndex = 0;
	SoundFile = g_Config.GetStringProperty("Setting","StopSound","貓頭鷹.wav" );
	if( (index = StopSoundComboBox->Items->IndexOf( SoundFile ))!= -1 )
		StopSoundComboBox->ItemIndex = index;
	else
		StopSoundComboBox->ItemIndex = 0;

	SoundFile = g_Config.GetStringProperty("Setting","PxAlarmSound","filled.wav" );
	if( (index = PxAlarmSoundComboBox->Items->IndexOf( SoundFile ))!= -1 )
		PxAlarmSoundComboBox->ItemIndex = index;
	else
		PxAlarmSoundComboBox->ItemIndex = 0;

	SoundFile = g_Config.GetStringProperty("Setting","LowPxAlarmSound","Warnings.wav" );
	if( (index = PxAlarmSoundComboBox->Items->IndexOf( SoundFile ))!= -1 )
		LowPxAlarmSoundComboBox->ItemIndex = index;
	else
		LowPxAlarmSoundComboBox->ItemIndex = 0;


	SoundListBox->Directory = CurrentDir;
	delete SoundListBox;

   FillSoundSwitch->Selected       = g_Config.GetBoolProperty("Setting","EnableFillSound",true);
   NewSoundSwitch->Selected        = g_Config.GetBoolProperty("Setting","EnableNewSound",true);
   RejectSoundSwitch->Selected     = g_Config.GetBoolProperty("Setting","EnableRejectSound",true);
   StopSoundSwitch->Selected       = g_Config.GetBoolProperty("Setting","EnableStopSound",true);
   PxAlarmSoundSwitch->Selected    = g_Config.GetBoolProperty("Setting","EnablePxAlarmSound",true);
   LowPxAlarmSoundSwitch->Selected = g_Config.GetBoolProperty("Setting","EnableLowPxAlarmSound",true);

   NewConfirmSwitch->Selected    = g_Config.GetBoolProperty("Setting","EnableNewConfirm",true);
   CancelConfirmSwitch->Selected = g_Config.GetBoolProperty("Setting","EnableCxlConfirm",true);
   LightRadioButton->Checked     = g_Config.GetBoolProperty("Setting","Skin", true );
   DarkRadioButton->Checked      = !LightRadioButton->Checked;
   LoadWaves();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SaveWavesSetting( void )
{
   g_Config.SetStringProperty("Setting","FillSound", FillSoundComboBox->Text );
   g_Config.SetStringProperty("Setting","NewSound",  NewOrdSoundComboBox->Text );
   g_Config.SetStringProperty("Setting","RejectSound", RejectSoundComboBox->Text );
   g_Config.SetStringProperty("Setting","StopSound", StopSoundComboBox->Text );
   g_Config.SetStringProperty("Setting","PxAlarmSound", PxAlarmSoundComboBox->Text );
   g_Config.SetStringProperty("Setting","LowPxAlarmSound", LowPxAlarmSoundComboBox->Text );

   g_Config.SetBoolProperty("Setting","EnableFillSound",  FillSoundSwitch->Selected );
   g_Config.SetBoolProperty("Setting","EnableNewSound",   NewSoundSwitch->Selected );
   g_Config.SetBoolProperty("Setting","EnableRejectSound", RejectSoundSwitch->Selected );
   g_Config.SetBoolProperty("Setting","EnableStopSound", StopSoundSwitch->Selected );
   g_Config.SetBoolProperty("Setting","EnablePxAlarmSound", PxAlarmSoundSwitch->Selected );
   g_Config.SetBoolProperty("Setting","EnableLowPxAlarmSound", LowPxAlarmSoundSwitch->Selected );

   g_Config.SetBoolProperty("Setting","EnableNewConfirm", NewConfirmSwitch->Selected );
   g_Config.SetBoolProperty("Setting","EnableCxlConfirm", CancelConfirmSwitch->Selected );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Speak( const String& Str )
{
	if( FVoice != NULL)
	{
		WideString WS( Str );
		FVoice->Speak( WS.c_bstr(), SPF_PURGEBEFORESPEAK|SPF_ASYNC, NULL);
	}
}
//---------------------------------------------------------------------------
const int BORDER_WIDTH = 3;
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnNCCalcSize( TMessage &Message )
{
	RECT* pRect0;
	RECT* pRect1;
	RECT* pRect2;
	LPNCCALCSIZE_PARAMS pNCSP;

	if( Message.WParam == TRUE )
	{
		pNCSP = (LPNCCALCSIZE_PARAMS)Message.LParam;
		pRect0 = &pNCSP->rgrc[0];
		pRect1 = &pNCSP->rgrc[1];
		pRect2 = &pNCSP->rgrc[2];

		pRect0->left    = pRect0->left   + BORDER_WIDTH;
		pRect0->top     = pRect0->top    + BORDER_WIDTH;
		pRect0->right   = pRect0->right  - BORDER_WIDTH;
		pRect0->bottom  = pRect0->bottom - BORDER_WIDTH;

		pRect2->left    = pRect1->left   + BORDER_WIDTH;
		pRect2->top     = pRect1->top    + BORDER_WIDTH;
		pRect2->right   = pRect1->right  - BORDER_WIDTH;
		pRect2->bottom  = pRect1->bottom - BORDER_WIDTH;
		Message.Result = 0;
	}
	else ///< lParam Windows Rect
	{    ///< return Client Rect
		pRect0 = (RECT*)Message.LParam;
		pRect0->left    = pRect0->left   + BORDER_WIDTH;
		pRect0->top     = pRect0->top    + BORDER_WIDTH;
		pRect0->right   = pRect0->right  - BORDER_WIDTH;
		pRect0->bottom  = pRect0->bottom - BORDER_WIDTH;
		Message.Result = 0;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnNCPaint( TMessage &Message )
{
	if( WindowState == wsMinimized )
	{
		inherited::WndProc( Message );
		return;
	}
	///< wParam
	///< Handle to the update region of the window.
	///< The update region is clipped to the window frame.
	///< When wParam is 1, the entire window frame needs to be updated.
	if( Message.WParam == 1 )
	{
		HRGN     WinRgn,ClientRgn;
		RECT     WinRect;

		GetWindowRect( Handle, &WinRect );
		WinRgn = CreateRectRgnIndirect( &WinRect );
		WinRect.left   += BORDER_WIDTH;
		WinRect.top    += BORDER_WIDTH;
		WinRect.right  -= BORDER_WIDTH;
		WinRect.bottom -= BORDER_WIDTH;
		ClientRgn = CreateRectRgnIndirect( &WinRect );
		CombineRgn( WinRgn, WinRgn, ClientRgn, RGN_DIFF);
		DrawNCArea( WinRgn );
		DeleteObject((HGDIOBJ)WinRgn );
	}
	else
		DrawNCArea( (HRGN) Message.WParam );
	Message.Result = 1;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::DrawNCArea( HRGN Rgn  )
{
	TCanvas* FormCanvas;
	TRect    WinRect,DrawRect;
	int      WinWidth,WinHeight;

	FormCanvas = new TCanvas( );
	FormCanvas->Handle =  GetDCEx( Handle,(HRGN)Rgn,
								   DCX_WINDOW|DCX_CACHE|
								   DCX_CLIPSIBLINGS|DCX_INTERSECTRGN );
	///< Get Windows current position.
	GetWindowRect( Handle, &WinRect );
	WinWidth  = WinRect.Width();
	WinHeight = WinRect.Height();
	///< Deaw Caption
	DrawRect = TRect( 0,0, WinWidth, BORDER_WIDTH );
	FormCanvas->Pen->Color = TColor( 0x00211B16 );
	FormCanvas->Brush->Color = TColor( 0x00211B16 );
	FormCanvas->FillRect( DrawRect );
	if( GSimMatch == true )
	{
		FormCanvas->Pen->Color = clRed;
		FormCanvas->Brush->Color = clRed;
		///< Top border
		DrawRect = TRect( 0,0, WinWidth, BORDER_WIDTH );
		FormCanvas->FillRect( DrawRect );
	}
	///< Left border
	DrawRect = TRect( 0,0, BORDER_WIDTH, WinHeight );
	FormCanvas->FillRect( DrawRect );
	///< Right border
	DrawRect = TRect( WinWidth-BORDER_WIDTH, 0, WinWidth, WinHeight );
	FormCanvas->FillRect( DrawRect );
	///< Bottom border
	DrawRect = TRect( 0, WinHeight-BORDER_WIDTH, WinWidth, WinHeight );
	FormCanvas->FillRect( DrawRect );

	///< Paint finished, Release resource.
	ReleaseDC( Handle, FormCanvas->Handle );
	delete FormCanvas;
	RedrawWindow( Handle, NULL/*&WinRect*/,(HRGN)Rgn, RDW_INVALIDATE|RDW_FRAME );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnNCHitTest( TMessage &Message )
{
	POINT point;
	RECT  WinRect;

	GetWindowRect( Handle, &WinRect);
	point.x = GET_X_LPARAM( Message.LParam ) - WinRect.left;
	point.y = GET_Y_LPARAM( Message.LParam ) - WinRect.top;
	if( point.y > 5 && point.y < 100 /*&& point.x > 50*/ )
	{
		Message.Result = HTCAPTION;
		return;
	}
	inherited::WndProc( Message );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnNCActive( TMessage &Message )
{
	{
		SuppressStyle ss( Handle, WS_VISIBLE );
		inherited::WndProc( Message );
	}
	RedrawWindow( Handle, NULL, NULL, RDW_INVALIDATE|RDW_FRAME );
	Message.Result = 1;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WndProc( TMessage &Msg )
{
	switch( Msg.Msg )
	{
		case WM_ERASEBKGND: PaintBk( Msg ); return;
		case WM_NCCALCSIZE: OnNCCalcSize( Msg ); return;
		case WM_NCPAINT:    OnNCPaint( Msg );return;
		case WM_NCACTIVATE: OnNCActive( Msg );return;
		case WM_NCHITTEST:  OnNCHitTest( Msg );	return;
		case WM_USER +100:  Application->Restore();
							Show();
							Application->BringToFront();
							return;
	}
	inherited::WndProc( Msg );
}
//---------------------------------------------------------------------------
HRGN __fastcall TMainForm::ExcludeRect( HRGN CaptionRgn, TRect EraseRect )
{
	HRGN EraseRgn;
	EraseRgn = CreateRectRgnIndirect( &EraseRect );
	CombineRgn( CaptionRgn, CaptionRgn, EraseRgn, RGN_DIFF );
	DeleteObject((HGDIOBJ)EraseRgn );
	return CaptionRgn;
}
//---------------------------------------------------------------------------
HRGN __fastcall TMainForm::CreateCaptionDrawRgn( void )
{
	TRect CaptionRect = TRect(0,0,ClientWidth, 50 );
	HRGN  CaptionRgn  = CreateRectRgnIndirect( &CaptionRect );
	int   TabCount = FTabs.ItemCount();

	CaptionRgn = ExcludeRect( CaptionRgn, ExButton->BoundsRect ); ///< Exclude ExButton

	for( register int i = 0; i < TabCount; i++ ) ///< hotkey dup
	{
		if( FTabs[i]->Visible == true )
			CaptionRgn = ExcludeRect( CaptionRgn, FTabs[i]->BoundsRect ); ///< Exclude Tab1Button
	}
	CaptionRgn = ExcludeRect( CaptionRgn, SearchEditBox->BoundsRect ); ///< Exclude SearchEditBox
	CaptionRgn = ExcludeRect( CaptionRgn, SearchButton->BoundsRect ); ///< Exclude SearchButton
	CaptionRgn = ExcludeRect( CaptionRgn, MinButton->BoundsRect ); ///< Exclude MinButton
	CaptionRgn = ExcludeRect( CaptionRgn, MaxButton->BoundsRect ); ///< Exclude MaxButton
	CaptionRgn = ExcludeRect( CaptionRgn, CloseButton->BoundsRect ); ///< Exclude CloseButton
	return CaptionRgn;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::PaintBk( TMessage &Msg )
{
	FBKBuffer->Canvas->Font = Font;
	FStatusBuffer->Canvas->Font = Font;
	int TextH = FBKBuffer->Canvas->TextHeight( "12345678CppmanXYZ" );
	int BorderH = (CAPTION_H - TextH )/2;
	int BottomTextY = (BOTTOM_H - TextH )/2;
	int SearchBoxR = SearchButton->Left + SearchButton->Width;
	TRect CaptionRect = TRect(0,0,ClientWidth, CAPTION_H );
	//TRect IDRect = TRect( SubscribeButton->Left + 72, BorderH, SubscribeButton->Left + 186, CAPTION_H-BorderH );
	TRect SearchRect = TRect( SearchEditBox->Left, 10, SearchBoxR, CAPTION_H - 10 );
	HRGN  CaptionRgn = CreateCaptionDrawRgn();
	TTextFormat Formats,CRFormats;

	if( FBKBuffer->Width != ClientWidth )
		FBKBuffer->Width = ClientWidth;
	if( FStatusBuffer->Width != ClientWidth )
		FStatusBuffer->Width = ClientWidth;
	///< Draw caption
	Formats<<tfLeft<<tfVerticalCenter<<tfEndEllipsis;
	CRFormats<<tfRight<<tfVerticalCenter;
	FBKBuffer->Canvas->Pen->Color = TColor( 0x002d1605 );
	FBKBuffer->Canvas->Brush->Color = TColor( 0x002d1605 );
	FBKBuffer->Canvas->FillRect( CaptionRect );
	///< Draw Search box
	FBKBuffer->Canvas->Pen->Color = TColor( 0x00402B1C );
	FBKBuffer->Canvas->Brush->Color = TColor( 0x00402B1C );
	FBKBuffer->Canvas->FillRect( SearchRect );
	FBKBuffer->Canvas->Ellipse( SearchEditBox->Left - 15, 10, SearchEditBox->Left +15 ,CAPTION_H - 10 );
	FBKBuffer->Canvas->Ellipse( SearchBoxR -15, 10, SearchBoxR + 15 ,CAPTION_H - 10 );
	///< Draw Speedy logo
	LogoImageList->Draw( FBKBuffer->Canvas, 40, 12, 0 );

	SelectClipRgn( Canvas->Handle, CaptionRgn );
	Canvas->Draw( 0,0, FBKBuffer );
	SelectClipRgn( Canvas->Handle, NULL );
	DeleteObject((HGDIOBJ)CaptionRgn );
	///< Draw bottom gray bar
	PaintStatusBar();
	Msg.Result = 1;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::PaintStatusBar( void )
{
	String MDRTTStr( L"行情品質" );
	int TextH = FStatusBuffer->Canvas->TextHeight( "12345678CppmanXYZ" );
	int TextW = FStatusBuffer->Canvas->TextWidth( MDRTTStr ) + 5;
	int BottomTextY = (BOTTOM_H - TextH )/2;
	TRect BottomRect = TRect( 0, 0, ClientWidth, BOTTOM_H );
	TRect RTTGraph = TRect( TextW, 1, TextW + RTT_WIDTH, BOTTOM_H - 1 );

	///< Draw bottom gray bar
	FStatusBuffer->Canvas->Pen->Color = TColor( 0x00a0a0a0 );
	FStatusBuffer->Canvas->Brush->Color = TColor( 0x00a0a0a0 );
	FStatusBuffer->Canvas->FillRect( BottomRect );
	FStatusBuffer->Canvas->Font->Color = clWhite;
	FStatusBuffer->Canvas->TextOutW( ClientWidth - FStatusBuffer->Canvas->TextWidth( FCopyRights ), BottomTextY, FCopyRights ); ///< Draw copyright string
	FStatusBuffer->Canvas->Font->Color = clWhite;
	FStatusBuffer->Canvas->TextOutW( 2,   BottomTextY, MDRTTStr ); ///< Draw MDRTT label
	FStatusBuffer->Canvas->TextOutW( TextW + RTT_WIDTH + 2, BottomTextY, FLoginUserStr ); ///< Draw login info
	PaintMDRTT( FStatusBuffer->Canvas, RTTGraph );
	Canvas->Draw( 0,ClientHeight - BOTTOM_H, FStatusBuffer );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CMarketDataStoreContractDownloadCompleted(int Count, int UseMS)
{
	// select contract popup menu.
	ContractViewerForm->AddSymbols( LoginForm->StatusLabel );
	// Select contract form in main form
	ContractInfoForm->Parent = ProdTabSheet;
	ContractInfoForm->Align = alClient;
	ContractInfoForm->Visible = true;
	ContractInfoForm->AddSymbol( LoginForm->StatusLabel );
	ContractInfoForm->LoadCustomNames();
	// select contract dialog.
	SelectContractForm->AddSymbol( NULL );
	SelectContractForm->LoadCustomNames();
	FDownloadOK = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Connect()
{
	FLoginBroker = g_Config.BrokerConfig( 0 );
	FDownloadOK = false;
	FConnectFailed = false;
	LoginForm->StatusLabel->Caption = L"連線行情伺服器...";
	Application->ProcessMessages();
	String ServerIP;
	int Port;

	if( GUseFixIP == true )
	{
		ServerIP = MD_SERVER_IP;
		Port     = MD_SERVER_PORT;
	}
	else if( GVIPServer == true )
		ServerIP = g_Config.GetVIPStarWaveIP( Port );
	else
		ServerIP = g_Config.GetStarWaveIP( Port );
	CMarketDataStore->IP   = ServerIP;
	CMarketDataStore->Port = Port;
	CMarketDataStore->Connect( 5 );
	if( CMarketDataStore->IsConnected() != true )
	{
		FConnectFailed = true;
		return;
	}
	LoginForm->StatusLabel->Caption = L"連線線圖伺服器...";
	Application->ProcessMessages();
	if (GUseFixIP == true)
	{
		ServerIP = CHART_SERVER_IP;
		Port = CHART_SERVER_PORT;
	}
	else if (GVIPServer == true)
		ServerIP = g_Config.GetVIPChartServerIP(Port);
	else
		ServerIP = g_Config.GetChartServerIP(Port);
	ChartsStore->IP = ServerIP;
	ChartsStore->Port = Port;
	ChartsStore->Connect(5);

	if (ChartsStore->IsConnected() == false)
		FConnectFailed = true;
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::Ready( void )
{
	if( ChartsStore->IsConnected() == true &&
		CMarketDataStore->IsConnected() == true &&
		FDownloadOK == true )
		return true;
	return false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CMarketDataStoreAppConnected(TObject *Sender)
{
	LoginForm->StatusLabel->Caption = L"行情主機連線成功,下載商品中...";
	CMarketDataStore->DownloadContract();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ChartsStoreXAppConnected(TObject *Sender)
{
	Speak( L"行情主機連線成功");
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::imgMenuClick(TObject *Sender)
{
  if( SV->Opened )
	  SV->Close();
  else
  {
	if( SettingSV->Opened )
		SettingSV->Close();
	SV->Open();
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::actProductsExecute(TObject *Sender)
{
	if( SettingSV->Opened == false )
	{
		SettingPageControl->ActivePage = ProdTabSheet;
		SettingPanel( true );
	}
	else
		SettingPanel( false );
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::actSettingExecute(TObject *Sender)
{
	SettingPageControl->ActivePage = TabNormal;
	SettingPanel( true );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::actSettingHotkeyExecute(TObject *Sender)
{
	SettingPageControl->ActivePage = TabHotKey;
	SettingPanel( true );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ShowTabs( bool Show )
{
	int   TabCount = FTabs.ItemCount();

	for( register int i = 0; i < TabCount; i++ )
	{
		FTabs[i]->Visible = Show;
		if( FTabs[i]->Selected == true )
			PageControl->ActivePageIndex = i;
	}
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::SettingPanel( bool Open )
{
	if( Open == SettingSV->Opened )
		return false;
	if( Open == true )
	{
		int SVWidth = 0;
		SettingSV->Width = ClientRect.Width() - SVWidth;
		SettingSV->Open();
		SettingSV->Align = alClient;
		TabsPanel->Visible = false;
		ShowTabs( false );
		return true;
	}
	else
	{
		if( SettingPageControl->ActivePage == TabHotKey )
		{
			SaveHotkey( );
			ContractViewerForm->LoadHotkey( NULL );
		}
		TabsPanel->Visible = true;
		SettingPageControl->ActivePage = TabNormal;
		SettingSV->Align = alNone;
		SettingSV->Close();
		ShowTabs( true );
	}
	return true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::InitOrderStore( void )
{
	OrderStore->InitialOrderAdapter();
	OrderStore->SetCAListener( NULL );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LiveUpdate( void )
{
	int UpdateCount = UpdateForm->GetUpdateCount();

	if( UpdateCount > 0 )
	{
		if( UpdateForm->UpdateMainProgram() == true )
		{
			TModalResult Result = TAskUpdateForm::AskForUpdate();

			if( Result == mrOk ) ///< Update files
			{
				UpdateForm->ShowModal();
			}
			else if( Result == mrClose )  ////< Re-Install
			{
				UpdateForm->ShowModal();
				SHELLEXECUTEINFO si;
				/// Fill SHELLEXECUTEINFO structure for ShellExecute Win API.
				ZeroMemory( &si, sizeof(si) );
				si.cbSize = sizeof(si);
				si.lpVerb = L"open";
				si.lpFile = L"SpeedyUnifySetup.exe";
				si.lpParameters = L"";
				si.hInstApp = GHinstance;
				si.nShow = SW_SHOWDEFAULT;
				si.fMask = SEE_MASK_NOCLOSEPROCESS;
				ShellExecuteEx( &si );   /// Execute the PMFNext
				Application->Terminate();
			}
		}
		else
			UpdateForm->ShowModal();
		g_Config.LoadAllIni();
	}
	else if( UpdateCount < 0 )
	{
		if( TUnifyDlgs::AskYesNoDialog( "更新失敗", "無法連線到更新伺服器,是否要繼續執行?" ) == false )
			Application->Terminate();
	}
}
//---------------------------------------------------------------------------
void __fastcall Test( void )
{
	 String Str("{\"column1\":\"123\",\"column2\":\"S12345\",\"checksum\":\"9873e0036fed71ed0c5b8a129a2f60dd\"}");
	 String Result = Base64Encode( Str );

	 ShowMessage( Result );

	 Result =  Base64Decode( Result );
	 ShowMessage( Result );

}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ControlPosition( void )
{
	BackgroundPanel->Top = CAPTION_H;
	BackgroundPanel->Height = ClientHeight - BOTTOM_H - CAPTION_H;
	Tab1Button->Top = 15;
	Tab1Button->Left = 205;
	Tab2Button->Top = 15;
	Tab2Button->Left = Tab1Button->Left + Tab1Button->Width + 1;
	Tab3Button->Top = 15;
	Tab3Button->Left = Tab2Button->Left + Tab2Button->Width + 1;
	Tab4Button->Top = 15;
	Tab4Button->Left = Tab3Button->Left + Tab3Button->Width + 1;
	Tab5Button->Top = 15;
	Tab5Button->Left = Tab4Button->Left + Tab4Button->Width + 1;

	CloseButton->Top = 10;
	CloseButton->Left = ClientWidth - CloseButton->Width - 5;
	MaxButton->Top = 10;
	MaxButton->Left = CloseButton->Left - MaxButton->Width;
	MinButton->Top = 10;
	MinButton->Left = MaxButton->Left - MinButton->Width;

	SearchButton->Top = 10;
	SearchButton->Left = MinButton->Left - 32;

	SearchEditBox->Height = 25;
	SearchEditBox->Top = (CAPTION_H - SearchEditBox->Height )/2;
	SearchEditBox->Left = SearchButton->Left - SearchEditBox->Width;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::EnableNuclear( void )
{
	NuclearBuyLabel->Visible = gNuclear;
	NuclearSellLabel->Visible = gNuclear;
	NuclearBuyText->Visible = gNuclear;
	NuclearSellText->Visible = gNuclear;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormShowTimerTimer(TObject *Sender)
{
	FormShowTimer->Enabled = false;
	catMenuItems->Enabled = false;
	LoadWavesSetting();
	LoadProperties( );
	EnableNuclear();
	SettingPanel( false );
	catMenuItems->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormShow(TObject *Sender)
{
	if( TrayIcon->Visible == false )
	{
		LiveUpdate();
		if( LoginForm->ShowModal() == mrOk )
		{
			ControlPosition();
			CMarketDataStore->OnAppDisconnected  = CMarketDataStoreAppDisconnected;
			ChartsStore->OnAppDisconnected  = ChartsStoreXAppDisconnected;
			SettingSV->Opened = false;
			LoadMainFormPosition();
			SettingPageControl->ActivePage = TabSheetLoading;
			SettingPanel( true );
			FormShowTimer->Enabled = true;
			RTTTimer->Enabled = true;
			PreventIdleTimer->Enabled = true;
		}
		else
		{
			Close();
			Application->Terminate();
		}
	}
	else
		TrayIcon->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::DefaultSymbol( String& Ex, String& Sym )
{
	if( (gMarketDataStore->GetBasicInformation( "TAIFEX", "TXFFront1", false ))!= NULL )
	{
		Ex  = L"TAIFEX";
		Sym = L"TXFFront1";
	}
	else if( (gMarketDataStore->GetBasicInformation( "TWSE", "2330", false ))!= NULL )
	{
		Ex  = L"TWSE";
		Sym = L"2330";
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::actDepthExecute(TObject *Sender)
{
	String Ex,Sym;

	if( SettingPanel( false ) == false )
	{
		DefaultSymbol( Ex, Sym );
		ContractViewerForm->OpenOrderBookForm(  Ex, Sym, true  );
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::actListExecute(TObject *Sender)
{
	if( SettingPanel( false ) == false )
		ContractViewerForm->OpenMarketDataListForm( -1, true );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::actKBarExecute(TObject *Sender)
{
	String Ex,Sym;

	if( SettingPanel( false ) == false )
    {
		DefaultSymbol( Ex, Sym );
		ContractViewerForm->OpenKBarForm( Ex, Sym );
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::actExecExecute(TObject *Sender)
{
	if( OrderStore->IsReady() == true )
		ContractViewerForm->OpenExecutionForm();
	else
	{
		Speak( L"請先登入下單Server" );
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"請先登入下單服務器" );
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	if( FUILoaded == true )
	{
		if( FForceClose == true )
		{
			TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"交易主機斷線,請重新啟動程式" );
			SaveProperties( );
			CloseAll();
		}
		else if( TUnifyDlgs::AskYesNoDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"確定要登出並離開程式?") == true )
		{
			SaveProperties( );
			CloseAll();
		}
		else
		{
			Action = System::Uitypes::TCloseAction::caNone;
			return;
		}
	}
	for( int i = 0; i < 100; i++ ) ///< wait 1 sec for all disconnected.
	{
		Application->ProcessMessages();
		if( CMarketDataStore->IsConnected() == false &&
			ChartsStore->IsConnected() == false &&
			OrderStore->IsConnected() == false )
			return;
		UFC::SleepMS( 10 );
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CloseAll( void )
{
	FUILoaded = false;
	RTTTimer->Enabled = false;
	PreventIdleTimer->Enabled = false;
	ContractViewerForm->CloseAllForm();
	ContractViewerForm->ClearSymbols();
	CMarketDataStore->OnAppDisconnected  = NULL;
	ChartsStore->OnAppDisconnected  = NULL;
	OrderStore->Logoff( true );
	OrderStore->Disconnect( true );
	CMarketDataStore->Clear();
	CMarketDataStore->Disconnect();
	ChartsStore->Disconnect();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CMarketDataStoreAppDisconnected(TObject *Sender)
{
	if( LoginForm->Visible == false )
	{
		if( Visible == true )
		{
			SaveProperties( );
			Visible = false;
			CloseAll( );
			if( Sender != NULL )
				TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"行情主機斷線,請重新登入." );
			else
	            TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"線圖主機斷線,請重新登入." );
			LoginForm->StatusLabel->Caption = L"";
			Show();
		}
	}
	else
		LoginForm->StatusLabel->Caption = L"行情主機連線失敗";
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ChartsStoreXAppDisconnected(TObject *Sender)
{
	CMarketDataStoreAppDisconnected( NULL );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::KeyCodeComboBoxKeyPress(TObject *Sender, System::WideChar &Key)
{
	if( Key != VK_UP && Key != VK_DOWN )
		Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::KeyCodeComboBoxMouseEnter(TObject *Sender)
{
	KeyCodeComboBox->SetFocus();
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::IsHotkeyDup( const String& NewHotkey )
{
	String DisableName = KeyCodeComboBox->Items->Strings[ 0 ];

	for( register int j = 0; j < FHotkeys.ItemCount(); j++ ) ///< hotkey dup
	{
		if( FHotkeys[j]->ButtonText != DisableName && FHotkeys[j]->ButtonText == NewHotkey )
		{
			TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName,L"熱鍵重複設定" );
			return true;
		}
	}
	return false;
}
//------------------------------------------------------------------------------
void __fastcall TMainForm::KeyCodeComboBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	for( register int i = 0; i < HOT_KEY_COUNT; i++ ) ///< Search hotkey
	{
		if( Key == HotkeyCode[ i ] )
		{
			String NewHotkey   = KeyCodeComboBox->Items->Strings[ i ];

			if( FSelectedFuncText->ButtonText != NewHotkey )
			{
				if( IsHotkeyDup( NewHotkey ) == true )
				{
					KeyCodeComboBox->ItemIndex = 0;
					FSelectedFuncText->ButtonText = KeyCodeComboBox->Items->Strings[ 0 ];
				}
				else
				{
					KeyCodeComboBox->ItemIndex = i;
					FSelectedFuncText->ButtonText = NewHotkey;
				}
				Key = 0;
				return;
			}
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TMainForm::KeyCodeComboBoxChange(TObject *Sender)
{
	if( FSelectedFuncText != NULL )
	{
		String NewHotkey  = KeyCodeComboBox->Text;

		if( FSelectedFuncText->ButtonText != NewHotkey ) ///< Not same hotkey.
		{
			String DisableName = KeyCodeComboBox->Items->Strings[ 0 ];

			if( IsHotkeyDup( NewHotkey ) == true )
			{
				KeyCodeComboBox->ItemIndex = 0;
				FSelectedFuncText->ButtonText = KeyCodeComboBox->Items->Strings[ 0 ];
			}
			FSelectedFuncText->ButtonText = NewHotkey;
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TMainForm::BuyTextClick(TObject *Sender)
{
	TGraphButton* FuncText = dynamic_cast<TGraphButton*>( Sender );
	if( FuncText != NULL )
	{
		int KeyIndex = KeyCodeComboBox->Items->IndexOf( FuncText->ButtonText );

		FSelectedFuncText = FuncText;
		if( KeyIndex == -1 )
			KeyIndex = 0;
		KeyCodeComboBox->Parent =  FuncText->Parent;
		KeyCodeComboBox->ItemIndex = KeyIndex;
		KeyCodeComboBox->SetBounds( FuncText->Left, FuncText->Top, FuncText->Width, FuncText->Height );
		if( KeyCodeComboBox->Visible == false )
			KeyCodeComboBox->Visible = true;
	}
}
//------------------------------------------------------------------------------
void __fastcall TMainForm::BuyTextMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y)
{
	if( Button == mbRight )
	{
		TGraphButton* FuncText = dynamic_cast<TGraphButton*>( Sender );
		if( FuncText != NULL )
		{
			int KeyIndex = KeyCodeComboBox->Items->IndexOf( FuncText->ButtonText );

			FSelectedFuncText = FuncText;
			if( KeyIndex == -1 )
				KeyIndex = 0;
			KeyCodeComboBox->Parent =  FuncText->Parent;
			KeyCodeComboBox->ItemIndex = KeyIndex;
			KeyCodeComboBox->SetBounds( FuncText->Left, FuncText->Top, FuncText->Width, FuncText->Height );
			if( KeyCodeComboBox->Visible == false )
				KeyCodeComboBox->Visible = true;
			WORD Key = VK_RBUTTON;
			KeyCodeComboBoxKeyDown( Sender, Key, Shift);
		}
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::TabHotKeyMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y)
{
	if( KeyCodeComboBox->Visible == true )
		KeyCodeComboBox->Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ExitButtonClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OrderStoreDisconnect(TObject *Sender)
{
	OrderStore->CancelAllStopOrder();

	CAButton->Visible = false;
	CxlWorkingButton->Visible = false;
	FLoginUserStr = L"交易帳號:尚未登入";
	ContractViewerForm->UnregisterOrderStore();
	ContractViewerForm->CloseExecutionForm();
	if( HoldOpenInterestForm != NULL )
	{
		HoldOpenInterestForm->Close();
		delete HoldOpenInterestForm;
		HoldOpenInterestForm = NULL;
	}
	LeaderBoardForm->Close();
	PaintStatusBar( );
	FForceClose = true;
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CheckAgreement( void )
{
	String Reason;

	if( FLoginBroker != NULL &&
		FLoginBroker->GetService()->SignAgreememt( gUser.LoginUserID,FCAChecker, Reason ) == false )
	{
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, Reason );
		OrderStore->Disconnect( true );
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LoginSimBroker( void )
{
	AnsiString       PhoneNo( gUser.UserID.c_str() );
	UFC::AnsiString  VAccount;
	String           OrderIP;
	AnsiString       LogFilePrefix;
	int              OrderPort;

	if( PhoneNo.Length()>= 10 )
		VAccount =  UFC::AnsiString( PhoneNo.c_str() + 3 );
	else
		VAccount =  UFC::AnsiString( PhoneNo.c_str() );
	VAccount.PadThis( 7 , '8' );

	OrderIP = g_Config.GetSimOrderServerIP( OrderPort, CMarketDataStore->IP, 45678 );
	FProxyLogon               = true;
	gUser.LoginUserID         = VAccount.c_str();
	gUser.AccountType         = hatBoth;
	LogFilePrefix.printf( "SU_SIM_%s", VAccount.c_str() );
	OrderStore->ClearPosStatisticRecHoldPosition();
	OrderStore->OrderLogFileNamePrefix = LogFilePrefix;
	OrderStore->IP            = OrderIP;  //L"60.250.82.135";
	OrderStore->Port          = OrderPort;
	OrderStore->BrokerID      = "F888000";
	OrderStore->TWSEBrokerID  = "8888";
	OrderStore->ClearMemberID = "F888";
	OrderStore->ID            = VAccount.c_str();
	OrderStore->Account       = VAccount.c_str();
	OrderStore->TWSEAccount   = VAccount.c_str();
	OrderStore->IB            = L"000";
	OrderStore->Password      = "888888";
	OrderStore->Version       = LoginForm->Version;
	//OrderStore->TryVersion    = gIsExpired;
	OrderStore->TryVersion    = false;
	OrderStore->MaxLots 	  =	g_Config.GetMaxLots();
	if( OrderStore->RecoverFileExist() == true )
	{
		if( TUnifyDlgs::AskYesNoDialog( L"練功房", L"是否刪除之前的練習成績?" ) == true )
			OrderStore->RemoveRecoverFile();
	}
	FLoginUserStr.printf( L"虛擬帳號:%s %s",OrderStore->Account, gUser.Nickname );
	CAButton->Left = RTTGraphRect.Right + 10 + FStatusBuffer->Canvas->TextWidth( FLoginUserStr );
	CxlWorkingButton->Left = CAButton->Left + CAButton->Width + 10;
	PaintStatusBar( );
	s888::CTaifexFeeQueryObject*     FeeObj = new s888::CTaifexFeeQueryObject( L"TaifexFee.xml", OrderStore->Account  );
	s888::CTaifexTaxRateQueryObject* TaxObj = new s888::CTaifexTaxRateQueryObject( L"TaifexTax.xml" );
	delete FeeObj;
	delete TaxObj;
	OrderStore->Connect();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CancelButtonClick(TObject *Sender)
{
	SettingPanel( false );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Tab1ButtonClick(TObject *Sender)
{
	TGraphButton* Btn = dynamic_cast<TGraphButton*>(Sender);

	if( Btn == NULL)
		return;

	if( Btn->Tag == 1 && ContractInfoForm->SupportOptions() == false )
	{
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"沒有選擇權商品");
		return;
	}
	int TabCount = FTabs.ItemCount();

	for( register int j = 0; j < TabCount; j++ ) ///< hotkey dup
	{
		if( FTabs[j] != Btn )
			FTabs[j]->Selected = false;
	}
	Btn->Selected = true;
	PageControl->ActivePageIndex = Btn->Tag;
	ContractViewerForm->ShowPage( PageControl->ActivePageIndex );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SetPage( int Page )
{
	int   TabCount = FTabs.ItemCount();

	if( Page < 0 || Page >= TabCount )
		Page = 0;
	for( register int i = 0; i < TabCount; i++ ) ///< hotkey dup
	{
		if( Page == i )
			FTabs[i]->Selected = true;
		else
			FTabs[i]->Selected = false;
	}
	PageControl->ActivePageIndex = Page;
	ContractViewerForm->ShowPage( Page );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::actReportExecute(TObject *Sender)
{
	if( OrderStore->IsReady() == false )
	{
		Speak( L"請先登入下單服務器" );
		TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"請先登入下單服務器" );
	}
	else
	{
		if( HoldOpenInterestForm == NULL )
            HoldOpenInterestForm = new THoldOpenInterestForm( this );
		HoldOpenInterestForm->LoadProperties( );
		HoldOpenInterestForm->Show();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SearchBoxMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y)
{
	SearchEditBox->Text = L"";
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormActivate(TObject *Sender)
{
	ContractViewerForm->Hide();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SearchEditBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	if( Key == VK_RETURN )
		SearchBoxInvokeSearch( this );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SearchBoxInvokeSearch(TObject *Sender)
{
	ContractViewerForm->Left = Left+ SearchEditBox->Left;
	ContractViewerForm->Top  = Top + SearchEditBox->Top + SearchEditBox->Height + 10;
	ContractViewerForm->SearchEdit->Text = SearchEditBox->Text;
	ContractViewerForm->SearchGlyphButtonClick( NULL );
	ContractViewerForm->OnSelectSymbol = SelectSymbol;
	ContractViewerForm->Show();
	SearchEditBox->Text = L"搜尋商品";
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SelectSymbol( System::TObject* Sender, TSelectActionType Type, const String& Ex, const String& Sym )
{
	switch( Type )
	{
		case satSymbol:
		case satDepth:  ContractViewerForm->OpenOrderBookForm( Ex, Sym, true );
						break;
		case satKChart: ContractViewerForm->OpenKBarForm( Ex, Sym );
						break;
	}
	ContractViewerForm->OnSelectSymbol = NULL;
	ContractViewerForm->Hide();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ApplicationEventsDeactivate(TObject *Sender)
{
	ContractViewerForm->Hide();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LoadTabName( void )
{
   FTabs[ 0 ]->ButtonText = g_Config.GetDesktopString("Desktop", "Tab1", "期貨" );
   FTabs[ 1 ]->ButtonText = g_Config.GetDesktopString("Desktop", "Tab2", "自訂桌面1" );
   FTabs[ 2 ]->ButtonText = g_Config.GetDesktopString("Desktop", "Tab3", "自訂桌面2" );
   FTabs[ 3 ]->ButtonText = g_Config.GetDesktopString("Desktop", "Tab4", "自訂桌面3" );
   FTabs[ 4 ]->ButtonText = g_Config.GetDesktopString("Desktop", "Tab5", "自訂桌面4" );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SaveTabName( void )
{
   g_Config.SetDesktopString("Desktop", "Tab1", FTabs[ 0 ]->ButtonText );
   g_Config.SetDesktopString("Desktop", "Tab2", FTabs[ 1 ]->ButtonText );
   g_Config.SetDesktopString("Desktop", "Tab3", FTabs[ 2 ]->ButtonText );
   g_Config.SetDesktopString("Desktop", "Tab4", FTabs[ 3 ]->ButtonText );
   g_Config.SetDesktopString("Desktop", "Tab5", FTabs[ 4 ]->ButtonText );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LoadProperties( void )
{
   int  ActivePage  = g_Config.GetDesktopInteger( "SpeedyUnify.Main", "ActivePage", 0 );

   LoadHotkey();
   ContractViewerForm->LoadProperties( );
   ContractViewerForm->LoadHotkey( NULL );
   ContractViewerForm->LoadCustomNames();
   SetPage( ActivePage );
   UpdateAutoCancel();
   FUILoaded = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LoadMainFormPosition( void )
{
   int  FormLeft    = g_Config.GetDesktopInteger( "SpeedyUnify.Main", "Left",   Left );
   int  FormTop     = g_Config.GetDesktopInteger( "SpeedyUnify.Main", "Top",    Top );
   int  FormWidth   = g_Config.GetDesktopInteger( "SpeedyUnify.Main", "Width",  Width );
   int  FormHeight  = g_Config.GetDesktopInteger( "SpeedyUnify.Main", "Height", Height );
   bool ShowMenu    = g_Config.GetDesktopBool( "SpeedyUnify.Main", "Menu",  true );

   CancelWorkingForm->SetEnable( g_Config.GetDesktopBool( "SpeedyUnify.Main", "AutoCancel", false ) );
   CancelWorkingForm->SetTime( g_Config.GetDesktopInteger( "SpeedyUnify.Main", "AutoCancelTime", 85950 ));
   if( FormHeight > Screen->WorkAreaHeight )
	   FormHeight = Screen->WorkAreaHeight;
   if( FormWidth > Screen->WorkAreaWidth )
	   FormWidth = Screen->WorkAreaWidth;
   if( FormLeft + FormWidth/2 > Screen->DesktopWidth )
	   FormLeft = Screen->DesktopWidth - FormWidth/2;
   if( FormTop + FormHeight/2 > Screen->DesktopHeight )
	   FormTop = Screen->DesktopHeight - FormHeight/2;
   if( FormLeft != -1 && FormTop != -1 && FormWidth != -1 && FormHeight != -1 )
	   SetWindowPos( Handle, HWND_TOP, FormLeft, FormTop, FormWidth, FormHeight, 0 );
   SV->UseAnimation = false;
   if( ShowMenu == true )
	   SV->Open();
   else
	   SV->Close();
   SV->UseAnimation = true;
   LoadTabName();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SaveProperties( void )
{
	if( FUILoaded == true )
	{
	   g_Config.SetDesktopInteger( "SpeedyUnify.Main", "Left",   Left );
	   g_Config.SetDesktopInteger( "SpeedyUnify.Main", "Top",    Top );
	   g_Config.SetDesktopInteger( "SpeedyUnify.Main", "Width",  Width );
	   g_Config.SetDesktopInteger( "SpeedyUnify.Main", "Height", Height );
	   g_Config.SetDesktopInteger( "SpeedyUnify.Main", "ActivePage", PageControl->ActivePageIndex );
	   g_Config.SetDesktopBool( "SpeedyUnify.Main", "Menu",  SV->Opened );
	   ContractViewerForm->SaveProperties( );
	   if( OrderStore->IsLogon() == true )
	   {
		   if( HoldOpenInterestForm != NULL )
			   HoldOpenInterestForm->SaveProperties( );
		   if( GSimMatch == true )
			   LeaderBoardForm->SaveProperties();
	   }
	   SaveTabName();
	   SaveHotkey();
	   SaveWavesSetting();
	   if( g_Config.Save() == false)
		   TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"設定檔存檔失敗!" );
	}
}
//---------------------------------------------------------------------------
int  __fastcall TMainForm::KeyStringToIndex( const String& KeyStr )
{
	int KeyIndex = KeyCodeComboBox->Items->IndexOf(	KeyStr );
	if( KeyIndex == -1 )
		return 0;
	return KeyIndex;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LoadHotkey( void )
{
	BuyText->ButtonText               = KeyCodeComboBox->Items->Strings[g_Config.GetIntegerProperty("Setting","HKBuy", 0 )];
	SellText->ButtonText              = KeyCodeComboBox->Items->Strings[g_Config.GetIntegerProperty("Setting","HKSell", 0 )];
	BuyMarketText->ButtonText         = KeyCodeComboBox->Items->Strings[g_Config.GetIntegerProperty("Setting","HKBuyMkt", 0 )];
	SellMarketText->ButtonText        = KeyCodeComboBox->Items->Strings[g_Config.GetIntegerProperty("Setting","HKSellMkt", 0 )];
	IncBetterText->ButtonText         = KeyCodeComboBox->Items->Strings[g_Config.GetIntegerProperty("Setting","HKDeccBetter", 0 )];
	DecBetterText->ButtonText         = KeyCodeComboBox->Items->Strings[g_Config.GetIntegerProperty("Setting","HKIncBetter", 0 )];
	CancelAllText->ButtonText         = KeyCodeComboBox->Items->Strings[g_Config.GetIntegerProperty("Setting","HKDelAll", 0 )];
	CancelAllStopText->ButtonText     = KeyCodeComboBox->Items->Strings[g_Config.GetIntegerProperty("Setting","HKCancelAllStop", 0 )];
	CancelAllandCloseText->ButtonText = KeyCodeComboBox->Items->Strings[g_Config.GetIntegerProperty("Setting","HKCancelAllClose", 0 )];
	CenterPxStaticText->ButtonText    = KeyCodeComboBox->Items->Strings[g_Config.GetIntegerProperty("Setting","HKCenterFill", 0 )];
	NuclearBuyText->ButtonText        = KeyCodeComboBox->Items->Strings[g_Config.GetIntegerProperty("Setting","HKNuclearBuy", 0 )];
	NuclearSellText->ButtonText       = KeyCodeComboBox->Items->Strings[g_Config.GetIntegerProperty("Setting","HKNuclearSell", 0 )];
	CloseAllStepComboBox->ItemIndex  = g_Config.GetIntegerProperty("Setting","HKCloseStep", 0 );
	AskCloseAllForm->CloseAllStepComboBox->ItemIndex = CloseAllStepComboBox->ItemIndex;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SaveHotkey( void )
{
	g_Config.SetIntegerProperty("Setting","HKBuy", KeyStringToIndex(BuyText->ButtonText) );
	g_Config.SetIntegerProperty("Setting","HKSell", KeyStringToIndex(SellText->ButtonText) );
	g_Config.SetIntegerProperty("Setting","HKBuyMkt", KeyStringToIndex(BuyMarketText->ButtonText) );
	g_Config.SetIntegerProperty("Setting","HKSellMkt", KeyStringToIndex(SellMarketText->ButtonText) );
	g_Config.SetIntegerProperty("Setting","HKDeccBetter", KeyStringToIndex(IncBetterText->ButtonText) );
	g_Config.SetIntegerProperty("Setting","HKIncBetter", KeyStringToIndex(DecBetterText->ButtonText) );
	g_Config.SetIntegerProperty("Setting","HKDelAll", KeyStringToIndex(CancelAllText->ButtonText) );
	g_Config.SetIntegerProperty("Setting","HKCancelAllStop", KeyStringToIndex(CancelAllStopText->ButtonText) );
	g_Config.SetIntegerProperty("Setting","HKCancelAllClose", KeyStringToIndex(CancelAllandCloseText->ButtonText) );
	g_Config.SetIntegerProperty("Setting","HKCenterFill", KeyStringToIndex(CenterPxStaticText->ButtonText) );
	g_Config.SetIntegerProperty("Setting","HKNuclearBuy", KeyStringToIndex(NuclearBuyText->ButtonText) );
   	g_Config.SetIntegerProperty("Setting","HKNuclearSell", KeyStringToIndex(NuclearSellText->ButtonText) );
	g_Config.SetIntegerProperty("Setting","HKCloseStep", CloseAllStepComboBox->ItemIndex );

}
//---------------------------------------------------------------------------
void __fastcall TMainForm::NewOrdSoundButtonClick(TObject *Sender)
{
	AnsiString Sound = FSoundDir + NewOrdSoundComboBox->Text;

	gDSAudio.LoadBaseWave( "New", Sound.c_str() );
	GdAudioWave* Wave = gDSAudio.GetBaseWave( "New" );
	if( Wave != NULL  )
		Wave->Play();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::RejectSoundButtonClick(TObject *Sender)
{
	AnsiString Sound = FSoundDir + RejectSoundComboBox->Text;

	gDSAudio.LoadBaseWave( "Reject", Sound.c_str() );
	GdAudioWave* Wave = gDSAudio.GetBaseWave( "Reject" );
	if( Wave != NULL  )
		Wave->Play();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::StopSoundButtonClick(TObject *Sender)
{
	AnsiString Sound = FSoundDir + StopSoundComboBox->Text;

	gDSAudio.LoadBaseWave( "Stop", Sound.c_str() );
	GdAudioWave* Wave = gDSAudio.GetBaseWave( "Stop" );
	if( Wave != NULL  )
		Wave->Play();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::PxAlarmSoundButtonClick(TObject *Sender)
{
	AnsiString Sound = FSoundDir + PxAlarmSoundComboBox->Text;

	gDSAudio.LoadBaseWave( "PxAlarm", Sound.c_str() );
	GdAudioWave* Wave = gDSAudio.GetBaseWave( "PxAlarm" );
	if( Wave != NULL  )
		Wave->Play();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LowPxAlarmSoundButtonClick(TObject *Sender)
{
	AnsiString Sound = FSoundDir + LowPxAlarmSoundComboBox->Text;

	gDSAudio.LoadBaseWave( "LowPxAlarm", Sound.c_str() );
	GdAudioWave* Wave = gDSAudio.GetBaseWave( "LowPxAlarm" );
	if( Wave != NULL  )
		Wave->Play();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FillSoundButtonClick(TObject *Sender)
{
	if( FillSoundComboBox->Text == L"語音" )
	{
		Speak( L"台股期貨近月在9800成交1口" );
	}
	else
	{
		AnsiString Sound = FSoundDir + FillSoundComboBox->Text;

		gDSAudio.LoadBaseWave( "Fill", Sound.c_str() );
		GdAudioWave* Wave = gDSAudio.GetBaseWave( "Fill" );
		if( Wave != NULL  )
			Wave->Play();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OrderStoreNewOrder(TObject *Sender, const UnicodeString &Exchange,
		  const UnicodeString &Symbol, SideEnum Side, double Px,
		  int Qty, OrderTypeEnum OrderType, TimeInForceEnum TimeInForce, PositionEffectEnum PositionEffect,
		  bool &IsAccept)
{
	if( NewConfirmSwitch->Selected == true )
	{
		String Msg;

		if( Side == sBuy )
			Msg.printf( L"要買[%s] %d@%0.3f 嗎?", Symbol, Qty, Px );
		else
			Msg.printf( L"要賣[%s] %d@%0.3f 嗎?", Symbol, Qty, Px );
		IsAccept = TUnifyDlgs::AskYesNoDialog( L"新單確認", Msg );
	}
	else
		IsAccept = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::StopOrderChange( TObject* Sender, const String& Exchange, const String& StopSymbol, double StopPrice,
									const String& Symbol, double Price, int Qty,
									nsOrderMessageDefine::TimeInForceEnum TimeInForce,
									nsOrderMessageDefine::SideEnum Side,
									StopOrderStatusEnum StopStatus, bool& IsAccept)
{
	if( StopSoundSwitch->Selected == true && FRecovering == false )
	{
		GdAudioWave* Wave = gDSAudio.GetBaseWave( "Stop" );
		if( Wave != NULL  )
			Wave->Play();
	}
	IsAccept = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OrderStoreCancelByOrderID(TObject *Sender, const UnicodeString &OrderID,
		  bool &IsAccept)
{
	if( CancelConfirmSwitch->Selected == true )
	{
		String Msg;

		Msg.printf( L"要刪除委託單[%s]嗎?", OrderID );
		IsAccept = TUnifyDlgs::AskYesNoDialog( L"刪單確認", Msg );
	}
	else
		IsAccept = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OrderStoreRejectedReply(TObject *Sender)
{
	if( RejectSoundSwitch->Selected == true && FRecovering == false )
	{
		GdAudioWave* Wave = gDSAudio.GetBaseWave( "Reject" );
		if( Wave != NULL  )
			Wave->Play();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OrderStoreNewOrderReply(TObject *Sender, const UnicodeString &OrderID,
		  const UnicodeString &Symbol, SideEnum Side, double Px,
		  int Qty, OrderTypeEnum OrderType, TimeInForceEnum TimeInForce, PositionEffectEnum PositionEffect)

{
	if( NewSoundSwitch->Selected == true && FRecovering == false )
	{
		GdAudioWave* Wave = gDSAudio.GetBaseWave( "New" );
		if( Wave != NULL  )
			Wave->Play();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SubscribeFilled( const String &Exchange,const String &Symbol, IOrderFilled* Listener )
{
	if( Listener != NULL )
	{
		UFC::List<IOrderFilled*>* SubscribeList;
		String Key = Exchange + Symbol;
		UFC::PLockObject Lock( FFillSubLock );

		if( FFulledMap.GetObjectByKey( Key, SubscribeList ) == false )
		{
			SubscribeList = new UFC::List<IOrderFilled*>();
			FFulledMap.Add( Key, SubscribeList );
		}
		if( SubscribeList->IndexOf( Listener ) == -1 )
			SubscribeList->Add( Listener );
	}
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::UnsubscribeFilled( const String &Exchange,const String &Symbol, IOrderFilled* Listener )
{
	UFC::List<IOrderFilled*>* SubscribeList;
	String Key = Exchange + Symbol;
	UFC::PLockObject Lock( FFillSubLock );

	if( FFulledMap.GetObjectByKey( Key, SubscribeList ) == true )
	{
		int index = SubscribeList->IndexOf( Listener );
		if( index != -1 )
		{
			SubscribeList->Delete( index );
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
void __fastcall  TMainForm::TriggerFill( const String &Exchange, const String &Symbol, const String &OrderID, SideEnum Side, double Px,int Qty, OrderStatusEnum OrderStatus)
{
	UFC::List<IOrderFilled*>* SubscribeList;
	String Key = Exchange + Symbol;
	UFC::PLockObject Lock( FFillSubLock );

	if( FFulledMap.GetObjectByKey( Key, SubscribeList ) == true )
	{
		for( int i = 0; i < SubscribeList->ItemCount(); i ++ )
		{
			IOrderFilled* Listener = SubscribeList->GetItem( i );
			Listener->OnOrderFilled( Exchange, Symbol, OrderID, Side, Px, Qty, OrderStatus );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OrderStoreFilledReply(TObject *Sender,
		  const UnicodeString &Exchange,const UnicodeString &Symbol, const UnicodeString &OrderID,
		  SideEnum Side, double Px,int Qty, OrderStatusEnum OrderStatus)

{
	if( FRecovering == false )
		TriggerFill( Exchange, Symbol, OrderID, Side, Px, Qty, OrderStatus );
	if( FillSoundSwitch->Selected == true && FRecovering == false )
	{
		if( FillSoundComboBox->Text == L"語音" )
		{
			BasicInformation* SymInfo;
			String Msg;

			if( (SymInfo = gMarketDataStore->GetBasicInformation( Exchange, Symbol, false ))!= NULL )
			{
				if( SymInfo->GetMarket() ==  ::mTWFutures )
				{
					String ProductID( SymInfo->GetProductID().c_str() );
					String ChSym( gMarketDataStore->GetChinesePrefix( Exchange, ProductID ));
					String Date( SymInfo->GetMaturityDate().c_str() );

					if( Date.Length() == 6 )
					{
						String Year = Date.SubString( 1,4 );
						String Mon  = Date.SubString( 5,2 );
						Msg.printf( L"%s%s年%s月在%0.*f成交%d口", ChSym, Year, Mon,  SymInfo->GetDigit(), Px, Qty );
						Speak( Msg );
					}
				}
				else if( SymInfo->GetMarket() ==  ::mTWOptions )
				{

				}
				else if( SymInfo->GetMarket() ==  ::mTSE || SymInfo->GetMarket() ==  ::mOTC )
				{
					String Sym( SymInfo->GetChineseName().c_str() );

					Msg.printf( L"%s在%0.*f成交%d張",Sym, SymInfo->GetDigit(), Px, Qty );
					Speak( Msg );
				}
			}
		}
		else
		{
			GdAudioWave* Wave = gDSAudio.GetBaseWave( "Fill" );
			if( Wave != NULL  )
				Wave->Play();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OrderStoreBeginRecover(TObject *Sender)
{
	FRecovering = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OrderStoreRecoverFinished(TObject *Sender)
{
	if( FRecovering == true )  ///< Make sure recover once.
	{
		FRecovering = false;
		OrderStore->SubscribeAllPosStatisticRecSymbol( false, true );
		///< Load hold position
		TBrokerConfig*  BrokerCfg = g_Config.BrokerConfig( 0 );
		String          Msg;

		if( GSimMatch == false )
		{
			if( gUser.AccountType == hatBoth )
			{
				if( BrokerCfg->GetService()->GetPosition( true , OrderStore->Account, Msg ) == false )
					TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"中台期貨部位查詢失敗:" + Msg );
				if( BrokerCfg->GetService()->GetPosition( false , OrderStore->TWSEAccount, Msg ) == false )
					TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"中台證券部位查詢失敗:" + Msg );
			}
			else if( gUser.AccountType == hatTAIFEX )
			{
				if( BrokerCfg->GetService()->GetPosition( true , OrderStore->Account, Msg ) == false )
					TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"中台期貨部位查詢失敗:" + Msg );
			}
			else if( gUser.AccountType == hatTWSE )
			{
				if( BrokerCfg->GetService()->GetPosition( false , OrderStore->TWSEAccount, Msg ) == false )
					TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"中台證券部位查詢失敗:" + Msg );
			}
		}
		else
		{
			BrowserForm = new TBrowserForm( this );
			BrowserForm->Caption = L"模擬盤公告";
			BrowserForm->WebBrowser->Navigate( L"https://unify.data-bee.com/download/news_training_01.html" );
			BrowserForm->Width = 390;
			BrowserForm->Height = 595;
			SetWindowPos( BrowserForm->Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
			BrowserForm->WaitLoading( );
			delete BrowserForm;
		}
		LogonReadyTimer->Enabled = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OrderStoreFloatingProfit(TObject *Sender,
														const String& ExchangeCode, const String& Symbol,
														void *PositionStatisticRecordPtr)
{
	if (PositionStatisticRecordPtr != NULL )
	{
		double floatingProfit,closeProfit,tax,fee,netBalance;

		s888::CPositionStatisticRecord *posStatisticRecPtr = static_cast<s888::CPositionStatisticRecord*>(PositionStatisticRecordPtr);
		posStatisticRecPtr->LockForRead();
		floatingProfit = posStatisticRecPtr->GetFloatingProfit(s888::rwNotLock);
		closeProfit    = posStatisticRecPtr->GetCloseProfit(s888::rwNotLock);
		tax            = posStatisticRecPtr->GetTax(s888::rwNotLock);
		fee            = posStatisticRecPtr->GetFee(s888::rwNotLock);
		posStatisticRecPtr->UnlockForRead();
		FNetBalance = floatingProfit + closeProfit - tax - fee;
		///< Update Depth forms.
		ContractViewerForm->UpdateBalance( ExchangeCode, Symbol, FNetBalance);
		///< Update balance form.
		if( HoldOpenInterestForm != NULL)
			HoldOpenInterestForm->UpdateFloatingProfit(ExchangeCode, Symbol, posStatisticRecPtr);
	}
}
//---------------------------------------------------------------------------
int __fastcall TMainForm::CloseBetterTick( void )
{
	if( CloseAllStepComboBox->ItemIndex >= 0 )
		return CloseAllStepComboBox->ItemIndex;
	return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormResize(TObject *Sender)
{
	String MDRTTStr( L"行情品質" );
	int    TextW = FStatusBuffer->Canvas->TextWidth( MDRTTStr ) + 5;

	BackgroundPanel->SetBounds( 0, CAPTION_H, ClientWidth, ClientHeight - BOTTOM_H - CAPTION_H );
	RTTGraphRect = TRect( TextW, ClientHeight - BOTTOM_H + 1, TextW + RTT_WIDTH, ClientHeight - 1 );
	CAButton->Top = ClientHeight - CAButton->Height -1;
   	CxlWorkingButton->Top = CAButton->Top;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CloseButtonClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MaxButtonClick(TObject *Sender)
{
	if( WindowState == wsMaximized )
        WindowState = wsNormal;
	else
		WindowState = wsMaximized;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MinButtonClick(TObject *Sender)
{
	TrayIcon->Visible = true;
	TrayIcon->ShowBalloonHint();
	Hide();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Tab2ButtonMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y)
{
	TControl* Control = dynamic_cast<TControl*>( Sender );
	if( Button == mbRight && Control != NULL )
	{
		TPoint ScrPT = Control->ClientToScreen( TPoint( X,Y ) );
		TabPopupMenu->Tag = Control->Tag;
		TabPopupMenu->Popup( ScrPT.X, ScrPT.Y);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CMarketDataStoreMarketDataServerAck(int RTT)
{
	while( true )
	{
		if( FMDRTTDataList->Count <= RTT_WIDTH )
			break;
		FMDRTTDataList->Delete( 0 );
	}
	FMDRTT = RTT;
	FMDRTTDataList->Add( (void*) RTT );
	PaintMDRTT( Canvas, RTTGraphRect );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::PaintMDRTT( TCanvas* canvas, TRect& RTTGraph )
{
	if( FMDRTTDataList->Count > 0 )
	{
		int      YPos,XPos,PointCount = 1;
		float    Scale   = (float)RTTGraph.Height() / 250.0f;
		int      XOffset = RTTGraph.Right  - FMDRTTDataList->Count;
		int      YOffset = RTTGraph.Bottom;
		String   Msg;

		///< Setup Polygon points.
		FMDPolygonPoints[0].x = XOffset;
		FMDPolygonPoints[0].y = YOffset - Scale*(int)FMDRTTDataList->Items[ 0 ];
		for( register int i = 1; i < FMDRTTDataList->Count; i++ )
		{
			FMDPolygonPoints[PointCount].x = i + XOffset;
			FMDPolygonPoints[PointCount].y = YOffset - Scale*(int)FMDRTTDataList->Items[ i ];
			if( FMDPolygonPoints[PointCount].y < RTTGraph.Top )
				FMDPolygonPoints[PointCount].y = RTTGraph.Top;
			PointCount++;
		}
		FMDPolygonPoints[PointCount].x = RTTGraph.Right;
		FMDPolygonPoints[PointCount].y = RTTGraph.Bottom;
		PointCount++;
		FMDPolygonPoints[PointCount].x = XOffset;
		FMDPolygonPoints[PointCount].y = YOffset;
		PointCount++;
		///< Draw Black background rectangle.
		canvas->Brush->Color = clBlack;
		canvas->FillRect( RTTGraph );
		///< Draw Green line
		canvas->Pen->Color = clLime;
		YPos = RTTGraph.Height() - 60*Scale;
		canvas->MoveTo( RTTGraph.Left, YPos + RTTGraph.Top );
		canvas->LineTo( RTTGraph.Right, YPos + RTTGraph.Top );
		///< Draw Yellow line
		canvas->Pen->Color = clYellow;
		YPos = RTTGraphRect.Height() - 120*Scale;
		canvas->MoveTo( RTTGraph.Left, YPos + RTTGraph.Top );
		canvas->LineTo( RTTGraph.Right, YPos + RTTGraph.Top );
		///< Draw Red line
		canvas->Pen->Color = clRed;
		canvas->MoveTo( RTTGraph.Left, RTTGraph.Top );
		canvas->LineTo( RTTGraph.Right, RTTGraph.Top );
		///< Draw poly lines
		canvas->Pen->Color   = (TColor)RGB(0,255,0);
		canvas->Brush->Color = (TColor)RGB(0,98,0);
		canvas->Polygon( FMDPolygonPoints, PointCount - 1 );
		///< Draw text
		canvas->Font->Color  = clWhite;
		canvas->Brush->Style = bsClear;
		Msg.printf( L"%dms", FMDRTT );
		canvas->TextOut( RTTGraph.Left + 3, RTTGraph.Top + 1, Msg );
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::RTTTimerTimer(TObject *Sender)
{
	String Msg;

	if( HoldOpenInterestForm != NULL && GSimMatch == true )
		Msg.printf( L"%s|%s|%0.1f|%d|", gUser.UserID, gUser.Nickname, HoldOpenInterestForm->GetTotalProfit(), FMDRTT );
	else
		Msg.printf( L"%s|%s|%0.1f|%d|", gUser.UserID, gUser.Nickname, 0, FMDRTT );
	UTF8String UTF8Msg( Msg );
	AnsiString AnsiMsg( UTF8Msg );
	CMarketDataStore->SendMessageToServer( AnsiMsg.c_str(), 1 );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::RenameItemClick(TObject *Sender)
{
	String Name = FTabs[ TabPopupMenu->Tag ]->ButtonText;

	if( TUnifyDlgs::AskStringDialog( L"重新命名", Name ) == true )
	{
		FTabs[ TabPopupMenu->Tag ]->ButtonText = Name;
		SaveTabName();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WebBrowserNavigateComplete2(TObject *ASender, IDispatch * const pDisp,
		  const OleVariant &URL)
{
	if( FRequestWeb == true )
	{
		FRequestWeb = false;
		ShowTabs( false );
		if( SettingSV->Opened == true )
			SettingPanel( false );
		PageControl->ActivePage = WebTabSheet;
		Application->ProcessMessages();
		WebBrowser->SetFocus();
	}
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::Support( const UnifyProductKey& key, int& YYYYMMDD )
{
	UnifyProductInfo* Info;

	if( FPurchaseList.GetObjectByKey( key.schedule_id, Info ) == true ||
		FProductList.GetObjectByKey( key.project_id, Info ) == true )
	{
		YYYYMMDD = Info->YYYYMMDD;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ClearProductInfo( void )
{
	UnifyProductInfo* DelInfo = FPurchaseList.First();

	while( DelInfo != NULL)
	{
		delete DelInfo;
		DelInfo = FPurchaseList.Next();
	}
	FPurchaseList.Clear();
	DelInfo = FProductList.First();
	while( DelInfo != NULL)
	{
		delete DelInfo;
		DelInfo = FProductList.Next();
	}
	FProductList.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::EnableUnifyLicense( bool Enable )
{
	Tab3Button->Visible = true;
	Tab4Button->Visible = true;
	Tab5Button->Visible = true;
	PurchaseTimer->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::UseLastLicense( UFC::PHashMap<String,UnifyProductInfo*>& InfoMap, const String& ID, const String& ExpDate )
{
	UnifyProductInfo* NewInfo = new UnifyProductInfo( ID, ExpDate );
	UnifyProductInfo* Originfo;

	if( InfoMap.GetObjectByKey( ID, Originfo ) == true )
	{
		 if( NewInfo->YYYYMMDD > Originfo->YYYYMMDD )
		 {
			InfoMap.Add( ID, NewInfo );
			delete Originfo;
		 }
		 else
			delete NewInfo;
	}
	else
		InfoMap.Add( ID, NewInfo );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::PreventIdleTimerTimer(TObject *Sender)
{
	UFCType::Int32 Now = UFC::GetHHMMSS();

	SetThreadExecutionState( ES_DISPLAY_REQUIRED|ES_SYSTEM_REQUIRED );

	if( Now >= 140000 && Now <= 140500  && OrderStore != NULL && OrderStore->IsLogon() == true && OrderStore->HasStopOrder() == true )
		OrderStore->CancelAllStopOrder();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::QuiteButtonClick(TObject *Sender)
{
	if( KeyCodeComboBox->Visible == true )
		KeyCodeComboBox->Visible = false;
	SettingPanel( false );
	SV->Open();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::NewConfirmSwitchClick(TObject *Sender)
{
	TGraphButton* OnOffSwitch = dynamic_cast<TGraphButton*>( Sender );

	if( OnOffSwitch != NULL )
	{
		OnOffSwitch->Selected = ! OnOffSwitch->Selected;
		SaveWavesSetting( );
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LightRadioButtonClick(TObject *Sender)
{
	g_Config.SetBoolProperty( "Setting","Skin", LightRadioButton->Checked );
	DarkRadioButton->Checked = ! LightRadioButton->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CAButtonClick(TObject *Sender)
{
	if( GSimMatch == false ) ///< Production
	{
		CASettingForm = new TCASettingForm( this );

		CASettingForm->Caption = Mdcomponentstrings_MD_SpeedyUnify_AppName + L" 憑證設定";
		CASettingForm->IDLabel->Caption = L"身分證號:" + gUser.LoginUserID;
		CASettingForm->CAFileEdit->Text = FCAChecker->FPFXFile;
		CASettingForm->SavePasswordCheckBox->Checked = g_Config.GetBoolProperty( gUser.LoginUserID, "SavePwd", true );
		if( CASettingForm->SavePasswordCheckBox->Checked == true )
			CASettingForm->CAPasswordEdit->Text = FCAChecker->FPFXFilePassword;
		else
			CASettingForm->CAPasswordEdit->Text = L"";
		SetWindowPos( CASettingForm->Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
		if( CASettingForm->ShowModal() == mrOk )
		{
			String Reason;
			AnsiString CAFile( FCAChecker->FPFXFile );
			AnsiString CAPwd( FCAChecker->FPFXFilePassword );

			UFC::BufferedLog::Printf( " [TMainForm::CAButtonClick] CA File[%s] Password[%s]", CAFile.c_str(), CAPwd.c_str() );
			g_Config.SetStringProperty( gUser.LoginUserID, "CAFile", FCAChecker->FPFXFile );
			g_Config.SetBoolProperty( gUser.LoginUserID, "SavePwd", CASettingForm->SavePasswordCheckBox->Checked );
			g_Config.SetStringProperty( gUser.LoginUserID, "CAPassword", FCAChecker->FPFXFilePassword );
			if( FCAChecker->InitialCheckerAndTest( Reason  ) == false )
			{
				CAButton->Selected = false;
				CAButton->ButtonText = L"憑證讀取錯誤";
			}
			else
			{
				CAButton->Selected = true;
				CAButton->ButtonText = L"憑證讀取成功";
				g_Config.Save();
				CheckAgreement();
			}
		}
		delete CASettingForm;
	}
	else ///< Traning room.
	{
		CAButton->Selected = true;
		CAButton->ButtonText = L"憑證讀取成功";
		return;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::RestoreItemClick(TObject *Sender)
{
	Show();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CloseItemClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CMarketDataStoreLeaderBoard(TStringList *Board)
{
	for( int i = 0; i < Board->Count; i++ )
	{
		AnsiString RecordStr( Board->Strings[i] );
		LeaderBoardForm->SetTopN( i + 1, RecordStr );
	}
	LeaderBoardForm->Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormConstrainedResize(TObject *Sender, int &MinWidth, int &MinHeight,
		  int &MaxWidth, int &MaxHeight)
{
	MinWidth = 1024;
	MinHeight = 768;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::actInfoExecute(TObject *Sender)
{
	SystemInfo();
	SettingPageControl->ActivePage = TabSysInfo;
	SettingPanel( true );
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::InfoOKButtonClick(TObject *Sender)
{
	SettingPanel( false );
	SV->Open();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SystemInfo( void )
{
	OSVERSIONINFOEX osvi;
	MEMORYSTATUSEX  statex;
	SYSTEM_INFO     si;
	String          ErrMsg,Msg;
	LARGE_INTEGER   PinTime;
	double          GHz = ProcSpeedCalc();
	int             AppVer = g_Config.GetVersion();

	///< Unify Version
	Msg.printf( (Mdcomponentstrings_MD_SpeedyUnify_AppName + L" 版本: %d.%d.%d").c_str() ,AppVer/1000000, (AppVer%1000000)/10000, (AppVer%10000)/100);
	SUVerLabel->Caption = Msg;
	///< OS Version
	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if( GetVersionEx((OSVERSIONINFO *)&osvi) != 0 )
	{
			String OSStr;

			if(	osvi.dwMajorVersion == 4 ) ///< 4.X Windows NT
			{
				OSStr = L"Windows NT";
				OSLabel->Font->Color = clRed;
			}
			else if( osvi.dwMajorVersion == 5 )  ///< 5.X Windows XP/2000/2003
			{
				switch( osvi.dwMinorVersion )
				{
					case 0: OSStr = L"Windows 2000";break;
					case 1: OSStr = L"Windows XP";break;
					case 2: if( GetSystemMetrics(SM_SERVERR2) != 0 )
								OSStr = L"Windows Server 2003 R2";
							else
								OSStr = L"Windows Server 2003";
							break;
				}
				OSLabel->Font->Color = clRed;
			}
			else if( osvi.dwMajorVersion == 6 )  ///< 6.X  Vista/Windows 7/8/Windows Server 2008/2012
			{
				if( osvi.dwMinorVersion == 0 )
				switch( osvi.dwMinorVersion )
				{
					case 0:	if( osvi.wProductType == VER_NT_WORKSTATION )
								OSStr = L"Windows Vista";
							else
								OSStr = L"Windows Server 2008";
							break;
					case 1:	if( osvi.wProductType == VER_NT_WORKSTATION )
								OSStr = L"Windows 7";
							else
								OSStr = L"Windows Server 2008 R2";
							break;
					case 2: if( osvi.wProductType == VER_NT_WORKSTATION )
								OSStr = L"Windows 8";
							else
								OSStr = L"Windows Server 2012";
							break;
					case 3: if( osvi.wProductType == VER_NT_WORKSTATION )
								OSStr = L"Windows 8.1";
							else
								OSStr = L"Windows Server 2012 R2";
							break;
				}
				OSLabel->Font->Color = clBlack;
			}
			else if( osvi.dwMajorVersion == 10 )  ///< 10.X Windows 10/Windows Server 2016
			{
				if( osvi.dwMinorVersion == 0 )
				switch( osvi.dwMinorVersion )
				{
					case 0:	if( osvi.wProductType == VER_NT_WORKSTATION )
								OSStr = L"Windows 10";
							else
								OSStr = L"Windows Server 2016";
							break;
				}
				OSLabel->Font->Color = clBlack;
			}
			Msg.printf( L"%s 版本: %d.%d", OSStr, osvi.dwMajorVersion, osvi.dwMinorVersion );
			OSLabel->Caption = Msg;
	}
	else
	{
		OSLabel->Font->Color = clRed;
		OSLabel->Caption = L"Windows 版本: 無法取得相關資訊";
	}
	///< CPU Clock
	Msg.printf( L"CPU 時脈: %0.2fGHz", GHz );
	CPUClockLabel->Caption = Msg;
	if( GHz < 2.0 ) ///< less than 2.0 GHz memory
		CPUClockLabel->Font->Color = clRed;
	else if( GHz >= 2.0 && GHz < 3.0 )
		CPUClockLabel->Font->Color = clBlack;
	else
		CPUClockLabel->Font->Color = clGreen;
	///< Memory
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	int TotalMem = (int)((__int64)statex.ullTotalPhys/((__int64)1024*1024));
	int FreeMem  = (int)((__int64)statex.ullAvailPhys/((__int64)1024*1024));

	///< Total Memory
	Msg.printf( L"安裝記憶體: %d MB", TotalMem );
	MemLabel->Caption = Msg;
	if( TotalMem <= 1024 ) ///< less than 1G memory
		MemLabel->Font->Color = clRed;
	else if( FreeMem > 1024 && FreeMem <= 4096 )
		MemLabel->Font->Color = clBlack;
	else
		MemLabel->Font->Color = clGreen;
	///< Free Memory
	Msg.printf( L"可用記憶體: %d MB", FreeMem );
	FreeMemLabel->Caption = Msg;
	if( FreeMem <= 512 ) ///< less than 512M memory
		FreeMemLabel->Font->Color = clRed;
	else if( FreeMem > 512 && FreeMem <= 2048 )
		FreeMemLabel->Font->Color = clBlack;
	else
		FreeMemLabel->Font->Color = clGreen;
	///< MarketData Server IP
	Msg.printf( L"行情主機: %s ", CMarketDataStore->IP);
	MarketDataLabel->Caption = Msg;
	Msg.printf( L"線圖主機: %s ", ChartsStore->IP );
	ChartServerLabel->Caption = Msg;
	///< Order Server IP
	if( OrderStore->IsLogon() == true )
	{
		Msg.printf( L"交易主機: %s", OrderStore->IP );
		TradeServerLabel->Font->Color = clBlack;
	}
	else
	{
		Msg = L"交易主機: 尚未連接";
		TradeServerLabel->Font->Color = clRed;
	}
	TradeServerLabel->Caption = Msg;

	Msg.printf( L"對外IP: %s", gUser.ClientIP );
	GWIPLabel->Caption = Msg;
	///< Network
	if( FMDRTTDataList->Count < 10 )
	{
		Msg = L"網路延遲: 資料不足,請稍後再試";
		NetworkLabel->Font->Color = clRed;
	}
	else
	{
		int Total = 0;
		int Max = 0;
		int Min = 999;
		int Avg;
		String Note;

		for( int i =0;i< FMDRTTDataList->Count; i ++ )
		{
			int rtt = (int)FMDRTTDataList->Items[ i ];
			if( rtt > Max )
				Max = rtt;
			if( rtt < Min )
				Min = rtt;
			Total += rtt;
		}
		Avg = Total/FMDRTTDataList->Count;
		if( Avg < 20 )
		{
			if( Max > 50 )
				Note = L"延遲低,但不穩定";
			else
				Note = L"延遲低,穩定";
			NetworkLabel->Font->Color = clGreen;
		}
		else if( Avg < 50 )
		{
			if( Max > 128 )
				Note = L"品質普通,且不穩定";
			else
				Note = L"品質尚可";
			NetworkLabel->Font->Color = clBlack;
		}
		else
		{
			if( Max > 200 )
				Note = L"品質差,建議更換";
			else
				Note = L"品質差,請多注意";
			NetworkLabel->Font->Color = clRed;
		}
		Msg.printf( L"網路延遲: %d ms 最大: %d ms [%s]", Avg, Max, Note );
	}
	NetworkLabel->Caption = Msg;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::UpdateAccount( bool IsFut )
{
	if( OrderStore->IsLogon() == true )
	{
		if( GSimMatch == false )
		{
			TBrokerUser* User = FLoginBroker->GetService()->GetAccount();
			if( IsFut == true )
				FLoginUserStr.printf( L"期貨帳號:%s %s",OrderStore->Account, User->GetName() );
			else
				FLoginUserStr.printf( L"證券帳號:%s %s",OrderStore->TWSEAccount,User->GetName() );
		}
		else
			FLoginUserStr.printf( L"虛擬帳號:%s %s",OrderStore->Account, gUser.Nickname );
	}
	else
		FLoginUserStr = L"交易帳號:尚未登入";
	CAButton->Left = RTTGraphRect.Right + 10 + FStatusBuffer->Canvas->TextWidth( FLoginUserStr );
	CxlWorkingButton->Left = CAButton->Left + CAButton->Width + 10;
	PaintStatusBar( );
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y)
{
	int TextW = FStatusBuffer->Canvas->TextWidth(  L"行情品質" )  + RTT_WIDTH + 7;

	if( Y >= CAButton->Top && X <= CAButton->Left && X >= TextW )
	{
		if( gOrderStore->IsLogon() == true )
		{
			if( GSimMatch == false )
			{
				TBrokerUser* User = FLoginBroker->GetService()->GetAccount();
				DefAccForm = new TDefAccForm( this );
				for( int i = 0; i < User->FuturesAccountCount(); i++ )// -1; i >=0; i -- )
				{
					TAccountInfo* FutAcc = User->FuturesAccount( i );
					DefAccForm->FUTComboBox->Items->Add( FutAcc->Account );
				}
				for( int i = 0; i < User->StockAccountCount(); i++ )// -1; i >=0; i -- )
				{
					TAccountInfo* StocAcc = User->StockAccount( i );
					DefAccForm->TSEComboBox->Items->Add( StocAcc->Account );
				}
				DefAccForm->FUTComboBox->ItemIndex = 0;
				DefAccForm->TSEComboBox->ItemIndex = 0;
				if( DefAccForm->ShowModal() == mrOk )
				{
					FFutAccIndex = DefAccForm->FUTComboBox->ItemIndex;
					FTseAccIndex = DefAccForm->TSEComboBox->ItemIndex;
					g_Config.SetDesktopInteger( L"SpeedyUnify\\ExAccount", "TAIFEX", FFutAccIndex );
					g_Config.SetDesktopInteger( L"SpeedyUnify\\ExAccount", "TWSE", FTseAccIndex );
				}
				delete DefAccForm;
			}
		}
		else
			TUnifyDlgs::MessageDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"您尚未登入交易服務器" );
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::UpdateAutoCancel( void )
{
	if( CancelWorkingForm->GetEnable() == true )
	{
		UFCType::Int32 Now = UFC::GetHHMMSS();

		CxlWorkingButton->ButtonText = CancelWorkingForm->GetTimeString() + L" 委託全刪";
		if( Now < CancelWorkingForm->GetTime() )
		{
			CxlWorkingButton->Selected = true;
			CancelAllTimer->Enabled = true;
		}
		else
		{
			CxlWorkingButton->Selected = false;
			CancelAllTimer->Enabled = false;
		}
	}
	else
	{
		CxlWorkingButton->ButtonText = L"停用定時委託全刪";
		CxlWorkingButton->Selected = false;
		CancelAllTimer->Enabled = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CxlWorkingButtonClick(TObject *Sender)
{
	CancelWorkingForm->ShowModal();
	UpdateAutoCancel();
	g_Config.SetDesktopBool( "SpeedyUnify.Main", "AutoCancel", CancelWorkingForm->GetEnable() );
	g_Config.SetDesktopInteger( "SpeedyUnify.Main", "AutoCancelTime", CancelWorkingForm->GetTime() );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CancelAllTimerTimer(TObject *Sender)
{
	UFCType::Int32 Now = UFC::GetHHMMSS();

	if( CxlWorkingButton->Selected == true )
	{
		if( Now >= CancelWorkingForm->GetTime() )
		{
			CancelAllTimer->Enabled = false;
			CxlWorkingButton->Selected = false;
			/// Trigger cancel all working.
			if( OrderStore != NULL && OrderStore->IsLogon() == true )
				OrderStore->CancelAllOrder();
		}
	}
}
//---------------------------------------------------------------------------
int __fastcall TMainForm::GetPositionCount( void )
{
	if( FLoadPositionOK == true && gOrderStore->IsLogon() == true  )
		return HoldOpenInterestForm->FPList->Items->Count -1;
	return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::GetPositionSymbol( int index , String& Ex,String& Sym,String& Disp )
{
	TListItem *Item;
	BasicInformation* Info;
	s888::CPositionStatisticRecord *curPosStatisticRecPtr;

	if( gOrderStore->IsLogon() == true && FLoadPositionOK == true &&
		index >= 0 && index <= HoldOpenInterestForm->FPList->Items->Count -1 )
	{
		if( (Item = HoldOpenInterestForm->FPList->Items->Item[ index ]) != NULL &&
			(curPosStatisticRecPtr = static_cast<s888::CPositionStatisticRecord*>(Item->Data)) != NULL )
		{
			Ex  = curPosStatisticRecPtr->GetExchangeCode(s888::rwNotLock);
			Sym = curPosStatisticRecPtr->GetSymbol(s888::rwNotLock);
			if( (Info = gMarketDataStore->GetBasicInformation( Ex,Sym,false )) != NULL )
			{
				if( Info->GetMarket() == nsOrderMessageDefine::mTSE || Info->GetMarket() == nsOrderMessageDefine::mOTC )
					Disp.printf( L"%s(%s)",Info->GetDisplayName(),Sym );
				else
					Disp.printf( L"%s", Info->GetDisplayName());
			}
		}
	}
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::HasPosition( const String& Ex, const String& Sym )
{
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LogonReadyTimerTimer(TObject *Sender)
{
	LogonReadyTimer->Enabled = false;
	FLoadPositionOK = true;
	ContractViewerForm->OrderStoreReady();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LoadingProgress( int Percent,  const String& Msg )
{
	LoadingProgressBar->Position = Percent;
	LoadingStaticText->Caption = Msg;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::TabSheetLoadingResize(TObject *Sender)
{
	ProgressPanel->Left =  (TabSheetLoading->Width - 400 )/2;
	ProgressPanel->Top  =  (TabSheetLoading->Height - 320 )/2;
}
//---------------------------------------------------------------------------

