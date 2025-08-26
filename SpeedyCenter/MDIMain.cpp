//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "FormAbout.h"
#include "SettingForm.h"
#include "EventViewer.h"
#include "MDIMain.h"
#include "SpeedyServerForm.h"
#include "SimTFXMain.h"
#include "SCStrings.hpp"
#include "reinit.hpp"
#include <Registry.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "SysTrayIcon"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
TMDIForm *MDIForm;
//---------------------------------------------------------------------------
AnsiString CurrentDir = GetCurrentDir();
//---------------------------------------------------------------------------
bool   gAutoLogon = false;
String gID;
String gIP;
String gPassword;
//---------------------------------------------------------------------------
extern LCID FCurrentLCID;
//---------------------------------------------------------------------------
__fastcall TMDIForm::TMDIForm(TComponent* Owner)
:TForm(Owner)
,FSystemDefaultLCID( GetSystemDefaultLCID())
,FCloseAll( false )
{
	LoadLanguage( );
	if( LoadNewResourceModule( FCurrentLCID ) )
		ReinitializeForms();
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::LoadAutoLogon( void )
{
	TIniFile* iniFile = new TIniFile( CurrentDir +"\\SpeedyClientCfg.ini");
	if( iniFile->SectionExists( L"Auto Logon" ) == true )
	{
		if( iniFile->ValueExists( L"Auto Logon", L"ID" ) == true &&
			iniFile->ValueExists( L"Auto Logon", L"IP" ) == true &&
			iniFile->ValueExists( L"Auto Logon", L"Password" ) == true )
		{
			gID = iniFile->ReadString( L"Auto Logon", L"ID", L"Simon" );
			gIP = iniFile->ReadString( L"Auto Logon", L"IP", L"192.168.0.45" );
			gPassword = iniFile->ReadString( L"Auto Logon", L"Password", L"1qazse4" );
			gAutoLogon = true;
		}
		else
			gAutoLogon = false;
	}
	else
		gAutoLogon = false;
	delete iniFile;
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::LoadLanguage( void )
{
	TIniFile* iniFile = new TIniFile( CurrentDir +"\\SpeedyClientCfg.ini");

	if( iniFile->ValueExists( L"Languang", L"LCID" ) == true  )
	{
		FCurrentLCID = iniFile->ReadInteger( L"Languang", L"LCID", FSystemDefaultLCID );
	}
	else
	{
		TRegistry *Reg = new TRegistry();
		Reg->RootKey = HKEY_LOCAL_MACHINE;
		if( Reg->OpenKey( "Software\\MDBS\\SpeedyCenter" ,true ) == true )
		{
			try
			{
				if( Reg->ValueExists( L"Lang" ) == true )
				{
					String InstallLang = Reg->ReadString( L"Lang" );
					if( InstallLang.Compare( L"Chinese (Taiwan)" ) == 0 )
						Reg->WriteInteger( L"LCID", ChineseTraditional );
					else if( InstallLang.Compare( L"Chinese (PRC)" ) == 0 )
						Reg->WriteInteger( L"LCID", ChineseSimplified );
					else
						Reg->WriteInteger( L"LCID", EnglishUS );
					Reg->DeleteValue( L"Lang" );
				}
				if( Reg->ValueExists( L"LCID") )
					FCurrentLCID = Reg->ReadInteger( L"LCID" );
				else
					FCurrentLCID = FSystemDefaultLCID;
			}
			catch( ...)	{}
			Reg->CloseKey();
		}
		delete Reg;
	}
	delete iniFile;
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::SaveLanguage( void )
{
	TIniFile* iniFile = new TIniFile( CurrentDir +"\\SpeedyClientCfg.ini");

	iniFile->WriteInteger( L"Languang", L"LCID", FCurrentLCID );
	delete iniFile;
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::LoadPosition( void )
{
	TIniFile* iniFile = new TIniFile( CurrentDir +"\\SpeedyClientCfg.ini");

	if( iniFile->ValueExists( L"Position", L"Left" ) == true  )
		Left = iniFile->ReadInteger( L"Position", L"Left", 0 );
	if( iniFile->ValueExists( L"Position", L"Top" ) == true  )
		Top = iniFile->ReadInteger( L"Position", L"Top", 0 );
	if( iniFile->ValueExists( L"Position", L"Width" ) == true  )
		Width = iniFile->ReadInteger( L"Position", L"Width", 800 );
	if( iniFile->ValueExists( L"Position", L"Height" ) == true  )
		Height = iniFile->ReadInteger( L"Position", L"Height", 600 );
	if( Top > Screen->DesktopTop + Screen->DesktopHeight )
		Top = Screen->DesktopTop;
	if( Left > Screen->DesktopLeft + Screen->DesktopWidth )
		Left = Screen->DesktopLeft;
	delete iniFile;
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::SavePosition( void )
{
	TIniFile* iniFile = new TIniFile( CurrentDir +"\\SpeedyClientCfg.ini");

	iniFile->WriteInteger( L"Position", L"Left", Left );
	iniFile->WriteInteger( L"Position", L"Top", Top );
	iniFile->WriteInteger( L"Position", L"Width", Width );
	iniFile->WriteInteger( L"Position", L"Height", Height );
	delete iniFile;
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::LogonButtonClick(TObject *Sender)
{
	static bool ReEntry = false;

	if( ReEntry == false )
	{
		ReEntry = true;
		if( MDIChildCount > 0 && ActiveMDIChild != NULL )
		{
			TSimTFXForm* ActiveChild = dynamic_cast<TSimTFXForm*>(ActiveMDIChild);
			if( ActiveChild != NULL  )
			{
				if( ServerForm->ShowModal( ) == mrOk )
				{
					String IP,Host;

					if( ServerForm->PopServerNameIP( Host, IP ) )
						ActiveChild->Logon( IP );
				}
			}
		}
		ReEntry = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::LogoutButtonClick(TObject *Sender)
{
	if( ActiveMDIChild != NULL )
	{
		TSimTFXForm* ActiveChild = dynamic_cast<TSimTFXForm*>(ActiveMDIChild);
		if( ActiveChild != NULL )
			ActiveChild->Logoff( );
	}
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::CheckSystemToolButtonClick(TObject *Sender)
{
	if( ActiveMDIChild != NULL )
	{
		TSimTFXForm* ActiveChild = dynamic_cast<TSimTFXForm*>(ActiveMDIChild);
		if( ActiveChild != NULL )
			ActiveChild->GetSystemInfo();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::ChangePasswdButtonClick(TObject *Sender)
{
	if( ActiveMDIChild != NULL )
	{
		TSimTFXForm* ActiveChild = dynamic_cast<TSimTFXForm*>(ActiveMDIChild);
		if( ActiveChild != NULL )
			ActiveChild->ChangePassword( );
	}
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::UpdateCaption( void )
{
	String CaptionText;

	CaptionText.printf( L"Speedy Center(%d Forms)",	ServerTabSet->Tabs->Count );
	Caption = CaptionText;
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::NewFormToolButtonClick(TObject *Sender)
{
	AnsiString TabName;

	Application->ProcessMessages();
	TabName.printf( "Server-%d", MDIChildCount + 1 );
	TSimTFXForm* NewChild;
	NewChild = new TSimTFXForm( Application );
	NewChild->OnLogon = MDILogon;
	NewChild->OnLogonFailed = MDILogonFailed;
	NewChild->OnAdminLogon = MDIAdminLogon;
	NewChild->OnLogoff = MDILogoff;
	NewChild->OnMaxinum = MDIMaxinum;
	NewChild->OnMDIClosed =	MDIClose;
	NewChild->OnMDIActive = MDIActive;
	NewChild->OnMDITime   = MDITimer;

	ServerTabSet->Tabs->AddObject( TabName, NewChild );
	ServerTabSet->TabIndex = ServerTabSet->Tabs->IndexOf( TabName );
	if( MDIChildCount > 1 && ActiveMDIChild->WindowState != wsMaximized )
	{
		NewChild->Left = 0;
		NewChild->Top =0 ;
	}
	else
	{
		NewChild->WindowState = wsMaximized;
		MDIMaxinum( NewChild );
	}
	NewChild->BringToFront();
	MDIActive( NewChild );
	ServerTabSet->Invalidate();
	UpdateCaption( );
	Application->ProcessMessages();
    if( Sender == NULL )
	{
		String Name,IP;

		if( ServerForm->PopServerNameIP( Name, IP ) == true )
			NewChild->Logon( Name, IP );
	}
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::SysTrayIconDoubleClick(TObject *Sender)
{
    if( SysTrayIcon->ShowIcon == true )
        SysTrayIcon->ShowIcon = false;
    if( SysTrayIcon->Enabled == true )
		SysTrayIcon->Enabled = false;
	ShowWindow( Application->Handle, SW_SHOW );
	Application->Restore();
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	SaveLanguage();
	SavePosition();
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::FormShow(TObject *Sender)
{
	LoadPosition();
	LoadAutoLogon();
	if( SysTrayIcon->ShowIcon == true )
		SysTrayIcon->ShowIcon = false;
	if( SysTrayIcon->Enabled == true )
		SysTrayIcon->Enabled = false;
	Constraints->MaxWidth = Screen->Width*2;
	Constraints->MaxHeight = Screen->Height*2;
	FormShowTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::AboutToolButtonClick(TObject *Sender)
{
	AboutForm = new TAboutForm( this );
	AboutForm->ShowModal();
	delete AboutForm;
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::SettingButtonClick(TObject *Sender)
{
	if( FormSetting->ShowModal() == mrOk )
	{
		LCID ChangeToLanguage = FormSetting->GetLanguage();

		if( FCurrentLCID != ChangeToLanguage )
		{
			if( FCurrentLCID == ChineseTraditional )
				ShowMessage( L"重新開啟程式後完成語言切換" );
			else if( FCurrentLCID == ChineseSimplified )
				ShowMessage( L"重新开启程序后完成语言切换" );
			else
				ShowMessage( L"Switch the language after restarting the program" );
			FCurrentLCID = ChangeToLanguage;
			SaveLanguage( );
			Application->Terminate();

			/*if( LoadNewResourceModule( ChangeToLanguage ) )
			{
				Visible = false;
				SaveLanguage( );
				FCurrentLCID = ChangeToLanguage;
				ReinitializeForms();
				for( int i =0; i < ServerTabSet->Tabs->Count; i++ )
				{
					TSimTFXForm* RestoreChild = dynamic_cast<TSimTFXForm*>(ServerTabSet->Tabs->Objects[i]);
					if( RestoreChild != NULL )
						RestoreChild->RestoreLogonState();
				}
				Visible = true;
			} */
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::MDIMaxinum(TObject *Sender)
{
	if( ActiveMDIChild == Sender )
	{                                
		int TabIndex = ServerTabSet->Tabs->IndexOfObject( Sender );
		ServerTabSet->Visible = true;
		if( TabIndex != -1 )
		{
			ServerTabSet->TabIndex = TabIndex;
			ServerTabSet->Invalidate();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::MDILogonFailed(TObject *Sender)
{
	while(	ServerForm->GetServerCount() > 0 )
	{
		String IP,Host;

		ServerForm->PopServerNameIP( Host, IP );
	};
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::MDILogon(TObject *Sender)
{
	if( ActiveMDIChild == Sender )
	{
		LogonButton->Enabled      = false;
		LogoutButton->Enabled     = true;
		ChangePasswdButton->Enabled = true;
		int TabIndex = ServerTabSet->Tabs->IndexOfObject( Sender );
		TSimTFXForm* ServerPanel = dynamic_cast<TSimTFXForm*>(ActiveMDIChild);

		if( TabIndex != -1 && ServerPanel != NULL )
		{
			ServerTabSet->Tabs->Strings[ TabIndex ] = ActiveMDIChild->Caption;
			ServerTabSet->Invalidate();
			if( ServerPanel->IsAdmin == false )
			{
				AnsiString ActiveServerName = ActiveMDIChild->Caption;

				ServerForm->AddServer( ServerPanel->GetOSIndex(), ActiveServerName, ServerPanel->GetIP() );
				ServerForm->SaveServers();
			}
			if(	ServerForm->GetServerCount() > 0 )
				NewFormToolButtonClick( NULL );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::MDIAdminLogon(TObject *Sender)
{
	if( ActiveMDIChild == Sender )
	{
		LogonButton->Enabled      = false;
		LogoutButton->Enabled     = true;
		ChangePasswdButton->Enabled = true;
		int TabIndex = ServerTabSet->Tabs->IndexOfObject( Sender );
		TSimTFXForm* ServerPanel = dynamic_cast<TSimTFXForm*>(ActiveMDIChild);

		if( TabIndex != -1 && ServerPanel != NULL )
		{
			ServerTabSet->Tabs->Strings[ TabIndex ] = ActiveMDIChild->Caption;
			ServerTabSet->Invalidate();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::MDILogoff(TObject *Sender)
{
	if( ActiveMDIChild == Sender )
	{
		LogonButton->Enabled      = true;
		LogoutButton->Enabled     = false;
		ChangePasswdButton->Enabled = false;
		int TabIndex = ServerTabSet->Tabs->IndexOfObject( Sender );

		if( TabIndex != -1 )
		{
			ServerTabSet->Tabs->Strings[ TabIndex ] = "Server-" + AnsiString( TabIndex + 1 );
			ServerTabSet->Invalidate();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::MDIClose(TObject *Sender)
{
	int TabIndex = ServerTabSet->Tabs->IndexOfObject( Sender );
	if( TabIndex != -1 )
	{
		ServerTabSet->Tabs->Delete( TabIndex );
		ServerTabSet->Invalidate();
		if( ServerTabSet->Tabs->Count == 0 )
		{
			LogonButton->Enabled        = false;
			LogoutButton->Enabled       = false;
			ChangePasswdButton->Enabled = false;
		}
	}
	Application->ProcessMessages();
	UpdateCaption( );
	if( FCloseAll == true )
	{
		if( MDIChildCount == 1 )
			StopCloseAll();
		if( MDIChildCount > 0 )
			CloseMenuItemClick( NULL );
	}
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::MDIActive(TObject *Sender)
{
	TSimTFXForm* ServerPanel = dynamic_cast<TSimTFXForm*>( Sender );

	if( ServerPanel != NULL )
	{
		if( ServerPanel->IsLogOn() == true )
		{
			LogonButton->Enabled      = false;
			LogoutButton->Enabled     = true;
			ChangePasswdButton->Enabled = true;
		}
		else
		{
			if(	ServerForm->GetServerCount() <= 0  )
				LogonButtonClick( Sender );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::MDITimer(TObject *Sender)
{
	if( ActiveMDIChild == Sender )
	{
		TSimTFXForm* ActiveChild = dynamic_cast<TSimTFXForm*>(ActiveMDIChild);
		ClockButton->Caption = ActiveChild->GetTime();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::ServerTabSetChange(TObject *Sender, int NewTab,
	  bool &AllowChange)
{
	TSimTFXForm* ActiveChild = dynamic_cast<TSimTFXForm*>(ServerTabSet->Tabs->Objects[ NewTab ]);
	if( ActiveChild != NULL )
	{
		ActiveChild->BringToFront();
		if( ActiveChild->IsLogOn( ) == true )
			MDILogon( ActiveChild );
		else
			MDILogoff( ActiveChild );
	}
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::ServerTabSetGetImageIndex(TObject *Sender,
	  int TabIndex, int &ImageIndex)
{
	TSimTFXForm* ActiveChild = dynamic_cast<TSimTFXForm*>(ServerTabSet->Tabs->Objects[ TabIndex ]);
	if( ActiveChild != NULL )
		ImageIndex = ActiveChild->GetOSIndex();
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::CascadeButtonClick(TObject *Sender)
{
	ServerTabSet->Visible = false;
	Cascade();
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::TileToolButtonClick(TObject *Sender)
{
	ServerTabSet->Visible = false;
	Tile();
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::ApplicationEventsMinimize(TObject *Sender)
{
	Application->Minimize();
	ShowWindow( Application->Handle, SW_HIDE );
	SysTrayIcon->IconHint = Caption;
	SysTrayIcon->ShowIcon = true;
	SysTrayIcon->Enabled  = true;
	SysTrayIcon->ShowToolTips( Scstrings_MINIMIZE_CAPTION,//"SpeedyCenter最小化",
							   Scstrings_MINIMIZE_HINT,//"SpeedyCenter最小化在System Tray中,\n雙擊Tray Icon可以還原.",
							   itInfo, 5000 );
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::EventLogToolButtonClick(TObject *Sender)
{
	EventForm->Show();
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	if( this->MDIChildCount == 0 )
    	CanClose = true;
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::ServerTabSetMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( Button == mbRight )
	{
		TPoint MousePt( X, Y );
		int    SelTab = ServerTabSet->ItemAtPos( MousePt );
		if( SelTab != -1  )
		{
			if( SelTab != ServerTabSet->TabIndex )
				ServerTabSet->TabIndex = SelTab;
			TPoint Pt = ServerTabSet->ClientToScreen( MousePt );
			TabPopupMenu->Popup( Pt.x, Pt.y );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::CloseMenuItemClick(TObject *Sender)
{
	int Index = ServerTabSet->TabIndex;
	if( Index != -1 )
	{
		TSimTFXForm* ActiveChild = dynamic_cast<TSimTFXForm*>(ServerTabSet->Tabs->Objects[ Index ]);
		if( ActiveChild != NULL )
			ActiveChild->Close();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::FormShowTimerTimer(TObject *Sender)
{
	FormShowTimer->Enabled = false;
	if( this->MDIChildCount == 0 )
		NewFormToolButtonClick( this );
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::CloseAllMenuItemClick(TObject *Sender)
{
	FCloseAll = true;
	NewWinButton->Enabled = false;
	CloseMenuItemClick( NULL );
}
//---------------------------------------------------------------------------
void __fastcall TMDIForm::StopCloseAll( void )
{
	FCloseAll = false;
	NewWinButton->Enabled = true;
}
//---------------------------------------------------------------------------


