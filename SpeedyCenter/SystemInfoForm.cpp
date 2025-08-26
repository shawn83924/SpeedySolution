//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SystemInfoForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSystemForm *SystemForm;
//---------------------------------------------------------------------------
__fastcall TSystemForm::TSystemForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TSystemForm::InfoListBoxDrawItem(TWinControl *Control, int Index,
		  TRect &Rect, TOwnerDrawState State)
{
	String Text = InfoListBox->Items->Strings[Index];
	if( Text[4] == ',' )
	{
		String Type = Text.SubString(0,3);

		if( Type == "war" )
		{
			InfoListBox->Canvas->Brush->Color = clYellow;
			InfoListBox->Canvas->FillRect( Rect );
			InfoListBox->Canvas->Font->Color = clNavy;
			StatImageList->Draw( InfoListBox->Canvas, Rect.left , Rect.top, 1 );
		}
		else if( Type == "err" )
		{
			InfoListBox->Canvas->Brush->Color = clRed;
			InfoListBox->Canvas->FillRect( Rect );
			InfoListBox->Canvas->Font->Color = clWhite;
			StatImageList->Draw( InfoListBox->Canvas, Rect.left , Rect.top, 2 );
		}
		else
		{
			InfoListBox->Canvas->Brush->Color = clWhite;
			InfoListBox->Canvas->FillRect( Rect );
			InfoListBox->Canvas->Font->Color = clBlack;
			StatImageList->Draw( InfoListBox->Canvas, Rect.left , Rect.top, 0 );
		}
		InfoListBox->Canvas->TextOut( Rect.left + 25,Rect.top + 2, Text.c_str() + 4 );
	}
	else
	{
		InfoListBox->Canvas->Brush->Color = clNavy;
		InfoListBox->Canvas->FillRect( Rect );
		InfoListBox->Canvas->Font->Color = clYellow;
		InfoListBox->Canvas->TextOut( Rect.left + 5,Rect.top + 2, Text );
	}
}
//---------------------------------------------------------------------------
