//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "WebBrowserForm.h"
#include "UFC.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma resource "*.dfm"
TBrowserForm *BrowserForm;
//---------------------------------------------------------------------------
__fastcall TBrowserForm::TBrowserForm(TComponent* Owner)
	: TForm(Owner)
{
	RoundFormEx->Lockbox = false;
}
//---------------------------------------------------------------------------
void __fastcall TBrowserForm::WebBrowserNavigateComplete2(TObject *ASender, IDispatch * const pDisp,
          const OleVariant &URL)
{
	 FLoading = false;
}
//---------------------------------------------------------------------------
void __fastcall TBrowserForm::WebBrowserBeforeNavigate2(TObject *ASender, IDispatch * const pDisp,
		  const OleVariant &URL, const OleVariant &Flags, const OleVariant &TargetFrameName,
		  const OleVariant &PostData, const OleVariant &Headers,
		  WordBool &Cancel)
{
	FLoading = true;
	AnsiString NavURL = URL;

	if( NavURL.Pos( L"successAP.do" ) != 0 )
	{
		if(  NavURL.Pos( L"action=1" ) != 0 )
		{
			LoginID = AnsiString( NavURL.c_str() + NavURL.Pos( L"mobile") + 6 );
			ModalResult = mrOk;
		}
		else if(  NavURL.Pos( L"action=0" ) != 0 )
		{
			LoginID = L"";
			ModalResult = mrCancel;
		}
	}
	else if( NavURL.Pos( L"Close.html" ) != 0 )
	{
		ModalResult = mrOk;
    }

}
//---------------------------------------------------------------------------
bool __fastcall TBrowserForm::WaitLoading( void )
{
	Screen->Cursor = crHourGlass;
	for( int i= 0;i< 200; i++ )
	{
		if( FLoading == false )
		{
			Screen->Cursor = crDefault;
			return true;
		}
		Application->ProcessMessages();
		UFC::SleepMS( 20 );
	}
	Screen->Cursor = crDefault;
	return false;
}
//---------------------------------------------------------------------------

