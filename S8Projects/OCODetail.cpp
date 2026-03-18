//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OCODetail.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "OCODataList"
#pragma link "RoundFormEx"
#pragma resource "*.dfm"
TOCODetailForm *OCODetailForm;
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
void __fastcall TOCODetailForm::SetData(void)
{
    OCODataList->TestFunctionForAddData();
}
//---------------------------------------------------------------------------

