//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UsersForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUsersList *UsersList;
//---------------------------------------------------------------------------
__fastcall TUsersList::TUsersList(TComponent* Owner, TStringList* Users)
	: TForm(Owner)
{
	PopulateUsersList(Users);
}
//---------------------------------------------------------------------------
__fastcall TUsersList::~TUsersList()
{
}
//---------------------------------------------------------------------------
void __fastcall TUsersList::PopulateUsersList( TStringList* Users )
{
	UsersListView->Items->Clear();
	UsersListView->Items->BeginUpdate();

	for( int i = 0; i < Users->Count; i++ )
	{
		TListItem* Item = UsersListView->Items->Add();
		Item->Caption = Users->Strings[i];
		Item->Checked = ( Users->Objects[i] != NULL );
	}
	UsersListView->Items->EndUpdate();
}
//---------------------------------------------------------------------------
AnsiString __fastcall TUsersList::GetUsers(void)
{
	TStringList* UserList = new TStringList();
	UserList->Delimiter       = ',';
	UserList->StrictDelimiter = true;

	for( int i = 0; i < UsersListView->Items->Count; i++ )
	{
		TListItem* Item = UsersListView->Items->Item[i];
		if( Item->Checked )
			UserList->Add( Item->Caption );
	}

	AnsiString Users = UserList->DelimitedText;
	delete UserList;
	return Users;
}
//---------------------------------------------------------------------------
