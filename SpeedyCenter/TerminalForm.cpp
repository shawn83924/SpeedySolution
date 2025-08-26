//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "TerminalForm.h"
#include "SCStrings.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTermForm *TermForm;
//---------------------------------------------------------------------------
__fastcall TTermForm::TTermForm(TComponent* Owner, bool IsTFX, bool ExtraTerm, bool Use62Carry )
:TForm( Owner )
,FIsTFX( IsTFX )
,FIsTermEx( ExtraTerm )
{
	if( FIsTFX == true )   ///< TAIFEX 2 or 3 digi
	{
		if( FIsTermEx == true )
			Caption = Scstrings_MAIN_TERM_TAIFEX_3DIGI;///"期權櫃號輸入(長度3碼)";
		else
			Caption = Scstrings_MAIN_TERM_TAIFEX_2DIGI;///<"期權櫃號輸入(長度2碼)";
	}
	else ///< TWSE 1 or 2 digi
	{
		if( FIsTermEx == true )
			Caption = Scstrings_MAIN_TERM_TSEC_2DIGI;///"證券櫃號輸入(長度2碼)";
		else
			Caption = Scstrings_MAIN_TERM_TSEC_1DIGI;///<"證券櫃號輸入(長度1碼)";
	}
	Num62CheckBox->Checked = Use62Carry;
}
//---------------------------------------------------------------------------
void __fastcall TTermForm::OKBtnClick(TObject *Sender)
{
	if( FIsTFX == true )
	{
		for( int i = TermMemo->Lines->Count -1; i >= 0; i-- )
		{
			if( FIsTermEx == true )
			{
				if( TermMemo->Lines->Strings[i].Length() < 3 )
					TermMemo->Lines->Delete( i );
				else if( TermMemo->Lines->Strings[i].Length() > 3 )
					TermMemo->Lines->Strings[i].SetLength( 3 );
			}
			else
			{
				if( TermMemo->Lines->Strings[i].Length() < 2 )
					TermMemo->Lines->Delete( i );
				else if( TermMemo->Lines->Strings[i].Length() > 2 )
					TermMemo->Lines->Strings[i].SetLength( 2 );
			}
		}
	}
	else
	{
		for( int i = TermMemo->Lines->Count -1; i >= 0; i-- )
		{
			if( FIsTermEx == true )
			{
				if( TermMemo->Lines->Strings[i].Length() < 2 )
					TermMemo->Lines->Delete( i );
				else if( TermMemo->Lines->Strings[i].Length() > 2 )
					TermMemo->Lines->Strings[i].SetLength( 2 );
			}
			else
			{
				if( TermMemo->Lines->Strings[i].Length() < 1 )
					TermMemo->Lines->Delete( i );
				else if( TermMemo->Lines->Strings[i].Length() > 1 )
					TermMemo->Lines->Strings[i].SetLength( 1 );
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TTermForm::TermMemoChange(TObject *Sender)
{
	int OrderCount;
	String Msg;
	if( FIsTFX == true )
	{
		if( Num62CheckBox->Checked == false)
		{
			if( FIsTermEx == true )
				OrderCount = 99*TermMemo->Lines->Count;
			else
				OrderCount = 999*TermMemo->Lines->Count;
		}
		else
		{
			if( FIsTermEx == true )
				OrderCount = 62*62*TermMemo->Lines->Count;
			else
				OrderCount = 62*62*62*TermMemo->Lines->Count;
		}
	}
	else
	{
		if( Num62CheckBox->Checked == false)
		{
			if( FIsTermEx == true )
				OrderCount = 999*TermMemo->Lines->Count;
			else
				OrderCount = 5999*TermMemo->Lines->Count;
		}
		else
		{
			if( FIsTermEx == true )
				OrderCount = 62*62*62*TermMemo->Lines->Count;
			else
				OrderCount = 62*62*62*62*TermMemo->Lines->Count;
		}
	}
	Msg.printf( Scstrings_MAIN_TERM_TERM_AND_ORDERID_COUNT.c_str(),TermMemo->Lines->Count,OrderCount );
	StaticText->Caption = Msg;
}
//---------------------------------------------------------------------------
void __fastcall TTermForm::TermMemoKeyPress(TObject *Sender, wchar_t &Key)
{
	if( (Key >= 'a' && Key <= 'z') ||
		(Key >= 'A' && Key <= 'Z') ||
		(Key >= '0' && Key <= '9') ||
		(Key >= VK_NUMPAD0 && Key <= VK_NUMPAD9 ) ||
		 Key == VK_RETURN || Key == VK_BACK || Key == VK_UP ||
		 Key == VK_DOWN   || Key == VK_LEFT || Key == VK_RIGHT ||
		 Key == VK_CONTROL )
		return;
	Key = VK_SHIFT;
}
//---------------------------------------------------------------------------

void __fastcall TTermForm::Num62CheckBoxClick(TObject *Sender)
{
	 TermMemoChange( NULL );
}
//---------------------------------------------------------------------------

