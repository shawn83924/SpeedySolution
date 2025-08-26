//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "SortListForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TSortForm::TSortForm(TComponent* Owner)
	: TForm(Owner)
{
	ListBox1->MultiSelect = false;
}
//---------------------------------------------------------------------------
__fastcall TSortForm::~TSortForm( void )
{
	for( register int i = 0; i < ListBox1->Count; i++ )
	{
		AnsiString DisStr = ListBox1->Items->operator [](i);
		UFC::AnsiString* Symbol = FSymbolList.GetObjectByKey( DisStr.c_str() );
		delete Symbol;
	}
	FSymbolList.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TSortForm::AddSymbols( String& Symbol, String& DisplayName )
{
	AnsiString Sym = Symbol;
	AnsiString Dsp = DisplayName;

	ListBox1->AddItem( DisplayName, NULL );
	FSymbolList.Add( Dsp.c_str(), new UFC::AnsiString( Sym.c_str() ) );
}
//---------------------------------------------------------------------------
void __fastcall TSortForm::Button1Click(TObject *Sender)
{
	int Index = ListBox1->ItemIndex;
	if( Index > 0 && Index < ListBox1->Count )
	{
		ListBox1->Items->Move( Index, Index -1 );
		ListBox1->ItemIndex = Index - 1;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSortForm::Button2Click(TObject *Sender)
{
	int Index = ListBox1->ItemIndex;
	if( Index >= 0 && Index < ListBox1->Count - 1 )
	{
		ListBox1->Items->Move( Index, Index + 1 );
		ListBox1->ItemIndex = Index + 1;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSortForm::Button3Click(TObject *Sender)
{
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TSortForm::Button4Click(TObject *Sender)
{
	ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TSortForm::GetSymbolOrder( UFC::PStringList& OrderList )
{
	OrderList.Clear();
	for( register int i = 0; i < ListBox1->Count; i++ )
	{
		AnsiString       DisSymbol = ListBox1->Items->operator [](i);
		UFC::AnsiString* Symbol    = FSymbolList.GetObjectByKey( DisSymbol.c_str() );

		if( Symbol != NULL )
			OrderList.Add( Symbol->c_str() );
	}
}
//---------------------------------------------------------------------------
