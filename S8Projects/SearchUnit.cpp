//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SearchUnit.h"
#include "ContractViewer.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "GraphButton"
#pragma resource "*.dfm"
TSearchSymbolForm *SearchSymbolForm;
//---------------------------------------------------------------------------
__fastcall TSearchSymbolForm::TSearchSymbolForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TSearchSymbolForm::WndProc( TMessage &Msg )
{
	switch( Msg.Msg )
	{
		case WM_ERASEBKGND: PaintBk( Msg );
							return;
	}
	inherited::WndProc( Msg );
}
//---------------------------------------------------------------------------
void __fastcall TSearchSymbolForm::PaintBk( TMessage &Msg )
{
	TRect SearchRect = SearchEditBox->BoundsRect;

	///< Deaw Caption
	TRect DrawRect = TRect( 0,0, Width, Height );
	Canvas->Pen->Color = TColor( 0x00211B16 );
	Canvas->Brush->Color = TColor( 0x00211B16 );
	Canvas->FillRect( DrawRect );
	///< Draw Search box
	Canvas->Pen->Color = TColor( 0x00402B1C );
	Canvas->Brush->Color = TColor( 0x00402B1C );
	Canvas->FillRect( SearchRect );
	Canvas->Ellipse( SearchRect.Left  - 15, SearchRect.Top, SearchRect.Left  + 15 ,SearchRect.Bottom );
	Canvas->Ellipse( SearchRect.Right +SearchButton->Width  - 15, SearchRect.Top, SearchRect.Right+SearchButton->Width + 15 ,SearchRect.Bottom );
	Msg.Result = 1;
}
//---------------------------------------------------------------------------
void __fastcall TSearchSymbolForm::SearchEditBoxKeyDown(TObject *Sender, WORD &Key,
		  TShiftState Shift)
{
	if(	SearchEditBox->Text == L"搜尋商品" )
        SearchEditBox->Text = L"";
	if( Key == VK_RETURN )
		SearchButtonClick( this );
}
//---------------------------------------------------------------------------
void __fastcall TSearchSymbolForm::SearchEditBoxMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y)
{
	SearchEditBox->Text = L"";
}
//---------------------------------------------------------------------------
void __fastcall TSearchSymbolForm::SearchButtonClick(TObject *Sender)
{
	ContractViewerForm->SearchEdit->Text = SearchEditBox->Text;
	ContractViewerForm->SearchGlyphButtonClick( NULL );
	Application->NormalizeTopMosts();
	SetWindowPos( ContractViewerForm->Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
	ContractViewerForm->Show();
	ContractViewerForm->Left = Left;
	ContractViewerForm->Top  = Top + SearchEditBox->Top + SearchEditBox->Height;
	SearchEditBox->Text = L"搜尋商品";
}
//---------------------------------------------------------------------------
void __fastcall TSearchSymbolForm::FormDeactivate(TObject *Sender)
{
	Hide();
}
//---------------------------------------------------------------------------
void __fastcall TSearchSymbolForm::FormShow(TObject *Sender)
{
	SearchEditBox->SetFocus();
}
//---------------------------------------------------------------------------

