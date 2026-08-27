//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UsersForm.h"
#include "SimTFXMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUsersList *UsersList;
//---------------------------------------------------------------------------
__fastcall TUsersList::TUsersList(TComponent* Owner,TSimTFXForm* SimTFXForm, const AnsiString& users)
	: TForm(Owner)
{
	ReadUsers(SimTFXForm);
	SetUsersList(users);
}
//---------------------------------------------------------------------------
__fastcall TUsersList::~TUsersList()
{
}
//---------------------------------------------------------------------------
void __fastcall TUsersList::ReadUsers(TSimTFXForm* SimTFXForm )
{
	UsersListView->Items->Clear();
	UsersListView->Items->BeginUpdate();

	AnsiString       ConfigFileName = SimTFXForm->GetUserConfigFileName();
	UFC::UiniFile    ini( ConfigFileName.c_str() );
	UFC::Section*    iniSection;

	int count = ini.SectionCount();
	for( int i = 0; i < count; i++ )
	{
		iniSection = ini.GetSection( i );
		if( iniSection->GetSectionName() == "Speedy" || iniSection->GetSectionName() == "SpeedyOffHour" )
			continue;
		UsersListView->Items->Add()->Caption = iniSection->GetSectionName().c_str();
	}
	UsersListView->Items->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TUsersList::SetUsersList( const AnsiString& users )
{
	TStringList* UserList = new TStringList();
	UserList->Delimiter       = ',';
	UserList->StrictDelimiter = true;
	UserList->DelimitedText   = users;

	for( int i = 0; i < UsersListView->Items->Count; i++ )
	{
		TListItem* Item = UsersListView->Items->Item[i];
		Item->Checked = ( UserList->IndexOf( Item->Caption ) >= 0 );
	}
	delete UserList;
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
