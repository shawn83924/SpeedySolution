//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ItemListForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TItemsForm *ItemsForm;
//---------------------------------------------------------------------------
__fastcall TItemsForm::TItemsForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
TModalResult TItemsForm::ShowDialog( const String& Txt, TStringList* Items )
{
	TModalResult Result;

	Caption = Txt;
	NewItemEdit->Text = L"";
	ItemListBox->Items->Clear();
	ItemListBox->Items->AddStrings( Items );
	Result = ShowModal();
	if( Result == mrOk )
	{
		Items->Clear();
		Items->AddStrings( ItemListBox->Items );
	}
	return Result;
}
//---------------------------------------------------------------------------
void __fastcall TItemsForm::DelBtnClick(TObject *Sender)
{
	int Sel = ItemListBox->ItemIndex;
	if( Sel != -1 )
	{
		if( MessageDlg( L"確定要刪除[" + ItemListBox->Items->Strings[Sel] + L"]?", mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0 ) == mrOk )
			ItemListBox->Items->Delete( ItemListBox->ItemIndex );
	}
	else
		MessageDlg( L"請選擇欲刪除的欄位", mtWarning, TMsgDlgButtons() << mbOK , 0 );
}
//---------------------------------------------------------------------------
void __fastcall TItemsForm::AddBtnClick(TObject *Sender)
{
	if( NewItemEdit->Text.Length() > 0 )
	{
		if( ItemListBox->Items->IndexOf( NewItemEdit->Text ) == -1 )
			ItemListBox->Items->Add( NewItemEdit ->Text );
		else
			MessageDlg( L"["+ NewItemEdit->Text +L"]已存在", mtWarning, TMsgDlgButtons() << mbOK , 0 );
	}
	else
		MessageDlg( L"新增欄位不能為空白", mtWarning, TMsgDlgButtons() << mbOK , 0 );
}
//---------------------------------------------------------------------------
void __fastcall TItemsForm::ItemListBoxClick(TObject *Sender)
{
	int Sel = ItemListBox->ItemIndex;

	if( Sel != -1 )
		NewItemEdit->Text = ItemListBox->Items->Strings[Sel];
}
//---------------------------------------------------------------------------


