//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ThroughputLimit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cspin"
#pragma resource "*.dfm"
TThroughputForm *ThroughputForm;
//---------------------------------------------------------------------------
__fastcall TThroughputForm::TThroughputForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void TThroughputForm::Load( MTree *Tree )
{
   UFC::AnsiString Account,Throughput,Name;
   int DefTP,TP,Count = 0;

   Tree->get( "Default", DefTP );
   Tree->get( "Count", Count );
   DefThroughputSpinEdit->Value = DefTP;
   ThroughputSpinEdit->Value = DefTP;
   AccountListView->Clear();
   NewItemEdit->Text = "";
   for( int i = 0; i < Count; i++ )
   {
	   Name.Printf( "ID%d", i+1 );
	   Tree->get( Name, Account );
	   Name.Printf( "TP%d", i+1 );
	   Tree->get( Name, TP );
	   TListItem* NewItem = AccountListView->Items->Add();

	   NewItem->Caption = Account.c_str();
	   NewItem->SubItems->Add( String(TP) );
   }
}
//---------------------------------------------------------------------------
void TThroughputForm::Save( MTree *Tree )
{
	UFC::AnsiString Name;
	int Count = 0;

	TListItemsEnumerator* Enumerator = AccountListView->Items->GetEnumerator();
	while( Enumerator->MoveNext() )
	{
		TListItem* SelItem = Enumerator->Current;
		if( SelItem != NULL)
		{
			AnsiString Account = SelItem->Caption;

			Count++;
			Name.Printf( "ID%d", Count );
			Tree->append( Name, Account.c_str() );
			Name.Printf( "TP%d", Count );
			Tree->append( Name, SelItem->SubItems->Strings[0].ToInt() );
		}
	}
	Tree->append( "Default", (int)DefThroughputSpinEdit->Value );
	Tree->append( "Count", Count );
}
//---------------------------------------------------------------------------
void __fastcall TThroughputForm::AccountListViewClick(TObject *Sender)
{
	TListItem* SelItem  = AccountListView->Selected;
	if( SelItem != NULL)
	{
		NewItemEdit->Text = SelItem->Caption;
		ThroughputSpinEdit->Value = SelItem->SubItems->Strings[0].ToInt();
	}
}
//---------------------------------------------------------------------------
void __fastcall TThroughputForm::AddBtnClick(TObject *Sender)
{
	TListItem* NewItem = AccountListView->FindCaption(0, NewItemEdit->Text, false, true, true );
	if( NewItem == NULL )
	{
		NewItem = AccountListView->Items->Add();
		NewItem->Caption = NewItemEdit->Text;
		NewItem->SubItems->Add( String(ThroughputSpinEdit->Value) );
	}
	else
		MessageDlg( L"["+ NewItemEdit->Text +L"]已存在", mtWarning, TMsgDlgButtons() << mbOK , 0 );
}
//---------------------------------------------------------------------------
void __fastcall TThroughputForm::DelBtnClick(TObject *Sender)
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
void __fastcall TThroughputForm::ThroughputSpinEditChange(TObject *Sender)
{
	TListItem* ModifyItem = AccountListView->FindCaption(0, NewItemEdit->Text, false, true, true );
	if( ModifyItem != NULL )
		ModifyItem->SubItems->Strings[0] = String(ThroughputSpinEdit->Value);
}
//---------------------------------------------------------------------------
