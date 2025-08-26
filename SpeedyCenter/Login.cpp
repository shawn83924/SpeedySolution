//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Login.h"
#include "IniFiles.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Subscriber"
#pragma link "IniEditBox"
#pragma resource "*.dfm"
extern AnsiString CurrentDir;
TLoginForm *LoginForm;
//---------------------------------------------------------------------------
extern bool   gAutoLogon;
extern String gID;
extern String gIP;
extern String gPassword;
//---------------------------------------------------------------------------
__fastcall TLoginForm::TLoginForm(TComponent* Owner)
    : TForm(Owner)
{
	if( gAutoLogon == false )
	{
		TIniFile* iniFile = new TIniFile( CurrentDir +"\\SpeedyClientCfg.ini");
		IDEdit->Text  = iniFile->ReadString( "UI Setting", "UserID","Admin");
		delete iniFile;
	}
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::FormShow(TObject *Sender)
{
	if( gAutoLogon == true )
	{
		IDEdit->Text = gID;
		PwdEdit->Text = gPassword;
		AutoLogonTimer->Enabled = true;
	}
	else
		PwdEdit->Text = L"";
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::OkBtnClick(TObject *Sender)
{
	TIniFile* iniFile = new TIniFile( CurrentDir +"\\SpeedyClientCfg.ini");
	try
	{
		iniFile->WriteString( "UI Setting", "UserID", IDEdit->Text );
	}
	catch(...){}
	delete iniFile;
}
//---------------------------------------------------------------------------

void __fastcall TLoginForm::AutoLogonTimerTimer(TObject *Sender)
{
	AutoLogonTimer->Enabled = false;
	gAutoLogon = false;
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------

