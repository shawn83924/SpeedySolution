//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Roomi.h"
#include "roome_client.h"
#include "FMTConfig.h"
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma resource "*.dfm"
TRoomiForm *RoomiForm = NULL;
extern TUnifyUser gUser;
//---------------------------------------------------------------------------
__fastcall TRoomiForm::TRoomiForm( TComponent* Owner)
:TForm(Owner)
{
	RoundFormEx->Lockbox = false;
}
//---------------------------------------------------------------------------
void __fastcall TRoomiForm::FormCreate(TObject *Sender)
{
	AnsiString Token( gUser.Token );
	Height = Screen->PrimaryMonitor->Height * 0.8;
	Width  = (int)((double)Height/4.0 * 2.4);
}
//---------------------------------------------------------------------------
void __fastcall TRoomiForm::FormPaint(TObject *Sender)
{
	Roome_Draw();
}
//---------------------------------------------------------------------------
void __fastcall TRoomiForm::FormDestroy(TObject *Sender)
{
	Roome_Deinit();
}
//---------------------------------------------------------------------------
void __fastcall TRoomiForm::WndProc(Messages::TMessage &Message)
{
	if(Roome_ProcessWndMessage(Handle, Message.Msg, Message.WParam, Message.LParam, true ) != 0)
	{
		TForm::WndProc(Message);
	}
}
//---------------------------------------------------------------------------
void __fastcall TRoomiForm::RedrawTimerTimer(TObject *Sender)
{
	Roome_Draw();
}
//---------------------------------------------------------------------------
void __fastcall TRoomiForm::FormActivate(TObject *Sender)
{
	RedrawTimer->Interval = 50;
}
//---------------------------------------------------------------------------
void __fastcall TRoomiForm::FormDeactivate(TObject *Sender)
{
	RedrawTimer->Interval = 200;
}
//---------------------------------------------------------------------------
void __fastcall TRoomiForm::FormHide(TObject *Sender)
{
	RedrawTimer->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TRoomiForm::FormShow(TObject *Sender)
{
	RedrawTimer->Enabled = true;
	Roome_Draw();
}
//---------------------------------------------------------------------------
void __fastcall TRoomiForm::LoadFromClipboard( void )
{
    Roome_SendClipboardImage();
}
//---------------------------------------------------------------------------
void __fastcall TRoomiForm::LoadProperties( void )
{
	Left = g_Config.GetDesktopInteger( L"SpeedyUnify\\RooMi", "Left", 10 );
	Top  = g_Config.GetDesktopInteger( L"SpeedyUnify\\RooMi", "Top", 10 );
}
//---------------------------------------------------------------------------
void __fastcall TRoomiForm::SaveProperties( void )
{
	g_Config.SetDesktopInteger( L"SpeedyUnify\\RooMi", "Left", Left );
	g_Config.SetDesktopInteger( L"SpeedyUnify\\RooMi", "Top", Top );
}
//---------------------------------------------------------------------------
