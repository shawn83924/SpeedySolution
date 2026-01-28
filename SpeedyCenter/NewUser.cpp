//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "NewUser.h"
#include "SCStrings.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TNewUserForm *NewUserForm;
//---------------------------------------------------------------------------
__fastcall TNewUserForm::TNewUserForm()
:TForm((TComponent*)NULL)
,CheckPassword( true )
{
	Caption = Scstrings_MAIN_USER_ADD_NEW;///"新增使用者";
	StateComboBox->Enabled = false;
}
//---------------------------------------------------------------------------
__fastcall TNewUserForm::TNewUserForm( TListItem* EditItem, bool ModifyAttr )
:TForm((TComponent*)NULL)
{
	if( ModifyAttr )
	{
		CheckPassword = false;
		Caption = Scstrings_MAIN_USER_MODIFY;///"修改使用者資料";
		IDEdit->Text = EditItem->Caption;
		IDEdit->Enabled = false;
		PwdEdit->Text = "1234rewq";
		PwdConfirmEdit->Text = "1234rewq";
		PwdPanel->Visible = false;
		GroupComboBox->ItemIndex = EditItem->SubItems->Strings[1].ToInt();
		StateComboBox->ItemIndex = EditItem->SubItems->Strings[2].ToInt();
	}
	else
	{
		CheckPassword = true;
		Caption = Scstrings_MAIN_USER_MODIFY_PASSWORD;///"修改密碼";
		IDEdit->Text = EditItem->Caption;
		IDEdit->Enabled = false;
		PwdEdit->Text = "";
		PwdConfirmEdit->Text = "";
		AttrPanel->Visible = false;
		GroupComboBox->ItemIndex = EditItem->SubItems->Strings[1].ToInt();
		StateComboBox->ItemIndex = EditItem->SubItems->Strings[2].ToInt();
	}
}
//---------------------------------------------------------------------------
__fastcall TNewUserForm::TNewUserForm( const AnsiString& ID )
:TForm((TComponent*)NULL)
,CheckPassword( true )
{
	Caption = Scstrings_MAIN_USER_MODIFY_PASSWORD;///"修改密碼";
	IDEdit->Text = ID;
	IDEdit->Enabled = false;
	PwdEdit->Text = "";
	PwdConfirmEdit->Text = "";
	AttrPanel->Visible = false;
	GroupComboBox->ItemIndex = 0;
	StateComboBox->ItemIndex = 0;
}
//---------------------------------------------------------------------------
bool __fastcall TNewUserForm::CharDup( AnsiString pwd )
{
	for( int i = 0; i < pwd.Length(); i++ )
    {
        char chkchar = pwd[ i + 1 ];
		for( int j = 0; j < pwd.Length(); j++ )
        {
            if( ( pwd[ j + 1 ] == chkchar ) && ( j != i ) )
                return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool __fastcall TNewUserForm::NumAndEn( AnsiString pwd )
{
	int NumCount = 0;
	int PwdLen = pwd.Length();
	for( int i = 0; i < PwdLen; i++ )
	{
		char chkchar = pwd[ i+1 ];
		if( chkchar > '0' && chkchar < '9' )
			NumCount++;
	}
	if( NumCount == 0 || NumCount == PwdLen )
		return false;
	return true;
}
//---------------------------------------------------------------------------
bool __fastcall TNewUserForm::EasyPassword( AnsiString pwd )
{
	for( int i = 1;i <= pwd.Length() - 4; i++ )
	{
		if( pwd[i] == pwd[i+1] && pwd[i+1] == pwd[i+2] &&  pwd[i+2] == pwd[i+3] )
			return true; ///< Example: 1111
		if( pwd[i] == pwd[i+1]-1 && pwd[i+1] == pwd[i+2]-1 &&  pwd[i+2] == pwd[i+3]-1 )
			return true;///< Example: 1234 ,abcd
		if( pwd[i] == pwd[i+1]+1 && pwd[i+1] == pwd[i+2]+1 &&  pwd[i+2] == pwd[i+3]+1 )
			return true;///< Example: 4321, dcba
	}
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TNewUserForm::FormShow(TObject *Sender)
{
    ClientHeight = BtnPanel->Top + BtnPanel->Height;
}
//---------------------------------------------------------------------------
void __fastcall TNewUserForm::OKBtnClick(TObject *Sender)
{
	if( IDEdit->Text.Length() < 5 )
	{
		ShowMessage( Scstrings_MAIN_USER_ID_AT_LEAST_LEN_5 );
		return;
	}
	if( CheckPassword == true )
	{
		if( IDEdit->Text.Compare( PwdEdit->Text ) == 0 )
		{
			ShowMessage( Scstrings_MAIN_USER_PASSWORD_CANT_SAME_AS_ID );///"密碼不得與帳號相同.");
			return;
		}
		if( IDEdit->Text.Pos( PwdEdit->Text ) != 0 )
		{
			ShowMessage( "密碼不得包含帳號." ); ///"密碼不得包含帳號.");
			return;
		}

		if( PwdEdit->Text.Length() < 6 )
		{
			ShowMessage( Scstrings_MAIN_USER_PASSWORD_AT_LEAST_LEN_6 );//"密碼長度至少要六碼.");
			return;
		}
		if( !NumAndEn( PwdEdit->Text ) )
		{
			ShowMessage( Scstrings_MAIN_USER_PASSWORD_MUST_CONTAINS_NUM_CHAR );///"密碼中的字元必須要包含英數字.");
			return;
		}
		if( EasyPassword( PwdEdit->Text) )
		{
			ShowMessage( Scstrings_MAIN_USER_CANT_USE_WEAK_PASSWORD );///"不可使用易猜的密碼,如1234,6543,abcd,3333等.");
			return;
		}
		if( PwdEdit->Text.Compare( PwdConfirmEdit->Text ) !=0 )
		{
			ShowMessage( Scstrings_MAIN_USER_PASSWORD_CONFIRM_NOT_EQUAL );///"密碼欄位與確認密碼欄位不同.");
			return;
		}
	}
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------

