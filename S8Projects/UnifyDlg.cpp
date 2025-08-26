//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnifyDlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma resource "*.dfm"
TUnifyDlgs *UnifyDlgs;
//---------------------------------------------------------------------------
__fastcall TUnifyDlgs::TUnifyDlgs(TComponent* Owner)
	: TForm(Owner)
{
	RoundFormEx->Lockbox = false;
}
//---------------------------------------------------------------------------
bool __fastcall TUnifyDlgs::AskStringDialog( const String& text, String& Name )
{
	TUnifyDlgs* MsgDlg = new TUnifyDlgs( NULL );
	MsgDlg->Caption = text;
	MsgDlg->MsgLabel->Visible = false;
	MsgDlg->NameEdit->Visible = true;
	MsgDlg->NameEdit->Text = Name;
	MsgDlg->OKButton->Visible = true;
	MsgDlg->OKButton->Left = (MsgDlg->ClientWidth - MsgDlg->OKButton->Width - MsgDlg->OKButton->Width - 15 )/2;
	MsgDlg->CancelButton->Visible = true;
	MsgDlg->CancelButton->Left = MsgDlg->OKButton->Left + MsgDlg->OKButton->Width +  15;
	if( MsgDlg->ShowModal() == mrOk )
	{
	   Name = MsgDlg->NameEdit->Text;
	   delete MsgDlg;
	   return true;
	}
	delete MsgDlg;
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TUnifyDlgs::MessageDialog( const String& caption, const String& text )
{
	TUnifyDlgs* MsgDlg = new TUnifyDlgs( NULL );
	int MinW = MsgDlg->Canvas->TextWidth( text ) + 30;

	if( MsgDlg->Width < MinW )
		MsgDlg->Width = MinW;
	MsgDlg->Height = 110;
	MsgDlg->Caption = caption;
	MsgDlg->MsgLabel->Caption = text;
	MsgDlg->MsgLabel->Visible = true;
	MsgDlg->NameEdit->Visible = false;
	MsgDlg->OKButton->Visible = true;
	MsgDlg->OKButton->Left = (MsgDlg->ClientWidth - MsgDlg->OKButton->Width)/2;
	MsgDlg->CancelButton->Visible = false;
	MsgDlg->ShowModal();
	delete MsgDlg;
}
//---------------------------------------------------------------------------
bool __fastcall TUnifyDlgs::AskYesNoDialog( const String& caption, const String& text )
{
	TUnifyDlgs* MsgDlg = new TUnifyDlgs( NULL );
	int MinW = MsgDlg->Canvas->TextWidth( text ) + 30;

	if( MsgDlg->Width < MinW )
		MsgDlg->Width = MinW;
	MsgDlg->Caption = caption;
	MsgDlg->MsgLabel->Caption = text;
	MsgDlg->MsgLabel->Visible = true;
	MsgDlg->NameEdit->Visible = false;
	MsgDlg->OKButton->Visible = true;
	MsgDlg->OKButton->Left = (MsgDlg->ClientWidth - MsgDlg->OKButton->Width - MsgDlg->OKButton->Width - 15 )/2;
	MsgDlg->CancelButton->Visible = true;
	MsgDlg->CancelButton->Left = MsgDlg->OKButton->Left + MsgDlg->OKButton->Width + 15;
	if( MsgDlg->ShowModal() == mrOk )
	{
		delete MsgDlg;
		return true;
	}
	delete MsgDlg;
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TUnifyDlgs::NameEditKeyPress(TObject *Sender, System::WideChar &Key)
{
	if( Key == VK_RETURN )
		this->ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TUnifyDlgs::OpenBrowser( const String& URL )
{
	SHELLEXECUTEINFOW si;
	/// Fill SHELLEXECUTEINFO structure for ShellExecute Win API.
	ZeroMemory( &si, sizeof(si) );
	si.cbSize   = sizeof(si);
	si.lpVerb   = L"open";
	si.lpFile   = URL.c_str();
	si.hInstApp = HInstance;
	si.nShow    = SW_SHOWDEFAULT;
	si.fMask    = SEE_MASK_NOCLOSEPROCESS;
	ShellExecuteExW( &si );
}
//---------------------------------------------------------------------------
void __fastcall TUnifyDlgs::FormCreate(TObject *Sender)
{
   Application->NormalizeTopMosts();
   SetWindowPos( Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
}
//---------------------------------------------------------------------------

