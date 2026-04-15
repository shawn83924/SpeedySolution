//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OCODetail.h"
#include "ContractViewer.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "OCODataList"
#pragma link "RoundFormEx"
#pragma resource "*.dfm"
TOCODetailForm *OCODetailForm;
//---------------------------------------------------------------------------
extern TOrderStore_OCO*   gOrderStore_OCO;
//---------------------------------------------------------------------------
__fastcall TOCODetailForm::TOCODetailForm(TComponent* Owner)
	: TForm(Owner)
{
	this->DefaultMonitor = dmDesktop;
	RoundFormEx->Lockbox = false;
}
//---------------------------------------------------------------------------
void __fastcall TOCODetailForm::Release(void)
{

}
//---------------------------------------------------------------------------
void __fastcall TOCODetailForm::SetVisible( bool IsVisible)
{
	if( IsVisible == false )
		Visible = false;
	else
		Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TOCODetailForm::FormCreate(TObject *Sender)
{
    Position = poMainFormCenter;
}
//---------------------------------------------------------------------------

void __fastcall TOCODetailForm::FormShow(TObject *Sender)
{
	OCODataList->SubscribeStore(gOrderStore_OCO);
}
//---------------------------------------------------------------------------
void __fastcall TOCODetailForm::WndProc( TMessage &Msg )
{
	switch( Msg.Msg )
	{
		case WM_ERASEBKGND:
			 Msg.Result = 1;
			 return;
		case WM_MOUSEACTIVATE:
			 BringToFront();
			 ContractViewerForm->ActiveClient( this );
			 Msg.Result = MA_ACTIVATE;
			 return;
	}
	inherited::WndProc( Msg );
}
//---------------------------------------------------------------------------
