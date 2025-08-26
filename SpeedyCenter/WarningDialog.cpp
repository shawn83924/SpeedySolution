//---------------------------------------------------------------------------
#include <vcl.h>
#include <Mmsystem.h>
#pragma hdrstop
#include "WarningDialog.h"
#include "SettingForm.h"
#include "..\UFC\UFC.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundForm"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
TWarningForm *WarningForm;
//---------------------------------------------------------------------------
extern AnsiString CurrentDir;
//---------------------------------------------------------------------------
__fastcall TWarningForm::TWarningForm(TComponent* Owner)
	: TForm(Owner)
{
	UFC::AnsiString Today;

	UFC::CreateDir( "log" );
	UFC::GetTradeYYYMMDD( Today );
	FLogFileName.printf( "%s\\log\\SpeedyCenter_%s_event.txt", CurrentDir.c_str(), Today.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TWarningForm::FormShow(TObject *Sender)
{
	if( FormSetting->IsCenter())
        SetWindowPos( Handle,
                      HWND_TOPMOST,   ///< Top most
                      (Screen->WorkAreaWidth - 311)/2,  ///< X
                      (Screen->WorkAreaHeight - 174)/2, ///< Y
                      311, ///< Width
                      174, ///< Height
                      SWP_SHOWWINDOW
                    );
    else
        SetWindowPos( Handle,
                      HWND_TOPMOST,   ///< Top most
                      Screen->WorkAreaWidth - 311,  ///< X
                      Screen->WorkAreaHeight - 174, ///< Y
                      311, ///< Width
                      174, ///< Height
                      SWP_SHOWWINDOW
                    );
}
//---------------------------------------------------------------------------
void __fastcall TWarningForm::HideTimerTimer(TObject *Sender)
{
    if( FFadeCountDown <= 0 )
    {
        HideTimer->Enabled = false;
        Hide();
    }
    else
    {
        AlphaBlendValue = FFadeCountDown*10;
        FFadeCountDown--;
    }
}
//---------------------------------------------------------------------------
void __fastcall TWarningForm::ShowWarning( const String& Host, const String& Message, const String& File )
{
	AlphaBlendValue = 255;
	FFadeCountDown  = 25;
	PlaySoundW( File.c_str(), NULL, SND_ASYNC|SND_FILENAME );
	HostLabel->Caption = L"@" + Host;
	MsgLabel->Caption  = Message;
	Show();
}
//---------------------------------------------------------------------------
void __fastcall TWarningForm::LogError( const AnsiString& Host,
										const AnsiString& IP,
										const AnsiString& ECode,
										const AnsiString& Message )
{
	if( FormSetting->EventLog() == true )
	{
		UFC::AnsiString LogMsg,TimeNow,Today;

		UFC::GetTimeString( TimeNow, TRUE );
		UFC::GetYYYYMMDD(  Today, TRUE );
		LogMsg.Printf( "%s|%s|%s|%s|%s|%s\n", Today.c_str(),
											  TimeNow.c_str(),
											  Host.c_str(),
											  IP.c_str(),
											  ECode.c_str(),
											  Message.c_str() );
		UFC::FileStream64 Log( FLogFileName.c_str(), "a" , true );
		Log.Write( LogMsg.c_str(),LogMsg.Length() );
	}
}
//---------------------------------------------------------------------------
void __fastcall TWarningForm::LogError( const AnsiString& Host,const AnsiString& IP,const AnsiString& ECode,const String& Message )
{
	if( FormSetting->EventLog() == true )
	{
		AnsiString AnsiMsg( Message );
		UFC::AnsiString LogMsg,TimeNow,Today;

		UFC::GetTimeString( TimeNow, TRUE );
		UFC::GetYYYYMMDD(  Today, TRUE );
		LogMsg.Printf( "%s|%s|%s|%s|%s|%s\n", Today.c_str(),
											  TimeNow.c_str(),
											  Host.c_str(),
											  IP.c_str(),
											  ECode.c_str(),
											  AnsiMsg.c_str() );

		UFC::FileStream64 Log( FLogFileName.c_str(), "a" , true );
		Log.Write( LogMsg.c_str(),LogMsg.Length() );
	}
}
//---------------------------------------------------------------------------
void __fastcall TWarningForm::ImageClick(TObject *Sender)
{
    HideTimer->Enabled  = true;
    HideTimer->Interval = 20;
}
//---------------------------------------------------------------------------

