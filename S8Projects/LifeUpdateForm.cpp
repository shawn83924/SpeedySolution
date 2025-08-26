//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "LifeUpdateForm.h"
#include "main.h"
#include "FMTConfig.h"
#include "UnifyDlg.h"
//#include "MessageForm.h"
//#include "PMFNextStrings.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma link "GraphButton"
#pragma link "GraphPanel"
#pragma resource "*.dfm"
//------------------------------------------------------------------------------
extern Config g_Config;
extern String GCommand;
//------------------------------------------------------------------------------
TUpdateForm *UpdateForm;
//---------------------------------------------------------------------------
__fastcall TUpdateForm::TUpdateForm(TComponent* Owner)
:TForm(Owner)
,FUpdateItemCount( 0 )
,FUpdateOKMsg( L"更新完畢,重新啟動程式." )
,FUpdateFailMsg( L"無法連線到更新伺服器,程式終止." )
,FUpdateFileMsg( L"正在下載[%s]" )
{
    this->RoundFormEx->Lockbox = false;
}
//---------------------------------------------------------------------------
void __fastcall TUpdateForm::CancelButtonClick(TObject *Sender)
{
	g_Config.CancelUpdate();
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TUpdateForm::FormShow(TObject *Sender)
{
	UpdateTimer->Enabled = true;
}
//---------------------------------------------------------------------------
int __fastcall TUpdateForm::GetUpdateCount( void )
{
	FUpdateItemCount = g_Config.RetriveUpdateList();
	return FUpdateItemCount;
}
//---------------------------------------------------------------------------
bool  __fastcall  TUpdateForm::UpdateMainProgram( void )
{
	return g_Config.UpdateMainProgram( );
}
//---------------------------------------------------------------------------
void __fastcall TUpdateForm::UpdateTimerTimer(TObject *Sender)
{
	bool NeedRestart;
	UpdateTimer->Enabled = false;

	if( FUpdateItemCount > 0 ) ///< Files need update.
	{
		ProgressTimer->Enabled = true;
		if( g_Config.StartUpdate( NeedRestart ) )
		{
			ProgressTimer->Enabled = false;
			if( NeedRestart == true )
			{
				TUnifyDlgs::MessageDialog( L"線上更新",FUpdateOKMsg );
				// run agent program to restart...
				SHELLEXECUTEINFOW si;
				String CMD( GCommand );

				ZeroMemory( &si, sizeof(si) );
				si.cbSize       = sizeof(si);
				si.fMask        = SEE_MASK_NOCLOSEPROCESS;
				si.lpVerb       = L"open";
				si.lpFile       = L"RestartAgent.exe";
				si.lpParameters = CMD.c_str();
				si.nShow        = SW_SHOWDEFAULT;
				si.hInstApp     = HInstance;
				ShellExecuteExW(&si);

				Application->Terminate();
			}
			else
			{
				ProgressTimer->Enabled = false;
				ModalResult = mrOk;
			}
		}
		else
		{
			ProgressTimer->Enabled = false;
			ModalResult = mrCancel;
		}
	}
	else if( FUpdateItemCount < 0 ) ///< Connection failed.
	{
		 TUnifyDlgs::MessageDialog( L"線上更新",FUpdateFailMsg );
		 Application->Terminate();
	}
	else ///< All files updated.
	{
		ProgressTimer->Enabled = false;
		ModalResult = mrOk;
	}
}
//---------------------------------------------------------------------------
void __fastcall TUpdateForm::ProgressTimerTimer(TObject *Sender)
{
	int    TotalPos = g_Config.GetTotalUpdateProgress();
	int    ThisPos  = g_Config.GetCurrentUpdateProgress();
	String Msg;
	String UpdateFile( g_Config.GetCurrentUpdateFile().c_str() );

	TotalProgressBar->Position = TotalPos;
	Msg.printf( L"%d%", TotalPos );
	TotalLabel->Caption = Msg;

	CurrentProgressBar->Position = ThisPos;
	Msg.printf( FUpdateFileMsg.c_str(), UpdateFile.c_str() );
	CurrentLabel->Caption = Msg;
}
//---------------------------------------------------------------------------

