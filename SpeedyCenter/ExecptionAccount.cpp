//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ExecptionAccount.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TExceptionAccForm *ExceptionAccForm;
//---------------------------------------------------------------------------
__fastcall TExceptionAccForm::TExceptionAccForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
String TExceptionAccForm::ToUnicodeString( const UFC::AnsiString& str )
{
	if( str == "Rule1" )
		return L"規則1";
	else if( str == "Rule2" )
		return L"規則2";
	else if( str == "Rule3" )
		return L"規則3";
	else if( str == "Rule4" )
		return L"規則4";
	else if( str == "Rule5" )
		return L"規則5";
	else
		return String( str.c_str() );
}
//---------------------------------------------------------------------------
const char* TExceptionAccForm::ToCString( const String& str )
{
	if( str == L"規則1" )
		return "Rule1";
	else if( str == "規則2" )
		return "Rule2";
	else if( str == L"規則3" )
		return "Rule3";
	else if( str == L"規則4" )
		return "Rule4";
	else if( str == L"規則5" )
		return "Rule5";
	else
		return "Rule1";
}
//---------------------------------------------------------------------------
void TExceptionAccForm::Init( UFC::UiniFile* ini )
{
	UFC::AnsiString Acc,Rule;
	UFC::Section* AccSec = ini->GetSection( "Account" );

	AccountListView->Clear();
	if( AccSec != NULL)
	{
		for( int i=0; i< AccSec->ItemCount(); i++ )
		{
		   AccSec->GetNameValue(i,Acc,Rule);
		   if( Acc != "Default" )
		   {
				TListItem* NewItem = AccountListView->Items->Add();

				NewItem->Caption = Acc.c_str();
				NewItem->SubItems->Add( ToUnicodeString( Rule ) );
		   }
		}
	}
	NewItemEdit->Text = L"";
	RuleComboBox->ItemIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TExceptionAccForm::AddBtnClick(TObject *Sender)
{
	TListItem* NewItem = AccountListView->FindCaption(0, NewItemEdit->Text, false, true, true );
	if( NewItem == NULL )
	{
		NewItem = AccountListView->Items->Add();
		NewItem->Caption = NewItemEdit->Text;
		NewItem->SubItems->Add( RuleComboBox->Text );
	}
	else
		MessageDlg( L"["+ NewItemEdit->Text +L"]已存在", mtWarning, TMsgDlgButtons() << mbOK , 0 );
}
//---------------------------------------------------------------------------
void __fastcall TExceptionAccForm::DelBtnClick(TObject *Sender)
{
	TListItem* SelItem  = AccountListView->Selected;

	if( SelItem != NULL )
	{
		if( MessageDlg( L"確定要刪除[" + SelItem->Caption + L"]?", mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0 ) == mrOk )
			AccountListView->Items->Delete( SelItem->Index );
	}
	else
		MessageDlg( L"請選擇欲刪除的欄位", mtWarning, TMsgDlgButtons() << mbOK , 0 );
}
//---------------------------------------------------------------------------
void __fastcall TExceptionAccForm::AccountListViewClick(TObject *Sender)
{
	TListItem* SelItem  = AccountListView->Selected;
	if( SelItem != NULL)
	{

		NewItemEdit->Text = SelItem->Caption;
		int index = RuleComboBox->Items->IndexOf( SelItem->SubItems->Strings[0] );
		if( index == -1 )
			RuleComboBox->ItemIndex = 0;
		else
			RuleComboBox->ItemIndex = index;
	}
}
//---------------------------------------------------------------------------
void __fastcall TExceptionAccForm::GetItems( UFC::UiniFile* ini )
{
	UFC::AnsiString DefRule( "Rule1" );
	TListItemsEnumerator* Enumerator = AccountListView->Items->GetEnumerator();

	ini->GetValue( "Account", "Default", DefRule );
	ini->Clear();
	while( Enumerator->MoveNext() )
	{
		TListItem* SelItem = Enumerator->Current;
		if( SelItem != NULL)
		{
			AnsiString Account = SelItem->Caption;
			UFC::AnsiString Rule( ToCString(SelItem->SubItems->Strings[0]) );
			ini->SetValue( "Account", Account.c_str(), Rule );
		}
	}
	ini->SetValue( "Account", "Default", DefRule );
}
//---------------------------------------------------------------------------
